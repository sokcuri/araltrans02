#include "StdAfx.h"

#include "Util.h"

#include "ATPlugin.h"

wstring StringToHex(string String)
{
	wchar_t Temp[4096]=L"\0";

	int T=0;

	for(int i=0;i<String.length();i++)
	{
		BYTE szBuf=(BYTE) String[i];
		T+= wsprintf(Temp+T,L"%02X",szBuf);
	}
	
	wstring WTemp=Temp;

	return WTemp;
}

string HexToString(wstring HexString)
{
	//그대로 긁어왔다!! 우왕 ㅋ 굳 ㅋ

	BYTE CharTemp1=0;
	string::size_type Len=HexString.length();

	string StringTemp="";
	int CharFull=0;
	for(string::size_type i=0;i<Len;i++){
		BYTE CharTemp2=0;
		if(HexString[i]==L' ')
		{
			//이건 넘긴다
		}
		else
		{
			if(L'A'<=HexString[i]&&HexString[i]<=L'F'){
				//A~F, 대문자만 받도록 되어있다.
				CharTemp2=(BYTE)(HexString[i]-L'A'+10);
			}
			else if(L'a'<=HexString[i]&&HexString[i]<=L'f'){
				//그러나 만약의 경우를 위해
				CharTemp2=(BYTE)(HexString[i]-L'a'+10);
			}
			else if(L'0'<=HexString[i]&&HexString[i]<=L'9'){
				//0~9 숫자범위
				CharTemp2=(BYTE)(HexString[i]-L'0');
			}
			else
			{
				CharFull+=10;
			}

			if(CharFull==0)
			{
				CharFull=1;
				CharTemp1=CharTemp2*16;
			}
			else if(CharFull==1)
			{
				CharFull=0;
				CharTemp1+=CharTemp2;

				char Temp3;
				Temp3=(char)CharTemp1;
				StringTemp+=Temp3;
			}
			else
			{
				CharFull-=10;
			}
		}
	}//for문 종료!

	return StringTemp;
}

BOOL MyCreateDirectory(LPCTSTR lpDirectory)
{
	DWORD pos,len;
	LPTSTR str;

	if (!lpDirectory) return FALSE;

	len=lstrlen(lpDirectory);
	str=new TCHAR [len+1];
	lstrcpy(str,lpDirectory);
	for (pos=0; str[pos]; pos++)
	{
		if (str[pos]==_T('\\'))
		{
			str[pos]=0;
			CreateDirectory(str,NULL);
			str[pos]=_T('\\');
		}
	}
	CreateDirectory(str,NULL);

	delete [] str;
	return CreateDirectory(lpDirectory,NULL) || ((pos=GetFileAttributes(lpDirectory)) != 0xFFFFFFFF && (pos & FILE_ATTRIBUTE_DIRECTORY));
}


const TCHAR *GetATDirectory()
{
	static TCHAR szATDirectory[MAX_PATH]={_T('\0'),};
	int i;
	
	if (szATDirectory[0] != _T('\0'))
		return szATDirectory;

	// 아랄트랜스 설치 디렉토리를 알아온다.
	
	GetModuleFileName(GetModuleHandle(_T("ATCTNR.DLL")), szATDirectory, MAX_PATH);
	
	for(i=lstrlen(szATDirectory); i>=0; i--)
	{
		if (szATDirectory[i] == _T('\\'))
		{
			szATDirectory[i]=_T('\0');
			break;
		}
	}
	return szATDirectory;
}

const TCHAR *GetGameDirectory()
{
	static TCHAR szGameDirectory[MAX_PATH]={_T('\0'),};
	int i;
	
	if (szGameDirectory[0] != _T('\0'))
		return szGameDirectory;

	//게임 디렉토리를 알아온다.
	
	GetModuleFileName(GetModuleHandle(NULL), szGameDirectory, MAX_PATH);
	
	for(i=lstrlen(szGameDirectory); i>=0; i--)
	{
		if (szGameDirectory[i] == _T('\\'))
		{
			szGameDirectory[i]=_T('\0');
			break;
		}
	}
	return szGameDirectory;
}

void GetATOptionsFromOptionString(const char *pszOption, ATOPTION_ARRAY &aOptions)
{
	tstring strNow;

	int nParentheseCount;

	int i, nOptionSize;

	ATOPTION_PAIR option;

	aOptions.clear();

	nOptionSize=lstrlenA(pszOption);
	if (nOptionSize == 0)
		return;


	strNow.reserve(nOptionSize);
	nParentheseCount=0;

	for(i=0; i<nOptionSize; i++)
	{

		switch(pszOption[i])
		{
			case ',':
				if (nParentheseCount)
				{
					// 옵션 속의 ,
					strNow+=MakeTChar(pszOption+i);
				}
				else
				{
					// () 가 없는 경우 : ex) aOption,bOption
					if (!strNow.empty())
					{
						// value 없는 플러그인으로 간주하고 등록
						option.strKey = strNow;
						option.strValue.erase();
						aOptions.push_back(option);

						option.strKey.erase();
						strNow.erase();
					}
				}
				break;

			case '(':
				if (!nParentheseCount)
				{
					nParentheseCount++;
					// 옵션 Key->옵션 Value 넘어감
					option.strKey=strNow;
					strNow.erase();
				}
				else
				{
					// Value 내부의 (
					nParentheseCount++;
					strNow+=MakeTChar(pszOption+i);
				}
				break;

			case ')':
				if (nParentheseCount == 1)
				{
					// 옵션 1개 완료 - 등록
					nParentheseCount--;
					option.strValue=strNow;
					aOptions.push_back(option);
	
					// 데이타 삭제
					option.strKey.erase();
					option.strValue.erase();
					strNow.erase();
				}
				else if (nParentheseCount)
				{
					nParentheseCount--;
					// Value 내부의 )
					strNow+=MakeTChar(pszOption+i);
				}
				break;	// 그 외 - 무시

			default:
				strNow+=MakeTChar(pszOption+i);
				if (IsDBCSLeadByteEx(949, (BYTE) pszOption[i]))
					i++;
		}
	}
	// 만약 strNow 가 아직 남아있으면 그냥 등록시킨다.
	if (!strNow.empty())
	{
		if (nParentheseCount)
		{
			option.strValue=strNow;
			aOptions.push_back(option);
		}
		else
		{
			option.strKey=strNow;
			option.strValue.erase();
			aOptions.push_back(option);
		}

	}
}

void GetOptionStringFromATOptions(const ATOPTION_ARRAY &aOptions, char *pszOption, int nMaxOptionLength)
{
	tstring strOption;

	int i, nOptions;

	nOptions=(int)aOptions.size();

	for(i=0; i<nOptions; i++)
	{
		if (aOptions[i].strValue.empty())
			strOption+=aOptions[i].strKey;
		else
			strOption+=aOptions[i].strKey+_T("(")+aOptions[i].strValue+_T(")");

		if (i != nOptions-1)
			strOption+=_T(',');
	}
	Wide2Kor(strOption.c_str(), pszOption);
}
