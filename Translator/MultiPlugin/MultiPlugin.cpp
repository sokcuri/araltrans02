#include "StdAfx.h"

#include "ATPluginContainer.h"
#include "MultiPlugin.h"

#include "Util.h"

CMultiPlugin g_cMultiPlugin;

PLUGININFO *CMultiPlugin::LoadPlugin(const TCHAR *pszPluginDirectory, const ATPLUGIN_ARGUMENT &pluginArg)
{
	TCHAR szDLLPath[MAX_PATH];
	PLUGININFO *pPlugin;

	pPlugin = new PLUGININFO;

	// 플러그인을 로드
	wsprintf(szDLLPath, _T("%s\\%s.DLL"), pszPluginDirectory, pluginArg.strPluginName.c_str() );

	pPlugin->strName=pluginArg.strPluginName;
	if (pPlugin->cPlugin.Load(szDLLPath))
	{
		char szOption[4096];

#ifdef _UNICODE
		Wide2Kor(pluginArg.strOption.c_str(), szOption);
#else
		lstrcpyn(szOption, pluginArg.strOption.c_str(), 4096);
#endif

		// 플러그인 초기화
		if ( !pPlugin->cPlugin.OnPluginInit(g_hSettingWnd, szOption) )
		{
			// 초기화 에러
			pPlugin->cPlugin.Unload();	
			MessageBox(g_hSettingWnd, _T("플러그인 초기화가 되지 않았습니다"), 
				pluginArg.strPluginName.c_str(), MB_OK | MB_ICONEXCLAMATION);
			delete pPlugin;
			return NULL;
		}
	}
	else
	{
		// 로드 에러
		pPlugin->cPlugin.Unload();
		MessageBox(g_hSettingWnd, _T("플러그인 로드가 되지 않았습니다"), 
			pluginArg.strPluginName.c_str(), MB_OK | MB_ICONEXCLAMATION);
		delete pPlugin;
		return NULL;
	}
	if (m_bIsHooker)
		pPlugin->cPlugin.OnPluginStart();

	return pPlugin;
}

BOOL CMultiPlugin::Add(const ATPLUGIN_ARGUMENT &pluginArg, bool bIsTranslator)
{
	PLUGININFO *pPlugin;

	tstring strPluginDirectory = GetATDirectory();
	if (bIsTranslator)
	{
		strPluginDirectory+=_T("\\Translator");
	}
	else
	{
		strPluginDirectory+=_T("\\Filter");
	}

	pPlugin = LoadPlugin(strPluginDirectory.c_str(), pluginArg);

	if (!pPlugin)
		return FALSE;

	if (bIsTranslator)
		m_pTranslator=pPlugin;
	else
		m_apFilters.push_back(pPlugin);
	return TRUE;
}

BOOL CMultiPlugin::Add(const ATPLUGIN_ARGUMENT_ARRAY &aPluginArgs)
{
	int i = 0;
	for(i = 0; i<aPluginArgs.size()-1; i++)
	{
		Add(aPluginArgs[i], false);
	}

	if (!aPluginArgs[i].strPluginName.empty())
		return Add(aPluginArgs[i], true);

	return TRUE;
}

BOOL CMultiPlugin::Remove(int nIndex)
{

	if (nIndex < 0)
	{
		if (m_pTranslator)
		{
			m_pTranslator->cPlugin.OnPluginClose();
			delete m_pTranslator;
			m_pTranslator=NULL;
		}
	}
	else
	{
		if (m_apFilters.size() <= nIndex)
			return FALSE;

		PLUGINPOINTERARRAY::iterator it;
		for(it=m_apFilters.begin(); it != m_apFilters.end(); it++)
		{
			// 해당 플러그인을 찾아 삭제
			if (*it == m_apFilters[nIndex])
			{
				if (m_bIsHooker)
					(*it)->cPlugin.OnPluginStop();
				(*it)->cPlugin.OnPluginClose();
				delete *it;

				m_apFilters.erase(it);
				break;
			}
		}
	}
	return TRUE;
}

BOOL CMultiPlugin::MoveUp(int nIndex)
{
	if (m_apFilters.size() <= nIndex)
		return FALSE;

	if (nIndex == 0)
		return FALSE;

	// 플러그인 순서 바꿈
	PLUGININFO *pTemp;
	pTemp = m_apFilters[nIndex-1];
	m_apFilters[nIndex-1] = m_apFilters[nIndex];
	m_apFilters[nIndex] = pTemp;
	return TRUE;
	
}

BOOL CMultiPlugin::MoveDown(int nIndex)
{
	if (m_apFilters.size() <= nIndex)
		return FALSE;
	
	if (nIndex+1 == m_apFilters.size())
		return FALSE;
	
	// 플러그인 순서 바꿈
	PLUGININFO *pTemp;
	pTemp = m_apFilters[nIndex];
	m_apFilters[nIndex] = m_apFilters[nIndex+1];
	m_apFilters[nIndex+1] = pTemp;
	return TRUE;
	
}

BOOL CMultiPlugin::MoveTop(int nIndex)
{
	if (m_apFilters.size() <= nIndex)
		return FALSE;
	
	if (nIndex == 0)
		return FALSE;

	// 해당 플러그인을 찾아 맨 위로 올림
	PLUGINPOINTERARRAY::iterator it;
	for(it=m_apFilters.begin(); it != m_apFilters.end(); it++)
	{
		if (*it == m_apFilters[nIndex])
		{
			PLUGININFO *pTemp;

			pTemp = *it;
			m_apFilters.erase(it);
			m_apFilters.insert(m_apFilters.begin(),pTemp);
			break;
		}
	}
	return TRUE;
}

BOOL CMultiPlugin::MoveBottom(int nIndex)
{
	if (m_apFilters.size() <= nIndex)
		return FALSE;
	
	if (nIndex+1 == m_apFilters.size())
		return FALSE;
	
	// 해당 플러그인을 찾아 맨 아래로 내림
	PLUGINPOINTERARRAY::iterator it;
	for(it=m_apFilters.begin(); it != m_apFilters.end(); it++)
	{
		if (*it == m_apFilters[nIndex])
		{
			PLUGININFO *pTemp;
			
			pTemp = *it;
			m_apFilters.erase(it);
			m_apFilters.push_back(pTemp);
			break;
		}
	}
	return TRUE;
}
void CMultiPlugin::RemoveAll()
{
	if (!m_apFilters.empty())
	{
		// 완전 삭제
		PLUGINPOINTERARRAY::reverse_iterator it;
		for(it=m_apFilters.rbegin(); it != m_apFilters.rend(); it++)
		{
			if (m_bIsHooker)
				(*it)->cPlugin.OnPluginStop();
			(*it)->cPlugin.OnPluginClose();
			delete *it;
		}
		m_apFilters.clear();
	}
	if (m_pTranslator)
	{
		m_pTranslator->cPlugin.OnPluginClose();
		delete m_pTranslator;
		m_pTranslator=NULL;
	}
}

void CMultiPlugin::GetPluginArgs(ATPLUGIN_ARGUMENT_ARRAY &aPluginArgs)
{
	aPluginArgs.clear();

	ATPLUGIN_ARGUMENT arg;

	if (!m_apFilters.empty())
	{
		// 플러그인을 찾아 이름과 옵션 복사
		PLUGINPOINTERARRAY::iterator it;
		for(it=m_apFilters.begin(); it!=m_apFilters.end(); it++)
		{
			arg.strPluginName=(*it)->strName;
			arg.strOption=MakeTString((*it)->cPlugin.GetOptionString());
			aPluginArgs.push_back(arg);
		}
	}
	if (m_pTranslator)
	{
		arg.strPluginName=m_pTranslator->strName;
		arg.strOption=MakeTString(m_pTranslator->cPlugin.GetOptionString());
	}
	else
	{
		arg.strPluginName.erase();
		arg.strOption.erase();
	}
	aPluginArgs.push_back(arg);

}

BOOL CMultiPlugin::OnPluginStart()
{
	m_bIsHooker=true;
	if (!m_apFilters.empty())
	{
		PLUGINPOINTERARRAY::iterator it;
		for(it=m_apFilters.begin(); it!=m_apFilters.end(); it++)
		{
			(*it)->cPlugin.OnPluginStart();	
		}
	}
	return TRUE;
}

BOOL CMultiPlugin::OnPluginStop()
{
	if (!m_apFilters.empty())
	{
		PLUGINPOINTERARRAY::reverse_iterator it;
		for(it=m_apFilters.rbegin(); it != m_apFilters.rend(); it++)
		{
			(*it)->cPlugin.OnPluginStop();
		}
	}
	m_bIsHooker=false;
	return TRUE;
}

BOOL CMultiPlugin::Translate(LPCSTR cszJapanese, LPSTR szKorean, int nBufSize)
{
	char *pszIn=NULL, *pszOut=NULL, *pszTemp=NULL;
	BOOL bIsSuccess=FALSE;

	// 버퍼 준비
	pszIn=new char[nBufSize];
	pszOut=new char[nBufSize];
	
	lstrcpynA(pszIn, cszJapanese, nBufSize);

	try 
	{
		if (!m_apFilters.empty())
		{

			// 번역 전처리
			PLUGINPOINTERARRAY::iterator it;
			for(it=m_apFilters.begin(); it!=m_apFilters.end(); it++)
			{
				// PreTranslate
				bIsSuccess=(*it)->cPlugin.PreTranslate(pszIn, pszOut, nBufSize);

				// 번역중 에러 발생시 번역 중지
				if (!bIsSuccess)
					throw bIsSuccess;
				
				// Out 을 새로운 In으로, 이전 In은 필요없으니 다음 Out으로 전용
				pszTemp=pszIn;
				pszIn=pszOut;
				pszOut=pszTemp;

			}
			
		}

		// 번역
		if (m_pTranslator)
			bIsSuccess=m_pTranslator->cPlugin.Translate(pszIn, pszOut, nBufSize);

		if (!bIsSuccess)
			throw bIsSuccess;

		// Out 을 새로운 In으로, 이전 In은 필요없으니 다음 Out으로 전용
		pszTemp=pszIn;
		pszIn=pszOut;
		pszOut=pszTemp;

		if (!m_apFilters.empty())
		{
			
			// 번역 후처리
			PLUGINPOINTERARRAY::reverse_iterator it;
			for(it=m_apFilters.rbegin(); it!=m_apFilters.rend(); it++)
			{
				// PreTranslate
				bIsSuccess=(*it)->cPlugin.PostTranslate(pszIn, pszOut, nBufSize);
				
				// 번역중 에러 발생시 번역 중지
				if (!bIsSuccess)
					throw bIsSuccess;
				
				// Out 을 새로운 In으로, 이전 In은 필요없으니 다음 Out으로 전용
				pszTemp=pszIn;
				pszIn=pszOut;
				pszOut=pszTemp;
				
			}
			
		}

	}	// try
	catch(BOOL e) 
	{}

	lstrcpynA(szKorean, pszIn, nBufSize);

	// 버퍼 삭제
	delete[] pszIn;
	delete[] pszOut;

	return bIsSuccess;
}

BOOL CMultiPlugin::OnPluginOption(int nIndex)
{

	if (nIndex < 0)
	{
		if (m_pTranslator)
			return m_pTranslator->cPlugin.OnPluginOption();
	}
	else
	{
		if (m_apFilters.size() <= nIndex)
			return FALSE;

		return m_apFilters[nIndex]->cPlugin.OnPluginOption();
	}
	return FALSE;
}

const ATPLUGIN_VERSION *CMultiPlugin::OnPluginVersion(int nIndex)
{

	if (nIndex < 0)
	{
		if (m_pTranslator)
			return m_pTranslator->cPlugin.OnPluginVersion();
	}
	else
	{
		if (m_apFilters.size() <= nIndex)
			return FALSE;

		return m_apFilters[nIndex]->cPlugin.OnPluginVersion();
	}
	
	return FALSE;
}