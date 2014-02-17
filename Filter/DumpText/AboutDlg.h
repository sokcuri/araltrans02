#pragma once


// CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CAboutDlg)

public:
	CAboutDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CAboutDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ABOUT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
};
