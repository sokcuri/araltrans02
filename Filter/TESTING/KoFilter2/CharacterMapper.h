#pragma once
#include <windows.h>

class CCharacterMapper
{
	static BYTE _SHIFT_JIS_MAP[0x10000];
	static BOOL bMapInitialized;

public:
	CCharacterMapper(void) {}
	~CCharacterMapper(void) {}
	void InitializeShiftJISMap();

	virtual BOOL EncodeK2J(LPCSTR cszKorCode, LPSTR szJpnCode);
	virtual BOOL DecodeJ2K(LPCSTR cszJpnCode, LPSTR szKorCode);
	//virtual WORD Ascii2ShiftJIS(char chAscii);
	virtual WORD Ascii2KS5601(char chAscii);
	virtual BOOL IsShiftJISText(LPCSTR cszJpnText);
	virtual BOOL IsEncodedText(LPCSTR cszCode);

};

class CCharacterMapper2 : public CCharacterMapper
{
protected:
	static WORD m_awCP932_Lead8586[2][188];

public:
	CCharacterMapper2() {}
	~CCharacterMapper2() {}

	virtual BOOL EncodeK2J(LPCSTR cszKorCode, LPSTR szJpnCode);
	virtual BOOL DecodeJ2K(LPCSTR cszJpnCode, LPSTR szKorCode);

	virtual BOOL IsEncodedText(LPCSTR cszCode);

protected:
	virtual BOOL GetUserDefinedJpnCode(const BYTE *cszKorCode, BYTE *szJpnCode);
	virtual BOOL GetUserDefinedKorCode(const BYTE *cszJpnCode, BYTE *szKorCode);
};