#pragma once

class CSyncLock;
class CDNUserSession;
class CDNUserSendManager;
class CDNParty
{
private:
	UINT m_nPartyMemberArray[PARTYMAX];		// m_nAccountDBID로 관리된다
	bool m_bPartyCheckMemberArray[PARTYMAX];		// push 되었을 경우 필요함.
	bool m_boCompleteMember;				// 모두 같은 게이트에 있을때
	UINT m_nLeaderSessionID;				//
	UINT m_nLeaderAccountDBID;				//

	ULONG m_nCreateTick, m_nPreTick, m_nCurTick;
	std::vector < std::pair <ULONG, WCHAR*> > m_VecInviteList;
	CSyncLock m_Sync;

#if defined( PRE_PARTY_DB )
	
	Party::Data m_Data;

	INT64 m_biSortPoint;

#else
	TPARTYID m_PartyID;
	int m_nChannelID;
	int m_nChannelMeritID;
	int m_nPartyMapIndex;
	WCHAR m_wszPartyName[PARTYNAMELENMAX];
	int m_nMemberMax;
	int m_nTargetMapIdx;
	TDUNGEONDIFFICULTY m_Difficulty;
	int m_nUserLvLimitMin;
	int m_nUserLvLimitMax;
	TPARTYITEMLOOTRULE m_ItemLootRule;
	TITEMRANK m_ItemLootRank;
	int m_nUpkeepCount;								//파티클리어 유지 카운트
#endif // #if defined( PRE_PARTY_DB )

	UINT m_nRandomSeed;
	//파티 이동 동기관련
	bool m_bIsMove;				//요건 파티 이동시(SetGameID를 받은 이후에)
	bool m_bIsStartGame;		//
	std::vector <UINT> m_VecKickList;				//영구킥리스트

public:

#if defined( PRE_PARTY_DB )
#else
	WCHAR m_wszPartyPassword[PARTYPASSWORDMAX];
	bool m_bRaidParty;
	BYTE m_cIsJobDice;
#endif // #if defined( PRE_PARTY_DB )

#ifdef _USE_VOICECHAT
	UINT m_nVoiceChannelID;
	ULONG m_nTalkingTick;
	TMemberInfo m_MemberVoiceInfo[PARTYMAX];		//게임 -> 마을로 돌아오는 순간에만 사용
#endif

public:

	CDNParty(ULONG nCurTick);
	virtual ~CDNParty(void);

#if defined( PRE_PARTY_DB )
	void Create( TPARTYID PartyID, const DBCreateParty* pData );
#else
	void Create(CDNUserSession * pUser, TPARTYID PartyID, const WCHAR * pPartyName, const WCHAR * pPassWord, int nMemberMax, TPARTYITEMLOOTRULE ItemLootRule, TITEMRANK ItemRank, int nLvLimitMin, int nLvLimitMax, int nTargetMapIdx, TDUNGEONDIFFICULTY Difficulty, BYTE cIsPartyJobDice);
#endif // #if defined( PRE_PARTY_DB )

#if defined( PRE_PARTY_DB )
	void PushParty( TAAddPartyAndMemberVillage* pPacket );
#else
	void PushParty( MAVIPushParty* pPacket );
#endif

#if defined( PRE_PARTY_DB )
	void SetPartyID( TPARTYID PartyID ){ m_Data.PartyID = PartyID; }	
	Party::Data GetPartyStructData() const { return m_Data; }
	void SetPartyStructData( Party::Data PartyData ) { m_Data = PartyData; }
	TPARTYID GetPartyID() const { return m_Data.PartyID; }
#else
	void SetPartyID( TPARTYID PartyID ){ m_PartyID = PartyID; }
	TPARTYID GetPartyID() const { return m_PartyID; }
#endif // #if defined( PRE_PARTY_DB )
	bool DoUpdate(DWORD CurTick);

	bool AddPartyMember(UINT nAccountDBID, UINT nSessionID, const WCHAR * pMemberName, int &nMemberIdx, bool boLeader = false);	// 멤버추가
	bool DelPartyMember(CDNUserSession * pUserObj, char cKickKind);
#if defined( PRE_PARTY_DB )
	bool DelPartyMember( const TAOutParty* pA, char cKickKind );
#endif
	bool SetPartyMemberID(UINT nLeaderDBID, UINT nAccountDBID, int nMemberIdx);	//파티 구겨 넣는용

#if defined( PRE_PARTY_DB )
	bool ModifyPartyInfo(int nChannelID, const WCHAR * pName, int iPassword, BYTE cPartyMemberMax, TPARTYITEMLOOTRULE LootRule, TITEMRANK LootRank, BYTE cUserLvLimitMin, int nTargetMapIdx, TDUNGEONDIFFICULTY Difficulty, int iBitFlag, int &nWaitListRet);
#else
	bool ModifyPartyInfo(int nChannelID, const WCHAR * pName, const WCHAR * pPass, BYTE cPartyMemberMax, TPARTYITEMLOOTRULE LootRule, TITEMRANK LootRank, BYTE cUserLvLimitMin, BYTE cUserLvLimitMax, int nTargetMapIdx, TDUNGEONDIFFICULTY Difficulty, BYTE cJobDice, int &nWaitListRet);
#endif // #if defined( PRE_PARTY_DB )
	int GetPartyMemberIdx(UINT nAccountDBID);
	bool IsKickedMember(UINT nAccountDBID);

	bool bIsPasswordParty() const;
	bool bIsMove() const { return m_bIsMove; }
	bool bIsStartGame() const { return m_bIsStartGame; }
#if defined( PRE_PARTY_DB )
	int GetPartyMapIndex() const;
	const WCHAR* GetPartyName() const { return m_Data.wszPartyName; }
	int GetTargetMapIndex() const { return m_Data.iTargetMapIndex; }
	int GetMinLevel() const { return m_Data.cMinLevel; }
	int GetMemberMax() const { return m_Data.nPartyMaxCount; }
	void SetMemberMax( short nCount ){ m_Data.nPartyMaxCount = nCount; }
	TDUNGEONDIFFICULTY GetDifficulty() const { return m_Data.TargetMapDifficulty; }
	TPARTYITEMLOOTRULE GetItemLootRule() const { return m_Data.LootRule; }
	TITEMRANK GetItemLootRank() const { return m_Data.LootItemRank; }
	int GetUpkeepCount() const { return m_Data.iUpkeepCount; }
	void SetUpkeepCount( int iCount ){ m_Data.iUpkeepCount = iCount; }
	bool bIsInVillage() const { return m_Data.LocationType == Party::LocationType::Village; }
	bool bIsInGame() const { return m_Data.LocationType == Party::LocationType::Worldmap; }
	INT64 GetLeaderCharacterID() { return m_Data.biLeaderCharacterDBID; }
	int GetCurMemberCount() { return m_Data.iCurMemberCount; }
	void SetTargetMap( int nTargetMap ) { m_Data.iTargetMapIndex = nTargetMap; }
	void SetPartyType( ePartyType ePartyType ) { m_Data.Type = ePartyType; }
	void SetLocationType( Party::LocationType::eCode eType ) { m_Data.LocationType = eType; }
#if defined( PRE_ADD_NEWCOMEBACK )
	bool CheckPartyMemberAppellation( int nAppellationID );	
	bool CheckPartyMemberAppellation();	
	void ApplyJoinMemberAppellation( CDNUserSession * pSession );
	void DelOutMemberAppellation( CDNUserSession * pSession );
	void DelPartyMemberAppellation( int nAppellationID );
	void SetComebackParty( bool bFlag ) { m_Data.bCheckComeBackParty = bFlag; }
	bool bIsComebackParty() { return m_Data.bCheckComeBackParty; }
#endif
#else
	int GetPartyMapIndex() const{ return m_nPartyMapIndex; }
	const WCHAR* GetPartyName() const { return m_wszPartyName; }
	int GetTargetMapIndex() const { return m_nTargetMapIdx; }
	int GetMinLevel() const { return m_nUserLvLimitMin; }
	int GetMaxLevel() const { return m_nUserLvLimitMax; }
	int GetMemberMax() const { return m_nMemberMax; }
	void SetMemberMax( short nCount ){ m_nMemberMax = nCount; }
	TDUNGEONDIFFICULTY GetDifficulty() const { return m_Difficulty; }
	TPARTYITEMLOOTRULE GetItemLootRule() const { return m_ItemLootRule; }
	TITEMRANK GetItemLootRank() const { return m_ItemLootRank; }
	int GetUpkeepCount() const { return m_nUpkeepCount; }
	void SetUpkeepCount( int iCount ){ m_nUpkeepCount = iCount; }
#endif // #if defined( PRE_PARTY_DB )
	void SetRandomSeed( UINT uiSeed );
	UINT GetRandomSeed() const { return m_nRandomSeed; }
#if defined( PRE_PARTY_DB )
	int GetChannelID() const;
	int GetChannelMeritID() const;
#else
	int GetChannelID() const { return m_nChannelID; }
	int GetChannelMeritID() const { return m_nChannelMeritID; }
#endif // #if defined( PRE_PARTY_DB )
	void SetChannelID( int iChannelID );
	void SetMove( bool bFlag );
	void SetStartGame( bool bFlag );
	const std::vector<UINT>& GetKickList(){ return m_VecKickList; }

#if defined( PRE_PARTY_DB )
	bool CheckPassword( int iPassword );
	void SetSortPoint( INT64 biPoint ){ m_biSortPoint = biPoint; }
	INT64 GetSortPoint() const { return m_biSortPoint; }
	void GetCharNameList( std::vector<std::wstring>& vCharName );
	int GetServerID() const { return m_Data.iServerID; }
#else
	bool CheckPassword( const WCHAR* pwszPassword );
#endif // #if defined( PRE_PARTY_DB )
	bool HasEmptyPlace();
	void BanAllPartyMember(UINT nLeaderAccountDBID);					// 모두 강퇴~
	int GetMemberInfo(SPartyMemberInfo *pInfo);								// 파티 멤버들 정보 얻어오기
	void GetMemberCharacterDBIDInfo( std::vector<INT64>& Data, INT64 biExceptCharDBID=0 );
	int GetMemberPartsInfo(int *EquipArray);								// 파티 멤버들 equip정보 얻어오기
	void SetMemberTargetMapIndexByStartStage();								// 시작버튼을 누르면 파티 멤버들 값 세팅 싹~ 해준다
	int CheckSameGateNo();													// 파티원 모두 게이트 번호가 같은지 체크
	int GetMemberCount() const;													// 파티원이 몇명인지 얻어오기
	void GetMembersByAccountDBID(UINT * nMembers);
	void GetMemberCheckFlag(bool * bMembers);
	bool DiffMemberGateNo(char cGateNo);
	bool InvitePartyMember(const WCHAR * pMemberName);
	bool InviteDenied(const WCHAR * pMemberName);
#if defined( PRE_PARTY_DB )
	int GetPartyInfo(SPartyListInfo * pInfo);
#else
	int GetPartyInfo(SPartyListInfo * pInfo, const sChannelInfo * pChInfo);
#endif // #if defined( PRE_PARTY_DB )
	int GetAvrUserLv();
	bool CheckCompleteParty();
	bool AdjustmentPartyInfo();
	bool ChangePartyLeader(UINT nNewLeaderAID = 0);
	bool IsPartyMemberWindowStateNone();	// window_none상태면 true, 아니면 false
	bool IsMember(UINT nAccountDBID);
	bool SwapMemberIndex( const CSPartySwapMemberIndex * pPacket);
	void SortMemberIndex(int nOutIndex);
	bool CheckPartyUserStartFlag();
	void SetPartyUserStartFlag(bool bFlag);
	void SetPartyUserMoveFlag(bool bFlag);
#if defined( PRE_PARTY_DB )
	ePartyType GetPartyType() const { return m_Data.Type; }
#if defined( PRE_WORLDCOMBINE_PARTY )
	int GetWorldPartyPrimaryIndex() const { return m_Data.nPrimaryIndex; }
	bool bIsWorldCombieParty() const { return Party::bIsWorldCombineParty(m_Data.Type); }
#endif // #if defined( PRE_WORLDCOMBINE_PARTY )
	int GetBitFlag() const { return m_Data.iBitFlag; }
	int GetPassword() const { return GetBitFlag()&Party::BitFlag::Password ? m_Data.iPassword : Party::Constants::INVALID_PASSWORD; }
	static bool ValidatePassword( int iPassword );
#endif // #if defined( PRE_PARTY_DB )
	bool bIsRaidParty();

#ifdef _USE_VOICECHAT
	bool CreateVoiceChannel(UINT nVoiceChannelID);
	void DestroyVoiceChannel();
	void TalkingUpdate(ULONG nCurTick);
	void SetInitVoiceInfo(TMemberInfo * pInfo, int nCount);
	TMemberVoiceInfo * GetInitVoiceInfo(UINT nAccountDBID);
	
	void SendRefreshVoiceInfo(UINT nOutAccountDBID = 0);
#endif

	void SendAllRefreshParty(int nRefreshSubject);
	void SendAllRefreshGateInfo();
	void SendAllGateInfo(int nGateNo);
	void SendAllFarmInfo();
	void SendAllCancelStage( short cRetCode );
	void SendAllReadyToGame(ULONG nIP, USHORT nPort, USHORT nTcpPort);
	void SendAllRoomID( GameTaskType::eType GameTaskType, BYTE cReqGameIDType, USHORT wGameID, int nRoomID);
	void SendSelectStage(char cSelectIndex);
#if defined( PRE_PARTY_DB )
#else
	void SendPartyMemberInfo(bool bIsRaidParty, CDNUserSendManager * pSender);
#endif // #if defined( PRE_PARTY_DB )
	void SendKickPartyMember(UINT nSessionID);
	void SendPartyMemberMove(UINT nSessionID, EtVector3 vPos);
	void SendPartyMemberMoveEachOther(UINT nSessionID, int nX, int nY, int nZ);
	void SendPartyMemberPart(CDNUserSession *pSession);
	void SendPartyChat(char cType, int cLen, const WCHAR *pwszCharacterName, const WCHAR *pwszChatMsg, int nRet = ERROR_NONE);
	void SendPartyUseRadio(UINT nSessionID, USHORT nRadioID);
	void SendPartyReadyRequest();
	void SendUserProfile(UINT nSessionID, TProfile & profile);
	void SendAllSwapMemberIndex(CSPartySwapMemberIndex * pPacket);
	
	inline void SetCompleteMember(bool boFlag) { m_boCompleteMember = boFlag; }		// 모두 시작눌러서 점프 준비 완료
	inline bool GetCompleteMember() { return m_boCompleteMember; }
	inline UINT GetLeaderSessionID() { return m_nLeaderSessionID; }
	inline void SetLeaderSessionID(UINT nLeaderUID) { m_nLeaderSessionID = nLeaderUID; }
	inline UINT GetLeaderAccountDBID() { return m_nLeaderAccountDBID; }
	inline void SetLeaderAccountDBID(UINT nNewLeaderAccountDBID) { m_nLeaderAccountDBID = nNewLeaderAccountDBID; }

	// 모든 파티 멤버의 게이트 번호를 임의로 세팅한다;
	void SetAllMemberGate(int nGateNo);

	void UpdateMasterSystemCountInfo();
#if defined( PRE_ADD_NAMEDITEM_SYSTEM )
	void SendPartyEffectSkillItemData();
	void ApplyPartyEffectSkillItemData( CSUseItem *pPacket, TItemData *pItemData, UINT nUseSessionID, EffectSkillNameSpace::ShowEffectType::eType eType, bool bSendDB, bool bUseItem );
	void RemovePartyEffectSkillItemData( int nSkillID, int nItemID );
#endif

	//Bonus
	void CheckFriendBonus();
#ifdef PRE_ADD_BEGINNERGUILD
	void CheckBeginnerGuildBonus();
#endif		//#ifdef PRE_ADD_BEGINNERGUILD

protected:
	UINT PickNewLeader();
};
