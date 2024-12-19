#pragma once

class CListenSocket
{
public:

	CListenSocket();
	~CListenSocket();

#if defined(_USE_ACCEPTEX)
	bool		bInitialize( const USHORT unPort );
#else
	bool		bInitialize( const USHORT unPort, const int nKey );	
#endif
	
	// Interface
	SOCKET		GetSocket()			const { return m_Socket; }
	const char*	pszGetIPAddress()	const { return m_szIPAddress; }
	UINT		uiGetIPAddress()	const { return m_uiIPAddress; }
	USHORT		unGetPort()			const { return m_unPort; }
#if defined(_USE_ACCEPTEX)
#else
	int			GetConnectionKey()	const { return m_nConnectionKey;}
#endif //#if defined(_USE_ACCEPTEX)

	static void	SetDefaultSocketOption( SOCKET s );

private:

	SOCKET	m_Socket;
	char	m_szIPAddress[IPLENMAX];
	UINT	m_uiIPAddress;
	USHORT	m_unPort;
#if defined(_USE_ACCEPTEX)
#else
	int		m_nConnectionKey;
#endif //#if defined(_USE_ACCEPTEX)
};

