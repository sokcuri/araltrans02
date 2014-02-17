// NonCached.cpp : Defines the entry point for the DLL application.
//

#include "NonCached.h"
#include "NonCachedTextArgMgr.h"

#ifdef _MANAGED
#pragma managed(push, off)
#endif

BOOL g_bMatchLen=FALSE;

CNonCachedTextArgMgr g_objNonCachedTextArgMgr;


/*
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
    return TRUE;
}
*/

extern "C" __declspec(dllexport) BOOL __stdcall OnPluginInit(HWND hSettingWnd, LPSTR cszOptionStringBuffer)
{
	return g_objNonCachedTextArgMgr.Init(hSettingWnd, cszOptionStringBuffer);
}

extern "C" __declspec(dllexport) BOOL __stdcall OnPluginOption()
{
	return g_objNonCachedTextArgMgr.Option();
}

extern "C" __declspec(dllexport) BOOL __stdcall OnPluginStart()
{
	return g_objNonCachedTextArgMgr.Start();
}

extern "C" __declspec(dllexport) BOOL __stdcall OnPluginStop()
{
	return g_objNonCachedTextArgMgr.Stop();
}

extern "C" __declspec(dllexport) BOOL __stdcall OnPluginClose()
{
	return g_objNonCachedTextArgMgr.Close();
}




int MyWideCharToMultiByte(
						  UINT CodePage, 
						  DWORD dwFlags, 
						  LPCWSTR lpWideCharStr, 
						  int cchWideChar, 
						  LPSTR lpMultiByteStr, 
						  int cbMultiByte, 
						  LPCSTR lpDefaultChar, 
						  LPBOOL lpUsedDefaultChar 
						  )
{
	int nRetVal = 0;

	if( g_objNonCachedTextArgMgr.m_sTextFunc.pfnOrigWideCharToMultiByte )
	{
		nRetVal = g_objNonCachedTextArgMgr.m_sTextFunc.pfnOrigWideCharToMultiByte(
			CodePage, 
			dwFlags, 
			lpWideCharStr, 
			cchWideChar, 
			lpMultiByteStr, 
			cbMultiByte, 
			lpDefaultChar, 
			lpUsedDefaultChar 
			);
	}
	else
	{
		nRetVal = ::WideCharToMultiByte(
			CodePage, 
			dwFlags, 
			lpWideCharStr, 
			cchWideChar, 
			lpMultiByteStr, 
			cbMultiByte, 
			lpDefaultChar, 
			lpUsedDefaultChar 
			);
	}

	return nRetVal;
}

int MyMultiByteToWideChar(
						  UINT CodePage, 
						  DWORD dwFlags, 
						  LPCSTR lpMultiByteStr, 
						  int cbMultiByte, 
						  LPWSTR lpWideCharStr, 
						  int cchWideChar 
						  )
{
	int nRetVal = 0;

	if( g_objNonCachedTextArgMgr.m_sTextFunc.pfnOrigMultiByteToWideChar )
	{
		nRetVal = g_objNonCachedTextArgMgr.m_sTextFunc.pfnOrigMultiByteToWideChar(
			CodePage, 
			dwFlags, 
			lpMultiByteStr, 
			cbMultiByte, 
			lpWideCharStr, 
			cchWideChar 
			);
	}
	else
	{
		nRetVal = ::MultiByteToWideChar(
			CodePage, 
			dwFlags, 
			lpMultiByteStr, 
			cbMultiByte, 
			lpWideCharStr, 
			cchWideChar 
			);
	}

	return nRetVal;
}


#ifdef _MANAGED
#pragma managed(pop)
#endif

