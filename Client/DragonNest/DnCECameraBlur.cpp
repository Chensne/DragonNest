#include "StdAfx.h"
#include "DnCECameraBlur.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnCECameraBlur::CDnCECameraBlur( DnCameraHandle hCamera )
: CDnCameraEffectBase( hCamera, true )
{
	m_StartTime = hCamera->GetLocalTime();
	m_dwFrame = -1;
	m_fBlendFactor = 0.8f;
	m_fBeginRatio = 0.1f;
	m_fEndRatio = 0.3f;
	m_bDestroy = false;
	m_pCameraBlur = NULL;

	m_pCameraBlur = ( CEtCameraBlurFilter * )EternityEngine::CreateFilter( SF_CAMERABLUR );
	m_pCameraBlur->Enable( false );
}

CDnCECameraBlur::~CDnCECameraBlur()
{
	SAFE_DELETE( m_pCameraBlur );
}

void CDnCECameraBlur::Process( LOCAL_TIME LocalTime, float fDelta )
{
	if( !m_pCameraBlur )
		return;

	if( !m_pCameraBlur->IsEnable() )
		return;

	if( m_StartTime == 0 ) m_StartTime = LocalTime;
	if( m_dwFrame != -1 && LocalTime > m_StartTime + m_dwFrame ) {
		m_pCameraBlur->Enable( false );
		return;
	}

	float fValue = 0.f;
	if( LocalTime < m_StartTime + ( m_dwFrame * m_fBeginRatio ) ) {
		fValue = ( LocalTime - m_StartTime ) / ( m_dwFrame * m_fBeginRatio );
		fValue *= m_fBlendFactor;
	}
	else if( LocalTime < m_StartTime + m_dwFrame - ( m_dwFrame * m_fEndRatio ) ) {
		fValue = m_fBlendFactor;
	}
	else {
		fValue = ( ( m_StartTime + m_dwFrame ) - LocalTime ) / ( m_dwFrame * m_fEndRatio );
		fValue *= m_fBlendFactor;
	}

	m_pCameraBlur->SetBlendFactor( fValue );
}

bool CDnCECameraBlur::IsDestroy()
{
	return m_bDestroy;
}

void CDnCECameraBlur::SetBlur( LOCAL_TIME StartTime, DWORD dwFrame, float fBlendFactor )
{
	if( !m_pCameraBlur ) return;
	m_StartTime = StartTime;
	m_dwFrame = dwFrame;
	m_fBlendFactor = fBlendFactor;
	m_pCameraBlur->SetBlendFactor( m_fBlendFactor );
	m_pCameraBlur->Enable( true );
}