#include "stdafx.h"
#include "SubFunc.h"

void atoh(LPSTR szDest,LPCSTR szSource)
{
	int nTemp=0;
	while((BYTE)*szSource)
	{
		nTemp += sprintf_s(szDest+nTemp,STRLEN-nTemp,"%02X",(BYTE)*szSource);
		szSource++;
	}
}
string atoh(string strSource)
{
	//±ÍÂú...
	char szTemp[STRLEN]="";
	atoh(szTemp,strSource.c_str());
	return string(szTemp);
}

/*void wtoh(LPWSTR szDest, LPCWSTR szSource)
{
	int nTemp=0;
	while((WORD)*szSource)
	{
		nTemp += sprintf_s(szDest+nTemp,STRLEN-nTemp,"%04X",(WORD)*szSource);
		szSource++;
	}
}
string stl_wtoh(wstring strSource)
{
	char szTemp[STRLEN]="";
	wtoh(szTemp,strSource.c_str());
	return string(szTemp);
}*/

void htoa(LPSTR szDest,LPCSTR szSource)
{
	bool bIsLow=false;
	BYTE Temp=0;

	while((BYTE)*szSource)
	{
		if('0'<=*szSource&&*szSource<='9')
		{
			Temp += *szSource-'0';
		}
		else if('A'<=*szSource&&*szSource<='F')
		{
			Temp += *szSource - 'A' + 0x0A;
		}
		else if('a'<=*szSource&&*szSource<='f')
		{
			Temp += *szSource - 'a' + 0x0A;
		}

		if(bIsLow) //µÞÀÚ¸®?
		{
			bIsLow=false;
			*szDest = Temp;
			szDest++;

			*szDest = 0x00;
			Temp = 0;
		}
		else
		{
			bIsLow=true;
			Temp *= 0x10;
		}
		szSource++;
	}
}

string htoa(string strSource)
{
	char Temp[STRLEN];
	htoa(Temp,strSource.c_str());
	return string(Temp);
}

string DecodeLine(string strSource)
{

	string strDest;

	int Len=strSource.length();

	for(int i=0;i<Len;i++)
	{
		if(strSource[i]=='\\')
		{
			i++;
			switch(strSource[i])
			{
			case '\\':
				strDest.push_back('\\');
				break;
			case 'r':
				strDest.push_back('\r');
				break;
			case 'n':
				strDest.push_back('\n');
				break;
			case 't':
				strDest.push_back('\t');
				break;
			default:
				i--;
				strDest.push_back('\\');
				break;
			}
		}
		else if(strSource[i] == '%')
		{
			if(isxdigit(strSource[i+1])&&isxdigit(strSource[i+2]))
			{
				strDest += htoa(strSource.substr(i+1,2));
				i+=2;
			}
			else
			{
				i++;
				switch(strSource[i])
				{
				case '%':
					strDest.push_back('%');
					break;
				case 't':
				case 'T':
					strDest.push_back((char)0x02);
					strDest.push_back((char)0x02);
					break;
				case 's':
				case 'S':
					strDest.push_back((char)0x01);
					break;
				default:
					i--;
					strDest.push_back('%');
					break;
				}
			}
		}
		else
		{
			strDest.push_back(strSource[i]);
		}
	}

	return strDest;
}

int Find(string strSource, string strFind, int nStart, int nCodePage)
{
	int nSize= strSource.length();
	int nSize2= strFind.length();
	int nReturn = -1;

	while(nStart<=nSize-nSize2)
	{
		if(strSource[nStart]==strFind[0] ||
			strFind[0] == (char)0x01 ||
			strFind[0] == (char)0x02)
		{
			int nSubSize = strFind.length();

			nReturn = nStart;

			//±æÀÌ Ã¼Å©

			if(strFind[0] == (char)0x01 &&
				IsDBCSLeadByteEx(nCodePage,BYTE(strSource[nStart])))
			{
				nReturn = -1;
				nSubSize=0;
			}
			else if(strFind[0] == (char)0x02 &&
				!IsDBCSLeadByteEx(nCodePage,BYTE(strSource[nStart])))
			{
				nReturn = -1;
				nSubSize=0;
			}

			for(int i=1;i<nSubSize;i++)
			{
				if(nStart+i>= nSize)
				{
					nReturn = -1;
					break;
				}

				if(strFind[i] == (char)0x01)
				{
					if(IsDBCSLeadByteEx(nCodePage,BYTE(strSource[nStart+i])))
					{
						nReturn = -1;
						break;
					}
				}
				else if(strFind[i] == (char)0x02)
				{
					if(IsDBCSLeadByteEx(nCodePage,BYTE(strSource[nStart+i])))
					{
						i++;
					}
					else
					{
						nReturn = -1;
						break;
					}
				}
				else if(strSource[nStart+i]!=strFind[i])
				{
					nReturn = -1;
					break;
				}
			}

			if(nReturn != -1)
			{
				break;
			}
		}

		if(IsDBCSLeadByteEx(nCodePage,BYTE(strSource[nStart])))
		{
			nStart++;
		}
		nStart++;
	}

	return nReturn;
}

string itoa(int n)
{
	char szTemp[20];
	_itoa_s(n,szTemp,20,10);
	return string(szTemp);
}

string wtoa(wstring wstrText,int nCodePage)
{
	char szTemp[2048]="";
	MyWideCharToMultiByte(nCodePage,0,wstrText.c_str(),-1,szTemp,2048,0,0);
	return string(szTemp);
}

wstring atow(string strText,int nCodePage)
{
	wchar_t wszTemp[2048]=L"";
	MyMultiByteToWideChar(nCodePage, 0,strText.c_str(), -1, wszTemp, 2048);
	return wstring(wszTemp);
}

string trim(string& strText,int nType){
	int nLeft=0;
	int nRight=strText.length()-1;
	if(nType<=0)nLeft=strText.find_first_not_of(" ");
	if(nType>=0)nRight=strText.find_last_not_of(" ");
	if(nRight-nLeft+1==strText.length()){
		return strText;
	}
	else{
		return strText.substr(nLeft,nRight-nLeft+1);
	}
}