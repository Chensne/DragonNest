#include "stdafx.h"
#include "DnDummySession.h"
#include "DnDummyClient.h"


void 
DnDummySessionManager::_CreateSessionPool(DWORD nSessionCnt)
{
	m_SessionArray.resize(nSessionCnt);

	for( DWORD i = 0 ; i < nSessionCnt ; i++ )
	{
		DnDummySession* pSession = new DnDummySession(i, m_pServerApp);
		m_SessionArray[i] = pSession;
	}
}

DnDummySession::DnDummySession(DWORD nUID, NxServerApp* pServerApp)
:NxSession( nUID, pServerApp )
{
	m_pDummyClient = NULL;
	m_bConnected = false;
}

DnDummySession::~DnDummySession()
{

}

// 클라이언트가 접속 했을때 호출된다. 
void	
DnDummySession::OnAccept( NxAsyncEvent* pEvent )
{  
	

}


void
DnDummySession::OnConnect( NxAsyncEvent* pEvent )
{ 
	m_bConnected = true;
	if ( !m_pDummyClient )		return;

	Event evt;
	evt.nEventType = Event::CONNECT;
	m_pDummyClient->AddEvent(evt);

	

}

// 접속 끊김
void
DnDummySession::OnClose()
{  
	if ( !m_pDummyClient )		return;
	Event evt;
	evt.nEventType = Event::DISCONNECT;
	m_bConnected = false;

	m_pDummyClient->AddEvent(evt);
}

// 사용자 정의 이벤트
void DnDummySession::OnUserEvent( NxAsyncEvent* pEvent )
{ 

}

// 패킷 수신시
void	
DnDummySession::OnPacket( NxPacket& Packet )
{ 
	if ( !m_pDummyClient )		return;
	Event evt;
	evt.nEventType = Event::PACKET;
	/*
	evt.Buffer.resize( Packet.GetPacketSize() );
	char* p = Packet.GetPacketBuffer(); 
	for ( int i = 0 ; i < (int)Packet.GetPacketSize() ; i++)
	{
		evt.Buffer.at(i) = p[i];
	}
	*/

	ASSERT( Packet.GetPacketSize() < BUFFER_SIZE  );

	memcpy( &(evt.BufferArray[0]), Packet.GetPacketBuffer(), Packet.GetPacketSize() );
	evt.nBufferSize = Packet.GetPacketSize();
	m_pDummyClient->AddEvent(evt);

}