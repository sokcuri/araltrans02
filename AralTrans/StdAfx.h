// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__0AF87941_E17E_45CB_8184_2A9044D4B161__INCLUDED_)
#define AFX_STDAFX_H__0AF87941_E17E_45CB_8184_2A9044D4B161__INCLUDED_

//#define TEST_VERSION	// 테스트버전 define. 자동 업데이트를 테스트버전에서 임시로 죽이기 위해 사용.

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WINVER 0x0500
#pragma warning(disable:4996)
#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__0AF87941_E17E_45CB_8184_2A9044D4B161__INCLUDED_)
