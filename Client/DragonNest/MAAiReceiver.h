#pragma once

#include "Timer.h"
#include "DnActor.h"

class MAAiBase;

class MAAiReceiver
{
public:
	MAAiReceiver();
	virtual ~MAAiReceiver();

protected:
	DnActorHandle m_hActor;
	MAAiBase* m_pAi;

public:
	bool Initialize( int nMonsterTableID );
	void Process( LOCAL_TIME LocalTime, float fDelta );

	void SetAggroTarget( DnActorHandle hActor );
	DnActorHandle GetAggroTarget();

	MAAiBase *GetAIBase() { return m_pAi; }
};
