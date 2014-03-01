// ATPlugin - 아랄트랜스 플러그인 규격을 따르는 기본 DLL 모듈
//

#include "stdafx.h"
#include "ATPlugin.h"

#include "ATPluginFrame/Debug.h"

// MBCS <-> UNICODE 변환이 필요하면 include
//#include "ATPluginFrame/Libs/tstring.h"

// ATCApi 사용하려면 include
//#include "ATPluginFrame/Libs/ATCApi.h"

// ATOptionParser 사용하려면 include
//#include "ATPluginFrame/Libs/ATOptionParser.h"

// Util 사용하려면 include
//#include "ATPluginFrame/Libs/Util.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// 전역 변수 선언부

// 유일한 CATPluginApp 개체입니다.
CATPluginApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CATPluginApp

#ifdef USE_MFC
BEGIN_MESSAGE_MAP(CATPluginApp, CWinApp)
	//{{AFX_MSG_MAP(CATPluginApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// USE_MFC

/////////////////////////////////////////////////////////////////////////////
// CATPluginApp 생성

CATPluginApp::CATPluginApp()
{
	// TODO: 여기에 생성 코드를 추가합니다.
	// InitInstance에 모든 중요한 초기화 작업을 배치합니다.

	// 여기쯤에서 PLUGIN_INFO 구조체를 채워넣는다.
	// 물론 기본적으로 프레임워크가 채워넣지만 플러그인 이름과 다운로드 URL
	// 등은 직접 넣어주는 것이 좋다.

	// PLUGIN_INFO 구조체의 멤버
	//	int			cch;				// PLUGIN_INFO 구조체 사이즈
	//	int			nIconID;			// Icon Resource ID
	//	wchar_t		wszPluginType[16];	// 플러그인 타입
	//	wchar_t		wszPluginName[64];	// 플러그인 이름
	//	wchar_t		wszDownloadUrl[256];// 플러그인 다운로드 URL
	m_sPluginInfo.nIconID = IDI_PLUGINICON;
	lstrcpyn(m_sPluginInfo.wszPluginName, _T("KoFilter2"), 64);
	lstrcpyn(m_sPluginInfo.wszDownloadUrl, _T("http://"), 256);
}

/////////////////////////////////////////////////////////////////////////////
// CATPluginApp 초기화
BOOL CATPluginApp::InitInstance() 
{
	// TODO: Add your specialized code here and/or call the base class

	return CATPluginFrameApp::InitInstance();
}

// 공통

BOOL CATPluginApp::OnPluginInit()
{
	CATPluginFrameApp::OnPluginInit();

	// TODO : 플러그인의 초기화, 자원 확보, 옵션 분석등을 한다.

	// ATCApi 사용시
	// if ( !g_cATCApi.Init() ) return FALSE;

	// ATOptionParser 의 옵션 파서 사용 예
	// ATOPTION_ARRAY aOptions;
	// GetATOptionsFromOptionString(m_sOptionString, aOptions);

	// 초기화 성공시 TRUE, 실패시 FALSE를 반환
	return TRUE;
}

BOOL CATPluginApp::OnPluginOption()
{
	if (m_hAralWnd && IsWindow(m_hAralWnd))
	{
		// TODO: 플러그인 옵션창 처리 및 옵션 조정을 한다.

		MessageBox(m_hAralWnd, _T("사용가능한 옵션은 없습니다"), m_sPluginInfo.wszPluginName, MB_OK);

		// ATOptionParser 의 옵션 파서 사용 예
		// ATOPTION_ARRAY aOptions;
		// ReadOptionsFromSomewhere(aOptions); // 어딘가에서 옵션을 읽어온다
		// GetOptionStringFromATOptions(aOptions, m_sOptionString, 4096);	// 옵션을 문자열로 바꿔 아랄트랜스로 넘긴다.

	}
	return TRUE;
}

BOOL CATPluginApp::OnPluginClose()
{
	// 주의: 여기서 메모리 확보 등을 하면 에러 발생.
	// TODO: 플러그인 종료 준비, 자원 반환 등을 한다.

	return TRUE;
}

///////////////////////////////////////////////////////////////////////
// 플러그인 종류에 따라 Override 되는 플러그인 관련 함수의 예
// 

// 후킹 플러그인 전용
/*
BOOL CATPluginApp::OnPluginStart()
{
	// TODO: 후킹 시작 작업.

	// 작업 성공시 TRUE, 실패시 FALSE 반환 
	return TRUE;
}
//*/

/*
BOOL CATPluginApp::OnPluginStop()
{
	// 주의: 여기서 메모리 확보 등을 하면 에러 발생.
	// TODO: 후킹 종료 작업.

	// 작업 성공시 TRUE, 실패시 FALSE 반환 
	return TRUE;
}
//*/

// 번역 플러그인 전용
/*
BOOL CATPluginApp::Translate(LPCSTR cszJapanese, LPSTR szKorean, int nBufSize)
{
	// TODO: 번역작업을 한다.

	lstrcpyA(szKorean, cszJapanese);

	// 작업 성공시 TRUE, 실패시 FALSE 반환 
	return TRUE;
}
//*/

// 필터 플러그인 전용
/*
BOOL CATPluginApp::PreTranslate(LPCSTR cszInJapanese, LPSTR szOutJapanese, int nBufSize)
{
	// TODO: 번역 전처리를 한다.
	// 특별한 처리를 하지 않더라도 문자열을 복사해 돌려주어야 한다.
	lstrcpyA(szOutJapanese, cszInJapanese);
	
	// 작업 성공시 TRUE, 실패시 FALSE 반환 
	return TRUE;
}
//*/
/*
BOOL CATPluginApp::PostTranslate(LPCSTR cszInKorean, LPSTR szOutKorean, int nBufSize)
{
	// TODO: 번역 후처리를 한다.
	// 특별한 처리를 하지 않더라도 문자열을 복사해 돌려주어야 한다.
	lstrcpyA(szOutKorean, cszInKorean);
	
	// 작업 성공시 TRUE, 실패시 FALSE 반환 
	return TRUE;
}
//*/

/*
BOOL CATPluginApp::OnObjectInit(TRANSLATION_OBJECT* pTransObj)
{
	if ( !CATPluginFrameApp::OnObjectInit(pTransObj) ) return FALSE;
	
	// TODO: 추가적인 번역 객체 초기화 작업을 한다.

	return TRUE;
}
//*/
/*
BOOL CATPluginApp::OnObjectClose(TRANSLATION_OBJECT* pTransObj)
{
	// TODO: 추가적인 번역 객체 삭제 작업을 한다.

	if ( !CATPluginFrameApp::OnObjectClose(pTransObj) ) return FALSE;
	return TRUE;
}
//*/
/*
BOOL CATPluginApp::OnObjectMove(TRANSLATION_OBJECT* pTransObj)
{
	if ( !CATPluginFrameApp::OnObjectMove(pTransObj) ) return FALSE;
	
	// TODO: 추가적인 번역 객체 순서변경 작업을 한다.	
	
	return TRUE;
}
//*/
/*
BOOL CATPluginApp::OnObjectOption(TRANSLATION_OBJECT* pTransObj)
{
	// TODO: 추가적인 번역 객체 옵션처리 작업을 한다.


	if ( !CATPluginFrameApp::OnObjectOption(pTransObj) ) return FALSE;
	return TRUE;
}
//*/
