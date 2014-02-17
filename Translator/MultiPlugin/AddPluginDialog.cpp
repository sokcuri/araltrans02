#include "StdAfx.h"
#include "resource.h"

#include "AddPluginDialog.h"

#include "MultiPlugin.h"
#include "Util.h"

#include "tstring.h"

using namespace std;

static bool bIsRedundantAllowed=false;

BOOL AddPluginDialogUpdate(HWND hwndDlg)
{
	HWND hwndList=GetDlgItem(hwndDlg, IDC_ADDPLUGINLIST);
	HWND hwndRedundantCheck=GetDlgItem(hwndDlg, IDC_REDUNDANTCHECK);
	ATPLUGIN_ARGUMENT_ARRAY aPluginArgs;
	HANDLE hFindFile;
	WIN32_FIND_DATA findData;

	int i;
	bool bIsLoaded;

	TCHAR szPath[MAX_PATH];

	EnableWindow(hwndList, FALSE);

	SendMessage(hwndList, LB_RESETCONTENT, NULL, NULL);

	wsprintf(szPath, _T("%s\\Filter\\*.DLL"), GetATDirectory());

	g_cMultiPlugin.GetPluginArgs(aPluginArgs);

	// 플러그인 파일 검색
	hFindFile=FindFirstFile(szPath, &findData);

	if (hFindFile == INVALID_HANDLE_VALUE)
		return FALSE;
	
	while(true)
	{
		bIsLoaded=false;

		// 파일명에서 확장자 제거
		for (i=lstrlen(findData.cFileName); i>=0; i--)
		{
			if (findData.cFileName[i] == '.')
			{
				findData.cFileName[i]='\0';
				break;
			}
		}

		// 이미 로드된 플러그인이면 무시
		// 같은 폴더의 같은 DLL이 두번 로드되면 오작동될 수 있음
		if (!lstrcmpi(findData.cFileName, _T("MultiPlugin")))
		{
			bIsLoaded=true;
		}
		else if (!bIsRedundantAllowed)
		{
			for(i=0; i<aPluginArgs.size(); i++)
			{
				if ( !lstrcmpi(findData.cFileName, aPluginArgs[i].strPluginName.c_str()) )
				{
					bIsLoaded=true;
					break;
				}
			}
		}

		// 없으면 리스트에 추가
		if (!bIsLoaded)
		{
			SendMessage(hwndList, LB_ADDSTRING, 0, (LPARAM)findData.cFileName);
		}

		if (!FindNextFile(hFindFile, &findData))
			break;

	}	// while(true)...

	if (bIsRedundantAllowed)
		SendMessage(hwndRedundantCheck, BM_SETCHECK, BST_CHECKED, NULL);
	else
		SendMessage(hwndRedundantCheck, BM_SETCHECK, BST_UNCHECKED, NULL);
	EnableWindow(hwndList, TRUE);

	return TRUE;
}


BOOL CALLBACK AddPluginDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static TCHAR *pszPluginName=NULL;

	switch(uMsg)
	{
		case WM_INITDIALOG:
			pszPluginName=(TCHAR *)lParam;	// TCHAR szPluginName[MAX_PATH]
			AddPluginDialogUpdate(hwndDlg);
			return TRUE;
		
		case WM_COMMAND:
		{
			HWND hwndList=GetDlgItem(hwndDlg, IDC_ADDPLUGINLIST);
			int nSelected=SendMessage(hwndList, LB_GETCURSEL, 0, 0);
			switch(LOWORD(wParam))
			{
				case IDOK:
					if (nSelected == LB_ERR)
					{
						MessageBox(hwndDlg, _T("플러그인을 선택해 주세요."), _T("플러그인 추가"), MB_OK | MB_ICONINFORMATION);
						break;
					}
					else
						SendMessage(hwndList, LB_GETTEXT, (WPARAM)nSelected, (LPARAM) pszPluginName);
				case IDCANCEL:
					EndDialog(hwndDlg, LOWORD(wParam));
					break;

				case IDC_REDUNDANTCHECK:
					if (bIsRedundantAllowed)
					{
						bIsRedundantAllowed=false;
						AddPluginDialogUpdate(hwndDlg);
					}
					else if (MessageBox(hwndDlg,
						_T("플러그인 중복 추가는 예상치 못한 결과를 야기할 수 있습니다. 계속하시겠습니까?"), 
						_T("주의"), 
						MB_YESNO) == IDYES
						)
					{
						bIsRedundantAllowed=true;
						AddPluginDialogUpdate(hwndDlg);
					}
					break;

				case IDC_ADDPLUGINLIST:
					if (HIWORD(wParam) == LBN_DBLCLK)
						SendMessage(hwndDlg, WM_COMMAND, IDOK, NULL);
				default:;
			}
			return TRUE;
		}
		default:;

	return FALSE;
	}
}
