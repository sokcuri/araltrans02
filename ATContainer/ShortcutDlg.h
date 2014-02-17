#pragma once
#include "Resource.h"
#include "afxwin.h"

// CShortcutDlg 대화 상자입니다.

class CShortcutDlg : public CDialog
{
	DECLARE_DYNAMIC(CShortcutDlg)

public:
	CShortcutDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CShortcutDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_SHORTCUT_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CString m_strShortcutFilePath;
	CString m_strDelayTime;
	CString m_strLoaderPath;
	BOOL m_nShortcutType;
	BOOL m_nHideType;
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio2();
	CButton m_chkHide;
	afx_msg void OnBnClickedCheckHide();
};
