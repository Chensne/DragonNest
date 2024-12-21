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
	// �ѱ� 2009.7.22
	// ������Ÿ�� ���� ����..  (#4851)
	// Catmullrom ������ ����ϴ� ���� �ƴ϶� �߷°��ӵ��� ����ǵ��� �ٲ�.
	// 2�� ����� ó��..
	if( m_bFirstProcess )
	{
		// Note: distance �� Ŭ���̾�Ʈ���� ������Ÿ�� �����Ǵ� ������ ���̰� direction�� �������� ���Ǵ� ȸ�����̶�� 
		// �ణ�� �ð��� ������ ������ �޶��� �� �����Ƿ� ������Ÿ�� �����Ǵ� ������ direction �� ������ �Ѵ�.
		// ������ ������Ÿ���� Ŭ������ �޾Ƽ� �����ϹǷ� ���������� �̷��� �ؾ� Ŭ�󿡼� ������ ������ �����ϰ� �Ǿ� Ʋ������ �ʴ´�.

		// #37334 ���ϰ� ������� �������� �ִ� ���̰����� ��ǥ������ ���� ���� �� ������ ������ ������ ���� ���� �ִ�.
		// �׷��� ��� ���� �߸� �����Ƿ� ������ ����..
		//if( m_vTargetPosition.y - m_vStartPos.y <= 0.0f )
		//	m_vTargetPosition.y += 50.0f;	// ĳ���� ���� �����ؼ� �� �� �÷���.

		EtVector3 vDistance = m_vTargetPosition - m_vStartPos;

		// ����� Direction�� xz ��ȣ�� ��ġ�ؾ߸� ��... 
		bool bXValid = (vDistance.x < 0.0f && m_vDirectionWhenCreate.x < 0.0f || vDistance.x >= 0.0f && m_vDirectionWhenCreate.x >= 0.0f);
		bool bZValid = (vDistance.z < 0.0f && m_vDirectionWhenCreate.z < 0.0f || vDistance.z >= 0.0f && m_vDirectionWhenCreate.z >= 0.0f);
		bool bValidDirection =  bXValid && bZValid;

		if( 0.0f == m_vDirectionWhenCreate.y )
			m_vDirectionWhenCreate.y = 0.1f;

		// ���� ������ ����� direction �� ���� �� �̹��ϰ� ��ȣ�� �޶����� ��찡 �����Ƿ� direction �� ���󰡵��� ó��.
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

		// ���� ������ ���⸸ŭ ������ ��ü��Ű�� 0.5*Gt^2 + c ������ ���� �������� �ð����� ����
		float fSlope = 0.0f;
		float fC = -vDistance.y;

		// ĳ������ ������ �������� ��찡 �ֳ� OTL
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

		// ������ �Ǵ� ���� � ������� Ȯ���غ���. -_-
		if( fD < 0.0f )
			fD *= -1.0f;
		//_ASSERT( fD >= 0.0f );
		float fEstimateElapseTime = sqrtf( fD ) / (2.0f * fA);

		// �ð��� ������ ���
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

	// ���� ������ ���� �ӵ� ���͸� ����ȭ ��Ų �Ͱ� ����.
	EtVector3 vNowDir = m_vInitialSpeed + m_fElapsedTime*CDnGravityOrbitCalculator::GRAVITY; 
	EtVector3 vDelta = 0.5f*CDnGravityOrbitCalculator::GRAVITY*(m_fElapsedTime*m_fElapsedTime) + m_vInitialSpeed*m_fElapsedTime;

	// ���� ��ü�� ȸ���Ǵ� �ɼ��� �߰���. (#15056)
	// ���࿡ �������� DestPosition ���� �س��� �������� �������� �س��ٸ� ���� ȸ�� ���� ������� �ʰ� x, z �״�� �����.
	// ���� ���, 90���� �������� ������ 90�� �������� ������� ���� �ƴ�. �������� x, z ���� �״�� �̹Ƿ�...
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

	// ��� ���ϸ� �Ƴ��� ���� ������ �� ���� ����Ѵ�. 
	// �ӵ��� 0�� �Ǵ� ������.
	EtVector3 vFallingStartPos( 0.0f, 0.0f, 0.0f );
	
	// y ������ �ӵ��� 0�� �Ǵ� ������ �ð��� ����.
	float fFallingStartTime = (-fInitialSpeed*vInitialDir.y) / GRAVITY.y;

	// ���� �������� �߻�ü ��ġ ����..
	vFallingStartPos = (fInitialSpeed*vInitialDir*fFallingStartTime + ((GRAVITY*fFallingStartTime*fFallingStartTime) / 2.0f));
	Cross.m_vPosition = vShootPos;					// vShootPos �� ������ǥ ����.
	Cross.MoveLocalYAxis( vFallingStartPos.y );			// vFallingStartPos �� ������ǥ ����. ���� ���̵����� ��ġ ����.
	Cross.MoveLocalZAxis( vFallingStartPos.z );
	Cross.MoveLocalXAxis( vFallingStartPos.x );
	EtVector3 vPrevPos = Cross.m_vPosition;

	// �ش� �������� ������ �� ���� ������ �ε����� Ȯ��..
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

		// ���� �ð��� �Ѿ�� �������� ���� ������ �����ϰ� ���� ����.
		if( 5.0f < fElapsedTime )
		{
			// ������ �浹�Ǵ� ������ ���� ��� ���� �� ��ġ�� y ���� �����ϴ� ��ġ�� �������ش�.
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