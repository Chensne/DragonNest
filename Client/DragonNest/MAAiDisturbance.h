#pragma once

#include "MAAiBase.h"
#include "DnActor.h"

class MAAiDisturbance : public MAAiBase
{
public:
	MAAiDisturbance( DnActorHandle hActor, MAAiReceiver *pReceiver );
	virtual ~MAAiDisturbance();

protected:
	int m_nState;
	LOCAL_TIME m_ChangeStateTime;

public:
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
};