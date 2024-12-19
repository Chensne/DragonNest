#pragma once

#include "DnDamageBase.h"
class MADamageBase : virtual public CDnDamageBase
{
public:
	MADamageBase() { SetDamageObjectType( DamageObjectTypeEnum::Actor ); }
	virtual ~MADamageBase() {}

protected:

public:
};