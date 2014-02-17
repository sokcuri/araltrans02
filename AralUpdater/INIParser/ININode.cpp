// ININode.cpp: implementation of the CININode class.
//
//////////////////////////////////////////////////////////////////////

#include "ININode.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CININode::CININode()
	: m_posCur(NULL)
{

}

CININode::~CININode()
{

}

void CININode::SetName(CString name)
{
	m_strName = name;
}

CString CININode::GetName()
{
	return m_strName;
}

void CININode::MoveFirstAttribute()
{
	m_posCur = m_mapAttr.GetStartPosition();
}

BOOL CININode::GetNextAttribute(CString& attr, CString& value)
{
	if( NULL==m_posCur ) return FALSE;
	m_mapAttr.GetNextAssoc(m_posCur, attr, value);
	
	// 따옴표 없애기
	int nLen = value.GetLength();
	
	if( nLen>=2 )
	{
		if( value[nLen-1] == _T('\"') ) value = value.Left( nLen-1 );
		if( value[0] == _T('\"') ) value = value.Mid(1);
	}
	
	return TRUE;
}


CString CININode::GetAttribute(CString attr)
{
	CString strRetVal = _T("");
	if( m_mapAttr.Lookup(attr, strRetVal)==FALSE ) strRetVal = _T("");

	// 따옴표 없애기
	int nLen = strRetVal.GetLength();
	
	if( nLen>=2 )
	{
		if( strRetVal[nLen-1] == _T('\"') ) strRetVal = strRetVal.Left( nLen-1 );
		if( strRetVal[0] == _T('\"') ) strRetVal = strRetVal.Mid(1);
	}

	return strRetVal;
}

int CININode::GetAttributeInt(CString attr)
{
	int nRetVal = 0;

	CString strVal = GetAttribute(attr);

	if( !strVal.IsEmpty() )
	{
		nRetVal = _ttoi( strVal );
	}

	return nRetVal;
}


void CININode::SetAttribute(CString attr, CString value)
{
	m_mapAttr[attr] = value;
}

void CININode::DeleteAttribute(CString attr)
{
	m_mapAttr.RemoveKey(attr);
}

void CININode::ClearAllAttribute()
{
	m_mapAttr.RemoveAll();
}

CString CININode::ToString()
{
	CString strRetVal = _T("");
	CString strLine = _T("");
	
	// 노드이름
	strLine = _T("[") + m_strName + _T("]\r\n");
	strRetVal += strLine;

	// 속성
	CString strAttr, strValue;
	POSITION pos = m_mapAttr.GetStartPosition();
	while(pos!=NULL)
	{
		m_mapAttr.GetNextAssoc(pos, strAttr, strValue);
		strLine = strAttr + _T("=") + strValue + _T("\r\n");
		strRetVal += strLine;
	}

	return strRetVal;
}
