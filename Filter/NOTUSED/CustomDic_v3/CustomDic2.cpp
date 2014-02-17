#include "StdAfx.h"
#include "CustomDic2.h"
#include "tstring.h"
#include "Util.h"
#include "hash.hpp"

CAutoLoad	*m_pAutoLoad = NULL;

const char * cszDivider = "MadeByHideD";
const int nDividerLen = strlen(cszDivider);

CCustomDic2::CCustomDic2(){
	NullWord.WordLen=-1;
	NullWord.WordN=-1;

	KeyIndex.clear();
	KeyBook.clear();
	ValueList.clear();
	while(!KeyList.empty())
	{
		KeyList.pop();
	}

	m_bNoMargin=false;

	//AfxBeginThread
	m_pAutoLoad = new CAutoLoad;
}

void CCustomDic2::Init()
{
	m_pAutoLoad->StartThread();
}

void CCustomDic2::SetMarginMode(bool bNoMargin){
	m_bNoMargin=bNoMargin;
}

void CCustomDic2::End()
{
	if(m_pAutoLoad)
	{
		m_pAutoLoad->EndThread();
		delete m_pAutoLoad;
		m_pAutoLoad=NULL;
	}
}

CCustomDic2::~CCustomDic2(void)
{
	if(m_pAutoLoad)
	{
		m_pAutoLoad->EndThread();
		delete m_pAutoLoad;
		m_pAutoLoad=NULL;
	}
}

string CCustomDic2::GetValue(int KeyN)
{
	return ValueList[KeyN];
	//출력
}

void CCustomDic2::FileClear()
{
	m_pAutoLoad->FileClear();
}

DicWord CCustomDic2::FindKey(LPCSTR Source, int Start, int End)
{
	//우선 문자열을 잘라내도록 하자 =ㅅ=;
	char FindText[2048]="";
	int _End=(int)strlen(Source)-1; //마지막'지점'이니까

	if(End!=-1 && End<=_End){
		//End값이 -1이 아니고(-1은 끝까지!)
		//End값이 수용가능한 벙위 내에 있다면.
		_End=End;
	}

	int TextLen=0;
	for(int i=Start;i<=End;i++)FindText[TextLen++]=Source[i];
	FindText[TextLen]='\0';
	//복사


	//검색 작업 개시.
	map<int,int>::iterator iterIndex;
	int IndexChar=65536*TextLen+FindText[0]*256+FindText[1];

	iterIndex = KeyIndex.find(IndexChar); //1차 검색
	if(iterIndex==KeyIndex.end()){
		//값이 없다면.
		return NullWord;
	}
	else{
		UINT Hash=MakeStringHash(FindText); //Hash값
		map<UINT,DicWord>::iterator iterBook;

		int ValuePointer=iterIndex->second;

		iterBook = KeyBook[ValuePointer].find(Hash); //2차 검색
		if(iterBook != KeyBook[ValuePointer].end()){
			//2단계까지 모두 있다면,
			return iterBook->second;
		}
		else{
			//여기도 없다면,
			return NullWord;
		}
	}
}

void CCustomDic2::PreCustomDic(LPSTR Dest, LPCSTR Source)
{
	//Thread로부터 변경된 내용이 있는가 확인해서 받아온다.
	if(m_pAutoLoad->GetChanged())
	{
		g_cs.Lock();
		m_pAutoLoad->SetChanged();
		KeyIndex = m_pAutoLoad->GetKeyIndex();
		KeyBook = m_pAutoLoad->GetKeyBook();
		ValueList = m_pAutoLoad->GetValueList();
		g_cs.Unlock();
	}

	int SourceLen=(int)strlen(Source);
	DicWord TempWord;

	while(!KeyList.empty())
	{
		KeyList.pop();
	}

	string Temp;
	int SPoint=0;

	char atoi_tmp[10]="";

	for(int head=0;head<SourceLen;head++)
	{
		for(int tail=SourceLen-1;tail>head;tail--)
		{
			TempWord=FindKey(Source,(int)head,(int)tail);
			if(TempWord.WordLen>0){
				//값이 있다면!
				//빈값이라면 -1 (CCustomDic2::CCustomDic2에서 정의)가 나오게됨
				
				//앞부분을 복사
				Temp+=string(Source,SPoint,head-SPoint);

				//단어 추가
				KeyList.push(TempWord.WordN);
				Temp+= cszDivider;
				SPoint=tail+1;
				head=tail;
				break;
			}
		}
		 //2바이트 처리, 수행속도 향상용
         if((BYTE)Source[head]>0x80){
              //전각문자라면
			 if((BYTE)Source[head]<0xA0||(BYTE)Source[head]>0xDF)
              head++;
         }
	}
	Temp+=string(Source,SPoint,SourceLen-SPoint); //마무리

	lstrcpyA(Dest,Temp.c_str());
}

void CCustomDic2::PostCustomDic(LPSTR Dest, LPCSTR Source)
{
	string Temp=Source;
	string Temp2="";
	//Temp -> Temp2 흐름

	string::size_type DicTagHead = 0;
	string::size_type DicTagTail = 0;

	string::size_type LastTail = 0;

	bool Added=false;

	for(;;){
		//무한 반복
		DicTagHead=Temp.find(cszDivider,DicTagTail);
		if(DicTagHead != string::npos)
		{
			Temp2 += RemoveSpace(Temp.substr(DicTagTail,DicTagHead-DicTagTail),!Added);
			DicTagTail=DicTagHead+nDividerLen;
			LastTail = DicTagTail;

			if(!KeyList.empty())
			{
				Temp2 += GetValue(KeyList.front());
				KeyList.pop();
			}
			//값을 추가.
			Added=true;
		}
		else{
			//남아있지 않으니까[...]
			break;
		}
	}
	Temp2 += RemoveSpace(Temp.substr(LastTail,Temp.length()-LastTail),!Added);
		
	//마무리[...]

	lstrcpyA(Dest,Temp2.c_str());
}
string CCustomDic2::RemoveSpace(std::string &strText,bool bHead){
	//IsDBCSLeadByteEx

	if(!m_bNoMargin)return strText;

	int nLen = strText.length();

	int nStart=0;
	int nMinus=0;

	if(!bHead&&strText[0]==' '){
		nStart++;
		nMinus++;
	}
	for(int i=0;i<nLen;i++){
		if(IsDBCSLeadByteEx(949,strText[i])){
			i++;
		}
		else if(i==nLen-1){
			if(strText[i]==' '){
				nMinus++;
			}
		}
	}

	if(nMinus==0)return strText;
	else return strText.substr(nStart,nLen-nMinus);

}
void CCustomDic2::AddDic(int Mode, LPCWSTR CustomPath)
{
	//파일 열기
	wchar_t Path[4096]=L"";
	int PathLen=(int)wcslen(CustomPath);
	
	//Mode 구분
	//0 - 사용자 정의
	//1 - GDic
	//나머지 - CDic

	if(Mode==0)
	{
		wcscpy_s(Path,4096,CustomPath);
		for(int i=PathLen-1;i>=0;i--){
			if(Path[i]==L'\\'){
				//파일의 디렉토리부가 잡혔다면!
				wchar_t CharTmp=Path[i+1];
				Path[i+1]=L'\0';
				MyCreateDirectory(Path); //일단 만들라고 시켜야지!
				Path[i+1]=CharTmp;
				break;
			}
		}
	}
	else if(Mode==1)
	{
		wcscpy_s(Path,4096,GetATDirectory());
		wcscat_s(Path,4096,L"\\");
		MyCreateDirectory(Path);
		wcscat_s(Path,4096,L"CustomDic.txt");
	}
	else
	{
		wcscpy_s(Path,4096,GetGameDirectory());
		wcscat_s(Path,4096,L"\\ATData\\");
		MyCreateDirectory(Path);
		wcscat_s(Path,4096,L"CustomDic.txt");
	}

	m_pAutoLoad->AddDic(Path);
}