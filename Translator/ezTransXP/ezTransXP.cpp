// ezTransXP.cpp : Defines the initialization routines for the DLL.
//
#pragma warning(disable:4996)

#include "stdafx.h"
#include "ezTransXP.h"
#include "hash.hpp"
#include "EZTransOptionDlg.h"
#include <stdio.h>
#include <process.h>
#include <map>

#define TRANS_BUF_SIZE 4096

//   왼쪽 2Byte :    KS5601 전각 기호 코드
// 오른쪽 2Byte : SHIFT-JIS 전각 기호 코드
const char _SHIFT_JIS_CHAR_MAP[][4] = {
	{'\xA1', '\xCD', '\x81', '\x8F'},	// '\\'
	{'\xA3', '\xC0', '\x81', '\x97'},	// '@'
	{'\xA3', '\xA8', '\x81', '\x69'},	// '('
	{'\xA3', '\xA9', '\x81', '\x6A'},	// ')'
	{'\xA3', '\xBC', '\x81', '\x83'},	// '<'
	{'\xA3', '\xBE', '\x81', '\x84'},	// '>'
	{'\xA3', '\xDB', '\x81', '\x6D'},	// '['
	{'\xA3', '\xDD', '\x81', '\x6E'},	// ']'
	{'\xA3', '\xFB', '\x81', '\x6F'},	// '{'
	{'\xA3', '\xFD', '\x81', '\x70'},	// '}'
	{'\xA2', '\xC8', '\x81', '\x4A'},	// ″
	{'\xA2', '\xC8', '\x81', '\x4B'},	// ″
	{'\xA2', '\xC8', '\x81', '\x8D'},	// ″
	{'\xA2', '\xC7', '\x81', '\x4C'},	// ´
	{'\xA2', '\xC7', '\x81', '\x4D'},	// ´
	{'\xA2', '\xC7', '\x81', '\x8C'},	// ´
	{'\xA1', '\xAE', '\x81', '\x65'},	// ‘
	{'\xA1', '\xAF', '\x81', '\x66'},	// ’
	{'\xA1', '\xB0', '\x81', '\x67'},	// “
	{'\xA1', '\xB1', '\x81', '\x68'},	// ”

	{'\x00', '\x00', '\x00', '\x00'}
};

HWND	g_hSettingWnd = NULL;
LPSTR	g_pOptStrBuf = NULL;


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// The one and only CezTransXPApp object

CezTransXPApp theApp;


// Export Functions
extern "C" __declspec(dllexport) BOOL __stdcall OnPluginInit(HWND hSettingWnd, LPSTR cszSettingStringBuffer)
{
	return theApp.OnPluginInit(hSettingWnd, cszSettingStringBuffer);
}

extern "C" __declspec(dllexport) BOOL __stdcall OnPluginOption()
{
	return theApp.OnPluginOption();
}

extern "C" __declspec(dllexport) BOOL __stdcall OnPluginClose()
{
	return theApp.OnPluginClose();
}

extern "C" __declspec(dllexport) BOOL __stdcall Translate(LPCSTR cszJapanese, LPSTR szKorean, int nBufSize)
{
	return theApp.Translate(cszJapanese, szKorean, nBufSize);
}




//
//TODO: If this DLL is dynamically linked against the MFC DLLs,
//		any functions exported from this DLL which call into
//		MFC must have the AFX_MANAGE_STATE macro added at the
//		very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//


// CezTransXPApp

BEGIN_MESSAGE_MAP(CezTransXPApp, CWinApp)
END_MESSAGE_MAP()


// CezTransXPApp construction

CezTransXPApp::CezTransXPApp()
	: m_bRemoveTrace(FALSE)
	, m_bRemoveDupSpace(FALSE)
	, m_hTransThread(NULL)
	, m_hRequestEvent(NULL)
	, m_hResponseEvent(NULL)
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}




// CezTransXPApp initialization

BOOL CezTransXPApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}

BOOL CezTransXPApp::OnPluginInit( HWND hSettingWnd, LPSTR cszSettingStringBuffer )
{
	TRACE(_T("CezTransXPApp::OnPluginInit() begin \n"));

	BOOL bRetVal = FALSE;

	// 컨테이너 윈도우 저장
	g_hSettingWnd = hSettingWnd;
	
	// 옵션 스트링 버퍼 연결
	g_pOptStrBuf = cszSettingStringBuffer;

	// 옵션 적용
	if(g_pOptStrBuf[0] == '0') m_bRemoveTrace = FALSE;
	else m_bRemoveTrace = TRUE;	// 괄호제거가 디폴트

	if(g_pOptStrBuf[1] == '1') m_bRemoveDupSpace = FALSE;
	else m_bRemoveDupSpace = TRUE;	// 띄워쓰기 제거가 디폴트

	// 동기화 객체 초기화
	InitializeCriticalSection(&m_csTrans);
	m_hRequestEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	m_hResponseEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	m_hTransThread = (HANDLE)_beginthreadex(NULL, 0, TransThreadFunc, this, 0, NULL);
	if(NULL == m_hTransThread)
	{
		MessageBox(NULL, _T("이지트랜스 쓰레드를 시작할 수 없습니다."), _T("ezTransXP Initialize Error"), MB_OK | MB_TOPMOST);
	}
	else
	{
		WaitForSingleObject(m_hResponseEvent, INFINITE);
		if(!m_strErrorMsg.IsEmpty())
		{
			MessageBox(NULL, m_strErrorMsg, _T("ezTransXP Initialize Error"), MB_OK | MB_TOPMOST);
		}
		else
		{
			bRetVal = TRUE;
		}

	}
	

	TRACE(_T("CezTransXPApp::OnPluginInit() end \n"));

	return bRetVal;

}

BOOL IsASCIIOnly(LPCTSTR cszString)
{
	if(NULL == cszString) return FALSE;

	size_t i = 0;
	while(cszString[i]) 
	{
		if( ((BYTE*)cszString)[i++] >= 0x80 ) return FALSE;
	}

	return TRUE;
}


void CezTransXPApp::InitEZTrans()
{
	m_strErrorMsg = _T("");
	m_strHomeDir = _T("");

	try
	{
		while(m_strHomeDir.IsEmpty())
		{
			m_strHomeDir = GetEZTransHomeDir();
			if( m_strHomeDir.IsEmpty() )
			{
				if( ::MessageBox(
						NULL, 
						_T("EZ Trans XP가 설치된 경로를 찾을 수 없습니다.\r\n경로를 직접 찾으시겠습니까?"), 
						_T("ezTransXP Plugin"), 
						MB_YESNO | MB_TOPMOST) == IDYES )
				{
					ITEMIDLIST *pidlBrowse; 
					TCHAR pszPathname[MAX_PATH] = {0,}; 
					BROWSEINFO BrInfo;
					BrInfo.hwndOwner = NULL; 
					BrInfo.pidlRoot = NULL;

					memset(&BrInfo, 0, sizeof(BrInfo));
					BrInfo.pszDisplayName = pszPathname;
					BrInfo.lpszTitle = _T("ez Trans-XP 위치 지정");
					BrInfo.ulFlags = BIF_RETURNONLYFSDIRS;

					pidlBrowse = ::SHBrowseForFolder(&BrInfo);

					if( pidlBrowse != NULL)
					{
						BOOL bModalRes = ::SHGetPathFromIDList(pidlBrowse, pszPathname);
						if(bModalRes)
						{
							m_strHomeDir.Format(_T("%s\\Dat"), pszPathname);
						}
					}
					
				}
				else
				{
					throw _T("EZ Trans XP가 설치된 경로를 찾을 수 없습니다.");
				}
			}
		}

		// 영문 경로인지 검사
		if(IsASCIIOnly(m_strHomeDir) == FALSE)
		{
			if( ::MessageBox(
				NULL, 
				_T("EZ Trans XP 경로에 한글이 섞여 있을 경우 제대로 동작하지 않을 수 있습니다.\r\n그래도 계속 진행하시겠습니까?"), 
				_T("ezTransXP Plugin"), 
				MB_YESNO) == IDNO )
			{
				throw _T("EZ Trans XP를 영문 폴더에 설치하신 후 다시 시도해 주십시오.");				
			}			
		}

		// 이지트랜스 함수모듬 구조체 초기화
		ZeroMemory(&m_EZDLL, sizeof(m_EZDLL));

		// J2K 번역 모듈 로드
		CString strJ2KDllPath = m_strHomeDir + _T("\\..\\J2KEngine.dll");
		m_EZDLL.hJKMod = LoadLibrary(strJ2KDllPath);	
		if (m_EZDLL.hJKMod == NULL) throw _T("J2KEngine.dll을 로드할 수 없습니다.");

		// DLL로부터 함수 포인터 얻기
		FARPROC pFuncAddr = NULL;

		pFuncAddr = GetProcAddress(m_EZDLL.hJKMod, "J2K_InitializeEx"); 
		memcpy( &m_EZDLL.J2K_InitializeEx, &pFuncAddr, sizeof(FARPROC) );

		pFuncAddr = GetProcAddress(m_EZDLL.hJKMod, "J2K_TranslateMMNT"); 
		memcpy( &m_EZDLL.J2K_TranslateMMNT, &pFuncAddr, sizeof(FARPROC) );

		pFuncAddr = GetProcAddress(m_EZDLL.hJKMod, "J2K_FreeMem"); 
		memcpy( &m_EZDLL.J2K_FreeMem, &pFuncAddr, sizeof(FARPROC) );

		pFuncAddr = GetProcAddress(m_EZDLL.hJKMod, "J2K_StopTranslation"); 
		memcpy( &m_EZDLL.J2K_StopTranslation, &pFuncAddr, sizeof(FARPROC) );

		pFuncAddr = GetProcAddress(m_EZDLL.hJKMod, "J2K_Terminate"); 
		memcpy( &m_EZDLL.J2K_Terminate, &pFuncAddr, sizeof(FARPROC) );


		if (m_EZDLL.J2K_InitializeEx == NULL || m_EZDLL.J2K_TranslateMMNT == NULL ||
			m_EZDLL.J2K_FreeMem == NULL || m_EZDLL.J2K_StopTranslation == NULL ||
			m_EZDLL.J2K_Terminate == NULL)  throw _T("잘못된 J2KEngine.dll 파일입니다.");

#ifdef UNICODE
		char szHomeDir[MAX_PATH];
		::WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)m_strHomeDir, -1, szHomeDir, MAX_PATH, NULL, NULL);
		m_EZDLL.J2K_InitializeEx(EZTR_INIT_STR, szHomeDir);
#else
		m_EZDLL.J2K_InitializeEx(EZTR_INIT_STR, (LPCSTR)m_strHomeDir);
#endif

	}
	catch (LPCTSTR strErr)
	{
		m_strErrorMsg = strErr;
	}


	return;
}



void CezTransXPApp::CloseEZTrans()
{
	if (m_EZDLL.J2K_StopTranslation && m_EZDLL.J2K_Terminate)
	{
		m_EZDLL.J2K_StopTranslation(0);		/*    m_EZDLL.J2K_Terminate(); */
	}

	if (m_EZDLL.hJKMod)
	{
		/*    FreeLibrary(m_EZDLL.hJKMod); */
	}

	ZeroMemory(&m_EZDLL, sizeof(m_EZDLL));

	g_hSettingWnd = NULL;

}

BOOL CezTransXPApp::OnPluginOption()
{
	if(g_hSettingWnd && IsWindow(g_hSettingWnd))
	{
		CEZTransOptionDlg opt_dlg;
		opt_dlg.m_bRemoveTrace = m_bRemoveTrace;
		opt_dlg.m_bRemoveDupSpace = m_bRemoveDupSpace;
		
		if(opt_dlg.DoModal() == IDOK)
		{
			// 옵션 적용
			m_bRemoveTrace = opt_dlg.m_bRemoveTrace;
			m_bRemoveDupSpace = opt_dlg.m_bRemoveDupSpace;
			
			g_pOptStrBuf[0] = '0' + m_bRemoveTrace;
			g_pOptStrBuf[1] = '0' + m_bRemoveDupSpace;
			g_pOptStrBuf[2] = '\0';

			// 옵션 생략 가능?
			if(m_bRemoveTrace == TRUE && m_bRemoveDupSpace == TRUE) g_pOptStrBuf[0] = '\0';

		}
	}
	return TRUE;
}


BOOL CezTransXPApp::OnPluginClose()
{
	if(m_hTransThread)
	{
		HANDLE hTmp = m_hTransThread;
		m_hTransThread = NULL;
		::SetEvent(m_hRequestEvent);
		::WaitForSingleObject(m_hTransThread, 3000);
		::CloseHandle(m_hTransThread);
	}

	// 동기화 객체 말기화
	::CloseHandle(m_hRequestEvent);
	::CloseHandle(m_hResponseEvent);
	m_hRequestEvent = NULL;
	m_hResponseEvent = NULL;
	DeleteCriticalSection(&m_csTrans);
	
	return TRUE;
}

BOOL CezTransXPApp::Translate( LPCSTR cszJapanese, LPSTR szKorean, int nBufSize )
{
	BOOL bRetVal = FALSE;

	EnterCriticalSection(&m_csTrans);

	m_nBufSize = nBufSize;
	m_pJpnText = cszJapanese;
	m_pKorText = szKorean;

	ResetEvent(m_hResponseEvent);
	BOOL bSetEventResult = ::SetEvent(m_hRequestEvent);
	TRACE(_T("[aral1] Request Trans Event ON!! (bSetEventResult : %d)"), bSetEventResult);

	DWORD dwTransWait = ::WaitForSingleObject(m_hResponseEvent, 3000);
	TRACE(_T("[aral1] Received Response of Trans Event (dwTransWait : %d)"), dwTransWait);

	if(dwTransWait == WAIT_OBJECT_0) bRetVal = TRUE;

	LeaveCriticalSection(&m_csTrans);

	return bRetVal;
}



// 이지트랜스 홈 디랙토리 초기화
CString CezTransXPApp::GetEZTransHomeDir()
{
	CString strRetVal = _T("");

	TCHAR szHomeDir[MAX_PATH];
	HKEY hKey;
	DWORD dwLen = MAX_PATH;

	try
	{
		if (RegOpenKeyEx(HKEY_CURRENT_USER, _T("Software\\ChangShin\\ezTrans"), 0,
			KEY_QUERY_VALUE, &hKey) != ERROR_SUCCESS) throw -1;


		if (RegQueryValueEx(hKey, _T("FilePath"), NULL, NULL, (LPBYTE)szHomeDir,
			&dwLen) != ERROR_SUCCESS) throw -2;

		RegCloseKey(hKey);

		if ( _tcslen(szHomeDir) == 0 ) throw -3;

		strRetVal.Format(_T("%s\\Dat"), szHomeDir);
	}
	catch (int nErrCode)
	{
		nErrCode = nErrCode;
	}

	return strRetVal;
}



unsigned int __stdcall CezTransXPApp::TransThreadFunc(void* pParam)
{
	TRACE(_T("[aral1] 번역 쓰레드 시작"));

	// 번역텍스트 캐시
	map<UINT, CTextElement*> mapCache;
	CTextElement CacheHead;
	CTextElement CacheTail;
	CacheHead.pNextLink = &CacheTail;
	CacheTail.pPrevLink = &CacheHead;
	
	// 번역용 버퍼
	char* pBuf1 = new char[TRANS_BUF_SIZE];
	char* pBuf2 = new char[TRANS_BUF_SIZE];


	CezTransXPApp* pThis = (CezTransXPApp*)pParam;

	pThis->InitEZTrans();
	::SetEvent(pThis->m_hResponseEvent);	// 이지트랜스 초기화가 되었다고 알려주는 용도

	while(pThis->m_hTransThread)
	{
		DWORD dwTransWait = WaitForSingleObject(pThis->m_hRequestEvent, INFINITE);
		TRACE(_T("[aral1] Received Request of Trans Event (dwTransWait : %d)"), dwTransWait);
		if(pThis->m_hTransThread == NULL)
		{
			TRACE(_T("[aral1] Exit (pThis->m_hTransThread == NULL)"));
			break;
		}

		if('\0' == pThis->m_pJpnText[0] || strlen(pThis->m_pJpnText) > TRANS_BUF_SIZE)
		{
			pThis->m_pKorText[0] = '\0';
		}
		else
		{
			// 이 원문 텍스트의 해시 구하기
			UINT dwTextHash = MakeStringHash(pThis->m_pJpnText);

			// 캐시에서 찾아보기
			map<UINT, CTextElement*>::iterator iter = mapCache.find(dwTextHash);
			CTextElement* pTextElem = NULL;
			
			// 캐시에 있으면
			if(mapCache.end() != iter)
			{
				pTextElem = iter->second;

				// 리스트에서 빠져나오기
				pTextElem->pPrevLink->pNextLink = pTextElem->pNextLink;
				pTextElem->pNextLink->pPrevLink = pTextElem->pPrevLink;

			}
			// 캐시에 없으면 이지트랜스로 번역
			else
			{
				LPCSTR cszJpnText = pThis->m_pJpnText;
				

				// 괄호 문자 인코딩
				if(pThis->m_bRemoveTrace)
				{
					pThis->EncodeTrace(cszJpnText, pBuf1);
					cszJpnText = pBuf1;
				}
				
				// 전각 문자 인코딩
				pThis->EncodeTwoByte(cszJpnText, pBuf2);
				cszJpnText = pBuf2;

				char* szTrans = pThis->m_EZDLL.J2K_TranslateMMNT(0, cszJpnText);

				if(szTrans)
				{
					LPCSTR cszKorText = szTrans;
					
					// 중복 공백 제거
					if(pThis->m_bRemoveDupSpace)
					{
						pThis->FilterDupSpaces(cszKorText, pBuf2);
						cszKorText = pBuf2;
					}

					// 전각 문자 디코딩
					pThis->DecodeTwoByte(cszKorText, pBuf1);
					cszKorText = pBuf1;

					// 괄호 문자 디코딩
					if(pThis->m_bRemoveTrace)
					{
						pThis->FilterTrace(cszKorText, pBuf2);
						pThis->DecodeTrace(pBuf2, pBuf1);
						cszKorText = pBuf1;
					}

					pThis->m_EZDLL.J2K_FreeMem(szTrans);

					pTextElem = new CTextElement();
					pTextElem->dwHash = dwTextHash;
					pTextElem->strTranslatedText = cszKorText;

					// 캐시가 꽉 찼다면 빈자리 확보해 놓기
					if(mapCache.size() >= 10000)
					{
						CTextElement* pDelElem = CacheTail.pPrevLink;
						CacheTail.pPrevLink = pDelElem->pPrevLink;

						mapCache.erase(pDelElem->dwHash);
						delete pDelElem;
					}

					// 캐시에 삽입
					mapCache.insert(pair<UINT, CTextElement*>(dwTextHash, pTextElem));
				}

			}


			if(pTextElem && pThis->m_pKorText)
			{
				int nLen = (int)pTextElem->strTranslatedText.length();

				if( nLen > (pThis->m_nBufSize-1) )
				{
					strncpy(pThis->m_pKorText, pTextElem->strTranslatedText.c_str(), pThis->m_nBufSize-1);
					pThis->m_pKorText[pThis->m_nBufSize-1] = _T('\0');
				}
				else
				{
					strcpy(pThis->m_pKorText, pTextElem->strTranslatedText.c_str());
				}
				
				// 헤드 다음으로 삽입
				pTextElem->pPrevLink = &CacheHead;
				pTextElem->pNextLink = CacheHead.pNextLink;

				CacheHead.pNextLink->pPrevLink = pTextElem;
				CacheHead.pNextLink = pTextElem;
			}

		}// end of else of if('\0' == pThis->m_pJpnText[0])

		BOOL bSetEventResult = ::SetEvent(pThis->m_hResponseEvent);
		TRACE(_T("[aral1] Response Trans Event ON!! (result : %d)"), bSetEventResult);
	}
	
	// 버퍼 삭제
	delete [] pBuf1;
	delete [] pBuf2;
	
	// 맵의 원소들 삭제
	TRACE(_T("[aral1] Map : %d"), mapCache.size());
	for(map<UINT, CTextElement*>::iterator iter = mapCache.begin();
		iter != mapCache.end();
		iter++)
	{
		CTextElement* pTextElem = iter->second;
		if(pTextElem) delete pTextElem;
	}


	pThis->CloseEZTrans();

	TRACE(_T("[aral1] Trans Thread Terminated!!!!!"));

	return 0;
}



//////////////////////////////////////////////////////////////////////////
//
// 번역기가 괄호를 마음대로 붙여버리는 경우 일부 게임에서 오류 증상
// 따라서 이런 경우 제거
// ex) "소(쇠)고기를 먹었다." -> "소고기를 먹었다."
//
//////////////////////////////////////////////////////////////////////////
void CezTransXPApp::FilterTrace(LPCSTR cszKorSrc, LPSTR szKorTar)
{
	if(NULL==cszKorSrc || NULL==szKorTar || '\0'==cszKorSrc[0] || '\0'==szKorTar[0]) return;

	size_t len = strlen(cszKorSrc);	
	size_t i = 0;	// 오리지널 인덱스
	size_t j = 0;	// 버퍼 인덱스

	while(i<len)
	{
		// 2바이트 문자면
		if((BYTE)cszKorSrc[i] >= 0x80 && (BYTE)cszKorSrc[i+1] != '\0')
		{
			if((BYTE)cszKorSrc[i] == 0xA1 && (BYTE)cszKorSrc[i+1] == 0xA1)
			{
				i += 2;
				szKorTar[j++] = ' ';
			}
			else
			{
				szKorTar[j++] = cszKorSrc[i++];
				szKorTar[j++] = cszKorSrc[i++];
			}
		}
		// 1바이트 문자면
		else
		{
			if(cszKorSrc[i] == '(')
			{
				//// AT 제어 문자이면
				//if( strncmp(&cszKorSrc[i], "(\at", 4) == 0 )
				//{
				//	do
				//	{
				//		szKorTar[j++] = cszKorSrc[i++];
				//	}while(cszKorSrc[i-1] != ')' && i<len);
				//}
				//// 아니라 그냥 '(' 괄호 이면
				//else
				//{
					while(cszKorSrc[i] != ')' && i<len)
					{
						if((BYTE)cszKorSrc[i] >= 0x80 && (BYTE)cszKorSrc[i+1] != '\0') i++;
						i++;
					}

					if(i<len)
					{
						i++;
						if(cszKorSrc[i]==' ') i++;
					}
				//}
			}
			else if(cszKorSrc[i] == '{')
			{
				while(cszKorSrc[i] != '}' && i<len)
				{
					if((BYTE)cszKorSrc[i] >= 0x80 && (BYTE)cszKorSrc[i+1] != '\0') i++;
					i++;
				}

				if(i<len)
				{
					i++;
					if(cszKorSrc[i]==' ') i++;
				}
			}
			else if(cszKorSrc[i] == '[')
			{
				while(cszKorSrc[i] != ']' && i<len)
				{
					if((BYTE)cszKorSrc[i] >= 0x80 && (BYTE)cszKorSrc[i+1] != '\0') i++;
					i++;
				}

				if(i<len)
				{
					i++;
					if(cszKorSrc[i]==' ') i++;
				}
			}
			else if(cszKorSrc[i] == '<')
			{
				while((cszKorSrc[i] != '>'  && cszKorSrc[i] != ')')&& i<len)
				{
					if((BYTE)cszKorSrc[i] >= 0x80 && (BYTE)cszKorSrc[i+1] != '\0') i++;
					i++;
				}

				if(i<len)
				{
					i++;
					if(cszKorSrc[i]==' ') i++;
					//if(j>0 && ':' == szKorText[j-1]) szKorText[j-1] = ' ';
				}
			}
			else if(cszKorSrc[i] == ':')
			{
				i++;
			}
			else
			{
				szKorTar[j++] = cszKorSrc[i++];
			}
		}
	}

	szKorTar[j] = '\0';

#ifdef DEBUG
	OutputDebugStringA("<FilterTrace Start>");
	OutputDebugStringA(cszKorSrc);
	OutputDebugStringA(szKorTar);
	OutputDebugStringA("<FilterTrace End>");
#endif
}


//////////////////////////////////////////////////////////////////////////
//
// 번역기를 거친 후 공백(' ')이 필요이상으로 늘어나는 경우가 있다.
// 이런 경우 하나만 남기고 모두 제거
// ex) "즉 , 이렇게  변함"   ->   "즉, 이렇게 변함"
//
//////////////////////////////////////////////////////////////////////////
void CezTransXPApp::FilterDupSpaces(LPCSTR cszKorSrc, LPSTR szKorTar)
{
	if(NULL==cszKorSrc || NULL==szKorTar || '\0'==cszKorSrc[0] || '\0'==szKorTar[0]) return;

	size_t len = strlen(cszKorSrc);	
	size_t i = 0;	// 오리지널 인덱스
	size_t j = 0;	// 버퍼 인덱스

	while(i<len)
	{
		// 2바이트 문자면
		if((BYTE)cszKorSrc[i] >= 0x80 && (BYTE)cszKorSrc[i+1] != '\0')
		{
			if((BYTE)cszKorSrc[i] == 0xA1 &&	//「 」 『 』 
				0xB8 <= (BYTE)cszKorSrc[i+1] && (BYTE)cszKorSrc[i+1] <= 0xBB)
			{
				if(j>0 && szKorTar[j-1] == ' ') j--;
				szKorTar[j++] = cszKorSrc[i++];
				szKorTar[j++] = cszKorSrc[i++];
				if(cszKorSrc[i] == ' ') i++;
			}
			else
			{
				szKorTar[j++] = cszKorSrc[i++];
				szKorTar[j++] = cszKorSrc[i++];
			}
			
		}
		// 1바이트 문자면
		else
		{
			if((cszKorSrc[i] == ' ') && (i+1 < len) && (cszKorSrc[i+1] == ' ' || cszKorSrc[i+1] == ','))
			{
				i++;
			}
			else
			{
				szKorTar[j++] = cszKorSrc[i++];
			}
		}
	}

	szKorTar[j] = '\0';

#ifdef DEBUG
	OutputDebugStringA("<FilterDupSpaces Start>");
	OutputDebugStringA(cszKorSrc);
	OutputDebugStringA(szKorTar);
	OutputDebugStringA("<FilterDupSpaces End>");
#endif
}

//////////////////////////////////////////////////////////////////////////
//
// 전각 특수문자를 이지트랜스가 마음대로 ASCII코드로 바꿔버려서 이를 방지
// 하기위해 값을 잠시 변환시킴
// ex) "〈ゆか〉" -> "(\atA3BC)ゆか(\atA3BE)"
//
//////////////////////////////////////////////////////////////////////////
void CezTransXPApp::EncodeTwoByte(LPCSTR cszJpnSrc, LPSTR szJpnTar)
{
	if(NULL==cszJpnSrc || NULL==szJpnTar) return;

	if('\0'==cszJpnSrc[0])
	{
		szJpnTar[0] = '\0';
		return;
	}
	
	size_t len = strlen(cszJpnSrc);

	size_t i = 0;	// 오리지널 인덱스
	size_t j = 0;	// 버퍼 인덱스

	while(i<len)
	{
		if((BYTE)cszJpnSrc[i] >= 0x80 && (BYTE)cszJpnSrc[i+1] != '\0')
		{
			int nTarInc = 2;

			szJpnTar[j] = cszJpnSrc[i];
			szJpnTar[j+1] = cszJpnSrc[i+1];

			for(int k=0; _SHIFT_JIS_CHAR_MAP[k][0]; k++)
			{
				if(_SHIFT_JIS_CHAR_MAP[k][2] == cszJpnSrc[i]
					&& _SHIFT_JIS_CHAR_MAP[k][3] == cszJpnSrc[i+1])
				{
					nTarInc = sprintf(&szJpnTar[j], "_&%03u%03u&_", (BYTE)_SHIFT_JIS_CHAR_MAP[k][0], (BYTE)_SHIFT_JIS_CHAR_MAP[k][1]);
				}
			}
			
			i += 2;
			j += nTarInc;
		}
		else
		{
			szJpnTar[j++] = cszJpnSrc[i++];
		}
	}

	szJpnTar[j] = '\0';
	
#ifdef DEBUG
	OutputDebugStringA("<EncodeTwoByte Start>");
	OutputDebugStringA(cszJpnSrc);
	OutputDebugStringA(szJpnTar);
	OutputDebugStringA("<EncodeTwoByte End>");
#endif
}




//////////////////////////////////////////////////////////////////////////
//
// 이지트랜스 번역 전 변환시켰던 전각특수기호들을 복구
// ex)  "(\atA3BC)주인공(\atA3BE)" -> "〈주인공〉"
//
//////////////////////////////////////////////////////////////////////////
void CezTransXPApp::DecodeTwoByte(LPCSTR cszKorSrc, LPSTR szKorTar)
{
	if(NULL==cszKorSrc || NULL==szKorTar) return;

	if('\0'==cszKorSrc[0])
	{
		szKorTar[0] = '\0';
		return;
	}

	size_t len = strlen(cszKorSrc);

	size_t i = 0;	// 오리지널 인덱스
	size_t j = 0;	// 버퍼 인덱스

	while(i<len)
	{		
		size_t nCopyLen = len-i;
		const char* pEncPtr = strstr(&cszKorSrc[i], "_&");
				
		// "_&" 프리픽스를 찾았다면
		if(NULL != pEncPtr)
		{
			// 변환시킬 데이터가 확실한가?
			//if(	(UINT_PTR)pEncPtr - (UINT_PTR)cszKorSrc + 9 < len
			//	&& '&' == *(pEncPtr+8)
			//	&& '_' == *(pEncPtr+9) )
			if(GetEncodedLen(pEncPtr) == 10)
			{
				nCopyLen = (UINT_PTR)pEncPtr - (UINT_PTR)(&cszKorSrc[i]);
			}
			// 변환시키면 안되는 _&라면
			else
			{
				pEncPtr = NULL;
				nCopyLen = 2;
			}
		}
		else
		{
			pEncPtr = NULL;
		}

		// 일반 문자열 복사
		memcpy(&szKorTar[j], &cszKorSrc[i], nCopyLen);
		i += nCopyLen;
		j += nCopyLen;

		if(pEncPtr)
		{
			// 특수 문자 디코드
			int val1, val2;
			sscanf(pEncPtr+2, "%03u%03u", &val1, &val2);
			((BYTE*)szKorTar)[j] = (BYTE)val1;
			((BYTE*)szKorTar)[j+1] = (BYTE)val2;
			
			i += 10;
			j += 2;
		}

	}

	szKorTar[j] = '\0';

#ifdef DEBUG
	OutputDebugStringA("<DecodeTwoByte Start>");
	OutputDebugStringA(cszKorSrc);
	OutputDebugStringA(szKorTar);
	OutputDebugStringA("<DecodeTwoByte End>");
#endif
}

//////////////////////////////////////////////////////////////////////////
//
// 자동 생성되는 괄호를 확실히 판별하기 위해
// 원래 있던 괄호는 부호화
// ex) "[ゆか]" -> "(\at5B)ゆか(\at5D)"
//
//////////////////////////////////////////////////////////////////////////
void CezTransXPApp::EncodeTrace(LPCSTR cszJpnSrc, LPSTR szJpnTar)
{
	if(NULL==cszJpnSrc || NULL==szJpnTar) return;

	if('\0'==cszJpnSrc[0])
	{
		szJpnTar[0] = '\0';
		return;
	}

	size_t len = strlen(cszJpnSrc);

	size_t i = 0;	// 오리지널 인덱스
	size_t j = 0;	// 버퍼 인덱스

	while(i<len)
	{
		
		// 2바이트 문자인 경우
		if((BYTE)cszJpnSrc[i] >= 0x80 && (BYTE)cszJpnSrc[i+1] != '\0')
		{
			szJpnTar[j++] = cszJpnSrc[i++];
			szJpnTar[j++] = cszJpnSrc[i++];
		}
		// 1바이트 문자인 경우
		else
		{
			// 문자가 괄호면
			if(cszJpnSrc[i] == '(' || cszJpnSrc[i] == ')'
				|| cszJpnSrc[i] == '[' || cszJpnSrc[i] == ']'
				|| cszJpnSrc[i] == '{' || cszJpnSrc[i] == '}'
				|| cszJpnSrc[i] == '<' || cszJpnSrc[i] == '>'
				|| cszJpnSrc[i] == ':')
			{
				j += sprintf(&szJpnTar[j], "_&%03u&_", (BYTE)cszJpnSrc[i++]);
			}
			// 일반 문자면
			else
			{
				szJpnTar[j++] = cszJpnSrc[i++];
			}
		}
	}

	szJpnTar[j] = '\0';

#ifdef DEBUG
	OutputDebugStringA("<EncodeTrace Start>");
	OutputDebugStringA(cszJpnSrc);
	OutputDebugStringA(szJpnTar);
	OutputDebugStringA("<EncodeTrace End>");
#endif
}




//////////////////////////////////////////////////////////////////////////
//
// 이지트랜스 번역 전 변환시켰던 괄호들을 복구
// ex) "(\at5B)ゆか(\at5D)" -> "[ゆか]"
//
//////////////////////////////////////////////////////////////////////////
void CezTransXPApp::DecodeTrace(LPCSTR cszKorSrc, LPSTR szKorTar)
{
	if(NULL==cszKorSrc || NULL==szKorTar) return;

	if('\0'==cszKorSrc[0])
	{
		szKorTar[0] = '\0';
		return;
	}

	size_t len = strlen(cszKorSrc);

	size_t i = 0;	// 오리지널 인덱스
	size_t j = 0;	// 버퍼 인덱스

	while(i<len)
	{
		size_t nCopyLen = len-i;
		const char* pEncPtr = strstr(&cszKorSrc[i], "_&");

		// "_&" 프리픽스를 찾았다면
		if(NULL != pEncPtr)
		{
			// 변환시킬 데이터가 확실한가?
			//if(	(UINT_PTR)pEncPtr - (UINT_PTR)cszKorSrc + 6 < len
			//	&& '&' == *(pEncPtr+5)
			//	&& '_' == *(pEncPtr+6) )
			if(GetEncodedLen(pEncPtr) == 7)
			{
				nCopyLen = (UINT_PTR)pEncPtr - (UINT_PTR)(&cszKorSrc[i]);
			}
			// 변환시키면 안되는 _&라면
			else
			{
				pEncPtr = NULL;
				nCopyLen = 2;
			}
		}
		else
		{
			pEncPtr = NULL;
		}

		// 일반 문자열 복사
		memcpy(&szKorTar[j], &cszKorSrc[i], nCopyLen);
		i += nCopyLen;
		j += nCopyLen;

		if(pEncPtr)
		{
			// 괄호 문자 디코드
			int val1;
			sscanf(pEncPtr+2, "%03u", &val1);
			((BYTE*)szKorTar)[j] = (BYTE)val1;

			i += 7;
			j += 1;
		}

	}

	szKorTar[j] = '\0';

#ifdef DEBUG
	OutputDebugStringA("<DncodeTrace Start>");
	OutputDebugStringA(cszKorSrc);
	OutputDebugStringA(szKorTar);
	OutputDebugStringA("<DncodeTrace End>");
#endif
}

size_t CezTransXPApp::GetEncodedLen( LPCSTR cszBytes )
{
	size_t nRetVal = 0;

	if(NULL != cszBytes)
	{
		size_t len = strlen(cszBytes);

		if(len >= 7 
			&& '_' == cszBytes[0] 
			&& '&' == cszBytes[1]
			&& isdigit(cszBytes[2])
			&& isdigit(cszBytes[3])
			&& isdigit(cszBytes[4])
			&& '&' == cszBytes[5] 
			&& '_' == cszBytes[6]) nRetVal = 7;
		else if(len >= 10 
			&& '_' == cszBytes[0] 
			&& '&' == cszBytes[1]
			&& isdigit(cszBytes[2])
			&& isdigit(cszBytes[3])
			&& isdigit(cszBytes[4])
			&& isdigit(cszBytes[5])
			&& isdigit(cszBytes[6])
			&& isdigit(cszBytes[7])
			&& '&' == cszBytes[8] 
			&& '_' == cszBytes[9]) nRetVal = 10;
	}

	return nRetVal;
}