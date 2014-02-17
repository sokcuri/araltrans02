// ATPlugin.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "ATPlugin.h"

#include "resource.h"

#include "CmdFilter.h"
#include "OptionDialog.h"

#ifdef _UNICODE
#include "tstring.h"
#endif

// ATCApi 사용하려면 include
//#include "ATCApi.h"

// Util 사용하려면 include
#include "Util.h"

#include "tstring.h"

#include <stdio.h>



/*
** 전역변수 선언부
*/

// 플러그인 버전
ATPLUGIN_VERSION g_pluginVer = {
	{ 0, 2, 20080704 },		// 플러그인 제작시 AT 버전
	{ 0, 3, 20080807 },		// 플러그인 버전
	"whoami",	// 제작자
	"CmdFilter",	// 플러그인 이름
	"커맨드 필터"	// 플러그인 설명
};

// 설정창 윈도우 핸들
HWND g_hSettingWnd=NULL;

// 이 플러그인의 모듈 핸들 = 인스턴스 핸들
HINSTANCE g_hThisModule;

// 옵션 버퍼 - 옵션이 바뀌었을 때 여기에 써주면 아랄트랜스 바로가기에도 등록할 수 있다.
// 주의 - 옵션길이는 최고 4096 바이트.
LPSTR g_szOptionStringBuffer=NULL;

// debug
tstring g_strLogFile;

void ApplyOption();

void TrimCommand(const char *cszOriginal, string &strPrefix, string &strBody, string &strPostfix);

void RestoreCommand(string &strPrefix, const char *cszTranslated, string &strPostfix, string &strRestored, int nMaxSize);

void SqueezeAndReplaceSpecialString(string &strData, bool bCutSpace=false);

// DLLMain
// 자체 리소스가 있는 경우 모듈 핸들을 알아오기 위해 필요
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			g_hThisModule = (HINSTANCE) hModule;
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}

// 플러그인 export 함수

// 공통
BOOL  __stdcall OnPluginInit(HWND hSettingWnd, LPSTR cszOptionStringBuffer)
{

	g_hSettingWnd = hSettingWnd;
	g_szOptionStringBuffer=cszOptionStringBuffer;
	
	ATOPTION_ARRAY aOptions;

	FILE *fp=NULL;

	GetATOptionsFromOptionString(cszOptionStringBuffer, aOptions);

	for(int i=0; i<aOptions.size(); i++)
	{
		if (!lstrcmpi(aOptions[i].strKey.c_str(), _T("DEBUG")))
		{
			g_strLogFile=aOptions[i].strValue;
			fp=_tfopen(g_strLogFile.c_str(), _T("w"));
		}
		else if (!lstrcmpi(aOptions[i].strKey.c_str(), _T("OVERWRITE")))
		{
			if (!lstrcmpi(aOptions[i].strValue.c_str(), _T("FALSE")))
				g_cCmdFilter.SetOverwriteSetting(false);
			else
				g_cCmdFilter.SetOverwriteSetting(true);
		}
		else if (!lstrcmpi(aOptions[i].strKey.c_str(), _T("REMOVESPACE")))
		{
			if (!lstrcmpi(aOptions[i].strValue.c_str(), _T("FALSE")))
				g_cCmdFilter.SetRemoveSpaceSetting(false);
			else
				g_cCmdFilter.SetRemoveSpaceSetting(true);
		}
		else if (!lstrcmpi(aOptions[i].strKey.c_str(), _T("TWOBYTE")))
		{
			if (!lstrcmpi(aOptions[i].strValue.c_str(), _T("FALSE")))
				g_cCmdFilter.SetTwoByteSetting(false);
			else
				g_cCmdFilter.SetTwoByteSetting(true);
		}
		else if (!lstrcmpi(aOptions[i].strKey.c_str(), _T("CUT")) )
		{
			int n=_ttoi(aOptions[i].strValue.c_str());

			if (n<0) n=0;

			if (fp) fprintf(fp, "CUT=%d\n", n);

			g_cCmdFilter.SetCutSizeSetting(n);
		}
		else if ( !lstrcmpi(aOptions[i].strKey.c_str(), _T("DENY")) )
		{
			g_cCmdFilter.SetDenySetting(aOptions[i].strValue);

			if (fp)
			{
#ifdef _UNICODE
				char strTemp[1024];
				Wide2Kor(aOptions[i].strValue.c_str(), strTemp, 1024);
				fprintf(fp, "Deny=%s\n", strTemp);
#else
				fprintf(fp, "Deny=%s\n", aOptions[i].strValue.c_str());
#endif
			}
		}
		else if ( !lstrcmpi(aOptions[i].strKey.c_str(), _T("PREINC")) )
		{
			g_cCmdFilter.SetPrefixIncludeSetting(aOptions[i].strValue);
			
			if (fp)
			{
#ifdef _UNICODE
				char strTemp[1024];
				Wide2Kor(aOptions[i].strValue.c_str(), strTemp, 1024);
				fprintf(fp, "PreInc=%s\n", strTemp);
#else
				fprintf(fp, "PreInc=%s\n", aOptions[i].strValue.c_str());
#endif
			}
		}
		else if ( !lstrcmpi(aOptions[i].strKey.c_str(), _T("PREEXC")) )
		{
			g_cCmdFilter.SetPrefixExcludeSetting(aOptions[i].strValue);
			
			if (fp)
			{
#ifdef _UNICODE
				char strTemp[1024];
				Wide2Kor(aOptions[i].strValue.c_str(), strTemp, 1024);
				fprintf(fp, "PreExc=%s\n", strTemp);
#else
				fprintf(fp, "PreExc=%s\n", aOptions[i].strValue.c_str());
#endif
			}
		}		
		else if ( !lstrcmpi(aOptions[i].strKey.c_str(), _T("BODYINC")) )
		{
			g_cCmdFilter.SetBodyIncludeSetting(aOptions[i].strValue);
			
			if (fp)
			{
#ifdef _UNICODE
				char strTemp[1024];
				Wide2Kor(aOptions[i].strValue.c_str(), strTemp, 1024);
				fprintf(fp, "BodyInc=%s\n", strTemp);
#else
				fprintf(fp, "BodyInc=%s\n", aOptions[i].strValue.c_str());
#endif
			}
		}
		else if ( !lstrcmpi(aOptions[i].strKey.c_str(), _T("BODYEXC")) )
		{
			g_cCmdFilter.SetBodyExcludeSetting(aOptions[i].strValue);
			
			if (fp)
			{
#ifdef _UNICODE
				char strTemp[1024];
				Wide2Kor(aOptions[i].strValue.c_str(), strTemp, 1024);
				fprintf(fp, "BodyExc=%s\n", strTemp);
#else
				fprintf(fp, "BodyExc=%s\n", aOptions[i].strValue.c_str());
#endif
			}
		}		
	}	// for (..)
	if (fp)
		fclose(fp);

	// 초기화 성공시 TRUE, 실패시 FALSE를 반환
	return TRUE;
}

BOOL  __stdcall OnPluginOption()
{
	if (g_hSettingWnd && IsWindow(g_hSettingWnd))
	{
		
		if (DialogBox((HINSTANCE)g_hThisModule, MAKEINTRESOURCE(IDD_OPTIONDIALOG), g_hSettingWnd, OptionDialogProc) == IDOK)
			ApplyOption();
	}
	return TRUE;
}
BOOL  __stdcall OnPluginClose()
{
	// 주의: 여기서 메모리 확보 등을 하면 에러 발생.
	// TODO: 플러그인 종료 준비, 자원 반환 등을 한다.

	return TRUE;
}
const ATPLUGIN_VERSION * __stdcall OnPluginVersion()
{
	// 플러그인 버전 반환

	return &g_pluginVer;
}

// 후킹 플러그인 전용
#ifdef ATPLUGIN_HOOKER
BOOL  __stdcall OnPluginStart()
{
	// TODO: 후킹 시작 작업.

	// 작업 성공시 TRUE, 실패시 FALSE 반환 
	return TRUE;
}
BOOL  __stdcall OnPluginStop()
{
	// 주의: 여기서 메모리 확보 등을 하면 에러 발생.
	// TODO: 후킹 종료 작업.

	// 작업 성공시 TRUE, 실패시 FALSE 반환 
	return TRUE;
}
#endif

// 번역 플러그인 전용
#ifdef ATPLUGIN_TRANSLATOR
BOOL  __stdcall Translate(LPCSTR cszJapanese, LPSTR szKorean, int nBufSize)
{

	// 작업 성공시 TRUE, 실패시 FALSE 반환 
	return TRUE;
}
#endif

// 필터 플러그인 전용
#ifdef ATPLUGIN_FILTER
BOOL  __stdcall PreTranslate(LPCSTR cszInJapanese, LPSTR szOutJapanese, int nBufSize)
{
	// TODO: 번역 전처리를 한다.
	
	// 작업 성공시 TRUE, 실패시 FALSE 반환 
	return g_cCmdFilter.PreTranslate(cszInJapanese, szOutJapanese, nBufSize);
}

BOOL  __stdcall PostTranslate(LPCSTR cszInKorean, LPSTR szOutKorean, int nBufSize)
{
	// TODO: 번역 후처리를 한다.
	
	// 작업 성공시 TRUE, 실패시 FALSE 반환 
	return g_cCmdFilter.PostTranslate(cszInKorean, szOutKorean, nBufSize);
}
#endif

void ApplyOption()
{
	ATOPTION_ARRAY aOptions;
	ATOPTION_PAIR option;
	tstring strBoundary;

	if (!g_strLogFile.empty())
	{
		option.strKey=_T("DEBUG");
		option.strValue = g_strLogFile;
		aOptions.push_back(option);
	}
	if (g_cCmdFilter.GetOverwriteSetting())
	{
		option.strKey=_T("OVERWRITE");
		option.strValue.erase();
		aOptions.push_back(option);
	}
	if (g_cCmdFilter.GetRemoveSpaceSetting())
	{
		option.strKey=_T("REMOVESPACE");
		option.strValue.erase();
		aOptions.push_back(option);
	}
	if (g_cCmdFilter.GetTwoByteSetting())
	{
		option.strKey=_T("TWOBYTE");
		option.strValue.erase();
		aOptions.push_back(option);
	}
	if (g_cCmdFilter.GetCutSizeSetting())
	{
		TCHAR szBuf[10];
		option.strKey=_T("CUT");
		option.strValue=_itot(g_cCmdFilter.GetCutSizeSetting(), szBuf, 10);
		aOptions.push_back(option);
	}
	if (g_cCmdFilter.GetDenySetting(strBoundary))
	{
		option.strKey=_T("DENY");
		option.strValue=strBoundary;
		aOptions.push_back(option);
	}
	if (g_cCmdFilter.GetPrefixIncludeSetting(strBoundary))
	{
		option.strKey=_T("PREINC");
		option.strValue=strBoundary;
		aOptions.push_back(option);
	}
	if (g_cCmdFilter.GetPrefixExcludeSetting(strBoundary))
	{
		option.strKey=_T("PREEXC");
		option.strValue=strBoundary;
		aOptions.push_back(option);
	}
	if (g_cCmdFilter.GetBodyIncludeSetting(strBoundary))
	{
		option.strKey=_T("BODYINC");
		option.strValue=strBoundary;
		aOptions.push_back(option);
	}
	if (g_cCmdFilter.GetBodyExcludeSetting(strBoundary))
	{
		option.strKey=_T("BODYEXC");
		option.strValue=strBoundary;
		aOptions.push_back(option);
	}


	GetOptionStringFromATOptions(aOptions, g_szOptionStringBuffer, 4096);

}
