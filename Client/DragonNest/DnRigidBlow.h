#pragma once
#include "DnBlow.h"

class CDnRigidBlow : public CDnBlow
{
public:
	CDnRigidBlow();
	virtual ~CDnRigidBlow(void);

public:
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
};
