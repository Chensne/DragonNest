
#pragma once

#include "PvPAllKillModeState.hpp"
#include "DnPlayerActor.h"

class CPvPAllKillModePlayingState : public IPvPAllKillModeState
{
public:

	CPvPAllKillModePlayingState( CPvPAllKillMode* pMode )
		:IPvPAllKillModeState( pMode )
	{
	}
	virtual ~CPvPAllKillModePlayingState(){}

	virtual CPvPAllKillMode::eState GetState(){ return CPvPAllKillMode::Playing; }

	virtual void BeginState()
	{
		_RemoveActiveActorBlowAndToggle( PvPCommon::TeamIndex::A );
		_RemoveActiveActorBlowAndToggle( PvPCommon::TeamIndex::B );
	}

	virtual void BeforeProcess( float fDelta )
	{

	}

	virtual void AfterProcess( float fDelta )
	{
		GetModePtr()->OnCheckFinishRound( PvPCommon::FinishReason::OpponentTeamAllGone );
	}

	virtual void EndState()
	{

	}

	virtual void OnLeaveUser( DnActorHandle hActor )
	{
		if( GetModePtr()->bIsActiveActor( hActor) == false )
			return;

		GetModePtr()->SetActiveActor( PvPCommon::Team2Index(static_cast<PvPCommon::Team::eTeam>(hActor->GetTeam())), CDnActor::Identity() );
		GetModePtr()->OnCheckFinishRound( PvPCommon::FinishReason::OpponentTeamAllGone );
	}

	virtual void OnDie( DnActorHandle hActor, DnActorHandle hHitter )
	{
		GetModePtr()->CPvPGameMode::OnDie( hActor, hHitter );
		GetModePtr()->OnCheckFinishRound( PvPCommon::FinishReason::OpponentTeamAllDead );
	}

	void _RemoveActiveActorBlowAndToggle( PvPCommon::TeamIndex::eCode TeamIndex )
	{
		DnActorHandle hActor = GetModePtr()->GetActiveActor( TeamIndex );
		if( !hActor )
			return;
		hActor->CmdRemoveStateEffect( STATE_BLOW::BLOW_230 );
		hActor->CmdRemoveStateEffect( STATE_BLOW::BLOW_099 );
		if( hActor->IsPlayerActor() )
		{
			CDnPlayerActor* pPlayer = static_cast<CDnPlayerActor*>(hActor.GetPointer());
			pPlayer->CmdToggleBattle( true );
		}
	}
};

