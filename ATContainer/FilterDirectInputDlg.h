#pragma once


// CFilterDirectInputDlg 대화 상자입니다.

class CFilterDirectInputDlg : public CDialog
{
	DECLARE_DYNAMIC(CFilterDirectInputDlg)

public:
	CFilterDirectInputDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CFilterDirectInputDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIRECT_OPTION_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	// 필터 에디트박스 DDE 컨트롤
	CString m_strFilters;
};
