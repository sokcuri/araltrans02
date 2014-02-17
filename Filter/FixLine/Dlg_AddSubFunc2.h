#pragma once
#include "afxcmn.h"
#include "Resource.h"

// CDlg_AddSubFunc2 대화 상자입니다.

class CDlg_AddSubFunc2 : public CDialog
{
	DECLARE_DYNAMIC(CDlg_AddSubFunc2)

public:
	CDlg_AddSubFunc2(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlg_AddSubFunc2();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_AddSubFunc2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
private:
	CSpinButtonCtrl m_ctrSpinSize;
	CSpinButtonCtrl m_ctrSpinMin;
	CSpinButtonCtrl m_ctrSpinMax;
	CToolTipCtrl	m_cToolTip;
	int m_nSize;
	bool m_bSizeAuto;
	bool m_bSize;
	int m_nMin;
	int m_nMax;
	bool m_bMin;
	bool m_bMax;
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
private:
	void SetEnableMode(bool bFunc);
	void SetEnableSize(int nMode);
	void SetEnableMin(int nMode);
	void SetEnableMax(int nMode);
	void SetEnableSizeAuto(int nMode);
public:
	afx_msg void OnBnClickedRadioFunc();
	afx_msg void OnBnClickedRadioText();
	afx_msg void OnBnClickedRadioSizeYes();
	afx_msg void OnBnClickedRadioSizeNo();
	afx_msg void OnBnClickedRadioSizeAuto();
	afx_msg void OnBnClickedRadioSizeCustom();
	afx_msg void OnBnClickedCheckLenMin();
	afx_msg void OnBnClickedCheckLenMax();
	afx_msg void OnBnClickedCheckDel();
	afx_msg void OnBnClickedCheckPass();
	afx_msg void OnBnClickedRadioTransOn();
	afx_msg void OnBnClickedRadioTransOff();

public:
	int nSize;
	bool bTrans;
	bool bDel;
	bool bPass;
	CString strText;
	int nMin;
	int nMax;
	bool bFunc;
	afx_msg void OnBnClickedOk();
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
public:
	afx_msg void OnBnClickedTextHelp();
};
