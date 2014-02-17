// RawHookedModule.cpp: implementation of the CRawHookedModule class.
//
//////////////////////////////////////////////////////////////////////

#pragma warning(disable:4996)
#pragma comment (lib, "psapi.lib")
#include "RawHookedModule.h"
#include "RawHookMgr.h"

#include "../Debug.h"

//#ifdef USE_HOOKINGMODE_2

#include "disasm.h"

disasm g_disasm;
void* ptrEmptyCodeArea = NULL;
//#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRawHookedModule::CRawHookedModule()
: m_hModule(NULL), m_pOrigImg(NULL)		//, m_listFuncs(NULL)
{
	memset(m_szModuleName, 0, MAX_PATH);
	memset(&m_ModuleInfo, 0, sizeof(MODULEINFO));
}

CRawHookedModule::~CRawHookedModule()
{
	Close();
}

void *CRawHookedModule::HandledCopyMemory(void * dest, const void *src, size_t count)
{
	void *pRet=NULL;

	// memcpy() 는 C 라이브러리 함수라서 C++ 에서 사용하는 try-catch 로는 잡을 수가 없다고 한다.. oTL
	// 단 OS 쪽 에러 핸들링은 가능하며 이 경우 __try-__except 로밖에 잡을 수 없다.
	//
	// NOTE: Debug 로 빌드 시 종료할 때 디버그 라이브러리에서 HEAP Corruption 에러가 나오지만 
	//       Release 에서는 문제가 없(을 것임.. 아마도. -_-)
	__try
	{
		pRet = memcpy(dest, src, count);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		pRet = NULL;
		NOTIFY_DEBUG_MESSAGE(_T("CRawHookedModule:HandledCopyMemory error\n"));
	}
	return pRet;

}

//////////////////////////////////////////////////////////////////////////
//
// 주어진 모듈을 훅 할 준비
//
//////////////////////////////////////////////////////////////////////////
BOOL CRawHookedModule::Init(HMODULE hModule)
{
	Close();

	NOTIFY_DEBUG_MESSAGE(_T("CRawHookedModule:Init: called, hModule=%p\n"), hModule);

	BOOL bRes = FALSE;
	BOOL bNeedToResumeAllThread = FALSE;

	try
	{
		// 모듈 이름 구하기
		TCHAR szModuleName[MAX_PATH] = {0,};
		::GetModuleFileName(hModule, szModuleName, MAX_PATH);
		TCHAR* pNameStart = _tcsrchr(szModuleName, _T('\\'));
		if(pNameStart) _tcscpy(m_szModuleName, pNameStart+1);
		else _tcscpy(m_szModuleName, szModuleName);

		// 이 모듈의 정보 구하기
		bRes = ::GetModuleInformation( ::GetCurrentProcess(), hModule, &m_ModuleInfo, sizeof(MODULEINFO) );
		if( FALSE==bRes )
		{
			throw (GetLastError());
			//DWORD nErr = GetLastError();
			//goto ModuleInitErr;
		}

		SIZE_T curOffset = 0;

		// 메모리를 할당하여 모듈 이미지 전체를 복사해 둔다
		m_pOrigImg = new BYTE[m_ModuleInfo.SizeOfImage];
		memset(m_pOrigImg, 0, m_ModuleInfo.SizeOfImage);

		// 모든 쓰레드 정지했는지 확인
		//if(CRawHookMgr::IsAllThreadSuspended() == FALSE)
		//{
		//	CRawHookMgr::SuspendAllThread();
		//	bNeedToResumeAllThread = TRUE;
		//}

		// 모듈 복사
		while(curOffset<m_ModuleInfo.SizeOfImage)	
		{

			// 페이지 정보 쿼리
			MEMORY_BASIC_INFORMATION mbi = {0,};
			SIZE_T retVal = ::VirtualQuery((LPBYTE)m_ModuleInfo.lpBaseOfDll + curOffset, &mbi, sizeof(MEMORY_BASIC_INFORMATION));

			NOTIFY_DEBUG_MESSAGE(_T("CRawHookedModule:Init: VirtualQuery: AllocBase=%p, AllocProt=%p, Base=%p, Prot=%p, RegSize=%p, State=%p, Type=%p\n"), 
				mbi.AllocationBase, mbi.AllocationProtect, mbi.BaseAddress, mbi.Protect, mbi.RegionSize, mbi.State, mbi.Type);
			
			//mbi.State : MEM_FREE(0x10000), MEM_RESERVE(0x2000), MEM_COMMIT(0x1000)
			//mbi.Type : MEM_IMAGE(0x1000000), MEM_MAPPED(0x40000), MEM_PRIVATE(0x20000)
			
			HMODULE hExecModule = CRawHookMgr::GetExecModuleHandle();
			// Protect 가 접근가능하면 복사
			if( mbi.Protect )
			{
				// 참조 에러면 나가자
				if(mbi.AllocationBase != m_ModuleInfo.lpBaseOfDll) //if((mbi.State == MEM_FREE) || !mbi.Type)
					break;

				// 페이지의 보호속성 변경
				DWORD dwOldProtect;
				bRes = ::VirtualProtect(mbi.BaseAddress, mbi.RegionSize, PAGE_EXECUTE_READWRITE, &dwOldProtect);

				NOTIFY_DEBUG_MESSAGE(_T("CRawHookedModule:Init: VirtualProtect: bRes=%s\n"), (bRes?_T("TRUE"):_T("FALSE")));

				if (bRes == FALSE)
				{
					DWORD dwErr = GetLastError();

					wchar_t buf[500] = {0,};
					wsprintf(buf,_T("%s : %p + %p (%p)\nVirtualQuery\nAllocBase=%p, AllocProtect=%p\n*BaseAddress=%p, Protect=%p\n*RegSize=%p, State=%p, Type=%p\nErrorNumber=%d, SizeOfImage=%p, curOffset=%p\n※후킹문제가 발생하였습니다.\n　게임진행에 문제가 있다면 해당창을 캡쳐하여 보고해주시기 바랍니다."), 
						m_szModuleName, m_ModuleInfo.lpBaseOfDll, curOffset, retVal,
						mbi.AllocationBase, mbi.AllocationProtect, mbi.BaseAddress, mbi.Protect, mbi.RegionSize, mbi.State, mbi.Type,
						dwErr, m_ModuleInfo.SizeOfImage, curOffset);
					
					MessageBox(NULL,buf,NULL,0);

					if (dwErr == ERROR_INVALID_ADDRESS)
					{
						// 이 말은, m_ModuleInfo.SizeOfImage가 부정확하다는 뜻임
						// 아마도 복사방지 혹은 실행파일 압축기 등이 이런 문제를 일으키는 듯.
						// 차라리 m_ModuleInfo.SizeOfImage 를 믿지 말고 그냥 여기서 빠져나갈까..
						m_ModuleInfo.SizeOfImage = curOffset;
						break;
					}
					/*else if(dwErr == ERROR_INVALID_PARAMETER)
					{
					}*/
					else
					{
						throw dwErr;
					}

				}

				NOTIFY_DEBUG_MESSAGE(_T("CRawHookedModule:Init: HandledCopyMemory(%p, %p, %p)\n"), m_pOrigImg + curOffset, mbi.BaseAddress, mbi.RegionSize);

				// 복사
				if (HandledCopyMemory(m_pOrigImg + curOffset, mbi.BaseAddress, mbi.RegionSize) == NULL)
				{
					// 복사 에러
					// 어떤 복사방지 혹은 실행파일 압축기 등은 VirtualProtect 에서 Pass 되는데도
					// 막상 memcpy() 를 해보면 AccessViolation 에러가 나는 경우가 있는 듯.
					// 역시 m_ModuleInfo.SizeOfImage가 부정확하다는 뜻임
					m_ModuleInfo.SizeOfImage = curOffset;
					break;
				}

				NOTIFY_DEBUG_MESSAGE(_T("CRawHookedModule:Init: HandledCopyMemory OK\n"));

				// 모듈의 보호속성 복구
				bRes = ::VirtualProtect(mbi.BaseAddress, mbi.RegionSize, mbi.Protect, &dwOldProtect);

				// 복사된 사본의 보호속성 변경
				bRes = ::VirtualProtect(m_pOrigImg + curOffset, mbi.RegionSize, PAGE_EXECUTE_READWRITE, &dwOldProtect);

			}
			curOffset += mbi.RegionSize;
		}

		// 모든 쓰레드 복구
		//if(bNeedToResumeAllThread) CRawHookMgr::ResumeAllThread();

		m_hModule = hModule;
	}
	catch (DWORD dwErr)
	{
//		TRACE(_T("CRawHookedModule::Init() error\n"));
//		TRACE(_T("   GetLastError= %d\n"), dwErr);
		dwErr=dwErr;

		NOTIFY_DEBUG_MESSAGE(_T("CRawHookedModule:Init: Error %08X\n"), dwErr);

		if(m_pOrigImg)
		{
			delete [] m_pOrigImg;
			m_pOrigImg = NULL;
		}
		m_hModule = NULL;
		memset(m_szModuleName, 0, MAX_PATH);
		memset(&m_ModuleInfo, 0, sizeof(MODULEINFO));
		return FALSE;
	}

	return TRUE;
/*
ModuleInitErr:
	if(m_pOrigImg)
	{
		delete [] m_pOrigImg;
		m_pOrigImg = NULL;
	}
	m_hModule = NULL;
	memset(m_szModuleName, 0, MAX_PATH);
	memset(&m_ModuleInfo, 0, sizeof(MODULEINFO));
	return FALSE;
*/
}


//////////////////////////////////////////////////////////////////////////
//
// 이 모듈의 훅을 해제한다
//
//////////////////////////////////////////////////////////////////////////
void CRawHookedModule::Close()
{
	if( m_hModule && m_pOrigImg )
	{
		// 훅 되어 있는 함수들을 복구시킨다
		CFunctionList::iterator iter = m_listFuncs.begin();
		for( ; iter!=m_listFuncs.end(); iter++ )
		{
			HOOKED_FUNC_INFO* pHook = (*iter);
			Unhook( pHook );
			delete pHook;
		}
		
		// 원본 이미지 삭제
		delete [] m_pOrigImg;

	}

	m_pOrigImg = NULL;
	m_hModule = NULL;
	// 펑션 리스트 클리어
	m_listFuncs.clear();
}

//#ifndef USE_HOOKINGMODE_2
//////////////////////////////////////////////////////////////////////////
//
// 이 모듈의 특정 함수 주소를 바꿔놓는다 (후킹모드1)
//
//////////////////////////////////////////////////////////////////////////
BOOL CRawHookedModule::Hook(LPVOID pFuncAddr, LPVOID pRedirAddr, BOOL bWriteCallCode)
{
	BOOL bRetVal = FALSE;
	MEMORY_BASIC_INFORMATION mbi, mbi2;
	ZeroMemory(&mbi, sizeof(MEMORY_BASIC_INFORMATION));
	ZeroMemory(&mbi2, sizeof(MEMORY_BASIC_INFORMATION));
	
	// 모든 쓰레드 정지했는지 확인
	BOOL bNeedToResumeAllThread = FALSE;
	if(CRawHookMgr::IsAllThreadSuspended() == FALSE)
	{
		CRawHookMgr::SuspendAllThread();
		bNeedToResumeAllThread = TRUE;
	}
	
	try
	{
		// 함수가 이 모듈껀지 검사
		if( (UINT_PTR)pFuncAddr < (UINT_PTR)m_ModuleInfo.lpBaseOfDll 
			|| (UINT_PTR)pFuncAddr > (UINT_PTR)m_ModuleInfo.lpBaseOfDll+(UINT_PTR)m_ModuleInfo.SizeOfImage) throw -1;

		// 함수가 존재하는 메모리 페이지 정보 쿼리
		::VirtualQuery(pFuncAddr, &mbi, sizeof(MEMORY_BASIC_INFORMATION));

		// 프로텍션을 READWRITE로
		if (FALSE == ::VirtualProtect(
			mbi.BaseAddress,
			mbi.RegionSize,
			PAGE_EXECUTE_READWRITE,
			&mbi.Protect)
			) throw -2;


		size_t nDistFromBase = (size_t)pFuncAddr - (size_t)m_hModule;
		LPBYTE pOrigCode = m_pOrigImg + nDistFromBase;
		size_t copysize = 5;

		// 첫 코드를 점프 또는 호출로 바꿔준다
		BYTE opcode;
		if( bWriteCallCode )
		{
			opcode = 0xE8;		// 32bit relative call
		}
		else
		{
			opcode = 0xE9;		// 32bit relative jmp
		}

		_asm
		{
			mov ebx, pFuncAddr;

			mov al, opcode;			// opcode
			mov byte ptr [ebx], al;

			mov eax, pRedirAddr;	// 새 함수의 주소에서
			sub eax, ebx;			// 이전 함수 주소를 빼고
			sub eax, 5;				// 5를 빼면 거리가 나옴

			mov dword ptr [ebx+1], eax;
		}

		// 훅 객체를 생성하여 리스트에 추가
		HOOKED_FUNC_INFO* pFuncObj	= new HOOKED_FUNC_INFO;
		pFuncObj->m_pFunction		= pFuncAddr;
		pFuncObj->m_pRedirection	= pRedirAddr;
		pFuncObj->m_nDistFromBase	= nDistFromBase;
		pFuncObj->m_nModifiedSize	= copysize;
		pFuncObj->m_pOrigCode		= pOrigCode;
		pFuncObj->m_bHookSet		= FALSE;
		m_listFuncs.push_back( pFuncObj );

		bRetVal = TRUE;
	}
	catch (int nErrCode)
	{
		nErrCode=nErrCode;
	}

	DWORD dwOldProtect;

	// 프로텍션을 복구
    if(mbi.BaseAddress)
		::VirtualProtect(
			mbi.BaseAddress,
			mbi.RegionSize,
			mbi.Protect,
			&dwOldProtect
			);

	if(mbi2.BaseAddress)
		::VirtualProtect(
			mbi2.BaseAddress,
			mbi2.RegionSize,
			mbi2.Protect,
			&dwOldProtect
			);

	// 모든 쓰레드 복구
	if(bNeedToResumeAllThread) 
		CRawHookMgr::ResumeAllThread();


	return bRetVal;
}


/*
//////////////////////////////////////////////////////////////////////////
//
// 언훅
//
//////////////////////////////////////////////////////////////////////////
BOOL CRawHookedModule::Unhook(HOOKED_FUNC_INFO* pHookInfo)
{	
	if(NULL==pHookInfo) return FALSE;

	BOOL bRetVal = FALSE;
	MEMORY_BASIC_INFORMATION mbi, mbi2;
	ZeroMemory(&mbi, sizeof(MEMORY_BASIC_INFORMATION));
	ZeroMemory(&mbi2, sizeof(MEMORY_BASIC_INFORMATION));


	// 모든 쓰레드 정지했는지 확인
	BOOL bNeedToResumeAllThread = FALSE;
	if(CRawHookMgr::IsAllThreadSuspended() == FALSE)
	{
		CRawHookMgr::SuspendAllThread();
		bNeedToResumeAllThread = TRUE;
	}

	try
	{
		MEMORY_BASIC_INFORMATION mbi;
		::VirtualQuery(pHookInfo->m_pFunction, &mbi, sizeof(MEMORY_BASIC_INFORMATION));

		// 프로텍션을 READWRITE로
		DWORD dwOldProtect;
		if (FALSE == ::VirtualProtect(
			mbi.BaseAddress,
			mbi.RegionSize,
			PAGE_EXECUTE_READWRITE,
			&dwOldProtect)
			) throw -2;


		memcpy(pHookInfo->m_pFunction, m_pOrigImg+pHookInfo->m_nDistFromBase, 5);
		
		// 훅 리스트에서 삭제
		CFunctionList::iterator iter = m_listFuncs.begin();
		for( ; iter!=m_listFuncs.end(); iter++ )
		{
			if((*iter) == pHookInfo)
			{
				delete pHookInfo;
				m_listFuncs.erase(iter);
				bRetVal = TRUE;
				break;
			}
		}

	}
	catch (int nErrCode)
	{
		nErrCode=nErrCode;
	}

	DWORD dwOldProtect;

	// 프로텍션을 복구
	if(mbi.BaseAddress)
		::VirtualProtect(
		mbi.BaseAddress,
		mbi.RegionSize,
		mbi.Protect,
		&dwOldProtect
		);

	if(mbi2.BaseAddress)
		::VirtualProtect(
		mbi2.BaseAddress,
		mbi2.RegionSize,
		mbi2.Protect,
		&dwOldProtect
		);

	// 모든 쓰레드 복구
	if(bNeedToResumeAllThread) CRawHookMgr::ResumeAllThread();

	return bRetVal;
}*/
//#else // USE_HOOKINGMODE_2
//////////////////////////////////////////////////////////////////////////
//
// 이 모듈의 특정 함수 주소를 바꿔놓는다 (후킹모드2)
//
//////////////////////////////////////////////////////////////////////////
BOOL CRawHookedModule::Hook2(LPVOID pFuncAddr, LPVOID pRedirAddr, BOOL bWriteCallCode)
{
	BOOL bRetVal = FALSE;
	MEMORY_BASIC_INFORMATION mbi, mbi2;
	ZeroMemory(&mbi, sizeof(MEMORY_BASIC_INFORMATION));
	ZeroMemory(&mbi2, sizeof(MEMORY_BASIC_INFORMATION));
	
	// 모든 쓰레드 정지했는지 확인
	BOOL bNeedToResumeAllThread = FALSE;
	if(CRawHookMgr::IsAllThreadSuspended() == FALSE)
	{
		CRawHookMgr::SuspendAllThread();
		bNeedToResumeAllThread = TRUE;
	}
	
	try
	{
		// 함수가 이 모듈껀지 검사
		if( (UINT_PTR)pFuncAddr < (UINT_PTR)m_ModuleInfo.lpBaseOfDll 
			|| (UINT_PTR)pFuncAddr > (UINT_PTR)m_ModuleInfo.lpBaseOfDll+(UINT_PTR)m_ModuleInfo.SizeOfImage) throw -1;

		// 함수가 존재하는 메모리 페이지 정보 쿼리
		::VirtualQuery(pFuncAddr, &mbi, sizeof(MEMORY_BASIC_INFORMATION));

		// 프로텍션을 READWRITE로
		if (FALSE == ::VirtualProtect(
			mbi.BaseAddress,
			mbi.RegionSize,
			PAGE_EXECUTE_READWRITE,
			&mbi.Protect)
			) throw -2;


		// 원본코드 복사 (2008.12.31 by sc.Choi)
		// 빈 코드공간 찾기
		LPBYTE pOrigCode = NULL;
		size_t copysize = 0;

		if(NULL==ptrEmptyCodeArea) EmptyCodeArea();

		if(ptrEmptyCodeArea)
		{
			::VirtualQuery(EmptyCodeArea, &mbi2, sizeof(MEMORY_BASIC_INFORMATION));
			if (FALSE == ::VirtualProtect(
				mbi2.BaseAddress,
				mbi2.RegionSize,
				PAGE_EXECUTE_READWRITE,
				&mbi2.Protect)
				) throw -3;

			for(size_t nOffset=0; nOffset<4096; nOffset+=32)
			{
				LPBYTE pTmpAddr = ((LPBYTE)ptrEmptyCodeArea + nOffset);
				if(0x90 == pTmpAddr[0])
				{
					pOrigCode = pTmpAddr;
					break;
				}
			}

		}
		
		if(pOrigCode)
		{
			while(copysize < 5)	// 후킹코드보다 작으면 계속 원본코드를 수집한다
			{
				// 어셈명령어줄 지정
				LPBYTE srcLine = (LPBYTE)pFuncAddr + copysize;
				LPBYTE tarLine = (LPBYTE)pOrigCode + copysize;

				// 명령어 크기 구함
				size_t linesize = g_disasm.GetLineSize( srcLine );
				if(0==linesize)
				{
					pOrigCode = NULL;
					copysize = 0;
					break;
				}

				// 명령어 복사
				memcpy( tarLine, srcLine, linesize );

				// 명령어가 점프 또는 호출이라면
				if( 0xE8 == srcLine[0] || 0xE9 == srcLine[0] )
				{
					// 주소 보정
					INT_PTR jmp_dist = *(INT_PTR*)(srcLine+1);
					jmp_dist += (INT_PTR)srcLine-(INT_PTR)tarLine;
					*(INT_PTR*)(tarLine+1) = jmp_dist;
				}

				// 명령어 부분을 nop로
				// memset( srcLine, 0x90, linesize );

				copysize += linesize;
			}

			if(pOrigCode)
			{
				_asm
				{
					mov ebx, pOrigCode;
					add ebx, copysize;

					mov al, 0xE9;			// opcode
					mov byte ptr [ebx], al;

					mov eax, pFuncAddr;		// 이전 함수의 주소에서
					sub eax, pOrigCode;		// 새 함수 주소를 빼고
					sub eax, 5;				// 5를 빼면 거리가 나옴

					mov dword ptr [ebx+1], eax;
				}
			}
		}

		// 첫 코드를 점프 또는 호출로 바꿔준다
		BYTE opcode;
		if( bWriteCallCode )
		{
			opcode = 0xE8;		// 32bit relative call
		}
		else
		{
			opcode = 0xE9;		// 32bit relative jmp
		}

		_asm
		{
			mov ebx, pFuncAddr;

			mov al, opcode;			// opcode
			mov byte ptr [ebx], al;

			mov eax, pRedirAddr;	// 새 함수의 주소에서
			sub eax, ebx;			// 이전 함수 주소를 빼고
			sub eax, 5;				// 5를 빼면 거리가 나옴

			mov dword ptr [ebx+1], eax;
		}

		// 훅 객체를 생성하여 리스트에 추가
		HOOKED_FUNC_INFO* pFuncObj	= new HOOKED_FUNC_INFO;
		pFuncObj->m_pFunction		= pFuncAddr;
		pFuncObj->m_pRedirection	= pRedirAddr;
		pFuncObj->m_nDistFromBase	= (size_t)pFuncAddr - (size_t)m_hModule;
		pFuncObj->m_nModifiedSize	= copysize;
		pFuncObj->m_pOrigCode		= pOrigCode;
		pFuncObj->m_bHookSet		= TRUE;
		m_listFuncs.push_back( pFuncObj );

		bRetVal = TRUE;
	}
	catch (int nErrCode)
	{
		nErrCode=nErrCode;
	}

	DWORD dwOldProtect;

	// 프로텍션을 복구
    if(mbi.BaseAddress)
		::VirtualProtect(
			mbi.BaseAddress,
			mbi.RegionSize,
			mbi.Protect,
			&dwOldProtect
			);

	if(mbi2.BaseAddress)
		::VirtualProtect(
			mbi2.BaseAddress,
			mbi2.RegionSize,
			mbi2.Protect,
			&dwOldProtect
			);

	// 모든 쓰레드 복구
	if(bNeedToResumeAllThread) 
		CRawHookMgr::ResumeAllThread();


	return bRetVal;
}



//////////////////////////////////////////////////////////////////////////
//
// 언훅 (후킹모드2)
//
//////////////////////////////////////////////////////////////////////////
BOOL CRawHookedModule::Unhook(HOOKED_FUNC_INFO* pHookInfo)
{	
	if(NULL==pHookInfo) return FALSE;

	BOOL bRetVal = FALSE;
	MEMORY_BASIC_INFORMATION mbi, mbi2;
	ZeroMemory(&mbi, sizeof(MEMORY_BASIC_INFORMATION));
	ZeroMemory(&mbi2, sizeof(MEMORY_BASIC_INFORMATION));


	// 모든 쓰레드 정지했는지 확인
	BOOL bNeedToResumeAllThread = FALSE;
	if(CRawHookMgr::IsAllThreadSuspended() == FALSE)
	{
		CRawHookMgr::SuspendAllThread();
		bNeedToResumeAllThread = TRUE;
	}

	try
	{
		MEMORY_BASIC_INFORMATION mbi;
		::VirtualQuery(pHookInfo->m_pFunction, &mbi, sizeof(MEMORY_BASIC_INFORMATION));

		// 프로텍션을 READWRITE로
		DWORD dwOldProtect;
		if (FALSE == ::VirtualProtect(
			mbi.BaseAddress,
			mbi.RegionSize,
			PAGE_EXECUTE_READWRITE,
			&dwOldProtect)
			) throw -2;


		// 원본 주소에 백업된 코드를 복사해 넣는다. (2008.12.31 by sc.Choi)
		if(pHookInfo->m_pOrigCode && pHookInfo->m_nModifiedSize>0 && pHookInfo->m_bHookSet==FALSE)
		{
			::VirtualQuery(EmptyCodeArea, &mbi2, sizeof(MEMORY_BASIC_INFORMATION));
			if (FALSE == ::VirtualProtect(
				mbi2.BaseAddress,
				mbi2.RegionSize,
				PAGE_EXECUTE_READWRITE,
				&mbi2.Protect)
				) throw -3;

			memcpy(pHookInfo->m_pFunction, pHookInfo->m_pOrigCode, pHookInfo->m_nModifiedSize);
			memset(pHookInfo->m_pOrigCode, 0x90, 32);
		}
		else
		{
			memcpy(pHookInfo->m_pFunction, m_pOrigImg+pHookInfo->m_nDistFromBase, 5);
		}
		
		// 훅 리스트에서 삭제
		CFunctionList::iterator iter = m_listFuncs.begin();
		for( ; iter!=m_listFuncs.end(); iter++ )
		{
			if((*iter) == pHookInfo)
			{
				delete pHookInfo;
				m_listFuncs.erase(iter);
				bRetVal = TRUE;
				break;
			}
		}

	}
	catch (int nErrCode)
	{
		nErrCode=nErrCode;
	}

	DWORD dwOldProtect;

	// 프로텍션을 복구
	if(mbi.BaseAddress)
		::VirtualProtect(
		mbi.BaseAddress,
		mbi.RegionSize,
		mbi.Protect,
		&dwOldProtect
		);

	if(mbi2.BaseAddress)
		::VirtualProtect(
		mbi2.BaseAddress,
		mbi2.RegionSize,
		mbi2.Protect,
		&dwOldProtect
		);

	// 모든 쓰레드 복구
	if(bNeedToResumeAllThread) CRawHookMgr::ResumeAllThread();

	return bRetVal;
}
//#endif

//////////////////////////////////////////////////////////////////////////
//
// 언훅
//
//////////////////////////////////////////////////////////////////////////
BOOL CRawHookedModule::Unhook(void* pFunc)
{
	if(NULL==pFunc) return FALSE;

	CFunctionList::iterator iter = m_listFuncs.begin();
	for( ; iter!=m_listFuncs.end(); iter++ )
	{
		HOOKED_FUNC_INFO* pHook = (*iter);
		if( pHook->m_pFunction == pFunc || pHook->m_pRedirection == pFunc )
		{
			return Unhook(pHook);
		}
	}

	return FALSE;

}



LPVOID	CRawHookedModule::GetOrigFunc(LPVOID pFunc)
{
	CFunctionList::iterator iter = m_listFuncs.begin();
	for( ; iter!=m_listFuncs.end(); iter++ )
	{
		HOOKED_FUNC_INFO* pHook = (*iter);
		if( pHook->m_pFunction == pFunc || pHook->m_pRedirection == pFunc )
		{
			//return (LPVOID)(m_pOrigImg + pHook->m_nDistFromBase);
			return (LPVOID)pHook->m_pOrigCode;
		}
	}

	return NULL;
}


HMODULE	CRawHookedModule::GetModule()
{
	return m_hModule;
}

BOOL CRawHookedModule::IsInHookedModule(LPVOID pCode)
{
	if(NULL==m_pOrigImg) return FALSE;

	INT_PTR pFind = (INT_PTR)pCode;
	INT_PTR pBegin = (INT_PTR)m_pOrigImg;
	INT_PTR pEnd = pBegin + (INT_PTR)m_ModuleInfo.SizeOfImage;
	
	if( pBegin <= pFind && pFind <= pEnd ) return TRUE;

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////
//
// 이 모듈의 특정 바이너리 값을 바꿔놓는다 (바이너리패치모드)
//
//////////////////////////////////////////////////////////////////////////
BOOL CRawHookedModule::preHook(LPVOID pFuncAddr, unsigned char* cBuff, size_t copysize)
{
	BOOL bRetVal = FALSE;
	MEMORY_BASIC_INFORMATION mbi;
	ZeroMemory(&mbi, sizeof(MEMORY_BASIC_INFORMATION));

	// 모든 쓰레드 정지했는지 확인
	BOOL bNeedToResumeAllThread = FALSE;
	if(CRawHookMgr::IsAllThreadSuspended() == FALSE)
	{
		CRawHookMgr::SuspendAllThread();
		bNeedToResumeAllThread = TRUE;
	}
	
	try
	{
		// 함수가 이 모듈껀지 검사
		if( (UINT_PTR)pFuncAddr < (UINT_PTR)m_ModuleInfo.lpBaseOfDll 
			|| (UINT_PTR)pFuncAddr > (UINT_PTR)m_ModuleInfo.lpBaseOfDll+(UINT_PTR)m_ModuleInfo.SizeOfImage) throw -1;

		// 함수가 존재하는 메모리 페이지 정보 쿼리
		::VirtualQuery(pFuncAddr, &mbi, sizeof(MEMORY_BASIC_INFORMATION));

		// 프로텍션을 READWRITE로
		if (FALSE == ::VirtualProtect(
			mbi.BaseAddress,
			mbi.RegionSize,
			PAGE_EXECUTE_READWRITE,
			&mbi.Protect)
			) throw -2;

		// 첫 코드를 점프 또는 호출로 바꿔준다

		_asm
		{
			mov ebx, pFuncAddr;
			mov edx, cBuff;
			mov ecx, copysize;
rout:
			cmp ecx, 0;
			je end;
			mov al, byte ptr [edx];
			mov byte ptr [ebx], al;
			inc edx;
			inc ebx;
			dec ecx;
			jmp rout;
end:
		}
		bRetVal = TRUE;
	}
	catch (int nErrCode)
	{
		nErrCode=nErrCode;
	}

	DWORD dwOldProtect;

	// 프로텍션을 복구
    if(mbi.BaseAddress)
		::VirtualProtect(
			mbi.BaseAddress,
			mbi.RegionSize,
			mbi.Protect,
			&dwOldProtect
			);

	// 모든 쓰레드 복구
	if(bNeedToResumeAllThread) 
		CRawHookMgr::ResumeAllThread();


	return bRetVal;
}