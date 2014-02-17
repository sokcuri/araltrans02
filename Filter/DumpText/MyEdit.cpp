// MyEdit.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MyEdit.h"


// CMyEdit

using namespace std;

IMPLEMENT_DYNAMIC(CMyEdit, CEdit)

CMyEdit::CMyEdit()
{
	m_bInit=false;

	m_crFG = RGB(0,0,0);
	/*m_crBG = this->GetDC()->GetBkColor();
	m_crFG = this->GetDC()->GetTextColor();

	m_brBG.CreateSolidBrush(m_crBG);*/
}

CMyEdit::~CMyEdit()
{
	if(m_brBG.GetSafeHandle())
		m_brBG.DeleteObject();
}


BEGIN_MESSAGE_MAP(CMyEdit, CEdit)
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_CREATE()
END_MESSAGE_MAP()



// CMyEdit 메시지 처리기입니다.




HBRUSH CMyEdit::CtlColor(CDC* pDC, UINT nCtlColor) 
{
    pDC->SetTextColor(m_crFG);
    pDC->SetBkColor(m_crBG);
    return m_brBG;
}

void CMyEdit::SetBackColor(COLORREF crBG)
{
    m_crBG = crBG;

	
    if (m_brBG.GetSafeHandle())
       m_brBG.DeleteObject();

    m_brBG.CreateSolidBrush(crBG);
	
    Invalidate(TRUE);
}

void CMyEdit::SetTextColor(COLORREF crFG)
{
    m_crFG = crFG;
    Invalidate(TRUE);    
}


int CMyEdit::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CEdit::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
	if(!m_bInit){
		m_crBG = this->GetDC()->GetBkColor();
		m_crFG = this->GetDC()->GetTextColor();

		if (m_brBG.GetSafeHandle())
		   m_brBG.DeleteObject();

		    m_brBG.CreateSolidBrush(m_crBG);

		m_bInit=true;
	}

	return 0;
}

void CMyEdit::addText(wstring strText,bool bFlush){
	m_strAddedText+= strText;
	if(bFlush){
		flushText();
	}
}

void CMyEdit::flushText(){
	if(m_strAddedText.length()==0){
		return;
	}

	int nTmpStart=0;
	int nTmpEnd=0;

	GetSel(nTmpStart,nTmpEnd);

	int nLen = GetWindowTextLengthW();
	SetSel(nLen,-1);
	ReplaceSel(m_strAddedText.c_str());

	SetSel(-1,-1);

	m_strAddedText.clear();
}

void CMyEdit::clearText(){
	SetSel(0,-1,TRUE);
	ReplaceSel(L"");
	SetSel(-1,-1);
}