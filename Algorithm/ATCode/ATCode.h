// ATCode.h : main header file for the ATCode DLL
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


// CATCodeApp
// See ATCode.cpp for the implementation of this class
//

class CATCodeApp : public CWinApp
{
public:
	CATCodeApp();

// Overrides
public:
	virtual BOOL InitInstance();
	virtual BOOL ExitInstance();

	DECLARE_MESSAGE_MAP()
};
