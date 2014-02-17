#include "StdAfx.h"
#include "DivideLine.h"

CDivideLine g_cDivideLine;

CDivideLine::CDivideLine(void)
{
	m_nReturnLen	= 2048;
	m_nMaxChar		= 2048;
	m_nMaxLine		= 6;
	m_nIgnoreLine	= 0;

	m_strReturnChar	= "\n";
	m_nReturnCharLen= 1;

	m_bRemoveSpace	= FALSE;
	m_bTwoByte		= FALSE;
}

CDivideLine::~CDivideLine(void)
{
}

void CDivideLine::SetBase(int nIgnoreLine, int nMaxChar, int nMaxLine,
					   int nReturnLen, string strReturnChar)
{
	m_nIgnoreLine	= nIgnoreLine;
	m_nMaxChar		= nMaxChar;
	m_nMaxLine		= nMaxLine;
	m_nReturnLen	= nReturnLen;

	m_strReturnChar	= strReturnChar;
	m_nReturnCharLen= (int)m_strReturnChar.length();
}

int CDivideLine::SplitLines(vector <string>* NewLines,vector <string> Lines)
{
	//FixLine에서 그대로 베껴옴
	//함수처리 부분만 살짝 변경됨

	NewLines->clear();
	int NewLineN=0;

	int IgnoreModeFuncN=0;
	int FuncN=0;
	int LineN=(int)Lines.size();
	//옮기자!
	for(int i=0;i<LineN;i++)
	{
		string::size_type SPoint=0;
		string::size_type Divider=0;
		string::size_type OldDivider=0;
		int CharN=0;
		while(1)
		{
			OldDivider=Divider;

			if(Divider>=Lines[i].length())
			{
				NewLines->push_back(Lines[i].substr(SPoint,Lines[i].length()-SPoint));
				NewLineN++;
				break;
			}
			//줄읽기

			if(Divider<Lines[i].length()-6&&
				//_*&n,Len&*_
				Lines[i][Divider]=='_'&&
				Lines[i][Divider+1]=='*'&&
				Lines[i][Divider+2]=='&')
			{
				//함수모드 On
				//좀 무식하긴 하지만 [....]

				string::size_type DividerEnd=Lines[i].find("&*_",Divider+3);

				string FuncNTemp=Lines[i].substr(Divider+3,DividerEnd-Divider-3);
				
				//DivideLine에서 추가된 부분
				string::size_type nTab=FuncNTemp.find(',');
				string strFuncNLen=FuncNTemp.substr(nTab+1,FuncNTemp.length()-nTab-1);
				int nFuncLen=atoi(strFuncNLen.c_str());

				Divider=DividerEnd+3;
				CharN+=nFuncLen;
			}
			else if(IsDBCSLeadByteEx(949,Lines[i][Divider])){
				//전각
				Divider+=2;
				CharN+=2;
			}
			else
			{
				//반각
				Divider+=1;
				CharN+=1;
			}

			if(CharN>m_nMaxChar)
			{
				NewLines->push_back(Lines[i].substr(SPoint,OldDivider-SPoint));
				SPoint=OldDivider;
				NewLineN++;

				CharN=0;

				if(NewLineN<=m_nIgnoreLine)
				{
					//무시라인이면, 더이상 입력하지 않고 자름
					break;
				}
			}
		}//while종료
	}//for 종료
	return NewLineN;
}

void CDivideLine::PreDivideLine(LPSTR szDest, LPCSTR szSource)
{
	string strSource=szSource;
	string strDest="";

	//줄처리
	//꽉 찬 문장이면 개행문자 없이 이어 정상적인 번역이 되도록 한다.
	
	//FixLine에서 퍼왔다.

	//벡터에 줄로 나눈다
	vector <string> Lines;
	//개행문자로 쪼개 Lines에 담는다.
	string::size_type SPoint=0;
	string::size_type Divider=0;
	int LineN=0;
	while(1)
	{

		LineN++;

		Divider=strSource.find(m_strReturnChar,SPoint);
		if(Divider==strSource.npos)
		{
			//더이상 없으면
			Lines.push_back(strSource.substr(SPoint,strSource.length()-SPoint));
			break;
		}
		else
		{
			//개행문자가 있으면
			Lines.push_back(strSource.substr(SPoint,Divider-SPoint));
			SPoint=Divider+m_nReturnCharLen;
		}
	}
	
	//길이를 체크해서 긴경우에는 뒤의 것과 이어서 출력한다.
	//FixLine 그대로 퍼왔다.
	for(int i=0;i<LineN;i++)
	{
		bool AddReturnChar=false;
		strDest+=Lines[i];
		//일단 추가하고
		if(i<m_nIgnoreLine)
		{
			//무시라인이면
			if(i<LineN-1)
			{
				AddReturnChar=true;
			}
		}
		else
		{
			//정상라인이라면 길이체크
			string::size_type thisLen=0;
			string::size_type SPoint=0;
			string::size_type Divider=0;
			while(1)
			{
				//함수를 찾아서 길이를 대충 더한다
				Divider = Lines[i].find("_*&",SPoint);
				if(Divider==Lines[i].npos)
				{
					//더이상 함수가 없다면!
					thisLen+=Lines[i].length()-SPoint;
					break;
				}
				else
				{
					//함수가 있다면
					string::size_type DividerEnd=Lines[i].find("&*_",Divider+3);

					string FuncNTemp=Lines[i].substr(Divider+3,DividerEnd-Divider-3);


					//DivideLine에서 추가된 부분
					string::size_type nTab=FuncNTemp.find(',');
					string strFuncNLen=FuncNTemp.substr(nTab+1,FuncNTemp.length()-nTab-1);
					int nFuncLen=atoi(strFuncNLen.c_str());

					thisLen+=Divider-SPoint;
					thisLen+=nFuncLen;

					SPoint=DividerEnd+3;
				}
			}

			//길이체크!
			if((int)thisLen<m_nReturnLen&&i<LineN-1)
			{
				//꽉차지 않았다면 엔터추가
				AddReturnChar=true;
			}
		}
		
		if(AddReturnChar)
		{
			//개행기호 추가가 설정되었다면
			strDest+='\n';
		}
	}
	
}

void CDivideLine::PostDivideLine(LPSTR szDest, LPCSTR szSource)
{
	//FixLine에서 일부분만 베껴옴
	string strSource=szSource;


	string strStep1;
	if(m_bRemoveSpace==FALSE&&m_bTwoByte==FALSE)
	{
		//옵션 아무것도 안켜면 이 작업 할 필요가 없다 =ㅅ=;
		strStep1=strSource;
	}
	else
	{
		string::size_type Len=strSource.length();
		string::size_type SPoint=0;
		string::size_type Divider=0;
		while(SPoint<Len)
		{
			Divider=strSource.find("_*&",SPoint);
			if(Divider==strSource.npos)
			{
				Divider=Len;
			}

			for(string::size_type i=SPoint;i<Divider;i++)
			{
				if(strSource[i]==' '&&m_bRemoveSpace!=FALSE)
				{
					//아무것도 하지 않는다.
				}
				else if(IsDBCSLeadByteEx(949,strSource[i]))
				{
					//전각
					strStep1+=strSource[i];
					i++;
					strStep1+=strSource[i];
				}
				else if(m_bTwoByte!=FALSE)
				{
					strStep1+=(char)0xA3;
					strStep1+=strSource[i]+(char)0x80;
				}
				else
				{
					strStep1+=strSource[i];
				}
			}

			if(Divider!=Len)
			{
				string::size_type TempDivider=Divider;
				Divider=strSource.find("&*_",TempDivider);

				strStep1+=strSource.substr(TempDivider,Divider-TempDivider+3);

				SPoint = Divider+3;
			}
			else
			{
				SPoint=Divider;
			}
		}
	}

	//줄처리
	//역시 FixLine에서 가져옴

	//줄쪼개기
	vector <string> Lines;
	int LineN=0;
	bool OverText=false;
	{

		//string strStep1
		string::size_type SPoint=0;
		string::size_type Divider=0;
		while(1)
		{

			LineN++;

			Divider=strStep1.find('\n',SPoint);
			if(Divider==strStep1.npos)
			{
				//더이상 없으면
				Lines.push_back(strStep1.substr(SPoint,strStep1.length()-SPoint));
				break;
			}
			else
			{
				//개행문자가 있으면
				Lines.push_back(strStep1.substr(SPoint,Divider-SPoint));
				SPoint=Divider+1;
			}
		}
	}

	//쪼개기
	//역시 FixLine 베껴오기
	vector <string> NewLines;
	if(SplitLines(&NewLines,Lines)>m_nMaxLine)
	{
		//제2모드
		//IgnoreLine은 종전대로 처리하고,
		//띄어쓰기 없이 몽땅 이어붙인다.

		
		vector <string> TempLines;
		for(int j=0;j<m_nIgnoreLine;j++)
		{
			TempLines.push_back(Lines[j]);
		}
		string Mode2Source="";
		for(int j=m_nIgnoreLine;j<LineN;j++)
		{
			Mode2Source+=Lines[j];
		}
		TempLines.push_back(Mode2Source);
		//싹 이어붙이고

		//한번 더 소환 =ㅅ=;
		SplitLines(&NewLines,TempLines);
	}

	string strDest="";
	int i=0;
	for(i=0;i<(int)NewLines.size()-1&&i<m_nMaxLine-1;i++)
	{
		strDest+=NewLines[i];
		strDest+=m_strReturnChar;
	}
	strDest+=NewLines[i];

	lstrcpyA(szDest,strDest.c_str());
}