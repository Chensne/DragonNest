#pragma once
#include "DnPropCondition.h"

class CDnPropOperatedCondition : public CDnPropCondition, public TBoostMemoryPool< CDnPropOperatedCondition >
{
private:
	int m_iRequireOperateState;


public:
	CDnPropOperatedCondition( DnPropHandle hEntity );
	virtual ~CDnPropOperatedCondition(void);

	void SetRequireState( int iRequireOperateState ) { m_iRequireOperateState = iRequireOperateState; };

	virtual bool IsSatisfy( void );
};
