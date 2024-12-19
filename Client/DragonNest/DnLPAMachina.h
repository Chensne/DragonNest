#pragma once

#include "DnLocalPlayerActor.h"
#include "TDnPlayerMachina.h"

class CDnLPAMachina : public TDnPlayerMachina< CDnLocalPlayerActor >
{
public:
	CDnLPAMachina( int nClassID, bool bProcess = true );
	virtual ~CDnLPAMachina();

protected:

public:
	// Actor Message
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
};