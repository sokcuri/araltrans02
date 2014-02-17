// ATPlugin.cpp : 해당 DLL의 초기화 루틴을 정의합니다.
//

#include "stdafx.h"
#include "ATPlugin.h"
#include "tstring.h"

#include "DivideLine.h"
#include "OptionDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Util 사용하려면 include
#include "Util.h"


/*
** 전역변수 선언부
*/

// 플러그인 버전
ATPLUGIN_VERSION g_pluginVer = {
	{ 0, 2, 20080704 },		// 플러그인 제작시 AT 버전
	{ 0, 1, 20080810 },		// 플러그인 버전
	"제작자",	// 제작자
	"플러그인 이름",	// 플러그인 이름
	"아랄트랜스 플러그인"	// 플러그인 설명
};

// 설정창 윈도우 핸들
HWND g_hSettingWnd=NULL;

// 이 플러그인의 모듈 핸들 = 인스턴스 핸들
HINSTANCE g_hThisModule;

// 옵션 버퍼 - 옵션이 바뀌었을 때 여기에 써주면 아랄트랜스 바로가기에도 등록할 수 있다.
// 주의 - 옵션길이는 최고 4096 바이트.
LPSTR g_szOptionStringBuffer=NULL;

// CATPluginApp

BEGIN_MESSAGE_MAP(CATPluginApp, CWinApp)
END_MESSAGE_MAP()

// CATPluginApp 생성

CATPluginApp::CATPluginApp()
{
	// TODO: 여기에 생성 코드를 추가합니다.
	// InitInstance에 모든 중요한 초기화 작업을 배치합니다.
}


// 유일한 CATPluginApp 개체입니다.

CATPluginApp theApp;
COptionDlg g_cOptionDlg;

// Export Functions
extern "C" __declspec(dllexport) BOOL __stdcall OnPluginInit(HWND hSettingWnd, LPSTR cszOptionStringBuffer)
{
	return theApp.OnPluginInit(hSettingWnd, cszOptionStringBuffer);
}

extern "C" __declspec(dllexport) BOOL __stdcall OnPluginOption()
{
	return theApp.OnPluginOption();
}

extern "C" __declspec(dllexport) BOOL __stdcall OnPluginClose()
{
	return theApp.OnPluginClose();
}

extern "C" __declspec(dllexport) BOOL __stdcall PreTranslate(LPCSTR cszInJapanese, LPSTR szOutJapanese, int nBufSize)
{
	return theApp.PreTranslate(cszInJapanese, szOutJapanese, nBufSize);
}

extern "C" __declspec(dllexport) BOOL __stdcall PostTranslate(LPCSTR cszInKorean, LPSTR szOutKorean, int nBufSize)
{
	return theApp.PostTranslate(cszInKorean, szOutKorean, nBufSize);
}

extern "C" __declspec(dllexport) const ATPLUGIN_VERSION * __stdcall OnPluginVersion()
{
	// 플러그인 버전 반환

	return _OnPluginVersion();
}

const ATPLUGIN_VERSION * _OnPluginVersion()
{
	// 플러그인 버전 반환

	return &g_pluginVer;
}




// CATPluginApp 초기화

BOOL CATPluginApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}

BOOL CATPluginApp::OnPluginInit(HWND hSettingWnd, LPSTR cszOptionStringBuffer)
{
	g_hSettingWnd = hSettingWnd;
	g_szOptionStringBuffer=cszOptionStringBuffer;

	ATOPTION_ARRAY aOptions;
	GetATOptionsFromOptionString(cszOptionStringBuffer, aOptions);

	// 옵션 방식
	// Base(IgnoreLine,MaxChar,MaxLine,ReturnLen,ReturnChar,LimitLine),Add(OptLen,OnlyText,OptString)
	for (size_t i=0; i<aOptions.size(); i++)
	{

		if ( !lstrcmpi(aOptions[i].strKey.c_str(), L"Base"))
		{
			wstring::size_type SPoint=0;
			wstring::size_type Divider=0;
			wstring Temp1=aOptions[i].strValue;
			wstring Temp2=L"";
			wstring Temp3=L"";

			int nReturnLen;
			int nMaxChar;
			int nMaxLine;
			int nIgnoreLine;

			string strReturnChar;

			for(int Flag=0;Flag<5;Flag++)
			{
				if(Flag<4)
				{
					Divider=Temp1.find(L',',SPoint);
				}
				else
				{
					Divider=Temp1.length()-1;
				}

				Temp2=Temp1.substr(SPoint,Divider-SPoint+1);
				SPoint=Divider+1;

				switch(Flag)
				{
				case 0://IgnoreLine
					nIgnoreLine=_wtoi(Temp2.c_str());
					break;

				case 1://MaxChar
					nMaxChar=_wtoi(Temp2.c_str());
					break;

				case 2://MaxLine
					nMaxLine=_wtoi(Temp2.c_str());
					break;

				case 3://ReturnLen
					nReturnLen=_wtoi(Temp2.c_str());
					break;

				case 4://ReturnChar
					strReturnChar=HexToString(Temp2);
					break;

				default:;
				}
			}
			g_cDivideLine.SetBase(nIgnoreLine,nMaxChar,nMaxLine,nReturnLen,strReturnChar);
		}
		else if ( !lstrcmpi(aOptions[i].strKey.c_str(), L"RemoveSpace"))
		{
			g_cDivideLine.SetRemoveSpace(true);
		}
		else if ( !lstrcmpi(aOptions[i].strKey.c_str(), L"TwoByte"))
		{
			g_cDivideLine.SetTwoByte(true);
		}
	}

	ApplySetting();

	// 초기화 성공시 TRUE, 실패시 FALSE를 반환
	return TRUE;
}

BOOL CATPluginApp::OnPluginClose()
{
	// 주의: 여기서 메모리 확보 등을 하면 에러 발생.
	// TODO: 플러그인 종료 준비, 자원 반환 등을 한다.
	return TRUE;
}

BOOL CATPluginApp::OnPluginOption()
{
	if (g_hSettingWnd && IsWindow(g_hSettingWnd))
	{
		// TODO: 플러그인 옵션창 처리 및 옵션 조정을 한다.

		if(g_cOptionDlg.DoModal()==IDOK)
		{
			ApplySetting();
		}
	}
	return TRUE;
}

BOOL CATPluginApp::PreTranslate(LPCSTR cszInJapanese, LPSTR szOutJapanese, int nBufSize)
{
	// TODO: 번역 전처리를 한다.
	// 특별한 처리를 하지 않더라도 문자열을 복사해 돌려주어야 한다.
	g_cDivideLine.PreDivideLine(szOutJapanese, cszInJapanese);
	
	// 작업 성공시 TRUE, 실패시 FALSE 반환 
	return TRUE;
}

BOOL CATPluginApp::PostTranslate(LPCSTR cszInKorean, LPSTR szOutKorean, int nBufSize)
{
	// TODO: 번역 후처리를 한다.
	// 특별한 처리를 하지 않더라도 문자열을 복사해 돌려주어야 한다.
	g_cDivideLine.PostDivideLine(szOutKorean, cszInKorean);
	
	// 작업 성공시 TRUE, 실패시 FALSE 반환 
	return TRUE;
}

void CATPluginApp::ApplySetting()
{
	ATOPTION_ARRAY aOptions;
	ATOPTION_PAIR option;
	
	//Base
	if(1)
	{
		TCHAR szbuf[10];
		option.strKey=_T("BASE");
		// Base(IgnoreLine,MaxChar,MaxLine,ReturnLen,ReturnChar)
		_itot_s(g_cDivideLine.GetIgnoreLine(),szbuf,10,10);
		option.strValue =szbuf;
		option.strValue+=_T(',');
		_itot_s(g_cDivideLine.GetMaxChar(),szbuf,10,10);
		option.strValue+=szbuf;
		option.strValue+=_T(',');
		_itot_s(g_cDivideLine.GetMaxLine(),szbuf,10,10);
		option.strValue+=szbuf;
		option.strValue+=_T(',');
		_itot_s(g_cDivideLine.GetReturnLen(),szbuf,10,10);
		option.strValue+=szbuf;
		option.strValue+=_T(',');
		option.strValue+=StringToHex(g_cDivideLine.GetReturnChar());
		aOptions.push_back(option);
	}


	if (g_cDivideLine.GetRemoveSpace()!=FALSE)
	{
		option.strKey=_T("RemoveSpace");
		option.strValue.clear();
		aOptions.push_back(option);
	}
	if (g_cDivideLine.GetTwoByte()!=FALSE)
	{
		option.strKey=_T("TwoByte");
		option.strValue.clear();
		aOptions.push_back(option);
	}

	GetOptionStringFromATOptions(aOptions, g_szOptionStringBuffer, 4096);
}