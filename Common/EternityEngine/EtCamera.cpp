#include "StdAfx.h"
#include "EtCamera.h"
#include "EtConvexVolume.h"
#include "EtRenderStack.h"
#include "EtRenderStack.h"
#include "EtMRTMng.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

DECL_SMART_PTR_STATIC( CEtCamera, 20 )

EtCameraHandle CEtCamera::s_hActiveCamera;
std::vector<CEtCamera*> CEtCamera::m_pVecProcessList;

CEtCamera::CEtCamera( bool bProcess )
{
	EtMatrixIdentity( &m_ViewMat );
	EtMatrixIdentity( &m_InvViewMat );
	if( bProcess ) {
		m_pVecProcessList.push_back( this );
	}
	m_fAspectRatio = 0.0f;
	memset(&m_Viewport, 0, sizeof(m_Viewport));
}

CEtCamera::~CEtCamera(void)
{
	int i;

	SAFE_RELEASE_SPTR( m_hRenderTarget );
	SAFE_RELEASE_SPTR( m_hDepthTarget );
	for( i = 0; i < ( int )m_vecRenderSkin.size(); i++ )
	{
		SAFE_RELEASE_SPTR( m_vecRenderSkin[ i ].hRenderSkin );
	}
	std::vector<CEtCamera*>::iterator it = std::find( m_pVecProcessList.begin(), m_pVecProcessList.end(), this );
	if( it != m_pVecProcessList.end() ) {
		m_pVecProcessList.erase( it );
	}
}

void CEtCamera::Initialize( SCameraInfo *pCamera )
{
	ASSERT( pCamera && "Invalid Camera Info" );

	m_CameraInfo = *pCamera;
	if( ( m_CameraInfo.Target == CT_RENDERTARGET ) || ( m_CameraInfo.Target == CT_RENDERTARGET_NO_GENERATE_BACKBUFFER ) )
	{
		ASSERT( ( m_CameraInfo.fWidth >= 1.0f ) && ( m_CameraInfo.fHeight >= 1.0f ) );
		m_Viewport.X = 0;
		m_Viewport.Y = 0;
		m_Viewport.Width = ( DWORD )( m_CameraInfo.fWidth );
		m_Viewport.Height = ( DWORD )( m_CameraInfo.fHeight );

		if( m_CameraInfo.Target == CT_RENDERTARGET )
		{
			SAFE_RELEASE_SPTR( m_hRenderTarget );
			SAFE_RELEASE_SPTR( m_hDepthTarget );
			m_hRenderTarget = CEtTexture::CreateRenderTargetTexture( m_Viewport.Width, m_Viewport.Height );
			m_hDepthTarget = CEtDepth::CreateDepthStencil( m_Viewport.Width, m_Viewport.Height );
		}
	}
	Reset();
}

void CEtCamera::Reset()
{
	m_Viewport.MinZ = 0.0f;
	m_Viewport.MaxZ = 1.0f;
	if( m_CameraInfo.Target == CT_SCREEN )
	{
		m_Viewport.X = ( DWORD )( m_CameraInfo.fStartX * GetEtDevice()->Width() );
		m_Viewport.Y = ( DWORD )( m_CameraInfo.fStartY * GetEtDevice()->Height() );
		m_Viewport.Width = ( DWORD )( m_CameraInfo.fWidth * GetEtDevice()->Width() );
		m_Viewport.Height = ( DWORD )( m_CameraInfo.fHeight * GetEtDevice()->Height() );
	}
	m_fAspectRatio = m_Viewport.Width / ( float )m_Viewport.Height;

	if( m_CameraInfo.Type == CT_PERSPECTIVE )
	{
		EtMatrixPerspectiveFovLH( &m_ProjMatForCull, m_CameraInfo.fFOV, m_fAspectRatio, m_CameraInfo.fNear, m_CameraInfo.fFar );
		EtMatrixPerspectiveFovLH( &m_ProjMat, m_CameraInfo.fFOV, m_fAspectRatio, m_CameraInfo.fNear, m_CameraInfo.fFar + 50000.0f );
	}
	else if( m_CameraInfo.Type == CT_ORTHOGONAL )
	{
		EtMatrixOrthoLH( &m_ProjMat, m_CameraInfo.fViewWidth, m_CameraInfo.fViewHeight, m_CameraInfo.fNear, m_CameraInfo.fFar );
	}
	else
	{
		ASSERT( 0 && "Invalid Camera Type!!!" );
	}
//	EtMatrixIdentity( &m_ViewMat );
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

EtMatrix *CEtCamera::GetViewProjMatForCull()
{
	static EtMatrix ViewProjMat;

	return EtMatrixMultiply( &ViewProjMat, &m_ViewMat, &m_ProjMatForCull );
}

void CEtCamera::SetAspectRatio( float fAspectRatio )
{
	m_fAspectRatio = fAspectRatio;
	if( m_CameraInfo.Type == CT_PERSPECTIVE )
	{
		EtMatrixPerspectiveFovLH( &m_ProjMat, m_CameraInfo.fFOV, m_fAspectRatio, m_CameraInfo.fNear, m_CameraInfo.fFar );
	}
}

void CEtCamera::SetFOV( float fFOV )
{
	m_CameraInfo.fFOV = fFOV;
	if( m_CameraInfo.Type == CT_PERSPECTIVE )
	{
		EtMatrixPerspectiveFovLH( &m_ProjMatForCull, m_CameraInfo.fFOV, m_fAspectRatio, m_CameraInfo.fNear, m_CameraInfo.fFar );
		EtMatrixPerspectiveFovLH( &m_ProjMat, m_CameraInfo.fFOV, m_fAspectRatio, m_CameraInfo.fNear, m_CameraInfo.fFar );
	}
}

void CEtCamera::SetOrthogonalView( int nViewWidth, int nViewHeight, float fOffsetX, float fOffsetY )
{
	if( m_CameraInfo.Type == CT_ORTHOGONAL )
	{
		m_CameraInfo.fViewWidth = ( float )nViewWidth;
		m_CameraInfo.fViewHeight = ( float )nViewHeight;
		if( fOffsetX == 0.0f && fOffsetY == 0.0f )
		{
			EtMatrixOrthoLH( &m_ProjMat, m_CameraInfo.fViewWidth, m_CameraInfo.fViewHeight, m_CameraInfo.fNear, m_CameraInfo.fFar );
		}
		else
		{
			EtMatrixOrthoOffCenterLH( &m_ProjMat,
				-m_CameraInfo.fViewWidth/2.0f + fOffsetX, m_CameraInfo.fViewWidth/2.0f + fOffsetX,
				-m_CameraInfo.fViewHeight/2.0f + fOffsetY, m_CameraInfo.fViewHeight/2.0f + fOffsetY,
				m_CameraInfo.fNear, m_CameraInfo.fFar );
		}
	}
}

void CEtCamera::CalcPositionAndDir( int nX, int nY, EtVector3 &Position, EtVector3 &Dir )
{
	Dir = EtVector3(0,0,0);
	if( fabsf(m_ProjMat._11) > 0.001f ) {
		Dir.x = ( ( ( 2.0f * nX ) / GetEtDevice()->Width() ) - 1 ) / m_ProjMat._11;
	}
	if( fabsf(m_ProjMat._22) > 0.001f ) {
		Dir.y = -( ( ( 2.0f * nY ) / GetEtDevice()->Height() ) - 1 ) / m_ProjMat._22;
	}
	Dir.z =  1.0f;
	EtVec3TransformNormal( &Dir, &Dir, &m_InvViewMat );
	EtVec3Normalize( &Dir, &Dir );
	Position.x = m_InvViewMat._41;
	Position.y = m_InvViewMat._42;
	Position.z = m_InvViewMat._43;
}

int CEtCamera::AddRenderObject( EtObjectHandle hObject )
{ 
	m_vecRenderTargetObject.push_back( hObject ); 
	return ( int )m_vecRenderTargetObject.size() - 1;
}

void CEtCamera::DeleteRenderObject( EtObjectHandle hObject )
{
	std::vector< EtObjectHandle >::iterator Iter;

	Iter = std::find( m_vecRenderTargetObject.begin(), m_vecRenderTargetObject.end(), hObject );
	if( Iter != m_vecRenderTargetObject.end() )
	{
		m_vecRenderTargetObject.erase( Iter );
	}
}

void CEtCamera::RenderObjectList()
{
	if( ( !m_hRenderTarget ) || ( m_vecRenderTargetObject.empty() ) )
	{
		return;
	}

	int i;

	GetEtDevice()->SetRenderTarget( m_hRenderTarget->GetSurfaceLevel() );
	GetEtDevice()->SetDepthStencilSurface( m_hDepthTarget->GetDepthBuffer() );
	GetEtDevice()->ClearBuffer( 0x00000000, 1.0f, 0 );
	for( i = 0; i < ( int )m_vecRenderTargetObject.size(); i++ )
	{
		m_vecRenderTargetObject[ i ]->InitRender();
		m_vecRenderTargetObject[ i ]->Render();
	}
	GetEtDevice()->RestoreRenderTarget();
	GetEtDevice()->RestoreDepthStencil();
}

EtSkinHandle CEtCamera::AddRenderSkin( const char *pSkinName, EtMatrix *pWorldMat )
{
	SRenderSkin RenderSkin;

	RenderSkin.szSkinName = pSkinName;
	RenderSkin.SkinRenderMat = *pWorldMat;
	RenderSkin.hRenderSkin = ::LoadResource( pSkinName, RT_SKIN );
	if( !RenderSkin.hRenderSkin ) return RenderSkin.hRenderSkin;
	m_vecRenderSkin.push_back( RenderSkin );

	return RenderSkin.hRenderSkin;
}

void CEtCamera::DeleteRenderSkin( const char *pSkinName )
{
	int i;

	for( i = 0; i < ( int )m_vecRenderSkin.size(); i++ )
	{
		if( stricmp( pSkinName, m_vecRenderSkin[ i ].szSkinName.c_str() ) == 0 )
		{
			SAFE_RELEASE_SPTR( m_vecRenderSkin[ i ].hRenderSkin );
			m_vecRenderSkin.erase( m_vecRenderSkin.begin() + i );
			return;
		}
	}
}

extern CSyncLock *g_pEtRenderLock;
void CEtCamera::RenderSkinList()
{
	if( !m_hRenderTarget )
	{
		return;
	}

	int i;

	if( g_pEtRenderLock )
	{
		g_pEtRenderLock->Lock();
	}
	GetEtDevice()->BeginScene();
	Activate();
	GetEtDevice()->SetRenderTarget( m_hRenderTarget->GetSurfaceLevel() );
	GetEtDevice()->SetDepthStencilSurface( m_hDepthTarget->GetDepthBuffer() );
	GetEtDevice()->ClearBuffer( 0x00000000, 1.0f, 0 );
	for( i = 0; i < ( int )m_vecRenderSkin.size(); i++ )
	{
		m_vecRenderSkin[ i ].hRenderSkin->Render( m_vecRenderSkin[ i ].SkinRenderMat, m_vecRenderSkin[ i ].SkinRenderMat, 1.0f, false );
	}
	GetEtDevice()->RestoreRenderTarget();
	GetEtDevice()->RestoreDepthStencil();
	GetEtDevice()->EndScene();
	if( g_pEtRenderLock )
	{
		g_pEtRenderLock->UnLock();
	}
}

void CEtCamera::RenderTerrain()
{
	if( !m_hRenderTarget )
	{
		return;
	}

	CEtConvexVolume Frustum;
	EtMatrix ViewProjMat;

	GetEtDevice()->BeginScene();
	Activate();
	GetEtDevice()->SetRenderTarget( m_hRenderTarget->GetSurfaceLevel() );
	GetEtDevice()->SetDepthStencilSurface( m_hDepthTarget->GetDepthBuffer() );
	GetEtDevice()->ClearBuffer( 0x00000000, 1.0f, 0 );

	bool bBackup;

	bBackup = CEtMRTMng::GetInstance().IsEnable();
	CEtMRTMng::GetInstance().Enable( false );
	Frustum.Initialize( *GetViewProjMatForCull() );
	CEtTerrainArea::RenderTerrainBlockList( &Frustum );
	GetCurRenderStack()->FlushRender( false );
	CEtMRTMng::GetInstance().Enable( bBackup );

	GetEtDevice()->RestoreRenderTarget();
	GetEtDevice()->RestoreDepthStencil();
	GetEtDevice()->EndScene();
}

void CEtCamera::Render()
{
	if( !m_hRenderTarget )
	{
		return;
	}

	int i;
	CEtConvexVolume Frustum;
	EtMatrix ViewProjMat;

	GetEtDevice()->BeginScene();
	Activate();
	GetEtDevice()->SetRenderTarget( m_hRenderTarget->GetSurfaceLevel() );
	GetEtDevice()->SetDepthStencilSurface( m_hDepthTarget->GetDepthBuffer() );
	GetEtDevice()->ClearBuffer( 0x00000000, 1.0f, 0 );

	bool bBackup;

	bBackup = CEtMRTMng::GetInstance().IsEnable();
	CEtMRTMng::GetInstance().Enable( false );
	Frustum.Initialize( *GetViewProjMatForCull() );
	CEtTerrainArea::RenderTerrainBlockList( &Frustum );
	GetCurRenderStack()->FlushRender( false );
	CEtMRTMng::GetInstance().Enable( bBackup );

	GetEtDevice()->SetRenderTarget( m_hRenderTarget->GetSurfaceLevel() );

	for( i = 0; i < ( int )m_vecRenderSkin.size(); i++ )
	{
		m_vecRenderSkin[ i ].hRenderSkin->Render( m_vecRenderSkin[ i ].SkinRenderMat, m_vecRenderSkin[ i ].SkinRenderMat, 1.0f, false );
	}
	for( i = 0; i < ( int )m_vecRenderTargetObject.size(); i++ )
	{
		m_vecRenderTargetObject[ i ]->InitRender();
		m_vecRenderTargetObject[ i ]->Render();
	}

	GetEtDevice()->RestoreRenderTarget();
	GetEtDevice()->RestoreDepthStencil();
	GetEtDevice()->EndScene();
}

EtCameraHandle CEtCamera::Activate()
{
	s_hActiveCamera = GetMySmartPtr();;
	CalcInvViewMat();
	GetEtDevice()->SetViewport( &m_Viewport );

	return s_hActiveCamera;
}

EtCameraHandle CEtCamera::SetActiveCamera( int nItemIndex )
{
	return GetItem( nItemIndex )->Activate();
}



int CEtCamera::GetProcessItemCount()
{
	return (int)m_pVecProcessList.size();
}

EtCameraHandle CEtCamera::GetProcessItem( int nIndex )
{
	if( nIndex < 0 || nIndex >= (int)m_pVecProcessList.size() ) return CEtCamera::Identity();
	return m_pVecProcessList[nIndex]->GetMySmartPtr();
}
