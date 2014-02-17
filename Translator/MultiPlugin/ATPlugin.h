#ifndef __ATPLUGIN_H__
#define __ATPLUGIN_H__

/*
** 플러그인 기본 세팅
*/

// 후킹 플러그인인 경우 define
//#define ATPLUGIN_HOOKER

// 번역 플러그인인 경우 define
#define ATPLUGIN_TRANSLATOR

/*
** 플러그인 기본 세팅 끝
*/

// ATPLUGIN_VERSION
// 이후 플러그인 형식 변경시 호환성 문제 해결을 위한 버전 정보.
// 0.2.20080518 공식 플러그인 규격은 아니지만 나중을 위해 넣어둠.
typedef struct _tagATPLUGIN_VERSION {
	struct {
		WORD Major;	// 메이저 버전. 현재 0
		WORD Minor;	// 마이너 버전. 현재 2
		DWORD BuildDate;	// 빌드 일자. 현재 20080518
	} ATVersion;	// 플러그인 제작 당시의 AralTrans 버전 정보

	struct {
		WORD Major;	// 메이저 버전
		WORD Minor;	// 마이너 버전
		DWORD BuildDate;	// 빌드 일자
	} PluginVersion;	// 플러그인 자체 버전 정보
	
	char szAuthor [16];	// 제작자
	char szPluginName [16];	// 플러그인 이름
	char szDescription [32];	// 플러그인 설명

} ATPLUGIN_VERSION;

/*
** 전역 변수
*/

// 플러그인 버전 정보
extern ATPLUGIN_VERSION g_pluginVer;
// 설정창 윈도우 핸들
extern HWND g_hSettingWnd;

extern HMODULE g_hThisModule;

extern const TCHAR g_szPluginName[];

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
const ATPLUGIN_VERSION * __stdcall OnPluginVersion();

// 후킹 플러그인 전용
#ifdef ATPLUGIN_HOOKER
BOOL __stdcall OnPluginStart();
BOOL __stdcall OnPluginStop();
#endif

// 번역 플러그인 전용
#ifdef ATPLUGIN_TRANSLATOR
BOOL __stdcall Translate(LPCSTR cszJapanese, LPSTR szKorean, int nBufSize);
#endif

// export 를 위한 링커 옵션
// 공통
#pragma comment(linker, "/EXPORT:OnPluginInit=_OnPluginInit@8")
#pragma comment(linker, "/EXPORT:OnPluginOption=_OnPluginOption@0")
#pragma comment(linker, "/EXPORT:OnPluginClose=_OnPluginClose@0")
#pragma comment(linker, "/EXPORT:OnPluginVersion=_OnPluginVersion@0")

// 후킹 플러그인 전용
#ifdef ATPLUGIN_HOOKER
#pragma comment(linker, "/EXPORT:OnPluginStart=_OnPluginStart@0")
#pragma comment(linker, "/EXPORT:OnPluginStop=_OnPluginStop@0")
#endif

// 번역 플러그인 전용
#ifdef ATPLUGIN_TRANSLATOR
#pragma comment(linker, "/EXPORT:Translate=_Translate@12")
#endif

#ifdef __cplusplus
}
#endif

#endif //__ATPLUGIN_H__