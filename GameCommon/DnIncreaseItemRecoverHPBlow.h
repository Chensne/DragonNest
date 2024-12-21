#pragma once
#include "dnblow.h"

class CDnIncreaseItemRecoverHPBlow : public CDnBlow, public TBoostMemoryPool< CDnIncreaseItemRecoverHPBlow >
{
public:
	CDnIncreaseItemRecoverHPBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnIncreaseItemRecoverHPBlow(void);

	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

};
