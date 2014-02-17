#pragma once

#include <vector>
#include <list>
#include "TransScriptParser.h"

using namespace std;

class COptionNode;
class CTransCommand;

//////////////////////////////////////////////////////////////////////////
class CHookPoint
{
private:
	static int	m_nKernelVer;
	HMODULE		m_hModule;
	UINT_PTR	m_pCodePoint, m_pCodeOffset;
	DWORD		m_pCodeSub[100];
	int			m_nCodeSubCount;
	CString		m_strModuleName;
	CTransScriptParser m_parser;
	vector<CTransCommand*> m_vectorTransCmd;

	CHookPoint();
	void ExecuteTransCmds(PREGISTER_ENTRY pRegisters, bool bPtrBack = false);
	BOOL EncodeMultiBytes(LPSTR szKorean);
	BOOL EnforceTwoBtye(LPSTR szText, LPWSTR wszText, BOOL b_nNull);
	BOOL EnforceOneBtye(LPSTR szText, LPWSTR wszText);
	BOOL ReduceSpaceA(LPSTR szText, int nSizeTo);
	BOOL EnforceAddNull(LPSTR szText, LPWSTR wszText, CTransCommand* pCmd);
	BOOL AnsiError(LPWSTR wszText);
	// BOOL DecodeJ2K(LPCSTR cszJpnCode, LPSTR szKorCode);
	BOOL EndCharAdd(CTransCommand* pCmd, LPVOID pOrigBasePtr, CTransScriptParser *pParser);
	BOOL TranslateScript(LPCWSTR cwszSrc, LPWSTR wszTar);
	BOOL TranslateUnicodeText(LPCWSTR cwszSrc, LPWSTR wszTar);
	int  GetTotalLines(LPCWSTR cwszText);
	int  GetNextLine(LPWSTR& wszBuf, CString& strLine);
	BOOL KiriName(LPWSTR wszTransTextBuf, LPWSTR wszOrigTextBuf);
	static void PointCallback(LPVOID pHookedPoint, PREGISTER_ENTRY pRegisters);

public:
	static bool IsWinVistaAndAbove();
	static CHookPoint* CreateInstance(CString strAddr);
	~CHookPoint();

	UINT_PTR		GetHookAddress();
	CString			GetHookAddressString();
	
	CTransCommand*	AddTransCmd(COptionNode *pNode);
	CTransCommand*	FindTransCmd(CString strArgScript);

	int				GetTransCmdCount();	
	CTransCommand*	GetTransCmd(int nIdx);
	void			DeleteTransCmd(int nIdx);
	void			DeleteAllTransCmd();
};
