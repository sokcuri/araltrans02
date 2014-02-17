//
// ATPlugin - 아랄트랜스 플러그인 규격을 따르는 기본 DLL 모듈
//

#ifndef __ATPLUGIN_H__
#define __ATPLUGIN_H__

// 설정창 윈도우 핸들
extern HWND g_hSettingWnd;

// 이 플러그인의 모듈 핸들 = 인스턴스 핸들
extern HINSTANCE g_hThisModule;

// 이 플러그인의 옵션
extern LPSTR g_szOptionStringBuffer;

// 플러그인 export 함수 선언
#ifdef __cplusplus
extern "C"
{
#endif

// 공통
BOOL __stdcall OnPluginInit(HWND hSettingWnd, LPSTR cszOptionStringBuffer);
BOOL __stdcall OnPluginOption();
BOOL __stdcall OnPluginClose();

BOOL __stdcall PreTranslate(LPCSTR cszInJapanese, LPSTR szOutJapanese, int nBufSize);
BOOL __stdcall PostTranslate(LPCSTR cszInKorean, LPSTR szOutKorean, int nBufSize);

// export 를 위한 링커 옵션
// 공통
#pragma comment(linker, "/EXPORT:OnPluginInit=_OnPluginInit@8")
#pragma comment(linker, "/EXPORT:OnPluginOption=_OnPluginOption@0")
#pragma comment(linker, "/EXPORT:OnPluginClose=_OnPluginClose@0")

#pragma comment(linker, "/EXPORT:PreTranslate=_PreTranslate@12")
#pragma comment(linker, "/EXPORT:PostTranslate=_PostTranslate@12")

#ifdef __cplusplus
}
#endif

#endif //__ATPLUGIN_H__