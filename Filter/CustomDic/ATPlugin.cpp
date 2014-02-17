// CustomDic.cpp : DLL 응용 프로그램을 위해 내보낸 함수를 정의합니다.
//
#include "stdafx.h"
#include "ATPlugin.h"
#include "CustomDic.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CustomDic theApp;
volatile LONG g_pHwnd=0;

extern "C" BOOL PASCAL EXPORT OnPluginInit(HWND hSettingWnd, LPSTR szOptionStringBuffer)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	InterlockedExchange(&g_pHwnd,(LONG)hSettingWnd);
	
	if(strlen(szOptionStringBuffer)!=0){
		theApp.Migration(szOptionStringBuffer);
		szOptionStringBuffer[0]='\0';
	}
	else{
		theApp.Initialize();
	}
	return TRUE;
}

extern "C" BOOL PASCAL EXPORT OnPluginOption()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	theApp.Option();
	return true;
}

extern "C" BOOL PASCAL EXPORT OnPluginClose()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return theApp.Close();
}

extern "C" BOOL PASCAL EXPORT PreTranslate(LPCSTR cszInJapanese, LPSTR szOutJapanese, int nBufSize)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	const string strIn=cszInJapanese;
	string strOut;
	bool bRet=theApp.PreTranslate(strIn,strOut);
	strcpy_s(szOutJapanese,nBufSize,strOut.c_str());
	return bRet;
}

extern "C" BOOL PASCAL EXPORT PostTranslate(LPCSTR cszInKorean, LPSTR szOutKorean, int nBufSize)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	const string strIn=cszInKorean;
	string strOut;
	bool bRet=theApp.PostTranslate(strIn,strOut);
	strcpy_s(szOutKorean,nBufSize,strOut.c_str());
	return bRet;
}