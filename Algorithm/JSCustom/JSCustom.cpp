// JSCustom.cpp : 해당 DLL의 초기화 루틴을 정의합니다.
//

#include "stdafx.h"
#include "JSCustom.h"
#include "v8Wrapper.h"
#include "ATJSAPI.h"

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

// CJSCustomApp

BEGIN_MESSAGE_MAP(CJSCustomApp, CWinApp)
END_MESSAGE_MAP()


// CJSCustomApp 생성
// 유일한 CJSCustomApp 개체입니다.

CJSCustomApp theApp;

extern "C" BOOL PASCAL EXPORT OnPluginInit(HWND hSettingWnd, LPSTR szOptionStringBuffer)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return theApp.OnPluginInit(hSettingWnd,szOptionStringBuffer);
}

extern "C" BOOL PASCAL EXPORT OnPluginOption()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return theApp.OnPluginOption();
}

extern "C" BOOL PASCAL EXPORT OnPluginClose()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return theApp.OnPluginClose();
}

extern "C" BOOL PASCAL EXPORT OnPluginStart()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return theApp.OnPluginStart();
}

extern "C" BOOL PASCAL EXPORT OnPluginStop()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return theApp.OnPluginStop();
}


CJSCustomApp::CJSCustomApp()
{
	// TODO: 여기에 생성 코드를 추가합니다.
	// InitInstance에 모든 중요한 초기화 작업을 배치합니다.
}


// CJSCustomApp 초기화

BOOL CJSCustomApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}

BOOL CJSCustomApp::OnPluginInit(HWND hSettingWnd, LPSTR szOptionStringBuffer){
	return TRUE;
}

BOOL CJSCustomApp::OnPluginOption(){
	return TRUE;
}

BOOL CJSCustomApp::OnPluginStart(){
	//할 일이 읍따
	return TRUE;
}

BOOL CJSCustomApp::OnPluginStop(){
	//할 일이 읍따
	return TRUE;
}

BOOL CJSCustomApp::OnPluginClose(){
	//v8Wrapper의 자동 소멸자가 알아서 한다[...]
	//결론 : 할 일이 읍따
	return TRUE;
}
