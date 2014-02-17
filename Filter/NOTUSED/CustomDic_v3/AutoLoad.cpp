#include "StdAfx.h"
#include "AutoLoad.h"
#include "SubFunc.h"
#include "hash.hpp"

volatile bool g_bRunning;
CWinThread * g_pLoadThread = NULL;
CCriticalSection g_cs;

CAutoLoad::CAutoLoad(void)
{
}

void CAutoLoad::StartThread(void)
{
	g_cs.Lock();
	g_bRunning=true;
	g_cs.Unlock();
	g_pLoadThread = AfxBeginThread(CallbackStub,this,THREAD_PRIORITY_NORMAL,0,CREATE_SUSPENDED,NULL);

	g_pLoadThread->ResumeThread();
}

void CAutoLoad::EndThread(void)
{
	g_cs.Lock();
	g_bRunning=false;
	g_cs.Unlock();
	if(NULL != g_pLoadThread)
	{
		::WaitForSingleObject(g_pLoadThread->m_hThread,INFINITE);
		g_pLoadThread = NULL;
	}
}

CAutoLoad::~CAutoLoad(void)
{
	if(NULL != g_pLoadThread)
	{
		g_cs.Lock();
		g_bRunning=false;
		g_cs.Unlock();
		::WaitForSingleObject(g_pLoadThread->m_hThread,INFINITE);
		g_pLoadThread = NULL;
	}
}

void CAutoLoad::FileClear()
{
	g_cs.Lock();
	FileList.clear();
	FileWriteTime.clear();
	g_cs.Unlock();
}

UINT CAutoLoad::CallbackStub(LPVOID param)
{
	int i=7;
	while(1)
	{
		g_cs.Lock();
		if(!g_bRunning)
		{
			g_cs.Unlock();
			break;
		}
		else
		{
			g_cs.Unlock();
			i++;
			if(i>=6)
			{
				i=0;
				reinterpret_cast<CAutoLoad*>(param)->Callback();
			}
		}
		 Sleep(500);
	}
	return 0L;
}

void CALLBACK CAutoLoad::Callback()
{
	//파일체크 개시
	g_cs.Lock();
	int FileN=(int)FileList.size(); //Size값이 실시간으로 변하지 않도록
	g_cs.Unlock();

	bool bReload=true;
	HANDLE h_file=NULL;
	for(int i=0;i<FileN;i++)
	{
		h_file=CreateFile(FileList[i].c_str(), GENERIC_READ, FILE_SHARE_READ, NULL,
																			 OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);


		if(h_file != INVALID_HANDLE_VALUE){
			FILETIME create_time, access_time, write_time;

			// 지정된 파일에서 파일의 생성, 최근 사용 그리고 최근 갱신된 시간을 얻는다.
			GetFileTime(h_file, &create_time, &access_time, &write_time);
			CloseHandle(h_file);

			g_cs.Lock();
			if(write_time.dwLowDateTime==FileWriteTime[i])
			{
				FileWriteTime[i]=write_time.dwLowDateTime;
				bReload=false;
			}
			g_cs.Unlock();
		}
	}
	

	if(bReload)
	{
		ResetDic();
		g_cs.Lock();
		for(int i=0;i<FileN;i++)
		{
			ReadDic(FileList[i].c_str());
		}
		m_bChanged=true;
		g_cs.Unlock();
	}
}

void CAutoLoad::AddDic(LPCWSTR Path)
{
	g_cs.Lock();
	FileList.push_back(Path);
	FileWriteTime.push_back(0);
	g_cs.Unlock();
}

void CAutoLoad::ReadDic(LPCWSTR Path)
{
	//파일 열기
	FILE * Src;

	_wfopen_s(&Src,Path,L"rt,ccs=UTF-8");

	if(!Src)
	{
		//파일이 없다면, 설명서를 만든다.
		_wfopen_s(&Src,Path,L"wt,ccs=UTF-8");
		if(Src)
		{
			fwprintf(Src,L"///////////////////////////\n");
			fwprintf(Src,L"//사용자 사전 문서\n");
			fwprintf(Src,L"///////////////////////////\n");
			fwprintf(Src,L"//\n");
			fwprintf(Src,L"//사용자 사전 플러그인용 단어 목록 파일입니다.\n");
			fwprintf(Src,L"//일본어\t한국어\n");
			fwprintf(Src,L"//의 형식으로 단어를 추가 할 수 있습니다.\n");
			fwprintf(Src,L"//\n");
			fwprintf(Src,L"//자세한 사용법은 http://aralgood.com/zbxe/61760 를 참고해주세요.\n");
			fwprintf(Src,L"//\n");
			fclose(Src);
		}
	}
	else
	{
		wchar_t _LineIn[3500]=L""; //LPSTR기준으로 2048이 한계니까, 반절해서 1024 * 3 + 약간 더

		//파일이 열렸다면 '읽는다'
		while(fgetws(_LineIn,3456,Src))
		{
			int LineLen=0;

			wstring LineIn=L"";
			wstring JpnWLine=L"";
			wstring KorWLine=L"";

			int Tmp=0;

			LineLen=(int)wcslen(_LineIn);
			if(_LineIn[LineLen-1]==L'\n')LineLen--; //엔터 제거
			for(Tmp=0;Tmp<LineLen;Tmp++){
				if(_LineIn[Tmp]==L'/' && _LineIn[Tmp+1]==L'/'){
					//주석 기호를 찾았다면
					break;
				}
			}
			LineIn=wstring(_LineIn,Tmp);
			LineLen=Tmp;
			//주석 처리 완료. 이후부턴 wstring LineIn에서 처리
			
			int Tab[4][2]={0,0,0,0,0,0,0,0};
			int TabFlag=0;

			//탭찾기, 3개 이후로는 별 필요가 없다.
			for(Tmp=0;Tmp<LineLen;Tmp++){
				if(LineIn[Tmp]==L'\t'){
					if(TabFlag==5)
					{
						//더 셀것 없음
						break;
					}
					else 
					{
						if(TabFlag%2==0)
						{
							//TabFlag가 짝수(head)
							Tab[TabFlag/2][0]=Tmp;
							TabFlag++;
						}
						if(TabFlag%2==1)
						{
							//TabFlag가 홀수(tail)
							//이 경우엔 맨 뒤까지 찾는다
							Tab[TabFlag/2][1]=Tmp;

							if(LineIn[Tmp+1]!=L'\t')TabFlag++;
						}
					}
				}
			}//탭찾기 종료

			if(LineLen > 0 && LineIn[LineLen-1]!=L'\t')
			{
				Tab[TabFlag/2][TabFlag%2] = LineLen;
			}
			else if(TabFlag>0)
			{
				TabFlag--;
			}
			for(int i=TabFlag+1;i<6;i++)
			{
				Tab[i/2][i%2] = Tab[TabFlag/2][TabFlag%2];
			}

			bool bEnhance=false;
			bool bIsWord=false;
			bool bIsOnlyJpn=false;

			//탭 구분 2
			
			//지원 가능한 형식
			//일본어(탭들)한국어
			//일본어(탭들)한국어(탭들)
			//(탭들)일본어확장(탭들)한국어확장
			//(탭들)일본어확장(탭들)한국어확장(탭들)

			//확장구분
			if(Tab[0][0]==0 && LineLen > 0)
			{
				bEnhance=true;

				if(Tab[1][0]!=0)
				{
					bIsWord=true;
				}
				if(bIsWord && Tab[2][0] <= Tab[1][1])
				{
					bIsOnlyJpn=true;
				}
				
				if(bIsWord)JpnWLine=LineIn.substr(Tab[0][1]+1,Tab[1][0]-Tab[0][1]-1);
				else JpnWLine=LineIn.substr(Tab[0][1]+1,LineLen-Tab[0][1]);//원문 복사 모드
				if(!bIsOnlyJpn)
				{
					KorWLine=LineIn.substr(Tab[1][1]+1,Tab[2][0]-Tab[1][1]-1);
				}
			}
			else if(LineLen > 0)
			{
				bEnhance=false;

				if(Tab[0][0]!=0)
				{
					bIsWord=true;
				}
				if(bIsWord && Tab[1][0] <= Tab[0][1])
				{
					bIsOnlyJpn=true;
				}
				
				if(bIsWord)JpnWLine=LineIn.substr(0,Tab[0][0]);
				else JpnWLine=LineIn.substr(0,LineLen);//원문 복사 모드
				if(!bIsOnlyJpn)
				{
					KorWLine=LineIn.substr(Tab[0][1]+1,Tab[1][0]-Tab[0][1]-1);
				}
			}

			if(bIsWord&&!bIsOnlyJpn){
				char JpnALine[2048]="";
				char KorALine[2048]="";

				MyWideCharToMultiByte(932,0,JpnWLine.c_str(),-1,JpnALine,2048,NULL,NULL);
				MyWideCharToMultiByte(949,0,KorWLine.c_str(),-1,KorALine,2048,NULL,NULL);
				//Ansi로 변환

				if(bEnhance)
				{
					//확장 모드인 경우
					string strJpnLine = DecodeLine(JpnALine);
					string strKorLine = DecodeLine(KorALine);

					strcpy_s(JpnALine,2048,strJpnLine.c_str());
					strcpy_s(KorALine,2048,strKorLine.c_str());
				}

				SetKey(JpnALine,KorALine);
			}
			else if(bIsWord&&bIsOnlyJpn){
				//원문 복사모드
				//탭없이 문장만 넣을때는 한글쪽에 그대로 뿌린다.
				char JpnALine[2048]="";
				MyWideCharToMultiByte(932,0,JpnWLine.c_str(),-1,JpnALine,2048,NULL,NULL);

				if(bEnhance)
				{
					//확장 모드
					string strJpnLine = DecodeLine(JpnALine);

					strcpy_s(JpnALine,2048,strJpnLine.c_str());
				}
				SetKey(JpnALine,JpnALine);
			}
		}
		fclose(Src);
	}
	
}

void CAutoLoad::ResetDic()
{
	g_cs.Lock();
	KeyIndex.clear();
	KeyBook.clear();

	ValueList.clear();

	WordN=0;
	BookN=0;
	g_cs.Unlock();
}

void CAutoLoad::SetKey(LPCSTR JpnWord, LPCSTR KorWord)
{
	map<UINT,DicWord> EmptyMap;
	DicWord TempWord;

	map<int,int>::iterator iterIndex;

	TempWord.WordLen = (int)strlen(JpnWord);
	TempWord.WordN = WordN++;

	UINT Hash=MakeStringHash(JpnWord);
	int IndexChar=(int)TempWord.WordLen*65536+JpnWord[0]*256+JpnWord[1];
	int ValuePointer=0;

	iterIndex = KeyIndex.find(IndexChar);
	if(iterIndex==KeyIndex.end()){
		//값이 들어있지 않다면
		KeyIndex[IndexChar]=BookN;
		KeyBook.push_back(EmptyMap);
		//새 맵을 집어넣는다.

		ValuePointer=BookN++;
	}
	else{
		//값이 들어있다면
		ValuePointer=iterIndex->second;
	}

	KeyBook[ValuePointer][Hash]=TempWord;
	ValueList.push_back(KorWord);
	//삽입
}

map<int,int> CAutoLoad::GetKeyIndex()
{
	return KeyIndex;
}

vector<map<UINT,DicWord>> CAutoLoad::GetKeyBook()
{
	return KeyBook;
}

vector<string> CAutoLoad::GetValueList()
{
	return ValueList;
}

bool CAutoLoad::GetChanged()
{
	g_cs.Lock();
	bool tmp=m_bChanged;
	g_cs.Unlock();
	return tmp;
}

void CAutoLoad::SetChanged()
{
	m_bChanged=false;
}