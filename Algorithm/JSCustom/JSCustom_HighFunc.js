/*
** JSCustom 고수준 함수 Library
** Revision : 0
*/

function AT1ArrTo2Arr_Rev0(Data){return ATNArrToNArr_Rev0(Data,1,2);}
function AT1ArrTo4Arr_Rev0(Data){return ATNArrToNArr_Rev0(Data,1,4);}
function AT2ArrTo1Arr_Rev0(Data){return ATNArrToNArr_Rev0(Data,2,1);}
function AT4ArrTo1Arr_Rev0(Data){return ATNArrToNArr_Rev0(Data,4,1);}

function ATNArrToNArr_Rev0(Data,InN,OutN){
	var TmpArray;
	
	if(!((InN==1||InN==2||InN==4)&&(OutN==1||OutN==2||OutN==4)))
		return null;
	
	if(InN==1){
		TmpArray=Data;
	}
	else{
		TmpArray = new Array(InN*Data.length);
		for(var i=0;i<Data.length;i++){
			var aNum = Data[i];
			for(var j=0;j<InN;j++){
				TmpArray[i*InN+j]=aNum%0x100;
				aNum/=100;
			}
		}
	}
	
	if(OutN==1){
		return TmpArray;
	}
	else{
		var OutArray = new Array();
		for(var i=0;i<Data.length;i+=OutN){
			var aNum=0;
			for(var j=OutN-1;j>=0;j--){
				
			}
		}
	}
}

function ATTranslate_Rev0(Text,LocaleIn='Shift-JIS',LocaleOut='Shift-JIS'){
	var BArraySrc = ATStringToArray_Rev0(Text,LocaleIn);
	if(BArraySrc == null)return Text;
	
	var BArrayDest = ATTranslateRaw_Rev0(BArraySrc);
	if(BArrayDest == null)return Text;
	
	var strRet = ATArrayToString_Rev0(BArrayDest,-1,LocaleOut);
	if(strRet == null)return Text;
	
	return strRet;
}

function ATCalcAddress_Rev0(AddressString){
	//시간 복잡도 : O(n^2)
	
	//숫자 처리
	if(typeof AddressString == typeof 1) return AddressString;
	
	//1차 파싱
	var TokenArray = new Array();
	String TempString = "";
	for(var i=0;i<AddressString.length;i++){
		var aChar = AddressString.charAt(i);
		if(aChar==' '||aChar=='\t')continue;
		else if(aChar=='"'){
			var j=i+1;
			for(;j<AddressString.length;j++){
				if(AddressString.charAt(j)=='"'){
					j++;
					break;
				}
			}
			TokenArray.push(AddressString.substring(i,j);
			i=j-1;
		}
		else if(aChar=='['){
			var j=i+1;
			var deepLevel=1;
			for(;j<AddressString.length;j++){
				switch(AddressString.charAt(j)){
				case '[':
					deepLevel++;
					break;
				case ']':
					deepLevel--;
					break;
				}
				if(deepLevel==0){
					j++;
					break;
				}
			}
			TokenArray.push(AddressString.substring(i,j);
			i=j-1;
		}
		else if(aChar=='('){
			var j=i+1;
			var deepLevel=1;
			for(;j<AddressString.length;j++){
				switch(AddressString.charAt(j)){
				case '(':
					deepLevel++;
					break;
				case ')':
					deepLevel--;
					break;
				}
				if(deepLevel==0){
					j++;
					break;
				}
			}
			TokenArray.push(AddressString.substring(i,j);
			i=j-1;
			continue;
		}
		else if(aChar=='+'||aChar=='-'||aChar=='*'||aChar=='/'||aChar=='%'){
			if(TempString.length>0){
				TokenArray.push(TempString);
				TempString="";
			}
			TokenArray.push(""+aChar);
		}
		else{
			TempString+=aChar;
		}
	}
	
	if(TokenArray.length==0){
		//에러
		return 0;
	}
	else if(TokenArray.length==1){
		//단일 토큰이므로 연산
		String retTemp = TokenArray[0];
		
		switch(retTemp.charAt(0){
		case '"':
			return ATGetModuleAddress_Rev0(retTmp.substring(1,retTemp.length-1));
		case '[':
			return ATGetIntFromMemory_Rev0(ATCalcAddress_Rev0(retTemp.substring(1,retTmp.length-1)));
		case '(':
			return ATCalcAddress_Rev0(retTemp.substring(1,retTmp.length-1));
		}
		
		if(retTemp=="EAX"||retTemp=="EBX"||retTemp=="ECX"||retTemp=="EDX"
			||retTemp=="EDI"||retTemp=="ESI"
			||retTemp=="EBP"||retTemp=="ESP"
			||retTemp=="EFL")
		{
			return ATGetRegister_Rev0(retTemp);
		}
		
		return parseInt(retTemp);
	}
	
	for(var i=0;<TokenArray.length;i++){
		var aStr = TokenArray[i];
		if(aStr == '+' || aStr == '-'|| aStr == '*'|| aStr == '/'|| aStr == '%'){
			continue;
		}
		else{
			TokenArray[i]=ATCalcAddress_Rev0(TokenArray[i]);
		}
	}
	
	
	//eval!!!!!!
	String strRet = TokenArray.join("");
	return eval(strRet);
}