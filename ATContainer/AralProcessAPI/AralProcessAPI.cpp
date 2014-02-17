// AralProcessAPI.cpp: implementation of the CAralProcessAPI class.
//
//////////////////////////////////////////////////////////////////////


#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif

#pragma warning(disable:4819)

#include <afx.h>
#include <tlhelp32.h>
#include <Psapi.h>
#include <Userenv.h>
#include <WtsApi32.h>
#include "AralProcessAPI.h"

#pragma comment(lib, "Userenv.lib")
#pragma comment(lib, "WtsApi32.lib")
#pragma comment(lib, "psapi.lib")

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAralProcessAPI::CAralProcessAPI()
{

}

CAralProcessAPI::~CAralProcessAPI()
{

}

DWORD CAralProcessAPI::GetPrimaryThreadId(DWORD dwPID)
{
	if(dwPID == 0) return 0;

	// 프로세스의 쓰레드 스냅샷을 만듭니다.
	CString szPath;
	HANDLE hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, dwPID);

	if(hThreadSnap == INVALID_HANDLE_VALUE)
	{
		TRACE( _T("[ aral1 ] ● Failed to CreateToolhelp32Snapshot \n") );
		return 0;
	}

	// 첫번째 스레드 요청해본다
	THREADENTRY32 te32 = {0,};
	te32.dwSize = sizeof(THREADENTRY32);

	if( !Thread32First( hThreadSnap, &te32 ) ) 
	{
		TRACE( _T("[ aral1 ] ● Failed to Thread32First \n") );
		CloseHandle( hThreadSnap );     // Must clean up the snapshot object!
		return 0;
	}

	// 첫 번째 쓰레드의 ID를 리턴합니다.
	DWORD dwTID = 0;

	do 
	{ 
		if( te32.th32OwnerProcessID == dwPID )
		{
			dwTID = te32.th32ThreadID;
			break;
		}
	} while( Thread32Next(hThreadSnap, &te32 ) ); 
  
		
	CloseHandle(hThreadSnap);

	return dwTID;

}


//////////////////////////////////////////////////////////////////////////
//
// 현재 코드의 모듈 경로를 반환
//
//////////////////////////////////////////////////////////////////////////
CString CAralProcessAPI::GetCurrentModulePath()
{
	CString strRetVal = _T("");

	// 현재 실행모듈명
	TCHAR szExeFileName[MAX_PATH] = {0,};
	if( ::GetModuleFileName( NULL, szExeFileName, MAX_PATH ) )
	{
		strRetVal = szExeFileName;
	}

	return strRetVal;
}


//////////////////////////////////////////////////////////////////////////
//
// PID로 프로세스 이름 구하기
//
//////////////////////////////////////////////////////////////////////////
CString CAralProcessAPI::GetProcessName(DWORD pid)
{
    CString strProcessName = _T("");

	// 프로세스의 이름을 얻는다
	HANDLE hProcess = ::OpenProcess( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid );
	TRACE( _T("[ aral1 ] PID : %d ->hProcess : 0x%p \n"), pid, hProcess );
	
	if( hProcess )
	{
		DWORD cb = 0;
		for(int i=0; i<10; i++)
		{
			::EnumProcessModules(hProcess, NULL, 0, &cb);
			if(cb) break;
			::Sleep(100);
		}
		
		if(0==cb)
		{
			TRACE( _T("[ aral1 ] EnumProcessModules returned zero size (ErrCode:%d) \n"), ::GetLastError() );
			return strProcessName;
		}
		
		TRACE( _T("[ aral1 ] EnumProcessModules require %d Bytes buffer \n"), cb );
		
		HMODULE* hMo = (HMODULE*)new BYTE[cb];
		::EnumProcessModules(hProcess, hMo, cb, &cb);

		TCHAR buf[MAX_PATH] = {0,};
		if( ::GetModuleFileNameEx(hProcess, hMo[0], buf, MAX_PATH) )
		{
			TCHAR szLongPath[MAX_PATH] = {0,};
			::GetLongPathName( buf, szLongPath, MAX_PATH );
			CString szPath = szLongPath;
			
			int nStrIdx = szPath.ReverseFind( _T('\\') );
			if( nStrIdx >= 0 )
			{
				strProcessName = szPath.Mid( nStrIdx + 1 );
			}
			else
			{
				strProcessName = szLongPath;
			}

			TRACE( _T("[ aral1 ] hProcess:0x%p -> Process Name:'%s' \n"), hProcess, (LPCTSTR)strProcessName );	
		}
		else
		{
			TRACE( _T("[ aral1 ] GetModuleFileNameEx failed (hProcess:0x%p, hModule:0x%p, Err:%d) \n"), hProcess, hMo[0], ::GetLastError() );	
		}

		delete [] (BYTE*)hMo;

		::CloseHandle( hProcess );
	}


	return strProcessName;

	/*
	// 프로세스의 모듈들의 스냅샷을 만듭니다.
	HANDLE hProcessSnap = (HANDLE)-1;
	if(pid == 0) return strProcessName;
	
	CString szPath;

	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
    if(hProcessSnap == INVALID_HANDLE_VALUE)
    {
		TRACE( "[ aral1 ] CreateToolhelp32Snapshot failed (PID:%d, ErrCode:%d) \n", pid, ::GetLastError() );
        return strProcessName;
    }

	TRACE( "[ aral1 ] CreateToolhelp32Snapshot succeed (PID:%d) \n", pid );

    MODULEENTRY32 me32;
	memset(&me32, 0,sizeof(MODULEENTRY32));
	me32.dwSize = sizeof(MODULEENTRY32);

	char szTemp[MAX_PATH];
	memset(szTemp, 0, MAX_PATH);

    // 첫 번째 모듈(exe 파일의 경로)의 경로를 리턴합니다.
    if (Module32First(hProcessSnap, &me32) == TRUE)
    {
		
		TRACE( "[ aral1 ] Module32First succeed (PID:%d) \n", pid );

		szPath = me32.szExePath;
		
		int nCount = szPath.Find("\\", 0);
		if(nCount == 0)
		{
			CString szPathTemp;
			szPath.Delete(nCount);
			nCount = szPath.Find("\\", 0);
			szPathTemp.Format("%%%s%%",szPath.Left(nCount));
			
			ExpandEnvironmentStrings(szPathTemp ,szTemp, MAX_PATH );
			
			if(szPathTemp.Compare(szTemp))
			{
				szPath.Delete(0, nCount);
				szPath = szTemp + szPath;
			}
			else
			{
				szPath.Delete(0, nCount + 1);
			}
		}
		
		char szLongPath[MAX_PATH] = {0,};
		::GetLongPathNameA( szPath, szLongPath, MAX_PATH );
		szPath = szLongPath;
		
		int nStrIdx = szPath.ReverseFind( '\\' );
		if( nStrIdx >= 0 )
		{
			strProcessName = szPath.Mid( nStrIdx + 1 );
		}
		else
		{
			strProcessName = szLongPath;
		}
    }
	else
	{
		TRACE( "[ aral1 ] Module32First failed (PID:%d, ErrCode:%d) \n", pid, ::GetLastError() );
	}

    CloseHandle (hProcessSnap);
	
    return strProcessName;
	*/
	
}


//////////////////////////////////////////////////////////////////////////
//
// PID로 32/64비트 구분하기
//
//////////////////////////////////////////////////////////////////////////
BOOL CAralProcessAPI::Is32bitProcess(DWORD dwPID)
{
    BOOL b32bit = TRUE;
 
	SYSTEM_INFO si = {0,};
	GetSystemInfo(&si);
	if(	(si.wProcessorArchitecture & PROCESSOR_ARCHITECTURE_IA64)
		||(si.wProcessorArchitecture & PROCESSOR_ARCHITECTURE_AMD64) )
	{
		typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);

		HMODULE hMod = GetModuleHandle(_T("kernel32"));
		LPFN_ISWOW64PROCESS fnIsWow64Process = 
			(LPFN_ISWOW64PROCESS)GetProcAddress( hMod, "IsWow64Process" );

		HANDLE hProcess = ::OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, dwPID);

		if( fnIsWow64Process && hProcess )
		{
			fnIsWow64Process(hProcess, &b32bit);
		}
	}


    return b32bit;	
}


HANDLE CAralProcessAPI::LaunchAppIntoDifferentSession(LPCTSTR strAppPath)
{
	PROCESS_INFORMATION pi;
	STARTUPINFO si;
	BOOL bResult = FALSE;
	DWORD dwSessionId,winlogonPid;
	HANDLE hUserToken,hUserTokenDup,hPToken,hProcess;
	DWORD dwCreationFlags;

	// Log the client on to the local computer.

	dwSessionId = WTSGetActiveConsoleSessionId();

	//////////////////////////////////////////
	// Find the winlogon process
	////////////////////////////////////////

	PROCESSENTRY32 procEntry;

	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnap == INVALID_HANDLE_VALUE)
	{
		return NULL;
	}

	procEntry.dwSize = sizeof(PROCESSENTRY32);

	if (!Process32First(hSnap, &procEntry))
	{
		return NULL;
	}

	do
	{
		if (_tcsicmp(procEntry.szExeFile, _T("winlogon.exe")) == 0)
		{
			// We found a winlogon process...
		// make sure it's running in the console session
			DWORD winlogonSessId = 0;
			if (ProcessIdToSessionId(procEntry.th32ProcessID, &winlogonSessId) 
					&& winlogonSessId == dwSessionId)
			{
				winlogonPid = procEntry.th32ProcessID;
				break;
			}
		}

	} while (Process32Next(hSnap, &procEntry));

	////////////////////////////////////////////////////////////////////////

	WTSQueryUserToken(dwSessionId, &hUserToken);
	dwCreationFlags = NORMAL_PRIORITY_CLASS|CREATE_NEW_CONSOLE;
	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb= sizeof(STARTUPINFO);
	si.lpDesktop = _T("winsta0\\default");
	ZeroMemory(&pi, sizeof(pi));
	TOKEN_PRIVILEGES tp;
	LUID luid;
	hProcess = OpenProcess(MAXIMUM_ALLOWED,FALSE,winlogonPid);

	if(!::OpenProcessToken(hProcess,TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY
				 |TOKEN_DUPLICATE|TOKEN_ASSIGN_PRIMARY|TOKEN_ADJUST_SESSIONID
						  |TOKEN_READ|TOKEN_WRITE,&hPToken))
	{
			   int abcd = GetLastError();
			   printf("Process token open Error: %u\n",GetLastError());
	}

	if (!LookupPrivilegeValue(NULL,SE_DEBUG_NAME,&luid))
	{
	   printf("Lookup Privilege value Error: %u\n",GetLastError());
	}
	tp.PrivilegeCount =1;
	tp.Privileges[0].Luid =luid;
	tp.Privileges[0].Attributes =SE_PRIVILEGE_ENABLED;

	DuplicateTokenEx(hPToken,MAXIMUM_ALLOWED,NULL,
			SecurityIdentification,TokenPrimary,&hUserTokenDup);
	int dup = GetLastError();

	//Adjust Token privilege
	SetTokenInformation(hUserTokenDup,
		TokenSessionId,(void*)dwSessionId,sizeof(DWORD));

	if (!AdjustTokenPrivileges(hUserTokenDup,FALSE,&tp,sizeof(TOKEN_PRIVILEGES),
						(PTOKEN_PRIVILEGES)NULL,NULL))
	{
	   int abc =GetLastError();
	   printf("Adjust Privilege value Error: %u\n",GetLastError());
	}

	if (GetLastError()== ERROR_NOT_ALL_ASSIGNED)
	{
	 printf("Token does not have the provilege\n");
	}

	LPVOID pEnv =NULL;

	if(CreateEnvironmentBlock(&pEnv,hUserTokenDup,TRUE))
	{
	   dwCreationFlags|=CREATE_UNICODE_ENVIRONMENT;
	}
	else
	  pEnv=NULL;

	// Launch the process in the client's logon session.

	bResult = CreateProcessAsUser(
	  hUserTokenDup,		   // client's access token
	  NULL,					   // file to execute
	  (LPTSTR)(LPCTSTR)strAppPath,     // command line
	  NULL,            // pointer to process SECURITY_ATTRIBUTES
	  NULL,               // pointer to thread SECURITY_ATTRIBUTES
	  FALSE,              // handles are not inheritable
	  dwCreationFlags,     // creation flags
	  pEnv,               // pointer to new environment block
	  NULL,               // name of current directory
	  &si,               // pointer to STARTUPINFO structure
	  &pi                // receives information about new process
	);
	// End impersonation of client.

	//GetLastError Shud be 0

	int iResultOfCreateProcessAsUser = GetLastError();

	//Perform All the Close Handles tasks

	CloseHandle(hProcess);
	CloseHandle(hUserToken);
	CloseHandle(hUserTokenDup);
	CloseHandle(hPToken);

	return pi.hProcess;
}

BOOL CAralProcessAPI::ImpersonateCurUser()
{
	DWORD dwSessionId,dwExplorerLogonPid;
	HANDLE hProcess,hPToken;
	//TCHAR szUserName[MAX_PATH];
	
	//Get the active desktop session id
	dwSessionId = WTSGetActiveConsoleSessionId();
	
	//We find the explorer process since it will have the user token
	
	//////////////////////////////////////////
	   // Find the explorer process
	////////////////////////////////////////
	
	PROCESSENTRY32 procEntry;
	
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnap == INVALID_HANDLE_VALUE)
	{
		TRACE(_T("[ aral1 ] CreateToolhelp32Snapshot Error: %u \n"), GetLastError());
		return FALSE;
	}
	
	procEntry.dwSize = sizeof(PROCESSENTRY32);
	
	if (!Process32First(hSnap, &procEntry))
	{
		TRACE(_T("[ aral1 ] Process32First Error: %u \n"), GetLastError());
		return FALSE;
	}
	
	do
	{
		if (_tcsicmp(procEntry.szExeFile, _T("explorer.exe")) == 0)
		{
			DWORD dwExplorerSessId = 0;
			if (ProcessIdToSessionId(procEntry.th32ProcessID, &dwExplorerSessId) 
				&& dwExplorerSessId == dwSessionId)
			{
				dwExplorerLogonPid = procEntry.th32ProcessID;
				break;
			}
		}
		
	} while (Process32Next(hSnap, &procEntry));
	
	////////////////////////////////////////////////////////////////////////
	hProcess = OpenProcess(MAXIMUM_ALLOWED,FALSE,dwExplorerLogonPid);
	
	if(!::OpenProcessToken(hProcess,TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY
	   |TOKEN_DUPLICATE|TOKEN_ASSIGN_PRIMARY|TOKEN_ADJUST_SESSIONID
	   |TOKEN_READ|TOKEN_WRITE,&hPToken))
	{
	   int abcd = GetLastError();
	   TRACE(_T("[ aral1 ] Process token open Error: %u \n"), GetLastError());
	}

	return ImpersonateLoggedOnUser(hPToken);
}


BOOL CAralProcessAPI::LaunchAppAsUser(LPCTSTR strAppPath)
{	
	DWORD dwSessionId,dwExplorerLogonPid;
	HANDLE hProcess,hPToken;
	//TCHAR szUserName[MAX_PATH];
	
	//Get the active desktop session id
	dwSessionId = WTSGetActiveConsoleSessionId();
	
	//We find the explorer process since it will have the user token
	
	//////////////////////////////////////////
	   // Find the explorer process
	////////////////////////////////////////
	
	PROCESSENTRY32 procEntry;
	
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnap == INVALID_HANDLE_VALUE)
	{
		TRACE(_T("[ aral1 ] CreateToolhelp32Snapshot Error: %u \n"), GetLastError());
		return FALSE;
	}
	
	procEntry.dwSize = sizeof(PROCESSENTRY32);
	
	if (!Process32First(hSnap, &procEntry))
	{
		TRACE(_T("[ aral1 ] Process32First Error: %u \n"), GetLastError());
		return FALSE;
	}
	
	do
	{
		if (_tcsicmp(procEntry.szExeFile, _T("explorer.exe")) == 0)
		{
			DWORD dwExplorerSessId = 0;
			if (ProcessIdToSessionId(procEntry.th32ProcessID, &dwExplorerSessId) 
				&& dwExplorerSessId == dwSessionId)
			{
				dwExplorerLogonPid = procEntry.th32ProcessID;
				break;
			}
		}
		
	} while (Process32Next(hSnap, &procEntry));
	
	////////////////////////////////////////////////////////////////////////
	hProcess = OpenProcess(MAXIMUM_ALLOWED,FALSE,dwExplorerLogonPid);
	
	if(!::OpenProcessToken(hProcess,TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY
		|TOKEN_DUPLICATE|TOKEN_ASSIGN_PRIMARY|TOKEN_ADJUST_SESSIONID
		|TOKEN_READ|TOKEN_WRITE,&hPToken))
	{
		int abcd = GetLastError();
		TRACE(_T("[ aral1 ] Process token open Error: %u \n"), GetLastError());
	}
	
	// We need to impersonate the service token to run as a user to access the Registry. This will cause our worker thread to run in the users token's context. 
	//Impersonate the explorer token which runs under the user account
	BOOL bRetVal = ImpersonateLoggedOnUser(hPToken);

	int iImpersonateResult = GetLastError();

	//if(iImpersonateResult == ERROR_SUCCESS)
	if(bRetVal)
	{
		/*		  
		//GetUserName will now return the username

		GetUserName(szUserName,&dwSize);

		//Since the thread is running as the user we can access the HKCU now
		dwRegDataSize = sizeof(szRegData);
		lRegResult = RegOpenKeyEx(HKEY_CURRENT_USER,
			szRegPath,0,KEY_QUERY_VALUE,&hKey);
		if (lRegResult == ERROR_SUCCESS)
		RegQueryValueEx(hKey,_T("SideBar"),NULL,NULL,(LPBYTE)&szRegData,&dwRegDataSize);
		*/
		
		STARTUPINFO si = {0,};
		PROCESS_INFORMATION pi = {0,};

		bRetVal = CreateProcessAsUser(
		  hPToken,
		  NULL,					   // file to execute
		  (LPTSTR)(LPCTSTR)strAppPath,     // command line
		  NULL,            // pointer to process SECURITY_ATTRIBUTES
		  NULL,               // pointer to thread SECURITY_ATTRIBUTES
		  FALSE,              // handles are not inheritable
		  0,				   // creation flags
		  NULL,               // pointer to new environment block
		  NULL,               // name of current directory
		  &si,               // pointer to STARTUPINFO structure
		  &pi                // receives information about new process
	   );

	}
	else
	{
		TRACE(_T("[ aral1 ] iImpersonateResult Error: %u \n"), iImpersonateResult);
	}

	//Once the operation is over revert back to system account.
	RevertToSelf();

	return bRetVal;
}

BOOL CAralProcessAPI::ExecuteAndWait(LPCTSTR cszCmdLine, int* pExitCode)
{
	STARTUPINFO si = {0,};
	PROCESS_INFORMATION pi = {0,};

	BOOL bRetVal = CreateProcess(
	  NULL,					// file to execute
	  (LPTSTR)cszCmdLine,	// command line
	  NULL,					// pointer to process SECURITY_ATTRIBUTES
	  NULL,					// pointer to thread SECURITY_ATTRIBUTES
	  FALSE,				// handles are not inheritable
	  0,					// creation flags
	  NULL,					// pointer to new environment block
	  NULL,					// name of current directory
	  &si,					// pointer to STARTUPINFO structure
	  &pi					// receives information about new process
	);

	// 프로세스 실행 성공시
	if(bRetVal)
	{
		if( WaitForSingleObject( pi.hProcess, INFINITE ) == WAIT_OBJECT_0 )
		{
			if(pExitCode)
			{
				bRetVal = GetExitCodeProcess( pi.hProcess, (DWORD*)pExitCode );
			}
		}
		else
		{
			bRetVal = FALSE;
		}
	}

	return bRetVal;
	
}
