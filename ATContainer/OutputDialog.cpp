 // OutputDialog.cpp : implementation file
//
#pragma warning(disable:4996)

#include "stdafx.h"
#include "resource.h"
#include "AralGoodHook.h"
#include "OutputDialog.h"
#include "ShortcutDlg.h"
#include "AppLocMgr/AppLocMgr.h"
#include "RegistryMgr/cRegistryMgr.h"
#include "AddFilterDlg.h"
#include "FilterDirectInputDlg.h"
#include "Util.h"
#include <Rpcdce.h>		// GUID 생성 관련

#pragma comment(lib, "Rpcrt4.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define  WM_TRAYICON_MSG (WM_USER+1)

#define MAINAPP CAralGoodHookApp::_gInst



/////////////////////////////////////////////////////////////////////////////
// COutputDialog dialog


COutputDialog::COutputDialog(CWnd* pParent)
	: CDialog(COutputDialog::IDD, pParent), m_pParentWnd(pParent)
	//: BTWindow<CDialog>(COutputDialog::IDD, pParent), m_pParentWnd(pParent)
{
	
	//{{AFX_DATA_INIT(COutputDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void COutputDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COutputDialog)
	DDX_Control(pDX, IDC_COMBO_HOOKMODE, m_comboHookMode);
	DDX_Control(pDX, IDC_COMBO_TRANSLATOR, m_comboTranslator);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_CHECK1, m_chkTrayMin);
}


BEGIN_MESSAGE_MAP(COutputDialog, CDialog)
	//{{AFX_MSG_MAP(COutputDialog)
	//}}AFX_MSG_MAP
	ON_CBN_SELCHANGE(IDC_COMBO_HOOKMODE, &COutputDialog::OnCbnSelchangeComboHookmode)
	ON_CBN_SELCHANGE(IDC_COMBO_TRANSLATOR, &COutputDialog::OnCbnSelchangeComboTranslator)
	ON_BN_CLICKED(IDC_BTN_HOOK_SETTING, &COutputDialog::OnBnClickedBtnHookSetting)
	ON_BN_CLICKED(IDC_BTN_HOOK_ONOFF, &COutputDialog::OnBnClickedBtnHookOnoff)
	ON_BN_CLICKED(IDC_BTN_TRANS_SETTING, &COutputDialog::OnBnClickedBtnTransSetting)
	//ON_BN_CLICKED(IDC_BTN_CREATE_SHORTCUT, &COptionDlg::OnBnClickedBtnCreateShortcut)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_WM_NCDESTROY()
	ON_BN_CLICKED(IDC_BTN_CREATE_SHORTCUT, &COutputDialog::OnBnClickedBtnCreateShortcut)
	ON_BN_CLICKED(IDC_CHECK1, &COutputDialog::OnBnClickedCheck1)
	ON_MESSAGE(WM_TRAYICON_MSG, TrayIconMsg)
	ON_WM_SYSCOMMAND()
	ON_BN_CLICKED(IDC_BTN_FILTER_SETTING, &COutputDialog::OnBnClickedBtnFilterSetting)
	ON_BN_CLICKED(IDC_BTN_FILTER_ADD, &COutputDialog::OnBnClickedBtnFilterAdd)
	ON_BN_CLICKED(IDC_BTN_FILTER_DELETE, &COutputDialog::OnBnClickedBtnFilterDelete)
	ON_BN_CLICKED(IDC_BTN_FILTER_UP, &COutputDialog::OnBnClickedBtnFilterUp)
	ON_BN_CLICKED(IDC_BTN_FILTER_DOWN, &COutputDialog::OnBnClickedBtnFilterDown)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_FILTER, &COutputDialog::OnNMDblclkListFilter)
	ON_BN_CLICKED(IDC_BTN_FILTER_DIRECT_SETTING, &COutputDialog::OnBnClickedBtnFilterDirectSetting)
	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST_FILTER, &COutputDialog::OnLvnKeydownListFilter)
//	ON_NOTIFY(NM_KILLFOCUS, IDC_LIST_FILTER, &COutputDialog::OnNMKillfocusListFilter)
//	ON_NOTIFY(NM_SETFOCUS, IDC_LIST_FILTER, &COutputDialog::OnNMSetfocusListFilter)
	ON_NOTIFY(NM_CLICK, IDC_LIST_FILTER, &COutputDialog::OnNMClickListFilter)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COutputDialog message handlers

BOOL COutputDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
// 	CString strWndTitle;
// 	m_pParentWnd->GetWindowText(strWndTitle);
// 	strWndTitle += _T("의 출력창");
// 	m_editOutput.SetWindowText(strWndTitle);

	

	// 후킹 플러그인 폴더 검색
	CString folderAlgorithm = (MAINAPP)->m_strAralTransDir + _T("\\Algorithm\\*.dll");
	CFileFind findAlgorithm;
	BOOL bFindAlgorithm = findAlgorithm.FindFile(folderAlgorithm);
	if(bFindAlgorithm)
	{
		do
		{
			bFindAlgorithm = findAlgorithm.FindNextFile();

			CString strPluginName = findAlgorithm.GetFileName();

			if( strPluginName.Right(4).CompareNoCase(_T(".dll")) == 0 )
			{
				strPluginName = strPluginName.Left( strPluginName.GetLength() - 4 );
			}

			m_comboHookMode.AddString(strPluginName);

		} while( bFindAlgorithm );

	}

	m_comboHookMode.SetCurSel(0);

	// 필터 플러그인
	InitFilterGrid();

	// 번역 플러그인 폴더 검색
	CString folderTranslator = (MAINAPP)->m_strAralTransDir + _T("\\Translator\\*.dll");
	CFileFind findTranslator;
	BOOL bFindTranslator = findTranslator.FindFile(folderTranslator);

	if(bFindTranslator)
	{
		do
		{
			bFindTranslator = findTranslator.FindNextFile();

			CString strPluginName = findTranslator.GetFileName();

			if( strPluginName.Right(4).CompareNoCase(_T(".dll")) == 0 )
			{
				strPluginName = strPluginName.Left( strPluginName.GetLength() - 4 );
			}

			m_comboTranslator.AddString(strPluginName);

		} while( bFindTranslator );
	}

	m_comboTranslator.SetCurSel(0);

	// 트레이 설정
	int nTrayMin = (int)CRegistryMgr::RegReadDWORD(_T("HKEY_CURRENT_USER\\Software\\AralGood"), _T("TrayMinimize"));
	m_chkTrayMin.SetCheck(nTrayMin);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void COutputDialog::PostNcDestroy() 
{
	TRACE(_T("COutputDialog::PostNcDestroy() \n"));

	delete this;
	CDialog::PostNcDestroy();
}


//////////////////////////////////////////////////////////////////////////
// 알고리즘 변경시
//////////////////////////////////////////////////////////////////////////
void COutputDialog::OnCbnSelchangeComboHookmode()
{
	(MAINAPP)->OnAlgorithmChanged();
}

//////////////////////////////////////////////////////////////////////////
// 알고리즘 옵션 버튼 클릭시
//////////////////////////////////////////////////////////////////////////
void COutputDialog::OnBnClickedBtnHookSetting()
{
	(MAINAPP)->OnAlgorithmOption();
}

//////////////////////////////////////////////////////////////////////////
// 알고리즘 On/Off 버튼 클릭시
//////////////////////////////////////////////////////////////////////////
void COutputDialog::OnBnClickedBtnHookOnoff()
{
	(MAINAPP)->OnAlgorithmOnOff();
}



//////////////////////////////////////////////////////////////////////////
// 번역기 변경시
//////////////////////////////////////////////////////////////////////////
void COutputDialog::OnCbnSelchangeComboTranslator()
{
	(MAINAPP)->OnTranslatorChanged();
}

//////////////////////////////////////////////////////////////////////////
// 번역기 옵션 버튼 클릭시
//////////////////////////////////////////////////////////////////////////
void COutputDialog::OnBnClickedBtnTransSetting()
{
	(MAINAPP)->OnTranslatorOption();
}

void COutputDialog::OnClose()
{
	int nMBRes = this->MessageBox(_T("아랄트랜스를 종료하시겠습니까?"), _T("Aral Trans"), MB_YESNO);
	
	if(nMBRes == IDYES)
	{
		CDialog::OnClose();
		(MAINAPP)->ATCleanUp();
	}
}

void COutputDialog::OnDestroy()
{
	DeleteTrayIcon();
	CDialog::OnDestroy();
}

void COutputDialog::OnNcDestroy()
{
	CDialog::OnNcDestroy();

	// TODO: Add your message handler code here
	(MAINAPP)->ATCleanUp();
	//FreeLibraryAndExitThread( GetModuleHandle("ATCTNR.DLL"), 7 );
	//HMODULE hModule = GetModuleHandle("ATCTNR.DLL");
	//DisableThreadLibraryCalls(hModule);
	//FreeLibrary(hModule);
	//DWORD dwTID = 0;
	//CreateThread(NULL, 0, 
	//	(LPTHREAD_START_ROUTINE) GetProcAddress( GetModuleHandle("kernel32.dll"), "FreeLibrary" ), 
	//	GetModuleHandle("ATCTNR.DLL"), 0, &dwTID );
}

//////////////////////////////////////////////////////////////////////////
// 바로가기 생성시
//////////////////////////////////////////////////////////////////////////
void COutputDialog::OnBnClickedBtnCreateShortcut()
{
	// 게임 실행파일 경로 얻기
	TCHAR szExeFileName[MAX_PATH] = {0,};
	if( ::GetModuleFileName( NULL, szExeFileName, MAX_PATH ) == FALSE ) return;

	CString strTmpName = szExeFileName;
	int nIdx = strTmpName.ReverseFind(_T('\\'));
	if(nIdx>=0) strTmpName = strTmpName.Mid(nIdx+1);
	nIdx = strTmpName.ReverseFind(_T('.'));
	if(nIdx>=0) strTmpName = strTmpName.Left(nIdx);

	// 실행 위치 결정	
	//CString strWorkDir = szExeFileName;
	//nIdx = strWorkDir.ReverseFind(_T('\\'));
	//if(nIdx > 0) strWorkDir = strWorkDir.Left(nIdx);
	//else strWorkDir = _T("");
	// 2008.10.28
	CString strWorkDir;	
	::GetCurrentDirectory(MAX_PATH, strWorkDir.GetBufferSetLength(MAX_PATH));
	strWorkDir.ReleaseBuffer();

	CShortcutDlg sc_dlg;
	sc_dlg.m_strShortcutFilePath = strWorkDir + _T("\\") + strTmpName + _T(".lnk");
	sc_dlg.m_strDelayTime = _T("0");
	sc_dlg.m_nShortcutType = 0;
	
	if( sc_dlg.DoModal() == IDOK )
	{
		// 아랄 트랜스 경로 얻기
		CString strAralTrans = _T("\"") + (MAINAPP)->m_strAralTransDir + _T("\\AralTrans.exe") + _T("\"");

		// 옵션 스트링 병합
		CString strAlgorithmOpt;
		this->GetDlgItem(IDC_EDIT_HOOK_OPTION_STRING)->GetWindowText(strAlgorithmOpt);
		CString strFilterOpt;
		this->GetDlgItem(IDC_EDIT_FILTER_OPTION_STRING)->GetWindowText(strFilterOpt);
		CString strTranslatorOpt;
		this->GetDlgItem(IDC_EDIT_TRANS_OPTION_STRING)->GetWindowText(strTranslatorOpt);

		// 스트링 concat (1차)
		CString strArg = strAlgorithmOpt + _T(" ") + strFilterOpt + _T(" ") + strTranslatorOpt;
		
		// 게임을 직접 실행한다면 (Execution Type)
		if(sc_dlg.m_nShortcutType == 0)
		{
			strArg += _T(" ");
			strArg += szExeFileName;
		}
		// 기존 게임을 후킹만 한다면 (Attachment Type)
		else
		{
			strArg += _T(" /P{") + strTmpName + _T(".exe}");

			// 로더 확인
			if (sc_dlg.m_strLoaderPath.Trim().IsEmpty() == FALSE)
			{
				strArg += _T(" ") + sc_dlg.m_strLoaderPath;
			}
		}

		// 스레드 시작주소 입력
		CString strADR = CRegistryMgr::RegRead(_T("HKEY_CURRENT_USER\\Software\\AralGood"), _T("SetTidAdr"));
		if( strADR.GetLength() > 0 )
			strArg += _T(" /M{") + strADR + _T("}");

		// 바이너리 선행패치 유무
		int nRestart = CRegistryMgr::RegReadDWORD(_T("HKEY_CURRENT_USER\\Software\\AralGood"), _T("BinaryRestart"));
		if( nRestart > 0 )
			strArg += _T(" /R");

		// 바이너리 패치 여부
		int nBiPatch = CRegistryMgr::RegReadDWORD(_T("HKEY_CURRENT_USER\\Software\\AralGood"), _T("BinaryPatch"));
		if( nBiPatch )
		{
			strArg += _T(" /b:{");
			int nCount = 0;
			CString regCount;
			unsigned char* cBuff = (unsigned char*)malloc(0x10000);

			while( nBiPatch > nCount )
			{
				nCount ++;
				DWORD dwPatchAdr = 0;
				regCount.Format(_T("BinaryName%d"),nCount);
				regCount = CRegistryMgr::RegRead(_T("HKEY_CURRENT_USER\\Software\\AralGood"), regCount);
				if( regCount != _T("") )
					strArg += regCount + _T("!");
				regCount.Format(_T("BinaryAdr%d"),nCount);
				regCount.Format(_T("%08x("),CRegistryMgr::RegReadDWORD(_T("HKEY_CURRENT_USER\\Software\\AralGood"), regCount));
				strArg += regCount;
				regCount.Format(_T("Binary%d"),nCount);
				int len = CRegistryMgr::RegReadBINARY(_T("HKEY_CURRENT_USER\\Software\\AralGood"), regCount, cBuff);
				int pos = 0;
				while( len > pos )
				{
					regCount.Format(_T("%02x"),cBuff[pos]);
					strArg += regCount;
					pos ++;
				}
				strArg += _T(")");
				
				if( nBiPatch > nCount )
					strArg += _T(",");
			}
			free(cBuff);
			strArg += _T("}");
		}

		if(sc_dlg.m_nHideType == 1)
			strArg += _T(" /H");

		// 후킹 딜레이
		if(sc_dlg.m_strDelayTime.Trim().IsEmpty() == FALSE)
		{
			int nDelay = _ttoi(sc_dlg.m_strDelayTime);
			if(nDelay > 0)
			{
				CString strDelayOption;
				strDelayOption.Format(_T(" /D%d"), nDelay);
				strArg += strDelayOption;
			}
		}
		
		HMODULE hKonJ = GetModuleHandle(_T("KonJ.dll"));
		HMODULE hJonK = GetModuleHandle(_T("JonK.dll"));
		HMODULE hNtlea = GetModuleHandle(_T("ntleah.dll"));

		// 어플로케일 사용 여부
		if(CAppLocMgr::IsLoaded())
		{
			strArg += _T(" /L");
		}
		// KonJ 사용여부
		else if(hKonJ)
		{			
			strArg += _T(" /KonJ");
			
			// KonJ 디렉토리를 레지스트리에 쓰기
			TCHAR szDllFilePath[MAX_PATH] = {0,};
			if( ::GetModuleFileName( hKonJ, szDllFilePath, MAX_PATH ) )
			{
				LPTSTR ptrDirEnd = _tcsrchr(szDllFilePath, _T('\\'));
				if(ptrDirEnd)
				{
					*ptrDirEnd = _T('\0');
					CRegistryMgr::RegWrite(_T("HKEY_CURRENT_USER\\Software\\AralGood"), _T("KonJJonKDir"), szDllFilePath);
				}
			}
		}
		// KonJ 사용여부
		else if(hJonK)
		{			
			strArg += _T(" /JonK");

			// KonJ 디렉토리를 레지스트리에 쓰기
			TCHAR szDllFilePath[MAX_PATH] = {0,};
			if( ::GetModuleFileName( hJonK, szDllFilePath, MAX_PATH ) )
			{
				LPTSTR ptrDirEnd = _tcsrchr(szDllFilePath, _T('\\'));
				if(ptrDirEnd)
				{
					*ptrDirEnd = _T('\0');
					CRegistryMgr::RegWrite(_T("HKEY_CURRENT_USER\\Software\\AralGood"), _T("KonJJonKDir"), szDllFilePath);
				}
			}
		}
		// NTLEA 사용여부
		else if(hNtlea)
		{			
			strArg += _T(" /N");

			// NTLEA 디렉토리를 레지스트리에 쓰기
			TCHAR szDllFilePath[MAX_PATH] = {0,};
			if( ::GetModuleFileName( hNtlea, szDllFilePath, MAX_PATH ) )
			{
				LPTSTR ptrDirEnd = _tcsrchr(szDllFilePath, _T('\\'));
				if(ptrDirEnd)
				{
					*ptrDirEnd = _T('\0');
					CRegistryMgr::RegWrite(_T("HKEY_CURRENT_USER\\Software\\AralGood"), _T("NtleaDir"), szDllFilePath);
				}
			}
		}
		
		// 생성될 바로가기 대상의 전체길이가 250자를 넘는다면
		if( strAralTrans.GetLength()+strArg.GetLength() > 250 )
		{
			// 고유 ID 생성	
			UUID uuid = {0,};
			unsigned short* sz = NULL;

			if( UuidCreate(&uuid) != RPC_S_OK || UuidToString(&uuid, &sz) == RPC_S_OK )
			{
				CString strGUID = (LPCWSTR)sz;
				RpcStringFree(&sz);

				if( CRegistryMgr::RegWrite(_T("HKEY_CURRENT_USER\\Software\\AralGood\\StoredOption"), strGUID, strAralTrans + _T(" ") + strArg) == TRUE)
				{
					strArg.Format(_T("/r:%s"), strGUID);
				}

			}

		}
		
		BOOL bRes = CreateShortcut(sc_dlg.m_strShortcutFilePath, strAralTrans, strArg, strWorkDir, szExeFileName, _T("AralTrans Autorun Icon"));

		if(bRes)
		{
			this->MessageBox(_T("바로가기가 생성되었습니다."), _T("알림"));
		}
		else
		{
			this->MessageBox(_T("바로가기 생성 중 오류가 발생했습니다."), _T("알림"));
		}
		

	}

	// 작업 디렉토리 복구 (2008.10.28 by sc.Choi)
	::SetCurrentDirectory(strWorkDir);
}


//---------------------------------------------------------------------------
//	◎함수명 : CreateShortcut
//	◎함수설명 : 바로가기를 만든다
//	◎인자
//	strPathLink : 생성될 바로가기 파일의 경로와 이름
//  strObjPath : 실행할 오브젝트
//  strArgs : 실행시 인자
//	strIcon : 사용될 아이콘 리소스 지정 (EXE, ICO 등)
//	strDesc : 마우스가 올라갈때 나타나는 툴팁
//	◎반환값 : 성공시 TRUE, 실패시 FALSE
//---------------------------------------------------------------------------
BOOL COutputDialog::CreateShortcut(LPCTSTR strShortcutPath, LPCTSTR strObjPath, LPCTSTR strArgs, LPCTSTR strWorkingDir, LPCTSTR strIconPath, LPCTSTR strDesc)
{
	BOOL bRetVal = FALSE;

	HRESULT hres = 0;
	IShellLink* psl = NULL;
	IPersistFile* ppf = NULL;
	
	//CString strMyPath = strPathLink;
	
	try
	{
		CString strTmpDir = _T("");

		if(NULL == strShortcutPath || NULL == strObjPath
		   || _T('\0') == strShortcutPath[0] || _T('\0') == strObjPath[0]) throw _T("파일 위치 지정이 잘못되었습니다.");

		if(NULL == strIconPath || _T('\0') == strIconPath) strIconPath = strObjPath;
		if(NULL == strWorkingDir || _T('\0') == strWorkingDir)
		{
			strTmpDir = strObjPath;
			int nIdx = strTmpDir.ReverseFind('\\');
			if(nIdx > 0) strTmpDir = strTmpDir.Left(nIdx);
			else strTmpDir = _T("");
		}
		else
		{
			strTmpDir = strWorkingDir;
		}
		
		::CoInitialize(NULL);
		hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*) &psl);
		if (FAILED(hres)) throw _T("ShellLink 객체를 생성할 수 없습니다.");

		psl->SetPath(strObjPath);
		psl->SetIconLocation(strIconPath, 0);
		psl->SetWorkingDirectory(strTmpDir);
		if(strArgs && strArgs[0]) psl->SetArguments(strArgs);
		if(strDesc && strDesc[0]) psl->SetDescription(strDesc);

		hres = psl->QueryInterface( IID_IPersistFile, (LPVOID *) &ppf);
		if (FAILED(hres)) throw _T("IPersistFile 인터페이스를 얻어올 수 없습니다.");

		// 확장자를 검사하여 붙여줌
		CString strMyPath = strShortcutPath;
		if(strMyPath.Right(4).CompareNoCase(_T(".lnk"))) strMyPath += _T(".lnk");

#ifdef UNICODE
		LPCWSTR wsz = (LPCWSTR)strMyPath;
#else
		wchar_t wsz[MAX_PATH] = {0,};
		MyMultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, (LPCSTR)strMyPath, -1, wsz, MAX_PATH);
#endif

		// 생성
		DeleteFile(wsz);
		hres = ppf->Save(wsz, TRUE);		
		if (hres != S_OK ) throw _T("IPersistFile->Save() 에러");

		bRetVal = TRUE;

	}
	catch (LPCTSTR cszErr)
	{
		cszErr = cszErr;
		bRetVal = FALSE;
	}

	if(ppf) ppf->Release();
	if(psl) psl->Release();

	return bRetVal;
}

void COutputDialog::OnBnClickedCheck1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nTrayMin = m_chkTrayMin.GetCheck();
	CRegistryMgr::RegWriteDWORD(_T("HKEY_CURRENT_USER\\Software\\AralGood"), _T("TrayMinimize"), (DWORD)nTrayMin);
}


void COutputDialog::CreateTrayIcon()
{
	NOTIFYICONDATA  nid;
	nid.cbSize = sizeof(nid);
	nid.hWnd = m_hWnd; // 메인 윈도우 핸들
	nid.uID = IDR_MAINFRAME;  // 아이콘 리소스 ID
	nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP; // 플래그 설정
	nid.uCallbackMessage = WM_TRAYICON_MSG; // 콜백메시지 설정
	nid.hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME); // 아이콘 로드 

	TCHAR strTitle[256];
	this->GetWindowText(strTitle, sizeof(strTitle)); // 캡션바에 출력된 문자열 얻음
	_tcscpy(nid.szTip, strTitle); 
	Shell_NotifyIcon(NIM_ADD, &nid);
	this->SendMessage(WM_SETICON, (WPARAM)TRUE, (LPARAM)nid.hIcon);
}


// WM_TRAYICON_MSG 핸들러
LRESULT COutputDialog::TrayIconMsg(WPARAM wParam, LPARAM lParam)
{
	// 등록된 TrayIcon 클릭하면 Show....
	if(lParam == WM_LBUTTONDBLCLK)
	{  
		ShowWindow(SW_SHOW);
		DeleteTrayIcon();
	}

	return 0;
}

void COutputDialog::DeleteTrayIcon() 
{
	NOTIFYICONDATA  nid;
	nid.cbSize = sizeof(nid);
	nid.hWnd = m_hWnd; // 메인 윈도우 핸들
	nid.uID = IDR_MAINFRAME;

	// 작업 표시줄(TaskBar)의 상태 영역에 아이콘을 삭제한다.
	Shell_NotifyIcon(NIM_DELETE, &nid);
}


void COutputDialog::OnSysCommand(UINT nID, LPARAM lParam)
{
	// 최소화 버튼을 눌렀을때
	if(nID == SC_MINIMIZE && m_chkTrayMin.GetCheck())
	{ 
		CreateTrayIcon();
		ShowWindow(SW_HIDE);
		return;
	}

	CDialog::OnSysCommand(nID, lParam);
}

BOOL COutputDialog::InitFilterGrid()
{
	CWnd* pList = GetDlgItem(IDC_LIST_FILTER);
	if(NULL == pList) return FALSE;

	LVCOLUMN lvColumn;
	ZeroMemory(&lvColumn, sizeof(lvColumn));

	// 리스트뷰 스타일 = 한줄 선택, 그리드라인
	ListView_SetExtendedListViewStyle(pList->m_hWnd, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	// 리스트뷰 컬럼 등록
	lvColumn.mask=LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvColumn.fmt=LVCFMT_LEFT;
	lvColumn.cx=90;
	lvColumn.pszText=_T("플러그인");
	lvColumn.iSubItem=0;
	ListView_InsertColumn(pList->m_hWnd, 0, &lvColumn);

	lvColumn.pszText=_T("현재설정");
	lvColumn.cx=90;
	lvColumn.iSubItem=1;
	ListView_InsertColumn(pList->m_hWnd, 1, &lvColumn);

	return TRUE;
}

void COutputDialog::UpdateFilterGrid()
{

	CWnd* pList = GetDlgItem(IDC_LIST_FILTER);
	CWnd* pFilterOpt = GetDlgItem(IDC_EDIT_FILTER_OPTION_STRING);
	CWnd* pFilterStatic = GetDlgItem(IDC_STATIC_FILTER_1);
	if(NULL == pList || NULL == pFilterOpt || NULL == pFilterStatic) return;

	// 등록 리스트 가져와서
	ATPLUGIN_ARGUMENT_ARRAY aPluginArgs;
	(MAINAPP)->m_cMultiPlugin.GetPluginArgs(aPluginArgs);

	// 리스트뷰 전체 삭제
	ListView_DeleteAllItems(pList->m_hWnd);

	// 재등록
	TCHAR szBuffer[1024];
	LVITEM lvItem;
	ZeroMemory(&lvItem, sizeof(lvItem));
	lvItem.mask=LVIF_TEXT;
	lvItem.pszText=szBuffer;
	CString strFilterOpt = _T("");
	for(size_t i=0; i< aPluginArgs.size(); i++)
	{
		lvItem.iItem=i;
		lvItem.iSubItem=0;
		lstrcpyn(szBuffer, aPluginArgs[i].strPluginName.c_str(), 1024);
		ListView_InsertItem(pList->m_hWnd, &lvItem);

		lstrcpyn(szBuffer, aPluginArgs[i].strOption.c_str(), 4096);
		ListView_SetItemText(pList->m_hWnd, i, 1, szBuffer);

		if(strFilterOpt.IsEmpty())
		{
			strFilterOpt = CString(_T("/f:")) + aPluginArgs[i].strPluginName.c_str()
						 + _T("{") + aPluginArgs[i].strOption.c_str() + _T("}");
		}
		else
		{
			strFilterOpt += CString(_T(",")) + aPluginArgs[i].strPluginName.c_str()
				+ _T("{") + aPluginArgs[i].strOption.c_str() + _T("}");
		}
	}

	::EnableWindow(pList->m_hWnd, TRUE);

	pFilterOpt->SetWindowText(strFilterOpt);
	BOOL bEnable = strFilterOpt.IsEmpty() ? FALSE : TRUE;
	::EnableWindow(pFilterOpt->m_hWnd, bEnable);
	::EnableWindow(pFilterStatic->m_hWnd, bEnable);		

	OnNMClickListFilter(NULL, NULL);

	return;
}


void COutputDialog::OnBnClickedBtnFilterSetting()
{
	CWnd* pList = GetDlgItem(IDC_LIST_FILTER);
	int nSelected = ListView_GetSelectionMark(pList->m_hWnd);

	if (nSelected < 0) 
	{
		MessageBox(_T("플러그인을 먼저 선택해 주세요"), _T("Aral Trans"), MB_OK | MB_ICONINFORMATION );
	}
	else if ((MAINAPP)->m_cMultiPlugin.OnPluginOption(nSelected))
	{
		UpdateFilterGrid();
		ListView_SetItemState(pList->m_hWnd, nSelected, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
		::SetFocus(pList->m_hWnd);
		OnNMClickListFilter(NULL, NULL);
	}
}

void COutputDialog::OnBnClickedBtnFilterAdd()
{
	TCHAR szPluginName[MAX_PATH];
	szPluginName[0]=_T('\0');

	if ( DialogBoxParam(GetModuleHandle(_T("ATCTNR.dll")), MAKEINTRESOURCE(IDD_ADDFILTERDIALOG), 
		m_hWnd, AddPluginDialogProc, (LPARAM)szPluginName) == IDOK )
	{
		ATPLUGIN_ARGUMENT arg;
		arg.strPluginName=szPluginName;
		(MAINAPP)->m_cMultiPlugin.Add(arg);
		UpdateFilterGrid();

		CWnd* pList = GetDlgItem(IDC_LIST_FILTER);
		int nSelected = (MAINAPP)->m_cMultiPlugin.GetFilterCount() - 1;
		
		if(nSelected >= 0)
		{
			ListView_SetItemState(pList->m_hWnd, nSelected, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
			::SetFocus(pList->m_hWnd);
			OnNMClickListFilter(NULL, NULL);
		}
	}
}

void COutputDialog::OnBnClickedBtnFilterDelete()
{
	CWnd* pList = GetDlgItem(IDC_LIST_FILTER);
	int nSelected = ListView_GetSelectionMark(pList->m_hWnd);

	if (nSelected < 0) 
	{
		MessageBox(_T("플러그인을 먼저 선택해 주세요"), _T("Aral Trans"), MB_OK | MB_ICONINFORMATION );
	}
	else
	{
		(MAINAPP)->m_cMultiPlugin.Remove(nSelected);
		UpdateFilterGrid();

		if(nSelected >= (MAINAPP)->m_cMultiPlugin.GetFilterCount()) nSelected--;

		if(nSelected >= 0)
		{
			ListView_SetItemState(pList->m_hWnd, nSelected, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
			::SetFocus(pList->m_hWnd);
			OnNMClickListFilter(NULL, NULL);
		}
	}
}

void COutputDialog::OnBnClickedBtnFilterUp()
{
	CWnd* pList = GetDlgItem(IDC_LIST_FILTER);
	int nSelected = ListView_GetSelectionMark(pList->m_hWnd);

	if (nSelected < 0) 
	{
		MessageBox(_T("플러그인을 먼저 선택해 주세요"), _T("Aral Trans"), MB_OK | MB_ICONINFORMATION );
	}
	else
	{
		if(nSelected>0)
		{
			(MAINAPP)->m_cMultiPlugin.MoveUp(nSelected);
			UpdateFilterGrid();
			nSelected--;
		}

		//ListView_SetSelectionMark(pList->m_hWnd, nSelected);
		ListView_SetItemState(pList->m_hWnd, nSelected, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
		::SetFocus(pList->m_hWnd);
		OnNMClickListFilter(NULL, NULL);
	}
}

void COutputDialog::OnBnClickedBtnFilterDown()
{
	CWnd* pList = GetDlgItem(IDC_LIST_FILTER);
	int nSelected = ListView_GetSelectionMark(pList->m_hWnd);

	if (nSelected < 0) 
	{
		MessageBox(_T("플러그인을 먼저 선택해 주세요"), _T("Aral Trans"), MB_OK | MB_ICONINFORMATION );
	}
	else
	{
		if(nSelected < (MAINAPP)->m_cMultiPlugin.GetFilterCount()-1 )
		{
			(MAINAPP)->m_cMultiPlugin.MoveDown(nSelected);
			UpdateFilterGrid();
			nSelected++;
		}

		//ListView_SetSelectionMark(pList->m_hWnd, nSelected);
		ListView_SetItemState(pList->m_hWnd, nSelected, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
		::SetFocus(pList->m_hWnd);
		OnNMClickListFilter(NULL, NULL);
	}
}

void COutputDialog::OnNMDblclkListFilter(NMHDR *pNMHDR, LRESULT *pResult)
{
	//LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<NMITEMACTIVATE>(pNMHDR);
	SendMessage(WM_COMMAND, IDC_BTN_FILTER_SETTING, NULL);
	*pResult = 0;
}


void COutputDialog::OnBnClickedBtnFilterDirectSetting()
{
	CWnd* pFilterOpt = GetDlgItem(IDC_EDIT_FILTER_OPTION_STRING);
	CString strPrevFilters;
	pFilterOpt->GetWindowText(strPrevFilters);
	if(strPrevFilters.GetLength() > 3 && strPrevFilters.Left(3).CompareNoCase(_T("/f:")) == 0)
		strPrevFilters = strPrevFilters.Mid(3);

	CFilterDirectInputDlg prompt_dlg;
	prompt_dlg.m_strFilters = strPrevFilters;

	if (prompt_dlg.DoModal() == IDOK)
	{
		char szStringBuffer[4096];
		MyWideCharToMultiByte(949, 0, prompt_dlg.m_strFilters, -1, szStringBuffer, 4096, NULL, NULL);
		ATPLUGIN_ARGUMENT_ARRAY aPluginArgs;
		GetATPluginArgsFromOptionString(szStringBuffer, aPluginArgs);

		(MAINAPP)->m_cMultiPlugin.RemoveAll();
		if((MAINAPP)->m_cMultiPlugin.Add(aPluginArgs) == FALSE)
		{
			MessageBox(_T("값을 적용하는데 실패했습니다."), _T("오류"));
			MyWideCharToMultiByte(949, 0, strPrevFilters, -1, szStringBuffer, 4096, NULL, NULL);
			GetATPluginArgsFromOptionString(szStringBuffer, aPluginArgs);
			(MAINAPP)->m_cMultiPlugin.RemoveAll();
			(MAINAPP)->m_cMultiPlugin.Add(aPluginArgs);
		}
		UpdateFilterGrid();
	}
}

void COutputDialog::OnLvnKeydownListFilter(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);
	
	CWnd* pList = GetDlgItem(IDC_LIST_FILTER);
	int nSelected = ListView_GetSelectionMark(pList->m_hWnd);

	if (nSelected >= 0 
		&& pLVKeyDow->hdr.hwndFrom == pList->m_hWnd 
		&& pLVKeyDow->hdr.code == LVN_KEYDOWN )
	{
		switch(pLVKeyDow->wVKey)
		{
			case VK_RETURN:
				OnBnClickedBtnFilterSetting();
				break;
			case VK_INSERT:
				OnBnClickedBtnFilterAdd();
				break;
			case VK_DELETE:
				OnBnClickedBtnFilterDelete();
				break;
		}
	}
	
	*pResult = 0;
}

//void COutputDialog::OnNMKillfocusListFilter(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	CWnd* pBtnFilterSetting	= GetDlgItem(IDC_BTN_FILTER_SETTING);
//	CWnd* pBtnFilterDelete	= GetDlgItem(IDC_BTN_FILTER_DELETE);
//	CWnd* pBtnFilterUp		= GetDlgItem(IDC_BTN_FILTER_UP);
//	CWnd* pBtnFilterDown	= GetDlgItem(IDC_BTN_FILTER_DOWN);
//
//	if(pBtnFilterSetting && ::IsWindow(pBtnFilterSetting->m_hWnd))
//		pBtnFilterSetting->EnableWindow(FALSE);
//	if(pBtnFilterDelete && ::IsWindow(pBtnFilterDelete->m_hWnd))
//		pBtnFilterDelete->EnableWindow(FALSE);
//	if(pBtnFilterUp && ::IsWindow(pBtnFilterUp->m_hWnd))
//		pBtnFilterUp->EnableWindow(FALSE);
//	if(pBtnFilterDown && ::IsWindow(pBtnFilterDown->m_hWnd))
//		pBtnFilterDown->EnableWindow(FALSE);
//
//	*pResult = 0;
//}

//void COutputDialog::OnNMSetfocusListFilter(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	CWnd* pList = GetDlgItem(IDC_LIST_FILTER);
//	int nSelected = ListView_GetSelectionMark(pList->m_hWnd);
//
//	if (nSelected >= 0)
//	{
//		CWnd* pBtnFilterSetting	= GetDlgItem(IDC_BTN_FILTER_SETTING);
//		CWnd* pBtnFilterDelete	= GetDlgItem(IDC_BTN_FILTER_DELETE);
//		CWnd* pBtnFilterUp		= GetDlgItem(IDC_BTN_FILTER_UP);
//		CWnd* pBtnFilterDown	= GetDlgItem(IDC_BTN_FILTER_DOWN);
//
//		if(pBtnFilterSetting && ::IsWindow(pBtnFilterSetting->m_hWnd))
//			pBtnFilterSetting->EnableWindow(TRUE);
//		if(pBtnFilterDelete && ::IsWindow(pBtnFilterDelete->m_hWnd))
//			pBtnFilterDelete->EnableWindow(TRUE);
//		if(pBtnFilterUp && ::IsWindow(pBtnFilterUp->m_hWnd))
//			pBtnFilterUp->EnableWindow(TRUE);
//		if(pBtnFilterDown && ::IsWindow(pBtnFilterDown->m_hWnd))
//			pBtnFilterDown->EnableWindow(TRUE);
//	}
//	else
//	{
//		OnNMKillfocusListFilter(pNMHDR, pResult);
//	}
//
//	*pResult = 0;
//}

void COutputDialog::OnNMClickListFilter(NMHDR *pNMHDR, LRESULT *pResult)
{
	//LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<NMITEMACTIVATE>(pNMHDR);

	CWnd* pList = GetDlgItem(IDC_LIST_FILTER);
	int nSelected = ListView_GetSelectionMark(pList->m_hWnd);

	BOOL bFilterSelected = FALSE;
	if (nSelected >= 0)
	{
		bFilterSelected = TRUE;
	}

	CWnd* pBtnFilterSetting	= GetDlgItem(IDC_BTN_FILTER_SETTING);
	CWnd* pBtnFilterDelete	= GetDlgItem(IDC_BTN_FILTER_DELETE);
	CWnd* pBtnFilterUp		= GetDlgItem(IDC_BTN_FILTER_UP);
	CWnd* pBtnFilterDown	= GetDlgItem(IDC_BTN_FILTER_DOWN);

	if(pBtnFilterSetting && ::IsWindow(pBtnFilterSetting->m_hWnd))
		pBtnFilterSetting->EnableWindow(bFilterSelected);
	if(pBtnFilterDelete && ::IsWindow(pBtnFilterDelete->m_hWnd))
		pBtnFilterDelete->EnableWindow(bFilterSelected);
	if(pBtnFilterUp && ::IsWindow(pBtnFilterUp->m_hWnd))
		pBtnFilterUp->EnableWindow(bFilterSelected);
	if(pBtnFilterDown && ::IsWindow(pBtnFilterDown->m_hWnd))
		pBtnFilterDown->EnableWindow(bFilterSelected);

	if(pResult) *pResult = 0;
}