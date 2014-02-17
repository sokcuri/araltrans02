#ifndef __TSTRING_H__
#define __TSTRING_H__

#include <tchar.h>
#include <string>

using namespace std;

#ifndef tstring

#ifdef _UNICODE
#define tstring wstring
#else
#define tstring string
#endif

#endif // tstring

wstring MakeWString(const char *pszString);
WCHAR MakeWChar(const char *pch);
inline char MakeChar (const char *pch) { return *pch; }

#ifdef _UNICODE
#define MakeTString(str) MakeWString(str)
#define MakeTChar(pch) MakeWChar(pch)
#else
#define MakeTString(str) (str)
#define MakeTChar(pch) MakeChar(pch)
#endif

int MyWideCharToMultiByte(
						  UINT CodePage, 
						  DWORD dwFlags, 
						  LPCWSTR lpWideCharStr, 
						  int cchWideChar, 
						  LPSTR lpMultiByteStr, 
						  int cbMultiByte, 
						  LPCSTR lpDefaultChar, 
						  LPBOOL lpUsedDefaultChar 
						  );

int MyMultiByteToWideChar(
						  UINT CodePage, 
						  DWORD dwFlags, 
						  LPCSTR lpMultiByteStr, 
						  int cbMultiByte, 
						  LPWSTR lpWideCharStr, 
						  int cchWideChar 
						  );


inline int Kor2Wide(LPCSTR lpKor, LPWSTR lpWide, int nWideSize=4096)
{
	return MyMultiByteToWideChar(949, 0, lpKor, -1, lpWide, nWideSize);
}

inline int Wide2Kor(LPCWSTR lpWide, LPSTR lpKor, int nKorSize=4096)
{
	return MyWideCharToMultiByte(949, NULL, lpWide, -1, lpKor, nKorSize, NULL, NULL);
}

#endif // __TSTRING_H__
