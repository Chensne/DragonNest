#include "StdAfx.h"
#include "DnCERadialBlur.h"
#include "GameOption.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

#ifndef _FINAL_BUILD
extern bool g_bRenderBlur;
#endif

CDnCERadialBlur::CDnCERadialBlur( DnCameraHandle hCamera, DWORD dwFrame, EtVector2 vBlurCenter, float fBlurSize, float fBeginRatio, float fEndRatio, bool bDefaultEffect )
: CDnCameraEffectBase( hCamera, bDefaultEffect )
{
	m_StartTime = hCamera->GetLocalTime();
	m_dwFrame = dwFrame;
	m_vBlurCenter = vBlurCenter;
	m_fBlurSize = fBlurSize;
	m_fBeginRatio = fBeginRatio;
	m_fEndRatio = fEndRatio;
	m_bDestroy = false;
	m_pRadialBlur = NULL;

	if( CGameOption::GetInstance().GetCurGraphicQuality() < 2 )
	{
		m_pRadialBlur = ( CEtRadialBlurFilter * )EternityEngine::CreateFilter( SF_RADIALBLUR );
		m_pRadialBlur->SetBlurCenter( m_vBlurCenter.x, m_vBlurCenter.y );
	}
}

CDnCERadialBlur::~CDnCERadialBlur()
{
	SAFE_DELETE( m_pRadialBlur );
}

void CDnCERadialBlur::Process( LOCAL_TIME LocalTime, float fDelta )
{
	if( !m_pRadialBlur )
	{
		return;
	}
#ifndef _FINAL_BUILD
	if( !g_bRenderBlur )
	{
		m_bDestroy = true;
		return;
	}
#endif

	if( m_StartTime == 0 ) m_StartTime = LocalTime;
	if( m_dwFrame != -1 && LocalTime > m_StartTime + m_dwFrame ) {
		m_bDestroy = true;
		return;
	}

	float fValue = 0.f;
	if( LocalTime < m_StartTime + ( m_dwFrame * m_fBeginRatio ) ) {
		fValue = 1.f / ( m_dwFrame * m_fBeginRatio ) * ( LocalTime - m_StartTime );
		fValue *= m_fBlurSize;
	}
	else if( LocalTime < m_StartTime + ( m_dwFrame * m_fEndRatio ) ) {
		fValue = m_fBlurSize;
	}
	else {
		fValue = 1.f / ( m_dwFrame * m_fEndRatio ) * ( ( m_StartTime + m_dwFrame ) - LocalTime );
		fValue *= m_fBlurSize;
	}

	m_pRadialBlur->SetBlurSize( fValue );
}

bool CDnCERadialBlur::IsDestroy()
{
	return m_bDestroy;
}
