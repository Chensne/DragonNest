
#pragma once

#include "ScoreSystem.h"

class CDNGameRoom;
class CDNUserSession;
class IPvPGameModeChecker;
class CPvPOccupactionSystem;

#if defined(PRE_ADD_REVENGE)
class CRevengeSystem;
#endif


class CPvPGameMode
{
public:

	virtual ~CPvPGameMode();

	// GetMethod
	const TPvPGameModeTable*		GetPvPGameModeTable() const { return m_pPvPGameModeTable; }
	CDNGameRoom*					GetGameRoom() const { return m_pGameRoom; }
	UINT							GetSelectWinCondition() const { return m_uiWinCondition; }
	UINT							GetSelectPlayTime() const { return m_uiPlayTimeSec; }
	UINT							GetRemainStartTick(){ return (m_fStartDelta>0.f) ? static_cast<UINT>(m_fStartDelta*1000) : 0; }
	UINT							GetGameModeCheck() const { return m_uiGameModeCheck; }
	UINT							GetWinTeam() const { return m_uiWinTeam; }
	PvPCommon::FinishReason::eCode	GetFinishReason() const { return m_FinishReason; }
	PvPCommon::FinishDetailReason::eCode GetFinishDetailReason() const { return m_FinishDetailReason; }
	UINT							GetStartTick() const { return m_uiStartTick; }
	float							GetTimeOver() const { return m_fTimeOver; }
#if !defined( _FINAL_BUILD )
	void							SetForceTimeOver(){ m_fTimeOver=0.f; }
#endif // #if !defined( _FINAL_BUILD )
	float							GetElapsedTimeRound() const { return m_fElapsedTimeRound; }
	void							GetGameModeScore( UINT& uiATeamScore, UINT& uiBTeamScore ) const;
	IScoreSystem*					GetScoreSystem() const { return m_pScoreSystem; }
	UINT							GetPlayTime( const wchar_t* pwszName );
	virtual CPvPOccupactionSystem * GetOccupationSystem() { return NULL; }
	virtual bool GetOccupationTeamScore(int nTeam, int &nScore) { return false; }
#if defined(PRE_ADD_REVENGE)
	CRevengeSystem*					GetRevengeSystem() const { return m_pRevengeSystem; }
#endif

	// SetMethod

	// Process
	virtual void	Process( LOCAL_TIME LocalTime, float fDelta );

	//
	bool	InitializeBase( const UINT uiWinCondition );
	void	FinishGameMode( const UINT uiWinTeam, PvPCommon::FinishReason::eCode Reason );
	void	SendFinishGameMode( CDNUserSession* pGameSession );
	bool	bIsInPlayTime( const UINT uiSec );
	int		GetPlayRound(){ return m_uiCurRound-1; }
	bool	bIsStarting(){ return (m_bStartGameModeFlag && m_fStartDelta > 0.f); }	// PvP시작 3,2,1 상태인가??
	bool	bIsPlaying();
	virtual bool InitializeOccupationSystem(TBattleGourndModeInfo * pModeInfo) { return false; }
	virtual bool InitializeOccupationMode(TOccupationModeInfo * pInfo) { return false; }

	virtual bool bIsIndividualMode(){ return false; }
	virtual bool bIsAllKillMode(){ return false; }
	//
	virtual void	OnInitializeActor( CDNUserSession* pSession );
	virtual bool	bIsFinishFlag(){ return m_bFinishGameModeFlag; }
	virtual void	OnFinishProcessDie( DnActorHandle hActor ){}
	virtual void	OnCheckFinishRound( PvPCommon::FinishReason::eCode Reason ) = 0;
	virtual void	OnSuccessBreakInto( CDNUserSession* pGameSession );
	virtual bool	bIsCaptainMode(){ return false; }
	virtual bool	bIsZombieMode(){ return false; }
	virtual bool bIsOccupationMode() { return false; }
	virtual bool bIsGuildWarSystem() { return false; }
	virtual bool SetForceWinGuild(UINT nWinGuildDBID) { return false; }
	virtual bool	bIsRoundMode(){ return false; }
	virtual bool bIsRacingMode() { return false; }
	virtual bool bIsTournamentMode() { return false; }
#if defined( PRE_ADD_PVP_COMBOEXERCISE )
	virtual bool bIsComboExerciseMode() { return false; }
#endif // #if defined( PRE_ADD_PVP_COMBOEXERCISE )
#if defined(PRE_ADD_RACING_MODE)
	virtual void SetRacingStart() {return;}
	virtual void SetRacingEnd(UINT nSessionID) {return;}
	virtual BYTE GetRacingRanking(UINT uiSessionID) { return 0;}
#endif

	// InGame
	virtual UINT OnCheckZeroUserWinTeam( const bool bCheckBreakIntoUser=true );
	virtual UINT OnCheckFinishWinTeam();
	virtual void OnRebirth( DnActorHandle hActor, bool bForce=false );
	virtual void OnDie( DnActorHandle hActor, DnActorHandle hHitter );
	virtual void OnDamage( DnActorHandle hActor, DnActorHandle hHitter, const INT64 iDamage );
	virtual void OnCmdAddStateEffect( const CDnSkill::SkillInfo* pSkillInfo );
	virtual void OnLeaveUser( DnActorHandle hActor );
	virtual void OnCmdPvPStartAddStateEffect( DnActorHandle hActor ){}

	virtual bool bIsPlayingUser( DnActorHandle hActor );
	virtual bool bIsFirstRound(){ return false; }

	bool OnTryAcquirePoint(DnActorHandle hActor, int nAreaID, LOCAL_TIME LocalTime);
	virtual bool OnCancelAcquirePoint(DnActorHandle hActor) { return false; }
	virtual bool OnAcquirePoint(int nTeam, const WCHAR * pName, bool bSNatched);

	virtual bool bIsBoss(DnActorHandle hActor) { return false; }
	virtual int OnRecvPvPMessage( LOCAL_TIME LocalTime, CDNUserSession * pSession, int nSubCmd, char *pData, int nLen ) { return ERROR_INVALIDPACKET; }

	virtual UINT GetPvPStartStateEffectDurationTick();
#ifdef PRE_ADD_COLOSSEUM_BEGINNER
	char GetPvPChannelType() { return m_cPvPChannelType; }
#endif		//#ifdef PRE_ADD_COLOSSEUM_BEGINNER

#if defined(PRE_ADD_PVP_TOURNAMENT)
	virtual void SetTournamentUserInfo(MAGAPVP_TOURNAMENT_INFO* pPacket){}
	UINT GetLastWinSessionID() const { return m_uiWinSessionID;}
	virtual BYTE GetTournamentRanking(UINT uiSessionID) { return 0;}
#endif

#if defined( PRE_WORLDCOMBINE_PVP )
	virtual void	SetTournamentUserInfo( int nIndex, CDNUserSession* pGameSession, bool bIgnorePrevData = true ){}
	virtual void SetTournamentUserInfo(){}	
	virtual void SetWorldPvPRoomStart(){}
	virtual void SetWorldPvPFARMRoomStart(){}
	bool    CheckWorldPvPRoomStart( float fDelta );
	bool    CheckWorldPvPRoomMinMemberCount();
	bool    CheckWorldMaxMemberCount();
	UINT	GetSendStartMsgSessionID() { return m_nSendStarMsgUserSessionID; }
	void	SetSendStartMsg( bool bFlag	) { m_bSendStartMsg = bFlag; }
	bool	bIsSendStartMsg() { return m_bSendStartMsg; }	
	bool	bIsWaitStartCount() { return m_bWaitStartCount;	}
	bool	bIsStartSetting() { return m_bStartSetting;	}
	void	SetStartSetting( bool bFlag	) { m_bStartSetting = bFlag; }
#endif

#if defined(PRE_ADD_DWC)
	void	SendFinishDWCGameMode();
#endif
#if defined( PRE_MOD_PVP_ROUNDMODE_PENALTY )
	virtual void GetFinishRoundPenalty( const UINT uiWinTeam, OUT UINT& uiPenaltyPercent ) { uiPenaltyPercent = 0; }
#endif // #if defined( PRE_MOD_PVP_ROUNDMODE_PENALTY )

protected:

	CPvPGameMode( CDNGameRoom* pGameRoom, const TPvPGameModeTable* pPvPGameModeTable, const MAGAPVP_GAMEMODE* pPacket );

	CDNGameRoom*						m_pGameRoom;
	IScoreSystem*						m_pScoreSystem;
#if defined(PRE_ADD_REVENGE)
	CRevengeSystem*						m_pRevengeSystem;
#endif

	UINT								m_uiWinTeam;
	PvPCommon::FinishReason::eCode			m_FinishReason;
	PvPCommon::FinishDetailReason::eCode	m_FinishDetailReason;
	bool								m_bStartGameModeFlag;
	bool								m_bFinishGameModeFlag;
	UINT								m_uiCurRound;
	bool								m_bRoundCountingTrigger;

	const	TPvPGameModeTable*			m_pPvPGameModeTable;
	const	UINT						m_uiWinCondition;
	const	UINT						m_uiPlayTimeSec;
	const	UINT						m_uiGameModeCheck;
	UINT								m_uiVictoryExp;
	UINT								m_uiVictoryBonusRate;
	UINT								m_uiDefeatExp;
	UINT								m_uiDefeatBonusRate;

	float								m_fStartDelta;
	UINT								m_uiStartTick;
	float								m_fTimeOver;
	float								m_fElapsedTimeRound;

	std::vector<IPvPGameModeChecker*>	m_vGameModeChecker;
	std::map<std::wstring,float>		m_mPlayTimeDelta;

#if defined(PRE_ADD_PVP_TOURNAMENT)
	UINT	m_uiWinSessionID;	
#endif

#if defined( PRE_WORLDCOMBINE_PVP )	
	int									m_nStartPlayer;
	bool								m_bSendStartMsg;
	UINT								m_nSendStarMsgUserSessionID;
	float								m_fWorldPvPRoomWaitStartDelta;
	bool								m_bWaitStartCount;
	bool								m_bStartSetting;
#endif

#ifdef PRE_ADD_COLOSSEUM_BEGINNER
	char m_cPvPChannelType;
#endif		//#ifdef PRE_ADD_COLOSSEUM_BEGINNER

#if defined(PRE_ADD_DWC)	
	void	_FinishDWCGameMode( PvPCommon::FinishReason::eCode Reason );	
#endif
	void	_FinishGameMode( CDNUserSession* pGameSession, PvPCommon::FinishReason::eCode Reason );
	void	_ProcessChecker();
	void	_ProcessPlayTime( const float fDelta );
	void	_Create();
	void	_CreateChecker(bool bIsGuildWar);
	void	_SendPVPModeScore( CDNUserSession* pGameSession );
	void	_SetRespawnPosition( DnActorHandle hActor, const bool bIsStartPosition=false );
	void	_SetSkillCoolTimeRule( DnActorHandle hActor );
	void	_SetPvPStartLog();
	void	_OnFinishToUpdatePeriodic( CDNUserSession* pGameSession );
	void	_OnKillPlayer( DnActorHandle hHitter );

	virtual void _OnRefreshHPSP( DnActorHandle hActor );
};
