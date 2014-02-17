#include "StdAfx.h"
#include "ControlChar.h"

CControlChar::CControlChar(void)
{
}

CControlChar::~CControlChar(void)
{
}

// 맵에 컨트롤 문자열 입력
bool CControlChar::AddCtrStr(LPCSTR ControlHex,bool isViewed)
{
	int SourceLen=(int)strlen(ControlHex);

	char ControlStr[100]="";
	int Len=0;


	bool isFull=false;
	BYTE TempChar=0;

	for(int i=0;i<SourceLen;i++)
	{
		bool isChar=false;
		BYTE TempHalf=0;

		if(!isFull)TempChar=0;

		if('9'>=ControlHex[i] && ControlHex[i]>='0')
		{
			TempHalf=ControlHex[i]-'0';
			isChar=true;
		}
		else if('F'>=ControlHex[i] && ControlHex[i]>='A')
		{
			TempHalf=ControlHex[i]-'A'+10;
			isChar=true;
		}

		if(isChar)
		{
			if(!isFull)
			{
				isFull=true;
				TempChar=TempHalf*16;
			}
			else
			{
				isFull=false;
				TempChar+=TempHalf;
				ControlStr[Len++]=(char)TempChar;
			}
		}
	}
	ControlStr[Len]='\0';

	return false;
}

bool CControlChar::PreControlChar(LPSTR Dest, LPCSTR Source)
{
	return false;
}

bool CControlChar::PostControlChar(LPSTR Dest, LPCSTR Source)
{
	return false;
}
