#include "StdAfx.h"
#include "resource.h"

#include "OptionDialog.h"
#include "CmdFilter.h"
#include "ATPlugin.h"

#include "tstring.h"

void OptionDialogInit(HWND hwndDlg)
{
	tstring strBoundary;
	char szTemp[1024];
	wsprintfA(szTemp, "%s : %s\r\n제작자 지나가다정착한이A, %s\r\n플러그인 버전 %d.%d.%d",
		g_pluginVer.szPluginName, g_pluginVer.szDescription,
		g_pluginVer.szAuthor, 
		g_pluginVer.PluginVersion.Major, g_pluginVer.PluginVersion.Minor, g_pluginVer.PluginVersion.BuildDate);
	
#ifdef _UNICODE
	WCHAR wszTemp[1024];
	Kor2Wide(szTemp, wszTemp, 1024);
	SetDlgItemText(hwndDlg, IDC_INFOSTATIC,wszTemp);
#else
	SetDlgItemText(hwndDlg, IDC_INFOSTATIC,szTemp);
#endif
	
	if (g_cCmdFilter.GetOverwriteSetting())
		SendMessage(GetDlgItem(hwndDlg, IDC_OVERWRITECHECK), BM_SETCHECK, BST_CHECKED, NULL);
	else
		SendMessage(GetDlgItem(hwndDlg, IDC_OVERWRITECHECK), BM_SETCHECK, BST_UNCHECKED, NULL);
	if (g_cCmdFilter.GetRemoveSpaceSetting())
		SendMessage(GetDlgItem(hwndDlg, IDC_REMOVESPACECHECK), BM_SETCHECK, BST_CHECKED, NULL);
	else
		SendMessage(GetDlgItem(hwndDlg, IDC_REMOVESPACECHECK), BM_SETCHECK, BST_UNCHECKED, NULL);
	if (g_cCmdFilter.GetTwoByteSetting())
		SendMessage(GetDlgItem(hwndDlg, IDC_TWOBYTECHECK), BM_SETCHECK, BST_CHECKED, NULL);
	else
		SendMessage(GetDlgItem(hwndDlg, IDC_TWOBYTECHECK), BM_SETCHECK, BST_UNCHECKED, NULL);

	SetDlgItemInt(hwndDlg, IDC_CUTSIZEEDIT, (UINT) g_cCmdFilter.GetCutSizeSetting(), FALSE);

	if (g_cCmdFilter.GetDenySetting(strBoundary))
		SetDlgItemText(hwndDlg, IDC_DENYEDIT, strBoundary.c_str());
	if (g_cCmdFilter.GetPrefixIncludeSetting(strBoundary))
		SetDlgItemText(hwndDlg, IDC_PREINCEDIT, strBoundary.c_str());
	if (g_cCmdFilter.GetPrefixExcludeSetting(strBoundary))
		SetDlgItemText(hwndDlg, IDC_PREEXCEDIT, strBoundary.c_str());
	if (g_cCmdFilter.GetBodyIncludeSetting(strBoundary))
		SetDlgItemText(hwndDlg, IDC_BODYINCEDIT, strBoundary.c_str());
	if (g_cCmdFilter.GetBodyExcludeSetting(strBoundary))
		SetDlgItemText(hwndDlg, IDC_BODYEXCEDIT, strBoundary.c_str());
}

void OptionStringRemoveSpace(const TCHAR *pszIn, tstring &strOut)
{
	int nSize = lstrlen(pszIn);
	strOut.erase();
	for (int i=0; i<nSize; i++)
	{
		if (pszIn[i] == ' ')
			continue;
		
		strOut+=pszIn[i];
	}
}

BOOL CALLBACK OptionDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_INITDIALOG:
			OptionDialogInit(hwndDlg);
			return TRUE;

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				case IDOK:
				{
					tstring strBoundary;
					TCHAR szBoundary[1024];

					if(SendMessage(GetDlgItem(hwndDlg, IDC_OVERWRITECHECK), BM_GETCHECK, NULL, NULL) == BST_CHECKED)
						g_cCmdFilter.SetOverwriteSetting(true);
					else
						g_cCmdFilter.SetOverwriteSetting(false);
					if(SendMessage(GetDlgItem(hwndDlg, IDC_REMOVESPACECHECK), BM_GETCHECK, NULL, NULL) == BST_CHECKED)
						g_cCmdFilter.SetRemoveSpaceSetting(true);
					else
						g_cCmdFilter.SetRemoveSpaceSetting(false);
					if(SendMessage(GetDlgItem(hwndDlg, IDC_TWOBYTECHECK), BM_GETCHECK, NULL, NULL) == BST_CHECKED)
						g_cCmdFilter.SetTwoByteSetting(true);
					else
						g_cCmdFilter.SetTwoByteSetting(false);

					g_cCmdFilter.SetCutSizeSetting((int)GetDlgItemInt(hwndDlg, IDC_CUTSIZEEDIT, NULL, TRUE));

					if (g_cCmdFilter.GetCutSizeSetting() < 0)
						g_cCmdFilter.SetCutSizeSetting(0);

					GetDlgItemText(hwndDlg, IDC_DENYEDIT, szBoundary, 1024);
					OptionStringRemoveSpace(szBoundary, strBoundary);
					g_cCmdFilter.SetDenySetting(strBoundary);
					
					GetDlgItemText(hwndDlg, IDC_PREINCEDIT, szBoundary, 1024);
					OptionStringRemoveSpace(szBoundary, strBoundary);
					g_cCmdFilter.SetPrefixIncludeSetting(strBoundary);
					GetDlgItemText(hwndDlg, IDC_PREEXCEDIT, szBoundary, 1024);
					OptionStringRemoveSpace(szBoundary, strBoundary);
					g_cCmdFilter.SetPrefixExcludeSetting(strBoundary);

					GetDlgItemText(hwndDlg, IDC_BODYINCEDIT, szBoundary, 1024);
					OptionStringRemoveSpace(szBoundary, strBoundary);
					g_cCmdFilter.SetBodyIncludeSetting(strBoundary);
					GetDlgItemText(hwndDlg, IDC_BODYEXCEDIT, szBoundary, 1024);
					OptionStringRemoveSpace(szBoundary, strBoundary);
					g_cCmdFilter.SetBodyExcludeSetting(strBoundary);

				}
				case IDCANCEL:
					EndDialog(hwndDlg, LOWORD(wParam));
					return TRUE;
				default:;
			}
			break;

		default:;

	}
	return FALSE;
}