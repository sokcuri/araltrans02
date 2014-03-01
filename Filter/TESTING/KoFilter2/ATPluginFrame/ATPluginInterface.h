// ATPluginInterface - 아랄트랜스 플러그인 인터페이스 규약
//

#ifndef __ATPLUGININTERFACE_H__
#define __ATPLUGININTERFACE_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// 플러그인 세팅이 설정되어 있지 않으면 에러 발생
#if !defined(USE_ARALTRANS_0_2) && !defined(USE_ARALTRANS_0_3)
#if _MSC_VER > 1200 // 1200 = Visual C++ 6.0
#error 플러그인이 사용될 아랄트랜스 종류가 정의되지 않았습니다. StdAfx.h 에서 USE_ARALTRANS_0_2 혹은 USE_ARALTRANS_0_3 중 최소한 하나를 정의 해 주세요.
#elif
#error Plugin type is not specified. Please define at least one of these types on StdAfx.h : ATPLUGIN_ALGORITHM, ATPLUGIN_TRANSLATOR, or ATPLUGIN_FILTER.
#endif // _MSC_VER > 1200
#endif // !defined(USE_ARALTRANS_0_2) && !defined(USE_ARALTRANS_0_3)

#if !defined(ATPLUGIN_ALGORITHM) && !defined(ATPLUGIN_TRANSLATOR) && !defined(ATPLUGIN_FILTER)
#if _MSC_VER > 1200 // 1200 = Visual C++ 6.0
#error 플러그인 종류가 정의되지 않았습니다. StdAfx.h 에서 ATPLUGIN_ALGORITHM, ATPLUGIN_TRANSLATOR, 혹은 ATPLUGIN_FILTER 중 최소한 하나를 정의 해 주세요.
#elif
#error Plugin type is not specified. Please define at least one of these types on StdAfx.h : ATPLUGIN_ALGORITHM, ATPLUGIN_TRANSLATOR, or ATPLUGIN_FILTER.
#endif // _MSC_VER > 1200
#endif // !defined(ATPLUGIN_ALGORITHM) && !defined(ATPLUGIN_TRANSLATOR) && !defined(ATPLUGIN_FILTER)


#if defined(USE_ARALTRANS_0_2) && !defined(USE_ARALTRANS_0_3)
// PLUGIN_INFO 구조체 (OnPluginInfo 함수 인자로 사용)
// 사실 아랄트랜스 0.2 에서는 필요 없지만 그래도 있으면 정보창 띄우기가 편하다.. ㅎㅎ
struct PLUGIN_INFO
{
	int			cch;				// PLUGIN_INFO 구조체 사이즈
	int			nIconID;			// Icon Resource ID
	wchar_t		wszPluginType[16];	// 플러그인 타입
	wchar_t		wszPluginName[64];	// 플러그인 이름
	wchar_t		wszDownloadUrl[256];// 플러그인 다운로드 URL
};
typedef void TRANSLATION_OBJECT;	// 0.2에서는 실제 사용하지 않는다.
#else
namespace NS_ARALTRANS_0_3
{
#include "DefStruct.h"
}
using NS_ARALTRANS_0_3::PLUGIN_INFO;
using NS_ARALTRANS_0_3::TRANSLATION_OBJECT;
#endif // defined(USE_ARALTRANS_0_2) && !defined(USE_ARALTRANS_0_3)


#ifndef USE_MFC
/////////////////////////////////////////////////////////////////////////////
// CWinApp
// 비 MFC 컴파일시 사용하는 가짜 CWinApp
//
// 주의: 당연하지만 진짜 CWinApp 처럼 사용하면 큰일납니다 -_-a

class CWinApp {
public:
	// 이 플러그인의 모듈 핸들 = 인스턴스 핸들
	HINSTANCE m_hInstance;
	
public:
	CWinApp() : m_hInstance(NULL) {}
	
	virtual BOOL InitInstance() { return TRUE; }
};
#endif // USE_MFC

///////////////////////////////////////////////////////////////////////////
// 플러그인 export 함수 선언
#ifdef __cplusplus
extern "C"
{
#endif

// 공통
BOOL __stdcall OnPluginInit(HWND hAralWnd, LPVOID pvPluginOption);
BOOL __stdcall OnPluginOption();
BOOL __stdcall OnPluginClose();
#ifdef USE_ARALTRANS_0_3	// 아랄트랜스 0.3 전용
BOOL __stdcall GetPluginInfo(PLUGIN_INFO * pPluginInfo);
#endif	// USE_ARALTRANS_0_3

#ifdef USE_ARALTRANS_0_2	// 아랄트랜스 0.2 전용
// 후킹 플러그인 전용
#ifdef ATPLUGIN_ALGORITHM
BOOL __stdcall OnPluginStart();
BOOL __stdcall OnPluginStop();
#endif

// 번역 플러그인 전용
#ifdef ATPLUGIN_TRANSLATOR
BOOL __stdcall Translate(LPCSTR cszJapanese, LPSTR szKorean, int nBufSize);
#endif

// 필터 플러그인 전용
#ifdef ATPLUGIN_FILTER
BOOL __stdcall PreTranslate(LPCSTR cszInJapanese, LPSTR szOutJapanese, int nBufSize);
BOOL __stdcall PostTranslate(LPCSTR cszInKorean, LPSTR szOutKorean, int nBufSize);
#endif
#endif // USE_ARALTRANS_0_2

#ifdef USE_ARALTRANS_0_3
// 필터 및 번역 플러그인 공용
#if defined(ATPLUGIN_FILTER) || defined(ATPLUGIN_TRANSLATOR)
BOOL __stdcall OnObjectInit(TRANSLATION_OBJECT* pTransObj);
BOOL __stdcall OnObjectClose(TRANSLATION_OBJECT* pTransObj);
BOOL __stdcall OnObjectMove(TRANSLATION_OBJECT* pTransObj);
BOOL __stdcall OnObjectOption(TRANSLATION_OBJECT* pTransObj);

#endif

BOOL __stdcall MainTranslateProcedure(TRANSLATION_OBJECT* pTransObj);

#endif // USE_ARALTRANS_0_3

// export 를 위한 링커 옵션
// 공통
#pragma comment(linker, "/EXPORT:OnPluginInit=_OnPluginInit@8")
#pragma comment(linker, "/EXPORT:OnPluginOption=_OnPluginOption@0")
#pragma comment(linker, "/EXPORT:OnPluginClose=_OnPluginClose@0")
#ifdef USE_ARALTRANS_0_3	// 아랄트랜스 0.3 전용
#pragma comment(linker, "/EXPORT:GetPluginInfo=_GetPluginInfo@4")
#endif	// USE_ARALTRANS_0_3

#ifdef USE_ARALTRANS_0_2	// 아랄트랜스 0.2 전용
// 후킹 플러그인 전용
#ifdef ATPLUGIN_ALGORITHM
#pragma comment(linker, "/EXPORT:OnPluginStart=_OnPluginStart@0")
#pragma comment(linker, "/EXPORT:OnPluginStop=_OnPluginStop@0")
#endif

// 번역 플러그인 전용
#ifdef ATPLUGIN_TRANSLATOR
#pragma comment(linker, "/EXPORT:Translate=_Translate@12")
#endif

// 필터 플러그인 전용
#ifdef ATPLUGIN_FILTER
#pragma comment(linker, "/EXPORT:PreTranslate=_PreTranslate@12")
#pragma comment(linker, "/EXPORT:PostTranslate=_PostTranslate@12")
#endif
#endif // USE_ARALTRANS_0_2

#ifdef USE_ARALTRANS_0_3
// 필터 및 번역 플러그인 공용
#if defined(ATPLUGIN_FILTER) || defined(ATPLUGIN_TRANSLATOR)
#pragma comment(linker, "/EXPORT:OnObjectInit=_OnObjectInit@4")
#pragma comment(linker, "/EXPORT:OnObjectClose=_OnObjectClose@4")
#pragma comment(linker, "/EXPORT:OnObjectMove=_OnObjectMove@4")
#pragma comment(linker, "/EXPORT:OnObjectOption=_OnObjectOption@4")
#endif
#endif // USE_ARALTRANS_0_3


#ifdef __cplusplus
}
#endif


#endif // __ATPLUGININTERFACE_H__

