#include "stdafx.h"
#include "NxAcceptor.h"
#include "NxConnectionManager.h"



NxAcceptor::NxAcceptor(NxConnectionManager* pConnectionManager, const char* szThreadNick /* = "Acceptor" */ )
: NxThread(szThreadNick), m_bFlagKill( false ) , m_nPortNo(0), m_pConnectionManager(pConnectionManager)
{
}

NxAcceptor::~NxAcceptor()
{
}

bool
NxAcceptor::Open( const TCHAR* szIPAddress, u_short nPortNo )
{
	if( m_hThreadHandle != INVALID_HANDLE_VALUE )
		return false;

	if( m_ListenSocket.Create( true ) == false )
		return false;

	if( m_ListenSocket.Bind( szIPAddress, nPortNo ) == false )
		return false;

	if( m_ListenSocket.Listen() == false )
		return false;

	m_bFlagKill = false;
	m_nPortNo = nPortNo;

	Start();

	return true;
}

void
NxAcceptor::Close()
{
	m_ListenSocket.Close();

	m_bFlagKill = true;
	if( WaitForTerminate( 1000 ) == false )
		Terminate( 0 );

	m_hThreadHandle = INVALID_HANDLE_VALUE;
}

void 
NxAcceptor::Run()
{
	sockaddr_in	 Addr;
	SOCKET		 hAcceptedSocket = INVALID_SOCKET;

	while( m_bFlagKill == false )
	{
		hAcceptedSocket = m_ListenSocket.Accept( 500, &Addr );
		
		if( hAcceptedSocket == INVALID_SOCKET )
			continue;

		if ( m_pConnectionManager )
			m_pConnectionManager->ActiveConnection( m_nPortNo, hAcceptedSocket, Addr );
	}
}