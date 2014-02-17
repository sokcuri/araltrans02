#ifndef __CHECKER_H__
#define __CHECKER_H__

#include "tstring.h"
#include <vector>

using namespace std;

// Checker 인터페이스
class IChecker {
	public:

		virtual void SetBoundary(tstring strBoundary) = NULL;
		virtual int GetBoundary(tstring &strBoundary) = NULL;

		virtual bool IsInBoundary(WCHAR wch) = NULL;

};

// CChecker
class CChecker : public IChecker {
	protected:
		WCHAR m_wchLow, m_wchHigh;

	public:
		CChecker() : m_wchLow(0), m_wchHigh(0) {}

		virtual void SetBoundary(tstring strBoundary);
		virtual int GetBoundary(tstring &strBoundary);

		virtual bool IsInBoundary(WCHAR wch) { return ( (m_wchLow <= wch) && (wch <= m_wchHigh) ); }

	protected:
		int ttoh(tstring strHexa);
};

// CMultiChecker
class CMultiChecker : public IChecker {
	protected:
		typedef vector<IChecker *> CCheckerArray;

		CCheckerArray m_cCheckers;

	public:
		~CMultiChecker() { Cleanup(); }

		virtual void SetBoundary(tstring strBoundary);
		virtual int GetBoundary(tstring &strBoundary);
		
		virtual bool IsInBoundary(WCHAR wch);

	protected:
		void Cleanup();
};


#endif//__CHECKER_H__