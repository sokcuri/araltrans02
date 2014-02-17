#include "StdAfx.h"
#include "tstring.h"

typedef int (__stdcall * PROC_WideCharToMultiByte)(UINT,DWORD,LPCWSTR,int,LPSTR,int,LPCSTR,LPBOOL);
typedef int (__stdcall * PROC_MultiByteToWideChar)(UINT,DWORD,LPCSTR,int,LPWSTR,int);

wstring MakeWString(const char *pszString)
{
	WCHAR wszBuf[4096];
	MyMultiByteToWideChar(949, 0, pszString, -1, wszBuf, 4096);
	return wstring(wszBuf);
}

WCHAR MakeWChar(const char *pch)
{
	WCHAR wszBuf[2];
	char szCh[3]={'\0',};

	szCh[0]=pch[0];
	if (IsDBCSLeadByteEx(949, (BYTE) *pch))
		szCh[1]=pch[1];

	MyMultiByteToWideChar(949, 0, szCh, -1, wszBuf, 2);
	return wszBuf[0];
}

