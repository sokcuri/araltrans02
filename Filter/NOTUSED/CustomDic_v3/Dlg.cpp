// Dlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Dlg.h"
#include "Util.h"

// CDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlg, CDialog)

CDlg::CDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDlg::IDD, pParent)
	, UseCDic(false)
	, UseGDic(false)
	, nNoMargin(0)
{
}

CDlg::~CDlg()
{
}

void CDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX,IDC_CHECK1,UseCDic);
	DDX_Check(pDX,IDC_CHECK2,UseGDic);
	DDX_Check(pDX, IDC_CHECK3, nNoMargin);
}


BEGIN_MESSAGE_MAP(CDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &CDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CDlg::OnBnClickedButton2)
END_MESSAGE_MAP()


// CDlg 메시지 처리기입니다.

void CDlg::OnBnClickedButton1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	wstring Temp=GetGameDirectory();
	Temp+=L"\\ATData\\";
	MyCreateDirectory(Temp.c_str());
    ShellExecute(NULL, L"Open", L"explorer.exe",  Temp.c_str(), L"", SW_SHOWNORMAL);
}

void CDlg::OnBnClickedButton2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	wstring Temp=GetATDirectory();
	Temp+=L"\\";
	ShellExecute(NULL, L"Open", L"explorer.exe", Temp.c_str(), L"", SW_SHOWNORMAL);
}
