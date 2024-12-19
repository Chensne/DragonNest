#pragma once

#ifdef PRE_ADD_KALI

#include "DnLocalPlayerActor.h"
#include "TDnPlayerKali.h"

class CDnLPAKali : public TDnPlayerKali< CDnLocalPlayerActor >
{
public:
	CDnLPAKali( int nClassID, bool bProcess = true );
	virtual ~CDnLPAKali();

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

#endif // #ifdef PRE_ADD_KALI