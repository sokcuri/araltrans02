#pragma once

#include "OptionPage1.h"
#include "OptionPage2.h"
#include "FileOptionParser.h"


// COptionSheet

class COptionSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(COptionSheet)

public:
	COptionSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	COptionSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	virtual ~COptionSheet();

	COptionPage1 m_cPage1;
	COptionPage2 m_cPage2;

	CFileOptionParser m_cFileOption;
protected:
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
};
