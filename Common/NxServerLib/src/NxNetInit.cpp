#include "stdafx.h"
#include "NxNetInit.h"


NxNetInit::NxNetInit()
{
	
}

NxNetInit::~NxNetInit()
{
	
}

bool
NxNetInit::StartupWinSock()
{
	WSADATA wsaData;

	//  ws2_32.dll을 사용하기 위해 초기화한다.
	if( ::WSAStartup( 0x202, &wsaData ) != 0 )
		return false;

	if( LOBYTE( wsaData.wVersion ) != 2 || HIBYTE( wsaData.wVersion ) != 2 )
	{
		::WSACleanup();

		return false;
	}
	return true;
}


void
NxNetInit::CleanupWinSock()
{
	::WSACleanup();
}

//--------------------------------------------------------------------------------------

int		
NxGetLastError::GetCode()
{
	return WSAGetLastError();
}

tstring
NxGetLastError::GetString()
{
	int nResult = WSAGetLastError();
	TCHAR Buf[512] = {NULL};
	::FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, nResult, LANG_SYSTEM_DEFAULT, Buf, 512, NULL );
	return tstring(Buf);

}

void	
NxGetLastError::Trace()
{
	int nResult = WSAGetLastError();
	TCHAR Buf[512] = {NULL};
	::FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, nResult, LANG_SYSTEM_DEFAULT, Buf, 512, NULL );
	OutputDebugString(Buf);

}
