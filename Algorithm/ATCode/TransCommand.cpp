#include "StdAfx.h"

#include "TransCommand.h"
#include "OptionMgr.h"
#include "CharacterMapper.h"
#include "TransScriptParser.h"
#include "ATCodeMgr.h"

#include "Debug.h"

#pragma warning(disable:4996)

//////////////////////////////////////////////////////////////////////////
//
// 기본 클래스 메소드
//
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// 옵션에 따라 그에 맞는 클래스 인스턴스를 생성합니다.
//
CTransCommand *CTransCommand::CreateTransCommand(CString &strArgScript, COptionNode *pNode)
{
	CTransCommand * pCmd = NULL;

	// 번역 방식 결정
	int cnt3 = pNode->GetChildCount();

	for(int k=1; k<cnt3; k++)
	{
		COptionNode* pNode3 = pNode->GetChild(k);
		CString strTransOption = pNode3->GetValue().MakeUpper();

		// 번역방식
		if(strTransOption == _T("NOP"))
		{
			pCmd = new CTransCommandNOP();
			NOTIFY_DEBUG_MESSAGE(_T("CreateTransCommand: NOP created\n"));
			break;
		}
		else if(strTransOption == _T("PTRCHEAT"))
		{
			pCmd = new CTransCommandPTRCHEAT();

			// 스페셜 옵션 분석
			int cnt4 = pNode3->GetChildCount();
			for (int i=0; i<cnt4; i++)
			{
				COptionNode* pNode4 = pNode3->GetChild(i);
				pCmd->SetSpecialOption(pNode4->GetValue().MakeUpper(), pNode4);
			}
			NOTIFY_DEBUG_MESSAGE(_T("CreateTransCommand: PTRCHEAT created\n"));
			break;
		}
		else if(strTransOption == _T("OVERWRITE"))
		{
			pCmd = new CTransCommandOVERWRITE();
			NOTIFY_DEBUG_MESSAGE(_T("CreateTransCommand: OVERWRITE created\n"));

			// 스페셜 옵션 분석
			int cnt4 = pNode3->GetChildCount();
			for (int i=0; i<cnt4; i++)
			{
				COptionNode* pNode4 = pNode3->GetChild(i);
				pCmd->SetSpecialOption(pNode4->GetValue().MakeUpper(), pNode4);
			}
			break;
		}
		else if(strTransOption == _T("SOW"))
		{
			pCmd = new CTransCommandSOW();
			NOTIFY_DEBUG_MESSAGE(_T("CreateTransCommand: SOW created\n"));

			// 스페셜 옵션 분석
			int cnt4 = pNode3->GetChildCount();
			for (int i=0; i<cnt4; i++)
			{
				COptionNode* pNode4 = pNode3->GetChild(i);
				pCmd->SetSpecialOption(pNode4->GetValue().MakeUpper(), pNode4);
			}
			break;
		}
		else if(strTransOption == _T("SMSTR"))
		{
			pCmd = new CTransCommandSMSTR();
			NOTIFY_DEBUG_MESSAGE(_T("CreateTransCommand: SMSTR created\n"));

			// 스페셜 옵션 분석
			int cnt4 = pNode3->GetChildCount();
			for (int i=0; i<cnt4; i++)
			{
				COptionNode* pNode4 = pNode3->GetChild(i);
				pCmd->SetSpecialOption(pNode4->GetValue().MakeUpper(), pNode4);
			}
			break;
		}
		
	}

	if (!pCmd) 
	{
		pCmd = new CTransCommandNOP();
		NOTIFY_DEBUG_MESSAGE(_T("CreateTransCommand: (default) NOP created\n"));
	}

	pCmd->SetArgScript(strArgScript);

	// 번역 옵션들 수집
	for(int k=1; k<cnt3; k++)
	{
		COptionNode* pNode3 = pNode->GetChild(k);
		pCmd->SetTransOption(pNode3->GetValue().MakeUpper(), pNode3);
	}

	return pCmd;
}

//////////////////////////////////////////////////////////////////////////
// 옵션을 분석, 저장합니다.
//
void CTransCommand::SetTransOption(CString strTransOption, COptionNode *pNode)
{
	// SCRTRANS 옵션
	if(strTransOption == _T("SCRTRANS"))
	{
		m_bTrans=TRUE;
		NOTIFY_DEBUG_MESSAGE(_T("SetTransOption: SCRTRANS\n"));
	}

	// 멀티바이트 / 유니코드 지정
	else if(strTransOption == _T("ANSI"))
	{
		m_bUnicode=FALSE;
		m_bUnicode8=FALSE;
		NOTIFY_DEBUG_MESSAGE(_T("SetTransOption: ANSI\n"));
	}
	else if(strTransOption == _T("UNICODE"))
	{
		m_bUnicode=TRUE;
		NOTIFY_DEBUG_MESSAGE(_T("SetTransOption: UNICODE\n"));
	}
	else if(strTransOption == _T("UNICODE8"))
	{
		m_bUnicode=TRUE;
		m_bUnicode8=TRUE;
		NOTIFY_DEBUG_MESSAGE(_T("SetTransOption: UNICODE8\n"));
	}

	// CLIPKOR 옵션
	else if(strTransOption == _T("CLIPKOR"))
	{
		m_bClipKor=TRUE;
		NOTIFY_DEBUG_MESSAGE(_T("SetTransOption: CLIPKOR\n"));
	}

	// CLIPJPN 옵션
	else if(strTransOption == _T("CLIPJPN"))
	{
		m_bClipJpn=TRUE;
		NOTIFY_DEBUG_MESSAGE(_T("SetTransOption: CLIPJPN\n"));
	}

	// REMOVESPACE 옵션
	else if(strTransOption == _T("REMOVESPACE"))
	{
		m_bRemoveSpace=TRUE;
		NOTIFY_DEBUG_MESSAGE(_T("SetTransOption: REMOVESPACE\n"));
	}

	// TWOBYTE 옵션
	else if(strTransOption == _T("TWOBYTE"))
	{
		m_bTwoByte=TRUE;
		NOTIFY_DEBUG_MESSAGE(_T("SetTransOption: TWOBYTE\n"));
	}

	// ONEBYTE 옵션
	else if(strTransOption == _T("ONEBYTE"))
	{
		m_bOneByte=TRUE;
		NOTIFY_DEBUG_MESSAGE(_T("SetTransOption: ONEBYTE\n"));
	}

	// SAFE 옵션
	else if(strTransOption == _T("SAFE"))
	{
		m_bSafe=TRUE;
		NOTIFY_DEBUG_MESSAGE(_T("SetTransOption: SAFE\n"));
	}

	// ADDNULL 옵션
	else if(strTransOption == _T("ADDNULL"))
	{
		m_bAddNull=TRUE;
		NOTIFY_DEBUG_MESSAGE(_T("SetTransOption: ADDNULL\n"));
	}

	// LEN 옵션
	else if(strTransOption == _T("LEN"))
	{
		TCHAR *pszRegs[] = {_T("EAX"), _T("EBX"), _T("ECX"), _T("EDX"), _T("ESI"), _T("EDI"), _T("EBP"), _T("ESP"), NULL};
		CString strLenPos = pNode->GetChild(0)->GetValue().MakeUpper();

		int i=0;
		bool bIsExpression = false;
		while(pszRegs[i])
		{
			if (strLenPos.Find(pszRegs[i]) >= 0)
			{
				// 레지스터 변수가 있음 : 수식
				bIsExpression = true;
				break;
			}
			i++;
		}

		if (bIsExpression)
		{
			m_strLenPos = pNode->GetChild(0)->GetValue();
			NOTIFY_DEBUG_MESSAGE(_T("SetTransOption: LEN(%s)\n"), m_strLenPos);
		}
		else
		{
			// 수식이 아님
			bool bIsMinus=false;
			i=0;
/*
			if(strLenPos[0] == _T('-'))
			{
				bIsMinus = true;
				i++;
			}
*/			
			for(; i<strLenPos.GetLength(); i++)
			{
				// 숫자 시작지점을 찾는다
				if ( strLenPos[i] != _T('+') && strLenPos[i] != _T('X') && strLenPos[i] != _T('0') )
					break;
			}
			
			_stscanf(strLenPos.Mid(i), _T("%x"), &m_nLenPos);

			NOTIFY_DEBUG_MESSAGE(_T("SetTransOption: LEN(%d)\n"), m_nLenPos);
		}
		m_bAdjustLen = TRUE;
	}

	// LENEND 옵션
	else if(strTransOption == _T("LENEND"))
	{
		m_bLenEnd=TRUE;
		NOTIFY_DEBUG_MESSAGE(_T("SetTransOption: LENEND\n"));
	}

	//KIRINAME 옵션
	else if(strTransOption == _T("KIRINAME"))
	{
		m_bKiriName=TRUE;
		NOTIFY_DEBUG_MESSAGE(_T("SetTransOption: KIRINAME\n"));
	}

	//PTRBACKUP 옵션
	else if(strTransOption == _T("PTRBACKUP"))
	{
		m_bPtrBack=TRUE;
		NOTIFY_DEBUG_MESSAGE(_T("SetTransOption: PTRBACKUP\n"));
	}

	//DUPLICATED 옵션
	else if(strTransOption == _T("DUPLICATED"))
	{
		m_bDuplicated=TRUE;
		NOTIFY_DEBUG_MESSAGE(_T("SetTransOption: DUPLICATED\n"));
	}
}

//////////////////////////////////////////////////////////////////////////
// 실제 번역할 원문의 위치를 반환합니다.
//
void *CTransCommand::GetTextPoint(CTransScriptParser *pParser, void *pBackUp, BOOL bPtrBack, void *pBackUpLength)
{
	// 이전 포인터 교체한것 복구하기
	if (bPtrBack)
	{
		// SOW 값 복원
		RestoreBackup();

		if(BackUpTextPoint(pBackUp))
			// 길이값 복원
			if (m_bAdjustLen)
				BackUpLength(pBackUpLength);
	}

	if (pParser == NULL) return NULL;

	NOTIFY_DEBUG_MESSAGE(_T("GetTextPoint: called\n"));

	void * pArgText = NULL;

	// ArgScript ( [ESI] 같은..) 가 가리키는 위치 얻기
	int nType;
	int* pRetVal = (int*)pParser->GetValue(GetArgScript(), &nType);
	if(pRetVal && 1 == nType)
	{
		pArgText = *(LPVOID *)pRetVal;
		delete pRetVal;
	}

	// 얻어온 위치의 유효성 검사
	if(pArgText == NULL || IsBadReadPtr(pArgText, sizeof(LPVOID))) 
	{
		return NULL;
	}

	if (m_bUnicode)
	{
		if (*(LPWSTR)pArgText == L'\0') 
		{
			return NULL;
		}

	}
	else // MBCS
	{
		if(*(LPSTR)pArgText == '\0') 
		{
			return NULL;
		}
	}

	m_pArgText=pArgText;
	NOTIFY_DEBUG_MESSAGE(_T("GetTextPoint: pArgText=%p\n"), pArgText);

	return pArgText;
}

//////////////////////////////////////////////////////////////////////////
// 이전 백업한 포인터를 복원합니다.
//
bool CTransCommand::BackUpTextPoint(void *pBackUp, int pMovSize)
{
	bool bTmp = false;
	if( *(&(*(DWORD*)pBackUp)+3) != 0 &&
		!IsBadReadPtr(((DWORD*)(*(&(*(DWORD*)pBackUp)))), pMovSize * 4) )
	{
		bTmp = true;
		if(*(&(*(DWORD*)pBackUp)+3) == 1)
		{
			if(*(&(*(DWORD*)(*(&(*(DWORD*)pBackUp))))+0) == *(&(*(DWORD*)pBackUp)+2))
				memmove((&(*(DWORD*)(*(&(*(DWORD*)pBackUp))))), (&(*(DWORD*)pBackUp)+4), pMovSize*4);
			else if(*(&(*(DWORD*)(*(&(*(DWORD*)pBackUp))))+0) == *(&(*(DWORD*)pBackUp)+1))
				memmove((&(*(DWORD*)(*(&(*(DWORD*)pBackUp))))), (&(*(DWORD*)pBackUp)+4), pMovSize*4);
		}
		else if(*(&(*(DWORD*)pBackUp)+3) == 2)
		{
			DWORD dwTest = 0, dwLen = *(&(*(DWORD*)pBackUp)+1) - 2;
			while(dwTest < dwLen){
				if(*(&(*(BYTE*)(*(&(*(DWORD*)pBackUp))))+dwTest)!=*(&(*(BYTE*)(*(&(*(DWORD*)pBackUp)+2)))+dwTest)){
					bTmp = false;
					break;
				}
				dwTest++;
			}
			if( bTmp )
				memmove((&(*(DWORD*)(*(&(*(DWORD*)pBackUp))))), (&(*(DWORD*)pBackUp)+4), *(&(*(DWORD*)pBackUp)+1));
		}
	}
	*(&(*(DWORD*)pBackUp)+1) = 0;
	*(&(*(DWORD*)pBackUp)+3) = 0;
	return bTmp;
}

//////////////////////////////////////////////////////////////////////////
// 변경된 포인터를 메모리에 기록합니다.
//
void CTransCommand::SetUpTextPoint(void *pBackUp, void *pArgText, void *ppArgText, int pMovSize, int nType)
{
	if(pMovSize == 0)
	{
		*(&(*(DWORD*)pBackUp)+3) = 0;
		return;
	}
	if(nType == 1)
	{
		*(&(*(DWORD*)pBackUp)) = (DWORD)ppArgText;
		*(&(*(DWORD*)pBackUp)+1) = (DWORD)pArgText;
		*(&(*(DWORD*)pBackUp)+2) = (DWORD)m_pTransTextBuf;
		*(&(*(DWORD*)pBackUp)+3) = nType;
		memmove((&(*(DWORD*)pBackUp)+4), ppArgText, pMovSize*4);
	}
	else if(nType == 2)
	{
		*(&(*(DWORD*)pBackUp)) = (DWORD)ppArgText;
		*(&(*(DWORD*)pBackUp)+1) = pMovSize;
		*(&(*(DWORD*)pBackUp)+2) = (DWORD)m_pTransTextBuf;
		*(&(*(DWORD*)pBackUp)+3) = nType;
		memmove((&(*(DWORD*)pBackUp)+4), ppArgText, pMovSize);
	}
}

//////////////////////////////////////////////////////////////////////////
// 받아온 문자열이 읽을 수 있는 문자열인지 확인합니다.
//
BOOL CTransCommand::IsValidTextPoint(void *pArgText, long nSize)
{
	BOOL bRet = TRUE;
	CCharacterMapper *pcCharMap = NULL;

	if (CATCodeMgr::GetInstance()->m_nEncodeKorean)
	{
		if (CATCodeMgr::GetInstance()->m_nEncodeKorean == 2)
			pcCharMap = new CCharacterMapper2;
		else
			pcCharMap = new CCharacterMapper;
	}

	NOTIFY_DEBUG_MESSAGE(_T("IsValidTextPoint: called, pArgText=%p, *pArgText=%08X\n"),pArgText, *(DWORD *) pArgText);

	if (m_bUnicode)
	{
		if( IsBadStringPtrW((LPCWSTR)pArgText, nSize) )
			bRet = FALSE;

	}
	else
	{
		if ( IsBadStringPtrA((LPCSTR)pArgText, nSize) )
			bRet = FALSE;

		// 잘못된 텍스트인가?
		if(m_bSafe && pcCharMap->IsShiftJISText((LPCSTR)pArgText) == FALSE)
		{
			delete pcCharMap;
			TRACE(_T("[aral1] ◆Bad Text \n"));
			throw -4; //_T("일본어 텍스트가 아닙니다.");
		}


	}
	
	NOTIFY_DEBUG_MESSAGE(_T("IsValidTextPoint: return %s\n"), (bRet?_T("TRUE"):_T("FALSE")) );
	if (pcCharMap) delete pcCharMap;
	return bRet;
}

//////////////////////////////////////////////////////////////////////////
// 혹시 같은 문자열을 다시 번역하려는지 확인합니다.
//
BOOL CTransCommand::IsDuplicateText(void *pArgText)
{
	NOTIFY_DEBUG_MESSAGE(_T("IsDuplicateText: called\n"));

	BOOL bRet=FALSE;
	if (m_bUnicode)
	{
		LPWSTR wszText = (LPWSTR) pArgText;

		// 최근 번역된 문장인가
		wchar_t wszPureText[MAX_TEXT_LENGTH];

		// UTF-8 -> UTF-16 변환
		if( m_bUnicode8 )
		{
			int len = MyMultiByteToWideChar(CP_UTF8, 0, (LPCSTR) pArgText, -1, NULL, 0);
			MyMultiByteToWideChar(CP_UTF8, 0, (LPCSTR) pArgText, -1, wszPureText, len);
		}
		else wcscpy(wszPureText, wszText);
		size_t wlen = wcslen(wszPureText);

		// 문장 끝의 공백 삭제
		while(wlen>0 && wszPureText[wlen-1] == L' ')
		{
			wszPureText[wlen-1] = L'\0';
			wlen--;
		}

		// 중복 체크
		if( wcslen((LPCWSTR)m_pTransTextBuf) > 0 && wcsstr((LPCWSTR)m_pTransTextBuf, wszPureText) )
		{
		//	throw -1; //_T("같은 위치를 중복 번역하려 합니다.");
			bRet = TRUE;
		}
		// 최근 처리된 일본어가 아닌 새로운 문장이라면		
		else if( wcscmp(wszText, (LPCWSTR)m_pOrigTextBuf) )
			bRet = FALSE;	// 번역 필요
		else
			bRet = TRUE;	// 번역 불필요
	}
	else
	{
		LPSTR szText = (LPSTR) pArgText;

		// 최근 번역된 문장인가
		size_t nCmpCnt = strlen(szText);

		// 문장 끝의 공백 무시
		while(nCmpCnt > 0 && szText[nCmpCnt-1] == ' ') nCmpCnt--;

		// 중복 체크
		if( strlen((LPCSTR)m_pTransTextBuf) > 0 && strncmp(szText, (LPCSTR)m_pTransTextBuf, nCmpCnt) == 0 )
		{
		//	throw -1; //_T("같은 위치를 중복 번역하려 합니다.");
			bRet = TRUE;
		}
		// 최근 처리된 일본어가 아닌 새로운 문장이라면
		else if( strcmp(szText, (LPCSTR)m_pOrigTextBuf) )
			bRet = FALSE;	// 번역 필요
		// 같은 일본어 문장이라도 이전것과 위치가 다르면 (예: 같은 단어가 복수로 다른 위치에 있을 경우)
		else if ( pArgText != m_pPrevArgText)
			bRet = FALSE;
		else
			bRet = TRUE;	// 번역 불필요
	}

	NOTIFY_DEBUG_MESSAGE(_T("IsDuplicateText: return %s\n"), (bRet?_T("TRUE"):_T("FALSE")) );
	return bRet;
}

//////////////////////////////////////////////////////////////////////////
// 문자열 길이를 번역된 문자열의 길이로 변경합니다.
//
BOOL CTransCommand::ChangeLength(void *pOrigBasePtr, CTransScriptParser *pParser, void *pNewBasePtr, void *pBackUp)
{
	if (!m_bAdjustLen) return FALSE;

	NOTIFY_DEBUG_MESSAGE(_T("ChangeLength: called\n"));

	// pNewBasePtr 가 생략되면 pOrigBasePtr 와 동일 포인터
	if (pNewBasePtr == NULL)
		pNewBasePtr = pOrigBasePtr;

	int *pnLen=NULL;

	// 길이가 저장된 위치를 찾는다
	if (m_nLenPos)
	{
		// m_nLenPos 가 있다면 pOrigBasePtr와의 상대위치로 계산
		pnLen = (int *)((DWORD)pOrigBasePtr + m_nLenPos);
	}
	else if (!m_strLenPos.IsEmpty())
	{
		// m_strLenPos 가 있다면 그대로 파서에 넣어서 계산
		int nType;
		int *pRetVal = (int *)pParser->GetValue(m_strLenPos, &nType);

		if (pRetVal && 1 == nType)
		{
			pnLen = *(int **)pRetVal;
			delete pRetVal;

		}
		else
			return FALSE;
	}
	else
		return FALSE;

	if (IsBadReadPtr(pnLen, sizeof(int *)))
	{
		NOTIFY_DEBUG_MESSAGE(_T("ChangeLength: Cannot read address %p\n"), pnLen);
		return FALSE;
	}

	// 찾은 길이가 정확한지 확인하고 몇 바이트짜리인지도 확인한다.
	int nOrigLen = (m_bUnicode ? lstrlenW((LPCWSTR)m_pOrigTextBuf) : lstrlenA((LPCSTR)m_pOrigTextBuf));
	int nTempLen = *pnLen;
	int nBytes;
	int nNullIncluded = -2;

	/*if (nOrigLen == nTempLen)
		nBytes=4;
	else if (nOrigLen == (nTempLen & 0x0FFFF))
		nBytes=2;
	else if (nOrigLen == (nTempLen & 0x0FF))
		nBytes=1;
	else
	{*/
	// NULL 문자 포함된 길이인지 한번 더 체크
	for(nNullIncluded = -2; nNullIncluded <= 2; nNullIncluded++)
	{
		int nOffset = nOrigLen + nNullIncluded;
		if (nOffset == nTempLen)
		{
			nBytes=4;
			break;
		}
		else if (nOffset == (nTempLen & 0x0FFFF))
		{
			nBytes=2;
			break;
		}
		else if (nOffset == (nTempLen & 0x0FF))
		{
			nBytes=1;
			break;
		}
	}
	//}

	NOTIFY_DEBUG_MESSAGE(_T("ChangeLength: address(%p), bytes(%d), nOrigLen=%d, nTempLen=%d, nNullIncluded=%d\n"),
			pnLen, nBytes, nOrigLen, nTempLen, nNullIncluded);

	// 이제 번역문의 길이로 바꿔넣는다.
	if (m_nLenPos)
		pnLen = (int *)((DWORD)pNewBasePtr + m_nLenPos + nNullIncluded);

	if (IsBadWritePtr(pnLen, sizeof(int)))
	{
		NOTIFY_DEBUG_MESSAGE(_T("ChangeLength: Cannot write address %p\n"), pnLen);
		return FALSE;
	}

	nTempLen = (m_bUnicode ? lstrlenW((LPCWSTR)m_pTransTextBuf) : lstrlenA((LPCSTR)m_pTransTextBuf));

	nTempLen += nNullIncluded;

	// 길이값 백업
	SetUpLength(pBackUp, pnLen, nBytes, nTempLen);

	NOTIFY_DEBUG_MESSAGE(_T("ChangeLength: success (address:%p, %d -> %d)\n"), pnLen, nOrigLen, nTempLen);

	return TRUE;
}
//////////////////////////////////////////////////////////////////////////
// 이전 백업한 문자열 길이를 복원합니다.
//
void CTransCommand::BackUpLength(void *pBackUp)
{
	if( *(&(*(DWORD*)pBackUp)+3) != 0 &&
		!IsBadReadPtr(((DWORD*)(*(&(*(DWORD*)pBackUp)))), sizeof(DWORD *)) )
	{
		if(*(&(*(DWORD*)(*(&(*(DWORD*)pBackUp)))))==*(&(*(DWORD*)pBackUp)+2))
		{
			*(&(*(DWORD*)(*(&(*(DWORD*)pBackUp)))))=*(&(*(DWORD*)pBackUp)+1);
		}
	}
	*(&(*(DWORD*)pBackUp)+3) = 0;
}

//////////////////////////////////////////////////////////////////////////
// 문자열 길이를 변경한 값을 메모리에 기록합니다.
//
void CTransCommand::SetUpLength(void *pBackUp, int *pnLen, int nBytes, int nTempLen)
{
	*(&(*(DWORD*)pBackUp)) = (DWORD)pnLen;
	*(&(*(DWORD*)pBackUp)+1) = *(&(*(DWORD*)pnLen));

	if (nBytes == 1)
		*(BYTE *)pnLen = nTempLen & 0x0FF;
	else if (nBytes == 2)
		*(WORD *)pnLen = nTempLen & 0x0FFFF;
	else
		*pnLen = nTempLen;

	*(&(*(DWORD*)pBackUp)+2) = *(&(*(DWORD*)pnLen));
	*(&(*(DWORD*)pBackUp)+3) = 1;
}

//////////////////////////////////////////////////////////////////////////
//
// PTRCHEAT 방식 클래스 메소드
//
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// 문자열 길이를 번역된 문자열의 길이로 변경합니다. (오버라이드됨)
//
BOOL CTransCommandPTRCHEAT::ChangeLength(void *pOrigBasePtr, CTransScriptParser *pParser, void *pNewBasePtr, void *pBackUp)
{
	if (!m_bAdjustLen) return FALSE;

	NOTIFY_DEBUG_MESSAGE(_T("PTRCHEAT: ChangeLength: m_pTransTextBuf(%p)\n"), m_pTransTextBuf);
	return CTransCommand::ChangeLength(pOrigBasePtr, pParser, m_pTransTextBuf, pBackUp);

}


//////////////////////////////////////////////////////////////////////////
// 번역된 문자열을 원 프로그램에 적용합니다. (오버라이드됨)
//
BOOL CTransCommandPTRCHEAT::ApplyTranslatedText(void *pArgText, PREGISTER_ENTRY pRegisters, CTransScriptParser *pParser, BOOL bIsDuplicated, void *pBackUp)
{
	if (bIsDuplicated)
	{
		if (pArgText != m_pPrevArgText)
			return FALSE;

		if (pArgText == m_pTransTextBuf)
			return FALSE;
	}

	CString strScript = m_strArgScript;
	int nLen = strScript.GetLength();

	NOTIFY_DEBUG_MESSAGE(_T("PTRCHEAT: ApplyTranslatedText: called\n"));

#if defined(_DEBUG) || defined(FORCE_DEBUG_LOG)

	if (m_bUnicode)
	{
		NOTIFY_DEBUG_MESSAGE(_T("PTRCHEAT::ApplyTranslatedText: szSrc =%s\n"), (LPCWSTR)m_pTransTextBuf);
		NOTIFY_DEBUG_MESSAGE(_T("PTRCHEAT::ApplyTranslatedText: szDest =%s\n"), (LPCWSTR)m_pOrigTextBuf);
	}
	else
	{
		WCHAR wszTemp[MAX_TEXT_LENGTH];
		MyMultiByteToWideChar(949, 0, (LPCSTR)m_pTransTextBuf, -1, wszTemp, MAX_TEXT_LENGTH);
		NOTIFY_DEBUG_MESSAGE(_T("PTRCHEAT::ApplyTranslatedText: szSrc =%s\n"), wszTemp);
		MyMultiByteToWideChar(932, 0, (LPCSTR)m_pOrigTextBuf, -1, wszTemp, MAX_TEXT_LENGTH);
		NOTIFY_DEBUG_MESSAGE(_T("PTRCHEAT::ApplyTranslatedText: szDest=%s\n"), wszTemp);
	}
#endif

	if(strScript.CompareNoCase(_T("EAX")) == 0) pRegisters->_EAX = (DWORD)(UINT_PTR)m_pTransTextBuf;
	else if(strScript.CompareNoCase(_T("EBX")) == 0) pRegisters->_EBX = (DWORD)(UINT_PTR)m_pTransTextBuf;
	else if(strScript.CompareNoCase(_T("ECX")) == 0) pRegisters->_ECX = (DWORD)(UINT_PTR)m_pTransTextBuf;
	else if(strScript.CompareNoCase(_T("EDX")) == 0) pRegisters->_EDX = (DWORD)(UINT_PTR)m_pTransTextBuf;
	else if(strScript.CompareNoCase(_T("ESI")) == 0) pRegisters->_ESI = (DWORD)(UINT_PTR)m_pTransTextBuf;
	else if(strScript.CompareNoCase(_T("EDI")) == 0) pRegisters->_EDI = (DWORD)(UINT_PTR)m_pTransTextBuf;
	else if(strScript.CompareNoCase(_T("EBP")) == 0) pRegisters->_EBP = (DWORD)(UINT_PTR)m_pTransTextBuf;
	else if(strScript.CompareNoCase(_T("ESP")) == 0) pRegisters->_ESP = (DWORD)(UINT_PTR)m_pTransTextBuf;
	else if(nLen >= 5 && _T('[') == strScript[0] && _T(']') == strScript[nLen-1])
	{
		int nType;
		int* pRetVal = (int*)pParser->GetValue(strScript.Mid(1, nLen-2), &nType);
		if(pRetVal && 1 == nType)
		{
			LPBYTE* ppArgText = *(LPBYTE**)pRetVal;
			delete pRetVal;

			if(::IsBadWritePtr(ppArgText, sizeof(LPBYTE)) == FALSE)
			{
				// 포인터를 교체하기전 미리 백업해둔다
				SetUpTextPoint(pBackUp, pArgText, ppArgText, 1);

				*ppArgText = m_pTransTextBuf;
			}

		}
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//
// OVERWRITE 방식 클래스 메소드
//
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// 옵션을 분석, 저장합니다.
//
void CTransCommandOVERWRITE::SetTransOption(CString strTransOption, COptionNode *pNode)
{
	if (strTransOption == _T("ALLSAMETEXT"))
	{
		NOTIFY_DEBUG_MESSAGE(_T("OVERWRITE: SetTransOption: ALLSAMETEXT\n"));
		m_bAllSameText=TRUE;
	}
	else
		CTransCommand::SetTransOption(strTransOption, pNode);	// global options
}

//////////////////////////////////////////////////////////////////////////
// 특별 옵션을 분석, 저장합니다.
//
void CTransCommandOVERWRITE::SetSpecialOption(CString strSpecialOption, COptionNode *pNode)
{
	if(strSpecialOption == _T("IGNORE"))
	{
		NOTIFY_DEBUG_MESSAGE(_T("OVERWRITE: SetSpecialOption: IGNORE\n"));
		m_bIgnoreBufLen=TRUE;
	}
	else
		CTransCommand::SetSpecialOption(strSpecialOption, pNode);
}

//////////////////////////////////////////////////////////////////////////
// 번역문을 원 프로그램에 덮어씌웁니다.
//
BOOL CTransCommandOVERWRITE::OverwriteTextBytes( LPVOID pDest, LPVOID pSrc, void *pBackUp )
{
	NOTIFY_DEBUG_MESSAGE(_T("OVERWRITE: OverwriteTextBytes: called\n"));

	if(m_bUnicode)
	{
		LPWSTR wszSrc = (LPWSTR)pSrc;
		UINT_PTR nSrcLen = wcslen(wszSrc);
		LPWSTR wszDest = (LPWSTR)pDest;
		UINT_PTR nDestLen = ( m_bIgnoreBufLen ? nSrcLen : wcslen(wszDest) );

		// 문자열 포인터가 잘못되었다면 리턴
		if( IsBadWritePtr(wszDest, nDestLen) || IsBadStringPtrW(wszDest, 1024*1024*1024) ) return FALSE;
		
		// 덮어쓸 길이
		size_t len = min(nDestLen, nSrcLen);

		// 메모리 백업
		SetUpTextPoint(pBackUp, pDest, wszDest, (nDestLen+1)*sizeof(wchar_t), 2);

		// 텍스트 복사
		while(len<nDestLen)
		{
			wszSrc[len] = L' ';
			len++;
		}
		wszSrc[len+1] = 0;
		memcpy(wszDest, wszSrc, (nDestLen+1)*sizeof(wchar_t));
	}
	else
	{

		LPSTR szSrc = (LPSTR)pSrc;
		UINT_PTR nSrcLen = strlen(szSrc);
		LPSTR szDest = (LPSTR)pDest;
		UINT_PTR nDestLen = ( m_bIgnoreBufLen ? nSrcLen : strlen(szDest) );


#if defined(_DEBUG) || defined(FORCE_DEBUG_LOG)
		WCHAR wszTemp[MAX_TEXT_LENGTH];
		MyMultiByteToWideChar(949, 0, szSrc, -1, wszTemp, MAX_TEXT_LENGTH);
		NOTIFY_DEBUG_MESSAGE(_T("OVERWRITE: OverwriteTextBytes: szSrc =%s\n"), wszTemp);
		MyMultiByteToWideChar(932, 0, szDest, -1, wszTemp, MAX_TEXT_LENGTH);
		NOTIFY_DEBUG_MESSAGE(_T("OVERWRITE: OverwriteTextBytes: szDest=%s\n"), wszTemp);
#endif


		// 문자열 포인터가 잘못되었다면 리턴
		if( IsBadWritePtr(szDest, nDestLen) || IsBadStringPtrA(szDest, 1024*1024*1024) ) return FALSE;

		// 덮어쓸 길이
		size_t len = 0;
		while(len<nDestLen && len<nSrcLen)
		{
			size_t addval = 1;
			if( (BYTE)0x80 <= (BYTE)szSrc[len] ) addval = 2;

			if( len + addval > nDestLen ) break;

			len += addval;
		}

		// 메모리 백업
		SetUpTextPoint(pBackUp, pDest, szDest, nDestLen+1, 2);

		// 텍스트 복사
		while(len<nDestLen)
		{
			szSrc[len] = ' ';
			len++;
		}
		memset(szDest, 0, (nDestLen+1));
		memcpy(szDest, szSrc, (nDestLen));
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// 스택 내의 모든 동일한 원문을 찾아 번역문으로 덮어씌웁니다.
// (ANSI 버전)
//
BOOL CTransCommandOVERWRITE::SearchTextA(UINT_PTR ptrBegin, LPCSTR cszText, list<LPVOID>* pTextList)
{
	BOOL bRetVal = FALSE;

	size_t dist = 0;
	size_t nOrigLen = strlen(cszText);

	while( IsBadReadPtr((void*)(ptrBegin+dist), sizeof(void*)) == FALSE )
	{
		LPSTR* ppText = (LPSTR*)(ptrBegin+dist);

		// 일치한다면
		if( IsBadStringPtrA(*ppText, 1024*1024*1024)==FALSE 
			&& strlen(*ppText) == nOrigLen
			&& strcmp(*ppText, cszText) == 0 )
		{
			// 목록에 추가
			pTextList->push_back(*ppText);
			bRetVal = TRUE;
		}

		dist += sizeof(void*);
	}

	TRACE(_T(" [ aral1 ] SearchTextA 찾은거리:0x%p~0x%p (%d bytes) \n"), ptrBegin, ptrBegin+dist, dist);

	return bRetVal;
}

//////////////////////////////////////////////////////////////////////////
// 스택 내의 모든 동일한 원문을 찾아 번역문으로 덮어씌웁니다.
// (UNICODE 버전)
//
BOOL CTransCommandOVERWRITE::SearchTextW(UINT_PTR ptrBegin, LPCWSTR cwszText, list<LPVOID>* pTextList)
{
	BOOL bRetVal = FALSE;

	size_t dist = 0;
	size_t nOrigLen = wcslen(cwszText);

	while( IsBadReadPtr((void*)(ptrBegin+dist), sizeof(void*)) == FALSE )
	{
		LPWSTR* ppText = (LPWSTR*)(ptrBegin+dist);

		// 일치한다면
		if( IsBadStringPtrW(*ppText, 1024*1024)==FALSE 
			&& wcslen(*ppText) == nOrigLen
			&& wcscmp(*ppText, cwszText) == 0 )
		{
			// 목록에 추가
			pTextList->push_back(*ppText);
			bRetVal = TRUE;
		}

		dist += sizeof(void*);

	}	

	TRACE(_T(" [ aral1 ] SearchTextW 찾은거리:0x%p~0x%p (%d bytes) \n"), ptrBegin, ptrBegin+dist, dist);

	return bRetVal;
}

//////////////////////////////////////////////////////////////////////////
//  번역된 문자열을 원 프로그램에 적용합니다. (오버라이드됨)
//
BOOL CTransCommandOVERWRITE::ApplyTranslatedText(void *pArgText, PREGISTER_ENTRY pRegisters, CTransScriptParser *pParser, BOOL bIsDuplicated, void *pBackUp)
{
	if (bIsDuplicated)
	{
		if (m_bUnicode || m_bUnicode8)
		{
			if (*(WCHAR *)m_pTransTextBuf == L'\0')
				return FALSE;
		}
		else
		{
			if (*(char *)m_pTransTextBuf == '\0')
				return FALSE;
		}
		// 번역 버퍼에 내용이 있으면 그대로 사용한다.
	}

	NOTIFY_DEBUG_MESSAGE(_T("OVERWRITE: ApplyTranslatedText: called\n"));

	list<LPVOID> listTexts;
	listTexts.push_back(pArgText);
	

	// 번역해야할 위치들 저장
	if(m_bAllSameText)
	{
		if(m_bUnicode)
		{
			SearchTextW((UINT_PTR)pRegisters->_EBP, (LPCWSTR)pArgText, &listTexts);
		}
		else
		{
			SearchTextA((UINT_PTR)pRegisters->_EBP, (LPCSTR)pArgText, &listTexts);
		}
	}


	// 위치들에 번역된 텍스트 Overwrite
	for(list<LPVOID>::iterator iter = listTexts.begin();
		iter != listTexts.end();
		iter++)
	{
		LPVOID pDest = (*iter);
		OverwriteTextBytes(pDest, m_pTransTextBuf, pBackUp);
	}
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//
// SOW 방식 클래스 메소드
//
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// SOW 방식의 메모리를 백업하고 번역문을 덮어씌웁니다
//
void CTransCommandSOW::DoBackupAndOverwrite(const PBYTE pBackupPoint, UINT nBackupSize)
{
	// 백업 복구가 되어있지 않으면 복구
	if (m_pBackupPoint)
		RestoreBackup();

	NOTIFY_DEBUG_MESSAGE(_T("SOW: DoBackupAndOverwrite: called\n"));

	m_pBackupBuffer = new BYTE [nBackupSize];
	m_nBackupSize = nBackupSize;
	m_pBackupPoint = pBackupPoint;

	// 백업
	CopyMemory(m_pBackupBuffer, m_pBackupPoint, m_nBackupSize);

	if (m_nLenBytes)
	{
		int nLen = (m_bUnicode ? lstrlenW((LPCWSTR)m_pTransTextBuf) : lstrlenA((LPCSTR)m_pTransTextBuf));
		if (m_bIsNullIncluded)
			nLen++;

		try
		{
		// 길이 위치가 상대위치면 위치를 구하고
		if (!m_pnBackupLenPoint)
			m_pnBackupLenPoint = (int *)(pBackupPoint + m_nLenPos);

		NOTIFY_DEBUG_MESSAGE(_T("SOW: DoBackupAndOverwrite: m_pnBackupLenPoint=%p\n"), m_pnBackupLenPoint);

		if (IsBadReadPtr(m_pnBackupLenPoint, sizeof(int *)))
		{
			NOTIFY_DEBUG_MESSAGE(_T("SOW: DoBackupAndOverwrite: BadReadPtr m_pnBackupLenPoint\n"));
			throw;
		}

		if (IsBadWritePtr(m_pnBackupLenPoint, sizeof(int)))
		{
			NOTIFY_DEBUG_MESSAGE(_T("SOW: DoBackupAndOverwrite: BadWritePtr m_pnBackupLenPoint\n"));

		}

		// 길이가 들어갈 위치의 데이터 백업
		m_nBackupLenPoint = *m_pnBackupLenPoint;

		// 길이 덮어쓰기
		if (m_nLenBytes == 1)
			*(BYTE *)m_pnBackupLenPoint = nLen & 0x0FF;
		else if (m_nLenBytes == 2)
			*(WORD *)m_pnBackupLenPoint = nLen & 0x0FFFF;
		else
			*m_pnBackupLenPoint = nLen;

		NOTIFY_DEBUG_MESSAGE(_T("SOW: DoBackupAndOverwrite: Len changed to %d\n"), nLen);

		// 작업 완료를 표시하기 위해 m_nLenBytes 를 0 으로 맞춤
		m_nLenBytes = 0;

		}
		catch (...)
		{
			m_pnBackupLenPoint = NULL;
			m_nLenBytes = 0;
			m_nBackupLenPoint = 0;
		}

	}

	// 덮어쓰기
	CopyMemory(m_pBackupPoint, m_pTransTextBuf, m_nBackupSize);

}

//////////////////////////////////////////////////////////////////////////
// SOW 방식의 백업 데이타를 복구합니다
//
void CTransCommandSOW::RestoreBackup()
{
	NOTIFY_DEBUG_MESSAGE(_T("SOW: RestoreBackup: called\n"));

	if (m_pBackupPoint)
	{
		// 원 메모리가 쓰기가능할 때만 백업 복구
		if (!IsBadWritePtr(m_pBackupPoint, m_nBackupSize))
		{
			bool bIsMismatch = false;
			UINT i;

			// 원 메모리가 백업 이후 바뀌었는지 체크
			for(i=0; i<m_nBackupSize; i++)
			{
				if (*(m_pBackupPoint+i) != *(m_pTransTextBuf+i))
				{
					bIsMismatch=true;
					break;
				}
			}

			// 바뀌지 않았으면 복구
			if (!bIsMismatch)
			{
				CopyMemory(m_pBackupPoint, m_pBackupBuffer, m_nBackupSize);

				// m_nLenBytes == 0 인데 m_pnBackupLenPoint 가 존재하면 문자길이 복구
				if (m_nLenBytes == 0 && m_pnBackupLenPoint)
				{
					*m_pnBackupLenPoint = m_nBackupLenPoint;

					m_pnBackupLenPoint = NULL;
					m_nBackupLenPoint = 0;
					NOTIFY_DEBUG_MESSAGE(_T("SOW: RestoreBackup: Len restored\n"));
				}

			}
		}
		delete[] m_pBackupBuffer;
		m_pBackupPoint=NULL;

	}

}

//////////////////////////////////////////////////////////////////////////
// 문자열 길이를 번역된 문자열의 길이로 변경합니다. (오버라이드됨)
//
BOOL CTransCommandSOW::ChangeLength(void *pOrigBasePtr, CTransScriptParser *pParser, void *pNewBasePtr, void *pBackUp)
{
	// SOW 방식에서는 시작 포인터가 변경되는 방식이기 때문에 여기서 변경을 하지 않고
	// 문자열 길이 확인만 해 둔 후 실제 변경은 DoBackupAndOverwrite 에서 실행한다.

	if (!m_bAdjustLen) return FALSE;

	NOTIFY_DEBUG_MESSAGE(_T("SOW: ChangeLength: called\n"));

	int *pnLen=NULL;

	// 길이가 저장된 위치를 찾는다
	if (m_nLenPos)
	{
		// m_nLenPos 가 있다면 pOrigBasePtr와의 상대위치로 계산
		pnLen = (int *)((DWORD)pOrigBasePtr + m_nLenPos);
	}
	else if (!m_strLenPos.IsEmpty())
	{
		// m_strLenPos 가 있다면 그대로 파서에 넣어서 계산
		int nType;
		int *pRetVal = (int *)pParser->GetValue(m_strLenPos, &nType);

		if (pRetVal && 1 == nType)
		{
			pnLen = *(int **)pRetVal;
			delete pRetVal;

		}
		else
		{
			m_nLenBytes=0;
			return FALSE;
		}
	}
	else
	{
		m_nLenBytes=0;
		return FALSE;
	}

	if (IsBadReadPtr(pnLen, sizeof(int *)))
	{
		NOTIFY_DEBUG_MESSAGE(_T("ChangeLength: Cannot read address %p\n"), pnLen);
		m_nLenBytes=0;
		return FALSE;
	}

	// 찾은 길이가 정확한지 확인하고 몇 바이트짜리인지도 확인한다.
	int nOrigLen = (m_bUnicode ? lstrlenW((LPCWSTR)m_pOrigTextBuf) : lstrlenA((LPCSTR)m_pOrigTextBuf));
	int nTempLen = *pnLen;
	int nBytes;
	bool bIsNullIncluded=false;

	if (nOrigLen == nTempLen)
		nBytes=4;
	else if (nOrigLen == (nTempLen & 0x0FFFF))
		nBytes=2;
	else if (nOrigLen == (nTempLen & 0x0FF))
		nBytes=1;
	else
	{
		// NULL 문자 포함된 길이인지 한번 더 체크
		nOrigLen++;
		if (nOrigLen == nTempLen)
		{
			bIsNullIncluded=true;
			nBytes=4;
		}
		else if (nOrigLen == (nTempLen & 0x0FFFF))
		{
			bIsNullIncluded=true;
			nBytes=2;
		}
		else if (nOrigLen == (nTempLen & 0x0FF))
		{
			bIsNullIncluded=true;
			nBytes=1;
		}
		else
		{
			m_nLenBytes=0;
			return FALSE;
		}
	}

	// 필요한 데이타는 nBytes, bIsNullIncluded.
	m_nLenBytes = nBytes;
	m_bIsNullIncluded = bIsNullIncluded;

	// 만약 LEN(수식) 인 경우 문자열 길이 위치도 필요.
	if (!m_strLenPos.IsEmpty())
		m_pnBackupLenPoint = pnLen;

	NOTIFY_DEBUG_MESSAGE(_T("SOW: ChangeLength: address(%p), bytes(%d), nOrigLen=%d, nTempLen=%d, bIsNullIncluded=%s\n"),
			pnLen, nBytes, nOrigLen, nTempLen, (bIsNullIncluded?_T("TRUE"):_T("FALSE")));

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//  번역된 문자열을 원 프로그램에 적용합니다. (오버라이드됨)
//
BOOL CTransCommandSOW::ApplyTranslatedText(void *pArgText, PREGISTER_ENTRY pRegisters, CTransScriptParser *pParser, BOOL bIsDuplicated, void *pBackUp)
{
	if (bIsDuplicated)
	{
		if (m_bUnicode || m_bUnicode8)
		{
			if (*(WCHAR *)m_pTransTextBuf == L'\0')
				return FALSE;
		}
		else
		{
			if (*(char *)m_pTransTextBuf == '\0')
				return FALSE;
		}
		// 번역 버퍼에 내용이 있으면 그대로 사용한다.
	}

	NOTIFY_DEBUG_MESSAGE(_T("SOW: ApplyTranslatedText: called\n"));

	int nOrigSize=0, nTransSize=0, nDelta=0;

	CString strScript = m_strArgScript;
	int nLen = strScript.GetLength();

	LPBYTE *ppArgText=NULL;

#if defined(_DEBUG) || defined(FORCE_DEBUG_LOG)

	if (m_bUnicode)
	{
		NOTIFY_DEBUG_MESSAGE(_T("SOW::ApplyTranslatedText: szSrc =%s\n"), (LPCWSTR)m_pTransTextBuf);
		NOTIFY_DEBUG_MESSAGE(_T("SOW::ApplyTranslatedText: szDest =%s\n"), (LPCWSTR)m_pOrigTextBuf);
	}
	else
	{
		WCHAR wszTemp[MAX_TEXT_LENGTH];
		MyMultiByteToWideChar(949, 0, (LPCSTR)m_pTransTextBuf, -1, wszTemp, MAX_TEXT_LENGTH);
		NOTIFY_DEBUG_MESSAGE(_T("SOW::ApplyTranslatedText: szSrc =%s\n"), wszTemp);
		MyMultiByteToWideChar(932, 0, (LPCSTR)m_pOrigTextBuf, -1, wszTemp, MAX_TEXT_LENGTH);
		NOTIFY_DEBUG_MESSAGE(_T("SOW::ApplyTranslatedText: szDest=%s\n"), wszTemp);
	}
#endif

	// 길이를 구하고
	if (m_bUnicode)
	{
		nOrigSize=wcslen((LPWSTR) m_pOrigTextBuf) *2;
		nTransSize=wcslen((LPWSTR) m_pTransTextBuf) *2;
	}
	else
	{
		nOrigSize=strlen((LPSTR) m_pOrigTextBuf);
		nTransSize=strlen((LPSTR) m_pTransTextBuf);
	}

	// 백업 및 덮어쓰기
	nDelta = nOrigSize - nTransSize;

	NOTIFY_DEBUG_MESSAGE(_T("pArgText=%p, BackupPoint=%p\n"), pArgText, (LPBYTE)pArgText + nDelta);
	DoBackupAndOverwrite((LPBYTE)pArgText + nDelta, nTransSize);

	// 포인터 위치 보정
	if(strScript.Left(3).CompareNoCase(_T("EAX")) == 0) ppArgText=(LPBYTE *)&(pRegisters->_EAX);
	else if(strScript.Left(3).CompareNoCase(_T("EBX")) == 0) ppArgText=(LPBYTE *)&(pRegisters->_EBX);
	else if(strScript.Left(3).CompareNoCase(_T("ECX")) == 0) ppArgText=(LPBYTE *)&(pRegisters->_ECX);
	else if(strScript.Left(3).CompareNoCase(_T("EDX")) == 0) ppArgText=(LPBYTE *)&(pRegisters->_EDX);
	else if(strScript.Left(3).CompareNoCase(_T("ESI")) == 0) ppArgText=(LPBYTE *)&(pRegisters->_ESI);
	else if(strScript.Left(3).CompareNoCase(_T("EDI")) == 0) ppArgText=(LPBYTE *)&(pRegisters->_EDI);
	else if(strScript.Left(3).CompareNoCase(_T("EBP")) == 0) ppArgText=(LPBYTE *)&(pRegisters->_EBP);
	else if(strScript.Left(3).CompareNoCase(_T("ESP")) == 0) ppArgText=(LPBYTE *)&(pRegisters->_ESP);
	else if(nLen >= 5 && _T('[') == strScript[0])
	{
		int nType;
		int nBracketCount=0;
		int i;
		int *pRetVal;

		for(i=0; i < nLen; i++)
		{
			if (strScript[i] == _T('['))
				nBracketCount++;
			else if (strScript[i] == _T(']'))
				nBracketCount--;

			if (nBracketCount == 0)
			{
				i++;
				break;
			}
		}

		pRetVal = (int*)pParser->GetValue(strScript.Mid(1, i-2), &nType);
		if(pRetVal && 1 == nType)
		{
			ppArgText = *(LPBYTE**)pRetVal;
			delete pRetVal;
		}
	}

	if (ppArgText && !IsBadWritePtr(ppArgText, sizeof(LPBYTE)))
	{
		SetUpTextPoint(pBackUp, (LPBYTE)pArgText + nDelta, ppArgText, 1);
		*ppArgText += nDelta;
	}
	else
		return FALSE;

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//
// SMSTR 방식 클래스 메소드
//
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// 특별 옵션을 분석, 저장합니다.
//
void CTransCommandSMSTR::SetSpecialOption(CString strSpecialOption, COptionNode *pNode)
{
	if(strSpecialOption == _T("IGNORE"))
	{
		NOTIFY_DEBUG_MESSAGE(_T("SMSTR: SetSpecialOption: IGNORE\n"));
		m_bIgnoreBufLen=TRUE;
	}
	else if(strSpecialOption == _T("MATCH"))
	{
		NOTIFY_DEBUG_MESSAGE(_T("SMSTR: SetSpecialOption: MATCH\n"));
		m_bMatchSize=TRUE;
	}
	else if(strSpecialOption == _T("FORCEPTR"))
	{
		NOTIFY_DEBUG_MESSAGE(_T("SMSTR: SetSpecialOption: FORCEPTR\n"));
		m_bForcePtr=TRUE;
	}
	else if(strSpecialOption == _T("NULLCHAR"))
	{
		NOTIFY_DEBUG_MESSAGE(_T("SMSTR: SetSpecialOption: NULLCHAR\n"));
		m_bNullchar=TRUE;
	}
	else if(strSpecialOption == _T("REVERSE"))
	{
		NOTIFY_DEBUG_MESSAGE(_T("SMSTR: SetSpecialOption: REVERSE\n"));
		m_bReverse=TRUE;
	}
	else if(strSpecialOption == _T("YURIS"))
	{
		NOTIFY_DEBUG_MESSAGE(_T("SMSTR: SetSpecialOption: YURIS\n"));
		m_YurisOffset = 0;
		m_bYuris=TRUE;
		m_bYurisP=TRUE;
	}
	else if(strSpecialOption == _T("WILL"))
	{
		NOTIFY_DEBUG_MESSAGE(_T("SMSTR: SetSpecialOption: WILL\n"));
		m_bWill=TRUE;
	}
	else if(strSpecialOption == _T("ENDPOS"))
	{
		NOTIFY_DEBUG_MESSAGE(_T("SMSTR: SetSpecialOption: ENDPOS\n"));
		m_bEndPos=TRUE;
	}
	else if(strSpecialOption == _T("TJSSTR"))
	{
		NOTIFY_DEBUG_MESSAGE(_T("SMSTR: SetSpecialOption: TJSSTR\n"));
		m_bTjsStr=TRUE;
	}
	else
		CTransCommand::SetSpecialOption(strSpecialOption, pNode);
}

//////////////////////////////////////////////////////////////////////////
// 실제 번역할 원문의 위치를 반환합니다. (오버라이드됨)
//
void *CTransCommandSMSTR::GetTextPoint(CTransScriptParser *pParser, void *pBackUp, BOOL bPtrBack, void *pBackUpLength)
{
	// 이전 포인터 교체한것 복구하기
	if (bPtrBack)
	{
		if(BackUpTextPoint(pBackUp, m_nBackUpSize))
			// 길이값 복원
			if (m_bAdjustLen)
				BackUpLength(pBackUpLength);
	}

	if (pParser == NULL) return NULL;

	NOTIFY_DEBUG_MESSAGE(_T("SMSTR: GetTextPoint: called\n"));

	void * pArgText = NULL;

	// ArgScript ( [ESI] 같은..) 가 가리키는 위치 얻기
	int nType;
	int* pRetVal = (int*)pParser->GetValue(GetArgScript(), &nType);
	if(pRetVal && 1 == nType)
	{
		pArgText = *(LPVOID *)pRetVal;
		delete pRetVal;
	}

	// 얻어온 위치의 유효성 검사 (SMSTR의 데이타 크기는 6 DWORD (24 바이트))
	if(pArgText == NULL || IsBadReadPtr(pArgText, sizeof(DWORD) * 6)) return NULL;

	m_pArgText=pArgText;

	NOTIFY_DEBUG_MESSAGE(_T("  m_pArgText=%p\n "), m_pArgText);
	NOTIFY_DEBUG_MESSAGE(_T("  %08X %08X %08X %08X %08X %08X\n"), 
		*(&(*(DWORD*)pArgText)+0), *(&(*(DWORD*)pArgText)+1), *(&(*(DWORD*)pArgText)+2),
		*(&(*(DWORD*)pArgText)+3), *(&(*(DWORD*)pArgText)+4), *(&(*(DWORD*)pArgText)+5));

	if (m_bWill)
	{
		// 개행수를 저장
		DWORD dwLen = *(&(*(DWORD*)pArgText));

		if(dwLen == 0)
		{
			NOTIFY_DEBUG_MESSAGE(_T("SMSTR: GetTextPoint: dwLen=0\n"));
			return NULL;
		}

		NOTIFY_DEBUG_MESSAGE(_T("SMSTR: GetTextPoint: Will Plus\n"));

		// 글자수가 비정상적이면 에러
		if(dwLen > 4096) return NULL;

		// pArgText 가 가리키는 버퍼가 읽기불가면 에러
		if ( IsBadReadPtr(pArgText, sizeof(CHAR) * 0x100) )
			return NULL;

		
		m_nBackUpSize = 0;

		DWORD dwTest=0;
		DWORD dwDest=0;
		BOOL bwOne=0;

		if (m_bUnicode)
		{
			LPWSTR wszText = NULL;
			int wPos = 0;

			wszText = (LPWSTR)pArgText + 4;
			dwTest = *(&(*(BYTE*)pArgText)+4);

			// 번역 가능한 유니코드 형태로 강제로 모으기
			while(dwLen > 1)
			{
				int i = 0;
				dwDest = *(&(*(BYTE*)pArgText)+8 + dwTest);
				if((dwTest == *(&(*(BYTE*)pArgText)+4)) && bwOne == 0)
				{
					if( ((*(&(*(BYTE*)pArgText)+8 + *(&(*(BYTE*)pArgText)+4) + 4 ) == 0x81)  &&
						 (*(&(*(BYTE*)pArgText)+8 + *(&(*(BYTE*)pArgText)+4) + 5 ) == 0x69)) ||
						((*(&(*(BYTE*)pArgText)+8 + *(&(*(BYTE*)pArgText)+4) + 4 ) == 0x81)  &&
						 (*(&(*(BYTE*)pArgText)+8 + *(&(*(BYTE*)pArgText)+4) + 5 ) == 0x6b)) ||
						((*(&(*(BYTE*)pArgText)+8 + *(&(*(BYTE*)pArgText)+4) + 4 ) == 0x81)  &&
						 (*(&(*(BYTE*)pArgText)+8 + *(&(*(BYTE*)pArgText)+4) + 5 ) == 0x6d)) ||
						((*(&(*(BYTE*)pArgText)+8 + *(&(*(BYTE*)pArgText)+4) + 4 ) == 0x81)  &&
						 (*(&(*(BYTE*)pArgText)+8 + *(&(*(BYTE*)pArgText)+4) + 5 ) == 0x6f)) ||
						((*(&(*(BYTE*)pArgText)+8 + *(&(*(BYTE*)pArgText)+4) + 4 ) == 0x81)  &&
						 (*(&(*(BYTE*)pArgText)+8 + *(&(*(BYTE*)pArgText)+4) + 5 ) == 0x71)) ||
						((*(&(*(BYTE*)pArgText)+8 + *(&(*(BYTE*)pArgText)+4) + 4 ) == 0x81)  &&
						 (*(&(*(BYTE*)pArgText)+8 + *(&(*(BYTE*)pArgText)+4) + 5 ) == 0x73)) ||
						((*(&(*(BYTE*)pArgText)+8 + *(&(*(BYTE*)pArgText)+4) + 4 ) == 0x81)  &&
						 (*(&(*(BYTE*)pArgText)+8 + *(&(*(BYTE*)pArgText)+4) + 5 ) == 0x75)) ||
						((*(&(*(BYTE*)pArgText)+8 + *(&(*(BYTE*)pArgText)+4) + 4 ) == 0x81)  &&
						 (*(&(*(BYTE*)pArgText)+8 + *(&(*(BYTE*)pArgText)+4) + 5 ) == 0x77)) ||
						((*(&(*(BYTE*)pArgText)+8 + *(&(*(BYTE*)pArgText)+4) + 4 ) == 0x81)  &&
						 (*(&(*(BYTE*)pArgText)+8 + *(&(*(BYTE*)pArgText)+4) + 5 ) == 0x79)) )
					{
						*(&(*(BYTE*)pArgText)+8 + *(&(*(BYTE*)pArgText)+4)) = 0x9F;
						*(&(*(BYTE*)pArgText)+8 + *(&(*(BYTE*)pArgText)+4) + 1) = 0;
						*(&(*(BYTE*)pArgText)+8 + *(&(*(BYTE*)pArgText)+4) + 2) = 0xFF;
						*(&(*(BYTE*)pArgText)+8 + *(&(*(BYTE*)pArgText)+4) + 3) = 0;
						wPos += 2;
					}
					else
					{
						*(&(*(BYTE*)pArgText)+8 + *(&(*(BYTE*)pArgText)+4)) = 0x0A;
						*(&(*(BYTE*)pArgText)+8 + *(&(*(BYTE*)pArgText)+4) + 1) = 0;
					}
					wPos += 2;
					bwOne = 1;
				}
				while(dwDest > (DWORD)i){
					*(&(*(BYTE*)pArgText)+8 + *(&(*(BYTE*)pArgText)+4) + wPos) = *(&(*(BYTE*)pArgText)+8 + dwTest + 4 + i);
					i ++;
					wPos ++;
				}
				*(&(*(BYTE*)pArgText)+8 + *(&(*(BYTE*)pArgText)+4) + wPos) = 0x0A;
				*(&(*(BYTE*)pArgText)+8 + *(&(*(BYTE*)pArgText)+4) + wPos + 1) = 0;
				wPos += 2;
				dwTest += dwDest + 4;
				dwLen --;
			}
			wPos -= 2;
			*(&(*(BYTE*)pArgText)+8 + *(&(*(BYTE*)pArgText)+4) + wPos) = 0;
			*(&(*(BYTE*)pArgText)+8 + *(&(*(BYTE*)pArgText)+4) + wPos + 1) = 0;
			return wszText;
		}
		else
		{
			LPSTR szText = NULL;
			int wPos = 0;

			szText = (LPSTR)pArgText + 8;
			dwTest = *(&(*(BYTE*)pArgText)+4);

			// 번역 가능한 유니코드 형태로 강제로 모으기
			while(dwLen > 1)
			{
				int i = 0;
				dwDest = *(&(*(BYTE*)pArgText)+8 + dwTest);
				if((dwTest == *(&(*(BYTE*)pArgText)+4)) && bwOne == 0)
				{
					if( ((*(&(*(BYTE*)pArgText)+8 + *(&(*(BYTE*)pArgText)+4) + 4 ) == 0x81)  &&
						 (*(&(*(BYTE*)pArgText)+8 + *(&(*(BYTE*)pArgText)+4) + 5 ) == 0x69)) ||
						((*(&(*(BYTE*)pArgText)+8 + *(&(*(BYTE*)pArgText)+4) + 4 ) == 0x81)  &&
						 (*(&(*(BYTE*)pArgText)+8 + *(&(*(BYTE*)pArgText)+4) + 5 ) == 0x6b)) ||
						((*(&(*(BYTE*)pArgText)+8 + *(&(*(BYTE*)pArgText)+4) + 4 ) == 0x81)  &&
						 (*(&(*(BYTE*)pArgText)+8 + *(&(*(BYTE*)pArgText)+4) + 5 ) == 0x6d)) ||
						((*(&(*(BYTE*)pArgText)+8 + *(&(*(BYTE*)pArgText)+4) + 4 ) == 0x81)  &&
						 (*(&(*(BYTE*)pArgText)+8 + *(&(*(BYTE*)pArgText)+4) + 5 ) == 0x6f)) ||
						((*(&(*(BYTE*)pArgText)+8 + *(&(*(BYTE*)pArgText)+4) + 4 ) == 0x81)  &&
						 (*(&(*(BYTE*)pArgText)+8 + *(&(*(BYTE*)pArgText)+4) + 5 ) == 0x71)) ||
						((*(&(*(BYTE*)pArgText)+8 + *(&(*(BYTE*)pArgText)+4) + 4 ) == 0x81)  &&
						 (*(&(*(BYTE*)pArgText)+8 + *(&(*(BYTE*)pArgText)+4) + 5 ) == 0x73)) ||
						((*(&(*(BYTE*)pArgText)+8 + *(&(*(BYTE*)pArgText)+4) + 4 ) == 0x81)  &&
						 (*(&(*(BYTE*)pArgText)+8 + *(&(*(BYTE*)pArgText)+4) + 5 ) == 0x75)) ||
						((*(&(*(BYTE*)pArgText)+8 + *(&(*(BYTE*)pArgText)+4) + 4 ) == 0x81)  &&
						 (*(&(*(BYTE*)pArgText)+8 + *(&(*(BYTE*)pArgText)+4) + 5 ) == 0x77)) ||
						((*(&(*(BYTE*)pArgText)+8 + *(&(*(BYTE*)pArgText)+4) + 4 ) == 0x81)  &&
						 (*(&(*(BYTE*)pArgText)+8 + *(&(*(BYTE*)pArgText)+4) + 5 ) == 0x79)) )
					{
						*(&(*(BYTE*)pArgText)+8 + *(&(*(BYTE*)pArgText)+4)) = 0x9F;
						*(&(*(BYTE*)pArgText)+8 + *(&(*(BYTE*)pArgText)+4) + 1) = 0xFF;
						wPos ++;
					}
					else
					{
						*(&(*(BYTE*)pArgText)+8 + *(&(*(BYTE*)pArgText)+4)) = 0x0A;
					}
					wPos ++;
					bwOne = 1;
				}
				while(dwDest > (DWORD)i){
					*(&(*(BYTE*)pArgText)+8 + *(&(*(BYTE*)pArgText)+4) + wPos) = *(&(*(BYTE*)pArgText)+8 + dwTest + 4 + i);
					i ++;
					wPos ++;
				}
				*(&(*(BYTE*)pArgText)+8 + *(&(*(BYTE*)pArgText)+4) + wPos) = 0x0A;
				wPos ++;
				dwTest += dwDest + 4;
				dwLen --;
			}
			wPos --;
			*(&(*(BYTE*)pArgText)+8 + *(&(*(BYTE*)pArgText)+4) + wPos) = 0;
			return szText;
		}
	}

	if (m_bEndPos)
	{
		// pArgText 가 가리키는 포인터가 비정상이면 에러
		if (*(DWORD *)pArgText == NULL) return NULL;
		if (*(&(*(DWORD*)pArgText)) == NULL) return NULL;
		if (*(&(*(DWORD*)pArgText)+1) == NULL) return NULL;

		DWORD dwLen = *(&(*(DWORD*)pArgText)+1) - *(&(*(DWORD*)pArgText));

		if(dwLen == 0)
		{
			NOTIFY_DEBUG_MESSAGE(_T("SMSTR: GetTextPoint: dwLen=0\n"));
			return NULL;
		}

		NOTIFY_DEBUG_MESSAGE(_T("SMSTR: GetTextPoint: EndPos\n"));

		// 글자수가 비정상적이면 에러
		if(dwLen > 4096) return NULL;

		// pArgText 가 가리키는 버퍼가 읽기불가면 에러
		if ( IsBadReadPtr((WCHAR *)(*(&(*(DWORD*)pArgText))), sizeof(WCHAR) * (dwLen + 1)) )
			return NULL;

		DWORD dwTest=0;

		if (m_bUnicode)
		{
			// 실제 글자수와 저장된 글자수가 정확한지 체크
			while(dwTest < dwLen){
				if(*(&(*(WCHAR*)(*(&(*(DWORD*)pArgText))))+dwTest)==L'\0'){
					return NULL;
				}
				dwTest++;
			}
		}
		else
		{
			// 실제 글자수와 저장된 글자수가 정확한지 체크
			while(dwTest < dwLen){
				if(*(&(*(CHAR*)(*(&(*(DWORD*)pArgText))))+dwTest)=='\0'){
					return NULL;
				}
				dwTest++;
			}
		}
		m_nBackUpSize = 2;
		return pArgText;
	}

	if (m_bTjsStr)
	{
		// pArgText 가 가리키는 포인터가 비정상이면 에러
		char chChk = *(&(*(BYTE*)pArgText));
		if(!(
			/*
			chChk == 0x0 || // 명령어
			chChk == 0x1 || // 띄어쓰기 이름, 명령어
			chChk == 0x3 || // 로드명령어
			*/
			chChk == 0x2 || // 대사스크립, 로드타임스크립
			chChk == 0x4 || // 백로드스크립, 컨피그스크립, 로드스크립
			chChk == 0x5 || // 세이브명령어, 컨피그타이핑스크립, 버튼 오버 스크립, 백로그 이름
			chChk == 0x6 || // 컨피그 오버 스크립
			chChk == 0x7 || // ?? 명령어 이름
			chChk == 0x8 || // 대사창명령어, 대사이름
			chChk == 0x9 || // 고정루비문자 아래 스크립
			chChk == 0xA || // 특수문자, 선택지
			chChk == 0xB || // 그림자 이름
			chChk == 0xC || // 대사스크립2
			chChk == 0xD || // 전투단어
			chChk == 0x13|| // 전투단어
			chChk == 0x16|| // 전투단어
			chChk == 0x17|| // 전투단어
			chChk == 0x18|| // 전투단어
			chChk == 0x19|| // 전투단어
			chChk == 0x1D|| // 전투단어
			chChk == 0x1E|| // 전투단어
			chChk == 0x23)) // 전투스크립
			return NULL;

		DWORD dwLen = *(&(*(DWORD*)pArgText)+13);

		if(dwLen == 0)
		{
			NOTIFY_DEBUG_MESSAGE(_T("SMSTR: GetTextPoint: dwLen=0\n"));
			return NULL;
		}

		NOTIFY_DEBUG_MESSAGE(_T("SMSTR: GetTextPoint: KiriKiri TJSString\n"));

		// 글자수가 비정상적이면 에러
		if(dwLen > 4096) return NULL;

		// 5 패턴 한글자일경우 무시
		if(chChk == 0x5 && dwLen <= 2) return NULL;

		// pArgText 가 가리키는 버퍼가 읽기불가면 에러
		if ( IsBadReadPtr(pArgText, sizeof(WCHAR) * 13) )
			return NULL;

		DWORD dwTest=0;

		if (m_bUnicode)
		{
			LPWSTR wszText = NULL;

			if (*(&(*(DWORD*)pArgText)+1) == NULL)
				wszText = (LPWSTR)pArgText + 4;
			else
				wszText = (LPWSTR)*(&(*(DWORD*)pArgText)+1);

			// 실제 글자수와 저장된 글자수가 정확한지 체크
			while(dwTest < dwLen){
				if(*(&(*(WCHAR*)wszText)+dwTest)==L'\0'){
					return NULL;
				}
				dwTest++;
			}
			m_nBackUpSize = 13;
			return wszText;
		}
		else
		{
			LPSTR szText = NULL;

			if (*(&(*(DWORD*)pArgText)+1) == NULL)
				szText = (LPSTR)pArgText + 8;
			else
				szText = (LPSTR)*(&(*(DWORD*)pArgText)+1);

			// 실제 글자수와 저장된 글자수가 정확한지 체크
			while(dwTest < dwLen){
				if(*(&(*(CHAR*)szText)+dwTest)=='\0'){
					return NULL;
				}
				dwTest++;
			}
			m_nBackUpSize = 13;
			return szText;
		}
	}

	if (m_bYuris)
	{
		if (m_bUnicode)
		{
			LPWSTR wszText = NULL;
			DWORD dwLen = 0;
			int nCount = 2;

			//유리스 길이 위치 Offset값 찾기
			if(m_YurisOffset==0/*&&m_bYurisP==TRUE*/)
			{
				while(nCount<20){
					if(*(&(*(DWORD*)pArgText)+nCount) == 0 && *(&(*(DWORD*)pArgText)+nCount+1) == 0)
					{
						m_YurisOffset = nCount - 2;
						dwLen = *(&(*(DWORD*)pArgText)+m_YurisOffset);
						m_bYurisP=TRUE;
						break;
					}
					nCount++;
				}
				if(nCount>=20) 
				{
					m_bYurisP=FALSE;
					m_YurisOffset=0;
					return NULL;
				}
			}
			/*else if(m_bYurisP==TRUE)
			{
				dwLen = *(&(*(DWORD*)pArgText)+m_YurisOffset);
			}*/
			else dwLen = *(&(*(DWORD*)pArgText)+m_YurisOffset); //return NULL;

			if(dwLen == 0)
			{
				NOTIFY_DEBUG_MESSAGE(_T("SMSTR: GetTextPoint: dwLen=0\n"));
				return NULL;
			}

			NOTIFY_DEBUG_MESSAGE(_T("SMSTR: GetTextPoint: Unicode Yuris\n"));
			// pArgText 가 가리키는 포인터가 비정상이면 에러
			if (*(DWORD *)pArgText == NULL) return NULL;

			// 글자수가 비정상적이면 에러
			if(dwLen > 4096) return NULL;

			// pArgText 가 가리키는 버퍼가 읽기불가면 에러
			if ( IsBadReadPtr((WCHAR *)(*(&(*(DWORD*)pArgText))), sizeof(WCHAR) * (dwLen + 1)) )
				return NULL;

			DWORD dwTest=0;

			// 실제 글자수와 저장된 글자수가 정확한지 체크
			while(dwTest < dwLen){
				if(*(&(*(WCHAR*)(*(&(*(DWORD*)pArgText))))+dwTest)==L'\0'){
					return NULL;
				}
				dwTest++;
			}

			// 글자수 이상으로 다른 데이타가 들어가있는지 체크
			if(*(&(*(WCHAR*)(*(&(*(DWORD*)pArgText))))+dwLen)!=L'\0'){
				return NULL;
			}
		
			// 실제 텍스트 위치는 pArgText 가 가리키는 메모리
			wszText = (LPWSTR)*(&(*(DWORD*)pArgText));

			m_nBackUpSize = m_YurisOffset + 2;
			return wszText;
		}
		else // MBCS
		{
			LPSTR szText = NULL;
			DWORD dwLen = 0;
			int nCount = 2;

			//유리스 길이 위치 Offset값 찾기
			if(m_YurisOffset==0/*&&m_bYurisP==TRUE*/)
			{
				while(nCount<20){
					if(*(&(*(DWORD*)pArgText)+nCount) == 0 && *(&(*(DWORD*)pArgText)+nCount+1) == 0)
					{
						m_YurisOffset = nCount - 2;
						dwLen = *(&(*(DWORD*)pArgText)+m_YurisOffset);
						m_bYurisP=TRUE;
						break;
					}
					nCount++;
				}
				if(nCount>=20) 
				{
					m_bYurisP=FALSE;
					m_YurisOffset=0;
					return NULL;
				}
			}
			/*else if(m_bYurisP==TRUE)
			{
				dwLen = *(&(*(DWORD*)pArgText)+m_YurisOffset);
			}*/
			else dwLen = *(&(*(DWORD*)pArgText)+m_YurisOffset); //return NULL;

			if(dwLen == 0)
			{
				NOTIFY_DEBUG_MESSAGE(_T("SMSTR: GetTextPoint: dwLen=0\n"));
				return NULL;
			}

			NOTIFY_DEBUG_MESSAGE(_T("SMSTR: GetTextPoint: ANSI Yuris\n"));
			// pArgText 가 가리키는 포인터가 비정상이면 에러
			if (*(DWORD *)pArgText == NULL) return NULL;

			// 글자수가 비정상적이면 에러
			if(dwLen > 4096) return FALSE;

			// pArgText 가 가리키는 버퍼가 읽기불가면 에러
			if ( IsBadReadPtr((CHAR *)(*(&(*(DWORD*)pArgText))), sizeof(CHAR) * (dwLen + 1)) )
				return NULL;

			DWORD dwTest=0;
		
			// 실제 글자수와 저장된 글자수가 정확한지 체크
			while(dwTest < dwLen){
				if(*(&(*(CHAR*)(*(&(*(DWORD*)pArgText))))+dwTest)=='\0'){
					return NULL;
				}
				dwTest++;
			}

			// 글자수 이상으로 다른 데이타가 들어가있는지 체크
			if(*(&(*(CHAR*)(*(&(*(DWORD*)pArgText))))+dwLen)!='\0'){
				return NULL;
			}
		
			// 실제 텍스트 위치는 pArgText 가 가리키는 메모리
			szText = (LPSTR)*(&(*(DWORD*)pArgText));

			m_nBackUpSize = m_YurisOffset + 2;
			return szText;
		}
	}
	else
	{
		m_nBackUpSize = 6;

		if (m_bUnicode)
		{
			LPWSTR wszText = NULL;
			DWORD dwLen = *(&(*(DWORD*)pArgText)+4);

			if(dwLen == 0)
			{
				NOTIFY_DEBUG_MESSAGE(_T("SMSTR: GetTextPoint: dwLen=0\n"));
				return NULL;
			}
			else if (m_bNullchar)
			{
				if((dwLen >= 0x04) || m_bForcePtr)	// 글자수 4자 (16바이트) 이상일때
				{
					NOTIFY_DEBUG_MESSAGE(_T("SMSTR: GetTextPoint: UnicodeX2 Long\n"));
					// pArgText 가 가리키는 포인터가 비정상이면 에러
					if (*(DWORD *)pArgText == NULL) return NULL;

					// 글자수가 비정상적이면 에러
					if(dwLen > 4096) return NULL;

					// pArgText 가 가리키는 버퍼가 읽기불가면 에러
					if ( IsBadReadPtr((WCHAR *)(*(&(*(DWORD*)pArgText))), sizeof(DWORD) * (dwLen + 1)) )
						return NULL;

					DWORD dwTest=0;

					// 실제 글자수와 저장된 글자수가 정확한지 체크
					while(dwTest < dwLen){
						if(*(&(*(DWORD*)(*(&(*(DWORD*)pArgText))))+dwTest)==NULL){
							return NULL;
						}
						dwTest++;
					}

					// 글자수 이상으로 다른 데이타가 들어가있는지 체크
					if(*(&(*(DWORD*)(*(&(*(DWORD*)pArgText))))+dwLen)!=NULL){
						return NULL;
					}

					dwTest=0;

					// 번역 가능한 유니코드 형태로 강제로 모으기
					while(dwTest <= dwLen){
						*(&(*(WORD*)(*(&(*(DWORD*)pArgText))))+dwTest) = (WORD)*(&(*(DWORD*)(*(&(*(DWORD*)pArgText))))+dwTest);
						dwTest++;
					}
				
					// 실제 텍스트 위치는 pArgText 가 가리키는 메모리
					wszText = (LPWSTR)*(&(*(DWORD*)pArgText));		
				}
				else	// 글자수가 4자 (16바이트) 미만일때
				{
					NOTIFY_DEBUG_MESSAGE(_T("SMSTR: GetTextPoint: Unicode Short\n"));
					DWORD dwTest=0;

					// 실제 글자수와 저장된 글자수가 정확한지 체크
					while(dwTest < dwLen){
						if((*(&(*(DWORD*)pArgText)+dwTest))==NULL){
							return NULL;
						}
						dwTest++;
					}

					// 글자수 이상으로 다른 데이타가 들어가있는지 체크
					if(*(&(*(DWORD*)pArgText)+dwLen)!=NULL){
						return NULL;
					}

					dwTest=0;

					// 번역 가능한 유니코드 형태로 강제로 모으기
					while(dwTest <= dwLen){
						*(&(*(WORD*)pArgText)+dwTest) = (WORD)*(&(*(DWORD*)pArgText)+dwTest);
						dwTest++;
					}
				
					// 실제 글자는 pArgText부터 들어있다
					wszText = (LPWSTR)pArgText;
				}
			}
			else
			{
				if((dwLen >= 0x08) || m_bForcePtr)	// 글자수 8자 (16바이트) 이상일때
				{
					NOTIFY_DEBUG_MESSAGE(_T("SMSTR: GetTextPoint: Unicode Long\n"));
					// pArgText 가 가리키는 포인터가 비정상이면 에러
					if (*(DWORD *)pArgText == NULL) return NULL;

					// 글자수가 비정상적이면 에러
					if(dwLen > 4096) return NULL;

					// pArgText 가 가리키는 버퍼가 읽기불가면 에러
					if ( IsBadReadPtr((WCHAR *)(*(&(*(DWORD*)pArgText))), sizeof(WCHAR) * (dwLen + 1)) )
						return NULL;

					DWORD dwTest=0;

					// 실제 글자수와 저장된 글자수가 정확한지 체크
					while(dwTest < dwLen){
						if(*(&(*(WCHAR*)(*(&(*(DWORD*)pArgText))))+dwTest)==L'\0'){
							return NULL;
						}
						dwTest++;
					}

					// 글자수 이상으로 다른 데이타가 들어가있는지 체크
					if(*(&(*(WCHAR*)(*(&(*(DWORD*)pArgText))))+dwLen)!=L'\0'){
						return NULL;
					}
				
					// 실제 텍스트 위치는 pArgText 가 가리키는 메모리
					wszText = (LPWSTR)*(&(*(DWORD*)pArgText));		
				}
				else	// 글자수가 8자 (16바이트) 미만일때
				{
					NOTIFY_DEBUG_MESSAGE(_T("SMSTR: GetTextPoint: Unicode Short\n"));
					DWORD dwTest=0;

					// 실제 글자수와 저장된 글자수가 정확한지 체크
					while(dwTest < dwLen){
						if((*(&(*(WCHAR*)pArgText)+dwTest))==L'\0'){
							return NULL;
						}
						dwTest++;
					}

					// 글자수 이상으로 다른 데이타가 들어가있는지 체크
					if(*(&(*(WCHAR*)pArgText)+dwLen)!=L'\0'){
						return NULL;
					}
				
					// 실제 글자는 pArgText부터 들어있다
					wszText = (LPWSTR)pArgText;
				}
			}

			// 유니코드 문자의 (ex. 0xXX 0xYY) 위치를 반전 (ex. 0xYY 0xXX)
			if(m_bReverse)
			{
				BYTE dwTest=0;
				BYTE wszBuf;
				while(dwTest < dwLen){
					wszBuf = *(&(*(BYTE*)wszText)+dwTest);
					*(&(*(BYTE*)wszText)+dwTest) = *(&(*(BYTE*)wszText)+dwTest+1);
					*(&(*(BYTE*)wszText)+dwTest+1) = wszBuf;
					dwTest+= 2;
				}
			}

			NOTIFY_DEBUG_MESSAGE(_T("SMSTR: GetTextPoint: %p\n"), wszText);
			return wszText;

			/*&if (pCmd->GetSmbuf() &&  pCmd->GetTransMethod() == TRANSCOMMAND_OVERWRITE ){
				if(*(&(*(DWORD*)pArgText)+4) >= 0x08) continue;
			}*/

		}
		else // MBCS
		{
			LPSTR szText = NULL;
			DWORD dwLen = *(&(*(DWORD*)pArgText)+4);

			if(dwLen == 0)
			{
				NOTIFY_DEBUG_MESSAGE(_T("SMSTR: GetTextPoint: dwLen=0\n"));
				return NULL;
			}
			else if (m_bNullchar)
			{
				if((dwLen >= 0x4) || m_bForcePtr)	// 글자수 4자 (16바이트) 이상일때
				{
					NOTIFY_DEBUG_MESSAGE(_T("SMSTR: GetTextPoint: ANSI Long\n"));
					// pArgText 가 가리키는 포인터가 비정상이면 에러
					if (*(DWORD *)pArgText == NULL) return NULL;

					// 글자수가 비정상적이면 에러
					if(dwLen > 4096) return FALSE;

					// pArgText 가 가리키는 버퍼가 읽기불가면 에러
					if ( IsBadReadPtr((CHAR *)(*(&(*(DWORD*)pArgText))), sizeof(DWORD) * (dwLen + 1)) )
						return NULL;

					DWORD dwTest=0;

					// 실제 글자수와 저장된 글자수가 정확한지 체크
					while(dwTest < dwLen){
						if(*(&(*(DWORD*)(*(&(*(DWORD*)pArgText))))+dwTest)==NULL){
							return NULL;
						}
						dwTest++;
					}

					// 글자수 이상으로 다른 데이타가 들어가있는지 체크
					if(*(&(*(DWORD*)(*(&(*(DWORD*)pArgText))))+dwLen)!=NULL){
						return NULL;
					}

					dwTest=0;

					// 번역 가능한 아스키 형태로 강제로 모으기
					while(dwTest <= dwLen){
						*(&(*(WORD*)(*(&(*(DWORD*)pArgText))))+dwTest) = (WORD)*(&(*(DWORD*)(*(&(*(DWORD*)pArgText))))+dwTest);
						dwTest++;
					}
				
					// 실제 텍스트 위치는 pArgText 가 가리키는 메모리
					szText = (LPSTR)*(&(*(DWORD*)pArgText));
				}
				else 	// 글자수가 4자 (16바이트) 미만일때
				{
					NOTIFY_DEBUG_MESSAGE(_T("SMSTR: GetTextPoint: ANSI Short\n"));
					DWORD dwTest=0;

					// 실제 글자수와 저장된 글자수가 정확한지 체크
					while(dwTest < dwLen){
						if((*(&(*(DWORD*)pArgText)+dwTest))==NULL){
							return NULL;
						}
						dwTest++;
					}

					// 글자수 이상으로 다른 데이타가 들어가있는지 체크
					if(*(&(*(DWORD*)pArgText)+dwLen)!=NULL){
						return NULL;
					}

					dwTest=0;

					// 번역 가능한 아스키 형태로 강제로 모으기
					while(dwTest <= dwLen){
						*(&(*(WORD*)pArgText)+dwTest) = (WORD)*(&(*(DWORD*)pArgText)+dwTest);
						dwTest++;
					}
				
					// 실제 글자는 pArgText부터 들어있다
					szText = (LPSTR)pArgText;
				}
			}
			else
			{
				if((dwLen >= 0x10) || m_bForcePtr)	// 글자수 16자 (16바이트) 이상일때
				{
					NOTIFY_DEBUG_MESSAGE(_T("SMSTR: GetTextPoint: ANSI Long\n"));
					// pArgText 가 가리키는 포인터가 비정상이면 에러
					if (*(DWORD *)pArgText == NULL) return NULL;

					// 글자수가 비정상적이면 에러
					if(dwLen > 4096) return FALSE;

					// pArgText 가 가리키는 버퍼가 읽기불가면 에러
					if ( IsBadReadPtr((CHAR *)(*(&(*(DWORD*)pArgText))), sizeof(CHAR) * (dwLen + 1)) )
						return NULL;

					DWORD dwTest=0;
				
					// 실제 글자수와 저장된 글자수가 정확한지 체크
					while(dwTest < dwLen){
						if(*(&(*(CHAR*)(*(&(*(DWORD*)pArgText))))+dwTest)=='\0'){
							return NULL;
						}
						dwTest++;
					}

					// 글자수 이상으로 다른 데이타가 들어가있는지 체크
					if(*(&(*(CHAR*)(*(&(*(DWORD*)pArgText))))+dwLen)!='\0'){
						return NULL;
					}
				
					// 실제 텍스트 위치는 pArgText 가 가리키는 메모리
					szText = (LPSTR)*(&(*(DWORD*)pArgText));
				}
				else 	// 글자수가 8자 (16바이트) 미만일때
				{
					NOTIFY_DEBUG_MESSAGE(_T("SMSTR: GetTextPoint: ANSI Short\n"));
					DWORD dwTest=0;

					// 실제 글자수와 저장된 글자수가 정확한지 체크
					while(dwTest < dwLen){
						if((*(&(*(CHAR*)pArgText)+dwTest))=='\0'){
							return NULL;
						}
						dwTest++;
					}

					// 글자수 이상으로 다른 데이타가 들어가있는지 체크
					if(*(&(*(CHAR*)pArgText)+dwLen)!='\0'){
						return NULL;
					}
				
					// 실제 글자는 pArgText부터 들어있다
					szText = (LPSTR)pArgText;
				}
			}

			/*if (pCmd->GetSmbuf() &&  pCmd->GetTransMethod() == TRANSCOMMAND_OVERWRITE ){
				if(*(&(*(DWORD*)pArgText)+4) >= 0x10) continue;
			}*/

			// 아스키 문자의 (ex. 0xXX 0xYY) 위치를 반전 (ex. 0xYY 0xXX)
			// 아스키일때는 1바이트 문자없음, 공문자 자체적인 제거요망 (1바이트 문자)
			if(m_bReverse)
			{
				BYTE dsTest=0;
				BYTE szBuf;
				BYTE dwSub=0;

				while(dsTest < dwLen){
					szBuf = *(&(*(BYTE*)szText)+(dsTest * 2));
					if(*(&(*(BYTE*)szText)+(dsTest * 2)+1) == 0x00)
						dwSub++;
					else
						*(&(*(BYTE*)szText)+(dsTest * 2)-dwSub) = *(&(*(BYTE*)szText)+(dsTest * 2)+1);
					*(&(*(BYTE*)szText)+(dsTest * 2)+1-dwSub) = szBuf;
					
					dsTest++;
				}
				*(&(*(BYTE*)szText)+(dsTest * 2)-dwSub) = 0x00;
			}

			NOTIFY_DEBUG_MESSAGE(_T("SMSTR: GetTextPoint: %p\n"), szText);
			return szText;

		}
	}
}
//////////////////////////////////////////////////////////////////////////
// 번역문의 길이에 따라 스마트 문자열을 적용시킵니다.
//
BOOL CTransCommandSMSTR::ApplySmartString( LPVOID pDest, LPVOID pSrc, void *pBackUp )
{
	NOTIFY_DEBUG_MESSAGE(_T("SMSTR: ApplySmartString: called\n"));

	void *pMain = m_pArgText;

	DWORD sLen;
	BOOL bWasLongMode;

	if (m_bWill)
	{
		// 포인터를 교체하기전 미리 백업해둔다
		SetUpTextPoint(pBackUp, (&(*(DWORD*)(*(&(*(DWORD*)pMain))))), (&(*(DWORD*)pMain)), m_nBackUpSize);

		if(m_bUnicode)
		{
			LPWSTR wszSrc = (LPWSTR)pSrc;
			UINT_PTR nSrcLen =  wcslen(wszSrc);
			LPWSTR wszDest = (LPWSTR)pDest;
			UINT_PTR nDestLen = wcslen(wszDest);

			BYTE dwTest=0;
			WORD wszBuf;
			BYTE dwSub=0;
			BYTE dwLen=1;

			memmove(wszSrc, wszSrc+2, nSrcLen);
			*(&(*(DWORD*)wszSrc)) = 0;
			nSrcLen += 2;
			
			while(dwTest < nSrcLen)
			{
				wszBuf = *(&(*(WORD*)wszSrc)+dwTest);
				if(wszBuf == 0x0A)
				{
					memmove(wszSrc+dwTest+1, wszSrc+dwTest, nSrcLen-dwTest);
					*(&(*(WORD*)wszSrc)+dwSub) = dwTest - 2 - dwSub;
					*(&(*(WORD*)wszSrc)+dwSub+1) = 0;
					*(&(*(WORD*)wszSrc)+dwTest) = 0;
					*(&(*(WORD*)wszSrc)+dwTest+1) = 0;
					dwSub = dwTest;
					nSrcLen += 2;
					dwTest ++;
					dwLen ++;
				}
				else if((*(&(*(WORD*)wszSrc)+dwTest+1) == 0xFF) && wszBuf == 0x9F)
				{
					memmove(wszSrc+dwTest, wszSrc+dwTest+1, nSrcLen-dwTest);
					*(&(*(WORD*)wszSrc)+dwSub) = dwTest - 2 - dwSub;
					*(&(*(WORD*)wszSrc)+dwSub+1) = 0;
					*(&(*(WORD*)wszSrc)+dwTest) = 0;
					*(&(*(WORD*)wszSrc)+dwTest+1) = 0;
					dwSub = dwTest;
					nSrcLen ++;
					dwTest ++;
					dwLen ++;
				}
				dwTest ++;
			}
			*(&(*(WORD*)wszSrc)+dwSub) = dwTest - 2 - dwSub;
			*(&(*(WORD*)wszSrc)+dwSub+1) = 0;
			*(&(*(WORD*)wszSrc)+nSrcLen) = 0;

			*(&(*(DWORD*)pMain)) = dwLen;

			// 메모리 덮어쓰기면 포인터 복구 금지
			*(&(*(DWORD*)pBackUp)+3) = 0;

			memcpy(*(DWORD**)pMain+1, wszSrc, nSrcLen*2);
		}
		else	// MBCS
		{
			LPSTR szSrc = (LPSTR)pSrc;
			UINT_PTR nSrcLen =  strlen(szSrc);
			LPSTR szDest = (LPSTR)pDest;
			UINT_PTR nDestLen = strlen(szDest);

			BYTE dwTest=0;
			BYTE szBuf;
			BYTE dwSub=0;
			BYTE dwLen=1;
 
			memmove(szSrc+4, szSrc, nSrcLen);
			*(&(*(DWORD*)szSrc)) = 0;
			nSrcLen += 4;
			
			while(dwTest < nSrcLen)
			{
				szBuf = *(&(*(BYTE*)szSrc)+dwTest);
				if(szBuf >= 0x80 && !((*(&(*(BYTE*)szSrc)+dwTest+1) == 0xFF) && szBuf == 0x9F))
					dwTest ++;
				else if(szBuf == 0x0A)
				{
					memmove(szSrc+dwTest+3, szSrc+dwTest, nSrcLen-dwTest);
					*(&(*(BYTE*)szSrc)+dwSub) = dwTest - 4 - dwSub;
					*(&(*(BYTE*)szSrc)+dwSub+1) = 0;
					*(&(*(BYTE*)szSrc)+dwSub+2) = 0;
					*(&(*(BYTE*)szSrc)+dwSub+3) = 0;
					*(&(*(BYTE*)szSrc)+dwTest) = 0;
					*(&(*(BYTE*)szSrc)+dwTest+1) = 0;
					*(&(*(BYTE*)szSrc)+dwTest+2) = 0;
					*(&(*(BYTE*)szSrc)+dwTest+3) = 0;
					dwSub = dwTest;
					nSrcLen += 3;
					dwTest += 4;
					dwLen ++;
				}
				else if((*(&(*(BYTE*)szSrc)+dwTest+1) == 0xFF) && szBuf == 0x9F)
				{
					memmove(szSrc+dwTest+2, szSrc+dwTest, nSrcLen-dwTest);
					*(&(*(BYTE*)szSrc)+dwSub) = dwTest - 4 - dwSub;
					*(&(*(BYTE*)szSrc)+dwSub+1) = 0;
					*(&(*(BYTE*)szSrc)+dwSub+2) = 0;
					*(&(*(BYTE*)szSrc)+dwSub+3) = 0;
					*(&(*(BYTE*)szSrc)+dwTest) = 0;
					*(&(*(BYTE*)szSrc)+dwTest+1) = 0;
					*(&(*(BYTE*)szSrc)+dwTest+2) = 0;
					*(&(*(BYTE*)szSrc)+dwTest+3) = 0;
					dwSub = dwTest;
					nSrcLen += 2;
					dwTest += 3;
					dwLen ++;
				}
				dwTest ++;
			}
			*(&(*(BYTE*)szSrc)+dwSub) = dwTest - 4 - dwSub;
			*(&(*(BYTE*)szSrc)+dwSub+1) = 0;
			*(&(*(BYTE*)szSrc)+dwSub+2) = 0;
			*(&(*(BYTE*)szSrc)+dwSub+3) = 0;
			*(&(*(BYTE*)szSrc)+nSrcLen) = 0;

			*(&(*(DWORD*)pMain)) = dwLen;

			// 메모리 덮어쓰기면 포인터 복구 금지
			*(&(*(DWORD*)pBackUp)+3) = 0;

			memcpy((DWORD*)pMain+1, szSrc, nSrcLen+1);
		}
	}
	else if (m_bEndPos)
	{
		// 포인터를 교체하기전 미리 백업해둔다
		SetUpTextPoint(pBackUp, (&(*(DWORD*)(*(&(*(DWORD*)pMain))))), (&(*(DWORD*)pMain)), m_nBackUpSize);

		if(m_bUnicode)
		{
			LPWSTR wszSrc = (LPWSTR)pSrc;
			sLen = wcslen(wszSrc);
			size_t wszJpnLen = 0;
			
			if (m_bIgnoreBufLen)
			{
				// 메모리 덮어쓰기면 포인터 복구 금지
				*(&(*(DWORD*)pBackUp)+3) = 0;

				if(*(&(*(DWORD*)pMain)+1) - *(&(*(DWORD*)pMain)) == sLen)
					memcpy(*(WCHAR **)pMain, wszSrc, sLen);
				else
				{
					wszJpnLen = wcslen((wchar_t*)*(&(*(DWORD*)pMain)+1));
					memmove(*(WCHAR **)pMain+sLen, (wchar_t*)*(&(*(DWORD*)pMain)+1), wszJpnLen);
					*(&(*(WORD*)(*(&(*(DWORD*)pMain))))+wszJpnLen+sLen) = 0x00;
					memcpy(*(WCHAR **)pMain, wszSrc, sLen);
					*(&(*(DWORD*)pMain)+1) = (DWORD)(*(WCHAR **)pMain+sLen);
				}
			}
			else
			{
				*(WCHAR **)pMain = wszSrc;
				*(&(*(DWORD*)pMain)+1) = (DWORD)(wszSrc+sLen);
			}
		}
		else	// MBCS
		{
			LPSTR szSrc = (LPSTR)pSrc;
			sLen = strlen(szSrc);
			size_t szJpnLen = 0;

			if (m_bIgnoreBufLen)
			{
				// 메모리 덮어쓰기면 포인터 복구 금지
				*(&(*(DWORD*)pBackUp)+3) = 0;

				if(*(&(*(DWORD*)pMain)+1) - *(&(*(DWORD*)pMain)) == sLen)
					memcpy(*(CHAR **)pMain, szSrc, sLen);
				else
				{
					szJpnLen = strlen((char*)*(&(*(DWORD*)pMain)+1));
					memmove(*(CHAR **)pMain+sLen, (char*)*(&(*(DWORD*)pMain)+1), szJpnLen);
					*(&(*(BYTE*)(*(&(*(DWORD*)pMain))))+szJpnLen+sLen) = 0x00;
					memcpy(*(CHAR **)pMain, szSrc, sLen);
					*(&(*(DWORD*)pMain)+1) = (DWORD)(*(CHAR **)pMain+sLen);
				}
			}
			else
			{
				*(CHAR **)pMain = szSrc;
				*(&(*(DWORD*)pMain)+1) = (DWORD)(szSrc+sLen);
			}
		}
	}

	else if (m_bTjsStr)
	{
		if(*(&(*(int*)pMain)) == 0x2) m_nBackUpSize = 0;

		// 포인터를 교체하기전 미리 백업해둔다
		SetUpTextPoint(pBackUp, (&(*(DWORD*)(*(&(*(DWORD*)pMain)+1)))), (&(*(DWORD*)pMain)+1), m_nBackUpSize);

		if(m_bUnicode)
		{
			LPWSTR wszSrc = (LPWSTR)pSrc;
			sLen =  wcslen(wszSrc);
			if(sLen >= 10)
				*(&(*(DWORD*)pMain)+1) = (DWORD)pSrc;
			else
			{
				*(&(*(DWORD*)pMain)+1) = 0;
				memcpy((DWORD*)pMain+2, wszSrc, (sLen+1)*2);
			}
		}
		else
			*(&(*(DWORD*)pMain)+1) = (DWORD)pSrc;
	}

	else if(m_bNullchar)	// Nullchar
	{
		LPWSTR wszSrc = (LPWSTR)pSrc;
		UINT_PTR nSrcLen =  wcslen(wszSrc);
		LPWSTR wszDest = (LPWSTR)pDest;
		// MATCH 는 OVERWRITE의 IGNORE 의 반대개념이므로 순서가 반대
		UINT_PTR nDestLen = (m_bMatchSize ? wcslen(wszDest) : nSrcLen);	

		NOTIFY_DEBUG_MESSAGE(_T("SMSTR: ApplySmartString: wszSrc =%s\n"), wszSrc);
		NOTIFY_DEBUG_MESSAGE(_T("SMSTR: ApplySmartString: wszDest=%s\n"), wszDest);

		// 원래 긴 문자열이었는지 체크
		if ((*(&(*(DWORD*)pMain)+4) >= 0x4) || m_bForcePtr)
			bWasLongMode = TRUE;
		else
			bWasLongMode = FALSE;

		// 문자열 포인터가 잘못되었다면 리턴
		if( IsBadWritePtr(wszDest, nDestLen) || IsBadStringPtrW(wszDest, 1024*1024*1024) ) return FALSE;

		// 포인터를 교체하기전 미리 백업해둔다
		SetUpTextPoint(pBackUp, (&(*(DWORD*)(*(&(*(DWORD*)pMain))))), (&(*(DWORD*)pMain)), m_nBackUpSize);

		if(m_bReverse)
		{
			if(m_bUnicode)	// UnicodeRev
			{
				BYTE dwTest=0;
				BYTE wszBuf;
				while(dwTest < nDestLen){
					wszBuf = *(&(*(BYTE*)wszSrc)+dwTest);
					*(&(*(BYTE*)wszSrc)+dwTest) = *(&(*(BYTE*)wszSrc)+dwTest+1);
					*(&(*(BYTE*)wszSrc)+dwTest+1) = wszBuf;
					dwTest+= 2;
				}
			}
			else
			{
				BYTE dwTest=0;
				BYTE wszBuf;
				BYTE dwSub=0;
				while(dwTest < nDestLen){
					wszBuf = *(&(*(BYTE*)wszSrc)+dwTest);
					if((*(&(*(BYTE*)wszSrc)+dwTest+1) == 0x0a) && wszBuf == 0x0d)
					{
						*(&(*(BYTE*)wszSrc)+dwTest) = *(&(*(BYTE*)wszSrc)+dwTest+1);
						*(&(*(BYTE*)wszSrc)+dwTest+1) = 0x00;
					}
					else if(wszBuf < 0x80 && wszBuf != 0x0d)
					{
						dwSub = nDestLen - dwTest;
						*(&(*(BYTE*)wszSrc)+nDestLen) = 0x00;
						*(&(*(BYTE*)wszSrc)+nDestLen+1) = 0x00;
						while(dwSub > 0)
						{
							*(&(*(BYTE*)wszSrc)+dwTest+dwSub) = *(&(*(BYTE*)wszSrc)+dwTest+dwSub-1);
							dwSub--;
						}
						*(&(*(BYTE*)wszSrc)+dwTest+dwSub+1) = 0x00;
						nDestLen++;
					}
					else 
					{
						*(&(*(BYTE*)wszSrc)+dwTest) = *(&(*(BYTE*)wszSrc)+dwTest+1);
						*(&(*(BYTE*)wszSrc)+dwTest+1) = wszBuf;
					}
					dwTest+=2;
				}
				*(&(*(BYTE*)wszSrc)+dwTest) = 0x00;
			}
		}

		// dwLen 자리에 바뀐 문자길이를 넣고
		*(&(*(DWORD*)pMain)+4) = nDestLen;

		// i 값 보정
		int i=1;
		sLen = (DWORD)nDestLen;

		// 다른 방식
		i=(sLen/0x4+1) * 0x4 - 1;

		// 변경된 i 값도 넣고
		*(&(*(DWORD*)pMain)+5) = i;

		sLen = (DWORD)nDestLen;
		if((sLen >= 0x4) || m_bForcePtr){
			// 긴 문자열

			NOTIFY_DEBUG_MESSAGE(_T("SMSTR: ApplySmartString: Unicode long\n"));

			// 기존 내용 삭제
			// ZeroMemory(pMain, sizeof(DWORD) * 4);

			// 번역문 적용
			if (m_bMatchSize)	// 길이 맞춤
			{
				// 메모리 덮어쓰기면 포인터 복구 금지
				*(&(*(DWORD*)pBackUp)+3) = 0;

				size_t len = min(nDestLen, nSrcLen);
				memcpy(wszDest, wszSrc, (len+1)*sizeof(wchar_t));
				while(len<nDestLen)
				{
					wszDest[len] = L' ';
					len++;
				}
			}
			else if (m_bIgnoreBufLen)	// 버퍼길이 무시
			{
				if (bWasLongMode)	// 원래 사용하던 버퍼가 있다
				{
					// 메모리 덮어쓰기면 포인터 복구 금지
					*(&(*(DWORD*)pBackUp)+3) = 0;

					lstrcpyW(wszDest, wszSrc);	// 버퍼에 복사
				}
				else	// 원래 짧은 문자열임 (버퍼 없음)
				{
					// 버퍼 변경
					*(WCHAR **)pMain = wszSrc;
				}
			}
			else	// 기본 동작
			{
				// 버퍼 변경
				*(WCHAR **)pMain = wszSrc;
			}

			// 번역문을 출력 형태에 맞게 늘리기
			*(&(*(DWORD*)(*(&(*(DWORD*)pMain))))+nDestLen) = NULL;
			while(nDestLen > 0){
				nDestLen--;
				*(&(*(DWORD*)(*(&(*(DWORD*)pMain))))+nDestLen) = *(&(*(WORD*)(*(&(*(DWORD*)pMain))))+nDestLen);
			}
		}
		else
		{
			// 짧은 문자열

			NOTIFY_DEBUG_MESSAGE(_T("SMSTR: ApplySmartString: Unicode short\n"));

			// 기존 내용 삭제
			ZeroMemory(pMain, sizeof(DWORD) * 4);

			// 번역문 적용
			if (m_bMatchSize)	// 길이 맞춤
			{
				size_t len = min(nDestLen, nSrcLen);
				memcpy(wszDest, wszSrc, (len+1)*sizeof(wchar_t));
				while(len<nDestLen)
				{
					wszDest[len] = L' ';
					len++;
				}
			}
			else	// 버퍼길이 무시 & 기본 동작 동일
			{
				// 문자열 복사
				lstrcpyW((WCHAR *)pMain, wszSrc);
			}

			// 번역문을 출력 형태에 맞게 늘리기
			*(&(*(DWORD*)pMain)+nDestLen) = NULL;
			while(nDestLen > 0){
				nDestLen--;
				*(&(*(DWORD*)pMain)+nDestLen) = *(&(*(WORD*)pMain)+nDestLen);
			}
		}
	}
	else
	{
		if(m_bYuris)
		{
			if(m_bUnicode)
			{
				LPWSTR wszSrc = (LPWSTR)pSrc;
				LPWSTR wszDest = (LPWSTR)pDest;
				UINT_PTR nSrcLen;
				// 유리스에서 글자 크기가 조정될때 원 길이보다 적으면 안된다
				//  번역문 < 원문
				if( wcslen(wszSrc) < wcslen(wszDest) )
				{
					nSrcLen = wcslen(wszDest);
					sLen = wcslen(wszSrc);
			
					while( sLen < (DWORD)nSrcLen )
					{
						*(&(*(WORD*)wszSrc)+sLen/2) = 0x20;
						sLen+=2;
					}
					*(&(*(WORD*)wszSrc)+sLen/2) = 0x00;
				}
				else nSrcLen =  wcslen(wszSrc);
				UINT_PTR nDestLen = nSrcLen;

				NOTIFY_DEBUG_MESSAGE(_T("SMSTR: ApplySmartString: wszSrc =%s\n"), wszSrc);
				NOTIFY_DEBUG_MESSAGE(_T("SMSTR: ApplySmartString: wszDest=%s\n"), wszDest);
				// 문자열 포인터가 잘못되었다면 리턴
				if( IsBadWritePtr(wszDest, nDestLen) || IsBadStringPtrW(wszDest, 1024*1024*1024) ) return FALSE;

				// 포인터를 교체하기전 미리 백업해둔다
				SetUpTextPoint(pBackUp, (&(*(DWORD*)(*(&(*(DWORD*)pMain))))), (&(*(DWORD*)pMain)), m_nBackUpSize);
				
				// dwLen 자리에 바뀐 문자길이를 넣고
				if(m_bYurisP == TRUE)
					*(&(*(DWORD*)pMain)+m_YurisOffset) = (DWORD)nDestLen;
				else return FALSE;

				// i 값 보정
				int i=1;
				sLen = 0;

				NOTIFY_DEBUG_MESSAGE(_T("SMSTR: ApplySmartString: Unicode Yuris\n"));

				if (m_bIgnoreBufLen)	// 버퍼길이 무시
				{
					// 메모리 덮어쓰기면 포인터 복구 금지
					*(&(*(DWORD*)pBackUp)+3) = 0;

					lstrcpyW(wszDest, wszSrc);	// 버퍼에 복사
				}
				else	// 기본 동작
				{
					// 버퍼 변경
					*(WCHAR **)pMain = wszSrc;
				}

				while((sLen * 2) < (DWORD)nDestLen )
				{
					*(&(*(BYTE*)(*(&(*(DWORD*)pMain)+1)))+sLen) = 0x32;
					sLen++;
				}
				*(&(*(BYTE*)(*(&(*(DWORD*)pMain)+1)))+sLen) = 0x00;
				*(&(*(BYTE*)(*(&(*(DWORD*)pMain)+1)))+sLen+1) = 0x00;
				*(&(*(BYTE*)(*(&(*(DWORD*)pMain)+1)))+sLen+2) = 0x00;
				*(&(*(BYTE*)(*(&(*(DWORD*)pMain)+1)))+sLen+3) = 0x00;
				*(&(*(BYTE*)(*(&(*(DWORD*)pMain)+1)))+sLen+4) =0x00;

				// sLen 자리에 바뀐 문자길이를 넣고
				*(&(*(DWORD*)pMain)+m_YurisOffset+1) = sLen;
			}
			else	// MBCS
			{
				LPSTR szSrc = (LPSTR)pSrc;
				LPSTR szDest = (LPSTR)pDest;
				UINT_PTR nSrcLen;
				// 유리스에서 글자 크기가 조정될때 원 길이보다 적으면 안된다
				//  번역문 < 원문
				if( strlen(szSrc) < strlen(szDest) )
				{
					nSrcLen = strlen(szDest);
					sLen = strlen(szSrc);
			
					while( sLen < (DWORD)nSrcLen )
					{
						*(&(*(BYTE*)szSrc)+sLen) = 0x20;
						sLen++;
					}
					*(&(*(BYTE*)szSrc)+sLen) = 0x00;
				}
				else nSrcLen =  strlen(szSrc);
				UINT_PTR nDestLen = nSrcLen;

				#if defined(_DEBUG) || defined(FORCE_DEBUG_LOG)
				WCHAR wszTemp[MAX_TEXT_LENGTH];

				MyMultiByteToWideChar(949, 0, szSrc, -1, wszTemp, MAX_TEXT_LENGTH);
				NOTIFY_DEBUG_MESSAGE(_T("SMSTR: ApplySmartString: szSrc =%s\n"), wszTemp);
				MyMultiByteToWideChar(932, 0, szDest, -1, wszTemp, MAX_TEXT_LENGTH);
				NOTIFY_DEBUG_MESSAGE(_T("SMSTR: ApplySmartString: szDest=%s\n"), wszTemp);
				#endif

				// 문자열 포인터가 잘못되었다면 리턴
				if( IsBadWritePtr(szDest, nDestLen) || IsBadStringPtrA(szDest, 1024*1024*1024) ) return FALSE;

				// 포인터를 교체하기전 미리 백업해둔다
				SetUpTextPoint(pBackUp, (&(*(DWORD*)(*(&(*(DWORD*)pMain))))), (&(*(DWORD*)pMain)), m_nBackUpSize);

				// dwLen 자리에 바뀐 문자길이를 넣고
				if(m_bYurisP == TRUE)
					*(&(*(DWORD*)pMain)+m_YurisOffset) = (DWORD)nDestLen;
				else return FALSE;

				// i 값 보정
				int i=1;
				sLen = 0;

				NOTIFY_DEBUG_MESSAGE(_T("SMSTR: ApplySmartString: ANSI Yuris\n"));
				
				if (m_bIgnoreBufLen)	// 버퍼길이 무시
				{
					// 메모리 덮어쓰기면 포인터 복구 금지
					*(&(*(DWORD*)pBackUp)+3) = 0;

					lstrcpyA(szDest, szSrc);
				}
				else	// 기본 동작
				{
					// 버퍼 변경
					*(CHAR **)pMain = szSrc;
				}

				DWORD dwTest=0;

				while(dwTest < (DWORD)nDestLen )
				{
					if( *(&(*(BYTE*)(*(&(*(DWORD*)pMain))))+dwTest) < 0x80 ||
						(*(&(*(BYTE*)(*(&(*(DWORD*)pMain))))+dwTest) > 0xA0 && *(&(*(BYTE*)(*(&(*(DWORD*)pMain))))+dwTest) < 0xE0) )
						*(&(*(BYTE*)(*(&(*(DWORD*)pMain)+1)))+sLen) = 0x31;
					else 
					{
						*(&(*(BYTE*)(*(&(*(DWORD*)pMain)+1)))+sLen) = 0x32;
						dwTest++;
					}
					dwTest++;
					sLen++;
				}
				*(&(*(BYTE*)(*(&(*(DWORD*)pMain)+1)))+sLen) = 0x00;
				*(&(*(BYTE*)(*(&(*(DWORD*)pMain)+1)))+sLen+1) = 0x00;
				*(&(*(BYTE*)(*(&(*(DWORD*)pMain)+1)))+sLen+2) = 0x00;
				*(&(*(BYTE*)(*(&(*(DWORD*)pMain)+1)))+sLen+3) = 0x00;
				*(&(*(BYTE*)(*(&(*(DWORD*)pMain)+1)))+sLen+4) =0x00;

				// sLen 자리에 바뀐 문자길이를 넣고
				*(&(*(DWORD*)pMain)+m_YurisOffset+1) = sLen;
			}
		}
		else
		{
			if(m_bUnicode)	// Unicode
			{
				LPWSTR wszSrc = (LPWSTR)pSrc;
				UINT_PTR nSrcLen =  wcslen(wszSrc);
				LPWSTR wszDest = (LPWSTR)pDest;
				// MATCH 는 OVERWRITE의 IGNORE 의 반대개념이므로 순서가 반대
				UINT_PTR nDestLen = (m_bMatchSize ? wcslen(wszDest) : nSrcLen);	

				NOTIFY_DEBUG_MESSAGE(_T("SMSTR: ApplySmartString: wszSrc =%s\n"), wszSrc);
				NOTIFY_DEBUG_MESSAGE(_T("SMSTR: ApplySmartString: wszDest=%s\n"), wszDest);

				// 원래 긴 문자열이었는지 체크
				if ((*(&(*(DWORD*)pMain)+4) >= 0x8) || m_bForcePtr)
					bWasLongMode = TRUE;
				else
					bWasLongMode = FALSE;

				// 문자열 포인터가 잘못되었다면 리턴
				if( IsBadWritePtr(wszDest, nDestLen) || IsBadStringPtrW(wszDest, 1024*1024*1024) ) return FALSE;

				// 포인터를 교체하기전 미리 백업해둔다
				SetUpTextPoint(pBackUp, (&(*(DWORD*)(*(&(*(DWORD*)pMain))))), (&(*(DWORD*)pMain)), m_nBackUpSize);

				// dwLen 자리에 바뀐 문자길이를 넣고
				*(&(*(DWORD*)pMain)+4) = nDestLen;

				// i 값 보정
				int i=1;
				sLen = (DWORD)nDestLen;

		/*		// 원래 방식
				while(sLen>=0x8){
					i++;
					sLen-=0x8;
				}
				i=(i*0x8)-0x1;
		*/
				// 다른 방식
				i=(sLen/0x8+1) * 0x8 - 1;

				// 변경된 i 값도 넣고
				*(&(*(DWORD*)pMain)+5) = i;

				sLen = (DWORD)nDestLen;
				if((sLen >= 0x8) || m_bForcePtr){
					// 긴 문자열

					NOTIFY_DEBUG_MESSAGE(_T("SMSTR: ApplySmartString: Unicode long\n"));

					// 기존 내용 삭제
					// ZeroMemory(pMain, sizeof(DWORD) * 4);

					// 번역문 적용
					if (m_bMatchSize)	// 길이 맞춤
					{
						// 메모리 덮어쓰기면 포인터 복구 금지
						*(&(*(DWORD*)pBackUp)+3) = 0;

						size_t len = min(nDestLen, nSrcLen);
						memcpy(wszDest, wszSrc, (len+1)*sizeof(wchar_t));
						while(len<nDestLen)
						{
							wszDest[len] = L' ';
							len++;
						}
					}
					else if (m_bIgnoreBufLen)	// 버퍼길이 무시
					{
						if (bWasLongMode)	// 원래 사용하던 버퍼가 있다
						{
							// 메모리 덮어쓰기면 포인터 복구 금지
							*(&(*(DWORD*)pBackUp)+3) = 0;

							lstrcpyW(wszDest, wszSrc);	// 버퍼에 복사
						}
						else	// 원래 짧은 문자열임 (버퍼 없음)
						{
							// 버퍼 변경
							*(WCHAR **)pMain = wszSrc;
						}
					}
					else	// 기본 동작
					{
						// 버퍼 변경
						*(WCHAR **)pMain = wszSrc;
					}
				}
				else
				{
					// 짧은 문자열

					NOTIFY_DEBUG_MESSAGE(_T("SMSTR: ApplySmartString: Unicode short\n"));

					// 기존 내용 삭제
					ZeroMemory(pMain, sizeof(DWORD) * 4);

					// 번역문 적용
					if (m_bMatchSize)	// 길이 맞춤
					{
						size_t len = min(nDestLen, nSrcLen);
						memcpy(wszDest, wszSrc, (len+1)*sizeof(wchar_t));
						while(len<nDestLen)
						{
							wszDest[len] = L' ';
							len++;
						}
					}
					else	// 버퍼길이 무시 & 기본 동작 동일
					{
						// 문자열 복사
						lstrcpyW((WCHAR *)pMain, wszSrc);
					}
				}

		/*		// 이건 뭐지?
				nDestLen=nSrcLen;
				if(nDestLen==0){
					return TRUE;
				}
		*/
			}
			else	// MBCS
			{

				// MATCH 는 OVERWRITE의 IGNORE 의 반대개념이므로 순서가 반대
				LPSTR szSrc = (LPSTR)pSrc;
				UINT_PTR nSrcLen =  strlen(szSrc);
				LPSTR szDest = (LPSTR)pDest;
				UINT_PTR nDestLen = (m_bMatchSize ? strlen(szDest) : nSrcLen);

				#if defined(_DEBUG) || defined(FORCE_DEBUG_LOG)
				WCHAR wszTemp[MAX_TEXT_LENGTH];

				MyMultiByteToWideChar(949, 0, szSrc, -1, wszTemp, MAX_TEXT_LENGTH);
				NOTIFY_DEBUG_MESSAGE(_T("SMSTR: ApplySmartString: szSrc =%s\n"), wszTemp);
				MyMultiByteToWideChar(932, 0, szDest, -1, wszTemp, MAX_TEXT_LENGTH);
				NOTIFY_DEBUG_MESSAGE(_T("SMSTR: ApplySmartString: szDest=%s\n"), wszTemp);
				#endif

				// 원래 긴 문자열이었는지 체크
				if ((*(&(*(DWORD*)pMain)+4) >= 0x10) || m_bForcePtr)
					bWasLongMode = TRUE;
				else
					bWasLongMode = FALSE;
		
				// 문자열 포인터가 잘못되었다면 리턴
				if( IsBadWritePtr(szDest, nDestLen) || IsBadStringPtrA(szDest, 1024*1024*1024) ) return FALSE;

				// 포인터를 교체하기전 미리 백업해둔다
				SetUpTextPoint(pBackUp, (&(*(DWORD*)(*(&(*(DWORD*)pMain))))), (&(*(DWORD*)pMain)), m_nBackUpSize);

				// dwLen 자리에 바뀐 문자길이를 넣고
				*(&(*(DWORD*)pMain)+4) = nDestLen;

				// i 값 보정
				int i=1;
				sLen = (DWORD)nDestLen;

		/*		// 원래 방식
				while(sLen>=0x10){
					i++;
					sLen-=0x10;
				}
				i=(i*0x10)-0x1;
		*/
				// 다른 방식
				i=(sLen/0x10+1) * 0x10 - 1;

				// 변경된 i 값도 넣고
				*(&(*(DWORD*)pMain)+5) = i;
			
				sLen = (DWORD)nDestLen;
				if((sLen >= 0x10) || m_bForcePtr){
					// 긴 문자열

					NOTIFY_DEBUG_MESSAGE(_T("SMSTR: ApplySmartString: ANSI long\n"));

					// 기존 내용 삭제
					// ZeroMemory(pMain, sizeof(DWORD) * 4);

					// 번역문 적용
					if (m_bMatchSize)	// 길이 맞춤
					{
						// 메모리 덮어쓰기면 포인터 복구 금지
						*(&(*(DWORD*)pBackUp)+3) = 0;

						size_t len = 0;
						while(len<nDestLen && len<nSrcLen)
						{
							size_t addval = 1;
							if( (BYTE)0x80 <= (BYTE)szSrc[len] ) addval = 2;

							if( len + addval > nDestLen ) break;

							len += addval;
						}

						memcpy(szDest, szSrc, (len+1));
						while(len<nDestLen)
						{
							szDest[len] = ' ';
							len++;
						}				

					}
					else if (m_bIgnoreBufLen)	// 버퍼길이 무시
					{
						if (bWasLongMode)	// 원래 사용하던 버퍼가 있다
						{
							// 메모리 덮어쓰기면 포인터 복구 금지
							*(&(*(DWORD*)pBackUp)+3) = 0;

							lstrcpyA(szDest, szSrc);
						}
						else	// 원래 짧은 문자열임 (버퍼 없음)
						{
							// 버퍼 변경
							*(CHAR **)pMain = szSrc;
						}
					}
					else	// 기본 동작
					{
						if(nSrcLen >= 0x10)
						{
							//*(&(*(DWORD*)szSrc)-1) = *(&(*(DWORD*)(*(&(*(DWORD*)pMain))))-1);
							//*(&(*(DWORD*)szSrc)-2) = *(&(*(DWORD*)(*(&(*(DWORD*)pMain))))-2);
						}
						// 버퍼 변경
						*(CHAR **)pMain = szSrc;
					}
				}
				else
				{
					NOTIFY_DEBUG_MESSAGE(_T("SMSTR: ApplySmartString: ANSI short\n"));
					// 짧은 문자열

					// 기존 내용 삭제
					ZeroMemory(pMain, sizeof(DWORD) * 4);

					// 번역문 적용
					if (m_bMatchSize)	// 길이 맞춤
					{
						size_t len = 0;
						while(len<nDestLen && len<nSrcLen)
						{
							size_t addval = 1;
							if( (BYTE)0x80 <= (BYTE)szSrc[len] ) addval = 2;

							if( len + addval > nDestLen ) break;

							len += addval;
						}

						memcpy(szDest, szSrc, (len+1));
						while(len<nDestLen)
						{
							szDest[len] = ' ';
							len++;
						}				

					}
					else	// 버퍼길이 무시 & 기본 동작 동일
					{
						// 문자열 복사
						lstrcpyA((CHAR *)pMain, szSrc);
					}
				}
			}
		}
	}
	NOTIFY_DEBUG_MESSAGE(_T("SMSTR: ApplySmartString: success\n"));
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// 문자열 길이를 번역된 문자열의 길이로 변경합니다. (오버라이드됨)
//
BOOL CTransCommandSMSTR::ChangeLength(void *pOrigBasePtr, CTransScriptParser *pParser, void *pNewBasePtr, void *pBackUp)
{
	return CTransCommand::ChangeLength(m_pArgText, pParser, pNewBasePtr, pBackUp);
}

//////////////////////////////////////////////////////////////////////////
//  번역된 문자열을 원 프로그램에 적용합니다. (오버라이드됨)
//
BOOL CTransCommandSMSTR::ApplyTranslatedText(void *pArgText, PREGISTER_ENTRY pRegisters, CTransScriptParser *pParser, BOOL bIsDuplicated, void *pBackUp)
{
	if (bIsDuplicated)
	{
		if (m_bUnicode || m_bUnicode8)
		{
			if (*(WCHAR *)m_pTransTextBuf == L'\0')
				return FALSE;
		}
		else
		{
			if (*(char *)m_pTransTextBuf == '\0')
				return FALSE;
		}
		// 번역 버퍼에 내용이 있으면 그대로 사용한다.
	}

	NOTIFY_DEBUG_MESSAGE(_T("SMSTR: ApplyTranslatedText: called\n"));

	return ApplySmartString(pArgText, m_pTransTextBuf, pBackUp);
	
}