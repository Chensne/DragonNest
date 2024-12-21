#include "stdafx.h"
#include "EtMatrixEx.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

MatrixEx::MatrixEx()
{
	Identity();
}

MatrixEx::MatrixEx( const EtMatrix &Matrix )
{
	*( EtMatrix * )this = Matrix;
}

void MatrixEx::Identity()
{
	EtMatrixIdentity( ( EtMatrix * )this );
}

void MatrixEx::RotateYaw( float fAngle )
{
	EtMatrix matRotYaw;
	EtMatrixRotationAxis( &matRotYaw, &m_vYAxis, -D3DXToRadian(fAngle) );
	EtVec3TransformNormal( &m_vXAxis, &m_vXAxis, &matRotYaw );
	EtVec3Normalize( &m_vXAxis, &m_vXAxis );
	EtVec3TransformNormal( &m_vZAxis, &m_vZAxis, &matRotYaw );
	EtVec3Normalize( &m_vZAxis, &m_vZAxis );
}

void MatrixEx::RotateRoll( float fAngle )
{
	EtMatrix matRotRoll;
	EtMatrixRotationAxis( &matRotRoll, &m_vZAxis, -D3DXToRadian( fAngle ) );
	EtVec3TransformNormal( &m_vXAxis, &m_vXAxis, &matRotRoll );
	EtVec3Normalize(&m_vXAxis, &m_vXAxis);
	EtVec3TransformNormal( &m_vYAxis, &m_vYAxis, &matRotRoll );
	EtVec3Normalize(&m_vYAxis, &m_vYAxis);
}

void MatrixEx::RotatePitch( float fAngle )
{
	EtMatrix matRotPitch;
	EtMatrixRotationAxis( &matRotPitch, &m_vXAxis, D3DXToRadian( fAngle ) );
	EtVec3TransformNormal( &m_vYAxis, &m_vYAxis, &matRotPitch );
	EtVec3Normalize(&m_vYAxis, &m_vYAxis);
	EtVec3TransformNormal( &m_vZAxis, &m_vZAxis, &matRotPitch );
	EtVec3Normalize(&m_vZAxis, &m_vZAxis);
}

void MatrixEx::RotateYAxis( float fAngle )
{
	EtMatrix matRotYaw;
	EtMatrixRotationY(&matRotYaw, D3DXToRadian(fAngle));
	EtVec3TransformNormal(&m_vZAxis, &m_vZAxis, &matRotYaw);
	EtVec3TransformNormal(&m_vXAxis, &m_vXAxis, &matRotYaw);
	EtVec3Normalize(&m_vZAxis, &m_vZAxis);
	EtVec3Normalize(&m_vXAxis, &m_vXAxis);
	EtVec3Cross(&m_vYAxis, &m_vZAxis, &m_vXAxis);
}

void MatrixEx::RotateZAxis( float fAngle )
{
	EtMatrix matRotRoll;

	EtMatrixRotationZ(&matRotRoll, D3DXToRadian(fAngle));
	EtVec3TransformNormal(&m_vZAxis, &m_vZAxis, &matRotRoll);
	EtVec3TransformNormal(&m_vXAxis, &m_vXAxis, &matRotRoll);
	EtVec3Normalize(&m_vZAxis, &m_vZAxis);
	EtVec3Normalize(&m_vXAxis, &m_vXAxis);
	EtVec3Cross(&m_vYAxis, &m_vZAxis, &m_vXAxis);
}

void MatrixEx::RotateXAxis( float fAngle )
{
	EtMatrix matRotPitch;

	EtMatrixRotationX(&matRotPitch, D3DXToRadian(fAngle));
	EtVec3TransformNormal(&m_vZAxis, &m_vZAxis, &matRotPitch);
	EtVec3TransformNormal(&m_vXAxis, &m_vXAxis, &matRotPitch);
	EtVec3Normalize(&m_vZAxis, &m_vZAxis);
	EtVec3Normalize(&m_vXAxis, &m_vXAxis);
	EtVec3Cross(&m_vYAxis, &m_vZAxis, &m_vXAxis);
}

void MatrixEx::SetPosition(float fX, float fY, float fZ)
{
	m_vPosition.x=fX;
	m_vPosition.y=fY;
	m_vPosition.z=fZ;
}

MatrixEx::operator EtMatrix *()
{
	return (EtMatrix*)this;
}

MatrixEx::operator EtMatrix &()
{
	return *(EtMatrix*)this;
}

MatrixEx &MatrixEx::operator = (EtMatrix &Matrix)
{
	*(EtMatrix*)this = Matrix;

	return *this;
}

void MatrixEx::CopyRotationFromThis( MatrixEx *pCross )
{
	if(pCross)
	{
		m_vXAxis = pCross->m_vXAxis;
		m_vYAxis = pCross->m_vYAxis;
		m_vZAxis = pCross->m_vZAxis;
	}
}

void MatrixEx::MakeUpCartesianByZAxis()
{	
	EtVec3Cross(&m_vXAxis, &m_vYAxis, &m_vZAxis);
	EtVec3Normalize(&m_vXAxis, &m_vXAxis);

	EtVec3Cross(&m_vYAxis, &m_vZAxis, &m_vXAxis);
	EtVec3Normalize(&m_vYAxis, &m_vYAxis);

	if( EtVec3LengthSq( &m_vXAxis ) == 0.f && EtVec3LengthSq( &m_vYAxis ) == 0.f ) {
		m_vXAxis = EtVector3( 1.f, 0.f, 0.f );
		MakeUpCartesianByXAxis();
	}
}

void MatrixEx::MakeUpCartesianByYAxis()
{
	EtVec3Cross(&m_vXAxis, &m_vYAxis, &m_vZAxis);
	EtVec3Normalize(&m_vXAxis, &m_vXAxis);

	EtVec3Cross(&m_vZAxis, &m_vXAxis, &m_vYAxis);
	EtVec3Normalize(&m_vZAxis, &m_vZAxis);
}

void MatrixEx::MakeUpCartesianByXAxis()
{
	EtVec3Cross(&m_vYAxis, &m_vZAxis, &m_vXAxis);
	EtVec3Normalize(&m_vYAxis, &m_vYAxis);

	EtVec3Cross(&m_vZAxis, &m_vXAxis, &m_vYAxis);
	EtVec3Normalize(&m_vZAxis, &m_vZAxis);
}

void MatrixEx::Look( EtVector3 *pTarget, EtVector3 *pUpVector /*=NULL*/ )
{
	EtVector3 vToTarget = *pTarget-m_vPosition;

	if( EtVec3Length( &vToTarget ) < 0.01f )
		return;

	EtVec3Normalize( &m_vZAxis, &vToTarget );
	
	if( pUpVector )
	{
		m_vYAxis =* pUpVector;
	}

	EtVec3Cross( &m_vXAxis, &m_vYAxis, &m_vZAxis );
	EtVec3Normalize( &m_vXAxis, &m_vXAxis );

	EtVec3Cross( &m_vYAxis, &m_vZAxis, &m_vXAxis );
	EtVec3Normalize( &m_vYAxis, &m_vYAxis );
}