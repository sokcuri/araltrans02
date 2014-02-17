#include "StdAfx.h"

#include "RLCmd.h"

// debug
extern tstring g_strLogFile;

CRLCmd g_cRLCmd;

BOOL CRLCmd::PreTranslate(LPCSTR cszInJapanese, LPSTR szOutJapanese, int nBufSize)
{
	FILE *fp=NULL;
	
	if (!g_strLogFile.empty())
		fp=_tfopen(g_strLogFile.c_str(), _T("a"));

	string strBody;
	
	m_nSize=lstrlenA(cszInJapanese);
	
	TrimCommand(cszInJapanese, m_strPrefix, strBody, m_strPostfix);
	
	if (fp)
	{
		fprintf(fp, "1st pass\n");
		fprintf(fp,	"Original=%s, size=%d\n",
			cszInJapanese, lstrlenA(cszInJapanese));
		fprintf(fp, "Prefix=%s, Body=%s, Postfix=%s\n",
			m_strPrefix.c_str(), strBody.c_str(), m_strPostfix.c_str());
		if (strBody.empty())
			fprintf(fp, "No body - no 2nd pass\n");
		
		fclose(fp);
	}
	
	lstrcpyA(szOutJapanese, strBody.c_str());
	
	if (strBody.empty())
		return FALSE;

	return TRUE;
}
BOOL CRLCmd::PostTranslate(LPCSTR cszInKorean, LPSTR szOutKorean, int nBufSize)
{
	FILE *fp=NULL;
	
	if (!g_strLogFile.empty())
		fp=_tfopen(g_strLogFile.c_str(), _T("a"));

	string strRestored;
	
	string strOriginal=cszInKorean;
	
	SqueezeAndReplaceSpecialString(strOriginal);
	
	RestoreCommand(m_strPrefix, strOriginal.c_str(), m_strPostfix, strRestored, m_nSize);
	if (fp)
	{
		fprintf(fp, "2nd pass\n");
		fprintf(fp, "Prefix=%s, Body=%s, Postfix=%s\n",
			m_strPrefix.c_str(), cszInKorean, m_strPostfix.c_str());
		fprintf(fp, "Restored=%s, size = %d\n", 
			strRestored.c_str(), strRestored.size());
		fclose(fp);
	}
	
	lstrcpyA(szOutKorean, strRestored.c_str());

	return TRUE;
}

void CRLCmd::TrimCommand(const char *cszOriginal, string &strPrefix, string &strBody, string &strPostfix)
{
	int idxBodyStart, idxBodyEnd;
	int i;
	int nLen;
	string strOriginal = cszOriginal;

	bool bIsInQuote = false;


	if ( (strOriginal[0] == '\"') && (strOriginal[1] == '\"') )
	{
		// 내용없음
		strPrefix=strOriginal;
		strBody.erase();
		strPostfix.erase();
		return;
	}
	nLen=strOriginal.size();
	idxBodyStart=idxBodyEnd=0;

//	FILE *fp=NULL;
//	fp=fopen("c:\\test.txt", "w");

	// BodyStart 를 찾는다.
	for(i=0;i<nLen; i++)
	{
		// 첫번째 2바이트 글자
		if (IsDBCSLeadByteEx(932, (BYTE)strOriginal[i]))
			break;

		if (strOriginal[i] == '\"')
			break;
	}

	if (i >= nLen)
	{
		// 100% 영문
		strPrefix=strOriginal;
		strBody.erase();
		strPostfix.erase();
		return;
	}
	else
	{
		idxBodyStart=i;
	}

//	fprintf(fp, "nBodyStart=%d\n", idxBodyStart);

	// BodyEnd 를 찾는다.
	for (i=idxBodyStart; i<nLen; i++)
	{
//		fprintf (fp, "i=%d, checkchar=0x%02X ", i, (BYTE)strOriginal[i]);
		// 2바이트 글자의 앞부분?
		if (IsDBCSLeadByteEx(932, (BYTE)strOriginal[i]))
		{
			i++;	// 2바이트 글자의 뒷부분으로 이동
//			fprintf(fp, "is DBCSLeadByte\n");
		}
		else
		{
//			fprintf (fp, "is HalfByte %c\n",strOriginal[i]);
			if ( ('A' <= strOriginal[i] ) && (strOriginal[i] <= 'Z') ||
				('a' <= strOriginal[i] ) && (strOriginal[i] <= 'z') )
				continue;

			if ( (strOriginal[i] == ' ') || (strOriginal[i] == '!')
				)
				continue;

			if (strOriginal[i] == '\"')
			{
				bIsInQuote=!bIsInQuote;
//				fprintf(fp, "bIsInQuote=%s\n", (bIsInQuote)?"true":"false");
				continue;
			}

			if (!bIsInQuote)
			{
				idxBodyEnd=i-1;	// idxBodyEnd= 2바이트 글자의 뒷부분
				break;
			}
		}
		
	}

	if (i == nLen)
		idxBodyEnd=nLen-1;

//	fprintf(fp, "nBodyEnd=%d\n", idxBodyEnd);
//	fclose(fp);

	// idxBodyEnd = 마지막 2바이트 글자의 뒷부분

	// 문자열을 잘라낸다. *참고-substr(시작,길이)
	strPrefix=strOriginal.substr(0, idxBodyStart);
	strBody=strOriginal.substr(idxBodyStart, idxBodyEnd-idxBodyStart+1);
	strPostfix=strOriginal.substr(idxBodyEnd+1);

}

void CRLCmd::RestoreCommand(string &strPrefix, const char *cszTranslated, string &strPostfix, string &strRestored, int nMaxSize)
{
	int i;

	// Prefix, Postfix 사용량을 최대크기에서 제거
	nMaxSize= nMaxSize-strPrefix.size()-strPostfix.size();
	strRestored=cszTranslated;

	if ( m_bIsOverwrite && (strRestored.size() >= nMaxSize) )	// 길이가 너무 길다.
	{
		// 커트 위치 검색
		for(i=0; i<nMaxSize; i++)
		{
			// 2바이트 글자의 앞부분?
			if (IsDBCSLeadByteEx(949, (BYTE)strRestored[i]))
			{
				if (i+1 == nMaxSize)
					break;	// 2바이트 글자가 전부 안들어간다 - 해당 글자 무시
				else
					i++;
			}
			else if (i+2 == nMaxSize)	// 2문자밖에 안남았을때
			{
				if (!IsDBCSLeadByteEx(949, (BYTE)strRestored[i+1]))	// 다음이 1바이트 문자
				{
					// 2바이트 빈칸으로 막는다.
					strRestored[i]=(char) 0x0A1;
					strRestored[i+1]=(char) 0x0A1;
				}
			}
			else if (i+1 == nMaxSize)	// 1바이트 문자로 끝난 문자열 - 해당 글자 무시
				break;
		}
		strRestored=strRestored.substr(0,i);	// 커트
	}

	// 길이의 홀짝을 맞춰준다
	if ( (strRestored.size() % 2) != (nMaxSize % 2) )
	{
		// 빈칸을 하나 추가해 문자의 홀짝을 맞춰준다
		for (i=0; i<strRestored.size(); i++)
		{
			if ( (BYTE)strRestored[i] < 0x80)
			{
				strRestored.insert(i+1, 1, ' ');
				break;
			}
		}
		
		// 빈칸이 없음
		if ( (strRestored.size() % 2) != (nMaxSize % 2) )
			strRestored.insert(2, 1, ' ');	// 무조건 넣음
		
	}

	// 이제 strRestored 길이는 nMaxSize 보다 작거나 같다.

	// strRestored 길이가 nMaxSize 보다 작다
	if (strRestored.size() < nMaxSize)
	{
		int nDiff= nMaxSize - strRestored.size();

		for(i=0; i<nDiff-2; i+=2)	// 빈자리 채워넣기
		{
			if (m_bRemoveSpace)
				strRestored+="　";
			else
				strRestored+="  ";
		}
		strRestored+="　";	
	}

	// 복원
	strRestored=strPrefix+strRestored+strPostfix;
}

void CRLCmd::SqueezeAndReplaceSpecialString(string &strData)
{

	int i;

	bool bIsSpecial=false;
	
	string strOriginal=strData;
	strData.erase();
	
	for(i=0; i<strOriginal.size(); i++)
	{
		if (!IsDBCSLeadByteEx(949,(BYTE)strOriginal[i] ) )
		{
			switch (strOriginal[i])
			{
				case '*':
					strData+= (char) 0x081;
					strData+= (char) 0x096;
					bIsSpecial=true;
					break;
				case '%':
					strData+= (char) 0x081;
					strData+= (char) 0x093;
					bIsSpecial=true;
					break;

				case ' ':
					bIsSpecial=false;
					if (!m_bRemoveSpace)
					{
						if ( (strOriginal[i+1] == ',') || (strOriginal[i+1] == ' ') )
							break;
						else
							strData+=strOriginal[i];
					}
					break;

				case '.':
					bIsSpecial=false;
					if (i == strOriginal.size()-1)
						strData+= "．";
					else
						strData+=strOriginal[i];
					break;

				case ',':
					bIsSpecial=false;
					strData+="，";
					if (strOriginal[i+1] == ' ')
						i++;
					break;

				case '!':
					bIsSpecial=false;
					strData+="！";
					break;

				case '\'':	// 홑따옴표, 겹따옴표 무시
				case '\"':
					break;

				default:

					if (bIsSpecial && ('A' <= strOriginal[i]) && (strOriginal[i] <= 'Z'))
					{
						strData+= (char) 0x082;
						strData+= (char) ((BYTE)strOriginal[i] + 0x1F );
					}
					else
					{
						bIsSpecial=false;
						strData+=strOriginal[i];
					}
			}	// switch...
		}
		else
		{
			WORD wch=MAKEWORD(strOriginal[i+1], strOriginal[i]);
			bIsSpecial=false;
			
			if ( wch == 0xA1BC)	// 【
			{
				if (!m_bIsBracket)
				{
					if (!m_bIsOverwrite)
					{
						strData+= (char) 0x081;
						strData+= (char) 0x079;
					}
				}
				else
				{
					strData+=strOriginal[i];
					strData+=strOriginal[i+1];
				}
			}
			else if (wch == 0xA1BD)	// 】
			{
				if (!m_bIsBracket)
				{
					if (!m_bIsOverwrite)
					{
						strData+= (char) 0x081;
						strData+= (char) 0x07A;
					}
					else
					{
						strData+= ' ';
					}
				}
				else
				{
					strData+=strOriginal[i];
					strData+=strOriginal[i+1];
				}

			}
			else
			{
				strData+=strOriginal[i];
				strData+=strOriginal[i+1];
			}
			i++;
		}	// if(...)
	}	// for..

	if (m_bIsNoFreeze && !m_bRemoveSpace)
	{
		// 퓩, 등의 확장완성형 문자 사용시 가끔 게임이 어는 현상을 방지하기 위해 1바이트,2바이트 빈칸을 추가
		strData+=" 　";
	}
}

