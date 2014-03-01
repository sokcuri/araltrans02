
#pragma once

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

typedef void (* PROC_HookCallback)(LPVOID pHookedPoint, PREGISTER_ENTRY pRegisters);

// 컨테이너가 제공하는 함수
typedef HMODULE (__stdcall * PROC_GetCurAlgorithm)();
typedef HMODULE (__stdcall * PROC_GetCurTranslator)();
typedef BOOL	(__stdcall * PROC_HookDllFunction)(LPCSTR cszDllName, LPCSTR cszFuncName, LPVOID pfnNewFunc);
typedef BOOL	(__stdcall * PROC_UnhookDllFunction)(LPCSTR cszDllName, LPCSTR cszFuncName);
typedef LPVOID	(__stdcall * PROC_GetOrigDllFunction)(LPCSTR cszDllName, LPCSTR cszFuncName);
typedef BOOL	(__stdcall * PROC_HookCodePoint)(LPVOID pCodePoint, PROC_HookCallback pfnCallback);
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
	PROC_UnhookDllFunction	pfnUnhookDllFunction;
	PROC_GetOrigDllFunction	pfnGetOrigDllFunction;
	PROC_HookCodePoint		pfnHookCodePoint;
	PROC_UnhookCodePoint	pfnUnhookCodePoint;
	PROC_TranslateText		pfnTranslateText;
	PROC_IsAppLocaleLoaded	pfnIsAppLocaleLoaded;
	PROC_EnableAppLocale	pfnEnableAppLocale;
	PROC_SuspendAllThread	pfnSuspendAllThread;
	PROC_ResumeAllThread	pfnResumeAllThread;
	PROC_IsAllThreadSuspended	pfnIsAllThreadSuspended;

} CONTAINER_FUNCTION_ENTRY, *PCONTAINER_FUNCTION_ENTRY;
