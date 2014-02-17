#pragma once
#include <vector>
#include "tstring.h"
#include "SubFunc.h"

using namespace std;

// STL의 String을 기반으로 하는 문자열 클래스
// 함수, 문자열 모두 담을 수 있다.

struct FLStringElement
{
	bool bFunc;
	bool bTrans;
	int nSize; //-2 : 길이 없음, -1 : 자동, 0~ : 길이 수동
	int nLen; //실제 길이
	bool bDel;
	bool bPass;
	string strText;
	int nMin; //-1 : 사용 안함
	int nMax; //-1 : 사용 안함
	
	//구버전(v1) 데이터
	/*
	int nType;
	//0:번역텍스트(길이있음)	1:번역텍스트(길이없음)
	//2:미번역텍스트(길이있음)	3:미번역텍스트(길이없음)
	//4:함수(길이있음)			5:함수(길이없음)
	string strText;
	int nLen;

	bool bDelete; //이녀석을 출력하지 않음
	*/
};

class FLString
{
public:
	FLString(bool bFunc=true);
	FLString(string strText, int nType=0, bool bDelete=0); //구버전 호환 -_-;
	//FLString(string strText, bool bFunc=false, bool bTrans=true, int nSize=-1,
	//	bool bDel=false, bool bPass=false,int nMin=-1, int nMax=-1); //신버전... 왜이리 기냐
	~FLString(void);

	void PushBack(string strText, bool bFunc=false, bool bTrans=true, int nSize=-1,
		bool bDel=false, bool bPass=false,int nMin=-1, int nMax=-1);
	void PushBack(int nType,string strText,int nLen,bool bDelete);
	void PushBack(FLStringElement Type,string strText);

	//이게 뭐야 OTL
	static FLString MakeString(string strOText,int * nLength,FLString FLType,int nStart=0);
	static FLString MakeString(string strOText,int * nLength,FLString FLType, vector<vector<int>> FuncList,int nStart=0);
	static void TryRecur(int MaxLen,int TmpMax,FLString* FLType,vector<vector<int>>* FuncList,vector<vector<int>>* CompleteList,vector<int>* ArrangedList,int nThis,int nPos);

	void Clear();	//초기화

	string GetString();						//전체 변환된 문장 반환
	int GetLength();							//전체 길이 반환
	bool GetFunc();
	int GetType();


	void SetFunc(bool bFunc);
	void SetType(int nType);
	void SetTrim(bool bTrim);
	bool GetTrim();
	int GetSize(); //요소 갯수 반환

	FLStringElement GetSubElement(int nNumber);
	string GetSubString(int nNumber); //요소의 텍스트 보내기
	bool GetSubFunc(int nNumber); //요소의 형식 반환
	int GetSubSize(int nNumber,bool bRealSize=true);  //요소의 길이형식 반환
	int GetSubLen(int nNumber);	//요소의 실제 길이 반환
	bool GetSubTrans(int nNumber); //요소의 번역 여부 반환
	bool GetSubDelete(int nNumber); //요스의 삭제 형식 반환
	bool GetSubPass(int nNumber); //요소의 무시 형식 반환
	int GetSubMin(int nNumber);
	int GetSubMax(int nNumber);

	void SetSubString(int nNumber,string strText); //요소 텍스트 재설정


private:
	vector <FLStringElement> mElementPool;
	int m_nType; //0:일반 1:문두 2:문미 3: 전체
	bool m_bFunc;
	int m_nFullLen;
	string m_strFullString;

	bool m_bChanged;
	bool m_bUseTrim; //앞뒤 공백 제거
};
