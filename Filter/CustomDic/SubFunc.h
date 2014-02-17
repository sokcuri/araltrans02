#pragma once

UINT hashStringList(UINT* hash_dest,const char * cszSource);

wstring trim(const wstring strText);

string parseText(const string strText);

map<wstring,wstring> parseHeader(wstring strText,bool bIsFileName);

void AddDBCSInfo(vector<bool>& arDBCS,const string& src,const int cp);