// Dlg_AddSubFunc2.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "FixLine.h"
#include "Dlg_AddSubFunc2.h"


// CDlg_AddSubFunc2 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlg_AddSubFunc2, CDialog)

CDlg_AddSubFunc2::CDlg_AddSubFunc2(CWnd* pParent /*=NULL*/)
	: CDialog(CDlg_AddSubFunc2::IDD, pParent)
	, m_nSize(0)
	, m_bSizeAuto(false)
	, m_bSize(false)
	, m_nMin(0)
	, m_nMax(0)
	, m_bMin(false)
	, m_bMax(false)
	, nSize(0)
	, bTrans(false)
	, bDel(false)
	, bPass(false)
	, strText(_T(""))
	, nMin(0)
	, nMax(0)
	, bFunc(false)
{

}

CDlg_AddSubFunc2::~CDlg_AddSubFunc2()
{
}

void CDlg_AddSubFunc2::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_Spin_Size, m_ctrSpinSize);
	DDX_Control(pDX, IDC_Spin_Len_Min, m_ctrSpinMin);
	DDX_Control(pDX, IDC_Spin_Len_Max, m_ctrSpinMax);
}


BEGIN_MESSAGE_MAP(CDlg_AddSubFunc2, CDialog)
	ON_BN_CLICKED(IDC_RADIO_Func, &CDlg_AddSubFunc2::OnBnClickedRadioFunc)
	ON_BN_CLICKED(IDC_Radio_Text, &CDlg_AddSubFunc2::OnBnClickedRadioText)
	ON_BN_CLICKED(IDC_Radio_Size_Yes, &CDlg_AddSubFunc2::OnBnClickedRadioSizeYes)
	ON_BN_CLICKED(IDC_Radio_Size_No, &CDlg_AddSubFunc2::OnBnClickedRadioSizeNo)
	ON_BN_CLICKED(IDC_Radio_Size_Auto, &CDlg_AddSubFunc2::OnBnClickedRadioSizeAuto)
	ON_BN_CLICKED(IDC_Radio_Size_Custom, &CDlg_AddSubFunc2::OnBnClickedRadioSizeCustom)
	ON_BN_CLICKED(IDC_Check_Len_Min, &CDlg_AddSubFunc2::OnBnClickedCheckLenMin)
	ON_BN_CLICKED(IDC_Check_Len_Max, &CDlg_AddSubFunc2::OnBnClickedCheckLenMax)
	ON_BN_CLICKED(IDC_Check_Del, &CDlg_AddSubFunc2::OnBnClickedCheckDel)
	ON_BN_CLICKED(IDC_Check_Pass, &CDlg_AddSubFunc2::OnBnClickedCheckPass)
	ON_BN_CLICKED(IDC_Radio_Trans_On, &CDlg_AddSubFunc2::OnBnClickedRadioTransOn)
	ON_BN_CLICKED(IDC_Radio_Trans_Off, &CDlg_AddSubFunc2::OnBnClickedRadioTransOff)
	ON_BN_CLICKED(IDOK, &CDlg_AddSubFunc2::OnBnClickedOk)
	ON_BN_CLICKED(IDC_Text_Help, &CDlg_AddSubFunc2::OnBnClickedTextHelp)
END_MESSAGE_MAP()


// CDlg_AddSubFunc2 메시지 처리기입니다.

BOOL CDlg_AddSubFunc2::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_cToolTip.Create(this);
	m_cToolTip.SetMaxTipWidth(700);
	
	///////////////////////////////
	//툴팁 설명 시작!!!
	///////////////////////////////

	//예제
	//m_cToolTip.AddTool(&m_edit, L"텍스트 박스입니다.");
	//m_cToolTip.AddTool(GetDlgItem(IDC_Check_Len_Min),L"뭥미");

	//함수, 텍스트 설명
	m_cToolTip.AddTool(GetDlgItem(IDC_RADIO_Func),L"해당 인자의 속성을 특정 텍스트로 지정합니다.\r\n일반적인 경우 게임 함수의 시작지점, 구분자 또는\r\n특수문자를 지정하기 위해 사용됩니다.");
	m_cToolTip.AddTool(GetDlgItem(IDC_Radio_Text),L"해당 인자의 속성을 임의의 텍스트로 지정합니다.\r\n대부분의 '대사 구간'이 지정됩니다.");

	//개행 설정
	m_cToolTip.AddTool(GetDlgItem(IDC_Radio_Size_Yes),L"개행 계산시 이 인자를 계산에 포함하도록 합니다.\r\n일반적으로 본문에 쓰인 대사나 특수 문자일 때 선택합니다.");
	m_cToolTip.AddTool(GetDlgItem(IDC_Radio_Size_No),L"개행 계산시 이 인자를 계산에 포함하지 않도록 합니다.\r\n설명에 사용되는 루비 텍스트나 함수부일 때 선택합니다.");
	m_cToolTip.AddTool(GetDlgItem(IDC_Radio_Size_Auto),L"길이를 자동으로 계산합니다.\r\n일반적으로 문자열의 길이만큼입니다.");
	m_cToolTip.AddTool(GetDlgItem(IDC_Radio_Size_Custom),L"길이를 직접 지정합니다.\r\n단위는 Byte이며 값이 0인 경우 '제외'를 선택한 것과 동일합니다.");
	m_cToolTip.AddTool(GetDlgItem(IDC_Edit_Size),L"길이를 직접 지정합니다.\r\n단위는 Byte이며 값이 0인 경우 '제외'를 선택한 것과 동일합니다.");

	//번역 처리
	m_cToolTip.AddTool(GetDlgItem(IDC_Radio_Trans_On),L"이 인자를 번역합니다.\r\n일반적인 대사는 이 옵션을 선택합니다.");
	m_cToolTip.AddTool(GetDlgItem(IDC_Radio_Trans_Off),L"이 인자를 번역하지 않습니다.\r\n함수부나 특별히 취급되는 텍스트(이름, 특수문자 등)의 경우 선택합니다.");

	//길이 처리
	m_cToolTip.AddTool(GetDlgItem(IDC_Check_Len_Min),L"최소한 가져야 할 길이를 지정합니다.\r\n이 길이를 충족시키지 못했을 경우 패턴으로 인식하지 않습니다.");
	m_cToolTip.AddTool(GetDlgItem(IDC_Edit_Len_Min),L"최소한 가져야 할 길이를 지정합니다.\r\n이 길이를 충족시키지 못했을 경우 패턴으로 인식하지 않습니다.");
	m_cToolTip.AddTool(GetDlgItem(IDC_Check_Len_Max),L"가져야 할 길이의 한도를 지정합니다.\r\n이 길이를 넘길경우 패턴으로 인식하지 않습니다.");
	m_cToolTip.AddTool(GetDlgItem(IDC_Edit_Len_Max),L"가져야 할 길이의 한도를 지정합니다.\r\n이 길이를 넘길경우 패턴으로 인식하지 않습니다.");

	//문자열 정의
	m_cToolTip.AddTool(GetDlgItem(IDC_Edit_Text),L"텍스트를 정합니다.\r\n일반적인 문자 이외에도 Hex코드와 같은 경우도 지정가능합니다.\r\n자세한 내용은 버튼을 누르면 확인할 수 있습니다.");
	//m_cToolTip.AddTool(GetDlgItem(IDC_Text_Help),L"뭥미");

	//기타 설정
	m_cToolTip.AddTool(GetDlgItem(IDC_Check_Del),L"이 인자를 삭제합니다.\r\n번역 후 반환되는 문장에는 이 인자가 출력되지 않습니다.");
	m_cToolTip.AddTool(GetDlgItem(IDC_Check_Pass),L"조건을 충족하지 않을 경우 '없는 것'으로 처리하고 다음 인자를 확인하도록 합니다.");

	///////////////////////////////
	//툴팁 설명 끝!!!
	///////////////////////////////

	m_ctrSpinSize.SetRange(0,1024);
	m_ctrSpinMin.SetRange(0,1024);
	m_ctrSpinMax.SetRange(0,1024);

	//초기값 설정 (true,false);


	//문자열 정의
	SetDlgItemTextW(IDC_Edit_Text,strText.GetString());

	//기타 처리
	CheckDlgButton(IDC_Check_Del,bDel);
	CheckDlgButton(IDC_Check_Pass,bPass);

	//번역 처리
	CheckDlgButton(IDC_Radio_Trans_On,bTrans);
	CheckDlgButton(IDC_Radio_Trans_Off,!bTrans);

	//개행 설정
	if(nSize>=0)
	{
		//포함 수동
		m_bSize=true;
		m_bSizeAuto=false;
		SetDlgItemInt(IDC_Edit_Size,nSize);

		CheckDlgButton(IDC_Radio_Size_Yes,1);
		CheckDlgButton(IDC_Radio_Size_No,0);

		CheckDlgButton(IDC_Radio_Size_Auto,0);
		CheckDlgButton(IDC_Radio_Size_Custom,1);

		SetEnableSize(1);

		SetDlgItemInt(IDC_Edit_Size,nSize);
	}
	else if(nSize==-1)
	{
		//포함 자동
		m_bSize=true;
		m_bSizeAuto=true;
		SetDlgItemInt(IDC_Edit_Size,0);

		CheckDlgButton(IDC_Radio_Size_Yes,1);
		CheckDlgButton(IDC_Radio_Size_No,0);

		CheckDlgButton(IDC_Radio_Size_Auto,1);
		CheckDlgButton(IDC_Radio_Size_Custom,0);

		SetEnableSize(1);
		
	}
	else
	{
		//제외
		m_bSize=false;
		m_bSizeAuto=true;
		SetDlgItemInt(IDC_Edit_Size,0);

		CheckDlgButton(IDC_Radio_Size_Yes,0);
		CheckDlgButton(IDC_Radio_Size_No,1);

		CheckDlgButton(IDC_Radio_Size_Auto,1);
		CheckDlgButton(IDC_Radio_Size_Custom,0);

		SetEnableSize(0);
	}

	//종류 선택
	CheckDlgButton(IDC_RADIO_Func,bFunc);
	CheckDlgButton(IDC_Radio_Text,!bFunc);
	
	//길이 설정
	//최소
	if(nMin>=0)
	{
		m_nMin=nMin;
		m_bMin=true;
		SetDlgItemInt(IDC_Edit_Len_Min,m_nMin);
		CheckDlgButton(IDC_Check_Len_Min,1);
	}
	else
	{
		m_nMin=0;
		m_bMin=false;
		SetDlgItemInt(IDC_Edit_Len_Min,m_nMin);
		CheckDlgButton(IDC_Check_Len_Min,0);
	}
	//최대
	if(nMax>=0)
	{
		m_nMax=nMax;
		m_bMax=true;
		SetDlgItemInt(IDC_Edit_Len_Max,m_nMax);
		CheckDlgButton(IDC_Check_Len_Max,1);
	}
	else
	{
		m_nMax=0;
		m_bMax=false;
		SetDlgItemInt(IDC_Edit_Len_Max,m_nMax);
		CheckDlgButton(IDC_Check_Len_Max,0);
	}

	SetEnableMode(bFunc);

	return FALSE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CDlg_AddSubFunc2::OnOK()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	CDialog::OnOK();

	
}

void CDlg_AddSubFunc2::SetEnableMode(bool bFunc)
{
	bool a,b;
	if(bFunc)
	{
		//이것은 함수
		SetEnableMin(-1);
		SetEnableMax(-1);

		a=true;
		b=false;
	}
	else
	{
		//이것은 텍스트
		if(m_bMin)	SetEnableMin(1);
		else		SetEnableMin(0);

		if(m_bMax)	SetEnableMax(1);
		else		SetEnableMax(0);

		a=false;
		b=true;
	}

	//true가 Enable임 -_-;

	GetDlgItem(IDC_Edit_Text)->EnableWindow(a);
	GetDlgItem(IDC_Text_Help)->EnableWindow(a);
}

void CDlg_AddSubFunc2::SetEnableSize(int nMode)
{
	bool a,b;
	if(nMode==0)
	{
		a=false;
		b=false;
		SetEnableSizeAuto(-1);
	}
	else
	{
		a=true;
		b=true;
		if(m_bSizeAuto)
		{
			SetEnableSizeAuto(1);
		}
		else
		{
			SetEnableSizeAuto(0);
		}
		
	}

	GetDlgItem(IDC_Radio_Size_Auto)->EnableWindow(a);
	GetDlgItem(IDC_Radio_Size_Custom)->EnableWindow(a);
	
}

void CDlg_AddSubFunc2::SetEnableSizeAuto(int nMode)
{
	bool a,b;
	if(nMode==-1)
	{
		a=false;
		b=false;
	}
	else if(nMode==0)
	{
		a=true;
		b=true;
	}
	else
	{
		a=true;
		b=false;
	}
	GetDlgItem(IDC_Edit_Size)->EnableWindow(b);
	GetDlgItem(IDC_Spin_Size)->EnableWindow(b);
	GetDlgItem(IDC_STATIC_Byte2)->EnableWindow(b);
}

void CDlg_AddSubFunc2::SetEnableMin(int nMode)
{
	bool a,b;
	if(nMode==-1)
	{
		a=false;
		b=false;
	}
	else if(nMode==0)
	{
		a=true;
		b=false;
	}
	else
	{
		a=true;
		b=true;
	}

	GetDlgItem(IDC_Check_Len_Min)->EnableWindow(a);
	GetDlgItem(IDC_Edit_Len_Min)->EnableWindow(b);
	GetDlgItem(IDC_Spin_Len_Min)->EnableWindow(b);
	GetDlgItem(IDC_Spin_Len_Min)->EnableWindow(b);
	GetDlgItem(IDC_STATIC_Len_Min)->EnableWindow(b);

	//추가 텍스트는 여기서 처리한다.
	GetDlgItem(IDC_STATIC_Len1)->EnableWindow(a);
	GetDlgItem(IDC_STATIC_Len2)->EnableWindow(a);
}

void CDlg_AddSubFunc2::SetEnableMax(int nMode)
{
	bool a,b;
	if(nMode==-1)
	{
		a=false;
		b=false;
	}
	else if(nMode==0)
	{
		a=true;
		b=false;
	}
	else
	{
		a=true;
		b=true;
	}

	GetDlgItem(IDC_Check_Len_Max)->EnableWindow(a);
	GetDlgItem(IDC_Edit_Len_Max)->EnableWindow(b);
	GetDlgItem(IDC_Spin_Len_Max)->EnableWindow(b);
	GetDlgItem(IDC_STATIC_Len_Max)->EnableWindow(b);
}

void CDlg_AddSubFunc2::OnBnClickedRadioFunc()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	bFunc=true;
	SetEnableMode(bFunc);
}

void CDlg_AddSubFunc2::OnBnClickedRadioText()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	bFunc=false;
	SetEnableMode(bFunc);
}

void CDlg_AddSubFunc2::OnBnClickedRadioSizeYes()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_bSize=true;
	SetEnableSize(m_bSize);
}

void CDlg_AddSubFunc2::OnBnClickedRadioSizeNo()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_bSize=false;
	SetEnableSize(m_bSize);
}


void CDlg_AddSubFunc2::OnBnClickedRadioSizeAuto()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_bSizeAuto=true;
	SetEnableSizeAuto(m_bSizeAuto);
}

void CDlg_AddSubFunc2::OnBnClickedRadioSizeCustom()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_bSizeAuto=false;
	SetEnableSizeAuto(m_bSizeAuto);
}

void CDlg_AddSubFunc2::OnBnClickedCheckLenMin()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if(IsDlgButtonChecked(IDC_Check_Len_Min))
	{
		m_bMin=true;
	}
	else
	{
		m_bMin=false;
	}
	SetEnableMin(m_bMin);
}

void CDlg_AddSubFunc2::OnBnClickedCheckLenMax()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if(IsDlgButtonChecked(IDC_Check_Len_Max))
	{
		m_bMax=true;
	}
	else
	{
		m_bMax=false;
	}
	SetEnableMax(m_bMax);
}

void CDlg_AddSubFunc2::OnBnClickedCheckDel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if(IsDlgButtonChecked(IDC_Check_Del))
	{
		bDel=true;
	}
	else
	{
		bDel=false;
	}
}

void CDlg_AddSubFunc2::OnBnClickedCheckPass()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if(IsDlgButtonChecked(IDC_Check_Pass))
	{
		bPass=true;
	}
	else
	{
		bPass=false;
	}
}

void CDlg_AddSubFunc2::OnBnClickedRadioTransOn()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	bTrans=true;
}

void CDlg_AddSubFunc2::OnBnClickedRadioTransOff()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	bTrans=false;
}
void CDlg_AddSubFunc2::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	bool bOK=true;
	//nSize
	if(m_bSize)
	{
		if(m_bSizeAuto)
		{
			nSize=-1;
		}
		else
		{
			nSize=GetDlgItemInt(IDC_Edit_Size);
		}
	}
	else
	{
		nSize=-2;
	}
	
	//bTrans OK
	//bPass OK
	//bDell OK

	
	if(!bFunc)
	{
		//nMin
		if(m_bMin)
		{
			nMin=GetDlgItemInt(IDC_Edit_Len_Min);
		}
		else
		{
			nMin=-1;
		}
		//nMax
		if(m_bMax)
		{
			nMax=GetDlgItemInt(IDC_Edit_Len_Max);
		}
		else
		{
			nMax=-1;
		}
	}
	else
	{
		nMin=-1;
		nMax=-1;
	}

	//bFunc OK

	//strText
	if(bFunc)
	{
		GetDlgItemTextW(IDC_Edit_Text,strText.GetBuffer(2048),2048);
		if(strText==L"")
		{
			bOK=false;
			MessageBox(L"내용을 입력해주세요.",L"에러",MB_ICONWARNING);
		}
	}
	else
	{
		strText=L"{T";

		CString strTmp2;
		if(nMin>=0&&nMax>=0)	strTmp2.Format(L",%d~%d",nMin,nMax);
		else if(nMin>=0)		strTmp2.Format(L",%d~",nMin);
		else if(nMax>=0)		strTmp2.Format(L",~%d",nMax);
		strText += strTmp2;

		strText += L"}";
	}

	if(nMin!=-1&&nMax!=-1)
	{
		if(nMin>nMax)
		{
			bOK=false;
			MessageBox(L"길이 제한이 잘못되었습니다.",L"에러",MB_ICONWARNING);
		}
	}

	if(bOK)
		OnOK();
}

BOOL CDlg_AddSubFunc2::PreTranslateMessage(MSG* pMsg)
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
	return CDialog::PreTranslateMessage(pMsg);
}

void CDlg_AddSubFunc2::OnBnClickedTextHelp()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//설명서 띄웁시다~
	CString strHelp;
	strHelp+=L"문자열 란에는 일반적인 문자 뿐만 아니라\r\n";
	strHelp+=L"추가로 원하는 문자를 지정 할 수 있습니다.\r\n";
	strHelp+=L"\r\n";
	strHelp+=L"\\t　: 탭(\\t, 0x09)\r\n";
	strHelp+=L"\\r　: 폼피드 기호(\\r, 0x0D)\r\n";
	strHelp+=L"\\n　: 캐리지 리턴 기호 (\\n, 0x0A)\r\n";
	strHelp+=L"\\\\ : \\대신 사용됩니다.\r\n";
	strHelp+=L"\r\n";
	strHelp+=L"%FF : Hex코드를 입력합니다. 1바이트당 한번씩 써 주어야 합니다. \r\n";
	strHelp+=L"　　　(ex:0xA0 0x22 -> %A0%22)\r\n";
	strHelp+=L"%S　: 임의의 1바이트 문자 하나를 받습니다.\r\n";
	strHelp+=L"　　　무조건 '반각문자'(숫자, 알파벳 등)일 경우에만 해당됩니다.\r\n";
	strHelp+=L"　　　(ex:125 -> %S%S%S)\r\n";
	strHelp+=L"%T　: 임의의 2바이트 문자 하나를 받습니다.\r\n";
	strHelp+=L"　　　무조건 '전각문자'(일본어 등)일 경우에만 해당됩니다.\r\n";
	strHelp+=L"　　　(ex:おはよう -> %T%T%T%T)\r\n";
	strHelp+=L"%%　: %대신 사용됩니다.";
	MessageBox(strHelp,L"문법 도움말");
}
