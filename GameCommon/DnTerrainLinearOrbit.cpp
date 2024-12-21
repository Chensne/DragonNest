#include "StdAfx.h"
#include "DnTerrainLinearOrbit.h"
#include "DnWorld.h"
#include "DnProjectile.h"


CDnTerrainLinearOrbit::CDnTerrainLinearOrbit( const S_PROJECTILE_PROPERTY* pProjectileInfo ) : m_fSpeed( 0.0f ), m_bFirstProcess( false )
{
	m_fSpeed = pProjectileInfo->fSpeed;
	m_bFirstProcess = true;
}

CDnTerrainLinearOrbit::~CDnTerrainLinearOrbit( void )
{

}

void CDnTerrainLinearOrbit::ProcessOrbit( /*IN OUT*/ MatrixEx& Cross, EtVector3& vPrevPos, LOCAL_TIME LocalTime, float fDelta )
{
	// 2009.7.30 한기
	// 다리위 같은 프랍위에 있으면 프랍위 y 위치까지 감안함
	//EtVector3 vPrevPos = Cross.m_vPosition;

	if( m_bFirstProcess )
	{
		Cross.m_vZAxis.y = 0.0f;
		EtVec3Normalize( &Cross.m_vZAxis, &Cross.m_vZAxis );
		Cross.MakeUpCartesianByZAxis();

		m_bFirstProcess = false;
	}

	Cross.m_vPosition += Cross.m_vZAxis * ( fDelta * m_fSpeed );

#ifdef _GAMESERVER
	Cross.m_vPosition.y = CDnWorld::GetInstance(m_pRoom).GetHeightWithProp( Cross.m_vPosition );
#else
	Cross.m_vPosition.y = CDnWorld::GetInstance().GetHeightWithProp( Cross.m_vPosition );
#endif

	Cross.m_vPosition.y += 20.0f;
	//RLKT try fix cleric Judgement Hammer 17.06.2016
	//Cross.m_vZAxis = Cross.m_vPosition - vPrevPos;
	//EtVec3Normalize( &Cross.m_vZAxis, &Cross.m_vZAxis );
	Cross.MakeUpCartesianByZAxis();
}
