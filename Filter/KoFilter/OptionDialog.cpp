#include "StdAfx.h"
#include "resource.h"

#include <commctrl.h>

#include "OptionDialog.h"

#include "ATPlugin.h"

#include "Util.h"

int nLevel;

void OptionDialogInit(HWND hwndDlg);
void OptionDialogUpdate(HWND hwndDlg);

void OptionDialogInit(HWND hwndDlg)
{
	HWND hwndSlider = GetDlgItem(hwndDlg, IDC_SETTINGSLIDER);
	char szTemp[1024];
	wsprintfA(szTemp, "%s\r\n\r\n지나가다정착한이A, Hide_D, %s\r\n버전 : %d.%d.%d",
		g_pluginVer.szDescription,
		g_pluginVer.szAuthor, 
		g_pluginVer.PluginVersion.Major, g_pluginVer.PluginVersion.Minor, g_pluginVer.PluginVersion.BuildDate);
	
#ifdef _UNICODE
	WCHAR wszTemp[1024];
	Kor2Wide(szTemp, wszTemp, 1024);
	SetDlgItemText(hwndDlg, IDC_INFOSTATIC,wszTemp);

	swprintf(wszTemp,L"%d",g_nDeny);
	SetDlgItemText(hwndDlg, IDC_Deny, wszTemp);

	swprintf(wszTemp,L"%d",g_nAllow);
	SetDlgItemText(hwndDlg, IDC_Allow, wszTemp);

#else
	SetDlgItemText(hwndDlg, IDC_INFOSTATIC,szTemp);

	sprintf(szTemp,L"%d",g_nDeny);
	SetDlgItemText(hwndDlg, IDC_Deny, szTemp);

	sprintf(szTemp,L"%d",g_nAllow);
	SetDlgItemText(hwndDlg, IDC_Allow, szTemp);
#endif
	SendMessage(hwndSlider, TBM_SETRANGE, FALSE, MAKELPARAM(0,4));
	SendMessage(hwndSlider, TBM_SETPOS, TRUE, (LPARAM) (nLevel/5));

	if (g_bNoRoman)
		SendMessage(GetDlgItem(hwndDlg, IDC_NOROMANCHECK), BM_SETCHECK, BST_CHECKED, NULL);
	else
		SendMessage(GetDlgItem(hwndDlg, IDC_NOROMANCHECK), BM_SETCHECK, BST_UNCHECKED, NULL);
	if (g_bNoRound)
		SendMessage(GetDlgItem(hwndDlg, IDC_NOROUNDCHECK), BM_SETCHECK, BST_CHECKED, NULL);
	else
		SendMessage(GetDlgItem(hwndDlg, IDC_NOROUNDCHECK), BM_SETCHECK, BST_UNCHECKED, NULL);

	OptionDialogUpdate(hwndDlg);
}

void OptionDialogUpdate(HWND hwndDlg)
{
	TCHAR *pSettingInfo;
	switch(nLevel)
	{
		case 0:
			pSettingInfo=_T("일어가 1글자이상이면 번역");
			break;
		case 5:
			pSettingInfo=_T("한글 > 일어인 경우 번역 중지");
			break;
		case 10:
			pSettingInfo=_T("한글 > 일어×2인 경우 번역 중지");
			break;
		case 15:
			pSettingInfo=_T("한글 > 일어×3인 경우 번역 중지");
			break;
		case 20:
			pSettingInfo=_T("한글이 1글자 이상이면 번역 중지");
			break;
		default:
			pSettingInfo=_T("잘못된 옵션");
	}
	SetDlgItemText(hwndDlg, IDC_SETTINGSTATIC, pSettingInfo);

}

BOOL CALLBACK OptionDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_INITDIALOG:
			nLevel=g_nLevel;
			OptionDialogInit(hwndDlg);
			return TRUE;

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				case IDOK:
					g_nLevel=nLevel;
					if(SendMessage(GetDlgItem(hwndDlg, IDC_NOROMANCHECK), BM_GETCHECK, NULL, NULL) == BST_CHECKED)
						g_bNoRoman=true;
					else
						g_bNoRoman=false;
					if(SendMessage(GetDlgItem(hwndDlg, IDC_NOROUNDCHECK), BM_GETCHECK, NULL, NULL) == BST_CHECKED)
						g_bNoRound=true;
					else
						g_bNoRound=false;

				case IDCANCEL:
					EndDialog(hwndDlg, LOWORD(wParam));
					return TRUE;
				default:;
			}
			return TRUE;

		case WM_HSCROLL:
			nLevel=SendMessage(GetDlgItem(hwndDlg, IDC_SETTINGSLIDER), TBM_GETPOS, 0, 0) * 5;

			OptionDialogUpdate(hwndDlg);
			return TRUE;

		default:;
	}
	return FALSE;
}