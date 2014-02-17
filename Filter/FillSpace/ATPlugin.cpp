// ATPlugin.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "ATPlugin.h"

HWND g_hSettingWnd=NULL;
HINSTANCE g_hThisModule;
LPSTR g_szOptionStringBuffer=NULL;

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

	return TRUE;
}

BOOL  __stdcall OnPluginOption()
{
	
	return TRUE;
}
BOOL  __stdcall OnPluginClose()
{

	return TRUE;
}

//여기서부터 쓰자
int nOriginalLength=0;

BOOL  __stdcall PreTranslate(LPCSTR cszInJapanese, LPSTR szOutJapanese, int nBufSize)
{
	nOriginalLength = strlen(cszInJapanese);
	memcpy_s(szOutJapanese,nBufSize,cszInJapanese,nOriginalLength+1);

	// 작업 성공시 TRUE, 실패시 FALSE 반환 
	return TRUE;
}

BOOL  __stdcall PostTranslate(LPCSTR cszInKorean, LPSTR szOutKorean, int nBufSize)
{
	int nNowLength=strlen(cszInKorean);
	memcpy_s(szOutKorean,nBufSize,cszInKorean,nNowLength+1);

	if(nNowLength<nOriginalLength)
	{
		memset(szOutKorean+nNowLength,nOriginalLength-nNowLength,' ');
		szOutKorean[nOriginalLength+1]='\0';
	}

	// 작업 성공시 TRUE, 실패시 FALSE 반환 
	return TRUE;
}