#pragma once
#include "afxeditbrowsectrl.h"
#include "afxwin.h"


// CFileAddDlg 대화 상자입니다.

class CFileAddDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CFileAddDlg)

public:
	CFileAddDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CFileAddDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_File };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

	void SetFileType(CString strPath);
public:
	bool m_bDragFile;
	int m_nLoadType;
	int m_nDicType;


	afx_msg void OnBnClickedRadioOpen();
	afx_msg void OnBnClickedRadioNew();

	virtual BOOL OnInitDialog();
	CEdit m_ctrPath;
	CString m_strPath;
	CButton m_ctrFind;
	afx_msg void OnBnClickedBtnFind();
	afx_msg void OnDropFiles(HDROP hDropInfo);
};
