#pragma once

#include "DNUserItem.h"
#include "DNUserQuest.h"
#include "DNCheatCommand.h"
#include "DNGMCommand.h"
#include "DNSchedule.h"
#include "DNUserSendManager.h"
#include "../../GameCommon/DNNotifierCommon.h"
#include "DnItemCompounder.h"

/*---------------------------------------------------------------------------------------

	CDNUserBase

	Village::UserSession과 Game::UserSession에 공통부분을 이곳에서 처리
	같은거 두곳에 만드는거 귀찮아서 만들었음.

	Mission, Appellation, Item, Quest는 그쪽 클래스에서 알아서 공통으로 쓸것임
	그 외적인 애들은 다 이쪽으로~

---------------------------------------------------------------------------------------*/


class CDNParty;
class CDNFriend;
class CDNIsolate;
class CDNMissionSystem;
class CDNEventSystem;
class CDNAppellation;
class CDNGesture;
class CDNRestraint;
class CDNDBConnection;
class CDNEffectRepository;
class CDNTimeEventSystem;
class CDNCommonVariable;
#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
class CReputationSystemRepository;
#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
#ifdef _GPK
#include "CryptPacket.h"
class CCryptoKey;
#endif
#if defined( PRE_ADD_SECONDARY_SKILL )
class CSecondarySkillRepository;
#endif // #if defined( PRE_ADD_SECONDARY_SKILL )
class CDNUserEventHandler;
#if defined (PRE_ADD_BESTFRIEND)
class CDNBestFriend;
#endif
#if defined( PRE_PRIVATECHAT_CHANNEL )
class CDNPrivateChaChannel;
#endif
#if defined( PRE_ADD_STAMPSYSTEM )
class CDNStampSystem;
#endif // #if defined( PRE_ADD_STAMPSYSTEM )

class CDNUserBase: public CDNUserSendManager, public CDNSchedule
{
public:
	enum eTick
	{
		eTickNpcTalk = 0,		// CS_NPCTALK 요청을 지연시킴
		eTickCnt,
	};

	enum eCheckPapering
	{
		eChatPapering = 0,
		eGesturePapering,

		eCheckPaperingAmount = 2,
	};

	// 패킷데이터 한번에 Copy 하기 위해서 상속 안받음.
	struct TPvPLadderScoreInfoByJobServer
	{
		TPvPLadderScoreInfoByJobServer():bInit(true){}

		bool bInit;
		TPvPLadderScoreInfoByJob Data;
	};

	struct TDatabaseMessageSequence
	{
		char cMainCmd;
		char cSubCmd;
		DWORD dwAddTime;
	};

protected:
	UINT m_nAccountDBID;				// 계정디비 아이디
	INT64 m_biCharacterDBID;				// 캐릭디비 아이디
	UINT m_nSessionID;

	WCHAR m_wszAccountName[IDLENMAX];	// 계정이름
	char m_szAccountName[IDLENMAX];	// 계정 char형(샨다 피로도 보낼때마다 변환하기 귀찮아서 만들었음;;)
	char m_szCharacterName[NAMELENMAX];

	bool m_bPCBang;				// 왠지 pcroom보다는 pcbang이 좋다! ㅋㅋㅋ
	char m_cPCBangGrade;
	int m_nPcBangBonusExp;		// 몬스터다이시 검사하는데 잦은 부분이라 로컬변수로 세팅해두고 찾는 부분을 줄인다.
	int m_iHackAbuseDBValue;
	int m_iHackAbuseDBCallCount;
	int m_iHackPlayRestraintValue;
	int m_iHackResetRestraintValue;
	int m_nHackAbuseCharacterCntWithoutMe;		//현재 세션캐릭터를 제외한 어뷰즈 걸린 캐릭터 카운트
#if defined(PRE_ADD_ANTI_CHAT_SPAM)
	bool m_bSpammer;
#endif

	int m_nWorldSetID;				//월드아이디
	BYTE m_cVillageID;				//현재 존아이디
	int m_nChannelID;
	USHORT m_wGameID;
	int m_nRoomID;

	bool m_bLoadUserData;

	CDNDBConnection * m_pDBCon;
	BYTE m_cDBThreadID;

	TChracterLevel m_OwnCharacterLevelList[CHARCOUNTMAX];	

	TUserData m_UserData;
	TKeySetting	m_KeySetting;
	TPadSetting m_PadSetting;
	TMasterSystemData m_MasterSystemData;
	TProfile m_Profile;
	TPvPLadderScoreInfo m_PvPLadderScoreInfo;
	TPvPLadderScoreInfoByJobServer m_PvPLadderScoreInfoByJob;
	TPvPGhoulScores m_PvPTotalGhoulScores; // 전체 구울점수	
	TPvPGhoulScores	m_PvPAddGhoulScores;	// 한판당 구울점수
	TUnionReputePointInfo m_UnionReputePointInfo;
	int m_nGuildWarRewardFestivalPoint;		// 보상으로 받을 축제 포인트
	int m_nGuildWarRewardGuildPoint;		// 보상으로 받을 길드 포인트
	__int64 m_biGuildWarFestivalPoint;		// 길드전 축제 포인트	
	short m_wGuildWarScheduleID;			// 길드전 스케쥴ID	

	CDNUserItem *m_pItem;
	CDNUserQuest* m_pQuest;
	CDNCheatCommand* m_pCheatCommand;
	CDNGMCommand* m_pGMCommand;
	CDNFriend * m_pFriend;
	CDNIsolate * m_pIsolate;
	CDNMissionSystem *m_pMissionSystem;
	CDNEventSystem *m_pEventSystem;
	CDNAppellation *m_pAppellation;
	CDNRestraint * m_pRestraint;
#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
	CReputationSystemRepository* m_pReputationSystem;
#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
#if defined( PRE_ADD_SECONDARY_SKILL )
	CSecondarySkillRepository* m_pSecondarySkillRepository;
#endif // #if defined( PRE_ADD_SECONDARY_SKILL )
#if defined (PRE_ADD_BESTFRIEND)
	CDNBestFriend* m_pBestFriend;
#endif
	CDNGesture * m_pGesture;		//상점이라든가 추가되면 살릴예정
	CDNTimeEventSystem *m_pTimeEventSystem;
	CDNUserEventHandler* m_pUserEventHandler;
	CDNCommonVariable *m_pCommonVariable;

	std::vector<TalkParam> m_TalkParamList;
	TALK_PARAGRAPH m_LastTalkParagraph;		// api_npc_NextScript 로 점프 할때는 한번만 m_LastTalkParagraph 체크를 스킵한다
	bool m_bSkipParagraphCheck;
	// next_talk 나 next_script 로 npc 가 응답을 보냈는지 여부.
	// 스크립트 오류 발생시 응답을 못보낼 경우 npc 에 붙잡히는 현상 때문에 에러 응답이라도 보내기 위해 체크하는 값
	bool m_bCalledNpcResponse; 
	bool m_bIsNpcTalk; 

	UINT m_nExchangeTargetSessionID;	// 거래하는 상대방 세션아이디
	UINT m_nExchangeSenderSID;	// 거래요청 관련 아이디
	UINT m_nExchangeReceiverSID;	// 거래수락 관련 아이디

	DWORD m_dwLastMessageTick;
	BYTE m_cLastMainCmd;
	BYTE m_cLastSubCmd;

	int m_nCashBalance;	// 캐쉬(total)
#if defined(_US)
	int m_nNxAPrepaid;	// not refund balance
	int m_nNxACredit;	// m_biCashBalance - m_biNxAPrepaid
#endif	// #if defined(_US)

	bool m_bHide;	// 운영자 투명상태(?)

	//일단 유저오브젝트에서 가지고 있자
	bool m_bNeedUpdateOption;
	TGameOptions m_GameOption;

	bool m_bIsSetSecondAuthPW;		// 2차 인증 비밀번호 설정 여부
	bool m_bIsSetSecondAuthLock;	// 2차 인증 계정 Lock 여부

#if defined(_CH)
	char m_cFCMState;			// eFCMState
	int m_nFCMOnlineMin;		// online 시간(분)
#endif	// _CH
	bool	m_bSecurityUpdate;
#if defined(_HSHIELD)
	AHNHS_CLIENT_HANDLE	m_hHSClient;	// [확장 서버연동] 클라이언트 관리 개체
	DWORD m_dwCheckLiveTick;			// live check
	DWORD m_dwHShieldResponseTick;		// CRC보내고 응답받을때까지 시간 체크
#endif	// #if defined(_HSHIELD)
#if defined(_GPK)
	DWORD m_dwCheckGPKTick;		// live check
	DWORD m_dwRecvGPKTick;
	volatile bool m_bRespondErr;
#endif	// #if defined(_GPK)
	bool m_bFriendBonus;
	int m_iRemoteEnchantItemID;
	int m_iRemoteItemCompoundItemID;

public:
#if defined(_KR) || defined(_US)
	UINT m_nNexonSN;
#endif	// #if defined(_KR)
	UINT m_nVehicleObjectID;
	DNNotifier::Repository	m_NotifierRepository;

#if defined(PRE_ADD_MISSION_COUPON)
	int m_nExpiredPetID;	// 장착중이던 기간만료 펫 정보 -> OnPetExpired 미션 호출에 사용
#endif
	short m_sUserWindowState;	// UI상태(?) 유저가 지금 뭐하는 상태인가 (bit연산)

	int m_nShopID;	// shop id 세팅하기
#if defined(PRE_ADD_REMOTE_OPENSHOP)
	bool m_bRemoteShopOpen;
#endif	// #if defined(PRE_ADD_REMOTE_OPENSHOP)

	int m_nCutSceneID;	// 플레이하고있는 컷씬 아이디(임시)

	int m_nClickedNpcID;	// 클릭한 npcid
	UINT m_nClickedNpcObjectID;	// 클릭한 Npc OjbectID

	bool m_bAdult;	// 어른인지 아닌지(피로도)
	char m_cAge;	// 나이(Nexon유료)
	BYTE m_cDailyCreateCount;	// 일일 캐릭터 생성가능 횟수
	int m_nPrmInt1;		// 파라메터 1(국가별 다목적)
	char m_cLastServerType;		// 최근 서버 타입(eServerType / 0 : 로그인, 2 : 빌리지, 3 : 게임)

	bool m_bCharOutLog;	// true면 FinalUser 하면서 로그를 남긴다 090326

	bool m_bCertified;			// 인증완료 여부(QUERY_CHECKAUTH 를 보내기 전에 false 로 세팅해야 함)
	DWORD m_dwCertifyingTick;	// 인증시간 체크(인증체크 요청 후 CHECKAUTHLIMITTERM 간격 안에 처리되어야 함)(파티멤버 체크(PVP 역시 ???))
	INT64 m_biCertifyingKey;	// 인증키

#if defined(PRE_ADD_MULTILANGUAGE)
	MultiLanguage::SupportLanguage::eSupportLanguage m_eSelectedLanguage;
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)
#ifdef PRE_ADD_COMEBACK
	bool m_bComebackUser;
#endif		//#ifdef PRE_ADD_COMEBACK
#if defined(PRE_ADD_GAMEQUIT_REWARD)
	bool m_bReConnectUserReward;
	GameQuitReward::RewardType::eType m_eUserGameQuitRewardType;
#endif	// #if defined(PRE_ADD_GAMEQUIT_REWARD)
#if defined(_ID)
	char m_szMacAddress[MACADDRLENMAX];
	char m_szKey[KREONKEYMAX];
	DWORD m_dwKreonCN;
#endif

#if defined(_GPK)
	int m_nCodeIndex;
	const unsigned char *m_pCode;
	int m_nCodeLen;
#endif	// _GPK

	// nexon pcbang
	WCHAR m_wszVirtualIp[IPLENMAX];
	char m_szVirtualIp[IPLENMAX];

	BYTE m_szMID[MACHINEIDMAX];
	DWORD m_dwGRC;

#ifdef PRE_ADD_GACHA_JAPAN
	int m_nGachaponShopID;
#endif // PRE_ADD_GACHA_JAPAN

	mutable DWORD m_dwTick[eTickCnt+1];		// 틱 카운트(각 개체의 특정 동작의 시간범위를 관리할 경우가 발생 시 여기에 공통적으로 등록하여 사용할 목적)

	char m_cLastStageClearRank;	// 가장 최근 스테이지 완료 랭크(SSS : 0 / SS : 1 / S : 2 / A : 3 / B : 4 / C : 5 / D : 6)
	TGuildSelfView m_GuildSelfView;		// 길드 시각정보(개인)

	bool m_bVoiceAvailable;		//보이스챗이 가능한지
#ifdef _USE_VOICECHAT
	UINT m_nVoiceChannelID;		//보이스채널에 속해있을 경우
	BYTE m_cIsTalking;
	int m_nVoiceJoinType;
	UINT m_nVoiceMutedList[PARTYCOUNTMAX];
#endif

	DWORD m_dwNpcTalkLastIndexHashCode;		// NPC 대사의 가장 마지막 인덱스 기록
	DWORD m_dwNpcTalkLastTargetHashCode;	// NPC 대사의 가장 마지막 대사파일 기록

	BYTE m_cVIPRebirthCoin;
	bool m_bVIP;
	int m_nVIPTotalPoint;			// 총 VIP 포인트
	__time64_t m_tVIPEndDate;		// VIP 기간 끝 일자
	bool m_bVIPAutoPay;				// 자동결제
	int m_nVIPBonusExp;		// 몬스터다이시 검사하는데 잦은 부분이라 로컬변수로 세팅해두고 찾는 부분을 줄인다.

	int m_nExpandNestClearCount;
#if defined(PRE_ADD_TSCLEARCOUNTEX)
	int m_nExpandTreasureStageClearCount;
#endif	// #if defined(PRE_ADD_TSCLEARCOUNTEX)
	bool m_bIntroducer;		// 샨다 프로모션 추천인

	// 도배방지
	DWORD m_dwCheckPaperingRemainTime[eCheckPaperingAmount];
	std::list<DWORD> m_listRecentCheckTime[eCheckPaperingAmount];

	CDNEffectRepository* m_pEffectRepository;

	// 상점팔기 딜레이
	DWORD m_dwResellItemTick;
#if defined(PRE_FIX_74387)
	bool m_bNowResellItem;
#endif

	DWORD m_dwCheckTcpPing;
	std::pair<DWORD,UINT> m_pairTcpPing;	// <Tick/Count>

	bool m_bSaleAbortListSended;
	Shop::Type::eCode m_ShopType;
	bool m_bFinalize;

	std::vector <TDatabaseMessageSequence> m_vDBMessageSequencer;

#if defined( PRE_ADD_LIMITED_SHOP )	
#if defined( PRE_FIX_74404 )
	std::list<LimitedShop::LimitedItemData> m_LimitedShopBuyedItemList;
#else // #if defined( PRE_FIX_74404 )
	std::map<int, LimitedShop::LimitedItemData> m_LimitedShopBuyedItem;
#endif // #if defined( PRE_FIX_74404 )
#endif // #if defined( PRE_ADD_LIMITED_SHOP )

#if defined( PRE_ADD_TOTAL_LEVEL_SKILL )
	int m_nTotalLevelSKillLevel;			// 통합레벨
	int m_nTotalLevelSKillData[TotalLevelSkill::Common::MAXSLOTCOUNT];			// 통합스킬
	bool m_bTotalLevelSkillCashSlot[TotalLevelSkill::Common::MAXSLOTCOUNT];		// 캐시슬롯
	INT64 m_nTotalLevelSkillCashSlot[TotalLevelSkill::Common::MAXSLOTCOUNT];	// 캐시슬롯	
#endif

#if defined( PRE_ADD_NEWCOMEBACK )
	int m_nComebackAppellation;
#endif
#ifdef PRE_ADD_JOINGUILD_SUPPORT
	BYTE m_cGuildJoinLevel;					//길드가입 레벨(최초한번)
	bool m_bWasRewardedGuildMaxLevel;		//길드에서 만렙찍을 경우 보상을 받았는지 플래그
	bool m_bPartyBegginerGuild;				//파티상태에서 초보길드원인지
#endif		//#ifdef PRE_ADD_JOINGUILD_SUPPORT

#if defined(_KRAZ)
	bool m_bShutdownDetach;
	TShutdownData m_ShutdownData;
#endif	// #if defined(_KRAZ)

#if defined(PRE_RECEIVEGIFTALL)
	int m_nReceiveGiftPageCount;
	std::vector<INT64> m_VecReceiveGiftResultList;
#endif	// #if defined(PRE_RECEIVEGIFTALL)

#if defined( PRE_ADD_STAMPSYSTEM )
	CDNStampSystem * m_pStampSystem;
#endif // #if defined( PRE_ADD_STAMPSYSTEM )
#if defined(PRE_ADD_CP_RANK)
	char m_cStageClearBestRank;
	int m_nStageClearBestCP;
#endif //#if defined(PRE_ADD_CP_RANK)

#if defined( PRE_ADD_NEW_MONEY_SEED )
	INT64 m_nSeedPoint;
#endif
public:
	CDNUserBase(void);
	virtual ~CDNUserBase(void);

	void FinalizeEvent();
	void SetDBConInfo( CDNDBConnection* pCon, BYTE cThreadID ){ m_pDBCon = pCon; m_cDBThreadID = cThreadID; }

	bool GetLoadUserData() {return m_bLoadUserData;}

	void SetSecurityUpdateFlag( bool bFlag ){ m_bSecurityUpdate = bFlag; }
	bool GetSecurityUpdateFlag(){ return m_bSecurityUpdate; }
	const TPvPLadderScoreInfo* GetPvPLadderScoreInfoPtr(){ return &m_PvPLadderScoreInfo; }
	const TPvPLadderScoreInfoByJobServer* GetPvPLadderScoreInfoPtrByJob(){ return &m_PvPLadderScoreInfoByJob; }
	const TUnionReputePointInfo* GetUnionReputePointInfoPtr() { return &m_UnionReputePointInfo; }

	virtual void GuildWarReset();
	inline short GetGuildWarScheduleID() { return m_wGuildWarScheduleID; };
	inline void SetGuildWarScheduleID(short wScheduleID) { m_wGuildWarScheduleID = wScheduleID;};
	inline __int64 GetGuildWarFestivalPoint() { return m_biGuildWarFestivalPoint; };
	inline void SetGuildWarFestivalPoint(__int64 biGuildWarFestivalPoint) { m_biGuildWarFestivalPoint=biGuildWarFestivalPoint;};
	inline int GetGuildWarRewardFestivalPoint() { return m_nGuildWarRewardFestivalPoint; };
	inline void SetGuildWarRewardFestivalPoint(int nGuildWarRewardFestivalPoint) { m_nGuildWarRewardFestivalPoint=nGuildWarRewardFestivalPoint; };
	inline int GetGuildWarRewardGuildPoint() { return m_nGuildWarRewardGuildPoint; };
	inline void SetGuildWarRewardGuildPoint(int nGuildWarRewardGuildPoint) { m_nGuildWarRewardGuildPoint=nGuildWarRewardGuildPoint; };	
	void DelGuildWarFestivalPoint(INT64 biPoint);

	//UserData
	virtual bool LoadUserData(TASelectCharacter *pSelect);
#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
	virtual bool LoadReputation( TAGetListNpcFavor* pA );
#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
	virtual bool SaveUserData();

	virtual void DoUpdate(DWORD dwCurTick);
	virtual void CashMessageProcess(int nMainCmd, int nSubCmd, char *pData, int nLen);
	virtual void DBMessageProcess(int nMainCmd, int nSubCmd, char *pData, int nLen);

	//DBMessage
	virtual void OnDBRecvAuth(int nSubCmd, char *pData);
	virtual void OnDBRecvCharInfo(int nSubCmd, char *pData);
	virtual void OnDBRecvEtc(int nSubCmd, char *pData);
	virtual void OnDBRecvQuest(int nSubCmd, char *pData);
	virtual void OnDBRecvMission(int nSubCmd, char *pData);
	virtual void OnDBRecvSkill(int nSubCmd, char *pData);
	virtual void OnDBRecvFriend(int nSubCmd, char *pData);
	virtual void OnDBRecvIsolate(int nSubCmd, char *pData);
	virtual void OnDBRecvPvP(int nSubCmd, char *pData);
	virtual void OnDBRecvDarkLair(int nSubCmd, char *pData);
	virtual void OnDBRecvGuild(int nSubCmd, char *pData);
	virtual void OnDBRecvMail(int nSubCmd, char *pData);
	virtual void OnDBRecvMarket(int nSubCmd, char *pData);
	virtual void OnDBRecvItem(int nSubCmd, char *pData);
	virtual void OnDBRecvCash(int nSubCmd, char *pData);
	virtual void OnDBRecvMsgadjustment(int nSubCmd, char * pData) {}
#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
	virtual void OnDBRecvReputation(int nSubCmd, char * pData);
#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
	virtual void OnDBRecvMasterSystem(int nSubCmd, char* pData);
#if defined( PRE_ADD_SECONDARY_SKILL )
	virtual void OnDBRecvSecondarySkill(int nSubCmd, char* pData);
#endif // #if defined( PRE_ADD_SECONDARY_SKILL )
	virtual void OnDBRecvFarm(int nSubCmd, char * pData);
	virtual void OnDBRecvGuildRecruit(int nSubCmd, char * pData);
#if defined (PRE_ADD_DONATION)
	virtual void OnDBRecvDonation(int nSubCmd, char* pData) {}
#endif // #if defined (PRE_ADD_DONATION)
#if defined( PRE_PARTY_DB )
	virtual void OnDBRecvParty( int nSubCmd, char* pData );
#endif // #if defined( PRE_PARTY_DB )
#if defined (PRE_ADD_BESTFRIEND)
	virtual void OnDBRecvBestFriend(int nSubCmd, char* pData);
#endif
#if defined( PRE_PRIVATECHAT_CHANNEL )
	virtual void OnDBRecvPrivateChatChannel(int nSubCmd, char* pData);
#endif
#if defined( PRE_ALTEIAWORLD_EXPLORE )
	virtual void OnDBRecvAlteiaWorld(int nSubCmd, char* pData);
#endif
#if defined( PRE_ADD_STAMPSYSTEM )
	virtual void OnDBRecvStampSystem( int nSubCmd, char* pData );
#endif // #if defined( PRE_ADD_STAMPSYSTEM )
#if defined(PRE_ADD_DWC)
	virtual void OnDBRecvDWC( int nSubCmd, char* pData );
#endif

	//UserMessage
	virtual int OnRecvCharMessage(int nSubCmd, char *pData, int nLen);
	virtual int OnRecvItemMessage(int nSubCmd, char *pData, int nLen);
	virtual int OnRecvItemGoodsMessage(int nSubCmd, char *pData, int nLen);
	virtual int OnRecvTradeMessage(int nSubCmd, char *pData, int nLen);
	virtual int OnRecvCashShopMessage(int nSubCmd, char *pData, int nLen);
	virtual int OnRecvQuestMessage(int nSubCmd, char *pData, int nLen);
	virtual int OnRecvSystemMessage(int nSubCmd, char *pData, int nLen);
	virtual int OnRecvFriendMessage(int nSubCmd, char *pData, int nLen);
	virtual int OnRecvIsolateMessage(int nSubCmd, char *pData, int nLen);
	virtual int OnRecvGameOptionMessage(int nSubCmd, char *pData, int nLen);
	virtual int OnRecvRadioMessage(int nSubCmd, char *pData, int nLen);
	virtual int OnRecvGuildMessage(int nSubCmd, char *pData, int nLen);
	virtual int OnRecvEtcMessage(int nSubCmd, char *pData, int nLen);
	virtual int OnRecvChatRoomMessage(int nSubCmd, char *pData, int nLen);
	virtual int OnRecvMasterSystemMessage( int nSubCmd, char* pData, int nLen );
#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
	virtual int OnRecvReputationMessage( int nSubCmd, char* pData, int nLen );
#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
#if defined( PRE_ADD_SECONDARY_SKILL )
	virtual int OnRecvSecondarySkillMessage( int nSubCmd, char* pData, int nLen );
#endif // #if defined( PRE_ADD_SECONDARY_SKILL )
	virtual int OnRecvGestureMessage(int nSubCmd, char *pData, int nLen);
	virtual int OnRecvPlayerCustomEventUI(int nSubCmd, char * pData, int nLen);
	virtual int OnRecvGuildRecruitMessage( int nSubCmd, char* pData, int nLen );
#if defined (PRE_ADD_DONATION)
	virtual int OnRecvDonation(int nSubCmd, char* pData, int nLen);
#endif // #if defined (PRE_ADD_DONATION)
#if defined (PRE_ADD_BESTFRIEND)
	virtual int OnRecvBestFriendMessage( int nSubCmd, char* pData, int nLen );
#endif
#if defined( PRE_PRIVATECHAT_CHANNEL )
	virtual int OnRecvPrivateChatChannelMessage( int nSubCmd, char* pData, int nLen );
#endif

#if defined( PRE_ALTEIAWORLD_EXPLORE )
	virtual int OnRecvWorldAlteiaMessage( int nSubCmd, char* pData, int nLen );
#endif // #if defined( PRE_ALTEIAWORLD_EXPLORE )
#if defined(PRE_ADD_DWC)
	virtual int OnRecvDWCMessage( int nSubCmd, char* pData, int nLen );
#endif
#if defined(PRE_ADD_CHAT_MISSION)
	virtual int OnRecvMissionMessage( int nSubCmd, char* pData, int nLen );
#endif

	CDNDBConnection * GetDBConnection() { return m_pDBCon; }
	BYTE GetDBThreadID() { return m_cDBThreadID; }
	bool CheckDBConnection();

	TCharacterStatus *GetStatusData() { return &m_UserData.Status; }
	TPvPGroup *GetPvPData() { return &m_UserData.PvP; }
	TQuestGroup *GetQuestData() { return &m_UserData.Quest; }
	TMissionGroup *GetMissionData() { return &m_UserData.Mission; }
	TAppellationGroup *GetAppellationData() { return &m_UserData.Appellation; }
	TSkillGroup *GetSkillData( bool bEntireData = false );
	TTimeEventGroup *GetTimeEventData() { return &m_UserData.TimeEvent; }
	CDNTimeEventSystem *GetTimeEventSystem() { return m_pTimeEventSystem; }

	CDNUserItem *GetItem() { return m_pItem; }
	CDNUserQuest *GetQuest() { return m_pQuest; }
	CDNIsolate *GetIsolate() { return m_pIsolate; }
	CDNMissionSystem *GetMissionSystem() { return m_pMissionSystem; }
	CDNEventSystem *GetEventSystem() { return m_pEventSystem; }
	CDNAppellation *GetAppellation() { return m_pAppellation; }
	CDNGMCommand *GetGMCommand() { return m_pGMCommand; }
	CDNFriend *GetFriend() { return m_pFriend; }
	CDNCheatCommand *GetCheatCommand() { return m_pCheatCommand; }
	CDNRestraint *GetRestraint() { return m_pRestraint; }
	CDNEffectRepository* GetEffectRepository(){ return m_pEffectRepository; }
#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
	CReputationSystemRepository* GetReputationSystem(){ return m_pReputationSystem; }
#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
#if defined( PRE_ADD_SECONDARY_SKILL )
	CSecondarySkillRepository* GetSecondarySkillRepository(){ return m_pSecondarySkillRepository; }
#endif // #if defined( PRE_ADD_SECONDARY_SKILL )
	const TMasterSystemData* GetMasterSystemData(){ return &m_MasterSystemData; }
#if defined (PRE_ADD_BESTFRIEND)
	CDNBestFriend* GetBestFriend() {return m_pBestFriend;}
	void CloseBestFirend();
#endif // #if defined (PRE_ADD_BESTFRIEND)
	UINT GetVehicleObjectID() {return m_nVehicleObjectID;}

#if defined(PRE_ADD_MISSION_COUPON)
	void SetExpiredPetID( int nValue ) { m_nExpiredPetID = nValue; }
	int GetExpiredPetID() { return m_nExpiredPetID; }
#endif

	int GetWorldSetID() { return m_nWorldSetID; }
	BYTE GetVillageID() { return m_cVillageID; }
	int GetChannelID() { return m_nChannelID; }

	void SetRoomID(int nRoomID) { m_nRoomID = nRoomID; }
	virtual int GetRoomID() { return m_nRoomID; }

	void SetGameID(USHORT wGameID) { m_wGameID = wGameID; }
	USHORT GetGameID() { return m_wGameID; }

	int GetAccountLevel() { return m_UserData.Status.cAccountLevel; }
	bool IsCheckFirstVillage() { return m_UserData.Status.IsCheckFirstVillage; }
	void SetCheckFirstVillage(bool bFlag) { m_UserData.Status.IsCheckFirstVillage = bFlag; }
	int GetDefaultBody() { return m_UserData.Status.nDefaultBody; }
	int GetDefaultLeg() { return m_UserData.Status.nDefaultLeg; }
	int GetDefaultHand() { return m_UserData.Status.nDefaultHand; }
	int GetDefaultFoot() { return m_UserData.Status.nDefaultFoot; }
#if defined(PRE_ADD_DWC)
	bool IsDWCCharacter() { return GetAccountLevel() == AccountLevel_DWC; }
#endif	

	void SetAccountDBID(UINT nAccountDBID) { m_nAccountDBID = nAccountDBID; }
	UINT GetAccountDBID() { return m_nAccountDBID; }

	void SetCharacterDBID(INT64 biCharacterDBID) { m_biCharacterDBID = biCharacterDBID; }
	INT64 GetCharacterDBID() { return m_biCharacterDBID; }

	void SetAccountName(WCHAR *pAccountName);
	WCHAR *GetAccountName() { return m_wszAccountName; }
	char *GetAccountNameA() { return m_szAccountName; }

	WCHAR *GetCharacterName() { return m_UserData.Status.wszCharacterName; }
	char *GetCharacterNameA() { return m_szCharacterName; }
	char GetClassID() { return m_UserData.Status.cClass; }

	void SetCharacterName(const WCHAR* pwszName);

	void SetLastVillageGateNo(char cGateNo) { m_UserData.Status.cLastVillageGateNo = cGateNo; }
	char GetLastVillageGateNo() { return m_UserData.Status.cLastVillageGateNo; }
	int GetLastVillageMapIndex() { return m_UserData.Status.nLastVillageMapIndex; }
	void SetLastSubVillageMapIndex(int nMapIndex) { m_UserData.Status.nLastSubVillageMapIndex = nMapIndex; }
	int GetLastSubVillageMapIndex() { return m_UserData.Status.nLastSubVillageMapIndex; }
	int GetPosX() { return m_UserData.Status.nPosX; }
	int GetPosY() { return m_UserData.Status.nPosY; }
	int GetPosZ() { return m_UserData.Status.nPosZ; }

	virtual void SetMapIndex(int nMapIndex);
	virtual void SetLastMapIndex( int nMapIndex );
	int GetMapIndex() { return m_UserData.Status.nMapIndex; }

#if defined(PRE_FIX_74387)
	void SetNowResellItem(bool bNowResellItem) { m_bNowResellItem = bNowResellItem;}
#endif

#if defined(PRE_ADD_EXPUP_ITEM)
	virtual void ChangeExp(int nChangeExp, int nLogCode, INT64 biFKey, bool bAbsolute=false );		// changeexp만큼 더하거나 빼주기(+, -를 써서) -> nLogCode에 따라 biFKey값도 넣어준다(eCharacterExpChangeCode참조)
#else
	virtual void ChangeExp(int nChangeExp, int nLogCode, INT64 biFKey);		// changeexp만큼 더하거나 빼주기(+, -를 써서) -> nLogCode에 따라 biFKey값도 넣어준다(eCharacterExpChangeCode참조)
#endif
	virtual void SetExp(UINT nExp, int nLogCode, INT64 biFKey, bool bDBSave);				// exp만큼 값 변경
	unsigned int GetExp() { return m_UserData.Status.nExp; }
	virtual void SetLevel(BYTE cLevel, int nLogCode, bool bDBSave);
	unsigned char GetLevel();

	virtual void SetUserJob(BYTE cJob);
	BYTE GetUserJob() { return m_UserData.Status.cJob; }
	// Coin
	bool AddCoin(INT64 nCoin, int nLogCode, INT64 biFKey, bool bSend = true);
	bool DelCoin(INT64 nCoin, int nLogCode, INT64 biFKey, bool bSend = true);
	INT64 GetCoin();
	bool CheckEnoughCoin(INT64 nCheckCoin);	// nCheckCoin만큼 돈이 있는지...
	bool CheckMaxCoin(INT64 nCheckCoin);	// 42억을 넘기지않는지

	void SetWarehouseCoin(INT64 nCoin);		// nCoin으로 세팅
	void AddWarehouseCoin(INT64 nCoin, int nLogCode, INT64 biFKey);
	void DelWarehouseCoin(INT64 nCoin, int nLogCode, INT64 biFKey);
	INT64 GetWarehouseCoin();
	bool CheckEnoughWarehouseCoin(INT64 nCheckCoin);	// nCheckCoin만큼 돈이 있는지...
	bool CheckMaxWarehouseCoin(INT64 nCheckCoin);	// 42억을 넘기지 않는지

	// SkillPoint
	void SetSkillPoint(USHORT nPoint, char cSkillPage);
	void ChangeSkillPoint(unsigned short nPoint, int nSkillID, bool bDBSave, int nLogCode, char cSkillPage = DualSkill::Type::MAX);
	void SetCheatSkillPoint( unsigned short nPoint, int nSkillID );		// setsp 치트키 전용
	USHORT GetSkillPoint();		//현재 선택된 스킬트리 포인트 
	USHORT GetSkillPoint(int nSkillPageIndex);	//특정 스킬트리 포인트
	float GetAvailSkillPointRatioByJob( int nSkillID );

	// Fatigue
	void SetCheatMaxFatigue();
	void SetCheatFatigue(int nFatigue, int nWeeklyFatigue, int nPCBangFatigue, int nEventFatigue, int nVIPFatigue);
	virtual void SetDefaultMaxFatigue(bool bSend = true);
	virtual void SetDefaultMaxWeeklyFatigue(bool bSend = true);
#if defined(PRE_ADD_VIP)
	void SetDefaultVIPFatigue();	// VIP사고나서 피로도 세팅
#endif	// #if defined(PRE_ADD_VIP)
	void SetDefaultPCBangFatigue();
	virtual void SetFatigue(int nFatigue, int nWeeklyFatigue, int nPCBangFatigue, int nEventFatigue, int nVIPFatigue, bool bDBSave = true);
	virtual void DecreaseFatigue(int nGap);
	virtual void IncreaseFatigue(int nGap);
	int GetFatigue() { return m_UserData.Status.wFatigue; }
	int GetWeeklyFatigue() { return m_UserData.Status.wWeeklyFatigue; }
	int GetPCBangFatigue();
	int GetVIPFatigue();
	int GetEventFatigue() { return m_UserData.Status.wEventFatigue; }
	int GetMaxEventFatigue();
	void ResetEventFatigue(int nResetTime);
	int GetAllFatigue();

	void AddCashRebirthCoin(int nRebirthCount);
	virtual void SetDefaultMaxRebirthCoin(bool bSend = true);
	void SetDefaultPCBangRebirthCoin();
	void SetRebirthCoin(int nRebirthCoin, int nPCBangRebirthCoin, int nRebirthCashCoin);
	void AddRebirthCoin(int nRebirthCoin, int nRebirthCashCoin);		//생명석 부활석 증가
	bool CheckRebirthCoin(int nRebirthCoin, int nRebirthCashCoin);		//생명석 부활서 사용가능한지 체크
	bool DecreaseRebirthCoin(int nGap);

	int GetPCBangRebirthCoin();
	BYTE GetRebirthCoin() { return m_UserData.Status.cRebirthCoin; }
	USHORT GetRebirthCashCoin() { return m_UserData.Status.wCashRebirthCoin; }
	BYTE GetVIPRebirthCoin();
	int GetTotalRebirthCoin();

	short GetMarketRegisterCount() { return m_UserData.Status.wMarketRegisterCount; }

	int GetMarketSellSuccess() { return m_UserData.Status.nMarketSellSuccess; }
	int GetMarketSellFailure() { return m_UserData.Status.nMarketSellFailure; }

	int GetMarketRegisterTax() { return m_UserData.Status.nMarketRegisterTax; }
	int GetMarketSellTax() { return m_UserData.Status.nMarketSellTax; }

	// Color
	void SetHairColor(DWORD dwColor) { m_UserData.Status.dwHairColor = dwColor; }
	DWORD GetHairColor() { return m_UserData.Status.dwHairColor; }
	void SetEyeColor(DWORD dwColor) { m_UserData.Status.dwEyeColor = dwColor; }
	DWORD GetEyeColor() { return m_UserData.Status.dwEyeColor; }
	void SetSkinColor(DWORD dwColor) { m_UserData.Status.dwSkinColor = dwColor; }
	DWORD GetSkinColor() { return m_UserData.Status.dwSkinColor; }
	bool ChangeDyeColor( int nItemType, int *pTypeParam );	// 염색약 사용
	void BroadcastChangeColor(int nItemType, DWORD dwColor);

	// QuickSlot
	virtual int AddQuickSlot(BYTE cSlotIndex, BYTE cSlotType, INT64 nID);
	virtual int DelQuickSlot(BYTE cSlotIndex);

#if !defined(PRE_DELETE_DUNGEONCLEAR)
	// Dungeon
	int GetDungeonLevelIndex( int nMapTableID, bool *bExist );
	virtual bool CheckDungeonEnterLevel( int nMapTableID );
	virtual void CalcDungeonEnterLevel(int nMapTableID, char &cOpenHard, char &cOpenVeryHard);

	// Dungeon Clear
	void SetDungeonClear(int nSlotIndex, int nMapIndex, int nType);
	void SetDungeonClearType(int nSlotIndex, int nType);
	int GetDungeonClearMapIndex(int nSlotIndex);
	char GetDungeonClearType(int nSlotIndex);
#endif	// #if !defined(PRE_DELETE_DUNGEONCLEAR)
	// Nest Clear
	void AddNestClear(int nMapIndex);
	char GetNestClearTotalCount(int nMapIndex);
	TNestClearData *GetNestClearCount(int nMapIndex);
	void InitNestClear(bool bSend);
	// 래더시스템
	void InitLadderGradePoint( bool bSend );

	int GetGlyphDelayTime() { return m_UserData.Status.nGlyphDelayTime; }
	int GetGlyphRemainTime() { return m_UserData.Status.nGlyphRemainTime; }

	void SetViewCashEquipBitmap(char cEquipIndex, bool bFlag);
	char* GetViewCashEquipBitmap();
	bool IsExistViewCashEquipBitmap(char cEquipIndex);

	// Mission
	int GetSelectAppellation() { return m_UserData.Appellation.nSelectAppellation; }
	int GetCoverAppellation() { return m_UserData.Appellation.nCoverAppellation; }
#if defined(PRE_ADD_PVPLEVEL_MISSION)
	void SetPvPLevel(BYTE cLevel, bool bCallEvent = true);
#endif

	// Cash
	int DelCashRebirthCoin(int nGap);

	void AddPetal(int nReserve);
	void DelPetal(int nReserve);
	void SetPetal(int nReserve);
	int GetPetal() { return m_UserData.Status.nPetal; }

	// Tick
	DWORD GetTick(eTick pTick) const;
	VOID SetTick(eTick pTick) const;
	VOID SetTick(eTick pTick, DWORD dwVal) const;
	VOID ResetTick(eTick pTick) const;
	BOOL IsTick(eTick pTick) const;

	bool GetAdult() const { return m_bAdult; }
	bool IsCertified() { return m_bCertified; }
	INT64 GetCertifyingKey() { return m_biCertifyingKey; }
	DWORD GetCertifyingTick() { return m_dwCertifyingTick; }
	char GetAge() const { return m_cAge; }
	BYTE GetDailyCreateCount() const { return m_cDailyCreateCount; }
	int GetPrmInt1() const { return m_nPrmInt1; }
	eServerType GetLastServerType() const { return(static_cast<eServerType>(m_cLastServerType)); }

	std::vector<TalkParam>& GetTalkParamList() { return m_TalkParamList; }
	TALK_PARAGRAPH& GetLastTalkParagraph() { return m_LastTalkParagraph; }
	bool& GetSkipParagraphCheck() { return m_bSkipParagraphCheck; }
	bool GetCalledNpcResponse() { return m_bCalledNpcResponse; }
	bool IsNpcTalk() { return m_bIsNpcTalk; }
	void SetCalledNpcResponse(bool bCalledNpcResponse, bool bIsNpcTalk) { m_bCalledNpcResponse = bCalledNpcResponse; m_bIsNpcTalk = bIsNpcTalk; }

	//GameOptions
	const TGameOptions * GetGameOption() { return &m_GameOption; }
	bool IsAcceptAbleOption(INT64 nCharacterDBID, UINT nAccountDBID, int nCheckType);

	void SetLastStageClearRank(char cLastStageClearRank) { m_cLastStageClearRank = cLastStageClearRank; }
	char GetLastStageClearRank() const { return m_cLastStageClearRank; }

	// Exchange
	void SetExchangeTargetSessionID(UINT nSessionID);
	UINT GetExchangeTargetSessionID() { return m_nExchangeTargetSessionID; }
	void ClearExchangeInfo();	// exchange관련된 변수들 클리어

	//gesture
	bool HasGesture(int nGestureID);
	CDNGesture * GetGesture() { return m_pGesture; }

	bool CheckPapering(eCheckPapering eType);

	void GetPartyMemberInfo(SPartyMemberInfo &Info);
	void SetFriendBonus(bool bFlag) { m_bFriendBonus = bFlag; }
	bool GetFriendBonus() { return m_bFriendBonus; }

#if defined(_CH)
	void SetFCMOnlineMin(int nOnlineMin, bool bSend);
	char GetFCMState() { return m_cFCMState; }
	int GetFCMOnlineMin() { return m_nFCMOnlineMin; }
	bool CheckSndaAuthFlag(eChSndaAuthFlag pSndaAuthFlag) const { return((static_cast<DWORD>(m_nPrmInt1) & pSndaAuthFlag)?(true):(false)); }
#endif	// _CH

#if defined(_HSHIELD)
	void SendMakeRequest();
#endif	// _HSHIELD

	virtual void SetHide(bool bHide) {}
	bool GetHide() { return m_bHide; }

	// Schedule
	virtual void OnScheduleEvent( ScheduleEventEnum ScheduleEvent, ScheduleStruct *pStruct, bool bInitialize );

	const TGuildUID& GetGuildUID() const { return(m_GuildSelfView.GuildUID); }

	TGuildSelfView& GetGuildSelfView() { return m_GuildSelfView; }
	const TGuildSelfView& GetGuildSelfView() const { return m_GuildSelfView; }
	virtual void SetGuildSelfView(const TGuildView& pGuildView);
	virtual void SetGuildSelfView(const TGuildSelfView& pGuildSelfView);
	virtual void ResetGuildSelfView();
	virtual void RefreshGuildSelfView();

	void AddGuildPoint(char cPointType, int nPointValue, int nMissionID = 0);
	
#ifdef _USE_VOICECHAT
	bool GetVoiceAvailable() { return m_bVoiceAvailable; }
	bool IsMutedUser(UINT nMuteID);
	void SetVoiceMute(UINT nMuteID, bool bMute);

	bool JoinVoiceChannel(UINT nVoiceChannelID, const char * pIP, TPosition * pPos, int nRotate, bool bForceSet = false, bool bMutePass = false);
	void LeaveVoiceChannel();
	void MakeMute();
	bool VoiceMuteOnetoOne(UINT nMuteAccountDBID, bool bMute);
	bool VoiceComplaintRequest(UINT nComplaineeAccountDBID, const char * pCategory, const char * pSubject, const char * pMsg);
	bool SetVoicePos(int nX, int nY, int nZ, int nRotate);
	bool IsTalking(BYTE * pTalking);
#endif

	void PCBangResult(MAPCBangResult *pResult);
	void SetPCBang(bool bPCBang);
	void SetPCBangGrade(char cGrade);
	bool IsPCBang() { return m_bPCBang; }
	int GetPcBangBonusExp() { return m_nPcBangBonusExp; }
	char GetPCBangGrade() { return m_cPCBangGrade; }
	void RefreshPCBangMissionList();

	bool IsVIP() { return m_bVIP; }
	__time64_t GetVIPEndDate() { return m_tVIPEndDate; }		// VIP 기간 끝 일자
	bool IsVIPAutoPay() { return m_bVIPAutoPay; }		// 자동결제
	int GetVIPBonusExp() { return m_nVIPBonusExp; }
	int GetVIPTotalPoint() { return m_nVIPTotalPoint; }			// 총 VIP 포인트
	void SetVIPTotalPoint(int nPoint);

	bool bIsSetSecondAuthPW(){ return m_bIsSetSecondAuthPW; }
	bool bIsSetSecondAuthLock(){ return m_bIsSetSecondAuthLock; }

	// Cash잔액
	int GetCashBalance() { return m_nCashBalance; }
	void SetCashBalance(int nBalance);
	void DelCashBalance(int nCash);
	void DelCashByPaymentRules(int nBalance, char cPaymentRules);
#if defined(PRE_ADD_CASH_REFUND)
	void AddCashBalance(int nCash);
#endif
	bool CheckEnoughCashByPaymentRules(int nBalance, char cPaymentRules);
	bool CheckEnoughCashBalance(int nBalance);	// nCheckCash만큼 돈이 있는지...
	bool CheckEnoughPetal(int nPetal);
#if defined(_US)
	int GetNxAPrepaid() { return m_nNxAPrepaid; }
	void SetNxAPrepaid(int nCash);
	void DelNxAPrepaid(int nCash);
	bool CheckEnoughNxAPrepaid(int nCash);

	int GetNxACredit() { return m_nNxACredit; }
	void SetNxACredit(int nCash);
	void DelNxACredit(int nCash);
	bool CheckEnoughNxACredit(int nCash);

	bool CheckNxACreditUsableLevel(int nItemSN);
#endif	// #if defined(_US)

	bool IsValidPacketByNpcClick();	// npc를 상대로 보내는 패킷이 유효한지 검사
	void MakeGiftByQuest(int nQuestID, int *nRewardItemList);

#ifdef _GPK
	void SetCheckGPKTick(DWORD nTick) { m_dwCheckGPKTick = nTick; }		// live check
	void SetRecvGPKTick(DWORD nTick) { m_dwRecvGPKTick = nTick; }		// live check
	DWORD GetGPKRecvTick() { return m_dwRecvGPKTick; }
	void SetGPKRespondErr(bool bFlag) { m_bRespondErr = bFlag; }
	bool GetGPKRespondErr() { return m_bRespondErr; } 
#endif

	TProfile * GetProfile()	{	return &m_Profile;	}

	void OnMasterSystemEvent( MasterSystem::EventType::eCode EventCode );
	bool bIsMasterApplicationPenalty();

	void ModifyCompleteQuest();
	void ResetRepeatQuest();

	void NotifyGuildMemberLevelUp(char cLevel);
	int	GetGuildWareAllowCount();
	int GetGuildRoleType();
	void ExtendGuildWareAllowCount(int nTotalSize);

	void UpdatePeriodQuest(int nWorldID, int nPeriodQuestType, std::vector<int>& vTotalQuestIDs);
	void CheckResetPeriodQuest(const TAGetPeriodQuestDate* pPacket);
	void MarkingPeriodQuest();
	void SetDefaultMaxDailyPeriodQuest(ScheduleStruct *pStruct);
	void SetDefaultMaxWeeklyPeriodQuest(ScheduleStruct *pStruct);

	void ResetPeriodQuest(int nType);
	void ProcessPeriodQuest (DWORD dwCurTick);
	bool m_bPeriodQuestSchedule[PERIODQUEST_RESET_MAX];
	DWORD m_dwPeriodQuestTick[PERIODQUEST_RESET_MAX];

	int GetDungeonClearCount() { return m_UserData.Status.nDungeonClearCount; }
	void SetDungeonClearCount(int nCount) {m_UserData.Status.nDungeonClearCount = nCount;}
	void IncreaseDungeonClearCount() {m_UserData.Status.nDungeonClearCount++;}

	void SetEtcPointList( const TAGetListEtcPoint *pPacket );
	void SetEtcPointEach(int nType, INT64 biLeftPoint);
	void AddEtcPoint( BYTE cType, int nPlusPoint );
	void UseEtcPoint( BYTE cType, int nUsePoint );
	int GetPvPUserState() { return 0; }
	void SetRemoteEnchantItemID( int iItemID ){ m_iRemoteEnchantItemID=iItemID; }
	int GetRemoteEnchantItemID(){ return m_iRemoteEnchantItemID; }
	bool bIsRemoteEnchant(){ return m_iRemoteEnchantItemID>0; }

	int ParseEnchant( CSEnchantItem* pPacket, int nLen );
	int ParseEnchantComplete( CSEnchantItem* pPacket, int nLen );
	int ParseEnchantCancel( int nLen );
	void SetRemoteItemCompoundItemID( int iItemID ){ m_iRemoteItemCompoundItemID=iItemID; }
	int GetRemoteItemCompoundItemID(){ return m_iRemoteItemCompoundItemID; }
	bool bIsRemoteItemCompound(){ return m_iRemoteItemCompoundItemID>0; }

	int ParseItemCompound( CSItemCompoundOpenReq* pPacket, int nLen );
	int ParseItemCompoundComplete( CSCompoundItemReq* pPacket, int nLen );
	int ParseItemCompoundCancel( CSItemCompoundCancelReq* pPacket, int nLen );

#if defined( PRE_ADD_VIP_FARM )
	bool bIsFarmVip();
#endif // #if defined( PRE_ADD_VIP_FARM )

	int GetHackAbuseDBValue() const{ return m_iHackAbuseDBValue; }
	int GetHackAbuseDBCallCount() const{ return m_iHackAbuseDBCallCount; }
	int GetHackPlayRestraintValue() const{ return m_iHackPlayRestraintValue; }
	int GetHackResetRestraintValue() const{ return m_iHackResetRestraintValue; }
	int GetHackCharacterCntWithoutMe() const { return m_nHackAbuseCharacterCntWithoutMe; }
#if defined(PRE_ADD_ANTI_CHAT_SPAM)
	void SetSpammer( bool bSpammer ) { m_bSpammer = bSpammer;};
	bool GetSpammer() { return m_bSpammer;};
#endif
#if defined(PRE_ADD_MUTE_USERCHATTING)
	bool IsMuteChatting();
#endif	// #if defined(PRE_ADD_MUTE_USERCHATTING)
	bool ModCommonVariableData( CommonVariable::Type::eCode Type, INT64 biValue, __time64_t tDate = 0 );
	bool GetCommonVariableDataValue( CommonVariable::Type::eCode Type, INT64& biValue );
	bool IncCommonVariableData( CommonVariable::Type::eCode Type, __time64_t tDate = 0 );

	void UpdateAttendanceEventData(bool bSendResult);
	bool CheckAttendanceEventDone( int iOngoingDay );
	int GetDateGap(__time64_t tLastModifyDate);

	virtual bool bIsCheckPing() = 0;
	virtual void WritePingLog() = 0;
	void AddGhoulScore(GhoulMode::PointType::eCode Type);
	void ClearAddGhoulScore();
	inline int GetAddGhoulScore(GhoulMode::PointType::eCode Type) { return m_PvPAddGhoulScores.nPoint[Type];};
	inline int GetTotalGhoulScore(GhoulMode::PointType::eCode Type) { return m_PvPTotalGhoulScores.nPoint[Type];};

	int GetMaxLevelCharacterCount();
	int GetLevelCharacterCount(int nLevel);

	void NextTalk(UINT nNpcObjectID, WCHAR* wszTalkIndex, WCHAR* wszTarget, std::vector<TalkParam>& talkParam  );

	void SetShopType( Shop::Type::eCode ShopType ){ m_ShopType = ShopType; }
	Shop::Type::eCode GetShopType(){ return m_ShopType; }

	void SetSkillPage(char cPage){ m_UserData.Status.cSkillPage = cPage; }
	char GetSkillPage(){ return m_UserData.Status.cSkillPage; }
	// 추가
	void AddWindowState( eWindowState AddState );
	// 셋팅
	void SetWindowState( eWindowState SetState );
	// 상태가 포함 되어 있는지.
	bool IsWindowState( eWindowState InclusionState) const;
	// None인 경우에만 State추가
	bool IsNoneWindowStateAdd( eWindowState AddState );
	// None인 경우에만 State 셋팅
	bool IsNoneWindowStateSet( eWindowState SetState );
	// 상태가 포함 되어 있을때만 None 셋팅
	void IsWindowStateNoneSet( eWindowState InclusionState);
	void SetNoneWindowState();
	bool IsNoneWindowState() const;

	virtual TPARTYID GetPartyID() = 0;
	bool DeleteWarpVillageItemByUse(INT64 nItemSerial);

	bool AddDataBaseMessageSequence(char cMainCmd, char cSubCmd);
	void DelDataBaseMessageSequence(char cMainCmd, char cSubCmd);
	bool CheckDataBaseMessageSequence(char cMainCmd, char cSubCmd);

	void SendLevelupEventMail(int nLevel, int nClass, int nJob);

#if defined( PRE_ADD_LIMITED_SHOP )		
	void ResetLimitedShopDailyCount();
	void ResetLimitedShopWeeklyCount();
#if defined( PRE_FIX_74404 )
	int GetLimitedShopBuyedItem( int nShopID, int nItemID);
	void AddLimitedShopBuyedItem(int nShopID,int nItemID, int nBuyCount, int nResetCycle, bool bDBSend = true);	
#else // #if defined( PRE_FIX_74404 )
	int GetLimitedShopBuyedItem(int nItemID);
	void AddLimitedShopBuyedItem(int nItemID, int nBuyCount, int nResetCycle, bool bDBSend = true);	
#endif // #if defined( PRE_FIX_74404 )	
	void DelLimitedShopBuyedItem(int eResetCycle);
#endif

#if defined( PRE_PRIVATECHAT_CHANNEL )
	int CheckInvalidChannelName(WCHAR* wszName);
	INT64 GetPrivateChannelID() {return m_UserData.Status.nPrivateChatChannel;}
	void SetPrivateChannelID( INT64 nChannelID ) {m_UserData.Status.nPrivateChatChannel = nChannelID;}
	void SendPrivateChatChannel( CDNPrivateChaChannel* pPrivateChatChannel );
#endif

#if defined( PRE_ADD_TOTAL_LEVEL_SKILL )
	bool AddTotalLevelSkillData(BYTE cSlotIndex, int nSkillID, bool isInitialize = false);
	int GetTotalLevelSkillLevel() {return m_nTotalLevelSKillLevel;}
	void AddTotalLevelSkillLevel( int nAdd ) { m_nTotalLevelSKillLevel += nAdd;	}
	int* GetTotalLevelSkillData() { return m_nTotalLevelSKillData; }
	void SetTotalLevelSkillCashSlot(int nIndex, INT64 ExpireDate) {m_nTotalLevelSkillCashSlot[nIndex] = ExpireDate; m_bTotalLevelSkillCashSlot[nIndex] = true;}
	INT64 GetTotalLevelSkillCashSlot(int nIndex) {return m_nTotalLevelSkillCashSlot[nIndex];}
	bool bIsTotalLevelSkillCashSlot(int nIndex) {return m_bTotalLevelSkillCashSlot[nIndex];}
	void ResetTotalLevelSkillMedalCount();
	int GetTotalLevelSkillMedalCount();
	void AddTotalLevelSkillMedalCount(int nMedalCount);
	float GetTotalLevelSkillEffect( TotalLevelSkill::Common::eVillageEffectType eType );
	int GetintTotalLevelSkillEffect( TotalLevelSkill::Common::eVillageEffectType eType );
#endif

#if defined( PRE_ADD_NEWCOMEBACK )	
	int GetComebackAppellation() { return m_nComebackAppellation; }
#endif

#ifdef PRE_ADD_BEGINNERGUILD
	bool CheckBegginerGuild() { return m_bPartyBegginerGuild; }
#endif		//#ifdef PRE_ADD_BEGINNERGUILD

#if defined( PRE_ADD_STAMPSYSTEM )
	CDNStampSystem * GetStampSystem() { return m_pStampSystem; }
#endif // #if defined( PRE_ADD_STAMPSYSTEM )
#if defined(PRE_ADD_GAMEQUIT_REWARD)
	GameQuitReward::RewardType::eType GetUserGameQuitRewardType(){ return m_eUserGameQuitRewardType; }
	void SetUserGameQuitRewardType(GameQuitReward::RewardType::eType eType){ m_eUserGameQuitRewardType = eType; }
	bool SetGameQuitRewardItem(TLevelupEvent::eEventType eType, DBDNWorldDef::AddMaterializedItem::eCode eLogType);
#endif
#if defined(PRE_ADD_CP_RANK)
	char GetStageClearBestRank() { return m_cStageClearBestRank; }
	int GetStageClearBestCP() { return m_nStageClearBestCP; }
#endif //#if defined(PRE_ADD_CP_RANK)

#if defined( PRE_ADD_NEW_MONEY_SEED )
	void SetSeedPoint( INT64 nSeedPoint );
	void AddSeedPoint( INT64 nSeedPoint );
	void DelSeedPoint( INT64 nSeedPoint );
	INT64 GetSeedPoint() { return m_nSeedPoint; }
	bool CheckEnoughSeed(int nPetal);
#endif

	//=========================================================================
	// Private Function
	//=========================================================================

private:
	int _MakeItemCompoundErrorCode( CDnItemCompounder::S_OUTPUT &Output );

	int _CmdSetQuestNotifier( const CSGameOptionQuestNotifier* pPacket );
	int	_CmdSetMissionNotifier( const CSGameOptionMissionNotifier* pPacket );

	int DelFatigue(int nGap);
	int DelWeeklyFatigue(int nGap);
	int DelPCBangFatigue(int nGap);
	int DelEventFatigue(int nGap);
	int DelVIPFatigue(int nGap);

	int DelRebirthCoin(int nGap);
	int DelPCBangRebirthCoin(int nGap);
	int DelVIPRebirthCoin(int nGap);

	void _MakeGiftVIPMonthItemByItemSN(int nItemSN);

	int _OnCashBalanceInquiry();
	int _OnCashShopOpen();
	int _OnCashBuy(CSCashShopBuy *pCashShop);
	int _OnCashPackageBuy(CSCashShopPackageBuy *pCashShop);
#if defined(PRE_ADD_CADGE_CASH)
	int _OnCashCadge(CSCashShopCadge *pCashShop);
#endif	// #if defined(PRE_ADD_CADGE_CASH)
	int _OnCashGift(CSCashShopGift *pCashShop);
	int _OnCashPackageGift(CSCashShopPackageGift *pCashShop);
	int _OnCashCheckReceiver(CSCashShopCheckReceiver *pCashShop);
	int _OnCashCoupon(CSCashShopCoupon *pCashShop);
#if defined(PRE_ADD_VIP)
	int _OnCashVIPBuy(CSVIPBuy *pCashShop);
	int _OnCashVIPGift(CSVIPGift *pCashShop);
#endif	// #if defined(PRE_ADD_VIP)
#if defined(_VILLAGESERVER) && defined(PRE_ADD_CASH_REFUND)
	int _OnMoveCashInven(CSMoveCashInven* pCashShop);
	int _OnCashRefund(CSCashRefund* pCashShop);
#endif	// #if defined(PRE_ADD_CASH_REFUND)

	int OnRecvWarpVillageList(const CSWarpVillageList* pPacket);
	int OnRecvWarpVillage(const CSWarpVillage* pPacket);

#if defined( PRE_ADD_NOTIFY_ITEM_COMPOUND )
	void SetCompoundNotify(const CSCompoundNotify* pPacket);
#endif
#if defined(_KRAZ)
	void _CheckShutdown();
#endif	// #if defined(_KRAZ)
	int _CheckSaleCoupon(int nSaleCouponSN, INT64 biSaleCouponSerial, int nBuyItemSN);
};
