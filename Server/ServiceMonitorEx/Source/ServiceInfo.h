

#pragma once

#include "Define.h"
#include "CriticalSection.hpp"
#include "DNServerDef.h"

#include <map>
#include <vector>

// WORLD

typedef struct WORLDINFO
{

public:
	WORLDINFO(INT nWorldID);
	WORLDINFO(const WORLDINFO& pWorldInfo);
	//	virtual ~WORLDINFO() { }

	VOID Set(INT nWorldID);						// 불변 지정
	VOID Set(const WORLDINFO* pWorldInfo);				// 전체 지정
	VOID SetChangeable(const WORLDINFO* pWorldInfo);	// 가변 지정
	VOID Reset();
	BOOL IsSet() const { return(EV_WORLDID_DEF != m_WorldID); }

public:
	// Common (불변)
	INT m_WorldID;

	// Common (가변)
	INT m_MaxUserLimit;
	WCHAR m_WorldName[WORLDNAMELENMAX];

} *LPWORLDINFO, * const LPCWORLDINFO;


// SERVERINFO

typedef struct SERVERINFO
{

public:
	SERVERINFO(
		EF_SERVERTYPE pServerType, 
		INT pServerID, 
		INT pNetLuncherID,
		LPCTSTR pResourceRevision,
		LPCTSTR pExeVersion,
		INT nWorldID[WORLDCOUNTMAX], 
		UINT pIpAddr,
		UINT pPublicIP
		);
	SERVERINFO(const SERVERINFO& pServerInfo);
	//	virtual ~SERVERINFO() { }

	VOID Set(											// 불변 지정
		EF_SERVERTYPE pServerType, 
		INT pServerID, 
		INT pNetLuncherID,
		LPCTSTR pResourceRevision,
		LPCTSTR pExeVersion,
		INT nWorldID[WORLDCOUNTMAX], 
		UINT pIpAddr,
		UINT pPublicIP
		);
	VOID Set(const SERVERINFO* pServerInfo);			// 전체 지정
	VOID SetChangeable(const SERVERINFO* pServerInfo);	// 가변 지정
	virtual VOID Reset(BOOL pIsAll = TRUE);
	BOOL IsSet() const { return(EV_SVS_NONE != m_ServerType); }

	BOOL FindWorldID(INT nWorldID) const;

public:
	// Common (불변)
	EF_SERVERTYPE m_ServerType;						// 서버 타입
	INT m_ServerID;							// 서버 ID
	INT m_NetLuncherID;					// 넷런처 ID
	TCHAR m_ResourceRevision[EV_RSCSVNRVS_MAXLEN];	// 서버가 로드한 리소스의 SVN 리비전 번호
	TCHAR m_ExeVersion[EV_SRVVER_MAXLEN];			// 서버의 실행파일 버전 (CCNet이할당하는 버전)	(1, 2 번 자리 위주로 검사. 마지막 자리는 RB 패치 후 개별빌드일 때 증가)
	INT m_WorldID[WORLDCOUNTMAX];			// 월드 ID (목록)
	UINT m_IpAddr;								// IP 주소
	UINT m_PublicIP;							// Public IP 주소

	// Specific (불변)
	
	// Common (가변)
	EF_SERVERSTATE m_ServerState;	// 서버 상태

	// Specific (가변)
	INT m_CurUserCount;	// 현재 인원 수 (LO, GA, VI)
	INT m_CurRoomCount;	// 현재 방 개수 (GA)
	INT m_MaxRoomCount;	// 전체 방 개수 (GA)
	INT m_TotalFarmUserCount;
	
	INT m_MaxFarmCount;	
	UINT m_FarmDBID[Farm::Max::FARMCOUNT];
	INT m_CurFarmUserCount[Farm::Max::FARMCOUNT];
	bool m_bActivate[Farm::Max::FARMCOUNT];
	INT m_FrameDelay[SERVERTYPE_MAX];	
	INT m_MeritID;			// 메리트 ID (GA)
	BOOL m_Visibility;		// 개방 여부 (GA)	

} *LPSERVERINFO, * const LPCSERVERINFO;


// CHANNEL (VI)

typedef struct CHANNELINFO
{

public:
	CHANNELINFO(
		INT pServerID, 
		INT pChannelID, 
		INT nWorldID, 
		INT pMapIndex, 
		INT pMaxUserCount, 
		INT pAttribute, 
		const WCHAR* wszMapName
		);
	CHANNELINFO(const CHANNELINFO& pChannelInfo);
	//	virtual ~CHANNELINFO() { }

	VOID Set(													// 불변 지정
		INT pServerID, 
		INT pChannelID,
		INT nWorldID,
		INT pMapIndex,
		INT pMaxUserCount,
		INT pAttribute, 
		const WCHAR* wszMapName
		);

	VOID Set(const CHANNELINFO* pChannelInfo);					// 전체 지정
	VOID SetChangeable(const CHANNELINFO* pChannelInfo);		// 가변 지정
	VOID Reset();
	BOOL IsSet() const { return(EV_CHANNELID_DEF != m_ChannelID); }

public:
	// Common (불변)
	INT m_ServerID;			// VI
	INT m_ChannelID;		// 채널 ID
	INT m_WorldID;			// 월드 ID
	INT m_MapIndex;			// 맵 인덱스
	INT m_MaxUserCount;	// 최대 인원 수
	INT m_Attribute;		// 채널 속성

	// Common (가변)
	INT m_CurUserCount;	// 현재 인원 수
	INT m_MeritID;			// 메리트 ID
	BOOL m_Visibility;		// 개방 여부
	INT m_LimitLevel;			// 입장 제한 레벨
	BOOL m_IsMidShow;				// DB 미드웨어 연결여부
	INT m_nServerID;
	CHAR m_cThreadID;
	WCHAR m_wszMapName[MAPNAMELENMAX];	// 맵이름

} *LPCHANNELINFO, * const LPCCHANNELINFO;


// MERIT (GA, VI)

typedef struct MERITINFO
{

public:
	MERITINFO(INT pMeritID, INT pMinLevel, INT pMaxLevel, INT pMeritType, INT pExtendValue);
	MERITINFO(const MERITINFO& pMeritInfo);

	VOID Set(											// 불변 지정
		INT pMeritID, 
		INT pMinLevel, 
		INT pMaxLevel, 
		INT pMeritType, 
		INT pExtendValue
		);
	VOID Set(const MERITINFO* pMeritInfo);				// 전체 지정
	VOID SetChangeable(const MERITINFO* pMeritInfo);	// 가변 지정
	VOID Reset();
	BOOL IsSet() const { return(EV_MERITID_DEF != m_MeritID); }

public:
	// Common (불변)
	INT m_MeritID;		// 메리트 ID
	INT m_MinLevel;		// 메리트 적용가능 최소레벨
	INT m_MaxLevel;		// 메리트 적용가능 최대레벨
	INT m_MeritType;	// 메리트 타입 (eMeritBonus 참조)
	INT m_ExtendValue;	// 추가 수치 (메리트 타입에 따라 다른 의미)

	// Common (가변)


} *LPMERITINFO, * const LPCMERITINFO;


// NET LUNCHER

typedef struct NETLUNCHERINFO
{

public:
	NETLUNCHERINFO(INT pNetLuncherID, UINT pIpAddr, UINT pPublicIP);
	VOID Set(INT pNetLuncherID, UINT pIpAddr, UINT pPublicIP);	// 불변 지정
	
	NETLUNCHERINFO(const NETLUNCHERINFO& pNetLuncherInfo);
	
	VOID Reset();
	BOOL IsSet() const { return(EV_NETLUNCHER_DEF != m_NetLuncherID); }

public:
	// Common (불변)
	INT m_NetLuncherID;		// 넷런처 ID
	UINT m_IpAddr;					// IP 주소
	UINT m_PublicIP;


} *LPNETLUNCHERINFO, * const LPCNETLUNCHERINFO;


class CServiceInfo
{
public:
	CServiceInfo ();
	virtual ~CServiceInfo ();


public:
	typedef	CCriticalSection		TP_LOCK;
	typedef	CLockAutoEx<TP_LOCK>	TP_LOCKAUTO;

	// WORLD
	typedef	std::map<INT, WORLDINFO>					TP_LISTWRLD;
	typedef	TP_LISTWRLD::iterator						TP_LISTWRLD_ITR;
	typedef	TP_LISTWRLD::const_iterator					TP_LISTWRLD_CTR;
	typedef	std::vector<WORLDINFO>						TP_LISTWRLDAUTO;
	typedef	TP_LISTWRLDAUTO::iterator					TP_LISTWRLDAUTO_ITR;
	typedef	TP_LISTWRLDAUTO::const_iterator				TP_LISTWRLDAUTO_CTR;

	// SERVER
	typedef	std::map<INT, SERVERINFO>					TP_LISTSERV;
	typedef	TP_LISTSERV::iterator						TP_LISTSERV_ITR;
	typedef	TP_LISTSERV::const_iterator					TP_LISTSERV_CTR;
	typedef	std::vector<SERVERINFO>						TP_LISTSERVAUTO;
	typedef	TP_LISTSERVAUTO::iterator					TP_LISTSERVAUTO_ITR;
	typedef	TP_LISTSERVAUTO::const_iterator				TP_LISTSERVAUTO_CTR;

	// CHANNEL (VI)
	typedef	std::map<DWORD64, CHANNELINFO>				TP_LISTCHNL;
	typedef	TP_LISTCHNL::iterator						TP_LISTCHNL_ITR;
	typedef	TP_LISTCHNL::const_iterator					TP_LISTCHNL_CTR;
	typedef	std::vector<CHANNELINFO>					TP_LISTCHNLAUTO;
	typedef	TP_LISTCHNLAUTO::iterator					TP_LISTCHNLAUTO_ITR;
	typedef	TP_LISTCHNLAUTO::const_iterator				TP_LISTCHNLAUTO_CTR;

	// MERIT (GA, VI)
	typedef	std::map<INT, MERITINFO>					TP_LISTMRIT;
	typedef	TP_LISTMRIT::iterator						TP_LISTMRIT_ITR;
	typedef	TP_LISTMRIT::const_iterator					TP_LISTMRIT_CTR;
	typedef	std::vector<MERITINFO>						TP_LISTMRITAUTO;
	typedef	TP_LISTMRITAUTO::iterator					TP_LISTMRITAUTO_ITR;
	typedef	TP_LISTMRITAUTO::const_iterator				TP_LISTMRITAUTO_CTR;

	// NET LUNCHER
	typedef	std::map<INT, NETLUNCHERINFO>				TP_LISTNTLC;
	typedef	TP_LISTNTLC::iterator						TP_LISTNTLC_ITR;
	typedef	TP_LISTNTLC::const_iterator					TP_LISTNTLC_CTR;
	typedef	std::vector<NETLUNCHERINFO>					TP_LISTNTLCAUTO;
	typedef	TP_LISTNTLCAUTO::iterator					TP_LISTNTLCAUTO_ITR;
	typedef	TP_LISTNTLCAUTO::const_iterator				TP_LISTNTLCAUTO_CTR;

public:
	DWORD Open();
	VOID Close();
	BOOL IsOpen() const { return(m_IsOpen); }

	INT GetTotalUserCount() const { return m_TotalUserCount; }
	void SetTotalUserCount(INT pTotalUserCount) { m_TotalUserCount = pTotalUserCount; }

	// WORLD
	BOOL IsWorldEmpty(EF_SERVICEMNGRTYPE pType) const { return(m_ListWrld[pType].empty()); }		// 동기화 않해도 무방 ?
	WORLDINFO* AtWorld(EF_SERVICEMNGRTYPE pType, INT nWorldID, BOOL pDoLock = FALSE);
	const WORLDINFO* AtWorld(EF_SERVICEMNGRTYPE pType, INT nWorldID, BOOL pDoLock = FALSE) const;
	DWORD AddWorld(EF_SERVICEMNGRTYPE pType, INT nWorldID, BOOL pDoLock = FALSE);
	DWORD AddWorld(EF_SERVICEMNGRTYPE pType, const WORLDINFO* pWorldInfo, BOOL pDoLock = FALSE);
	VOID GetWorldList(EF_SERVICEMNGRTYPE pType, TP_LISTWRLDAUTO& pList, BOOL pDoLock = FALSE) const;
	VOID ClearWorldList(EF_SERVICEMNGRTYPE pType, BOOL pDoLock = FALSE);
	TP_LISTWRLD_ITR BeginWorld(EF_SERVICEMNGRTYPE pType, BOOL pDoLock = FALSE);
	TP_LISTWRLD_CTR BeginWorld(EF_SERVICEMNGRTYPE pType, BOOL pDoLock = FALSE) const;
	TP_LISTWRLD_ITR EndWorld(EF_SERVICEMNGRTYPE pType, BOOL pDoLock = FALSE);
	TP_LISTWRLD_CTR EndWorld(EF_SERVICEMNGRTYPE pType, BOOL pDoLock = FALSE) const;

	// SERVER
	BOOL IsServerEmpty(EF_SERVICEMNGRTYPE pType) const { return(m_ListServ[pType].empty()); }		// 동기화 않해도 무방 ?
	SERVERINFO* AtServer(EF_SERVICEMNGRTYPE pType, INT pServerID, BOOL pDoLock = FALSE);
	const SERVERINFO* AtServer(EF_SERVICEMNGRTYPE pType, INT pServerID, BOOL pDoLock = FALSE) const;
	DWORD AddServer(EF_SERVICEMNGRTYPE pType, const SERVERINFO* pServerInfo, BOOL pDoLock = FALSE);
	VOID GetServerList(EF_SERVICEMNGRTYPE pType, TP_LISTSERVAUTO& pList, INT nWorldID = EV_WORLDID_ALL, BOOL pDoLock = FALSE) const;
	VOID ClearServerList(EF_SERVICEMNGRTYPE pType, BOOL pDoLock = FALSE);
	TP_LISTSERV_ITR BeginServer(EF_SERVICEMNGRTYPE pType, BOOL pDoLock = FALSE);
	TP_LISTSERV_CTR BeginServer(EF_SERVICEMNGRTYPE pType, BOOL pDoLock = FALSE) const;
	TP_LISTSERV_ITR EndServer(EF_SERVICEMNGRTYPE pType, BOOL pDoLock = FALSE);
	TP_LISTSERV_CTR EndServer(EF_SERVICEMNGRTYPE pType, BOOL pDoLock = FALSE) const;

	// CHANNEL (VI)
	BOOL IsChannelEmpty(EF_SERVICEMNGRTYPE pType) const { return(m_ListChnl[pType].empty()); }	// 동기화 않해도 무방 ?
	CHANNELINFO* AtChannel(EF_SERVICEMNGRTYPE pType, INT pServerID, INT pChannelID, BOOL pDoLock = FALSE);
	const CHANNELINFO* AtChannel(EF_SERVICEMNGRTYPE pType, INT pServerID, INT pChannelID, BOOL pDoLock = FALSE) const;
	DWORD AddChannel(EF_SERVICEMNGRTYPE pType, INT pServerID, INT pChannelID, INT nWorldID, INT pMapIndex, INT pMaxUserCount, INT pAttribute, const WCHAR* wszMapName, BOOL pDoLock = FALSE);
	DWORD AddChannel(EF_SERVICEMNGRTYPE pType, const CHANNELINFO* pChannelInfo, BOOL pDoLock = FALSE);
	VOID GetChannelList(EF_SERVICEMNGRTYPE pType, TP_LISTCHNLAUTO& pList, INT pServerID = EV_SERVERID_ALL, BOOL pDoLock = FALSE) const;
	VOID ClearChannelList(EF_SERVICEMNGRTYPE pType, BOOL pDoLock = FALSE);
	TP_LISTCHNL_ITR BeginChannel(EF_SERVICEMNGRTYPE pType, BOOL pDoLock = FALSE);
	TP_LISTCHNL_CTR BeginChannel(EF_SERVICEMNGRTYPE pType, BOOL pDoLock = FALSE) const;
	TP_LISTCHNL_ITR EndChannel(EF_SERVICEMNGRTYPE pType, BOOL pDoLock = FALSE);
	TP_LISTCHNL_CTR EndChannel(EF_SERVICEMNGRTYPE pType, BOOL pDoLock = FALSE) const;

	// MERIT (GA, VI)
	BOOL IsMeritEmpty(EF_SERVICEMNGRTYPE pType) const { return(m_ListMrit[pType].empty()); }	// 동기화 않해도 무방 ?
	MERITINFO* AtMerit(EF_SERVICEMNGRTYPE pType, INT pMeritID, BOOL pDoLock = FALSE);
	const MERITINFO* AtMerit(EF_SERVICEMNGRTYPE pType, INT pMeritID, BOOL pDoLock = FALSE) const;
	DWORD AddMerit(EF_SERVICEMNGRTYPE pType, INT pMeritID, INT pMinLevel, INT pMaxLevel, INT pMeritType, INT pExtendValue, BOOL pDoLock = FALSE);
	DWORD AddMerit(EF_SERVICEMNGRTYPE pType, const MERITINFO* pMeritInfo, BOOL pDoLock = FALSE);
	VOID GetMeritList(EF_SERVICEMNGRTYPE pType, TP_LISTMRITAUTO& pList, BOOL pDoLock = FALSE) const;
	VOID ClearMeritList(EF_SERVICEMNGRTYPE pType, BOOL pDoLock = FALSE);
	TP_LISTMRIT_ITR BeginMerit(EF_SERVICEMNGRTYPE pType, BOOL pDoLock = FALSE);
	TP_LISTMRIT_CTR BeginMerit(EF_SERVICEMNGRTYPE pType, BOOL pDoLock = FALSE) const;
	TP_LISTMRIT_ITR EndMerit(EF_SERVICEMNGRTYPE pType, BOOL pDoLock = FALSE);
	TP_LISTMRIT_CTR EndMerit(EF_SERVICEMNGRTYPE pType, BOOL pDoLock = FALSE) const;

	// NET LUNCHER
	BOOL IsNetLuncherEmpty(EF_SERVICEMNGRTYPE pType) const { return(m_ListNtlc[pType].empty()); }	// 동기화 않해도 무방 ?
	NETLUNCHERINFO* AtNetLuncher(EF_SERVICEMNGRTYPE pType, INT pNetLuncherID, BOOL pDoLock = FALSE);
	const NETLUNCHERINFO* AtNetLuncher(EF_SERVICEMNGRTYPE pType, INT pNetLuncherID, BOOL pDoLock = FALSE) const;
	DWORD AddNetLuncher(EF_SERVICEMNGRTYPE pType, const NETLUNCHERINFO* pNetLuncherInfo, BOOL pDoLock = FALSE);
	VOID GetNetLuncherList(EF_SERVICEMNGRTYPE pType, TP_LISTNTLCAUTO& pList, BOOL pDoLock = FALSE) const;
	VOID ClearNetLuncherList(EF_SERVICEMNGRTYPE pType, BOOL pDoLock = FALSE);
	TP_LISTNTLC_ITR BeginNetLuncher(EF_SERVICEMNGRTYPE pType, BOOL pDoLock = FALSE);
	TP_LISTNTLC_CTR BeginNetLuncher(EF_SERVICEMNGRTYPE pType, BOOL pDoLock = FALSE) const;
	TP_LISTNTLC_ITR EndNetLuncher(EF_SERVICEMNGRTYPE pType, BOOL pDoLock = FALSE);
	TP_LISTNTLC_CTR EndNetLuncher(EF_SERVICEMNGRTYPE pType, BOOL pDoLock = FALSE) const;

	BOOL GetRecvMode(EF_SERVICERECVTYPE pType) const;
	VOID SetRecvMode(EF_SERVICERECVTYPE pType, BOOL pRecvMode);
	VOID ResetAllRecvMode();

	BOOL IsRecvTick(EF_SERVICERECVTYPE pType) const;
	VOID SetRecvTick(EF_SERVICERECVTYPE pType);
	DWORD GetRecvTickTerm(EF_SERVICERECVTYPE pType) const;

	VOID ClearAllList(EF_SERVICEMNGRTYPE pType, BOOL pDoLock = FALSE);
	VOID ApplyListTempToReal(BOOL pDoLock);		// 임시 목록을 실제 목록에 일괄 적용

	TP_LOCK* GetLock() const { return(&m_Lock); }
#if defined(_DEBUG)
	BOOL IsLock() const { return(m_Lock.IsLock()); }
#endif	// _DEBUG

	LONG64 GetServiceInfoUpdateNo() const { return m_ServiceInfoUpdateNo; }
	LONG64 IncServiceInfoUpdateNo() { return(++m_ServiceInfoUpdateNo); }

	INT GetContainerVersion() const { return m_ContainerVersion; }
	VOID SetContainerVersion(INT pContainerVersion) { m_ContainerVersion = pContainerVersion; }

	void ResetDelayCount(EF_SERVICEMNGRTYPE pType);
	void ResetFarmInfo(EF_SERVICEMNGRTYPE pType);
	
	VOID SetFarmInfoResetFlag(EF_SERVICEMNGRTYPE pType, BOOL bIsReset){ m_IsFarmInfoReset[pType] = bIsReset; }
	BOOL IsFarmInfoReset(EF_SERVICEMNGRTYPE pType){ return m_IsFarmInfoReset[pType]; }
private:
	DWORD GetTickTerm(DWORD pOldTick, DWORD pCurTick) const;

private:
	BOOL m_IsOpen;
	BOOL m_IsFarmInfoReset[EV_SMT_CNT];

	TP_LISTWRLD m_ListWrld[EV_SMT_CNT];
	TP_LISTSERV m_ListServ[EV_SMT_CNT];
	TP_LISTCHNL m_ListChnl[EV_SMT_CNT];
	TP_LISTMRIT m_ListMrit[EV_SMT_CNT];
	TP_LISTNTLC m_ListNtlc[EV_SMT_CNT];

	INT m_TotalUserCount;

	BOOL m_RecvMode[EV_SRT_CNT];
	DWORD m_RecvTick[EV_SRT_CNT];

	mutable TP_LOCK m_Lock;
	LONG64 m_ServiceInfoUpdateNo;
	INT m_ContainerVersion;
};