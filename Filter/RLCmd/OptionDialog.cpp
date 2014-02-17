#include "StdAfx.h"
#include "resource.h"

#include "OptionDialog.h"
#include "RLCmd.h"
#include "ATPlugin.h"

#include "tstring.h"

void OptionDialogInit(HWND hwndDlg)
{
	char szTemp[1024];
	wsprintfA(szTemp, "%s : %s\r\n\r\n제작자 지나가다정착한이A, %s, 플러그인 버전 %d.%d.%d",
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
	
	if (g_cRLCmd.GetOverwriteSetting())
		SendMessage(GetDlgItem(hwndDlg, IDC_OVERWRITECHECK), BM_SETCHECK, BST_CHECKED, NULL);
	else
		SendMessage(GetDlgItem(hwndDlg, IDC_OVERWRITECHECK), BM_SETCHECK, BST_UNCHECKED, NULL);
	if (g_cRLCmd.GetRemoveSpaceSetting())
		SendMessage(GetDlgItem(hwndDlg, IDC_REMOVESPACECHECK), BM_SETCHECK, BST_CHECKED, NULL);
	else
		SendMessage(GetDlgItem(hwndDlg, IDC_REMOVESPACECHECK), BM_SETCHECK, BST_UNCHECKED, NULL);
	if (g_cRLCmd.GetBracketSetting())
		SendMessage(GetDlgItem(hwndDlg, IDC_BRACKETCHECK), BM_SETCHECK, BST_CHECKED, NULL);
	else
		SendMessage(GetDlgItem(hwndDlg, IDC_BRACKETCHECK), BM_SETCHECK, BST_UNCHECKED, NULL);
	if (g_cRLCmd.GetNoFreezeSetting())
		SendMessage(GetDlgItem(hwndDlg, IDC_NOFREEZECHECK), BM_SETCHECK, BST_CHECKED, NULL);
	else
		SendMessage(GetDlgItem(hwndDlg, IDC_NOFREEZECHECK), BM_SETCHECK, BST_UNCHECKED, NULL);

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
					if(SendMessage(GetDlgItem(hwndDlg, IDC_OVERWRITECHECK), BM_GETCHECK, NULL, NULL) == BST_CHECKED)
						g_cRLCmd.SetOverwriteSetting(true);
					else
						g_cRLCmd.SetOverwriteSetting(false);
					if(SendMessage(GetDlgItem(hwndDlg, IDC_REMOVESPACECHECK), BM_GETCHECK, NULL, NULL) == BST_CHECKED)
						g_cRLCmd.SetRemoveSpaceSetting(true);
					else
						g_cRLCmd.SetRemoveSpaceSetting(false);
					if(SendMessage(GetDlgItem(hwndDlg, IDC_BRACKETCHECK), BM_GETCHECK, NULL, NULL) == BST_CHECKED)
						g_cRLCmd.SetBracketSetting(true);
					else
						g_cRLCmd.SetBracketSetting(false);
					if(SendMessage(GetDlgItem(hwndDlg, IDC_NOFREEZECHECK), BM_GETCHECK, NULL, NULL) == BST_CHECKED)
						g_cRLCmd.SetNoFreezeSetting(true);
					else
						g_cRLCmd.SetNoFreezeSetting(false);


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