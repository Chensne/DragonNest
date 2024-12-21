
#pragma once

#include "Connection.h"

class CServiceSession : public CConnection
{
public:
	CServiceSession();
	~CServiceSession();

	int IdleProcess(ULONG nCurTick);
	int MessageProcess(int iMainCmd, int iSubCmd, char *pData, int iLen);

	void SendSessionInfo();
	void SendNoticeMassage(int nNoticeType, int nManagedID, int nChannelID, int nMapIdx, int nSec, const char * pMsg);
	void SendNoticeCancel();
	void SendMakeDump();
	void SendChangeWorldMaxuser(UINT nWorldMaxUser);
	void SendChannelControl(int nChannelID, bool bVisibility, int nChannelPopulation);
	void SendPopulationControl(int nServerType, int nManagedID, bool bZeroPopulation);
	void SendGameAffinity(int nManagedID, int nType);
	void SendUpdateEvent();
	void SendUpdateSimpleConfig();
	void SendCloseService();
	void SendUserRestraint(UINT nAccountDBID);
	void SendUserBan(const WCHAR * pName, UINT nAccountDBID, eBanFileType eType);
	void SendPing();
	void SendSelectJoin(int nSID, const WCHAR * pwszCharacterName);
	void SendClearSelectjoin();
	void SendScheduleCloseService(__time64_t _tOderedTime, __time64_t _tCloseTime);
	void SendUpdateSaleData();
	void SendUpdateFarm(int nWorldID, bool bForceStart = false);
	void SendUpdateGuildWarSchedule();
	void SendUpdateGuildWare(int nGuildID);
	void SendExtReload(int nType);
	void SendCreatEachDB(int nWorldID);
	void SendReloadSchedule(int nWorldID);
	void SendChangeLimitedItemMax(int nSN, int nLimitMax);
	void SendUpdateWorldPvPRoom();
	void SendCreateGambleRoom(int nRoomIndex, int nRegulation, int nGambleType, int nPrice );
	void SendStopGambleRoom();
	void SendDelGambleRoom();
	void SendUpdateDWCStatus();


	//for chn
	void SendReloadDynCode();

	int GetMID() { return m_nMID; }
	int GetType() { return m_nManagedType; }		//eManagedType
	bool GetClose() { return m_bIsServiceClose; }

	ULONG GetLastReportTime() { return m_nLastReportedTick; }
	ULONG GetLastDelayedTime() { return m_nLastDelayedTick; }
	void SetLastDelayedTime(ULONG nLastDelayedTick) { m_nLastDelayedTick = nLastDelayedTick; }

	int m_nLoginUserCount;
	int m_nVillageUserCount;
	int m_nGameUserCount;
	int m_nReportedCount;
	bool m_bZeroPopulation;
	int m_nGameRoomCount;
	int m_nTotalRoomCount;

	//checker
	bool IsCompleteConnected(ULONG nCurTick);

private:
	int m_nMID;
	int m_nManagedType;
	char m_szVersion[NAMELENMAX];
	char m_szResVersion[NAMELENMAX];
	ULONG m_nCreateSessionTick;

	bool m_bIsServiceClose;

	//for village
	ULONG m_nLastIdleTick;
	ULONG m_nLastPingTick;
	int m_nVillageFaultSafeCount;
	bool m_bFaultSafeDump;

	//for game
	ULONG m_nLastReportedTick;
	ULONG m_nLastDelayedTick;

	bool m_bUnrecoveryBlock;
	ULONG m_nUnrecoveryTick;
	bool m_bPermanentlyBlock;

	//for logcount
	ULONG m_nLastIdleLogTick;
	std::string m_szPublicIP;

};