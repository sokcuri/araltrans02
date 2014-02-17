// ATPlugin.h : ATPlugin DLL의 기본 헤더 파일입니다.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH에 대해 이 파일을 포함하기 전에 'stdafx.h'를 포함합니다."
#endif

#include "resource.h"		// 주 기호입니다.
#include "FixLine.h"
#include "OptionSheet.h"

// Export Functions
extern "C" __declspec(dllexport) BOOL __stdcall OnPluginInit(HWND hSettingWnd, LPSTR cszOptionStringBuffer);
extern "C" __declspec(dllexport) BOOL __stdcall OnPluginOption();
extern "C" __declspec(dllexport) BOOL __stdcall OnPluginClose();
extern "C" __declspec(dllexport) BOOL __stdcall PreTranslate(LPCSTR cszInJapanese, LPSTR szOutJapanese, int nBufSize);
extern "C" __declspec(dllexport) BOOL __stdcall PostTranslate(LPCSTR cszInKorean, LPSTR szOutKorean, int nBufSize);

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

private:
	CFixLine m_cFixLine;
	bool m_bUseFixLine;
	bool m_bCanUse;
};
