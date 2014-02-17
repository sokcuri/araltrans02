#pragma once
// MyWideCharToMultiByte - from ATCode.cpp for ATCode plugin
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

// MyMultiByteToWideChar - from ATCode.cpp for ATCode plugin
int MyMultiByteToWideChar(
						  UINT CodePage, 
						  DWORD dwFlags, 
						  LPCSTR lpMultiByteStr, 
						  int cbMultiByte, 
						  LPWSTR lpWideCharStr, 
						  int cchWideChar 
						  );

// Kor2Wide - 한글일 경우 간단하게 MultiByteToWideChar 사용하기 위한 인라인 함수
inline int Kor2Wide(LPCSTR lpKor, LPWSTR lpWide, int nWideSize=4096)
{
	return MyMultiByteToWideChar(949, 0, lpKor, -1, lpWide, nWideSize);
}

// Kor2Wide - 한글일 경우 간단하게 WideCharToMultiByte 사용하기 위한 인라인 함수
inline int Wide2Kor(LPCWSTR lpWide, LPSTR lpKor, int nKorSize=4096)
{
	return MyWideCharToMultiByte(949, NULL, lpWide, -1, lpKor, nKorSize, NULL, NULL);
}
