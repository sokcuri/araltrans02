//함수Version관리내역

var_AT_FunctionList=new Array();

_AT_FunctionList["ATHookAddress"]			= [0];
_AT_FunctionList["ATGetAddressHere"]		= [0];
_AT_FunctionList["ATGetModuleNameHere"]		= [0];
_AT_FunctionList["ATGetModuleAddress"]		= [0];
_AT_FunctionList["ATReadMemory"]			= [0];
_AT_FunctionList["ATSaveMemory"]			= [0];
_AT_FunctionList["ATAllocMemory"]			= [0];
_AT_FunctionList["ATFreeMemory"]			= [0];
_AT_FunctionList["ATProtectMemory"]			= [0];
_AT_FunctionList["ATGetRegister"]			= [0];
_AT_FunctionList["ATSetRegister"]			= [0];
_AT_FunctionList["ATGetFlag"]				= [0];
_AT_FunctionList["ATSetFlag"]				= [0];
_AT_FunctionList["ATArrayToInt"]			= [0];
_AT_FunctionList["ATArrayToFloat"]			= [0];
_AT_FunctionList["ATArrayToString"]			= [0];
_AT_FunctionList["ATIntToArray"]			= [0];
_AT_FunctionList["ATFloatToArray"]			= [0];
_AT_FunctionList["ATStringToArray"]			= [0];
_AT_FunctionList["ATNOT"]					= [0];
_AT_FunctionList["ATAND"]					= [0];
_AT_FunctionList["ATOR"]					= [0];
_AT_FunctionList["ATXAND"]					= [0];
_AT_FunctionList["ATXOR"]					= [0];
_AT_FunctionList["ATShiftLeft"]				= [0];
_AT_FunctionList["ATShiftRight"]			= [0];
_AT_FunctionList["ATRotateLeft"]			= [0];
_AT_FunctionList["ATRotateRight"]			= [0];
_AT_FunctionList["ATTranslateRaw"]			= [0];
_AT_FunctionList["ATMakeContext"]			= [0];
_AT_FunctionList["ATConvertLocale"]			= [0];
_AT_FunctionList["ATAlert"]					= [0];
_AT_FunctionList["ATCallWinAPI"]			= [0];
_AT_FunctionList["ATGetFile"]				= [0];
_AT_FunctionList["ATSetFile"]				= [0];
_AT_FunctionList["ATGetGameExePath"]		= [0];
_AT_FunctionList["ATGetATDataPath"]			= [0];
_AT_FunctionList["ATGetAralTransPath"]		= [0];
_AT_FunctionList["ATNArrToNArr"]			= [0];
_AT_FunctionList["ATGetStringFromMemory"]	= [0];
_AT_FunctionList["ATGetIntFromMemory"]		= [0];
_AT_FunctionList["ATGetFloatFromMemory"]	= [0];
_AT_FunctionList["ATSetStringToMemory"]		= [0];
_AT_FunctionList["ATSetIntToMemory"]		= [0];
_AT_FunctionList["ATSetFloatToMemory"]		= [0];
_AT_FunctionList["ATCalcAddress"]			= [0];
_AT_FunctionList["ATTranslate"]				= [0];

var ATHookAddress = function(){return null;};
var ATGetAddressHere = function(){return null;};
var ATGetModuleNameHere = function(){return null;};
var ATGetModuleAddress = function(){return null;};
var ATReadMemory = function(){return null;};
var ATSaveMemory = function(){return null;};
var ATAllocMemory = function(){return null;};
var ATFreeMemory = function(){return null;};
var ATProtectMemory = function(){return null;};
var ATGetRegister = function(){return null;};
var ATSetRegister = function(){return null;};
var ATGetFlag = function(){return null;};
var ATSetFlag = function(){return null;};
var ATArrayToInt = function(){return null;};
var ATArrayToFloat = function(){return null;};
var ATArrayToString = function(){return null;};
var ATIntToArray = function(){return null;};
var ATFloatToArray = function(){return null;};
var ATStringToArray = function(){return null;};
var ATNOT = function(){return null;};
var ATAND = function(){return null;};
var ATOR = function(){return null;};
var ATXAND = function(){return null;};
var ATXOR = function(){return null;};
var ATShiftLeft = function(){return null;};
var ATShiftRight = function(){return null;};
var ATRotateLeft = function(){return null;};
var ATRotateRight = function(){return null;};
var ATTranslateRaw = function(){return null;};
var ATMakeContext = function(){return null;};
var ATConvertLocale = function(){return null;};
var ATAlert = function(){return null;};
var ATCallWinAPI = function(){return null;};
var ATGetFile = function(){return null;};
var ATSetFile = function(){return null;};
var ATGetGameExePath = function(){return null;};
var ATGetATDataPath = function(){return null;};
var ATGetAralTransPath = function(){return null;};
var ATNArrToNArr = function(){return null;};
var ATGetStringFromMemory = function(){return null;};
var ATGetIntFromMemory = function(){return null;};
var ATGetFloatFromMemory = function(){return null;};
var ATSetStringToMemory = function(){return null;};
var ATSetIntToMemory = function(){return null;};
var ATSetFloatToMemory = function(){return null;};
var ATCalcAddress = function(){return null;};
var ATTranslate = function(){return null;};

function ATSetLibraryVer(Revision){
	_AT_FunctionList.forEach(function(verList,funcName,_list_){
		var i = verList.length-1;
		for(;i>0;i--){
			if(verList[i]<=Revision)
				break;
		}
		eval(funcName + "="+funcName+"_Rev"+verList[i]+";");
	});
}