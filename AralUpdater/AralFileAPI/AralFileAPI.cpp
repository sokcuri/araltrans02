// AralFileAPI.cpp: implementation of the CFileUtil class.
//
//////////////////////////////////////////////////////////////////////

#ifndef WINVER				// Allow use of features specific to Windows XP or later.
#define WINVER 0x0501		// Change this to the appropriate value to target other versions of Windows.
#endif

#include <afx.h>
#include "AralFileAPI.h"
#include "FileOperations.h"
#include <shlwapi.h>

#pragma comment (lib, "shlwapi.lib")

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAralFileAPI::CAralFileAPI()
{

}

CAralFileAPI::~CAralFileAPI()
{

}


//////////////////////////////////////////////////////////////////////////
//
// 풀패스에서 파일 이름만 반환해 준다
//
//////////////////////////////////////////////////////////////////////////
CString CAralFileAPI::GetFileName(CString strFullPath)
{
	CString strRetVal = _T("");
	CString strOrigPath = strFullPath;
	strOrigPath.Replace('/', '\\');
	
	int nStrIdx = strOrigPath.ReverseFind('\\');
	if( nStrIdx<0 || (nStrIdx+1)>(strFullPath.GetLength()-1) ) return strRetVal;

	return strFullPath.Mid( nStrIdx+1 );
}


BOOL CAralFileAPI::CreateDirectoryRecursive(CString strDir)
{
	CFileFind find;
	if( find.FindFile( strDir ) == TRUE ) return TRUE;

	int nStrIdx = strDir.ReverseFind('\\');
	if( nStrIdx < 0 ) return FALSE;

	CString strParentDir = strDir.Left(nStrIdx);
	if( CreateDirectoryRecursive( strParentDir ) == FALSE ) return FALSE;

	return CreateDirectory( strDir, NULL );

}


//////////////////////////////////////////////////////////////////////////
//
// 파일의 확장자를 반환한다. ( '.'은 제외하고 반환됨 )
//
//////////////////////////////////////////////////////////////////////////
CString CAralFileAPI::GetFileExt(CString strFullPath)
{
	CString strRetVal = _T("");
	CString strOrigPath = strFullPath;
	
	int nStrIdx = strOrigPath.ReverseFind('.');
	if( nStrIdx<0 || (nStrIdx+1)>(strFullPath.GetLength()-1) ) return strRetVal;

	return strFullPath.Mid( nStrIdx+1 );
}

CString CAralFileAPI::GetFileAssociatedProgram(CString strPathOrExt)
{
	CString strExt;

	// ".xxx"형식으로 
	int nStrIdx = strPathOrExt.ReverseFind('.');
	if( nStrIdx<0 || (nStrIdx+1)>(strPathOrExt.GetLength()-1) ) strExt = "." + strPathOrExt;
	else strExt = strPathOrExt.Mid( nStrIdx );

	// 파일 형식에 적합한 실행파일을 얻어옴
	TCHAR szExecutable[1024] = {0,};
	DWORD Size = sizeof(szExecutable);
	AssocQueryString(0, ASSOCSTR_EXECUTABLE, (LPCTSTR)strExt, _T("Open"), szExecutable, &Size);

	return szExecutable;
}

//////////////////////////////////////////////////////////////////////////
//
// 파일의 디렉토리를 반환한다.
//
//////////////////////////////////////////////////////////////////////////
CString CAralFileAPI::GetFileDir(CString strFullPath)
{
	CString strRetVal = _T("");
	CString strOrigPath = strFullPath;
	strOrigPath.Replace('/', '\\');
	
	int nStrIdx = strOrigPath.ReverseFind('\\');
	if( nStrIdx<=0 ) return strRetVal;

	return strFullPath.Left( nStrIdx );	
}


//////////////////////////////////////////////////////////////////////////
//
// 디렉토리를 통째로 복사한다.
//
//////////////////////////////////////////////////////////////////////////
BOOL CAralFileAPI::CopyDirectory(CString strSrcDir, CString strTarDir)
{
	CFileOperation fo;      // create object
	fo.SetOverwriteMode(true); // reset OverwriteMode flag (optional)
	if (!fo.Copy(strSrcDir, strTarDir)) // do Copy
	{
		return FALSE;
		//fo.ShowError(); // if copy fails show error message
	}

	return TRUE;
}
