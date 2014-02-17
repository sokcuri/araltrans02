#pragma once
#include "afxwin.h"
#include <queue>

using namespace std;

CCriticalSection g_csThread;
volatile bool g_bContinue;

// CDumpDlg 대화 상자입니다.

class CDumpDlg : public CDialog
{
	DECLARE_DYNAMIC(CDumpDlg)

public:
	CDumpDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDumpDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DumpDlg };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

	CWinThread m_PrintThread;
	queue <CString> m_PrintQueue;
	UINT PrintThreadFunc(__in LPVOID lpParameter);

public:
	BOOL PrintText(CString Text);
public:
	CEdit DumpEdit;
};
