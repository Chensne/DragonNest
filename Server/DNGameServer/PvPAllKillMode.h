
#pragma once

#include "PvPRoundMode.h"
#include "DnActor.h"

class IPvPAllKillModeState;

#if defined( PRE_ADD_PVP_TOURNAMENT )
class CPvPAllKillMode:public CPvPRoundMode
#else
class CPvPAllKillMode:public CPvPRoundMode,public TBoostMemoryPool<CPvPAllKillMode>
#endif // #if defined( PRE_ADD_PVP_TOURNAMENT )
{
public:
	enum eState
	{
		None = 0,
		SelectPlayer,
		Starting,
		Playing,
		FinishRound,
	};
	CPvPAllKillMode( CDNGameRoom* pGameRoom, const TPvPGameModeTable* pPvPGameModeTable, const MAGAPVP_GAMEMODE* pPacket );

	virtual ~CPvPAllKillMode();

	virtual bool	bIsAllKillMode(){ return true; }
	virtual int		OnRecvPvPMessage( LOCAL_TIME LocalTime, CDNUserSession * pSession, int nSubCmd, char *pData, int nLen );
	virtual void	Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void	OnCheckFinishRound( PvPCommon::FinishReason::eCode Reason );
	virtual void	OnFinishRound();
	virtual void	OnDie( DnActorHandle hActor, DnActorHandle hHitter );
	virtual void	OnLeaveUser( DnActorHandle hActor );
	virtual void	OnInitializeActor( CDNUserSession* pSession );
	virtual UINT	GetPvPStartStateEffectDurationTick();
	virtual void	OnCmdPvPStartAddStateEffect( DnActorHandle hActor );
	virtual void	OnSuccessBreakInto( CDNUserSession* pGameSession );

	virtual void ChangeState( eState State );
	void SetActiveActorStartPosition();
	DnActorHandle GetPreWinActor();
	
	void SetGroupCaptainActor( PvPCommon::TeamIndex::eCode Team, DnActorHandle hActor, bool bSend=false );
	DnActorHandle GetGroupCaptainActor( PvPCommon::TeamIndex::eCode Team );
	void ClearGroupCaptain( DnActorHandle hActor );
	void CalcGroupCaptain( bool bSend, DnActorHandle hIgnoreActor=CDnActor::Identity() );
	bool bIsGroupCaptain( DnActorHandle hActor );
	void SendGroupCaptain();

	void SetActiveActor( PvPCommon::TeamIndex::eCode Team, DnActorHandle hActor, bool bSend=false );
	DnActorHandle GetActiveActor( PvPCommon::TeamIndex::eCode Team );
	void ClearActiveActor( DnActorHandle hActor );
	void CalcActiveActor( bool bSend, DnActorHandle hIgnoreActor=CDnActor::Identity() );
	bool bIsActiveActor( DnActorHandle hActor );
	void SendActiveActor();

	void SendBattleActor(CDNUserSession* pBreakIntoUserSession=NULL);

#if defined( PRE_WORLDCOMBINE_PVP )
	virtual void SetWorldPvPRoomStart();
#endif
protected:
	IPvPAllKillModeState* m_pState;

	virtual void	_OnRefreshHPSP( DnActorHandle hActor );
	virtual void	_OnAfterProcess();
	virtual bool	_CanProcess();
	virtual bool	_CheckFinishGameMode( UINT& uiWinTeam );
	virtual UINT	_GetPvPRoundStartStateEffectDurationTick();
	virtual bool	_CheckWinContition(){ return false; }
	virtual void	_ProcessFinishRoundMode( CDNUserSession* pSession, bool bIsWin, UINT uiWinTeam, PvPCommon::FinishReason::eCode Reason );

	virtual void _BeforeProcess( LOCAL_TIME LocalTime, float fDelta );
	virtual void _AfterProcess( LOCAL_TIME LocalTime, float fDelta );
	
	void _SendGroupCaptain( PvPCommon::TeamIndex::eCode TeamIndex );
	void _SendActiveActor( PvPCommon::TeamIndex::eCode TeamIndex );

private:	

	DnActorHandle m_hGroupCaptainActor[PvPCommon::TeamIndex::Max];
	DnActorHandle m_hActiveActor[PvPCommon::TeamIndex::Max];
};
