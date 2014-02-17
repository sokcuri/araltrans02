#ifndef __CMDFILTER_H__
#define __CMDFILTER_H__

#include "tstring.h"
#include "Checker.h"


class CCmdFilter {
protected:
	string m_strPrefix, m_strPostfix;
	int m_nSize;

	CMultiChecker m_cDenyChecker;

	CMultiChecker m_cPrefixIncludeChecker;
	CMultiChecker m_cPrefixExcludeChecker;

	CMultiChecker m_cBodyIncludeChecker;
	CMultiChecker m_cBodyExcludeChecker;
	
	int m_nCutSize;
	bool m_bIsOverwrite, m_bRemoveSpace, m_bIsTwoByte;
	
public:

	CCmdFilter();
	
	BOOL PreTranslate(LPCSTR cszInJapanese, LPSTR szOutJapanese, int nBufSize);
	BOOL PostTranslate(LPCSTR cszInKorean, LPSTR szOutKorean, int nBufSize);
	
	void SetOverwriteSetting(bool bIsOverwrite) { m_bIsOverwrite=bIsOverwrite; }
	bool GetOverwriteSetting() { return m_bIsOverwrite; }
	
	void SetRemoveSpaceSetting(bool bRemoveSpace) { m_bRemoveSpace=bRemoveSpace; }
	bool GetRemoveSpaceSetting() { return m_bRemoveSpace; }

	void SetTwoByteSetting(bool bIsTwoByte) { m_bIsTwoByte=bIsTwoByte; }
	bool GetTwoByteSetting() { return m_bIsTwoByte; }
	
	void SetCutSizeSetting(int nCutSize) { m_nCutSize=nCutSize; }
	int GetCutSizeSetting() { return m_nCutSize; }

	void SetDenySetting(tstring strBoundary) { m_cDenyChecker.SetBoundary(strBoundary); }
	int GetDenySetting(tstring &strBoundary) { return m_cDenyChecker.GetBoundary(strBoundary); }

	void SetPrefixIncludeSetting(tstring strBoundary) { m_cPrefixIncludeChecker.SetBoundary(strBoundary); }
	int GetPrefixIncludeSetting(tstring &strBoundary) { return m_cPrefixIncludeChecker.GetBoundary(strBoundary); }

	void SetPrefixExcludeSetting(tstring strBoundary) { m_cPrefixExcludeChecker.SetBoundary(strBoundary); }
	int GetPrefixExcludeSetting(tstring &strBoundary) { return m_cPrefixExcludeChecker.GetBoundary(strBoundary); }

	void SetBodyIncludeSetting(tstring strBoundary) { m_cBodyIncludeChecker.SetBoundary(strBoundary); }
	int GetBodyIncludeSetting(tstring &strBoundary) { return m_cBodyIncludeChecker.GetBoundary(strBoundary); }

	void SetBodyExcludeSetting(tstring strBoundary) { m_cBodyExcludeChecker.SetBoundary(strBoundary); }
	int GetBodyExcludeSetting(tstring &strBoundary) { return m_cBodyExcludeChecker.GetBoundary(strBoundary); }

protected:
	BOOL TrimCommand(const char *cszOriginal, string &strPrefix, string &strBody, string &strPostfix);
	
	void RestoreCommand(string &strPrefix, const char *cszTranslated, string &strPostfix, string &strRestored, int nMaxSize);
	
	void SqueezeAndReplaceSpecialString(string &strData);
};

extern CCmdFilter g_cCmdFilter;

#endif//__CMDFILTER_H__