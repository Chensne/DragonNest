#include "StdAfx.h"
#include "DnCurveOrbit.h"
#include "DnWorld.h"
#include "DnProjectile.h"



CDnCurveOrbit::CDnCurveOrbit( const S_PROJECTILE_PROPERTY* pProjectileInfo ) : m_vTargetPosition( 0.0f, 0.0f, 0.0f ),
																			    m_fSpeed( 0.0f ),
																			    m_bFirstProcess( true )
{
	// TODO: 각 정한 타입별로 시그널이 제대로 셋팅되었는지 ASSERTION 걸어놓자.
	m_fSpeed = pProjectileInfo->fSpeed;
}

CDnCurveOrbit::~CDnCurveOrbit( void )
{

}

void CDnCurveOrbit::ProcessOrbit( /*IN OUT*/ MatrixEx& Cross, EtVector3& vPrevPos, LOCAL_TIME LocalTime, float fDelta )
{
	if( m_bFirstProcess )
	{
#ifdef _GAMESERVER
		m_vTargetPosition.y = CDnWorld::GetInstance(m_pRoom).GetHeight( m_vTargetPosition );
#else
		m_vTargetPosition.y = CDnWorld::GetInstance().GetHeight( m_vTargetPosition );
#endif
		EtVector3 vDir = m_vTargetPosition - Cross.m_vPosition;

		float fLength = EtVec3Length( &vDir );
		EtVec3Normalize( &vDir, &vDir );
		vDir.y = 0.017f;

		float nextPos = fLength / 4.0f;

		EtVector3 v0 = Cross.m_vPosition;
		EtVector3 v1 = Cross.m_vPosition + (vDir * nextPos * 1 );
		v1.y += abs(vDir.y * nextPos * fLength*0.015f);
		EtVector3 v2 = Cross.m_vPosition + (vDir * nextPos * 3 );
		v2.y += abs(vDir.y * nextPos * fLength*0.015f);
		EtVector3 v3 = m_vTargetPosition;// + ( vDir * nextPos * 1 );

		m_CatmullRom.Init( v0, v1, v2, v3, (LOCAL_TIME)( ( fLength / m_fSpeed ) * 1000.f ), LocalTime );
		m_bFirstProcess = false;
	}

	m_CatmullRom.Process( LocalTime );

	EtVector3 vCurPos = m_CatmullRom.GetPos();
	EtVector3 vCurDir = m_CatmullRom.GetCurDir();

	if( EtVec3LengthSq( &vCurDir ) != 0.0f )
		Cross.m_vZAxis = vCurDir;

	Cross.m_vPosition = vCurPos;
	Cross.MakeUpCartesianByZAxis();
}