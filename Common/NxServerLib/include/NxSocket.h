#pragma once

#include "NxAsyncEvent.h"

class NxSocket
{
	
public:
	NxSocket();
	virtual ~NxSocket();

	bool	Create( int nSocketType = SOCK_STREAM, bool bOverlapped = false );
	void	Close();

	bool	Attach( SOCKET hSocket, sockaddr_in* pAddr );
	SOCKET	Accept( DWORD ms, sockaddr_in* pAddr );
	bool	Connect( const TCHAR* szIPAddress, unsigned short nPortNo );
	bool	Bind( const TCHAR* szIPAddress, unsigned short nPortNo );
	bool	Listen( int nBackLogCount = SOMAXCONN );

	int		Send( char* pBuffer, int nLength, NxAsyncEvent* pEvent );
	int		Recv( char* pBuffer, int nBufSize, NxAsyncEvent* pEvent );

	const TCHAR*	GetIPAddress()	const { return m_szIPAddress; }
	WORD	GetPort() { return ntohs(m_SockAddr.sin_port);	}
	HANDLE	GetNativeHandle();
	void	GetBufferSize( int* nSendBufSize, int* nRecvBufSize );

	void	SetReuseAddr();
	void	SetIoctlSocket();
	void	SetLinger( bool bFlag );
	void	SetKeepAlive( bool bFlag );
	void	SetBufferSize( int nSendBufSize, int nRecvBufSize );

private:

	TCHAR		m_szIPAddress[20];
	sockaddr_in	m_SockAddr;
	SOCKET		m_Socket;
};
