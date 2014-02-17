
#pragma warning(disable:4312)
#pragma warning(disable:4313)
#pragma warning(disable:4996)

#include "Cached.h"
#include "CachedTextArgMgr.h"
#include "CachedTextArg.h"
#include "CharacterMapper.h"
#include "Function.h"
#include "RegistryMgr/cRegistryMgr.h"
#include "AralProcessAPI/AralProcessAPI.h"

#define TEXT_ARG_POOL_SIZE 100

extern CCachedTextArgMgr g_objCachedTextArgMgr;
CCachedTextArgMgr*	CCachedTextArgMgr::_Inst = NULL;


CCachedTextArgMgr::CCachedTextArgMgr(void)
	: m_hFont(NULL), m_distBest(0)
{
	_Inst = this;
	ZeroMemory(&m_sContainerFunc, sizeof(CONTAINER_FUNCTION_ENTRY));
	ZeroMemory(&m_sTextFunc, sizeof(TEXT_FUNCTION_ENTRY));
}


CCachedTextArgMgr::~CCachedTextArgMgr(void)
{
	_Inst = NULL;
	Close();
}

// 초기화
BOOL CCachedTextArgMgr::Init(HWND hSettingWnd, LPSTR cszOptionStringBuffer) 
{
	Close();

	BOOL bRetVal = FALSE;

	// 부모 윈도우 핸들 저장
	if(NULL==hSettingWnd) return FALSE;
	m_hContainerWnd = hSettingWnd;

	// 컨테이너 함수 포인터 얻어오기
	m_hContainer = GetModuleHandle("ATCTNR.DLL");
	if(m_hContainer)
	{
		m_sContainerFunc.pfnGetCurAlgorithm		= (PROC_GetCurAlgorithm) GetProcAddress( m_hContainer, "GetCurAlgorithm" );
		m_sContainerFunc.pfnGetCurTranslator	= (PROC_GetCurTranslator) GetProcAddress( m_hContainer, "GetCurTranslator" );
		m_sContainerFunc.pfnHookDllFunctionEx		= (PROC_HookDllFunctionEx) GetProcAddress( m_hContainer, "HookDllFunctionEx" );
		m_sContainerFunc.pfnGetOrigDllFunction	= (PROC_GetOrigDllFunction) GetProcAddress( m_hContainer, "GetOrigDllFunction" );
		m_sContainerFunc.pfnUnhookDllFunction	= (PROC_UnhookDllFunction) GetProcAddress( m_hContainer, "UnhookDllFunction" );
		m_sContainerFunc.pfnHookCodePointEx		= (PROC_HookCodePointEx) GetProcAddress( m_hContainer, "HookCodePointEx" );
		m_sContainerFunc.pfnUnhookCodePoint		= (PROC_UnhookCodePoint) GetProcAddress( m_hContainer, "UnhookCodePoint" );
		m_sContainerFunc.pfnTranslateText		= (PROC_TranslateText) GetProcAddress( m_hContainer, "TranslateText" );
		m_sContainerFunc.pfnSuspendAllThread	= (PROC_SuspendAllThread) GetProcAddress( m_hContainer, "SuspendAllThread" );
		m_sContainerFunc.pfnResumeAllThread		= (PROC_ResumeAllThread) GetProcAddress( m_hContainer, "ResumeAllThread" );
		m_sContainerFunc.pfnIsAllThreadSuspended= (PROC_IsAllThreadSuspended) GetProcAddress( m_hContainer, "IsAllThreadSuspended" );
	}

	if( m_sContainerFunc.pfnGetCurAlgorithm && m_sContainerFunc.pfnGetCurTranslator
		&& m_sContainerFunc.pfnHookDllFunctionEx && m_sContainerFunc.pfnGetOrigDllFunction
		&& m_sContainerFunc.pfnUnhookDllFunction && m_sContainerFunc.pfnHookCodePointEx
		&& m_sContainerFunc.pfnUnhookCodePoint && m_sContainerFunc.pfnTranslateText
		&& m_sContainerFunc.pfnSuspendAllThread && m_sContainerFunc.pfnResumeAllThread
		&& m_sContainerFunc.pfnIsAllThreadSuspended)
	{
		// 모든 쓰레드 정지
		m_sContainerFunc.pfnSuspendAllThread();		
		
		// 텍스트 함수군 후킹		
		// GetGlyphOutlineA
		if( m_sContainerFunc.pfnHookDllFunctionEx( "GDI32.DLL", "GetGlyphOutlineA", NewGetGlyphOutlineA, 0 ) )
		{
			m_sTextFunc.pfnGetGlyphOutlineA = 
				(PROC_GetGlyphOutline) m_sContainerFunc.pfnGetOrigDllFunction("GDI32.DLL", "GetGlyphOutlineA");
		}

		// GetGlyphOutlineW
		if( m_sContainerFunc.pfnHookDllFunctionEx( "GDI32.DLL", "GetGlyphOutlineW", NewGetGlyphOutlineW, 0 ) )
		{
			m_sTextFunc.pfnGetGlyphOutlineW = 
				(PROC_GetGlyphOutline) m_sContainerFunc.pfnGetOrigDllFunction("GDI32.DLL", "GetGlyphOutlineW");
		}

		// TextOutA
		if( m_sContainerFunc.pfnHookDllFunctionEx( "GDI32.DLL", "TextOutA", NewTextOutA, 0 ) )
		{
			m_sTextFunc.pfnTextOutA = 
				(PROC_TextOut) m_sContainerFunc.pfnGetOrigDllFunction("GDI32.DLL", "TextOutA");
		}

		// TextOutW
		if( m_sContainerFunc.pfnHookDllFunctionEx( "GDI32.DLL", "TextOutW", NewTextOutW, 0 ) )
		{
			m_sTextFunc.pfnTextOutW = 
				(PROC_TextOut) m_sContainerFunc.pfnGetOrigDllFunction("GDI32.DLL", "TextOutW");
		}

		// ExtTextOutA
		if( m_sContainerFunc.pfnHookDllFunctionEx( "GDI32.DLL", "ExtTextOutA", NewExtTextOutA, 0 ) )
		{
			m_sTextFunc.pfnExtTextOutA = 
				(PROC_ExtTextOut) m_sContainerFunc.pfnGetOrigDllFunction("GDI32.DLL", "ExtTextOutA");
		}

		// ExtTextOutW
		if( m_sContainerFunc.pfnHookDllFunctionEx( "GDI32.DLL", "ExtTextOutW", NewExtTextOutW, 0 ) )
		{
			m_sTextFunc.pfnExtTextOutW = 
				(PROC_ExtTextOut) m_sContainerFunc.pfnGetOrigDllFunction("GDI32.DLL", "ExtTextOutW");
		}

		// 어플로케일 관련 함수
		m_sTextFunc.pfnOrigMultiByteToWideChar =
			(PROC_MultiByteToWideChar) CRegistryMgr::RegReadDWORD("HKEY_CURRENT_USER\\Software\\AralGood", "M2WAddr");

		m_sTextFunc.pfnOrigWideCharToMultiByte =
			(PROC_WideCharToMultiByte) CRegistryMgr::RegReadDWORD("HKEY_CURRENT_USER\\Software\\AralGood", "W2MAddr");

		if( m_sTextFunc.pfnExtTextOutA && m_sTextFunc.pfnExtTextOutW
			&& m_sTextFunc.pfnGetGlyphOutlineA && m_sTextFunc.pfnGetGlyphOutlineW
			&& m_sTextFunc.pfnOrigMultiByteToWideChar && m_sTextFunc.pfnOrigWideCharToMultiByte
			&& m_sTextFunc.pfnTextOutA && m_sTextFunc.pfnTextOutW )
		{
			// 어플로케일 검사
			HMODULE hApplocale = GetModuleHandleA("AlLayer.dll");
			if(hApplocale)
			{
				m_bApplocale = TRUE;
			}

			// 텍스트 인자 풀 생성
			for(int i=0; i<TEXT_ARG_POOL_SIZE; i++)
			{
				m_setInactivatedArgs.insert(new CCachedText());
			}

			bRetVal = TRUE;
		}

		// 모든 쓰레드 재가동
		m_sContainerFunc.pfnResumeAllThread();
	}

	if( FALSE == bRetVal ) Close();

	return bRetVal;
}

// 말기화
BOOL CCachedTextArgMgr::Close()
{
	if(NULL==m_hContainerWnd) return FALSE;

	// 모든 쓰레드 정지
	m_sContainerFunc.pfnSuspendAllThread();		

	// GetGlyphOutlineA 언훅
	if( m_sTextFunc.pfnGetGlyphOutlineA )
	{
		m_sContainerFunc.pfnUnhookDllFunction( "GDI32.DLL", "GetGlyphOutlineA" );
		m_sTextFunc.pfnGetGlyphOutlineA = NULL;
	}

	// GetGlyphOutlineW 언훅
	if( m_sTextFunc.pfnGetGlyphOutlineW )
	{
		m_sContainerFunc.pfnUnhookDllFunction( "GDI32.DLL", "GetGlyphOutlineW" );
		m_sTextFunc.pfnGetGlyphOutlineW = NULL;
	}

	// TextOutA 언훅
	if( m_sTextFunc.pfnTextOutA )
	{
		m_sContainerFunc.pfnUnhookDllFunction( "GDI32.DLL", "TextOutA" );
		m_sTextFunc.pfnTextOutA = NULL;
	}

	// TextOutW 언훅
	if( m_sTextFunc.pfnTextOutW )
	{
		m_sContainerFunc.pfnUnhookDllFunction( "GDI32.DLL", "TextOutW" );
		m_sTextFunc.pfnTextOutW = NULL;
	}

	// ExtTextOutA 언훅
	if( m_sTextFunc.pfnExtTextOutA )
	{
		m_sContainerFunc.pfnUnhookDllFunction( "GDI32.DLL", "ExtTextOutA" );
		m_sTextFunc.pfnExtTextOutA = NULL;
	}

	// ExtTextOutW 언훅
	if( m_sTextFunc.pfnExtTextOutW )
	{
		m_sContainerFunc.pfnUnhookDllFunction( "GDI32.DLL", "ExtTextOutW" );
		m_sTextFunc.pfnExtTextOutW = NULL;
	}

	// 활성화 텍스트 인스턴스 모두 삭제
	for(CCachedTextSet::iterator iter = m_setActivatedArgs.begin();
		iter != m_setActivatedArgs.end();
		iter++)
	{
		CCachedText* pCachedText = *(iter);
		delete pCachedText;
	}
	m_setActivatedArgs.clear();
	
	// 비활성화 텍스트 인스턴스 모두 삭제
	for(CCachedTextSet::iterator iter = m_setInactivatedArgs.begin();
		iter != m_setInactivatedArgs.end();
		iter++)
	{
		CCachedText* pCachedText = *(iter);
		delete pCachedText;
	}
	m_setInactivatedArgs.clear();

	// 적중한 거리들 삭제
	m_mapHitDist.clear();
	m_distBest = 0;

	// 함수 목록 삭제
	for( CFunctionMap::iterator iter2 = m_mapFunc.begin();
		iter2 != m_mapFunc.end();
		iter2++)
	{
		CFunction* pFunc = iter2->second;
		delete pFunc;
	}
	m_mapFunc.clear();

	// 후킹한 함수들 해제
	for( CArgInfoMap::iterator iter3 = m_mapArgInfoA.begin();
		iter3 != m_mapArgInfoA.end();
		iter3++)
	{
		m_sContainerFunc.pfnUnhookCodePoint( (LPVOID)iter3->first );
	}
	m_mapArgInfoA.clear();

	for( CArgInfoMap::iterator iter4 = m_mapArgInfoW.begin();
		iter4 != m_mapArgInfoW.end();
		iter4++)
	{
		m_sContainerFunc.pfnUnhookCodePoint( (LPVOID)iter4->first );
	}
	m_mapArgInfoW.clear();
	

	// 후킹예약배열 삭제
	m_setReservedHooks.clear();

	// 기타 변수 리셋
	m_bApplocale = FALSE;
	m_hContainerWnd = NULL;

	// 모든 쓰레드 재가동
	m_sContainerFunc.pfnResumeAllThread();

	return TRUE;

}



// 새로운 문자열 후보를 추가한다
int CCachedTextArgMgr::AddTextArg(LPVOID pText, BOOL bWideChar, BOOL bAutoTrans, UINT_PTR ptrFunc, size_t dist)
{
	BOOL nRetVal = 0;

	CCachedText* pCachedText = NULL;
	
	// 기존에 들어있는 문자열인지 검사 (활성화 텍스트 인스턴스 모두 순환)
	for(CCachedTextSet::iterator iter = m_setActivatedArgs.begin();
		iter != m_setActivatedArgs.end();
		iter++)
	{
		CCachedText* pTmpCachedText = *(iter);

		if( pTmpCachedText->IsWideCharacter() == bWideChar && pTmpCachedText->TestText(pText) )
		{
			pCachedText = pTmpCachedText;
			nRetVal = 2;
		}		
	}		


	// 삽입이 필요하면 빈노드 또는 최악의 노드를 가져다 추가한다
	if( NULL == pCachedText)
	{	
		if(!m_setInactivatedArgs.empty()) 
		{
			CCachedTextSet::iterator iter = m_setInactivatedArgs.begin();
			pCachedText = ( *iter );
			m_setInactivatedArgs.erase(pCachedText);
			m_setActivatedArgs.insert(pCachedText);
		}
		else
		{
			// 활성화 텍스트 인스턴스 모두 순환
			for(CCachedTextSet::iterator iter = m_setActivatedArgs.begin();
				iter != m_setActivatedArgs.end();
				iter++)
			{
				CCachedText* pTmpCachedText = *(iter);

				if( NULL == pCachedText || (pCachedText->GetHitCount() > pTmpCachedText->GetHitCount()) )
				{
					pCachedText = pTmpCachedText;
				}		
			}		
		}
			

		if( pCachedText->SetCachedText(pText, bWideChar, m_sContainerFunc.pfnTranslateText) )
		{
			nRetVal = 1;
		}
		else
		{
			m_setActivatedArgs.erase( pCachedText );
			m_setInactivatedArgs.insert(pCachedText);
			pCachedText = NULL;
		}
	}

	// 결과가 성공이면 
	if( nRetVal!=0 )
	{
		// 당시 스택 정보 추가
		if( ptrFunc && dist )
		{
			pCachedText->m_setFuncArg.insert( pair<UINT_PTR,size_t>(ptrFunc,dist) );
		}

		// 번역이 필요하면 번역
		if( bAutoTrans )
		{
			pCachedText->Translate();
		}

		wchar_t dbg[2048];
		swprintf(dbg, L"[ aral1 ] %c[0x%p] '%s'('%s') \n",
			(pCachedText->m_bTranslated ? L'★' : L'☆'),
			pText,
			pCachedText->m_wszJapaneseText, 
			pCachedText->m_wszKoreanText
			);
		//OutputDebugStringW(dbg);

	}

	return nRetVal;
}

// 문자열 후보들 전체를 테스트한다. (더이상 일치하지 않는 후보는 바로 삭제)
BOOL CCachedTextArgMgr::TestCharacter(wchar_t wch, void* baseESP)
{
	BOOL bRetVal = FALSE;

	// 활성화 텍스트 인스턴스를 모두 순회
	for(CCachedTextSet::iterator iter = m_setActivatedArgs.begin();
		iter != m_setActivatedArgs.end();)
	{
		CCachedText* pCachedText = (*iter);
		iter++;
		
		// 검사 수행
		int nRes = pCachedText->TestCharacter(wch);
		
		// 히트(유예)시
		if( 0 != nRes )
		{
			if( nRes & 0x01 )
			{
				ModifyHitMap(pCachedText, baseESP, +1);
				bRetVal = TRUE;

				for(set<pair<UINT_PTR,size_t>>::iterator iter = pCachedText->m_setFuncArg.begin();
					iter != pCachedText->m_setFuncArg.end();
					iter++)
				{
					CFunction* pFunc = m_mapFunc[iter->first];
					size_t distArg = iter->second;
					
					if(pFunc && distArg)
					{
						pFunc->m_mapDistScores[distArg]++;
						
						// 만일 특정수치 이상 적중되었으면 이부분 후킹
						if( pFunc->m_mapDistScores[distArg] > 30 
							&& m_sContainerFunc.pfnHookCodePointEx
							&& m_mapArgInfoA.find(pFunc->m_ptrFunction) == m_mapArgInfoA.end()
							&& m_mapArgInfoW.find(pFunc->m_ptrFunction) == m_mapArgInfoW.end() )
						{
							HMODULE hExeMod = GetModuleHandle(NULL);
							HMODULE hHookMod = NULL;

							if( GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCSTR)pFunc->m_ptrFunction, &hHookMod)
								&& hExeMod == hHookMod )
							{
								/*
								PRESERVED_HOOK_POINT pRHP = new RESERVED_HOOK_POINT;
								pRHP->bWideChar	= pCachedText->m_bWideChar;
								pRHP->nArgDist	= distArg;
								pRHP->pHookPoint	= pFunc->m_ptrFunction;
								m_setReservedHooks.insert(pRHP);
								TRACE("[ aral1 ] Function 0x%p(+%d) was reserved for hook \n", pFunc->m_ptrFunction, distArg);
								*/
							}
						}

					}
				}
			}
		}
		// 폐기시
		else
		{
			//// 인자 점수 감소
			//for(set<void*>::iterator iter = pCachedText->m_setSourcePtr.begin();
			//	iter != pCachedText->m_setSourcePtr.end();
			//	iter++)
			//{
			//	void* pSource = (*iter);
			//	size_t dist = (size_t)pSource - (size_t)baseESP;
			//	ModifyHitMap(dist, -1);
			//}
			for(set<pair<UINT_PTR,size_t>>::iterator iter = pCachedText->m_setFuncArg.begin();
				iter != pCachedText->m_setFuncArg.end();
				iter++)
			{
				CFunction* pFunc = m_mapFunc[iter->first];
				size_t distArg = iter->second;

				if(pFunc && distArg)
				{
					pFunc->m_mapDistScores[distArg]--;
				}
			}

			ModifyHitMap(pCachedText, baseESP, -1);
			m_setActivatedArgs.erase(pCachedText);
			m_setInactivatedArgs.insert(pCachedText);
		}
	}	

	return bRetVal;
}


UINT_PTR CCachedTextArgMgr::GetFuncAddrFromReturnAddr(UINT_PTR pAddr)
{
	UINT_PTR funcAddr = NULL;

	__try
	{
		if( !IsBadReadPtr( (void*)pAddr, sizeof(void*) ) )
		{
			// 함수콜 모양인지 검사
			BYTE* pRetAddr = (BYTE*)pAddr;
			if( 0xE8 == *(pRetAddr-5) )	// call 코드 case 1
			{
				UINT_PTR func_dist = *( (UINT_PTR*)(pRetAddr-4) );		// 이동거리 구하기
				funcAddr = pAddr + func_dist;							// 구해진 함수주소
			}
			else if( 0xFF == *(pRetAddr-6) && 0x15 == *(pRetAddr-5) )	// call 코드 case 2
			{
				funcAddr = **( (UINT_PTR**)(pRetAddr-4) );				// 구해진 함수주소
			}


			// 함수 포인터 유효성 검사
			if( funcAddr && IsBadCodePtr((FARPROC)funcAddr) )
			{
				funcAddr = NULL;
			}

		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{

	}

	return funcAddr;

}


int CCachedTextArgMgr::SearchStringA(INT_PTR ptrBegin, char ch1, char ch2)
{
	int iRetVal = 0;

	// 인자거리맵 삭제
	//FindBestDistAndClearHitMap();
	
	size_t dist = 0;
	size_t arg_dist = 0;
	CFunction* pCurFunc = NULL;

	while( IsBadReadPtr((void*)(ptrBegin+dist), sizeof(void*)) == FALSE )
	{
		LPSTR* ppText = (LPSTR*)(ptrBegin+dist);
		LPVOID found = NULL;

		if( IsBadStringPtrA(*ppText, 1024*1024*1024)==FALSE && strlen(*ppText) < 1024 )
		{
			if('\0'==ch2)
			{
				found = (LPVOID)strchr(*ppText, ch1);
			}
			else
			{
				char tmp[4] = {0,};
				tmp[0] = ch1;
				tmp[1] = ch2;
				found = (LPVOID)strstr(*ppText, tmp);
			}
		}

		// 문자열이라면
		if(found)
		{
			int nAddRes = AddTextArg( *ppText, FALSE, IsAutoTransPoint(dist), (pCurFunc?pCurFunc->m_ptrFunction:NULL), arg_dist );
			if( nAddRes )
			{
				// 인자 점수 등록
				m_mapHitDist.insert( pair<size_t, int>(dist, 0) );
				iRetVal++;

				//// 함수 점수 증가
				//if( pCurFunc )
				//{
				//	pCurFunc->m_mapDistScores[arg_dist]++;
				//}

			}
		}
		else
		{
			UINT_PTR funcAddr = GetFuncAddrFromReturnAddr( *((UINT_PTR*)(ptrBegin+dist)) );

			// 함수 리턴주소라면 현재 지정된 함수를 교체한다
			if( funcAddr )
			{
				CFunctionMap::iterator iter = m_mapFunc.find( funcAddr );
				// 지금 리스트에 존재하면
				if( iter != m_mapFunc.end() )
				{
					pCurFunc = iter->second;
				}
				// 없으면 새로 생성 & 추가
				else
				{
					pCurFunc = new CFunction(funcAddr);
					m_mapFunc[funcAddr] = pCurFunc;
				}
				
				arg_dist = 0;
			}
		}

		dist += sizeof(void*);
		arg_dist += sizeof(void*);
	}
	
	//char dbg[1024];
	//sprintf(dbg, " [ aral1 ] 찾은거리:0x%p~0x%p (%d bytes) \n", ptrBegin, ptrBegin+dist, dist); OutputDebugStringA(dbg);

	return iRetVal;
}

int CCachedTextArgMgr::SearchStringW(INT_PTR ptrBegin, wchar_t wch)
{
	int iRetVal = 0;

	// 인자거리맵 삭제
	//FindBestDistAndClearHitMap();

	size_t dist = 0;
	size_t arg_dist = 0;
	CFunction* pCurFunc = NULL;

	while( IsBadReadPtr((void*)(ptrBegin+dist), sizeof(void*)) == FALSE )
	{
		LPWSTR* ppText = (LPWSTR*)(ptrBegin+dist);

		// 문자열이라면
		if( IsBadStringPtrW(*ppText, 1024*1024)==FALSE && wcslen(*ppText) < 1024 && NULL!=wcschr(*ppText, wch) )
		{

			int nAddRes = AddTextArg( *ppText, TRUE, IsAutoTransPoint(dist), pCurFunc->m_ptrFunction, arg_dist );
			if( nAddRes )
			{
				// 인자 점수 등록
				m_mapHitDist.insert( pair<size_t, int>(dist, 0) );
				iRetVal++;

				//// 함수 점수 증가
				//if( pCurFunc )
				//{
				//	pCurFunc->m_mapDistScores[arg_dist]++;
				//}

			}
		}
		else
		{
			UINT_PTR funcAddr = GetFuncAddrFromReturnAddr( *((UINT_PTR*)(ptrBegin+dist)) );

			// 함수 리턴주소라면 현재 지정된 함수를 교체한다
			if( funcAddr )
			{
				CFunctionMap::iterator iter = m_mapFunc.find( funcAddr );
				// 지금 리스트에 존재하면
				if( iter != m_mapFunc.end() )
				{
					pCurFunc = iter->second;
				}
				// 없으면 새로 생성 & 추가
				else
				{
					pCurFunc = new CFunction(funcAddr);
					m_mapFunc[funcAddr] = pCurFunc;
				}

				arg_dist = 0;
			}
		}

		dist += sizeof(void*);
		arg_dist += sizeof(void*);

	}	

	return iRetVal;
}


void CCachedTextArgMgr::CheckFont(HDC hdc) 
{
	static long lLastFontHeight = -1;

	TEXTMETRIC tm;
	//ZeroMemory(&tm, sizeof(TEXTMETRIC));
	BOOL bRes = GetTextMetrics(hdc, &tm);

	// 폰트 다시 로드
	if( bRes )//&& tm.tmHeight != lLastFontHeight )
	{
		HFONT font = NULL;

		// 이 크기에 해당하는 폰트가 없을 경우 폰트를 생성
		if( m_mapFonts.find(tm.tmHeight) == m_mapFonts.end() )
		{
			font = CreateFont(tm.tmHeight, 0, 0, 0, tm.tmWeight, tm.tmItalic, tm.tmUnderlined, tm.tmStruckOut,
				HANGEUL_CHARSET,	//ANSI_CHARSET,
				OUT_DEFAULT_PRECIS,
				CLIP_DEFAULT_PRECIS,
				ANTIALIASED_QUALITY,
				DEFAULT_PITCH,		// | FF_SWISS,
				"돋움");

			m_mapFonts[tm.tmHeight] = font;
			
		}
		else
		{
			font = m_mapFonts[tm.tmHeight];
		}

		SelectObject(hdc, font);
		lLastFontHeight = tm.tmHeight;

		ZeroMemory(&tm, sizeof(TEXTMETRIC));
		GetTextMetrics(hdc, &tm);
		hdc = hdc;

	}

}


// 최고로 확률이 높은 번역 문자를 반환
wchar_t CCachedTextArgMgr::GetBestTranslatedCharacter() 
{
	wchar_t wchRetVal = L'\0';
	CCachedText* pBestArg = NULL;

	// 활성화 텍스트 인스턴스 모두 순환
	for(CCachedTextSet::iterator iter = m_setActivatedArgs.begin();
		iter != m_setActivatedArgs.end();
		iter++)
	{
		CCachedText* pCachedTextArg = *(iter);

		if( NULL == pBestArg )
		{
			if( 0 < pCachedTextArg->GetHitCount() ) pBestArg = pCachedTextArg;
		}
		else if( pBestArg->m_bTranslated < pCachedTextArg->m_bTranslated )
		{
			pBestArg = pCachedTextArg;
		}
		else if( pBestArg->m_nHitCnt < pCachedTextArg->m_nHitCnt )
		{
			pBestArg = pCachedTextArg;
		}
		else if( pBestArg->m_nHitCnt == pCachedTextArg->m_nHitCnt )
		{
			size_t nBestRef = pBestArg->m_setSourcePtr.size();
			size_t nTempRef = pCachedTextArg->m_setSourcePtr.size();
			if( nBestRef < nTempRef )
			{
				pBestArg = pCachedTextArg;				
			}
			else if( nBestRef == nTempRef && pBestArg->m_nJapaneseLen < pCachedTextArg->m_nJapaneseLen )
			{
				pBestArg = pCachedTextArg;
			}
		}
	}

	if(pBestArg)
	{
		wchRetVal = pBestArg->GetBestTranslatedCharacter();
		//wchar_t dbg[1024];
		//swprintf(dbg, L"[ aral1 ] GetBestTranslatedCharacter() returned '%c' ('%s'[%d]) \n", (wchRetVal ? wchRetVal:L'0'), pBestArg->m_wszKoreanText, pBestArg->m_nNextTestIdx-1);
		//OutputDebugStringW(dbg);
	}

	return wchRetVal;
}


//////////////////////////////////////////////////////////////////////////
//
// 인자 점수 증감 처리
//
//////////////////////////////////////////////////////////////////////////
void CCachedTextArgMgr::ModifyHitMap( CCachedText* pCachedText, void* baseESP, int increment ) 
{
	// 인자 점수 증가
	for(map<size_t,int>::iterator iter = m_mapHitDist.begin();
		iter != m_mapHitDist.end();)
	{
		void** ppSource = (void**)( (UINT_PTR)baseESP + iter->first );
		
		// 텍스트인자에 해당 문자열 포인터가 있으면
		if( IsBadReadPtr(ppSource, sizeof(void*)) == FALSE
			 && pCachedText->m_setSourcePtr.find(*ppSource) != pCachedText->m_setSourcePtr.end() )
		{
			iter->second += increment;
		}

		if( iter->second > 10 )
		{
			iter->second = 10;
			iter++;
		}
		// 더이상 필요없으면 삭제
		else if( iter->second < 0 )
		{
			size_t key = iter->first;
			iter++;
			m_mapHitDist.erase(key);
		}
		// 아직 존재해야 한다면
		else
		{
			iter++;
		}
	}


	//map<size_t,int>::iterator iter = m_mapHitDist.find(dist);
	//
	//// 신규 추가가 필요하면
	//if( iter == m_mapHitDist.end() )
	//{
	//	if( increment > 0 ) m_mapHitDist.insert( pair<size_t,int>(dist, increment) );
	//}
	//// 기존 값 조작시
	//else
	//{
	//	iter->second += increment;
	//	if( iter->second <= 0 )
	//	{
	//		m_mapHitDist.erase(iter);
	//	}
	//}
}


void CCachedTextArgMgr::FindBestDistAndClearHitMap()
{
	int nBestVal = 0;

	// 최고 인자 선별
	for(map<size_t,int>::iterator iter = m_mapHitDist.begin();
		iter != m_mapHitDist.end();
		iter++)
	{
		if( iter->second > nBestVal && iter->second > 2 )
		{
			m_distBest = iter->first;
			nBestVal = iter->second;
		}
	}

	m_mapHitDist.clear();

}

//////////////////////////////////////////////////////////////////////////
//
// 자동 번역지점인지 판단한다
//
//////////////////////////////////////////////////////////////////////////
BOOL CCachedTextArgMgr::IsAutoTransPoint( size_t dist ) 
{
	BOOL bRetVal = FALSE;

	map<size_t,int>::iterator iter = m_mapHitDist.find( dist );
	if( iter != m_mapHitDist.end() && iter->second > 3 )
	{
		bRetVal = TRUE;
	}

	return bRetVal;
}


//////////////////////////////////////////////////////////////////////////
//
// 특정지점 후킹의 콜백 함수 (멀티바이트용)
//
//////////////////////////////////////////////////////////////////////////
void CCachedTextArgMgr::ModifyValueA(LPVOID pHookedPoint, PREGISTER_ENTRY pRegisters)
{
	if(NULL==CCachedTextArgMgr::_Inst) return;

	// 인자정보 테이블에서 인자 구하기
	CArgInfoMap::iterator iter = CCachedTextArgMgr::_Inst->m_mapArgInfoA.find((UINT_PTR)pHookedPoint);
	if( iter != CCachedTextArgMgr::_Inst->m_mapArgInfoA.end() )
	{
		size_t dist = iter->second;
		LPSTR* pArgText = (LPSTR*)( pRegisters->_ESP + dist );		// 텍스트 인자 포인터 구하기
		if( CCachedTextArgMgr::_Inst && IsBadReadPtr(pArgText, sizeof(LPSTR)) == FALSE 
			&& IsBadStringPtrA(*pArgText, 1024*1024*1024) == FALSE )
		{
			LPSTR pText = *pArgText;
			char ch1 = pText[0];
			char ch2 = pText[1];
			int nRes = CCachedTextArgMgr::_Inst->AddTextArg( pText, FALSE, TRUE, NULL, NULL );
			if( nRes == 1 )
			{
				CCachedTextArgMgr::_Inst->SearchStringA((INT_PTR)pRegisters->_ESP, ch1, ch2 );
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////
//
// 특정지점 후킹의 콜백 함수 (유니코드용)
//
//////////////////////////////////////////////////////////////////////////
void CCachedTextArgMgr::ModifyValueW(LPVOID pHookedPoint, PREGISTER_ENTRY pRegisters)
{
	if(NULL==CCachedTextArgMgr::_Inst) return;

	// 인자정보 테이블에서 인자 구하기
	CArgInfoMap::iterator iter = CCachedTextArgMgr::_Inst->m_mapArgInfoW.find((UINT_PTR)pHookedPoint);
	if( iter != CCachedTextArgMgr::_Inst->m_mapArgInfoW.end() )
	{
		size_t dist = iter->second;
		LPWSTR* pArgText = (LPWSTR*)( pRegisters->_ESP + dist );		// 텍스트 인자 포인터 구하기
		if( CCachedTextArgMgr::_Inst && IsBadReadPtr(pArgText, sizeof(LPWSTR)) == FALSE 
			&& IsBadStringPtrW(*pArgText, 1024*1024*1024) == FALSE )
		{
			LPWSTR pText = *pArgText;
			wchar_t wch = pText[0];
			int nRes = CCachedTextArgMgr::_Inst->AddTextArg( pText, TRUE, TRUE, NULL, NULL );
			if( nRes == 1 )
			{
				CCachedTextArgMgr::_Inst->SearchStringW((INT_PTR)pRegisters->_ESP, wch );
			}
		}
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
DWORD CCachedTextArgMgr::InnerGetGlyphOutlineA(
	  HDC hdc,             // handle to device context
	  UINT uChar,          // character to query
	  UINT uFormat,        // format of data to return
	  LPGLYPHMETRICS lpgm, // pointer to structure for metrics
	  DWORD cbBuffer,      // size of buffer for data
	  LPVOID lpvBuffer,    // pointer to buffer for data
	  CONST MAT2 *lpmat2   // pointer to transformation matrix structure
	  )
{

	BOOL bDecoded = FALSE;
	char chArray[10] = {0,};
	char tmpbuf[10]  = {0,};
	wchar_t wchArray[10] = {0,};
	wchar_t wch = L'\0';

	CheckFont(hdc);

	// 바로 앞 전 EBP를 구함
	INT_PTR _CUR_EBP = NULL;
	_asm
	{
		mov eax, [ebp];
		mov _CUR_EBP, eax;
	}

	// char 배열에 문자 넣음
	size_t i,j;
	j = 0;
	for(i=sizeof(UINT); i>0; i--)
	{
		char one_ch = *( ((char*)&uChar) + i - 1 );
		if(one_ch)
		{
			chArray[j] = one_ch;
			j++;
		}
	}

	// 한글 디코드 먼저 시도
	if( 0xE0 <= (BYTE)chArray[0] && (BYTE)chArray[0] <= 0xFC )
	{
		chArray[2] = '\0';

		if( CCharacterMapper::DecodeJ2K(chArray, tmpbuf) )
		{
			MyMultiByteToWideChar(949, 0, tmpbuf, sizeof(UINT), wchArray, 10 );
			bDecoded = TRUE;
		}

	}

	// 디코드 처리된 문자가 아니라면
	if( FALSE == bDecoded )
	{
		MyMultiByteToWideChar(932, 0, chArray, sizeof(UINT), wchArray, 10 );
	}

	wch = wchArray[0];

	// 검사중인 텍스트 포인터들 모두 순회
	BOOL bHitOnce = TestCharacter(wch, (void*)_CUR_EBP);

	// 적중된 포인터가 없다면 검색
	if( FALSE == bHitOnce && FALSE == bDecoded && wch > 0x80)
	{			
		// 예약된 후킹작업 수행
		HookAllReservedPoints();

		// 검색
		int iRes = SearchStringA(_CUR_EBP, chArray[0], chArray[1]);
		if(iRes)
		{
			TestCharacter(wch, (void*)_CUR_EBP);
		}
		
		//sprintf(dbg, "[ aral1 ] ============== 새 검색 결과 : %d ================= \n", iRes ); OutputDebugStringA(dbg);
	}

	//wchar_t dbg[MAX_PATH];
	//swprintf(dbg, L"[ aral1 ] NewTextOutA에서 캐싱 : '%c'( %2X %2X ) \n", wch, lpString[0], lpString[1]);
	//OutputDebugStringW(dbg);


	// 문자 변환
	if(bDecoded)
	{
		uChar = (UINT)(MAKEWORD(tmpbuf[1], tmpbuf[0]));
	}
	else
	{
		wchar_t wtmp[2] = {0,};
		wtmp[0] = GetBestTranslatedCharacter();
		if(wtmp[0])
		{
			MyWideCharToMultiByte(949, 0, wtmp, -1, tmpbuf, 10, NULL, NULL);
			//sprintf(dbg, "[ aral1 ] GetBestTranslatedCharacter() returned (%p) -> (%p)'%s' \n", MAKEWORD(chArray[1], chArray[0]), MAKEWORD(tmpbuf[1], tmpbuf[0]), tmpbuf ); OutputDebugStringA(dbg);
			if(tmpbuf[1])
			{
				uChar = (UINT)(MAKEWORD(tmpbuf[1], tmpbuf[0]));
			}
			else
			{
				uChar = (UINT)tmpbuf[0];
			}
		}
	}


	// 원래함수 호출
	DWORD dwRetVal = 0;

	if( m_sTextFunc.pfnGetGlyphOutlineA )
	{
		//if(uChar<=0x80)
		//{
		//	uFormat = GGO_NATIVE;
		//}

		dwRetVal = m_sTextFunc.pfnGetGlyphOutlineA(hdc, uChar, uFormat, lpgm, cbBuffer, lpvBuffer, lpmat2);
	}

	return dwRetVal;

}


//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
DWORD CCachedTextArgMgr::InnerGetGlyphOutlineW(
	HDC hdc,             // handle to device context
	UINT uChar,          // character to query
	UINT uFormat,        // format of data to return
	LPGLYPHMETRICS lpgm, // pointer to structure for metrics
	DWORD cbBuffer,      // size of buffer for data
	LPVOID lpvBuffer,    // pointer to buffer for data
	CONST MAT2 *lpmat2   // pointer to transformation matrix structure
	)
{
	CheckFont(hdc);

	// 바로 앞 전 EBP를 구함
	INT_PTR _CUR_EBP = NULL;
	_asm
	{
		mov eax, [ebp];
		mov _CUR_EBP, eax;
	}

	wchar_t wch = (wchar_t)uChar;

	// 검사중인 텍스트 포인터들 모두 순회
	BOOL bHitOnce = TestCharacter(wch, (void*)_CUR_EBP);

	// 적중된 포인터가 없다면 검색
	if( FALSE == bHitOnce && wch > 0x80 )
	{			
		// 예약된 후킹작업 수행
		HookAllReservedPoints();

		// 검색
		int iRes = SearchStringW(_CUR_EBP, wch);
		if(iRes)
		{
			TestCharacter(wch, (void*)_CUR_EBP);
		}
		TRACE("[ aral1 ] ============== 새 검색 결과 : %d ================= \n", iRes );			
	}

	//wchar_t dbg[MAX_PATH];
	//swprintf(dbg, L"[ aral1 ] NewTextOutA에서 캐싱 : '%c'( %2X %2X ) \n", wch, lpString[0], lpString[1]);
	//OutputDebugStringW(dbg);

	wch = GetBestTranslatedCharacter();
	if(wch)
	{
		//wchar_t dbg[MAX_PATH];
		//swprintf(dbg, L"[ aral1 ] MyGetGlyphOutlineW : '%c'->'%c' \n", (wchar_t)uChar, wch);
		//OutputDebugStringW(dbg);
		uChar = (UINT)wch;
	}
	else
	{
		uChar = 0x20;
	}

	// 원래함수 호출
	DWORD dwRetVal = 0;

	if( m_sTextFunc.pfnGetGlyphOutlineW )
	{
		if(uChar<=0x80)
		{
			uFormat = GGO_NATIVE;
		}

		dwRetVal = m_sTextFunc.pfnGetGlyphOutlineW(hdc, uChar, uFormat, lpgm, cbBuffer, lpvBuffer, lpmat2);
	}

	return dwRetVal;
}



//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
BOOL CCachedTextArgMgr::InnerTextOutA(
	HDC hdc,           // handle to DC
	int nXStart,       // x-coordinate of starting position
	int nYStart,       // y-coordinate of starting position
	LPCSTR lpString,   // character string
	int cbString       // number of characters
	)
{
	BOOL bRetVal = FALSE;
	BOOL bDecoded = FALSE;
	char tmpbuf[10];

	CheckFont(hdc);

	// 바로 앞 전 EBP를 구해서
	INT_PTR _CUR_EBP = NULL;
	_asm
	{
		mov eax, [ebp];
		mov _CUR_EBP, eax;
	}

	if( cbString<=2 || strlen(lpString)<=2 )
	{
		wchar_t wchArray[10] = {0,};

		// 한글 디코드 먼저 시도
		if( 0xE0 <= (BYTE)lpString[0] && (BYTE)lpString[0] <= 0xFC )
		{
			tmpbuf[2] = '\0';

			if( CCharacterMapper::DecodeJ2K(lpString, tmpbuf) )
			{
				MyMultiByteToWideChar(949, 0, tmpbuf, sizeof(UINT), wchArray, 10 );
				bDecoded = TRUE;
			}
			
		}

		// 디코드 처리된 문자가 아니라면
		if( FALSE == bDecoded )
		{
			MyMultiByteToWideChar(932, 0, lpString, sizeof(UINT), wchArray, 10 );
		}

		wchar_t wch = wchArray[0];

		// 검사중인 텍스트 포인터들 모두 순회
		BOOL bHitOnce = TestCharacter(wch, (void*)_CUR_EBP);

		// 적중된 포인터가 없다면 검색
		if( FALSE == bHitOnce && FALSE == bDecoded && wch > 0x80 )
		{			
			// 예약된 후킹작업 수행
			HookAllReservedPoints();
			
			// 검색
			TRACE("[ aral1 ] \n");			
			TRACE("[ aral1 ] \n");						
			//wchar_t dbg[MAX_PATH];
			//swprintf(dbg, L"[ aral1 ] ============== 새 검색 시작 ('%c') ================= \n", wch);
			//OutputDebugStringW(dbg);
			int iRes = SearchStringA(_CUR_EBP, lpString[0], lpString[1]);
			if(iRes)
			{
				TestCharacter(wch, (void*)_CUR_EBP);
			}
			TRACE("[ aral1 ] ============== 새 검색 결과 : (%d/%d) ================= \n", m_setActivatedArgs.size(), iRes );
		}


		// 문자 변환
		wchar_t wtmp[2] = {0,};
		if( bDecoded )
		{
			wtmp[0] = wch;
		}
		else
		{
			wtmp[0] = GetBestTranslatedCharacter();
			if( L'\0' == wtmp[0] ) wtmp[0] = L' ';
		}

		wchar_t dbg2[MAX_PATH];
		if( bDecoded )
		{
			wcscpy(dbg2, L"Decoded");
		}
		else if( wtmp[0] )
		{
			wcscpy(dbg2, L"GetBestCharacter");
		}
		else
		{
			wcscpy(dbg2, L"failed..");
		}
		//swprintf(dbg, L"[ aral1 ] NewTextOutA : '%c' -> '%c' (%s) \n", wch, wtmp[0], dbg2 );
		//OutputDebugStringW(dbg);

		//lpString = tmpbuf;

		// 원래함수 호출
		if( m_sTextFunc.pfnTextOutW )
		{
			bRetVal = m_sTextFunc.pfnTextOutW(hdc, nXStart, nYStart, wtmp, 1);
		}
	}
	else
	{
		// 원래함수 호출
		if( m_sTextFunc.pfnTextOutA )
		{
			bRetVal = m_sTextFunc.pfnTextOutA(hdc, nXStart, nYStart, lpString, cbString);
		}

	}

	return bRetVal;
}


//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
BOOL CCachedTextArgMgr::InnerTextOutW(
	HDC hdc,           // handle to DC
	int nXStart,       // x-coordinate of starting position
	int nYStart,       // y-coordinate of starting position
	LPCWSTR lpString,   // character string
	int cbString       // number of characters
	)
{
	// 원래함수 호출
	BOOL bRetVal = 0;

	if( m_sTextFunc.pfnTextOutW )
	{
		bRetVal = m_sTextFunc.pfnTextOutW(hdc, nXStart, nYStart, lpString, cbString);
	}

	return bRetVal;
}



//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
BOOL CCachedTextArgMgr::InnerExtTextOutA(
	HDC hdc,          // handle to DC
	int X,            // x-coordinate of reference point
	int Y,            // y-coordinate of reference point
	UINT fuOptions,   // text-output options
	CONST RECT* lprc, // optional dimensions
	LPCSTR lpString, // string
	UINT cbCount,     // number of characters in string
	CONST INT* lpDx   // array of spacing values
	)
{
	// 원래함수 호출
	BOOL bRetVal = 0;

	if( m_sTextFunc.pfnExtTextOutW )
	{
		bRetVal = m_sTextFunc.pfnExtTextOutW(hdc, X, Y, fuOptions, lprc, lpString, cbCount, lpDx);
	}

	return bRetVal;
}



//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
BOOL CCachedTextArgMgr::InnerExtTextOutW(
	HDC hdc,          // handle to DC
	int X,            // x-coordinate of reference point
	int Y,            // y-coordinate of reference point
	UINT fuOptions,   // text-output options
	CONST RECT* lprc, // optional dimensions
	LPCWSTR lpString, // string
	UINT cbCount,     // number of characters in string
	CONST INT* lpDx   // array of spacing values
	)
{
	LPCWSTR lpNewString = lpString;
	//wchar_t wszNewString[1024];

	/*
	if( lpString && lpString[0] )
	{

	// m_hLastDC 가 달라졌다면
	if(hdc != _gInst->m_hLastDC)
	{
	_gInst->m_hLastDC = hdc;
	}

	if( NULL==_gInst->m_hFont )
	{
	_gInst->m_hFont=CreateFont(20, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
	ANSI_CHARSET,
	OUT_DEFAULT_PRECIS,
	CLIP_DEFAULT_PRECIS,
	NONANTIALIASED_QUALITY,
	DEFAULT_PITCH | FF_SWISS,
	"궁서");

	}

	SelectObject(hdc, _gInst->m_hFont);

	// 바로 앞 전 EBP를 구해서
	INT_PTR _CUR_EBP = NULL;
	_asm
	{
	mov _CUR_EBP, ebp;
	}


	size_t len = wcslen(lpString);
	if(cbCount==1)
	{
	wchar_t wch = lpString[0];

	// 검사중인 텍스트 포인터들 모두 순회
	BOOL bHitOnce = _gInst->m_TextArgMgr.TestCharacter(wch);

	// 적중된 포인터가 없다면 검색
	if( FALSE == bHitOnce )
	{
	size_t dist = 0;
	while( IsBadReadPtr((void*)(_CUR_EBP+dist), sizeof(void*)) == FALSE )
	{
	LPWSTR* ppText = (LPWSTR*)(_CUR_EBP+dist);
	if( IsBadStringPtrW(*ppText, 1024*1024)==FALSE && **ppText == wch )
	{
	_gInst->m_TextArgMgr.AddTextArg(*ppText);
	}
	dist += sizeof(void*);
	}
	}

	// 최고의 변역 문자를 얻어옴
	if( _gInst->m_TextArgMgr.GetBestTranslatedCharacter(wszNewString) )
	{
	wchar_t dbg[MAX_PATH];
	swprintf(dbg, L"[ aral1 ] MyExtTextOutW : '%c'->'%s' \n", wch, wszNewString);
	OutputDebugStringW(dbg);

	lpNewString = wszNewString;
	cbCount = wcslen(wszNewString);

	//PROC_TextOut pFunc = (PROC_TextOut) CRawHookMgr::GetOrigFunc(MyTextOutA);
	//return pFunc(hdc, nXStart, nYStart, szNewString, strlen(szNewString));
	}
	}

	else if(len < 50 && 0)
	{
	// 번역
	ITranslator* pTranslator = _gInst->m_TextArgMgr.GetTranslator();
	if(pTranslator)
	{
	// 번역기에 집어넣어라
	char szJapanese[1024] = {0,};
	::WideCharToMultiByte(932, 0, lpString, -1, szJapanese, 1023, NULL, NULL);

	CString strKorean;
	BOOL bTrans = pTranslator->TransJ2K(szJapanese, strKorean);

	if( bTrans )
	{
	::MultiByteToWideChar(949, 0, (LPCSTR)strKorean, -1, wszNewString, 1023 );
	lpNewString = wszNewString;
	cbCount = wcslen(wszNewString);

	wchar_t dbg[MAX_PATH];
	swprintf(dbg, L"[ aral1 ] MyExtTextOutW : '%s'->'%s' \n", lpString, wszNewString);
	OutputDebugStringW(dbg);
	}

	}

	}

	}
	*/

	// 원래함수 호출
	BOOL bRetVal = 0;

	if( m_sTextFunc.pfnExtTextOutW )
	{
		bRetVal = m_sTextFunc.pfnExtTextOutW(hdc, X, Y, fuOptions, lprc, lpNewString, cbCount, lpDx);
	}

	return bRetVal;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
DWORD CCachedTextArgMgr::NewGetGlyphOutlineA(
	HDC hdc,             // handle to device context
	UINT uChar,          // character to query
	UINT uFormat,        // format of data to return
	LPGLYPHMETRICS lpgm, // pointer to structure for metrics
	DWORD cbBuffer,      // size of buffer for data
	LPVOID lpvBuffer,    // pointer to buffer for data
	CONST MAT2 *lpmat2   // pointer to transformation matrix structure
	)
{
	DWORD dwRetVal = 0;

	if(g_objCachedTextArgMgr.m_bRunning)
	{
		dwRetVal = g_objCachedTextArgMgr.InnerGetGlyphOutlineA(hdc, uChar, uFormat, lpgm, cbBuffer, lpvBuffer, lpmat2);
	}
	else if(g_objCachedTextArgMgr.m_sTextFunc.pfnGetGlyphOutlineA)
	{
		dwRetVal = g_objCachedTextArgMgr.m_sTextFunc.pfnGetGlyphOutlineA(hdc, uChar, uFormat, lpgm, cbBuffer, lpvBuffer, lpmat2);
	}

	return dwRetVal;
}


//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
DWORD CCachedTextArgMgr::NewGetGlyphOutlineW(
	HDC hdc,             // handle to device context
	UINT uChar,          // character to query
	UINT uFormat,        // format of data to return
	LPGLYPHMETRICS lpgm, // pointer to structure for metrics
	DWORD cbBuffer,      // size of buffer for data
	LPVOID lpvBuffer,    // pointer to buffer for data
	CONST MAT2 *lpmat2   // pointer to transformation matrix structure
	)
{
	DWORD dwRetVal = 0;

	if(g_objCachedTextArgMgr.m_bRunning)
	{
		dwRetVal = g_objCachedTextArgMgr.InnerGetGlyphOutlineW(hdc, uChar, uFormat, lpgm, cbBuffer, lpvBuffer, lpmat2);
	}
	else if(g_objCachedTextArgMgr.m_sTextFunc.pfnGetGlyphOutlineW)
	{
		dwRetVal = g_objCachedTextArgMgr.m_sTextFunc.pfnGetGlyphOutlineW(hdc, uChar, uFormat, lpgm, cbBuffer, lpvBuffer, lpmat2);
	}

	return dwRetVal;

}



//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
BOOL CCachedTextArgMgr::NewTextOutA(
									   HDC hdc,           // handle to DC
									   int nXStart,       // x-coordinate of starting position
									   int nYStart,       // y-coordinate of starting position
									   LPCSTR lpString,   // character string
									   int cbString       // number of characters
									   )
{
	BOOL bRetVal = 0;

	if(g_objCachedTextArgMgr.m_bRunning)
	{
		bRetVal = g_objCachedTextArgMgr.InnerTextOutA(hdc, nXStart, nYStart, lpString, cbString);
	}
	else if(g_objCachedTextArgMgr.m_sTextFunc.pfnTextOutA)
	{
		bRetVal = g_objCachedTextArgMgr.m_sTextFunc.pfnTextOutA(hdc, nXStart, nYStart, lpString, cbString);
	}

	return bRetVal;

}


//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
BOOL CCachedTextArgMgr::NewTextOutW(
									   HDC hdc,           // handle to DC
									   int nXStart,       // x-coordinate of starting position
									   int nYStart,       // y-coordinate of starting position
									   LPCWSTR lpString,   // character string
									   int cbString       // number of characters
									   )
{
	BOOL bRetVal = 0;

	if(g_objCachedTextArgMgr.m_bRunning)
	{
		bRetVal = g_objCachedTextArgMgr.InnerTextOutW(hdc, nXStart, nYStart, lpString, cbString);
	}
	else if(g_objCachedTextArgMgr.m_sTextFunc.pfnTextOutW)
	{
		bRetVal = g_objCachedTextArgMgr.m_sTextFunc.pfnTextOutW(hdc, nXStart, nYStart, lpString, cbString);
	}

	return bRetVal;

}



//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
BOOL CCachedTextArgMgr::NewExtTextOutA(
	HDC hdc,          // handle to DC
	int X,            // x-coordinate of reference point
	int Y,            // y-coordinate of reference point
	UINT fuOptions,   // text-output options
	CONST RECT* lprc, // optional dimensions
	LPCSTR lpString, // string
	UINT cbCount,     // number of characters in string
	CONST INT* lpDx   // array of spacing values
	)
{
	BOOL bRetVal = 0;

	if(g_objCachedTextArgMgr.m_bRunning)
	{
		bRetVal = g_objCachedTextArgMgr.InnerExtTextOutA(hdc, X, Y, fuOptions, lprc, lpString, cbCount, lpDx);
	}
	else if(g_objCachedTextArgMgr.m_sTextFunc.pfnExtTextOutA)
	{
		bRetVal = g_objCachedTextArgMgr.m_sTextFunc.pfnExtTextOutA(hdc, X, Y, fuOptions, lprc, lpString, cbCount, lpDx);
	}

	return bRetVal;

}



//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
BOOL CCachedTextArgMgr::NewExtTextOutW(
	HDC hdc,          // handle to DC
	int X,            // x-coordinate of reference point
	int Y,            // y-coordinate of reference point
	UINT fuOptions,   // text-output options
	CONST RECT* lprc, // optional dimensions
	LPCWSTR lpString, // string
	UINT cbCount,     // number of characters in string
	CONST INT* lpDx   // array of spacing values
	)
{

	BOOL bRetVal = 0;

	if(g_objCachedTextArgMgr.m_bRunning)
	{
		bRetVal = g_objCachedTextArgMgr.InnerExtTextOutW(hdc, X, Y, fuOptions, lprc, lpString, cbCount, lpDx);
	}
	else if(g_objCachedTextArgMgr.m_sTextFunc.pfnExtTextOutW)
	{
		bRetVal = g_objCachedTextArgMgr.m_sTextFunc.pfnExtTextOutW(hdc, X, Y, fuOptions, lprc, lpString, cbCount, lpDx);
	}

	return bRetVal;

}

BOOL CCachedTextArgMgr::Start()
{
	m_bRunning = TRUE;
	return TRUE;
}

BOOL CCachedTextArgMgr::Stop()
{
	m_bRunning = FALSE;
	return TRUE;
}

BOOL CCachedTextArgMgr::Option()
{
	if(m_hContainerWnd && IsWindow(m_hContainerWnd))
	{
		::MessageBox( m_hContainerWnd, "현재 지원되지 않는 기능입니다.", "Cached Plugin", MB_OK );
	}
	return TRUE;
}

void CCachedTextArgMgr::HookAllReservedPoints()
{
	for(CReservedHooks::iterator iter = m_setReservedHooks.begin();
		iter != m_setReservedHooks.end();
		iter++)
	{

		PRESERVED_HOOK_POINT pRHP = (*iter);
		
		// 유니코드인 경우
		if(pRHP->bWideChar)
		{

			BOOL bHooked = m_sContainerFunc.pfnHookCodePointEx( (LPVOID)pRHP->pHookPoint, ModifyValueW,0 );

			if( bHooked )
			{
				m_mapArgInfoW.insert( CArgInfo(pRHP->pHookPoint, pRHP->nArgDist) );		
				TRACE("[ aral1 ] Function 0x%p(+%d) was hooked as unicode text \n", pRHP->pHookPoint, pRHP->nArgDist);
			}

		}
		// 멀티바이트인 경우
		else
		{

			BOOL bHooked = m_sContainerFunc.pfnHookCodePointEx( (LPVOID)pRHP->pHookPoint, ModifyValueA,0 );

			if( bHooked )
			{
				m_mapArgInfoA.insert( CArgInfo(pRHP->pHookPoint, pRHP->nArgDist) );		
				TRACE("[ aral1 ] Function 0x%p(+%d) was hooked as multibyte text \n", pRHP->pHookPoint, pRHP->nArgDist);
			}

		}

		delete pRHP;
	}

	m_setReservedHooks.clear();


}