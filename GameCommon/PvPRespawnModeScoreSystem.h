
#pragma once

#include "PvPGameModeScoreSystem.h"

class CPvPRespawnModeScoreSystem:public IPVPGameModeScoreSystem,public TBoostMemoryPool<CPvPRespawnModeScoreSystem>
{
public:
	
	CPvPRespawnModeScoreSystem(){}
	virtual ~CPvPRespawnModeScoreSystem(){}

	virtual void OnDie( DnActorHandle hActor, DnActorHandle hHitter );
};
