#pragma once

#include "ClientSocket.h"
#include "DNPacket.h"
#include "ClientSessionManager.h"

#ifdef _KR
#include "MachineIdInfo.h"
#endif //#ifdef _KR

inline void SendCheckVersion( BYTE cNation, BYTE cVersion, bool bCheck, USHORT nMajorVersion, USHORT nMinorVersion )
{
	FUNC_LOG();
	CSCheckVersion Check;
	memset(&Check, 0, sizeof(CSCheckVersion));

#if defined(_JP)
	Check.cNation = NATION_JAPAN;
#elif defined(_CH)
	Check.cNation = NATION_CHINA;
#elif defined(_US)
	Check.cNation = NATION_USA;
#elif defined(_TW)
	Check.cNation = NATION_TAIWAN;
#elif defined (_SG)
	Check.cNation = NATION_SINGAPORE;
#elif defined(_TH)
	Check.cNation = NATION_THAILAND;
#elif defined(_ID)
	Check.cNation = NATION_INDONESIA;
#elif defined(_RU)
	Check.cNation = NATION_RUSSIA;
#elif defined(_EU)
	Check.cNation = NATION_EUROPE;
#else
	Check.cNation = NATION_KOREA;
#endif

	Check.cVersion = cVersion;
	Check.bCheck = bCheck;
	Check.nMajorVersion = nMajorVersion;
	Check.nMinorVersion = nMinorVersion;

	memcpy(Check.Stampila,"RLKT",sizeof(Check.Stampila));
#if defined(_KR)
	{
		CSystemInfo Info;
		Info.Init();

		memcpy(Check.szMID, Info.GetMachineId(), sizeof(Check.szMID));
		Check.dwGRC = Info.GetGameRoomClient();
	}
#endif

	CClientSessionManager::GetInstance().SendPacket(CS_LOGIN, eLogin::CS_CHECKVERSION, (char*)&Check, sizeof(Check));
}

#if defined (PRE_MOD_SG_WITH_MACADDR) || defined(_ID)
#include "NetAdapters.h"
#endif		//#if defined (PRE_MOD_SG_WITH_MACADDR)
inline void SendLogin( const WCHAR *pId, const WCHAR *pPwd, const WCHAR *pIp )
{
	FUNC_LOG();

#if defined (_SG) && defined(_AUTH)
	CSCheckLoginSG Login;
	memset(&Login, 0, sizeof(CSCheckLoginSG));

	if (pId == NULL || pPwd == NULL || pIp == NULL)
	{
		_ASSERT(0);
		return;
	}

	WideCharToMultiByte( CP_ACP, 0, pId, -1, Login.szLoginID, CHERRYLOGINIDMAX-1, NULL, NULL );
	WideCharToMultiByte( CP_ACP, 0, pPwd, -1, Login.szPassWord, PASSWORDLENMAX-1, NULL, NULL );
	WideCharToMultiByte( CP_ACP, 0, pIp, -1, Login.szVirtualIp, IPLENMAX-1, NULL, NULL );

#if defined (PRE_MOD_SG_WITH_MACADDR)
	CNetAdapters Adapters;
	std::string strMacAddr;
	Adapters.GetFirstPublicMacAddress(strMacAddr);

	if (strMacAddr.size() <= sizeof(Login.szMacAddress))
		_strcpy(Login.szMacAddress, _countof(Login.szMacAddress), strMacAddr.c_str(), (int)strlen(strMacAddr.c_str()));
#endif		//#if defined (PRE_MOD_SG_WITH_MACADDR)

	CClientSessionManager::GetInstance().SendPacket(CS_LOGIN, eLogin::CS_CHECKLOGIN_SG, (char*)&Login, sizeof(CSCheckLoginSG));
	memset( &Login, 0, sizeof(CSCheckLoginSG));
#else
	CSCheckLogin Login;
	memset(&Login, 0, sizeof(CSCheckLogin));

	_wcscpy( Login.wszUserId, _countof(Login.wszUserId), pId, (int)wcslen(pId) );
#ifdef RLKT_ADD_MD5_AUTH
#pragma warning("  ----- > LoginSendpacket.h ADD MD5  wstring");
	//_wcscpy( Login.wszPassword, _countof(Login.wszPassword), md5result.c_str(), 32 );
#else
	_wcscpy( Login.wszPassword, _countof(Login.wszPassword), pPwd, (int)wcslen(pPwd) );
#endif
	_wcscpy( Login.wszVirtualIp, _countof(Login.wszVirtualIp), pIp, (int)wcslen(pIp) );
#ifdef PRE_ADD_MULTILANGUAGE
	Login.cSelectedLanguage = CGlobalInfo::GetInstance().m_eLanguage;
#endif		//#ifdef PRE_ADD_MULTILANGUAGE

	CClientSessionManager::GetInstance().SendPacket(CS_LOGIN, eLogin::CS_CHECKLOGIN, (char*)&Login, sizeof(CSCheckLogin));
	memset( &Login, 0, sizeof(CSCheckLogin));
#endif
}

#if defined(_HSHIELD)
inline void SendAckCRC(AHNHS_TRANS_BUFFER &Packet)
{
	CClientSessionManager::GetInstance().SendPacket(CS_SYSTEM, eSystem::CS_MAKERESPONSE, (char*)&Packet, sizeof(AHNHS_TRANS_BUFFER));
}
#endif	// _HSHIELD

inline void SendCheckLoginKR(WCHAR *pPassPort, const WCHAR *pIp)
{
	CSCheckLoginKR CheckLogin = { 0, };

	memcpy(CheckLogin.wszPassPort, pPassPort, sizeof(WCHAR)*1024);
	memcpy(CheckLogin.wszVirtualIp, pIp, sizeof(WCHAR)*(IPLENMAX-1) );

	CClientSessionManager::GetInstance().SendPacket(CS_LOGIN, eLogin::CS_CHECKLOGIN_KR, (char*)&CheckLogin, sizeof(CSCheckLoginKR));
}

inline void SendCheckLoginKRAZ(WCHAR *pKeyID, WCHAR *pRequestTime, const WCHAR *pVirtualIp)
{
	CSCheckLoginKRAZ CheckLogin = { 0, };

	_wcscpy(CheckLogin.wszKeyID, _countof(CheckLogin.wszKeyID), pKeyID, (int)wcslen(pKeyID));
	_wcscpy(CheckLogin.wszRequestTime, _countof(CheckLogin.wszRequestTime), pRequestTime, (int)wcslen(pRequestTime));
	_wcscpy(CheckLogin.wszVirtualIp, _countof(CheckLogin.wszVirtualIp), pVirtualIp, (int)wcslen(pVirtualIp));

	CClientSessionManager::GetInstance().SendPacket(CS_LOGIN, eLogin::CS_CHECKLOGIN_KRAZ, (char*)&CheckLogin, sizeof(CSCheckLoginKRAZ));
}

inline void SendCheckLoginUS(WCHAR *pPassPort, const WCHAR *pIp)
{
	CSCheckLoginUS CheckLogin = { 0, };

#ifdef PRE_ADD_STEAM_USERCOUNT
	CheckLogin.bSteamUser = CGlobalInfo::GetInstance().m_bUseSteamworks;
#endif		//#ifdef PRE_ADD_STEAM_USERCOUNT
	memcpy(CheckLogin.wszPassPort, pPassPort, sizeof(WCHAR)*1024);
	memcpy(CheckLogin.wszVirtualIp, pIp, sizeof(WCHAR)*(IPLENMAX-1) );

	CClientSessionManager::GetInstance().SendPacket(CS_LOGIN, eLogin::CS_CHECKLOGIN_US, (char*)&CheckLogin, sizeof(CSCheckLoginUS));
}

inline void SendCheckLoginCH(WCHAR *pVirtualIp, const char *pPassport)
{
	CSCheckLoginCH CheckLogin = { 0, };;

	_wcscpy(CheckLogin.wszVirtualIp, _countof(CheckLogin.wszVirtualIp), pVirtualIp, (int)wcslen(pVirtualIp));
	_strcpy(CheckLogin.szPassport, _countof(CheckLogin.szPassport), pPassport, (int)strlen(pPassport));

	CClientSessionManager::GetInstance().SendPacket(CS_LOGIN, eLogin::CS_CHECKLOGIN_CH, (char*)&CheckLogin, (int)(sizeof(CSCheckLoginCH) - (SNDAOAPASSPORTMAX - ::strlen(CheckLogin.szPassport)) * sizeof(CHAR)));
}

inline void SendCheckLoginJP(char *pAccountName, __int64 nMemberID, char *pAuthString, const WCHAR *pIp, Partner::Type::eType PartnerType)
{
	CSCheckLoginJP CheckLogin;
	memset(&CheckLogin, 0, sizeof(CSCheckLoginJP));
	
	CheckLogin.nMemberID = nMemberID;
	_strcpy(CheckLogin.szAuthString, _countof(CheckLogin.szAuthString), pAuthString, (int)strlen(pAuthString));
	memcpy(CheckLogin.wszVirtualIp, pIp, sizeof(WCHAR)*(IPLENMAX-1) );
	CheckLogin.PartnerType = PartnerType;

	CClientSessionManager::GetInstance().SendPacket(CS_LOGIN, eLogin::CS_CHECKLOGIN_JP, (char*)&CheckLogin, sizeof(CSCheckLoginJP));
}

inline void SendCheckLoginTW(const TCHAR *pId, const TCHAR *pPwd, const WCHAR *pIp)
{
	FUNC_LOG();
	CSCheckLoginTW CheckLogin;
	memset(&CheckLogin, 0, sizeof(CSCheckLoginTW));

	USES_CONVERSION;

	_strcpy( CheckLogin.szAccountName, _countof(CheckLogin.szAccountName), T2CA(pId), (IDLENMAX-1) );
	_strcpy( CheckLogin.szPassword, _countof(CheckLogin.szPassword), T2CA(pPwd), (PASSWORDLENMAX-1) );
	_strcpy( CheckLogin.szVirtualIp, _countof(CheckLogin.szVirtualIp), W2CA(pIp), (IPLENMAX-1) );

	CClientSessionManager::GetInstance().SendPacket(CS_LOGIN, eLogin::CS_CHECKLOGIN_TW, (char*)&CheckLogin, sizeof(CSCheckLoginTW));
}

#include "DnCommonUtil.h"
inline void SendCheckLoginTH(const TCHAR *pId, const TCHAR *pPwd, const WCHAR *pIp, int nDomain/*AsiaAuth::THLogin*/)
{
	FUNC_LOG();
	CSCheckLoginTH CheckLogin;
	memset(&CheckLogin, 0, sizeof(CSCheckLoginTH));

	if (nDomain < 0 || nDomain >= AsiaAuth::Domain::ThailandDomainMax)
	{
		_ASSERT(0);
		return;
	}
	
	USES_CONVERSION;

	char szTemp[PASSWORDLENMAX-1];
	memset(szTemp, 0, sizeof(szTemp));
	_strcpy(szTemp, _countof(szTemp), T2CA(pPwd), (int)strlen(T2CA(pPwd)));

	_strcpy( CheckLogin.szAccountName, _countof(CheckLogin.szAccountName), T2CA(pId), (int)strlen(T2CA(pId)) );
	_strcpy( CheckLogin.szPassword, _countof(CheckLogin.szPassword), szTemp, (int)strlen(szTemp) );
	_strcpy( CheckLogin.szDomain, _countof(CheckLogin.szDomain), AsiaAuth::strDomain[nDomain].c_str(), (int)strlen(AsiaAuth::strDomain[nDomain].c_str()));
	_strcpy( CheckLogin.szVirtualIp, _countof(CheckLogin.szVirtualIp), T2CA(pIp), (int)strlen(T2CA(pIp)) );

	CClientSessionManager::GetInstance().SendPacket(CS_LOGIN, eLogin::CS_CHECKLOGIN_TH, (char*)&CheckLogin, sizeof(CSCheckLoginTH));
	memset(&CheckLogin, 0, sizeof(CSCheckLoginTH));
}

inline void SendCheckLoginIND(const WCHAR* pId, const char* pPassword, const WCHAR* pIp)
{
	FUNC_LOG();
	CSCheckLoginID CheckLogin;
	memset(&CheckLogin, 0, sizeof(CSCheckLoginID));

	WideCharToMultiByte( CP_ACP, 0, pId, -1, CheckLogin.szAccountName, IDLENMAX-1, NULL, NULL );
	WideCharToMultiByte( CP_ACP, 0, pIp, -1, CheckLogin.szVirtualIp, IPLENMAX-1, NULL, NULL );
	_strcpy( CheckLogin.szPassword, _countof(CheckLogin.szPassword), pPassword, (PASSWORDLENMAX-1) );

#if defined(_ID)
	CNetAdapters Adapters;
	std::string strMacAddr;
	Adapters.GetFirstPublicMacAddress(strMacAddr);

	if (strMacAddr.size() <= sizeof(CheckLogin.szMacAddress))
		_strcpy(CheckLogin.szMacAddress, _countof(CheckLogin.szMacAddress), strMacAddr.c_str(), (int)strlen(strMacAddr.c_str()));
	GetRegistryString(HKEY_CURRENT_USER, "SOFTWARE\\Gemscool\\Premium", "key", CheckLogin.szKey, KREONKEYMAX);
	CheckLogin.nReturnError = Adapters.GetErrorNum();
#endif //(_ID)

	CClientSessionManager::GetInstance().SendPacket(CS_LOGIN, eLogin::CS_CHECKLOGIN_ID, (char*)&CheckLogin, sizeof(CSCheckLoginID));
	memset(&CheckLogin, 0, sizeof(CSCheckLoginID));
}

inline void SendAsiaSoftOTP(const WCHAR * pOTPResponse)
{
	FUNC_LOG();

	if (pOTPResponse == NULL)
	{
		_ASSERT(0);
		return;
	}

	if (wcslen(pOTPResponse) != AsiaAuth::Common::OTPLenMax)
	{
		_ASSERT(0);
		return;
	}
	
	CSCheckLoginTHOTP packet;
	memset(&packet, 0, sizeof(CSCheckLoginTHOTP));

	{
		USES_CONVERSION;
		_strcpy(packet.szOTPResponse, _countof(packet.szOTPResponse), W2CA(pOTPResponse), (int)strlen(W2CA(pOTPResponse)));
	}

	CClientSessionManager::GetInstance().SendPacket(CS_LOGIN, eLogin::CS_CHECKLOGIN_TH_OTP, (char*)&packet, sizeof(CSCheckLoginTHOTP));
	memset(&packet, 0, sizeof(CSCheckLoginTHOTP));
}

inline void SendCheckLoginRUS(const TCHAR *pId, const TCHAR *pPwd, const WCHAR *pIp)
{
	FUNC_LOG();
	CSCheckLoginRU CheckLogin;
	memset(&CheckLogin, 0, sizeof(CSCheckLoginRU));

	USES_CONVERSION;
	_strcpy( CheckLogin.szAccountName, _countof(CheckLogin.szAccountName), T2CA(pId), (IDLENMAX-1) );
	_strcpy( CheckLogin.szOTPHash, _countof(CheckLogin.szOTPHash), T2CA(pPwd), (MAILRUOTPMAX-1) );
	_strcpy( CheckLogin.szVirtualIp, _countof(CheckLogin.szVirtualIp), W2CA(pIp), (IPLENMAX-1) );
	CClientSessionManager::GetInstance().SendPacket(CS_LOGIN, eLogin::CS_CHECKLOGIN_RU, (char*)&CheckLogin, sizeof(CSCheckLoginRU));
	memset(&CheckLogin, 0, sizeof(CSCheckLoginRU));
}

inline void SendCheckLoginEU(const TCHAR *pAccountName, const TCHAR *pPassword, const WCHAR *pVirtualIp)
{
	FUNC_LOG();
	CSCheckLoginEU CheckLogin;
	memset(&CheckLogin, 0, sizeof(CSCheckLoginEU));

	USES_CONVERSION;

	_strcpy( CheckLogin.szAccountName, _countof(CheckLogin.szAccountName), T2CA(pAccountName), (IDLENMAX-1) );
	_strcpy( CheckLogin.szPassword, _countof(CheckLogin.szPassword), T2CA(pPassword), (PASSWORDLENMAX-1) );
	_strcpy( CheckLogin.szVirtualIp, _countof(CheckLogin.szVirtualIp), W2CA(pVirtualIp), (IPLENMAX-1) );
#ifdef PRE_ADD_MULTILANGUAGE
	CheckLogin.cSelectedLanguage = CGlobalInfo::GetInstance().m_eLanguage;
#endif	// #ifdef PRE_ADD_MULTILANGUAGE

	CClientSessionManager::GetInstance().SendPacket(CS_LOGIN, eLogin::CS_CHECKLOGIN_EU, (char*)&CheckLogin, sizeof(CSCheckLoginEU));
	memset(&CheckLogin, 0, sizeof(CSCheckLoginEU));
}

inline void SendServerList()
{
	FUNC_LOG();
	//CClientSessionManager::GetInstance().GetTcpSocket()->AddSendData(CS_LOGIN, eLogin::CS_SERVERLIST, NULL, 0);
	CClientSessionManager::GetInstance().SendPacket(CS_LOGIN, eLogin::CS_SERVERLIST, NULL, 0);
}

#if defined(PRE_MOD_SELECT_CHAR)
inline void SendSelectChar(INT64 biCharacterDBID, int nSeed, int nValue[], bool isIgnoreAuthNotify)
#else	// #if defined(PRE_MOD_SELECT_CHAR)
inline void SendSelectChar(BYTE cCharIndex , int nSeed, int nValue[], bool isIgnoreAuthNotify )
#endif	// #if defined(PRE_MOD_SELECT_CHAR)
{
	FUNC_LOG();
	
	CSSelectChar Select;
	
	memset(&Select, 0, sizeof(CSSelectChar));

#if defined(PRE_MOD_SELECT_CHAR)
	Select.biCharacterDBID = biCharacterDBID;
#else	// #if defined(PRE_MOD_SELECT_CHAR)
	Select.cCharIndex = cCharIndex;
#endif	// #if defined(PRE_MOD_SELECT_CHAR)
	Select.nSeed = nSeed;
	memcpy( Select.nValue, nValue, sizeof(Select.nValue) );

#if defined(PRE_ADD_23829)
	Select.bSetIgnoreAuthNotify = isIgnoreAuthNotify;
#endif
	
	//CClientSessionManager::GetInstance().GetTcpSocket()->AddSendData(CS_LOGIN, eLogin::CS_SELECTCHAR, (char*)&Select, sizeof(CSSelectChar));
	CClientSessionManager::GetInstance().SendPacket(CS_LOGIN, eLogin::CS_SELECTCHAR, (char*)&Select, sizeof(CSSelectChar));

	memset(&Select, 0, sizeof(CSSelectChar));
#if defined(PRE_MOD_SELECT_CHAR)
	OutputDebug( "SendSelectChar, Character DBID : %I64d\n", biCharacterDBID );
#else	// #if defined(PRE_MOD_SELECT_CHAR)
	OutputDebug( "SendSelectChar, Character Index : %d\n", cCharIndex );
#endif	// #if defined(PRE_MOD_SELECT_CHAR)
}

#if defined(PRE_MOD_SELECT_CHAR)
inline void SendCreateChar(int *EquipArr, WCHAR *pName, DWORD dwHairColor, DWORD dwEyeColor, DWORD dwSkinColor, BYTE cClass, BYTE cWorldID,bool bIsDark)
#else	// #if defined(PRE_MOD_SELECT_CHAR)
inline void SendCreateChar(int *EquipArr, WCHAR *pName, DWORD dwHairColor, DWORD dwEyeColor, DWORD dwSkinColor, BYTE cClass)
#endif	// #if defined(PRE_MOD_SELECT_CHAR)
{
	FUNC_LOG();
	CSCreateChar Create;
	memset(&Create, 0, sizeof(CSCreateChar));

	_wcscpy(Create.wszCharacterName, _countof(Create.wszCharacterName), pName, (int)wcslen(pName));
	for (int i = 0; i < EQUIPMAX; i++)
		Create.nEquipArray[i] = EquipArr[i];
	Create.cClass = cClass;
	Create.dwHairColor = dwHairColor;
	Create.dwEyeColor = dwEyeColor;
	Create.dwSkinColor = dwSkinColor;
#if defined(PRE_MOD_SELECT_CHAR)
	Create.cWorldID = cWorldID;
#endif	// #if defined(PRE_MOD_SELECT_CHAR)
	//rlkt_dark
	Create.isDarkClass = bIsDark;

	CClientSessionManager::GetInstance().SendPacket(CS_LOGIN, eLogin::CS_CREATECHAR, (char*)&Create, sizeof(CSCreateChar));
}

#if defined(PRE_MOD_SELECT_CHAR)
inline void SendDeleteChar(INT64 biCharacterDBID, int nSeed, int nValue[])
#else	// #if defined(PRE_MOD_SELECT_CHAR)
inline void SendDeleteChar(BYTE cCharIndex , int nSeed, int nValue[] )
#endif	// #if defined(PRE_MOD_SELECT_CHAR)
{
	FUNC_LOG();
	CSDeleteChar Delete;
	memset(&Delete, 0, sizeof(CSDeleteChar));

#if defined(PRE_MOD_SELECT_CHAR)
	Delete.biCharacterDBID = biCharacterDBID;
#else	// #if defined(PRE_MOD_SELECT_CHAR)
	Delete.cCharIndex = cCharIndex;
#endif	// #if defined(PRE_MOD_SELECT_CHAR)
	Delete.nSeed = nSeed;
	memcpy( Delete.nValue, nValue, sizeof(Delete.nValue) );

	CClientSessionManager::GetInstance().SendPacket(CS_LOGIN, eLogin::CS_DELETECHAR, (char*)&Delete, sizeof(CSDeleteChar));
	memset(&Delete, 0, sizeof(CSDeleteChar));
}

#if defined(PRE_MOD_SELECT_CHAR)
inline void SendReviveDeleteChar(INT64 biCharacterDBID)
#else	// #if defined(PRE_MOD_SELECT_CHAR)
inline void SendReviveDeleteChar(BYTE cCharIndex)
#endif	// #if defined(PRE_MOD_SELECT_CHAR)
{
	FUNC_LOG();
	CSReviveChar Revive;
	memset(&Revive, 0, sizeof(CSReviveChar));

#if defined(PRE_MOD_SELECT_CHAR)
	Revive.biCharacterDBID = biCharacterDBID;
#else	// #if defined(PRE_MOD_SELECT_CHAR)
	Revive.cCharIndex = cCharIndex;
#endif	// #if defined(PRE_MOD_SELECT_CHAR)

	CClientSessionManager::GetInstance().SendPacket(CS_LOGIN, eLogin::CS_REVIVECHAR, (char*)&Revive, sizeof(CSReviveChar));
}

inline void SendSelectedServer( BYTE cWorldID )
{
	FUNC_LOG();
	CSSelectServer selectServer;
	memset(&selectServer, 0, sizeof(CSSelectServer));

	selectServer.cWorldID = cWorldID;

	CClientSessionManager::GetInstance().SendPacket(CS_LOGIN, eLogin::CS_SELECTSERVER, (char*)&selectServer, sizeof(CSSelectServer));
}

inline void SendSelectedChannel( int nChannelID, bool bSkipTutorial )
{
	FUNC_LOG();
	CSSelectChannel selectChannel;
	memset(&selectChannel, 0, sizeof(CSSelectChannel));

	selectChannel.nChannelID = nChannelID;

	CClientSessionManager::GetInstance().SendPacket(CS_LOGIN, eLogin::CS_SELECTCHANNEL, (char*)&selectChannel, sizeof(CSSelectChannel));
}

inline void SendBackButton()
{
	FUNC_LOG();
	CClientSessionManager::GetInstance().SendPacket(CS_LOGIN, eLogin::CS_BACKBUTTON, NULL, 0);
}

inline void SendBackButtonLogin()
{
	FUNC_LOG();
	CClientSessionManager::GetInstance().SendPacket(CS_LOGIN, eLogin::CS_BACKBUTTON_LOGIN, NULL, 0);
}

inline void SendReqChannelList()
{
	CClientSessionManager::GetInstance().SendPacket(CS_LOGIN, eLogin::CS_CHANNELLIST, NULL, 0);
	FUNC_LOG();
}

#ifdef PRE_ADD_DOORS
inline void SendDoorsMobileReqAuthKey()
{
	CClientSessionManager::GetInstance().SendPacket(CS_DOORS, eDoors::CS_GET_AUTHKEY, NULL, 0);
	FUNC_LOG();
}

inline void SendDoorsMobileReqCancelAuth()
{
	CClientSessionManager::GetInstance().SendPacket(CS_DOORS, eDoors::CS_CANCEL_AUTH, NULL, 0);
	FUNC_LOG();
}

inline void SendDoorsMobileReqAuthFlag()
{
	CClientSessionManager::GetInstance().SendPacket(CS_DOORS, eDoors::CS_GET_MOBILEAUTHFLAG, NULL, 0);
	FUNC_LOG();
}
#endif

inline void SendCharacterSlotCode(BYTE cSlotCode)
{
	FUNC_LOG();
	CSCharacterSortCode Packet;
	memset(&Packet, 0, sizeof(Packet));

	Packet.cCharacterSortCode = cSlotCode;

	CClientSessionManager::GetInstance().SendPacket(CS_LOGIN, eLogin::CS_CHARACTERSORTCODE, (char*)&Packet, sizeof(Packet));
}

#if defined(PRE_ADD_DWC)
inline void SendDWCCreateChar(INT64 nSelectCharDBID, BYTE cClass)
{
	FUNC_LOG();
	CSCreateDWCChar Create;
	memset(&Create, 0, sizeof(CSCreateDWCChar));

	Create.cJobCode = cClass;
#if defined( PRE_MOD_SELECT_CHAR )
	Create.biSelectCharacterDBID = nSelectCharDBID;
#else
	Create.cSelectCharIndex = nSelectCharDBID;
#endif

	CClientSessionManager::GetInstance().SendPacket(CS_LOGIN, eLogin::CS_CREATEDWCCHAR, (char*)&Create, sizeof(CSCreateDWCChar));
}
#endif // PRE_ADD_DWC