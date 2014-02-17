#include "StdAfx.h"
#include "ATPlugin.h"
#include "ATOptionDlg.h"

void OptionDialogInit(HWND hwndDlg)
{
	if (gMode[0])
		SendMessage(GetDlgItem(hwndDlg, IDC_CheckJpnText), BM_SETCHECK, BST_CHECKED, NULL);
	else
		SendMessage(GetDlgItem(hwndDlg, IDC_CheckJpnText), BM_SETCHECK, BST_UNCHECKED, NULL);

	if (gMode[1])
		SendMessage(GetDlgItem(hwndDlg, IDC_CheckJpnHex), BM_SETCHECK, BST_CHECKED, NULL);
	else
		SendMessage(GetDlgItem(hwndDlg, IDC_CheckJpnHex), BM_SETCHECK, BST_UNCHECKED, NULL);

	if (gMode[2])
		SendMessage(GetDlgItem(hwndDlg, IDC_CheckKorText), BM_SETCHECK, BST_CHECKED, NULL);
	else
		SendMessage(GetDlgItem(hwndDlg, IDC_CheckKorText), BM_SETCHECK, BST_UNCHECKED, NULL);

	if (gMode[3])
		SendMessage(GetDlgItem(hwndDlg, IDC_CheckKorHex), BM_SETCHECK, BST_CHECKED, NULL);
	else
		SendMessage(GetDlgItem(hwndDlg, IDC_CheckKorHex), BM_SETCHECK, BST_UNCHECKED, NULL);

	if (gMode[4])
		SendMessage(GetDlgItem(hwndDlg, IDC_CHECK1), BM_SETCHECK, BST_CHECKED, NULL);
	else
		SendMessage(GetDlgItem(hwndDlg, IDC_CHECK1), BM_SETCHECK, BST_UNCHECKED, NULL);
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
					if(SendMessage(GetDlgItem(hwndDlg, IDC_CheckJpnText), BM_GETCHECK, NULL, NULL) == BST_CHECKED)
						gMode[0]=true;
					else
						gMode[0]=false;
				
					if(SendMessage(GetDlgItem(hwndDlg, IDC_CheckJpnHex), BM_GETCHECK, NULL, NULL) == BST_CHECKED)
						gMode[1]=true;
					else
						gMode[1]=false;

					if(SendMessage(GetDlgItem(hwndDlg, IDC_CheckKorText), BM_GETCHECK, NULL, NULL) == BST_CHECKED)
						gMode[2]=true;
					else
						gMode[2]=false;

					if(SendMessage(GetDlgItem(hwndDlg, IDC_CheckKorHex), BM_GETCHECK, NULL, NULL) == BST_CHECKED)
						gMode[3]=true;
					else
						gMode[3]=false;

					if(SendMessage(GetDlgItem(hwndDlg, IDC_CHECK1), BM_GETCHECK, NULL, NULL) == BST_CHECKED)
						gMode[4]=true;
					else
						gMode[4]=false;

				case IDCANCEL:
					EndDialog(hwndDlg, LOWORD(wParam));
					return TRUE;
				default:;
			}
			return TRUE;
		default:;
	}
	return FALSE;
}