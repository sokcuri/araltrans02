#include "StdAfx.h"

#include "Debug.h"

#if defined(_DEBUG) || defined(FORCE_DEBUG_LOG)

// DEBUG_LOG_TYPE : 디버그 메세지 출력 방식 지정
// 0 - OutputDebugString
// 1 - MessageBox
// 2 - fprintf
// 3 - Log window (아직 지원않음)
#ifndef DEBUG_LOG_TYPE
#define DEBUG_LOG_TYPE	0
#endif

// DEBUG_WINDOW_CAPTION : 디버그 윈도우 캡션 (type 1, 3 에서만 의미 있음)
#ifndef DEBUG_WINDOW_CAPTION
#define DEBUG_WINDOW_CAPTION "Debug"
#endif

// DEBUG_LOG_FILEPATH : 디버그 로그 파일 지정 (type 2 에서만 의미 있음)
#ifndef DEBUG_LOG_FILEPATH
#define DEBUG_LOG_FILEPATH	"C:\\Debug.log"
#endif

CDebugLog g_cDebugLog;

CDebugLog::CDebugLog() : m_fp(NULL)
{
	// DEBUG_LOG_TYPE : 디버그 메세지 출력 방식 지정
	// 0 - OutputDebugString
	// 1 - MessageBox
	// 2 - fprintf
	// 3 - Log window (아직 지원않음)
	if (DEBUG_LOG_TYPE == 2)
	{
		m_fp = fopen(DEBUG_LOG_FILEPATH, "wb");
#ifdef UNICODE
		fputc(0xFF, m_fp); fputc(0xFE, m_fp);	// BOM (UTF-16LE)
#endif
	}
}

void CDebugLog::Log(LPCTSTR cszFormat, ...)
{
	LPTSTR szLog = new TCHAR[4096];

	va_list args;

	va_start(args, cszFormat);

	_vsntprintf(szLog, 4096, cszFormat, args);
	
	va_end(args);
	szLog[4095] = NULL;

	if (DEBUG_LOG_TYPE == 0)
		OutputDebugString(szLog);
	else if (DEBUG_LOG_TYPE == 1)
		MessageBox(NULL, szLog, _T(DEBUG_WINDOW_CAPTION), MB_OK | MB_ICONINFORMATION);
	else if (DEBUG_LOG_TYPE == 2)
	{	
		_ftprintf(m_fp, _T("%s"), szLog);
		fflush(m_fp);
	}
	else
		;	// not yet implemented
	
	delete[] szLog;
}



void NotifyError(LPCTSTR cszFileName, DWORD dwLineNo, DWORD dwErrCode)
{
	LPVOID lpMsgBuf;

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dwErrCode,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR) &lpMsgBuf,
		0,
		NULL );

	g_cDebugLog.Log(_T("Error: %sFile: %s\r\nLine: %d\r\n"), (LPTSTR) lpMsgBuf, cszFileName, dwLineNo);
	
	// Free the buffer.
	LocalFree( lpMsgBuf );
}



#endif // _DEBUG