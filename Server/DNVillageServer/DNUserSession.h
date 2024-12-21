#pragma once

#include "Connection.h"
#include "DNBaseObject.h"
#include "DNUserSkill.h"
#include "DnItemCompounder.h"
#include "DNUserBase.h"

/*---------------------------------------------------------------------------------------
									CDNUserSession

	- CDNBaseObject, CConnection 상속받아서 생성
---------------------------------------------------------------------------------------*/

class CDNUserSession: public CConnection, public CDNBaseObject, public CDNUserBase
{
private:
	CDNUserSkill* m_pSkill;

	// PvP
	DWORD	m_dwPvPRoomListRefreshTime;
	BYTE	m_cPvPVillageID;
	USHORT	m_unPvPVillageChannelID;
	UINT	m_uiPvPIndex;
	USHORT	m_usPvPTeam;
	UINT	m_uiPvPUserState;

	bool m_bPartyInto;
	CDNCoolTime m_SkillCoolTime;
	TPARTYID m_PartyID;

#if defined( PRE_PARTY_DB )
	CSPartyListInfo m_PrevPartyListInfo;
	DWORD m_dwPrevPartyListInfoTick;
#endif // #if defined( PRE_PARTY_DB )

#if defined(PRE_ADD_TRANSFORM_POTION)
	int m_nTransformID;						// 변신 물약 ID
	DWORD m_dwTransformTick;				// 변신 시간..
#endif

public:

#if !defined( _FINAL_BUILD ) && !defined (__COVERITY__)
	FRIEND_TEST( NpcReputation_unittest, CONNECTDATE_TEST );
#endif // #if !defined( _FINAL_BUILD )

#if !defined( _FINAL_BUILD )	// 사제시스템 치트용 임시 변수
	bool m_bIsMasterSystemSkipDate;
#endif // #if defined( _FINAL_BUILD )

	// 이동할 게임서버 정보
	GameTaskType::eType m_GameTaskType;
	BYTE	m_cReqGameIDType;
	ULONG	m_nGameServerIP;
	USHORT	m_nGameServerPort;
	USHORT	m_nGameServerTcpPort;
	DWORD	m_dwLastEscapeTime;

	TConnectionInfo m_MoveToVillageInfo;	// 이동할 빌리지 정보

	eUserState m_eUserState;

	bool m_bChangeSameServer;	// 같은 서버로 이동
	bool m_bFirst;	// Village에 제일 처~음 접속인지 걍 game->village, village->village인지 체크(처음이 아니면 굳이 inven등 큰 데이터들 좌라락~ 보낼필요 없음)
	bool m_bChannelMove;
	bool m_boPartyLeader;	// 리더인지 아닌지
	int m_nPartyMemberIndex;	// 멤버 배열 인덱스를 직접 가지고 있자
	char m_cGateNo;
	int m_nGateType;
	char m_cGateSelect;
	int m_nSingleRandomSeed;	// 혼자 플레이할때 날려줘야함

	volatile bool m_bIsChannelCounted;	//채널카운트용

	DWORD m_dwNpcTalkLastIndexHashCode;		// NPC 대사의 가장 마지막 인덱스 기록
	DWORD m_dwNpcTalkLastTargetHashCode;	// NPC 대사의 가장 마지막 대사파일 기록

#ifdef _USE_VOICECHAT
	int m_nVoiceRotate;
#endif

	UINT m_nChatRoomID;

	INT64	m_biCosMixSerialCache;
	INT64	m_biCosDesignMixSerialCache;
#ifdef PRE_ADD_COSRANDMIX
	INT64 m_biCosRandomMixSerialCache;
	#ifdef PRE_ADD_COSRANDMIX_ACCESSORY
	CostumeMix::RandomMix::eOpenType m_CosRandMixOpenType;
	#endif
	#ifdef PRE_ADD_COSRANDMIX_MIXERTYPE
	int m_nCosRandMixerEnablePartsType;
	#endif
#endif

	int m_nLastUseItemSkillID;

	bool m_bIsMove;
	bool m_bIsStartGame;
	int m_nInvalidSendCount;
	bool m_bIsLadderUser;
	int m_nAbuseLogSendCount;

	// 예선
	int m_nGuildWarPoint;			// 예선 포인트
	bool m_bGuildWarVote;			// 투표 여부
	bool m_bGuildWarVoteLoading;	// 투표 여부를 DB에서 꺼내 왔는지 여부
	// 예선 결과용
	bool m_bGuildWarStats;			// 정보 로딩 여부
	SMyGuildWarMissionRankingInfo m_sMyGuildWarMissionRaningInfo[GUILDWAR_RANKINGTYPE_MAX];			// 내 순위
	bool m_bRewardResults[GUILDWAR_REWARD_CHAR_MAX];			// 저장된 개인보상 결과
	std::map<int, int> m_GuildWarBuyedItem;

	bool m_bIsCheckLastDungeonInfo;
#ifdef PRE_ADD_COLOSSEUM_BEGINNER
	PvPCommon::RoomType::eRoomType m_ePvPChannel;
#endif		//#ifdef PRE_ADD_COLOSSEUM_BEGINNER
#if defined(PRE_ADD_PCBANG_RENTAL_ITEM)
	bool m_bDeletePCRentalItem;
#endif

#ifdef PRE_FIX_VILLAGEZOMBIE
	DWORD m_dwCalledSendInsideDisconnectTick;
	volatile bool m_bZombieChecked;
#endif		//#ifdef PRE_FIX_VILLAGEZOMBIE

#if defined( PRE_ALTEIAWORLD_EXPLORE )
	std::list<INT64> m_AlteiaSendTicketList;
	BYTE m_cTicketCount;
	BYTE m_cSendTicketCount;
	BYTE m_cAlteiaBestGoldKeyCount;
	UINT m_uiAlteiaBestPlayTime;
	BYTE m_cWeeklyPlayCount;
	BYTE m_cDailyPlayCount;	
#endif

#if defined(PRE_ADD_DWC)
	UINT m_nDWCTeamID;
#endif

#if defined( PRE_ADD_CHARACTERCHECKSUM )
	UINT m_uiDBCheckSum;
	UINT m_uiRestoreCheckSum;
#endif // #if defined( PRE_ADD_CHARACTERCHECKSUM )

public:
	CDNUserSession();
	virtual ~CDNUserSession();

	CSocketContext* GetSocketContext(){ return m_pSocketContext; }

	void InitAccount(MAVICheckUser* pCheckUser);

	bool EnterWorld();
	bool LeaveWorld();

	bool InitObject(WCHAR *pName, UINT nUID, int nChannelID, int nMapIndex, TPosition *pCurPos);
	bool FinalObject();

	void SetCharacterName(const WCHAR* wszName);
	void SetSessionID(UINT nSessionID);
	void SetChannelID(int nChannelID);

	void SendInsideDisconnectPacket(wchar_t *pIdent);	// IN_DISCONNECT 처리

	//UserData
	virtual bool LoadUserData(TASelectCharacter *pSelect);
	virtual bool SaveUserData();

	// 저장
	void UpdateMoveToVillageUserData();	// 다른 village 서버로 갈때 저장
	void UpdateMoveToGameUserData();	// game 서버로 갈때 저장
	void LastUpdateUserData();			// 접속종료시 저장

	int FieldProcess(CDNBaseObject *pSender, USHORT wMsg, TBaseData *pSenderData, TParamData *pParamData);
	virtual void DoUpdate(DWORD CurTick);

	const WCHAR* GetIpW(){ return m_wszIP; }	// 게임서버랑 인터페이스 통일시키기 위해 추가.

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
	virtual void OnDBRecvMasterSystem(int nSubCmd, char* pData);
	virtual void OnDBRecvFarm(int nSubCmd, char * pData);
	virtual void OnDBRecvGuildRecruit(int nSubCmd, char * pData);
#if defined (PRE_ADD_DONATION)
	virtual void OnDBRecvDonation(int nSubCmd, char* pData) override;
#endif // #if defined (PRE_ADD_DONATION)
#if defined( PRE_PARTY_DB )
	virtual void OnDBRecvParty( int nSubCmd, char* pData );
#endif // #if defined( PRE_PARTY_DB )
#if defined (PRE_ADD_BESTFRIEND)
	virtual void OnDBRecvBestFriend( int nSubCmd, char* pData );
#endif
#if defined( PRE_PRIVATECHAT_CHANNEL )
	virtual void OnDBRecvPrivateChatChannel( int nSubCmd, char* pData );
#endif
#if defined( PRE_ALTEIAWORLD_EXPLORE )
	virtual void OnDBRecvAlteiaWorld( int nSubCmd, char* pData );
#endif
#if defined(PRE_ADD_DWC)
	virtual void OnDBRecvDWC( int nSubCmd, char* pData );
#endif


	int MessageProcess(int nMainCmd, int nSubCmd, char *pData, int nLen);

	virtual int OnRecvSystemMessage(int nSubCmd, char *pData, int nLen);
	virtual int OnRecvCharMessage(int nSubCmd, char *pData, int nLen);
	virtual int OnRecvActorMessage(int nSubCmd, char *pData, int nLen);
	virtual int OnRecvPropMessage(int nSubCmd, char *pData, int nLen);
	virtual int OnRecvPartyMessage(int nSubCmd, char *pData, int nLen);
	virtual int OnRecvItemMessage(int nSubCmd, char *pData, int nLen);
	virtual int OnRecvItemGoodsMessage(int nSubCmd, char *pData, int nLen);
	virtual int OnRecvNpcMessage(int nSubCmd, char *pData, int nLen);
	virtual int OnRecvQuestMessage(int nSubCmd, char *pData, int nLen);
	virtual int OnRecvChatMessage(int nSubCmd, char *pData, int nLen);
	virtual int OnRecvTradeMessage(int nSubCmd, char *pData, int nLen);
	virtual int OnRecvSkillMessage(int nSubCmd, char *pData, int nLen);
	virtual int OnRecvFriendMessage(int nSubCmd, char * pData, int nLen);
	virtual int OnRecvGuildMessage(int nSubCmd, char * pData, int nLen);
	virtual int OnRecvPvPMessage(int nSubCmd, char * pData, int nLen);
	virtual int OnRecvFarmMessage(int nSubCmd, char * pData, int nLen);
	virtual int OnRecvIsolateMessage(int nSubCmd, char * pData, int nLen);
	virtual int OnRecvAppellationMessage(int nSubCmd, char * pData, int nLen);
	virtual int OnRecvGameOptionMessage(int nSubCmd, char * pData, int nLen);
	virtual int OnRecvRadioMessage(int nSubCmd, char * pData, int nLen);
#ifdef _USE_VOICECHAT
	virtual int OnRecvVoiceChatMessage(int nSubCmd, char * pData, int nLen);
#endif
	virtual void OnDBRecvMsgadjustment(int nSubCmd, char * pData);
	virtual int OnRecvMasterSystemMessage( int nSubCmd, char* pData, int nLen );
#if defined( PRE_ADD_SECONDARY_SKILL )
	virtual int OnRecvSecondarySkillMessage( int nSubCmd, char* pData, int nLen );
#endif // #if defined( PRE_ADD_SECONDARY_SKILL )
	virtual int OnRecvChatRoomMessage(int nSubCmd, char *pData, int nLen);
	virtual int OnRecvGuildRecruitMessage( int nSubCmd, char* pData, int nLen );
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
	int _RecvInviteDWCTeamMembReq(LPCWSTR pwszToCharacterName);	
	int _RecvInviteDWCTeamMembAck(bool bAccept, UINT nFromAccountDBID, UINT nTeamID);
#endif

	int _MakeEmblemErrorCode( CDnItemCompounder::S_OUTPUT &Output );

	bool _CheckPvPGameModeInventory(UINT uiPvPIndex);

#if defined(PRE_ADD_TRANSFORM_POTION)
	void SetTransformID(int nTransformID, int nExpireMinute);
	int GetTransformID() { return m_nTransformID;}	
	void CalcTransformExpire(DWORD CurTick);
#endif

private:
	bool _CheckPvPGameModeInventory();

	int	_RecvMovePvPVillageToLobby();
	int	_RecvMovePvPLobbyToPvPVillage();

	short _RecvPvPCreateRoom( const CSPVP_CREATEROOM* pPacket );
	short _RecvPvPModifyRoom( const CSPVP_MODIFYROOM* pPacket );

	// Party
	int	_RecvPartyCreate( const CSCreateParty* pPacket, int iLen );
	int	_RecvPartyJoin( const CSJoinParty* pPacket, int iLen );
	int	_RecvPartyOut( int iLen );
	int	_RecvPartyListInfo( const CSPartyListInfo* pPacket, int iLen );
#if defined(PRE_MOD_REQ_JOIN_PARTY_ANSWER_MSG_APP)
	int	_RecvPartyInfo( const CSPartyInfo* pPacket, int iLen );
#endif
	int	_RecvPartyRefreshGateInfo( const CSRefreshGateInfo* pPacket, int iLen );
	int	_RecvPartyStartStage( const CSStartStage* pPacket, int iLen );
	int	_RecvPartyCancelStage( int iLen );
	int	_RecvPartyInvite( const CSInvitePartyMember* pPacket, int iLen );
	int	_RecvPartyInviteDenied( const CSInviteDenied* pPacket, int iLen );
	int	_RecvPartyInviteOverServerResult( const CSPartyInviteOverServerResult* pPacket, int iLen );
	int	_RecvPartyMemberInfo( const CSPartyMemberInfo* pPacket, int iLen );
	int	_RecvPartyMemberKick( const CSPartyMemberKick* pPacket, int iLen );
	int	_RecvPartyLeaderSwap( const CSPartyLeaderSwap* pPacket, int iLen );
	int	_RecvPartyChannelList( int iLen );
	int	_RecvPartySelectChannel( const CSVillageSelectChannel* pPacket, int iLen );
	int	_RecvPartyInfoModify( const CSPartyInfoModify* pPacket, int iLen );
	int	_RecvPartyReadyQuest( int iLen );
	int	_RecvPartyAskJoin( const CSPartyAskJoin* pPacket, int iLen );
	int	_RecvPartyAskJoinDecision( const CSPartyAskJoinDecision* pPacket, int iLen );
	int	_RecvPartySwapMemberIndex( const CSPartySwapMemberIndex* pPacket, int iLen );
	int	_RecvPartyConfirmLastDungeonInfo( const CSPartyConfirmLastDungeonInfo* pPacket, int iLen );
	int	_RecvPartySelectStage( const CSSelectStage* pPacket, int iLen );

#if defined( PRE_PARTY_DB )
	int	_RecvDBCreateParty( const TAAddParty* pA );
	int	_RecvDBJoinParty( const TAJoinParty* pA );
	int	_RecvDBOutParty( const TAOutParty* pA );
#endif // #if defined( PRE_PARTY_DB )

public:
	inline CDNUserSkill *GetSkill() { return m_pSkill; }

	static bool	bIsCheckPvPCreateRoom( CDNUserSession* pSession, const CSPVP_CREATEROOM* pPacket, const UINT uiMapIndex, UINT& uiPvPGameMode );

	void SetPartyID( TPARTYID PartyID ){ m_PartyID = PartyID; }
	void SetPartyData(TPARTYID PartyID);
	void SetTargetMapIndexByStartStage();
	int GetTargetMapIndex();
	void ClearPartyInfo();	// partyindex, leader, memberindex 초기화
	virtual TPARTYID GetPartyID() override { return m_PartyID; }
	int GetPartyMemberIndex() { return m_nPartyMemberIndex; }

	// PvP
	void CreatePvPRoom( const MAVIPVP_CREATEROOM* pPacket );

	void NextTalk(UINT nNpcObjectID, WCHAR* wszTalkIndex, WCHAR* wszTarget, std::vector<TalkParam>& talkParam  );

	void ChangeMap(BYTE cVillageID, BYTE cTargetGateNo, int nTargetChannelID, int nTargetMapIndex, char *pIp, USHORT nPort);
	void ChangeMap(BYTE cVillageID, int nTargetChannelID, int nTargetMapIndex, char *pIp, USHORT nPort, bool bPartyinto = false);	// cheat용
	void ChangePos(int nX, int nY, int nZ, float fLookX, float fLookZ);

	void ReconnectLogin(short nRet);
	void QueryGetMasterSystemCountInfo( bool bClientSend );
#ifdef PRE_ADD_COLOSSEUM_BEGINNER
	void SetPvPChannelType(BYTE cType) { m_ePvPChannel = static_cast<PvPCommon::RoomType::eRoomType>(cType); }
	BYTE GetPvPChannelType() { return static_cast<BYTE>(m_ePvPChannel); }
#endif		//#ifdef PRE_ADD_COLOSSEUM_BEGINNER

	// PvP
	void SetPvPRoomListRefreshTime( const DWORD dwTime ){ m_dwPvPRoomListRefreshTime = dwTime; }
	bool bIsPvPRoomListRefreshTime();
	void SetPvPVillageInfo( const BYTE cVillageID, const USHORT unChannelID ){ m_cPvPVillageID=cVillageID; m_unPvPVillageChannelID=unChannelID; }
	BYTE GetPvPVillageID() const { return m_cPvPVillageID; }
	USHORT GetPvPVillageChannelID() const { return m_unPvPVillageChannelID; }
	void SetPvPIndex( const UINT uiPvPIndex ){ m_uiPvPIndex = uiPvPIndex; }
	UINT GetPvPIndex() const { return m_uiPvPIndex; }

	bool bIsLadderUser();
	void SetLadderUser( bool bFlag ){ m_bIsLadderUser = bFlag; }

	//Farm
	bool ReqFarmInfo(bool bRefreshGate=true);

	virtual void SetLevel(BYTE cLevel, int nLogCode, bool bDBSave);
#if defined(PRE_ADD_EXPUP_ITEM)
	virtual void ChangeExp(int nChangeExp, int nLogCode, INT64 biFKey, bool bAbsolute=false);		// changeexp만큼 더하거나 빼주기(+, -를 써서)
#else
	virtual void ChangeExp(int nChangeExp, int nLogCode, INT64 biFKey);		// changeexp만큼 더하거나 빼주기(+, -를 써서)
#endif
	virtual void SetExp(UINT nExp, int nLogCode, INT64 biFKey, bool bDBSave);				// exp만큼 값 변경
	virtual void SetUserJob(BYTE cJob);
	virtual void SetHide(bool bHide);
	virtual void SetDefaultMaxFatigue(bool bSend = true);
	virtual void SetDefaultMaxWeeklyFatigue(bool bSend = true);
	virtual void SetDefaultMaxRebirthCoin(bool bSend = true);
	virtual void IncreaseFatigue(int nGap);

	virtual void RefreshGuildSelfView();

	virtual bool bIsUserSession(){ return true; }
	// Cmd
	int CmdSelectChannel( const int iChannelID, bool bSkipVerifyChannelID=false );
	void BroadcastingEffect(char cType, char cState);
	void BroadcastingChatRoom( int nSubCmd, UINT nChatRoomID );

	void	StartCostumeMix(int nInvenType, INT64 biInvenSerial);
	int		OnEndCostumeMix();
	void	StartCostumeDesignMix(int nInvenType, int nInvenIndex, INT64 biInvenSerial);
	int		OnEndCostumeDesignMix();
	void	ResetCostumeDesignCache();
	INT64	GetCosDesignMixSerial() {return m_biCosDesignMixSerialCache;}
#ifdef PRE_ADD_COSRANDMIX
	#ifdef PRE_ADD_COSRANDMIX_ACCESSORY
	void	StartCostumeRandomMix(CostumeMix::RandomMix::eOpenType openType, int nInvenType, INT64 biInvenSerial);
	CostumeMix::RandomMix::eOpenType GetCurrentRandomMixOpenType() const { return m_CosRandMixOpenType; }
	#else
	void	StartCostumeRandomMix(int nInvenType, INT64 biInvenSerial);
	#endif
	#ifdef PRE_ADD_COSRANDMIX_MIXERTYPE
	int GetCurrentRandomMixerEnablePartsType() const { return m_nCosRandMixerEnablePartsType; }
	#endif
	int		OnEndCostumeRandomMix();
	void	ResetCostumeRandomMixCache();
	INT64	GetRandomDesignMixSerial() {return m_biCosRandomMixSerialCache;}
#endif
	void	CheckValidCollisionHeight();
	void SetLastUseItemSkillID( int nSkillID ) { m_nLastUseItemSkillID = nSkillID; }
	int GetLastUseItemSkillID() { return m_nLastUseItemSkillID; }

	virtual void GuildWarReset();
	inline void SetGuildWarVote(bool bVote) { m_bGuildWarVote = bVote;};
	inline bool GetGuildWarVote() { return m_bGuildWarVote;};
	inline void SetGuildWarVoteLoading(bool bLoading) { m_bGuildWarVoteLoading = bLoading;};
	inline bool GetGuildWarVoteLoading() { return m_bGuildWarVoteLoading;};	
	inline int GetGuildWarPoint() { return m_nGuildWarPoint;};
	inline void SetGuildWarPoint(int nGuildWarPoint) { m_nGuildWarPoint=nGuildWarPoint;};
	inline void AddGuildWarPoint(int nAddPoint) { m_nGuildWarPoint += nAddPoint; };
	void SetGuildWarMissionRankingInfo(TAGetGuildWarPointPartTotal* pGetGuildWarPointPartTotal);
	inline SMyGuildWarMissionRankingInfo* GetGuildWarMissionRankingInfo() { return m_sMyGuildWarMissionRaningInfo;};
	inline bool GetGuildWarStats() { return m_bGuildWarStats;};
	void SetGuildWarRewardResults(bool* pRewardResults );
	void SetGuildWarRewardResultIndex(char cRewardType, bool bResult);
	char GetGuildWarRewardEnable();
	bool IsGuildWarReward();
	void AddGuildWarBuyedItem(int itemID, int count);
	int GetGuildWarBuyedItem(int itemID) const;
	void ResetGuildWarBuyedItems();

#ifdef PRE_FIX_VILLAGEZOMBIE
	inline DWORD GetInsideDisconnectTick() { return m_dwCalledSendInsideDisconnectTick; }
	void SetZombieChecked()	{ m_bZombieChecked = true; }
#endif		//#ifdef PRE_FIX_VILLAGEZOMBIE

	inline bool IsMove() {return m_bIsMove;}
	inline bool IsStartGame() {return m_bIsStartGame;}

#ifdef PRE_ADD_CHANGEJOB_CASHITEM
	// 전직 아이템 사용시 같은 차수로 직업이 변경 가능한지 여부를 체크.
	bool CheckAndCalcParallelChangeJob( /*IN*/ int iJobID, /*OUT*/ int& iFirstJobID,  /*OUT*/ int& iSecondJobID );
#endif // #ifdef PRE_ADD_CHANGEJOB_CASHITEM

	void SetCheckLastDungeonInfo( bool bFlag ){ m_bIsCheckLastDungeonInfo = bFlag; }
	bool bIsCheckLastDungeonInfo(){ return m_bIsCheckLastDungeonInfo; }

	virtual bool bIsCheckPing(){ return m_pField ? true : false; }
	virtual void WritePingLog();

	void UseExpandSkillPage();	//스킬트리확장 아이템 사용시 기본스킬 및 스킬포인트 저장

	int TryWarpVillage(int nMapIndex, INT64 nItemSerial);
	void WarpVillage(MAVITargetVillageInfo* pPacket);

#if defined( PRE_ADD_NAMEDITEM_SYSTEM )
	void ApplyPartyEffectSkillItemData( CSUseItem *pPacket, TItemData *pItemData, UINT nUseSessionID, EffectSkillNameSpace::ShowEffectType::eType eType , bool bSendDB, bool bUseItem );	
#endif
#if defined( PRE_FIX_BUFFITEM )
	void DelPartyEffectSkillItemData();
#endif
#if defined(PRE_FIX_62281)
#if defined(PRE_PARTY_DB)
	int CheckPartyInvitableState(const WCHAR * pInviterName, int nPartyMinLevel); //파티 초대 가능 여부 체크
#else
	int CheckPartyInvitableState(const WCHAR * pInviterName, int nPartyMinLevel, int nPartyMaxLevel);
#endif
#endif
#if defined( PRE_ADD_NEWCOMEBACK )
	void CheckComebackRewardItem( TLevelupEvent ComebackInven );
	void SetComebackAppellation( int nAppelationID );	
#endif

#if defined( PRE_ALTEIAWORLD_EXPLORE )
	void AlteiaWorldInfo( AlteiaWorld::CSAlteiaWorldInfo* pPacket );
	void AlteiaWorldDailyResetInfo();
	void AlteiaWorldWeeklyResetInfo();
	int AlteiaWorldJoin();
	void AddAlteiaTicket( int nCount = 1 );
	void DelAlteiaSendTicket( int nCount = 1 );
	void AddAlteiaSendTicketList( INT64 biCharacterDBID ) { m_AlteiaSendTicketList.push_back(biCharacterDBID); }	
	void SetAlteiaBestGoldKeyCount( BYTE cAlteiaBestGoldKeyCount ) { m_cAlteiaBestGoldKeyCount = cAlteiaBestGoldKeyCount; }
	void SetAlteiaBestPlayTime( UINT uiAlteiaBestPlayTimeCount ) { m_uiAlteiaBestPlayTime = uiAlteiaBestPlayTimeCount; }
	void SetAlteiaWeeklyPlayCount( BYTE cAlteiaWeeklyPlayCount ) { m_cWeeklyPlayCount = cAlteiaWeeklyPlayCount; }
	void SetAlteiaDailyPlayCount( BYTE cAlteiaDailyPlayCount ) { m_cDailyPlayCount = cAlteiaDailyPlayCount; }
	void SetAlteiaTicketCount( BYTE cTicketCount ) { m_cTicketCount = cTicketCount; }	
	void SetAlteiaSendTicketCount( BYTE cSendTicketCount ) { m_cSendTicketCount = cSendTicketCount; }	
	BYTE GetAlteiaBestGoldKeyCount() { return m_cAlteiaBestGoldKeyCount; }
	UINT GetAlteiaBestPlayTime() { return m_uiAlteiaBestPlayTime; }
	BYTE GetAlteiaWeeklyPlayCount() { return m_cWeeklyPlayCount; }
	BYTE GetAlteiaDailyPlayCount() { return m_cDailyPlayCount; }	
	BYTE GetAlteiaTicketCount() { return m_cTicketCount; }	
	BYTE GetAlteiaSendTicketCount() { return m_cSendTicketCount; }	
	bool bIsAlteiaSendTicketList( INT64 biCharacterDBID );
	void AlteiaWorldSendTicketInfo();
#endif

#if defined( PRE_DRAGONBUFF )
	void ApplyWorldBuff( WCHAR* wszCharacterName, int nItemID );
#endif

#if defined(PRE_ADD_DWC)	
	UINT GetDWCTeamID() { return m_nDWCTeamID; }
	void SetDWCTeamID( UINT nDWCTeamID ) { m_nDWCTeamID = nDWCTeamID; }
	void RefresDWCTeamName();
#endif

};

class ScopeEncryptFlag
{
public:

	ScopeEncryptFlag( bool* pFlag ):m_pFlag(pFlag){ *m_pFlag = false; }
	~ScopeEncryptFlag(){ *m_pFlag = false; }

private:

	bool* m_pFlag;
};
