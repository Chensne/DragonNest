#include "StdAfx.h"
#include "DnApplySEWhenTargetNormalHitProcessor.h"
#include "DnActor.h"

#ifdef _GAMESERVER

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


CDnApplySEWhenTargetNormalHitProcessor::CDnApplySEWhenTargetNormalHitProcessor( DnActorHandle hActor, float fProbability ) : 
																				IDnSkillProcessor( hActor ),
																				m_fProbability( fProbability )
{
	m_iType = APPLY_SE_WHEN_TARGET_NORMAL_HIT;

	// MASkillUser 에 자기 자신의 객체의 포인터를 등록.
	m_hHasActor->AddApplySEWhenTargetNormalHitProcessor( this );
}

CDnApplySEWhenTargetNormalHitProcessor::~CDnApplySEWhenTargetNormalHitProcessor( void )
{
	// MASkillUser 에서 제거.
	m_hHasActor->RemoveApplySEWhenTargetNormalHitProcessor( this );
}


void CDnApplySEWhenTargetNormalHitProcessor::OnBegin( LOCAL_TIME LocalTime, float fDelta, DnSkillHandle hParentSkill )
{

}

void CDnApplySEWhenTargetNormalHitProcessor::Process( LOCAL_TIME LocalTime, float fDelta )
{

}


void CDnApplySEWhenTargetNormalHitProcessor::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{

}

void CDnApplySEWhenTargetNormalHitProcessor::OnNormalHitSuccess( DnActorHandle hHittedTarget )
{
	if( !m_hParentSkill )
		return;

	if( !hHittedTarget )
		return;

	if( hHittedTarget->IsDie() )
		return;

	int iNumSE = m_hParentSkill->GetStateEffectCount();
	for( int i = 0; i < iNumSE; ++i )
	{
		const CDnSkill::StateEffectStruct* pSE = m_hParentSkill->GetStateEffectFromIndex( i );

		// target 으로 지정된 녀석들만 넣어준다.
		if( CDnSkill::ApplyTarget == pSE->ApplyType )
		{
			hHittedTarget->CmdAddStateEffect( m_hParentSkill->GetInfo(), (STATE_BLOW::emBLOW_INDEX)pSE->nID, pSE->nDurationTime, pSE->szValue.c_str() );
		}
	}
}

#endif // #ifdef _GAMESERVER