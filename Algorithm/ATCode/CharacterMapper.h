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

#include <vector>
#include <string>

using namespace std;

typedef vector<string> CSTLStringArray;

#define ENCODE_MARKER_STRING "ENCODE2KOR"
#define ENCODE_MARKER_LENGTH 10

class CCharacterMapper2 : public CCharacterMapper
{
protected:
	static WORD m_awCP932_Lead8586[2][188];
	static CRITICAL_SECTION *m_pCS;

protected:
	CSTLStringArray m_cStringArray;

public:
	CCharacterMapper2();
	~CCharacterMapper2() {}

	static void ClearStaticMembers();

	virtual BOOL EncodeK2J(LPCSTR cszKorCode, LPSTR szJpnCode);
	virtual BOOL DecodeJ2K(LPCSTR cszJpnCode, LPSTR szKorCode);

	virtual BOOL IsEncodedText(LPCSTR cszCode);

	virtual BOOL StoreEncodedText(LPSTR __inout szText);
	virtual BOOL RestoreDecodedText(LPSTR __inout szText);

protected:
	virtual BOOL GetUserDefinedJpnCode(const BYTE *cszKorCode, BYTE *szJpnCode);
	virtual BOOL GetUserDefinedKorCode(const BYTE *cszJpnCode, BYTE *szKorCode);
};