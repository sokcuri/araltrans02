#include "StdAfx.h"
#include "CustomDic.h"
#include "DicStruct.h"
#include "AutoLoadDic.h"
#include "OptionDlg.h"
#include "SubFunc.h"

BEGIN_MESSAGE_MAP(CustomDic, CWinApp)
END_MESSAGE_MAP()

CustomDic::CustomDic(void)
	:m_strFullToken("MadeByHideD"),
	m_pAutoLoad(NULL),
	m_pOption(NULL)
{
}

BOOL CustomDic::InitInstance(){
	// 응용 프로그램 매니페스트가 ComCtl32.dll 버전 6 이상을 사용하여 비주얼 스타일을
	// 사용하도록 지정하는 경우, Windows XP 상에서 반드시 InitCommonControlsEx()가 필요합니다.
	// InitCommonControlsEx()를 사용하지 않으면 창을 만들 수 없습니다.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 응용 프로그램에서 사용할 모든 공용 컨트롤 클래스를 포함하도록
	// 이 항목을 설정하십시오.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	return TRUE;
}

CustomDic::~CustomDic(void)
{
}

bool CustomDic::Initialize(){
	if(m_pAutoLoad==NULL){
		m_pAutoLoad = new AutoLoadDic(this);
	}
	if(m_pOption==NULL){
		m_pOption = new COptionDlg();
		m_pOption->LoadOption();
	}

	m_csLock.Lock();

	m_PreDicDB.clear();
	m_PostDicDB.clear();
	m_infoDic.clear();

	auto& fileList = m_pOption->m_arFileList;
	auto& preList = m_pOption->m_arPreList;
	auto& postList = m_pOption->m_arPostList;

	_fullDic nullDic;

	for(auto iter0 = preList.begin();iter0!=preList.end();++iter0){
		int idx = *iter0;
		m_PreDicDB.push_back(nullDic);
		auto iter = m_PreDicDB.end();
		--iter;

		fileInfo& fInfo = fileList[idx];

		auto infoIter = m_infoDic.find(fInfo.name);
		if(infoIter == m_infoDic.end()){
			DBInfo newDB;
			newDB.UsePre=true;
			newDB.UsePost=false;

			newDB.preIter = iter;
			m_infoDic.insert(make_pair(fInfo.name,newDB));
		}
		else{
			infoIter->second.UsePre=true;
			infoIter->second.preIter=iter;
		}
	}

	for(auto iter0=postList.begin();iter0!=postList.end();++iter0){
		int idx = *iter0;
		
		m_PostDicDB.push_back(nullDic);
		auto iter = m_PostDicDB.end();
		--iter;

		fileInfo& fInfo = fileList[idx];
		auto infoIter = m_infoDic.find(fInfo.name);
		if(infoIter == m_infoDic.end()){
			DBInfo newDB;
			newDB.UsePre=false;
			newDB.UsePost=true;

			newDB.postIter = iter;
			m_infoDic.insert(make_pair(fInfo.name,newDB));
		}
		else{
			infoIter->second.UsePost=true;
			infoIter->second.postIter=iter;
		}
	}

	m_csLock.Unlock();

	m_pAutoLoad->SetDic(fileList);
	m_pAutoLoad->FirstLoad();

	m_pAutoLoad->SetInterval(m_pOption->m_nUseAutoLoad*1000);
	m_bTrimWord = m_pOption->m_bTrimWord;

	m_pAutoLoad->StartAutoLoad();

	return true;
}

bool CustomDic::Option(){
	m_pAutoLoad->StopAutoLoad();

	m_pOption->m_nUseAutoLoad = m_pAutoLoad->GetInterval()/1000;
	m_pOption->m_bTrimWord = m_bTrimWord;

	m_pOption->DoModal();

	bool bRet;

	if(m_pOption->isChanged()){
		Initialize();
		bRet=true;
	}
	else{
		m_pAutoLoad->SetInterval(m_pOption->m_nUseAutoLoad*1000);
		m_bTrimWord = m_pOption->m_bTrimWord;

		m_pAutoLoad->StartAutoLoad();
		bRet=false;
	}
	
	return bRet;

}

bool CustomDic::Close(){
	m_pAutoLoad->StopAutoLoad();

	delete m_pAutoLoad;
	delete m_pOption;
	m_pAutoLoad=NULL;
	m_pOption=NULL;
	return true;
}

void CustomDic::ChangeDic(wstring strFileName,_fullDic& dicList){
	auto iter = m_infoDic.find(strFileName);

	if(iter==m_infoDic.end())return;

	auto& info = iter->second;

	m_csLock.Lock();
	
	if(info.UsePre){
		ASSERT(info.preIter!=m_PreDicDB.end());
		info.preIter->swap(dicList);
	}
	else if(info.UsePost){
		ASSERT(info.postIter!=m_PostDicDB.end());
		info.postIter->swap(dicList);
	}
	m_csLock.Unlock();
}
void CustomDic::ChangeDic(wstring strFileName,_fullDic& dicPre,_fullDic& dicPost){
	auto iter = m_infoDic.find(strFileName);

	if(iter==m_infoDic.end())return;

	auto& info = iter->second;

	m_csLock.Lock();
	ASSERT(info.preIter!=m_PreDicDB.end()&&info.postIter!=m_PostDicDB.end());

	info.preIter->swap(dicPre);
	info.postIter->swap(dicPost);

	m_csLock.Unlock();
}

bool CustomDic::PreTranslate(const string& preStr,string& postStr){
	list<TokenStruct> arPartIn;
	list<TokenStruct> arPartOut;

	const int cnBaseCP = 932;

	TokenStruct tmp;
	AddDBCSInfo(tmp.dbcs,preStr,cnBaseCP);
	tmp.aux=0;
	tmp.token=preStr;

	arPartOut.push_back(tmp);

	m_TokenList.clear();
	m_csLock.Lock();

	PatternProcessing(arPartOut,arPartIn,m_PreDicDB,cnBaseCP);
	
	m_csLock.Unlock();

	postStr.resize(0);

	for(auto iterToken=arPartOut.begin();iterToken!=arPartOut.end();++iterToken){
		TokenStruct& tmp = *iterToken;
		if(tmp.aux==0){
			postStr += tmp.token;
		}
		else{
			m_TokenList.push_back(tmp.aux);
			postStr += m_strFullToken;
		}
	}

	return true;
}

bool CustomDic::PostTranslate(const string& preStr, string& postStr){
	list<TokenStruct> arPartIn;
	list<TokenStruct> arPartOut;

	const int cnBaseCP = 949;


	//원본 문장을 쪼개서 TokenList로 변환!

	size_t nIdx=0;

	vector<bool> arDBCS;
	arDBCS.reserve(preStr.size());
	AddDBCSInfo(arDBCS,preStr,cnBaseCP);

	while(!m_TokenList.empty()){
		int nTmpIdx = preStr.find(m_strFullToken,nIdx);
		if(nTmpIdx<0){
			ASSERT(false);//사실 토큰 갯수가 저장된 List보다 적어선 안된다.
			m_TokenList.clear();
			break;
		}
		else{
			int nNext= nTmpIdx+m_strFullToken.length();
			if(m_bTrimWord){
				nNext = preStr.find_first_not_of(' ',nNext);
				while(nTmpIdx>0){
					int newIdx = nTmpIdx-1;
					if(arDBCS[newIdx]==true){
						break;
					}
					else if(preStr[newIdx]!=' '){
						break;
					}
					--nTmpIdx;
				}
			}

			if(nTmpIdx-nIdx>0){
				TokenStruct tmp;
				tmp.aux=0;
				tmp.token=preStr.substr(nIdx,nTmpIdx-nIdx);
				tmp.dbcs.insert(tmp.dbcs.end(),arDBCS.begin()+nIdx,arDBCS.begin()+nTmpIdx);

				arPartOut.push_back(tmp);
			}

			TokenStruct tmp;
			tmp.aux=m_TokenList.front();
			m_TokenList.pop_front();

			arPartOut.push_back(tmp);

			nIdx = nNext;
		}
	}

	if(nIdx>=0 && nIdx < preStr.length()){
		TokenStruct tmp;
		tmp.aux=0;
		tmp.token=preStr.substr(nIdx);
		tmp.dbcs.insert(tmp.dbcs.end(),arDBCS.begin()+nIdx,arDBCS.end());
		arPartOut.push_back(tmp);
	}

	m_csLock.Lock();

	PatternProcessing(arPartOut,arPartIn,m_PostDicDB,cnBaseCP);
	
	m_csLock.Unlock();

	ASSERT(arPartOut.size()<=1);//바보짓을 하지 않았다면 하나 또는 0개
	postStr.resize(0);

	//이 경우 토큰이 0개이거나 의외의 삽질을 했을때도 대응 가능하다.
	for(auto iterToken = arPartOut.begin();iterToken != arPartOut.end(); ++iterToken){
		postStr+=iterToken->token;
	}

	return true;
}

void CustomDic::PatternProcessing(list<TokenStruct>& arPartOut,list<TokenStruct>& arPartIn,const list<_fullDic>& baseDicDB,const int cnBaseCP){
	for(auto iterFile=baseDicDB.begin();iterFile!=baseDicDB.end();++iterFile){
		for(auto iterRank = iterFile->begin();iterRank!=iterFile->end();++iterRank){
			arPartIn.swap(arPartOut);
			arPartOut.clear();

			arPartOut.push_back(TokenStruct());
			auto nextToken = arPartOut.end();
			--nextToken;
			nextToken->aux=0;

			for(auto iterToken=arPartIn.begin();iterToken!=arPartIn.end();++iterToken){
				TokenStruct& thisToken = *iterToken;

				if(thisToken.aux>0){
					int nTokenKey = thisToken.aux%256;
					auto& result1 = (*iterRank)[0][nTokenKey];

					int nTokenVal = thisToken.aux/256;
					auto& result2 = result1.find(nTokenVal);

					if(result2==result1.end()){
						//여기에선 이 Token은 안쓰이는 모양이다
						arPartOut.push_back(thisToken);
						arPartOut.push_back(TokenStruct());
						nextToken=arPartOut.end();
						--nextToken;
						nextToken->aux=0;
					}
					else{
						//빙고! 받아오자
						ASSERT(result2->second.size()==1);
						auto& datum = *(result2->second.begin());
						nextToken->token += datum.value;
						AddDBCSInfo(nextToken->dbcs,datum.value,cnBaseCP);
					}
				}//Aux끝
				else{
					int nIdx = 0;
					int nLen = thisToken.token.length();

					ASSERT(thisToken.token.size()==thisToken.dbcs.size());

					while(nIdx<nLen){
						BYTE ch = thisToken.token[nIdx];

						m_Hashed.resize(0);
						int nEnd;

						const wordInfo* finalResult=NULL;

						for(nEnd=nLen;nEnd>nIdx;nEnd-=1+thisToken.dbcs[nEnd-1]){
							int nTokenLen = nEnd-nIdx;
							int nFindLen = nTokenLen>MAXSTRLEN?MAXSTRLEN:nTokenLen;

							auto& result1 = (*iterRank)[nFindLen][ch];
							if(result1.empty())continue;

							string strTmp = thisToken.token.substr(nIdx,nTokenLen);
							UINT uHash;
							if(m_Hashed.size()==0){
								m_Hashed.resize(nTokenLen);
								hashStringList(m_Hashed.data(),strTmp.c_str());
							}
							uHash = m_Hashed[nTokenLen-1];

							auto iter = result1.find(uHash);
							if(iter==result1.end())continue;

							auto& matchList = iter->second;

							bool bFind = false;

							for(auto finalIter=matchList.begin();finalIter!=matchList.end();++finalIter){
								if(finalIter->key.compare(strTmp)==0){
									bFind=true;
									finalResult = &(*finalIter);
									break;
								}
							}

							if(bFind)break;
						}//끝자리 순회

						if(finalResult!=NULL){
							if(finalResult->value.empty()){
								TokenStruct tmp;
								tmp.aux=finalResult->aux;
								arPartOut.push_back(tmp);

								tmp.aux=0;
								arPartOut.push_back(tmp);
								nextToken = arPartOut.end();
								--nextToken;
							}
							else{
								AddDBCSInfo(nextToken->dbcs,finalResult->value,cnBaseCP);

								nextToken->token+=finalResult->value;
							}

							nIdx = nEnd;
						}
						else if(thisToken.dbcs[nIdx]){
							nextToken->token.push_back(thisToken.token[nIdx]);
							nextToken->token.push_back(thisToken.token[nIdx+1]);

							nextToken->dbcs.push_back(thisToken.dbcs[nIdx]);
							nextToken->dbcs.push_back(thisToken.dbcs[nIdx+1]);

							nIdx+=2;
						}
						else{
							nextToken->token.push_back(thisToken.token[nIdx]);
							nextToken->dbcs.push_back(thisToken.dbcs[nIdx]);

							nIdx++;
						}
					}//앞자리 순회
				}//일반 모드 끝
			}//iterToken끝
		}//iterRank끝
	}//iterFile끝
}

void CustomDic::Migration(string strOption){
	if(m_pOption==NULL){
		m_pOption = new COptionDlg();
	}

	if(!m_pOption->LoadOption()){
		bool bCDic=false;
		bool bGDic=false;
		bool bTrim=false;

		if(strOption.find("CDic")!=strOption.npos){
			bCDic=true;
		}

		if(strOption.find("GDic")!=strOption.npos){
			bGDic=true;
		}

		if(strOption.find("NoMargin")!=strOption.npos){
			bTrim=true;
		}

		m_pOption->Migration(bCDic,bGDic,bTrim);
	}

	Initialize();
}