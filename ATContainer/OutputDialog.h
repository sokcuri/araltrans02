#include "afxwin.h"
#include "afxcmn.h"
#if !defined(AFX_OUTPUTDIALOG_H__F393FFD4_AF99_497D_B148_0A0BAC9221C1__INCLUDED_)
#define AFX_OUTPUTDIALOG_H__F393FFD4_AF99_497D_B148_0A0BAC9221C1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OutputDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COutputDialog dialog

class COutputDialog : public CDialog
{
private:
	CWnd*	m_pParentWnd;

	BOOL CreateShortcut(LPCTSTR strShortcutPath, LPCTSTR strObjPath, LPCTSTR strArgs, LPCTSTR strWorkingDir, LPCTSTR strIconPath, LPCTSTR strDesc);
	BOOL InitFilterGrid();
// Construction
public:
	COutputDialog(CWnd* pParent = NULL);   // standard constructor
	
// Dialog Data
	//{{AFX_DATA(COutputDialog)
	enum { IDD = IDD_MODALESS_TEXTOUT };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COutputDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COutputDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	//afx_msg void OnNMCustomdrawProgress1(NMHDR *pNMHDR, LRESULT *pResult);
	//afx_msg void OnStnClickedStaticStat();
	
	CComboBox m_comboHookMode;
	CComboBox m_comboTranslator;
	CProgressCtrl m_progAnalyze;
	CButton m_chkTrayMin;

	void CreateTrayIcon();
	void DeleteTrayIcon();
	void UpdateFilterGrid();

	afx_msg void OnCbnSelchangeComboHookmode();
	afx_msg void OnCbnSelchangeComboTranslator();
	afx_msg void OnBnClickedBtnHookSetting();
	afx_msg void OnBnClickedBtnHookOnoff();
	afx_msg void OnBnClickedBtnTransSetting();
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	afx_msg void OnNcDestroy();
	afx_msg void OnBnClickedBtnCreateShortcut();
	afx_msg void OnBnClickedCheck1();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg LRESULT TrayIconMsg(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedBtnFilterSetting();
	afx_msg void OnBnClickedBtnFilterAdd();
	afx_msg void OnBnClickedBtnFilterDelete();
	afx_msg void OnBnClickedBtnFilterUp();
	afx_msg void OnBnClickedBtnFilterDown();
	afx_msg void OnNMDblclkListFilter(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBtnFilterDirectSetting();
	afx_msg void OnLvnKeydownListFilter(NMHDR *pNMHDR, LRESULT *pResult);
//	afx_msg void OnNMKillfocusListFilter(NMHDR *pNMHDR, LRESULT *pResult);
//	afx_msg void OnNMSetfocusListFilter(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickListFilter(NMHDR *pNMHDR, LRESULT *pResult);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OUTPUTDIALOG_H__F393FFD4_AF99_497D_B148_0A0BAC9221C1__INCLUDED_)
