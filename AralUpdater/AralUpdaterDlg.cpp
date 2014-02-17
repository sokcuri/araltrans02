// AralUpdaterDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AralUpdater.h"
#include "AralUpdaterDlg.h"
#include "HttpDownloader/HttpDownloader.h"
#include "RegistryMgr/cRegistryMgr.h"
#include "AralFileAPI/AralFileAPI.h"
#include "AralProcessAPI/AralProcessAPI.h"
#include <afxinet.h>
#include <wingdi.h>
#include <winbase.h>
#include "unzip.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define DEFAULT_UPDATE_INFO_URL	_T("http://www.aralgood.com/update_files/AralTransUpdate.ini")
//#define UPDATE_INFO_FILE_NAME	_T("update.ini")


/////////////////////////////////////////////////////////////////////////////
// CAralUpdaterDlg dialog

CAralUpdaterDlg::CAralUpdaterDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAralUpdaterDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAralUpdaterDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_BkBrush.CreateSolidBrush(RGB(255, 255, 255));	// 브러시 세팅 (박스의 배경색)
	m_nAlpha = 0;
	m_pRgn = NULL;
	m_pBG = NULL;
	m_hUserDll = NULL;
	m_pThread = NULL;
	m_pChildThread = NULL;
	m_paWorkList = NULL;
	m_nTotalSize = 0;
	m_nTranSize = 0;
	m_bStop = FALSE;
	m_BoldFont.CreateFont( 11,0,0,0,FW_NORMAL,FALSE,FALSE,0,
							DEFAULT_CHARSET,
							OUT_DEFAULT_PRECIS,
							CLIP_DEFAULT_PRECIS,
							ANTIALIASED_QUALITY,
							DEFAULT_PITCH,
							_T("tahoma"));

	// Program Files 폴더
	m_strProgramFilesFolder = CRegistryMgr::RegRead( _T("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion"), _T("ProgramFilesDir"));
	if(m_strProgramFilesFolder.IsEmpty()) m_strProgramFilesFolder = _T("C:\\Program Files");
	
	// System32 폴더
	TCHAR szDir[MAX_PATH];
	ZeroMemory(szDir, MAX_PATH);
	::GetSystemDirectory(szDir, MAX_PATH);
	m_strSystemFolder = szDir;

	// 윈도우즈 폴더
	ZeroMemory(szDir, MAX_PATH);
	::GetWindowsDirectory(szDir, MAX_PATH);
	m_strWindowsFolder = szDir;

	
	// Wow64 프로세스인가
	m_bIsWow64 = FALSE;
	typedef BOOL (__stdcall *PROC_IsWow64Process)(HANDLE,PBOOL);	
	PROC_IsWow64Process pFunc = (PROC_IsWow64Process) GetProcAddress( GetModuleHandle(_T("kernel32.dll")), "IsWow64Process" );
	if(pFunc) pFunc( ::GetCurrentProcess(), &m_bIsWow64 );

}

BOOL CAralUpdaterDlg::SetWow64Redirection(BOOL bEnable)
{	
	PVOID OldValue = NULL;
	BOOL bRet = FALSE;

	typedef BOOL (__stdcall *PROC_Wow64FsRedirection)(PVOID);
	
	PROC_Wow64FsRedirection pFunc = NULL;
	
	// 리다이렉션 켠다
	if( bEnable )
	{
		pFunc = (PROC_Wow64FsRedirection) GetProcAddress( GetModuleHandle(_T("kernel32.dll")), "Wow64RevertWow64FsRedirection" );
		if(pFunc) bRet = pFunc (OldValue);
	}

	// 리다이렉션 끈다
	else
	{
		pFunc = (PROC_Wow64FsRedirection) GetProcAddress( GetModuleHandle(_T("kernel32.dll")), "Wow64DisableWow64FsRedirection" );
		if(pFunc) bRet = pFunc (&OldValue);
	}

	return bRet;
}


void CAralUpdaterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAralUpdaterDlg)
	DDX_Control(pDX, IDC_TRAN_SIZE, m_ctrlTranText);
	DDX_Control(pDX, IDC_COMMENT, m_ctrlCommentText);
	DDX_Control(pDX, IDC_PERCENT, m_ctrlPercentText);
	DDX_Control(pDX, IDC_PROGRESS1, m_ctrlProgress);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAralUpdaterDlg, CDialog)
	//{{AFX_MSG_MAP(CAralUpdaterDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_WM_CTLCOLOR()
	ON_WM_NCHITTEST()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAralUpdaterDlg message handlers

BOOL CAralUpdaterDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CString strServerINIData;
	if( PrepareUpdate(strServerINIData) == FALSE )
	{
		CleanupUpdate();
		PostQuitMessage(0);
		OnOK();
		return FALSE;
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	CenterWindow();
	// 스킨 비트맵 로딩 (이곳을 제거하면 '스킨없음'으로 설정할수있음)
	m_pBG = new CBitmap;
	m_pBG->LoadBitmap(IDB_BG_INSTALL);
	// 폰트세팅
	GetDlgItem(IDC_COMMENT)->SetFont(&m_BoldFont);
	GetDlgItem(IDC_TRAN_SIZE)->SetFont(&m_BoldFont);
	GetDlgItem(IDC_PERCENT)->SetFont(&m_BoldFont);
	// 둥근 모서리
// 	CRect rcWindow;
// 	GetWindowRect(rcWindow);
// 	m_pRgn = new CRgn;
// 	m_pRgn->CreateRoundRectRgn (0, 0, rcWindow.Width() + 1, rcWindow.Height() + 1, 15, 15);
// 	SetWindowRgn(*m_pRgn, TRUE);

	// 투명도 세팅
 	m_hUserDll = ::LoadLibrary(_T("USER32.dll"));
 	SetWindowLong(m_hWnd, GWL_EXSTYLE, ::GetWindowLong(m_hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
	m_nAlpha = 100;
//	SetTransparent(m_hWnd, 0, 0, LWA_ALPHA );

	SetTimer(0, 50, NULL);
	
	// 메인 쓰레드 및 제 2 쓰레드 시작
 	m_pThread = ::AfxBeginThread(MainThread, this);
 	m_pChildThread = ::AfxBeginThread(StaticUpdateThread, this);
	//프로그래스바의 배경색과 바 색을 변경
	m_ctrlProgress.SendMessage(PBM_SETBKCOLOR, (WPARAM)0, (LPARAM)(COLORREF)RGB(230,230,230) );
	m_ctrlProgress.SendMessage(PBM_SETBARCOLOR, (WPARAM)0, (LPARAM)(COLORREF)RGB(130,190,255) ); //RGB(0x32,0x8C,0xFF)
	
	// 취소 버튼
// 	m_btnCancel.Create(NULL, BS_OWNERDRAW | WS_VISIBLE | WS_CHILD, CRect(120,170,20,20), this, IDC_WORK_STOP);
// 	m_btnCancel.LoadBitmaps(IDB_BTN_CANCEL_NORMAL, IDB_BTN_CANCEL_DOWN);
// 	m_btnCancel.SizeToContent();

	CStringA systemDir = _T("");
	CStringA FindFileName = _T("");

	::GetSystemDirectoryA(systemDir.GetBuffer(255), 255);
	FindFileName.Format("%s\\%s", systemDir, L"hhctrl.ocx");
	HINSTANCE h = ::LoadLibraryA(FindFileName);
	if (h != NULL)
		FARPROC pFunc = ::GetProcAddress(h,"DllRegisterServer");

	FILE *pData;
	pData=fopen("help.chm","rb");
	CString strdir  = _T("help.chm");
	CString strurl = _T("http://www.aralgood.com/update_files/help.chm");
	if(pData==NULL){
		DeleteUrlCacheEntryW(strurl);
		URLDownloadToFileW(NULL,strurl,strdir,NULL,NULL);
		DeleteUrlCacheEntryW(strurl);
	}
	else
	{
		fseek(pData,0,SEEK_END);
		if(ftell(pData) <= 0)
		{
			fclose(pData);
			DeleteUrlCacheEntryW(strurl);
			URLDownloadToFileW(NULL,strurl,strdir,NULL,NULL);
			DeleteUrlCacheEntryW(strurl);
		}
		else
			fclose(pData);
	}
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

BOOL CAralUpdaterDlg::PrepareUpdate(CString &strINIData)
{
	BOOL bRetVal = FALSE;

	strINIData.Empty();
	CString strServerINIData;
	
	try
	{
		// 레지스트리에 등록되어 있는 update.ini 파일의 URL주소를 가져옴
		CString strUpdateInfoURL = CRegistryMgr::RegRead( _T("HKEY_CURRENT_USER\\Software\\AralGood\\Update"), _T("UpdateInfoURL") );
		if(strUpdateInfoURL.IsEmpty())
		{
			strUpdateInfoURL = DEFAULT_UPDATE_INFO_URL;
			CRegistryMgr::RegWrite( _T("HKEY_CURRENT_USER\\Software\\AralGood\\Update"), _T("UpdateInfoURL"), strUpdateInfoURL );
		}

		// 서버상의 최신 업데이트 정보
		if( CHttpDownloader::Http_Download_To_Buffer(strUpdateInfoURL, strServerINIData, NULL) != 0 ) 
			throw _T("[ aral1 ] [Updater] Err 3 \n");

		strINIData = strServerINIData;

		// 파싱
		if( m_ININewVer.Parse(strServerINIData) == FALSE )
			throw _T("[ aral1 ] [Updater] Err 4 \n");

		CININode* pNode = m_ININewVer.FindNode(_T("AppInfo"));

		if( NULL == pNode ) 
			throw _T("[ aral1 ] [Updater] Err 6 \n");

		// 만약 업데이트 정보 URL이 변경되었다면 리다이렉션 처리
		CString strNewInfoURL = pNode->GetAttribute(_T("UpdateInfoURL"));
		TRACE( _T("[ aral1 ] [Updater] OLD URL : '%s' \n"), (LPCTSTR)strUpdateInfoURL );
		TRACE( _T("[ aral1 ] [Updater] NEW URL : '%s' \n"), (LPCTSTR)strNewInfoURL );
		if( strUpdateInfoURL.CompareNoCase(strNewInfoURL) != 0 )
		{
			TRACE( _T("[ aral1 ] [Updater] URL redirect to '%s' \n"), (LPCTSTR)strNewInfoURL );

			m_ININewVer.Clear();
			
			// 레지스트리에 등록되어 있는 update.ini 파일의 새로운 URL주소를 등록
			CRegistryMgr::RegWrite( _T("HKEY_CURRENT_USER\\Software\\AralGood\\Update"), _T("UpdateInfoURL"), strNewInfoURL );
			return PrepareUpdate(strINIData);
		}


		CString strServerVersion = pNode->GetAttribute(_T("Version"));

		TRACE( _T("[ aral1 ] Latest Version : '%s' \n"), strServerVersion );


		// 로컬상의 현재 업데이트 정보
		CString strRegPath = pNode->GetAttribute(_T("RegPath"));

		/*
		if( !strRegPath.IsEmpty() )
		{
		// 클라이언트 설치 디렉토리
		m_strClientFolder = CRegistryMgr::RegRead( strRegPath, "AppDir" );
		// 		if( m_strClientFolder.IsEmpty() )
		// 		{		
		// 			m_strClientFolder =  ParseSystemVariable( pNode->GetAttribute("DefaultAppDir") );
		// 			if( m_strClientFolder.IsEmpty() )
		// 			{
		// 				TRACE( _T("[ aral1 ] [Updater] Err 8 \n") );
		// 				return FALSE;
		// 			}
		// 
		// 		}
		}

		if( m_strClientFolder.IsEmpty() ) m_strClientFolder = strCurDir;
		*/

		m_strClientFolder = CAralFileAPI::GetFileDir( CAralProcessAPI::GetCurrentModulePath() );

		CString strLocalVersion = CRegistryMgr::RegRead( strRegPath, _T("Version") );

		TRACE( _T("[ aral1 ] Current App Version : '%s' \n"), strLocalVersion );

		// 클라이언트가 없거나 버전이 낮으면 다운로드
		m_paWorkList = new CPtrArray();
		AddCommonFileWork(m_paWorkList);
		AddDriverFileWork(m_paWorkList);

		// 받아야 할 파일이 한개도 없으면
		if( m_paWorkList->GetSize() == 0 )
			throw _T("[ aral1 ] [Updater] No more Update File \n");

		bRetVal = TRUE;
	}
	catch (LPCTSTR strErr)
	{
		TRACE(strErr);
		strErr = NULL;
	}


	return bRetVal;
}

BOOL CAralUpdaterDlg::AddCommonFileWork(CPtrArray* pWorkList)
{
	// 일반 파일
	int cnt = 0;
	CString strNodeName;
	
	strNodeName.Format(_T("File%d"), cnt);
	CININode* pNode = m_ININewVer.FindNode(strNodeName);

	while( pNode )
	{
		CString strVersion   = pNode->GetAttribute(_T("Version"));
		CString strFilename  = ParseSystemVariable( pNode->GetAttribute(_T("Location")) );
		CString strFileURL   = pNode->GetAttribute(_T("DownloadURL"));
		
		CFileFind find;
		
		// 파일이 없거나 최신버전과 다르면 파일 다운로드 예약
		if( find.FindFile( strFilename ) == FALSE 
			|| ( !strVersion.IsEmpty() && strVersion != GetFileVersion(strFilename) )
		)
		{
			if(strFilename.Mid(strFilename.ReverseFind(_T('\\')) + 1) == _T("help.chm"))
				CRegistryMgr::RegWrite( _T("HKEY_CURRENT_USER\\Software\\AralGood\\Update"), _T("HelpVersion"), strVersion );
			CFileWork* work = new CFileWork();
			work->m_strURL = strFileURL;
			work->m_strLocalPath = strFilename;
			pWorkList->Add(work);
			TRACE( _T("[ aral1 ] Download '%s' to '%s' \n"), work->m_strURL, work->m_strLocalPath );
		}

		// 다음 파일
		cnt++;
		strNodeName.Format(_T("File%d"), cnt);
		pNode = m_ININewVer.FindNode(strNodeName);

	}

	return TRUE;
}

BOOL CAralUpdaterDlg::AddDriverFileWork(CPtrArray* pWorkList)
{
	int cnt = 0;
	CString strNodeName = _T("");
	CININode* pNode = NULL;
	
	// x86 driver 파일
	if( m_bIsWow64 == FALSE )
	{

		strNodeName.Format(_T("x86Driver%d"), cnt);
		pNode = m_ININewVer.FindNode(strNodeName);

		// 드라이버들 추가
		while( pNode )
		{
			
			CString strVersion   = pNode->GetAttribute(_T("Version"));
			CString strFileURL   = pNode->GetAttribute(_T("DownloadURL"));
			int nStrIdx = strFileURL.ReverseFind(_T('/'));
			if(nStrIdx>0)
			{
				CString strFilename  = m_strWindowsFolder + _T("\\System32\\drivers\\") + strFileURL.Mid(nStrIdx+1);				
				CFileFind find;
				
				// 파일이 없거나 최신버전과 다르면 파일 다운로드 예약
				if( find.FindFile( strFilename ) == FALSE 
					|| ( !strVersion.IsEmpty() && strVersion != GetFileVersion(strFilename) )
				)
				{
					CFileWork* work = new CFileWork();
					work->m_strURL = strFileURL;
					work->m_strLocalPath = strFilename;
					pWorkList->Add(work);
					TRACE( _T("[ aral1 ] Download '%s' to '%s' \n"), work->m_strURL, work->m_strLocalPath );
				}
			}


			// 다음 파일
			cnt++;
			strNodeName.Format(_T("x86Driver%d"), cnt);
			pNode = m_ININewVer.FindNode(strNodeName);

		}
		

	}
	// x64 driver 파일
	else
	{

		int cnt = 0;
		strNodeName.Format(_T("x64Driver%d"), cnt);
		pNode = m_ININewVer.FindNode(strNodeName);

		// 64비트면 리다이렉션 끈다
		PVOID OldValue = NULL;
		BOOL bRet = SetWow64Redirection(FALSE);

		// 드라이버들 추가
		while( pNode )
		{
			CString strVersion   = pNode->GetAttribute(_T("Version"));
			CString strFileURL   = pNode->GetAttribute(_T("DownloadURL"));
			int nStrIdx = strFileURL.ReverseFind('/');
			if(nStrIdx>0)
			{
				CString strFilename1  = m_strWindowsFolder + _T("\\System32\\drivers\\") + strFileURL.Mid(nStrIdx+1);
				CFileFind find;
				
				// 파일이 없거나 최신버전과 다르면 파일 다운로드 예약
				if( find.FindFile( strFilename1 ) == FALSE 
					|| ( !strVersion.IsEmpty() && strVersion != GetFileVersion(strFilename1) )
				)
				{
					CFileWork* work = new CFileWork();
					work->m_strURL = strFileURL;
					work->m_strLocalPath = strFilename1;
					pWorkList->Add(work);
					TRACE( _T("[ aral1 ] Download '%s' to '%s' \n"), work->m_strURL, work->m_strLocalPath );
				}

				CString strFilename2  = m_strWindowsFolder + _T("\\SysWow64\\drivers\\") + strFileURL.Mid(nStrIdx+1);
				
				// 파일이 없거나 최신버전과 다르면 파일 다운로드 예약
				if( find.FindFile( strFilename2 ) == FALSE 
					|| ( !strVersion.IsEmpty() && strVersion != GetFileVersion(strFilename2) )
				)
				{
					CFileWork* work = new CFileWork();
					work->m_strURL = strFileURL;
					work->m_strLocalPath = strFilename2;
					pWorkList->Add(work);
					TRACE( _T("[ aral1 ] Download '%s' to '%s' \n"), work->m_strURL, work->m_strLocalPath );
				}
			}


			// 다음 파일
			cnt++;
			strNodeName.Format(_T("x64Driver%d"), cnt);
			pNode = m_ININewVer.FindNode(strNodeName);

		}

		// 리다이렉션 켠다
		bRet = SetWow64Redirection(TRUE);
	}

	return TRUE;
}


BOOL CAralUpdaterDlg::CleanupUpdate()
{
	if( m_paWorkList )
	{
		// 작업리스트 모든 원소들 지움
		int i, count = m_paWorkList->GetSize();
		for(i=0; i<count; i++)
		{
			if(m_paWorkList->GetAt(i)) delete (CFileWork*) m_paWorkList->GetAt(i);
		}
		m_paWorkList->RemoveAll();
		delete m_paWorkList;
		m_paWorkList = NULL;

	}

	DoRegistryWork();
	
	// 레지스트리에 업데이트 정보 갱신
	CININode* pNode = m_ININewVer.FindNode(_T("AppInfo"));
	if( pNode )
	{
		// 로컬상의 현재 업데이트 정보
		CString strRegPath = pNode->GetAttribute(_T("RegPath"));
		if( !strRegPath.IsEmpty() )
		{
			BOOL bRegWrite = CRegistryMgr::RegWrite( strRegPath, _T("AppDir"), m_strClientFolder );
			bRegWrite = CRegistryMgr::RegWrite( strRegPath, _T("Name"), pNode->GetAttribute(_T("Name")) );
			bRegWrite = CRegistryMgr::RegWrite( strRegPath, _T("Version"), pNode->GetAttribute(_T("Version")) );
		}

		// 오토런 실행
		CString strAutoRun = ParseSystemVariable( pNode->GetAttribute(_T("AutoRun")) );
		if( !strAutoRun.IsEmpty() )
		{
			// exe 파일 실행
 			STARTUPINFO si={0,};
 			PROCESS_INFORMATION pi;			
 			BOOL bRes = ::CreateProcess( NULL, (LPTSTR)(LPCTSTR)strAutoRun, NULL,NULL,FALSE,0,NULL, NULL, &si , &pi );
			TRACE( _T("[ aral1 ] Auto Run : %s -> Result:%d \n"), strAutoRun, bRes );
			//::ShellExecute(NULL, "open", (LPSTR)(LPCTSTR)strAutoRun, NULL, NULL, SW_SHOWNORMAL);
		}
	}

	
	return TRUE;
}






// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CAralUpdaterDlg::OnPaint() 
{
	/*
	CPaintDC dc(this); // device context for painting
	CDC* pWDC = GetWindowDC();
	CRect rect;
	GetWindowRect(&rect);
	pWDC->SelectObject(GetStockObject(NULL_BRUSH));
	pWDC->SelectObject(GetStockObject(BLACK_PEN));
	ReleaseDC(pWDC);
	*/
	CDialog::OnPaint();
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CAralUpdaterDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

BOOL CAralUpdaterDlg::DownloadUpdateInfo()
{
	return TRUE;
}

BOOL CAralUpdaterDlg::GetSelfUpdateRequirement()
{
	return TRUE;
}


UINT CAralUpdaterDlg::MainThread(void *lParam)
{
	CAralUpdaterDlg* pThisDlg;
	CPtrArray* paWorks;
	int nIndex = 0;
	int nCount = 0;
	CFileWork* work;
	//CWinThread* pChildThread; 

	pThisDlg = (CAralUpdaterDlg*)lParam;	// 이 다이얼로그의 포인터
	paWorks = pThisDlg->m_paWorkList;		// 작업목록
	//pChildThread = ::AfxBeginThread(StaticUpdateThread, lParam);	// 전송량을 표시해주는 자식 쓰레드 ON
	pThisDlg->m_nTotalSize = pThisDlg->GetTotalWorkSize();		// 총 전송해야할 양 구함
	
	
	// 64비트면 리다이렉션 끈다
	PVOID OldValue = NULL;
	if(pThisDlg->m_bIsWow64)
	{
		BOOL bRet = SetWow64Redirection(FALSE);
	}

	nCount = paWorks->GetSize();

	// 먼저 업데이터부터 갱신되어야 하는지 조사한다
	CString strThisUpdater = CAralProcessAPI::GetCurrentModulePath();
	if(strThisUpdater.Right(4).CompareNoCase(_T(".dmy")) == 0)
	{
		strThisUpdater = strThisUpdater.Left( strThisUpdater.GetLength()-4 ) + _T(".exe");
	}

	TCHAR szThisUpdater[MAX_PATH] = {0,};
	::GetLongPathName( (LPCTSTR)strThisUpdater, szThisUpdater, MAX_PATH );
	if(_tcslen(szThisUpdater) > 0)
	{
		strThisUpdater = szThisUpdater;
	}

	for(int i=0; i<nCount; i++)
	{
		work = (CFileWork*)paWorks->ElementAt(i);

		CString strDownFile = work->m_strLocalPath;
		TCHAR szDownFile[MAX_PATH] = {0,};
		::GetLongPathName( (LPCTSTR)strDownFile, szDownFile, MAX_PATH );
		if(_tcslen(szDownFile) > 0)
		{
			strDownFile = szDownFile;
		}

		if( strDownFile.CompareNoCase(strThisUpdater) == 0 )
		{
			// 여기서 파일 받는다
			pThisDlg->WorkDownload(work);

			// 업데이터를 다시 실행시키고
			STARTUPINFO si={0,};
			PROCESS_INFORMATION pi;			
			BOOL bRes = ::CreateProcess( NULL, (LPTSTR)(LPCTSTR)strThisUpdater,NULL,NULL,FALSE,0,NULL, NULL, &si , &pi );

			// 이건 종료
			::TerminateProcess( GetCurrentProcess(), 0 );
		}

		if(pThisDlg->m_bStop) break;
		nIndex++;
	}

	// 아니면 나머지 파일 다운로드 작업
	nIndex = 0;
	while(nIndex<nCount)
	{
		work = (CFileWork*)paWorks->ElementAt(nIndex);

		// 여기서 파일 받자
		pThisDlg->WorkDownload(work);

		if(pThisDlg->m_bStop) break;
		nIndex++;
	}

	// 리다이렉션 켠다
	if(pThisDlg->m_bIsWow64)
	{
		BOOL bRet = SetWow64Redirection(TRUE);
	}

	pThisDlg->m_pThread = NULL;
	while(pThisDlg->m_nAlpha > 0 || pThisDlg->m_pChildThread) ::Sleep(10);
	
	HWND hWnd = pThisDlg->GetSafeHwnd();
	if(pThisDlg->m_bStop)
	{
		::PostMessage(hWnd, WM_COMMAND, IDCANCEL, 0);
	}
	else
	{
		//pThisDlg->Invalidate(TRUE);
		//pThisDlg->CloseSmoothly();
		pThisDlg->CleanupUpdate();
		::PostMessage(hWnd, WM_COMMAND, IDOK, 0);
	}
	
	return 0;
}



// 다운로드 작업에 대한 총 크기를 구해주는 함수
// Byte단위로 반환
int CAralUpdaterDlg::GetTotalWorkSize()
{
	if(NULL==m_paWorkList) return 0;
	
	CFileWork* work = NULL;
	int nTotalSize = 0;
	int nIndex = 0;
	int nCount = m_paWorkList->GetSize();

	while(nIndex<nCount){		// 작업 개수만큼 루프
		work = (CFileWork*)m_paWorkList->ElementAt(nIndex);
		CString strURL = work->m_strURL;
		
		// 파일의 크기 구해서 더함
		DWORD dwStat;
		CInternetSession* pSession = new CInternetSession;
		CHttpFile *pHttpFile = NULL;
		pHttpFile = (CHttpFile*) pSession->OpenURL(strURL, pSession->GetContext()); //, 1, INTERNET_FLAG_TRANSFER_BINARY | INTERNET_FLAG_RELOAD | INTERNET_FLAG_DONT_CACHE);
		if(!pHttpFile->QueryInfoStatusCode(dwStat) || dwStat > 299 || dwStat < 200)
		{
			MessageBox(_T("지정된 HTTP파일을 찾을 수 없습니다 : ") + strURL);
		}

		DWORD dwSize;
		pHttpFile->QueryInfo(HTTP_QUERY_CONTENT_LENGTH, dwSize);
		nTotalSize += dwSize;

		pHttpFile->Close();
		delete pHttpFile;
		pSession->Close();
		delete pSession;
		nIndex++;
		::Sleep(0);
	}

	
	return nTotalSize;
}

// FTP에서 파일 1개를 다운로드한다
BOOL CAralUpdaterDlg::WorkDownload(CFileWork *pFW)
{

	CString strComment;
	int nDotIndex = pFW->m_strURL.ReverseFind( _T('.') );
	int nStartIndex = pFW->m_strURL.ReverseFind( _T('/') ) + 1;

	if(nDotIndex>0) strComment = pFW->m_strURL.Mid(nStartIndex, nDotIndex-nStartIndex);
	else strComment = pFW->m_strURL.Mid(nStartIndex);
	m_ctrlCommentText.SetWindowText(strComment + _T(" Module Request"));

	DWORD			dwService;		// 인터넷 서비스 종류
	CString			strURL;			// 완전한 URL
	CString			servername;		// 서버명
	CString			objname;		// 개체명
	INTERNET_PORT	nPort;			// 포트번호
	TCHAR strTempPath[MAX_PATH];
	TCHAR strTempFileName[MAX_PATH];
	if(!GetTempPath(MAX_PATH, strTempPath)) return FALSE;
	if(!GetTempFileName(strTempPath, _T("AralUpdateTempFile"), 0, strTempFileName)) return FALSE;

	strURL = pFW->m_strURL;
	if(!AfxParseURL(strURL, dwService, servername, objname, nPort))
	{
		CString msg;
		msg.Format( _T("invalid URL!\r\n%s"), strURL );
		MessageBox( msg );
		return FALSE;
	}


	// 인터넷 연결
	CInternetSession* pSession = new CInternetSession;
	CHttpFile *pHttpFile = NULL;

	// HTTP 파일 열기
	pHttpFile = (CHttpFile*) pSession->OpenURL(strURL, 1, INTERNET_FLAG_TRANSFER_BINARY | INTERNET_FLAG_RELOAD | INTERNET_FLAG_DONT_CACHE);

	// HTTP파일이 무사히 열렸다면 로컬에 저장 할 차례 (임시파일명으로)
	HANDLE hLocal = CreateFile(strTempFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL,NULL);
	if(hLocal != INVALID_HANDLE_VALUE)		// 로컬파일이 제대로 열렸다면
	{

		DWORD dwWritten;
		UINT nSize = 0;
		CHAR Data[16384] = "";
		BOOL bWriteOK;

		BOOL bBegin = FALSE;

		do {
			nSize = pHttpFile->Read((LPVOID)Data, 16384);
			if(nSize){
				if(!bBegin){
					m_ctrlCommentText.SetWindowText( strComment + _T(" Module Downloading..") );
					bBegin=TRUE;
				}
				bWriteOK = WriteFile(hLocal, Data, nSize, &dwWritten, NULL);
				if(bWriteOK) m_nTranSize += dwWritten;	// 총 전송량 증가
			}

			if(m_bStop){
				CloseHandle(hLocal);				// 로컬파일 닫기
				pHttpFile->Close();					// http파일 닫기
				DeleteFile(strTempFileName);		// 받던파일 삭제
				return FALSE;
			}
		} while(nSize);
		CloseHandle(hLocal);				// 로컬파일 닫기
		pHttpFile->Close();					// http파일 닫기
		delete pHttpFile;
		pSession->Close();					// Session 닫기
		delete pSession;
	}
	else
	{
		pHttpFile->Close();					// http파일 닫기
		delete pHttpFile;
		pSession->Close();					// Session 닫기
		delete pSession;
		CString err = _T("Cannot create temporary file!");
		err += strTempFileName;
		this->MessageBox(err, _T("Detectron Font Updater"), MB_ICONWARNING | MB_OK);
		return FALSE;
	}

	// 복사하기 전에 디렉토리 생성
	int nStrIdx = pFW->m_strLocalPath.ReverseFind(_T('\\'));
	if( nStrIdx > 0 )
	{
		CString strDirectory = pFW->m_strLocalPath.Left(nStrIdx);
		BOOL bCreateDir = CreateDirectoryRecursive( strDirectory );
	}

	
	
	BOOL bRetVal = FALSE;

	for(int i=0; i<5; i++)
	{
		// 압축파일이면 압축 해제
		if( pFW->m_strURL.Right(4).MakeUpper() == _T(".ZIP")
			&& pFW->m_strLocalPath.Right(4).MakeUpper() != _T(".ZIP") )
		{
			HZIP hz = OpenZip(strTempFileName, NULL);
			ZIPENTRY ze; 
			GetZipItem(hz,0,&ze);
			HANDLE h = CreateFile(pFW->m_strLocalPath,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,ze.attr,NULL);
			if (h != INVALID_HANDLE_VALUE)
			{
				if( UnzipItem(hz,0,h) == ZR_OK )
				{
					CloseHandle(h);
					CloseZip(hz);
					bRetVal = TRUE;
					break;
				}
				CloseHandle(h);
			}

			CloseZip(hz);
		}
		// 압축파일이 아니면 파일 복사
		else
		{
			if( CopyFile(strTempFileName, pFW->m_strLocalPath, FALSE) == TRUE )
			{
				bRetVal = TRUE;
				break;
			}
		}

		Sleep(1000);
	}
	
	DeleteFile(strTempFileName);		// 임시파일 삭제

	return bRetVal;
}

BOOL CAralUpdaterDlg::CreateDirectoryRecursive(CString strDir)
{
	CFileFind find;
	if( find.FindFile( strDir ) == TRUE ) return TRUE;

	int nStrIdx = strDir.ReverseFind(_T('\\'));
	if( nStrIdx < 0 ) return FALSE;

	CString strParentDir = strDir.Left(nStrIdx);
	if( CreateDirectoryRecursive( strParentDir ) == FALSE ) return FALSE;

	return CreateDirectory( strDir, NULL );

}



// 부드럽게 사라지는 다이알로그
BOOL CAralUpdaterDlg::SetTransparent(HWND hWnd, COLORREF crKey, BYTE bAlpha, DWORD dwFlags)
{
  BOOL bRet = TRUE;
  typedef BOOL (WINAPI* lpfnSetTransparent)(HWND hWnd, COLORREF crKey, BYTE bAlpha, DWORD dwFlags);

  if ( m_hUserDll )
  {
    lpfnSetTransparent pFnSetTransparent = NULL;
    pFnSetTransparent =
      (lpfnSetTransparent)GetProcAddress(m_hUserDll,
                    "SetLayeredWindowAttributes");

    if (pFnSetTransparent )
       bRet = pFnSetTransparent(hWnd, crKey, bAlpha, dwFlags);
    else
       bRet = FALSE;
  } // if( m_hUserDll )

   return bRet;
}

void CAralUpdaterDlg::CloseSmoothly()
{
  for(int nPercent=100; nPercent >= 0 ;nPercent--)
  {
	 SetTransparent( m_hWnd, 0, 255 * nPercent/100, LWA_ALPHA);
	 Sleep(15);
  }
}


BOOL CAralUpdaterDlg::PreTranslateMessage(MSG* pMsg)
{
	// 업데이트중 종료되는것을 막음
	if(pMsg->message == WM_SYSKEYDOWN && pMsg->wParam == VK_F4)
		return TRUE;

	if(pMsg->wParam == VK_ESCAPE)
		return TRUE;
	
	return CDialog::PreTranslateMessage(pMsg);
}

void CAralUpdaterDlg::OnTimer(UINT nIDEvent) 
{
 	if(m_pThread==NULL)
	{
 		m_nAlpha -= 6;
 		if(m_nAlpha < 0) m_nAlpha = 0;
 	}
 	else
	{
 		m_nAlpha += 6;
 		if(m_nAlpha > 100) m_nAlpha = 100;
 	}
 	SetTransparent(m_hWnd, 0, 255 * (BYTE)m_nAlpha / 100, LWA_ALPHA);
	
	//CDialog::OnTimer(nIDEvent);
}

void CAralUpdaterDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	KillTimer(0);
	if(m_pRgn) delete m_pRgn;
	if(m_pBG) delete m_pBG; //m_pBG->DeleteObject();
}



HBRUSH CAralUpdaterDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{         
	/*
	static HBRUSH hBrush_bmp=NULL;

	HBRUSH hbr;
    pDC->SetBkMode(TRANSPARENT);  
    if(!hBrush_bmp) hBrush_bmp=CreatePatternBrush((HBITMAP)*m_pBG);
    if(hBrush_bmp) hbr=hBrush_bmp;
	else hbr = (HBRUSH)m_BkBrush; //CDialog::OnCtlColor(pDC,pWnd,nCtlColor);
    return hbr;
	*/

	return CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
}

LRESULT CAralUpdaterDlg::OnNcHitTest(CPoint point) 
{
	UINT nHit = CDialog::OnNcHitTest(point);
	if(nHit == HTCLIENT) nHit = HTCAPTION;
	return nHit;
}



UINT CAralUpdaterDlg::StaticUpdateThread(void *lParam)
{
	CAralUpdaterDlg* pThisDlg;
	CString temp;

	pThisDlg = (CAralUpdaterDlg*)lParam;	// 이 다이얼로그의 포인터
	while(pThisDlg->m_pThread)
	{		
		// 총전송량 갱신		
		temp.Format(_T("%d / %d KBytes"), pThisDlg->m_nTranSize/1024, pThisDlg->m_nTotalSize/1024);
		pThisDlg->m_ctrlTranText.SetWindowText(temp);

		// 프로그래스바 갱신
		pThisDlg->m_ctrlProgress.SetRange(0, pThisDlg->m_nTotalSize/1024);
		pThisDlg->m_ctrlProgress.SetPos(pThisDlg->m_nTranSize/1024);

		// 퍼센트 갱신
		float percent = ((float)pThisDlg->m_nTranSize / (float)pThisDlg->m_nTotalSize) * 100;
		temp.Format(_T("%d%%"), (int)percent);
		pThisDlg->m_ctrlPercentText.SetWindowText(temp);
		Sleep(0);

	}

	pThisDlg->m_pChildThread = NULL;
	return 0;
}

//---------------------------------------------------------------------------
//	◎함수명 : GetFileVersion
//	◎함수설명 : 지정한 파일의 버전을 가르쳐준다.
//	◎인자 : CString 절대경로를 포함한 파일이름
//	◎반환값 : CString 형식의 버전 ("xx.xx.xx.xx")
//---------------------------------------------------------------------------
CString CAralUpdaterDlg::GetFileVersion(CString strFilePathName)
{
	DWORD dwHdlDest = 0;
	DWORD dwSizeDest = 0;
	DWORD dwDestLS, dwDestMS;
	CString strVersion = _T("");

	TRACE( _T("[ aral1 ] CBGActiveXCtrl::GetFileVersion(\"%s\") called! \n"), strFilePathName );
	dwSizeDest = GetFileVersionInfoSize((LPTSTR)(LPCTSTR)strFilePathName, &dwHdlDest);

	if(dwSizeDest)
	{
	   _TCHAR* pDestData = new TCHAR[dwSizeDest + 1];

	   if(GetFileVersionInfo((LPTSTR)(LPCTSTR)strFilePathName, dwHdlDest, dwSizeDest, pDestData))
	   {
		VS_FIXEDFILEINFO * pvsInfo;
		UINT uLen;
		BOOL dwRet;
		dwRet = VerQueryValue(pDestData, _T("\\"), (void**)&pvsInfo, &uLen);
		if(dwRet)
		{
			dwDestMS = pvsInfo->dwFileVersionMS;    // 주버전
			dwDestLS = pvsInfo->dwFileVersionLS;    // 보조버전

			int dwDestLS1, dwDestLS2, dwDestMS1, dwDestMS2;
			dwDestMS1 = (dwDestMS / 65536);
			dwDestMS2 = (dwDestMS % 65536);
			dwDestLS1 = (dwDestLS / 65536);
			dwDestLS2 = (dwDestLS % 65536);

			strVersion.Format( _T("%d.%d.%d.%d"),dwDestMS1,dwDestMS2,dwDestLS1,dwDestLS2);
			delete [] pDestData;
			return strVersion;
		}
	   }
	   delete [] pDestData;
	}
	else
	{
		if(strFilePathName.Mid(strFilePathName.ReverseFind(_T('\\')) + 1) == _T("help.chm"))
		{
			CString strUpdateInfoURL = CRegistryMgr::RegRead( _T("HKEY_CURRENT_USER\\Software\\AralGood\\Update"), _T("HelpVersion") );
			if( strUpdateInfoURL == _T("") )
				strVersion.Format( _T("0.0.0.0"));
			else
				strVersion = strUpdateInfoURL;
		}
	}
    return strVersion;
}

CString CAralUpdaterDlg::ParseSystemVariable(CString strSource)
{
	CString strResult = strSource;

	strResult.Replace( _T("%AppDir%"), m_strClientFolder );
	strResult.Replace( _T("%System32%"), m_strSystemFolder );
	strResult.Replace( _T("%Windows%"), m_strWindowsFolder );
	strResult.Replace( _T("%ProgramFiles%"), m_strProgramFilesFolder );

	return strResult;

}


BOOL CAralUpdaterDlg::DoRegistryWork()
{
	BOOL bRetVal = TRUE;

	// 레지스트리 작업
	int cnt = 0;
	CString strNodeName;
	
	strNodeName.Format(_T("Registry%d"), cnt);
	CININode* pNode = m_ININewVer.FindNode(strNodeName);

	while( pNode )
	{
		CString strRegPath		= pNode->GetAttribute(_T("RegPath"));
		CString strRegName		= pNode->GetAttribute(_T("RegName"));
		CString strRegValueType	= pNode->GetAttribute(_T("RegValueType"));
		CString strRegValue		= ParseSystemVariable( pNode->GetAttribute(_T("RegValue")) );

		if( strRegValueType == _T("DWORD") )
		{
			DWORD dwValue = (DWORD)_ttoi( strRegValue );
			bRetVal = bRetVal && CRegistryMgr::RegWriteDWORD( strRegPath, strRegName, dwValue );
			
		}
		else
		{
			bRetVal = bRetVal && CRegistryMgr::RegWrite( strRegPath, strRegName, strRegValue );

		}

		// 다음 레지스트리 작업
		cnt++;
		strNodeName.Format(_T("Registry%d"), cnt);
		pNode = m_ININewVer.FindNode(strNodeName);

	}

	return bRetVal;

}
