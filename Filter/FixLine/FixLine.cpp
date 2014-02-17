#include "StdAfx.h"
#include "FixLine.h"
#include "Migration.h"

const char szDiv[]="_-/_";
const int nDivLen=strlen(szDiv);

CFixLine::CFixLine(void)
{
	m_cOption.SetOptionFile(L"ATData\\FixLine.dat");
	Initialize();
}

CFixLine::~CFixLine(void)
{
}

void CFixLine::Clear()
{
	while(!m_FuncQueue.empty())
	{
		m_FuncQueue.pop();
	}
	m_FuncPool.clear();
	m_NamePool.clear();
	m_strRetChar.clear();
	m_nMaxChar=2048;
	m_nMaxLine=12;
	m_bUseLimitLine=false;
	m_bUseRetChar=false;
	m_bUseRet=false;

	m_ElementPool.clear();
	m_nLineN=0;
	
	m_nMaxLen=-2;
	m_bForceLen=false;

}

void CFixLine::Initialize()
{
	Clear();

	m_cOption.Clear();
	m_cOption.OpenOptionFile();

	if(m_cOption.GetOptionString(L"_Format").compare(L"")==0)
	{
		CMigration::Migration1to2(L"ATData\\FixLine.dat");
		m_cOption.Clear();
		m_cOption.OpenOptionFile();
	}
	else if(m_cOption.GetOptionString(L"_Format").compare(L"V2")==0){
		CMigration::Migration2to2_1(L"ATData\\FixLine.dat");
		m_cOption.Clear();
		m_cOption.OpenOptionFile();
	}

	bool bError=false;
	wstring strError=L"";

	if(m_cOption.IsInData(L"strRetChar"))
	{
		m_strRetChar = DecodeLine(wtoa(m_cOption.GetOptionString(L"strRetChar")));
	}
	else
	{
		m_strRetChar = "\n";
	}
	m_strRetChar = DecodeLine(wtoa(m_cOption.GetOptionString(L"strRetChar")));
	if(m_cOption.IsInData(L"MaxChar"))
	{
		m_nMaxChar = m_cOption.GetOptionInt(L"MaxChar");
	}
	else
	{
		m_nMaxChar = 40;
	}
	if(m_cOption.IsInData(L"MaxLine"))
	{
		m_nMaxLine = m_cOption.GetOptionInt(L"MaxLine");
	}
	else
	{
		m_nMaxLine = 3;
	}
	m_bUseLimitLine = m_cOption.GetOptionBool(L"UseLimitLine");
	m_bUseRetChar = m_cOption.GetOptionBool(L"UseRetChar");
	m_bUseRet = m_cOption.GetOptionBool(L"UseRet");

	m_nMaxLen = m_cOption.GetOptionInt(L"MaxLen");
	m_bForceLen = m_cOption.GetOptionBool(L"ForceLen");


	//마무리.
	if(m_strRetChar.length() ==0)
	{
		m_strRetChar = "\n";
	}
	if(0==m_nMaxChar)
	{
		m_nMaxChar=50;
	}
	if(0==m_nMaxLine)
	{
		m_nMaxLine=3;
	}

	if(m_nMaxLen==-2)
	{
		m_nSetMaxLen=2048;
	}

	if(m_bUseRet)
	{
		//개행 처리에 완벽히 쓰레기값
		m_nMaxChar=2048;
		m_nMaxLine=10;
		m_strRetChar = "spijdfgoiefjgofwiejsafgoidrgjfoieasjdghbidjdfscorhdgofhdewosgfhfnodtirejdoiqwharefgifwehjdgudhtngfijhweisdgsrudhfwieerterwspijdfgoiefjgofwiejsafgoidrgjfoieasjdghbidjdfscorhdgofhdewosgfhfnodtirejdoiqwharefgifwehjdgudhtngfijhweisdgsrudhfwieerterwspijdfgoiefjgofwiejsafgoidrgjfoieasjdghbidjdfscorhdgofhdewosgfhfnodtirejdoiqwharefgifwehjdgudhtngfijhweisdgsrudhfwieerterwspijdfgoiefjgofwiejsafgoidrgjfoieasjdghbidjdfscorhdgofhdewosgfhfnodtirejdoiqwharefgifwehjdgudhtngfijhweisdgsrudhfwieerterwspijdfgoiefjgofwiejsafgoidrgjfoieasjdghbidjdfscorhdgofhdewosgfhfnodtirejdoiqwharefgifwehjdgudhtngfijhweisdgsrudhfwieerterw";
		m_bUseRetChar = false;
	}

	int nFuncN=0;

	while(m_cOption.IsInData(L"ElemType",1,nFuncN))
	{
		FLString FLTemp;
		FLTemp.SetType(m_cOption.GetOptionInt(L"ElemType",1,nFuncN));
		FLTemp.SetTrim(m_cOption.GetOptionBool(L"ElemTrim",1,nFuncN));
		FLTemp.SetFunc(true);

		int nSubN=0;
		while(m_cOption.IsInData(L"ElemSubFunc",2,nFuncN,nSubN))
		{
			int nMax=-1;
			int nMin=-1;
			if(m_cOption.IsInData(L"ElemSubMin",2,nFuncN,nSubN)) nMin=m_cOption.GetOptionInt(L"ElemSubMin",2,nFuncN,nSubN);
			if(m_cOption.IsInData(L"ElemSubMax",2,nFuncN,nSubN)) nMax=m_cOption.GetOptionInt(L"ElemSubMax",2,nFuncN,nSubN);
			FLTemp.PushBack(
				DecodeLine(wtoa(m_cOption.GetOptionString(L"ElemSubString",2,nFuncN,nSubN))),
				m_cOption.GetOptionBool(L"ElemSubFunc",2,nFuncN,nSubN),
				m_cOption.GetOptionBool(L"ElemSubTrans",2,nFuncN,nSubN),
				m_cOption.GetOptionInt(L"ElemSubSize",2,nFuncN,nSubN),
				m_cOption.GetOptionBool(L"ElemSubDel",2,nFuncN,nSubN),
				m_cOption.GetOptionBool(L"ElemSubPass",2,nFuncN,nSubN),
				nMin,
				nMax
			);

			nSubN++;
		}

		m_FuncPool.push_back(FLTemp);

		nFuncN++;
	}
}

void CFixLine::PreFixLine(LPSTR szDest,LPCSTR szSource)
{
	//일단 초기화
	while(!m_FuncQueue.empty())
	{
		m_FuncQueue.pop();
	}
	m_ElementPool.clear();
	m_nLineN=0;
	szDest[0]='\0';
	m_bSetLastRet=false;
	m_nSetMaxLen=0;

	string strStep = szSource;
	if(m_nMaxLen==-2)		m_nSetMaxLen=2048;
	else if(m_nMaxLen==-1)	m_nSetMaxLen=strStep.length();
	else					m_nSetMaxLen=m_nMaxLen;

	/*FixLine 전처리 구성도
	
	1. 함수로 쪼갠다(FindFunc 사용)
	2. 함수의 텍스트만 쪼개서 넣는다.
	3. 재처리한다.
	4. 남은 텍스트에서 줄 기호를 제거
	5. 반환!

	즉, 기존 버전에서 4번 줄 기호를 제거가 선행 되던것이
	뒤로 밀게 되었다.
	*/
	


	//처리 원칙
	//속도가 좀 느려지더라도,
	//보기 쉽게 처리하는것을 원칙으로함 - ... 이걸 써놓고서도 왜 퀄리티가 이따구야.... OTL
	
	

	//1. 원문 줄 갯수 세기
	m_nLineN=1;

	int nHeadidx=0;
	int nTailidx=0;
	
	int i=0;
	int j=0;

	nTailidx = 0;
	nHeadidx = 0;

	//2-2. 함수로 쪼갠다
	
	//일단 모두 찾아둔다.
	vector<vector<vector<int>>> arFunctionList;
	for(i=0;i<m_FuncPool.size();i++)
	{
		vector<vector<int>> FuncList;
		FLString* pFLTmp = &m_FuncPool[i];

		for(j=0;j<pFLTmp->GetSize();j++)
		{
			vector<int> FindList;
			if(pFLTmp->GetSubFunc(j))
			{
				string strTmp=pFLTmp->GetSubString(j);
				int nPos=0;
				while(1)
				{
					nPos=Find(strStep,strTmp,nPos,932);
					if(nPos<0) break;

					FindList.push_back(nPos);
					nPos+=pFLTmp->GetSubString(j).length();
				}
			}
			else
			{
				FindList.push_back(-1);
			}
			FuncList.push_back(FindList);
		}
		arFunctionList.push_back(FuncList);
	}

	int nBefore=0;
	for(i=0;i<strStep.length();)
	{
		int nAnsLen=999999;
		FLString FLAns;
		for(j=0;j<m_FuncPool.size();j++)
		{
			int nTmpLen=0;
			FLString FLTmp=FLString::MakeString(strStep,&nTmpLen,m_FuncPool[j],arFunctionList[j],i);

			if(nTmpLen>0)
			{
				//일단 걸렸네염 -_-ㅋ

				//함수 특성 체크
				//냅다 break;
				if(m_FuncPool[j].GetType()==1)
				{
					//문두
					if(i!=0) continue; //0아니면 넘겨야지
				}
				else if(m_FuncPool[j].GetType()==2)
				{
					//문미
					if(i+nTmpLen!=strStep.length())continue; //맨끝에 안걸치면 넘겨야지
				}
				else if(m_FuncPool[j].GetType()==3)
				{
					//전체
					if(i==0&&nTmpLen!=strStep.length())continue; //시작점, 전체 길이 안 맞으면 넘겨야지
				}
				
				//살아남은 놈들 길이 비교
				if(nTmpLen<nAnsLen)
				{
					nAnsLen = nTmpLen;
					FLAns = FLTmp;
				}
			}
		}
		//다 돌았겠져 -_-?
		if(nAnsLen<999999) //걸렸음?
		{
			if(i-nBefore>0)
			{
				string strTmp=strStep.substr(nBefore,i-nBefore);
				string strTmp2;
				int headIdx=0;
				while(true){
					int tmpIdx=Find(strTmp,m_strRetChar,headIdx,932);
					if(tmpIdx<0){
						strTmp2+=strTmp.substr(headIdx);
						break;
					}
					else{
						m_nLineN++;
						strTmp2+=strTmp.substr(headIdx,tmpIdx-headIdx);
						headIdx=tmpIdx+m_strRetChar.length();
					}
				}
				FLString FLTemp(strTmp2);
				m_ElementPool.push_back(FLTemp);
			}
			m_ElementPool.push_back(FLAns);
			i+=nAnsLen;
			nBefore=i;
			i--;
		}
		i++;
	}
	//이제 남은것 쓸어담기!
	if(i-nBefore>0)
	{
		string strTmp=strStep.substr(nBefore,i-nBefore);
		string strTmp2;
		int headIdx=0;
		while(true){
			int tmpIdx=Find(strTmp,m_strRetChar,headIdx,932);
			if(tmpIdx<0){
				strTmp2+=strTmp.substr(headIdx);
				break;
			}
			else{
				m_nLineN++;
				strTmp2+=strTmp.substr(headIdx,tmpIdx-headIdx);
				headIdx=tmpIdx+m_strRetChar.length();
				if(headIdx==strTmp.length()){
					m_bSetLastRet=true;
					break;
				}
			}
		}
		FLString FLTemp(strTmp2);
		m_ElementPool.push_back(FLTemp);
	}

	//3. 재처리한다.
	
	//재처리 대상은
	//함수 내부에서 텍스트(길이있음)혼자만 삭제되지 않음 선택된경우
	//옆에 있는 텍스트와 합친다.

	i=0;
	while(i<m_ElementPool.size()-1)
	{
		if(!m_ElementPool[i].GetFunc() && !m_ElementPool[i+1].GetFunc())
		{
			//둘다 텍스트인경우
			m_ElementPool[i].SetSubString(0,m_ElementPool[i].GetString()+m_ElementPool[i+1].GetString());
			m_ElementPool.erase(m_ElementPool.begin()+i+1);
		}//둘다
		else if(m_ElementPool[i].GetFunc()==0 && m_ElementPool[i+1].GetFunc()==1)
		{
			//앞은 텍스트 뒤는 함수인경우
			//뒤를 조사한다.

			bool bOnlyText=true;
			for(j=0;j<m_ElementPool[i+1].GetSize();j++)
			{
				//일단 생각좀 해보자
				//m_ElementPool[i+1].GetSubFunc(); : 그러고보니 이제 이건 의미가 없음 -_-; 어차피 번역할건 번역하고 안할건 안함
				if((m_ElementPool[i+1].GetSubSize(j,false)!=-1&&m_ElementPool[i+1].GetSubTrans(j)) || !m_ElementPool[i+1].GetSubTrans(j))
				{
					bOnlyText=false;
					break;
				}
			}

			if(bOnlyText)
			{
				//조건 충족

				//앞에꺼에 합치고, 뒤에껀 지운다.
				m_ElementPool[i].SetSubString(0,m_ElementPool[i].GetString()+m_ElementPool[i+1].GetString());
				m_ElementPool.erase(m_ElementPool.begin()+i+1);
			}
			else
			{
				i++;
			}
		}//앞텍,뒤함
		else if(m_ElementPool[i].GetFunc() && !m_ElementPool[i+1].GetFunc())
		{
			//앞은 함수 뒤는 텍스트인경우
			//앞을 조사한다.

			bool bOnlyText=true;
			for(j=0;j<m_ElementPool[i].GetSize();j++)
			{
				if((m_ElementPool[i].GetSubSize(j,false)!=-1&&m_ElementPool[i].GetSubTrans(j)) || !m_ElementPool[i].GetSubTrans(j))
				{
					bOnlyText=false;
					break;
				}
			}

			if(bOnlyText)
			{
				//조건 충족

				//뒤에꺼에 합치고, 앞에껀 지운다.
				m_ElementPool[i+1].SetSubString(0,m_ElementPool[i].GetString()+m_ElementPool[i+1].GetString());
				m_ElementPool.erase(m_ElementPool.begin()+i);
			}
			else
			{
				i++;
			}
		}//앞함 뒤텍
		else if(m_ElementPool[i].GetFunc()==1 && m_ElementPool[i+1].GetFunc()==1)
		{
			//앞, 뒤 다 함수인경우
			//둘다 조사한후,
			//둘다 조건에 맞으면, 텍스트로 바꿔버린후
			//지운다.

			bool bOnlyText=true;

			//앞에껏
			for(j=0;j<m_ElementPool[i].GetSize();j++)
			{
				if((m_ElementPool[i].GetSubSize(j,false)!=-1&&m_ElementPool[i].GetSubTrans(j)) || !m_ElementPool[i].GetSubTrans(j))
				{
					bOnlyText=false;
					break;
				}
			}

			//뒤에껏
			for(j=0;j<m_ElementPool[i+1].GetSize();j++)
			{
				if((m_ElementPool[i+1].GetSubSize(j,false)!=-1&&m_ElementPool[i+1].GetSubTrans(j)) || !m_ElementPool[i+1].GetSubTrans(j))
				{
					bOnlyText=false;
					break;
				}
			}

			if(bOnlyText)
			{
				FLString FLTemp(m_ElementPool[i].GetString()+m_ElementPool[i+1].GetString());
				m_ElementPool[i]=FLTemp;
				m_ElementPool.erase(m_ElementPool.begin()+i+1);
			}
			else
			{
				i++;
			}
		}//앞뒤함
	}


	//4. 함수의 텍스트만 쪼개서 넣는다.

	bool m_bStart=true;
	int nBufSize=0;
	//번역하고, 길이 있는 텍스트 출력
	for(i=0;i<m_ElementPool.size();i++)
	{
		for(j=0;j<m_ElementPool[i].GetSize();j++)
		{
			if(m_ElementPool[i].GetSubTrans(j) && m_ElementPool[i].GetSubSize(j)!=-2 && !m_ElementPool[i].GetSubDelete(j))
			{
				if(m_bStart)
				{
					nBufSize += sprintf_s(szDest+nBufSize,2048-nBufSize,
						"%s",
						m_ElementPool[i].GetSubString(j).c_str());
					m_bStart = false;
				}
				else
				{
					nBufSize += sprintf_s(szDest+nBufSize,2048-nBufSize,
						"%s%s",
						szDiv,m_ElementPool[i].GetSubString(j).c_str());
				}
				m_FuncQueue.push(i);
				m_FuncQueue.push(j);
			}
		}
	}
	//번역하고, 길이 없는 텍스트 출력
	for(i=0;i<m_ElementPool.size();i++)
	{
		for(j=0;j<m_ElementPool[i].GetSize();j++)
		{
			if(m_ElementPool[i].GetSubTrans(j) && m_ElementPool[i].GetSubSize(j)==-2 && !m_ElementPool[i].GetSubDelete(j))
			{
				if(m_bStart)
				{
					nBufSize += sprintf_s(szDest+nBufSize,2048-nBufSize,
						"%s",
						m_ElementPool[i].GetSubString(j).c_str());
					m_bStart = false;
				}
				else
				{
					nBufSize += sprintf_s(szDest+nBufSize,2048-nBufSize,
						"%s%s",
						szDiv,m_ElementPool[i].GetSubString(j).c_str());
				}
				m_FuncQueue.push(i);
				m_FuncQueue.push(j);
			}
		}
	}
}

void CFixLine::PostFixLine(LPSTR szDest,LPCSTR szSource)
{
	//FixLine 후처리 구성도

	//1. 쪼개서 ElementPool을 갱신
	//2. 줄수 맞춰가며 출력

	szDest[0]='\0';
	string strStep0 = szSource;

	//1. 쪼개서 ElementPool을 갱신	
	int nStep0idx=0;
	bool bStart=true;
	int i=0;
	int j=0;
	while(1)
	{
		if(nStep0idx<0)
			break;

		int nTempidx=0;
		if(bStart)
		{
			nTempidx = Find(strStep0,szDiv,0,949);
		}
		else
		{
			nTempidx = Find(strStep0,szDiv,nStep0idx+nDivLen,949);
		}
		if(nTempidx<0)
			nTempidx=strStep0.length();

		if(nTempidx==0||m_FuncQueue.empty())
			break;

		i = m_FuncQueue.front();
		m_FuncQueue.pop();
		j = m_FuncQueue.front();
		m_FuncQueue.pop();
		string k;
		if(bStart)
		{
			k = strStep0.substr(nStep0idx,nTempidx-nStep0idx);
			bStart=false;
		}
		else
		{
			k = strStep0.substr(nStep0idx+nDivLen,nTempidx-nStep0idx-nDivLen);
		}

		m_ElementPool[i].SetSubString(j,k);

		nStep0idx=nTempidx;

		if(nTempidx==strStep0.length())
			break;
	}

	//2. 줄수 맞춰가며 출력
	int nLineN=0;
	int nChar=0;
	int nBufSize=0;
	int nMaxLine=0;
	i=0;

	if(m_bUseLimitLine) //줄처리
	{
		nMaxLine = m_nLineN;
	}
	else
	{
		nMaxLine = m_nMaxLine;
	}

	int nCurLen=0;
	szDest[0]='\0';
	int nTmpLen=0;
	for(i=0;i<m_ElementPool.size();i++)
	{
		if(m_ElementPool[i].GetFunc())
		{
			//함수인경우 한줄에 전체가 들어가지 않으면, 다음줄로 내린다.
			if(m_ElementPool[i].GetLength() <= m_nMaxChar)
			{
				if(nChar + m_ElementPool[i].GetLength() <= m_nMaxChar && ((m_nSetMaxLen>=nBufSize && m_bForceLen)||(m_nSetMaxLen>=nCurLen&&!m_bForceLen)))
				{
					nChar += m_ElementPool[i].GetLength();
					nBufSize += sprintf_s(szDest+nBufSize,2048-nBufSize,"%s",m_ElementPool[i].GetString().c_str());
					nCurLen += m_ElementPool[i].GetLength();
				}
				else
				{
					nChar = m_ElementPool[i].GetLength();

					if(m_nSetMaxLen<=nBufSize && m_bForceLen)
						continue;
					if(m_nSetMaxLen<=nCurLen&&!m_bForceLen)
						continue;

					if(nLineN >= nMaxLine)
						continue;
					nLineN++;
					if(nLineN >= nMaxLine)
						continue;
					if(!m_bUseRetChar)
					{
						nBufSize += sprintf_s(szDest+nBufSize,2048-nBufSize,"%s%s",m_strRetChar.c_str(),m_ElementPool[i].GetString().c_str());
						nCurLen += m_strRetChar.length()+m_ElementPool[i].GetLength();
					}
					else
					{
						nBufSize += sprintf_s(szDest+nBufSize,2048-nBufSize,"%s",m_ElementPool[i].GetString().c_str());
						nCurLen += m_ElementPool[i].GetLength();
					}
				}
			}
		}
		else
		{
			//텍스트인경우 적당히 알아서 자른다.
			string strTemp=m_ElementPool[i].GetString();
			int nTempLen=m_ElementPool[i].GetLength();
			if((m_nSetMaxLen<nBufSize+nTempLen && m_bForceLen)||(m_nSetMaxLen<=nCurLen+nTempLen&&!m_bForceLen))
			{
				//근데 이건 컷 걸렸넹 -_-; 이걸 어쩔

				//엔터로 날려먹는거 계산해야겠네
				int nMoreRetChar=(nChar+nTempLen)/m_nMaxChar;
				int nMoreRetChar2=nMaxLine-nLineN-1;
				if(nMoreRetChar2<nMoreRetChar) nMoreRetChar=nMoreRetChar2;
				//적은걸로 계산하죠.

				int nMore=m_nSetMaxLen-nBufSize-nMoreRetChar;
				string strTmp2=strTemp.substr(0,nMore);
				strTemp=strTmp2; //강제로 줄이기!
				nTempLen=nMore;

				//근데 앞으로는 희망이 없으므로 텍스트는 버리져 -_-ㅋ
				for(j=i+1;j<m_ElementPool.size();j++)
				{
					if(!m_ElementPool[j].GetFunc())
					{
						m_ElementPool.erase(m_ElementPool.begin()+j);
					}
				}
			}
			if(nChar + nTempLen <= m_nMaxChar)
			{
				nChar += nTempLen;
				nBufSize += sprintf_s(szDest+nBufSize,2048-nBufSize,"%s",strTemp.c_str());
				nCurLen += nTempLen;
			}
			else
			{
				string strSource = strTemp;
				string strFixedString;

				int nRemainCharN = m_nMaxChar-nChar;

				for(int i=0;i<strSource.length();i++)
				{
					j=0;
					if(IsDBCSLeadByteEx(949,strSource[i]))
					{
						j=2;
					}
					else
					{
						j=1;
					}
					
					if(nRemainCharN-j<0)
					{
						if(nLineN >= nMaxLine)
							break;
						nLineN++;
						if(nLineN >= nMaxLine)
							break;
						if(!m_bUseRetChar)
						{
							strFixedString += m_strRetChar;
							nCurLen += m_strRetChar.length();
						}
						nRemainCharN = m_nMaxChar;
					}

					if(j==2)
					{
						strFixedString.push_back(strSource[i++]);
						nCurLen ++;
						nRemainCharN--;
					}
					strFixedString.push_back(strSource[i]);
					nCurLen ++;
					nRemainCharN--;
				}// for

				nChar = m_nMaxChar - nRemainCharN;
				nBufSize += sprintf_s(szDest+nBufSize,2048-nBufSize,"%s",strFixedString.c_str());

			}//m_nMaxChar
			
		}//GetType()
	}//for Size();
	if(nChar>0 && m_bSetLastRet)
	{
		nBufSize += sprintf_s(szDest+nBufSize,2048-nBufSize,"%s",m_strRetChar.c_str());
	}
}