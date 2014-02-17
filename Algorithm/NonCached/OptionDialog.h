#pragma once

#include "resource.h"
// COptionDialog 대화 상자입니다.

class COptionDialog : public CDialog
{
	DECLARE_DYNAMIC(COptionDialog)

public:
	COptionDialog(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~COptionDialog();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_OPTIONDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	BOOL m_bMatchLen;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedMatchlencheck();
};
