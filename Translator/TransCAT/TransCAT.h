// TransCAT.h : main header file for the TransCAT DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols
#include <string>

#define EZTR_INIT_STR "CSUSER123455"

using namespace std;

// Export Functions
extern "C" __declspec(dllexport) BOOL __stdcall OnPluginInit(HWND hSettingWnd, LPSTR cszSettingStringBuffer);
extern "C" __declspec(dllexport) BOOL __stdcall OnPluginOption();
extern "C" __declspec(dllexport) BOOL __stdcall OnPluginClose();
extern "C" __declspec(dllexport) BOOL __stdcall Translate(LPCSTR cszJapanese, LPSTR szKorean, int nBufSize);


// CTextElement 클래스
class CTextElement
{
public:
	
	CTextElement*	pPrevLink;
	CTextElement*	pNextLink;
	UINT			dwHash;
	string			strTranslatedText;

	CTextElement() : pPrevLink(NULL), pNextLink(NULL), dwHash(0x00000000) {}
};


// CTransCATApp
// See TransCAT.cpp for the implementation of this class
//

class CTransCATApp : public CWinApp
{
	struct
	{
		HMODULE hJKMod;
		int (__stdcall * JK_TransStart)(const char *jpStr, int jpLen);
		bool (__stdcall * JK_TransEndingEx)(int data1, const char *krStr, int jpLen);
		int (__stdcall * GSetJK_gdDir)(int data1, const char *szDir);
		int (__stdcall * GSetJK_gdTempGenbun)(int data1, const char *szStr);
		int (__stdcall * GSetJK_gdTempTerget)(int data1, const char *szStr);
		int (__stdcall * GSetJK_gdTempMorph)(int data1, const char *szStr);
		int (__stdcall * GSetJK_gdTempChnGen)(int data1, const char *szStr);

		int (__stdcall * GSetJK_gQUESTIONMARK)(int data1, int data2);
		int (__stdcall * GSetJK_gEXCLAMATION)(int data1, int data2);
		int (__stdcall * GSetJK_gSEMICOLONMARK)(int data1, int data2);
		int (__stdcall * GSetJK_gCOLONMARK)(int data1, int data2);
		int (__stdcall * GSetJK_gdChudanFlag)(int data1, int data2);
		int (__stdcall * GSetJK_gdUseShiftJisCode)(int data1, int data2);
		int (__stdcall * GSetJK_gdDdeSpaceCut)(int data1, int data2);
		int (__stdcall * GSetJK_gnDdeTransFlag)(int data1, int data2);
		int (__stdcall * GSetJK_gdFileTransFlag)(int data1, int data2);
		int (__stdcall * GSetJK_gdLineFlag)(int data1, int data2);
		int (__stdcall * GSetJK_gdLineLength)(int data1, int data2);
		int (__stdcall * GSetJK_gdPeriodOnly)(int data1, int data2);
		int (__stdcall * GSetJK_gdUseGairaiFlag)(int data1, int data2);
		int (__stdcall * GSetJK_gdUseKanjiFlag)(int data1, int data2);
		int (__stdcall * GSetJK_gbUnDDE)(int data1, int data2);

		int (__stdcall * ControlJK_UserDic)(int data1);
		int (__stdcall * GSetJK_UserDicInit)();
		bool (__stdcall * GSetJK_ITEngineEx)(const char *szTmp, const char *szTmp2);
	} m_TCDLL;

private:
	BOOL	m_bRemoveTrace;
	BOOL	m_bRemoveDupSpace;
	HANDLE	m_hTransThread;
	HANDLE	m_hRequestEvent;
	HANDLE	m_hResponseEvent;
	CRITICAL_SECTION m_csTrans;
	LPCSTR	m_pJpnText;
	LPSTR	m_pKorText;
	int		m_nBufSize;
	CString m_strHomeDir;
	CString m_strErrorMsg;


	CString GetTransCATHomeDir();	// 트랜스캣 홈 디랙토리 얻기
	void	InitTransCAT();
	void	CloseTransCAT();
	static unsigned int __stdcall TransThreadFunc(void* pParam);
	void	EncodeTwoByte(LPCSTR cszJpnSrc, LPSTR szJpnTar);
	void	EncodeTrace(LPCSTR cszJpnSrc, LPSTR szJpnTar);
	void	FilterTrace(LPCSTR cszKorSrc, LPSTR szKorTar);
	void	FilterDupSpaces(LPCSTR cszKorSrc, LPSTR szKorTar);
	void	DecodeTrace(LPCSTR cszKorSrc, LPSTR szKorTar);
	void	DecodeTwoByte(LPCSTR cszKorSrc, LPSTR szKorTar);
	size_t	GetEncodedLen(LPCSTR cszBytes);

public:
	CTransCATApp();

	BOOL OnPluginInit(HWND hSettingWnd, LPSTR cszSettingStringBuffer);
	BOOL OnPluginOption();
	BOOL OnPluginClose();
	BOOL Translate(LPCSTR cszJapanese, LPSTR szKorean, int nBufSize);

	// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
