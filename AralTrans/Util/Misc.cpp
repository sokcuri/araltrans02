#include "../StdAfx.h"
#include "Misc.h"


//---------------------------------------------------------------------------
//	◎함수명 : GetFileVersion
//	◎함수설명 : 지정한 파일의 버전을 가르쳐준다.
//	◎인자 : CString 절대경로를 포함한 파일이름
//	◎반환값 : CString 형식의 버전 ("xx.xx.xx.xx")
//---------------------------------------------------------------------------
CString GetFileVersion(CString strFilePathName)
{
	DWORD dwHdlDest = 0;
	DWORD dwSizeDest = 0;
	DWORD dwDestLS, dwDestMS;
	CString strVersion = _T("");

	dwSizeDest = GetFileVersionInfoSize((LPTSTR)(LPCTSTR)strFilePathName, &dwHdlDest);

	if(dwSizeDest)
	{
		_TCHAR* pDestData = new TCHAR[dwSizeDest + 1];

		if(GetFileVersionInfo((LPTSTR)(LPCTSTR)strFilePathName, dwHdlDest, dwSizeDest, pDestData))
		{
			VS_FIXEDFILEINFO * pvsInfo;
			UINT uLen;
			BOOL dwRet;
			dwRet = VerQueryValue(pDestData, _T("\\"), (void**)&pvsInfo, &uLen);
			if(dwRet)
			{
				dwDestMS = pvsInfo->dwFileVersionMS;    // 주버전
				dwDestLS = pvsInfo->dwFileVersionLS;    // 보조버전

				int dwDestLS1, dwDestLS2, dwDestMS1, dwDestMS2;
				dwDestMS1 = (dwDestMS / 65536);
				dwDestMS2 = (dwDestMS % 65536);
				dwDestLS1 = (dwDestLS / 65536);
				dwDestLS2 = (dwDestLS % 65536);

				strVersion.Format( _T("%d.%d.%d.%d"),dwDestMS1,dwDestMS2,dwDestLS1,dwDestLS2);
				delete [] pDestData;
				return strVersion;
			}
		}
		delete [] pDestData;
	}
	return strVersion;
}