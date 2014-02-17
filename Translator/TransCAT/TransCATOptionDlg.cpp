// TransCATOptionDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "TransCAT.h"
#include "TransCATOptionDlg.h"


// CTransCATOptionDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CTransCATOptionDlg, CDialog)

CTransCATOptionDlg::CTransCATOptionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTransCATOptionDlg::IDD, pParent)
	, m_bRemoveTrace(FALSE)
	, m_bRemoveDupSpace(FALSE)
{

}

CTransCATOptionDlg::~CTransCATOptionDlg()
{
}

void CTransCATOptionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHK_ALLOW_TRACE, m_bRemoveTrace);
	DDX_Check(pDX, IDC_CHK_ALLOW_DUPSPACE, m_bRemoveDupSpace);
}


BEGIN_MESSAGE_MAP(CTransCATOptionDlg, CDialog)
END_MESSAGE_MAP()


// CTransCATOptionDlg 메시지 처리기입니다.
