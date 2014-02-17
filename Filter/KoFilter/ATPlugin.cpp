// ATPlugin.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "ATPlugin.h"

#ifdef _UNICODE
#include "tstring.h"
#endif

// ATCApi 사용하려면 include
//#include "ATCApi.h"

// Util 사용하려면 include
#include "Util.h"

#include "resource.h"
#include "OptionDialog.h"
#include "FileOptionParser.h"
#include "SubFunc.h"
#include "hash.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <set>

using namespace std;


/*
** 전역변수 선언부
*/

// 기본 번역 레벨 = 한글 > 일어x2
#define LEVEL_DEFAULT 10

// 번역 레벨
int g_nLevel=LEVEL_DEFAULT;

// 특수문자 사용 여부
bool g_bNoRoman = false;
bool g_bNoRound = false;

// 플러그인 버전
ATPLUGIN_VERSION g_pluginVer = {
	{ 0, 2, 20081207 },		// 플러그인 제작시 AT 버전
	{ 1, 0, 20090114 },		// 플러그인 버전
	"whoami",	// 제작자
	"KoFilter",	// 플러그인 이름
	"한글 중복번역 방지 플러그인"	// 플러그인 설명
};

// 설정창 윈도우 핸들
HWND g_hSettingWnd=NULL;

// 이 플러그인의 모듈 핸들 = 인스턴스 핸들
HINSTANCE g_hThisModule;

// 옵션 버퍼 - 옵션이 바뀌었을 때 여기에 써주면 아랄트랜스 바로가기에도 등록할 수 있다.
// 주의 - 옵션길이는 최고 4096 바이트.
LPSTR g_szOptionStringBuffer=NULL;

bool g_bLogJp=false;
bool g_bLogKo=false;

set <UINT> g_Deny;
set <UINT> g_Allow;

int g_nAllow = 0;
int g_nDeny = 0;

int IsDenyAllow(const char *pszTestString);
BOOL IsKorean(const char *pszTestString, bool bCheckEncodeKor=true);
BOOL DecodeJ2K(LPCSTR cszJpnCode, LPSTR szKorCode);

void OpenDenyAllow();
void ApplySetting();

// DLLMain
// 자체 리소스가 있는 경우 모듈 핸들을 알아오기 위해 필요
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			g_hThisModule = (HINSTANCE) hModule;
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}

// 플러그인 export 함수

// 공통
BOOL  __stdcall OnPluginInit(HWND hSettingWnd, LPSTR cszOptionStringBuffer)
{
	g_hSettingWnd = hSettingWnd;
	g_szOptionStringBuffer=cszOptionStringBuffer;

	// TODO : 플러그인의 초기화, 자원 확보, 옵션 분석등을 한다.

	// ATCApi 사용시
	// if ( !g_cATCApi.IsValid() ) return FALSE;

	// Util 의 옵션 파서 사용 예
	ATOPTION_ARRAY aOptions;
	GetATOptionsFromOptionString(cszOptionStringBuffer, aOptions);

	for (int i=0; i<aOptions.size(); i++)
	{
		if ( !lstrcmpi(aOptions[i].strKey.c_str(), _T("LOG")) )
		{
			if ( !lstrcmpi(aOptions[i].strValue.c_str(), _T("JP")) )
				g_bLogJp=true;
			else if ( !lstrcmpi(aOptions[i].strValue.c_str(), _T("KO")) )
				g_bLogKo=true;

			FILE *fp=fopen("c:\\KoFilter.log", "w");
			fclose(fp);
		}
		else if ( !lstrcmpi(aOptions[i].strKey.c_str(), _T("LEVEL")) )
			g_nLevel= _ttoi(aOptions[i].strValue.c_str());
		else if ( !lstrcmpi(aOptions[i].strKey.c_str(), _T("NOROMAN")) )
			g_bNoRoman=true;
		else if ( !lstrcmpi(aOptions[i].strKey.c_str(), _T("NOROUND")) )
			g_bNoRound=true;

	}

	g_cOption.SetOptionFile(L"ATData\\KoFilter.dat");

	OpenDenyAllow();
	// 초기화 성공시 TRUE, 실패시 FALSE를 반환
	return TRUE;
}

BOOL  __stdcall OnPluginOption()
{
	OpenDenyAllow();
	if (g_hSettingWnd && IsWindow(g_hSettingWnd))
	{
		// TODO: 플러그인 옵션창 처리 및 옵션 조정을 한다.

		if (DialogBox((HINSTANCE)g_hThisModule, MAKEINTRESOURCE(IDD_OPTIONDIALOG), g_hSettingWnd, OptionDialogProc) == IDOK)
			ApplySetting();
		
		// Util 의 옵션 파서 사용 예
		// ATOPTION_ARRAY aOptions;
		// ReadOptionsFromSomewhere(aOptions); // 어단가에서 옵션을 읽어온다
		// GetOptionStringFromATPluginArgs(aOptions, g_szOptionStringBuffer, 4096);	// 옵션을 문자열로 바꿔 아랄트랜스로 넘긴다.

	}
	return TRUE;
}
BOOL  __stdcall OnPluginClose()
{
	// 주의: 여기서 메모리 확보 등을 하면 에러 발생.
	// TODO: 플러그인 종료 준비, 자원 반환 등을 한다.

	return TRUE;
}
const ATPLUGIN_VERSION * __stdcall OnPluginVersion()
{
	// 플러그인 버전 반환

	return &g_pluginVer;
}

// 후킹 플러그인 전용
#ifdef ATPLUGIN_HOOKER
BOOL  __stdcall OnPluginStart()
{
	// TODO: 후킹 시작 작업.

	// 작업 성공시 TRUE, 실패시 FALSE 반환 
	return TRUE;
}
BOOL  __stdcall OnPluginStop()
{
	// 주의: 여기서 메모리 확보 등을 하면 에러 발생.
	// TODO: 후킹 종료 작업.

	// 작업 성공시 TRUE, 실패시 FALSE 반환 
	return TRUE;
}
#endif

// 번역 플러그인 전용
#ifdef ATPLUGIN_TRANSLATOR
BOOL  __stdcall Translate(LPCSTR cszJapanese, LPSTR szKorean, int nBufSize)
{
	// TODO: 번역작업을 한다.

	lstrcpyA(szKorean, cszJapanese);

	if (IsKorean(cszJapanese))
		return FALSE;
		return TRUE;
}
#endif

// 필터 플러그인 전용
#ifdef ATPLUGIN_FILTER
BOOL  __stdcall PreTranslate(LPCSTR cszInJapanese, LPSTR szOutJapanese, int nBufSize)
{
	// TODO: 번역 전처리를 한다.
	// 특별한 처리를 하지 않더라도 문자열을 복사해 돌려주어야 한다.
	lstrcpyA(szOutJapanese, cszInJapanese);
	int i = IsDenyAllow(cszInJapanese);
	if (i!=0)
	{
		if(i==1)
			return FALSE;
		else
			return TRUE;
	}
	else if (IsKorean(cszInJapanese))
		return FALSE;
	return TRUE;
}

BOOL  __stdcall PostTranslate(LPCSTR cszInKorean, LPSTR szOutKorean, int nBufSize)
{
	// TODO: 번역 후처리를 한다.
	// 특별한 처리를 하지 않더라도 문자열을 복사해 돌려주어야 한다.
	lstrcpyA(szOutKorean, cszInKorean);
	
	// 작업 성공시 TRUE, 실패시 FALSE 반환 
	return TRUE;
}
#endif

void OpenDenyAllow()
{
	g_nAllow = 0;
	g_nDeny = 0;

	g_cOption.Clear();
	g_cOption.OpenOptionFile();

	g_Allow.clear();
	g_Deny.clear();

	//Deny
	for(int i=1;;i++)
	{
		if(g_cOption.IsInData(L"Deny",i))
		{
			g_Deny.insert(MakeStringHash(wtoa(g_cOption.GetOptionString(L"Deny",i)).c_str()));
			g_nDeny++;
		}
		else
			break;
	}

	for(int i=1;;i++)
	{
		if(g_cOption.IsInData(L"Allow",i))
		{
			g_Allow.insert(MakeStringHash(wtoa(g_cOption.GetOptionString(L"Allow",i)).c_str()));
			g_nAllow++;
		}
		else
			break;
	}
}

int IsDenyAllow(const char *pszTestString)
{
	UINT uHash = MakeStringHash(pszTestString);

	if(g_Deny.count(uHash)!=0)
	{
		return 1;
	}
	else if(g_Allow.count(uHash)!=0)
	{
		return 2;
	}
	else
		return 0;
}

BOOL IsKorean(const char *pszTestString, bool bCheckEncodeKor)
{
	volatile BYTE szBuf[3] = { '\0', };
	WORD wch;
	int nKoCount, nJpCount;

	int i, nLen;

	BOOL bIsKorean;

	nLen=lstrlenA(pszTestString);

	// 기본 체크 루틴
	nKoCount=nJpCount=0;
	for (i=0; i<nLen; i++)
	{
		bIsKorean=FALSE;	// 여기서는 각 글자의 한글판정

		// 체크 바이트
		szBuf[0]=(BYTE) pszTestString[i];
		szBuf[1]=(BYTE) pszTestString[i+1];

		wch=MAKEWORD(pszTestString[i+1], pszTestString[i]);

		if (szBuf[0] < 0x80)	// 1바이트 코드
			continue;	// 무시

		if (bCheckEncodeKor && (					// EncodeKor 코드범위 (CCharacterMapper::EncodeK2J from ATCode)
			( (0x889F <= wch) && (wch <= 0x9872) ) ||	// 0x889F - 0x9872,
			( (0x989F <= wch) && (wch <= 0x9FFC) ) ||	// 0x989F - 0x9FFC,
			( (0xE040 <= wch) && (wch <= 0xEAA4) ) ||	// 0xE040 - 0xEAA4,
			( (0xED40 <= wch) && (wch <= 0xEEEC) )		// 0xED40 - 0xEEEC
			))
		{
			// 한글인가? (EncodeKor)

			// Decode 해본다
			DecodeJ2K(pszTestString+i, (char *)szBuf);
		}

		// KSC-5601 : 0xA1A1 - 0xA1FE
		//   코드        -        -
		//   범위     0xFEA1 - 0xFEFE
		// 첫번째 바이트 
		// 0xA1 : 구둣점, 각종 괄호
		// 0xA2 : 기호 (화살표, 하트 등)
		// 0xA3 : 전각 0-9A-Za-z등
		// 0xA4 : 자음, 모음, 고어
		// 0xA5 : 로마문자, 로마숫자
		// 0xA6 : 선문자 (표 등)
		// 0xA7 : 도량형
		// 0xA8 : 원문자
		// 0xA9 : 괄호문자
		// 0xAA : 히라가나
		// 0xAB : 카타카나
		// 0xAC : 러시아어
		// 0xAD-0xAF : 기타
		// 0xB0-0xC8 : 한글 (가-힝)
		// 0xC9 : 기타
		// 0xCA-0xFD : 한자 (伽-詰)
		// 0xFE : 기타
		// 두번째 바이트
		// 0xA1-0xFE 사이만 유효

		// 1. 한글 체크 (0xB0-0xC8)
		if (((0xB0 <= szBuf[0])&&(szBuf[0] <= 0xC8)) && 
			((0xA1 <= szBuf[1])&&(szBuf[1] <= 0xFE)))
			bIsKorean=TRUE;
		// 2. 특수문자 - 구둣점 (0xA1), 전각 숫자/영문자(0xA3)
		else if (((0xA1 == szBuf[0])||(0xA3 == szBuf[0])) && 
			((0xA1 <= szBuf[1])&&(szBuf[1] <= 0xFE)))
			bIsKorean=TRUE;
		// 3. 로마숫자
		else if (!g_bNoRoman && (0xA5 == szBuf[0]) &&
			( ((0xA1 <= szBuf[1])&&(szBuf[1] <= 0xAA)) ||	// ⅰ-ⅹ
			 ((0xB0 <= szBuf[1])&&(szBuf[1] <= 0xB9)) ) 	// Ⅰ-Ⅹ
			)
			bIsKorean=TRUE;
		// 4. 원문자
		else if (!g_bNoRound && (0xA8 == szBuf[0]) &&
			((0xE7 <= szBuf[1])&&(szBuf[1] <= 0xF5))	// ① - ⑮
			)
			bIsKorean=TRUE;

		if (bIsKorean)
		{
			// 한글
			nKoCount++;
			i++; // 이미 체크했으므로 다음 문자로 넘어감	
		}
		else
		{
			// 일어
			nJpCount++;
			if (IsDBCSLeadByteEx(932, pszTestString[i]))
				i++;
		}
		// 기타 글자는 무시
	}

	// 최종 판정
	bIsKorean=FALSE;	// 여기서부터는 전체 문장의 한글 판정
	switch(g_nLevel)
	{
		case 0: // 일어가 1자라도 있으면 번역
			if (!nJpCount)
				bIsKorean=TRUE;
			break;

		case 5:	// 한글 > 일어 일때 번역 중지
			if (nKoCount > nJpCount)
				bIsKorean=TRUE;
			break;

		case 10:	// 한글 > 일어x2 일때 번역 중지
			if (nKoCount > nJpCount*2)
				bIsKorean=TRUE;
			break;

		case 15:	// 한글 > 일어x3 일때 번역 중지
			if (nKoCount > nJpCount*3)
				bIsKorean=TRUE;
			break;

		case 20:	// 한글이 1자라도 있으면 번역 중지
			if (nKoCount)
				bIsKorean=TRUE;
			break;

		default:
			if (nKoCount > nJpCount*2)
				bIsKorean=TRUE;

	}

	if ( (g_bLogKo && bIsKorean) || (g_bLogJp && !bIsKorean) )
	{
		FILE *fp=fopen("c:\\KoFilter.log", "a");
		
		int nSize = lstrlenA(pszTestString);
		
		if (bIsKorean)
			fprintf(fp, "Korean : %s\n", pszTestString);
		else
			fprintf(fp, "Japanese : %s\n", pszTestString);

		fprintf(fp, "Decode : ");
		
		for(int i=0; i<nSize; i++)
		{
			if ((BYTE)pszTestString[i] > 0x80)
			{
				DecodeJ2K(pszTestString+i, (char *)szBuf);
				fprintf(fp,"%s", szBuf);
				i++;
			}
			else
				fprintf(fp, "%c", pszTestString[i]);
		}
		fprintf(fp, "\nCount(Ko/Jp) = (%d/%d), Level = %d\n", nKoCount, nJpCount, g_nLevel);
		fclose(fp);
	}

	return bIsKorean;

}

BOOL DecodeJ2K(LPCSTR cszJpnCode, LPSTR szKorCode)
{
	BOOL bRetVal = FALSE;

	// 889F~9872, 989F~9FFC, E040~EAA4, ED40~EEEC
	// 95~3074, 3119~4535, 16632~18622, 19089~19450
	if(cszJpnCode && szKorCode && 0x88 <= (BYTE)cszJpnCode[0] && (BYTE)cszJpnCode[0] <= 0xEE)
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

		//TRACE("[ aral1 ] DecodeJ2K : (%p) -> (%p)'%s' \n", (UINT_PTR)(MAKEWORD(cszJpnCode[1], cszJpnCode[0])), (UINT_PTR)(MAKEWORD(szKorCode[1], szKorCode[0])), szKorCode);

		if( 0xA0 <= (BYTE)szKorCode[0] && 0xA0 <= (BYTE)szKorCode[1] )
		{
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

			bRetVal = TRUE;
		}
	}

	return bRetVal;
}

void ApplySetting()
{
	ATOPTION_ARRAY aOptions;
	ATOPTION_PAIR option;

	if (g_nLevel != LEVEL_DEFAULT)
	{
		TCHAR szLevel[10];
		option.strKey=_T("LEVEL");
		option.strValue=_itot(g_nLevel, szLevel, 10);
		aOptions.push_back(option);
	}
	if (g_bLogJp)
	{
		option.strKey=_T("LOG");
		option.strValue=_T("JP");
		aOptions.push_back(option);
	}
	if (g_bLogKo)
	{
		option.strKey=_T("LOG");
		option.strValue=_T("KO");
		aOptions.push_back(option);
	}
	if (g_bNoRoman)
	{
		option.strKey=_T("NOROMAN");
		option.strValue.erase();
		aOptions.push_back(option);
	}
	if (g_bNoRound)
	{
		option.strKey=_T("NOROUND");
		option.strValue.erase();
		aOptions.push_back(option);
	}
	GetOptionStringFromATOptions(aOptions, g_szOptionStringBuffer, 4096);
}