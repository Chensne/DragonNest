#pragma once
#include "clientsession.h"

class CVillageClient : public CClientSession
{
public:
	CVillageClient(void);
	~CVillageClient(void);

protected:
	char m_szRemoteAddress[ 32 ];
	USHORT m_nRemotePort;
	TCHAR m_szID[ 32 ];
	TCHAR m_szPassword[ 32 ];

public:
	void OnIoConnected(void);
	void OnIoDisconnected(void);
	void OnIoRead();

	void OnVillageUserEntered( BYTE *pPacket );
	void OnVillageCreatedParty( BYTE *pPacket );
	void OnVillageJoinPartySucc( BYTE *pPacket );
	void OnVillageJoinPartyFail( BYTE *pPacket );
	void OnVillageStartStageSucc( BYTE *pPacket );
	void OnVillageStartStageFail( BYTE *pPacket );

	BOOL Begin( char *pRemoteAddress, USHORT nRemotePort, LPTSTR pID, LPTSTR pPassword );
	void JoinParty( LPTSTR pLeaderName );
	void ReqStartStage();
};
