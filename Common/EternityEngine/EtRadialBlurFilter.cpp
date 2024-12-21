#include "StdAfx.h"
#include "EtRadialBlurFilter.h"
#include "EtBackBufferMng.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CEtRadialBlurFilter::CEtRadialBlurFilter()
{
	m_fBlurSize = 0.2f;
	m_vBlurCenter = EtVector4( 0.5f, 0.5f, 0.0f, 0.0f );
}

CEtRadialBlurFilter::~CEtRadialBlurFilter()
{
	Clear();
}

void CEtRadialBlurFilter::Initialize()
{
	Clear();

	m_hMaterial = LoadResource( "RadialBlurFilter.fx", RT_SHADER );
	if( !m_hMaterial ) return;

	AddCustomParam( m_vecCustomParam, EPT_VECTOR_PTR, m_hMaterial, "g_BlurCenter", &m_vBlurCenter );
	AddCustomParam( m_vecCustomParam, EPT_FLOAT_PTR, m_hMaterial, "g_fBlurSize", &m_fBlurSize );

	int nTexIndex = GetEtBackBufferMng()->GetBackBufferIndex();
	AddCustomParam( m_vecCustomParam, EPT_TEX, m_hMaterial, "g_BackBuffer", &nTexIndex );
}