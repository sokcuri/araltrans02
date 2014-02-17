/////////////////////////////////////////////////
// MFC + v8 의 단순 초기화 도구
/////////////////////////////////////////////////
//
// 단순하게 단일 스레드 내에서
// C++의 함수를 JavaScript 내부 함수로 연결하고
// 외부 함수의 콜을 내부로 이어주는 기능만 하는
// 모듈입니다.
//
// 2010-07-26. Hide_D
/////////////////////////////////////////////////
#include "DefATContainer.h"

#pragma once

// TODO : 전달하고자 하는 메시지를 구성합니다.
enum MessageType {EXIT=-1,NOP=0,CALL=1};

// TODO : 전달할 메시지 타입을 지정합니다.
typedef uint32_t v8key_t;
typedef PREGISTER_ENTRY v8data_t;

struct v8message{
	MessageType type;
	v8key_t key;
	v8data_t value;
	bool async;
};

void v8RunScript(const wchar_t *);
void v8RunScript(const wchar_t ** filelist,int fileN);

bool v8SendMessage(v8message& message);

void v8Reset();