#include "StdAfx.h"

#include "Checker.h"

#include "ctype.h"

void CChecker::SetBoundary(tstring strBoundary)
{
	int idx;

	idx=strBoundary.find_first_of(_T('-'));

	if (idx != strBoundary.npos)
	{
		m_wchLow=(WCHAR)ttoh(strBoundary.substr(0,idx));
		m_wchHigh=(WCHAR)ttoh(strBoundary.substr(idx+1));
	}
	else
		m_wchHigh=m_wchLow=(WCHAR)ttoh(strBoundary);
}

int CChecker::GetBoundary(tstring &strBoundary)
{
	TCHAR szBoundary[20];
	if (m_wchLow == m_wchHigh)
	{
		if (m_wchLow < 0x100)
			wsprintf(szBoundary, _T("%02X"), m_wchLow);
		else
			wsprintf(szBoundary, _T("%04X"), m_wchLow);
	}
	else
	{
		if (m_wchHigh < 0x100)
			wsprintf(szBoundary, _T("%02X-%02X"), m_wchLow, m_wchHigh);
		else
			wsprintf(szBoundary, _T("%04X-%04X"), m_wchLow, m_wchHigh);
	}

	strBoundary = szBoundary;
	return (m_wchHigh - m_wchLow +1);
}

int CChecker::ttoh(tstring strHexa)
{
	int nRet=0;
	int nSize = strHexa.size();
	int i;

	TCHAR ch;

	for (i=0; i<nSize; i++)
	{
		ch=_totupper(strHexa[i]);
		if ( (_T('0') <= ch) && (ch <= _T('9')) )	// 0 - 9
		{
			nRet *= 16;
			nRet += strHexa[i] - _T('0');
		}
		else if ( (_T('A') <= ch) && (ch <= _T('F')) ) // A - F
		{
			nRet *= 16;
			nRet += strHexa[i] - _T('A') + 10;
		}
		else if ( ch == _T('X') )	// X (0x) : ¹«½Ã
			continue;
		else
			break;
	}
	return nRet;
}

void CMultiChecker::Cleanup()
{
	if (!m_cCheckers.empty())
	{
		CCheckerArray::iterator it;

		for (it = m_cCheckers.begin(); it != m_cCheckers.end(); it++)
			delete *it;

		m_cCheckers.clear();
	}
}

void CMultiChecker::SetBoundary(tstring strBoundary)
{
	int idxFirst=0, idxDelim;
	IChecker *pChecker;

	Cleanup();

	if (strBoundary.empty()) return;

	idxDelim = strBoundary.find(_T(','), idxFirst);

	while (idxDelim != strBoundary.npos)
	{
		pChecker = new CChecker;

		pChecker->SetBoundary(strBoundary.substr(idxFirst, idxDelim-idxFirst));

		m_cCheckers.push_back(pChecker);

		idxFirst = idxDelim+1;
		idxDelim = strBoundary.find(_T(','), idxFirst);
	}

	pChecker = new CChecker;

	pChecker->SetBoundary(strBoundary.substr(idxFirst));
	
	m_cCheckers.push_back(pChecker);
	
}

int CMultiChecker::GetBoundary(tstring &strBoundary)
{
	tstring strLocalBoundary;
	int i, nSize = m_cCheckers.size();
	int nRet=0;

	strBoundary.erase();

	if (nSize == 0)
		return 0;

	for (i=0; i<nSize-1; i++)
	{
		nRet+=m_cCheckers[i]->GetBoundary(strLocalBoundary);
		strBoundary+=strLocalBoundary;
		strBoundary+=_T(',');
	}
	nRet+=m_cCheckers[i]->GetBoundary(strLocalBoundary);
	strBoundary+=strLocalBoundary;

	return nRet;
}

bool CMultiChecker::IsInBoundary(WCHAR wch)
{
	CCheckerArray::iterator it;

	for(it = m_cCheckers.begin(); it != m_cCheckers.end(); it++)
	{
		if ( (*it)->IsInBoundary(wch) )
			return true;
	}
	return false;
}