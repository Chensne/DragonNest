
#pragma once

#include "SecondarySkill.h"

#if defined( PRE_ADD_SECONDARY_SKILL )

//========================================================================
// ���꽺ų
//========================================================================

class CProductionSkill:public CSecondarySkill
{
public:

	CProductionSkill( const int iSkillID, SecondarySkill::ExecuteType::eType ExecuteType )
		:CSecondarySkill( iSkillID, ExecuteType )
	{
	}
	virtual ~CProductionSkill(){}

	virtual SecondarySkill::Type::eType		GetType(){ return SecondarySkill::Type::ProductionSkill; }
	virtual	int								GetCurrentRecipeCount(){ return 0; }
};

//========================================================================
// ���꽺ų-���
//========================================================================

class CCultivationSkill:public CProductionSkill,public TBoostMemoryPool<CCultivationSkill>
{
public:

	CCultivationSkill( const int iSkillID, SecondarySkill::ExecuteType::eType ExecuteType )
		:CProductionSkill( iSkillID, ExecuteType )
	{
	}
	virtual ~CCultivationSkill(){}

	virtual SecondarySkill::SubType::eType	GetSubType(){ return SecondarySkill::SubType::CultivationSkill; }
};

#endif // #if defined( PRE_ADD_SECONDARY_SKILL )
