#pragma once


// CMyEdit

class CMyEdit : public CEdit
{
	DECLARE_DYNAMIC(CMyEdit)

public:
	CMyEdit();
	virtual ~CMyEdit();

protected:
	DECLARE_MESSAGE_MAP()

	std::wstring m_strAddedText;

	//text and text background colors
	COLORREF m_crFG;
    COLORREF m_crBG;
    //background brush
    CBrush m_brBG;

	bool m_bInit;

public:
	void SetTextColor(COLORREF crFG);
    void SetBackColor(COLORREF crBG);

	void addText(std::wstring strText,bool bFlush=false);
	void flushText();
	void clearText();

	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};


