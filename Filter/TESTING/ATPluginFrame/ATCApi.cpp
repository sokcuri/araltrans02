// ATCApi.cpp : 아랄 트랜스 컨테이너 API Wrapper

#include "StdAfx.h"
#include <tchar.h>

#include "ATCApi.h"

CATCApi g_cATCApi;

CATCApi::CATCApi() : m_hContainer(NULL)
{
	ZeroMemory(&m_cEntry, sizeof(m_cEntry));

	try 
	{
		// ATCTNR.DLL 을 찾는다
		m_hContainer=GetModuleHandle(_T("ATCTNR.DLL"));
		if (!m_hContainer)	throw(_T("ATCTNR.DLL을 찾을 수 없습니다."));

		// 각 함수 포인터를 얻어온다

		m_cEntry.pfnGetCurAlgorithm=(PROC_GetCurAlgorithm)GetProcAddress(m_hContainer, "GetCurAlgorithm");
		if (!m_cEntry.pfnGetCurAlgorithm) throw(_T("GetCurAlgorithm 함수를 찾을 수 없습니다."));

		m_cEntry.pfnGetCurTranslator=(PROC_GetCurTranslator)GetProcAddress(m_hContainer, "GetCurTranslator");
		if (!m_cEntry.pfnGetCurTranslator) throw(_T("GetCurTranslator 함수를 찾을 수 없습니다."));

		m_cEntry.pfnHookDllFunction=(PROC_HookDllFunction)GetProcAddress(m_hContainer, "HookDllFunction");
		if (!m_cEntry.pfnHookDllFunction) throw(_T("HookDllFunction 함수를 찾을 수 없습니다."));

		m_cEntry.pfnUnhookDllFunction=(PROC_UnhookDllFunction)GetProcAddress(m_hContainer, "UnhookDllFunction");
		if (!m_cEntry.pfnUnhookDllFunction) throw(_T("UnhookDllFunction 함수를 찾을 수 없습니다."));

		m_cEntry.pfnGetOrigDllFunction=(PROC_GetOrigDllFunction)GetProcAddress(m_hContainer, "GetOrigDllFunction");
		if (!m_cEntry.pfnGetOrigDllFunction) throw(_T("GetOrigDllFunction 함수를 찾을 수 없습니다."));

		m_cEntry.pfnHookCodePoint=(PROC_HookCodePoint)GetProcAddress(m_hContainer, "HookCodePoint");
		if (!m_cEntry.pfnHookCodePoint) throw(_T("HookCodePoint 함수를 찾을 수 없습니다."));

		m_cEntry.pfnUnhookCodePoint=(PROC_UnhookCodePoint)GetProcAddress(m_hContainer, "UnhookCodePoint");
		if (!m_cEntry.pfnUnhookCodePoint) throw(_T("UnhookCodePoint 함수를 찾을 수 없습니다."));

		m_cEntry.pfnTranslateText=(PROC_TranslateText)GetProcAddress(m_hContainer, "TranslateText");
		if (!m_cEntry.pfnTranslateText) throw(_T("TranslateText 함수를 찾을 수 없습니다."));

		m_cEntry.pfnIsAppLocaleLoaded=(PROC_IsAppLocaleLoaded)GetProcAddress(m_hContainer, "IsAppLocaleLoaded");
		if (!m_cEntry.pfnIsAppLocaleLoaded) throw(_T("IsAppLocaleLoaded 함수를 찾을 수 없습니다."));

		m_cEntry.pfnEnableAppLocale=(PROC_EnableAppLocale)GetProcAddress(m_hContainer, "EnableAppLocale");
		if (!m_cEntry.pfnEnableAppLocale) throw(_T("EnableAppLocale 함수를 찾을 수 없습니다."));
	}
	catch(TCHAR *szErrText)
	{
		OutputDebugString(szErrText);
		m_hContainer=NULL;
	}
}