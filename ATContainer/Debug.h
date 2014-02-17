// Debug - 디버그용 에러 표시 라이브러리
//

#ifndef __DEBUG_H__
#define __DEBUG_H__

// FORCE_DEBUG_LOG : 보통 디버그 모드일때는 디버그 메세지 출력, 릴리즈 모드일때는 무시
//                   하지만 FORCE_DEBUG_LOG 가 켜져있으면 무조건 디버그 메세지를 출력
//#define FORCE_DEBUG_LOG

#define DEBUG_LOG_TYPE	2
#define DEBUG_LOG_FILEPATH	"D:\\ATCTNR.log"

// 에러코드 : wLow(윈도우 에러코드), wHigh(라인번호)
#define ERROR_CODE(e) MAKELONG(e, __LINE__)

// SetLastError : 윈도우 에러코드 + 29번째 비트 on (=application-defined error code)
#define SETLASTERROR(error_code) SetLastError(LOWORD(error_code) | 0x20000000)

#if defined(_DEBUG) || defined(FORCE_DEBUG_LOG)
// 디버그 모드 컴파일

#include <stdio.h>
#include <stdarg.h>

class CDebugLog {
	FILE *m_fp;

	public:
		CDebugLog();
		~CDebugLog() { if (m_fp) fclose(m_fp); }

		void Log(LPCTSTR cszFormat, ...);
};

extern CDebugLog g_cDebugLog;

#define NOTIFY_ERROR(error_code)	NotifyError(_T(__FILE__), HIWORD(error_code), LOWORD(error_code))	// 에러 알림
#define NOTIFY_DEBUG_MESSAGE	g_cDebugLog.Log	// 디버그 메세지 알림 (printf 처럼 사용)

void NotifyError(LPCTSTR cszFileName, DWORD dwLineNo, DWORD dwErrCode);



#else
// 릴리즈 모드 컴파일

#define NOTIFY_ERROR(error_code)	// 아무 일도 하지 않는다.
#define NOTIFY_DEBUG_MESSAGE(...)

#endif // _DEBUG

#endif // __DEBUG_H__