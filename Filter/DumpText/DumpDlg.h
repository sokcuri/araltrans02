#pragma once
#include "myedit.h"

#define DOPT_JPN	0x01L
#define DOPT_KOR	0x02L
#define DOPT_SIZE	0x04L
#define DOPT_NEWLN	0x08L
#define DOPT_TOP	0x10L
#define DOPT_FONT	0x20L
#define DOPT_BG		0x40L
#define DOPT_ALL	0xFFL


// CDumpDlg 대화 상자입니다.

class CDumpDlg : public CDialogEx
{ 
	DECLARE_DYNAMIC(CDumpDlg)

public:
	CDumpDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDumpDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_WINDOW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

	UINT_PTR m_pTimer;
public:
	virtual BOOL OnInitDialog();

	afx_msg void OnTimer(UINT_PTR nIDEvent);

	std::wstring getHexString(std::wstring strData,int nLocale);
	std::wstring getUniHexString(std::wstring strData);

protected:
	void applyOption(DWORD dOpt=DOPT_ALL);

	void getOption(bool isDefault);
	void setOption(bool isDefault,DWORD dOpt=DOPT_ALL);
	void inheritOption();
	void resetOption();

	void CheckSimple(UINT Item,bool chk);

	bool m_bInitialized;

	CFont m_cFont;
	CMenu* m_pMenu;

	COLORREF m_fgcolor_default;
	COLORREF m_bgcolor_default;
	LOGFONT m_font_default;

	int m_nTop_default;
	int m_nLeft_default;
	int m_nWidth_default;
	int m_nHeight_default;

	std::wstring m_strRegDefault;
	std::wstring m_strRegMy;

	//옵션 List
	bool m_bShowJText;
	bool m_bShowJHex;
	bool m_bShowJHexU;
	bool m_bShowKText;
	bool m_bShowKHex;
	bool m_bShowKHexU;

	bool m_bNewLine;

	LOGFONT m_font;
	COLORREF m_fgcolor;
	COLORREF m_bgcolor;

	bool m_bAlwaysTop;

	int m_nTop;
	int m_nLeft;
	int m_nWidth;
	int m_nHeight;
	

public:
	CMyEdit m_cEdit;

	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMove(int x, int y);
	afx_msg void OnMAbout();
	afx_msg void OnMAlwaystop();
	afx_msg void OnMBackground();
	afx_msg void OnMClear();
	afx_msg void OnMFilesave();
	afx_msg void OnMFont();
	afx_msg void OnMJhex();
	afx_msg void OnMJhexuni();
	afx_msg void OnMJtext();
	afx_msg void OnMKhex();
	afx_msg void OnMKhexuni();
	afx_msg void OnMKtext();
	afx_msg void OnMLoaddefault();
	afx_msg void OnMNewline();
	afx_msg void OnMSavedefault();
};