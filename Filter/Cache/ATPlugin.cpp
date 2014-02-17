#include <Windows.h>
#include <map>
#include <algorithm>
#include <vector>
#include <string>
#include <list>

#include "ATPlugin.h"
#include "mzcrc.h"
#include "Util.h"


using namespace std;

HANDLE g_hFile=NULL;
HWND g_hSettingWnd=NULL;
//동시성 X까! 이건 싱글 파이프라인 전용!

wstring wstrATData;

struct pair_string{
	string jpn;
	string kor;
};

typedef __mz_uint64_t crc64_t;

map<crc64_t,list<pair_string> > g_cacheData;

crc64_t CRC64(const char* data,int size){
	crc64_t uRet=0;

	if(size==0)return uRet;

	uRet = mz_crc64(1,uRet,&data[0],sizeof(BYTE));
	for(int i=1;i<size;i++){
		uRet = mz_crc64(0,uRet,&data[0]+i,sizeof(BYTE));
	}

	return uRet;
}

void Init(){
	g_cacheData.clear();

	g_hFile = CreateFile(wstrATData.c_str(),GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ,0,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
	if(g_hFile==INVALID_HANDLE_VALUE){
		return;
	}
	
	DWORD nLen = GetFileSize(g_hFile,NULL);

	if(nLen==0)return;

	vector<BYTE> arData(nLen);

	ReadFile(g_hFile,&arData[0],nLen,&nLen,0);

	bool bIsJpn=true;

	pair_string tmp_pair;

	for(int i=0;i<nLen;i++){
		char ch = arData[i];
		if(ch=='\0'){
			if(bIsJpn)bIsJpn=false;
			else{
				bIsJpn=true;
				crc64_t crc = CRC64(tmp_pair.jpn.c_str(),tmp_pair.jpn.length());

				g_cacheData[crc].push_back(tmp_pair);
				tmp_pair.jpn.resize(0);
				tmp_pair.kor.resize(0);
			}
		}
		else{
			if(bIsJpn){
				tmp_pair.jpn.push_back(ch);
			}
			else{
				tmp_pair.kor.push_back(ch);
			}
		}
	}

}
void Close(){
	CloseHandle(g_hFile);
}

BOOL  __stdcall OnPluginInit(HWND hSettingWnd, LPSTR szOptionStringBuffer){
	g_hSettingWnd=hSettingWnd;

	wstrATData = GetGameDirectory();
	wstrATData += L"\\ATData";

	MyCreateDirectory(wstrATData.c_str());
	wstrATData+= L"\\Cache.dat";

	szOptionStringBuffer[0]='\0';

	Init();
	return TRUE;
}
BOOL  __stdcall OnPluginOption(){
	if(::MessageBoxW(g_hSettingWnd,L"캐시를 초기화할까요?",L"Cache",MB_YESNO|MB_ICONQUESTION)==IDYES){
		g_cacheData.clear();
		Close();
		DeleteFile(wstrATData.c_str());
		Init();
		::MessageBoxW(g_hSettingWnd,L"초기화 했습니다",L"Cache",MB_OK|MB_ICONASTERISK);
	}
	return TRUE;
}
BOOL  __stdcall OnPluginClose(){
	Close();
	return TRUE;
}

bool bFind=false;
crc64_t tmpCRC=0;
string strJpn;
string strKor;

BOOL  __stdcall PreTranslate(LPCSTR cszInJapanese, LPSTR szOutJapanese, int nBufSize){

	strJpn=cszInJapanese;

	tmpCRC= CRC64(strJpn.c_str(),strJpn.length());

	bFind=false;

	auto iter=g_cacheData.find(tmpCRC);
	if(iter!=g_cacheData.end()){
		for(auto iter2=iter->second.begin();iter2!=iter->second.end();++iter2){
			if(iter2->jpn==strJpn){
				bFind=true;
				strKor=iter2->kor;
				break;
			}
		}
	}

	if(!bFind){
		memcpy(szOutJapanese,cszInJapanese,strJpn.length()+1);
	}
	else{
		szOutJapanese[0]='\0';
	}
	return TRUE;
}

vector <BYTE> arBuff;
BOOL  __stdcall PostTranslate(LPCSTR cszInKorean, LPSTR szOutKorean, int nBufSize){

	if(bFind){
		memcpy(szOutKorean,strKor.c_str(),strKor.length()+1);
	}
	else{
		strcpy_s(szOutKorean,nBufSize,cszInKorean);
		pair_string tmpPair;
		tmpPair.jpn=strJpn;
		tmpPair.kor=cszInKorean;

		if(tmpPair.jpn.length()>0&&tmpPair.kor.length()>0){
			g_cacheData[tmpCRC].push_back(tmpPair);

			int nTmpJpnLen= tmpPair.jpn.length()+1;
			int nTmpKorLen= tmpPair.kor.length()+1;
			arBuff.resize(nTmpJpnLen+nTmpKorLen);
			memcpy(&arBuff[0],			 tmpPair.jpn.c_str(),nTmpJpnLen);
			memcpy(&arBuff[0]+nTmpJpnLen,tmpPair.kor.c_str(),nTmpKorLen);

			DWORD dwLen=arBuff.size();
			if(g_hFile!=INVALID_HANDLE_VALUE)
				WriteFile(g_hFile,&arBuff[0],dwLen,&dwLen,0);
			else
				MessageBoxW(NULL,L"!!",L"!!",0);
		}

	}

	return TRUE;
}