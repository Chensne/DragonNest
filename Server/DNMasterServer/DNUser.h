#pragma once

class CDNVillageConnection;
class CDNGameConnection;

class CDNUser
{
private:
	UINT m_nSessionID;
	UINT m_nAccountDBID;
	INT64 m_biCharacterDBID;
	WCHAR m_wszAccountName[IDLENMAX];
	char m_szAccountName[IDLENMAX];		// 피로도 때문에 char형으로 한개더 만듬 081211
	WCHAR m_wszCharacterName[NAMELENMAX];
	char m_szCharacterName[NAMELENMAX];	// 피로도 때문에 char형으로 한개더 만듬 081211

	int m_nLoginServerID;		//
	BYTE m_cPreVillageID;		// STATE_CHECKVILLAGE 일때 옮기기전 VillageID 찾기

	BYTE m_cVillageID;			// village찾을때 쓰임
	USHORT m_nChannelID;		// channel은 village와 Set~
	
	USHORT m_wGameID;			// Game Con찾을때
	short m_nGameServerIdx;	// ThreadIdx는 Game과 Set~	

	eUserState m_eUserState;	// 상태값 (gameid, villageid 세팅할때 상태값도 같이 바뀐다)
	ULONG m_nCheckStateTick;

	char m_szIp[IPLENMAX];		// 피로도때 필요한 접속 ip	081211
	char m_szVirtualIp[IPLENMAX];		// 피로도때 필요한 접속 ip	081211
	bool m_bAdult;				// 어른인지 아닌지 (피로도 체크때문에)

	bool m_bPCBang;				// 왠지 pcroom보다는 pcbang이 좋다! ㅋㅋㅋ
	char m_cPCBangGrade;
	int m_nAccountLevel;
#if defined(PRE_ADD_DWC)
	BYTE m_cCharacterAccountLevel;
	UINT m_nDWCTeamID;
	std::vector<INT64> m_VecDWCMemberCharacterDBIDList;	//빌리지에서 전송해준 정보를 들고 있음
#endif

	// PvP
	UINT m_uiPvPIndex;
	USHORT m_usPvPTeam;
	UINT m_uiPvPUserState;

	BYTE m_cPvPVillageID;			// PvP빌리지로 돌아올때 돌아올 PvPVillage정보
	USHORT m_nPvPVillageChannelID;
	bool m_bPvPFatigue;				// 피로도 소모 On,Off
#if defined(PRE_ADD_PVP_VILLAGE_ACCESS)
	LadderSystem::MatchType::eCode	m_MatchType;	// 래더 매치타입 저장..
#endif
#ifdef PRE_ADD_COLOSSEUM_BEGINNER
	PvPCommon::RoomType::eRoomType m_ePvPChannelType;
#endif		//#ifdef PRE_ADD_COLOSSEUM_BEGINNER

#if defined(_CH)
	int m_nFCMOnlineMin;			// online 시간(분)
#endif	// #if defined(_CH)

#if defined(PRE_ADD_PVP_TOURNAMENT)
	BYTE m_cJob;
#endif
#if defined(PRE_ADD_GAMEQUIT_REWARD)
	GameQuitReward::RewardType::eType m_eUserGameQuitRewardType;
#endif	// #if defined(PRE_ADD_GAMEQUIT_REWARD)
public:
	UINT m_nNexonUserNo;
#if defined(_KR)
	INT64 m_biNexonSessionNo;

	char m_cPCBangResult;				// eAuthResult
	char m_cPCBangAuthorizeType;		// 사용자 ip에 대한 인증결과
	char m_cPCBangOption;				// 사용자에게 보여주는 메시지
	int m_nPCBangArgument;				// 기타 더 필요한 데이터를 여기에 첨부 (ex. 시간제의 경우 남은시간, 정액제인 경우 만료날짜 등등)
	bool m_bShutdowned;
	char m_cPolicyError;
	BYTE m_szMID[MACHINEIDMAX];
	DWORD m_dwGRC;
	int m_nShutdownTime;
#endif	// #if defined(_KR)

#if defined (_JP) && defined (WIN64)
	char m_szNHNNetCafeCode[NHNNETCAFECODEMAX];			// NetCafe의 코드라고 합니다. 필요가 생길거 같아서 저장해둠
	char m_szNHNProdectCode[NHNNETCAFECODEMAX];			// NetCafe의 Product코드 라고 합니다.
#endif	// #if defined (_JP) && defined (WIN64)

#if defined(_KRAZ)
	TShutdownData m_ShutdownData;
#endif	// #if defined(_KRAZ)

#if defined(PRE_ADD_SENDLOGOUT)
	bool m_bSendLogOut;
	CSyncLock m_SendLogOutLock;
#endif

#ifdef PRE_ADD_STEAM_USERCOUNT
	bool m_bSteamUser;
#endif		//#ifdef PRE_ADD_STEAM_USERCOUNT

	ULONG m_nCreateTick;			///테스트
	int m_nRoomID;

	TPARTYID m_VillageCheckPartyID;			//마스터에서 파티는 없습니다. 게임에서 빌리지 이동시 임시적으로 유지합니다.(체크유저시 필요)

	int m_nRandomSeed;
	int m_nMapIndex;
	BYTE m_cGateNo;
	BYTE m_cGateSelect;

	int	m_nDungeonClearCount;

#if defined(PRE_ADD_MULTILANGUAGE)
	MultiLanguage::SupportLanguage::eSupportLanguage m_eSelectedLanguage;
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)
#ifdef PRE_ADD_COMEBACK
	bool m_bComebackUser;
#endif		//#ifdef PRE_ADD_COMEBACK
#if defined(PRE_ADD_GAMEQUIT_REWARD)
	bool m_bReConnectNewbieReward;
	void SetUserGameQuitRewardType(GameQuitReward::RewardType::eType eType){ m_eUserGameQuitRewardType = eType;}
	GameQuitReward::RewardType::eType GetUserGameQuitRewardType(){ return m_eUserGameQuitRewardType; }
#endif	// #if defined(PRE_ADD_GAMEQUIT_REWARD)
#if defined(_ID)
	bool m_bBlockPcCafe;				// 블럭된 PC카페
	char m_szMacAddress[MACADDRLENMAX];
	char m_szKey[KREONKEYMAX];
	DWORD m_dwKreonCN;
#endif

#if defined(PRE_ADD_REMOTE_QUEST)
	std::vector<int> m_AcceptWaitRemoteQuestList;
#endif

public:
	CDNUser(void);
	~CDNUser(void);

#if defined(PRE_MOD_SELECT_CHAR)
	bool InitUser(LOMAAddUser *pPacket, int nServerID);
	bool InitUser(GAMAAddUserList *pPacket);
	bool InitUser(VIMAAddUserList *pPacket);
#else	// #if defined(PRE_MOD_SELECT_CHAR)
	void PreInit(int nServerID, UINT nAccountDBID, int nAccountLevel, const char * pszIP);
	void Init(UINT nAccountDBID, UINT nSessionID, INT64 biCharacterDBID, WCHAR *pwszCharacterName, WCHAR *pwszAccountName, USHORT nChannelID, bool bAdult, char *pszIp, char *pszVirtualIp);
#endif	// #if defined(PRE_MOD_SELECT_CHAR)

	bool SetCheckVillageInfo(BYTE cVillageID, USHORT nChannelID);
	bool SetCheckGameInfo(USHORT nGameID, short nServerIdx);
	bool SetVillageInfo(BYTE cVillageID, USHORT nChannelID, bool bForce);
	bool SetGameInfo(USHORT nGameID, USHORT nServerIdx, bool bForce);
	void SetCheckReconnectLogin();

	CDNVillageConnection* GetCurrentVillageConnection();
	CDNGameConnection* GetCurrentGameConnection();

#if defined(_CH) && defined(_FINAL_BUILD)
	void FCMIDOnline();
	void FCMIDOffline();

	void SetFCMOnlineMin(int nOnlineMin);	// online 시간(분)
	inline int GetFCMOnlineMin() { return m_nFCMOnlineMin; }
#endif

	void SetSessionID(UINT nSessionID) { m_nSessionID = nSessionID; }
	UINT GetSessionID() { return m_nSessionID; }
	void SetAccountDBID(UINT nAccountDBID) { m_nAccountDBID = nAccountDBID; }
	UINT GetAccountDBID() { return m_nAccountDBID; }
	void SetCharacterDBID(INT64 biCharacterDBID) { m_biCharacterDBID = biCharacterDBID; }
	INT64 GetCharacterDBID() { return m_biCharacterDBID; }

	void SetCharacterName(const WCHAR *pwszCharacterName);
	WCHAR* GetCharacterName() { return m_wszCharacterName; }
	const char *GetChracterNameA() { return m_szCharacterName; }

	void SetAccountName(const WCHAR *pwszAccountName);
	WCHAR* GetAccountName() { return m_wszAccountName; }
	const char* GetAccountNameA() { return m_szAccountName; } 

	void SetAdult(bool bAdult) { m_bAdult = bAdult; }
	bool IsAdult() { return m_bAdult; }
	void SetIp(char *pszIp);
	char* GetIp() { return m_szIp; }
	void SetVirtualIp(char *pszIp);
	char* GetVirtualIp() { return m_szVirtualIp; }
	void SetMapIndex(int nMapIndex) { m_nMapIndex = nMapIndex; }
	int GetMapIndex(){ return m_nMapIndex; }
	void SetLoginServerID(int nLoginID) { m_nLoginServerID = nLoginID; }
	int GetLoginServerID() { return m_nLoginServerID; }
	BYTE GetVillageID() { return m_cVillageID; }
	BYTE GetPreVillageID() { return m_cPreVillageID; }	
	USHORT GetGameID() { return m_wGameID; }
	void SetChannelID(USHORT wChannelID) { m_nChannelID = wChannelID; }
	USHORT GetChannelID() { return m_nChannelID; }
	USHORT GetGameServerIdx() { return m_nGameServerIdx; }
	void SetUserState(eUserState eState) { m_eUserState = eState; }
	eUserState GetUserState() { return m_eUserState; }
	ULONG GetCheckStateTick() { return m_nCheckStateTick; }

	eLocationState GetLocationState();

	void SetPCGrade(char cGrade);
	void SetPCBang(bool bFlag);
	char GetPCBangGrade();
	inline bool IsPCBang() { return m_bPCBang; }
	inline int GetAccountLevel() { return m_nAccountLevel; }
#if defined(PRE_ADD_DWC)
	void SetCharacterAccountLevel(BYTE cAccountLevel) { m_cCharacterAccountLevel = cAccountLevel; }
	inline BYTE GetCharacterAccountLevel() { return m_cCharacterAccountLevel; }
	UINT GetDWCTeamID(){return m_nDWCTeamID;}
	void SetDWCTeamInfo(VIMADWCTeamMemberList *pPacket);
	void GetDWCMemberList(std::vector<INT64> &vecMemberList);
	bool AddDWCMemberList(INT64 biCharacterDBID);
	bool DelDWCMemberList(INT64 biCharacterDBID);
	void ResetDWCInfo();
#endif

#if defined(_KR)
	void SetMachineID(BYTE *pszMID, DWORD dwGRC);
#endif	// #if defined(_KR)
#if defined(_KRAZ)
	void SetShutdownData(TShutdownData &ShutdownData);
#endif	// #if defined(_KRAZ)
	// PvP
	bool bIsPvPVillage();
	bool bIsPvPLobby();
	bool bIsPvPGroupCaptain(){ return GetPvPUserState()&PvPCommon::UserState::GroupCaptain ? true : false; }
	BYTE GetPvPVillageID(){ return m_cPvPVillageID; }
	USHORT GetPvPVillageChannelID(){ return m_nPvPVillageChannelID; }
	void SetPvPVillageInfo(){ m_cPvPVillageID=m_cVillageID; m_nPvPVillageChannelID=m_nChannelID; }
	void SetPvPVillageInfo( const BYTE cVillageID, const USHORT unChannelID ){ m_cPvPVillageID=cVillageID; m_nPvPVillageChannelID=unChannelID; }
	UINT GetPvPIndex(){ return m_uiPvPIndex; }
	USHORT GetPvPTeam(){ return m_usPvPTeam; }
	UINT GetPvPUserState(){ return m_uiPvPUserState; }
	void SetPvPIndex( const UINT uiIndex ){ m_uiPvPIndex = uiIndex; }
	void SetPvPTeam( const USHORT usTeam ){ m_usPvPTeam	= usTeam; }
	void SetPvPUserState( const UINT uiState ){ m_uiPvPUserState = uiState; }

	bool bIsObserver(){ return m_usPvPTeam == PvPCommon::Team::Observer; }

	int	GetDungeonClearCount() {return m_nDungeonClearCount;}
	void SetDungeonClearCount(const int nCount) {m_nDungeonClearCount = nCount;}
	void SetPvPFatigue(bool bFatigue) { m_bPvPFatigue = bFatigue;};
	bool GetPvPFatiue() { return m_bPvPFatigue;};
#if defined(PRE_ADD_PVP_VILLAGE_ACCESS)
	void SetLadderMatchType(LadderSystem::MatchType::eCode MatchType) { m_MatchType = MatchType; };
	LadderSystem::MatchType::eCode GetLadderMatchType() { return m_MatchType;};
#endif
#ifdef PRE_ADD_COLOSSEUM_BEGINNER
	void SetPvPChannelType(BYTE cType) { m_ePvPChannelType = static_cast<PvPCommon::RoomType::eRoomType>(cType); }
	PvPCommon::RoomType::eRoomType GetPvPChannelType() { return m_ePvPChannelType; }
#endif		//#ifdef PRE_ADD_COLOSSEUM_BEGINNER

#if defined(PRE_ADD_REMOTE_QUEST)
	void SetAcceptWaitRemoteQuest(int nAcceptWaitRemoteQuestCount, int *AcceptWaitRemoteQuestList);
	void GetAcceptWaitRemoteQuest(int *nAcceptWaitRemoteQuestCount, int *AcceptWaitRemoteQuestList);
#endif
#if defined(PRE_ADD_PVP_TOURNAMENT)
	void SetUserJob(BYTE cJob) { m_cJob = cJob; }
	BYTE GetUserJob() { return m_cJob; }
#endif
#if defined(PRE_ADD_GAMEQUIT_REWARD)
	void SetGameQuitUserReward(GameQuitReward::RewardType::eType eRewardType){ m_eUserGameQuitRewardType = eRewardType; }
	GameQuitReward::RewardType::eType GetGameQuitUserReward() { return m_eUserGameQuitRewardType; }
#endif
};
