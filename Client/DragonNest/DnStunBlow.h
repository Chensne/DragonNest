#pragma once
#include "DnBlow.h"

class CDnStunBlow : public CDnBlow
{
public:
	CDnStunBlow();
	virtual ~CDnStunBlow(void);

public:
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
};
