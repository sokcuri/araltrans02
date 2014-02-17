// OptionDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "CustomDic.h"
#include "OptionDlg.h"
#include "afxdialogex.h"
#include "SubFunc.h"
#include "SubOptDlg.h"
#include "FileAddDlg.h"
#include "Util.h"

// COptionDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(COptionDlg, CDialogEx)

COptionDlg::COptionDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(COptionDlg::IDD, pParent)
{
	m_bActive=false;
	m_bChanged=false;
	m_strBaseAralRegPath = L"SOFTWARE\\AralGood\\CustomDic v4\\";
	m_strRegPath = m_strBaseAralRegPath;
	m_strRegPath+= GetGameName();
	m_strRegPath+= L"\\";

	m_nUseAutoLoad=5;
}

COptionDlg::~COptionDlg()
{
}

void COptionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST2, m_cPreDicList);
	DDX_Control(pDX, IDC_LIST1, m_cPostDicList);
	DDX_Control(pDX, IDC_BUTTON1, m_ctrButtonAdd);
	DDX_Control(pDX, IDC_BUTTON2, m_ctrButtonUp1);
	DDX_Control(pDX, IDC_BUTTON4, m_ctrButtonDown1);
	DDX_Control(pDX, IDC_BUTTON3, m_ctrButtonDelete1);
	DDX_Control(pDX, IDC_BUTTON8, m_ctrButtonUp2);
	DDX_Control(pDX, IDC_BUTTON10, m_ctrButtonDown2);
	DDX_Control(pDX, IDC_BUTTON9, m_ctrButtonDelete2);
}


BEGIN_MESSAGE_MAP(COptionDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON5, &COptionDlg::OnBnClickedButton5)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST2, &COptionDlg::OnLvnItemchangedList2)
	ON_NOTIFY(LVN_INSERTITEM, IDC_LIST2, &COptionDlg::OnLvnInsertitemList2)
	ON_NOTIFY(LVN_DELETEITEM, IDC_LIST2, &COptionDlg::OnLvnDeleteitemList2)
	ON_NOTIFY(LVN_INSERTITEM, IDC_LIST1, &COptionDlg::OnLvnInsertitemList1)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, &COptionDlg::OnLvnItemchangedList1)
	ON_NOTIFY(LVN_DELETEITEM, IDC_LIST1, &COptionDlg::OnLvnDeleteitemList1)
	ON_WM_DROPFILES()
	ON_BN_CLICKED(IDC_BUTTON1, &COptionDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON7, &COptionDlg::OnBnClickedButton7)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON3, &COptionDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON2, &COptionDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON4, &COptionDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON8, &COptionDlg::OnBnClickedButton8)
	ON_BN_CLICKED(IDC_BUTTON10, &COptionDlg::OnBnClickedButton10)
	ON_BN_CLICKED(IDC_BUTTON9, &COptionDlg::OnBnClickedButton9)
END_MESSAGE_MAP()


// COptionDlg 메시지 처리기입니다.

bool COptionDlg::LoadOption(){
	m_arFileList.resize(0);
	m_arPreList.resize(0);
	m_arPostList.resize(0);
	
	//옵션 로드

	vector<wchar_t> txtBuff;

	m_nUseAutoLoad=5;
	m_bTrimWord=false;

	CRegKey cReg;
	bool bRegMode=true;
	if(cReg.Open(HKEY_CURRENT_USER,m_strRegPath.c_str(),KEY_READ) != ERROR_SUCCESS){
		bRegMode=false;
	}

	if(bRegMode){
		DWORD dwReadSize=0;

		int nStatus=cReg.QueryMultiStringValue(L"FileList",NULL,&dwReadSize);
		if(nStatus!=ERROR_SUCCESS){
			bRegMode=false;
		}
		else{
			txtBuff.resize(dwReadSize);
			nStatus=cReg.QueryMultiStringValue(L"FileList",txtBuff.data(),&dwReadSize);
			if(nStatus!=ERROR_SUCCESS){
				bRegMode=false;
			}
		}

		DWORD dwTmp=0;
		if(cReg.QueryDWORDValue(L"AutoLoad",dwTmp)==ERROR_SUCCESS){
			m_nUseAutoLoad = (int)dwTmp;
		}
		
		if(cReg.QueryDWORDValue(L"TrimWord",dwTmp)==ERROR_SUCCESS){
			m_bTrimWord = dwTmp!=0;
		}

		cReg.Close();

		for(size_t i=0;i<txtBuff.size();i++){
			if(txtBuff[i]==L'\0'){
				txtBuff[i]=L'\n';
			}
		}
		txtBuff.push_back(L'\0');
	}

	if(!bRegMode){
		m_bChanged=true;

		wchar_t szTmpPath[MAX_PATH]=L"";
		swprintf_s(szTmpPath,MAX_PATH,L"%s\\ATData\\CDic4Default.txt",GetATDirectory());
		FILE * fp = NULL;
		_wfopen_s(&fp,szTmpPath,L"rb");//UTF-16LE이므로 BOM2바이트 건너뜀

		if(fp==NULL)return false;

		fseek(fp,0,SEEK_END);
		int nTxtLen = (ftell(fp)-2)/sizeof(wchar_t);
		fseek(fp,2,SEEK_SET);

		txtBuff.resize(nTxtLen+1);
		fread(txtBuff.data(),sizeof(wchar_t),nTxtLen,fp);

		fclose(fp);

		for(size_t i=0;i<txtBuff.size();i++){
			if(txtBuff[i]==L'\r'){
				txtBuff[i]=L'\n';
			}
		}
	}

	txtBuff[txtBuff.size()-1]=L'\0';

	int nMode=0;

	/* 파일 예제
	[FileList]
	0 932 932 | ~~~
	1 932 949 | c:\~~~~~
	0 949 949 | c:\~~~~
	[PreList]
	1
	0
	[PostList]
	1
	2

	split이 0일 경우 Pre와 Post의 구분은 그 다음 순서에서 위(전)이냐 아래(후)이냐에 따른다.
	*/

	wchar_t * tmp = NULL;
	wchar_t * tmp_end = txtBuff.data();
	while(true){
		//strtok 비슷한 기능
		tmp = tmp_end;
		if(tmp==NULL)break;

		tmp_end = wcsstr(tmp_end,L"\n");
		if(tmp_end!=NULL){
			*tmp_end=L'\0';
			tmp_end++;
		}

		while(true){
			if(*tmp==L' '||*tmp==L'\t'){
				tmp++;
			}
			else{
				break;
			}
		}

		if(trim(tmp).length()==0){
			continue;
		}

		if(tmp[0]==L'['){
			if(wcscmp(tmp,L"[FileList]")==0){
				nMode=0;
				continue;
			}
			else if(wcscmp(tmp,L"[PreList]")==0){
				nMode=1;
				continue;
			}
			else if(wcscmp(tmp,L"[PostList]")==0){
				nMode=2;
				continue;
			}
		}

		if(nMode==0){
			int nSplit;
			int nKeyCP;
			int nValCP;

			swscanf_s(tmp,L"%d %d %d",&nSplit,&nKeyCP,&nValCP);

			wchar_t * pos = wcsstr(tmp+6,L"|");

			fileInfo sFile;
			sFile.split=nSplit!=0;
			sFile.keyCP=nKeyCP;
			sFile.valCP=nValCP;
			sFile.name=trim(pos+1);

			m_allFiles.insert(sFile.name);
			m_arFileList.push_back(sFile);
		}
		else if(nMode==1){
			int n;

			swscanf_s(tmp,L"%d",&n);
			m_arPreList.push_back(n);
		}
		else if(nMode==2){
			int n;
			swscanf_s(tmp,L"%d",&n);
			m_arPostList.push_back(n);
		}
	}

	return bRegMode;
}

void COptionDlg::SaveOption(const bool bDefault){
	wchar_t szBuff[MAX_PATH*2]=L"";

	vector<wchar_t> arOption;
	wstring strTmp;

	strTmp=L"[FileList]";
	arOption.insert(arOption.end(),strTmp.begin(),strTmp.end());
	arOption.push_back('\0');

	for_each(m_arFileList.begin(),m_arFileList.end(),[&arOption,&szBuff](fileInfo& info){
		swprintf_s(szBuff,MAX_PATH*2,L"%d %d %d | %s",info.split,info.keyCP,info.valCP,info.name.c_str());
		arOption.insert(arOption.end(),szBuff,szBuff+wcslen(szBuff)+1);
	});

	strTmp=L"[PreList]";
	arOption.insert(arOption.end(),strTmp.begin(),strTmp.end());
	arOption.push_back('\0');

	for_each(m_arPreList.begin(),m_arPreList.end(),[&arOption,&szBuff](int& i){
		swprintf_s(szBuff,MAX_PATH*2,L"%d",i);
		arOption.insert(arOption.end(),szBuff,szBuff+wcslen(szBuff)+1);
	});
	
	strTmp=L"[PostList]";
	arOption.insert(arOption.end(),strTmp.begin(),strTmp.end());
	arOption.push_back('\0');

	for_each(m_arPostList.begin(),m_arPostList.end(),[&arOption,&szBuff](int& i){
		swprintf_s(szBuff,MAX_PATH*2,L"%d",i);
		arOption.insert(arOption.end(),szBuff,szBuff+wcslen(szBuff)+1);
	});

	if(!bDefault){
		arOption.push_back(L'\0');
		CRegKey cReg;

		if(cReg.Create(HKEY_CURRENT_USER,m_strRegPath.c_str()) != ERROR_SUCCESS){
			AfxMessageBox(L"심각한 오류!\r\n레지스트리 초기화에 실패했습니다.\r\n개발자에게 보고가 필요합니다",MB_ICONSTOP);
			return;
		}

		if(cReg.SetMultiStringValue(L"FileList",arOption.data())!=ERROR_SUCCESS){
			AfxMessageBox(L"심각한 오류!\r\n레지스트리 기록에 실패했습니다.\r\n개발자에게 보고가 필요합니다",MB_ICONSTOP);
		}

		if(cReg.SetDWORDValue(L"AutoLoad",m_nUseAutoLoad)!=ERROR_SUCCESS){
			AfxMessageBox(L"심각한 오류!\r\n레지스트리 기록에 실패했습니다.\r\n개발자에게 보고가 필요합니다",MB_ICONSTOP);
		}

		if(cReg.SetDWORDValue(L"TrimWord",m_bTrimWord)!=ERROR_SUCCESS){
			AfxMessageBox(L"심각한 오류!\r\n레지스트리 기록에 실패했습니다.\r\n개발자에게 보고가 필요합니다",MB_ICONSTOP);
		}

		cReg.Close();
	}
	else{
		for(size_t i=0;i<arOption.size();i++){
			if(arOption[i]==L'\0'){
				arOption[i]=L'\n';
			}
		}
		arOption.push_back(L'\0');

		wchar_t szTmpPath[MAX_PATH]=L"";
		swprintf_s(szTmpPath,MAX_PATH,L"%s\\ATData\\CDic4Default.txt",GetATDirectory());

		FILE* fp = NULL;
		_wfopen_s(&fp,szTmpPath,L"wt,ccs=UTF-16LE");
		if(fp==NULL){
			AfxMessageBox(L"기본 설정을 기록하지 못했습니다.");
			return;
		}
		
		fwprintf(fp,L"%s",arOption.data());
		fclose(fp);
	}
	
}

void COptionDlg::LoadFileList(){
	vector<wstring> arFileComments(m_arFileList.size());

	for(size_t i=0;i<m_arFileList.size();i++){
		wstring & fileName = m_arFileList[i].name;
		map<wstring,wstring> header = parseHeader(fileName,true);

		auto iter = header.find(L"Comment");
		if(iter==header.end()){
			arFileComments[i]=L"설명이 없습니다.";
		}
		else{
			arFileComments[i] = iter->second;
		}
	}

	//전처리 리스트에 추가
	for(size_t i=0;i<m_arPreList.size();i++){
		int nFileIdx = m_arPreList[i];
		wstring strType;
		if(m_arFileList[nFileIdx].split){
			strType = L"원->번";
		}
		else{
			strType = L"원->원";
		}

		m_cPreDicList.InsertItem(i,strType.c_str());
		m_cPreDicList.SetItemText(i,1,arFileComments[nFileIdx].c_str());
		m_cPreDicList.SetItemText(i,2,m_arFileList[nFileIdx].name.c_str());
	}

	//후처리 리스트에 추가
	for(size_t i=0;i<m_arPostList.size();i++){
		int nFileIdx = m_arPostList[i];
		wstring strType;
		if(m_arFileList[nFileIdx].split){
			strType = L"원->번";
		}
		else{
			strType = L"번->번";
		}

		m_cPostDicList.InsertItem(i,strType.c_str());
		m_cPostDicList.SetItemText(i,1,arFileComments[nFileIdx].c_str());
		m_cPostDicList.SetItemText(i,2,m_arFileList[nFileIdx].name.c_str());
	}
}

void COptionDlg::SaveFileList(){

	//사실상 여기가 가장 중요하다.
	m_arFileList.resize(0);
	m_arPreList.resize(0);
	m_arPostList.resize(0);

	map<wstring,int> filePool;

	vector<wstring> preFiles;
	vector<wstring> postFiles;

	int nPreSize= m_cPreDicList.GetItemCount();
	for(int i=0;i<nPreSize;i++){
		wstring strFileName = m_cPreDicList.GetItemText(i,2);
		preFiles.push_back(strFileName);
		filePool[strFileName] = 0;
	}

	int nPostSize = m_cPostDicList.GetItemCount();
	for(int i=0;i<nPostSize;i++){
		wstring strFileName = m_cPostDicList.GetItemText(i,2);

		postFiles.push_back(strFileName);
		auto iter = filePool.find(strFileName);
		if(iter==filePool.end()){
			filePool.insert(make_pair(strFileName,0));
		}
		else{
			iter->second=1;
		}
	}

	int nTotal = 0;
	for(auto iter=filePool.begin();iter!=filePool.end();++iter){
		fileInfo fInfo;
		fInfo.split = iter->second!=0;
		fInfo.name = iter->first;

		m_arFileList.push_back(fInfo);

		iter->second = nTotal++; //자리 주의!
	}

	for(int i=0;i<nPreSize;i++){
		int nIdx = filePool[preFiles[i]];

		m_arPreList.push_back(nIdx);

		fileInfo& fInfo = m_arFileList[nIdx];

		fInfo.keyCP = 932;
		if(!fInfo.split){
			fInfo.valCP = 932;
		}
	}

	for(int i=0;i<nPostSize;i++){
		int nIdx = filePool[postFiles[i]];

		m_arPostList.push_back(nIdx);

		fileInfo& fInfo = m_arFileList[nIdx];

		fInfo.valCP = 949;
		if(!fInfo.split){
			fInfo.keyCP= 949;
		}
	}

}

void COptionDlg::InitListCtrl(){
	//List의 요소
	//종류, 설명, 파일 경로

	m_cPreDicList.ModifyStyle(LVS_TYPEMASK,LVS_REPORT|LVS_NOSORTHEADER|LVS_SINGLESEL|LVS_SHOWSELALWAYS);
	m_cPreDicList.SetExtendedStyle(LVS_EX_DOUBLEBUFFER|LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	m_cPreDicList.InsertColumn(0,L"종류",0,60);
	m_cPreDicList.InsertColumn(1,L"사전 정보",0,150);
	m_cPreDicList.InsertColumn(2,L"파일 경로",0,300);

	m_cPostDicList.ModifyStyle(LVS_TYPEMASK,LVS_REPORT|LVS_NOSORTHEADER|LVS_SINGLESEL|LVS_SHOWSELALWAYS);
	m_cPostDicList.SetExtendedStyle(LVS_EX_DOUBLEBUFFER|LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	m_cPostDicList.InsertColumn(0,L"종류",0,60);
	m_cPostDicList.InsertColumn(1,L"사전 정보",0,150);
	m_cPostDicList.InsertColumn(2,L"파일 경로",0,300);
}


BOOL COptionDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	
	m_ctrButtonAdd.SetIcon((HICON)LoadImage(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDI_ICON_ADD),IMAGE_ICON,16,16,LR_SHARED));

	m_ctrButtonUp1.SetIcon((HICON)LoadImage(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDI_ICON_UP),IMAGE_ICON,16,16,LR_SHARED));
	m_ctrButtonDown1.SetIcon((HICON)LoadImage(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDI_ICON_DOWN),IMAGE_ICON,16,16,LR_SHARED));
	m_ctrButtonDelete1.SetIcon((HICON)LoadImage(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDI_ICON_DELETE),IMAGE_ICON,16,16,LR_SHARED));

	m_ctrButtonUp2.SetIcon((HICON)LoadImage(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDI_ICON_UP),IMAGE_ICON,16,16,LR_SHARED));
	m_ctrButtonDown2.SetIcon((HICON)LoadImage(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDI_ICON_DOWN),IMAGE_ICON,16,16,LR_SHARED));
	m_ctrButtonDelete2.SetIcon((HICON)LoadImage(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDI_ICON_DELETE),IMAGE_ICON,16,16,LR_SHARED));

	InitListCtrl();

	LoadOption();
	LoadFileList();
	//종류, 설명, 파일경로


	UpdateData(0);

	m_bActive=true;

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void COptionDlg::OnBnClickedButton5()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CSubOptDlg cDlg;
	if(m_nUseAutoLoad>0){
		cDlg.m_bAutoLoad=true;
		cDlg.m_nAutoInterval=m_nUseAutoLoad;
	}
	else{
		cDlg.m_bAutoLoad=false;
		cDlg.m_nAutoInterval=5;
	}

	cDlg.m_bTrim = m_bTrimWord;
	
	if(cDlg.DoModal()==IDOK){
		if(cDlg.m_bAutoLoad){
			m_nUseAutoLoad = cDlg.m_nAutoInterval;
		}
		else{
			m_nUseAutoLoad=0;
		}

		m_bTrimWord = cDlg.m_bTrim!=FALSE;

		if(cDlg.m_bSaveDefaultOption){
			//흐갹
			SaveFileList();
			SaveOption(true);
		}
	}
}

bool COptionDlg::isChanged(){
	return m_bChanged;
}

void COptionDlg::OnLvnItemchangedList2(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;


	if(m_bActive)
		m_bChanged=true;
}


void COptionDlg::OnLvnInsertitemList2(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;


	if(m_bActive)
		m_bChanged=true;
}


void COptionDlg::OnLvnDeleteitemList2(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;


	if(m_bActive)
		m_bChanged=true;
}


void COptionDlg::OnLvnInsertitemList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;


	if(m_bActive)
		m_bChanged=true;

}


void COptionDlg::OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	

	if(m_bActive)
		m_bChanged=true;
}


void COptionDlg::OnLvnDeleteitemList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	if(m_bActive)
		m_bChanged=true;
}


void COptionDlg::OnDropFiles(HDROP hDropInfo)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	DWORD dwBufSize;
	DWORD dwNumDrop;
	CString strFileName;

	dwNumDrop = DragQueryFile(hDropInfo,0xFFFFFFFF,NULL,0);

	for(DWORD i=0;i<dwNumDrop;i++){
		dwBufSize = DragQueryFile(hDropInfo,0,NULL,0)+1;
		LPWSTR pszFileName = strFileName.GetBufferSetLength(dwBufSize);
		DragQueryFile(hDropInfo,0,pszFileName,dwBufSize);
		strFileName.ReleaseBuffer();

		CFileAddDlg cDlg;
		cDlg.m_bDragFile=true;
		cDlg.m_strPath = strFileName;
		if(cDlg.DoModal()==IDOK){
			AddFile(cDlg.m_strPath,cDlg.m_nLoadType!=0,cDlg.m_nDicType);
		}
	}

	CDialogEx::OnDropFiles(hDropInfo);
}


void COptionDlg::OnBnClickedButton1()
{
	//사전 추가 버튼

	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CFileAddDlg cDlg;
	if(cDlg.DoModal()==IDOK){
		AddFile(cDlg.m_strPath,cDlg.m_nLoadType!=0,cDlg.m_nDicType);
	}
}


void COptionDlg::OnBnClickedButton7()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if(AfxMessageBox(L"초기화할까요?",MB_YESNO)==IDYES){
		m_cPreDicList.DeleteAllItems();
		m_cPostDicList.DeleteAllItems();

		CRegKey cReg;
		if(cReg.Open(HKEY_CURRENT_USER,m_strBaseAralRegPath.c_str())==ERROR_SUCCESS){
			cReg.RecurseDeleteKey(GetGameName());
		}
		else{
			AfxMessageBox(L"초기화에 실패했습니다!");
			return;
		}
		cReg.Close();

		LoadOption();
		LoadFileList();

		UpdateData(0);

		AfxMessageBox(L"초기화 했습니다");
	}
}

bool COptionDlg::AddFile(CString _strFilePath,bool bOpen,int nType){
	//이미 추가되었는지를 '무식하게' 확인하자
	bool bDuplicate=false;

	int nLen=m_cPreDicList.GetItemCount();
	for(int i=0;i<nLen;i++){
		CString strTmpPath = m_cPreDicList.GetItemText(i,2);
		if(strTmpPath==_strFilePath){
			bDuplicate=true;
			break;
		}
	}

	if(!bDuplicate){
		nLen=m_cPostDicList.GetItemCount();
		for(int i=0;i<nLen;i++){
			CString strTmpPath = m_cPostDicList.GetItemText(i,2);
			if(strTmpPath==_strFilePath){
				bDuplicate=true;
				break;
			}
		}
	}

	if(bDuplicate){
		AfxMessageBox(L"이미 추가된 파일입니다.");
		return false;
	}

	wstring strFilePath = _strFilePath.GetString();
	wstring strComment;

	if(!bOpen){
		//새 파일 생성!
		wstring strDirPath = strFilePath;
		while(strDirPath[strDirPath.length()-1]!=L'\\'){
			strDirPath.pop_back();
		}

		MyCreateDirectory(strDirPath.c_str());

		CFile cFile;
		if(cFile.Open(strFilePath.c_str(),CFile::modeCreate|CFile::modeReadWrite|CFile::shareExclusive)!=FALSE){
			wstring strHeader = L"\uFEFF";
			if(nType==0){//Pre
				strHeader += L"//Header{DicType:\"Pre\",Comment:\"원문 -> 원문(자동 생성)\"}\r\n\r\n";
			}
			else if(nType==2){//Post
				strHeader += L"//Header{DicType:\"Post\",Comment:\"번역문 -> 번역문(자동 생성)\"}\r\n\r\n";
			}
			else{//Body
				strHeader += L"//Header{DicType:\"Body\",Comment:\"원문 -> 번역문(자동 생성)\"}\r\n\r\n";
			}
			wchar_t cszPreFile[] = 
				L"//////////////////////////////////\r\n"
				L"//사용자 사전 | CustomDic v4     //\r\n"
				L"//////////////////////////////////\r\n"
				L"//\r\n"
				L"//사용자 사전 플러그인용 단어 목록 파일입니다.\r\n"
				L"//일본어	한국어	우선순위(없을경우 10)\r\n"
				L"//의 형식으로 단어를 추가 할 수 있습니다.\r\n"
				L"//\r\n"
				L"//자세한 사용법은 http://wiki.aralgood.com/index.php/CustomDic 을 참고해주세요.\r\n"
				L"//\r\n";

			cFile.Write(strHeader.c_str(),sizeof(wchar_t)*strHeader.length());
			cFile.Write(cszPreFile,sizeof(cszPreFile)-sizeof(wchar_t));
			cFile.Close();
		}
	}

	//나머지 단계는 비 효율적이라도 그냥 bOpen 일경우로 처리하자
	map<wstring,wstring> fHeader = parseHeader(strFilePath,true);

	auto iter = fHeader.find(L"Status");
	if(iter==fHeader.end()||iter->second!=L"OK"){
		return false;
	}

	iter = fHeader.find(L"Comment");
	if(iter!=fHeader.end()){
		strComment = iter->second;
	}

	int idx=m_cPreDicList.GetItemCount();
	if(nType==0){//Pre
		m_cPreDicList.InsertItem(0,L"원->원");
		m_cPreDicList.SetItemText(0,1,strComment.c_str());
		m_cPreDicList.SetItemText(0,2,strFilePath.c_str());
	}
	else if(nType==2){
		m_cPostDicList.InsertItem(idx,L"번->번");
		m_cPostDicList.SetItemText(idx,1,strComment.c_str());
		m_cPostDicList.SetItemText(idx,2,strFilePath.c_str());
	}
	else{
		m_cPreDicList.InsertItem(0,L"원->번");
		m_cPreDicList.SetItemText(0,1,strComment.c_str());
		m_cPreDicList.SetItemText(0,2,strFilePath.c_str());

		m_cPostDicList.InsertItem(idx,L"원->번");
		m_cPostDicList.SetItemText(idx,1,strComment.c_str());
		m_cPostDicList.SetItemText(idx,2,strFilePath.c_str());
	}
	
	return true;
}

void COptionDlg::DeleteFile(CString strFilePath){
	//대충대충 하나하나 확인하고 지우자

	int nLen=m_cPreDicList.GetItemCount();
	for(int i=0;i<nLen;i++){
		CString strTmpPath = m_cPreDicList.GetItemText(i,2);
		if(strTmpPath==strFilePath){
			m_cPreDicList.DeleteItem(i);
			break;
		}
	}

	nLen=m_cPostDicList.GetItemCount();
	for(int i=0;i<nLen;i++){
		CString strTmpPath = m_cPostDicList.GetItemText(i,2);
		if(strTmpPath==strFilePath){
			m_cPostDicList.DeleteItem(i);
			break;
		}
	}
}

void COptionDlg::OnDestroy()
{
	m_bActive=false;

	CDialogEx::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	//비효율적이지만 UI 란 그런거니 그렇다 치자[...]
	SaveFileList();
	SaveOption();
}

void COptionDlg::OnBnClickedButton2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//1번 위로!
	POSITION tmp =m_cPreDicList.GetFirstSelectedItemPosition();
	if(tmp==NULL)return;

	int nIdx = m_cPreDicList.GetNextSelectedItem(tmp);
	if(nIdx==-1){
		return;
	}

	if(nIdx==0){
		return;
	}

	CString strBuff;

	for(int i=0;i<3;i++){
		strBuff = m_cPreDicList.GetItemText(nIdx-1,i);
		m_cPreDicList.SetItemText(nIdx-1,i,m_cPreDicList.GetItemText(nIdx,i));
		m_cPreDicList.SetItemText(nIdx,i,strBuff);
	}
}


void COptionDlg::OnBnClickedButton4()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//1번 아래로!
	TRACE(L"DOWN\r\n");

	POSITION tmp =m_cPreDicList.GetFirstSelectedItemPosition();
	if(tmp==NULL)return;

	int nIdx = m_cPreDicList.GetNextSelectedItem(tmp);
	if(nIdx==-1){
		return;
	}

	if(nIdx==m_cPreDicList.GetItemCount()-1){
		return;
	}

	CString strBuff;

	for(int i=0;i<3;i++){
		strBuff = m_cPreDicList.GetItemText(nIdx+1,i);
		m_cPreDicList.SetItemText(nIdx+1,i,m_cPreDicList.GetItemText(nIdx,i));
		m_cPreDicList.SetItemText(nIdx,i,strBuff);
	}
}

void COptionDlg::OnBnClickedButton3()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//1번 삭제!

	POSITION tmp =m_cPreDicList.GetFirstSelectedItemPosition();
	if(tmp==NULL)return;

	int nIdx = m_cPreDicList.GetNextSelectedItem(tmp);

	CString strFilePath;

	if(nIdx!=-1){
		strFilePath = m_cPreDicList.GetItemText(nIdx,2);
	}
	DeleteFile(strFilePath);
}


void COptionDlg::OnBnClickedButton8()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//2번 위!
	POSITION tmp =m_cPostDicList.GetFirstSelectedItemPosition();
	if(tmp==NULL)return;

	int nIdx = m_cPostDicList.GetNextSelectedItem(tmp);

	if(nIdx==-1){
		return;
	}

	if(nIdx==0){
		return;
	}

	CString strBuff;

	for(int i=0;i<3;i++){
		strBuff = m_cPostDicList.GetItemText(nIdx-1,i);
		m_cPostDicList.SetItemText(nIdx-1,i,m_cPostDicList.GetItemText(nIdx,i));
		m_cPostDicList.SetItemText(nIdx,i,strBuff);
	}
}


void COptionDlg::OnBnClickedButton10()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//2번 아래!
	POSITION tmp =m_cPostDicList.GetFirstSelectedItemPosition();
	if(tmp==NULL)return;

	int nIdx = m_cPostDicList.GetNextSelectedItem(tmp);

	if(nIdx==-1){
		return;
	}

	if(nIdx==m_cPostDicList.GetItemCount()-1){
		return;
	}

	CString strBuff;

	for(int i=0;i<3;i++){
		strBuff = m_cPostDicList.GetItemText(nIdx+1,i);
		m_cPostDicList.SetItemText(nIdx+1,i,m_cPostDicList.GetItemText(nIdx,i));
		m_cPostDicList.SetItemText(nIdx,i,strBuff);
	}
}


void COptionDlg::OnBnClickedButton9()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//2번 삭제!
	POSITION tmp =m_cPostDicList.GetFirstSelectedItemPosition();
	if(tmp==NULL)return;

	int nIdx = m_cPostDicList.GetNextSelectedItem(tmp);

	CString strFilePath;

	if(nIdx!=-1){
		strFilePath = m_cPostDicList.GetItemText(nIdx,2);
	}
	DeleteFile(strFilePath);
}

void COptionDlg::Migration(const bool bCDic,const bool bGDic,const bool bTrim){
	wchar_t szBuff[MAX_PATH];

	if(bGDic){
		swprintf_s(szBuff,MAX_PATH,L"%s\\CustomDic.txt",GetATDirectory());

		bool bInsert=true;
		for(int i=0;i<m_arFileList.size();i++){
			if(m_arFileList[i].name==szBuff){
				bInsert=false;
				break;
			}
		}

		if(bInsert){
			fileInfo fInfo;
			fInfo.split=true;
			fInfo.keyCP=932;
			fInfo.valCP=949;
			fInfo.name=szBuff;

			m_arFileList.push_back(fInfo);

			int nIdx = m_arFileList.size()-1;

			m_arPreList.insert(m_arPreList.begin(),1,nIdx);
			m_arPostList.insert(m_arPostList.end(),1,nIdx);
		}
	}

	if(bCDic){
		swprintf_s(szBuff,MAX_PATH,L"%s\\ATData\\CustomDic.txt",GetGameDirectory());

		bool bInsert=true;
		for(int i=0;i<m_arFileList.size();i++){
			if(m_arFileList[i].name==szBuff){
				bInsert=false;
				break;
			}
		}

		if(bInsert){
			fileInfo fInfo;
			fInfo.split=true;
			fInfo.keyCP=932;
			fInfo.valCP=949;
			fInfo.name=szBuff;

			m_arFileList.push_back(fInfo);

			int nIdx = m_arFileList.size()-1;

			m_arPreList.insert(m_arPreList.begin(),1,nIdx);
			m_arPostList.insert(m_arPostList.end(),1,nIdx);
		}
	}

	if(bTrim){
		m_bTrimWord=true;
	}

	SaveOption();
}