아랄 트랜스 플러그인 개발 프레임워크 20080810 for AralTrans 0.2.20080704

* 파일 설명
	ATPlugin.h - 아랄트랜스 플러그인 규격에 맞는 익스포트 정보.
	ATPlugin.cpp - 아랄트랜스 플러그인이 제공해야 하는 실제 함수 모음
	DefATContainer.h - 아랄트랜스 컨테이너가 제공하는 함수의 원형 (ATCode 플러그인에서 슬쩍 ^^)
	ATCApi.h / ATCApi.cpp - 아랄트랜스 컨테이너 제공 함수를 사용하기 쉽게 Wrapping한 클래스
	tstring.h / tstring.cpp - 유니코드용 변환 함수 및 string 지원 모듈
	Util.h / Util.cpp - 플러그인 제작시 유용한 유틸리티 함수 모음
	resource.h / ATPlugin.rc - 플러그인 버전 표시 리소스
	ATPlugin.dsw / ATPlugin.dsp - Visual C++ 6.0 용 프로젝트 파일

* 후킹 플러그인을 만들 때 
	ATPlugin.h 의 #define ATPLUGIN_HOOKER 를 주석 해제하시고 개발하세요.

* 번역 플러그인을 만들 때
	ATPlugin.h 의 #define ATPLUGIN_TRANSLATOR 를 주석 해제하시고 개발하세요.

* 필터 플러그인을 만들 때
	ATPlugin.h 의 #define ATPLUGIN_FILTER 를 주석 해제하시고 개발하세요.

* ATPLUGIN_VERSION 과 OnPluginVersion()
	0.2.20080518 공식 플러그인 규격은 아니지만 이후 플러그인 사양이 변했을 때 아랄트랜스
	제작자께서 참조 가능하도록 추가했습니다;

	(20080726 추가 - AralUpdater 가 플러그인의 버전 정보 리소스를 기본으로 업데이트한다는 것을
	 알아냈으므로 OnPluginVersion()은 필요없을 것 같습니다만.. 일단 남겨둡니다.
	 추가로.. 기본적으로 플러그인의 버전 정보 리소스를 추가하도록 ATPlugin.rc 를 동봉하였으니
	 이 파일의 버전 리소스를 고쳐 사용하도록 하시기 바랍니다)

* ATCApi 사용하기 위한 준비
	1. ATCApi.h 를 include 하면 전역 객체로 g_cATCApi가 잡힙니다.
	2. g_cATCApi의 IsValid() 가 TRUE 이면 사용 가능.

* 유니코드 기반 프로젝트를 만들 때
	Visual C++ 6.0 설정의 컴파일러 옵션 define 중 _MBCS 를 지우고 UNICODE, _UNICODE 를 대신 넣으면 됩니다.
	동봉된 프로젝트 파일에는 유니코드용/MBCS용의 설정이 둘 다 있습니다.
	다른 버전의 컴파일러 사용시에는 각 버전의 유니코드 설정 방법을 참고하세요.

* tstring 모듈의 함수 간단 설명
	tstring - 유니코드 설정에 따라 string 혹은 wstring 으로 동작합니다.
	tstring MakeTString(const char *pszString) - char 형식의 문자열을 tstring 으로 바꿔줍니다.
	TCHAR MakeTChar(const char ch) - char 형식의 문자를 TCHAR 로 바꿔줍니다.. 실제로는 필요없을지도;
	MyWideCharToMultiByte() - WideCharToMultiByte 대용 함수입니다. 어플로케일때문에 이렇게 사용해야 한다고..
	MyMultiByteToWideChar() - MyMultiByteToWideChar 대용 함수입니다. 어플로케일때문에 이렇게 사용해야 한다고..
	Kor2Wide(LPCSTR lpKor, LPWSTR lpWide, int nWideSize) - 한글일 경우 MyMultiByteToWideChar를 쉽게 쓰기 위한 간략함수입니다.
	Wide2Kor(LPCWSTR lpWide, LPSTR lpKor, int nKorSize) - 한글일 경우 MyWideCharToMultiByte를 쉽게 쓰기 위한 간략함수입니다.


* Util 함수 간단 설명
	const char *GetATDirectory()
	- 아랄트랜스 설치 디렉토리를 알아온다.
	void GetATOptionsFromOptionString(const char *pszOption, ATOPTION_ARRAY &aOptions)
	- 간단한 옵션 파서. 옵션은 key1(value1),key2(value2) 형식 혹은 key1,key2 도 가능.
	void GetOptionStringFromATOptions(const ATOPTION_ARRAY &aOptions, char *pszOption, int nMaxOptionLength)
	- 위의 옵션 파서와 반대로 옵션 모음을 옵션 문자열로 바꿔주는 함수.