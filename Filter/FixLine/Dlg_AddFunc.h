#pragma once
#include "Resource.h"
#include "Dlg_AddSubFunc2.h"
#include "afxcmn.h"
#include <vector>

using namespace std;

// CDlg_AddFunc 대화 상자입니다.

struct FLStringElement2
{
	int nSize;
	bool bTrans;
	bool bDel;
	bool bPass;
	wstring strText;
	int nMin;
	int nMax;
	bool bFunc;
};

class CDlg_AddFunc : public CDialog
{
	DECLARE_DYNAMIC(CDlg_AddFunc)

public:
	CDlg_AddFunc(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlg_AddFunc();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_AddFunc };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

private:
	//CDlg_AddSubFunc2	m_cAddSubFunc;
	afx_msg void OnBnClickedButton1();
public:
	vector<FLStringElement2>	m_FuncPool;
public:
	CListCtrl m_ctrList;
	int m_nSelect;
	int m_nItemN;

	CToolTipCtrl m_cToolTip;
public:
	CString m_strText;
public:
	afx_msg void OnBnClickedButton3();
public:
	afx_msg void OnNMClickList1(NMHDR *pNMHDR, LRESULT *pResult);
public:
	afx_msg void OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult);
public:
	afx_msg void OnBnClickedButton2();
	int m_nType;
	afx_msg void OnBnClickedOk();
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	BOOL m_bTrim;
};
