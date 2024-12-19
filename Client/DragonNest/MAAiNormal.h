#pragma once

#include "MAAiBase.h"
#include "DnActor.h"

class MAAiNormal : public MAAiBase
{
public:
	MAAiNormal( DnActorHandle hActor, MAAiReceiver *pReceiver );
	virtual ~MAAiNormal();

protected:
	int m_nState;
	LOCAL_TIME m_ChangeStateTime;

public:
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
};

