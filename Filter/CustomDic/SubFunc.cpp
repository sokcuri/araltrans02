#include "stdafx.h"
#include "SubFunc.h"
#include "hash.hpp"

UINT hashStringList(UINT* hash_dest,const char * cszSource){
	char ch;
	UINT ret = 0;

	if(cszSource)
	{
		if(hash_dest){
			while(ch=*(cszSource++) ){
				ret = HashRotateLeft(ret,7) + ch;
				*hash_dest = ret;
				hash_dest++;
			}
		}
		else{
			return MakeStringHash(cszSource);
		}
		
	}
	return ret;
}

int atoh(const string& strText,int nIdx){
	int nRet=0;
	int nLen = strText.size();
	if(nIdx+1<nLen){
		char ch=strText[nIdx];
		char ch2=strText[nIdx+1];
		if('0'<=ch&&ch<='9'){
			nRet+= ch-'0';
		}
		else if('a'<=ch&&ch<='f'){
			nRet+= ch-'a'+0xa;
		}
		else if('A'<=ch&&ch<='F'){
			nRet+= ch-'A'+0xa;
		}
		else{
			return 0;
		}
		nRet*=0x10;
		if('0'<=ch2&&ch2<='9'){
			nRet+= ch2-'0';
		}
		else if('a'<=ch2&&ch2<='z'){
			nRet+= ch2-'a'+0xa;
		}
		else if('A'<=ch2&&ch2<='Z'){
			nRet+= ch2-'A'+0xa;
		}
		else{
			return 0;
		}
	}
	return nRet;
}

string parseText(const string strText){
	vector<char> buff(20);

	string strDest;
	strDest.reserve(strText.size());//언제나 strDest<=strText

	int nIdx=0;
	int tmp=0;
	int nLen = strText.size();
	while(nIdx<nLen){
		char ch = strText[nIdx];
		switch(ch){
		case L'%':
			tmp=0;
			if(strText[nIdx+1]=='%'){
				strDest.push_back('%');
				nIdx+=2;
			}
			else{
				int tmp=atoh(strText,nIdx+1);
				if(tmp!=0){
					strDest.push_back((char)tmp);
					nIdx+=3;
				}
				else{
					strDest.push_back('%');
					nIdx++;
				}
			}
			break;
		case '\\':
			switch(strText[nIdx+1]){
			case '\\':
				strDest.push_back('\\');
				nIdx+=2;
				break;
			case 'r':
				strDest.push_back('\r');
				nIdx+=2;
				break;
			case 'n':
				strDest.push_back('\n');
				nIdx+=2;
				break;
			case 't':
				strDest.push_back('\t');
				nIdx+=2;
				break;
			case 'x':
				{//tmp 사용을 위한 편법
					int tmp = atoh(strText,nIdx+2);
					if(tmp!=0){
						strDest.push_back((char)tmp);
						nIdx+=4;
					}
					else{
						strDest.push_back('\\');
						nIdx++;
					}
				}
			default:
				strDest.push_back('\\');
				nIdx++;
			}
			break;
		default:
			strDest.push_back(ch);
			nIdx++;
			break;
		}
	}

	return strDest;
}
wstring trim(wstring strText){

	//TestCase
	//""
	//" "  
	//" a "

	int start=0;
	int end=strText.length();

	for(int i=0;i<end;i++){
		wchar_t wch = strText[i];
		if(wch==L' '||wch==L'\t'){
			continue;
		}
		start=i;
		break;
	}

	for(int i=end-1;i>=start;i--){
		wchar_t wch = strText[i];
		if(wch==L' '||wch==L'\t'){
			continue;
		}
		end=i+1;
		break;
	}

	return strText.substr(start,end-start);
}

map<wstring,wstring> parseHeader(wstring strText,bool bIsFileName){
	map<wstring,wstring> retVal;
	if(bIsFileName){
		//파일의 첫줄을 읽어 //Header{DicType:"Pre",Comment:"사용자사전 12월 12일자 버전입니다."} 의 구조를 파싱해온다.
		//문제는 파일의 인코딩이 유니코드이기만 하면 OK라는거[...]

		//BOM이 있는 경우 UTF-16LE, UTF-16BE, UTF-8를 지원하고
		//BOM이 없는 경우 UTF-8로 인식

		vector<BYTE> buff(4,0x00);//UTF-8 NoBOM의 경우 3글자 + 종결문자열로 인해 4개
		vector<wchar_t> buff2;
		CFile cFile;
		if(!cFile.Open(strText.c_str(),CFile::modeRead|CFile::shareDenyWrite)){
			AfxMessageBox(L"파일이 없거나 열수 없습니다!",MB_ICONSTOP );
			return retVal;
		}

		int nType=0;
		int nIdx=0;

		cFile.Read(buff.data(),3);
		if(buff[0]==0xFF&&buff[1]==0xFE){
			//UTF-16LE
			nType=0;
			buff[0]=buff[2];
			nIdx=1;
		}
		else if(buff[0]==0xFE&&buff[1]==0xFF){
			//UTF-16BE
			nType=1;
			buff[0]=buff[2];
			nIdx=1;
		}
		else if(buff[0]==0xEF&&buff[1]==0xBB&&buff[2]==0xBF){
			//UTF-8 With BOM
			nType=2;
		}
		else{
			//UTF-8 With NoBOM
			nType=2;
			nIdx=3;
		}

		static const int cnLineBase=240;
		
		////Header{DicType="Body", Comment="아랄 사용자 사전 - 원문->번역문 Ver 2009.11.10, 65230"}
		//가 상식적인 최고 길이로 가정하고(-_-)
		//첫줄이 (78+2)*3=240 바이트가 넘어가진 않길 바라며 작성한 코드다
		
		//물론 첫줄의 최대 글자 제한은 없지만
		//그 길이를 넘어가면 좀 비효율적으로 작동한다.

		while(true){
			buff.resize(buff.size()+cnLineBase,0);
			int nReadCount=cFile.Read(buff.data()+nIdx,cnLineBase);

			//읽어온 부분을 UTF-16LE로 변환해 buff2로 넣는다.
			if(nType==0){
				buff2.resize(buff.size()/2+1,0);
				memcpy(buff2.data(),buff.data(),nIdx+nReadCount);//순서가 같으니 그냥 밀어붙이면 만사OK
			}
			else if(nType==1){
				buff2.resize(0);
				for(size_t i=0;i<buff.size();i+=2){
					wchar_t wch = (((wchar_t)buff[i])<<8)|((wchar_t)buff[i+1]);//망할 타입 캐스팅 -_-
					buff2.push_back(wch);
				}
				buff2.push_back(L'\0');//안전빵
			}
			else{
				int nNeedCount = MyMultiByteToWideChar(CP_UTF8,0,(LPCSTR)buff.data(),-1,NULL,NULL);
				buff2.resize(nNeedCount);
				MyMultiByteToWideChar(CP_UTF8,0,(LPCSTR)buff.data(),-1,buff2.data(),nNeedCount);
			}

			//buff2에 L'\n'이 있으면 종료
			auto iter = find(buff2.begin(),buff2.end(),L'\n');
			if(iter!=buff2.end()){
				buff2.resize(iter-buff2.begin()+1);
				break;
			}

			if(nReadCount<cnLineBase)break;
			//nReadCount<cnLineBase인 경우는
			//파일의 끝을 만나서 더 읽을 수 없는 경우다.

			nIdx+=cnLineBase;
		}
		cFile.Close();

		buff2.push_back(L'\0');
		wstring strHeader=buff2.data();
		return parseHeader(strHeader,false);
	}
	else{

		retVal[L"Status"] = L"OK";

		int nLen = strText.length();

		int nIdx=strText.find(L"Header");
		if(nIdx<0)return retVal;
		nIdx = strText.find(L'{',nIdx+6);
		if(nIdx<0)return retVal;
		nIdx++;

		while(nIdx<nLen){
			if(strText[nIdx]==L',')nIdx++;
			else if(strText[nIdx]==L'}')break;

			int newIdx = strText.find(L':',nIdx);
			wstring lhs = trim(strText.substr(nIdx,newIdx-nIdx));
			nIdx= newIdx+1;

			while(nIdx<nLen){
				wchar_t wch = strText[nIdx];
				if(wch==L' '||wch==L'\t')continue;
				break;
			}
			if(nIdx>=nLen)break;

			wstring rhs;
			if(strText[nIdx]!=L'"'){
				for(int nPosRhs=nIdx;nPosRhs<nLen;nPosRhs++){
					wchar_t wch = strText[nPosRhs];
					if(wch==L','||wch==L'}'){
						rhs = trim(strText.substr(nIdx,nPosRhs-nIdx));
						break;
					}
				}
			}
			else{
				for(int nPosRhs=nIdx+1;nPosRhs<nLen;nPosRhs++){
					wchar_t wch = strText[nPosRhs];
					if(wch==L'"'){
						nIdx=nPosRhs+1;
						break;
					}
					else if(wch==L'\\'){
						switch(strText[nPosRhs+1]){
						case L'r':
							rhs.push_back(L'\r');
							nPosRhs++;
							break;
						case L'n':
							rhs.push_back(L'\n');
							nPosRhs++;
							break;
						case L't':
							rhs.push_back(L'\t');
							nPosRhs++;
							break;
						case L'\\':
							rhs.push_back(L'\\');
							nPosRhs++;
							break;
						case L'"':
							rhs.push_back(L'"');
							nPosRhs++;
							break;
						default:
							rhs.push_back(L'\\');
							break;
						}
					}
					else{
						rhs.push_back(wch);
					}
				}//End for(nPosRhs;;)
				while(nIdx<nLen){
					wchar_t wch = strText[nIdx];
					if(wch==L','||wch==L'}'){
						break;
					}
					nIdx++;
				}
			}//End L'"'
			if(nIdx>=nLen)break;

			retVal[lhs] = rhs;
		}

		return retVal;
	}
}

void AddDBCSInfo(vector<bool>& arDBCS,const string& src,const int cp){
	for(size_t i=0;i<src.length();i++){
		if(IsDBCSLeadByteEx(cp,src[i])&&i+1<src.length()){
			arDBCS.push_back(true);
			arDBCS.push_back(true);
			i++;
		}
		else{
			arDBCS.push_back(false);
		}
	}
}