// ATPlugin.h : ATPlugin DLL의 기본 헤더 파일입니다.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH에 대해 이 파일을 포함하기 전에 'stdafx.h'를 포함합니다."
#endif

#include "resource.h"		// 주 기호입니다.

// ATPLUGIN_VERSION
// 이후 플러그인 형식 변경시 호환성 문제 해결을 위한 버전 정보.
// 0.2.20080518 공식 플러그인 규격은 아니지만 나중을 위해 넣어둠.
typedef struct _tagATPLUGIN_VERSION {
	struct {
		WORD Major;	// 메이저 버전. 현재 0
		WORD Minor;	// 마이너 버전. 현재 2
		DWORD BuildDate;	// 빌드 일자. 현재 20080704
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

const ATPLUGIN_VERSION * _OnPluginVersion();

// Export Functions
extern "C" __declspec(dllexport) BOOL __stdcall OnPluginInit(HWND hSettingWnd, LPSTR cszOptionStringBuffer);
extern "C" __declspec(dllexport) BOOL __stdcall OnPluginOption();
extern "C" __declspec(dllexport) BOOL __stdcall OnPluginClose();
extern "C" __declspec(dllexport) BOOL __stdcall PreTranslate(LPCSTR cszInJapanese, LPSTR szOutJapanese, int nBufSize);
extern "C" __declspec(dllexport) BOOL __stdcall PostTranslate(LPCSTR cszInKorean, LPSTR szOutKorean, int nBufSize);
extern "C" __declspec(dllexport) const ATPLUGIN_VERSION * __stdcall OnPluginVersion();

// 플러그인 버전 정보
extern ATPLUGIN_VERSION g_pluginVer;


// 설정창 윈도우 핸들
extern HWND g_hSettingWnd;

// 이 플러그인의 옵션
extern LPSTR g_szOptionStringBuffer;

// CATPluginApp
// 이 클래스의 구현을 보려면 ATPlugin.cpp를 참조하십시오.
//

class CATPluginApp : public CWinApp
{
public:
	CATPluginApp();

	BOOL OnPluginInit(HWND hSettingWnd, LPSTR cszOptionStringBuffer);
	BOOL OnPluginOption();
	BOOL OnPluginClose();
	BOOL PreTranslate(LPCSTR cszInJapanese, LPSTR szOutJapanese, int nBufSize);
	BOOL PostTranslate(LPCSTR cszInKorean, LPSTR szOutKorean, int nBufSize);

// 재정의입니다.
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
