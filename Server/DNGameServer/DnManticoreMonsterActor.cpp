#include "StdAfx.h"
#include "DnManticoreMonsterActor.h"
#include "DnActorClassDefine.h"
#include "DnGameTask.h"
#include "TaskManager.h"
#include "MAAiBase.h"


CDnManticoreMonsterActor::CDnManticoreMonsterActor( CMultiRoom *pRoom, int nClassID )
:  CDnPartsMonsterActor( pRoom, nClassID )
{
	m_fShakeDelta = 0.f;
	m_nIgnoreNormalSuperArmorTime = 0;
	m_HitCheckType = HitCheckTypeEnum::Collision;
}

CDnManticoreMonsterActor::~CDnManticoreMonsterActor()
{
}


bool CDnManticoreMonsterActor::Initialize()
{
	CDnPartsMonsterActor::Initialize();

	if( !m_hObject )
		return false;

	m_hObject->SetCollisionGroup( COLLISION_GROUP_DYNAMIC( 3 ) );
	m_hObject->SetTargetCollisionGroup( 0 );

	return true;
}


void CDnManticoreMonsterActor::Process( LOCAL_TIME LocalTime, float fDelta )
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

/*
	if( !IsDie() ) {
		std::vector<DnActorHandle> hVecList;
		EtVector3 vAniDist = *GetPosition();
		vAniDist.x -= GetAniDistance()->x;
		vAniDist.z -= GetAniDistance()->z;

		int nCount =  ScanActor( GetRoom(), vAniDist, (float)GetUnitSize(), hVecList );
		if( nCount > 0 ) {
			EtVector2 vDir;
			int nSumSize;
			for( int i=0; i<nCount; i++ ) {
				if( hVecList[i] == GetMySmartPtr() ) continue;
				if( hVecList[i]->IsDie() ) continue;
				vDir = EtVec3toVec2( *hVecList[i]->GetPosition() - *GetPosition() );
				nSumSize = GetUnitSize() + hVecList[i]->GetUnitSize();
				float fLength = D3DXVec2Length( &vDir );
				if( fLength > (float)nSumSize ) continue;

				D3DXVec2Normalize( &vDir, &vDir );
				MAMovementBase *pMovement = dynamic_cast<MAMovementBase *>(hVecList[i].GetPointer());
				if( pMovement ) pMovement->MoveToWorld( vDir * (float)( nSumSize - fLength ) );
			}
		}
	}
*/

	CDnPartsMonsterActor::Process( LocalTime, fDelta );

	// 슈퍼아머 무시되는 상태고
	if ( GetIgnoreNormalSuperArmor() )
	{
		if ( LocalTime - m_nIgnoreNormalSuperArmorTime > 7000 )
		{
			SetIgnoreNormalSuperArmor(false);
			m_nIgnoreNormalSuperArmorTime = 0;
			for ( size_t i = 0 ; i < m_Parts.size() ; i++ )
			{
				m_Parts[i].ResetSetting();
			}
		}
	}
}


void CDnManticoreMonsterActor::OnDamage( CDnDamageBase *pHitter, SHitParam &HitParam, HitStruct *pHitStruct )
{
	m_fShakeDelta = 0.3f;
	CDnPartsMonsterActor::OnDamage( pHitter, HitParam, pHitStruct );
}

bool CDnManticoreMonsterActor::CheckDamageAction( DnActorHandle hActor )
{
	//MAMovementBase *pMovement = dynamic_cast<MAMovementBase *>(this);
	// 모양세 셋팅
	if( IsFly() ) 
	{
		m_HitParam.szActionName = "Hit_AirDown";
		m_HitParam.vVelocity.y = 0.0f;
		m_HitParam.vResistance.y = 0.0f;

		OutputDebug( "-----------Hit_AirDown------------\n");
	}

	return true;
}


void CDnManticoreMonsterActor::OnDrop( float fCurVelocity )
{
	/*
	if( IsAir() && IsHit() ) {
		std::string szAction;
		float fBlendFrame = 2.f;
		// 떨어지는 속도가 10이상이면 bigBounce로 한번 더 띄어준다.
		if( fCurVelocity < -6.f && m_HitParam.vVelocity.y != 0.f ) {
			if( m_HitParam.vVelocity.y > 0.f ) {
				m_HitParam.vVelocity.y *= 0.6f;
				SetVelocityY( m_HitParam.vVelocity.y );
			}
			else { // 가속도가 처음부터 바닥으로 향해있는 경우에는 뒤집어줘야한다.
				m_HitParam.vVelocity.y *= -0.6f;
				m_HitParam.vResistance.y *= -1.f;
				SetVelocityY( m_HitParam.vVelocity.y );
				SetResistanceY( m_HitParam.vResistance.y );
			}
			szAction = "Hit_AirBounce";
		}
		else {
			szAction = "Down_SmallBounce";
			fBlendFrame = 0.f;
		}
		SetActionQueue( szAction.c_str(), 0, fBlendFrame );
	}
	*/
}
