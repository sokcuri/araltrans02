#pragma once
#include "afxcmn.h"


// CSubOptDlg 대화 상자입니다.

class CSubOptDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSubOptDlg)

public:
	CSubOptDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CSubOptDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_Sub };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	int m_nAutoInterval;
	BOOL m_bAutoLoad;
	BOOL m_bTrim;
	CSpinButtonCtrl m_ctrSpin;
	BOOL m_bSaveDefaultOption;
	virtual BOOL OnInitDialog();
};
