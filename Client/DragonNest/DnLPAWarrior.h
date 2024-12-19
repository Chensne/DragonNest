#pragma once

#include "DnLocalPlayerActor.h"
#include "TDnPlayerWarrior.h"

class CDnLPAWarrior : public TDnPlayerWarrior< CDnLocalPlayerActor >
{
public:
	CDnLPAWarrior( int nClassID, bool bProcess = true );
	virtual ~CDnLPAWarrior();

protected:

public:
	// Actor Message
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
};