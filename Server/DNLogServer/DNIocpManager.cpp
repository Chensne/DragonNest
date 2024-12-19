#include "StdAfx.h"
#include "DNIocpManager.h"
#include "DNConnection.h"
#include "DNThreadManager.h"
#include "DnServiceConnection.h"
#include "Log.h"
#include "DNConnectionManager.h"

#if defined(_KRAZ)
#include "DNActozShield.h"
#endif	// #if defined(_KRAZ)

CDNIocpManager* g_pIocpManager = NULL;

CDNIocpManager::CDNIocpManager(void): CIocpManager()
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
		
		if (g_pServiceConnection)
			g_pServiceConnection->Reconnect(CurTick);
#if defined(_KRAZ)
		if (g_pActozShield)
			g_pActozShield->RepeatCheckerResult(CurTick);
#endif	// #if defined(_KRAZ)
		
		Sleep(1);
	}
	return 0;
}

int CDNIocpManager::CreateUpdateThread()
{
	HANDLE hThread;
	UINT ThreadID;

	hThread = (HANDLE)_beginthreadex(NULL, 0, &UpdateThread, this, 0, &ThreadID);
	if (hThread == INVALID_HANDLE_VALUE) return -1;
	CloseHandle(hThread);

	return 0;
}

UINT __stdcall CDNIocpManager::UpdateThread(void *pParam)
{
	CDNIocpManager *pIocp = (CDNIocpManager*)pParam;

	DWORD CurTick = 0;

	while(pIocp->m_bThreadSwitch){
		CurTick = timeGetTime();		
		if (g_pConnectionManager) g_pConnectionManager->DoUpdate(CurTick);
		Sleep(5000); //그냥 5초 쉬어
	}
	return 0;
}

void CDNIocpManager::OnAccept(CSocketContext *pSocketContext, const char* pIp, const int nPort)
{
	//g_Log.Log(LogType::_FILELOG, L"[OnAccept:%d] Socket:%d, Ip:%S, Port:%d\r\n", pSocketContext->m_dwKeyParam, pSocketContext->m_Socket, pIp, nPort);

	switch(pSocketContext->m_dwKeyParam)
	{
	case CONNECTIONKEY_DEFAULT:
		{
			CDNConnection *pCon = (CDNConnection *)g_pConnectionManager->AddConnection(pIp, nPort);
			if (!pCon) return;

			pCon->SetServerConnection(true);
			pCon->SetSocketContext(this, pSocketContext);
			pSocketContext->SetParam(pCon);

			g_Log.Log(LogType::_FILELOG, L"[OnAccept] Socket:%d, Ip:%S\r\n", pSocketContext->m_Socket, pCon->GetIp());
		}
		break;

	case CONNECTIONKEY_SERVICEMANAGER:
		{
			pSocketContext->SetParam(g_pServiceConnection);
			g_pServiceConnection->SetSocketContext(this, pSocketContext);
			g_pServiceConnection->SetServerConnection(false);

			g_Log.Log(LogType::_FILELOG, L"[OnAccept:Monitor] Socket:%d, Ip:%S\r\n", pSocketContext->m_Socket, g_pServiceConnection->GetIp());
		}
		break;
	}
}

void CDNIocpManager::OnConnected(CSocketContext *pSocketContext)
{
	switch(pSocketContext->m_dwKeyParam)
	{
	case CONNECTIONKEY_DEFAULT:
		{
			CIocpManager::OnConnected(pSocketContext);

			CConnection *pCon = (CConnection*)pSocketContext->GetParam();

			pCon->SetActive(true);
			pCon->SetDelete(false);

			g_Log.Log(LogType::_FILELOG, L"[OnConnected] Socket:%d, Ip:%S\r\n", pSocketContext->m_Socket, pCon->GetIp());
		}
		break;

	case CONNECTIONKEY_SERVICEMANAGER:
		{
			g_pServiceConnection->SetActive(true);
			g_pServiceConnection->SetDelete(false);
		}
		break;
	}
}

void CDNIocpManager::OnDisconnected(CSocketContext *pSocketContext)
{
	switch(pSocketContext->m_dwKeyParam)
	{
	case CONNECTIONKEY_DEFAULT:
		{
			CConnection *pCon = (CConnection*)pSocketContext->GetParam();

			g_Log.Log(LogType::_FILELOG, L"[OnDisconnected] Socket:%d, Ip:%S\r\n", pSocketContext->m_Socket, pCon->GetIp());

#if defined( PRE_FIX_SOCKETCONTEXT_DANGLINGPTR )
			while( true )
			{
				if( InterlockedCompareExchange( &pSocketContext->m_lActiveCount, CSocketContext::DeleteFlag, 0 ) == 0 )
				{
					g_pConnectionManager->DelConnection(pCon);
					CIocpManager::OnDisconnected(pSocketContext);
					break;
				}
			}
#else
			CIocpManager::OnDisconnected(pSocketContext);
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

	CConnection *pCon = (CConnection*)pSocketContext->GetParam();
	if (pCon) {
		int nResult = pCon->AddRecvData( pSocketContext );

		if (nResult > 0){
			g_ThreadManager.AddThreadCall(pSocketContext);
		}
		else if (nResult == SIZEERR){
			g_Log.Log(LogType::_FILELOG, L"[OnReceive SizeError] Socket:%lld Size:%d\r\n", pSocketContext->m_Socket, pSocketContext->m_RecvIO.Len);
			DetachSocket(pSocketContext, L"OnReceive SizeError");
		}
		else if (nResult == -1){
			g_Log.Log(LogType::_FILELOG, L"[OnReceive] Socket:%lld Size:%d\r\n", pSocketContext->m_Socket, pSocketContext->m_RecvIO.Len);
			DetachSocket(pSocketContext, L"OnReceive");
		}
	}

	/*if (!pCon) return;

	int nResult = pCon->AddRecvData(pSocketContext->m_RecvIO.buffer, pSocketContext->m_RecvIO.Len);
	if (nResult > 0){
		g_ThreadManager.AddThreadCall(pSocketContext);
	}
	else if (nResult == SIZEERR){
		g_Log.Log(L"[OnReceive SizeError] Socket:%d Size:%d\r\n", pSocketContext->m_Socket, pSocketContext->m_RecvIO.Len);
		DetachSocket(pSocketContext, L"OnReceive SizeError");
	}
	else if (nResult == -1){
		g_Log.Log(L"[OnReceive] Socket:%d Size:%d\r\n", pSocketContext->m_Socket, pSocketContext->m_RecvIO.Len);
		DetachSocket(pSocketContext, L"OnReceive");
	}*/
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
