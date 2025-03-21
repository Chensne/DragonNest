#pragma once

#include "EtTriggerObject.h"

class CDnTriggerObject : public CEtTriggerObject, public TBoostMemoryPool< CDnTriggerObject >
{
public:
	CDnTriggerObject( CEtTrigger *pTrigger );
	virtual ~CDnTriggerObject();

protected:

#if defined( PRE_TRIGGER_LOG )
	CDNGameRoom*	m_pGameRoom;
	bool			m_bLog;

	LARGE_INTEGER	m_liFrequency;
	LARGE_INTEGER	m_liStartTime;
	LARGE_INTEGER	m_liCurTime;
#endif // #if defined( PRE_TRIGGER_LOG )

public:
	virtual CEtTriggerElement *AllocTriggerElement();

#if defined( PRE_TRIGGER_LOG )
	virtual bool Process( LOCAL_TIME LocalTime, float fDelta );
	virtual bool ProcessQueue( LOCAL_TIME LocalTime, float fDelta );
#endif // #if defined( PRE_TRIGGER_LOG )

};