// stdafx.cpp : source file that includes just the standard includes
// DragonNest.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file


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
