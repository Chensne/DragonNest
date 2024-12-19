#include "StdAfx.h"
#include "GameClientSession.h"
#include "MATransAction.h"
#include "DnWorldProp.h"
#include "GameSendPacket.h"
#include "VillageSendPacket.h"
#include "PartySendPacket.h"
#include "DnPartyTask.h"
#include "DnMessageManager.h"
#include "DnLoadingTask.h"
#include "TaskManager.h"
#include "DnInterface.h"
#include "DnLocalPlayerActor.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

CGameClientSession::CGameClientSession() : CTaskListener(true)
{
	m_bSyncComplete = false;

	m_nStageRandomSeed = 0;
	m_nGateIndex = 0;
	m_cSeqLevel = 0;
}

CGameClientSession::~CGameClientSession()
{
}

void CGameClientSession::OnDisconnectUdp( bool bValidDisconnect )

{
	if( !bValidDisconnect )
		OutputDebug( "Disconnect UDP\n" );
}

void CGameClientSession::OnDisconnectTcp( bool bValidDisconnect )
{
	if( !bValidDisconnect )
		OutputDebug( "Disconnect TCP\n" );
}

void CGameClientSession::AddConnectUser( DnActorHandle hActor, TCHAR *szUserID )
{
	ScopeLock<CSyncLock> Lock( m_ConnectListLock );

	SConnectUserStruct Struct;
	Struct.hActor = hActor;

	Struct.szUserID = szUserID;
	m_VecConnectList.push_back( Struct );
}

DWORD CGameClientSession::GetConnectUserCount()
{
	return (DWORD)m_VecConnectList.size();
}

CGameClientSession::SConnectUserStruct *CGameClientSession::GetConnectUserData( DWORD dwIndex )
{
	ScopeLock<CSyncLock> Lock( m_ConnectListLock );

	if( m_VecConnectList.size() <= dwIndex )
		return NULL;

	return &m_VecConnectList[dwIndex];
}

void CGameClientSession::OnDispatchMessage( int nMainCmd, int nSubCmd, char *pData, int nSize )
{
	if( CDnLoadingTask::IsActive() ) {
		if( CDnLoadingTask::GetInstance().InsertLoadPacket( this, nMainCmd, nSubCmd, (void*)pData, nSize ) ) return;
	}

	bool bProcessDispatch = false;
	switch( nMainCmd ) {
		case SC_ACTOR: OnRecvActorMessage( nSubCmd, pData, nSize ), bProcessDispatch = true;  break;
		case SC_ACTORBUNDLE: OnRecvActorBundleMessage( nSubCmd, pData, nSize ), bProcessDispatch = true;  break;
		case SC_PROP: OnRecvPropMessage( nSubCmd, pData, nSize ), bProcessDispatch = true;  break;
		case SC_ROOM: OnRecvRoomMessage( nSubCmd, pData, nSize ), bProcessDispatch = true;  break;
		case SC_PARTY: OnRecvPartyMessage( nSubCmd, pData, nSize ), bProcessDispatch = true;  break;
		case SC_CHAR: OnRecvCharMessage( nSubCmd, pData, nSize ), bProcessDispatch = true;  break;
		//blondy
		case SC_PVP: OnRecvPVPMessage( nSubCmd, pData, nSize ), bProcessDispatch = true;  break;
		//blondy end
	}
	if( bProcessDispatch ) {
		WriteLog( 1, ", Info, CGameClientSession::OnDispatchMessage End : nMainCmd(%d), nSubCmd(%d), nSize(%d)\n", nMainCmd, nSubCmd, nSize );
	}

#if 0
	GetMessageManager().RemoveMessage( nMainCmd, nSubCmd );
#endif
}

void CGameClientSession::OnRecvPartyMessage( int nSubCmd, char *pData, int nSize )
{
	switch( nSubCmd ) {
		case eParty::SC_REFRESHGATEINFO: OnRecvPartyRefreshGateInfo( (SCRefreshGateInfo *)pData ); break;
		case eParty::SC_GATEINFO: OnRecvPartyReadyGate( (SCGateInfo *)pData ); break;
		case eParty::SC_STARTSTAGE: OnRecvPartyStageStart( (SCStartStage *)pData ); break;
		case eParty::SC_STARTSTAGEDIRECT: OnRecvPartyStageStartDirect( (SCStartStageDirect*)pData ); break;
		case eParty::SC_STARTVILLAGE: OnRecvPartyVillageStart( pData ); break;
		case eParty::SC_CANCELSTAGE: OnRecvPartyStageCancel( (SCCancelStage *)pData ); break;
		case eParty::SC_REFRESHPARTY: OnRecvPartyRefresh( (SCRefreshParty *)pData ); break;
//		case eParty::SC_EQUIPDATA: OnRecvPartyEquipData( (SCEquipData *)pData ); break;
//		case eParty::SC_SKILLDATA: OnRecvPartySkillData( (SCSkillData *)pData ); break;
//		case eParty::SC_ETERNITYITEM: OnRecvPartyEternityItemData( (SCEternityItemData *)pData ); break;
//		case eParty::SC_DEFAULTPARTSDATA: OnRecvPartyDefaultPartsData( (SCDefaultPartsData *)pData ); break;
		case eParty::SC_SELECTDUNGEON: OnRecvPartySelectDungeonInfo((SCSelectDunGeon*)pData); break;

#if defined ( PRE_ADD_BESTFRIEND)
		case eParty::SC_BESTFRIENDDATA: OnRecvBestFriendData( pData ); break;
#endif

	}
}

void CGameClientSession::OnRecvRoomMessage( int nSubCmd, char *pData, int nSize )
{
	switch( nSubCmd ) 
	{
		case eRoom::SC_GENERATION_MONSTER_MSG:			OnRecvRoomGenerationMonsterMsg( (SCGenerationMonster *)pData );						break;
		case eRoom::SC_GATEINFO_MSG:					OnRecvRoomGateInfoMsg( (SCGateInfoMessage *)pData );								break;
		case eRoom::SC_DUNGEONCLEAR_MSG:				OnRecvRoomDungeonClearMsg( (SCDungeonClear*)pData );								break;
		case eRoom::SC_CHANGE_GATESTATE_MSG:			OnRecvRoomChangeGateState( (SCChangeGateState *)pData );							break;
//		case eRoom::SC_GENERATION_PROP_MSG:				OnRecvRoomGenerationPropMsg( (SCGenerationProp *)pData );							break;
		case eRoom::SC_GENERATION_PROP_MSG:				OnRecvRoomGenerationPropMsg( pData, nSize );										break;
		case eRoom::SC_CHANGE_GAMESPEED:				OnRecvRoomChangeGameSpeed( (SCChangeGameSpeed *)pData );							break;
		case eRoom::SC_WARP_DUNGEONCLEAR:				OnRecvRoomWarpDungeonClear( (SCWarpDungeonClear*)pData );							break;
		case eRoom::SC_SYNC_START:						OnRecvRoomSyncStart( (SCSyncStart*)pData );											break;
		case eRoom::SC_GATEEJECTINFO_MSG:				OnRecvRoomGateEjectInfo( (SCGateEjectInfo *)pData );								break;
		case eRoom::SC_DUNGEONFAILED_MSG:				OnRecvRoomDungeonFailedMsg( (SCDungeonFailed*)pData );								break;
		case eRoom::SC_OPENDUNGEONLEVEL_MSG:			OnRecvRoomOpenDungeonLevel( (SCOpenDungeonOpenLevel *)pData );						break;
		case eRoom::SC_SELECT_REWARDITEM:				OnRecvRoomSelectRewardItem( (SCSelectRewardItem *)pData );							break;
		case eRoom::SC_DUNGEONCLEAR_SELECTREWARDITEM:	OnRecvRoomDungeonClearSelectRewardItem( pData );									break;
		case eRoom::SC_DUNGEONCLEAR_REWARDITEM:			OnRecvRoomDungeonClearRewardItem( (SCDungeonClearRewardItem *)pData );				break;
		case eRoom::SC_DUNGEONCLEAR_REWARDITEMRESULT:	OnRecvRoomDungeonClearRewardItemResult( (SCDungeonClearRewardItemResult*)pData );	break;
#if defined( PRE_ADD_NAMEDITEM_SYSTEM )
		case::eRoom::SC_DUNGEONCLEAR_REWARDBOXTYPE:		OnRecvRoomDungeonClearRewardBoxType((SCDungeonClearBoxType*)pData); break;
#endif
		case eRoom::SC_SYNC_DATUMTICK:					OnRecvRoomSyncDatumTick( pData );													break;
		case eRoom::SC_DLDUNGEONCLEAR_MSG:				OnRecvRoomDLDungeonClearMsg( (SCDLDungeonClear*)pData );							break;
		case eRoom::SC_DLDUNGEONCLEAR_RANKINFO:			OnRecvRoomDLDungeonClearRankInfo( (SCDLRankInfo*)pData );							break;
		case eRoom::SC_DUNGEONCLEAR_IDENTIFYREWARDITEM: OnRecvRoomIdentifyRewardItem( (SCIdentifyRewardItem *)pData );						break;
		case eRoom::SC_DUNGEONCLEAR_CANNOTWARP:			OnRecvRoomDungeonClearWarpFailed((SCDungeonClearWarpOutFail*)pData);				break;
		case eRoom::SC_DUNGEONCLEAR_ENABLE_LEADERWARP:	OnRecvRoomDungeonClearEnableLeaderWarp( (SCDungeonClearLeaderWarp*)pData);			break;
		case eRoom::SC_ROOMSYNC_SEQLEVEL:				OnRecvRoomSeqLevel((SCRoomSyncSeqLevel*)pData);										break;
		case eRoom::SC_DLCHALLENGE_REQUEST:				OnRecvDLChallengeRequest();															break;
		case eRoom::SC_DLCHALLENGE_RESPONSE:			OnRecvDLChallengeResponse( (SCDLChallengeResponse*)pData );							break;
		case::eRoom::SC_SYNC_DUNGEONTIMEATTACK:			OnRecvDungeonTimeAttack( (SCSyncDungeonTimeAttack*)pData );							break;
		case::eRoom::SC_STOP_DUNGEONTIMEATTACK:			OnRecvDungeonTimeAttackReset();														break;
		case::eRoom::SC_SPECIAL_REBIRTHITEM:			OnRecvSpecialRebirthItem( (SCSpecialRebirthItem*)pData );							break;
#ifdef PRE_MOD_DARKLAIR_RECONNECT
		case::eRoom::SC_DLROUND_INFO:					OnRecvDarkLairRoundInfo( (SCDLRoundInfo*)pData );									break;	
#endif 
#if defined(PRE_ADD_CP_RANK)
		case::eRoom::SC_ABYSS_STAGE_CLEAR_BEST:			OnRecvCPRank( (SCAbyssStageClearBest *)pData );																		break;
#endif
	}
} 

void CGameClientSession::OnRecvCharMessage(int nSubCmd, char *pData, int nSize )
{
	switch( nSubCmd ) {
		case eChar::SC_ENTER: OnRecvCharEntered( (SCEnter*)pData ); break;
		case eChar::SC_HIDE: OnRecvCharHide( (SCHide*)pData ); break;
		case eChar::SC_MAXLEVEL_CHARACTER_COUNT: OnRecvMaxCharCount( (SCMaxLevelCharacterCount*)pData ); break;
#if defined(PRE_ADD_REBIRTH_EVENT)
		case eChar::SC_REBIRTH_MAXCOIN : OnRecvRebirthMaxCoin( (SCRebirthMaxCoin*)pData); break;
#endif
	}
}

void CGameClientSession::OnRecvPVPMessage(int nSubCmd, char *pData, int nSize )
{
	switch( nSubCmd ) 
	{
		case ePvP::SC_FINISH_PVPMODE:		OnRecvFinishPvPMode( (void *)pData );						break;
		case ePvP::SC_FINISH_PVPROUND:		OnRecvFinishPvPRound( (void*)pData );						break;
		case ePvP::SC_START_PVPROUND:		OnRecvStartPVPRound( (void*)pData );						break;
		case ePvP::SC_MODE_STARTTICK:		OnRecvModeStartTick( (SCPVP_MODE_STARTTICK*)pData );		break;
		case ePvP::SC_MODE_SCORE:			OnRecvModeScrore( (SCPVP_MODE_SCORE *) pData );				break;
		case ePvP::SC_RESPAWN_POINT:		OnRecvRespawnPoint( (SCPVP_RESPAWN_POINT *) pData );		break;
		case ePvP::SC_XPSCORE:				OnRecvXpScore( (SCPVP_XPSCORE *) pData );					break;
		case ePvP::SC_BREAKINTO_SUCCESS:	OnRecvBreakIntoSuccess( (SCPVP_SUCCESSBREAKINTO *)pData );	break;
		case ePvP::SC_ADDPOINT:				OnRecvPvPAddPoint( pData );									break;

		case ePvP::SC_SELECTCAPTAIN:        OnRecvPvPSelectCaptain((SCPVP_SELECTCAPTAIN *) pData);      break;

		case ePvP::SC_CAPTAINKILL_COUNT:	OnRecvCaptainKillCount( (SCPVP_CAPTAINKILL_COUNT*)pData );	break;
		case ePvP::SC_SELECTZOMBIE:         OnRecvPvPSelectZombie((SCPVP_SELECTZOMBIE*)pData); break;
		case ePvP::SC_PVP_HOLYWATER_KILLCOUNT: OnRecvPVPAddGhoulScore((SCPVP_HOLYWATER_KILLCOUNT*)pData); break;
		case ePvP::SC_FATIGUE_REWARD :		OnRecvPvPFatigueReward((SCPVP_FATIGUE_REWARD*)pData); break;
		case ePvP::SC_PVP_TRYACQUIREPOINT :			OnRecvPvPTryAcquirePoint( (SCPvPTryAcquirePoint*)pData ); break;
		case ePvP::SC_PVP_OCCUPATION_POINTSTATE :	OnRecvPvPOccupationPointState( (SCPvPOccupationState*)pData ); break;
		case ePvP::SC_PVP_OCCUPATION_TEAMSTATE :	OnRecvPvPOccupationTeamState( (SCPvPOccupationTeamState*)pData ); break;
		case ePvP::SC_OCCUPATIONSCORE:				OnRecvOccupationScore( (TPvPOccupationScore *)pData ); break;
		case ePvP::SC_CONCENTRATE_ORDER:			OnRecvConcentrateOrder( (SCConcentrateOrder *)pData ); break;
		case ePvP::SC_PVP_CLIMAXMODE:				OnRecvOccupationClimaxMode(); break;

		case ePvP::SC_PVP_ALLKILL_SHOW_SELECTPLAYER: OnRecvAllKillModeShowSelectPlayer( (SCPVP_ALLKILL_SHOW_SELECTPLAYER *)pData ); break;
		case ePvP::SC_PVP_ALLKILL_ACTIVEPLAYER: OnRecvAllKillModeActivePlayer( (SCPVP_ALLKILL_ACTIVEPLAYER *)pData ); break;
		case ePvP::SC_PVP_ALLKILL_GROUPCAPTAIN: OnRecvAllKillModeSelectGroupCaptain( (SCPVP_ALLKILL_GROUPCAPTAIN *)pData ); break;
		case ePvP::SC_PVP_ALLKILL_CONTINUOUSWIN: OnRecvAllKillContinousWinCount( (SCPVP_ALLKILL_CONTINUOUSWIN *) pData ); break;
		case ePvP::SC_PVP_ALLKILL_FINISH_DETAILREASON: OnRecvAllKillFinishDetailReason( (SCPVP_ALLKILL_FINISHDETAILREASON *)pData ); break;
		case ePvP::SC_PVP_ALLKILL_BATTLEPLAYER: OnRecvAllKillBattlePlayer( (SCPVP_ALLKILL_BATTLEPLAYER *)pData ); break;
#if defined( PRE_ADD_RACING_MODE )
			case ePvP::SC_PVP_RACING_RAPTIME:	OnRecvRacingRapTime( (SCPVP_RACING_RAPTIME*)pData ); break;
			case ePvP::SC_PVP_RACING_FIRST_END:	OnRecvRacingFirstEnd( (SCPVP_RACING_FIRST_END*)pData ); break;
#endif	// #if defined( PRE_ADD_RACING_MODE )
#ifdef PRE_ADD_PVP_TOURNAMENT
		case ePvP::SC_PVP_TOURNAMENT_MATCHLIST: OnRecvPVPTournamentGameMatchList((SCPvPTournamentMatchList*)pData); break;
		case ePvP::SC_PVP_TOURNAMENT_DEFAULTWIN: OnRecvPVPTournamentDefaultWin((SCPvPTournamentDefaultWin*)pData); break;
		case ePvP::SC_PVP_TOURNAMENT_IDLE_TICK: OnRecvPVPTournamentIdleTick((SCPvPTournamentIdleTick*)pData); break;
		case ePvP::SC_PVP_TOURNAMENT_TOP4: OnRecvPVPTournamentTop4((SCPvPTournamentTop4*)pData); break;
#endif
#ifdef PRE_WORLDCOMBINE_PVP
		case ePvP::SC_PVP_WORLDPVPROOM_STARTMSG: OnRecvWorldPVPRoomStartMsg( (WorldPvPMissionRoom::SCWorldPvPRoomStartMsg*)pData ); break;
		case ePvP::SC_PVP_WORLDPVPROOM_START_RESULT: OnRecvWorldPVPRoomStartResult( (WorldPvPMissionRoom::SCWorldPvPRoomStartReturn*)pData ); break;
		case ePvP::SC_PVP_WORLDPVPROOM_JOIN_RESULT: OnRecvWorldPVPRoomJoinResult( (WorldPvPMissionRoom::SCWorldPvPRoomJoinResult*)pData ); break;
		case ePvP::SC_PVP_WORLDPVPROOM_ALLKILLTEAMINFO: OnRecvWorldPVPRoomAllKillTeamInfo( (WorldPvPMissionRoom::SCWorldPvPRoomAllKillTeamInfo*)pData ); break;
		case ePvP::SC_PVP_WORLDPVPROOM_TOURNAMENTUSERINFO: OnRecvWorldPVPRoomTournamentUserInfo( (WorldPvPMissionRoom::SCWorldPvPRoomTournamentUserInfo*)pData ); break;
#endif // PRE_WORLDCOMBINE_PVP

#ifdef PRE_ADD_PVP_COMBOEXERCISE
		case ePvP::SC_PVP_COMBOEXERCISE_ROOMMASTERINFO: OnRecvPVPComboModeMaster( (SCPvPComboExerciseRoomMasterInfo * )pData ); break;
		case ePvP::SC_PVP_COMBOEXERCISE_CHANGEROOMMASTER: OnRecvPVPComboModeChangeMaster( (SCPvPComboExerciseChangeRoomMaster *)pData ); break;
		case ePvP::SC_PVP_COMBOEXERCISE_RECALLMONSTERRESULT: OnRecvPVPComboModeMonsterGen( (SCPvPComboExerciseRecallMonsterResult *)pData ); break;
#endif // PRE_ADD_PVP_COMBOEXERCISE
	}
}

#ifndef _FINAL_BUILD
extern bool g_bPauseMode;
#endif

void CGameClientSession::OnRecvActorMessage( int nSubCmd, char *pData, int nSize )
{
#ifndef _FINAL_BUILD
	if( g_bPauseMode )
	{
		return;
	}
#endif

	SCActorMessage *pPacket = (SCActorMessage *)pData;

	DnActorHandle hActor = CDnActor::FindActorFromUniqueID( pPacket->nSessionID );
	if( !hActor ) return;
	MATransAction *pTrans = dynamic_cast<MATransAction *>(hActor.GetPointer());
	if( pTrans ) {
#ifdef PACKET_DELAY
		pTrans->InsertRecvPacketDelay( nSubCmd, (BYTE*)pPacket->cBuf, nSize );
#else
		pTrans->OnDispatchMessage( nSubCmd, (BYTE*)pPacket->cBuf );
#endif PACKET_DELAY
	}
}

void CGameClientSession::OnRecvActorBundleMessage( int nSubCmd, char *pData, int nSize )
{
	SCActorBundleMessage *pPacket = (SCActorBundleMessage *)pData;

	BYTE cSubCmd;
	WORD nCurSize;
	int nOffset = 0;
	SCActorMessage Packet;
	ZeroMemory(&Packet, sizeof(SCActorMessage));
	for( int i=0; i<nSubCmd; i++ ) {
		memcpy( &Packet.nSessionID, pPacket->cBuf + nOffset, sizeof(DWORD) ); nOffset += sizeof(DWORD);
		memcpy( &cSubCmd, pPacket->cBuf + nOffset, sizeof(BYTE) ); nOffset += sizeof(BYTE);
		memcpy( &nCurSize, pPacket->cBuf + nOffset, sizeof(WORD) ); nOffset += sizeof(WORD);
		memcpy( Packet.cBuf, pPacket->cBuf + nOffset, nCurSize ); nOffset += nCurSize;

		// 번들로 온거는 나누면서 다시 로드오브젝트인지 체크해준다.
		// 또한 사이즈는 패킷 풀 사이즈를 줘야하기 떄문에 sizeof(DWORD) 해준다.(nSessionID 땜시 )
		if( CDnLoadingTask::IsActive() ) {
			if( CDnLoadingTask::GetInstance().InsertLoadPacket( this, SC_ACTOR, cSubCmd, (char*)&Packet, nCurSize + sizeof(DWORD) ) ) continue;
		}

		OnRecvActorMessage( cSubCmd, (char*)&Packet, nCurSize );
	}
}

void CGameClientSession::OnRecvPropMessage( int nSubCmd, char *pData, int nSize )
{
	SCPropMessage *pPacket = (SCPropMessage *)pData;

	DnPropHandle hProp = CDnWorldProp::FindPropFromUniqueID( pPacket->nUniqueID );
	if( !hProp ) return;
	MPTransAction *pTrans = dynamic_cast<MPTransAction *>(hProp.GetPointer());
	if( pTrans ) pTrans->OnDispatchMessage( nSubCmd, (BYTE*)pPacket->cBuf );
}

void CGameClientSession::OnRecvPartyReadyGate( SCGateInfo *pPacket )
{
	m_nGateIndex = pPacket->cGateNo;
}

/*
void CGameClientSession::OnRecvPartyDefaultPartsData( SCDefaultPartsData *pPacket )
{
	CDnPartyTask::GetInstance().SetDefaultPartsInfo( (int*)pPacket->nDefaultPartsIndex );
}
*/

void CGameClientSession::OnRecvPartySelectDungeonInfo(SCSelectDunGeon * pPacket)
{
	if( !pPacket )
		return;

	if( CDnPartyTask::GetInstance().GetPartyRole() == CDnPartyTask::MEMBER )
	{
		GetInterface().SetSelectDungeonInfo( pPacket->nMapIndex, pPacket->cDiffyculty );
	}
}
/*
void CGameClientSession::OnRecvPartyEquipData( SCEquipData *pPacket ) 
{
	LogWnd::Log(1, _T("OnTRecvPartyStageStart "));

	TPartyMemberEquip MemberEquip[PARTYMAX];
	memset( &MemberEquip, 0, sizeof(MemberEquip) );
	int nOffset = sizeof(char);
	int nSize = 0;

	for( int i=0; i<pPacket->cCount; i++ ) {
		char cCount = *((char*)pPacket + nOffset);
		nSize = ( cCount * sizeof(TItemInfo) ) + sizeof(char);
		memcpy( &MemberEquip[i], (char*)pPacket + nOffset, nSize );
		nOffset += nSize;
	}

	CDnPartyTask::GetInstance().SetEquipInfo( MemberEquip );
}

void CGameClientSession::OnRecvPartySkillData( SCSkillData *pPacket )
{
	TPartyMemberSkill MemberSkill[PARTYMAX];
	memset( &MemberSkill, 0, sizeof(MemberSkill) );
	int nOffset = sizeof(char);
	int nSize = 0;

	for( int i=0; i<pPacket->cCount; i++ ) {
		char cCount = *((char*)pPacket + nOffset);
		nSize = ( cCount * sizeof(TSkill) ) + sizeof(char);
		memcpy( &MemberSkill[i], (char*)pPacket + nOffset, nSize );
		nOffset += nSize;
	}

	CDnPartyTask::GetInstance().SetSkillInfo( MemberSkill );
} 


// 효과 영구적용 아이템 정보
void CGameClientSession::OnRecvPartyEternityItemData( SCEternityItemData *pPacket )
{
	TPartyMemberEternityItem MemberEternityItemArray[ PARTYMAX ];
	memset( &MemberEternityItemArray, 0, sizeof(MemberEternityItemArray) );

	_ASSERT( sizeof(MemberEternityItemArray) == sizeof(pPacket->EternityItem) );
	memcpy_s( MemberEternityItemArray, sizeof(TPartyMemberEternityItem)*pPacket->cMemberCount, 
			  pPacket->EternityItem, sizeof(TPartyMemberEternityItem)*pPacket->cMemberCount );
	
	//int nCount = 0;
	//for( int i = 0; i < pPacket->cMemberCount; ++i )
	//{
	//	memcpy_s( &(MemberEternityItemArray[ i ]), sizeof(TPartyMemberEternityItem), &(pPacket->EternityItem[ i ]), sizeof(TPartyMemberEternityItem) );
	//}

	CDnPartyTask::GetInstance().SetEternityItemInfo( MemberEternityItemArray );
}

void CGameClientSession::OnRecvPartyEtcData( SCEtcData *pPacket )
{
	TPartyEtcData EtcData[PARTYMAX];
	memset( &EtcData, 0, sizeof(EtcData) );

	CDnPartyTask::GetInstance().SetEtcInfo( EtcData );
}
*/

void CGameClientSession::SetSyncComplete( bool bFlag )
{
	m_bSyncComplete = bFlag;
}

void CGameClientSession::Process( LOCAL_TIME LocalTime, float fDelta )
{
#ifdef PACKET_DELAY
	for( DWORD i=0; i<CDnActor::s_pVecProcessList.size(); i++ ) {
		CDnActor::s_pVecProcessList[i]->ProcessDelay( LocalTime, fDelta );
	}
#endif //PACKET_DELAY
}

void CGameClientSession::OnRecvRoomSyncStart( SCSyncStart *pPacket )
{
	FUNC_LOG();
	DebugLog("CGameClientSession::OnRecvRoomSyncStart");
	SetSyncComplete( true );
}

void CGameClientSession::OnRecvRoomSyncDatumTick( char* pPacket )
{
	FUNC_LOG();

	if( CDnActor::s_hLocalActor  ) 
	{
		CDnLocalPlayerActor* pLocal = (CDnLocalPlayerActor*)(CDnActor::s_hLocalActor.GetPointer());
		_ASSERT( pLocal );
		pLocal->SetSyncDatumTick();
	}
}

bool __stdcall CGameClientSession::OnCheckLoadingPacket( void *pThis, UINT nUniqueID, int nMainCmd, int nSubCmd, void *pParam, int nSize )
{
	if( pThis != dynamic_cast<CGameClientSession*>(CTaskManager::GetInstance().GetTask( "GameTask" )) ) return false;
	switch( nMainCmd ) {
		case SC_ACTOR:
			if( ((SCActorMessage *)pParam)->nSessionID == nUniqueID ) return true;
			break;
			/*
		case SC_ACTORBUNDLE: 
			if( ((SCActorBundleMessage *)pParam)->nSessionID == nUniqueID ) return true;
			break;
			*/
		case SC_PROP:
			{
				CPacketCompressStream PacketStream( pParam, nSize );
				DWORD dwUniqueID = 0;
				PacketStream.Read( &dwUniqueID, sizeof(DWORD) );
				if( dwUniqueID == nUniqueID )
					return true;
				//if( ((SCPropMessage *)pParam)->nUniqueID == nUniqueID ) return true;
			}
			break;
	}
	return false;
}

void CGameClientSession::OnRecvXpScore( SCPVP_XPSCORE * pData )	
{
	FUNC_LOG();

	for( int iCount=0;iCount< pData->cCount; iCount++ )
	{
#ifdef PRE_MOD_PVP_LADDER_XP
		GetInterface().SetPVPXP( pData->sXPScoreArr[iCount].uiSessionID, pData->sXPScoreArr[iCount].uiAddXPScore, 
			pData->sXPScoreArr[iCount].uiResultXPScore, pData->sXPScoreArr[iCount].uiGainMedalCount, pData->sXPScoreArr[iCount].uiAddLadderXPScore ); // 메달이 추가됐습니다.
#else // PRE_MOD_PVP_LADDER_XP
		GetInterface().SetPVPXP( pData->sXPScoreArr[iCount].uiSessionID, pData->sXPScoreArr[iCount].uiAddXPScore, 
								pData->sXPScoreArr[iCount].uiResultXPScore, pData->sXPScoreArr[iCount].uiGainMedalCount ); // 메달이 추가됐습니다.
#endif // PRE_MOD_PVP_LADDER_XP
	}
}