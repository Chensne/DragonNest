#include "StdAfx.h"
#include "EtLoader.h"
#include "EtPostProcessMng.h"
#include "EtPostProcessFilter.h"
#include "EtBackBufferMng.h"
#include "EtDrawQuad.h"
#include "EtMRTMng.h"
#include "EtDrawQuad.h"
#include "EtOptionController.h"
#include "EtSaveMat.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CEtPostProcessFilter::CEtPostProcessFilter(void)
{
	m_fDrawScale = 1.0f;
	m_Type = SF_DEFAULT;
	m_bEnable = true;
}

CEtPostProcessFilter::~CEtPostProcessFilter(void)
{
	Clear();
	GetEtPostProcessMng()->RemoveFilter( this );
}

void CEtPostProcessFilter::Clear()
{
	SAFE_RELEASE_SPTR( m_hMaterial );
	m_vecCustomParam.clear();
}

void CEtPostProcessFilter::Render( float fElapsedTime )
{
	DrawFilter( CEtTexture::Identity(), 0 );
}

void CEtPostProcessFilter::DrawFilter( EtTextureHandle hTarget, int nTechnique, EtVector2 vStart, EtVector2 vEnd, bool bClear )
{
	if( hTarget )
	{
		GetEtDevice()->SetRenderTarget( hTarget->GetSurfaceLevel() );		
	}
	else
	{
		GetEtDevice()->RestoreRenderTarget();
	}
	if( bClear ) {
		GetEtDevice()->ClearBuffer( 0x0, 1.0f, 0, true, false, false );
	}

	int nPasses = 0;

	if( !m_hMaterial ) 
		return;
	m_hMaterial->SetTechnique( nTechnique );	
	m_hMaterial->BeginEffect( nPasses );
	m_hMaterial->BeginPass( 0 );
	m_hMaterial->SetCustomParamList( m_vecCustomParam );
	m_hMaterial->CommitChanges();
	GetEtDevice()->SetVertexShader( NULL );	// Use Fixed Pipeline
	DrawQuad( &vStart, &vEnd, &vStart, &vEnd );
	m_hMaterial->EndPass();
	m_hMaterial->EndEffect();
}

EtTextureHandle CEtPostProcessFilter::AddRenderTarget( int nWidth, int nHeight, const char *pParamName, EtFormat Format )
{
	EtTextureHandle hTexture;

	hTexture = CEtTexture::CreateRenderTargetTexture( nWidth, nHeight, Format );
	int nTexIndex = hTexture->GetMyIndex();
	AddCustomParam( m_vecCustomParam, EPT_TEX, m_hMaterial, pParamName, &nTexIndex );

	return hTexture;
}

/*
CEtOilPaintingFilter::CEtOilPaintingFilter()
{
	m_Type = SF_OILPAINTING;
	m_nBrushRadius = 2;
}

CEtOilPaintingFilter::~CEtOilPaintingFilter()
{
	SAFE_RELEASE_SPTR( m_hCopyTarget );
	SAFE_RELEASE_SPTR( m_hOilPaintingTarget );
}

void CEtOilPaintingFilter::Initialize()
{
	m_hCopyTarget = CEtTexture::CreateNormalTexture( -1, -1, FMT_X8B8G8R8, USAGE_DEFAULT, POOL_SYSTEMMEM );
	m_hOilPaintingTarget = CEtTexture::CreateNormalTexture( -1, -1, FMT_A8B8G8R8, USAGE_DEFAULT, POOL_MANAGED );
}

bool g_bEnable = false;
void CEtOilPaintingFilter::Render( float fElapsedTime )
{
	if( g_bEnable == false )
	{
		return;
	}
	EtTextureHandle hBackBuffer = GetEtBackBufferMng()->GetBackBuffer();
	int nStride, nOilPaintStride;

	HRESULT hResult = ( ( IDirect3DDevice9 * )GetEtDevice()->GetDevicePtr() )->GetRenderTargetData( hBackBuffer->GetSurfaceLevel(), m_hCopyTarget->GetSurfaceLevel() );
	unsigned char *pBackBuffer = ( unsigned char * )m_hCopyTarget->Lock( nStride );
	unsigned char *pOilPaintingBuffer = ( unsigned char * )m_hOilPaintingTarget->Lock( nOilPaintStride );

	int nX, nY, i, j;
	int nWidth = hBackBuffer->Width();
	int nHeight = hBackBuffer->Height();
	for( nY = 0; nY < nHeight; nY++ )
	{
		for( nX = 0; nX < nWidth; nX++ )
		{
			int anRed[ 256 ], anGreen[ 256 ], anBlue[ 256 ], anIntensity[ 256 ];
			memset( anRed, 0, sizeof( int ) * 256 );
			memset( anGreen, 0, sizeof( int ) * 256 );
			memset( anBlue, 0, sizeof( int ) * 256 );
			memset( anIntensity, 0, sizeof( int ) * 256 );
			for( i = -m_nBrushRadius; i <= m_nBrushRadius; i++ )
			{
				if( nY + i < 0 )
				{
					continue;
				}
				if( nY + i >= nHeight )
				{
					break;
				}
				for( j = -m_nBrushRadius; j <= m_nBrushRadius; j++ )
				{
					if( nX + j < 0 )
					{
						continue;
					}
					if( nX + j >= nWidth )
					{
						break;
					}
					unsigned char *pOffset = ( unsigned char * )pBackBuffer + nStride * ( nY + i ) + ( nX + j ) * sizeof( DWORD );
					unsigned char cIntensity = ( unsigned char )( 0.2125f * pOffset[ 2 ] + 0.7154f * pOffset[ 1 ] + 0.0721f * pOffset[ 0 ] );
					anIntensity[ cIntensity ]++;
					anRed[ cIntensity ] += pOffset[ 2 ];
					anGreen[ cIntensity ] += pOffset[ 1 ];
					anBlue[ cIntensity ] += pOffset[ 0 ];
				}
			}
			int nMaxIntensity = 0;
			int nMaxIndex = -1;
			for( i = 0; i < 256; i++ )
			{
				if( anIntensity[ i ] > nMaxIntensity )
				{
					nMaxIntensity = anIntensity[ i ];
					nMaxIndex = i;
				}
			}
			if( nMaxIndex != -1 )
			{
				unsigned char *pOilPaintingOffset = pOilPaintingBuffer + nStride * nY + nX * sizeof( DWORD );
				pOilPaintingOffset[ 3 ] = 0xff;
				pOilPaintingOffset[ 2 ] = anRed[ nMaxIndex ] / anIntensity[ nMaxIndex ];
				pOilPaintingOffset[ 1 ] = anGreen[ nMaxIndex ] / anIntensity[ nMaxIndex ];
				pOilPaintingOffset[ 0 ] = anBlue[ nMaxIndex ] / anIntensity[ nMaxIndex ];
			}
		}
	}

	m_hCopyTarget->Unlock();
	m_hOilPaintingTarget->Unlock();

	DrawQuadWithTex( &EtVector2( 0.0f, 0.0f ), &EtVector2( 1.0f, 1.0f ), &EtVector2( 0.0f, 0.0f ), &EtVector2( 1.0f, 1.0f ), m_hOilPaintingTarget->GetTexturePtr() );
}
*/

