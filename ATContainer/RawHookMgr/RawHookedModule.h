// RawHookedModule.h: interface for the CRawHookedModule class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RAWHOOKEDMODULE_H__0CAD71A2_C201_4B80_9971_678C74A124FC__INCLUDED_)
#define AFX_RAWHOOKEDMODULE_H__0CAD71A2_C201_4B80_9971_678C74A124FC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>
#include <tchar.h>
#include <Psapi.h>
#include <list>
using namespace std;

struct HOOKED_FUNC_INFO
{
	LPVOID	m_pFunction;
	LPVOID	m_pRedirection;
	size_t	m_nDistFromBase;
	size_t	m_nModifiedSize;
	LPBYTE	m_pOrigCode;
	BOOL	m_bHookSet;
};

typedef std::list<HOOKED_FUNC_INFO*> CFunctionList;

class CRawHookedModule  
{
private: friend class CRawHookMgr;
	
	HMODULE			m_hModule;		// 이 모듈의 핸들
	LPBYTE			m_pOrigImg;		// 원본모듈 이미지 백업
	CFunctionList	m_listFuncs;	// 후킹된 함수 리스트
	TCHAR			m_szModuleName[MAX_PATH];	// 모듈 이름
	MODULEINFO		m_ModuleInfo;

	CRawHookedModule();
	virtual ~CRawHookedModule();
	
	BOOL	Init(HMODULE hModule);
	BOOL	Hook(LPVOID pFuncAddr, LPVOID pRedirAddr, BOOL bWriteCallCode);
	BOOL	Hook2(LPVOID pFuncAddr, LPVOID pRedirAddr, BOOL bWriteCallCode);
	BOOL	preHook(LPVOID pFuncAddr, unsigned char* cBuff, size_t copysize);
	BOOL	Unhook(HOOKED_FUNC_INFO* pHookInfo);
	BOOL	Unhook(void* pFunc);
	LPVOID	GetOrigFunc(LPVOID pFunc);
	HMODULE	GetModule();
	BOOL	IsInHookedModule(LPVOID pCode);
	void	Close();
	void *	HandledCopyMemory(void * dest, const void *src, size_t count);
};

void EmptyCodeArea();

#endif // !defined(AFX_RAWHOOKEDMODULE_H__0CAD71A2_C201_4B80_9971_678C74A124FC__INCLUDED_)
