아랄 트랜스 플러그인 개발 프레임워크 20100424

	이 플러그인 개발 프레임워크는 아랄트랜스 버전 0.2.20081115 혹은 
	아랄트랜스 버전 0.3.20100123 버전에 맞게 제작되었습니다.

* 개요
	아랄트랜스 플러그인 개발 프레임워크는 아랄트랜스용 플러그인을 더 쉽게 개발하기 위해
	아랄트랜스의 플러그인 사양 및 편리한 몇몇 함수를 구현해 놓은 프레임워크입니다.

	이 소스는 Public Domain 이므로 누구나 자유롭게 사용할 수 있습니다.

* 파일 및 폴더 설명
/ - 프로젝트 폴더
	- StdAfx.h / StdAfx.cpp - 플러그인의 기본 세팅이 정의되어 있는 헤더
	- ATPlugin.h - 플러그인의 메인 클래스가 정의되어 있는 헤더
	- ATPlugin.cpp - 플러그인의 메인 클래스의 내용의 구현
	- resource.h / ATPlugin.rc - 플러그인 버전 및 기타 리소스 정의
	- ATPlugin.dsw / ATPlugin.dsp - Visual C++ 6.0 용 프로젝트 파일
	- ATPlugin.sln / ATPlugin.vcproj - Visual C++ 2005 용 프로젝트 파일
	/ ATPluginFrame - 플러그인 프레임워크 폴더
		- DefATContainer.h - 아랄트랜스 0.2의 공용 헤더 파일
		- DefStruct.h - 아랄트랜스 0.3의 공용 헤더 파일
		- ATPluginInterface.h / ATPluginInterface.cpp - 아랄트랜스 플러그인 인터페이스가 정의된 파일
		- ATPluginFrame.h / ATPluginFrame.cpp - 아랄트랜스 플러그인 프레임워크 파일
		- Debug.h / Debug.cpp - 디버그 메시지 처리 모듈
		/ Libs - 플러그인 개발에 유용한 라이브러리 모음
			- ATCApi.h / ATCApi.cpp - 아랄트랜스 컨테이너 제공 함수를 사용하기 쉽게 Wrapping한 클래스
			- tstring.h / tstring.cpp - 유니코드용 변환 함수 및 string 지원 모듈
			- ATOptionParser.h / ATOptionParser.cpp - 간단한 옵션 인식 파서
			- Util.h / Util.cpp - 플러그인 제작시 유용한 유틸리티 함수 모음

* 플러그인 제작의 기본 흐름
	1. 컴파일러의 프로젝트 속성에서 원하는 구성과 결과 파일명을 지정합니다.
	2. 프로젝트 폴더의 StdAfx.h 에서 제작하려는 플러그인 스위치를 선택합니다.
	3. ATPlugin.h / ATPlugin.cpp 에서 필요한 메소드를 재정의합니다.
	4. 플러그인 개발을 시작합니다.
	5. 필요에 따라 제공되는 라이브러리를 프로젝트에 추가, 사용합니다.

* 플러그인이 실행될 아랄트랜스 버전 선택
	- StdAfx.h 의 #define USE_ARALTRANS_0_2 를 주석 해제하면 아랄트랜스 0.2 용 플러그인이 제작됩니다.
	- StdAfx.h 의 #define USE_ARALTRANS_0_3 을 주석 해제하면 아랄트랜스 0.3 용 플러그인이 제작됩니다.

* 알고리즘 (후킹) 플러그인을 만들 때 
	1. StdAfx.h 의 #define ATPLUGIN_ALGORITHM 을 주석 해제합니다.
	2. CATPluginApp 의 OnPluginStart() 와 OnPluginClose() 함수를 재정의하여 개발합니다.
	3. 필요에 따라 ATCApi 라이브러리를 사용하거나 직접 공용 헤더 파일을 추가합니다.

* 번역 플러그인을 만들 때
	1. StdAfx.h 의 #define ATPLUGIN_TRANSLATOR 를 주석 해제합니다.
	2. CATPluginApp 의 Translate() 함수를 재정의하여 개발합니다.

* 필터 플러그인을 만들 때
	1. StdAfx.h 의 #define ATPLUGIN_FILTER 를 주석 해제합니다..
	2. CATPluginApp 의 PreTranslate() 와 PostTranslate() 함수를 재정의하여 개발합니다.

* MFC 를 사용하고자 할 때
	1. 프로젝트를 MFC 사용 - 정적 라이브러리에서 MFC 사용으로 맞춥니다.
	2. 프로젝트 옵션 혹은 StdAfx.h 에서 USE_MFC 를 define 후 개발합니다.

* 포함 라이브러리의 설명
	프레임워크에 포함된 라이브러리는 플러그인 제작에 반드시 필요한 것은 아니나 플러그인 제작시 비교적
	많이 사용하는 함수들을 개발자의 편의를 위해 미리 만들어 놓은 것입니다.

	모든 라이브러리의 소스 및 헤더 파일은 ATPluginFrame/Libs 에 들어있으며 각각의 역할은 다음과 같습니다.
	- ATCApi : 후킹 플러그인 개발 시 ATCTNR.dll 과 통신을 위한 모듈
	- tstring : 문자열 유니코드-MBCS 변환 함수 및 지원 모듈
	- ATOptionParser : 옵션 문자열을 옵션명-값 으로 변환 혹은 그 반대의 역할을 하는 모듈
	- Util : 기타 자잘한 도구 함수 모듈

* 추가 정보
	아랄트랜스 플러그인 개발 프레임워크에 대한 더 자세한 사항은 동봉된 아랄트랜스 플러그인 프레임워크
	프로그래머 가이드 (Guide.mht) 를 참조하시기 바랍니다.
