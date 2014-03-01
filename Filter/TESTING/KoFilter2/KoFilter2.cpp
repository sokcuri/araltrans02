#include "StdAfx.h"
#include "ATPlugin.h"
#include "CharacterMapper.h"

BOOL CATPluginApp::PreTranslate(LPCSTR cszInJapanese, LPSTR szOutJapanese, int nBufSize)
{
	int nLength, i;

	m_cStringArray.clear();

	string strResult;

	string strEncodedKorean;
	bool bIsKorean = false;

	CCharacterMapper2 cCharMap;

	nLength = lstrlenA(cszInJapanese);

	for (i=0; i<nLength; i++)
	{
		if (cCharMap.IsEncodedText(cszInJapanese+i))
		{
			// EncodeKor2 한글
			if (bIsKorean == false)
			{
				// 첫번째 한글 글자
				bIsKorean = true;
			}
			// 인코딩된 한글을 저장한다
			strEncodedKorean+=cszInJapanese[i];
			i++;
			if (i == nLength) break;
			strEncodedKorean+=cszInJapanese[i];
		}
		else
		{
			if (bIsKorean == true)
			{
				// 인코딩된 한글 끝
				bIsKorean = false;

				int nEncodedLength = strEncodedKorean.size();
				char szEncode[3]={0,}, szDecode[3]={0,};
				string strDecodedKorean;

				// 디코드하고
				for(int idx=0; idx < nEncodedLength; idx+=2)
				{
					szEncode[0]=strEncodedKorean[idx];
					szEncode[1]=strEncodedKorean[idx+1];

					cCharMap.DecodeJ2K(szEncode, szDecode);

					strDecodedKorean += szDecode;
				}

				// 저장한 후 마크
				m_cStringArray.push_back(strDecodedKorean);
				strResult+=ENCODE_MARKER_STRING;
				strEncodedKorean.clear();
			}
			if (IsDBCSLeadByteEx(932, (BYTE)cszInJapanese[i]))
			{
				strResult+= cszInJapanese[i];
				i++;
			}
			if (i == nLength) break;
			strResult+=cszInJapanese[i];
		}
	}

	if (bIsKorean)
	{
		// 문장 마지막이 한글 - 마지막 한글을 저장후 마크
		int nEncodedLength = strEncodedKorean.size();
		char szEncode[3]={0,}, szDecode[3]={0,};
		string strDecodedKorean;

		for(int idx=0; idx < nEncodedLength; idx+=2)
		{
			szEncode[0]=strEncodedKorean[idx];
			szEncode[1]=strEncodedKorean[idx+1];

			cCharMap.DecodeJ2K(szEncode, szDecode);

			strDecodedKorean += szDecode;
		}

		m_cStringArray.push_back(strDecodedKorean);
		strResult+=ENCODE_MARKER_STRING;
	}

	// 인코딩된 한글 이외에 번역할 것이 없으면 번역 포기
	nLength = strResult.size();
	for(i=0; i<nLength; i++)
	{
		if ((BYTE)strResult[i] >= 0x80)
			break;
	}
	if (i == nLength)
		return FALSE;

	lstrcpyA(szOutJapanese, strResult.c_str());


	return TRUE;
}

BOOL CATPluginApp::PostTranslate(LPCSTR cszInKorean, LPSTR szOutKorean, int nBufSize)
{
	if (m_cStringArray.empty())
	{
		// 인코딩 치환할 것이 없으니 단순복사
		lstrcpyA(szOutKorean, cszInKorean);
		return TRUE;
	}
	

	int idxStringId = 0;
	int i, nLength = lstrlenA(cszInKorean);
	string strResult;

	for (i=0; i<nLength; i++)
	{
		if ( (nLength - i >= ENCODE_MARKER_LENGTH) && 
			(CompareStringA(LOCALE_NEUTRAL, NULL, cszInKorean+i, ENCODE_MARKER_LENGTH, ENCODE_MARKER_STRING, ENCODE_MARKER_LENGTH) == CSTR_EQUAL) )
		{
			// 저장 마커를 찾았으니 치환
			strResult+=m_cStringArray[idxStringId];
			idxStringId++;
			i+=ENCODE_MARKER_LENGTH;
			if (i == nLength)
				break;
		}
		strResult+=cszInKorean[i];
	}

	lstrcpyA(szOutKorean, strResult.c_str());

	return TRUE;
}