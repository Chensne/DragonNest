#include "StdAfx.h"
#include "EtBackBufferMng.h"
#include "EtMRTMng.h"
#include "EtDrawQuad.h"
#include "EtLightShaftFilter.h"

CEtLightShaftFilter::CEtLightShaftFilter()
{
	m_vSunPosition = EtVector2( 0.5f, 0.5f );
}

CEtLightShaftFilter::~CEtLightShaftFilter()
{
	SAFE_RELEASE_SPTR( m_hLightShaftSour );
	SAFE_RELEASE_SPTR( m_hLightShaftDest );
}

void CEtLightShaftFilter::Initialize()
{
	Clear();

	m_hMaterial = LoadResource( "LightShaftFilter.fx", RT_SHADER );
	if( !m_hMaterial ) return;
	m_hLightShaftSour = AddRenderTarget( -4, -4, "g_LightShaftSourBuffer" );
	m_hLightShaftDest = AddRenderTarget( -4, -4, "g_LightShaftDestBuffer" );

	int nTexIndex = GetEtBackBufferMng()->GetBackBufferIndex();
	AddCustomParam( m_vecCustomParam, EPT_TEX, m_hMaterial, "g_BackBuffer", &nTexIndex );
	nTexIndex = CEtMRTMng::GetInstance().GetDepthTarget()->GetMyIndex();
	AddCustomParam( m_vecCustomParam, EPT_TEX, m_hMaterial, "g_DepthTex", &nTexIndex );
}

void CEtLightShaftFilter::Render( float fElapsedTime )
{
	CEtPostProcessFilter::DrawFilter( m_hLightShaftSour, 2, EtVector2( 0.0f, 0.0f ), EtVector2( 1.0f, 1.0f ), true );
	int i;
	float fSize = 0.4f;
	for( i = 0; i < 6; i++ )
	{
		EtVector2 vStart, vEnd, vOffset;

		vStart.x = -fSize;
		vStart.y = -fSize;
		vEnd.x = fSize + 1.0f;
		vEnd.y = fSize + 1.0f;
		vOffset = EtVector2( 0.5f, 0.5f ) - m_vSunPosition;
		vOffset *= fSize;
		vStart += vOffset;
		vEnd += vOffset;
		fSize *= 0.5f;

		CEtPostProcessFilter::DrawFilter( m_hLightShaftDest, 0 );
		bool bEnableAlphaBlend = GetEtDevice()->EnableAlphaBlend( true );
		DrawFilter( m_hLightShaftSour, 1, vStart, vEnd, EtVector2( 0.0f, 0.0f ), EtVector2( 1.0f, 1.0f ) );
		GetEtDevice()->EnableAlphaBlend( bEnableAlphaBlend );
	}
	bool bEnableAlphaBlend = GetEtDevice()->EnableAlphaBlend( true );
	CEtPostProcessFilter::DrawFilter( CEtTexture::Identity(), 3 );
	GetEtDevice()->EnableAlphaBlend( bEnableAlphaBlend );
}

void CEtLightShaftFilter::DrawFilter( EtTextureHandle hTarget, int nTechnique, EtVector2 vStart, EtVector2 vEnd, EtVector2 vStartUV, EtVector2 vEndUV )
{
	if( hTarget )
	{
		GetEtDevice()->SetRenderTarget( hTarget->GetSurfaceLevel() );		
	}
	else
	{
		GetEtDevice()->RestoreRenderTarget();
	}

	int nPasses = 0;
	if( !m_hMaterial ) 
		return;
	m_hMaterial->SetTechnique( nTechnique );	
	m_hMaterial->BeginEffect( nPasses );
	m_hMaterial->BeginPass( 0 );
	m_hMaterial->SetCustomParamList( m_vecCustomParam );
	m_hMaterial->CommitChanges();
	GetEtDevice()->SetVertexShader( NULL );
	DrawQuad( &vStart, &vEnd, &vStartUV, &vEndUV );
	m_hMaterial->EndPass();
	m_hMaterial->EndEffect();
}
