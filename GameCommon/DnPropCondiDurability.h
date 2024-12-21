#pragma once
#include "DnPropCondition.h"


// 프랍의 내구도와 비교하는 조건 비교 객체,, 현재는 Broken Prop 에만 해당됨.
// 프랍의 내구도 [operation] Standard value 형식임.
class CDnPropCondiDurability : public CDnPropCondition, public TBoostMemoryPool< CDnPropCondiDurability >
{
public:
	enum
	{
		EQUAL,				// ==
		GREATER,			// >
		LESS,				// <
		GREATER_EQUAL,		// >=
		LESS_EQUAL,			// <=
		OPERATION_COUNT,
	};

private:
	int m_iStandardDurability;
	int m_iOperation;

public:
	CDnPropCondiDurability( DnPropHandle hEntity );
	virtual ~CDnPropCondiDurability(void);

	void Initialize( int iStandardDurability, int iOperation );

	virtual bool IsSatisfy( void );
};
