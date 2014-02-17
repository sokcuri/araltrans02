// HttpDownloader.h: interface for the CHttpDownloader class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HTTPDOWNLOADER_H__97BEB4F4_B175_4472_A168_BE791AD820A4__INCLUDED_)
#define AFX_HTTPDOWNLOADER_H__97BEB4F4_B175_4472_A168_BE791AD820A4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afx.h>

//---------------------------------------------------------------------------//
// Defines
//---------------------------------------------------------------------------//


#define HTTP_STATES_INIT			0		// 진입
#define HTTP_STATES_DOWNLOADING		1		// 다운로드 중
#define HTTP_STATES_ERROR			2		// 에러발생
#define HTTP_STATES_FINALIZE		3		// 종료

typedef int (*pUpdateProgressCallBack)
( 
	int		iStates,		// 진행 상황
	int		iErr,			// 오류 정보
	CString strSource,		// 원본 경로
	CString strTarget,		// 대상 경로(파일로 저장일 경우만)
	ULONGLONG TotalSize,	// 파일 총 크기
	ULONGLONG CurrentSize	// 현재까지 누적 다운로드 크기
);

#ifndef CallBackInfo

#define CallBackInfo(_pfunc, _iStates, _iErr, _strSource, _strTarget, _TotalSize, _CurrentSize)		\
		if( _pfunc ){ _pfunc( _iStates, _iErr, _strSource, _strTarget, _TotalSize, _CurrentSize); }

#endif
					



//---------------------------------------------------------------------------//
// Class
//---------------------------------------------------------------------------//

class CHttpDownloader  
{
private:
	CHttpDownloader();
	virtual ~CHttpDownloader();

public:

	//------------------------------------------//
	// HTTP 다운로드 : 파일로 저장
	//------------------------------------------//
	static int	Http_Download_To_File( 
		IN CString strURL, 
		IN CString strLocalPath, 
		IN PVOID pCallBack, 
		IN BOOL bCheckSize = FALSE, 
		IN ULONGLONG FileSize = 0  );

	
	//------------------------------------------//
	// HTTP 다운로드 : 버퍼에 저장
	//------------------------------------------//
	static int	Http_Download_To_Buffer( 
		IN CString strURL, 
		IN OUT CString &strBuf, 
		IN PVOID pCallBack );


};

#endif // !defined(AFX_HTTPDOWNLOADER_H__97BEB4F4_B175_4472_A168_BE791AD820A4__INCLUDED_)
