#pragma once 

#ifdef __cplusplus
extern "C"
{
#endif

BOOL __stdcall OnPluginInit(HWND hSettingWnd, LPSTR cszOptionStringBuffer);
BOOL __stdcall OnPluginOption();
BOOL __stdcall OnPluginClose();

BOOL __stdcall PreTranslate(LPCSTR cszInJapanese, LPSTR szOutJapanese, int nBufSize);
BOOL __stdcall PostTranslate(LPCSTR cszInKorean, LPSTR szOutKorean, int nBufSize);

#pragma comment(linker, "/EXPORT:OnPluginInit=_OnPluginInit@8")
#pragma comment(linker, "/EXPORT:OnPluginOption=_OnPluginOption@0")
#pragma comment(linker, "/EXPORT:OnPluginClose=_OnPluginClose@0")

#pragma comment(linker, "/EXPORT:PreTranslate=_PreTranslate@12")
#pragma comment(linker, "/EXPORT:PostTranslate=_PostTranslate@12")

#ifdef __cplusplus
}
#endif