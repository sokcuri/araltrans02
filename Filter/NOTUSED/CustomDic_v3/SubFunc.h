#pragma once
#include "tstring.h"

#define STRLEN 2048

// Hex <-> 문자열 변환 함수 셋
void atoh(LPSTR szDest,LPCSTR szSource);
string atoh(string strSource);

/*void wtoh(LPWSTR szDest, LPCWSTR szSource);
string stl_wtoh(wstring strSource);*/

void htoa(LPSTR szDest,LPCSTR szSource);
string htoa(string strSource);

/*void htow(LPWSTR szDest, LPCSTR szSource);
wstring stl_htow(string strSource);*/

//string EncodeLine(string strSource);
string DecodeLine(string strSource);

int Find(string* strSource, string strFind, int nStart, int nCodePage);