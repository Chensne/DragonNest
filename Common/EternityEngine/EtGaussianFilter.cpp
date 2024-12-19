#include "StdAfx.h"
#include "EtGaussianFilter.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

std::vector< CEtGaussianFilter::STempRenderTarget > CEtGaussianFilter::s_vecTempRenderTarget;

CEtGaussianFilter::CEtGaussianFilter()
{	
	m_nTexParamIndex = -1;
	m_nBlurRadiusIndex = 0;
}

CEtGaussianFilter::~CEtGaussianFilter()
{
	Clear();
}

void CEtGaussianFilter::Clear()
{
	SAFE_RELEASE_SPTR( m_hCurRenderTarget );
	CEtPostProcessFilter::Clear();
}

void CEtGaussianFilter::Initialize()
{
	Clear();

	m_hMaterial = LoadResource( "GaussianFilter.fx", RT_SHADER );
	if( !m_hMaterial ) return;

	int nTexIndex = -1;
	m_nTexParamIndex = AddCustomParam( m_vecCustomParam, EPT_TEX, m_hMaterial, "g_GaussianSource", &nTexIndex );
	m_nBlurRadiusIndex = AddCustomParam( m_vecCustomParam, EPT_VECTOR_PTR, m_hMaterial, "g_fBlurRadius", &m_vBlurRadius );
}

void CEtGaussianFilter::CreateRenderTarget( int nWidth, int nHeight )
{
	int i;
	SAFE_RELEASE_SPTR( m_hCurRenderTarget );

	for( i = 0; i < ( int )s_vecTempRenderTarget.size(); i++ )
	{
		if( ( s_vecTempRenderTarget[ i ].nWidth == nWidth ) && ( s_vecTempRenderTarget[ i ].nHeight == nHeight ) )
		{
			m_hCurRenderTarget = s_vecTempRenderTarget[ i ].hRenderTarget;
			m_hCurRenderTarget->AddRef();
			return;
		}
	}

	EtTextureHandle hTexture;
	STempRenderTarget TempRenderTarget;
	hTexture = CEtTexture::CreateRenderTargetTexture( nWidth, nHeight );
	TempRenderTarget.nWidth = nWidth;
	TempRenderTarget.nHeight = nHeight;
	TempRenderTarget.hRenderTarget = hTexture;
	s_vecTempRenderTarget.push_back( TempRenderTarget );
	m_hCurRenderTarget = hTexture;
}

void CEtGaussianFilter::RenderGaussianFilter( CEtTexture *pSourTex, CEtTexture *pTargetTex, float fBlurSize )
{
	if( !m_hCurRenderTarget )
	{
		CreateRenderTarget( pTargetTex->Width(), pTargetTex->Height() );
	}

	m_vBlurRadius.x = fBlurSize / 1024.0f;
	m_vBlurRadius.y = m_vBlurRadius.x * ( GetEtDevice()->Width() / ( float )GetEtDevice()->Height() );

	m_vecCustomParam[ m_nTexParamIndex ].nTextureIndex = pSourTex->GetMyIndex();
	DrawFilter( m_hCurRenderTarget, 0 );
	m_vecCustomParam[ m_nTexParamIndex ].nTextureIndex = m_hCurRenderTarget->GetMyIndex();
	DrawFilter( pTargetTex->GetMySmartPtr(),  1 );
}
