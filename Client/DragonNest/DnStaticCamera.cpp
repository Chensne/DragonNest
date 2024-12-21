#include "StdAfx.h"
#include "DnStaticCamera.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnStaticCamera::CDnStaticCamera()
{
	m_CameraType = CameraTypeEnum::FreeCamera;

	m_CameraInfo.fFogNear = 1000000.f;
	m_CameraInfo.fFogFar = 1000000.f;
	m_CameraInfo.fFar = 1000000.f;
}

bool CDnStaticCamera::Activate()
{
	m_hCamera = EternityEngine::CreateCamera( &m_CameraInfo );
	m_hCamera->Activate();

	return ( m_hCamera ) ? true : false;
}

bool CDnStaticCamera::DeActivate()
{
	CDnCamera::DeActivate();
	SAFE_RELEASE_SPTR( m_hCamera );
	return true;
}

void CDnStaticCamera::Process( LOCAL_TIME LocalTime, float fDelta )
{
	m_hCamera->Update( m_matExWorld );
}