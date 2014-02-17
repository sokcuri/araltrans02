// ATPlugin.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "ATPlugin.h"

#include "resource.h"

#include "RLCmd.h"
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
	{ 1, 0, 20081005 },		// 플러그인 버전
	"whoami",	// 제작자
	"RLCmd",	// 플러그인 이름
	"RealLive 커맨드 필터"	// 플러그인 설명
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

	GetATOptionsFromOptionString(cszOptionStringBuffer, aOptions);

	for(int i=0; i<aOptions.size(); i++)
	{
		if (!lstrcmpi(aOptions[i].strKey.c_str(), _T("DEBUG")))
		{
			FILE *fp;
			g_strLogFile=aOptions[i].strValue;
			fp=_tfopen(g_strLogFile.c_str(), _T("w"));
			fclose(fp);
		}
		else if (!lstrcmpi(aOptions[i].strKey.c_str(), _T("OVERWRITE")))
		{
			if (!lstrcmpi(aOptions[i].strValue.c_str(), _T("FALSE")))
				g_cRLCmd.SetOverwriteSetting(false);
			else
				g_cRLCmd.SetOverwriteSetting(true);
		}
		else if (!lstrcmpi(aOptions[i].strKey.c_str(), _T("REMOVESPACE")))
		{
			if (!lstrcmpi(aOptions[i].strValue.c_str(), _T("FALSE")))
				g_cRLCmd.SetRemoveSpaceSetting(false);
			else
				g_cRLCmd.SetRemoveSpaceSetting(true);
		}
		else if (!lstrcmpi(aOptions[i].strKey.c_str(), _T("BRACKET")))
		{
			if (!lstrcmpi(aOptions[i].strValue.c_str(), _T("FALSE")))
				g_cRLCmd.SetBracketSetting(false);
			else
				g_cRLCmd.SetBracketSetting(true);
		}
		else if (!lstrcmpi(aOptions[i].strKey.c_str(), _T("NOFREEZE")))
		{
			if (!lstrcmpi(aOptions[i].strValue.c_str(), _T("FALSE")))
				g_cRLCmd.SetNoFreezeSetting(false);
			else
				g_cRLCmd.SetNoFreezeSetting(true);
		}
		
	}
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
	return g_cRLCmd.PreTranslate(cszInJapanese, szOutJapanese, nBufSize);
}

BOOL  __stdcall PostTranslate(LPCSTR cszInKorean, LPSTR szOutKorean, int nBufSize)
{
	// TODO: 번역 후처리를 한다.
	
	// 작업 성공시 TRUE, 실패시 FALSE 반환 
	return g_cRLCmd.PostTranslate(cszInKorean, szOutKorean, nBufSize);
}
#endif

void ApplyOption()
{
	ATOPTION_ARRAY aOptions;
	ATOPTION_PAIR option;

	if (!g_strLogFile.empty())
	{
		option.strKey=_T("DEBUG");
		option.strValue = g_strLogFile;
		aOptions.push_back(option);
	}
	if (g_cRLCmd.GetOverwriteSetting())
	{
		option.strKey=_T("OVERWRITE");
		option.strValue.erase();
		aOptions.push_back(option);
	}
	if (g_cRLCmd.GetRemoveSpaceSetting())
	{
		option.strKey=_T("REMOVESPACE");
		option.strValue.erase();
		aOptions.push_back(option);
	}
	if (g_cRLCmd.GetBracketSetting())
	{
		option.strKey=_T("BRACKET");
		option.strValue.erase();
		aOptions.push_back(option);
	}
	if (g_cRLCmd.GetNoFreezeSetting())
	{
		option.strKey=_T("NOFREEZE");
		option.strValue.erase();
		aOptions.push_back(option);
	}
	GetOptionStringFromATOptions(aOptions, g_szOptionStringBuffer, 4096);

}
