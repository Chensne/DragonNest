#pragma once
#include "TDnFSMState.h"

//#define ENABLE_PROP_STATE_LOG


class CDnPropState : public TDnFSMState<DnPropHandle>
{
public:
	enum
	{
		NORMAL,
		DO_ACTION,
		ITEM_DROP,
		TRIGGER,
		CMD_OPERATION,
		SCAN_ACTOR,
		COUNT,
	};

protected:
	int m_iType;

public:
	CDnPropState( DnPropHandle hEntity );
	virtual ~CDnPropState(void);

	static CDnPropState* Create( DnPropHandle hEntity, int iType );

	int GetType( void ) { return m_iType; };
};
	