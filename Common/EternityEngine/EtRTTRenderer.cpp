#include "StdAfx.h"
#include "EtRTTRenderer.h"
#include "EtBackBufferMng.h"
#include "EtDrawQuad.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

DECL_SMART_PTR_STATIC( CEtRTTRenderer, 200 )

CEtRTTRenderer::CEtRTTRenderer(void)
{
	m_nWidth = -1;
	m_nHeight = -1;
	m_Format = FMT_A8B8G8R8;
	m_nRenderFrameCount = 0;
	m_bRenderChild = true;
}

CEtRTTRenderer::~CEtRTTRenderer(void)
{
	SAFE_RELEASE_SPTR( m_hCamera );
	SAFE_RELEASE_SPTR( m_hRenderTarget );
	SAFE_RELEASE_SPTR( m_hDepthTarget );
	ClearRenderObject();
}

void CEtRTTRenderer::Initialize( SCameraInfo &CameraInfo, int nWidth, int nHeight, int nViewPortWidth, int nViewPortHeight, EtFormat Format )
{
	m_nWidth = nWidth;
	m_nHeight = nHeight;
	m_Format = Format;

	m_hRenderTarget = CEtTexture::CreateRenderTargetTexture( m_nWidth, m_nHeight, m_Format );
	m_hDepthTarget = CEtDepth::CreateDepthStencil( m_nWidth, m_nHeight, FMT_D24S8 );//, GetEtDevice()->GetSupportedAAType() );

	CameraInfo.Target = CT_RENDERTARGET_NO_GENERATE_BACKBUFFER;
	if( ( nViewPortWidth == -1 ) || ( nViewPortHeight == -1 ) )
	{
		CameraInfo.fWidth = ( float )m_nWidth;
		CameraInfo.fHeight = ( float )m_nHeight;
	}
	else
	{
		CameraInfo.fViewWidth = ( float )nViewPortWidth;
		CameraInfo.fViewHeight = ( float )nViewPortHeight;
	}
	m_hCamera = EternityEngine::CreateCamera( &CameraInfo );
}

void CEtRTTRenderer::InitCamera( SCameraInfo &CameraInfo ) 
{ 
	CameraInfo.Target = CT_RENDERTARGET_NO_GENERATE_BACKBUFFER;
	CameraInfo.fWidth = ( float )m_nWidth;
	CameraInfo.fHeight = ( float )m_nHeight;
	m_hCamera->Initialize( &CameraInfo ); 
}

void CEtRTTRenderer::ClearRenderObject()
{
	SAFE_RELEASE_SPTRVEC( m_vecEffectObject );
	SAFE_RELEASE_SPTRVEC( m_vecParticle );
	int i;
	for( i = 0; i < ( int )m_vecObject.size(); i++ )
	{
		if( m_vecObject[ i ].bDelete )
		{
			SAFE_RELEASE_SPTR( m_vecObject[ i ].hHandle );
		}
	}
	m_vecObject.clear();
}

EtObjectHandle CEtRTTRenderer::CreateStaticObject( const char *pSkinName )
{
	EtObjectHandle hObject = EternityEngine::CreateStaticObject( pSkinName, false );
	if( hObject )
	{
		SRTTObject RTTObject;
		RTTObject.hHandle = hObject;
		RTTObject.bDelete = true;
		m_vecObject.push_back( RTTObject );
		hObject->SetRenderType( RT_TARGET );
	}

	return hObject;
}

EtAniObjectHandle CEtRTTRenderer::CreateAniObject( const char *pSkinName, const char *pAniName )
{
	EtAniObjectHandle hObject = EternityEngine::CreateAniObject( pSkinName, pAniName, false );
	if( hObject )
	{
		SRTTObject RTTObject;
		RTTObject.hHandle = hObject;
		RTTObject.bDelete = true;
		m_vecObject.push_back( RTTObject );
		hObject->SetRenderType( RT_TARGET );
	}

	return hObject;
}

EtAniObjectHandle CEtRTTRenderer::CreateAniObject( EtSkinHandle hSkin, const char *pAniName )
{
	EtAniObjectHandle hObject = EternityEngine::CreateAniObject( hSkin, pAniName, false );
	if( hObject )
	{
		SRTTObject RTTObject;
		RTTObject.hHandle = hObject;
		RTTObject.bDelete = true;
		m_vecObject.push_back( RTTObject );
		hObject->SetRenderType( RT_TARGET );
	}

	return hObject;
}

void CEtRTTRenderer::AddObject( EtObjectHandle hObject, bool bAutoDelete )
{
	if( hObject )
	{
		SRTTObject RTTObject;
		RTTObject.hHandle = hObject;
		RTTObject.bDelete = bAutoDelete;
		m_vecObject.push_back( RTTObject );
		// 외부에서 Add된 오브젝트는 다른곳에 렌더링으로 쓰일 수 있기 때문에 RT_TARGET하지는 않는다..
//		hObject->SetRenderType( RT_TARGET );
	}
}

EtBillboardEffectHandle CEtRTTRenderer::CreateBillboardEffect( int nParticleDataIndex, EtMatrix *pWorldMat, bool bReuseParticleInstance)
{
	EtBillboardEffectHandle hParticle = EternityEngine::CreateBillboardEffect( nParticleDataIndex, pWorldMat, bReuseParticleInstance );
	if( hParticle )
	{
		m_vecParticle.push_back( hParticle );
		hParticle->SetRenderType( RT_TARGET );
	}

	return hParticle;
}

EtEffectObjectHandle CEtRTTRenderer::CreateEffectObject( int nEffectDataIndex, EtMatrix *pWorldMat )
{
	EtEffectObjectHandle hEffectObject = EternityEngine::CreateEffectObject( nEffectDataIndex, pWorldMat );
	if( hEffectObject )
	{
		m_vecEffectObject.push_back( hEffectObject );
		hEffectObject->SetRenderType( RT_TARGET );
	}

	return hEffectObject;
}

void CEtRTTRenderer::InitRenderObject()
{
	for( DWORD itr = 0; itr < m_vecObject.size(); ++itr )
	{
		if( m_vecObject[ itr ].hHandle )
			m_vecObject[ itr ].hHandle->InitRender();
	}
}

void CEtRTTRenderer::BeginRTT()
{
	m_hCamera->Activate();
	GetEtDevice()->SetRenderTarget( m_hRenderTarget->GetSurfaceLevel() );
	GetEtDevice()->SetDepthStencilSurface( m_hDepthTarget->GetDepthBuffer() );
	GetEtDevice()->ClearBuffer( 0x0, 1.0f, 0, true, true, false );
	
//  테스트중 백버퍼 사용하기
// 	GetEtBackBufferMng()->DumpBackBuffer();
// 	DrawQuadWithTex( &EtVector2( 0.f, 0.f ), &EtVector2( 1.f, 1.f ), &EtVector2( 0.f, 0.f ), &EtVector2( 1.f, 1.f ),
// 	GetEtBackBufferMng()->GetBackBuffer()->GetTexturePtr() , 1.0f );

}

void CEtRTTRenderer::Render( float fElapsedTime )
{
	EtViewPort OldViewPort;
	GetEtDevice()->GetViewport( &OldViewPort );
	BeginRTT();
	RenderRTT( fElapsedTime );
	EndRTT();
	if( m_nRenderFrameCount > 0 )
	{
		m_nRenderFrameCount--;
		if( m_nRenderFrameCount <= 0 )
		{
			ClearRenderObject();
		}
	}
	GetEtDevice()->SetViewport( &OldViewPort );
}

void CEtRTTRenderer::EndRTT()
{
	GetEtDevice()->RestoreDepthStencil();
	GetEtDevice()->RestoreRenderTarget();
}

void CEtRTTRenderer::RenderRTT( float fElapsedTime )
{
	int i;

	for( i = 0; i < ( int )m_vecObject.size(); i++ )
	{
		if( m_vecObject[ i ].hHandle )
			m_vecObject[ i ].hHandle->RenderImmediate( m_bRenderChild );
	}

	// 0x0에 그리는거기때문에 알파값이 누적되는 형태가 제일 어울린다.(SrcAlpha-InvSrcAlpha보다 이게 더 잘 보이게 된다.)
	// 그래서 아래와 같이 알파만 따로 블렌딩 옵션으로 조절한다.
	// D3DBLENDOP_MAX 옵션을 사용해서 그려지는 픽셀 중 제일 높은 알파만 사용할 수도 있으나 One-One 블렌딩보다 흐리기때문에 사용하지 않는다.
	bool bAlphaBlend = GetEtDevice()->EnableAlphaBlend( true );
	GetEtDevice()->SetRenderState( D3DRS_SEPARATEALPHABLENDENABLE, TRUE );
	GetEtDevice()->SetRenderState( D3DRS_SRCBLENDALPHA, D3DBLEND_ONE );
	GetEtDevice()->SetRenderState( D3DRS_DESTBLENDALPHA, D3DBLEND_ONE );
	bool bZWrite = GetEtDevice()->EnableZWrite( false );

	// 일반 필드와 달리 거리에 따라 정렬하지 않기 때문에, 깔끔하게 테두리 안빠질때 있다.
	// 이것까지 보완하려면 아래 알파객체들 그릴때 거리 먼 것부터 그리면 된다.
	for( i = 0; i < ( int )m_vecObject.size(); i++ )
	{
		if( m_vecObject[ i ].hHandle )
		{
			m_vecObject[ i ].hHandle->RenderAlphaImmediate( m_bRenderChild );
		}
	}

	for( i = 0; i < ( int )m_vecEffectObject.size(); i++ )
	{
		if( m_vecEffectObject[ i ] )
		{
			m_vecEffectObject[ i ]->RenderImmediate( fElapsedTime );
		}
	}

	for( i = 0; i < ( int )m_vecParticle.size(); i++ )
	{
		if( m_vecParticle[ i ] )
		{
			m_vecParticle[ i ]->RenderImmediate();
		}
	}

	GetEtDevice()->EnableZWrite( bZWrite );

	GetEtDevice()->EnableAlphaBlend( bAlphaBlend );
	GetEtDevice()->SetRenderState( D3DRS_SEPARATEALPHABLENDENABLE, FALSE );
	GetEtDevice()->SetRenderState( D3DRS_SRCBLENDALPHA, D3DBLEND_ONE );
	GetEtDevice()->SetRenderState( D3DRS_DESTBLENDALPHA, D3DBLEND_ZERO );
}

void CEtRTTRenderer::RenderRTTList( float fElapsedTime )
{
	ScopeLock<CSyncLock> Lock( s_SmartPtrLock );

	int i, nCount;

	nCount = GetItemCount();
	for( i = 0; i < nCount; i++ )
	{
		EtRTTHandle hHandle = GetItem( i );
		if( !hHandle )
		{
			continue;
		}
		if( hHandle->m_nRenderFrameCount == 0 )
		{
			continue;
		}
		hHandle->Render( fElapsedTime );
	}
}
