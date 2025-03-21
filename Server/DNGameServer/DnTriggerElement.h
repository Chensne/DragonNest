#pragma once

#include "EtTriggerElement.h"

class CDnTriggerElement : public CEtTriggerElement, public TBoostMemoryPool< CDnTriggerElement >
{
public:
	CDnTriggerElement( CEtTriggerObject *pObject );
	virtual ~CDnTriggerElement();

protected:
	virtual void RegisterLuaApi();

protected:

public:
	virtual bool CallLuaFunction();
};