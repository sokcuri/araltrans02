// OptionInputDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ATCode.h"
#include "OptionInputDlg.h"


// COptionInputDlg dialog

IMPLEMENT_DYNAMIC(COptionInputDlg, CDialog)

COptionInputDlg::COptionInputDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COptionInputDlg::IDD, pParent)
	, m_strInputString(_T(""))
{

}

COptionInputDlg::~COptionInputDlg()
{
}

void COptionInputDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_strInputString);
}


BEGIN_MESSAGE_MAP(COptionInputDlg, CDialog)
	ON_BN_CLICKED(IDOK, &COptionInputDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// COptionInputDlg message handlers

void COptionInputDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OnOK();
}
