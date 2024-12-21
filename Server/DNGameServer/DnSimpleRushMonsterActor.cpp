#include "StdAfx.h"
#include "DnSimpleRushMonsterActor.h"
#include "MAAiScript.h"
#include "MAStaticMovement.h"

CDnSimpleRushMonsterActor::CDnSimpleRushMonsterActor( CMultiRoom *pRoom, int nClassID )
: CDnNormalMonsterActor( pRoom, nClassID )
{
}

CDnSimpleRushMonsterActor::~CDnSimpleRushMonsterActor()
{
}

#if defined( PRE_ADD_ACADEMIC )

CDnAcademicAutomatMonsterActor::CDnAcademicAutomatMonsterActor( CMultiRoom *pRoom, int nClassID )
:CDnSimpleRushMonsterActor(pRoom,nClassID)
{

}

CDnAcademicAutomatMonsterActor::~CDnAcademicAutomatMonsterActor()
{

}

bool CDnAcademicAutomatMonsterActor::OnAINonTarget()
{
	MAAiScript* pScript = static_cast<MAAiScript*>(GetAIBase());

	bool bResult = m_hActor->IsProcessSkill();
	if ( bResult )
		return false;

	pScript->GetMonsterSkillAI()->UseSkill();

	return false;

}

#endif // #if defined( PRE_ADD_ACADEMIC )

MAMovementBase* CDnNoAggroTrapMonsterActor::CreateMovement()
{
	MAMovementBase* pMovement = new IBoostPoolMAStaticMovement();
	return pMovement;
}
