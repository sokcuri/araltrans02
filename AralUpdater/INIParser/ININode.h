// ININode.h: interface for the CININode class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ININODE_H__093D9552_9C59_42A0_9DC6_383684B611F5__INCLUDED_)
#define AFX_ININODE_H__093D9552_9C59_42A0_9DC6_383684B611F5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afx.h>

class CININode  
{
	friend class CINIParser;
private:
	CString				m_strName;
	CMapStringToString	m_mapAttr;
	POSITION			m_posCur;

	CININode();
	virtual ~CININode();

public:
	CString ToString();
	void	SetName(CString name);
	CString GetName();

	void	MoveFirstAttribute();
	BOOL	GetNextAttribute(CString& attr, CString& value);
	CString GetAttribute(CString attr);
	int		GetAttributeInt(CString attr);
	void	SetAttribute(CString attr, CString value);
	void	DeleteAttribute(CString attr);
	void	ClearAllAttribute();
};

#endif // !defined(AFX_ININODE_H__093D9552_9C59_42A0_9DC6_383684B611F5__INCLUDED_)
