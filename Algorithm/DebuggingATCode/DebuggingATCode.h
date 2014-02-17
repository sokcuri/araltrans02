// DebuggingATCode.h : main header file for the DebuggingATCode DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// Export Functions
extern "C" __declspec(dllexport) BOOL __stdcall OnPluginInit(HWND hSettingWnd, LPSTR cszOptionStringBuffer);
extern "C" __declspec(dllexport) BOOL __stdcall OnPluginOption();
extern "C" __declspec(dllexport) BOOL __stdcall OnPluginStart();
extern "C" __declspec(dllexport) BOOL __stdcall OnPluginStop();
extern "C" __declspec(dllexport) BOOL __stdcall OnPluginClose();


// CDebuggingATCodeApp
// See DebuggingATCode.cpp for the implementation of this class
//

class CDebuggingATCodeApp : public CWinApp
{
public:
	CDebuggingATCodeApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()


};
