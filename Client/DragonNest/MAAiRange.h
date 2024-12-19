#pragma once

#include "MAAiBase.h"
#include "DnActor.h"

class MAAiRange : public MAAiBase
{
public:
	MAAiRange( DnActorHandle hActor, MAAiReceiver *pReceiver );
	virtual ~MAAiRange();

protected:
	int m_nState;
	LOCAL_TIME m_ChangeStateTime;

public:
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
};
