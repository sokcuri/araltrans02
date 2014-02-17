#pragma once

#include <vector>
#include <string>

using namespace std;

class CDivideLine
{
public:
	CDivideLine(void);
	~CDivideLine(void);

public:
	void PreDivideLine(LPSTR szDest,LPCSTR szSource);
	void PostDivideLine(LPSTR szDest, LPCSTR szSource);

	// Base(IgnoreLine,MaxChar,MaxLine,ReturnLen,ReturnChar)
	void SetBase(int nIgnoreLine,int nMaxChar,int nMaxLine, int nReturnLen,string strReturnChar);
	void SetRemoveSpace(BOOL bRemoveSpace){m_bRemoveSpace=bRemoveSpace;}
	void SetTwoByte(BOOL bTwoByte){m_bTwoByte=bTwoByte;}

	int GetReturnLen(){return m_nReturnLen;}
	int GetMaxChar(){return m_nMaxChar;}
	int GetMaxLine(){return m_nMaxLine;}
	int GetIgnoreLine(){return m_nIgnoreLine;}
	string GetReturnChar(){return m_strReturnChar;}
	int GetReturnCharLen(){return m_nReturnCharLen;}
	BOOL GetRemoveSpace(){return m_bRemoveSpace;}
	BOOL GetTwoByte(){return m_bTwoByte;}

private:

	int SplitLines(vector <string>* NewLines,vector <string> Lines);

private:

	int m_nReturnLen;
	int m_nMaxChar;
	int m_nMaxLine;
	int m_nIgnoreLine;

	string m_strReturnChar;
	int m_nReturnCharLen;

	BOOL m_bRemoveSpace;
	BOOL m_bTwoByte;

};
extern CDivideLine g_cDivideLine;