#include "StdAfx.h"
#include "DnWorldHitStateEffectProp.h"


CDnWorldHitStateEffectProp::CDnWorldHitStateEffectProp( CMultiRoom* pRoom ) : CDnWorldActProp( pRoom ),
CDnDamageBase( DamageObjectTypeEnum::Prop )
//,m_pFSM( new TDnFiniteStateMachine<DnPropHandle>(GetMySmartPtr()) )
{
		
}

CDnWorldHitStateEffectProp::~CDnWorldHitStateEffectProp(void)
{
	//SAFE_DELETE( m_pFSM );
	ReleasePostCustomParam();
}


bool CDnWorldHitStateEffectProp::Initialize( CEtWorldSector *pParentSector, const char *szPropName, EtVector3 &vPos, EtVector3 &vRotate, EtVector3 &vScale )
{
	if( CDnWorldActProp::Initialize( pParentSector, szPropName, vPos, vRotate, vScale ) == false ) 
		return false;

	if( m_hMonster ) 
		*m_hMonster->GetMatEx() = *GetMatEx();

	return true;
}

bool CDnWorldHitStateEffectProp::InitializeTable( int nTableID )
{
	if( CDnWorldActProp::InitializeTable( nTableID ) == false ) 
		return false;

	if( GetData() )
	{
		HitStateEffectStruct *pStruct = static_cast< HitStateEffectStruct * >( GetData() );		
#ifdef PER_ADD_PROP_TRAP_DIFFICULT_SET		
		bool bResult = InitializeMonsterActorProp( pStruct->MonsterTableID, pStruct->MonsterWeight );
#else
		bool bResult = InitializeMonsterActorProp( pStruct->MonsterTableID );
#endif

		if( bResult )
		{		
			// Skill Component.
			if( 0 != pStruct->SkillTableID )
				m_SkillComponent.Initialize( pStruct->SkillTableID, pStruct->SkillLevel );

			// HitComponent.
			m_HitComponent.Initialize( m_hMonster, &m_Cross, GetMySmartPtr(), this );	

			// FSM.
//			CDnPropState * pNormalState = CDnPropState::Create( GetMySmartPtr(), CDnPropState::NORMAL );			
		}
		else
			return false;
	}

	return true;
}


void CDnWorldHitStateEffectProp::OnSignal( SignalTypeEnum Type, void *pPtr, LOCAL_TIME LocalTime, LOCAL_TIME SignalStartTime, LOCAL_TIME SignalEndTime, int nSignalIndex )
{	
	/*if( m_hMonster )
	{	
		*(m_hMonster->GetMatEx()) = m_Cross;
		m_SkillComponent.OnSignal( m_hMonster, GetMySmartPtr(), type, pPtr, LocalTime, SignalStartTime, SignalEndTime, nSignalIndex );
	}*/


	if( m_hMonster )
	{	
		DNVector(DnActorHandle) vlHittedActors;
		m_HitComponent.OnSignal( Type, pPtr, LocalTime, SignalStartTime, SignalEndTime, nSignalIndex, &vlHittedActors );
		if( false == vlHittedActors.empty() )
		{		
			HitStruct * pStruct = (HitStruct *)pPtr;

			// 프랍에 적용될 때는 스킬에 있는 타겟 타입등은 무시하고 절차의 간소화를 위해서 hitsignal 을 통과한 경우
			// 스킬에 지정된 대상 팀 체크를 따로 하지 않는다.
			m_SkillComponent.ApplyStateBlowsToTheseTargets( m_hMonster, vlHittedActors, pStruct );
		}

	}

//	CDnWorldBrokenProp::OnSignal( Type, pPtr, LocalTime, SignalStartTime, SignalEndTime, nSignalIndex );

}