
#pragma once

#include "PvPGameModeScoreSystem.h"

class CPvPRoundModeScoreSystem:public IPVPGameModeScoreSystem
{
public:

	CPvPRoundModeScoreSystem(){}
	virtual ~CPvPRoundModeScoreSystem(){}

	virtual void OnDie( DnActorHandle hActor, DnActorHandle hHitter );
};

class IBoostPoolPvPRoundModeScoreSystem : public CPvPRoundModeScoreSystem, public TBoostMemoryPool<IBoostPoolPvPRoundModeScoreSystem>
{
public :
	//
};
