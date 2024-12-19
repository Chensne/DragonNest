#include "StdAfx.h"
#include "Task.h"
#include "FrameSync.h"

CTask::CTask(CDNGameRoom * pRoom)
{
	Initialize( pRoom );
	m_dwHandle = -1;
	m_bUseThread = false;
	m_hThreadHandle = NULL;
	m_dwThreadID = 0;
	m_PrevLocalTime = 0;
	m_fDelta = 0.f;

	m_bDestroyTask = false;
	m_bDeleteThis = false;
	m_bEnableTaskProcess = true;

	m_PrevLocalTime = m_LocalTime = 0;
	m_bResetTimer = false;

	m_pFrameSync = new CFrameSync();
}

CTask::~CTask()
{
	SAFE_DELETE( m_pFrameSync );
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

bool CTask::IsPartyLeader( CDNUserSession* pSession )
{
	CDNGameRoom::PartyStruct* pStruct = GetPartyData( pSession );
	if( pStruct && pStruct->bLeader )
		return true;
	return false;
}