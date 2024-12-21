#include "StdAfx.h"
#include "EtColorAdjustTexFilter.h"
#include "EtBackBufferMng.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CEtColorAdjustTexFilter::CEtColorAdjustTexFilter()
{
	m_Type = SF_COLORADJUSTTEX;

	m_SceneShadow = EtVector4( 0.0f, 0.0f, 0.0f, 0.0f );
	m_SceneMidTone = EtVector4( 0.0f, 0.0f, 0.0f, 0.0f );
	m_SceneHilight = EtVector4( 0.0f, 0.0f, 0.0f, 1.0f );
	m_fSceneSaturation = 1.0f;


	// PRE_ADD_FILTEREVENT
	m_SceneMonochrome = 0.0f;
	m_SceneAbsoluteColor = EtVector3( 1.0f, 1.0f, 1.0f );
	//


}

CEtColorAdjustTexFilter::~CEtColorAdjustTexFilter()
{
	Clear();
}

void CEtColorAdjustTexFilter::Clear()
{
	SAFE_RELEASE_SPTR( m_hLookupTex );
}

void CEtColorAdjustTexFilter::SetTexture( int nTextureIndex )
{	
	if( !m_hMaterial ) return;
	AddCustomParam( m_vecCustomParam, EPT_TEX, m_hMaterial, "g_BackBuffer", &nTextureIndex );
}

void CEtColorAdjustTexFilter::Initialize()
{
	Clear();

	m_hMaterial = LoadResource( "ColorAdjustTexFilter.fx", RT_SHADER );
	if( !m_hMaterial ) return;

	int nBackBufferIndex = GetEtBackBufferMng()->GetBackBufferIndex();
	SetTexture( nBackBufferIndex );
	AddCustomParam( m_vecCustomParam, EPT_VECTOR_PTR, m_hMaterial, "g_fSceneLuminanceWeight", &m_SceneLuminanceWeight );
	AddCustomParam( m_vecCustomParam, EPT_FLOAT_PTR, m_hMaterial, "g_fSceneSaturation", &m_fSceneSaturation );

	// PRE_ADD_FILTEREVENT.	
	AddCustomParam( m_vecCustomParam, EPT_VECTOR_PTR, m_hMaterial, "g_fAbsoluteColor", &m_SceneAbsoluteColor );
	AddCustomParam( m_vecCustomParam, EPT_FLOAT_PTR, m_hMaterial, "g_iMonochrome", &m_SceneMonochrome );
	//

	m_hLookupTex = CEtTexture::CreateNormalTexture( 256, 1, FMT_A8R8G8B8, USAGE_DEFAULT, POOL_MANAGED );
	int nTexIndex = m_hLookupTex->GetMyIndex();
	AddCustomParam( m_vecCustomParam, EPT_TEX, m_hMaterial, "g_LookupTex", &nTexIndex );
}

void CEtColorAdjustTexFilter::SetSceneSaturation( float fSaturation )
{
	float fDesaturation;

	fDesaturation = 1.0f - fSaturation;
	m_SceneLuminanceWeight.x = 0.30000001f * fDesaturation;
	m_SceneLuminanceWeight.y = 0.58999997f * fDesaturation;
	m_SceneLuminanceWeight.z = 0.11000000f * fDesaturation;
	m_fSceneSaturation = fSaturation;
}

void CEtColorAdjustTexFilter::SetParam( EtVector4 &vShadow, EtVector4 &vMidTone, EtVector4 &vHilight, float fSaturation )
{
#ifdef PRE_FIX_MATERIAL_DUMP
	if( !m_hMaterial ) return;
#endif

	int i, j, nStride;
	char *pBuffer;
	EtVector3 HilightMin, HilightMul, HilightCenter, HilightMulAdd;
	EtVector3 MidToneMin, MidToneMul, MidToneCenter, MidToneMulAdd;
	EtVector3 ShadowMin, ShadowMul, ShadowCenter, ShadowMulAdd;

	m_SceneShadow = vShadow;
	m_SceneMidTone = vMidTone;
	m_SceneHilight = vHilight;
	SetSceneSaturation( fSaturation );

	for( i = 0; i < 3; i++ )
	{
		if( m_SceneHilight[ i ] >= 0.0f )
		{
			HilightMin[ i ] = HILIGHT_MIN_PLUS;
			HilightMul[ i ] = HILIGHT_MUL_PLUS;
			HilightCenter[ i ] = HILIGHT_CENTER_PLUS;
			HilightMulAdd[ i ] = HILIGHT_MULADD_PLUS;
		}
		else
		{
			HilightMin[ i ] = HILIGHT_MIN_MINUS;
			HilightMul[ i ] = HILIGHT_MUL_MINUS;
			HilightCenter[ i ] = HILIGHT_CENTER_MINUS;
			HilightMulAdd[ i ] = HILIGHT_MULADD_MINUS;
		}
		if( m_SceneMidTone[ i ] >= 0.0f )
		{
			MidToneMin[ i ] = MIDTONE_MIN_PLUS;
			MidToneMul[ i ] = MIDTONE_MUL_PLUS;
			MidToneCenter[ i ] = MIDTONE_CENTER_PLUS;
			MidToneMulAdd[ i ] = MIDTONE_MULADD_PLUS;
		}
		else
		{
			MidToneMin[ i ] = MIDTONE_MIN_MINUS;
			MidToneMul[ i ] = MIDTONE_MUL_MINUS;
			MidToneCenter[ i ] = MIDTONE_CENTER_MINUS;
			MidToneMulAdd[ i ] = MIDTONE_MULADD_MINUS;
		}
		if( m_SceneShadow[ i ] >= 0.0f )
		{
			ShadowMin[ i ] = SHADOW_MIN_PLUS;
			ShadowMul[ i ] = SHADOW_MUL_PLUS;
			ShadowCenter[ i ] = SHADOW_CENTER_PLUS;
			ShadowMulAdd[ i ] = SHADOW_MULADD_PLUS;
		}
		else
		{
			ShadowMin[ i ] = SHADOW_MIN_MINUS;
			ShadowMul[ i ] = SHADOW_MUL_MINUS;
			ShadowCenter[ i ] = SHADOW_CENTER_MINUS;
			ShadowMulAdd[ i ] = SHADOW_MULADD_MINUS;
		}
	}
	pBuffer = ( char * )m_hLookupTex->Lock( nStride, true );
	for( i = 0; i < 256; i++ )
	{
		float fOrigColor = i / 255.0f;
		EtVector3 vHilightInner, vMidToneInner, vShadowInner;
		for( j = 0; j < 3; j++ )
		{
			vHilightInner[ j ] = abs( fOrigColor - HilightCenter[ j ] ) * HilightMul[ j ] + HilightMin[ j ] + max( 0, fOrigColor - HilightCenter[ j ] ) * HilightMulAdd[ j ];
			vMidToneInner[ j ] = abs( fOrigColor - MidToneCenter[ j ] ) * MidToneMul[ j ] + MidToneMin[ j ] + max( 0, fOrigColor - MidToneCenter[ j ] ) * MidToneMulAdd[ j ];
			vShadowInner[ j ] = abs( fOrigColor - ShadowCenter[ j ] ) * ShadowMul[ j ] + ShadowMin[ j ] + max( 0, fOrigColor - ShadowCenter[ j ] ) * ShadowMulAdd[ j ];

			float fFinalColor = fOrigColor + vHilightInner[ j ] * m_SceneHilight[ j ] + vMidToneInner[ j ] * m_SceneMidTone[ j ] + vShadowInner[ j ] * m_SceneShadow[ j ];
			fFinalColor = min( max( fFinalColor, 0.0f ), 1.0f );
			pBuffer[ i * 4 + 2 - j ] = ( unsigned char )( fFinalColor * 255 );
		}
	}
	m_hLookupTex->Unlock();
}


// PRE_ADD_FILTEREVENT
void CEtColorAdjustTexFilter::SetSceneAbsoluteColor( bool bMonochrome, EtVector3 & vColor, float fVolume )
{
	m_SceneMonochrome = bMonochrome ? 1.0f : 0.0f;
	m_SceneAbsoluteColor = vColor * fVolume;
}
//