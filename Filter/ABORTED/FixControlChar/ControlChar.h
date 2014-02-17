#pragma once

#include <string>
#include <vector>
#include <map>

using namespace std;

class CControlChar
{
public:
	CControlChar(void);
public:
	~CControlChar(void);
public:
	// 맵에 컨트롤 문자열 입력
	bool AddCtrStr(LPCSTR ControlHex,bool isViewed);

	multimap <char,string> ConrolCharMap; 
public:
	bool PreControlChar(LPSTR Dest, LPCSTR Source);
public:
	bool PostControlChar(LPSTR Dest, LPCSTR Source);
};
