#pragma once

#pragma pack(push, 1)

//--------------------------------------------------------------
//	Login Packet
//--------------------------------------------------------------

namespace Login
{
	struct TQueryLoginOutput
	{
		UINT nAccountDBID;
		bool IsNewAccount;
		TIMESTAMP_STRUCT LogOutDate;
#if defined(PRE_ADD_GAMEQUIT_REWARD)
		TIMESTAMP_STRUCT RegistDate;
#endif	// #if defined(PRE_ADD_GAMEQUIT_REWARD)
	};
};

struct LOMARegist
{
	int nServerID;			// g_Config.nManagedID 의미
};

#if defined(PRE_MOD_SELECT_CHAR)

struct LOMAAddUser
{
	UINT nAccountDBID;
	UINT nSessionID;
	INT64 biCharacterDBID;
	WCHAR wszAccountName[IDLENMAX];
	WCHAR wszCharacterName[NAMELENMAX];
	char cPCBangGrade;	// 피씨방 등급 (0: 피씨방 아님, 나머지: ePCBangGrade)
	char szIP[IPLENMAX];	// 피로도때문에 넣음
	char szVirtualIp[IPLENMAX];
	USHORT wChannelID;
	bool bAdult;			// 어른인지 아닌지 (피로도)
#ifdef _KR
	BYTE szMID[MACHINEIDMAX];
	DWORD dwGRC;
#endif	// #ifdef _KR
#ifdef PRE_ADD_MULTILANGUAGE
	char cSelectedLanguage;
#endif	//#ifdef PRE_ADD_MULTILANGUAGE

	bool bForce;
	BYTE cVillageID;
	BYTE cAccountLevel;
#if defined(PRE_ADD_DWC)
	BYTE cCharacterAccountLevel;
#endif
	bool bTutorial;
#ifdef PRE_ADD_COMEBACK
	bool bComebackUser;
#endif		//PRE_ADD_COMEBACK
#if defined(PRE_ADD_GAMEQUIT_REWARD)
	bool bReConnectNewbieReward;
	GameQuitReward::RewardType::eType eUserGameQuitReward;
#endif	// #if defined(PRE_ADD_GAMEQUIT_REWARD)
#if defined(_US)
	UINT nNexonUserNo;
#endif	// _US
#if defined(_ID)
	char szMacAddress[MACADDRLENMAX];
	char szKey[KREONKEYMAX];
	DWORD dwKreonCN;
#endif
#if defined(_KRAZ)
	TShutdownData ShutdownData;
#endif	// #if defined(_KRAZ)
#ifdef PRE_ADD_STEAM_USERCOUNT
	bool bSteamUser;
#endif		//#ifdef PRE_ADD_STEAM_USERCOUNT
};

#else		// #if defined(PRE_MOD_SELECT_CHAR)

struct LOMAAddUser				// LOMA_ADDUSER
{
	UINT nAccountDBID;
	UINT nSessionID;
	WCHAR wszAccountName[IDLENMAX];
#ifdef _KR
	BYTE szMID[MACHINEIDMAX];
	DWORD dwGRC;
#endif //#ifdef _KR
	bool bForce;
	BYTE cAccountLevel;
	char cWorldID;
#ifdef PRE_MOD_RESTRICT_IDENTITY_IP
	char szIP[IPLENMAX];
#endif		//#ifdef PRE_MOD_RESTRICT_IDENTITY_IP
#ifdef PRE_ADD_MULTILANGUAGE
	char cSelectedLanguage;
#endif		//#ifdef PRE_ADD_MULTILANGUAGE
#ifdef PRE_ADD_COMEBACK
	bool bComebackUser;
#endif		//PRE_ADD_COMEBACK
#if defined(PRE_ADD_GAMEQUIT_REWARD)
	bool bReConnectNewbieReward;
	GameQuitReward::RewardType::eType eUserGameQuitReward;
#endif	// #if defined(PRE_ADD_GAMEQUIT_REWARD)
#ifdef PRE_ADD_STEAM_USERCOUNT
	bool bSteamUser;
#endif		//#ifdef PRE_ADD_STEAM_USERCOUNT
};

struct LOMAUserInfo				// LOMA_USERINFO
{
	UINT	nSessionID;
	UINT	nAccountDBID;
	INT64	biCharacterDBID;
	WCHAR	wszAccountName[IDLENMAX];
	WCHAR	wszCharacterName[NAMELENMAX];
	BYTE	cVillageID;
	USHORT	nChannelID;
	bool	bAdult;			// 어른이냐 아니냐(피로도)
	char	szIP[IPLENMAX];	// 피로도때문에 넣음
	char	szVirtualIp[IPLENMAX];
	bool	bTutorial;
	char	cPCBangGrade;	// 피씨방 등급 (0: 피씨방 아님, 나머지: ePCBangGrade)
#if defined(_US)
	UINT	nNexonUserNo;
#endif	// _US
#if defined(_ID)
	char szMacAddress[MACADDRLENMAX];
	char szKey[KREONKEYMAX];
	DWORD dwKreonCN;
#endif
#if defined(PRE_ADD_DWC)
	BYTE	cCharacterAccountLevel;
#endif
};

#endif	// #if defined(PRE_MOD_SELECT_CHAR)

struct LOMAAddWaitUser			// LOMA_ADDWAITUSER
{
	int nServerID;
	UINT nAccountDBID;
};

struct LOMADelWaitUser			// LOMA_DELWAITUSER
{
	int nServerID;
	UINT nAccountDBID;
};

struct LOMADetachUser			// LOMA_DETACHUSER
{
	UINT nAccountDBID;
	BYTE cIsReconnectLogin;
	bool bIsDuplicate;
	UINT nSessionID;
#if !defined(PRE_MOD_SELECT_CHAR)
	BYTE cAccountLevel;
#ifdef _KR
	BYTE szMID[MACHINEIDMAX];
	DWORD dwGRC;
#endif //#ifdef _KR
#ifdef PRE_MOD_RESTRICT_IDENTITY_IP
	char szIP[IPLENMAX];
#endif		//#ifdef PRE_MOD_RESTRICT_IDENTITY_IP
#endif	// #if defined(PRE_MOD_SELECT_CHAR)
};

struct LOMADetachUserOther
{
	UINT nAccountDBID;
	int nLoginServerID;	
};

struct LOMADetachUserbyIP
{
	char szIP[IPLENMAX];
};

struct LOMAReqTutorialGameID	// LOMA_REQTUTORIALGAMEID
{
	UINT nAccountDBID;
	UINT nSessionID;
	int nTutorialMapIndex;
	char cTutorialGateNo;
};

struct LOMAConCountInfo			// LOMA_CONCOUNTINFO
{
public:
	LOMAConCountInfo()
	{
		Reset();
	}
	void Set(int pServerID, int pCurConCount, int pMaxConCount)
	{
		// 		DN_ASSERT(0 != nServerID,		"Invalid!");

		nServerID = pServerID;
		nCurConCount = pCurConCount;
		nMaxConCount = pMaxConCount;
	}
	void Reset()
	{
		nServerID = 0;
		nCurConCount = 0;
		nMaxConCount = 0;
	}
public:
	int nServerID;
	int nCurConCount;
	int nMaxConCount;
};

#pragma pack(pop)
