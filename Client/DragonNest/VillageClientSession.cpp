#include "StdAfx.h"
#include "VillageClientSession.h"
#include "VillageSendPacket.h"
#include "PartySendPacket.h"
#include "DnTableDB.h"
#include "DnInterface.h"
#include "DnPartyTask.h"
#include "DnMessageManager.h"
#include "DNProtocol.h"
#include "DnLoadingTask.h"
#include "DnMainMenuDlg.h"
#include "TaskManager.h"
#include "DNCommonDef.h"
#include "DnPVPGameRoomDlg.h"
#include "DnBridgeTask.h"
#include "DnLoginTask.h"
#include "DnItemTask.h"
#include "./boost/format.hpp"
#include "DnPVPLadderSystemDlg.h"
#include "DnPVPLadderTabDlg.h"
#include "DnPVPLadderInviteConfirmDlg.h"

#include "DnPVPLobbyChatTabDlg.h"
#include "DnPVPRoomListDlg.h"
#ifdef PRE_ADD_COOKING_SYSTEM
#include "DnLifeSkillCookingTask.h"
#endif // PRE_ADD_COOKING_SYSTEM

#ifdef PRE_ADD_DWC
#include "DnPVPDWCTabDlg.h"
#include "PvPSendPacket.h"
#include "DnDWCTask.h"
//#include "DnCommonTask.h"
#endif

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

#ifdef PRE_MOD_REQ_JOIN_PARTY_ANSWER_MSG_APP
#include "DnDirectDlg.h"
#include "DnContextMenuDlg.h"
#endif 

CVillageClientSession::CVillageClientSession() : CTaskListener(false)
{
	m_nVillageMapIndex = -1;
	m_nVillageMapEnviIndex = -1;
	m_nVillageMapEnviArrayIndex = -1;
	m_nVillageMapArrayIndex = -1;

	m_fRefreshPartyDelta = 0.f;

	m_bRefreshPartyList = false;

	m_nStageRandomSeed = 0;
	m_nGateIndex = 0;
	m_nMainClientUniqueID = -1;

	m_iGameServerIP = 0;
	m_iGameServerPort = 0;

	m_nPartyListCount = 0;
	m_nPartyListCurrentPage = -1;
#ifdef PRE_PARTY_DB
	m_unPartyListStartPageFromServer = 0;
	m_nPartyListTotalCount = 0;
#endif

	m_nChannelID = 0;
	m_nChannelIdx = 0;
	
	m_VillageType = VillageTypeEnum::WorldVillage;
	memset(&m_MeritInfo,0,sizeof(m_MeritInfo));
	
#ifdef PRE_ADD_COLOSSEUM_BEGINNER
	m_bPVPChannelGradeUpped = false;
#endif // #ifdef PRE_ADD_COLOSSEUM_BEGINNER
}

CVillageClientSession::~CVillageClientSession()
{
}

void CVillageClientSession::OnConnectTcp(void)
{
	OutputDebug( "OnConnect\n" );

}

void CVillageClientSession::OnDisconnectTcp( bool bValidDisconnect )
{
	if( !bValidDisconnect )
		OutputDebug( "OnDisconnect\n" );
}

void CVillageClientSession::OnDispatchMessage( int nMainCmd, int nSubCmd, char *pData, int nSize )
{
	if( CDnLoadingTask::IsActive() ) {
		if( CDnLoadingTask::GetInstance().InsertLoadPacket( this, nMainCmd, nSubCmd, (void*)pData, nSize ) ) return;
	}
 
	bool bProcessDispatch = false;
	switch( nMainCmd ) {
		case SC_CHAR: OnRecvCharMessage( nSubCmd, pData, nSize ), bProcessDispatch = true; break;
		case SC_PARTY: OnRecvPartyMessage( nSubCmd, pData, nSize ), bProcessDispatch = true; break;
		case SC_ACTOR: OnRecvActorMessage( nSubCmd, pData, nSize ), bProcessDispatch = true; break;
		case SC_PVP: OnRecvPVPMessage(nSubCmd, pData, nSize), bProcessDispatch = true; break;
		case SC_FARM: OnRecvFarmMessage(nSubCmd, pData, nSize), bProcessDispatch = true; break;
	}
	if( bProcessDispatch ) {
		WriteLog( 1, ", Info, CVillageClientSession::OnDispatchMessage End : nMainCmd(%d), nSubCmd(%d), nSize(%d)\n", nMainCmd, nSubCmd, nSize );
	}

#if 0
	GetMessageManager().RemoveMessage( nMainCmd, nSubCmd );
#endif
}

void CVillageClientSession::OnRecvPVPMessage(int nSubCmd, char *pData, int nSize )
{
	switch( nSubCmd ) 
	{
		case ePvP::SC_CREATEROOM:
			OnRecvCreateRoom( (SCPVP_CREATEROOM * )pData );
			break;
		case ePvP::SC_LEAVEROOM:
			OnRecvLeaveRoom( (SCPVP_LEAVEROOM * )pData );
			break;
		case ePvP::SC_ROOMLIST:
			OnRecvPVPRoomList( (SCPVP_ROOMLIST*)pData );
			break;
		case ePvP::SC_JOINROOM:
			OnRecvJoinRoom( (SCPVP_JOINROOM*)pData );
			break;
		case ePvP::SC_JOINUSER:
			OnRecvJoinUser( (SCPVP_JOINUSER *) pData );
			break;
		case ePvP::SC_CHANGETEAM:
			OnRecvChangeTeam( (SCPVP_CHANGETEAM *) pData );
			break;
		case ePvP::SC_USERSTATE:
			OnRecvUserState( (SCPVP_USERSTATE *) pData );
			break;
		case ePvP::SC_START:
			OnRecvStart( (SCPVP_START *) pData );
			break;
		case ePvP::SC_ROOMSTATE:
			OnRecvStart( (SCPVP_ROOMSTATE *) pData );
			break;
		case ePvP::SC_STARTMSG:
			OnRecvStartMsg( (SCPVP_STARTMSG *) pData );
			break;
		case ePvP::SC_MODIFYROOM:
			OnRecvModifyRoom( (SCPVP_MODIFYROOM *) pData );
			break;
		case ePvP::SC_ROOMINFO:
			OnRecvPvPRoomInfo( pData );
			break;
		case ePvP::SC_LEVEL:
			OnRecvPvPLevel( pData );
			break;
		case ePvP::SC_WAITUSERLIST:
			OnRecvPvPWaitUserList((SCPVP_WAITUSERLIST *) pData);
			break;

#ifdef PRE_ADD_COLOSSEUM_BEGINNER
		case ePvP::SC_PVP_CHANGE_CHANNEL:
			OnRecvPvPChangeChannel((SCPVP_CHANGE_CHANNEL *) pData);
			break;
#endif // #ifdef PRE_ADD_COLOSSEUM_BEGINNER
		case ePvP::SC_ENTER_LADDERCHANNEL:
			OnRecvPvPEnterLadderChannel((LadderSystem::SC_ENTER_CHANNEL*) pData);
			break;
		case ePvP::SC_LEAVE_LADDERCHANNEL:
			OnRecvPvPLeaveLadderChannel((LadderSystem::SC_LEAVE_CHANNEL*) pData);
			break;
		case ePvP::SC_LADDER_NOTIFY_LEADER:
			OnRecvPVPNotifyLeader((LadderSystem::SC_NOTIFY_LEADER*) pData);
			break;
		case ePvP::SC_LADDER_NOTIFY_ROOMSTATE:
			OnRecvPVPLadderState((LadderSystem::SC_NOTIFY_ROOMSTATE*) pData);
			break;
		case ePvP::SC_LADDER_MATCHING:
			OnRecvPVPMatchingResult((LadderSystem::SC_LADDER_MATCHING*) pData);
			break;
		case ePvP::SC_LADDER_NOTIFY_GAMEMODE_TABLEID:
			OnRecvPVPLadderGameMode((LadderSystem::SC_NOTIFY_GAMEMODE_TABLEID*) pData);
			break;
		case ePvP::SC_LADDER_PLAYING_ROOMLIST:
			OnRecvPVPLadderRoomList((LadderSystem::SC_PLAYING_ROOMLIST*) pData);
			break;
		case ePvP::SC_LADDER_SCOREINFO:
			OnRecvPVPLadderScoreInfo((LadderSystem::SC_SCOREINFO*) pData);
			break;
		case ePvP::SC_LADDER_SCOREINFO_BYJOB:
			OnRecvPVPLadderJobScoreInfo((LadderSystem::SC_SCOREINFO_BYJOB*) pData);
			break;
		case ePvP::SC_LADDER_MATCHING_AVGSEC:
			OnRecvPVPLadderTimeLeft((LadderSystem::SC_MATCHING_AVGSEC*) pData);
			break;
		case ePvP::SC_LADDER_POINT_REFRESH:
			OnRecvPVPLadderPointRefresh((LadderSystem::SC_LADDERPOINT_REFRESH*) pData);
			break;
		case ePvP::SC_LADDER_NOTIFY_JOINUSER:
			OnRecvPVPLadderJoinUser((LadderSystem::SC_NOTIFY_JOINUSER*) pData);
			break;
		case ePvP::SC_LADDER_NOTIFY_LEAVEUSER:
			OnRecvPVPLadderLeaveUser((LadderSystem::SC_NOTIFY_LEAVEUSER*) pData);
			break;
		case ePvP::SC_LADDER_INVITE:
			OnRecvPVPLadderInviteUser((LadderSystem::SC_INVITE*) pData);
			break;
		case ePvP::SC_LADDER_INVITE_CONFIRM_REQ:
			OnRecvPVPLadderInviteConfirmReq((LadderSystem::SC_INVITE_CONFIRM_REQ*) pData);
			break;
		case ePvP::SC_LADDER_INVITE_CONFIRM:
			OnRecvPVPLadderInviteConfirm((LadderSystem::SC_INVITE_CONFIRM*) pData);
			break;
		case ePvP::SC_LADDER_REFRESH_USERINFO:
			OnRecvPVPLadderRefreshUserInfo((LadderSystem::SC_REFRESH_USERINFO*) pData);
			break;
		case ePvP::SC_LADDER_KICKOUT:
			OnRecvPVPLadderKickOut((LadderSystem::SC_KICKOUT*) pData);
			break;
		case ePvP::SC_PVP_GHOULSCORES:
			OnRecvPVPGhoulScoreInfo((SCPVP_GHOUL_SCORES*) pData);
			break;
#ifdef PRE_ADD_PVP_VILLAGE_ACCESS
		case ePvP::SC_PVP_LIST_OPEN_UI:
			OnRecvOpenPVPVillageAcess((SCPvPListOpenUI*) pData);
			break;
#endif

#ifdef PRE_ADD_PVP_TOURNAMENT
		case ePvP::SC_PVP_SWAP_TOURNAMENT_INDEX:
			OnRecvPvPTournamentSwapIndex((SCPvPSwapTournamentIndex*) pData);
			break;
#endif
	}
}


void CVillageClientSession::OnRecvFarmMessage(int nSubCmd, char * pData, int nSize)
{
	switch (nSubCmd)
	{
	case eFarm::SC_FARMINFO:
		{
			SCFarmInfo * pPacket = (SCFarmInfo*)pData;

			//농장마을에서 게이트 접촉시 이패킷이 옵니다. 이때 UI띄워주세요 상세 정보는 이다음패킷으로 날라옵니다.
			OnRecvFarmInfo( pPacket );

#ifdef PRE_ADD_COOKING_SYSTEM
			if( CDnLifeSkillCookingTask::IsActive() && GetLifeSkillCookingTask().IsNowCooking() )
				GetLifeSkillCookingTask().CancelManufacture();
#endif // PRE_ADD_COOKING_SYSTEM
		}
		break;

	case eFarm::SC_FARMPLANTEDINFO:
		{
			SCFarmPlantedInfo * pPacket = (SCFarmPlantedInfo*)pData;

			//농장의 정보를 받은 후에 내가 심은 데이타가 옵니다. 들어오면 UI에 추가해주세요.
			OnRecvFarmPlantedInfo( pPacket );
		}
		break;

	case eFarm::SC_OPEN_FARMLIST:
		{
			OnRecvOpenFarmList();
		}
		break;
	}
}

void CVillageClientSession::OnRecvFarmInfo(SCFarmInfo * pPacket)
{
	FUNC_LOG();
}

void CVillageClientSession::OnRecvFarmPlantedInfo( SCFarmPlantedInfo * pPacket )
{
	FUNC_LOG();
}

void CVillageClientSession::OnRecvOpenFarmList()
{
	FUNC_LOG();
}

void CVillageClientSession::OnRecvCharMessage( int nSubCmd, char *pData, int nSize )
{
	switch( nSubCmd ) {
		case eChar::SC_MAPINFO: OnRecvCharMapInfo( (SCMapInfo*)pData ); break;
		case eChar::SC_ENTER: OnRecvCharEntered( (SCEnter *)pData ); break;
		case eChar::SC_ENTERUSER: OnRecvCharUserEntered( (SCEnterUser*)pData, nSize ); break;
		case eChar::SC_LEAVEUSER: OnRecvCharUserLeave( (SCLeaveUser*)pData ); break;
		case eChar::SC_HIDE: OnRecvCharHide( (SCHide *)pData ); break;
	}
}

void CVillageClientSession::OnRecvPartyMessage( int nSubCmd, char *pData, int nSize )
{
	switch( nSubCmd ) {
		case eParty::SC_CREATEPARTY: OnRecvPartyCreateParty( (SCCreateParty *)pData ); break;
		case eParty::SC_JOINPARTY: OnRecvPartyJoinParty( (SCJoinParty *)pData ); break;

		case eParty::SC_REFRESHPARTY: OnRecvPartyRefresh( (SCRefreshParty *)pData ); break;
		case eParty::SC_PARTYOUT: OnRecvPartyOutParty( (SCPartyOut*)pData ); break;
		case eParty::SC_PARTYLISTINFO: OnRecvPartyListInfo( (SCPartyListInfo*)pData ); break;
		case eParty::SC_PARTYMEMBER_INFO: OnRecvPartyMemberInfo( (SCPartyMemberInfo*)pData ); break;
		case eParty::SC_PARTYINFO_ERROR: OnRecvPartyInfoError( (SCPartyInfoErr*)pData); break;

		case eParty::SC_REFRESHGATEINFO: OnRecvPartyRefreshGateInfo( (SCRefreshGateInfo *)pData ); break;
		case eParty::SC_GATEINFO: OnRecvPartyReadyGate( (SCGateInfo *)pData ); break;
		case eParty::SC_CANCELSTAGE: OnRecvPartyStageCancel( (SCCancelStage *)pData ); break;
		case eParty::SC_CHANNELLIST: OnRecvPartyMoveChannelList((SCChannelList*)pData); break;
		case eParty::SC_MOVECHANNELFAIL: OnRecvPartyMevoChannelFail((SCMoveChannelFail*)pData); break;

		case eParty::SC_PARTYMEMBER_MOVE: OnRecvPartyMemberMove((SCPartyMemberMove *)pData); break;
	}
}

void CVillageClientSession::OnRecvActorMessage( int nSubCmd, char *pData, int nSize )
{
	SCActorMessage *pPacket = (SCActorMessage *)pData;

	DnActorHandle hActor = CDnActor::FindActorFromUniqueID( pPacket->nSessionID );
	if( !hActor ) return;
	MATransAction *pTrans = dynamic_cast<MATransAction *>(hActor.GetPointer());
	if( pTrans ) pTrans->OnDispatchMessage( nSubCmd, (BYTE*)pPacket->cBuf );
}

void CVillageClientSession::OnRecvCharMapInfo( SCMapInfo *pPacket )
{
	FUNC_LOG();
	m_nVillageMapIndex = pPacket->nMapIndex;
	m_nVillageMapArrayIndex = pPacket->cMapArrayIndex;
	m_nVillageMapEnviIndex = pPacket->cEnviIndex;
	m_nVillageMapEnviArrayIndex = pPacket->cEnviArrayIndex;
	m_nChannelID = pPacket->nChannelID;
	m_nChannelIdx = pPacket->wChannelIdx;
	m_MeritInfo = pPacket->MeritInfo;
}

#ifdef PRE_PARTY_DB
void CVillageClientSession::ResetPartyList()
{
	m_unPartyListStartPageFromServer = 0;
	m_nPartyListTotalCount = 0;
	CDnPartyTask::GetInstance().ResetPartyList();
}
#endif

#ifdef PRE_PARTY_DB
bool CVillageClientSession::OnRecvPartyListInfo( SCPartyListInfo *pPacket )
#else
void CVillageClientSession::OnRecvPartyListInfo( SCPartyListInfo *pPacket )
#endif
{
	FUNC_LOG();

#ifdef PRE_PARTY_DB
	if (CDnPartyTask::IsActive())
		CDnPartyTask::GetInstance().LockPartyReqPartyList(false);

	if (pPacket->nRetCode != ERROR_NONE)
	{
		if (pPacket->nRetCode == ERROR_PARTY_PARTYLIST_OVERFLOW)
		{
			ResetPartyList();
			return false;
		}

		std::wstring msg;
		GetInterface().GetServerMessage(msg, pPacket->nRetCode);
		GetInterface().AddChatMessage(CHATTYPE_SYSTEM, L"", msg.c_str(), true);
		return false;
	}
#endif

	m_mapPartyListInfo.clear();

#if defined( PRE_PARTY_DB )
#else
	if( pPacket->cCount >= PARTYLISTOFFSET )
	{
		ASSERT(0);
	}
#endif // #if defined( PRE_PARTY_DB )

#if defined( PRE_PARTY_DB )
	for( int i=0; i<pPacket->iListCount; i++ )
#else
	for( int i=0; i<pPacket->cCount; i++ )
#endif // #if defined( PRE_PARTY_DB )
	{
		PartyListStruct Struct;
		Struct.nCurCount = pPacket->PartyListInfo[i].cCurrentCount;
		Struct.nMaxCount = pPacket->PartyListInfo[i].cMaxCount;
		Struct.PartyID = pPacket->PartyListInfo[i].PartyID;
		Struct.nUserLvLimitMin = pPacket->PartyListInfo[i].cUserLvLimitMin;
#if defined( PRE_PARTY_DB )
	#ifdef _WORK
	//	Struct.sortPoint = pPacket->PartyListInfo[i].biSortPoint;
	#endif // _WORK
		Struct.nCurrentMapIdx = pPacket->PartyListInfo[i].iCurMapIndex;
		Struct.bIsBonus = pPacket->PartyListInfo[i].bIsBonus;
#else
		Struct.nUserLvLimitMax = pPacket->PartyListInfo[i].cUserLvLimitMax;
#endif // #if defined( PRE_PARTY_DB )
		Struct.bSecret = pPacket->PartyListInfo[i].cIsPrivateRoom == 1 ? true : false;
		Struct.bEmpty = false;
#if defined( PRE_PARTY_DB )
#else
		Struct.bMasterFatigueEmpty = (pPacket->PartyListInfo[i].wMasterFatigue != 0) ? false : true;
#endif // #if defined( PRE_PARTY_DB )
		Struct.szPartyName.assign( pPacket->PartyListInfo[i].wszBuf, pPacket->PartyListInfo[i].cPartyNameLen );
		Struct.nMapIdx = pPacket->PartyListInfo[i].nTargetMapIdx;
		Struct.Difficulty = pPacket->PartyListInfo[i].TargetDifficulty;
		Struct.bUseVoice = pPacket->PartyListInfo[i].cUseVoice == 1 ? true : false;
		Struct.PartyType = pPacket->PartyListInfo[i].PartyType;

#ifdef PRE_WORLDCOMBINE_PARTY
		Struct.nWorldCombinePartyTableIndex = pPacket->PartyListInfo[i].nWorldCombinePartyTableIndex;
#endif

#ifdef PRE_ADD_NEWCOMEBACK
		Struct.bComeback = pPacket->PartyListInfo[i].bCheckComeBackParty;
#endif // PRE_ADD_NEWCOMEBACK

		m_mapPartyListInfo.push_back(Struct);
	}

#if defined( PRE_PARTY_DB )
	m_nPartyListTotalCount = pPacket->iTotalCount;
	m_nPartyListCount = pPacket->iListCount;
	m_unPartyListStartPageFromServer = pPacket->unReqPage;

	return true;
#else
	m_nPartyListCount = pPacket->nWholeCount;

	if (pPacket->nRetCode == ERROR_PARTY_CONDITION_MATCHED_LIST_NOT_FOUND)
	{
		std::wstring msg;
		GetInterface().GetServerMessage(msg, pPacket->nRetCode);
		GetInterface().AddChatMessage(CHATTYPE_SYSTEM, L"", msg.c_str(), true);
	}
#endif // #if defined( PRE_PARTY_DB )
}

#ifdef PRE_PARTY_DB
int CVillageClientSession::GetPartyMaxPage() const
{
	int nPartyListTotalCount = GetPartyListTotalCount();

	int nMaxPage = nPartyListTotalCount / PARTYLISTOFFSET;
	if( nPartyListTotalCount % PARTYLISTOFFSET )
		nMaxPage++;

	if( nMaxPage == 0 ) 
		nMaxPage = 1;

	return nMaxPage;
}

bool CVillageClientSession::HasPartyListInfoPage(int nPage) const
{
	if (nPage < 0 || nPage > GetPartyMaxPage())
		return false;

	int nPageOffset = m_nPartyListCount / PARTYLISTOFFSET;
	if (m_nPartyListCount % PARTYLISTOFFSET)
		nPageOffset++;

	return (nPage >= m_unPartyListStartPageFromServer && nPage < (m_unPartyListStartPageFromServer + nPageOffset));
}

int CVillageClientSession::GetPartyListCountOffset(int nPage) const
{
	if (HasPartyListInfoPage(nPage) == false)
		return -1;

	int nOffset = (nPage - m_unPartyListStartPageFromServer) * PARTYLISTOFFSET;
	if (nOffset < 0)
		return -1;

	return nOffset;
}
#endif

void CVillageClientSession::OnRecvPartyMemberInfo( SCPartyMemberInfo *pPacket )
{
#if defined( PRE_PARTY_DB )
	FUNC_LOG();
	int nCount(0);
	m_vecPartyMemberInfo.clear();

	if (pPacket->nCount >= PARTYCOUNTMAX)
	{
		_ASSERT(0);
		return;
	}
	
	SPartyMemberInfo sPartyMemberInfo;
	for (int i = 0; i < pPacket->nCount; i++)
	{
		sPartyMemberInfo.nUserLevel = pPacket->MemberData[i].cLevel;
		sPartyMemberInfo.nJob = pPacket->MemberData[i].cJob;
		sPartyMemberInfo.nClassID = CDnPlayerState::GetJobToBaseClassID(sPartyMemberInfo.nJob);
		sPartyMemberInfo.bMaster = pPacket->MemberData[i].bLeader;
		_wcscpy(sPartyMemberInfo.wszName, _countof(sPartyMemberInfo.wszName), pPacket->MemberData[i].wszCharName, (int)wcslen(pPacket->MemberData[i].wszCharName));
		sPartyMemberInfo.characterDBID = pPacket->MemberData[i].biCharacterDBID;
#ifdef PRE_ADD_NEWCOMEBACK		
		sPartyMemberInfo.bComeback = pPacket->MemberData[i].bCheckComeback;
#endif // PRE_ADD_NEWCOMEBACK

		m_vecPartyMemberInfo.push_back(sPartyMemberInfo);
	}

#ifdef PRE_MOD_REQ_JOIN_PARTY_ANSWER_MSG_APP 

	CDnChatContextMenuDlg* pChatMenuDlg = dynamic_cast<CDnChatContextMenuDlg*>( GetInterface().GetContextMenuDlg(CDnInterface::CONTEXTM_CHAT) ); 
	CDnDirectDlg*		   pDirectDlg = GetInterface().GetDirectDlg();

	if( pDirectDlg && pDirectDlg->IsDirectPlayerAskParty() || 
		pChatMenuDlg && pChatMenuDlg->IsReqPartyWithChat() )
	{
		GetInterface().GetDirectDlg()->EnablePartyDlg( true );
		GetInterface().GetDirectDlg()->SetDirectPlayerAskParty(false);
		pChatMenuDlg->SetReqPartyWithChat(false);
		return;
	}	
#endif

#ifdef PRE_WORLDCOMBINE_PARTY
	CDnPartyTask::GetInstance().OnRecvPartyMemberInfo(pPacket->PartyType);
#else
	CDnPartyTask::GetInstance().OnRecvPartyMemberInfo(pPacket->PartyType ? _RAID_PARTY_8 : _NORMAL_PARTY);
#endif
	CDnPartyTask::GetInstance().LockPartyReqPartyMemberInfo(false);
#else
	FUNC_LOG();
	int nCount(0);

	//PARTYLISTINFO_MAP_ITER iter = m_mapPartyListInfo.find( pPacket->nPartyIdx );
	//if( iter != m_mapPartyListInfo.end() )
	//{
	//	nCount = iter->second.nCurCount;
	//}
	//else
	//{
	//	CDebugSet::ToLogFile( "CVillageClientSession::OnRecvPartyMemberInfo, Invalid Party Index(%d)!", pPacket->nPartyIdx );
	//}

	m_vecPartyMemberInfo.clear();

	SPartyMemberInfo sPartyMemberInfo;
	//pPacket->nPartyID;		//요청했던 파티아이디
	for (int i = 0; i < pPacket->cCount; i++)
	{
		sPartyMemberInfo.bMaster = (pPacket->cLeaderIdx == i) ? true : false;
		//sPartyMemberInfo.nActLevel;	//사용하지 않습니다.
		sPartyMemberInfo.nClassID = pPacket->Info[i].cClassID;
		sPartyMemberInfo.nUserLevel = pPacket->Info[i].cUserLevel;
		sPartyMemberInfo.nJob = pPacket->Info[i].wJob;
		_wcscpy(sPartyMemberInfo.wszName, _countof(sPartyMemberInfo.wszName), pPacket->Info[i].wszName, (int)wcslen(pPacket->Info[i].wszName));
#ifdef PRE_ADD_NEWCOMEBACK
		sPartyMemberInfo.bComeback = pPacket->MemberData[i].bComeback;
#endif // PRE_ADD_NEWCOMEBACK

		m_vecPartyMemberInfo.push_back( sPartyMemberInfo );
	}

	CDnPartyTask::GetInstance().OnRecvPartyMemberInfo(pPacket->cIsRaidParty ? _RAID_PARTY_8 : _NORMAL_PARTY);

	CDnPartyTask::GetInstance().LockPartyReqPartyMemberInfo(false);
#endif // #if defined( PRE_PARTY_DB )
}

void CVillageClientSession::OnRecvPartyInfoError( SCPartyInfoErr *pPacket )
{
#ifdef PRE_PARTY_DB
	int retCode = pPacket->nRetCode;
	switch(retCode)
	{
	case ERROR_PARTY_NOTFOUND_MEMBERINFO:
		{
			GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 100017 ), MB_OK ); // UISTRING : 파티에 참여할 수 없습니다
			RequestPartyListInfo( m_nPartyListCurrentPage );

			if (CDnPartyTask::IsActive())
				CDnPartyTask::GetInstance().LockPartyReqPartyMemberInfo(false);
		}
		break;

	case ERROR_PARTY_REFRESHLIST_TIMERESTRAINT:
	case ERROR_PARTY_LIST_ERROR:
		{
			if (CDnPartyTask::IsActive())
				CDnPartyTask::GetInstance().LockPartyReqPartyList(false);
		}
		break;

	default:
		break;
	}
#else
	if (pPacket->nRetCode == ERROR_PARTY_NOTFOUND_MEMBERINFO)
	{
		GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 100017 ), MB_OK ); // UISTRING : 파티에 참여할 수 없습니다
		RequestPartyListInfo( m_nPartyListCurrentPage );

		CDnPartyTask::GetInstance().LockPartyReqPartyMemberInfo(false);
	}
#endif
}

void CVillageClientSession::OnRecvPartyListInfoErr(SCPartyInfoErr * pPacket)
{
	GetInterface().ServerMessageBox(pPacket->nRetCode);
	CDnPartyTask::GetInstance().LockPartyReqPartyMemberInfo(false);
}

void CVillageClientSession::OnRecvPartyReadyGate( SCGateInfo *pPacket )
{
	FUNC_LOG();
	m_nMainClientUniqueID = pPacket->nLeaderUID;
	m_nGateIndex = pPacket->cGateNo;
}

/*
void CVillageClientSession::OnTRecvPartyStageStart( SCStartStage *pPacket )
{
	LogWnd::Log(1, _T("OnTRecvPartyStageStart "));
	SCStartStage *pData = (SCStartStage *)pPacket;

	TPartyMemberEquip MemberEquip[PARTYMAX];
	memset( &MemberEquip, 0, sizeof(MemberEquip) );
#ifdef _AGENT
	int nOffset = sizeof(TPacketHeader) + sizeof(char) + sizeof(unsigned long) + sizeof(unsigned short);
#else
	int nOffset = sizeof(char) + sizeof(unsigned long) + sizeof(unsigned short);
#endif
	int nSize = 0;

	for( int i=0; i<pData->cCount; i++ ) {
		char cCount = *((char*)pData + nOffset);
		nSize = ( cCount * sizeof(TItemInfo) ) + sizeof(char);
		memcpy( &MemberEquip[i], (char*)pData + nOffset, nSize );
		nOffset += nSize;
	}

	CDnPartyTask::GetInstance().SetEquipInfo( MemberEquip );
}
*/

void CVillageClientSession::OnRecvPartyStageCancel( SCCancelStage *pPacket )
{
	GetInterface().CloseFarmChannel();
}

void CVillageClientSession::Process( LOCAL_TIME LocalTime, float fDelta )
{
#ifdef PACKET_DELAY
	for( DWORD i=0; i<CDnActor::s_pVecProcessList.size(); i++ ) {
		CDnActor::s_pVecProcessList[i]->ProcessDelay( LocalTime, fDelta );
	}
#endif //PACKET_DELAY

	if( m_bRefreshPartyList ) 
	{
		m_fRefreshPartyDelta -= fDelta;

		if( m_fRefreshPartyDelta <= 0.f ) 
		{
#ifdef PRE_PARTY_DB
			if (CDnPartyTask::IsActive() && CDnPartyTask::GetInstance().IsLockedReqPartyList() == false)
			{
				RequestPartyListInfo( m_nPartyListCurrentPage );
#ifdef _WORK
				if (CDnPartyTask::IsActive())
					m_fRefreshPartyDelta = CDnPartyTask::GetInstance().GetPartyListRefreshTime();
				else
					m_fRefreshPartyDelta = Party::Constants::PARTY_LIST_REFRESH_SEC_CLIENT;
#else
				m_fRefreshPartyDelta = Party::Constants::PARTY_LIST_REFRESH_SEC_CLIENT;
#endif
			}
			else
			{
#ifdef _WORK
				if (CDnPartyTask::IsActive())
					m_fRefreshPartyDelta = CDnPartyTask::GetInstance().GetPartyListRefreshTime() * 2;
				else
					m_fRefreshPartyDelta = Party::Constants::PARTY_LIST_REFRESH_SEC_CLIENT * 2;
#else
				m_fRefreshPartyDelta = Party::Constants::PARTY_LIST_REFRESH_SEC_CLIENT * 2;
#endif
			}
#else
			RequestPartyListInfo( m_nPartyListCurrentPage );
			m_fRefreshPartyDelta = 60.0f;
#endif
		}
	}
}

void CVillageClientSession::EnableRefreshPartyList( bool bEnable )
{
	if( m_bRefreshPartyList == bEnable )
		return;

	m_bRefreshPartyList = bEnable;

	if( m_bRefreshPartyList )
	{
		m_fRefreshPartyDelta = 0.f;
	}
}

const CVillageClientSession::PartyListStruct* CVillageClientSession::GetPartyListInfo( TPARTYID PartyIndex ) const
{
	PARTYLISTINFO_MAP_CONST_ITER iter = m_mapPartyListInfo.begin();
	for (; iter != m_mapPartyListInfo.end(); ++iter)
	{
		const PartyListStruct* pPartyStruct = &(*iter);
		if (pPartyStruct)
		{
			if (pPartyStruct->PartyID == PartyIndex)
				return pPartyStruct;
		}
	}

	return NULL;
}

#ifdef PRE_PARTY_DB
void CVillageClientSession::RequestPartyListInfo(int nPage)
{
	const WCHAR* pWord = CDnInterface::GetInstance().GetPartyListSearchWord();
	std::wstring searchWord = (pWord == NULL) ? L"" : pWord;

	RequestPartyListInfo(nPage, searchWord);
}

void CVillageClientSession::RequestPartyListInfo(int nPage, const std::wstring& searchWord)
{
	if( nPage < 0 ) return;
	if (CDnPartyTask::IsActive())
		CDnPartyTask::GetInstance().ReqPartyListInfo(nPage, searchWord);
}
#else
void CVillageClientSession::RequestPartyListInfo( int nPage )
{
	if( nPage < 0 ) return;
	if (CDnPartyTask::IsActive())
	{
		CDnPartyTask::GetInstance().ReqPartyListInfo(nPage, false);
	}

	//정렬 조건이 0 이면 무시
	//::SendReqPartyListInfo( nPage, 0, 0 );
}
#endif

bool __stdcall CVillageClientSession::OnCheckLoadingPacket( void *pThis, UINT nUniqueID, int nMainCmd, int nSubCmd, void *pParam, int nSize )
{
	if( pThis == dynamic_cast<CVillageClientSession*>(CTaskManager::GetInstance().GetTask( "VillageTask" )) ) {
		switch( nMainCmd ) {
			case SC_CHAR:
				switch( nSubCmd ) {
					case eChar::SC_ENTERUSER:
						if( ((SCEnterUser*)pParam)->nSessionID == nUniqueID ) {
							OutputDebug( "EnterUser-Thread\n" );
							return true;
						}
						break;
					case eChar::SC_LEAVEUSER: 
						if( ((SCLeaveUser*)pParam)->nSessionID == nUniqueID ) {
							OutputDebug( "LeaveUser-Thread\n" );
							return true;
						}
						break;
					case eChar::SC_HIDE:
						if( ((SCHide*)pParam)->nSessionID == nUniqueID ) {
							OutputDebug( "Hide-Thread\n" );
							return true;
						}
						break;

				}
				break;
			case SC_ACTOR:
				if( ((SCActorMessage*)pParam)->nSessionID == nUniqueID ) return true;
				break;
		}
	}
	if( pThis == dynamic_cast<CDnItemTask*>(CTaskManager::GetInstance().GetTask( "ItemTask" )) ) {
		switch( nMainCmd ) {
			case SC_ITEM:
				switch( nSubCmd ) {
					case eItem::SC_CHANGEEQUIP:
						if( ((SCChangeEquip *)pParam)->nSessionID == nUniqueID ) return true;
						break;
					case eItem::SC_CHANGECASHEQUIP:
						if( ((SCChangeCashEquip *)pParam)->nSessionID == nUniqueID ) return true;
						break;
					case eItem::SC_CHANGEVEHICLEPARTS:
						if( ((SCChangeVehicleParts *)pParam)->nSessionID == nUniqueID ) return true;
						break;
					case eItem::SC_CHANGEVEHICLECOLOR:
						if( ((SCChangeVehicleColor *)pParam)->nSessionID == nUniqueID ) return true;
						break;
				}
				break;
		}
	}

	return false;
}

bool CVillageClientSession::CompareChannelInfo( const sChannelInfo *s1, const sChannelInfo *s2 )
{
	/*
	CHANNEL_ATT_NORMAL		= 0x00,
	CHANNEL_ATT_PVP			= 0x01,
	CHANNEL_ATT_PVPLOBBY	= 0x02,
	CHANNEL_ATT_MERITBONUS	= 0x04,
	*/
	int nAttrOrder1 = -1;
	int nAttrOrder2 = -1;
	// PvP
	if( s1->nChannelAttribute&CHANNEL_ATT_PVP ) nAttrOrder1 = 2;
	if( s2->nChannelAttribute&CHANNEL_ATT_PVP ) nAttrOrder2 = 2;
	// 다크레어
	if( s1->nChannelAttribute&CHANNEL_ATT_DARKLAIR ) nAttrOrder1 = 1;
	if( s2->nChannelAttribute&CHANNEL_ATT_DARKLAIR ) nAttrOrder2 = 1;
	// 나머지는 노말로 판단.
	if( !(s1->nChannelAttribute&CHANNEL_ATT_PVP) && !(s1->nChannelAttribute&CHANNEL_ATT_DARKLAIR) ) nAttrOrder1 = 100;
	if( !(s2->nChannelAttribute&CHANNEL_ATT_PVP) && !(s2->nChannelAttribute&CHANNEL_ATT_DARKLAIR) ) nAttrOrder2 = 100;

	int nPlayerLevel = 1;
	CDnLoginTask *pLoginTask = (CDnLoginTask *)CTaskManager::GetInstance().GetTask( "LoginTask" );
	if( pLoginTask ) {
		nPlayerLevel = pLoginTask->GetCurrentSelectLevel();
	}
	else if( CDnActor::s_hLocalActor ) {
		nPlayerLevel = CDnActor::s_hLocalActor->GetLevel();
	}
	else {
		ASSERT( false && "유저레벨을 구하지 못했다.");
	}

	if( nAttrOrder1 > nAttrOrder2 ) return true;
	else if( nAttrOrder1 < nAttrOrder2 ) return false;
	
	int nLevelOrder1 = ((s1->cMinLevel <= nPlayerLevel && nPlayerLevel <= s1->cMaxLevel ) ? 0 : 1 );
	int nLevelOrder2 = ((s2->cMinLevel <= nPlayerLevel && nPlayerLevel <= s2->cMaxLevel ) ? 0 : 1 );

	float fUserRatio1 = (float)s1->nCurrentUserCount / s1->nMaxUserCount;
	float fUserRatio2 = (float)s2->nCurrentUserCount / s2->nMaxUserCount;

	if( nLevelOrder1 < nLevelOrder2 ) return true;
	else if( nLevelOrder1 > nLevelOrder2 ) return false;
	else {
		if( nLevelOrder1 == 0 ) {			// 	추천 레벨 안에 있는 경우는 덜혼잡한 순으로 소팅한다.
			if( fUserRatio1 < fUserRatio2 ) return true;
			else if( fUserRatio1 > fUserRatio2 ) return false;
			return false;
		}
		else {		// 추천 레벨 아닌 경우는 채널 인덱스 순으로 소팅한다.
			if( (float)s1->nChannelIdx < (float)s2->nChannelIdx ) return true;
			else if( (float)s1->nChannelIdx > (float)s2->nChannelIdx ) return false;
			return false;
		}
	}
}

void CVillageClientSession::OnRecvPartyMoveChannelList(SCChannelList * pPacket)
{
	FUNC_LOG();

	std::vector<sChannelInfo*> vecChannelInfo;

	for( int i=0; i<pPacket->cCount; i++ )
	{
		vecChannelInfo.push_back( &pPacket->Info[i] );
	}

//#ifndef _FINAL_BUILD
	std::stable_sort( vecChannelInfo.begin(), vecChannelInfo.end(), CompareChannelInfo );
//#else
//	std::random_shuffle( vecChannelInfo.begin(), vecChannelInfo.end() );
//#endif

	for( int i=0; i<(int)vecChannelInfo.size(); i++ )
	{
		GetInterface().AddChannelList( vecChannelInfo[i] );
	}

	GetInterface().SetDefaultChannelList();
}

void CVillageClientSession::OnRecvPartyMevoChannelFail(SCMoveChannelFail * pData)
{
	FUNC_LOG();
	//처리할게 있으면 해주시고
	GetInterface().ServerMessageBox(pData->nRetCode);
}


void CVillageClientSession::OnRecvPVPRoomList( SCPVP_ROOMLIST * pData )
{
	FUNC_LOG();

	if( pData->nRetCode != ERROR_NONE )
	{
		ErrorLog("CVillageClientSession::OnRecvPVPRoomList %d ",pData->nRetCode);
		if( pData->nRetCode != ERROR_GENERIC_MASTERCON_NOT_FOUND )
		{
			CDnBaseRoomDlg::ErrorHandler( ERROR_GENERIC_MASTERCON_NOT_FOUND );
		}
		return;
	}	

	int In=0;

	GetInterface().SetFirstRoom( true );
	
	for(int i =0 ;i < pData->cRoomCount;i++)
	{
		sRoomInfo RoomInfo;		
		
		RoomInfo.cGameMode = pData->RoomInfoList.uiGameModeTableID[i];
		RoomInfo.cRoomState = pData->RoomInfoList.cRoomState[i];
		RoomInfo.nMapIndex = pData->RoomInfoList.uiMapIndex[i];

		RoomInfo.uiObjective = pData->RoomInfoList.unWinCondition[i];

		SecureZeroMemory( RoomInfo.RoomName, sizeof(RoomInfo.RoomName) );

		memcpy( RoomInfo.RoomName, pData->RoomInfoList.wszBuffer + In, sizeof(WCHAR)*pData->RoomInfoList.cRoomNameLen[i]);
		In += pData->RoomInfoList.cRoomNameLen[i];

		RoomInfo.cPlayerNum = pData->RoomInfoList.cCurUserCount[i];
		RoomInfo.cMaxPlayerNum = pData->RoomInfoList.cMaxUserCount[i];
		RoomInfo.nPVPIndex = pData->RoomInfoList.uiPvPIndex[i];

		RoomInfo.bInGameJoin = pData->RoomInfoList.unRoomOptionBit[i] & PvPCommon::RoomOption::BreakInto ? true : false;
		RoomInfo.bDropItem = pData->RoomInfoList.unRoomOptionBit[i] & PvPCommon::RoomOption::DropItem ? true : false;
		RoomInfo.cMaxLevel = pData->RoomInfoList.cMaxLevel[i];
		RoomInfo.cMinLevel = pData->RoomInfoList.cMinLevel[i];

		RoomInfo.nEventID = pData->RoomInfoList.nEventID[i];

		for( int itr = 0; itr < PvPCommon::TeamIndex::Max; ++itr )
			RoomInfo.nGuildDBID[itr] = pData->RoomInfoList.nGuildDBID[i][itr];
#ifdef PRE_MOD_PVPOBSERVER
		RoomInfo.bExtendObserver = pData->RoomInfoList.bExtendObserver[i];
#endif // PRE_MOD_PVPOBSERVER
#ifdef PRE_WORLDCOMBINE_PVP
		RoomInfo.nWorldPvPRoomType = pData->RoomInfoList.nWorldPvPRoomType[i];
#endif // PRE_WORLDCOMBINE_PVP

#ifdef PRE_PVP_GAMBLEROOM
		RoomInfo.cGambleType = pData->RoomInfoList.cGambleType[i];
		RoomInfo.nGamblePrice = pData->RoomInfoList.nGamblePrice[i];
#endif // PRE_PVP_GAMBLEROOM

		GetInterface().AddLobbyList( &RoomInfo ,pData->uiMaxPage);
	}
}

void CVillageClientSession::OnRecvPvPWaitUserList(SCPVP_WAITUSERLIST *pData)
{
	FUNC_LOG();

	if( pData->nRetCode != ERROR_NONE )
	{
		ErrorLog("CVillageClientSession::OnRecvPVPRoomList %d ",pData->nRetCode);
		if( pData->nRetCode != ERROR_GENERIC_MASTERCON_NOT_FOUND )
		{
			CDnBaseRoomDlg::ErrorHandler( ERROR_GENERIC_MASTERCON_NOT_FOUND );
		}
		return;
	}

	if(GetInterface().GetPvPRoomListDlg() && GetInterface().GetPvPRoomListDlg()->IsShow() && GetInterface().GetPvPRoomListDlg()->GetCurrentRefreshUserType() == CDnPVPRoomListDlg::eWaitUser::TypeLobby)
	{
		GetInterface().GetPvPRoomListDlg()->ClearWaitUserList();
		GetInterface().GetPvPRoomListDlg()->SetRefreshUserCount(pData->cUserCount);

		PvPCommon::WaitUserList::Repository Repository;
		memset( &Repository, 0, sizeof(Repository) );

		for( int i=0 ; i<pData->cUserCount ; ++i )
			Repository.WaitUserList[i] = pData->Repository.WaitUserList[i];

		GetInterface().GetPvPRoomListDlg()->SetWaitUserList( Repository, pData->unMaxPage );
	}

	if(	GetInterface().GetPvPLadderSystemDlg() && GetInterface().GetPvPLadderSystemDlg()->GetLadderTeamGameDlg()->GetLadderInviteUserListDlg()->IsShow())
	{
		GetInterface().GetPvPLadderSystemDlg()->GetLadderTeamGameDlg()->GetLadderInviteUserListDlg()->ClearWaitUserList();
		GetInterface().GetPvPLadderSystemDlg()->GetLadderTeamGameDlg()->GetLadderInviteUserListDlg()->SetRefreshUserCount(pData->cUserCount);

		PvPCommon::WaitUserList::Repository Repository;
		memset( &Repository, 0, sizeof(Repository) );
		for( int i=0 ; i<pData->cUserCount ; ++i )
			Repository.WaitUserList[i] = pData->Repository.WaitUserList[i];
		GetInterface().GetPvPLadderSystemDlg()->GetLadderTeamGameDlg()->GetLadderInviteUserListDlg()->SetWaitUserList( Repository, pData->unMaxPage );
	}
}

#ifdef PRE_ADD_COLOSSEUM_BEGINNER
void CVillageClientSession::OnRecvPvPChangeChannel( SCPVP_CHANGE_CHANNEL* pData )
{
	if( pData->nRetCode == ERROR_NONE )
	{
		// 채널 변경 서버에서 승인. UI 에 알려줌.
		CDnPVPLadderTabDlg* pPVPLadderTabDlg = GetInterface().GetPVPLadderTabDlg();
		CDnPVPRoomListDlg* pPVPRoomListDlg = GetInterface().GetPvPRoomListDlg();
		if( pPVPLadderTabDlg && pPVPRoomListDlg )
		{
			pPVPLadderTabDlg->SetSelectedGradeChannelTab( (PvPCommon::RoomType::eRoomType)pData->cType );
			pPVPRoomListDlg->SetSelectedGradeChannel( (PvPCommon::RoomType::eRoomType)pData->cType );

			if( pPVPRoomListDlg->IsShow() )
				pPVPRoomListDlg->RefreshRoomByChangeGradeChannel();

		}
	}
	else
	{
		// TODO: 채널 변경 실패. 메시지 뿌려줌.
		
	}
}
#endif // #ifdef PRE_ADD_COLOSSEUM_BEGINNER

void CVillageClientSession::OnRecvPvPEnterLadderChannel(LadderSystem::SC_ENTER_CHANNEL *pData)
{
	if(pData->iRet != ERROR_NONE)
	{
		if(pData->iRet == ERROR_LADDERSYSTEM_CANT_ENTER_LEVELLIMIT)
		{
			GetInterface().GetPVPLadderTabDlg()->SelectLadderTab(LadderSystem::MatchType::None);

			WCHAR wszString[256];
			swprintf_s( wszString, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 126174 ), static_cast<int>(CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::PvPLadder_LimitLevel)) );
			GetInterface().MessageBox(wszString);
		}

#ifdef PRE_ADD_DWC
		if( pData->iRet == ERROR_DWC_LADDER_MATCH_CLOSED ) // DWC시즌중 - 래더를 돌릴수 있는 시간이 종료됬을때.
		{
			CDnPVPDWCTabDlg* pDwcTablDlg = GetInterface().GetPVPDWCTablDlg();
			if(pDwcTablDlg)
				pDwcTablDlg->SetDWCRadioButton(CDnPVPDWCTabDlg::eDWCChannel::eChannel_Normal);

			// mid: 본선경기 시간이 종료되어 더이상 경기를 진행 할 수 없습니다.
			GetInterface().MessageBox(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 120266));
		}
#endif
		return; 
	}

#if defined(PRE_ADD_DWC)
	// DWC전용 탭 불러오기
	if(GetDWCTask().IsDWCChar())
	{
		if(GetInterface().GetPVPDWCTablDlg())
			GetInterface().GetPVPDWCTablDlg()->SetDWCRadioButton(pData->MatchType);
	}
	else
	{
		if(GetInterface().GetPVPLadderTabDlg())
			GetInterface().GetPVPLadderTabDlg()->SelectLadderTab(pData->MatchType);
	}
#else
	if(GetInterface().GetPVPLadderTabDlg())
		GetInterface().GetPVPLadderTabDlg()->SelectLadderTab(pData->MatchType);
#endif // PRE_ADD_DWC
	
	if(GetInterface().GetPvPRoomListDlg() && GetInterface().GetPvPRoomListDlg()->IsShow())
		GetInterface().GetPvPRoomListDlg()->Show(false);

	if(GetInterface().GetPvPLadderSystemDlg() && !GetInterface().GetPvPLadderSystemDlg()->IsShow())
	{
		GetInterface().GetPvPLadderSystemDlg()->SetLadderType(pData->MatchType);
		GetInterface().GetPvPLadderSystemDlg()->Show(true);
		if(pData->MatchType == LadderSystem::MatchType::_1vs1)
			GetInterface().GetPvPLadderSystemDlg()->ShowLadderListBox(true,pData->MatchType);
		else if(pData->MatchType != LadderSystem::MatchType::None)
			GetInterface().GetPvPLadderSystemDlg()->ShowLadderListBox(false,pData->MatchType);
	}
}

void CVillageClientSession::OnRecvPvPLeaveLadderChannel(LadderSystem::SC_LEAVE_CHANNEL *pData)
{
	if(pData->iRet != 0)
		return;

	if(GetInterface().GetPvPRoomListDlg() && !GetInterface().GetPvPRoomListDlg()->IsShow())
		GetInterface().GetPvPRoomListDlg()->Show(true);

	if(GetInterface().GetPvPLadderSystemDlg() && GetInterface().GetPvPLadderSystemDlg()->IsShow())
		GetInterface().GetPvPLadderSystemDlg()->Show(false);

#if defined(PRE_ADD_DWC)
	if(GetDWCTask().IsDWCChar() == false)
	{
		if(GetInterface().GetPVPLadderTabDlg())
			GetInterface().GetPVPLadderTabDlg()->SelectLadderTab(LadderSystem::MatchType::None);
	}
#else	// else PRE_ADD_DWC
	if(GetInterface().GetPVPLadderTabDlg())
		GetInterface().GetPVPLadderTabDlg()->SelectLadderTab(LadderSystem::MatchType::None);
#endif  // PRE_ADD_DWC
	
}

void CVillageClientSession::OnRecvPVPNotifyLeader(LadderSystem::SC_NOTIFY_LEADER *pData)
{
	LadderSystem::SC_NOTIFY_LEADER TempData;
	memset(&TempData,0,sizeof(TempData));
	wsprintf(TempData.wszLeaderName , pData->wszLeaderName);

	if(GetInterface().GetPvPLadderSystemDlg() && GetInterface().GetPvPLadderSystemDlg()->IsShow())
		GetInterface().GetPvPLadderSystemDlg()->GetLadderTeamGameDlg()->ShowMasterMarkByName(TempData.wszLeaderName);

	if(GetInterface().GetPvPLadderSystemDlg() && GetInterface().GetPvPLadderSystemDlg()->IsShow())
		GetInterface().GetPvPLadderSystemDlg()->SetRoomLeaderName(TempData.wszLeaderName);

} // 방장 설정

void CVillageClientSession::OnRecvPVPLadderState(LadderSystem::SC_NOTIFY_ROOMSTATE *pData)
{
	if(GetInterface().GetPvPLadderSystemDlg() && GetInterface().GetPvPLadderSystemDlg()->IsShow())
		GetInterface().GetPvPLadderSystemDlg()->SetPVPLadderState(pData->State);

}

void CVillageClientSession::OnRecvPVPMatchingResult(LadderSystem::SC_LADDER_MATCHING *pData)
{
#ifdef PRE_ADD_DWC
	ASSERT(pData);
	if(pData->iRet == ERROR_DWC_LADDER_MATCH_CLOSED)
	{
		// mid: 본선경기 시간이 종료되어 더이상 경기를 진행 할 수 없습니다.
		GetInterface().MessageBox(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 120266));

		SendPvPLeaveChannel();
	}
#else
	if(pData->iRet !=0)
		return; // 에러메세지 오면 값넣어줍시다.
#endif
}

void CVillageClientSession::OnRecvPVPLadderGameMode(LadderSystem::SC_NOTIFY_GAMEMODE_TABLEID *pData)
{
	CDnBridgeTask *pTask = (CDnBridgeTask *)CTaskManager::GetInstance().GetTask( "BridgeTask" );
	if(pTask)
	{
		DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TPVPGAMEMODE );
		if ( !pSox ) 
		{
			ErrorLog("CDnPVPGameRoomDlg::RoomSetting:: gamemode table Not found!! ");
			return;
		}

		int nGameMode =  pData->iTableID;
		int nGameWinCondition = 0;
		int nGamePlayTime = 0;
		int nMaxPlayer = 0;
		
		nGameWinCondition = pSox->GetFieldFromLablePtr( pData->iTableID , "WinCondition_1" )->GetInteger();
		nGamePlayTime = pSox->GetFieldFromLablePtr( pData->iTableID , "PlayTime_1" )->GetInteger();
		nMaxPlayer = pSox->GetFieldFromLablePtr( pData->iTableID , "NumOfPlayers_Max" )->GetInteger();
		
#ifdef PRE_MOD_CONSTANT_TEXT_CONVERT_TO_TABLE_TEXT
		pTask->SetPVPGameStatus( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 120219) ,nGameWinCondition,nGameMode,nGamePlayTime,nMaxPlayer,false, false);
#else
		pTask->SetPVPGameStatus(L"Ladder",nGameWinCondition,nGameMode,nGamePlayTime,nMaxPlayer,false, false);
#endif 
	}
}

void CVillageClientSession::OnRecvPVPLadderRoomList(LadderSystem::SC_PLAYING_ROOMLIST *pData)
{
	LadderSystem::SC_PLAYING_ROOMLIST TempData;
	memset(&TempData,0,sizeof(TempData));
	
	TempData.iRet = pData->iRet;
	TempData.unMaxPage = pData->unMaxPage;
	TempData.unNameCount = pData->unNameCount;

	for(int i=0; i<pData->unNameCount ; i++)
		wsprintf( TempData.wszCharName[i] , pData->wszCharName[i]);

	int nSize = ( LadderSystem::Common::eCode::RoomListPerPage * LadderSystem::MatchType::MaxMatchType * 2 );
	for(int i = 0 ; i < nSize ; ++i)
		TempData.cJob[i] = pData->cJob[i];

	if(pData->iRet != 0)
	{
		if(GetInterface().GetPvPLadderSystemDlg() && GetInterface().GetPvPLadderSystemDlg()->IsShow())
		{
			GetInterface().GetPvPLadderSystemDlg()->ResetRoomList(); 
			GetInterface().GetPvPLadderSystemDlg()->ResetRoomPage();
		}
		return; // 페이지 오류 발생
	}

#ifdef PRE_ADD_COLOSSEUM_BEGINNER
	// TODO: 초보채널, 일반채널 나눠서 표시..
#endif // #ifdef PRE_ADD_COLOSSEUM_BEGINNER

	if(GetInterface().GetPvPLadderSystemDlg() && GetInterface().GetPvPLadderSystemDlg()->IsShow())
		GetInterface().GetPvPLadderSystemDlg()->SetRoomList(&TempData);
}

void CVillageClientSession::OnRecvPVPLadderObserverMsg(LadderSystem::SC_OBSERVER *pData)
{
	if(pData->iRet != 0)
	{
		return; // 오류발생
	}
}

void CVillageClientSession::OnRecvPVPLadderScoreInfo(LadderSystem::SC_SCOREINFO *pData) // 빌리지 / PVP 에서 공용으로 쓰기때문에 PartyTask에서 가지고있도록 설정, 기존 PVP전적도 파티테스크에서 가지고있었음.
{	
	if( CDnPartyTask::IsActive() ) {
		
		CDnPartyTask::GetInstance().SetPVPLadderInfo(pData);
	
		if(CDnInterface::IsActive() && GetInterface().IsPVPLobby())
		{
			if(GetInterface().GetPvPLadderSystemDlg() && GetInterface().GetPvPLadderSystemDlg()->IsShow())
				GetInterface().GetPvPLadderSystemDlg()->SetLadderScoreInfo(pData);
		}
	}
	else
	{
		ErrorLog("CVillageClientSession OnRecvPVPLadderScoreInfo : PartyTask Not found");
		return;
	}
}

void CVillageClientSession::OnRecvPVPLadderJobScoreInfo(LadderSystem::SC_SCOREINFO_BYJOB *pData) // 이경우는 PVPLAdder에서만 필요한정보
{
	if(pData->iRet != 0)
		return;

	if(GetInterface().GetPvPLadderSystemDlg() && GetInterface().GetPvPLadderSystemDlg()->IsShow())
		GetInterface().GetPvPLadderSystemDlg()->SetLadderJobScoreInfo(pData);
}

void CVillageClientSession::OnRecvPVPLadderTimeLeft(LadderSystem::SC_MATCHING_AVGSEC *pData)
{
	if(GetInterface().GetPvPLadderSystemDlg() && GetInterface().GetPvPLadderSystemDlg()->IsShow())
		GetInterface().GetPvPLadderSystemDlg()->SetLadderLeftTime(pData);
}

void CVillageClientSession::OnRecvPVPLadderPointRefresh(LadderSystem::SC_LADDERPOINT_REFRESH *pData)
{
	if(CDnPartyTask::IsActive())
	{
		LadderSystem::SC_SCOREINFO TempData = *CDnPartyTask::GetInstance().GetPVPLadderInfo();
		TempData.Data.iPvPLadderPoint = pData->iPoint;
		CDnPartyTask::GetInstance().SetPVPLadderInfo(&TempData);
		GetInterface().RefreshLadderInfo();
	}
}

void CVillageClientSession::OnRecvPVPLadderJoinUser(LadderSystem::SC_NOTIFY_JOINUSER *pData)
{
	LadderSystem::SC_NOTIFY_JOINUSER TempData;
	memset(&TempData,0,sizeof(TempData));

	TempData.sUserInfo.biCharDBID = pData->sUserInfo.biCharDBID;
	TempData.sUserInfo.cJob = pData->sUserInfo.cJob;
	TempData.sUserInfo.iGradePoint = pData->sUserInfo.iGradePoint;
	wsprintf ( TempData.sUserInfo.wszCharName , pData->sUserInfo.wszCharName);

	if(GetInterface().GetPvPLadderSystemDlg() && GetInterface().GetPvPLadderSystemDlg()->IsShow())
	{
		GetInterface().GetPvPLadderSystemDlg()->GetLadderTeamGameDlg()->InsterLadderUser(&TempData);

		if(GetInterface().GetPvPLadderSystemDlg()->GetLadderTeamGameDlg()->IsReady())
			GetInterface().GetPvPLadderSystemDlg()->EnableSearchButton(true,true);
	}
}

void CVillageClientSession::OnRecvPVPLadderLeaveUser(LadderSystem::SC_NOTIFY_LEAVEUSER *pData)
{
	if(GetInterface().GetPvPLadderSystemDlg() && GetInterface().GetPvPLadderSystemDlg()->IsShow())
	{
		GetInterface().GetPvPLadderSystemDlg()->GetLadderTeamGameDlg()->RemoveUserByDBID(pData->biCharDBID);

		if(!GetInterface().GetPvPLadderSystemDlg()->GetLadderTeamGameDlg()->IsReady() )
			GetInterface().GetPvPLadderSystemDlg()->EnableSearchButton(GetInterface().GetPvPLadderSystemDlg()->GetLadderTeamGameDlg()->IsMasterUser(),false);
	}
}

void CVillageClientSession::OnRecvPVPLadderRefreshUserInfo(LadderSystem::SC_REFRESH_USERINFO *pData)
{
	LadderSystem::SC_REFRESH_USERINFO TempData;
	memset(&TempData,0,sizeof(TempData));
	TempData.cCount = pData->cCount;

	for(int i=0;i<pData->cCount;i++)
	{
		TempData.sUserInfoArr[i].biCharDBID = pData->sUserInfoArr[i].biCharDBID;
		TempData.sUserInfoArr[i].cJob = pData->sUserInfoArr[i].cJob;
		TempData.sUserInfoArr[i].iGradePoint = pData->sUserInfoArr[i].iGradePoint;
		wsprintf ( TempData.sUserInfoArr[i].wszCharName , pData->sUserInfoArr[i].wszCharName);
	}

	if(GetInterface().GetPvPLadderSystemDlg() && GetInterface().GetPvPLadderSystemDlg()->IsShow())
		GetInterface().GetPvPLadderSystemDlg()->GetLadderTeamGameDlg()->RefrehsLadderUser(&TempData);
}

void CVillageClientSession::OnRecvPVPLadderInviteUser(LadderSystem::SC_INVITE *pData)
{
	if(pData->iRet != 0)
	{
		switch(pData->iRet)
		{
			case ERROR_LADDERSYSTEM_INVITE_DENY:
				GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 126258 ), MB_OK ); // 초대거절
				break;
			case ERROR_LADDERSYSTEM_CANT_INVITE_LEVELLIMIT:
				GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 126259 ), MB_OK ); // 초대거절
				break;
			case ERROR_GENERIC_USER_NOT_FOUND:
				GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3601 ), MB_OK ); // 존재하지 않는 유저일때
				break;
			case ERROR_LADDERSYSTEM_CANT_RECVINVITESTATE:
				GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 126254 ), MB_OK ); // 초대거절
				break;
			default:
				GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 126258 ), MB_OK ); // 초대거절
				break;
		}
	}
}

void CVillageClientSession::OnRecvPVPLadderInviteConfirmReq(LadderSystem::SC_INVITE_CONFIRM_REQ *pData)
{
	LadderSystem::SC_INVITE_CONFIRM_REQ TempData;
	memset(&TempData,0,sizeof(TempData));

	TempData.iAvgGradePoint = pData->iAvgGradePoint;
	TempData.iCurUserCount = pData->iCurUserCount;
	TempData.MatchType = pData->MatchType;
	wsprintf(TempData.wszCharName , pData->wszCharName);

	if(GetInterface().GetPvPLadderSystemDlg())
	{
		GetInterface().GetPvPRoomListDlg()->ShowLadderInviteConfirmDlg(&TempData);
	}
}

void CVillageClientSession::OnRecvPVPLadderInviteConfirm(LadderSystem::SC_INVITE_CONFIRM *pData)
{
	if(pData->iRet != 0)
	{
		switch(pData->iRet)
		{
		case ERROR_LADDERSYSTEM_INVITE_DENY:
			GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 126258 ), MB_OK ); // 초대거절
			break;
		case ERROR_LADDERSYSTEM_CANT_INVITE_LEVELLIMIT:
			GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 126259 ), MB_OK ); // 레벨제한.
			break;
		case ERROR_LADDERSYSTEM_ROOM_FULLUSER:
			GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 126255 ), MB_OK ); // 빈자리없음.
			break;
		default:
			GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 126258 ), MB_OK ); // 초대거절
			break;
		}

	}
}

void CVillageClientSession::OnRecvPVPLadderKickOut(LadderSystem::SC_KICKOUT *pData)
{
	if(pData->iRet != 0)
	{
		switch(pData->iRet)
		{
		case ERROR_LADDERSYSTEM_KICKOUT:
			{
				GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 126261 ), MB_OK ); // 강퇴 탕함 메세지.

#ifdef PRE_ADD_DWC
				if(GetDWCTask().IsDWCChar())
				{
					// 일반탭으로 이동
					if( GetInterface().GetPVPDWCTablDlg() )
						GetInterface().GetPVPDWCTablDlg()->SetDWCRadioButton(CDnPVPDWCTabDlg::eDWCChannel::eChannel_Normal);
				}
#endif
			}
			break;
		case ERROR_LADDERSYSTEM_CANT_KICKOUT_ROOMSTATE:
			GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 126260 ), MB_OK ); // 지금은 내보낼수가 없는 상태.
			break;
		default:
			GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 126260 ), MB_OK ); // 강퇴 탕함 메세지.
		}
	}
}

void CVillageClientSession::OnRecvPVPGhoulScoreInfo(SCPVP_GHOUL_SCORES *pData)
{
	if( CDnPartyTask::IsActive() ) 
	{
		TPvPGhoulScores Temp;
		memset(&Temp,0,sizeof(Temp));

		for(int i=0;i<GhoulMode::PointType::Max;i++)
			Temp.nPoint[i] = pData->GhoulScores.nPoint[i];

		CDnPartyTask::GetInstance().SetPvPGhoulInfo(&Temp);
	}
}

#ifdef PRE_ADD_PVP_VILLAGE_ACCESS
void CVillageClientSession::OnRecvOpenPVPVillageAcess(SCPvPListOpenUI *pData)
{
	if(pData->nResult == ERROR_NONE)
		GetInterface().ShowPVPVillageAccessDlg(true);
	else
	{
		std::wstring str = FormatW(GetEtUIXML().GetUIString( CEtUIXML::idCategory1 , 120153 ) ,pData->nLimitLevel );
		GetInterface().ShowCaptionDialog( CDnInterface::typeCaption2 ,  str.c_str() , textcolor::YELLOW );
	}

}
#endif

#ifdef PRE_ADD_PVP_TOURNAMENT
void CVillageClientSession::OnRecvPvPTournamentSwapIndex(SCPvPSwapTournamentIndex* pData)
{
	if (pData->nRetCode == ERROR_NONE)
	{
		CDnBaseRoomDlg* pDlg = GetInterface().GetGameRoomDlg();
		if (pDlg == NULL)
		{
			_ASSERT(0);
			return;
		}

		pDlg->OnSwapPosition(pData->cSourceIndex, pData->cDestIndex);
	}
	else
	{
		GetInterface().ServerMessageBox(pData->nRetCode);
	}
}
#endif

void CVillageClientSession::OnRecvJoinRoom( SCPVP_JOINROOM * pData )
{
	FUNC_LOG();

	if( pData == NULL )
		return;

	/*
	UINT	uiMapIndex;									// 맵인덱스
	BYTE	cGameMode;									// PvPCommon::GameMode
	UINT	uiWinCondition;
	UINT	uiPlayTimeSec;
	BYTE	cBreakIntoFlag;								// 난입여부
	BYTE	cMaxUserCount;								// 최대인원
	BYTE	cRoomState;									// PvPCommon::RoomState
	*/

	int In=0,UIErrorString = 0;
	bool isMaster = false;	

	if( pData->nRetCode != ERROR_NONE )
	{
		CDnBaseRoomDlg::ErrorHandler( pData->nRetCode );

#ifdef PRE_MOD_PVPOBSERVER
		GetInterface().SetPVPRoomPassword( std::wstring(L"") );
		GetInterface().ResetPvpObserverJoinFlag();
#endif // PRE_MOD_PVPOBSERVER

		return;
	}
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TPVPGAMEMODE );
	if ( !pSox ) 
	{
		ErrorLog("CDnPVPGameRoomDlg::OnRecvJoinRoom:: gamemode table Not found!! ");
		return;
	}

	int nGameMode = pSox->GetFieldFromLablePtr( pData->RoomInfo.uiGameModeTableID , "GamemodeID" )->GetInteger();

	if( PvPCommon::GameMode::PvP_GuildWar == nGameMode )
		GetInterface().SelectPVPRoom( CDnInterface::eGuildWar_Room );
	#ifdef PRE_ADD_PVP_TOURNAMENT
	else if (nGameMode == PvPCommon::GameMode::PvP_Tournament)
		GetInterface().SelectPVPRoom(CDnInterface::eTournament_Room);
	#endif
	else
		GetInterface().SelectPVPRoom( CDnInterface::ePVP_Room );
#ifdef PRE_WORLDCOMBINE_PVP
	WCHAR wszRoomName[PvPCommon::TxtMax::RoomName];
	WCHAR wszPlayerName[NAMELENMAX];

#ifdef PRE_ADD_PVP_TOURNAMENT
	ZeroMemory(wszRoomName, sizeof(wszRoomName));
#else
	SecureZeroMemory(wszRoomName,sizeof(wszRoomName));
#endif
	memcpy(wszRoomName,pData->RoomInfo.wszBuffer,pData->RoomInfo.cRoomNameLen * sizeof(WCHAR));

	if( pData->RoomInfo.nWorldPvPRoomType )
	{
		if( GetInterface().GetPvPRoomListDlg() )
			GetInterface().GetPvPRoomListDlg()->SetFirstRoom( true );

		if( PvPCommon::GameMode::PvP_Tournament == nGameMode )
		{
			CDnPVPTournamentDataMgr& pvpTournamentDataMgr = CDnBridgeTask::GetInstance().GetPVPTournamentDataMgr();
			pvpTournamentDataMgr.Reset();
			pvpTournamentDataMgr.SetRoomInfo( pData->RoomInfo );
		}

		WCHAR wszIndex[256];
		wsprintf( wszIndex,L"%d. ", pData->RoomInfo.uiIndex );
		std::wstring wszFullRoomName = wszIndex;
		wszFullRoomName.append( wszRoomName , wcslen(wszRoomName) );
		bool bShowHp = pData->RoomInfo.unRoomOptionBit & PvPCommon::RoomOption::ShowHP ? true:false;
		bool bRevision = pData->RoomInfo.unRoomOptionBit & PvPCommon::RoomOption::NoRegulation ? true:false;
		CDnBridgeTask::GetInstance().SetPVPGameStatus( wszFullRoomName, pData->RoomInfo.uiWinCondition, pData->RoomInfo.uiGameModeTableID, 
														pData->RoomInfo.uiPlayTimeSec, pData->RoomInfo.cMaxUserCount , bShowHp, bRevision );
	}
	else
	{
		GetInterface().SetFirstRoom( true );
	}
#else // PRE_WORLDCOMBINE_PVP
	GetInterface().SetFirstRoom( true );

	WCHAR wszRoomName[PvPCommon::TxtMax::RoomName];
	WCHAR wszPlayerName[NAMELENMAX];

#ifdef PRE_ADD_PVP_TOURNAMENT
	ZeroMemory(wszRoomName, sizeof(wszRoomName));
#else
	SecureZeroMemory(wszRoomName,sizeof(wszRoomName));
#endif
	memcpy(wszRoomName,pData->RoomInfo.wszBuffer,pData->RoomInfo.cRoomNameLen * sizeof(WCHAR));
#endif // PRE_WORLDCOMBINE_PVP

	if( GetInterface().GetGameRoomDlg())
		GetInterface().GetGameRoomDlg()->RoomSetting( &pData->RoomInfo, false );

	GetInterface().GetPVPLobbyChatTabDlg()->RemoveObserver(0,true);

	for(int i =0 ;i < pData->cUserCount ;i++)
	{
		SecureZeroMemory(wszPlayerName,sizeof(wszPlayerName));
		memcpy(wszPlayerName,pData->UserInfoList.wszBuffer +In , pData->UserInfoList.cCharacterNameLen[i] * sizeof(WCHAR));

		In += pData->UserInfoList.cCharacterNameLen[i];
		isMaster = pData->UserInfoList.uiUserState[i]&PvPCommon::UserState::Captain?true:false;
		int nTeamNumber = 0;

		switch(pData->UserInfoList.usTeam[i])
		{
		case PvPCommon::Team::A:
				nTeamNumber =  PvPCommon::Team::A;
			break;
		case PvPCommon::Team::B:
				nTeamNumber =  PvPCommon::Team::B;
			break;
		case PvPCommon::Team::Observer:
				nTeamNumber = PvPCommon::Team::Observer; // 1000 = A , 1001 = B , 1003 = Observer
			break;
		}

		if(nTeamNumber != PvPCommon::Team::Observer)
		{
			if( GetInterface().GetGameRoomDlg())
				GetInterface().GetGameRoomDlg()->InsertPlayer( nTeamNumber,
				pData->UserInfoList.uiUserState[i],
				pData->UserInfoList.uiSessionID[i]  , 
				pData->UserInfoList.iJobID[i] , 
				pData->UserInfoList.cLevel[i] ,
				pData->UserInfoList.cPvPLevel[i],
				wszPlayerName
				, pData->UserInfoList.cTeamIndex[i]
			);	
		}
		else if(nTeamNumber == PvPCommon::Team::Observer)
		{
			GetInterface().GetPVPLobbyChatTabDlg()->InsertObserver(nTeamNumber,
				pData->UserInfoList.uiUserState[i],
				pData->UserInfoList.uiSessionID[i]  , 
				pData->UserInfoList.iJobID[i] , 
				pData->UserInfoList.cLevel[i] ,
				pData->UserInfoList.cPvPLevel[i],
				wszPlayerName
				);

#ifdef PRE_MOD_PVPOBSERVER
			GetInterface().GetGameRoomDlg()->InsertObserver( pData->UserInfoList.uiUserState[i], pData->UserInfoList.uiSessionID[i] );
#endif // PRE_MOD_PVPOBSERVER
		}
	}

	if( GetInterface().GetGameRoomDlg())
		GetInterface().GetGameRoomDlg()->SetRoomState( pData->RoomInfo.cRoomState );

#ifdef PRE_WORLDCOMBINE_PVP
	if( pData->RoomInfo.nWorldPvPRoomType )
	{
		// 현재 선택된 초보채널, 래더를 저장.
		CDnBridgeTask *pBridgeTask = (CDnBridgeTask *)CTaskManager::GetInstance().GetTask( "BridgeTask" );
		CDnPVPLadderTabDlg* pPVPLadderTabDlg = GetInterface().GetPVPLadderTabDlg();
		if( pBridgeTask && pPVPLadderTabDlg )
		{	
			pBridgeTask->SetSelectedPVPGradeChannelOrLadder( pPVPLadderTabDlg->GetSelectedGradeChannel(), pPVPLadderTabDlg->GetSelectedChannel() );
		}
	}
	else
	{
		GetInterface().ChageLobbyState( CDnInterface::emLobbyState::InGameRoom );
	}
#else // PRE_WORLDCOMBINE_PVP
	GetInterface().ChageLobbyState( CDnInterface::emLobbyState::InGameRoom );
#endif // PRE_WORLDCOMBINE_PVP

#ifdef PRE_ADD_COLOSSEUM_BEGINNER
	// 따로 업데이트 해주는 곳이 없어서 여기서 업데이트. 
	// 일정 등급 이상이 되면 강제로 일반 채널로 옮겨주어야 하기 때문..
	CDnPartyTask* pPartyTask = (CDnPartyTask *)CTaskManager::GetInstance().GetTask( "PartyTask" );
	if( pPartyTask )
	{
		// PartyTask 로 부터 얻어오는 pvp 등급 정보는 나중에 villagetask의 sc_enter 메시지로 업데이트 되지만
		// pvp 채널 나누는 정보는 PartyTask 에 있는 정보에 기반하므로 여기서 업데이트를 해준다.
		TPvPGroup *pPvPInfo = pPartyTask->GetPvPInfo();
		char cOriginalLevel = pPvPInfo->cLevel;

		char cPVPLevelAfterGame = 0;
		for( int i = 0; i < pData->cUserCount; ++i )
		{
			if( CDnBridgeTask::GetInstance().GetSessionID() == pData->UserInfoList.uiSessionID[ i ] )
			{
				cPVPLevelAfterGame = pData->UserInfoList.cPvPLevel[ i ];
				break;
			}
		}

#ifdef PRE_WORLDCOMBINE_PVP
		if( !pData->RoomInfo.nWorldPvPRoomType )
			pPvPInfo->cLevel = cPVPLevelAfterGame;
#else // PRE_WORLDCOMBINE_PVP
		pPvPInfo->cLevel = cPVPLevelAfterGame;
#endif // PRE_WORLDCOMBINE_PVP

		// 원래 레벨이 콜로세움 초보자 채널 이전 레벨에서 콜로세움 초보자 채널레벨 이상이 되었을 때 
		// 룸에서 나갔을 때 강제로 채널을 바꿔주는 처리를 하는 플래그를 켜둔다.
		int iColosseumBeginnerRank = (int)CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::Colosseum_Limit_Rank); 
		if( cOriginalLevel < iColosseumBeginnerRank &&
			iColosseumBeginnerRank <= pPvPInfo->cLevel )
		{
			m_bPVPChannelGradeUpped = true;
		}
	}
#endif // #ifdef PRE_ADD_COLOSSEUM_BEGINNER
}

void CVillageClientSession::OnRecvLeaveRoom( SCPVP_LEAVEROOM * pData )
{
	FUNC_LOG();
	
	if( pData->nRetCode != ERROR_NONE && pData->nRetCode != ERROR_PVP_FORCELEAVEROOM_MASTERSERVER_DOWN )
	{
		ErrorLog("CVillageClientSession::OnRecvLeaveRoom ErrorCode %d ",pData->nRetCode);
		return;
	}

	if( pData->uiLeaveUserSessionID == CDnBridgeTask::GetInstance().GetSessionID())
	{
		GetInterface().ChageLobbyState( CDnInterface::emLobbyState::InLobby );

		GetInterface().GetPVPLobbyChatTabDlg()->RemoveObserver(pData->uiLeaveUserSessionID,true); 

		if( pData->nRetCode == ERROR_PVP_FORCELEAVEROOM_MASTERSERVER_DOWN )
		{
			if( GetInterface().GetGameRoomDlg())
				CDnBaseRoomDlg::ErrorHandler( ERROR_GENERIC_MASTERCON_NOT_FOUND );
		}

		int nUIString = 0;
		if( pData->eType == PvPCommon::LeaveType::Ban ) nUIString = 121059;
		else if( pData->eType == PvPCommon::LeaveType::PermanenceBan ) nUIString = 121060;
		if( nUIString )
			GetInterface().AddChatMessage(CHATTYPE_SYSTEM, L"", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nUIString ), false );

#ifdef PRE_ADD_COLOSSEUM_BEGINNER
		CDnPVPLadderTabDlg* pPVPLadderTabDlg = GetInterface().GetPVPLadderTabDlg();
		if( pPVPLadderTabDlg )
		{
			if( m_bPVPChannelGradeUpped )
			{
				pPVPLadderTabDlg->UpdateAndSelectTabProperly();

				CDnPartyTask* pPartyTask = (CDnPartyTask *)CTaskManager::GetInstance().GetTask( "PartyTask" );
				if( pPartyTask )
				{
					TPvPGroup *pPvPInfo = pPartyTask->GetPvPInfo();
					DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TPVPRANK );
					int iPVPGradeUIString = 0;
					if ( pSox && pSox->IsExistItem( pPvPInfo->cLevel ) )
					{
						iPVPGradeUIString = pSox->GetFieldFromLablePtr( pPvPInfo->cLevel, "PvPRankUIString" )->GetInteger();

						WCHAR wszString[ 256 ] = { 0 };
						wstring strGradeName = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, iPVPGradeUIString );
						swprintf_s( wszString, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 120160 ), strGradeName.c_str() );
						GetInterface().MessageBox( wszString );
					}
				}

				m_bPVPChannelGradeUpped = false;
			}
			else
			{
				pPVPLadderTabDlg->UpdatePvPChannelTab();
			}
		}
#endif // #ifdef PRE_ADD_COLOSSEUM_BEGINNER
	}
	else
	{
		if( GetInterface().GetGameRoomDlg())
			GetInterface().GetGameRoomDlg()->RemovePlayer( pData->uiLeaveUserSessionID, pData->eType );

		GetInterface().GetPVPLobbyChatTabDlg()->RemoveObserver(pData->uiLeaveUserSessionID,false);
		GetInterface().GetGameRoomDlg()->EnableKickButton(false); 
		GetInterface().GetGameRoomDlg()->UpdateSlot( false );     
	}

}

void CVillageClientSession::OnRecvCreateRoom( SCPVP_CREATEROOM * pData )
{
	FUNC_LOG();

	if( pData->nRetCode != ERROR_NONE )
	{
		ErrorLog("CVillageClientSession::OnRecvCreateRoom %d ",pData->nRetCode);

		if( pData->nRetCode == ERROR_PVP_CREATEROOM_FAILED )
		{
			CDnBaseRoomDlg::ErrorHandler( ERROR_PVP_CREATEROOM_FAILED );
		}
		else
		{
			ErrorLog("CVillageClientSession::OnRecvModifyRoom Unknown Error Msg %d ",pData->nRetCode);
			CDnBaseRoomDlg::ErrorHandler( ERROR_GENERIC_MASTERCON_NOT_FOUND );
		}

		return;
	}

#ifdef PRE_ADD_PVP_TOURNAMENT
#else
	WCHAR wszRoomName[PvPCommon::TxtMax::RoomName];
	SecureZeroMemory(wszRoomName,sizeof(wszRoomName));
	memcpy(wszRoomName,pData->sCSPVP_CREATEROOM.wszBuf,pData->sCSPVP_CREATEROOM.cRoomNameLen * sizeof(WCHAR));
#endif
	GetInterface().ChageLobbyState( CDnInterface::emLobbyState::InGameRoom );
}

void CVillageClientSession::OnRecvJoinUser( SCPVP_JOINUSER * pData )
{
	FUNC_LOG();
	WCHAR   wszPlayerName[NAMELENMAX];
	SecureZeroMemory(wszPlayerName,sizeof(wszPlayerName));
	memcpy(wszPlayerName,pData->UserInfo.wszCharacterName , pData->UserInfo.cCharacterNameLen * sizeof(WCHAR));

	int nTeamNumber = 0;

	switch(pData->UserInfo.usTeam)
	{
	case PvPCommon::Team::A:
		nTeamNumber =  PvPCommon::Team::A;
		break;
	case PvPCommon::Team::B:
		nTeamNumber =  PvPCommon::Team::B;
		break;
	case PvPCommon::Team::Observer:
		nTeamNumber = PvPCommon::Team::Observer; // 1000 = A , 1001 = B , 1003 = Observer
		break;
	}

	if(nTeamNumber != PvPCommon::Team::Observer)
	{
		if( GetInterface().GetGameRoomDlg())
			GetInterface().GetGameRoomDlg()->InsertPlayer( nTeamNumber,
			pData->UserInfo.uiUserState,
			pData->UserInfo.uiSessionID, 
			pData->UserInfo.iJobID , 
			pData->UserInfo.cLevel ,
			pData->UserInfo.cPvPLevel,
			wszPlayerName
			, pData->UserInfo.cSlotIndex
			);	
	}
	else if(nTeamNumber == PvPCommon::Team::Observer)
	{
		GetInterface().GetPVPLobbyChatTabDlg()->InsertObserver(
			nTeamNumber,
			pData->UserInfo.uiUserState,
			pData->UserInfo.uiSessionID, 
			pData->UserInfo.iJobID , 
			pData->UserInfo.cLevel ,
			pData->UserInfo.cPvPLevel,
			wszPlayerName
			);
	}
}

void CVillageClientSession::OnRecvUserState( SCPVP_USERSTATE * pData )
{
	FUNC_LOG();
	/*
	Master = 0,
	Ready = 1,
	Playing 
	*/

	bool isMe = false;
	
	if( CDnBridgeTask::GetInstance().GetSessionID() ==  pData->uiSessionID )
		isMe = true;

	if( GetInterface().GetGameRoomDlg())
		GetInterface().GetGameRoomDlg()->SetUserState( pData->uiSessionID , pData->uiUserState , true);

	if( GetInterface().GetPVPLobbyChatTabDlg() )
		GetInterface().GetPVPLobbyChatTabDlg()->SetObserverSlotUserState( pData->uiSessionID , pData->uiUserState  );
}

void CVillageClientSession::OnRecvChangeTeam( SCPVP_CHANGETEAM * pData )
{
	FUNC_LOG();

	if( pData->nRetCode != ERROR_NONE )
	{
		ErrorLog("CVillageClientSession::OnRecvChangeTeam %d ",pData->nRetCode);
		if( pData->nRetCode != ERROR_GENERIC_MASTERCON_NOT_FOUND )
		{
			CDnBaseRoomDlg::ErrorHandler( pData->nRetCode );
		}
		return;
	}	

	if( pData->usTeam == PvPCommon::Team::A || pData->usTeam == PvPCommon::Team::B) //
	{
		if( GetInterface().GetGameRoomDlg())
		{
			GetInterface().GetGameRoomDlg()->ChangeTeam( pData->uiSessionID, pData->usTeam, pData->cTeamSlotIndex );
		}

	}
	else if(pData->usTeam == PvPCommon::Team::Observer) 
	{
		if( GetInterface().GetGameRoomDlg())
			GetInterface().GetGameRoomDlg()->ChangeTeam( pData->uiSessionID  , PvPCommon::Team::Observer );
	}
}


void CVillageClientSession::OnRecvStart( SCPVP_START * pData )
{
	FUNC_LOG();

	if( pData->nRetCode == ERROR_NONE )
		return;

	CDnBaseRoomDlg::ErrorHandler( pData->nRetCode );

	return;
}

void CVillageClientSession:: OnRecvStart( SCPVP_ROOMSTATE * pData )
{
	FUNC_LOG();

	if( GetInterface().GetGameRoomDlg())
		GetInterface().GetGameRoomDlg()->SetRoomState( pData->uiRoomState );	
}

void CVillageClientSession::OnRecvModifyRoom( SCPVP_MODIFYROOM * pData )
{

	if( pData->nRetCode != ERROR_NONE )
	{
		ErrorLog("CVillageClientSession::OnRecvModifyRoom %d ",pData->nRetCode);
		if( pData->nRetCode == ERROR_PVP_MODIFYROOM_FAILED )
		{
			CDnBaseRoomDlg::ErrorHandler( ERROR_PVP_MODIFYROOM_FAILED );
		}
		else
		{
			ErrorLog("CVillageClientSession::OnRecvModifyRoom Unknown Error Msg %d ",pData->nRetCode);
			CDnBaseRoomDlg::ErrorHandler( ERROR_PVP_MODIFYROOM_FAILED );
		}

		return;
	}

	FUNC_LOG();

	byte cPassWordLen = 0;
	cPassWordLen = pData->sCSPVP_MODIFYROOM.sCSPVP_CREATEROOM.cRoomPWLen;

	if( GetInterface().GetGameRoomDlg()) {
		if( GetInterface().GetGameRoomDlg()->GetCurMapIndex() != pData->sCSPVP_MODIFYROOM.sCSPVP_CREATEROOM.uiMapIndex )
			GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 121068 ), false );

		PvPCommon::RoomInfo sRoomInfo;
		memset(&sRoomInfo,0,sizeof(PvPCommon::RoomInfo));

		sRoomInfo.nEventID = 0;
		sRoomInfo.unRoomOptionBit = pData->sCSPVP_MODIFYROOM.sCSPVP_CREATEROOM.unRoomOptionBit;
		sRoomInfo.cMaxLevel = pData->sCSPVP_MODIFYROOM.sCSPVP_CREATEROOM.cMaxLevel;
		sRoomInfo.cMaxUserCount = pData->sCSPVP_MODIFYROOM.sCSPVP_CREATEROOM.cMaxUser;
		sRoomInfo.cMinLevel = pData->sCSPVP_MODIFYROOM.sCSPVP_CREATEROOM.cMinLevel;
		sRoomInfo.cRoomNameLen = pData->sCSPVP_MODIFYROOM.sCSPVP_CREATEROOM.cRoomNameLen;
		sRoomInfo.cRoomState = cPassWordLen?1:0;
		sRoomInfo.uiIndex = 0;
		sRoomInfo.uiGameModeTableID = pData->sCSPVP_MODIFYROOM.sCSPVP_CREATEROOM.uiGameModeTableID;
		sRoomInfo.uiMapIndex = pData->sCSPVP_MODIFYROOM.sCSPVP_CREATEROOM.uiMapIndex;
		sRoomInfo.uiPlayTimeSec = pData->sCSPVP_MODIFYROOM.sCSPVP_CREATEROOM.uiSelectPlayTimeSec;
		sRoomInfo.uiWinCondition = pData->sCSPVP_MODIFYROOM.sCSPVP_CREATEROOM.uiSelectWinCondition;
#ifdef PRE_MOD_PVPOBSERVER
		sRoomInfo.bExtendObserver = pData->sCSPVP_MODIFYROOM.sCSPVP_CREATEROOM.bExtendObserver;
#endif // PRE_MOD_PVPOBSERVER
		memcpy(sRoomInfo.wszBuffer,pData->sCSPVP_MODIFYROOM.sCSPVP_CREATEROOM.wszBuf,pData->sCSPVP_MODIFYROOM.sCSPVP_CREATEROOM.cRoomNameLen * sizeof(WCHAR));

		GetInterface().GetGameRoomDlg()->RoomSetting( &sRoomInfo ,true);
	}
}

void CVillageClientSession::OnRecvPvPRoomInfo( char* pData )
{
	SCPVP_ROOMINFO* pPacket = reinterpret_cast<SCPVP_ROOMINFO*>(pData);

	DN_ASSERT( CDnBridgeTask::GetInstancePtr() != NULL, "CVillageClientSession::OnRecvPvPRoomInfo() CDnBridgeTask::GetInstancePtr() != NULL" );

	std::wstring wStrName = boost::io::str( boost::wformat( L"%d. observer room") % pPacket->uiIndex );
	CDnBridgeTask::GetInstance().SetPVPGameStatus( wStrName, pPacket->uiWinCondition, pPacket->uiGameModeTableID, pPacket->uiPlayTimeSec, pPacket->cMaxUser
		,false, false);

}

void CVillageClientSession::OnRecvPvPLevel( char* pData )
{
	SCPVP_LEVEL* pPacket = reinterpret_cast<SCPVP_LEVEL*>( pData );

	if( GetInterface().GetGameRoomDlg() )
		GetInterface().GetGameRoomDlg()->UpdatePvPLevel( pPacket );
}


void CVillageClientSession::GetMeritChannelText(std::wstring& result, int userLevel) const
{
	if (m_MeritInfo.nMeritType == MERIT_BONUS_NONE)
		return;

	if (userLevel >= m_MeritInfo.nMinLevel && userLevel <= m_MeritInfo.nMaxLevel)
	{
		if (m_MeritInfo.nMeritType == MERIT_BONUS_COMPLETEEXP)
		{
			result = FormatW(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1211 ), m_MeritInfo.nExtendValue); // UISTRING : 스테이지 클리어 보너스 경험치 : %d
			return;
		}
	}
}