#include "StdAfx.h"
#include "ServiceInfo.h"


// WORLDINFO

WORLDINFO::WORLDINFO(INT nWorldID)
{
	Reset();

	Set(nWorldID);
}

WORLDINFO::WORLDINFO(const WORLDINFO& pWorldInfo)
{
	Reset();

	(*this) = pWorldInfo;
}

VOID WORLDINFO::Set(INT nWorldID)
{
	m_WorldID = nWorldID;
}

VOID WORLDINFO::Reset()
{
	m_WorldID = 0;
	m_MaxUserLimit = 0;
	m_WorldName[0] = L'\0';
}

VOID WORLDINFO::Set(const WORLDINFO* pWorldInfo)
{
	(*this) = (*pWorldInfo);
}

VOID WORLDINFO::SetChangeable(const WORLDINFO* pWorldInfo)
{
	m_MaxUserLimit = pWorldInfo->m_MaxUserLimit;
	::wcsncpy_s(m_WorldName, pWorldInfo->m_WorldName, _countof(m_WorldName));
}


// SERVERINFO
SERVERINFO::SERVERINFO(
	EF_SERVERTYPE pServerType, 
	INT pServerID, 
	INT pNetLuncherID,
	LPCTSTR pResourceRevision,
	LPCTSTR pExeVersion,
	INT nWorldID[WORLDCOUNTMAX], 
	UINT pIpAddr,
	UINT pPublicIP
	)
{
	Reset();
	Set(pServerType, pServerID, pNetLuncherID, pResourceRevision, pExeVersion, nWorldID, pIpAddr, pPublicIP);
}

SERVERINFO::SERVERINFO(const SERVERINFO& pServerInfo)
{
	Reset();

	(*this) = pServerInfo;
}

VOID SERVERINFO::Set(
	 EF_SERVERTYPE pServerType, 
	 INT pServerID, 
	 INT pNetLuncherID,
	 LPCTSTR pResourceRevision,
	 LPCTSTR pExeVersion,
	 INT nWorldID[WORLDCOUNTMAX], 
	 UINT pIpAddr,
	 UINT pPublicIP
	)
{
	m_ServerType = pServerType;
	m_ServerID = pServerID;
	m_NetLuncherID = pNetLuncherID;
	_tcsncpy_s(m_ResourceRevision, pResourceRevision, _countof(m_ResourceRevision));
	_tcsncpy_s(m_ExeVersion, pExeVersion, _countof(m_ExeVersion));
	::memcpy(m_WorldID, nWorldID, sizeof(m_WorldID));
	m_IpAddr = pIpAddr;
	m_PublicIP = pPublicIP;
}

VOID SERVERINFO::Set(const SERVERINFO* pServerInfo)
{
	(*this) = (*pServerInfo);
}

VOID SERVERINFO::SetChangeable(const SERVERINFO* pServerInfo)
{
	// Common (����)
	m_ServerState = pServerInfo->m_ServerState;

	// Specific  (����)
	m_CurUserCount = pServerInfo->m_CurUserCount;
	m_CurRoomCount = pServerInfo->m_CurRoomCount;
	m_MaxRoomCount = pServerInfo->m_MaxRoomCount;
	m_MaxFarmCount = pServerInfo->m_MaxFarmCount;
	m_TotalFarmUserCount = pServerInfo->m_TotalFarmUserCount;
	for (int i=0; i<pServerInfo->m_MaxFarmCount; i++)
	{
		m_FarmDBID[i] = pServerInfo->m_FarmDBID[i];
		m_CurFarmUserCount[i] = pServerInfo->m_CurFarmUserCount[i];
		m_bActivate[i] = pServerInfo->m_bActivate[i];
	}
	memcpy (m_FrameDelay, pServerInfo->m_FrameDelay, sizeof (pServerInfo->m_FrameDelay));
	m_MeritID = pServerInfo->m_MeritID;
	m_Visibility = pServerInfo->m_Visibility;
}

VOID SERVERINFO::Reset(BOOL /*pIsAll*/)
{
	// Common (�Һ�)
	m_ServerType = EV_SVT_NONE;
	m_ServerID = EV_SERVERID_DEF;
	m_NetLuncherID = EV_NETLUNCHER_DEF;
	m_ResourceRevision[0] = _T('\0');
	m_ExeVersion[0] = _T('\0');
	::memset(m_WorldID, 0, sizeof(m_WorldID));
	m_IpAddr = 0;
	m_PublicIP = 0;

	// Specific (�Һ�)
	m_MeritID = EV_MERITID_DEF;
	m_Visibility = FALSE;

	// Common (����)
	m_ServerState = EV_SVS_NONE;

	// Specific  (����)
	m_CurUserCount = 0;
	m_CurRoomCount = 0;
	m_MaxRoomCount = 0;
	m_MaxFarmCount = 0;
	m_TotalFarmUserCount = 0;
	memset (m_FarmDBID, 0, sizeof(m_FarmDBID));
	memset (m_CurFarmUserCount, 0, sizeof(m_CurFarmUserCount));
	memset (m_bActivate, 0, sizeof(m_bActivate));
	memset (m_FrameDelay, 0, sizeof(m_FrameDelay));
	m_MeritID = EV_MERITID_DEF;
	m_Visibility = FALSE;
}

BOOL SERVERINFO::FindWorldID(INT nWorldID) const
{
	for (int aIndex = 0 ; WORLDCOUNTMAX > aIndex ; ++aIndex) 
	{
		if (EV_WORLDID_DEF == m_WorldID[aIndex])
			return FALSE;
	
		if (nWorldID == m_WorldID[aIndex])
			return TRUE;
	}

	return FALSE;
}


// CHANNEL (VI)
CHANNELINFO::CHANNELINFO(INT pServerID, INT pChannelID, INT nWorldID, INT pMapIndex, INT pMaxUserCount, INT pAttribute, const WCHAR* wszMapName)
{
	Reset();

	Set(pServerID, pChannelID, nWorldID, pMapIndex, pMaxUserCount, pAttribute, wszMapName);
}


CHANNELINFO::CHANNELINFO(const CHANNELINFO& pChannelInfo)
{
	Reset();

	(*this) = pChannelInfo;
}

VOID CHANNELINFO::Set(INT pServerID, INT pChannelID, INT nWorldID, INT pMapIndex, INT pMaxUserCount, INT pAttribute, const WCHAR* wszMapName)
{
	m_ServerID = pServerID;
	m_ChannelID = pChannelID;
	m_WorldID = nWorldID;
	m_MapIndex = pMapIndex;
	m_MaxUserCount = pMaxUserCount;
	m_Attribute = pAttribute;
	wcscpy_s(m_wszMapName, MAPNAMELENMAX, wszMapName);
}

VOID CHANNELINFO::Set(const CHANNELINFO* pChannelInfo)
{
	(*this) = (*pChannelInfo);
}

VOID CHANNELINFO::SetChangeable(const CHANNELINFO* pChannelInfo)
{
	m_CurUserCount = pChannelInfo->m_CurUserCount;
	m_MeritID = pChannelInfo->m_MeritID;
	m_Visibility = pChannelInfo->m_Visibility;
	m_LimitLevel = pChannelInfo->m_LimitLevel;
	m_IsMidShow = pChannelInfo->m_IsMidShow;
	wcscpy_s(m_wszMapName, MAPNAMELENMAX, pChannelInfo->m_wszMapName);
}

VOID CHANNELINFO::Reset()
{
	// Common (�Һ�)
	m_ServerID = EV_SERVERID_DEF;
	m_ChannelID = EV_CHANNELID_DEF;
	m_MapIndex = EV_MAPIDX_DEF;
	m_MaxUserCount = 0;
	m_Attribute = EV_CHNLATTR_DEF;

	// Common (����)
	m_CurUserCount = 0;
	m_MeritID = EV_MERITID_DEF;
	m_Visibility = FALSE;
	m_LimitLevel = EV_LEVEL_DEF;
	m_IsMidShow = FALSE;
	m_nServerID = -1;
	m_cThreadID = 0;

	memset(m_wszMapName, 0x00, sizeof(m_wszMapName));
}


// MERIT (GA, VI)

MERITINFO::MERITINFO(INT pMeritID, INT pMinLevel, INT pMaxLevel, INT pMeritType, INT pExtendValue)
{
	Reset();

	Set(pMeritID, pMinLevel, pMaxLevel, pMeritType, pExtendValue);
}

MERITINFO::MERITINFO(const MERITINFO& pMeritInfo)
{
	Reset();

	(*this) = pMeritInfo;
}

VOID MERITINFO::Set(INT pMeritID, INT pMinLevel, INT pMaxLevel, INT pMeritType, INT pExtendValue)
{
	m_MeritID = pMeritID;
	m_MinLevel = pMinLevel;
	m_MaxLevel = pMaxLevel;
	m_MeritType = pMeritType;
	m_ExtendValue = pExtendValue;
}

VOID MERITINFO::Set(const MERITINFO* pMeritInfo)
{
	(*this) = (*pMeritInfo);
}

VOID MERITINFO::SetChangeable(const MERITINFO* pMeritInfo)
{
		
}

VOID MERITINFO::Reset()
{
	m_MeritID = EV_MERITID_DEF;
	m_MinLevel = 0;
	m_MaxLevel = 0;
	m_MeritType = 0;
	m_ExtendValue = 0;
}


// NET LUNCHER

NETLUNCHERINFO::NETLUNCHERINFO(INT pNetLuncherID, UINT pIpAddr, UINT pPublicIP)
{
	Reset();

	Set(pNetLuncherID, pIpAddr, pPublicIP);
}

VOID NETLUNCHERINFO::Set(INT pNetLuncherID, UINT pIpAddr, UINT pPublicIP)
{
	m_NetLuncherID = pNetLuncherID;
	m_IpAddr = pIpAddr;
	m_PublicIP = pPublicIP;
}

NETLUNCHERINFO::NETLUNCHERINFO(const NETLUNCHERINFO& pNetLuncherInfo)
{
	Reset();

	(*this) = pNetLuncherInfo;
}



VOID NETLUNCHERINFO::Reset()
{
	m_NetLuncherID = EV_NETLUNCHER_DEF;
	m_IpAddr = 0;	
}

CServiceInfo::CServiceInfo ()
{
	m_IsOpen = FALSE; 
	m_ServiceInfoUpdateNo = EV_SVCINFOUPD_DEF;
	m_ContainerVersion = EV_CTNRVERSION_DEF;
	m_TotalUserCount = 0;
	ResetAllRecvMode();

	for(int i = 0; i < EV_SMT_CNT; i++ )
		m_IsFarmInfoReset[i] = FALSE;
}


CServiceInfo::~CServiceInfo ()
{

}

DWORD CServiceInfo::Open()
{
	DWORD aRetVal = m_Lock.Open();
	if (NOERROR != aRetVal) {
		BASE_RETURN(aRetVal);
	}

	
	m_IsOpen = TRUE;

	return NOERROR;
}

VOID CServiceInfo::Close()
{
	for (int aIndex = 0 ; EV_SMT_CNT > aIndex ; ++aIndex) 
	{
		m_ListWrld[aIndex].clear();
		m_ListServ[aIndex].clear();
		m_ListChnl[aIndex].clear();
		m_ListMrit[aIndex].clear();
		m_ListNtlc[aIndex].clear();
	}

	m_Lock.Close();
	m_IsOpen = FALSE;
}

WORLDINFO* CServiceInfo::AtWorld(EF_SERVICEMNGRTYPE pType, INT nWorldID, BOOL pDoLock)
{
	TP_LOCK* aLock = (pDoLock)?(GetLock()):(NULL);
	TP_LOCKAUTO AutoLock(aLock);


	TP_LISTWRLD_ITR aIt = m_ListWrld[pType].find(nWorldID);
	if (m_ListWrld[pType].end() != aIt)
		return(static_cast<WORLDINFO*>(&aIt->second));
	
	return NULL;
}

const WORLDINFO* CServiceInfo::AtWorld(EF_SERVICEMNGRTYPE pType, INT nWorldID, BOOL pDoLock) const
{
	return(static_cast<const WORLDINFO*>(const_cast<CServiceInfo*>(this)->AtWorld(pType, nWorldID, pDoLock)));
}

DWORD CServiceInfo::AddWorld(EF_SERVICEMNGRTYPE pType, INT nWorldID, BOOL pDoLock)
{
	TP_LOCK* aLock = (pDoLock)?(GetLock()):(NULL);
	TP_LOCKAUTO AutoLock(aLock);
	
	if (m_ListWrld[pType].find(nWorldID) != m_ListWrld[pType].end())
		return NOERROR;

	std::pair<TP_LISTWRLD_CTR, bool> aRetVal = m_ListWrld[pType].insert(TP_LISTWRLD::value_type(nWorldID, WORLDINFO(nWorldID)));
	
	return NOERROR;
}

DWORD CServiceInfo::AddWorld(EF_SERVICEMNGRTYPE pType, const WORLDINFO* pWorldInfo, BOOL pDoLock)
{
	TP_LOCK* aLock = (pDoLock)?(GetLock()):(NULL);
	TP_LOCKAUTO AutoLock(aLock);

	if (m_ListWrld[pType].find(pWorldInfo->m_WorldID) != m_ListWrld[pType].end())
		return NOERROR;

	std::pair<TP_LISTWRLD_CTR, bool> aRetVal = m_ListWrld[pType].insert(TP_LISTWRLD::value_type(pWorldInfo->m_WorldID, WORLDINFO((*pWorldInfo))));
	
	return NOERROR;
}

VOID CServiceInfo::GetWorldList(EF_SERVICEMNGRTYPE pType, TP_LISTWRLDAUTO& pList, BOOL pDoLock) const
{
	TP_LOCK* aLock = (pDoLock)?(GetLock()):(NULL);
	TP_LOCKAUTO AutoLock(aLock);


	TP_LISTWRLD_CTR aIt = m_ListWrld[pType].begin();
	for (; m_ListWrld[pType].end() != aIt ; ++aIt) 
	{
		const WORLDINFO* aWorldInfo = (&aIt->second);	
		pList.push_back(aIt->second);
	}
}

VOID CServiceInfo::ClearWorldList(EF_SERVICEMNGRTYPE pType, BOOL pDoLock)
{
	TP_LOCK* aLock = (pDoLock)?(GetLock()):(NULL);
	TP_LOCKAUTO AutoLock(aLock);
	
	m_ListWrld[pType].clear();
}

CServiceInfo::TP_LISTWRLD_ITR CServiceInfo::BeginWorld(EF_SERVICEMNGRTYPE pType, BOOL pDoLock)
{
	TP_LOCK* aLock = (pDoLock)?(GetLock()):(NULL);
	TP_LOCKAUTO AutoLock(aLock);

	
	return(m_ListWrld[pType].begin());
}

CServiceInfo::TP_LISTWRLD_CTR CServiceInfo::BeginWorld(EF_SERVICEMNGRTYPE pType, BOOL pDoLock) const
{
	TP_LOCK* aLock = (pDoLock)?(GetLock()):(NULL);
	TP_LOCKAUTO AutoLock(aLock);


	return(m_ListWrld[pType].begin());
}

CServiceInfo::TP_LISTWRLD_ITR CServiceInfo::EndWorld(EF_SERVICEMNGRTYPE pType, BOOL pDoLock)
{
	TP_LOCK* aLock = (pDoLock)?(GetLock()):(NULL);
	TP_LOCKAUTO AutoLock(aLock);
	
	return(m_ListWrld[pType].end());
}

CServiceInfo::TP_LISTWRLD_CTR CServiceInfo::EndWorld(EF_SERVICEMNGRTYPE pType, BOOL pDoLock) const
{
	TP_LOCK* aLock = (pDoLock)?(GetLock()):(NULL);
	TP_LOCKAUTO AutoLock(aLock);
	
	return(m_ListWrld[pType].end());
}

SERVERINFO* CServiceInfo::AtServer(EF_SERVICEMNGRTYPE pType, INT pServerID, BOOL pDoLock)
{
	TP_LOCK* aLock = (pDoLock)?(GetLock()):(NULL);
	TP_LOCKAUTO AutoLock(aLock);

	TP_LISTSERV_ITR aIt = m_ListServ[pType].find(pServerID);
	if (m_ListServ[pType].end() != aIt)
		return(static_cast<SERVERINFO*>(&aIt->second));

	return NULL;
}

const SERVERINFO* CServiceInfo::AtServer(EF_SERVICEMNGRTYPE pType, INT pServerID, BOOL pDoLock) const
{
	return(static_cast<const SERVERINFO*>(const_cast<CServiceInfo*>(this)->AtServer(pType, pServerID, pDoLock)));
}

DWORD CServiceInfo::AddServer(EF_SERVICEMNGRTYPE pType, const SERVERINFO* pServerInfo, BOOL pDoLock)
{
	TP_LOCK* aLock = (pDoLock)?(GetLock()):(NULL);
	TP_LOCKAUTO AutoLock(aLock);

	std::pair<TP_LISTSERV_CTR, bool> aRetVal = m_ListServ[pType].insert(TP_LISTSERV::value_type(pServerInfo->m_ServerID, SERVERINFO((*pServerInfo))));
	if (!aRetVal.second)
		return HASERROR;
		
	
	return NOERROR;
}

VOID CServiceInfo::GetServerList(EF_SERVICEMNGRTYPE pType, TP_LISTSERVAUTO& pList, INT nWorldID, BOOL pDoLock) const
{
	TP_LOCK* aLock = (pDoLock)?(GetLock()):(NULL);
	TP_LOCKAUTO AutoLock(aLock);

	TP_LISTSERV_CTR aIt = m_ListServ[pType].begin();
	for (; m_ListServ[pType].end() != aIt ; ++aIt) 
	{
		const SERVERINFO* aServerInfo = (&aIt->second);
		if (EV_WORLDID_ALL != nWorldID && !aServerInfo->FindWorldID(nWorldID))
			continue;
		
		pList.push_back(aIt->second);
	}
}

VOID CServiceInfo::ClearServerList(EF_SERVICEMNGRTYPE pType, BOOL pDoLock)
{
	TP_LOCK* aLock = (pDoLock)?(GetLock()):(NULL);
	TP_LOCKAUTO AutoLock(aLock);
	
	m_ListServ[pType].clear();
}

CServiceInfo::TP_LISTSERV_ITR CServiceInfo::BeginServer(EF_SERVICEMNGRTYPE pType, BOOL pDoLock)
{
	TP_LOCK* aLock = (pDoLock)?(GetLock()):(NULL);
	TP_LOCKAUTO AutoLock(aLock);

	
	return(m_ListServ[pType].begin());
}

CServiceInfo::TP_LISTSERV_CTR CServiceInfo::BeginServer(EF_SERVICEMNGRTYPE pType, BOOL pDoLock) const
{
	TP_LOCK* aLock = (pDoLock)?(GetLock()):(NULL);
	TP_LOCKAUTO AutoLock(aLock);

	
	return(m_ListServ[pType].begin());
}

CServiceInfo::TP_LISTSERV_ITR CServiceInfo::EndServer(EF_SERVICEMNGRTYPE pType, BOOL pDoLock)
{
	TP_LOCK* aLock = (pDoLock)?(GetLock()):(NULL);
	TP_LOCKAUTO AutoLock(aLock);


	return(m_ListServ[pType].end());
}

CServiceInfo::TP_LISTSERV_CTR CServiceInfo::EndServer(EF_SERVICEMNGRTYPE pType, BOOL pDoLock) const
{
	TP_LOCK* aLock = (pDoLock)?(GetLock()):(NULL);
	TP_LOCKAUTO AutoLock(aLock);

	
	return(m_ListServ[pType].end());
}

CHANNELINFO* CServiceInfo::AtChannel(EF_SERVICEMNGRTYPE pType, INT pServerID, INT pChannelID, BOOL pDoLock)
{
	TP_LOCK* aLock = (pDoLock)?(GetLock()):(NULL);
	TP_LOCKAUTO AutoLock(aLock);

	
	TP_LISTCHNL_ITR aIt = m_ListChnl[pType].find(MAKE_SM_SVRCHN(pServerID, pChannelID));
	if (m_ListChnl[pType].end() != aIt)
		return(static_cast<CHANNELINFO*>(&aIt->second));
	
	return NULL;
}

const CHANNELINFO* CServiceInfo::AtChannel(EF_SERVICEMNGRTYPE pType, INT pServerID, INT pChannelID, BOOL pDoLock) const
{
	return(static_cast<const CHANNELINFO*>(const_cast<CServiceInfo*>(this)->AtChannel(pType, pServerID, pChannelID, pDoLock)));
}

DWORD CServiceInfo::AddChannel(EF_SERVICEMNGRTYPE pType, INT pServerID, INT pChannelID, INT nWorldID, INT pMapIndex, INT pMaxUserCount, INT pAttribute, const WCHAR* wszMapName, BOOL pDoLock)
{
	TP_LOCK* aLock = (pDoLock)?(GetLock()):(NULL);
	TP_LOCKAUTO AutoLock(aLock);

	std::pair<TP_LISTCHNL_CTR, bool> aRetVal = m_ListChnl[pType].insert(TP_LISTCHNL::value_type(MAKE_SM_SVRCHN(pServerID, pChannelID), CHANNELINFO(pServerID, pChannelID, nWorldID, pMapIndex, pMaxUserCount, pAttribute, wszMapName)));
	if (!aRetVal.second)
		return (HASERROR+0);
	
	return NOERROR;
}

DWORD CServiceInfo::AddChannel(EF_SERVICEMNGRTYPE pType, const CHANNELINFO* pChannelInfo, BOOL pDoLock)
{

	TP_LOCK* aLock = (pDoLock)?(GetLock()):(NULL);
	TP_LOCKAUTO AutoLock(aLock);


	std::pair<TP_LISTCHNL_CTR, bool> aRetVal = m_ListChnl[pType].insert(TP_LISTCHNL::value_type(MAKE_SM_SVRCHN(pChannelInfo->m_ServerID, pChannelInfo->m_ChannelID), CHANNELINFO((*pChannelInfo))));
	if (!aRetVal.second)
		return HASERROR;
	
	return NOERROR;
}

VOID CServiceInfo::GetChannelList(EF_SERVICEMNGRTYPE pType, TP_LISTCHNLAUTO& pList, INT pServerID, BOOL pDoLock) const
{
	TP_LOCK* aLock = (pDoLock)?(GetLock()):(NULL);
	TP_LOCKAUTO AutoLock(aLock);


	TP_LISTCHNL_CTR aIt = m_ListChnl[pType].begin();
	for (; m_ListChnl[pType].end() != aIt ; ++aIt) 
	{
		const CHANNELINFO* aChannelInfo = (&aIt->second);
		if (EV_SERVERID_ALL != pServerID && aChannelInfo->m_ServerID != pServerID)
			continue;
		pList.push_back(aIt->second);
	}
}

VOID CServiceInfo::ClearChannelList(EF_SERVICEMNGRTYPE pType, BOOL pDoLock)
{
	TP_LOCK* aLock = (pDoLock)?(GetLock()):(NULL);
	TP_LOCKAUTO AutoLock(aLock);
	
	m_ListChnl[pType].clear();
}

CServiceInfo::TP_LISTCHNL_ITR CServiceInfo::BeginChannel(EF_SERVICEMNGRTYPE pType, BOOL pDoLock)
{
	TP_LOCK* aLock = (pDoLock)?(GetLock()):(NULL);
	TP_LOCKAUTO AutoLock(aLock);
		
	return(m_ListChnl[pType].begin());
}

CServiceInfo::TP_LISTCHNL_CTR CServiceInfo::BeginChannel(EF_SERVICEMNGRTYPE pType, BOOL pDoLock) const
{
	TP_LOCK* aLock = (pDoLock)?(GetLock()):(NULL);
	TP_LOCKAUTO AutoLock(aLock);
	

	return(m_ListChnl[pType].begin());
}

CServiceInfo::TP_LISTCHNL_ITR CServiceInfo::EndChannel(EF_SERVICEMNGRTYPE pType, BOOL pDoLock)
{
	TP_LOCK* aLock = (pDoLock)?(GetLock()):(NULL);
	TP_LOCKAUTO AutoLock(aLock);

	return(m_ListChnl[pType].end());
}

CServiceInfo::TP_LISTCHNL_CTR CServiceInfo::EndChannel(EF_SERVICEMNGRTYPE pType, BOOL pDoLock) const
{
	TP_LOCK* aLock = (pDoLock)?(GetLock()):(NULL);
	TP_LOCKAUTO AutoLock(aLock);

	
	return(m_ListChnl[pType].end());
}

MERITINFO* CServiceInfo::AtMerit(EF_SERVICEMNGRTYPE pType, INT pMeritID, BOOL pDoLock)
{
	TP_LOCK* aLock = (pDoLock)?(GetLock()):(NULL);
	TP_LOCKAUTO AutoLock(aLock);


	TP_LISTMRIT_ITR aIt = m_ListMrit[pType].find(pMeritID);
	if (m_ListMrit[pType].end() != aIt) {
		return(static_cast<MERITINFO*>(&aIt->second));
	}

	return NULL;
}

const MERITINFO* CServiceInfo::AtMerit(EF_SERVICEMNGRTYPE pType, INT pMeritID, BOOL pDoLock) const
{
	return(static_cast<const MERITINFO*>(const_cast<CServiceInfo*>(this)->AtMerit(pType, pMeritID, pDoLock)));
}

DWORD CServiceInfo::AddMerit(EF_SERVICEMNGRTYPE pType, INT pMeritID, INT pMinLevel, INT pMaxLevel, INT pMeritType, INT pExtendValue, BOOL pDoLock)
{
	TP_LOCK* aLock = (pDoLock)?(GetLock()):(NULL);
	TP_LOCKAUTO AutoLock(aLock);

	std::pair<TP_LISTMRIT_CTR, bool> aRetVal = m_ListMrit[pType].insert(TP_LISTMRIT::value_type(pMeritID, MERITINFO(pMeritID, pMinLevel, pMaxLevel, pMeritType, pExtendValue)));
	if (!aRetVal.second)
		return (HASERROR+0);
	
	
	return NOERROR;
}

DWORD CServiceInfo::AddMerit(EF_SERVICEMNGRTYPE pType, const MERITINFO* pMeritInfo, BOOL pDoLock)
{
	TP_LOCK* aLock = (pDoLock)?(GetLock()):(NULL);
	TP_LOCKAUTO AutoLock(aLock);

	std::pair<TP_LISTMRIT_CTR, bool> aRetVal = m_ListMrit[pType].insert(TP_LISTMRIT::value_type(pMeritInfo->m_MeritID, MERITINFO((*pMeritInfo))));
	if (!aRetVal.second)
		return HASERROR;
	
	return NOERROR;
}

VOID CServiceInfo::GetMeritList(EF_SERVICEMNGRTYPE pType, TP_LISTMRITAUTO& pList, BOOL pDoLock) const
{
	TP_LOCK* aLock = (pDoLock)?(GetLock()):(NULL);
	TP_LOCKAUTO AutoLock(aLock);


	TP_LISTMRIT_CTR aIt = m_ListMrit[pType].begin();
	for (; m_ListMrit[pType].end() != aIt ; ++aIt) 
	{
		const MERITINFO* aMeritInfo = (&aIt->second);
		pList.push_back(aIt->second);
	}
}

VOID CServiceInfo::ClearMeritList(EF_SERVICEMNGRTYPE pType, BOOL pDoLock)
{
	TP_LOCK* aLock = (pDoLock)?(GetLock()):(NULL);
	TP_LOCKAUTO AutoLock(aLock);


	m_ListMrit[pType].clear();
}

CServiceInfo::TP_LISTMRIT_ITR CServiceInfo::BeginMerit(EF_SERVICEMNGRTYPE pType, BOOL pDoLock)
{
	TP_LOCK* aLock = (pDoLock)?(GetLock()):(NULL);
	TP_LOCKAUTO AutoLock(aLock);


	return(m_ListMrit[pType].begin());
}

CServiceInfo::TP_LISTMRIT_CTR CServiceInfo::BeginMerit(EF_SERVICEMNGRTYPE pType, BOOL pDoLock) const
{
	TP_LOCK* aLock = (pDoLock)?(GetLock()):(NULL);
	TP_LOCKAUTO AutoLock(aLock);


	return(m_ListMrit[pType].begin());
}

CServiceInfo::TP_LISTMRIT_ITR CServiceInfo::EndMerit(EF_SERVICEMNGRTYPE pType, BOOL pDoLock)
{
	TP_LOCK* aLock = (pDoLock)?(GetLock()):(NULL);
	TP_LOCKAUTO AutoLock(aLock);


	return(m_ListMrit[pType].end());
}

CServiceInfo::TP_LISTMRIT_CTR CServiceInfo::EndMerit(EF_SERVICEMNGRTYPE pType, BOOL pDoLock) const
{
	TP_LOCK* aLock = (pDoLock)?(GetLock()):(NULL);
	TP_LOCKAUTO AutoLock(aLock);


	return(m_ListMrit[pType].end());
}

NETLUNCHERINFO* CServiceInfo::AtNetLuncher(EF_SERVICEMNGRTYPE pType, INT pNetLuncherID, BOOL pDoLock)
{
	TP_LOCK* aLock = (pDoLock)?(GetLock()):(NULL);
	TP_LOCKAUTO AutoLock(aLock);


	TP_LISTNTLC_ITR aIt = m_ListNtlc[pType].find(pNetLuncherID);
	if (m_ListNtlc[pType].end() != aIt)
		return(static_cast<NETLUNCHERINFO*>(&aIt->second));	

	return NULL;
}

const NETLUNCHERINFO* CServiceInfo::AtNetLuncher(EF_SERVICEMNGRTYPE pType, INT pNetLuncherID, BOOL pDoLock) const
{
	return(static_cast<const NETLUNCHERINFO*>(const_cast<CServiceInfo*>(this)->AtNetLuncher(pType, pNetLuncherID, pDoLock)));
}

DWORD CServiceInfo::AddNetLuncher(EF_SERVICEMNGRTYPE pType, const NETLUNCHERINFO* pNetLuncherInfo, BOOL pDoLock)
{
	TP_LOCK* aLock = (pDoLock)?(GetLock()):(NULL);
	TP_LOCKAUTO AutoLock(aLock);


	std::pair<TP_LISTNTLC_CTR, bool> aRetVal = m_ListNtlc[pType].insert(TP_LISTNTLC::value_type(pNetLuncherInfo->m_NetLuncherID, NETLUNCHERINFO((*pNetLuncherInfo))));
	if (!aRetVal.second)
		return HASERROR;
	
	return NOERROR;
}

VOID CServiceInfo::GetNetLuncherList(EF_SERVICEMNGRTYPE pType, TP_LISTNTLCAUTO& pList, BOOL pDoLock) const
{

	TP_LOCK* aLock = (pDoLock)?(GetLock()):(NULL);
	TP_LOCKAUTO AutoLock(aLock);
	

	TP_LISTNTLC_CTR aIt = m_ListNtlc[pType].begin();
	for (; m_ListNtlc[pType].end() != aIt ; ++aIt) 
	{
		const NETLUNCHERINFO* aNetLuncherInfo = (&aIt->second);	
		pList.push_back(aIt->second);
	}
}

VOID CServiceInfo::ClearNetLuncherList(EF_SERVICEMNGRTYPE pType, BOOL pDoLock)
{
	TP_LOCK* aLock = (pDoLock)?(GetLock()):(NULL);
	TP_LOCKAUTO AutoLock(aLock);


	m_ListNtlc[pType].clear();
}

CServiceInfo::TP_LISTNTLC_ITR CServiceInfo::BeginNetLuncher(EF_SERVICEMNGRTYPE pType, BOOL pDoLock)
{
	TP_LOCK* aLock = (pDoLock)?(GetLock()):(NULL);
	TP_LOCKAUTO AutoLock(aLock);

	
	return(m_ListNtlc[pType].begin());
}

CServiceInfo::TP_LISTNTLC_CTR CServiceInfo::BeginNetLuncher(EF_SERVICEMNGRTYPE pType, BOOL pDoLock) const
{
	TP_LOCK* aLock = (pDoLock)?(GetLock()):(NULL);
	TP_LOCKAUTO AutoLock(aLock);


	return(m_ListNtlc[pType].begin());
}

CServiceInfo::TP_LISTNTLC_ITR CServiceInfo::EndNetLuncher(EF_SERVICEMNGRTYPE pType, BOOL pDoLock)
{
	TP_LOCK* aLock = (pDoLock)?(GetLock()):(NULL);
	TP_LOCKAUTO AutoLock(aLock);


	return(m_ListNtlc[pType].end());
}

CServiceInfo::TP_LISTNTLC_CTR CServiceInfo::EndNetLuncher(EF_SERVICEMNGRTYPE pType, BOOL pDoLock) const
{
	TP_LOCK* aLock = (pDoLock)?(GetLock()):(NULL);
	TP_LOCKAUTO AutoLock(aLock);

	
	return(m_ListNtlc[pType].end());
}

BOOL CServiceInfo::GetRecvMode(EF_SERVICERECVTYPE pType) const
{
	return(m_RecvMode[pType]);
}

VOID CServiceInfo::SetRecvMode(EF_SERVICERECVTYPE pType, BOOL pRecvMode)
{
	if (pRecvMode)
		m_RecvTick[pType] = 0;
	else 
	{
		DWORD aCurTick = ::GetTickCount();
		m_RecvTick[pType] = ((aCurTick)?(aCurTick):(1));
	}
	m_RecvMode[pType] = pRecvMode;
}

VOID CServiceInfo::ResetAllRecvMode()
{
	for (int aIndex = 0 ; EV_SRT_CNT > aIndex ; ++aIndex) 
	{
		DWORD aCurTick = ::GetTickCount();
		m_RecvTick[aIndex] = ((aCurTick)?(aCurTick):(1));
		m_RecvMode[aIndex] = FALSE;
	}
}

BOOL CServiceInfo::IsRecvTick(EF_SERVICERECVTYPE pType) const
{
	return((m_RecvTick[pType])?(TRUE):(FALSE));
}

VOID CServiceInfo::SetRecvTick(EF_SERVICERECVTYPE pType)
{
	DWORD aCurTick = ::GetTickCount();
	m_RecvTick[pType] = ((aCurTick)?(aCurTick):(1));
}

DWORD CServiceInfo::GetRecvTickTerm(EF_SERVICERECVTYPE pType) const
{
	return(GetTickTerm(m_RecvTick[pType], ::GetTickCount()));
}

VOID CServiceInfo::ClearAllList(EF_SERVICEMNGRTYPE pType, BOOL pDoLock)
{
	TP_LOCK* aLock = (pDoLock)?(GetLock()):(NULL);
	TP_LOCKAUTO AutoLock(aLock);

	
	ClearWorldList(pType, FALSE);
	ClearServerList(pType, FALSE);
	ClearChannelList(pType, FALSE);
	ClearMeritList(pType, FALSE);
	ClearNetLuncherList(pType, FALSE);
}

VOID CServiceInfo::ApplyListTempToReal(BOOL pDoLock)
{
	TP_LOCK* aLock = (pDoLock)?(GetLock()):(NULL);
	TP_LOCKAUTO AutoLock(aLock);

		
	// WORLD
	{
		m_ListWrld[EV_SMT_REAL].clear();

		TP_LISTWRLD_CTR aIt = m_ListWrld[EV_SMT_TEMP].begin();
		for (; m_ListWrld[EV_SMT_TEMP].end() != aIt ; ++aIt) {
			m_ListWrld[EV_SMT_REAL].insert(TP_LISTWRLD::value_type(aIt->first, aIt->second));
		}
	}

	// SERVER
	{
		m_ListServ[EV_SMT_REAL].clear();

		TP_LISTSERV_CTR aIt = m_ListServ[EV_SMT_TEMP].begin();
		for (; m_ListServ[EV_SMT_TEMP].end() != aIt ; ++aIt) {
			m_ListServ[EV_SMT_REAL].insert(TP_LISTSERV::value_type(aIt->first, aIt->second));
		}
	}
	
	// CHANNEL (VI)
	{
		m_ListChnl[EV_SMT_REAL].clear();

		TP_LISTCHNL_CTR aIt = m_ListChnl[EV_SMT_TEMP].begin();
		for (; m_ListChnl[EV_SMT_TEMP].end() != aIt ; ++aIt) {
			m_ListChnl[EV_SMT_REAL].insert(TP_LISTCHNL::value_type(aIt->first, aIt->second));
		}
	}

	// MERIT (GA, VI)
	{
		m_ListMrit[EV_SMT_REAL].clear();
		
		TP_LISTMRIT_CTR aIt = m_ListMrit[EV_SMT_TEMP].begin();
		for (; m_ListMrit[EV_SMT_TEMP].end() != aIt ; ++aIt) {
			m_ListMrit[EV_SMT_REAL].insert(TP_LISTMRIT::value_type(aIt->first, aIt->second));
		}
	}

	// NET LUNCHER
	{
		m_ListNtlc[EV_SMT_REAL].clear();
		
		TP_LISTNTLC_CTR aIt = m_ListNtlc[EV_SMT_TEMP].begin();
		for (; m_ListNtlc[EV_SMT_TEMP].end() != aIt ; ++aIt) {
			m_ListNtlc[EV_SMT_REAL].insert(TP_LISTNTLC::value_type(aIt->first, aIt->second));
		}
	}

	++ m_ServiceInfoUpdateNo;
}

void CServiceInfo::ResetDelayCount(EF_SERVICEMNGRTYPE pType)
{
	TP_LISTSERV_ITR aIt = m_ListServ[pType].begin();
	for (; m_ListServ[pType].end() != aIt ; ++aIt) 
	{
		for (int i=0; i<SERVERTYPE_MAX; i++)
		{
			SERVERINFO* aServerInfo = (&aIt->second);
			aServerInfo->m_FrameDelay[i] = 0;
		}
	}
}

void CServiceInfo::ResetFarmInfo(EF_SERVICEMNGRTYPE pType)
{
	//���� ���� ������, ���� ���� ���Ŵ� 1ȸ�� �ϱ� ���� �÷��� �˻�
	if(IsFarmInfoReset(pType)) return;
	m_IsFarmInfoReset[pType] = TRUE;

	TP_LISTSERV_ITR aIt = m_ListServ[pType].begin();
	for (; m_ListServ[pType].end() != aIt ; ++aIt) 
	{
		SERVERINFO* aServerInfo = (&aIt->second);
		aServerInfo->m_MaxFarmCount = 0;
		aServerInfo->m_TotalFarmUserCount = 0;
		memset (aServerInfo->m_bActivate, 0, sizeof(aServerInfo->m_bActivate));
		memset (aServerInfo->m_FarmDBID, 0, sizeof(aServerInfo->m_FarmDBID));
		memset (aServerInfo->m_CurFarmUserCount, 0, sizeof(aServerInfo->m_CurFarmUserCount));
	}
}

inline DWORD CServiceInfo::GetTickTerm(DWORD pOldTick, DWORD pCurTick) const
{
	return((pCurTick >= pOldTick)?(pCurTick - pOldTick):(ULONG_MAX - pOldTick + pCurTick));
}
