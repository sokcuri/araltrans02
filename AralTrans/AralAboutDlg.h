#pragma once

#include "resource.h"

// CAralAboutDlg 대화 상자입니다.

class CAralAboutDlg : public CDialog
{
	DECLARE_DYNAMIC(CAralAboutDlg)

public:
	CAralAboutDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CAralAboutDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ABOUT_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CString m_strInfo;
	HANDLE m_hMapFile;
	bool m_bIsSystemInfo;
private:
	BOOL Is64bitWindows();
	CString GetFilesVersion(LPCTSTR szFilesPath);
	BOOL TestUpdate();
	void GetVersionInfo(CString & strVersionInfo);
	void GetSystemInfo(CString & strSystemInfo);
public:
	afx_msg void OnBnClickedChangeInfoButton();
	afx_msg void OnBnClickedSaveButton();
};
