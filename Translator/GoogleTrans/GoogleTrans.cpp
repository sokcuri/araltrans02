// GoogleTrans.cpp : DLL 응용 프로그램을 위해 내보낸 함수를 정의합니다.
//

#include "stdafx.h"
#include "GoogleTrans.h"
#include "resource.h"
#include "StatusDlg.h"
#include "proxy.h"

using namespace std;

BOOL APIENTRY DllMain( HMODULE hModule,
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


/*
 * 세션 초기화
 */
BOOL __stdcall OnPluginInit(HWND hSettingWnd, LPSTR szOptionStringBuffer){
	g_hSettingWnd = hSettingWnd;
	g_szOptionStringBuffer = szOptionStringBuffer;

	INITCOMMONCONTROLSEX iccex;
	iccex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	iccex.dwICC = ICC_LINK_CLASS|ICC_WIN95_CLASSES;
	BOOL tr;
	tr = InitCommonControlsEx(&iccex);

	//세션 열기
	if(g_hSession==NULL){
		array<wchar_t,500> szUserAgent;
		DWORD dSize=500;
		{
			array<char,500> tmpAgent;
			ObtainUserAgentString(0,tmpAgent.data(),&dSize);
			tmpAgent[dSize]='\0'; //만일을 위해
			MyMultiByteToWideChar(0,0,tmpAgent.data(),-1,szUserAgent.data(),500);
		}

		g_hSession = InternetOpen(szUserAgent.data(),INTERNET_OPEN_TYPE_PRECONFIG,NULL,NULL,0);

		g_hStatusDlg = CreateDialog(g_hThisModule,MAKEINTRESOURCE(IDD_DIALOG1),NULL,StatusProc);
		ShowWindow(g_hStatusDlg,SW_SHOW);

	}
	else{
		if(::IsWindow(g_hSettingWnd)){
			::MessageBox(g_hSettingWnd,L"플러그인이 여러번 로드되었습니다!\r\n예기치 못한 문제가 발생할 수 있습니다!",L"Google Translation Plugin",MB_ICONWARNING);
		}
	}

	return TRUE;
}

/*
 * 옵션 처리, 아직까지는 용도가 없다.
 */
BOOL __stdcall OnPluginOption(){

	if(::IsWindow(g_hSettingWnd)){
		::MessageBox(g_hSettingWnd,L"별다른 옵션을 지원하지 않습니다",L"Google Translation Plugin",MB_ICONINFORMATION);
	}

	return TRUE;
}

/*
 * Session을 닫고 혹시 남아있는 Request 핸들을 모두 닫는다.
 */
BOOL __stdcall OnPluginClose(){

	if(g_hStatusDlg!=NULL){
		DestroyWindow(g_hStatusDlg);
		g_hStatusDlg=NULL;
	}

	//Session 닫기
	if(g_hSession!=NULL){
		InternetCloseHandle(g_hSession);
		g_hSession=NULL;
	}

	//번역 작업이 모두 종료될때까지 최대 10초간 대기
	for(int i=0;i<100;i++){
		if(g_nTransCount<=0){
			g_nTransCount=0;
			break;
		}
		Sleep(100);
	}

	//Request 핸들을 모두 닫는다. 일반적인 상황에선 핸들이 남아선 안된다.
	auto iter = g_hRequests.begin();
	while(iter!=g_hRequests.end()){
		if(*iter != NULL){
			InternetCloseHandle(*iter);
			g_hRequests.erase(iter++); //iterator의 ++기호에 주의
		}
	}

	return TRUE;
}

/*
 * 실제 번역 
 * http://ajax.googleapis.com/ajax/services/language/translate?v=1.0&q=내용(UTF-8)&langpair=ja%7Cko 의 방식으로 GET을 이용해 보낸다.
 * 
 */
BOOL __stdcall Translate(LPCSTR cszJapanese, LPSTR szKorean, int nBufSize){

	int nSrcLen=strlen(cszJapanese);

	//번역 할 필요가 있는지를 확인한다.

	//1바이트로 구성되어있는지 확인한다.
	{
		bool bIs1Byte=true;

		//MBCS인지 즉 글자 중에 0x80 이상인 것이 있는지 확인한다.
		for(int i=0;i<nSrcLen;i++){
			if((BYTE)cszJapanese[i]>=0x80){
				bIs1Byte=false;
				break;
			}
		}

		if(bIs1Byte){
			strcpy_s(szKorean,nBufSize,cszJapanese);
			return TRUE;
		}
	}

	//이미 처리되어 있는지 확인한다.
	UINT uHash = MakeStringHash(cszJapanese);
	{
		auto iter = g_Cache.find(uHash);
		if(iter!=g_Cache.end()){
			strcpy_s(szKorean,nBufSize,iter->second.c_str());
			return TRUE;
		}
	}

	//Session이 열려 있는지 확인한다.
	if(g_hSession==NULL){
		strcpy_s(szKorean,nBufSize,cszJapanese);
		return TRUE;
	}

	g_nTransCount++;

	wstring strJapanese;
	{
		vector<wchar_t> strJpnBuff;
		strJpnBuff.resize(nSrcLen);
		int nLen=MyMultiByteToWideChar(932,0,cszJapanese,-1,strJpnBuff.data(),strJpnBuff.size());
		//여기의 nLen은 실제 배열의 크기이므로 -1을 해야한다.
		nLen--;
		strJapanese.insert(strJapanese.end(),strJpnBuff.begin(),strJpnBuff.begin()+nLen);
	}

	//Status 창으로 원문 전송
	SetJString(strJapanese);

	//안전하지 않은 문자를 미리 변환
	strJapanese = EncodeUnsafeCharactor(strJapanese);

	wstring strUrl;//Http 쿼리

	{
		strUrl=L"http://ajax.googleapis.com/ajax/services/language/translate?v=1.0&q=";
		strUrl+=GetEscapeJSString(strJapanese);
		strUrl+=L"&langpair=ja%7Cko";
	}

	wstring strHeader=L""; //HTTP 헤더, 추가로 전송할 내용이 없다.

	//Request
	HINTERNET hRequest = InternetOpenUrl(g_hSession,strUrl.c_str(),strHeader.c_str(),strHeader.length(),INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE,0);


	if(hRequest==NULL){
		strcpy_s(szKorean,nBufSize,cszJapanese);
	}
	else{
		g_hRequests.insert(hRequest);

		const int cnBuffSize = 8192;

		vector<BYTE> dataBuff;
		{
			array<BYTE,cnBuffSize> tmpBuff;
			BOOL bResult;
			while(true){
				DWORD dw=0;
				bResult = InternetReadFile(hRequest,tmpBuff.data(),cnBuffSize,&dw);

				if(bResult&&dw){
					dataBuff.insert(dataBuff.end(),tmpBuff.begin(),tmpBuff.begin()+dw);
				}
				else{
					break;
				}
			}
		}

		dataBuff.push_back('\0');

		InternetCloseHandle(hRequest);
		g_hRequests.erase(hRequest);
		//수신 종료

		wstring strResponseData;

		//UTF-8 -> UTF-16LE
		{
			vector<wchar_t> transBuff;
			transBuff.resize(dataBuff.size(),L'\0');
			int nLen=MultiByteToWideChar(CP_UTF8,0,(LPCSTR)dataBuff.data(),-1,transBuff.data(),transBuff.size());
			strResponseData.insert(strResponseData.end(),transBuff.begin(),transBuff.begin()+nLen-1);
		}

		//최종 추출
		strResponseData = getTranslatedText(strResponseData);
		strResponseData = DecodeUnsafeCharactor(strResponseData);

		//번역문 등록
		SetKString(strResponseData);

		if(strResponseData.length()==0){
			strcpy_s(szKorean,nBufSize,cszJapanese);
		}
		else{
			MyWideCharToMultiByte(949,0,strResponseData.c_str(),-1,szKorean,nBufSize,NULL,NULL);
		}

		//캐시 등록
		g_Cache.insert(make_pair(uHash,string(szKorean)));

	}

	g_nTransCount--;
	return TRUE;
}


/*
 * FireFox의 urlCharType
 * ECMA 262에 따르면 저 배열에서 &1 (AND 1)을 했을 때 true이면 변환 작업이 필요 없다.
 */
static const BYTE urlCharType[256] =
/*   0 1 2 3 4 5 6 7 8 9 A B C D E F */
{    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,       /* 0x */
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,       /* 1x */
     0,0,0,0,0,0,0,0,0,0,7,4,0,7,7,4,       /* 2x   !"#$%&'()*+,-./  */
     7,7,7,7,7,7,7,7,7,7,0,0,0,0,0,0,       /* 3x  0123456789:;<=>?  */
     7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,       /* 4x  @ABCDEFGHIJKLMNO  */
     7,7,7,7,7,7,7,7,7,7,7,0,0,0,0,7,       /* 5X  PQRSTUVWXYZ[\]^_  */
     0,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,       /* 6x  `abcdefghijklmno  */
     7,7,7,7,7,7,7,7,7,7,7,0,0,0,0,0,       /* 7X  pqrstuvwxyz{\}~  DEL */
     0, };

/*
 * 해당 문자열을 JavaScript의 Escape기능과 동일하게 변경한다
 * ECMA 262를 따르며, urlCharType에 해당하는 문자, %uFFFF, %FF의 구성으로 이루어진다.
 */
wstring GetEscapeJSString(wstring strSource){
	vector<wchar_t> buff;
	buff.reserve(strSource.length()*6+10);

	char buff3[5];
	for(UINT i=0;i<strSource.length();i++){
		unsigned short ch=strSource[i];
		if(ch==L'\0'){
			break;
		}
		else if(ch>=0x256){
			sprintf_s(buff3,5,"%04X",ch);
			buff.push_back(L'%');
			buff.push_back(L'u');
			buff.push_back(buff3[0]);
			buff.push_back(buff3[1]);
			buff.push_back(buff3[2]);
			buff.push_back(buff3[3]);
		}
		else if(urlCharType[ch]&1){
			buff.push_back(ch);
		}
		else{
			sprintf_s(buff3,5,"%02X",ch);
			buff.push_back(L'%');
			buff.push_back(buff3[0]);
			buff.push_back(buff3[1]);
		}
	}

	if(buff.size()==0){
		return wstring();
	}
	else{
		return wstring(buff.begin(),buff.end());
	}
}

/*
 * 어떤 문자가 HEX에 쓰이는 문자인지 판단한다.
 */
bool isHex(wchar_t ch){
	return (('0'<=ch&&ch<='9')||('a'<=ch&&ch<='f')||('A'<=ch&&ch<='F'));
}

/*
 * 00A0, FF와 같이 Hex로 이루어진 문자열을 하나의 문자로 변환한다.
 * 해당하는 구간은 무조건 Hex 문자로 이루어져 있다고 가정한다.
 */
wchar_t convertHexToWchar(wchar_t* psz,int nCount){ //한글자만!
	if(nCount>4)nCount=4;
	wchar_t cRet=0;
	for(int i=0;i<nCount;i++){
		wchar_t ch=psz[i];
		cRet<<=4;
		if(L'0'<=ch&&ch<=L'9'){
			cRet+=ch-'0';
		}
		else if(L'a'<=ch&&ch<=L'f'){
			cRet+=ch-'a'+0xa;
		}
		else if(L'A'<=ch&&ch<=L'F'){
			cRet+=ch-'A'+0xa;
		}
		else{
			cRet>>=8;
			break;
		}

	}
	return cRet;
}

/*
 * 해당 문자열을 JavaScript의 Unescape기능과 동일하게 변경한다
 * \FF \uFFFF 일반문자 의 구성으로 이루어져있다.
 */
wstring GetUnescapeJSString(wstring strSource){
	vector<wchar_t> src;
	src.insert(src.end(),strSource.begin(),strSource.end());

	vector<wchar_t> buff;
	buff.reserve(src.size());

	for(UINT i=0;i<src.size();i++){
		if(strSource[i]==L'\0'){
			break;
		}
		else if(i<=src.size()-6&&
			src[i]=='\\'&&
			src[i+1]=='u'&&
			isHex(src[i+2])&&
			isHex(src[i+3])&&
			isHex(src[i+4])&&
			isHex(src[i+5]))
		{
			buff.push_back(convertHexToWchar(&src[i+2],4));
			i+=6-1;
		}
		else if(i<=src.size()-3&&
			src[i]=='\\'&&
			isHex(src[i+1])&&
			isHex(src[i+2]))
		{
			buff.push_back(convertHexToWchar(&src[i+1],2));
			i+=3-1;
		}
		else{
			buff.push_back(src[i]);
		}
	}

	if(buff.size()==0){
		return wstring();
	}
	else{
		return wstring(buff.begin(),buff.end());
	}
}

/*
 * Google Translation API의 결과물을 파싱하는 함수
 * JSON으로 온 결과물( {"responseData": {"translatedText":"결과물"}, "responseDetails": null, "responseStatus": 200} )을 간단히 쪼갠 후
 * &quot; &amp; &lt; &gt; &nbsp; 에 대해 변환을 수행한 최종 결과물을 반환한다.
 */
wstring getTranslatedText(wstring strSource){

	//Tokenizing

	const wchar_t preTok[]=L"\"translatedText\":\"";
	wstring::size_type preIdx= strSource.find(preTok);

	if(strSource.npos==preIdx)return L"";

	preIdx+=sizeof(preTok)/sizeof(wchar_t)-1;

	wstring::size_type postIdx= strSource.find(L'"',preIdx);

	if(strSource.npos==postIdx)return L"";

	wstring strRawText = strSource.substr(preIdx,postIdx-preIdx);

	//Unescape
	strRawText = GetUnescapeJSString(strRawText);

	//Symbolic HTML entity
	vector<wchar_t> tmpBuff=vector<wchar_t>(strRawText.begin(),strRawText.end());
	vector<wchar_t> tmpRet;
	tmpRet.reserve(tmpBuff.size()*3/2);

	for(UINT i=0;i<tmpBuff.size();i++){
		if(tmpBuff[i]==L'&'){
			if(_wcsnicmp(&tmpBuff[i],L"&quot;",6)==0){
				i+=6-1;
				tmpRet.push_back(L'"');
			}
			else if(_wcsnicmp(&tmpBuff[i],L"&amp;",5)==0){
				i+=5-1;
				tmpRet.push_back(L'&');
			}
			else if(_wcsnicmp(&tmpBuff[i],L"&lt;",4)==0){
				i+=4-1;
				tmpRet.push_back(L'<');
			}
			else if(_wcsnicmp(&tmpBuff[i],L"&gt;",4)==0){
				i+=4-1;
				tmpRet.push_back(L'>');
			}
			else if(_wcsnicmp(&tmpBuff[i],L"&nbsp;",6)==0){
				i+=6-1;
				tmpRet.push_back(L' ');
			}
			else{
				tmpRet.push_back(tmpBuff[i]);
			}
		}
		else{
			tmpRet.push_back(tmpBuff[i]);
		}
	}

	return wstring(tmpRet.begin(),tmpRet.end());
}

/*
 * g_strUnsafeChars에 해당하는 문자를 <tok:#>의 형식으로 변환한다.
 */
wstring EncodeUnsafeCharactor(wstring strSource){
	wstring strRet;
	strRet.reserve(strSource.length()*2);

	wchar_t buff[11]=L"0";

	for(UINT i=0;i<strSource.length();i++){
		UINT idx=g_strUnsafeChars.find(strSource[i]);
		if(idx==g_strUnsafeChars.npos){
			strRet.push_back(strSource[i]);
		}
		else{
			strRet+=g_strPreToken;
			_itow_s(idx,buff,10,10);
			strRet+=buff;
			strRet+=g_strPostToken;
		}
	}

	return strRet;
}

/*
 * <tok:#>의 형식으로 설정된 문자를 g_strUnSafeChars 를 참조해 재 변환한다.
 */
wstring DecodeUnsafeCharactor(wstring strSource){
	wstring strRet;
	strRet.reserve(strSource.length());

	wstring::size_type last_idx = 0;
	wstring::size_type pre_idx = 0;
	wstring::size_type post_idx = 0;

	while(true){
		if(last_idx==strSource.length()){
			break;
		}

		pre_idx = strSource.find(g_strPreToken,last_idx);

		if(pre_idx == strSource.npos){
			strRet.insert(strRet.end(),strSource.begin()+last_idx,strSource.end());
			break;
		}
		
		if(pre_idx>last_idx){
			strRet.insert(strRet.end(),strSource.begin()+last_idx,strSource.begin()+pre_idx);
		}

		last_idx=pre_idx;

		pre_idx+=g_strPreToken.length();

		post_idx = strSource.find(g_strPostToken,pre_idx);

		if(post_idx == strSource.npos){
			strRet.insert(strRet.end(),strSource.begin()+last_idx,strSource.begin()+pre_idx);
			continue;
		}

		while(strRet.length()>0&&strRet.back()==L' '){
			strRet.pop_back();
		}

		wstring strNumber = strSource.substr(pre_idx,post_idx-pre_idx);
		UINT nWord = (UINT)_wtoi(strNumber.c_str());
		if(nWord<g_strUnsafeChars.size()){
			strRet.push_back(g_strUnsafeChars[nWord]);
		}
		else{
			strRet+=strNumber;
		}

		last_idx=post_idx+g_strPostToken.length();

		while(last_idx<strSource.length()&&strSource[last_idx]==L' '){
			last_idx++;
		}

	}

	return strRet;
}
