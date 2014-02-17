// OptionPage1.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Resource.h"
#include "OptionPage1.h"

// COptionPage1 대화 상자입니다.

IMPLEMENT_DYNAMIC(COptionPage1, CPropertyPage)

COptionPage1::COptionPage1()
	: CPropertyPage(COptionPage1::IDD)
	, m_nMaxChar(0)
	, m_nMaxLine(0)
	, m_bLimitLine(FALSE)
	, m_strRetChar(_T(""))
	, m_bOK(false)
	, m_bLoaded(false)
	, m_bUseRetChar(FALSE)
	, m_bUseRet(FALSE)
	, m_nLenRadio(0)
	, m_bForceLen(FALSE)
	, m_nMaxLen(0)
{
	m_psp.dwFlags &= ~PSP_HASHELP;
}

COptionPage1::~COptionPage1()
{
}

void COptionPage1::OnOK()
{
	m_nMaxChar=GetDlgItemInt(IDC_TAB1_EDIT2,0,1);
	m_nMaxLine=GetDlgItemInt(IDC_TAB1_EDIT5,0,1);
	UpdateData();
	if(m_strRetChar.GetLength()==0)
	{
		MessageBox(L"개행 기호는 공란으로 둘 수 없습니다!",L"경고",MB_ICONWARNING);
		m_strRetChar=L"\\n";
	}
	m_bOK=true;

	if(m_nLenRadio==0)
	{
		m_nMaxLen=-2;
		m_bForceLen=FALSE;
	}
	else if(m_nLenRadio==1)
	{
		m_nMaxLen=-1;
		m_bForceLen=TRUE;
	}
	else if(m_nLenRadio==2)
	{
		m_nMaxLen=GetDlgItemInt(IDC_TAB1_EDIT7);
	}

}

BOOL COptionPage1::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	m_cToolTip.Create(this);
	m_cToolTip.SetMaxTipWidth(700);
	
	///////////////////////////////
	//툴팁 설명 시작!!!
	///////////////////////////////

	//예제
	//m_cToolTip.AddTool(&m_edit, L"텍스트 박스입니다.");
	//m_cToolTip.AddTool(GetDlgItem(IDC_Check_Len_Min),L"뭥미");

	//사용 안함 설정
	m_cToolTip.AddTool(GetDlgItem(IDC_CHECK3),L"개행문자에 따른 개행 처리를 사용하지 않습니다.");

	////////////////개행 설정////////////////////
	//한줄당 최대 바이트
	m_cToolTip.AddTool(GetDlgItem(IDC_TAB1_EDIT2),L"한 줄에 들어 갈 수 있는 최대 길이를 입력합니다.\r\n값을 적절히 조절해가며 알맞은 값을 정합니다.");

	//최대 줄수
	m_cToolTip.AddTool(GetDlgItem(IDC_TAB1_EDIT5),L"최대 줄 수를 정합니다.");
	m_cToolTip.AddTool(GetDlgItem(IDC_TAB1_CHECK1),L"원문 줄 수를 넘기지 않도록 합니다.");

	//개행 문자
	m_cToolTip.AddTool(GetDlgItem(IDC_TAB1_EDIT1),L"개행의 기준이 되는 문자를 입력합니다.\r\n\\r\\n이나 %0D%0A와 같은 식으로도 입력 할 수 있습니다.");
	m_cToolTip.AddTool(GetDlgItem(IDC_CHECK1),L"개행 문자를 제거합니다.\r\n개행 문자가 없더라도 자동으로 개행처리를 하는 경우에 사용할 수 있습니다.");

	/////////////////길이제한///////////////////
	m_cToolTip.AddTool(GetDlgItem(IDC_RADIO8),L"문자열 전체 길이 제한을 사용하지 않습니다.(기본값)");
	m_cToolTip.AddTool(GetDlgItem(IDC_RADIO9),L"원문 길이 그대로 유지해서 번역 후 되돌려줍니다.\r\n원문 길이를 초과하는 경우 에러가 나는 경우 사용할 수 있습니다.");
	m_cToolTip.AddTool(GetDlgItem(IDC_RADIO10),L"지정한 길이내로 유지해서 번역 후 되돌려줍니다.\r\n주로 대화창 최대길이를 초과할때 에러가 나는 경우 사용할 수 있습니다.");
	m_cToolTip.AddTool(GetDlgItem(IDC_TAB1_EDIT7),L"지정한 길이내로 유지해서 번역 후 되돌려줍니다.\r\n주로 대화창 최대길이를 초과할때 에러가 나는 경우 사용할 수 있습니다.");
	m_cToolTip.AddTool(GetDlgItem(IDC_CHECK2),L"'다음길이로'의 기준을\r\n'길이'(패턴의 개행처리 O,X 여부)가 아니라\r\n'Byte 수'로 하도록 설정합니다.");

	///////////////////////////////
	//툴팁 설명 끝!!!
	///////////////////////////////

	m_ctrSpin1.SetRange(1,1024);
	m_ctrSpin2.SetRange(1,10);
	m_ctrSpin3.SetRange(1,1024);

	SetDlgItemInt(IDC_TAB1_EDIT2,m_nMaxChar);
	SetDlgItemInt(IDC_TAB1_EDIT5,m_nMaxLine);

	SetDisable(m_bUseRet);

	if(m_nMaxLen==-2)
	{
		OnBnClickedRadio8();
		m_nLenRadio=0;
		m_bForceLen=false;
	}
	else if(m_nMaxLen==-1)
	{
		OnBnClickedRadio9();
		m_nLenRadio=1;
		m_bForceLen=false;
	}
	else
	{
		OnBnClickedRadio10();
		SetDlgItemInt(IDC_TAB1_EDIT7,m_nMaxLen);
		m_nLenRadio=2;
	}


	UpdateData(0);
	m_bOK=false;

	m_bLoaded=true;

	return FALSE;
}

void COptionPage1::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1_SPIN2, m_ctrSpin1);
	DDX_Control(pDX, IDC_TAB1_SPIN3, m_ctrSpin2);
	DDX_Control(pDX, IDC_TAB1_SPIN5, m_ctrSpin3);
	DDX_Check(pDX, IDC_TAB1_CHECK1, m_bLimitLine);
	DDX_Text(pDX, IDC_TAB1_EDIT1, m_strRetChar);
	DDX_Check(pDX, IDC_CHECK1, m_bUseRetChar);
	DDX_Check(pDX, IDC_CHECK3, m_bUseRet);
	DDX_Radio(pDX, IDC_RADIO8, m_nLenRadio);
	DDX_Check(pDX, IDC_CHECK2, m_bForceLen);
}


BEGIN_MESSAGE_MAP(COptionPage1, CPropertyPage)
	ON_BN_CLICKED(IDC_CHECK3, &COptionPage1::OnBnClickedCheck3)
	ON_EN_CHANGE(IDC_TAB1_EDIT7, &COptionPage1::OnEnChangeTab1Edit7)
	ON_BN_CLICKED(IDC_RADIO8, &COptionPage1::OnBnClickedRadio8)
	ON_BN_CLICKED(IDC_RADIO9, &COptionPage1::OnBnClickedRadio9)
	ON_BN_CLICKED(IDC_RADIO10, &COptionPage1::OnBnClickedRadio10)
END_MESSAGE_MAP()


// COptionPage1 메시지 처리기입니다.

void COptionPage1::SetDisable(BOOL bDisable)
{
	if(bDisable)
		bDisable=false;
	else
		bDisable=true;
	GetDlgItem(IDC_TAB1_CHECK1)->EnableWindow(bDisable);
	GetDlgItem(IDC_TAB1_SPIN2)->EnableWindow(bDisable);
	GetDlgItem(IDC_TAB1_SPIN3)->EnableWindow(bDisable);
	GetDlgItem(IDC_TAB1_STATIC2)->EnableWindow(bDisable);
	GetDlgItem(IDC_TAB1_EDIT2)->EnableWindow(bDisable);
	GetDlgItem(IDC_TAB1_STATIC3)->EnableWindow(bDisable);
	GetDlgItem(IDC_TAB1_STATIC1)->EnableWindow(bDisable);
	GetDlgItem(IDC_TAB1_EDIT1)->EnableWindow(bDisable);
	GetDlgItem(IDC_TAB1_EDIT5)->EnableWindow(bDisable);
	GetDlgItem(IDC_CHECK1)->EnableWindow(bDisable);

	UpdateData(0);
}

void COptionPage1::OnBnClickedCheck3()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData();
	SetDisable(m_bUseRet);
}
void COptionPage1::OnEnChangeTab1Edit7()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CPropertyPage::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if(m_bLoaded)
		m_nMaxLen=GetDlgItemInt(IDC_TAB1_EDIT7,0,1);
}

void COptionPage1::OnBnClickedRadio8()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	GetDlgItem(IDC_TAB1_EDIT7)->EnableWindow(FALSE);
	GetDlgItem(IDC_TAB1_SPIN5)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHECK2)->EnableWindow(FALSE);
	GetDlgItem(IDC_STATIC_Byte)->EnableWindow(FALSE);
}

void COptionPage1::OnBnClickedRadio9()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	GetDlgItem(IDC_TAB1_EDIT7)->EnableWindow(FALSE);
	GetDlgItem(IDC_TAB1_SPIN5)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHECK2)->EnableWindow(FALSE);
	GetDlgItem(IDC_STATIC_Byte)->EnableWindow(FALSE);
}

void COptionPage1::OnBnClickedRadio10()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	GetDlgItem(IDC_TAB1_EDIT7)->EnableWindow(TRUE);
	GetDlgItem(IDC_TAB1_SPIN5)->EnableWindow(TRUE);
	GetDlgItem(IDC_CHECK2)->EnableWindow(TRUE);
	GetDlgItem(IDC_STATIC_Byte)->EnableWindow(TRUE);
}

BOOL COptionPage1::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	switch(pMsg->message) 
	{
    case WM_LBUTTONDOWN:            
    case WM_LBUTTONUP:              
    case WM_MOUSEMOVE:
        // 툴팁을 보여줌
        m_cToolTip.RelayEvent(pMsg);
    }
	return CPropertyPage::PreTranslateMessage(pMsg);
}
