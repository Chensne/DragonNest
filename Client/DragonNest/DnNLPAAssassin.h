#pragma once

#if defined( PRE_ADD_ASSASSIN )

#include "DnPlayerActor.h"
#include "TDnPlayerAssassin.h"

class CDnNLPAAssassin : public TDnPlayerAssassin< CDnPlayerActor >
{
public:
	CDnNLPAAssassin( int nClassID, bool bProcess = true );
	virtual ~CDnNLPAAssassin();

protected:
	float m_fPitchDelta;
	float m_fTargetPitchAngle;
	float m_fCurrentPitchAngle;

public:
	// Actor Message
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );

	// TransAction Message
	virtual void OnDispatchMessage( DWORD dwActorProtocol, BYTE *pPacket );
};

#endif	// #if defined( PRE_ADD_ASSASSIN )