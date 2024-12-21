
#pragma once

#include "PvPGameMode.h"

class CPvPRoundMode:public CPvPGameMode
{
public:

	CPvPRoundMode( CDNGameRoom* pGameRoom, const TPvPGameModeTable* pPvPGameModeTable, const MAGAPVP_GAMEMODE* pPacket );
	virtual ~CPvPRoundMode();

	virtual void	Process( LOCAL_TIME LocalTime, float fDelta );

	virtual bool	bIsFinishFlag(){ return m_bFinishRoundFlag; }
	virtual bool	bIsPlayingUser( DnActorHandle hActor );
	virtual bool	bIsFirstRound();
	virtual bool	bIsRoundMode(){ return true; }

	virtual void	OnFinishProcessDie( DnActorHandle hActor );
	virtual void	OnCheckFinishRound( PvPCommon::FinishReason::eCode Reason );
	virtual void	OnFinishRound(){}
	virtual void	OnSuccessBreakInto( CDNUserSession* pGameSession );
	virtual void	OnRebirth( DnActorHandle hActor, bool bForce=false );
	virtual void	OnDie( DnActorHandle hActor, DnActorHandle hHitter );
	virtual void	OnLeaveUser( DnActorHandle hActor );
#if defined( PRE_MOD_PVP_ROUNDMODE_PENALTY )
	virtual void	GetFinishRoundPenalty( const UINT uiWinTeam, OUT UINT& uiPenaltyPercent );
#endif // #if defined( PRE_MOD_PVP_ROUNDMODE_PENALTY )

protected:

	virtual void	_OnStartRound();
	virtual void	_ProcessFinishRoundMode( CDNUserSession* pSession, bool bIsWin, UINT uiWinTeam, PvPCommon::FinishReason::eCode Reason ){}
	virtual void	_OnAfterProcess();
	virtual void	_OnStartRoundStartPosition( DnActorHandle hActor );
	virtual bool	_CanProcess(){ return true; }
	virtual bool	_CheckFinishGameMode( UINT& uiWinTeam ){ return false; }
	virtual UINT	_GetPvPRoundStartStateEffectDurationTick(){ return PvPCommon::Common::GameModeStartDelta*1000; }
	virtual bool	_CheckWinContition(){ return true; }

	void			_FinishRound( const UINT uiWinTeam, PvPCommon::FinishReason::eCode Reason );
	void			_ProcessFinishRound( const UINT uiWinTeam, PvPCommon::FinishReason::eCode Reason );

	bool	m_bFinishRoundFlag;
	float	m_fFinishRoundDelta;
	DWORD	m_dwCheckAllDeadTick;
#if defined(PRE_ADD_PVP_TOURNAMENT)	
	char	m_cTournamentStep;			// 몇강?
	char	m_cTournamentStepCount;	// 몇번째 방?	
	char	m_cCurrentTournamentStep;	// 현재 몇강인지..
#endif
	std::map<std::wstring,CDNUserSession*>	m_mBreakInto;
};

class IBoostPoolPvPRoundMode:public CPvPRoundMode, public TBoostMemoryPool<IBoostPoolPvPRoundMode>
{
public :
	IBoostPoolPvPRoundMode( CDNGameRoom* pGameRoom, const TPvPGameModeTable* pPvPGameModeTable, const MAGAPVP_GAMEMODE* pPacket ):CPvPRoundMode(pGameRoom,pPvPGameModeTable,pPacket){}
	virtual ~IBoostPoolPvPRoundMode(){}
};
