#pragma once

#if defined(PRE_ADD_ACADEMIC)

#include "DnPlayerActor.h"
#include "TDnPlayerAcademic.h"

class CDnNLPAAcademic : public TDnPlayerAcademic< CDnPlayerActor >, public TBoostMemoryPool< CDnNLPAAcademic >
{
public:
	CDnNLPAAcademic( CMultiRoom *pRoom, int nClassID );
	virtual ~CDnNLPAAcademic();

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

#endif	// #if defined(PRE_ADD_ACADEMIC)