#pragma once
#include "tstring.h"
#include <vector>
#include <map>

using namespace std;

extern CCriticalSection g_cs;

struct DicWord
{
	int WordN;
	int WordLen;
};

class CAutoLoad
{
public:
	CAutoLoad(void);
public:
	~CAutoLoad(void);

public:
	void StartThread();
	void EndThread();

	void AddDic(LPCWSTR CustomPath);

	map<int,int> GetKeyIndex();
	vector<map<UINT,DicWord>> GetKeyBook();
	vector<string> GetValueList();
	int GetWordN();
	int GetBookN();

	bool GetChanged();
	void SetChanged();

	void FileClear();


	//Load 쓰레드 처리
	bool m_bChanged;
	void CALLBACK Callback();
	static UINT CallbackStub(LPVOID param);
	static UINT CallbackTest(LPVOID param);

private:
	void ResetDic();
	void ReadDic(LPCWSTR CustomPath);

	void SetKey(LPCSTR JpnWord,LPCSTR KorWord);

	//사전 데이터
	map<int, int> KeyIndex;
	vector<map<UINT,DicWord>> KeyBook;

	vector<string> ValueList;

	int WordN;
	int BookN;

	vector<wstring> FileList;
	vector<DWORD> FileWriteTime;
};