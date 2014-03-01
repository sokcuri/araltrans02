  // PageMain.cpp : implementation file
//

#pragma warning(disable:4996)
#pragma warning(disable:4101)

#include "stdafx.h"
#include "ATCode.h"
#include "PageMain.h"
#include "OptionMgr.h"
#include "OptionDlg.h"
#include "RegistryMgr/cRegistryMgr.h"

// CPageMain dialog

IMPLEMENT_DYNAMIC(CPageMain, CDialog)

LPCTSTR _FONT_LOAD_DESC[] = {
	_T("한글 폰트를 로드하지 않습니다."),
	_T("문자 출력 함수에 한글폰트를 적용합니다."),
	_T("한글폰트 적용 후 복구하지 않습니다."),
	_T("폰트 로드 시 한글 폰트를 로드해줍니다."),
	_T("프로그램의 모든 폰트를 한글로 바꿉니다.")
};

LPCTSTR _UNIKO_LOAD_DESC[] = {
	_T("유니코드 KoFilter 사용하지 않습니다."),
	_T("일어가 1글자이상이면 번역"),
	_T("한글×2 > 일어인 경우 번역 중지"),
	_T("한글 > 일어인 경우 번역 중지"),
	_T("한글 > 일어×2인 경우 번역 중지"),
	_T("한글 > 일어×3인 경우 번역 중지"),
	_T("한글이 1글자 이상이면 번역 중지")
};

LPCTSTR _M2W_LOAD_DESC[] = {
	_T("MultiByteToWideChar의 코드페이지를 변경하지 않습니다."),
	_T("일어가 1글자도 없으면 코드페이지 변경"),
	_T("한글×2 > 일어인 경우 코드페이지 변경"),
	_T("한글 > 일어인 경우 코드페이지 변경"),
	_T("한글 > 일어×2인 경우 코드페이지 변경"),
	_T("한글이 1글자 이상이면 코드페이지 변경")
};


CPageMain::CPageMain(CWnd* pParent /*=NULL*/)
	: CDialog(CPageMain::IDD, pParent), m_pRootNode(NULL), m_nEncodeKor(0)
{

}

CPageMain::~CPageMain()
{
}

void CPageMain::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_CHK_FORCE_FONT, m_chkForceFont);
	DDX_Control(pDX, IDC_SLIDER1, m_sliderFontLoad);
	DDX_Control(pDX, IDC_EDIT_FONTDESC, m_editFontDesc);
	DDX_Control(pDX, IDC_CHK_FIX_SIZE, m_chkFixSize);
	DDX_Control(pDX, IDC_CHK_ENCODEKOR, m_chkEncodeKor);
	DDX_Control(pDX, IDC_BTN_FONT, m_btnFont);
	DDX_Control(pDX, IDC_CHK_UITRANS, m_chkUITrans);
	DDX_Control(pDX, IDC_CHK_NOASLR, m_chkNoAslr);
	DDX_Control(pDX, IDC_SLIDER2, m_sliderUniKofilter);
	DDX_Control(pDX, IDC_SLIDER3, m_sliderM2W);
	DDX_Control(pDX, IDC_CHK_COMPAREJP, m_chkComJP);
}

BOOL CPageMain::OnInitDialog()
{
	BOOL bRetVal = CDialog::OnInitDialog();

	m_sliderFontLoad.SetRange(0,4);
	m_sliderFontLoad.SetPos(0);
	m_sliderFontLoad.SetTicFreq(1);
	m_sliderUniKofilter.SetRange(0,6);
	m_sliderUniKofilter.SetPos(0);
	m_sliderUniKofilter.SetTicFreq(1);
	m_sliderM2W.SetRange(0,5);
	m_sliderM2W.SetPos(0);
	m_sliderM2W.SetTicFreq(1);

	return bRetVal;
}

BEGIN_MESSAGE_MAP(CPageMain, CDialog)
	ON_BN_CLICKED(IDC_BTN_FONT, &CPageMain::OnBnClickedBtnFont)
	ON_BN_CLICKED(IDC_CHK_FIX_SIZE, &CPageMain::OnBnClickedChkFixSize)
	ON_BN_CLICKED(IDC_CHK_ENCODEKOR, &CPageMain::OnBnClickedChkEncodekor)	
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_BTN_ADD_HOOK, &CPageMain::OnBnClickedBtnAddHook)
	ON_BN_CLICKED(IDC_CHK_UITRANS, &CPageMain::OnBnClickedChkUitrans)
	ON_BN_CLICKED(IDC_CHK_NOASLR, &CPageMain::OnBnClickedChkNoAslr)
	ON_BN_CLICKED(IDC_CHK_COMPAREJP, &CPageMain::OnBnClickedChkComparejp)
	ON_BN_CLICKED(IDC_BTN_HELP, &CPageMain::OnBnClickedBtnHelp)
	ON_BN_CLICKED(IDC_RADIO_ENCODEKOR1, &CPageMain::OnBnClickedRadioEncodekor1)
	ON_BN_CLICKED(IDC_RADIO_ENCODEKOR2, &CPageMain::OnBnClickedRadioEncodekor2)
END_MESSAGE_MAP()


// CPageMain message handlers

void CPageMain::OnBnClickedBtnFont()
{
	// TODO: Add your control notification handler code here
	
	LOGFONT logfont;
	ZeroMemory(&logfont, sizeof(LOGFONT));
	
	// 현재 폰트 노드를 가지고 LOGFONT 구조체 값 세팅
	COptionNode* pFontNode = m_pRootNode->GetChild(_T("FONT"));
	if(pFontNode)
	{
		COptionNode* pFontFaceNode = pFontNode->GetChild(0);
		if(pFontFaceNode) _tcscpy(logfont.lfFaceName, pFontFaceNode->GetValue());
		COptionNode* pFontSizeNode = pFontNode->GetChild(1);
		if(pFontSizeNode) logfont.lfHeight = (LONG) _ttoi(pFontSizeNode->GetValue());
	}
	

	// 폰트 다이얼로그
	CFontDialog fd(&logfont, CF_EFFECTS | CF_SCREENFONTS | CF_NOVERTFONTS);
	if( fd.DoModal() == IDOK )
	{
		// LOGFONT 구조체를 보고 폰트 노드를 세팅
		if(NULL==pFontNode)
		{
			pFontNode = m_pRootNode->CreateChild();
			pFontNode->SetValue(_T("FONT"));
		}

		// 폰트 페이스명
		COptionNode* pFontFaceNode = pFontNode->GetChild(0);
		if(NULL==pFontFaceNode)
		{
			pFontFaceNode = pFontNode->CreateChild();
		}
		pFontFaceNode->SetValue(logfont.lfFaceName);
		if( pFontFaceNode->GetValue().IsEmpty() ) pFontFaceNode->SetValue(_T("궁서"));

		// 폰트 사이즈
		COptionNode* pFontSizeNode = pFontNode->GetChild(1);
		if(NULL==pFontSizeNode)
		{
			pFontSizeNode = pFontNode->CreateChild();
		}
		CString strSize;
		strSize.Format(_T("%d"), logfont.lfHeight);
		pFontSizeNode->SetValue(strSize);

		// 선택된 폰트 표시
		CString strFontDesc;
		strFontDesc.Format(_T("%s,%d"), logfont.lfFaceName, logfont.lfHeight);
		m_editFontDesc.SetWindowText(strFontDesc);

		COptionDlg::_Inst->m_btnApply.EnableWindow(TRUE);

	}
}

BOOL CPageMain::InitFromRootNode( COptionNode* pRootNode )
{
	BOOL bRetVal = FALSE;

	try
	{
		if(NULL==pRootNode) throw -1;

		// 컨트롤들 기본 상태로 세팅
		ClearCtrlValues();

		// 모든 노드 순회
		int cnt = pRootNode->GetChildCount();
		for(int i=0; i<cnt; i++)
		{
			COptionNode* pNode = pRootNode->GetChild(i);
			CString strValue = pNode->GetValue().MakeUpper();

			// FORCEFONT 옵션
			if(strValue == _T("FORCEFONT"))
			{
				COptionNode* pLevelNode = pNode->GetChild(0);
				
				// 과거 형식과 호환을 위해
				if(NULL==pLevelNode)
				{
					pLevelNode = pNode->CreateChild();
					pLevelNode->SetValue(_T("10"));
				}
				
				int nLevel = _ttoi(pLevelNode->GetValue().Trim());
				nLevel /= 5;
				this->GetDlgItem(IDC_FONT_DESC)->SetWindowText(_FONT_LOAD_DESC[nLevel]);
				m_sliderFontLoad.SetPos(nLevel);

				m_chkFixSize.EnableWindow(TRUE);
				m_btnFont.EnableWindow(TRUE);
			}

			// UNIKOFILTER 옵션
			if(strValue == _T("UNIKOFILTER"))
			{
				COptionNode* pLevelNode = pNode->GetChild(0);

				int nLevel = _ttoi(pLevelNode->GetValue().Trim());
				nLevel /= 5;
				this->GetDlgItem(IDC_UNIKO_DESC)->SetWindowText(_UNIKO_LOAD_DESC[nLevel]);
				m_sliderUniKofilter.SetPos(nLevel);
			}

			// MULTTOWIDE 옵션
			if(strValue == _T("MULTTOWIDE"))
			{
				COptionNode* pLevelNode = pNode->GetChild(0);

				int nLevel = _ttoi(pLevelNode->GetValue().Trim());
				nLevel /= 5;
				this->GetDlgItem(IDC_M2W_DESC)->SetWindowText(_M2W_LOAD_DESC[nLevel]);
				m_sliderM2W.SetPos(nLevel);
			}

			// FIXFONTSIZE 옵션
			else if(strValue == _T("FIXFONTSIZE"))
			{
				m_chkFixSize.SetCheck(1);
			}

			// FONT 옵션
			else if(strValue == _T("FONT"))
			{
				CString strFontDesc = pNode->GetChild(0)->GetValue();
				if(pNode->GetChild(1))
				{
					strFontDesc += _T(',');
					strFontDesc += pNode->GetChild(1)->GetValue();
				}
				m_editFontDesc.SetWindowText( strFontDesc );
			}

			// ENCODEKOR 옵션
			else if(strValue == _T("ENCODEKOR"))
			{
				m_chkEncodeKor.SetCheck(1);

				if (pNode->GetChildCount())
				{
					COptionNode *pEncodeOptionNode = pNode->GetChild(0);
					m_nEncodeKor = _ttoi(pEncodeOptionNode->GetValue().Trim());
				}
				else
					m_nEncodeKor = 1;

				CButton * pButton = (CButton *)(this->GetDlgItem(IDC_RADIO_ENCODEKOR2));
				pButton->EnableWindow(TRUE);

				if (m_nEncodeKor == 2)
					pButton->SetCheck(1);

				pButton = (CButton *)(this->GetDlgItem(IDC_RADIO_ENCODEKOR1));
				pButton->EnableWindow(TRUE);

				if (m_nEncodeKor != 2)
					pButton->SetCheck(1);

			}

			// UITRANS 옵션
			else if(strValue == _T("UITRANS"))
			{
				m_chkUITrans.SetCheck(1);
			}

			// NOASLR 옵션
			else if(strValue == _T("NOASLR"))
			{
				m_chkNoAslr.SetCheck(0);
			}

			// COMPAREJP 옵션
			else if(strValue == _T("COMPAREJP"))
			{
				m_chkComJP.SetCheck(1);
			}
		}

		bRetVal = TRUE;

		m_pRootNode = pRootNode;


	}
	catch (int nErr)
	{
	}

	return bRetVal;
}



//////////////////////////////////////////////////////////////////////////
//
// 모든 UI 컨트롤 초기화
//
//////////////////////////////////////////////////////////////////////////
void CPageMain::ClearCtrlValues()
{
	// 폰트설정 관련 UI
	m_chkFixSize.SetCheck(0);
	m_chkFixSize.EnableWindow(FALSE);
	m_editFontDesc.Clear();
	m_editFontDesc.EnableWindow(FALSE);
	m_btnFont.EnableWindow(FALSE);
	m_chkEncodeKor.SetCheck(0);
	m_chkUITrans.SetCheck(0);
	m_chkNoAslr.SetCheck(1);
	m_chkComJP.SetCheck(0);
}



//////////////////////////////////////////////////////////////////////////
//
// 새로운 후킹코드 추가
//
//////////////////////////////////////////////////////////////////////////
void CPageMain::SetChildNodeFromCheckbox(COptionNode* pParentNode, LPCTSTR cszChildName, CButton& checkbox, BOOL reverse)
{
	if(NULL==pParentNode || NULL==cszChildName || _T('\0')==cszChildName[0]) return;

	COptionNode* pNode = pParentNode->GetChild(cszChildName);

	// 역체크일 경우
	if(reverse)	reverse=!checkbox.GetCheck();
	else reverse=checkbox.GetCheck();
	// 체크한 경우
	if(reverse)
	{
		if(NULL==pNode)
		{
			pNode = pParentNode->CreateChild();
			pNode->SetValue(cszChildName);
		}
	}
	// 체크 해제 한 경우
	else
	{
		if(pNode)
		{
			pParentNode->DeleteChild(pNode);
		}
	}

	
	if( COptionDlg::_Inst && ::IsWindow(COptionDlg::_Inst->m_btnApply.m_hWnd))
	{
		COptionDlg::_Inst->m_btnApply.EnableWindow(TRUE);
	}
}



//////////////////////////////////////////////////////////////////////////
//
// 모든 UI 컨트롤 이벤트 핸들러
//
//////////////////////////////////////////////////////////////////////////
/*
void CPageMain::OnBnClickedChkForceFont()
{
	SetChildNodeFromCheckbox(m_pRootNode, _T("FORCEFONT"), m_chkForceFont);
	
	BOOL bEnable = (BOOL)m_chkForceFont.GetCheck();
	
	m_chkFixSize.EnableWindow(bEnable);
	m_editFontDesc.EnableWindow(bEnable);
	m_btnFont.EnableWindow(bEnable);

}
*/

void CPageMain::OnBnClickedChkFixSize()
{
	SetChildNodeFromCheckbox(m_pRootNode, _T("FIXFONTSIZE"), m_chkFixSize);
}


void CPageMain::OnBnClickedChkEncodekor()
{
	SetChildNodeFromCheckbox(m_pRootNode, _T("ENCODEKOR"), m_chkEncodeKor);

	if (m_chkEncodeKor.GetCheck())
	{
		GetDlgItem(IDC_RADIO_ENCODEKOR1)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_ENCODEKOR2)->EnableWindow(TRUE);

		TCHAR szOption[2]={NULL, };
		szOption[0] = _T('0') + m_nEncodeKor;

		COptionNode *pNode = m_pRootNode->GetChild(_T("ENCODEKOR"));
		pNode = pNode->CreateChild();
		pNode->SetValue(szOption);
	}
	else
	{
		GetDlgItem(IDC_RADIO_ENCODEKOR1)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_ENCODEKOR2)->EnableWindow(FALSE);
	}
}


void CPageMain::OnBnClickedChkUitrans()
{
	SetChildNodeFromCheckbox(m_pRootNode, _T("UITRANS"), m_chkUITrans);
}

void CPageMain::OnBnClickedChkNoAslr()
{
	SetChildNodeFromCheckbox(m_pRootNode, _T("NOASLR"), m_chkNoAslr, TRUE);
}

void CPageMain::OnBnClickedBtnAddHook()
{
	// TODO: Add your control notification handler code here
	COptionDlg::_Inst->OnBnClickedBtnAddHook();
}


void CPageMain::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	// 어떤 슬라이더인지 검사
	if(pScrollBar == (CScrollBar*)&m_sliderFontLoad)
	{
		// 기존 폰트 로드 값을 지움
		COptionNode* pNode = m_pRootNode->GetChild(_T("FORCEFONT"));
		if(pNode)
		{
			m_pRootNode->DeleteChild(pNode);
		}

		// 슬라이더로부터 값을 얻어옴
		int pos = m_sliderFontLoad.GetPos();

		// 업뎃
		CString strValue = _T("");
		this->GetDlgItem(IDC_FONT_DESC)->SetWindowText(_FONT_LOAD_DESC[pos]);
		switch(pos)
		{
		case 1:
			strValue = _T("5");
			break;
		case 2:
			strValue = _T("10");
			break;
		case 3:
			strValue = _T("15");
			break;
		case 4:
			strValue = _T("20");
			break;
		default:
			pNode = NULL;
		}

		BOOL bEnable = FALSE;
		if(!strValue.IsEmpty())
		{
			bEnable = TRUE;
			pNode = m_pRootNode->CreateChild();
			pNode->SetValue(_T("FORCEFONT"));
			pNode = pNode->CreateChild();
			pNode->SetValue(strValue);
		}

		m_chkFixSize.EnableWindow(bEnable);
		m_editFontDesc.EnableWindow(bEnable);
		m_btnFont.EnableWindow(bEnable);

	}

	// 어떤 슬라이더인지 검사
	else if(pScrollBar == (CScrollBar*)&m_sliderUniKofilter)
	{
		// 기존 폰트 로드 값을 지움
		COptionNode* pNode = m_pRootNode->GetChild(_T("UNIKOFILTER"));
		if(pNode)
		{
			m_pRootNode->DeleteChild(pNode);
		}

		// 슬라이더로부터 값을 얻어옴
		int pos = m_sliderUniKofilter.GetPos();

		// 업뎃
		CString strValue = _T("");
		this->GetDlgItem(IDC_UNIKO_DESC)->SetWindowText(_UNIKO_LOAD_DESC[pos]);
		switch(pos)
		{
		case 1:
			strValue = _T("5");
			break;
		case 2:
			strValue = _T("10");
			break;
		case 3:
			strValue = _T("15");
			break;
		case 4:
			strValue = _T("20");
			break;
		case 5:
			strValue = _T("25");
			break;
		case 6:
			strValue = _T("30");
			break;
		default:
			pNode = NULL;
		}

		BOOL bEnable = FALSE;
		if(!strValue.IsEmpty())
		{
			bEnable = TRUE;
			pNode = m_pRootNode->CreateChild();
			pNode->SetValue(_T("UNIKOFILTER"));
			pNode = pNode->CreateChild();
			pNode->SetValue(strValue);
		}
	}

	// 어떤 슬라이더인지 검사
	else if(pScrollBar == (CScrollBar*)&m_sliderM2W)
	{
		// 기존 폰트 로드 값을 지움
		COptionNode* pNode = m_pRootNode->GetChild(_T("MULTTOWIDE"));
		if(pNode)
		{
			m_pRootNode->DeleteChild(pNode);
		}

		// 슬라이더로부터 값을 얻어옴
		int pos = m_sliderM2W.GetPos();

		// 업뎃
		CString strValue = _T("");
		this->GetDlgItem(IDC_M2W_DESC)->SetWindowText(_M2W_LOAD_DESC[pos]);
		switch(pos)
		{
		case 1:
			strValue = _T("5");
			break;
		case 2:
			strValue = _T("10");
			break;
		case 3:
			strValue = _T("15");
			break;
		case 4:
			strValue = _T("20");
			break;
		case 5:
			strValue = _T("25");
			break;
		default:
			pNode = NULL;
		}

		BOOL bEnable = FALSE;
		if(!strValue.IsEmpty())
		{
			bEnable = TRUE;
			pNode = m_pRootNode->CreateChild();
			pNode->SetValue(_T("MULTTOWIDE"));
			pNode = pNode->CreateChild();
			pNode->SetValue(strValue);
		}
	}

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}
void CPageMain::OnBnClickedChkComparejp()
{
	SetChildNodeFromCheckbox(m_pRootNode, _T("COMPAREJP"), m_chkComJP);
}

void CPageMain::OnBnClickedBtnHelp()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString path = CRegistryMgr::RegRead(_T("HKEY_CURRENT_USER\\Software\\AralGood"), _T("AralTransHomeDir")) + _T("\\help.chm");
	::ShellExecute(::GetDesktopWindow(), L"open", path , 0, 0, SW_SHOWDEFAULT);
}

void CPageMain::OnBnClickedRadioEncodekor1()
{
	m_nEncodeKor = 1;

	COptionNode *pNode = m_pRootNode->GetChild(_T("ENCODEKOR"));

	if (!pNode->GetChildCount())
		pNode = pNode->CreateChild();
	else
		pNode = pNode->GetChild(0);

	pNode->SetValue(_T("1"));

}

void CPageMain::OnBnClickedRadioEncodekor2()
{
	m_nEncodeKor = 2;

	COptionNode *pNode = m_pRootNode->GetChild(_T("ENCODEKOR"));

	if (!pNode->GetChildCount())
		pNode = pNode->CreateChild();
	else
		pNode = pNode->GetChild(0);

	pNode->SetValue(_T("2"));

}
