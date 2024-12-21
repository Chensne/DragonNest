
#pragma once

#include "Connection.h"
#include "CriticalSection.h"

class CDNIocpManager;
class CDNServiceConnection : public CConnection
{
public:
	CDNServiceConnection(int nManagedID);
	~CDNServiceConnection();

	int MessageProcess(int iMainCmd, int iSubCmd, char *pData, int iLen);
	void Reconnect(DWORD CurTick);
	void Disconnected();	
	
	void ScopeElapsedTick(ULONG nDelta);
	void SendDetectException(int nExceptionType);
	inline int GetManagedID() { return m_nManagedID; }

#if defined (_MASTERSERVER)
	void SendWorldInfo();
	void SendMeritInfo();
	void SendVillageInfo(UINT nUserCnt, std::vector<TVillageInfo> * vList);
#endif

#if defined( _VILLAGESERVER )
	void SendUpdateWorldPvPRoom(int nRetCode);
#endif

	void LoadWorldEvent();

#ifdef PRE_MOD_RESTRICT_IDENTITY_IP
	void LoadSimpleConfig();
#endif		//#ifdef PRE_MOD_RESTRICT_IDENTITY_IP

	//
	void SendServiceClosed();
	void SendRportDelayedProcess(int nManagedID, ULONG nDelayedTick);
	void SendReportUnrecoverBlock(bool bBlock, bool bPermannently = false);
	bool GetServerClose() { return m_bCloseServer; }

	//
	void SendReloadResult(bool bResult);
	void SendLoginWorldDBResult(int nResult, int nWorldID, char* szIp, int nPort);
	void SendQueryDelayedReport(int nManagedID, const char* szQueryName, DWORD dwThreadID, DWORD dwElapsed);
	void SendGameDelayedReport(WCHAR* wszLog);
	void SendVillageDelayedReport(WCHAR * wszLog);
	void RealTimeReport() {ReportProcess();}

	void SendHeartBeat();

private:
	CSyncLock m_Sync;
	int m_nManagedID;
	int m_nManagedType;
	DWORD m_dwReconnectTick;	

	bool m_bIsInit;

	void ReportProcess();

	//Send To ServiceManager
	void SendReqeustServiceInfo();
	void SendRegistManager(int nManagedID, int nType, const char * pVersion, const char * pResVersion, int nWorldID, BYTE cWorldCount = 0, BYTE * pWorldID = NULL);
	void SendReportLogin(UINT nUserCount);
	void SendReportVillage(UINT nUserCount, std::map <int, int> & Map);
	void SendReportMaster(int nWorldID, UINT nWaitUserCount);
	void SendReportGame(bool bZeropopulation, UINT nUserCount, UINT nRoomCount, UINT nTotalRoomCount);
	void SendPong();
	void SendGPKLoadFail();

	int m_nScopeCnt;
	ULONG m_nScopeElapsed[32];
	
	volatile bool m_bCloseServer;
};

extern CDNServiceConnection * g_pServiceConnection;


template < class _T >
class CCheckScopeDelay
{
public:
	CCheckScopeDelay(_T & ReportObj) : m_pReportObj(&ReportObj)
	{
		m_nPreTick = timeGetTime();
	}

	~CCheckScopeDelay()
	{
		unsigned long nDelta = timeGetTime() - m_nPreTick;
		if (m_pReportObj && nDelta > PROCESS_DELAY_CHECK_TICK)
			m_pReportObj->ScopeElapsedTick(nDelta);
	}

private:
	_T * m_pReportObj;
	unsigned long m_nPreTick;
};