//
// Util - 플러그인 제작시 유용한 유틸리티 함수 모듈
//

#ifndef __UTIL_H__
#define __UTIL_H__

#include "tstring.h"
#include <vector>

using namespace std;

// 옵션 구조체
typedef struct _tagATOPTION_PAIR {
	tstring strKey;	// 옵션 키
	tstring strValue;	// 옵션 값
} ATOPTION_PAIR, *PATOPTION_PAIR;

// 옵션 모음
typedef vector<ATOPTION_PAIR> ATOPTION_ARRAY;	

// MyCreateDirectory - 지정한 절대경로를 서브폴더까지 만들어준다.
BOOL MyCreateDirectory(LPCTSTR lpDirectory);

// GetATDirectory - 아랄트랜스 설치 디렉토리를 알아온다.
const TCHAR *GetATDirectory();

// GetGameDirectory - 게임 디렉토리를 알아온다.
const TCHAR *GetGameDirectory();

// GetATOptionsFromOptionString - 옵션 문자열을 파싱해서 ATOPTION_ARRAY 에 저장한다.
void GetATOptionsFromOptionString(const char *pszOption, ATOPTION_ARRAY &aOptions);

// GetOptionStringFromATOptions - ATOPTION_ARRAY 를 가지고 옵션 문자열로 바꿔준다.
void GetOptionStringFromATOptions(const ATOPTION_ARRAY &aOptions, char *pszOption, int nMaxOptionLength);

#endif //__UTIL_H__