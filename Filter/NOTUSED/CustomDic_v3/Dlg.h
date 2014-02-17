#pragma once
#include "afxwin.h"
#include "ATPlugin.h"


// CDlg 대화 상자입니다.

class CDlg : public CDialog
{
	DECLARE_DYNAMIC(CDlg)

public:
	CDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	int UseCDic;
public:
	int UseGDic;
public:
	afx_msg void OnBnClickedButton1();
public:
	afx_msg void OnBnClickedButton2();
	int nNoMargin;
};
