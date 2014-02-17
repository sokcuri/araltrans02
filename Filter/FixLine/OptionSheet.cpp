// OptionSheet.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Resource.h"
#include "OptionSheet.h"
#include "Migration.h"


// COptionSheet

IMPLEMENT_DYNAMIC(COptionSheet, CPropertySheet)

COptionSheet::COptionSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
	AddPage(&m_cPage1);
	AddPage(&m_cPage2);

	m_psh.dwFlags |= PSH_NOAPPLYNOW;
	m_psh.dwFlags &= ~PSH_HASHELP;
}

COptionSheet::COptionSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	AddPage(&m_cPage1);
	AddPage(&m_cPage2);

	m_psh.dwFlags |= PSH_NOAPPLYNOW;
	m_psh.dwFlags &= ~PSH_HASHELP;
}

COptionSheet::~COptionSheet()
{
	if(m_cPage1.m_bOK || m_cPage2.m_bOK)
	{
		//옵션 저장!
		//Page1
		m_cFileOption.Clear();

		m_cFileOption.SetOptionString(L"V2.1",L"_Format"); //신버전 포맷

		m_cFileOption.SetOptionString(m_cPage1.m_strRetChar.GetString(),L"strRetChar");
		m_cFileOption.SetOptionInt(m_cPage1.m_nMaxChar,L"MaxChar");
		m_cFileOption.SetOptionInt(m_cPage1.m_nMaxLine,L"MaxLine");
		m_cFileOption.SetOptionInt(m_cPage1.m_bLimitLine,L"UseLimitLine");
		m_cFileOption.SetOptionInt(m_cPage1.m_bUseRetChar,L"UseRetChar");
		m_cFileOption.SetOptionInt(m_cPage1.m_bUseRet,L"UseRet");
		
		if(m_cPage1.m_nMaxLen>=0)	m_cFileOption.SetOptionInt(m_cPage1.m_bForceLen,L"ForceLen");
		else						m_cFileOption.SetOptionInt(0,L"ForceLen");

		m_cFileOption.SetOptionInt(m_cPage1.m_nMaxLen,L"MaxLen");

		//Page2
		int i=0;
		for(i=0;i<m_cPage2.m_FuncPool.size();i++)
		{
			m_cFileOption.SetOptionInt(m_cPage2.m_FuncPool3[i],L"ElemType",1,i);
			m_cFileOption.SetOptionInt(m_cPage2.m_FuncPool4[i],L"ElemTrim",1,i);
			for(int j=0;j<m_cPage2.m_FuncPool[i].size();j++)
			{
				m_cFileOption.SetOptionInt(m_cPage2.m_FuncPool[i][j].nSize,L"ElemSubSize",2,i,j);
				m_cFileOption.SetOptionInt(m_cPage2.m_FuncPool[i][j].bTrans,L"ElemSubTrans",2,i,j);
				m_cFileOption.SetOptionInt(m_cPage2.m_FuncPool[i][j].bDel,L"ElemSubDel",2,i,j);
				m_cFileOption.SetOptionInt(m_cPage2.m_FuncPool[i][j].bPass,L"ElemSubPass",2,i,j);
				m_cFileOption.SetOptionString(m_cPage2.m_FuncPool[i][j].strText,L"ElemSubString",2,i,j);
				m_cFileOption.SetOptionInt(m_cPage2.m_FuncPool[i][j].nMin,L"ElemSubMin",2,i,j);
				m_cFileOption.SetOptionInt(m_cPage2.m_FuncPool[i][j].nMax,L"ElemSubMax",2,i,j);
				m_cFileOption.SetOptionInt(m_cPage2.m_FuncPool[i][j].bFunc,L"ElemSubFunc",2,i,j);
			}
		}

		//저장
		m_cFileOption.SaveOptionFile();
	}
}

BOOL COptionSheet::OnInitDialog()
{
	m_cFileOption.SetOptionFile(L"ATData\\FixLine.dat");
	m_cFileOption.Clear();
	m_cFileOption.OpenOptionFile();
	
	if(m_cFileOption.GetOptionString(L"_Format").compare(L"")==0)
	{
		//마이그레이션
		CMigration::Migration1to2(L"ATData\\FixLine.dat");
		CMigration::Migration2to2_1(L"ATData\\FixLine.dat");
		m_cFileOption.Clear();
		m_cFileOption.OpenOptionFile();
	}
	else if(m_cFileOption.GetOptionString(L"_Format").compare(L"V2")==0){
		//마이그레이션
		CMigration::Migration2to2_1(L"ATData\\FixLine.dat");
		m_cFileOption.Clear();
		m_cFileOption.OpenOptionFile();
	}

	if(m_cFileOption.IsInData(L"strRetChar"))
	{
		m_cPage1.m_strRetChar = m_cFileOption.GetOptionString(L"strRetChar").c_str();
	}
	else
	{
		m_cPage1.m_strRetChar = "\\n";
	}
	if(m_cFileOption.IsInData(L"MaxChar"))
	{
		m_cPage1.m_nMaxChar = m_cFileOption.GetOptionInt(L"MaxChar");
	}
	else
	{
		m_cPage1.m_nMaxChar = 40;
	}
	if(m_cFileOption.IsInData(L"MaxLine"))
	{
		m_cPage1.m_nMaxLine = m_cFileOption.GetOptionInt(L"MaxLine");
	}
	else
	{
		m_cPage1.m_nMaxLine = 3;
	}
	m_cPage1.m_bLimitLine = m_cFileOption.GetOptionBool(L"UseLimitLine");
	m_cPage1.m_bUseRetChar = m_cFileOption.GetOptionBool(L"UseRetChar");
	m_cPage1.m_bUseRet = m_cFileOption.GetOptionBool(L"UseRet");

	if(m_cFileOption.IsInData(L"MaxLen"))
	{
		m_cPage1.m_nMaxLen = m_cFileOption.GetOptionInt(L"MaxLen");
		m_cPage1.m_bForceLen = m_cFileOption.GetOptionBool(L"ForceLen");
	}
	else
	{
		m_cPage1.m_nMaxLen=-2;
		m_cPage1.m_bForceLen=false;
	}


	int nFuncN=0;
	m_cPage2.m_FuncPool.clear();
	while(m_cFileOption.IsInData(L"ElemType",1,nFuncN))
	{
		m_cPage2.m_FuncPool3.Add(m_cFileOption.GetOptionInt(L"ElemType",1,nFuncN));
		m_cPage2.m_FuncPool4.Add(m_cFileOption.GetOptionBool(L"ElemTrim",1,nFuncN));
		vector <FLStringElement2> Temp;

		int nSubN=0;
		while(m_cFileOption.IsInData(L"ElemSubFunc",2,nFuncN,nSubN))
		{
			FLStringElement2 FLStringTemp;
			FLStringTemp.nSize = m_cFileOption.GetOptionInt(L"ElemSubSize",2,nFuncN,nSubN);
			FLStringTemp.bTrans = m_cFileOption.GetOptionBool(L"ElemSubTrans",2,nFuncN,nSubN);
			FLStringTemp.bDel = m_cFileOption.GetOptionBool(L"ElemSubDel",2,nFuncN,nSubN);
			FLStringTemp.bPass = m_cFileOption.GetOptionBool(L"ElemSubPass",2,nFuncN,nSubN);
			FLStringTemp.strText = m_cFileOption.GetOptionString(L"ElemSubString",2,nFuncN,nSubN).c_str();
			FLStringTemp.nMin = m_cFileOption.GetOptionInt(L"ElemSubMin",2,nFuncN,nSubN);
			FLStringTemp.nMax = m_cFileOption.GetOptionInt(L"ElemSubMax",2,nFuncN,nSubN);
			FLStringTemp.bFunc = m_cFileOption.GetOptionBool(L"ElemSubFunc",2,nFuncN,nSubN);
			Temp.push_back(FLStringTemp);

			nSubN++;
		}

		m_cPage2.m_FuncPool.push_back(Temp);

		wstring strTemp;
		for(int i=0;i<Temp.size();i++)
		{
			strTemp += Temp[i].strText;
		}

		m_cPage2.m_FuncPool2.Add(strTemp.c_str());

		nFuncN++;
	}

	CRect tmpRect;
	GetWindowRect(tmpRect);

	CStatic*	ctrStatic = new CStatic;
	CFont *tFont = this->GetFont();
	
	ctrStatic->Create(L"Hide_D,\r\n지나가다정착한이A",WS_CHILD|WS_VISIBLE|WS_DISABLED|SS_LEFT,CRect(10,tmpRect.Height()+51,120,tmpRect.Height()+81),this);

	CPropertySheet::OnInitDialog();

	
	ctrStatic->SetFont(tFont);

	return FALSE;
}


BEGIN_MESSAGE_MAP(COptionSheet, CPropertySheet)
END_MESSAGE_MAP()