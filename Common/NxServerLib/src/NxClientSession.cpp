#include "stdafx.h"
#include "NxClientSession.h"

NxClientSession::_ClientSessionList
NxClientSession::ms_SessionList;

NxClientSession::NxClientSession()
: m_RecvPacketSize( 0 ), m_Socket( INVALID_SOCKET ), m_SessionEvent( INVALID_HANDLE_VALUE )
{
	

}

NxClientSession::~NxClientSession()
{
	_RemoveClientSession((NxClientSession*)this);
	Close();
}


void 
NxClientSession::_AddClientSession(NxClientSession* pClientSession)
{
	for ( size_t i = 0 ; i < ms_SessionList.size() ; i++ )
	{
		NxClientSession* pSession = ms_SessionList.at(i);
		if ( pSession && pSession == pClientSession )
			return;
	}
	
	ms_SessionList.push_back(pClientSession);

}

bool
NxClientSession::_RemoveClientSession(NxClientSession* pClientSession)
{

	for ( size_t i = 0 ; i < ms_SessionList.size() ; i++ )
	{
		NxClientSession* pSocket = ms_SessionList.at(i);
		if ( pSocket && pSocket == pClientSession )
		{
			ms_SessionList.erase(ms_SessionList.begin()+i);
			return true;
		}
	}
	return false;
}

void
NxClientSession::UpdateClientSession()
{
	for ( int i = 0 ; i < (int)ms_SessionList.size() ; i++ )
	{
		NxClientSession* pClientSession = ms_SessionList.at(i);
		if ( pClientSession )
			pClientSession->_SelectEvent();

	}
}


void
NxClientSession::TraceGetLastError()
{
	int nResult = WSAGetLastError();
	TCHAR Buf[512] = {NULL};
	::FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, nResult, LANG_SYSTEM_DEFAULT, Buf, 512, NULL );
	OutputDebugString(Buf);
	
}

//--------------------------------------------------------------------

bool
NxClientSession::Create(NxClientSessionType nSocketType)
{
	int nRet = 0;
	m_SocketType = nSocketType;

	//  Create socket
	if ( m_SocketType == NxClientSession::NX_TCP )
	{
		m_Socket = ::socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
	}

	else if ( m_SocketType == NxClientSession::NX_UDP )
	{
		m_Socket = ::socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
	}

	if( m_Socket == INVALID_SOCKET || nRet == SOCKET_ERROR ) 
		return false;

	_AddClientSession((NxClientSession*)this);
	m_SessionEvent	 = ::WSACreateEvent();

	return true;
}

bool		
NxClientSession::Bind(const TCHAR* szIPAddress, unsigned int nPortNo )
{
	int		nError;

	::memset( &m_SockAddr, 0, sizeof( m_SockAddr ) );
	m_SockAddr.sin_family			= AF_INET;
	m_SockAddr.sin_addr.s_addr		= INADDR_ANY;//::inet_addr( INADDR_ANY );
	m_SockAddr.sin_port				= ::htons( nPortNo );

	nError = ::bind( m_Socket, ( const sockaddr* )&m_SockAddr, sizeof( m_SockAddr ) );
	::_stprintf_s( m_szIPAddress, _T("%d.%d.%d.%d"),
		m_SockAddr.sin_addr.S_un.S_un_b.s_b1,
		m_SockAddr.sin_addr.S_un.S_un_b.s_b2,
		m_SockAddr.sin_addr.S_un.S_un_b.s_b3,
		m_SockAddr.sin_addr.S_un.S_un_b.s_b4 );

	return ( nError != SOCKET_ERROR );

}
//--------------------------------------------------------------------

void
NxClientSession::ConnectTo( const TCHAR* szIPAddress, unsigned int nPort )
{
	if ( m_SocketType == NX_UDP )
		return;

	if ( m_Socket == 0 )
	{
		return;
	}

	//  Initialize sockaddr_in structure
	struct sockaddr_in serv_addr;

	::ZeroMemory( &serv_addr, sizeof( serv_addr ) );
	serv_addr.sin_family      = AF_INET;

#ifdef _UNICODE
	char szBuff[256]="";
	NxWideStringToMultiString(szBuff, szIPAddress );
	serv_addr.sin_addr.s_addr		= ::inet_addr( szBuff );
#else
	serv_addr.sin_addr.s_addr		= ::inet_addr( szIPAddress );
#endif 

	serv_addr.sin_port        = ::htons( nPort );


	//  Try to connect server
	if( ::connect( m_Socket, ( struct sockaddr* )&serv_addr, sizeof( serv_addr ) ) != 0 )
		OnConnect( ::WSAGetLastError() );
	else
		OnConnect( 0 );
}


void 
NxClientSession::Close()
{
	if( m_SessionEvent != INVALID_HANDLE_VALUE )
	{
		::WSACloseEvent( m_SessionEvent );
		m_SessionEvent = INVALID_HANDLE_VALUE;
	}

	::shutdown( m_Socket, SD_BOTH );
	::closesocket( m_Socket );
}

//--------------------------------------------------------------------


int	
NxClientSession::Send( NxPacket& packet )
{
	DWORD writtenBytes;
	DWORD dwFlags = 0;
	
	WSABUF wsaBuf;
	wsaBuf.buf = packet.GetPacketBuffer();
	wsaBuf.len = packet.GetPacketSize();

	int nResult = ::WSASend(m_Socket, &wsaBuf, 1, &writtenBytes, 0, NULL, NULL);

	if ( nResult < 0 )
	{
		NxClientSession::TraceGetLastError();
	}


	return nResult;

}

int		
NxClientSession::SendTo(NxPacket& packet, sockaddr_in* ToAddr)
{
	DWORD writtenBytes =0;
	DWORD dwFlags = 0;

	WSABUF wsaBuf;
	wsaBuf.buf = packet.GetPacketBuffer();
	wsaBuf.len = packet.GetPacketSize();

	int nResult = 0;
	nResult = ::WSASendTo( m_Socket, &wsaBuf, 1, &writtenBytes, 0, (sockaddr*)ToAddr, sizeof(sockaddr), NULL, NULL);
	
	if ( nResult < 0 )
	{
		NxClientSession::TraceGetLastError();
	}

	return nResult;
}



void
NxClientSession::OnReceive() 
{
	if ( m_SocketType == NX_TCP )
	{
		_RecvPacket_TCP();
	}

	else if ( m_SocketType == NX_UDP )
	{
		_RecvPacket_UDP();
	}

}

void	
NxClientSession::_RecvPacket_TCP()
{
	// 패킷 풀에서 할당 받는다.
	NxPacket ReceivedPacket;

	DWORD bytesTransferred;
	DWORD bufSize = PACKETBUFFERSIZE - m_RecvPacketSize;
	DWORD dwFlags = 0;

	WSABUF wsaBuf;
	wsaBuf.buf = &(m_RecvBuffer[m_RecvPacketSize]);
	wsaBuf.len = bufSize;

	//  Receiving packet
	::WSARecv(m_Socket, &wsaBuf, 1, &bytesTransferred, &dwFlags, NULL, NULL);

	if( bytesTransferred > 0 )
		m_RecvPacketSize += bytesTransferred;

	while( m_RecvPacketSize > 0 )
	{
		ReceivedPacket.CopyToBuffer( m_RecvBuffer, m_RecvPacketSize );

		if( ReceivedPacket.IsValidPacket() == true && m_RecvPacketSize >= ( int )ReceivedPacket.GetPacketSize() )
		{
			//  Packet parsing
			OnPacket(ReceivedPacket);

			char buffer[PACKETBUFFERSIZE] = {0,};

			m_RecvPacketSize -= ReceivedPacket.GetPacketSize();
			::CopyMemory( buffer, ( m_RecvBuffer + ReceivedPacket.GetPacketSize() ), m_RecvPacketSize );
			::CopyMemory( m_RecvBuffer, buffer, m_RecvPacketSize );
			
		}
		else
			break;
	}
}

void		
NxClientSession::_RecvPacket_UDP()
{
	NxPacket ReceivedPacket;
	DWORD bytesTransferred;
	DWORD dwFlags = 0;
	WSABUF wsaBuf;

	wsaBuf.buf = ReceivedPacket.GetPacketBuffer();
	wsaBuf.len = PACKETBUFFERSIZE;

	sockaddr_in FromAddr;
	::ZeroMemory(&FromAddr, sizeof(sockaddr_in));
	int FromSize = sizeof(sockaddr_in);

	//  Receiving packet
	int nResult = ::WSARecvFrom( m_Socket, &wsaBuf, 1, &bytesTransferred, &dwFlags,
								 (sockaddr*)&FromAddr, &FromSize, NULL, NULL);


	OnPacket(ReceivedPacket, (sockaddr_in*)&FromAddr);

	
}


//--------------------------------------------------------------------

void 
NxClientSession::_SelectEvent()
{
	WSANETWORKEVENTS NetEvent;

	//  Event select
	::ZeroMemory( &NetEvent, sizeof( NetEvent ) );
	::WSAEventSelect( m_Socket, m_SessionEvent, FD_READ | FD_CLOSE | FD_CONNECT );
	::WSAEnumNetworkEvents( m_Socket, m_SessionEvent, &NetEvent );
	

	if( ( NetEvent.lNetworkEvents & FD_READ ) == FD_READ )
		OnReceive();
	if( ( NetEvent.lNetworkEvents & FD_CLOSE ) == FD_CLOSE )
		OnClose();
	if( ( NetEvent.lNetworkEvents & FD_CONNECT ) == FD_CONNECT )
		OnConnect( NetEvent.iErrorCode[FD_CONNECT_BIT] );
}
