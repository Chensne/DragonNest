#pragma once

#include "GameListener.h"
#include "DNPacket.h"

class CDNUserSession;
class CGameServerSession : public CGameListener {
public:
	CGameServerSession();
	virtual ~CGameServerSession();

protected:
	int m_nStageRandomSeed;
	int m_nGateIndex;
	bool m_bSyncComplete;

protected:
	int OnRecvCharMessage( CDNUserSession * pSession, int nSubCmd, char *pData, int nLen );
	int OnRecvTradeMessage( CDNUserSession * pSession, int nSubCmd, char *pData, int nLen );
	int OnRecvActorMessage( CDNUserSession * pSession, int nSubCmd, char *pData, int nLen );
	int OnRecvActorBundleMessage( CDNUserSession * pSession, int nSubCmd, char *pData, int nLen );
	int OnRecvNpcMessage( CDNUserSession * pSession, int nSubCmd, char *pData, int nLen );
	int OnRecvQuestMessage( CDNUserSession * pSession, int nSubCmd, char *pData, int nLen );
	int OnRecvPropMessage( CDNUserSession * pSession, int nSubCmd, char *pData, int nLen );
	int OnRecvRoomMessage( CDNUserSession * pSession, int nSubCmd, char * pData, int nLen );
	int OnRecvAppellationMessage( CDNUserSession * pSession, int nSubCmd, char * pData, int nLen );
	int OnRecvCashShopMessage( CDNUserSession * pSession, int nSubCmd, char *pData, int nLen );	
	int OnRecvChatMessage( CDNUserSession * pSession, int nSubCmd, char *pData, int nLen );
	int OnRecvPvPMessage(CDNUserSession * pSession, int nSubCmd, char *pData, int nLen);
	int OnRecvGestureMessage( CDNUserSession * pSession, int nSubCmd, char *pData, int nLen );
	int OnRecvPlayerCustomEventUI(CDNUserSession * pSession, int nSubCmd, char *pData, int nLen);

public:
	int OnDispatchMessage( CDNUserSession * pSession, int nMainCmd, int nSubCmd, char *pData, int nLen );

	virtual int OnRecvNpcTalkMsg( CDNUserSession * pSession, CSNpcTalk *pPacket, int nLen ) { return ERROR_NONE; }
	virtual int OnRecvNpcTalkEndMsg( CDNUserSession * pSession, CSNpcTalkEnd *pPacket, int nLen ) { return ERROR_NONE; }
	virtual int OnRecvChatChatMsg( CDNUserSession * pSession, CSChat *pPacket, int nLen ) { return ERROR_NONE; }
	virtual int OnRecvChatPrivateMsg( CDNUserSession * pSession, CSChatPrivate *pPacket, int nLen ) { return ERROR_NONE; }
	virtual int OnRecvDiceMsg(CDNUserSession* pSession, CSDice* pPacket, int nLen)	{ return ERROR_NONE; }
	virtual int OnRecvChatChatRoomMsg( CDNUserSession * pSession, CSChatChatRoom *pPacket, int nLen ) { return ERROR_NONE; }
#if defined (PRE_ADD_ANTI_CHAT_SPAM)
	virtual int OnRecvChatSpammer( CDNUserSession * pSession, bool* bSpammer ) { return ERROR_NONE; }
#endif

	virtual int OnRecvRoomRequestDungeonFailed( CDNUserSession *pSession, char *pPacket ) { return ERROR_NONE; }
	virtual int OnRecvRoomWarpDungeon( CDNUserSession * pSession, CSWarpDungeon *pPacket ) { return ERROR_NONE; }
	virtual int OnRecvRoomSelectRewardItem( CDNUserSession * pSession, CSSelectRewardItem *pPacket ) { return ERROR_NONE; }
	virtual int OnRecvRoomIdentifyRewardItem( CDNUserSession * pSession, CSIdentifyRewardItem *pPacket ) { return ERROR_NONE; }
	virtual int OnRecvRoomDLChallengeResponse( CDNUserSession* pSession, CSDLChallengeResponse* pPacket ){ return ERROR_NONE; }

	virtual int OnRecvQuestCompleteCutScene( CDNUserSession * pSession, char *pPacket ) { return ERROR_NONE; }
	virtual int OnRecvQuestSkipCutScene( CDNUserSession * pSession, char *pPacket ) { return ERROR_NONE; }

	virtual int OnRecvSelectAppellation( CDNUserSession *pSession, CSSelectAppellation *pPacket ) { return ERROR_NONE; }
	virtual int OnRecvCollectionBook( CDNUserSession *pSession, CSCollectionBook *pPacket ) { return ERROR_NONE; }

	virtual void SetSyncComplete( bool bFlag ) { m_bSyncComplete = bFlag; }
	bool IsSyncComplete() { return m_bSyncComplete; }

	void SetGateIndex( int nValue ) { m_nGateIndex = nValue; }
};