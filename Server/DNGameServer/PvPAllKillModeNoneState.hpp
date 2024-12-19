
#pragma once

#include "PvPAllKillModeState.hpp"
#include "DNPvPGameRoom.h"

class CPvPAllKillModeNoneState : public IPvPAllKillModeState
{
public:

	CPvPAllKillModeNoneState( CPvPAllKillMode* pMode )
		:IPvPAllKillModeState( pMode )
	{

	}
	virtual ~CPvPAllKillModeNoneState(){}

	virtual CPvPAllKillMode::eState GetState(){ return CPvPAllKillMode::None; }

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

	virtual UINT GetRemainStateTick()
	{ 
		if( GetPvPGameRoom()->GetRoomOptionBit()&PvPCommon::RoomOption::AllKill_RandomOrder )
			return 0;
		return PvPCommon::AllKillMode::Time::SelectPlayerTick;
	}
};

