// JSCustom.h : JSCustom DLL의 기본 헤더 파일입니다.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH에 대해 이 파일을 포함하기 전에 'stdafx.h'를 포함합니다."
#endif

#include "resource.h"		// 주 기호입니다.


extern "C" BOOL PASCAL EXPORT OnPluginInit(HWND hSettingWnd, LPSTR cszOptionStringBuffer);
extern "C" BOOL PASCAL EXPORT OnPluginOption();
extern "C" BOOL PASCAL EXPORT OnPluginStart();
extern "C" BOOL PASCAL EXPORT OnPluginStop();
extern "C" BOOL PASCAL EXPORT OnPluginClose();

// CJSCustomApp
// 이 클래스의 구현을 보려면 JSCustom.cpp를 참조하십시오.
//

class CJSCustomApp : public CWinApp
{
public:
	BOOL OnPluginInit(HWND hSettingWnd, LPSTR szOptionStringBuffer);
	BOOL OnPluginOption();
	BOOL OnPluginClose();
	BOOL OnPluginStart();
	BOOL OnPluginStop();

	CJSCustomApp();

// 재정의입니다.
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
