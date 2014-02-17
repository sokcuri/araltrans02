// ATPlugin.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "ATPlugin.h"

#include "tstring.h"
#include "Util.h"

#include <vector>

using namespace std;

// 설정창 윈도우 핸들
HWND g_hSettingWnd=NULL;

// 이 플러그인의 모듈 핸들 = 인스턴스 핸들
HINSTANCE g_hThisModule;

// 옵션 버퍼 - 옵션이 바뀌었을 때 여기에 써주면 아랄트랜스 바로가기에도 등록할 수 있다.
// 주의 - 옵션길이는 최고 4096 바이트.
LPSTR g_szOptionStringBuffer=NULL;

SYSTEMTIME st;
wchar_t wszFileName[MAX_PATH]=L"";

HANDLE hFileHandle=NULL;

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

	GetLocalTime(&st);

	//폴더 만들기
	int nTmpLne=swprintf_s(wszFileName,MAX_PATH,L"%s\\ATData\\Dump",GetGameDirectory());
	MyCreateDirectory(wszFileName);
	swprintf_s(wszFileName+nTmpLne,MAX_PATH-nTmpLne,L"\\Dump_%02d%02d%02d_%02d%02d%02d.txt",st.wYear%100,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);

	//파일 열기
	hFileHandle=CreateFileW(wszFileName,GENERIC_WRITE,FILE_SHARE_READ,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
	if(hFileHandle==INVALID_HANDLE_VALUE)
	{
		Sleep(1000);
		hFileHandle=CreateFileW(wszFileName,GENERIC_WRITE,FILE_SHARE_READ,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
		if(hFileHandle==INVALID_HANDLE_VALUE)
		{
			::MessageBoxW(NULL,L"파일 생성 실패",L"?",NULL);
			return FALSE;
		}
	}

	DWORD Tmp=0;
	WriteFile(hFileHandle,"\xFF\xFE",2,&Tmp,NULL);

	return TRUE;
}

BOOL  __stdcall OnPluginOption()
{

	::MessageBoxW(NULL,L"입력되는 데이터를 \r\nATData\\Dump\\Dump_일자_시간.txt\r\n 로 저장해주는 간이 필터입니다.\r\n\r\n-Hide_D-",L"Info",NULL);
	return TRUE;
}
BOOL  __stdcall OnPluginClose()
{
	// 주의: 여기서 메모리 확보 등을 하면 에러 발생.
	// TODO: 플러그인 종료 준비, 자원 반환 등을 한다.

	if(hFileHandle)
	{
		CloseHandle(hFileHandle);
	}

	return TRUE;
}

BOOL  __stdcall PreTranslate(LPCSTR cszInJapanese, LPSTR szOutJapanese, int nBufSize)
{

	lstrcpyA(szOutJapanese, cszInJapanese);
	
	vector<wchar_t> wstrTmp;
	wstrTmp.reserve(nBufSize*4+10);
	wstrTmp.resize(nBufSize);

	int nLen=MyMultiByteToWideChar(932,NULL,szOutJapanese,-1,&wstrTmp[0],nBufSize);

	wstrTmp.resize(nLen-1);
	wstrTmp.push_back(L'\r');
	wstrTmp.push_back(L'\n');

	LPSTR pszSrc=szOutJapanese;

	while(*pszSrc!='\0')
	{
		if(IsDBCSLeadByteEx(932,(BYTE)(*pszSrc)))
		{
			BYTE up = (BYTE)(*pszSrc)/16;
			BYTE down =(BYTE)(*pszSrc)%16;
			wstrTmp.push_back(up>=0xA?L'A'+up-0xA:L'0'+up);
			wstrTmp.push_back(down>=0xA?L'A'+down-0xA:L'0'+down);
			pszSrc++;
			if(*pszSrc=='\0')break;
		}
		BYTE up = (BYTE)(*pszSrc)/16;
		BYTE down =(BYTE)(*pszSrc)%16;
		wstrTmp.push_back(up>=0xA?L'A'+up-0xA:L'0'+up);
		wstrTmp.push_back(down>=0xA?L'A'+down-0xA:L'0'+down);
		wstrTmp.push_back(L' ');
		pszSrc++;
	}
	wstrTmp.push_back(L'\r');
	wstrTmp.push_back(L'\n');

	DWORD Tmp=0;

	WriteFile(hFileHandle,&wstrTmp[0],wstrTmp.size()*sizeof(wchar_t),&Tmp,NULL);
	FlushFileBuffers(hFileHandle);

	return TRUE;
}

BOOL  __stdcall PostTranslate(LPCSTR cszInKorean, LPSTR szOutKorean, int nBufSize)
{
	lstrcpyA(szOutKorean, cszInKorean);

	vector<wchar_t> wstrTmp;
	wstrTmp.reserve(nBufSize*4+10);
	wstrTmp.resize(nBufSize);

	int nLen=MyMultiByteToWideChar(949,NULL,szOutKorean,-1,&wstrTmp[0],nBufSize);

	wstrTmp.resize(nLen-1);
	wstrTmp.push_back(L'\r');
	wstrTmp.push_back(L'\n');

	LPSTR pszSrc=szOutKorean;

	while(*pszSrc!='\0')
	{
		if(IsDBCSLeadByteEx(949,(BYTE)(*pszSrc)))
		{
			BYTE up = (BYTE)(*pszSrc)/16;
			BYTE down =(BYTE)(*pszSrc)%16;
			wstrTmp.push_back(up>=0xA?L'A'+up-0xA:L'0'+up);
			wstrTmp.push_back(down>=0xA?L'A'+down-0xA:L'0'+down);
			pszSrc++;
			if(*pszSrc=='\0')break;
		}
		BYTE up = (BYTE)(*pszSrc)/16;
		BYTE down =(BYTE)(*pszSrc)%16;
		wstrTmp.push_back(up>=0xA?L'A'+up-0xA:L'0'+up);
		wstrTmp.push_back(down>=0xA?L'A'+down-0xA:L'0'+down);
		wstrTmp.push_back(L' ');
		pszSrc++;
	}
	wstrTmp.push_back(L'\r');
	wstrTmp.push_back(L'\n');

	DWORD Tmp=0;

	WriteFile(hFileHandle,&wstrTmp[0],wstrTmp.size()*sizeof(wchar_t),&Tmp,NULL);
	FlushFileBuffers(hFileHandle);

	return TRUE;
}
