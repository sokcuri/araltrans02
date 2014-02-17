#pragma once

typedef BOOL (__stdcall * PROC_ExtTextOutW)(HDC hdc, int X, int Y, UINT fuOptions, CONST RECT* lprc, LPCWSTR lpString, UINT cbCount, CONST INT* lpDx);
typedef BOOL (__stdcall * PROC_SetMenu)(HWND hWnd, HMENU hMenu);


class CWin32UIMgr
{
private:
	static CWin32UIMgr* _Inst;
	HWND	m_hWnd;
	HMENU	m_hOldMenu;
	//HMENU	m_hNewMenu;	
	BOOL	m_bTitleTrans;
	BOOL	m_bMenuTrans;
	PROC_ExtTextOutW m_pfnOrigExtTextOutW;
	PROC_SetMenu m_pfnOrigSetMenu;
	CString m_strOldTitle;
	CString m_strNewTitle;
	CStringArray m_arrKorItem;
	CStringArray m_arrJpnItem;

	CString TranslateUnicodeText(LPCWSTR cwszText);
	int FindString(CStringArray& arr, LPCTSTR strToFind);
	void ScanMenuString(HMENU hMenu);
	void ChangeMenuRecursive(HMENU hMenu, CStringArray& arrSrc, CStringArray& arrTar);

	HMENU CreateMenuFromMenu(HMENU hOldMenu);		// 실패작 함수

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

	static BOOL __stdcall NewSetMenu(          
		HWND hWnd,
		HMENU hMenu
		);


public:
	CWin32UIMgr(void);
	~CWin32UIMgr(void);

	BOOL Init(HWND hWnd);
	void Close();

	void Invalidate();

	BOOL TransMenu();
	BOOL RestoreMenu();

	BOOL TransTitle();
	BOOL SetTitle(LPCTSTR szNewTitle);
	BOOL RestoreTitle();

};
