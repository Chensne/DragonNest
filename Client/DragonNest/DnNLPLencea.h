#pragma once
/*
#if defined(PRE_ADD_ACADEMIC)

#include "DnPlayerActor.h"
#include "TDnPlayerLencea.h"
class CDnNLPALencea : public TDnPlayerLencea< CDnPlayerActor >
{
public:
	CDnNLPALencea( int nClassID, bool bProcess = true );
	virtual ~CDnNLPALencea();

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

#endif	// #if defined(PRE_ADD_ACADEMIC) */