#pragma once

struct fileInfo{
	bool split;
	int keyCP;
	int valCP;
	wstring name;
};

struct wordInfoW{
	wstring key;
	wstring value;
	int aux;
};

struct wordInfo{
	string key;
	string value;
	int aux;
	//key -> aux 또는 aux -> value 가능
};

struct TokenStruct{
	int aux;//<0이면 일반 텍스트, 0>=이면 교환 번호
	vector<bool> dbcs;
	string token;
};

#define MAXSTRLEN 150
typedef array<array<map<UINT,list<wordInfo>>,256>,MAXSTRLEN+1> _rankDic; //_rankDic을 스택에 놓고 쓰지 말것! 크기가 몹시 큼!
typedef list<_rankDic> _fullDic;
//우선순위(링크드리스트) -> 단어길이(배열) -> 첫번째 글자(배열) -> Hash값을 기반으로 하는 BinaryTree -> 원문:번역문 1대1 대응
//array에서 길이 0번은 aux 데이터로 할당되며,
//이때 보통 첫번째 글자로 쓰이는 256 범위에는 aux%256의 값이 들어가고 UINT로 aux/256이 들어감

struct DicStruct{
	std::wstring filename;

	_fullDic dic;
};