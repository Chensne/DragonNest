#pragma once

#include "MAAiBase.h"
#include "DnActor.h"

class MAAiDefense : public MAAiBase
{
public:
	MAAiDefense( DnActorHandle hActor, MAAiReceiver *pReceiver );
	virtual ~MAAiDefense();

protected:
	int m_nState;
	LOCAL_TIME m_ChangeStateTime;

public:
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
};