#pragma once

#include "EtTriggerElement.h"

class CDnTriggerElement : public CEtTriggerElement
{
public:
	CDnTriggerElement( CEtTriggerObject *pObject );
	virtual ~CDnTriggerElement();

protected:
	virtual void RegisterLuaApi();

public:
	virtual bool CallLuaFunction();
};