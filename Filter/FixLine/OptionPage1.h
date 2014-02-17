#pragma once
#include "afxcmn.h"


// COptionPage1 대화 상자입니다.

class COptionPage1 : public CPropertyPage
{
	DECLARE_DYNAMIC(COptionPage1)

public:
	COptionPage1();
	virtual ~COptionPage1();
	virtual void OnOK();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_Page1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

public:
	CToolTipCtrl m_cToolTip;
	int m_nMaxChar;
	int m_nMaxLine;
	BOOL m_bLimitLine;
	CString m_strRetChar;
	BOOL m_bUseRetChar;
	BOOL m_bUseRet;

	int m_nMaxLen;
	BOOL m_bForceLen;
	
private:
	int m_nLenRadio;

private:
	afx_msg void OnBnClickedTab1Radio1();
	afx_msg void OnBnClickedTab1Radio2();
	afx_msg void OnBnClickedTab1Radio3();
	afx_msg void OnBnClickedCheck3();
	afx_msg void OnEnChangeTab1Edit7();

	CSpinButtonCtrl m_ctrSpin1;
	CSpinButtonCtrl m_ctrSpin2;
	CSpinButtonCtrl m_ctrSpin3;
public:
	bool m_bOK;
	bool m_bLoaded;
	void SetDisable(BOOL bDisable);
	afx_msg void OnBnClickedRadio8();
	afx_msg void OnBnClickedRadio9();
	afx_msg void OnBnClickedRadio10();
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
