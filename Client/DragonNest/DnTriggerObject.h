#pragma once

#include "EtTriggerObject.h"

class CDnTriggerObject : public CEtTriggerObject
{
public:
	CDnTriggerObject( CEtTrigger *pTrigger );
	virtual ~CDnTriggerObject();

protected:

public:
	virtual CEtTriggerElement *AllocTriggerElement();

};