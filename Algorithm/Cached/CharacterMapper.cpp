
#include "CharacterMapper.h"

CCharacterMapper::CCharacterMapper(void)
{
}

CCharacterMapper::~CCharacterMapper(void)
{
}

BOOL CCharacterMapper::EncodeK2J(LPCSTR cszKorCode, LPSTR szJpnCode)
{
	BOOL bRetVal = FALSE;

	if(cszKorCode && szJpnCode && 0xA0 <= (BYTE)cszKorCode[0] && 0xA0 <= (BYTE)cszKorCode[1])
	{
		WORD dwKorTmp = ((BYTE)cszKorCode[0] - 0xA0) * 0xA0;	// 96*96

		dwKorTmp += ((BYTE)cszKorCode[1] - 0xA0);
		dwKorTmp += 0xE000;
		if( 0xE000<dwKorTmp && dwKorTmp<0xFD00 )
		{
			szJpnCode[0] = HIBYTE(dwKorTmp);
			szJpnCode[1] = LOBYTE(dwKorTmp);

			//TRACE("[ aral1 ] EncodeK2J : '%s'(%p) -> (%p) \n", cszKorCode, MAKEWORD(cszKorCode[1], cszKorCode[0]), MAKEWORD(szJpnCode[1], szJpnCode[0]) );
			bRetVal = TRUE;
		}
	}

	return bRetVal;
}

BOOL CCharacterMapper::DecodeJ2K(LPCSTR cszJpnCode, LPSTR szKorCode)
{
	BOOL bRetVal = FALSE;

	if(cszJpnCode && szKorCode && 0xE0 <= (BYTE)cszJpnCode[0] && (BYTE)cszJpnCode[0] <= 0xFC)
	{
		WORD dwJpnTmp = MAKEWORD(cszJpnCode[1], cszJpnCode[0]);

		dwJpnTmp -= 0xE000;
		szKorCode[0] = (char)( ((UINT_PTR)dwJpnTmp/(UINT_PTR)0xA0) + (UINT_PTR)0xA0);
		szKorCode[1] = (char)( ((UINT_PTR)dwJpnTmp%(UINT_PTR)0xA0) + (UINT_PTR)0xA0);

		if( 0xA0 <= (BYTE)szKorCode[0] && 0xA0 <= (BYTE)szKorCode[1] )
		{
			//char dbg[1024];
			//sprintf(dbg, "[ aral1 ] DecodeJ2K : (%p) -> (%p)'%s' \n", 
			//	(UINT_PTR)(MAKEWORD(cszJpnCode[1], cszJpnCode[0])), (UINT_PTR)(MAKEWORD(szKorCode[1], szKorCode[0])), szKorCode );
			//OutputDebugStringA(dbg);

			return TRUE;

		}
	}

	return bRetVal;
}

