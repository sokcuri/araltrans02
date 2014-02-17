#include "stdafx.h"
#include "Win32UIMgr.h"
#include "ATCodeMgr.h"


CWin32UIMgr* CWin32UIMgr::_Inst = NULL;


CWin32UIMgr::CWin32UIMgr(void)
	: m_hWnd(NULL), 
	m_hOldMenu(NULL), 
	m_bMenuTrans(FALSE),
	m_bTitleTrans(FALSE), 
	m_pfnOrigExtTextOutW(NULL), 
	m_pfnOrigSetMenu(NULL)
{
	_Inst = this;
}

CWin32UIMgr::~CWin32UIMgr(void)
{
	Close();
	_Inst = NULL;
}

BOOL CWin32UIMgr::FindGameWindow()
{
	m_hWnd = NULL;
	m_dwPID = GetCurrentProcessId();
	
	// EnumWindows를 호출해준다
	EnumWindows(EnumWindowsProc, (LPARAM)this);

	return (m_hWnd ? TRUE : FALSE);
}

BOOL CALLBACK CWin32UIMgr::EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	DWORD dwProcessId = 0;
	DWORD dwThreadId = 0;
	dwThreadId = ::GetWindowThreadProcessId( hwnd, &dwProcessId );

	if( lParam && dwProcessId && dwThreadId )
	{
		CWin32UIMgr* pThis = (CWin32UIMgr*)lParam;

		if(pThis->m_dwPID == dwProcessId)
		{
			CRect rect;
			if(GetWindowRect(hwnd, &rect) && rect.Width()>=640 && rect.Height()>=480)
			{
				pThis->m_hWnd = hwnd;
				return FALSE;
			}
		}
	}

	return TRUE;
}

CString CWin32UIMgr::TranslateUnicodeText(LPCWSTR cwszText, UINT nCP)
{
	CString strRetVal = _T("");

	if(CATCodeMgr::GetInstance()->m_sContainerFunc.pfnTranslateText)
	{
		char szSrcBuf[1024];
		char szTarBuf[1024];
		ZeroMemory(szSrcBuf, 1024);
		ZeroMemory(szTarBuf, 1024);

		MyWideCharToMultiByte(nCP, 0, cwszText, -1, szSrcBuf, 1024, NULL, NULL);
		
		if( CATCodeMgr::GetInstance()->m_sContainerFunc.pfnTranslateText(szSrcBuf, szTarBuf, 1024) )
		{
			wchar_t wszTmp[1024];
			ZeroMemory(wszTmp, 1024*sizeof(wchar_t));
			MyMultiByteToWideChar(949, 0, szTarBuf, -1, wszTmp, 1024);
			strRetVal = wszTmp;
		}

	}

	return strRetVal;
}

BOOL CWin32UIMgr::Init()
{
	BOOL bRetVal = FALSE;

	Close();

	if( FindGameWindow() == FALSE || IsWindow(m_hWnd) == FALSE ) return FALSE;


	// 메뉴 얻기
	m_hOldMenu = GetMenu(m_hWnd);
	
	// 타이틀 얻기
	GetCurWindowTitle();

	// 모든 쓰레드 정지
	CATCodeMgr::GetInstance()->m_sContainerFunc.pfnSuspendAllThread();	
	
	// ExtTextOutW 함수 후킹
	if( CATCodeMgr::GetInstance()->m_sContainerFunc.pfnHookDllFunctionEx( "GDI32.DLL", "ExtTextOutW", NewExtTextOutW, 1 ) )
	{
		m_pfnOrigExtTextOutW = 
			(PROC_ExtTextOutW) CATCodeMgr::GetInstance()->m_sContainerFunc.pfnGetOrigDllFunction("GDI32.DLL", "ExtTextOutW");
	}

	// SetMenu 함수 후킹
	if( CATCodeMgr::GetInstance()->m_sContainerFunc.pfnHookDllFunctionEx( "USER32.DLL", "SetMenu", NewSetMenu, 1 ) )
	{
		m_pfnOrigSetMenu = 
			(PROC_SetMenu) CATCodeMgr::GetInstance()->m_sContainerFunc.pfnGetOrigDllFunction("USER32.DLL", "SetMenu");
		TRACE("[aral1] SetMenu hooked");
	}

	// 모든 쓰레드 재가동
	CATCodeMgr::GetInstance()->m_sContainerFunc.pfnResumeAllThread();

	if(m_pfnOrigExtTextOutW && m_pfnOrigSetMenu) bRetVal = TRUE;

	if(FALSE == bRetVal) Close();

	return bRetVal;
}


void CWin32UIMgr::Close()
{
	// 타이틀 & 메뉴 복구
	RestoreTitle();
	RestoreMenu();

	// ExtTextOutW 언훅
	if( m_pfnOrigSetMenu )
	{
		CATCodeMgr::GetInstance()->m_sContainerFunc.pfnUnhookDllFunction( "USER32.DLL", "SetMenu" );
		m_pfnOrigSetMenu = NULL;
	}

	// ExtTextOutW 언훅
	if( m_pfnOrigExtTextOutW )
	{
		CATCodeMgr::GetInstance()->m_sContainerFunc.pfnUnhookDllFunction( "GDI32.DLL", "ExtTextOutW" );
		m_pfnOrigExtTextOutW = NULL;
	}


	m_hWnd			= NULL;
	m_hOldMenu		= NULL;
	m_bMenuTrans	= FALSE;
	m_bTitleTrans	= FALSE;
	m_strOldTitle	= _T("");
	m_strNewTitle	= _T("");
	m_arrKorItem.RemoveAll();
	m_arrJpnItem.RemoveAll();
}

void CWin32UIMgr::GetCurWindowTitle()
{
	m_strOldTitle = _T("");
	m_strNewTitle = _T("");
	
	// 원래 윈도우 타이틀 얻어 놓기
	TCHAR szTitle[MAX_PATH] = {0,};
	if(GetWindowText(m_hWnd, szTitle, MAX_PATH))
	{
		m_strOldTitle = szTitle;
		UINT nCodePage = 932;

		// 어플로케일이 없으면
		if(CATCodeMgr::GetInstance()->m_sContainerFunc.pfnIsAppLocaleLoaded() == FALSE)
		{
			nCodePage = 949;
		}

		m_strNewTitle = TranslateUnicodeText(m_strOldTitle, nCodePage);
		m_strNewTitle.Replace(_T("?"), _T("~"));
	}
}

void CWin32UIMgr::Invalidate()
{

}



BOOL CWin32UIMgr::TransMenu()
{	
	TRACE("[aral1] TransMenu start");

	m_bMenuTrans = TRUE;

	BOOL bRetVal = FALSE;
	
	if( m_hOldMenu )
	{
		ScanMenuString(m_hOldMenu);
		ChangeMenuRecursive(m_hOldMenu, m_arrJpnItem, m_arrKorItem);
		bRetVal = m_pfnOrigSetMenu(m_hWnd, m_hOldMenu);
		DrawMenuBar(m_hWnd);
	}
	
	TRACE("[aral1] TransMenu End");

	return bRetVal;
}



BOOL CWin32UIMgr::RestoreMenu()
{
	TRACE("[aral1] RestoreMenu start");

	m_bMenuTrans = FALSE;

	BOOL bRetVal = FALSE;

	if( m_hOldMenu )
	{
		ScanMenuString(m_hOldMenu);
		ChangeMenuRecursive(m_hOldMenu, m_arrKorItem, m_arrJpnItem);
		bRetVal = m_pfnOrigSetMenu(m_hWnd, m_hOldMenu);		
		DrawMenuBar(m_hWnd);
	}

	TRACE("[aral1] RestoreMenu end");

	return bRetVal;
}



BOOL CWin32UIMgr::TransTitle()
{
	m_bTitleTrans = TRUE;
	if( m_hWnd && ::IsWindow(m_hWnd) )
	{
		SetWindowText(m_hWnd, m_strOldTitle);
	}

	return TRUE;

}



BOOL CWin32UIMgr::SetTitle(LPCTSTR szNewTitle)
{
	m_strNewTitle = szNewTitle;
	return TransTitle();
}



BOOL CWin32UIMgr::RestoreTitle()
{
	m_bTitleTrans = FALSE;
	
	if( !m_strOldTitle.IsEmpty()
		&& m_hWnd 
		&& SetWindowText(m_hWnd, m_strOldTitle) )
	{
		return TRUE;
	}

	return FALSE;	
}



//////////////////////////////////////////////////////////////////////////
//
// ExtTextOutW 대체 함수
//
//////////////////////////////////////////////////////////////////////////
BOOL __stdcall CWin32UIMgr::NewExtTextOutW(
	HDC hdc,          // handle to DC
	int X,            // x-coordinate of reference point
	int Y,            // y-coordinate of reference point
	UINT fuOptions,   // text-output options
	CONST RECT* lprc, // optional dimensions
	LPCWSTR lpString, // string
	UINT cbCount,     // number of characters in string
	CONST INT* lpDx   // array of spacing values
	)
{
	static CString strCuttingOldTitle = _T("");
	static CString strCuttingNewTitle = _T("");
	BOOL bRetVal = FALSE;
	CString strNewString = lpString;

	if(CWin32UIMgr::_Inst && CWin32UIMgr::_Inst->m_pfnOrigExtTextOutW )
	{
		CString strTemp = _T("");

		if( CWin32UIMgr::_Inst->m_bTitleTrans
			&& lpString
			&& lpString[0] )
		{
			//OutputDebugStringW(lpString);
			
			if(strCuttingOldTitle.IsEmpty())
			{
				//CWin32UIMgr::_Inst->GetCurWindowTitle();
				strCuttingOldTitle = CWin32UIMgr::_Inst->m_strOldTitle;
				strCuttingNewTitle = CWin32UIMgr::_Inst->m_strNewTitle;
			}
			
			//int nFindIdx = CWin32UIMgr::_Inst->m_strOldTitle.Find(lpString);
			//if( nFindIdx >= 0 )
			if(strCuttingOldTitle.Compare(lpString) == 0 && strCuttingOldTitle.IsEmpty() == FALSE)
			{
				strNewString = strCuttingNewTitle;

				int idx1 = strCuttingOldTitle.Find(_T('?'));
				int idx2 = strCuttingNewTitle.Find(_T('~'));
				if(idx1 == 0 && idx2 == 0)
				{
					strCuttingOldTitle.Delete(0);
					strCuttingNewTitle.Delete(0);
				}
				else if( idx1 > 0 && idx2 > 0 )
				{
					strCuttingOldTitle = strCuttingOldTitle.Mid(idx1);
					strCuttingNewTitle = strCuttingNewTitle.Mid(idx2);
				}
				else
				{
					strCuttingOldTitle = _T("");
					strCuttingNewTitle = _T("");
				}
			}
			else if(CWin32UIMgr::_Inst->m_strOldTitle.Compare(lpString) == 0)
			{
				//strNewString = (LPCWSTR)CWin32UIMgr::_Inst->m_strNewTitle.Mid(nFindIdx);
				strNewString = CWin32UIMgr::_Inst->m_strNewTitle;
			}
			else
			{
				int cnt = CWin32UIMgr::_Inst->m_arrJpnItem.GetCount();
				for(int i=0; i<cnt; i++)
				{
					strTemp = CWin32UIMgr::_Inst->m_arrJpnItem[i];
					strTemp.Replace(L"&", L"");
					if(strTemp.Compare(lpString) == 0)
					{
						strTemp = CWin32UIMgr::_Inst->m_arrKorItem[i];
						strTemp.Replace(L"&", L"");
						strNewString = strTemp;
						break;
					}
				}
			}
		}

		cbCount = strNewString.GetLength();
		bRetVal = CWin32UIMgr::_Inst->m_pfnOrigExtTextOutW(hdc, X, Y, fuOptions, lprc, (LPCWSTR)strNewString, cbCount, lpDx);
		
	}

	return bRetVal;
}



//////////////////////////////////////////////////////////////////////////
//
// SetMenu 대체 함수
//
//////////////////////////////////////////////////////////////////////////
BOOL __stdcall CWin32UIMgr::NewSetMenu(HWND hWnd, HMENU hMenu)
{
	BOOL bRetVal = FALSE;

	if(CWin32UIMgr::_Inst && CWin32UIMgr::_Inst->m_pfnOrigSetMenu )
	{
		if(hWnd && hMenu && CWin32UIMgr::_Inst->m_hWnd == hWnd)
		{
			CWin32UIMgr::_Inst->m_hOldMenu = hMenu;

			if(CWin32UIMgr::_Inst->m_bMenuTrans)
			{
				bRetVal = CWin32UIMgr::_Inst->TransMenu();
			}
			else
			{
				bRetVal = CWin32UIMgr::_Inst->RestoreMenu();
			}

		}
		else
		{
			bRetVal = CWin32UIMgr::_Inst->m_pfnOrigSetMenu(hWnd, hMenu);
		}
		

	}

	return bRetVal;
}



void CWin32UIMgr::ScanMenuString(HMENU hMenu)
{
	if(NULL == hMenu) return;
	
	int nItemCnt = GetMenuItemCount(hMenu);
	for(int i=0; i<nItemCnt; i++)
	{
		TCHAR szMenuString[256] = {0,};

		MENUITEMINFO mii;
		ZeroMemory(&mii, sizeof(MENUITEMINFO));
		mii.cbSize = sizeof(MENUITEMINFO);
		mii.dwTypeData = szMenuString;
		mii.cch = 256;
		mii.fMask = MIIM_FTYPE | MIIM_DATA | MIIM_ID | MIIM_STATE | MIIM_SUBMENU | MIIM_STRING;

		if( GetMenuItemInfo(hMenu, i, TRUE, &mii) )
		{

			//TRACE(_T("[aral1] ID:%s "), mii.dwTypeData);

			UINT nItemID = 0;
			UINT uFlags = mii.fType;


			// 하부 메뉴를 가진 아이템이면					
			if(mii.hSubMenu)
			{
				ScanMenuString(mii.hSubMenu);
			}

			// 처음 보는 스트링이면 스트링 저장
			if( szMenuString[0]
				&& FindString(m_arrKorItem, szMenuString) < 0 
				&& FindString(m_arrJpnItem, szMenuString) < 0 )
			{
				// 일본어로 간주
				CString strKor = TranslateUnicodeText(szMenuString);

				// 추가
				if(!strKor.IsEmpty())
				{
					m_arrJpnItem.Add(szMenuString);
					m_arrKorItem.Add(strKor);
				}
			}	
		}
	}// end of for

}




void CWin32UIMgr::ChangeMenuRecursive( HMENU hMenu, CStringArray& arrSrc, CStringArray& arrTar )
{
	if(NULL == hMenu) return;

	int nItemCnt = GetMenuItemCount(hMenu);
	for(int i=0; i<nItemCnt; i++)
	{
		TCHAR szMenuString[256] = {0,};

		MENUITEMINFO mii;
		ZeroMemory(&mii, sizeof(MENUITEMINFO));
		mii.cbSize = sizeof(MENUITEMINFO);
		mii.dwTypeData = szMenuString;
		mii.cch = 256;
		mii.fMask = MIIM_FTYPE | MIIM_DATA | MIIM_ID | MIIM_STATE | MIIM_SUBMENU | MIIM_STRING;

		if( GetMenuItemInfo(hMenu, i, TRUE, &mii) )
		{

			//TRACE(_T("[aral1] ID:%s "), mii.dwTypeData);

			UINT nItemID = 0;
			UINT uFlags = mii.fType;


			// 하부 메뉴를 가진 아이템이면					
			if(mii.hSubMenu)
			{
				ChangeMenuRecursive(mii.hSubMenu, arrSrc, arrTar);
			}

			// 스크링 교체
			int idx = FindString(arrSrc, szMenuString);
			if( 0 <= idx && idx < arrTar.GetCount() )
			{
				
				mii.dwTypeData = (LPTSTR)(LPCTSTR)arrTar[idx];
				mii.cch = arrTar[idx].GetLength();
				BOOL bRes = SetMenuItemInfo(hMenu, i, TRUE, &mii);

			}	
		}
	}// end of for
}




int CWin32UIMgr::FindString(CStringArray& arr, LPCTSTR strToFind)
{
	int cnt = arr.GetCount();

	for(int i=0; i<cnt; i++)
	{
		if(arr[i].Compare(strToFind) == 0) return i;
	}

	return -1;
}
