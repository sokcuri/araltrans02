// Dlg_AddName.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "FixLine.h"
#include "Dlg_AddName.h"


// CDlg_AddName 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlg_AddName, CDialog)

CDlg_AddName::CDlg_AddName(CWnd* pParent /*=NULL*/)
	: CDialog(CDlg_AddName::IDD, pParent)
	, m_strText(_T(""))
{

}

CDlg_AddName::~CDlg_AddName()
{
}

void CDlg_AddName::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT2, m_strText);
}


BEGIN_MESSAGE_MAP(CDlg_AddName, CDialog)
END_MESSAGE_MAP()


// CDlg_AddName 메시지 처리기입니다.
