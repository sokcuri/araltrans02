// ATPlugin.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "ATPlugin.h"
#include "resource.h"

#include "OptionDlg.h"

#ifdef _UNICODE
#include "tstring.h"
#endif

// ATCApi 사용하려면 include
//#include "ATCApi.h"

// Util 사용하려면 include
#include "Util.h"

#include "DenyWord.h"

tstring g_strDebugFile;

/*
** 전역변수 선언부
*/

// 플러그인 버전
ATPLUGIN_VERSION g_pluginVer = {
	{ 0, 2, 20080704 },		// 플러그인 제작시 AT 버전
	{ 0, 1, 20080811 },		// 플러그인 버전
	"whoami",	// 제작자
	"DenyWord",	// 플러그인 이름
	"특정 단어 번역거부 필터"	// 플러그인 설명
};

// 설정창 윈도우 핸들
HWND g_hSettingWnd=NULL;

// 이 플러그인의 모듈 핸들 = 인스턴스 핸들
HINSTANCE g_hThisModule;

// 옵션 버퍼 - 옵션이 바뀌었을 때 여기에 써주면 아랄트랜스 바로가기에도 등록할 수 있다.
// 주의 - 옵션길이는 최고 4096 바이트.
LPSTR g_szOptionStringBuffer=NULL;

void ApplyOption();

// DLLMain
// 자체 리소스가 있는 경우 모듈 핸들을 알아오기 위해 필요
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			g_hThisModule = (HINSTANCE) hModule;
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}

// 플러그인 export 함수

// 공통
BOOL  __stdcall OnPluginInit(HWND hSettingWnd, LPSTR cszOptionStringBuffer)
{
	g_hSettingWnd = hSettingWnd;
	g_szOptionStringBuffer=cszOptionStringBuffer;

	// TODO : 플러그인의 초기화, 자원 확보, 옵션 분석등을 한다.

	// ATCApi 사용시
	// if ( !g_cATCApi.IsValid() ) return FALSE;

	// Util 의 옵션 파서 사용 예
	ATOPTION_ARRAY aOptions;
	GetATOptionsFromOptionString(cszOptionStringBuffer, aOptions);

	for (int i=0; i<aOptions.size(); i++)
	{
		if ( !lstrcmpi(aOptions[i].strKey.c_str(), _T("DEBUG")) )
			g_strDebugFile = aOptions[i].strValue;
		else if ( !lstrcmpi(aOptions[i].strKey.c_str(), _T("CUT")) )
			g_cDenyWord.SetCutSetting(_ttoi(aOptions[i].strValue.c_str()));
		else if ( !lstrcmpi(aOptions[i].strKey.c_str(), _T("PASS")) )
			g_cDenyWord.SetPassKanjiSetting(_ttoi(aOptions[i].strValue.c_str()));
		else if ( !lstrcmpi(aOptions[i].strKey.c_str(), _T("NOTKANJI")) )
			g_cDenyWord.SetNotKanjiSetting(true);
	}


	g_cDenyWord.Load();
	// 초기화 성공시 TRUE, 실패시 FALSE를 반환
	return TRUE;
}

BOOL  __stdcall OnPluginOption()
{
	if (g_hSettingWnd && IsWindow(g_hSettingWnd))
	{
		// TODO: 플러그인 옵션창 처리 및 옵션 조정을 한다.
		g_cDenyWord.Load();

		//다이얼로그
		if (DialogBox((HINSTANCE)g_hThisModule, MAKEINTRESOURCE(IDD_DIALOG1), g_hSettingWnd, OptionDialogProc) == IDOK)
		{
			ApplyOption();
		}
		
		// Util 의 옵션 파서 사용 예
		// ATOPTION_ARRAY aOptions;
		// ReadOptionsFromSomewhere(aOptions); // 어단가에서 옵션을 읽어온다
		// GetOptionStringFromATPluginArgs(aOptions, g_szOptionStringBuffer, 4096);	// 옵션을 문자열로 바꿔 아랄트랜스로 넘긴다.

	}
	return TRUE;
}
BOOL  __stdcall OnPluginClose()
{
	// 주의: 여기서 메모리 확보 등을 하면 에러 발생.
	// TODO: 플러그인 종료 준비, 자원 반환 등을 한다.

	return TRUE;
}
const ATPLUGIN_VERSION * __stdcall OnPluginVersion()
{
	// 플러그인 버전 반환

	return &g_pluginVer;
}

// 후킹 플러그인 전용
#ifdef ATPLUGIN_HOOKER
BOOL  __stdcall OnPluginStart()
{
	// TODO: 후킹 시작 작업.

	// 작업 성공시 TRUE, 실패시 FALSE 반환 
	return TRUE;
}
BOOL  __stdcall OnPluginStop()
{
	// 주의: 여기서 메모리 확보 등을 하면 에러 발생.
	// TODO: 후킹 종료 작업.

	// 작업 성공시 TRUE, 실패시 FALSE 반환 
	return TRUE;
}
#endif

// 번역 플러그인 전용
#ifdef ATPLUGIN_TRANSLATOR
BOOL  __stdcall Translate(LPCSTR cszJapanese, LPSTR szKorean, int nBufSize)
{
	// TODO: 번역작업을 한다.

	lstrcpyA(szKorean, cszJapanese);

	// 작업 성공시 TRUE, 실패시 FALSE 반환 
	return TRUE;
}
#endif

// 필터 플러그인 전용
#ifdef ATPLUGIN_FILTER
BOOL  __stdcall PreTranslate(LPCSTR cszInJapanese, LPSTR szOutJapanese, int nBufSize)
{
	// TODO: 번역 전처리를 한다.
	// 특별한 처리를 하지 않더라도 문자열을 복사해 돌려주어야 한다.
	lstrcpyA(szOutJapanese, cszInJapanese);
	
	// 작업 성공시 TRUE, 실패시 FALSE 반환 

	if (g_cDenyWord.IsDenied(cszInJapanese))
		return FALSE;
	return TRUE;
}

BOOL  __stdcall PostTranslate(LPCSTR cszInKorean, LPSTR szOutKorean, int nBufSize)
{
	// TODO: 번역 후처리를 한다.
	// 특별한 처리를 하지 않더라도 문자열을 복사해 돌려주어야 한다.
	lstrcpyA(szOutKorean, cszInKorean);
	
	// 작업 성공시 TRUE, 실패시 FALSE 반환 
	return TRUE;
}
#endif



void ApplyOption()
{
	ATOPTION_ARRAY aOptions;
	ATOPTION_PAIR option;
	tstring strBoundary;

	if (!g_strDebugFile.empty())
	{
		option.strKey=_T("DEBUG");
		option.strValue = g_strDebugFile;
		aOptions.push_back(option);
	}
	if (g_cDenyWord.GetPassKanjiSetting()>0)
	{
		TCHAR szbuf[10];
		option.strKey=_T("PASS");
		option.strValue=_itot(g_cDenyWord.GetPassKanjiSetting(),szbuf,10);
		aOptions.push_back(option);
	}
	if (g_cDenyWord.GetNotKanjiSetting())
	{
		option.strKey=_T("NOTKANJI");
		option.strValue.erase();
		aOptions.push_back(option);
	}
	if (g_cDenyWord.GetCutSetting()>0)
	{
		TCHAR szbuf[10];
		option.strKey=_T("CUT");
		option.strValue=_itot(g_cDenyWord.GetCutSetting(),szbuf,10);
		aOptions.push_back(option);
	}


	GetOptionStringFromATOptions(aOptions, g_szOptionStringBuffer, 4096);

}
