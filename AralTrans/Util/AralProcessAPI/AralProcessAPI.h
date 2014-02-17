// AralProcessAPI.h: interface for the CAralProcessAPI class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ARALPROCESSAPI_H__834CAC67_6494_4674_B4E2_4BAA4B7012F6__INCLUDED_)
#define AFX_ARALPROCESSAPI_H__834CAC67_6494_4674_B4E2_4BAA4B7012F6__INCLUDED_

#define ThreadQuerySetWin32StartAddress 9

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CAralProcessAPI  
{
private:
	CAralProcessAPI();
	virtual ~CAralProcessAPI();

public:
	static CString GetProcessName(DWORD dwPID);
	static DWORD GetPrimaryThreadId(DWORD dwPID, DWORD dwADR);
	static DWORD GetPrimaryThreadIdNew(DWORD dwPID);
	static CString GetCurrentModulePath();
	static BOOL Is32bitProcess(DWORD dwPID);
	static HANDLE LaunchAppIntoDifferentSession(LPCTSTR strAppPath);
	static BOOL LaunchAppAsUser(LPCTSTR strAppPath);
	static BOOL ExecuteAndWait(LPCTSTR cszCmdLine, int* pExitCode=NULL);
	static BOOL ImpersonateCurUser();
	static BOOL GetThreadStartAddress(ULONG tid, DWORD *address);
	static DWORD GetPrimaryThreadIdHook(DWORD dwPID, DWORD dwTID);
	static DWORD GetPrimaryThreadIdNext(DWORD dwPID, DWORD dwTID);
};

#endif // !defined(AFX_ARALPROCESSAPI_H__834CAC67_6494_4674_B4E2_4BAA4B7012F6__INCLUDED_)
