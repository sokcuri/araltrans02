// AralGoodDlg.h : header file
//

#include "afxwin.h"
#include "HyperLink.h"
#if !defined(AFX_ARALGOODDLG_H__B98859FB_3164_40A5_9431_B4CF398A3BEA__INCLUDED_)
#define AFX_ARALGOODDLG_H__B98859FB_3164_40A5_9431_B4CF398A3BEA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CAralGoodDlg dialog

class CAralGoodDlg : public CDialog
{
private:
	static BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam);
	static BOOL GetModulePath(DWORD pid, LPTSTR lpFilename, int nSize);
	static DWORD GetChildProcessID(DWORD dwParentPID, LPCTSTR cszModulePathName=NULL);
	static int GetIconIndex(LPCTSTR lpPath);
	static unsigned int __stdcall HookWorkThreadFunc(void* pParam);

	BOOL	IsUpdateRequired();
	void	UpdateClient();
	void	InitWindowList();
	BOOL	FillWindowList();
	void	Hook(
		DWORD dwProcessId,
		DWORD dwAddress,
		DWORD dwDelay,
		LPCTSTR cszAlgorithmName,
		LPCTSTR cszAlgorithmOption,
		LPCTSTR cszTranslatorName,
		LPCTSTR cszTranslatorOption,
		LPCTSTR cszFilters);

	
	DWORD	m_dwPID;
	DWORD	m_dwADR;
	DWORD	m_dwDelay;
	CString m_strHookName;
	CString m_strHookOption;
	CString m_strTransName;
	CString m_strTransOption;
	CString m_strFilters;

	CString m_strGameDirectory;

	HANDLE	m_hHookWorkThread;
	RECT	m_rtOrigSize;
	BOOL	m_bShowUI;

	BOOL	m_bUseATDataZip;
	BOOL	m_bDeleteLocalFiles;
	BOOL	m_bAppLocAutoConfirm;
	DWORD	m_dwAutoUpdate;


// Construction
public:
	CAralGoodDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CAralGoodDlg)
	enum { IDD = IDD_ARALGOOD_DIALOG };
	CListCtrl	m_ctrlWindowList;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAralGoodDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON	m_hIcon;
	HMODULE	m_hDllMod;

	void SetDebugPrivilege();

	// Generated message map functions
	//{{AFX_MSG(CAralGoodDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBtnHook();
	afx_msg void OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBtnRefresh();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedBtnUnhook();
	afx_msg void OnBnClickedButtonHomepage();
	afx_msg void OnWindowPosChanging(WINDOWPOS FAR* lpwndpos);
public:
	CHyperLink m_linkHome;
private:
	// ATData.zip 에서 파일을 꺼내옵니다
	BOOL ExtractATData(void);
	// ATData.zip에 파일을 보관합니다
	BOOL ArchiveATData(void);
	// 선행 바이너리 패치입니다
	void BinaryPatch(HANDLE hProcess);
public:
	afx_msg void OnBnClickedBtnOption();
	afx_msg void OnBnClickedBtnAbout();
	afx_msg void OnBnClickedCheckExpert();
	CButton mbCheExpert;
	afx_msg void OnHelp();
	afx_msg void OnBnClickedBtnDir();
	afx_msg void OnLvnItemchangedList2(NMHDR *pNMHDR, LRESULT *pResult);
	CButton mbCheRestart;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ARALGOODDLG_H__B98859FB_3164_40A5_9431_B4CF398A3BEA__INCLUDED_)
