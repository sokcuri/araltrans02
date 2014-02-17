/*
** JSCustom 고수준 보조 함수 Library
** Revision : 0
*/

function ATInclude_Rev0(FilePath){
	var FileData = ATGetFile_Rev0(FilePath);
	FileData = ATArrayToString_Rev0(FileData,-1,'UTF-8');
	return eval(FileData);
}

function ATAlert_Rev0(Content,Title="메시지",Type=0){
	return ATCallWinAPI_Rev0('User32.dll','MessageBoxW',0,Content,Title,Type);
}

function ATConvertLocale_Rev0(Data,LocaleIn,LocaleOut){
	var tmp = ATArrayToString_Rev0(Data,-1,LocaleIn);
	if(tmp!=null){
		return ATStringToArray_Rev0(tmp,LocaleOut);
	}
	return null;
}