#include "StdAfx.h"
#include "DnCannonMonsterActor.h"
#include "DnProjectile.h"
#include "DnPlayerActor.h"


CDnCannonMonsterActor::CDnCannonMonsterActor( CMultiRoom *pRoom, int nClassID ) : TDnMonsterActor( pRoom, nClassID ),
																				  m_vShootDir( 0.0f, 0.0f, 0.0f ),
																				  m_vCannonGroundHitPos( 0.0f, 0.0f, 0.0f ),
																				  m_vCannonTargetingDir( 0.0f, 0.0f, 0.0f )
{
	m_vCannonLookDirection = GetMatEx()->m_vZAxis; // Rotha - 대포의 시작 초기 방향은 현재 대포자체의 시점과 일치.
}

CDnCannonMonsterActor::~CDnCannonMonsterActor()
{
	if( m_hMasterPlayerActor )
	{
		CDnPlayerActor* pPlayerActor = static_cast<CDnPlayerActor*>( m_hMasterPlayerActor.GetPointer() );
		pPlayerActor->OnCannonMonsterDie();

		ClearMasterPlayerActor();
	}
}

MAMovementBase* CDnCannonMonsterActor::CreateMovement()
{
	MAMovementBase* pMovement = new IBoostPoolMAStaticMovement();
	return pMovement;
}

void CDnCannonMonsterActor::SetMasterPlayerActor( DnActorHandle hMasterPlayerActor )
{
	m_hMasterPlayerActor = hMasterPlayerActor;
	SlaveOf( hMasterPlayerActor->GetUniqueID() );		// 노예 지정을 해줘야 플레이어 화면에 데미지 뜸. 알아서 클라로 패킷도 보낸다.
	SetCannonLookDirection(&GetMatEx()->m_vZAxis);  // Rotha 타고 내릴때 대포 Look 정보를 초기화 해 줍니다.
	
	// TODO: 대포 테스트 코드
	SetTeam( hMasterPlayerActor->GetTeam() );
}

void CDnCannonMonsterActor::ClearMasterPlayerActor( void )
{
	m_hMasterPlayerActor.Identity();
	SetCannonLookDirection(&GetMatEx()->m_vZAxis); // Rotha 타고내릴때 대포 Look 정보를 초기화해줍니다.
	// 노예 해제. 
	SlaveRelease();
}

void CDnCannonMonsterActor::Look( EtVector2& vVec, bool bForceRotate)
{
	if( bForceRotate ) 
	{
		if( EtVec2LengthSq( &vVec ) > 0.f ) {
			GetMatEx()->m_vZAxis = EtVector3( vVec.x, 0.f, vVec.y );
			EtVec3Normalize( &GetMatEx()->m_vZAxis, &GetMatEx()->m_vZAxis );
			GetMatEx()->MakeUpCartesianByZAxis();
		}
	}
}

void CDnCannonMonsterActor::OnHitSuccess( LOCAL_TIME LocalTime, DnActorHandle hActor, HitStruct *pStruct )
{
	// 대포를 잡고 있는 액터의 콤보 및 CP 로 처리되도록 한다.
	if( m_hMasterPlayerActor && m_hMasterPlayerActor->IsPlayerActor() )
	{
		m_hMasterPlayerActor->OnHitSuccess( LocalTime, hActor, pStruct );
	}
}

void CDnCannonMonsterActor::OnHitFinish( LOCAL_TIME LocalTime, HitStruct *pStruct )
{
	if( m_hMasterPlayerActor && m_hMasterPlayerActor->IsPlayerActor() )
	{
		m_hMasterPlayerActor->OnHitFinish( LocalTime, pStruct );
	}
}

void CDnCannonMonsterActor::OnMasterPlayerActorDie( void )
{
	ClearMasterPlayerActor();
}

void CDnCannonMonsterActor::OnDie( DnActorHandle hHitter )
{
	if( m_hMasterPlayerActor )
	{
		CDnPlayerActor* pPlayerActor = static_cast<CDnPlayerActor*>( m_hMasterPlayerActor.GetPointer() );
		pPlayerActor->OnCannonMonsterDie();

		ClearMasterPlayerActor();
	}
}


void CDnCannonMonsterActor::OnSignal( SignalTypeEnum Type, void *pPtr, LOCAL_TIME LocalTime, LOCAL_TIME SignalStartTime, LOCAL_TIME SignalEndTime, int nSignalIndex )
{
	switch( Type )
	{
		case STE_Projectile:
			{
				// 대포 발사체는 대포에서 클라에서 보내준 타겟 정보로 강제 타겟팅해서 따로 쏜다.
				ProjectileStruct* pStruct = (ProjectileStruct *)pPtr;

				// 대포의 실제 쏘는 방향에 따라 포신 본만 돌려주기 때문에 실제 m_Cross 값은 회전값이 반영되어있지 않다.
				// 여기서 반영해서 발사체를 생성해준다.
				MatrixEx LocalCross = m_Cross;
				LocalCross.m_vZAxis = m_vCannonTargetingDir;
				LocalCross.m_vZAxis.y = 0.0f;
				EtVec3Normalize( &LocalCross.m_vZAxis, &LocalCross.m_vZAxis );
				LocalCross.MakeUpCartesianByZAxis();

				// 몬스터가 발사체 쏘는 가운데 CreateProjectile 내부에서 몬스터의 MatrixEx 를 참조하여 Target 을 정하는 
				// 루틴이 있어서 잠시 값을 여기서 바꿔줬다 복구 시켜줌.
				MatrixEx Temp = m_Cross;
				m_Cross = LocalCross;

				CDnProjectile* pProjectile = CDnProjectile::CreateProjectile( GetRoom(), GetMySmartPtr(), LocalCross, pStruct, 
																			  &m_vCannonGroundHitPos, CDnActor::Identity(), &m_vShootDir );
				if( pProjectile == NULL ) 
					return;
				pProjectile->SetShooterType( GetMySmartPtr(), m_nActionIndex, nSignalIndex );

				SendProjectile( pProjectile, pStruct, LocalCross, nSignalIndex );
				OnProjectile( pProjectile, pStruct, LocalCross, nSignalIndex );
				OnSkillProjectile( pProjectile );

				m_Cross = Temp;

				return;
			}
			break;
	}

	CDnMonsterActor::OnSignal( Type, pPtr, LocalTime, SignalStartTime, SignalEndTime, nSignalIndex );
}
