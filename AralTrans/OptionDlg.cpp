// OptionDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "OptionDlg.h"

// COptionDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(COptionDlg, CDialog)

COptionDlg::COptionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COptionDlg::IDD, pParent)
	, m_nAutoUpdate(0)
	, m_bUseATDataZip(FALSE)
	, m_bDeleteLocalFiles(FALSE)
	, m_bAppLocAutoConfirm(FALSE)
{

}

COptionDlg::~COptionDlg()
{
}

void COptionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO_AUTOUPDATE0, m_nAutoUpdate);
	DDX_Check(pDX, IDC_CHECK_USEATDATA, m_bUseATDataZip);
	DDX_Check(pDX, IDC_CHECK_DELETELOCALFILE, m_bDeleteLocalFiles);
	DDX_Check(pDX, IDC_CHECK_APPLOCAUTOCONFIRM, m_bAppLocAutoConfirm);
}


BEGIN_MESSAGE_MAP(COptionDlg, CDialog)
	ON_BN_CLICKED(IDC_CHECK_USEATDATA, &COptionDlg::OnBnClickedCheckUseATData)
END_MESSAGE_MAP()


// COptionDlg 메시지 처리기입니다.

BOOL COptionDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	if (m_bUseATDataZip)
		GetDlgItem(IDC_CHECK_DELETELOCALFILE)->EnableWindow(TRUE);
	else
		GetDlgItem(IDC_CHECK_DELETELOCALFILE)->EnableWindow(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}
void COptionDlg::OnBnClickedCheckUseATData()
{
	UpdateData(TRUE);

	if (m_bUseATDataZip)
		GetDlgItem(IDC_CHECK_DELETELOCALFILE)->EnableWindow(TRUE);
	else
		GetDlgItem(IDC_CHECK_DELETELOCALFILE)->EnableWindow(FALSE);
}
