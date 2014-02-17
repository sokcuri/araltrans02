// OptionPage2.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Resource.h"
#include "OptionPage2.h"


// COptionPage2 대화 상자입니다.

IMPLEMENT_DYNAMIC(COptionPage2, CPropertyPage)

COptionPage2::COptionPage2()
	: CPropertyPage(COptionPage2::IDD)
	, m_nSelect(65536)
	, m_nItemN(0)
	, m_bOK(false)
{
	m_psp.dwFlags &= ~PSP_HASHELP;
}

COptionPage2::~COptionPage2()
{
}

BOOL COptionPage2::OnInitDialog()
{

	CPropertyPage::OnInitDialog();
	
	m_ctrFuncList.ModifyStyle(LVS_TYPEMASK,LVS_REPORT|LVS_SHOWSELALWAYS | LVS_NOSORTHEADER);
	m_ctrFuncList.SetExtendedStyle(m_ctrFuncList.GetExtendedStyle() | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	LV_COLUMN lvColumn;
	lvColumn.mask = LVCF_FMT|LVCF_TEXT|LVCF_WIDTH;
	lvColumn.fmt=LVCFMT_CENTER;
	lvColumn.cx=40;
	lvColumn.pszText = L"종류";
	lvColumn.cchTextMax = 5;
	m_ctrFuncList.InsertColumn(0,&lvColumn);
	m_ctrFuncList.SetColumn(0,&lvColumn);
	m_ctrFuncList.InsertColumn(1,L"패턴 내용",LVCFMT_LEFT,260);
	m_ctrFuncList.SetColumnWidth(1,LVSCW_AUTOSIZE_USEHEADER);

	m_ctrFuncList.DeleteAllItems();

	for(m_nItemN=0;m_nItemN<m_FuncPool2.GetSize();m_nItemN++)
	{
		CString strTmp=L"";
		if(m_FuncPool3[m_nItemN]==1)
		{
			strTmp=L"문두";
		}
		else if(m_FuncPool3[m_nItemN]==2)
		{
			strTmp=L"문미";
		}
		else if(m_FuncPool3[m_nItemN]==3)
		{
			strTmp=L"일치";
		}
		else
		{
			strTmp=L"일반";
		}
		m_ctrFuncList.InsertItem(m_nItemN,strTmp);
		m_ctrFuncList.SetItemText(m_nItemN,1,m_FuncPool2[m_nItemN]);
	}

	m_bOK=false;
	return FALSE;
}

void COptionPage2::OnOK()
{
	UpdateData();
	m_bOK=true;
}

void COptionPage2::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB2_FuncList, m_ctrFuncList);
}


BEGIN_MESSAGE_MAP(COptionPage2, CPropertyPage)
	ON_BN_CLICKED(IDC_TAB2_BUTTON3, &COptionPage2::OnBnClickedTab2Button3)
	ON_BN_CLICKED(IDC_TAB2_BUTTON5, &COptionPage2::OnBnClickedTab2Button5)
	ON_BN_CLICKED(IDC_TAB2_BUTTON4, &COptionPage2::OnBnClickedTab2Button4)
	ON_NOTIFY(NM_CLICK, IDC_TAB2_FuncList, &COptionPage2::OnNMClickTab2Funclist)
	ON_NOTIFY(NM_DBLCLK, IDC_TAB2_FuncList, &COptionPage2::OnNMDblclkTab2Funclist)
END_MESSAGE_MAP()


// COptionPage2 메시지 처리기입니다.

void COptionPage2::OnBnClickedTab2Button3()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	//함수 추가
	m_cAddFunc.m_FuncPool.clear();
	m_cAddFunc.m_strText.Empty();
	m_cAddFunc.m_nType=0;
	m_cAddFunc.m_bTrim=FALSE;
	if(m_cAddFunc.DoModal()==IDOK)
	{
		m_FuncPool.push_back(m_cAddFunc.m_FuncPool);
		m_FuncPool2.Add(m_cAddFunc.m_strText);
		m_FuncPool3.Add(m_cAddFunc.m_nType);
		m_FuncPool4.Add(m_cAddFunc.m_bTrim!=FALSE?true:false);
		
		CString strTmp=L"";
		if(m_FuncPool3[m_nItemN]==1)
		{
			strTmp=L"문두";
		}
		else if(m_FuncPool3[m_nItemN]==2)
		{
			strTmp=L"문미";
		}
		else if(m_FuncPool3[m_nItemN]==3)
		{
			strTmp=L"일치";
		}
		else
		{
			strTmp=L"일반";
		}

		m_ctrFuncList.InsertItem(m_nItemN,strTmp);
		m_ctrFuncList.SetItemText(m_nItemN,1,m_FuncPool2[m_nItemN]);

		m_nItemN++;
	}

}

void COptionPage2::OnBnClickedTab2Button5()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	//함수 수정

	if(m_nSelect>=0&&m_nSelect<m_nItemN)
	{
		m_cAddFunc.m_FuncPool=m_FuncPool[m_nSelect];
		m_cAddFunc.m_strText=m_FuncPool2[m_nSelect];
		m_cAddFunc.m_nType=m_FuncPool3[m_nSelect];
		m_cAddFunc.m_bTrim=m_FuncPool4[m_nSelect];
		if(m_cAddFunc.DoModal()==IDOK)
		{
			m_FuncPool[m_nSelect]=m_cAddFunc.m_FuncPool;
			m_FuncPool2[m_nSelect] = m_cAddFunc.m_strText;
			m_FuncPool3[m_nSelect] = m_cAddFunc.m_nType;
			m_FuncPool4[m_nSelect] = m_cAddFunc.m_bTrim!=FALSE?true:false;

			CString strTmp;
			if(m_FuncPool3[m_nSelect]==1)
			{
				strTmp=L"문두";
			}
			else if(m_FuncPool3[m_nSelect]==2)
			{
				strTmp=L"문미";
			}
			else if(m_FuncPool3[m_nSelect]==3)
			{
				strTmp=L"일치";
			}
			else
			{
				strTmp=L"일반";
			}
			m_FuncPool2[m_nSelect]=m_cAddFunc.m_strText;
			m_ctrFuncList.SetItemText(m_nSelect,1,m_FuncPool2[m_nSelect]);

			m_ctrFuncList.SetItemText(m_nSelect,0,strTmp);
		}
	}
}

void COptionPage2::OnBnClickedTab2Button4()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	//함수 제거
	if(m_nSelect>=0&&m_nSelect<m_nItemN)
	{
		m_FuncPool.erase(m_FuncPool.begin()+m_nSelect);
		m_FuncPool2.RemoveAt(m_nSelect);
		m_FuncPool3.RemoveAt(m_nSelect);
		m_FuncPool4.RemoveAt(m_nSelect);
		m_ctrFuncList.DeleteItem(m_nSelect);
		
		m_nItemN--;
		m_nSelect--;
	}
}

void COptionPage2::OnNMClickTab2Funclist(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
	NM_LISTVIEW * pNMListView = (NM_LISTVIEW*) pNMHDR;

	m_nSelect=pNMListView->iItem;
}

void COptionPage2::OnNMDblclkTab2Funclist(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
	NM_LISTVIEW * pNMListView = (NM_LISTVIEW*) pNMHDR;

	m_nSelect=pNMListView->iItem;
	OnBnClickedTab2Button5();
}
