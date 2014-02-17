#pragma once
#include "Resource.h"
#include "afxwin.h"
#include "PictureEx/PictureEx.h"


// CWaitingDlg 대화 상자입니다.

class CWaitingDlg : public CDialog
{
	DECLARE_DYNAMIC(CWaitingDlg)

public:
	CWaitingDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CWaitingDlg();
	DWORD	m_dwEndTime;
	void	ShowRemainedTime(DWORD dwTime);

// 대화 상자 데이터입니다.
	enum { IDD = IDD_WAITING_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CStatic		m_lblWaitingTime;
	CPictureEx	m_picWaiting;
	DWORD		m_dwWaitingTime;
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnNcDestroy();
};
