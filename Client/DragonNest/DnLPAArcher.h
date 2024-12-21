#pragma once

#include "DnLocalPlayerActor.h"
#include "TDnPlayerArcher.h"

class CDnLPAArcher : public TDnPlayerArcher< CDnLocalPlayerActor >
{
public:
	CDnLPAArcher( int nClassID, bool bProcess = true );
	virtual ~CDnLPAArcher();

protected:
	float m_fPrevPitchAngle;
	LOCAL_TIME m_LastSendPitchMsg;

protected:
	// LocalPCActor Message
	virtual void ProcessInput( LOCAL_TIME LocalTime, float fDelta );

public:
	// Actor Message
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );

	virtual void ResetActor();
};