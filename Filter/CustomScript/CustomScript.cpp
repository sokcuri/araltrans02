#include "StdAfx.h"
#include "CustomScript.h"
#include "hash.hpp"
#include "util.h"

wchar_t WriteFileName[4096]=L"";
wchar_t ListFileName[4096]=L"";

CCustomScript::CCustomScript(void)
{
	wchar_t Folder[4096]=L"";
	wcscpy_s(Folder,4096,GetGameDirectory());
	wcscat_s(Folder,4096,L"\\ATData\\");
	MyCreateDirectory(Folder);

	wcscpy_s(WriteFileName,4096,GetGameDirectory());
	wcscat_s(WriteFileName,4096,L"\\ATData\\CustomScript_Auto.txt");

	wcscpy_s(ListFileName,4096,GetGameDirectory());
	wcscat_s(ListFileName,4096,L"\\ATData\\CSList.tmp");

	ThisLine.Line="";
	ThisLine.PositionN=0;
	ThisLine.ScriptN=0;

	WritePassN=0;

	WriteFile = NULL;
}

CCustomScript::~CCustomScript(void)
{
	if(WriteFile)
		fclose(WriteFile);
}

void CCustomScript::Clear()
{
	LineList.clear();
	ThisLine.Line="";
	ThisLine.PositionN=0;
	ThisLine.ScriptN=0;

	if(WriteFile)
		fclose(WriteFile);
}

void CCustomScript::InitR(void)
{
	Clear();

	FILE * ListFile=NULL;
	//일단 파일리스트를 연다.
	_wfopen_s(&ListFile,ListFileName,L"rt,ccs=UTF-8");

	if(ListFile)
	{
		//열었다면
		wchar_t FileName[4096]=L"";

		while(fgetws(FileName,4096,ListFile))
		{
			size_t FileNameLen=wcslen(FileName);
			if(FileName[FileNameLen-1]==L'\n')
			{
				FileNameLen--;
				FileName[FileNameLen]=L'\0';
			}

			if(FileNameLen>0)
			{
				//길이가 정상적으로 있다면.
				AddScript(MakeFullPathSTL(FileName).c_str());
			}
		}
		fclose(ListFile);
	}
}

void CCustomScript::InitW()
{
	Clear();

	int LinesN=AddScript(WriteFileName);

	ScriptN=1;
	ThisLine.ScriptN=0;
	ThisLine.PositionN=LinesN;

	_wfopen_s(&WriteFile,WriteFileName,L"rt,ccs=UTF-16LE");
	//일단 한번 열어보고
	if(WriteFile)
	{
		//있으면
		fclose(WriteFile);
		_wfopen_s(&WriteFile,WriteFileName,L"at,ccs=UTF-16LE");
	}
	else
	{
		_wfopen_s(&WriteFile,WriteFileName,L"wt,ccs=UTF-16LE");
		fwprintf(WriteFile,L"\"Title\"\t\"제목\"\n");
		fwprintf(WriteFile,L"\"Description\"\t\"설명\"");
	}
}

int CCustomScript::CheckJpnLine(LPCSTR Source)
{
	//이 문장이 지금 리스트에 들어있는지 확인!
	//있다면 ThisLine에 대상 한글을 붙여넣음

	UINT HashedSource=MakeStringHash(Source);

	if(LineList.count(HashedSource))
	{
		//내용이 있으면
		multimap<UINT,LineDetail>::iterator FrontLine=LineList.lower_bound(HashedSource);
		multimap<UINT,LineDetail>::iterator EndLine=LineList.upper_bound(HashedSource);

		multimap<UINT,LineDetail>::iterator ThisLine;

		LineDetail TempLine;

		//각 문자열마다 현재 문장에 대해 얼마나 가까운지 '점수'를 매겨
		//가장 점수가 '낮은' 녀석으로 정한다.

		//같은 스크립트인경우 라인번호차이만큼
		//다른 스크립트인경우 1000점만큼

		int MinScore=10000000;

		for(ThisLine=FrontLine;ThisLine!=EndLine;ThisLine++)
		{
			int TempScore=10000000;

			if(ThisLine->second.ScriptN!=CCustomScript::ThisLine.ScriptN)
			{
				//다른 스크립트
				TempScore=1000;
			}
			else
			{
				//같은 스크립트
				TempScore=abs(ThisLine->second.PositionN-CCustomScript::ThisLine.PositionN);
			}

			if(TempScore<MinScore)
			{
				//스코어가 낮다면
				MinScore=TempScore;
				TempLine=ThisLine->second;
			}
		}

		//이제 TempLine의 내용을 저장
		CCustomScript::ThisLine=TempLine;

		return MinScore;
	}
	else
	{
		//내용이 없으니까
		return -1;
	}
}

void CCustomScript::WriteScript(LPCSTR JpnLine, LPCSTR KorLine)
{

	bool Passed=false;
	if(WritePassN==-1)
	{
		if(CheckJpnLine(JpnLine)==-1)
			Passed=true;
	}
	else if(WritePassN==0)
	{
		Passed=true;
	}
	else
	{
		if(CheckJpnLine(JpnLine)>WritePassN)
			Passed=true;
	}
	if(Passed && WriteFile)
	{
		UINT HashedJpnLine=MakeStringHash(JpnLine);

		wchar_t JpnLineW[4096]=L"";
		wchar_t KorLineW[4096]=L"";

		MyMultiByteToWideChar(932,0,JpnLine,-1,JpnLineW,4096);
		MyMultiByteToWideChar(949,0,KorLine,-1,KorLineW,4096);

		AddLine(HashedJpnLine,wstring(KorLineW),ThisLine.ScriptN,ThisLine.PositionN++);

		wchar_t JpnEncodedW[4096]=L"";
		wchar_t KorEncodedW[4096]=L"";

		EncodeLine(JpnEncodedW,JpnLineW);
		EncodeLine(KorEncodedW,KorLineW);

		fwprintf(WriteFile,L"\n\"%u\"\t%s\t%s",HashedJpnLine,JpnEncodedW,KorEncodedW);
	}
}

void CCustomScript::GetKorLine(LPSTR Dest)
{
	//출력
	lstrcpyA(Dest,ThisLine.Line.c_str());
}

void CCustomScript::EncodeLine(LPWSTR Dest,LPCWSTR Source)
{
	size_t SourceLen=wcslen(Source);
	size_t DestLen=0;


	Dest[DestLen++]=L'\"';

	for(size_t i=0;i<SourceLen;i++)
	{
		switch (Source[i])
		{
		case L'\n':
			Dest[DestLen++]=L'\\';
			Dest[DestLen++]=L'n';
			break;
		case L'\r':
			Dest[DestLen++]=L'\\';
			Dest[DestLen++]=L'r';
			break;
		case L'\t':
			Dest[DestLen++]=L'\\';
			Dest[DestLen++]=L't';
			break;
		case L'\"':
			Dest[DestLen++]=L'\"';
			Dest[DestLen++]=L'\"';
			break;
		case L'\\':
			Dest[DestLen++]=L'\\';
			Dest[DestLen++]=L'\\';
			break;
		default:
			Dest[DestLen++]=Source[i];
			break;
		}
	}
	Dest[DestLen++]=L'\"';
	Dest[DestLen]=L'\0';
}

wstring CCustomScript::DecodeLine(wstring Source)
{
	wstring Dest=L"";

	bool isCSV=false;

	int End=0;
	while(1)
	{
		if(Source[End]==L'\"')
		{
			End++;
		}
		else
		{
			if(End%2==1)
			{
				isCSV=true;
			}
			break;
		}
	}

	wstring::size_type i=0;
	wstring::size_type Len=Source.length();

	if(isCSV)
	{
		i=i+1;
		Len=Len-1;
	}

	while(i<Len)
	{
		if(isCSV && Source[i]==L'\"' && Source[i+1]==L'\"')
		{
			i++;
			Dest+=L'\"';
		}
		else if(Source[i]==L'\\')
		{
			i++;
			switch(Source[i])
			{
			case L'n':
				Dest+=L'\n';
				break;
			case L'r':
				Dest+=L'\r';
				break;
			case L't':
				Dest+=L'\t';
			case L'\\':
				Dest+=L'\\';
				break;
			default:
				Dest+=L'\\';
				Dest+=Source[i];
				break;
			}
		}
		else
		{
			Dest+=Source[i];
		}
		i++;
	}
	return Dest;
}

int CCustomScript::AddScript(LPCWSTR Source)
{
	int LineN=0;

	FILE * ScriptFile=NULL;
	_wfopen_s(&ScriptFile,Source,L"rt,ccs=UTF-16LE");

	if(ScriptFile)
	{
		wchar_t TempLine[4096]=L"";

		while(fgetws(TempLine,4096,ScriptFile))
		{
			size_t TempLineLen=wcslen(TempLine);
			if(TempLine[TempLineLen-1]==L'\n')
			{
				TempLineLen--;
				TempLine[TempLineLen]=L'\0';
			}

			//LineN
			//0 : Title
			//1 : Description
			if(LineN<2){
				LineN++;
				continue;
			}


			if(TempLineLen>0)
			{
				//정상적으로 있다면.
				
				wstring _TempLine=TempLine;
				//쓰기 편하도록 변환

				wstring::size_type Tab1;
				wstring::size_type Tab2;
				wstring::size_type Tab3;//만약을 위해서

				Tab1=_TempLine.find(L'\t');
				if(Tab1!=_TempLine.npos)
					Tab2=_TempLine.find(L'\t',Tab1+1);
				if(Tab2!=_TempLine.npos)
					Tab3=_TempLine.find(L'\t',Tab2+1);
				if(Tab3==_TempLine.npos)
					Tab3=_TempLine.length();

				if(Tab1!=_TempLine.npos &&
					Tab2!=_TempLine.npos &&
					Tab3!=_TempLine.npos)
				{
					wstring Hashed=DecodeLine(_TempLine.substr(0,Tab1));
					//wstring _JpnLine=_TempLine.substr(Tab1+1,Tab2-Tab1-1);
					wstring _KorLine=_TempLine.substr(Tab2+1,Tab3-Tab2-1);

					UINT HashedJpnLine=0;
					for(wstring::size_type j=0;j<Hashed.length();j++)
					{
						HashedJpnLine*=10;
						HashedJpnLine+=Hashed[j]-L'0';
					}

					AddLine(HashedJpnLine,DecodeLine(_KorLine),ScriptN,LineN++);
				}
			}
		}

		ScriptN++;

		fclose(ScriptFile);
	}
	else
	{
		wstring strTemp = wstring(Source) + wstring(L"\r\n파일이 열리지 않습니다.\r\n파일이 없거나 다른 프로그램이 사용 중일 수 있습니다.");
		::MessageBox(NULL, strTemp.c_str(), L"오류", MB_OK|MB_ICONWARNING|MB_TOPMOST);
	}

	return LineN;
}


void CCustomScript::AddLine(UINT HashedJpnLine, wstring KorLineW, int ScriptN, int PositionN)
{
	char KorLine[4096]="";
	MyWideCharToMultiByte(949,0,KorLineW.c_str(),-1,KorLine,4096,NULL,NULL);

	LineDetail TempLine;

	TempLine.Line=KorLine;
	TempLine.PositionN=PositionN;
	TempLine.ScriptN=ScriptN;

	LineList.insert(make_pair(HashedJpnLine,TempLine));
}