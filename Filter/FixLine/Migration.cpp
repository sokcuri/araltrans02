#include "StdAfx.h"
#include "Migration.h"

CMigration::CMigration(void)
{
}

CMigration::~CMigration(void)
{
}

void CMigration::Migration1to2(wstring strFileName)
{
	CFileOptionParser m_cOption1;
	m_cOption1.SetOptionFile(strFileName);
	m_cOption1.OpenOptionFile();
	CFileOptionParser m_cOption2;
	m_cOption2.Clear();
	m_cOption2.SetOptionFile(strFileName);

	//구버전 코드
	m_cOption2.SetOptionString(L"V2",L"_Format");
	if(m_cOption1.IsInData(L"strRetChar"))
	{
		m_cOption2.SetOptionString(m_cOption1.GetOptionString(L"strRetChar"),L"strRetChar");
	}
	m_cOption2.SetOptionInt(m_cOption1.GetOptionInt(L"IgnoreLine"),L"IgnoreLine");
	if(m_cOption1.IsInData(L"MaxChar"))
	{
		m_cOption2.SetOptionInt(m_cOption1.GetOptionInt(L"MaxChar"),L"MaxChar");
	}
	if(m_cOption1.IsInData(L"MaxLine"))
	{
		m_cOption2.SetOptionInt(m_cOption1.GetOptionInt(L"MaxLine"),L"MaxLine");
	}
	m_cOption2.SetOptionInt(m_cOption1.GetOptionBool(L"UseLimitLine"),L"UseLimitLine");
	m_cOption2.SetOptionInt(m_cOption1.GetOptionBool(L"IgnoreNonTrans"),L"IgnoreNonTrans");
	m_cOption2.SetOptionInt(m_cOption1.GetOptionBool(L"UseRetChar"),L"UseRetChar");
	m_cOption2.SetOptionInt(m_cOption1.GetOptionBool(L"UseRet"),L"UseRet");

	int nFuncN=0;
	while(m_cOption1.IsInData(L"ElemType",1,nFuncN))
	{
		if(m_cOption1.GetOptionBool(L"HeadType",1,nFuncN))
					m_cOption2.SetOptionInt(1,L"ElemType",1,nFuncN);
		else		m_cOption2.SetOptionInt(0,L"ElemType",1,nFuncN);
		
		int nSubN=0;
		while(m_cOption1.IsInData(L"ElemSubType",2,nFuncN,nSubN))
		{
			m_cOption2.SetOptionString(m_cOption1.GetOptionString(L"ElemSubString",2,nFuncN,nSubN),L"ElemSubString",2,nFuncN,nSubN);
			int nSubType=m_cOption1.GetOptionInt(L"ElemSubType",2,nFuncN,nSubN);

			if(nSubType>=4)	m_cOption2.SetOptionInt(1,L"ElemSubFunc",2,nFuncN,nSubN);
			else					m_cOption2.SetOptionInt(0,L"ElemSubFunc",2,nFuncN,nSubN);

			if(nSubType%2==1)		m_cOption2.SetOptionInt(-2,L"ElemSubSize",2,nFuncN,nSubN);
			else if(nSubType>=4)	m_cOption2.SetOptionInt(m_cOption1.GetOptionInt(L"ElemSubLen",2,nFuncN,nSubN),L"ElemSubSize",2,nFuncN,nSubN);
			else					m_cOption2.SetOptionInt(-1,L"ElemSubSize",2,nFuncN,nSubN);
			
			if(nSubType<=1)			m_cOption2.SetOptionInt(1,L"ElemSubTrans",2,nFuncN,nSubN);
			else					m_cOption2.SetOptionInt(0,L"ElemSubTrans",2,nFuncN,nSubN);

			m_cOption2.SetOptionInt(m_cOption1.GetOptionBool(L"ElemSubDelete",2,nFuncN,nSubN),L"ElemSubDel",2,nFuncN,nSubN);

			m_cOption2.SetOptionInt(-1,L"ElemSubMin",2,nFuncN,nSubN);
			m_cOption2.SetOptionInt(-1,L"ElemSubMax",2,nFuncN,nSubN);

			nSubN++;
		}
		nFuncN++;
	}

	int nNameN=0;
	while(m_cOption1.IsInData(L"Name",1,nNameN))
	{
		m_cOption2.SetOptionString(m_cOption1.GetOptionString(L"Name",1,nNameN),L"Name",1,nNameN);
		nNameN++;
	}

	m_cOption2.SetOptionInt(-2,L"MaxLen");
	m_cOption2.SetOptionInt(0,L"ForceLen");

	m_cOption2.SaveOptionFile();
}

void CMigration::Migration2to2_1(wstring strFileName)
{
	CFileOptionParser m_cOption;
	m_cOption.SetOptionFile(strFileName);
	m_cOption.OpenOptionFile();

	//주요변경점
	//IgnoreLine계열의 '전체 삭제'
	//변경된 작동 방식으로 '패턴 대응'으로 처리 가능

	m_cOption.SetOptionString(L"V2.1",L"_Format");

	int nIgnoreLine = m_cOption.GetOptionInt(L"IgnoreLine");
	wstring strRet = m_cOption.GetOptionString(L"strRetChar");
	int nFuncN=0;
	while(m_cOption.IsInData(L"ElemType",1,nFuncN)){
		nFuncN++;
	}

	vector<wstring> nameList;

	m_cOption.removeData(L"IgnoreLine");
	if(true){
		int i=0;
		while(m_cOption.IsInData(L"Name",1,i)){
			nameList.push_back(m_cOption.GetOptionString(L"Name",1,i));
			m_cOption.removeData(L"Name",1,i);
			i++;
		}
	}

	if(nIgnoreLine>0){
		m_cOption.SetOptionInt(1,L"ElemType",1,nFuncN);
		m_cOption.SetOptionInt(true,L"ElemTrim",1,nFuncN);

		for(int i=0;i<nIgnoreLine;i++){
			m_cOption.SetOptionString(L"{T}",L"ElemSubString",2,nFuncN,i*2);
			m_cOption.SetOptionInt(0,L"ElemSubFunc",2,nFuncN,i*2);
			m_cOption.SetOptionInt(false,L"ElemSubTrans",2,nFuncN,i*2);
			m_cOption.SetOptionInt(-2,L"ElemSubSize",2,nFuncN,i*2);
			m_cOption.SetOptionInt(false,L"ElemSubDel",2,nFuncN,i*2);
			m_cOption.SetOptionInt(false,L"ElemSubPass",2,nFuncN,i*2);
			m_cOption.SetOptionInt(-1,L"ElemSubMin",2,nFuncN,i*2);
			m_cOption.SetOptionInt(-1,L"ElemSubMax",2,nFuncN,i*2);

			m_cOption.SetOptionString(strRet,L"ElemSubString",2,nFuncN,i*2+1);
			m_cOption.SetOptionInt(1,L"ElemSubFunc",2,nFuncN,i*2+1);
			m_cOption.SetOptionInt(false,L"ElemSubTrans",2,nFuncN,i*2+1);
			m_cOption.SetOptionInt(-2,L"ElemSubSize",2,nFuncN,i*2+1);
			m_cOption.SetOptionInt(false,L"ElemSubDel",2,nFuncN,i*2+1);
			m_cOption.SetOptionInt(false,L"ElemSubPass",2,nFuncN,i*2+1);
			m_cOption.SetOptionInt(-1,L"ElemSubMin",2,nFuncN,i*2+1);
			m_cOption.SetOptionInt(-1,L"ElemSubMax",2,nFuncN,i*2+1);
		}		
	}
	else if(nIgnoreLine<0){
		for(int i=0;i<nameList.size();i++){
			m_cOption.SetOptionInt(1,L"ElemType",1,nFuncN);
			m_cOption.SetOptionInt(true,L"ElemTrim",1,nFuncN);

			m_cOption.SetOptionString(nameList[i],L"ElemSubString",2,nFuncN,0);
			m_cOption.SetOptionInt(0,L"ElemSubFunc",2,nFuncN,0);
			m_cOption.SetOptionInt(false,L"ElemSubTrans",2,nFuncN,0);
			m_cOption.SetOptionInt(-2,L"ElemSubSize",2,nFuncN,0);
			m_cOption.SetOptionInt(false,L"ElemSubDel",2,nFuncN,0);
			m_cOption.SetOptionInt(false,L"ElemSubPass",2,nFuncN,0);
			m_cOption.SetOptionInt(-1,L"ElemSubMin",2,nFuncN,0);
			m_cOption.SetOptionInt(-1,L"ElemSubMax",2,nFuncN,0);

			m_cOption.SetOptionString(strRet,L"ElemSubString",2,nFuncN,1);
			m_cOption.SetOptionInt(1,L"ElemSubFunc",2,nFuncN,1);
			m_cOption.SetOptionInt(false,L"ElemSubTrans",2,nFuncN,1);
			m_cOption.SetOptionInt(-2,L"ElemSubSize",2,nFuncN,1);
			m_cOption.SetOptionInt(false,L"ElemSubDel",2,nFuncN,1);
			m_cOption.SetOptionInt(false,L"ElemSubPass",2,nFuncN,1);
			m_cOption.SetOptionInt(-1,L"ElemSubMin",2,nFuncN,1);
			m_cOption.SetOptionInt(-1,L"ElemSubMax",2,nFuncN,1);

			nFuncN++;
		}
	}

	m_cOption.SaveOptionFile();
}