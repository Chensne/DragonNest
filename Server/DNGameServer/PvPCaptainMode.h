
#pragma once

#include "PvPRoundMode.h"

class CPvPCaptainMode:public CPvPRoundMode,public TBoostMemoryPool<CPvPCaptainMode>
{
public:

	CPvPCaptainMode( CDNGameRoom* pGameRoom, const TPvPGameModeTable* pPvPGameModeTable, const MAGAPVP_GAMEMODE* pPacket );
	virtual ~CPvPCaptainMode();

	virtual void	Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void	OnFinishProcessDie( DnActorHandle hActor );
	virtual void	OnCheckFinishRound( PvPCommon::FinishReason::eCode Reason );
	virtual void	OnFinishRound();
	virtual void	OnDie( DnActorHandle hActor, DnActorHandle hHitter );
	virtual void	OnLeaveUser( DnActorHandle hActor );
	virtual void	OnSuccessBreakInto( CDNUserSession* pGameSession );
	virtual bool	bIsCaptainMode(){ return true; }

public:

	bool	bIsCaptain( DnActorHandle hActor );

protected:

	void	_AddCaptainBlow( DnActorHandle hActor );
	void	_ProcessSelectCaptain();
	void	_UpdateSelectCaptainCount( UINT uiSessionID );
	UINT	_GetSelectCaptainCount( UINT uiSessionID );
	void	_GetCaptainCandidate( PvPCommon::Team::eTeam Team, std::vector<DnActorHandle>& vResult );

	DnActorHandle		m_hCaptain[PvPCommon::TeamIndex::Max];
	std::map<UINT,UINT>	m_mSelectCaptainCount;	// SessionID/SlectCaptainCount
};
