#include "StdAfx.h"

#include "Util.h"

#include "ATPlugin.h"


const TCHAR *GetATDirectory()
{
	static TCHAR szDirectory[MAX_PATH]={_T('\0'),};
	int i;
	
	if (szDirectory[0] != _T('\0'))
		return szDirectory;

	// 아랄트랜스 설치 디렉토리를 알아온다.
	
	GetModuleFileName(GetModuleHandle(_T("ATCTNR.DLL")), szDirectory, MAX_PATH);
	
	for(i=lstrlen(szDirectory); i>=0; i--)
	{
		if (szDirectory[i] == _T('\\'))
		{
			szDirectory[i]=_T('\0');
			break;
		}
	}
	return szDirectory;
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

	nOptions=aOptions.size();

	for(i=0; i<nOptions; i++)
	{
		if (aOptions[i].strValue.empty())
			strOption+=aOptions[i].strKey;
		else
			strOption+=aOptions[i].strKey+_T("(")+aOptions[i].strValue+_T(")");

		if (i != nOptions-1)
			strOption+=_T(',');
	}
#ifdef _UNICODE
	Wide2Kor(strOption.c_str(), pszOption);
#else
	lstrcpyn(pszOption, strOption.c_str(), nMaxOptionLength);
#endif
}
