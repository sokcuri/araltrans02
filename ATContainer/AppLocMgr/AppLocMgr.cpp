#include <afx.h>
#include "AppLocMgr.h"
#include "../RawHookMgr/RawHookMgr.h"
#include "../RegistryMgr/cRegistryMgr.h"


char CAppLocMgr::_AppLocFuncName[APPLOC_FUNC_CNT][32] = {
	"GetACP",
	"GetCPInfo",
	"GetLocaleInfoA",
	"GetOEMCP",
	"GetSystemDefaultLangID",
	"GetSystemDefaultLCID",
	"IsDBCSLeadByte",
	"MultiByteToWideChar",
	"WideCharToMultiByte"
};


CAppLocMgr::CAppLocMgr(void)
{
}

CAppLocMgr::~CAppLocMgr(void)
{
}

BOOL CAppLocMgr::IsLoaded()
{
	BOOL bRetVal = FALSE;

	if( GetModuleHandle(_T("AlLayer.dll")) != NULL )
	{
		bRetVal = TRUE;
	}

	return bRetVal;
}

BOOL CAppLocMgr::SaveProcAddrToReg()
{
	BOOL bRetVal = FALSE;

	if( IsLoaded() == FALSE )
	{
		HMODULE hMod = GetModuleHandle(_T("kernel32.dll"));

		for(int i=0; i<APPLOC_FUNC_CNT; i++)
		{
			UINT_PTR pRelAddr = (UINT_PTR)GetProcAddress(hMod, _AppLocFuncName[i]);
			pRelAddr -= (UINT_PTR)hMod;
			CRegistryMgr::RegWriteDWORD(_T("HKEY_CURRENT_USER\\Software\\AralGood\\kernel32"), _AppLocFuncName[i], (DWORD)pRelAddr);
		}

		bRetVal = TRUE;
	}

	return bRetVal;
}

BOOL CAppLocMgr::Enable()
{
	BOOL bRetVal = FALSE;

	HMODULE hKernel32 = GetModuleHandle(_T("kernel32.dll"));
	HMODULE hAlLayer = GetModuleHandle(_T("AlLayer.dll"));

	if( hKernel32 && hAlLayer )
	{
		for(int i=0; i<APPLOC_FUNC_CNT; i++)
		{
			UINT_PTR pAddr = (UINT_PTR)GetProcAddress(hKernel32, _AppLocFuncName[i]);
			
			// 반환된 함수 주소가 AlLayer.dll의 범위에 있다면
			HMODULE hOwnerMod = NULL;
			if( GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCTSTR)pAddr, &hOwnerMod)
				&& hOwnerMod == hAlLayer )
			{
				CRawHookMgr::Unhook((LPVOID)pAddr);
			}

		}

		bRetVal = TRUE;
	}

	return bRetVal;
}

BOOL CAppLocMgr::Disable()
{
	BOOL bRetVal = FALSE;

	HMODULE hKernel32 = GetModuleHandle(_T("kernel32.dll"));
	HMODULE hAlLayer = GetModuleHandle(_T("AlLayer.dll"));

	if( hKernel32 && hAlLayer )
	{
		bRetVal = TRUE;

		for(int i=0; i<APPLOC_FUNC_CNT; i++)
		{
			UINT_PTR pAddr = (UINT_PTR)GetProcAddress(hKernel32, _AppLocFuncName[i]);

			// 반환된 함수 주소가 AlLayer.dll의 범위에 있다면
			HMODULE hOwnerMod = NULL;
			if( GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCTSTR)pAddr, &hOwnerMod)
				&& hOwnerMod == hAlLayer )
			{
				UINT_PTR pNewAddr = (UINT_PTR) CRegistryMgr::RegReadDWORD(_T("HKEY_CURRENT_USER\\Software\\AralGood\\kernel32"), _AppLocFuncName[i]);
				if(pNewAddr == 0)
				{
					bRetVal = FALSE;
					continue;
				}

				pNewAddr +=  (UINT_PTR)hKernel32;

				bRetVal = bRetVal && CRawHookMgr::Hook((LPVOID)pAddr, (LPVOID)pNewAddr);
			}

		}

	}

	return bRetVal;
}
