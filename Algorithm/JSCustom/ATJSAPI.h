#pragma once
#include "DefATContainer.h"

using namespace v8;
using namespace std;

typedef vector<BYTE> BArray;

//Handle<Value> ATHookAddress(const Arguments& args);
//이 함수는 v8Wrapper에서 관리한다.

Handle<Value> ATGetModuleAddress(const Arguments& args);

Handle<Value> ATGetAddressHere(const Arguments& args);
Handle<Value> ATGetModuleNameHere(const Arguments& args);

Handle<Value> ATReadMemory(const Arguments& args);
Handle<Value> ATSaveMemory(const Arguments& args);

Handle<Value> ATAllocMemory(const Arguments& args);
Handle<Value> ATFreeMemory(const Arguments& args);
Handle<Value> ATProtectMemory(const Arguments& args);

Handle<Value> ATGetRegister(const Arguments& args);
Handle<Value> ATSetRegister(const Arguments& args);

Handle<Value> ATGetFlag(const Arguments& args);
Handle<Value> ATSetFlag(const Arguments& args);

Handle<Value> ATArrayToInt(const Arguments& args);
Handle<Value> ATArrayToFloat(const Arguments& args);
Handle<Value> ATArrayToString(const Arguments& args);

Handle<Value> ATIntToArray(const Arguments& args);
Handle<Value> ATFloatToArray(const Arguments& args);
Handle<Value> ATStringToArray(const Arguments& args);

Handle<Value> ATNOT(const Arguments& args);
Handle<Value> ATXOR(const Arguments& args);
Handle<Value> ATXAND(const Arguments& args);
Handle<Value> ATAND(const Arguments& args);
Handle<Value> ATOR(const Arguments& args);

Handle<Value> ATShiftLeft(const Arguments& args);
Handle<Value> ATShiftRight(const Arguments& args);
Handle<Value> ATRotateLeft(const Arguments& args);
Handle<Value> ATRotateRight(const Arguments& args);

Handle<Value> ATTranslateRaw(const Arguments& args);

Handle<Value> ATCallWinAPI(const Arguments& args);

Handle<Value> ATGetFile(const Arguments& args);
Handle<Value> ATSetFile(const Arguments& args);

Handle<Value> ATGetGameExePath(const Arguments& args);
Handle<Value> ATGetATDataPath(const Arguments& args);
Handle<Value> ATGetAralTransPath(const Arguments& args);

//Raw 함수 목록

void	SetAralAPI();
void	SetBasicData(LPVOID pAddress,PVOID pRegister);
void	MakeModuleList();
CONTAINER_FUNCTION_ENTRY& GetAralAPI();

DWORD	RawGetModuleAddress(wstring strModName);

DWORD	RawGetAddressHere();
wstring	RawGetModuleNameHere();

bool	RawReadMemory(DWORD dwAddress,BArray& arData,size_t uLen);
bool	RawSaveMemory(BArray& arData,DWORD dwAddress);

DWORD	RawAllocMemory(size_t uLen);
DWORD	RawFreeMemory(int nType,DWORD dwAddress,DWORD dwOpt,size_t uLen);
bool	RawProtectMemory(DWORD dwAddress,size_t uLen,DWORD dwProtect,DWORD& dwOld);

bool	RawGetRegister(int nRegister,DWORD& dwValue);
bool	RawSetRegister(DWORD dwValue,int nRegister);

bool	RawGetFlag(int nFlag,bool & bValue);
bool	RawSetFlag(bool bValue,int nFlag);

int64_t	RawArrayToInt(BArray& arData,bool bSigned);
double	RawArrayToFloat(BArray& arData);
wstring	RawArrayToString(BArray& arData,int nLocale,BArray arTerm);

BArray	RawIntToArray(int64_t nValue,size_t uLen);
BArray	RawFloatToArray(double dValue,size_t uLen);
BArray	RawStringToArray(wstring strData,int nLocale,BArray arTerm);

int64_t	RawNOT(int64_t nValue,size_t uLen);
int64_t	RawOR(int64_t nValue1,int64_t nValue2,size_t uLen);
int64_t	RawAND(int64_t nValue1,int64_t nValue2,size_t uLen);

int64_t	RawShift(int64_t nValue,int nBit,size_t uLen);
int64_t	RawRotate(int64_t nValue,int nBit,size_t uLen);

bool	RawTranslate(string& strData,string& strOut);

bool	RawCallWinAPI(wstring strDLLName,string strFuncName,vector<pair<BYTE,BArray>>& arArgs,DWORD& dwRet);

bool	RawGetFile(wstring strPath,BArray& arData);
bool	RawSetFile(BArray& arData,wstring strPath);

wstring	RawGetGameExePath();
wstring	RawGetATDataPath();
wstring	RawGetAralTransPath();