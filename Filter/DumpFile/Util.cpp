#include "StdAfx.h"

#include "Util.h"

#include "ATPlugin.h"


BOOL MyCreateDirectory(LPCTSTR lpDirectory)
{
	DWORD pos,len;
	LPTSTR str;

	if (!lpDirectory) return FALSE;

	len=lstrlen(lpDirectory);
	str=new TCHAR [len+1];
	lstrcpy(str,lpDirectory);
	for (pos=0; str[pos]; pos++)
	{
		if (str[pos]==_T('\\'))
		{
			str[pos]=0;
			CreateDirectory(str,NULL);
			str[pos]=_T('\\');
		}
	}
	CreateDirectory(str,NULL);

	delete [] str;
	return CreateDirectory(lpDirectory,NULL) || ((pos=GetFileAttributes(lpDirectory)) != 0xFFFFFFFF && (pos & FILE_ATTRIBUTE_DIRECTORY));
}

const TCHAR *GetGameDirectory()
{
	static TCHAR szGameDirectory[MAX_PATH]={_T('\0'),};
	int i;

	if (szGameDirectory[0] != _T('\0'))
		return szGameDirectory;

	// 게임 디렉토리를 알아온다.

	GetModuleFileName(NULL, szGameDirectory, MAX_PATH);

	for(i=lstrlen(szGameDirectory); i>=0; i--)
	{
		if (szGameDirectory[i] == _T('\\'))
		{
			szGameDirectory[i]=_T('\0');
			break;
		}
	}
	return szGameDirectory;
}

const TCHAR *GetATDirectory()
{
	static TCHAR szATDirectory[MAX_PATH]={_T('\0'),};
	int i;

	if (szATDirectory[0] != _T('\0'))
		return szATDirectory;

	// 아랄트랜스 설치 디렉토리를 알아온다.

	GetModuleFileName(GetModuleHandle(_T("ATCTNR.DLL")), szATDirectory, MAX_PATH);

	for(i=lstrlen(szATDirectory); i>=0; i--)
	{
		if (szATDirectory[i] == _T('\\'))
		{
			szATDirectory[i]=_T('\0');
			break;
		}
	}
	return szATDirectory;
}