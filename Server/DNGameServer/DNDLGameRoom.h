#pragma once

#include "DNGameRoom.h"

class CDNUserSession;
class CDNDLGameRoom : public CDNGameRoom
{
public:

	CDNDLGameRoom( CDNRUDPGameServer* pServer, unsigned int iRoomID, MAGAReqRoomID* pPacket );
	virtual ~CDNDLGameRoom();

protected:
	int m_iProtectionKey;
	TQUpdateDarkLairResult m_RankQueryData;
	bool m_bRecvRankQueryData;
#if defined(PRE_FIX_69108)
	bool m_bSendRankQueryData;
#endif
	TAUpdateDarkLairResult *m_pRankQueryResultInfo;

public:

	virtual bool bIsDLRoom(){ return true; }

	virtual void OnDie( DnActorHandle hActor, DnActorHandle hHitter );
	virtual void OnDBMessageProcess(int iMainCmd, int iSubCmd, char *pData, int iLen);

	void MakeRankQueryData();
	void UpdateResultRankQueryData();
	void UpdateResultRankMapIndex();
	void SetUpdateRankData( CDNUserSession *pSession );
	void RequestRankQueryData();
	bool IsRecvRankQueryData() { return m_bRecvRankQueryData; }
	TQUpdateDarkLairResult *GetRankQueryData() { return &m_RankQueryData; }

	void FlushRankQueryResultInfo() { SAFE_DELETE( m_pRankQueryResultInfo ); }
	TAUpdateDarkLairResult *GetRankQueryResultInfo() { return m_pRankQueryResultInfo; }

	virtual void OnDBRecvDarkLair(int nSubCmd, char *pData, int iLen);
	virtual bool bIsLevelRegulation() { return true; }
	virtual void OnDelPartyMember( UINT iDelMemberSessionID, char cKickKind );

#ifdef PRE_MOD_DARKLAIR_RECONNECT
	virtual void OnSuccessBreakInto( CDNUserSession* pGameSession );
#endif // PRE_MOD_DARKLAIR_RECONNECT
};