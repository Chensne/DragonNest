
#pragma once

#include "LauncherSession.h"
#include "ServiceSession.h"
#include "PatcherSession.h"
#include "FtpUpdater.h"
#include "HttpUpdater.h"
#include "CriticalSection.h"
#include "DataManager.h"
#include "urlmon.h"
#include "URLUpdater.h"
#include "ReservedNotice.h"
#include "PatchWatcher.h"
#include "MonitorSession.h"
#include "ServiceCloser.h"
#include "WatcherGroup.h"
#include "LivePatcher.h"

const int _CLOSE_REPORTED_SERVICE_WAITING_TIME = 5 * 1000;
const int _CLOSE_SERVICE_WAITING_TIME = 240 * 1000;

const int _MERITSEND_CALCSIZE = 4000/sizeof(sMonitorChannelMeritInfo);
const int _NETLAUNCHER_CALCSIZE = 4000/sizeof(sMonitorNetLauncherInfo);
const int _CHANNEL_CALCSIZE = 4000/sizeof(sMonitorChannelInfo);
const int _SERVERINFO_CALCSIZE = 4000/sizeof(sMonitorServerInfo);

const int _LOGINUPDATE_CALCSIZE = 4000/sizeof(sUserCountInfo);
const int _VILLAGEUPDATE_CALCSIZE = 4000/sizeof(sChannelUpdateInfo);
const int _GAMEUPDATE_CALCSIZE = 4000/sizeof(sGameUserInfo);
const int _SERVERSTATE_CALCSIZE = 4000/sizeof(sServerStateInfo);
const int _FARMUPDATE_CALCSIZE = 4000/sizeof(TFarmStatus);

static std::wstring _SZACCOUNTNAMETYPE = L"AccountName";
static std::wstring _SZCHARACTERNAMETYPE = L"CharacterName";
static std::wstring _SZACCOUNTIDTYPE = L"AccountID";

enum eServiceProcessValue
{
	SERVICEPROCESS_NONE = 0,
	SERVICEPROCESS_DELAYED,
	SERVICEPROCESS_FAULTSAFE,
};

typedef void (__cdecl *LPFARMUPDATE)();

typedef struct 
{
	LPFARMUPDATE lpFarmUpdate;
} CMDTABLE, *LPCMDTABLE;

#if defined (_SERVICEMANAGER_EX)
class CServiceManagerExApp;
#endif // #if defined (_SERVICEMANAGER_EX)

class CServiceManager
{
public:
#if defined (_SERVICEMANAGER_EX)
	CServiceManager(CServiceManagerExApp* pApp);
#else
	CServiceManager();
#endif // #if defined (_SERVICEMANAGER_EX)
	~CServiceManager();

	//test
	void MakeRandomException();

	int m_nMajorVersion;
	int m_nMinorVersion;

	void IdleProcess();
	void SyncTaskProcess();

	//Connections
	CLauncherSession * AddLauncherSession(const char * pIP, unsigned short nPort);
	CServiceSession * AddServiceSession(const char * pIP, unsigned short nPort);
	CPatcherSession * AddPatcherSession(const char * pIP, unsigned short nPort);
	CMonitorSession * AddMonitorSession(const char * pIP, unsigned short nPort);

	bool DelConnection(CConnection * pCon, eConnectionKey eConType);

	//Command Parser
	void ParseCommand(const char* pCmd, char* pRet = NULL);
	int Parse(const char * pCmd, char * pRet = NULL);

	//GSM Operator
	bool ReLoadStruct();			//Load ServerStruct
	bool ReLoadUser();				//Load MonitorUser

	//Monitor Operator
	int CheckMonitorLogin(const char * pID, const char * pPass, int & nMonitorLevel, TP_SMVERSION Version);	
	void CalcPageSize(int &nFirst, int &nLast, BYTE cOffSet, BYTE cPageNum, int nMaxSize, int nWholeCount);
	void SendExceptionReport(int nSID, int nExceptionType, const char * pDetail);
	void SendUpdateNotice(int nNotice);

	//for struct
	int GetMeritList(int nContainerVersion, int nLastRecvID, BYTE & cIsLast, short & cCount, sMonitorChannelMeritInfo * pInfo, int nDestCountMax);
	int GetNetLauncherList(int nContainerVersion, int nLastRecvLauncherID, BYTE & cIsLast, short & cCount, sMonitorNetLauncherInfo * pInfo, int nDestCountMax);
	int GetServerList(int nContainerVersion, int nLastRecvSID, BYTE & cIsLast, short & cCount, sMonitorServerInfo * pInfo, int nDestCountMax);
	int GetChannelList(int nContainerVersion, INT64 nLastRecvCHID, BYTE & cIsLast, short & cCount, sMonitorChannelInfo * pInfo, int nDestCountMax);

	//for livepatch
	bool GetMangedIDList(std::vector <int> &vList);

	//WorldInfo
	int GetWorldInfo(int nContainerVersion, BYTE & cCount, sMonitorWorldInfo * pInfo);

	//for update
	int GetLoginUpdateList(int nLastRecvLoginSID, BYTE & cIsLast, short & nCount, sUserCountInfo * pInfo, int nDestCountMax);
	int GetVillageUpdateList(INT64 nLastRecvCHID, BYTE & cIsLast, BYTE & cCount, sChannelUpdateInfo * pInfo, int nDestCountMax);
	int GetGameUpdateList(int nLastRecvGameSID, BYTE & cIsLast, short & nCount, sGameUserInfo * pInfo, int nDestCountMax);	
	int GetServerStateList(int nLastRecvSID, BYTE & cIsLast, short & nCount, sServerStateInfo * pInfo, int nDestCountMax);
	void GetFarmUpdateList(short nFarmCount, short & nCount, TFarmStatus * pInfo, int nDestCountMax);

	//Operating Command
	int StartServiceAll(char * pMsg = NULL);		//Start All Server
	int StartServiceEach(int nSID, bool bForcePatch, char * pMsg = NULL);
	int StartServiceEachbyLauncher(int nNID, char * pMsg = NULL);
#if defined (_SERVICEMANAGER_EX)
	bool IsRunLauncher();
	bool IsStartAllLauncherAndContinue();
#endif	// #if defined (_SERVICEMANAGER_EX)

	void ResetCloseService(char * pMsg = NULL);
	int TerminateServiceForce(char * pMsg = NULL);	
	int TerminateServiceEachForce(int nSID, char * pMsg = NULL);
	int TerminateServiceAll(char * pMsg = NULL, int nAfterClose = 0);
	int TerminateServiceDB(char * pMsg = NULL, bool bWithOutLock = false);
	int TerminateServiceEach(int nSID, char * pMsg = NULL, bool bReCreate = false);
	int TerminateServiceEachbyLauncher(int nNID, char * pMsg = NULL);
	int TerminateServiceForceEachbyLauncher(int nNID, char * pMsg = NULL);
	int CancelTerminateProcess();

	void NoticeWholeWorld(int nSec, const char * pMsg);
	void NoticeWorld(int nMasterMID, int nSec, const char * pMsg);
	void NoticeServer(int nMasterMID, int nMID, int nSec, const char * pMsg);
	void NoticeChannel(int nMasterMID, int nChannelID, int nSec, const char * pMsg);
	void NoticeZone(int nMasterMID, int nMapIdx, int nSec, const char * pMsg);
	void NoticeCancel(int nMasterMID);

	void MakeDumpAll();
	int MakeDumEach(int nMID);

	void WorldMaxuserControl(int nMasterMID, UINT nChangeMaxuser);
	int PopulationControl(int nMasterMID, int nControlMID, bool bZeroPopulation);
	int ChannelControl(int nMasterMID, int nChannelID, bool bVisibility, int nChannelPopulation);
	void GameAffinityControl(int nMasterMID, int nGameMID, int nType);
	void EventUpdate();
	void SimpleConfigUpdate();
	void ReserveNoticeUpdate(const char * pURL, char * pRet = NULL);
	void UserRestraint(UINT nAccountDBID, char * pRet = NULL);
	void UserBanbyCharacterName(const char * pName);
	void UserBanbyCharacterName(const WCHAR * pwszName);
	void UserBanbyAccountDBID(UINT nAccountDBID);
	void UserBanbyAccountName(const WCHAR * pwszName);

	//patch content
	void PatchUpdateUrl(const char * pUrl, char * pMsg = NULL);
	void PatchStruct(char * pMsg = NULL);
	void PatchInfo(int nNID, char * pMsg = NULL);
	void PatchSystem(int nNID, char * pMsg = NULL);
	void PatchConfig(int nNID, char * pMsg = NULL);
	void PatchExe(int nNID, char * pMsg = NULL);
	void PatchFull(int nNID, char * pMsg = NULL);
	void PatchApply(char * pMsg = NULL);
	void InfoCopy(int nNID, char * pMsg = NULL);	
	void Patch(const char * pUrl, char * pMsg = NULL);
	void PatchBatch(int nNID, char * pMsg = NULL);
	void RunBatch(int nNID, char * pMsg = NULL);
	void StopBatch(int nNID, char * pMsg = NULL);
	void PatchESM(int nNID, char * pMsg = NULL);
	int StartESM();
	int StopESM();

	//for chn
	void PatchDyncode(char * pMsg = NULL);
	void ReloadDyncode(char * pMsg = NULL);

	//for extpatch
	void PatchExt();
	void CancelExtPatch();
	void RecvFileList(const char * pUrl);
	bool CompleteLivePatchItem(int nPatchID);
	void CompleteLiveExtCopy(bool bRet);
	void CompleteLiveExtReload(int nMID, bool bRet);

	void SendDelExt();
	void SendCopyExt();

	bool SendExtReload(int nReLoadType);

	//cash
	void SendChangeLimiteItemMax(int nSN, int nLimitMax);

	//NetLauncher Service For GSM
	void StartLauncher(char * pMsg = NULL);
	void StopLauncher(char * pMsg = NULL);
	void StartEachLauncher(int nPSID, char * pMsg = NULL);
	void StopEachLauncher(int nPSID, char * pMsg = NULL);
	void StopEachLauncherAsync(int nPSID);

	//NetLauncher Patch
	void PatchNetLauncher(char * pMsg = NULL);
	void PatchNetLauncherEach(int nID, char * pMsg = NULL);
	void PatchedLauncher(int nNID);

	//check
	void CheckPatch();
	void Clearpatch();
	void CheckNetLauncher(int nNID = -1);
	void TestLauncher(int nNID);
	void CheckBatch(int nNID = -1);
	void CheckAliveNetLauncher();

	//Service Launcher Reported
	bool CreateServiceServer(int nSID);
	bool OnlineServiceServer(int nSID, int nServiceSessionID, const char * pVersion, const char * pResVersion, CServiceSession * pSession, BYTE cWorldCount, BYTE * pWorldID, bool bFinalBuild);

	bool ReportTerminateService(int nSID);
	bool ReportServiceInfoLogin(int nSID, int nLoginUser);
	bool ReportServiceInfoVillage(int nSID, UINT nUserCount, int nCnt, TVillageUserReport * pReport);
	bool ReportServiceMaster(int nSID, int nWorldID, UINT nWaitUserCount);
	bool ReportServiceInfoGame(int nSID, UINT nUserCount, UINT nRoomCount);
	void ReportedServiceClosed(int nSID);

	bool CheckAllServiceOnline();

	//
	void LauncherConnected(CLauncherSession * pSession);
	void EjectService(int nSID);

	//Channel
	void UpdateWorldMaxUser(TServiceWorldInfo * pPacket);
	void UpdateMeritInfo(TServiceMeritInfo * pPacket);
	void UpdateChannelInfo(TServiceChannelInfo * pPacket);

	//GetManaged Master
	int GetMasterSIDByChannelID(int nWorldID, int nChannelID);
	bool GetMasterSIDByGameSID(int nGameSID, int * pnMasterIDs);

	int GetDBWorldIDBySID (int nSID);

	CPatchWatcher * GetPatchWatcher() { return &m_PatchWatcher; }
	int GetContainerVersion() { return m_nContainerVersion; }

	CRunBatchWatcher * GetRunBatchWatcher() { return &m_RunBatchWatcher; }
	CStopBatchWatcher * GetStopBatchWatcher() { return &m_StopBatchWatcher; }

	//test Launcher
	void LauncherReturn(int nNID, int nIdx, ULONG nReturnPing);
	void CheckTestLauncher();

	// 선택진입
	void SelectJoin(int nMasterSID, int nSID, const WCHAR * pwszCharacterName);
	void ClearSelectJoin(int nMasterSID);
	void SendServiceClose(__time64_t _tOderedTime, __time64_t _tCloseTime);

	// 캐쉬커머디티 업데이트
	void UpdateSaleData();
	// 대량밴처리
	void UpdateBanFile(const char * pUrl);

	//농장
	void UpdateFarm(int nWorldID);
	void StartFarm();				//강제시작
	void ReqUpdateFarm();
	static void UpdateFarm();
	void UpdateFarmStatus (TServiceReportMaster* pPacket);

	//길드전
	void UpdateGuildWarSchedule();

	//
	void SendUpdateGuildWare(int nGuildID);

	//
	int SendPatchEachItem(int nNID, const WCHAR * pURL, const WCHAR * pDest);
	void AddServerDelayMID(int nType, int nMID);
	void ReportToMonitorServerDelay(int nType);

	// 월드통합파티룸
	void SendUpdateWorldPvPRoom();

	// 중국 도박모드
	void SendCreateGambleRoom(int nRoomIndex, int nRegulation, int nGambleType, int nPrice );	
	void SendStopGambleRoom();	
	void SendDelGambleRoom();	

	// DWC 모드
	void SendUpdateDWCStatus();

private:
	bool m_bCloseService;
	ULONG m_nTerminateDBTick;
	char m_szProcess[256];

	//GSM
	bool m_bHttpSendFail;
	ULONG m_nHttpFailTick;

	bool m_bPatchComplete;

	//Sessions
	std::map <UINT, CLauncherSession*> m_LauncherSessionList;
	std::map <UINT, CServiceSession*> m_ServiceSessionList;

	std::vector <CPatcherSession*> m_vPatcherSessionList;
	std::map <UINT, CPatcherSession*> m_PatcherSessionList;
	std::vector <CMonitorSession*> m_MonitorSessionList;

	bool VerifyPatcher(const char * pIP);

	//Now OnLine ServiceList
	std::map <int, sServiceState*> m_ServiceList;
	std::map <INT64, tMonitorChannelInfo> m_ChannelList;
	std::map <int, TMeritInfo> m_ChannelMerit;
	std::map <int, sMonitorWorldInfo> m_WorldInfo;
	std::map <int, std::vector<TFarmStatus>> m_FarmStatus[WORLDCOUNTMAX];

	//Container Version
	int m_nContainerVersion;

	//AutoRestart
	std::vector <int> m_AutoRestartList;

	struct _CLOSESERVICE
	{
		int nMID;
		ULONG nInsertTick;
		ULONG nTerminateTick;
	};
	std::vector <_CLOSESERVICE> m_CloseServieList;

	CSyncLock m_Sync;

	bool IsCloseService(int nSID);
	bool ExcuteProcess(const TServerExcuteData * pExe, bool bForcePatch = false);		//with sync

	bool MakeServiceList(int nSID, int nNetLauncherID, const TCHAR * pType, bool bForce = false);
	bool MakeWorldList();
	sServiceState * GetServiceState(int nSID);
	sServiceState * GetServiceStateByServiceSession(int nServiceSessionID);
	int GetNeedResType(int nNetLauncherID);

	bool TerminateServiceEachForceAsync(int nSID);
	void StartServiceEachAsync(int nSID);

	void DeleteManagedService(int nNetLauncherID);
	void ClearList();	//with async

	void EjectForce();
	void EjectServiceAll();
	void EjectServiceDB();
	bool IsService();
	bool IsService(int nSID);

	CURLUpdater m_UrlUpdater;
	CReservedNotice m_ReservedNotice;
	CserviceCloser m_ServiceCloser;
	CPatchWatcher m_PatchWatcher;
	CLivePatcher m_LivePatcher;
	int m_nPatchID;

	CRunBatchWatcher m_RunBatchWatcher;
	CStopBatchWatcher m_StopBatchWatcher;
	int m_nRunBatchID;
	int m_nStopBatchID;

	void SendPatchStart(int nNID);
	void SendPatch(int nNID, const WCHAR * pKey, int nPatchID);
	void SendPatchEnd(int nNID);
	void SendInfoCopy(int nID);

	// Login
	void SendCreatEachDB(int nWorldID);

	//guildwar
	void SendReloadGuildwar(int nWorldID);

	//
	bool AmountTokenizedString(char * pExtStr, int nExtMaxSize, std::vector<std::string> &vStr, int nStartCount, int nEndCount);

	//
	int GetServiceCount(int nServiceType, std::map <UINT, int> * pmList);
	int GetVillageServiceCount();
	int GetGameServiceCount();
	bool GetChannelInfo(int nChannelID, short & nMeritID, int & nLimitLevel, bool & bVisibility, bool & bShow);

	//Report
	void ReportToGSMServiceState(int nSID, int nServiceState, const char * pVersion, const char * pRevision);
	void ReportToMonitorServiceState(int nSID, int nServiceState, int nServiceException = _EXCEPTIONTYPE_NONE, const char * pDetail = NULL);

	//test launcher
	std::map <int, ULONG> m_LaunchetTestList;

	CHttpUpdater m_HttpUpdater;
	CSyncLock m_HttpSync;

	struct _SYNCGSMTASK
	{
		short nSize;
		char szMsg[4100];
	};
	CSpinBuffer * m_pStoredPacketBuffer;

	//Report to GSM
	bool SendPacketToGsm(const WCHAR * fmt, ...);
	bool SendPacketToGsm(char * fmt, int nSize);

	//Read
	TCHAR * ReadFile(const TCHAR * pPath);

	//
	CMDTABLE m_CmdTable;
	ULONG m_nLastMonitorReportedTick[SERVERTYPE_MAX];
	std::vector<int> m_vServerDelayMIDs[SERVERTYPE_MAX];

#if defined (_SERVICEMANAGER_EX)
public:
	void GetPartitionList(OUT vector<const TPartitionInfo*>& list) const;
	void GetWorldList(OUT vector<const TServerInfo*>& list) const;
	void GetLauncherList(OUT vector<const TNetLauncher*>& list) const;
	void GetServiceServerList(OUT vector<const sServiceState*>& list) const;
	void GetPartitionText(OUT wchar_t text[256]) const;

	const TDefaultServerInfo& GetDefaultServerInfo() const;
	const TDefaultDatabaseInfo& GetDefaultDatabaseInfo() const;

	const TServerInfo* GetWorldInfo(int id) const;
	const TNetLauncher* GetLauncherInfo(int id) const;
	const TServerExcuteData* GetServerExecuteData(int id) const;

	void GetActiveLaunchers(OUT vector<int>& list) const;
	bool IsLauncherConnected(int id) const;

public:
	void OnPatchStart();
	void OnPatchProgress(int id, const wchar_t* key, unsigned long progress, unsigned long progressMax);
	void OnPatchEnd();
	void SetPatchComplete(bool bComplete) { m_bPatchComplete = bComplete;};

	void OnUnzipProgress(int id, const wchar_t* filename, unsigned long progress, unsigned long progressMax);
	void OnWorldMaxUser(int id, int maxUser);
	void OnPatchFail(int id, const wchar_t* msg);	
	void OnPatchCompleted(int id);
	void OnCommandPatch();

public:
	void BuildExceptionReport(size_t days, wstring& buffer) const;

public:
	void InitializeCommand();
	const map<const wstring, int>& GetCommandList() const { return m_Commands; }

private:
	CServiceManagerExApp* m_pApp;
	map<const wstring, int> m_Commands;
#endif // #if defined (_SERVICEMANAGER_EX)
};

extern CServiceManager * g_pServiceManager;