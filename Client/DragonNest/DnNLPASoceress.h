#pragma once

#include "DnPlayerActor.h"
#include "TDnPlayerSoceress.h"
class CDnNLPASoceress : public TDnPlayerSoceress< CDnPlayerActor >
{
public:
	CDnNLPASoceress( int nClassID, bool bProcess = true );
	virtual ~CDnNLPASoceress();

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