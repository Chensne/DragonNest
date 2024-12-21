#include "StdAfx.h"
#include "DnTerrainHomingOrbit.h"
#include "DnActor.h"
#include "DnProjectile.h"
#include "DnWorld.h"


CDnTerrainHomingOrbit::CDnTerrainHomingOrbit( const S_PROJECTILE_PROPERTY* pProjectileInfo ) : m_fSpeed( 0.0f ),
																				 m_vTargetPosition( 0.0f, 0.0f, 0.0f )
{
	m_fSpeed = pProjectileInfo->fSpeed;
}

CDnTerrainHomingOrbit::~CDnTerrainHomingOrbit( void )
{

}



void CDnTerrainHomingOrbit::ProcessOrbit( /*IN OUT*/ MatrixEx& Cross, EtVector3& vPrevPos, LOCAL_TIME LocalTime, float fDelta )
{
	// 호밍 처리.
	if( m_hTargetActor )
	{
		m_vTargetPosition = __super::FindTargetPosition(m_hTargetActor, m_nTargetPartsIndex, m_nTargetPartsBoneIndex);
	}

	EtVector3 vDir = m_vTargetPosition - Cross.m_vPosition;
	float fDistanceSQ = EtVec3LengthSq( &vDir );
	EtVec3Normalize( &vDir, &vDir );
	Cross.m_vPosition += vDir * m_fSpeed * fDelta;

	// 근처로 거의 다 왔다면 해당 위치로 셋팅
#ifdef _GAMESERVER
	// 게임서버는 20 프레임이기 때문에 체크 간격을 넓게 해준다.
	if( fDistanceSQ < 1000.0f )
		Cross.m_vPosition = m_vTargetPosition;
#else
	if( fDistanceSQ < 100.0f )
		Cross.m_vPosition = m_vTargetPosition;
#endif // #ifdef _GAMESERVER

	// 지형에 붙어서 날아가도록 처리. 다리위 같은 프랍위에 있으면 프랍위 y 위치까지 감안함.
#ifdef _GAMESERVER
	Cross.m_vPosition.y = CDnWorld::GetInstance(m_pRoom).GetHeightWithProp( Cross.m_vPosition );
#else
	Cross.m_vPosition.y = CDnWorld::GetInstance().GetHeightWithProp( Cross.m_vPosition );
#endif

	Cross.m_vPosition.y += 20.0f;
}