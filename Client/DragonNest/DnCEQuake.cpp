#include "StdAfx.h"
#include "DnCEQuake.h"
#include "EtMatrixEx.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


CDnCEQuake::CDnCEQuake( DnCameraHandle hCamera, DWORD dwFrame, float fStartRatio, float fEndRatio, float fDelta, bool bDefaultEffect )
: CDnCameraEffectBase( hCamera, bDefaultEffect )
{
	m_StartTime = hCamera->GetLocalTime();
	m_dwFrame = dwFrame;
	m_fStartRatio = fStartRatio;
	m_fEndRatio = fEndRatio;
	m_fDelta = fDelta;
	m_bDestroy = false;
}

CDnCEQuake::~CDnCEQuake()
{
}

void CDnCEQuake::Process( LOCAL_TIME LocalTime, float fDelta )
{
	if( m_StartTime == 0 ) m_StartTime = LocalTime;
	if( LocalTime > m_StartTime + m_dwFrame ) {
		m_bDestroy = true;
		return;
	}

	float fQuakeDelta = 0.f;
	if( LocalTime < m_StartTime + ( m_dwFrame * m_fStartRatio ) ) {
		fQuakeDelta = 1.f / ( m_dwFrame * m_fStartRatio ) * ( LocalTime - m_StartTime );
		fQuakeDelta *= m_fDelta;
	}
	else if( LocalTime < m_StartTime + ( m_dwFrame * m_fEndRatio ) ) {
		fQuakeDelta = m_fDelta;
	}
	else {
		fQuakeDelta = 1.f / ( m_dwFrame * m_fEndRatio ) * ( ( m_StartTime + m_dwFrame ) - LocalTime );
		fQuakeDelta *= m_fDelta;
	}

	if( fQuakeDelta > 0.1f ) {
		MatrixEx *pCross = m_hCamera->GetMatEx();
		pCross->m_vPosition.x += -fQuakeDelta + ( ( _rand()%(int)(fQuakeDelta*20) ) / 10.f );
		pCross->m_vPosition.y += -fQuakeDelta + ( ( _rand()%(int)(fQuakeDelta*20) ) / 10.f );
		pCross->m_vPosition.z += -fQuakeDelta + ( ( _rand()%(int)(fQuakeDelta*20) ) / 10.f );
	}

}

bool CDnCEQuake::IsDestroy()
{
	return m_bDestroy;
}
