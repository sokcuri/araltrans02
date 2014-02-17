// AralGood.h : main header file for the ARALGOOD application
//

#if !defined(AFX_ARALGOOD_H__B4CB3D80_6DFD_4858_A230_41608178E47E__INCLUDED_)
#define AFX_ARALGOOD_H__B4CB3D80_6DFD_4858_A230_41608178E47E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CAralGoodApp:
// See AralGood.cpp for the implementation of this class
//

class CAralGoodApp : public CWinApp
{
public:
	CAralGoodApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAralGoodApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CAralGoodApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ARALGOOD_H__B4CB3D80_6DFD_4858_A230_41608178E47E__INCLUDED_)
