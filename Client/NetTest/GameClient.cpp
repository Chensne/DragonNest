#include "StdAfx.h"
#include "GameClient.h"
#include "GameProtocol.h"
#include "Game_Structure.h"
#include "Game_ReadPacket.h"
#include "Game_WritePacket.h"

CGameClient::CGameClient(void)
{
}

CGameClient::~CGameClient(void)
{
}

void CGameClient::OnIoRead(void)
{
	CClientSession::OnIoRead();

/*	DWORD dwProtocol, dwPacketLength;
	BYTE Packet[ MAX_BUFFER_LENGTH ];
	char szIP[ 1024 ];
	USHORT nPort;
	if( !ReadFromPacket( dwProtocol, szIP, nPort, Packet, dwPacketLength ) )
	{
		return;
	}*/
}

void CGameClient::OnIoConnected(void)
{
}

void CGameClient::OnIoDisconnected(void)
{
}

BOOL CGameClient::Begin( USHORT nRemotePort )
{
	return BeginUdp( nRemotePort );
}
