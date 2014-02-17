#include "StdAfx.h"

#include "Util.h"

#include "MultiPlugin.h"
#include "ATPlugin.h"

#include "tstring.h"

using namespace std;


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

void GetATPluginArgsFromOptionString(const char *pszOption, ATPLUGIN_ARGUMENT_ARRAY &aPluginArgs)
{
	tstring strNow;

	int nParentheseCount;

	int i, nOptionSize;

	ATPLUGIN_ARGUMENT arg;

	aPluginArgs.clear();

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
					// 옵션이 아닌경우 : ex) aPlugin,bPlugin
					if (!strNow.empty())
					{
						// 옵션 없는 플러그인으로 간주하고 등록
						arg.strPluginName = strNow;
						arg.strOption.erase();
						aPluginArgs.push_back(arg);

						arg.strPluginName.erase();
						strNow.erase();
					}
				}
				break;

			case '{':
				if (!nParentheseCount)
				{
					nParentheseCount++;
					// 플러그인 이름->옵션 넘어감
					arg.strPluginName=strNow;
					strNow.erase();
				}
				else
				{
					// 옵션 내부의 {
					nParentheseCount++;
					strNow+=MakeTChar(pszOption+i);
				}
				break;

			case '}':
				if (nParentheseCount == 1)
				{
					// 옵션 종료 - 등록
					nParentheseCount--;
					arg.strOption=strNow;
					aPluginArgs.push_back(arg);
	
					// 데이타 삭제
					arg.strPluginName.erase();
					arg.strOption.erase();
					strNow.erase();
				}
				else if (nParentheseCount)
				{
					nParentheseCount--;
					// 옵션 내부의 }
					strNow+=MakeTChar(pszOption+i);
				}
				break;	// 그 외 - 무시

			default:
				strNow+=MakeTChar(pszOption+i);
				if (IsDBCSLeadByteEx(949, (BYTE) pszOption[i]))
					i++;

		}
	}
	// 만약 strNow 가 아직 남아있으면 그냥 플러그인에게 넘겨준다.
	if (!strNow.empty())
	{
		if (nParentheseCount)
		{
			arg.strOption=strNow;
			aPluginArgs.push_back(arg);
		}
		else
		{
			arg.strPluginName=strNow;
			arg.strOption.erase();
			aPluginArgs.push_back(arg);
		}

	}
}

void GetOptionStringFromATPluginArgs(const ATPLUGIN_ARGUMENT_ARRAY &aPluginArgs, char *pszOption, int nMaxOptionLength)
{
	tstring strOption;

	int i, nPlugins;

	nPlugins=aPluginArgs.size();

	for(i=0; i<nPlugins; i++)
	{
		if (!aPluginArgs[i].strPluginName.empty())
			strOption+=aPluginArgs[i].strPluginName+_T("{")+aPluginArgs[i].strOption+_T("}");

		if (i != nPlugins-1)
			strOption+=_T(',');
	}
#ifdef _UNICODE
	Wide2Kor(strOption.c_str(), pszOption);
#else
	lstrcpyn(pszOption, strOption.c_str(), nMaxOptionLength);
#endif
}


