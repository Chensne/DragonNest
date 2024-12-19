
#pragma once

#include "PvPAllKillModeState.hpp"
#include "DNPvPGameRoom.h"

class CPvPAllKillModeSelectPlayerState : public IPvPAllKillModeState
{
public:

	CPvPAllKillModeSelectPlayerState( CPvPAllKillMode* pMode )
		:IPvPAllKillModeState( pMode )
	{

	}
	virtual ~CPvPAllKillModeSelectPlayerState(){}

	virtual CPvPAllKillMode::eState GetState(){ return CPvPAllKillMode::SelectPlayer; }

	virtual void BeginState()
	{
#if defined(PRE_ADD_PVP_TOURNAMENT)
		if( GetModePtr()->bIsTournamentMode() )
			return;
#endif
		DnActorHandle hATeamActive = GetModePtr()->GetActiveActor( PvPCommon::TeamIndex::A );
		DnActorHandle hBTeamActive = GetModePtr()->GetActiveActor( PvPCommon::TeamIndex::B );
		DnActorHandle hPreWinActor = GetModePtr()->GetPreWinActor();
		
		if( hPreWinActor )
		{
			if( hATeamActive != hPreWinActor )
				GetModePtr()->SetActiveActor( PvPCommon::TeamIndex::A, CDnActor::Identity() );
			if( hBTeamActive != hPreWinActor )
				GetModePtr()->SetActiveActor( PvPCommon::TeamIndex::B, CDnActor::Identity() );
		}
		else
		{
			GetModePtr()->SetActiveActor( PvPCommon::TeamIndex::A, CDnActor::Identity() );
			GetModePtr()->SetActiveActor( PvPCommon::TeamIndex::B, CDnActor::Identity() );
		}

		GetPvPGameRoom()->SendAllKillShowSelectPlayer();
		GetModePtr()->CalcGroupCaptain( false );
		GetModePtr()->SendGroupCaptain();
		GetModePtr()->CalcActiveActor( false );
		GetModePtr()->SendActiveActor();
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
		if( !hActor )
			return;

		// GroupCaptain
		if( GetModePtr()->bIsGroupCaptain( hActor ) == true )
		{
			GetModePtr()->ClearGroupCaptain( hActor );
			GetModePtr()->CalcGroupCaptain( true, hActor );
		}

		// ActiveActor
		if( GetModePtr()->bIsActiveActor( hActor ) == true )
		{
			GetModePtr()->ClearActiveActor( hActor );
			GetModePtr()->CalcActiveActor( true, hActor );
		}
	}

	virtual void OnDie( DnActorHandle hActor, DnActorHandle hHitter )
	{

	}

	virtual UINT GetRemainStateTick()
	{ 
		int iTick = PvPCommon::AllKillMode::Time::SelectPlayerTick - GetElapsedTick();
		if( iTick > 0 )
			return iTick;
		return 0; 
	}
};
