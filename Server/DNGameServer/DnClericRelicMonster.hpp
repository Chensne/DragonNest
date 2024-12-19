
#pragma once

#include "DnNormalMonsterActor.h"
#include "MAAiScript.h"

class CDnClericRelicMonsterActor : public CDnNormalMonsterActor , public TBoostMemoryPool< CDnClericRelicMonsterActor >
{
public:
	CDnClericRelicMonsterActor( CMultiRoom *pRoom, int nClassID ):CDnNormalMonsterActor( pRoom, nClassID ){}
	virtual ~CDnClericRelicMonsterActor(){}

	virtual bool OnAINonTarget()
	{
		MAAiScript* pScript = static_cast<MAAiScript*>(GetAIBase());

		bool bResult = m_hActor->IsProcessSkill();
		if ( bResult )
			return false;

		pScript->GetMonsterSkillAI()->UseSkill();

		return false;
	}

	// from CDnMonsterActor
	virtual bool IsClericRelicMonster( void ) { return true; };
};

