#include "StdAfx.h"
#include "DnGravityOrbit.h"
#include "DnProjectile.h"
#include "DnWorld.h"


const EtVector3 CDnGravityOrbitCalculator::GRAVITY( 0.0f, -980.0f, 0.0f );



CDnGravityOrbit::CDnGravityOrbit( const S_PROJECTILE_PROPERTY* pProjectileInfo ) : m_vStartPos( 0.0f, 0.0f, 0.0f ),
																				   m_vDirectionWhenCreate( 0.0f, 0.0f, 0.0f ),
																				   m_vInitialSpeed( 0.0f, 0.0f, 0.0f ),
																				   m_vTargetPosition( 0.0f, 0.0f, 0.0f ), 
																				   m_fProjectileOrbitRotateZDegree( 0.0f ),
																				   m_bFirstProcess( true ),
																				   m_fElapsedTime( 0.0f )
{
	m_fProjectileOrbitRotateZDegree = pProjectileInfo->fProjectileOrbitRotateZ;
}

CDnGravityOrbit::~CDnGravityOrbit( void )
{

}

void CDnGravityOrbit::Initialize( const MatrixEx& Cross, const MatrixEx& OffsetCross )
{
	m_vStartPos = Cross.m_vPosition;
	
	m_vDirectionWhenCreate = Cross.m_vZAxis;
	EtVec3Normalize( &m_vDirectionWhenCreate, &Cross.m_vZAxis );
}

void CDnGravityOrbit::ProcessOrbit( /*IN OUT*/ MatrixEx& Cross, EtVector3& vPrevPos, LOCAL_TIME LocalTime, float fDelta )
{
	// 한기 2009.7.22
	// 프로젝타일 궤적 관련..  (#4851)
	// Catmullrom 보간을 사용하는 것이 아니라 중력가속도로 적용되도록 바꿈.
	// 2차 곡선으로 처리..
	if( m_bFirstProcess )
	{
		// Note: distance 는 클라이언트에서 프로젝타일 생성되는 시점의 값이고 direction은 서버에서 계산되는 회전값이라면 
		// 약간의 시간차 때문에 방향이 달라질 수 있으므로 프로젝타일 생성되는 시점의 direction 을 사용토록 한다.
		// 서버의 프로젝타일은 클라한테 받아서 생성하므로 서버에서도 이렇게 해야 클라에서 정해준 방향대로 생성하게 되어 틀어지지 않는다.

		// #37334 약하게 쏴질경우 포물선의 최대 높이값보다 목표지점의 높이 값이 더 높아져 버리는 문제가 생길 수도 있다.
		// 그러면 결과 값이 잘못 나오므로 보정값 제거..
		//if( m_vTargetPosition.y - m_vStartPos.y <= 0.0f )
		//	m_vTargetPosition.y += 50.0f;	// 캐릭터 높이 감안해서 좀 더 올려줌.

		EtVector3 vDistance = m_vTargetPosition - m_vStartPos;

		// 방향과 Direction의 xz 부호가 일치해야만 함... 
		bool bXValid = (vDistance.x < 0.0f && m_vDirectionWhenCreate.x < 0.0f || vDistance.x >= 0.0f && m_vDirectionWhenCreate.x >= 0.0f);
		bool bZValid = (vDistance.z < 0.0f && m_vDirectionWhenCreate.z < 0.0f || vDistance.z >= 0.0f && m_vDirectionWhenCreate.z >= 0.0f);
		bool bValidDirection =  bXValid && bZValid;

		if( 0.0f == m_vDirectionWhenCreate.y )
			m_vDirectionWhenCreate.y = 0.1f;

		// 거의 원점에 가까운 direction 이 나올 때 미묘하게 부호가 달라지는 경우가 있으므로 direction 을 따라가도록 처리.
		if( false == bXValid )
		{
			if( m_vDirectionWhenCreate.x < 0.0f && vDistance.x >= 0.0f ||
				m_vDirectionWhenCreate.x >= 0.0f && vDistance.x < 0.0f )
				vDistance.x *= -1.0f;
		}
		else
		if( false == bZValid )
		{
			if( m_vDirectionWhenCreate.z < 0.0f && vDistance.z >= 0.0f ||
				m_vDirectionWhenCreate.z >= 0.0f && vDistance.z < 0.0f )
				vDistance.z *= -1.0f;
		}

		// 방향 벡터의 기울기만큼 뒷항을 대체시키고 0.5*Gt^2 + c 형식의 근의 공식으로 시간값을 구함
		float fSlope = 0.0f;
		float fC = -vDistance.y;

		// 캐릭터의 방향이 영벡터인 경우가 있나 OTL
		_ASSERT( m_vDirectionWhenCreate.x != 0.0f || m_vDirectionWhenCreate.z != 0.0f );
		if( m_vDirectionWhenCreate.x == 0.0f && m_vDirectionWhenCreate.z == 0.0f )
			m_vDirectionWhenCreate.x = m_vDirectionWhenCreate.z = 0.1f;

		bool bUseX = true;
		if( 0.0f != m_vDirectionWhenCreate.x )
		{
			fSlope = m_vDirectionWhenCreate.y / m_vDirectionWhenCreate.x;
			fC += vDistance.x * fSlope;
		}
		else
		{
			fSlope = m_vDirectionWhenCreate.y / m_vDirectionWhenCreate.z;
			fC += vDistance.z * fSlope;
			bUseX = false;
		}

		float fA = 0.5f * CDnGravityOrbitCalculator::GRAVITY.y;
		float fD = -4.0f * fA * fC;

		// 음수가 되는 경우는 어떤 경우인지 확인해보자. -_-
		if( fD < 0.0f )
			fD *= -1.0f;
		//_ASSERT( fD >= 0.0f );
		float fEstimateElapseTime = sqrtf( fD ) / (2.0f * fA);

		// 시간은 무조건 양수
		if( fEstimateElapseTime < 0.0f )
			fEstimateElapseTime *= -1.0f;

		float fPower = 0.0f;
		if( bUseX )
			fPower = vDistance.x / (m_vDirectionWhenCreate.x*fEstimateElapseTime);
		else
			fPower = vDistance.z / (m_vDirectionWhenCreate.z*fEstimateElapseTime);

		m_vInitialSpeed = m_vDirectionWhenCreate * fPower;

		m_bFirstProcess = false;
	}

	// 현재 방향은 현재 속도 벡터를 정규화 시킨 것과 같음.
	EtVector3 vNowDir = m_vInitialSpeed + m_fElapsedTime*CDnGravityOrbitCalculator::GRAVITY; 
	EtVector3 vDelta = 0.5f*CDnGravityOrbitCalculator::GRAVITY*(m_fElapsedTime*m_fElapsedTime) + m_vInitialSpeed*m_fElapsedTime;

	// 궤적 자체가 회전되는 옵션이 추가됨. (#15056)
	// 만약에 목적지를 DestPosition 으로 해놓고 랜덤으로 떨어지게 해놨다면 궤적 회전 값이 적용되지 않고 x, z 그대로 적용됨.
	// 예를 들어, 90도로 눕혀놔도 궤적이 90도 기준으로 흩어지는 것이 아님. 목적지가 x, z 기준 그대로 이므로...
	if( 0.0f != m_fProjectileOrbitRotateZDegree )
	{
		EtMatrix matRotZ;
		EtVector3 vCharViewXZ( m_vDirectionWhenCreate.x, 0.0f, m_vDirectionWhenCreate.z );
		EtVec3Normalize( &vCharViewXZ, &vCharViewXZ );
		EtMatrixRotationAxis( &matRotZ, &vCharViewXZ, D3DXToRadian(m_fProjectileOrbitRotateZDegree) );
		EtVec3TransformNormal( &vDelta, &vDelta, &matRotZ );
		EtVec3TransformNormal( &vNowDir, &vNowDir, &matRotZ );
	}
	// 

	EtVector3 vNowPos = m_vStartPos + vDelta;
	
	EtVec3Normalize( &vNowDir, &vNowDir );
	if( EtVec3LengthSq( &vNowDir ) != 0.0f ) 
		Cross.m_vZAxis = vNowDir;

	Cross.m_vPosition = vNowPos;
	Cross.MakeUpCartesianByZAxis();

	m_fElapsedTime += fDelta;
}

#ifndef _GAMESERVER
//////////////////////////////////////////////////////////////////////////
bool CDnGravityOrbitCalculator::CalcHitGroundPos( MatrixEx Cross, const EtVector3& vShootPos, const float fInitialSpeed, 
												  const EtVector3& vInitialDir, EtVector3& vGroundHitPosition )
{
	bool bResult = false;

	// 계산 부하를 아끼기 위해 낙하할 때 부터 계산한다. 
	// 속도가 0이 되는 시점임.
	EtVector3 vFallingStartPos( 0.0f, 0.0f, 0.0f );
	
	// y 축으로 속도가 0이 되는 시점의 시간을 구함.
	float fFallingStartTime = (-fInitialSpeed*vInitialDir.y) / GRAVITY.y;

	// 낙하 지점으로 발사체 위치 지정..
	vFallingStartPos = (fInitialSpeed*vInitialDir*fFallingStartTime + ((GRAVITY*fFallingStartTime*fFallingStartTime) / 2.0f));
	Cross.m_vPosition = vShootPos;					// vShootPos 는 월드좌표 기준.
	Cross.MoveLocalYAxis( vFallingStartPos.y );			// vFallingStartPos 는 로컬좌표 기준. 따라서 축이동으로 위치 지정.
	Cross.MoveLocalZAxis( vFallingStartPos.z );
	Cross.MoveLocalXAxis( vFallingStartPos.x );
	EtVector3 vPrevPos = Cross.m_vPosition;

	// 해당 지점부터 낙하할 때 까지 지형과 부딪히나 확인..
	float fTimeDelta = 1.0f / 20.0f;
	float fElapsedTime = fFallingStartTime;
	while( true )
	{
		fElapsedTime += fTimeDelta;

		EtVector3 vNowPos = (fInitialSpeed*vInitialDir*fElapsedTime + ((GRAVITY*fElapsedTime*fElapsedTime) / 2.0f));
		Cross.m_vPosition = vShootPos;
		Cross.MoveLocalYAxis( vNowPos.y );
		Cross.MoveLocalZAxis( vNowPos.z );
		Cross.MoveLocalXAxis( vNowPos.x );

		EtVector3 vDir = Cross.m_vPosition - vPrevPos;
		EtVec3Normalize( &vDir, &vDir );
		EtVector3 vPickPos;
		float fHeight = CDnWorld::GetInstance().GetHeight( Cross.m_vPosition );
		if( Cross.m_vPosition.y <= fHeight )
		{
			if( CDnWorld::GetInstance().Pick( vPrevPos, vDir, vPickPos ) == true ) 
			{
				vGroundHitPosition = vPickPos;
				bResult = true;
				break;
			}
		}

		// 일정 시간이 넘어가면 낙하지점 없는 것으로 간주하고 루프 종료.
		if( 5.0f < fElapsedTime )
		{
			// 지형에 충돌되는 지점이 없는 경우 원래 쏜 위치의 y 값에 도달하는 위치로 셋팅해준다.
			float fYPosZeroTime = -2.0f*(fInitialSpeed*vInitialDir.y) / GRAVITY.y;
			EtVector3 vYPosZero = (fInitialSpeed*vInitialDir*fYPosZeroTime + ((GRAVITY*fYPosZeroTime*fYPosZeroTime) / 2.0f));
			Cross.m_vPosition = vShootPos;
			Cross.MoveLocalYAxis( vYPosZero.y );
			Cross.MoveLocalZAxis( vYPosZero.z );
			Cross.MoveLocalXAxis( vYPosZero.x );
			vGroundHitPosition = Cross.m_vPosition;
			break;
		}

		vPrevPos = Cross.m_vPosition;
	}

	return bResult;
}
#endif