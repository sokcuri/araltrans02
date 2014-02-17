#pragma once
#include "tstring.h"
#include "AutoLoad.h"
#include <vector>
#include <map>
#include <queue>

using namespace std;

class CCustomDic2
{
public:
	CCustomDic2();

	void AddDic(int Mode,LPCWSTR CustomPath);

	void PreCustomDic(LPSTR Dest, LPCSTR Source);
	void PostCustomDic(LPSTR Dest, LPCSTR Source);

	void FileClear();
	void SetMarginMode(bool bNoMargin);

	void Init();
	void End();
private:
	DicWord FindKey(LPCSTR Source,int Start,int End);
	
	string GetValue(int KeyN);

	map<int, int> KeyIndex;
	vector<map<UINT,DicWord>> KeyBook;

	vector<string> ValueList;

	DicWord NullWord; //공백 처리용

	queue<int> KeyList;

	bool m_bNoMargin;

	string RemoveSpace(std::string &strText,bool bHead);

public:
	~CCustomDic2(void);
};
