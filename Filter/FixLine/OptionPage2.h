#pragma once
#include "Dlg_AddFunc.h"
#include "tstring.h"
#include <vector>

using namespace std;

// COptionPage2 대화 상자입니다.

class COptionPage2 : public CPropertyPage
{
	DECLARE_DYNAMIC(COptionPage2)

public:
	COptionPage2();
	virtual ~COptionPage2();
	virtual void OnOK();
	afx_msg void OnBnClickedTab2Button3();

	CListCtrl m_ctrFuncList;

// 대화 상자 데이터입니다.
	enum { IDD = IDD_Page2 };

protected:
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

private:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

private:
	CDlg_AddFunc	m_cAddFunc;
	
public:
	vector<vector<FLStringElement2>> m_FuncPool;  //여긴 조각조각
	CStringArray m_FuncPool2;  //여긴 완성판
	CArray<int> m_FuncPool3;  //함수 특성 보관
	CArray<bool> m_FuncPool4; //Trim 특성 보관
	
public:
	afx_msg void OnBnClickedTab2Button5();
public:
	afx_msg void OnBnClickedTab2Button4();
	int m_nSelect;
public:
	int m_nItemN;
public:
	afx_msg void OnNMClickTab2Funclist(NMHDR *pNMHDR, LRESULT *pResult);
public:
	afx_msg void OnNMDblclkTab2Funclist(NMHDR *pNMHDR, LRESULT *pResult);
public:
	bool m_bOK;
};
