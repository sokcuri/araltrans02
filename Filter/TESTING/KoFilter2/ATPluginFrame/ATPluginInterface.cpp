// ATPluginInterface - 아랄트랜스 플러그인 인터페이스 규약
//

#include "StdAfx.h"
#include "ATPluginInterface.h"
#include "../ATPlugin.h"

/*
** 플러그인 export 함수
*/

// 공통
BOOL  __stdcall OnPluginInit(HWND hAralWnd, LPVOID pvPluginOption)
{
	BOOL bRetVal;

	bRetVal = theApp._PrePluginInit(hAralWnd, pvPluginOption);

#ifdef ATPLUGIN_ALGORITHM
	if ( bRetVal && theApp.m_dwAralVersion == MAKELONG(0003,0000) ) theApp.OnPluginStart();
#endif
	
	return bRetVal;
}

BOOL  __stdcall OnPluginOption()
{
	return theApp.OnPluginOption();
}

BOOL  __stdcall OnPluginClose()
{
#ifdef ATPLUGIN_ALGORITHM
	if ( theApp.m_dwAralVersion == MAKELONG(0003,0000) ) theApp.OnPluginStop();
#endif

	return theApp.OnPluginClose();
}

#ifdef USE_ARALTRANS_0_3	// 아랄트랜스 0.3 전용
BOOL __stdcall GetPluginInfo(PLUGIN_INFO * pPluginInfo)
{
	return theApp.GetPluginInfo(pPluginInfo);
}
#endif	// USE_ARALTRANS_0_3


#ifdef USE_ARALTRANS_0_2	// 아랄트랜스 0.2 전용

// 후킹 플러그인 전용
#ifdef ATPLUGIN_ALGORITHM
BOOL  __stdcall OnPluginStart()
{
	return theApp.OnPluginStart();
}
BOOL  __stdcall OnPluginStop()
{
	return theApp.OnPluginStop();
}
#endif

// 번역 플러그인 전용
#ifdef ATPLUGIN_TRANSLATOR
BOOL  __stdcall Translate(LPCSTR cszJapanese, LPSTR szKorean, int nBufSize)
{
	return theApp.Translate(cszJapanese, szKorean, nBufSize);
}
#endif

// 필터 플러그인 전용
#ifdef ATPLUGIN_FILTER
BOOL  __stdcall PreTranslate(LPCSTR cszInJapanese, LPSTR szOutJapanese, int nBufSize)
{
	return theApp.PreTranslate(cszInJapanese,szOutJapanese,nBufSize);
}

BOOL  __stdcall PostTranslate(LPCSTR cszInKorean, LPSTR szOutKorean, int nBufSize)
{
	return theApp.PostTranslate(cszInKorean, szOutKorean, nBufSize);
}
#endif
#endif // USE_ARALTRANS_0_2


#ifdef USE_ARALTRANS_0_3
// 필터 및 번역 플러그인 공용
#if defined(ATPLUGIN_FILTER) || defined(ATPLUGIN_TRANSLATOR)
BOOL __stdcall OnObjectInit(TRANSLATION_OBJECT* pTransObj)
{
	return theApp.OnObjectInit(pTransObj);
}
BOOL __stdcall OnObjectClose(TRANSLATION_OBJECT* pTransObj)
{
	return theApp.OnObjectClose(pTransObj);
}
BOOL __stdcall OnObjectMove(TRANSLATION_OBJECT* pTransObj)
{
	return theApp.OnObjectMove(pTransObj);
}
BOOL __stdcall OnObjectOption(TRANSLATION_OBJECT* pTransObj)
{
	return theApp.OnObjectOption(pTransObj);
}

BOOL __stdcall MainTranslateProcedure(TRANSLATION_OBJECT* pTransObj)
{
	return theApp.MainTranslateProcedure(pTransObj);
}

#endif
#endif // USE_ARALTRANS_0_3

#ifndef USE_MFC

// DLLMain
// 자체 리소스가 있는 경우 모듈 핸들을 알아오기 위해 필요
// MFC 에서는 CWinApp 가 전부 해 주기 때문에 필요없음..
BOOL APIENTRY DllMain( HANDLE hModule, 
					  DWORD  ul_reason_for_call, 
					  LPVOID lpReserved
					  )
{
    switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		theApp.m_hInstance = (HINSTANCE) hModule;
		theApp.InitInstance();
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
    }
    return TRUE;
}
#endif // USE_MFC
