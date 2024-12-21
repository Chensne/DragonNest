#include "StdAfx.h"
#include "EtBloomFilter.h"
#include "EtBackBufferMng.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CEtBloomFilter::CEtBloomFilter()
{
	SetBloomScale( 1.0f );
	SetBloomSensitivity( 6.0f );
	SetDrawSacle( 1.0f / 8 );

	m_Type = SF_BLOOM;
}

CEtBloomFilter::~CEtBloomFilter()
{
	Clear();
}

void CEtBloomFilter::Clear()
{
	SAFE_RELEASE_SPTR( m_hBrightTarget );
	SAFE_RELEASE_SPTR( m_hHoriBlurTarget );
	SAFE_RELEASE_SPTR( m_hVertBlurTarget );
	CEtPostProcessFilter::Clear();
}

void CEtBloomFilter::Initialize()
{
	Clear();

	EtVector4 PixelSize;

	m_hMaterial = LoadResource( "BloomFilter.fx", RT_SHADER );
	if( !m_hMaterial ) return;

	AddCustomParam( m_vecCustomParam, EPT_FLOAT_PTR, m_hMaterial, "g_fBloomScale", &m_fBloomScale );
	PixelSize.x = 1.0f / ( GetEtDevice()->Width() * m_fDrawScale );
	PixelSize.y = 1.0f / ( GetEtDevice()->Height() * m_fDrawScale );
	AddCustomParam( m_vecCustomParam, EPT_VECTOR, m_hMaterial, "g_fPixelSize", &PixelSize );
	int nBackBufferIndex = GetEtBackBufferMng()->GetBackBufferIndex();
	AddCustomParam( m_vecCustomParam, EPT_TEX, m_hMaterial, "g_BackBuffer", &nBackBufferIndex );

	int nWidth, nHeight;

	nWidth = ( int )( GetEtDevice()->Width() * m_fDrawScale );
	nHeight = ( int )( GetEtDevice()->Height() * m_fDrawScale );
	m_hBrightTarget = AddRenderTarget( nWidth, nHeight, "g_BrightPass" );
	m_hHoriBlurTarget = AddRenderTarget( nWidth, nHeight, "g_BloomHori" );
	m_hVertBlurTarget = AddRenderTarget( nWidth, nHeight, "g_BloomVert" );
}

void CEtBloomFilter::Render( float fElapsedTime )
{
	DrawFilter( m_hBrightTarget, 0 );
	DrawFilter( m_hHoriBlurTarget, 1 );
	DrawFilter( m_hVertBlurTarget, 2 );
	DrawFilter( CEtTexture::Identity(), 3 );
}

void CEtBloomFilter::OnResetDevice()
{
	int nWidth, nHeight;

	nWidth = ( int )( GetEtDevice()->Width() * m_fDrawScale );
	nHeight = ( int )( GetEtDevice()->Height() * m_fDrawScale );
	m_hBrightTarget->SetSize( nWidth, nHeight );
	m_hHoriBlurTarget->SetSize( nWidth, nHeight );
	m_hVertBlurTarget->SetSize( nWidth, nHeight );
}