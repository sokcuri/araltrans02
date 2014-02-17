#ifndef __DENYWORD_H__
#define __DENYWORD_H__

#include "tstring.h"
#include <map>

using namespace std;

typedef multimap<char, string> CCheckMap;

class CDenyWord {
	protected:
		CCheckMap m_cExactCheck, m_cDenyCheck, m_cAllowCheck;
		int m_nCut;

		int m_nPassKanji;
		bool m_bNotKanji;

	public:
		CDenyWord() : m_nCut(0),m_nPassKanji(0),m_bNotKanji(false) {}

		void Load();
		
		bool IsDenied(const char *pszJapanese);

		int GetCutSetting() { return m_nCut; }
		void SetCutSetting(int nCut) { m_nCut=nCut; }

		int GetPassKanjiSetting() { return m_nPassKanji; }
		void SetPassKanjiSetting(int nPassKanji)
		{
			if(nPassKanji<0)
			{
				m_nPassKanji=0;
			}
			else if(nPassKanji>100)
			{
				m_nPassKanji=100;
			}
			else
			{
				m_nPassKanji=nPassKanji;
			}
		}

		bool GetNotKanjiSetting() { return m_bNotKanji; }
		void SetNotKanjiSetting(bool bNotKanji) { m_bNotKanji=bNotKanji; }

		int GetDenyStringCount() { return m_cDenyCheck.size(); }
		int GetAllowStringCount() { return m_cAllowCheck.size(); }
		int GetExactStringCount() { return m_cExactCheck.size(); }
};

extern CDenyWord g_cDenyWord;
#endif