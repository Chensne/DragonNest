#pragma once

#include "DnPlayerActor.h"
#include "TDnPlayerArcher.h"

class CDnNLPAArcher : public TDnPlayerArcher< CDnPlayerActor >, public TBoostMemoryPool< CDnNLPAArcher >
{
public:
	CDnNLPAArcher( CMultiRoom *pRoom, int nClassID );
	virtual ~CDnNLPAArcher();

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