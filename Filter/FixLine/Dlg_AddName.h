#pragma once
#include "Resource.h"

// CDlg_AddName 대화 상자입니다.

class CDlg_AddName : public CDialog
{
	DECLARE_DYNAMIC(CDlg_AddName)

public:
	CDlg_AddName(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlg_AddName();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_AddName };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CString m_strText;
};
