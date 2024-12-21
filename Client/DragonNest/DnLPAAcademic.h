#pragma once

#if defined(PRE_ADD_ACADEMIC)

#include "DnLocalPlayerActor.h"
#include "TDnPlayerAcademic.h"

class CDnLPAAcademic : public TDnPlayerAcademic< CDnLocalPlayerActor >
{
public:
	CDnLPAAcademic( int nClassID, bool bProcess = true );
	virtual ~CDnLPAAcademic();

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

#endif	// #if defined(PRE_ADD_ACADEMIC)