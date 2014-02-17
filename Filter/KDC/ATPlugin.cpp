// ATPlugin.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "ATPlugin.h"

#include "hash.hpp"

using namespace std;

HWND g_hSettingWnd=NULL;
HINSTANCE g_hThisModule;
LPSTR g_szOptionStringBuffer=NULL;

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			g_hThisModule = (HINSTANCE) hModule;
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}

// 플러그인 export 함수

// 공통
BOOL  __stdcall OnPluginInit(HWND hSettingWnd, LPSTR cszOptionStringBuffer)
{
	g_hSettingWnd = hSettingWnd;
	g_szOptionStringBuffer=cszOptionStringBuffer;

	return TRUE;
}

BOOL  __stdcall OnPluginOption()
{
	
	return TRUE;
}
BOOL  __stdcall OnPluginClose()
{

	return TRUE;
}

//여기서부터 쓰자
UINT64 MakeADVHash(const string& strText);
string EncodeK2J(const string& strKor);
BOOL EncodeK2J(LPCSTR cszKorCode, LPSTR szJpnCode);

map<UINT64,string> wordList;

bool bIsChanged=false;
string strJapanese;

BOOL  __stdcall PreTranslate(LPCSTR cszInJapanese, LPSTR szOutJapanese, int nBufSize)
{
	bIsChanged=false;

	strJapanese=cszInJapanese;
	int nLen=strJapanese.length();

	vector<int> arHeadPosition;
	arHeadPosition.reserve(nLen);

	bool bOnly1Byte=true;
	for(int i=0;i<nLen;i++)
	{
		if(IsDBCSLeadByteEx(932,strJapanese[i]))
		{
			arHeadPosition.push_back(i);
			i++;
			bOnly1Byte=false;
		}
		else
		{
			arHeadPosition.push_back(i);
		}
	}

	arHeadPosition.push_back(nLen);

	if(bOnly1Byte){
		bIsChanged=true;
		memcpy_s(szOutJapanese,nBufSize,cszInJapanese,nLen+1);
		return TRUE;
	}

	vector<char> strBuffer;
	strBuffer.reserve(nLen+4);

	int nCharSize=(int)arHeadPosition.size();
	for(int i=0;i<nCharSize-1;i++)
	{
		int nStart=arHeadPosition[i];

		if(!IsDBCSLeadByteEx(932,strJapanese[nStart]))
		{
			strBuffer.push_back(strJapanese[nStart]);
			continue;
		}

		bool bFound=false;

		for(int j=nCharSize-1;j>i;j--)
		{
			int nEnd=arHeadPosition[j];

			map<UINT64,string>::iterator iter = wordList.find(MakeADVHash(strJapanese.substr(nStart,nEnd-nStart)));
			if(iter!=wordList.end())
			{
				strBuffer.insert(strBuffer.end(),iter->second.begin(),iter->second.end());
				bIsChanged=true;
				bFound=true;
				i=j-1;
				break;
			}
		}

		if(!bFound)
		{
			strBuffer.push_back(strJapanese[nStart]);
			strBuffer.push_back(strJapanese[nStart+1]);
		}
	}

	strBuffer.push_back('\0');
	memcpy_s(szOutJapanese,nBufSize,&strBuffer[0],strBuffer.size());

	// 작업 성공시 TRUE, 실패시 FALSE 반환 
	return TRUE;
}

BOOL  __stdcall PostTranslate(LPCSTR cszInKorean, LPSTR szOutKorean, int nBufSize)
{
	strcpy_s(szOutKorean,nBufSize, cszInKorean);
	
	if(!bIsChanged)
	{
		string strKorean=cszInKorean;
		if(strKorean.length()>2&&strKorean!=strJapanese)
			wordList[MakeADVHash(EncodeK2J(strKorean))]=strJapanese;
	}

	// 작업 성공시 TRUE, 실패시 FALSE 반환 
	return TRUE;
}

UINT64 MakeADVHash(const string& strText)
{
	return (strText[0]*65536*256+strText[1]*65536+strText.length())*0x100000000+MakeStringHash(strText.c_str());
}

string EncodeK2J(const string& strKor)
{

	int nLen=strKor.length();

	vector<char> strSrcBuffer(strKor.begin(),strKor.end());
	vector<char> strBuffer;
	strBuffer.resize(nLen+4,'\0');
	int nTmpIdx=0;

	for(int i=0;i<nLen;i++)
	{
		if(IsDBCSLeadByteEx(949,strKor[i]))
		{
			if(EncodeK2J(&strSrcBuffer[i],&strBuffer[nTmpIdx]))
			{
				nTmpIdx+=2;
			}
			i++;

		}
		else
		{
			strBuffer[i]=strKor[i];
			nTmpIdx++;
		}
	}

	return string(strBuffer.begin(),strBuffer.begin()+nTmpIdx);
}

//Encode함수 ATCode Plugin에서 가져옴
BOOL EncodeK2J(LPCSTR cszKorCode, LPSTR szJpnCode)
{
	BOOL bRetVal = FALSE;

	if(cszKorCode && cszKorCode && 0xA0 <= (BYTE)cszKorCode[0] && 0xA0 <= (BYTE)cszKorCode[1])
	{
		// 889F~9872, 989F~9FFC, E040~EAA4, ED40~EEEC
		// 95~3074, 3119~4535, 16632~18622, 19089~19450
		BYTE hiKorByte = (BYTE)cszKorCode[0];
		BYTE loKorByte = (BYTE)cszKorCode[1];

		// 예외 ('아')
		if(hiKorByte==0xBE && loKorByte==0xC6){ hiKorByte = 0xA8; loKorByte = 0xA1; }
		// 예외 ('렇')
		if(hiKorByte==0xB7 && loKorByte==0xB8){ hiKorByte = 0xA8; loKorByte = 0xA2; }
		// 예외 ('먼')
		if(hiKorByte==0xB8 && loKorByte==0xD5){ hiKorByte = 0xA8; loKorByte = 0xA3; }


		WORD dwKorTmp = ( hiKorByte - 0xA0) * 0xA0;	// 96*96
		dwKorTmp += (loKorByte - 0xA0);		

		dwKorTmp += 95;
		if(dwKorTmp > 3074) dwKorTmp += 44;
		if(dwKorTmp > 4535) dwKorTmp += 12096;
		if(dwKorTmp > 18622) dwKorTmp += 466;
		if(dwKorTmp > 19450) return FALSE;

		BYTE hiByte = (dwKorTmp / 189) + 0x88;
		BYTE loByte = (dwKorTmp % 189) + 0x40;
		dwKorTmp = MAKEWORD( loByte, hiByte );

		if( 0x889F <= dwKorTmp && dwKorTmp <= 0xEEEC )
		{
			szJpnCode[0] = HIBYTE(dwKorTmp);
			szJpnCode[1] = LOBYTE(dwKorTmp);

			bRetVal = TRUE;
		}
	}

	return bRetVal;
}