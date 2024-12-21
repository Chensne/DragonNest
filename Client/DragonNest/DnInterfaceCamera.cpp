#include "StdAfx.h"
#include "DnInterfaceCamera.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnInterfaceCamera::CDnInterfaceCamera()
{
	m_CameraType = InterfaceCamera;

	m_CameraInfo.fFogNear = 100000.f;
	m_CameraInfo.fFogFar = 100000.f;
	m_CameraInfo.fFar = 100000.f;
}

CDnInterfaceCamera::~CDnInterfaceCamera()
{
}

bool CDnInterfaceCamera::Activate()
{
	m_hCamera = EternityEngine::CreateCamera( &m_CameraInfo );

	return ( m_hCamera ) ? true : false;
}

bool CDnInterfaceCamera::DeActivate()
{
	CDnCamera::DeActivate();
	SAFE_RELEASE_SPTR( m_hCamera );
	return true;
}

void CDnInterfaceCamera::Process( LOCAL_TIME LocalTime, float fDelta )
{
	m_hCamera->Update( m_matExWorld );
}
