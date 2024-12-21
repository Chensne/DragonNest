#pragma once

#ifdef PRE_ADD_KALI

#include "DnPlayerActor.h"
#include "TDnPlayerKali.h"
class CDnNLPAKali : public TDnPlayerKali< CDnPlayerActor >
{
public:
	CDnNLPAKali( int nClassID, bool bProcess = true );
	virtual ~CDnNLPAKali();

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

#endif	// #ifdef PRE_ADD_KALI