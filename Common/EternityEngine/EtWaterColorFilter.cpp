#include "StdAfx.h"
#include "EtWaterColorFilter.h"
#include "EtBackBufferMng.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CEtWaterColorFilter::CEtWaterColorFilter()
{
	m_Type = SF_WATERCOLOR;
}

CEtWaterColorFilter::~CEtWaterColorFilter()
{
}

void CEtWaterColorFilter::Clear()
{
	SAFE_RELEASE_SPTR( m_hHoriBlurTarget );
	SAFE_RELEASE_SPTR( m_hLookupTex );
	CEtPostProcessFilter::Clear();
}

int g_nGradeCount = 12;
void CEtWaterColorFilter::Initialize()
{
	Clear();

	m_hMaterial = LoadResource( "WaterColorFilter.fx", RT_SHADER );
	if( !m_hMaterial ) return;

	AddCustomParam( m_vecCustomParam, EPT_VECTOR_PTR, m_hMaterial, "g_fBlurRadius", &m_vBlurRadius );
	int nBackBufferIndex = GetEtBackBufferMng()->GetBackBufferIndex();
	AddCustomParam( m_vecCustomParam, EPT_TEX, m_hMaterial, "g_BackBuffer", &nBackBufferIndex );

	m_hHoriBlurTarget = AddRenderTarget( -1, 01, "g_BlurHoriBuffer" );
	m_hLookupTex = CEtTexture::CreateNormalTexture( 256, 1, FMT_A8R8G8B8, USAGE_DEFAULT, POOL_MANAGED );
	int nTexIndex = m_hLookupTex->GetMyIndex();
	AddCustomParam( m_vecCustomParam, EPT_TEX, m_hMaterial, "g_LookupTex", &nTexIndex );

	float fBlurSize = 5;
	m_vBlurRadius.x = fBlurSize / 1024.0f;
	m_vBlurRadius.y = m_vBlurRadius.x * ( GetEtDevice()->Width() / ( float )GetEtDevice()->Height() );

	SetGradeCount( g_nGradeCount );
}

void CEtWaterColorFilter::Render( float fElapsedTime )
{
	DrawFilter( m_hHoriBlurTarget, 0 );
	DrawFilter( GetEtBackBufferMng()->GetBackBuffer(), 1 );
	DrawFilter( CEtTexture::Identity(), 2 );
}

void CEtWaterColorFilter::SetGradeCount( int nGrade )
{
	int i, nStride;
	unsigned char *pBuffer;

	pBuffer = ( unsigned char * )m_hLookupTex->Lock( nStride, true );
	for( i = 0; i < 256; i++ )
	{
		float fDegree = 1.0f / nGrade;
		float fGradeValue = ( floor( ( i / 256.0f ) / fDegree ) + 1.0f ) * 256.0f;
		pBuffer[ i * 4 + 2 ] = ( unsigned char )min( 255, fGradeValue * fDegree );
	}
	m_hLookupTex->Unlock();
}
