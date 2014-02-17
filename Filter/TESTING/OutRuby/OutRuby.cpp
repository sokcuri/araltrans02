#include "StdAfx.h"
#include "OutRuby.h"

COutRuby::COutRuby(void)
{
}

COutRuby::~COutRuby(void)
{
}

void COutRuby::SetRubyString(LPCSTR szRuby)
{
	//형식이 약간 달라 수정해 줘야한다.
	strRuby=szRuby;
	wstring strRubyEnd;
	int Len=(int)strRuby.length();

	bool isHead=true;
	for(int i=0;i<Len;i++)
	{
		if(isHead)
		{
			strRubyEnd+=L"H";
			isHead=false;
		}
		strRubyEnd+=(wchar_t)strRuby[i];

		if(strRuby[i]=='!' || strRuby[i]=='*' || strRuby[i]=='^')
		{
			isHead=true;
		}
	}

	RubyString.InitFunc(0,true,strRubyEnd);
}

LPCSTR COutRuby::GetRubyString()
{
	return strRuby.c_str();
}

void COutRuby::PreOutRuby(LPSTR szDest, LPCSTR szSource)
{

	//FixLine에서 일부만 빼왔음

	string strSource=szSource;
	string strDest;

	string::size_type Len=strSource.length();
	string::size_type Finder=0;


	
	while(Finder<Len)
	{

		//일단 이부분이 함수인지 확인해보자
		int Len=RubyString.FindFunc(strSource,(int)Finder);

		if(Len==-1)
		{
			//그냥 일반 문자열이라면
			if(IsDBCSLeadByteEx(932,strSource[Finder])){
				//전각?
				strDest.push_back(strSource[Finder]);
				strDest.push_back(strSource[Finder+1]);
				Finder+=2;
			}
			else
			{
				//반각?
				strDest.push_back(strSource[Finder]);
				Finder++;
			}
		}
		else
		{
			//함수라면 =ㅅ=;
			FLFunctionString TempFunc=RubyString;
			TempFunc.InitText(strSource.substr(Finder,Len));
			strDest+=TempFunc.GetOnlyText();
			Finder+=Len;
		}
	}

	lstrcpyA(szDest,strDest.c_str());
}