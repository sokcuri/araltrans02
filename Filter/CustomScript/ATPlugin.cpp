// ATPlugin.cpp : 해당 DLL의 초기화 루틴을 정의합니다.
//

#include "stdafx.h"
#include "ATPlugin.h"
#include "tstring.h"
#include "CustomScript.h"
#include "CSDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Util 사용하려면 include
#include "Util.h"


/*
** 전역변수 선언부
*/
CCustomScript CustomScript;
CCSDlg CSDlg;

bool isWrite=false;
bool ScriptChecked=false;
bool Cached=false;
char TempJpn[4096]="";
bool AlwaysTrans=false;

// 플러그인 버전
ATPLUGIN_VERSION g_pluginVer = {
	{ 0, 2, 20080704 },		// 플러그인 제작시 AT 버전
	{ 0, 1, 20080810 },		// 플러그인 버전
	"Hide_D君",	// 제작자
	"CustomScript",	// 플러그인 이름
	"사용자 대본 플러그인"	// 플러그인 설명
};

// 설정창 윈도우 핸들
HWND g_hSettingWnd=NULL;

// 이 플러그인의 모듈 핸들 = 인스턴스 핸들
HINSTANCE g_hThisModule;

// 옵션 버퍼 - 옵션이 바뀌었을 때 여기에 써주면 아랄트랜스 바로가기에도 등록할 수 있다.
// 주의 - 옵션길이는 최고 4096 바이트.
LPSTR g_szOptionStringBuffer=NULL;

// CATPluginApp

BEGIN_MESSAGE_MAP(CATPluginApp, CWinApp)
END_MESSAGE_MAP()

// CATPluginApp 생성

CATPluginApp::CATPluginApp()
{
	// TODO: 여기에 생성 코드를 추가합니다.
	// InitInstance에 모든 중요한 초기화 작업을 배치합니다.
}


// 유일한 CATPluginApp 개체입니다.

CATPluginApp theApp;

// Export Functions
extern "C" __declspec(dllexport) BOOL __stdcall OnPluginInit(HWND hSettingWnd, LPSTR cszOptionStringBuffer)
{
	return theApp.OnPluginInit(hSettingWnd, cszOptionStringBuffer);
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




// CATPluginApp 초기화

BOOL CATPluginApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}

BOOL CATPluginApp::OnPluginInit(HWND hSettingWnd, LPSTR cszOptionStringBuffer)
{
	g_hSettingWnd = hSettingWnd;
	g_szOptionStringBuffer=cszOptionStringBuffer;

	ATOPTION_ARRAY aOptions;
	GetATOptionsFromOptionString(cszOptionStringBuffer, aOptions);

	for (ATOPTION_ARRAY::size_type i=0; i<aOptions.size(); i++)
	{
		if ( !lstrcmpi(aOptions[i].strKey.c_str(), _T("Write")) )
			isWrite=true;
		else if ( !lstrcmpi(aOptions[i].strKey.c_str(), _T("Read")) )
			isWrite=false;
		else if ( !lstrcmpi(aOptions[i].strKey.c_str(), _T("Pass")) )
			CustomScript.SetWritePassN(_ttoi(aOptions[i].strValue.c_str()));
		else if ( !lstrcmpi(aOptions[i].strKey.c_str(), _T("Cache")) )
			Cached=true;
		else if ( !lstrcmpi(aOptions[i].strKey.c_str(), _T("Always")) )
			AlwaysTrans=true;
	}

	if(!isWrite)
	{
		CustomScript.InitR();
	}
	else
	{
		CustomScript.InitW();
	}

	return TRUE;
}

BOOL CATPluginApp::OnPluginClose()
{
	// 주의: 여기서 메모리 확보 등을 하면 에러 발생.
	// TODO: 플러그인 종료 준비, 자원 반환 등을 한다.
	return TRUE;
}

BOOL CATPluginApp::OnPluginOption()
{
	if (g_hSettingWnd && IsWindow(g_hSettingWnd))
	{
		// TODO: 플러그인 옵션창 처리 및 옵션 조정을 한다
		CustomScript.FileClose();

		CSDlg.SetisWrite(isWrite);
		CSDlg.SetPassN(CustomScript.GetWritePassN());
		CSDlg.SetListFilename(ListFileName);
		CSDlg.Cached=Cached;
		CSDlg.Always=AlwaysTrans;

		CSDlg.Init();

		if(CSDlg.DoModal()==IDOK)
		{
			isWrite=CSDlg.GetisWrite();
			CustomScript.SetWritePassN(CSDlg.GetPassN());
			Cached=CSDlg.Cached;
			AlwaysTrans = CSDlg.Always;
			
			if(!isWrite)
			{
				CustomScript.InitR();
				sprintf_s(g_szOptionStringBuffer,4096,"Read");

				if(AlwaysTrans)
				{
					strcat_s(g_szOptionStringBuffer,4096,",Always");
				}
			}



		}

		if(isWrite)
		{
				CustomScript.InitW();
				sprintf_s(g_szOptionStringBuffer,4096,"Write,Pass(%d)",CSDlg.GetPassN());
				if(Cached)
					strcat_s(g_szOptionStringBuffer,4096,",Cache");
		}
		
	}
	return TRUE;
}

BOOL CATPluginApp::PreTranslate(LPCSTR cszInJapanese, LPSTR szOutJapanese, int nBufSize)
{
	// TODO: 번역 전처리를 한다.
	// 특별한 처리를 하지 않더라도 문자열을 복사해 돌려주어야 한다.
	ScriptChecked=false;

	if((!isWrite || (isWrite&&Cached)) && CustomScript.CheckJpnLine(cszInJapanese)>=0)
	{
		ScriptChecked=true;
		szOutJapanese[0]='\0';
		if(AlwaysTrans)
			lstrcpyA(szOutJapanese, cszInJapanese);
	}
	else
	{
		lstrcpyA(szOutJapanese, cszInJapanese);
	}
	
	if(isWrite)
		strcpy_s(TempJpn,4096,cszInJapanese);
	// 작업 성공시 TRUE, 실패시 FALSE 반환 
	return TRUE;
}

BOOL CATPluginApp::PostTranslate(LPCSTR cszInKorean, LPSTR szOutKorean, int nBufSize)
{
	// TODO: 번역 후처리를 한다.
	// 특별한 처리를 하지 않더라도 문자열을 복사해 돌려주어야 한다.
	if(ScriptChecked)
	{
		CustomScript.GetKorLine(szOutKorean);
	}
	else
	{
		lstrcpyA(szOutKorean, cszInKorean);
	}

	if(isWrite)
			CustomScript.WriteScript(TempJpn,cszInKorean);
	// 작업 성공시 TRUE, 실패시 FALSE 반환 
	return TRUE;
}