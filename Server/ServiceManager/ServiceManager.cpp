
#include "stdafx.h"
#include "ServiceServer.h"
#include "ServiceManager.h"
#include "DNConfig.h"
#include "Log.h"
#include "IniFile.h"
#include "StringUtil.h"
#include "SpinBuffer.h"
#include "csvreader.h"
#if defined (_SERVICEMANAGER_EX)
#include "ServiceManagerEx.h"
#endif // #if defined (_SERVICEMANAGER_EX)

#if defined(_KRAZ)
#include "DNActozShield.h"
#endif	// #if defined(_KRAZ)

extern CLog g_ExceptionReportLog;
extern CLog s_ExcuteLog;

#if defined( _SERVICEMANAGER_EX )
extern CLog g_FileLog;
#endif

static TCHAR * pOffLine = { _T("Offline") };
static TCHAR * pOnline = { _T("Online") };
static TCHAR * pCreate = { _T("Create") };
static TCHAR * pTerminated = { _T("Terminated") };

static TCHAR * pBlack = { _T("Black") };
static TCHAR * pGreen = { _T("Green") };
static TCHAR * pYellow = { _T("Yellow") };
static TCHAR * pRed = { _T("Red") };
static TCHAR * pDefault = { _T("Default") };

const TCHAR * ConvertState(int nState)
{
	switch (nState)
	{
	case _SERVICE_STATE_OFFLINE: return pOffLine;
		case _SERVICE_STATE_CREATE: return pCreate;
		case _SERVICE_STATE_ONLINE: return pOnline;
		case _SERVICE_STATE_TERMINATED: return pTerminated;

		default: return pDefault;
	}
	return NULL;
};

extern TServiceManagerConfig g_Config;
CServiceManager * g_pServiceManager = NULL;


#if defined (_SERVICEMANAGER_EX)
CServiceManager::CServiceManager(CServiceManagerExApp* pApp)
: m_pApp(pApp)
#else
CServiceManager::CServiceManager()
#endif // #if defined (_SERVICEMANAGER_EX)
{
	memset(m_szProcess, 0, sizeof(m_szProcess));
	m_UrlUpdater.UpdaterInit(g_Config.wszPatchBaseURL, g_Config.wszPatchURL);

	if (CDataManager::GetInstance()->LoadUser() == false)
	{
		g_Log.Log(LogType::_FILELOG, L"MonitorUser.xml LoadFail\n");
	}

	if (CDataManager::GetInstance()->LoadStruct() == false)
	{
		_ASSERT_EXPR(0, L"[CServiceManager] CDataManager::GetInstance()->LoadStruct() == false");
		return;
	}
	else
	{
		std::vector <int> vSIDList;
		CDataManager::GetInstance()->GetCommandList(&vSIDList);
		for (int i = 0; i < (int)vSIDList.size(); i++)
		{
			const TServerExcuteData * pExe = CDataManager::GetInstance()->GetCommand(vSIDList[i]);
			if (MakeServiceList(pExe->nSID, pExe->nAssignedLauncherID, pExe->wszType) == false)
				_ASSERT_EXPR(0, L"[CServiceManager] MakeServiceList(pExe->nSID, pExe->nAssignedLauncherID, pExe->wszType) == false");
		}
	}

	MakeWorldList();

	char szNotice[256];
	WideCharToMultiByte(CP_ACP, 0, g_Config.wszNoticePath, -1, szNotice, 256, NULL, NULL);
	if (_access(szNotice, 0) != -1)
		m_ReservedNotice.LoadNotice(szNotice);

	std::string strCmd;
	strCmd = g_Config.szVersion;
	ToLowerA(strCmd);

	std::vector<std::string> tokens;
	TokenizeA(strCmd, tokens, ".");

	m_nMajorVersion = atoi(tokens[0].c_str());
	m_nMinorVersion = atoi(tokens[1].c_str());

	m_nPatchID = 0;
	m_nContainerVersion = 0;
	m_bCloseService = false;
	m_nTerminateDBTick = 0;

	m_bHttpSendFail = false;
	m_nHttpFailTick = 0;
	m_pStoredPacketBuffer = new (std::nothrow) CRawSpinBuffer( 1, 1024*1024*10 );
	m_nRunBatchID = 0;
	m_nStopBatchID = 0;

	m_CmdTable.lpFarmUpdate = CServiceManager::UpdateFarm;

	for (int i=0; i<SERVERTYPE_MAX; i++)
	{
		m_nLastMonitorReportedTick[i] = 0;
		m_vServerDelayMIDs[i].clear();
	}

#if defined(_KRAZ)
	g_pActozShield = new CDNActozShield;
	if (g_pActozShield)
		g_pActozShield->Init();
#endif	// #if defined(_KRAZ)
}

CServiceManager::~CServiceManager()
{
	ScopeLock <CSyncLock> sync(m_Sync);
	for (std::map <UINT, CLauncherSession*>::iterator ii = m_LauncherSessionList.begin(); ii != m_LauncherSessionList.end(); ii++)
		SAFE_DELETE((*ii).second);
	m_LauncherSessionList.clear();

	for (std::map <UINT, CServiceSession*>::iterator ii = m_ServiceSessionList.begin(); ii != m_ServiceSessionList.end(); ii++)
		SAFE_DELETE((*ii).second);
	m_ServiceSessionList.clear();

	for (std::map <int, sServiceState*>::iterator ii = m_ServiceList.begin(); ii != m_ServiceList.end(); ii++)
		SAFE_DELETE((*ii).second);
	m_ServiceList.clear();

	for (std::vector <CPatcherSession*>::iterator ii = m_vPatcherSessionList.begin(); ii != m_vPatcherSessionList.end(); ii++)
		SAFE_DELETE((*ii));
	m_vPatcherSessionList.clear();
	m_PatcherSessionList.clear();
	delete m_pStoredPacketBuffer;

#if defined(_KRAZ)
	SAFE_DELETE(g_pActozShield);
#endif	// #if defined(_KRAZ)
}

void CServiceManager::MakeRandomException()
{
	return;//rlkt_test

	switch (rand()%4)
	{
	case 0:
		{
			SendExceptionReport(1004, _EXCEPTIONTYPE_ROOMCRASH, NULL);
			break;
		}
	case 1:
		{
			SendExceptionReport(1004, _EXCEPTIONTYPE_SESSIONCRASH, NULL);
			break;
		}
	case 2:
		{
			SendExceptionReport(1004, _EXCEPTIONTYPE_ROOMDESTRUCTOR, NULL);
			break;
		}
	case 3:
		{
			SendExceptionReport(1000, _EXCEPTIONTYPE_DBMW_DELAYED, "DataBase MiddleWare Delayed Over 60Second");
			break;
		}
	}
}

void CServiceManager::IdleProcess()
{
	if (m_ServiceCloser.IdleProcess())
	{
		TerminateServiceAll();
		return;
	}

	m_ReservedNotice.IdleProcess();
	
	ScopeLock <CSyncLock> sync(m_Sync);

	ULONG nCurTick = timeGetTime();

	static int nCount = 0;
	if (nCount%12 == 0)
	{
		std::map <int, sServiceState*>::iterator ServiceItor;
		for (ServiceItor = m_ServiceList.begin(); ServiceItor != m_ServiceList.end(); ServiceItor++)
			SendPacketToGsm(_T("msg=PInfo,%d,%s"), (*ServiceItor).second->nSID, ConvertState((*ServiceItor).second->nState));
	}
	nCount++;

	std::map <int, sServiceState*>::iterator ServiceItor;
	for (ServiceItor = m_ServiceList.begin(); ServiceItor != m_ServiceList.end(); ServiceItor++)
	{
		if ((*ServiceItor).second->nState == _SERVICE_STATE_CREATE)
		{
			if ((*ServiceItor).second->nCreateTime > 0 && (*ServiceItor).second->nCreateTime + (5 * 60 * 1000) < nCurTick)
			{
				std::map <UINT, CServiceSession*>::iterator ii2;
				for (ii2 = m_ServiceSessionList.begin(); ii2 != m_ServiceSessionList.end(); ii2++)
				{
					if ((*ServiceItor).first == (*ii2).second->GetMID())
					{
						(*ii2).second->DetachConnection(L"CreateOverTime");
						break;
					}
				}
			}
		}
	}
	

	std::vector <int> vList;
	std::map <UINT, CServiceSession*>::iterator sessionitor;
	for (sessionitor = m_ServiceSessionList.begin(); sessionitor != m_ServiceSessionList.end(); sessionitor++)
	{
		if ((*sessionitor).second->IsCompleteConnected(nCurTick) == false)
		{
			(*sessionitor).second->DetachConnection(L"IncompleteConnect");
			continue;
		}
	}

	for (sessionitor = m_ServiceSessionList.begin(); sessionitor != m_ServiceSessionList.end(); sessionitor++)
	{
		if ((*sessionitor).second->GetType() == MANAGED_TYPE_VILLAGE)
		{
			int nServiceRet = (*sessionitor).second->IdleProcess(nCurTick);
			if (nServiceRet != SERVICEPROCESS_NONE)
			{
				vList.push_back((*sessionitor).second->GetMID());
				g_Log.Log(LogType::_ERROR, L"VillageServer Report Delayed Over120 Second SID[%d]\n", (*sessionitor).second->GetMID());
				g_ExceptionReportLog.Log(LogType::_FILELOG, L"VillageServer Report Delayed Over120 Second SID[%d]\n", (*sessionitor).second->GetMID());
			}
		}
		else if ((*sessionitor).second->GetType() == MANAGED_TYPE_GAME)
		{
			int nServiceRet = (*sessionitor).second->IdleProcess(nCurTick);
			if (nServiceRet == SERVICEPROCESS_NONE)
			{
			}
			else if (nServiceRet == SERVICEPROCESS_FAULTSAFE)
			{
				vList.push_back((*sessionitor).second->GetMID());
				g_Log.Log(LogType::_ERROR, L"GameServer FaultSafe SID[%d]\n", (*sessionitor).second->GetMID());
			}
			else if ((*sessionitor).second->GetLastReportTime() > 0 && (*sessionitor).second->GetLastReportTime() + (120 * 1000) < nCurTick)
			{
				if ((*sessionitor).second->GetLastDelayedTime() <= 0 || (*sessionitor).second->GetLastDelayedTime() + (120 * 1000) < nCurTick)
				{
					(*sessionitor).second->SetLastDelayedTime(timeGetTime());
					g_Log.Log(LogType::_ERROR, L"GameServer Report Delayed Over120 Second SID[%d]\n", (*sessionitor).second->GetMID());
					g_ExceptionReportLog.Log(LogType::_FILELOG, L"GameServer Report Delayed Over120 Second SID[%d]\n", (*sessionitor).second->GetMID());
				}

				if ((*sessionitor).second->GetLastReportTime() + (3 * 60 * 1000) < nCurTick)
					vList.push_back((*sessionitor).second->GetMID());
			}
		}
		else if ((*sessionitor).second->GetType() == MANAGED_TYPE_LOGIN)
		{
			(*sessionitor).second->IdleProcess(nCurTick);
			if ((*sessionitor).second->GetLastReportTime() > 0 && (*sessionitor).second->GetLastReportTime() + (120 * 1000) < nCurTick)
			{
				
				g_Log.Log(LogType::_ERROR, L"LoginServer Report Delayed Over120 Second SID[%d]\n", (*sessionitor).second->GetMID());
				g_ExceptionReportLog.Log(LogType::_FILELOG, L"LoginServer Report Delayed Over120 Second SID[%d]\n", (*sessionitor).second->GetMID());

				(*sessionitor).second->SetLastDelayedTime(timeGetTime());
				if ((*sessionitor).second->GetLastDelayedTime() <= 0)
					MakeDumEach((*sessionitor).second->GetMID());

				else if ((*sessionitor).second->GetLastDelayedTime() + (2 * 60 * 1000) < nCurTick)
				{
					vList.push_back((*sessionitor).second->GetMID());
				}

			}
		}
	}

	for (int i = 0; i < (int)vList.size(); i++)
	{
		bool bterminateFalg = TerminateServiceEachForceAsync(vList[i]);
		if (bterminateFalg == false)
		{
			g_Log.Log(LogType::_FILELOG, L"Already Req Terminate [SID%d]\n", vList[i]);
			std::map <UINT, CServiceSession*>::iterator sitor;
			for (sitor = m_ServiceSessionList.begin(); sitor != m_ServiceSessionList.end(); sitor++)
			{
				if ((*sitor).second->GetMID() == vList[i])
				{
					g_Log.Log(LogType::_FILELOG, L"Detach Connection [SID%d]\n", (*sitor).second->GetMID());
					(*sitor).second->DetachConnection(L"TerminateForce FaultSafe");
					break;
				}
			}
		}
	}

	if (m_CloseServieList.size() > 0)
	{
		std::vector <_CLOSESERVICE>::iterator ii;
		for (ii = m_CloseServieList.begin(); ii != m_CloseServieList.end(); )
		{
			_CLOSESERVICE close = (*ii);
			if ((close.nTerminateTick > 0 && close.nTerminateTick + (_CLOSE_REPORTED_SERVICE_WAITING_TIME) < nCurTick) ||
				(close.nInsertTick + (_CLOSE_SERVICE_WAITING_TIME) < nCurTick))
			{
				std::map <int, sServiceState*>::iterator ih = m_ServiceList.find(close.nMID);
				if (ih != m_ServiceList.end())
				{
					sServiceState * pService = (*ih).second;
					std::map <UINT, CLauncherSession*>::iterator ij = m_LauncherSessionList.find(pService->nManagedLauncherID);
					if (ij != m_LauncherSessionList.end())
						(*ij).second->SendTerminateProcess(pService->nSID);
				}
				ii = m_CloseServieList.erase(ii);
			}
			else
				ii++;
		}

		if (m_CloseServieList.empty())
		{
			g_Log.Log(LogType::_FILELOG, L"AllService Close After Terminate Process OK\n");
			if (m_bCloseService)
			{
				g_Log.Log(LogType::_FILELOG, L"Terminate Process After DBServerClose Start\n");
				m_bCloseService = false;
				if (m_nTerminateDBTick <= 0)
					m_nTerminateDBTick = timeGetTime() + 10 * 1000;
			}
		}
	}

	if (m_nTerminateDBTick > 0 && m_nTerminateDBTick > nCurTick)
	{		
		TerminateServiceDB(NULL, true);
		m_nTerminateDBTick = 0;
		g_Log.Log(LogType::_FILELOG, L"Terminate Process After DBServerClose End\n");
		SendPacketToGsm(_T("msg=ServiceClose"));
	}

	std::map <UINT, CLauncherSession*>::iterator netItor;
	for (netItor = m_LauncherSessionList.begin(); netItor != m_LauncherSessionList.end(); netItor++)
	{
		if ((*netItor).second->GetPingTick() <= 0)
		{
			if ((*netItor).second->GetCreateTick() + (60 * 1000) < nCurTick)
			{
				StopEachLauncherAsync((*netItor).second->GetSessionID());
				break;
			}
			continue;
		}
		
		ULONG nGap = 0;
		if (nCurTick > (*netItor).second->GetPingTick())
			nGap = nCurTick - (*netItor).second->GetPingTick();
		else if (nCurTick < (*netItor).second->GetPingTick())
			nGap = (*netItor).second->GetPingTick() - nCurTick;
		
		if (nGap > (20 * 60 * 1000))
		{
			g_Log.Log(LogType::_ERROR, L"PingDelay [GAP:%d][Cur:%d][Ping:%d]\n", nGap, nCurTick, (*netItor).second->GetPingTick());
			(*netItor).second->DetachConnection(L"PingDelay");
		}
	}

	std::map <int, ULONG>::iterator testItor;
	if (m_LaunchetTestList.size() > 0)
	{
		for (testItor = m_LaunchetTestList.begin(); testItor != m_LaunchetTestList.end(); )
		{
			if (nCurTick > (*testItor).second + (30 * 1000))
			{
				g_Log.Log(LogType::_ERROR, L"NetLauncher ping return delayed [NID:%d]\n", (*testItor).first);
				testItor = m_LaunchetTestList.erase(testItor);
			}
			else
				testItor++;
		}
	}
	for (int i=0; i<SERVERTYPE_MAX; i++)
	{
		if (nCurTick > m_nLastMonitorReportedTick[i] + 3000)
		{
			ReportToMonitorServerDelay(i);
			m_nLastMonitorReportedTick[i] = nCurTick;
		}
	}
}

void CServiceManager::SyncTaskProcess()
{
	ScopeSpinBufferSwitch Scope( m_pStoredPacketBuffer );

	char * pBuffer = Scope.pGetBuffer();
	UINT uiSize	= Scope.uiGetSize();

	while(uiSize)
	{
		_SYNCGSMTASK * pTask = (_SYNCGSMTASK*)pBuffer;

		if (m_HttpUpdater.QueryPost(g_Config.wszGSMIP, NULL, NULL, g_Config.wszGSMCodePage, pTask->szMsg, pTask->nSize - sizeof(short)) == false)
		{
			m_bHttpSendFail = true;
			m_nHttpFailTick = timeGetTime();

			g_Log.Log(LogType::_FILELOG, L"GSM Send Error %S\n", pTask->szMsg);
		}

		uiSize -= pTask->nSize;
		pBuffer += pTask->nSize;
	}
}

CLauncherSession * CServiceManager::AddLauncherSession(const char * pIP, unsigned short nPort)
{
	ScopeLock <CSyncLock> sync(m_Sync);

	int nID = CDataManager::GetInstance()->VerifyNetLauncher(_inet_addr(pIP));
	if (nID > 0)
	{
		if (m_LauncherSessionList.find(nID) != m_LauncherSessionList.end())
			return NULL;

		CLauncherSession * pSession = new CLauncherSession();
		pSession->SetSessionID(nID);
		pSession->SetIp(pIP);
		m_LauncherSessionList[nID] = pSession;

#if defined (_SERVICEMANAGER_EX)
		g_FileLog.Log(LogType::_FILELOG, "Connected NetLauncher IP[%s] Port[%d]\n", pIP, nPort);
#else
		printf("Connected NetLauncher IP[%s] Port[%d]\n", pIP, nPort);
#endif // #if defined (_SERVICEMANAGER_EX)
		SendPacketToGsm(_T("msg=NInfo,%d,Online"), pSession->GetSessionID());

		if (CDataManager::GetInstance()->GetLauncherCount() == (int)m_LauncherSessionList.size())
			g_Log.Log(LogType::_FILELOG, L"All Launcher Connected\n");

		return pSession;
	}
#if defined (_SERVICEMANAGER_EX)
	g_Log.Log(LogType::_NORMAL, "Connect Failed NetLauncher IP[%s] Port[%d]\n", pIP, nPort);
#else
	printf("Connect Failed NetLauncher IP[%s] Port[%d]\n", pIP, nPort);
#endif // #if defined (_SERVICEMANAGER_EX)
	return NULL;
}

CServiceSession * CServiceManager::AddServiceSession(const char * pIP, unsigned short nPort)
{
	ScopeLock <CSyncLock> sync(m_Sync);

	int nID = CDataManager::GetInstance()->VerifyNetLauncher(_inet_addr(pIP));
	if (nID <= 0)		//�����ǰ� �ִ� ���ǿ����� ����常 �޽��ϴ�.
		return NULL;	
	
	CServiceSession * pSession = new CServiceSession;
	if (pSession)
	{
		static int s_nID = 0;
		_ASSERT(m_ServiceSessionList.find(s_nID) == m_ServiceSessionList.end());
		pSession->SetSessionID(s_nID);
		pSession->SetIp(pIP);
		m_ServiceSessionList[s_nID] = pSession;
		s_nID++;
	}
	return pSession;
}

CPatcherSession * CServiceManager::AddPatcherSession(const char * pIP, unsigned short nPort)
{
	ScopeLock <CSyncLock> sync(m_Sync);

	if (VerifyPatcher(pIP) == false) return NULL;

	int nID = CDataManager::GetInstance()->VerifyNetLauncher(_inet_addr(pIP));
	if (nID > 0)
	{
		if (m_PatcherSessionList.find(nID) != m_PatcherSessionList.end())
			return NULL;

		CPatcherSession * pSession = new CPatcherSession;
		if (!pSession) return NULL;

		pSession->SetSessionID(nID);
		pSession->SetIp(pIP);

		m_vPatcherSessionList.push_back(pSession);
		pSession->SetPatcherServiceID(nID);
		m_PatcherSessionList[nID] = pSession;

#if defined (_SERVICEMANAGER_EX)
		g_FileLog.Log(LogType::_FILELOG, "Connect NetLauncherPatcher Service IP[%s] Port[%d]\n", pIP, nPort);
#else
		printf("Connect NetLauncherPatcher Service IP[%s] Port[%d]\n", pIP, nPort);
#endif // #if defined (_SERVICEMANAGER_EX)

		if (CDataManager::GetInstance()->GetLauncherCount() == (int)m_PatcherSessionList.size())
			g_Log.Log(LogType::_FILELOG, L"All ESM Connected\n");
		return pSession;
	}
	//printf("Connect Failed UnManaged NetLauncherPatcher Service IP[%s] Port[%d]\n", pIP, nPort);
	return NULL;
}

CMonitorSession * CServiceManager::AddMonitorSession(const char * pIP, unsigned short nPort)
{
	ScopeLock <CSyncLock> sync(m_Sync);

	CMonitorSession * pSession = new CMonitorSession;
	if (!pSession) return NULL;

	static int nMonitorID = 1;
	pSession->SetSessionID(nMonitorID++);
	pSession->SetIp(pIP);

	m_MonitorSessionList.push_back(pSession);

#if defined (_SERVICEMANAGER_EX)
	g_Log.Log(LogType::_NORMAL, "Connect MonitorSession IP[%s] Port[%d]\n", pIP, nPort);
#else
	printf("Connect MonitorSession IP[%s] Port[%d]\n", pIP, nPort);
#endif // #if defined (_SERVICEMANAGER_EX)
	return pSession;
}

bool CServiceManager::DelConnection(CConnection * pCon, eConnectionKey eConType)
{
	ScopeLock <CSyncLock> Lock(m_Sync);
	if (eConType == CONNECTIONKEY_LAUNCHER)
	{
		std::map <UINT, CLauncherSession*>::iterator ii = m_LauncherSessionList.find(pCon->GetSessionID());
		if (ii != m_LauncherSessionList.end())
		{
			CLauncherSession * pSession = (*ii).second;
			int nID = (*ii).second->GetSessionID();
			//DeleteManagedService(nID);
			m_LauncherSessionList.erase(ii);
			
			g_Log.Log(LogType::_NORMAL, L"NetLauncher DisConnected NID:[%d] IP:[%S]\n", nID, pCon->GetIp());
			m_PatchWatcher.DelWatchContent(nID);

			if (m_LauncherSessionList.size() <=0)
			{
				g_Log.Log(LogType::_FILELOG, L"All Launcher Disconnet\n");

				if (m_LivePatcher.IsStarted())
				{
					m_LivePatcher.CancelPatch();
					g_Log.Log(LogType::_ERROR, L"LivePatchFailed\n");
				}
			}

			return true;
		}
		_DANGER_POINT();
	}
	else if (eConType == CONNECTIONKEY_SERVICEMANAGER)
	{
		std::map <UINT, CServiceSession*>::iterator ii = m_ServiceSessionList.find(pCon->GetSessionID());
		if (ii != m_ServiceSessionList.end())
		{
			CServiceSession * pSession = (*ii).second;
			int nSID = 0;
			sServiceState * pState = GetServiceStateByServiceSession((*ii).second->GetSessionID());
			if (pState)
			{
				pState->Init();
				nSID = pState->nSID;
			}
			else  //pState�� ã�� �� ���� ���� ��ϵ��� �ʴ� �����ų�, ������ ������������ ��Ų �����. ������ ���� �α׳���
				g_Log.Log(LogType::_FILELOG, L"[Session:%d, MID %d] DelConnection Failed\n",  pSession->GetSessionID(), pSession->GetMID());			

			if (m_AutoRestartList.size() > 0)
			{
				std::vector <int>::iterator ir;
				for (ir = m_AutoRestartList.begin(); ir != m_AutoRestartList.end(); )
				{
					if ((*ir) == nSID)
					{
						StartServiceEachAsync(nSID);
						ir = m_AutoRestartList.erase(ir);
					}
					else
						ir++;
				}
			}

			m_ServiceSessionList.erase(ii);
			SendPacketToGsm(_T("msg=PInfo,%d,Offline"), nSID);
			
			if (m_LivePatcher.IsStarted())
			{
				m_LivePatcher.CancelPatch();
				g_Log.Log(LogType::_ERROR, L"LivePatchFailed!!!!!!!!!!! Check Service!!!!!!\n");
			}

			if (m_ServiceSessionList.empty())
				g_Log.Log(LogType::_FILELOG, L"All Service Closed\n");

			return true;
		}
		_DANGER_POINT();
	}
	else if (eConType == CONNECTIONKEY_SERVICEPATCHER)
	{
		std::vector <CPatcherSession*>::iterator ii;
		for (ii = m_vPatcherSessionList.begin(); ii != m_vPatcherSessionList.end(); ii++)
		{
			if (pCon->GetSessionID() == (*ii)->GetSessionID())
			{
				std::map <UINT, CPatcherSession*>::iterator ih = m_PatcherSessionList.find((*ii)->GetPSID());
				if (ih != m_PatcherSessionList.end())
				{
					m_vPatcherSessionList.erase(ii);
					m_PatcherSessionList.erase(ih);
					return true;
				}
				return true;
			}
		}
		_DANGER_POINT();
	}
	else if (eConType == CONNECTIONKEY_SERVERMONITOR)
	{
		std::vector <CMonitorSession*>::iterator ii;
		for (ii = m_MonitorSessionList.begin(); ii != m_MonitorSessionList.end(); ii++)
		{
			if (pCon->GetSessionID() == (*ii)->GetSessionID())
			{
				m_MonitorSessionList.erase(ii);
				g_Log.Log(LogType::_NORMAL, L"ServiceMonitor Disconnected IP:[%S]\n", pCon->GetIp());
				return true;
			}
		}
	}
	return false;
}

void CServiceManager::ParseCommand(const char* pCmd, char* pRet)
{
	int nRet = g_pServiceManager->Parse(pCmd, pRet);
	if (nRet != 0)
		g_Log.Log(LogType::_ERROR, L"%s", nRet == -1 ? L"Argument Insufficiency\n" : L"Unkown Message\n");
	else if (strlen(pCmd) > 0)
		g_Log.Log(LogType::_NORMAL, L"ConsoleCmd [%S]\n", pCmd);
}

int CServiceManager::Parse(const char * pCmd, char * pRet)
{
	WCHAR wszTemp[2048];
	MultiByteToWideChar(CP_ACP, 0, pCmd, -1, wszTemp, 2048);

	std::wstring strCmd;
	strCmd = wszTemp;

	std::vector<std::wstring> wtokens;
	TokenizeW(strCmd, wtokens, L"|");

	std::vector<std::string> tokens;
	char szTemp[2048];
	for (int i = 0; i < (int)wtokens.size(); i++)
	{
		WideCharToMultiByte(CP_ACP, 0, wtokens[i].c_str(), -1, szTemp, 2048, NULL, NULL);
		std::string strTemp = szTemp;
		tokens.push_back(strTemp);
	}

	if (tokens.size() > 0)
	{
		if (!stricmp(tokens[0].c_str(), "makeselfdump"))
		{
			if (g_pServiceServer)
				g_pServiceServer->StoreMiniDump();
		}
		else if (!stricmp(tokens[0].c_str(), "testgsm"))
		{
			std::wstring wstrTemp;
			ToWideString(tokens[1], wstrTemp);

			g_pServiceManager->SendPacketToGsm(const_cast<TCHAR*>(wstrTemp.c_str()));
		}
		else if (!stricmp(tokens[0].c_str(), "reload"))
		{
			if (ReLoadStruct())
				SendUpdateNotice(SERVERMONITOR_UPDATENOTICE_SERVERINFO);
		}
		else if (!stricmp(tokens[0].c_str(),  "reloaduser")) ReLoadUser();
		else if (!stricmp(tokens[0].c_str(),  "resetclose")) ResetCloseService();
		else if (!stricmp(tokens[0].c_str(),  "start")) StartServiceAll(pRet);
		else if (!stricmp(tokens[0].c_str(),  "starteach"))
		{
			if (tokens.size() < 2)
				return -1;

			bool bForcePatch = false;
			if (tokens.size() > 2)
				bForcePatch = tokens[2] == "true" ? true : false;
			StartServiceEach(atoi(tokens[1].c_str()), bForcePatch, pRet);
		}
		else if (!stricmp(tokens[0].c_str(),  "starteachbylauncher"))
		{
			if (tokens.size() < 2)
				return -1;

			StartServiceEachbyLauncher(atoi(tokens[1].c_str()), pRet);
		}
		else if (!stricmp(tokens[0].c_str(),  "stopforce")) TerminateServiceForce(pRet);
		else if (!stricmp(tokens[0].c_str(),  "stopforceeach"))
		{
			if (tokens.size() <= 1)
				return -1;

			TerminateServiceEachForce(atoi(tokens[1].c_str()), pRet);
		}
		else if (!stricmp(tokens[0].c_str(),  "stopforceeachbylauncher"))
		{
			if (tokens.size() <= 1)
				return -1;

			TerminateServiceForceEachbyLauncher(atoi(tokens[1].c_str()), pRet);
		}
		else if (!stricmp(tokens[0].c_str(),  "stopall"))
		{
			if (tokens.size() <= 1)
				TerminateServiceAll(pRet);
			else
				TerminateServiceAll(pRet, atoi(tokens[1].c_str()));
		}
		else if (!stricmp(tokens[0].c_str(),  "cancelstop")) CancelTerminateProcess();
		else if (!stricmp(tokens[0].c_str(),  "stopdb")) TerminateServiceDB(pRet);
		else if (!stricmp(tokens[0].c_str(),  "stopeach"))
		{
			if (tokens.size() <= 1)
				return -1;	

			TerminateServiceEach(atoi(tokens[1].c_str()), pRet);
		}
		else if (!stricmp(tokens[0].c_str(),  "stopeachbylauncher"))
		{
			if (tokens.size() <= 1)
				return -1;

			TerminateServiceEachbyLauncher(atoi(tokens[1].c_str()), pRet);
		}
		else if (!stricmp(tokens[0].c_str(),  "patchurl"))
		{
			if (tokens.size() <= 1)
				return -1;

			PatchUpdateUrl(tokens[1].c_str(), pRet);
		}
		else if (!stricmp(tokens[0].c_str(),  "patchstruct")) PatchStruct(pRet);
		else if (!stricmp(tokens[0].c_str(),  "patchinfo"))
		{
			if (tokens.size() < 2)
				PatchInfo(0, pRet);
			else
				PatchInfo(atoi(tokens[1].c_str()), pRet);
		}
		else if (!stricmp(tokens[0].c_str(),  "patchsystem"))
		{
			if (tokens.size() < 2)
				PatchSystem(0, pRet);
			else
				PatchSystem(atoi(tokens[1].c_str()), pRet);
		}
		else if (!stricmp(tokens[0].c_str(),  "patchconfig"))
		{
			if (tokens.size() < 2)
				PatchConfig(0, pRet);
			else
				PatchConfig(atoi(tokens[1].c_str()), pRet);
		}
		else if (!stricmp(tokens[0].c_str(),  "patchexe"))
		{
			if (tokens.size() < 2)
				PatchExe(0, pRet);
			else
				PatchExe(atoi(tokens[1].c_str()), pRet);
		}
		else if (!stricmp(tokens[0].c_str(),  "patchfull"))
		{
			if (tokens.size() < 2)
				PatchFull(0, pRet);
			else
				PatchFull(atoi(tokens[1].c_str()), pRet);
		}
		else if (!stricmp(tokens[0].c_str(),  "patchapply")) PatchApply(pRet);
		else if (!stricmp(tokens[0].c_str(),  "infocopy"))
		{
			if (tokens.size() < 2)
				InfoCopy(0, pRet);
			else
				InfoCopy(atoi(tokens[1].c_str()), pRet);
		}
		else if (!stricmp(tokens[0].c_str(),  "patch"))
		{
			if (tokens.size() <= 1)
				return -1;

			Patch(tokens[1].c_str(), pRet);
		}
		else if (!stricmp(tokens[0].c_str(),  "patchdyncode")) PatchDyncode();
		//�۾� ��ü�� ext��ġ ������ �ϴ� �߱����� ĳ����ġ�� �̸��� ���߿� �ʿ��� ������ ����� Ǯ� ����
		else if (!stricmp(tokens[0].c_str(), "patchcash")) PatchExt();
		else if (!stricmp(tokens[0].c_str(), "cancelpatchcash")) CancelExtPatch();
		else if (!stricmp(tokens[0].c_str(), "filelist"))
		{
			if (tokens.size() <= 1)
				return -1;

			RecvFileList(tokens[1].c_str());
		}
		else if (!stricmp(tokens[0].c_str(),  "notice"))
		{
			if (tokens.size() <= 3)
				return -1;

			NoticeWorld(atoi(tokens[1].c_str()), atoi(tokens[2].c_str()), tokens[3].c_str());
		}
		else if (!stricmp(tokens[0].c_str(), "noticeall"))
		{
			if (tokens.size() <= 2)
				return -1;

			NoticeWholeWorld(atoi(tokens[1].c_str()), tokens[2].c_str());
		}
		else if (!stricmp(tokens[0].c_str(),  "noticechannel"))
		{
			if (tokens.size() <= 4)
				return -1;

			NoticeChannel(atoi(tokens[1].c_str()), atoi(tokens[2].c_str()), atoi(tokens[3].c_str()), tokens[4].c_str());
		}
		else if (!stricmp(tokens[0].c_str(),  "noticeserver"))
		{
			if (tokens.size() <= 4)
				return -1;

			NoticeServer(atoi(tokens[1].c_str()), atoi(tokens[2].c_str()), atoi(tokens[3].c_str()), tokens[4].c_str());
		}
		else if (!stricmp(tokens[0].c_str(),  "noticezone"))
		{
			if (tokens.size() <= 4)
				return -1;

			NoticeZone(atoi(tokens[1].c_str()), atoi(tokens[2].c_str()), atoi(tokens[3].c_str()), tokens[4].c_str());
		}
		else if (!stricmp(tokens[0].c_str(),  "cancelnotice"))
		{
			if (tokens.size() < 2)
				return -1;

			NoticeCancel(atoi(tokens[1].c_str()));
		}
		else if (!stricmp(tokens[0].c_str(),  "worldmaxuser"))
		{
			if (tokens.size() < 3)
				return -1;

			WorldMaxuserControl(atoi(tokens[1].c_str()), atoi(tokens[2].c_str()));
		}
		else if (!stricmp(tokens[0].c_str(),  "channelcontrol"))
		{
			if (tokens.size() < 4)
				return -1;
			int nChannelPopulation = tokens.size() == 5 ? atoi(tokens[4].c_str()) : -1;
			bool bIsOpen = !stricmp(tokens[3].c_str(), "true");
			ChannelControl(atoi(tokens[1].c_str()), atoi(tokens[2].c_str()), bIsOpen, nChannelPopulation);
		}
		else if (!stricmp(tokens[0].c_str(),  "populationcontrol"))
		{
			if (tokens.size() <= 3)
				return -1;
			PopulationControl(atoi(tokens[1].c_str()), atoi(tokens[2].c_str()), tokens[3] == "true" ? true : false);
		}
		else if (!stricmp(tokens[0].c_str(),  "affinitycontrol"))
		{
			if (tokens.size() <= 3)
				return -1;
			GameAffinityControl(atoi(tokens[1].c_str()), atoi(tokens[2].c_str()), atoi(tokens[3].c_str()));
		}
		else if (!stricmp(tokens[0].c_str(),  "eventupdate")) EventUpdate();
		else if (!stricmp(tokens[0].c_str(), "configupdate")) SimpleConfigUpdate();
		else if (!stricmp(tokens[0].c_str(),  "makedumpall")) MakeDumpAll();
		else if (!stricmp(tokens[0].c_str(),  "makedumpeach"))
		{
			if (tokens.size() <= 1)
				return -1;
			MakeDumEach(atoi(tokens[1].c_str()));
		}
		else if (!stricmp(tokens[0].c_str(),  "reservenotice"))
		{
			if (tokens.size() <= 1)
				return -1;
			ReserveNoticeUpdate(tokens[1].c_str(), pRet);
		}
		else if (!stricmp(tokens[0].c_str(),  "userban"))
		{
			if (tokens.size() <= 1)
				return -1;
			UserBanbyCharacterName(tokens[1].c_str());
		}
		else if (!stricmp(tokens[0].c_str(),  "userbanid"))
		{
			if (tokens.size() <= 1)
				return -1;
			UserBanbyAccountDBID(atoi(tokens[1].c_str()));
		}
		else if (!stricmp(tokens[0].c_str(),  "restraint"))
		{
			if (tokens.size() <= 1)
				return -1;
			UserRestraint(atoi(tokens[1].c_str()), pRet);
		}
		else if (!stricmp(tokens[0].c_str(),  "startlauncher")) StartLauncher(pRet);
		else if (!stricmp(tokens[0].c_str(),  "stoplauncher")) StopLauncher(pRet);
		else if (!stricmp(tokens[0].c_str(),  "starteachlauncher"))
		{
			if (tokens.size() < 2)
				return -1;

			StartEachLauncher(atoi(tokens[1].c_str()), pRet);
		}
		else if (!stricmp(tokens[0].c_str(),  "stopeachlauncher"))
		{
			if (tokens.size() < 2)
				return -1;

			StopEachLauncher(atoi(tokens[1].c_str()), pRet);
		}
		else if (!stricmp(tokens[0].c_str(),  "patchlauncher")) PatchNetLauncher(pRet);
		else if (!stricmp(tokens[0].c_str(),  "patchlaunchereach"))
		{
			if (tokens.size() < 2)
				return -1;

			PatchNetLauncherEach(atoi(tokens[1].c_str()), pRet);
		}
		else if (!stricmp(tokens[0].c_str(),  "checkpatch")) CheckPatch();
		else if (!stricmp(tokens[0].c_str(),  "clearpatch")) Clearpatch();
		else if (!stricmp(tokens[0].c_str(),  "checklauncher"))
		{
			if (tokens.size() < 2)
				CheckNetLauncher();
			else
				CheckNetLauncher(atoi(tokens[1].c_str()));
		}
		else if (!stricmp(tokens[0].c_str(),  "testlauncher"))
		{
			TestLauncher(0);
		}
		else if (!stricmp(tokens[0].c_str(),  "checktestlauncher")) CheckTestLauncher();
                else if (!stricmp(tokens[0].c_str(),  "reloaddyncode")) ReloadDyncode();
		else if (!stricmp(tokens[0].c_str(),  "checkalivelauncher")) CheckAliveNetLauncher();
		else if (!stricmp(tokens[0].c_str(),  "selectjoin"))
		{
			if (tokens.size() < 4)
				return -1;
			
			std::wstring szWStr;
			ToWideString(tokens[3], szWStr);

			SelectJoin(atoi(tokens[1].c_str()), atoi(tokens[2].c_str()), szWStr.c_str());
		}
		else if (!stricmp(tokens[0].c_str(),  "clearselectjoin"))
		{
			if (tokens.size() < 2)
				return -1;

			ClearSelectJoin(atoi(tokens[1].c_str()));
		}
		else if (!stricmp(tokens[0].c_str(),  "updatesale")) UpdateSaleData();
		else if (!stricmp(tokens[0].c_str(),  "patchbatch"))
		{
			if (tokens.size() < 2)
				PatchBatch(0, pRet);
			else
				PatchBatch(atoi(tokens[1].c_str()), pRet);
		}	
		else if (!stricmp(tokens[0].c_str(),  "runbatch"))  RunBatch(0, pRet);
		else if (!stricmp(tokens[0].c_str(),  "runbatcheach"))
		{
			if (tokens.size() <= 1)
				return -1;

			RunBatch(atoi(tokens[1].c_str()), pRet);
		}
		else if (!stricmp(tokens[0].c_str(),  "stopbatch"))  
		{
			if (tokens.size() < 2)
				StopBatch(0, pRet);
			else
				StopBatch(atoi(tokens[1].c_str()), pRet);
		}
		else if (!stricmp(tokens[0].c_str(),  "checkbatch"))
		{
			if (tokens.size() < 2)
				CheckBatch();
			else
				CheckBatch(atoi(tokens[1].c_str()));
		}
		else if (!stricmp(tokens[0].c_str(),  "userbanfile"))
		{
			if (tokens.size() < 2)
				return -1;

			UpdateBanFile(tokens[1].c_str());
		}
		else if (!stricmp(tokens[0].c_str(),  "farmupdate"))
		{
			if (pRet == NULL)
			ReqUpdateFarm();
		}
		else if (!stricmp(tokens[0].c_str(), "farmupdated"))
		{
		}
		else if (!stricmp(tokens[0].c_str(), "farmstart"))
		{
			StartFarm();
		}
		else if (!stricmp(tokens[0].c_str(),  "guildwarupdate"))
		{
			UpdateGuildWarSchedule();
		}
		else if (!stricmp(tokens[0].c_str(),  "createdb"))
		{
			if (tokens.size() < 2)
				return -1;
			else
				SendCreatEachDB(atoi(tokens[1].c_str()));
		}
		else if (!stricmp(tokens[0].c_str(), "reloadguildwar"))
		{
			if (tokens.size() >= 2)
				SendReloadGuildwar(atoi(tokens[1].c_str()));
			else
				SendReloadGuildwar(0);
		}
		else if (!stricmp(tokens[0].c_str(), "changelimitmax"))
		{
			if (tokens.size() < 3)
				return -1;

			SendChangeLimiteItemMax(atoi(tokens[1].c_str()), atoi(tokens[2].c_str()));
		}
		else if (!stricmp(tokens[0].c_str(),  "refreshguildware"))
		{
			if (tokens.size() < 2)
				return -1;
			else
				SendUpdateGuildWare(atoi(tokens[1].c_str()));
		}		
		else if (!stricmp(tokens[0].c_str(),  "patchesm"))
			PatchESM(0, pRet);
		else if (!stricmp(tokens[0].c_str(), "startesm"))
			StartESM();
		else if (!stricmp(tokens[0].c_str(), "stopesm"))
			StopESM();
		else if (!stricmp(tokens[0].c_str(),  "updateworldpvproom"))
		{			
			SendUpdateWorldPvPRoom();
		}		
		else if (!stricmp(tokens[0].c_str(),  "creategambleroom"))
		{			
			if (tokens.size() < 4)
				return -1;
			
			SendCreateGambleRoom(atoi(tokens[1].c_str()), atoi(tokens[2].c_str()), atoi(tokens[3].c_str()), atoi(tokens[4].c_str()) );			
		}
		else if (!stricmp(tokens[0].c_str(),  "stopgambleroom"))
		{	
			SendStopGambleRoom();
		}
		else if (!stricmp(tokens[0].c_str(),  "delgambleroom"))
		{	
			SendDelGambleRoom();
		}
		else if (!stricmp(tokens[0].c_str(),  "updatedwcstatus"))
		{
			SendUpdateDWCStatus();
		}
		else
			return -2;
		return 0;
	}
	return 0;
}

bool CServiceManager::ReLoadStruct()
{
	ScopeLock <CSyncLock> sync(m_Sync);	

	if (CDataManager::GetInstance()->LoadStruct() == false)
	{
		g_Log.Log(LogType::_FILELOG, L"DataLoad Failed\n");
		return false;
	}
	else
	{
		ClearList();
		std::vector <int> vSIDList;
		CDataManager::GetInstance()->GetCommandList(&vSIDList);
		for (int i = 0; i < (int)vSIDList.size(); i++)
		{
			const TServerExcuteData * pExe = CDataManager::GetInstance()->GetCommand(vSIDList[i]);
			if (pExe)
			{
				CServiceSession * pServiceSession = NULL;
				std::map <UINT, CServiceSession*>::iterator sessionitor;
				for (sessionitor = m_ServiceSessionList.begin(); sessionitor != m_ServiceSessionList.end(); sessionitor++)
				{
					if ((*sessionitor).second->GetMID() == pExe->nSID)
						pServiceSession = (*sessionitor).second;
				}

				if (MakeServiceList(pExe->nSID, pExe->nAssignedLauncherID, pExe->wszType, pServiceSession == NULL ? false : true) == false)
					_ASSERT_EXPR(0, L"[ReLoadStruct] Fail MakeServiceList");
			}
		}

		MakeWorldList();

		std::map <UINT, CLauncherSession*>::iterator ii;
		for (ii = m_LauncherSessionList.begin(); ii != m_LauncherSessionList.end(); ii++)
		{
			std::vector <TServerExcuteData> vExeList;
			CDataManager::GetInstance()->GetExeCommandList((*ii).second->GetSessionID(), &vExeList);
			bool bOther = CDataManager::GetInstance()->IsNeedOtherExcutePath((*ii).second->GetSessionID());
			(*ii).second->SendManagingExeItem(&vExeList, bOther);

			std::list <char> lList;
			CDataManager::GetInstance()->GetAssingedPatchTypeList((*ii).second->GetSessionID(), &lList);
			(*ii).second->SendConnectedResult(&lList, g_Config.wszPatchBaseURL, g_Config.wszPatchURL);
		}

		std::map <UINT, CPatcherSession*>::iterator ih;
		for (ih = m_PatcherSessionList.begin(); ih != m_PatcherSessionList.end(); ih++)
			(*ih).second->SendConnectedResult(g_Config.wszPatchBaseURL, g_Config.wszPatchURL);		

		g_Log.Log(LogType::_FILELOG, L"ReDataLoad OK\n");
		m_nContainerVersion++;
	}
	return true;
}

bool CServiceManager::ReLoadUser()
{
	ScopeLock <CSyncLock> sync(m_Sync);

	if (CDataManager::GetInstance()->LoadUser())
		return true;

	g_Log.Log(LogType::_ERROR, L"MonitorUser.xml Load Failed\n");
	return false;
}

int CServiceManager::CheckMonitorLogin(const char * pID, const char * pPass, int & nMonitorLevel, TP_SMVERSION Version)
{
	ScopeLock <CSyncLock> sync(m_Sync);

	if (GET_SM_VERSION() != Version) return ERROR_SM_LOGINFAIL_VERSIONUNMATCHED;

	bool bDuplicate = false;
	int nTempMonitorLevel = SERVERMONITOR_LEVEL_NONE;
	nMonitorLevel = SERVERMONITOR_LEVEL_NONE;

	bool bRet = CDataManager::GetInstance()->CheckMonitorUser(pID, pPass, nTempMonitorLevel, bDuplicate);
	if (bRet && bDuplicate == false)		//�ߺ��α��� �Ұ���� Ȯ��
	{
		if (strcmp("eyeadmin", pID))
		{
			std::vector <CMonitorSession*>::iterator ii;
			for (ii = m_MonitorSessionList.begin(); ii != m_MonitorSessionList.end(); ii++)
			{
				if (!strcmp((*ii)->GetMonitorID(), "eyeadmin")) continue;
				if ((*ii)->GetMonitorLevel() == nTempMonitorLevel)
					return ERROR_SM_LOGINFAIL_LEVEL_DUPLICATED;
			}
		}
	}
	nMonitorLevel = nTempMonitorLevel;
	return bRet == true ? ERROR_NONE : ERROR_SM_LOGINFAIL;
}

void CServiceManager::CalcPageSize(int &nFirst, int &nLast, BYTE cOffSet, BYTE cPageNum, int nMaxSize, int nWholeCount)
{
	int nLastPageNum = nWholeCount/((cOffSet <= 0 || cOffSet > nMaxSize) ? nMaxSize : cOffSet);
	cPageNum = nLastPageNum < cPageNum ? nLastPageNum : cPageNum;

	nFirst = cPageNum * cOffSet;
	nLast = nFirst + cOffSet;
}

void CServiceManager::SendExceptionReport(int nSID, int nExceptionType, const char * pDetail)
{
	ScopeLock <CSyncLock> sync(m_Sync);

	ReportToMonitorServiceState(nSID, _SERVICE_STATE_ONLINE, nExceptionType, pDetail);
	SendPacketToGsm(_T("msg=pInfo,%d,Exception,%d"), nSID, nExceptionType);
}

void CServiceManager::SendUpdateNotice(int nNotice)
{
	ScopeLock <CSyncLock> sync(m_Sync);

	std::vector <CMonitorSession*>::iterator im;
	for (im = m_MonitorSessionList.begin(); im != m_MonitorSessionList.end(); im++)
		(*im)->SendUpdateNotice(nNotice);
}

int CServiceManager::GetMeritList(int nContainerVersion, int nLastRecvID, BYTE & cIsLast, short & cCount, sMonitorChannelMeritInfo * pInfo, int nDestCountMax)
{
	int nWholeCount;
	ScopeLock <CSyncLock> sync(m_Sync);

	cIsLast = 1;
	nWholeCount = (int)m_ChannelMerit.size();
	if (nWholeCount <= 0) return 0;
	if (GetContainerVersion() != nContainerVersion) return nWholeCount;

	std::map <int, TMeritInfo>::iterator ii;
	//ii = nLastRecvID <= 0 ? m_ChannelMerit.begin() : m_ChannelMerit.find(nLastRecvID);
	if (nLastRecvID <= 0)
		ii = m_ChannelMerit.begin();
	else
	{
		ii = m_ChannelMerit.find(nLastRecvID);
		if (ii == m_ChannelMerit.end()) return nWholeCount;
		ii++;
	}
	if (ii == m_ChannelMerit.end()) return nWholeCount;

	for (cCount = 0; ii != m_ChannelMerit.end(); ii++, cCount++)
	{
		if (cCount >= _MERITSEND_CALCSIZE || cCount >= nDestCountMax) break;
		TMeritInfo * pMeritInfo = &(*ii).second;
		pInfo[cCount].nID = pMeritInfo->nID;
		pInfo[cCount].nMeritType = pMeritInfo->nMeritType;
		pInfo[cCount].nExtendValue = pMeritInfo->nExtendValue;
		pInfo[cCount].nMinLevel = pMeritInfo->nMinLevel;
		pInfo[cCount].nMaxLevel = pMeritInfo->nMaxLevel;
	}

	cIsLast = m_ChannelMerit.end() == ii ? 1 : 0;

	return nWholeCount;
}

int CServiceManager::GetNetLauncherList(int nContainerVersion, int nLastRecvLauncherID, BYTE & cIsLast, short & cCount, sMonitorNetLauncherInfo * pInfo, int nDestCountMax)
{
	int nWholeCount;
	ScopeLock <CSyncLock> sync(m_Sync);

	cIsLast = 1;
	std::map <int, TNetLauncherInfo> vList;
	std::map <int, TNetLauncherInfo>::iterator ii;
	CDataManager::GetInstance()->GetLauncherIds(&vList);
	
	nWholeCount = (int)vList.size();
	if (nWholeCount <= 0) return 0;
	if (GetContainerVersion() != nContainerVersion) return nWholeCount;

	//ii = nLastRecvLauncherID <= 0 ? vList.begin() : vList.find(nLastRecvLauncherID);
	if (nLastRecvLauncherID <= 0)
		ii = vList.begin();
	else
	{
		ii = vList.find(nLastRecvLauncherID);
		if (ii == vList.end()) return nWholeCount;
		ii++;
	}
	if (ii == vList.end()) return nWholeCount;

	for (cCount = 0; ii != vList.end(); ii++, cCount++)
	{
		if (cCount > _NETLAUNCHER_CALCSIZE || cCount >= nDestCountMax) break;
		TNetLauncherInfo * pNLInfo = &(*ii).second;
		pInfo[cCount].nID = pNLInfo->nNID;
		STRCPYW(pInfo[cCount].wszIP, IPLENMAX, pNLInfo->wszIP);
		STRCPYW(pInfo[cCount].wszPublicIP, IPLENMAX, pNLInfo->wszPublicIP);
	}

	cIsLast = vList.end() == ii ? 1 : 0;

	return nWholeCount;
}

int CServiceManager::GetServerList(int nContainerVersion, int nLastRecvSID, BYTE & cIsLast, short & cCount, sMonitorServerInfo * pInfo, int nDestCountMax)
{
	int nWholeCount;
	ScopeLock <CSyncLock> sync(m_Sync);

	cIsLast = 1;

	nWholeCount = (int)m_ServiceList.size();
	if (nWholeCount <= 0) return 0;
	if (GetContainerVersion() != nContainerVersion) return nWholeCount;

	std::map <int, sServiceState*>::iterator ii;
	//ii = nLastRecvSID <= 0 ? m_ServiceList.begin() : m_ServiceList.find(nLastRecvSID);
	if (nLastRecvSID <= 0)
		ii = m_ServiceList.begin();
	else
	{
		ii = m_ServiceList.find(nLastRecvSID);
		if (ii == m_ServiceList.end()) return nWholeCount;
		ii++;
	}
	if (ii == m_ServiceList.end()) return nWholeCount;

	for (cCount = 0; ii != m_ServiceList.end(); ii++, cCount++)
	{
		if (cCount >= _SERVERINFO_CALCSIZE || cCount >= nDestCountMax) break;

		pInfo[cCount].nSID = (*ii).second->nSID;
		pInfo[cCount].nManagedLauncherID = (*ii).second->nManagedLauncherID;
		pInfo[cCount].nServerState = (*ii).second->nState;
		WideCharToMultiByte(CP_ACP, 0, (*ii).second->szType, -1, pInfo[cCount].szType, NAMELENMAX, NULL, NULL);
		pInfo[cCount].nServerType = GetServerType((*ii).second->szType);
		STRCPYA(pInfo[cCount].szResourceRevision, 32, (*ii).second->szResource);
		STRCPYA(pInfo[cCount].szExeVersion, SERVERVERSIONMAX, (*ii).second->szVersion);
		memcpy(pInfo[cCount].cWorldIDs, (*ii).second->cWorldIDs, sizeof(pInfo[cCount].cWorldIDs));
	}

	cIsLast = m_ServiceList.end() == ii ? 1 : 0;

	return nWholeCount;
}

int CServiceManager::GetChannelList(int nContainerVersion, INT64 nLastRecvCHID, BYTE & cIsLast, short & cCount, sMonitorChannelInfo * pInfo, int nDestCountMax)
{
	int nWholeCount;
	ScopeLock <CSyncLock> sync(m_Sync);

	cIsLast = 1;

	nWholeCount = (int)m_ChannelList.size();
	if (nWholeCount <= 0) return 0;
	if (GetContainerVersion() != nContainerVersion) return nWholeCount;

	std::map <INT64, tMonitorChannelInfo>::iterator ii;
	//ii = nLastRecvCHID <= 0 ? m_ChannelList.begin() : m_ChannelList.find(nLastRecvCHID);
	if (nLastRecvCHID <= 0)
		ii = m_ChannelList.begin();
	else
	{
		ii = m_ChannelList.find(nLastRecvCHID);
		if (ii == m_ChannelList.end()) return nWholeCount;
		ii++;
	}
	if (ii == m_ChannelList.end()) return nWholeCount;

	for (cCount = 0; ii != m_ChannelList.end(); ii++, cCount++)
	{
		if (cCount >= _CHANNEL_CALCSIZE || cCount >= nDestCountMax) break;

		tMonitorChannelInfo * pChInfo = &(*ii).second;
		pInfo[cCount].nSID = pChInfo->nManagedID;
		pInfo[cCount].nWorldID = pChInfo->nWorldID;
		pInfo[cCount].nChannelID = pChInfo->Info.nChannelID;
		pInfo[cCount].nMapIdx = pChInfo->Info.nMapIdx;
		pInfo[cCount].nMaxUserCount = pChInfo->Info.nMaxUserCount;
		pInfo[cCount].nChannelAttribute = pChInfo->Info.nChannelAttribute;
		CDataManager::GetInstance()->GetChannelMapName(pChInfo->Info.nMapIdx, pInfo[cCount].wszMapName);
	}

	cIsLast = m_ChannelList.end() == ii ? 1 : 0;

	return nWholeCount;
}

bool CServiceManager::GetMangedIDList(std::vector <int> &vList)
{
	ScopeLock <CSyncLock> sync(m_Sync);

	std::map <UINT, CServiceSession*>::iterator ii;
	for (ii = m_ServiceSessionList.begin(); ii != m_ServiceSessionList.end(); ii++)
	{
		int nMID = (*ii).second->GetMID();
		vList.push_back(nMID);
	}
	return true;
}

int CServiceManager::GetWorldInfo(int nContainerVersion, BYTE & cCount, sMonitorWorldInfo * pInfo)
{
	ScopeLock <CSyncLock> sync(m_Sync);

	std::map <int, sMonitorWorldInfo>::iterator ii;
	for (ii = m_WorldInfo.begin(); ii != m_WorldInfo.end(); ii++)
		memcpy(&pInfo[cCount++], &(*ii).second, sizeof(sMonitorWorldInfo));

	return (int)m_WorldInfo.size();
}

int CServiceManager::GetLoginUpdateList(int nLastRecvLoginSID, BYTE & cIsLast, short & nCount, sUserCountInfo * pInfo, int nDestCountMax)
{
	int nWholeCount;
	ScopeLock <CSyncLock> sync(m_Sync);

	cIsLast = 1;

	std::map <UINT, int> TempSIDList;
	std::map <UINT, int>::iterator ii;
	
	nWholeCount = GetServiceCount(MANAGED_TYPE_LOGIN, &TempSIDList);
	if (nWholeCount <= 0) return 0;

	//ii = nLastRecvLoginSID <= 0 ? TempSIDList.begin() : TempSIDList.find(nLastRecvLoginSID);
	if (nLastRecvLoginSID <= 0)
		ii = TempSIDList.begin();
	else
	{
		ii = TempSIDList.find(nLastRecvLoginSID);
		if (ii == TempSIDList.end()) return nWholeCount;
		ii++;
	}
	if (ii == TempSIDList.end()) return nWholeCount;
	
	std::map <UINT, CServiceSession*>::iterator ih;
	for (int i = 0; ii != TempSIDList.end(); ii++, i++)
	{
		if (nCount >= _LOGINUPDATE_CALCSIZE || nCount >= nDestCountMax) break;

		for (ih = m_ServiceSessionList.begin(); ih != m_ServiceSessionList.end(); ih++)
		{
			if ((*ih).second->GetMID() == (*ii).first)
			{
				pInfo[nCount].nSID = (*ih).second->GetMID();
				pInfo[nCount].nUserCount = (*ih).second->m_nLoginUserCount;
				nCount++;
			}
		}
	}

	cIsLast = TempSIDList.end() == ii ? 1 : 0;

	return nWholeCount;
}

int CServiceManager::GetVillageUpdateList(INT64 nLastRecvCHID, BYTE & cIsLast, BYTE & cCount, sChannelUpdateInfo * pInfo, int nDestCountMax)
{
	int nWholeCount;
	ScopeLock <CSyncLock> sync(m_Sync);

	cIsLast = 1;

	nWholeCount = (int)m_ChannelList.size();
	if (nWholeCount <= 0) return 0;

	std::map <INT64, tMonitorChannelInfo>::iterator ii;
	//ii = nLastRecvCHID <= 0 ? m_ChannelList.begin() : m_ChannelList.find(nLastRecvCHID);
	if (nLastRecvCHID <= 0)
		ii = m_ChannelList.begin();
	else
	{
		ii = m_ChannelList.find(nLastRecvCHID);
		if (ii == m_ChannelList.end()) return nWholeCount;
		ii++;
	}
	if (ii == m_ChannelList.end()) return nWholeCount;

	for (cCount = 0; ii != m_ChannelList.end(); ii++, cCount++)
	{
		if (cCount >= _VILLAGEUPDATE_CALCSIZE || cCount >= nDestCountMax) break;

		tMonitorChannelInfo * pChInfo = &(*ii).second;
		pInfo[cCount].nSID = pChInfo->nManagedID;
		pInfo[cCount].bShow = pChInfo->Info.bShow;
		pInfo[cCount].bVisibility = pChInfo->Info.bVisibility;
		pInfo[cCount].nChannelID = pChInfo->Info.nChannelID;
		pInfo[cCount].nChannelUserCount = pChInfo->Info.nCurrentUserCount;
		pInfo[cCount].nLimitLevel = pChInfo->Info.nLimitLevel;
		pInfo[cCount].nMeritBonusID = pChInfo->Info.nMeritBonusID;
		pInfo[cCount].nServerID = pChInfo->Info.nServerID;
		pInfo[cCount].cThreadID = pChInfo->Info.cThreadID;
	}

	cIsLast = m_ChannelList.end() == ii ? 1 : 0;

	return nWholeCount;
}


int CServiceManager::GetGameUpdateList(int nLastRecvGameSID, BYTE & cIsLast, short & nCount, sGameUserInfo * pInfo, int nDestCountMax)
{
	int nWholeCount;
	ScopeLock <CSyncLock> sync(m_Sync);

	cIsLast = 1;

	std::map <UINT, int> TempSIDList;
	std::map <UINT, int>::iterator ii;
	
	nWholeCount = GetServiceCount(MANAGED_TYPE_GAME, &TempSIDList);
	if (nWholeCount <= 0) return 0;

	//ii = nLastRecvGameSID <= 0 ? TempSIDList.begin() : TempSIDList.find(nLastRecvGameSID);
	if (nLastRecvGameSID <= 0)
		ii = TempSIDList.begin();
	else
	{
		ii = TempSIDList.find(nLastRecvGameSID);
		if (ii == TempSIDList.end()) return nWholeCount;
		ii++;
	}
	if (ii == TempSIDList.end()) return nWholeCount;
	
	std::map <UINT, CServiceSession*>::iterator ih;
	for (int i = 0; ii != TempSIDList.end(); ii++, i++)
	{
		if (nCount >= _GAMEUPDATE_CALCSIZE || nCount >= nDestCountMax) break;

		for (ih = m_ServiceSessionList.begin(); ih != m_ServiceSessionList.end(); ih++)
		{
			if ((*ih).second->GetMID() == (*ii).first)
			{
				pInfo[nCount].nSID = (*ih).second->GetMID();
				pInfo[nCount].nUserCount = (*ih).second->m_nGameUserCount;
				pInfo[nCount].cIsOpened = (*ih).second->m_bZeroPopulation == true ? false : true;
				pInfo[nCount].nRoomCount = (*ih).second->m_nGameRoomCount;
				pInfo[nCount].nTotalRoomCount = (*ih).second->m_nTotalRoomCount;
				nCount++;
			}
		}
	}

	cIsLast = TempSIDList.end() == ii ? 1 : 0;

	return nWholeCount;
}


void CServiceManager::GetFarmUpdateList(short nFarmCount, short & nCount, TFarmStatus * pInfo, int nDestCountMax)
{
	ScopeLock <CSyncLock> sync(m_Sync);

	bool bAddFarm = false;
	nCount = 0;

	for (int i=0; i<WORLDCOUNTMAX; i++)
	{
		std::map <int, std::vector<TFarmStatus>>::iterator managed_itor;
		for (managed_itor = m_FarmStatus[i].begin(); managed_itor != m_FarmStatus[i].end(); managed_itor++)
		{
			std::vector<TFarmStatus>::iterator farm_itor = managed_itor->second.begin();
			for (; farm_itor != managed_itor->second.end(); farm_itor++)
			{
				TFarmStatus& pStatus = *farm_itor;
			
				nFarmCount--;

				if (pStatus.nFarmDBID > 0 && nFarmCount < 0)
				{
					if (nCount >= _FARMUPDATE_CALCSIZE || nCount >= nDestCountMax) break;

					pInfo[nCount].bActivate = pStatus.bActivate;
					pInfo[nCount].nFarmDBID = pStatus.nFarmDBID;
					pInfo[nCount].nManagedID = pStatus.nManagedID;
					pInfo[nCount].nFarmCurUserCount = pStatus.nFarmCurUserCount;
					nCount++;
				}
			}
		}
	}
}


int CServiceManager::GetServerStateList(int nLastRecvSID, BYTE & cIsLast, short & nCount, sServerStateInfo * pInfo, int nDestCountMax)
{
	int nWholeCount;
	ScopeLock <CSyncLock> sync(m_Sync);

	cIsLast = 1;

	nWholeCount = (int)m_ServiceList.size();

	std::map <int, sServiceState*>::iterator ii;
	//ii = nLastRecvSID <= 0 ? m_ServiceList.begin() : m_ServiceList.find(nLastRecvSID);
	if (nLastRecvSID <= 0)
		ii = m_ServiceList.begin();
	else
	{
		ii = m_ServiceList.find(nLastRecvSID);
		if (ii == m_ServiceList.end()) return 0;
		ii++;
	}
	if (ii == m_ServiceList.end()) return 0;

	for (int i = 0; ii != m_ServiceList.end(); ii++, i++)
	{
		if (nCount >= _SERVERSTATE_CALCSIZE || nCount >= nDestCountMax) break;

		pInfo[nCount].nServerState = (*ii).second->nState;
		pInfo[nCount].nSID = (*ii).second->nSID;

		nCount++;
	}

	cIsLast = m_ServiceList.end() == ii ? 1 : 0;

	return nWholeCount;
}

#include "GsmCmdProcess.h"
int CServiceManager::StartServiceAll(char * pMsg)
{
	if (IsCloseService(0))
	{
		g_Log.Log(LogType::_FILELOG, L"Terminate Process Busy\n");
		if (pMsg) strcpy(pMsg, "Terminate Process Busy");
		return ERROR_SM_TERMINATE_BUSY;
	}

	//Start Service Batch Flow
	ScopeLock <CSyncLock> sync(m_Sync);

	if (m_LauncherSessionList.size() != CDataManager::GetInstance()->GetLauncherCnt())
	{
		std::vector <TNetLauncherInfo> vList;
		CDataManager::GetInstance()->GetLauncherIds(&vList);

		std::map <UINT, CLauncherSession*>::iterator itorlauncher;
		for (itorlauncher = m_LauncherSessionList.begin(); itorlauncher != m_LauncherSessionList.end(); itorlauncher++)
		{
			std::vector <TNetLauncherInfo>::iterator itorv;
			for (itorv = vList.begin(); itorv != vList.end(); )
			{
				if ((*itorv).nNID == (*itorlauncher).second->GetSessionID())
					itorv = vList.erase(itorv);
				else
					itorv++;
			}
		}

		for (int i = 0; i < (int)vList.size(); i++)
			g_Log.Log(LogType::_FILELOG, L"Launcher Connection Insufficiency NID[%d] NIP[%s]\n", vList[i].nNID, vList[i].wszIP);
		if (pMsg) strcpy(pMsg, "StartServiceAll Failed Launcher Connection Insufficiency");
		return ERROR_SM_NETLAUNCHER_INSUFFISIENCY;
	}

	std::map <UINT, CLauncherSession*>::iterator ih;
	for (ih = m_LauncherSessionList.begin(); ih != m_LauncherSessionList.end(); ih++)
		(*ih).second->SendInfoCopy(CDataManager::GetInstance()->IsNeedOtherExcutePath((*ih).first));

	std::map <int, sServiceState*>::iterator ii;
	for (ii = m_ServiceList.begin(); ii != m_ServiceList.end(); ii++)
	{
		if ((*ii).second->nState == _SERVICE_STATE_OFFLINE || (*ii).second->nState == _SERVICE_STATE_TERMINATED)
		{
			const TServerExcuteData * pExe = CDataManager::GetInstance()->GetCommand((*ii).second->nSID);
			if (pExe)
				ExcuteProcess(pExe);
			else
			{
				g_Log.Log(LogType::_FILELOG, L"StartServiceAll Failed SID[%d]\n", (*ii).second->nSID);
				if (pMsg) strcpy(pMsg, "StartServiceAll Failed");
			}
		}
	}

	g_Log.Log(LogType::_FILELOG, L"StartServiceAll OK\n");
	if (pMsg) strcpy(pMsg, "StartServiceAll OK");
	return ERROR_NONE;
}

int CServiceManager::StartServiceEach(int nSID, bool bForcePatch, char * pMsg)
{
	if (IsCloseService(nSID))
	{
		g_Log.Log(LogType::_FILELOG, L"Terminate Process Busy\n");
		if (pMsg) strcpy(pMsg, "Terminate Process Busy");
		return ERROR_SM_TERMINATE_BUSY;
	}

	ScopeLock <CSyncLock> sync(m_Sync);
	
	bool bRet = false;
	const TServerExcuteData * pExe = CDataManager::GetInstance()->GetCommand(nSID);
	if (pExe)
		bRet = ExcuteProcess(pExe, bForcePatch);

	if (bRet)
	{
		g_Log.Log(LogType::_FILELOG, L"StartServiceEach OK\n");
		if (pMsg) strcpy(pMsg, "StartServiceEach OK");
		return ERROR_NONE;
	}
	else
	{
		g_Log.Log(LogType::_FILELOG, L"StartServiceEach Failed\n");
		if (pMsg) strcpy(pMsg, "StartServiceEach Failed");
		return ERROR_SM_STARTEACH_FAILED;
	}
	return ERROR_NONE;
}

int CServiceManager::StartServiceEachbyLauncher(int nNID, char * pMsg)
{
	CLauncherSession * pSession = NULL;

	ScopeLock <CSyncLock> sync(m_Sync);

	std::map <UINT, CLauncherSession*>::iterator ih;
	for (ih = m_LauncherSessionList.begin(); ih != m_LauncherSessionList.end(); ih++)
	{
		if ((*ih).second->GetSessionID() == nNID)
		{
			pSession = (*ih).second;
			break;
		}
	}

	if (pSession == NULL)
	{
		g_Log.Log(LogType::_FILELOG, L"NetLauncher not Found\n");
		if (pMsg) strcpy(pMsg, "NetLauncher not Found");
		return ERROR_SM_SELECTED_NETLAUNCHER_NOTFOUND;
	}

	std::map <int, sServiceState*>::iterator ii;
	for (ii = m_ServiceList.begin(); ii != m_ServiceList.end(); ii++)
	{
		const TServerExcuteData * pExe = CDataManager::GetInstance()->GetCommand((*ii).second->nSID);
		if (pExe)
		{
			if (pExe->nAssignedLauncherID == nNID)
				ExcuteProcess(pExe);
		}
	}
	return ERROR_NONE;
}

void CServiceManager::ResetCloseService(char * pMsg)
{
	m_Sync.Lock();

	std::vector <_CLOSESERVICE>::iterator ii;
	for (ii = m_CloseServieList.begin(); ii != m_CloseServieList.end(); )
		ii = m_CloseServieList.erase(ii);
	m_CloseServieList.clear();

	m_Sync.UnLock();
}


int CServiceManager::TerminateServiceForce(char * pMsg)
{
	m_Sync.Lock();

	std::map <int, sServiceState*>::iterator ii;
	for (ii = m_ServiceList.begin(); ii != m_ServiceList.end(); ii++)
	{
		std::map <UINT, CLauncherSession*>::iterator ih;
		for (ih = m_LauncherSessionList.begin(); ih != m_LauncherSessionList.end(); ih++)
			(*ih).second->SendTerminateProcess((*ii).second->nSID);
	}

	m_Sync.UnLock();

	EjectForce();
	g_Log.Log(LogType::_FILELOG, L"All Service Terminated Force\n");
	if (pMsg) strcpy(pMsg, "TerminateServiceAll Force Ok");
	m_bCloseService = false;

	SendPacketToGsm(_T("msg=Farm,0"));
	return ERROR_NONE;
}

int CServiceManager::TerminateServiceEachForce(int nSID, char * pMsg)
{
	/*if (IsService(nSID) == false)
		return ERROR_SM_TERMINATE_SERVICE_NOTFOUND;*/

	m_Sync.Lock();	

	std::map <UINT, CLauncherSession*>::iterator ii;
	for (ii = m_LauncherSessionList.begin(); ii != m_LauncherSessionList.end(); ii++)
		(*ii).second->SendTerminateProcess(nSID);	

	m_Sync.UnLock();
	g_Log.Log(LogType::_FILELOG, L"TerminateServiceEach Force Ok\n");
	if (pMsg) strcpy(pMsg, "TerminateServiceEach Force Ok");
	return ERROR_NONE;
}

bool CServiceManager::TerminateServiceEachForceAsync(int nSID)
{
	//���� ����
	std::map <UINT, CLauncherSession*>::iterator ii;
	for (ii = m_LauncherSessionList.begin(); ii != m_LauncherSessionList.end(); ii++)
		(*ii).second->SendTerminateProcess(nSID, true);

	std::vector <int>::iterator itor;
	for (itor = m_AutoRestartList.begin(); itor != m_AutoRestartList.end(); itor++)
	{
		if ((*itor) == nSID)
			break;
	}

	g_Log.Log(LogType::_FILELOG, L"TerminateServiceEach Force Ok\n");

	if (itor == m_AutoRestartList.end())
	{
		m_AutoRestartList.push_back(nSID);
		return true;
	}
	return false;
}

void CServiceManager::StartServiceEachAsync(int nSID)
{
	//���� ����
	if (IsCloseService(nSID))
	{
		g_Log.Log(LogType::_FILELOG, L"Terminate Process Busy\n");
		return;
	}
	
	bool bRet = false;
	const TServerExcuteData * pExe = CDataManager::GetInstance()->GetCommand(nSID);
	if (pExe)
		bRet = ExcuteProcess(pExe, false);

	if (bRet)
		g_Log.Log(LogType::_FILELOG, L"StartServiceEach OK\n");
	else
		g_Log.Log(LogType::_FILELOG, L"StartServiceEach Failed\n");
}

int CServiceManager::TerminateServiceAll(char * pMsg, int nAfterClose)
{
	if (IsCloseService(0))
	{
		g_Log.Log(LogType::_FILELOG, L"Terminate Process Busy\n");
		if (pMsg) strcpy(pMsg, "Terminate Process Busy");
		return ERROR_SM_TERMINATE_BUSY;
	}

	if (m_LauncherSessionList.empty())
	{
		g_Log.Log(LogType::_FILELOG, L"There Is No TerminateSessions\n");
		if (pMsg) strcpy(pMsg, "There Is No TerminateSessions");
		return ERROR_SM_NETLAUNCHER_NOTFOUND;
	}

	SendPacketToGsm(_T("msg=Farm,0"));

	if (nAfterClose > 0)
	{
		__time64_t _tCloseTime;
		if (m_ServiceCloser.IsClosing(_tCloseTime) == false)
		{
			if (m_ServiceCloser.ServiceClose(nAfterClose))
				g_Log.Log(LogType::_NORMAL, L"Service Will Close [Min:%d] Later\n", nAfterClose);
		}
		else
			g_Log.Log(LogType::_ERROR, L"Error TerminateProcess Already On\n");
	}
	else
	{
		{
			ScopeLock <CSyncLock> Lock(m_Sync);

			std::map <int, sServiceState*>::iterator ii;
			for (ii = m_ServiceList.begin(); ii != m_ServiceList.end(); ii++)
			{
				if (wcscmp((*ii).second->szType, L"DB") && wcscmp((*ii).second->szType, L"Log") 
					&& wcscmp((*ii).second->szType, L"Village") && wcscmp((*ii).second->szType, L"Game") && wcscmp((*ii).second->szType, L"Master") \
					&& wcscmp((*ii).second->szType, L"Login"))
				{
					std::map <UINT, CLauncherSession*>::iterator ih;
					for (ih = m_LauncherSessionList.begin(); ih != m_LauncherSessionList.end(); ih++)
						(*ih).second->SendTerminateProcess((*ii).second->nSID);
				}
			}

			std::map <UINT, CServiceSession*>::iterator ih;
			for (ih = m_ServiceSessionList.begin(); ih != m_ServiceSessionList.end(); ih++)
			{
				if ((*ih).second->GetType() == MANAGED_TYPE_VILLAGE || (*ih).second->GetType() == MANAGED_TYPE_GAME || (*ih).second->GetType() == MANAGED_TYPE_LOGIN)
				{
					(*ih).second->SendCloseService();

					_CLOSESERVICE close;
					memset(&close, 0, sizeof(close));

					close.nMID = (*ih).second->GetMID();
					close.nInsertTick = timeGetTime();

					m_CloseServieList.push_back(close);
				}
			}
		}

		EjectServiceAll();
		g_Log.Log(LogType::_FILELOG, L"All Service Terminated (without db, village, game)\n");
		if (pMsg) strcpy(pMsg, "TerminateServiceAll (without dbvillage, game) Ok");
		m_bCloseService = true;
	}

	return ERROR_NONE;
}

int CServiceManager::TerminateServiceDB(char * pMsg, bool bWithOutLock)
{
	if (IsCloseService(0))
	{
		g_Log.Log(LogType::_FILELOG, L"Terminate Process Busy\n");
		if (pMsg) strcpy(pMsg, "Terminate Process Busy");
		return ERROR_SM_TERMINATE_BUSY;
	}

	if (m_LauncherSessionList.empty())
	{
		g_Log.Log(LogType::_FILELOG, L"There Is No TerminateSessions\n");
		if (pMsg) strcpy(pMsg, "There Is No TerminateSessions");
		return ERROR_SM_NETLAUNCHER_NOTFOUND;
	}

	if (bWithOutLock == false)
		m_Sync.Lock();

	std::map <int, sServiceState*>::iterator ii;
	for (ii = m_ServiceList.begin(); ii != m_ServiceList.end(); ii++)
	{
		if (!wcscmp((*ii).second->szType, L"DB") || !wcscmp((*ii).second->szType, L"Log") || !wcscmp((*ii).second->szType, L"Master"))
		{
			std::map <UINT, CLauncherSession*>::iterator ih;
			for (ih = m_LauncherSessionList.begin(); ih != m_LauncherSessionList.end(); ih++)
				(*ih).second->SendTerminateProcess((*ii).second->nSID);
		}
	}

	if (bWithOutLock == false)
		m_Sync.UnLock();

	EjectServiceDB();
	g_Log.Log(LogType::_FILELOG, L"DB Service Terminated [Callby:%s]\n", bWithOutLock == true ? L"IdleProcess" : L"ConsolCmd");
	if (pMsg) strcpy(pMsg, "TerminateServiceDB Ok");
	return ERROR_NONE;
}

int CServiceManager::TerminateServiceEach(int nSID, char * pMsg, bool bReCreate)
{
	if (IsCloseService(nSID))
	{
		g_Log.Log(LogType::_FILELOG, L"Terminate Process Busy\n");
		if (pMsg) strcpy(pMsg, "Terminate Process Busy");
		return ERROR_SM_TERMINATE_BUSY;
	}

	m_Sync.Lock();

	std::map <UINT, CServiceSession*>::iterator ih;
	for (ih = m_ServiceSessionList.begin(); ih != m_ServiceSessionList.end(); ih++)
	{
		if ((*ih).second->GetMID() == nSID)
		{
			if ((*ih).second->GetType() == MANAGED_TYPE_VILLAGE || (*ih).second->GetType() == MANAGED_TYPE_GAME || (*ih).second->GetType() == MANAGED_TYPE_LOGIN)
			{
				(*ih).second->SendCloseService();

				_CLOSESERVICE close;
				memset(&close, 0, sizeof(close));

				close.nMID = (*ih).second->GetMID();
				close.nInsertTick = timeGetTime();

				m_CloseServieList.push_back(close);
			}
			else
			{
				std::map <UINT, CLauncherSession*>::iterator ii;
				for (ii = m_LauncherSessionList.begin(); ii != m_LauncherSessionList.end(); ii++)
					(*ii).second->SendTerminateProcess(nSID);
			}

			if (bReCreate)
			{
				std::vector <int>::iterator itor;
				for (itor = m_AutoRestartList.begin(); itor != m_AutoRestartList.end(); itor++)
				{
					if ((*itor) == nSID)
						break;
				}
				if (itor == m_AutoRestartList.end())
					m_AutoRestartList.push_back(nSID);
			}
		}
	}

	m_Sync.UnLock();
	if (pMsg) strcpy(pMsg, "TerminateServiceEach Ok");
	return ERROR_NONE;
}

int CServiceManager::TerminateServiceEachbyLauncher(int nNID, char * pMsg)
{
	m_Sync.Lock();

	std::map <int, sServiceState*>::iterator ii;
	for (ii = m_ServiceList.begin(); ii != m_ServiceList.end(); ii++)
	{
		if (wcscmp((*ii).second->szType, L"DB") && wcscmp((*ii).second->szType, L"Log") 
			&& wcscmp((*ii).second->szType, L"Village") && wcscmp((*ii).second->szType, L"Game") \
			&& wcscmp((*ii).second->szType, L"Login"))
		{
			std::map <UINT, CLauncherSession*>::iterator ih;
			for (ih = m_LauncherSessionList.begin(); ih != m_LauncherSessionList.end(); ih++)
				(*ih).second->SendTerminateProcess((*ii).second->nSID);
		}
		else
		{
			if ((*ii).second->nManagedLauncherID == nNID)
			{
				std::map <UINT, CServiceSession*>::iterator ih;
				for (ih = m_ServiceSessionList.begin(); ih != m_ServiceSessionList.end(); ih++)
				{
					if ((*ih).second->GetMID() == (*ii).second->nSID)
					{
						if ((*ih).second->GetType() == MANAGED_TYPE_VILLAGE || (*ih).second->GetType() == MANAGED_TYPE_GAME || (*ih).second->GetType() == MANAGED_TYPE_LOGIN)
						{
							(*ih).second->SendCloseService();

							_CLOSESERVICE close;
							memset(&close, 0, sizeof(close));

							close.nMID = (*ih).second->GetMID();
							close.nInsertTick = timeGetTime();

							m_CloseServieList.push_back(close);
						}
					}
					else
					{
						std::map <UINT, CLauncherSession*>::iterator ij;
						for (ij = m_LauncherSessionList.begin(); ij != m_LauncherSessionList.end(); ij++)
						{
							if ((*ij).second->GetSessionID() == nNID)
							{
								(*ij).second->SendTerminateProcess((*ii).second->nSID);
							}
						}
					}
				}
			}
		}
	}	

	m_Sync.UnLock();
	if (pMsg) strcpy(pMsg, "TerminateServiceEachbyLauncher Ok");

	return ERROR_NONE;
}

int CServiceManager::TerminateServiceForceEachbyLauncher(int nNID, char * pMsg)
{
	CLauncherSession * pSession = NULL;

	ScopeLock <CSyncLock> sync(m_Sync);

	std::map <UINT, CLauncherSession*>::iterator ih;
	for (ih = m_LauncherSessionList.begin(); ih != m_LauncherSessionList.end(); ih++)
	{
		if ((*ih).second->GetSessionID() == nNID)
		{
			pSession = (*ih).second;
			break;
		}
	}

	if (pSession == NULL)
	{
		g_Log.Log(LogType::_FILELOG, L"NetLauncher not Found\n");
		if (pMsg) strcpy(pMsg, "NetLauncher not Found");
		return ERROR_SM_NETLAUNCHER_NOTFOUND;
	}

	pSession->SendTerminateProcess();
	return ERROR_NONE;
}

int CServiceManager::CancelTerminateProcess()
{
	if (m_ServiceCloser.CancelServiceClose())
	{
		g_Log.Log(LogType::_NORMAL, L"Cancel TerminateProcess\n");
	}
	else
	{
		g_Log.Log(LogType::_ERROR, L"No TerminateProcess\n");
	}
	return ERROR_NONE;
}

void CServiceManager::PatchESM(int nNID, char * pMsg)
{
	if (m_LauncherSessionList.empty())
	{
		g_Log.Log(LogType::_ERROR, L"No Launcher\n");
		return;
	}

	if (m_PatchWatcher.HasWatchingContent())
	{
		g_Log.Log(LogType::_FILELOG, L"PatchESM Fail try later");
		if (pMsg) strcpy(pMsg, "PatchESM Fail try later");
		return;
	}

	SendPatch(nNID, L"ESM", m_nPatchID++);

	if (pMsg) strcpy(pMsg, "ESM Patch OK");
}

int CServiceManager::StartESM()
{
	m_Sync.Lock();

	std::map <UINT, CLauncherSession*>::iterator ih;
	for (ih = m_LauncherSessionList.begin(); ih != m_LauncherSessionList.end(); ih++)
		(*ih).second->SendStartESM();
	
	m_Sync.UnLock();
	return ERROR_NONE;
}

int CServiceManager::StopESM()
{
	m_Sync.Lock();

	std::map <UINT, CLauncherSession*>::iterator ih;
	for (ih = m_LauncherSessionList.begin(); ih != m_LauncherSessionList.end(); ih++)
		(*ih).second->SendStopESM();
	m_Sync.UnLock();
	return ERROR_NONE;
}

void CServiceManager::NoticeWholeWorld(int nSec, const char * pMsg)
{
	m_Sync.Lock();

	std::map <UINT, CServiceSession*>::iterator ii;
	for (ii = m_ServiceSessionList.begin(); ii != m_ServiceSessionList.end(); ii++)
		if ((*ii).second->GetType() == MANAGED_TYPE_MASTER)
			(*ii).second->SendNoticeMassage(_NOTICETYPE_WORLD, 0, 0, 0, nSec, pMsg);

	m_Sync.UnLock();
}

void CServiceManager::NoticeWorld(int nMasterMID, int nSec, const char * pMsg)
{
	m_Sync.Lock();

	std::map <UINT, CServiceSession*>::iterator ii;
	for (ii = m_ServiceSessionList.begin(); ii != m_ServiceSessionList.end(); ii++)
		if ((*ii).second->GetMID() == nMasterMID && (*ii).second->GetType() == MANAGED_TYPE_MASTER)
			(*ii).second->SendNoticeMassage(_NOTICETYPE_WORLD, 0, 0, 0, nSec, pMsg);

	m_Sync.UnLock();
}

void CServiceManager::NoticeServer(int nMasterMID, int nMID, int nSec, const char * pMsg)
{
	m_Sync.Lock();

	std::map <UINT, CServiceSession*>::iterator ii;
	for (ii = m_ServiceSessionList.begin(); ii != m_ServiceSessionList.end(); ii++)
		if ((*ii).second->GetMID() == nMasterMID && (*ii).second->GetType() == MANAGED_TYPE_MASTER)
			(*ii).second->SendNoticeMassage(_NOTICETYPE_EACHSERVER, nMID, 0, 0, nSec, pMsg);

	m_Sync.UnLock();
}

void CServiceManager::NoticeChannel(int nMasterMID, int nChannelID, int nSec, const char * pMsg)
{
	m_Sync.Lock();

	std::map <UINT, CServiceSession*>::iterator ii;
	for (ii = m_ServiceSessionList.begin(); ii != m_ServiceSessionList.end(); ii++)
		if (((*ii).second->GetMID() == nMasterMID || nMasterMID == -1) && (*ii).second->GetType() == MANAGED_TYPE_MASTER)
			(*ii).second->SendNoticeMassage(_NOTICETYPE_CHANNEL, 0, nChannelID, 0, nSec, pMsg);

	m_Sync.UnLock();
}

void CServiceManager::NoticeZone(int nMasterMID, int nMapIdx, int nSec, const char * pMsg)
{
	m_Sync.Lock();

	std::map <UINT, CServiceSession*>::iterator ii;
	for (ii = m_ServiceSessionList.begin(); ii != m_ServiceSessionList.end(); ii++)
		if (((*ii).second->GetMID() == nMasterMID || nMasterMID == -1) && (*ii).second->GetType() == MANAGED_TYPE_MASTER)
			(*ii).second->SendNoticeMassage(_NOTICETYPE_ZONE, 0, 0, nMapIdx, nSec, pMsg);

	m_Sync.UnLock();
}

void CServiceManager::NoticeCancel(int nMasterMID)
{
	ScopeLock <CSyncLock> sync(m_Sync);

	std::map <UINT, CServiceSession*>::iterator ii;
	for (ii = m_ServiceSessionList.begin(); ii != m_ServiceSessionList.end(); ii++)
		if (((*ii).second->GetMID() == nMasterMID || nMasterMID == -1) && (*ii).second->GetType() == MANAGED_TYPE_MASTER)
			(*ii).second->SendNoticeCancel();
}

void CServiceManager::MakeDumpAll()
{
	if (m_LauncherSessionList.empty())
	{
		g_Log.Log(LogType::_ERROR, L"No Launcher\n");
		return;
	}

	m_Sync.Lock();

	std::map <UINT, CServiceSession*>::iterator ii;
	for (ii = m_ServiceSessionList.begin(); ii != m_ServiceSessionList.end(); ii++)
		(*ii).second->SendMakeDump();

	m_Sync.UnLock();
}

int CServiceManager::MakeDumEach(int nMID)
{
	if (m_LauncherSessionList.empty())
	{
		g_Log.Log(LogType::_ERROR, L"No Launcher\n");
		return ERROR_SM_NETLAUNCHER_NOTFOUND;
	}

	m_Sync.Lock();

	bool bRet = false;
	std::map <UINT, CServiceSession*>::iterator ii;
	for (ii = m_ServiceSessionList.begin(); ii != m_ServiceSessionList.end(); ii++)
	{
		if ((*ii).second->GetMID() == nMID)
		{
			(*ii).second->SendMakeDump();
			bRet = true;
			break;
		}
	}

	m_Sync.UnLock();

	return bRet == true ? ERROR_NONE : ERROR_SM_MAKEDUMP_FAIL;
}

void CServiceManager::WorldMaxuserControl(int nMasterMID, UINT nChangeMaxuser)
{
	ScopeLock <CSyncLock> sync(m_Sync);

	std::map <UINT, CServiceSession*>::iterator ii, ih;
	for (ih = m_ServiceSessionList.begin(); ih != m_ServiceSessionList.end(); ih++)
	{
		//�����͸� ���� ã��
		if ((*ih).second->GetMID() == nMasterMID && (*ih).second->GetType() == MANAGED_TYPE_MASTER)
		{
			(*ih).second->SendChangeWorldMaxuser(nChangeMaxuser);
			return;
		}
	}
}

int CServiceManager::PopulationControl(int nMasterMID, int nControlMID, bool bZeroPopulation)
{
	ScopeLock <CSyncLock> sync(m_Sync);

	std::map <UINT, CServiceSession*>::iterator ii, ih;
	for (ih = m_ServiceSessionList.begin(); ih != m_ServiceSessionList.end(); ih++)
	{
		//�����͸� ���� ã��
		if ((*ih).second->GetMID() == nMasterMID && (*ih).second->GetType() == MANAGED_TYPE_MASTER)
		{
			//��Ʈ���� ������ ã�´�.
			for (ii = m_ServiceSessionList.begin(); ii != m_ServiceSessionList.end(); ii++)
			{
				//��Ʈ���� ������ ã�Ұ� 
				if ((*ii).second->GetMID() == nControlMID)
				{
					//�� ������ ���������
					if (MANAGED_TYPE_VILLAGE == (*ii).second->GetType() || MANAGED_TYPE_GAME == (*ii).second->GetType())
					{
						g_ExceptionReportLog.Log(LogType::_FILELOG, L"ConnectRate GameServer SID[%d] BlockFlag[%s]\n", (*ii).second->GetMID(), bZeroPopulation == true ? L"TRUE" : L"FALSE");
						(*ih).second->SendPopulationControl((*ii).second->GetType(), (*ii).second->GetMID(), bZeroPopulation);
						return ERROR_NONE;
					}
					else
						_DANGER_POINT();	//�̿ܿ� ó������ �ʴ´�.
					break;
				}
			}
			break;
		}
	}
		
	return ERROR_SM_GAMECONTROL_FAIL;
}

int CServiceManager::ChannelControl(int nMasterMID, int nChannelID, bool bVisibility, int nChannelPopulation)
{
	bool bFlag = false;
	m_Sync.Lock();

	std::map <UINT, CServiceSession*>::iterator ii;
	for (ii = m_ServiceSessionList.begin(); ii != m_ServiceSessionList.end(); ii++)
	{
		if ((*ii).second->GetMID() == nMasterMID && (*ii).second->GetType() == MANAGED_TYPE_MASTER)
		{
			(*ii).second->SendChannelControl(nChannelID, bVisibility, nChannelPopulation);
			bFlag = true;
		}
	}

	m_Sync.UnLock();

	return bFlag == true ? ERROR_NONE : ERROR_SM_CHANNELCONTROL_FAIL;
}

void CServiceManager::GameAffinityControl(int nMasterMID, int nGameMID, int nType)
{
	if (nType != _GAMESERVER_AFFINITYTYPE_NORMAL && nType != _GAMESERVER_AFFINITYTYPE_HYBRYD && nType != _GAMESERVER_AFFINITYTYPE_PVP)
		return;

	m_Sync.Lock();

	std::map <UINT, CServiceSession*>::iterator ii;
	for (ii = m_ServiceSessionList.begin(); ii != m_ServiceSessionList.end(); ii++)
		if ((*ii).second->GetMID() == nMasterMID && (*ii).second->GetType() == MANAGED_TYPE_MASTER)
			(*ii).second->SendGameAffinity(nGameMID, nType);

	m_Sync.UnLock();
}

void CServiceManager::EventUpdate()
{
	m_Sync.Lock();

	std::map <UINT, CServiceSession*>::iterator ii;
	for (ii = m_ServiceSessionList.begin(); ii != m_ServiceSessionList.end(); ii++)
		(*ii).second->SendUpdateEvent();		//������ ���Ӽ����� ���������������� �̺�Ʈó���� ������ ���߿� ��� ���� �𸣹Ƿ� �ϴ��� �� ����

	m_Sync.UnLock();
}

void CServiceManager::SimpleConfigUpdate()
{
	m_Sync.Lock();

	std::map <UINT, CServiceSession*>::iterator ii;
	for (ii = m_ServiceSessionList.begin(); ii != m_ServiceSessionList.end(); ii++)
		(*ii).second->SendUpdateSimpleConfig();

	m_Sync.UnLock();
}

void CServiceManager::ReserveNoticeUpdate(const char * pURL, char * pRet)
{
	if (pURL == NULL)
		return;

	if (strstr (pURL, "http"))
	{
		char szCurPath[MAX_PATH];
		GetCurrentDirectoryA(sizeof(szCurPath), szCurPath);

		std::wstring wstrUrl;
		ToWideString(const_cast<CHAR*>(pURL), wstrUrl);

		std::wstring wstrDest;
		ToWideString(szCurPath, wstrDest);
		wstrDest.append(L"\\");
		wstrDest.append(g_Config.wszNoticePath);

		if (_waccess(wstrDest.c_str(), 0) != -1)
		{
			if (DeleteFileW(wstrDest.c_str()) == 0)
				return;
		}

		// DownLoad
		if (m_UrlUpdater.UrlUpdateUrl(wstrUrl.c_str(), wstrDest.c_str()))
		{
			char szNotice[FILENAME_MAX];
			WideCharToMultiByte(CP_ACP, 0, g_Config.wszNoticePath, -1, szNotice, 256, NULL, NULL);
			if (_access(szNotice, 0) != -1)
			{
				if (m_ReservedNotice.LoadNotice(szNotice))
				{
					g_IniFile.SetValue(L"ReserveNoticeInfo", L"FileName", g_Config.wszNoticePath);
					return;
				}
			}
		}
	}
	else
	{
		WCHAR szTemp[FILENAME_MAX];
		MultiByteToWideChar(CP_ACP, 0, pURL, -1, szTemp, FILENAME_MAX);

		g_Log.Log(LogType::_FILELOG, L"Update Reserved Notice [URL:%s]\n", szTemp);

		if (m_UrlUpdater.UrlUpdateEach(szTemp, szTemp, true))
		{
			if (m_ReservedNotice.LoadNotice(pURL))
			{
				g_IniFile.SetValue(L"ReserveNoticeInfo", L"FileName", szTemp);
				return;
			}
		}

		g_Log.Log(LogType::_FILELOG, L"ReserveNotice Download Fail [URL:%s]\n", szTemp);
		if (pRet)
			strcpy_s(pRet, 256, "ReserveNotice Load Fail CheckURL\n");
	}
}

void CServiceManager::UserRestraint(UINT nAccountDBID, char * pRet)
{
	m_Sync.Lock();

	std::map <UINT, CServiceSession*>::iterator ii;
	for (ii = m_ServiceSessionList.begin(); ii != m_ServiceSessionList.end(); ii++)
		if ((*ii).second->GetType() == MANAGED_TYPE_MASTER)
			(*ii).second->SendUserRestraint(nAccountDBID);

	m_Sync.UnLock();
}

void CServiceManager::UserBanbyCharacterName(const char * pName)
{
	WCHAR wszName[NAMELENMAX];
	memset(wszName, 0, sizeof(wszName));
	MultiByteToWideChar(CP_ACP, 0, pName, -1, wszName, NAMELENMAX);

	m_Sync.Lock();

	std::map <UINT, CServiceSession*>::iterator ii;
	for (ii = m_ServiceSessionList.begin(); ii != m_ServiceSessionList.end(); ii++)
		if ((*ii).second->GetType() == MANAGED_TYPE_MASTER)
			(*ii).second->SendUserBan(wszName, 0, _BANTYPE_CHARACTERNAME);

	m_Sync.UnLock();
}

void CServiceManager::UserBanbyCharacterName(const WCHAR * pwszName)
{
	m_Sync.Lock();

	std::map <UINT, CServiceSession*>::iterator ii;
	for (ii = m_ServiceSessionList.begin(); ii != m_ServiceSessionList.end(); ii++)
		if ((*ii).second->GetType() == MANAGED_TYPE_MASTER)
			(*ii).second->SendUserBan(pwszName, 0, _BANTYPE_CHARACTERNAME);

	m_Sync.UnLock();
}

void CServiceManager::UserBanbyAccountDBID(UINT nAccountDBID)
{
	m_Sync.Lock();

	std::map <UINT, CServiceSession*>::iterator ii;
	for (ii = m_ServiceSessionList.begin(); ii != m_ServiceSessionList.end(); ii++)
		if ((*ii).second->GetType() == MANAGED_TYPE_MASTER)
			(*ii).second->SendUserBan(NULL, nAccountDBID, _BANTYPE_ACCOUNTID);

	m_Sync.UnLock();
}

void CServiceManager::UserBanbyAccountName(const WCHAR * pwszName)
{
	m_Sync.Lock();

	std::map <UINT, CServiceSession*>::iterator ii;
	for (ii = m_ServiceSessionList.begin(); ii != m_ServiceSessionList.end(); ii++)
		if ((*ii).second->GetType() == MANAGED_TYPE_MASTER)
			(*ii).second->SendUserBan(pwszName, 0, _BANTYPE_ACCOUNTNAME);

	m_Sync.UnLock();
}

//use http
void CServiceManager::PatchUpdateUrl(const char * pUrl, char * pMsg)
{
	if (m_PatchWatcher.HasWatchingContent())
	{
		if (pMsg) strcpy(pMsg, "PatchList Update Fail");
		return;
	}

	WCHAR wszPath[256];
	memset(wszPath, 0, sizeof(wszPath));
	MultiByteToWideChar(CP_ACP, 0, pUrl, -1, wszPath, 256);

	wcscpy_s(g_Config.wszPatchURL, wszPath);
	g_IniFile.SetValue(L"PatchInfo", L"PatchUrl", wszPath);
	
	m_UrlUpdater.UpdaterInit(g_Config.wszPatchBaseURL, g_Config.wszPatchURL);
	
	ScopeLock <CSyncLock> sync(m_Sync);
	std::map <UINT, CLauncherSession*>::iterator ii;
	for (ii = m_LauncherSessionList.begin(); ii != m_LauncherSessionList.end(); ii++)
	{
		std::vector <TServerExcuteData> vExeList;
		CDataManager::GetInstance()->GetExeCommandList((*ii).second->GetSessionID(), &vExeList);
		bool bOther = CDataManager::GetInstance()->IsNeedOtherExcutePath((*ii).second->GetSessionID());
		(*ii).second->SendManagingExeItem(&vExeList, bOther);

		std::list <char> lList;
		CDataManager::GetInstance()->GetAssingedPatchTypeList((*ii).second->GetSessionID(), &lList);
		(*ii).second->SendConnectedResult(&lList, g_Config.wszPatchBaseURL, g_Config.wszPatchURL);
	}

	std::map <UINT, CPatcherSession*>::iterator ih;
	for (ih = m_PatcherSessionList.begin(); ih != m_PatcherSessionList.end(); ih++)
		(*ih).second->SendConnectedResult(g_Config.wszPatchBaseURL, g_Config.wszPatchURL);

	if (pMsg) strcpy(pMsg, "PatchList Updated");
}

void CServiceManager::PatchStruct(char * pMsg)
{
	if (m_UrlUpdater.UrlUpdate(L"ServerStruct", L"System", false))
	{
		if (ReLoadStruct())
		{
			if (pMsg) strcpy(pMsg, "ServerStruct Patched");
			SendUpdateNotice(SERVERMONITOR_UPDATENOTICE_SERVERINFO);
			return ;
		}
		else
		{
			g_Log.Log(LogType::_FILELOG, _T("ServerStruct Patched Failed\n"));
			if (pMsg) strcpy(pMsg, "ServerStruct Patched Failed");
		}
	}

	g_Log.Log(LogType::_FILELOG, _T("Struct Patch Failed [URLDown]\n"));
	if (pMsg) strcpy(pMsg, "Struct Patch Failed [URLDown]");
}

void CServiceManager::PatchInfo(int nNID, char * pMsg)
{
	if (m_LauncherSessionList.empty())
	{
		g_Log.Log(LogType::_ERROR, L"No Launcher\n");
		return;
	}

	if (m_PatchWatcher.HasWatchingContent())
	{
		g_Log.Log(LogType::_FILELOG, L"PatchInfo Fail try later");
		if (pMsg) strcpy(pMsg, "PatchInfo Fail try later");
		return;
	}

	SendPatch(nNID, _T("Info"), m_nPatchID++);
	
	g_Log.Log(LogType::_FILELOG, _T("Send NetLauncher Info Patch\n"));
	if (pMsg) strcpy(pMsg, "Send NetLauncher Info Patch");
}

void CServiceManager::PatchSystem(int nNID, char * pMsg)
{
	if (m_LauncherSessionList.empty())
	{
		g_Log.Log(LogType::_ERROR, L"No Launcher\n");
		return;
	}

	if (m_PatchWatcher.HasWatchingContent())
	{
		g_Log.Log(LogType::_FILELOG, L"PatchSystem Fail try later");
		if (pMsg) strcpy(pMsg, "PatchSystem Fail try later");
		return;
	}

	SendPatch(nNID, _T("System"), m_nPatchID++);

	g_Log.Log(LogType::_FILELOG, _T("Send NetLauncher System Patch\n"));
	if (pMsg) strcpy(pMsg, "Send NetLauncher System Patch");
}

void CServiceManager::PatchConfig(int nNID, char * pMsg)
{
	if (m_LauncherSessionList.empty())
	{
		g_Log.Log(LogType::_ERROR, L"No Launcher\n");
		return;
	}

	if (m_PatchWatcher.HasWatchingContent())
	{
		g_Log.Log(LogType::_FILELOG, L"PatchConfig Fail try later");
		if (pMsg) strcpy(pMsg, "PatchConfig Fail try later");
		return;
	}

	SendPatch(nNID, _T("Config"), m_nPatchID++);

	g_Log.Log(LogType::_FILELOG, _T("Send NetLauncher Config Patch\n"));
	if (pMsg) strcpy(pMsg, "Send NetLauncher Config Patch");
}

void CServiceManager::PatchExe(int nNID, char * pMsg)
{
	if (m_LauncherSessionList.empty())
	{
		g_Log.Log(LogType::_ERROR, L"No Launcher\n");
		return;
	}

	if (m_PatchWatcher.HasWatchingContent())
	{
		g_Log.Log(LogType::_FILELOG, L"PatchExe Fail try later");
		if (pMsg) strcpy(pMsg, "PatchExe Fail try later");
		return;
	}

	if (m_LauncherSessionList.empty())
		g_Log.Log(LogType::_ERROR, L"No Launcher\n");
	else
		SendPatch(nNID, _T("Exe"), m_nPatchID++);
}

void CServiceManager::PatchFull(int nNID, char * pMsg)
{
	if (m_PatchWatcher.HasWatchingContent())
	{
		g_Log.Log(LogType::_ERROR, L"PatchFull Fail try later");
		if (pMsg) strcpy(pMsg, "PatchFull Fail try later");
		return;
	}

	if (m_LauncherSessionList.empty())
		g_Log.Log(LogType::_ERROR, L"No Launcher\n");
	else
	{
#if defined(_SERVICEMANAGER_EX)
		//���� �ȵ� ���İ� ������ ��ġ�ϰڳİ� ���
		if(!IsStartAllLauncherAndContinue())
		{
			g_Log.Log(LogType::_ERROR, L"PatchFull Fail try later");
			SetPatchComplete(false);
			m_pApp->OnPatchEnd(false);
			return ;
		}
#endif	// #if defined(_SERVICEMANAGER_EX)

		SendPatchStart(nNID);
		SendPatch(nNID, L"Config", m_nPatchID);
		SendPatch(nNID, L"Info", m_nPatchID);
		SendPatch(nNID, L"System", m_nPatchID);
		SendPatch(nNID, L"Exe", m_nPatchID);
		SendPatch(nNID, L"Res", m_nPatchID);
#if defined(_CH)
		SendPatch(nNID, L"Bat", m_nPatchID);
#endif	// #if defined(_CH)
		SendPatchEnd(nNID);
		SendInfoCopy(nNID);
		m_nPatchID++;
#if defined(_SERVICEMANAGER_EX)
		SetPatchComplete(true);
#endif
	}
}

void CServiceManager::PatchApply(char * pMsg)
{
	if (m_LauncherSessionList.empty())
	{
		g_Log.Log(LogType::_ERROR, L"No Launcher\n");
		if (pMsg) strcpy(pMsg, "No Launcher");
		return;
	}
	
	TerminateServiceAll();

	ScopeLock <CSyncLock> sync(m_Sync);
	std::map <UINT, CLauncherSession*>::iterator ii;
	for (ii = m_LauncherSessionList.begin(); ii != m_LauncherSessionList.end(); ii++)
		(*ii).second->SendPatchApply();
	
	if (pMsg) strcpy(pMsg, "Patch Apply OK");
}

void CServiceManager::InfoCopy(int nNID, char * pMsg)
{
	SendInfoCopy(nNID);
	if (pMsg) strcpy(pMsg, "Send Request NetLauncher InfoCopy");
}

void CServiceManager::Patch(const char * pUrl, char * pMsg)
{
	ScopeLock <CSyncLock> sync(m_Sync);
	
	std::map <UINT, CLauncherSession*>::iterator ih;
	for (ih = m_LauncherSessionList.begin(); ih != m_LauncherSessionList.end(); ih++)
	{
		if (m_PatchWatcher.AddWatchContent(m_nPatchID, (*ih).second->GetSessionID(), L"Url"))
		{
			(*ih).second->SendPatchByUrl(m_nPatchID, pUrl, L"Url", false, NULL);
		}
		else
			_DANGER_POINT();
	}
	m_nPatchID++;

	if (pMsg) strcpy(pMsg, "Send Request Patch By Url");
}

void CServiceManager::PatchBatch(int nNID, char * pMsg)
{
	if (m_LauncherSessionList.empty())
	{
		g_Log.Log(LogType::_ERROR, L"No Launcher\n");
		return;
	}

	if (m_PatchWatcher.HasWatchingContent())
	{
		g_Log.Log(LogType::_FILELOG, L"PatchBatch Fail try later");
		if (pMsg) strcpy(pMsg, "PatchBatch Fail try later");
		return;
	}

	SendPatch(nNID, L"Bat", m_nPatchID++);

	if (pMsg) strcpy(pMsg, "Batch Patch OK");
}

void CServiceManager::RunBatch(int nNID, char * pMsg)
{
	if (m_LauncherSessionList.empty())
	{
		g_Log.Log(LogType::_ERROR, L"No Launcher\n");
		return;
	}

	if (m_RunBatchWatcher.HasWatchingSpec())
	{
		if (pMsg) strcpy(pMsg, "Batch Run Fail");
		return;
	}


	ScopeLock <CSyncLock> sync(m_Sync);

	std::map <UINT, CLauncherSession*>::iterator ii;
	for (ii = m_LauncherSessionList.begin(); ii != m_LauncherSessionList.end(); ii++)
	{
		if (nNID > 0 && (*ii).second->GetSessionID() != nNID)
			continue;

		if (m_RunBatchWatcher.AddWatchSpec(m_nRunBatchID, (*ii).second->GetSessionID(), L"RunBat"))
		{
			(*ii).second->SendBatchRun(m_nRunBatchID);
			(*ii).second->SetBatch(false);
		}
		else
			_DANGER_POINT();
	}
	m_nRunBatchID++;

	if (pMsg) strcpy(pMsg, "Batch Run OK");
}

void CServiceManager::StopBatch(int nNID, char * pMsg)
{
	if (m_LauncherSessionList.empty())
	{
		g_Log.Log(LogType::_ERROR, L"No Launcher\n");
		return;
	}

	ScopeLock <CSyncLock> sync(m_Sync);

	std::map <UINT, CLauncherSession*>::iterator ii;
	for (ii = m_LauncherSessionList.begin(); ii != m_LauncherSessionList.end(); ii++)
	{
		if (nNID > 0 && (*ii).second->GetSessionID() != nNID)
			continue;

		if (m_StopBatchWatcher.AddWatchSpec(m_nStopBatchID, (*ii).second->GetSessionID(), L"StopBat"))
		{
			(*ii).second->SendBatchStop(m_nStopBatchID);
			(*ii).second->SetBatch(true);
		}
	}

	++m_nStopBatchID;

	m_RunBatchWatcher.ClearWatchingSpec();

	if (pMsg) strcpy(pMsg, "Batch Stop OK");
}

//for chn
void CServiceManager::PatchDyncode(char * pMsg)
{
	if (m_LauncherSessionList.empty())
	{
		g_Log.Log(LogType::_ERROR, L"No Launcher\n");
		return;
	}

	if (m_PatchWatcher.HasWatchingContent())
	{
		g_Log.Log(LogType::_FILELOG, L"PatchDynCodeBin Fail try later");
		if (pMsg) strcpy(pMsg, "PatchDynCodeBin Fail try later");
		return;
	}

	if (m_LauncherSessionList.empty())
		g_Log.Log(LogType::_ERROR, L"No Launcher\n");
	else
		SendPatch(0, _T("DynCodeBin"), m_nPatchID++);
}

void CServiceManager::ReloadDyncode(char * pMsg)
{
	if (IsService() == false)
	{
		g_Log.Log(LogType::_ERROR, L"No Service\n");
		if (pMsg) strcpy(pMsg, "No Service");
		return;
	}

	ScopeLock <CSyncLock> sync(m_Sync);

	std::map <UINT, CServiceSession*>::iterator ii;
	for (ii = m_ServiceSessionList.begin(); ii != m_ServiceSessionList.end(); ii++)
	{
		if ((*ii).second->GetType() == MANAGED_TYPE_GAME || (*ii).second->GetType() == MANAGED_TYPE_VILLAGE || (*ii).second->GetType() == MANAGED_TYPE_LOGIN)
		{
			(*ii).second->SendReloadDynCode();
		}
	}
}

//for extpatch
void CServiceManager::PatchExt()
{
	//live ext��ġ�� �ٸ� ��ġ�÷ο�� �ٸ��� ó���Ǿ��� (����)
	//���� GSM�� ������ ��� �ִ� ������ ��û�Ѵ�.

	if (m_LivePatcher.IsStarted())
	{
		g_Log.Log(LogType::_ERROR, L"Already Live Ext Patching PatchFailed\n");
		return;
	}

	std::wstring wstrTemp;
	wstrTemp = L"msg=RequestFileList,";
	wstrTemp += g_Config.wszPatchURL;
	wstrTemp += L"/ext";

	if (SendPacketToGsm(wstrTemp.c_str()))
	{
		if (m_LivePatcher.StartPatch() == false)
			g_Log.Log(LogType::_ERROR, L"Already Live Ext Patching PatchFailed\n");

		g_pServiceManager->SendDelExt();
		g_Log.Log(LogType::_FILELOG, L"Live Ext Patch Start\n");
		return;
	}

	g_Log.Log(LogType::_ERROR, L"ext patch err\n");
}

void CServiceManager::CancelExtPatch()
{
	m_LivePatcher.CancelPatch();
}

void CServiceManager::RecvFileList(const char * pUrl)
{
	//�ش� Url���� ������ �ٿ� �޴´�!
	if (pUrl == NULL)
		return;

	char szCurPath[MAX_PATH];
	GetCurrentDirectoryA(sizeof(szCurPath), szCurPath);

	std::wstring wstrUrl;
	ToWideString(const_cast<CHAR*>(pUrl), wstrUrl);

	std::wstring wstrDest;
	ToWideString(szCurPath, wstrDest);
	wstrDest.append(L"\\FileList.csv");

	if (_waccess(wstrDest.c_str(), 0) != -1)
	{
		if (DeleteFileW(wstrDest.c_str()) == 0)
			return;
	}

	if (m_UrlUpdater.UrlUpdateUrl(wstrUrl.c_str(), wstrDest.c_str()))
	{
		TCHAR * pFile = NULL;
		pFile = ReadFile(L"FileList.csv");
		if (pFile)
		{
			if (m_LivePatcher.LoadPatchList(pFile) == false)
			{
				g_Log.Log(LogType::_ERROR, L"LoadPatchList Fail\n");
				m_LivePatcher.CancelPatch();
				SAFE_DELETEARRAY(pFile);
				return;
			}
			SAFE_DELETEARRAY(pFile);
			m_LivePatcher.SendLivePatch();
		}
		else
		{
			m_LivePatcher.CancelPatch();
			g_Log.Log(LogType::_ERROR, L"PatchFileList LoadFail Path[%s]\n", wstrDest.c_str());
			return;
		}
	}
	else
		m_LivePatcher.CancelPatch();
}

bool CServiceManager::CompleteLivePatchItem(int nPatchID)
{
	if (m_LivePatcher.IsStarted())
	{
		if (m_LivePatcher.RecvCompleteItem(nPatchID))
		{
			//���� ���ϵǸ� ������ �����۵��� ���� ��ġ�� �Ǿ����Ŵ� �� �����鿡�� �ε��϶� �˸���
			g_Log.Log(LogType::_NORMAL, L"LivePatch Ext DownLoadComplete\n");

			if (IsService() == false)
			{
				g_Log.Log(LogType::_ERROR, L"LivePatch Ext Live Patch Reload Failed No Service\n");
				m_LivePatcher.CancelPatch();
				return false;
			}

			g_Log.Log(LogType::_NORMAL, L"LivePatch Ext Live Patch Starting...\n");
			g_pServiceManager->SendCopyExt();
			return true;
		}
	}
	return false;
}

void CServiceManager::CompleteLiveExtCopy(bool bRet)
{
	if (m_LivePatcher.IsStarted())
	{
		if (bRet)
		{
			//���� ��缭���� ext���� �߰��ε��϶� �˸���~
			std::vector <int> vList;
			GetMangedIDList(vList);

			m_LivePatcher.SetReloadFlag(vList);
			if (SendExtReload(0) == false)
			{
				m_LivePatcher.CancelPatch();
				g_Log.Log(LogType::_ERROR, L"LivePatch Ext Live Patch Failed! No Service\n");
			}
		}
		else
		{
			//���� �ֿܼ� �޼����� ǥ��������
			m_LivePatcher.CancelPatch();
			g_Log.Log(LogType::_ERROR, L"LivePatch Ext Live Patch Failed! Check ext files\n");
		}
	}
}

void CServiceManager::CompleteLiveExtReload(int nMID, bool bRet)
{
	if (bRet == false)
		g_Log.Log(LogType::_ERROR, L"LivePatch Ext Live Reload Failed!!!!! CheckServer!!!!\n");

	bool bReloadFlag = false;
	if (m_LivePatcher.SetReloadResult(nMID, bRet, bReloadFlag))
	{
		g_Log.Log(LogType::_NORMAL, L"LivePatch Ext Live Reload Ret[%s]\n", (bReloadFlag) ? L"Success" : L"Failed");
	}
}

void CServiceManager::SendDelExt()
{
	ScopeLock <CSyncLock> sync(m_Sync);

	std::map <UINT, CLauncherSession*>::iterator ii;
	for (ii = m_LauncherSessionList.begin(); ii != m_LauncherSessionList.end(); ii++)
		(*ii).second->SendLiveExtDel();
}


void CServiceManager::SendCopyExt()
{
	ScopeLock <CSyncLock> sync(m_Sync);

	std::map <UINT, CLauncherSession*>::iterator ii;
	for (ii = m_LauncherSessionList.begin(); ii != m_LauncherSessionList.end(); ii++)
		(*ii).second->SendLiveExtCopy();
}

bool CServiceManager::SendExtReload(int nReLoadType)
{
	ScopeLock <CSyncLock> sync(m_Sync);

	if (m_ServiceSessionList.empty())
		return false;

	std::map <UINT, CServiceSession*>::iterator ii;
	for (ii = m_ServiceSessionList.begin(); ii != m_ServiceSessionList.end(); ii++)
	{
		(*ii).second->SendExtReload(nReLoadType);
	}
	return true;
}


//NetLauncher Service For GSM
void CServiceManager::StartLauncher(char * pMsg)
{
	if (m_PatcherSessionList.empty())
	{
		g_Log.Log(LogType::_FILELOG, L"No PatcherService\n");
		if (pMsg) strcpy(pMsg, "No PatcherService");
		return;
	}

	ScopeLock <CSyncLock> sync(m_Sync);

	//Managed PatcherServiceOnly
	std::map <UINT, CPatcherSession*>::iterator ii;
	for (ii = m_PatcherSessionList.begin(); ii != m_PatcherSessionList.end(); ii++)
		(*ii).second->SendStartNetLauncher();

	if (pMsg) strcpy(pMsg, "Send Request Start NetLauncher");
}

void CServiceManager::StopLauncher(char * pMsg)
{
	if (IsService())
	{
		g_Log.Log(LogType::_FILELOG, L"DragonNest Service Online Plz Try Later\n");
		if (pMsg) strcpy(pMsg, "DragonNest Service Online Plz Try Later");
		return;
	}

	if (m_PatcherSessionList.empty())
	{
		g_Log.Log(LogType::_FILELOG, L"No PatcherService\n");
		if (pMsg) strcpy(pMsg, "No PatcherService");
		return;
	}

	ScopeLock <CSyncLock> sync(m_Sync);

	std::map <UINT, CPatcherSession*>::iterator ii;
	for (ii = m_PatcherSessionList.begin(); ii != m_PatcherSessionList.end(); ii++)
		(*ii).second->SendStopNetLauncher();

	if (pMsg) strcpy(pMsg, "Send Request Stop NetLauncher");
}

void CServiceManager::StartEachLauncher(int nPSID, char * pMsg)
{
	if (m_PatcherSessionList.empty())
	{
		g_Log.Log(LogType::_FILELOG, L"No PatcherService\n");
		if (pMsg) strcpy(pMsg, "No PatcherService");
		return;
	}

	ScopeLock <CSyncLock> sync(m_Sync);

	std::map <UINT, CPatcherSession*>::iterator ii = m_PatcherSessionList.find(nPSID);
	if (ii != m_PatcherSessionList.end())
		(*ii).second->SendStartNetLauncher();

	if (pMsg) strcpy(pMsg, "Send Request Start NetLauncher");
}

void CServiceManager::StopEachLauncher(int nPSID, char * pMsg)
{
	if (m_PatcherSessionList.empty())
	{
		g_Log.Log(LogType::_FILELOG, L"No PatcherService\n");
		if (pMsg) strcpy(pMsg, "No PatcherService");
		return;
	}

	ScopeLock <CSyncLock> sync(m_Sync);

	std::map <UINT, CPatcherSession*>::iterator ii = m_PatcherSessionList.find(nPSID);
	if (ii != m_PatcherSessionList.end())
		(*ii).second->SendStopNetLauncher();

	if (pMsg) strcpy(pMsg, "Send Request Stop NetLauncher");
}

void CServiceManager::StopEachLauncherAsync(int nPSID)
{
	if (m_PatcherSessionList.empty())
	{
		std::map <UINT, CLauncherSession*>::iterator itor = m_LauncherSessionList.find(nPSID);
		if (itor != m_LauncherSessionList.end())
			(*itor).second->DetachConnection(L"NetLauncher ProblemDetach");
		return;
	}

	std::map <UINT, CPatcherSession*>::iterator ii = m_PatcherSessionList.find(nPSID);
	if (ii != m_PatcherSessionList.end())
		(*ii).second->SendStopNetLauncher();
}

//NetLauncher Patch
void CServiceManager::PatchNetLauncher(char * pMsg)
{
	if (m_PatcherSessionList.empty())
	{
		g_Log.Log(LogType::_FILELOG, L"No PatcherService\n");
		if (pMsg) strcpy(pMsg, "No PatcherService");
		return;
	}

	std::map <UINT, CPatcherSession*>::iterator ii;
	std::vector <TNetLauncherInfo>::iterator itor;

	//check
	std::vector <TNetLauncherInfo> vList;
	CDataManager::GetInstance()->GetLauncherIds(&vList);
	int nCount = 0;
	for (itor = vList.begin(); itor != vList.end(); itor++)
	{
		ii = m_PatcherSessionList.find((*itor).nNID);
		if (ii != m_PatcherSessionList.end())
			nCount++;
		else
		{
			//������ ������
			g_Log.Log(LogType::_FILELOG, L"LauncherPatcher Not Exists IP[%s]\n", (*itor).wszIP);
		}
	}
	if (vList.size() != nCount)
	{
		g_Log.Log(LogType::_FILELOG, L"Launcher Patch Failed\n");
		return;
	}
	
	if (IsService())
	{
		g_Log.Log(LogType::_FILELOG, L"DragonNest Service Online PatchFailed\n");
		if (pMsg) strcpy(pMsg, "DragonNest Service Online PatchFailed");
		return;
	}

	ScopeLock <CSyncLock> sync(m_Sync);
	
	for (ii = m_PatcherSessionList.begin(); ii != m_PatcherSessionList.end(); ii++)
		(*ii).second->SendStopNetLauncher();

	Sleep(500);

	for (ii = m_PatcherSessionList.begin(); ii != m_PatcherSessionList.end(); ii++)
	{
		if ((*ii).second->SendNetLauncherPatch())
			m_PatchWatcher.AddWatchContent(-1, (*ii).second->GetSessionID(), L"Net");
	}

	if (pMsg) strcpy(pMsg, "Send Request Patch NetLauncher");
}

void CServiceManager::PatchNetLauncherEach(int nID, char * pMsg)
{
	if (m_PatcherSessionList.empty())
	{
		g_Log.Log(LogType::_FILELOG, L"No PatcherService\n");
		if (pMsg) strcpy(pMsg, "No PatcherService");
		return;
	}

	std::map <UINT, CPatcherSession*>::iterator ii;
	std::vector <TNetLauncherInfo>::iterator itor;

	//check
	bool bCheck = false;
	std::vector <TNetLauncherInfo> vList;
	CDataManager::GetInstance()->GetLauncherIds(&vList);

	ScopeLock <CSyncLock> sync(m_Sync);
	ii = m_PatcherSessionList.find(nID);
	if (ii != m_PatcherSessionList.end())
	{
		for (itor = vList.begin(); itor != vList.end(); itor++)
		{
			if ((*itor).nNID == nID)
			{
				bCheck = true;
				break;
			}
		}
	}

	if (bCheck == false)
	{
		g_Log.Log(LogType::_FILELOG, L"LauncherPatcher Not Exists IP[%s]\n", ii == m_PatcherSessionList.end() ? "none" : (*ii).second->GetIp());
		return;
	}	

	ii = m_PatcherSessionList.find(nID);
	if (ii != m_PatcherSessionList.end())
	{
		(*ii).second->SendStopNetLauncher();
		Sleep(500);
		(*ii).second->SendNetLauncherPatch();
	}

	if (pMsg) strcpy(pMsg, "Send Request Patch NetLauncher");
}

void CServiceManager::PatchedLauncher(int nNID)
{
	bool bComplete = false;
#if defined(_SERVICEMANAGER_EX)
	bool bNIDComplete = false;
	m_PatchWatcher.DelWatchContent(-1, nNID, L"Net", bComplete, bNIDComplete);
#else
	m_PatchWatcher.DelWatchContent(-1, nNID, L"Net", bComplete);
#endif
	if (bComplete)
	{
		g_Log.Log(LogType::_NORMAL, L"NetLauncherPatch Complete\n");
	}
}

void CServiceManager::CheckPatch()
{
	if (m_PatchWatcher.HasWatchingContent() == false)
	{
		g_Log.Log(LogType::_NORMAL, L"No Watching Patch Content\n");
	}
}

void CServiceManager::Clearpatch()
{
	m_PatchWatcher.ClearWatchingContent();
}

void CServiceManager::CheckNetLauncher(int nNID)
{
	ScopeLock <CSyncLock> sync(m_Sync);

	if (nNID == -1)
	{
		std::vector <TNetLauncherInfo> vTemp;
		CDataManager::GetInstance()->GetLauncherIds(&vTemp);

		for (int i = 0; i < (int)vTemp.size(); i++)
		{
			std::map <UINT, CLauncherSession*>::iterator ii = m_LauncherSessionList.find(vTemp[i].nNID);
			if (ii == m_LauncherSessionList.end())
				g_Log.Log(LogType::_NORMAL, L"NetLauncher was not connected[ID:%d][IP:%s]\n", vTemp[i].nNID, vTemp[i].wszIP);
		}
	}
	else
	{
		const TNetLauncher * pLauncher = CDataManager::GetInstance()->GetLauncherInfobyID(nNID);
		if (pLauncher)
		{
			g_Log.Log(LogType::_NORMAL, L"NetLauncher [ID:%d][PID:%d][WID:%d][IP:%s]\n", pLauncher->nID, pLauncher->nPID, pLauncher->nWID, pLauncher->szIP);
		}

		std::map <UINT, CLauncherSession*>::iterator ii = m_LauncherSessionList.find(nNID);
		if (ii != m_LauncherSessionList.end())
		{
			(*ii).second->SendReturnPing(-1);
		}
	}
}

void CServiceManager::CheckAliveNetLauncher()
{
	ScopeLock <CSyncLock> sync(m_Sync);

	if (m_LauncherSessionList.size() <=0)
	{
		g_Log.Log(LogType::_FILELOG, L"All Launchers Closed\n");
		return;
	}

	std::map <UINT, CLauncherSession*>::iterator ii;
	for (ii = m_LauncherSessionList.begin(); ii != m_LauncherSessionList.end(); ii++)
	{
		g_Log.Log(LogType::_NORMAL, L"NetLauncher is alive [ID:%d][IP:%S]\n", (*ii).second->GetSessionID(), (*ii).second->GetIp());
	}
	
}

void CServiceManager::TestLauncher(int nNID)
{
	ScopeLock <CSyncLock> sync(m_Sync);
	ULONG nCurTick = timeGetTime();
	m_LaunchetTestList.clear();
	
	std::map <UINT, CLauncherSession*>::iterator ii;
	for (ii = m_LauncherSessionList.begin(); ii != m_LauncherSessionList.end(); ii++)
	{
		if ((*ii).second->SendReturnPing(0))
			m_LaunchetTestList.insert(std::make_pair((*ii).second->GetSessionID(), nCurTick));
	}
}

void CServiceManager::CheckBatch(int nNID)
{
	// BatchWatcher���ٴ� ���� ������ ��� ���� CheckNetLauncheró�� ó���Ѵ�.
	ScopeLock <CSyncLock> sync(m_Sync);

	if (nNID == -1)
	{
		std::vector <TNetLauncherInfo> vTemp;
		CDataManager::GetInstance()->GetLauncherIds(&vTemp);

		for (int i = 0; i < (int)vTemp.size(); i++)
		{
			std::map <UINT, CLauncherSession*>::iterator ii = m_LauncherSessionList.find(vTemp[i].nNID);
			if (ii == m_LauncherSessionList.end())
				g_Log.Log(LogType::_NORMAL, L"NetLauncher was not connected[ID:%d][IP:%s]\n", vTemp[i].nNID, vTemp[i].wszIP);
			else
			{
				if (false == (*ii).second->IsBatch())
				{
					g_Log.Log(LogType::_ERROR, L"Batch was not completed [ID:%d][IP:%s]\n", vTemp[i].nNID, vTemp[i].wszIP);
				}
			}
		}
	}
	else
	{
		const TNetLauncher * pLauncher = CDataManager::GetInstance()->GetLauncherInfobyID(nNID);
		if (pLauncher)
		{
			g_Log.Log(LogType::_NORMAL, L"NetLauncher [ID:%d][PID:%d][WID:%d][IP:%s]\n", pLauncher->nID, pLauncher->nPID, pLauncher->nWID, pLauncher->szIP);
		}
		else
		{
			g_Log.Log(LogType::_NORMAL, L"CheckBatch NID[%d] NotFound\n", nNID);
		}

		std::map <UINT, CLauncherSession*>::iterator ii = m_LauncherSessionList.find(nNID);
		if (ii != m_LauncherSessionList.end())
		{
			if (false == (*ii).second->IsBatch())
			{
				g_Log.Log(LogType::_ERROR, L"Batch was not completed [ID:%d][IP:%s]\n", nNID, (*ii).second->GetIp());
			}
		}
	}
}

bool CServiceManager::CreateServiceServer(int nSID)
{
	ScopeLock <CSyncLock> sync(m_Sync);

	std::map <int, sServiceState*>::iterator ii = m_ServiceList.find(nSID);
	if (ii != m_ServiceList.end())
	{
		if ((*ii).second->nState != _SERVICE_STATE_ONLINE || (*ii).second->nState != _SERVICE_STATE_CREATE)
		{
			(*ii).second->nState = _SERVICE_STATE_CREATE;
			g_Log.Log(LogType::_FILELOG, L"[LauncherReport [NLID:%d] [SID:%d]Service Created\n", (*ii).second->nManagedLauncherID, (*ii).second->nSID);
			int nState = (*ii).second->nState;

			(*ii).second->nCreateTime = timeGetTime();

			ReportToGSMServiceState(nSID, nState, NULL, NULL);
			ReportToMonitorServiceState(nSID, nState);
		}
		return true;
	}
	return false;
}

bool CServiceManager::OnlineServiceServer(int nSID, int nServiceSessionID, const char * pVersion, const char * pResVersion, CServiceSession * pSession, BYTE cWorldCount, BYTE * pWorldID, bool bFinalBuild)
{
	std::string strCmd;
	strCmd = pVersion;
	ToLowerA(strCmd);

	std::vector<std::string> tokens;
	TokenizeA(strCmd, tokens, ".");

	if (tokens.size() < 2)
		return false;

	int nMajorVersion = atoi(tokens[0].c_str());
	int nMinorVersion = atoi(tokens[1].c_str());

	if (nMajorVersion != m_nMajorVersion && nMinorVersion != m_nMinorVersion)
	{
		//TerminateServiceEachForce(nSID);
		g_Log.Log(LogType::_ERROR, L"Server Version MisMatch ServiceManager[%S] Server[%S] IP[%S]\n", g_Config.szVersion, pVersion, pSession->GetIp());
	}

	ScopeLock <CSyncLock> sync(m_Sync);

	std::map <int, sServiceState*>::iterator ii = m_ServiceList.find(nSID);
	if (ii != m_ServiceList.end())
	{
		if ((*ii).second->nState == _SERVICE_STATE_ONLINE)
		{
			if((*ii).second->bIsReConnect == false)
			{
				(*ii).second->bIsReConnect = true;
				g_Log.Log(LogType::_FILELOG, L"[LauncherReport [NLID:%d] [SID:%d] [Session:%d, ConnectedSession:%d] Reconnect Service\n", (*ii).second->nManagedLauncherID, (*ii).second->nSID, pSession->GetSessionID(), (*ii).second->nServiceSessionID);
			}
			else
			{
				g_Log.Log(LogType::_FILELOG, L"[LauncherReport [NLID:%d] [SID:%d] [Session:%d, ConnectedSession:%d] Already Service Online\n", (*ii).second->nManagedLauncherID, (*ii).second->nSID, pSession->GetSessionID(), (*ii).second->nServiceSessionID);
				return true;
			}
		}
		else
#if defined( _SERVICEMANAGER_EX )
			g_FileLog.Log(LogType::_FILELOG, L"[LauncherReport [NLID:%d] [SID:%d] [Version:%S] [ResVersion:%S] [Session:%d] Service Online\n", (*ii).second->nManagedLauncherID, (*ii).second->nSID, pVersion, pResVersion, pSession->GetSessionID());
#else
			g_Log.Log(LogType::_FILELOG, L"[LauncherReport [NLID:%d] [SID:%d] [Version:%S] [ResVersion:%S] [Session:%d] Service Online\n", (*ii).second->nManagedLauncherID, (*ii).second->nSID, pVersion, pResVersion, pSession->GetSessionID());
#endif
		
		(*ii).second->nState = _SERVICE_STATE_ONLINE;
		(*ii).second->nServiceSessionID = nServiceSessionID;

		int nState = (*ii).second->nState;
		//memcpy((*ii).second->cWorldIDs, pWorldID, cWorldCount);
		strncpy ((*ii).second->szVersion, pVersion, SERVERVERSIONMAX);
		(*ii).second->bFinalBuild = bFinalBuild;

		ReportToGSMServiceState((*ii).second->nSID, (*ii).second->nState, pVersion, pResVersion);
		ReportToMonitorServiceState((*ii).second->nSID, (*ii).second->nState);
		return true;
	}
	return false;
}

bool CServiceManager::CheckAllServiceOnline()
{
	for (std::map <int, sServiceState*>::iterator ii = m_ServiceList.begin(); ii != m_ServiceList.end(); ii++)
	{
		if( ii->second->nState != _SERVICE_STATE_ONLINE )
			return false;		
	}
	return true;
}

bool CServiceManager::ReportTerminateService(int nSID)
{
	ScopeLock <CSyncLock> sync(m_Sync);

	std::map <int, sServiceState*>::iterator ii = m_ServiceList.find(nSID);
	if (ii != m_ServiceList.end())
	{
		CLauncherSession * pLauncher = NULL;
		std::map <UINT, CLauncherSession*>::iterator ih = m_LauncherSessionList.find((*ii).second->nManagedLauncherID);
		if (ih != m_LauncherSessionList.end())
			pLauncher = (*ih).second;

		(*ii).second->nState = _SERVICE_STATE_TERMINATED;
		int nState = (*ii).second->nState;		
		g_Log.Log(LogType::_FILELOG, L"[LauncherReport [NLID:%d] [SID:%d] [IP:%S]Service Terminated\n", (*ii).second->nManagedLauncherID, (*ii).second->nSID, pLauncher ? pLauncher->GetIp() : "0");
		g_ExceptionReportLog.Log(LogType::_FILELOG, L"[LauncherReport [NLID:%d] [SID:%d] [IP:%S]Service Terminated\n", (*ii).second->nManagedLauncherID, (*ii).second->nSID, pLauncher ? pLauncher->GetIp() : "0");

		//Ȥ�� �𸣴� ���� ���� ����ġ�մϴ�
		std::map <UINT, CServiceSession*>::iterator is;
		for (is = m_ServiceSessionList.begin(); is != m_ServiceSessionList.end(); is++)
		{
			if ((*is).second->GetMID() == nSID)
			{
				(*is).second->DetachConnection(L"terminated");
				break;
			}
		}

		ReportToGSMServiceState((*ii).second->nSID, (*ii).second->nState, NULL, NULL);
		ReportToMonitorServiceState((*ii).second->nSID, (*ii).second->nState);
		return true;
	}
	return false;
}

void CServiceManager::EjectForce()
{
	ScopeLock <CSyncLock> sync(m_Sync);

	std::map <int, sServiceState*>::iterator ii;
	for (ii = m_ServiceList.begin(); ii != m_ServiceList.end(); ii++)
	{
		(*ii).second->nState = _SERVICE_STATE_OFFLINE;
		(*ii).second->bIsReConnect = false;

		ReportToGSMServiceState((*ii).second->nSID, (*ii).second->nState, NULL, NULL);
		ReportToMonitorServiceState((*ii).second->nSID, (*ii).second->nState);
	}
}

void CServiceManager::EjectServiceAll()
{
	ScopeLock <CSyncLock> sync(m_Sync);

	std::map <int, sServiceState*>::iterator ii;
	for (ii = m_ServiceList.begin(); ii != m_ServiceList.end(); ii++)
	{
		if (wcscmp((*ii).second->szType, L"DB") && wcscmp((*ii).second->szType, L"Log"))
		{
			(*ii).second->nState = _SERVICE_STATE_OFFLINE;
			(*ii).second->bIsReConnect = false;

			ReportToGSMServiceState((*ii).second->nSID, (*ii).second->nState, NULL, NULL);
			ReportToMonitorServiceState((*ii).second->nSID, (*ii).second->nState);
		}
	}
}

void CServiceManager::EjectServiceDB()
{
	m_Sync.Lock();

	std::map <int, sServiceState*>::iterator ii;
	for (ii = m_ServiceList.begin(); ii != m_ServiceList.end(); ii++)
	{
		if (!wcscmp((*ii).second->szType, L"DB") || !wcscmp((*ii).second->szType, L"Log"))
		{
			(*ii).second->nState = _SERVICE_STATE_OFFLINE;
			(*ii).second->bIsReConnect = false;

			ReportToGSMServiceState((*ii).second->nSID, (*ii).second->nState, NULL, NULL);
			ReportToMonitorServiceState((*ii).second->nSID, (*ii).second->nState);
		}
	}

	m_Sync.UnLock();
}

void CServiceManager::EjectService(int nSID)
{
	ScopeLock <CSyncLock> sync(m_Sync);

	std::map <int, sServiceState*>::iterator ii;
	for (ii = m_ServiceList.begin(); ii != m_ServiceList.end(); ii++)
	{
		if ((*ii).second->nSID == nSID)
		{
			(*ii).second->nState = _SERVICE_STATE_OFFLINE;
			(*ii).second->bIsReConnect = false;

			ReportToGSMServiceState((*ii).second->nSID, (*ii).second->nState, NULL, NULL);
			ReportToMonitorServiceState((*ii).second->nSID, (*ii).second->nState);

			std::vector <_CLOSESERVICE>::iterator ih;
			for (ih = m_CloseServieList.begin(); ih != m_CloseServieList.end(); ih++)
			{
				if ((*ih).nMID == nSID)
				{
					m_CloseServieList.erase(ih);
					break;
				}
			}
			return;
		}
	}
}

void CServiceManager::UpdateWorldMaxUser(TServiceWorldInfo * pPacket)
{
	{
		ScopeLock <CSyncLock> sync(m_Sync);

		std::map <int, sMonitorWorldInfo>::iterator ii = m_WorldInfo.find(pPacket->nWorldID);
		if (ii != m_WorldInfo.end())
		{
			(*ii).second.nWorldMaxUser = pPacket->nWorldMaxuser;
		}
		else
		{
			sMonitorWorldInfo World;
			memset(&World, 0, sizeof(World));

			World.nWorldID = pPacket->nWorldID;
			World.nWorldMaxUser = pPacket->nWorldMaxuser;
			m_WorldInfo.insert(std::make_pair(pPacket->nWorldID, World));
		}

#if defined (_SERVICEMANAGER_EX)
		OnWorldMaxUser(pPacket->nWorldID, pPacket->nWorldMaxuser);
#endif

		m_nContainerVersion++;
	}

	SendUpdateNotice(SERVERMONITOR_UPDATENOTICE_WOLRDINFO);
}

void CServiceManager::UpdateMeritInfo(TServiceMeritInfo * pPacket)
{
	{
		ScopeLock <CSyncLock> sync(m_Sync);
		m_ChannelMerit.clear();

		std::map <int, TMeritInfo>::iterator ii;
		for (int i = 0; i < pPacket->cCount; i++)
		{
			ii = m_ChannelMerit.find(pPacket->Info[i].nID);
			if (ii == m_ChannelMerit.end())
				m_ChannelMerit.insert(std::make_pair(pPacket->Info[i].nID, pPacket->Info[i]));
		}

		m_nContainerVersion++;
	}

	SendUpdateNotice(SERVERMONITOR_UPDATENOTICE_MERITINFO);
}

void CServiceManager::UpdateChannelInfo(TServiceChannelInfo * pPacket)
{
	bool NeedNotice = false;
	{
		ScopeLock <CSyncLock> sync(m_Sync);

		std::map <INT64, tMonitorChannelInfo>::iterator ii;

		bool bNeedInclease = false;
		tMonitorChannelInfo Info;
		INT64 nKey = 0;
		for (int i = 0; i < pPacket->cCount; i++)
		{
			nKey = MAKE_SM_SVRCHN(pPacket->nManagedID, pPacket->Info[i].nChannelID);
			ii = m_ChannelList.find(nKey);
			if (ii != m_ChannelList.end())
				memcpy(&(*ii).second.Info, &pPacket->Info[i], sizeof(sChannelInfo));
			else
			{
				memset(&Info, 0, sizeof(Info));

				Info.nManagedID = pPacket->nManagedID;
				Info.nWorldID = pPacket->nWorldID;

				m_ChannelList.insert(std::make_pair(nKey, Info));
				bNeedInclease = true;
				NeedNotice = true;
			}
		}

		if (bNeedInclease)
			m_nContainerVersion++;
	}

	if (NeedNotice)
		SendUpdateNotice(SERVERMONITOR_UPDATENOTICE_CHANNELINFO);
}

int CServiceManager::GetMasterSIDByChannelID(int nWorldID, int nChannelID)
{
	ScopeLock <CSyncLock> sync(m_Sync);

	std::map <int, sServiceState*>::iterator ih;
	for (ih = m_ServiceList.begin(); ih != m_ServiceList.end(); ih++)
	{
		if (GetServerType((*ih).second->szType) == MANAGED_TYPE_MASTER)
		{
			for (int i = 0; i < WORLDCOUNTMAX; i++)
			{
				if ((*ih).second->cWorldIDs[i] <= 0) continue;
				if ((*ih).second->cWorldIDs[i] == nWorldID)
					return (*ih).second->nSID;
			}
		}
	}
	return -1;
}

bool CServiceManager::GetMasterSIDByGameSID(int nGameSID, int * pnMasterIDs)
{
	ScopeLock <CSyncLock> sync(m_Sync);

	BYTE cWorldIDs[WORLDCOUNTMAX];
	std::map <int, sServiceState*>::iterator ii = m_ServiceList.find(nGameSID);
	if (ii != m_ServiceList.end())
		memcpy(cWorldIDs, (*ii).second->cWorldIDs, sizeof(cWorldIDs));
	else
		return false;

	int nCount = 0;
	std::map <int, sServiceState*>::iterator ih;
	for (ih = m_ServiceList.begin(); ih != m_ServiceList.end(); ih++)
	{
		if (GetServerType((*ih).second->szType) == MANAGED_TYPE_MASTER)
		{
			for (int i = 0; i < WORLDCOUNTMAX; i++)
			{
				if (cWorldIDs[i] <= 0) continue;
				for (int h = 0; h < WORLDCOUNTMAX; h++)
				{
					if ((*ih).second->cWorldIDs[h] <= 0) continue;
					if (cWorldIDs[i] == (*ih).second->cWorldIDs[h])
						pnMasterIDs[nCount++] = (*ih).first;
				}
			}
		}
	}

	return true;
}

int CServiceManager::GetDBWorldIDBySID (int nSID)
{
	ScopeLock <CSyncLock> sync(m_Sync);

	std::map <int, sServiceState*>::iterator ih;
	for (ih = m_ServiceList.begin(); ih != m_ServiceList.end(); ih++)
	{
		if (GetServerType((*ih).second->szType) == MANAGED_TYPE_DB)
		{
			for (int i = 0; i < WORLDCOUNTMAX; i++)
			{
				if ((*ih).second->cWorldIDs[i] <= 0) continue;
				if ((*ih).second->nSID == nSID)
					return (*ih).second->cWorldIDs[i];
			}
		}

	}

	return -1;
}

void CServiceManager::LauncherReturn(int nNID, int nIdx, ULONG nReturnPing)
{
	if (nIdx == -1)
	{
		g_Log.Log(LogType::_NORMAL, L"NetLauncher ReturnPing [NID:%d][Gap:%d]\n", nNID, timeGetTime() - nReturnPing);
		return;
	}

	ScopeLock <CSyncLock> sync(m_Sync);
	std::map <int, ULONG>::iterator ii = m_LaunchetTestList.find(nNID);
	if (ii != m_LaunchetTestList.end())
	{
		m_LaunchetTestList.erase(ii);
	}
}

void CServiceManager::CheckTestLauncher()
{
	ScopeLock <CSyncLock> sync(m_Sync);
	ULONG nCurTick = timeGetTime();
	std::map <int, ULONG>::iterator testItor;
	if (m_LaunchetTestList.size() > 0)
	{
		for (testItor = m_LaunchetTestList.begin(); testItor != m_LaunchetTestList.end(); )
		{
			if (nCurTick > (*testItor).second + (30 * 1000))
			{
				g_Log.Log(LogType::_ERROR, L"NetLauncher ping return delayed [NID:%d]\n", (*testItor).first);
				testItor = m_LaunchetTestList.erase(testItor);
			}
			else
				testItor++;
		}
	}
	else
		g_Log.Log(LogType::_NORMAL, L"All Launcher Test OK\n");
}

void CServiceManager::SelectJoin(int nMasterSID, int nSID, const WCHAR * pwszCharacterName)
{
	ScopeLock <CSyncLock> sync(m_Sync);

	std::map <UINT, CServiceSession*>::iterator ii;
	for (ii = m_ServiceSessionList.begin(); ii != m_ServiceSessionList.end(); ii++)
	{
		if ((*ii).second->GetMID() == nMasterSID)
		{
			if ((*ii).second->GetType() == MANAGED_TYPE_MASTER)
				(*ii).second->SendSelectJoin(nSID, pwszCharacterName);
			else
				g_Log.Log(LogType::_ERROR, L"[SID:%d] Is not masterserver\n", nMasterSID);
			break;
		}
	}
}

void CServiceManager::ClearSelectJoin(int nMasterSID)
{
	ScopeLock <CSyncLock> sync(m_Sync);

	std::map <UINT, CServiceSession*>::iterator ii;
	for (ii = m_ServiceSessionList.begin(); ii != m_ServiceSessionList.end(); ii++)
	{
		if ((*ii).second->GetMID() == nMasterSID)
		{
			if ((*ii).second->GetType() == MANAGED_TYPE_MASTER)
				(*ii).second->SendClearSelectjoin();
			else
				g_Log.Log(LogType::_ERROR, L"[SID:%d] Is not masterserver\n", nMasterSID);
			break;
		}
	}
}

void CServiceManager::SendServiceClose(__time64_t _tOderedTime, __time64_t _tCloseTime)
{
	ScopeLock <CSyncLock> sync(m_Sync);

	std::map <UINT, CServiceSession*>::iterator ii;
	for (ii = m_ServiceSessionList.begin(); ii != m_ServiceSessionList.end(); ii++)
	{
		if ((*ii).second->GetType() == MANAGED_TYPE_GAME || (*ii).second->GetType() == MANAGED_TYPE_VILLAGE || (*ii).second->GetType() == MANAGED_TYPE_LOGIN)
			(*ii).second->SendScheduleCloseService(_tOderedTime, _tCloseTime);
	}
}

void CServiceManager::UpdateSaleData()
{
	ScopeLock <CSyncLock> sync(m_Sync);

	std::map <UINT, CServiceSession*>::iterator ii;
	for (ii = m_ServiceSessionList.begin(); ii != m_ServiceSessionList.end(); ii++)
	{
		if ((*ii).second->GetType() == MANAGED_TYPE_CASH || (*ii).second->GetType() == MANAGED_TYPE_DB || (*ii).second->GetType() == MANAGED_TYPE_VILLAGE)
			(*ii).second->SendUpdateSaleData();
	}
}

void CServiceManager::UpdateBanFile(const char * pUrl)
{
	if (pUrl == NULL)
		return;

	char szCurPath[MAX_PATH];
	GetCurrentDirectoryA(sizeof(szCurPath), szCurPath);

	std::wstring wstrUrl;
	ToWideString(const_cast<CHAR*>(pUrl), wstrUrl);

	std::wstring wstrDest;
	ToWideString(szCurPath, wstrDest);
	wstrDest.append(L"/BanFile.csv");

	if (_waccess(wstrDest.c_str(), 0) != -1)
	{
		if (DeleteFileW(wstrDest.c_str()) == 0)
			return;
	}
	
	if (m_UrlUpdater.UrlUpdateUrl(wstrUrl.c_str(), wstrDest.c_str()))
	{
		eBanFileType eBanType = _BANTYPE_MAX;
		std::vector <std_str> vBanList;

		TCHAR * pFile = NULL;
		pFile = ReadFile(L"BanFile.csv");
		if (pFile)			//���ϸ� �����ؾ���
		{
			std::vector<std::wstring> tokens;
			TokenizeW(pFile, tokens, L",");

			for (int i = 0; i < (int)tokens.size(); i++)
			{
				std_str strTempName = tokens[i];

				if (i == 0)
				{
					if (strTempName == _SZACCOUNTNAMETYPE)
						eBanType = _BANTYPE_ACCOUNTNAME;
					else if (strTempName == _SZACCOUNTIDTYPE)
						eBanType = _BANTYPE_ACCOUNTID;
					else if (strTempName == _SZCHARACTERNAMETYPE)
						eBanType = _BANTYPE_CHARACTERNAME;
					else
					{
						g_Log.Log(LogType::_FILELOG, L"UserBanFile Seperater Err [:%S]\n", strTempName);
						SAFE_DELETEARRAY(pFile);
						return;
					}
				}
				else
				{
					if (strTempName.size() > 0)
						vBanList.push_back(strTempName);
				}
			}

			SAFE_DELETEARRAY(pFile);
		}

		std::vector <std_str>::iterator ii;
		for (ii = vBanList.begin(); ii != vBanList.end(); ii++)
		{
			switch (eBanType)
			{
			case _BANTYPE_ACCOUNTNAME:
				UserBanbyAccountName((*ii).c_str());
				break;
			case _BANTYPE_CHARACTERNAME:
				UserBanbyCharacterName((*ii).c_str());
				break;
			case _BANTYPE_ACCOUNTID:
				UserBanbyAccountDBID(_wtoi((*ii).c_str()));
				break;
			}
		}
	}
}

void CServiceManager::UpdateFarm(int nWorldID)
{
	ScopeLock <CSyncLock> sync(m_Sync);

	std::map <UINT, CServiceSession*>::iterator ii;
	for (ii = m_ServiceSessionList.begin(); ii != m_ServiceSessionList.end(); ii++)
	{
		(*ii).second->SendUpdateFarm(nWorldID);
	}
}

void CServiceManager::StartFarm()
{
	ScopeLock <CSyncLock> sync(m_Sync);

	std::map <UINT, CServiceSession*>::iterator ii;
	for (ii = m_ServiceSessionList.begin(); ii != m_ServiceSessionList.end(); ii++)
	{
		(*ii).second->SendUpdateFarm(0, true);
	}
}

void CServiceManager::ReqUpdateFarm()
{
	SendPacketToGsm(_T("msg=Farm,1"));
	CGsmCmdProcess::GetInstance()->AddCmdProcess("msg=Farm,1", "farmupdated", m_CmdTable.lpFarmUpdate);
}

void CServiceManager::UpdateFarm()
{
	g_pServiceManager->UpdateFarm(0);
}

void CServiceManager::UpdateFarmStatus (TServiceReportMaster* pPacket)
{	
	if (pPacket->nWorldID >= WORLDCOUNTMAX)
		return;

	ScopeLock <CSyncLock> sync(m_Sync);

	std::map <int, std::vector<TFarmStatus>>& pFarmStatus = m_FarmStatus[pPacket->nWorldID];

	pFarmStatus.clear();

	for( int i=0; i<pPacket->cFarmCount; i++ )
	{
		std::map <int, std::vector<TFarmStatus>>::iterator managed_itor = pFarmStatus.find(pPacket->FarmStatus[i].nManagedID);
		if( managed_itor == pFarmStatus.end() )
		{
			std::vector<TFarmStatus> vList;
			vList.push_back(pPacket->FarmStatus[i]);
			pFarmStatus.insert (std::make_pair(pPacket->FarmStatus[i].nManagedID, vList));
		}
		else
		{
			std::vector<TFarmStatus>::iterator farm_itor = managed_itor->second.begin();

			bool bAddStatus = true;
			for (; farm_itor != managed_itor->second.end(); farm_itor++)
			{
				TFarmStatus& pStatus = *farm_itor;
				if (pStatus.nFarmDBID == pPacket->FarmStatus[i].nFarmDBID)
				{

					bAddStatus = false;
					if (pPacket->FarmStatus[i].bActivate)
						pStatus.nFarmCurUserCount = pPacket->FarmStatus[i].nFarmCurUserCount;
					else
						pStatus.nFarmCurUserCount = 0;

					break;
				}
			}

			if (bAddStatus)
				managed_itor->second.push_back(pPacket->FarmStatus[i]);
		}
	}
}

void CServiceManager::UpdateGuildWarSchedule()
{
	ScopeLock <CSyncLock> sync(m_Sync);

	std::map <UINT, CServiceSession*>::iterator ii;
	for (ii = m_ServiceSessionList.begin(); ii != m_ServiceSessionList.end(); ii++)
	{
		if ((*ii).second->GetType() == MANAGED_TYPE_MASTER)
		{
			(*ii).second->SendUpdateGuildWarSchedule();
		}	
	}
}

void CServiceManager::SendUpdateGuildWare(int nGuildID)
{
	ScopeLock <CSyncLock> sync(m_Sync);

	std::map <UINT, CServiceSession*>::iterator ii;
	for (ii = m_ServiceSessionList.begin(); ii != m_ServiceSessionList.end(); ii++)
	{
		if ((*ii).second->GetType() == MANAGED_TYPE_MASTER)
		{
			(*ii).second->SendUpdateGuildWare(nGuildID);
		}	
	}
}

bool CServiceManager::IsService()
{
	bool bFlag = false;
	m_Sync.Lock();
	std::map <int, sServiceState*>::iterator ii;
	for (ii = m_ServiceList.begin(); ii != m_ServiceList.end(); ii++)
	if ((*ii).second->nState == _SERVICE_STATE_CREATE || (*ii).second->nState == _SERVICE_STATE_ONLINE)
	{
		bFlag = true;
		break;
	}
	m_Sync.UnLock();
	return bFlag;
}

bool CServiceManager::IsService(int nSID)
{
	bool bFlag = false;
	m_Sync.Lock();
	std::map <int, sServiceState*>::iterator ii = m_ServiceList.find(nSID);
	if (ii != m_ServiceList.end())
	{
		if ((*ii).second->nState == _SERVICE_STATE_CREATE || (*ii).second->nState == _SERVICE_STATE_ONLINE)
			bFlag = true;
	}
	m_Sync.UnLock();
	return bFlag;
}

bool CServiceManager::ReportServiceInfoLogin(int nSID, int nLoginUser)
{
	ScopeLock <CSyncLock> Lock(m_Sync);

	std::map <int, sServiceState*>::iterator ii = m_ServiceList.find(nSID);
	if (ii != m_ServiceList.end())
	{
		SendPacketToGsm(_T("msg=SInfo,%d,%d"), nSID, nLoginUser);
		return true;
	}
	return false;
}

bool CServiceManager::ReportServiceInfoVillage(int nSID, UINT nUserCount, int nCnt, TVillageUserReport * pReport)
{
	ScopeLock <CSyncLock> Lock(m_Sync);

	std::map <int, sServiceState*>::iterator ii = m_ServiceList.find(nSID);
	if (ii != m_ServiceList.end())
	{
		INT64 nKey = 0;
		for (int i = 0; i < nCnt; i++)
		{
			nKey = MAKE_SM_SVRCHN(nSID, pReport[i].nChannelID);
			std::map <INT64, tMonitorChannelInfo>::iterator itor = m_ChannelList.find(nKey);
			if (itor != m_ChannelList.end())
			{
				(*itor).second.Info.nCurrentUserCount = pReport[i].nChannelUserCount;
			}
		}

		char szCmdBuf[4096];
		memset(szCmdBuf, 0, sizeof(szCmdBuf));
		sprintf(szCmdBuf, "msg=SInfo,%d,%d,%d,", nSID, nUserCount, nCnt);

		int nLen = (int)strlen(szCmdBuf);
		memcpy_s(szCmdBuf + nLen, sizeof(szCmdBuf) - (nLen + 2), pReport, sizeof(TVillageUserReport) * nCnt);
		SendPacketToGsm(szCmdBuf, nLen + sizeof(TVillageUserReport) * nCnt);
		return true;
	}
	return false;
}

bool CServiceManager::ReportServiceMaster(int nSID, int nWorldID, UINT nWaitUserCount)
{
	ScopeLock <CSyncLock> Lock(m_Sync);

		SendPacketToGsm(L"msg=WInfo,%d,%d", nWorldID, nWaitUserCount);
		return true;
}

bool CServiceManager::ReportServiceInfoGame(int nSID, UINT nUserCount, UINT nRoomCount)
{
	ScopeLock <CSyncLock> Lock(m_Sync);
	std::map <int, sServiceState*>::iterator ii = m_ServiceList.find(nSID);
	if (ii != m_ServiceList.end())
	{
		SendPacketToGsm(L"msg=SInfo,%d,%d,%d", nSID, nUserCount, nRoomCount);
		return true;
	}
	return false;
}

void CServiceManager::ReportedServiceClosed(int nSID)
{
	g_Log.Log(LogType::_FILELOG, L"ReportedService Closed MID[%d]\n", nSID);
	
	ScopeLock <CSyncLock> sync(m_Sync);
	std::vector <_CLOSESERVICE>::iterator in;
	for (in = m_CloseServieList.begin(); in != m_CloseServieList.end(); in++)
	{
		if ((*in).nMID == nSID)
			(*in).nTerminateTick = timeGetTime();
	}
}

void CServiceManager::LauncherConnected(CLauncherSession * pSession)
{
	ScopeLock <CSyncLock> sync(m_Sync);

	//1��
	std::vector <TServerExcuteData> vExeList;
	CDataManager::GetInstance()->GetExeCommandList(pSession->GetSessionID(), &vExeList);
	bool bOther = CDataManager::GetInstance()->IsNeedOtherExcutePath(pSession->GetSessionID());
	pSession->SendManagingExeItem(&vExeList, bOther);

	//2�� ���� �ٲٸ� �ȵ�~
	std::list <char> lList;
	CDataManager::GetInstance()->GetAssingedPatchTypeList(pSession->GetSessionID(), &lList);
	pSession->SendConnectedResult(&lList, g_Config.wszPatchBaseURL, g_Config.wszPatchURL);
}

bool CServiceManager::VerifyPatcher(const char * pIP)
{
	ScopeLock <CSyncLock> sync(m_Sync);
	std::vector <CPatcherSession*>::iterator ii;
	for (ii = m_vPatcherSessionList.begin(); ii != m_vPatcherSessionList.end(); ii++)
	{
		if (!strcmp((*ii)->GetIp(), pIP))
			return false;
	}
	return true;
}

bool CServiceManager::IsCloseService(int nSID)
{
	ScopeLock <CSyncLock> sync(m_Sync);

	if (nSID == 0)
	{
		bool bCheck = m_CloseServieList.empty() ? false : true;
		return bCheck;
	}
	else
	{
		std::vector <_CLOSESERVICE>::iterator ii;
		for (ii = m_CloseServieList.begin(); ii != m_CloseServieList.end(); ii++)
		{
			if ((*ii).nMID == nSID)
				return true;
		}
	}
	return false;
}

bool CServiceManager::ExcuteProcess(const TServerExcuteData * pExe, bool bForcePatch)
{
	std::map <UINT, CLauncherSession*>::iterator ii;
	ii = m_LauncherSessionList.find(pExe->nAssignedLauncherID);
	if (ii != m_LauncherSessionList.end())
	{
		sServiceState * pState = GetServiceState(pExe->nSID);
		if (pState && pState->nState != _SERVICE_STATE_CREATE && pState->nState != _SERVICE_STATE_ONLINE)
		{
			(*ii).second->SendRunProcess(pExe->nSID, pExe->nCreateCount, pExe->nCreateIndex, pExe->wszType, pExe->wstrExcuteData.c_str(), bForcePatch);
			s_ExcuteLog.Log(LogType::_FILELOG, L"ExcuteParam[%s]\n", pExe->wstrExcuteData.c_str());
			pState->Init();
			return true;
		}
		else
			g_Log.Log(LogType::_FILELOG, L"Service Already Online SID[%d]\n", pExe->nSID);
	}
	
	g_Log.Log(LogType::_FILELOG, L"NetLauncher Offline ID[%d]\n", pExe->nAssignedLauncherID);
	return false;
}

bool CServiceManager::MakeServiceList(int nSID, int nNetLauncherID, const TCHAR * pType, bool bForce)
{
	if (GetServiceState(nSID) != NULL)
		return false;

	sServiceState * pState = new sServiceState;
	memset(pState, 0, sizeof(sServiceState));
	
	pState->nSID = nSID;
	pState->nManagedLauncherID = nNetLauncherID;
	pState->nState = bForce == false ? _SERVICE_STATE_OFFLINE : _SERVICE_STATE_ONLINE;
	_tcscpy_s(pState->szType, pType);
	pState->bIsReConnect = false;
	m_ServiceList[pState->nSID] = pState;
	return true;
}

bool CServiceManager::MakeWorldList()
{
	std::map <int, sServiceState*>::iterator ii;
	for (ii = m_ServiceList.begin(); ii != m_ServiceList.end(); ii++)
	{
		if (GetServerType((*ii).second->szType) == MANAGED_TYPE_MASTER)
		{
			const TServerExcuteData * pExe = CDataManager::GetInstance()->GetCommand((*ii).second->nSID);
			if (pExe)
			{
				int nWorldID = 0;
				GetFirstRightIntValue(L"mwi", pExe->wstrExcuteData.c_str(), nWorldID);
				(*ii).second->cWorldIDs[0] = nWorldID;
			}
		}
	}

	for (ii = m_ServiceList.begin(); ii != m_ServiceList.end(); ii++)
	{
		int nCount = 0;
		const TServerExcuteData * pExe = CDataManager::GetInstance()->GetCommand((*ii).second->nSID);
		if (pExe == NULL) continue;

		switch (GetServerType((*ii).second->szType))
		{
		//case MANAGED_TYPE_LOGIN:
		/*case MANAGED_TYPE_DB:
			{
				std::vector <std::wstring>::const_iterator ih;
				for (ih = pExe->strMasterCon.begin(); ih != pExe->strMasterCon.end(); ih++)
				{
					std::vector<std::wstring> tokens;
					TokenizeW((*ih), tokens, L" ");

					if (_wtoi(tokens[0].c_str()) > 0)
						(*ii).second->cWorldIDs[nCount++] = _wtoi(tokens[0].c_str());
					else if (_wtoi(tokens[0].c_str()) <= 0)
					{
						int nLast = _wtoi(tokens[1].c_str());
						for (int i = 0; i < nLast; i++)
							(*ii).second->cWorldIDs[nCount++] = i+1;
					}
				}
			}
			break;*/
		case MANAGED_TYPE_DB:
			{
				std::wstring wstrTempWorld;
				GetValueCount(L"wdb", pExe->wstrExcuteData, nCount);
				for (int h = 0; h < nCount; h++)
				{
					if (GetRightValueByIndex(L"wdb", pExe->wstrExcuteData, h, wstrTempWorld))
						(*ii).second->cWorldIDs[h] = _wtoi(wstrTempWorld.c_str());
					else
						_ASSERT_EXPR(0, L"[MakeWorldList] Can't find wdb");
				}
			}
			break;

		case MANAGED_TYPE_VILLAGE:
			{
				int nVMID = 0;
				if (GetFirstRightIntValue(L"vwid", pExe->wstrExcuteData.c_str(), nVMID) == false)
					_ASSERT_EXPR(0, L"[MakeWorldList] Can't find vwid");
				(*ii).second->cWorldIDs[0] = nVMID;
			}
			break;

		case MANAGED_TYPE_GAME:
			{
				std::wstring wstrTempWorld;
				GetValueCount(L"gwid", pExe->wstrExcuteData, nCount);
				for (int h = 0; h < nCount; h++)
				{
					if (GetRightValueByIndex(L"gwid", pExe->wstrExcuteData, h, wstrTempWorld))
						(*ii).second->cWorldIDs[h] = _wtoi(wstrTempWorld.c_str());
					else
						_ASSERT_EXPR(0, L"[MakeWorldList] Can't find gwid");
				}
			}
			break;
			
		case MANAGED_TYPE_CASH:
			{
				int nCashID = 0;
				if (GetFirstRightIntValue(L"cwid", pExe->wstrExcuteData.c_str(), nCashID) == false)
					_ASSERT_EXPR(0, L"[MakeWorldList] Can't find cwid");
				(*ii).second->cWorldIDs[0] = nCashID;
			}
			break;
		}
	}
	return true;
}

sServiceState * CServiceManager::GetServiceState(int nSID)
{
	std::map <int, sServiceState*>::iterator ii = m_ServiceList.find(nSID);
	if (ii != m_ServiceList.end())
		return (*ii).second;
	return NULL;
}

sServiceState * CServiceManager::GetServiceStateByServiceSession(int nServiceSessionID)
{
	std::map <int, sServiceState*>::iterator ii;
	for (ii = m_ServiceList.begin(); ii != m_ServiceList.end(); ii++)	
		if ((*ii).second->nServiceSessionID == nServiceSessionID)
			return (*ii).second;
	return NULL;
}

int CServiceManager::GetNeedResType(int nNetLauncherID)
{
	int nRet = 0;			//0 �ƹ��͵� ���ʿ��� 1 VillageRes 2 GameRes
	std::map <int, sServiceState*>::iterator ii;
	for (ii = m_ServiceList.begin(); ii != m_ServiceList.end(); ii++)
	{
		if ((*ii).second->nManagedLauncherID == nNetLauncherID)
		{
			if (_tcsstr((*ii).second->szType, _T("Game")))
				nRet = 2;
			else	if (_tcsstr((*ii).second->szType, _T("Login")) || _tcsstr((*ii).second->szType, _T("Master")) || _tcsstr((*ii).second->szType, _T("Village")) || _tcsstr((*ii).second->szType, _T("DB")))
				nRet = nRet >= 2 ? 2 : 1;
		}
	}
	return nRet;
}

void CServiceManager::DeleteManagedService(int nNetLauncherID)
{
	std::map <int, sServiceState*>::iterator ii;
	for (ii = m_ServiceList.begin(); ii != m_ServiceList.end(); ii++)
		if ((*ii).second->nManagedLauncherID == nNetLauncherID)
			(*ii).second->Init();
}

void CServiceManager::ClearList()
{
	std::map <int, sServiceState*>::iterator ii;
	for (ii = m_ServiceList.begin(); ii != m_ServiceList.end(); ii++)
		SAFE_DELETE((*ii).second);
	m_ServiceList.clear();
}

void CServiceManager::SendPatchStart(int nNID)
{
	ScopeLock <CSyncLock> sync(m_Sync);

	std::map <UINT, CLauncherSession*>::iterator ii;
	for (ii = m_LauncherSessionList.begin(); ii != m_LauncherSessionList.end(); ii++)
	{
		if (nNID > 0 && (*ii).second->GetSessionID() != nNID)
			continue;

		(*ii).second->SendPatchStart();
	}
}

void CServiceManager::SendPatch(int nNID, const WCHAR * pKey, int nPatchID)
{
	ScopeLock <CSyncLock> sync(m_Sync);

	std::map <UINT, CLauncherSession*>::iterator ii;
	for (ii = m_LauncherSessionList.begin(); ii != m_LauncherSessionList.end(); ii++)
	{
		if (nNID > 0 && (*ii).second->GetSessionID() != nNID)
			continue;

		if ((*ii).second->SendPatch(pKey, GetNeedResType((*ii).second->GetSessionID()), nPatchID))
		{
			if (m_PatchWatcher.AddWatchContent(nPatchID, (*ii).second->GetSessionID(), pKey) == false)
				_DANGER_POINT();
		}
		else
		{
			g_Log.Log(LogType::_ERROR, L"SendPatch Fail Check netlauncher [NID:%d][IP:%S]\n", (*ii).second->GetSessionID(), (*ii).second->GetIp());
		}
	}
}

int CServiceManager::SendPatchEachItem(int nNID, const WCHAR * pURL, const WCHAR * pDest)
{
	ScopeLock <CSyncLock> sync(m_Sync);

	char szTempURL[512];

	std::map <UINT, CLauncherSession*>::iterator ii;
	for (ii = m_LauncherSessionList.begin(); ii != m_LauncherSessionList.end(); ii++)
	{
		if (nNID > 0 && (*ii).second->GetSessionID() != nNID)
			continue;

		WideCharToMultiByte(CP_ACP, 0, pURL, -1, szTempURL, 512, NULL, NULL);
		if (m_PatchWatcher.AddWatchContent(m_nPatchID, (*ii).second->GetSessionID(), L"Live"))
		{
			if ((*ii).second->SendPatchByUrl(m_nPatchID, szTempURL, L"Live", true, pDest) == false)
				g_Log.Log(LogType::_ERROR, L"LivePatchError [NID:%d] [IP:%s] [Url:%s]\n", (*ii).second->GetSessionID(), (*ii).second->GetwszIp(), pURL);
		}
	}
	int nPatchTemp = m_nPatchID;
	m_nPatchID++;
	return nPatchTemp;
}

void CServiceManager::SendPatchEnd(int nNID)
{
	ScopeLock <CSyncLock> sync(m_Sync);

	std::map <UINT, CLauncherSession*>::iterator ii;
	for (ii = m_LauncherSessionList.begin(); ii != m_LauncherSessionList.end(); ii++)
	{
		if (nNID > 0 && (*ii).second->GetSessionID() != nNID)
			continue;

		(*ii).second->SendPatchEnd();
	}
}

void CServiceManager::SendInfoCopy(int nNID)
{
	ScopeLock <CSyncLock> sync(m_Sync);

	std::map <UINT, CLauncherSession*>::iterator ii;
	for (ii = m_LauncherSessionList.begin(); ii != m_LauncherSessionList.end(); ii++)
	{
		if (nNID > 0 && (*ii).second->GetSessionID() != nNID)
			continue;

		(*ii).second->SendInfoCopy(CDataManager::GetInstance()->IsNeedOtherExcutePath((*ii).first));
	}
}

void CServiceManager::SendCreatEachDB(int nWorldID)
{
	m_Sync.Lock();

	std::map <UINT, CServiceSession*>::iterator ii;
	for (ii = m_ServiceSessionList.begin(); ii != m_ServiceSessionList.end(); ii++)
		if ((*ii).second->GetType() == MANAGED_TYPE_LOGIN)
			(*ii).second->SendCreatEachDB(nWorldID);

	m_Sync.UnLock();
}

void CServiceManager::SendReloadGuildwar(int nWorldID)
{
	m_Sync.Lock();

	std::map <UINT, CServiceSession*>::iterator ii;
	for (ii = m_ServiceSessionList.begin(); ii != m_ServiceSessionList.end(); ii++)
		if ((*ii).second->GetType() == MANAGED_TYPE_MASTER)
			(*ii).second->SendReloadSchedule(nWorldID);

	m_Sync.UnLock();
}


void CServiceManager::SendChangeLimiteItemMax(int nSN, int nLimitMax)
{
	ScopeLock <CSyncLock> sync(m_Sync);
	std::map <UINT, CServiceSession*>::iterator ii;
	for (ii = m_ServiceSessionList.begin(); ii != m_ServiceSessionList.end(); ii++)
		if ((*ii).second->GetType() == MANAGED_TYPE_CASH)
			(*ii).second->SendChangeLimitedItemMax(nSN, nLimitMax);
}


bool CServiceManager::AmountTokenizedString(char * pExtStr, int nExtMaxSize, std::vector<std::string> &vStr, int nStartCount, int nEndCount)
{
	int nLen = 0;
	for (int i = nStartCount; i < (int)vStr.size() && i < nEndCount; i++)
	{
		int nTempLen = (int)strlen(vStr[i].c_str());
		if (nTempLen + nLen >= nExtMaxSize)
			return false;
		_strcpy(pExtStr+nLen, vStr[i].c_str(), nTempLen);
		nLen += nTempLen;
		pExtStr[nLen++] = ' ';
	}
	if(nLen < nExtMaxSize)
		pExtStr[nLen] = '\0';
	else
		return false;
	return true;
}

int CServiceManager::GetServiceCount(int nServiceType, std::map <UINT, int> * pmList)
{
	//Need Sync
	int nCount = 0;

	std::map <UINT, CServiceSession*>::iterator ii;
	for (ii = m_ServiceSessionList.begin(); ii != m_ServiceSessionList.end(); ii++)
	{
		if ((*ii).second->GetType() == nServiceType)
		{
			pmList->insert(std::make_pair((*ii).second->GetMID(), (*ii).second->GetType()));
			nCount++;
		}
	}

	return nCount;
}

int CServiceManager::GetVillageServiceCount()
{
	//Need Sync
	int nCount = 0;

	std::map <UINT, CServiceSession*>::iterator ii;
	for (ii = m_ServiceSessionList.begin(); ii != m_ServiceSessionList.end(); ii++)
	{
		if ((*ii).second->GetType() == MANAGED_TYPE_VILLAGE)
			nCount++;
	}

	return nCount;
}

int CServiceManager::GetGameServiceCount()
{
	//Need Sync
	int nCount = 0;

	std::map <UINT, CServiceSession*>::iterator ii;
	for (ii = m_ServiceSessionList.begin(); ii != m_ServiceSessionList.end(); ii++)
	{
		if ((*ii).second->GetType() == MANAGED_TYPE_GAME)
			nCount++;
	}

	return nCount;
}

bool CServiceManager::GetChannelInfo(int nChannelID, short & nMeritID, int & nLimitLevel, bool & bVisibility, bool & bShow)
{
	//Need Sync
	std::map <INT64, tMonitorChannelInfo>::iterator ii;
	for (ii = m_ChannelList.begin(); ii != m_ChannelList.end(); ii++)
	{
		if ((*ii).second.Info.nChannelID == nChannelID)
		{
			nMeritID = (*ii).second.Info.nMeritBonusID;
			nLimitLevel = (*ii).second.Info.nLimitLevel;
			bVisibility = (*ii).second.Info.bVisibility;
			bShow = (*ii).second.Info.bShow;

			return true;
		}
	}
	return false;
}

void CServiceManager::ReportToGSMServiceState(int nSID, int nServiceState, const char * pVersion, const char * pRevision)
{
	switch (nServiceState)
	{
	case _SERVICE_STATE_OFFLINE:
		{
			SendPacketToGsm(_T("msg=PInfo,%d,Terminated"), nSID);
			break;
		}

	case _SERVICE_STATE_CREATE:
		{
			SendPacketToGsm(_T("msg=PInfo,%d,Create"), nSID);
			break;
		}

	case _SERVICE_STATE_ONLINE:
		{
			SendPacketToGsm(_T("msg=PInfo,%d,Online,%S,%S"), nSID, pVersion, pRevision);
			break;
		}

	case _SERVICE_STATE_TERMINATED:
		{
			SendPacketToGsm(_T("msg=PInfo,%d,Terminated"), nSID);
			break;
		}
	}
}

void CServiceManager::ReportToMonitorServiceState(int nSID, int nServiceState, int nServiceException, const char * pDetail)
{
	//NeedSync
	std::vector <CMonitorSession*>::iterator ii;
	for (ii = m_MonitorSessionList.begin(); ii != m_MonitorSessionList.end(); ii++)
		(*ii)->SendUpdateServerState(nSID, nServiceState, nServiceException, pDetail);
}

void CServiceManager::AddServerDelayMID(int nType, int nMID)
{
	if (m_vServerDelayMIDs[nType].size() > SERVICEMANAGER_SERVERDEALY_SIZE)
		return;

	m_vServerDelayMIDs[nType].push_back(nMID);
};

void CServiceManager::ReportToMonitorServerDelay(int nType)
{
	if (m_vServerDelayMIDs[nType].size() <= 0)
		return;

	//NeedSync
	std::vector <CMonitorSession*>::iterator ii;
	for (ii = m_MonitorSessionList.begin(); ii != m_MonitorSessionList.end(); ii++)
		(*ii)->SendServerDelayInfo(nType, m_vServerDelayMIDs[nType]);
	m_vServerDelayMIDs[nType].clear();
}

bool CServiceManager::SendPacketToGsm(const WCHAR * fmt, ...)
{
	WCHAR buf[4096];
	va_list arg;
	va_start(arg, fmt);
	_vsntprintf(buf, _countof(buf), fmt, arg);
	
	char szMsg[4096];
	WideCharToMultiByte(CP_ACP, 0, buf, -1, szMsg, 4096, NULL, NULL);

	bool bRet = SendPacketToGsm(szMsg, (int)strlen(szMsg));

	va_end(arg);
	return bRet;
}

bool CServiceManager::SendPacketToGsm(char * fmt, int nSize)
{
	if (m_bHttpSendFail)
	{
		if (m_nHttpFailTick + (60 * 1000) > timeGetTime())
			return false;
		else
		{
			m_bHttpSendFail = false;
			m_nHttpFailTick = 0;
		}
	}

	_SYNCGSMTASK task;
	memset(&task, 0, sizeof(_SYNCGSMTASK));

	task.nSize = (short)(nSize + sizeof(short));
	STRCPYA(task.szMsg, 4096, fmt);

	m_pStoredPacketBuffer->Lock();

	int nRet = m_pStoredPacketBuffer->Push(&task, task.nSize);
	if (nRet < 0)
		_DANGER_POINT();

	m_pStoredPacketBuffer->UnLock();
	g_pServiceServer->AddSyncCall();
	return true;
}

TCHAR * CServiceManager::ReadFile(const TCHAR * fname)
{
	char * ptr;
	int len;

	FILE *fp;
	fp = _tfopen(fname, _T("rb"));
	if (fp == NULL)
		return NULL;
	fseek(fp, 0, SEEK_END);
	len = ftell(fp);
	if( len < 0 )
	{
		fclose(fp);
		return NULL;
	}
	fseek(fp, 0, SEEK_SET);
	ptr = new char [len+2];
	fread(ptr, 1, len, fp);
	fclose(fp);

	ptr[len] = '\0';
	ptr[len+1] = '\0';

	static unsigned char _unicodeset[] = {
		0xff, 0xfe
	} ;
	if (!memcmp(ptr, _unicodeset, 2))
	{	//	�����ڵ�� ����Ǿ� ����
#ifdef UNICODE
		TCHAR * wptr = new TCHAR [(len-2) / sizeof(TCHAR) + 1];
		memcpy(wptr, &ptr[2], (len+2) - 2);
		delete [] ptr;
		return wptr;
#else
		char * bptr = new char [len*2 + 1];
		memset(bptr, 0, len*2 + 1);
		WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)&ptr[2], (len-2)/2, bptr, len*2 + 1, NULL, NULL);
		delete [] ptr;
		return bptr;
#endif
	}	else
	{
#ifdef UNICODE
		TCHAR * wptr = new TCHAR [len + 1];
		memset(wptr, 0, sizeof(TCHAR) * (len+1));
		MultiByteToWideChar(CP_ACP, 0, ptr, len, wptr, len+1);
		delete [] ptr;
		return wptr;
#else
		return ptr;
#endif
	}
}

void CServiceManager::SendUpdateWorldPvPRoom()
{
	ScopeLock <CSyncLock> sync(m_Sync);

	std::map <UINT, CServiceSession*>::iterator ii;
	for (ii = m_ServiceSessionList.begin(); ii != m_ServiceSessionList.end(); ii++)
	{
		if ((*ii).second->GetType() == MANAGED_TYPE_VILLAGE)
		{
			(*ii).second->SendUpdateWorldPvPRoom();
		}	
	}
}

void CServiceManager::SendCreateGambleRoom(int nRoomIndex, int nRegulation, int nGambleType, int nPrice )
{
	ScopeLock <CSyncLock> sync(m_Sync);

	std::map <UINT, CServiceSession*>::iterator ii;
	for (ii = m_ServiceSessionList.begin(); ii != m_ServiceSessionList.end(); ii++)
	{
		if ((*ii).second->GetType() == MANAGED_TYPE_VILLAGE)
		{
			(*ii).second->SendCreateGambleRoom( nRoomIndex, nRegulation, nGambleType, nPrice);
		}	
	}
}

void CServiceManager::SendStopGambleRoom()
{
	ScopeLock <CSyncLock> sync(m_Sync);

	std::map <UINT, CServiceSession*>::iterator ii;
	for (ii = m_ServiceSessionList.begin(); ii != m_ServiceSessionList.end(); ii++)
	{
		if ((*ii).second->GetType() == MANAGED_TYPE_VILLAGE)
		{
			(*ii).second->SendStopGambleRoom();
		}	
	}
}

void CServiceManager::SendDelGambleRoom()
{
	ScopeLock <CSyncLock> sync(m_Sync);

	std::map <UINT, CServiceSession*>::iterator ii;
	for (ii = m_ServiceSessionList.begin(); ii != m_ServiceSessionList.end(); ii++)
	{
		if ((*ii).second->GetType() == MANAGED_TYPE_VILLAGE)
		{
			(*ii).second->SendDelGambleRoom();
		}	
	}
}

void CServiceManager::SendUpdateDWCStatus()
{
	ScopeLock <CSyncLock> sync(m_Sync);

	std::map <UINT, CServiceSession*>::iterator ii;
	for (ii = m_ServiceSessionList.begin(); ii != m_ServiceSessionList.end(); ii++)
	{
		if ((*ii).second->GetType() == MANAGED_TYPE_VILLAGE || (*ii).second->GetType() == MANAGED_TYPE_LOGIN)
		{
			(*ii).second->SendUpdateDWCStatus();
		}	
	}
}

#if defined (_SERVICEMANAGER_EX)
bool CServiceManager::IsStartAllLauncherAndContinue(void)
{
	//���ĸ���Ʈ�� ���ļ��� ���� �ٸ��� ����ڿ��� ��ġ�� �Ұ����� Ȯ�� �� ����
	int nNotConnectCnt = CDataManager::GetInstance()->GetLauncherCount() - (int)m_LauncherSessionList.size();
	if(nNotConnectCnt > 0)
	{	
		if( AfxMessageBox(L"Find Offline Launcher!\nPatch Conitnue?", MB_YESNO) == IDNO)
			return false;
	}
	return true;
}
bool CServiceManager::IsRunLauncher(void){ 
	if(m_LauncherSessionList.empty())
		return false;

	return true; 
}
void CServiceManager::GetPartitionList(OUT vector<const TPartitionInfo*>& list) const
{
	CDataManager::GetInstance()->GetPartitionList(list);
}

void CServiceManager::GetPartitionText(OUT wchar_t text[256]) const
{
	CDataManager::GetInstance()->GetPartitionText(text);
}

void CServiceManager::GetWorldList(OUT vector<const TServerInfo*>& list) const
{
	CDataManager::GetInstance()->GetWorldList(list);
}

void CServiceManager::GetLauncherList(OUT vector<const TNetLauncher*>& list) const
{
	CDataManager::GetInstance()->GetLauncherList(list);
}

void CServiceManager::GetServiceServerList(OUT vector<const sServiceState*>& list) const
{
	for each (map<int, sServiceState*>::value_type v in m_ServiceList)
	{
		list.push_back(v.second);
	}
}

const TDefaultServerInfo& CServiceManager::GetDefaultServerInfo() const
{
	return CDataManager::GetInstance()->GetDefaultServerInfo();
}

const TDefaultDatabaseInfo& CServiceManager::GetDefaultDatabaseInfo() const
{
	return CDataManager::GetInstance()->GetDefaultDatabaseInfo();
}

const TServerInfo* CServiceManager::GetWorldInfo(int id) const
{
	return CDataManager::GetInstance()->GetServerInfo(id);
}

const TNetLauncher* CServiceManager::GetLauncherInfo(int id) const
{
	return CDataManager::GetInstance()->GetLauncherInfobyID(id);
}

const TServerExcuteData* CServiceManager::GetServerExecuteData(int id) const
{
	return CDataManager::GetInstance()->GetCommand(id);
}

void CServiceManager::GetActiveLaunchers(OUT vector<int>& list) const
{
	for each (map<UINT, CLauncherSession*>::value_type v in m_LauncherSessionList)
	{
		list.push_back(v.first);
	}
}

bool CServiceManager::IsLauncherConnected(int id) const
{
	map<UINT, CLauncherSession*>::const_iterator it = m_LauncherSessionList.find(id);
	if (it == m_LauncherSessionList.end())
		return false;

	return true;
}

void CServiceManager::OnPatchStart()
{
	m_pApp->OnPatchStart();
}

void CServiceManager::OnPatchProgress(int id, const wchar_t* key, unsigned long progress, unsigned long progressMax)
{
	m_pApp->OnPatchProgress(id, key, progress, progressMax);
}

void CServiceManager::OnPatchEnd()
{
	m_pApp->OnPatchEnd(m_bPatchComplete);
}

void CServiceManager::OnUnzipProgress(int id, const wchar_t* filename, unsigned long progress, unsigned long progressMax)
{
	m_pApp->OnUnzipProgress(id, filename, progress, progressMax);
}

void CServiceManager::OnWorldMaxUser(int id, int maxUser)
{
	m_pApp->OnWorldMaxUser(id, maxUser);
}

void CServiceManager::OnPatchFail(int id, const wchar_t* msg)
{
	m_pApp->OnPatchFail(id, msg);
}

void CServiceManager::OnPatchCompleted(int id)
{
	m_pApp->OnPatchCompleted(id);
}

void CServiceManager::BuildExceptionReport(size_t days, wstring& buffer) const
{
	m_pApp->ReportExceptionToBuffer(days, buffer);
}

void CServiceManager::OnCommandPatch()
{
	m_pApp->OnCommandPatch();
}

void CServiceManager::InitializeCommand()
{
	m_Commands.clear();

	m_Commands.insert(make_pair(L"makeselfdump", 0));
	m_Commands.insert(make_pair(L"testgsm", 0));
	m_Commands.insert(make_pair(L"reload", 0));
	m_Commands.insert(make_pair(L"reloaduser", 0));
	m_Commands.insert(make_pair(L"resetclose", 0));
	m_Commands.insert(make_pair(L"start", 0));
	m_Commands.insert(make_pair(L"starteach", 0));
	m_Commands.insert(make_pair(L"starteachbylauncher", 0));
	m_Commands.insert(make_pair(L"stopforce", 0));
	m_Commands.insert(make_pair(L"stopforceeach", 0));
	m_Commands.insert(make_pair(L"stopforceeachbylauncher", 0));
	m_Commands.insert(make_pair(L"stopall", 0));
	m_Commands.insert(make_pair(L"cancelstop", 0));
	m_Commands.insert(make_pair(L"stopdb", 0));
	m_Commands.insert(make_pair(L"stopeach", 0));
	m_Commands.insert(make_pair(L"stopeachbylauncher", 0));
	m_Commands.insert(make_pair(L"patchurl", 0));
	m_Commands.insert(make_pair(L"patchstruct", 0));
	m_Commands.insert(make_pair(L"patchinfo", 0));
	m_Commands.insert(make_pair(L"patchsystem", 0));
	m_Commands.insert(make_pair(L"patchconfig", 0));
	m_Commands.insert(make_pair(L"patchexe", 0));
	m_Commands.insert(make_pair(L"patchfull", 0));
	m_Commands.insert(make_pair(L"patchapply", 0));
	m_Commands.insert(make_pair(L"infocopy", 0));
	m_Commands.insert(make_pair(L"patch", 0));
	m_Commands.insert(make_pair(L"patchdyncode", 0));
	m_Commands.insert(make_pair(L"patchcash", 0));
	m_Commands.insert(make_pair(L"cancelpatchcash", 0));
	m_Commands.insert(make_pair(L"filelist", 0));
	m_Commands.insert(make_pair(L"notice", 0));
	m_Commands.insert(make_pair(L"noticeall", 0));
	m_Commands.insert(make_pair(L"noticechannel", 0));
	m_Commands.insert(make_pair(L"noticeserver", 0));
	m_Commands.insert(make_pair(L"noticezone", 0));
	m_Commands.insert(make_pair(L"cancelnotice", 0));
	m_Commands.insert(make_pair(L"worldmaxuser", 0));
	m_Commands.insert(make_pair(L"channelcontrol", 0));
	m_Commands.insert(make_pair(L"populationcontrol", 0));
	m_Commands.insert(make_pair(L"affinitycontrol", 0));
	m_Commands.insert(make_pair(L"eventupdate", 0));
	m_Commands.insert(make_pair(L"makedumpall", 0));
	m_Commands.insert(make_pair(L"makedumpeach", 0));
	m_Commands.insert(make_pair(L"reservenotice", 0));
	m_Commands.insert(make_pair(L"userban", 0));
	m_Commands.insert(make_pair(L"userbanid", 0));
	m_Commands.insert(make_pair(L"restraint", 0));
	m_Commands.insert(make_pair(L"startlauncher", 0));
	m_Commands.insert(make_pair(L"stoplauncher", 0));
	m_Commands.insert(make_pair(L"starteachlauncher", 0));
	m_Commands.insert(make_pair(L"stopeachlauncher", 0));
	m_Commands.insert(make_pair(L"patchlauncher", 0));
	m_Commands.insert(make_pair(L"patchlaunchereach", 0));
	m_Commands.insert(make_pair(L"checkpatch", 0));
	m_Commands.insert(make_pair(L"clearpatch", 0));
	m_Commands.insert(make_pair(L"checklauncher", 0));
	m_Commands.insert(make_pair(L"testlauncher", 0));
	m_Commands.insert(make_pair(L"checktestlauncher", 0));
	m_Commands.insert(make_pair(L"reloaddyncode", 0));
	m_Commands.insert(make_pair(L"checkalivelauncher", 0));
	m_Commands.insert(make_pair(L"selectjoin", 0));
	m_Commands.insert(make_pair(L"clearselectjoin", 0));
	m_Commands.insert(make_pair(L"updatesale", 0));
	m_Commands.insert(make_pair(L"patchbatch", 0));
	m_Commands.insert(make_pair(L"runbatch", 0));
	m_Commands.insert(make_pair(L"runbatcheach", 0));
	m_Commands.insert(make_pair(L"stopbatch", 0));
	m_Commands.insert(make_pair(L"checkbatch", 0));
	m_Commands.insert(make_pair(L"userbanfile", 0));
	m_Commands.insert(make_pair(L"farmupdate", 0));
	m_Commands.insert(make_pair(L"farmupdated", 0));
	m_Commands.insert(make_pair(L"farmstart", 0));
	m_Commands.insert(make_pair(L"guildwarupdate", 0));
	m_Commands.insert(make_pair(L"createdb", 0));
}
#endif // #if defined (_SERVICEMANAGER_EX)