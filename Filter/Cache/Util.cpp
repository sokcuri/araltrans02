#include <windows.h>
#include "Util.h"


BOOL MyCreateDirectory(LPCWSTR lpDirectory)
{
	DWORD pos,len;
	LPTSTR str;

	if (!lpDirectory) return FALSE;

	len=lstrlen(lpDirectory);
	str=new wchar_t [len+1];
	lstrcpy(str,lpDirectory);
	for (pos=0; str[pos]; pos++)
	{
		if (str[pos]==L'\\')
		{
			str[pos]=0;
			CreateDirectory(str,NULL);
			str[pos]=L'\\';
		}
	}
	CreateDirectory(str,NULL);

	delete [] str;
	return CreateDirectory(lpDirectory,NULL) || ((pos=GetFileAttributes(lpDirectory)) != 0xFFFFFFFF && (pos & FILE_ATTRIBUTE_DIRECTORY));
}

const wchar_t *GetGameDirectory()
{
	static wchar_t szGameDirectory[MAX_PATH]={L'\0',};
	int i;

	if (szGameDirectory[0] != L'\0')
		return szGameDirectory;

	// 게임 디렉토리를 알아온다.

	GetModuleFileName(NULL, szGameDirectory, MAX_PATH);

	for(i=lstrlen(szGameDirectory); i>=0; i--)
	{
		if (szGameDirectory[i] == L'\\')
		{
			szGameDirectory[i]=L'\0';
			break;
		}
	}
	return szGameDirectory;
}
