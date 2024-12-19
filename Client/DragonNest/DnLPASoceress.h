#pragma once

#include "DnLocalPlayerActor.h"
#include "TDnPlayerSoceress.h"

class CDnLPASoceress : public TDnPlayerSoceress< CDnLocalPlayerActor > {
public:
	CDnLPASoceress( int nClassID, bool bProcess = true );
	virtual ~CDnLPASoceress();

protected:
	float m_fPrevPitchAngle;
	LOCAL_TIME m_LastSendPitchMsg;

	LOCAL_TIME m_ChargeTime;

protected:
	// LocalPCActor Message
	virtual void ProcessInput( LOCAL_TIME LocalTime, float fDelta );

public:
	// Actor Message
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );

	virtual bool IsCanToggleBattleMode();
	virtual void ResetActor();
};