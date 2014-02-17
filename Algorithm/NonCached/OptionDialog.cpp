// OptionDialog.cpp : 구현 파일입니다.
//

#include "NonCached.h"
#include "OptionDialog.h"


// COptionDialog 대화 상자입니다.

IMPLEMENT_DYNAMIC(COptionDialog, CDialog)

COptionDialog::COptionDialog(CWnd* pParent /*=NULL*/)
	: CDialog(COptionDialog::IDD, pParent)
	, m_bMatchLen(FALSE)
{

}

COptionDialog::~COptionDialog()
{
}

void COptionDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_MATCHLENCHECK, m_bMatchLen);
}


BEGIN_MESSAGE_MAP(COptionDialog, CDialog)
	ON_BN_CLICKED(IDC_MATCHLENCHECK, &COptionDialog::OnBnClickedMatchlencheck)
END_MESSAGE_MAP()


// COptionDialog 메시지 처리기입니다.

BOOL COptionDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_bMatchLen = g_bMatchLen;
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void COptionDialog::OnBnClickedMatchlencheck()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData();
}
