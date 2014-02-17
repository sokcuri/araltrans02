// ATPlugin.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "ATPlugin.h"

// ATCApi 사용하려면 include
//#include "ATCApi.h"

// Util 사용하려면 include
//#include "Util.h"

#include "tstring.h"

#pragma warning(disable:4305 4309)

/*
** 전역변수 선언부
*/

// 플러그인 버전
ATPLUGIN_VERSION g_pluginVer = {
	{ 0, 2, 20080704 },		// 플러그인 제작시 AT 버전
	{ 0, 1, 20080726 },		// 플러그인 버전
	"whoami",	// 제작자
	"preHK2FK",	// 플러그인 이름
	"Shift-JIS 반각-전각 변환 필터"	// 플러그인 설명
//	.12345678901234567890123456789012
};

// 설정창 윈도우 핸들
HWND g_hSettingWnd=NULL;

// 이 플러그인의 모듈 핸들 = 인스턴스 핸들
HINSTANCE g_hThisModule;

// 옵션 버퍼 - 옵션이 바뀌었을 때 여기에 써주면 아랄트랜스 바로가기에도 등록할 수 있다.
// 주의 - 옵션길이는 최고 4096 바이트.
LPSTR g_szOptionStringBuffer=NULL;


// HK2FK
void HK2FK(const char *pszIn, char *pszOut);

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
	// ATOPTION_ARRAY aOptions;
	// GetATOptionsFromOptionString(cszOptionStringBuffer, aOptions);

	// 초기화 성공시 TRUE, 실패시 FALSE를 반환
	return TRUE;
}

BOOL  __stdcall OnPluginOption()
{
	if (g_hSettingWnd && IsWindow(g_hSettingWnd))
	{
		// TODO: 플러그인 옵션창 처리 및 옵션 조정을 한다.

		char szTemp[1024];
		wsprintfA(szTemp, "%s : %s\r\n\r\n제작자 %s, 플러그인 버전 %d.%d.%d\r\n\r\n필요 AT 버전 %d.%d.%d\r\n",
			g_pluginVer.szPluginName, g_pluginVer.szDescription,
			g_pluginVer.szAuthor, 
			g_pluginVer.PluginVersion.Major, g_pluginVer.PluginVersion.Minor, g_pluginVer.PluginVersion.BuildDate,
			g_pluginVer.ATVersion.Major, g_pluginVer.ATVersion.Minor, g_pluginVer.ATVersion.BuildDate);

#ifdef _UNICODE
		WCHAR wszTemp[1024], wszName[16];
		Kor2Wide(szTemp, wszTemp, 1024);
		Kor2Wide(g_pluginVer.szPluginName, wszName, 16);
		MessageBox(g_hSettingWnd, wszTemp, wszName, MB_OK);
#else
		MessageBox(g_hSettingWnd, szTemp, g_pluginVer.szPluginName, MB_OK);
#endif

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

	HK2FK(cszJapanese, szKorean);

	// 작업 성공시 TRUE, 실패시 FALSE 반환 
	return TRUE;
}
#endif

// 필터 플러그인 전용
#ifdef ATPLUGIN_FILTER
BOOL  __stdcall PreTranslate(LPCSTR cszInJapanese, LPSTR szOutJapanese, int nBufSize)
{
	// TODO: 번역 전처리를 한다.
	// 특별한 처리를 하지 않더라도 문자열을 복사해 돌려주어야 한다.
	HK2FK(cszInJapanese, szOutJapanese);
	
	// 작업 성공시 TRUE, 실패시 FALSE 반환 
	return TRUE;
}

BOOL  __stdcall PostTranslate(LPCSTR cszInKorean, LPSTR szOutKorean, int nBufSize)
{
	// TODO: 번역 후처리를 한다.
	// 특별한 처리를 하지 않더라도 문자열을 복사해 돌려주어야 한다.
	wsprintfA(szOutKorean, "%s", cszInKorean);
	
	// 작업 성공시 TRUE, 실패시 FALSE 반환 
	return TRUE;
}
#endif


void HK2FK(const char *pszIn, char *pszOut)
{
	const char szHK2FKTable[][2] = {
		{0x81, 0x40}, {0x81, 0x42}, {0x81, 0x75}, {0x81, 0x76}, 	 // 0xA0... 0xA3
		{0x81, 0x41}, {0x81, 0x45}, {0x83, 0x92}, {0x83, 0x40}, 	 // 0xA4... 0xA7
		{0x83, 0x42}, {0x83, 0x44}, {0x83, 0x46}, {0x83, 0x48}, 	 // 0xA8... 0xAB
		{0x83, 0x83}, {0x83, 0x85}, {0x83, 0x87}, {0x83, 0x62}, 	 // 0xAC... 0xAF
		{0x81, 0x5B}, {0x83, 0x41}, {0x83, 0x43}, {0x83, 0x45}, 	 // 0xB0... 0xB3
		{0x83, 0x47}, {0x83, 0x49}, {0x83, 0x4A}, {0x83, 0x4C}, 	 // 0xB4... 0xB7
		{0x83, 0x4E}, {0x83, 0x50}, {0x83, 0x52}, {0x83, 0x54}, 	 // 0xB8... 0xBB
		{0x83, 0x56}, {0x83, 0x58}, {0x83, 0x5A}, {0x83, 0x5C}, 	 // 0xBC... 0xBF
		{0x83, 0x5E}, {0x83, 0x60}, {0x83, 0x63}, {0x83, 0x65}, 	 // 0xC0... 0xC3
		{0x83, 0x67}, {0x83, 0x69}, {0x83, 0x6A}, {0x83, 0x6B}, 	 // 0xC4... 0xC7
		{0x83, 0x6C}, {0x83, 0x6D}, {0x83, 0x6E}, {0x83, 0x71}, 	 // 0xC8... 0xCB
		{0x83, 0x74}, {0x83, 0x77}, {0x83, 0x7A}, {0x83, 0x7D}, 	 // 0xCC... 0xCF
		{0x83, 0x7E}, {0x83, 0x80}, {0x83, 0x81}, {0x83, 0x82}, 	 // 0xD0... 0xD3
		{0x83, 0x84}, {0x83, 0x86}, {0x83, 0x88}, {0x83, 0x89}, 	 // 0xD4... 0xD7
		{0x83, 0x8A}, {0x83, 0x8B}, {0x83, 0x8C}, {0x83, 0x8D}, 	 // 0xD8... 0xDB
		{0x83, 0x8F}, {0x83, 0x93}, {0x81, 0x4A}, {0x81, 0x4B}  	 // 0xDC... 0xDF
	};
	
	
	int idxIn, idxOut;
	int nInLen;
	BYTE chNow;
	nInLen=lstrlenA(pszIn);
	
	idxIn=idxOut=0;
	
	while(idxIn < nInLen)
	{
		// 글자 받아옴
		chNow=(BYTE)pszIn[idxIn];
		
		// 글자 = LeadByte?
		if (IsDBCSLeadByteEx(932, chNow))
//		if ( ((0x81 <= chNow) && (chNow < 0xA0)) || ((0xE0 <=chNow) && (chNow < 0xFF)) )
		{
			// LeadByte 이면 2자 복사
			pszOut[idxOut]=pszIn[idxIn];
			pszOut[idxOut+1]=pszIn[idxIn+1];
			
			// index 보정
			idxIn+=2;
			idxOut+=2;
		}
		else if( (0xA0 <=chNow) && (chNow < 0xE0) )
		{
			// 글자 = 반각 카나
			chNow-=0xA0;
			pszOut[idxOut]=szHK2FKTable[chNow][0];
			pszOut[idxOut+1]=szHK2FKTable[chNow][1];

			// 탁음, 반탁음 처리
			chNow=(BYTE)pszIn[idxIn+1];
			if ( chNow == 0xDE && (BYTE)pszIn[idxIn] != 0xB3)
			{
				// 탁음
				pszOut[idxOut+1]++;
				idxIn+=2;
			}
			else if ( chNow == 0xDF)
			{
				// 반탁음
				pszOut[idxOut+1]+=2;
				idxIn+=2;
			}
			else if ( chNow == 0xDE && (BYTE)pszIn[idxIn] == 0xB3)
			{
				// 예외 탁음
				pszOut[idxOut+1]+=79;
				idxIn+=2;
			}
			else
			{
				// 일반
				idxIn++;
			}
			idxOut+=2;
		}
		else
		{
			// 일반 ASCII
			pszOut[idxOut]=pszIn[idxIn];
			idxIn++;
			idxOut++;
		}
		
	}	// while(...)
	pszOut[idxOut]='\0';
}