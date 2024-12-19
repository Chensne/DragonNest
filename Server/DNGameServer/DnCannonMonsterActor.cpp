#include "StdAfx.h"
#include "DnCannonMonsterActor.h"
#include "DnProjectile.h"
#include "DnPlayerActor.h"


CDnCannonMonsterActor::CDnCannonMonsterActor( CMultiRoom *pRoom, int nClassID ) : TDnMonsterActor( pRoom, nClassID ),
																				  m_vShootDir( 0.0f, 0.0f, 0.0f ),
																				  m_vCannonGroundHitPos( 0.0f, 0.0f, 0.0f ),
																				  m_vCannonTargetingDir( 0.0f, 0.0f, 0.0f )
{
	m_vCannonLookDirection = GetMatEx()->m_vZAxis; // Rotha - ������ ���� �ʱ� ������ ���� ������ü�� ������ ��ġ.
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
	SlaveOf( hMasterPlayerActor->GetUniqueID() );		// �뿹 ������ ����� �÷��̾� ȭ�鿡 ������ ��. �˾Ƽ� Ŭ��� ��Ŷ�� ������.
	SetCannonLookDirection(&GetMatEx()->m_vZAxis);  // Rotha Ÿ�� ������ ���� Look ������ �ʱ�ȭ �� �ݴϴ�.
	
	// TODO: ���� �׽�Ʈ �ڵ�
	SetTeam( hMasterPlayerActor->GetTeam() );
}

void CDnCannonMonsterActor::ClearMasterPlayerActor( void )
{
	m_hMasterPlayerActor.Identity();
	SetCannonLookDirection(&GetMatEx()->m_vZAxis); // Rotha Ÿ������ ���� Look ������ �ʱ�ȭ���ݴϴ�.
	// �뿹 ����. 
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
	// ������ ��� �ִ� ������ �޺� �� CP �� ó���ǵ��� �Ѵ�.
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
				// ���� �߻�ü�� �������� Ŭ�󿡼� ������ Ÿ�� ������ ���� Ÿ�����ؼ� ���� ���.
				ProjectileStruct* pStruct = (ProjectileStruct *)pPtr;

				// ������ ���� ��� ���⿡ ���� ���� ���� �����ֱ� ������ ���� m_Cross ���� ȸ������ �ݿ��Ǿ����� �ʴ�.
				// ���⼭ �ݿ��ؼ� �߻�ü�� �������ش�.
				MatrixEx LocalCross = m_Cross;
				LocalCross.m_vZAxis = m_vCannonTargetingDir;
				LocalCross.m_vZAxis.y = 0.0f;
				EtVec3Normalize( &LocalCross.m_vZAxis, &LocalCross.m_vZAxis );
				LocalCross.MakeUpCartesianByZAxis();

				// ���Ͱ� �߻�ü ��� ��� CreateProjectile ���ο��� ������ MatrixEx �� �����Ͽ� Target �� ���ϴ� 
				// ��ƾ�� �־ ��� ���� ���⼭ �ٲ���� ���� ������.
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
