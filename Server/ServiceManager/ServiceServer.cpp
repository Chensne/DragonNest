
#include "stdafx.h"
#include "ServiceServer.h"
#include "GSMServer.h"
#include "DataManager.h"
#include "Connection.h"
#include "LauncherSession.h"
#include "Log.h"
#include "DNBAM.h"
#include "IniFile.h"
#include "Version.h"
#include "MonitorSession.h"
#include <locale.h>

HANDLE CServiceServer::m_hSignalProcess = INVALID_HANDLE_VALUE;
HANDLE CServiceServer::m_hSignalSyncTask = INVALID_HANDLE_VALUE;

TServiceManagerConfig g_Config;
CLog s_CountLog;
CLog s_ExcuteLog;

CServiceServer * g_pServiceServer = NULL;

CLog g_ExceptionReportLog;
CLog g_DBDelayLog;
CLog g_GameDelayLog;
CLog g_VillageDelayLog;
CLog g_DBErrorLog;
CLog g_DBSystemErrorLog;
CLog g_MonitorLog;
CLog g_FileLog;

CServiceServer::CServiceServer()
{
	m_bTaskAdded = false;
	m_hDestroyThreadEvent[0] = NULL;
	m_hDestroyThreadEvent[1] = NULL;
}

CServiceServer::~CServiceServer()
{

}

void CServiceServer::Close()
{
	CIocpManager::CloseAcceptors();
	CIocpManager::ThreadStop();
	WaitForMultipleObjects(2, m_hDestroyThreadEvent, TRUE, INFINITE);

	SAFE_DELETE(g_pServiceManager);
	CIocpManager::Final();
	while (!m_ProcessCalls.empty()){
		m_ProcessCalls.pop();
	}

	CloseHandle(m_hSignalProcess);
	CloseHandle(m_hSignalSyncTask);
	CloseHandle(m_hDestroyThreadEvent[0]);
	CloseHandle(m_hDestroyThreadEvent[1]);
}

bool CServiceServer::Initialize(int nSocketCountMax, int nLauncherPort, int nServicePort, int nServicePatcherPort, int nMonitorPort)
{
	if (CIocpManager::Init(nSocketCountMax, 8) == -1)
	{
		g_Log.Log(LogType::_FILELOG, L"IOCP Init Failed\n");
		return false;
	}

	if (CreateThread() == false)
	{
		g_Log.Log(LogType::_FILELOG, L"Thread Create Failed\n");
		return false;
	}

	if (CIocpManager::AddAcceptConnection(CONNECTIONKEY_LAUNCHER, nLauncherPort, false) == -1)
	{
		g_Log.Log(LogType::_FILELOG, L"PortOpen Failed NetLauncherPort[%d]\n", nLauncherPort);
		return false;
	}

	if (CIocpManager::AddAcceptConnection(CONNECTIONKEY_SERVICEMANAGER, nServicePort, false) == -1)
	{
		g_Log.Log(LogType::_FILELOG, L"PortOpen Failed NetLauncherPort[%d]\n", nLauncherPort);
		return false;
	}

	if (CIocpManager::AddAcceptConnection(CONNECTIONKEY_SERVICEPATCHER, nServicePatcherPort, false) == -1)
	{
		g_Log.Log(LogType::_FILELOG, L"PortOpen Failed NetLauncherPort[%d]\n", nLauncherPort);
		return false;
	}

	if (CIocpManager::AddAcceptConnection(CONNECTIONKEY_SERVERMONITOR, nMonitorPort, false) == -1)
	{
		g_Log.Log(LogType::_FILELOG, L"PortOpen Failed NetLauncherPort[%d]\n", nLauncherPort);
		return false;
	}
	
	for(int i = 0; i < 2; i++)
	{
		m_hDestroyThreadEvent[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
		if(m_hDestroyThreadEvent[i] == INVALID_HANDLE_VALUE)
		{
			g_Log.Log(LogType::_FILELOG, L"DestroyThreadEvent Event Create Failed\n");
			return false;
		}
	}
	
	return true;
}

void CServiceServer::AddProcessCall(CSocketContext *pSocketContext)
{
	m_ProcessLock.Lock();
	m_ProcessCalls.push(pSocketContext);
	SetEvent(m_hSignalProcess);
	m_ProcessLock.UnLock();
}

void CServiceServer::AddSyncCall()
{
	ScopeLock<CSyncLock> Lock(m_SyncTaskLock);
	
	SetEvent(m_hSignalProcess);
	m_bTaskAdded = true;
}

UINT __stdcall CServiceServer::ProcessThread(void *pParam)
{
	CServiceServer *pIocp = (CServiceServer*)pParam;

	CSocketContext *pSocketContext;
	CConnection *pCon = NULL;
	USHORT nRecv = 0;
	
#if defined( PRE_FIX_SOCKETCONTEXT_DANGLINGPTR )
	std::list<CSocketContext*> listDeleteContext;
#endif // #if defined( PRE_FIX_SOCKETCONTEXT_DANGLINGPTR )

	while(pIocp->m_bThreadSwitch)
	{
#if defined( PRE_FIX_SOCKETCONTEXT_DANGLINGPTR )
		for( std::list<CSocketContext*>::iterator itor=listDeleteContext.begin() ; itor!=listDeleteContext.end() ; )
		{
			if( InterlockedCompareExchange( &(*itor)->m_lActiveCount, CSocketContext::DeleteFlag, 0 ) == 0 )
			{
				pSocketContext = (*itor);
				itor = listDeleteContext.erase( itor );

				pCon = (CConnection*)pSocketContext->GetParam();

				g_pServiceManager->DelConnection(pCon, (eConnectionKey)pSocketContext->m_dwKeyParam);
				pIocp->ClearSocketContext(pSocketContext);
				pCon->SetSocketContext(NULL, NULL);
				SAFE_DELETE(pCon);
			}
			else
			{
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
			pCon = (CConnection*)pSocketContext->GetParam();
			if (pCon && pCon->FlushRecvData(0) == false)
			{
				listDeleteContext.push_back( pSocketContext );
			}
#else
			pCon = (CConnection*)pSocketContext->GetParam();
			if (pCon && pCon->FlushRecvData(0) == false)
			{
				g_pServiceManager->DelConnection(pCon, (eConnectionKey)pSocketContext->m_dwKeyParam);
				pIocp->ClearSocketContext(pSocketContext);
				pCon->SetSocketContext(NULL, NULL);
				SAFE_DELETE(pCon);
			}
#endif
		}
		else
		{
			ResetEvent(m_hSignalProcess);
			::WaitForSingleObject(m_hSignalProcess, 1000);
		}

		if (listDeleteContext.empty() == false)
			continue;

		DWORD CurTick = 0;
		static DWORD PreTick = 0;

		if (PreTick == 0)
			PreTick = timeGetTime();
		CurTick = timeGetTime();

		//
		//g_pServiceManager->SendCopyExt();
		if (PreTick != 0 && PreTick + IDLEDISTRIBUTING_TIME < CurTick)
		{
			g_pServiceManager->IdleProcess();
			PreTick = timeGetTime();
		}
	}

	pIocp->ThreadDestroyEvent(0);
	return 0;
}

UINT __stdcall CServiceServer::SyncTaskProcess(void * pParam)
{
	CServiceServer *pIocp = (CServiceServer*)pParam;
	while(pIocp->m_bThreadSwitch)
	{
		if (pIocp->m_bTaskAdded)
		{
			g_pServiceManager->SyncTaskProcess();

			ScopeLock <CSyncLock> Lock(pIocp->m_SyncTaskLock);
			pIocp->m_bTaskAdded = false;
		}
		else
		{
			ResetEvent(m_hSignalSyncTask);
			::WaitForSingleObject(m_hSignalSyncTask, 1000);
		}
	}
	pIocp->ThreadDestroyEvent(1);
	return 0;
}

void CServiceServer::OnAccept(CSocketContext *pSocketContext, const char* pIp, const int nPort)
{
	switch(pSocketContext->m_dwKeyParam)
	{
		case CONNECTIONKEY_LAUNCHER:
		{
			CLauncherSession * pSession = g_pServiceManager->AddLauncherSession(pIp, nPort);
			if (pSession)
			{
				pSocketContext->SetParam(pSession);
				pSession->SetSocketContext(this, pSocketContext);
			}
			else
				ClearSocketContext(pSocketContext);
			break;
		}
		case CONNECTIONKEY_SERVICEMANAGER:
		{
			CServiceSession * pSession = g_pServiceManager->AddServiceSession(pIp, nPort);
			if (pSession)
			{
				pSocketContext->SetParam(pSession);
				pSession->SetSocketContext(this, pSocketContext);
			}
			else
				ClearSocketContext(pSocketContext);
			break;
		}
		case CONNECTIONKEY_SERVICEPATCHER:
		{
			CPatcherSession * pSession = g_pServiceManager->AddPatcherSession(pIp, nPort);
			if (pSession)
			{
				pSocketContext->SetParam(pSession);
				pSession->SetSocketContext(this, pSocketContext);
			}
			else
				ClearSocketContext(pSocketContext);
			break;
		}

		case CONNECTIONKEY_SERVERMONITOR:
		{
			CMonitorSession * pSession = g_pServiceManager->AddMonitorSession(pIp, nPort);
			if (pSession)
			{
				pSocketContext->SetParam(pSession);
				pSession->SetSocketContext(this, pSocketContext);
			}
			else
				ClearSocketContext(pSocketContext);
			break;
		}
	}
}

void CServiceServer::OnConnected(CSocketContext *pSocketContext)
{
#if defined( PRE_FIX_SOCKETCONTEXT_DANGLINGPTR )
	CScopeInterlocked Scope( &pSocketContext->m_lActiveCount );
	if( Scope.bIsDelete() )
		return;
#endif // #if defined( PRE_FIX_SOCKETCONTEXT_DANGLINGPTR )

	switch(pSocketContext->m_dwKeyParam)
	{
		case CONNECTIONKEY_LAUNCHER:
		{
			CLauncherSession * pSession = (CLauncherSession*)pSocketContext->GetParam();
			if (pSession)
			{
				pSession->SetActive(true);
				pSession->SetDelete(false);
				
				g_pServiceManager->LauncherConnected(pSession);
			}
			break;
		}
		case CONNECTIONKEY_SERVICEMANAGER:
		{
			CServiceSession * pSession = (CServiceSession*)pSocketContext->GetParam();
			if (pSession)
			{
				pSession->SetActive(true);
				pSession->SetDelete(false);
			}
			break;
		}

		case CONNECTIONKEY_SERVICEPATCHER:
		{
			CPatcherSession * pSession = (CPatcherSession*)pSocketContext->GetParam();
			if (pSession)
			{
				pSession->SetActive(true);
				pSession->SetDelete(false);

				pSession->SendConnectedResult(g_Config.wszPatchBaseURL, g_Config.wszPatchURL);
			}
			break;
		}
		case CONNECTIONKEY_SERVERMONITOR:
		{
			CMonitorSession * pSession = (CMonitorSession*)pSocketContext->GetParam();
			if (pSession)
			{
				pSession->SetActive(true);
				pSession->SetDelete(false);
			}
			break;
		}
	}
}

void CServiceServer::OnDisconnected(CSocketContext *pSocketContext)
{
#if defined( PRE_FIX_SOCKETCONTEXT_DANGLINGPTR )
	CScopeInterlocked Scope( &pSocketContext->m_lActiveCount );
	if( Scope.bIsDelete() )
		return;
#endif // #if defined( PRE_FIX_SOCKETCONTEXT_DANGLINGPTR )

	switch(pSocketContext->m_dwKeyParam)
	{
		case CONNECTIONKEY_LAUNCHER:
		{
			CLauncherSession * pSession = (CLauncherSession*)pSocketContext->GetParam();
			if (pSession)
			{
				DNTPacket Header = { 0, };
				Header.cMainCmd = IN_DISCONNECT;
				Header.iLen = sizeof(USHORT) + (sizeof(BYTE) * 2);

				pSession->BufferClear();
				pSession->SetActive(false);
				pSession->SetDelete(true);
				pSession->AddRecvData( Header );

				AddProcessCall(pSocketContext);
			}
			break;
		}
		case CONNECTIONKEY_SERVICEMANAGER:
		{
			CServiceSession * pSession = (CServiceSession*)pSocketContext->GetParam();
			if (pSession)
			{
				DNTPacket Header = { 0, };
				Header.cMainCmd = IN_DISCONNECT;
				Header.iLen = sizeof(USHORT) + (sizeof(BYTE) * 2);

				pSession->BufferClear();
				pSession->SetActive(false);
				pSession->SetDelete(true);
				pSession->AddRecvData( Header );

				AddProcessCall(pSocketContext);
			}
			break;
		}

		case CONNECTIONKEY_SERVICEPATCHER:
		{
			CServiceSession * pSession = (CServiceSession*)pSocketContext->GetParam();
			if (pSession)
			{
				DNTPacket Header = { 0, };
				Header.cMainCmd = IN_DISCONNECT;
				Header.iLen = sizeof(USHORT) + (sizeof(BYTE) * 2);

				pSession->BufferClear();
				pSession->SetActive(false);
				pSession->SetDelete(true);
				pSession->AddRecvData( Header );

				AddProcessCall(pSocketContext);
			}
			break;
		}
		case CONNECTIONKEY_SERVERMONITOR:
		{
			CMonitorSession * pSession = (CMonitorSession*)pSocketContext->GetParam();
			if (pSession)
			{
				DNTPacket Header = { 0, };
				Header.cMainCmd = IN_DISCONNECT;
				Header.iLen = sizeof(USHORT) + (sizeof(BYTE) * 2);

				pSession->BufferClear();
				pSession->SetActive(false);
				pSession->SetDelete(true);
				pSession->AddRecvData( Header );

				AddProcessCall(pSocketContext);
			}
			break;
		}
	}
}

void CServiceServer::OnReceive(CSocketContext *pSocketContext, DWORD dwBytesTransferred)
{
#if defined( PRE_FIX_SOCKETCONTEXT_DANGLINGPTR )
	CScopeInterlocked Scope( &pSocketContext->m_lActiveCount );
	if( Scope.bIsDelete() )
		return;
#endif // #if defined( PRE_FIX_SOCKETCONTEXT_DANGLINGPTR )

	CConnection *pCon = (CConnection*)pSocketContext->GetParam();
	if (pCon)
	{
		int nResult = pCon->AddRecvData(pSocketContext);

		if (nResult > 0){
			AddProcessCall(pSocketContext);
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
}

bool CServiceServer::CreateThread()
{
	HANDLE hThread;
	UINT ThreadID;

	hThread = (HANDLE)_beginthreadex(NULL, 0, &ProcessThread, this, 0, &ThreadID);
	if (hThread == INVALID_HANDLE_VALUE)
		return false;
	CloseHandle(hThread);

	hThread = (HANDLE)_beginthreadex(NULL, 0, &SyncTaskProcess, this, 0, &ThreadID);
	if (hThread == INVALID_HANDLE_VALUE)
		return false;
	CloseHandle(hThread);

	return true;
}

#if !defined (_SERVICEMANAGER_EX)
bool LoadConfig()
{
	wstring wszFileName = L"./Config/DNServiceManager.ini";			// 한국
	if (!g_IniFile.Open(wszFileName.c_str())){
		g_Log.Log(LogType::_FILELOG, L"%s File not Found!!\r\n", wszFileName.c_str());
		return false;
	}
	memset(&g_Config, 0, sizeof(TServiceManagerConfig));

	g_IniFile.GetValue(L"Region", L"RegionInfo", g_Config.wszRegion);

	g_IniFile.GetValue(L"Connection", L"NetLauncherPort", &g_Config.nLauncherPort);
	g_IniFile.GetValue(L"Connection", L"ServicePort", &g_Config.nServicePort);
	g_IniFile.GetValue(L"Connection", L"ServicePatcherPort", &g_Config.nServicePatcherPort);
	g_IniFile.GetValue(L"Connection", L"GSMPort", &g_Config.nGSMPort);
	g_IniFile.GetValue(L"Connection", L"MonitorPort", &g_Config.nMonitorPort);

	g_IniFile.GetValue(L"GSMInfo", L"IP", g_Config.wszGSMIP);
	g_IniFile.GetValue(L"GSMInfo", L"CodePage", g_Config.wszGSMCodePage);
	g_IniFile.GetValue(L"PatchInfo", L"BaseUrl", g_Config.wszPatchBaseURL);
	g_IniFile.GetValue(L"PatchInfo", L"PatchUrl", g_Config.wszPatchURL);
	g_IniFile.GetValue(L"ReserveNoticeInfo", L"FileName", g_Config.wszNoticePath);
	strcpy_s(g_Config.szVersion, szServiceManagerVersion);

	g_Log.Log(LogType::_FILELOG, L"## LauncherPort:%d, ServicePort:%d GsmPort:%d ESMPort:%d MonitorPort:%d\r\n", g_Config.nLauncherPort, g_Config.nServicePort, g_Config.nGSMPort, g_Config.nServicePatcherPort, g_Config.nMonitorPort);
	g_Log.Log(LogType::_FILELOG, L"## ServiceManager Version : %S\r\n", szServiceManagerVersion);
	if (_access("./System", 0) == -1)
	{
		mkdir("./System");
	}
	return true;
}

void main(int argc, char * argv[])
{
	SetMiniDump();

	g_Log.Init(L"ServiceManager", LOGTYPE_CRT_FILE_DAY);
	s_CountLog.Init(L"LogCount", LOGTYPE_FILE_DAY);
	g_ExceptionReportLog.Init(L"ExceptionReport", LOGTYPE_FILE_DAY);
	g_DBDelayLog.Init(L"DBDelay", LOGTYPE_FILE_DAY);
	s_ExcuteLog.Init(L"Excute", LOGTYPE_FILE_DAY);
	g_GameDelayLog.Init(L"GameDelay", LOGTYPE_FILE_DAY);
	g_VillageDelayLog.Init(L"VillageDelay", LOGTYPE_FILE_DAY);
	g_DBErrorLog.Init(L"DBError", LOGTYPE_FILE_DAY);
	g_DBSystemErrorLog.Init(L"DBSystemError", LOGTYPE_FILE_DAY);
	g_MonitorLog.Init(L"MonitorLog", LOGTYPE_FILE_DAY);
	g_FileLog.Init(L"ServiceManager", LOGTYPE_FILE_DAY);


	if (LoadConfig() == false)
		_ASSERT_EXPR(0, L"fail LoadConfig");

	int nSocketCount = 1000;
	if (!_wcsicmp(g_Config.wszRegion, L"KR") || !_wcsicmp(g_Config.wszRegion, L"KOR") || !_wcsicmp(g_Config.wszRegion, L"DEV"))
		setlocale(LC_ALL, "Korean");
	else if (!_wcsicmp(g_Config.wszRegion, L"CH") || !_wcsicmp(g_Config.wszRegion, L"CHN"))
	{
		setlocale(LC_ALL, "chinese-simplified");
		nSocketCount = 3000;
	}
	else if (!_wcsicmp(g_Config.wszRegion, L"JP") || !_wcsicmp(g_Config.wszRegion, L"JPN"))
		setlocale(LC_ALL, "japanese");
	else if (!_wcsicmp(g_Config.wszRegion, L"US") || !_wcsicmp(g_Config.wszRegion, L"USA"))
		setlocale(LC_ALL, "us");

//#if defined(_KR)
//	g_pBAM = new CDNBAM;
//	if (!g_pBAM) return;
//#endif	// #if defined(_KR)

#ifdef _UNICODE
	g_pServiceManager = new CServiceManager();
#else
	char szIP[IPLENMAX], szCode[256];
	WideCharToMultiByte(CP_ACP, 0, g_Config.wszGSMIP, -1, szIP, IPLENMAX, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, g_Config.wszGSMCodePage, -1, szCode, IPLENMAX, NULL, NULL);

	g_pServiceManager = new CServiceManager(szIP, szCode);
#endif
	if (!g_pServiceManager) return;

	g_pServiceServer = new CServiceServer;
	if (!g_pServiceServer) return;

	if (g_pServiceServer->Initialize(nSocketCount, g_Config.nLauncherPort, g_Config.nServicePort, g_Config.nServicePatcherPort, g_Config.nMonitorPort) == false)
		_ASSERT_EXPR(0, L"fail g_pServiceServer->Initialize");

	CGSMServer server;
	if (server.Open(g_Config.nGSMPort) == false)
		_ASSERT_EXPR(0, L"CGSMServer fail to open");

	wprintf(L"exit 명령을 치면 종료\r\n");

	char szCmd[256] = {0,};	
	while (1)
	{
		if (strcmp(szCmd, "exit") == 0)	break;

		g_pServiceManager->ParseCommand(szCmd);
		
		printf("CMD>");
		cgets(szCmd);
	}

	g_pServiceServer->Close();
}
#endif // #if !defined (_SERVICEMANAGER_EX)