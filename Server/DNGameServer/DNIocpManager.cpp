#include "StdAfx.h"
#include "Connection.h"
#include "DNIocpManager.h"
#include "DNMasterConnection.h"
#include "DNMasterConnectionManager.h"
#include "DNDBConnection.h"
#include "DNDBConnectionManager.h"
#include "DNGameServerManager.h"
#include "Log.h"
#include "DNLogConnection.h"
#include "DnServiceConnection.h"
#include "DNUserTcpConnection.h"
#include "DNEvent.h"
#include "DNAuthManager.h"
#include "DNCashConnection.h"
#include "DNGuildSystem.h"
#ifdef _USE_VOICECHAT
#include "DNVoiceChat.h"
#endif
#include "DNPeriodQuestSystem.h"
#if defined (PRE_ADD_DONATION)
#include "DNDonationScheduler.h"
#endif // #if defined (PRE_ADD_DONATION)
#if defined( PRE_PRIVATECHAT_CHANNEL )
#include "DNPrivateChatChannel.h"
#include "DnPrivateChatManager.h"
#endif

#ifdef _AUTH_
#include "RLKTAuth.h"
#endif

#if defined(_KRAZ)
#include "DNActozShield.h"
#endif	// #if defined(_KRAZ)

CDNIocpManager* g_pIocpManager;
extern TGameConfig g_Config;

CDNIocpManager::CDNIocpManager(void): CIocpManager()
{
#if defined(_GAMESERVER) && defined(_AUTH_)
	if (!isValidated())
		return;
#endif
	// 2009.01.20 김밥
	m_hTerminateEvent	= CreateEvent( NULL, false, false, NULL );
	m_hReconnectThread	= INVALID_HANDLE_VALUE;

	Final();

	for (int i = 0; i < ACCEPTOPEN_VERIFY_TYPE_MAX; i++)
		m_bAcceptVerifyList[i] = false;
	m_bClientAcceptOpened = false;
#if defined( PRE_PARTY_DB )
	m_bFirstDBConnect = false;
#endif
}

CDNIocpManager::~CDNIocpManager(void)
{
	Final();
	
	CloseHandle( m_hTerminateEvent );
}

UINT __stdcall CDNIocpManager::ReconnectThread(void *pParam)
{
	// 2009.01.20 김밥
	// graceful종료하게 코드 수정

	CDNIocpManager *pIocp = (CDNIocpManager*)pParam;

	DWORD CurTick = 0;

	while(true)
	{
		if( WaitForSingleObject( pIocp->GetTerminateEvent(), 500 ) == WAIT_OBJECT_0 )
			break;

		CurTick = timeGetTime();
		if (g_pMasterConnectionManager) g_pMasterConnectionManager->Reconnect(CurTick);
		if (g_pDBConnectionManager) g_pDBConnectionManager->Reconnect(CurTick);
		if (g_pLogConnection) g_pLogConnection->Reconnect(CurTick);
		if (g_pServiceConnection) 
		{
			if (g_pMasterConnectionManager && g_pMasterConnectionManager->IsAllWorldMasterConnected() && pIocp->m_bClientAcceptOpened)
				g_pServiceConnection->Reconnect(CurTick);
			else if( g_pServiceConnection->GetActive() )
				g_Log.Log(LogType::_ERROR, L"Not Report ServiceManager [%d] \r\n", g_pServiceConnection->GetManagedID());
		}
		if (g_pAuthManager) g_pAuthManager->Run(CurTick);
		if (g_pGuildManager) {
			g_pGuildManager->DoUpdate(CurTick);
		}
		if (g_pCashConnection) g_pCashConnection->Reconnect();
		if (g_pPeriodQuestSystem) g_pPeriodQuestSystem->DoUpdate(CurTick);
#if defined (PRE_ADD_DONATION)
		if (CDNDonationScheduler::IsActive())
			CDNDonationScheduler::GetInstance().DoUpdate(CurTick);
#endif // #if defined (PRE_ADD_DONATION)

#if defined(_KRAZ)
		if (g_pActozShield)
			g_pActozShield->RepeatCheckerResult(CurTick);
#endif	// #if defined(_KRAZ)

		Sleep(500);
	}
	return 0;
}

int CDNIocpManager::Init(int nSocketCountMax, int nWorkerThreadSize)
{
#if defined(_GAMESERVER) && defined(_AUTH_)
	if (!isValidated())
		return -1;
#endif
	CIocpManager::Init(nSocketCountMax, nWorkerThreadSize);
	return CreateThread();
}

void CDNIocpManager::Final()
{
	CIocpManager::Final();
}

// 2008.01.20 김밥
// ClearApp() 와 ReconnectThread 간에 동기화 이슈로 별도의 쓰레드 종료 함수 만듬
void CDNIocpManager::FinalReconnectThread()
{
	// ReconnectThread 종료
	if( m_hReconnectThread != INVALID_HANDLE_VALUE )
	{
		SetEvent( m_hTerminateEvent );
		WaitForSingleObject( m_hReconnectThread, INFINITE );
		
		CloseHandle( m_hReconnectThread );
		m_hReconnectThread = INVALID_HANDLE_VALUE;
	}
}

int CDNIocpManager::CreateThread()
{
	UINT ThreadID;

	m_hReconnectThread = (HANDLE)_beginthreadex(NULL, 0, &ReconnectThread, this, 0, &ThreadID);
	if (m_hReconnectThread == INVALID_HANDLE_VALUE) 
		return -1;

	return 0;
}

#if defined( PRE_WORLDCOMBINE_PARTY )
void CDNIocpManager::VerifyAccept(int nVerifyType, BYTE cWorldSetID/*=0*/ )
#else
void CDNIocpManager::VerifyAccept(int nVerifyType)
#endif // #if defined( PRE_WORLDCOMBINE_PARTY )
{
	m_bAcceptVerifyList[nVerifyType] = true;

	for (int i = 0; i < ACCEPTOPEN_VERIFY_TYPE_MAX; i++)
	{
		if (m_bAcceptVerifyList[i] == false)
			return;
	}

	if (m_bClientAcceptOpened == false)
	{
		if (AddAcceptConnection(CONNECTIONKEY_USER, g_Config.nClientAcceptPort, SOMAXCONN) < 0)
			g_Log.Log(LogType::_ERROR, L"ClientAcceptPort Fail(%d)\r\n", g_Config.nClientAcceptPort);
		else
		{
			m_bClientAcceptOpened = true;
			g_Log.Log(LogType::_NORMAL, L"ClientAcceptPort (%d)\r\n", g_Config.nClientAcceptPort);
		}
	}

#if defined( PRE_WORLDCOMBINE_PARTY )
	g_pMasterConnectionManager->SendConnectComplete( cWorldSetID );
#else
	g_pMasterConnectionManager->SendConnectComplete();
#endif // #if defined( PRE_WORLDCOMBINE_PARTY )
}

void CDNIocpManager::OnAccept(CSocketContext *pSocketContext, const char* pIp, const int nPort)
{
	switch(pSocketContext->m_dwKeyParam)
	{
	case CONNECTIONKEY_USER:
		{
			CDNTcpConnection * pCon = g_pGameServerManager->CreateTcpConnection(pIp, nPort);
			if (pCon == NULL)	return ;

			pSocketContext->SetParam(pCon);
			pCon->SetSocketContext(this, pSocketContext);
			pCon->SetServerConnection(false);

#ifdef _DEBUG
			g_Log.Log( LogType::_FILELOG, L"[OnAccept:User] Socket:%d, Ip:%S\r\n", pSocketContext->m_Socket, pCon->GetIp());
#endif
		}
		break;

	case CONNECTIONKEY_CASH:
		{
			if (!g_pCashConnection) return;

			pSocketContext->SetParam(g_pCashConnection);
			g_pCashConnection->SetSocketContext(this, pSocketContext);
			g_pCashConnection->SetIp(pIp);
			g_pCashConnection->SetPort(nPort);

			g_Log.Log( LogType::_NORMAL, L"[OnAccept:Cash] Socket:%d, Ip:%S\r\n", pSocketContext->m_Socket, g_pCashConnection->GetIp());
		}
		break;

	case CONNECTIONKEY_MASTER:
		{
			CDNMasterConnection *pMasterCon = (CDNMasterConnection*)g_pMasterConnectionManager->GetConnectionByIPPort(pIp, nPort);
			if (!pMasterCon) return;

			pSocketContext->SetParam(pMasterCon);
			pMasterCon->SetSocketContext(this, pSocketContext);
			pMasterCon->SetServerConnection(false);

			g_Log.Log(LogType::_NORMAL, L"[OnAccept:Master] Socket:%d, Ip:%S\r\n", pSocketContext->m_Socket, pMasterCon->GetIp());
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
			pSocketContext->SetParam(g_pLogConnection);
			g_pLogConnection->SetSocketContext(this, pSocketContext);
			g_pLogConnection->SetServerConnection(false);

			g_Log.Log( LogType::_NORMAL, L"[OnAccept:Log] Socket:%d, Ip:%S\r\n", pSocketContext->m_Socket, g_pLogConnection->GetIp());
		}
		break;

	case CONNECTIONKEY_SERVICEMANAGER:
		{
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
			CDNTcpConnection * pCon = (CDNTcpConnection*)pSocketContext->GetParam();
			if (pCon == NULL)	return;

			pCon->SetActive(true);
			pCon->SetDelete(false);
			//pCon->BufferClear();

#ifdef _DEBUG
			g_Log.Log(LogType::_FILELOG, L"[OnConnected:User] Socket:%d\r\n", pSocketContext->m_Socket);
#endif
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
			CDNMasterConnection *pMasterCon = (CDNMasterConnection*)pSocketContext->GetParam();
			if (!pMasterCon) return;

			pMasterCon->SetActive(true);
			pMasterCon->SetDelete(false);			

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
#if defined( PRE_PARTY_DB )
			if(!m_bFirstDBConnect)
			{
				pDBCon->QueryDelPartyForGameServer( 0, g_Config.nManagedID );
#if defined( PRE_WORLDCOMBINE_PVP )
				pDBCon->QueryDelWorldPvPRoomForServer( g_Config.nManagedID );
#endif
				m_bFirstDBConnect = true;
			}
#endif

			if (g_pMasterConnectionManager && g_pEvent)
			{
				std::vector <int> vList;
				g_pMasterConnectionManager->GetConnectedWorldID(&vList);

				for (int i = 0; i < (int)vList.size(); i++)
				{
					if (g_pEvent->IsInitWorld(vList[i]) == false)
						pDBCon->QueryEventList(rand()%THREADMAX,vList[i]);

					if (g_pPeriodQuestSystem)
						g_pPeriodQuestSystem->LoadWorldQuestInfo(vList[i]);
#if defined (PRE_ADD_DONATION)
					if (CDNDonationScheduler::IsActive())
						CDNDonationScheduler::GetInstance().AddWorldID(vList[i]);
#endif // #if defined (PRE_ADD_DONATION)
#if defined( PRE_PRIVATECHAT_CHANNEL )
					if( g_pPrivateChatChannelManager && g_pPrivateChatChannelManager->GetFirst() == false )
					{						
						pDBCon->QueryGetPrivateChatChannelInfo(0, vList[i]);
						pDBCon->QueryDelPrivateMemberServerID(0, vList[i], g_Config.nManagedID );
						g_pPrivateChatChannelManager->SetFirst(true);
					}
#endif
				}
			}
			g_Log.Log(LogType::_NORMAL, L"[OnConnected:DB] Socket:%d\r\n", pSocketContext->m_Socket);
		}
		break;

	case CONNECTIONKEY_LOG:
		{
			g_pLogConnection->SetActive(true);
			g_pLogConnection->SetDelete(false);
		}
		break;

	case CONNECTIONKEY_SERVICEMANAGER:
		{
			g_pServiceConnection->SetActive(true);
			g_pServiceConnection->SetDelete(false);
			//g_pServiceConnection->OnConnected();
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
	case CONNECTIONKEY_DB :
		{
#if defined( STRESS_TEST )
#else
			g_Log.Log(LogType::_ERROR, L"DBServer Fail(%S, %d)\r\n", pConnection->GetIp(), pConnection->GetPort());
#endif // #if defined( STRESS_TEST )
		}
		break;
	case  CONNECTIONKEY_MASTER :
		{
#if defined( STRESS_TEST )
#else
			g_Log.Log(LogType::_ERROR, L"MasterServer Fail(%S, %d)\r\n", pConnection->GetIp(), pConnection->GetPort());
#endif // #if defined( STRESS_TEST )
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

			CDNTcpConnection * pCon = (CDNTcpConnection*)pSocketContext->GetParam();
			if (pCon == NULL) return;
			std::wstring wstrDetachReason;
			pSocketContext->GetDetachReason(wstrDetachReason);
			pCon->TcpDisconnected((wstrDetachReason.size() > 0) ? wstrDetachReason.c_str() : L"OnDisconnected");
		}
		break;

	case CONNECTIONKEY_CASH:
		{
			if (!g_pCashConnection) return;

			g_pCashConnection->SetActive(false);
			g_pCashConnection->SetDelete(true);

			g_Log.Log(LogType::_NORMAL, L"[OnDisconnected:Cash] Socket:%d\r\n", pSocketContext->m_Socket);
			ClearSocketContext(pSocketContext);
			g_pCashConnection->SetSocketContext(NULL, NULL);		
		}
		break;

	case CONNECTIONKEY_MASTER:
		{
			CDNMasterConnection *pMasterCon = (CDNMasterConnection*)pSocketContext->GetParam();
			if (!pMasterCon) return;

			g_pGameServerManager->MasterDisConnected(pMasterCon->m_cWorldSetID);

			pMasterCon->SetActive(false);
			pMasterCon->SetDelete(true);
			pMasterCon->SetInComplete();

			g_Log.Log(LogType::_NORMAL, L"[OnDisconnected:Master] Socket:%d\r\n", pSocketContext->m_Socket);
			ClearSocketContext(pSocketContext);
			pMasterCon->SetSocketContext(NULL, NULL);
		}
		break;
	
	case CONNECTIONKEY_DB:
		{
			CDNDBConnection *pDBCon = (CDNDBConnection*)pSocketContext->GetParam();
			if (!pDBCon) return;

			pDBCon->SetActive(false);
			pDBCon->SetDelete(true);
			g_Log.Log(LogType::_NORMAL, L"[OnDisconnected:DB] Socket:%d\r\n", pSocketContext->m_Socket);
			ClearSocketContext(pSocketContext);
			pDBCon->SetSocketContext(NULL, NULL);
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
	CConnection *pCon = (CConnection*)pSocketContext->GetParam();
	if (pCon == NULL)
	{
		_DANGER_POINT();
		return;
	}	
	
	if (pSocketContext->m_dwKeyParam != CONNECTIONKEY_USER)
	{
		if( pCon->AddRecvData( pSocketContext ) < 0 )
		{
			DetachSocket(pSocketContext, L"OnReceive");
			return;
		}

		pCon->FlushRecvData(0);
	}
#ifdef _SKIP_BLOCK
	else
	{
		CDNTcpConnection * pTcpCon = (CDNTcpConnection*)pSocketContext->GetParam();
		if( pTcpCon->AddRecvData( pSocketContext ) < 0 )
		{
			if (pTcpCon->IsAttachedToSession() == false)
			{
#if defined( PRE_FIX_SOCKETCONTEXT_DANGLINGPTR )
				DWORD dwTime = timeGetTime();
				bool bDelete = false;
#if defined( _WORK )
				while( true )
#else
				while( timeGetTime()-dwTime <= 1000 )
#endif // #if defined( _WORK )
				{
					if( InterlockedCompareExchange( &pSocketContext->m_lActiveCount, CSocketContext::DeleteFlag, 0 ) == 0 )
					{
						bDelete = true;
						g_Log.Log(LogType::_ERROR, L"OnReceive AddRecvData Err" );
						if( pTcpCon->GetUserSession() )
							pTcpCon->GetUserSession()->SetTcpConnection(NULL);
						g_pIocpManager->DelSocket(pSocketContext);
						pTcpCon->SetSocketContext(NULL, NULL);
						SAFE_DELETE(pTcpCon);
						ClearSocketContext(pSocketContext);
#if defined( PRE_FIX_SOCKETCONTEXT_DANGLINGPTR ) && defined( _WORK)
						std::cout << "삭제삭제오예~~~~~" << std::endl;
#endif // #if defined( PRE_FIX_SOCKETCONTEXT_DANGLINGPTR ) && defined( _WORK)
						break;
					}
					else
					{
#if defined( _WORK)
						std::cout << "댕글댕글댕글링~~~" << std::endl;
#else
						g_Log.Log(LogType::_ERROR, L"[%d] GameServer Occur DanglingPointer!!!\r\n", g_Config.nManagedID );
#endif // #if defined( _WORK)
					}
				}

				if( bDelete == false )
				{
					g_Log.Log(LogType::_ERROR, L"[%d] GameServer DanglingPointer Delete Fail!!!\r\n", g_Config.nManagedID );
				}

#else
				g_Log.Log(LogType::_ERROR, L"OnReceive AddRecvData Err" );
				g_pIocpManager->DelSocket(pTcpCon->GetSocketContext());
				ClearSocketContext(pTcpCon->GetSocketContext());
				pTcpCon->SetSocketContext(NULL, NULL);
				SAFE_DELETE(pTcpCon);
#endif // #if defined( PRE_FIX_SOCKETCONTEXT_DANGLINGPTR )
			}
			else
				DetachSocket(pSocketContext, L"OnReceive");
			return;
		}

		if (pTcpCon->IsAttachedToSession() == false)
		{
			if (pCon->FlushRecvData(0) && pTcpCon->GetAttachVerify() )
				pTcpCon->SetAttachSession();
			else
			{
#if defined( PRE_FIX_SOCKETCONTEXT_DANGLINGPTR )
				DWORD dwTime = timeGetTime();
				bool bDelete = false;
#if defined( _WORK )
				while( true )
#else
				while( timeGetTime()-dwTime <= 1000 )
#endif // #if defined( _WORK )
				{
					if( InterlockedCompareExchange( &pSocketContext->m_lActiveCount, CSocketContext::DeleteFlag, 0 ) == 0 )
					{
						bDelete = true;
						g_Log.Log(LogType::_ERROR, L"TcpConnect Err" );
						if( pTcpCon->GetUserSession() )
							pTcpCon->GetUserSession()->SetTcpConnection(NULL);
						g_pIocpManager->DelSocket(pSocketContext);
						pTcpCon->SetSocketContext(NULL, NULL);
						SAFE_DELETE(pTcpCon);
						ClearSocketContext(pSocketContext);
#if defined( PRE_FIX_SOCKETCONTEXT_DANGLINGPTR ) && defined( _WORK)
						std::cout << "삭제삭제오예~~~~~" << std::endl;
#endif // #if defined( PRE_FIX_SOCKETCONTEXT_DANGLINGPTR ) && defined( _WORK)
						break;
					}
					else
					{
#if defined( _WORK)
						std::cout << "댕글댕글댕글링~~~" << std::endl;
#else
						g_Log.Log(LogType::_ERROR, L"[%d] GameServer Occur DanglingPointer!!!\r\n", g_Config.nManagedID );
#endif // #if defined( _WORK)
					}
				}

				if( bDelete == false )
				{
					g_Log.Log(LogType::_ERROR, L"[%d] GameServer DanglingPointer Delete Fail!!!\r\n", g_Config.nManagedID );
				}

#else
				g_Log.Log(LogType::_ERROR, L"TcpConnect Err");
				g_pIocpManager->DelSocket(pTcpCon->GetSocketContext());
				ClearSocketContext(pTcpCon->GetSocketContext());
				pTcpCon->SetSocketContext(NULL, NULL);
				SAFE_DELETE(pTcpCon);
#endif // #if defined( PRE_FIX_SOCKETCONTEXT_DANGLINGPTR )
			}
		}
	}
#endif
}



