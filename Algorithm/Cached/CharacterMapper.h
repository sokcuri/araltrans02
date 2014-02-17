#pragma once
#include <windows.h>

class CCharacterMapper
{
	CCharacterMapper(void);
	~CCharacterMapper(void);
public:
	static BOOL EncodeK2J(LPCSTR cszKorCode, LPSTR szJpnCode);
	static BOOL DecodeJ2K(LPCSTR cszJpnCode, LPSTR szKorCode);
};
