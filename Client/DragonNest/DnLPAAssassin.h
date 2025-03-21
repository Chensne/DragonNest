#pragma once

#ifdef PRE_ADD_ASSASSIN

#include "DnLocalPlayerActor.h"
#include "TDnPlayerAssassin.h"

class CDnLPAAssassin : public TDnPlayerAssassin< CDnLocalPlayerActor >
{
public:
	CDnLPAAssassin( int nClassID, bool bProcess = true );
	virtual ~CDnLPAAssassin();

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

#endif // #ifdef PRE_ADD_ASSASSIN