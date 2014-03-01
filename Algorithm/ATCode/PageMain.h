#pragma once

#include "resource.h"
#include "afxwin.h"
#include "afxcmn.h"

class COptionNode;

// CPageMain dialog

class CPageMain : public CDialog
{
	DECLARE_DYNAMIC(CPageMain)
private:
	COptionNode*	m_pRootNode;

public:
	CPageMain(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPageMain();

	BOOL InitFromRootNode(COptionNode* pRootNode);
	void ClearCtrlValues();
	void SetChildNodeFromCheckbox(COptionNode* pParentNode, LPCTSTR cszChildName, CButton& checkbox, BOOL reverse = FALSE);

// Dialog Data
	enum { IDD = IDD_PAGE_MAIN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();


	DECLARE_MESSAGE_MAP()
public:
	CButton m_chkFixSize;
public:
	CEdit m_editFontDesc;
public:
	CButton m_chkEncodeKor;
	int m_nEncodeKor;
public:
	afx_msg void OnBnClickedBtnFont();
public:
	afx_msg void OnBnClickedChkForceFont();
public:
	CButton m_btnFont;
public:
//	afx_msg void OnClickRadioTransMode();
public:
	afx_msg void OnBnClickedChkNoAslr();
public:
	CButton m_chkNoAslr;
public:
	afx_msg void OnBnClickedChkFixSize();
public:
	afx_msg void OnBnClickedRadioPtrchange();
public:
	afx_msg void OnBnClickedRadioOverwrite();
public:
	afx_msg void OnBnClickedChkEncodekor();
public:
	afx_msg void OnBnClickedChkRemoveSpace();
public:
	afx_msg void OnBnClickedChkCapKor();
public:
	afx_msg void OnBnClickedChkCapJpn();
public:
	afx_msg void OnBnClickedChkClipKor();
public:
	afx_msg void OnBnClickedChkClipJpn();
public:
	afx_msg void OnBnClickedBtn();
public:
	afx_msg void OnBnClickedChkTwobyte();
public:
	afx_msg void OnBnClickedBtnAddHook();
public:
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
public:
	afx_msg void OnBnClickedChkUitrans();
public:
	CButton m_chkUITrans;
	CSliderCtrl m_sliderM2W;
	CSliderCtrl m_sliderFontLoad;
	CSliderCtrl m_sliderUniKofilter;
public:
	CButton m_chkComJP;
	afx_msg void OnBnClickedChkComparejp();
	afx_msg void OnBnClickedBtnHelp();
	afx_msg void OnBnClickedRadioEncodekor1();
	afx_msg void OnBnClickedRadioEncodekor2();
};
