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

// Ŭ���̾�Ʈ�� ���� ������ ȣ��ȴ�. 
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

// ���� ����
void
DnDummySession::OnClose()
{  
	if ( !m_pDummyClient )		return;
	Event evt;
	evt.nEventType = Event::DISCONNECT;
	m_bConnected = false;

	m_pDummyClient->AddEvent(evt);
}

// ����� ���� �̺�Ʈ
void DnDummySession::OnUserEvent( NxAsyncEvent* pEvent )
{ 

}

// ��Ŷ ���Ž�
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