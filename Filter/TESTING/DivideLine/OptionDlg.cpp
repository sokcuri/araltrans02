// OptionDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "OptionDlg.h"
#include "Util.h"


// COptionDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(COptionDlg, CDialog)

COptionDlg::COptionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COptionDlg::IDD, pParent)
	, bRemoveSpace(false)
	, bTwoByte(false)
{

}

COptionDlg::~COptionDlg()
{
}

void COptionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK1, bRemoveSpace);
	DDX_Check(pDX, IDC_CHECK2, bTwoByte);
	DDX_Control(pDX, IDC_SPIN1, Spin1);
	DDX_Control(pDX, IDC_SPIN2, Spin2);
	DDX_Control(pDX, IDC_SPIN3, Spin3);
	DDX_Control(pDX, IDC_SPIN4, Spin4);
}


BEGIN_MESSAGE_MAP(COptionDlg, CDialog)
	ON_BN_CLICKED(IDOK, &COptionDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// COptionDlg 메시지 처리기입니다.

BOOL COptionDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	bRemoveSpace	= g_cDivideLine.GetRemoveSpace();
	bTwoByte		= g_cDivideLine.GetTwoByte();

	SetIgnoreLine(g_cDivideLine.GetIgnoreLine());
	SetMaxChar(g_cDivideLine.GetMaxChar());
	SetMaxLine(g_cDivideLine.GetMaxLine());
	SetReturnLen(g_cDivideLine.GetReturnLen());

	wstring RCharHex=StringToHex(g_cDivideLine.GetReturnChar());
	SetReturnCharHex(RCharHex.c_str());

	Spin1.SetRange(0,2048);
	Spin2.SetRange(0,6);
	Spin3.SetRange(0,2048);
	Spin4.SetRange(0,6);

	UpdateData(FALSE);
	return FALSE;
}

int COptionDlg::GetReturnLen(void)
{
	return (int)GetDlgItemInt(IDC_EDIT2,0,1);
}
void COptionDlg::SetReturnLen(int nReturnLen)
{
	SetDlgItemInt(IDC_EDIT2,nReturnLen,1);
}

LPCWSTR COptionDlg::GetReturnCharHex(void)
{
	CString Temp;
	GetDlgItemTextW(IDC_EDIT1,Temp);
	return Temp.GetString();
}
void COptionDlg::SetReturnCharHex(LPCWSTR szReturnCharHex)
{
	SetDlgItemTextW(IDC_EDIT1,szReturnCharHex);
}

int COptionDlg::GetIgnoreLine(void)
{
	return (int)GetDlgItemInt(IDC_EDIT3,0,1);
}
void COptionDlg::SetIgnoreLine(int nIgnoreLine)
{
	SetDlgItemInt(IDC_EDIT3,nIgnoreLine,1);
}

int COptionDlg::GetMaxChar(void)
{
	return (int)GetDlgItemInt(IDC_EDIT4,0,1);
}
void COptionDlg::SetMaxChar(int nMaxChar)
{
	SetDlgItemInt(IDC_EDIT4,nMaxChar,1);
}

int COptionDlg::GetMaxLine(void)
{
	return (int)GetDlgItemInt(IDC_EDIT5,0,1);
}
void COptionDlg::SetMaxLine(int nMaxLine)
{
	SetDlgItemInt(IDC_EDIT5,nMaxLine,1);
}
void COptionDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	wstring RCharHex=GetReturnCharHex();

	g_cDivideLine.SetBase(
		GetIgnoreLine(),
		GetMaxChar(),
		GetMaxLine(),
		GetReturnLen(),
		HexToString(RCharHex));

	g_cDivideLine.SetRemoveSpace(bRemoveSpace);
	g_cDivideLine.SetTwoByte(bTwoByte);

	
	OnOK();
}
