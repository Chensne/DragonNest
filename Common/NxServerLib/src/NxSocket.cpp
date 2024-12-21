#include "stdafx.h"
#include "NxSocket.h"

NxSocket::NxSocket()
: m_Socket( INVALID_SOCKET )
{
}

NxSocket::~NxSocket()
{
	Close();
}

bool
NxSocket::Create( int nSocketType, bool bOverlapped )
{
	if( m_Socket != INVALID_SOCKET )
		return false;

	int nProtocol = 0;
	if ( nSocketType == SOCK_STREAM )
	{
        nProtocol = IPPROTO_TCP ;
	}
	else
	{
        nProtocol = IPPROTO_UDP ;
	}

	if( bOverlapped == true )
		m_Socket  = ::WSASocket( AF_INET, nSocketType , nProtocol, 0, 0, WSA_FLAG_OVERLAPPED );
	else
		m_Socket = ::socket( AF_INET, nSocketType, nProtocol );

	if( m_Socket == INVALID_SOCKET )
		return false;

	::memset( &m_SockAddr, 0, sizeof( m_SockAddr ) );

	return true;
}

void
NxSocket::Close()
{
	if( m_Socket == INVALID_SOCKET )
		return;

	::closesocket( m_Socket );
	m_Socket = INVALID_SOCKET;
}

bool
NxSocket::Attach( SOCKET hSocket, sockaddr_in* pAddr )
{
	m_Socket = hSocket;

	if( pAddr != 0 )
	{
		
		::_stprintf_s( m_szIPAddress, _T("%d.%d.%d.%d"),
			pAddr->sin_addr.S_un.S_un_b.s_b1,
			pAddr->sin_addr.S_un.S_un_b.s_b2,
			pAddr->sin_addr.S_un.S_un_b.s_b3,
			pAddr->sin_addr.S_un.S_un_b.s_b4 );
		::memcpy( &m_SockAddr, pAddr, sizeof( sockaddr_in ) );
	}

	bool reuse = true, keepAlive = true;
	::setsockopt( m_Socket, SOL_SOCKET, SO_REUSEADDR, ( char* )&reuse, sizeof( reuse ) );
	::setsockopt( m_Socket, SOL_SOCKET, SO_KEEPALIVE, ( char* )&keepAlive, sizeof( keepAlive ) );

	LINGER LingerStruct; 
	LingerStruct.l_onoff = 1; 
	LingerStruct.l_linger = 0; 
	::setsockopt( m_Socket, SOL_SOCKET, SO_LINGER, (char*)&LingerStruct, sizeof(LingerStruct) ); 
	::setsockopt( m_Socket, SOL_SOCKET, SO_LINGER, (char*)&LingerStruct, sizeof(LingerStruct) ); 


	return true;
}

SOCKET
NxSocket::Accept( DWORD ms, sockaddr_in* pAddr )
{
	struct timeval	Timeout;
	fd_set			fds;
	int				nLen = sizeof( sockaddr_in );

	FD_ZERO( &fds );
	FD_SET( m_Socket, &fds );

	Timeout.tv_sec  = ms / 1000;
	Timeout.tv_usec = ms % 1000;

	if( ::select( 0, &fds, 0, 0, &Timeout ) == SOCKET_ERROR )
		return INVALID_SOCKET;

	if( FD_ISSET( m_Socket, &fds ) )
	{
		SOCKET	hSocket;
		int		size = sizeof( m_SockAddr );

		hSocket = ::accept( m_Socket, ( struct sockaddr* )pAddr, &nLen );
		::_stprintf_s( m_szIPAddress, _T("%d.%d.%d.%d"),
			pAddr->sin_addr.S_un.S_un_b.s_b1,
			pAddr->sin_addr.S_un.S_un_b.s_b2,
			pAddr->sin_addr.S_un.S_un_b.s_b3,
			pAddr->sin_addr.S_un.S_un_b.s_b4 );

		return hSocket;
	}

	return INVALID_SOCKET;
}

bool 
NxSocket::Connect( const TCHAR* szIPAddress, unsigned short nPortNo )
{
	if( m_Socket == INVALID_SOCKET )
		return false;

	struct	timeval	t = { 1, 0 };
	fd_set	fds;


	FD_ZERO( &fds );
	FD_SET( m_Socket, &fds );

	::memset( &m_SockAddr, 0, sizeof( sockaddr_in ) );
	m_SockAddr.sin_family			= AF_INET;
#ifdef _UNICODE
	char szBuff[256]="";
	NxWideStringToMultiString(szBuff, szIPAddress );
	m_SockAddr.sin_addr.s_addr		= ::inet_addr( szBuff );
#else
	m_SockAddr.sin_addr.s_addr		= ::inet_addr( szIPAddress );
#endif 
	m_SockAddr.sin_port				= ::htons( nPortNo );

	
	if( ::connect( m_Socket, ( struct sockaddr* )&m_SockAddr, sizeof( m_SockAddr ) ) == SOCKET_ERROR )
		return false;


	::_stprintf_s( m_szIPAddress, _T("%d.%d.%d.%d"),
		m_SockAddr.sin_addr.S_un.S_un_b.s_b1,
		m_SockAddr.sin_addr.S_un.S_un_b.s_b2,
		m_SockAddr.sin_addr.S_un.S_un_b.s_b3,
		m_SockAddr.sin_addr.S_un.S_un_b.s_b4 );

	return true;
}

bool 
NxSocket::Bind( const TCHAR* szIPAddress, unsigned short nPortNo )
{
	int		nError;

	::memset( &m_SockAddr, 0, sizeof( m_SockAddr ) );
	m_SockAddr.sin_family			= AF_INET;
	m_SockAddr.sin_addr.s_addr		= INADDR_ANY;//::inet_addr(szIPAddress); 
	m_SockAddr.sin_port				= ::htons( nPortNo );

	nError = ::bind( m_Socket, ( const sockaddr* )&m_SockAddr, sizeof( m_SockAddr ) );
	
	::_stprintf_s( m_szIPAddress, _T("%d.%d.%d.%d"),
		m_SockAddr.sin_addr.S_un.S_un_b.s_b1,
		m_SockAddr.sin_addr.S_un.S_un_b.s_b2,
		m_SockAddr.sin_addr.S_un.S_un_b.s_b3,
		m_SockAddr.sin_addr.S_un.S_un_b.s_b4 );

	return ( nError != SOCKET_ERROR );
}

bool 
NxSocket::Listen( int nBackLogCount )
{
	return ( ::listen( m_Socket, nBackLogCount ) != SOCKET_ERROR );
}

int
NxSocket::Send( char* pBuffer, int nLength, NxAsyncEvent* pEvent )
{
	WSABUF	wsaBuf;
	int		ret;

	wsaBuf.buf	= pBuffer;
	wsaBuf.len	= nLength;

	ret				= ::WSASend( m_Socket, &wsaBuf, 1, &pEvent->nTransBytes, 0, ( LPOVERLAPPED )pEvent, 0 );
	pEvent->nError	= ::WSAGetLastError();

	if( ret == SOCKET_ERROR )
	{
		if( pEvent->nError != ERROR_IO_PENDING && pEvent->nError != ERROR_SUCCESS )
			return 0;
	}

	return pEvent->nTransBytes;
}

int 
NxSocket::Recv( char* pBuffer, int nBufSize, NxAsyncEvent* pEvent )
{
	WSABUF	wsaBuf;
	DWORD	bFlag;
	int		ret;

	bFlag		= 0;
	wsaBuf.buf	= pBuffer;
	wsaBuf.len	= nBufSize;

	ret = ::WSARecv( m_Socket, &wsaBuf, 1, &pEvent->nTransBytes, &bFlag, ( LPOVERLAPPED )pEvent, 0 );
	pEvent->nError	= ::WSAGetLastError();
	if( ret == SOCKET_ERROR )
	{
		if( pEvent->nError != ERROR_IO_PENDING && pEvent->nError != ERROR_SUCCESS )
			return 0;
	}

	return pEvent->nTransBytes;
}

HANDLE 
NxSocket::GetNativeHandle()
{
	return ( HANDLE )m_Socket;
}

void 
NxSocket::GetBufferSize( int* nSendBufSize, int* nRecvBufSize )
{
	int		len = sizeof( int );

	::getsockopt( m_Socket, SOL_SOCKET, SO_SNDBUF, ( char* )nSendBufSize, &len );
	::getsockopt( m_Socket, SOL_SOCKET, SO_RCVBUF, ( char* )nRecvBufSize, &len );
}

void
NxSocket::SetReuseAddr()
{
	BOOL	reuse = TRUE;

	::setsockopt( m_Socket, SOL_SOCKET, SO_REUSEADDR, ( const char* )&reuse, sizeof( reuse ) );
}

void 
NxSocket::SetIoctlSocket()
{
	unsigned long	argp = 1;

	::ioctlsocket( m_Socket, FIONBIO, &argp );
}

void
NxSocket::SetLinger( bool bFlag )
{
	LINGER	opt =
	{
		bFlag, 0
	};

	::setsockopt( m_Socket, SOL_SOCKET, SO_LINGER, ( char* )&opt, sizeof( opt ) );
}

void 
NxSocket::SetKeepAlive( bool bFlag )
{
	int		argp = bFlag;

	::setsockopt( m_Socket, SOL_SOCKET, SO_KEEPALIVE, ( char* )&argp, sizeof( argp ) );
}

void 
NxSocket::SetBufferSize( int nSendBufSize, int nRecvBufSize )
{
	::setsockopt( m_Socket, SOL_SOCKET, SO_SNDBUF, ( char* )&nSendBufSize, sizeof( int ) );
	::setsockopt( m_Socket, SOL_SOCKET, SO_RCVBUF, ( char* )&nRecvBufSize, sizeof( int ) );
}