
#pragma once

#include "PvPAllKillModeState.hpp"
#include "DNPvPGameRoom.h"

class CPvPAllKillModeFinishRoundState : public IPvPAllKillModeState
{
public:

	CPvPAllKillModeFinishRoundState( CPvPAllKillMode* pMode )
		:IPvPAllKillModeState( pMode )
	{

	}
	virtual ~CPvPAllKillModeFinishRoundState(){}

	virtual CPvPAllKillMode::eState GetState(){ return CPvPAllKillMode::FinishRound; }

	virtual void BeginState()
	{
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

