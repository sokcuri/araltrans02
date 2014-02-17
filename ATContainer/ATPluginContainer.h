#ifndef __ATPLUGINCONTAINER_H__
#define __ATPLUGINCONTAINER_H__

/*
// 버전 규약 삭제 - 쓰이지 않음.

// ATPLUGIN_VERSION
// 이후 플러그인 형식 변경시 호환성 문제 해결을 위한 버전 정보.
// 0.2.20080518 공식 플러그인 규격은 아니지만 나중을 위해 넣어둠.
typedef struct _tagATPLUGIN_VERSION {
	struct {
		WORD Major;	// 메이저 버전. 현재 0
		WORD Minor;	// 마이너 버전. 현재 2
		DWORD BuildDate;	// 빌드 일자. 현재 20080518
	} ATVersion;	// 플러그인 제작 당시의 AralTrans 버전 정보

	struct {
		WORD Major;	// 메이저 버전
		WORD Minor;	// 마이너 버전
		DWORD BuildDate;	// 빌드 일자
	} PluginVersion;	// 플러그인 자체 버전 정보

	char szAuthor [16];	// 제작자
	char szPluginName [16];	// 플러그인 이름
	char szDescription [32];	// 플러그인 설명

} ATPLUGIN_VERSION;
*/
#include <tchar.h>

// 파일 포인터 타입 정의
typedef BOOL (__stdcall * PROC_OnPluginDefault) ();
typedef BOOL (__stdcall * PROC_OnPluginInit) (HWND, LPSTR);
//typedef const ATPLUGIN_VERSION * (__stdcall * PROC_OnPluginVersion)();
typedef BOOL (__stdcall * PROC_Translate) (LPCSTR, LPSTR, int);

class CATPluginContainer {
	protected:
		HMODULE m_hModule;
		char *m_pszOption;

		PROC_OnPluginInit m_pfnOnPluginInit;
		PROC_OnPluginDefault m_pfnOnPluginOption;
		PROC_OnPluginDefault m_pfnOnPluginClose;

// 안쓰임
//		PROC_OnPluginVersion m_pfnOnPluginVersion;

/*
// 알고리즘 플러그인용 - 필터전용으로 사용할 것이므로 안쓰임
		PROC_OnPluginDefault m_pfnOnPluginStart;
		PROC_OnPluginDefault m_pfnOnPluginStop;
*/

/*
// 번역 플러그인용 - 필터전용으로 사용할 것이므로 안쓰임
		PROC_Translate m_pfnTranslate;
*/
		PROC_Translate m_pfnPreTranslate;
		PROC_Translate m_pfnPostTranslate;

	public:
		CATPluginContainer() : m_hModule(NULL), m_pszOption(NULL),
			m_pfnOnPluginInit(NULL), m_pfnOnPluginOption(NULL), m_pfnOnPluginClose(NULL), // m_pfnOnPluginVersion(NULL),
//			m_pfnOnPluginStart(NULL), m_pfnOnPluginStop(NULL), m_pfnTranslate(NULL),
			m_pfnPreTranslate(NULL), m_pfnPostTranslate(NULL) {}
		~CATPluginContainer() { Unload(); }

		const char *GetOptionString() const { return (m_pszOption)?m_pszOption:""; }

		virtual BOOL Load(LPCTSTR pszModulePath);
		virtual void Unload();

		virtual BOOL OnPluginInit(HWND hSettingWnd, LPCSTR cszOptionStringBuffer);
		virtual BOOL OnPluginOption()
			{ return (m_pfnOnPluginOption)?m_pfnOnPluginOption():FALSE; }
		virtual BOOL OnPluginClose()
			{ return (m_pfnOnPluginClose)?m_pfnOnPluginClose():FALSE; }
/*
		virtual const ATPLUGIN_VERSION *OnPluginVersion()
			{ return (m_pfnOnPluginVersion)?m_pfnOnPluginVersion():NULL; }
		
		virtual BOOL OnPluginStart()
			{ return (m_pfnOnPluginStart)?m_pfnOnPluginStart():FALSE; }
		virtual BOOL OnPluginStop()
			{ return (m_pfnOnPluginStop)?m_pfnOnPluginStop():FALSE; }
		
		virtual BOOL Translate(LPCSTR cszJapanese, LPSTR szKorean, int nBufSize)
			{ return (m_pfnTranslate)?m_pfnTranslate(cszJapanese, szKorean, nBufSize):FALSE; }
*/
		virtual BOOL PreTranslate(LPCSTR cszInJapanese, LPSTR szOutJapanese, int nBufSize)
			{ return (m_pfnPreTranslate)?m_pfnPreTranslate(cszInJapanese, szOutJapanese, nBufSize):FALSE; }
		virtual BOOL PostTranslate(LPCSTR cszInKorean, LPSTR szOutKorean, int nBufSize)
			{ return (m_pfnPostTranslate)?m_pfnPostTranslate(cszInKorean, szOutKorean, nBufSize):FALSE; }


};

#endif // __ATPLUGINCONTAINER_H__