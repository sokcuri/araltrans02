
/* 
 * 파일 옵션 파서 - Hide_D君
 *
 * 옵션을 파일로 저장하는 경우 사용
 *
 * 옵션은
 *
 * 옵션명:옵션값
 *
 * 이 기본이며, 서브값으로
 *
 * 옵션명_n:옵션값
 * 
 * 옵션명_n_n:옵션값
 * 
 * 구조로 사용할 수 있음
 */

#pragma once

#include <map>
#include <vector>
#include "tstring.h"

using namespace std;

class CFileOptionParser
{
public:
	CFileOptionParser(tstring strFileName, bool AutoRead);
	CFileOptionParser(tstring strFileName);
	CFileOptionParser();
public:
	~CFileOptionParser(void);

public:
	void SetOptionFile(tstring strFileName);

	void OpenOptionFile();
	void SaveOptionFile();

	void Clear();

	int GetOptionInt(tstring strOptionName, int nTokCount=0, ...);
	int GetOptionInt(tstring strOptionName, vector<int>& subArgs);

	bool GetOptionBool(tstring strOptionName, int nTokCount=0, ...);

	tstring GetOptionString(tstring strOptionName, int nTokCount=0, ...);
	tstring GetOptionString(tstring strOptionName, vector<int>& subArgs);


	void SetOptionInt(int nOptionData, tstring strOptionName, int nTokCount=0, ...);
	void SetOptionInt(int nOptionData, tstring strOptionName, vector<int>& subArgs);

	void SetOptionString(tstring strOptionData, tstring strOptionName, int nTokCount=0, ...);
	void SetOptionString(tstring strOptionData, tstring strOptionName, vector<int>& subArgs);	

	bool IsInData(tstring strOptionName, int nTokCount=0, ...);
	bool IsInData(tstring strOptionName, vector<int>& subArgs);

	bool removeData(tstring strOptionName, int nTokCount=0, ...);
	bool removeData(tstring strOptionName, vector<int>& subArgs);

private:
	tstring RemoveSpace(tstring strData);

	map <tstring,tstring> m_Options;	//옵션 저장용 맵
	tstring m_strFileName;				//옵션 파일명

};
