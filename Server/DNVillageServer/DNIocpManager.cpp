#include "StdAfx.h"
#include "Connection.h"
#include "DNIocpManager.h"
#include "DNDBConnection.h"
#include "DNDBConnectionManager.h"
#include "DNMasterConnection.h"
#include "DNUserSession.h"
#include "DNUserSessionManager.h"
#include "DNPartyManager.h"
#include "Log.h"
#include "DNLogConnection.h"
#include "DNServiceConnection.h"
#include "DNPvPRoomManager.h"
#include "DNEvent.h"
#include "DNAuthManager.h"
#include "DNCashConnection.h"
#include "DNGuildSystem.h"
#include "DNGameDataManager.h"

#ifdef _USE_VOICECHAT
#include "DNVoiceChat.h"
#endif
#include "DNFieldManager.h"
#include "DNFarm.h"
#include "DNPeriodQuestSystem.h"
#include "DNLadderSystemManager.h"

#if defined (PRE_ADD_DONATION)
#include "DNDonationScheduler.h"
#endif // #if defined (PRE_ADD_DONATION)
#if defined( PRE_PRIVATECHAT_CHANNEL )
#include "DNPrivateChatChannel.h"
#include "DnPrivateChatManager.h"
#endif
#if defined(PRE_ADD_DWC)
#include "DNDWCSystem.h"
#endif

#if defined(_KRAZ)
#include "DNActozShield.h"
#endif	// #if defined(_KRAZ)

#include "RLKTAuth.h"

extern CSyncLock g_UpdateLock;
extern TVillageConfig g_Config;

HANDLE CDNIocpManager::m_hSignalProcess = INVALID_HANDLE_VALUE;
volatile ULONG CDNIocpManager::m_nLasterProcessTick = 0;

CDNIocpManager* g_pIocpManager;

CDNIocpManager::CDNIocpManager(void): CIocpManager()
{
#ifdef _AUTH_
	if (!RLKTAuth::GetInstance().isValidated())
		return;
#endif


	m_hProcessThread = INVALID_HANDLE_VALUE;
	m_hUpdateThread	= INVALID_HANDLE_VALUE;
	m_hReconnectThread = INVALID_HANDLE_VALUE;
	m_hProcessCheckerThread = INVALID_HANDLE_VALUE;

	Final();

#if defined(_WORK)
	m_hSignalProcess = CreateEvent(NULL, TRUE, FALSE, NULL);
#endif

	for (int i = 0; i < ACCEPTOPEN_VERIFY_TYPE_MAX; i++)
		m_bAcceptVerifyList[i] = false;
	m_bClientAcceptOpened = false;
	m_nProcessThreadID = 0;
}

CDNIocpManager::~CDNIocpManager(void)
{
	Final();
}

UINT __stdcall CDNIocpManager::ProcessThread(void *pParam)
{
	CDNIocpManager *pIocp = (CDNIocpManager*)pParam;

	CSocketContext *pSocketContext;
	CConnection *pCon = NULL;
	USHORT nRecv = 0;
	int nCount = 0;

#if defined( PRE_FIX_SOCKETCONTEXT_DANGLINGPTR )
	std::list<CSocketContext*> listDeleteContext;
#endif // #if defined( PRE_FIX_SOCKETCONTEXT_DANGLINGPTR )

	while(pIocp->m_bThreadSwitch)
	{
		m_nLasterProcessTick = timeGetTime();

#if defined( PRE_FIX_SOCKETCONTEXT_DANGLINGPTR )
		for( std::list<CSocketContext*>::iterator itor=listDeleteContext.begin() ; itor!=listDeleteContext.end() ; )
		{
			if( InterlockedCompareExchange( &(*itor)->m_lActiveCount, CSocketContext::DeleteFlag, 0 ) == 0 )
			{
				pSocketContext = (*itor);
				itor = listDeleteContext.erase( itor );

				pCon = (CConnection*)pSocketContext->GetParam();
				if( pCon == NULL )
					continue;

				g_Log.Log(LogType::_NORMAL, static_cast<CDNUserSession*>(pCon), L"## IN_DISCONNECT connection destroyed %x (SID:%u)\r\n", pCon, pCon->GetSessionID());

				g_pUserSessionManager->DelSession((CDNUserSession*)pCon, pSocketContext);
				g_pUserSessionManager->DelTempSession((CDNUserSession*)pCon, pSocketContext);

#if defined( PRE_FIX_SOCKETCONTEXT_DANGLINGPTR ) && defined( _WORK)
				std::cout << "삭제삭제오예~~~~~" << std::endl;
#endif // #if defined( PRE_FIX_SOCKETCONTEXT_DANGLINGPTR ) && defined( _WORK)

				pCon->SetSocketContext(NULL, NULL);
				SAFE_DELETE(pCon);
				pIocp->ClearSocketContext(pSocketContext);

				//g_Log.Log(LogType::_NORMAL, g_Config.nWorldSetID, 0, 0, 0, L"VillageID [%d] UserCount (Con:%d, SC:%d)\r\n", g_Config.nVillageID, g_pUserSessionManager->GetUserCount(), pIocp->GetSocketContextCount());
			}
			else
			{
#if defined( PRE_FIX_SOCKETCONTEXT_DANGLINGPTR ) && defined( _WORK)
				std::cout << "댕글댕글댕글링~~~" << std::endl;
#endif // #if defined( PRE_FIX_SOCKETCONTEXT_DANGLINGPTR ) && defined( _WORK)
				++itor;
			}
		}
#endif // #if defined( PRE_FIX_SOCKETCONTEXT_DANGLINGPTR )

		if (!pIocp->m_ProcessCalls.empty())
		{
			pIocp->m_ProcessLock.Lock();
			pSocketContext = pIocp->m_ProcessCalls.front();
			pIocp->m_ProcessCalls.pop();
			pIocp->m_ProcessLock.UnLock();
#if defined( PRE_FIX_SOCKETCONTEXT_DANGLINGPTR )
			if( std::find( listDeleteContext.begin(), listDeleteContext.end(), pSocketContext ) != listDeleteContext.end() )
				continue;

			CScopeInterlocked Scope( &pSocketContext->m_lActiveCount, pSocketContext->m_dwKeyParam );
			if(Scope.bIsDelete())
				continue;
#endif // #if defined( PRE_FIX_SOCKETCONTEXT_DANGLINGPTR )

			pCon = (CConnection*)pSocketContext->GetParam();
			if (pCon && pCon->FlushRecvData(m_nLasterProcessTick) == false)
			{
				if (pSocketContext->m_dwKeyParam == CONNECTIONKEY_USER)
				{
#if defined( PRE_FIX_SOCKETCONTEXT_DANGLINGPTR )
					listDeleteContext.push_back( pSocketContext );
					g_Log.Log(LogType::_NORMAL, static_cast<CDNUserSession*>(pCon), L"## Push DeleteListContext %x (SID:%u)\r\n", pCon, pCon->GetSessionID());
#else
					g_Log.Log(LogType::_NORMAL, static_cast<CDNUserSession*>(pCon), L"## IN_DISCONNECT connection destroyed %x (SID:%u)\r\n", pCon, pCon->GetSessionID());
					pIocp->ClearSocketContext(pSocketContext);
					if (g_pUserSessionManager->DelConnection((CDNUserSession*)pCon)){
						pCon->SetSocketContext(NULL, NULL);
						SAFE_DELETE(pCon);
					}
					else{
						_DANGER_POINT();
					}

					//g_Log.Log(LogType::_NORMAL, g_Config.nWorldSetID, 0, 0, 0, L"VillageID [%d] UserCount (Con:%d, Obj:%d)\r\n", g_Config.nVillageID, g_pUserSessionManager->GetCount(), g_pUserSessionManager->GetObjCount());
#endif // #if defined( PRE_FIX_SOCKETCONTEXT_DANGLINGPTR )
				}
			}
		}
#if !defined(_WORK)
		// 열라 돌려서 처리시키쟈
		if (++nCount > 10000){
			nCount = 0;
			Sleep(1);
		}
#else
		// 프로세스큐가 비면 논시그널로 바꾸고 신호대기
		else
		{
			ResetEvent(m_hSignalProcess);
			::WaitForSingleObject(m_hSignalProcess, 1);
		}
#endif

		DWORD dwTime = timeGetTime();
		if (g_pGuildManager) 
			g_pGuildManager->DoUpdate(dwTime);
		LadderSystem::CManager::GetInstance().Process( dwTime );
#if defined( PRE_PARTY_DB )
		if( g_pPartyManager )
			g_pPartyManager->InternalDoUpdate( dwTime );
#endif // #if defined( PRE_PARTY_DB )
		if (g_pUserSessionManager)
			g_pUserSessionManager->InternalDoUpdate(dwTime);

		g_pDataManager->DoUpdate(dwTime, 0);
	}

	g_Log.Log(LogType::_NORMAL, L"## ProcessThread Finalize\r\n" );

	return 0;
}

UINT __stdcall CDNIocpManager::UpdateThread(void *pParam)
{
	CDNIocpManager *pIocp = (CDNIocpManager*)pParam;

	DWORD CurTick = 0;
	int nCount = 0;

	while(pIocp->m_bThreadSwitch){
		CurTick = timeGetTime();
		if (g_pUserSessionManager) g_pUserSessionManager->ExternalDoUpdate(CurTick);
#if defined( PRE_PARTY_DB )
#else
		if (g_pPartyManager) g_pPartyManager->DoUpdate(CurTick);
#endif // #if defined( PRE_PARTY_DB )
		if (g_pPeriodQuestSystem) g_pPeriodQuestSystem->DoUpdate(CurTick);
		if( CDNPvPRoomManager::IsActive() )
			CDNPvPRoomManager::GetInstance().DoUpdate( CurTick );

#if defined (PRE_ADD_DONATION)
		if (CDNDonationScheduler::IsActive())
			CDNDonationScheduler::GetInstance().DoUpdate(CurTick);
#endif // #if defined (PRE_ADD_DONATION)

		Sleep(1);
	}

	g_Log.Log(LogType::_NORMAL, L"## UpdateThread Finalize\r\n" );

	return 0;
}

UINT __stdcall CDNIocpManager::ReconnectThread(void *pParam)
{
	CDNIocpManager *pIocp = (CDNIocpManager*)pParam;

	DWORD CurTick = 0;

	while(pIocp->m_bThreadSwitch)
	{
		CurTick = timeGetTime();
		if (g_pDBConnectionManager) g_pDBConnectionManager->Reconnect(CurTick);
		if (g_pMasterConnection)
		{
			g_pMasterConnection->Reconnect();
			g_pMasterConnection->SendVillageUserReport();
			if( g_pFieldManager )
				g_pFieldManager->UpdateChannelShowInfo( false );
		}
		if (g_pLogConnection) g_pLogConnection->Reconnect(CurTick);
		if (g_pServiceConnection) g_pServiceConnection->Reconnect(CurTick);
		if (g_pAuthManager) g_pAuthManager->Run(CurTick);
		if (g_pCashConnection)
		{
			g_pCashConnection->Reconnect();
#ifdef PRE_ADD_LIMITED_CASHITEM
			g_pCashConnection->SendGetLimitedItemList();
#endif		//#ifdef PRE_ADD_LIMITED_CASHITEM
		}

#if defined(_KRAZ)
		if (g_pActozShield)
			g_pActozShield->RepeatCheckerResult(CurTick);
#endif	// #if defined(_KRAZ)

		Sleep(5000);	// 5초에 한번씩
	}
	return 0;
}

UINT __stdcall CDNIocpManager::ProcessChecker(void *pParam)
{
	CDNIocpManager *pIocp = (CDNIocpManager*)pParam;
	while(pIocp->m_bThreadSwitch)
	{
		if (timeGetTime() > m_nLasterProcessTick + (10 * 1000))
		{
			g_Log.Log(LogType::_ERROR, L"Process Thread Stuck Write Dump\n");

#if defined(_FINAL_BUILD)
			pIocp->StoreMiniDump();
#endif
		}
		Sleep(1000);	// 1초에 한번씩
	}
	return 0;
}

int CDNIocpManager::Init(int nSocketCountMax)
{
#ifdef _AUTH_
	if (!RLKTAuth::GetInstance().isValidated())
		return -1;
#endif
	CIocpManager::Init(nSocketCountMax);
	return CreateThread();
}

void CDNIocpManager::Final()
{
	CIocpManager::Final();

	if( m_hProcessThread != INVALID_HANDLE_VALUE )
	{
		WaitForSingleObject( m_hProcessThread, INFINITE );
		m_hProcessThread = INVALID_HANDLE_VALUE;
	}
	if( m_hUpdateThread != INVALID_HANDLE_VALUE )
	{
		WaitForSingleObject( m_hUpdateThread, INFINITE );
		m_hUpdateThread = INVALID_HANDLE_VALUE;
	}
	if( m_hReconnectThread != INVALID_HANDLE_VALUE )
	{
		WaitForSingleObject( m_hReconnectThread, INFINITE );
		m_hReconnectThread = INVALID_HANDLE_VALUE;
	}

	while (!m_ProcessCalls.empty()){
		m_ProcessCalls.pop();
	}

#if defined(_WORK)
	if( m_hSignalProcess != INVALID_HANDLE_VALUE ) 
	{
		CloseHandle(m_hSignalProcess);
		m_hSignalProcess = INVALID_HANDLE_VALUE;
	}
#endif
}

int CDNIocpManager::CreateThread()
{
	UINT ThreadID;

	m_hProcessThread = (HANDLE)_beginthreadex(NULL, 0, &ProcessThread, this, 0, &m_nProcessThreadID);
	if (m_hProcessThread == INVALID_HANDLE_VALUE) return -1;

	m_hUpdateThread = (HANDLE)_beginthreadex(NULL, 0, &UpdateThread, this, 0, &ThreadID);
	if (m_hUpdateThread == INVALID_HANDLE_VALUE) return -1;

	m_hReconnectThread = (HANDLE)_beginthreadex(NULL, 0, &ReconnectThread, this, 0, &ThreadID);
	if (m_hReconnectThread == INVALID_HANDLE_VALUE) return -1;

	/*m_hProcessCheckerThread = (HANDLE)_beginthreadex(NULL, 0, &ProcessChecker, this, 0, &ThreadID);
	if (m_hProcessCheckerThread == INVALID_HANDLE_VALUE) return -1;*/

	return 0;
}

void CDNIocpManager::AddProcessCall(CSocketContext *pSocketContext )
{
	if (pSocketContext->GetParam() == NULL) return;

	m_ProcessLock.Lock();
	m_ProcessCalls.push(pSocketContext);
	m_ProcessLock.UnLock();

#if defined(_WORK)
	SetEvent(m_hSignalProcess);
#endif
}

void CDNIocpManager::OnAccept(CSocketContext *pSocketContext, const char* pIp, const int nPort)
{
	switch(pSocketContext->m_dwKeyParam)
	{
	case CONNECTIONKEY_USER:
		{
			CDNUserSession *pUserObj = g_pUserSessionManager->AddSession(pIp, nPort);
			if (!pUserObj) return;

			pSocketContext->SetParam(pUserObj);
			pUserObj->SetSocketContext(this, pSocketContext);

			//g_Log.Log(LogType::_NORMAL, L"[OnAccept:User] Socket:%d, Ip:%S\r\n", pSocketContext->m_Socket, pUserObj->GetIp());
		}
		break;

	case CONNECTIONKEY_CASH:
		{
			if (!g_pCashConnection) return;

			pSocketContext->SetParam(g_pCashConnection);
			g_pCashConnection->SetSocketContext(this, pSocketContext);
			g_pCashConnection->SetIp(pIp);
			g_pCashConnection->SetPort(nPort);

			g_Log.Log(LogType::_NORMAL, L"[OnAccept:Cash] Socket:%d, Ip:%S\r\n", pSocketContext->m_Socket, g_pCashConnection->GetIp());
		}
		break;

	case CONNECTIONKEY_MASTER:
		{
			if (!g_pMasterConnection) return;

			pSocketContext->SetParam(g_pMasterConnection);
			g_pMasterConnection->SetSocketContext(this, pSocketContext);
			g_pMasterConnection->SetIp(pIp);
			g_pMasterConnection->SetPort(nPort);

			g_Log.Log(LogType::_NORMAL, L"[OnAccept:Master] Socket:%d, Ip:%S\r\n", pSocketContext->m_Socket, g_pMasterConnection->GetIp());
		}
		break;

	case CONNECTIONKEY_DB:
		{
			CDNDBConnection *pDBCon = (CDNDBConnection*)g_pDBConnectionManager->GetConnectionByIPPort(pIp, nPort);
			if (!pDBCon) return;

			pSocketContext->SetParam(pDBCon);
			pDBCon->SetSocketContext(this, pSocketContext);
			pDBCon->SetServerConnection(true);

			g_Log.Log(LogType::_NORMAL, L"[OnAccept:DB] Socket:%d, Ip:%S\r\n", pSocketContext->m_Socket, pDBCon->GetIp());
		}
		break;

	case CONNECTIONKEY_LOG:
		{
			if (!g_pLogConnection) return;

			pSocketContext->SetParam(g_pLogConnection);
			g_pLogConnection->SetSocketContext(this, pSocketContext);
			g_pLogConnection->SetServerConnection(false);

			g_Log.Log(LogType::_NORMAL, L"[OnAccept:Log] Socket:%d, Ip:%S\r\n", pSocketContext->m_Socket, g_pLogConnection->GetIp());
		}
		break;

	case CONNECTIONKEY_SERVICEMANAGER:
		{
			if (!g_pServiceConnection) return;

			pSocketContext->SetParam(g_pServiceConnection);
			g_pServiceConnection->SetSocketContext(this, pSocketContext);
			g_pServiceConnection->SetServerConnection(false);

			g_Log.Log(LogType::_NORMAL, L"[OnAccept:Service] Socket:%d, Ip:%S\r\n", pSocketContext->m_Socket, g_pServiceConnection->GetIp());
		}
		break;
	}
}

// 2009.02.03 김밥
// OnConnected() 에서 BufferClear()을 할경우 접속하자마자 패킷보내는경우 버퍼 꼬여서 패킷 유실됨.
// CIocpManager::AttachSocket() 함수에서 BufferClear() 로 변경함

void CDNIocpManager::OnConnected(CSocketContext *pSocketContext)
{
	switch(pSocketContext->m_dwKeyParam)
	{
	case CONNECTIONKEY_USER:
		{
			CDNUserSession *pUserObj = (CDNUserSession*)pSocketContext->GetParam();
			if (!pUserObj) return;

			pUserObj->SetActive(true);
			pUserObj->SetDelete(false);

			//g_Log.Log(LogType::_NORMAL, pUserObj, L"[OnConnected:User] Socket:%d IP[%S]\r\n", pSocketContext->m_Socket, pUserObj->GetIp());
		}
		break;

	case CONNECTIONKEY_CASH:
		{
			if (!g_pCashConnection) return;

			g_pCashConnection->SetActive(true);
			g_pCashConnection->SetDelete(false);

			g_Log.Log(LogType::_NORMAL, L"[OnConnected:Cash] Socket:%d\r\n", pSocketContext->m_Socket);
		}
		break;

	case CONNECTIONKEY_MASTER:
		{
			// 2010.10.1 haling..간혹 _WORK에서 SetDelete(false)되기 전에 VIMA_REGIST 날릴일이 있어 위로 올립니다.
			if (!g_pMasterConnection) return;

			g_pMasterConnection->SetActive(true);
			g_pMasterConnection->SetDelete(false);
			//g_pMasterConnection->BufferClear();	

			if( g_pFieldManager )
				g_pFieldManager->UpdateChannelShowInfo();

			g_Log.Log(LogType::_NORMAL, L"[OnConnected:Master] Socket:%d\r\n", pSocketContext->m_Socket);
		}
		break;

	case CONNECTIONKEY_DB:
		{
			CDNDBConnection *pDBCon = (CDNDBConnection*)pSocketContext->GetParam();
			if (!pDBCon) return;

			pDBCon->SetActive(true);
			pDBCon->SetDelete(false);
			//pDBCon->BufferClear();

			if (g_Config.nWorldSetID > 0) 
			{
				if (g_pEvent && g_pEvent->IsInitWorld(g_Config.nWorldSetID) == false)
					pDBCon->QueryEventList(rand()%THREADMAX, g_Config.nWorldSetID);

				if (g_pFarm && g_pFarm->GetInitFlag() == false)
					pDBCon->QueryFarmList(rand()%THREADMAX, g_Config.nWorldSetID);
				if (g_pPeriodQuestSystem)
					g_pPeriodQuestSystem->LoadWorldQuestInfo(g_Config.nWorldSetID);
//#ifdef PRE_MOD_RESTRICT_IDENTITY_IP
//				pDBCon->QuerySimpleConfig(rand()%THREADMAX);
//#endif		//#ifdef PRE_MOD_RESTRICT_IDENTITY_IP

#if defined (PRE_ADD_DONATION)
				if (CDNDonationScheduler::IsActive())
					CDNDonationScheduler::GetInstance().AddWorldID(g_Config.nWorldSetID);
#endif // #if defined (PRE_ADD_DONATION)
#if defined( PRE_PRIVATECHAT_CHANNEL )
				if( g_pPrivateChatChannelManager && g_pPrivateChatChannelManager->GetFirst() == false )
				{					
					pDBCon->QueryGetPrivateChatChannelInfo(0, g_Config.nWorldSetID);
					pDBCon->QueryDelPrivateMemberServerID(0, g_Config.nWorldSetID, g_Config.nManagedID );
					g_pPrivateChatChannelManager->SetFirst(true);
				}
#endif
#if defined( PRE_WORLDCOMBINE_PVP )
				pDBCon->QueryUpdateWorldPvPRoom(0, g_Config.nWorldSetID);
#endif
#if defined( PRE_ALTEIAWORLD_EXPLORE )
				if( !g_pDataManager->bIsAlteiaWorldEventTime() )
				{
					pDBCon->QueryResetAlteiaWorldEvent(0, g_Config.nWorldSetID);
#if defined( _WORK )
					g_Log.Log(LogType::_NORMAL, g_Config.nWorldSetID, 0, 0, 0, L"Reset AlteiaWorldEvent WorldID:%d\r\n", g_Config.nWorldSetID);
#endif
				}

				time_t tAlteiaEventStartTime = g_pDataManager->GetAlteiaWorldEventBeginTime();
				time_t tAlteiaEventEndTime = g_pDataManager->GetAlteiaWorldEventEndTime();				
				if( tAlteiaEventStartTime > 0 && tAlteiaEventEndTime > 0 )
				{
					pDBCon->QueryAddAlteiaWorldEventTime(0, g_Config.nWorldSetID, tAlteiaEventStartTime, tAlteiaEventEndTime);
				}
#endif
#if defined(PRE_ADD_DWC)
				if (g_pDWCTeamManager && g_pDWCTeamManager->GetInitFlag() == false)
					pDBCon->QueryGetDWCChannelInfo(rand()%THREADMAX, g_Config.nWorldSetID);
#endif
			}

			if( g_pFieldManager )
				g_pFieldManager->UpdateChannelShowInfo();

			g_Log.Log(LogType::_NORMAL, L"[OnConnected:DB] Socket:%d\r\n", pSocketContext->m_Socket);
		}
		break;

	case CONNECTIONKEY_LOG:
		{
			if (!g_pLogConnection) return;

			g_pLogConnection->SetActive(true);
			g_pLogConnection->SetDelete(false);
		}
		break;

	case CONNECTIONKEY_SERVICEMANAGER:
		{
			if (!g_pServiceConnection) return;

			g_pServiceConnection->SetActive(true);
			g_pServiceConnection->SetDelete(false);
		}
		break;
	}
}

void CDNIocpManager::OnConnectFail(CSocketContext *pSocketContext)
{
	// 서버에서 커넥트를 시도 하는 애들만 넣어주세요..AddConnectionEx를 호출하는 애들?
	CConnection* pConnection = static_cast<CConnection*>(pSocketContext->GetParam());
	if( !pConnection )
		return;

	pConnection->SetConnecting(false);

	switch(pSocketContext->m_dwKeyParam)
	{
	case CONNECTIONKEY_LOG:
		{
			// g_Log.Log(_ERROR, L"LogServer Fail(%S, %d)\r\n", pConnection->GetIp(), pConnection->GetPort());
			// 잠깐 막아두겠음!!! 080918 saset
		}
		break;
	case CONNECTIONKEY_SERVICEMANAGER :
		{
			g_Log.Log(LogType::_ERROR, L"ServiceManager Connect Failed(%S, %d)\r\n", pConnection->GetIp(), pConnection->GetPort());
		}
		break;
	case CONNECTIONKEY_CASH :
		{
			// g_Log.Log(LogType::_ERROR, L"CashServer Fail(%S, %d)\r\n", pConnection->GetIp(), pConnection->GetPort());
		}
		break;
	case CONNECTIONKEY_DB :
		{
			g_Log.Log(LogType::_ERROR, L"DBServer Fail(%S, %d)\r\n", pConnection->GetIp(), pConnection->GetPort());
		}
		break;
	case  CONNECTIONKEY_MASTER :
		{
			g_Log.Log(LogType::_ERROR, L"MasterServer Fail(%S, %d)\r\n", pConnection->GetIp(), pConnection->GetPort());
		}
		break;
	}
}


void CDNIocpManager::OnDisconnected(CSocketContext *pSocketContext)
{
	switch(pSocketContext->m_dwKeyParam)
	{
	case CONNECTIONKEY_USER:
		{
#if defined( PRE_FIX_SOCKETCONTEXT_DANGLINGPTR )
			CScopeInterlocked Scope( &pSocketContext->m_lActiveCount );
			if( Scope.bIsDelete() )
				break;
#endif // #if defined( PRE_FIX_SOCKETCONTEXT_DANGLINGPTR )
			CDNUserSession *pUserObj = (CDNUserSession*)pSocketContext->GetParam();
			if (!pUserObj) return;

			//g_Log.Log(L"[OnDisconnected:User] (AUID:%u CUID:%u) Socket:%d\r\n", pUserCon->GetAccountDBID(), pUserCon->GetSessionID(), pSocketContext->m_Socket);
			std::wstring wstrDetachReason;
			pSocketContext->GetDetachReason(wstrDetachReason);
			pUserObj->SendInsideDisconnectPacket((wstrDetachReason.size() > 0) ? wstrDetachReason.c_str() : L"OnDisconnected");		// 객체 정리 패킷 생성
		}
		break;

	case CONNECTIONKEY_CASH:
		{
			if (!g_pCashConnection) return;

			g_pCashConnection->SetActive(false);
			g_pCashConnection->SetDelete(true);

			std::wstring wstrDetachReason;
			pSocketContext->GetDetachReason(wstrDetachReason);

			g_Log.Log(LogType::_NORMAL, L"[OnDisconnected:Cash] Socket:%d %s\r\n", pSocketContext->m_Socket, wstrDetachReason.c_str());
			ClearSocketContext(pSocketContext);
			g_pCashConnection->SetSocketContext(NULL, NULL);		
		}
		break;

	case CONNECTIONKEY_MASTER:
		{
			if (!g_pMasterConnection) return;

#if defined( PRE_FIX_SOCKETCONTEXT_DANGLINGPTR )

			while( true )
			{
				if( InterlockedCompareExchange( &pSocketContext->m_lActiveCount, CSocketContext::DeleteFlag, 0 ) == 0 )
				{
					CDNPvPRoomManager::GetInstance().OnDisconnected( CONNECTIONKEY_MASTER );
#if defined( PRE_WORLDCOMBINE_PARTY )
					g_pPartyManager->ClearReqWorldParty();
#endif
					g_pMasterConnection->SetActive(false);
					g_pMasterConnection->SetDelete(true);
					g_pMasterConnection->SetInComplete();

					std::wstring wstrDetachReason;
					pSocketContext->GetDetachReason(wstrDetachReason);

					g_Log.Log(LogType::_NORMAL, L"[OnDisconnected:Master] Socket:%d %s\r\n", pSocketContext->m_Socket, wstrDetachReason.c_str());
					ClearSocketContext(pSocketContext);
					g_pMasterConnection->SetSocketContext(NULL, NULL);
					break;
				}
			}
#else

			CDNPvPRoomManager::GetInstance().OnDisconnected( CONNECTIONKEY_MASTER );

			g_pMasterConnection->SetActive(false);
			g_pMasterConnection->SetDelete(true);
			g_pMasterConnection->SetInComplete();

			g_Log.Log(LogType::_NORMAL, L"[OnDisconnected:Master] Socket:%d\r\n", pSocketContext->m_Socket);
			ClearSocketContext(pSocketContext);
			g_pMasterConnection->SetSocketContext(NULL, NULL);

#endif // #if defined( PRE_FIX_SOCKETCONTEXT_DANGLINGPTR )

		}
		break;

	case CONNECTIONKEY_DB:
		{
#if defined( PRE_FIX_SOCKETCONTEXT_DANGLINGPTR )
			while( true )
			{
				if( InterlockedCompareExchange( &pSocketContext->m_lActiveCount, CSocketContext::DeleteFlag, 0 ) == 0 )
				{
					CDNDBConnection *pDBCon = (CDNDBConnection*)pSocketContext->GetParam();
					if (!pDBCon) 
						break;

					pDBCon->SetActive(false);
					pDBCon->SetDelete(true);
					std::wstring wstrDetachReason;
					pSocketContext->GetDetachReason(wstrDetachReason);
					g_Log.Log(LogType::_NORMAL, L"[OnDisconnected:DB] Socket:%d\r\n", pSocketContext->m_Socket, wstrDetachReason.c_str());
					ClearSocketContext(pSocketContext);
					pDBCon->SetSocketContext(NULL, NULL);

					if( g_pFieldManager )
						g_pFieldManager->UpdateChannelShowInfo();

					break;
				}
			}
#else
			CDNDBConnection *pDBCon = (CDNDBConnection*)pSocketContext->GetParam();
			if (!pDBCon) return;

			pDBCon->SetActive(false);
			pDBCon->SetDelete(true);
			g_Log.Log(LogType::_NORMAL, L"[OnDisconnected:DB] Socket:%d\r\n", pSocketContext->m_Socket);
			ClearSocketContext(pSocketContext);
			pDBCon->SetSocketContext(NULL, NULL);

			if( g_pFieldManager )
				g_pFieldManager->UpdateChannelShowInfo();

#endif // #if defined( PRE_FIX_SOCKETCONTEXT_DANGLINGPTR )
		}
		break;

	case CONNECTIONKEY_LOG:
		{
#if defined( PRE_FIX_SOCKETCONTEXT_DANGLINGPTR )
			
			while( true )
			{
				if( InterlockedCompareExchange( &pSocketContext->m_lActiveCount, CSocketContext::DeleteFlag, 0 ) == 0 )
				{
					g_pLogConnection->SetActive(false);
					g_pLogConnection->SetDelete(true);
					ClearSocketContext(pSocketContext);
					g_pLogConnection->SetSocketContext(NULL,NULL);
#if defined(PRE_ADD_LOGSERVER_HEARTBEAT)
					g_pLogConnection->ResetLiveCheck();
#endif
					break;
				}
			}
#else

			g_pLogConnection->SetActive(false);
			g_pLogConnection->SetDelete(true);
			ClearSocketContext(pSocketContext);
			g_pLogConnection->SetSocketContext(NULL,NULL);
#if defined(PRE_ADD_LOGSERVER_HEARTBEAT)
			g_pLogConnection->ResetLiveCheck();
#endif
#endif // #if defined( PRE_FIX_SOCKETCONTEXT_DANGLINGPTR )

		}
		break;

	case CONNECTIONKEY_SERVICEMANAGER:
		{
			g_pServiceConnection->Disconnected();
			g_pServiceConnection->SetActive(false);
			g_pServiceConnection->SetDelete(true);
			ClearSocketContext(pSocketContext);
			g_pServiceConnection->SetSocketContext(NULL,NULL);
		}
		break;
	}
}

void CDNIocpManager::OnReceive(CSocketContext *pSocketContext, DWORD dwBytesTransferred)
{
#if defined( PRE_FIX_SOCKETCONTEXT_DANGLINGPTR )
	CScopeInterlocked Scope( &pSocketContext->m_lActiveCount );
	if( Scope.bIsDelete() )
		return;
#endif // #if defined( PRE_FIX_SOCKETCONTEXT_DANGLINGPTR )
	CConnection* pCon = static_cast<CConnection*>(pSocketContext->GetParam());
	if (pCon) {
		int nResult = pCon->AddRecvData( pSocketContext );
		if (nResult > 0)
		{
			AddProcessCall(pSocketContext);

			//test
			if (pSocketContext->m_dwKeyParam == CONNECTIONKEY_USER)
				m_nAddRecvBufSize += dwBytesTransferred;
		}
		else if (nResult == SIZEERR){
			g_Log.Log(LogType::_ERROR, L"[OnReceive SizeError] Socket:%lld Size:%d\r\n", pSocketContext->m_Socket, pSocketContext->m_RecvIO.Len);
			DetachSocket(pSocketContext, L"OnReceive SizeError");
		}
		else if (nResult == -1)
		{
			if (pSocketContext->m_dwKeyParam == CONNECTIONKEY_USER)
			{
				g_Log.Log(LogType::_ERROR, static_cast<CDNUserSession*>(pCon), L"[OnReceive] Last Cmd:%d, Sub:%d\r\n", pCon->m_DebugInfo[pCon->m_DebugInfoCount&31]._DebugMainCmd,
					pCon->m_DebugInfo[pCon->m_DebugInfoCount&31]._DebugSubCmd);
			}
			else
				g_Log.Log(LogType::_ERROR, L"[OnReceive] Socket:%lld Size:%d\r\n", pSocketContext->m_Socket, pSocketContext->m_RecvIO.Len);
			DetachSocket(pSocketContext, L"OnReceive");
		}
	}
}

void CDNIocpManager::ProcessDelay(int nConKey, int nMainCmd, int nSubCmd, int nTick)
{
	g_Log.Log(LogType::_ERROR, L"Report|ProcessDelayed CK[%d] MC[%d] SC[%d] Tick[%d]\n", nConKey, nMainCmd, nSubCmd, nTick);
}

void CDNIocpManager::StagnatePacket(int nCnt, int nTick, CSocketContext *pSocketContext)
{
	if (pSocketContext->m_dwKeyParam == CONNECTIONKEY_USER)
	{
		DetachSocket(pSocketContext, L"Stagnated Packet");		//유저인 경우에만 패킷이 쌓였을 경우 끊어준다
		g_Log.Log(LogType::_ERROR, L"Connect|StagnatePacket DetachUser\n");
	}

	g_Log.Log(LogType::_ERROR, L"Report|StagnatePacket UserCnt[%d] Cnt[%d] Tick[%d]\n", g_pUserSessionManager->GetUserCount(), nCnt, nTick);
}

void CDNIocpManager::VerifyAccept(int nVerifyType)
{
	if (m_bClientAcceptOpened) return;
	m_bAcceptVerifyList[nVerifyType] = true;
	
	for (int i = 0; i < ACCEPTOPEN_VERIFY_TYPE_MAX; i++)
	{
		if (m_bAcceptVerifyList[i] == false)
			return;
	}

	if (AddAcceptConnection(CONNECTIONKEY_USER, g_Config.nClientAcceptPort, SOMAXCONN) < 0)
		g_Log.Log(LogType::_ERROR, L"ClientAcceptPort Fail(%d)\r\n", g_Config.nClientAcceptPort);
	else
	{
		g_Log.Log(LogType::_NORMAL, L"ClientAcceptPort (%d)\r\n", g_Config.nClientAcceptPort);
		m_bClientAcceptOpened = true;
	}
}



