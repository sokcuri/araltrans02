#pragma once
#include "afxwin.h"

class COptionNode;

// CPageHook dialog

class CPageHook : public CDialog
{
	DECLARE_DYNAMIC(CPageHook)
private:
	COptionNode*	m_pHookNode;
	int m_nOffset;
	int m_nHookType;
	int m_iTransMethod;

public:
	CPageHook(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPageHook();
	BOOL InitFromHookNode(COptionNode* pHookNode);
	void ClearCtrlValues();
	void SetChildNodeFromCheckbox(COptionNode* pParentNode, LPCTSTR cszChildName, CButton& checkbox);

// Dialog Data
	enum { IDD = IDD_PAGE_HOOK };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_comboTransArgs;
	CButton m_RdoHook1;
	CButton m_RdoHook2;
	CButton m_chkScrTrans;
	CButton m_chkUnicode;
	CButton m_chkUnicode8;
	CButton m_chkAllSameText;
	CButton m_chkClipJpn;
	CButton m_chkClipKor;
	CButton m_btnDelArg;
	CButton m_chkSafe;
	CButton m_chkRemoveSpace;
	CButton m_chkTwoByte;
	CButton m_chkOneByte;
	CButton m_chkAddNull;
	CButton m_chkKiriName;
	CButton m_chkIgnore;
	CButton m_chkMatch;
	CButton m_chkForcePtr;
	CButton m_chkUseLen;
	CButton m_chkNullchar;
	CButton m_chkReverse;
	CButton m_chkYuris;
	CButton m_chkWill;
	CButton m_chkEndPos;
	CEdit m_editLenPos;
	CButton m_chkLenEnd;
	CButton m_chkTjsStr;
	CButton m_chkDuplicated;
	CButton m_chkPtrBack;

public:
	afx_msg void OnCbnSelchangeComboTransArgs();
	afx_msg void OnBnClickedRadioHook();
	afx_msg void OnBnClickedChkUnicode();
	afx_msg void OnBnClickedChkUnicode8();
	afx_msg void OnBnClickedChkAllsametext();
	afx_msg void OnBnClickedBtnArgAdd();
	afx_msg void OnBnClickedBtnArgDel();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedBtnDelHook();
	afx_msg void OnBnClickedChkClipJpn();
	afx_msg void OnBnClickedChkClipKor();
	afx_msg void OnBnClickedRadio();
	afx_msg void OnBnClickedChkRemoveSpace();
	afx_msg void OnBnClickedChkTwobyte();
	afx_msg void OnBnClickedChkOnebyte();
	afx_msg void OnBnClickedChkSafe();
	afx_msg void OnBnClickedChkIgnore();
	afx_msg void OnBnClickedChkMatch();
	afx_msg void OnBnClickedChkForcePtr();
	afx_msg void OnBnClickedChkUselen();
	afx_msg void OnBnClickedChkNullchar();
	afx_msg void OnBnClickedChkReverse();
	afx_msg void OnBnClickedChkYuris();
	afx_msg void OnEnChangeEditLenpos();
	afx_msg void OnBnClickedChkAddnull();
	afx_msg void OnBnClickedChkKiriname();
	afx_msg void OnBnClickedChkEndpos();
	afx_msg void OnBnClickedChkLenend();
	afx_msg void OnBnClickedChkTjsstr();
	afx_msg void OnBnClickedChkScrtrans();
	afx_msg void OnBnClickedChkWill();
	afx_msg void OnBnClickedChkPtrback();
	afx_msg void OnBnClickedChkDuplicated();
};
