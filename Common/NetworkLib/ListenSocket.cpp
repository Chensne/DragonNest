
#include "StdAfx.h"
#include "ListenSocket.h"
#include "Log.h"
#include <iostream>

CListenSocket::CListenSocket()
: m_Socket( INVALID_SOCKET ), m_uiIPAddress( 0 ), m_unPort( 0 )
{
	memset( m_szIPAddress, 0, sizeof(m_szIPAddress) );
#if defined(_USE_ACCEPTEX)
#else
	m_nConnectionKey =0;
#endif //#if defined(_USE_ACCEPTEX)
}

CListenSocket::~CListenSocket()
{
	if( m_Socket != INVALID_SOCKET )
	{
		closesocket( m_Socket );
		m_Socket = INVALID_SOCKET;
	}
}

void CListenSocket::SetDefaultSocketOption( SOCKET s )
{
	DWORD dwMode = 1;
	ioctlsocket( s, FIONBIO, &dwMode );

	bool boNoDelay = true;
	setsockopt(s, IPPROTO_TCP, TCP_NODELAY, (const char FAR*)&boNoDelay, sizeof(boNoDelay));
	

	//Send만 소켓버퍼를 사용하지 않고 Recv는 소켓버퍼를 사용.
	INT32 iZero = 0;	
	setsockopt( s, SOL_SOCKET, SO_SNDBUF, reinterpret_cast<char*>(&iZero), sizeof(iZero) );
	
	int RecvBuf = INTERNALBUFFERLENMAX;
	setsockopt(s, SOL_SOCKET, SO_RCVBUF, (char*)&RecvBuf, sizeof(RecvBuf));

	LINGER lingerStruct;
	lingerStruct.l_onoff  = 0;
	lingerStruct.l_linger = 0;
	setsockopt( s, SOL_SOCKET, SO_LINGER, reinterpret_cast<char*>(&lingerStruct), sizeof(lingerStruct) );
}

#if defined(_USE_ACCEPTEX)
bool CListenSocket::bInitialize( const USHORT unPort )
#else
bool CListenSocket::bInitialize( const USHORT unPort, const int nKey )
#endif //#if defined(_USE_ACCEPTEX)
{
	m_Socket = WSASocket( AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED );
	if( m_Socket == INVALID_SOCKET )
	{
		g_Log.Log( LogType::_FILELOG, "CListenSocket::bInitialize() PORT=%d 소켓 생성 실패\r\n", unPort );
		return false;
	}

	INT32 iReUseAddr = 1;
	setsockopt( m_Socket, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<char*>(&iReUseAddr), sizeof(iReUseAddr) );

	m_unPort = unPort;
#if defined(_USE_ACCEPTEX)
#else
	m_nConnectionKey = nKey;
#endif //#if defined(_USE_ACCEPTEX)

	sockaddr_in serv_addr;
	int iLen = sizeof(serv_addr);
	memset( &serv_addr, 0, sizeof(serv_addr) );
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(unPort);
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);	// 내부, 외부 아이피로 고정하지않고 any로 수정
	
	//char szIPPort[MAX_PATH];
	//sprintf_s( szIPPort, "%s:%d", pszIPAddress, unPort );

	//if( WSAStringToAddressA( szIPPort, AF_INET, 0, reinterpret_cast<LPSOCKADDR>(&serv_addr), &iLen ) )
	//	return false;

	//strcpy_s( m_szIPAddress, IPLENMAX, pszIPAddress );
	//m_uiIPAddress	= serv_addr.sin_addr.S_un.S_addr;
	//m_unPort		= ntohs( serv_addr.sin_port );

	if(::bind( m_Socket, reinterpret_cast<SOCKADDR*>(&serv_addr), sizeof(serv_addr) ) )
	{
		g_Log.Log( LogType::_FILELOG, "CListenSocket::bInitialize() PORT=%d bind() 실패 Error=%d \r\n", unPort, WSAGetLastError() );
		return false;
	}

	if( listen( m_Socket, SOMAXCONN ) )
	{
		g_Log.Log( LogType::_FILELOG, "CListenSocket::bInitialize() PORT=%d listen() 실패 Error=%d \r\n", unPort, WSAGetLastError() );
		return false;
	}

	SetDefaultSocketOption( m_Socket );

	return true;
}
