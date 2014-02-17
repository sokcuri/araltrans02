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

WCHAR MakeWChar(const char ch)
{
	WCHAR wszBuf[2];
	char szCh[2]={'\0',};
	szCh[0]=ch;
	MyMultiByteToWideChar(949, 0, szCh, -1, wszBuf, 2);
	return wszBuf[0];
}

int MyWideCharToMultiByte(
						  UINT CodePage, 
						  DWORD dwFlags, 
						  LPCWSTR lpWideCharStr, 
						  int cchWideChar, 
						  LPSTR lpMultiByteStr, 
						  int cbMultiByte, 
						  LPCSTR lpDefaultChar, 
						  LPBOOL lpUsedDefaultChar 
						  )
{
	int nRetVal = 0;
	
	static PROC_WideCharToMultiByte pfnWideCharToMultiByte=NULL;
	static bool bCheckAddr=true;

	if (bCheckAddr)
	{
		HKEY hKey;
		DWORD dwType=REG_DWORD, cbData=sizeof(DWORD);
		
		if (RegOpenKeyEx(HKEY_CURRENT_USER, _T("Software\\AralGood"), NULL, KEY_READ, &hKey) == ERROR_SUCCESS)
		{
			RegQueryValueEx(hKey, _T("W2MAddr"), NULL, &dwType, (BYTE *)&pfnWideCharToMultiByte, &cbData );
			RegCloseKey(hKey);
		}
		bCheckAddr=false;
	}

	if( pfnWideCharToMultiByte )
	{
		nRetVal = pfnWideCharToMultiByte(
			CodePage, 
			dwFlags, 
			lpWideCharStr, 
			cchWideChar, 
			lpMultiByteStr, 
			cbMultiByte, 
			lpDefaultChar, 
			lpUsedDefaultChar 
			);
	}
	else
	{
		nRetVal = WideCharToMultiByte(
			CodePage, 
			dwFlags, 
			lpWideCharStr, 
			cchWideChar, 
			lpMultiByteStr, 
			cbMultiByte, 
			lpDefaultChar, 
			lpUsedDefaultChar 
			);
	}
	
	return nRetVal;
}

int MyMultiByteToWideChar(
						  UINT CodePage, 
						  DWORD dwFlags, 
						  LPCSTR lpMultiByteStr, 
						  int cbMultiByte, 
						  LPWSTR lpWideCharStr, 
						  int cchWideChar 
						  )
{
	int nRetVal = 0;

	static PROC_MultiByteToWideChar pfnMultiByteToWideChar=NULL;
	static bool bCheckAddr=true;
	
	if (bCheckAddr)
	{
		HKEY hKey;
		DWORD dwType=REG_DWORD, cbData=sizeof(DWORD);
		
		if (RegOpenKeyEx(HKEY_CURRENT_USER, _T("Software\\AralGood"), NULL, KEY_READ, &hKey) == ERROR_SUCCESS)
		{
			RegQueryValueEx(hKey, _T("M2WAddr"), NULL, &dwType, (BYTE *)&pfnMultiByteToWideChar, &cbData );
			RegCloseKey(hKey);
		}
		bCheckAddr=false;
	}
	
	if( pfnMultiByteToWideChar )
	{
		nRetVal = pfnMultiByteToWideChar(
			CodePage, 
			dwFlags, 
			lpMultiByteStr, 
			cbMultiByte, 
			lpWideCharStr, 
			cchWideChar 
			);
	}
	else
	{
		nRetVal = MultiByteToWideChar(
			CodePage, 
			dwFlags, 
			lpMultiByteStr, 
			cbMultiByte, 
			lpWideCharStr, 
			cchWideChar 
			);
	}
	
	return nRetVal;
}