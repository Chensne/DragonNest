#include "StdAfx.h"
#include "EtCameraBlurFilter.h"
#include "EtBackBufferMng.h"
#include "EtDrawQuad.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CEtCameraBlurFilter::CEtCameraBlurFilter()
{
	m_Type = SF_CAMERABLUR;
	m_fBlendFactor = 0.0f;
}

CEtCameraBlurFilter::~CEtCameraBlurFilter()
{
	Clear();
}

void CEtCameraBlurFilter::Clear()
{
	SAFE_RELEASE_SPTR( m_hAccumBlurTarget );
	CEtPostProcessFilter::Clear();
}

void CEtCameraBlurFilter::Initialize()
{
	Clear();

	int nWidth, nHeight;
	nWidth = ( int )( GetEtDevice()->Width() * m_fDrawScale );
	nHeight = ( int )( GetEtDevice()->Height() * m_fDrawScale );
	m_hAccumBlurTarget = CEtTexture::CreateRenderTargetTexture( nWidth, nHeight, FMT_A8R8G8B8 );
}

void CEtCameraBlurFilter::Render( float fElapsedTime )
{
	if( !m_hAccumBlurTarget ) return;
	GetEtDevice()->SetVertexShader( NULL );	// Use Fixed Pipeline
	GetEtDevice()->SetPixelShader( NULL );
	GetEtDevice()->SetTexture( 0, NULL );
	bool bAlphaBlend = GetEtDevice()->EnableAlphaBlend( true );

	DrawQuadWithDiffuseTex( &EtVector2( 0.f, 0.f ), &EtVector2( 1.f, 1.f ), &EtVector2( 0.f, 0.f ), &EtVector2( 1.f, 1.f ),
		m_hAccumBlurTarget->GetTexturePtr(), D3DCOLOR_ARGB((int)(m_fBlendFactor*255), 255, 255, 255) );

	GetEtDevice()->SetRenderTarget( m_hAccumBlurTarget->GetSurfaceLevel() );
	GetEtBackBufferMng()->DumpBackBuffer();
	DrawQuadWithTex( &EtVector2( 0.f, 0.f ), &EtVector2( 1.f, 1.f ), &EtVector2( 0.f, 0.f ), &EtVector2( 1.f, 1.f ),
		GetEtBackBufferMng()->GetBackBuffer()->GetTexturePtr() );

	//static int i = 0;
	//if( i % 20 )
	//	D3DXSaveSurfaceToFile( "AccumBlur.dds", D3DXIFF_DDS, m_hAccumBlurTarget->GetSurfaceLevel(), NULL, NULL );
	//++i;

	/*
	////////////////////////////////////////////////////////////////////////////////
	// 테스트코드
	EtVector2 *pStart = &EtVector2( 0.f, 0.f );
	EtVector2 *pEnd = &EtVector2( 1.f, 1.0f );
	SColorVertex Vertices[ 4 ];
	int nWidth, nHeight;
	EtViewPort Viewport;

	GetEtDevice()->SetFVF( D3DFVF_XYZRHW | D3DFVF_DIFFUSE );
	GetEtDevice()->GetViewport( &Viewport );
	nWidth = Viewport.Width;
	nHeight = Viewport.Height;
	for( int i = 0; i < 4; ++i )
	{
		Vertices[ i ].z = 0.0f;
		Vertices[ i ].h = 1.0f;
	}
	Vertices[ 0 ].x = pStart->x * nWidth - 0.5f;
	Vertices[ 0 ].y = pStart->y * nHeight - 0.5f;
	Vertices[ 0 ].color = D3DCOLOR_ARGB((int)(m_fBlendFactor*255), 255, 255, 255);
	Vertices[ 1 ].x = pEnd->x * nWidth - 0.5f;
	Vertices[ 1 ].y = pStart->y * nHeight - 0.5f;
	Vertices[ 1 ].color = D3DCOLOR_ARGB((int)(m_fBlendFactor*255), 255, 255, 255);
	Vertices[ 2 ].x = pEnd->x * nWidth - 0.5f;
	Vertices[ 2 ].y = pEnd->y * nHeight - 0.5f;
	Vertices[ 2 ].color = D3DCOLOR_ARGB((int)(m_fBlendFactor*255), 255, 255, 255);
	Vertices[ 3 ].x = pStart->x * nWidth - 0.5f;
	Vertices[ 3 ].y = pEnd->y * nHeight - 0.5f;
	Vertices[ 3 ].color = D3DCOLOR_ARGB((int)(m_fBlendFactor*255), 255, 255, 255);
	GetEtDevice()->DrawPrimitiveUP( PT_TRIANGLEFAN, 2, Vertices, sizeof( SColorVertex ) );
	////////////////////////////////////////////////////////////////////////////////
	*/

	GetEtDevice()->EnableAlphaBlend( bAlphaBlend );
}

void CEtCameraBlurFilter::OnResetDevice()
{
	int nWidth, nHeight;
	nWidth = ( int )( GetEtDevice()->Width() * m_fDrawScale );
	nHeight = ( int )( GetEtDevice()->Height() * m_fDrawScale );
	m_hAccumBlurTarget->SetSize( nWidth, nHeight );
}