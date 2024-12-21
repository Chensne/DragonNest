#include "StdAfx.h"
#include "GameServerSession.h"
#include "DnActor.h"
#include "GameSendPacket.h"
#include "DnWorld.h"
#include "DnWorldProp.h"
#include "Task.h"
#include "DNProtocol.h"
#include "DNGameRoom.h"
#include "ExceptionReport.h"
#include "DNServiceConnection.h"
#include "DNRestraint.h"
#include "DnWorldKeepOperationProp.h"

CGameServerSession::CGameServerSession()
{
	m_bSyncComplete = false;

	m_nStageRandomSeed = 0;
	m_nGateIndex = 0;
}

CGameServerSession::~CGameServerSession()
{
}

int CGameServerSession::OnDispatchMessage( CDNUserSession * pSession, int nMainCmd, int nSubCmd, char *pData, int nLen )
{
	switch( nMainCmd )
	{
		case CS_CHAR: return OnRecvCharMessage( pSession, nSubCmd, pData, nLen );
		case CS_ACTOR: return OnRecvActorMessage( pSession, nSubCmd, pData, nLen );
		case CS_ACTORBUNDLE: return OnRecvActorBundleMessage( pSession, nSubCmd, pData, nLen );
		case CS_NPC: return OnRecvNpcMessage( pSession, nSubCmd, pData, nLen );
		case CS_QUEST: return OnRecvQuestMessage( pSession, nSubCmd, pData, nLen );
		case CS_CHAT: return OnRecvChatMessage( pSession, nSubCmd, pData, nLen );
		case CS_PROP: return OnRecvPropMessage( pSession, nSubCmd, pData, nLen );
		case CS_ROOM: return OnRecvRoomMessage( pSession, nSubCmd, pData, nLen );
		case CS_TRADE: return OnRecvTradeMessage( pSession, nSubCmd, pData, nLen );
		case CS_APPELLATION: return OnRecvAppellationMessage( pSession, nSubCmd, pData, nLen );
		case CS_CASHSHOP: return OnRecvCashShopMessage( pSession, nSubCmd, pData, nLen );
		case CS_PVP: return OnRecvPvPMessage(pSession, nSubCmd, pData, nLen);
		case CS_GESTURE: return OnRecvGestureMessage(pSession, nSubCmd, pData, nLen);
		case CS_CUSTOMEVENTUI: return OnRecvPlayerCustomEventUI(pSession, nSubCmd, pData, nLen);
	}
	return ERROR_NONE;
}

int CGameServerSession::OnRecvCharMessage( CDNUserSession * pSession, int nSubCmd, char *pData, int nLen )
{
	return pSession->OnRecvCharMessage( nSubCmd, pData, nLen );
}

int CGameServerSession::OnRecvTradeMessage( CDNUserSession * pSession, int nSubCmd, char *pData, int nLen )
{
	return pSession->OnRecvTradeMessage( nSubCmd, pData, nLen );
}

int CGameServerSession::OnRecvActorMessage( CDNUserSession * pSession, int nSubCmd, char *pData, int nLen )
{
	//액터관련 메세지는 세션스테이트가 플레이상태가 아니면 처리하지 않는게 좋겠다.
	if (pSession->GetState() != SESSION_STATE_GAME_PLAY) return ERROR_NONE;

	CSActorMessage *pPacket = (CSActorMessage *)pData;

	DnActorHandle hActor = CDnActor::FindActorFromUniqueID( pSession->GetGameRoom(), pPacket->nSessionID ); // SessionID 변경후 - 변경전 pSession->GetSessionID()
	if( !hActor ) return ERROR_NONE;

	CDnBaseTransAction *pTrans = static_cast<CDnBaseTransAction *>(hActor.GetPointer());
	if( pTrans ) pTrans->OnDispatchMessage( pSession, nSubCmd, (BYTE*)pPacket->cBuf );
	
	if (pSession->GetKeepOperationID() > 0)
	{
		switch(nSubCmd)
		{
		case eActor::CS_CMDMOVE:
		case eActor::CS_CMDSTOP:
			{
				DnPropHandle hProp = CDnWorldProp::FindPropFromUniqueID(pSession->GetGameRoom(), pSession->GetKeepOperationID());
				if( !hProp ) return ERROR_NONE;

				CDnWorldKeepOperationProp * pKeepOperation = static_cast<CDnWorldKeepOperationProp*>(hProp.GetPointer());
				if(pKeepOperation) 
					pKeepOperation->OnCancelKeepOperationProp( pSession );
			}
			break;
		}
	}

	return ERROR_NONE;
}

int CGameServerSession::OnRecvActorBundleMessage( CDNUserSession * pSession, int nSubCmd, char *pData, int nLen )
{
	//액터관련 메세지는 세션스테이트가 플레이상태가 아니면 처리하지 않는게 좋겠다.
	if (pSession->GetState() != SESSION_STATE_GAME_PLAY) return ERROR_NONE;

	CSActorBundleMessage *pPacket = (CSActorBundleMessage *)pData;

	BYTE cSubCmd;
	WORD nCurSize;
	int nOffset = 0;
	CSActorMessage Packet;
	ZeroMemory(&Packet, sizeof(CSActorMessage));
	for( int i=0; i<nSubCmd; i++ ) {
		memcpy( &Packet.nSessionID, pPacket->cBuf + nOffset, sizeof(DWORD) ); nOffset += sizeof(DWORD);
		memcpy( &cSubCmd, pPacket->cBuf + nOffset, sizeof(BYTE) ); nOffset += sizeof(BYTE);
		memcpy( &nCurSize, pPacket->cBuf + nOffset, sizeof(WORD) ); nOffset += sizeof(WORD);
		if( nCurSize <= _countof(Packet.cBuf) )
		{
			memcpy( Packet.cBuf, pPacket->cBuf + nOffset, nCurSize ); nOffset += nCurSize;
		}
		else
		{
			g_Log.Log( LogType::_ERROR, pSession, L"나쁜놈 패킷을 이상하게 보냄!!! SubCmd=%d CurSize=%d\r\n", static_cast<int>(cSubCmd), nCurSize );
			continue;
		}

		OnRecvActorMessage( pSession, cSubCmd, (char*)&Packet, nCurSize );
	}
	return ERROR_NONE;
}

int CGameServerSession::OnRecvNpcMessage( CDNUserSession * pSession, int nSubCmd, char *pData, int nLen )
{
	switch( nSubCmd ) {
		case eNpc::CS_NPCTALK: return OnRecvNpcTalkMsg( pSession, (CSNpcTalk*)pData, nLen );
		case eNpc::CS_NPCTALKEND: return OnRecvNpcTalkEndMsg( pSession, (CSNpcTalkEnd*)pData, nLen );
	}
	return ERROR_NONE;
}

int CGameServerSession::OnRecvQuestMessage( CDNUserSession * pSession, int nSubCmd, char *pData, int nLen )
{
	switch( nSubCmd ) {
		case eQuest::CS_COMPLETE_CUTSCENE: return OnRecvQuestCompleteCutScene( pSession, pData );
		case eQuest::CS_SKIP_CUTSCENE: return OnRecvQuestSkipCutScene( pSession, pData );
	}
	return pSession->OnRecvQuestMessage( nSubCmd, pData, nLen );
}

int CGameServerSession::OnRecvChatMessage( CDNUserSession * pSession, int nSubCmd, char *pData, int nLen )
{
	if (pSession->GetRestraint()->CheckRestraint(_RESTRAINTTYPE_CHAT) == false)
		return ERROR_NONE;

	switch( nSubCmd ) {
		case eChat::CS_CHATMSG: return OnRecvChatChatMsg( pSession, (CSChat*)pData, nLen );
		case eChat::CS_CHAT_PRIVATE: return OnRecvChatPrivateMsg( pSession, (CSChatPrivate*)pData, nLen );
		case eChat::CS_DICE: return OnRecvDiceMsg(pSession, (CSDice*)pData, nLen);
		case eChat::CS_CHAT_CHATROOM: return OnRecvChatChatRoomMsg( pSession, (CSChatChatRoom*)pData, nLen );
#if defined( PRE_ADD_ANTI_CHAT_SPAM)
		case eChat::CS_CHAT_SPAMMER : return OnRecvChatSpammer( pSession, (bool*)pData);
#endif
	}

	return ERROR_NONE;
}

int CGameServerSession::OnRecvPvPMessage(CDNUserSession * pSession, int nSubCmd, char *pData, int nLen)
{
	return pSession->OnRecvPvPMessage(nSubCmd, pData, nLen);
}

int CGameServerSession::OnRecvRoomMessage(CDNUserSession * pSession, int nSubCmd, char * pData, int nLen )
{
	switch( nSubCmd ) {
		case eRoom::CS_REQUEST_DUNGEONFAILED: return OnRecvRoomRequestDungeonFailed( pSession, pData );
		case eRoom::CS_WARP_DUNGEON: return OnRecvRoomWarpDungeon( pSession, (CSWarpDungeon*)pData );
		case eRoom::CS_SELECT_REWARDITEM: return OnRecvRoomSelectRewardItem( pSession, (CSSelectRewardItem *)pData );
		case eRoom::CS_IDENTIFY_REWARDITEM: return OnRecvRoomIdentifyRewardItem( pSession, (CSIdentifyRewardItem *)pData );
		case eRoom::CS_DLCHALLENGE_RESPONSE: return OnRecvRoomDLChallengeResponse( pSession, (CSDLChallengeResponse*)pData );
	}
	return ERROR_NONE;
}

int CGameServerSession::OnRecvAppellationMessage( CDNUserSession * pSession, int nSubCmd, char * pData, int nLen )
{
	switch( nSubCmd ) {
		case eAppellation::CS_SELECT_APPELLATION: return OnRecvSelectAppellation( pSession, (CSSelectAppellation*)pData );
#if defined(PRE_MOD_APPELLATIONBOOK_RENEWAL)
#else
		case eAppellation::CS_COLLECTIONBOOK: return OnRecvCollectionBook( pSession, (CSCollectionBook*)pData );
#endif	// #if defined(PRE_MOD_APPELLATIONBOOK_RENEWAL)
	}
	return ERROR_NONE;
}

int CGameServerSession::OnRecvCashShopMessage( CDNUserSession * pSession, int nSubCmd, char *pData, int nLen )
{
	return pSession->OnRecvCashShopMessage(nSubCmd, pData, nLen);
}

int CGameServerSession::OnRecvPropMessage( CDNUserSession * pSession, int nSubCmd, char *pData, int nLen )
{
	CSPropMessage *pPacket = (CSPropMessage *)pData;

	DnPropHandle hProp = CDnWorldProp::FindPropFromUniqueID( pSession->GetGameRoom(), pPacket->nUniqueID );
	if( !hProp ) return ERROR_NONE;

	CDnBaseTransAction *pTrans = static_cast<CDnBaseTransAction *>(hProp.GetPointer());
	if( pTrans ) pTrans->OnDispatchMessage( pSession, nSubCmd, (BYTE*)pPacket->cBuf );

	return ERROR_NONE;
}

int CGameServerSession::OnRecvGestureMessage( CDNUserSession * pSession, int nSubCmd, char *pData, int nLen )
{
	return pSession->OnRecvGestureMessage(nSubCmd, pData, nLen);
}

int CGameServerSession::OnRecvPlayerCustomEventUI(CDNUserSession * pSession, int nSubCmd, char *pData, int nLen)
{
	return pSession->OnRecvPlayerCustomEventUI(nSubCmd, pData, nLen);
}
