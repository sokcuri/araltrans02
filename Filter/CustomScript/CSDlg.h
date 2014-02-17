#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include <set>

using namespace std;

struct FileDetail{
	CString FileName;
	CString Subject;
	CString Description;
};

// CCSDlg 대화 상자입니다.

class CCSDlg : public CDialog
{
	DECLARE_DYNAMIC(CCSDlg)

public:
	CCSDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CCSDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	bool GetisWrite(){if(Mode>0)return true;else return false;}
	void SetisWrite(bool isWrite){Mode=(int)isWrite;}

	void SetListFilename(LPCWSTR name){ListFilename=name;}

	void Init();

	void SetPassN(int _PassN);
	int GetPassN();

	BOOL Cached;
	BOOL Always;

	FileDetail SetFileDetail(CString FilePath);
private:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnNMClickList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio2();
	CSpinButtonCtrl Spin1;
	
	afx_msg void OnEnChangeEdit2();
	afx_msg void OnBnClickedRadio5();
	afx_msg void OnBnClickedRadio4();
	afx_msg void OnBnClickedRadio3();

private:
	set<UINT> FileList;
	int ItemN;
	int SelectN;
	int PassN;
	int Radio2;
	int Mode;
	CString ListFilename;
	CListCtrl CSFList;
	afx_msg void OnBnClickedButton3();
	void VisibleSet();
};
