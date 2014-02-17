#pragma once
#include <vector>
#include <queue>
#include "tstring.h"
#include "util.h"

using namespace std;

class FLFunctionString
{
public:
	FLFunctionString(void);
public:
	~FLFunctionString(void);

	void InitFunc(int Len,int TextOnly,wstring FuncString);
	bool AddText(int Address, string PushText);

	string GetSubText(queue<int>* AddressList);

	string GetFullText();
	string GetOnlyText();

	int Length();
	bool TextOnly();

	void InitText(string FuncSource);

	int FindFunc(string Source,int Start);

private:
	int OptLen;//옵션시 받은 길이
	int Len;//텍스트 포함 실제길이
	int AddressLen;
	vector <int> Type;
	vector <string> Text;
	//Type 구분
	//0 : 함수부분(Hex)
	//1 : 함수부분(문자열)
	//2 : 텍스트, 무시(^)
	//3 : 텍스트, 셈(*)
	//4 : 텍스트, 무시, 미번역(!)

	bool TOnly;

	string::size_type Find(string Source,string Key,string::size_type Start,int CodePage);
};

struct FLOption{
	int ReturnLen;
	int MaxChar;
	int MaxLine;
	int IgnoreLine;
	bool LimitLine;
	int ReturnCharLen;
	string ReturnChar;

	vector <FLFunctionString> OptionFuncStrings;
	//옵션용 여기엔 Len과 Type 2,3,4의 Text가 미완성인 상태,
	//이걸 퍼가서 완성만 하면 된다는 느낌으로[...]
	int OptFuncStringN;

	bool TwoByte;
	bool RemoveSpace;
};
