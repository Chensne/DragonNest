#include "StdAfx.h"
#include "EtBackBufferMng.h"
#include "EtPostProcessFilter.h"
#include "EtPostProcessMng.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CEtPostProcessMng g_EtPostProcessMng;

bool FilterSortFunc( CEtPostProcessFilter *pFilter1, CEtPostProcessFilter *pFilter2 )
{
	if( pFilter1->GetType() < pFilter2->GetType() )
	{
		return true;
	}

	return false;
}

CEtPostProcessMng::CEtPostProcessMng(void)
{
}

CEtPostProcessMng::~CEtPostProcessMng(void)
{
	Clear();
}

void CEtPostProcessMng::Clear()
{
	SAFE_DELETE_PVEC( m_vecFilter );
}

void CEtPostProcessMng::Initialize()
{
	Clear();
	GetEtBackBufferMng()->CreateBackBuffer();
}

void CEtPostProcessMng::Render( float fElapsedTime )
{
	if( m_vecFilter.empty() ) {
		return;
	}

	int i, nSize;

	nSize = ( int )m_vecFilter.size();
	GetEtDevice()->EnableZWrite( false );		
	for( i = 0; i < nSize; i++ )
	{
		if( !m_vecFilter[ i ]->IsEnable() )
		{
			continue;
		}
		GetEtBackBufferMng()->DumpBackBuffer();
		m_vecFilter[ i ]->Render( fElapsedTime );
	}
	GetEtDevice()->EnableZWrite( true );
	GetEtDevice()->RestoreRenderTarget();
}

CEtPostProcessFilter *CEtPostProcessMng::CreateFilter( ScreenFilter Filter )
{
	CEtPostProcessFilter *pFilter;

	switch( Filter )
	{
		case SF_LIGHTSHAFT:
			pFilter = new CEtLightShaftFilter();
			break;
		case SF_BLOOM:
			pFilter = new CEtBloomFilter();
			break;
		case SF_WATERCOLOR:
			pFilter = new CEtWaterColorFilter();
			break;
		case SF_COLORADJUST:
			pFilter = new CEtColorAdjustFilter();
			break;
		case SF_COLORADJUSTTEX:
			pFilter = new CEtColorAdjustTexFilter();
			break;
		case SF_DOF:
			pFilter = new CEtDOFFilter();
			break;
		case SF_MOTIONBLUR:
			pFilter = new CEtMotionBlurFilter();
			break;
		case SF_RADIALBLUR:
			pFilter = new CEtRadialBlurFilter();
			break;
		case SF_HEATHAZE:
			pFilter = new CEtHeatHazeFilter();
			break;
		case SF_OUTLINE:
			pFilter = new CEtOutlineFilter();
			break;
		case SF_TRANSITION:
			pFilter = new CEtTransitionFilter();
			break;
		case SF_CAMERABLUR:
			pFilter = new CEtCameraBlurFilter();
			break;
		default:
			return NULL;
	}

	pFilter->Initialize();
	m_vecFilter.push_back( pFilter );

	std::sort( m_vecFilter.begin(), m_vecFilter.end(), FilterSortFunc );

	return pFilter;
}

void CEtPostProcessMng::RemoveFilter( CEtPostProcessFilter *pFilter )
{
	for( DWORD i=0; i<m_vecFilter.size(); i++ ) {
		if( m_vecFilter[i] == pFilter ) {
			m_vecFilter.erase( m_vecFilter.begin() + i );
		}
	}
}

CEtPostProcessFilter* CEtPostProcessMng::GetFilter( ScreenFilter Filter )
{
	for( DWORD i=0; i<m_vecFilter.size(); i++ ) {
		if( m_vecFilter[i]->GetType() == Filter ) {
			return m_vecFilter[i];
		}
	}
	return NULL;
}