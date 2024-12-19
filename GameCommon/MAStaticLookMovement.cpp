
#include "Stdafx.h"
#include "MAStaticLookMovement.h"
#include "DnActor.h"

MAStaticLookMovement::MAStaticLookMovement()
:MAStaticMovement()
,m_vTargetLookVec( 0.f, 0.f )
,m_LastLookTargetTime(0)
{

}

void MAStaticLookMovement::Look( EtVector2 &vVec, bool bForceRotate )
{
	if( bForceRotate ) {
		if( EtVec2LengthSq( &vVec ) > 0.f ) {
			m_pMatExWorld->m_vZAxis = EtVector3( vVec.x, 0.f, vVec.y );
			EtVec3Normalize( &m_pMatExWorld->m_vZAxis, &m_pMatExWorld->m_vZAxis );
			m_pMatExWorld->MakeUpCartesianByZAxis();
		}
		m_vTargetLookVec = EtVector2( 0.f, 0.f );
	}
	else {
		m_vTargetLookVec = vVec;
	}
}

EtVector3 *MAStaticLookMovement::GetLookDir()
{
	return &m_pMatExWorld->m_vZAxis;
}

void MAStaticLookMovement::LookTarget( DnActorHandle &hActor )
{
	m_hLookTarget = hActor;

	if ( m_hLookTarget )
	{
		OnBeginLook();
	}
	else
	{
		OnEndLook();
	}
}

void MAStaticLookMovement::ResetLook()
{
	m_hLookTarget.Identity();
	m_vTargetLookVec = EtVector2( 0.f, 0.f );
}

DnActorHandle MAStaticLookMovement::GetLookTarget()
{
	return m_hLookTarget;
}

void MAStaticLookMovement::Process( LOCAL_TIME LocalTime, float fDelta )
{
	m_pActor->SetPrevPosition( m_pMatExWorld->m_vPosition );

	EtVector3 *vDist = m_pActor->GetAniDistance();
	m_pMatExWorld->m_vPosition += m_pMatExWorld->m_vXAxis * vDist->x;
	m_pMatExWorld->m_vPosition += m_pMatExWorld->m_vYAxis * vDist->y;
	m_pMatExWorld->m_vPosition += m_pMatExWorld->m_vZAxis * vDist->z;
	m_pActor->SetStaticPosition( m_pMatExWorld->m_vPosition );
	// Look 贸府
	if( m_hLookTarget ) {
		EtVector2 vLook;
		vLook.x = m_hLookTarget->GetPosition()->x - m_pMatExWorld->m_vPosition.x;
		vLook.y = m_hLookTarget->GetPosition()->z - m_pMatExWorld->m_vPosition.z;
		if( EtVec2LengthSq( &vLook ) > 0.f ) {
			EtVec2Normalize( &vLook, &vLook );
			float fDot = EtVec2Dot( &vLook, &EtVec3toVec2( m_pMatExWorld->m_vZAxis ) );
			if( fDot >= 1.f ) fDot = 1.f;
			float fAngle = EtToDegree( EtAcos( fDot ) );
			bool bForceLook = true;

			if( fAngle > m_pActor->GetRotateAngleSpeed() * fDelta ) bForceLook = false;

			m_pActor->Look( vLook, bForceLook );

			if ( bForceLook )
			{
				OnEndLook();
			}
			m_LastLookTargetTime = LocalTime;
		}
	}

	// Look 贸府
	if( EtVec2LengthSq( &m_vTargetLookVec ) > 0.f ) {
		float fDot = D3DXVec2Dot( &m_vTargetLookVec, &EtVec3toVec2( m_pMatExWorld->m_vZAxis ) );
		if( fDot >= 1.f ) fDot = 1.f;
		float fAngle = EtToDegree( EtAcos(fDot) );
		float fAngleSpeed = m_pActor->GetRotateAngleSpeed() * fDelta;
		if( fAngle > fAngleSpeed ) {
			EtVector3 vCrossVec;
			D3DXVec3Cross( &vCrossVec, &EtVec2toVec3( m_vTargetLookVec ), &m_pMatExWorld->m_vZAxis );
			if( vCrossVec.y > 0.f )
				m_pMatExWorld->RotateYaw( fAngleSpeed );
			else m_pMatExWorld->RotateYaw( -fAngleSpeed );

			fDot = D3DXVec2Dot( &m_vTargetLookVec, &EtVec3toVec2( m_pMatExWorld->m_vZAxis ) );
			if( fDot >= 1.f ) fDot = 1.f;
			fAngle = EtToDegree( EtAcos(fDot) );
			if( fAngle <= fAngleSpeed ) {
				m_pMatExWorld->m_vZAxis = EtVec2toVec3( m_vTargetLookVec );
				m_pMatExWorld->MakeUpCartesianByZAxis();
				m_vTargetLookVec = EtVector2( 0.f, 0.f );
			}
		}
		else {
			m_pMatExWorld->m_vZAxis = EtVec2toVec3( m_vTargetLookVec );
			m_pMatExWorld->MakeUpCartesianByZAxis();
			m_vTargetLookVec = EtVector2( 0.f, 0.f );
		}
	}
}
