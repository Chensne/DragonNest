#pragma once
#include "DnPropCondition.h"

class CDnPropHitCondition : public CDnPropCondition, public TBoostMemoryPool< CDnPropHitCondition >
{
public:
	CDnPropHitCondition( DnPropHandle hEntity );
	virtual ~CDnPropHitCondition(void);

	virtual bool IsSatisfy( void );
};
