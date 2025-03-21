#include "StdAfx.h"
#include "Arcball.h"
#include <assert.h>



CArcBall::CArcBall(void)
{
	ZeroMemory( &m_vClkPos, sizeof(EtVector3) );
	ZeroMemory( &m_vDragPos, sizeof(EtVector3) );
}


CArcBall::CArcBall( float fNewWidth, float fNewHeight )
{
	ZeroMemory( &m_vClkPos, sizeof(EtVector3) );
	ZeroMemory( &m_vDragPos, sizeof(EtVector3) );

	SetBound( fNewWidth, fNewHeight );
}


CArcBall::~CArcBall(void)
{

}


void CArcBall::SetBound( float fNewWidth, float fNewHeight )
{
	assert( (fNewWidth > 1.0f) && (fNewHeight > 1.0f) );

	m_fAdjustWidth = 1.0f / ((fNewWidth - 1.0f) * 0.5f);
	m_fAdjustHeight = 1.0f / ((fNewHeight - 1.0f) * 0.5f);
}


void CArcBall::_MapToSphere( int iNewXPos, int iNewYPos, EtVector3& vNewVec )
{
	float fTempXPos = 0.0f;
	float fTempYPos = 0.0f;
	float fLength = 0.0f;

	fTempXPos = (float)iNewXPos;
	fTempYPos = (float)iNewYPos;

	fTempXPos = (fTempXPos * m_fAdjustWidth) - 1.0f;
	fTempYPos = 1.0f - (fTempYPos * m_fAdjustHeight);

	fLength = fTempXPos*fTempXPos + fTempYPos*fTempYPos;

	if( fLength > 1.0f )
	{
		float fNormal = 1.0f / sqrt( fLength );

		vNewVec.x = fTempXPos * fNormal;
		vNewVec.y = fTempYPos * fNormal;
		vNewVec.z = 0.0f;
	}
	else
	{
		vNewVec.x = fTempXPos;
		vNewVec.y = fTempYPos;
		vNewVec.z = -sqrt( 1.0f - fLength );
	}
}



void CArcBall::Click( int iXPos, int iYPos )
{
	_MapToSphere( iXPos, iYPos, m_vClkPos );
}


void CArcBall::Drag( int iNewXPos, int iNewYPos, EtQuat* qNewRot )
{
	_MapToSphere( iNewXPos, iNewYPos, m_vDragPos );

	if( qNewRot )
	{
		EtVector3 vPerp;

		EtVec3Cross( &vPerp, &m_vClkPos, &m_vDragPos );

		if( EtVec3Length( &vPerp ) > 1.0e-5 )
		{
			EtQuaternionRotationAxis( qNewRot, &vPerp, acosf( EtVec3Dot( &m_vDragPos, &m_vClkPos ) ) );

			qNewRot->x = vPerp.x;
			qNewRot->y = vPerp.y;
			qNewRot->z = vPerp.z;
			qNewRot->w = EtVec3Dot( &m_vDragPos, &m_vClkPos );
		}
		else
		{
			qNewRot->x = qNewRot->y = qNewRot->z = qNewRot->w = 0.0f;
		}
	}
}