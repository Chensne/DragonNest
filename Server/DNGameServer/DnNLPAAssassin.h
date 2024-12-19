#pragma once

#ifdef PRE_ADD_ASSASSIN

#include "DnPlayerActor.h"
#include "TDnPlayerAssassin.h"

class CDnNLPAAssassin : public TDnPlayerAssassin< CDnPlayerActor >, public TBoostMemoryPool< CDnNLPAAssassin >
{
public:
	CDnNLPAAssassin( CMultiRoom *pRoom, int nClassID );
	virtual ~CDnNLPAAssassin();

protected:
	float m_fPitchDelta;
	float m_fTargetPitchAngle;
	float m_fCurrentPitchAngle;

public:
	// Actor Message
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );

	// TransAction Message
	virtual void OnDispatchMessage( CDNUserSession *pSession, DWORD dwActorProtocol, BYTE *pPacket );
};

#endif // #ifdef PRE_ADD_ASSASSIN