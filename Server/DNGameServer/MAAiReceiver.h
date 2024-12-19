#pragma once

#include "Timer.h"
#include "DnActor.h"

class MAAiBase;

class MAAiReceiver
{
public:
	MAAiReceiver();
	virtual ~MAAiReceiver();

	enum AIState {
		Disable,
		Gentle,
		Threat,
	};


protected:
	DnActorHandle m_hActor;
	MAAiBase* m_pAi;
	AIState m_AIState;

public:
	bool Initialize( int nMonsterTableID, const char *szLuaFileName );
	void Process( LOCAL_TIME LocalTime, float fDelta );

	void SetAggroTarget( DnActorHandle hActor, int nAggroValue = 0 );
	DnActorHandle GetAggroTarget();

	DnActorHandle GetReservedProjectileTarget( void );

	MAAiBase *GetAIBase() { return m_pAi; }

	void SetAIState( AIState State, bool bRefreshAggro = true );
	AIState GetAIState() { return m_AIState; }

};
