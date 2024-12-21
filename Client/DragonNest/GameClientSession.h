#pragma once

//#include "ClientUdpSession.h"
#include "MessageListener.h"
#include "DnActor.h"
#include "DNPacket.h"

class CGameClientSession : public CTaskListener
{
public:
	CGameClientSession();
	virtual ~CGameClientSession();

	struct SConnectUserStruct {
		DnActorHandle hActor;
		tstring szUserID;
		/*
		std::string szVirtualAddress;
		USHORT nVirtualPort;
		bool bCanP2P;
		*/

		SConnectUserStruct() {
//			bCanP2P = true;
//			nVirtualPort = -1;
		};
	};

	CSyncLock m_ConnectListLock;

protected:
	std::vector<SConnectUserStruct> m_VecConnectList;	// 해당 컨테이너는 난입시 쓰레디 동기 보호가 안되므로 반드시 m_ConnectListLock 를 사용해야한다!!!!!!!
	DnActorHandle m_hLocalPlayerActor;

	int m_nStageRandomSeed;
	int m_nGateIndex;
	bool m_bSyncComplete;
 
protected:
	void OnRecvPartyMessage( int nSubCmd, char *pData, int nSize );
	void OnRecvRoomMessage( int nSubCmd, char *pData, int nSize );
	void OnRecvActorMessage( int nSubCmd, char *pData, int nSize );
	void OnRecvActorBundleMessage( int nSubCmd, char *pData, int nSize );
	void OnRecvPropMessage( int nSubCmd, char *pData, int nSize );
	void OnRecvCharMessage(int nSubCmd, char *pData, int nSize );

	//blondy pvp
	void OnRecvPVPMessage(int nSubCmd, char *pData, int nSize );
	//blondy end 

	BYTE m_cSeqLevel;

public:
	// Loading 관련
	static bool __stdcall OnCheckLoadingPacket( void *pThis, UINT nUniqueID, int nMainCmd, int nSubCmd, void *pParam, int nSize );

	virtual void AddConnectUser( DnActorHandle hActor, TCHAR *szUserID );

	DWORD GetConnectUserCount();
	SConnectUserStruct *GetConnectUserData( DWORD dwIndex );
	DnActorHandle GetLocalPlayerActor() { return m_hLocalPlayerActor; }

	virtual void SetSyncComplete( bool bFlag );
	bool IsSyncComplete() { return m_bSyncComplete; }

	int GetReadyGateIndex() { return m_nGateIndex; }
	BYTE GetSeqLevel() { return m_cSeqLevel; }

	virtual void Process( LOCAL_TIME LocalTime, float fDelta );

	// ClientSession
	virtual void OnDisconnectUdp( bool bValidDisconnect );
	virtual void OnDisconnectTcp( bool bValidDisconnect );

	virtual void OnDispatchMessage( int nMainCmd, int nSubCmd, char *pData, int nSize );

	virtual void OnRecvRoomGenerationMonsterMsg( SCGenerationMonster *pPacket ) {}
	virtual void OnRecvRoomGateInfoMsg( SCGateInfoMessage *pPacket ) {}
	virtual void OnRecvRoomDungeonClearMsg( SCDungeonClear *pPacket ) {}
	virtual void OnRecvRoomChangeGateState( SCChangeGateState *pPacket ) {}
	//virtual void OnRecvRoomGenerationPropMsg( SCGenerationProp *pPacket ) {}
	virtual void OnRecvRoomGenerationPropMsg( char* pPacket, int nSize ) {};
	virtual void OnRecvRoomChangeGameSpeed( SCChangeGameSpeed *pPacket ) {}
//	virtual void OnRecvRoomOKDungeonClear( SCOKDungeonClear *pPacket ) {}
	virtual void OnRecvRoomGateEjectInfo( SCGateEjectInfo *pPacket ) {}
	virtual void OnRecvRoomDungeonFailedMsg( SCDungeonFailed *pPacket ) {}
	virtual void OnRecvRoomOpenDungeonLevel( SCOpenDungeonOpenLevel *pPacket ) {}
	virtual void OnRecvRoomSelectRewardItem( SCSelectRewardItem *pPacket ) {}
	virtual void OnRecvRoomDungeonClearSelectRewardItem( char *pPacket ) {}
	virtual void OnRecvRoomDungeonClearRewardItem( SCDungeonClearRewardItem *pPacket ) {}
#if defined( PRE_ADD_NAMEDITEM_SYSTEM )
	virtual void OnRecvRoomDungeonClearRewardBoxType(SCDungeonClearBoxType* pData) {}
#endif
	virtual void OnRecvRoomIdentifyRewardItem( SCIdentifyRewardItem *pPacket ) {}
	virtual void OnRecvRoomDungeonClearWarpFailed(SCDungeonClearWarpOutFail* pPacket) {}
	virtual void OnRecvRoomDungeonClearEnableLeaderWarp(SCDungeonClearLeaderWarp* pPacket) {}
	void OnRecvRoomSeqLevel(SCRoomSyncSeqLevel * pPacket) { m_cSeqLevel = pPacket->cSeqLevel; }
	virtual void OnRecvRoomDungeonClearRewardItemResult( SCDungeonClearRewardItemResult *pPacket ) {}
	virtual void OnRecvRoomWarpDungeonClear( SCWarpDungeonClear *pPacket ) {}
	virtual void OnRecvRoomDLDungeonClearMsg( SCDLDungeonClear *pPacket ) {}
	virtual void OnRecvRoomDLDungeonClearRankInfo( SCDLRankInfo *pPacket ) {}
	virtual void OnRecvDLChallengeRequest() {}
	virtual void OnRecvDLChallengeResponse( SCDLChallengeResponse *pPacket ) {}
	virtual void OnRecvDungeonTimeAttack( SCSyncDungeonTimeAttack* pPacket ) {}
	virtual void OnRecvDungeonTimeAttackReset() {}
	virtual void OnRecvPartyRefreshGateInfo( SCRefreshGateInfo *pPacket ) {}
	virtual void OnRecvPartyReadyGate( SCGateInfo *pPacket );
	virtual void OnRecvPartyStageStart( SCStartStage *pPacket ) {}
	virtual void OnRecvPartyStageStartDirect( SCStartStageDirect *pPacket ) {}
	virtual void OnRecvPartyStageCancel( SCCancelStage *pPacket ) {}
	virtual void OnRecvPartyVillageStart( char *pPacket ) {}
	virtual void OnRecvPartyRefresh( SCRefreshParty *pPacket ) {}
	/*
	virtual void OnRecvPartyEquipData( SCEquipData *pPacket );
	virtual void OnRecvPartySkillData( SCSkillData *pPacket );
	virtual void OnRecvPartyEternityItemData( SCEternityItemData *pPacket );
	virtual void OnRecvPartyEtcData( SCEtcData *pPacket );
	virtual void OnRecvPartyDefaultPartsData( SCDefaultPartsData *pPacket );
	*/

	virtual void OnRecvRoomSyncStart( SCSyncStart *pPacket );
	virtual void OnRecvRoomSyncDatumTick( char* pPacket );
	virtual void OnRecvPartySelectDungeonInfo(SCSelectDunGeon * pPacket);

	virtual void OnRecvCharEntered( SCEnter *pPacket ) {}
	virtual void OnRecvCharHide( SCHide *pPacket ) {}
	virtual void OnRecvMaxCharCount( SCMaxLevelCharacterCount* pPacket ) {}
#if defined(PRE_ADD_REBIRTH_EVENT)
	virtual void OnRecvRebirthMaxCoin( SCRebirthMaxCoin* pPacket ) {};
#endif

#ifdef PRE_ADD_BESTFRIEND
	virtual void OnRecvBestFriendData( char * pData ){}
#endif

	//blondy pvp
	virtual void OnRecvFinishPvPMode(  void * pData ){};
	virtual void OnRecvFinishPvPRound( void* pData ){};
	virtual void OnRecvStartPVPRound( void* pData ){};
	virtual void OnRecvModeStartTick( SCPVP_MODE_STARTTICK * pData ){};
	virtual void OnRecvModeScrore( SCPVP_MODE_SCORE * pData ){};
	virtual void OnRecvCaptainKillCount( SCPVP_CAPTAINKILL_COUNT * pData ){};
	virtual void OnRecvRespawnPoint ( SCPVP_RESPAWN_POINT * pData ){};
	virtual void OnRecvXpScore( SCPVP_XPSCORE * pData );
	virtual void OnRecvBreakIntoSuccess( SCPVP_SUCCESSBREAKINTO *pData ) {}
	virtual void OnRecvPvPAddPoint( char* pData ){}

	virtual void OnRecvPvPSelectCaptain( SCPVP_SELECTCAPTAIN *pData ){}
	virtual void OnRecvPvPSelectZombie( SCPVP_SELECTZOMBIE *pData){}

	virtual void OnRecvPVPAddGhoulScore(SCPVP_HOLYWATER_KILLCOUNT* pData){}
	//blondy end
	virtual void OnRecvPvPFatigueReward( SCPVP_FATIGUE_REWARD *pData ){};
	virtual void OnRecvPvPTryAcquirePoint( SCPvPTryAcquirePoint * pData ){};
	virtual void OnRecvPvPOccupationPointState( SCPvPOccupationState * pData){};
	virtual void OnRecvPvPOccupationTeamState( SCPvPOccupationTeamState * pData){};
	virtual void OnRecvOccupationScore(TPvPOccupationScore * pData){};
	virtual void OnRecvConcentrateOrder( SCConcentrateOrder *pData ) {}
	virtual void OnRecvOccupationClimaxMode() {}

	virtual void OnRecvSpecialRebirthItem( SCSpecialRebirthItem * pData ) {}

	virtual void OnRecvAllKillModeShowSelectPlayer( SCPVP_ALLKILL_SHOW_SELECTPLAYER * pData ) {}
	virtual void OnRecvAllKillModeActivePlayer( SCPVP_ALLKILL_ACTIVEPLAYER * pData ) {}
	virtual void OnRecvAllKillModeSelectGroupCaptain( SCPVP_ALLKILL_GROUPCAPTAIN *pData ) {}
	virtual void OnRecvAllKillContinousWinCount( SCPVP_ALLKILL_CONTINUOUSWIN *pData ) {}
	virtual void OnRecvAllKillFinishDetailReason( SCPVP_ALLKILL_FINISHDETAILREASON *pData ) {}
	virtual void OnRecvAllKillBattlePlayer( SCPVP_ALLKILL_BATTLEPLAYER *pData ) {}

#ifdef PRE_ADD_PVP_TOURNAMENT
	virtual void OnRecvPVPTournamentGameMatchList(SCPvPTournamentMatchList* pData) {}
	virtual void OnRecvPVPTournamentDefaultWin(SCPvPTournamentDefaultWin* pData) {}
	virtual void OnRecvPVPTournamentIdleTick(SCPvPTournamentIdleTick* pData) {}
	virtual void OnRecvPVPTournamentTop4(SCPvPTournamentTop4* pData) {}
#endif

#ifdef PRE_WORLDCOMBINE_PVP
	virtual void OnRecvWorldPVPRoomStartMsg( WorldPvPMissionRoom::SCWorldPvPRoomStartMsg* pPacket ) {}
	virtual void OnRecvWorldPVPRoomStartResult( WorldPvPMissionRoom::SCWorldPvPRoomStartReturn* pPacket ) {}
	virtual void OnRecvWorldPVPRoomJoinResult( WorldPvPMissionRoom::SCWorldPvPRoomJoinResult* pPacket ) {}
	virtual void OnRecvWorldPVPRoomAllKillTeamInfo( WorldPvPMissionRoom::SCWorldPvPRoomAllKillTeamInfo* pPacket ) {}
	virtual void OnRecvWorldPVPRoomTournamentUserInfo( WorldPvPMissionRoom::SCWorldPvPRoomTournamentUserInfo* pPacket ) {}
#endif // PRE_WORLDCOMBINE_PVP

#if defined( PRE_ADD_RACING_MODE )
	virtual void OnRecvRacingRapTime( SCPVP_RACING_RAPTIME * pData ) {}
	virtual void OnRecvRacingFirstEnd( SCPVP_RACING_FIRST_END * pData ) {}
#endif	// #if defined( PRE_ADD_RACING_MODE )

#ifdef PRE_ADD_PVP_COMBOEXERCISE
	virtual void OnRecvPVPComboModeMaster( SCPvPComboExerciseRoomMasterInfo * pData ){}
	virtual void OnRecvPVPComboModeChangeMaster( SCPvPComboExerciseChangeRoomMaster * pData ){}
	virtual void OnRecvPVPComboModeMonsterGen( SCPvPComboExerciseRecallMonsterResult * pData ){}
#endif // PRE_ADD_PVP_COMBOEXERCISE

#ifdef PRE_MOD_DARKLAIR_RECONNECT
	virtual void OnRecvDarkLairRoundInfo( SCDLRoundInfo* pData ){}
#endif 

#if defined(PRE_ADD_CP_RANK)
	virtual void OnRecvCPRank( SCAbyssStageClearBest *pData ){}
#endif	// #if defined(PRE_ADD_CP_RANK)
};

