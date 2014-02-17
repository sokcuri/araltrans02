// FixControlChar.cpp : 해당 DLL의 초기화 루틴을 정의합니다.
//

#include "stdafx.h"
#include "FixControlChar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO: 이 DLL이 MFC DLL에 대해 동적으로 링크되어 있는 경우
//		MFC로 호출되는 이 DLL에서 내보내지는 모든 함수의
//		시작 부분에 AFX_MANAGE_STATE 매크로가
//		들어 있어야 합니다.
//
//		예:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// 일반적인 함수 본문은 여기에 옵니다.
//		}
//
//		이 매크로는 MFC로 호출하기 전에
//		각 함수에 반드시 들어 있어야 합니다.
//		즉, 매크로는 함수의 첫 번째 문이어야 하며 
//		개체 변수의 생성자가 MFC DLL로
//		호출할 수 있으므로 개체 변수가 선언되기 전에
//		나와야 합니다.
//
//		자세한 내용은
//		MFC Technical Note 33 및 58을 참조하십시오.
//



// CFixControlCharApp

BEGIN_MESSAGE_MAP(CFixControlCharApp, CWinApp)
END_MESSAGE_MAP()

// CFixControlCharApp 생성

CFixControlCharApp::CFixControlCharApp()
{
	// TODO: 여기에 생성 코드를 추가합니다.
	// InitInstance에 모든 중요한 초기화 작업을 배치합니다.
}


// 유일한 CFixControlCharApp 개체입니다.

CFixControlCharApp theApp;

// Export Functions
extern "C" __declspec(dllexport) BOOL __stdcall OnPluginInit(HWND hSettingWnd, LPSTR cszSettingStringBuffer)
{
	return theApp.OnPluginInit(hSettingWnd, cszSettingStringBuffer);
}

extern "C" __declspec(dllexport) BOOL __stdcall OnPluginOption()
{
	return theApp.OnPluginOption();
}

extern "C" __declspec(dllexport) BOOL __stdcall OnPluginClose()
{
	return theApp.OnPluginClose();
}

extern "C" __declspec(dllexport) BOOL __stdcall PreTranslate(LPCSTR cszInJapanese, LPSTR szOutJapanese, int nBufSize)
{
	return theApp.PreTranslate(cszInJapanese, szOutJapanese, nBufSize);
}

extern "C" __declspec(dllexport) BOOL __stdcall PostTranslate(LPCSTR cszInKorean, LPSTR szOutKorean, int nBufSize)
{
	return theApp.PostTranslate(cszInKorean, szOutKorean, nBufSize);
}

extern "C" __declspec(dllexport) const ATPLUGIN_VERSION * __stdcall OnPluginVersion()
{
	// 플러그인 버전 반환

	return _OnPluginVersion();
}

const ATPLUGIN_VERSION * _OnPluginVersion()
{
	// 플러그인 버전 반환

	return &g_pluginVer;
}




// CFixControlCharApp 초기화

BOOL CFixControlCharApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}

BOOL CFixControlCharApp::OnPluginInit(HWND hSettingWnd, LPSTR cszSettingStringBuffer)
{
	return TRUE;
}

BOOL CFixControlCharApp::OnPluginClose()
{
	return TRUE;
}

BOOL CFixControlCharApp::OnPluginOption()
{
	return TRUE;
}

BOOL CFixControlCharApp::PreTranslate(LPCSTR cszInJapanese, LPSTR szOutJapanese, int nBufSize)
{
	return TRUE;
}

BOOL CFixControlCharApp::PostTranslate(LPCSTR cszInKorean, LPSTR szOutKorean, int nBufSize)
{
	return TRUE;
}