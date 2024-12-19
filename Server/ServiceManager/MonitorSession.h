
#pragma once

#include "Connection.h"
#include "CriticalSection.h"
#include "DataManager.h"

class CMonitorSession : public CConnection
{
public:
	CMonitorSession();
	~CMonitorSession() {}

	int MessageProcess(int iMainCmd, int iSubCmd, char *pData, int iLen);

	void SendMonitorLoginResult(int nRetCode);

	//struct info
	void SendMeritList(int nContainerVersion, int nLastRecvID);
	void SendNetLauncherList(int nContainerVersion, int nLastRecvLauncherID);
	void SendChannelInfolist(int nContainerVersion, INT64 nLastRecvCHID);
	void SendServerInfoList(int nContainerVersion, int nLastRecvSID);

	//Update
	void SendUpdateNotice(int nUpdateType);
	void SendUpdateServerState(int nSID, int nServerState, int nServiceException, const char * pDetail);
	void SendServerDelayInfo(int nType, std::vector<int>& vServerDelayMIDs);

	//worldinfo
	void SendWorldInfo(int nContainerVersion);
	
	//UserCount & State
	void SendLoginUpdate(int nLastRecvLoginSID);
	void SendVillageUpdate(INT64 uiLastRecvCHID);
	void SendGameUpdate(int nLastRecvGameSID);
	void SendServerState(int nLastRecvSID);
	void SendFarmUpdate();

	//Send Operating Result
	void SendOperatingResult(int nMainCmd, int nRetCode);

	//Patch
	void SendPatchState();

	//
	int GetMonitorLevel() { return m_MonitorSessionLevel; }
	const char * GetMonitorID() { return m_szID; }



#if defined (_SERVICEMANAGER_EX)
	void SendServiceManagerReport(size_t nPage);

private:
	bool ReadServiceManagerReport(size_t nPage, OUT wchar_t* pBuffer) const;
#endif // #if defined (_SERVICEMANAGER_EX)

private:
	int m_MonitorSessionLevel;
	char m_szID[IDLENMAX];
	char m_szPass[PASSWORDLENMAX];

#if defined (_SERVICEMANAGER_EX)
	std::wstring m_ReportBuffer;
#endif // #if defined (_SERVICEMANAGER_EX)

	bool CheckAllowMsg(int nMainCmd);
};