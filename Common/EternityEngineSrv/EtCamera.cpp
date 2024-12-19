#include "StdAfx.h"
#include "EtCamera.h"
#include "EtConvexVolume.h"

DECL_MULTISMART_PTR_STATIC( CEtCamera, MAX_SESSION_COUNT, 1 )

STATIC_DECL_INIT( CEtCamera, EtCameraHandle, s_hActiveCamera );

CEtCamera::CEtCamera( CMultiRoom *pRoom )
: CMultiSmartPtrBase< CEtCamera, MAX_SESSION_COUNT>(pRoom)
{
	m_fAspectRatio =0.0f;
}

CEtCamera::~CEtCamera(void)
{
}

void CEtCamera::Initialize( SCameraInfo *pCamera )
{
	ASSERT( pCamera && "Invalid Camera Info" );

	m_CameraInfo = *pCamera;
	Reset();
}

void CEtCamera::Reset()
{
	if( m_CameraInfo.Type == CT_PERSPECTIVE )
	{
		EtMatrixPerspectiveFovLH( &m_ProjMat, m_CameraInfo.fFOV, m_fAspectRatio, m_CameraInfo.fNear, m_CameraInfo.fFar );
	}
	else if( m_CameraInfo.Type == CT_ORTHOGONAL )
	{
		EtMatrixOrthoLH( &m_ProjMat, m_CameraInfo.fViewWidth, m_CameraInfo.fViewHeight, m_CameraInfo.fNear, m_CameraInfo.fFar );
	}
	else
	{
		ASSERT( 0 && "Invalid Camera Type!!!" );
	}
	EtMatrixIdentity( &m_ViewMat );
}

void CEtCamera::LookAt( EtVector3 &Eye, EtVector3 &At, EtVector3 &Up )
{
	EtMatrixLookAtLH( &m_ViewMat, &Eye, &At, &Up );
}

EtMatrix *CEtCamera::GetViewProjMat()
{
	static EtMatrix ViewProjMat;

	return EtMatrixMultiply( &ViewProjMat, &m_ViewMat, &m_ProjMat );
}

void CEtCamera::CalcPositionAndDir( int nX, int nY, EtVector3 &Position, EtVector3 &Dir )
{
}

EtCameraHandle CEtCamera::Activate()
{
	STATIC_INSTANCE(s_hActiveCamera) = GetMySmartPtr();
	CalcInvViewMat();

	return STATIC_INSTANCE(s_hActiveCamera);
}

EtCameraHandle CEtCamera::SetActiveCamera( CMultiRoom *pRoom, int nItemIndex )
{
	return GetItem( pRoom, nItemIndex )->Activate();
}

