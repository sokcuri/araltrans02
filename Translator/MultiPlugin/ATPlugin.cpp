// ATPlugin.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "resource.h"

#include "ATPlugin.h"
#include "OptionDialog.h"

#include "MultiPlugin.h"

#include "Util.h"

// 전역변수 선언부
ATPLUGIN_VERSION g_pluginVer = {
	{ 0, 2, 20080703 },		// 플러그인 제작시 AT 버전
	{ 2, 0, 20080810 },		// 플러그인 버전
	"whoami",	// 제작자
	"MultiPlugin",	// 플러그인 이름
	"다중플러그인 실행 플러그인"	// 플러그인 설명
};

// 설정창 윈도우 핸들
HWND g_hSettingWnd=NULL;
// 이 플러그인의 모듈 핸들
HMODULE g_hThisModule=NULL;
// 옵션 버퍼
LPSTR g_szOptionStringBuffer=NULL;

const TCHAR g_szPluginName[]=_T("MultiPlugin");

// DLLMain
// 현재 모듈 핸들을 알기 위해 여기서는 꼭 필요함..
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			g_hThisModule=(HMODULE)hModule;
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
	g_szOptionStringBuffer = cszOptionStringBuffer;

	ATPLUGIN_ARGUMENT_ARRAY args;

	GetATPluginArgsFromOptionString(cszOptionStringBuffer, args);
	
	if (!args.empty() && !g_cMultiPlugin.Add(args))
		return FALSE;

	// 초기화 성공시 TRUE, 실패시 FALSE를 반환
	return TRUE;
}

BOOL  __stdcall OnPluginOption()
{
	if (g_hSettingWnd && IsWindow(g_hSettingWnd))
	{
		ATPLUGIN_ARGUMENT_ARRAY args;

		DialogBox((HINSTANCE)g_hThisModule, MAKEINTRESOURCE(IDD_OPTIONDIALOG), g_hSettingWnd, OptionDialogProc);

		g_cMultiPlugin.GetPluginArgs(args);
		GetOptionStringFromATPluginArgs(args, g_szOptionStringBuffer, 4096);
		
	}
	return TRUE;
}
BOOL  __stdcall OnPluginClose()
{
	g_cMultiPlugin.RemoveAll();

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

	return g_cMultiPlugin.OnPluginStart();
}
BOOL  __stdcall OnPluginStop()
{
	// 주의: 여기서 메모리 확보 등을 하면 에러 발생.

	return g_cMultiPlugin.OnPluginStop();
}
#endif

// 번역 플러그인 전용
#ifdef ATPLUGIN_TRANSLATOR
BOOL  __stdcall Translate(LPCSTR cszJapanese, LPSTR szKorean, int nBufSize)
{
	return g_cMultiPlugin.Translate(cszJapanese, szKorean, nBufSize);
}
#endif
