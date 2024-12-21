#include "stdafx.h"
#include "NxSession.h"
#include "NxServerApp.h"

NxSession::NxSession(DWORD nUID, NxServerApp* pServerApp)
{
	m_nUID = nUID;
	m_pServerApp = pServerApp;

}

NxSession::~NxSession()
{

}

// 패킷 송신시
void
NxSession::SendPacket( NxPacket& Packet )
{
	m_pServerApp->SendPacket(m_nUID, Packet);

}
