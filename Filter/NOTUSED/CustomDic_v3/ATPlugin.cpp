// ATPlugin.cpp : 해당 DLL의 초기화 루틴을 정의합니다.
//

#include "stdafx.h"
#include "ATPlugin.h"
#include "tstring.h"
#include "CustomDic2.h"
#include "Dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Util 사용하려면 include
#include "Util.h"


/*
** 전역변수 선언부
*/

int UseCDic=false;
int UseGDic=false;

bool bInit=false;

bool g_bNoMargin=false;

// 플러그인 버전
ATPLUGIN_VERSION g_pluginVer = {
	{ 0, 2, 20080704 },		// 플러그인 제작시 AT 버전
	{ 0, 1, 20080810 },		// 플러그인 버전
	"제작자",	// 제작자
	"플러그인 이름",	// 플러그인 이름
	"아랄트랜스 플러그인"	// 플러그인 설명
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
void Init();

// 유일한 CATPluginApp 개체입니다.

CATPluginApp theApp;
CCustomDic2	* g_pCustomDic=NULL;
CDlg *g_pDlg=NULL;

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

	// TODO : 플러그인의 초기화, 자원 확보, 옵션 분석등을 한다.

	// ATCApi 사용시
	// if ( !g_cATCApi.IsValid() ) return FALSE;

	// Util 의 옵션 파서 사용 예
	ATOPTION_ARRAY aOptions;
	GetATOptionsFromOptionString(cszOptionStringBuffer, aOptions);

	for (size_t i=0; i<aOptions.size(); i++)
	{
		if ( !lstrcmpi(aOptions[i].strKey.c_str(), L"CDic"))
		{
			UseCDic=true;
		}
		else if ( !lstrcmpi(aOptions[i].strKey.c_str(), L"GDic"))
		{
			UseGDic=true;
		}
		else if(!lstrcmpi(aOptions[i].strKey.c_str(), L"NoMargin"))
		{
			g_bNoMargin=true;
		}
	}
	// 초기화 성공시 TRUE, 실패시 FALSE를 반환
	return TRUE;
}

BOOL CATPluginApp::OnPluginClose()
{
	// 주의: 여기서 메모리 확보 등을 하면 에러 발생.
	// TODO: 플러그인 종료 준비, 자원 반환 등을 한다.
	if(bInit)
	{
		if(g_pCustomDic)
		{
			g_pCustomDic->End();
			delete g_pCustomDic;
		}
		if(g_pDlg)
		{
			delete g_pDlg;
		}
	}
	return TRUE;
}

BOOL CATPluginApp::OnPluginOption()
{
	if(!bInit)
	{
		bInit=true;
		Init();
	}
	if (g_hSettingWnd && IsWindow(g_hSettingWnd))
	{
		// TODO: 플러그인 옵션창 처리 및 옵션 조정을 한다.
		g_pDlg->UseCDic=UseCDic;
		g_pDlg->UseGDic=UseGDic;
		g_pDlg->nNoMargin=g_bNoMargin;

		if(g_pDlg->DoModal()==IDOK)
		{
			bool Reset=false;

			if(UseCDic!=g_pDlg->UseCDic||UseGDic!=g_pDlg->UseGDic)
				Reset=true;

			UseCDic=g_pDlg->UseCDic;
			UseGDic=g_pDlg->UseGDic;
			g_bNoMargin=g_pDlg->nNoMargin==0?false:true;

			if(Reset)
			{
				Reset=false;
				g_pCustomDic->FileClear();
				if(UseGDic)g_pCustomDic->AddDic(1,L"");
				if(UseCDic)g_pCustomDic->AddDic(2,L"");
				g_pCustomDic->SetMarginMode(g_bNoMargin);
			}


			ATOPTION_ARRAY aOptions;
			ATOPTION_PAIR option;

			if(UseCDic)
			{
				option.strKey=L"CDic";
				option.strValue=L"";
				aOptions.push_back(option);
			}

			if(UseGDic)
			{
				option.strKey=L"GDic";
				option.strValue=L"";
				aOptions.push_back(option);
			}

			if(g_bNoMargin)
			{
				option.strKey=L"NoMargin";
				option.strValue=L"";
				aOptions.push_back(option);
			}
		
			GetOptionStringFromATOptions(aOptions, g_szOptionStringBuffer, 4096);
		}

	}
	return TRUE;
}

BOOL CATPluginApp::PreTranslate(LPCSTR cszInJapanese, LPSTR szOutJapanese, int nBufSize)
{
	// TODO: 번역 전처리를 한다.
	// 특별한 처리를 하지 않더라도 문자열을 복사해 돌려주어야 한다.
	if(!bInit)
	{
		bInit=true;
		Init();
		Sleep(300);
	}
	if(UseCDic||UseGDic)
		g_pCustomDic->PreCustomDic(szOutJapanese,cszInJapanese);
	else
		lstrcpyA(szOutJapanese, cszInJapanese);
	
	// 작업 성공시 TRUE, 실패시 FALSE 반환 
	return TRUE;
}

BOOL CATPluginApp::PostTranslate(LPCSTR cszInKorean, LPSTR szOutKorean, int nBufSize)
{
	// TODO: 번역 후처리를 한다.
	// 특별한 처리를 하지 않더라도 문자열을 복사해 돌려주어야 한다.
	if(UseCDic||UseGDic)
		g_pCustomDic->PostCustomDic(szOutKorean,cszInKorean);
	else
		lstrcpyA(szOutKorean, cszInKorean);
	
	// 작업 성공시 TRUE, 실패시 FALSE 반환 
	return TRUE;
}

void Init()
{
	g_pCustomDic=new CCustomDic2();
	g_pCustomDic->SetMarginMode(g_bNoMargin);
	g_pDlg=new CDlg();

	g_pCustomDic->FileClear();
	if(UseGDic)g_pCustomDic->AddDic(1,L"");
	if(UseCDic)g_pCustomDic->AddDic(2,L"");

	g_pCustomDic->Init();
	
}