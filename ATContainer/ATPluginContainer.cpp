#include "StdAfx.h"

#include "ATPluginContainer.h"

void CATPluginContainer::Unload()
{

	if (m_pszOption)
	{
		delete[] m_pszOption;
		m_pszOption=NULL;
	}

	if (m_hModule)
	{
		FreeLibrary(m_hModule);
		m_hModule=NULL;
	}
}

BOOL CATPluginContainer::Load(LPCTSTR pszModulePath)
{

	if (m_hModule)
		Unload();

	try 
	{
		// 플러그인 로드
		m_hModule=LoadLibrary(pszModulePath);

		if(!m_hModule) throw(_T("LoadLibrary 에러"));

		// 함수 주소 받아옴

		// 기본 함수
		m_pfnOnPluginInit=(PROC_OnPluginInit)GetProcAddress(m_hModule, "OnPluginInit");
		m_pfnOnPluginOption=(PROC_OnPluginDefault)GetProcAddress(m_hModule, "OnPluginOption");
		m_pfnOnPluginClose=(PROC_OnPluginDefault)GetProcAddress(m_hModule, "OnPluginClose");

		if (!m_pfnOnPluginInit || !m_pfnOnPluginOption || !m_pfnOnPluginClose) 
			throw(_T("기본함수 주소를 찾을 수 없습니다"));

/*
		// 버전 (옵션)
		m_pfnOnPluginVersion=(PROC_OnPluginVersion)GetProcAddress(m_hModule, "OnPluginVersion");

		// 후킹 관련
		m_pfnOnPluginStart=(PROC_OnPluginDefault)GetProcAddress(m_hModule, "OnPluginStart");
		m_pfnOnPluginStop=(PROC_OnPluginDefault)GetProcAddress(m_hModule, "OnPluginStop");

		// 번역 관련
		m_pfnTranslate=(PROC_Translate)GetProcAddress(m_hModule, "Translate");
*/
		// 필터 관련
		m_pfnPreTranslate = (PROC_Translate)GetProcAddress(m_hModule, "PreTranslate");
		m_pfnPostTranslate = (PROC_Translate)GetProcAddress(m_hModule, "PostTranslate");

/*
		if (!m_pfnOnPluginStart || !m_pfnOnPluginStop)
		{
			m_pfnOnPluginStart=m_pfnOnPluginStop=NULL;
		}
*/
		if (!m_pfnPreTranslate || !m_pfnPostTranslate)
		{
			m_pfnPreTranslate=m_pfnPostTranslate=NULL;
		}
		if (/*!m_pfnOnPluginStart && !m_pfnTranslate &&*/ !m_pfnPreTranslate || !m_pfnPostTranslate)
				throw(_T("이 DLL은 필터 플러그인이 아닙니다."));
	}
	catch(TCHAR *errMsg)
	{
		MessageBox(NULL, errMsg, _T("에러"), MB_OK | MB_ICONEXCLAMATION);
		Unload();
		return FALSE;
	}
	return TRUE;
}

BOOL CATPluginContainer::OnPluginInit(HWND hSettingWnd, LPCSTR cszOptionStringBuffer)
{
	if (!m_pfnOnPluginInit)
		return FALSE;

	// 각 플러그인마다 옵션이 필요함
	m_pszOption=new char[4096];
	if (cszOptionStringBuffer)
		lstrcpyA(m_pszOption, cszOptionStringBuffer);
	else
		m_pszOption[0]='\0';

	return m_pfnOnPluginInit(hSettingWnd, m_pszOption);
}
