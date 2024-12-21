#pragma once
#include "DnBlow.h"

class CDnAttackRateUpBlow : public CDnBlow
{
public:
	CDnAttackRateUpBlow(void);
	virtual ~CDnAttackRateUpBlow(void);

public:
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
};
