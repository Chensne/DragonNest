#include "StdAfx.h"
#include "DNUser.h"
#include "DNVillageConnection.h"
#include "DNGameConnection.h"
#include "DNDivisionManager.h"
#include "Log.h"
#include "Util.h"

#if defined(_CH)
#include "DNShandaFCM.h"
#endif	// _CH

#if !defined(_FINAL_BUILD)
extern int g_nPCBangGrade;
#endif	// #if defined(_WORK)

extern TMasterConfig g_Config;

CDNUser::CDNUser(void)
{
	m_nNexonUserNo = 0;
#if defined(_KR)
	m_biNexonSessionNo = 0;
	m_cPCBangResult = m_cPCBangOption = 0;
	m_cPCBangAuthorizeType = 0;

	m_nPCBangArgument = 0;
	m_bShutdowned = false;
	m_cPolicyError = 0;

	memset(m_szMID, 0, sizeof(m_szMID));
	m_dwGRC = 0;
	m_nShutdownTime = 0;
#endif	// #if defined(_KR)

#if defined (_JP) && defined (WIN64)
	memset(m_szNHNNetCafeCode, 0, sizeof(m_szNHNNetCafeCode));
	memset(m_szNHNProdectCode, 0, sizeof(m_szNHNProdectCode));
#endif	// #if defined (_JP) && defined (WIN64)

#if defined(_KRAZ)
	memset(&m_ShutdownData, 0, sizeof(TShutdownData));
#endif	// #if defined(_KRAZ)

	m_nSessionID = m_nAccountDBID = m_nChannelID = 0;
	m_biCharacterDBID = 0;

	memset(&m_wszAccountName, 0, sizeof(m_wszAccountName));
	memset(&m_szAccountName, 0, sizeof(m_szAccountName));
	memset(&m_wszCharacterName, 0, sizeof(m_wszCharacterName));
	memset(&m_szCharacterName, 0, sizeof(m_szCharacterName));
	memset(&m_szIp, 0, sizeof(m_szIp));
	memset(&m_szVirtualIp, 0, sizeof(m_szVirtualIp));

	m_nLoginServerID = 0;

	m_cVillageID = 0;
	m_cPreVillageID = 0;

	m_wGameID = 0;
	m_nGameServerIdx = -1;
	
	m_nRoomID = m_nRandomSeed = m_nMapIndex = 0;
	m_VillageCheckPartyID = 0;
	m_cGateNo = 0;
	m_cGateSelect = -1;

	m_eUserState = STATE_NONE;
	m_nCheckStateTick = 0;

	memset(&m_szIp, 0, sizeof(char) * IPLENMAX);
	m_bAdult = true;
	m_bPCBang = false;
	m_cPCBangGrade = PCBang::Grade::None;

	// PvP
	m_uiPvPIndex			= 0;
	m_usPvPTeam				= PvPCommon::Team::Max;
	m_uiPvPUserState		= PvPCommon::UserState::None;
	m_cPvPVillageID			= 0;
	m_nPvPVillageChannelID	= 0;

#if defined(_CH)
	m_nFCMOnlineMin = 0;
#endif
	m_nCreateTick = 0;
	m_nAccountLevel = 0;
#if defined(PRE_ADD_DWC)
	m_cCharacterAccountLevel = 0;
	m_nDWCTeamID = 0;
	m_VecDWCMemberCharacterDBIDList.clear();
	m_VecDWCMemberCharacterDBIDList.reserve(DWC::DWC_MAX_MEMBERISZE);
#endif

#if defined(PRE_ADD_SENDLOGOUT)
	m_bSendLogOut = false;
#endif
	m_nDungeonClearCount = 0;
	m_bPvPFatigue = false;
#if defined(PRE_ADD_MULTILANGUAGE)
	m_eSelectedLanguage = MultiLanguage::eDefaultLanguage;
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)
#if defined(PRE_ADD_PVP_VILLAGE_ACCESS)
	m_MatchType = LadderSystem::MatchType::None;
#endif
#ifdef PRE_ADD_COMEBACK
	m_bComebackUser = false;
#endif		//#ifdef PRE_ADD_COMEBACK
#if defined(PRE_ADD_GAMEQUIT_REWARD)
	m_bReConnectNewbieReward = false;
	m_eUserGameQuitRewardType = GameQuitReward::RewardType::None;
#endif	// #if defined(PRE_ADD_GAMEQUIT_REWARD)
#ifdef PRE_ADD_COLOSSEUM_BEGINNER
	m_ePvPChannelType = PvPCommon::RoomType::max;
#endif		//#ifdef PRE_ADD_COLOSSEUM_BEGINNER
#if defined(_ID)
	memset(m_szMacAddress, 0, sizeof(m_szMacAddress));
	memset(m_szKey, 0, sizeof(m_szKey));
	m_bBlockPcCafe = false;
	m_dwKreonCN = 0;
#endif
#if defined(PRE_ADD_REMOTE_QUEST)
	m_AcceptWaitRemoteQuestList.clear();
#endif
#ifdef PRE_ADD_STEAM_USERCOUNT
	m_bSteamUser = false;
#endif		//#ifdef PRE_ADD_STEAM_USERCOUNT
}

CDNUser::~CDNUser(void)
{
}

eLocationState CDNUser::GetLocationState()
{
	switch( GetUserState() )
	{
		case STATE_VILLAGE:
			return _LOCATION_VILLAGE;
		case STATE_GAME:
			return _LOCATION_GAME;
		case STATE_LOGIN:
			return _LOCATION_LOGIN;
		case STATE_CHECKVILLAGE:
		case STATE_CHECKGAME:
			return _LOCATION_MOVE;
	}

	return _LOCATION_NONE;
}

void CDNUser::SetPCGrade(char cGrade)
{
	m_cPCBangGrade = cGrade;

	if (cGrade == PCBang::Grade::None)
		m_bPCBang = false;
	else
		m_bPCBang = true;
}

void CDNUser::SetPCBang(bool bFlag)
{
	m_bPCBang = bFlag;
	if (m_bPCBang)
		m_cPCBangGrade = PCBang::Grade::Normal;
}

char CDNUser::GetPCBangGrade()
{
#if !defined(_FINAL_BUILD)
	return g_nPCBangGrade;		// 이거 테스트용
#else	// #if !defined(_WORK)
	return m_cPCBangGrade;		// 이게 진짜
#endif	// #if !defined(_WORK)
}

#if defined(_KR)
void CDNUser::SetMachineID(BYTE *pszMID, DWORD dwGRC)
{
	if (pszMID)
		memcpy(m_szMID, pszMID, sizeof(m_szMID));

	m_dwGRC = dwGRC;
}
#endif	// #if defined(_KR)

#if defined(_KRAZ)
void CDNUser::SetShutdownData(TShutdownData &ShutdownData)
{
	m_ShutdownData = ShutdownData;
}
#endif	// #if defined(_KRAZ)

#if defined(PRE_MOD_SELECT_CHAR)
bool CDNUser::InitUser(LOMAAddUser *pPacket, int nServerID)
{
	SetLoginServerID(nServerID);

	SetAccountDBID(pPacket->nAccountDBID);
	SetSessionID(pPacket->nSessionID);
	SetCharacterDBID(pPacket->biCharacterDBID);
	SetAccountName(pPacket->wszAccountName);
	SetCharacterName(pPacket->wszCharacterName);
	SetPCGrade(pPacket->cPCBangGrade);
	SetIp(pPacket->szIP);
	SetVirtualIp(pPacket->szIP);
	SetChannelID(pPacket->wChannelID);
	SetAdult(pPacket->bAdult);
#if defined (_KR)
	SetMachineID(pPacket->szMID, pPacket->dwGRC);
#endif
#if defined(_KRAZ)
	SetShutdownData(pPacket->ShutdownData);
#endif	// CDNUser::
#ifdef PRE_ADD_MULTILANGUAGE
	m_eSelectedLanguage = static_cast<MultiLanguage::SupportLanguage::eSupportLanguage>(pPacket->cSelectedLanguage);
#endif		//#ifdef PRE_ADD_MULTILANGUAGE
	m_nAccountLevel = pPacket->cAccountLevel;
#if defined(PRE_ADD_DWC)
	m_cCharacterAccountLevel = pPacket->cCharacterAccountLevel;
#endif
#if defined(_US)
	m_nNexonUserNo = pPacket->nNexonUserNo;
#endif	// _US
#ifdef PRE_ADD_COMEBACK
	m_bComebackUser = pPacket->bComebackUser;
#endif		//#ifdef PRE_ADD_COMEBACK
#if defined(PRE_ADD_GAMEQUIT_REWARD)
	m_bReConnectNewbieReward = pPacket->bReConnectNewbieReward;
	m_eUserGameQuitRewardType =  pPacket->eUserGameQuitReward;
#endif	// #if defined(PRE_ADD_GAMEQUIT_REWARD)
#if defined(_CH) && defined(_FINAL_BUILD)
	FCMIDOnline();	// 피로도 online 
#endif	// _CH
#if defined(_ID)
	SetVirtualIp(pPacket->szVirtualIp);
	_strcpy(m_szMacAddress, _countof(m_szMacAddress),  pPacket->szMacAddress, (int)strlen( pPacket->szMacAddress));					
	_strcpy(m_szKey, _countof(m_szKey),  pPacket->szKey, (int)strlen( pPacket->szKey));
	m_dwKreonCN = pPacket->dwKreonCN;
#endif //#if defined(_ID)

	SetUserState(STATE_LOGIN);
	m_nCreateTick = timeGetTime();

	bool bMoveServerCheck = false;
	if (pPacket->cVillageID > 0)
		bMoveServerCheck = SetCheckVillageInfo(pPacket->cVillageID, pPacket->wChannelID);

	return bMoveServerCheck;
}

bool CDNUser::InitUser(GAMAAddUserList *pPacket)
{
	SetAccountDBID(pPacket->nAccountDBID);
	SetSessionID(pPacket->nSessionID);
	SetCharacterDBID(pPacket->biCharacterDBID);
	SetAccountName(pPacket->wszAccountName);
	SetCharacterName(pPacket->wszCharacterName);
	SetPCGrade(pPacket->cPCBangGrade);
	SetIp(pPacket->szIP);
#if defined(_ID)
	SetVirtualIp(pPacket->szVirtualIp);
#else
	SetVirtualIp(pPacket->szIP);
#endif
	SetChannelID(pPacket->wChannelID);
	SetAdult(pPacket->bAdult);
#if defined (_KR)
	SetMachineID(pPacket->szMID, pPacket->dwGRC);
#endif
#if defined(_KRAZ)
	SetShutdownData(pPacket->ShutdownData);
#endif	// CDNUser::
#ifdef PRE_ADD_MULTILANGUAGE
	m_eSelectedLanguage = static_cast<MultiLanguage::SupportLanguage::eSupportLanguage>(pPacket->cSelectedLanguage);
#endif		//#ifdef PRE_ADD_MULTILANGUAGE

	m_nRoomID = pPacket->nRoomID;

#if defined(_CH) && defined(_FINAL_BUILD)
	FCMIDOnline();	// 피로도 online 
#endif	// _CH
#if defined(PRE_ADD_DWC)
	SetCharacterAccountLevel(pPacket->cAccountLevel);
#endif

	m_nCreateTick = timeGetTime();

	return true;
}

bool CDNUser::InitUser(VIMAAddUserList *pPacket)
{
	SetAccountDBID(pPacket->nAccountDBID);
	SetSessionID(pPacket->nSessionID);
	SetCharacterDBID(pPacket->biCharacterDBID);
	SetAccountName(pPacket->wszAccountName);
	SetCharacterName(pPacket->wszCharacterName);
	SetPCGrade(pPacket->cPCBangGrade);
	SetIp(pPacket->szIP);
#if defined(_ID)
	SetVirtualIp(pPacket->szVirtualIp);
#else
	SetVirtualIp(pPacket->szIP);
#endif
	SetChannelID(pPacket->wChannelID);
	SetAdult(pPacket->bAdult);
#if defined (_KR)
	SetMachineID(pPacket->szMID, pPacket->dwGRC);
#endif
#if defined(_KRAZ)
	SetShutdownData(pPacket->ShutdownData);
#endif	// CDNUser::
#ifdef PRE_ADD_MULTILANGUAGE
	m_eSelectedLanguage = static_cast<MultiLanguage::SupportLanguage::eSupportLanguage>(pPacket->cSelectedLanguage);
#endif		//#ifdef PRE_ADD_MULTILANGUAGE

	SetPvPVillageInfo(pPacket->cPvPVillageID, pPacket->unPvPVillageChannelID);

#if defined(_CH) && defined(_FINAL_BUILD)
	FCMIDOnline();	// 피로도 online 
#endif	// _CH
#if defined(PRE_ADD_DWC)
	SetCharacterAccountLevel(pPacket->cAccountLevel);
#endif

	m_nCreateTick = timeGetTime();

	return true;
}

#else	// #if defined(PRE_MOD_SELECT_CHAR)

void CDNUser::PreInit(int nServerID, UINT nAccountDBID, int nAccountLevel, const char * pszIP)
{
	m_nLoginServerID = nServerID;

	m_nAccountDBID = nAccountDBID;
	m_eUserState = STATE_NONE;
	m_nAccountLevel = nAccountLevel;
	m_nCreateTick = timeGetTime();
#ifdef PRE_MOD_RESTRICT_IDENTITY_IP
	if (pszIP)
		_strcpy(m_szIp, _countof(m_szIp), pszIP, (int)strlen(pszIP));
#endif		//#ifdef PRE_MOD_RESTRICT_IDENTITY_IP
}

void CDNUser::Init(UINT nAccountDBID, UINT nSessionID, INT64 biCharacterDBID, WCHAR *pwszCharacterName, WCHAR *pwszAccountName, USHORT nChannelID, bool bAdult, char *pszIp, char *pszVirtualIp)
{
	m_nAccountDBID = nAccountDBID;
	m_nSessionID = nSessionID;
	m_biCharacterDBID = biCharacterDBID;
	m_nChannelID = nChannelID;

	_wcscpy(m_wszCharacterName, NAMELENMAX, pwszCharacterName, (int)wcslen(pwszCharacterName));
	WideCharToMultiByte(CP_ACP, 0, m_wszCharacterName, -1, m_szCharacterName, NAMELENMAX, NULL, NULL);
	_wcscpy(m_wszAccountName, IDLENMAX, pwszAccountName, (int)wcslen(pwszAccountName));
	WideCharToMultiByte(CP_ACP, 0, m_wszAccountName, -1, m_szAccountName, IDLENMAX, NULL, NULL);

	_strcpy(m_szIp, _countof(m_szIp), pszIp, (int)strlen(pszIp));
	_strcpy(m_szVirtualIp, _countof(m_szVirtualIp), pszVirtualIp, (int)strlen(pszVirtualIp));
	m_bAdult = bAdult;
	m_eUserState = STATE_LOGIN;
}

#endif	// #if defined(PRE_MOD_SELECT_CHAR)

bool CDNUser::SetCheckVillageInfo(BYTE cVillageID, USHORT nChannelID)
{
	if (m_eUserState == STATE_CHECKVILLAGE && (cVillageID != m_cVillageID || nChannelID != m_nChannelID))
	{
		g_Log.Log(LogType::_MOVEPACKET_SEQ, this, L"[ADBID:%u CDBID:%I64d SID:%u] Fail SetCheckVillageInfo():%d (VillageID:%d!=%d) (ChannelID:%d!=%d)\r\n", GetAccountDBID(), GetCharacterDBID(), GetSessionID(), m_nCheckStateTick, (int)cVillageID, (int)m_cVillageID, nChannelID, m_nChannelID );
		//_ASSERT_EXPR(0, L"명재를 찾아주세요!");
		return false;
	}
	m_cPreVillageID = m_cVillageID;
	m_cVillageID = cVillageID;
	m_nChannelID = nChannelID;
	m_eUserState = STATE_CHECKVILLAGE;
	m_nCheckStateTick = timeGetTime();
	g_Log.Log(LogType::_NORMAL, this, L"[ADBID:%u CDBID:%I64d SID:%u] SetCheckVillageInfo():%d (VillageID:%d) (ChannelID:%d)\r\n", GetAccountDBID(), GetCharacterDBID(), GetSessionID(), m_nCheckStateTick, (int)cVillageID, nChannelID );
	return true;
}

bool CDNUser::SetCheckGameInfo(USHORT nGameID, short nServerIdx)
{
	if (m_eUserState == STATE_CHECKGAME && m_nGameServerIdx != -1 && nServerIdx == -1)
	{
		g_Log.Log(LogType::_MOVEPACKET_SEQ, this, L"[ADBID:%u CDBID:%I64d SID:%u] Fail SetCheckGameInfo():%d (GameID:%d,%d)\r\n", GetAccountDBID(), GetCharacterDBID(), GetSessionID(), m_nCheckStateTick, nGameID, nServerIdx );
		//_ASSERT_EXPR(0, L"명재를 찾아주세요!");
		return false;
	}

	m_wGameID = nGameID;
	m_nGameServerIdx = nServerIdx;
	m_eUserState = STATE_CHECKGAME;
	m_nCheckStateTick = timeGetTime();

	if( nServerIdx > -1 )
		g_Log.Log(LogType::_NORMAL, this, L"[ADBID:%u CDBID:%I64d SID:%u] SetCheckGameInfo():%d (GameID:%d,%d)\r\n", GetAccountDBID(), GetCharacterDBID(), GetSessionID(), m_nCheckStateTick, nGameID, nServerIdx );
	return true;
}

bool CDNUser::SetVillageInfo(BYTE cVillageID, USHORT nChannelID, bool bForce)
{
	//STATE_CHECKRECONNECTLOGIN상태로 빌리지서버 또는 게임서버에서 늦게 리포팅이 올 수 있다. 테스트일경우에는 무시해서 처리합니다.
	if (bForce == false && ((m_eUserState != STATE_CHECKVILLAGE && m_eUserState != STATE_CHECKRECONNECTLOGIN) || m_cVillageID != cVillageID || m_nChannelID != nChannelID))	
	{
		g_Log.Log(LogType::_MOVEPACKET_SEQ, this, L"Fail SetVillageInfo ADBID[%u] VID[%d:%d] CHID[%d:%d] STATE[%d]\n", GetAccountDBID(), m_cVillageID, cVillageID, m_nChannelID, nChannelID, static_cast<int>(m_eUserState));
		_ASSERT_EXPR(0, L"명재를 찾아주세요!");
		return false;
	}

	m_cVillageID = cVillageID;
	m_cPreVillageID = m_cVillageID;
	m_nChannelID = nChannelID;
	m_eUserState = STATE_VILLAGE;
	m_nCheckStateTick = 0;
	g_Log.Log(LogType::_NORMAL, this, L"SetVillageInfo ADBID[%u] VID[%d] CHID[%d]\n", GetAccountDBID(), cVillageID, nChannelID);
	return true;
}

bool CDNUser::SetGameInfo(USHORT nGameID, USHORT nServerIdx, bool bForce)
{
	if (bForce == false && (m_eUserState != STATE_CHECKGAME || m_wGameID != nGameID || m_nGameServerIdx != nServerIdx))
	{
		g_Log.Log(LogType::_MOVEPACKET_SEQ, this, L"Fail SetGameInfo ADBID[%u] GID[%d] SIDX[%d]\n", GetAccountDBID(), nGameID, nServerIdx);
		_ASSERT_EXPR(0, L"명재를 찾아주세요!");
		return false;
	}

	m_wGameID = nGameID;
	m_nGameServerIdx = nServerIdx;
	m_eUserState = STATE_GAME;
	m_nCheckStateTick = 0;
	g_Log.Log(LogType::_NORMAL, this, L"SetGameInfo ADBID[%u] GID[%d] SIDX[%d]\n", GetAccountDBID(), nGameID, nServerIdx);
	return true;
}

void CDNUser::SetCheckReconnectLogin()
{
	m_eUserState = STATE_CHECKRECONNECTLOGIN;
	m_nCheckStateTick = timeGetTime();

	g_Log.Log(LogType::_NORMAL, this, L"SetCheckReconnectLogin ADBID[%u]\n", GetAccountDBID());
}

CDNVillageConnection* CDNUser::GetCurrentVillageConnection()
{
	CDNVillageConnection *pVillageCon = g_pDivisionManager->GetVillageConnectionByVillageID(m_cVillageID);
	return pVillageCon;
}

CDNGameConnection* CDNUser::GetCurrentGameConnection()
{
	CDNGameConnection *pGameCon = g_pDivisionManager->GetGameConnectionByGameID(m_wGameID);
	return pGameCon;
}

bool CDNUser::bIsPvPVillage()
{
	const TChannelInfo* pChannelInfo = g_pDivisionManager->GetChannelInfo( m_cVillageID, m_nChannelID );
	if( pChannelInfo )
	{
#if defined(PRE_ADD_DWC)
		if( pChannelInfo->nAttribute&GlobalEnum::CHANNEL_ATT_PVP || pChannelInfo->nAttribute&GlobalEnum::CHANNEL_ATT_DWC )
#else
		if( pChannelInfo->nAttribute&GlobalEnum::CHANNEL_ATT_PVP )
#endif
			return true;
	}
	else
		_DANGER_POINT();

	return false;
}

bool CDNUser::bIsPvPLobby()
{
	const TChannelInfo* pChannelInfo = g_pDivisionManager->GetChannelInfo( m_cVillageID, m_nChannelID );
	if( pChannelInfo )
	{
		if( pChannelInfo->nAttribute&GlobalEnum::CHANNEL_ATT_PVPLOBBY )
			return true;
	}
	else
		_DANGER_POINT();

	return false;
}

#if defined(_CH) && defined(_FINAL_BUILD)
void CDNUser::FCMIDOnline()
{
	if (m_bAdult) return;	// 미성년만 적용
	g_pShandaFCM->FCMIDOnline(m_szAccountName, m_szCharacterName, m_szIp);	// 피로도 online
}

void CDNUser::FCMIDOffline()
{
	if (m_bAdult) return;	// 미성년만 적용
	g_pShandaFCM->FCMIDOffline(m_szAccountName, m_szCharacterName, m_szIp);	// 피로도 offline
}

void CDNUser::SetFCMOnlineMin(int nOnlineMin)
{
	if (m_bAdult) return;	// 미성년만 적용
	if (m_nFCMOnlineMin == nOnlineMin) return;

	m_nFCMOnlineMin = nOnlineMin;

	if (m_eUserState == STATE_VILLAGE){
		CDNVillageConnection *pVillageCon = GetCurrentVillageConnection();
		if (pVillageCon)
			pVillageCon->SendFCMState(m_nAccountDBID, m_nFCMOnlineMin, true);
	}
	else if (m_eUserState == STATE_GAME){
		CDNGameConnection *pGameCon = GetCurrentGameConnection();
		if (pGameCon)
			pGameCon->SendFCMState(m_nAccountDBID, m_nFCMOnlineMin, true);
	}
}
#endif

void CDNUser::SetCharacterName(const WCHAR *pwszCharacterName)
{
	if (!pwszCharacterName) return;

	_wcscpy(m_wszCharacterName, NAMELENMAX, pwszCharacterName, (int)wcslen(pwszCharacterName));
	WideCharToMultiByte(CP_ACP, 0, m_wszCharacterName, -1, m_szCharacterName, NAMELENMAX, NULL, NULL);
}

void CDNUser::SetAccountName(const WCHAR *pwszAccountName)
{
	if (!pwszAccountName) return;

	_wcscpy(m_wszAccountName, IDLENMAX, pwszAccountName, (int)wcslen(pwszAccountName));
	WideCharToMultiByte(CP_ACP, 0, m_wszAccountName, -1, m_szAccountName, IDLENMAX, NULL, NULL);
}

void CDNUser::SetIp(char *pszIp)
{
	if (!pszIp) return;
	_strcpy(m_szIp, _countof(m_szIp), pszIp, (int)strlen(pszIp));
}

void CDNUser::SetVirtualIp(char *pszIp)
{
	if (!pszIp) return;
	_strcpy(m_szVirtualIp, _countof(m_szVirtualIp), pszIp, (int)strlen(pszIp));
}

#if defined(PRE_ADD_REMOTE_QUEST)
void CDNUser::SetAcceptWaitRemoteQuest(int nAcceptWaitRemoteQuestCount, int *AcceptWaitRemoteQuestList)
{
	SAFE_DELETE_VEC(m_AcceptWaitRemoteQuestList);
	if( nAcceptWaitRemoteQuestCount > MAX_PLAY_QUEST) nAcceptWaitRemoteQuestCount = MAX_PLAY_QUEST;
	for(int  i = 0 ; i < nAcceptWaitRemoteQuestCount ; i++)
		m_AcceptWaitRemoteQuestList.push_back(AcceptWaitRemoteQuestList[i]);
}

void CDNUser::GetAcceptWaitRemoteQuest(int *nAcceptWaitRemoteQuestCount, int *AcceptWaitRemoteQuestList)
{	
	for(int i = 0 ; i < m_AcceptWaitRemoteQuestList.size() ; i++)
	{
		if( i >= MAX_PLAY_QUEST ) break;
		AcceptWaitRemoteQuestList[i] = m_AcceptWaitRemoteQuestList[i];
	}
	
	*nAcceptWaitRemoteQuestCount = (int)m_AcceptWaitRemoteQuestList.size();
	if(*nAcceptWaitRemoteQuestCount > MAX_PLAY_QUEST)
		*nAcceptWaitRemoteQuestCount = MAX_PLAY_QUEST;
}
#endif

#if defined(PRE_ADD_DWC)
void CDNUser::SetDWCTeamInfo(VIMADWCTeamMemberList *pPacket)
{
	m_nDWCTeamID = pPacket->nTeamID;
	m_VecDWCMemberCharacterDBIDList.clear();

	for(int i = 0 ; i < pPacket->cCount ; i++)
		m_VecDWCMemberCharacterDBIDList.push_back(pPacket->MemberListCharacterDBID[i]);
}

void CDNUser::GetDWCMemberList(std::vector<INT64> &vecMemberList)
{
	vecMemberList.assign(m_VecDWCMemberCharacterDBIDList.begin(), m_VecDWCMemberCharacterDBIDList.end());
}

bool CDNUser::AddDWCMemberList(INT64 biCharacterDBID)
{
	if( m_nDWCTeamID <= 0 )
		return false;

	if( m_VecDWCMemberCharacterDBIDList.size() > DWC::DWC_MAX_MEMBERISZE )
		return false;

	for( int i = 0 ; i < m_VecDWCMemberCharacterDBIDList.size() ; i++ )
	{
		if( m_VecDWCMemberCharacterDBIDList[i] == biCharacterDBID )
			return false;
	}

	m_VecDWCMemberCharacterDBIDList.push_back(biCharacterDBID);
	return true;
}

bool CDNUser::DelDWCMemberList(INT64 biCharacterDBID)
{
	if( m_nDWCTeamID <= 0 )
		return false;

	if( biCharacterDBID == GetCharacterDBID() )
	{
		ResetDWCInfo();
		return true;
	}
	
	for( int i = 0 ; i < m_VecDWCMemberCharacterDBIDList.size() ; i++ )
	{
		if( m_VecDWCMemberCharacterDBIDList[i] == biCharacterDBID )
		{
			m_VecDWCMemberCharacterDBIDList.erase( m_VecDWCMemberCharacterDBIDList.begin()+i );
			return true;
		}
	}
	return false;
}

void CDNUser::ResetDWCInfo()
{	
	m_nDWCTeamID = 0;
	m_VecDWCMemberCharacterDBIDList.clear();
}
#endif