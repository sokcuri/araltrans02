#pragma once
#include "FLFunctionString.h"
#include "tstring.h"
#include "util.h"

class COutRuby
{
public:
	COutRuby(void);
	~COutRuby(void);

private:
	FLFunctionString RubyString;

	string strRuby;

public:
	void PreOutRuby(LPSTR szDest,LPCSTR szSource);

	void SetRubyString(LPCSTR szRuby);
	LPCSTR GetRubyString();
};
