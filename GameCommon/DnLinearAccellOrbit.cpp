#include "StdAfx.h"
#include "DnLinearAccellOrbit.h"
#include "VelocityFunc.h"
#include "DnProjectile.h"



CDnLinearAccellOrbit::CDnLinearAccellOrbit( const S_PROJECTILE_PROPERTY* pProjectileInfo ) : m_fSpeed( 0.0f ),
																							 m_fResistance( 0.0f )
{
	m_fSpeed = pProjectileInfo->fSpeed;
	m_fResistance = pProjectileInfo->fResistance;
}

CDnLinearAccellOrbit::~CDnLinearAccellOrbit( void )
{

}

void CDnLinearAccellOrbit::ProcessOrbit( /*IN OUT*/ MatrixEx& Cross, EtVector3& vPrevPos, LOCAL_TIME LocalTime, float fDelta )
{
	float fResult = CalcMovement( m_fSpeed, fDelta, FLT_MAX, -FLT_MAX, m_fResistance );

	Cross.m_vPosition += Cross.m_vZAxis * fResult;
}