#include "stdafx.h"
#include "OpQueue.h"

using namespace std;

CCriticalSection g_csQueue;
std::list<OpData> g_OpQueue;

bool pushOpQueue(OpData& sOP,bool isBack){
	
	g_csQueue.Lock();
	if(isBack){
		g_OpQueue.push_back(sOP);
	}
	else{
		g_OpQueue.push_front(sOP);
	}
	g_csQueue.Unlock();

	return true;
}


bool pushOpQueue(DWORD type,std::wstring data,bool isBack){
	OpData tmpOp;
	tmpOp.type = type;
	tmpOp.data = data;

	return pushOpQueue(tmpOp,isBack);
}

OpData popOpQueue(){
	OpData OpRet;

	g_csQueue.Lock();
	if(g_OpQueue.empty()){
		OpRet.type = 0xFF01;
	}
	else{
		OpRet = g_OpQueue.front();
		g_OpQueue.pop_front();
	}
	g_csQueue.Unlock();

	return OpRet;
}

bool isOpQueueEmpty(){
	bool bRet;

	g_csQueue.Lock();
	bRet = g_OpQueue.empty();
	g_csQueue.Unlock();

	return bRet;
}
