#include "StdAfx.h"
#include "DnStateCondition.h"

CDnStateCondition::CDnStateCondition()
{
	m_StateCondition = StateConditionEnum::Unknown;
	m_fLifeDelta = 0;
	m_fDelta = 0.f;
	m_fValue = 0.f;
}

CDnStateCondition::~CDnStateCondition()
{
}

void CDnStateCondition::Initialize( StateConditionEnum StateCondition, ValueTypeEnum Type, int nTime, float fValue )
{
	m_StateCondition = StateCondition;
	m_ValueType = Type;
	m_fLifeDelta = nTime / 1000.f;
	m_fValue = fValue;
}
