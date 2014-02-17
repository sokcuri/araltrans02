#pragma once
#include <vector>
#include <map>
#include "tstring.h"

using namespace std;

extern wchar_t ListFileName[4096];

struct ScriptInfo{
	wstring FileName;
	wstring Title;
	wstring Description;
};

class CCustomScript
{
public:
	CCustomScript(void);
	~CCustomScript(void);

public:
	void InitR(void);
	void InitW(void);

	int CheckJpnLine(LPCSTR Source);
	void WriteScript(LPCSTR JpnLine, LPCSTR KorLine);

	void GetKorLine(LPSTR Dest);

	void SetWritePassN(int _WritePassN){WritePassN=_WritePassN;}
	int GetWritePassN(){return WritePassN;}

	void FileClose(){if(WriteFile)fclose(WriteFile);}

private:
	//함수목록
	int AddScript(LPCWSTR Source);

	void AddLine(UINT HashedJpnLine, wstring KorLineW,int ScriptN,int PositionN);

	void EncodeLine(LPWSTR Dest,LPCWSTR Source);
	wstring DecodeLine(wstring Source);

	void Clear();

private:
	FILE * WriteFile;

	//변수목록
	struct LineDetail{
		string Line;
		int ScriptN;
		int PositionN;
	};

	int WritePassN;

	int ScriptN;

	LineDetail ThisLine;

	multimap <UINT,LineDetail> LineList;
};
