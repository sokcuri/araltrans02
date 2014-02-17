// Kirikiri.h : Kirikiri DLL의 기본 헤더 파일입니다.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH에 대해 이 파일을 포함하기 전에 'stdafx.h'를 포함합니다."
#endif

#include "resource.h"		// 주 기호입니다.
#include "OptionMgr.h"
#include "KAGScriptMgr.h"
#include "Win32UIMgr.h"
#include <map>

using namespace std;


// 스트링관련 오리지널 함수 엔트리
typedef DWORD (__stdcall * PROC_GetGlyphOutline)(HDC, UINT, UINT, LPGLYPHMETRICS, DWORD, LPVOID, CONST MAT2*);
//typedef BOOL (__stdcall * PROC_TextOut)(HDC, int, int, LPCVOID, int);

typedef int (__stdcall * PROC_WideCharToMultiByte)(UINT,DWORD,LPCWSTR,int,LPSTR,int,LPCSTR,LPBOOL);
typedef int (__stdcall * PROC_MultiByteToWideChar)(UINT,DWORD,LPCSTR,int,LPWSTR,int);

typedef HFONT (__stdcall * PROC_CreateFontIndirect)(LPVOID lpLogFont);
typedef HFONT (__stdcall * PROC_CreateFont)(
	int nHeight,               // height of font
	int nWidth,                // average character width
	int nEscapement,           // angle of escapement
	int nOrientation,          // base-line orientation angle
	int fnWeight,              // font weight
	DWORD fdwItalic,           // italic attribute option
	DWORD fdwUnderline,        // underline attribute option
	DWORD fdwStrikeOut,        // strikeout attribute option
	DWORD fdwCharSet,          // character set identifier
	DWORD fdwOutputPrecision,  // output precision
	DWORD fdwClipPrecision,    // clipping precision
	DWORD fdwQuality,          // output quality
	DWORD fdwPitchAndFamily,   // pitch and family
	LPVOID lpszFace           // typeface name
	);

typedef struct _TEXT_FUNCTION_ENTRY
{

	PROC_GetGlyphOutline		pfnGetGlyphOutlineW;
	//PROC_TextOut				pfnTextOutW;
	PROC_WideCharToMultiByte	pfnOrigWideCharToMultiByte;
	PROC_MultiByteToWideChar	pfnOrigMultiByteToWideChar;

} TEXT_FUNCTION_ENTRY, *PTEXT_FUNCTION_ENTRY;


typedef struct _FONT_FUNCTION_ENTRY
{

	PROC_CreateFont		pfnCreateFontA;
	PROC_CreateFont		pfnCreateFontW;
	PROC_CreateFontIndirect pfnCreateFontIndirectA;
	PROC_CreateFontIndirect pfnCreateFontIndirectW;

} FONT_FUNCTION_ENTRY, *PFONT_FUNCTION_ENTRY;


// CKirikiriApp
// 이 클래스의 구현을 보려면 Kirikiri.cpp를 참조하십시오.
//

class CKirikiriApp : public CWinApp
{
private:
	BOOL				m_bRunning;
	HMODULE				m_hContainer;
	HWND				m_hContainerWnd;
	LPSTR				m_szOptionString;
	COptionNode			m_optionRoot;

	CString				m_strLastFontFace;
	map<long, HFONT>	m_mapFonts;

	UINT_PTR			m_pCodePoint;
	BYTE				m_byteRegister;
	LPCWSTR				m_cwszOrigScript;
	LPWSTR				m_wszScriptBuf;
	CKAGScriptMgr2		m_ScriptMgr;
	CWin32UIMgr			m_UIMgr;
	
	BOOL				m_bUseCodePoint2;
	int					m_nCodePoint2Type;
	UINT_PTR			m_pCodePoint2;


	HFONT CheckFont( HDC hdc );
	void ResetOption();
	BOOL AdjustOption(COptionNode* pRootNode);
	BOOL HookKirikiri();
	BOOL HookKirikiri2();
	void UnhookKiriKiri2();
	void OnScriptLoad(PREGISTER_ENTRY pRegisters);
	void OnArrayLoad(PREGISTER_ENTRY pRegisters);

	static void PointCallback(LPVOID pHookedPoint, PREGISTER_ENTRY pRegisters);

	static DWORD __stdcall NewGetGlyphOutlineW(
		HDC hdc,             // handle to device context
		UINT uChar,          // character to query
		UINT uFormat,        // format of data to return
		LPGLYPHMETRICS lpgm, // pointer to structure for metrics
		DWORD cbBuffer,      // size of buffer for data
		LPVOID lpvBuffer,    // pointer to buffer for data
		CONST MAT2 *lpmat2   // pointer to transformation matrix structure
		);

	static HFONT __stdcall NewCreateFontIndirectA(LOGFONTA* lplf);
	static HFONT __stdcall NewCreateFontIndirectW(LOGFONTW* lplf);
	HFONT	InnerCreateFontIndirect(LOGFONTW* lplf);


public:
	long				m_lFontSize;
	CString				m_strFontFace;

	CONTAINER_FUNCTION_ENTRY	m_sContainerFunc;
	TEXT_FUNCTION_ENTRY			m_sTextFunc;
	FONT_FUNCTION_ENTRY			m_sFontFunc;

	CKirikiriApp();
	~CKirikiriApp();

	BOOL			ApplyOption( COptionNode* pRootNode );
	BOOL			ClearCache();
	CKAGScriptMgr*	GetKAGScriptMgr(){ return &m_ScriptMgr; };
	LPCWSTR			GetOriginalScript(){ return m_cwszOrigScript; };
	LPWSTR			GetScriptBuffer(){ return m_wszScriptBuf; };


	BOOL Init(HWND hSettingWnd, LPSTR cszOptionStringBuffer);
	BOOL Start();
	BOOL Stop();
	BOOL Option();
	BOOL Close();


	virtual BOOL InitInstance();
	virtual BOOL ExitInstance();

	DECLARE_MESSAGE_MAP()
};
