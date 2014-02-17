#ifndef __MISC_H__
#define __MISC_H__

inline void TimetToFileTime( time_t t, LPFILETIME pft )
{
    LONGLONG ll = Int32x32To64(t, 10000000) + 116444736000000000;
    pft->dwLowDateTime = (DWORD) ll;
    pft->dwHighDateTime = ll >>32;
}

CString GetFileVersion(CString strFilePathName);

#endif