// ATPlugin - 아랄트랜스 플러그인 규격을 따르는 기본 DLL 모듈
//

#ifndef __ATPLUGIN_H__
#define __ATPLUGIN_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"		// 주 기호입니다.

#ifdef USE_MFC
#ifndef __AFXWIN_H__
#error include 'stdafx.h' before including this file for PCH
#endif
#endif // USE_MFC

/////////////////////////////////////////////////////////////////////////////
// CATPluginApp
// 클래스 상속 구조 : CWinApp -> CATPluginFrameApp -> CATPluginApp
// 이 클래스의 구현을 보려면 ATPlugin.cpp를 참조하십시오.
//

#include <vector>
#include <string>

using namespace std;

typedef vector<string> CStringArray;

#define ENCODE_MARKER_STRING "ENCODE2KOR"
#define ENCODE_MARKER_LENGTH 10


class CATPluginApp : public CATPluginFrameApp
{
	protected:
		CStringArray m_cStringArray;

	public:
		CATPluginApp();
	
	// 플러그인 관련
	public:
		// 공통 메소드
		virtual BOOL OnPluginInit();
		virtual BOOL OnPluginOption();
		virtual BOOL OnPluginClose();

		// 후킹 플러그인 전용 기본 메소드
//		virtual BOOL OnPluginStart();
//		virtual BOOL OnPluginStop();

		// 번역 플러그인 전용 기본 메소드
//		virtual BOOL Translate(LPCSTR cszJapanese, LPSTR szKorean, int nBufSize);

		// 필터 플러그인 전용 기본 메소드
		virtual BOOL PreTranslate(LPCSTR cszInJapanese, LPSTR szOutJapanese, int nBufSize);
		virtual BOOL PostTranslate(LPCSTR cszInKorean, LPSTR szOutKorean, int nBufSize);


		// 아랄트랜스 0.3 번역/필터 플러그인용 객체 관련 함수 
//		virtual BOOL OnObjectInit(TRANSLATION_OBJECT* pTransObj);	// 번역 개체 생성 이벤트 (아랄트랜스 0.3 전용)
//		virtual BOOL OnObjectClose(TRANSLATION_OBJECT* pTransObj);	// 번역 개체 삭제 이벤트	(아랄트랜스 0.3 전용)
//		virtual BOOL OnObjectMove(TRANSLATION_OBJECT* pTransObj);	// 번역 개체 순서변경 이벤트? (아랄트랜스 0.3 전용)
//		virtual BOOL OnObjectOption(TRANSLATION_OBJECT* pTransObj);	// 번역 개체 옵션창 이벤트? (아랄트랜스 0.3 전용)


// 재정의입니다.
	// VC 6.0 과의 호환성을 위해 아래 주석들은 지우지 말아 주세요.
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CATPluginApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CATPluginApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
#ifdef USE_MFC
	DECLARE_MESSAGE_MAP()
#endif // USE_MFC
};

extern CATPluginApp theApp;

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __ATPLUGIN_H__
