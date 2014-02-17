// AralUpdaterDlg.h : header file
//

#if !defined(AFX_AralUPDATERDLG_H__16A39DA0_542F_4982_9252_A57D5F7EB5A1__INCLUDED_)
#define AFX_AralUPDATERDLG_H__16A39DA0_542F_4982_9252_A57D5F7EB5A1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "INIParser/INIParser.h"

/////////////////////////////////////////////////////////////////////////////
// CAralUpdaterDlg dialog
class CFileWork  
{
public:
	CString m_strURL;			// 소스파일 경로
	CString m_strLocalPath;		// 타깃파일 경로
};


class CAralUpdaterDlg : public CDialog
{
// Construction
public:
	CAralUpdaterDlg(CWnd* pParent = NULL);	// standard constructor
	BOOL PrepareUpdate(CString &strINIData);

// Dialog Data
	//{{AFX_DATA(CAralUpdaterDlg)
	enum { IDD = IDD_AralUPDATER_DIALOG };
	CStatic	m_ctrlTranText;
	CStatic	m_ctrlCommentText;
	CStatic	m_ctrlPercentText;
	CProgressCtrl	m_ctrlProgress;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAralUpdaterDlg)
	protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	BOOL DoRegistryWork();
	CString ParseSystemVariable(CString strSource);
	HICON		m_hIcon;
	CString		m_strProgramFilesFolder;
	CString		m_strSystemFolder;
	CString		m_strWindowsFolder;
	CString		m_strClientFolder;
	CINIParser	m_ININewVer;
	int			m_nTotalSize;				// 전송할 총 용량
	int			m_nTranSize;				// 현재까지 전송한 용량
	BOOL		m_bStop;
	BOOL		m_bIsWow64;

	int			m_nAlpha;	
	CBitmap*	m_pBG;
	CRgn*		m_pRgn;
	CFont		m_BoldFont;
	CBrush		m_BkBrush;
	HMODULE		m_hUserDll;
	CWinThread*	m_pThread;
	CWinThread*	m_pChildThread;
	CPtrArray*	m_paWorkList;				// 작업목록

	BOOL CleanupUpdate();
	BOOL DownloadUpdateInfo();
	BOOL GetSelfUpdateRequirement();
	BOOL AddCommonFileWork(CPtrArray* pWorkList);
	BOOL AddDriverFileWork(CPtrArray* pWorkList);
	void CloseSmoothly();
	BOOL SetTransparent(HWND hWnd, COLORREF crKey, BYTE  bAlpha, DWORD dwFlags);
	int  GetTotalWorkSize();
	BOOL WorkDownload(CFileWork* pFW);		// 인터넷으로부터 특정파일 1개를 다운로드
	BOOL CreateDirectoryRecursive(CString strDir);
	CString GetFileVersion(CString strFilePathName);
	static BOOL SetWow64Redirection(BOOL bEnable);
	static UINT MainThread(void *lParam);
	static UINT StaticUpdateThread(void* lParam);


	// Generated message map functions
	//{{AFX_MSG(CAralUpdaterDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AralUPDATERDLG_H__16A39DA0_542F_4982_9252_A57D5F7EB5A1__INCLUDED_)
