
#include "stdafx.h"
#include "CharacterMapper.h"


BYTE CCharacterMapper::_SHIFT_JIS_MAP[0x10000];
BOOL CCharacterMapper::bMapInitialized = FALSE;

const BYTE _MAP[][4]={
	{ '\xBE', '\xC6', '\xA8', '\xA1' },	// ('아')
	{ '\xB7', '\xB8', '\xA8', '\xA2' },	// ('렇')
	{ '\xB8', '\xD5', '\xA8', '\xA3' },	// ('먼')
	{ '\x86', '\x9C', '\xA8', '\xA4' },	// ('넑')
	{ '\x9A', '\x9C', '\xA8', '\xA5' },	// ('슌')
	{ '\x8B', '\xAF', '\xA8', '\xA6' },	// ('떙')
	{ '\x00', '\x00', '\x00', '\x00' }
};

//////////////////////////////////////////////////////////////////////////
//
// ★ 한글코드를 SHIFTJIS 중 사용 빈도가 낮은 한자코드영역에 매핑
//
//////////////////////////////////////////////////////////////////////////
BOOL CCharacterMapper::EncodeK2J(LPCSTR cszKorCode, LPSTR szJpnCode)
{
	BOOL bRetVal = FALSE;

	for(int k=3;_MAP[k][0];k++)
	{
	if(cszKorCode && cszKorCode && 0xA0 <= (BYTE)cszKorCode[0] && 0xA0 <= (BYTE)cszKorCode[1]
		|| _MAP[k][0] == (BYTE)cszKorCode[0] && _MAP[k][1] == (BYTE)cszKorCode[1]	)
	{
		// 889F~9872, 989F~9FFC, E040~EAA4, ED40~EEEC
		// 95~3074, 3119~4535, 16632~18622, 19089~19450
		BYTE hiKorByte = (BYTE)cszKorCode[0];
		BYTE loKorByte = (BYTE)cszKorCode[1];
		/*
		// 예외 ('아')
		if(hiKorByte==0xBE && loKorByte==0xC6){ hiKorByte = 0xA8; loKorByte = 0xA1; }
		// 예외 ('렇')
		if(hiKorByte==0xB7 && loKorByte==0xB8){ hiKorByte = 0xA8; loKorByte = 0xA2; }
		// 예외 ('먼')
		if(hiKorByte==0xB8 && loKorByte==0xD5){ hiKorByte = 0xA8; loKorByte = 0xA3; }
		// 예외 ('넑')
		if(hiKorByte==0x86 && loKorByte==0x9C){ hiKorByte = 0xA8; loKorByte = 0xA4; }
		// 예외 ('슌')
		if(hiKorByte==0x9A && loKorByte==0x9C){ hiKorByte = 0xA8; loKorByte = 0xA5; }
		// 예외 ('떙')
		if(hiKorByte==0x8B && loKorByte==0xAF){ hiKorByte = 0xA8; loKorByte = 0xA6; }
		*/

		int z=0;
		while(_MAP[z][0])
		{
			if(hiKorByte==_MAP[z][0] && loKorByte==_MAP[z][1])
			{
				hiKorByte = _MAP[z][2];
				loKorByte = _MAP[z][3];
				break;
			}
			(BYTE)z++;
		}

		WORD dwKorTmp = ( hiKorByte - 0xA0) * 0xA0;	// 96*96
		dwKorTmp += (loKorByte - 0xA0);

		dwKorTmp += 95;
		if(dwKorTmp > 3074) dwKorTmp += 44;
		if(dwKorTmp > 4535) dwKorTmp += 12096;
		if(dwKorTmp > 18622) dwKorTmp += 466;
		if(dwKorTmp > 19450) return FALSE;
		
		BYTE hiByte = (dwKorTmp / 189) + 0x88;
		BYTE loByte = (dwKorTmp % 189) + 0x40;
		dwKorTmp = MAKEWORD( loByte, hiByte );

		TRACE("[ aral1 ] EncodeK2J : '%s'(%p) -> (%p) \n", cszKorCode, MAKEWORD(cszKorCode[1], cszKorCode[0]), dwKorTmp );

		if( 0x889F <= dwKorTmp && dwKorTmp <= 0xEEEC )
		{
			szJpnCode[0] = HIBYTE(dwKorTmp);
			szJpnCode[1] = LOBYTE(dwKorTmp);

			bRetVal = TRUE;
		}
	}
	}
	return bRetVal;
}


//////////////////////////////////////////////////////////////////////////
//
// ★ 매핑되었던 한글코드를 복구
//
//////////////////////////////////////////////////////////////////////////
BOOL CCharacterMapper::DecodeJ2K(LPCSTR cszJpnCode, LPSTR szKorCode)
{
	BOOL bRetVal = FALSE;

	// 889F~9872, 989F~9FFC, E040~EAA4, ED40~EEEC
	// 95~3074, 3119~4535, 16632~18622, 19089~19450
	if	( cszJpnCode && szKorCode && 
			(
				(0x88 <= (BYTE)cszJpnCode[0] && (BYTE)cszJpnCode[0] <= 0x9F) ||
				(0xE0 <= (BYTE)cszJpnCode[0] && (BYTE)cszJpnCode[0] <= 0xEE)
			)
		)
	{


		WORD dwKorTmp = (WORD)( (BYTE)cszJpnCode[0] - 0x88 ) * 189;
		dwKorTmp += (WORD)( (BYTE)cszJpnCode[1] - 0x40 );

		if(dwKorTmp > 19450) return FALSE;
		if(dwKorTmp > 18622) dwKorTmp -= 466;
		if(dwKorTmp > 4535) dwKorTmp -= 12096;
		if(dwKorTmp > 3074) dwKorTmp -= 44;
		dwKorTmp -= 95;

		szKorCode[0] = (char)( ((UINT_PTR)dwKorTmp/(UINT_PTR)0xA0) + (UINT_PTR)0xA0);
		szKorCode[1] = (char)( ((UINT_PTR)dwKorTmp%(UINT_PTR)0xA0) + (UINT_PTR)0xA0);

		TRACE("[ aral1 ] DecodeJ2K : (%p) -> (%p)'%s' \n", (UINT_PTR)(MAKEWORD(cszJpnCode[1], cszJpnCode[0])), (UINT_PTR)(MAKEWORD(szKorCode[1], szKorCode[0])), szKorCode);

		for(int k=3;_MAP[k][0];k++)
		{
		if( 0xA0 <= (BYTE)szKorCode[0] && 0xA0 <= (BYTE)szKorCode[1]
			|| _MAP[k][0] == (BYTE)szKorCode[0] && _MAP[k][1] == (BYTE)szKorCode[1]	)
		{
			int z=0;
			while(_MAP[z][0])
			{
				if( (BYTE)szKorCode[0]==(BYTE)_MAP[z][2] && (BYTE)szKorCode[1]==(BYTE)_MAP[z][3] )
				{
					*((BYTE*)szKorCode) = (BYTE)_MAP[z][0];
					*((BYTE*)szKorCode+1) = (BYTE)_MAP[z][1];
					break;
				}
				(BYTE)z++;
			}
			/*
			// 예외 ('아')
			if( (BYTE)szKorCode[0]==0xA8 && (BYTE)szKorCode[1]==0xA1 )
			{
				*((BYTE*)szKorCode) = 0xBE;
				*((BYTE*)szKorCode+1) = 0xC6;
			}

			// 예외 ('렇')
			if( (BYTE)szKorCode[0]==0xA8 && (BYTE)szKorCode[1]==0xA2 )
			{
				*((BYTE*)szKorCode) = 0xB7;
				*((BYTE*)szKorCode+1) = 0xB8;
			}

			// 예외 ('먼')
			if( (BYTE)szKorCode[0]==0xA8 && (BYTE)szKorCode[1]==0xA3 )
			{
				*((BYTE*)szKorCode) = 0xB8;
				*((BYTE*)szKorCode+1) = 0xD5;
			}
			// 예외 ('넑')
			if( (BYTE)szKorCode[0]==0xA8 && (BYTE)szKorCode[1]==0xA4 )
			{
				*((BYTE*)szKorCode) = 0x86;
				*((BYTE*)szKorCode+1) = 0x9C;
			}
			// 예외 ('슌')
			if( (BYTE)szKorCode[0]==0xA8 && (BYTE)szKorCode[1]==0xA5 )
			{
				*((BYTE*)szKorCode) = 0x9A;
				*((BYTE*)szKorCode+1) = 0x9C;
			}
			// 예외 ('떙')
			if( (BYTE)szKorCode[0]==0xA8 && (BYTE)szKorCode[1]==0xA6 )
			{
				*((BYTE*)szKorCode) = 0x8B;
				*((BYTE*)szKorCode+1) = 0xAF;
			}
			*/

			bRetVal = TRUE;
		}
		}
	}

	return bRetVal;
}


//////////////////////////////////////////////////////////////////////////
//
// 이 함수는 머였더라...? ㅡ,.ㅡ;;
//
//////////////////////////////////////////////////////////////////////////
WORD CCharacterMapper::Ascii2KS5601( char chAscii )
{
	WORD wChar = 0;

	if( 0x20 <= (BYTE)chAscii && (BYTE)chAscii <= 0x7E )
	{	
		if(' ' == chAscii) wChar = 0xA1A1;
		else wChar = MAKEWORD( 0x80 + (BYTE)chAscii, 0xA3 );
	}

	return wChar;	
}


//////////////////////////////////////////////////////////////////////////
//
// _SHIFT_JIS_MAP 배열을 사용하여 주어진 문자열의 문자들이
// 모두 유효한 일본어인지 검사
// TRUE:유효, FALSE:비정상 문자 포함
//
//////////////////////////////////////////////////////////////////////////
BOOL CCharacterMapper::IsShiftJISText( LPCSTR cszJpnText )
{
	BOOL bRetVal = TRUE;

	if(FALSE == bMapInitialized)
	{
		InitializeShiftJISMap();
		bMapInitialized = TRUE;
	}

	int i=0;
	while(cszJpnText[i])
	{
		WORD chCode = 0;

		// 문자 코드 만들기
		if((BYTE)cszJpnText[i] >= 0x80 && cszJpnText[i+1] != '\0')
		{
			chCode = MAKEWORD((BYTE)cszJpnText[i+1], (BYTE)cszJpnText[i]);
			i++;
		}
		else
		{
			chCode = MAKEWORD((BYTE)cszJpnText[i], 0);
		}

		// 코드 맵에서 검사
		if(_SHIFT_JIS_MAP[chCode] == FALSE)
		{
			bRetVal = FALSE;
			break;
		}

		i++;
	}

	return bRetVal;
}



//////////////////////////////////////////////////////////////////////////
//
// SAFE 옵션을 위한 SHIFTJIS 코드 유효 테이블
// 1: 유효 문자, 0: 비정상 문자
//
//////////////////////////////////////////////////////////////////////////
void CCharacterMapper::InitializeShiftJISMap()
{
	ZeroMemory(_SHIFT_JIS_MAP, 0x10000);
	
	_SHIFT_JIS_MAP[0x0A] = 1;
	_SHIFT_JIS_MAP[0x0D] = 1;
	memset(&_SHIFT_JIS_MAP[0x20], 1, 0x7E-0x20+1);

	memset(&_SHIFT_JIS_MAP[0x8140], 1, 0x81AC-0x8140+1);
	memset(&_SHIFT_JIS_MAP[0x81B8], 1, 0x81BF-0x81B8+1);
	memset(&_SHIFT_JIS_MAP[0x81C8], 1, 0x81CE-0x81C8+1);
	memset(&_SHIFT_JIS_MAP[0x81DA], 1, 0x81E8-0x81DA+1);
	memset(&_SHIFT_JIS_MAP[0x81F0], 1, 0x81F7-0x81F0+1);
	_SHIFT_JIS_MAP[0x91FC] = 1;
	
	memset(&_SHIFT_JIS_MAP[0x824F], 1, 0x8258-0x824F+1);
	memset(&_SHIFT_JIS_MAP[0x8260], 1, 0x8279-0x8260+1);
	memset(&_SHIFT_JIS_MAP[0x8281], 1, 0x829A-0x8281+1);
	memset(&_SHIFT_JIS_MAP[0x829F], 1, 0x82F1-0x829F+1);

	memset(&_SHIFT_JIS_MAP[0x8340], 1, 0x8396-0x8340+1);
	memset(&_SHIFT_JIS_MAP[0x839F], 1, 0x83B6-0x839F+1);
	memset(&_SHIFT_JIS_MAP[0x83BF], 1, 0x83D6-0x83BF+1);

	memset(&_SHIFT_JIS_MAP[0x8440], 1, 0x8460-0x8440+1);
	memset(&_SHIFT_JIS_MAP[0x8470], 1, 0x8491-0x8470+1);
	memset(&_SHIFT_JIS_MAP[0x849F], 1, 0x84BE-0x849F+1);

	memset(&_SHIFT_JIS_MAP[0x8740], 1, 0x875D-0x8740+1);
	memset(&_SHIFT_JIS_MAP[0x875F], 1, 0x8775-0x875F+1);
	memset(&_SHIFT_JIS_MAP[0x877E], 1, 0x879C-0x877E+1);

	memset(&_SHIFT_JIS_MAP[0x889F], 1, 0x88FC-0x889F+1);
	for(BYTE hiByte = 0x89; hiByte <= 0xE9; hiByte++)
	{
		WORD idxBegin = MAKEWORD(0x40, hiByte);
		WORD idxEnd = MAKEWORD(0xFC, hiByte);
		memset(&_SHIFT_JIS_MAP[idxBegin], 1, idxEnd-idxBegin+1);		
	}
	memset(&_SHIFT_JIS_MAP[0xEA40], 1, 0xEAA4-0xEA40+1);

	memset(&_SHIFT_JIS_MAP[0xED40], 1, 0xEDFC-0xED40+1);
	memset(&_SHIFT_JIS_MAP[0xEE40], 1, 0xEEFC-0xEE40+1);

}
//////////////////////////////////////////////////////////////////////////
//
// 해당 텍스트 문자가 Encode 가능성이 있는지 확인하는 함수
// TRUE : 디코드 필요 FALSE : 디코드 불필요
//
//////////////////////////////////////////////////////////////////////////

BOOL CCharacterMapper::IsEncodedText(LPCSTR cszCode)
{
	// 889F~9872, 989F~9FFC, E040~EAA4, ED40~EEEC
	// 95~3074, 3119~4535, 16632~18622, 19089~19450
	if ( 
		(0x88 <= (BYTE)cszCode[0] && (BYTE)cszCode[0] <= 0x9F) ||
		(0xE0 <= (BYTE)cszCode[0] && (BYTE)cszCode[0] <= 0xEE)
	)
		return TRUE;
	
	return FALSE;
}

/*
	* CCharacterMapper2 : 새로 제작하는 EncodeKor 관련 라이브러리

	1) 기존 EncodeKor 의 문제점
		기존 EncodeKor (CCharacterMapper) 의 경우 KS X 1001 (KS C 5601)의 기호, 한글,
		한자 전체를 일본어 한자영역에 강제맵핑하는 방식으로 이루어져 일본어와 혼용되도록
		되어 있어 해당 글자가 일본어 맵핑된 한글인지, 원래 일본어 한자인지 알 방도가 없다.

	2) 아이디어
		- 어차피 번역되어 나오는 글자의 99%는 (커스텀딕 등을 통하지 않는 이상) 한자가
		포함되어 있지 않은 순 한글이므로 KS X 1001 의 한자 맵핑은 필요치 않다. 혹 있더라도
		Shift-JIS 의 한자로 맵핑할 수 있지 않을까.
		- 도형 (원문자, 2-byte 영어, 로마자 등)의 경우 Shift-JIS 에 최소 95% 동일한
		글자가 존재한다.
		- 일상 사용되는 한글의 99%는 KS X 1001 의 2350자 한글에 포함되고 아주 가끔씩
		CP949 (확장 완성형 한글)에 존재하는 글자가 나온다. 이 글자는 한 게임당 많아봐야
		100자 미만으로 나올 것 같으니 동적으로 코드를 할당해서 처리할 수 있지 않을까.
		- 보통 대부분의 게임에서는 IsLeadByte(Ex) 같은 Windows API 함수로 Leadbyte를 사용해
		엄정하게 체크하지 않고 그냥 0x81-9F는 2바이트, 0xE0-FC 는 2바이트, 나머지는 1바이트로
		처리한다.
		- 왜인지 모르지만 CP932 (Shift-JIS 확장형) 에는 빈 공간이 있다. 그 공간은
			(1) Leadbyte 0x85, 86, EB, EC, EF 까지 5 구간 (이유 불명)
			(2) Leadbyte 0xF0 ~ F9 까지 10 구간 (이 부분은 일명 외자공간. 사용자 정의공간이다)
		이며, 각 구간의 Trailbyte 0x40~7E, 0x80~FC 까지 188자 공간을 사용할 수 있다.
		따라서, 해당 공간을 풀로 사용할 경우 15 x 188 = 2820 글자의 CP932 와 겹치지 않는
		공간을 사용가능하다.
		- KS X 1001 의 한글 (도형 및 한자 제외)은 Leadbyte 0xB0 ~ C8 까지 25구간이며,
		각 구간은 Trailbyte 0xA1 ~ FE 까지 94자, 총 25 x 94 = 2350자이다.
		- 흥미로운 것은 CP932 의 1 구간당 188자는 KS X 1001의 2구간 2 x 94 = 188 과 동일하며,
		KS X 1001 의 2 구간 글자는 CP 932의 1 구간 글자에 바로 맵핑할 수 있다.
	
	3) 구현
		- KS X 1001 의 25구간 2350자는 CP932의 EB, EC, EF, F0~F9 의 13구간에 2:1로 맵핑한다.
		- 도형 및 한자는 (물론 1:1 맵핑되지 않는 글자가 많긴 하지만) CP932로 역변환 시켜서
		일본어 취급한다. 혹은, 어차피 한글에서 한자의 경우 거의 발생하지 않기 때문에 예외처리
		시키는 것도 좋다 (이것은 추후 결정)
		- CP949 확장 완성형 한글은 예외처리를 하여, 동적으로 CP932 의 85, 86 구간에 할당한다.

	4) 예상되는 장점
		- 한글과 일어가 완전히 나뉘므로 어떤 문자열이 인코딩된 한글인지 번역안된 일본어인지
		고민할 필요가 없어진다 (따라서 KoFilter 가 필요 없어질 수도 있다)
		- 출력시 한글과 일본어, 한자가 섞여도 제대로 표시될 것이다
		- 이에 따른 불필요한 번역작업이나 관련 필터가 사라지기 때문에 속도향상을 기대해
		볼 수 있다

	5) 예상되는 단점
		- 이유는 모르지만 어쨌든 사용하지 않는 5 구간을 억지로 사용하는 것이기 때문에
		예견 못했던 문제가 생길 수 있다.
		- IsLeadByte(Ex) 등으로 엄격하게 MBCS 체크를 하는 경우 문제가 생길 수 있다.
		- F0~F9 까지의 10 영역은 원래 이렇게 쓰라고 남겨놓은 구간이므로 위의 문제는 생기지
		않지만 이 공간을 사용하는 프로그램이 있을 수 있으며 (일례로 일본 휴대폰의 경우
		이 영역에 이모티콘을 맵핑하여 사용하고 있다) 그 경우 문제가 생길 수 있다.

*/

WORD CCharacterMapper2::m_awCP932_Lead8586[2][188] = {NULL, };
CRITICAL_SECTION *CCharacterMapper2::m_pCS = NULL;

CCharacterMapper2::CCharacterMapper2()
{
	if (!m_pCS)
	{
		CRITICAL_SECTION *pCS = new CRITICAL_SECTION;
		if (InterlockedCompareExchange((LONG *)&m_pCS, (LONG)pCS, NULL) == NULL)
			InitializeCriticalSection(m_pCS);
		else
			delete pCS;
	}
}

void CCharacterMapper2::ClearStaticMembers()
{
	CRITICAL_SECTION *pCS = (CRITICAL_SECTION *)InterlockedExchange((LONG *)&m_pCS, NULL);
	if (pCS)
	{
		ZeroMemory(m_awCP932_Lead8586, sizeof(WORD)*2*188);
		DeleteCriticalSection(pCS);
		delete pCS;
	}
}

BOOL CCharacterMapper2::EncodeK2J(LPCSTR cszKorCode, LPSTR szJpnCode)
{
	if (!cszKorCode || !szJpnCode)
		return FALSE;

	if (cszKorCode[0] == '\0') 
		return FALSE;

	BOOL bRet = FALSE;
		
	BYTE achKor[3] = {0,}, achJpn[3] = {0,};
	achKor[0] = (BYTE) cszKorCode[0];
	achKor[1] = (BYTE) cszKorCode[1];

	if ( (0xB0 <= achKor[0]) && (achKor[0] <= 0xC8) && (0xA1 <= achKor[1]) && (achKor[1] <= 0xFE) )
	{
		// KS X 1001 한글 영역

		// Leadbyte 결정
		BYTE byTempLead = (achKor[0] - 0xB0)/2;	// B0-B1=0, B2-B3=1, ...

		if (byTempLead == 0)	// B0-B1
			achJpn[0] = 0xEB;
		else if (byTempLead == 1)	// B2-B3
			achJpn[0] = 0xEC;
		else	// B4-C8
			achJpn[0] = 0xEF + byTempLead - 2;	// B4-B5: 0xEF + 2 - 2 = 0xEF, B6-B7: 0xEF + 3 - 2 = 0xF0, ...

		// Trailbyte 결정
		BYTE byTempTrail = achKor[1] - 0xA1;	// 0xA1-FE 를 0-93 범위로 옮기고
		
		if (achKor[0] % 2)
			byTempTrail += 94;	// LeadByte 가 홀수인 경우 뒤쪽 (94-187)에 맵핑

		byTempTrail += 0x40;	// CP932 의 위치인 0x40 이후로 돌려놓은 후

		if (byTempTrail >= 0x7F)
			byTempTrail ++;		// 0x7F 는 사용하지 않으므로 +1 시켜줌
		
		achJpn[1] = byTempTrail;	// 완성!
		bRet = TRUE;
	}
	else if ( (0xA1 <= achKor[0]) && (achKor[0] <= 0xAF) && (0xA1 <= achKor[1]) && (achKor[1] <= 0xFE) )
	{
		// KS X 1001 도형, 일어, 로마숫자 영역

		WCHAR wchTemp[2] = {0, };

		try
		{
			/*
			// 일어로 역변환시킨다
			if (!MyMultiByteToWideChar(949, 0, (LPCSTR)achKor, -1, wchTemp, 2))
				throw -1;
			if (!MyWideCharToMultiByte(932, 0, wchTemp, -1, (LPSTR)achJpn, 3, NULL, NULL))
				throw -2;
			bRet = TRUE;
			*/

			// 원래는 역변환시켜 처리하려 했는데 어차피 도형이나 로마숫자가 많이 나올 것 같지 않으므로
			// 그냥 예외처리 시킨다. 만약 나중에 예외처리 영역이 혹시라도 넘친다면 역변환을 생각해 보자
			bRet = GetUserDefinedJpnCode(achKor, achJpn);
		}
		catch (LONG e)
		{
			// 원래는 에러처리를 하기 위해 받았지만 새삼 Debug.h 넣기도 뭐하고 해서 일단 제외
			UNREFERENCED_PARAMETER(e);
		}
	}
	else if ( (0xC9 <= achKor[0]) && (achKor[0] <= 0xFE) && (0xA1 <= achKor[1]) && (achKor[1] <= 0xFE) )
	{
		// KS X 1001 한자 영역
		bRet = GetUserDefinedJpnCode(achKor, achJpn);
	}
	else if ( (0x81 <= achKor[0]) && (achKor[0] <= 0xC6) && (0x41 <= achKor[1]) && (achKor[1] <= 0xFE) )
	{
		// CP 949 확장 한글 영역
		bRet = GetUserDefinedJpnCode(achKor, achJpn);
	}

	if (bRet)
	{
		szJpnCode[0] = (char)achJpn[0];
		szJpnCode[1] = (char)achJpn[1];
	}

	return bRet;
}
BOOL CCharacterMapper2::DecodeJ2K(LPCSTR cszJpnCode, LPSTR szKorCode)
{
	if (!cszJpnCode || !szKorCode)
		return FALSE;

	if (cszJpnCode[0] == '\0')
		return FALSE;

	BOOL bRet = FALSE;

	BYTE achKor[3] = {0,}, achJpn[3] = {0,};
	achJpn[0] = (BYTE) cszJpnCode[0];
	achJpn[1] = (BYTE) cszJpnCode[1];

	if ( (0xEB == achJpn[0] || 0xEC == achJpn[0] || (0xEF <= achJpn[0] && achJpn[0] <= 0xF9)) &&
		(0x40 <= achJpn[1] && achJpn[1] <= 0xFE) )
	{
		// KS X 1001 한글 영역

		// Leadbyte 결정
		BYTE byTempLead = achJpn[0] - 0xEB;	// 0xEB = 0, 0xEC = 1
		if (achJpn[0] >= 0xEF)
			byTempLead -=2; // 0xEF = 0xEF-0xEB - 2 = 2, 0xF0 = 3, ...

		byTempLead = byTempLead * 2 + 0xB0; // 0 * 2 + 0xB0 = 0xB0, 1 * 2 + 0xB0 = 0xB2, 2 * 2 + 0xB0 = 0xB4, ...

		// Trailbyte 결정
		BYTE byTempTrail = achJpn[1] - 0x40; // Trailbyte 를 0-188 범위로 옮기고
		
		if (achJpn[1] > 0x7F)
			byTempTrail --;		// 안쓰는 0x7F 를 걸러낸 다음

		if (byTempTrail >= 94)	// Trailbyte 가 후반부 (94-187) 이면
		{
			byTempLead ++;		// Leadbyte 는 홀수
			byTempTrail -= 94;	// Trailbyte 는 94를 빼서 0-93 범위로 맞춤
		}
		byTempTrail += 0xA1;	// A1-FE 범위로 돌려놓음

		achKor[0] = byTempLead;
		achKor[1] = byTempTrail;
		bRet = TRUE;
	}
	else if ( (0x85 == achJpn[0] || 0x86 == achJpn[0]) && (0x40 <= achJpn[1] && achJpn[1] <= 0xFE) )
	{
		// 사용자 지정 글자
		bRet = GetUserDefinedKorCode(achJpn, achKor);
	}

	if (bRet)
	{
		szKorCode[0] = (char)achKor[0];
		szKorCode[1] = (char)achKor[1];
	}

	return bRet;
}

BOOL CCharacterMapper2::IsEncodedText(LPCSTR cszCode)
{
	BYTE byTest = cszCode[0];

	if ( 0x85 == byTest || 0x86 == byTest || 0xEB == byTest || 0xEC == byTest || (0xEF <= byTest && byTest <= 0xF9) )
	{
		return TRUE;
	}
	return FALSE;
}

BOOL CCharacterMapper2::StoreEncodedText(LPSTR __inout szText)
{
	int nLength, i;

	m_cStringArray.clear();

	string strResult;

	string strEncodedKorean;
	bool bIsKorean = false;

	nLength = lstrlenA(szText);

	for (i=0; i<nLength; i++)
	{
		if (IsEncodedText(szText+i))
		{
			// EncodeKor2 한글
			if (bIsKorean == false)
			{
				// 첫번째 한글 글자
				bIsKorean = true;
			}
			// 인코딩된 한글을 저장한다
			strEncodedKorean+=szText[i];
			i++;
			if (i == nLength) break;
			strEncodedKorean+=szText[i];
		}
		else
		{
			if (bIsKorean == true)
			{
				// 인코딩된 한글 끝
				bIsKorean = false;

				int nEncodedLength = strEncodedKorean.size();
				char szEncode[3]={0,}, szDecode[3]={0,};
				string strDecodedKorean;

				// 디코드하고
				for(int idx=0; idx < nEncodedLength; idx+=2)
				{
					szEncode[0]=strEncodedKorean[idx];
					szEncode[1]=strEncodedKorean[idx+1];

					DecodeJ2K(szEncode, szDecode);

					strDecodedKorean += szDecode;
				}

				// 저장한 후 마크
				m_cStringArray.push_back(strDecodedKorean);
				strResult+=ENCODE_MARKER_STRING;
				strEncodedKorean.clear();
			}
			if (IsDBCSLeadByteEx(932, (BYTE)szText[i]))
			{
				strResult+= szText[i];
				i++;
			}
			if (i == nLength) break;
			strResult+=szText[i];
		}
	}

	if (bIsKorean)
	{
		// 문장 마지막이 한글 - 마지막 한글을 저장후 마크
		int nEncodedLength = strEncodedKorean.size();
		char szEncode[3]={0,}, szDecode[3]={0,};
		string strDecodedKorean;

		for(int idx=0; idx < nEncodedLength; idx+=2)
		{
			szEncode[0]=strEncodedKorean[idx];
			szEncode[1]=strEncodedKorean[idx+1];

			DecodeJ2K(szEncode, szDecode);

			strDecodedKorean += szDecode;
		}

		m_cStringArray.push_back(strDecodedKorean);
		strResult+=ENCODE_MARKER_STRING;
	}

	/*
	// 인코딩된 한글 이외에 번역할 것이 없으면 번역 포기
	nLength = strResult.size();
	for(i=0; i<nLength; i++)
	{
		if ((BYTE)strResult[i] >= 0x80)
			break;
	}
	if (i == nLength)
		return FALSE;
	*/

	lstrcpyA(szText, strResult.c_str());

	return TRUE;
}
BOOL CCharacterMapper2::RestoreDecodedText(LPSTR __inout szText)
{
	if (m_cStringArray.empty())
	{
		// 인코딩 치환할 것이 없음
		return TRUE;
	}

	int idxStringId = 0;
	int i, nLength = lstrlenA(szText);
	string strResult;

	for (i=0; i<nLength; i++)
	{
		if ( (nLength - i >= ENCODE_MARKER_LENGTH) && 
			(CompareStringA(LOCALE_NEUTRAL, NULL, szText+i, ENCODE_MARKER_LENGTH, ENCODE_MARKER_STRING, ENCODE_MARKER_LENGTH) == CSTR_EQUAL) )
		{
			// 저장 마커를 찾았으니 치환
			strResult+=m_cStringArray[idxStringId];
			idxStringId++;
			i+=ENCODE_MARKER_LENGTH;
			if (i == nLength)
				break;
		}
		strResult+=szText[i];
	}
/*
	FILE *fp;
	fp=fopen("d:\\charmap.ko.txt", "a");
	fprintf(fp, "%s\n", strResult.c_str());
	fclose(fp);
*/
	lstrcpyA(szText, strResult.c_str());

	return TRUE;
}

BOOL CCharacterMapper2::GetUserDefinedJpnCode(const BYTE *cszKorCode, BYTE *szJpnCode)
{
	BOOL bRet = FALSE;
	BYTE byLead, byTrail;
	WORD wKorCode = *(WORD *)cszKorCode;

	EnterCriticalSection(m_pCS);

	for (byLead = 0; byLead < 2; byLead++)
	{
		for (byTrail = 0; byTrail < 188; byTrail ++)
		{
			if (m_awCP932_Lead8586[byLead][byTrail] == wKorCode)
			{
				bRet = TRUE;
				break;
			}
			else if (m_awCP932_Lead8586[byLead][byTrail] == NULL)
			{
				m_awCP932_Lead8586[byLead][byTrail] = wKorCode;
				bRet = TRUE;
				break;
			}
		}
		if (bRet)
			break;
	}
	LeaveCriticalSection(m_pCS);

	if (bRet)
	{
		byLead += 0x85;
		byTrail += 0x40;
		if (byTrail >= 0x7F)
			byTrail++;

		szJpnCode[0] = byLead;
		szJpnCode[1] = byTrail;
	}
	return bRet;

}

BOOL CCharacterMapper2::GetUserDefinedKorCode(const BYTE *cszJpnCode, BYTE *szKorCode)
{
	BYTE byLead, byTrail;
	WORD wKorCode;

	byLead = cszJpnCode[0] - 0x85;
	byTrail = cszJpnCode[1] - 0x40;
	if (cszJpnCode[1] > 0x7F)
		byTrail--;

	wKorCode = m_awCP932_Lead8586[byLead][byTrail];

	if (wKorCode)
	{
		*(WORD *)szKorCode = wKorCode;
		return TRUE;
	}

	return FALSE;
}