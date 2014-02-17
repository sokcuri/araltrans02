// RawHookMgr.cpp: implementation of the CRawHookMgr class.
//
//////////////////////////////////////////////////////////////////////
#include "RawHookMgr.h"
#include "RawHookedModule.h"
#include <tlhelp32.h>

#include "../Debug.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CModuleList CRawHookMgr::m_listModules;
BOOL		CRawHookMgr::m_bIsAllThreadSuspended = FALSE;
HMODULE		CRawHookMgr::m_hExecModule = NULL;

CRawHookMgr::CRawHookMgr()
{

}

CRawHookMgr::~CRawHookMgr()
{

}



LPVOID CRawHookMgr::GetFixedFuncAddr( LPVOID pFunc )
{
	LPVOID pRetFunc = NULL;
	
	// 함수가 들어있는 모듈을 알아낸다
	HMODULE hModule = HandleFromAddress(pFunc);

	// 해당 주소가 있으면 그대로 돌려줌
	if(hModule)
	{
		pRetFunc = pFunc;
	}
	// 해당 주소가 없으면 실행 모듈로 간주
	else
	{
		hModule = GetExecModuleHandle();

		if(hModule && (UINT_PTR)hModule < (UINT_PTR)pFunc )
		{
			MODULEINFO mi;
			UINT_PTR dist = (UINT_PTR)pFunc - (UINT_PTR)hModule;
			BOOL bRes = ::GetModuleInformation( ::GetCurrentProcess(), hModule, &mi, sizeof(MODULEINFO) );
			if( TRUE == bRes && (DWORD)dist < mi.SizeOfImage)
			{
				pRetFunc = (LPVOID)((UINT_PTR)hModule + dist);
			}
		}
	}

	return pRetFunc;
}


//////////////////////////////////////////////////////////////////////////
//
// 특정 함수 후킹
//
//////////////////////////////////////////////////////////////////////////
BOOL CRawHookMgr::Hook(LPVOID pfnOld, LPVOID pfnNew, BOOL bWriteCallCode, INT nHookType)
{
	NOTIFY_DEBUG_MESSAGE(_T("CRawHookMgr:Hook: called, pfnOld=%p, pfnNew=%p, bWrite=%s\n"), pfnOld, pfnNew, (bWriteCallCode?_T("TRUE"):_T("FALSE")));

	pfnOld = GetFixedFuncAddr(pfnOld);
	if(NULL == pfnOld) return FALSE;

	// 함수가 들어있는 모듈을 알아낸다
	HMODULE hModule = HandleFromAddress(pfnOld);
	if(NULL == hModule) return FALSE;

	// 현재 관리되고 있는 모듈들 중 이 모듈이 있는 찾아보고
	CRawHookedModule* pModule = FindModule(hModule);

	// 없으면 새 모듈 등록
	if( NULL == pModule )
	{
		pModule = new CRawHookedModule();
		if( pModule->Init( hModule ) == FALSE )
		{
			delete pModule;
			return FALSE;
		}
		m_listModules.push_back(pModule);
	}

	// 모듈객체에 이 함수를 후킹하라 지시
	if(nHookType == ATHOOKTYPE_SOURCE) return pModule->Hook2(pfnOld, pfnNew, bWriteCallCode);
	else return pModule->Hook(pfnOld, pfnNew, bWriteCallCode);
}

//////////////////////////////////////////////////////////////////////////
//
// 특정 바이너리 패치
//
//////////////////////////////////////////////////////////////////////////
BOOL CRawHookMgr::preHook(LPVOID pfnOld, unsigned char* cBuff, size_t copysize)
{
	pfnOld = GetFixedFuncAddr(pfnOld);
	if(NULL == pfnOld) return FALSE;

	// 함수가 들어있는 모듈을 알아낸다
	HMODULE hModule = HandleFromAddress(pfnOld);
	if(NULL == hModule) return FALSE;

	// 현재 관리되고 있는 모듈들 중 이 모듈이 있는 찾아보고
	CRawHookedModule* pModule = FindModule(hModule);

	// 없으면 새 모듈 등록
	if( NULL == pModule )
	{
		pModule = new CRawHookedModule();
		if( pModule->Init( hModule ) == FALSE )
		{
			delete pModule;
			return FALSE;
		}
		m_listModules.push_back(pModule);
	}

	return pModule->preHook(pfnOld, cBuff, copysize);
}

BOOL CRawHookMgr::Hook(LPCTSTR cszDllName, LPCTSTR cszExportName, LPVOID pfnNew, INT nHookType)
{
	// 있건 없건 일단 강제로 라이브러리를 로드한다
	::LoadLibrary( cszDllName );
	
	// 모듈핸들 얻기
	HMODULE hModule = ::GetModuleHandle( cszDllName );
	if(NULL==hModule) return FALSE;
	
#ifdef _UNICODE
	char szFuncName[MAX_PATH];
	::WideCharToMultiByte(CP_ACP, 0, cszExportName, -1, szFuncName, MAX_PATH, NULL, NULL);
#else
	LPCSTR szFuncName = cszExportName;
#endif

	FARPROC pOrigFunc = ::GetProcAddress( hModule, szFuncName );
	if(NULL==pOrigFunc) return FALSE;

	return CRawHookMgr::Hook(pOrigFunc, pfnNew, FALSE, nHookType);
}


///////////////////////////////////////////////////////////////////////////
//
// 특정 함수 언훅
//
//////////////////////////////////////////////////////////////////////////
BOOL CRawHookMgr::Unhook(LPVOID pFunc)
{
	BOOL bRetVal = FALSE;	

	pFunc = GetFixedFuncAddr(pFunc);
	if(NULL == pFunc) return FALSE;

	// 현재 관리되고 있는 모듈들을 순회하며 Unhook 시도
	CModuleList::iterator iter = m_listModules.begin();
	for( ; iter!=m_listModules.end(); iter++ )
	{
		CRawHookedModule* pTmpMod = (*iter);
		
		// 언훅 시도
		if( pTmpMod->Unhook(pFunc) )
		{
			// 더이상 후킹하고 있는 함수가 없으면
			// -> No! 후킹하고 있지 않더라도 실행되어야 할 수 있으므로 안전을 위해 남겨둔다.
			if( pTmpMod->m_listFuncs.empty() )
			{				
				// 이 모듈을 삭제
				pTmpMod->Close();
				delete pTmpMod;
				m_listModules.erase(iter);
			}

			bRetVal = TRUE;
			break;
		}

	}

	return bRetVal;
}

///////////////////////////////////////////////////////////////////////////
//
// 지금까지 훅 되 있던 함수 전부 해제
//
//////////////////////////////////////////////////////////////////////////
BOOL CRawHookMgr::Unhook()
{
	// 현재 관리되고 있는 모듈들 전부 Unhook시키고 삭제
	CModuleList::iterator iter = m_listModules.begin();
	for( ; iter!=m_listModules.end(); iter++ )
	{
		CRawHookedModule* pTmpMod = (*iter);
		pTmpMod->Close();
		delete pTmpMod;
	}
	m_listModules.clear();
	
	return TRUE;
}


//////////////////////////////////////////////////////////////////////////
//
// 특정 함수의 원래 코드를 반환
//
//////////////////////////////////////////////////////////////////////////
LPVOID CRawHookMgr::GetOrigFunc(LPVOID pFunc)
{
	LPVOID pfnOrig = NULL;

	pFunc = GetFixedFuncAddr(pFunc);
	if(NULL == pFunc) return NULL;

	CModuleList::iterator iter = m_listModules.begin();
	for( ; iter!=m_listModules.end(); iter++ )
	{
		CRawHookedModule* pTmpMod = (*iter);
		pfnOrig = pTmpMod->GetOrigFunc( pFunc );
		if( pfnOrig ) break;
	}

	if(NULL == pfnOrig) pfnOrig = pFunc;	// 그새 unhook되었으면 원래함수 주소 return
	
	return pfnOrig;
}




//////////////////////////////////////////////////////////////////////////
//
// 특정 주소에 사상되어 있는 모듈 핸들을 반환
//
//////////////////////////////////////////////////////////////////////////
HMODULE CRawHookMgr::HandleFromAddress(LPVOID pAddr)
{
	/*
	MEMORY_BASIC_INFORMATION mbi;

	return ((::VirtualQuery(pAddr, &mbi, sizeof(mbi)) != 0) 
	        ? (HMODULE) mbi.AllocationBase : NULL);
	*/

	HMODULE hOwnerMod = NULL;

	::GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCTSTR)pAddr, &hOwnerMod);

	return hOwnerMod;
}



///////////////////////////////////////////////////////////////////////////
//
// 핸들값으로 관리 중인 모듈객체 찾아냄
//
//////////////////////////////////////////////////////////////////////////
CRawHookedModule* CRawHookMgr::FindModule(HMODULE hModule)
{
	CRawHookedModule* pModule = NULL;

	CModuleList::iterator iter = m_listModules.begin();
	for( ; iter!=m_listModules.end(); iter++ )
	{
		CRawHookedModule* pTmpMod = (*iter);
		if( pTmpMod->GetModule() == hModule )
		{
			pModule = pTmpMod;
			break;
		}
	}

	return pModule;
}

///////////////////////////////////////////////////////////////////////////
//
// 이 포인터가 훅된 모듈에 있는가 반환
//
//////////////////////////////////////////////////////////////////////////
BOOL CRawHookMgr::IsInHookedModule(LPVOID pCode)
{
	pCode = GetFixedFuncAddr(pCode);
	if(NULL == pCode) return FALSE;

	CModuleList::iterator iter = m_listModules.begin();
	for( ; iter!=m_listModules.end(); iter++ )
	{
		CRawHookedModule* pTmpMod = (*iter);
		if( pTmpMod->IsInHookedModule( pCode ) ) return TRUE;
	}

	return FALSE;	
}


///////////////////////////////////////////////////////////////////////////
//
// 이 포인터가 훅된 모듈에 있는가 반환
//
//////////////////////////////////////////////////////////////////////////
BOOL CRawHookMgr::RemoveModuleProtection(HMODULE hModule)
{
	MODULEINFO ModuleInfo;
	
	// 이 모듈의 정보 구하기
	BOOL bRes = ::GetModuleInformation( ::GetCurrentProcess(), hModule, &ModuleInfo, sizeof(MODULEINFO) );
	if( FALSE==bRes )
	{
		DWORD nErr = GetLastError();
		return FALSE;
	}

	SIZE_T curOffset = 0;

	while(curOffset<ModuleInfo.SizeOfImage)	
	{

		// 페이지 정보 쿼리
		MEMORY_BASIC_INFORMATION mbi = {0,};
		SIZE_T retVal = ::VirtualQuery((LPBYTE)ModuleInfo.lpBaseOfDll + curOffset, &mbi, sizeof(MEMORY_BASIC_INFORMATION));

		// 페이지면 변경
		if( mbi.Protect )
		{
			// 페이지의 보호속성 변경
			DWORD dwOldProtect;
			bRes = ::VirtualProtect(mbi.BaseAddress, mbi.RegionSize, PAGE_EXECUTE_READWRITE, &dwOldProtect);

		}

		curOffset += mbi.RegionSize;
	}

	return TRUE;
}


//////////////////////////////////////////////////////////////////////////
//
// 모든 쓰레드를 정지시킨다 (현재 쓰래드만 뺴고)
//
//////////////////////////////////////////////////////////////////////////
void CRawHookMgr::SuspendAllThread()
{
	// 모든 쓰레드들의 스냅샷을 만듭니다.
	HANDLE hThreadSnap = INVALID_HANDLE_VALUE;
	hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, GetCurrentProcessId());

	if(hThreadSnap == INVALID_HANDLE_VALUE) return;


	THREADENTRY32 te32;
	memset(&te32, 0,sizeof(THREADENTRY32));
	te32.dwSize = sizeof(THREADENTRY32);

	// 현재 프로세스ID와 쓰레드ID
	DWORD dwCurPID = GetCurrentProcessId();
	DWORD dwCurTID = GetCurrentThreadId();

	// 쓰레드 순회
	BOOL bNext = Thread32First(hThreadSnap, &te32);
	while(bNext)
	{
		if( te32.th32OwnerProcessID == dwCurPID && te32.th32ThreadID != dwCurTID )
		{
			// 쓰레드 정지
			HANDLE hThread = OpenThread(THREAD_SUSPEND_RESUME, FALSE, te32.th32ThreadID);
			if(hThread)
			{
				SuspendThread(hThread);
				//CloseHandle(hThread);
			}
		}

		bNext = Thread32Next(hThreadSnap, &te32);
	}
	CloseHandle (hThreadSnap);

	m_bIsAllThreadSuspended = TRUE;
}



//////////////////////////////////////////////////////////////////////////
//
// 모든 쓰레드를 가동시킨다
//
//////////////////////////////////////////////////////////////////////////
void CRawHookMgr::ResumeAllThread()
{
	// 모든 쓰레드들의 스냅샷을 만듭니다.
	HANDLE hThreadSnap = INVALID_HANDLE_VALUE;
	hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, GetCurrentProcessId());

	if(hThreadSnap == INVALID_HANDLE_VALUE) return;


	THREADENTRY32 te32;
	memset(&te32, 0,sizeof(THREADENTRY32));
	te32.dwSize = sizeof(THREADENTRY32);

	// 현재 프로세스ID와 쓰레드ID
	DWORD dwCurPID = GetCurrentProcessId();
	DWORD dwCurTID = GetCurrentThreadId();

	// 쓰레드 순회
	BOOL bNext = Thread32First(hThreadSnap, &te32);
	while(bNext)
	{
		if( te32.th32OwnerProcessID == dwCurPID && te32.th32ThreadID != dwCurTID )
		{
			// 쓰레드 정지
			HANDLE hThread = OpenThread(THREAD_SUSPEND_RESUME, FALSE, te32.th32ThreadID);
			if(hThread)
			{
				ResumeThread(hThread);
				//CloseHandle(hThread);
			}
		}

		bNext = Thread32Next(hThreadSnap, &te32);
	}
	CloseHandle (hThreadSnap);

	m_bIsAllThreadSuspended = FALSE;
}

BOOL CRawHookMgr::IsAllThreadSuspended()
{
	return m_bIsAllThreadSuspended;
}

