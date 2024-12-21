#pragma once
#include "DnPropCondition.h"

class CDnWorldActProp;

// 프랍의 서브 내구도를 비교해 전이 시킴.. MultiDurabilityProp 에서만 사용됨.
class CDnPropCondiSubDurability : public CDnPropCondition, 
								  public TBoostMemoryPool<CDnPropCondiSubDurability>
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
	int m_iDurabilityIndex;
	int m_iOperation;

	CDnWorldActProp* m_pProp;

public:
	CDnPropCondiSubDurability( DnPropHandle hEntity );
	virtual ~CDnPropCondiSubDurability( void );

	void Initialize( int iStandardDurability, int iDurabilityIndex, int iOperation );
	virtual bool IsSatisfy( void );
};
