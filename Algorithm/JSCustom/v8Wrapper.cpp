#include "stdafx.h"
#include "v8Wrapper.h"
#include "ATJSAPI.h" //각종 Low Level 함수가 있는곳

#include <afxwin.h>
#include <afxmt.h>

#include <map>
#include <string>
#include <list>

#include <v8.h>

using namespace std;
using namespace v8;


///////////////////////////////////////////////
// 기본 정의된 구간입니다.
// 명확한 이유가 있지 않은 이상 변경하지 마세요.
///////////////////////////////////////////////

CEvent g_Event(0,TRUE);
CEvent g_SendEvent(0,TRUE);
CCriticalSection g_CS;
CWinThread * g_pThread=NULL;
list<v8message> g_v8MessageList;
list<wstring> g_scriptList;

list<pair<wstring,InvocationCallback>> g_funcList;

UINT v8ThreadProc(LPVOID arg);
Handle<String> ReadJS(wstring name);

void ConnectFunction();
bool ParseMessage(MessageType mType,v8key_t key,v8data_t value,bool bAsync);

//Persistent<Function>의 형태의 함수를 보관할 OnEvent 컨테이너
map<v8key_t,Persistent<Function>> g_EventMap;

void v8RunScript(const wchar_t * filepath){
	v8RunScript(&filepath,1);
}
void v8RunScript(const wchar_t ** filelist,int fileN){
	v8Reset();
	
	if(filelist==NULL){
		::MessageBoxW(NULL,L"파일 목록이 없습니다!",L"에러!",0);
	}

	for(int i=0;i<fileN;++i){
		const wchar_t * psz = filelist[i];
		if(psz!=NULL){
			g_scriptList.push_back(psz);
		}
	}

	g_pThread = AfxBeginThread(v8ThreadProc,NULL,0,0,CREATE_SUSPENDED);
	g_pThread->m_bAutoDelete=false;
	g_pThread->ResumeThread();
}

//v8 Thead에서 구동되는 함수입니다.
UINT v8ThreadProc(LPVOID arg){
	HandleScope handle_scope;

	Handle<ObjectTemplate> global = ObjectTemplate::New();

	for(auto iter=g_funcList.begin();iter!=g_funcList.end();++iter){
		global->Set(String::New(iter->first.c_str()),FunctionTemplate::New(iter->second));
	}

	Handle<Context> context = Context::New(NULL,global);
	Context::Scope context_scope(context);

	for(auto iter=g_scriptList.begin();iter!=g_scriptList.end();++iter){
		Handle<String> script_name = String::New(iter->c_str());
		Handle<String> script_data = ReadJS(*iter);
		Handle<Script> script;
		{
			TryCatch try_catch;
			script = Script::Compile(script_data,script_name);
			if(script.IsEmpty()){
				MessageBoxW(NULL,L"JavaScript 파일 컴파일 실패!\n구문이 잘못되었을 가능성이 있습니다!",L"열기 실패",0);
				return 0;
			}
		}

		{
			TryCatch try_catch;
			script->Run();
			/*if(try_catch.HasCaught()){
			
			}*/
		}
	}

	bool isRun=true;
	while(isRun){
		::WaitForSingleObject(g_Event.m_hObject,INFINITE);

		list<v8message> tempList;
		g_CS.Lock();
		if(!g_v8MessageList.empty()){
			tempList.splice(tempList.end(),g_v8MessageList);
		}
		g_Event.ResetEvent();
		g_CS.Unlock();

		while(!tempList.empty()){
			v8message message = tempList.front();
			tempList.pop_front();

			if(!ParseMessage(message.type,message.key,message.value,message.async)){
				isRun=false;
				break;
			}
		}
	}

	return 0;
}

void v8Reset(){
	if(g_pThread!=NULL){
		v8message exitMessage;
		exitMessage.type=EXIT;
		exitMessage.async=true;
		g_CS.Lock();
		v8SendMessage(exitMessage);
		g_Event.SetEvent();
		g_CS.Unlock();

		::WaitForSingleObject(g_pThread->m_hThread,INFINITE);
		g_pThread->Delete();

		g_pThread=NULL;
	}
	g_v8MessageList.clear();

	g_scriptList.clear();
	for(auto iter=g_EventMap.begin();iter!=g_EventMap.end();++iter){
		iter->second.Dispose();
	}
	g_EventMap.clear();
}

bool v8SendMessage(v8message& message){
	if(g_pThread==NULL)
	{
		return false;
	}

	g_CS.Lock();
	g_v8MessageList.push_back(message);
	g_Event.SetEvent();
	g_CS.Unlock();

	if(message.async){
		::WaitForSingleObject(g_SendEvent.m_hObject,INFINITE);
	}

	return true;
}

Handle<String> ReadJS(wstring name){

	//v8::HandleScope handle_scope;

	CFile cfile;
	if(cfile.Open(name.c_str(),CFile::modeRead|CFile::shareDenyWrite)==0){
		MessageBoxW(NULL,L"파일을 여는데 실패했습니다.",L"에러",0);
		return v8::Handle<v8::String>();
	}
	
	int size = (int)cfile.GetLength();

	char * chars = new char[size+1];
	chars[size] = '\0';

	for(int i =0; i<size;++i){
		int read = cfile.Read(&chars[i],size-i);
		i+=read;
	}

	cfile.Close();

	v8::Handle<v8::String> result = v8::String::New(chars,size);

	delete[] chars;
	return result;
}

inline void AddFunc(LPCWSTR sz,InvocationCallback func){
	g_funcList.push_back(make_pair(sz,func));
}

// Non Thread-Safe로 작동하는 자동 초기화 도구
class AutoCleaner{
public:
	AutoCleaner(){
		ConnectFunction();
	}
	~AutoCleaner(){
		v8Reset();
	}
} _c_cleaner;

////////////////////////////////////////
// 여기서부터 사용자 설정 구간입니다.
////////////////////////////////////////

// TODO: JavaScript에 연결할 함수를 구성합니다.
Handle<Value> ATHookAddress(const Arguments& args);

//v8 Thead에서 구동되는 함수입니다.
void ConnectFunction(){
	// TODO : JavaScript에 연결할 C++ 함수를 추가합니다.
	AddFunc(L"ATHookAddress_Rev0",ATHookAddress);

	AddFunc(L"ATGetModuleAddress_Rev0",ATGetModuleAddress);

	AddFunc(L"ATGetAddressHere_Rev0",ATGetAddressHere);
	AddFunc(L"ATGetModuleNameHere_Rev0",ATGetModuleNameHere);

	AddFunc(L"ATReadMemory_Rev0",ATReadMemory);
	AddFunc(L"ATSaveMemory_Rev0",ATSaveMemory);

	AddFunc(L"ATAllocMemory_Rev0",ATAllocMemory);
	AddFunc(L"ATFreeMemory_Rev0",ATFreeMemory);
	AddFunc(L"ATProtectMemory_Rev0",ATProtectMemory);

	AddFunc(L"ATGetRegister_Rev0",ATGetRegister);
	AddFunc(L"ATSetRegister_Rev0",ATSetRegister);

	AddFunc(L"ATGetFlag_Rev0",ATGetFlag);
	AddFunc(L"ATSetFlag_Rev0",ATSetFlag);

	AddFunc(L"ATArrayToInt_Rev0",ATArrayToInt);
	AddFunc(L"ATArrayToFloat_Rev0",ATArrayToFloat);
	AddFunc(L"ATArrayToString_Rev0",ATArrayToString);

	AddFunc(L"ATIntToArray_Rev0",ATIntToArray);
	AddFunc(L"ATFloatToArray_Rev0",ATFloatToArray);
	AddFunc(L"ATStringToArray_Rev0",ATStringToArray);

	AddFunc(L"ATNOT_Rev0",ATNOT);
	AddFunc(L"ATXOR_Rev0",ATXOR);
	AddFunc(L"ATXAND_Rev0",ATXAND);
	AddFunc(L"ATAND_Rev0",ATAND);

	AddFunc(L"ATShiftLeft_Rev0",ATShiftLeft);
	AddFunc(L"ATShiftRight_Rev0",ATShiftRight);
	AddFunc(L"ATRotateLeft_Rev0",ATRotateLeft);
	AddFunc(L"ATRotateRight_Rev0",ATRotateRight);

	AddFunc(L"ATTranslateRaw_Rev0",ATTranslateRaw);

	AddFunc(L"ATCallWinAPI_Rev0",ATCallWinAPI);

	AddFunc(L"ATGetFile_Rev0",ATGetFile);
	AddFunc(L"ATSetFile_Rev0",ATSetFile);

	AddFunc(L"ATGetGameExePath_Rev0",ATGetGameExePath);
	AddFunc(L"ATGetATDataPath_Rev0",ATGetATDataPath);
	AddFunc(L"ATGetAralTransPath_Rev0",ATGetAralTransPath);
}
bool ParseMessage(MessageType mType,v8key_t keyVal,v8data_t dataVal,bool bAsync){
	// TODO : 메시지를 분류합니다.
	if(mType==EXIT){
		if(bAsync)g_SendEvent.SetEvent();
		return false;
	}
	else if(mType==CALL){
		auto iter = g_EventMap.find(keyVal);
		if(iter!=g_EventMap.end()){
			HandleScope handle_scope;

			TryCatch try_catch;

			SetBasicData((LPVOID)keyVal,dataVal);

			auto& funcVal=iter->second;
			funcVal->Call(funcVal,0,NULL);

			/*if(try_catch.HasCaught()){
			}*/
		}
		if(bAsync)g_SendEvent.SetEvent();
	}

	return true;
}
void HookCallbackProc(LPVOID pHookedPoint, PREGISTER_ENTRY pRegisters){

	v8message message;
	message.type= CALL;
	message.key= (v8key_t)pHookedPoint;
	message.value = (v8data_t)pRegisters;
	message.async = true;

	v8SendMessage(message);

}

Handle<Value> ATHookAddress(const Arguments& args){
	HandleScope handle_scope;
	if(args.Length()!=3){
		return Boolean::New(false);
	}

	v8key_t keyData = args[1]->Uint32Value();
	if(args[0]->IsNull()){
	}
	else if(args[0]->IsNumber()){
		keyData+=args[0]->Uint32Value();
	}
	else if(args[0]->IsString()){
		String::Value text(args[0]);
		keyData+=RawGetModuleAddress(*text);
	}

	v8::Handle<v8::Function> func;
	if(args[2]->IsFunction()){
		func = Handle<Function>::Cast(args[2]);
	}
	else{
		return v8::Boolean::New(false);
	}
	Persistent<Function> func_backup = Persistent<Function>::New(func);
	g_EventMap.insert(make_pair(keyData,func_backup));

	bool bRet = GetAralAPI().pfnHookCodePoint((LPVOID)keyData,HookCallbackProc)!=FALSE;

	return v8::Boolean::New(bRet);
}
/*
Handle<Value> ATAlert(const Arguments& args){
	HandleScope handle_scope;
	wstring content;
	wstring title;
	int type=0;
	if(args.Length()>=1){
		v8::String::Value temp(args[0]);
		content = *temp;
	}
	if(args.Length()>=2){
		v8::String::Value temp(args[1]);
		title = *temp;
	}
	if(args.Length()>=3){
		type = args[2]->Int32Value();
	}

	::MessageBoxW(NULL,content.c_str(),title.c_str(),type);

	return v8::Null();
}*/