// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.


#ifndef __STDAFX_H__
#define __STDAFX_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////////////////////////
// 플러그인 기본 세팅
//

// 아랄트랜스 0.2 용 플러그인일 경우 설정한다.
#define USE_ARALTRANS_0_2

// 아랄트랜스 0.3 용 플러그인일 경우 설정한다.
//#define USE_ARALTRANS_0_3


// 알고리즘 (후킹) 플러그인인 경우 설정한다.
//#define ATPLUGIN_ALGORITHM

// 번역 플러그인인 경우 설정한다.
//#define ATPLUGIN_TRANSLATOR

// 필터 플러그인인 경우 설정한다.
#define ATPLUGIN_FILTER

// MFC 와 비 MFC 를 결정하는 스위치
//
// 어차피 컴파일러 옵션에서 MFC 를 사용.. 도 시켜줘야 하므로 여기에서 USE_MFC 를 설정하는
// 것 보다는 컴파일러 옵션에서 설정하는 것을 권장합니다.
//#define USE_MFC

// 번역 버퍼 기본 크기
#define TRANS_BUF_LEN 10240

//
// 플러그인 기본 세팅 끝
//////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////
// 디버그 세팅

// FORCE_DEBUG_LOG : 보통 디버그 모드일때는 디버그 메세지 출력, 릴리즈 모드일때는 무시
//                   하지만 FORCE_DEBUG_LOG 가 켜져있으면 무조건 디버그 메세지를 출력
//#define FORCE_DEBUG_LOG

// DEBUG_LOG_TYPE : 디버그 메세지 출력 방식 지정
// 0 - OutputDebugString
// 1 - MessageBox
// 2 - FILE
// 3 - Log window (아직 지원않음)
#define DEBUG_LOG_TYPE	1

// DEBUG_WINDOW_CAPTION : 디버그 윈도우 캡션 (type 1, 3 에서만 의미 있음)
#define DEBUG_WINDOW_CAPTION "ATPlugin"

// DEBUG_LOG_FILEPATH : 디버그 로그 파일 지정 (type 2 에서만 의미 있음)
#define DEBUG_LOG_FILEPATH	"C:\\ATPlugin.log"

// _CRT_SECURE_NO_WARNINGS : fopen 과 _vsnwprintf 를 사용시 발생하는 warning C4996 을 꺼준다.
//                           fopen_s 와 _vsnwprintf_s 를 사용하면 안뜨지만 VC++ 6.0 에서 미지원.
//                           warning 이 귀찮을 때만 사용할 것을 권장한다.
//#define _CRT_SECURE_NO_WARNINGS

// 디버그 세팅 끝
//////////////////////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////////////////////
// 여기 밑으로는 일반적인 StdAfx.h 의 내용임

#if _MSC_VER > 1200 // 1200 = Visual C++ 6.0
#ifndef WINVER              
#define WINVER 0x0501		// WinXP
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501 // WinXP
#endif
#endif // _MSC_VER

#ifndef USE_MFC	// MFC 사용 않음

#define WIN32_LEAN_AND_MEAN		// 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.

#include <windows.h>
#include <tchar.h>

#else			// MFC 사용

#define VC_EXTRALEAN		// 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// 일부 CString 생성자는 명시적으로 선언됩니다.

#include <afxwin.h>         // MFC 핵심 및 표준 구성 요소입니다.
#include <afxext.h>         // MFC 확장입니다.

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // MFC OLE 클래스입니다.
#include <afxodlgs.h>       // MFC OLE 대화 상자 클래스입니다.
#include <afxdisp.h>        // MFC 자동화 클래스입니다.
#endif // _AFX_NO_OLE_SUPPORT

#ifndef _AFX_NO_DB_SUPPORT
#include <afxdb.h>			// MFC ODBC 데이터베이스 클래스입니다.
#endif // _AFX_NO_DB_SUPPORT

#ifndef _AFX_NO_DAO_SUPPORT
#include <afxdao.h>			// MFC DAO 데이터베이스 클래스입니다.
#endif // _AFX_NO_DAO_SUPPORT

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>		// Internet Explorer 4 공용 컨트롤에 대한 MFC 지원입니다.
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// Windows 공용 컨트롤에 대한 MFC 지원입니다.
#endif // _AFX_NO_AFXCMN_SUPPORT

#endif // USE_MFC

// TODO: reference additional headers your program requires here

#include "ATPluginFrame/ATPluginFrame.h"

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __STDAFX_H__
