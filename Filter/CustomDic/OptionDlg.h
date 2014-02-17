#pragma once
#include "dragdroplistctrl.h"
#include "DicStruct.h"
#include "afxwin.h"

// COptionDlg 대화 상자입니다.


class COptionDlg : public CDialogEx
{
	DECLARE_DYNAMIC(COptionDlg)

public:
	COptionDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~COptionDlg();

	vector<fileInfo> m_arFileList;
	vector<int> m_arPreList;
	vector<int> m_arPostList;

	bool isChanged();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_Option };

	bool LoadOption();
	void SaveOption(const bool bDefault=false);

	void Migration(const bool bCDic,const bool bGDic,const bool bTrim);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

	void LoadFileList();
	void SaveFileList();

	void InitListCtrl();

	bool AddFile(CString strFilePath,bool bOpen,int nType);
	void DeleteFile(CString strFilePath);
	set<wstring> m_allFiles;//중복 파일 체크용

	wstring m_strBaseAralRegPath;
	wstring m_strRegPath;

	bool m_bChanged;
	bool m_bActive;
public:

	int m_nUseAutoLoad;
	bool m_bTrimWord;

	CDragDropListCtrl m_cPreDicList;
	CDragDropListCtrl m_cPostDicList;
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnLvnItemchangedList2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnInsertitemList2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnDeleteitemList2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnInsertitemList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnDeleteitemList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton7();
	CButton m_ctrButtonAdd;
	CButton m_ctrButtonUp1;
	CButton m_ctrButtonDown1;
	CButton m_ctrButtonDelete1;
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton4();
	CButton m_ctrButtonUp2;
	CButton m_ctrButtonDown2;
	CButton m_ctrButtonDelete2;
	afx_msg void OnBnClickedButton8();
	afx_msg void OnBnClickedButton10();
	afx_msg void OnBnClickedButton9();
};
