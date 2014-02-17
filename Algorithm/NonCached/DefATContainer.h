
#pragma once

// DefATContainer.h
// 아랄트랜스 0.2 API 관련 헤더 파일
//
// Changelog
// * 이전 내용 정리 예정 ^^ *
// 2012.10.06
//		HookDllFunction, HookCodePoint 에 후킹 타입 추가
// 2013.06.16
//		하위 호환성을 위해 후킹타입 추가된 HookDllFunction, HookCodePoint 를 HookDllFunctionEx, HookCodePointEx 로 변경
//		기존 HookDllFunction, HookCodePoint 를 API 에 재등록

// 콜백 함수형 정의
typedef struct _REGISTER_ENTRY
{

	DWORD _EAX;
	DWORD _EBX;
	DWORD _ECX;
	DWORD _EDX;
	DWORD _ESI;
	DWORD _EDI;
	DWORD _EBP;
	DWORD _ESP;
	DWORD _EFL;

} REGISTER_ENTRY, *PREGISTER_ENTRY;

// 후킹 타입 (INT nHookType)
enum _ATHOOKTYPE { ATHOOKTYPE_COPY = 0, ATHOOKTYPE_SOURCE = 1};

typedef void (* PROC_HookCallback)(LPVOID pHookedPoint, PREGISTER_ENTRY pRegisters);

// 컨테이너가 제공하는 함수
typedef HMODULE (__stdcall * PROC_GetCurAlgorithm)();
typedef HMODULE (__stdcall * PROC_GetCurTranslator)();
typedef BOOL	(__stdcall * PROC_HookDllFunction)(LPCSTR cszDllName, LPCSTR cszFuncName, LPVOID pfnNewFunc);	// obsolete; 구버전 호환용
typedef BOOL	(__stdcall * PROC_HookDllFunctionEx)(LPCSTR cszDllName, LPCSTR cszFuncName, LPVOID pfnNewFunc, INT nHookType);
typedef BOOL	(__stdcall * PROC_UnhookDllFunction)(LPCSTR cszDllName, LPCSTR cszFuncName);
typedef LPVOID	(__stdcall * PROC_GetOrigDllFunction)(LPCSTR cszDllName, LPCSTR cszFuncName);
typedef BOOL	(__stdcall * PROC_HookCodePoint)(LPVOID pCodePoint, PROC_HookCallback pfnCallback);	// obsolete; 구버전 호환용
typedef BOOL	(__stdcall * PROC_HookCodePointEx)(LPVOID pCodePoint, PROC_HookCallback pfnCallback, INT nHookType);
typedef BOOL	(__stdcall * PROC_UnhookCodePoint)(LPVOID pCodePoint);
typedef BOOL	(__stdcall * PROC_TranslateText)(LPCSTR cszJapanese, LPSTR szKorean, int nBufSize);
typedef BOOL	(__stdcall * PROC_TranslateText)(LPCSTR cszJapanese, LPSTR szKorean, int nBufSize);
typedef BOOL	(__stdcall * PROC_IsAppLocaleLoaded)();
typedef BOOL	(__stdcall * PROC_EnableAppLocale)(BOOL bEnable);
typedef BOOL	(__stdcall * PROC_SuspendAllThread)();
typedef BOOL	(__stdcall * PROC_ResumeAllThread)();
typedef BOOL	(__stdcall * PROC_IsAllThreadSuspended)();

typedef struct _CONTAINER_FUNCTION_ENTRY
{

	PROC_GetCurAlgorithm	pfnGetCurAlgorithm;
	PROC_GetCurTranslator	pfnGetCurTranslator;
	PROC_HookDllFunction	pfnHookDllFunction;
	PROC_HookDllFunctionEx	pfnHookDllFunctionEx;
	PROC_UnhookDllFunction	pfnUnhookDllFunction;
	PROC_GetOrigDllFunction	pfnGetOrigDllFunction;
	PROC_HookCodePoint		pfnHookCodePoint;
	PROC_HookCodePointEx	pfnHookCodePointEx;
	PROC_UnhookCodePoint	pfnUnhookCodePoint;
	PROC_TranslateText		pfnTranslateText;
	PROC_IsAppLocaleLoaded	pfnIsAppLocaleLoaded;
	PROC_EnableAppLocale	pfnEnableAppLocale;
	PROC_SuspendAllThread	pfnSuspendAllThread;
	PROC_ResumeAllThread	pfnResumeAllThread;
	PROC_IsAllThreadSuspended	pfnIsAllThreadSuspended;

} CONTAINER_FUNCTION_ENTRY, *PCONTAINER_FUNCTION_ENTRY;
