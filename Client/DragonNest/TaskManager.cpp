#include "StdAfx.h"
#include <process.h>
#include "TaskManager.h"
#include "Task.h"
#include "FrameSync.h"
#include "GlobalValue.h"
#include "DnCommonUtil.h"
#include <mmsystem.h>

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CTaskManager g_TaskManager;
CTaskManager::CTaskManager()
{
	m_bLockDebugTask = false;
	m_nDebugTestLag = -1;
	m_PrevGlobalTime = 0;
}

CTaskManager::~CTaskManager()
{
	for( DWORD i=0; i<m_pVecTaskList.size(); i++ ) {
		RemoveTask( m_pVecTaskList[i]->m_dwHandle );
		--i;
	}
}


DWORD CTaskManager::AddTask( CTask *pTask, const char *szName, int nSecureFrame, bool bUseThread )
{
	static DWORD dwCounter = 0;

	std::vector<CTask*>::iterator it = std::find( m_pVecTaskList.begin(), m_pVecTaskList.end(), pTask );
	if( it != m_pVecTaskList.end() ) {
		_ASSERT(0);
	}

	m_pVecTaskList.push_back( pTask );
	pTask->m_dwHandle = dwCounter;
	++dwCounter;

	pTask->m_bUseThread = bUseThread;

	pTask->ResetTimer();
	pTask->m_pFrameSync->Begin( ( m_bLockDebugTask == true ) ? 60 : nSecureFrame );
	pTask->SetTaskName( szName );

	if( bUseThread == true ) {
		pTask->m_hEndRequest = CreateEvent( NULL, FALSE, FALSE, NULL );
		pTask->m_hThreadHandle = (HANDLE)_beginthreadex( NULL, 65536, BeginThread, (void*)pTask, 0, &pTask->m_dwThreadID );
	}
	return pTask->m_dwHandle;
}

void CTaskManager::RemoveAllTask( bool bImmediate )
{
	if( bImmediate == true ) {
		for( DWORD i=0; i<m_pVecTaskList.size(); i++ ) {
			if( m_pVecTaskList[i]->m_bUseThread == true ) {

				//m_pVecTaskList[i]->m_bUseThread = false;
				SetEvent( m_pVecTaskList[i]->m_hEndRequest );

#if defined( PRE_FIX_CLIENT_FREEZING )
				char szString[1024] = {0,};
				sprintf_s( szString, _countof(szString), "CTaskManager::RemoveAllTask - Task Name : %s - Before", m_pVecTaskList[i]->GetTaskName() );
				CommonUtil::PrintFreezingLog( szString );
#endif

				WaitForSingleObject( m_pVecTaskList[i]->m_hThreadHandle, INFINITE );

#if defined( PRE_FIX_CLIENT_FREEZING )
				sprintf_s( szString, _countof(szString), "CTaskManager::RemoveAllTask - Task Name : %s - After\n", m_pVecTaskList[i]->GetTaskName() );
				CommonUtil::PrintFreezingLog( szString );
#endif

				SAFE_CLOSE_HANDLE( m_pVecTaskList[i]->m_hThreadHandle );
				SAFE_CLOSE_HANDLE( m_pVecTaskList[i]->m_hEndRequest );

				/*while(1) {					
					Sleep(10);
					if( m_pVecTaskList[i]->m_bProcessThread == false ) break;
				}*/
			}
			SAFE_DELETE( m_pVecTaskList[i] );
			m_pVecTaskList.erase( m_pVecTaskList.begin() + i );
			i--;
		}
	}
	else {
		for( DWORD i=0; i<m_pVecTaskList.size(); i++ ) {
			m_pVecTaskList[i]->DestroyTask( true );
		}
	}
	//	SAFE_DELETE_PVEC( m_pVecTaskList );
}

void CTaskManager::RemoveTask( DWORD dwTaskHandle, bool bImmediate )
{

	for( DWORD i=0; i<m_pVecTaskList.size(); i++ ) {
		if( m_pVecTaskList[i]->m_dwHandle == dwTaskHandle ) {
			if( m_pVecTaskList[i]->m_bUseThread == true ) {
				//m_pVecTaskList[i]->m_bUseThread = false;
				if( bImmediate ) {

					SetEvent( m_pVecTaskList[i]->m_hEndRequest );

#if defined( PRE_FIX_CLIENT_FREEZING )
					char szString[1024] = {0,};
					sprintf_s( szString, _countof(szString), "CTaskManager::RemoveAllTask - Task Name : %s - Before", m_pVecTaskList[i]->GetTaskName() );
					CommonUtil::PrintFreezingLog( szString );
#endif

					WaitForSingleObject( m_pVecTaskList[i]->m_hThreadHandle, INFINITE );

#if defined( PRE_FIX_CLIENT_FREEZING )
					sprintf_s( szString, _countof(szString), "CTaskManager::RemoveAllTask - Task Name : %s - After\n", m_pVecTaskList[i]->GetTaskName() );
					CommonUtil::PrintFreezingLog( szString );
#endif

					SAFE_CLOSE_HANDLE( m_pVecTaskList[i]->m_hThreadHandle );
					SAFE_CLOSE_HANDLE( m_pVecTaskList[i]->m_hEndRequest );
					/*while(1) {
						Sleep(10);						
						if( m_pVecTaskList[i]->m_bProcessThread == false ) break;
					}*/
				}
			}
			m_pVecTaskList.erase( m_pVecTaskList.begin() + i );
			return;
		}
	}
}

void CTaskManager::RemoveTask( const char *szName, bool bImmediate )
{
	CTask *pTask = GetTask( szName );
	if( !pTask ) return;
	RemoveTask( pTask->m_dwHandle, bImmediate );
}

CTask *CTaskManager::GetTask( DWORD dwTaskHandle )
{
	for( DWORD i=0; i<m_pVecTaskList.size(); i++ ) {
		if( m_pVecTaskList[i]->m_dwHandle == dwTaskHandle ) return m_pVecTaskList[i];
	}
	return NULL;
}

CTask *CTaskManager::GetTask( const char *szName )
{
	for( DWORD i=0; i<m_pVecTaskList.size(); i++ ) {

		if( m_pVecTaskList[i] && !strcmp( m_pVecTaskList[i]->GetTaskName(), szName ) ) return m_pVecTaskList[i];
	}
	return NULL;
}

#ifdef _RDEBUG
#define _LAG_CHECK 1
#else
#define _LAG_CHECK 0
#endif

#if ( _LAG_CHECK )
static DWORD s_dwCurTime = 0;
static DWORD s_dwLagThreshold = 100;
#endif

#ifndef _FINAL_BUILD
extern bool g_bPauseMode;
#endif

bool CTaskManager::Execute()
{
#if ( _LAG_CHECK )
	s_dwCurTime = timeGetTime();
#endif
	LOCAL_TIME CurTime = timeGetTime();
	CGlobalInfo::GetInstance().m_LocalTime = CurTime;
	CGlobalInfo::GetInstance().m_fDeltaTime = ( CurTime - m_PrevGlobalTime ) * 0.001f;

	bool bSleep = false;
	for( DWORD i=0; i<m_pVecTaskList.size(); i++ ) {
		CTask *pTask = m_pVecTaskList[i];
		if( pTask->m_bUseThread == false ) {			
			if( pTask->IsEnableTaskProcess() ) {
				if( pTask->m_pFrameSync->CheckSync() ) {
					LOCAL_TIME LocalTime = pTask->m_pFrameSync->GetMSTime();
#ifndef _FINAL_BUILD
					if( g_bPauseMode ) LocalTime = pTask->GetPrevLocalTime();
#endif
					float fDelta = ( LocalTime - pTask->GetPrevLocalTime() ) / 1000.f;

					pTask->SetLocalTime( LocalTime );
					pTask->SetDeltaTime( fDelta );

					pTask->Process( LocalTime, fDelta );

					if( pTask->m_bResetTimer ) {
						pTask->m_bResetTimer = false;
						continue;
					}
					pTask->SetPrevLocalTime( LocalTime );
				}
				pTask->m_pFrameSync->UpdateTime( ( m_bLockDebugTask == true ) ? 60 : 0 );			
			}
		}
		else {
			if( pTask->IsBusy() ) {
				bSleep = true;
			}
		}
		if( pTask->m_bDestroyTask == true ) {
			RemoveTask( pTask->m_dwHandle );
			i--;
			if( pTask->m_bDeleteThis ) {
				SAFE_DELETE( pTask );
			}
		}
	}

	if( bSleep ) {		// 로딩쓰레드 작업중이면 메인쓰레드 Sleep 해준다.		
		Sleep( 1 );		
	}
	if( m_nDebugTestLag > 0 ) {
		Sleep( (_rand() % m_nDebugTestLag) /2 + m_nDebugTestLag / 2 );
	}
	if( m_pVecTaskList.empty() ) return false;
	return true;
}

UINT __stdcall CTaskManager::BeginThread( void *pParam )
{
	CTask *pTask = (CTask*)pParam;
	while( true ) {

		if( WaitForSingleObject( pTask->m_hEndRequest, 0 ) == WAIT_OBJECT_0 ) {
			break;
		}

		if( !pTask->IsEnableTaskProcess() ) continue;
		if( pTask->m_pFrameSync->CheckSync() ) {
			LOCAL_TIME LocalTime = pTask->m_pFrameSync->GetMSTime();
			float fDelta = ( LocalTime - pTask->GetPrevLocalTime() ) / 1000.f;

			pTask->SetLocalTime( LocalTime );
			pTask->SetDeltaTime( fDelta );

			pTask->Process( LocalTime, fDelta );

			pTask->SetPrevLocalTime( LocalTime );
		}
		pTask->m_pFrameSync->UpdateTime();

#if (_LAG_CHECK)
		if( timeGetTime() - s_dwCurTime > s_dwLagThreshold ) {
			int aaa=0;
		}
#endif
		Sleep( 1 );		
	}
	return 0;
}

LRESULT CTaskManager::WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	for( DWORD i=0; i<m_pVecTaskList.size(); i++ ) {
		m_pVecTaskList[i]->WndProc( hWnd, message, wParam, lParam );
	}
	return 0;
}

bool CTaskManager::OnCloseTask()
{
	for( DWORD i=0; i<m_pVecTaskList.size(); i++ ) {
		m_pVecTaskList[i]->OnCloseTask();
	}
	return true;
}