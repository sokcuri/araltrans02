#ifndef __RLCMD_H__
#define __RLCMD_H__

#include "tstring.h"

class CRLCmd {
	protected:
		string m_strPrefix, m_strPostfix;
		int m_nSize;

		bool m_bIsOverwrite, m_bRemoveSpace, m_bIsBracket, m_bIsNoFreeze;

	public:
		CRLCmd() : m_nSize(0), m_bIsOverwrite(false), m_bRemoveSpace(false), m_bIsBracket(false), m_bIsNoFreeze(false) {}

		BOOL PreTranslate(LPCSTR cszInJapanese, LPSTR szOutJapanese, int nBufSize);
		BOOL PostTranslate(LPCSTR cszInKorean, LPSTR szOutKorean, int nBufSize);

		void SetOverwriteSetting(bool bIsOverwrite) { m_bIsOverwrite=bIsOverwrite; }
		bool GetOverwriteSetting() { return m_bIsOverwrite; }

		void SetRemoveSpaceSetting(bool bRemoveSpace) { m_bRemoveSpace=bRemoveSpace; }
		bool GetRemoveSpaceSetting() { return m_bRemoveSpace; }

		void SetBracketSetting(bool bIsBracket) { m_bIsBracket = bIsBracket; }
		bool GetBracketSetting() { return m_bIsBracket; }

		void SetNoFreezeSetting(bool bIsNoFreeze) { m_bIsNoFreeze = bIsNoFreeze; }
		bool GetNoFreezeSetting() { return m_bIsNoFreeze; }


	protected:
		void TrimCommand(const char *cszOriginal, string &strPrefix, string &strBody, string &strPostfix);
		
		void RestoreCommand(string &strPrefix, const char *cszTranslated, string &strPostfix, string &strRestored, int nMaxSize);
		
		void SqueezeAndReplaceSpecialString(string &strData);

};

extern CRLCmd g_cRLCmd;

#endif//__RLCMD_H__