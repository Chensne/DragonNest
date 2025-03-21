#include "Stdafx.h"
#include "StrToActozStr.h"
#include <strsafe.h>

string StrChangeStr2(string sSourceStr, string sTargetStr, string sSubStr);

int StrToActozStr2(const char *_szInbuf, char *_szOutbuf) // 문자열을 Actoz 문자열로 변환.
{
	string strResult;

	strResult = _szInbuf;

	strResult = StrChangeStr2(strResult, "\\_"	, "\\_0"); //"\\_"문자를 "\\_0"문자로 변환
	strResult = StrChangeStr2(strResult, "\n"	, "\\_1"); //"\n" 문자를 "\\_1"문자로 변환
	strResult = StrChangeStr2(strResult, "\r"	, "\\_2"); //"\r" 문자를 "\\_2"문자로 변환
	strResult = StrChangeStr2(strResult, " "		, "\\_3"); //" "  문자를 "\\_3"문자로 변환
	strResult = StrChangeStr2(strResult, "\""	, "\\_4"); //"\"" 문자를 "\\_4"문자로 변환
	strResult = StrChangeStr2(strResult, "'"		, "\\_5"); //"'"  문자를 "\\_5"문자로 변환
	strResult = StrChangeStr2(strResult, "`"		, "\\_6"); //"`"  문자를 "\\_6"문자로 변환
	strResult = StrChangeStr2(strResult, ":"		, "\\_7"); //":"  문자를 "\\_7"문자로 변환
	strResult = StrChangeStr2(strResult, ";"		, "\\_8"); //";"  문자를 "\\_8"문자로 변환
	
	int iOutBufSize = (int)strlen((char *)strResult.c_str());
	memset(_szOutbuf, 0x00, iOutBufSize+1); 
	StringCchCopyA(_szOutbuf, iOutBufSize+1, (char *)strResult.c_str()); // 변환된 문자열을 반환.
	
	return iOutBufSize;
}

string StrChangeStr2(string _strSource, string _strTarget, string _strSub)//문자열에서 _strTarget를 찾아서 _strSub로 변환 후 String 으로 넘김..
{
	int i = 0;
	string strResult ="";
	string strTmpStrFirst ="";
	
	while((int)_strSource.find(_strTarget) != -1){ //문자열에서 특정문자(_strTarget)를 찾음.
		i = (int)_strSource.find(_strTarget); //특정문자(_strTarget)를 찾았다면 위치를 저장.
		strTmpStrFirst = _strSource.substr(0, i); //문자열의 처음부터 저장된 위치까지 strTmpStrFirst 에 저장.
		_strSource.erase(0,i+_strTarget.length());//문자열의 처음부터 저장된 위치+특정문자(_strTarget)열의 길이까지 지움.
	
		strResult = strResult+strTmpStrFirst+_strSub;//strResult 에 strTmpStrFirst(문자열의 처음부터 저장된 위치까지 저장된 문자열)+_strSub(변환값)를 더한다.
	}

	strResult = strResult + _strSource;//변환된 값에 변환후 남은 문자열을 붙임.

	return strResult; //변환된 문자열 값을 넘김.
}
