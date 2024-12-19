
#include "StdAfx.h"
#include "DnLotusGolemMonsterActor.h"
#include "DnActorClassDefine.h"
#include "DnGameTask.h"
#include "TaskManager.h"
#include "MAAiBase.h"
#include "MAStaticLookMovement.h"

CDnLotusGolemMonsterActor::CDnLotusGolemMonsterActor( CMultiRoom *pRoom, int nClassID )
:  CDnPartsMonsterActor( pRoom, nClassID )
{
	m_fShakeDelta = 0.f;
	m_HitCheckType = HitCheckTypeEnum::Collision;
}

CDnLotusGolemMonsterActor::~CDnLotusGolemMonsterActor()
{
}

MAMovementBase* CDnLotusGolemMonsterActor::CreateMovement()
{
	MAMovementBase* pMovement = new IBoostPoolMAStaticMovement();
	return pMovement;
}

bool CDnLotusGolemMonsterActor::Initialize()
{
	CDnPartsMonsterActor::Initialize();

	if( !m_hObject )
		return false;

	m_hObject->SetCollisionGroup( COLLISION_GROUP_DYNAMIC( 3 ) );
	m_hObject->SetTargetCollisionGroup( 0 );

	return true;
}


void CDnLotusGolemMonsterActor::Process( LOCAL_TIME LocalTime, float fDelta )
{
	float fValue = 1.f;
	if( m_fShakeDelta > 0.f ) {
		m_fShakeDelta -= fDelta;
		if( m_fShakeDelta <= 0.f ) {
			m_fShakeDelta = 0.f;
		}
		fValue = 1.f - ( ( _rand(m_hActor->GetRoom())%100 ) * 0.0005f );
	}
	MAActorRenderBase::SetScale( EtVector3( GetScale() * fValue, GetScale(), GetScale() * fValue ) );

	CDnPartsMonsterActor::Process( LocalTime, fDelta );
}

void CDnLotusGolemMonsterActor::OnDamage( CDnDamageBase *pHitter, SHitParam &HitParam, HitStruct *pHitStruct )
{
	m_fShakeDelta = 0.3f;
	CDnPartsMonsterActor::OnDamage( pHitter, HitParam, pHitStruct );
}