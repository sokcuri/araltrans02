//
// Util - 플러그인 제작시 유용한 유틸리티 함수 모듈
//

#ifndef __UTIL_H__
#define __UTIL_H__

#include "tstring.h"

// MyCreateDirectory - 지정한 절대경로를 서브폴더까지 만들어준다.
BOOL MyCreateDirectory(LPCTSTR lpDirectory);

// GetATDirectory - 아랄트랜스 설치 디렉토리를 알아온다.
const TCHAR *GetATDirectory();

// GetGameDirectory - 게임 디렉토리를 알아온다.
const TCHAR *GetGameDirectory();

#endif //__UTIL_H__