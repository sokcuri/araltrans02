// DumpText.cpp : 해당 DLL의 초기화 루틴을 정의합니다.
//

#include "stdafx.h"
#include "DumpText.h"
#include "DumpDlg.h"
#include "OpQueue.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;

// 유일한 CDumpTextApp 개체입니다.

CDumpTextApp theApp;

//
//TODO: 이 DLL이 MFC DLL에 대해 동적으로 링크되어 있는 경우
//		MFC로 호출되는 이 DLL에서 내보내지는 모든 함수의
//		시작 부분에 AFX_MANAGE_STATE 매크로가
//		들어 있어야 합니다.
//
//		예:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// 일반적인 함수 본문은 여기에 옵니다.
//		}
//
//		이 매크로는 MFC로 호출하기 전에
//		각 함수에 반드시 들어 있어야 합니다.
//		즉, 매크로는 함수의 첫 번째 문이어야 하며 
//		개체 변수의 생성자가 MFC DLL로
//		호출할 수 있으므로 개체 변수가 선언되기 전에
//		나와야 합니다.
//
//		자세한 내용은
//		MFC Technical Note 33 및 58을 참조하십시오.
//

// CDumpTextApp

extern "C" BOOL PASCAL EXPORT OnPluginInit(HWND hSettingWnd, LPSTR szOptionStringBuffer)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return theApp.OnPluginInit(hSettingWnd,szOptionStringBuffer);
}

extern "C" BOOL PASCAL EXPORT OnPluginOption()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return theApp.OnPluginOption();
}

extern "C" BOOL PASCAL EXPORT OnPluginClose()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return theApp.OnPluginClose();
}

extern "C" BOOL PASCAL EXPORT PreTranslate(LPCSTR cszInJapanese, LPSTR szOutJapanese, int nBufSize)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return theApp.PreTranslate(cszInJapanese,szOutJapanese,nBufSize);
}

extern "C" BOOL PASCAL EXPORT PostTranslate(LPCSTR cszInKorean, LPSTR szOutKorean, int nBufSize)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return theApp.PostTranslate(cszInKorean,szOutKorean,nBufSize);
}

BEGIN_MESSAGE_MAP(CDumpTextApp, CWinApp)
END_MESSAGE_MAP()


// CDumpTextApp 생성

CDumpTextApp::CDumpTextApp()
{
	// TODO: 여기에 생성 코드를 추가합니다.
	// InitInstance에 모든 중요한 초기화 작업을 배치합니다.
	m_pTmpTimer=NULL;
	m_pThread=NULL;
}


// CDumpTextApp 초기화

BOOL CDumpTextApp::InitInstance()
{
	// 응용 프로그램 매니페스트가 ComCtl32.dll 버전 6 이상을 사용하여 비주얼 스타일을
	// 사용하도록 지정하는 경우, Windows XP 상에서 반드시 InitCommonControlsEx()가 필요합니다.
	// InitCommonControlsEx()를 사용하지 않으면 창을 만들 수 없습니다.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 응용 프로그램에서 사용할 모든 공용 컨트롤 클래스를 포함하도록
	// 이 항목을 설정하십시오.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	return TRUE;
}


UINT threadProc(LPVOID data){
	CDumpDlg dlg;
	dlg.DoModal();
	return 0L;
}

VOID CALLBACK TimerProc(HWND hwnd,UINT uMsg,UINT_PTR idEvent,DWORD dwTime){
	KillTimer(NULL,idEvent);
	theApp.m_pThread = AfxBeginThread(threadProc,NULL,0,0U,CREATE_SUSPENDED);
	if(theApp.m_pThread!=NULL){
		theApp.m_pThread->m_bAutoDelete=FALSE;
		theApp.m_pThread->ResumeThread();
	}
}


BOOL CDumpTextApp::OnPluginInit(HWND hSettingWnd, LPSTR szOptionStringBuffer){
	m_hSettingWnd = hSettingWnd;
	m_szOptionStringBuffer = szOptionStringBuffer;

	//Thread Run!
	//도저히 이게 안정적으로 객체가 생성된 이후임을 보장할 수가 없다!
	//그러니 1초간 지연 후 실행되도록 하자!
	m_pTmpTimer = SetTimer(NULL,3815,1000,TimerProc);

	m_szOptionStringBuffer[0]='\0';
	m_szOptionStringBuffer[1]='\0';

	return TRUE;
}

BOOL CDumpTextApp::OnPluginOption(){
	if(m_hSettingWnd!=NULL && IsWindow(m_hSettingWnd)){
		//AfxMessageBox(L"관련 설정은 DumpText 윈도우에서 할 수 있습니다.",MB_OK|MB_ICONINFORMATION);
		wstring strRegPath;
		CRegKey cReg;

		std::wstring m_strRegDefault;
		std::wstring m_strRegMy;

		m_strRegDefault = L"SOFTWARE\\AralGood\\DumpText Option";
		{
			wchar_t szFileName[2048]=L"";
			int nLen = ::GetModuleFileNameW(NULL,szFileName,2048);
			while(nLen>=0&&szFileName[nLen]!=L'\\'){
				nLen--;
			}
			m_strRegMy = m_strRegDefault;
			m_strRegMy += (szFileName+nLen);
		}

		strRegPath = m_strRegMy;

		if(cReg.Create(HKEY_CURRENT_USER,strRegPath.c_str()) != ERROR_SUCCESS){
			AfxMessageBox(L"심각한 오류!\r\n레지스트리 초기화에 실패했습니다.\r\n개발자에게 보고가 필요합니다",MB_ICONSTOP);
			return TRUE;
		}
		//DOPT_SIZE
		DWORD data=0;
		wchar_t szData[2048]=L"";
		data = swprintf_s(szData,2048,L"%d|%d|%d|%d",200,200,400,500);
		cReg.SetStringValue(L"DOPT_SIZE",szData);

		cReg.Close();
		OnPluginClose();
		AfxMessageBox(L"DumpText 필터의 위치 및 크기를 초기화 했습니다.\n필터를 삭제 후 다시 추가해주세요.",MB_OK|MB_ICONINFORMATION);
	}

	return TRUE;
}

BOOL CDumpTextApp::OnPluginClose(){
	KillTimer(NULL,m_pTmpTimer);

	OpData offOp;
	offOp.type = 0x0101;

	pushOpQueue(offOp,false);
	for(int i=0;i<4;i++){
		pushOpQueue(offOp); //종료 시그널을... 마구 쏘자 -_-+++
	}

	if(m_pThread!=NULL){
		::WaitForSingleObject(m_pThread->m_hThread,INFINITE);
	}

	return TRUE;

}

BOOL CDumpTextApp::PreTranslate(LPCSTR cszInJapanese, LPSTR szOutJapanese, int nBufSize){
	size_t uLen = strlen(cszInJapanese);
	strcpy_s(szOutJapanese,nBufSize,cszInJapanese);

	uLen+=2;

	vector<wchar_t> buff(uLen);

	int nLen = MyMultiByteToWideChar(932,0,cszInJapanese,-1,buff.data(),uLen);
	nLen--;

	OpData textData;
	textData.type = 0x0200;
	textData.data.insert(textData.data.end(),buff.begin(),buff.begin()+nLen);

	pushOpQueue(textData);

	return TRUE;
}

BOOL CDumpTextApp::PostTranslate(LPCSTR cszInKorean, LPSTR szOutKorean, int nBufSize){
	size_t uLen = strlen(cszInKorean);
	strcpy_s(szOutKorean,nBufSize,cszInKorean);

	uLen+=2;

	vector<wchar_t> buff(uLen);

	int nLen = MyMultiByteToWideChar(949,0,cszInKorean,-1,buff.data(),uLen);
	nLen--;

	OpData textData;
	textData.type = 0x0210;
	textData.data.insert(textData.data.end(),buff.begin(),buff.begin()+nLen);

	pushOpQueue(textData);

	return TRUE;
}