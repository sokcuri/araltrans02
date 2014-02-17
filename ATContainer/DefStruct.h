#pragma once


// 번역 플러그인 DLL 구조체

typedef BOOL (__stdcall * PROC_TRANS_OnPluginInit)(HWND,LPSTR);
typedef BOOL (__stdcall * PROC_TRANS_OnPluginOption)();
typedef BOOL (__stdcall * PROC_TRANS_OnPluginClose)();
typedef BOOL (__stdcall * PROC_TRANS_Translate)(LPCSTR,LPSTR,int);

typedef struct _TRANS_FUNCTION_ENTRY
{

	PROC_TRANS_OnPluginInit	pfnOnPluginInit;
	PROC_TRANS_OnPluginOption pfnOnPluginOption;
	PROC_TRANS_OnPluginClose	pfnOnPluginClose;
	PROC_TRANS_Translate pfnTranslate;

} TRANS_FUNCTION_ENTRY, *PTRANS_FUNCTION_ENTRY;




// 후킹 플러그인 DLL 구조체

typedef BOOL (__stdcall * PROC_HOOK_OnPluginInit)(HWND,LPSTR);
typedef BOOL (__stdcall * PROC_HOOK_OnPluginOption)();
typedef BOOL (__stdcall * PROC_HOOK_OnPluginStart)();
typedef BOOL (__stdcall * PROC_HOOK_OnPluginStop)();
typedef BOOL (__stdcall * PROC_HOOK_OnPluginClose)();

typedef struct _HOOKING_FUNCTION_ENTRY
{

	PROC_HOOK_OnPluginInit		pfnOnPluginInit;
	PROC_HOOK_OnPluginOption	pfnOnPluginOption;
	PROC_HOOK_OnPluginStart		pfnOnPluginStart;
	PROC_HOOK_OnPluginStop		pfnOnPluginStop;
	PROC_HOOK_OnPluginClose		pfnOnPluginClose;

} HOOKING_FUNCTION_ENTRY, *PHOOKING_FUNCTION_ENTRY;



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
