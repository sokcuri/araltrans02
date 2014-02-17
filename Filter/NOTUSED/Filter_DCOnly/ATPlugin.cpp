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
//#include "Util.h"

/*
** 전역변수 선언부
*/

// 플러그인 버전
ATPLUGIN_VERSION g_pluginVer = {
	{ 0, 2, 20080816 },		// 플러그인 제작시 AT 버전
	{ 1, 0, 20080817 },		// 플러그인 버전
	"Hide_D",	// 제작자
	"D.C.2 Only",	// 플러그인 이름
	"다카포2 전용 필터입니다"	// 플러그인 설명
};

// 설정창 윈도우 핸들
HWND g_hSettingWnd=NULL;

// 이 플러그인의 모듈 핸들 = 인스턴스 핸들
HINSTANCE g_hThisModule;

// 옵션 버퍼 - 옵션이 바뀌었을 때 여기에 써주면 아랄트랜스 바로가기에도 등록할 수 있다.
// 주의 - 옵션길이는 최고 4096 바이트.
LPSTR g_szOptionStringBuffer=NULL;

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
	// TODO: 번역작업을 한다.

	lstrcpyA(szKorean, cszJapanese);

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
	size_t Len=strlen(cszInJapanese);
	size_t InN=0;
	size_t OutN=0;


	bool Ignore=false;
	while(InN<Len)
	{
		if(IsDBCSLeadByteEx(932,cszInJapanese[InN]))
		{
			if((BYTE)cszInJapanese[InN]==0x81&&(BYTE)cszInJapanese[InN+1]==0x6F)
			{
				Ignore=true;
				InN+=2;
			}
			else if((BYTE)cszInJapanese[InN]==0x81&&(BYTE)cszInJapanese[InN+1]==0x5E)
			{
				Ignore=false;
				InN+=2;
			}
			else if((BYTE)cszInJapanese[InN]==0x81&&(BYTE)cszInJapanese[InN+1]==0x70)
			{
				//얜 무시[..]
				InN+=2;
			}
			else if(!Ignore)
			{
				szOutJapanese[OutN++]=cszInJapanese[InN++];
				szOutJapanese[OutN++]=cszInJapanese[InN++];
			}
			else
			{
				InN+=2;
			}
		}
		else if(cszInJapanese[InN]!='\n' && !Ignore)
		{
			szOutJapanese[OutN++]=cszInJapanese[InN++];
		}
		else
		{
			InN++;
		}
	}
	szOutJapanese[OutN]='\0';
	
	// 작업 성공시 TRUE, 실패시 FALSE 반환 
	return TRUE;
}

BOOL  __stdcall PostTranslate(LPCSTR cszInKorean, LPSTR szOutKorean, int nBufSize)
{
	// TODO: 번역 후처리를 한다.
	// 특별한 처리를 하지 않더라도 문자열을 복사해 돌려주어야 한다.

	string Temp1=cszInKorean;
	string Temp2="";

	string::size_type Len1=Temp1.length();
	
	string::size_type SPoint1=0;
	int NLineChar=0;
	while(SPoint1<Len1){
		if(NLineChar>=22)
		{
			Temp2+='\n';
			NLineChar=0;
		}

		if(Temp1[SPoint1]==' '){
			//띄어쓰기 제거 =ㅅ=;;
			SPoint1++;
		}
		else if(IsDBCSLeadByteEx(949,Temp1[SPoint1]))
		{
			Temp2+=Temp1[SPoint1++];
			Temp2+=Temp1[SPoint1++];
			NLineChar++;
		}
		else
		{
			Temp2+=(char)0xA3;
			Temp2+=Temp1[SPoint1++]+(char)0x80;
			NLineChar++;
		}
	}

	lstrcpyA(szOutKorean, Temp2.c_str());
	
	// 작업 성공시 TRUE, 실패시 FALSE 반환 
	return TRUE;
}
#endif
