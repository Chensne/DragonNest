#pragma once
 
#include "Connection.h"
#include "Util.h"
#include "MemPool.h"
#ifdef _GPK
#include "CryptPacket.h"
class CCryptoKey;
#endif

class CDNUserConnection: public CConnection, public TBoostMemoryPool<CDNUserConnection>
{
private:
#if defined(PRE_MOD_SELECT_CHAR)
	typedef std::map<INT64, TDBListCharData> TMapCharacterList;
	TMapCharacterList m_MapCharacterList;

	typedef std::map<int,int> TMapWorldCharCount;
	TMapWorldCharCount m_MapWorldCharCount;	// WorldID, CharCount
#else	// #if defined(PRE_MOD_SELECT_CHAR)
	TDBListCharData m_CharDataArray[CHARCOUNTMAX];		// �ѹ� ������ ĳ���� ����ũ ���̵� �����ϱ�
#endif	// #if defined(PRE_MOD_SELECT_CHAR)
	TMapChannel m_ChannelList;

#if defined(PRE_MOD_SELECT_CHAR)
	int m_nCreateCharacterSlotMaxPerAccount;		// ������ �ִ���� ������ ĳ���� ���� (GlobalWeight 1022)
	int m_nDefaultCreateCharacterSlotPerAccount;	// ������ �⺻���� ĳ���� ���� (GlobalWeight 1023)
	char m_cAvailableCharacterSlotCount;			// ���� �� �ִ� �ִ� ĳ����
#else	// #if defined(PRE_MOD_SELECT_CHAR)
	char m_cCharacterLimitCount;	// ������ִ� �ִ밳�� (�⺻�� 4 + ������..)
	char m_cCharCountMax;	// �������ִ� ĳ���� �ִ밳��
#endif	// #if defined(PRE_MOD_SELECT_CHAR)

	UINT m_nNexonSN;
	UINT m_nNexonUserNo;	// _US
	Partner::Type::eType m_PartnerType;

	UINT m_nAccountDBID;		// ���� ����ũ ��ȣ (DB Account Table���� ������ ��ȣ)
	WCHAR m_wszAccountName[IDLENMAX];	// ����

	char m_cWorldID;			// �����ִ� ���� ��ȣ
	char m_cWaitWorldID;		// ��� ���� ��ȣ
	int m_nWorldSetID;
	int m_nWaitWorldSetID;

	bool m_bPCBang;
	char m_cPCBangGrade;

	LONG m_dwSendInsideDisconnect;

#if defined(_HSHIELD)
	AHNHS_CLIENT_HANDLE	m_hHSClient;	// [Ȯ�� ��������] Ŭ���̾�Ʈ ���� ��ü
	DWORD m_dwCheckLiveTick;			// live check
	DWORD m_dwHShieldResponseTick;		// CRC������ ������������� �ð� üũ

	bool m_bUseHshield;

#elif defined(_GPK)
	const unsigned char *m_pCode;
	int m_nCodeLen;
	int m_nCodeIndex;
	volatile DWORD m_dwCheckGPKTick;		// live check
	CCryptoKey m_CryptoKey;

#endif	// _HSHIELD
#if defined(PRE_ADD_LOGIN_USERCOUNT)
	DWORD m_dwKickTick;					// ���� ���� Tick
#endif

	DWORD m_dwLastMessageTick;
	BYTE m_cLastMainCmd;
	BYTE m_cLastSubCmd;
#if defined( PRE_LOGINDUMP_LOG )
	CSReconnectLogin m_ReconnectData;
	int m_nReconnectLoginStep;
#endif

	WCHAR m_wszVirtualIp[IPLENMAX];
	char m_szVirtualIp[IPLENMAX];

	bool m_bIsSetSecondAuthPW;
	bool m_bIsSetSecondAuthLock;
	__time64_t	m_tSecondAuthResetDate;
#ifdef PRE_ADD_23829
	bool m_bPassSecondAuth;
#endif	//#ifdef PRE_ADD_23829
#ifdef PRE_ADD_IGNORESECONDAUTH_EKEYECARD
	bool m_bCompletlyPassSecondAuth;		//�߱����� ��Ű ��ī�� ���� 3�� �Ⱥ��� üũ ���¿����� 2������ ���� ��ư ����
#endif

#if defined (_SG)
	CherryAuth::TCherryAuthInfo m_CherryAuthInfo;
#endif

#if defined (_TH)
	AsiaAuth::TAsiaSoftAuthInfo m_AsiaAuthInfo;
#endif		//#if defined (_TH)

	//Parse
#if defined(_HSHIELD)
	int RequestHShieldResponse(AHNHS_TRANS_BUFFER *pData, int nLen);
#elif defined(_GPK)
	int RequestGPKData(CSGPKData *pData, int nLen);
	int RequestGPKAuthData(CSGPKAuthData *pData, int nLen);
#endif	// _GPK

#if defined(_JP)
	int GetJpnLoginMid(int nRet);
#endif	// #if defined(_JP)
#if defined(_SG)
	int ConvertCherryCode2DNCode(int nCherryCode);
#endif	// #if defined(_SG)

	int OnRecvLoginMessage(int nSubCmd, const char * pData, int nLen);
	int OnRecvSystemMessage(int nSubCmd, const char * pData, int nLen);
	int OnRecvGameOptionMessage( int nSubCmd, const char* pData, int nLen );
#ifdef PRE_ADD_DOORS
	int OnRecvDoorsMessage(int nSubCmd, const char * pData, int nLen);
#endif		//#ifdef PRE_ADD_DOORS

	int RequestCheckVersion(CSCheckVersion *pVersion, int nLen);

	int RequestCheckLogin(CSCheckLogin *pLogin, int nLen);
	int RequestCheckLoginKR(CSCheckLoginKR *pLogin, int nLen);
	int RequestCheckLoginKRAZ(CSCheckLoginKRAZ *pLogin, int nLen);
	int RequestCheckLoginUS(CSCheckLoginUS *pLogin, int nLen);
	int RequestCheckLoginCH(CSCheckLoginCH *pLogin, int nLen);
	int RequestCheckLoginJP(CSCheckLoginJP *pLogin, int nLen);
	int RequestCheckLoginTW(CSCheckLoginTW *pLogin, int nLen);
	int RequestCheckLoginSG(CSCheckLoginSG *pLogin, int nLen);
	int RequestCheckLoginTH(CSCheckLoginTH *pLogin, int nLen);
	int RequestCheckLoginTHOTP(CSCheckLoginTHOTP * pLogin, int nLen);
	int RequestCheckLoginID(CSCheckLoginID* pLogin, int nLen);
	int RequestCheckLoginRU(CSCheckLoginRU* pLogin, int nLen);
	int RequestCheckLoginEU(CSCheckLoginEU* pLogin, int nLen);

	int RequestServerList(int nLen);
	int RequestSelectServer(CSSelectServer *pSelectServer, int nLen);
	int RequestSelectCharacter(CSSelectChar *pSelectChar, int nLen);
	int RequestCreateCharacter(CSCreateChar *pCreate, int nLen);
	int RequestDeleteCharacter(CSDeleteChar *pDelete, int nLen);
	int RequestSelectChannel(CSSelectChannel *pSelectChannel, int nLen);
	int RequestBackButton(int nLen);
	int RequestReconnectLogin(CSReconnectLogin *pReconnect, int nLen);
	int RequestChannelList(int nLen);
	int RequestReviveCharacter( CSReviveChar* pRevive, int nLen );

	int RequestUpdateSecondAuthPassword( const char* pData, int nLen );
	int RequestInitSecondAuth( const char* pData, int nLen );

#ifdef PRE_ADD_DOORS
	int RequestGetDoorsAuthKey(const char * pData, int nLen);
	int RequestCancelDoorsAuth(const char * pData, int nLen);
	int RequestGetAuthFlag(const char * pData, int nLen);
#endif		//#ifdef PRE_ADD_DOORS
	int RequestCharacterSortCode(CSCharacterSortCode *pSortCode, int nLen);
#if defined( PRE_ADD_DWC )
	int RequestCreateDWCCharacter(CSCreateDWCChar *pDWCCreate, int nLen);
#endif // #if defined( PRE_ADD_DWC )

#ifdef PRE_ADD_23829
	int InitSecondAuth();
#endif	//#ifdef PRE_ADD_23829

#if defined(PRE_MOD_SELECT_CHAR)
	TDBListCharData* GetCharacterData(INT64 biCharacterDBID);
#endif	// #if defined(PRE_MOD_SELECT_CHAR)

public:
	BYTE m_cVillageID;					// �����ִ� �� ��ȣ (Village �� ������ ����)
	UINT m_nChannelID;

#if defined(PRE_MOD_SELECT_CHAR)
	TDBListCharData m_SelectCharacterData;	// ������ ������
#else	// #if defined(PRE_MOD_SELECT_CHAR)
	BYTE m_cSelectCharIndex;			// ĳ���� ���ý� ������ �ε��� (DB Char Table���� ������ ��ȣ)
#endif	// #if defined(PRE_MOD_SELECT_CHAR)
	BYTE m_cGender;						// ����
	TGameOptions m_GameOptions;			// ���ӿɼ�

	bool m_bAdult;						// ����� �ƴ��� (�߱��Ƿε�)
	char m_cAge;						// ���� (Nexon ����)
	BYTE m_cDailyCreateCount;			// ���� ĳ���� �������� Ƚ��
	char m_cAccountLevel;				// ���� ���� (���, ������, �Ϲ� ���� ���)
	int m_nPrmInt1;						// �Ķ���� 1 (������ �ٸ���)
										/*
											�ѱ� - Nexon SN ����
											�߱� - ���� ���� �÷��� ���� (eChSndaAuthFlag)
										*/
	char m_cLastServerType;				// �ֱ� ���� Ÿ�� (eServerType / 0 : �α���, 2 : ������, 3 : ����)

	BYTE m_cRegion;						// �븸 �������� �ڵ�(95-ȫ��, 207-Ÿ�̿�)

#if defined(PRE_ADD_LOGIN_USERCOUNT)
	bool m_bSummitUserCount;			// ���� ī��Ʈ ���� ����
#endif

#if defined(PRE_ADD_SENDLOGOUT)
	bool m_bSendLogOut;
	CSyncLock m_SendLogOutLock;
#endif

	int m_nTutorialMapIndex;			// Ʃ�丮�� �� �ε���
	char m_cTutorialGateNo;				// Ʃ�丮�� ����Ʈ

	char m_cUserState;					// �������� (eUserState)
#if !defined(PRE_MOD_SELECT_CHAR)
	bool m_bMasterAddUser;				// �����Ϳ� ���� �߰��� ���� ��������
#endif	// #if defined(PRE_MOD_SELECT_CHAR)

	bool m_bReconnected;				// ĳ���� ���� ���� ���� ���� VI, GA �κ��� �翬��� �������� ���� (true:����/false:���� �α���)
	bool m_bCertified;					// �����Ϸ� ���� (QUERY_CHECKAUTH �� ������ ���� false �� �����ؾ� ��)
	INT64 m_biCertifyingKey;			// ����Ű

	BYTE m_szMID[MACHINEIDMAX];
	DWORD m_dwGRC;

#ifdef PRE_ADD_MULTILANGUAGE
	MultiLanguage::SupportLanguage::eSupportLanguage m_eSelectedLanguage;
#endif		//#ifdef PRE_ADD_MULTILANGUAGE

#ifdef PRE_ADD_COMEBACK
	bool m_bComebackUser;
#endif		//#ifdef PRE_ADD_COMEBACK
#if defined( PRE_ADD_GAMEQUIT_REWARD )
	bool m_bReConnectNewbieReward;
	__time64_t m_tAccountRegistDate;
	GameQuitReward::RewardType::eType m_eUserGameQuitReward;	// GameQuitReward::RewardType �� ��Ī�˴ϴ�
#endif	// #if defined( PRE_ADD_GAMEQUIT_REWARD )
#if defined(_ID)
	char m_szMacAddress[MACADDRLENMAX];
	char m_szKey[KREONKEYMAX];
	DWORD m_dwKreonCN;
#endif
#ifdef PRE_ADD_DOORS
	bool m_bMobileAuthentication;
#endif		//#ifdef PRE_ADD_DOORS
	BYTE m_cCharacterSortCode;

#if defined(_KRAZ)
	TShutdownData m_ShutdownData;
#endif	// #if defined(_KRAZ)

#ifdef PRE_ADD_STEAM_USERCOUNT
	bool m_bSteamUser;
#endif		//#ifdef PRE_ADD_STEAM_USERCOUNT

public:
	CDNUserConnection(void);
	virtual ~CDNUserConnection(void);

	void Clear();
	void DoUpdate(DWORD CurTick);

	int MessageProcess(int nMainCmd, int nSubCmd, char *pData, int nLen);
	void SendInsideDisconnectPacket(wchar_t *pIdent);	// IN_DISCONNECT ó��

#if defined(PRE_MOD_SELECT_CHAR)
	void SetAvailableCharacterSlotCount();
	char GetAvailableCharacterSlotCount();

	int GetEmptyCharacterSlotCount();

#else	// #if defined(PRE_MOD_SELECT_CHAR)
	char GetCharacterLimitCount() const { return m_cCharacterLimitCount; }
	char GetCharacterCountMax() { return m_cCharCountMax; }
	void SetCharacterCountMax(BYTE cMaxCount);
#endif	// #if defined(PRE_MOD_SELECT_CHAR)
	void ClearCharacterData();
#if defined(PRE_MOD_SELECT_CHAR)
	void SetSelectCharacterData(INT64 biCharacterDBID);
	void DelCharacterData(INT64 biCharacterDBID);

#else	// #if defined(PRE_MOD_SELECT_CHAR)
	void SetCharData(char cCharIndex, TDBListCharData &CharData);
	void DelCharData(char cCharIndex);
	TDBListCharData* SelectCharData();
	TDBListCharData* GetCharacterData(char cCharIndex);
	int GetEmptyCharIndex();
	INT64 GetCharacterDBID( BYTE cIndex );
#endif	// #if defined(PRE_MOD_SELECT_CHAR)
	INT64 GetSelectCharacterDBID();	// ������ ĳ���� dbid
	WCHAR* GetSelectCharacterName();
	int GetSelectCharacterLastMapIndex();	// ������ ĳ������ LastMapIndex
	int GetSelectCharacterMapIndex();
	BYTE GetSelectCharacterJob();
#if defined(PRE_ADD_DWC)
	BYTE GetSelectCharacterAccountLevel();
	int GetDWCWorldSetID();
	bool IsExistDWCCharacter();
	bool CheckDWCLimitCharacterLevel( BYTE cLimitCharacterLevel );
#endif

	char GetUserState() const { return m_cUserState; }

	TChannelInfoEx* GetChannelInfo(int nChannelID);
	TChannelInfoEx* GetOtherChannelInfo(int nChannelID);

	UINT GetNexonSN() { return m_nNexonSN; }
	UINT GetNexonUserNo() { return m_nNexonUserNo; }
	void SetNexonUserNo(UINT nUserNo) { m_nNexonUserNo = nUserNo; }
#ifdef _CH
	bool CheckSndaAuthFlag(eChSndaAuthFlag pSndaAuthFlag) const { return((static_cast<DWORD>(m_nPrmInt1) & pSndaAuthFlag)?(true):(false)); }
#endif //#ifdef _CH
	void SetAccountDBID(UINT nAccountDBID) { m_nAccountDBID = nAccountDBID; }
	UINT GetAccountDBID() { return m_nAccountDBID; }
#if !defined(PRE_MOD_SELECT_CHAR)
	TDBListCharData* GetCharDataArray(){ return  m_CharDataArray; }
#endif	// #if defined(PRE_MOD_SELECT_CHAR)
	void SetAdult(bool bFlag) { m_bAdult = bFlag; }
	bool GetAdult() { return m_bAdult; }
	void SetAge(char cAge) { m_cAge = cAge; }
	char GetAge() const { return m_cAge; }
	void SetDailyCreateCount(char cDailyCreateCount) { m_cDailyCreateCount = cDailyCreateCount; }
	void DelDailyCreateCount();
	BYTE GetDailyCreateCount() const { return m_cDailyCreateCount; }
	int GetPrmInt1() const { return m_nPrmInt1; }
	void SetPrmInt1(int nPrmInt1) { m_nPrmInt1 = nPrmInt1; }
	eServerType GetLastServerType() const { return(static_cast<eServerType>(m_cLastServerType)); }
	void SetWorldID(char cWorldID);
	char GetWorldID() { return m_cWorldID; }
	void SetAccountName(const WCHAR *pName) { wcsncpy(m_wszAccountName, pName, IDLENMAX); }
	WCHAR* GetAccountName() { return m_wszAccountName; }
	void SetAccountLevel(char cAccountLevel) { m_cAccountLevel = cAccountLevel; }
	char GetAccountLevel() { return m_cAccountLevel; }
	WCHAR* GetVirtualIpW() { return m_wszVirtualIp; }
	char* GetVirtualIp() { return m_szVirtualIp; }
	bool IsReconnected() { return m_bReconnected; }
	bool IsCertified() { return m_bCertified; }
	void ResetCertified() { m_bCertified = false; }
	INT64 GetCertifyingKey() { return m_biCertifyingKey; }
	void SetPCBang(bool bFlag) { m_bPCBang = bFlag; }
	bool IsPCBang() { return m_bPCBang; }
	void SetPCBangGrade(char cGrade) { m_cPCBangGrade = cGrade; }
	char GetPCBangGrade() { return m_cPCBangGrade; }
	void SetWaitWorldID(char cWaitWorld) { m_cWaitWorldID = cWaitWorld; }
	char GetWaitWorldID() { return m_cWaitWorldID; }
	void SetWorldSetID(int nWorldSetID) { m_nWorldSetID = nWorldSetID; }
	int GetWorldSetID() { return m_nWorldSetID; }
	void SetWaitWorldSetID(int nWorldSetID) { m_nWaitWorldSetID = nWorldSetID; }
	int GetWaitWorldSetID() { return m_nWaitWorldSetID; }
	Partner::Type::eType GetPartnerType() const { return m_PartnerType; }
#if defined (_TH)
	bool SetAsiaAuthInfo(AsiaAuth::TAsiaSoftAuthInfo &AuthInfo);
#endif		//#if defined (_TH)

#if defined(PRE_MOD_SELECT_CHAR)
	void SetWaitWorldIDBySelectCharacter();
	BYTE GetWorldIDByCharacterDBID(INT64 biCharacterDBID);
	int GetWorldSetIDByCharacterDBID(INT64 biCharacterDBID);

	bool IsValidCharacter(INT64 biCharacterDBID);
#else	// #if defined(PRE_MOD_SELECT_CHAR)
	bool IsValidCharacterIndex(char cCharIndex);
#endif	// #if defined(PRE_MOD_SELECT_CHAR)
	void CheckVillageGameInfo();

	void PCBangResult(char cResult);

	void CheckLoginCH(char cIsLimited, char cIsProtected);
	void CheckLoginTW();
	void CheckLoginTH();
	void CheckLoginEU(int nResult, const char *pszAccountName);
	void CheckLoginKRAZ(char cResult, const WCHAR *pReservedFields, const WCHAR *pwszAccountName);

	//Master
	void SendDelUser();

	// -> User
	// System
	void SendCompleteDetachMsg(int nRetCode, const WCHAR * pwszDetachReason);
	void SendVillageInfo(int nRet, UINT nAccountDBID, INT64 biCertifyingKey);
	void SendGameInfo(ULONG nIP, USHORT nPort, USHORT nTcpPort, int nRet, UINT nAccountDBID, INT64 biCertifyingKey);
	void SendReconnectLogin(int nRet, UINT nAccountDBID, INT64 biCertifyingKey);

	// Login
	void SendCheckVersion(int nRet);
	void SendCheckLogin(int nRet, const char* pInfoURL=NULL);
	void SendServerList( std::map<int,int>& mWorldUserCount );
	void SendCharList(int nRet);	// DB Query���� Send�ϴ°�
	void SendCharacterList(bool bProcessSecondAuth);	// DB Query�ϰ� Send
	void SendChannelList( int nRet=ERROR_NONE, BYTE cFailCount=0 );
	void SendSelectChar(int nRet);
	void SendCreateChar(WCHAR *pName, int nRet);
#if defined(PRE_MOD_SELECT_CHAR)
	void SendDeleteChar(INT64 biCharacterDBID, int nRet, BYTE cFailCount=0);
	void SendReviveChar(INT64 biCharacterDBID, int nRet);
#else	// #if defined(PRE_MOD_SELECT_CHAR)
	void SendDeleteChar(BYTE cCharIndex, int nRet, BYTE cFailCount=0 );
	void SendReviveChar( BYTE cCharIndex, int nRet );
#endif	// #if defined(PRE_MOD_SELECT_CHAR)
	void SendTutorialMapInfo(int nMapIndex, char cGateNo);
	void SendWaitUser(BYTE cWorldID, UINT nWaitNumber, ULONG nEstimateTime);
	void SendBackButtonLogin();
	void SendCheckBlock(__time64_t serverTime, TRestraintForAccountAndCharacter* pRestraint );
#if defined (_TH)
	void SendAsiaAuthRequestOTP();
	void SendAsiaOTPResult(int nRet);
#endif		//#if defined (_TH)
#if defined( PRE_ADD_DWC )
	void SendCreateDWCChar(int nRet);
	void SendDWCChannelInfo();
#endif // #if defined( PRE_ADD_DWC )

	// GameOption
	void SendSecondAuthInfo();
	void SendResInitSecondAuth( int iRet, BYTE cFailCount=0 );

#if defined( PRE_ADD_DWC )
	int QueryCharacterListAndSecondAuth( bool bProcessSecondAuth, INT64 biDWCCharacterDBID );
#else // #if defined( PRE_ADD_DWC )
	int QueryCharacterListAndSecondAuth( bool bProcessSecondAuth );
#endif // #if defined( PRE_ADD_DWC )
	int QueryValidataSecondAuthPassphrase( WCHAR* pwszPW, BYTE& cFailCount, bool bSetIgnoreSecondAuth = false );

#if defined(_HSHIELD)
	void SendMakeRequest();
#elif defined(_GPK)
	void SendGPKCode();
	void SendGPKData();

	void SendGPKAuthData();
#endif	// _HSHIELD

	bool CheckAbuserCharLength();
	void QueryAddRestraint(wchar_t* pwszReason);

#ifdef PRE_MOD_RESTRICT_IDENTITY_IP
	void QueryAddRestraintIP(const WCHAR * pwszReason, const char * pszIP);
#endif		//#ifdef PRE_MOD_RESTRICT_IDENTITY_IP

	int ConvertNexonAuthResult(int nAuthResult);
	void CheckPCBangIp();
#if defined(_ID)
	const char* GetMacAddress() { return m_szMacAddress;}
	const char* GetKey() { return m_szKey;}
	const DWORD GetKreonCN() { return m_dwKreonCN;}
#endif

#ifdef PRE_ADD_DOORS
	//Doors
	void SendMobileAuthenticationKey(const char * szKeys, int nRetCode, bool bFlag);
	void SendMobileCancelAuthentication(int nRetCode);
	void SendMobileAuthenticationFlag(bool bFlag);
#endif		//#ifdef PRE_ADD_DOORS

private:
	int InitWrap(int nRecvLen, int nSendLen, int nCompLen = 0);
};
