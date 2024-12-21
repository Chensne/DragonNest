
#pragma once

#include "DNGameRoom.h"

namespace LadderSystem
{
	class CStatsRepository;
#if defined(PRE_ADD_DWC)
	class CDWCStatsRepository;
#endif
};

class CDNPvPGameRoom:public CDNGameRoom,public TBoostMemoryPool<CDNPvPGameRoom>
{
public:
	CDNPvPGameRoom( CDNRUDPGameServer* pServer, unsigned int iRoomID, MAGAReqRoomID* pPacket );
	virtual ~CDNPvPGameRoom();

	virtual CPvPGameMode*	GetPvPGameMode() const { return m_pPvPGameMode; }

	virtual void OnSetPlayState();
	virtual void OnDelPartyMember( UINT iDelMemberSessionID, char cKickKind );
	//
	virtual bool bIsPvPStart();
	virtual void OnDamage( DnActorHandle hActor, DnActorHandle hHitter, const INT64 iDamage );
	virtual void OnRebirth( DnActorHandle hActor );
	virtual void OnDie( DnActorHandle hActor, DnActorHandle hHitter );
	virtual void OnCmdAddStateEffect( const CDnSkill::SkillInfo* pSkillInfo );
	virtual void OnLeaveUser( const UINT uiSessionID );

	virtual void OnFinishProcessDie( DnActorHandle hActor );
	virtual bool bIsPvPRoom(){ return true; }
	virtual bool bIsLadderRoom(){ return m_bIsLadderRoom; }
	virtual bool bIsZombieMode(){ return GetPvPGameModeCode() == PvPCommon::GameMode::PvP_Zombie_Survival; }
	virtual bool bIsAllKillMode(){ return GetPvPGameModeCode() == PvPCommon::GameMode::PvP_AllKill; } 
#if defined( PRE_ADD_PVP_TOURNAMENT)
	virtual bool bIsTournamentMode(){ return GetPvPGameModeCode() == PvPCommon::GameMode::PvP_Tournament; } 
#endif
	virtual bool bIsGuildWarMode() { return GetPvPGameModeCode() == PvPCommon::GameMode::PvP_GuildWar; }
	virtual bool bIsOccupationMode();
#if defined( PRE_ADD_PVP_COMBOEXERCISE )
	virtual bool bIsComboExerciseMode() { return GetPvPGameModeCode() == PvPCommon::GameMode::PvP_ComboExercise; }
#endif // #if defined( PRE_ADD_PVP_COMBOEXERCISE )

	virtual bool bIsGuildWarSystem();
	void GetGuildWarInfoByIndex(int nIndex, UINT &nGuildDBID, int &nScore);
	virtual void OnSendPvPLobby();
	virtual bool bIsLevelRegulation();

	virtual bool InitializePvPGameMode( const MAGAPVP_GAMEMODE* pPacket );

	virtual void OnRequestSyncStartMsg( CDNUserSession* pGameSession );
	virtual void GetPvPSN( INT64& biMain, int& iSub ){ biMain = m_biSNMain; iSub = m_iSNSub; }

	UINT GetPvPGameModeCode();

	//GuildWar
	virtual void QueryAddGuildWarFinalResults(UINT nGuildDBID, char cMatchResultCode);

protected:
#if defined(PRE_ADD_DWC)
	virtual void OnInitGameRoomUser();
#endif
	float GetDieDelta();

public:
	virtual void OnSuccessBreakInto( CDNUserSession* pSession );
	virtual void OnSuccessBreakInto( std::list<CDNUserSession*>::iterator& itor );

private:

	virtual void OnSendPartyMemberInfo( CDNUserSession* pBreakIntoSession );
	virtual void OnSendTeamData( CDNUserSession* pBreakIntoSession );

private:

	void _InitializeRespawnPosition( DnActorHandle hActor );
	//
	void _SuccessBreakInto( CDNUserSession* pBreakIntoGameSession );
	void _OnDropItem( DnActorHandle hDieActor );
	void _ShuffleTeam();

	INT64	m_biSNMain;
	int		m_iSNSub;

	bool	m_bIsLadderRoom;
	INT64	m_biLadderRoomIndexArr[2];
	LadderSystem::MatchType::eCode m_PvPLadderMatchType;

	bool	m_bIsPWRoom;
	USHORT	m_unRoomOptionBit;
	BYTE	m_cMaxUser;

	CPvPGameMode*	m_pPvPGameMode;
	LadderSystem::CStatsRepository*	m_pLadderStatsRepository;
#if defined(PRE_ADD_DWC)
	LadderSystem::CDWCStatsRepository* m_pDWCStatsRepository;
#endif

	UINT m_nGuildDBID[PvPCommon::Common::DefaultGuildCount];
	int m_nGuildQualifyingScore[PvPCommon::Common::DefaultGuildCount];
	INT64	m_biRoomMasterCharacterDBID;
	INT64	m_biCreateRoomCharacterDBID;
#if defined( PRE_ADD_PVP_COMBOEXERCISE )
	UINT	m_uiRoomMasterSessionID;
#endif // #if defined( PRE_ADD_PVP_COMBOEXERCISE )

#if defined( PRE_PVP_GAMBLEROOM )
	BYTE m_cGambleType;
	BYTE m_cGamblePlayerCount;
	int m_nGamblePrice;
#endif

public:

	void	CmdPvPStartAddStateEffect( DnActorHandle hActor, const int iTick, const bool bAddForce=false );
	void	CmdObserverAddStateEffect( DnActorHandle hActor, const bool bAddForce=false );
	void CmdPvPOccupationAddStateEffect(CDnSkill::SkillInfo * pSkillInfo, DnActorHandle hActor, int nBLow, const int iTick, const char * pVal, const bool bAddForce = false);
	void CmdPvPOccupationRemoveStateEffect(DnActorHandle hActor, int nBLow);
	bool	bIsPWRoom() const { return m_bIsPWRoom; }
	bool	bIsBreakIntoRoom() const { return m_unRoomOptionBit&PvPCommon::RoomOption::BreakInto ? true : false; }
	bool	bIsDropItemRoom() const { return m_unRoomOptionBit&PvPCommon::RoomOption::DropItem ? true : false; }
	USHORT	GetRoomOptionBit() const { return m_unRoomOptionBit; }
	virtual BYTE	GetPvPMaxUser() const { return m_cMaxUser; }
	INT64	GetLadderRoomIndex( int iIndex ){ return m_biLadderRoomIndexArr[iIndex]; }
	LadderSystem::MatchType::eCode GetPvPLadderMatchType(){ return m_PvPLadderMatchType; }
	void	SetPvPLadderMatchType( LadderSystem::MatchType::eCode Type ){ m_PvPLadderMatchType = Type; }
	LadderSystem::CStatsRepository*	GetLadderStatsRepositoryPtr(){ return m_pLadderStatsRepository; }
#if defined(PRE_ADD_DWC)
	LadderSystem::CDWCStatsRepository*	GetDWCStatsRepositoryPtr(){ return m_pDWCStatsRepository; }
#endif

	void	SendSelectCaptain( DnActorHandle hActor );
	void	SendSelectZombie( std::map<DWORD,DnActorHandle>& mZombie );
	void	SendSelectZombie( DnActorHandle hActor, int nTableID, bool bZombie, bool bRemoveStateBlow, int iScale=100 );

	void SendMemberGrade(USHORT nTeam, UINT uiUserState, UINT nChangedSessionID, int nRetCode);
	void SendMemberIndex(USHORT nTeam, BYTE cCount, const TSwapMemberIndex * pIndex, int nRetCode);
	void SendAllKillShowSelectPlayer();
	INT64	GetRoomMasterCharacterDBID() { return m_biRoomMasterCharacterDBID;}
	INT64	GetCreateRoomCharacterDBID() { return m_biCreateRoomCharacterDBID;}

#if defined(PRE_ADD_REVENGE)	
	void BroadcastRevengeSuccess( UINT uiSessionID, UINT uiRevengeTargetSessionID );
	void OnLeaveRoomBeforeFinish( UINT uiSessionID );	//방에서 나간 상황(PVP 도중 게임종료 or 케릭선택창으로 이동)
#endif

#if defined( PRE_ADD_PVP_COMBOEXERCISE )
	void	ChangeRoomMaster( INT64 biRoomMasterCharacterDBID, UINT uiRoomMasterSessionID );
	UINT	GetRoomMasterSessionID() const { return m_uiRoomMasterSessionID; }
#endif // #if defined( PRE_ADD_PVP_COMBOEXERCISE )

#if defined( PRE_PVP_GAMBLEROOM )
	BYTE GetGambleType(){ return m_cGambleType; }
	int GetGamblePrice(){ return m_nGamblePrice; }
	BYTE GetGamblePlayerCount(){ return m_cGamblePlayerCount; }
	void SetGamblePlayerCount( BYTE cPlayerCount ){ m_cGamblePlayerCount = cPlayerCount; }
#endif

#if defined(PRE_ADD_DWC)
	bool bIsDWCMatch();
	CDNUserSession* GetFirstTeamMember(int iTeam);
	void SendDWCTeamNameInfo(const WCHAR * pATeamName, const WCHAR * pBTeamName);
#endif
};
