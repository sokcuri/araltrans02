// AboutDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "DumpText.h"
#include "AboutDlg.h"
#include "afxdialogex.h"


// CAboutDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CAboutDlg, CDialogEx)

CAboutDlg::CAboutDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CAboutDlg::IDD, pParent)
{

}

CAboutDlg::~CAboutDlg()
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CAboutDlg 메시지 처리기입니다.
