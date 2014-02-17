#pragma once

#define APPLOC_FUNC_CNT 9

class CAppLocMgr
{
private:
	CAppLocMgr(void);
	~CAppLocMgr(void);

	static char _AppLocFuncName[APPLOC_FUNC_CNT][32];

public:
	static BOOL IsLoaded();

	static BOOL SaveProcAddrToReg();

	static BOOL Enable();
	static BOOL Disable();


};
