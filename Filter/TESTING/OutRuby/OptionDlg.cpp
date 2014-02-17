// OptionDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "OutRuby.h"
#include "OptionDlg.h"



// COptionDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(COptionDlg, CDialog)

COptionDlg::COptionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COptionDlg::IDD, pParent)
	, RubyText(_T(""))
{

}

COptionDlg::~COptionDlg()
{
}

void COptionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, RubyText);
}


BEGIN_MESSAGE_MAP(COptionDlg, CDialog)
END_MESSAGE_MAP()



// COptionDlg 메시지 처리기입니다.

void COptionDlg::SetRubyText(LPCSTR szRuby)
{
	RubyText=szRuby;
}

void COptionDlg::GetRubyText(LPSTR szDest)
{
	lstrcpyA(szDest,RubyText.GetString());
}