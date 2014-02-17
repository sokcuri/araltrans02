#pragma once
#include "FileOptionParser.h"

class CMigration
{
public:
	CMigration(void);
	~CMigration(void);
	static void Migration1to2(wstring strFileName);
	static void Migration2to2_1(wstring strFileName);
};