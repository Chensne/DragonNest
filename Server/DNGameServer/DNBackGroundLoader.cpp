
#include "stdafx.h"
#include "DNBackGroundLoader.h"
#include "ExceptionReport.h"
#if defined( PRE_WORLDUSERSTATE_OPTIMIZE )
#include "DNGameWorldUserState.hpp"
#endif // #if defined( PRE_WORLDUSERSTATE_OPTIMIZE )
#include "DNServiceConnection.h"

extern TGameConfig g_Config;
CDNBackGroundLoader * g_pBackLoader = NULL;
HANDLE CDNBackGroundLoader::m_hCallSignal = INVALID_HANDLE_VALUE;

CDNBackGroundLoader::CDNBackGroundLoader( bool bLog ) : Thread("BackLoader"), m_bLog(bLog)
{
	m_hCallSignal = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_inowLoading = 0;
	m_pStandbyList = new _STANDBY_ROOM[MAX_SESSION_COUNT];
	m_bRunning = true;
	Start();
}

CDNBackGroundLoader::~CDNBackGroundLoader()
{
	m_bRunning = false;
	SetEvent( m_hCallSignal );
	WaitForTerminate();

	while (!m_CallList.empty())
	{
		m_CallList.pop();
	}
	CloseHandle(m_hCallSignal);

	delete [] m_pStandbyList;
}

bool CDNBackGroundLoader::PushToLoadProcess(CDNGameRoom * pRoom, bool bContinue)
{
	if (m_pStandbyList[pRoom->GetRoomID()].pRoom == NULL)
	{
		_ASSERT(m_pStandbyList[pRoom->GetRoomID()].pRoom == NULL);

		m_CallSync.Lock();
		m_CallList.push(pRoom->GetRoomID());

		m_pStandbyList[pRoom->GetRoomID()].bIsLoaded = false;
		m_pStandbyList[pRoom->GetRoomID()].bIsContinue = bContinue;
		m_pStandbyList[pRoom->GetRoomID()].pRoom = pRoom;
#if defined( PRE_THREAD_ROOMDESTROY )
		m_pStandbyList[pRoom->GetRoomID()].bIsDestroyProcess = false;
#endif // #if defined( PRE_THREAD_ROOMDESTROY )
		SetEvent(m_hCallSignal);		
		m_CallSync.UnLock();
		return true;
	}
	return false;
}

#if defined( PRE_THREAD_ROOMDESTROY )
bool CDNBackGroundLoader::PushToDestroyProcess( CDNGameRoom* pRoom )
{
	if (m_pStandbyList[pRoom->GetRoomID()].pRoom == NULL)
	{
		m_CallSync.Lock();
		m_CallList.push(pRoom->GetRoomID());

		m_pStandbyList[pRoom->GetRoomID()].bIsDestroyProcess = true;
		m_pStandbyList[pRoom->GetRoomID()].pRoom = pRoom;
		SetEvent(m_hCallSignal);		
		m_CallSync.UnLock();
		return true;
	}
	return false;
}

void CDNBackGroundLoader::DestroyConfirm( int idx )
{
	_ASSERT(m_pStandbyList[idx].pRoom != NULL);
	m_pStandbyList[idx].bIsLoaded = false;
	m_pStandbyList[idx].bIsContinue = false;
	m_pStandbyList[idx].bIsInitFail = false;
	m_pStandbyList[idx].pRoom = NULL;
	m_pStandbyList[idx].bIsDestroyProcess = false;
#ifdef _DEBUG
	if( m_bLog )
		g_Log.Log( LogType::_NORMAL, L"CDNBackGroundLoader Destroy Confirm RoomID : %d\n", idx );
#endif
}

#endif // #if defined( PRE_THREAD_ROOMDESTROY )

bool CDNBackGroundLoader::IsLoaded(int idx)
{
	_ASSERT(m_pStandbyList[idx].pRoom != NULL);
	if (m_pStandbyList[idx].bIsLoaded == true)
		return true;
	return false;
}

bool CDNBackGroundLoader::LoadConfirm(int idx)
{
	_ASSERT(m_pStandbyList[idx].pRoom != NULL);
	bool bInitFail = m_pStandbyList[idx].bIsInitFail;
	m_pStandbyList[idx].bIsLoaded = false;
	m_pStandbyList[idx].bIsContinue = false;
	m_pStandbyList[idx].bIsInitFail = false;
	m_pStandbyList[idx].pRoom = NULL;
#if defined( PRE_THREAD_ROOMDESTROY )
	m_pStandbyList[idx].bIsDestroyProcess = false;
#endif // #if defined( PRE_THREAD_ROOMDESTROY )
#ifdef _DEBUG
	if( m_bLog )
		g_Log.Log( LogType::_NORMAL, L"CDNBackGroundLoader Load Confirm RoomID : %d\n", idx );
#endif
	return bInitFail;
}

bool CDNBackGroundLoader::LoadCancel(int idx)
{
	if (m_inowLoading == idx)
		return false;

	if (m_pStandbyList[idx].pRoom == NULL)
		return true;
	
	m_pStandbyList[idx].bIsLoaded = false;
	m_pStandbyList[idx].bIsContinue = false;
	m_pStandbyList[idx].pRoom = NULL;
#if defined( PRE_THREAD_ROOMDESTROY )
	m_pStandbyList[idx].bIsDestroyProcess = false;
#endif // #if defined( PRE_THREAD_ROOMDESTROY )
#ifdef _DEBUG
	if( m_bLog )
		g_Log.Log( LogType::_NORMAL, L"CDNBackGroundLoader Cancel Load RoomID : %d\n", idx );
#endif
	return true;
}

bool CDNBackGroundLoader::IsLoading( int idx )
{
	if (m_inowLoading == idx)
		return true;

	return false;
}

void CDNBackGroundLoader::Run()
{
	// 2009.01.20 김밥
	// 무한루프로 되어빠지는 경우가 있어 m_bRunning 변수로 루프 체크
	while (m_bRunning)
	{
		if (!m_CallList.empty())
		{
#if defined( PRE_WORLDUSERSTATE_OPTIMIZE )
			if( g_pWorldUserState )
				g_pWorldUserState->SnapShotUserState();
#endif // #if defined( PRE_WORLDUSERSTATE_OPTIMIZE )
			m_CallSync.Lock();
			int nCallID = m_CallList.front();
			m_CallList.pop();
			m_CallSync.UnLock();

			m_inowLoading = nCallID;
#if defined( PRE_THREAD_ROOMDESTROY )
			if( m_pStandbyList[nCallID].pRoom && m_pStandbyList[nCallID].bIsDestroyProcess == true )
			{
				InterlockedCompareExchange( &m_pStandbyList[nCallID].pRoom->m_lRoomDestroyInterLocked, CDNGameRoom::eRoomDestoryStep::Destroyed, CDNGameRoom::eRoomDestoryStep::PushQueue );
				m_inowLoading = -1;
				continue;
			}
#endif // #if defined( PRE_THREAD_ROOMDESTROY )

			// try catch 때문에 Scope 함수를 못쓴다..힝~
			//CScopeNowLoading Scope( &m_inowLoading, nCallID );

			if (m_pStandbyList[nCallID].pRoom == NULL || m_pStandbyList[nCallID].bIsLoaded != false)
			{
				m_inowLoading = -1;
				continue;
			}

			static bool bProcessLog = true;
			if( m_pStandbyList[nCallID].pRoom->m_lRoomProcessInterLocked > 0 )
			{
				m_CallSync.Lock();
				m_CallList.push( nCallID );
				if( bProcessLog )
				{
					g_Log.Log( LogType::_NORMAL, L"[%d] CDNBackGroundLoader Process 중이어서 다시 Push함 RoomID : %d\n", g_Config.nManagedID, nCallID );
					bProcessLog = false;
				}
				m_CallSync.UnLock();
				m_inowLoading = -1;
				continue;
			}

			bProcessLog = true;

#if defined(_WORK)
			if( m_bLog )
				g_Log.Log( LogType::_NORMAL, L"CDNBackGroundLoader Start Load RoomID : %d\n", nCallID );
#endif // #if defined(_WORK)

			if (m_pStandbyList[nCallID].pRoom != NULL)
			{
#ifdef _FINAL_BUILD
				__try
				{
#endif
				m_pStandbyList[nCallID].bIsInitFail = m_pStandbyList[nCallID].pRoom->LoadData( m_pStandbyList[nCallID].bIsContinue );
#ifdef _FINAL_BUILD
				}

				__except(CExceptionReport::GetInstancePtr()->Proc(GetExceptionInformation(), MiniDumpNormal)) 
				{
					if( m_pStandbyList[nCallID].pRoom )
						m_pStandbyList[nCallID].pRoom->SetRoomCrashFlag( true );

					g_Log.Log( LogType::_ROOMCRASH, L"[%d] DNBackGroundLoader Room Crash! RoomID=%d\n", g_Config.nManagedID, m_pStandbyList[nCallID].pRoom->GetRoomID() );
					LoadCancel(nCallID);
					//g_Log.Log(L"Destroy|BackLoader Crash\n");

#if !defined (_TH)
					if (g_pServiceConnection)
						g_pServiceConnection->SendDetectException(_EXCEPTIONTYPE_ROOMCRASH);
#endif // #if !defined (_TH)
				}
#endif
			}
			else
				LoadCancel(nCallID);

			m_pStandbyList[nCallID].bIsLoaded = true;

#if defined(_WORK)
			if( m_bLog )
				g_Log.Log( LogType::_NORMAL, L"CDNBackGroundLoader Finish Load RoomID : %d\n", nCallID );
#endif // #if defined(_WORK)
			m_inowLoading = -1;
		}
		else
		{
#if defined( PRE_WORLDUSERSTATE_OPTIMIZE )
			if( g_pWorldUserState )
				g_pWorldUserState->SnapShotUserState();
#endif // #if defined( PRE_WORLDUSERSTATE_OPTIMIZE )

			ResetEvent(m_hCallSignal);
#if defined( PRE_WORLDUSERSTATE_OPTIMIZE )
			::WaitForSingleObject(m_hCallSignal, 1000 );
#else
			::WaitForSingleObject(m_hCallSignal, INFINITE);
#endif // #if defined( PRE_WORLDUSERSTATE_OPTIMIZE )
		}
	}
}
