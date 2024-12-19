
#pragma once

#include "PvPAllKillModeState.hpp"

class CPvPAllKillModeStartingState : public IPvPAllKillModeState
{
public:

	CPvPAllKillModeStartingState( CPvPAllKillMode* pMode )
		:IPvPAllKillModeState( pMode )
	{
	}
	virtual ~CPvPAllKillModeStartingState(){}

	virtual CPvPAllKillMode::eState GetState(){ return CPvPAllKillMode::Starting; }

	virtual void BeginState()
	{
		GetModePtr()->SendBattleActor();
		GetModePtr()->SetActiveActorStartPosition();
	}

	virtual void BeforeProcess( float fDelta )
	{

	}

	virtual void AfterProcess( float fDelta )
	{

	}

	virtual void EndState()
	{

	}

	virtual void OnLeaveUser( DnActorHandle hActor )
	{

	}

	virtual void OnDie( DnActorHandle hActor, DnActorHandle hHitter )
	{

	}
};

