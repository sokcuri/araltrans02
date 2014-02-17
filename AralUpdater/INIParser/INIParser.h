// INIParser.h: interface for the CINIParser class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INIPARSER_H__65048B6F_809D_4837_BC56_B92D7437DA33__INCLUDED_)
#define AFX_INIPARSER_H__65048B6F_809D_4837_BC56_B92D7437DA33__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ININode.h"
#include <afxtempl.h>

class CINIParser  
{
private:
	CList<CININode*,CININode*> m_listININode;
	BOOL m_bIsUnicode;

public:
	CString ToString();
	CINIParser();
	virtual ~CINIParser();

	BOOL Load(CString path);
	BOOL Save(CString path);
	BOOL Parse(CString strINISource);
	
	CININode*	CreateNode(CString name);
	CININode*	FindNode(CString name);
	CININode*	GetNodeAt(int index);
	int			GetNodeCount();
	void		DeleteNode(CString name);
	void		Clear();

};

#endif // !defined(AFX_INIPARSER_H__65048B6F_809D_4837_BC56_B92D7437DA33__INCLUDED_)
