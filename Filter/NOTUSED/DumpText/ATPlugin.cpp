// ATPlugin.cpp : Defines the entry point for the DLL application.
//
#include "stdafx.h"
#include "ATPlugin.h"

/*
** 전역변수 선언부
*/

bool gMode[5]={0,0,0,0,0};

// 플러그인 버전
ATPLUGIN_VERSION g_pluginVer = {
	{ 0, 2, 20080712 },		// 플러그인 제작시 AT 버전
	{ 0, 1, 20080801 },		// 플러그인 버전
	"Hide_D君",	// 제작자
	"DumpText",	// 플러그인 이름
	"원문,번역문 출력"	// 플러그인 설명
};

// 설정창 윈도우 핸들
HWND g_hSettingWnd=NULL;

// 텍스트창 윈도우 핸들
HWND hDumpText=NULL;
HWND _hDumpText=NULL;

// 이 플러그인의 모듈 핸들 = 인스턴스 핸들
HINSTANCE g_hThisModule;

// 옵션 버퍼 - 옵션이 바뀌었을 때 여기에 써주면 아랄트랜스 바로가기에도 등록할 수 있다.
// 주의 - 옵션길이는 최고 4096 바이트.
LPSTR g_szOptionStringBuffer=NULL;

LPSTR ConvertHex(LPCSTR source,LPSTR complete,int nCodePage);
void DumpText(LPCSTR source,int Type);
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

	ATOPTION_ARRAY aOptions;
	GetATOptionsFromOptionString(cszOptionStringBuffer, aOptions);

	for (unsigned int i=0; i<aOptions.size(); i++)
	{
		// SET(1100) 같은 느낌으로
		if ( !lstrcmpi(aOptions[i].strKey.c_str(), _T("SET")) )
		{
			if(aOptions[i].strValue[0]==_T('1'))
				gMode[0]=true;
			else
				gMode[0]=false;

			if(aOptions[i].strValue[1]==_T('1'))
				gMode[1]=true;
			else
				gMode[1]=false;

			if(aOptions[i].strValue[2]==_T('1'))
				gMode[2]=true;
			else
				gMode[2]=false;

			if(aOptions[i].strValue[3]==_T('1'))
				gMode[3]=true;
			else
				gMode[3]=false;

			if(aOptions[i].strValue[4]==_T('1'))
				gMode[4]=true;
			else
				gMode[4]=false;
		}
	}

	//창 띄우기
	hDumpText=CreateDialog((HINSTANCE)g_hThisModule,MAKEINTRESOURCE(IDD_Window),_hDumpText,DumpTextProc);
	ShowWindow(hDumpText,SW_SHOW);

	//Start명령
	SendMessage(hDumpText, WM_USER+1, 0, 0);
	return TRUE;
}

BOOL  __stdcall OnPluginOption()
{
	if (g_hSettingWnd && IsWindow(g_hSettingWnd))
	{
		// TODO: 플러그인 옵션창 처리 및 옵션 조정을 한다.

		if(DialogBox((HINSTANCE)g_hThisModule, MAKEINTRESOURCE(IDD_Option), g_hSettingWnd, OptionDialogProc)==IDOK){
			ApplySetting();
		}
	}
	return TRUE;
}
BOOL  __stdcall OnPluginClose()
{
	// 주의: 여기서 메모리 확보 등을 하면 에러 발생.
	// TODO: 플러그인 종료 준비, 자원 반환 등을 한다.

	// 열었으면 닫아야지 =ㅅ=
	DestroyWindow(hDumpText);
	hDumpText=NULL;
	return TRUE;
}
const ATPLUGIN_VERSION * __stdcall OnPluginVersion()
{
	// 플러그인 버전 반환

	return &g_pluginVer;
}

BOOL  __stdcall PreTranslate(LPCSTR cszInJapanese, LPSTR szOutJapanese, int nBufSize)
{
	// TODO: 번역 전처리를 한다.
	// 특별한 처리를 하지 않더라도 문자열을 복사해 돌려주어야 한다.
	lstrcpyA(szOutJapanese, cszInJapanese);
	
	char temp[2048];

	if(cszInJapanese[0]!='\0')
	{
		if(gMode[0])
			DumpText(cszInJapanese,0);
		if(gMode[1])
			DumpText(ConvertHex(cszInJapanese,temp,932),1);
	}
	else
		szOutJapanese[0]='\0';

	// 작업 성공시 TRUE, 실패시 FALSE 반환 
	return TRUE;
}

BOOL  __stdcall PostTranslate(LPCSTR cszInKorean, LPSTR szOutKorean, int nBufSize)
{
	// TODO: 번역 후처리를 한다.
	// 특별한 처리를 하지 않더라도 문자열을 복사해 돌려주어야 한다.
	lstrcpyA(szOutKorean, cszInKorean);
	
	char temp[2048];
	if(cszInKorean[0]!='\0')
	{
		if(gMode[2])
			DumpText(cszInKorean,2);
		if(gMode[3])
			DumpText(ConvertHex(cszInKorean,temp,949),3);
	}
	else
		szOutKorean[0]='\0';

	// 작업 성공시 TRUE, 실패시 FALSE 반환 
	return TRUE;
}

LPSTR ConvertHex(LPCSTR source,LPSTR Temp, int nCodePage)
{
	size_t Len=strlen(source);

	BYTE szBuf[3] = { '\0', };

	int j=0;

	for(size_t i=0;i<Len;i++){
		szBuf[0]=(BYTE) source[i];
		szBuf[1]=(BYTE) source[i+1];
		if(!IsDBCSLeadByteEx(nCodePage,szBuf[0]))//1바이트 코드라면
		{
			j += sprintf_s(Temp+j,2048,"%02X ",szBuf[0]);
		}
		else{ //2바이트라면
			j += sprintf_s(Temp+j,2048,"%02X%02X ",szBuf[0],szBuf[1]);
			i++;
		}
	}
	return Temp;
}

void DumpText(LPCSTR source,int Type){
	wchar_t Temp[2048]=L"";
	int T=strlen(source);
	if(Type==0){
		MyMultiByteToWideChar(932,0,source,-1,Temp,2048);
	}
	else if(Type==2){
		MyMultiByteToWideChar(949,0,source,-1,Temp,2048);
	}
	else{
		for(int i=0;i<T;i++){
			Temp[i]=source[i];
		}
	}
	SendMessage(hDumpText, WM_USER, 0, (LPARAM)Temp);
}

void ApplySetting(){
	ATOPTION_ARRAY aOptions;
	ATOPTION_PAIR option;


	option.strKey=L"SET";

	if(gMode[0]==0)
		option.strValue.push_back(L'0');
	else
		option.strValue.push_back(L'1');

	if(gMode[1]==0)
		option.strValue.push_back(L'0');
	else
		option.strValue.push_back(L'1');

	if(gMode[2]==0)
		option.strValue.push_back(L'0');
	else
		option.strValue.push_back(L'1');

	if(gMode[3]==0)
		option.strValue.push_back(L'0');
	else
		option.strValue.push_back(L'1');

	if(gMode[4]==0)
		option.strValue.push_back(L'0');
	else
		option.strValue.push_back(L'1');
	aOptions.push_back(option);
	
	SendMessage(hDumpText, WM_USER+1, 0, (LPARAM)gMode[4]);

	GetOptionStringFromATOptions(aOptions, g_szOptionStringBuffer, 4096);
}