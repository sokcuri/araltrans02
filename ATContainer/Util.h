#ifndef __UTIL_H__
#define __UTIL_H__

#include "MultiPlugin.h"

const TCHAR *GetATDirectory();

void GetATPluginArgsFromOptionString(const char *pszOption, ATPLUGIN_ARGUMENT_ARRAY &aPluginArgs);

void GetOptionStringFromATPluginArgs(const ATPLUGIN_ARGUMENT_ARRAY &aPluginArgs, char *pszOption, int nMaxOptionLength);

#endif //__UTIL_H__