#ifndef __ADDPLUGINDIALOG_H__
#define __ADDPLUGINDIALOG_H__

// DialogBoxParam(......, (LPARAM) /* TCHAR */ szPluginName[ /*MAX_PATH*/ ]);
BOOL CALLBACK AddPluginDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

#endif // __ADDPLUGINDIALOG_H__