// FileUtil.h: interface for the CFileUtil class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ARALFILEUTIL_H__46FF629A_D7A7_4E2C_8FD5_00EB507612F3__INCLUDED_)
#define AFX_ARALFILEUTIL_H__46FF629A_D7A7_4E2C_8FD5_00EB507612F3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CAralFileAPI
{
private:
	CAralFileAPI();
	virtual ~CAralFileAPI();

public:
	static CString GetFileDir(CString strFullPath);
	static CString GetFileName(CString strFullPath);
	static CString GetFileExt(CString strFullPath);
	static CString GetFileAssociatedProgram(CString strPathOrExt);
	static BOOL CreateDirectoryRecursive(CString strDir);
	static BOOL CopyDirectory(CString strSrcDir, CString strTarDir);

};

#endif // !defined(AFX_FILEUTIL_H__46FF629A_D7A7_4E2C_8FD5_00EB507612F3__INCLUDED_)
