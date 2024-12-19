#include "StdAfx.h"
#include "EtD3DDevice9.h"
#include "EtDepth.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CEtDepth::CEtDepth(void)
{
	m_MultiSampleType = MULTISAMPLE_NONE;
}

CEtDepth::~CEtDepth(void)
{
	SAFE_RELEASE( m_pSurface );
}

void CEtDepth::CreateDepthBuffer( int nWidth, int nHeight, EtFormat Format, EtMultiSampleType MultiSampleType )
{
	m_nWidth = nWidth;
	m_nHeight = nHeight;
	m_Format = Format;
	m_pSurface = GetEtDevice()->CreateDepthStencil( nWidth, nHeight, Format, MultiSampleType );
	m_MultiSampleType = MultiSampleType;
	if( !m_pSurface && MultiSampleType != MULTISAMPLE_NONE ) {	// 안티알리아싱 실패하는 경우...
		m_pSurface = GetEtDevice()->CreateDepthStencil( nWidth, nHeight, Format, MULTISAMPLE_NONE );
		m_MultiSampleType = MULTISAMPLE_NONE;
	}
	SetDeleteImmediate( true );
}

void CEtDepth::OnLostDevice()
{
	SAFE_RELEASE( m_pSurface );
}

void CEtDepth::OnResetDevice()
{
	CreateDepthBuffer( m_nWidth, m_nHeight, m_Format, m_MultiSampleType );
}

EtDepthHandle CEtDepth::CreateDepthStencil( int nWidth, int nHeight, EtFormat Format, EtMultiSampleType MultiSampleType )
{
	CEtDepth *pDepth;

	pDepth = new CEtDepth();
	pDepth->CreateDepthBuffer( nWidth, nHeight, Format, MultiSampleType );
	pDepth->AddRef();

	return pDepth->GetMySmartPtr();
}
