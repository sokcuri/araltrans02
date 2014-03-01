// ATPluginFrame - 아랄트랜스 플러그인 프레임워크
//

#ifndef __ATPLUGINFRAME_H__
#define __ATPLUGINFRAME_H__

#include "ATPluginInterface.h"

/////////////////////////////////////////////////////////////////////////////
// CATPluginFrameApp
// 아랄트랜스 플러그인과의 연결을 위한 상위 클래스
class CATPluginFrameApp : public CWinApp
{
	public:
		// 현재 실행중인 아랄트랜스 버전
		DWORD m_dwAralVersion;

		// 플러그인 정보
		PLUGIN_INFO m_sPluginInfo;

		// 아랄트랜스 (설정)창 윈도우 핸들
		HWND m_hAralWnd;
		
		// 옵션 버퍼 - 옵션이 바뀌었을 때 여기에 써주면 아랄트랜스 바로가기에도 등록할 수 있다.
		// 주의 - 옵션길이는 아랄트랜스 0.2의 경우 최고 4096 바이트, 0.3의 경우 10240 (MAX_OPTION_LEN)

		struct OPTION_STRING {
			union {
				LPSTR pszAnsi;	// 아랄트랜스 0.2 의 경우 ANSI
				LPWSTR pwszUnicode;	// 아랄트랜스 0.3 의 경우 UNICODE
			};
			BOOL bIsUnicode;
		} m_sOptionString;

	public:
		CATPluginFrameApp();
		BOOL _PrePluginInit(HWND hAralWnd, LPVOID pPluginOption);	// 옵션 스트링과 아랄 윈도우 핸들 세팅
		
	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 플러그인 이벤트 관련
	public:
		// 공통
		virtual BOOL OnPluginInit() { return TRUE; } // 플러그인 초기화 이벤트
		virtual BOOL OnPluginOption() { return TRUE; }	// 플러그인 옵션창 이벤트
		virtual BOOL OnPluginClose() { return TRUE; }	// 플러그인 종료 이벤트
		virtual BOOL GetPluginInfo(PLUGIN_INFO * pPluginInfo);	// 플러그인 정보 (아랄트랜스 0.3 전용)

		// 후킹 플러그인 전용 (아랄트랜스 0.2)
		virtual BOOL OnPluginStart() { return TRUE; }	// 플러그인 시작 이벤트 (아랄트랜스 0.2 전용)
		virtual BOOL OnPluginStop() { return TRUE; }	// 플러그인 정지 이벤트 (아랄트랜스 0.2 전용)
		
		// 번역 플러그인 전용 (아랄트랜스 0.2 혹은 아랄트랜스 0.3 프레임워크에서 기본 사용)
		virtual BOOL Translate(LPCSTR cszJapanese, LPSTR szKorean, int nBufSize)	// 번역 이벤트
		{ lstrcpyA(szKorean, cszJapanese); return TRUE; }
		
		// 필터 플러그인 전용 (아랄트랜스 0.2 혹은 아랄트랜스 0.3 프레임워크에서 기본 사용)
		virtual BOOL PreTranslate(LPCSTR cszInJapanese, LPSTR szOutJapanese, int nBufSize)	// 번역 전처리 이벤트
		{ lstrcpyA(szOutJapanese, cszInJapanese); return TRUE; }
		virtual BOOL PostTranslate(LPCSTR cszInKorean, LPSTR szOutKorean, int nBufSize)	// 번역 후처리 이벤트
		{ lstrcpyA(szOutKorean, cszInKorean); return TRUE; }


		// 번역 개체 관련 이벤트 (아랄트랜스 0.3 번역/필터 플러그인 전용)
	public:
		virtual BOOL OnObjectInit(TRANSLATION_OBJECT* pTransObj);	// 번역 개체 생성 이벤트 (아랄트랜스 0.3 전용)
		virtual BOOL OnObjectClose(TRANSLATION_OBJECT* pTransObj);	// 번역 개체 삭제 이벤트	(아랄트랜스 0.3 전용)
		virtual BOOL OnObjectMove(TRANSLATION_OBJECT* pTransObj) { return TRUE; }	// 번역 개체 순서변경 이벤트? (아랄트랜스 0.3 전용)
		virtual BOOL OnObjectOption(TRANSLATION_OBJECT* pTransObj) { return TRUE; }	// 번역 개체 옵션창 이벤트? (아랄트랜스 0.3 전용)


	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	// MainTranslateProcedure 파생 관련 (번역/필터 플러그인 전용 고급 메소드)
	//
	// 아랄트랜스 0.3의 프레임워크 기본 번역 프로시저 (MainTranslateProcedure)가 다음의 파생 메소드를 불러준다.
	// 아랄트랜스 0.3의 추가적인 기능 (유니코드 번역 혹은 다음 번역개체로 번역 넘기지 않기 등) 을 사용하지 않는다면
	// 0.2 의 PreTranslate/PostTranslate/Translate 를 그냥 사용해도 무방하다.
	// (파생 메소드의 기본 세팅에서 자동으로 불러줌)
	//
	public:
		virtual BOOL MainTranslateProcedure(TRANSLATION_OBJECT* pTransObj);	// 번역 이벤트 (아랄트랜스 0.3 전용)

		// 결과값
		enum TRESULT {
			TR_STOPALL			= 0,	// 00000000 (2), 번역불가 (FALSE) 판정을 내린다.
			TR_POSTONLY			= 1,	// 00000001 (2), 후처리만 한다.
			TR_TRANSONLY		= 2,	// 00000010 (2), (필터 겸 번역기일 때) 자체 번역만 한다.
			TR_TRANS_POST		= 3,	// 00000011 (2), (필터 겸 번역기일 때) 자체 번역 및 후처리만 한다.
			TR_NEXTONLY			= 4,	// 00000100 (2), 다음 번역 개체의 번역 프로시저만 실행한다.
			TR_NEXT_POST		= 5,	// 00000101 (2), (필터 겸 번역기일 때) 다음 번역 개체의 번역 프로시저와 후처리만 한다.
			TR_NEXT_TRANS		= 6,	// 00000110 (2), (필터 겸 번역기일 때) 자체 번역 및 다음 번역 개체의 번역 프로시저만 한다.
			TR_TRANS_NEXT_POST	= 7,	// 00000111 (2), 자체 번역, 다음 번역개체, 후처리를 모두 한다.
			TR_DOALL			= 7,	// = TR_TRANS_NEXT_POST
			TR_STOP_OK			= 8,	// 00001000 (2), 더 이상의 처리를 중지하지만 번역완료 (TRUE) 판정을 내린다.
		};

		virtual TRESULT PreTranslateEx(TRANSLATION_OBJECT *pTransObj);	// 번역 전처리 작업
		virtual TRESULT TranslateEx(TRANSLATION_OBJECT *pTransObj);		// 자체 번역 작업
		virtual TRESULT NextTranslateEx(TRANSLATION_OBJECT *pTransObj);	// 다음 개체의 번역 작업
		virtual TRESULT PostTranslateEx(TRANSLATION_OBJECT *pTransObj);	// 번역 후처리 작업



	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 유틸리티 메소드
	//
		TRANSLATION_OBJECT *GetFirstObject(TRANSLATION_OBJECT *pTransObj);	// 번역 객체 체인의 가장 처음 객체를 찾는다.
		TRANSLATION_OBJECT *GetLastObject(TRANSLATION_OBJECT *pTransObj);	// 번역 객체 체인의 가장 마지막 객체를 찾는다.

};



#endif //__ATPLUGINFRAME_H__
