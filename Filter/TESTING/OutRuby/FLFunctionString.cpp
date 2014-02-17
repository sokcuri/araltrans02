#include "StdAfx.h"
#include "FLFunctionString.h"

FLFunctionString::FLFunctionString(void)
{
	//초기화
	OptLen=0;
	Len=0;
	AddressLen=0;
	Type.clear();
	Text.clear();
	TOnly=false;
}

FLFunctionString::~FLFunctionString(void)
{
}

void FLFunctionString::InitFunc(int Len,int TextOnly, wstring FuncString)
{

	if(TextOnly>0)
	{
		FLFunctionString::TOnly=true;
	}
	else
	{
		FLFunctionString::TOnly=false;
	}

	OptLen=0;
	OptLen=Len;
	FLFunctionString::Len=Len;

	int Flag=-1;
	int OldFlag=-1;
	//Flag 구분
	//0 : 함수부분(Hex)
	//1 : 함수부분(문자열)
	//2 : 텍스트, 무시(^)
	//3 : 텍스트, 셈(*)
	//4 : 텍스트, 무시, 미번역(!)
	wstring::size_type FSLen=FuncString.length();
	wstring::size_type SPoint=0;
	wstring::size_type Divider=0;

	bool isSet=false;
	for(Divider=0;Divider<=FSLen;Divider++)
	{
		isSet=false;

		if(Divider==FSLen)
		{
			isSet=true;
		}
		else
		{
			switch(FuncString[Divider])
			{
			case L'h':
			case L'H':
				Flag=0;
				isSet=true;
				break;
			
			case L't':
			case L'T':
				Flag=1;
				isSet=true;
				break;

			case L'^':;
				Flag=2;
				isSet=true;
				break;

			case L'*':;
				Flag=3;
				isSet=true;
				break;

			case L'!':;
				Flag=4;
				isSet=true;
				break;

			default:
				break;
			}
		}

		if(isSet)
		{
			
			//구역은 SPoint~Divider-1까지

			if(OldFlag!=-1)
				Type.push_back(OldFlag);
			switch(OldFlag)
			{
			case 0:
			case 1:
				//hex 데이터
				Text.push_back(HexToString(FuncString.substr(SPoint+1,Divider-SPoint-1)));
				break;

			case 2:
			case 3:
			case 4:
				//일단 문자열
				Text.push_back("");
				break;
				
			default:;
			}

			if(OldFlag!=-1)
			{
				AddressLen++;
			}
			OldFlag=Flag;
			SPoint=Divider;
		}
	}
}
bool FLFunctionString::AddText(int Address, string PushText)
{
	if(Address<AddressLen&&Type[Address]>1)
	{
		//제대로 된 범위에 들어왔을 때에만!
		if(Type[Address]==3)
			Len-=(int)Text[Address].length();
		//원문 길이를 빼고(일본어 문장이 남아있을경우!)

		if(Text[Address].length()>0&&PushText.length()==0)
		{
			//원문은 내용이 있는데 번역문에 내용이 없으면
			Text[Address]="에러";
			//에러 출력
		}
		else
		{
			//정상 문장이라면
			Text[Address]=PushText;
			//텍스트를 추가
		}

		if(Type[Address]==3)
			Len+=(int)Text[Address].length();
		//길이를 더한다.

		return true;
		//성공했으므로
	}
	else
		return false;
}

string FLFunctionString::GetSubText(queue<int>* AddressList)
{
	//Source의 뒤에 _##*를 붙이며 텍스트를 잇는다.
	//그리고 Queue의 뒤에 현재 번호를 붙인다.

	
	string Source="";
	for(int i=0;i<AddressLen;i++)
	{
		if(Type[i]==2||Type[i]==3)
		{
			//타입이 번역하는 텍스느 ^,* 일때
			//!는 번역하지 않으므로 무시한다.
			AddressList->push(i);
			Source+="_##*";
			Source+=Text[i];
		}
	}
	AddressList->push(-1);//이녀석이 끝났으니 끝났다는 기호
	return Source;
}

string FLFunctionString::GetFullText()
{
	string Temp="";
	for(int i=0;i<AddressLen;i++)
	{
		Temp+=Text[i];
	}
	return Temp;
}

string FLFunctionString::GetOnlyText()
{
	string Temp="";
	for(int i=0;i<AddressLen;i++)
	{
		if(Type[i]==2)
		{
			Temp+=Text[i];
		}
	}
	return Temp;
}

int FLFunctionString::Length()
{
	return Len;
}

int FLFunctionString::FindFunc(std::string Source, int Start)
{
	//함수 원본을 이용해 소스 스트링에서 이것이 맞는가 찾아낸다.
	//찾아낸 이후에는 함수 길이를 반환!
	//없으면 물론 -1

	//Head Check
	int HeadLen=(int)Text[0].length();
	int Len=HeadLen;

	int SourceLen=(int)Source.length();

	for(int i=0;i<HeadLen;i++)
	{
		if(Start+i>=SourceLen)
		{
			//범위 밖으로 나가면
			Len=-1;
			break;
		}

		if(Text[0][i]!=Source[Start+i])
		{
			Len=-1;
			break;
		}
	}
	//Head 체크 완료

	
	if(Len!=-1)
	{
		string::size_type SPoint=Start+HeadLen;
		for(int Address=1;Address<AddressLen;Address++)
		{
			if(Type[Address]<2)
			{
				SPoint=Find(Source,Text[Address],SPoint,932);
				if(SPoint==Source.npos)
				{
					Len=-1;
					break;
				}
				else
				{
					Len=(int)SPoint+(int)Text[Address].length()-Start;
				}
			}
		}
	}
	return Len;
}

void FLFunctionString::InitText(string FuncSource)
{
	//String을 찾아 추가시킨다.
	//찾은 시점에서 추가시키는게 효율이 좋지만,
	//어쩔 수 없다.

	//추가되는 데이터 : 일어 원문

	vector <string::size_type> Where;
	Where.push_back(0);
	//주소 찾기
	//함수 영역만 찾으면 그것을 기준으로 텍스트 영역을 찾을 수 있다.
	for(int Address=1;Address<AddressLen;Address++)
	{
		if(Type[Address]<2)
		{
			Where.push_back(Find(FuncSource,Text[Address],Where[Address-1],932));
		}
		else
		{
			Where.push_back(Where[Address-1]+Text[Address-1].length());
		}
	}

	//텍스트추가
	for(int Address=1;Address<AddressLen-1;Address++)
	{
		if(Type[Address]>=2)
		{
			AddText(Address,FuncSource.substr(Where[Address],Where[Address+1]-Where[Address]));
		}
	}
}

bool FLFunctionString::TextOnly()
{
	return FLFunctionString::TOnly;
}

string::size_type FLFunctionString::Find(string Source,string Key, string::size_type Start,int CodePage)
{
	string::size_type Finder=Start;
	string::size_type Len=Source.length();
	string::size_type KeyLen=Key.length();

	string::size_type ReturnData=Source.npos;

	while(Finder<Len)
	{
		if(Source[Finder]==Key[0])
		{
			string::size_type KeyFinder=1;
			ReturnData=Finder;
			while(KeyFinder<KeyLen)
			{
				if(Source[Finder+KeyFinder]!=Key[KeyFinder])
				{
					ReturnData=Source.npos;
					break;
				}
				KeyFinder++;
			}
			
			if(ReturnData!=Source.npos)
			{
				//값을 찾았다면
				break;
			}
		}
		else if(IsDBCSLeadByteEx(CodePage,Source[Finder]))
		{
			Finder+=2;
		}
		else
		{
			Finder++;
		}
	}

	return ReturnData;
}