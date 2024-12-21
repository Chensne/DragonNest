#pragma once

#include "DnActorState.h"
#include "MIInventoryItem.h"
#include "DNItemTypeDef.h"
#include "DNTableFile.h"

const int NUM_DISJOINT_DROP = 2;

class CDnItem : public CDnState, public MIInventoryItem {
public:
	CDnItem();
	virtual ~CDnItem();

	enum ItemReversionEnum {
		NoReversion,
		Belong,
		Trade,
		GuildBelong,
		OnlyTradeWorldStorage,

		ItemReversionEnum_Amount,
	};


	// 서버랑 공용으로 쓰기위해 DnCommonDef.h 에 있는거 쓴다
	// 그냥 DnCommonDef.h 꺼만 쓸려고 했으나..이미 기존 코드가 넘 많아 귀찮아서 redefine
	enum ItemSkillApplyType
	{
		None = Item::SkillUsingType::None,
		ApplyStateBlow = Item::SkillUsingType::ApplyStateBlow,
		ApplySkill = Item::SkillUsingType::ApplySkill,
		Eternity = Item::SkillUsingType::Eternity,					// 효과 영구 적용 아이템
		SkillLevelUp = Item::SkillUsingType::SkillLevelUp,			// 스킬 레벨업용 아이템
		PrefixSkill = Item::SkillUsingType::PrefixSkill,			// 접두사 스킬용 아이템
	};

	struct DropItemStruct {
		int nItemID;
		int nSeed;
		char cOption;
		int nCount;
		DWORD dwUniqueID;
	};

	static int s_nStateValueMatchUIStringIndex[];
	static int GetStateValueMatchUIStringIndex(int nIndex);
	static bool IsEffectSkillItem(int nItemType, bool bCheckVisible = false);
	static bool IsVisualChangeItem(int nItemType);

protected:
	int m_nSeed;
	int m_nClassID;

	tstring m_szName;
	eItemTypeEnum m_ItemType;
	eItemRank m_Rank;
	ItemReversionEnum m_Reversion;
	ItemSkillApplyType m_SkillApplyType;
	int m_nLevelLimit;
	bool m_bIsCashItem;

#ifdef PRE_ADD_CASHREMOVE	
	__time64_t m_tExpireTime; // 삭제대기시간.
	bool m_bIsCashRemoveItem; // 삭제된캐쉬아이템 여부.
#endif

	bool m_bNeedAuthentication;
	bool m_bCanDestruction;
	int m_nAmount;
	int m_nNeedBuyLadderPoint;  // 구입시 필요한 레더포인트
	int m_nNeedBuyGuildWarPoint;
	int m_nNeedBuyUnionPoint;
	int m_nMaxOverlapCount;
	int m_nOverlapCount;
	int m_nSellAmount;

	int m_nRootSoundIndex;
	int m_nDragSoundIndex;
	int m_nUseSoundIndex;

	DnSkillHandle m_hSkill;
	int m_nSkillID;
	int m_nSkillLevel;
	int m_nTypeParam[2];
	int m_nExchangeType;

	INT64 m_nSerialID;

	std::vector<int> m_nVecPermitJobList;

	float m_fCoolTime;		// UI 에서 외부 표시용으로 사용할 변수.. (0.0 ~ 1.0 클리핑된 값)
	float m_fDelayTime;
	float m_fElapsedDelayTime;

	bool m_bNewGain;

	bool m_bAuthentication;
	bool m_bIdentity;

	int m_nEnchantTableID;
	int m_nEnchantID;
	char m_cEnchantLevel;
	char m_cMaxEnchantLevel;
	char m_cPotentialIndex;
	char m_cOptionIndex;
	bool m_bSoulBound;
	char m_cSealCount;
	int m_nSealID;
	bool	m_bEternity;
	time_t	m_ExpireDate;
	int m_nLookItemID;
#ifdef PRE_ADD_DMIX_DESIGNNAME
	tstring m_szLookItemDescriptionCache;
#endif // PRE_ADD_DMIX_DESIGNNAME
#ifdef PRE_ADD_EXCHANGE_POTENTIAL
	char m_cPotentialMoveCount;
#endif
#ifdef PRE_ADD_REDUCE_TABLE_RESOURCE
	int	m_nOptionSkillID;
	int m_nOptionSkillLevel;
	int m_nOptionSkillUsingType;
#endif // PRE_ADD_REDUCE_TABLE_RESOURCE

	CDnState *m_pEnchant;
	CDnState *m_pPotential;

	bool m_bCanDisjoint;
	int m_nDisjointType;
	int m_nDisjointCost;
	int m_nDisjointDropTableID[NUM_DISJOINT_DROP];
#if defined(PRE_FIX_69709)
	int m_nEnchantDisjointDrop;
#endif

	int m_nAllowedGameType;

	int m_nNeedBuyItem;			// 구입시 필요아이템
	int m_nNeedBuyItemCount;	// 구입시 필요아이템 수량
	int m_nNeedPvpRank;			// 착용시 필요 PVPRank

	bool m_bShowAlarmDialog;

	static std::map<INT64, CDnItem *> s_nMapSerialSearch;
	static CSyncLock s_SerialSearchLock;

	bool m_bEnableCostumeMix;
	int m_nCashTradeCount;

	bool m_bInfoItem;

	int m_nApplicableValue;
#ifdef PRE_ADD_ITEMCAT_TOOLTIP_INFO
	int m_nCategoryType;
#endif
#if defined(PRE_ADD_SERVER_WAREHOUSE)
	bool m_bWstorage;
#endif
#if defined( PRE_ADD_STAGE_USECOUNT_ITEM )
	bool m_bUseLimited;
#endif

#ifdef PRE_ADD_DRAGON_GEM
	int m_nDragonGemType;
#endif

protected:
	static int GetRandomStateNumber(int nMin, int nMax);
	static float GetRandomStateRatio(float fMin, float fMax);

	bool IsPossibleProb(DNTableFileFormat* pSox, const char *szLabel);
	static void DummyRandom(int nCount = 1);

	void InitializeEnchant();
	void InitializeOption();
	void InitializePotential();

	static void InsertSerialSearchMap(CDnItem *pItem);
	static void RemoveSerialSearchMap(CDnItem *pItem);

	static int GetOptionTableID(int nItemTableID, char cOptionIndex);

public:
	virtual bool Initialize(int nTableID, int nSeed, char cOptionIndex = 0, char cEnchantLevel = 0, char cPotentialIndex = 0, char cSealCount = 0, bool bSoulBound = false, int nLookItemID = ITEMCLSID_NONE);

	// 기본정보만을 보기 위해 반드시 필요한 정보들로 구성된 초기화 함수. 현재 가챠에서 사용됨.
	bool InitializeForInformation(int nTableID, int nSeed, char cOptionIndex);

#ifdef PRE_ADD_DMIX_DESIGNNAME
	TCHAR* GetName() const;
	const TCHAR *GetDescription();
#else
	TCHAR* GetName() const { return (TCHAR*)m_szName.c_str(); }
#endif
	virtual int GetClassID() const { return m_nClassID; }
	int GetSeed() const { return m_nSeed; }

	int GetMaxOverlapCount() { return m_nMaxOverlapCount; }
	int GetItemAmount() const { return m_nAmount; }
	int GetItemSellAmount() const { return m_nSellAmount; }
	int GetLevelLimit() const { return m_nLevelLimit; }

	void SetOverlapCount(int nValue) { m_nOverlapCount = nValue; }
	int GetOverlapCount() const { return m_nOverlapCount; }

	INT64 GetSerialID() const { return m_nSerialID; }
	void SetSerialID(INT64 nSerial);

	eItemTypeEnum GetItemType() const { return m_ItemType; }
	eItemRank GetItemRank() const { return m_Rank; }
	ItemReversionEnum GetReversion() const { return m_Reversion; }
	bool IsNeedAuthentication() { return m_bNeedAuthentication; }
	bool IsCanDestruction() { return m_bCanDestruction; }
	bool IsCashItem() const { return m_bIsCashItem; }

#ifdef PRE_ADD_CASHREMOVE
	void SetCashRemoveItem(bool bRemove) {
		m_bIsCashRemoveItem = bRemove;
	} // 삭제된캐쉬아이템 여부.
	bool IsCashRemoveItem() const { return m_bIsCashRemoveItem; } // 삭제된캐쉬아이템 여부.

	void SetExpireTime(__time64_t _time) { m_tExpireTime = _time; }
	__time64_t GetExpireTime() { return m_tExpireTime; }
#endif

	bool IsEnableCostumeMixItem() const { return m_bEnableCostumeMix; }
	int GetLookItemID() const { return m_nLookItemID; }
	void SetLookItemID(int nLookItemID, bool bChangeIconIdx);
#ifdef PRE_ADD_DMIX_DESIGNNAME
	void MakeLookItemDescription(DNTableFileFormat* pTableFormat);
#endif // PRE_ADD_DMIX_DESIGNNAME

	int GetRootSoundIndex() { return m_nRootSoundIndex; }
	int GetDragSoundIndex() { return m_nDragSoundIndex; }
	int GetUseSoundIndex() { return m_nUseSoundIndex; }

	void PlayInstantUseSound();

	static bool IsPermitPlayer(const int nItemID, const std::vector<int> &nVecJobList);
	bool IsNeedJob() const;
	bool IsPermitPlayer(const std::vector<int> &nVecJobList) const;
	int GetNeedJobCount() const { return (int)m_nVecPermitJobList.size(); }
	int GetNeedJob(int nIndex) const;

	bool ActivateSkillEffect(DnActorHandle hActor, bool bPickUp = false);
	bool DeactivateSkillEffect();

	static CDnItem *CreateItem(int nTableID, int nSeed, bool bSoulBound = false, int nLookItemID = ITEMCLSID_NONE);
	static eItemTypeEnum GetItemType(int nItemTableID);
	static int GetItemTypeParam(int nItemTableID, int nIndex = 0);
	static tstring GetItemName(int nItemTableID);
	static tstring GetItemFullName(int nItemTableID, char cOptionIndex = 0);
	static bool IsCashItem(int nTableID);
	int  GetAllowedGameType() { return m_nAllowedGameType; }

	void SetElapsedDelayTime(float fDelayTime) { m_fElapsedDelayTime = fDelayTime; };
#ifdef PRE_ADD_SKILLCOOLTIME
	float GetElapsedDelayTime() const { return m_fElapsedDelayTime; }
#endif

	// from MIInventoryItem
	virtual float GetCoolTime() { return m_fCoolTime; };
	virtual ItemConditionEnum GetItemCondition();

	void ProcessCoolTime(LOCAL_TIME LocalTime, float fDelta);
	int GetSkillID(void) { return m_nSkillID; };
	int GetSkillLevel(void) { return m_nSkillLevel; };
	int GetSkillLevelID(void);
	void BeginCoolTime(void);
	virtual int GetClickedEquipPage();

	bool IsNewGain() { return m_bNewGain; }

	// bExistItem
	// 새로운 자리로 들어오는 거라면 false,
	// 있던 자리에 들어오는 거라면 true(물약 갯수 증가)
	void SetNewGain(bool bNew, bool bExistItem = false);

	virtual bool IsQuickSlotItem();

	//	DWORD GetColor() const { return m_dwColor; }
	//	void SetColor( DWORD dwColor ) { m_dwColor = dwColor; }
	bool IsAuthentication() { return m_bAuthentication; }
	void SetAuthentication(bool bValue) { m_bAuthentication = bValue; }
	bool IsIdentity() { return m_bIdentity; }
	void SetIdentity(bool bValue) { m_bIdentity = bValue; }

	int GetEnchantTableID() { return m_nEnchantTableID; }
	int GetEnchantID() { return m_nEnchantID; }
	bool IsActiveEnchant() { return (m_cEnchantLevel == 0) ? false : true; }
	bool IsActivePotential() { return (m_cPotentialIndex == 0) ? false : true; }
	char GetEnchantLevel() const { return m_cEnchantLevel; }
	void SetEnchantLevel(char nLevel) { m_cEnchantLevel = nLevel; }
	char GetMaxEnchantLevel() { return m_cMaxEnchantLevel; }
	char GetPotentialIndex() const { return m_cPotentialIndex; }
	char GetOptionIndex() const { return m_cOptionIndex; }
	int GetTypeParam(int nIndex = 0) const { return m_nTypeParam[nIndex]; };
	void SetTypeParam(int nIndex, int nValue) { m_nTypeParam[nIndex] = nValue; }
	int GetExchangeType() { return m_nExchangeType; }
	bool IsSoulbBound() const;
	char GetSealCount() const { return m_cSealCount; }
	void SetSoulBound(bool bSet) { m_bSoulBound = bSet; }
	int GetSealID() { return m_nSealID; }
	void SetSealCount(char cCount) { m_cSealCount = cCount; } // 요건 쓰면안됨..툴팁땜에..
#ifdef PRE_ADD_EXCHANGE_POTENTIAL
	int GetPotentialMoveCount() const { return m_cPotentialMoveCount; }
	bool IsPotentialExtractableItem();
#endif
#if defined(PRE_ADD_PCBANG_RENTAL_ITEM)
	bool IsPcCafeRentItem();
#endif
#ifdef PRE_ADD_ITEMCAT_TOOLTIP_INFO
	int GetCategoryType() const { return m_nCategoryType; }
#endif
#if defined(PRE_ADD_SERVER_WAREHOUSE)
	int IsWstorage() const { return m_bWstorage; }
#endif

	bool IsEternityItem() const;
	void SetEternityItem(bool bEternity);
	const time_t* GetExpireDate() const;
	void SetExpireDate(const time_t& date);

	CDnState *GetEnchantState() { return m_pEnchant; }
	CDnState *GetPotentialState() { return m_pPotential; }

	int GetDisjointType() { return m_nDisjointType; }
	bool CanDisjointType(int nDisjointType);
	bool CanDisjoint(void);
	int GetDisjointCost(void) { return m_nDisjointCost; };
	int GetDisjointItemDrop() { return m_nDisjointDropTableID[0]; };//rlkt_disjoint

	int GetNeedBuyItem() { return m_nNeedBuyItem; }
	int GetNeedBuyItemCount() { return m_nNeedBuyItemCount; }
	int GetNeedPvpRank() const { return m_nNeedPvpRank; }
	int GetNeedBuyLadderPoint() { return m_nNeedBuyLadderPoint; }
	int GetNeedBuyGuildWarPoint() { return m_nNeedBuyGuildWarPoint; }
	int GetNeedBuyUnionPoint() { return m_nNeedBuyUnionPoint; }

	int GetApplicableValue() { return m_nApplicableValue; }

	//// 분해시에 드랍될 아이템들 미리 생성해 둠
	//void GenerationDropItem( void ); 
	//bool Disjoint( void );

	//
	static CDnItem *FindItemFromSerialID(INT64 nSerialID);
	static void CalcStateValue(CDnState *pState, int nStateType, char *szMin, char *szMax, bool bRandomValue = true, bool bResetValue = true, float fRatio = 1.0f);
#ifdef PRE_FIX_MEMOPT_ENCHANT
	static void CalcStateValue(CDnState *pState, int nStateType, float fMin, float fMax, int nMin, int nMax, bool bRandomValue = true, bool bResetValue = true);
#endif

	int GetTotalSealCount(int nSealItemTypeParam);
	bool GetCharmItemKeyID(std::vector<int> &vecKeyID);

	float GetDelayTime(void) { return m_fDelayTime; }

	bool IsShowAlarmDialog() { return m_bShowAlarmDialog; }

	virtual CDnItem &operator = (TItem &e);

	// 인벤토리에 있는 아이템을 다른 창에 등록할때 이 TItemInfo를 사용해 CreateItem으로 복사아이템 만들어 쓰면 된다.
	virtual void GetTItemInfo(TItemInfo &Info);

	// 미션이나 보상창, 아이템 제작창처럼 가지고 있는 아이템은 아니고 들어오게될 아이템을 보여줄때 이 함수 사용해 얻으면 된다.
	static bool MakeItemInfo(int nItemID, int nCount, TItemInfo &ItemInfo);
	static bool MakeItemInfo(int nItemID, int nCount, TItem &ItemInfo);
	static void MakeItemDurationString(int iDurationTimeByMS, /*IN OUT*/ wstring &strDurationTime);

	int GetSkillApplyType(void) const { return m_SkillApplyType; };
	bool IsInfoItem() const { return m_bInfoItem; }
	void SetInfoItem(bool bInfoItem);

	//#ifdef PRE_ADD_GACHA_JAPAN
	//	const CDnState* GetOptionState( void ) { return &m_OptionState; };
	//#endif // PRE_ADD_GACHA_JAPAN

	bool HasPrefixSkill();
	bool HasPrefixSkill(int &nSkillID, int &nSkillLevel);
	bool HasLevelUpInfo(int &nSkillID, int &nSkillLevel, int &nSkillUsingType);

	const TCHAR* GetSkillDescription(void);
	int GetSkillSEDurationTime(void);
	void ChangeToSkillIconIndex(void);

	int GetSkillEffectIconIndex(bool bBuff = true);
#ifdef PRE_ADD_SERVER_WAREHOUSE
	bool IsTradable(bool bIsWStorage) const;
#endif

#if defined( PRE_ADD_STAGE_USECOUNT_ITEM )
	bool IsUseLimited() const { return m_bUseLimited; }
	void SetUseLimit(bool bFlag) { m_bUseLimited = bFlag; }
#endif
#ifdef PRE_ADD_DRAGON_GEM
	int GetDragonGemType() { return m_nDragonGemType; }
#endif
	bool IsDragonGemApplicable() { return (m_nDragonGemType > 0); }
public:
	void ChangeIconIndex(int nIconIndex) { m_nIconImageIndex = nIconIndex; m_Reversion = NoReversion; }
	void ChangeRank(eItemRank itemRank) { m_Rank = itemRank; m_Reversion = NoReversion; }
	void ChangeItemType(eItemTypeEnum itemType) { m_ItemType = itemType; m_Reversion = NoReversion; }
};
