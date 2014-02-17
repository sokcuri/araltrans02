// RawHookMgr.h: interface for the CRawHookMgr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RAWHOOKMGR_H__32D8C6B5_3C6F_43E3_91BA_EA52B2E9C83E__INCLUDED_)
#define AFX_RAWHOOKMGR_H__32D8C6B5_3C6F_43E3_91BA_EA52B2E9C83E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef WINVER				// Allow use of features specific to Windows XP or later.
#define WINVER 0x0501		// Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif						


#include <windows.h>
#include <list>
class CRawHookedModule;
typedef std::list<CRawHookedModule*> CModuleList;
using namespace std;

enum _ATHOOKTYPE { ATHOOKTYPE_COPY = 0, ATHOOKTYPE_SOURCE = 1};

class CRawHookMgr  
{
	static CModuleList m_listModules;
	static BOOL	m_bIsAllThreadSuspended;
	static HMODULE m_hExecModule;

	CRawHookMgr();
	virtual ~CRawHookMgr();
	static HMODULE HandleFromAddress(LPVOID pAddr);
	static CRawHookedModule* FindModule(HMODULE hModule);
	static LPVOID	GetFixedFuncAddr(LPVOID pFunc);

public:
	static HMODULE	GetExecModuleHandle() { if (m_hExecModule == NULL) m_hExecModule = GetModuleHandle(NULL); return m_hExecModule; }
	static BOOL		RemoveModuleProtection(HMODULE hModule);
	static BOOL		Hook(LPCTSTR cszDllName, LPCTSTR cszExportName, LPVOID pfnNew, INT nHookType=ATHOOKTYPE_COPY);
	static BOOL		Hook(LPVOID pfnOld, LPVOID pfnNew, BOOL bWriteCallCode=FALSE, INT nHookType=ATHOOKTYPE_COPY);
	static BOOL		preHook(LPVOID pfnOld, unsigned char* cBuff, size_t copysize);
	static LPVOID	GetOrigFunc(LPVOID pFunc);
	static BOOL		IsInHookedModule(LPVOID pCode);
	static BOOL		Unhook(LPVOID pFunc);
	static BOOL		Unhook();
	static void		SuspendAllThread();
	static void		ResumeAllThread();
	static BOOL		IsAllThreadSuspended();

};

#endif // !defined(AFX_RAWHOOKMGR_H__32D8C6B5_3C6F_43E3_91BA_EA52B2E9C83E__INCLUDED_)
