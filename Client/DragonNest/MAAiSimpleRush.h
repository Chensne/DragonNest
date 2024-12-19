#pragma once

#include "MAAiBase.h"
#include "DnActor.h"

class MAAiSimpleRush : public MAAiBase
{
public:
	MAAiSimpleRush( DnActorHandle hActor, MAAiReceiver *pReceiver );
	virtual ~MAAiSimpleRush();

protected:
	int m_nState;
	LOCAL_TIME m_ChangeStateTime;

public:
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
};