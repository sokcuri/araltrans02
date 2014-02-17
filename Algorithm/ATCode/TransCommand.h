#pragma once

#include <list>

using namespace std;

#define MAX_TEXT_LENGTH 1024

enum {
	TRANSCOMMAND_NOP = 0,
	TRANSCOMMAND_PTRCHEAT = 1,
	TRANSCOMMAND_OVERWRITE = 2,
	TRANSCOMMAND_SOW = 3,
	TRANSCOMMAND_SMSTR = 4
};

class COptionNode;
class CTransScriptParser;

//////////////////////////////////////////////////////////////////////////
// CTransCommand : CTransCommand 기본 클래스
class CTransCommand
{
public:
	friend class CHookPoint;

protected:

	int		m_nTransMethod;
	BOOL	m_bTrans;
	BOOL	m_bUnicode;
	BOOL	m_bUnicode8;
	BOOL	m_bClipKor;
	BOOL	m_bClipJpn;
	BOOL	m_bRemoveSpace;
	BOOL	m_bTwoByte;
	BOOL	m_bOneByte;
	BOOL	m_bSafe;
	BOOL	m_bAddNull;
	BOOL	m_bAdjustLen;
	BOOL	m_bEndPos;
	BOOL	m_bLenEnd;
	BOOL	m_bTjsStr;
	BOOL	m_bKiriName;
	BOOL	m_bPtrBack;
	BOOL	m_bDuplicated;
	CString m_strLenPos;
	int		m_nLenPos;
	int		m_nBackUpSize;

	CString m_strArgScript;

public:
	void	*m_pArgText;
	void	*m_pPrevArgText;
	BYTE	m_pOrigTextBuf[MAX_TEXT_LENGTH*2];
	BYTE	m_byReserved[0x100];					// LEN()을 위한 완충지대
	BYTE	m_pTransTextBuf[MAX_TEXT_LENGTH*2];
	BYTE	m_pBackUpBuf[MAX_TEXT_LENGTH*2 + 0x10];					// 포인터 복원을 위한 버퍼지역
	BYTE	m_pBackUpLength[0x10];					// LEN 복원을 위한 버퍼지역

public:
	static CTransCommand *CreateTransCommand(CString &strArgScript, COptionNode *pNode);	// CTransCommand 생성용 static 함수

protected:
	CTransCommand() : 
		m_nTransMethod(TRANSCOMMAND_NOP), m_bTrans(FALSE), m_bUnicode(FALSE), m_bUnicode8(FALSE), m_bClipKor(FALSE), 
		m_bClipJpn(FALSE), m_bRemoveSpace(FALSE), m_bTwoByte(FALSE), m_bOneByte(FALSE), m_bSafe(FALSE), m_bAddNull(FALSE), m_bEndPos(FALSE),
		m_bAdjustLen(FALSE), m_bLenEnd(FALSE), m_bTjsStr(FALSE), m_nLenPos(0), m_bKiriName(FALSE), m_bPtrBack(FALSE), m_bDuplicated(FALSE), m_nBackUpSize(0),
		m_pArgText(NULL), m_pPrevArgText(NULL)
	{
		m_strArgScript.Empty();
		ZeroMemory(m_pOrigTextBuf, MAX_TEXT_LENGTH*2);
		ZeroMemory(m_byReserved, 0x100);
		//ZeroMemory(m_pTransTextBuf, MAX_TEXT_LENGTH*2);
		ZeroMemory(m_pBackUpBuf, MAX_TEXT_LENGTH*2 + 0x10);
		ZeroMemory(m_pBackUpLength, 0x10);
	}
	~CTransCommand() { RestoreBackup(); }

protected:
	// Set functions //
	void SetArgScript(LPCTSTR strScript){ m_strArgScript = strScript; };

public:
	//void SetDistFromEBP(int nDistFromESP){ m_nDistFromESP = nDistFromESP; };
	//void SetTransMethod(int nMethod){ m_nTransMethod = nMethod; };
/*	void SetUnicode(BOOL bValue){ m_bUnicode = bValue; };
	void SetClipKor(BOOL bClipKor){ m_bClipKor = bClipKor; };
	void SetClipJpn(BOOL bClipJpn){ m_bClipJpn = bClipJpn; };
	void SetRemoveSpace(BOOL bRemove){ m_bRemoveSpace = bRemove; };
	void SetTwoByte(BOOL bEnable){ m_bTwoByte = bEnable; };
	void SetSafe(BOOL bEnable){ m_bSafe = bEnable; };
*/
	// 번역 옵션을 저장합니다.
	virtual void SetTransOption(CString strTransOption, COptionNode *pNode);
	// 특별 옵션 (OVERWRITE(IGNORE) 등) 을 저장합니다.
	virtual void SetSpecialOption(CString strSpecialOption, COptionNode *pNode) {}

	// Get functions //

	//int	 GetDistFromEBP(){ return m_nDistFromESP; };
	CString	GetArgScript(){ return m_strArgScript; };
	int  GetTransMethod(){ return m_nTransMethod; };
	BOOL GetTrans(){ return m_bTrans; };
	BOOL GetUnicode(){ return m_bUnicode; };
	BOOL GetUnicode8(){ return m_bUnicode8; };
	BOOL GetClipKor(){ return m_bClipKor; };
	BOOL GetClipJpn(){ return m_bClipJpn; };
	BOOL GetRemoveSpace(){ return m_bRemoveSpace; };
	BOOL GetTwoByte(){ return m_bTwoByte; };
	BOOL GetOneByte(){ return m_bOneByte; };
	BOOL GetSafe(){ return m_bSafe; };
	BOOL GetAddNull(){ return m_bAddNull; };
	BOOL GetEndPos(){ return m_bEndPos; };
	BOOL GetLenEnd(){ return m_bLenEnd; };
	BOOL GetKiriName(){ return m_bKiriName; };
	BOOL GetPtrBack(){ return m_bPtrBack; };
	BOOL GetDuplicated(){ return m_bDuplicated; };

	// Execute functions //

	// (백업 복구가 필요한 경우) 백업에서 원문을 복구합니다.
	virtual void RestoreBackup() {}

	// 실제 문자열의 위치를 가져옵니다. 잘못된 문자열인 경우 NULL.
	virtual void *GetTextPoint(CTransScriptParser *pParser, void *pBackUp, BOOL bPtrBack, void *pBackUpLength);

	// 받아온 문자열이 읽을 수 있는 문자열인지 확인합니다.
	virtual BOOL IsValidTextPoint(void *pArgText, long nSize = 1024*1024*1024);

	// 혹시 같은 문자열을 다시 번역하려는지 확인합니다.
	virtual BOOL IsDuplicateText(void *pArgText);

	// 문자열 길이를 번역된 문자열의 길이로 변경합니다.
	virtual BOOL ChangeLength(void *pOrigBasePtr, CTransScriptParser *pParser, void *pNewBasePtr=NULL, void *pBackUp=NULL);

	// 문자열 길이 백업과 복원을 합니다.
	virtual void BackUpLength(void *pBackUp);
	virtual void SetUpLength(void *pBackUp, int *pnLen, int nBytes, int nTempLen);

	// 포인터 백업과 복원을 합니다.
	virtual bool BackUpTextPoint(void *pBackUp, int pMovSize = 1);
	virtual void SetUpTextPoint(void *pBackUp, void *pArgText, void *ppArgText, int pMovSize, int nType = 1);

	// 번역된 문자열을 원 프로그램에 적용합니다.
	virtual BOOL ApplyTranslatedText(void *pArgText, PREGISTER_ENTRY pRegisters, CTransScriptParser *pParser, BOOL bIsDuplicated, void *pBackUp) { return TRUE; }

};

//////////////////////////////////////////////////////////////////////////
// CTransCommandNOP : NOP용 CTransCommand 확장 클래스
class CTransCommandNOP : public CTransCommand {
public:
	friend class CTransCommand;

protected:
	CTransCommandNOP() : CTransCommand()
		{ m_nTransMethod = TRANSCOMMAND_NOP; }
	~CTransCommandNOP() {}

public:

};

//////////////////////////////////////////////////////////////////////////
// CTransCommandPTRCHEAT : PTRCHEAT 방식을 위한 CTransCommand 확장 클래스
class CTransCommandPTRCHEAT : public CTransCommand {
public:
	friend class CTransCommand;

protected:
	CTransCommandPTRCHEAT() : CTransCommand()
		{ m_nTransMethod = TRANSCOMMAND_PTRCHEAT; }
	~CTransCommandPTRCHEAT() {}
public:
	virtual BOOL ChangeLength(void *pOrigBasePtr, CTransScriptParser *pParser, void *pNewBasePtr, void *pBackUp);

	virtual BOOL ApplyTranslatedText(void *pArgText, PREGISTER_ENTRY pRegisters, CTransScriptParser *pParser, BOOL bIsDuplicated, void *pBackUp);

};

//////////////////////////////////////////////////////////////////////////
// CTransCommandOVERWRITE : OVERWRITE 방식을 위한 CTransCommand 확장 클래스
class CTransCommandOVERWRITE : public CTransCommand {
public:
	friend class CTransCommand;

protected:
	BOOL	m_bAllSameText;
	BOOL	m_bIgnoreBufLen;

protected:
	CTransCommandOVERWRITE() : CTransCommand(), m_bAllSameText(FALSE), m_bIgnoreBufLen(FALSE)
		{ m_nTransMethod = TRANSCOMMAND_OVERWRITE; }
	~CTransCommandOVERWRITE() {}

	// 번역문을 원 프로그램에 덮어씌웁니다.
	BOOL OverwriteTextBytes( LPVOID pDest, LPVOID pSrc, void *pBackUp );

	// 스택 내의 모든 동일한 원문을 찾아 번역문으로 덮어씌웁니다.
	BOOL SearchTextA(UINT_PTR ptrBegin, LPCSTR cszText, list<LPVOID>* pTextList);
	BOOL SearchTextW(UINT_PTR ptrBegin, LPCWSTR cwszText, list<LPVOID>* pTextList);

public:
	BOOL GetAllSameText(){ return m_bAllSameText; };
	BOOL GetIgnoreBufLen(){ return m_bIgnoreBufLen; };

	virtual void SetTransOption(CString strTransOption, COptionNode *pNode);
	virtual void SetSpecialOption(CString strSpecialOption, COptionNode *pNode);

	virtual BOOL ApplyTranslatedText(void *pArgText, PREGISTER_ENTRY pRegisters, CTransScriptParser *pParser, BOOL bIsDuplicated, void *pBackUp);

};


//////////////////////////////////////////////////////////////////////////
// CTransCommandSOW : SOW 방식을 위한 CTransCommand 확장 클래스
class CTransCommandSOW : public CTransCommand {
public:
	friend class CTransCommand;

protected:
	BYTE *m_pBackupPoint;
	BYTE *m_pBackupBuffer;
	UINT m_nBackupSize;

	// LEN() 관련
	int *m_pnBackupLenPoint;
	int m_nLenBytes;
	int m_nBackupLenPoint;
	bool m_bIsNullIncluded;

protected:
	CTransCommandSOW() : CTransCommand(), m_pBackupPoint(NULL), m_pBackupBuffer(NULL), m_nBackupSize(0),
						m_pnBackupLenPoint(NULL), m_nLenBytes(0), m_nBackupLenPoint(0), m_bIsNullIncluded(false)
		{ m_nTransMethod = TRANSCOMMAND_SOW; }
	~CTransCommandSOW() { RestoreBackup(); }

	// 원 데이타를 백업하고 번역문을 덮어씌웁니다.
	void DoBackupAndOverwrite(const PBYTE pBackupPoint, UINT nBackupSize);

public:
	virtual void RestoreBackup();
	virtual BOOL ChangeLength(void *pOrigBasePtr, CTransScriptParser *pParser, void *pNewBasePtr, void *pBackUp);
	virtual BOOL ApplyTranslatedText(void *pArgText, PREGISTER_ENTRY pRegisters, CTransScriptParser *pParser, BOOL bIsDuplicated, void *pBackUp);

};

//////////////////////////////////////////////////////////////////////////
// CTransCommandSMSTR : SMSTR 방식을 위한 CTransCommand 확장 클래스
class CTransCommandSMSTR : public CTransCommand {
public:
	friend class CTransCommand;

protected:
	BOOL	m_bIgnoreBufLen;
	BOOL	m_bMatchSize;
	BOOL	m_bForcePtr;
	BOOL	m_bNullchar;
	BOOL	m_bReverse;
	BOOL	m_bYuris;
	BOOL	m_bYurisP;
	BOOL	m_bWill;

	int m_YurisOffset;

protected:
	CTransCommandSMSTR() : 
		CTransCommand(), m_bIgnoreBufLen(FALSE), m_bMatchSize(FALSE), m_bForcePtr(FALSE), 
		m_bNullchar(FALSE), m_bReverse(FALSE), m_bYuris(FALSE), m_bYurisP(FALSE), m_bWill(FALSE)
		{ m_nTransMethod = TRANSCOMMAND_SMSTR;}
	~CTransCommandSMSTR() {}

	// 번역문의 길이에 따라 스마트 문자열을 적용시킵니다.
	BOOL ApplySmartString( LPVOID pDest, LPVOID pSrc, void *pBackUp );

public:
	virtual void SetSpecialOption(CString strSpecialOption, COptionNode *pNode);

	virtual void *GetTextPoint(CTransScriptParser *pParser, void *pBackUp, BOOL bPtrBack, void *pBackUpLength);

	virtual BOOL ChangeLength(void *pOrigBasePtr, CTransScriptParser *pParser, void *pNewBasePtr, void *pBackUp);

	virtual BOOL ApplyTranslatedText(void *pArgText, PREGISTER_ENTRY pRegisters, CTransScriptParser *pParser, BOOL bIsDuplicated, void *pBackUp);

};