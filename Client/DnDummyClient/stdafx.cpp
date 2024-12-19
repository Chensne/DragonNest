// stdafx.cpp : source file that includes just the standard includes
// DnDummyClient.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file


bool ToMultiString(IN std::wstring& wstr, OUT std::string& str )
{
	static char szBuff[8192] = "";
	ZeroMemory(szBuff, sizeof(char)*8192);
	WideCharToMultiByte( CP_ACP, 0, wstr.c_str(), -1, szBuff, 8192, NULL, NULL );
	str = szBuff;
	return true;

}

bool ToWideString(IN std::string& str, OUT std::wstring& wstr )
{
	static WCHAR wszBuff[8192] = L"";
	ZeroMemory(wszBuff, sizeof(WCHAR)*8192);
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, wszBuff, 8192 );
	wstr = wszBuff;
	return true;
}

bool ToMultiString(IN WCHAR* wstr, OUT std::string& str )
{
	return ToMultiString(std::wstring(wstr), str);
}

bool ToWideString(IN CHAR* str, OUT std::wstring& wstr )
{
	return ToWideString(std::string(str), wstr);

}

void ToLowerA(std::string& str)
{
	std::transform(str.begin(), str.end(), str.begin(), towlower); 
}

void ToLowerW(std::wstring& str)
{
	std::transform(str.begin(), str.end(), str.begin(), tolower); 
}

void ToUpperA(std::string& str)
{
	std::transform(str.begin(), str.end(), str.begin(), toupper); 
}
void ToUpperW(std::wstring& str)
{
	std::transform(str.begin(), str.end(), str.begin(), towupper); 
}


std::wstring FormatW( const WCHAR* fmt, ... )
{
	static WCHAR s_wszBuffer[8192] = L"";

	ZeroMemory(s_wszBuffer, sizeof(WCHAR) * 8192);
	va_list args;
	va_start( args, fmt );
	_vsnwprintf_s( s_wszBuffer, 8192-1, fmt, args );
	va_end( args );
	s_wszBuffer[8192-1] = 0;
	return std::wstring(s_wszBuffer);
}

std::string FormatA( const CHAR* fmt, ... )
{
	static CHAR s_szBuffer[8192] = "";

	ZeroMemory(s_szBuffer, sizeof(CHAR) * 8192);
	va_list args;
	va_start( args, fmt );
	_vsnprintf_s( s_szBuffer, 8192-1, fmt, args );
	va_end( args );
	s_szBuffer[8192-1] = 0;
	return std::string(s_szBuffer);
}


std::wstring GetTimeString(time_t time)
{
	struct tm* _tm = NULL;
	_tm = localtime(&(time));
	std::wstring strTime = FormatW( _T("%04d��%02d��%02d�� %02d��%02d��%02d��"),	1900+_tm->tm_year, _tm->tm_mon+1, _tm->tm_mday, _tm->tm_hour, _tm->tm_min, _tm->tm_sec);
	return strTime;

}


void RemoveStringA(std::string& str, std::string& del)
{
	while (true)
	{
		std::string::size_type npos = str.find(del);
		if ( npos < str.size() )
		{
			str.erase( npos, del.size());
		}
		else
			break;
	}
}

void TokenizeW( const std::wstring& str, std::vector<std::wstring>& tokens, const std::wstring& delimiters /* = L" "*/ )
{
	// �� ù ���ڰ� �������� ��� ����
	std::wstring::size_type lastPos = str.find_first_not_of( delimiters, 0 );
	// �����ڰ� �ƴ� ù ���ڸ� ã�´�
	std::wstring::size_type pos = str.find_first_of( delimiters, lastPos );

	while( std::wstring::npos != pos || std::wstring::npos != lastPos )
	{
		// token�� ã������ vector�� �߰��Ѵ�
		tokens.push_back( str.substr( lastPos, pos - lastPos ) );
		// �����ڸ� �پ�Ѵ´�.  "not_of"�� �����϶�
		lastPos = str.find_first_not_of( delimiters, pos );
		// ���� �����ڰ� �ƴ� ���ڸ� ã�´�
		pos = str.find_first_of( delimiters, lastPos );
	}
}

#if defined(_FILELOG_PACKET) || defined(_FILELOG_SPOS)
bool g_bInvalidDisconnect = false;
void _WriteLog( int nLogType, const char *szStr, ... ) 
{
#ifdef _FILELOG_PACKET
	if( nLogType != 1 ) return;
#elif _FILELOG_SPOS
	if( nLogType != 0 ) return;
#endif
	_WriteLogA(szStr);

}

void _WriteLogA( const char *szStr, ... ) 
{
	va_list list;
	char szTemp[65535];

	va_start( list, szStr );
	vsprintf_s( szTemp, szStr, list );
	std::wstring wStr;
	ToWideString( szTemp, wStr );
	g_Log.Log( LogType::_FILELOG, const_cast<wchar_t*>(wStr.c_str()) );
	va_end( list );
}


void _WriteDisplayLog( int Logtype , const char *szStr , ... ) 
{
	va_list list;
	char szTemp[20000];
	char szString[10000];

	va_start( list, szStr );	

	switch( Logtype )
	{
	case 1:
		sprintf_s(szTemp,"[Info] ");
		break;
	case 2:
		sprintf_s(szTemp,"[Warning] ");
		break;
	case 3:
		sprintf_s(szTemp,"[Error] ");
		break;
	case 4:
		sprintf_s(szTemp,"[Debug] ");
		break;
	default:
		sprintf_s(szTemp,"[Undefine] ");
		break;
	}

	vsprintf_s( szString, szStr, list );
	strcat_s(szString,"\n");
	strcat_s(szTemp,szString);

	std::wstring wStr;
	ToWideString( szTemp, wStr );
	g_Log.Log( LogType::_FILELOG, const_cast<wchar_t*>(wStr.c_str()) );

	OutputDebug(szTemp);

	va_end( list );

}
#else
void _WriteLog( int nLogType, const char *szStr, ... ){}
void _WriteDisplayLog( const char *szStr, ... ){}
void _WriteLogA( const char *szStr, ... ) {}
#endif