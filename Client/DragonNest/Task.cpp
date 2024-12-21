#include "StdAfx.h"
#include "Task.h"
#include "FrameSync.h"

#if !defined(USE_BOOST_MEMPOOL)
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif
#endif

CTask::CTask()
{
	m_dwHandle = -1;
	m_bUseThread = false;
	//	m_pFrameSync = NULL;
	m_hThreadHandle = INVALID_HANDLE_VALUE;
	m_dwThreadID = 0;
	m_PrevLocalTime = 0;
	m_fDelta = 0.f;

	m_bDestroyTask = false;
	m_bDeleteThis = false;
	m_bEnableTaskProcess = true;

	m_PrevLocalTime = m_LocalTime = 0;
	m_bResetTimer = false;

	m_pFrameSync = new CFrameSync();

	m_hEndRequest = INVALID_HANDLE_VALUE;
}

CTask::~CTask()
{
	SAFE_DELETE( m_pFrameSync );
	SAFE_CLOSE_HANDLE( m_hEndRequest );
	SAFE_CLOSE_HANDLE( m_hThreadHandle );
}
void CTask::ResetTimer()
{
	m_bResetTimer = true;
	m_PrevLocalTime = m_LocalTime = 0;
	if( m_pFrameSync )
		m_pFrameSync->Reset();
}

void CTask::SetLocalTimeSpeed( float fSpeed, DWORD dwDelay )
{
	m_pFrameSync->SetTimeSpeed( fSpeed, dwDelay );
}

void CTask::EnableTaskProcess( bool bEnable ) 
{ 
	if( m_bEnableTaskProcess == bEnable ) return;
	m_bEnableTaskProcess = bEnable; 

	if( !m_pFrameSync ) return;

	if( m_bEnableTaskProcess ) { 
		m_pFrameSync->UpdateTime();
		LOCAL_TIME LocalTime = m_pFrameSync->GetMSTime();
		SetPrevLocalTime( LocalTime );
	}
}