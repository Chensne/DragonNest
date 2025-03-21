#include "StdAfx.h"
#include "Connection.h"
#include "DNIocpManager.h"
#include "DNMasterConnection.h"
#include "DNMasterConnectionManager.h"
#include "DNUserConnection.h"
#include "DNUserConnectionManager.h"
#include "DNExtManager.h"
#include "Log.h"
#include "DNLogConnection.h"
#include "DNServiceConnection.h"
#include "DNAuthManager.h"

#include "DNSQLMembershipManager.h"
#include "DNSQLWorldManager.h"

#ifdef _USE_ACCEPTEX
#include "SocketContextMgr.h"
#endif

#if defined(_KRAZ)
#include "DNActozShield.h"
#endif	// #if defined(_KRAZ)

#if defined(_TW)
#include "DNGamaniaAuth.h"
#endif	// _TW

#if defined(_TH)
#include "DNAsiaSoftAuth.h"
#include "DNAsiaSoftOTP.h"
#endif //#if defined(_TH)

CDNIocpManager* g_pIocpManager;

#ifndef _USE_ACCEPTEX
HANDLE CDNIocpManager::m_hSignalProcess = INVALID_HANDLE_VALUE;
#endif

extern TLoginConfig g_Config;

CDNIocpManager::CDNIocpManager(void): CIocpManager()
{
	Final();

#ifdef _USE_ACCEPTEX
	m_hReconnectThread	= INVALID_HANDLE_VALUE;
	m_hUpdateThread		= INVALID_HANDLE_VALUE;
#else
	m_hSignalProcess = CreateEvent(NULL, TRUE, FALSE, NULL);
#endif
}

CDNIocpManager::~CDNIocpManager(void)
{
	Final();
}

#ifndef _USE_ACCEPTEX

UINT __stdcall CDNIocpManager::ProcessThread(void *pParam)
{
	g_Log.Log(LogType::_FILELOG, L"[Thread-Start] ProcessThread - TID : %d\r\n", ::GetCurrentThreadId());

	CDNIocpManager *pIocp = (CDNIocpManager*)pParam;

	CSocketContext *pSocketContext;
	CConnection *pCon = NULL;
	USHORT nRecv = 0;

	while(pIocp->m_bThreadSwitch)
	{
		if (!pIocp->m_ProcessCalls.empty())
		{
			pIocp->m_ProcessLock.Lock();
			pSocketContext = pIocp->m_ProcessCalls.front();
			pIocp->m_ProcessCalls.pop();
			pIocp->m_ProcessLock.UnLock();

			pCon = (CConnection*)pSocketContext->GetParam();

#if defined(_TW)
			if (pCon && 
				(
					pSocketContext->m_dwKeyParam == CONNECTIONKEY_TW_AUTHLOGIN || 
					pSocketContext->m_dwKeyParam == CONNECTIONKEY_TW_AUTHLOGOUT
				))
			{
				pCon->FlushAuthData_TW();
				continue;
			}
#endif	// #if defined(_TW)

			if (pCon && pCon->FlushRecvData() == false)
			{
				if (pSocketContext->m_dwKeyParam == CONNECTIONKEY_USER){
					g_pUserConnectionManager->DelConnection(pCon);
				}
				else if (pSocketContext->m_dwKeyParam == CONNECTIONKEY_MASTER){
					g_pMasterConnectionManager->DelConnection(pCon);
					g_Log.Log(LogType::_NORMAL, pCon, L"## Master IN_DISCONNECT connection destroyed %x World:%d\r\n", pCon, pCon->GetSessionID());
				}

#ifdef _USE_ACCEPTEX
				CSocketContextMgr::GetInstance().OnDisconnect( pSocketContext );
#endif
				pIocp->ClearSocketContext(pSocketContext);
				pCon->SetSocketContext(NULL, NULL);
				SAFE_DELETE(pCon);
			}
		}
		// 프로세스큐가 비면 논시그널로 바꾸고 신호대기
		else
		{
			ResetEvent(m_hSignalProcess);
			::WaitForSingleObject(m_hSignalProcess, 1000);
		}
	}

	g_Log.Log(LogType::_FILELOG, L"[Thread-End] ProcessThread - TID : %d\r\n", ::GetCurrentThreadId());

	return 0;
}

#endif

UINT __stdcall CDNIocpManager::UpdateThread(void *pParam)
{
	g_Log.Log(LogType::_FILELOG, L"[Thread-Start] UpdateThread - TID : %d\r\n", ::GetCurrentThreadId());

	CDNIocpManager *pIocp = (CDNIocpManager*)pParam;

	DWORD CurTick = 0;
	int nCount = 0;

	while(pIocp->m_bThreadSwitch){
		CurTick = timeGetTime();
		if (g_pUserConnectionManager) g_pUserConnectionManager->DoUpdate(CurTick);

		if (g_pSQLMembershipManager)
			g_pSQLMembershipManager->DoUpdate(CurTick);
		if (g_pSQLWorldManager)
			g_pSQLWorldManager->DoUpdate(CurTick);
		
#if defined(_TW) && defined(_FINAL_BUILD)
		if (g_pGamaniaAuthLogin) {
			g_pGamaniaAuthLogin->DoUpdate(CurTick);
		}
		if (g_pGamaniaAuthLogOut) {
			g_pGamaniaAuthLogOut->DoUpdate(CurTick);
		}
#endif	// #if defined(_TW)

		Sleep(1);
	}

	g_Log.Log(LogType::_FILELOG, L"[Thread-End] UpdateThread - TID : %d\r\n", ::GetCurrentThreadId());

	return 0;
}

extern void SaveUserSessionID();
UINT __stdcall CDNIocpManager::ReconnectThread(void *pParam)
{
	g_Log.Log(LogType::_FILELOG, L"[Thread-Start] ReconnectThread - TID : %d\r\n", ::GetCurrentThreadId());

	CDNIocpManager *pIocp = (CDNIocpManager*)pParam;

	DWORD CurTick = 0;

	while(pIocp->m_bThreadSwitch){
		CurTick = timeGetTime();
		
		if (g_pLogConnection) g_pLogConnection->Reconnect(CurTick);
		if (g_pServiceConnection) g_pServiceConnection->Reconnect(CurTick);
		if (g_pAuthManager) g_pAuthManager->Run(CurTick);

#if defined(_KRAZ) && defined(_FINAL_BUILD)
		if (g_pActozShield)
			g_pActozShield->RepeatCheckerResult(CurTick);
#endif	// #if defined(_KRAZ)
#if defined(_TW) && defined(_FINAL_BUILD)
		if (g_pGamaniaAuthLogin) {
			g_pGamaniaAuthLogin->Reconnect(CurTick);
		}
		if (g_pGamaniaAuthLogOut) {
			g_pGamaniaAuthLogOut->Reconnect(CurTick);
		}
#endif	// #if defined(_TW)
#if defined(_TH) && defined(_FINAL_BUILD)
		if (g_pAsiaSoftAuth)
			g_pAsiaSoftAuth->Reconnect(CurTick);		
#endif	//#if defined(_TH) && defined(_FINAL_BUILD)

		static int nCount = 0;
		if (nCount == 0 || (nCount%(1000*3)) == 0){
			if (g_IDGenerator.IsUserConnectionSet()) {	// LO 초기화 시 세션 ID 로드보다 먼저 호출되어 세션 ID 값을 무조건 0 으로 초기화시키는 문제가 있어서 추가 (20100215 b4nfter)
				SaveUserSessionID();	// 여기에 세들어서(?) 저장좀... 080918 saset
			}
		}
		nCount++;

		Sleep(1);
	}

	g_Log.Log(LogType::_FILELOG, L"[Thread-End] ReconnectThread - TID : %d\r\n", ::GetCurrentThreadId());

	return 0;
}

int CDNIocpManager::Init(int nSocketCountMax, int nWorkerThreadCount )
{
	if( CIocpManager::Init(nSocketCountMax, nWorkerThreadCount) < 0)
		return -1;
	return CreateThread();
}

void CDNIocpManager::Final()
{
	CIocpManager::Final();

#ifndef _USE_ACCEPTEX
	while (!m_ProcessCalls.empty())
	{
		m_ProcessCalls.pop();
	}

	CloseHandle(m_hSignalProcess);
#endif
}

int CDNIocpManager::CreateThread()
{
	UINT ThreadID;

#ifndef _USE_ACCEPTEX
	HANDLE hThread;
	hThread = (HANDLE)_beginthreadex(NULL, 0, &ProcessThread, this, 0, &ThreadID);
	if (hThread == INVALID_HANDLE_VALUE) return -1;
	CloseHandle(hThread);
#endif

#ifdef _USE_ACCEPTEX
	m_hUpdateThread = (HANDLE)_beginthreadex(NULL, 0, &UpdateThread, this, 0, &ThreadID);
	if (m_hUpdateThread == INVALID_HANDLE_VALUE) 
		return -1;

	m_hReconnectThread = (HANDLE)_beginthreadex(NULL, 0, &ReconnectThread, this, 0, &ThreadID);
	if (m_hReconnectThread == INVALID_HANDLE_VALUE) 
		return -1;
#else
	hThread = (HANDLE)_beginthreadex(NULL, 0, &UpdateThread, this, 0, &ThreadID);
	if (hThread == INVALID_HANDLE_VALUE) return -1;
	CloseHandle(hThread);

	hThread = (HANDLE)_beginthreadex(NULL, 0, &ReconnectThread, this, 0, &ThreadID);
	if (hThread == INVALID_HANDLE_VALUE) return -1;
	CloseHandle(hThread);
#endif

	return 0;
}

#ifdef _USE_ACCEPTEX

void CDNIocpManager::PostDisconnect(CSocketContext *pSocketContext)
{
	PostQueuedCompletionStatus( m_hIOCP, 4, (ULONG_PTR)pSocketContext, &pSocketContext->m_RecvIO );
}

void CDNIocpManager::ThreadStop()
{
	CIocpManager::ThreadStop();

	if( m_hUpdateThread != INVALID_HANDLE_VALUE )
	{
		WaitForSingleObject( m_hUpdateThread, INFINITE );
		CloseHandle( m_hUpdateThread );
	}

	if( m_hReconnectThread != INVALID_HANDLE_VALUE )
	{
		WaitForSingleObject( m_hReconnectThread, INFINITE );
		CloseHandle( m_hReconnectThread );
	}
}

#else

void CDNIocpManager::AddProcessCall(CSocketContext *pSocketContext)
{
	m_ProcessLock.Lock();
	m_ProcessCalls.push(pSocketContext);
	SetEvent(m_hSignalProcess);
	m_ProcessLock.UnLock();
}

#endif

void CDNIocpManager::OnAccept(CSocketContext *pSocketContext, const char* pIp, const int nPort)
{
	switch(pSocketContext->m_dwKeyParam)
	{
	case CONNECTIONKEY_USER:
		{
			CDNUserConnection *pUserCon = (CDNUserConnection*)g_pUserConnectionManager->AddConnection(pIp, nPort);
			if (!pUserCon) return;

			pSocketContext->SetParam(pUserCon);
			pUserCon->SetSocketContext(this, pSocketContext);
		}
		break;

	case CONNECTIONKEY_MASTER:
		{
			CDNMasterConnection *pMasterCon = (CDNMasterConnection*)g_pMasterConnectionManager->AddConnection(pIp, nPort);
			if (!pMasterCon) return;

			pSocketContext->SetParam(pMasterCon);
			pMasterCon->SetSocketContext(this, pSocketContext);
			pMasterCon->SetServerConnection(false);

			g_Log.Log(LogType::_FILELOG, L"[OnAccept:Master] Socket:%d, Ip:%S\r\n", pSocketContext->m_Socket, pMasterCon->GetIp());
		}
		break;

	case CONNECTIONKEY_LOG:
		{
			pSocketContext->SetParam(g_pLogConnection);
			g_pLogConnection->SetSocketContext(this, pSocketContext);
			g_pLogConnection->SetServerConnection(false);

			g_Log.Log(LogType::_FILELOG, L"[OnAccept:Log] Socket:%d, Ip:%S\r\n", pSocketContext->m_Socket, g_pLogConnection->GetIp());
		}
		break;

	case CONNECTIONKEY_SERVICEMANAGER:
		{
			pSocketContext->SetParam(g_pServiceConnection);
			g_pServiceConnection->SetSocketContext(this, pSocketContext);
			g_pServiceConnection->SetServerConnection(false);

			g_Log.Log(LogType::_FILELOG, L"[OnAccept:ServiceManager] Socket:%d, Ip:%S\r\n", pSocketContext->m_Socket, g_pServiceConnection->GetIp());
		}
		break;

#if defined(_TW) && defined(_FINAL_BUILD)
	case CONNECTIONKEY_TW_AUTHLOGIN:
		{
			pSocketContext->SetParam(g_pGamaniaAuthLogin);
			g_pGamaniaAuthLogin->SetSocketContext(this, pSocketContext);
			g_pGamaniaAuthLogin->SetServerConnection(false);

			g_Log.Log(LogType::_FILELOG, L"[OnAccept:GamaniaAuthLogin] Socket:%d, Ip:%S, Port:%d\r\n", pSocketContext->m_Socket, g_pGamaniaAuthLogin->GetIp(), g_pGamaniaAuthLogin->GetPort());
		}
		break;

	case CONNECTIONKEY_TW_AUTHLOGOUT:
		{
			pSocketContext->SetParam(g_pGamaniaAuthLogOut);
			g_pGamaniaAuthLogOut->SetSocketContext(this, pSocketContext);
			g_pGamaniaAuthLogOut->SetServerConnection(false);

			g_Log.Log(LogType::_FILELOG, L"[OnAccept:GamaniaAuthLogOut] Socket:%d, Ip:%S, Port:%d\r\n", pSocketContext->m_Socket, g_pGamaniaAuthLogOut->GetIp(), g_pGamaniaAuthLogOut->GetPort());
		}
		break;
#endif	// #if defined(_TW)

#if defined(_TH) && defined(_FINAL_BUILD)
	case CONNECTIONKEY_TH_AUTH:
		{
			pSocketContext->SetParam(g_pAsiaSoftAuth);
			g_pAsiaSoftAuth->SetSocketContext(this, pSocketContext);
			g_pAsiaSoftAuth->SetServerConnection(false);

			g_Log.Log(LogType::_FILELOG, L"[OnAccept:AsiaSoftAuth] Socket:%d, Ip:%S, Port:%d\r\n", pSocketContext->m_Socket, g_pAsiaSoftAuth->GetIp(), g_pAsiaSoftAuth->GetPort());
		}
		break;
		
	case CONNECTIONKEY_TH_OTP:
		{
			CDNAsiaSoftOTP * pOTP = static_cast<CDNAsiaSoftOTP*>(pSocketContext->GetParam());
			if (pOTP)
			{
				pOTP->SetSocketContext(this, pSocketContext);
				pOTP->SetServerConnection(false);

				g_Log.Log(LogType::_FILELOG, L"[OnAccept:AsiaSoftOTP] Socket:%d, Ip:%S, Port:%d\r\n", pSocketContext->m_Socket, pOTP->GetIp(), pOTP->GetPort());
			}
			else
				g_Log.Log(LogType::_FILELOG, L"[OnAccept:AsiaSoftOTP] Accept Fail Socket:%d\r\n", pSocketContext->m_Socket);
		}
		break;

#endif	// #if defined(_TW)
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
			CDNUserConnection *pUserCon = (CDNUserConnection*)pSocketContext->GetParam();
			if (!pUserCon) return;

			pUserCon->SetActive(true);
			pUserCon->SetDelete(false);
		}
		break;

	case CONNECTIONKEY_MASTER:
		{
			CDNMasterConnection *pMasterCon = (CDNMasterConnection*)pSocketContext->GetParam();
			if (!pMasterCon) return;

			pMasterCon->SetActive(true);
			pMasterCon->SetDelete(false);
			//pMasterCon->BufferClear();

			if (pMasterCon->SendRegist(g_pAuthManager->GetServerID()) == false)
			{
				DetachSocket(pSocketContext, L"SendRegist Failed");
				g_Log.Log(LogType::_FILELOG, L"[OnConnected:Master] Socket:%d SendRegist Failed!!!!\r\n", pSocketContext->m_Socket);
			}

			g_Log.Log(LogType::_FILELOG, L"[OnConnected:Master] Socket:%d\r\n", pSocketContext->m_Socket);
		}
		break;

	case CONNECTIONKEY_LOG:
		{
			g_pLogConnection->SetActive(true);
			g_pLogConnection->SetDelete(false);

			g_Log.Log(LogType::_FILELOG, L"[OnConnected:Log] Socket:%d\r\n", pSocketContext->m_Socket);
		}
		break;

	case CONNECTIONKEY_SERVICEMANAGER:
		{
			g_pServiceConnection->SetActive(true);
			g_pServiceConnection->SetDelete(false);

			g_Log.Log(LogType::_FILELOG, L"[OnConnected:ServiceManager] Socket:%d\r\n", pSocketContext->m_Socket);
		}
		break;

#if defined(_TW) && defined(_FINAL_BUILD)
	case CONNECTIONKEY_TW_AUTHLOGIN:
		{
			g_pGamaniaAuthLogin->SetActive(true);
			g_pGamaniaAuthLogin->SetDelete(false);

			g_Log.Log(LogType::_FILELOG, L"[OnConnected:GamaniaAuthLogin] Socket:%d\r\n", pSocketContext->m_Socket);
		}
		break;

	case CONNECTIONKEY_TW_AUTHLOGOUT:
		{
			g_pGamaniaAuthLogOut->SetActive(true);
			g_pGamaniaAuthLogOut->SetDelete(false);

			g_Log.Log(LogType::_FILELOG, L"[OnConnected:GamaniaAuthLogOut] Socket:%d\r\n", pSocketContext->m_Socket);
		}
		break;
#endif	// #if defined(_TW)

#if defined(_TH) && defined(_FINAL_BUILD)
	case CONNECTIONKEY_TH_AUTH:
		{
			g_pAsiaSoftAuth->SetActive(true);
			g_pAsiaSoftAuth->SetDelete(false);

			g_Log.Log(LogType::_FILELOG, L"[OnConnected:AsiasoftAuth] Socket:%d\r\n", pSocketContext->m_Socket);
		}
		break;
	case CONNECTIONKEY_TH_OTP:
		{
			CDNAsiaSoftOTP * pOTP = static_cast<CDNAsiaSoftOTP*>(pSocketContext->GetParam());
			if (!pOTP) return;

			pOTP->SetActive(true);
			pOTP->SetDelete(false);

			pOTP->OnConnected();

			g_Log.Log(LogType::_FILELOG, L"[OnConnected:AsiasoftOTP] Socket:%d\r\n", pSocketContext->m_Socket);
		}
		break;
#endif	//#if defined(_TH) && defined(_FINAL_BUILD)
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
	case CONNECTIONKEY_LOG :
		{
			g_Log.Log(LogType::_FILELOG, L"LogServer Fail(%S, %d)\r\n", pConnection->GetIp(), pConnection->GetPort());
		}
		break;
	case CONNECTIONKEY_SERVICEMANAGER :
		{
			g_Log.Log(LogType::_FILELOG, L"ServiceManager Connect Failed(%S, %d)\r\n", pConnection->GetIp(), pConnection->GetPort());
		}
		break;	
#if defined(_TW) && defined(_FINAL_BUILD)
	case CONNECTIONKEY_TW_AUTHLOGIN:
		{
			g_Log.Log(LogType::_FILELOG, L"AuthLoginServer Fail(%S, %d)\r\n", pConnection->GetIp(), pConnection->GetPort());
		}
		break;
	case CONNECTIONKEY_TW_AUTHLOGOUT:
		{
			g_Log.Log(LogType::_FILELOG, L"AuthLogOutServer Fail(%S, %d)\r\n", pConnection->GetIp(), pConnection->GetPort());
		}
		break;
#elif defined (_TH)
	case CONNECTIONKEY_TH_AUTH:
		{
			g_Log.Log(LogType::_FILELOG, L"AsiaSoftAuth Fail(%S, %d)\r\n", pConnection->GetIp(), pConnection->GetPort());
		}
		break;
	case CONNECTIONKEY_TH_OTP:
		{
			g_Log.Log(LogType::_FILELOG, L"AsiaSoftOTP Fail(%S, %d)\r\n", pConnection->GetIp(), pConnection->GetPort());
		}
		break;
#endif	// #if defined(_TW)
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

			CDNUserConnection *pUserCon = (CDNUserConnection*)pSocketContext->GetParam();
			if (!pUserCon) return;

			if ((pUserCon->GetUserState() != STATE_CONNECTVILLAGE) && (pUserCon->GetUserState() != STATE_CONNECTGAME)) // village, game 로 넘어가서 끊기는 유저가 아님
			{		
				if (pUserCon->IsCertified()) {	// 주의 !!! - 반드시 QUERY_BEGINAUTH, QUERY_CHECKAUTH 를 통과하여 인증을 받은 사용자에 한해서만 인증정보 리셋이 수행되어야 함 !!!

					if (0 != pUserCon->GetAccountDBID()) {	// P.S.> CDNUserConnection::RequestBackButton(...) 을 통해서 이런 상태에 도달할 수 있음
#if defined(_TW) && defined(_FINAL_BUILD)
						{
							USES_CONVERSION;
							if( wcslen(pUserCon->GetAccountName()) != 0 )
							{
								ScopeLock<CSyncLock> Lock(pUserCon->m_SendLogOutLock);
								if( !pUserCon->m_bSendLogOut )
								{
									int iResult = g_pGamaniaAuthLogOut->SendLogout(W2A(pUserCon->GetAccountName()), pUserCon->GetIp());
									if (0 >= iResult) {
										// 오류
									}
									pUserCon->m_bSendLogOut = true;
									g_Log.Log(LogType::_FILELOG, L"[GASH] LogOut OnDisconnected %s, %d\r\n", pUserCon->GetAccountName(), pUserCon->GetSessionID());
								}								
							}
						}
#endif	//#if defined(_TW)
						{
							// 리스트에 넣지말고 즉시 처리하자
							g_pAuthManager->QueryResetAuth(pUserCon->GetWorldSetID(), pUserCon->GetAccountDBID(), pUserCon->GetSessionID());
						}
					}

					pUserCon->ResetCertified();
				}
			}
#if defined(PRE_ADD_LOGIN_USERCOUNT)
			if( pUserCon->m_bSummitUserCount ) 
			{
				g_pUserConnectionManager->DelUserCount();
				pUserCon->m_bSummitUserCount = false;
			}
#endif

			// 2010-10-08 haling QueryResetAuth 때문에 밑으로 이동..
			pUserCon->SendInsideDisconnectPacket(L"OnDisconnected");		// 객체 정리 패킷 생성
		}
		break;

	case CONNECTIONKEY_MASTER:
		{
#if defined( PRE_FIX_SOCKETCONTEXT_DANGLINGPTR )
			CScopeInterlocked Scope( &pSocketContext->m_lActiveCount );
			if( Scope.bIsDelete() )
				break;
#endif // #if defined( PRE_FIX_SOCKETCONTEXT_DANGLINGPTR )

			CDNMasterConnection *pMasterCon = (CDNMasterConnection*)pSocketContext->GetParam();
			if (!pMasterCon) return;

			g_Log.Log( pMasterCon->GetWorldSetID()>0?LogType::_NORMAL:LogType::_FILELOG, L"[OnDisconnect] %x WorldID:%d\r\n", this, pMasterCon->GetWorldSetID() );
			g_pExtManager->SetOnOffServerInfo(pMasterCon->GetWorldSetID(), false);	// on, off 세팅해주자
			//마스터가 죽으면 이 대기열에 거렬있는 인원은 일단은 끊어 버립니다.

#ifdef _USE_ACCEPTEX
			ScopeLock<CSyncLock> Lock( g_pUserConnectionManager->m_ConSync );
#endif
			g_pUserConnectionManager->DetachUserByWorldSetID(pMasterCon->GetWorldSetID());

			DNTPacket Header = { 0, };
			Header.cMainCmd = IN_DISCONNECT;
			Header.iLen = sizeof(USHORT) + (sizeof(BYTE) * 2);

			pMasterCon->BufferClear();
			pMasterCon->SetActive(false);
			pMasterCon->SetDelete(true);
			pMasterCon->AddRecvData( Header );

			g_Log.Log(LogType::_FILELOG, L"[OnDisconnected:Master] Socket:%d\r\n", pSocketContext->m_Socket);
#ifdef _USE_ACCEPTEX
			PostDisconnect( pSocketContext );
#else
			AddProcessCall(pSocketContext);
#endif
		}
		break;

	case CONNECTIONKEY_LOG:
		{
			g_pLogConnection->SetActive(false);
			g_pLogConnection->SetDelete(true);
			ClearSocketContext(pSocketContext);
			g_pLogConnection->SetSocketContext(NULL,NULL);
#if defined(PRE_ADD_LOGSERVER_HEARTBEAT)
			g_pLogConnection->ResetLiveCheck();
#endif
			g_Log.Log(LogType::_FILELOG, L"[OnDisconnected:Log] Socket:%d\r\n", pSocketContext->m_Socket);
		}
		break;

	case CONNECTIONKEY_SERVICEMANAGER:
		{
			g_pServiceConnection->Disconnected();
			g_pServiceConnection->SetActive(false);
			g_pServiceConnection->SetDelete(true);
			ClearSocketContext(pSocketContext);
			g_pServiceConnection->SetSocketContext(NULL,NULL);

			g_Log.Log(LogType::_FILELOG, L"[OnDisconnected:ServiceManager] Socket:%d\r\n", pSocketContext->m_Socket);
		}
		break;

#if defined(_TW) && defined(_FINAL_BUILD)
	case CONNECTIONKEY_TW_AUTHLOGIN:
		{
			g_pGamaniaAuthLogin->SetActive(false);
			g_pGamaniaAuthLogin->SetDelete(true);
			ClearSocketContext(pSocketContext);
			g_pGamaniaAuthLogin->SetSocketContext(NULL,NULL);

			g_Log.Log(LogType::_FILELOG, L"[OnDisconnected:GamaniaAuthLogin] Socket:%d\r\n", pSocketContext->m_Socket);
		}
		break;

	case CONNECTIONKEY_TW_AUTHLOGOUT:
		{
			g_pGamaniaAuthLogOut->SetActive(false);
			g_pGamaniaAuthLogOut->SetDelete(true);
			ClearSocketContext(pSocketContext);
			g_pGamaniaAuthLogOut->SetSocketContext(NULL,NULL);

			g_Log.Log(LogType::_FILELOG, L"[OnDisconnected:GamaniaAuthLogOut] Socket:%d\r\n", pSocketContext->m_Socket);
		}
		break;
#endif	// #if defined(_TW)

#if defined(_TH) && defined(_FINAL_BUILD)
	case CONNECTIONKEY_TH_AUTH:
		{
			g_pAsiaSoftAuth->SetActive(false);
			g_pAsiaSoftAuth->SetDelete(true);
			ClearSocketContext(pSocketContext);
			g_pAsiaSoftAuth->SetSocketContext(NULL,NULL);

			g_Log.Log(LogType::_FILELOG, L"[OnDisconnected:Asiasoft Auth] Socket:%d\r\n", pSocketContext->m_Socket);
		}
		break;

	case CONNECTIONKEY_TH_OTP:
		{
			CDNAsiaSoftOTP * pOTP = static_cast<CDNAsiaSoftOTP*>(pSocketContext->GetParam());
			if (!pOTP) return;

			pOTP->SetActive(false);
			pOTP->SetDelete(true);
			ClearSocketContext(pSocketContext);
			pOTP->SetSocketContext(NULL,NULL);
			pOTP->OnDisconnected();

			g_Log.Log(LogType::_FILELOG, L"[OnDisconnected:Asiasoft OTP] Socket:%d\r\n", pSocketContext->m_Socket);
		}
		break;
#endif	// #if defined(_TH)

	}
}

void CDNIocpManager::OnReceive(CSocketContext *pSocketContext, DWORD dwBytesTransferred)
{
#ifdef _USE_ACCEPTEX

#if defined( PRE_FIX_SOCKETCONTEXT_DANGLINGPTR )
	CScopeInterlocked Scope( &pSocketContext->m_lActiveCount );
	if( Scope.bIsDelete() )
		return;
#endif // #if defined( PRE_FIX_SOCKETCONTEXT_DANGLINGPTR )

	CConnection *pCon = (CConnection*)pSocketContext->GetParam();
	if (pCon) {
		int nResult = pCon->AddRecvData( pSocketContext );

		if (nResult > 0)
		{
#if defined(_KR)
			// CONNECTIONKEY_AUTH 예외처리
			if( pSocketContext->m_dwKeyParam == CONNECTIONKEY_AUTH )
			{
				pCon->FlushAuthData();
				return;
			}
#endif // #if defined(_KR)

#if defined(_TW)
			if (pCon && pSocketContext->m_dwKeyParam == CONNECTIONKEY_TW_AUTHLOGIN) {
				pCon->FlushAuthData_TW();
				return;
			}
			if (pCon && pSocketContext->m_dwKeyParam == CONNECTIONKEY_TW_AUTHLOGOUT) {
				pCon->RecvBufferClear();	// 수신 패킷 무시
				return;
			}
#endif	// #if defined(_TW)

#if defined(_TH)
			if (pCon && (pSocketContext->m_dwKeyParam == CONNECTIONKEY_TH_AUTH))
			{
				pCon->FlushAuthData_TH();
				return;
			}

			if (pCon && (pSocketContext->m_dwKeyParam == CONNECTIONKEY_TH_OTP))
			{
				pCon->FlushOTPData_TH();
				return;
			}
#endif	// #if defined(_TH)
			
			if( pCon->FlushRecvData(timeGetTime()) == false )
			{
#ifdef PRE_FIX_SOCKETCONTEXT_DANGLINGPTR
				while (true)
				{
					if (InterlockedCompareExchange(&pSocketContext->m_lActiveCount, CSocketContext::DeleteFlag, 1) == 1)
					{
#endif		//#ifdef PRE_FIX_SOCKETCONTEXT_DANGLINGPTR

						if( pSocketContext->m_dwKeyParam == CONNECTIONKEY_USER)
						{
							g_pUserConnectionManager->DelConnection(pCon);

							ScopeLock<CSyncLock> Lock( g_pUserConnectionManager->m_ConSync );

							CConnection* pDeleteCon = static_cast<CConnection*>(pSocketContext->GetParam());
							if( pDeleteCon ) {
#if defined(_WORK)
								g_Log.Log(LogType::_NORMAL, L"## User IN_DISCONNECT connection destroyed %x SID:%u\r\n", pDeleteCon, pDeleteCon->GetSessionID());
								g_Log.Log(LogType::_NORMAL, L"UserCount:%d\r\n", g_pUserConnectionManager->GetCount(false));
#endif
							}

							CSocketContextMgr::GetInstance().OnDisconnect( pSocketContext );
							SAFE_DELETE( pDeleteCon );					
							return;
						}
						else if (pSocketContext->m_dwKeyParam == CONNECTIONKEY_MASTER)
						{
							g_pMasterConnectionManager->DelConnection(pCon);

							ScopeLock<CSyncLock> Lock( g_pMasterConnectionManager->m_ConSync );

							CConnection* pDeleteCon = static_cast<CConnection*>(pSocketContext->GetParam());
							if( pDeleteCon )
								g_Log.Log(LogType::_NORMAL, L"## Master IN_DISCONNECT connection destroyed %x World:%d\r\n", pDeleteCon, pDeleteCon->GetSessionID());

							CSocketContextMgr::GetInstance().OnDisconnect( pSocketContext );
							SAFE_DELETE( pDeleteCon );
							return;
						}

						ScopeLock<CSyncLock> Lock( m_DeleteLock );

						CConnection* pDeleteCon = static_cast<CConnection*>(pSocketContext->GetParam());

						CSocketContextMgr::GetInstance().OnDisconnect( pSocketContext );
						SAFE_DELETE( pDeleteCon );

#ifdef PRE_FIX_SOCKETCONTEXT_DANGLINGPTR
						break;
					}
				}
#endif		//#ifdef PRE_FIX_SOCKETCONTEXT_DANGLINGPTR
			}
			else
			{
				if (pSocketContext->m_dwKeyParam == CONNECTIONKEY_USER)
					m_nAddRecvBufSize += dwBytesTransferred;
			}
		}
		else if (nResult == SIZEERR){
			g_Log.Log(LogType::_ERROR, L"[OnReceive SizeError] Socket:%lld Size:%d\r\n", pSocketContext->m_Socket, dwBytesTransferred);
			DetachSocket(pSocketContext, L"OnReceive SizeError");
		}
		else if (nResult == -1){
			g_Log.Log(LogType::_ERROR, L"[OnReceive] Socket:%lld Size:%d\r\n", pSocketContext->m_Socket, dwBytesTransferred);
			DetachSocket(pSocketContext, L"OnReceive");
		}
	}
#else

	CConnection *pCon = (CConnection*)pSocketContext->GetParam();
	if (pCon) {
		int nResult = pCon->AddRecvData( pSocketContext );

		if (nResult > 0){
			AddProcessCall(pSocketContext);

			if (pSocketContext->m_dwKeyParam == CONNECTIONKEY_USER)
				m_nAddRecvBufSize += dwBytesTransferred;
		}
		else if (nResult == SIZEERR){
			g_Log.Log(LogType::_ERROR, L"[OnReceive SizeError] Socket:%lld Size:%d\r\n", pSocketContext->m_Socket, dwBytesTransferred);
			DetachSocket(pSocketContext, L"OnReceive SizeError");
		}
		else if (nResult == -1){
			g_Log.Log(LogType::_ERROR, L"[OnReceive] Socket:%lld Size:%d\r\n", pSocketContext->m_Socket, dwBytesTransferred);
			DetachSocket(pSocketContext, L"OnReceive");
		}
	}

#endif // #ifdef _USE_ACCEPTEX
}

void CDNIocpManager::DetachSocket (CSocketContext *pSocketContext, wchar_t *pIdent)
{
	if (pSocketContext->DelRef() == 0){
		if (pSocketContext->m_dwKeyParam != CONNECTIONKEY_DEFAULT){
			//g_Log.Log(L"Socket:%d [!] DetachSocket (%x) %S \r\n", pSocketContext->m_Socket, pSocketContext->GetParam(), pIdent);
		}

		DelSocket(pSocketContext);
		OnDisconnected(pSocketContext);
	}
}


