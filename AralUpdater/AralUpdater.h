// AralUpdater.h : main header file for the AralUPDATER application
//

#if !defined(AFX_AralUPDATER_H__05D7E40B_E63B_43A7_9900_0532EB589372__INCLUDED_)
#define AFX_AralUPDATER_H__05D7E40B_E63B_43A7_9900_0532EB589372__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CAralUpdaterApp:
// See AralUpdater.cpp for the implementation of this class
//

class CAralUpdaterApp : public CWinApp
{
public:
	CString m_strExeFilePath;
	CString m_strDmyFilePath;
	CAralUpdaterApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAralUpdaterApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CAralUpdaterApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AralUPDATER_H__05D7E40B_E63B_43A7_9900_0532EB589372__INCLUDED_)
