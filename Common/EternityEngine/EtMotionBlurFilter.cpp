#include "StdAfx.h"
#include "EtMotionBlurFilter.h"
#include "EtBackBufferMng.h"
#include "EtMRTMng.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

#define DEFAULT_MOTION_BLUR_SCALE		0.01f
CEtMotionBlurFilter::CEtMotionBlurFilter()
{
	m_fMotionBlurScale = 0.1f;
}

CEtMotionBlurFilter::~CEtMotionBlurFilter()
{
	Clear();
}

void CEtMotionBlurFilter::Initialize()
{
	Clear();

	m_hMaterial = LoadResource( "MotionBlurFilter.fx", RT_SHADER );
	if( !m_hMaterial ) return;

	AddCustomParam( m_vecCustomParam, EPT_FLOAT_PTR, m_hMaterial, "g_fMotionBlurScale", &m_fMotionBlurScale );

	int nTexIndex = GetEtBackBufferMng()->GetBackBufferIndex();
	AddCustomParam( m_vecCustomParam, EPT_TEX, m_hMaterial, "g_BackBuffer", &nTexIndex );
	nTexIndex = CEtMRTMng::GetInstance().GetVelocityTarget()->GetMyIndex();
	AddCustomParam( m_vecCustomParam, EPT_TEX, m_hMaterial, "g_VelocityTex", &nTexIndex );
}

void CEtMotionBlurFilter::Render( float fElapsedTime )
{
	m_fMotionBlurScale = 0.1666666f / fElapsedTime * DEFAULT_MOTION_BLUR_SCALE;
	CEtPostProcessFilter::Render( fElapsedTime );
}
