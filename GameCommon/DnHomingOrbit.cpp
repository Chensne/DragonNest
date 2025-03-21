#include "StdAfx.h"
#include "DnHomingOrbit.h"
#include "DnActor.h"
#include "DnProjectile.h"


CDnHomingOrbit::CDnHomingOrbit( const S_PROJECTILE_PROPERTY* pProjectileInfo ) : m_fSpeed( 0.0f ),
																				 m_vTargetPosition( 0.0f, 0.0f, 0.0f ),
																				 m_bFallGravity( false )
{
	m_fSpeed = pProjectileInfo->fSpeed;
}

CDnHomingOrbit::~CDnHomingOrbit( void )
{

}



void CDnHomingOrbit::ProcessOrbit( /*IN OUT*/ MatrixEx& Cross, EtVector3& vPrevPos, LOCAL_TIME LocalTime, float fDelta )
{
	if( m_hTargetActor )
	{
		m_vTargetPosition = __super::FindTargetPosition(m_hTargetActor, m_nTargetPartsIndex, m_nTargetPartsBoneIndex);
	}

	EtVector3 vDir = m_vTargetPosition - Cross.m_vPosition;
	float fDistanceSQ = EtVec3LengthSq( &vDir );
	EtVec3Normalize( &vDir, &vDir );
	Cross.m_vPosition += vDir * m_fSpeed * fDelta;

	// fallgravity 상황에선 위치만 갱신해준다.
	if( false == m_bFallGravity )
	{
		Cross.m_vZAxis = vDir;
		Cross.MakeUpCartesianByZAxis();
	}

	// 근처로 거의 다 왔다면 해당 위치로 셋팅
#ifdef _GAMESERVER
	// 게임서버는 20 프레임이기 때문에 체크 간격을 넓게 해준다.
	if( fDistanceSQ < 1000.0f )
		Cross.m_vPosition = m_vTargetPosition;
#else
	if( fDistanceSQ < 100.0f )
		Cross.m_vPosition = m_vTargetPosition;
#endif // #ifdef _GAMESERVER
}



void CDnHomingOrbit::ProcessFallGravityOrbit( MatrixEx& Cross, EtVector3& vPrevPos, LOCAL_TIME LocalTime, float fDelta, float& fGravity, float fFallGravityAccell )
{
	IDnOrbitProcessor::ProcessFallGravityOrbit( Cross, vPrevPos, LocalTime, fDelta, fGravity, fFallGravityAccell );

	m_bFallGravity = true;

	// #29199 이슈 관련. fallgravity 상태가 되면 발사체의 진행방향으로 월드 행렬을 맞춰준다.
	// fallgravity 상황에선 fallgravity 처리된 위치 + 현재 호밍에서 갱신된 위치 기준의 prevpos 를 따로 저장해서 
	// 해당 벡터로 방향을 처리한다. 인자로 넘겨주는 projectile 의 vPrevPos 는 현재 프레임에서 fallgravity 가 적용되기 전이다.
	//if( m_bFallGravity )
	//{
		Cross.m_vZAxis = Cross.m_vPosition - vPrevPos;
		EtVec3Normalize( &Cross.m_vZAxis, &Cross.m_vZAxis);
	//}
}
