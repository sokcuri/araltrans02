#pragma once

#include <set>
#include <map>

using namespace std;


// 스트링관련 오리지널 함수 엔트리
typedef DWORD (__stdcall * PROC_GetGlyphOutline)(HDC, UINT, UINT, LPGLYPHMETRICS, DWORD, LPVOID, CONST MAT2*);
typedef BOOL (__stdcall * PROC_TextOut)(HDC, int, int, LPCVOID, int);
typedef BOOL (__stdcall * PROC_ExtTextOut)(HDC, int, int, UINT, CONST RECT*, LPCVOID, UINT, CONST INT*);
typedef int (__stdcall * PROC_DrawText)(HDC, LPCVOID, int, LPRECT, UINT);
typedef int (__stdcall * PROC_DrawTextEx)(HDC, LPCVOID ,int ,LPRECT, UINT, LPDRAWTEXTPARAMS);
typedef int (__stdcall * PROC_WideCharToMultiByte)(UINT,DWORD,LPCWSTR,int,LPSTR,int,LPCSTR,LPBOOL);
typedef int (__stdcall * PROC_MultiByteToWideChar)(UINT,DWORD,LPCSTR,int,LPWSTR,int);

typedef struct _TEXT_FUNCTION_ENTRY
{

	PROC_GetGlyphOutline		pfnGetGlyphOutlineA;
	PROC_GetGlyphOutline		pfnGetGlyphOutlineW;
	PROC_TextOut				pfnTextOutA;
	PROC_TextOut				pfnTextOutW;
	PROC_ExtTextOut				pfnExtTextOutA;
	PROC_ExtTextOut				pfnExtTextOutW;
	PROC_DrawText				pfnDrawTextA;
	PROC_DrawText				pfnDrawTextW;
	PROC_DrawTextEx				pfnDrawTextExA;
	PROC_DrawTextEx				pfnDrawTextExW;
	PROC_WideCharToMultiByte	pfnOrigWideCharToMultiByte;
	PROC_MultiByteToWideChar	pfnOrigMultiByteToWideChar;

} TEXT_FUNCTION_ENTRY, *PTEXT_FUNCTION_ENTRY;


class CNonCachedTextArg;
typedef set<CNonCachedTextArg*> CNonCachedTextArgSet;

//////////////////////////////////////////////////////////////////////////
//
class CNonCachedTextArgMgr
{
private:
	//TEXT_FUNCTION_ENTRY*	m_pOrigTextFunc;			// 원래 함수 포인터
	BOOL					m_bApplocale;
	BOOL					m_bRunning;
	HMODULE					m_hContainer;
	HWND					m_hContainerWnd;
	LPSTR					m_pszOptionStringBuffer;


	HDC						m_hLastDC;
	HFONT					m_hFont;
	UINT					m_aDupCntTable[16];			// 중복 카운트 테이블
	CNonCachedTextArgSet	m_setActivatedArgs;			// 활성화된 텍스트 인자
	CNonCachedTextArgSet	m_setInactivatedArgs;		// 비활성화 되어 있는 인자들
	map<long, HFONT>		m_mapFonts;



	BOOL	AddTextArg(LPCWSTR wszText);		// 새로운 문자열 후보를 추가한다
	BOOL	TestCharacter(wchar_t wch);					// 문자열 후보들 전체를 테스트한다. (더이상 일치하지 않는 후보는 바로 삭제)
	BOOL	GetBestTranslatedCharacter(wchar_t* pTransResultBuf);		// 최고로 확률이 높은 번역 문자를 반환
	CNonCachedTextArg*	FindString(LPCWSTR pTestString, int nSize = -1);
	BOOL	GetTranslatedStringA(INT_PTR ptrBegin, LPCSTR szOrigString, int nOrigSize, wchar_t *pTransResultBuf, int nBufSize, int &nTransSize);
	BOOL	GetTranslatedStringW(INT_PTR ptrBegin, LPCWSTR wszOrigString, int nOrigSize, wchar_t *pTransResultBuf, int nBufSize, int &nTransSize);
	BOOL	IsEmpty();									// 현재 활성화 된 텍스트 인자들이 하나도 없는가?
	int		SearchStringA(INT_PTR ptrBegin, char ch1, char ch2);
	int		SearchStringW(INT_PTR ptrBegin, wchar_t wch);
	void	CheckFont(HDC hdc);


	DWORD InnerGetGlyphOutlineA(
		HDC hdc,             // handle to device context
		UINT uChar,          // character to query
		UINT uFormat,        // format of data to return
		LPGLYPHMETRICS lpgm, // pointer to structure for metrics
		DWORD cbBuffer,      // size of buffer for data
		LPVOID lpvBuffer,    // pointer to buffer for data
		CONST MAT2 *lpmat2   // pointer to transformation matrix structure
		);

	DWORD InnerGetGlyphOutlineW(
		HDC hdc,             // handle to device context
		UINT uChar,          // character to query
		UINT uFormat,        // format of data to return
		LPGLYPHMETRICS lpgm, // pointer to structure for metrics
		DWORD cbBuffer,      // size of buffer for data
		LPVOID lpvBuffer,    // pointer to buffer for data
		CONST MAT2 *lpmat2   // pointer to transformation matrix structure
		);

	BOOL InnerTextOutA(
		HDC hdc,           // handle to DC
		int nXStart,       // x-coordinate of starting position
		int nYStart,       // y-coordinate of starting position
		LPCSTR lpString,   // character string
		int cbString       // number of characters
		);

	BOOL InnerTextOutW(
		HDC hdc,           // handle to DC
		int nXStart,       // x-coordinate of starting position
		int nYStart,       // y-coordinate of starting position
		LPCWSTR lpString,   // character string
		int cbString       // number of characters
		);

	BOOL InnerExtTextOutA(
		HDC hdc,          // handle to DC
		int X,            // x-coordinate of reference point
		int Y,            // y-coordinate of reference point
		UINT fuOptions,   // text-output options
		CONST RECT* lprc, // optional dimensions
		LPCSTR lpString, // string
		UINT cbCount,     // number of characters in string
		CONST INT* lpDx   // array of spacing values
		);

	BOOL InnerExtTextOutW(
		HDC hdc,          // handle to DC
		int X,            // x-coordinate of reference point
		int Y,            // y-coordinate of reference point
		UINT fuOptions,   // text-output options
		CONST RECT* lprc, // optional dimensions
		LPCWSTR lpString, // string
		UINT cbCount,     // number of characters in string
		CONST INT* lpDx   // array of spacing values
		);

	int InnerDrawTextA(
		HDC hDC,          // handle to DC
		LPCSTR lpString,  // text to draw
		int nCount,       // text length
		LPRECT lpRect,    // formatting dimensions
		UINT uFormat      // text-drawing options
		);

	int InnerDrawTextW(
		HDC hDC,          // handle to DC
		LPCWSTR lpString, // text to draw
		int nCount,       // text length
		LPRECT lpRect,    // formatting dimensions
		UINT uFormat      // text-drawing options
		);

	int InnerDrawTextExA(
		HDC hdc,                     // handle to DC
		LPSTR lpchText,              // text to draw
		int cchText,                 // length of text to draw
		LPRECT lprc,                 // rectangle coordinates
		UINT dwDTFormat,             // formatting options
		LPDRAWTEXTPARAMS lpDTParams  // more formatting options
		);

	int InnerDrawTextExW(
		HDC hdc,                     // handle to DC
		LPWSTR lpchText,              // text to draw
		int cchText,                 // length of text to draw
		LPRECT lprc,                 // rectangle coordinates
		UINT dwDTFormat,             // formatting options
		LPDRAWTEXTPARAMS lpDTParams  // more formatting options
		);


	//////////////////////////////////////////////////////////////////////////
	// Static Functions
	static DWORD __stdcall NewGetGlyphOutlineA(
		HDC hdc,             // handle to device context
		UINT uChar,          // character to query
		UINT uFormat,        // format of data to return
		LPGLYPHMETRICS lpgm, // pointer to structure for metrics
		DWORD cbBuffer,      // size of buffer for data
		LPVOID lpvBuffer,    // pointer to buffer for data
		CONST MAT2 *lpmat2   // pointer to transformation matrix structure
		);

	static DWORD __stdcall NewGetGlyphOutlineW(
		HDC hdc,             // handle to device context
		UINT uChar,          // character to query
		UINT uFormat,        // format of data to return
		LPGLYPHMETRICS lpgm, // pointer to structure for metrics
		DWORD cbBuffer,      // size of buffer for data
		LPVOID lpvBuffer,    // pointer to buffer for data
		CONST MAT2 *lpmat2   // pointer to transformation matrix structure
		);

	static BOOL __stdcall NewTextOutA(
		HDC hdc,           // handle to DC
		int nXStart,       // x-coordinate of starting position
		int nYStart,       // y-coordinate of starting position
		LPCSTR lpString,   // character string
		int cbString       // number of characters
		);

	static BOOL __stdcall NewTextOutW(
		HDC hdc,           // handle to DC
		int nXStart,       // x-coordinate of starting position
		int nYStart,       // y-coordinate of starting position
		LPCWSTR lpString,   // character string
		int cbString       // number of characters
		);

	static BOOL __stdcall NewExtTextOutA(
		HDC hdc,          // handle to DC
		int X,            // x-coordinate of reference point
		int Y,            // y-coordinate of reference point
		UINT fuOptions,   // text-output options
		CONST RECT* lprc, // optional dimensions
		LPCSTR lpString, // string
		UINT cbCount,     // number of characters in string
		CONST INT* lpDx   // array of spacing values
		);

	static BOOL __stdcall NewExtTextOutW(
		HDC hdc,          // handle to DC
		int X,            // x-coordinate of reference point
		int Y,            // y-coordinate of reference point
		UINT fuOptions,   // text-output options
		CONST RECT* lprc, // optional dimensions
		LPCWSTR lpString, // string
		UINT cbCount,     // number of characters in string
		CONST INT* lpDx   // array of spacing values
		);

	static int __stdcall NewDrawTextA(
		HDC hDC,          // handle to DC
		LPCSTR lpString,  // text to draw
		int nCount,       // text length
		LPRECT lpRect,    // formatting dimensions
		UINT uFormat      // text-drawing options
		);

	static int __stdcall NewDrawTextW(
		HDC hDC,          // handle to DC
		LPCWSTR lpString, // text to draw
		int nCount,       // text length
		LPRECT lpRect,    // formatting dimensions
		UINT uFormat      // text-drawing options
		);

	static int __stdcall NewDrawTextExA(
		HDC hdc,                     // handle to DC
		LPSTR lpchText,              // text to draw
		int cchText,                 // length of text to draw
		LPRECT lprc,                 // rectangle coordinates
		UINT dwDTFormat,             // formatting options
		LPDRAWTEXTPARAMS lpDTParams  // more formatting options
		);

	static int __stdcall NewDrawTextExW(
		HDC hdc,                     // handle to DC
		LPWSTR lpchText,              // text to draw
		int cchText,                 // length of text to draw
		LPRECT lprc,                 // rectangle coordinates
		UINT dwDTFormat,             // formatting options
		LPDRAWTEXTPARAMS lpDTParams  // more formatting options
		);

public:
	CONTAINER_FUNCTION_ENTRY	m_sContainerFunc;
	TEXT_FUNCTION_ENTRY			m_sTextFunc;

	CNonCachedTextArgMgr(void);
	~CNonCachedTextArgMgr(void);

	BOOL Init(HWND hSettingWnd, LPSTR cszOptionStringBuffer);
	BOOL Start();
	BOOL Stop();
	BOOL Option();
	BOOL Close();

};
