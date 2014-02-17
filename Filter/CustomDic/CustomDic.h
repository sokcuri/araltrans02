#pragma once

#include "CDic_Proxy.h"


struct DBInfo{
	bool UsePre;
	bool UsePost;
	list<_fullDic>::iterator preIter;
	list<_fullDic>::iterator postIter;
};

class CustomDic : public CWinApp
{
public:
	CustomDic(void);
	~CustomDic(void);

	//Plugin
	void Migration(string strOption);
	bool Initialize();
	bool Option();
	bool Close();

	bool PreTranslate(const string& preStr,string& postStr);
	bool PostTranslate(const string& preStr, string& postStr);

	//AutoLoad
	void ChangeDic(wstring strFileName,_fullDic& dicList);
	void ChangeDic(wstring strFileName,_fullDic& dicPre,_fullDic& dicPost);

protected:
	virtual BOOL InitInstance();

	void PatternProcessing(list<TokenStruct>& arPartOut,list<TokenStruct>& arPartIn,const list<_fullDic>& baseDicDB,const int cnBaseCP);

	DECLARE_MESSAGE_MAP()

	const string m_strFullToken;

	list<int> m_TokenList;

	COptionDlg* m_pOption;
	AutoLoadDic* m_pAutoLoad;

	CCriticalSection m_csLock;

	list<_fullDic> m_PreDicDB;
	list<_fullDic> m_PostDicDB;
	map<wstring,DBInfo> m_infoDic;

	bool m_bTrimWord;

	//버퍼용도
	vector<UINT> m_Hashed;

};

