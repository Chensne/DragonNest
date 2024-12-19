#pragma once

#include "DnLocalPlayerActor.h"
#include "TDnPlayerCleric.h"

class CDnLPACleric : public TDnPlayerCleric< CDnLocalPlayerActor >
{
public:
	CDnLPACleric( int nClassID, bool bProcess = true );
	virtual ~CDnLPACleric();

protected:

public:
	// Actor Message
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
};