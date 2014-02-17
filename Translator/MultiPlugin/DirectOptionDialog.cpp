#include "StdAfx.h"
#include "resource.h"

#include "DirectOptionDialog.h"

#include "ATPlugin.h"
#include "Util.h"



BOOL CALLBACK DirectOptionDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		SetDlgItemTextA(hwndDlg, IDC_DIRECTOPTIONEDIT, g_szOptionStringBuffer);
		return TRUE;
		
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			GetDlgItemTextA(hwndDlg, IDC_DIRECTOPTIONEDIT, g_szOptionStringBuffer, 4096);
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