#include "StdAfx.h"
#include "DnApplySEWhenActionSetBlowEnabledProcessor.h"
#include "DnClericMentalChargeBlow.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


CDnApplySEWhenActionSetBlowEnabledProcessor::CDnApplySEWhenActionSetBlowEnabledProcessor( DnActorHandle hActor ) : IDnSkillProcessor( hActor )
{
	m_iType = APPLY_SE_WHEN_ACTIONSET_ENABLED;
}

CDnApplySEWhenActionSetBlowEnabledProcessor::~CDnApplySEWhenActionSetBlowEnabledProcessor(void)
{

}


void CDnApplySEWhenActionSetBlowEnabledProcessor::OnBegin( LOCAL_TIME LocalTime, float fDelta, DnSkillHandle hParentSkill )
{
	//// 부모 상태효과 중에 ChangeActionSet 상태효과를 제외하고 추가함.
	//// 스킬 쪽에서는 발동될 때 이 상태효과들을 추가하지 않도록 되어있다.
	//int iNumSE = m_hParentSkill->GetStateEffectCount();
	//for( int i = 0; i < iNumSE; ++i )
	//{
	//	const CDnSkill::StateEffectStruct* pStateEffect = m_hParentSkill->GetStateEffectFromIndex( i );
	//	if( STATE_BLOW::BLOW_129 != pStateEffect->nID )		// change action set 상태효과를 제외하고 적용할 상태효과를 따로 받아둔다.
	//	{
	//		m_vlStateEffect
	//	}
	//}
}

#ifdef _GAMESERVER
void CDnApplySEWhenActionSetBlowEnabledProcessor::AddStateEffect( const CDnSkill::StateEffectStruct* pStateEffect )
{
	// 스킬 생성될 때 부터 스킬 객체쪽에서 자신에게 주어진 상태효과들을 change action set 상태효과만 제외하고
	// 전부 이쪽으로 넣어준 다음에 제거하는 것으로.. 
	m_vlpStateEffect.push_back( (CDnSkill::StateEffectStruct*)pStateEffect );
}

void CDnApplySEWhenActionSetBlowEnabledProcessor::OnChangeActionBegin( void )
{
	for( int i = 0; i < (int)m_vlpStateEffect.size(); ++i )
	{
		const CDnSkill::StateEffectStruct* pStateEffect = m_vlpStateEffect.at( i );
		int iID = m_hHasActor->CmdAddStateEffect( m_hParentSkill->GetInfo(), (STATE_BLOW::emBLOW_INDEX)pStateEffect->nID, 
												  /*pStateEffect->nDurationTime*/-1, pStateEffect->szValue.c_str() );

		if( -1 != iID )
			m_vlAppliedStateEffectID.push_back( iID );
	}
	
	// #33312 강제로 부모 스킬의 속성 부여
	if( m_hParentSkill )
	{
		CDnState::ElementEnum eElement = m_hParentSkill->GetElement();
		m_hHasActor->PushForceHitElement( eElement );
	}
}

void CDnApplySEWhenActionSetBlowEnabledProcessor::OnChangeActionSetBlowEnd( void )
{
	// 추가했던 상태효과들 제거.
	for( int i = 0; i < (int)m_vlAppliedStateEffectID.size(); ++i )
	{
		m_hHasActor->CmdRemoveStateEffectFromID( m_vlAppliedStateEffectID.at(i) );
	}

	m_vlAppliedStateEffectID.clear();

	// #33312 강제로 부모 스킬의 속성 부였했던것 풀어줌.
	if( m_hParentSkill )
	{
		CDnState::ElementEnum eElement = m_hParentSkill->GetElement();
		m_hHasActor->PopForceHitElement();
	}
}
#endif


void CDnApplySEWhenActionSetBlowEnabledProcessor::Process( LOCAL_TIME LocalTime, float fDelta )
{


}


void CDnApplySEWhenActionSetBlowEnabledProcessor::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{

}