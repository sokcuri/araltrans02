#include "StdAfx.h"
#include "FileOptionParser.h"

#include "Util.h"

CFileOptionParser::CFileOptionParser()
{
}

CFileOptionParser::CFileOptionParser(tstring strFileName, bool AutoRead)
{
	if(strFileName.size()>0)
		SetOptionFile(strFileName);

	if(strFileName.size()>0 && AutoRead)
		OpenOptionFile();
		
}

CFileOptionParser::CFileOptionParser(tstring strFileName)
{
	if(strFileName.size()>0)
		SetOptionFile(strFileName);	
}

CFileOptionParser::~CFileOptionParser(void)
{
}

// 파일명 지정
// 상대경로도 가능
void CFileOptionParser::SetOptionFile(tstring strFileName)
{
	//절대경로 여부 확인
	//드라이브 문자의 : 를 기준으로
	if(strFileName.find(_T(':'))!=strFileName.npos)
	{
		//절대경로라면
		m_strFileName=strFileName;
	}
	else
	{
		//상대경로라면
		//게임 폴더 기준
		m_strFileName = GetGameDirectory();
		m_strFileName += _T('\\');
		m_strFileName += strFileName;
	}

	MyCreateDirectory(m_strFileName.substr(0,m_strFileName.rfind(_T('\\'))).c_str());
}

//파일 열기
void CFileOptionParser::OpenOptionFile()
{
	m_Options.clear();

	FILE * pFile;	//파일 포인터

#ifdef _UNICODE
	_tfopen_s(&pFile,m_strFileName.c_str(),_T("rt,ccs=UTF-16LE"));	//UTF-16LE
#else
	_tfopen_s(&pFile,m_strFileName.c_str(),_T("rt"));			//MBCS
#endif

	if(pFile)
	{
		TCHAR szTempLine[1024]=_T("\0");	//읽은 줄을 임시 보관

		while(_fgetts(szTempLine,1024,pFile) && szTempLine[0]!=0x00)
		{
			size_t nLen = _tcslen(szTempLine);

			// 뒷부분 정리
			if(szTempLine[nLen-1]==_T('\n'))
				szTempLine[--nLen]=_T('\0');

			tstring strTempLine=szTempLine;
			tstring strOptionName;			//옵션명
			tstring strOptionData;			//옵션값

			int nDivider=strTempLine.find(_T('='));

			if(nDivider>0)
			{
				strOptionName = RemoveSpace(strTempLine.substr(0,nDivider));
				strOptionData = RemoveSpace(strTempLine.substr(nDivider+1,nLen-nDivider-1));

				m_Options[strOptionName] = strOptionData;
			}

		} // while End

		fclose(pFile);

	} // pFile End
}

void CFileOptionParser::SaveOptionFile()
{
	FILE * pFile;

#ifdef _UNICODE
	_tfopen_s(&pFile,m_strFileName.c_str(),_T("wt,ccs=UTF-16LE"));	//UTF-16LE
#else
	_tfopen_s(&pFile,m_strFileName.c_str(),_T("wt"));			//MBCS
#endif

	if(pFile)
	{
		map <tstring,tstring>::iterator iter=m_Options.begin();

		while(iter!=m_Options.end())
		{
			_ftprintf(pFile,_T("%s=%s\n"),iter->first.c_str(),iter->second.c_str());
			iter++;
		}

		fclose(pFile);
	}

}


void CFileOptionParser::Clear()
{
	m_Options.clear();
}

tstring CFileOptionParser::GetOptionString(tstring strOptionName, vector<int>& subArgs){
	tstring strTemp=strOptionName;

	TCHAR szTemp[10]=_T("\0");
	for(int i=0;i<subArgs.size();i++){
		_itot_s(subArgs[i],szTemp,10,10);
		strTemp.push_back(_T('_'));
		strTemp+=szTemp;
	}

	if(m_Options.find(strTemp)!=m_Options.end())
		return m_Options[strTemp];
	else
		return tstring(_T(""));
}

int CFileOptionParser::GetOptionInt(tstring strOptionName, vector<int>& subArgs){
	return _ttoi(GetOptionString(strOptionName,subArgs).c_str());
}

bool CFileOptionParser::GetOptionBool(tstring strOptionName, int nTokCount, ...){
	vector<int> tmpArr;
	va_list ap;
	va_start(ap,nTokCount);
	while(--nTokCount>=0){
		tmpArr.push_back(va_arg(ap,int));
	}
	va_end(ap);

	return GetOptionInt(strOptionName,tmpArr)!=0?true:false;
}

int CFileOptionParser::GetOptionInt(tstring strOptionName, int nTokCount, ...){
	vector<int> tmpArr;
	va_list ap;
	va_start(ap,nTokCount);
	while(--nTokCount>=0){
		tmpArr.push_back(va_arg(ap,int));
	}
	va_end(ap);

	return GetOptionInt(strOptionName,tmpArr);
}

tstring CFileOptionParser::GetOptionString(tstring strOptionName, int nTokCount, ...){
	vector<int> tmpArr;
	va_list ap;
	va_start(ap,nTokCount);
	while(--nTokCount>=0){
		tmpArr.push_back(va_arg(ap,int));
	}
	va_end(ap);

	return GetOptionString(strOptionName,tmpArr);
}

void CFileOptionParser::SetOptionString(tstring strOptionData, tstring strOptionName, vector<int>& subArgs){
	tstring strTemp=strOptionName;

	TCHAR szTemp[20]=_T("\0");
	for(int i=0;i<subArgs.size();i++){
		_itot_s(subArgs[i],szTemp,20,10);
		strTemp.push_back(_T('_'));
		strTemp+=szTemp;
	}

	m_Options[strTemp]=strOptionData;
}

void CFileOptionParser::SetOptionInt(int nOptionData, tstring strOptionName, vector<int>& subArgs){
	TCHAR szTemp[20]=_T("\0");
	_itot_s(nOptionData,szTemp,20,10);

	tstring strTemp=szTemp;

	SetOptionString(strTemp,strOptionName,subArgs);
}

void CFileOptionParser::SetOptionString(tstring strOptionData, tstring strOptionName, int nTokCount, ...){
	vector<int> tmpArr;
	va_list ap;
	va_start(ap,nTokCount);
	while(--nTokCount>=0){
		tmpArr.push_back(va_arg(ap,int));
	}
	va_end(ap);

	SetOptionString(strOptionData,strOptionName,tmpArr);
}

void CFileOptionParser::SetOptionInt(int nOptionData, tstring strOptionName, int nTokCount, ...){
	vector<int> tmpArr;
	va_list ap;
	va_start(ap,nTokCount);
	while(--nTokCount>=0){
		tmpArr.push_back(va_arg(ap,int));
	}
	va_end(ap);

	SetOptionInt(nOptionData,strOptionName,tmpArr);
}

tstring CFileOptionParser::RemoveSpace(tstring strData)
{
	int nHead;
	int nTail;

	nHead = strData.find_first_not_of(_T(' '));
	nTail = strData.find_last_not_of(_T(' '));

	return strData.substr(nHead,nTail-nHead+1);
}

bool CFileOptionParser::IsInData(tstring strOptionName, vector<int>& subArgs){
	tstring strTemp=strOptionName;

	TCHAR szTemp[20]=_T("\0");
	for(int i=0;i<subArgs.size();i++){
		_itot_s(subArgs[i],szTemp,20,10);
		strTemp.push_back(_T('_'));
		strTemp+=szTemp;
	}

	return (m_Options.find(strTemp)!=m_Options.end());
}

bool CFileOptionParser::removeData(tstring strOptionName, vector<int>& subArgs){
	tstring strTemp=strOptionName;

	TCHAR szTemp[20]=_T("\0");
	for(int i=0;i<subArgs.size();i++){
		_itot_s(subArgs[i],szTemp,20,10);
		strTemp.push_back(_T('_'));
		strTemp+=szTemp;
	}

	map <tstring,tstring>::iterator iter=m_Options.find(strTemp);
	if(iter==m_Options.end())return false;

	m_Options.erase(iter);
	return true;
}

bool CFileOptionParser::IsInData(tstring strOptionName, int nTokCount, ...){
	vector<int> tmpArr;
	va_list ap;
	va_start(ap,nTokCount);
	while(--nTokCount>=0){
		tmpArr.push_back(va_arg(ap,int));
	}
	va_end(ap);

	return IsInData(strOptionName,tmpArr);
}

bool CFileOptionParser::removeData(tstring strOptionName, int nTokCount, ...){
	vector<int> tmpArr;
	va_list ap;
	va_start(ap,nTokCount);
	while(--nTokCount>=0){
		tmpArr.push_back(va_arg(ap,int));
	}
	va_end(ap);

	return removeData(strOptionName,tmpArr);
}