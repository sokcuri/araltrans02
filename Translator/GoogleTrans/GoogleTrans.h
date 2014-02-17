#pragma once

BOOL __stdcall OnPluginInit(HWND hSettingWnd, LPSTR szOptionStringBuffer);
BOOL __stdcall OnPluginOption();
BOOL __stdcall OnPluginClose();
BOOL __stdcall Translate(LPCSTR cszJapanese, LPSTR szKorean, int nBufSize);


//일반 함수
std::wstring GetQueryInfo(HINTERNET hRequest,DWORD dwInfoLevel);

std::wstring GetEscapeJSString(std::wstring strSource);
std::wstring GetUnescapeJSString(std::wstring strSource);

bool isHex(wchar_t ch);
wchar_t convertHexToWchar(wchar_t* psz,int nCount=4);

std::wstring getTranslatedText(std::wstring strSource);

std::wstring EncodeUnsafeCharactor(std::wstring strSource);
std::wstring DecodeUnsafeCharactor(std::wstring strSource);

HINSTANCE g_hThisModule;

//윈도우 핸들
HWND g_hSettingWnd=NULL;

//옵션 스트링
LPSTR g_szOptionStringBuffer=NULL;


//인터넷 핸들
HINTERNET   g_hSession=NULL;
std::set<HINTERNET> g_hRequests;

volatile int g_nTransCount=0;
volatile bool g_bLock=false;//안써도 되는데 보험 정도로...

std::map<UINT,std::string> g_Cache;

std::wstring g_strUnsafeChars  =L"<>{}＜＞「」【】『』《》";

std::wstring g_strPreToken = L"<tok:";
std::wstring g_strPostToken = L">";
