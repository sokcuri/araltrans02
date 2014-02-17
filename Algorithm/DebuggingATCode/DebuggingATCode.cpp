// DebuggingATCode.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "DebuggingATCode.h"
#include "ATTextArgMgr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CATTextArgMgr g_objATTextArgMgr;


extern "C" __declspec(dllexport) BOOL __stdcall OnPluginInit(HWND hSettingWnd, LPSTR cszOptionStringBuffer)
{
	return g_objATTextArgMgr.Init(hSettingWnd, cszOptionStringBuffer);
}

extern "C" __declspec(dllexport) BOOL __stdcall OnPluginOption()
{
	return g_objATTextArgMgr.Option();
}

extern "C" __declspec(dllexport) BOOL __stdcall OnPluginStart()
{
	return g_objATTextArgMgr.Start();
}

extern "C" __declspec(dllexport) BOOL __stdcall OnPluginStop()
{
	return g_objATTextArgMgr.Stop();
}

extern "C" __declspec(dllexport) BOOL __stdcall OnPluginClose()
{
	return g_objATTextArgMgr.Close();
}




int MyWideCharToMultiByte(
						  UINT CodePage, 
						  DWORD dwFlags, 
						  LPCWSTR lpWideCharStr, 
						  int cchWideChar, 
						  LPSTR lpMultiByteStr, 
						  int cbMultiByte, 
						  LPCSTR lpDefaultChar, 
						  LPBOOL lpUsedDefaultChar 
						  )
{
	int nRetVal = 0;

	if( g_objATTextArgMgr.m_sTextFunc.pfnOrigWideCharToMultiByte )
	{
		nRetVal = g_objATTextArgMgr.m_sTextFunc.pfnOrigWideCharToMultiByte(
			CodePage, 
			dwFlags, 
			lpWideCharStr, 
			cchWideChar, 
			lpMultiByteStr, 
			cbMultiByte, 
			lpDefaultChar, 
			lpUsedDefaultChar 
			);
	}
	else
	{
		nRetVal = ::WideCharToMultiByte(
			CodePage, 
			dwFlags, 
			lpWideCharStr, 
			cchWideChar, 
			lpMultiByteStr, 
			cbMultiByte, 
			lpDefaultChar, 
			lpUsedDefaultChar 
			);
	}

	return nRetVal;
}

int MyMultiByteToWideChar(
						  UINT CodePage, 
						  DWORD dwFlags, 
						  LPCSTR lpMultiByteStr, 
						  int cbMultiByte, 
						  LPWSTR lpWideCharStr, 
						  int cchWideChar 
						  )
{
	int nRetVal = 0;

	if( g_objATTextArgMgr.m_sTextFunc.pfnOrigMultiByteToWideChar )
	{
		nRetVal = g_objATTextArgMgr.m_sTextFunc.pfnOrigMultiByteToWideChar(
			CodePage, 
			dwFlags, 
			lpMultiByteStr, 
			cbMultiByte, 
			lpWideCharStr, 
			cchWideChar 
			);
	}
	else
	{
		nRetVal = ::MultiByteToWideChar(
			CodePage, 
			dwFlags, 
			lpMultiByteStr, 
			cbMultiByte, 
			lpWideCharStr, 
			cchWideChar 
			);
	}

	return nRetVal;
}



//
//TODO: If this DLL is dynamically linked against the MFC DLLs,
//		any functions exported from this DLL which call into
//		MFC must have the AFX_MANAGE_STATE macro added at the
//		very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//


// CDebuggingATCodeApp

BEGIN_MESSAGE_MAP(CDebuggingATCodeApp, CWinApp)
END_MESSAGE_MAP()


// CDebuggingATCodeApp construction

CDebuggingATCodeApp::CDebuggingATCodeApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CDebuggingATCodeApp object

CDebuggingATCodeApp theApp;


// CDebuggingATCodeApp initialization

BOOL CDebuggingATCodeApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}

