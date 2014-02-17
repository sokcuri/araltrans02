// ShortcutDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ShortcutDlg.h"


// CShortcutDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CShortcutDlg, CDialog)

CShortcutDlg::CShortcutDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CShortcutDlg::IDD, pParent)
	, m_strShortcutFilePath(_T(""))
	, m_strDelayTime(_T(""))
	, m_nShortcutType(FALSE)
	, m_nHideType(FALSE)
	, m_strLoaderPath(_T(""))
{

}

CShortcutDlg::~CShortcutDlg()
{
}

void CShortcutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_strShortcutFilePath);
	DDX_Text(pDX, IDC_EDIT2, m_strDelayTime);
	//	DDX_Radio(pDX, IDC_RADIO1, m_nShortcutType);
	DDX_Text(pDX, IDC_EDIT3, m_strLoaderPath);
	DDX_Control(pDX, IDC_CHECK_HIDE, m_chkHide);
}


BEGIN_MESSAGE_MAP(CShortcutDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &CShortcutDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CShortcutDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_RADIO1, &CShortcutDlg::OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO2, &CShortcutDlg::OnBnClickedRadio2)
	ON_BN_CLICKED(IDC_CHECK_HIDE, &CShortcutDlg::OnBnClickedCheckHide)
END_MESSAGE_MAP()


// '찾아보기' 버튼 클릭시
void CShortcutDlg::OnBnClickedButton1()
{

	CFileDialog fd(FALSE, _T("lnk"), m_strShortcutFilePath, OFN_OVERWRITEPROMPT, _T("바로가기 파일(*.lnk)|*.lnk|All Files(*.*)|*.*||"), this);
	if( fd.DoModal() == IDOK )
	{
		m_strShortcutFilePath = fd.GetPathName();
		UpdateData(FALSE);
	}


}

void CShortcutDlg::OnBnClickedButton2()
{
	CFileDialog fd(TRUE, NULL, NULL, NULL, _T("실행파일(*.exe;*.com)|*.exe;*.com|All Files(*.*)|*.*||"), this);
	if( fd.DoModal() == IDOK )
	{
		m_strLoaderPath = fd.GetPathName();
		UpdateData(FALSE);
	}
}

BOOL CShortcutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	if (!m_nShortcutType)
	{
		GetDlgItem(IDC_EDIT3)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON2)->EnableWindow(FALSE);
		CheckRadioButton(IDC_RADIO1, IDC_RADIO2, IDC_RADIO1);
	}
	else
	{
		GetDlgItem(IDC_EDIT3)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON2)->EnableWindow(TRUE);
		CheckRadioButton(IDC_RADIO1, IDC_RADIO2, IDC_RADIO2);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CShortcutDlg::OnBnClickedRadio1()
{
	m_nShortcutType=0;
	GetDlgItem(IDC_EDIT3)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON2)->EnableWindow(FALSE);
	UpdateData(FALSE);
}

void CShortcutDlg::OnBnClickedRadio2()
{
	m_nShortcutType=1;
	GetDlgItem(IDC_EDIT3)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON2)->EnableWindow(TRUE);
	UpdateData(FALSE);
}

void CShortcutDlg::OnBnClickedCheckHide()
{
	if( m_chkHide.GetCheck() == 1 )
		m_nHideType=1;
	else
		m_nHideType=0;
	UpdateData(FALSE);
}
