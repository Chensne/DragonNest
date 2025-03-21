#include "StdAfx.h"
#include "EtColorAdjustFilter.h"
#include "EtBackBufferMng.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CEtColorAdjustFilter::CEtColorAdjustFilter()
{
	m_Type = SF_COLORADJUST;
	m_fSceneSaturation = 0.0f;
}

CEtColorAdjustFilter::~CEtColorAdjustFilter()
{
	Clear();
}

void CEtColorAdjustFilter::SetSceneSaturation( float fSaturation )
{
	float fDesaturation;

	fDesaturation = 1.0f - fSaturation;
	m_SceneLuminanceWeight.x = 0.30000001f * fDesaturation;
	m_SceneLuminanceWeight.y = 0.58999997f * fDesaturation;
	m_SceneLuminanceWeight.z = 0.11000000f * fDesaturation;
	m_fSceneSaturation = fSaturation;
}

void CEtColorAdjustFilter::Initialize()
{
	Clear();

	m_hMaterial = LoadResource( "ColorAdjustFilter2.fx", RT_SHADER );
	if( !m_hMaterial ) return;

	m_SceneShadow = EtVector4( 0.0f, 0.0f, 0.0f, 0.0f );
	m_SceneMidTone = EtVector4( 0.0f, 0.0f, 0.0f, 0.0f );
	m_SceneHilight = EtVector4( 0.0f, 0.0f, 0.0f, 1.0f );
	SetSceneSaturation( 1.0f );

	int nBackBufferIndex = GetEtBackBufferMng()->GetBackBufferIndex();
	AddCustomParam( m_vecCustomParam, EPT_TEX, m_hMaterial, "g_BackBuffer", &nBackBufferIndex );
	AddCustomParam( m_vecCustomParam, EPT_VECTOR_PTR, m_hMaterial, "g_fSceneShadow", &m_SceneShadow );
	AddCustomParam( m_vecCustomParam, EPT_VECTOR_PTR, m_hMaterial, "g_fSceneMidTone", &m_SceneMidTone );
	AddCustomParam( m_vecCustomParam, EPT_VECTOR_PTR, m_hMaterial, "g_fSceneHilight", &m_SceneHilight );
	AddCustomParam( m_vecCustomParam, EPT_VECTOR_PTR, m_hMaterial, "g_fSceneLuminanceWeight", &m_SceneLuminanceWeight );
	AddCustomParam( m_vecCustomParam, EPT_FLOAT_PTR, m_hMaterial, "g_fSceneSaturation", &m_fSceneSaturation );

	AddCustomParam( m_vecCustomParam, EPT_VECTOR_PTR, m_hMaterial, "g_fHilightMinParam", &m_HilightMin );
	AddCustomParam( m_vecCustomParam, EPT_VECTOR_PTR, m_hMaterial, "g_fHilightMulParam", &m_HilightMul );
	AddCustomParam( m_vecCustomParam, EPT_VECTOR_PTR, m_hMaterial, "g_fHilightCenterParam", &m_HilightCenter );
	AddCustomParam( m_vecCustomParam, EPT_VECTOR_PTR, m_hMaterial, "g_fHilightMulAddParam", &m_HilightMulAdd );

	AddCustomParam( m_vecCustomParam, EPT_VECTOR_PTR, m_hMaterial, "g_fMidToneMinParam", &m_MidToneMin );
	AddCustomParam( m_vecCustomParam, EPT_VECTOR_PTR, m_hMaterial, "g_fMidToneMulParam", &m_MidToneMul );
	AddCustomParam( m_vecCustomParam, EPT_VECTOR_PTR, m_hMaterial, "g_fMidToneCenterParam", &m_MidToneCenter );
	AddCustomParam( m_vecCustomParam, EPT_VECTOR_PTR, m_hMaterial, "g_fMidToneMulAddParam", &m_MidToneMulAdd );

	AddCustomParam( m_vecCustomParam, EPT_VECTOR_PTR, m_hMaterial, "g_fShadowMinParam", &m_ShadowMin );
	AddCustomParam( m_vecCustomParam, EPT_VECTOR_PTR, m_hMaterial, "g_fShadowMulParam", &m_ShadowMul );
	AddCustomParam( m_vecCustomParam, EPT_VECTOR_PTR, m_hMaterial, "g_fShadowCenterParam", &m_ShadowCenter );
	AddCustomParam( m_vecCustomParam, EPT_VECTOR_PTR, m_hMaterial, "g_fShadowMulAddParam", &m_ShadowMulAdd );
}

void CEtColorAdjustFilter::Render( float fElapsedTime )
{
	int i;

	for( i = 0; i < 3; i++ )
	{
		if( m_SceneHilight[ i ] >= 0.0f )
		{
			m_HilightMin[ i ] = HILIGHT_MIN_PLUS;
			m_HilightMul[ i ] = HILIGHT_MUL_PLUS;
			m_HilightCenter[ i ] = HILIGHT_CENTER_PLUS;
			m_HilightMulAdd[ i ] = HILIGHT_MULADD_PLUS;
		}
		else
		{
			m_HilightMin[ i ] = HILIGHT_MIN_MINUS;
			m_HilightMul[ i ] = HILIGHT_MUL_MINUS;
			m_HilightCenter[ i ] = HILIGHT_CENTER_MINUS;
			m_HilightMulAdd[ i ] = HILIGHT_MULADD_MINUS;
		}
		if( m_SceneMidTone[ i ] >= 0.0f )
		{
			m_MidToneMin[ i ] = MIDTONE_MIN_PLUS;
			m_MidToneMul[ i ] = MIDTONE_MUL_PLUS;
			m_MidToneCenter[ i ] = MIDTONE_CENTER_PLUS;
			m_MidToneMulAdd[ i ] = MIDTONE_MULADD_PLUS;
		}
		else
		{
			m_MidToneMin[ i ] = MIDTONE_MIN_MINUS;
			m_MidToneMul[ i ] = MIDTONE_MUL_MINUS;
			m_MidToneCenter[ i ] = MIDTONE_CENTER_MINUS;
			m_MidToneMulAdd[ i ] = MIDTONE_MULADD_MINUS;
		}
		if( m_SceneShadow[ i ] >= 0.0f )
		{
			m_ShadowMin[ i ] = SHADOW_MIN_PLUS;
			m_ShadowMul[ i ] = SHADOW_MUL_PLUS;
			m_ShadowCenter[ i ] = SHADOW_CENTER_PLUS;
			m_ShadowMulAdd[ i ] = SHADOW_MULADD_PLUS;
		}
		else
		{
			m_ShadowMin[ i ] = SHADOW_MIN_MINUS;
			m_ShadowMul[ i ] = SHADOW_MUL_MINUS;
			m_ShadowCenter[ i ] = SHADOW_CENTER_MINUS;
			m_ShadowMulAdd[ i ] = SHADOW_MULADD_MINUS;
		}
	}

	CEtPostProcessFilter::Render( fElapsedTime );
}