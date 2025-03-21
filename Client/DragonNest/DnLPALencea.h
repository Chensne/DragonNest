#pragma once

#include "DnLocalPlayerActor.h"
#include "TDnPlayerLencea.h"

class CDnLPALencea : public TDnPlayerLencea< CDnLocalPlayerActor >
{
public:
	CDnLPALencea( int nClassID, bool bProcess = true );
	virtual ~CDnLPALencea();

protected:

public:
	// Actor Message
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
};