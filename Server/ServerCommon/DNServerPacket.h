#pragma once

#if !defined(DNPRJSAFE)
#include "DNPrjSafe.h"
#endif	// DNPRJSAFE
#include "DNProtocol.h"
#include "DNUserData.h"
#include "DNServerProtocol.h"
#include "DNServerPacketVillage.h"
#include "SMVersion.h"

/*--------------------------------------------------------------------------
				Server 내에서 사용하는 Protocol, Packet

	=> 파일이 길어져서 DNServerProtocol.h 랑 분리시킴 090218
	=> 더 길어져서 이제 서버별로 분리 ㅋㅋ 100124
	   (db, game, login, master, village외에 것들만 이곳에)
--------------------------------------------------------------------------*/

#pragma pack(push, 1)

//-------------------------------------------------
//	ServiceManager <-> NetLauncher or Service
//-------------------------------------------------
struct sServiceState
{
	int nSID;
	int nManagedLauncherID;
	int nServiceSessionID;
	int nState;			//eServiceState
	TCHAR szType[NAMELENMAX];
	ULONG nLastReportTick;
	char szVersion[NAMELENMAX];
	char szResource[32];
	BYTE cWorldIDs[WORLDCOUNTMAX];
	ULONG nCreateTime;
	bool bFinalBuild;
	bool bIsReConnect;	//재접 시도을 1번만 허용하기 위해 추가

	void Init()
	{
		nServiceSessionID = 0;
		nLastReportTick = 0;
		nState = _SERVICE_STATE_OFFLINE;
		memset(szVersion, 0, sizeof(szVersion));
		memset(szResource, 0, sizeof(szResource));
		nCreateTime = 0;
		bFinalBuild = false;
		bIsReConnect = false;
	}
};

struct TNetResult				//NETSERVICE_RESULT
{
	char cAssingedType[MANAGEDMAX];
	WCHAR szBaseUrl[MAX_PATH];
	WCHAR szPatchUrl[MAX_PATH];
};

struct TRunProcess			//NETSERVICE_RUN_PROCESS
{
	int nSID;
};

struct TTerminateProcess		// NETSERVICE_TERMINATE_PROCESS
{
	int nSID;
	bool bRestart;
};

struct TPatch		//NETSERVICE_PATCH
{
	int nPatchResLevel;
	int nPatchID;
	WCHAR szKey[64];
};

struct TPatchUrl		//NETSERVICE_PATCH_BYURL
{
	int nPatchID;
	WCHAR szKey[64];
	bool bOnlyUseURL;
	WCHAR szDest[64];
	short nLen;
	char szUrl[1024];
};

struct TInfoCopy		//NETSERVICE_INFOCOPY
{
	bool bNeed;
};


struct TReturnPing		//NETSERVICE_RETURN_PING
{
	int nIdx;
	ULONG nTick;
};

struct TServiceConnected	//NETSERVICE_CONNECTED
{
	int nSessionID;
};

struct TServiceNotice			//NETSERVICE_NOTICE
{
	int nNoticeType;			//eSMNoticeType
	int nManagedID;
	int nChannelID;
	int nMapIdx;
	int nSlideShowSec;			//몇초동안 슬라이드 공지(전광판....-_-;?) 이 값이 0 이상이면 전광판이다
	USHORT nLen;
	WCHAR wszMsg[CHATLENMAX];
};

struct TServiceChannelControl			//NETSERVICE_CHANNEL_CONTROL
{
	int nChannelID;
	BYTE cVisibility;
	int nChannelPopulation;
};

struct TServicePopulationControl			//NETSERVICE_POPULATION_CONTROL
{
	int nServerType;
	int nManagedID;
	BYTE cZeroPopulation;						//인구수 조절
};

struct TServiceAffinityControl			//NETSERVICE_AFFINITY_CONTROL
{
	int nManagedID;
	BYTE cAffinityType;
};

struct TServiceUserRestraint			//NETSERVICE_USERRESTRAINT
{
	UINT nAccountDBID;
};

struct TServiceUserBan					//NETSERVICE_USERBAN
{
	eBanFileType eType;
	UINT nAccountDBID;
	WCHAR wszName[NAMELENMAX];
};

struct TServiceWordlMaxUser				//NETSERVICE_WORLDMAXUSER
{
	UINT nChangeMaxUser;
};

struct TServiceSelectjoin	//NETSERVICE_SELECTJOIN
{
	int nSID;
	WCHAR wszCharacterName[NAMELENMAX];
};

struct TScheduleServiceClose	//NETSERVICE_SCHEDULE_SERVERCLOSE
{
	__time64_t _tOderedTime;
	__time64_t _tCloseTime;
};

struct TUpdateFarm				//NETSERVICE_UPDATEFARM
{
	int nWorldID;
	bool bForceStart;
};

struct TExtReload				//NETSERVICE_EXTRELOAD
{
	int nType;
};

struct TUPdateGuildWare			//NETSERVICE_UPDATEGUILDWARE
{
	int nGuildID;
};

struct TCreateDB				//NETSERVICE_CREATEWORLDDB
{
	int nWorldID;
};

struct TReloadGuildwarSchedule	//NETSERVICE_RELOADGUILDWARSCHEDULE
{
	int nWorldID;
};

struct TChangeLimitItemMax		//NETSERVICE_LIMITEITEM_CHANGEQUANTITY
{
	int nSN;
	int nLimiteMax;
};

struct TServiceStartManage			//NETSERVICE_STARTOF_MANAGING
{
	bool bOther;
};

struct TServiceManaging					//NETSERVICE_MANAGING
{
	int nSID;
	BYTE cCreateCount;
	BYTE cCreateIndex;
	USHORT nCmdSize;
	char szCmd[EXCUTELENMAX + EXCUTECMDMAX];
};

struct TPatcherSrevicePatchState		//PATCHERSERVICE_PATCHSTATE
{
	BYTE cState;				// 0 fail, otherwise success
};

struct TServerManagedList	//SERVICE_LAUNCHER_PROCESS_LIST
{
	BYTE cCount;
	int nListArr[255];
};

struct TServerProcessState		//SERVICE_LAUNCHER_PROCESS_STATE
{
	BYTE cPatch;
	char szState[256];
};

struct TServerEsmState
{
	char szState[256];
};

struct TServiceLauncherVersion		//SERVICE_LAUNCHER_VERSION
{
	char szVersion[SERVERVERSIONMAX];
};

struct TPatchState		//SERVICE_LAUNCHER_PATCH_STATE
{
	bool bFlag;
	int nPatchID;
	BYTE cLenState;
	BYTE cLenKey;
	char szState[128];
};

struct TPatchProgress //SERVICE_LAUNCER_PATCH_DATA_PROGRESS
{
	ULONG progress;
	ULONG progressMax;
	BYTE keyLen;
	WCHAR key[32];
};

struct TUnzipProgress
{
	ULONG progress;
	ULONG progressMax;
	short len;
	WCHAR filename[MAX_PATH + 1];
};

struct TSendPing		// SERVICE_LAUNCHER_PING
{
	char szPublicIP[IPLENMAX];
};

struct TRunBatch
{
	int nBatchID;
};
struct TRunBatchState
{
	int nRet;
	int nBatchID;
};

struct TStopBatch
{
	int nBatchID;
};

struct TStopBatchState
{
	int nBatchID;
};

struct TLiveExtCopy		//SERVICE_LAUNCHER_LIVEEXTCOPY
{
	bool bCopyRet;
};

struct TServerManagedID		//SERVICE_SERVER_REGIST_MANAGEDID
{
	int nMID;
	int nManagedType;
	int nServiceID;
	bool bFinalBuild;
	char szVersion[NAMELENMAX];
	char szResVersion[NAMELENMAX];
	BYTE cWorldCount;
	BYTE cWorldID[WORLDCOUNTMAX];
};

struct TServiceReportLogin				//SERVICE_SERVER_USUAL_LOGINREPORT
{
	UINT nUserCount;
};

struct TServiceReportVillage			//SERVICE_SERVER_USUAL_VILLAGEREPORT
{
	UINT nUserCount;
	USHORT nCount;
	TVillageUserReport Info[VILLAGECHANNELMAX];
};

struct TFarmStatus
{
	int nManagedID;
	UINT nFarmDBID;
	int nFarmCurUserCount;
	bool bActivate;
};

struct TServiceReportMaster				//SERVICE_SERVER_USUAL_MASTERREPORT
{
	int nWorldID;
	UINT nWaitserCount;
	BYTE cFarmCount;
	TFarmStatus FarmStatus[Farm::Max::FARMCOUNT/2];
};

struct TGameUserReport
{
	int nMapIdx;
	BYTE cUserCount;
};

struct TServiceReportGame				//SERVICE_SERVER_USUAL_GAMEREPORT
{
	BYTE cIsZeroPopulation;
	UINT nUserCount;
	UINT nRoomCount;
	UINT nTotalRoomCount;
};

struct TServiceDelayedReport		//SERVICE_SERVER_DELAYEDREPORT
{
	int nManagedID;
	ULONG nDelayedTick;
};

struct TServiceUnrecover			//SERVICE_SERVER_UNRECOVERY
{
	bool bBlock;
	bool bPermannently;
};

struct TServiceChannelInfo			//SERVICE_SERVER_CHANNELINFO
{
	int nManagedID;
	int nWorldID;
	BYTE cCount;
	sChannelInfo Info[VILLAGECHANNELMAX];
};

struct TServiceWorldInfo			//SERVICE_SERVER_WORLDINFO
{
	int nWorldID;
	int nWorldMaxuser;
};

struct TServiceMeritInfo			//SERVICE_SERVER_MERITINFO
{
	BYTE cCount;
	TMeritInfo Info[MERITINFOMAX];
};

struct TServerException			//SERVICE_SERVER_EXCEPTION_REPORT
{
	int nMID;
	int nType;
};

struct TServerExtReloadResult		//SERVICE_SERVER_EXT_RELOADRESULT
{
	bool bResult;
};

struct TServerLoginWorldDResult		// SERVICE_SERVER_LOGIN_WORDDBRESULT
{
	int nResult;				// 0 : Success / 1:Connect / 2:Version / 3:Not Find / 4:Already Created
	int nWorldID;
	char szIp[IPLENMAX];
	int nPort;
};

struct TQueryDelayedReport
{
	int nMID;
	DWORD dwThreadID;
	DWORD dwElasped;
	char szQueryName[QUERYNAMESIZE];
};

struct TGameDelayedReport
{
	WCHAR  wszLog[GAMEDELAYSIZE];
};

struct TQueryErrorReport
{
	USHORT	unLogType;
	USHORT	unWorldSetID;
	UINT	uiAccountDBID;
	INT64	biCharDBID;
	UINT	uiSessionID;
	USHORT	unServerType;
	USHORT	unLen;
	WCHAR	wszBuf[800];
};

struct TVillageDelayedReport
{
	WCHAR  wszLog[GAMEDELAYSIZE];
};

struct TNetLauncherInfo
{
	int nNID;
	WCHAR wszIP[IPLENMAX];
	WCHAR wszPublicIP[IPLENMAX];
};

struct TApplyWorldPvPRoom
{
	int nRetCode;
};

struct TCreateGambleRoom
{
	int nRoomIndex;	
	bool bRegulation;
	int nGambleType;
	int nPrice;
};

struct TStopGambleRoom
{
	int nRetCode;
};

struct TDelGambleRoom
{
	int nRetCode;
};

struct TUpdateDWCStatus
{
	int nRetCode;
};

//-------------------------------------------------
//	ServiceManager <-> Patcher
//-------------------------------------------------
struct TPatcherResult				//PATCHERSERVICE_RESULT
{
	WCHAR szBaseUrl[MAX_PATH];
	WCHAR szPatchUrl[MAX_PATH];
};

//-------------------------------------------------
//	ServiceManager <-> Monitor
//-------------------------------------------------
struct TSMCheckLogin				//MONITOR2MANAGER_CHECK_LOGIN
{
	TP_SMVERSION m_Version;
	char szID[IDLENMAX];
	char szPass[PASSWORDLENMAX];
};

struct TSMServerStructReq			// MONITOR2MANAGER_SERVERSTRUCT_REQ
{
	int nContainerVersion;
};

struct TSMRealTimeReq				// MONITOR2MANAGER_REALTIME_REQ
{
	int nContainetVersion;
};

struct TSMMeritInfoListReq			//MONITOR2MANAGER_MERITSTRUCT_REQ
{
	int nContainerVersion;
	int nLastRecvMeritID;
};

struct TSMNetLauncherInfoListReq	//MONITOR2MANAGER_NETLAUNCHERSTRUCT_REQ
{
	int nContainerVersion;
	int nLastRecvNLID;
};

struct TSMServerInfoListReq			//MONITOR2MANAGER_SERVERSTRUCTDATA_REQ
{
	int nContainerVersion;
	int nLastRecvSID;
};

struct TSMChannelInfoListReq			//MONITOR2MANAGER_CHANNELSTRUCTDATA_REQ
{
	int nContainerVersion;
	int nSID;
	int nChannelID;
};

struct TSMLoginUserCountReq			//MONITOR2MANAGER_LOGINSERCOUNT_REQ
{
	int nLastRecvLoginSID;
};

struct TSMVillageUserCountReq			//MONITOR2MANAGER_VILLAGESERCOUNT_REQ
{
	INT64 uiLastRecvCHID;
};

struct TSMGameUserCountReq			//MONITOR2MANAGER_GAMESERCOUNT_REQ
{
	int nLastRecvGameSID;
};

struct TSMServerStateReq			//MONITOR2MANAGER_SERVERSTATE_REQ
{
	int nLastRecvSID;
};

struct TSMWorldInfoReq				//MONITOR2MANAGER_WORLDINFO_REQ
{
	int nContainetVersion;
};

struct TSMServiceStart				//MONITOR2MANAGER_STARTSERVICE
{
	int nStartType;			//eServiceStartType
	BYTE cCount;
	int nIDs[SERVERCONTROLMAX];				//Need Value if nStartType "_SERVICE_START_EACH" & "_SERVICE_START_EACH_BY_LAUNCHERID"
};

struct TSMServiceStop				//MONITOR2MANAGER_STOPSERVICE
{
	int nStopType;			//eServiceStopType
	BYTE cCount;
	int nIDs[SERVERCONTROLMAX];				//Need Value if nStopType "_SERVICE_STOP_EACH" & "_SERVICE_STOP_EACH_FORCE"
};

struct TSMMakeDump					//MONITOR2MANAGER_MAKEDUMP
{
	BYTE cCount;
	int nSID[SERVERCONTROLMAX];
};

struct sChannelControl
{
	int nWorldID;
	int nChannelID;
};

struct TSMChannelControl			//MONITOR2MANAGER_CHANNELCONTROL
{
	bool bVisibility;
	BYTE cCount;
	sChannelControl Control[SERVERCONTROLMAX];
};

struct TSMGameControl				//MONITOR2MANAGER_GAMECONTROL
{
	bool bClose;
	BYTE cCount;
	int nSID[SERVERCONTROLMAX];
};

struct TSMReportReq // MONITOR2MANAGER_REPORT_REQ
{
	bool bBuild;
	BYTE cDays;
	int nPage;
};

struct TChangePassword				// MONITOR2MANAGER_CHANGEPASSWORD
{
	char szUserName[NAMELENMAX];
	char szPassWord[PASSWORDLENMAX];
	int nMonitorLevel;
};

///////////////////////////////////////////////////
struct TMSLoginResult		//MANAGER2MONITOR_LOGIN_RESULT
{
	int nRetCode;
	int nMonitorLevel;		//eServerMonitorLevel 참조
	int nContainerVersion;
};

struct sMonitorNetLauncherInfo
{
	int nID;	
	WCHAR wszIP[IPLENMAX];
	WCHAR wszPublicIP[IPLENMAX];
};

struct TMSNetLauncherList	//MANAGER2MONITOR_NETLAUNCHERLIST
{
	int nWholeCount;
	int nLastReqNLID;
	int nContainerVersion;
	BYTE cIsLast;
	int nRet;
	short nCount;
	sMonitorNetLauncherInfo Info[NETLAUNCHERLISTMAX];
};

struct sMonitorServerInfo
{
	int nSID;
	int nManagedLauncherID;
	int nServerState;	//eServiceState
	int nServerType;	//eManagedType
	char szType[NAMELENMAX];
	char szResourceRevision[32];
	char szExeVersion[SERVERVERSIONMAX];
	BYTE cWorldIDs[WORLDCOUNTMAX];
};

struct TMonitorServerInfoList		//MANAGER2MONITOR_SERVERINFO_LIST
{
	int nWholeCount;
	int nLastReqSID;
	int nContainerVersion;
	BYTE cIsLast;
	int nRet;
	short nCount;
	sMonitorServerInfo Info[SERVERINFOCOUNTMAX];
};

struct sMonitorChannelMeritInfo
{
	int nID;
	int nMinLevel;
	int nMaxLevel;
	int nMeritType;
	int nExtendValue;
};

struct TMSMeritList			//MANAGER2MONITOR_MERITLIST
{
	int nWholeCount;		//전체카운트
	int nLastReqMeritID;
	int nContainerVersion;
	BYTE cIsLast;
	int nRet;
	short nCount;
	sMonitorChannelMeritInfo Info[MERITINFOMAX];
};

struct sMonitorChannelInfo
{
	int nSID;
	int nWorldID;
	UINT nChannelID;
	UINT nMapIdx;
	USHORT nMaxUserCount;
	int nChannelAttribute;
	WCHAR wszMapName[MAPNAMELENMAX];
};

struct TMSMonitorChannelList		//MANAGER2MONITOR_CHANNELINFO_LIST
{
	int nWholeCount;
	INT64 nLastReqCHID;
	int nContainerVersion;
	BYTE cIsLast;
	int nRet;
	short nCount;
	sMonitorChannelInfo Info[VILLAGECHANNELMAX];
};

struct TMSUpdateNotice				//MANAGER2MONITOR_UPDATE_NOTICE
{
	int nNoticeType;				//eServerMonitorNotice
	int nContainerVersion;
};

struct sServerStateInfo
{
	int nSID;
	int nServerState;				//eServiceState
};

struct TMSServerStateUpdate : public sServerStateInfo	//MANAGER2MONITOR_UPDATE_SERVERSTATE
{
	int nServerException;			//eExceptionRepert
	char szDetail[128];				//reserve
};

struct TMSServerDealyReq // MANAGER2MONITOR_SERVER_DEALYINFO_REQ
{
	int nType;
	int nSID;
};
struct TMSServerDealyInfo	// MANAGER2MONITOR_SERVER_DEALYINFO
{
	int nType;
	int nCount;
	int nMIDs[SERVICEMANAGER_SERVERDEALY_SIZE];
};

struct sMonitorWorldInfo
{
	int nWorldID;
	int nWorldMaxUser;
	WCHAR wszWorldName[WORLDNAMELENMAX];
};

struct TMSWorldInfo				//MANAGER2MONITOR_WORLDINFO_RESULT
{
	int nRet;
	BYTE cCount;
	sMonitorWorldInfo WorldInfo[WORLDCOUNTMAX];
};

struct sUserCountInfo
{
	int nSID;
	int nUserCount;
	short nRoomCount;
};

struct TMSLoginUserCount			//MANAGER2MONITOR_LOGIN_UPDATEINFO
{
	int nWholeCount;
	int nLastReqLoginSID;
	BYTE cIsLast;
	short nCount;
	sUserCountInfo Update[LOGINCOUNTMAX];
};

struct sChannelUpdateInfo : public TVillageUserReport
{
	int nSID;
	short nMeritBonusID;
	int nLimitLevel;
	bool bVisibility;
	bool bShow;
	int nServerID;
	CHAR cThreadID;
};

struct sVillageUserInfo : public sUserCountInfo
{
	BYTE cChannelCount;
	sChannelUpdateInfo Info[VILLAGECHANNELMAX];
};

struct TMSVillageUserCount			//MANAGER2MONITOR_VILLAGE_UPDATEINFO
{
	int nWholeCount;
	INT64 uiLastReqCHID;
	BYTE cIsLast;
	BYTE cCount;
	sChannelUpdateInfo Update[VILLAGECHANNELMAX];
};

struct sGameUserInfo : public sUserCountInfo
{
	BYTE cIsOpened;
	int nTotalRoomCount;
};

struct TMSGameUserCount				//MANAGER2MONITOR_GAME_UPDATEINFO
{
	int nWholeCount;
	int nLastReqGameSID;
	BYTE cIsLast;
	short nCount;
	sGameUserInfo Update[GAMECOUNTMAX];
};

struct TMSFarmUserCount				// MANAGER2MONITOR_FARM_UPDATEINFO
{
	short nCount;
	TFarmStatus Update[Farm::Max::FARMCOUNT];
};

struct TMSServerStateList			//MANAGER2MONITOR_SERVERSTATE_LIST
{
	int nWholeCount;
	int nLastReqSID;
	BYTE cIsLast;
	short nCount;
	sServerStateInfo State[STATECOUNTMAX];
};

struct TMSOperatingResult			//MANAGER2MONITOR_OPERATING_RESULT
{
	int nCmd;		//요청하신 명령
	int nRet;		//에 대한 리턴밸류
};

struct TMSReportReslut
{
	int nPage;
	int nMaxPage;
	bool bEnd;
	wchar_t szReport[SERVICEMANAGER_REPORT_SIZE + 1];
};

//-------------------------------------------------
//	Log Packet
//-------------------------------------------------

const int LOGPARAMSIZEMAX = 1024;
struct TLog
{
	UINT nAccountDBID;
	INT64 biCharacterDBID;
	UINT nSessionID;
	char cWorldID;
	int nLogType;
	short wTotalParamCount;
	short wParamSize;
	char szParam[LOGPARAMSIZEMAX];
};

struct TLogFile
{
	USHORT	unLogType;
	USHORT	unWorldSetID;
	int		nServerID;
	UINT	uiAccountDBID;
	INT64	biCharDBID;
	UINT	uiSessionID;
	USHORT	unServerType;
	USHORT	unLen;
	WCHAR	wszBuf[1024];
};

//-------------------------------------------------
//	Auth Packet (Nexon)
//-------------------------------------------------
struct TAuthHeader
{
	BYTE cHeader;		// 0xAA 고정
	USHORT wLength;		// 65536까지
};


#pragma pack(pop)

