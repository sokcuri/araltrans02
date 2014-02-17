// AralUpdater.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "AralUpdater.h"
#include "AralUpdaterDlg.h"
#include "RegistryMgr/cRegistryMgr.h"
#include <afxinet.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAralUpdaterApp

BEGIN_MESSAGE_MAP(CAralUpdaterApp, CWinApp)
	//{{AFX_MSG_MAP(CAralUpdaterApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAralUpdaterApp construction

CAralUpdaterApp::CAralUpdaterApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CAralUpdaterApp object

CAralUpdaterApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CAralUpdaterApp initialization

BOOL CAralUpdaterApp::InitInstance()
{
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

	// 현재 실행모듈명
	TCHAR szExeFileName[MAX_PATH] = {0,};
	::GetModuleFileName( NULL, szExeFileName, MAX_PATH );
	size_t nDotIdx = _tcslen(szExeFileName) - 4;
	if( szExeFileName[nDotIdx] != _T('.') ) return FALSE;
	szExeFileName[nDotIdx] = _T('\0');

	// exe 파일명
	m_strExeFilePath = szExeFileName;
	m_strExeFilePath += _T(".exe");
	
	// 더미 파일명
	m_strDmyFilePath = szExeFileName;
	m_strDmyFilePath += _T(".dmy");

	// 실행인자
	CString strCmdLine = this->m_lpCmdLine;

	if( strCmdLine.Left(7) == _T("execute") )
	{
		// 이미 실행중이라면 나간다
		HANDLE hMutex = ::CreateMutex( NULL, FALSE, _T("AralUpdater_Mutex") );
		if( GetLastError() == ERROR_ALREADY_EXISTS )
		{
			CloseHandle( hMutex );
			return FALSE;
		}
		
		AfxEnableControlContainer();

		// Standard initialization
		// If you are not using these features and wish to reduce the size
		//  of your final executable, you should remove from the following
		//  the specific initialization routines you do not need.

		CAralUpdaterDlg dlg;
		m_pMainWnd = &dlg;
		
		int nResponse = dlg.DoModal();
		if (nResponse == IDOK)
		{
			// TODO: Place code here to handle when the dialog is
			//  dismissed with OK
		}
		else if (nResponse == IDCANCEL)
		{
			// TODO: Place code here to handle when the dialog is
			//  dismissed with Cancel
		}
	
		// exe 파일 실행
		STARTUPINFO si={0,};
		PROCESS_INFORMATION pi;			
		BOOL bRes = ::CreateProcess( NULL, (LPTSTR)(LPCTSTR)(m_strExeFilePath + _T(" delete")),NULL,NULL,FALSE,0,NULL, NULL, &si , &pi );
	}
	else if( strCmdLine.Left(6) == _T("delete") )
	{
		CFileFind find;
		
		// 파일이 존재하면
		if( find.FindFile( m_strDmyFilePath ) )
		{
			// 지워라
			for(int i=0; i<10; i++)
			{
				if( DeleteFile( m_strDmyFilePath ) ) break;
				Sleep(500);
			}

		}
	}
	else
	{		
		TRACE( _T("[ aral1 ] [Updater] Normal Start! \n") );
		
		CAralUpdaterDlg dlg;
		BOOL bNeedUpdate = FALSE;
		CString strServerINIData;

		bNeedUpdate = dlg.PrepareUpdate(strServerINIData);

		TRACE( _T("[ aral1 ] [Updater] dlg.PrepareUpdate() returned %d \n"), bNeedUpdate );

		if( bNeedUpdate == TRUE )
		{
			if( strCmdLine.Left(5) != "check" )
			{
				// 더미파일 생성
				for(int i=0; i<10; i++)
				{
					if( CopyFile( m_strExeFilePath, m_strDmyFilePath, FALSE ) ) break;
					Sleep(500);
				}

				// 더미파일 실행
				STARTUPINFO si={0,};
				PROCESS_INFORMATION pi;			
				BOOL bRes = ::CreateProcess( NULL, (LPTSTR)(LPCTSTR)(m_strDmyFilePath + _T(" execute")),NULL,NULL,FALSE,0,NULL, NULL, &si , &pi );

			}
			else
			{
				HANDLE hMapFile=NULL;
				hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, NULL, _T("ATUpdateINI"));

				if (hMapFile)
				{
					LPTSTR szINIFile=NULL;

					szINIFile = (LPTSTR) MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(TCHAR) * 20480 );
					if (szINIFile) 
					{
						int i, len;
						len=strServerINIData.GetLength();
						memcpy(szINIFile, (LPCTSTR) strServerINIData, (len+1) * sizeof(TCHAR));

						for(i=0; i<len; i++)
						{
							if (szINIFile[i] == _T('\0'))
								szINIFile[i] = _T('\n');
						}

						UnmapViewOfFile(szINIFile);
					}

					CloseHandle(hMapFile);
				}

			}

			// 엑시트코드 세팅하며 종료
			::TerminateProcess( GetCurrentProcess(), -1 );

		}
	}

	return FALSE;
	


	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
