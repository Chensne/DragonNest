// stdafx.cpp : 표준 포함 파일을 포함하는 소스 파일입니다.
// NxFileSystemTool.pch는 미리 컴파일된 헤더가 됩니다.
// stdafx.obj는 미리 컴파일된 형식 정보를 포함합니다.

#include "stdafx.h"




std::string GetTimeString(time_t time)
{
	struct tm* _tm = NULL;
	_tm = localtime(&(time));
	std::string strTime = FormatA( _T("%04d년%02d월%02d일 %02d시%02d분%02d초"),	1900+_tm->tm_year, _tm->tm_mon+1, _tm->tm_mday, _tm->tm_hour, _tm->tm_min, _tm->tm_sec);
	return strTime;

}