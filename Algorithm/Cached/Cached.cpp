// NonCached.cpp : Defines the entry point for the DLL application.
//

#include "Cached.h"
#include "CachedTextArgMgr.h"

#ifdef _MANAGED
#pragma managed(push, off)
#endif


CCachedTextArgMgr g_objCachedTextArgMgr;


extern "C" __declspec(dllexport) BOOL __stdcall OnPluginInit(HWND hSettingWnd, LPSTR cszOptionStringBuffer)
{
	return g_objCachedTextArgMgr.Init(hSettingWnd, cszOptionStringBuffer);
}

extern "C" __declspec(dllexport) BOOL __stdcall OnPluginOption()
{
	return g_objCachedTextArgMgr.Option();
}

extern "C" __declspec(dllexport) BOOL __stdcall OnPluginStart()
{
	return g_objCachedTextArgMgr.Start();
}

extern "C" __declspec(dllexport) BOOL __stdcall OnPluginStop()
{
	return g_objCachedTextArgMgr.Stop();
}

extern "C" __declspec(dllexport) BOOL __stdcall OnPluginClose()
{
	return g_objCachedTextArgMgr.Close();
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

	if( g_objCachedTextArgMgr.m_sTextFunc.pfnOrigWideCharToMultiByte )
	{
		nRetVal = g_objCachedTextArgMgr.m_sTextFunc.pfnOrigWideCharToMultiByte(
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

	if( g_objCachedTextArgMgr.m_sTextFunc.pfnOrigMultiByteToWideChar )
	{
		nRetVal = g_objCachedTextArgMgr.m_sTextFunc.pfnOrigMultiByteToWideChar(
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

