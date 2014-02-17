// SubOptDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "CustomDic.h"
#include "SubOptDlg.h"
#include "afxdialogex.h"


// CSubOptDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CSubOptDlg, CDialogEx)

CSubOptDlg::CSubOptDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSubOptDlg::IDD, pParent)
	, m_nAutoInterval(0)
	, m_bAutoLoad(FALSE)
	, m_bTrim(FALSE)
	, m_bSaveDefaultOption(FALSE)
{

}

CSubOptDlg::~CSubOptDlg()
{
}

void CSubOptDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT2, m_nAutoInterval);
	DDX_Check(pDX, IDC_CHECK1, m_bAutoLoad);
	DDX_Check(pDX, IDC_CHECK2, m_bTrim);
	DDX_Control(pDX, IDC_SPIN1, m_ctrSpin);
	DDX_Check(pDX, IDC_CHECK3, m_bSaveDefaultOption);
}


BEGIN_MESSAGE_MAP(CSubOptDlg, CDialogEx)
END_MESSAGE_MAP()


// CSubOptDlg 메시지 처리기입니다.


BOOL CSubOptDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	m_ctrSpin.SetRange(1,300);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}
