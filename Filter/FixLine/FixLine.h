#pragma once
#include <set>
#include <queue>
#include "FLString.h"
#include "FileOptionParser.h"

using namespace std;

class CFixLine
{
public:
	CFixLine(void);
	~CFixLine(void);

public:
	void PreFixLine(LPSTR szDest,LPCSTR szSource);
	void PostFixLine(LPSTR szDest,LPCSTR szSource);
	void Clear();
	void Initialize();

private:
	CFileOptionParser m_cOption;

private:
	//可记 贸府侩
	vector <FLString> m_FuncPool;
	set	<string>	m_NamePool;
	
	string m_strRetChar;

	int m_nMaxChar;
	int m_nMaxLine;
	bool m_bUseLimitLine;
	bool m_bUseRetChar;
	bool m_bUseRet;

	int m_nMaxLen;
	bool m_bForceLen;

private:
	//单捞磐 贸府侩
	vector <FLString> m_ElementPool;

	queue <int> m_FuncQueue;

	int m_nLineN;
	bool m_bSetLastRet;

	int m_nSetMaxLen;
};

