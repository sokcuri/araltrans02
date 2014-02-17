#pragma once
#include "resource.h"

// COptionDlg 대화 상자입니다.

class COptionDlg : public CDialog
{
	DECLARE_DYNAMIC(COptionDlg)

public:
	COptionDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~COptionDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
private:
	CString RubyText;

public:
	void SetRubyText(LPCSTR szRuby);
	void GetRubyText(LPSTR szDest);
};
