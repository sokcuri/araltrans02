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

	string::size_type Len=strSource.length();

	for(string::size_type i=0;i<Len;i++)
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
					strDest.push_back(0x01);
					break;
				case 'r':
					strDest.push_back(0x02);
					break;
				case 'x':
					strDest.push_back(0x03);
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
	int nReturn = -1;

	while(nStart<nSize)
	{
		if(strSource[nStart]==strFind[0])
		{
			int nSubSize = strFind.length();

			nReturn = nStart;

			for(int i=1;i<nSubSize;i++)
			{
				if(strSource[nStart+i]!=strSource[i])
				{
					nReturn = -1;
					break;
				}
				else
				{
					nReturn++;
				}
			}

			if(nReturn != -1)
			{
				break;
			}
		}
		
		nStart++;
		if(IsDBCSLeadByteEx(nCodePage,strSource[nStart]))
			nStart++;
			
	}

	return nReturn;
}