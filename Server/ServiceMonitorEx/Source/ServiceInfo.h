

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

	VOID Set(INT nWorldID);						// �Һ� ����
	VOID Set(const WORLDINFO* pWorldInfo);				// ��ü ����
	VOID SetChangeable(const WORLDINFO* pWorldInfo);	// ���� ����
	VOID Reset();
	BOOL IsSet() const { return(EV_WORLDID_DEF != m_WorldID); }

public:
	// Common (�Һ�)
	INT m_WorldID;

	// Common (����)
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

	VOID Set(											// �Һ� ����
		EF_SERVERTYPE pServerType, 
		INT pServerID, 
		INT pNetLuncherID,
		LPCTSTR pResourceRevision,
		LPCTSTR pExeVersion,
		INT nWorldID[WORLDCOUNTMAX], 
		UINT pIpAddr,
		UINT pPublicIP
		);
	VOID Set(const SERVERINFO* pServerInfo);			// ��ü ����
	VOID SetChangeable(const SERVERINFO* pServerInfo);	// ���� ����
	virtual VOID Reset(BOOL pIsAll = TRUE);
	BOOL IsSet() const { return(EV_SVS_NONE != m_ServerType); }

	BOOL FindWorldID(INT nWorldID) const;

public:
	// Common (�Һ�)
	EF_SERVERTYPE m_ServerType;						// ���� Ÿ��
	INT m_ServerID;							// ���� ID
	INT m_NetLuncherID;					// �ݷ�ó ID
	TCHAR m_ResourceRevision[EV_RSCSVNRVS_MAXLEN];	// ������ �ε��� ���ҽ��� SVN ������ ��ȣ
	TCHAR m_ExeVersion[EV_SRVVER_MAXLEN];			// ������ �������� ���� (CCNet���Ҵ��ϴ� ����)	(1, 2 �� �ڸ� ���ַ� �˻�. ������ �ڸ��� RB ��ġ �� ���������� �� ����)
	INT m_WorldID[WORLDCOUNTMAX];			// ���� ID (���)
	UINT m_IpAddr;								// IP �ּ�
	UINT m_PublicIP;							// Public IP �ּ�

	// Specific (�Һ�)
	
	// Common (����)
	EF_SERVERSTATE m_ServerState;	// ���� ����

	// Specific (����)
	INT m_CurUserCount;	// ���� �ο� �� (LO, GA, VI)
	INT m_CurRoomCount;	// ���� �� ���� (GA)
	INT m_MaxRoomCount;	// ��ü �� ���� (GA)
	INT m_TotalFarmUserCount;
	
	INT m_MaxFarmCount;	
	UINT m_FarmDBID[Farm::Max::FARMCOUNT];
	INT m_CurFarmUserCount[Farm::Max::FARMCOUNT];
	bool m_bActivate[Farm::Max::FARMCOUNT];
	INT m_FrameDelay[SERVERTYPE_MAX];	
	INT m_MeritID;			// �޸�Ʈ ID (GA)
	BOOL m_Visibility;		// ���� ���� (GA)	

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

	VOID Set(													// �Һ� ����
		INT pServerID, 
		INT pChannelID,
		INT nWorldID,
		INT pMapIndex,
		INT pMaxUserCount,
		INT pAttribute, 
		const WCHAR* wszMapName
		);

	VOID Set(const CHANNELINFO* pChannelInfo);					// ��ü ����
	VOID SetChangeable(const CHANNELINFO* pChannelInfo);		// ���� ����
	VOID Reset();
	BOOL IsSet() const { return(EV_CHANNELID_DEF != m_ChannelID); }

public:
	// Common (�Һ�)
	INT m_ServerID;			// VI
	INT m_ChannelID;		// ä�� ID
	INT m_WorldID;			// ���� ID
	INT m_MapIndex;			// �� �ε���
	INT m_MaxUserCount;	// �ִ� �ο� ��
	INT m_Attribute;		// ä�� �Ӽ�

	// Common (����)
	INT m_CurUserCount;	// ���� �ο� ��
	INT m_MeritID;			// �޸�Ʈ ID
	BOOL m_Visibility;		// ���� ����
	INT m_LimitLevel;			// ���� ���� ����
	BOOL m_IsMidShow;				// DB �̵���� ���Ῡ��
	INT m_nServerID;
	CHAR m_cThreadID;
	WCHAR m_wszMapName[MAPNAMELENMAX];	// ���̸�

} *LPCHANNELINFO, * const LPCCHANNELINFO;


// MERIT (GA, VI)

typedef struct MERITINFO
{

public:
	MERITINFO(INT pMeritID, INT pMinLevel, INT pMaxLevel, INT pMeritType, INT pExtendValue);
	MERITINFO(const MERITINFO& pMeritInfo);

	VOID Set(											// �Һ� ����
		INT pMeritID, 
		INT pMinLevel, 
		INT pMaxLevel, 
		INT pMeritType, 
		INT pExtendValue
		);
	VOID Set(const MERITINFO* pMeritInfo);				// ��ü ����
	VOID SetChangeable(const MERITINFO* pMeritInfo);	// ���� ����
	VOID Reset();
	BOOL IsSet() const { return(EV_MERITID_DEF != m_MeritID); }

public:
	// Common (�Һ�)
	INT m_MeritID;		// �޸�Ʈ ID
	INT m_MinLevel;		// �޸�Ʈ ���밡�� �ּҷ���
	INT m_MaxLevel;		// �޸�Ʈ ���밡�� �ִ뷹��
	INT m_MeritType;	// �޸�Ʈ Ÿ�� (eMeritBonus ����)
	INT m_ExtendValue;	// �߰� ��ġ (�޸�Ʈ Ÿ�Կ� ���� �ٸ� �ǹ�)

	// Common (����)


} *LPMERITINFO, * const LPCMERITINFO;


// NET LUNCHER

typedef struct NETLUNCHERINFO
{

public:
	NETLUNCHERINFO(INT pNetLuncherID, UINT pIpAddr, UINT pPublicIP);
	VOID Set(INT pNetLuncherID, UINT pIpAddr, UINT pPublicIP);	// �Һ� ����
	
	NETLUNCHERINFO(const NETLUNCHERINFO& pNetLuncherInfo);
	
	VOID Reset();
	BOOL IsSet() const { return(EV_NETLUNCHER_DEF != m_NetLuncherID); }

public:
	// Common (�Һ�)
	INT m_NetLuncherID;		// �ݷ�ó ID
	UINT m_IpAddr;					// IP �ּ�
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
	BOOL IsWorldEmpty(EF_SERVICEMNGRTYPE pType) const { return(m_ListWrld[pType].empty()); }		// ����ȭ ���ص� ���� ?
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
	BOOL IsServerEmpty(EF_SERVICEMNGRTYPE pType) const { return(m_ListServ[pType].empty()); }		// ����ȭ ���ص� ���� ?
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
	BOOL IsChannelEmpty(EF_SERVICEMNGRTYPE pType) const { return(m_ListChnl[pType].empty()); }	// ����ȭ ���ص� ���� ?
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
	BOOL IsMeritEmpty(EF_SERVICEMNGRTYPE pType) const { return(m_ListMrit[pType].empty()); }	// ����ȭ ���ص� ���� ?
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
	BOOL IsNetLuncherEmpty(EF_SERVICEMNGRTYPE pType) const { return(m_ListNtlc[pType].empty()); }	// ����ȭ ���ص� ���� ?
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
	VOID ApplyListTempToReal(BOOL pDoLock);		// �ӽ� ����� ���� ��Ͽ� �ϰ� ����

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