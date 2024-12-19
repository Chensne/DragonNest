#include "StdAfx.h"
#include "DnProjectile.h"
#include "DnWeapon.h"
#include "MAActorRenderBase.h"
#include "VelocityFunc.h"
#include "EtActionSignal.h"
#include "DnWorld.h"
#include "DnMonsterActor.h"
#include "MAAiBase.h"
#include "DnWorldBrokenProp.h"
#include "DNGameServerManager.h"
#include "DnChainAttackBlow.h"
#include "DnPingpongBlow.h"
#include "MAWalkMovement.h"
#include "DnPlayerActor.h"
#include "TaskManager.h"
#include "DnGameTask.h"
#include "IDnOrbitProcessor.h"
#include "DNMissionSystem.h"
#include "DnPartsMonsterActor.h"
#include "DnComboDamageLimitBlow.h"
#include "DnStateBlow.h"
#include "DnCurseBlow.h"
#include "DnChangeStandActionBlow.h"


int CDnProjectile::s_iHitUniqueID = 0;

#define SHOOT_ACTION_NAME "Shoot"
#define HIT_ACTION_NAME "Hit"
#define DESTROY_ACTION_NAME "Destroy"		// 서버에서는 이 액션을 실행시키지 않는다.

// 게임서버와 클라이언트의 DnProjectile 클래스 파일이 따로 있으므로 수정할 경우 각각 수정해주어야 함.
const int PROJECTILE_PACKET_BUFFER_SIZE = 128;



CDnProjectile::CDnProjectile( CMultiRoom *pRoom, DnActorHandle hActor, bool bProcess, bool bIncreaseUniqueID )
: CDnWeapon( pRoom, bProcess, bIncreaseUniqueID )
, m_vPrevPos( 0.f, 0.f, 0.f )
, m_vStartPos( 0.f, 0.f, 0.f )
, m_vTargetPosition( 0.f, 0.f, 0.f )
, m_vHitPos( 0.f, 0.f, 0.f )
, m_bOnCollisionCalled( false )
, m_bPierce( false )
, m_LastHitSignalEndTime( 0 )
, m_iHitUniqueID( -1 )
, m_bHasHitAction( false )
, m_bHasHitSignalInHitAction( false )
, m_bHasHitSignalInShootAction( false )
, m_nLastHitSignalIndex( 0 )
, m_fElapsedTime( 0.0f )
, m_iShooterShootActionIndex( -1 )
, m_iSignalArrayIndex( -1 )
, m_fHitApplyPercent( -1.0f )
, m_pOrbitProcessor( NULL )
, m_pPacketBuffer( NULL )
, m_pPacketStream( NULL )
, m_vForceDir( 0.0f, 0.0f, 0.0f )
, m_bUseForceDir( false )
{
	SetWeaponType( WeaponTypeEnum::Projectile );
	m_hShooter = hActor;

	m_InvalidLocalTime = 0;
	m_CreateLocalTime = 0;

	m_bValidDamage = true;
	m_bFallGravity = false;
	m_fSpeed = 3000.f;
	m_fResistance = 0.f;
	m_nDestroyOrbitTimeGap = 3000;

	m_bStick = false;
	m_fGravity = 0.f;

	m_bFirstProcess = true;

	m_DestroyOrbitType = FallGravity;
	m_ValidType = ValidTypeEnum( WeaponLength | Stick );
	memset( &m_HitStruct, 0, sizeof(m_HitStruct) );

	m_iHitUniqueID = ++s_iHitUniqueID;

	m_bHitSignalProcessed = false;
	m_bBombHitSignalStarted = false; // 2009.7.29 한기 - 폭발시 히트 시그널이 뒤에 있는 경우가 있으므로 뒤에 있는 케이스가 있어서 히트 시그널 시작 시에 플래그 켜줌.
	

	m_VelocityType = Accell;
	m_ParentSkillInfo.hSkillUser = hActor;
	m_nMaxHitCount = 0;

	m_bHitActionStarted = false;

	m_bProcessingBombHitSignal = false;
	
	m_bFromCharger = false;

	m_fProjectileOrbitRotateZDegree = 0.0f;

	m_iShootActionIndex = -1;
	m_iHitActionIndex = -1;
	m_iDestroyActionIndex = -1;

	m_bHasDestroyAction = false;
	
	// 발사체로 쏜 것은 데미지 계산할 때 쏘았을 당시에 받아놨던 state 를 기반으로 처리.z
	m_HitParam.bFromProjectile = true;

	m_bTraceHitTarget = false;
	m_bTraceHitActorHittable = false;

	m_cShooterType = 0;
	m_dwShooterUniqueID = -1;
	m_nShooterSerialID = -1;
	m_nShooterActionIndex = -1;
	m_nShooterSignalIndex = -1;

	m_eForceHitElement = ElementEnum::ElementEnum_Amount;

	m_iSummonMonsterForceSkillLevel = 0;
	m_OrbitType = OrbitTypeCount;
	m_TargetType = TargetTypeCount;
	m_nTargetPartsBoneIndex = 0;
	m_nTargetPartsIndex = 0;
	m_nValidTime = 0;

#if defined(PRE_FIX_52329)
	m_nIgnoreHitType = 0;
#endif // PRE_FIX_52329

#if defined(PRE_ADD_55295)
	m_bHitActionVectorInit = false;
#endif // PRE_ADD_55295

	m_SkillStartTime = 0;	

#if defined(PRE_FIX_65287)
	m_fShooterFinalDamageRate = 0.0f;
#endif // PRE_FIX_65287

}

CDnProjectile::~CDnProjectile()
{
	if (m_ParentSkillInfo.hSkillUser)
	{
		DnSkillHandle hSkill = m_ParentSkillInfo.hSkillUser->FindSkill(m_ParentSkillInfo.iSkillID);
		if (hSkill)
			hSkill->RemoveProjectile(this);
	}

	SAFE_DELETE( m_pOrbitProcessor );
	SAFE_DELETE( m_pPacketBuffer );
	SAFE_DELETE( m_pPacketStream );

	FreeAction();
}

bool CDnProjectile::Initialize( MatrixEx &Offset, OrbitTypeEnum OrbitType, DestroyOrbitTypeEnum DestroyType, TargetTypeEnum TargetType )
{
	m_OrbitType = OrbitType;
	m_DestroyOrbitType = DestroyType;
	m_TargetType = TargetType;

	m_Cross = Offset;
	m_OffsetCross = Offset;
	m_vStartPos = m_vPrevPos = m_Cross.m_vPosition;
	SetActionQueue( SHOOT_ACTION_NAME, 0, 0.f );

	m_iShootActionIndex = GetElementIndex( SHOOT_ACTION_NAME );

	m_nValidTime = 0;

	m_bFirstProcess = true;
	if( m_OrbitType == Homing ||
		m_OrbitType == TerrainHoming ) {
		m_ValidType = (ValidTypeEnum)( m_ValidType | Time );
		m_ValidType = (ValidTypeEnum)( m_ValidType & ~WeaponLength );
	}

	if( IsExistAction( SHOOT_ACTION_NAME ) )
	{
		// 히트 시그널이 있는지 확인한다.
		// 현재 호밍 가속도인 경우엔 시간이 다되면 데미지를 적용하도록 되어있기 때문에 Shoot 액션에 Hit 시그널이 없는 경우
		// 데미지 처리를 하지 않도록 구분하기 위해서 여기서 미리 구분해두도록 한다.
		CEtActionBase::ActionElementStruct* pShootActionElement = this->GetElement( SHOOT_ACTION_NAME );
		int iNumSignals = static_cast<int>(pShootActionElement->pVecSignalList.size());
		for( int iSignal = 0; iSignal < iNumSignals; ++iSignal )
		{
			SignalTypeEnum eSignalType = (SignalTypeEnum)pShootActionElement->pVecSignalList.at( iSignal )->GetSignalIndex();
			if( STE_Hit == eSignalType )
			{
				m_bHasHitSignalInShootAction = true;
				break;
			}
		}
	}

	if( IsExistAction( HIT_ACTION_NAME ) )
	{
		m_bHasHitAction = true;

		// 히트 시그널이 있어야 실제로 히트 액션이 유효하다고 판단한다.
		CEtActionBase::ActionElementStruct* pHitActionElement = this->GetElement( HIT_ACTION_NAME );
		m_iHitActionIndex = GetElementIndex( HIT_ACTION_NAME );
		int iNumSignals = static_cast<int>(pHitActionElement->pVecSignalList.size());
		for( int iSignal = 0; iSignal < iNumSignals; ++iSignal )
		{
			SignalTypeEnum eSignalType = (SignalTypeEnum)pHitActionElement->pVecSignalList.at( iSignal )->GetSignalIndex();
			if( STE_Hit == eSignalType ||
				STE_Projectile == eSignalType )		// 디스토션 에로우 등은 발사체의 hit 액션에서 발사체를 다시 쏜다..
			{
				m_bHasHitSignalInHitAction = true;
				break;
			}
		}
	}

	if( IsExistAction( DESTROY_ACTION_NAME ) )
	{
		m_iDestroyActionIndex = GetElementIndex( DESTROY_ACTION_NAME );
		m_bHasDestroyAction = true;
	}

	return true;
}

bool CDnProjectile::PostInitialize( void )
{
	bool bResult = false;

	_ASSERT( NULL == m_pOrbitProcessor );

	S_PROJECTILE_PROPERTY OrbitProperty;
	OrbitProperty.eOrbitType = m_OrbitType;
	OrbitProperty.eTargetType = m_TargetType;
	OrbitProperty.eVelocityType = m_VelocityType;
	OrbitProperty.fSpeed = m_fSpeed;
	OrbitProperty.fResistance = m_fResistance;
	OrbitProperty.fProjectileOrbitRotateZ = m_fProjectileOrbitRotateZDegree;
	m_pOrbitProcessor = IDnOrbitProcessor::Create( m_Cross, m_OffsetCross, &OrbitProperty );		// 초기위치는 오프셋과 같지만.. 의미상 이렇게 따로 넣어준다.

	_ASSERT( m_pOrbitProcessor );
	if( NULL != m_pOrbitProcessor )
	{
		m_pOrbitProcessor->SetRoom( GetRoom() );
		m_pOrbitProcessor->SetTargetActor( m_hTargetActor );
		m_pOrbitProcessor->SetTargetPosition( m_vTargetPosition );
		m_pOrbitProcessor->SetValidTimePointer( &m_nValidTime );
		m_pOrbitProcessor->SetTargetPartsIndex(m_nTargetPartsIndex, m_nTargetPartsBoneIndex);

		bResult = true;
	}

	return bResult;
}


void CDnProjectile::ProcessOrbit( LOCAL_TIME LocalTime, float fDelta )
{
	if( m_bTraceHitTarget )
	{
		if( m_hTraceActor )
		{
			EtVector3 vPos = *m_hTraceActor->GetPosition();
			m_Cross.SetPosition( vPos );
			return;
		}
	}

	if( m_bStick )
		return;

	// 생성되다 만 발사체는 PostInitialize 가 호출이 안되어 있는 경우도 있다.
	// 대표적인 예가 화살통 보조무기 없이 에어리얼 체인샷이 나가는 경우.
	if( m_pOrbitProcessor )
		m_pOrbitProcessor->ProcessOrbit( m_Cross, m_vPrevPos, LocalTime, fDelta );
}

void CDnProjectile::ProcessValid( LOCAL_TIME LocalTime, float fDelta )
{
	// 한 번 invalid 체크된 것은 다시 체크하지 않는다.
	// hit 되어 폭발하는 발사체 같은 경우 m_bValidDamage 플래그가 폭발시에 켜졌다가
	// 다시 ProcessValid 내에서 사거리가 다 되어 ValidDamage가 false로 되는 경우가 있다.
	if( 0 == m_InvalidLocalTime )
	{
		if( m_bValidDamage && ( m_ValidType & ValidTypeEnum::WeaponLength ) )
		{
			if( GetWeaponLength() != -1 )
			{
				bool bRangeOut = false;
				if( RangeFallGravity == m_DestroyOrbitType )
				{
					float fAdditionalRange = CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::RangeFallGravityAdditionalProjectileRange );
					float fRangeFallGravityLength = float(GetWeaponLength()) * fAdditionalRange;
					bRangeOut = (fRangeFallGravityLength < EtVec3Length( &( m_Cross.m_vPosition - m_vStartPos ) ));
				}
				else
				{
					bRangeOut = ( EtVec3Length( &( m_Cross.m_vPosition - m_vStartPos ) ) > GetWeaponLength() );
				}

				if( bRangeOut )
				{
					m_bValidDamage = false;
					m_InvalidLocalTime = LocalTime;

					// #43085 클라에서는 Destroy 액션을 실행시키는 시점.. 서버에서는 곧바로 발사체 제거.
					if( m_bHasDestroyAction )
					{
						// FallGravity 는 퍼니싱 스윙이나 파이어볼 처럼 고각으로 쏴서 떨어뜨리는 건 맞아야 하므로 RangeFallGravity 만 처리. 
						if( RangeFallGravity == m_DestroyOrbitType )
							SetDestroy(); 
					}
				}
			}
		}
	}

	// 한번 invalid 된 것은 다시 invalid 처리 되지 않도록 한다.
	// hit 액션이 있는 경우 validdamage 가 다시 켜지므로 m_bOnCollisionCalled 로 구분한다.
	if( m_bValidDamage && ( m_ValidType & ValidTypeEnum::Time ) && false == m_bOnCollisionCalled ) 
	{
		if( (int)( LocalTime - m_CreateLocalTime ) > m_nValidTime ) 
		{
			m_bValidDamage = false;
			m_InvalidLocalTime = LocalTime;

			if( m_hTargetActor && m_hShooter &&
				OrbitTypeEnum::Homing == m_OrbitType &&
				VelocityTypeEnum::Accell == m_VelocityType )
			{
				if( m_HitParam.szActionName.empty() )
					m_HitParam.szActionName.assign( "Hit_Front" );

				// hit 액션에 hit 시그널이 있다면 valid damage go on 플래그를 true 로 호출한다.
				// 호밍 가속도라도 ProcessDamage 에서 hit 처리가 먼저될 때도 있고 여기서 될 때도 있다.
				// 발사체의 위치로 충돌 처리가 된다면 ProcessDamage 쪽에서 처리가 되고 그렇지 않다면 여기서 처리가 된다.
				// 호밍 가속도 발사체에 Shoot 액션에 Hit 시그널이 없다면 Hit 처리는 당연히 안되는 거고,
				// CheckAndApplyDamage() 함수 내부에서 추가되도록 처리되는 상태효과가 추가되면 안되므로 OnCollisionWithActor() 만 호출해준다.
				// 이쪽에는 처음 hit 될 때 이므로 Shoot 액션일 때 들어옴.
				if( m_bHasHitSignalInShootAction )
				{
					if( m_bTraceHitTarget || m_hTargetActor->IsHittable( m_hShooter, LocalTime, &m_HitStruct ) )
					{
#if defined(PRE_FIX_59238)
						//꼭두각시와 꼭두각시를 소환한 주인 액터는 동시 히트 되지 않아야 한다.
						if (IsHittable(m_hTargetActor))
						{
							CheckAndApplyDamage( m_hTargetActor, m_bHasHitSignalInHitAction );
						}
#else
						CheckAndApplyDamage( m_hTargetActor, m_bHasHitSignalInHitAction );
#endif // PRE_FIX_59238
					}
				}
				else
				{
					OnCollisionWithActor();
				}
			}
			else
			{
				// #38437 클라에서는 Destroy 액션을 실행시키므로 서버에서는 있는지 없는지만 체크해서 있으면 
				// Destroy 액션 실행이 아닌 바로 없애도록 처리.
				if( m_bHasDestroyAction )
					SetDestroy(); 
			}
		}
	}

#if defined(PRE_FIX_52329)
	//IgnoreHitType 설정이 0이 아닌 경우 Valid체크는 하지 않도록 한다..
	if (m_nIgnoreHitType == 0)
	{
#endif // PRE_FIX_52329

	// terrain linear 라면 현재 위치가 갈 수 없는 지역에선 프로젝타일 사라지도록 처리.
	// 롤링 라바 같은 경우 terraing linear 이면서 못가는 곳에 부딪혔을 때도 hit 의 hit 시그널이
	// 처리 되어야 하므로 hit 액션을 갖고 있는 발사체인 경우 데미지 처리하도록 변경.
	if( OrbitTypeEnum::TerrainLinear == m_OrbitType ||
		OrbitTypeEnum::TerrainHoming == m_OrbitType)
	{
		if( false == MAWalkMovement::IsMovableBlock( &INSTANCE(CDnWorld), m_Cross.GetPosition() ) )
		{
			if( m_bValidDamage ) 
			{
				m_bValidDamage = false;
				m_InvalidLocalTime = LocalTime;
			}

			m_bStick = true;

			OnCollisionWithGround();
		}
	}

	if( !m_bStick && ( m_ValidType & ValidTypeEnum::Stick ) )
	{
#if defined(PRE_FIX_55855)
		float fHeight = CDnWorld::GetInstance( GetRoom() ).GetMaxHeightWithProp( m_Cross.m_vPosition );
#else
		float fHeight = CDnWorld::GetInstance(GetRoom()).GetHeight( m_Cross.m_vPosition );
		
#endif // PRE_FIX_55855
		if( m_Cross.m_vPosition.y < fHeight )
		{
			// 발사체의 절대적인 위치 값 기준으로 방향을 구한다.
			// ZVector 는 OrbitProcessor 에서 컨트롤하고, 실제 방향과 다를 수 있다.
			EtVector3 vDir = m_Cross.m_vPosition - m_vPrevPos;
			EtVec3Normalize( &vDir, &vDir );

			EtVector3 vPickPos;
			if( CDnWorld::GetInstance(GetRoom()).Pick( m_vPrevPos, vDir, vPickPos ) == true ) 
			{
				m_Cross.m_vPosition = vPickPos;
#if defined(PRE_FIX_55855)
				m_Cross.m_vPosition.y = CDnWorld::GetInstance( GetRoom() ).GetMaxHeightWithProp( vPickPos );
#endif // PRE_FIX_55855
			}
			else m_Cross.m_vPosition.y = fHeight;

			//m_Cross.m_vPosition += m_Cross.m_vZAxis * -10.f;
			if( m_bValidDamage ) {
				m_bValidDamage = false;
				m_InvalidLocalTime = LocalTime;
			}
			m_bStick = true;

			OnCollisionWithGround();
		}
	}

	// 가속도 호밍인 경우엔 프랍과 충돌했는지 확인.
	if( m_hTargetActor && 
		OrbitTypeEnum::Homing == m_OrbitType &&
		VelocityTypeEnum::Accell == m_VelocityType )
	{
		float fLength = EtVec3Length( &( m_Cross.m_vPosition - m_vPrevPos ) );
		EtVector3 vCenter = m_vPrevPos + ( m_Cross.m_vZAxis * ( fLength * 0.5f ) );
		float fPropContactDistance = FLT_MAX;
		float fPropContactTime = FLT_MAX;
		DnPropHandle hResultProp = _CheckPropCollision( vCenter, fLength, fPropContactTime, fPropContactDistance );

		if( hResultProp )
		{
			OnCollisionWithProp();
			m_bValidDamage = false;
			m_InvalidLocalTime = LocalTime;
			m_bStick = true;
		}
	}

#if defined(PRE_FIX_52329)
	}
#endif // PRE_FIX_52329

	// 이미 hit 액션을 실행하고 있다면 액션을 다시 실행시키지 않는다. (#21384)
	// 따로 OnCollision~ 함수가 호출되지 않았는데 hit 액션으로 바뀌어있다면 shoot 액션에서 
	// 직접 바꾼 경우이므로 OnCollisionWithGround() 를 호출해준다.
	if( false == m_bOnCollisionCalled && m_iHitActionIndex == GetCurrentActionIndex() && m_iHitActionIndex != -1 )
	{
		if( m_bValidDamage ) 
		{
			m_bValidDamage = false;
			m_InvalidLocalTime = LocalTime;
		}
		m_bStick = true;
		OnChangedNextActionToHit();
	}
}


void CDnProjectile::ProcessDestroyOrbit( LOCAL_TIME LocalTime, float fDelta )
{
	if( LocalTime - m_InvalidLocalTime > m_nDestroyOrbitTimeGap ) {
		SetDestroy();
	}
	switch( m_DestroyOrbitType ) {
		case Instantly:
			if( false == m_bHasHitAction )		// hit 액션 갖고 있는 발사체는 hit 액션 끝나거나 valid time 다 되면 알아서 destroy 됨.
				SetDestroy();
			break;
		case FallGravity:
		case RangeFallGravity:
			{
				if( !m_bStick ) 
				{
					m_Cross.m_vPosition.y += CalcMovement( m_fGravity, fDelta, FLT_MAX, FLT_MIN, -15.f );

					// summon comet 처럼 사거리 다 되어 fall gravity 로 떨어지도록 연출하는 경우가 있으므로 
					// fall gravity 로 떨어지는 동안에도 데미지를 먹도록 처리.
					m_bValidDamage = true;
					m_bFallGravity = true;
				}

			}
			break;
	}
}


DnPropHandle CDnProjectile::_CheckPropCollision( EtVector3& vCenter, float fLength, float& fPropContactDistance, float& fPropContactTime )
{
	DnPropHandle hResult;

	DNVector(DnPropHandle) hVecProp;
	int nCount = CDnWorld::GetInstance(GetRoom()).ScanProp( vCenter, fLength * 0.5f, hVecProp, FilterProjectileHitSignal() );

	if( nCount > 0 ) 
	{
		float fMinContactTime = FLT_MAX;
		float fNowContactTime = 0.f;
		SCollisionResponse Response;
		SSegment Segment;
		Segment.vOrigin = m_vPrevPos;
		Segment.vDirection = m_Cross.m_vZAxis * fLength;

		for( int i=0; i<nCount; i++ ) 
		{
			if( !hVecProp[i] )
				continue;

			if( hVecProp[i]->IsProjectileSkip() )
				continue;

			if( hVecProp[i] == m_hShooterProp )
				continue;

			if( hVecProp[i]->GetObjectHandle() && hVecProp[i]->GetObjectHandle()->FindSegmentCollision( Segment, Response ) ) 
			{
				// 서버에서는 프레임이 늦을 수 있기 때문에 발사체의 현재 위치로 비교하면 안됨.
				// 엔진에서 던져주는 도달시간 기준으로 골라내도록한다.
				//fNowContactTime = EtVec3LengthSq( &( hVecProp[i]->GetMatEx()->m_vPosition - m_Cross.m_vPosition ) );
				fNowContactTime = Response.fContactTime;

				if( fNowContactTime < fMinContactTime ) 
				{
					fMinContactTime = fNowContactTime;
					hResult = hVecProp[i];

					//						EtVector3 vVec = m_Cross.m_vPosition - m_vPrevPos;
					//						EtVec3Normalize( &vVec, &vVec );
					m_vHitPos = m_vPrevPos + ( Segment.vDirection * Response.fContactTime );
					fPropContactDistance = EtVec3LengthSq( &( Segment.vDirection * Response.fContactTime ) );
					fPropContactTime = Response.fContactTime;
				}
			}
		}

	}	
	
	return hResult;
}

bool CDnProjectile::ProcessDamage( LOCAL_TIME LocalTime, float fDelta, LOCAL_TIME SignalStartTime, 
								   LOCAL_TIME SignalEndTime, int nSignalIndex, BOOL bUseHitSignalArea )
{
	bool bHitResult = false;
	// 한번 hit 된 녀석에겐 다시 데미지 주지 않는다.

	// #40133 미션에 알려주기 위해 죽인 애들 카운트.
	int iKillCount = 0;

	if( !m_hShooter ) return false;
	if( (m_hShooter->GetActorType() != CDnActorState::PropActor) && m_hShooter->IsDie() ) return false;

	float fLength = EtVec3Length( &( m_Cross.m_vPosition - m_vPrevPos ) );
	//if( /*fLength > 0.f ||*/ m_bOnCollisionCalled ) 
	{
		EtVector3 vCenter = m_vPrevPos + ( m_Cross.m_vZAxis * ( fLength * 0.5f ) );

		// Prop Check
		float fPropContactDistance = FLT_MAX;
		float fPropContactTime = FLT_MAX;
		DnPropHandle hResultProp = _CheckPropCollision( vCenter, fLength, fPropContactDistance, fPropContactTime );

		// 액터 체크
		float fNowFrameActorContactDistanceSQ = FLT_MAX;
		DNVector(DnActorHandle) hVecList;

		float fMinDistance = FLT_MAX;
		float fDist = 0.f;

		// 히트 시그널 들고가는 프로젝타일과 폭발 히트시그널에서 같이 사용함.
		if( TRUE == bUseHitSignalArea || m_bOnCollisionCalled )
		{
			m_bProcessingBombHitSignal = true;

			MatrixEx CrossTemp = m_Cross;
			CrossTemp.MoveLocalZAxis( m_HitStruct.vOffset->z );
			CrossTemp.MoveLocalXAxis( m_HitStruct.vOffset->x );
			CrossTemp.MoveLocalYAxis( m_HitStruct.vOffset->y );

			EtVector3 vPos = CrossTemp.m_vPosition;

			float fDistance = max( m_HitStruct.fDistanceMax, (m_HitStruct.fHeightMax - m_HitStruct.fHeightMin) );
			float fXZDistanceSQ = m_HitStruct.fDistanceMax;
			float fXZDistanceMinSQ = m_HitStruct.fDistanceMin;

			fXZDistanceSQ *= fXZDistanceSQ;
			fXZDistanceMinSQ *= fXZDistanceMinSQ;

			CDnActor::ScanActor( GetRoom(), vPos, fDistance, hVecList );

			//#53454 꼭두각시 소환액터인 경우, 꼭두각시 주인?은 HitList에서 제외 시킨다.
#if defined(PRE_FIX_61382)
			DNVector(DnActorHandle) hVecActorToApplyStateEffect;
			CDnActor::ExceptionHitList2(hVecList, m_Cross, GetShooterActor(), &m_HitStruct, hVecActorToApplyStateEffect, 1, fDistance, m_vPrevPos);
#else
			CDnActor::ExceptionHitList(hVecList, m_Cross, GetShooterActor(), &m_HitStruct);
#endif // PRE_FIX_61382

			EtVector3 vDir;
			EtVector3 vZVec = m_Cross.m_vZAxis;

			if( m_HitStruct.fCenterAngle != 0.f ) {
				EtMatrix matRotate;
				EtMatrixRotationY( &matRotate, EtToRadian( m_HitStruct.fCenterAngle ) );
				EtVec3TransformNormal( &vZVec, &vZVec, &matRotate );

#if defined(PRE_FIX_63356)
				CrossTemp.m_vZAxis = vZVec;
#endif // PRE_FIX_63356
			}

			SAABox Box;
			float fDot = 0.0f;
			bool bFirstHit = true;
			bool bCheckEndTime = false;

#ifdef PRE_FIX_PIERCE_WITH_HIT_AREA
			if( m_LastHitSignalEndTime > LocalTime )
			{
				if( m_bPierce )
				{
					bCheckEndTime = true;
				}
				else
				{
					return false;
				}
			}
#else
			if( m_LastHitSignalEndTime > LocalTime )
				return false;
#endif

			if( m_nLastHitSignalIndex != nSignalIndex ) {
				bFirstHit = true;
				m_nLastHitSignalIndex = nSignalIndex;
			}

			bool isFirstHitActor = true;

			int nHitCount = 0;
			bool isHitLimited = false;

			// Actor 체크
			for( DWORD i=0; i<hVecList.size(); i++ ) 
			{
#ifdef PRE_FIX_PIERCE_WITH_HIT_AREA
				bool bFindHittedActor = false;
				if( m_bPierce == true )
				{
					if( !m_VecHittedActor.empty() )
					{
						std::vector<DnActorHandle>::iterator iter = find( m_VecHittedActor.begin(), m_VecHittedActor.end(), hVecList[i] );
						if( iter != m_VecHittedActor.end()  )
						{
							bFindHittedActor = true;
							if( bCheckEndTime )
								continue;
						}
					}
				}
#endif

				//////////////////////////////////////////////////////////////////////////
				// Hit수 제한
				//최대 Hit수가 설정되어 있고, Hit수가 최대 Hit수를 넘어 가면 멈춘다.
				isHitLimited = (m_HitParam.nHitLimitCount != 0 && nHitCount >= m_HitParam.nHitLimitCount);
				if (isHitLimited)
					break;
				//////////////////////////////////////////////////////////////////////////

				if (false == m_HitStruct.isSelfCheck)
				{
					// 자기 자신은 체크하지 않음.
					if( m_hShooter == hVecList[i] )
						continue;
				}

#if defined(PRE_FIX_59238)
				//꼭두각시와 꼭두각시를 소환한 주인 액터는 동시 히트 되지 않아야 한다.
				if (IsHittable(hVecList[i]) == false)
					continue;
#endif // PRE_FIX_59238

				bool bSendRegist = false;
				if( !hVecList[i]->IsHittableSkill( m_ParentSkillInfo.iSkillID , bSendRegist ) )
				{
					if( bSendRegist )
					{
						hVecList[i]->SendAddSEFail( CDnStateBlow::ADD_FAIL_BY_IMMUNE , STATE_BLOW::BLOW_154 );
					}
					continue;
				}

				if( !hVecList[i]->IsHittable( m_hShooter, LocalTime, &m_HitStruct, m_HitParam.iUniqueID ) ) 
					continue;

				switch( hVecList[i]->GetHitCheckType() ) {
					case CDnActor::BoundingBox: 
						{
							m_HitParam.vPosition = vPos;
							vDir = *hVecList[i]->GetPosition() - vPos;
							vDir.y = 0.f;

							hVecList[i]->GetBoundingBox( Box );

							if( CDnActor::SquaredDistance( vPos, Box ) > fXZDistanceSQ ) continue;
							if( CDnActor::SquaredDistance( vPos, Box, false ) < fXZDistanceMinSQ ) continue;

							EtVec3Normalize( &vDir, &vDir );
							fDot = EtVec3Dot( &vZVec, &vDir );
							if( EtToDegree( acos( fDot ) ) > m_HitStruct.fAngle ) continue;

							if( Box.Min.y < vPos.y + m_HitStruct.fHeightMin && 
								Box.Max.y < vPos.y + m_HitStruct.fHeightMin ) continue;
							if( Box.Min.y > vPos.y + m_HitStruct.fHeightMax && 
								Box.Max.y > vPos.y + m_HitStruct.fHeightMax ) continue;
							fNowFrameActorContactDistanceSQ = EtVec3LengthSq( &EtVector3( vPos - m_vPrevPos ) );
							if( fNowFrameActorContactDistanceSQ > fPropContactDistance ) continue;

							m_HitParam.vViewVec = -vDir;
						}
						break;
					case CDnActor::Collision:
						{
							SCollisionCapsule				Capsule;
							SCollisionResponse				CollisionResult;
							DNVector(SCollisionResponse)	vCollisionResult;
							//
							Capsule.Segment.vOrigin = vPos;
							float fHeight = m_HitStruct.fHeightMax - m_HitStruct.fHeightMin;
							Capsule.Segment.vOrigin.y = Capsule.Segment.vOrigin.y - ( m_HitStruct.fHeightMin + ( fHeight / 2.f ) );
							Capsule.Segment.vDirection = EtVector3( 0.f, fHeight / 2.f, 0.f );
							Capsule.fRadius = m_HitStruct.fDistanceMax;

							EtVector3 vDestPos;
							if( hVecList[i]->GetObjectHandle()->CEtCollisionEntity::FindCapsuleCollision( Capsule, CollisionResult, &vCollisionResult ) == false ) continue;
							if( CollisionResult.pCollisionPrimitive )
							{
								for( UINT k=0 ; k<vCollisionResult.size() ; ++k )
								{
									if( vCollisionResult[k].pCollisionPrimitive ) {
										vCollisionResult[k].pCollisionPrimitive->GetBoundingBox( Box );
										if( Box.Min.y < vPos.y + m_HitStruct.fHeightMin && Box.Max.y < vPos.y + m_HitStruct.fHeightMin ) {
											vCollisionResult.erase( vCollisionResult.begin() + k );
											k--;
											continue;
										}
										if( Box.Min.y > vPos.y + m_HitStruct.fHeightMax && Box.Max.y > vPos.y + m_HitStruct.fHeightMax ) {
											vCollisionResult.erase( vCollisionResult.begin() + k );
											k--;
											continue;
										}

										DNVector(EtVector3) vPointList;
										vPointList.push_back( Box.GetCenter() );
#ifdef PRE_FIX_COLMESH_RECTANGLE_HITSIGNAL
										if( vCollisionResult[k].pCollisionPrimitive->Type == CT_BOX ||
											vCollisionResult[k].pCollisionPrimitive->Type == CT_CAPSULE ) {
#else
										if( vCollisionResult[k].pCollisionPrimitive->Type == CT_BOX ) {
#endif // #ifdef PRE_FIX_COLMESH_RECTANGLE_HITSIGNAL
											Box.GetVertices( vPointList );
										}
										bool bCheck = false;
										for( DWORD m=0; m<vPointList.size(); m++ ) {
#if defined(PRE_FIX_63356)
											if( hVecList[i]->CheckCollisionHitCondition(vPos, CrossTemp, vPointList[m], m_HitStruct.fAngle) == true ) {
#else
											if( hVecList[i]->CheckCollisionHitCondition(vPos, m_Cross, vPointList[m], m_HitStruct.fAngle) == true ) {
#endif // PRE_FIX_63356
												bCheck = true;
												break;
											}
										}
										if( bCheck == false ) {
											vCollisionResult.erase( vCollisionResult.begin() + k );
											k--;
											continue;
										}
										m_HitParam.vBoneIndex.push_back( hVecList[i]->GetObjectHandle()->GetParentBoneIndex( vCollisionResult[k].pCollisionPrimitive ) );
									}
								}
								if( vCollisionResult.empty() ) continue;

								GetCenterPos( *CollisionResult.pCollisionPrimitive, vDestPos );
								m_HitParam.vPosition = vDestPos;
							}
							else
							{
								ASSERT( 0 );
							}

							if( m_HitStruct.fDistanceMin > 100.f ) {
								vCollisionResult.clear();
								Capsule.fRadius = m_HitStruct.fDistanceMin;
								if( hVecList[i]->GetObjectHandle()->CEtCollisionEntity::FindCapsuleCollision( Capsule, CollisionResult, &vCollisionResult ) == true ) {
									if( CollisionResult.pCollisionPrimitive )
									{
										for( UINT k=0 ; k<vCollisionResult.size() ; ++k )
										{
											if( vCollisionResult[k].pCollisionPrimitive ) {
												vCollisionResult[k].pCollisionPrimitive->GetBoundingBox( Box );
												if( Box.Min.y < vPos.y + m_HitStruct.fHeightMin && Box.Max.y < vPos.y + m_HitStruct.fHeightMin ) {
													vCollisionResult.erase( vCollisionResult.begin() + k );
													k--;
													continue;
												}
												if( Box.Min.y > vPos.y + m_HitStruct.fHeightMax && Box.Max.y > vPos.y + m_HitStruct.fHeightMax ) {
													vCollisionResult.erase( vCollisionResult.begin() + k );
													k--;
													continue;
												}
												DNVector(EtVector3) vPointList;
												vPointList.push_back( Box.GetCenter() );
#ifdef PRE_FIX_COLMESH_RECTANGLE_HITSIGNAL
												if( vCollisionResult[k].pCollisionPrimitive->Type == CT_BOX ||
													vCollisionResult[k].pCollisionPrimitive->Type == CT_CAPSULE ) {
#else
												if( vCollisionResult[k].pCollisionPrimitive->Type == CT_BOX ) {
#endif // #ifdef PRE_FIX_COLMESH_RECTANGLE_HITSIGNAL
													Box.GetVertices( vPointList );
												}
												bool bCheck = false;
												for( DWORD m=0; m<vPointList.size(); m++ ) {
#ifdef PRE_FIX_COLMESH_RECTANGLE_HITSIGNAL
													// 빼야하는 영역에서 한점이라도 안 걸리면 hit 영역과 걸쳐 있는 놈이므로 빼지 않는다...
#if defined(PRE_FIX_63356)
													if( hVecList[i]->CheckCollisionHitCondition(vPos, CrossTemp, vPointList[m], m_HitStruct.fAngle) == false ) {
#else
													if( hVecList[i]->CheckCollisionHitCondition(vPos, m_Cross, vPointList[m], m_HitStruct.fAngle) == false ) {
#endif // PRE_FIX_63356
#else
#if defined(PRE_FIX_63356)
													if( hVecList[i]->CheckCollisionHitCondition(vPos, CrossTemp, vPointList[m], m_HitStruct.fAngle) == true ) {
#else
													if( hVecList[i]->CheckCollisionHitCondition(vPos, m_Cross, vPointList[m], m_HitStruct.fAngle) == true ) {
#endif // PRE_FIX_63356
#endif // #ifdef PRE_FIX_COLMESH_RECTANGLE_HITSIGNAL
														bCheck = true;
														break;
													}
												}
#ifdef PRE_FIX_COLMESH_RECTANGLE_HITSIGNAL
												if( bCheck == true ) {
#else
												if( bCheck == false ) {
#endif // #ifdef PRE_FIX_COLMESH_RECTANGLE_HITSIGNAL
													vCollisionResult.erase( vCollisionResult.begin() + k );
													k--;
													continue;
												}
												int nBoneIndex = hVecList[i]->GetObjectHandle()->GetParentBoneIndex( vCollisionResult[k].pCollisionPrimitive );
												std::vector<int>::iterator it = std::find( m_HitParam.vBoneIndex.begin(), m_HitParam.vBoneIndex.end(), nBoneIndex );
												if( it != m_HitParam.vBoneIndex.end() ) {
													m_HitParam.vBoneIndex.erase( it );
												}
											}
										}
									}
								}
								if( m_HitParam.vBoneIndex.empty() ) continue;
							}

							m_HitParam.vViewVec = vPos - vDestPos;
							fNowFrameActorContactDistanceSQ = EtVec3LengthSq( &m_HitParam.vViewVec );
							if( fNowFrameActorContactDistanceSQ > fPropContactDistance ) continue;

							EtVec3Normalize( &m_HitParam.vViewVec, &m_HitParam.vViewVec );
						}
						break;
				}

				m_HitParam.bFirstHit = bFirstHit;
				bFirstHit = false;
				bHitResult = true;
				bool bExistActionName = !m_HitParam.szActionName.empty();

				if( bExistActionName ) 
				{
					m_vHitPos = vPos;
					hResultProp.Identity();
				}

#if !defined( PRE_FIX_PROJECTILE_PREFIX_APPLY_POINT )
#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
				// HitSignal에서 스킬 여부 상관없이 접두사 스킬 발동
				// 접두어 상태효과 무시하는 상태효과가 있으면 적용 안됨
				// #40186 접미사? 발동 조건 변경 (데미지 비율이 0인 경우 발동 되지 않도록함.)
				if (m_HitParam.bFirstHit && 
					m_HitStruct.fDamageProb != 0.0f &&
					!m_hShooter->IsAppliedThisStateBlow(STATE_BLOW::BLOW_183) && 
					isFirstHitActor)
				{
					m_hShooter->ProcessPrefixOffenceSkill_New();
					isFirstHitActor = false;
				}
#else
				//////////////////////////////////////////////////////////////////////////
				// 접두어 공격용 스킬 발동 준비 작업..
				// 발사체는 bFromProjectileSkill일 경우는 스킬에 의한 발사체...
				// 접두어 상태효과 무시하는 상태효과가 있으면 적용 안됨 [2011/03/23 semozz]
				if( (!m_HitParam.bFromProjectileSkill && !m_hShooter->IsAppliedThisStateBlow(STATE_BLOW::BLOW_183))
					&& m_HitParam.bFirstHit && isFirstHitActor)
				{
					OutputDebug("CDnProjectile::ProcessDamage 1 -> STE_Hit start %d current %d end %d\n", SignalStartTime, LocalTime, SignalEndTime);

					//여기서는 자신에게 적용 하는 상태 효과만 적용시키고,
					//맞는 녀석에게 적용해야할 상태 효과는 Target->OnDamage에서 처리 되도록한다.??
					m_hShooter->ProcessPrefixOffenceSkill(1.0f);

					isFirstHitActor = false;
				}

#endif // PRE_ADD_PREFIX_SYSTE_RENEW
#endif // PRE_FIX_PROJECTILE_PREFIX_APPLY_POINT

#ifdef PRE_FIX_PIERCE_WITH_HIT_AREA
				if( m_bPierce == true && bFindHittedActor == false )
					m_VecHittedActor.push_back( hVecList[i] );
#endif
				// CheckAndApplyDamage 에서 액션 이름이 있고 없고를 구분해서 처리하고 있으므로 액션 이름 존재 여부 조건문 바깥에서 호출해도 된다.
				// #23510, #23591 이슈 때문에, 데미지는 들어가지 않더라도 상태효과는 부여되도록 수정합니다.
				CheckAndApplyDamage( hVecList[i], true, &nHitCount );

				if( bHitResult )
				{
					// [2010/12/02 semozz]
					// Hit 시그널 마지막 시간을 넣어 놓으면 마지막 프레임에 걸려서 한번 더 맞을 수 있다.
					// 그래서 Hit 처리가 되고 나서 m_LastHitSignalEndTime에 SignalEndTime + 1을 더 이상 Hit 처리 안되도록 한다.
					m_LastHitSignalEndTime = SignalEndTime + 1;

					if( hVecList[ i ]->IsDie() )
						iKillCount++;
				}

			}

#if defined(PRE_FIX_61382)
			//히트 영역에서 소환 액터와 주인 액터가 동시에 존재 할때 주인 액터는 히트 리스트에서 제외 된다.
			//상태효과는 적용되어야 하는데, 이 시점에서 상태효과를 적용한다.
			int nApplyStateEffectActorListSize = (int)hVecActorToApplyStateEffect.size();
			for (int iIndex = 0; iIndex < nApplyStateEffectActorListSize; ++iIndex)
			{
				DnActorHandle hActor = hVecActorToApplyStateEffect[iIndex];
				ApplySkillStateEffect(hActor);
			}
#endif // PRE_FIX_61382

#if defined(PRE_ADD_50903)
			//데미지 분산 상태효과를 위한 최종 HitCount 설정.
			DnSkillHandle hParentSkill = GetParentSkill();
			if (hParentSkill)
				hParentSkill->SetHitCountForVarianceDamage(nHitCount);
#endif // PRE_ADD_50903

			// 히트시그널 들고가는 발사체의 경우 프랍도 체크
			// Note: 프랍체크는 상태효과만 있는 Heal 같은 것은 체크하지 않도록 합니다. HitAction 이 없는 것으로 구분.
			if( false == m_HitParam.szActionName.empty() )
			{
				DNVector(CEtWorldProp*) pVecProp;
				CDnWorld* pWorld = CDnWorld::GetInstancePtr(GetRoom());
				pWorld->ScanProp( vPos, fDistance, pVecProp );

				for( DWORD i=0; i<pVecProp.size(); i++ ) 
				{
					if( false == ((CDnWorldProp*)pVecProp[i])->IsBrokenType() )
						continue;

					if( !((CDnWorldBrokenProp*)pVecProp[i])->IsHittable( m_hShooter, LocalTime ) ) 
						continue;

					pVecProp[i]->GetBoundingBox( Box );

					if( CDnActor::SquaredDistance( vPos, Box ) > fXZDistanceSQ ) continue;
					if( CDnActor::SquaredDistance( vPos, Box, false ) < fXZDistanceMinSQ ) continue;

					vDir = ((CDnWorldProp*)pVecProp[i])->GetMatEx()->m_vPosition - vPos;

					EtVec3Normalize( &vDir, &vDir );
					fDot = EtVec3Dot( &vZVec, &vDir );
					if( EtToDegree( acos( fDot ) ) > m_HitStruct.fAngle ) 
						continue;

					if( Box.Min.y < vPos.y + m_HitStruct.fHeightMin && 
						Box.Max.y < vPos.y + m_HitStruct.fHeightMin ) 
						continue;
					if( Box.Min.y > vPos.y + m_HitStruct.fHeightMax && 
						Box.Max.y > vPos.y + m_HitStruct.fHeightMax ) 
						continue;

					m_HitParam.vViewVec = -vDir;
					((CDnWorldProp*)pVecProp[i])->OnDamage( m_hShooter, m_HitParam );
				}
			}
		}
		else
		{
			size_t nCount = 0;
			nCount = CDnActor::ScanActor( GetRoom(), vCenter, fLength, hVecList );

			//#53454 꼭두각시 소환액터인 경우, 꼭두각시 주인?은 HitList에서 제외 시킨다.
#if defined(PRE_FIX_61382)
			DNVector(DnActorHandle) hVecActorToApplyStateEffect;
			nCount = CDnActor::ExceptionHitList2(hVecList, m_Cross, GetShooterActor(), &m_HitStruct, hVecActorToApplyStateEffect, 2, fLength, m_vPrevPos);
#else
			nCount = CDnActor::ExceptionHitList(hVecList, m_Cross, GetShooterActor(), &m_HitStruct);
#endif // PRE_FIX_61382

			if( nCount > 0 )
			{
				m_LastHitSignalEndTime = SignalEndTime;

				float fThickness = max( fabs( m_HitStruct.fHeightMin ), fabs( m_HitStruct.fHeightMax ) );
				SSegment Segment;
				Segment.vOrigin = m_vPrevPos;
				Segment.vDirection = m_Cross.m_vZAxis * fLength;

				SCollisionCapsule Capsule;
				Capsule.Segment = Segment;
				Capsule.fRadius = fThickness;

				bool isFirstHitActor = true;

				int nHitCount = 0;
				bool isHitLimited = false;

				for( size_t i=0; i<nCount; i++ ) 
				{
					// Hit수 제한
					//최대 Hit수가 설정되어 있고, Hit수가 최대 Hit수를 넘어 가면 멈춘다.
					isHitLimited = (m_HitParam.nHitLimitCount != 0 && nHitCount >= m_HitParam.nHitLimitCount);
					if (isHitLimited)
						break;

					// Note: 느리게 가는 프로젝타일은 여러번 맞을 수 있어야 하기 때문에 이 부분은 뺍니다..
					vector<DnActorHandle>::iterator iter = find( m_VecHittedActor.begin(), m_VecHittedActor.end(), hVecList[i] );
					if( !m_VecHittedActor.empty() )
					{
						if( iter != m_VecHittedActor.end() )
							continue;
					}

#if defined(PRE_FIX_59238)
					//꼭두각시와 꼭두각시를 소환한 주인 액터는 동시 히트 되지 않아야 한다.
					if (IsHittable(hVecList[i]) == false)
						continue;
#endif // PRE_FIX_59238

					if( !hVecList[i]->IsHittable( m_hShooter, LocalTime, &m_HitStruct, m_HitParam.iUniqueID ) ) 
						continue;

					if( !hVecList[i]->GetObjectHandle() )
						continue;

					// 체인 라이트닝 발사체인 경우엔 체인 발사체를 쏜 액터가 다시 hit 되지 않도록 한다.
					// 체인 라이트닝을 사용했을 때만 hPrevAttacker 가 셋팅된다.
					if( m_ParentSkillInfo.hPrevAttacker == hVecList[i] )
						continue;

					bool bResult = false;
					SCollisionResponse				Response;
					DNVector(SCollisionResponse)	vResponse;

					CDnActor::HitCheckTypeEnum eHitCheckType = hVecList[i]->GetHitCheckType();
					switch( eHitCheckType ) 
					{
						case CDnActor::BoundingBox:
							if( fThickness == 0.f ) bResult = hVecList[i]->GetObjectHandle()->FindSegmentCollision( Segment, Response );
							else bResult = hVecList[i]->GetObjectHandle()->FindCapsuleCollision( Capsule, Response );

							fNowFrameActorContactDistanceSQ = EtVec3LengthSq( &EtVector3( *hVecList[i]->GetPosition() - m_vPrevPos ) );
							if( fNowFrameActorContactDistanceSQ > fPropContactDistance ) continue;
							break;
						case CDnActor::Collision:
							{
								int nParentBoneIndex = -1;
								EtVector3 vDestPos;
								if( fThickness == 0.f ) 
								{
									bResult = hVecList[i]->GetObjectHandle()->FindSegmentCollision( Segment, Response, &vResponse );
								}
								else
								{
									bResult = hVecList[i]->GetObjectHandle()->FindCapsuleCollision( Capsule, Response, &vResponse );
								}

								if( bResult ) 
								{
									fNowFrameActorContactDistanceSQ = EtVec3LengthSq( &EtVector3( Segment.vDirection * Response.fContactTime ) );
									if( fNowFrameActorContactDistanceSQ > fPropContactDistance ) continue;

									for( UINT k=0 ; k<vResponse.size() ; ++k )
									{
										if( vResponse[k].pCollisionPrimitive )
											m_HitParam.vBoneIndex.push_back( hVecList[i]->GetObjectHandle()->GetParentBoneIndex( vResponse[k].pCollisionPrimitive ) );
									}
								}
							}
							break;
					}

					if( bResult )
					{
						EtVector3 vHitPos = Segment.vOrigin + (Segment.vDirection * Response.fContactTime);
						fDist = EtVec3Length( &( *hVecList[i]->GetPosition() - m_Cross.m_vPosition ) );
						float fLength2 = EtVec3Length( &( *hVecList[i]->GetPosition() - m_vStartPos ) );
						SSphere Sphere;
						hVecList[i]->GetBoundingSphere( Sphere );

						// 이 타입은 fallgravity 상태가 된 순간부터 추가 사거리 다 되면 바로 삭제.
						if( RangeFallGravity == m_DestroyOrbitType )
						{
							float fAdditionalRange = CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::RangeFallGravityAdditionalProjectileRange );
							float fRangeFallGravityLength = float(GetWeaponLength()) * fAdditionalRange;

							if( CDnActor::Collision == eHitCheckType )
							{
								// 캡슐인 경우 캡슐의 radius 도 감안해 준다.
								if( 0.0f < fThickness )
								{
									fRangeFallGravityLength += Capsule.fRadius;
									if( CollisionType::CT_CAPSULE == Response.pCollisionPrimitive->Type )
									{
										fRangeFallGravityLength += static_cast<SCollisionCapsule*>(Response.pCollisionPrimitive)->fRadius;
									}
								}

								float fWeaponLengthSQ = fRangeFallGravityLength*fRangeFallGravityLength;
								EtVector3 vShootToHitPosDist = vHitPos - m_vStartPos;
								float fShootToHitPosDistSQ = EtVec3LengthSq( &vShootToHitPosDist );
								if( fWeaponLengthSQ < fShootToHitPosDistSQ )
									continue;
							}
							else
							if( fLength2 > fRangeFallGravityLength + ( Sphere.fRadius / 2.f ) )
							{
								continue;
							}
						}
						else
						{
							float fWeaponLength = float(GetWeaponLength());
							if( CDnActor::Collision == eHitCheckType )
							{
								// 캡슐인 경우 캡슐의 radius 도 감안해 준다.
								if( 0.0f < fThickness )
								{
									fWeaponLength += Capsule.fRadius;
									if( CollisionType::CT_CAPSULE == Response.pCollisionPrimitive->Type )
									{
										fWeaponLength += static_cast<SCollisionCapsule*>(Response.pCollisionPrimitive)->fRadius;
									}
								}

								EtVector3 vShootToHitPosDist = vHitPos - m_vStartPos;
								float fShootToHitPosDistSQ = EtVec3LengthSq( &vShootToHitPosDist );
								if( fWeaponLength*fWeaponLength < fShootToHitPosDistSQ )
									continue;

							}
							else
							if( fLength2 > fWeaponLength + ( Sphere.fRadius / 2.f ) ) 
							{
								continue;
							}
						}

						if( fDist < fMinDistance ) 
						{
							fMinDistance = fDist;

							// 일직선 상으로 먼저 도달하는 것에 먼저 맞음. 프랍이 먼저 맞는다면 액터는 hit 처리하지 않는다.
							m_vHitPos = vHitPos;
							fNowFrameActorContactDistanceSQ = EtVec3LengthSq( &(Segment.vDirection * Response.fContactTime) );
							if( Response.fContactTime > fPropContactTime ) continue;

							// 피격된 위치로 발사체의 위치를 셋팅해준다.
							// 이렇게 해야 폭발 액션의 hit 시그널 연산을 할 때 정확하게 체크가 된다.
							if( false == m_bPierce )
								m_Cross.m_vPosition = m_vHitPos;
						}

#if !defined( PRE_FIX_PROJECTILE_PREFIX_APPLY_POINT )				
#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
						// HitSignal에서 스킬 여부 상관없이 접두사 스킬 발동
						// 접두어 상태효과 무시하는 상태효과가 있으면 적용 안됨
						// #40186 접미사? 발동 조건 변경 (데미지 비율이 0인 경우 발동 되지 않도록함.)
						if (m_HitParam.bFirstHit && 
							m_HitStruct.fDamageProb != 0.0f &&
							!m_hShooter->IsAppliedThisStateBlow(STATE_BLOW::BLOW_183) && 
							isFirstHitActor)
						{
							m_hShooter->ProcessPrefixOffenceSkill_New();
							isFirstHitActor = false;
						}
#else
						//////////////////////////////////////////////////////////////////////////
						// 접두어 공격용 스킬 발동 준비 작업..
						// 평타일때만 접두어 시스템 공격스킬 발동
						// 발사체는 bFromProjectileSkill일 경우는 스킬에 의한 발사체...
						// 접두어 상태효과 무시하는 상태효과가 있으면 적용 안됨 [2011/03/23 semozz]
						if( (!m_HitParam.bFromProjectileSkill && !m_hShooter->IsAppliedThisStateBlow(STATE_BLOW::BLOW_183))
							&& m_HitParam.bFirstHit && isFirstHitActor)
						{
							OutputDebug("CDnProjectile::ProcessDamage 2 -> STE_Hit start %d current %d end %d\n", SignalStartTime, LocalTime, SignalEndTime);

							//여기서는 자신에게 적용 하는 상태 효과만 적용시키고,
							//맞는 녀석에게 적용해야할 상태 효과는 Target->OnDamage에서 처리 되도록한다.??
							m_hShooter->ProcessPrefixOffenceSkill(1.0f);

							isFirstHitActor = false;
						}
						//////////////////////////////////////////////////////////////////////////
#endif	// PRE_ADD_PREFIX_SYSTE_RENEW
#endif

						m_VecHittedActor.push_back( hVecList[i] );
						
						CheckAndApplyDamage( hVecList[i], true, &nHitCount );
						
						bHitResult = true;
						if( hVecList[ i ]->IsDie() )
							iKillCount++;
						
						if(false == m_bPierce)
							return bHitResult;
					}
				}

#if defined(PRE_FIX_61382)
				//히트 영역에서 소환 액터와 주인 액터가 동시에 존재 할때 주인 액터는 히트 리스트에서 제외 된다.
				//상태효과는 적용되어야 하는데, 이 시점에서 상태효과를 적용한다.
				int nApplyStateEffectActorListSize = (int)hVecActorToApplyStateEffect.size();
				for (int iIndex = 0; iIndex < nApplyStateEffectActorListSize; ++iIndex)
				{
					DnActorHandle hActor = hVecActorToApplyStateEffect[iIndex];
					ApplySkillStateEffect(hActor);
				}
#endif // PRE_FIX_61382

#if defined(PRE_ADD_50903)
				//데미지 분산 상태효과를 위한 최종 HitCount 설정.
				DnSkillHandle hParentSkill = GetParentSkill();
				if (hParentSkill)
					hParentSkill->SetHitCountForVarianceDamage(nHitCount);
#endif // PRE_ADD_50903
			}
		}

		// MakeHitParam 에서 쓰는 m_vHitPos가 액터 체크할때 업데이트 되므로 프랍 처리는 액터 처리 끝난 후 여기서.
		if( hResultProp )
		{
			switch( hResultProp->GetPropType() ) 
			{
				case PTE_Static:
					{
#if defined(PRE_FIX_52329)
						//IgnoreHitType 설정이 0이 아닌 경우 Valid체크는 하지 않도록 한다..
						if (m_nIgnoreHitType == 0)
						{
							if( m_bValidDamage ) 
							{
								m_bValidDamage = false;
								m_InvalidLocalTime = LocalTime;
							}
							m_bStick = true;
						}
#endif
					}
					break;
				case PTE_Action:
				case PTE_Trap:
					if( m_bValidDamage ) 
					{
						m_bValidDamage = false;
						m_InvalidLocalTime = LocalTime;
					}
					m_bStick = true;
					break;

				case PTE_Broken:
				case PTE_BrokenDamage:
				case PTE_HitMoveDamageBroken:
				case PTE_ShooterBroken:
				case PTE_BuffBroken:
					{
						if( !m_bValidDamage ) break;

						// 프랍까지 거리가 사거리보다 길면 맞으면 안됨. #23322
						float fLengthToPropSQ = EtVec3LengthSq(&EtVector3(m_vStartPos - hResultProp->GetMatEx()->m_vPosition));

						// 이 타입은 fallgravity 상태가 된 순간부터 추가 사거리 다 되면 바로 삭제.
						float fWeaponLength = 0.0f;
						if( RangeFallGravity == m_DestroyOrbitType )
						{
							float fAdditionalRange = CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::RangeFallGravityAdditionalProjectileRange );
							fWeaponLength = float(GetWeaponLength()) * fAdditionalRange;
						}
						else
						{
							fWeaponLength = (float)GetWeaponLength();
						}

						float fWeaponLengthSQ = fWeaponLength * fWeaponLength;
						if( fWeaponLengthSQ < fLengthToPropSQ )		// 제곱기준으로 fPropDistance 값 들어있음.
							break;

						if( ((CDnWorldBrokenProp*)hResultProp.GetPointer())->IsHittable( m_hShooter, LocalTime ) ) {
							CDnDamageBase::SHitParam HitParam;
							MakeHitParam( HitParam );

							hResultProp->OnDamage( m_hShooter, HitParam );
						}
						else {
							m_bValidDamage = false;
							m_InvalidLocalTime = LocalTime;
							m_bStick = true;
						}
					}
					break;
			}

#if defined(PRE_FIX_52329)
			//IgnoreHitType 설정이 0이 아닌 경우 Valid체크는 하지 않도록 한다..
			if (m_nIgnoreHitType == 0)
			{
				OnCollisionWithProp();
			}
#endif
		}
	}

	if( 2 <= iKillCount && m_hShooter->IsPlayerActor() )
	{
		CDnPlayerActor* pPlayerActor = static_cast<CDnPlayerActor*>(m_hShooter.GetPointer());
		CDNUserSession* pSession = pPlayerActor->GetUserSession();
		pSession->GetEventSystem()->OnEvent( EventSystem::OnKillMonster, 1,
											   EventSystem::GenocideCount, iKillCount );
	}

	return bHitResult;
}

void CDnProjectile::CheckAndApplyDamage( DnActorHandle hActor, bool bValidDamageGoOn/* = true*/, int *pHitCount /*= NULL*/ )
{
	if( !m_hShooter || !hActor ) return;

	// 옵션에 따라 추적중인 액터에게는 hit 되지 않도록 한다. 디폴트는 안맞는 거임.
	if( false == m_bTraceHitActorHittable )
	{
		if( m_hTraceActor == hActor ) 
			return;
	}

	if( !m_HitParam.szActionName.empty() && !hActor->IsDie() ) 
	{
		CDnDamageBase::SHitParam HitParam;
		MakeHitParam( HitParam );

		// 차져 상태효과로 발사된 발사체라면 액터에 셋팅하고 OnDamage 호출하고 바로 풀어줌.
		// 차져 상태효과에서 나간 발사체는 콤보 계산에서 뺀다.
		if( m_bFromCharger && m_hShooter )
			m_hShooter->SetAllowCombo( false );

		hActor->OnDamage( m_hShooter, HitParam, &m_HitStruct );

#if defined(PRE_FIX_59238)
		AddHittedActor(hActor);
#endif // PRE_FIX_59238

		if (pHitCount)
			(*pHitCount)++;

		if( m_bFromCharger && m_hShooter )
			m_hShooter->SetAllowCombo( true );

		bValidDamageGoOn = SetTraceActor( hActor, bValidDamageGoOn );

		OnCollisionWithActor();

		// Note: OnCollision() 에서 폭발 데미지를 처리하기 위해서 ValidDamage 를 다시 true로 켜주지만
		// 이 함수 호출 시에 bValidDamageGoOn 을 끄고 호출한 경우엔 폭발 데미지를 감아하지 않으므로 ValidDamage 를 꺼준다.
		if( false == bValidDamageGoOn )
			m_bValidDamage = false;
	}

	//발사체 스킬 상태효과 적용..
	ApplySkillStateEffect(hActor);

	// 관통형일때만,, 최대 히트수가 0인 경우엔 원래 처리되던대로 끝까지 계속 맞게 된다.
	if( m_bPierce && m_nMaxHitCount != 0 )
	{
		//m_nMaxHitCount--;
		map<CDnActor*, int>::iterator iter = m_mapMaxHitCount.find(hActor.GetPointer() );
		if( m_mapMaxHitCount.end() == iter )
			m_mapMaxHitCount[ hActor.GetPointer() ] = 0;
		else
		{
			++(iter->second);
			if( m_nMaxHitCount-1 == iter->second )
			{
				m_bValidDamage = false;
				m_InvalidLocalTime = m_LocalTime;

				// 클라는 알 수가 없기 때문에 서버에서 패킷을 보내준다.
				BYTE pBuffer[ 32 ];
				CPacketCompressStream Stream( pBuffer, 32 );
				Stream.Write( &m_dwUniqueID, sizeof(DWORD) );
				m_hShooter->Send( eActor::SC_INVALID_PROJECTILE, &Stream );
			}
		}
	}
}

void CDnProjectile::Process( LOCAL_TIME LocalTime, float fDelta )
{
	if( m_CreateLocalTime == 0 ) m_CreateLocalTime = LocalTime;

	// 한기 2009.7.22
	// 프로젝타일 생성된 후의 지나간 시간
	// 프로젝타일 궤적 관련..  (#4851)
	m_fElapsedTime = float(LocalTime - m_CreateLocalTime) * 0.001f;

	m_bHitSignalProcessed = false;

	ProcessOrbit( LocalTime, fDelta );

	CDnWeapon::Update( m_Cross );
	CDnWeapon::Process( LocalTime, fDelta );

	if(  0 != m_InvalidLocalTime ) 
		ProcessDestroyOrbit( LocalTime, fDelta );

	ProcessValid( LocalTime, fDelta );

	m_vPrevPos = m_Cross.m_vPosition;

	m_bFirstProcess = false;

	// 301번 외딴섬에서 플레이어의 위치가 틀어졌을 때 칵퉤꽃 발사체의 방향 벡터가 0, 0, 0 으로 셋팅되어 사라지지 않는 경우 방어코드 넣음.
	if( EtVec3LengthSq( &m_Cross.m_vZAxis ) < 0.001f )
	{
		// Projectile 타입의 발사체는 속도가 0 으로 셋팅되어있을 수 있다. 실시간으로 속도를 계산하기 때문에~
		if( Projectile != m_OrbitType )
			SetDestroy();
	}
}

void CDnProjectile::MakeHitParam( CDnDamageBase::SHitParam &HitParam )
{
	HitParam = m_HitParam;

	HitParam.RemainTime = m_LastHitSignalEndTime;
	HitParam.hHitter = m_hShooter;
	HitParam.hWeapon = GetMySmartPtr();
	HitParam.bFirstHit = true;
	HitParam.vPosition = m_vHitPos;

	//  영역이 있는 히트 시그널을 사용하는 폭발하는 발사체는 이미 hit 시그널 중심점을 기준으로 제대로된 뷰 벡터가 들어가 있음. 
	if( !m_bProcessingBombHitSignal )
		HitParam.vViewVec = -m_Cross.m_vZAxis;

	HitParam.iUniqueID = m_iHitUniqueID;

	if( -1.0f != m_fHitApplyPercent )
		HitParam.fDamage = m_fHitApplyPercent;
}

void CDnProjectile::OnSignal( SignalTypeEnum Type, void *pPtr, LOCAL_TIME LocalTime, LOCAL_TIME SignalStartTime, 
							  LOCAL_TIME SignalEndTime, int nSignalIndex )
{
	switch( Type ) {
		case STE_Hit:
			{
				//OutputDebug( "STE_Hit\n" );

				if (m_hShooter && m_hShooter->ProcessIgnoreHitSignal() == true)
					break;

				HitStruct *pStruct = (HitStruct *)pPtr;
#ifdef PRE_FIX_MEMOPT_SIGNALH
				CopyShallow_HitStruct(m_HitStruct, pStruct);
#else
				m_HitStruct = *pStruct;
#endif

				m_HitParam.szActionName = pStruct->szTargetHitAction;
				m_HitParam.fDamage = pStruct->fDamageProb * 0.01f;
				m_HitParam.fDurability = pStruct->fDurabilityDamageProb * 0.01f;
				m_HitParam.vVelocity = *pStruct->vVelocity;
				m_HitParam.vResistance = *pStruct->vResistance;
				m_HitParam.fStiffProb = pStruct->fStiffProb * 0.01f;
				m_HitParam.nSkillSuperAmmorIndex = pStruct->nApplySuperAmmorIndex - 1;
				m_HitParam.nSkillSuperAmmorDamage = pStruct->nApplySuperAmmorDamage;
				m_HitParam.nDamageType = pStruct->nDamageType;
				m_HitParam.DistanceType	= (pStruct->nDistanceType == 0) ? CDnDamageBase::DistanceTypeEnum::Melee : CDnDamageBase::DistanceTypeEnum::Range;
				m_HitParam.cAttackType = (char)pStruct->nAttackType;
				
				if( ElementEnum_Amount != m_eForceHitElement )
				{
					m_HitParam.HasElement = m_eForceHitElement;
				}
				else
				{
					m_HitParam.HasElement = ( m_hShooter ) ? m_hShooter->CalcHitElementType( ( pStruct->bUseSkillApplyWeaponElement == TRUE ) ? true : false, m_ParentSkillInfo.eSkillElement, true ) : CDnActorState::ElementEnum_Amount;
				}

				m_HitParam.bIgnoreCanHit = ( pStruct->bIgnoreCanHit == TRUE );
				m_HitParam.bIgnoreParring = ( pStruct->bIgnoreParring == TRUE );

				m_HitParam.nHitLimitCount = pStruct->nHitLimitCount;

				if( m_bValidDamage )
				{
					bool bHitResult = ProcessDamage( LocalTime, 0.0f, SignalStartTime, SignalEndTime, nSignalIndex, pStruct->bUseHitAreaOnProjectile );
					if( bHitResult )
					{
						// #59118 발사체도 g_Lua_BeHitSkill 적용한다.
						if( m_hShooter && m_hShooter->IsMonsterActor() )
						{
							CDnMonsterActor* pMonster = static_cast<CDnMonsterActor*>(m_hShooter.GetPointer());
#ifdef PRE_FIX_PROPMON_PROJ
							//	프랍 몬스터의 경우 MonsterActor이면서도 AI를 가지지 않는 경우가 있음. (#59952)
							if (pMonster)
							{
								MAAiBase* pCurBase = pMonster->GetAIBase();
								if (pCurBase)
									pCurBase->OnHitFinish( LocalTime, &m_HitStruct );
							}
#else
							pMonster->GetAIBase()->OnHitFinish( LocalTime, &m_HitStruct );
#endif
						}
					}
				}

				m_bHitSignalProcessed = true;

				// 폭발시 히트 시그널이 뒤에 있는 경우가 있으므로 뒤에 있는 케이스가 있어서 히트 시그널 시작 시에 플래그 켜줌.
				// 안그러면 m_bValidDamage가 false 가 되어 폭발 처리가 안됨.
				if( m_bOnCollisionCalled && m_bHitActionStarted )
					m_bBombHitSignalStarted = true; 
			}
			break;
			case STE_SummonMonster:
			{
				SummonMonsterStruct* pStruct = (SummonMonsterStruct *)pPtr;

				CDnGameTask* pGameTask = static_cast<CDnGameTask*>(CTaskManager::GetInstancePtr(GetRoom())->GetTask( "GameTask" ));
				if( pGameTask )
				{
					if( m_hShooter )
					{
						// 내부적으로 값을 바꿔서 사용하기 때문에 반드시 복사해서 사용한다.
#ifdef PRE_FIX_MEMOPT_SIGNALH
						SummonMonsterStruct Struct;
						CopyShallow_SummonMonsterStruct(Struct, pStruct);
#else
						SummonMonsterStruct Struct = *pStruct;
#endif

						// 167번 소환 몬스터 스킬레벨 강제 셋팅 상태효과 처리. ///////////////////
						if( m_hShooter->IsPlayerActor() )
						{
							if( 0 < m_iSummonMonsterForceSkillLevel )
								Struct.nForceSkillLevel = m_iSummonMonsterForceSkillLevel;
						}
						//////////////////////////////////////////////////////////////////////////

						// 발사체에서 몬스터 소환시 발사체의 위치로 대체시켜 생성시켜 준다. #18315

						// #36873 네비게이션 메시나 attribute 가 무시되므로 보정된 위치로..
						// #37991 이전 위치 기준으로 네비 메시가 맞는지 확인해보고 아니라면 현재 캐릭터가 서 있는 곳으로 소환시켜준다.
						MatrixEx CrossRevisionedPos;
						if( true == INSTANCE(CDnWorld).IsOnNavigationMesh( m_vPrevPos ) )
						{
							CrossRevisionedPos = m_Cross;
							INSTANCE(CDnWorld).RevisionPosByNaviMesh( CrossRevisionedPos, m_vPrevPos );
						}
						else
						{
							CrossRevisionedPos.m_vPosition = m_hShooter->GetMatEx()->m_vPosition;
							if( Struct.PositionCheck == TRUE )
								break;
						}

						EtVector3 vTemp = *(m_hShooter->GetPosition());;
						m_hShooter->SetPosition( CrossRevisionedPos.m_vPosition );
						pGameTask->RequestSummonMonster( m_hShooter, &Struct, false, m_ParentSkillInfo.iSkillID );
						m_hShooter->SetPosition( vTemp );
					}
				}
				break;
			}
		case STE_Gravity:
			{
				if( !m_hShooter ) break;
				GravityStruct *pStruct = (GravityStruct *)pPtr;

				EtVector3 vPos = m_Cross.m_vPosition + *pStruct->vOffset;
				DNVector(DnActorHandle) hVecList;
				CDnActor::ScanActor( GetRoom(), vPos, pStruct->fDistanceMax, hVecList );

				for( DWORD i=0; i<hVecList.size(); i++ ) {
					if( !hVecList[i] ) continue;
					if( hVecList[i] == m_hShooter ) continue;
					if( hVecList[i]->IsDie() ) continue;
					if( hVecList[i]->GetWeight() == 0.f ) continue;
					if( hVecList[i]->IsNpcActor() ) continue;
					if( hVecList[i]->ProcessIgnoreGravitySignal() ) continue;

					bool bHittable = false;
					switch( pStruct->nTargetType ) {
						case 0: // Enemy
							if( hVecList[i]->GetTeam() == m_hShooter->GetTeam() ) break;
							bHittable = true;
							break;
						case 1: // Friend
							if( hVecList[i]->GetTeam() != m_hShooter->GetTeam() ) break;
							bHittable = true;
							break;
						case 2: // All
							bHittable = true;
							break;
					}
					if( !bHittable ) 
						continue;

					MAMovementBase *pMovement = hVecList[i]->GetMovement();
					if( !pMovement ) continue;

					EtVector3 vDir = *hVecList[i]->GetPosition() - vPos;
					float fDistance = EtVec3Length( &vDir );
					float fDistance2 = EtVec3Length( &EtVector3( vDir.x, 0.f, vDir.z ) );
					EtVec3Normalize( &vDir, &vDir );

					float fTemp = pStruct->fMinVelocity + ( ( (pStruct->fMaxVelocity - pStruct->fMinVelocity) / pStruct->fDistanceMax ) * ( pStruct->fDistanceMax - fDistance ) );

					EtVector3 vVel;
					if( pMovement ) {
						pMovement->Look( EtVec3toVec2(vDir) );
						pMovement->SetVelocityZ( -fTemp );
						pMovement->SetResistanceZ( fTemp * 2.f );
					}
				}
			}
			break;
	}
	CDnWeapon::OnSignal( Type, pPtr, LocalTime, SignalStartTime, SignalEndTime, nSignalIndex );
}


void CDnProjectile::OnDamageSuccess( DnActorHandle hDamagedActor, CDnDamageBase::SHitParam &HitParam )
{
	// 여기서 피어싱이나 기타 등등의 타입에 따라서 없엘건지 따위를 셋팅해주게 하자.
	// 폭발하는 거 맞았을 땐 Destroy 처리 해주지 않는다.
	if( false == m_bPierce && /*m_bHasHitAction &&*/ !m_bOnCollisionCalled )
	{
		ShowWeapon( false );
		m_bValidDamage = false;
		m_InvalidLocalTime = m_LocalTime;
		m_DestroyOrbitType = FallGravity;
		m_nDestroyOrbitTimeGap = 1000;
		m_bStick = true;
	}

	// 발사체를 쏜 액터에게 발사체가 명중했음을 알린다.
	if( m_hShooter )
	{
		m_hShooter->OnHitProjectile( m_LocalTime, hDamagedActor, HitParam );
	}
}



void CDnProjectile::AddStateEffect( CDnSkill::StateEffectStruct &Struct )
{
#if defined(PRE_FIX_59336)
	//242번 상태효과만 따로 리스트로 관리 하도록 한다.
	if (Struct.nID == STATE_BLOW::BLOW_242)
	{
		m_ComboLimitStateEffectList.push_back(Struct);
	}
	else
	{
		m_VecStateEffectList.push_back( Struct );
	}
#else
	m_VecStateEffectList.push_back( Struct );
#endif // PRE_FIX_59336
}


void CDnProjectile::OnCollisionWithActor( void )
{
	if( false == m_bPierce && false == m_bOnCollisionCalled )
	{
		if( m_bHasHitAction )
		{
#if defined(PRE_ADD_55295)
			//#55295
			//히트 액션으로 변경될때 이 플래그가 설정 되어 있으면 방향벡터를 평면과 평행? 하도록 변경한다.
			if (m_bHitActionVectorInit)
				ChangeProjectileRotation();
#endif // PRE_ADD_55295

			SetActionQueue( HIT_ACTION_NAME, 0, 0.0f );

			m_bOnCollisionCalled = true;

			if( false == m_bValidDamage )
				m_bValidDamage = true;

			// hit 액션 길이만큼 시간 뽑아둠
			CEtActionBase::ActionElementStruct* pActionElement = GetElement( HIT_ACTION_NAME );
			m_nDestroyOrbitTimeGap = int(((float)pActionElement->dwLength / CDnActionBase::m_fFPS) * 1000.0f);

			// 새로운 폭발 hit 시그널이 돌아갈 것이므로 발사체 맞은 첫 액터 기준 end signal 시간은 초기화 시킨다. 안그러면 폭발 데미지 안들어감.
			m_LastHitSignalEndTime = 0;
		}
	}
}


void CDnProjectile::OnCollisionWithGround( void )
{
	if( false == m_bOnCollisionCalled )
	{
		if( m_bHasHitAction )
		{
#if defined(PRE_ADD_55295)
			//#55295
			//히트 액션으로 변경될때 이 플래그가 설정 되어 있으면 방향벡터를 평면과 평행? 하도록 변경한다.
			if (m_bHitActionVectorInit)
				ChangeProjectileRotation();
#endif // PRE_ADD_55295

			SetActionQueue( HIT_ACTION_NAME, 0, 0.0f );

			m_bOnCollisionCalled = true;

			if( false == m_bValidDamage )
				m_bValidDamage = true;

			// hit 액션 길이만큼 시간 뽑아둠
			CEtActionBase::ActionElementStruct* pActionElement = GetElement( HIT_ACTION_NAME );
			m_nDestroyOrbitTimeGap = int(((float)pActionElement->dwLength / CDnActionBase::m_fFPS) * 1000.0f);

			// TargetPositionType 이면 목적 위치 곧바로 셋팅
			if( 2 == m_TargetType )
				m_Cross.SetPosition( m_vTargetPosition );

			// 새로운 폭발 hit 시그널이 돌아갈 것이므로 발사체 맞은 첫 액터 기준 end signal 시간은 초기화 시킨다. 안그러면 폭발 데미지 안들어감.
			m_LastHitSignalEndTime = 0;
		}
	}
}


void CDnProjectile::OnCollisionWithProp( void )
{
	if( false == m_bOnCollisionCalled )
	{
		if( m_bHasHitAction )
		{
#if defined(PRE_ADD_55295)
			//#55295
			//히트 액션으로 변경될때 이 플래그가 설정 되어 있으면 방향벡터를 평면과 평행? 하도록 변경한다.
			if (m_bHitActionVectorInit)
				ChangeProjectileRotation();
#endif // PRE_ADD_55295

			// 이미 hit 액션을 실행하고 있다면 액션을 다시 실행시키지 않는다. (#21384)
			SetActionQueue( HIT_ACTION_NAME, 0, 0.0f );

			m_bOnCollisionCalled = true;

			if( false == m_bValidDamage )
				m_bValidDamage = true;

			// hit 액션 길이만큼 시간 뽑아둠
			CEtActionBase::ActionElementStruct* pActionElement = GetElement( HIT_ACTION_NAME );
			m_nDestroyOrbitTimeGap = int(((float)pActionElement->dwLength / CDnActionBase::m_fFPS) * 1000.0f);

			// 새로운 폭발 hit 시그널이 돌아갈 것이므로 발사체 맞은 첫 액터 기준 end signal 시간은 초기화 시킨다. 안그러면 폭발 데미지 안들어감.
			m_LastHitSignalEndTime = 0;
		}
		else
		{
			// 프랍에 부딪혔다면 hit 액션이 없거나 hit 액션은 있되 hit signal 이 없는 발사체는 곧바로 삭제되도록 처리.
			SetDestroy();
		}
	}
}


void CDnProjectile::OnChangedNextActionToHit( void )
{
	if( false == m_bOnCollisionCalled )
	{
		if( m_bHasHitAction )
		{
			m_bOnCollisionCalled = true;

			if( false == m_bValidDamage )
				m_bValidDamage = true;

			// hit 액션 길이만큼 시간 뽑아둠
			CEtActionBase::ActionElementStruct* pActionElement = GetElement( HIT_ACTION_NAME );
			m_nDestroyOrbitTimeGap = int(((float)pActionElement->dwLength / CDnActionBase::m_fFPS) * 1000.0f);

			// TargetPositionType 이면 목적 위치 곧바로 셋팅
			if( 2 == m_TargetType )
				m_Cross.SetPosition( m_vTargetPosition );
		}
	}
}


void CDnProjectile::OnChangeAction( const char *szPrevAction )
{
	if( m_bHasHitAction )
	{
		if( NULL != szPrevAction && 
			strcmp( SHOOT_ACTION_NAME, szPrevAction ) )
		{
			m_bHitActionStarted = true;
		}
	}

	// 히트된 대상이 또 히트되는것을 방지하기위해서 m_LastHitSignalEndTime이 존재하는데 , 발사체의 액션자체가 변할경우 이전 시그널 기준의 EndTime 이 설정되기때문에
	// 액션이 변하고 새로운 Hit가 들어와도 이전값을 사용해서 오작동이 일어나게된다
	// 그래서 액션이 바뀌면 m_LastHitSignalEndTime을 초기화 해주도록 하자 , 명확하게 하려면 발사체 액션마다 Hit된 대상의 이전정보를 기억해서 맞은 대상이 또 맞지않도록
	// 설정을 해야하지만 일단은 짜여진 대로 설정하도록 합니다.
	m_LastHitSignalEndTime = 0;
}

void CDnProjectile::OnFinishAction( const char* szPrevAction, LOCAL_TIME time )
{
	// 폭발 액션 종료시엔 곧바로 삭제.
	if( m_bHasHitAction ) 
		if( 0 == strcmp( HIT_ACTION_NAME, szPrevAction ) )
			SetDestroy();
}


// NOTE: CrossHair 프로젝타일인 경우 서버 쪽에서는 프로젝타일 관련 정보를 클라에서 생성된 것을 그대로 받기 때문에 (CS_PROJECTILE)
// 여기서 생성되지 않습니다.
// 발사체 시그널을 직접 받아서 생성.
CDnProjectile *CDnProjectile::CreateProjectile( CMultiRoom *pRoom, DnActorHandle hShooter, MatrixEx &LocalCross, ProjectileStruct *pStruct, 
												EtVector3* pForceTargetPos/*=NULL*/, DnActorHandle hForceTarget/*=CDnActor::Identity()*/, EtVector3* pForceDir/* = NULL*/ )
{
	MatrixEx CrossResult = LocalCross;
	CrossResult.m_vPosition += CrossResult.m_vXAxis * ( pStruct->vOffset->x * hShooter->GetScale()->x );
	CrossResult.m_vPosition += CrossResult.m_vYAxis * ( pStruct->vOffset->y * hShooter->GetScale()->y );
	CrossResult.m_vPosition += CrossResult.m_vZAxis * ( pStruct->vOffset->z * hShooter->GetScale()->z );

	CDnProjectile *pProjectile = NULL;

	float fVelocityMultiply = 1.0f;

	// 시그널 구조체에 있는 방향값은 길이와 관계 없이 방향만 의미가 있고,
	// CrossResult.m_vZAxis 를 정규화시키고 속도는 오로지 시그널 구조체에서 정해진 fSpeed 로 결정된다.

	switch( pStruct->nTargetType )
	{
		case TargetTypeEnum::CrossHair:
			{
				SAFE_DELETE( pProjectile );
				return NULL;
			}
			break;

		case TargetTypeEnum::Direction:
			{
				pProjectile = new CDnProjectile( pRoom, hShooter );

				EtVector3	vDirection	= *pStruct->vDirection;
				bool		bAILook		= false;

				// MonsterActor
				if( hShooter && hShooter->IsMonsterActor() )
				{
					CDnMonsterActor *pMonster = static_cast<CDnMonsterActor *>(hShooter.GetPointer());

					if( pMonster && pMonster->bIsAILook() )
					{
						// STE_ProjectileTargetPosition 은 TargetPosition 인 경우에만 사용하는 시그널이다.
						if( !pMonster->bIsProjectileTargetSignal() )
						{
							vDirection	= EtVec2toVec3( *pMonster->GetAIBase()->GetAILook() );
							bAILook		= true;
						}
					}
				}

				EtVector3 vTemp;
				if( bAILook )
					vTemp = vDirection;
				else
					EtVec3TransformNormal( &vTemp, &vDirection, LocalCross );

				if( FALSE == pStruct->bNormalizeDirectionVector )
				{
					fVelocityMultiply = EtVec3Length( &vTemp );
					CrossResult.m_vZAxis = vTemp / fVelocityMultiply;
				}
				else
				{
					fVelocityMultiply = 1.0f;
					EtVec3Normalize( &vTemp, &vTemp );
					CrossResult.m_vZAxis = vTemp;
				}

				CrossResult.MakeUpCartesianByZAxis();
			}
			break;

		case TargetTypeEnum::TargetPosition:
		case TargetTypeEnum::Target:
		case TargetTypeEnum::Shooter:
			{
				if( hShooter->IsPlayerActor() )
				{
					SAFE_DELETE( pProjectile );
					return NULL;
				}
				else if( hShooter->IsMonsterActor() )
				{
					pProjectile = new CDnProjectile( pRoom, hShooter );

					CDnMonsterActor *pMonster = static_cast<CDnMonsterActor *>(hShooter.GetPointer());

					// 발사 대상이 Shooter 자기 자신인 경우
					DnActorHandle hTarget;

					// ReserveProjectileTarget 시그널로 미리 정해두었던 타겟이 있다면 받아온다.
					// 없으면 원래 얻어오는 AI 에게서 얻어옴.
					hTarget = pMonster->GetReservedProjectileTarget();
					if( !hTarget )
						hTarget = pMonster->GetAggroTarget();

					EtVector3 vTargetPos;

					bool bTarget = true;
					// 강제타겟설정
					if( pForceTargetPos )
						vTargetPos = *pForceTargetPos;
					else if( pStruct->nTargetType == TargetTypeEnum::TargetPosition && pMonster->bIsProjectileTargetSignal() )
						vTargetPos = *pMonster->GetAIBase()->GetProjectileTarget();
					else if( hTarget )
						vTargetPos = *hTarget->GetPosition();
					else
						bTarget = false;

					if( bTarget )
					{
						// TargetPosRandomValue
						if( pStruct->nTargetPosRandomValue > 0 )
						{
							_srand( pRoom, pProjectile->GetUniqueID() );
							vTargetPos.x += cos( EtToRadian( _rand(pRoom)%360 ) ) * pStruct->nTargetPosRandomValue;
							vTargetPos.z += sin( EtToRadian( _rand(pRoom)%360 ) ) * pStruct->nTargetPosRandomValue;
						}

						float fHeight = CDnWorld::GetInstance(pRoom).GetHeight( vTargetPos );
						
						// #28665 대포 몹이 아닌 경우에만. 계산된 대포의 목적지는 건드리지 않는다.
#ifdef PRE_FIX_PARTSMONSTER_AI_TARGETTING
						if( hTarget && fHeight < vTargetPos.y && (pMonster->GetActorType() != CDnActorState::Cannon) && hTarget->IsPartsMonsterActor() == false )
							vTargetPos -= hTarget->GetMatEx()->m_vZAxis * ( vTargetPos.y - fHeight );
#else
						if( hTarget && fHeight < vTargetPos.y && (pMonster->GetActorType() != CDnActorState::Cannon) )
							vTargetPos -= hTarget->GetMatEx()->m_vZAxis * ( vTargetPos.y - fHeight );
#endif
					}
					else
					{
						vTargetPos = *hShooter->GetPosition() + ( hShooter->GetMatEx()->m_vZAxis * pMonster->GetThreatRange() );
					}

					// TargetPosition
					if( pStruct->nTargetType == TargetTypeEnum::TargetPosition ) 
					{
						pProjectile->SetTargetPosition( vTargetPos );

						CrossResult.m_vZAxis = vTargetPos - CrossResult.GetPosition();
						EtVec3Normalize( &CrossResult.m_vZAxis, &CrossResult.m_vZAxis );
					}

					// Target
					if( pStruct->nTargetType == TargetTypeEnum::Target )
					{
						// 강제타겟설정
						if( hForceTarget )
							hTarget = hForceTarget;

						if( !hTarget ) 
							hTarget = hShooter;

						if (pStruct->nTargetStateIndex != 0)
						{
							vTargetPos = *hTarget->GetPosition();
							pProjectile->SetTargetPosition(vTargetPos);
						}

						pProjectile->SetTargetActor( hTarget );
					}
					else if( pStruct->nTargetType == TargetTypeEnum::Shooter )
					{
						hTarget = hShooter;
						vTargetPos = *hTarget->GetPosition();

						pProjectile->SetTargetActor( hTarget );
						pProjectile->SetTargetPosition( vTargetPos );
					}

					//목표 지점으로 날아 가는 발사체인 경우 현재 발사체 위치에서 목표 지점으로의 방향 벡터 변경 해야 한다.
					//일단 낙인용 발사체에서만 동작 하도록 수정한다.
					if (pStruct->nTargetStateIndex != 0)
					{
						CrossResult.m_vZAxis = vTargetPos - CrossResult.GetPosition();
						EtVec3Normalize( &CrossResult.m_vZAxis, &CrossResult.m_vZAxis );
					}
					else if( EtVec3LengthSq( pStruct->vDirection ) == 0.f ) 
					{
						_ASSERT( !(pStruct->nOrbitType == Projectile && pStruct->VelocityType == Accell && pStruct->nTargetType == 2) &&
							"중력 가속도가 적용되는 Projectile/TargetPosition 타입의 발사체는 반드시 방향을 정해줘야 합니다." );
						CrossResult.m_vZAxis = vTargetPos - CrossResult.GetPosition();
						EtVec3Normalize( &CrossResult.m_vZAxis, &CrossResult.m_vZAxis );
					}
					else if( !( (pStruct->nOrbitType == OrbitTypeEnum::Linear) && 
						(pStruct->nTargetType == TargetTypeEnum::TargetPosition) ) )
					{
						// 대포에서 유저가 정한 방향으로 쏘아야할 경우 해당 방향으로 셋팅해준다.
						if( NULL == pForceDir )
						{
							EtVector3 vTemp;
							EtVec3TransformNormal( &vTemp, pStruct->vDirection, LocalCross );

							if( pStruct->bUseTargetPositionDir )
							{
								EtVector3 vTargetPosDir = vTargetPos - CrossResult.m_vPosition;
								EtVec3Normalize( &vTargetPosDir, &vTargetPosDir );

								MatrixEx CrossTargetPosDir;
								CrossTargetPosDir.m_vZAxis = vTargetPosDir;
								CrossTargetPosDir.MakeUpCartesianByZAxis();

								EtVector3 vToolDefinedDir = *pStruct->vDirection;
								EtVector3 vDirZ( vToolDefinedDir.x, 0.0f, vToolDefinedDir.z );
								EtVec3Normalize( &vDirZ, &vDirZ );
								EtVec3Normalize( &vToolDefinedDir, &vToolDefinedDir );
								float fDot = EtVec3Dot( &vToolDefinedDir, &vDirZ );
								float fRotX = EtAcos( fDot );

								CrossTargetPosDir.RotatePitch( EtToDegree( -fRotX ) );
								vTemp = CrossTargetPosDir.m_vZAxis;
							}

							if( FALSE == pStruct->bNormalizeDirectionVector )
							{
								fVelocityMultiply = EtVec3Length( &vTemp );
								CrossResult.m_vZAxis = vTemp / fVelocityMultiply;
							}
							else
							{
								fVelocityMultiply = 1.0f;
								EtVec3Normalize( &vTemp, &vTemp );
								CrossResult.m_vZAxis = vTemp;
							}
						}
						else
						{
							EtVector3 vTemp;
							EtVec3TransformNormal( &vTemp, pForceDir, LocalCross );
							EtVec3Normalize( &CrossResult.m_vZAxis, &vTemp );
							pProjectile->SetForceDir( *pForceDir );
						}
					}

					CrossResult.MakeUpCartesianByZAxis();
				}
			}
			break;

		case TargetTypeEnum::DestPosition:
			{
				pProjectile = new CDnProjectile( pRoom, hShooter );

				EtVector3 vDestOffset = *(pStruct->vDestPosition);
				if( pStruct->nTargetPosRandomValue > 0 )
				{
					_srand( pRoom, pProjectile->GetUniqueID() );
					vDestOffset.x += cos( EtToRadian( _rand(pRoom)%360 ) ) * pStruct->nTargetPosRandomValue;
					vDestOffset.z += sin( EtToRadian( _rand(pRoom)%360 ) ) * pStruct->nTargetPosRandomValue;
				}

				// 방향벡터가 툴에서 설정되어있다면 dest position 이지만 그쪽 방향으로 돌려줌..
				EtVector3 vDirection;
				if( EtVec3LengthSq( pStruct->vDirection ) != 0.f )
				{
					vDirection = vDestOffset;
					EtVec3Normalize( &vDirection, &vDirection );

					MatrixEx CrossDestDir;
					CrossDestDir.m_vZAxis = vDirection;
					CrossDestDir.MakeUpCartesianByZAxis();

					EtVector3 vToolDefinedDir = *pStruct->vDirection;
					EtVector3 vDirZ( vToolDefinedDir.x, 0.0f, vToolDefinedDir.z );
					EtVec3Normalize( &vDirZ, &vDirZ );
					EtVec3Normalize( &vToolDefinedDir, &vToolDefinedDir );
					float fDot = EtVec3Dot( &vToolDefinedDir, &vDirZ );
					float fRotX = EtAcos( fDot );

					CrossDestDir.RotatePitch( EtToDegree( -fRotX ) );
					vDirection = CrossDestDir.m_vZAxis;
				}
				else
				{
					// 방향 벡터가 설정되어있지 않다면 dest position 쪽으로 방향을 잡아준다.
					vDirection = vDestOffset - (*pStruct->vOffset);
					EtVec3Normalize( &vDirection, &vDirection );
				}

				EtVector3 vWorldDirection;
				EtVec3TransformNormal( &vWorldDirection, &vDirection, LocalCross );
				CrossResult.m_vZAxis = vWorldDirection;
				EtVec3Normalize( &CrossResult.m_vZAxis, &CrossResult.m_vZAxis );

				CrossResult.MakeUpCartesianByZAxis();

				// 이미 DestPosition 을 향해 방향이 정해진 상태이므로 목적지는 로컬 Z축으로 거리값만큼 이동시키면된다.
				float fDestLength = EtVec3Length( &vDestOffset );
				MatrixEx crossDestPosition = CrossResult;
				crossDestPosition.m_vPosition += CrossResult.m_vZAxis * fDestLength;

				crossDestPosition.m_vPosition.y = CDnWorld::GetInstance( hShooter->GetRoom() ).GetHeightWithProp( crossDestPosition.m_vPosition );

				pProjectile->SetTargetPosition( crossDestPosition.m_vPosition );
			}
			break;
	}

	if( pStruct->nWeaponTableID > 0 ) {
		if( pStruct->nProjectileIndex != -1 ) {
			DnWeaponHandle hWeapon = CDnWeapon::GetSmartPtr( (CMultiRoom*)g_pGameServerManager->GetRootRoom(), pStruct->nProjectileIndex );
			if( hWeapon ) *(CDnWeapon*)pProjectile = *hWeapon.GetPointer();
		}
		else {
			if (pProjectile == NULL)
			{
				_DANGER_POINT();
				return NULL;
			}

			pProjectile->CDnWeapon::Initialize( pStruct->nWeaponTableID, 0 );
			pProjectile->CreateObject();
		}
	}
	else if( hShooter->GetWeapon(1) ) {
		*(CDnWeapon*)pProjectile = *hShooter->GetWeapon(1);
	}
	else
	{
		SAFE_DELETE( pProjectile );
		return NULL;
	}

	pProjectile->SetWeaponType( (WeaponTypeEnum)( pProjectile->GetWeaponType() | WeaponTypeEnum::Projectile ) );
	pProjectile->Initialize( CrossResult, (CDnProjectile::OrbitTypeEnum)pStruct->nOrbitType, (CDnProjectile::DestroyOrbitTypeEnum)pStruct->nDestroyOrbitType,
		(CDnProjectile::TargetTypeEnum)pStruct->nTargetType);

	pProjectile->SetSpeed( pStruct->fSpeed * fVelocityMultiply );
	pProjectile->SetResistance( pStruct->fResistance );
	pProjectile->SetValidTime( pStruct->nValidTime );

	if( pProjectile->GetOrbitType() == OrbitTypeEnum::Homing ||
		pProjectile->GetOrbitType() == OrbitTypeEnum::TerrainHoming ||
		pProjectile->GetOrbitType() == OrbitTypeEnum::Projectile )
	{
		pProjectile->SetVelocityType( (VelocityTypeEnum)pStruct->VelocityType );
	}

	int nLength = pProjectile->GetWeaponLength();
	if( pStruct->bIncludeMainWeaponLength )
	{
		DnWeaponHandle hWeapon = hShooter->GetWeapon(0);
		if ( hWeapon )
			nLength += hWeapon->GetWeaponLength();
	}

	if( hShooter->IsProcessSkill() )
	{
		nLength += hShooter->GetProcessSkill()->GetIncreaseRange();
	}

	if( hShooter->IsEnabledToggleSkill() )
	{
		nLength += hShooter->GetEnabledToggleSkill()->GetIncreaseRange();
	}

	pProjectile->SetWeaponLength( nLength );

	pProjectile->SetPierce( ( pStruct->bPierce == TRUE ) ? true : false );
	pProjectile->SetMaxHitCount( pStruct->nMaxHitCount );
	pProjectile->SetProjectileOrbitRotateZ( pStruct->fProjectileOrbitRotateZ );
	pProjectile->SetTraceHitTarget( (pStruct->bTraceHitTarget == TRUE) ? true : false, 
									(pStruct->bTraceHitActorHittable == TRUE) ? true : false );

#if defined(PRE_FIX_52329)
	pProjectile->SetIgnoreHitType(pStruct->nIgnoreHitType);
#endif // PRE_FIX_52329

#if defined(PRE_ADD_55295)
	pProjectile->SetHitActionVectorInit(pStruct->bHitActionVectorInit == TRUE ? true : false);
#endif // PRE_ADD_55295

	pProjectile->PostInitialize();

	return pProjectile;
}


// 클라로부터 패킷을 받아서 생성하는 발사체 객체.
// 각 클라의 LocalPlayer 들로부터 패킷들이 올라옴.
CDnProjectile* CDnProjectile::CreatePlayerProjectileFromClientPacket( DnActorHandle hShooter, const BYTE* pPacket )
{
	if( false == hShooter->IsPlayerActor() )
		return NULL;

	CPacketCompressStream Stream( const_cast<BYTE*>(pPacket), 128 );
	CDnPlayerActor* pPlayerActor = static_cast<CDnPlayerActor*>(hShooter.GetPointer());
	if( pPlayerActor->IsInvalidPlayerChecker() ) return NULL;

	bool bActorAttachWeapon;
	int nWeaponTableID, nWeaponLength = 0;
	DWORD dwUniqueID,dwGap;
	float fSpeed = 0.f;
	float fResistance = 0.f;
	int nValidTime = 0;
	int VelocityType = 0;
	EtVector3 vTargetPos;
	DWORD dwTargetUniqueID;
	CDnProjectile::OrbitTypeEnum OrbitType;
	CDnProjectile::DestroyOrbitTypeEnum DestroyType;
	CDnProjectile::TargetTypeEnum TargetType;
	MatrixEx Cross;
	EtVector3 vDirection( 0.0f, 0.0f, 0.0f );
	int nActionIndex;
	int nMaxHitCount = 0;
	int nSignalIndex = -1;
	float fProjectileOrbitRotateZ = 0.0f;
	ProjectileStruct* pProjectileStruct = NULL;

	Stream.Read( &dwGap, sizeof(DWORD) );
	Stream.Read( &bActorAttachWeapon, sizeof(bool) );
	Stream.Read( &dwUniqueID, sizeof(DWORD) );
	if( !bActorAttachWeapon ) 
	{
		Stream.Read( &nWeaponTableID, sizeof(int) );
	}

	Stream.Read( &Cross.m_vPosition, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
	Stream.Read( &Cross.m_vXAxis, sizeof(EtVector3), CPacketCompressStream::VECTOR3_SHORT );
	Stream.Read( &Cross.m_vYAxis, sizeof(EtVector3), CPacketCompressStream::VECTOR3_SHORT );
	Stream.Read( &Cross.m_vZAxis, sizeof(EtVector3), CPacketCompressStream::VECTOR3_SHORT );

#ifdef PRE_MOD_PROJECTILE_HACK
	char cShooterType;
	DWORD dwParentShooterUniqueID = -1;
	DWORD dwShooterUniqueID = -1;
	INT64 nShooterSerialID = -1;
	int nShooterActionIndex;
	int nShooterSignalIndex;
	bool bSendSerialID;
	DnWeaponHandle hParentProjectileWeapon;

	Stream.Read( &cShooterType, sizeof(char) );
	Stream.Read( &dwParentShooterUniqueID, sizeof(DWORD) );
	switch( cShooterType ) {
		case 0:
			dwShooterUniqueID = dwParentShooterUniqueID;
			break;
		case 1:
			{
				Stream.Read( &bSendSerialID, sizeof(bool) );
				if( bSendSerialID )
					Stream.Read( &nShooterSerialID, sizeof(INT64) );
				else Stream.Read( &dwShooterUniqueID, sizeof(DWORD) );
			}
			break;
	}

	Stream.Read( &nShooterActionIndex, sizeof(int), CPacketCompressStream::INTEGER_SHORT );
	Stream.Read( &nShooterSignalIndex, sizeof(int), CPacketCompressStream::INTEGER_CHAR );

	ProjectileStruct *pShooterStruct = NULL;
	nWeaponLength = 0;
	switch( cShooterType ) {
		case 0:
			{
				DnActorHandle hHandle = CDnActor::FindActorFromUniqueID( pPlayerActor->GetRoom(), dwShooterUniqueID );
				if( hHandle ) 
				{
					if( hHandle->GetCurrentActionIndex() == nShooterActionIndex )
					{
						bool bCheckStandAction = CDnChangeStandActionBlow::CheckUsableAction( pPlayerActor->GetActorHandle(), true, pPlayerActor->GetCurrentAction() );
						if( bCheckStandAction == false )
						{
							pPlayerActor->ReportInvalidAction();
							return NULL;
						}

						CEtActionSignal *pSignal = hHandle->GetSignal( nShooterActionIndex, nShooterSignalIndex );
						if( pSignal && pSignal->GetSignalIndex() == STE_Projectile ) 
						{
							pShooterStruct = static_cast<ProjectileStruct*>( pSignal->GetData() );
						}
						else if( hHandle->GetCustomActionIndex() > 0 )
						{
							CEtActionSignal *pCustomActionSignal = hHandle->GetSignal( hHandle->GetCustomActionIndex(), nShooterSignalIndex );
							if( pCustomActionSignal && pCustomActionSignal->GetSignalIndex() == STE_Projectile ) 
							{
								pShooterStruct = static_cast<ProjectileStruct*>( pCustomActionSignal->GetData() );
							}
						}
					}
					else
					{
						// 클라와 서버가 액션 인덱스가 다름. 핵으로 처리.
						return NULL;
					}
				}
			}
			break;
		case 1:
			{
				DnWeaponHandle hHandle;
				if( bSendSerialID ) {
					for( DWORD i=0; i<2; i++ ) { // FindItemFromSerialID 를 만들어야 하지만 일단은 필요없어서 EquipWeapon 에서만 찾습니다.
						DnWeaponHandle hWeapon = pPlayerActor->GetWeapon(i);
						if( hWeapon && hWeapon->GetSerialID() == nShooterSerialID ) {
							hHandle = hWeapon;
							break;
						}
						hWeapon = pPlayerActor->GetCashWeapon(i);
						if( hWeapon && hWeapon->GetSerialID() == nShooterSerialID ) {
							hHandle = hWeapon;
							break;
						}
					}
				}
				else {
					hHandle = CDnWeapon::FindWeaponFromUniqueIDAndShooterUniqueID( pPlayerActor->GetRoom(), dwShooterUniqueID, dwParentShooterUniqueID );
					if( hHandle ) {
						hParentProjectileWeapon = hHandle;
//						nWeaponLength += hHandle->GetWeaponLength();
					}
				}
				if( hHandle ) {
					CEtActionSignal *pSignal = hHandle->GetSignal( nShooterActionIndex, nShooterSignalIndex );
					if( pSignal && pSignal->GetSignalIndex() == STE_Projectile ) {
						pShooterStruct = static_cast<ProjectileStruct*>( pSignal->GetData() );
					}					
				}
			}
			break;
	}
	if( !pShooterStruct ) return NULL;
	pProjectileStruct = pShooterStruct;
	OrbitType = (CDnProjectile::OrbitTypeEnum)pShooterStruct->nOrbitType;
	DestroyType = (CDnProjectile::DestroyOrbitTypeEnum)pShooterStruct->nDestroyOrbitType;
	TargetType = (CDnProjectile::TargetTypeEnum)pShooterStruct->nTargetType;
	nValidTime = pShooterStruct->nValidTime;
	fProjectileOrbitRotateZ = pShooterStruct->fProjectileOrbitRotateZ;
#ifdef PRE_ADD_PROJECTILE_RANDOM_WEAPON
	// 랜덤 발사체 관련 체크
	if( pShooterStruct->RandomWeaponParam && strlen( pShooterStruct->RandomWeaponParam ) > 0 )
	{
		std::vector<int> vecWeaponID;
		vecWeaponID.push_back( pShooterStruct->nWeaponTableID );
		std::vector<std::string> tokens;
		TokenizeA( pShooterStruct->RandomWeaponParam, tokens, "/" );
		if( tokens.size() > 1 )
		{
			for( int i=1; i<static_cast<int>( tokens.size() ); i++ )
			{
				std::vector<std::string> tokensparam;
				TokenizeA( tokens[i].c_str(), tokensparam, ";" );
				if( tokensparam.size() > 0 )
				{
					vecWeaponID.push_back( atoi( tokensparam[0].c_str() ) );
				}
				tokensparam.clear();
			}
		}

		bool bUsableWeaponTableID = false;
		for( int i=0; i<static_cast<int>( vecWeaponID.size() ); i++ )
		{
			if( vecWeaponID[i] == nWeaponTableID )
			{
				bUsableWeaponTableID = true;
				break;
			}
		}

		tokens.clear();
		vecWeaponID.clear();

		if( !bUsableWeaponTableID )	// 사용할 수 없는 WeaponID 임!
			return NULL;
	}
#endif // PRE_ADD_PROJECTILE_RANDOM_WEAPON
#else

	Stream.Read( &OrbitType, sizeof(int), CPacketCompressStream::INTEGER_CHAR );
	Stream.Read( &DestroyType, sizeof(int), CPacketCompressStream::INTEGER_CHAR );
	Stream.Read( &TargetType, sizeof(int), CPacketCompressStream::INTEGER_CHAR );
	Stream.Read( &nValidTime, sizeof(int), CPacketCompressStream::INTEGER_SHORT );
	Stream.Read( &nWeaponLength, sizeof(int) );
	Stream.Read( &fProjectileOrbitRotateZ, sizeof(float) );
#endif

	int nTargetPartsIndex = -1;
	int nTargetBoneIndex = -1;

	Stream.Read(&nTargetPartsIndex, sizeof(int));
	Stream.Read(&nTargetBoneIndex, sizeof(int));

	switch( TargetType )
	{
		case CDnProjectile::CrossHair:
		case CDnProjectile::Direction:
			break;

		case CDnProjectile::TargetPosition:
		case CDnProjectile::Target:
		case CDnProjectile::DestPosition:
		case CDnProjectile::Shooter:
			Stream.Read( &vTargetPos, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
			Stream.Read( &dwTargetUniqueID, sizeof(DWORD), CPacketCompressStream::NOCOMPRESS );
			break;
	}

	Stream.Read( &fSpeed, sizeof(float) );

#ifdef PRE_MOD_PROJECTILE_HACK
	bool bPierce = ( pShooterStruct->bPierce == TRUE );
	nMaxHitCount = pShooterStruct->nMaxHitCount;
	bool bTraceHitTarget = ( pShooterStruct->bTraceHitTarget == TRUE );
	bool bTraceHitActorHittable = (pShooterStruct->bTraceHitActorHittable == TRUE);

	switch( OrbitType )  {
		case CDnProjectile::Linear:
		case CDnProjectile::TerrainLinear:
			break;

		case CDnProjectile::Projectile:
			VelocityType = pShooterStruct->VelocityType;
			break;

		case CDnProjectile::Homing:
		case CDnProjectile::TerrainHoming:
			VelocityType = pShooterStruct->VelocityType;
			break;
		case CDnProjectile::Acceleration:
			fResistance = pShooterStruct->fResistance;
			break;
	}
	Stream.Read( &nActionIndex, sizeof(int), CPacketCompressStream::INTEGER_SHORT );

	if( false == pPlayerActor->IsIgnoreSkillCoolTime() )
	{
		if( nActionIndex != -1 && nActionIndex != pPlayerActor->GetCurrentActionIndex() ) 
		{ 
			// 핵쓴것입니다.
			//break;
			return NULL;
		}
	}
	nSignalIndex = nShooterSignalIndex;

#else
	bool bPierce = false;
	Stream.Read( &bPierce, sizeof(bool) );

	if( bPierce )
		Stream.Read( &nMaxHitCount, sizeof(int) );

	bool bTraceHitTarget = false;
	bool bTraceHitActorHittable = false;
	Stream.Read( &bTraceHitTarget, sizeof(bool) );
	Stream.Read( &bTraceHitActorHittable, sizeof(bool) );

	switch( OrbitType ) 
	{
		case CDnProjectile::Linear:
		case CDnProjectile::TerrainLinear:
			break;

		case CDnProjectile::Projectile:
			Stream.Read( &VelocityType, sizeof(int), CPacketCompressStream::INTEGER_CHAR );
			break;

		case CDnProjectile::Homing:
		case CDnProjectile::TerrainHoming:
			Stream.Read( &VelocityType, sizeof(int), CPacketCompressStream::INTEGER_CHAR );
			break;
		case CDnProjectile::Acceleration:
			Stream.Read( &fResistance, sizeof(float), CPacketCompressStream::FLOAT_SHORT, 1.f );
			break;
	}

	Stream.Read( &nActionIndex, sizeof(int) );

	if( false == pPlayerActor->IsIgnoreSkillCoolTime() )
	{
		if( nActionIndex != -1 && nActionIndex != pPlayerActor->GetCurrentActionIndex() ) 
		{ 
			return NULL;
		}
	}

	// Note 한기: 체인 공격 상태효과를 위해 시그널 struct 를 찾을 수 있는 index 가 필요함.
	Stream.Read( &nSignalIndex, sizeof(int) );

	// Note 한기: projectile 시그널 struct 를 찾는다.
	// 스킬일 때만 ActionIndex가 유효함.
	if( -1 != nSignalIndex && -1 != nActionIndex )
	{
		CEtActionSignal *pSignal = pPlayerActor->GetSignal( nActionIndex, nSignalIndex );
		if( pSignal && pSignal->GetSignalIndex() == STE_Projectile ) {
			pProjectileStruct = static_cast<ProjectileStruct*>( pSignal->GetData() );
		}
	}
#endif

#ifdef PRE_MOD_PROJECTILE_HACK
#else
	// 보우 마스터의 레볼루셔너리 발리스타 같은 발사체어서 발사체 쏘는 스킬에서 최초로 나간 발사체에서
	// 능력치를 이전하기 위한 정보...
	DWORD dwParentID = 0;
	Stream.Read( &dwParentID, sizeof(DWORD) );

	DWORD dwParentShooterUniqueID = 0;
	Stream.Read( &dwParentShooterUniqueID, sizeof(DWORD) );
#endif

#if defined(PRE_FIX_52329)
	int nIgnoreHitType = 0;
	Stream.Read(&nIgnoreHitType, sizeof(nIgnoreHitType));
#endif // PRE_FIX_52329

#if defined(PRE_ADD_55295)
	bool bHitActionVectorInit = false;
	Stream.Read(&bHitActionVectorInit, sizeof(bHitActionVectorInit));
#endif // PRE_ADD_55295

	pPlayerActor->GetPlayerSpeedHackChecker()->OnSyncDatumGap( dwGap );

	CDnProjectile* pParentProjectile = NULL;
	DnSkillHandle hParentSkill;
	bool bFromParentProjectile = false;
#ifndef PRE_MOD_PROJECTILE_HACK
	DnWeaponHandle hParentProjectileWeapon = CDnWeapon::FindWeaponFromUniqueIDAndShooterUniqueID( pPlayerActor->GetRoom(), 
																								 dwParentID, dwParentShooterUniqueID );
#endif
	if( hParentProjectileWeapon )
	{
		// 발사체에서 발사체를 쏘는 경우 클라이언트에서 패킷이 이렇게 온다.
		if( hParentProjectileWeapon->GetWeaponType() == CDnWeapon::Projectile )
		{
			pParentProjectile = static_cast<CDnProjectile*>(hParentProjectileWeapon.GetPointer());
			hParentSkill = pParentProjectile->GetParentSkill();
			if( hParentSkill )		// 발사체에서 발사체를 쏘는 경우의 스킬만 골라서..
			{
				pPlayerActor->AddReservedProjectileCount();
				bFromParentProjectile = true;
			}
		}
	}

	// 발사체에서 발사체를 쏘는 경우는 체크에서 제외한다.
	// 이유는, 발사체 쏘는 위치를 핵에서 서버로 보내더라도 실시간으로 생성되는 부모 발사체의 id 를 같이
	// 보내주지 않으면 바로 위의 루틴에서 걸리기 때문에 발사체에서 발사체를 쏘는 것으로 인정되지 않기 때문이다.
	// 우선 일반 공격인 경우에만 막자.
	if( (false == bFromParentProjectile) &&
		(-1 == nSignalIndex || -1 == nActionIndex) )
	{
		// X, Z 평면 기준으로만 체크한다. 높이까지 체크하는 것은 추후에 필요하면 추가.
		EtVector2 vShooterXZPos( hShooter->GetPosition()->x, hShooter->GetPosition()->z );
		EtVector2 vProjectileShootPosXZ( Cross.m_vPosition.x, Cross.m_vPosition.z );
		EtVector2 vDist = vProjectileShootPosXZ - vShooterXZPos;
		float fDistanceSQ = EtVec2LengthSq( &vDist );
		//#37327 데몰리션 발사체 경우 100을 넘게 Offset값이 설정된 경우가 있음.
		//일단 넉넉하게 최대 값 200으로 체크 하도록 수정(200*200 = 40000)
		if( 40000.0f < fDistanceSQ )
		{
			// 발사체 시작 위치를 임의대로 조작해서 보낸 핵으로 판단.
#ifndef _FINAL_BUILD
			OutputDebug( "발사체 발사 위치 거리 체크에 걸려 핵으로 판단.\n" );
#endif // #ifndef _FINAL_BUILD
			return NULL;
		}
	}

	bool bValidProjectile = true;
	//낙인을 위한 발사체인 경우 발사체 시그널 수 보다 많은 발사체가 발사된다.
	//낙인용 발사체가 아닌경우만 발사체 체크 하도록 수정함.
	if (pShooterStruct->nTargetStateIndex == 0)
		bValidProjectile = pPlayerActor->UseAndCheckAvailProjectileCount();

	if( false == bValidProjectile )
		return NULL;

	// 현재 타이밍에 맞는 발사체인지 체크
	// 발사체에서 발사체를 쏘는 특수한 경우는 제외.
	if( false == bFromParentProjectile )
	{
		bValidProjectile = true;

		//낙인을 위한 발사체인 경우 발사체 시그널 수 보다 많은 발사체가 발사된다.
		//낙인용 발사체가 아닌경우만 발사체 체크 하도록 수정함.
		if (pShooterStruct->nTargetStateIndex == 0)
			bValidProjectile = pPlayerActor->CheckProjectileSignalFrameTerm();
		
		if( false == bValidProjectile )
		{
#ifndef _FINAL_BUILD
			OutputDebug( "CDnPlayerActor::CheckProjectileSignalFrameTerm(): 현재 타이밍에 올 수 없는 발사체 요청 시그널. 핵으로 판단.\n" );
#endif // #ifndef _FINAL_BUILD
			return NULL;
		}
	}

	CDnProjectile *pProjectile = new CDnProjectile( pPlayerActor->GetRoom(), hShooter );
	pProjectile->SetUniqueID( dwUniqueID );
	pProjectile->SetPierce( bPierce );
	pProjectile->SetMaxHitCount( nMaxHitCount );
	pProjectile->SetTargetPartsIndex(nTargetPartsIndex, nTargetBoneIndex);

	if( bActorAttachWeapon ) 
	{
		CDnWeapon *pWeapon = pPlayerActor->GetWeapon(1);

		if( pWeapon )
		{
			*(CDnWeapon*)pProjectile = *pWeapon;
		}
	}
	else 
	{
		if( pProjectileStruct && pProjectileStruct->nProjectileIndex != -1 )
		{
			DnWeaponHandle hLocalParentProjectileWeapon = CDnWeapon::GetSmartPtr( (CMultiRoom*)g_pGameServerManager->GetRootRoom(), 
				pProjectileStruct->nProjectileIndex );
			if( hLocalParentProjectileWeapon ) *(CDnWeapon*)pProjectile = *hLocalParentProjectileWeapon.GetPointer();
		}
		else
		{
			// 짱깨에서 일반 공격으로 무기 ID 바꿔서 보내는 경우 체크. 
			// 발사체에서 발사체 쏘는 특수한 경우는 제외.
			if( false == bFromParentProjectile )
			{
				if( false == pPlayerActor->CheckAndEraseWeaponIDUsingProjectileSignal( nWeaponTableID ) )
				{
					// TODO: 핵이다! 접속 끊어버렸으면 좋겠는데..
#ifndef _FINAL_BUILD
					OutputDebug( "CDnProjectile::CreatePlayerProjectileFromClientPacket(): 현재 액션에 있는 발사체 시그널에서 사용하지 않는 무기 테이블 인덱스. 핵입니다.\n" );
#endif // #ifndef _FINAL_BUILD
					return NULL;
				}
			}
			pProjectile->CDnWeapon::Initialize( nWeaponTableID, 0 );
			pProjectile->CDnWeapon::CreateObject();
		}
	}

#ifdef PRE_MOD_PROJECTILE_HACK
	nWeaponLength = pProjectile->GetWeaponLength();
	if( pShooterStruct->bIncludeMainWeaponLength && hShooter->GetWeapon(0) ) nWeaponLength += hShooter->GetWeapon(0)->GetWeaponLength();
	if( hShooter->IsProcessSkill() ) nWeaponLength += hShooter->GetProcessSkill()->GetIncreaseRange();
	if( hShooter->IsEnabledToggleSkill() ) nWeaponLength += hShooter->GetEnabledToggleSkill()->GetIncreaseRange();
#endif

	pProjectile->SetWeaponLength( nWeaponLength );
	pProjectile->SetWeaponType( (CDnWeapon::WeaponTypeEnum)( pProjectile->GetWeaponType() | CDnWeapon::Projectile ) );

	pProjectile->SetSpeed( fSpeed );
	switch( OrbitType ) 
	{
		case CDnProjectile::Acceleration:
			pProjectile->SetResistance( fResistance );
			break;
	}

	switch( TargetType )
	{
		case CDnProjectile::TargetPosition:
		case CDnProjectile::Target:
		case CDnProjectile::DestPosition:
			pProjectile->SetTargetPosition( vTargetPos );
			pProjectile->SetTargetActor( CDnActor::FindActorFromUniqueID( pPlayerActor->GetRoom(), dwTargetUniqueID ), false );

			//////////////////////////////////////////////////////////////////////////
			//목표 지점으로 날아 가는 발사체인 경우 현재 발사체 위치에서 목표 지점으로의 방향 벡터 변경 해야 한다.
			//일단 낙인용 발사체에서만 동작 하도록 수정한다.
			if (pShooterStruct->nTargetStateIndex != 0)
			{
				Cross.m_vZAxis = vTargetPos - Cross.GetPosition();
				EtVec3Normalize( &Cross.m_vZAxis, &Cross.m_vZAxis );
			}
			//////////////////////////////////////////////////////////////////////////
			break;
	}

	pProjectile->Initialize( Cross, OrbitType, DestroyType, TargetType );
	pProjectile->SetValidTime( nValidTime );
	pProjectile->SetVelocityType( (CDnProjectile::VelocityTypeEnum)VelocityType );
	pProjectile->SetProjectileOrbitRotateZ( fProjectileOrbitRotateZ );
	pProjectile->SetTraceHitTarget( bTraceHitTarget, bTraceHitActorHittable );

	// Note: 클라이언트에서 스킬을 썼을 때만 nActionIndex 가 셋팅되어 패킷으로 날라옴.
	// pProjectileStruct 가 NULL 인 경우엔 클라와 서버의 nSignalIndex 가 일치하지 않아서임.
	// 즉, 서버/클라간의 action 파일 리소스가 다르다는 의미.
	if( pProjectileStruct && -1 != nSignalIndex && -1 != nActionIndex )
		pPlayerActor->OnProjectile( pProjectile, pProjectileStruct, Cross, nSignalIndex );

	if( bFromParentProjectile )
	{
		pProjectile->SetShooterStateSnapshot( pParentProjectile->GetShooterStateSnapshot() );
		pProjectile->SetParentSkill( hParentSkill );

#ifdef PRE_ADD_PROJECTILE_SE_INFO
		pProjectile->SetShooterStateBlow( pParentProjectile->GetShooterStateBlow() );
#endif

		//발사체에서 발사체를 쏘는 경우 부모 발사체에 있던 상태효과들을 등록한다..
		pProjectile->ApplyParentProjectile(pParentProjectile);

		//발사체 -> 발사체 발사 할때 스킬에 의한 발사체인지 부모 발사체에서 정보를 받아서 설정 해줘야 함.
		pProjectile->FromSkill(pParentProjectile->IsFromSkill());

#if defined(PRE_FIX_65287)
		pProjectile->SetShooterFinalDamageRate(pParentProjectile->GetShooterFinalDamageRate());
#endif // PRE_FIX_65287
	}
	else
	{
		bool bApplyStateBlowInfo = pProjectileStruct->bApplyStateBlowInfo == TRUE;

#if defined( PRE_FIX_PROJECTILE_PREFIX_APPLY_POINT )				
		if( hShooter && hShooter->IsPlayerActor() )
		{
			if( hShooter->IsAppliedThisStateBlow(STATE_BLOW::BLOW_183) == false )
			{
				hShooter->ProcessPrefixOffenceSkill_New();
				if( hShooter->IsApplyPrefixOffenceSkill() )
					bApplyStateBlowInfo = true;
			}
		}
#endif

		// 스킬이던 일반 평타이던 플레이어는 능력치 셋팅 필요함. (#17829)
		boost::shared_ptr<CDnState> pActorStateSnapshot( new CDnState ); 		
		*pActorStateSnapshot = *static_cast<CDnState*>( pPlayerActor );
		pProjectile->SetShooterStateSnapshot( pActorStateSnapshot );

#ifdef PRE_ADD_PROJECTILE_SE_INFO
		if( bApplyStateBlowInfo == true )
		{
			boost::shared_ptr<CDnStateBlow> pActorStateBlow = boost::shared_ptr<CDnStateBlow>(new CDnStateBlow(pPlayerActor->GetMySmartPtr()));
			pActorStateBlow->MakeCloneStateBlowList( pPlayerActor->GetStateBlow()->GetStateBlowList() );
			pProjectile->SetShooterStateBlow( pActorStateBlow );
		}
#endif

#if defined(PRE_FIX_65287)

		float fFinalDamageRate = 0.0f;
		if (pPlayerActor && pPlayerActor->IsAppliedThisStateBlow(STATE_BLOW::BLOW_050))
		{
			DNVector(DnBlowHandle) vlhBlows;
			pPlayerActor->GatherAppliedStateBlowByBlowIndex( STATE_BLOW::BLOW_050, vlhBlows );
			int iNumBlow = (int)vlhBlows.size();
			for( int i = 0; i < iNumBlow; ++i )
			{
				fFinalDamageRate += vlhBlows[i]->GetFloatValue();
			}
		}

		pProjectile->SetShooterFinalDamageRate(fFinalDamageRate);
#endif // PRE_FIX_65287

		// #30571 현재 하고 있는 스킬 액션에서 발사된 발사체에서만 호출해준다.
		// else 위 구문의 발사체에서 발사체를 쏘는 경우엔 추후에 발사체에서 발사체가
		// 발사될 때 클라에서 패킷이 또 오므로 여기서 구분해주어서 호출해줘야 한다.
		hShooter->OnSkillProjectile( pProjectile );
	}

#if defined(PRE_FIX_52329)
	pProjectile->SetIgnoreHitType(nIgnoreHitType);
#endif // PRE_FIX_52329

#if defined(PRE_ADD_55295)
	pProjectile->SetHitActionVectorInit(bHitActionVectorInit);
#endif // PRE_FIX_52329

	pProjectile->PostInitialize();


	return pProjectile;
}


// 생성된 발사체 객체에서 패킷을 만든다.
// 클라이언트와 완전히 동일한 루틴이어야 함. (이 파일 자체는 클라/서버 분리 되어있으므로 주의)
CPacketCompressStream* CDnProjectile::GetPacketStream( void )
{
	if( !m_hShooter )
		return NULL;

	// 처음 요청 들어온 것이라 객체라 없다면 패킷 스트림 만들어 줌.
	// 한번 만든 이후엔 발사체 객체의 내용이 바뀌어도 반영되지 않으므로 주의.
	// 현재 Create 된 후 생성된 발사체의 내용을 바꾸는 경우는 없다.
	if( NULL == m_pPacketStream )
	{
		m_pPacketBuffer = new char[ PROJECTILE_PACKET_BUFFER_SIZE ];
		m_pPacketStream = new CPacketCompressStream( m_pPacketBuffer, PROJECTILE_PACKET_BUFFER_SIZE*sizeof(char) );

		bool bActorAttachWeapon = false;
		int VelocityType = 0;
		int nValidTime = 0;
		float fValue;
		if( m_hShooter->GetWeapon(1) && 
			this->GetClassID() == m_hShooter->GetWeapon(1)->GetClassID() ) 
			bActorAttachWeapon = true;

		DWORD dwGap = 0;

		m_pPacketStream->Write( &dwGap, sizeof(DWORD) );		// 서버에서 보내는 것이므로 처리할 필요 없음.
		m_pPacketStream->Write( &bActorAttachWeapon, sizeof(bool) );
		int nValue = 0;
		DWORD dwValue = this->GetUniqueID();
		m_pPacketStream->Write( &dwValue, sizeof(DWORD) );

		if( bActorAttachWeapon == false ) {
			nValue = this->GetClassID();
			m_pPacketStream->Write( &nValue, sizeof(int) );
		}

		m_pPacketStream->Write( &this->GetMatEx()->m_vPosition, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
		m_pPacketStream->Write( &this->GetMatEx()->m_vXAxis, sizeof(EtVector3), CPacketCompressStream::VECTOR3_SHORT );
		m_pPacketStream->Write( &this->GetMatEx()->m_vYAxis, sizeof(EtVector3), CPacketCompressStream::VECTOR3_SHORT );
		m_pPacketStream->Write( &this->GetMatEx()->m_vZAxis, sizeof(EtVector3), CPacketCompressStream::VECTOR3_SHORT );

#ifdef PRE_MOD_PROJECTILE_HACK
		DWORD dwShooterUniqueID = m_hShooter->GetUniqueID();
		m_pPacketStream->Write( &m_cShooterType, sizeof(char) );
		m_pPacketStream->Write( &dwShooterUniqueID, sizeof(DWORD) );
		switch( m_cShooterType ) {
			case 0: 
				break;
			case 1:
				{
					bool bSendSerialID = ( m_nShooterSerialID == -1 ) ? false : true;
					m_pPacketStream->Write( &bSendSerialID, sizeof(bool) );
					if( bSendSerialID )
						m_pPacketStream->Write( &m_nShooterSerialID, sizeof(INT64) );
					else m_pPacketStream->Write( &m_dwShooterUniqueID, sizeof(DWORD) );
				}
				break;
		}
		m_pPacketStream->Write( &m_nShooterActionIndex, sizeof(int), CPacketCompressStream::INTEGER_SHORT );
		m_pPacketStream->Write( &m_nShooterSignalIndex, sizeof(int), CPacketCompressStream::INTEGER_CHAR );
#else
		nValue = (int)this->GetOrbitType();
		m_pPacketStream->Write( &nValue, sizeof(int), CPacketCompressStream::INTEGER_CHAR );
		nValue = (int)this->GetDestroyOrbitType();
		m_pPacketStream->Write( &nValue, sizeof(int), CPacketCompressStream::INTEGER_CHAR );
		nValue = (int)this->GetTargetType();
		m_pPacketStream->Write( &nValue, sizeof(int), CPacketCompressStream::INTEGER_CHAR );

		nValidTime = this->GetValidTime();
		m_pPacketStream->Write( &nValidTime, sizeof(int), CPacketCompressStream::INTEGER_SHORT );

		nValue = this->GetWeaponLength();
		m_pPacketStream->Write( &nValue, sizeof(int) );

		fValue = this->GetProjectileOrbitRotateZ();
		m_pPacketStream->Write( &fValue, sizeof(float) );
#endif

		m_pPacketStream->Write(&m_nTargetPartsIndex, sizeof(int));
		m_pPacketStream->Write(&m_nTargetPartsBoneIndex, sizeof(int));

		switch( this->GetTargetType() ) {
		case CDnProjectile::CrossHair:
		case CDnProjectile::Direction:
			break;
		case CDnProjectile::TargetPosition:
		case CDnProjectile::Target:
		case CDnProjectile::DestPosition:
		case CDnProjectile::Shooter:
			{
				m_pPacketStream->Write( this->GetTargetPosition(), sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
				DWORD dwUniqueID = ( this->GetTargetActor() ) ? this->GetTargetActor()->GetUniqueID() : -1;
				m_pPacketStream->Write( &dwUniqueID, sizeof(DWORD) );
			}
			break;
		}

		fValue = this->GetSpeed();
		m_pPacketStream->Write( &fValue, sizeof(float) );

#ifdef PRE_MOD_PROJECTILE_HACK
		int nActionIndex = -1;
		if( m_hShooter->GetProcessSkill() ) 
			nActionIndex = m_hShooter->GetCurrentActionIndex();

		m_pPacketStream->Write( &nActionIndex, sizeof(int), CPacketCompressStream::INTEGER_SHORT );
#else
		bool bPierce = this->GetPierce();
		m_pPacketStream->Write( &bPierce, sizeof(bool) );

		if( bPierce )
		{
			int nMaxHitCount = this->GetMaxHitCount();
			m_pPacketStream->Write( &nMaxHitCount, sizeof(int) );
		}

		bool bTraceHitTarget = this->IsTraceHitTarget();
		bool bTraceHitActorHittable = this->IsTraceHitActorHittable();
		m_pPacketStream->Write( &bTraceHitTarget, sizeof(bool) );
		m_pPacketStream->Write( &bTraceHitActorHittable, sizeof(bool) );

		VelocityType = (int)this->GetVelocityType();

		switch( this->GetOrbitType() ) {
		case CDnProjectile::Linear:
		case CDnProjectile::TerrainLinear:
			break;

		case CDnProjectile::Projectile:
			m_pPacketStream->Write( &VelocityType, sizeof(int), CPacketCompressStream::INTEGER_CHAR );
			break;

		case CDnProjectile::Homing:
		case CDnProjectile::TerrainHoming:
			m_pPacketStream->Write( &VelocityType, sizeof(int), CPacketCompressStream::INTEGER_CHAR );
			break;

		case CDnProjectile::Acceleration:
			fValue = this->GetResistance();
			m_pPacketStream->Write( &fValue, sizeof(float), CPacketCompressStream::FLOAT_SHORT, 1.f );
			break;
		}

		// 아래 이하는 몬스터에서 사용하지 않는다.

		int nActionIndex = -1;
		if( m_hShooter->GetProcessSkill() ) 
			nActionIndex = m_hShooter->GetCurrentActionIndex();

		m_pPacketStream->Write( &nActionIndex, sizeof(int) );

		// Note : 체인 공격 상태효과를 위해 시그널 struct 를 찾을 수 있는 index 가 필요함.
		m_pPacketStream->Write( &m_iSignalArrayIndex, sizeof(int) );

		// 보우 마스터의 레볼루셔너리 발리스타 같은 발사체어서 발사체 쏘는 스킬에서 최초로 나간 발사체에서
		// 능력치를 이전하기 위한 정보... 
		// 클라에서 서버로 보내는 것으로 서버에서 클라로 보낼 일은 없다. 
		// 형식을 맞추기 위해 써준다.
		DWORD dwParentID = UINT_MAX;//this->GetParentProjectileID();
		DWORD dwParentShooterUniqueID = m_hShooter->GetUniqueID();
		m_pPacketStream->Write( &dwParentID, sizeof(DWORD) );
		m_pPacketStream->Write( &dwParentShooterUniqueID, sizeof(DWORD) );
#endif

#if defined(PRE_FIX_52329)
		m_pPacketStream->Write( &m_nIgnoreHitType, sizeof(m_nIgnoreHitType));
#endif // PRE_FIX_52329
	}

	return m_pPacketStream;
}


void CDnProjectile::SetShooterType( DnActorHandle hActor, int nActionIndex, int nSignalIndex )
{
	m_cShooterType = 0;
	m_dwShooterUniqueID = hActor->GetUniqueID();
	m_nShooterActionIndex = nActionIndex;
	m_nShooterSignalIndex = nSignalIndex;
}

void CDnProjectile::SetShooterType( DnWeaponHandle hWeapon, int nActionIndex, int nSignalIndex )
{
	m_cShooterType = 1;

	m_dwShooterUniqueID = -1;
	m_nShooterSerialID = -1;
	if( hWeapon->GetSerialID() == -1 )
		m_dwShooterUniqueID = hWeapon->GetUniqueID();
	else m_nShooterSerialID = hWeapon->GetSerialID();

	m_nShooterActionIndex = nActionIndex;
	m_nShooterSignalIndex = nSignalIndex;
}


void CDnProjectile::ApplyParentProjectile(CDnProjectile* pParentProjectile)
{
	if (pParentProjectile == NULL)
		return;

	const std::vector<CDnSkill::StateEffectStruct>& stateEffectList = pParentProjectile->GetStateEffectList();
	if (stateEffectList.empty())
		return;

	int nStateEffectCount = (int)stateEffectList.size();
	for (int i = 0; i < nStateEffectCount; ++i)
	{
		CDnSkill::StateEffectStruct& stateEffect = const_cast<CDnSkill::StateEffectStruct&>(stateEffectList.at(i));
		if( stateEffect.ApplyType == CDnSkill::ApplySelf ) continue;

		this->AddStateEffect( stateEffect );
	}
}

void CDnProjectile::SetTargetActor( DnActorHandle hActor, bool bUpdateTargetPartsIndex/* = true*/ )
{ 
	m_hTargetActor = hActor;

	if (bUpdateTargetPartsIndex)
		UpdateTargetPartsIndex();
}

void CDnProjectile::UpdateTargetPartsIndex()
{
	int nSelectPartsIndex = -1;
	int nSelectBoneIndex = -1;

	//파츠 몬스터 일 경우 해당 파츠를 선택 해야 한다..
	if (m_hTargetActor && m_hTargetActor->IsMonsterActor())
	{
		if( m_hTargetActor->IsPartsMonsterActor() ) {
			CDnPartsMonsterActor* pPartsMonsterActor = static_cast<CDnPartsMonsterActor*>(m_hTargetActor.GetPointer());
			int nCount = pPartsMonsterActor->GetPartsSize();

			//선택가능한 Parts Index를 저장 해놓는다..
			struct SelectPartsInfo
			{
				int PartsIndex;
				int BoneCount;
			};

			std::vector<SelectPartsInfo> indexList;

			for (int i = 0; i < nCount; ++i)
			{
				MonsterParts* pMonsterParts = pPartsMonsterActor->GetPartsByIndex(i);
				if (pMonsterParts == NULL)
					continue;

				const MonsterParts::_Info& boneInfo = pMonsterParts->GetPartsInfo();
				if (boneInfo.PartsState != MonsterParts::ePartsState::eNormal)
					continue;

				//가능한 parts 인덱스와 이 Parts의 bone 갯수를 담아 놓는다.
				SelectPartsInfo _tempInfo;
				_tempInfo.PartsIndex = i;
				_tempInfo.BoneCount = (int)boneInfo.vParts.size();

				indexList.push_back(_tempInfo);
			}

			int nListCount = (int)indexList.size();

			if (nListCount > 0)
			{
				//담긴 파츠들 중에서 하나를 선택한다..
				int nIndex = rand() % nListCount;
				SelectPartsInfo &tempPartsInfo = indexList[nIndex];

				nSelectPartsIndex = tempPartsInfo.PartsIndex;
				
				//선택된 파츠중 본 하나를 선택 한다.
				nSelectBoneIndex = rand() % tempPartsInfo.BoneCount;
			}
		}
	}

	SetTargetPartsIndex(nSelectPartsIndex, nSelectBoneIndex);
}

bool CDnProjectile::SetTraceActor( DnActorHandle hActor, bool bValidDamageGoOn )
{
	bool bValidDamageGoOnResult = bValidDamageGoOn;

	// 따라가는 발사체의 경우 damage 를 hit 상태에서 계속 줄 수 있기 때문에 valid damage 를 켜준다. #22666
	// shoot 액션일 때 한번만 셋팅되면 됨.
	if( m_nActionIndex == m_iShootActionIndex && 
		m_bTraceHitTarget && hActor &&
		!m_hTraceActor )
	{
		bValidDamageGoOn = true;
		m_hTraceActor = hActor;

		// 추적 발사체가 되면 hit 액션 끝날 때 까지 시간과 무관하게 사라지지 않는다.
		m_ValidType = (ValidTypeEnum)( m_ValidType & ~Time );
	}

	return bValidDamageGoOnResult;
}

void CDnProjectile::GetChainAttackInfo(const CDnSkill::StateEffectStruct& stateEffectStruct, float& fRange, int& nMaxCount)
{
	switch(stateEffectStruct.nID)
	{
	case STATE_BLOW::BLOW_060:
		{
			std::string str = stateEffectStruct.szValue;
			std::vector<std::string> tokens;
			std::string delimiters = ";";

			//1. 구분
			TokenizeA(str, tokens, delimiters);

			if (tokens.size() != 3)
				fRange = 1000.0f;
			else
			{
				fRange = (float)atoi(tokens[0].c_str());
				nMaxCount = atoi(tokens[1].c_str());
			}
		}
		break;
	case STATE_BLOW::BLOW_208:
		{
			std::string str = stateEffectStruct.szValue;//"최대히트수;범위(cm);비율";
			std::vector<std::string> tokens;
			std::string delimiters = ";";

			//1. 구분
			TokenizeA(str, tokens, delimiters);

			if (tokens.size() != 3)
				fRange = 1000.0f;
			else
			{
				nMaxCount = atoi(tokens[0].c_str());
				fRange = (float)atoi(tokens[1].c_str());
			}
		}
		break;
	}
}

DnActorHandle CDnProjectile::FindNextChainActor(int iRootAttackerTeam, DnActorHandle hActor, DnActorHandle hPrevActor, float fRange)
{
	DNVector(DnActorHandle) vlActorsInRange;
	CDnActor::ScanActor( hActor->GetRoom(), *hActor->GetPosition(), fRange, vlActorsInRange );

	float fShortestDistanceSQ = FLT_MAX;
	DnActorHandle hActorToAttack;
	DWORD dwNumActors = (DWORD)vlActorsInRange.size();
	for( DWORD dwActor = 0; dwActor < dwNumActors; ++dwActor )
	{
		DnActorHandle hTargetActor = vlActorsInRange.at( dwActor );
		if (!hTargetActor)
			continue;

		if( false == (hTargetActor->IsShow() && hTargetActor->IsProcess()) )
			continue;

		//죽은 녀석은 스킵...
		if (hTargetActor->IsDie())
			continue;

		// 직전에 나에게 상태효과 넘겨줬던 액터한테는 다시 주지 않는다.
		if( hPrevActor != hTargetActor )
		{
			// #30643 나 이외엔 다 적이다~~~
			if( iRootAttackerTeam != hTargetActor->GetTeam() &&
				hTargetActor != hActor )
			{
				EtVector3 vDistance = (*hActor->GetPosition()) - (*hTargetActor->GetPosition());

				float fLengthSQ = EtVec3LengthSq( &vDistance );
				if( fLengthSQ < fShortestDistanceSQ )
				{
					fShortestDistanceSQ = fLengthSQ;
					hActorToAttack = hTargetActor;
				}
			}
		}
	}

	return hActorToAttack;
}

void CDnProjectile::CreateChainAttackProjectile(DnActorHandle hRootAttacker, DnActorHandle hActor, DnActorHandle hActorToAttack, 
												ProjectileStruct* pProjectileSignalInfo, CDnSkill::SkillInfo& parentSkillInfo)
{
	// 프로젝타일 발사 관련 기타 설정들.
	DnSkillHandle hSkill = hRootAttacker->FindSkill( parentSkillInfo.iSkillID );

	// 호밍으로 타겟을 설정토록 정해준다.
	pProjectileSignalInfo->nOrbitType = CDnProjectile::Homing;
	pProjectileSignalInfo->nTargetType = CDnProjectile::Target;
	pProjectileSignalInfo->VelocityType = CDnProjectile::Accell;
	pProjectileSignalInfo->fSpeed = 1000.0f;//CHAINATTACK_PROJECTILE_SPEED;
	pProjectileSignalInfo->nValidTime = 5000;

	MatrixEx Cross = *hActor->GetMatEx();
	Cross.m_vPosition.y += hActor->GetHeight() / 2.0f;

	CDnProjectile *pProjectile = new CDnProjectile( GetRoom(), hRootAttacker );

	pProjectile->SetPierce( pProjectileSignalInfo->bPierce == TRUE ? true : false );
	pProjectile->SetMaxHitCount( pProjectileSignalInfo->nMaxHitCount );

	if( pProjectileSignalInfo->nWeaponTableID > 0 ) 
	{
		if( pProjectileSignalInfo->nProjectileIndex != -1 )
		{
			DnWeaponHandle hWeapon = CDnWeapon::GetSmartPtr( (CMultiRoom*)g_pGameServerManager->GetRootRoom(), pProjectileSignalInfo->nProjectileIndex );
			if( hWeapon ) *(CDnWeapon*)pProjectile = *hWeapon.GetPointer();
		}
	}

	int nLength = pProjectile->GetWeaponLength();
	if( pProjectileSignalInfo->bIncludeMainWeaponLength ) 
	{
		DnWeaponHandle hWeapon = hActor->GetWeapon(0);
		if ( hWeapon )
			nLength += hWeapon->GetWeaponLength();
	}
	pProjectile->SetWeaponLength( nLength );

	pProjectile->SetWeaponType( (CDnWeapon::WeaponTypeEnum)( pProjectile->GetWeaponType() | CDnWeapon::Projectile ) );

	pProjectile->SetSpeed( pProjectileSignalInfo->fSpeed );

	pProjectile->SetTargetPosition( *hActorToAttack->GetPosition() );
	pProjectile->SetTargetActor( hActorToAttack );

	pProjectile->Initialize( Cross, static_cast<CDnProjectile::OrbitTypeEnum>(pProjectileSignalInfo->nOrbitType), 
		static_cast<CDnProjectile::DestroyOrbitTypeEnum>(pProjectileSignalInfo->nDestroyOrbitType), 
		static_cast<CDnProjectile::TargetTypeEnum>(pProjectileSignalInfo->nTargetType) );
	pProjectile->SetValidTime( pProjectileSignalInfo->nValidTime );
	pProjectile->SetVelocityType( static_cast<CDnProjectile::VelocityTypeEnum>(pProjectileSignalInfo->VelocityType) );

	pProjectile->SetParentSkill( hSkill );

	parentSkillInfo.hPrevAttacker = hActor;
	parentSkillInfo.iLeaveCount -= 1; // 설정되어있는 카운트 하나씩 줄인다.
	pProjectile->SetParentSkillInfo( parentSkillInfo );
	pProjectile->FromSkill( true );

	CDnState rootAttackerState;
	int iNumSE = hSkill->GetStateEffectCount();
	for( int i = 0; i < iNumSE; ++i )
	{
		const CDnSkill::StateEffectStruct* pStateEffect = hSkill->GetStateEffectFromIndex( i );
		if( pStateEffect->ApplyType == CDnSkill::ApplySelf ) 
			continue;

		CDnSkill::StateEffectStruct SE = *pStateEffect;

		pProjectile->AddStateEffect( SE );
	}

	//#45331
	//발사체가 생성될 시점에 저장되어 있던 액터의 상태값을 그대로 전달 해준다.
	pProjectile->SetShooterStateSnapshot( m_pShooterState );

#if defined(PRE_FIX_65287)
	pProjectile->SetShooterFinalDamageRate(m_fShooterFinalDamageRate);
#endif // PRE_FIX_65287

	pProjectile->PostInitialize();
}

#if defined(PRE_ADD_55295)
void CDnProjectile::ChangeProjectileRotation()
{
	//방향은 그대로 두고, Y축만 다시 설정을 위해, Y축은 초기화 해서
	//X, Z방향 벡터 다시 계산
	m_Cross.m_vYAxis = EtVector3(0.0f, 1.0f, 0.0f);

	m_Cross.MakeUpCartesianByYAxis();
}
#endif // PRE_ADD_55295


#if defined(PRE_FIX_59238)
void CDnProjectile::AddHittedActor(DnActorHandle hHittedActor)
{
	if (hHittedActor)
		m_HittedActorList.insert(std::make_pair(hHittedActor->GetUniqueID(), hHittedActor));
}

bool CDnProjectile::IsHittable(DnActorHandle hActor)
{
	//먼저 액터 유효성 확인..
	if (!hActor)
		return false;

	bool isSummonMonster = false;
	bool isOwnerActor = false;

	//꼭두각시 상태효과가 있는 주인액터 인지..
	if (hActor->IsAppliedThisStateBlow(STATE_BLOW::BLOW_247))
	{
		isOwnerActor = true;
	}
	//소환 몬스터인지...
	else if (hActor->IsMonsterActor())
	{
		CDnMonsterActor* pMonsterActor = static_cast<CDnMonsterActor*>(hActor.GetPointer());
		if (pMonsterActor && 
			pMonsterActor->IsSummonedMonster() &&
			pMonsterActor->IsPuppetSummonMonster())
			isSummonMonster = true;		
	}

	//소환 몬스터도 아니고, 꼭두각시 상태효과를 가진 액터도 아니라면 히트 가능..
	//소환 몬스터이고, 꼭두각시 상태효과를 가진 액터라면 뭔가 문제 있음..(그냥 히트 가능 하도록...)
	if ((isSummonMonster == false && isOwnerActor == false) ||
		(isSummonMonster == true && isOwnerActor == true))
		return true;


	//꼭두각시 상태효과를 가진 액터라면 이미 히트된 액터 리스트에서 자신이 소환한 소환 몬스터 액터가
	//포함되어 있다면 자신은 히트 되지 않아야 한다.
	if (isOwnerActor == true)
	{
		//자신의 소환 몬스터 리스트
		const std::list<DnMonsterActorHandle> & listSummonMonster = hActor->GetSummonedMonsterList();
		std::list<DnMonsterActorHandle>::const_iterator iter = listSummonMonster.begin();
		for( iter; iter != listSummonMonster.end(); ++iter )
		{
			DnMonsterActorHandle hMonster = (*iter);
			if (!hMonster)
				continue;

			//소환 몬스터가 아니고, PuppetSummon몬스터가 아니면 스킵
			if (hMonster->IsSummonedMonster() == false || hMonster->IsPuppetSummonMonster() == false)
				continue;

			//소환한 몬스터가 이미 히트 리스트에 있다면 자신은 히트 되지 않아야 한다.
			std::map<DWORD, DnActorHandle>::iterator iter = m_HittedActorList.find(hMonster->GetUniqueID());
			if (iter != m_HittedActorList.end())
				return false;
		}

		//자신의 소환 그룹 몬스터 리스트
		const std::map<int, std::list<DnMonsterActorHandle> >& groupSummonMonster = hActor->GetGroupingSummonedMonsterList();
		std::map<int, std::list<DnMonsterActorHandle> >::const_iterator mapIter;
		for (mapIter = groupSummonMonster.begin(); mapIter != groupSummonMonster.end(); ++mapIter)
		{
			const std::list<DnMonsterActorHandle> & listSummonMonster = mapIter->second;
			std::list<DnMonsterActorHandle>::const_iterator iter = listSummonMonster.begin();
			for (; iter != listSummonMonster.end(); ++iter)
			{
				DnMonsterActorHandle hMonster = (*iter);
				if (!hMonster)
					continue;

				//소환 몬스터가 아니고, PuppetSummon몬스터가 아니면 스킵
				if (hMonster->IsSummonedMonster() == false || hMonster->IsPuppetSummonMonster() == false)
					continue;

				//소환한 몬스터가 이미 히트 리스트에 있다면 자신은 히트 되지 않아야 한다.
				std::map<DWORD, DnActorHandle>::iterator iter = m_HittedActorList.find(hMonster->GetUniqueID());
				if (iter != m_HittedActorList.end())
					return false;
			}
		}

		return true;
	}
	//소환 몬스터라면, 이미 히트된 액터 리스트에서 자신의 주인 액터가 있다면 히트 되지 않아야 한다.
	else if (isSummonMonster == true)
	{
		DnActorHandle hOwnerActor;
		CDnMonsterActor* pMonsterActor = static_cast<CDnMonsterActor*>(hActor.GetPointer());
		if (pMonsterActor)
			hOwnerActor = pMonsterActor->GetSummonerPlayerActor();

		//소환 주인 액터가 꼭두각시 상태효과를 가지고 있지 않으면 히트되어도 된다..
		if (!hOwnerActor || hOwnerActor->IsAppliedThisStateBlow(STATE_BLOW::BLOW_247) == false)
			return true;

		//소환 몬스터의 주인 액터가 이미 히트 리스트에 존재 하면 이 액터는 히트 되면 안되고,
		//주인 액터가 히트 리스트에 없으면 히트 되어야함.
		std::map<DWORD, DnActorHandle>::iterator iter = m_HittedActorList.find(hOwnerActor->GetUniqueID());
		if (iter != m_HittedActorList.end())
			return false;
		else
			return true;
	}

	return true;
}
#endif // PRE_FIX_59238

#if defined(PRE_FIX_59336)
void CDnProjectile::ApplyComboLimitStateEffect( DnActorHandle hActor )
{
	for( DWORD i= 0 ; i < m_ComboLimitStateEffectList.size(); i++ ) 
	{
		const CDnSkill::StateEffectStruct& SE = m_ComboLimitStateEffectList.at(i);

		if (m_HitStruct.szSkipStateBlows && CDnSkill::IsSkipStateBlow(m_HitStruct.szSkipStateBlows, (STATE_BLOW::emBLOW_INDEX)SE.nID))
			continue;

		// 아군까지 힐 시켜주는 스킬의 경우엔 Self 힐과 Target 힐 두 개의 상태효과가 선언되어있따.
		// Self 상태효과는 자신에게 이미 적용되었고 여긴 Hit 시그널 판정되는 곳이기 떄문에 Target 만 적용된다.
		switch( SE.ApplyType )
		{
		case CDnSkill::ApplySelf:
			continue;
			break;

		case CDnSkill::ApplyTarget:
			break;

		case CDnSkill::ApplyEnemy:
			if( m_hShooter->GetTeam() == hActor->GetTeam() )
				continue;
			break;

		case CDnSkill::ApplyFriend:
			if( m_hShooter->GetTeam() != hActor->GetTeam() )
				continue;
			break;
		}

		m_ParentSkillInfo.iProjectileShootActionIndex = m_iShooterShootActionIndex;
		m_ParentSkillInfo.iProjectileSignalArrayIndex = m_iSignalArrayIndex;

		//같은 스킬의 같은 시작 시간이 있으면 추가 안되도록..
		bool isExistSameSkillTimeBlow = false;
		DNVector(DnBlowHandle) vlAppliedBlows;
		hActor->GatherAppliedStateBlowByBlowIndex( (STATE_BLOW::emBLOW_INDEX)m_ComboLimitStateEffectList[i].nID, vlAppliedBlows );
		int iNumAppliedBlow = (int)vlAppliedBlows.size();
		for( int iAppliedBlow = 0; iAppliedBlow < iNumAppliedBlow; ++iAppliedBlow )
		{
			DnBlowHandle hBlow = vlAppliedBlows.at( iAppliedBlow );
			if (!hBlow)
				continue;

			CDnSkill::SkillInfo* pExistSkillInfo = const_cast<CDnSkill::SkillInfo*>(hBlow->GetParentSkillInfo());

			//////////////////////////////////////////////////////////////////////////
			// #56880
			// 스킬 시작 시간이 같은지도 확인 해야 한다..
			LOCAL_TIME parentSkillStartTime = 0;
			LOCAL_TIME nowBlowSkillStartTime = 0;

			parentSkillStartTime = GetSkillStartTime();
			CDnComboDamageLimitBlow* pNowBlow = static_cast<CDnComboDamageLimitBlow*>(hBlow.GetPointer());
			if (pNowBlow)
				nowBlowSkillStartTime = pNowBlow->GetSkillStartTime();
			//////////////////////////////////////////////////////////////////////////

			if (pExistSkillInfo)
			{
				if (m_ParentSkillInfo.hSkillUser && 
					m_ParentSkillInfo.hSkillUser == pExistSkillInfo->hSkillUser &&	//스킬 사용자가 같고
					m_ParentSkillInfo.iSkillID == pExistSkillInfo->iSkillID &&		//스킬이 같은경우
					nowBlowSkillStartTime == parentSkillStartTime					//스킬 시작 시간이 같은 경우
					)
				{
					isExistSameSkillTimeBlow = true;
					break;
				}
			}

		}

		int iID = -1;
		bool bCheckCanBegin = true;
		if (isExistSameSkillTimeBlow == false)
		{
			//발사체에서 상태효과 추가할때 발사체 스킬 시작 시간을 담아서 전달한다..
			m_ParentSkillInfo.projectileSkillStartTime = GetSkillStartTime();

#ifdef PRE_FIX_REMOVE_STATE_EFFECT_PACKET // 서버에서만 사용하는 형태이기때문에 패킷쏘지않습니다.
			iID = hActor->CDnActor::CmdAddStateEffect( &m_ParentSkillInfo, (STATE_BLOW::emBLOW_INDEX)m_ComboLimitStateEffectList[i].nID, 
				m_ComboLimitStateEffectList[i].nDurationTime, m_ComboLimitStateEffectList[i].szValue.c_str(), false, bCheckCanBegin );
#else
			iID = hActor->CmdAddStateEffect( &m_ParentSkillInfo, (STATE_BLOW::emBLOW_INDEX)m_ComboLimitStateEffectList[i].nID, 
				m_ComboLimitStateEffectList[i].nDurationTime, m_ComboLimitStateEffectList[i].szValue.c_str(), false, bCheckCanBegin );
#endif

			//발사체 시간 리셋..
			m_ParentSkillInfo.projectileSkillStartTime = 0;
		}

		// 추가한 상태효과 중에 chain attack 상태효과가 있다면 발사체 관련 정보들 셋팅해줘야 한다.
		// 핑퐁밤 상태효과도 마찬가지로 추가해줘야 함.
		if( iID != -1 )
		{
			DnBlowHandle hBlow = hActor->GetStateBlowFromID( iID );
			//#56880 발사체로 상태효과 추가 될때 242상태효과 인경우 발사체 스킬의 시작 시간을 설정한다..??
			if (m_ComboLimitStateEffectList[i].nID == STATE_BLOW::BLOW_242)
			{
				LOCAL_TIME skillStartTime = GetSkillStartTime();

				DnBlowHandle hBlow = hActor->GetStateBlowFromID( iID );
				if( hBlow )
				{
					CDnComboDamageLimitBlow* pComboLimitBlow = static_cast<CDnComboDamageLimitBlow*>( hBlow.GetPointer() );
					if (pComboLimitBlow)
						pComboLimitBlow->SetSkillStartTime(skillStartTime);
				}
			}
		}
	}
}
#endif // PRE_FIX_59336


void CDnProjectile::ApplySkillStateEffect(DnActorHandle hActor)
{
	if (!hActor)
		return;

	// 대상이 얼음감옥 상태일때는 상태효과 적용 무시
	if (hActor->IsAppliedThisStateBlow(STATE_BLOW::BLOW_149))
		return;

	// 스킬 대상 설정이 아군인가 타겟인가에 따라 상태효과 적용을 구분한다.
	switch( m_ParentSkillInfo.eTargetType )
	{
	case CDnSkill::Enemy:
	case CDnSkill::Self:
		if( m_hShooter->GetTeam() == hActor->GetTeam() )
			return;
		break;

	case CDnSkill::Friend:
	case CDnSkill::Party:
		if( m_hShooter->GetTeam() != hActor->GetTeam() )
			return;
		break;

		// 스킬 적용 대상이 아군/적군 전부 다 라면 상태효과 적용 쪽에서 적용 여부를 구분해야 한다.
	case CDnSkill::All:
		break;
	}

	// 상태이상 Add
	// 어떤 스킬로 인해 생성된 Projectile 이라면,
	CDnSkill::CanApply eResult = CDnSkill::CanApply::Apply;
	map<int, bool> mapDuplicateResult;

	//스킬 레벨업에 의해 발사체 스킬 정보가 Invalid될 수 있다.
	//발사체 액터가 있고, 스킬 ID가 설정되어 있는데 스킬이 Invalid하면 스킬 다시 설정한다.
	if (m_hShooter && m_ParentSkillInfo.iSkillID != 0 && !m_hParentSkill)
	{
		DnSkillHandle hSkill = m_hShooter->FindSkill(m_ParentSkillInfo.iSkillID);
		if (hSkill)
			SetParentSkill(hSkill);
	}

	if( m_hParentSkill )
	{
		// 해당 Actor의 지속효과 구분 인덱스를 구분하여 성공한 경우에 상태이상 추가 시킴.
		eResult = CDnSkill::CanApplySkillStateEffect( hActor, m_hParentSkill, mapDuplicateResult, true );
	}


	if( CDnSkill::CanApply::Fail != eResult )
	{
		for( DWORD i= 0 ; i < m_VecStateEffectList.size(); i++ ) 
		{
			const CDnSkill::StateEffectStruct& SE = m_VecStateEffectList.at(i);

			if (m_HitStruct.szSkipStateBlows && CDnSkill::IsSkipStateBlow(m_HitStruct.szSkipStateBlows, (STATE_BLOW::emBLOW_INDEX)SE.nID))
				continue;


			// 아군까지 힐 시켜주는 스킬의 경우엔 Self 힐과 Target 힐 두 개의 상태효과가 선언되어있따.
			// Self 상태효과는 자신에게 이미 적용되었고 여긴 Hit 시그널 판정되는 곳이기 떄문에 Target 만 적용된다.
			switch( SE.ApplyType )
			{
			case CDnSkill::ApplySelf:
				continue;
				break;

			case CDnSkill::ApplyTarget:
				break;

			case CDnSkill::ApplyEnemy:
				if( m_hShooter->GetTeam() == hActor->GetTeam() )
					continue;
				break;

			case CDnSkill::ApplyFriend:
				if( m_hShooter->GetTeam() != hActor->GetTeam() )
					continue;
				break;
			}

			// 같은 스킬 중첩일 경우엔 스킬 효과 중에 확률 체크하는 것들은 이미 CanApplySkillStateEffect 에서 확률체크되고
			// 통과된 상태이다. 따라서 여기선 확률 체크 된건지 확인하고 된거라면 다시 확률 체크 안하도록 함수 호출 해준다.
			bool bAllowAddThisSE = true;
			bool bCheckCanBegin = true;
			if( CDnSkill::ApplyDuplicateSameSkill == eResult )
			{
				map<int, bool>::iterator iter = mapDuplicateResult.find( m_VecStateEffectList[i].nID );
				// 맵에 없는 경우 현재 액터가 상태효과에 걸려있지 않으므로 그냥 정상적으로 상태효과 추가 루틴 실행.
				if( mapDuplicateResult.end() != iter )
				{
					// 같은 스킬의 확률있는 상태효과가 현재 걸려있어서 CanAdd 를 호출해보았으나 실패했음.
					// 이런 경우엔 상태효과 추가하지 않는다.
					if( false == (iter->second) )
						bAllowAddThisSE = false;
					else
						// 이미 CanAdd 를 통과한 상태이므로 CmdAddStateEffect 호출 시 따로 체크하지 않도록 해준다.
						bCheckCanBegin = false;
				}
			}

			m_ParentSkillInfo.iProjectileShootActionIndex = m_iShooterShootActionIndex;
			m_ParentSkillInfo.iProjectileSignalArrayIndex = m_iSignalArrayIndex;

			if( bAllowAddThisSE )
			{
				// #72931 스크리머 저주 쿨타임 공유 처리 -> 나중에 이런식으로 쓰는거 많아지면 일반화해야함
				bool bShareCurseCoolTime = false;
				if( m_VecStateEffectList[i].nID == STATE_BLOW::BLOW_244 )
				{
					DNVector(DnBlowHandle) vlhBlows;
					hActor->GatherAppliedStateBlowByBlowIndex( STATE_BLOW::BLOW_244, vlhBlows );
					if( static_cast<int>( vlhBlows.size() ) > 0 )
					{
						CDnCurseBlow* pDnCurseBlow = static_cast<CDnCurseBlow*>( vlhBlows[0].GetPointer() );
						if( pDnCurseBlow )
						{
							m_VecStateEffectList[i].szValue += ";";
							m_VecStateEffectList[i].szValue += FormatA( "%f", pDnCurseBlow->GetCoolTime() );
							bShareCurseCoolTime = true;
						}
					}
				}

				// 여기서 등록되어 있는 제거 되어야할 상태효과들 없앤다. [2010/12/08 semozz]
				hActor->RemoveResetStateBlow();

				//발사체에서 상태효과 추가할때 발사체 스킬 시작 시간을 담아서 전달한다..
				m_ParentSkillInfo.projectileSkillStartTime = GetSkillStartTime();
				int iID = hActor->CmdAddStateEffect( &m_ParentSkillInfo, (STATE_BLOW::emBLOW_INDEX)m_VecStateEffectList[i].nID, 
					m_VecStateEffectList[i].nDurationTime, m_VecStateEffectList[i].szValue.c_str(), false,
					bCheckCanBegin );

				if( bShareCurseCoolTime )	// #72931 쿨타임 붙였던거 다시 제거
				{
					std::string::size_type delimiterindex = m_VecStateEffectList[i].szValue.rfind( ";" );
					if( delimiterindex != std::string::npos )
					{
						m_VecStateEffectList[i].szValue.erase( delimiterindex, m_VecStateEffectList[i].szValue.length() - delimiterindex );
					}
				}

				//발사체 시간 리셋..
				m_ParentSkillInfo.projectileSkillStartTime = 0;

				// 추가한 상태효과 중에 chain attack 상태효과가 있다면 발사체 관련 정보들 셋팅해줘야 한다.
				// 핑퐁밤 상태효과도 마찬가지로 추가해줘야 함.
				if( iID != -1 )
				{
					DnBlowHandle hBlow = hActor->GetStateBlowFromID( iID );
					if( hBlow )
					{
						if( STATE_BLOW::BLOW_060 == m_VecStateEffectList.at( i ).nID )
						{
							CDnChainAttackBlow* pChainAttackBlow = static_cast<CDnChainAttackBlow*>( hBlow.GetPointer() );
							pChainAttackBlow->SetProjectileSignal( m_pProjectileSignal.get() );
							pChainAttackBlow->SetRootAttackerState( m_pShooterState.get() );
						}
						else
							if( STATE_BLOW::BLOW_208 == m_VecStateEffectList.at( i ).nID )
							{
								CDnPingpongBlow* pPingPongBlow = static_cast<CDnPingpongBlow*>( hBlow.GetPointer() );
								pPingPongBlow->SetProjectileSignal( m_pProjectileSignal.get() );
								pPingPongBlow->SetRootAttackerState( m_pShooterState.get() );
							}
					}

					//#56880 발사체로 상태효과 추가 될때 242상태효과 인경우 발사체 스킬의 시작 시간을 설정한다..??
					if (m_VecStateEffectList[i].nID == STATE_BLOW::BLOW_242)
					{
						LOCAL_TIME skillStartTime = GetSkillStartTime();

						DnBlowHandle hBlow = hActor->GetStateBlowFromID( iID );
						if( hBlow )
						{
							CDnComboDamageLimitBlow* pComboLimitBlow = static_cast<CDnComboDamageLimitBlow*>( hBlow.GetPointer() );
							if (pComboLimitBlow)
								pComboLimitBlow->SetSkillStartTime(skillStartTime);
						}
					}
				}
				else
				{
					//ChainAttack이 아닌 경우는 아래 루틴 스킵함.
					STATE_BLOW::emBLOW_INDEX eBlowIndex = (STATE_BLOW::emBLOW_INDEX)m_VecStateEffectList[i].nID;
					if (eBlowIndex != STATE_BLOW::BLOW_060 && eBlowIndex != STATE_BLOW::BLOW_208)
						continue;

					//0. 상태효과 설정값 확인...
					float fRange = 1000.0f;
					int nMaxCount = -1;
					GetChainAttackInfo(m_VecStateEffectList[i], fRange, nMaxCount);

					//발사체 전이 횟수 설정.
					if (m_ParentSkillInfo.iLeaveCount == -1)
						m_ParentSkillInfo.iLeaveCount = nMaxCount;
					//////////////////////////////////////////////////////////////////////////

					//2. 다음 타겟 캐릭터 선택
					//////////////////////////////////////////////////////////////////////////
					DnActorHandle hRootAttacker = m_ParentSkillInfo.hSkillUser;
					int iRootAttackerTeam = hRootAttacker->GetTeam();

					DnActorHandle hActorToAttack = FindNextChainActor(iRootAttackerTeam, hActor, m_ParentSkillInfo.hPrevAttacker, fRange);

					//발사체 이전 횟수가 없거나, 이전될 액터를 못 찾으면 스킵..
					if (m_ParentSkillInfo.iLeaveCount <= 0 || !hActorToAttack)
						continue;
					//////////////////////////////////////////////////////////////////////////

					//1. 발사체 시그널 정보 설정.
					//////////////////////////////////////////////////////////////////////////

					// 가장 처음 공격한 공격자 액터의 unique id
					DWORD dwRootAttackerActorUniqueID = hRootAttacker->GetUniqueID();
					DWORD dwPrevAttackerActorUniqueID = m_ParentSkillInfo.hPrevAttacker ? m_ParentSkillInfo.hPrevAttacker->GetUniqueID() : UINT_MAX;


					// 프로젝타일 시그널을 클라이언트에서 찾는데 필요한 고유 정보들.
					int iActionIndex = m_ParentSkillInfo.iProjectileShootActionIndex;
					int iProjectileSignalArrayIndex = m_ParentSkillInfo.iProjectileSignalArrayIndex;

					ProjectileStruct projectileSignalInfo;
					bool bFound = false;

					CEtActionSignal *pSignal = hRootAttacker->GetSignal( iActionIndex, iProjectileSignalArrayIndex );
					if( pSignal && pSignal->GetSignalIndex() == STE_Projectile ) {
#ifdef PRE_FIX_MEMOPT_SIGNALH
						CopyShallow_ProjectileStruct(projectileSignalInfo, static_cast<ProjectileStruct*>(pSignal->GetData()));
#else
						projectileSignalInfo = *(static_cast<ProjectileStruct*>(pSignal->GetData()));
#endif

						bFound = true;
					}
					//////////////////////////////////////////////////////////////////////////

					//패킷 클라이언트로 전송..
					hActor->SendChainAttackProjectile(hRootAttacker, dwPrevAttackerActorUniqueID, iActionIndex, iProjectileSignalArrayIndex, hActorToAttack, m_ParentSkillInfo.iSkillID);


					//3. 발사체 정보 설정 하고 생성
					//////////////////////////////////////////////////////////////////////////
					CreateChainAttackProjectile(hRootAttacker, hActor, hActorToAttack, &projectileSignalInfo, m_ParentSkillInfo);
					//////////////////////////////////////////////////////////////////////////

				}
			}
		}
	}

#if defined(PRE_FIX_59336)
	//242번 상태효과 추가..
	ApplyComboLimitStateEffect(hActor);
#endif // PRE_FIX_59336

}