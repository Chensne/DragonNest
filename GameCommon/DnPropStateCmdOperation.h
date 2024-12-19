#pragma once
#include "DnPropState.h"

class CDnPropStateCmdOperation : public CDnPropState, public TBoostMemoryPool< CDnPropStateCmdOperation >
{
protected:
	int m_iOperateIDToSend;

public:
	CDnPropStateCmdOperation( DnPropHandle hEntity );
	virtual ~CDnPropStateCmdOperation(void);

	void SetOperateIDToSend( int iID ) { m_iOperateIDToSend = iID; };
	
	virtual bool OnEntry( LOCAL_TIME LocalTime, float fDelta );
	virtual bool OnProcess( LOCAL_TIME LocalTime, float fDelta );
	virtual bool OnLeave( LOCAL_TIME LocalTime, float fDelta );
};
