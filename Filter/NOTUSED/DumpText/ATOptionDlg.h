#pragma once

#include "resource.h"
#include <commctrl.h>
#include "ATPlugin.h"
#include "Util.h"

BOOL CALLBACK OptionDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

void OptionDialogInit(HWND hwndDlg);