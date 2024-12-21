#include "StdAfx.h"
#include "DNIocpManager.h"
#include "DNConnection.h"
#include "DNThreadManager.h"
#include "DNServiceConnection.h"
#include "DNLogConnection.h"
#include "Log.h"
#include "DNUserDeleteManager.h"

#if defined(_KR) || defined(_US)
#include "DNBillingConnectionKR.h"
#elif defined(_KRAZ)
#include "DNActozShield.h"
#include "DNBillingConnectionKRAZ.h"
#elif defined(_TW)
#include "DNBillingConnectionTW.h"
#elif defined(_TH)
#include "DNBillingConnectionTH.h"
#endif

CDNIocpManager* g_pIocpManager = NULL;

CDNIocpManager::CDNIocpManager(void)
{
}

CDNIocpManager::~CDNIocpManager(void)
{
}

UINT __stdcall CDNIocpManager::ReconnectThread(void *pParam)
{
	CDNIocpManager *pIocp = (CDNIocpManager*)pParam;

	DWORD CurTick = 0;

	while(pIocp->m_bThreadSwitch){
		CurTick = timeGetTime();

		if (g_pServiceConnection) g_pServiceConnection->Reconnect(CurTick);
		if (g_pLogConnection) g_pLogConnection->Reconnect(CurTick);
#if defined(_KR) || defined(_US)
		if (g_pBillingConnection){
			g_pBillingConnection->Reconnect(CurTick);
			g_pBillingConnection->DoUpdate(CurTick);
		}
#endif	// _KR
#if defined(_KRAZ)
		if (g_pActozShield)
			g_pActozShield->RepeatCheckerResult(CurTick);
		if (g_pBillingConnection)
			g_pBillingConnection->Reconnect(CurTick);
#endif	// #if defined(_KRAZ)
#if defined(_TW)
		if (g_pBillingPointConnection)		
			g_pBillingPointConnection->Reconnect(CurTick);					
		
		if( g_pBillingShopConnection )
		{
			g_pBillingShopConnection->Reconnect(CurTick);
			g_pBillingShopConnection->DoUpdate(CurTick);
		}
		if( g_pBillingCouponConnection )		
			g_pBillingCouponConnection->Reconnect(CurTick);				
		if( g_pBillingCouponRollBackConnection )
			g_pBillingCouponRollBackConnection->Reconnect(CurTick);
#endif
#if defined(_TH)
		if (g_pBillingPointConnection)		
			g_pBillingPointConnection->Reconnect(CurTick);

		if( g_pBillingShopConnection )		
			g_pBillingShopConnection->Reconnect(CurTick);					
#endif
#if defined(PRE_DEL_ONNULL)
		if( g_pUserDeleteManager )
			g_pUserDeleteManager->DoUpdate();
#if defined(PRE_ADD_HEARTBEAT_TO_SQL)
		if( g_pThreadManager )
			g_pThreadManager->AddThreadCall((CSocketContext*)DNProcessCheck::HeartbeatKey);
#endif //#if defined(PRE_ADD_HEARTBEAT_TO_SQL)
#else // #if defined(PRE_DEL_ONNULL)
		if (g_pUserDeleteManager && g_pThreadManager) 
		{
			g_pThreadManager->AddThreadCall(NULL);	// CThreadManager::WaitProcess() ���� NULL �� ��ȯ�Ͽ� Ÿ�̸ӿ� ���� ����� �� �ֵ��� ��
		}
#endif //#if defined(PRE_DEL_ONNULL)

		Sleep(1000);
	}
	return 0;
}

void CDNIocpManager::OnAccept(CSocketContext *pSocketContext, const char* pIp, const int nPort)
{
	g_Log.Log(LogType::_NORMAL, L"[OnAccept:%d] Socket:%d, Ip:%S, Port:%d\r\n", pSocketContext->m_dwKeyParam, pSocketContext->m_Socket, pIp, nPort);

	switch(pSocketContext->m_dwKeyParam)
	{
	case CONNECTIONKEY_DEFAULT:
		{
			CDNConnection *pCon = new CDNConnection;
			if (!pCon) return;

			pCon->SetIp(pIp);
			pCon->SetPort(nPort);
			pCon->SetServerConnection(true);
			pCon->SetSocketContext(this, pSocketContext);
			pSocketContext->SetParam(pCon);

			g_Log.Log(LogType::_NORMAL, L"[OnAccept] Socket:%d, Ip:%S\r\n", pSocketContext->m_Socket, pCon->GetIp());
		}
		break;

	case CONNECTIONKEY_SERVICEMANAGER:
		{
			pSocketContext->SetParam(g_pServiceConnection);
			g_pServiceConnection->SetSocketContext(this, pSocketContext);
			g_pServiceConnection->SetServerConnection(false);

			g_Log.Log(LogType::_NORMAL, L"[OnAccept:Monitor] Socket:%d, Ip:%S\r\n", pSocketContext->m_Socket, g_pServiceConnection->GetIp());
		}
		break;

	case CONNECTIONKEY_LOG:
		{
			pSocketContext->SetParam(g_pLogConnection);
			g_pLogConnection->SetSocketContext(this, pSocketContext);
			g_pLogConnection->SetServerConnection(false);

			g_Log.Log( LogType::_NORMAL, L"[OnAccept:Log] Socket:%d, Ip:%S\r\n", pSocketContext->m_Socket, g_pLogConnection->GetIp());
			break;
		}

#if defined(_KR) || defined(_US)
	case CONNECTIONKEY_BILLING:
		{
			pSocketContext->SetParam(g_pBillingConnection);
			g_pBillingConnection->SetSocketContext(this, pSocketContext);
			g_pBillingConnection->SetServerConnection(false);

			g_Log.Log(LogType::_NORMAL, L"[OnAccept:Cash] Socket:%d, Ip:%S\r\n", pSocketContext->m_Socket, g_pBillingConnection->GetIp());
		}
		break;
#endif	// _KR
#if defined(_TW)
	case CONNECTIONKEY_TW_QUERY :
		{
			pSocketContext->SetParam(g_pBillingPointConnection);
			g_pBillingPointConnection->SetSocketContext(this, pSocketContext);
			g_pBillingPointConnection->SetServerConnection(false);

			g_Log.Log(LogType::_NORMAL, L"[OnAccept:GASH QUERY] Socket:%d, Ip:%S\r\n", pSocketContext->m_Socket, g_pBillingPointConnection->GetIp());
		}
		break;
	case CONNECTIONKEY_TW_SHOPITEM :
		{
			pSocketContext->SetParam(g_pBillingShopConnection);
			g_pBillingShopConnection->SetSocketContext(this, pSocketContext);
			g_pBillingShopConnection->SetServerConnection(false);

			g_Log.Log(LogType::_NORMAL, L"[OnAccept:GASH SHOP] Socket:%d, Ip:%S\r\n", pSocketContext->m_Socket, g_pBillingShopConnection->GetIp());
		}
		break;
	case CONNECTIONKEY_TW_COUPON :
		{
			pSocketContext->SetParam(g_pBillingCouponConnection);
			g_pBillingCouponConnection->SetSocketContext(this, pSocketContext);
			g_pBillingCouponConnection->SetServerConnection(false);

			g_Log.Log(LogType::_NORMAL, L"[OnAccept:GASH COUPON] Socket:%d, Ip:%S\r\n", pSocketContext->m_Socket, g_pBillingCouponConnection->GetIp());
		}
		break;
	case CONNECTIONKEY_TW_COUPON_ROLLBACK :
		{
			pSocketContext->SetParam(g_pBillingCouponRollBackConnection);
			g_pBillingCouponRollBackConnection->SetSocketContext(this, pSocketContext);
			g_pBillingCouponRollBackConnection->SetServerConnection(false);

			g_Log.Log(LogType::_NORMAL, L"[OnAccept:GASH COUPON ROLLBACK] Socket:%d, Ip:%S\r\n", pSocketContext->m_Socket, g_pBillingCouponRollBackConnection->GetIp());
		}
		break;
#endif //#if defined(_TW)
#if defined(_TH)
	case CONNECTIONKEY_TH_QUERY :
		{
			pSocketContext->SetParam(g_pBillingPointConnection);
			g_pBillingPointConnection->SetSocketContext(this, pSocketContext);
			g_pBillingPointConnection->SetServerConnection(false);

			g_Log.Log(LogType::_NORMAL, L"[OnAccept:AsiaSoft QUERY] Socket:%d, Ip:%S\r\n", pSocketContext->m_Socket, g_pBillingPointConnection->GetIp());
		}
		break;
	case CONNECTIONKEY_TH_SHOPITEM :
		{
			pSocketContext->SetParam(g_pBillingShopConnection);
			g_pBillingShopConnection->SetSocketContext(this, pSocketContext);
			g_pBillingShopConnection->SetServerConnection(false);

			g_Log.Log(LogType::_NORMAL, L"[OnAccept:AsiaSoft SHOP] Socket:%d, Ip:%S\r\n", pSocketContext->m_Socket, g_pBillingShopConnection->GetIp());
		}
		break;
#endif
	}
}

void CDNIocpManager::OnConnected(CSocketContext *pSocketContext)
{
	switch(pSocketContext->m_dwKeyParam)
	{
	case CONNECTIONKEY_DEFAULT:
		{
			CConnection *pCon = (CConnection*)pSocketContext->GetParam();
			pCon->SetActive(true);
			pCon->SetDelete(false);
			g_Log.Log(LogType::_NORMAL, L"[OnConnected] Socket:%d, Ip:%S\r\n", pSocketContext->m_Socket, pCon->GetIp());
		}
		break;

	case CONNECTIONKEY_SERVICEMANAGER:
		{
			g_pServiceConnection->SetActive(true);
			g_pServiceConnection->SetDelete(false);
		}
		break;

	case CONNECTIONKEY_LOG:
		{
			g_pLogConnection->SetActive(true);
			g_pLogConnection->SetDelete(false);
		}
		break;

#if defined(_KR) || defined(_US)
	case CONNECTIONKEY_BILLING:
		{
			g_pBillingConnection->SetActive(true);
			g_pBillingConnection->SetDelete(false);

			g_pBillingConnection->SendInitialize();

			g_Log.Log(LogType::_NORMAL, L"[OnConnected:Cash] Socket:%d\r\n", pSocketContext->m_Socket);
		}
		break;
#endif	// _KR
#if defined(_TW)
	case CONNECTIONKEY_TW_QUERY:
		{
			g_pBillingPointConnection->SetActive(true);
			g_pBillingPointConnection->SetDelete(false);			

			g_Log.Log(LogType::_NORMAL, L"[OnConnected:GASH QUERY] Socket:%d\r\n", pSocketContext->m_Socket);
		}
		break;
	case CONNECTIONKEY_TW_SHOPITEM:
		{
			g_pBillingShopConnection->SetActive(true);
			g_pBillingShopConnection->SetDelete(false);			

			g_Log.Log(LogType::_NORMAL, L"[OnConnected:GASH SHOP] Socket:%d\r\n", pSocketContext->m_Socket);
		}
		break;
	case CONNECTIONKEY_TW_COUPON:
		{
			g_pBillingCouponConnection->SetActive(true);
			g_pBillingCouponConnection->SetDelete(false);			

			g_Log.Log(LogType::_NORMAL, L"[OnConnected:GASH COUPON] Socket:%d\r\n", pSocketContext->m_Socket);
		}
		break;
	case CONNECTIONKEY_TW_COUPON_ROLLBACK:
		{
			g_pBillingCouponRollBackConnection->SetActive(true);
			g_pBillingCouponRollBackConnection->SetDelete(false);			

			g_Log.Log(LogType::_NORMAL, L"[OnConnected:GASH COUPON ROLLBACK] Socket:%d\r\n", pSocketContext->m_Socket);
		}
		break;
#endif
#if defined(_TH)
	case CONNECTIONKEY_TH_QUERY:
		{
			g_pBillingPointConnection->SetActive(true);
			g_pBillingPointConnection->SetDelete(false);			

			g_Log.Log(LogType::_NORMAL, L"[OnConnected:AsiaSoft QUERY] Socket:%d\r\n", pSocketContext->m_Socket);
		}
		break;
	case CONNECTIONKEY_TH_SHOPITEM:
		{
			g_pBillingShopConnection->SetActive(true);
			g_pBillingShopConnection->SetDelete(false);			

			g_Log.Log(LogType::_NORMAL, L"[OnConnected:AsiaSoft SHOP] Socket:%d\r\n", pSocketContext->m_Socket);
		}
		break;
#endif
	}
}
void CDNIocpManager::OnConnectFail(CSocketContext *pSocketContext)
{
	// �������� Ŀ��Ʈ�� �õ� �ϴ� �ֵ鸸 �־��ּ���..AddConnectionEx�� ȣ���ϴ� �ֵ�?
	CConnection* pConnection = static_cast<CConnection*>(pSocketContext->GetParam());
	if( !pConnection )
		return;

	pConnection->SetConnecting(false);

	switch(pSocketContext->m_dwKeyParam)
	{
	case CONNECTIONKEY_LOG:
		{
			// g_Log.Log(_ERROR, L"LogServer Fail(%S, %d)\r\n", pConnection->GetIp(), pConnection->GetPort());
		}
		break;
	case CONNECTIONKEY_SERVICEMANAGER :
		{
			g_Log.Log(LogType::_ERROR, L"ServiceManager Connect Failed(%S, %d)\r\n", pConnection->GetIp(), pConnection->GetPort());
		}
		break;
#if defined(_KR) || defined(_US)
	case CONNECTIONKEY_BILLING:
		{
			g_Log.Log(LogType::_ERROR, L"BillingServer Fail(%S, %d)\r\n", pConnection->GetIp(), pConnection->GetPort());			
		}
		break;
#endif	// _KR
#if defined(_TW)
	case CONNECTIONKEY_TW_QUERY:
		{
			g_Log.Log(LogType::_ERROR, L"BillingQueryServer Fail(%S, %d)\r\n", pConnection->GetIp(), pConnection->GetPort());
		}
		break;
	case CONNECTIONKEY_TW_SHOPITEM:
		{
			g_Log.Log(LogType::_ERROR, L"BillingShopServer Fail(%S, %d)\r\n", pConnection->GetIp(), pConnection->GetPort());
		}
		break;
	case CONNECTIONKEY_TW_COUPON:
		{
			g_Log.Log(LogType::_ERROR, L"BillingCouponServer Fail(%S, %d)\r\n", pConnection->GetIp(), pConnection->GetPort());
		}
		break;
	case CONNECTIONKEY_TW_COUPON_ROLLBACK:
		{
			g_Log.Log(LogType::_ERROR, L"BillingCouponRollbackServer Fail(%S, %d)\r\n", pConnection->GetIp(), pConnection->GetPort());
		}
		break;
#endif //_TW
#if defined(_TH)
	case CONNECTIONKEY_TH_QUERY:
		{
			g_Log.Log(LogType::_ERROR, L"BillingQueryServer Fail(%S, %d)\r\n", pConnection->GetIp(), pConnection->GetPort());
		}
		break;
	case CONNECTIONKEY_TH_SHOPITEM:
		{
			g_Log.Log(LogType::_ERROR, L"BillingShopServer Fail(%S, %d)\r\n", pConnection->GetIp(), pConnection->GetPort());
		}
		break;
#endif
	}
}

void CDNIocpManager::OnDisconnected(CSocketContext *pSocketContext)
{
	if( !pSocketContext->GetParam())
		return;

	switch(pSocketContext->m_dwKeyParam)
	{
	case CONNECTIONKEY_DEFAULT:
		{
			CConnection *pCon = (CConnection*)pSocketContext->GetParam();
			pCon->SetActive(false);
			pCon->SetDelete(true);
			ClearSocketContext(pSocketContext);
			pCon->SetSocketContext(NULL, NULL);
			g_Log.Log(LogType::_NORMAL, L"[OnDisconnected] Socket:%d, Ip:%S\r\n", pSocketContext->m_Socket, pCon->GetIp());

#if defined(_WORK)
			SAFE_DELETE(pCon);
#endif	// _WORK
		}
		break;

	case CONNECTIONKEY_SERVICEMANAGER:
		{
			g_pServiceConnection->Disconnected();
			g_pServiceConnection->SetActive(false);
			g_pServiceConnection->SetDelete(true);
			ClearSocketContext(pSocketContext);
			g_pServiceConnection->SetSocketContext(NULL, NULL);
		}
		break;

	case CONNECTIONKEY_LOG:
		{
			g_pLogConnection->SetActive(false);
			g_pLogConnection->SetDelete(true);
			ClearSocketContext(pSocketContext);
			g_pLogConnection->SetSocketContext(NULL, NULL);
#if defined(PRE_ADD_LOGSERVER_HEARTBEAT)
			g_pLogConnection->ResetLiveCheck();
#endif
		}
		break;

#if defined(_KR) || defined(_US)
	case CONNECTIONKEY_BILLING:
		{
			g_pBillingConnection->SetActive(false);
			g_pBillingConnection->SetDelete(true);
			ClearSocketContext(pSocketContext);
			g_pBillingConnection->SetSocketContext(NULL, NULL);

			g_Log.Log(LogType::_NORMAL, L"[OnDisconnected:Cash] Socket:%d\r\n", pSocketContext->m_Socket);
		}
		break;
#endif	// _KR
#if defined(_TW)
	case CONNECTIONKEY_TW_QUERY:
		{
			g_pBillingPointConnection->SetActive(false);
			g_pBillingPointConnection->SetDelete(true);
			ClearSocketContext(pSocketContext);
			g_pBillingPointConnection->SetSocketContext(NULL, NULL);

			g_Log.Log(LogType::_NORMAL, L"[OnDisconnected:GASH QUERY] Socket:%d\r\n", pSocketContext->m_Socket);
		}
		break;
	case CONNECTIONKEY_TW_SHOPITEM :
		{
			g_pBillingShopConnection->SetActive(false);
			g_pBillingShopConnection->SetDelete(true);
			ClearSocketContext(pSocketContext);
			g_pBillingShopConnection->SetSocketContext(NULL, NULL);

			g_Log.Log(LogType::_NORMAL, L"[OnDisconnected:GASH SHOP] Socket:%d\r\n", pSocketContext->m_Socket);
		}
		break;
	case CONNECTIONKEY_TW_COUPON :
		{
			g_pBillingCouponConnection->SetActive(false);
			g_pBillingCouponConnection->SetDelete(true);
			ClearSocketContext(pSocketContext);
			g_pBillingCouponConnection->SetSocketContext(NULL, NULL);

			g_Log.Log(LogType::_NORMAL, L"[OnDisconnected:GASH COUPON] Socket:%d\r\n", pSocketContext->m_Socket);
		}
		break;
	case CONNECTIONKEY_TW_COUPON_ROLLBACK :
		{
			g_pBillingCouponRollBackConnection->SetActive(false);
			g_pBillingCouponRollBackConnection->SetDelete(true);
			ClearSocketContext(pSocketContext);
			g_pBillingCouponRollBackConnection->SetSocketContext(NULL, NULL);

			g_Log.Log(LogType::_NORMAL, L"[OnDisconnected:GASH COUPON ROLLBACK] Socket:%d\r\n", pSocketContext->m_Socket);
		}
		break;
#endif
#if defined(_TH)
	case CONNECTIONKEY_TH_QUERY:
		{
			g_pBillingPointConnection->SetActive(false);
			g_pBillingPointConnection->SetDelete(true);
			ClearSocketContext(pSocketContext);
			//g_pBillingPointConnection->SetSocketContext(NULL, NULL);

			g_Log.Log(LogType::_NORMAL, L"[OnDisconnected:AsiaSoft QUERY] Socket:%d\r\n", pSocketContext->m_Socket);
		}
		break;
	case CONNECTIONKEY_TH_SHOPITEM :
		{
			g_pBillingShopConnection->SetActive(false);
			g_pBillingShopConnection->SetDelete(true);
			ClearSocketContext(pSocketContext);
			//g_pBillingShopConnection->SetSocketContext(NULL, NULL);

			g_Log.Log(LogType::_NORMAL, L"[OnDisconnected:AsiaSoft SHOP] Socket:%d\r\n", pSocketContext->m_Socket);
		}
		break;
#endif
	}
}

void CDNIocpManager::OnReceive(CSocketContext *pSocketContext, DWORD dwBytesTransferred)
{
	CConnection *pCon = (CConnection*)pSocketContext->GetParam();
	if (pCon)
	{
		int nResult = pCon->AddRecvData( pSocketContext );

		if (nResult > 0){
			g_pThreadManager->AddThreadCall(pSocketContext);
		}
		else if (nResult == SIZEERR){
			g_Log.Log(LogType::_NORMAL, L"[OnReceive SizeError] Socket:%lld Size:%d\r\n", pSocketContext->m_Socket, pSocketContext->m_RecvIO.Len);
			DetachSocket(pSocketContext, L"OnReceive SizeError");
		}
		else if (nResult == -1){
			g_Log.Log(LogType::_NORMAL, L"[OnReceive] Socket:%lld Size:%d\r\n", pSocketContext->m_Socket, pSocketContext->m_RecvIO.Len);
			DetachSocket(pSocketContext, L"OnReceive");
		}
	}
}

int CDNIocpManager::CreateThread()
{
	HANDLE hThread;
	UINT ThreadID;

	hThread = (HANDLE)_beginthreadex(NULL, 0, &ReconnectThread, this, 0, &ThreadID);
	if (hThread == INVALID_HANDLE_VALUE) return -1;
	CloseHandle(hThread);

	return 0;
}
