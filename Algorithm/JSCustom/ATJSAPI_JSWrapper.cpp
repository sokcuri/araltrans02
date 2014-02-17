#include "stdafx.h"
#include "v8Wrapper.h"
#include "ATJSAPI.h"

using namespace v8;
using namespace std;

//매크로

//매크로 함수들 내부에서 Revision 관리되는 함수를 사용할 수 있으므로
//변경시마다 Revision 체크를 해줘야한다.
inline void _ToUpper(wstring& data){
	for(size_t i=0;i<data.length();i++){
		wchar_t & wch = data[i];
		if(L'a'<=wch && wch<=L'z')wch+='A'-'a';
	}
}

inline DWORD _RawAddress(Handle<Value>& Name){
	if(Name->IsString()){
		String::Value text(Name);
		return RawGetModuleAddress(*text);//Revision 변경 필수!
	}
	else if(Name->IsNumber()){
		return Name->Uint32Value();
	}
	else{
		return 0;
	}
}
inline DWORD _RawAddress(Handle<Value>& Name,DWORD Address){
	if(Name->IsString()){
		String::Value text(Name);
		return Address+RawGetModuleAddress(*text);//Revision 변경 필수!
	}
	else if(Name->IsNumber()){
		return Address+Name->Uint32Value();
	}
	else{
		return Address;
	}
}
inline DWORD _RawAddress(Handle<Value>& Name,Handle<Value>& Address){
	return _RawAddress(Name,Address->Uint32Value());
}

inline void _ArConvert(Handle<Value>& jValue,size_t uIdx,BArray& arData){
	Handle<Array> jarData = Handle<Array>::Cast(jValue);
	for(size_t i=0;i<arData.size();i++){
		jarData->Set(i+uIdx,Integer::New(arData[i]));
	}
}

inline Local<Value> _ArConvert(BArray& arData){
	Local<Array> retVal(Array::New(arData.size()));
	for(size_t i=0;i<arData.size();i++){
		retVal->Set(i,Integer::New(arData[i]));
	}

	return retVal;
}

inline BArray _ArConvert(Handle<Value>& arData){
	if(!arData->IsArray()){
		return BArray();
	}

	Handle<Array> ar2 = Handle<Array>::Cast(arData);
	BArray retVal(ar2->Length());
	for(size_t i=0;i<retVal.size();i++){
		retVal[i]=(BYTE)(ar2->Get(i)->Int32Value());
	}

	return retVal;
}

inline int _GetLocale(Handle<Value>& jstrData){
	if(!jstrData->IsString()){
		return jstrData->Uint32Value();
	}
	String::Value tmp(jstrData);
	wstring str = *tmp;
	_ToUpper(str);

	int nRet=0;

	if(str==L"SHIFT-JIS")nRet=932;
	else if(str==L"SHIFT-JIS")nRet=932;
	else if(str==L"CP932")nRet=932;
	else if(str==L"CP-932")nRet=932;

	else if(str==L"CP949")nRet=949;
	else if(str==L"CP-949")nRet=949;
	else if(str==L"KS_C_5601_1987")nRet=949;
	else if(str==L"MS949")nRet=949;

	else if(str==L"UTF-8")nRet=65001;
	else if(str==L"UTF8")nRet=65001;
	else if(str==L"CP_UTF8")nRet=65001;

	else if(str==L"UTF16")nRet=1200;
	else if(str==L"UTF-16")nRet=1200;
	else if(str==L"UTF16LE")nRet=1200;
	else if(str==L"UTF-16LE")nRet=1200;

	else if(str==L"UTF16BE")nRet=1201;
	else if(str==L"UTF-16BE")nRet=1201;

	else if(str==L"UTF32")nRet=12000;
	else if(str==L"UTF-32")nRet=12000;
	else if(str==L"UTF32LE")nRet=12000;
	else if(str==L"UTF-32LE")nRet=12000;

	else if(str==L"UTF32BE")nRet=12001;
	else if(str==L"UTF-32BE")nRet=12001;

	return nRet;
}

inline int _GetRegister(Handle<Value>& jstrData){
	if(!jstrData->IsString()){
		return jstrData->Uint32Value();
	}
	String::Value tmp(jstrData);
	wstring str = *tmp;
	_ToUpper(str);

	int nRegister=11;

	if(str==L"EAX")nRegister=0;
	else if(str==L"EBX")nRegister=1;
	else if(str==L"ECX")nRegister=2;
	else if(str==L"EDX")nRegister=3;
	else if(str==L"ESI")nRegister=4;
	else if(str==L"EDI")nRegister=5;
	else if(str==L"ESP")nRegister=6;
	else if(str==L"EBP")nRegister=7;
	else if(str==L"EFL")nRegister=8;

	return nRegister;
}

inline int _GetFlag(Handle<Value>& jstrData){
	if(!jstrData->IsString()){
		return jstrData->Uint32Value();
	}
	String::Value tmp(jstrData);
	wstring str = *tmp;
	_ToUpper(str);

	int nFlag=11;

	if(str==L"C")nFlag=0;
	else if(str==L"P")nFlag=2;
	else if(str==L"A")nFlag=4;
	else if(str==L"Z")nFlag=6;
	else if(str==L"S")nFlag=7;
	else if(str==L"O")nFlag=11;
	else if(str==L"CF")nFlag=0;
	else if(str==L"PF")nFlag=2;
	else if(str==L"AF")nFlag=4;
	else if(str==L"ZF")nFlag=6;
	else if(str==L"SF")nFlag=7;
	else if(str==L"OF")nFlag=11;

	return nFlag;
}

inline BArray _HexToArray(Handle<Value>& jstrData){

	String::Value strTmp(jstrData);
	wstring strData = *strTmp;
	_ToUpper(strData);

	BArray arRet;
	arRet.reserve(strData.length()/2);

	bool isHead=true;
	BYTE Data=0;
	for(size_t i=0;i<strData.length();i++){
		wchar_t & wch = strData[i];
		if(wch==L' ')continue;
		if(!((L'0'<=wch&&wch<=L'9')||(L'A'<=wch&&wch<=L'F')))break;

		if(isHead)Data=0;
		Data*=16;
		if(L'0'<=wch&&wch<=L'9')Data+=wch-L'0';
		else					Data+=wch-L'A'+0xA;

		if(isHead){
			isHead=false;
		}
		else{
			arRet.push_back(Data);
			isHead=true;
		}
	}

	return arRet;
}

inline int _TermSize(int nLocale){
	if(nLocale>60000)return 1;
	else if(nLocale>10000)return 4;
	else if(nLocale>1000)return 2;
	else return 1;
}

//함수

Handle<Value> ATGetModuleAddress(const Arguments& args){
	HandleScope handle_scope;
	if(args.Length()!=1){
		return v8::Integer::New(0);
	}

	return Integer::NewFromUnsigned(_RawAddress(args[0]));
}

Handle<Value> ATGetAddressHere(const Arguments& args){
	HandleScope handle_scope;
	return Integer::NewFromUnsigned(RawGetAddressHere());
}
Handle<Value> ATGetModuleNameHere(const Arguments& args){
	HandleScope handle_scope;
	return String::New(RawGetModuleNameHere().c_str());
}

Handle<Value> ATReadMemory(const Arguments& args){
	HandleScope handle_scope;

	if(args.Length()!=2){
		return Null();
	}

	DWORD dwAddress= args[0]->Uint32Value();
	BArray arData;
	bool bRet = RawReadMemory(dwAddress,arData,args[1]->Uint32Value());

	if(!bRet)return Null();
	return _ArConvert(arData);
}

Handle<Value> ATSaveMemory(const Arguments& args){
	HandleScope handle_scope;

	size_t argc = (size_t)args.Length();
	if(argc!=2){
		return Boolean::New(false);
	}

	DWORD dwAddress = args[1]->Uint32Value();


	BArray arData=_ArConvert(args[0]);

	bool bRet = RawSaveMemory(arData,dwAddress);
	return Boolean::New(bRet);
}

Handle<Value> ATAllocMemory(const Arguments& args){
	HandleScope handle_scope;

	if(args.Length()!=1){
		return Null();
	}

	DWORD dwAddress = RawAllocMemory(args[0]->Uint32Value());
	if(dwAddress=0){
		return Null();
	}
	else{
		return Integer::NewFromUnsigned(dwAddress);
	}
}
Handle<Value> ATFreeMemory(const Arguments& args){
	HandleScope handle_scope;

	size_t argc = (size_t)args.Length();

	if(argc!=3 || argc!= 4)return Boolean::New(false);

	int nType=-1;
	if(args[0]->IsNumber()){
		nType= args[0]->Int32Value();
	}
	else{
		wstring strText = *String::Value(args[0]);
		if(strText==L"delete[]")nType=0;
		else if(strText==L"delete")nType=1;
		else if(strText==L"free")nType=2;
		else if(strText==L"HeapFree")nType=3;
		else if(strText==L"LocalFree")nType=4;
		else if(strText==L"GlobalFree")nType=5;
		else if(strText==L"VirtualFree")nType=6;
	}

	if(nType<0||nType>6)return Boolean::New(false);

	DWORD dwAddress = (DWORD)args[1]->Uint32Value();

	DWORD dwOpt = 0;
	if(argc>=3){
		dwOpt = args[3]->Uint32Value();;
	}

	size_t uLen = 0;
	if(argc==4){
		uLen = args[3]->Uint32Value();
	}

	DWORD uRet = RawFreeMemory(nType,dwAddress,dwOpt,uLen);
	return Boolean::New(uRet!=0);
}
Handle<Value> ATProtectMemory(const Arguments& args){
	HandleScope handle_scope;

	if(args.Length()!=3)return Null();

	DWORD dwAddress = args[0]->Uint32Value();
	size_t uLength = args[1]->Uint32Value();
	DWORD dwMode = args[2]->Uint32Value();

	DWORD dwRet=0;
	bool bOK = RawProtectMemory(dwAddress,uLength,dwMode,dwRet);

	if(!bOK)return Null();
	return Integer::NewFromUnsigned(dwRet);
}

Handle<Value> ATGetRegister(const Arguments& args){
	HandleScope handle_scope;

	if(args.Length()!=1)return Null();

	int nRegister=_GetRegister(args[0]);

	if(0<=nRegister&&nRegister<=8){
		DWORD dwRet=0;
		bool bRet=RawGetRegister(nRegister,dwRet);

		if(bRet)return Integer::NewFromUnsigned(dwRet);
	}

	return Null();
}
Handle<Value> ATSetRegister(const Arguments& args){
	HandleScope handle_scope;

	if(args.Length()!=2)return Boolean::New(false);

	int nRegister=_GetRegister(args[0]);
	
	if(0<=nRegister&&nRegister<=8){
		DWORD dwData = args[1]->Uint32Value();
		return Boolean::New(RawSetRegister(dwData,nRegister));
	}

	return Boolean::New(false);
}

Handle<Value> ATGetFlag(const Arguments& args){
	HandleScope handle_scope;

	if(args.Length()!=1)return Null();

	int nFlag = _GetFlag(args[0]);

	if(0<=nFlag && nFlag<32){
		bool bData=false;
		bool bRet = RawGetFlag(nFlag,bData);
		if(bRet){
			return Boolean::New(bData);
		}
		
	}

	return Null();
}
Handle<Value> ATSetFlag(const Arguments& args){
	HandleScope handle_scope;

	if(args.Length()!=2)return Null();

	int nFlag = _GetFlag(args[0]);

	if(0<=nFlag && nFlag<32){
		bool bData = args[1]->Uint32Value()!=0;
		return Boolean::New(RawSetFlag(bData,nFlag));
	}

	return Null();
}

Handle<Value> ATArrayToInt(const Arguments& args){
	HandleScope handle_scope;

	BArray arData;
	size_t uLength=4;
	bool bSigned=true;


	int argc=args.Length();
	if(argc>=5||argc<1)return Null();
	if(argc>=1)arData = _ArConvert(args[0]);
	if(argc>=2)uLength = args[1]->Uint32Value();
	if(argc==3){
		if(args[2]->IsBoolean())bSigned = args[2]->BooleanValue();
		else bSigned = args[2]->Uint32Value()!=0?true:false;
	}

	if(arData.size()<=uLength)return Null();

	arData.resize(uLength);

	int64_t lnRet = RawArrayToInt(arData,bSigned);
	if(lnRet<0){
		return Integer::NewFromUnsigned((uint32_t)lnRet);
	}
	else{
		return Integer::New((int32_t)lnRet);
	}
}
Handle<Value> ATArrayToFloat(const Arguments& args){
	HandleScope handle_scope;

	BArray arData;
	size_t uLength=4;

	int argc=args.Length();
	if(argc>2||argc<1)return Null();
	if(argc>=1)arData = _ArConvert(args[0]);
	if(argc>=2)uLength = args[1]->Uint32Value();

	if(arData.size()<=uLength)return Null();

	arData.resize(uLength);

	double dRet = RawArrayToFloat(arData);
	return Number::New(dRet);
}
Handle<Value> ATArrayToString(const Arguments& args){
	HandleScope handle_scope;

	BArray arData;
	int nLocale = 932;
	bool bDefaultEnd=true;
	BArray arTerminator;

	int argc=args.Length();
	if(argc>3||argc<1)return Null();
	if(argc>=1)arData = _ArConvert(args[0]);
	if(argc>=2)nLocale = _GetLocale(args[1]);
	if(argc>=3){
		bDefaultEnd=false;
		arTerminator = _HexToArray(args[2]);
	}

	if(bDefaultEnd){
		arTerminator.resize(_TermSize(nLocale),0);
	}

	wstring strData = RawArrayToString(arData,nLocale,arTerminator);

	return String::New(strData.c_str());

}

Handle<Value> ATIntToArray(const Arguments& args){
	HandleScope handle_scope;

	int nData=0;
	size_t uLength = 4;

	int argc=args.Length();
	if(argc>2||argc<1)return Null();
	if(argc>=1)nData = args[0]->Int32Value();
	if(argc>=2)uLength = args[1]->Uint32Value();

	return _ArConvert(RawIntToArray(nData,uLength));
}
Handle<Value> ATFloatToArray(const Arguments& args){
	HandleScope handle_scope;

	double dData=0;
	size_t uLength = 4;

	int argc=args.Length();
	if(argc>2||argc<1)return Null();
	if(argc>=1)dData = args[0]->NumberValue();
	if(argc>=2)uLength = args[1]->Uint32Value();

	return _ArConvert(RawFloatToArray(dData,uLength));
}
Handle<Value> ATStringToArray(const Arguments& args){
	HandleScope handle_scope;

	bool bDefaultEnd=true;
	wstring strData;
	int nLocale = 932;
	BArray arTerm;

	int argc=args.Length();
	if(argc>3||argc<1)return Null();
	if(argc>=1){
		String::Value tmpData(args[0]);
		strData = *tmpData;
	}
	if(argc>=2)nLocale=_GetLocale(args[1]);
	if(argc>=3){
		bDefaultEnd=false;
		arTerm = _HexToArray(args[2]);
	}

	if(bDefaultEnd){
		arTerm.resize(_TermSize(nLocale),0);
	}

	return _ArConvert(RawStringToArray(strData,nLocale,arTerm));

}

Handle<Value> ATNOT(const Arguments& args){
	HandleScope handle_scope;

	int64_t nData=0;
	size_t uLength=4;

	int argc=args.Length();
	if(argc>2||argc<1)return Null();
	if(argc>=1)nData = args[0]->IntegerValue();
	if(argc>=2)uLength = args[1]->Uint32Value();

	return Integer::NewFromUnsigned((uint32_t)RawNOT(nData,uLength));
}
Handle<Value> ATXOR(const Arguments& args){
	HandleScope handle_scope;

	int64_t nData1=0;
	int64_t nData2=0;
	size_t uLength=4;

	int argc=args.Length();
	if(argc>3||argc<1)return Null();
	if(argc>=1)nData1 = args[0]->IntegerValue();
	if(argc>=2)nData2 = args[1]->IntegerValue();
	if(argc>=3)uLength = args[2]->Uint32Value();

	return Integer::NewFromUnsigned((uint32_t)RawNOT(RawOR(nData1,nData2,uLength),uLength));
}
Handle<Value> ATXAND(const Arguments& args){
	HandleScope handle_scope;
	
	int64_t nData1=0;
	int64_t nData2=0;
	size_t uLength=4;

	int argc=args.Length();
	if(argc>3||argc<1)return Null();
	if(argc>=1)nData1 = args[0]->IntegerValue();
	if(argc>=2)nData2 = args[1]->IntegerValue();
	if(argc>=3)uLength = args[2]->Uint32Value();

	return Integer::NewFromUnsigned((uint32_t)RawNOT(RawAND(nData1,nData2,uLength),uLength));
}
Handle<Value> ATAND(const Arguments& args){
	HandleScope handle_scope;
	
	int64_t nData1=0;
	int64_t nData2=0;
	size_t uLength=4;

	int argc=args.Length();
	if(argc>3||argc<1)return Null();
	if(argc>=1)nData1 = args[0]->IntegerValue();
	if(argc>=2)nData2 = args[1]->IntegerValue();
	if(argc>=3)uLength = args[2]->Uint32Value();

	return Integer::NewFromUnsigned((uint32_t)RawAND(nData1,nData2,uLength));
}

Handle<Value> ATOR(const Arguments& args){
	HandleScope handle_scope;
	
	int64_t nData1=0;
	int64_t nData2=0;
	size_t uLength=4;

	int argc=args.Length();
	if(argc>3||argc<1)return Null();
	if(argc>=1)nData1 = args[0]->IntegerValue();
	if(argc>=2)nData2 = args[1]->IntegerValue();
	if(argc>=3)uLength = args[2]->Uint32Value();

	return Integer::NewFromUnsigned((uint32_t)RawOR(nData1,nData2,uLength));
}

Handle<Value> ATShiftLeft(const Arguments& args){
	HandleScope handle_scope;
	
	int64_t nData=0;
	int nBit=0;
	size_t uLength=4;

	int argc=args.Length();
	if(argc>3||argc<1)return Null();
	if(argc>=1)nData = args[0]->IntegerValue();
	if(argc>=2)nBit = args[1]->Int32Value();
	if(argc>=3)uLength = args[2]->Uint32Value();

	return Integer::NewFromUnsigned((uint32_t)RawShift(nData,nBit,uLength));
}
Handle<Value> ATShiftRight(const Arguments& args){
	HandleScope handle_scope;
	
	int64_t nData=0;
	int nBit=0;
	size_t uLength=4;

	int argc=args.Length();
	if(argc>3||argc<1)return Null();
	if(argc>=1)nData = args[0]->IntegerValue();
	if(argc>=2)nBit = -args[1]->Int32Value();
	if(argc>=3)uLength = args[2]->Uint32Value();

	return Integer::NewFromUnsigned((uint32_t)RawShift(nData,nBit,uLength));
}
Handle<Value> ATRotateLeft(const Arguments& args){
	HandleScope handle_scope;
	
	int64_t nData=0;
	int nBit=0;
	size_t uLength=4;

	int argc=args.Length();
	if(argc>3||argc<1)return Null();
	if(argc>=1)nData = args[0]->IntegerValue();
	if(argc>=2)nBit = args[1]->Int32Value();
	if(argc>=3)uLength = args[2]->Uint32Value();

	return Integer::NewFromUnsigned((uint32_t)RawRotate(nData,nBit,uLength));
}
Handle<Value> ATRotateRight(const Arguments& args){
	HandleScope handle_scope;
	
	int64_t nData=0;
	int nBit=0;
	size_t uLength=4;

	int argc=args.Length();
	if(argc>3||argc<1)return Null();
	if(argc>=1)nData = args[0]->IntegerValue();
	if(argc>=2)nBit = -args[1]->Int32Value();
	if(argc>=3)uLength = args[2]->Uint32Value();

	return Integer::NewFromUnsigned((uint32_t)RawRotate(nData,nBit,uLength));
}

Handle<Value> ATTranslateRaw(const Arguments& args){
	HandleScope handle_scope;

	BArray arData;
	string strData;

	int argc=args.Length();
	if(argc!=1)return Null();
	else{
		arData=_ArConvert(args[0]);
		arData.push_back(0);
		strData = (char*)arData.data();
	}

	string strOut;

	bool bRet=RawTranslate(strData,strOut);

	if(!bRet){
		return Null();
	}

	return String::New(strData.c_str());
}

Handle<Value> ATCallWinAPI(const Arguments& args){
	HandleScope handle_scope;

	int argc=args.Length();
	if(argc<2)return Null();

	wstring strDLLName=*String::Value(args[0]);
	string strFuncName=*String::AsciiValue(args[1]);

	vector<pair<BYTE,BArray>> arArgs;

	for(int i=2;i<argc;i++){
		Local<Value>& jArg=args[i];

		BYTE nMod=0;
		BArray arArg;
		if(jArg->IsArray()){
			arArg=_ArConvert(jArg);
			nMod=2;
		}
		else if(jArg->IsBoolean()){
			arArg=RawIntToArray(jArg->BooleanValue(),4);
		}
		else if(jArg->IsNumber()){
			arArg=RawIntToArray(jArg->IntegerValue(),4);
		}
		else if(jArg->IsString()){
			arArg=RawStringToArray(*String::Value(jArg),1200,BArray(2,0));
			nMod=1;
		}
		else if(jArg->IsNull()){
			arArg=RawIntToArray(0,4);
		}
		else{
			arArg=RawIntToArray(0,4);
		}
		
		arArgs.push_back(make_pair(nMod,arArg));
	}

	DWORD dwRet=0;
	bool bRet=RawCallWinAPI(strDLLName,strFuncName,arArgs,dwRet);

	if(!bRet){
		return Null();
	}

	for(int i=2;i<argc;i++){
		if(arArgs[i].first<2)continue;

		args[i]=_ArConvert(arArgs[i].second);
	}

	return Integer::NewFromUnsigned(dwRet);
}

Handle<Value> ATGetFile(const Arguments& args){
	HandleScope handle_scope;

	wstring strPath;

	int argc=args.Length();
	if(argc!=1)return Null();
	else{
		String::Value tmpPath(args[0]);
		strPath = *tmpPath;
	}

	BArray arRet;
	bool bRet=RawGetFile(strPath,arRet);

	if(!bRet)return Null();

	return _ArConvert(arRet);

}
Handle<Value> ATSetFile(const Arguments& args){
	HandleScope handle_scope;

	wstring strPath;
	BArray arData;

	int argc=args.Length();
	arData = _ArConvert(args[0]);
	strPath = *String::Value(args[1]);

	return Boolean::New(RawSetFile(arData,strPath));
}

Handle<Value> ATGetGameExePath(const Arguments& args){
	HandleScope handle_scope;

	return String::New(RawGetGameExePath().c_str());
}
Handle<Value> ATGetATDataPath(const Arguments& args){
	HandleScope handle_scope;

	return String::New(RawGetATDataPath().c_str());
}
Handle<Value> ATGetAralTransPath(const Arguments& args){
	HandleScope handle_scope;

	return String::New(RawGetAralTransPath().c_str());
}