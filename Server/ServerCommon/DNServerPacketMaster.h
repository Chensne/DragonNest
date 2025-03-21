#pragma once

#pragma pack(push, 1)

//--------------------------------------------------------------
//	Master Packet
//--------------------------------------------------------------

// 공통 (Master -> 어떤 서버든...)
struct MARegist				// MAVI_REGISTWORLDINFO, MAGA_REGIST
{
	char cWorldSetID;
	BYTE cGameID;
};

struct MAReqUserList		// MAVI_REQUSERLIST
{
	int nRetCode;
};

struct MAPrivateChat			// MAVI_PRIVATECHAT, MAGA_PRIVATECHAT
{
	UINT nAccountDBID;
	WCHAR wszFromCharacterName[NAMELENMAX];
	char cType;		// GM이냐 아니냐.. (이렇게까지해야해? -_-;)
	short wChatLen;
	int nRet;
	WCHAR wszChatMsg[CHATLENMAX];
};

struct MAChat					// MAVI_CHAT, MAGA_CHAT
{
	UINT nAccountDBID;
	char cType;
	WCHAR wszFromCharacterName[NAMELENMAX];
	short wChatLen;
	WCHAR wszChatMsg[CHATLENMAX];
};

struct MAZoneChat			//MAVI_ZONECHAT
{
	int nMapIdx;
	WCHAR wszFromCharacterName[NAMELENMAX];
	short wChatLen;
	WCHAR wszChatMsg[CHATLENMAX];
};

struct MAWorldSystemMsg			// MAVI_WORLDSYSTEMMSG,	MAGA_WORLDSYSTEMMSG
{
	WCHAR wszFromCharacterName[NAMELENMAX];
	char cType;
	int nID;
	int nValue;
	WCHAR wszToCharacterName[NAMELENMAX];
};

#if defined( PRE_PRIVATECHAT_CHANNEL )
struct MAPrivateChannelChat					// MAVI_PRIVATECHNNELCHAT, MAGA_PRIVATECHNNELCHAT
{	
	char cWorldSetID;
	WCHAR wszFromCharacterName[NAMELENMAX];
	short wChatLen;
	INT64 nChannelID;
	WCHAR wszChatMsg[CHATLENMAX];	
};
#endif

struct TNoticeTypeInfo
{
	int nNoticeType;
	int nSlideShowSec;					//0면 일반 공지 아니면 전광판..--;
	int nManagedID;
	int nChannelID;
	int nMapIdx;
};

struct MANotice				//MAGA_NOTICE
{
	TNoticeTypeInfo Info;
	USHORT nLen;
	WCHAR wszNoticeMsg[CHATLENMAX];
};

struct MAUserState			//MAVI_USERSTATE / MAGA_USERSTATE
{
	WorldUserState::eCode Type;
	INT64	biCharacterDBID;
	BYTE	cLocationState;
	BYTE	cCommunityState;
	int		nChannelID;
	int		nMapIdx;
	BYTE	cNameLen;
	WCHAR	wszBuf[NAMELENMAX];
};

struct MADetachUser			// MAVI_DETACHUSER / MAGA_DETACHUSER
{
	UINT nAccountDBID;
	UINT nSessionID;
	bool bIsDuplicate;	
};

struct MAResetAuthNode
{
public:
	MAResetAuthNode()
	{
		uiAccountDBID = 0;
		nSessionID = 0;
	}
	void Set(UINT pAccountDBID, UINT pSessionID)
	{
		uiAccountDBID = pAccountDBID;
		nSessionID = pSessionID;
	}
public:
	UINT uiAccountDBID;		// 계정 DBID
	UINT nSessionID;		// 세션 ID
};

struct MAResetAuthList		// MAVI_RESETAUTHLIST / MAGA_RESETAUTHLIST / VIMA_RESETAUTHLIST / GAMA_RESETAUTHLIST / LOMA_RESETAUTHLIST / MALO_RESETAUTHLIST
{
public:
	MAResetAuthList()
	{
		cWorldSetID = 0;
		nCount = 0;
	}
	int GetSize() const
	{ 
		return(static_cast<int>((sizeof(*this) - (sizeof(this->List[0]) * (_countof(List) - nCount)))&ULONG_MAX));
	}
	bool IsFull() const
	{
		return(RESETAUTHLISTMAX <= nCount);
	}
	bool IsEmpty() const
	{
		return(0 >= nCount);
	}
	bool Add(UINT pAccountDBID, UINT pSessionID)
	{
		if (RESETAUTHLISTMAX <= nCount) {
			return false;
		}

		List[nCount].Set(pAccountDBID, pSessionID);
		++nCount;

		return true;
	}
	const MAResetAuthNode* At(int iIndex) const
	{
		if (!CHECK_LIMIT(iIndex, _countof(List))) {
			return NULL;
		}

		return(&List[iIndex]);
	}
public:
	char cWorldSetID;			// 월드 ID
	int nCount;				// 목록 개수
	MAResetAuthNode List[RESETAUTHLISTMAX];		// 계정 DBID 리셋 요청 목록
};

struct MAVIVoiceChannelID		//MAVI_VOICECHANNELID
{
	TINSTANCEID InstanceID;
	BYTE cType;
	int nPvPLobbyChannelID;
	UINT nVoiceChannelD[PvPCommon::TeamIndex::Max];
};

typedef MAGAFarmSync MAVIFarmSync;

struct MAResetAuthAccWld	// MALO_RESETAUTHLIST
{
public:
	MAResetAuthAccWld()
	{
		cWorldSetID = 0;
		nCount = 0;
	}
	int GetSize() const
	{ 
		return(static_cast<int>((sizeof(*this) - (sizeof(this->List[0]) * (_countof(List) - nCount)))&ULONG_MAX));
	}
	bool IsFull() const
	{
		return(RESETAUTHLISTMAX <= nCount);
	}
	bool IsEmpty() const
	{
		return(0 >= nCount);
	}
	bool Add(UINT pAccountDBID, UINT pSessionID)
	{
		if (RESETAUTHLISTMAX <= nCount) {
			return false;
		}

		List[nCount].Set(pAccountDBID, pSessionID);
		++nCount;

		return true;
	}
	const MAResetAuthNode* At(int iIndex) const
	{
		if (!CHECK_LIMIT(iIndex, _countof(List))) {
			return NULL;
		}

		return(&List[iIndex]);
	}
public:
	char cWorldSetID;			// 월드 ID (LO 일 경우 0 임)
	int nCount;				// 목록 개수
	MAResetAuthNode List[RESETAUTHLISTMAX];		// 계정 DBID 리셋 요청 목록
};

struct MAResetAuthOnly		// MAGA_RESETAUTHONLY / GAMA_RESETAUTHONLY
{
public:
	MAResetAuthOnly()
	{
		cWorldSetID = 0;
		nAccountDBID = 0;
		nSessionID = 0;
	}
public:
	char cWorldSetID;						// 월드 ID
	UINT nAccountDBID;					// 계정 DBID
	int nSessionID;
};

struct MAFCMState			// MAVI_FCMSTATE / MAGA_FCMSTATE
{
	UINT nAccountDBID;
	int nOnlineMin;	// 온라인 시간(min)
	bool bSend;
};

struct MANotifyMail			// MAVI_NOTIFYMAIL / MAGA_NOTIFYMAIL
{
	UINT nToAccountDBID;	// 받는이 AccountDBID
	INT64 biToCharacterDBID;
	short wTotalMailCount;
	short wNotReadMailCount;
	short w7DaysLeftCount;
	bool bNewMail;
};

struct MANotifyMarket		// MAVI_NOTIFYMARKET / MAGA_NOTIFYMARKET
{
	UINT nSellerAccountDBID;			// AccountDBID
	INT64 biSellerCharacterDBID;
	int nItemID;				// 팔린아이템
	short wCalculationCount;	// 정산카운트
};

struct MANotifyGift			// MAVI_NOTIFYGIFT / MAGA_NOTIFYGIFT
{
	UINT nToAccountDBID;	// 받는이 AccountDBID
	INT64 biToCharacterDBID;
	bool bNew;
	int nGiftCount;
};

struct MARestraint		//MAGA_RESTRAINT / MAVI_RESTRAINT
{
	UINT nAccountDBID;
};

// Guild
// VIMA_DISMISSGUILD / MAVI_DISMISSGUILD / MAGA_DISMISSGUILD
struct MADismissGuild
{
	int nManagedID;		// 서버 관리 ID
	TGuildUID GuildUID;	// 길드 UID
};

struct MAInviteGuildMember
{	
	// 초대자
	TGuildUID	GuildUID;							// 길드아이디
	UINT		nAccountDBID;						// 길드초대자 DBID
	UINT		nSessionID;							// 세션아이디
	INT64		nCharacterDBID;						// 캐릭터 아이디
	WCHAR		wszFromCharacterName[NAMELENMAX];	// 캐릭터 이름
	WCHAR		wszGuildName[GUILDNAME_MAX];		// 길드명
	// 초대 받은 자
	WCHAR		wszToCharacterName[NAMELENMAX];	
};

struct VIMAInviteGuildMemberResult		//VIMA_RETINVITEGUILDMEMBER
{
	int		nRetCode;					// 결과코드
	UINT	nInviterDBID;				// 결과를 받아야할 유저
	bool	bAck;						// 요청/결과
	WCHAR	wszInvitedName[NAMELENMAX];	// 초대받은 사람 이름
};

struct MAVIGuildMemberInvitedResult
{
	UINT		nAccountDBID;
	int			nRet;
	bool		bAck;						// 요청/결과
	WCHAR		wszInvitedName[NAMELENMAX];	// 초대받은 사람 이름
};

struct MAGuildWareInfo
{
	int				nManagedID;		// 서버 관리 ID
	TGuildUID		GuildUID;		// 길드아이디
};

struct MAGuildWareInfoResult
{
	int				nFromManagedID;				// 서버 관리 ID

	TGuildUID		GuildUID;					// 길드아이디
	__time64_t		VersionDate;				// 버젼

	BYTE			cSlotListCount;				// 슬롯카운트
	TGuildWareSlot	WareSlotList[GUILD_WAREHOUSE_MAX];	// 슬롯리스트
	
};

struct MAGuildMemberLevelUp
{
	int				nManagedID;		// 서버 관리 ID
	TGuildUID		GuildUID;		// 길드 DBID
	INT64			nCharacterDBID;	// 캐릭터 DBID
	char			cLevel;			// 캐릭터 최종레벨
};

// VIMA_ADDGUILDMEMB / MAVI_ADDGUILDMEMB / MAGA_ADDGUILDMEMBER
struct MAAddGuildMember
{
	int nManagedID;						// 서버 관리 ID
	TGuildUID GuildUID;					// 길드 UID
	UINT nAccountDBID;					// 가입한 사용자 계정 DBID
	INT64 nCharacterDBID;					// 가입한 캐릭터 DBID
	TP_JOB nJob;							// 캐릭터 전직직업
	char cLevel;							// 캐릭터 레벨
	__time64_t JoinDate;				// 길드 가입 날짜
	TCommunityLocation Location;			// 현재 위치 (로그인 여부도 본 멤버의 상태로 체크)
	WCHAR wszCharacterName[NAMELENMAX];	// 가입한 캐릭터 이름
};

// VIMA_DELGUILDMEMB / MAVI_DELGUILDMEMB / MAGA_DELGUILDMEMBER
struct MADelGuildMember
{
	int nManagedID;			// 서버 관리 ID
	TGuildUID GuildUID;		// 길드 UID
	UINT nAccountDBID;		// 제거된 사용자 계정 DBID
	INT64 nCharacterDBID;		// 제거된 캐릭터 DBID
	bool bIsExiled;			// 추방되었는지 여부
};

// VIMA_CHANGEGUILDINFO / MAVI_CHANGEGUILDINFO / MAGA_CHANGEGUILDINFO
struct MAChangeGuildInfo
{
	int nManagedID;				// 서버 관리 ID
	TGuildUID GuildUID;			// 길드 UID
	UINT nAccountDBID;			// 요청한 계정 DBID (일반적인 경우 중요하지 않고 참조 용도로만 사용)
	INT64 nCharacterDBID;			// 요청한 캐릭터 DBID (일반적인 경우 중요하지 않고 참조 용도로만 사용)
	BYTE btGuildUpdate;	// 길드 정보 변경 타입 (eGuildUpdateType)
	int Int1;
	int Int2;
	INT64 Int64;
	WCHAR Text[GUILDHISTORYTEXT_MAX+1];
	int Int3;
	int Int4;
};

// VIMA_CHANGEGUILDMEMBINFO / MAVI_CHANGEGUILDMEMBINFO / MAGA_CHANGEGUILDMEMBERINFO
struct MAChangeGuildMemberInfo	
{
	int nManagedID;			// 서버 관리 ID
	TGuildUID GuildUID;		// 길드 UID
	UINT nReqAccountDBID;	// 요청한 계정 DBID (길드장이 길드원 정보를 변경하는 경우에는 변경대상과 달라짐)
	INT64 nReqCharacterDBID;	// 요청한 캐릭터 DBID (길드장이 길드원 정보를 변경하는 경우에는 변경대상과 달라짐)
	UINT nChgAccountDBID;	// 변경될 계정 DBID 
	INT64 nChgCharacterDBID;	// 변경될 캐릭터 DBID
	BYTE btGuildMemberUpdate;		// 길드원 정보 변경 타입 (eGuildMemberUpdateType)
	int Int1;
	int Int2;
	INT64 Int64;
	WCHAR Text[GUILDPARAMTEXT_MAX+1];
};

// MAGA_REFRESHGUILDSELFVIEW
struct MARefreshGuildSelfView
{
	TGuildUID GuildUID;		// 길드 UID
	UINT nAccountDBID;		// 계정 DBID
	INT64 nCharacterDBID;		// 캐릭터 DBID
};

// VIMA_GUILDCHAT / GAMA_GUILDCHAT / MAVI_GUILDCHAT / MAGA_GUILDCHAT
struct MAGuildChat
{
	int nManagedID;				// 서버 관리 ID
	TGuildUID GuildUID;			// 길드 UID
	UINT nAccountDBID;			// 계정 DBID
	INT64 nCharacterDBID;			// 캐릭터 DBID
	short nLen;					// 채팅 길이
	WCHAR wszChatMsg[CHATLENMAX];	// 채팅 메시지
};

#ifdef PRE_ADD_DOORS_GUILDCHAT_DISCONNECT
struct MADoorsGuildChat
{
	int nManagedID;				// 서버 관리 ID
	TGuildUID GuildUID;			// 길드 UID
	INT64 nCharacterDBID;			// 캐릭터 DBID
	short nLen;					// 채팅 길이
	WCHAR wszChatMsg[CHATLENMAX];	// 채팅 메시지
};
#endif		//#ifdef PRE_ADD_DOORS_GUILDCHAT_DISCONNECT

// MAGA_GUILDMEMBLOGINLIST
struct MAGuildMemberLoginList
{
	TGuildUID GuildUID;
	int nCount;
	INT64 List[GUILDSIZE_MAX];
};

//VIMA_GUILDCHANGENAME , MAVI_GUILDCHANGE_NAME, MAGA_CHANGEGUILDNAME
struct MAGuildChangeName
{
	TGuildUID	GuildUID;
	WCHAR		wszGuildName[GUILDNAME_MAX];  // 변경한 길드명
};

struct MAGuildChangeMark
{
	TGuildUID	GuildUID;
	short		wMark;
	short		wMarkBG;
	short		wMarkBorder;
};

struct MAGuildLevelUp
{
	TGuildUID	GuildUID;
	int nLevel;
};

struct MAGuildExp
{
	TGuildUID	GuildUID;
	BYTE cPointType;
	int nPointValue;
	int nGuildExp;
	INT64 biCharacterDBID;
	int nMissionID;
};

struct MAEnrollGuildWar
{
	int			nManagedID;
	TGuildUID	GuildUID;
	short		wScheduleID;
	BYTE		cTeamColorCode;
};
//MAVI_GETGUILDWAR_FINALSCHEDULE
struct MAGetGuildWarFinalSchedule
{
	short		wScheduleID;
};
//MAVI_GETGUILDWAR_TOURNAMENTINFO
struct MAGetGuildWarTournamentInfo
{
	short		wScheduleID;
};
//MAVI_GETGUILDWAR_PREWIN_GUILD
struct MAGetGuildWarPreWinGuild
{
	short		wScheduleID;
};
//MAVI_SETGUILDWAR_FINAL_TEAM,
struct MAVISetGuildWarFinalTeam
{
	UINT	GuildDBID[GUILDWAR_FINALS_TEAM_MAX];	
};
//MAVI_SETGUILDWAR_FINALPROCESS, 
struct MASetGuildWarFinalProcess
{
	char cGuildFinalPart;			// 현재 진행중인 본선 차수
	__time64_t tBeginTime;			// 시작시간?(없으면 진행중인거임)
};
//MAVI_SETGUILDWAR_TOURNAMENTINFO, VIMA_SETGUILDWAR_TOURNAMENTINFO
struct MAGuildWarTournamentInfo
{
	SGuildTournamentInfo		sGuildWarFinalInfo[GUILDWAR_FINALS_TEAM_MAX]; // 대진표 순 길드 정렬
};
//MAVI_SETGUILDWAR_PREWIN_GUILD, VIMA_SETGUILDWAR_PRE_WIN_GUILD
struct MAGuildWarPreWinGuild
{
	TGuildUID GuildUID;		// 우승 길드 UID
	bool bPreWin;			
	short	wScheduleID;
};

//MAVI_SETGUILDWAR_PREWIN_SKILLCOOLTIME, VIMA_SETGUILDWAR_PREWIN_SKILLCOOLTIME
struct MAGuildWarPreWinSkillCoolTime
{
	int nManagedID;			// 서버 관리 ID
	DWORD dwSkillCoolTime;		// 쿨타임
};

//MAVI_SETGUILDWAR_FINAL_RESULT
struct MAVISetGuildWarFinalResult
{
	char	cMatchTypeCode;
	char	cWinGuildIndex;
	char	cLoseGuildIndex;
};
//MAVI_SETGUILDWAR_FINAL_REWARD, MAVI_SETGUILDWAR_FINAL_RESULT_DB
struct MAVISetGuildWarFinalReward
{
	char	cMatchTypeCode;		// 몇강?
	UINT	GuildDBID;
	bool	bWin;	
};

struct MAVISetGuildWarFinalDB
{
	short	wScheduleID;
	char	cMatchSequence;	
	char	cMatchTypeCode;		// 몇강?
	UINT	GuildDBID;
	bool	bWin;
};

// MAVI_SETGUILDWAR_EVENT_TIME, MAGA_SETGUILDWAR_EVENT_TIME
struct MASetGuildWarEventTime
{
	// 본선 진행 여부
	bool bFinalProgress;		
	// 전체 길드전 스케쥴
	TGuildWarEventInfo sGuildWarTime[GUILDWAR_STEP_END]; 
	// 본선 진행 차수
	TGuildWarEventInfo sFinalPartTime[GUILDWAR_FINALPART_MAX];
};

//MAVI_SETGUILDWAR_TOURNAMENTWIN, MAGA_SETGUILDWAR_TOURNAMENTWIN
struct MASetGuildWarTournamentWin
{	
	// 승리 차수
	char cMatchTypeCode;					// 현재 진행중인 차수 1=결승, 2=4강, 3=8강, 4=16강
	// 승리한 길드명
	WCHAR wszGuildName[GUILDNAME_MAX];	// 길드 이름
};

// VIMA_UPDATEGUILDEXP / MAVI_UPDATEGUILDEXP / MAGA_UPDATEGUILDEXP / GAMA_UPDATEGUILDEXP
struct MAUpdateGuildExp
{
	int nManagedID;				// 서버관리ID
	TGuildUID GuildUID;			// 길드UID
	char cPointType;			// 포인트타입
	int nPointValue;			// 포인트값
	int nTotalGuildExp;			// 길드경험치
	int nDailyPointValue;		// 일일 포인트값
	int nLevel;					// 레벨업
	INT64 biCharacterDBID;		// 캐릭터 ID
	int nMissionID;				// 미션 ID
};

struct MAGuildRecruitMember
{
	int			nManagedID;				// 서버 관리 ID
	BYTE		cAcceptType;
	UINT		uiAccountDBID;
	WCHAR		wszCharacterName[NAMELENMAX];
	INT64		biCharacterDBID;
	TGuildUID	GuildUID;
	TGuildView	GuildView;
};
struct MAGuildRewardItem
{
	int			nManagedID;				// 서버 관리 ID
	TGuildUID	GuildUID;
	int			nItemID;	
	BYTE		cItemBuyType;
	__time64_t	m_tExpireDate;
};
struct MAExtendGuildSize
{
	int			nManagedID;				// 서버 관리 ID
	TGuildUID	GuildUID;
	short		nGuildSize;	
};

//MAVI_ADD_DBJOBSYSTEM_RESERVE
struct MAVIAddDBJobSystemReserve
{
	char cJobType;
};
struct MAVIGetDBJobSystemReserve
{
	int nJobSeq;
};

struct MAChangeCharacterName
{
	int			nManagedID;
	TGuildUID	GuildUID;
	char		cWorldSetID;
	UINT		uiAccountDBID;
	INT64		nCharacterDBID;
	WCHAR		wszOriginName[NAMELENMAX];
	WCHAR		wszCharacterName[NAMELENMAX];
#if defined (PRE_ADD_BESTFRIEND)
	UINT		uiBFAccountDBID;
	INT64		biBFCharacterDBID;
#endif
};

struct MAUpdateWorldEventCounter
{
	char		cWorldSetID;
	int			nScheduleID;
	int			nCount;
};

struct MAPCBangResult
{
	UINT nAccountDBID;
#if defined(_KR)
	char cResult;				// eAuthResult
	char cAuthorizeType;
	char cOption;				// 사용자에게 보여주는 메시지
	int nArgument;				// 기타 더 필요한 데이터를 여기에 첨부 (ex. 시간제의 경우 남은시간, 정액제인 경우 만료날짜 등등)
	bool bShutdowned;			// 셧다운제
	char cPolicyError;
	int nShutdownTime;
#else	// #if defined(_KR)

#if defined(_ID)
	bool bBlockPcCafe;
#endif //#if defined(_ID)
	char cPCBangGrade;
#if defined(_KRAZ)
	TShutdownData ShutdownData;
#endif	// #if defined(_KRAZ)

#endif	// #if defined(_KR)
};

struct MAGAZeroPopulation		//MAGA_ZEROPOPULATION
{
	bool bZeroPopulation; //true유저 뺍니다. false유저들어와도 되요
};

// Master -> Login
struct MALORegist				//MALO_REGIST
{
	char cWorldSetID;
	UINT nWorldMaxUser;
};

struct MALOVillageInfo			//MALO_VILLAGEINFO
{
	int nVillageID;
	UINT nWorldUserCount;
	UINT nWorldMaxUserCount;
	BYTE cCount;
	sChannelInfo Info[VILLAGECHANNELMAX];
};

struct MALOVillageInfoDelete				//MALO_VILLAGEINFODEL		
{
	int nVillageID;
};

struct MALOAddUser			//MALO_ADDUSER
{
	UINT nAccountDBID;
	UINT nSessionID;
	char cWorldID;
	int nRet;
};

#if !defined(PRE_MOD_SELECT_CHAR)
struct MALOUserInfo			// MALO_USERINFO
{
	UINT nAccountDBID;
	UINT nSessionID;
	int nRet;
#ifdef PRE_MOD_RESTRICT_IDENTITY_IP
	char szIP[IPLENMAX];
#endif		//#ifdef PRE_MOD_RESTRICT_IDENTITY_IP
};
#endif	// #if !defined(PRE_MOD_SELECT_CHAR)

struct MALODetachUser		//MALO_DETACHUSER
{
	UINT nAccountDBID;
};

struct MALOSetTutorialGameID	// MALO_SETTUTORIALGAMEID
{
	UINT nSessionID;
	ULONG nIP;
	USHORT nPort;
	USHORT nTcpPort;
	BYTE cServerIdx;
	USHORT wGameID;
	int nRoomID;
	int nRet;
};

struct MALOResetAuthServer		// MALO_RESETAUTHSERVER
{
	int nServerID;
};

struct MALOWaitUserProcess		// MALO_WAITUSERPROCESS
{
	USHORT nCurCount;
	short nCount;
	UINT nAccountArr[WAITPROCESSMAX];
};

struct MALOAddWaitUser			// MALO_ADDWAITUSER
{
	UINT nAccountDBID;
	USHORT nTicketNum;
	short nRetCode;
};

struct MALOUpdateChannelShowInfo
{
	USHORT	unChannelID;
	bool	bShow;
};

struct TUpdateTicket
{
	short nIndex;
	short nCount;
};

struct MALODuplicateLogin
{
	UINT nAccountDBID;
	UINT nSessionID;
};

// Master -> Village
//struct MAVIVillageServerInfo		//MAVI_VILLAGESERVERINFO
//{
//	USHORT nInfoCount;
//	TChannelInfo ChannelList[VILLAGECHANNELMAX];
//};

#define MAVIVillageInfo MALOVillageInfo		//MAVI_VILLAGEINFO

#define MAVIVillageInfoDelete MALOVillageInfoDelete //MAVI_VILLAGEINFODEL

struct MAVILoginState		// MAVI_LOGINSTATE
{
	UINT nSessionID;
	int nRet;
};

struct MAVICheckUser			// MAVI_CHECKUSER
{
	UINT	nSessionID;
	UINT	nAccountDBID;
	WCHAR	wszAccountName[IDLENMAX];
	INT64	biCharacterDBID;
#if !defined( PRE_PARTY_DB )
	TPARTYID	PartyID;
#endif
	USHORT	nChannelID;
	BYTE	cPvPVillageID;
	USHORT	unPvPVillageChannelID;
	bool	bAdult;			// 어른인지 아닌지 (피로도)
	char	cPCBangGrade;	// pcbang
	short	nRet;
#if defined(PRE_ADD_MULTILANGUAGE)
	char cSelectedLanguage;
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)
#ifdef PRE_ADD_COMEBACK
	bool bComebackUser;
#endif		//#ifdef PRE_ADD_COMEBACK
#if defined(PRE_ADD_GAMEQUIT_REWARD)
	bool bReConnectNewbieReward;
#endif	// #if defined(PRE_ADD_GAMEQUIT_REWARD)
#if defined(_ID)
	char szMacAddress[MACADDRLENMAX];
	char szKey[KREONKEYMAX];	
#endif
};

struct MAVITargetVillageInfo		//MAVI_TARGETVILLAGEINFO
{
	UINT nAccountDBID;
	BYTE cVillageID;
	int nTargetChannelID;				//maybe
	int nTargetMapIdx;
	char cTargetGateNo;
	char szIP[IPLENMAX];
	USHORT nPort;
	int nRet;
	BYTE cPartyInto;
	INT64 nItemSerial;
};

struct MAVISetGameID			// MAVI_SETGAMEID
{
	GameTaskType::eType GameTaskType;
	BYTE	cReqGameIDType;
	TINSTANCEID InstanceID;
	ULONG	nIP;
	USHORT	nPort;
	USHORT	nTcpPort;
	BYTE	cServerIdx;
	USHORT	wGameID;
	int		iRoomID;
	USHORT	unVillageChannelID;
	BYTE cVillageID;
	short	nRet;
	BYTE cUseVoice;
#if defined( PRE_WORLDCOMBINE_PARTY )
	ePartyType Type;
#endif
#if defined( PRE_WORLDCOMBINE_PVP )
	WorldPvPMissionRoom::Common::eReqType eWorldReqType;
#endif
};

struct MAVILadderSetGameID
{
	ULONG	nIP;
	USHORT	nPort;
	USHORT	nTcpPort;
	BYTE	cServerIdx;
	USHORT	wGameID;
	int		iRoomID;
	USHORT	unVillageChannelID;
	BYTE	cVillageID;
	short	nRet;
	int		nGameModeTableID;
	BYTE	cUserCount;
	UINT	uiAccountDBIDArr[PARTYMAX];
};

struct MAVIPushParty				// MAVI_PUSHPARTY
{
	UINT nLeaderAccountDBID;	
#if defined( PRE_PARTY_DB )
	Party::Data PartyData;	
#else
	TPARTYID PartyID;
	int nChannelID;
	BYTE cMemberMax;
	TPARTYITEMLOOTRULE ItemLootRule;
	TITEMRANK ItemLootRank;	

	int nTargetMapIdx;
	TDUNGEONDIFFICULTY Difficulty;
#endif
	int nRandomSeed;
	BYTE cPushType;
	UINT nKickedMemberList[PARTYKICKMAX];
	WCHAR wszPartyName[PARTYNAMELENMAX];
#if defined( PRE_PARTY_DB )
#else
	USHORT nUserLvLimitMin;
	USHORT nUserLvLimitMax;
	BYTE cIsPartyJobDice;
	WCHAR wszPartyPass[PARTYPASSWORDMAX];
	BYTE cUpkeepCount;
	BYTE cMemberCount;
#endif // #if defined( PRE_PARTY_DB )
	UINT nVoiceChannelID;	
	TMemberInfo MemberInfo[PARTYMAX];
};

struct MAVIInvitePartyMember : public TInvitePartyInfo		//MAVI_INVITEPARTYMEMBER
{
	int nGameServerID;				//혹시나 해서 어느 게임서버에서 초대한건지
	int nVillageMapIdx;				//마스터에서 한번 검증은 하지만 다시!하기 위해서 파티가 어디로 간녀석인지

	UINT nInviterAccountDBID;
	INT64 biInviterCharacterDBID;
	WCHAR wszInviterName[NAMELENMAX];	//초대한 놈의 이름
	UINT nInvitedAccountDBID;			//초대받을 놈의 디비아이디	
	WCHAR wszInvitedName[NAMELENMAX];	//초대받을 놈의 이름
	BYTE cPermitLevel;
	char cPassClassIds[PERMITMAPPASSCLASSMAX];
};

#if defined( PRE_PARTY_DB )

struct MAVIResPartyInvite
{
	int iRet;
	UINT uiReqAccountDBID;
};

#else
struct MAVIReqPartyID			// MAVI_REQPARTYID
{
	UINT nAccountDBID;
	UINT nSessionID;
	TPARTYID PartyID;
	BYTE cMemberMax;
	int nChannelID;	
	TPARTYITEMLOOTRULE ItemLootRule;
	TITEMRANK ItemRank;
	BYTE cUserLvLimitMin;
	BYTE cUserLvLimitMax;

	int nTargetMapIdx;
	TDUNGEONDIFFICULTY Difficulty;

	BYTE cUseVoice;
	UINT nAfterInviteAccountDBID;

	WCHAR wszPartyName[PARTYNAMELENMAX];
	BYTE cPartyJobDice;
	WCHAR wszPartyPass[PARTYPASSWORDMAX];
};
#endif // #if defined( PRE_PARTY_DB )

struct MAVIDelParty			// MAVI_DELPARTY
{
	UINT nAccountDBID;
	int nPartyIndex;
	bool bSend;
};

struct MAVIAddPartyMember		// MAVI_ADDPARTYMEMBER
{
	UINT nAccountDBID;
	UINT nSessionID;
	int nPartyIndex;
	int nMemberIndex;
};

struct MAVIDelPartyMember		// MAVI_DELPARTYMEMBER
{
	UINT nAccountDBID;
	int nPartyIndex;
	int nMemberIndex;
	UINT nNewLeaderAccountDBID;
	UINT nNewLeaderSeessionID;
	bool bSend;
	char cKickKind;
};

struct MAVISwapPartyLeader		//MAVI_SWAPPARTYLEADER
{
	int nPartyIdx;
	int nNewLeaderMemberIdx;
	UINT nAccountDBID;
	UINT nSessionID;
};

struct MAVIReservePartyMember		//MAVI_RESERVEPARTYMEMBER
{
	UINT nAccountDBID;
	int nPartyIdx;
	int nChannelID;
	int nRet;
};

struct MAVIDelVoiceChannel	 //MAVI_DELVOICECHANNEL
{
	int nPartyIdx;
};

struct MAVIResult				// MAVI_ADDPARTYFAIL, MAVI_DELPARTYFAIL, MAVI_ADDPARTYMEMBERFAIL, MAVI_DELPARTYMEMBERFAIL
{
	UINT nAccountDBID;
	int nMainCmd;
	int nRet;
};

struct MAVIFriendAddNotice			//MAVI_FRIENDADDNOTICE
{
	UINT nAddedAccountDBID;
	WCHAR wszAddName[NAMELENMAX];
};

// PvP
struct MAVIPVP_CREATEROOM
{
	int					nRetCode;
	UINT				uiCreateAccountDBID;
	UINT				uiPvPIndex;
	USHORT				unVillageChannelID;
	int					nEventID;
	TEvent				EventData;
	BYTE				cGameMode;
	UINT nGuildWarDBID[PvPCommon::TeamIndex::Max];		//cGameMode가 GuildWar일경우에만 참조
#if defined( PRE_PVP_GAMBLEROOM )
	BYTE				cGambleType;
	int					nPrice;
#endif
	CSPVP_CREATEROOM	sCSPVP_CREATEROOM;
#if defined( PRE_WORLDCOMBINE_PVP )
	BYTE cWorldPvPRoomCurMemberCount;
#endif
};

struct MAVIPVP_MODIFYROOM
{
	int					nRetCode;
	UINT				uiAccountDBID;
	UINT				uiPvPIndex;
	USHORT				unVillageChannelID;
	BYTE				cGameMode;
	CSPVP_MODIFYROOM	sCSPVP_MODIFYROOM;
};

struct MAVIPVP_DESTROYROOM
{
	USHORT				unVillageChannelID;
	UINT				uiPvPIndex;
};

struct MAVIPVP_LEAVEROOM
{
	short								nRetCode;
	USHORT								unVillageChannelID;
	UINT								uiPvPIndex;
	UINT								uiLeaveAccountDBID;
	PvPCommon::LeaveType::eLeaveType	eType;
};

struct MAVIPVP_ROOMLIST
{
	UINT			uiAccountDBID;
	USHORT			unVillageChannelID;
	CSPVP_ROOMLIST	sCSPVP_ROOMLIST;
};

#if defined(PRE_ADD_PVP_VILLAGE_ACCESS)
struct MAVIPVP_ROOMLIST_RELAY
{
	BYTE			 cVillageID;
	UINT			 uiAccountDBID;
	MAVIPVP_ROOMLIST sRoomList;	
};
#endif //#if defined(PRE_ADD_PVP_VILLAGE_ACCESS)

#ifdef PRE_ADD_COLOSSEUM_BEGINNER
struct MAVIPVP_CHANGECHANNEL
{
	UINT nAccountDBID;
	BYTE cType;
	int nRetCode;
};
#endif		//#ifdef PRE_ADD_COLOSSEUM_BEGINNER

struct MAVIPVP_WAITUSERLIST
{
	UINT				uiAccountDBID;
	USHORT				unVillageChannelID;
	CSPVP_WAITUSERLIST	sCSPVP_WAITUSERLIST;
};

struct MAVIPVP_JOINROOM
{
	short	nRetCode;
	UINT	uiAccountDBID;
	USHORT	unVillageChannelID;
	UINT32	uiPvPIndex;
	UINT16	unPvPTeam;
	UINT	uiUserState;
	BYTE cIndex;
};


struct MAVIPVP_ROOMSTATE
{
	USHORT	unVillageChannelID;
	UINT	uiPvPIndex;
	UINT	uiRoomState;
};

struct MAVIPVP_CHANGETEAM
{
	int					nRetCode;
	USHORT				unVillageChannelID;
	UINT32				uiPvPIndex;
	VIMAPVP_CHANGETEAM	sVIMAPVP_CHANGETEAM;
	char cTeamSlotIndex;
};

struct MAVIPVP_CHANGEUSERSTATE
{
	USHORT	unVillageChannelID;
	UINT	uiPvPIndex;
	UINT	uiAccountDBID;
	UINT	uiUserState;
};

struct MAVIPVP_START
{
	short	nRetCode;
	USHORT	unVillageChannelID;
	UINT	uiPvPIndex;
	UINT	uiAccountDBID;
};

struct MAVIPVP_STARTMSG
{
	USHORT	unVillageChannelID;
	UINT	uiPvPIndex;
	BYTE	cSec;
};

struct MAVIPVP_ROOMINFO
{
	UINT	uiAccountDBID;
	UINT	uiIndex;
	UINT	uiWinCondition;
	UINT	uiGameModeTableID;
	UINT	uiPlayTimeSec;
	BYTE	cMaxUser;
	bool bIsGuildWar;		//이게 트루면 나가기만 활성화 나머지는 전부 막힘
#if defined( PRE_PVP_GAMBLEROOM )
	BYTE cGambleType;
	int nGamblePrice;
#endif
};

struct MAVIPvPMemberIndex		//MAVI_PVP_MEMBERINDEX
{
	UINT nAccountDBID;
	USHORT nTeam;
	int nRetCode;
	BYTE cCount;
	TSwapMemberIndex Index[PARTYMAX];
};

struct MAVIPvPMemberGrade		//MAVI_PVP_MEMBERGRADE
{
	UINT nAccountDBID;
	USHORT nTeam;
	UINT uiUserState;
	UINT nChangedSessionID;
	int nRetCode;
};

struct MAVIResRecall		// MAVI_RESRECALL
{
	UINT	uiAccountDBID;			// 요청한사람 AccountDBID
	UINT	uiRecallAccountDBID;	// Recall당할사람 AccountDBID
	UINT	uiRecallChannelID;		// Recall당할 ChannelID
#if defined(PRE_ADD_MULTILANGUAGE)
	char cRecallerSelectedLang;		// 요청한 사람의 설정언어
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)
};

struct MAVIVillageTrace		// MAVI_VILLAGETRACE
{
	UINT	uiAccountDBID;
	UINT	uiTargetAccountDBID;
	int		iTargetVillageID;
	int		iTargetChannelID;
	bool	bIsGMCall;
#if defined(PRE_ADD_MULTILANGUAGE)
	char cSelectedLang;		//uiAccountDBID해당하는 유저의 설정언어
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)
};

struct MAVIUpdateChannelShowInfo
{
	USHORT	unChannelID;
	bool	bShow;
};

struct MAVIUserTempDataResult	//MAVI_USERTEMPDATA_RESULT
{
	UINT uiAccountDBID;
	int nDungeonClearCount;
#if defined(PRE_ADD_GAMEQUIT_REWARD)
	GameQuitReward::RewardType::eType eUserGameQuitRewardType;
#endif	// #if defined(PRE_ADD_GAMEQUIT_REWARD)
#if defined(PRE_ADD_REMOTE_QUEST)
	int nAcceptWaitRemoteQuestCount;
	int AcceptWaitRemoteQuestList[MAX_PLAY_QUEST];
#endif
};

// Master -> Game
struct MAGAReqMemberInfo
{
	UINT MemberAccountDBID;
	UINT MemberSessionID;
	INT64 biMemberCharacterDBID;
	UINT MemberTeam;
	WCHAR wszAccountName[IDLENMAX];
	bool bAdult;			// 어른이냐 아니냐(피로도)	
	char cPCBangGrade;		// PCBang
	BYTE cMemberIndex;
	bool bPvPFatigue;
	UINT uiPvPUserState;
#if defined(PRE_ADD_MULTILANGUAGE)
	char cSelectedLanguage;
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)
#if defined( PRE_ADD_NEWCOMEBACK )
	bool bCheckComebackAppellation;
#endif
#if defined(_ID)
	char szMacAddress[MACADDRLENMAX];
	char szKey[KREONKEYMAX];	
#endif
	TMemberVoiceInfo VoiceInfo;
};

// 해당 패킷을 GameTaskType,ReqGameIDType 별로 공용으로 쓰고 있어서
// 효율성이 좋지않다.. 언젠가 정리를 해야 할 듯....
struct MAGAReqRoomID			// MAGA_REQROOMID
{
	GameTaskType::eType GameTaskType;
	BYTE cReqGameIDType;
	UINT nLeaderAccountDBID;
	union
	{
		LadderSystem::MatchType::eCode MatchType;
		TINSTANCEID InstanceID;
	};
	int nRandomSeed;
#if defined( PRE_PARTY_DB )
	Party::Data PartyData;
#else
	TPARTYITEMLOOTRULE ItemLootRule;
	TITEMRANK ItemLootRank;	
	BYTE cUserLvLimitMin;
	TDUNGEONDIFFICULTY PartyDifficulty;
	BYTE cUpkeepCount;
	BYTE cMemberMax;	
#endif	
	BYTE cMemberCount;
	int	nMapIndex;
	BYTE cGateNo;
	BYTE cGateSelect;
	TDUNGEONDIFFICULTY StageDifficulty;
	char cWorldSetID;			//같은월드에서 넘어오시겠지!
	BYTE cVillageID;		//요것도 같은 빌리지에서 넘어오시겠지! (아닐 수가 있게되면 늘려주삼~)
	USHORT nMeritBonusID;			//특정채널에서 게임으로 넘어갈 경우 특정의 보너스를 받을 수 있음 	
#if defined( PRE_PARTY_DB )
#else
	BYTE cUserLvLimitMax;
#endif // #if defined( PRE_PARTY_DB )
#if defined( PRE_ADD_VIP_FARM ) || defined( PRE_ADD_FARM_DOWNSCALE )
	union
	{
#if defined( PRE_ADD_FARM_DOWNSCALE )
		int iFarmAttr;
#else
		Farm::Attr::eType FarmAttr;
#endif // #if defined( PRE_ADD_FARM_DOWNSCALE )
		int nTargetMapIdx;
	};
#else
	int nTargetMapIdx;
#endif // #if defined( PRE_ADD_VIP_FARM ) || defined( PRE_ADD_FARM_DOWNSCALE )
	
	int nChannelID;
	UINT nKickedMemberList[PARTYKICKMAX];

	UINT nGuildDBID[PvPCommon::Common::DefaultGuildCount];
	int nGuildQualifyingScore[PvPCommon::Common::DefaultGuildCount];

	bool bStart;

	int nEventRoomIndex;

	bool bDirectConnect;

	INT64 biLadderRoomIndexArr[2];
	WCHAR wszPartyName[PARTYNAMELENMAX];

#if defined( PRE_PARTY_DB )	
#else
	WCHAR wszPartyPass[PARTYPASSWORDMAX];
	BYTE cIsJobDice;
#endif // #if defined( PRE_PARTY_DB )
#if defined( PRE_WORLDCOMBINE_PVP )
	WorldPvPMissionRoom::Common::eReqType eWorldReqType;
	TWorldPvPMissionRoom MissonRoomData;
#endif
#if defined( PRE_PVP_GAMBLEROOM )
	BYTE cGambleType;
	int nGamblePrice;
#endif
	INT64 biCreateRoomCharacterDBID;
	UINT nVoiceChannelID[PvPCommon::TeamIndex::Max];					//게임서버에서 생성할 보이스채널아이디 (현재 PvP팀이 가장많아서 최대값을 팀맥스로)	
	MAGAReqMemberInfo arrMemberInfo[PvPCommon::Common::MaxPlayer];	// 한방에 들어가는 유저수는 현재 PvP 방이 제일 많으므로 PvP방 최대수로 잡는다.
};

struct MAGAInvitePartyMemberResult		//MAGA_INVITEPARTYMEMBERRESULT
{
	UINT nAccountDBID;
	WCHAR wszInvitedName[NAMELENMAX];
	int nRetCode;
};

struct MAGAForceStopPvP		//MAGA_PVP_FORCESTOP
{
	UINT nGameRoomID;
	UINT nForceWinGuildDBID;
};

struct MAGAReqTutorialRoomID	// MAGA_REQTUTORIALROOMID
{
	UINT	nAccountDBID;
	UINT	nSessionID;
	INT64	biCharacterDBID;
	int		nRandomSeed;
	int		nTutorialMapIndex;
	BYTE	cTutorialGateNo;
	char	cWorldSetID;
	bool	bAdult;			// 어른이냐 아니냐(피로도)
	WCHAR	wszAccountName[IDLENMAX];
	int		nLoginServerID;
#if defined(PRE_ADD_MULTILANGUAGE)
	char cSelectedLanguage;
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)
};

struct MAGALoginState // MAGA_LOGINSTATE
{
	UINT nAccountDBID;
	int nRet;
};

struct MAGAVillageState	// MAGA_VILLAGESTATE
{
	UINT nAccountDBID;
	int nRoomID;
	int nMapIndex;
	int nRet;
};

struct MAGAFriendAddNotice		//MAGA_FRIENDADDNOTICE
{
	UINT nAddedAccountDBID;
	WCHAR wszAddName[NAMELENMAX];
};

struct MAGAPVP_GAMEMODE
{
	int		iGameRoomID;
	UINT	uiPvPIndex;
	UINT	uiPvPGameModeTableID;
	UINT	uiWinCondition;
	UINT	uiPlayTimeSec;
	USHORT	unGameModeCheck;
	INT64	biSNMain;
	int		iSNSub;
	BYTE	cIsPWRoom;
	USHORT	unRoomOptionBit;
	BYTE	cMaxUser;
	int		nEventID;
	bool	bIsLadderRoom;
	bool bIsGuildWar;		//시스템에 의한 생성인지
#ifdef PRE_ADD_COLOSSEUM_BEGINNER
	char cPvPChannelType;
#endif		//#ifdef PRE_ADD_COLOSSEUM_BEGINNER
#if defined( PRE_WORLDCOMBINE_PVP )
	int nStartPlayer;
#endif
};
#if defined(PRE_ADD_PVP_TOURNAMENT)
struct STournamentUserInfo
{
	UINT	uiAccountDBID;
	INT64	biCharacterDBID;
	BYTE	cJob;
	UINT	uiSessionID;
	WCHAR	wszCharName[NAMELENMAX];
};
struct STournamentMatchInfo
{
	bool bStartMatch;			// 게임 진행여부.
	char cTournamentStep;		// 토너먼트 스텝
	bool bWin;					// 이겼는지 졌는지.		
	bool bAbuse;				// 어뷰징 여부
	bool bLeave;				// 나갔는지 여부
	STournamentUserInfo sTournamentUserInfo;	
};
struct MAGAPVP_TOURNAMENT_INFO
{
	int nGameRoomID;
	int nCount;
	STournamentUserInfo sTournamentUserInfo[PvPCommon::Common::PvPTournamentUserMax];
};
#endif

struct MAGABreakIntoRoom
{
	int		iGameRoomID;
	//
	char	cWorldSetID;
	BYTE	cVillageID;
	UINT	uiAccountDBID;
	UINT	uiSessionID;
	INT64	biCharacterDBID;
	UINT	uiTeam;
	bool	bAdult;			// 어른이냐 아니냐(피로도)
	char	cPCBangGrade;		// PCBang
	BreakInto::Type::eCode		BreakIntoType;
	bool	bPvPFatigue;	// PvP시 피로도 소모를 통한 경험치 받기
	char cPvPTeamIndex;
	UINT uiPvPUserState;
#if defined(PRE_ADD_MULTILANGUAGE)
	char cSelectedLanguage;
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)
#if defined(_ID)
	char szMacAddress[MACADDRLENMAX];
	char szKey[KREONKEYMAX];	
#endif
	WCHAR	wszAccountName[IDLENMAX];
	WCHAR wszCharacterName[NAMELENMAX];
};

#define MAGAPvPMemberIndex MAVIPvPMemberIndex		//GAVI_PVP_MEMBERINDEX
#define MAGAPvPMemberGrade MAVIPvPMemberGrade		//MAGA_PVP_MEMBERGRADE

struct MAGAPVPChangeUserState		//MAGA_PVP_MEMBERUSERSTATE
{
	UINT nPvPRoomID;
	UINT nAccountDBID;
	UINT uiUserState;
};

struct MAGADelPartyMember		// MAGA_DELPARTYMEMBER
{
	UINT nNewLeaderAccountDBID;	// 새 리더 아이디
	int nRoomID;				// 방번호
	UINT nMemberSessionID;		// 지워질 멤버 아이디
	UINT nNewLeaderSessionID;
	//bool bIsUnintendMember;
	char cKickKind;
};

#if defined(PRE_MOD_REQ_JOIN_PARTY_ANSWER_MSG_APP)
struct MAGAGetPartyID
{	
	UINT nAccountDBID;
	UINT nSenderAccountDBID;	//정보를 요청한 녀석
};

struct GAMAGetPartyIDResult
{
	UINT nSenderAccountDBID;	//정보를 요청한 녀석
	TPARTYID PartyID;
};

struct MAGAReqPartyAskJoin
{
	UINT nAccountDBID;
	UINT nReqAccountDBID;
	INT64 biReqCharacterDBID;
	char cReqUserJob;
	BYTE cReqUserLevel;
	WCHAR wszReqChracterName[NAMELENMAX];	
};

struct GAMAResPartyAskJoin
{
	int iRet;
	UINT uiReqAccountDBID;
	WCHAR wszReqChracterName[NAMELENMAX];
};

struct MAGAResPartyAskJoin
{
	int iRet;
	UINT uiReqAccountDBID;	
};

struct GAMAAskJoinAgreeInfo
{
	TPARTYID PartyID;
	int iPassword;
	WCHAR wszAskerCharName[NAMELENMAX];	
};
#endif

struct MAGANextVillageInfo		// MAGA_NEXTVILLAGEINFO
{
	UINT nAccountDBID;
	int nChannelID;				//maybe
	int nMapIdx;
	int nNextMapIdx;
	int nNextGateIdx;
	char szIP[IPLENMAX];
	USHORT nPort;
	int nRet;
	INT64 nItemSerial;
};

struct MAGARebirthVillageInfo	// MAGA_REBIRTHVILLAGEINFO
{
	UINT nAccountDBID;
	int nVillageID;
	int nChannelID;				//maybe
	char szIP[IPLENMAX];
	USHORT nPort;
	int nRet;
};

struct MAGAMovePvPGameToPvPLobby	// MAGA_MOVEPVPGAMETOPVPLOBBY
{
	UINT	uiAccountDBID;
	char	szIP[IPLENMAX];
	USHORT	unPort;
	short	nRet;
};

struct MAGAResRecall		// MAGA_RESRECALL
{
	UINT	uiAccountDBID;			// 요청한사람 AccountDBID
	UINT	uiRecallAccountDBID;	// Recall당할사람 AccountDBID
	UINT	uiRecallChannelID;		// Recall당할 ChannelID
	int		iRecallMapIndex;
#if defined(PRE_ADD_MULTILANGUAGE)
	char cRecallerSelectedLang;		// 요청한 사람의 설정언어
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)
};

namespace LadderSystem
{
	struct MAVIDelUser
	{
		WCHAR	wszCharName[NAMELENMAX];
		INT64	biCharDBID;
	};

	struct MAVIRoomSync
	{
		INT64	biRoomIndexArr[2];
		LadderSystem::RoomState::eCode State;
	};
};

namespace MasterSystem
{
	struct MAVICheckMasterApplication
	{
		short	nRet;
		UINT	uiPupilAccountDBID;
		INT64	biMasterCharacterDBID;
	};

	struct MAVISyncSimpleInfo
	{
		UINT							uiAccountDBID;
		INT64							biCharacterDBID;
		MasterSystem::EventType::eCode	EventCode;
	};

	typedef MAVISyncSimpleInfo MAGASyncSimpleInfo;

	struct MAVISyncJoin
	{
		UINT	uiAccountDBID;
		INT64	biCharacterDBID;
		bool	bIsAddPupil;
	};

	typedef MAVISyncJoin MAGASyncJoin;

	struct MAVISyncLeave
	{
		UINT	uiAccountDBID;
		INT64	biCharacterDBID;
		bool	bIsDelPupil;
	};

	typedef MAVISyncLeave MAGASyncLeave;

	struct MAVISyncGraduate
	{
		UINT	uiAccountDBID;
		WCHAR	wszCharName[NAMELENMAX];
	};

	typedef MAVISyncGraduate MAGASyncGraduate;

	struct MAVISyncConnect
	{
		UINT	uiAccountDBID;
		bool	bIsConnect;
		WCHAR	wszCharName[NAMELENMAX];
	};

	typedef MAVISyncConnect	 MAGASyncConnect;

	struct MAVICheckLeave
	{
		int		iRet;
		UINT	uiAccountDBID;
		INT64	biDestCharacterDBID;
		bool	bIsMaster;
	};

	struct MAVIRecallMaster
	{
		int		iRet;
		UINT	uiAccountDBID;
		WCHAR	wszCharName[NAMELENMAX];
		bool	bIsConfirm;
	};
	struct MAVIJoinConfirm
	{		
		UINT uiMasterAccountDBID;
		BYTE cLevel;
		BYTE cJob;
		WCHAR wszPupilCharName[NAMELENMAX];
	};
	struct MAVIJoinResult
	{
		int iRet;
		bool bIsAccept;
		INT64 biMasterCharacterDBID;
		WCHAR wszPupilCharName[NAMELENMAX];
	};
}

// VIMA_REFRESH_GUILDITEM / MAVI_REFRESH_GUILDITEM
struct MARefreshGuildItem
{
	int			nManagedID;
	TGuildUID	GuildUID;

	char cMoveType;

	INT64	biSrcSerial;
	INT64   biDestSerial;

	TItemInfo SrcInfo;
	TItemInfo DestInfo;

	__time64_t VersionDate;
};

// VIMA_REFRESH_GUILDCOIN / MAVI_REFRESH_GUILDCOIN
struct MARefreshGuildCoin
{
	int			nManagedID;
	TGuildUID	GuildUID;

	INT64		biTotalCoin;
};

//VIMA_EXTEND_GUILDWARE,  MAVI_EXTEND_GUILDWARE
struct MAExtendGuildWare
{
	int			nManagedID;
	TGuildUID	GuildUID;

	short		wWareSize;
};

struct TFarmInfo
{
	UINT nFarmDBID;
	int nFarmCurUserCount;
	bool bActivate;
};

struct MAVIFarmInfoUpdate	
{
	BYTE cFarmCount;
	TFarmInfo FarmInfo[Farm::Max::FARMCOUNT];
};

struct MAGAAssginPeriodQuest	// MAGA_ASSIGN_PERIODQUEST
{
	UINT nQuestID;
	bool bFlag;
};

struct MAGANoticePeriodQuest	// MAGA_NOTICE_PERIODQUEST
{
	char cWorldSetID;
	int nItemID;
	int nNoticeCount;
};

struct MAGAUserTempDataResult	//MAGA_USERTEMPDATA_RESULT
{
	UINT uiAccountDBID;
	int nDungeonClearCount;
#if defined(PRE_ADD_GAMEQUIT_REWARD)
	GameQuitReward::RewardType::eType eUserGameQuitRewardType;
#endif	// #if defined(PRE_ADD_GAMEQUIT_REWARD)
#if defined(PRE_ADD_REMOTE_QUEST)
	int nAcceptWaitRemoteQuestCount;
	int AcceptWaitRemoteQuestList[MAX_PLAY_QUEST];
#endif
};

struct VIMACheckLastDungeonInfo
{
	UINT	uiAccountDBID;
	INT64	biCharacterDBID;
	int		iManagedID;
	int		iRoomID;
};

struct MAVICheckLastDungeonInfo
{
	UINT	uiAccountDBID;
	INT64	biCharacterDBID;
	bool	bIsCheck;
	WCHAR	wszPartyName[PARTYNAMELENMAX];
};

struct MAGACheckLastDungeonInfo
{
	char	cWorldSetID;
	UINT	uiAccountDBID;
	INT64	biCharacterDBID;
	int		iRoomID;
};

struct GAMACheckLastDungeonInfo
{
	UINT	uiAccountDBID;
	INT64	biCharacterDBID;
	bool	bIsCheck;
	WCHAR	wszPartyName[PARTYNAMELENMAX];
};

struct VIMAConfirmLastDungeonInfo
{
	UINT	uiAccountDBID;
	INT64	biCharacterDBID;
	int		iManagedID;
	int		iRoomID;
	bool	bIsOK;
#if defined( PRE_PARTY_DB )
	BreakInto::Type::eCode BreakIntoType;
#endif // #if defined( PRE_PARTY_DB )
};

struct MAVIConfirmLastDungeonInfo
{
	int		iRet;
	UINT	uiAccountDBID;
#if defined( PRE_PARTY_DB )
	BreakInto::Type::eCode BreakIntoType;
#endif // #if defined( PRE_PARTY_DB )
};

struct GAMAConfirmLastDungeonInfo
{
	int		iRet;
	UINT	uiAccountDBID;
#if defined( PRE_PARTY_DB )
	BreakInto::Type::eCode BreakIntoType;
#endif // #if defined( PRE_PARTY_DB )
};

struct MAGADeleteBackupDungeonInfo
{
	UINT	uiAccountDBID;
	INT64	biCharacterDBID;
	int		iRoomID;
};

#if defined(PRE_ADD_QUICK_PVP)
//VIMA_MAKEQUICKPVP_ROOM
struct VIMAMakeQuickPvPRoom
{
	UINT	uiMasterAccountDBID;
	UINT	uiSlaveAccountDBID;
};
//MAVI_MAKEQUICKPVP_ROOM
struct MAVIMakeQuickPvPRoom
{
	int		nRet;
	UINT	uiMasterAccountDBID;
	UINT	uiSlaveAccountDBID;
};
#endif

struct DBGetGuildRewardItem
{
	TGuildUID GuildUID;				// 길드 UID
};

#if defined (PRE_ADD_BESTFRIEND)
struct MASearchBestFriend
{
	int nRet;
	bool bAck;
	UINT nAccountDBID;
	WCHAR wszName[NAMELENMAX];

	char cJob;
	char cLevel;
};

struct MARegistBestFriend
{
	UINT nAccountDBID;
	INT64 biCharacterDBID;
	WCHAR wszFromName[NAMELENMAX];

	UINT nToAccountDBID;
	WCHAR wszToName[NAMELENMAX];

	int nRegistItemID;
};

struct MARegistBestFriendResult
{
	UINT nAccountDBID;
	int	 nRet;
	bool bAck;
	INT64 biFromCharacterDBID;
	UINT nToAccountDBID;
	INT64 biToCharacterDBID;
	WCHAR wszFromName[NAMELENMAX];
	WCHAR wszToName[NAMELENMAX];
};

struct MACompleteBestFriend
{
	UINT nFromAccountDBID;
	
	// 신청자
	UINT nToAccountDBID;
	INT64 biCharacterDBID;
	WCHAR wszName[NAMELENMAX];

	int nItemID;
};

struct MAEditBestFriendMemo
{
	UINT nFromAccountDBID;
	UINT nToAccountDBID;
	INT64 biToCharacterDBID;
	WCHAR wszMemo[BESTFRIENDMEMOMAX];
};

struct MACancelBestFriend
{
	bool bCancel;
	UINT nAccountDBID;			// 상대방 어카운트코드(게임서버에서만 사용)
	WCHAR wszFromName[NAMELENMAX];
	WCHAR wszToName[NAMELENMAX];
};

struct MACloseBestFriend
{
	UINT nAccountDBID;			// 상대방 어카운트코드(게임서버에서만 사용)
	WCHAR wszFromName[NAMELENMAX];
	WCHAR wszToName[NAMELENMAX];
};

struct MALevelUpBestFriend
{
	UINT nAccountDBID;			// 상대방 어카운트코드(게임서버에서만 사용)	
	BYTE cLevel;
	WCHAR wszName[NAMELENMAX];
};
#endif


#if defined( PRE_WORLDCOMBINE_PARTY )
struct MADelWorldParty
{
	int nPrimaryIndex;	// 삭제된 월드파티룸 키
};

struct MAGetWorldPartyMember
{
	UINT nAccountDBID;
	INT64 biCharacterDBID;
	int iServerID;
	TPARTYID PartyID;
	int iRoomID;	
};

struct GAMASendWorldPartyMember
{
	UINT nAccountDBID;
	INT64 biCharacterDBID;
	int nRetCode;
	TPARTYID PartyID;
	int nCount;
	Party::MemberData MemberData[PARTYCOUNTMAX];
};

#if defined( PRE_FIX_WORLDCOMBINEPARTY )
struct GAMAWorldCombineGameserver
{
	bool bFlag;
};
#endif
#endif

#if defined( PRE_PRIVATECHAT_CHANNEL )
struct MAAddPrivateChannel
{
	char cWorldSetID;
	int nManagedID;				// 서버 관리 ID
	TPrivateChatChannelInfo tPrivateChatChannel;
};

struct MAModPrivateChannel
{
	char cWorldSetID;
	int nManagedID;				// 서버 관리 ID
	INT64 nPrivateChatChannelID;
	PrivateChatChannel::Common::eModType eType;
	int nPassWord;
	INT64 biCharacterDBID;
};

struct MAAddPrivateChannelMember
{
	char cWorldSetID;
	int nManagedID;				// 서버 관리 ID
	INT64 nPrivateChatChannelID;
	TPrivateChatChannelMember Member;
};

struct MAInvitePrivateChannelMember
{
	char cWorldSetID;
	INT64 nPrivateChatChannelID;
	UINT nMasterAccountDBID;	
	UINT nInviteAccountDBID;
	WCHAR wszInviteName[NAMELENMAX];
};

struct MAInvitePrivateChannelMemberResult
{
	char cWorldSetID;
	int nRet;
	UINT nMasterAccountDBID;	
};

struct MADelPrivateChannelMember
{
	char cWorldSetID;
	int nManagedID;				// 서버 관리 ID
	PrivateChatChannel::Common::eModType eType;
	INT64 nPrivateChatChannelID;	
	INT64 biCharacterDBID;
	WCHAR wszName[NAMELENMAX];
};

struct MAKickPrivateChannelMemberResult
{
	UINT nAccountDBID;
	INT64 biCharacterDBID;
};

struct MAModPrivateChannelMemberName
{	
	char cWorldSetID;
	int nManagedID;	
	INT64 nPrivateChatChannelID;	
	INT64 biCharacterDBID;
	WCHAR wszName[NAMELENMAX];
};
#endif

#if defined( PRE_WORLDCOMBINE_PVP )

struct MADelWorldPvPRoom
{	
	int nRoomIndex;
};

struct MAWorldPvPRoomJoinResult
{		
	UINT uiAccountDBID;
	int nRet;		
};

struct MAWorldPvPRoomGMCreateResult
{		
	UINT uiAccountDBID;
	UINT uiPvPIndex;
	int nRet;		
	TWorldPvPRoomDBData Data;
};

struct VIMAWorldPvPRoomBreakInto
{
	UINT	uiAccountDBID;
	INT64	biCharacterDBID;
	int		nManagedID;
	int		nRoomIndex;
	int		nRoomID;
	bool    bPvPLobby;
	UINT	uiTeam;
	BreakInto::Type::eCode BreakIntoType;
};

#endif

#if defined( PRE_ADD_PVP_COMBOEXERCISE )
struct MAGAChangeRoomMaster
{
	//UINT	uiAccountDBID;
	UINT	uiRoomID;
	INT64	biRoomMasterCharacterDBID;
	UINT	uiRoomMasterSessionID;
};
#endif // #if defined( PRE_ADD_PVP_COMBOEXERCISE )

#if defined(PRE_ADD_MUTE_USERCHATTING)
struct MAMuteUserFind	//GAMA_MUTE_USERFIND, VIMA_MUTE_USERFIND
{
	UINT uiGmAccountID; //제재를 내린 GM AccountID
	WCHAR wszMuteUserName[NAMELENMAX]; // 대상 캐릭터명
	int nMuteMinute; //제재 시간(분)
#if defined(PRE_ADD_MULTILANGUAGE)
	char cSelectedLanguage;
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)
};
struct MAMuteUserChat	//MAGA_MUTE_USERCHAT, MAVI_MUTE_USERCHAT
{
	UINT uiGmAccountID; //제재 내린 GM AccountID
	UINT uiMuteUserAccountID; //제재 대상 AccountID
	int nMuteMinute; //제재 시간(분)
};
#endif	// #if defined(PRE_ADD_MUTE_USERCHATTING)

#if defined( PRE_MOD_71820 )
struct GAMANotifyMail
{
	UINT nToAccountDBID;		// 받는이 AccountDBID
	INT64 biToCharacterDBID;
	short wTotalMailCount;		// 총 우편수
	short wNotReadMailCount;	// 읽지 않은 메일
	short w7DaysLeftMailCount;	// 만료경고 메일
	bool bNewMail;
};
#endif // #if defined( PRE_MOD_71820 )

#if defined( PRE_ALTEIAWORLD_EXPLORE )
struct MAAlteiaWorldSendTicket
{	
	char cWorldSetID;
	UINT nAccountDBID;
	WCHAR wszRecvCharacterName[NAMELENMAX];
	INT64 biSendCharacterDBID;
	WCHAR wszSendCharacterName[NAMELENMAX];
};

struct MAAlteiaWorldSendTicketResult
{	
	int nRetCode;
	UINT nSendAccountDBID;
	INT64 biSendCharacterDBID;
};
#endif

#if defined( PRE_ADD_CHNC2C )
struct MAC2CGetCoinBalance
{	
	INT64 biCharacterDBID;
	char szSeqID[64];
};

struct MAC2CGetCoinBalanceResult
{
	int nRetCode;	
	INT64 biCoinBalance;
	char szBookID[50];
	char szSeqID[64];	
};

struct MAC2CAddCoin
{
	UINT uiAccountDBID;
	INT64 biCharacterDBID;
	INT64 biAddCoin;
	char szBookID[50];
	char szSeqID[64];
};

struct MAC2CAddCoinResult
{
	int nRetCode;	
	char szSeqID[64];
};

struct MAC2CReduceCoin
{
	UINT uiAccountDBID;
	INT64 biCharacterDBID;
	INT64 biReduceCoin;
	char szBookID[50];
	char szSeqID[64];
};

struct MAC2CReduceCoinResult
{
	int nRetCode;	
	char szSeqID[64];
};
#endif //#if defined( PRE_ADD_CHNC2C )

#if defined(PRE_DRAGONBUFF )
struct MAApplyWorldBuff
{
	WCHAR wszCharacterName[NAMELENMAX];
	int	nManagedID;
	int nItemID;
	int nMapIdx;
};
#endif

#if defined(PRE_ADD_DWC)
struct MAInviteDWCTeamMember	//VIMA_INVITE_DWCTEAMMEMB / MAVI_INVITE_DWCTEAMMEMB
{	
	UINT		nTeamID;							// 팀아이디
	UINT		nAccountDBID;						// 길드초대자 DBID
	WCHAR		wszFromCharacterName[NAMELENMAX];	// 캐릭터 이름
	WCHAR		wszTeamName[GUILDNAME_MAX];			// 팀명	
	WCHAR		wszToCharacterName[NAMELENMAX];		// 초대 받은 유저
};

struct MAInviteDWCTeamMemberAck	//VIMA_INVITE_DWCTEAMMEMB_ACK / MAVI_INVITE_DWCTEAMMEMB_ACK
{
	int		nRetCode;					// 결과코드
	UINT	nInviterDBID;				// 결과를 받아야할 유저
	UINT	nInviteeDBID;				// 결과를 보낸 유저(초대받은유저)
	bool	bAck;						// 요청/결과
	WCHAR	wszInvitedName[NAMELENMAX];	// 초대받은 사람 이름
};

struct MAAddDWCTeamMember	//VIMA_ADD_DWCTEAMMEMB / MAVI_ADD_DWCTEAMMEMB / MAGA_ADD_DWCTEAMMEMB
{
	UINT nTartgetUserAccountDBID;			// 정보를 받을 유저
	UINT nTeamID;							// 팀 ID
	UINT nAccountDBID;						// 가입한 사용자 계정 DBID
	INT64 biCharacterDBID;					// 가입한 캐릭터 DBID
	TP_JOB nJob;							// 캐릭터 전직직업
	TCommunityLocation Location;			// 현재 위치 (로그인 여부도 본 멤버의 상태로 체크)
	WCHAR wszCharacterName[NAMELENMAX];		// 가입한 캐릭터 이름
	bool bAlredySentByVillage;				// 빌리지 서버에서 유저에게 패킷 전송했는지 체크
};

struct MALeaveDWCTeamMember // VIMA_DEL_DWCTEAMMEMB / MAVI_DEL_DWCTEAMMEMB / MAGA_DEL_DWCTEAMMEMB
{
	UINT nTartgetUserAccountDBID;
	int nRet;
	UINT nTeamID;
	INT64 biLeaveUserCharacterDBID;
	WCHAR wszCharacterName[NAMELENMAX];
	bool bAlredySentByVillage;				// 빌리지 서버에서 유저에게 패킷 전송했는지 체크
};

struct MADismissDWCTeam	//VIMA_DISMISS_DWCTEAM / MAVI_DISMISS_DWCTEAM / MAGA_DISMISS_DWCTEAM
{
	UINT nTartgetUserAccountDBID;
	int nRet;
	UINT nTeamID;
	bool bAlredySentByVillage;				// 빌리지 서버에서 유저에게 패킷 전송했는지 체크
};

struct MAChangeDWCTeamMemberState	//VIMA_CHANGE_DWCTEAMMEMB_STATE
{
	UINT nTartgetUserAccountDBID;
	UINT nTeamID;
	INT64 biCharacterDBID;
	TCommunityLocation Location;
	bool bLogin;
};

struct VIMADWCTeamMemberList	//VIMA_DWC_TEAMMEMBERLIST
{
	UINT nAccountDBID;
	UINT nTeamID;
	BYTE cCount;
	INT64 MemberListCharacterDBID[DWC::DWC_MAX_MEMBERISZE];
};

// VIMA_DWC_TEAMCHAT / GAMA_DWC_TEAMCHAT / MAVI_DWC_TEAMCHAT / MAGA_DWC_TEAMCHAT
struct MADWCTeamChat
{
	UINT nTeamID;			// 길드 UID
	UINT nAccountDBID;		// 계정 DBID
	INT64 biCharacterDBID;	// 캐릭터 DBID
	short nLen;				// 채팅 길이
	WCHAR wszChatMsg[CHATLENMAX];	// 채팅 메시지
};

struct MADWCUpdateScore	// GAMA_DWC_UPDATE_SCORE / MAVI_DWC_UPDATE_SCORE
{
	UINT nTeamID;
	TDWCScore DWCScore;
};
#endif

#if defined( PRE_PVP_GAMBLEROOM )
struct MACreateGambleRoom
{
	bool bFlag;
	int nTotalGambleRoomCount;
};
struct MADelGambleRoom
{
	bool bFlag;
};
#endif
#pragma pack(pop)
