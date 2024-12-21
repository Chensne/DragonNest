#pragma once

#include "DNTableFile.h" 
#include "DNDataManagerBase.h"

#include "DnItemCompounder.h"
#include "DnSkillTreeSystem.h"
#include "DnCostumeMixDataMgr.h"
#ifdef PRE_ADD_COSRANDMIX
#include "DnCostumeRandomMixDataMgr.h"
#endif

class QuestCondition;
class CDNGameDataManager : public CDNDataManagerBase
{
public:
	typedef map<int, TCollectionBookData> TMapCollectionBook;	// first: CollectionID

#if !defined( _FINAL_BUILD ) && !defined (__COVERITY__)
	FRIEND_TEST( Trigger_unittest, PerformanceTest );
#endif // #if !defined( _FINAL_BUILD ) && !defined (__COVERITY__)

#if defined(PRE_FIX_NEXTSKILLINFO)
protected:
	static SKILL_LEVEL_INFO ms_SkillLevelTableInfo;
	SKILL_LEVEL_TABLEID_LIST m_SkillLevelTableIDList[2];

	void InitSkillLevelTableIDList();
	void AddSkillLevelTableID(int nSkillID, int nSkillLevel, int nSkillLevelTableID, int nApplyType);
	//rlkt_pvp_skills
	void AddToPvpSkillList(int iSkillLevelTableID, int nSkillID, int iSkillLevel, float fNewDamageMultiplier);
#endif // PRE_FIX_NEXTSKILLINFO

private:
	struct TUpdateLocalTime
	{
		DWORD dwUpdateTick;
		tm pCurLocalTime;
	};

public:
	struct TPVPSkills
	{
		int nSkillID;
		int nSkillLevel;
		float fPDamage;
	};

protected:
	std::map<int, TPVPSkills *> m_pPvpSkills;

private:
	TUpdateLocalTime m_UpdateLocalTime[30];

	typedef map<int, TMapInfo*> TMapInfoMap;
	TMapInfoMap m_pMapInfo;
	typedef map<int, TItemData*> TMapItemData;
	TMapItemData m_pItemData;
	typedef map<int, TActorData*> TMapActorData;
	TMapActorData m_pActorData;
	typedef map<int, TNpcData*> TMapNpcData;
	TMapNpcData m_pNpcData;

	typedef map<int, TWeaponData*> TMapWeaponData;
	TMapWeaponData m_pWeaponData;
	typedef map<int, TPartData*> TMapPartData;
	TMapPartData m_pPartData;

	typedef map<int, TPotentialJewelData*> TMapPotentialJewelData;
	TMapPotentialJewelData m_pPotentialJewelData;
	typedef map<int, TEnchantJewelData*> TMapEnchantJewelData;
	TMapEnchantJewelData m_pEnchantJewelData;

	TALK_MAP m_TalkMap;
	TALK_ANSWER_MAP m_TalkAnswerMap;	// NPC 대사 별 문단응답(talk_answer / link_index) 스트링집합(단지 클라이언트 대사요청 무결성 체크 오류메시지를 막는 용도)

	typedef map<int, TShopData*> TMapShopData;
	TMapShopData m_pShopData;
	typedef map<int, TSkillShopData*> TMapSkillShopData;
	TMapSkillShopData m_pSkillShop;
	typedef map<int, TSkillData*> TMapSkillData;
	TMapSkillData m_pSkillData;
	typedef map<int, TSkillTreeData*> TMapSkillTreeData;
	TMapSkillTreeData m_pSkillTreeData;
	vector<TSkillTreeData*> m_vlpSkillTreeData;

	typedef map<BYTE, TCoinCountData*> TMapCoinCount;	// first: Level
	TMapCoinCount m_pMapCoinCount;

	typedef map<DWORD, std::wstring> TXMLFileMap;
	TXMLFileMap  m_XMLFileMap; 
	typedef map<DWORD, std::wstring> TXMLIndexMap;
	TXMLIndexMap m_XMLIndexMap;

	typedef map<UINT, TQuestReward>	TMapQuestReward;
	typedef TMapQuestReward::iterator TMapQuestRewardIt;
	TMapQuestReward m_QuestRewardMap;

	typedef vector<int> TVecEventTable;
	TVecEventTable m_EventTable;	//디비베이스 이벤트와는 관계없음
	typedef map<tr1::tuple<char, char>, TQuestLevelCapReward> TMapQuestLevelCapReward;
	TMapQuestLevelCapReward m_QuestLevelCapRewards;
	typedef map<UINT, std::set<int> >	TMapQuestItemData;
	typedef TMapQuestItemData::iterator TMapQuestItemDataIt;
	TMapQuestItemData	m_QuestItemData;
	typedef map<UINT, QuestCondition*>	TMapQuestConditionData;
	typedef TMapQuestConditionData::iterator TMapQuestConditionDataIt;
	TMapQuestConditionData m_QuestConditionData;

	typedef map<int, TEnchantData*> TMapEnchantData;
	typedef map<char, TEnchantLevelData*> TMapEnchantLevelData;
	TMapEnchantData m_pEnchantTable;
#ifdef PRE_FIX_MEMOPT_ENCHANT
	typedef map<int, TEnchantNeedItemData> TMapEnchantNeedItemData;
	TMapEnchantNeedItemData m_EnchantNeedItemTable;
#endif
	typedef map<int, TPotentialData*> TMapPotentialData;
	TMapPotentialData m_pPotentialTable;
	typedef map<int, TItemDropData*> TMapItemDropData;
	TMapItemDropData m_pItemDropTable;
#if defined(PRE_ADD_STAGE_CLEAR_ADD_REWARD)
	typedef map<int, TBonusDropTable> TMapBonusDropTable;
	TMapBonusDropTable m_MapBonusDropTable;
#endif // #if defined(PRE_ADD_STAGE_CLEAR_ADD_REWARD)
#if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
	typedef map<int, TItemDropEnchantData> TMapItemDropEnchantTable;
	TMapItemDropEnchantTable m_MapItemDropEnchantTable;
#endif
	typedef map<int, TAppellationData> TMapAppellationData;
	TMapAppellationData m_MapAppellationData;
	std::vector<TMissionData *> m_pMissionTable;
	typedef map<int, TDailyMissionData *> TMapDailyMissionData;
	TMapDailyMissionData m_pDailyMissionTable;
	std::vector<TDailyMissionData *> m_pWeekendEventMissionTable;
	std::vector<TDailyMissionData *> m_pGuildWarMissionTable;
	std::vector<TGuildWarRewardData *> m_pGuildWarRewardData;
	std::vector<TDailyMissionData *> m_pGuildCommonMissionTable;
	std::vector<TDailyMissionData *> m_pWeekendRepeatMissionTable;
#if defined(PRE_ADD_CHAT_MISSION)
	std::vector<int> m_pTypingTable;
#endif

	TMapCollectionBook m_MapCollectionBookData;

	typedef vector<TTaxData*> TVecTaxData;
	TVecTaxData m_pTaxData;
	typedef vector<TSealCountData*> TVecSealCountData;
	TVecSealCountData m_pSealCountData;

	typedef map<int, TPCBangData*> TMapPCBangData;
	TMapPCBangData m_pPCBangData;

	typedef vector<std::pair<int, int>> TVecVillagePermitLevel;
	TVecVillagePermitLevel m_vVillagePermitLevel;

	typedef vector<std::pair<int, int>> TVecPermitPartyCount;
	TVecPermitPartyCount m_vPermitPartyCount;
	typedef vector <TPromotionData*> TVecPromotion;
	TVecPromotion m_vLvPromotion;

	// PvP
	typedef map<UINT, TPvPMapTable*> TPvPMapTableData;
	TPvPMapTableData m_PvPMapTable;
	typedef map<UINT, TPvPGameModeTable*> TPvPGameModeTableData;
	TPvPGameModeTableData m_PvPGameModeTable;
	typedef map<UINT, TPvPGameStartConditionTable*>	TPvPGameStartConditionData;
	TPvPGameStartConditionData m_PvPGameStartConditionTable;
	typedef map<UINT, TPvPRankTable*> TPvPRankTableData;
	TPvPRankTableData m_PvPRankTable;
#ifdef PRE_MOD_PVPRANK
	UINT m_nThresholdPvPExpValue;
#endif		//#ifdef PRE_MOD_PVPRANK

	// Darklair
	typedef map<int, TDLMapData *> TMapDLMapData;
	TMapDLMapData m_pDLMapTable;
	typedef map<int, TDLClearData *> TMapDLClearData;
	TMapDLClearData m_pDLClearTable;

	typedef map<int,TItemOptionTableData> TMapItemOptionTableData;
	TMapItemOptionTableData m_ItemOptionTableData;

	// 호감도
#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
	typedef map<int,TReputeTableData>	TMapReputeTableData;
	TMapReputeTableData m_ReputeTableData;
	typedef map<int,TPresentTableData>	TMapPresentTableData;
	TMapPresentTableData m_PresentTableData;
	typedef map<int,TMailTableData>		TMapMailTableData;
	TMapMailTableData m_MailTableData;
	typedef multimap<int,TStoreBenefitData>	TMapStoreBenefitData;
	TMapStoreBenefitData m_StoreBenefitData;
#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )

	typedef map<std::tr1::tuple<int,int,int>,TCombinedShopTableData>	TMapCombinedShopTableData;
	TMapCombinedShopTableData m_CombinedShopTableData;
	std::map<int,int> m_CombinedShopIndex;

	typedef map<int,TMasterSystemDecreaseTableData>	TMapMasterSystemDecreaseTableData;
	TMapMasterSystemDecreaseTableData m_MasterSystemDecreaseTableData;
	typedef map<std::pair<int,TDUNGEONDIFFICULTY>,TMasterSystemGainTableData> TMapMasterSystemGainTableData;
	TMapMasterSystemGainTableData m_MasterSystemGainTableData;
#if defined( PRE_ADD_SECONDARY_SKILL )
	typedef map<int,TSecondarySkillTableData> TMapSecondarySkillTableData;
	TMapSecondarySkillTableData	m_SecondarySkillTableData;
	typedef map<int,std::vector<TSecondarySkillLevelTableData>>	TMapSecondarySkillLevelTableData;
	TMapSecondarySkillLevelTableData m_SecondarySkillLevelTableData;
	typedef map<int,TSecondarySkillRecipeTableData>	TMapSecondarySkillRecipeTableData;
	TMapSecondarySkillRecipeTableData m_SecondarySkillRecipeTableData;
#endif // #if defined( PRE_ADD_SECONDARY_SKILL )
	typedef map<int,int> TMapInstantItemTableData;
	TMapInstantItemTableData m_InstantItemTableData;
	typedef map<int,TFarmCultivateTableData> TMapFarmCultivateTableData;
	TMapFarmCultivateTableData m_FarmCultivateTableData;
	typedef map<int, TFishingTableData> TMapFishingTableData;
	TMapFishingTableData m_FishingTableData;
	typedef map<int, TFishingPointTableData> TMapFishingPointTableData;
	TMapFishingPointTableData m_FishingPointTableData;
	typedef map<int, TFishingAreaTableData> TMapFishingAreaTableData;
	TMapFishingAreaTableData m_FishingAreaTableData;

	typedef map<int, TJobTableData>		TMapJobTableData;
	TMapJobTableData m_JobTableData;

#ifdef PRE_ADD_GACHA_JAPAN
	typedef map<int, TGachaponData_JP *> TMapGachaData_JP;
	TMapGachaData_JP m_mapGachaData;
#endif
	// 디폴트 creteskill 정보만 채워둠.
	map<int, TDefaultCreateData> m_mapDefaultCreateData;

	// 문장보옥/아이템 조합 담당하는 객체. 빌리지 서버에서만 아이템조합/문장보옥 사용함
	CDnItemCompounder* m_pItemCompounder;
	// 스킬트리
	CDnSkillTreeSystem* m_pSkillTreeSystem;

	CDnCostumeMixDataMgr m_CosMixDataMgr;
#ifdef PRE_ADD_COSRANDMIX
	CDnCostumeRandomMixDataMgr m_CosRandMixDataMgr;
#endif

	// CashShop
	typedef map<int, TCashCommodityData*> TMapCashCommodityData;
	TMapCashCommodityData m_pCashCommodityData;
	typedef map<int, TCashPackageData*> TMapCashPackageData;
	TMapCashPackageData m_pCashPackageData;
	typedef vector<TLevelupEvent*> TVecLevelupEvent;
	TVecLevelupEvent m_pVecLevelupEvent;
#if defined(PRE_ADD_VIP)
	typedef vector<TVIPData*> TVecVIPData;
	TVecVIPData m_pVecVIPData;
#endif	// #if defined(PRE_ADD_VIP)

	typedef map<int, TGlyphSkillData> TMapGlyphSkillData;
	TMapGlyphSkillData m_MapGlyphSkill;
	typedef vector<TGlyphSlotData> TVecGlyphSlot;
	TVecGlyphSlot m_VecGlyphSlot;
	typedef vector<TGlyphChargeData> TVecGlyphCharge;
	TVecGlyphCharge m_VecGlyphCharge;
	typedef map<int, TCharmItemKeyData> TMapCharmKeyItem;	// first: CharmItemID
	TMapCharmKeyItem m_MapCharmKeyItem;
	typedef map<int, TCharmItemData> TMapCharmItem;	// first: CharmNo 
	TMapCharmItem m_MapCharmItem;
	typedef map<int, TCharmCountData> TMapCharmCount;
	TMapCharmCount m_MapCharmCount;
	typedef map<int, TVehicleData> TMapVehicleData;	// first: ItemID
	TMapVehicleData m_MapVehicle;
	typedef map<int, TVehiclePartsData> TMapVehicleParts;	// first: ItemID
	TMapVehicleParts m_MapVehicleParts;
	typedef map<int, TPetLevelData*> TMapPetLevelData;	// first: PetLevelTypeID
	TMapPetLevelData m_pPetLevelData;
	typedef map<int, TPetFoodData> TMapPetFoodData;	// first: ItemID
	TMapPetFoodData m_MapPetFoodData;
#if defined (PRE_ADD_CHAOSCUBE)
	typedef map<int, TChaosStuffItem> TMapChaosStuffItem;	// first: ItemID
	TMapChaosStuffItem m_MapChaosStuffItem;
	typedef map<int, TChaosItemData> TMapChaosItem;	// first: ChaosNo
	TMapChaosItem m_MapChaosItem;
#endif

	std::map<int, int> m_GuildLimitItem;	
	std::map<int, int> m_GuildLimitCoin;
	int m_nGuildLimitMaxItem;
	int m_nGuildLimitMaxCoin;

	typedef map<int, TGuildLevel> TMapGuilLevel;	// first: Level
	TMapGuilLevel m_GuildLevel;
	typedef map<int, TGuildWarPoint> TMapGuilWarPoint;	// first: Level
	TMapGuilWarPoint m_GuildWarPoint;
	int m_nMaxGuildExp;
	int m_nMaxGuildLevel;

	typedef std::map <unsigned int, std::wstring> TProhibitWord;
#if defined(PRE_ADD_MULTILANGUAGE)
	std::map <int, TProhibitWord> m_ProhibitWordList;
#else		//#if defined(PRE_ADD_MULTILANGUAGE)
	TProhibitWord m_ProhibitWordList;
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)

	typedef map<int, UINT>	TMapQuestRewardID;
	TMapQuestRewardID m_MapQuestCommonRewardID;

	typedef map<int, TExchangeTrade> TMapExchangeTrade;	// first: ItemID
	TMapExchangeTrade m_MapExchangeTrade;
	std::vector<char> m_VecExchangeGroupID[ITEMEXCHANGEGROUP_MAX];

	typedef std::map<int, TPvPMissionRoom> TMapPvPMissionRoom;	// first: ItemID
	TMapPvPMissionRoom m_MapPvPMissionRoom;
	TVecPvPMissionRoom m_VecPvPMissionRoom;

	typedef vector <TMonsterMutationGroup> TVecMonsterMutationGroup;
	TVecMonsterMutationGroup m_VecMonsterMutationGroup;
	typedef map <int,TMonsterMutationData> TMapMonsterMutationData;
	TMapMonsterMutationData	m_mMonsterMutationData;
	typedef vector <TMonsterMutationSkill> TVecMonsterMutationSkill;
	TVecMonsterMutationSkill m_VecMonsterMutationSkill;
	typedef vector <TGhoulConditionData> TVecGhoulModeCondition;
	TVecGhoulModeCondition m_VecGhoulModeCondition;
	map<pair<int,int>,vector<pair<int,int>>> m_mPvPGameModeSkillSetting;

	std::map <int, TBattleGourndModeInfo> m_mBattleGroundModeInfo;
	std::map <int, std::vector<TPositionAreaInfo>> m_mBattleGroundPositionInfo;
	std::vector <TBattleGroundSkillInfo> m_vBattleGroundSkill;
	std::vector <TBattleGroundEffectValue> m_vBattleGroundSkillLevel;

	typedef vector <TUnionReputeBenefitData> TVecUnionReputeBenefitData;
	TVecUnionReputeBenefitData m_VecUnionReputeBenefitData;
	typedef vector <TGuildMarkData> TVecGuildMarkData;
	TVecGuildMarkData m_VecGuildMarkData;	
	std::map<int,TPlayerCommonLevelTableInfo> m_mPlayerCommonLevelTable;
	std::vector <TMasterSysFeelTableInfo> m_vMasterSysFeelTableInfo;
	typedef vector <TGlobalEventData> TVecGlobalEvent;
	TVecGlobalEvent	m_VecGlobalEvent;

	typedef vector<TEveryDayEventData> TVecEveryDayEvent;
	TVecEveryDayEvent m_VecEveryDayEvent;

	//지금은 cash관련 테이블 두개(TCASHCOMMODITY, TCASHPACKAGE)에만 사용한다. 추가로 라이브 도중 리로드가 필요하여질 경우 사용 영역을 넓히자 처리시 펑션들 구조를 변경해야함(중요!)
	CSyncLock m_Sync;

	typedef map<int, int> TMapPeriodAppellationID;	//first:ID second:arrayID
	TMapPeriodAppellationID m_mPeriodAppellation;

	typedef map<int, int> TMapGuildAppellationID;
	TMapGuildAppellationID m_mGuildAppellation;
#if defined( PRE_ADD_BESTFRIEND )
	typedef map<int, int> TMapBestFriendAppellationID;
	TMapBestFriendAppellationID m_mBestFriendAppellation;
#endif

	typedef map<int,int> TMapGuildReversionTableData;
	TMapGuildReversionTableData m_GuildReversionTableData;

#if defined( PRE_PARTY_DB )
	typedef std::map<Party::SortWeightIndex::eCode,int> TMapPartySortWeight;
	TMapPartySortWeight m_PartySortWeight;
	
	typedef std::map<int,int> TMapDungeonSortWeight;
	TMapDungeonSortWeight m_DungeonSortWeight;
#endif // #if defined( PRE_PARTY_DB )

#ifdef _WORK
	bool m_bAllLoaded;
#endif		//#ifdef _WORK
	int m_nMaxPlayerLevel;
	typedef map<int, TGuildRewardItemData> TMapGuildRewardItem;
	TMapGuildRewardItem m_mGuildRewardItemTableData;
#if defined(PRE_ADD_SALE_COUPON)
	typedef map<int, TSaleCouponData> TMapSaleCoupon;
	TMapSaleCoupon m_mSaleCouponItemTableData;
#endif

	vector<int> m_vVillageMaps;

#if defined( PRE_ADD_NAMEDITEM_SYSTEM )
	typedef map<int, TNamedItemData> TMapNamedItem;
	TMapNamedItem m_mNamedItemTableData;
#endif

#ifdef PRE_ADD_EXCHANGE_POTENTIAL
	typedef vector<TPotentialTransferData> TVecPotentialTransferItem;
	TVecPotentialTransferItem m_vPotentialTransferList;
#endif		//#ifdef PRE_ADD_EXCHANGE_POTENTIAL

	typedef vector<int> TVecCashErasableType;
	TVecCashErasableType m_VecCashErasableType;

#if defined( PRE_WORLDCOMBINE_PARTY )	
	typedef std::map<int,WorldCombineParty::WrldCombinePartyData> TMapWorldCombinePartyData;
	TMapWorldCombinePartyData m_mWorldCombinePartyData;
#endif
#if defined( PRE_ADD_PCBANG_RENTAL_ITEM )
	typedef std::map<int,TPcBangRentItem> TMapPcBangRentItemData;
	TMapPcBangRentItemData m_mPcBangRentItemData;
#endif
#if defined(PRE_ADD_EXCHANGE_ENCHANT)
	typedef vector<TExchangeEnchantData> TVecExchangeEnchantData;
	TVecExchangeEnchantData m_vExchangeEnchantData;
#endif

#if defined(PRE_ADD_INSTANT_CASH_BUY)
	typedef map<int, TCashBuyShortcutData> TMapCashBuyShortcut;
	TMapCashBuyShortcut m_MapCashBuyShortcut;
#endif	// #if defined(PRE_ADD_INSTANT_CASH_BUY)

#if defined(PRE_ADD_WEEKLYEVENT)
	typedef map<int, WeeklyEvent::TWeeklyEvent*> TMapWeeklyEvent;
	TMapWeeklyEvent m_pMapWeeklyEvent;
#endif	// #if defined(PRE_ADD_WEEKLYEVENT)

#if defined( PRE_ADD_TOTAL_LEVEL_SKILL )
	typedef map<int, TotalLevelSkill::TTotalSkillLevelTable> TMapTotalSkillLevelTable;
	TMapTotalSkillLevelTable m_MapTotalSkillLevelTabel;
	typedef map<int, TotalLevelSkill::TTotalSkillSlotTable> TMapTotalSkillSlotTable;
	TMapTotalSkillSlotTable m_MapTotalSkillSlotTabel;
	typedef map<int, std::list<TotalLevelSkill::TTotalSkillBlowTable> > TMapTotalSkillBlow;
	TMapTotalSkillBlow m_MapTotalSkillBlow;	
#endif

#if defined(PRE_SPECIALBOX)
	typedef map<int, TKeepBoxProvideItemData*> TMapKeepBoxProvideItemData;
	TMapKeepBoxProvideItemData m_pMapKeepBoxProvideItemData;
#endif	// #if defined(PRE_SPECIALBOX)

#ifdef PRE_ADD_JOINGUILD_SUPPORT
	typedef vector<TGuildSupportData> TVecGuildSupportData;
	TVecGuildSupportData m_VecGuildSupportData;
#endif		//#ifdef PRE_ADD_JOINGUILD_SUPPORT

#if defined(PRE_ADD_REMOTE_QUEST)
	typedef std::vector<TRemoteQuestData *> TVecRemoteQuestData;
	TVecRemoteQuestData m_pQuestRemoteTable;
 	typedef map<int, int>	TMapRemoteQuestID;
 	TMapRemoteQuestID m_MapRemoteQuestID;
#endif
#if defined(PRE_ADD_ACTIVEMISSION)
	typedef std::vector<TActiveMissionData*> TVecActiveMissionData;
	TVecActiveMissionData m_pActiveMissionTable;
	std::map<int, std::vector<int>> m_ActiveSetTable;
#endif

#if defined( PRE_WORLDCOMBINE_PVP )	
	typedef std::map<int, TWorldPvPMissionRoom> TMapWorldPvPMissionRoom;	// first: ItemID
	TMapWorldPvPMissionRoom m_MapWorldPvPMissionRoom;
	TVecWorldPvPMissionRoom m_VecWorldPvPMissionRoom;
	std::list<int> m_listWorldPvPRoomMode;
#endif

#if defined( PRE_ALTEIAWORLD_EXPLORE )
	typedef std::map<int, TAlteiaWorldMapInfo> TMapAlteiaWorldMapInfo;	// first: ItemID
	TMapAlteiaWorldMapInfo m_MapAlteiaWorldMapInfo;	
	int m_nAlteiaWorldMapMaxCount;		
	time_t m_tAlteiaWorldBeginTime;
	time_t m_tAlteiaWorldEndTime;
#endif // #if defined( PRE_ALTEIAWORLD_EXPLORE )

#if defined( PRE_ADD_STAMPSYSTEM )
	typedef std::map<INT64, StampSystem::TStampTableData> TMapStampTableData; // first: StartTime
	TMapStampTableData m_MapStampTableData;
#endif // #if defined( PRE_ADD_STAMPSYSTEM )

#if defined(PRE_ADD_TALISMAN_SYSTEM)
	typedef std::vector<TTalismanSlotData*> TVecTalismanSlotData;
	TVecTalismanSlotData m_pTalismanSlotTable;

	typedef map<int, TTalismanData> TMapTalismanData;
	TMapTalismanData m_MapTalismanTable;
#endif

#if defined( PRE_DRAGONBUFF )
	typedef map<int, time_t> TMapWorldBuffData;
	TMapWorldBuffData m_MapWorldBuffData;
#endif

#if defined( PRE_PVP_GAMBLEROOM )	
	typedef std::map<int, TPvPMissionRoom> TMapPvPGambleRoom;	// first: ItemID
	TMapPvPGambleRoom m_MapPvPGambleRoom;
#endif

#if defined( PRE_ADD_STAGE_WEIGHT )
	typedef std::map<int, std::map<DWORD, TStageWeightData>> TMapStageWeightData;
	TMapStageWeightData m_MapStageWeightData;
#endif // #if defined( PRE_ADD_STAGE_WEIGHT )

public:
	CDNGameDataManager(void);
	virtual ~CDNGameDataManager(void);

	bool AllLoad();

#ifdef _WORK
	bool GetLoaded() { return m_bAllLoaded; }
#endif		//#ifdef _WORK

	void DoUpdate(DWORD dwCurTick, int nThreadIndex);

	bool LoadMapInfo();
	const TMapInfo * GetMapInfo(int nMapID) const;
	int GetMapIndexByGateNo( int nMapIndex, int nGateNo, int nSelect=0 );
	int GetGateNoByGateNo( int nMapIndex, int nGateNo, int nSelect=0 );
	GlobalEnum::eMapTypeEnum GetMapType( int nMapIndex );
	GlobalEnum::eMapSubTypeEnum GetMapSubType( int nMapIndex );
	bool IsFindValidMap( const TMapInfo* pMapInfo, int nTargetIndex );
	bool CheckChangeMap( int nPrevMapIndex, int nNextMapIndex );
	bool IsMapExpandable(int nMapIndex);
	bool IsVehicleMode(int nMapIndex);
	bool IsPetMode(int nMapIndex);
	int GetMapPermitLevel(int nMapID);
	TVecVillagePermitLevel GetVillageMapPermitLevel() const { return TVecVillagePermitLevel(m_vVillagePermitLevel); }
	
#if defined(PRE_ADD_CHALLENGE_DARKLAIR)
	bool IsCloseGateByTime(int nMapIndex);
#endif
#if defined(PRE_ADD_VEHICLE_SPECIAL_ACTION)
	bool IsVehicleSpecialActionMode(int nMapIndex);
#endif

	bool LoadItemData();
	TItemData* GetItemData(int nItemID);
	const TItemData* GetItemData(int nItemID) const;
	bool IsItemNeedJob(int nItemID, int nJob);
	bool IsPermitItemJob(int nItemID, BYTE *cJobArray);
	bool IsItemNeedJobHistory( int nItemID, const BYTE* cJobArray, bool bForDice = false );
	bool IsItemOnlyNeedJob(int nItemID, int nJob) const;
	int GetItemMainType(int nItemID);		// 아이템 타입중에 eItemTypeEnum 인지
	BYTE GetItemDetailType(int nItemID);	// eItemTypeEnum중 하나라면 eWeaponType, eParts ... 중에 하나인지
	BYTE GetItemDetailType(TItemData * pItemData);	// eItemTypeEnum중 하나라면 eWeaponType, eParts ... 중에 하나인지
	BYTE GetItemRank(int nItemID);
	int GetItemOverlapCount(int nItemID);
	BYTE GetItemReversion(int nItemID);
	int GetItemPrice(int nItemID);
	bool IsItemDestruction(int nItemID);
	char GetItemSkillUsingType(int nItemID);
	int GetItemTypeParam1(int nItemID);
	int GetItemTypeParam2(int nItemID);
	bool IsCashItem(int nItemID);
	int GetItemCashTradeCount(int nItemID);
	BYTE GetItemLevelLimit(int nItemID);
	char GetItemSealCount(int nItemID);

	bool IsUseItemAllowMapTypeCheck(int iItemID, int iMapIndex);

	int GetMapPermitPartyCount(int nMapID);
	const CDnCostumeMixDataMgr& GetCosMixDataMgr() const { return m_CosMixDataMgr; }
#ifdef PRE_ADD_COSRANDMIX
	bool LoadCostumeRandomMixData();
	CDnCostumeRandomMixDataMgr& GetCosRandomMixDataMgr() { return m_CosRandMixDataMgr; }
#endif

	bool LoadActorData();
	TActorData* GetActorData(int nActorIndex);

	bool LoadNpcData();
	TNpcData* GetNpcData( int nNpcIndex );

	bool LoadWeaponData();
	TWeaponData* GetWeaponData(int nWeaponIndex);

	bool LoadPartData();
	TPartData* GetPartData(int nPartIndex);
	const TPartData* GetPartData(int nPartIndex) const;
	bool IsSubParts(int nPartIndex);
	bool IsFaceParts(int nPartIndex);
	bool IsHairParts(int nPartIndex);

	bool LoadPotentialJewelData();
	TPotentialJewelData *GetPotentialJewelData( int nItemID );

	bool LoadEnchantJewelData();
	TEnchantJewelData *GetEnchantJewelData( int nItemID );

	int GetItemDurability( int nItemID );
	int GetItemDurabilityRepairCoin(int nItemID);

	int GetExp(int nClass, BYTE cLevel);
	int GetFatigue(int nClass, char cLevel);
	int GetWeeklyFatigue(int nClass, char cLevel);
	BYTE GetLevel(int nClass, int nCurExp);
#if defined(PRE_ADD_VIP)
	int GetVIPExp(int nClass, int nLevel);
	int GetVIPFatigue(int nClass, int nLevel);
#endif	// #if defined(PRE_ADD_VIP)

	// CoinTable
	bool LoadCoinCount();
	int GetCoinCount(BYTE cLevel, char cWorldID);
	int GetCashCoinCount(BYTE cLevel, char cWorldID);
#if defined(PRE_ADD_VIP)
	int GetVIPRebirthCoinCount(BYTE cLevel);
#endif	// #if defined(PRE_ADD_VIP)
	int GetCoinLimit(BYTE cLevel);
	int GetCashCoinLimit(BYTE cLevel);

	bool ReadTalkFile(const char* szFolderName);
	bool LoadTalkData();
	bool AddTalk(const WCHAR* _wszTalkFileName);
	bool RemoveTalk(const WCHAR* wszTalkFileName);
	bool GetTalk(const WCHAR* wszTalkFileName, OUT TALK_PARAGRAPH_MAP& TalkParagraphMap );
	bool GetTalkParagraph(IN std::wstring& wszIndex, IN std::wstring& wszTarget, OUT TALK_PARAGRAPH& talk );
	TALK_PARAGRAPH & GetEmptyTalkParagraph();
	void GetTalkFileList(IN OUT std::vector<std::wstring>& TalkFileList);
	DWORD GetStringHashCode(const WCHAR* pString);
	bool GetTalkFileName(DWORD nHashCode, OUT std::wstring& wszIndex);
	bool GetTalkIndexName(DWORD nHashCode, OUT std::wstring& wszIndex);
	bool CheckTalkAnswer(IN std::wstring& wszIndex, IN std::wstring& wszTarget);

	bool LoadQuestReward();
	bool GetQuestReward(UINT nRewardCheck, OUT TQuestReward& recom);
	bool GetQuestCommonRewardByQuestID(int nQuestID, OUT TQuestReward& recom);	//클래스 공통 리워드 정보만 처리함

	bool LoadPlayerCustomEventUI();
	void GetPlayerCustomEventUIList(std::vector <int> &vList);

	bool LoadQuestLevelCapReward();
	const TQuestLevelCapReward* GetQuestLevelCapReward(char cType, char cClassID) const;

	// 해당 퀘스트에 연관되어진 퀘스트아이템 목록을 얻는다. 
	bool GetQuestItemSet(UINT nQuestID, OUT std::set<int>& questItemSet);

	void LoadQuestCondition(TiXmlElement* pRootElement, UINT nQuestID);
	QuestCondition * GetQuestCondition(UINT nQuestID);

	// Shop
	bool LoadShopData();
	TShopData* GetShopData(int nShopID);
	TShopItem* GetShopItem(int nShopID, int nShopTabID, int nIndex);

	// SkillShop
	bool LoadSkillShopData();
	TSkillShopData* GetSkillShopData(int nShopID);

	// Skill
	bool LoadSkillData();
	TSkillData* GetSkillData(int nSkillID);
	TSkillLevelData* GetSkillLevelData(int nSkillID, char cSkillLevel);
	TSkillTreeData* GetSkillTreeData( int nSkillID );
	int GetNumSkillTreeData() { return(int)m_vlpSkillTreeData.size(); };
	TSkillTreeData* GetSkillTreeDataByIndex( int iIndex );
	int GetSkillDelayTime(int nSkillID, char cSkillLevel);	// skill delaytime = item cooltime
	int GetSkillGlobalCoolTime( int nSkillID );
	int GetSkillGlobalGroupID( int nSkillID ); 
	bool IsSkillNeedJob(int nSkillID, char cSkillLevel, int nJob);
	bool IsSkillLevel(int nSkillID, char cSkillLevel, BYTE cLevel);
	bool IsSkillShopData(int nSkillID, int nJob, BYTE cLevel);	// 스킬상점에서 살 조건이 되는가?
	int GetNeedSkillPoint(int nSkillID, char cSkillLevel);
	bool GetCreateDefaultSkill( char cClassID, int* aDefaultSkills );

	// Enchant
	bool LoadEnchantData();
#ifdef PRE_FIX_MEMOPT_ENCHANT
	bool LoadEnchantMainTable(const DNTableFileFormat& mainTable);
	bool LoadEnchantNeedItemTable(const DNTableFileFormat& needItemTable);
#endif

	bool LoadPotentialData();
	TEnchantData *GetEnchantData( int nEnchantID ) const;
	TEnchantLevelData *GetEnchantLevelData( int nEnchantID, char cLevel );
#ifdef PRE_FIX_MEMOPT_ENCHANT
	const TEnchantNeedItemData* GetEnchantNeedItemData(int nEnchantID, char cLevel) const;
	const TEnchantNeedItemData* GetEnchantNeedItemData(const TEnchantLevelData& levelData) const;
#endif
	int GetEnchantMaxLevel( int nEnchantID );
	TPotentialData *GetPotentialData( int nPotentailID );

	// Item Drop
	bool LoadItemDropData();
	TItemDropData *GetItemDropData( int nDropItemTableID );

	// Appellation
	int LoadAppellationData();
	TAppellationData *GetAppellationData(int nAppellationItemID);
	int GetAppellationArrayIndex(int nAppellationItemID);
	int GetAppellationLevelLimit(int nAppellationItemID);
	void ResetPeriodAppellation(char *Appellation, std::map<int, int> &mapDelAppellation);
	int GetPeriodAppellationArrayID(int nItemID);

	void ResetGuildAppellation(char *Appellation, std::map<int, int> &mapDelAppellation);	
#if defined( PRE_ADD_BESTFRIEND )
	void ResetBestFriendAppellation(char *Appellation, std::map<int, int> &mapDelAppellation);	
#endif

	// Mission
	bool LoadMissionData();
	TMissionData *GetMissionData( int nArrayIndex );
	char GetMissionType(int nArrayIndex);
	int GetMissionMailID(int nArrayIndex);

	bool LoadDailyMissionData();
	TDailyMissionData *GetDailyMissionData( int nItemID );
	int GetDailyMissionRewarePoint(int nMissionID);
	TDailyMissionData *GetWeekendEventMissionData( int nArrayIndex );
	TDailyMissionData *GetGuildWarMissionData( int nArrayIndex );
	void GetRandomGuildMission( int nRandomSeed, int* vGuildMissionID );
	bool LoadGuildWarRewardData();
	TGuildWarRewardData* GetGuildWarRewardData(char cType, char cClass);
	TDailyMissionData *GetGuildCommonMissionData( int nArrayIndex );
	TDailyMissionData *GetWeekendRepeatMissionData( int nArrayIndex );
#if defined(PRE_ADD_ACTIVEMISSION)
	bool LoadActiveMissionData();
	TActiveMissionData* GetActiveMissionData(int nIndex);
	bool LoadActiveSetData();
	TActiveMissionData* GetRandomActiveMission( int nMapIndex, int nRandomSeed );
#endif
#if defined(PRE_ADD_WEEKLYEVENT_MISSION_ENDTIME)
	int GetWeekendMissionCount(eDailyMissionType eMissionType);
#endif	// #if defined(PRE_ADD_WEEKLYEVENT_MISSION_ENDTIME)
#if defined(PRE_ADD_CHAT_MISSION)
	bool LoadTypingData();
	bool CheckTypingData(int nUIStringMid);
#endif
	// CollectionBook
	bool LoadCollectionBookData();
	TCollectionBookData *GetCollectionBookData(int nID);
	const TMapCollectionBook &CollectionBookList() const { return m_MapCollectionBookData; }

	// Tax
	bool LoadTaxData();
	float GetTaxRate(int nTaxType, int nLevel, INT64 nPrice);
	int GetTaxAmount(int nTaxType, int nLevel, INT64 nPrice);
	INT64 CalcTax(int nTaxType, int nLevel, INT64 nPrice);

	// Seal
	bool LoadSealData();
	int GetTotalSealNeeds(int nTypeParam, int nType2, char cItemLevel);
	char GetSealSuccessRate(int nTypeParam, int nType2, char cItemLevel);

	// PCBang
	bool LoadPCBangData();
	int GetPCBangDataCount();
	TPCBangData *GetPCBangData(int nPCBangType);

	int GetPCBangParam1(int nPCBangType, int nPCBangGrade);	// 데이터 1줄만 있는경우 (PCBang::Type::)Exp, Appellation, Fatigue, RebirthCoin, Mission
	int GetPCBangParam2(int nPCBangType, int nPCBangGrade);	// 데이터 1줄만 있는경우 (PCBang::Type::)Exp, Appellation, Fatigue, RebirthCoin, Mission
	int GetPCBangNestClearCount(int nPCBangGrade, int nMapID);
	bool GetPCBangClearBoxFlag(int nPCBangGrade);
	bool CheckPcbangAppellation(int nAppllationIndex);										//피씨방칭호가 맞는지 체크

	// Item Compound
	bool LoadItemCompoundData();
#if defined( _GAMESERVER )
	void EmblemCompound( CMultiRoom* pRoom, int iPlateItemID, CDnItemCompounder::S_ITEM_SETTING_INFO& ItemSetting, CDnItemCompounder::S_OUTPUT* pOutput );
	void ItemCompound( CMultiRoom* pRoom, CDnItemCompounder::S_ITEM_SETTING_INFO& ItemSetting, CDnItemCompounder::S_OUTPUT* pOutput );
#else
	void EmblemCompound( int iPlateItemID, CDnItemCompounder::S_ITEM_SETTING_INFO& ItemSetting, CDnItemCompounder::S_OUTPUT* pOutput );
	void ItemCompound( CDnItemCompounder::S_ITEM_SETTING_INFO& ItemSetting, CDnItemCompounder::S_OUTPUT* pOutput );
#endif // #if defined( _GAMESERVER )
	CDnItemCompounder* GetItemCompounder() { return m_pItemCompounder; }

	bool LoadSkillTreeData();
	CDnSkillTreeSystem* GetSkillTreeSystem() { return m_pSkillTreeSystem; };

	// PvP
	bool LoadPvPMapTable();
	bool LoadPvPGameModeTable();
	bool LoadPvPGameStartConditionTable();
	bool LoadPvPRankTable();
	const TPvPMapTable*	GetPvPMapTable( const int nItemID );
	const TPvPGameModeTable* GetPvPGameModeTable( const int nItemID );
	const TPvPGameStartConditionTable* GetPvPGameStartConditionTable( const UINT uiMaxUser );
	const TPvPRankTable* GetPvPRankTable( const UINT uiLevel );
#ifdef PRE_MOD_PVPRANK
	UINT GetPvPExpThreshold();
	int GetRelativePvPRank(int nExpAbsoluteRank, float fExpRateRank);
#if defined(PRE_ADD_PVP_RANKING)
	void SetRelativePvPRank( BYTE *cPvPLevel, UINT uiExp, int iPvPRank, float fPvPRaito );
#endif
#endif		//#ifdef PRE_MOD_PVPRANK
	int	GetRandomLadderMapIndex( LadderSystem::MatchType::eCode MatchType );

	// CashShop
	bool LoadCashCommodityData(bool bReload = false);

	bool GetCashCommodityData(int nSN, TCashCommodityData &Data);
	int GetCashCommodityPrice(int nSN);
	bool IsReserveCommodity(int nSN);	// 적립금 가능 상품인지
	bool IsPresentCommodity(int nSN);	// 선물 가능 상품인지
	bool IsLimitCommodity(int nSN);		// 한정판매인지
	int GetCashCommodityPeriod(int nSN);
	int GetCashCommodityCount(int nSN);
	bool IsValidCashCommodityItemID(int nItemSN, int nItemID);
#if defined(PRE_ADD_VIP)
	int GetCashCommodityVIPPoint(int nSN);	// vippoint얻기
	bool GetCashCommodityPay(int nSN);
	int GetCashCommodityVIPAutomaticPaySalePrice(int nSN);	// VIP자동결제때 할인가격
	bool IsVIPSell(int nSN);	// VIP 전용샵 아이템
	int GetCashCommodityVIPLevel(int nSN);	// 살수있는 레벨
#endif	// #if defined(PRE_ADD_VIP)
	int GetCashCommodityItem0(int nSN);
	int GetCashCommodityOverlapBuy(int nSN);	// 중복구매가능여부
	int GetCashCommodityItemIDCount(int nSN);	// 옵션아이템 개수
	bool CheckCashSNItemID(int nItemSN, int nItemID);

#if defined(PRE_ADD_CASH_REFUND)
	bool GetCashCommodityNoRefund(int nID, int nSN);		// 캐쉬 환불 불가능여부
#endif
#if defined(PRE_ADD_CASHSHOP_CREDIT)
	bool GetCashCommodityCreditAble(int nSN);
	int GetCashCommodityCreditAbleLevel(int nSN);
#endif	// #if defined(PRE_ADD_CASHSHOP_CREDIT)
#if defined(PRE_ADD_SALE_COUPON)
	bool IsSaleCouponByItem(int nSN);
#endif

#if defined( PRE_ADD_NEW_MONEY_SEED )
	bool IsSeedCommodity(int nSN);	
#endif

	bool LoadCashPackageData(bool bReload = false);
	bool GetCashPackageData(int nSN, TCashPackageData &Data);
	bool IsCashPackageData(int nSN);
	
	bool GetCashCommodityItemSNListByPackage(int nPackageSN, std::vector<DBPacket::TItemSNIDOption> &VecItemSNIDList);
	int GetCashPackageCount(int nPackageSN);

#if defined(PRE_ADD_VIP)
	// 중국 VIP
	bool LoadVIPData();
	TVIPData *GetVIPData();	// 어차피 1개만 있다고 한다 -_-; 
	int GetVIPAutoPayItemSN();
	int GetVIP7DaysLeftMailID();
	int GetVIP1DayLeftMailID();
	int GetVIPCloseMailID();
	int GetVIPShortCashMailID();
#endif	// #if defined(PRE_ADD_VIP)

	// DarkLair
	bool LoadDarkLairMapData();
	const TDLMapData *GetDLMapData( int nItemID );

	bool LoadDarkLairClearData();
	TDLClearDataItem *GetDLClearData( int nItemID, int nLevel, int nRound );

	bool LoadItemOptionTableData();
	const TItemOptionTableData* GetItemOptionTableData( int iItemID );

#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
	bool LoadReputeTableData();
	TReputeTableData* GetReputeTableData( int iItemID );
	bool LoadPresentTableData();
	TPresentTableData* GetPresentTableData( int iItemID );
	bool LoadMailTableData();
	TMailTableData* GetMailTableData( int iItemID );
	bool LoadStoreBenefitData();
	void GetStoreBenefitData( int iNpcID, /*IN OUT*/ vector<TStoreBenefitData*>& vlpDatas );
#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )

	bool LoadCombinedShopTableData();
	bool IsCombinedShop( int iShopID );
	const TCombinedShopTableData*	GetCombinedShopItemTableData( int iShopID, int iTabID, int iListID );

	bool LoadMasterSystemDecreaseTableData();
	TMasterSystemDecreaseTableData* GetMasterSystemDecreaseTableData( int iPupilLevel );
	bool LoadMasterSystemGainTableData();
	TMasterSystemGainTableData*	GetMasterSystemGainTableData( int iLevel, TDUNGEONDIFFICULTY StageDifficulty );
#if defined( PRE_ADD_SECONDARY_SKILL )
	bool LoadSecondarySkillTableData();
	TSecondarySkillTableData* GetSecondarySkillTableData( int iSecondarySkillID );
	bool LoadSecondarySkillLevelTableData();
	TSecondarySkillLevelTableData* GetSecondarySkillLevelTableData( int iSecondarySkillID, SecondarySkill::Grade::eType Grade, int iSkillLevel );
	TSecondarySkillLevelTableData* GetSecondarySkillLevelTableData( int iSecondarySkillID, int iExp );
	bool LoadSecondarySkillRecipeTableData();
	TSecondarySkillRecipeTableData* GetSecondarySkillRecipeTableData( int iItemID );
#endif // #if defined( PRE_ADD_SECONDARY_SKILL )

	bool LoadInstantItemTableData();
	bool bIsInstantItem( int iItemID );

	bool LoadFarmCultivateTableData();
	TFarmCultivateTableData* GetFarmCultivateTableData( int iItemID );

	bool LoadFishingTableData();
	TFishingTableData * GetFishingTableData(int nItemID);
	bool LoadFishingPointTableData();
	TFishingPointTableData * GetFishingPointTableData(int nItemID);
	bool LoadFishingAreaTableData();
	TFishingAreaTableData * GetFishingAreaTableData(int nItemID);
	void GetFishingAreaTableDataByMapID(int nMapID, std::vector <TFishingAreaTableData> &vFishingArea);

	// Gachapon_JP
#ifdef PRE_ADD_GACHA_JAPAN
	bool LoadGachaData_JP();
	TGachaponData_JP *GetGachaponData_JP( int nGachaShopID );
#endif

	bool LoadJobTableData();
	TJobTableData* GetJobTableData( int nJobTableID );
	int GetBaseClass( int iJob );	// 현재 직업의 기본 클래스
	int GetParentJob( int iJob );	// 부모직업 얻어오기
	int GetJobNumber( int iJob );	// 직업의 전직 차수 얻어오기
	int GetClass( int iJob );		// 클래스 얻어오기
	void GetJobGroupID(int nItemID, char cJobCode[]);

	//LevelUp Event
	bool LoadLevelupEvent();
	void GetLevelupEvent(int nLevel, int nClass, int nJob, std::vector<TLevelupEvent*> &VecEventList);
	bool GetLevelupEventbyType(int nLevel, TLevelupEvent::eEventType eType, TLevelupEvent &Event);

	//PromotionData
	bool LoadPromotionData();
	int GetPromotionCount();
	const TPromotionData * GetPromotionByIdx(int nIndex);
	const TPromotionData * GetPromotionByIID(int nID);

	// GlyphSkill
	bool LoadGlyphSkillData();
	TGlyphSkillData *GetGlyphSkillData(int nGlyphID);
	int GetGlyphType(int nGlyphID);

	// GlyphSlot
	bool LoadGlyphSlotData();
	int GLYPHINDEX_2_SLOTINDEX(int nGlyphIndex);
	bool IsGlyphSlotLevel(int nGlyphIndex, int nCharLevel);
	bool IsGlyphLevel(int nGlyphID, int nCharLevel);

	// GlyphCharge
	bool LoadGlyphChargeData();
	int GetGlyphCharge(char cGlyphType, char cGlyphRank);

	// CharmItemKeyTable
	bool LoadCharmItemKeyData();
	TCharmItemKeyData *GetCharmKeyData(int nItemID);

	// CharmItemTable
	bool LoadCharmItemData();
	TCharmItemData *GetCharmItemData(int nCharmNo);

#if defined(PRE_ADD_GETITEM_WITH_RANDOMVALUE)
	// CharmCountTable
	bool LoadCharmCountData();
	TCharmCountData *GetCharmCountData(int nID);
#endif	// #if defined(PRE_ADD_GETITEM_WITH_RANDOMVALUE)

#if defined (PRE_ADD_CHAOSCUBE)
	// ChaosCubeStuffTable
	bool LoadChaosCubeStuffData();
	TChaosStuffItem *GetChaosStuffItemData(int nItemID);

	// ChaosCubeResultTable
	bool LoadChaosCubeResultData();
	TChaosItemData *GetChaosItemData(int nChaosNo);
#endif // #if defined (PRE_ADD_CHAOSCUBE)

	int GetMaxGuildExp();
	int GetMaxMaxGuildLevel();
	bool LoadGuildLevelData();
	TGuildLevel *GetGuildLevelData(int nLevel);

	bool LoadGuildWarPointData();
	TGuildWarPoint *GetGuildWarPointData(int nRank);

	// GuildWareHouse
	bool LoadGuildWareData();
	int GetGuildLimitItem(int nIndex);
	int GetGuildLimitCoin(int nIndex);
	int GetGuildLimitMaxItem();
	int GetGuildLimitMaxCoin();

	// VehicleTable
	bool LoadVehicleData();
	TVehicleData *GetVehicleData(int nItemID);
	int GetVehicleClassID(int nItemID);
	int GetVehicleActorID(int nItemID);
	int GetPetLevelTypeID(int nItemID);

	// VehiclePartsTable
	bool LoadVehiclePartsData();
	TVehiclePartsData *GetVehiclePartsData(int nItemID);
	int GetVehiclePartsType(int nItemID);

	bool LoadPetLevelData();
	TPetLevelData *GetPetLevelData(int nPetLevelType);
	TPetLevelDetail *GetPetLevelDetail(int nPetBodyItemID, int nPetExp);
	int GetPetLevelTableIndex(int nPetBodyItemID, int nPetExp);

	bool LoadPetFoodData();
	TPetFoodData *GetPetFoodData(int nItemID);

	// 금칙어
	unsigned int MemoryToRSHashKey(const wchar_t* pszData, long nSize);
#if defined(PRE_ADD_MULTILANGUAGE)
	bool LoadProhibitWord(int nLanguage, const char * pszFilePath);
	TProhibitWord * GetLanguageSlangFilter(int nLanguage);
	bool CheckProhibitWord(int nLanguage, const wchar_t *pwszWord);
#else		//#if defined(PRE_ADD_MULTILANGUAGE)
	bool LoadProhibitWord();
	bool CheckProhibitWord(const wchar_t *pwszWord);
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)

	bool LoadExchangeTradeData();
	bool GetExchangeGroupID(int nGroupID, char cItemChange[]);
	TExchangeTrade* GetExchageTradeData(int nItemID);

	std::map<LadderSystem::MatchType::eCode,std::vector<int>> m_LadderMatchTypeMapIndex;

	bool LoadPVPMissionRoom();
	TPvPMissionRoom* GetPvPMissionRoom(int nItemID);
	TVecPvPMissionRoom& GetVecPvPMissionRoom();
	bool LoadMonsterLevel();
	bool LoadGhoulModeCondition();
	void GetGhoulGroup(int nUserCount, TMonsterMutationGroup &Group, int &nGhoulCount);

	bool LoadMonsterMutationGroupData();
	bool GetMonsterMutationGroup(int nGroupID, TMonsterMutationGroup &Group);

	bool LoadMonsterMutationData();
	bool GetMonsterMutationData(int nMutationDataID, TMonsterMutationData &Data);
	int	 GetMonsterMutationActorID( int nMutationDataID );
	int	 GetMonsterMutationMoveSpeed( int nMutationDataID );
	bool LoadMonsterMutationSkillData();

#if defined( _GAMESERVER )
	bool LoadPvPGameModeSkillSetting();
	bool GetPvPGameModeSkillSetting( int iGroupID, int iClassID, std::vector<std::pair<int,int>>& vData );
#endif // #if defined( _GAMESERVER )

	bool LoadBattleGroundMode();			//일단은 테이블이 이렇게 설정되어 이름이 이런데 나중에 PvPMode로 통합이 될지도?
	bool GetBattleGroundModeInfo(int nPvPModeID, TBattleGourndModeInfo &ModeInfo);

	bool LoadBattleGroundResourceArea();			//마찮가지로 점령전에 대한 영역설정
	bool GetBattleGroundResourceAreaInfo(int nMapID, int nPvPModeID, std::vector<TPositionAreaInfo> &vList);

	bool LoadBattleGroundSkillSet();
	TBattleGroundSkillInfo * GetBattleGroundSkill(int nSkillID);
	TBattleGroundEffectValue * GetBattleGroundSkillLevel(int nSkillID, int nLevel);

	bool LoadReputeBenefitData();
	TUnionReputeBenefitData* GetUnionReputeBenefitByItemID(int nItemID);

	bool LoadGuildMarkData();
	int  IsValidCompleteMark(int nItemID, int nType);
	bool IsValidGuildMark(int nItemID, bool bCheckCash);

	bool LoadPlayerCommonLeaveTable();
	int GetFatigue10000RatioFromPlayerCommonLevelTable( int iLevel );
	TPlayerCommonLevelTableInfo* GetPlayerCommonLevelTable(int nLevel);
#if defined(PRE_ADD_TALISMAN_SYSTEM)
	int GetTalismanCostFromPlayerCommonLevelTable( int iLevel );
#endif

	bool LoadMasterSysFeelTable();
	int GetMasterSysFeelAddExp(int nFavor);
	bool LoadGlobalEventData();
	TGlobalEventData* GetGlobalEventData(int nScheduleID);

	bool LoadEveryDayEventData();
	TEveryDayEventData* GetEveryDayEventRewardMailID(int iOngoingDay);	
	bool CheckEveryDayEventTime(WORD *index);	//마지막으로 체크한 회차가 일치하는지 체크. 일치하지 않는다면 새로운 이벤트이므로 기존 데이터 리셋

#ifdef PRE_FIX_MEMOPT_EXT
	char* GetFileNameFromFileEXT(DNTableFileFormat* pRawSox, int itemId, const char* pFieldName, DNTableFileFormat* pFileNameSox = NULL);
	void GetFileNameFromFileEXT(std::string& result, DNTableFileFormat* pRawSox, int itemId, const char* pFieldName, DNTableFileFormat* pFileNameSox = NULL);
	void GetFileNameFromFileEXT(std::string& result, DNTableFileFormat* pRawSox, int itemId, int fieldIndex, DNTableFileFormat* pFileNameSox = NULL);
	char* GetFileNameFromFileEXT(int index, DNTableFileFormat* pFileSox = NULL);
#endif

	bool bIsGuildReversionItem( int iItemID );

#if defined( PRE_PARTY_DB )
	bool LoadPartySortWeightTable();
	int GetPartySortWeight( Party::SortWeightIndex::eCode Type );

	bool LoadDungeonSortWeightTable();
	int GetDungeonSortWeight( int iMapindex );
#endif // #if defined( PRE_PARTY_DB )

	bool LoadGuildRewardItemData();
	TGuildRewardItemData* GetGuildRewardItemData(int nItemID);

	vector<int> GetVillageMaps() const { return vector<int>(m_vVillageMaps); }
#if defined( PRE_ADD_SALE_COUPON)
	bool LoadSaleCouponData();
	TSaleCouponData* GetSaleCouponItemData(int nItemID);
	bool IsSaleCouponItemBuyItem(int nCouponItemID, int nBuyItemSN);
#endif // #if defined( PRE_ADD_SALE_COUPON)

#if defined( PRE_ADD_NAMEDITEM_SYSTEM )
	bool LoadNamedItemData();
	int	 GetNamedItemMaxCount(int nItemID);
	int	 GetNamedItemSwapID(int nItemID);
#endif

#ifdef PRE_ADD_EXCHANGE_POTENTIAL
	bool LoadPotentialTransferData();
	int GetPotentialTransConsumptionCount(int nExtractLevel, int nExtractRank, int nInjectLevel, int nInjectRank, int nMainType, int nSubType);
#endif		//#ifdef PRE_ADD_EXCHANGE_POTENTIAL

	bool LoadCashErasableType();
	bool IsCashErasableType(int nItemID);

#if defined( PRE_WORLDCOMBINE_PARTY )
	bool LoadCombinePartyData();	
	WorldCombineParty::WrldCombinePartyData* GetCombinePartyData(int nPrimaryIndex);
	void CheckCombinePartyData(std::map<int,WorldCombineParty::WrldCombinePartyData> ExistCombineParty, std::map<int,WorldCombineParty::WrldCombinePartyData> &CreateCombineParty);	
#endif

#if defined( PRE_ADD_PCBANG_RENTAL_ITEM)
	bool LoadPcCafeRentData();
	bool IsPcCafeRentItem(int nItemID);	
	int GetPcCafeRentItemID(BYTE cLevel, char cClassID, BYTE cJob, BYTE cPcCafeGrade, std::map<int,int> &vList );	
#endif

#if defined(PRE_ADD_INSTANT_CASH_BUY)
	bool LoadCashBuyShortcut();
	TCashBuyShortcutData *GetCashBuyShortcut(int nType);
	bool IsCashBuyShortcut(int nType, int nMapIndex, int nItemSN);
#endif	// #if defined(PRE_ADD_INSTANT_CASH_BUY)

#if defined(PRE_ADD_EXCHANGE_ENCHANT)
	bool LoadEnchantTransferData();
	TExchangeEnchantData* GetExchangeEnchantData(BYTE cRank, BYTE cLevelLimit, BYTE cEnchantLevel);		
#endif

#if defined(PRE_FIX_NEXTSKILLINFO)
	SKILL_LEVEL_INFO* GetSkillLevelTableIDList(int nSkillID, int nApplyType);
#endif	// #if defined(PRE_FIX_NEXTSKILLINFO)

#if defined(PRE_ADD_WEEKLYEVENT)
	bool LoadWeeklyEvent();
	WeeklyEvent::TWeeklyEvent *GetWeeklyEvent(int nDayOfWeek);
	int GetWeeklyEventValue(int nRaceType, int nClassType, int nEventType, int nThreadIndex);
	float GetWeeklyEventValuef(int nRaceType, int nClassType, int nEventType, int nThreadIndex);
	void WeeklyEventClear();	
#endif	// #if defined(PRE_ADD_WEEKLYEVENT)

#if defined( PRE_ADD_TOTAL_LEVEL_SKILL )
	bool LoadTotalLevelSkill();
	int GetTotalLevelSkillType( int nSkillID );
	float GetTotalLevelSkillValue( int nSkillID, TotalLevelSkill::Common::eVillageEffectType eType );
	int GetintTotalLevelSkillValue( int nSkillID, TotalLevelSkill::Common::eVillageEffectType eType );
	bool bIsTotalLevelSkillLimitLevel( int nSlotIndex, int nSkillID, int nTotalLevel, int nCharacterLevel);		
	bool bIsTotalLevelSkillCashSlot( int nSlotIndex );
#endif

#if defined(PRE_SPECIALBOX)
	bool LoadKeepBoxProvideItem();
	TKeepBoxProvideItemData *GetKeepBoxProvideItemData(int nType);
#endif	// #if defined(PRE_SPECIALBOX)

#ifdef PRE_ADD_JOINGUILD_SUPPORT
	bool LoadGuildSupport();
	int GetRewardGuildSupportMailID(int nLevel);
	int GetRewardFisrtGuildJoinMailID(int nLevel);
#endif		//#ifdef PRE_ADD_JOINGUILD_SUPPORT

#if defined(PRE_ADD_REMOTE_QUEST)
	bool LoadRemoteQuestData();
	int GetRemoteQuestDataSize();
	TRemoteQuestData * GetRemoteQuestData( int nArrayIndex );
	TRemoteQuestData * GetRemoteQuestDataByQuestID( int nQuestID );
#endif

#if defined(PRE_ADD_STAGE_CLEAR_ADD_REWARD)
	bool LoadBonusDropTable(void);
	int GetBonusDropGroupID(int nMapID, int nNeedItemID);
#endif	// #if defined(PRE_ADD_STAGE_CLEAR_ADD_REWARD)

#if defined( PRE_WORLDCOMBINE_PVP )
	bool LoadWorldPVPMissionRoom();
	TWorldPvPMissionRoom* GetWorldPvPMissionRoom(int nItemID);
	TVecWorldPvPMissionRoom& GetWorldVecPvPMissionRoom();	
	bool bIsWorldPvPRoomAllowMode( int nMode );
#endif
#if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
	bool LoadDropItemEnchant();
	TItemDropEnchantData* GetDropItemEnchant(int nDropTableID)	;
#endif	// #if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
#if defined( PRE_ALTEIAWORLD_EXPLORE )
	bool LoadAlteiaWorldMapInfo();	
	TAlteiaWorldMapInfo* GetAlteiaWorldMapInfo(int nIndex);
	int GetAlteiaWorldMapMaxCount() { return m_nAlteiaWorldMapMaxCount;}
	bool bIsAlteiaWorldEventTime();
	time_t GetAlteiaWorldEventBeginTime() { return m_tAlteiaWorldBeginTime;}
	time_t GetAlteiaWorldEventEndTime() { return m_tAlteiaWorldEndTime;}
#endif // #if defined( PRE_ALTEIAWORLD_EXPLORE )
#if defined( PRE_ADD_STAMPSYSTEM )
	bool LoadStampTable();
	const StampSystem::TStampTableData * GetStampTableData( INT64 biTime ) const;
#endif // #if defined( PRE_ADD_STAMPSYSTEM )

#if defined(PRE_ADD_TALISMAN_SYSTEM)
	bool LoadTalismanData();
	TTalismanData* GetTalismanData(int nTalismanID);

	bool LoadTalismanSlotData();
	TTalismanSlotData* GetTalismanSlotData(int nIndex);
	float GetTalismanSlotEfficiency(int nIndex);
	bool IsTalismanSlotLevel(int nTalismanIndex, int nCharLevel);	
#endif

#if defined( PRE_DRAGONBUFF )
	void AddWorldBuffData( int nItemID );
	bool bIsWorldBuffData( int nItemID );
#endif

#if defined( PRE_PVP_GAMBLEROOM )
	bool LoadPVPGambleRoomData();
	TPvPMissionRoom* GetPvPGambleRoom(int nItemID);
#endif

#if defined( PRE_ADD_STAGE_WEIGHT )
	bool LoadStageWeightData();
	const TStageWeightData* GetStageWeigthData( int nStageWeightID, DWORD dwUserCount );
#endif // #if defined( PRE_ADD_STAGE_WEIGHT )
	
	TPVPSkills* GetPVPSkillPDamage(int nSkillID, int iSkillLevel);
};

extern CDNGameDataManager* g_pDataManager;
