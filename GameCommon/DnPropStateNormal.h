#pragma once
#include "DnPropState.h"

// DnWorldActProp 의 포인터만 받는다...
class CDnPropStateNormal : public CDnPropState, public TBoostMemoryPool< CDnPropStateNormal >
{	
private:
	

public:
	CDnPropStateNormal( DnPropHandle hEntity );
	virtual ~CDnPropStateNormal(void);

	virtual bool OnEntry( LOCAL_TIME LocalTime, float fDelta );
	virtual bool OnProcess( LOCAL_TIME LocalTime, float fDelta );
	virtual bool OnLeave( LOCAL_TIME LocalTime, float fDelta );
};
