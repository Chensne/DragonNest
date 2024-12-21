#include "StdAfx.h"
#include "VillageClient.h"

#include "VillageProtocol.h"
#include "Village_Structure.h"
#include "Village_ReadPacket.h"
#include "Village_WritePacket.h"

CVillageClient::CVillageClient(void)
{
}

CVillageClient::~CVillageClient(void)
{
}

BOOL CVillageClient::Begin( char *pRemoteAddress, USHORT nRemotePort, LPTSTR pID, LPTSTR pPassword )
{
	strcpy( m_szRemoteAddress, pRemoteAddress );
	m_nRemotePort = nRemotePort;
	_tcscpy( m_szID, pID );
	_tcscpy( m_szPassword, pPassword );
	return BeginTcp( m_szRemoteAddress, m_nRemotePort );
}

void CVillageClient::OnIoConnected(void)
{
	BYTE WriteBuffer[MAX_BUFFER_LENGTH]	= {0,};
	WCHAR szLocalIP[ 32 ];
	GetLocalIP( szLocalIP );
	WritePacket( PT_VILLAGE_ENTER, WriteBuffer, WriteVillageEnter( WriteBuffer, m_szID, m_szPassword, szLocalIP, GetLocalPort() ) );
}

void CVillageClient::OnIoDisconnected(void)
{
}

void CVillageClient::OnIoRead()
{
	CClientSession::OnIoRead();

	DWORD dwProtocol, dwPacketLength;
	BYTE Packet[ MAX_BUFFER_LENGTH ];
	if( !ReadPacket( dwProtocol, Packet, dwPacketLength ) )
	{
		return;
	}
	switch( dwProtocol )
	{
	case PT_VILLAGE_USER_ENTERED:
		OnVillageUserEntered( Packet );
		break;
	case PT_VILLAGE_CREATED_PARTY:
		OnVillageCreatedParty( Packet );
		break;
	case PT_VILLAGE_JOIN_PARTY_SUCC:
		OnVillageJoinPartySucc( Packet );
		break;
	case PT_VILLAGE_JOIN_PARTY_FAIL:
		OnVillageJoinPartyFail( Packet );
		break;
	case PT_VILLAGE_START_STAGE_SUCC:
		OnVillageStartStageSucc( Packet );
		break;
	case PT_VILLAGE_START_STAGE_FAIL:
		OnVillageStartStageFail( Packet );
		break;
	}
}

void CVillageClient::OnVillageUserEntered( BYTE *pPacket )
{
	SVillageUserEntered Data;
	ReadVillageUserEntered( pPacket, Data );
}

void CVillageClient::OnVillageCreatedParty( BYTE *pPacket )
{
	SVillageCreatedParty Data;
	ReadVillageCreatedParty( pPacket, Data );
}

void CVillageClient::OnVillageJoinPartySucc( BYTE *pPacket )
{
	SVillageJoinPartySucc Data;
	ReadVillageJoinPartySucc( pPacket, Data );
}

void CVillageClient::OnVillageJoinPartyFail( BYTE *pPacket )
{
}

void CVillageClient::OnVillageStartStageSucc( BYTE *pPacket )
{
	// Start Game...
}

void CVillageClient::OnVillageStartStageFail( BYTE *pPacket )
{
}

void CVillageClient::JoinParty( LPTSTR pLeaderName )
{
	BYTE WriteBuffer[MAX_BUFFER_LENGTH]	= {0,};
	WritePacket( PT_VILLAGE_JOIN_PARTY, WriteBuffer, WriteVillageJoinParty( WriteBuffer, pLeaderName ) );
}

void CVillageClient::ReqStartStage()
{
	WritePacket( PT_VILLAGE_JOIN_PARTY );
}
