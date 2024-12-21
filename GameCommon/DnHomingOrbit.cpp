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

	// fallgravity ��Ȳ���� ��ġ�� �������ش�.
	if( false == m_bFallGravity )
	{
		Cross.m_vZAxis = vDir;
		Cross.MakeUpCartesianByZAxis();
	}

	// ��ó�� ���� �� �Դٸ� �ش� ��ġ�� ����
#ifdef _GAMESERVER
	// ���Ӽ����� 20 �������̱� ������ üũ ������ �а� ���ش�.
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

	// #29199 �̽� ����. fallgravity ���°� �Ǹ� �߻�ü�� ����������� ���� ����� �����ش�.
	// fallgravity ��Ȳ���� fallgravity ó���� ��ġ + ���� ȣ�ֿ��� ���ŵ� ��ġ ������ prevpos �� ���� �����ؼ� 
	// �ش� ���ͷ� ������ ó���Ѵ�. ���ڷ� �Ѱ��ִ� projectile �� vPrevPos �� ���� �����ӿ��� fallgravity �� ����Ǳ� ���̴�.
	//if( m_bFallGravity )
	//{
		Cross.m_vZAxis = Cross.m_vPosition - vPrevPos;
		EtVec3Normalize( &Cross.m_vZAxis, &Cross.m_vZAxis);
	//}
}
