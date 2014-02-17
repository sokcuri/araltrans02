#pragma once

/* 소통 창구 */
struct OpData{
	DWORD type;
	/* Type 데이터는 16진수다! 잊지말자!
	 *
	 * 0x01xx : 시스템 시그널
	 *     00 : 초기화(사용 안함)
	 *     01 : 종료 개시
	 *	   02 : 종료 완료(!!!)
	 *     03 : 비우기(사용 안함)
	 *
	 * 0x02xx : 텍스트 시그널
	 *     00 : 원문(932)
	 *     0x : 원문(Reserved)
	 *     10 : 번역문(949)
	 *     1x : 번역문(Reserved)
	 *
	 * 0xFFxx : 에러 발생
	 *     00 : Critical Section Lock 실패
	 *     01 : Queue가 비어있음
	 */
	std::wstring data; //최소 32Byte
};

bool pushOpQueue(OpData& sOP,bool isBack=true);
bool pushOpQueue(DWORD type,std::wstring data,bool isBack=true);

OpData popOpQueue();

bool isOpQueueEmpty();