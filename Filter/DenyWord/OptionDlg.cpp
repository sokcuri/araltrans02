#include "stdafx.h"

#include "resource.h"
#include "OptionDlg.h"

#include "ATPlugin.h"
#include "DenyWord.h"

#include <shellapi.h>
#include <commctrl.h>

void OptionDialogInit(HWND hwndDlg)
{
	g_cDenyWord.Load();

	SendMessage(GetDlgItem(hwndDlg, IDC_SPIN2), UDM_SETRANGE, 0, (LPARAM) MAKELONG ((short) 100, (short) 0));

	SendMessage(GetDlgItem(hwndDlg, IDC_SPIN1), UDM_SETRANGE, 0, (LPARAM) MAKELONG ((short) 100, (short) 0));

	tstring strBoundary;
	if (g_cDenyWord.GetNotKanjiSetting())
		SendMessage(GetDlgItem(hwndDlg, IDC_CHECK2), BM_SETCHECK, BST_CHECKED, NULL);
	else
		SendMessage(GetDlgItem(hwndDlg, IDC_CHECK2), BM_SETCHECK, BST_UNCHECKED, NULL);
	
	SetDlgItemInt(hwndDlg, IDC_EDIT5, (UINT) g_cDenyWord.GetPassKanjiSetting(), FALSE);
	SetDlgItemInt(hwndDlg, IDC_EDIT1, (UINT) g_cDenyWord.GetCutSetting(), FALSE);

	SetDlgItemInt(hwndDlg, IDC_EDIT2, (UINT) g_cDenyWord.GetDenyStringCount(), FALSE);
	SetDlgItemInt(hwndDlg, IDC_EDIT3, (UINT) g_cDenyWord.GetAllowStringCount(), FALSE);
	SetDlgItemInt(hwndDlg, IDC_EDIT4, (UINT) g_cDenyWord.GetExactStringCount(), FALSE);
}


BOOL CALLBACK OptionDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	TCHAR szDirectory[4096]=_T("");

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

					if(SendMessage(GetDlgItem(hwndDlg, IDC_CHECK2), BM_GETCHECK, NULL, NULL) == BST_CHECKED)
						g_cDenyWord.SetNotKanjiSetting(true);
					else
						g_cDenyWord.SetNotKanjiSetting(false);

					g_cDenyWord.SetPassKanjiSetting((int)GetDlgItemInt(hwndDlg, IDC_EDIT5, NULL, TRUE));
					g_cDenyWord.SetCutSetting((int)GetDlgItemInt(hwndDlg, IDC_EDIT1, NULL, TRUE));
				}
				case IDCANCEL:
					EndDialog(hwndDlg, LOWORD(wParam));
					return TRUE;

				case IDC_BUTTON1:					  
					SHELLEXECUTEINFO ExecInfo;  
					ExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);   
					ExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;   
					ExecInfo.hwnd = hwndDlg;   
					ExecInfo.lpVerb = _T("open");   
					ExecInfo.lpFile = _T("");   
					ExecInfo.lpParameters = NULL;
					GetModuleFileName(NULL, szDirectory, MAX_PATH);
					for(int i=lstrlen(szDirectory); i>=0; i--)
					{
						if (szDirectory[i] == _T('\\'))
						{
							szDirectory[i]=_T('\0');
							break;
						}
					}
					_tcscat(szDirectory,_T("\\ATData\\"));
					ExecInfo.lpDirectory=szDirectory;
					ExecInfo.nShow = SW_SHOW;   
					ExecInfo.hInstApp = g_hThisModule;
					  
					ShellExecuteEx(&ExecInfo);  
					break;
				default:;
			}
			break;

		default:;

	}
	return FALSE;
}