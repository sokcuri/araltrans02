#include "stdafx.h"
#include "v8Wrapper.h"
#include "ATJSAPI.h"

using namespace v8;
using namespace std;

wstring g_strNowModuleName=L"";
DWORD g_dwNowAddress=0;

HMODULE g_hAT=NULL;

struct lessWS
{
	bool operator()(const pair<wstring,string>& _Left, const pair<wstring,string>& _Right) const
	{
		int nLCmp = _Left.first.compare(_Right.first);
		if(nLCmp<0)return true;
		else if(nLCmp>0)return false;
		else return _Left.second<_Right.second;
	};
};

map<wstring,DWORD> g_moduleList;
map<DWORD,wstring> g_moduleList2;

map<pair<wstring,string>,FARPROC,lessWS> g_usedFuncList;

PREGISTER_ENTRY g_pRegister;
CONTAINER_FUNCTION_ENTRY g_sAralAPI={0,0,0,0,0,0,0,0,0,0,0,0,0};

//매크로
inline int _TermSize(int nLocale){
	if(nLocale>60000)return 1;
	else if(nLocale>10000)return 4;
	else if(nLocale>1000)return 2;
	else return 1;
}

inline wstring _ToUpper(wstring data){
	wstring strRet=data;
	for(size_t i=0;i<strRet.length();i++){
		wchar_t & wch = strRet[i];
		if(L'a'<=wch && wch<=L'z')wch+='A'-'a';
	}
	return strRet;
}

CONTAINER_FUNCTION_ENTRY& GetAralAPI(){
	if(g_sAralAPI.pfnTranslateText==NULL)SetAralAPI();
	return g_sAralAPI;
}

void SetAralAPI(){
	if(g_hAT==NULL){
		g_hAT = GetModuleHandle(L"ATCTNR.dll");
	}

	if(g_hAT){
		g_sAralAPI.pfnGetCurAlgorithm		= (PROC_GetCurAlgorithm)	 GetProcAddress	(g_hAT, "GetCurAlgorithm");
		g_sAralAPI.pfnGetCurTranslator		= (PROC_GetCurTranslator)	 GetProcAddress	(g_hAT, "GetCurTranslator");
		g_sAralAPI.pfnHookDllFunction		= (PROC_HookDllFunction)	 GetProcAddress	(g_hAT, "HookDllFunction");
		g_sAralAPI.pfnGetOrigDllFunction	= (PROC_GetOrigDllFunction)  GetProcAddress	(g_hAT, "GetOrigDllFunction");
		g_sAralAPI.pfnUnhookDllFunction		= (PROC_UnhookDllFunction)	 GetProcAddress	(g_hAT, "UnhookDllFunction");
		g_sAralAPI.pfnHookCodePoint			= (PROC_HookCodePoint)		 GetProcAddress	(g_hAT, "HookCodePoint");
		g_sAralAPI.pfnUnhookCodePoint		= (PROC_UnhookCodePoint)	 GetProcAddress	(g_hAT, "UnhookCodePoint");
		g_sAralAPI.pfnTranslateText			= (PROC_TranslateText)		 GetProcAddress	(g_hAT, "TranslateText");
		g_sAralAPI.pfnIsAppLocaleLoaded		= (PROC_IsAppLocaleLoaded)	 GetProcAddress	(g_hAT, "IsAppLocaleLoaded");
		g_sAralAPI.pfnEnableAppLocale		= (PROC_EnableAppLocale)	 GetProcAddress	(g_hAT, "EnableAppLocale");
		g_sAralAPI.pfnSuspendAllThread		= (PROC_SuspendAllThread)	 GetProcAddress	(g_hAT, "SuspendAllThread");
		g_sAralAPI.pfnResumeAllThread		= (PROC_ResumeAllThread)	 GetProcAddress	(g_hAT, "ResumeAllThread");
		g_sAralAPI.pfnIsAllThreadSuspended	= (PROC_IsAllThreadSuspended)GetProcAddress (g_hAT, "IsAllThreadSuspended");
	}
}

void SetBasicData(LPVOID pAddress,PVOID pRegister){
	g_dwNowAddress = (DWORD)pAddress;
	g_pRegister=(PREGISTER_ENTRY)pRegister;

}

void	MakeModuleList(){
	g_moduleList.clear();
	g_moduleList2.clear();

	g_moduleList.insert(make_pair(wstring(L""),0));
	g_moduleList2.insert(make_pair(0,wstring(L"")));

	MODULEENTRY32 sTmp;
	
	// 프로세스 스냅샷 핸들을 생성
	HANDLE hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetCurrentProcessId());
	if(INVALID_HANDLE_VALUE == hModuleSnap) return;

	sTmp.dwSize = sizeof(MODULEENTRY32);
	BOOL bExist = Module32First(hModuleSnap, &sTmp);

	while( bExist != FALSE)
	{
		wstring strData = sTmp.szModule;
		DWORD dwData = (DWORD)sTmp.hModule;
		g_moduleList.insert(make_pair(strData,dwData));
		g_moduleList2.insert(make_pair(dwData,strData));

		sTmp.dwSize = sizeof(MODULEENTRY32);
		bExist = Module32Next(hModuleSnap, &sTmp);
	}

	CloseHandle (hModuleSnap);
}


DWORD RawGetModuleAddress(wstring strModName){
	if(strModName==L"")return 0;

	auto iter = g_moduleList.find(strModName);

	if(iter!=g_moduleList.end()){
		return (DWORD)iter->second;
	}

	MakeModuleList();

	iter = g_moduleList.find(strModName);

	if(iter!=g_moduleList.end()){
		return (DWORD)iter->second;
	}

	return 0;
}

DWORD RawGetAddressHere(){
	return g_dwNowAddress;
}

wstring RawGetModuleNameHere(){
	DWORD dwAddress = g_dwNowAddress;

	if(g_moduleList2.size()<2){
		MakeModuleList();
	}

	auto iter = g_moduleList2.lower_bound(dwAddress);
	return iter->second;
}

bool RawReadMemory(DWORD dwAddress,BArray& arData,size_t uLen){
	arData.resize(uLen,0);
	memcpy_s(arData.data(),uLen,(const void*)dwAddress,uLen);

	return true;
}

bool RawSaveMemory(BArray& arData,DWORD dwAddress){
	memcpy_s((void *)dwAddress,arData.size(),arData.data(),arData.size());
	return true;
}

DWORD RawAllocMemory(size_t uLen){
	BYTE * pRet = new BYTE[uLen];
	return (DWORD)pRet;
}

DWORD	RawFreeMemory(int nType,DWORD dwAddress,DWORD dwOpt,size_t uLen){
	DWORD dwRet=0;
	try{
		switch(nType){
		case 0:
			delete[] (void*)dwAddress;
			dwRet=1;
			break;
		case 1:
			delete (void*)dwAddress;
			dwRet=1;
			break;
		case 2:
			free((void*)dwAddress);
			dwRet=1;
			break;
		case 3:
			dwRet = (DWORD)HeapFree((HANDLE)dwOpt,0,(LPVOID)dwAddress);
			break;
		case 4:
			dwRet = (DWORD)LocalFree((HLOCAL)dwAddress);
			break;
		case 5:
			dwRet = (DWORD)GlobalFree((HGLOBAL)dwAddress);
			break;
		case 6:
			dwRet = VirtualFree((LPVOID)dwAddress,uLen,dwOpt);
			break;
		}
	}
	catch(...){
		dwRet=0;
	}
	return dwRet;
}

bool	RawProtectMemory(DWORD dwAddress,size_t uLen,DWORD dwProtect,DWORD& dwOld){
	return VirtualProtect((LPVOID)dwAddress,uLen,dwProtect,&dwOld)!=FALSE;
}

//레지스터 작업
bool	RawGetRegister(int nRegister,DWORD& dwValue){
	switch(nRegister){
	case 0:
		dwValue = g_pRegister->_EAX;
	case 1:
		dwValue = g_pRegister->_EBX;
	case 2:
		dwValue = g_pRegister->_ECX;
	case 3:
		dwValue = g_pRegister->_EDX;
	case 4:
		dwValue = g_pRegister->_ESI;
	case 5:
		dwValue = g_pRegister->_EDI;
	case 6:
		dwValue = g_pRegister->_ESP;
	case 7:
		dwValue = g_pRegister->_EBP;
	case 8:
		dwValue = g_pRegister->_EFL;
	default:
		return false;
	}
	return true;
}
bool	RawSetRegister(DWORD dwValue,int nRegister){
	DWORD* dRegister=NULL;
	switch(nRegister){
	case 0:
		dRegister = &g_pRegister->_EAX;
	case 1:
		dRegister = &g_pRegister->_EBX;
	case 2:
		dRegister = &g_pRegister->_ECX;
	case 3:
		dRegister = &g_pRegister->_EDX;
	case 4:
		dRegister = &g_pRegister->_ESI;
	case 5:
		dRegister = &g_pRegister->_EDI;
	case 6:
		dRegister = &g_pRegister->_ESP;
	case 7:
		dRegister = &g_pRegister->_EBP;
	case 8:
		dRegister = &g_pRegister->_EFL;
	default:
		return false;
	}
	*dRegister = dwValue;
	return true;
}

bool	RawGetFlag(int nFlag,bool & bValue){
	vector<bool> arFlags(32); //vector<bool>는 1비트 여러개로 이루어져 있다는 사실을 기억하자!
	DWORD * uTmp = (DWORD*)&arFlags[0];
	*uTmp = g_pRegister->_EFL;

	if(nFlag<0||nFlag>=32){
		return false;
	}

	bValue = arFlags[nFlag];

	return true;
}
bool	RawSetFlag(bool bValue,int nFlag){
	vector<bool> arFlags(32);
	DWORD * uTmp = (DWORD*)&arFlags[0];
	*uTmp = g_pRegister->_EFL;

	if(nFlag<0||nFlag>=32){
		return false;
	}

	arFlags[nFlag]=bValue;

	g_pRegister->_EFL = *uTmp;

	return true;
}

//변환 함수
int64_t	RawArrayToInt(BArray& arData,bool bSigned){
	int64_t nRet=0;
	int nLen=arData.size();

	if(nLen==1){
		if(bSigned){
			nRet = *(signed char*)(arData.data());
		}
		else{
			nRet = *(unsigned char*)(arData.data());
		}
	}
	else if(nLen==2){
		if(bSigned){
			nRet = *(signed short*)(arData.data());
		}
		else{
			nRet = *(unsigned short*)(arData.data());
		}
	}
	else if(nLen>=4){
		if(bSigned){
			nRet = *(signed int*)(arData.data());
		}
		else{
			nRet = *(unsigned int*)(arData.data());
		}
	}

	return nRet;
}
double	RawArrayToFloat(BArray& arData){
	double dRet=0;

	int nLen=arData.size();
	if(nLen==4){
		dRet = *(float*)(arData.data());
	}
	else if(nLen>=8){
		dRet = *(double*)(arData.data());
	}

	return dRet;
}
wstring	RawArrayToString(BArray& arData,int nLocale,BArray arTerm){
	{
		size_t nMax=arData.size()-arTerm.size();
		for(size_t i=0;i<=nMax;i++)
		{
			bool bOn=true;
			for(size_t j=0;j<=arTerm.size();j++){
				if(arTerm[j]!=arData[i+j]){
					bOn=false;
					break;
				}
			}
			if(bOn){
				arData.resize(i);
				break;
			}
		}
	}
	arData.resize(arData.size()+2,0);

	vector<wchar_t> arStr(arData.size());

	MyMultiByteToWideChar(nLocale,0,(LPCSTR)arData.data(),-1,arStr.data(),arStr.size());

	return wstring(arStr.data());
}

BArray	RawIntToArray(int64_t nValue,size_t uLen){
	BArray arRet(8);
	int64_t * pnTmp = (int64_t*)arRet.data();
	*pnTmp = nValue;

	arRet.resize(uLen);

	return arRet;
}
BArray	RawFloatToArray(double dValue,size_t uLen){
	BArray arRet;
	if(uLen==4){
		arRet.resize(4);
		float * pfTmp = (float*)arRet.data();
		*pfTmp = (float)dValue;
	}
	else if(uLen==8){
		arRet.resize(8);
		double * pdTmp = (double*)arRet.data();
		*pdTmp = dValue;
	}

	return arRet;
}
BArray	RawStringToArray(wstring strData,int nLocale,BArray arTerm){
	BArray arRet(strData.size()*3);

	int nTotalLen = MyWideCharToMultiByte(nLocale,0,strData.c_str(),-1,(LPSTR)arRet.data(),arRet.size(),0,0);

	arRet.resize(nTotalLen-_TermSize(nLocale));
	arRet.insert(arRet.end(),arTerm.begin(),arTerm.end());

	return arRet;
}


//기본 비트 함수

int64_t	RawNOT(int64_t nValue,size_t uLen){
	int64_t nRet=0;
	if(uLen==1){
		uint8_t nTmp= (uint8_t)nValue;
		nTmp = ~nTmp;
		nRet=nTmp;
	}
	else if(uLen==2){
		uint16_t nTmp=(uint16_t)nValue;
		nTmp = ~nTmp;
		nRet = nTmp;
	}
	else if(uLen==4){
		uint32_t nTmp=(uint32_t)nValue;
		nTmp = ~nTmp;
		nRet = nTmp;
	}

	return nRet;
}
int64_t	RawOR(int64_t nValue1,int64_t nValue2,size_t uLen){
	int64_t nRet=0;
	if(uLen==1){
		uint8_t nTmp1 = (uint8_t)nValue1;
		uint8_t nTmp2 = (uint8_t)nValue2;
		nTmp2 |= nTmp1;
		nRet=nTmp2;
	}
	else if(uLen==2){
		uint16_t nTmp1 = (uint16_t)nValue1;
		uint16_t nTmp2 = (uint16_t)nValue2;
		nTmp2 |= nTmp1;
		nRet=nTmp2;
	}
	else if(uLen==4){
		DWORD nTmp1 = (DWORD)nValue1;
		DWORD nTmp2 = (DWORD)nValue2;
		nTmp2 |= nTmp1;
		nRet=nTmp2;
	}

	return nRet;
}
int64_t	RawAND(int64_t nValue1,int64_t nValue2,size_t uLen){
	int64_t nRet=0;
	if(uLen==1){
		uint8_t nTmp1 = (uint8_t)nValue1;
		uint8_t nTmp2 = (uint8_t)nValue2;
		nTmp2 &= nTmp1;
		nRet=nTmp2;
	}
	else if(uLen==2){
		uint16_t nTmp1 = (uint16_t)nValue1;
		uint16_t nTmp2 = (uint16_t)nValue2;
		nTmp2 &= nTmp1;
		nRet=nTmp2;
	}
	else if(uLen==4){
		DWORD nTmp1 = (DWORD)nValue1;
		DWORD nTmp2 = (DWORD)nValue2;
		nTmp2 &= nTmp1;
		nRet=nTmp2;
	}

	return nRet;
}

int64_t	RawShift(int64_t nValue,int nBit,size_t uLen){
	int64_t nRet=0;
	if(uLen==1){
		uint8_t nTmp=(uint8_t)nValue;
		nTmp = nTmp<<nBit;
		nRet=nTmp;
	}
	else if(uLen==2){
		uint16_t nTmp=(uint16_t)nValue;
		nTmp = nTmp<<nBit;
		nRet=nTmp;
	}
	else if(uLen==4){
		DWORD nTmp=(DWORD)nValue;
		nTmp = nTmp<<nBit;
		nRet=nTmp;
	}

	return nRet;
}
int64_t	RawRotate(int64_t nValue,int nBit,size_t uLen){
	int64_t nRet=0;
	if(uLen==1){
		uint8_t nTmp=(uint8_t)nValue;
		nTmp = nTmp<<nBit|nTmp>>(8*uLen-nBit);
		nRet=nTmp;
	}
	else if(uLen==2){
		uint16_t nTmp=(uint16_t)nValue;
		nTmp = nTmp<<nBit|nTmp>>(8*uLen-nBit);
		nRet=nTmp;
	}
	else if(uLen==4){
		DWORD nTmp=(DWORD)nValue;
		nTmp = nTmp<<nBit|nTmp>>(8*uLen-nBit);
		nRet=nTmp;
	}

	return nRet;
}


//파일 작업
bool RawTranslate(string& strData,string& strOut){
	static PROC_TranslateText TransProc = NULL;
	if(TransProc==NULL){
		if(g_sAralAPI.pfnTranslateText==NULL)
			SetAralAPI();
		if(g_sAralAPI.pfnTranslateText==NULL)
			return false;

		TransProc = g_sAralAPI.pfnTranslateText;
	}
	
	vector<char> arBuff(max(strData.size()*10,4096),'\0');

	bool bRet = TransProc(strData.c_str(),arBuff.data(),arBuff.size())!=FALSE;

	if(bRet){
		strOut = (const char*)arBuff.data();
	}

	return bRet;
}


typedef DWORD (CALLBACK* LPFNDLLFUNC)(...);

bool	RawCallWinAPI(wstring strDLLName,string strFuncName,vector<pair<BYTE,BArray>>& arArgs,DWORD& dwRet){
	DWORD ar[20];
	int argc=arArgs.size();
	FARPROC realFunc=NULL;

	HINSTANCE hDLL;               // Handle to DLL

	auto prKey = make_pair(strDLLName,strFuncName);
	auto iter = g_usedFuncList.find(prKey);
	if(iter!=g_usedFuncList.end()){
		realFunc = iter->second;
	}
	else{
		hDLL = GetModuleHandle(strDLLName.c_str());
		if(hDLL==NULL){
			hDLL = LoadLibrary(strDLLName.c_str());
		}
	
		if(hDLL==NULL)return false;

		strFuncName.push_back('W');

		realFunc = (FARPROC)GetProcAddress(hDLL,strFuncName.c_str());

		if(realFunc==NULL){
			strFuncName.pop_back();

			realFunc = (FARPROC)GetProcAddress(hDLL,strFuncName.c_str());
		}

		if(realFunc==NULL)return false;

		g_usedFuncList.insert(make_pair(prKey,realFunc));
	}

	//변수 지정
	for(int i=0;i<argc;i++){
		if(arArgs[i].first==0){
			ar[i]=*(DWORD*)arArgs[i].second.data();
		}
		else{
			ar[i]=(DWORD)arArgs[i].second.data();
		}
	}

	bool runOK=true;

	switch(argc){
		case 0:dwRet=((LPFNDLLFUNC)realFunc)();break;
		case 1:dwRet=((LPFNDLLFUNC)realFunc)(ar[0]);break;
		case 2:dwRet=((LPFNDLLFUNC)realFunc)(ar[0],ar[1]);break;
		case 3:dwRet=((LPFNDLLFUNC)realFunc)(ar[0],ar[1],ar[2]);break;
		case 4:dwRet=((LPFNDLLFUNC)realFunc)(ar[0],ar[1],ar[2],ar[3]);break;
		case 5:dwRet=((LPFNDLLFUNC)realFunc)(ar[0],ar[1],ar[2],ar[3],ar[4]);break;
		case 6:dwRet=((LPFNDLLFUNC)realFunc)(ar[0],ar[1],ar[2],ar[3],ar[4],ar[5]);break;
		case 7:dwRet=((LPFNDLLFUNC)realFunc)(ar[0],ar[1],ar[2],ar[3],ar[4],ar[5],ar[6]);break;
		case 8:dwRet=((LPFNDLLFUNC)realFunc)(ar[0],ar[1],ar[2],ar[3],ar[4],ar[5],ar[6],ar[7]);break;
		case 9:dwRet=((LPFNDLLFUNC)realFunc)(ar[0],ar[1],ar[2],ar[3],ar[4],ar[5],ar[6],ar[7],ar[8]);break;
		case 10:dwRet=((LPFNDLLFUNC)realFunc)(ar[0],ar[1],ar[2],ar[3],ar[4],ar[5],ar[6],ar[7],ar[8],ar[9]);break;
		case 11:dwRet=((LPFNDLLFUNC)realFunc)(ar[0],ar[1],ar[2],ar[3],ar[4],ar[5],ar[6],ar[7],ar[8],ar[9],ar[10]);break;
		case 12:dwRet=((LPFNDLLFUNC)realFunc)(ar[0],ar[1],ar[2],ar[3],ar[4],ar[5],ar[6],ar[7],ar[8],ar[9],ar[10],ar[11]);break;
		case 13:dwRet=((LPFNDLLFUNC)realFunc)(ar[0],ar[1],ar[2],ar[3],ar[4],ar[5],ar[6],ar[7],ar[8],ar[9],ar[10],ar[11],ar[12]);break;
		case 14:dwRet=((LPFNDLLFUNC)realFunc)(ar[0],ar[1],ar[2],ar[3],ar[4],ar[5],ar[6],ar[7],ar[8],ar[9],ar[10],ar[11],ar[12],ar[13]);break;
		case 15:dwRet=((LPFNDLLFUNC)realFunc)(ar[0],ar[1],ar[2],ar[3],ar[4],ar[5],ar[6],ar[7],ar[8],ar[9],ar[10],ar[11],ar[12],ar[13],ar[14]);break;
		case 16:dwRet=((LPFNDLLFUNC)realFunc)(ar[0],ar[1],ar[2],ar[3],ar[4],ar[5],ar[6],ar[7],ar[8],ar[9],ar[10],ar[11],ar[12],ar[13],ar[14],ar[15]);break;
		case 17:dwRet=((LPFNDLLFUNC)realFunc)(ar[0],ar[1],ar[2],ar[3],ar[4],ar[5],ar[6],ar[7],ar[8],ar[9],ar[10],ar[11],ar[12],ar[13],ar[14],ar[15],ar[16]);break;
		case 18:dwRet=((LPFNDLLFUNC)realFunc)(ar[0],ar[1],ar[2],ar[3],ar[4],ar[5],ar[6],ar[7],ar[8],ar[9],ar[10],ar[11],ar[12],ar[13],ar[14],ar[15],ar[16],ar[17]);break;
		case 19:dwRet=((LPFNDLLFUNC)realFunc)(ar[0],ar[1],ar[2],ar[3],ar[4],ar[5],ar[6],ar[7],ar[8],ar[9],ar[10],ar[11],ar[12],ar[13],ar[14],ar[15],ar[16],ar[17],ar[18]);break;
		case 20:dwRet=((LPFNDLLFUNC)realFunc)(ar[0],ar[1],ar[2],ar[3],ar[4],ar[5],ar[6],ar[7],ar[8],ar[9],ar[10],ar[11],ar[12],ar[13],ar[14],ar[15],ar[16],ar[17],ar[18],ar[19]);break;
		default:runOK=false;break;
	}

	return runOK;

}

bool	RawGetFile(wstring strPath,BArray& arData){
	HANDLE hFile = CreateFile(strPath.c_str(),GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if(hFile==INVALID_HANDLE_VALUE){
		return false;
	}

	BArray arTmp(1048796);
	arData.clear();

	while(true){
		DWORD dwRead=1048796;
		ReadFile(hFile,arTmp.data(),1048796,&dwRead,0);
		arData.insert(arData.end(),arTmp.begin(),arTmp.begin()+dwRead);
		if(dwRead<1048796){
			break;
		}
	}

	CloseHandle(hFile);

	return true;
}
bool	RawSetFile(BArray& arData,wstring strPath){
	HANDLE hFile = CreateFile(strPath.c_str(),GENERIC_WRITE,FILE_SHARE_READ,NULL,CREATE_ALWAYS ,FILE_ATTRIBUTE_NORMAL,NULL);
	if(hFile==INVALID_HANDLE_VALUE){
		return false;
	}

	DWORD dTmp = arData.size();
	WriteFile(hFile,arData.data(),arData.size(),&dTmp,0);

	CloseHandle(hFile);

	return true;

}


//경로 관련 함수
wstring	RawGetGameExePath(){
	static wchar_t szGamePath[MAX_PATH]={L'\0',};
	
	if (szGamePath[0] != L'\0')
		return wstring(szGamePath);

	GetModuleFileName(NULL, szGamePath, MAX_PATH);
	
	return wstring(szGamePath);
}
wstring	RawGetATDataPath(){
	static wchar_t szATDataDir[MAX_PATH]={L'\0',};
	int i;
	
	if (szATDataDir[0] != L'\0')
		return wstring(szATDataDir);
	
	wcscpy_s(szATDataDir,MAX_PATH,RawGetGameExePath().c_str());
	
	for(i=wcslen(szATDataDir); i>=0; i--)
	{
		if (szATDataDir[i] == L'\\')
		{
			szATDataDir[i]=L'\0';
			break;
		}
	}

	wcscpy_s(szATDataDir+i,MAX_PATH-i,L"\\ATData");

	CreateDirectory(szATDataDir,NULL);

	return wstring(szATDataDir);
}

wstring	RawGetAralTransPath(){
	static wchar_t szATDirectory[MAX_PATH]={L'\0',};
	int i;
	
	if (szATDirectory[0] !=L'\0')
		return wstring(szATDirectory);

	// 아랄트랜스 설치 디렉토리를 알아온다.
	
	GetModuleFileName(GetModuleHandle(L"ATCTNR.DLL"), szATDirectory, MAX_PATH);
	
	for(i=lstrlen(szATDirectory); i>=0; i--)
	{
		if (szATDirectory[i] == L'\\')
		{
			szATDirectory[i]=L'\0';
			break;
		}
	}
	return wstring(szATDirectory);
}
