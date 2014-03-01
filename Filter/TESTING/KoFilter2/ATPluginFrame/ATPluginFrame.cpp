// ATPluginFrame - 아랄트랜스 플러그인 프레임워크

#include "StdAfx.h"
#include "ATPluginFrame.h"
#include "../ATPlugin.h"

#include "Debug.h"

#if _MSC_VER <= 1200 // 1200 = Visual C++ 6.0
#define memcpy_s(d,nd,s,ns) memcpy(d,s,ns)
#endif


CATPluginFrameApp::CATPluginFrameApp() : m_hAralWnd(NULL), m_dwAralVersion(0)
{
	ZeroMemory(&m_sOptionString, sizeof(OPTION_STRING));

	// 플러그인 정보 초기 설정
	LPWSTR pwszPluginType;
#ifdef ATPLUGIN_ALGORITHM
	pwszPluginType = L"Algorithm";
#endif
#ifdef ATPLUGIN_TRANSLATOR
	pwszPluginType = L"Translator";
#endif
#ifdef ATPLUGIN_FILTER
	pwszPluginType = L"Filter";
#endif

	ZeroMemory(&m_sPluginInfo, sizeof(PLUGIN_INFO));
	m_sPluginInfo.cch = sizeof(PLUGIN_INFO);
	lstrcpy(m_sPluginInfo.wszPluginType, pwszPluginType);
	lstrcpy(m_sPluginInfo.wszPluginName, _T("ATPlugin"));
	lstrcpy(m_sPluginInfo.wszDownloadUrl, _T("http://"));

}

BOOL CATPluginFrameApp::_PrePluginInit(HWND hAralWnd, LPVOID pPluginOption)
{
	BOOL bRetVal = FALSE;

	m_hAralWnd = hAralWnd;

	if (theApp.m_dwAralVersion == 0)
	{
		theApp.m_dwAralVersion = MAKELONG(0002,0000);

		m_sOptionString.pszAnsi = (LPSTR) pPluginOption;
		m_sOptionString.bIsUnicode = FALSE;

	}
	else
	{
		m_sOptionString.pwszUnicode = (LPWSTR) pPluginOption;
		m_sOptionString.bIsUnicode = TRUE;
	}
	bRetVal = theApp.OnPluginInit();

	return bRetVal;
}

BOOL CATPluginFrameApp::GetPluginInfo(PLUGIN_INFO * pPluginInfo)
{
	// 이것이 불렸다는 것은 플러그인이 아랄트랜스 0.3 에서 작동한다는 뜻이다.
	m_dwAralVersion = MAKELONG(0003,0000);

	if (sizeof(PLUGIN_INFO) <= pPluginInfo->cch)
	{
		m_sPluginInfo.cch=pPluginInfo->cch; 
		memcpy(pPluginInfo, &m_sPluginInfo, m_sPluginInfo.cch);
		return TRUE;
	}
	return FALSE;
}

BOOL CATPluginFrameApp::OnObjectInit(TRANSLATION_OBJECT* pTransObj)
{
	BOOL bRetVal = FALSE;

#if defined(USE_ARALTRANS_0_3) && ( defined(ATPLUGIN_TRANSLATOR) || defined(ATPLUGIN_FILTER) )
	if(pTransObj)
	{
		try
		{
			// 번역 전처리용 버퍼 생성
			pTransObj->pPreTransBuf = new char[TRANS_BUF_LEN];
			if (!pTransObj->pPreTransBuf) throw ERROR_CODE(ERROR_OUTOFMEMORY);
			pTransObj->nPreTransBufLen = TRANS_BUF_LEN;
			ZeroMemory(pTransObj->pPreTransBuf, pTransObj->nPreTransBufLen);

			// 번역 후처리용 버퍼 생성
			pTransObj->pPostTransBuf = new char[TRANS_BUF_LEN];
			if (!pTransObj->pPostTransBuf) throw ERROR_CODE(ERROR_OUTOFMEMORY);
			pTransObj->nPostTransBufLen = TRANS_BUF_LEN;
			ZeroMemory(pTransObj->pPostTransBuf, pTransObj->nPostTransBufLen);

			// 번역 프로시저 지정
			pTransObj->procTranslate = ::MainTranslateProcedure;
			bRetVal = TRUE;
		}
		catch (long error_code)
		{
			NOTIFY_ERROR(error_code);
			SETLASTERROR(error_code);

			return FALSE;
		}
	}
#endif // defined(USE_ARALTRANS_0_3) && ( defined(ATPLUGIN_TRANSLATOR) || defined(ATPLUGIN_FILTER) )
	return bRetVal;
}
BOOL CATPluginFrameApp::OnObjectClose(TRANSLATION_OBJECT* pTransObj)
{
	BOOL bRetVal = FALSE;

#if defined(USE_ARALTRANS_0_3) && ( defined(ATPLUGIN_TRANSLATOR) || defined(ATPLUGIN_FILTER) )
	if(pTransObj)
	{
		// 전처리용 버퍼 삭제
		if(pTransObj->pPreTransBuf)
		{
			delete[] pTransObj->pPreTransBuf;
			pTransObj->pPreTransBuf = NULL;
		}
		pTransObj->nPreTransBufLen = 0;
		
		// 후처리용 버퍼 삭제
		if(pTransObj->pPostTransBuf)
		{
			delete[] pTransObj->pPostTransBuf;
			pTransObj->pPostTransBuf = NULL;
		}
		pTransObj->nPostTransBufLen = 0;

		// 번역 프로시저 지정 취소
		pTransObj->procTranslate = NULL;
		bRetVal = TRUE;
	}

#endif // defined(USE_ARALTRANS_0_3) && ( defined(ATPLUGIN_TRANSLATOR) || defined(ATPLUGIN_FILTER) )
	return bRetVal;
}

BOOL CATPluginFrameApp::MainTranslateProcedure(TRANSLATION_OBJECT* pTransObj)
{

#if defined(USE_ARALTRANS_0_3) && ( defined(ATPLUGIN_TRANSLATOR) || defined(ATPLUGIN_FILTER) )

	TRESULT tResult = (TRESULT)(TR_DOALL | TR_STOP_OK), tResCurrent;

	try
	{

	// 번역 개체가 존재하는지 확인
	if(NULL == pTransObj) throw ERROR_CODE(ERROR_OBJECT_NOT_FOUND);

	TRANSLATION_OBJECT* pPrevObj = pTransObj->pPrevObject;
	TRANSLATION_OBJECT* pNextObj = pTransObj->pNextObject;

	// 전처리 버퍼가 존재하는지 확인
	if(NULL == pTransObj->pPreTransBuf) throw ERROR_CODE(ERROR_INVALID_USER_BUFFER);
	ZeroMemory(pTransObj->pPreTransBuf, pTransObj->nPreTransBufLen);

	// 이전 개체가 존재하는지 확인
	if(NULL == pPrevObj) throw ERROR_CODE(ERROR_OBJECT_NOT_FOUND);
	// 이전 개체의 전처리 버퍼가 존재하는지 확인
	if(NULL == pPrevObj->pPreTransBuf) throw ERROR_CODE(ERROR_INVALID_USER_BUFFER);
	// 이전 개체의 전처리 버퍼의 크기가 충분한지 확인
	if(pTransObj->nPreTransBufLen < pPrevObj->nPreTransBufLen) throw ERROR_CODE(ERROR_INSUFFICIENT_BUFFER);

	//////////////////////////////////////////////////////////////////////////
	//
	// 전처리 작업
	//
#ifdef ATPLUGIN_FILTER
	// 필터 플러그인이라면 전처리 작업
	tResCurrent = PreTranslateEx(pTransObj);
	tResult = (TRESULT)((tResult | TR_STOP_OK) & tResCurrent);
#else
	// 필터 플러그인이 아니라면 단순 복사
	memcpy_s(pTransObj->pPreTransBuf, pTransObj->nPreTransBufLen,
		pPrevObj->pPreTransBuf, pPrevObj->nPreTransBufLen);
#endif

	//
	// 전처리 작업 완료
	//
	//////////////////////////////////////////////////////////////////////////

	// 다음 개체가 존재하는지 확인
	if(NULL == pNextObj) throw ERROR_CODE(ERROR_OBJECT_NOT_FOUND);
	// 다음 개체의 번역 프로시저가 존재하는지 확인
	if(NULL == pNextObj->procTranslate) throw ERROR_CODE(ERROR_INVALID_FUNCTION);

	// 후처리 버퍼가 존재하는지 확인
	if(NULL == pTransObj->pPostTransBuf) throw ERROR_CODE(ERROR_INVALID_USER_BUFFER);
	ZeroMemory(pTransObj->pPostTransBuf, pTransObj->nPostTransBufLen);

	// 다음 개체의 후처리 버퍼가 존재하는지 확인
	if(NULL == pNextObj->pPostTransBuf) throw ERROR_CODE(ERROR_INVALID_USER_BUFFER);
	// 다음 개체의 후처리 버퍼가 충분한지 확인
	if(pTransObj->nPostTransBufLen < pNextObj->nPostTransBufLen) throw ERROR_CODE(ERROR_INSUFFICIENT_BUFFER);

	//////////////////////////////////////////////////////////////////////////
	//
	// 번역 작업
	//
	// 자체 번역
#ifdef ATPLUGIN_TRANSLATOR
	if (tResult & TR_TRANSONLY)
		tResCurrent = TranslateEx(pTransObj);
	tResult = (TRESULT)((tResult | TR_STOP_OK) & tResCurrent);
#endif
	// * 다음 객체의 번역 프로시저 실행
	if (tResult & TR_NEXTONLY)
	{
		tResCurrent = NextTranslateEx(pTransObj);
		tResult = (TRESULT)((tResult | TR_STOP_OK) & tResCurrent);
	}
	else
		memcpy_s(pNextObj->pPostTransBuf, pNextObj->nPostTransBufLen,
			pTransObj->pPreTransBuf, pTransObj->nPreTransBufLen);


	//
	// 번역 작업 완료
	//
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	//
	// 후처리 작업
	//
#ifdef ATPLUGIN_FILTER
	// 필터 플러그인이라면 전처리 작업
	if (tResult & TR_POSTONLY)
	{
		tResCurrent = PostTranslateEx(pTransObj);
		tResult = (TRESULT)((tResult | TR_STOP_OK) & tResCurrent);
	}
#else
	// 필터 플러그인이 아니라면 단순 복사
	if (tResult & TR_POSTONLY)
		memcpy_s(pTransObject->pPostTransBuf, pTransObject->nPostTransBufLen, 
			pNextObject->pPostTransBuf, pNextObject->nPostTransBufLen);

#endif

	//
	// 후처리 작업 완료
	//
	//////////////////////////////////////////////////////////////////////////

	}
	catch (long error_code)
	{
		NOTIFY_ERROR(error_code);
		SETLASTERROR(error_code);

		return FALSE;
	}
	if ( tResult == TR_STOPALL )
		return FALSE;

#endif // defined(USE_ARALTRANS_0_3) && ( defined(ATPLUGIN_TRANSLATOR) || defined(ATPLUGIN_FILTER) )
	return TRUE;
}

CATPluginFrameApp::TRESULT CATPluginFrameApp::PreTranslateEx(TRANSLATION_OBJECT *pTransObj)
{
#if defined(USE_ARALTRANS_0_3) && ( defined(ATPLUGIN_TRANSLATOR) || defined(ATPLUGIN_FILTER) )
	BOOL bRes = PreTranslate((LPCSTR)pTransObj->pPrevObject->pPreTransBuf, (LPSTR)pTransObj->pPreTransBuf, pTransObj->nPreTransBufLen);
	if (bRes == FALSE) return TR_STOPALL;
#endif // defined(USE_ARALTRANS_0_3) && ( defined(ATPLUGIN_TRANSLATOR) || defined(ATPLUGIN_FILTER) )
	return TR_DOALL;
}

CATPluginFrameApp::TRESULT CATPluginFrameApp::TranslateEx(TRANSLATION_OBJECT *pTransObj)
{
#if defined(USE_ARALTRANS_0_3) && ( defined(ATPLUGIN_TRANSLATOR) || defined(ATPLUGIN_FILTER) )
	LPSTR pszTransBuf = new char[pTransObj->nPreTransBufLen];
	BOOL bRes = Translate((LPCSTR)pTransObj->pPreTransBuf, pszTransBuf, pTransObj->nPreTransBufLen);
	if (bRes == FALSE) 
	{
		delete[] pszTransBuf;
		return TR_STOPALL;
	}
	memcpy(pTransObj->pPreTransBuf, pszTransBuf, pTransObj->nPreTransBufLen);
	delete[] pszTransBuf;

#endif // defined(USE_ARALTRANS_0_3) && ( defined(ATPLUGIN_TRANSLATOR) || defined(ATPLUGIN_FILTER) )
	return TR_DOALL;

}

CATPluginFrameApp::TRESULT CATPluginFrameApp::NextTranslateEx(TRANSLATION_OBJECT *pTransObj)
{
#if defined(USE_ARALTRANS_0_3) && ( defined(ATPLUGIN_TRANSLATOR) || defined(ATPLUGIN_FILTER) )
	BOOL bRes = pTransObj->pNextObject->procTranslate(pTransObj->pNextObject);
	if(FALSE == bRes) return TR_STOPALL;
#endif // defined(USE_ARALTRANS_0_3) && ( defined(ATPLUGIN_TRANSLATOR) || defined(ATPLUGIN_FILTER) )
	return TR_DOALL;
}

CATPluginFrameApp::TRESULT CATPluginFrameApp::PostTranslateEx(TRANSLATION_OBJECT *pTransObj)
{
#if defined(USE_ARALTRANS_0_3) && ( defined(ATPLUGIN_TRANSLATOR) || defined(ATPLUGIN_FILTER) )
	BOOL bRes = PostTranslate((LPCSTR)pTransObj->pNextObject->pPostTransBuf, (LPSTR)pTransObj->pPostTransBuf, pTransObj->nPostTransBufLen);
	if (bRes == FALSE) return TR_STOPALL;
#endif // defined(USE_ARALTRANS_0_3) && ( defined(ATPLUGIN_TRANSLATOR) || defined(ATPLUGIN_FILTER) )
	return TR_DOALL;
}


TRANSLATION_OBJECT *CATPluginFrameApp::GetFirstObject(TRANSLATION_OBJECT *pTransObj)
{
	TRANSLATION_OBJECT *pObj = NULL;
#if defined(USE_ARALTRANS_0_3) && ( defined(ATPLUGIN_TRANSLATOR) || defined(ATPLUGIN_FILTER) )
	pObj = pTransObj;
	while(pObj->pPrevObject) pObj = pObj->pPrevObject;	// skip until there is no previous object
#endif // defined(USE_ARALTRANS_0_3) && ( defined(ATPLUGIN_TRANSLATOR) || defined(ATPLUGIN_FILTER) )
	return pObj;
}

TRANSLATION_OBJECT *CATPluginFrameApp::GetLastObject(TRANSLATION_OBJECT *pTransObj)
{
	TRANSLATION_OBJECT *pObj = NULL;
#if defined(USE_ARALTRANS_0_3) && ( defined(ATPLUGIN_TRANSLATOR) || defined(ATPLUGIN_FILTER) )
	pObj = pTransObj;
	while(pObj->pNextObject) pObj = pObj->pNextObject;	// skip until there is no next object
#endif // defined(USE_ARALTRANS_0_3) && ( defined(ATPLUGIN_TRANSLATOR) || defined(ATPLUGIN_FILTER) )
	return pObj;
}
