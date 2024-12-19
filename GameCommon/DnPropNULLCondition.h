#pragma once
#include "DnPropCondition.h"

class CDnPropNULLCondition : public CDnPropCondition, public TBoostMemoryPool< CDnPropNULLCondition >
{
public:
	CDnPropNULLCondition( DnPropHandle hEntity );
	virtual ~CDnPropNULLCondition(void);

	virtual bool IsSatisfy( void );
};
	