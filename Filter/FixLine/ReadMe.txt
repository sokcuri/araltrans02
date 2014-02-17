FixLine.dat 파일 규격

_Format = V2
//버전 정보
//주요 포맷이 바뀔때 올라간다.

strRetChar = string
//개행문자
//기본값은 \n

MaxChar = int
//한줄의 최대 '바이트'수

MaxLine = int
//전체 문장의 최대 '줄' 수

IgnoreLine = int
//무시 옵션
//0초과 : 무시 줄수
//0 : 사용안함
//-1 : 첫줄 제한

UseLimitLine = bool
//원문 줄 수 제한

IgnoreNonTrans = bool
//무시된 줄 번역 안함

UseRetChar = bool
//개행 문자 뗌
//여기는 반대로 true가 '개행 문자를 없앤다'

UseRet = bool
//개행 처리 사용
//위의 옵션의 사용 여부 결정

MaxLen = int
//'총' 바이트 수 제한 설정
//0이상 : 지정된 길이로 맞춤
//-1 : 원문 길이와 동일하게
//-2 : 사용 안함

ForceLen = bool
//길이X인것도 MaxLen에 적용할 것인가 여부

ElemType_i = int
//함수 옵션
//이 값이 있으면 ElemSub를 읽는다.
// 0 : 일반
// 1 : 문두
// 2 : 문미

ElemSubSize_i_j = int
//길이 적용 옵션
// 0이상 : 길이 설정
// -1 : 글 길이대로
// -2 : 사용안함

ElemSubTrans_i_j = bool
//번역 여부

ElemSubDel_i_j = bool
//삭제 여부

ElemSubPass_i_j = bool
//통과 여부

ElemSubString_i_j = string
//문자열

ElemSubMin_i_j = int
//최소 설정
//-1인 경우 '사용안함'

ElemSubMax_i_j = int
//최대 설정
//-1인 경우 '사용안함'

ElemSubFunc_i_j = bool
//함수 여부


Name_i = string
//첫줄 제한