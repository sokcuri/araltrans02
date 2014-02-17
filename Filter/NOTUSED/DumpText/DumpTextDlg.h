#pragma once

#include "resource.h"
#include <commctrl.h>
#include "ATPlugin.h"
#include <vector>
#include <queue>
using namespace std;

BOOL CALLBACK DumpTextProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

void AddString(const wchar_t * pstr );

unsigned _stdcall CallThreadHandlerProc(void  *pThreadHandler);