//
// ATCApi - 아랄트랜스 컨테이너 제공 API 를 사용하기 위한 모듈
//

#ifndef __ATCAPI_H__
#define __ATCAPI_H__

#include "DefATContainer.h"

// CATCApi : 아랄 트랜스 컨테이너 제공 API
//           버전 0.2.20080518
class CATCApi {
	protected:
		HMODULE m_hContainer;
		CONTAINER_FUNCTION_ENTRY m_cEntry;

	public:
		CATCApi();

		// 제대로 초기화 되었는지를 알아온다. 실제 호출시 확인하지 않으므로 미리 확인할 것.
		bool IsValid() { return (m_hContainer)?TRUE:FALSE; }

		// 현재 로드된 알고리즘의 모듈 핸들을 가져온다.
		HMODULE GetCurAlgorithm() 
			{ return m_cEntry.pfnGetCurAlgorithm(); }
		// 현재 로드된 번역기의 모듈 핸들을 가져온다.
		HMODULE GetCurTranslator() 
			{ return m_cEntry.pfnGetCurTranslator(); }
		// 특정 DLL 함수를 후킹한다.
		BOOL HookDllFunction(LPCSTR cszDllName, LPCSTR cszFuncName, LPVOID pfnNewFunc) 
			{ return m_cEntry.pfnHookDllFunction(cszDllName, cszFuncName, pfnNewFunc); }
		// 특정 DLL 함수의 후킹을 종료한다.
		BOOL UnhookDllFunction(LPCSTR cszDllName, LPCSTR cszFuncName)
			{ return m_cEntry.pfnUnhookDllFunction(cszDllName, cszFuncName); }
		// 후킹된 DLL함수의 원래 포인터를 가져온다.
		LPVOID GetOrigDllFunction(LPCSTR cszDllName, LPCSTR cszFuncName)
			{ return m_cEntry.pfnGetOrigDllFunction(cszDllName, cszFuncName); }
		// 코드의 특정지점을 후킹한다.
		BOOL HookCodePoint(LPVOID pCodePoint, PROC_HookCallback pfnCallback)
			{ return m_cEntry.pfnHookCodePoint(pCodePoint, pfnCallback); }
		// 특정지점의 후킹을 종료한다.
		BOOL UnhookCodePoint(LPVOID pCodePoint) 
			{ return m_cEntry.pfnUnhookCodePoint(pCodePoint); }
		// 문자열을 번역한다.
		BOOL TranslateText(LPCSTR cszJapanese, LPSTR szKorean, int nBufSize)
			{ return m_cEntry.pfnTranslateText(cszJapanese, szKorean, nBufSize); }
		// AppLocale 이 로드되었는지를 알아온다.
		BOOL IsAppLocaleLoaded()
			{ return m_cEntry.pfnIsAppLocaleLoaded(); }
		// AppLocale 을 켜거나 끈다. (현재 안쓰임, 정상 작동 보장못함)
		BOOL EnableAppLocale(BOOL bEnable)
			{ return m_cEntry.pfnEnableAppLocale(bEnable); }


};

// ATCApi 본체
extern CATCApi g_cATCApi;

#endif //__ATCAPI_H__