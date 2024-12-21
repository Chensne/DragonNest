// stdafx.cpp : ǥ�� ���� ������ �����ϴ� �ҽ� �����Դϴ�.
// NxFileSystemTool.pch�� �̸� �����ϵ� ����� �˴ϴ�.
// stdafx.obj�� �̸� �����ϵ� ���� ������ �����մϴ�.

#include "stdafx.h"




std::string GetTimeString(time_t time)
{
	struct tm* _tm = NULL;
	_tm = localtime(&(time));
	std::string strTime = FormatA( _T("%04d��%02d��%02d�� %02d��%02d��%02d��"),	1900+_tm->tm_year, _tm->tm_mon+1, _tm->tm_mday, _tm->tm_hour, _tm->tm_min, _tm->tm_sec);
	return strTime;

}