// Dlg_AddFunc.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "FixLine.h"
#include "Dlg_AddFunc.h"


// CDlg_AddFunc 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlg_AddFunc, CDialog)

CDlg_AddFunc::CDlg_AddFunc(CWnd* pParent /*=NULL*/)
	: CDialog(CDlg_AddFunc::IDD, pParent)
	, m_nSelect(0)
	, m_nItemN(0)
	, m_strText(_T(""))
	, m_nType(0)
	, m_bTrim(FALSE)
{

}

CDlg_AddFunc::~CDlg_AddFunc()
{
}

void CDlg_AddFunc::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ctrList);
	DDX_Text(pDX, IDC_EDIT1, m_strText);
	DDX_Radio(pDX, IDC_RADIO1, m_nType);
	DDX_Check(pDX, IDC_CHECK2, m_bTrim);
}

BOOL CDlg_AddFunc::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_cToolTip.Create(this);
	m_cToolTip.SetMaxTipWidth(700);
	
	///////////////////////////////
	//툴팁 설명 시작!!!
	///////////////////////////////

	//예제
	//m_cToolTip.AddTool(&m_edit, L"텍스트 박스입니다.");
	//m_cToolTip.AddTool(GetDlgItem(IDC_Check_Len_Min),L"뭥미");

	//문두, 문미 설정
	m_cToolTip.AddTool(GetDlgItem(IDC_RADIO1),L"해당 패턴이 특별한 속성을 가지지 않도록 합니다.\r\n대부분의 경우 이 값이 적절합니다.");
	m_cToolTip.AddTool(GetDlgItem(IDC_RADIO2),L"해당 패턴이 꼭 문자열의 시작 지점에 올 경우(와야할 경우) 사용합니다.");
	m_cToolTip.AddTool(GetDlgItem(IDC_RADIO3),L"해당 패턴이 꼭 문자열의 마지막 지점에 올 경우(와야할 경우) 사용합니다.");

	///////////////////////////////
	//툴팁 설명 끝!!!
	///////////////////////////////

	m_ctrList.ModifyStyle(LVS_TYPEMASK,LVS_REPORT|LVS_SHOWSELALWAYS|LVS_NOSORTHEADER);
	m_ctrList.SetExtendedStyle(m_ctrList.GetExtendedStyle() | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	LV_COLUMN lvColumn;
	lvColumn.mask = LVCF_FMT|LVCF_TEXT|LVCF_WIDTH;
	lvColumn.fmt=LVCFMT_CENTER;
	lvColumn.cx=38;
	lvColumn.pszText = L"형식";
	lvColumn.cchTextMax = 6;
	m_ctrList.InsertColumn(0,&lvColumn);
	m_ctrList.SetColumn(0,&lvColumn);
	m_ctrList.InsertColumn(1,L"인자 내용",LVCFMT_LEFT,111);
	m_ctrList.InsertColumn(2,L"번역",LVCFMT_CENTER,38);
	m_ctrList.InsertColumn(3,L"길이",LVCFMT_CENTER,38);
	m_ctrList.InsertColumn(4,L"제외",LVCFMT_CENTER,38);
	m_ctrList.InsertColumn(5,L"통과",LVCFMT_CENTER,38);
	m_ctrList.SetColumnWidth(5,LVSCW_AUTOSIZE_USEHEADER);

	
	for(m_nItemN=0;m_nItemN<m_FuncPool.size();m_nItemN++)
	{
		CString strTemp;
		if(m_FuncPool[m_nItemN].bFunc)
		{
			m_ctrList.InsertItem(m_nItemN,L"고정");
		}
		else
		{
			m_ctrList.InsertItem(m_nItemN,L"임의");
		}
		
		m_ctrList.SetItemText(m_nItemN,1,m_FuncPool[m_nItemN].strText.c_str());


		if(m_FuncPool[m_nItemN].bTrans)
				strTemp=L"○";
			else
				strTemp=L"×";
		m_ctrList.SetItemText(m_nItemN,2,strTemp);

		switch(m_FuncPool[m_nItemN].nSize)
		{
		case -1:
			m_ctrList.SetItemText(m_nItemN,3,L"자동");
			break;
		case -2:
			m_ctrList.SetItemText(m_nItemN,3,L"×");
			break;
		default:
			strTemp.Format(L"%d",m_FuncPool[m_nItemN].nSize);
			m_ctrList.SetItemText(m_nItemN,3,strTemp);
		}

		if(m_FuncPool[m_nItemN].bDel)
				strTemp=L"○";
			else
				strTemp=L"×";
		m_ctrList.SetItemText(m_nItemN,4,strTemp);

		if(m_FuncPool[m_nItemN].bPass)
				strTemp=L"○";
			else
				strTemp=L"×";
		m_ctrList.SetItemText(m_nItemN,5,strTemp);
	}

	return FALSE;
}

BEGIN_MESSAGE_MAP(CDlg_AddFunc, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &CDlg_AddFunc::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON3, &CDlg_AddFunc::OnBnClickedButton3)
	ON_NOTIFY(NM_CLICK, IDC_LIST1, &CDlg_AddFunc::OnNMClickList1)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, &CDlg_AddFunc::OnNMDblclkList1)
	ON_BN_CLICKED(IDC_BUTTON2, &CDlg_AddFunc::OnBnClickedButton2)
	ON_BN_CLICKED(IDOK, &CDlg_AddFunc::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlg_AddFunc 메시지 처리기입니다.

void CDlg_AddFunc::OnBnClickedButton1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CDlg_AddSubFunc2 m_cAddSubFunc;
	//인자 수정
	if(m_nSelect>=0&&m_nSelect<m_nItemN)
	{
		m_cAddSubFunc.nSize=m_FuncPool[m_nSelect].nSize;
		m_cAddSubFunc.bTrans=m_FuncPool[m_nSelect].bTrans;
		m_cAddSubFunc.bDel=m_FuncPool[m_nSelect].bDel;
		m_cAddSubFunc.bPass=m_FuncPool[m_nSelect].bPass;
		if(m_FuncPool[m_nSelect].bFunc) m_cAddSubFunc.strText=m_FuncPool[m_nSelect].strText.c_str();
		else							m_cAddSubFunc.strText=L"";
		
		m_cAddSubFunc.nMin=m_FuncPool[m_nSelect].nMin;
		m_cAddSubFunc.nMax=m_FuncPool[m_nSelect].nMax;
		m_cAddSubFunc.bFunc=m_FuncPool[m_nSelect].bFunc;
		
		if(m_cAddSubFunc.DoModal()==IDOK)
		{
			FLStringElement2 FLTemp;
			FLTemp.nSize=m_cAddSubFunc.nSize;
			FLTemp.bTrans=m_cAddSubFunc.bTrans;
			FLTemp.bDel=m_cAddSubFunc.bDel;
			FLTemp.bPass=m_cAddSubFunc.bPass;
			FLTemp.strText=m_cAddSubFunc.strText;
			FLTemp.nMin=m_cAddSubFunc.nMin;
			FLTemp.nMax=m_cAddSubFunc.nMax;
			FLTemp.bFunc=m_cAddSubFunc.bFunc;
			m_FuncPool[m_nSelect] = FLTemp;

			CString strTemp;
			if(m_FuncPool[m_nSelect].bFunc)
			{
				m_ctrList.SetItemText(m_nSelect,0,L"고정");
			}
			else
			{
				m_ctrList.SetItemText(m_nSelect,0,L"임의");
			}
			
			m_ctrList.SetItemText(m_nSelect,1,m_FuncPool[m_nSelect].strText.c_str());

			if(m_FuncPool[m_nSelect].bTrans)
				strTemp=L"○";
			else
				strTemp=L"×";
			m_ctrList.SetItemText(m_nSelect,2,strTemp);

			switch(m_FuncPool[m_nSelect].nSize)
			{
			case -1:
				m_ctrList.SetItemText(m_nSelect,3,L"자동");
				break;
			case -2:
				m_ctrList.SetItemText(m_nSelect,3,L"×");
				break;
			default:
				strTemp.Format(L"%d",m_FuncPool[m_nSelect].nSize);
				m_ctrList.SetItemText(m_nSelect,3,strTemp);
			}

			if(m_FuncPool[m_nSelect].bDel)
					strTemp=L"○";
				else
					strTemp=L"×";
			m_ctrList.SetItemText(m_nSelect,4,strTemp);

			if(m_FuncPool[m_nSelect].bPass)
					strTemp=L"○";
				else
					strTemp=L"×";
			m_ctrList.SetItemText(m_nSelect,5,strTemp);
		}
	}

	m_strText=L"";
	for(int i =0;i<m_nItemN;i++)
	{
		m_strText+=m_FuncPool[i].strText.c_str();
	}

	UpdateData(0);
}

void CDlg_AddFunc::OnBnClickedButton3()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	//인자 추가
	CDlg_AddSubFunc2 m_cAddSubFunc;
	m_cAddSubFunc.bFunc=true;
	m_cAddSubFunc.nSize=-1;
	m_cAddSubFunc.bTrans=false;
	m_cAddSubFunc.bDel=false;
	m_cAddSubFunc.bPass=false;
	m_cAddSubFunc.strText=L"";
	m_cAddSubFunc.nMin=-1;
	m_cAddSubFunc.nMax=-1;
	if(m_cAddSubFunc.DoModal()==IDOK)
	{
		FLStringElement2 FLTemp;
		FLTemp.nSize=m_cAddSubFunc.nSize;
		FLTemp.bTrans=m_cAddSubFunc.bTrans;
		FLTemp.bDel=m_cAddSubFunc.bDel;
		FLTemp.bPass=m_cAddSubFunc.bPass;
		FLTemp.strText=m_cAddSubFunc.strText;
		FLTemp.nMin=m_cAddSubFunc.nMin;
		FLTemp.nMax=m_cAddSubFunc.nMax;
		FLTemp.bFunc=m_cAddSubFunc.bFunc;
		m_FuncPool.push_back(FLTemp);

		CString strTemp;
		if(m_FuncPool[m_nItemN].bFunc)
		{
			m_ctrList.InsertItem(m_nItemN,L"고정");
		}
		else
		{
			m_ctrList.InsertItem(m_nItemN,L"임의");
		}
		
		m_ctrList.SetItemText(m_nItemN,1,m_FuncPool[m_nItemN].strText.c_str());

		if(m_FuncPool[m_nItemN].bTrans)
			strTemp=L"○";
		else
			strTemp=L"×";
		m_ctrList.SetItemText(m_nItemN,2,strTemp);


		switch(m_FuncPool[m_nItemN].nSize)
		{
		case -1:
			m_ctrList.SetItemText(m_nItemN,3,L"자동");
			break;
		case -2:
			m_ctrList.SetItemText(m_nItemN,3,L"×");
			break;
		default:
			strTemp.Format(L"%d",m_FuncPool[m_nItemN].nSize);
			m_ctrList.SetItemText(m_nItemN,3,strTemp);
		}

		if(m_FuncPool[m_nItemN].bDel)
				strTemp=L"○";
			else
				strTemp=L"×";
		m_ctrList.SetItemText(m_nItemN,4,strTemp);

		if(m_FuncPool[m_nItemN].bPass)
				strTemp=L"○";
			else
				strTemp=L"×";
		m_ctrList.SetItemText(m_nItemN,5,strTemp);
		m_nItemN++;
	}
	m_strText=L"";
	for(int i =0;i<m_nItemN;i++)
	{
		m_strText.Append(m_FuncPool[i].strText.c_str());
	}
	
	UpdateData(0);
}

void CDlg_AddFunc::OnBnClickedButton2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	//인자 삭제
	if(m_nSelect>=0&&m_nSelect<m_nItemN)
	{
		m_ctrList.DeleteItem(m_nSelect);
		m_FuncPool.erase(m_FuncPool.begin()+m_nSelect);
		
		m_nItemN--;
		m_nSelect--;
	}

	m_strText=L"";
	for(int i =0;i<m_nItemN;i++)
	{
		m_strText+=m_FuncPool[i].strText.c_str();
	}
	UpdateData(0);
}

void CDlg_AddFunc::OnNMClickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
	NM_LISTVIEW * pNMListView = (NM_LISTVIEW*) pNMHDR;

	m_nSelect=pNMListView->iItem;
}

void CDlg_AddFunc::OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
	NM_LISTVIEW * pNMListView = (NM_LISTVIEW*) pNMHDR;

	m_nSelect=pNMListView->iItem;

	OnBnClickedButton1();
}



void CDlg_AddFunc::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if(m_nItemN>0)
		OnOK();
	else
		MessageBox(L"인자가 하나도 없습니다",L"에러",MB_ICONWARNING);
}

BOOL CDlg_AddFunc::PreTranslateMessage(MSG* pMsg)
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
