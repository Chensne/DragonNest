#pragma once

#include "NxPacket.h"
#include "DNSecure.h"
#include "RUDPCommonDef.h"


class DnDummyClient;
class DnDummyClientTCPReceiver
{
public:
	DnDummyClientTCPReceiver() { m_pDummyClient =NULL; } 
	virtual ~DnDummyClientTCPReceiver(){}

	void	SetDummyClient(DnDummyClient* p) { m_pDummyClient = p; }

	void OnPacket(NxPacket& Packet);
	void _OnPacket(int iMainCmd, int iSubCmd, char *pData, int nSize);
	
public:
	// main command
	void OnTRecvActorMsg(int iMainCmd, int iSubCmd, char *pData, int nSize);
	void OnTRecvActorBundleMessage(int iMainCmd, int iSubCmd, char *pData, int nSize);
	void OnTRecvLoginMsg(int iMainCmd, int iSubCmd, char *pData, int nSize);
	void OnTRecvCharMsg(int iMainCmd, int iSubCmd, char *pData, int nSize);
	void OnTRecvPartyMsg(int iMainCmd, int iSubCmd, char *pData, int nSize);
	void OnTRecvSystemMsg(int iMainCmd, int iSubCmd, char *pData, int nSize);
	void OnTRecvRoomMsg(int iMainCmd, int iSubCmd, char *pData, int nSize);
	void OnTRecvItemMsg(int iMainCmd, int iSubCmd, char *pData, int nSize);
	
	
	// sub command
	void OnRecvSystemVillageInfo( char *pData );
	void OnRecvSystemConnectVillage( char *pData );
	void OnRecvSystemGameInfo( char *pData );
	void OnRecvSystemConnectedResult( char *pData );
	void OnRecvSystemReconnectReq();
	void OnRecvSystemGameTcpConnectReq();
	void OnRecvSystemReconnectLogin( char* pData);

	void OnTRecvLoginCheckLogin( char *pData );
	void OnTRecvLoginServerList( char *pData );
	void OnTRecvLoginChannelList( char *pData );
	void OnTRecvLoginCharList( char *pData );
	void OnTRecvLoginCharCreate( char *pData );
	void OnTRecvLoginCharDelete( char *pData );
	void OnTRecvLoginEnterVillage( char *pData );//@@ Áö¿ìÀÚ

	void OnTRecvVillageActorMsg( char *pData);
	void OnTRecvVillageMapInfo( char *pData);
	void OnTRecvVillageEntered( char *pData);
	void OnTRecvVillageUserEntered( char *pData);

	void OnTRecvPartyList( char *pData);
	void OnTRecvPartyListInfo( char *pData );
	void OnTRecvPartyListInfoError( char *pData );

	void OnTRecvPartyCreateParty( char *pData );
	void OnTRecvPartyJoinParty( char *pData );
	void OnTRecvPartyRefresh( char *pData );

	void OnTRecvPartyRefreshGateInfo( char *pData ); 
	void OnTRecvPartyReadyGate( char *pData ); 
	void OnTRecvPartyStageStart( char *pData ); 
	void OnTRecvPartyStageCancel( char *pData ); 
	void OnRecvPartyEquipData(char * pData );

	void OnURecvPartySyncWait( char *pPacket );
	void OnURecvPartySyncStart( char *pPacket ); 
	void OnURecvGameActorMsg( char *pPacket ); 

	void OnRecvRoomSyncWait( char *pPacket ); 
	void OnRecvRoomSyncStart( char *pPacket ); 

	void OnRecvInvenList( char *pPacket );

	
	
private:
	DnDummyClient*	m_pDummyClient;

	int m_nSelectedServerIdx;
	SCInvenList m_InvenList;
};

class DnDummyClientUDPReceiver
{
public:
	DnDummyClientUDPReceiver() { m_pDummyClient =NULL; } 
	virtual ~DnDummyClientUDPReceiver(){}

	void SetDummyClient(DnDummyClient* p) { m_pDummyClient = p; }
	void OnPacket(int header, int sub_header, char * data, int size, _ADDR * addr);

public:
	void OnURecvPartySyncWait( char *pPacket );
	void OnURecvPartySyncStart( char *pPacket ); 
	void OnURecvGameActorMsg( char *pPacket ); 
	
private:
	DnDummyClient*	m_pDummyClient;

};