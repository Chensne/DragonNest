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

	Village::UserSession�� Game::UserSession�� ����κ��� �̰����� ó��
	������ �ΰ��� ����°� �����Ƽ� �������.

	Mission, Appellation, Item, Quest�� ���� Ŭ�������� �˾Ƽ� �������� ������
	�� ������ �ֵ��� �� ��������~

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
		eTickNpcTalk = 0,		// CS_NPCTALK ��û�� ������Ŵ
		eTickCnt,
	};

	enum eCheckPapering
	{
		eChatPapering = 0,
		eGesturePapering,

		eCheckPaperingAmount = 2,
	};

	// ��Ŷ������ �ѹ��� Copy �ϱ� ���ؼ� ��� �ȹ���.
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
	UINT m_nAccountDBID;				// ������� ���̵�
	INT64 m_biCharacterDBID;				// ĳ����� ���̵�
	UINT m_nSessionID;

	WCHAR m_wszAccountName[IDLENMAX];	// �����̸�
	char m_szAccountName[IDLENMAX];	// ���� char��(���� �Ƿε� ���������� ��ȯ�ϱ� �����Ƽ� �������;;)
	char m_szCharacterName[NAMELENMAX];

	bool m_bPCBang;				// ���� pcroom���ٴ� pcbang�� ����! ������
	char m_cPCBangGrade;
	int m_nPcBangBonusExp;		// ���ʹ��̽� �˻��ϴµ� ���� �κ��̶� ���ú����� �����صΰ� ã�� �κ��� ���δ�.
	int m_iHackAbuseDBValue;
	int m_iHackAbuseDBCallCount;
	int m_iHackPlayRestraintValue;
	int m_iHackResetRestraintValue;
	int m_nHackAbuseCharacterCntWithoutMe;		//���� ����ĳ���͸� ������ ����� �ɸ� ĳ���� ī��Ʈ
#if defined(PRE_ADD_ANTI_CHAT_SPAM)
	bool m_bSpammer;
#endif

	int m_nWorldSetID;				//������̵�
	BYTE m_cVillageID;				//���� �����̵�
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
	TPvPGhoulScores m_PvPTotalGhoulScores; // ��ü ��������	
	TPvPGhoulScores	m_PvPAddGhoulScores;	// ���Ǵ� ��������
	TUnionReputePointInfo m_UnionReputePointInfo;
	int m_nGuildWarRewardFestivalPoint;		// �������� ���� ���� ����Ʈ
	int m_nGuildWarRewardGuildPoint;		// �������� ���� ��� ����Ʈ
	__int64 m_biGuildWarFestivalPoint;		// ����� ���� ����Ʈ	
	short m_wGuildWarScheduleID;			// ����� ������ID	

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
	CDNGesture * m_pGesture;		//�����̶�簡 �߰��Ǹ� �츱����
	CDNTimeEventSystem *m_pTimeEventSystem;
	CDNUserEventHandler* m_pUserEventHandler;
	CDNCommonVariable *m_pCommonVariable;

	std::vector<TalkParam> m_TalkParamList;
	TALK_PARAGRAPH m_LastTalkParagraph;		// api_npc_NextScript �� ���� �Ҷ��� �ѹ��� m_LastTalkParagraph üũ�� ��ŵ�Ѵ�
	bool m_bSkipParagraphCheck;
	// next_talk �� next_script �� npc �� ������ ���´��� ����.
	// ��ũ��Ʈ ���� �߻��� ������ ������ ��� npc �� �������� ���� ������ ���� �����̶� ������ ���� üũ�ϴ� ��
	bool m_bCalledNpcResponse; 
	bool m_bIsNpcTalk; 

	UINT m_nExchangeTargetSessionID;	// �ŷ��ϴ� ���� ���Ǿ��̵�
	UINT m_nExchangeSenderSID;	// �ŷ���û ���� ���̵�
	UINT m_nExchangeReceiverSID;	// �ŷ����� ���� ���̵�

	DWORD m_dwLastMessageTick;
	BYTE m_cLastMainCmd;
	BYTE m_cLastSubCmd;

	int m_nCashBalance;	// ĳ��(total)
#if defined(_US)
	int m_nNxAPrepaid;	// not refund balance
	int m_nNxACredit;	// m_biCashBalance - m_biNxAPrepaid
#endif	// #if defined(_US)

	bool m_bHide;	// ��� �������(?)

	//�ϴ� ����������Ʈ���� ������ ����
	bool m_bNeedUpdateOption;
	TGameOptions m_GameOption;

	bool m_bIsSetSecondAuthPW;		// 2�� ���� ��й�ȣ ���� ����
	bool m_bIsSetSecondAuthLock;	// 2�� ���� ���� Lock ����

#if defined(_CH)
	char m_cFCMState;			// eFCMState
	int m_nFCMOnlineMin;		// online �ð�(��)
#endif	// _CH
	bool	m_bSecurityUpdate;
#if defined(_HSHIELD)
	AHNHS_CLIENT_HANDLE	m_hHSClient;	// [Ȯ�� ��������] Ŭ���̾�Ʈ ���� ��ü
	DWORD m_dwCheckLiveTick;			// live check
	DWORD m_dwHShieldResponseTick;		// CRC������ ������������� �ð� üũ
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
	int m_nExpiredPetID;	// �������̴� �Ⱓ���� �� ���� -> OnPetExpired �̼� ȣ�⿡ ���
#endif
	short m_sUserWindowState;	// UI����(?) ������ ���� ���ϴ� �����ΰ� (bit����)

	int m_nShopID;	// shop id �����ϱ�
#if defined(PRE_ADD_REMOTE_OPENSHOP)
	bool m_bRemoteShopOpen;
#endif	// #if defined(PRE_ADD_REMOTE_OPENSHOP)

	int m_nCutSceneID;	// �÷����ϰ��ִ� �ƾ� ���̵�(�ӽ�)

	int m_nClickedNpcID;	// Ŭ���� npcid
	UINT m_nClickedNpcObjectID;	// Ŭ���� Npc OjbectID

	bool m_bAdult;	// ����� �ƴ���(�Ƿε�)
	char m_cAge;	// ����(Nexon����)
	BYTE m_cDailyCreateCount;	// ���� ĳ���� �������� Ƚ��
	int m_nPrmInt1;		// �Ķ���� 1(������ �ٸ���)
	char m_cLastServerType;		// �ֱ� ���� Ÿ��(eServerType / 0 : �α���, 2 : ������, 3 : ����)

	bool m_bCharOutLog;	// true�� FinalUser �ϸ鼭 �α׸� ����� 090326

	bool m_bCertified;			// �����Ϸ� ����(QUERY_CHECKAUTH �� ������ ���� false �� �����ؾ� ��)
	DWORD m_dwCertifyingTick;	// �����ð� üũ(����üũ ��û �� CHECKAUTHLIMITTERM ���� �ȿ� ó���Ǿ�� ��)(��Ƽ��� üũ(PVP ���� ???))
	INT64 m_biCertifyingKey;	// ����Ű

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

	mutable DWORD m_dwTick[eTickCnt+1];		// ƽ ī��Ʈ(�� ��ü�� Ư�� ������ �ð������� ������ ��찡 �߻� �� ���⿡ ���������� ����Ͽ� ����� ����)

	char m_cLastStageClearRank;	// ���� �ֱ� �������� �Ϸ� ��ũ(SSS : 0 / SS : 1 / S : 2 / A : 3 / B : 4 / C : 5 / D : 6)
	TGuildSelfView m_GuildSelfView;		// ��� �ð�����(����)

	bool m_bVoiceAvailable;		//���̽�ê�� ��������
#ifdef _USE_VOICECHAT
	UINT m_nVoiceChannelID;		//���̽�ä�ο� �������� ���
	BYTE m_cIsTalking;
	int m_nVoiceJoinType;
	UINT m_nVoiceMutedList[PARTYCOUNTMAX];
#endif

	DWORD m_dwNpcTalkLastIndexHashCode;		// NPC ����� ���� ������ �ε��� ���
	DWORD m_dwNpcTalkLastTargetHashCode;	// NPC ����� ���� ������ ������� ���

	BYTE m_cVIPRebirthCoin;
	bool m_bVIP;
	int m_nVIPTotalPoint;			// �� VIP ����Ʈ
	__time64_t m_tVIPEndDate;		// VIP �Ⱓ �� ����
	bool m_bVIPAutoPay;				// �ڵ�����
	int m_nVIPBonusExp;		// ���ʹ��̽� �˻��ϴµ� ���� �κ��̶� ���ú����� �����صΰ� ã�� �κ��� ���δ�.

	int m_nExpandNestClearCount;
#if defined(PRE_ADD_TSCLEARCOUNTEX)
	int m_nExpandTreasureStageClearCount;
#endif	// #if defined(PRE_ADD_TSCLEARCOUNTEX)
	bool m_bIntroducer;		// ���� ���θ�� ��õ��

	// �������
	DWORD m_dwCheckPaperingRemainTime[eCheckPaperingAmount];
	std::list<DWORD> m_listRecentCheckTime[eCheckPaperingAmount];

	CDNEffectRepository* m_pEffectRepository;

	// �����ȱ� ������
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
	int m_nTotalLevelSKillLevel;			// ���շ���
	int m_nTotalLevelSKillData[TotalLevelSkill::Common::MAXSLOTCOUNT];			// ���ս�ų
	bool m_bTotalLevelSkillCashSlot[TotalLevelSkill::Common::MAXSLOTCOUNT];		// ĳ�ý���
	INT64 m_nTotalLevelSkillCashSlot[TotalLevelSkill::Common::MAXSLOTCOUNT];	// ĳ�ý���	
#endif

#if defined( PRE_ADD_NEWCOMEBACK )
	int m_nComebackAppellation;
#endif
#ifdef PRE_ADD_JOINGUILD_SUPPORT
	BYTE m_cGuildJoinLevel;					//��尡�� ����(�����ѹ�)
	bool m_bWasRewardedGuildMaxLevel;		//��忡�� �������� ��� ������ �޾Ҵ��� �÷���
	bool m_bPartyBegginerGuild;				//��Ƽ���¿��� �ʺ���������
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
	virtual void ChangeExp(int nChangeExp, int nLogCode, INT64 biFKey, bool bAbsolute=false );		// changeexp��ŭ ���ϰų� ���ֱ�(+, -�� �Ἥ) -> nLogCode�� ���� biFKey���� �־��ش�(eCharacterExpChangeCode����)
#else
	virtual void ChangeExp(int nChangeExp, int nLogCode, INT64 biFKey);		// changeexp��ŭ ���ϰų� ���ֱ�(+, -�� �Ἥ) -> nLogCode�� ���� biFKey���� �־��ش�(eCharacterExpChangeCode����)
#endif
	virtual void SetExp(UINT nExp, int nLogCode, INT64 biFKey, bool bDBSave);				// exp��ŭ �� ����
	unsigned int GetExp() { return m_UserData.Status.nExp; }
	virtual void SetLevel(BYTE cLevel, int nLogCode, bool bDBSave);
	unsigned char GetLevel();

	virtual void SetUserJob(BYTE cJob);
	BYTE GetUserJob() { return m_UserData.Status.cJob; }
	// Coin
	bool AddCoin(INT64 nCoin, int nLogCode, INT64 biFKey, bool bSend = true);
	bool DelCoin(INT64 nCoin, int nLogCode, INT64 biFKey, bool bSend = true);
	INT64 GetCoin();
	bool CheckEnoughCoin(INT64 nCheckCoin);	// nCheckCoin��ŭ ���� �ִ���...
	bool CheckMaxCoin(INT64 nCheckCoin);	// 42���� �ѱ����ʴ���

	void SetWarehouseCoin(INT64 nCoin);		// nCoin���� ����
	void AddWarehouseCoin(INT64 nCoin, int nLogCode, INT64 biFKey);
	void DelWarehouseCoin(INT64 nCoin, int nLogCode, INT64 biFKey);
	INT64 GetWarehouseCoin();
	bool CheckEnoughWarehouseCoin(INT64 nCheckCoin);	// nCheckCoin��ŭ ���� �ִ���...
	bool CheckMaxWarehouseCoin(INT64 nCheckCoin);	// 42���� �ѱ��� �ʴ���

	// SkillPoint
	void SetSkillPoint(USHORT nPoint, char cSkillPage);
	void ChangeSkillPoint(unsigned short nPoint, int nSkillID, bool bDBSave, int nLogCode, char cSkillPage = DualSkill::Type::MAX);
	void SetCheatSkillPoint( unsigned short nPoint, int nSkillID );		// setsp ġƮŰ ����
	USHORT GetSkillPoint();		//���� ���õ� ��ųƮ�� ����Ʈ 
	USHORT GetSkillPoint(int nSkillPageIndex);	//Ư�� ��ųƮ�� ����Ʈ
	float GetAvailSkillPointRatioByJob( int nSkillID );

	// Fatigue
	void SetCheatMaxFatigue();
	void SetCheatFatigue(int nFatigue, int nWeeklyFatigue, int nPCBangFatigue, int nEventFatigue, int nVIPFatigue);
	virtual void SetDefaultMaxFatigue(bool bSend = true);
	virtual void SetDefaultMaxWeeklyFatigue(bool bSend = true);
#if defined(PRE_ADD_VIP)
	void SetDefaultVIPFatigue();	// VIP����� �Ƿε� ����
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
	void AddRebirthCoin(int nRebirthCoin, int nRebirthCashCoin);		//���� ��Ȱ�� ����
	bool CheckRebirthCoin(int nRebirthCoin, int nRebirthCashCoin);		//���� ��Ȱ�� ��밡������ üũ
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
	bool ChangeDyeColor( int nItemType, int *pTypeParam );	// ������ ���
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
	// �����ý���
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
	void ClearExchangeInfo();	// exchange���õ� ������ Ŭ����

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
	__time64_t GetVIPEndDate() { return m_tVIPEndDate; }		// VIP �Ⱓ �� ����
	bool IsVIPAutoPay() { return m_bVIPAutoPay; }		// �ڵ�����
	int GetVIPBonusExp() { return m_nVIPBonusExp; }
	int GetVIPTotalPoint() { return m_nVIPTotalPoint; }			// �� VIP ����Ʈ
	void SetVIPTotalPoint(int nPoint);

	bool bIsSetSecondAuthPW(){ return m_bIsSetSecondAuthPW; }
	bool bIsSetSecondAuthLock(){ return m_bIsSetSecondAuthLock; }

	// Cash�ܾ�
	int GetCashBalance() { return m_nCashBalance; }
	void SetCashBalance(int nBalance);
	void DelCashBalance(int nCash);
	void DelCashByPaymentRules(int nBalance, char cPaymentRules);
#if defined(PRE_ADD_CASH_REFUND)
	void AddCashBalance(int nCash);
#endif
	bool CheckEnoughCashByPaymentRules(int nBalance, char cPaymentRules);
	bool CheckEnoughCashBalance(int nBalance);	// nCheckCash��ŭ ���� �ִ���...
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

	bool IsValidPacketByNpcClick();	// npc�� ���� ������ ��Ŷ�� ��ȿ���� �˻�
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
	// �߰�
	void AddWindowState( eWindowState AddState );
	// ����
	void SetWindowState( eWindowState SetState );
	// ���°� ���� �Ǿ� �ִ���.
	bool IsWindowState( eWindowState InclusionState) const;
	// None�� ��쿡�� State�߰�
	bool IsNoneWindowStateAdd( eWindowState AddState );
	// None�� ��쿡�� State ����
	bool IsNoneWindowStateSet( eWindowState SetState );
	// ���°� ���� �Ǿ� �������� None ����
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
