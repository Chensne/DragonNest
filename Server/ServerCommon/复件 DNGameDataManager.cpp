#include "StdAfx.h"
#include "DNGameDataManager.h"
#include "EtResourceMng.h"
#include "XmlParser.h"
#include "EtUIXML.h"
#include "Log.h"
#include "Util.h"
#include "./boost/lexical_cast.hpp"
#include "DNQuestCondition.h"
#include "DNQuest.h"
#include "DNQuestManager.h"
#include "DnUIString.h"
#include "DnWorldGateStruct.h"
#include "TimeSet.h"
#include "DNMissionSystem.h"
#if defined(_VILLAGESERVER)
#include "DNFieldManager.h"
#include "DNFieldDataManager.h"
#include "DnItemCompounder.h"

extern TVillageConfig g_Config;

#elif defined(_GAMESERVER)
#include "DNDLGameRoom.h"
#include "DnWorld.h"
#include "DNSchedule.h"
extern TGameConfig g_Config;

#endif	// _GAMESERVER

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDNGameDataManager* g_pDataManager = NULL;

CDNGameDataManager::CDNGameDataManager(void)
{
	memset(&m_UpdateLocalTime, 0, sizeof(m_UpdateLocalTime));

	m_pSkillTreeSystem = NULL;

	m_pMapInfo.clear();
	m_pSkillData.clear();
	m_pItemData.clear();
	m_pActorData.clear();
	m_pNpcData.clear();
	m_pWeaponData.clear();
	m_pPartData.clear();
	m_pPotentialJewelData.clear();
	m_pEnchantJewelData.clear();
	m_pSkillShop.clear();
	m_pEnchantTable.clear();
	m_pPotentialTable.clear();
	m_pTaxData.clear();
	m_pPCBangData.clear();
	m_pMapCoinCount.clear();
	// PvP
	m_PvPMapTable.clear();
	m_PvPGameModeTable.clear();
	m_PvPGameStartConditionTable.clear();
	m_PvPRankTable.clear();
	m_pItemCompounder = NULL;

	{
		ScopeLock <CSyncLock> Lock(m_Sync);

		m_pCashCommodityData.clear();
		m_pCashPackageData.clear();
	}

#ifdef PRE_ADD_GACHA_JAPAN
	m_mapGachaData.clear();
#endif // PRE_ADD_GACHA_JAPAN
	m_pVecLevelupEvent.clear();
#if defined(PRE_ADD_VIP)
	m_pVecVIPData.clear();
#endif	// #if defined(PRE_ADD_VIP)
	m_ItemOptionTableData.clear();
#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
	m_ReputeTableData.clear();
	m_PresentTableData.clear();
#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
	m_CombinedShopTableData.clear();
	m_MapGlyphSkill.clear();
	m_VecGlyphSlot.clear();
	m_VecGlyphCharge.clear();
	m_MapCharmKeyItem.clear();
	m_MapCharmItem.clear();

	m_GuildLevel.clear();
	m_nMaxGuildExp = 0;
	m_nMaxGuildLevel = 0;
	m_GuildLimitItem.clear();
	m_GuildLimitCoin.clear();

	m_MapVehicle.clear();
	m_MapVehicleParts.clear();

	m_pPetLevelData.clear();

	m_mPeriodAppellation.clear();

	m_MapCollectionBookData.clear();
	m_MapAppellationData.clear();

#ifdef _WORK
	m_bAllLoaded = false;
#endif		//#ifdef _WORK
	m_nMaxPlayerLevel = 0;
	m_mGuildAppellation.clear();
	m_mGuildRewardItemTableData.clear();
#if defined( PRE_ADD_BESTFRIEND )
	m_mBestFriendAppellation.clear();
#endif
#if defined(PRE_ADD_SALE_COUPON)
	m_mSaleCouponItemTableData.clear();
#endif
	m_MapPetFoodData.clear();
#if defined( PRE_ADD_NAMEDITEM_SYSTEM )
	m_mNamedItemTableData.clear();
#endif
#if defined( PRE_WORLDCOMBINE_PARTY )
	m_mWorldCombinePartyData.clear();
#endif
#if defined(PRE_ADD_PCBANG_RENTAL_ITEM)
	m_mPcBangRentItemData.clear();
#endif
#ifdef PRE_MOD_PVPRANK
	m_nThresholdPvPExpValue = 0;
#endif		//#ifdef PRE_MOD_PVPRANK
#if defined(PRE_ADD_WEEKLYEVENT)
	m_pMapWeeklyEvent.clear();
#endif	// #if defined(PRE_ADD_WEEKLYEVENT)
#if defined( PRE_ADD_TOTAL_LEVEL_SKILL )
	m_MapTotalSkillLevelTabel.clear();
	m_MapTotalSkillSlotTabel.clear();
	m_MapTotalSkillBlow.clear();
#endif
#if defined(PRE_SPECIALBOX)
	m_pMapKeepBoxProvideItemData.clear();
#endif	// #if defined(PRE_SPECIALBOX)
#if defined(PRE_ADD_STAGE_CLEAR_ADD_REWARD)
	m_MapBonusDropTable.clear();
#endif	// #if defined(PRE_ADD_STAGE_CLEAR_ADD_REWARD)
#if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
	m_MapItemDropEnchantTable.clear();
#endif	// #if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
#if defined( PRE_ADD_STAMPSYSTEM )
	m_MapStampTableData.clear();
#endif // #if defined( PRE_ADD_STAMPSYSTEM )
#if defined( PRE_ALTEIAWORLD_EXPLORE )
	m_MapAlteiaWorldMapInfo.clear();
	m_nAlteiaWorldMapMaxCount = 0;
	m_tAlteiaWorldBeginTime = 0;
#endif
#if defined(PRE_ADD_TALISMAN_SYSTEM)
	m_pTalismanSlotTable.clear();
#endif
#if defined( PRE_PVP_GAMBLEROOM )
	m_MapPvPGambleRoom.clear();
#endif
#if defined( PRE_ADD_STAGE_WEIGHT )
	m_MapStageWeightData.clear();
#endif // #if defined( PRE_ADD_STAGE_WEIGHT )
}

CDNGameDataManager::~CDNGameDataManager(void)
{
	SAFE_DELETE_PMAP( TMapItemDropData, m_pItemDropTable );
	SAFE_DELETE_PMAP( TMapInfoMap, m_pMapInfo );
	SAFE_DELETE_PMAP( TMapItemData, m_pItemData );
	SAFE_DELETE_PMAP( TMapActorData, m_pActorData );
	SAFE_DELETE_PMAP( TMapNpcData, m_pNpcData );
	SAFE_DELETE_PMAP( TMapWeaponData, m_pWeaponData );
	SAFE_DELETE_PMAP( TMapPartData, m_pPartData );
	SAFE_DELETE_PMAP( TMapPotentialJewelData, m_pPotentialJewelData );
	SAFE_DELETE_PMAP( TMapEnchantJewelData, m_pEnchantJewelData );
	SAFE_DELETE_PMAP( TMapShopData, m_pShopData );
	SAFE_DELETE_PMAP( TMapSkillData, m_pSkillData );
	SAFE_DELETE_PMAP( TMapSkillShopData, m_pSkillShop );
	SAFE_DELETE_PMAP( TMapSkillTreeData, m_pSkillTreeData );
	SAFE_DELETE_PMAP( TPvPMapTableData, m_PvPMapTable );
	SAFE_DELETE_PMAP( TPvPGameModeTableData, m_PvPGameModeTable );
	SAFE_DELETE_PMAP( TPvPGameStartConditionData, m_PvPGameStartConditionTable );
	SAFE_DELETE_PMAP( TPvPRankTableData, m_PvPRankTable );
	SAFE_DELETE_PVEC( m_pMissionTable );
	SAFE_DELETE_PMAP( TMapDailyMissionData, m_pDailyMissionTable );
	SAFE_DELETE_PVEC( m_pTaxData );
	SAFE_DELETE_PMAP( TMapPCBangData, m_pPCBangData );
	SAFE_DELETE_PMAP( TMapCoinCount, m_pMapCoinCount );
	SAFE_DELETE_PMAP( TMapQuestConditionData, m_QuestConditionData );

	SAFE_DELETE_PMAPF( TMapEnchantData, m_pEnchantTable, SAFE_DELETE_PMAP( TMapEnchantLevelData, TMapEnchantData_iter->second->pMapLevelData ) );
	SAFE_DELETE_PMAPF( TMapPotentialData, m_pPotentialTable, SAFE_DELETE_PVEC( TMapPotentialData_iter->second->pVecItemData ) );
	SAFE_DELETE_PVEC( m_pGuildWarRewardData );

	SAFE_DELETE( m_pItemCompounder );
	SAFE_DELETE( m_pSkillTreeSystem );

	{
		ScopeLock <CSyncLock> Lock(m_Sync);

		SAFE_DELETE_PMAP(TMapCashCommodityData, m_pCashCommodityData);
		SAFE_DELETE_PMAP(TMapCashPackageData, m_pCashPackageData);
	}

#ifdef PRE_ADD_GACHA_JAPAN
	SAFE_DELETE_PMAP( TMapGachaData_JP, m_mapGachaData );
#endif
	SAFE_DELETE_PVEC(m_pVecLevelupEvent);
#if defined(PRE_ADD_VIP)
	SAFE_DELETE_PVEC(m_pVecVIPData);
#endif	// #if defined(PRE_ADD_VIP)
	SAFE_DELETE_PVEC( m_pSealCountData );
	SAFE_DELETE_PVEC( m_vLvPromotion );
	SAFE_DELETE_PMAP( TMapDLMapData, m_pDLMapTable );
	SAFE_DELETE_PMAPF( TMapDLClearData, m_pDLClearTable, SAFE_DELETE_PVEC( TMapDLClearData_iter->second->pVecClearItem ) );
	SAFE_DELETE_PMAP(TMapPetLevelData, m_pPetLevelData);	
#if defined(PRE_ADD_MULTILANGUAGE)
	for (std::map <int, TProhibitWord>::iterator ii = m_ProhibitWordList.begin(); ii != m_ProhibitWordList.end(); ii++)
		(*ii).second.clear();
	m_ProhibitWordList.clear();
#else		//#if defined(PRE_ADD_MULTILANGUAGE)
	m_ProhibitWordList.clear();
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)
#if defined(PRE_ADD_WEEKLYEVENT)
	SAFE_DELETE_PMAP( TMapWeeklyEvent, m_pMapWeeklyEvent );
#endif	// #if defined(PRE_ADD_WEEKLYEVENT)
#if defined(PRE_ADD_REMOTE_QUEST)
	SAFE_DELETE_PVEC(m_pQuestRemoteTable);
	SAFE_DELETE_MAP(m_MapRemoteQuestID);
#endif
#if defined(PRE_SPECIALBOX)
	SAFE_DELETE_PMAP(TMapKeepBoxProvideItemData, m_pMapKeepBoxProvideItemData);
#endif	// #if defined(PRE_SPECIALBOX)
#if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
	SAFE_DELETE_MAP(m_MapItemDropEnchantTable);
#endif	// #if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
#if defined(PRE_ADD_ACTIVEMISSION)
	SAFE_DELETE_PVEC( m_pActiveMissionTable );
#endif
#if defined(PRE_ADD_TALISMAN_SYSTEM)
	SAFE_DELETE_PVEC( m_pTalismanSlotTable );
#endif
}

bool CDNGameDataManager::AllLoad()
{
	int iLoadFailCount = 0;

	if( !LoadMapInfo() )
		++iLoadFailCount;

	if( !LoadSkillData() )
		++iLoadFailCount;

	if( !LoadItemData() )
		++iLoadFailCount;

	if( !LoadActorData() )
		++iLoadFailCount;

	if( !LoadNpcData() )		
		++iLoadFailCount;

	if( !LoadWeaponData() )		
		++iLoadFailCount;

	if( !LoadPartData() )		
		++iLoadFailCount;

	if( !LoadPotentialJewelData() )
		++iLoadFailCount;

	if( !LoadTalkData() )		
		++iLoadFailCount;

	if( !LoadShopData() )		
		++iLoadFailCount;

	if( !LoadSkillShopData() )	
		++iLoadFailCount;

	if( !LoadQuestReward() )
		++iLoadFailCount;

	if( !LoadEnchantData() )
		++iLoadFailCount;

	if( !LoadPotentialData() )
		++iLoadFailCount;

	if( !LoadItemDropData() )
		++iLoadFailCount;
#if defined(PRE_ADD_STAGE_CLEAR_ADD_REWARD)
	if( !LoadBonusDropTable() )
		++iLoadFailCount;
#endif	// #if defined(PRE_ADD_STAGE_CLEAR_ADD_REWARD)
	if( !LoadAppellationData() )
		++iLoadFailCount;

	if( !LoadMissionData() )
		++iLoadFailCount;

	if( !LoadDailyMissionData() )
		++iLoadFailCount;

#if defined(PRE_ADD_ACTIVEMISSION)
	if( !LoadActiveMissionData() )
		++iLoadFailCount;

	if( !LoadActiveSetData() )
		++iLoadFailCount;
#endif

#if defined(PRE_ADD_CHAT_MISSION)
	if( !LoadTypingData() )
		++iLoadFailCount;
#endif

	if( !LoadTaxData() )
		++iLoadFailCount;

	if( !LoadPCBangData() )
		++iLoadFailCount;

	if( !LoadCoinCount() )
		++iLoadFailCount;

	if( !LoadSealData() )
		++iLoadFailCount;

	if( !LoadItemCompoundData() )
		++iLoadFailCount;

	if( !LoadSkillTreeData() )
		++iLoadFailCount;

	// PvP
	if( !LoadPvPMapTable() )
		++iLoadFailCount;
	if( !LoadPvPGameModeTable() )
		++iLoadFailCount;
	if( !LoadPvPGameStartConditionTable() )
		++iLoadFailCount;
	if( !LoadPvPRankTable() )
		++iLoadFailCount;
	//Channel & Merit
	if(!LoadChannelInfo())
		++iLoadFailCount;
	if( !LoadCashCommodityData() )
		++iLoadFailCount;
	if( !LoadCashPackageData() )
		++iLoadFailCount;
	if( !LoadDarkLairMapData() )
		++iLoadFailCount;
	if( !LoadDarkLairClearData() )
		++iLoadFailCount;

#ifdef PRE_ADD_GACHA_JAPAN
	if( !LoadGachaData_JP() )
		++iLoadFailCount;
#endif // PRE_ADD_GACHA_JAPAN

	if( !LoadLevelupEvent() )
		++iLoadFailCount;

	if(LoadPromotionData() == false)
		++iLoadFailCount;

#if defined(PRE_ADD_VIP)
	if( !LoadVIPData() )
		++iLoadFailCount;
#endif	// #if defined(PRE_ADD_VIP)
	if( !LoadItemOptionTableData() )
		++iLoadFailCount;
#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
	if( !LoadReputeTableData() )
		++iLoadFailCount;
	if( !LoadPresentTableData() )
		++iLoadFailCount;
	if( !LoadMailTableData() )
		++iLoadFailCount;
	if( !LoadStoreBenefitData() )
		++iLoadFailCount;
#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
	if( !LoadCombinedShopTableData() )
		++iLoadFailCount;

#ifdef _WORK
	if(!m_CosMixDataMgr.LoadData(m_bAllLoaded))
#else		//#ifdef _WORK
	if(!m_CosMixDataMgr.LoadData())
#endif		//#ifdef _WORK
		++iLoadFailCount;

#ifdef PRE_ADD_COSRANDMIX
	if (!LoadCostumeRandomMixData())
		++iLoadFailCount;
#endif
	if( !LoadMasterSystemDecreaseTableData() )
		++iLoadFailCount;
	if( !LoadMasterSystemGainTableData() )
		++iLoadFailCount;
#if defined( PRE_ADD_SECONDARY_SKILL )
	if( !LoadSecondarySkillTableData() )
		++iLoadFailCount;
	if( !LoadSecondarySkillLevelTableData() )
		++iLoadFailCount;
	if( !LoadSecondarySkillRecipeTableData() )
		++iLoadFailCount;
#endif // #if defined( PRE_ADD_SECONDARY_SKILL )
	if( !LoadInstantItemTableData() )
		++iLoadFailCount;
	if( !LoadFarmCultivateTableData() )
		++iLoadFailCount;
	if(!LoadFishingTableData())
		++iLoadFailCount;
	if(!LoadFishingPointTableData())
		++iLoadFailCount;
	if(!LoadFishingAreaTableData())
		++iLoadFailCount;
	if( !LoadJobTableData() )
		++iLoadFailCount;
	if (!LoadGlyphSkillData())
		++iLoadFailCount;
	if (!LoadGlyphSlotData())
		++iLoadFailCount;
	if (!LoadGlyphChargeData())
		++iLoadFailCount;
	if (!LoadCharmItemData())
		++iLoadFailCount;
	if (!LoadCharmItemKeyData())
		++iLoadFailCount;
#if defined(PRE_ADD_GETITEM_WITH_RANDOMVALUE)
	if (!LoadCharmCountData())
		++iLoadFailCount;
#endif	// #if defined(PRE_ADD_GETITEM_WITH_RANDOMVALUE)
	if (!LoadVehicleData())
		++iLoadFailCount;
	if (!LoadVehiclePartsData())
		++iLoadFailCount;
	if (!LoadPetLevelData())
		++iLoadFailCount;
	if (!LoadPetFoodData())
		++iLoadFailCount;

#if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
	if(!LoadDropItemEnchant())
		++iLoadFailCount;
#endif	// #if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)

	if (!LoadGuildWareData())
		++iLoadFailCount;

#if defined(PRE_ADD_MULTILANGUAGE)
	CLfhHeap::GetInstance()->InitPool();

	std::string strFilePath;
	for (int i = 0; i < MultiLanguage::SupportLanguage::NationMax; i++)
	{
		strFilePath.clear();
		strFilePath = g_Config.szResourcePath;
		strFilePath.append("/Resource/UIString/ProhibitWord");
		if (i != 0)		//일단 0번은 디폴트
			strFilePath.append(MultiLanguage::NationString[i]);
		strFilePath.append(".xml");
		LoadProhibitWord(i, strFilePath.c_str());
	}
#else		//#if defined(PRE_ADD_MULTILANGUAGE)
	CLfhHeap::GetInstance()->InitPool();
	if (!LoadProhibitWord())
		++iLoadFailCount;
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)	

	if (!LoadExchangeTradeData())
		++iLoadFailCount;

	if (!LoadCollectionBookData())
		++iLoadFailCount;

	if (!LoadEnchantJewelData())
		++iLoadFailCount;
	if (!LoadPVPMissionRoom())
		++iLoadFailCount;

	if (!LoadMonsterMutationGroupData())
		++iLoadFailCount;

	if (!LoadMonsterMutationData())
		++iLoadFailCount;

	if (!LoadMonsterMutationSkillData())
		++iLoadFailCount;

	if (!LoadGhoulModeCondition())
		++iLoadFailCount;

#if defined( _GAMESERVER )
	if( !LoadPvPGameModeSkillSetting() )
		++iLoadFailCount;
#endif // #if defined( _GAMESERVER )

#if defined(_GAMESERVER)
	if(!LoadBattleGroundMode())
		++iLoadFailCount;

	if(!LoadBattleGroundResourceArea())
		++iLoadFailCount;

	if(!LoadBattleGroundSkillSet())
		++iLoadFailCount;
#endif	//#if defined(_GAMESERVER)
	if(!LoadGuildWarRewardData())
		++iLoadFailCount;
	if(!LoadReputeBenefitData())
		++iLoadFailCount;

	if(!LoadGuildMarkData())
		++iLoadFailCount;

	if( !LoadPlayerCommonLeaveTable() )
		++iLoadFailCount;

	if (!LoadMonsterLevel())
		++iLoadFailCount;
	if( !LoadMasterSysFeelTable() )
		++iLoadFailCount;
	if( !LoadGlobalEventData() )
		++iLoadFailCount;

	if( !LoadEveryDayEventData() )
		++iLoadFailCount;

	if( !LoadPlayerCustomEventUI() )
		++iLoadFailCount;

	if( !LoadGuildLevelData() )
		++iLoadFailCount;
	if( !LoadGuildWarPointData() )
		++iLoadFailCount;
	if( !LoadGuildRewardItemData() )
		++iLoadFailCount;

	if (!LoadQuestLevelCapReward())
		++iLoadFailCount;
#if defined(PRE_ADD_SALE_COUPON)
	if(!LoadSaleCouponData())
		++iLoadFailCount;
#endif // #if defined(PRE_ADD_SALE_COUPON)

#if defined( PRE_ADD_NAMEDITEM_SYSTEM )
	if( !LoadNamedItemData() )
		++iLoadFailCount;
#endif

#ifdef PRE_ADD_EXCHANGE_POTENTIAL
	if (!LoadPotentialTransferData())
		++iLoadFailCount;
#endif		//#ifdef PRE_ADD_EXCHANGE_POTENTIAL

#if defined( PRE_PARTY_DB )
	if( LoadPartySortWeightTable() == false )
		++iLoadFailCount;
	if( LoadDungeonSortWeightTable() == false )
		++iLoadFailCount;
#endif // #if defined( PRE_PARTY_DB )
	if( LoadCashErasableType() == false )
		++iLoadFailCount;
#if defined (PRE_ADD_CHAOSCUBE)
	if( LoadChaosCubeStuffData() == false )
		++iLoadFailCount;
	if( LoadChaosCubeResultData() == false )
		++iLoadFailCount;
#endif  // #if defined (PRE_ADD_CHAOSCUBE)
#if defined( PRE_WORLDCOMBINE_PARTY )
	if( LoadCombinePartyData() == false )
		++iLoadFailCount;
#endif
#if defined( PRE_ADD_PCBANG_RENTAL_ITEM )
	if( LoadPcCafeRentData() == false )
		++iLoadFailCount;
#endif
#if defined(PRE_ADD_INSTANT_CASH_BUY)
	if( LoadCashBuyShortcut() == false )
		++iLoadFailCount;
#endif	// #if defined(PRE_ADD_INSTANT_CASH_BUY)
#if defined(PRE_ADD_EXCHANGE_ENCHANT)
	if( LoadEnchantTransferData() == false )
		++iLoadFailCount;
#endif
#if defined(PRE_ADD_WEEKLYEVENT)
	if( LoadWeeklyEvent() == false )
		++iLoadFailCount;
#endif	// #if defined(PRE_ADD_WEEKLYEVENT)
#if defined( PRE_ADD_TOTAL_LEVEL_SKILL )
	if( LoadTotalLevelSkill() == false)
		++iLoadFailCount;
#endif

#ifdef PRE_ADD_JOINGUILD_SUPPORT
	if (LoadGuildSupport() == false)
		++iLoadFailCount;
#endif		//#ifdef PRE_ADD_JOINGUILD_SUPPORT

#if defined(PRE_ADD_REMOTE_QUEST)
	if( LoadRemoteQuestData() == false)
		++iLoadFailCount;
#endif
#if defined(PRE_SPECIALBOX)
	if( LoadKeepBoxProvideItem() == false )
		++iLoadFailCount;
#endif	// #if defined(PRE_SPECIALBOX)

#if defined( PRE_WORLDCOMBINE_PVP )
	if( LoadWorldPVPMissionRoom() == false )
		++iLoadFailCount;
#endif

#if defined( PRE_ALTEIAWORLD_EXPLORE )
	if( LoadAlteiaWorldMapInfo() == false )
		++iLoadFailCount;
#endif // #if defined( PRE_ALTEIAWORLD_EXPLORE )

#if defined( PRE_ADD_STAMPSYSTEM )
	if( LoadStampTable() == false )
		++ iLoadFailCount;
#endif // #if defined( PRE_ADD_STAMPSYSTEM )

#if defined( PRE_ADD_TALISMAN_SYSTEM )
	if( LoadTalismanSlotData() == false )
		++ iLoadFailCount;
#endif // #if defined( PRE_ADD_TALISMAN_SYSTEM )

#if defined( PRE_PVP_GAMBLEROOM )
	if( LoadPVPGambleRoomData() == false )
		++ iLoadFailCount;
#endif

#if defined( PRE_ADD_STAGE_WEIGHT )
	if( LoadStageWeightData() == false )
		++ iLoadFailCount;
#endif // #if defined( PRE_ADD_STAGE_WEIGHT )

	if( iLoadFailCount != 0 )
		return false;

#if defined(PRE_FIX_NEXTSKILLINFO)
	InitSkillLevelTableIDList();
#endif // PRE_FIX_NEXTSKILLINFO

#ifdef _WORK
	if (m_bAllLoaded)
		g_Log.Log(LogType::_FILELOG, L"Reloadext complete\n");
	m_bAllLoaded = true;
#endif		//#ifdef _WORK
	return true;
}

void CDNGameDataManager::DoUpdate(DWORD dwCurTick, int nThreadIndex)
{	
	if (nThreadIndex >= 30) return;

#if defined(PRE_ADD_WEEKLYEVENT)
	if (m_UpdateLocalTime[nThreadIndex].dwUpdateTick + 10*1000 <= dwCurTick){ // 10초 마다 한번씩..
		m_UpdateLocalTime[nThreadIndex].dwUpdateTick = dwCurTick;

		time_t Time;
		time(&Time);
		localtime_s(&m_UpdateLocalTime[nThreadIndex].pCurLocalTime, &Time);
	}
#endif	// #if defined(PRE_ADD_WEEKLYEVENT)
}

//---------------------------------------------------------------------------------
// Map(MapTable.dnt - TMapInfo)
//---------------------------------------------------------------------------------

bool CDNGameDataManager::LoadMapInfo()
{
#ifdef _WORK
	if (m_bAllLoaded)		//작업은 되어 있으나 맵관련은 리로드 하지 않습니다. 필드나 게임룸 이미 생성되어진 상태라서 꼭 필요하다면 추가 작업이 필요해보임
		return true;

	DNTableFileFormat *pSox;
	if (m_bAllLoaded)
		pSox = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TMAP );
	else
		pSox = GetDNTable( CDnTableDB::TMAP );
#else		//#ifdef _WORK
	DNTableFileFormat *pSox = GetDNTable( CDnTableDB::TMAP );
#endif		//#ifdef _WORK
	if (!pSox)
	{
		g_Log.Log( LogType::_FILELOG, L"MapTable.dnt failed\r\n" );
		return false;
	}

	if (pSox->GetItemCount() <= 0)
	{
		g_Log.Log( LogType::_FILELOG, L"MapTable.dnt Count(%d)\r\n", pSox->GetItemCount());
		return false;
	}

	char szTemp[256] = { 0, };

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{
		int	_MapType;
		int	_MapSubType;
		int	_WorldLevel;
		std::vector<int> _ToolName;
		std::vector<int> _GateMapIndex;
		std::vector<int> _GateStartGate;
		int	_MaxClearCount;
		int	_AllowMapType;
		int _MapAreaIndex;
		int	_Expandable;
		int	_UserReturnSystem;
		int	_VehicleMode;
		int	_PetMode;
		int	_AllowFreePass;
		int _IncludeBuild;
#if defined( PRE_PARTY_DB )
		int _MapNameID;
		int _EnterConditionTable;
#endif // #if defined( PRE_PARTY_DB )
#if defined( PRE_NORMALSTAGE_REGULATION )
		int _RevisionNum;
#endif
#if defined(PRE_ADD_VEHICLE_SPECIAL_ACTION)
		bool _VehicleSpecialActionMode;
#endif
	};
	
	TempFieldNum sFieldNum;
	sFieldNum._MapType		= pSox->GetFieldNum( "_MapType" );
	sFieldNum._MapSubType	= pSox->GetFieldNum( "_MapSubType" );
	sFieldNum._WorldLevel	= pSox->GetFieldNum( "_WorldLevel" );
	sFieldNum._AllowMapType	= pSox->GetFieldNum( "_AllowMapType" );
	sFieldNum._ToolName.reserve( 10 );
	for( int j=0 ; j<10 ; ++j ) 
	{
		sprintf_s( szTemp, "_ToolName%d", j+1 );
		sFieldNum._ToolName.push_back( pSox->GetFieldNum( szTemp ) );
	}

	sFieldNum._GateMapIndex.reserve(WORLDMAP_GATECOUNT);
	sFieldNum._GateStartGate.reserve(WORLDMAP_GATECOUNT);
	for( int j=0 ; j<WORLDMAP_GATECOUNT ; ++j ) 
	{
		sprintf_s( szTemp, "_Gate%d_MapIndex_txt", j+1 );
		sFieldNum._GateMapIndex.push_back( pSox->GetFieldNum( szTemp) );
		sprintf_s( szTemp, "_Gate%d_StartGate_txt", j+1);
		sFieldNum._GateStartGate.push_back( pSox->GetFieldNum( szTemp) );
	}
	sFieldNum._MaxClearCount = pSox->GetFieldNum("_MaxClearCount");
	sFieldNum._MapAreaIndex = pSox->GetFieldNum("_MapArea");
	sFieldNum._Expandable = pSox->GetFieldNum("_Expandable");
	sFieldNum._UserReturnSystem	= pSox->GetFieldNum( "_UserReturnSystem" );
	sFieldNum._VehicleMode = pSox->GetFieldNum("_VehicleMode");
	sFieldNum._PetMode = pSox->GetFieldNum("_PetMode");
	sFieldNum._AllowFreePass = pSox->GetFieldNum("_AllowFreePass");
	sFieldNum._IncludeBuild = pSox->GetFieldNum("_IncludeBuild");
#if defined( PRE_PARTY_DB )
	sFieldNum._MapNameID = pSox->GetFieldNum("_MapNameID");
	sFieldNum._EnterConditionTable = pSox->GetFieldNum("_EnterConditionTableID");
#endif // #if defined( PRE_PARTY_DB )
#if defined( PRE_NORMALSTAGE_REGULATION )
	sFieldNum._RevisionNum = pSox->GetFieldNum("_RevisionNum");
#endif
#if defined(PRE_ADD_VEHICLE_SPECIAL_ACTION)
	sFieldNum._VehicleSpecialActionMode = pSox->GetFieldNum("_USEVehicleSpecialmotion") ? true : false;
#endif

	//##################################################################
	// Load
	//##################################################################

#ifdef _WORK
	DNTableFileFormat * pGateSox;
	if (m_bAllLoaded)
		pGateSox = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TMAPGATE );
	else
		pGateSox = GetDNTable( CDnTableDB::TMAPGATE );
#else		//#ifdef _WORK
	DNTableFileFormat * pGateSox = GetDNTable( CDnTableDB::TMAPGATE );
#endif		//#ifdef _WORK
	if (!pGateSox)
	{
		g_Log.Log( LogType::_FILELOG, L"MapGateTable.dnt failed\r\n" );
		return false;
	}

	if (pGateSox->GetItemCount() <= 0)
	{
		g_Log.Log( LogType::_FILELOG, L"MapGateTable.dnt Count(%d)\r\n", pGateSox->GetItemCount());
		return false;
	}	

#ifdef _WORK
	if (m_bAllLoaded)
	{
		SAFE_DELETE_PMAP(TMapInfoMap, m_pMapInfo);
		m_pMapInfo.clear();
		m_vVillagePermitLevel.clear();
		m_vPermitPartyCount.clear();
		m_vVillageMaps.clear();
	}
#endif		//#ifdef _WORK

#if defined(_GAMESERVER)
	std::vector <std::string> vFailMap;
	MAGAReqRoomID Packet;
	memset( &Packet, 0, sizeof(Packet) );
	CDNGameRoom* pRoom	= new CDNGameRoom( NULL, 0, &Packet );
	CDnWorld* pCheckWorld = new CDnWorld(pRoom);
#endif	// #if defined(_GAMESERVER)
	bool IsIncludeBuild = false;

	for( int i=0 ; i<pSox->GetItemCount() ; i++ )
	{
		int nItemID = pSox->GetItemID(i);
		TMapInfo* pMapInfo = new TMapInfo;
		memset(pMapInfo, 0, sizeof(TMapInfo));

		int iIdx = pSox->GetIDXprimary( nItemID );

		pMapInfo->nMapID		= nItemID;
		pMapInfo->MapType		= (GlobalEnum::eMapTypeEnum)pSox->GetFieldFromLablePtr( iIdx, sFieldNum._MapType )->GetInteger();
		pMapInfo->MapSubType	= (GlobalEnum::eMapSubTypeEnum)pSox->GetFieldFromLablePtr( iIdx, sFieldNum._MapSubType )->GetInteger();
		pMapInfo->nWorldLevel	= pSox->GetFieldFromLablePtr( iIdx, sFieldNum._WorldLevel )->GetInteger();
		pMapInfo->nAllowMapType	= pSox->GetFieldFromLablePtr( iIdx, sFieldNum._AllowMapType )->GetInteger();
		IsIncludeBuild			= pSox->GetFieldFromLablePtr( iIdx, sFieldNum._IncludeBuild )->GetInteger() ? true : false;
#if defined( PRE_PARTY_DB )
		pMapInfo->wstrMapName = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, pSox->GetFieldFromLablePtr( iIdx, sFieldNum._MapNameID )->GetInteger() );
		pMapInfo->iEnterConditionTable = pSox->GetFieldFromLablePtr( iIdx, sFieldNum._EnterConditionTable )->GetInteger();
#endif // #if defined( PRE_PARTY_DB )
#if defined( PRE_NORMALSTAGE_REGULATION )
		pMapInfo->cRevisionNum = (char)pSox->GetFieldFromLablePtr( iIdx, sFieldNum._RevisionNum )->GetInteger();
#endif

#if defined( _GAMESERVER )
		if ( pMapInfo->nMapID >= DarkLair::TopFloorAbstractMapIndex )
		{
			g_Log.Log(LogType::_FILELOG, L"MapID:%d overflow TopFloorAbstractMapIndex!\r\n", pMapInfo->nMapID );
			delete pMapInfo;
			return false;
		}
#endif // #if defined( _GAMESERVER )

		for( int j=0 ; j<10 ; ++j ) 
		{
			_strcpy(pMapInfo->szMapName[j], _countof(pMapInfo->szMapName[j]), pSox->GetFieldFromLablePtr( iIdx, sFieldNum._ToolName[j] )->GetString(), (int)strlen(pSox->GetFieldFromLablePtr( iIdx, sFieldNum._ToolName[j] )->GetString()) );
			if ( strlen( pMapInfo->szMapName[j] ) == 0 ) 
				continue;

#if defined(_VILLAGESERVER)
			CDNFieldData *pFieldData = g_pFieldDataManager->AddFieldData(pMapInfo->szMapName[j], pMapInfo->nMapID);
			if (!pFieldData)
			{
				g_Log.Log(LogType::_FILELOG, L"MapID:%d, %S, FieldData NULL!\r\n", pMapInfo->nMapID, pMapInfo->szMapName[j]);
				continue;
			}
#endif

#if defined(_GAMESERVER)
			if (IsIncludeBuild)
			{
				bool bResult = pCheckWorld->Initialize( CEtResourceMng::GetInstance().GetFullPath( "MapData" ).c_str(), pMapInfo->szMapName[j]);
				if (!bResult)
				{
					g_Log.Log(LogType::_FILELOG, L"[Map] Grid:%S does not existed \r\n", pMapInfo->szMapName[j] );
					vFailMap.push_back(pMapInfo->szMapName[j]);
				}
			}
#endif	// #if defined(_GAMESERVER)
		}

		for( int j=0 ; j<WORLDMAP_GATECOUNT ; ++j ) 
		{
			char* pszGateMapIndexs = pSox->GetFieldFromLablePtr( iIdx, sFieldNum._GateMapIndex[j] )->GetString();
			std::vector<string> vGateMapIndexs;
			TokenizeA(pszGateMapIndexs, vGateMapIndexs, ";");

			char* pszGateStartIndexs = pSox->GetFieldFromLablePtr( iIdx, sFieldNum._GateStartGate[j] )->GetString();
			std::vector<string> vGateStartIndexs;
			TokenizeA(pszGateStartIndexs, vGateStartIndexs, ";");

			for (int k=0; k<(int)vGateMapIndexs.size(); k++)
			{
				if (k < WORLDMAP_GATECOUNT)
				{
					pMapInfo->GateMapIndexs[j][k] = (USHORT)atoi(vGateMapIndexs[k].c_str());
					pMapInfo->GateStartIndexs[j][k] = (BYTE)atoi(vGateStartIndexs[k].c_str());
					pMapInfo->GateMapCount[j] = k+1;
				}
			}
		}

		pMapInfo->nMaxClearCount = pSox->GetFieldFromLablePtr( iIdx, sFieldNum._MaxClearCount )->GetInteger();

		std::string areaString = pSox->GetFieldFromLablePtr( iIdx, sFieldNum._MapAreaIndex )->GetString();
		if (areaString.empty() == false)
		{
			std::vector<std::string> tokens;
			TokenizeA(areaString, tokens, ";");

			std::vector<std::string>::const_iterator iter = tokens.begin();
			for(; iter != tokens.end(); ++iter)
			{
				const std::string& MapIDString = (*iter);
				pMapInfo->vMapAreaIndex.push_back(atoi(MapIDString.c_str()));
			}
		}
		pMapInfo->bExpandable = pSox->GetFieldFromLablePtr( iIdx, sFieldNum._Expandable )->GetInteger() ? true : false;
		pMapInfo->bUserReturnSystem = pSox->GetFieldFromLablePtr( iIdx, sFieldNum._UserReturnSystem )->GetInteger() ? true : false;
		pMapInfo->bVehicleMode = pSox->GetFieldFromLablePtr( iIdx, sFieldNum._VehicleMode )->GetInteger() ? true : false;
		pMapInfo->bPetMode = pSox->GetFieldFromLablePtr( iIdx, sFieldNum._PetMode )->GetInteger() ? true : false;
		pMapInfo->bAllowFreePass = pSox->GetFieldFromLablePtr(iIdx, sFieldNum._AllowFreePass)->GetInteger() ? true : false;
#if defined(PRE_ADD_VEHICLE_SPECIAL_ACTION)
		pMapInfo->bVehicleSpecalActionMode = pSox->GetFieldFromLablePtr( iIdx, sFieldNum._VehicleSpecialActionMode )->GetInteger() ? true : false;
#endif

		if (!(m_pMapInfo.insert(make_pair(pMapInfo->nMapID, pMapInfo)).second))
			delete pMapInfo;
		else
		{
			if (pMapInfo->MapType == GlobalEnum::MAP_VILLAGE)
			{
				std::vector<int> nVecItemList;
				pGateSox->GetItemIDListFromField( "_MapIndex", pMapInfo->nMapID, nVecItemList );

				if (!nVecItemList.empty())
				{
					std::vector<int>::iterator ii;
					for(ii = nVecItemList.begin(); ii != nVecItemList.end(); ii++)
					{
						bool bCheck = false;
						TVecVillagePermitLevel::iterator verifyItor;
						for(verifyItor = m_vVillagePermitLevel.begin(); verifyItor != m_vVillagePermitLevel.end(); verifyItor++)
						{
							if ((*verifyItor).first == pMapInfo->nMapID)
								bCheck = true;
						}
						
						if (bCheck == false)
						{
							int nPermitLevel = pGateSox->GetFieldFromLablePtr((*ii), "_PermitPlayerLevel" )->GetInteger();
							m_vVillagePermitLevel.push_back(std::make_pair(pMapInfo->nMapID, nPermitLevel));
						}
					}
				}

				if (pSox->GetFieldFromLablePtr(pMapInfo->nMapID, "_IncludeBuild")->GetInteger() == 1)
					m_vVillageMaps.push_back(pMapInfo->nMapID);
			}

			std::vector<int> nVecItemList;
			pGateSox->GetItemIDListFromField( "_MapIndex", pMapInfo->nMapID, nVecItemList );

			if (!nVecItemList.empty())
			{
				std::vector<int>::iterator ii;
				for(ii = nVecItemList.begin(); ii != nVecItemList.end(); ii++)
				{
					bool bCheck = false;
					TVecPermitPartyCount::iterator verifyItor;
					for(verifyItor = m_vPermitPartyCount.begin(); verifyItor != m_vPermitPartyCount.end(); verifyItor++)
					{
						if ((*verifyItor).first == pMapInfo->nMapID)
							bCheck = true;
					}

					if (bCheck == false)
					{
						int nPermitCount = pGateSox->GetFieldFromLablePtr((*ii), "_PartyLimit" )->GetInteger();
						if (nPermitCount > 0)
							m_vPermitPartyCount.push_back(std::make_pair(pMapInfo->nMapID, nPermitCount));
					}
				}
			}
		}
	}

#if defined(_GAMESERVER)
	SAFE_DELETE (pCheckWorld);
	SAFE_DELETE (pRoom);
#if defined( _FINAL_BUILD )
	if (vFailMap.size() > 0){
		g_Log.Log(LogType::_FILELOG, L"[MapTable] vFailMap.size(): %d\r\n", (int)vFailMap.size());
		return false;
	}
#endif // #if defined( _FINAL_BUILD )
#endif	// #if defined(_GAMESERVER)
	return true;
}

const TMapInfo * CDNGameDataManager::GetMapInfo(int nMapID) const
{
	TMapInfoMap::const_iterator ii = m_pMapInfo.find(nMapID);
	if (ii != m_pMapInfo.end())
		return(*ii).second;
	return NULL;
}

int CDNGameDataManager::GetMapIndexByGateNo(int nMapIndex, int nGateNo, int nSelect)
{
	if (nMapIndex < 0) return -1;
	if (nGateNo <= 0) return -1;
	if (nSelect < 0) return -1;

	TMapInfoMap::iterator iter = m_pMapInfo.find(nMapIndex);
	if (iter != m_pMapInfo.end())
	{
		if (nSelect < iter->second->GateMapCount[nGateNo-1])
			return iter->second->GateMapIndexs[nGateNo-1][nSelect];
	}

	return -1;
}

int CDNGameDataManager::GetGateNoByGateNo( int nMapIndex, int nGateNo, int nSelect )
{
	if (nMapIndex < 0) return -1;
	if (nGateNo <= 0) return -1;
	if (nSelect < 0) return -1;

	const TMapInfo *pMapInfo = GetMapInfo(nMapIndex);
	if (!pMapInfo) return -1;

	if (nSelect < pMapInfo->GateMapCount[nGateNo-1])
		return pMapInfo->GateStartIndexs[nGateNo-1][nSelect];

	return -1;
}

GlobalEnum::eMapTypeEnum CDNGameDataManager::GetMapType(int nMapIndex)
{
	if (nMapIndex < 0) return GlobalEnum::MAP_UNKNOWN;

	const TMapInfo *pMapInfo = GetMapInfo(nMapIndex);
	if (!pMapInfo) return GlobalEnum::MAP_UNKNOWN;

	return pMapInfo->MapType;
}

GlobalEnum::eMapSubTypeEnum CDNGameDataManager::GetMapSubType( int nMapIndex )
{
	if (nMapIndex < 0) 
		return GlobalEnum::MAPSUB_NONE;

	const TMapInfo *pMapInfo = GetMapInfo(nMapIndex);
	if (!pMapInfo) return GlobalEnum::MAPSUB_NONE;

	return pMapInfo->MapSubType;
}

bool CDNGameDataManager::IsFindValidMap( const TMapInfo* pMapInfo, int nTargetIndex )
{
	USHORT wMapIndex  = 0;
	int nTemp;
	char szLabel[64];
	DNTableFileFormat *pDungeonSox = GetDNTable( CDnTableDB::TDUNGEONMAP );

	for( int i=0; i<WORLDMAP_GATECOUNT; i++ )
	{
		for (int j=0; j<pMapInfo->GateMapCount[i]; j++)
		{
			wMapIndex = pMapInfo->GateMapIndexs[i][j];
			if( wMapIndex == 0 ) continue;

			if( wMapIndex < DUNGEONGATE_OFFSET )
			{
				if( wMapIndex == nTargetIndex )
					return true;
			}
			else
			{
				// 스테이지 셋으로 검사
				for( int k=0; k<5; k++ )
				{
					sprintf_s( szLabel, "_MapIndex%d", k + 1 );
					nTemp = pDungeonSox->GetFieldFromLablePtr( wMapIndex, szLabel )->GetInteger();
					if( nTemp < 1 ) continue;

					if( nTemp == nTargetIndex )
						return true;
				}
			}		

			if( pMapInfo->vMapAreaIndex.empty() == false )
			{
				std::vector<int>::const_iterator iter = pMapInfo->vMapAreaIndex.begin();
				for( ; iter != pMapInfo->vMapAreaIndex.end(); ++iter )
				{
					wMapIndex = (*iter);
					if( wMapIndex == nTargetIndex )
						return true;
				}
			}
		}
	}

	return false;
}

bool CDNGameDataManager::CheckChangeMap( int nPrevMapIndex, int nNextMapIndex )
{
	// 캐릭터 생성할 경우
	if( nPrevMapIndex == 0)
		return true;

	if( nPrevMapIndex == nNextMapIndex )
		return true;

	// 쌍방향으로 검사한다.
	const TMapInfo* pPrevMapInfo = GetMapInfo( nPrevMapIndex );
	if( !pPrevMapInfo) return false;	// 이전 맵 데이터가 없다

	if( IsFindValidMap(pPrevMapInfo, nNextMapIndex) )
		return true;

	const TMapInfo* pNextMapInfo = GetMapInfo( nNextMapIndex );
	if( !pNextMapInfo ) return false;	// 다음 맵 데이터가 없다

	if( IsFindValidMap( pNextMapInfo, nPrevMapIndex) )
		return true;

	return false;
}

bool CDNGameDataManager::IsMapExpandable(int nMapIndex)
{
	if (nMapIndex < 0) return false;

	const TMapInfo *pMapInfo = GetMapInfo(nMapIndex);
	if (!pMapInfo) return false;

	return pMapInfo->bExpandable;
}

bool CDNGameDataManager::IsVehicleMode(int nMapIndex)
{
	if (nMapIndex < 0) return false;

	const TMapInfo *pMapInfo = GetMapInfo(nMapIndex);
	if (!pMapInfo) return false;

	return pMapInfo->bVehicleMode;
}

#if defined(PRE_ADD_VEHICLE_SPECIAL_ACTION)
bool CDNGameDataManager::IsVehicleSpecialActionMode(int nMapIndex)
{
	if (nMapIndex < 0) return false;

	const TMapInfo *pMapInfo = GetMapInfo(nMapIndex);
	if (!pMapInfo) return false;

	return pMapInfo->bVehicleSpecalActionMode ? true : false;
}
#endif


bool CDNGameDataManager::IsPetMode(int nMapIndex)
{
	if (nMapIndex < 0) return false;

	const TMapInfo *pMapInfo = GetMapInfo(nMapIndex);
	if (!pMapInfo) return false;

	return pMapInfo->bPetMode;
}


//---------------------------------------------------------------------------------
// Item(ItemTable.dnt - TItemData)
//---------------------------------------------------------------------------------

bool CDNGameDataManager::LoadItemData()
{
#ifdef _WORK
	DNTableFileFormat *pSox, *pPeriodSox;
	if (m_bAllLoaded){
		pSox = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TITEM );
		pPeriodSox = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TITEMPERIOD );
	}
	else{
		pSox = GetDNTable( CDnTableDB::TITEM );
		pPeriodSox = GetDNTable( CDnTableDB::TITEMPERIOD );
	}
#else		//#ifdef _WORK
	DNTableFileFormat *pSox = GetDNTable( CDnTableDB::TITEM );
	DNTableFileFormat *pPeriodSox = GetDNTable( CDnTableDB::TITEMPERIOD );
#endif		//#ifdef _WORK
	if (!pSox){
		g_Log.Log( LogType::_FILELOG, L"ItemTable.dnt failed\r\n");
		return false;
	}
	if (pSox->GetItemCount() <= 0){
		g_Log.Log(LogType::_FILELOG, L"ItemTable.dnt Count(%d)\r\n", pSox->GetItemCount());
		return false;
	}
	if (!pPeriodSox){
		g_Log.Log( LogType::_FILELOG, L"itemperiod.dnt failed\r\n");
		return false;
	}
	if (pPeriodSox->GetItemCount() <= 0){
		g_Log.Log(LogType::_FILELOG, L"itemperiod.dnt Count(%d)\r\n", pSox->GetItemCount());
		return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		SAFE_DELETE_PMAP( TMapItemData, m_pItemData );
	}
#endif		//#ifdef _WORK

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{
		int _NameID;
		int _NameIDParam;
		int	_Type;
		int	_TypeParam1;
		int	_TypeParam2;
		int	_TypeParam3;
		int _LevelLimit;
		int _Rank;
		int _Reversion;
		int _IsCash;
		int _IsAuthentication;
		int _IsDestruction;
		int _Amount;
		int _SellAmount;
		int _OverlapCount;
		int _SkillID;
		int _SkillLevel;
		int _SkillUsingType;
		int _AllowMapTypes;
		int _NeedJobClass;
		int _EnchantID;
		int	_SealCount;
		int _SealID;
		int _NeedBuyItem;
		int _NeedBuyItemCount;
		int _NeedPvPRank;
		int _IsSealed;
		int _CashTradeCount;
		int _IsEnableCostumeMix;
		int _ApplicableValue;
		int _IsCollectingEvent;
		int _ExchangeType;
		int _NeedBuyLadderPoint;
		int _UseLevelLimit;
		int _NeedUnionPoint;
		int _NeedBuyGuildWarPoint;
		int _isRebuyable;
#if defined( PRE_ADD_SERVER_WAREHOUSE )
		int _IsWStorage;
#endif
	};
	
	TempFieldNum sFieldNum;
	sFieldNum._NameID			= pSox->GetFieldNum( "_NameID" );
	sFieldNum._NameIDParam		= pSox->GetFieldNum( "_NameIDParam" );
	sFieldNum._Type				= pSox->GetFieldNum( "_Type" );
	sFieldNum._TypeParam1		= pSox->GetFieldNum( "_TypeParam1" );
	sFieldNum._TypeParam2		= pSox->GetFieldNum( "_TypeParam2" );
	sFieldNum._TypeParam3		= pSox->GetFieldNum( "_TypeParam3" );
	sFieldNum._LevelLimit		= pSox->GetFieldNum( "_LevelLimit" );
	sFieldNum._Rank				= pSox->GetFieldNum( "_Rank" );
	sFieldNum._Reversion		= pSox->GetFieldNum( "_Reversion" );
	sFieldNum._IsCash			= pSox->GetFieldNum( "_IsCash" );
	sFieldNum._IsAuthentication = pSox->GetFieldNum( "_IsAuthentication" );
	sFieldNum._IsDestruction	= pSox->GetFieldNum( "_IsDestruction" );
	sFieldNum._Amount			= pSox->GetFieldNum( "_Amount" );
	sFieldNum._SellAmount		= pSox->GetFieldNum( "_SellAmount" );
	sFieldNum._OverlapCount		= pSox->GetFieldNum( "_OverlapCount" );
	sFieldNum._SkillID			= pSox->GetFieldNum( "_SkillID" );
	sFieldNum._SkillLevel		= pSox->GetFieldNum( "_SkillLevel" );
	sFieldNum._SkillUsingType	= pSox->GetFieldNum( "_SkillUsingType" );
	sFieldNum._AllowMapTypes	= pSox->GetFieldNum( "_AllowMapTypes" );
	sFieldNum._NeedJobClass		= pSox->GetFieldNum( "_NeedJobClass" );
	sFieldNum._EnchantID		= pSox->GetFieldNum( "_EnchantID" );
	sFieldNum._SealCount		= pSox->GetFieldNum( "_SealCount" );
	sFieldNum._SealID			= pSox->GetFieldNum( "_SealID" );
	sFieldNum._NeedBuyItem		= pSox->GetFieldNum( "_NeedBuyItem" );
	sFieldNum._NeedBuyItemCount	= pSox->GetFieldNum( "_NeedBuyItemCount" );
	sFieldNum._NeedPvPRank		= pSox->GetFieldNum( "_NeedPvPRank" );
	sFieldNum._IsSealed			= pSox->GetFieldNum( "_IsSealed" );
	sFieldNum._CashTradeCount	= pSox->GetFieldNum( "_CashTradeCount" );
	sFieldNum._IsEnableCostumeMix= pSox->GetFieldNum( "_Compose" );
	sFieldNum._ApplicableValue	= pSox->GetFieldNum( "_ApplicableValue" );
	sFieldNum._IsCollectingEvent = pSox->GetFieldNum("_IsCollectingEvent");
	sFieldNum._ExchangeType = pSox->GetFieldNum("_ExchangeType");
	sFieldNum._NeedBuyLadderPoint = pSox->GetFieldNum("_NeedBuyLadderPoint");
	sFieldNum._UseLevelLimit = pSox->GetFieldNum( "_UseLevelLimit" );
	sFieldNum._NeedUnionPoint = pSox->GetFieldNum( "_NeedUnionPoint" );
	sFieldNum._NeedBuyGuildWarPoint = pSox->GetFieldNum( "_NeedGuildFestPoint" );
	sFieldNum._isRebuyable = pSox->GetFieldNum( "_IsRebuyable" );
#if defined( PRE_ADD_SERVER_WAREHOUSE )
	sFieldNum._IsWStorage = pSox->GetFieldNum( "_AbleWStorage");
#endif

	//##################################################################
	// Load
	//##################################################################

	int nJob	= 0;
	int nNameID = 0;
	char *szNameParam = NULL;
	wstring wszName;

	for (int i = 0; i < pSox->GetItemCount(); i++)
	{
		TItemData* pItemData = new TItemData;
		pItemData->Reset();

		pItemData->nItemID = pSox->GetItemID(i);

		int iIdx = pSox->GetIDXprimary( pItemData->nItemID );

		nNameID = pSox->GetFieldFromLablePtr( iIdx, sFieldNum._NameID )->GetInteger();
		szNameParam = pSox->GetFieldFromLablePtr( iIdx, sFieldNum._NameIDParam )->GetString();
		MakeUIStringUseVariableParam( wszName, nNameID, szNameParam );

		_wcscpy(pItemData->wszItemName, _countof(pItemData->wszItemName), wszName.c_str(), (int)wcslen(wszName.c_str()) );

		pItemData->nType				= pSox->GetFieldFromLablePtr( iIdx, sFieldNum._Type )->GetInteger();
		pItemData->nTypeParam[0]		= pSox->GetFieldFromLablePtr( iIdx, sFieldNum._TypeParam1 )->GetInteger();
		pItemData->nTypeParam[1]		= pSox->GetFieldFromLablePtr( iIdx, sFieldNum._TypeParam2 )->GetInteger();
		pItemData->nTypeParam[2]		= pSox->GetFieldFromLablePtr( iIdx, sFieldNum._TypeParam3 )->GetInteger();
		pItemData->cLevelLimit			= pSox->GetFieldFromLablePtr( iIdx, sFieldNum._LevelLimit )->GetInteger();
		pItemData->cRank				= pSox->GetFieldFromLablePtr( iIdx, sFieldNum._Rank )->GetInteger();
		pItemData->cReversion			= pSox->GetFieldFromLablePtr( iIdx, sFieldNum._Reversion )->GetInteger();
		pItemData->IsCash				= pSox->GetFieldFromLablePtr( iIdx, sFieldNum._IsCash )->GetInteger() ? true : false;
		pItemData->IsAuthentication		= pSox->GetFieldFromLablePtr( iIdx, sFieldNum._IsAuthentication )->GetInteger() ? true : false;
		pItemData->IsDestruction		= pSox->GetFieldFromLablePtr( iIdx, sFieldNum._IsDestruction )->GetInteger() ? true : false;
		pItemData->nAmount				= pSox->GetFieldFromLablePtr( iIdx, sFieldNum._Amount )->GetInteger();
		pItemData->nSellAmount			= pSox->GetFieldFromLablePtr( iIdx, sFieldNum._SellAmount )->GetInteger();
		pItemData->nOverlapCount		= pSox->GetFieldFromLablePtr( iIdx, sFieldNum._OverlapCount )->GetInteger();
		pItemData->nSkillID				= pSox->GetFieldFromLablePtr( iIdx, sFieldNum._SkillID )->GetInteger();
		pItemData->cSkillLevel			= pSox->GetFieldFromLablePtr( iIdx, sFieldNum._SkillLevel )->GetInteger();
		pItemData->cSkillUsingType		= pSox->GetFieldFromLablePtr( iIdx, sFieldNum._SkillUsingType )->GetInteger();
		pItemData->nAllowMapType		= pSox->GetFieldFromLablePtr( iIdx, sFieldNum._AllowMapTypes )->GetInteger();
		pItemData->nNeedBuyItemID		= pSox->GetFieldFromLablePtr( iIdx, sFieldNum._NeedBuyItem )->GetInteger();
		pItemData->nNeedBuyItemCount	= pSox->GetFieldFromLablePtr( iIdx, sFieldNum._NeedBuyItemCount )->GetInteger();
		pItemData->nNeedPvPRank			= pSox->GetFieldFromLablePtr( iIdx, sFieldNum._NeedPvPRank )->GetInteger();
		pItemData->IsSealed				= pSox->GetFieldFromLablePtr( iIdx, sFieldNum._IsSealed)->GetInteger() ? true : false;
		pItemData->nCashTradeCount		= pSox->GetFieldFromLablePtr( iIdx, sFieldNum._CashTradeCount )->GetInteger();
		pItemData->IsEnableCostumeMix	= pSox->GetFieldFromLablePtr( iIdx, sFieldNum._IsEnableCostumeMix)->GetInteger() ? true : false;
		pItemData->nApplicableValue		= pSox->GetFieldFromLablePtr( iIdx, sFieldNum._ApplicableValue )->GetInteger();
		pItemData->IsCollectingEvent	= pSox->GetFieldFromLablePtr( iIdx, sFieldNum._IsCollectingEvent)->GetInteger() ? true : false;
		pItemData->nExchangeCode		= pSox->GetFieldFromLablePtr( iIdx, sFieldNum._ExchangeType )->GetInteger();
		pItemData->iNeedBuyLadderPoint	= pSox->GetFieldFromLablePtr( iIdx, sFieldNum._NeedBuyLadderPoint )->GetInteger();
		pItemData->iUseLevelLimit		= pSox->GetFieldFromLablePtr( iIdx, sFieldNum._UseLevelLimit )->GetInteger();
		pItemData->iNeedBuyUnionPoint	= pSox->GetFieldFromLablePtr( iIdx, sFieldNum._NeedUnionPoint )->GetInteger();
		pItemData->iNeedBuyGuildWarPoint	= pSox->GetFieldFromLablePtr( iIdx, sFieldNum._NeedBuyGuildWarPoint )->GetInteger();
		pItemData->bRebuyable	= pSox->GetFieldFromLablePtr( iIdx, sFieldNum._isRebuyable )->GetInteger() ? true : false;
#if defined( PRE_ADD_SERVER_WAREHOUSE )
		pItemData->IsWStorage			= pSox->GetFieldFromLablePtr( iIdx, sFieldNum._IsWStorage )->GetInteger() ? true : false;
#endif

		std::vector<std::string> JobTokens;

		char *pStr = pSox->GetFieldFromLablePtr( iIdx, sFieldNum._NeedJobClass )->GetString();
		TokenizeA(pStr, JobTokens, ";");

		for (int j = 0; j < (int)JobTokens.size(); j++)
		{
			int iJob = atoi(JobTokens[j].c_str());
			if( iJob > 0 )
				pItemData->nNeedJobClassList.push_back(iJob);
		}

		pItemData->nMaxCoolTime = GetSkillDelayTime(pItemData->nSkillID, pItemData->cSkillLevel);
		pItemData->nEnchantID = pSox->GetFieldFromLablePtr( iIdx, sFieldNum._EnchantID)->GetInteger();

		pItemData->cSealCount = pSox->GetFieldFromLablePtr( iIdx, sFieldNum._SealCount )->GetInteger();
		pItemData->nSealID = pSox->GetFieldFromLablePtr( iIdx, sFieldNum._SealID )->GetInteger();

		pItemData->nPeriod = pPeriodSox->GetFieldFromLablePtr( pItemData->nItemID, "_Period" )->GetInteger();

		std::pair<TMapItemData::iterator,bool> Ret = m_pItemData.insert(make_pair(pItemData->nItemID, pItemData));

		if( pItemData->cReversion == ITEMREVERSION_GUILD )
			m_GuildReversionTableData.insert( make_pair(pItemData->nItemID, 1) );
		if( Ret.second == false )
			delete pItemData;
	}

	return true;
}

TItemData* CDNGameDataManager::GetItemData(int nItemID)
{
	if (nItemID <= 0) return NULL;

	TMapItemData::iterator iter = m_pItemData.find(nItemID);
	if (iter != m_pItemData.end()){
		return iter->second;
	}

	return NULL;
}

const TItemData* CDNGameDataManager::GetItemData(int nItemID) const
{
	if (nItemID <= 0) return NULL;

	TMapItemData::const_iterator iter = m_pItemData.find(nItemID);
	if (iter != m_pItemData.end()){
		return iter->second;
	}

	return NULL;
}

bool CDNGameDataManager::IsItemNeedJob(int nItemID, int nJob)
{
	if (nItemID <= 0) return false;

	TItemData *pItemData = GetItemData(nItemID);
	if (!pItemData) return false;
	if (pItemData->nNeedJobClassList.empty()) return true;

	for (int i = 0; i <(int)pItemData->nNeedJobClassList.size(); i++){
		if (pItemData->nNeedJobClassList[i] == nJob) return true;
	}

	return false;
}

bool CDNGameDataManager::IsPermitItemJob(int nItemID, BYTE *cJobArray)
{
	if (nItemID <= 0) return false;

	TItemData *pItemData = GetItemData(nItemID);
	if (!pItemData) return false;
	if (pItemData->nNeedJobClassList.empty()) return true;

	for (int j = 0; j < JOBMAX; j++){
		if (cJobArray[j] <= 0) continue;
		for (int i = 0; i <(int)pItemData->nNeedJobClassList.size(); i++){
			if (pItemData->nNeedJobClassList[i] == cJobArray[j]) return true;
		}
	}

	return false;
}

bool CDNGameDataManager::IsItemNeedJobHistory( int nItemID, const BYTE* cJobArray, bool bForDice )
{
	if ( nItemID <= 0 ) return false;

	TItemData *pItemData = GetItemData(nItemID);
	if (!pItemData) return false;
	if (pItemData->nNeedJobClassList.empty())
		return bForDice ? false : true;

	for (int i = 0; i <(int)pItemData->nNeedJobClassList.size(); i++)
	{
		for( int k = 0; k < JOBMAX; ++k )
		{
			if( 0 < cJobArray[ k ] )
			{
				if(pItemData->nNeedJobClassList[ i ] == cJobArray[ k ]) 
					return true;
			}
			else
				break;
		}
	}

	return false;
}

bool CDNGameDataManager::IsItemOnlyNeedJob(int nItemID, int nJob) const
{
	if (nItemID <= 0)
		return false;

	const TItemData *pItemData = GetItemData(nItemID);
	if (!pItemData)
		return false;

	if (pItemData->nNeedJobClassList.empty())
		return false;

	for (int i = 0; i <(int)pItemData->nNeedJobClassList.size(); i++)
	{
		if (pItemData->nNeedJobClassList[i] == nJob)
			return true;
	}

	return false;
}

int CDNGameDataManager::GetItemMainType(int nItemID)
{
	if (nItemID <= 0) return 0;

	TItemData *pItemData = GetItemData(nItemID);
	if (!pItemData) return 0;

	return pItemData->nType;
}

BYTE CDNGameDataManager::GetItemDetailType(int nItemID)
{
	if (nItemID <= 0) return 0;

	TItemData *pItemData = GetItemData(nItemID);
	if (!pItemData) return 0;
	
	return GetItemDetailType(pItemData);
}

BYTE CDNGameDataManager::GetItemDetailType(TItemData * pItemData)
{
	if (pItemData == NULL) 
		return 0;

	switch(pItemData->nType)
	{
	case ITEMTYPE_WEAPON:		// 무기/공격보조구
		{
			TWeaponData *pWeapon = GetWeaponData(pItemData->nItemID);
			if (!pWeapon) break;
			return pWeapon->cEquipType;
		}
		break;

	case ITEMTYPE_PARTS:		// 방어구
		{
			TPartData *pPart = GetPartData(pItemData->nItemID);
			if (!pPart) break;
			return pPart->nParts;
		}
		break;

	case ITEMTYPE_NORMAL:		// 기타
		break;

	case ITEMTYPE_SKILL:
		break;

	case ITEMTYPE_JEWEL:		// 보옥
		break;

	case ITEMTYPE_PLATE:		// 문장(플레이트)
		break;

	case ITEMTYPE_INSTANT:		//
		break;
	case ITEMTYPE_GLYPH:		// 문장
		{
			return GetGlyphType(pItemData->nItemID);
		}
		break;
#if defined(PRE_ADD_TALISMAN_SYSTEM)
	case ITEMTYPE_TALISMAN:		// 탈리스만
		break;
#endif
	}

	return 0;
}

BYTE CDNGameDataManager::GetItemRank(int nItemID)
{
	if (nItemID <= 0) return 0;

	TItemData *pItemData = GetItemData(nItemID);
	if (!pItemData) return 0;

	return pItemData->cRank;
}

int CDNGameDataManager::GetItemOverlapCount(int nItemID)
{
	if (nItemID <= 0) return 0;

	TItemData *pItemData = GetItemData(nItemID);
	if (!pItemData) return 0;

	return pItemData->nOverlapCount;
}

BYTE CDNGameDataManager::GetItemReversion(int nItemID)
{
	if (nItemID <= 0) return 0;

	TItemData *pItemData = GetItemData(nItemID);
	if (!pItemData) return 0;

	return pItemData->cReversion;
}

int CDNGameDataManager::GetItemPrice(int nItemID)
{
	if (nItemID <= 0) return 0;

	TItemData *pItemData = GetItemData(nItemID);
	if (!pItemData) return 0;

	return pItemData->nAmount;
}

bool CDNGameDataManager::IsItemDestruction(int nItemID)
{
	if (nItemID <= 0) return 0;

	TItemData *pItemData = GetItemData(nItemID);
	if (!pItemData) return 0;

	return pItemData->IsDestruction;
}

char CDNGameDataManager::GetItemSkillUsingType(int nItemID)
{
	if (nItemID <= 0) return 0;

	TItemData *pItemData = GetItemData(nItemID);
	if (!pItemData) return 0;

	return pItemData->cSkillUsingType;
}

int CDNGameDataManager::GetItemTypeParam1(int nItemID)
{
	if (nItemID <= 0) return 0;

	TItemData *pItemData = GetItemData(nItemID);
	if (!pItemData) return 0;

	return pItemData->nTypeParam[0];
}

int CDNGameDataManager::GetItemTypeParam2(int nItemID)
{
	if (nItemID <= 0) return 0;

	TItemData *pItemData = GetItemData(nItemID);
	if (!pItemData) return 0;

	return pItemData->nTypeParam[1];
}

bool CDNGameDataManager::IsCashItem(int nItemID)
{
	if (nItemID <= 0) return false;

	TItemData *pItemData = GetItemData(nItemID);
	if (!pItemData) return false;

	return pItemData->IsCash;
}

int CDNGameDataManager::GetItemCashTradeCount(int nItemID)
{
#if defined(_JP)
	return 0;	// 일본은 안쓴다
#endif	// _JP

	if (nItemID <= 0) return 0;

	TItemData *pItemData = GetItemData(nItemID);
	if (!pItemData) return 0;

	return pItemData->nCashTradeCount;
}

BYTE CDNGameDataManager::GetItemLevelLimit(int nItemID)
{
	if (nItemID <= 0) return 0;

	TItemData *pItemData = GetItemData(nItemID);
	if (!pItemData) return 0;

	return pItemData->cLevelLimit;
}

char CDNGameDataManager::GetItemSealCount(int nItemID)
{
	if (nItemID <= 0) return 0;

	TItemData *pItemData = GetItemData(nItemID);
	if (!pItemData) return 0;

	return pItemData->cSealCount;
}

bool CDNGameDataManager::IsUseItemAllowMapTypeCheck(int iItemID, int iMapIndex)
{
	TItemData*	pItemData = GetItemData( iItemID );
	if( pItemData == NULL )
		return false;

	const TMapInfo*	pMapData = GetMapInfo( iMapIndex );
	if( pMapData == NULL )
		return false;

	return (pItemData->nAllowMapType&pMapData->nAllowMapType) ? true : false;
}

int CDNGameDataManager::GetMapPermitLevel(int nMapID)
{
	TVecVillagePermitLevel::iterator ii;
	for (ii = m_vVillagePermitLevel.begin(); ii != m_vVillagePermitLevel.end(); ii++)
	{
		if ((*ii).first == nMapID)
			return(*ii).second;
	}
	return -1;
}

#if defined(PRE_ADD_CHALLENGE_DARKLAIR)
bool CDNGameDataManager::IsCloseGateByTime(int nMapIndex)
{
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TMAPGATE );

	std::vector<int> nVecDeungeonItemList;
	pSox->GetItemIDListFromField( "_MapIndex", nMapIndex, nVecDeungeonItemList );
	
	for( DWORD i=0; i<nVecDeungeonItemList.size(); i++ ) 
	{
		int nItemID = nVecDeungeonItemList[i];
		if (nItemID <= 0)
			continue;

		DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TMAPGATE );
		char* pszDateTime = pSox->GetFieldFromLablePtr( nItemID, "_DateTime" )->GetString();
		if(!pszDateTime )
			continue;

		std::vector<string> vGateDateTime;
		TokenizeA(pszDateTime , vGateDateTime, ":");

		int nType = 0, nAttr1 = 0, nAttr2 = 0, nAttr3 = 0;
		if (vGateDateTime.size() > 0)
		{
			CTimeSet LocalSet;
			nType = atoi(vGateDateTime[0].c_str());
			//오픈 시간
			if (nType == MapGateCondition::oDailyHours && vGateDateTime.size() == 3)	// 시간
			{
				nAttr1 = atoi(vGateDateTime[1].c_str());
				nAttr2 = atoi(vGateDateTime[2].c_str());

				if (nAttr1 <= LocalSet.GetHour() && LocalSet.GetHour() < nAttr2)
					continue;
				else
					return true;
			}
			else if (nType == MapGateCondition::oDayOfWeek && vGateDateTime.size() == 2) // 요일
			{
				nAttr1 = atoi(vGateDateTime[1].c_str());		
				if (LocalSet.GetDayOfWeek() != nAttr1)
					return true;
			}
			else if (nType == MapGateCondition::oDayOfWeekAndHours && vGateDateTime.size() == 4) // 요일 & 시간
			{
				nAttr1 = atoi(vGateDateTime[1].c_str());
				nAttr2 = atoi(vGateDateTime[2].c_str());
				nAttr3 = atoi(vGateDateTime[3].c_str());

				if (LocalSet.GetDayOfWeek() != nAttr1)
					return true;

				if (nAttr2 <= LocalSet.GetHour() && LocalSet.GetHour() < nAttr3)
					continue;
				else
					return true;
			}
			else if (nType == MapGateCondition::oDayOfWeekDuringFatigueTime && vGateDateTime.size() == 2) // 요일 피로도 시간
			{
				nAttr1 = atoi(vGateDateTime[1].c_str());
				if( LocalSet.GetDayOfWeek() != nAttr1 && LocalSet.GetDayOfWeek() != (nAttr1+1)%7 ) 
					return true;

				DNTableFileFormat *pSox = GetDNTable( CDnTableDB::TSCHEDULE );
				if( !pSox )
					return true;

				int nItemID = CDNSchedule::ResetFatigue +1;
				if( !pSox->IsExistItem( nItemID ) ) return true;
				int nHour = pSox->GetFieldFromLablePtr( nItemID, "_Hour" )->GetInteger();
				int nMinute = pSox->GetFieldFromLablePtr( nItemID, "_Minute" )->GetInteger();

				if( LocalSet.GetDayOfWeek() == nAttr1 )	//초기화 시간 이후 인지 쳌
				{
					if( LocalSet.GetHour() >= nHour && LocalSet.GetMinute() >= nMinute )
						continue;
					else
						return true;
				}
				else if( LocalSet.GetDayOfWeek() == (nAttr1+1)%7 )	//초기화 시간 전 인지 쳌
				{
					if( LocalSet.GetHour() < nHour || (LocalSet.GetMinute() < nMinute && LocalSet.GetHour() <= nHour) )
						continue;
					else
						return true;
				}
			}
			//클로즈 시간
			else if (nType == MapGateCondition::cMonthlyHours && vGateDateTime.size() == 4) // 매월 해당일 & 시간만 *클로즈*
			{
				nAttr1 = atoi(vGateDateTime[1].c_str());
				nAttr2 = atoi(vGateDateTime[2].c_str());
				nAttr3 = atoi(vGateDateTime[3].c_str());

				if (LocalSet.GetDay() != nAttr1)
					continue;

				if (LocalSet.GetHour() >= nAttr2 && LocalSet.GetHour() < nAttr3)
					return true;
				else
					continue;
			}
		}
	}

	return false;

}
#endif

int CDNGameDataManager::GetMapPermitPartyCount(int nMapID)
{
	TVecPermitPartyCount::iterator ii;
	for (ii = m_vPermitPartyCount.begin(); ii != m_vPermitPartyCount.end(); ii++)
	{
		if ((*ii).first == nMapID)
			return(*ii).second;
	}
	return 0;
}


//---------------------------------------------------------------------------------
// Actor(ActorTable.dnt - TActorData)
//---------------------------------------------------------------------------------

bool CDNGameDataManager::LoadActorData()
{
#ifdef _WORK
	DNTableFileFormat *pSox;
	if (m_bAllLoaded)
		pSox = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TACTOR );
	else
		pSox = GetDNTable( CDnTableDB::TACTOR );
#else		//#ifdef _WORK
	DNTableFileFormat *pSox = GetDNTable( CDnTableDB::TACTOR );
#endif		//#ifdef _WORK
	if (!pSox){
		g_Log.Log(LogType::_FILELOG, L"ActorTable.dnt failed\r\n");
		return false;
	}

	if (pSox->GetItemCount() <= 0){
		g_Log.Log(LogType::_ERROR, L"ActorTable.dnt Count(%d)\r\n", pSox->GetItemCount());
		return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		SAFE_DELETE_PMAP( TMapActorData, m_pActorData );
	}
#endif		//#ifdef _WORK

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	//##################################################################
	// Load
	//##################################################################

	for( int i=0 ; i<pSox->GetItemCount() ; ++i )
	{
		TActorData* pActorData = new TActorData;
		memset(pActorData, 0, sizeof(TActorData));

		pActorData->nActorID = pSox->GetItemID(i);
		std::pair<TMapActorData::iterator,bool> Ret = m_pActorData.insert(make_pair(pActorData->nActorID, pActorData));
		if( Ret.second == false )
			delete pActorData;
	}

	return true;
}

TActorData* CDNGameDataManager::GetActorData(int nActorIndex)
{
	if (nActorIndex <= 0) return NULL;
	if (m_pActorData.empty()) return NULL;

	TMapActorData::iterator iter = m_pActorData.find(nActorIndex);
	if (iter != m_pActorData.end()){
		return iter->second;
	}

	return NULL;
}

//---------------------------------------------------------------------------------
// Npc(NPCTable.dnt - TNpcData, TActorData)
//---------------------------------------------------------------------------------

bool CDNGameDataManager::LoadNpcData()
{
#ifdef _WORK
	DNTableFileFormat *pSox;
	if (m_bAllLoaded)
		pSox = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TNPC );
	else
		pSox = GetDNTable( CDnTableDB::TNPC );
#else		//#ifdef _WORK
	DNTableFileFormat *pSox = GetDNTable( CDnTableDB::TNPC );
#endif		//#ifdef _WORK
	if (!pSox)
	{
		g_Log.Log(LogType::_FILELOG, L"NPCTable.dnt failed\r\n");
		return false;
	}

	if (pSox->GetItemCount() <= 0)
	{
		g_Log.Log(LogType::_FILELOG, L"NPCTable.dnt Count(%d)\r\n", pSox->GetItemCount());
		return false;
	}

#ifdef PRE_FIX_MEMOPT_EXT

#ifdef _WORK
	DNTableFileFormat* pFileNameSox;
	if (m_bAllLoaded)
		pFileNameSox = CDnTableDB::GetInstance().ReLoadSox(CDnTableDB::TFILE);
	else
		pFileNameSox = GetDNTable(CDnTableDB::TFILE);
#else		//#ifdef _WORK
	DNTableFileFormat* pFileNameSox = GetDNTable(CDnTableDB::TFILE);
#endif		//#ifdef _WORK
	if (!pFileNameSox)
	{
		g_Log.Log(LogType::_FILELOG, L"FileTable.dnt failed\r\n");
		return false;
	}

	if (pFileNameSox->GetItemCount() <= 0)
	{
		g_Log.Log(LogType::_FILELOG, L"FileTable.dnt Count(%d)\r\n", pFileNameSox->GetItemCount());
		return false;
	}

#endif		//#ifdef PRE_FIX_MEMOPT_EXT

#ifdef _WORK
	if (m_bAllLoaded)
	{
		SAFE_DELETE_PMAP( TMapNpcData, m_pNpcData );
	}
#endif	//#ifdef _WORK

	char szTemp[256];
	memset( szTemp, 0, sizeof(szTemp) );

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{
		int					_NameID;
		int					_ActorIndex;
		int					_TalkFile;
		int					_ScriptFile;
		int					_Param[2];
		std::vector<int>	_Quest;
	};

	TempFieldNum sFieldNum;
	sFieldNum._NameID		= pSox->GetFieldNum( "_NameID" );
	sFieldNum._ActorIndex	= pSox->GetFieldNum( "_ActorIndex" );
	sFieldNum._TalkFile		= pSox->GetFieldNum( "_TalkFile" );
	sFieldNum._ScriptFile	= pSox->GetFieldNum( "_ScriptFile" );
	sFieldNum._Param[0]		= pSox->GetFieldNum( "_Param1" );
	sFieldNum._Param[1]		= pSox->GetFieldNum( "_Param2" );
	sFieldNum._Quest.reserve( QUEST_MAX_CNT );
	for( int j=0 ; j<QUEST_MAX_CNT ; ++j )
	{
		sprintf_s( szTemp, "_Quest%d", j+1 );
		sFieldNum._Quest.push_back( pSox->GetFieldNum( szTemp ) );
	}

	//##################################################################
	// Load
	//##################################################################

	for (int i = 0; i < pSox->GetItemCount(); i++)
	{
		TNpcData* pNpcData = new TNpcData;
		memset(pNpcData, 0, sizeof(TNpcData));

		pNpcData->nNpcID = pSox->GetItemID(i);

		int iIdx = pSox->GetIDXprimary( pNpcData->nNpcID );

#if defined(PRE_ADD_MULTILANGUAGE)
		//npc관련내용은 좀더 생각해 봐야한다. 일단은 디폰트언어값으로설정
		_wcscpy( pNpcData->wszName, _countof(pNpcData->wszName), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, pSox->GetFieldFromLablePtr( iIdx, sFieldNum._NameID )->GetInteger(), MultiLanguage::eDefaultLanguage ),
			(int)wcslen(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, pSox->GetFieldFromLablePtr( iIdx, sFieldNum._NameID )->GetInteger(), MultiLanguage::eDefaultLanguage )) );
#else		//#if defined(PRE_ADD_MULTILANGUAGE)
		_wcscpy( pNpcData->wszName, _countof(pNpcData->wszName), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, pSox->GetFieldFromLablePtr( iIdx, sFieldNum._NameID )->GetInteger() ),
			(int)wcslen(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, pSox->GetFieldFromLablePtr( iIdx, sFieldNum._NameID )->GetInteger() )) );
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)
		int	ActorIndex = pSox->GetFieldFromLablePtr( iIdx, sFieldNum._ActorIndex )->GetInteger();
		TActorData* pActorData = GetActorData(ActorIndex);
		if( pActorData ) 
			pNpcData->ActorData = *pActorData;
		//else 
		//	g_Log.g( L"[NpcID:%d ActorID:%d] ActorID not found!!\r\n", pNpcData->nNpcID, ActorIndex);

		pNpcData->nParam[0] = pSox->GetFieldFromLablePtr( iIdx, sFieldNum._Param[0] )->GetInteger();
		pNpcData->nParam[1] = pSox->GetFieldFromLablePtr( iIdx, sFieldNum._Param[1] )->GetInteger();
		_strcpy( pNpcData->szTalkFile, _countof(pNpcData->szTalkFile), pSox->GetFieldFromLablePtr( iIdx, sFieldNum._TalkFile )->GetString(), (int)strlen(pSox->GetFieldFromLablePtr( iIdx, sFieldNum._TalkFile )->GetString()));
		_strcpy( pNpcData->szScriptFile, _countof(pNpcData->szScriptFile), pSox->GetFieldFromLablePtr( iIdx, sFieldNum._ScriptFile )->GetString(), (int)strlen(pSox->GetFieldFromLablePtr( iIdx, sFieldNum._ScriptFile )->GetString()));

		for( int j=0 ; j<QUEST_MAX_CNT ; ++j )
			pNpcData->QuestIndexArr[j] = pSox->GetFieldFromLablePtr( iIdx, sFieldNum._Quest[j] )->GetInteger();

		std::pair<TMapNpcData::iterator,bool> Ret = m_pNpcData.insert(make_pair(pNpcData->nNpcID, pNpcData));
		if( Ret.second == false )
			delete pNpcData;
	}

	return true;
}

TNpcData* CDNGameDataManager::GetNpcData( int nNpcIndex )
{
	if (m_pNpcData.empty()) return NULL;

	TMapNpcData::iterator iter = m_pNpcData.find(nNpcIndex);
	if (iter != m_pNpcData.end()){
		return iter->second;
	}

	return NULL;
}

//---------------------------------------------------------------------------------
// Weapon(WeaponTable.dnt - TWeaponData)
//---------------------------------------------------------------------------------

bool CDNGameDataManager::LoadWeaponData()
{
#ifdef _WORK
	DNTableFileFormat *pSox;
	DNTableFileFormat *pItemSox;
	if (m_bAllLoaded)
	{
		pSox = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TWEAPON );
		pItemSox = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TITEM );
	}
	else
	{
		pSox = GetDNTable( CDnTableDB::TWEAPON );
		pItemSox = GetDNTable( CDnTableDB::TITEM );
	}
#else		//#ifdef _WORK
	DNTableFileFormat *pSox = GetDNTable( CDnTableDB::TWEAPON );
	DNTableFileFormat *pItemSox = GetDNTable( CDnTableDB::TITEM );
#endif
	if (!pSox){
		g_Log.Log(LogType::_FILELOG, L"WeaponTable.dnt failed\r\n");
		return false;
	}

	if (pSox->GetItemCount() <= 0){
		g_Log.Log(LogType::_FILELOG, L"WeaponTable.dnt Count(%d)\r\n", pSox->GetItemCount());
		return false;
	}

#ifdef _WORK

	if (m_bAllLoaded)
	{
		SAFE_DELETE_PMAP( TMapWeaponData, m_pWeaponData );
	}
	
	
#endif		//#ifdef _WORK

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{
		int _EquipType;
		int	_Length;
		int	_Durability;
		int	_DurabilityRepairCoin;
		int _OneType;
	};

	TempFieldNum sFieldNum;
	sFieldNum._EquipType			= pSox->GetFieldNum( "_EquipType" );
	sFieldNum._Length				= pSox->GetFieldNum( "_Length" );
	sFieldNum._Durability			= pSox->GetFieldNum( "_Durability" );
	sFieldNum._DurabilityRepairCoin	= pSox->GetFieldNum( "_DurabilityRepairCoin" );
	sFieldNum._OneType				= pSox->GetFieldNum( "_OneType" );

	//##################################################################
	// Load
	//##################################################################

	DNTableCell Field;

	for (int i = 0; i < pSox->GetItemCount(); i++)
	{
		TWeaponData* pWeaponData = new TWeaponData;
		memset(pWeaponData, 0, sizeof(TWeaponData));

		pWeaponData->nWeaponIndex = pSox->GetItemID(i);

		int iIdx = pSox->GetIDXprimary( pWeaponData->nWeaponIndex );

		pWeaponData->cEquipType				= pSox->GetFieldFromLablePtr( iIdx, sFieldNum._EquipType )->GetInteger();
		pWeaponData->nLength				= pSox->GetFieldFromLablePtr( iIdx, sFieldNum._Length )->GetInteger();
		pWeaponData->nDurability			= pSox->GetFieldFromLablePtr( iIdx, sFieldNum._Durability )->GetInteger();
		pWeaponData->nDurabilityRepairCoin	= pSox->GetFieldFromLablePtr( iIdx, sFieldNum._DurabilityRepairCoin )->GetInteger();
		pWeaponData->bOneType				= ( pSox->GetFieldFromLablePtr( iIdx, sFieldNum._OneType )->GetInteger() == TRUE ) ? true : false;
		
		memset( pWeaponData->nPermitJob, 0, sizeof(pWeaponData->nPermitJob) );

		if( pItemSox->GetFieldFromLable( pWeaponData->nWeaponIndex, "_NeedJobClass", Field ) == false ) 
		{
			SAFE_DELETE( pWeaponData );
			continue;
		}
		char *szNeedJob = Field.GetString();

		for( int j=0;; j++ ) 
		{
			const char *pStr = _GetSubStrByCount( j, szNeedJob, ';' );
			if( pStr == NULL ) 
				break;
			pWeaponData->nPermitJob[j] = atoi(pStr);
		}

		std::pair<TMapWeaponData::iterator,bool> Ret = m_pWeaponData.insert(make_pair(pWeaponData->nWeaponIndex, pWeaponData));
		if( Ret.second == false )
			delete pWeaponData;
	}

	return true;
}

TWeaponData* CDNGameDataManager::GetWeaponData(int nWeaponIndex)
{
	if (m_pWeaponData.empty()) return NULL;

	TMapWeaponData::iterator iter = m_pWeaponData.find(nWeaponIndex);
	if (iter != m_pWeaponData.end()){
		return iter->second;
	}

	return NULL;
}

//---------------------------------------------------------------------------------
// Part(PartsTable.dnt - TPartData)
//---------------------------------------------------------------------------------

bool CDNGameDataManager::LoadPartData()
{
#ifdef _WORK
	DNTableFileFormat *pSox;
	DNTableFileFormat *pItemSox;
	if (m_bAllLoaded)
	{
		pSox = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TPARTS );
		pItemSox = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TITEM );
	}
	else
	{
		pSox = GetDNTable( CDnTableDB::TPARTS );
		pItemSox = GetDNTable( CDnTableDB::TITEM );
	}
	
#ifdef PRE_FIX_MEMOPT_EXT
	DNTableFileFormat *pFileNameSox;
	if (m_bAllLoaded)
		pFileNameSox = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TFILE );
	else
		pFileNameSox = GetDNTable( CDnTableDB::TFILE );
	
	if (!pFileNameSox ||(pFileNameSox->GetItemCount() <= 0))
	{
		g_Log.Log(LogType::_FILELOG, L"FileTable.dnt failed\r\n");
		return false;
	}
#endif		//#ifdef PRE_FIX_MEMOPT_EXT

#else		//#ifdef _WORK
	DNTableFileFormat *pSox = GetDNTable( CDnTableDB::TPARTS );
	DNTableFileFormat *pItemSox = GetDNTable( CDnTableDB::TITEM );
#ifdef PRE_FIX_MEMOPT_EXT
	DNTableFileFormat *pFileNameSox = GetDNTable( CDnTableDB::TFILE );
	if (!pFileNameSox ||(pFileNameSox->GetItemCount() <= 0))
	{
		g_Log.Log(LogType::_FILELOG, L"FileTable.dnt failed\r\n");
		return false;
	}
#endif		//#ifdef PRE_FIX_MEMOPT_EXT

#endif		//#ifdef _WORK
	if (!pSox){
		g_Log.Log(LogType::_FILELOG, L"PartsTable.dnt failed\r\n");
		return false;
	}

	if (pSox->GetItemCount() <= 0){
		g_Log.Log(LogType::_FILELOG, L"PartsTable.dnt Count(%d)\r\n", pSox->GetItemCount());
		return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		SAFE_DELETE_PMAP( TMapPartData, m_pPartData );
	}
#endif		//#ifdef _WORK

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{
		int _Parts;
		int	_Durability;
		int	_DurabilityRepairCoin;
		int _SubParts[4];
	};

	TempFieldNum sFieldNum;
	sFieldNum._Parts				= pSox->GetFieldNum( "_Parts" );
	sFieldNum._Durability			= pSox->GetFieldNum( "_Durability" );
	sFieldNum._DurabilityRepairCoin	= pSox->GetFieldNum( "_DurabilityRepairCoin" );
	sFieldNum._SubParts[0]			= pSox->GetFieldNum( "_SubParts1" );
	sFieldNum._SubParts[1]			= pSox->GetFieldNum( "_SubParts2" );
	sFieldNum._SubParts[2]			= pSox->GetFieldNum( "_SubParts3" );
	sFieldNum._SubParts[3]			= pSox->GetFieldNum( "_SubParts4" );

	//##################################################################
	// Load
	//##################################################################
	
	DNTableCell Field;

	for (int i = 0; i < pSox->GetItemCount(); i++)
	{
		TPartData* pPartData = new TPartData;
		//2010.10.7 haling STL 보호 위해 생성자로 변환
		//memset(pPartData, 0, sizeof(TPartData));

		pPartData->nPartIndex				= pSox->GetItemID(i);

		int iIdx = pSox->GetIDXprimary( pPartData->nPartIndex );

		pPartData->nParts					= pSox->GetFieldFromLablePtr( iIdx, sFieldNum._Parts )->GetInteger();
		pPartData->nDurability				= pSox->GetFieldFromLablePtr( iIdx, sFieldNum._Durability )->GetInteger();
		pPartData->nDurabilityRepairCoin	= pSox->GetFieldFromLablePtr( iIdx, sFieldNum._DurabilityRepairCoin )->GetInteger();
		for (int j = 0; j < 4; j++){
			int nSubParts = pSox->GetFieldFromLablePtr( iIdx, sFieldNum._SubParts[j] )->GetInteger();
			if (nSubParts > 0)
				pPartData->nSubPartsList.push_back(nSubParts);
		}

		memset( pPartData->nPermitJob, 0, sizeof(pPartData->nPermitJob) );
		if( pItemSox->GetFieldFromLable( pPartData->nPartIndex, "_NeedJobClass", Field ) == false ) 
		{
			SAFE_DELETE( pPartData );
			continue;
		}

		char *szNeedJob = Field.GetString();
		for( int j=0; j<10; j++ ) 
		{
			const char *pStr = _GetSubStrByCount( j, szNeedJob, ';' );
			if( pStr == NULL ) 
				break;
			pPartData->nPermitJob[j] = atoi(pStr);
		}

		std::pair<TMapPartData::iterator,bool> Ret = m_pPartData.insert(make_pair(pPartData->nPartIndex, pPartData));
		if( Ret.second == false )
			delete pPartData;
	}

	return true;
}

TPartData* CDNGameDataManager::GetPartData(int nPartIndex)
{
	if (m_pPartData.empty()) return NULL;

	TMapPartData::iterator iter = m_pPartData.find(nPartIndex);
	if (iter != m_pPartData.end()){
		return iter->second;
	}

	return NULL;
}

const TPartData* CDNGameDataManager::GetPartData(int nPartIndex) const
{
	if (m_pPartData.empty()) return NULL;

	TMapPartData::const_iterator iter = m_pPartData.find(nPartIndex);
	if (iter != m_pPartData.end()){
		return iter->second;
	}

	return NULL;
}

bool CDNGameDataManager::IsSubParts(int nPartIndex)
{
	TPartData *pPartData = GetPartData(nPartIndex);
	if (!pPartData) return false;
	if (pPartData->nSubPartsList.empty()) return false;
	return true;
}

bool CDNGameDataManager::IsFaceParts(int nPartIndex)
{
	TPartData *pPartData = GetPartData(nPartIndex);
	if (!pPartData) return false;

	if (pPartData->nParts == EQUIP_FACE) return true;
	return false;
}

bool CDNGameDataManager::IsHairParts(int nPartIndex)
{
	TPartData *pPartData = GetPartData(nPartIndex);
	if (!pPartData) return false;

	if (pPartData->nParts == EQUIP_HAIR) return true;
	return false;
}

bool CDNGameDataManager::LoadPotentialJewelData()
{
#ifdef _WORK
	DNTableFileFormat *pSox;
	if (m_bAllLoaded)
	{
		pSox = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TPOTENTIALJEWEL );
	}
	else
	{
		pSox = GetDNTable( CDnTableDB::TPOTENTIALJEWEL );
	}
#else		//#ifdef _WORK
	DNTableFileFormat *pSox = GetDNTable( CDnTableDB::TPOTENTIALJEWEL );
#endif		//#ifdef _WORK
	if (!pSox){
		g_Log.Log(LogType::_FILELOG, L"PotentialJewelTable.dnt failed\r\n");
		return false;
	}

	if (pSox->GetItemCount() <= 0){
		g_Log.Log(LogType::_FILELOG, L"PotentialJewelTable.dnt Count(%d)\r\n", pSox->GetItemCount());
		return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		SAFE_DELETE_PMAP( TMapPotentialJewelData, m_pPotentialJewelData );
	}
#endif		//#ifdef _WORK

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{
		int _ApplicableValue;
		int	_PotentialNo;
		int	_IsErasable;
		int _CanApplySealedItem;

#if defined(PRE_ADD_REMOVE_PREFIX)
		int _IsSuffix;
#endif // PRE_ADD_REMOVE_PREFIX
#if defined(PRE_MOD_POTENTIAL_JEWEL_RENEWAL)
		int _nRollbackAmount;
#endif	// #if defined(PRE_MOD_POTENTIAL_JEWEL_RENEWAL)
	};

	TempFieldNum sFieldNum;
	sFieldNum._ApplicableValue	= pSox->GetFieldNum( "_ApplicableValue" );
	sFieldNum._PotentialNo		= pSox->GetFieldNum( "_PotentialNo" );
	sFieldNum._IsErasable		= pSox->GetFieldNum( "_IsErasable" );
	sFieldNum._CanApplySealedItem = pSox->GetFieldNum( "_CanApplySealedItem" );

#if defined(PRE_ADD_REMOVE_PREFIX)
	sFieldNum._IsSuffix = pSox->GetFieldNum( "_Issuffix" );
#endif // PRE_ADD_REMOVE_PREFIX
#if defined(PRE_MOD_POTENTIAL_JEWEL_RENEWAL)
	sFieldNum._nRollbackAmount = pSox->GetFieldNum("_Charge");
#endif	// #if defined(PRE_MOD_POTENTIAL_JEWEL_RENEWAL)
	//##################################################################
	// Load
	//##################################################################

	for (int i = 0; i < pSox->GetItemCount(); i++)
	{
		TPotentialJewelData* pPotentialData = new TPotentialJewelData;
		memset(pPotentialData, 0, sizeof(TPotentialJewelData));

		pPotentialData->nItemID				= pSox->GetItemID(i);

		int iIdx = pSox->GetIDXprimary( pPotentialData->nItemID );

		pPotentialData->nApplyApplicableValue	= pSox->GetFieldFromLablePtr( iIdx, sFieldNum._ApplicableValue )->GetInteger();
		pPotentialData->nPotentialNo			= pSox->GetFieldFromLablePtr( iIdx, sFieldNum._PotentialNo )->GetInteger();
		pPotentialData->bErasable				= ( pSox->GetFieldFromLablePtr( iIdx, sFieldNum._IsErasable )->GetInteger() == TRUE ) ? true : false;
		pPotentialData->bCanApplySealedItem		= ( pSox->GetFieldFromLablePtr( iIdx, sFieldNum._CanApplySealedItem )->GetInteger() == TRUE ) ? true : false;

#if defined(PRE_ADD_REMOVE_PREFIX)
		pPotentialData->bSuffix		= ( pSox->GetFieldFromLablePtr( iIdx, sFieldNum._IsSuffix )->GetInteger() == TRUE ) ? true : false;
#endif // PRE_ADD_REMOVE_PREFIX
#if defined(PRE_MOD_POTENTIAL_JEWEL_RENEWAL)
		pPotentialData->nRollbackAmount = pSox->GetFieldFromLablePtr( iIdx, sFieldNum._nRollbackAmount )->GetInteger();
#endif	// #if defined(PRE_MOD_POTENTIAL_JEWEL_RENEWAL)
		std::pair<TMapPotentialJewelData::iterator,bool> Ret = m_pPotentialJewelData.insert(make_pair(pPotentialData->nItemID, pPotentialData));
		if( Ret.second == false )
			delete pPotentialData;
	}

	return true;
}

TPotentialJewelData *CDNGameDataManager::GetPotentialJewelData( int nItemID )
{
	if (m_pPotentialJewelData.empty()) return NULL;

	TMapPotentialJewelData::iterator iter = m_pPotentialJewelData.find(nItemID);
	if (iter != m_pPotentialJewelData.end()){
		return iter->second;
	}

	return NULL;
}

bool CDNGameDataManager::LoadEnchantJewelData()
{
#ifdef _WORK
	DNTableFileFormat *pSox;
	if (m_bAllLoaded)
	{
		pSox = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TENCHANTJEWEL );
	}
	else
	{
		pSox = GetDNTable( CDnTableDB::TENCHANTJEWEL );
	}
#else		//#ifdef _WORK
	DNTableFileFormat *pSox = GetDNTable( CDnTableDB::TENCHANTJEWEL );
#endif		//#ifdef _WORK
	if (!pSox){
		g_Log.Log(LogType::_FILELOG, L"EnchantJewelTable.dnt failed\r\n");
		return false;
	}

	if (pSox->GetItemCount() <= 0){
		g_Log.Log(LogType::_FILELOG, L"EnchantJewelTable.dnt Count(%d)\r\n", pSox->GetItemCount());
		return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		SAFE_DELETE_PMAP( TMapEnchantJewelData, m_pEnchantJewelData );
	}
#endif		//#ifdef _WORK

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{
		int _ApplicableValue;		// Potential과 마찬가지
		int _RequireMaxItemLevel;	// 이 제한레벨 이하 아이템에만 적용가능(위 _ApplicableValue와 겹치는 기능이나 세부조절을 위해 추가)
		int	_RequireEnchantLevel;	// 이 강화수치보다 높게 강화된 아이템에만 적용가능
		int	_EnchantLevel;			// 이 강화레벨로 변경.
		int _CanApplySealedItem;
	};

	TempFieldNum sFieldNum;
	sFieldNum._ApplicableValue		= pSox->GetFieldNum( "_ApplicableValue" );
	sFieldNum._RequireMaxItemLevel	= pSox->GetFieldNum( "_RequireMaxItemLevel" );
	sFieldNum._RequireEnchantLevel	= pSox->GetFieldNum( "_RequireEnchantLevel" );
	sFieldNum._EnchantLevel			= pSox->GetFieldNum( "_EnchantLevel" );
	sFieldNum._CanApplySealedItem	= pSox->GetFieldNum( "_CanApplySealedItem" );

	//##################################################################
	// Load
	//##################################################################

	for (int i = 0; i < pSox->GetItemCount(); i++)
	{
		TEnchantJewelData* pEnchantJewelData = new TEnchantJewelData;
		memset(pEnchantJewelData, 0, sizeof(TEnchantJewelData));

		pEnchantJewelData->nItemID				= pSox->GetItemID(i);

		int iIdx = pSox->GetIDXprimary( pEnchantJewelData->nItemID );

		pEnchantJewelData->nApplyApplicableValue	= pSox->GetFieldFromLablePtr( iIdx, sFieldNum._ApplicableValue )->GetInteger();
		pEnchantJewelData->nRequireMaxItemLevel		= pSox->GetFieldFromLablePtr( iIdx, sFieldNum._RequireMaxItemLevel )->GetInteger();
		pEnchantJewelData->nRequireEnchantLevel		= pSox->GetFieldFromLablePtr( iIdx, sFieldNum._RequireEnchantLevel )->GetInteger();
		pEnchantJewelData->nEnchantLevel			= pSox->GetFieldFromLablePtr( iIdx, sFieldNum._EnchantLevel )->GetInteger();
		pEnchantJewelData->bCanApplySealedItem		= ( pSox->GetFieldFromLablePtr( iIdx, sFieldNum._CanApplySealedItem )->GetInteger() == TRUE ) ? true : false;

		std::pair<TMapEnchantJewelData::iterator,bool> Ret = m_pEnchantJewelData.insert(make_pair(pEnchantJewelData->nItemID, pEnchantJewelData));
		if( Ret.second == false )
			delete pEnchantJewelData;
	}

	return true;
}

TEnchantJewelData *CDNGameDataManager::GetEnchantJewelData( int nItemID )
{
	if (m_pEnchantJewelData.empty()) return NULL;

	TMapEnchantJewelData::iterator iter = m_pEnchantJewelData.find(nItemID);
	if (iter != m_pEnchantJewelData.end()){
		return iter->second;
	}

	return NULL;
}

int CDNGameDataManager::GetItemDurability( int nItemID )
{
	// 내구도 구해서 셋팅한다.
	int nDurability = 0;
	TItemData *pItemData = GetItemData( nItemID );
	if( pItemData ) {
		switch( pItemData->nType ) {
			case ITEMTYPE_WEAPON:
				{
					TWeaponData *pWeapon = GetWeaponData( nItemID );
					if( pWeapon ) nDurability = pWeapon->nDurability;
				}
				break;
			case ITEMTYPE_PARTS:
				{
					TPartData *pParts = GetPartData( nItemID );
					if( pParts ) nDurability = pParts->nDurability;
				}
				break;
		}
	}
	return nDurability;
}

int CDNGameDataManager::GetItemDurabilityRepairCoin(int nItemID)
{
	int nPrice = 0;

	TItemData *pItemData = GetItemData( nItemID );

	if( pItemData ) {
		switch( pItemData->nType ) {
			case ITEMTYPE_WEAPON:
				{
					TWeaponData *pWeapon = GetWeaponData( nItemID );
					if( pWeapon ) nPrice = pWeapon->nDurabilityRepairCoin;
				}
				break;
			case ITEMTYPE_PARTS:
				{
					TPartData *pParts = GetPartData( nItemID );
					if( pParts ) nPrice = pParts->nDurabilityRepairCoin;
				}
				break;
		}
	}
	return nPrice;
}

int CDNGameDataManager::GetExp(int nClass, BYTE cLevel)
{
	if ((cLevel <= 0) ||(cLevel > CHARLEVELMAX)) return 0;

	return CPlayerLevelTable::GetInstance().GetValue( nClass, cLevel, CPlayerLevelTable::Experience );
}

int CDNGameDataManager::GetFatigue(int nClass, char cLevel)
{
	if ((cLevel <= 0) ||(cLevel > CHARLEVELMAX)) return 0;

	return CPlayerLevelTable::GetInstance().GetValue( nClass, cLevel, CPlayerLevelTable::Fatigue );
}

int CDNGameDataManager::GetWeeklyFatigue(int nClass, char cLevel)
{
	if ((cLevel <= 0) ||(cLevel > CHARLEVELMAX)) return 0;
	return CPlayerLevelTable::GetInstance().GetValue( nClass, cLevel, CPlayerLevelTable::WeekFatigue );
}

BYTE CDNGameDataManager::GetLevel(int nClass, int nCurExp)
{
	if (nCurExp <= 0) return 0;

	int nValue = 0, nLevel = 0;
	int nExp;
	for (int i = 0; i < CHARLEVELMAX; i++){
		nExp = CPlayerLevelTable::GetInstance().GetValue( nClass, i + 1, CPlayerLevelTable::Experience );
		if( nExp <= nCurExp ) nLevel = i + 1;
		else break;
	}

	return nLevel;
}

#if defined(PRE_ADD_VIP)
int CDNGameDataManager::GetVIPExp(int nClass, int nLevel)
{
	if ((nLevel <= 0) ||(nLevel > CHARLEVELMAX)) return 0;
	return CPlayerLevelTable::GetInstance().GetValue( nClass, nLevel, CPlayerLevelTable::VIPExp );
}

int CDNGameDataManager::GetVIPFatigue(int nClass, int nLevel)
{
	if ((nLevel <= 0) ||(nLevel > CHARLEVELMAX)) return 0;
	return CPlayerLevelTable::GetInstance().GetValue( nClass, nLevel, CPlayerLevelTable::VIPFatigue );
}

#endif	// #if defined(PRE_ADD_VIP)



//---------------------------------------------------------------------------------
// CoinData(TCoinCountData)
//---------------------------------------------------------------------------------

bool CDNGameDataManager::LoadCoinCount()
{
#ifdef _WORK
	DNTableFileFormat *pSox;
	if (m_bAllLoaded)
		pSox = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TREBIRTHCOIN );
	else
		pSox = GetDNTable( CDnTableDB::TREBIRTHCOIN );
#else		//#ifdef _WORK
	DNTableFileFormat *pSox = GetDNTable( CDnTableDB::TREBIRTHCOIN );
#endif		//#ifdef _WORK
	if (!pSox){
		g_Log.Log(LogType::_FILELOG, L"ext(CoinTable.dnt) failed\r\n");
		return false;
	}

	if (pSox->GetItemCount() <= 0){
		g_Log.Log(LogType::_FILELOG, L"CoinTable.dnt Count(%d)\r\n", pSox->GetItemCount());
		return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		SAFE_DELETE_PMAP( TMapCoinCount, m_pMapCoinCount );
	}
#endif		//#ifdef _WORK

	char szTemp[MAX_PATH];

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{
		std::vector<int>	_WorldCoin;
		std::vector<int>	_WorldCashCoin;
		int					_CoinLimit;
		int					_CashCoinLimit;
#if defined(PRE_ADD_VIP)
		int					_VIPCoin;
#endif	// #if defined(PRE_ADD_VIP)
	};

	TempFieldNum sFieldNum;
	sFieldNum._WorldCoin.reserve( WORLDCOUNTMAX );
	sFieldNum._WorldCashCoin.reserve( WORLDCOUNTMAX );
	for( int j=0 ; j<WORLDCOUNTMAX ; ++j )
	{
		sprintf_s( szTemp, "_World%dCoin", j+1);
		sFieldNum._WorldCoin.push_back( pSox->GetFieldNum(szTemp) );
		sprintf_s( szTemp, "_World%dCashCoin", j+1);
		sFieldNum._WorldCashCoin.push_back( pSox->GetFieldNum(szTemp) );
	}
	sFieldNum._CoinLimit		= pSox->GetFieldNum( "_CoinLimit" );
	sFieldNum._CashCoinLimit	= pSox->GetFieldNum( "_CashCoinLimit" );
#if defined(PRE_ADD_VIP)
	sFieldNum._VIPCoin			= pSox->GetFieldNum( "_VIPCoin" );
#endif	// #if defined(PRE_ADD_VIP)

	//##################################################################
	// Load
	//##################################################################


	for( int i=0 ; i<pSox->GetItemCount() ; ++i)
	{
		TCoinCountData* pCoinCount = new TCoinCountData;
		memset(pCoinCount, 0, sizeof(TCoinCountData));

		int nIndex = pSox->GetItemID(i);
		pCoinCount->cLevel = nIndex;

		int iIdx = pSox->GetIDXprimary( nIndex );

		// CoinCount
		for (int j = 0; j < WORLDCOUNTMAX; j++){
			pCoinCount->nRebirthCoin[j] = pSox->GetFieldFromLablePtr( iIdx, sFieldNum._WorldCoin[j] )->GetInteger();
			pCoinCount->nCashRebirthCoin[j] = pSox->GetFieldFromLablePtr( iIdx, sFieldNum._WorldCashCoin[j] )->GetInteger();
		}
		pCoinCount->nRebirthCoinLimit = pSox->GetFieldFromLablePtr( iIdx, sFieldNum._CoinLimit )->GetInteger();
		pCoinCount->nCashRebirthCoinLimit = pSox->GetFieldFromLablePtr( iIdx, sFieldNum._CashCoinLimit )->GetInteger();
#if defined(PRE_ADD_VIP)
		pCoinCount->nVIPRebirthCoin = pSox->GetFieldFromLablePtr( iIdx, sFieldNum._VIPCoin )->GetInteger();
#endif	// #if defined(PRE_ADD_VIP)

		m_pMapCoinCount.insert(make_pair(pCoinCount->cLevel, pCoinCount));
	}

	return true;
}

int CDNGameDataManager::GetCoinCount(BYTE cLevel, char cWorldID)
{
	if ((cLevel <= 0) ||(cLevel > CHARLEVELMAX)) return -1;
	if ((cWorldID <= 0) ||(cWorldID > WORLDCOUNTMAX)) return -1;

	TMapCoinCount::iterator iter = m_pMapCoinCount.find(cLevel);
	if (iter != m_pMapCoinCount.end()){
		return iter->second->nRebirthCoin[cWorldID - 1];
	}

	return 0;
}

int CDNGameDataManager::GetCashCoinCount(BYTE cLevel, char cWorldID)
{
	if ((cLevel <= 0) ||(cLevel > CHARLEVELMAX)) return -1;
	if ((cWorldID <= 0) ||(cWorldID > WORLDCOUNTMAX)) return -1;

	TMapCoinCount::iterator iter = m_pMapCoinCount.find(cLevel);
	if (iter != m_pMapCoinCount.end()){
		return iter->second->nCashRebirthCoin[cWorldID - 1];
	}

	return 0;
}

#if defined(PRE_ADD_VIP)
int CDNGameDataManager::GetVIPRebirthCoinCount(BYTE cLevel)
{
	if ((cLevel <= 0) ||(cLevel > CHARLEVELMAX)) return -1;

	TMapCoinCount::iterator iter = m_pMapCoinCount.find(cLevel);
	if (iter != m_pMapCoinCount.end()){
		return iter->second->nVIPRebirthCoin;
	}

	return 0;
}

#endif	// #if defined(PRE_ADD_VIP)

int CDNGameDataManager::GetCoinLimit(BYTE cLevel)
{
	if ((cLevel <= 0) ||(cLevel > CHARLEVELMAX)) return -1;

	TMapCoinCount::iterator iter = m_pMapCoinCount.find(cLevel);
	if (iter != m_pMapCoinCount.end()){
		return iter->second->nRebirthCoinLimit;
	}

	return 0;
}

int CDNGameDataManager::GetCashCoinLimit(BYTE cLevel)
{
	if ((cLevel <= 0) ||(cLevel > CHARLEVELMAX)) return -1;

	TMapCoinCount::iterator iter = m_pMapCoinCount.find(cLevel);
	if (iter != m_pMapCoinCount.end()){
		return iter->second->nCashRebirthCoinLimit;
	}

	return 0;
}

//---------------------------------------------------------------------------------
// NpcTalk
//---------------------------------------------------------------------------------
bool CDNGameDataManager::ReadTalkFile(const char* szFolderName)
{
	g_Log.Log(LogType::_FILELOG, "%s...Folder : %s\n", szFolderName, CEtResourceMng::GetInstance().GetFullPath( szFolderName ).c_str());

	std::vector<CFileNameString> FileList;
	CEtResourceMng::GetInstance().FindFileListAll_IgnoreExistFile( szFolderName, "*.xml", FileList );

	for( size_t i = 0 ; i < FileList.size() ; i++ )
	{
		WCHAR wszFile[2048] = {0,};
		if ((int)FileList[i].size() >= 2048)
			_ASSERT(0);
		MultiByteToWideChar(CP_ACP, 0, FileList[i].c_str(), -1, wszFile, (int)FileList[i].size() );

		// 20100624 파일전체경로에서이름만추출하도록추가(b4nfter)
		{
			TCHAR szFileName[MAX_PATH] = { _T('\0'), };
			TCHAR szExecName[MAX_PATH] = { _T('\0'), };

			errno_t nRetVal = _wsplitpath_s(wszFile, NULL, 0, NULL, 0, szFileName, _countof(szFileName), szExecName, _countof(szExecName));
			if (nRetVal) {
				// 오류발생
				g_Log.Log( LogType::_FILELOG, L"%s(%s) failed\r\n", szFolderName, szFileName);
				return false;
			}

			_sntprintf_s(wszFile, _countof(wszFile), _T("%s%s"), szFileName, szExecName);
		}

		AddTalk(wszFile);
	}

	g_Log.Log( LogType::_FILELOG, "%s...Size : %d\n", szFolderName, (int)FileList.size());

	return true;
}

bool CDNGameDataManager::LoadTalkData()
{
#if defined (_WORK) && defined (PRE_ADD_SERVER_LOAD_SHORTENING)
	if (g_ConfigWork.HasCommand(L"ExceptScript"))
		return true;
#endif // #if defined (_WORK) && defined (PRE_ADD_SERVER_LOAD_SHORTENING)

	m_XMLFileMap.clear();
	m_XMLIndexMap.clear();

	// 맨처음호출되는start 인덱스는수동으로등록
	std::wstring _szStart(L"start");
	DWORD nHashCode = GetStringHashCode(_szStart.c_str());
	m_XMLIndexMap.insert( make_pair(nHashCode, _szStart ) );

	m_TalkMap.clear();
	
	if (!ReadTalkFile ("Talk_Npc"))
		return false;

	if (!ReadTalkFile ("Talk_Quest"))
		return false;

#if defined (_WORK)
	if (!ReadTalkFile ("ignore_npc"))
		return false;

	if (!ReadTalkFile ("ignore_quest"))
		return false;
#endif

	return true;
}

bool CDNGameDataManager::AddTalk(const WCHAR* _wszTalkFileName)
{
	TALK_PARAGRAPH_MAP TalkParagraphMap;
	TALK_ANSWER_SET TalkAnswerSet;

	std::wstring wszTalkFileName;
	wszTalkFileName = _wszTalkFileName;

	std::transform(wszTalkFileName.begin(), wszTalkFileName.end(), wszTalkFileName.begin(), towlower); 

	// 이미 존재하면
	if( GetTalk(wszTalkFileName.c_str(), TalkParagraphMap) == true ) 
		return true;

	char szTalkFileName[2048] = { 0,} ;
	WideCharToMultiByte( CP_ACP, 0, _wszTalkFileName, -1, szTalkFileName, 1024, NULL, NULL );
	
	CFileStream Stream( CEtResourceMng::GetInstance().GetFullName(szTalkFileName).c_str(), CFileStream::OPEN_READ );
	if( Stream.IsValid() == false )
	{
		g_Log.Log(LogType::_ERROR, L"Cannot open file : %s\n" , wszTalkFileName);
		return false;
	}

	// 파일명으로 해쉬코드를 만들어서 맵에 저장해놓는다.
	std::wstring __wsz(_wszTalkFileName);
	ToLowerW(__wsz);
	DWORD nHashCode = GetStringHashCode(__wsz.c_str());
	TXMLFileMap::iterator it = m_XMLFileMap.find(nHashCode);
	if( it != m_XMLFileMap.end() )
	{
		g_Log.Log(LogType::_FILELOG, L"Already added hash code. code : %u file : %s\n" , nHashCode, __wsz.c_str());
		return false;
	}
	m_XMLFileMap.insert( make_pair(nHashCode, __wsz));

	int nLen = Stream.Size();

	int nDummy = 256;
	char* pBuffer = new char[nLen+nDummy];
	ZeroMemory(pBuffer, nLen+nDummy);

	class __Temp
	{
	public:
		__Temp(char* p) { m_p = p; }
		~__Temp() 
		{
			if( m_p )
				delete[] m_p; 
			m_p = NULL; 
		}
		char* m_p;
	};
	__Temp ScopeDelete(pBuffer);


	int nReadSize = Stream.Read(pBuffer, nLen);

	TiXmlDocument doc;
	doc.ClearError();
	
	doc.Parse(pBuffer, 0, TIXML_ENCODING_UTF8 );
	if( doc.Error() )
	{
		std::wstring msg;
		msg = L"xml 파일 파싱 실패 : ";
		msg += wszTalkFileName;
		msg += L" error msg : ";
		const char* errmsg = doc.ErrorDesc();
		WCHAR buff[1024] = {0,};
		MultiByteToWideChar(CP_ACP, 0, errmsg, -1, buff, 512 );
		msg += buff;
		g_Log.Log(LogType::_FILELOG, L"%s", msg.c_str());

		return false;
	}

	TiXmlElement* pElement;
	pElement = doc.RootElement();

	if( !pElement )
	{
		std::wstring msg;
		msg = L"xml 파일 파싱 실패. root element 없음";
		msg += wszTalkFileName;
		msg += L" error msg : ";
		const char* errmsg = doc.ErrorDesc();
		WCHAR buff[1024] = {0,};
		MultiByteToWideChar(CP_ACP, 0, errmsg, -1, buff, 512 );
		msg += buff;
		g_Log.Log(LogType::_FILELOG, L"%s", msg.c_str());
		return false;
	}
	TiXmlNode* pNode = pElement->FirstChild("talk_paragraph");

	if( !pNode )		return false;

	pElement = pNode->ToElement();

	if(!pElement) return false;

	const int ___BUF_SIZE = 4096;
	WCHAR buff[___BUF_SIZE] = L"";
	const char* sz = NULL;

	for( pElement ; pElement != NULL ; pElement = pElement->NextSiblingElement() )
	{
		sz = pElement->Value();
		ZeroMemory(&buff, sizeof(WCHAR)*___BUF_SIZE);
		MultiByteToWideChar(CP_UTF8, 0, sz, -1, buff, ___BUF_SIZE );
		std::wstring szString(buff);

		// question
		if( szString != L"talk_paragraph" )
		{
			continue;
		}

		TALK_PARAGRAPH talk;

		const char* szTalkIndex = pElement->Attribute("index");


		ZeroMemory(&buff, sizeof(WCHAR)*___BUF_SIZE);
		MultiByteToWideChar(CP_UTF8, 0, szTalkIndex, -1, buff, ___BUF_SIZE );


		talk.szIndex = buff;

		TiXmlElement* pTalkElement = pElement->FirstChildElement();

		for( pTalkElement ; pTalkElement != NULL ; pTalkElement = pTalkElement->NextSiblingElement() )
		{
			sz = pTalkElement->Value();
			ZeroMemory(&buff, sizeof(WCHAR)*___BUF_SIZE);
			MultiByteToWideChar(CP_UTF8, 0, sz, -1, buff, ___BUF_SIZE );
			std::wstring szLocalString(buff);

			// question
			if( szLocalString == L"talk_question" )
			{
				sz = pTalkElement->GetText();
				ZeroMemory(&buff, sizeof(WCHAR)*___BUF_SIZE);
				MultiByteToWideChar(CP_UTF8, 0, sz, -1, buff, ___BUF_SIZE );
				talk.Question.szQuestion = buff;

			}
			// talk_answer
			else if( szLocalString == L"talk_answer" )
			{
				TALK_ANSWER answer;
				sz = pTalkElement->Attribute("link_index");
				ZeroMemory(&buff, sizeof(WCHAR)*___BUF_SIZE);
				MultiByteToWideChar(CP_UTF8, 0, sz, -1, buff, ___BUF_SIZE );
				answer.szLinkIndex = buff;

				// 각 인덱별로 해쉬코드를 만들어 둔다.
				std::wstring _wsz;
				_wsz = answer.szLinkIndex;
				ToLowerW(_wsz);
				DWORD nLocalHashCode = GetStringHashCode(_wsz.c_str());
				m_XMLIndexMap.insert( make_pair(nLocalHashCode, _wsz) );


				sz = pTalkElement->Attribute("link_target");
				ZeroMemory(&buff, sizeof(WCHAR)*___BUF_SIZE);
				MultiByteToWideChar(CP_UTF8, 0, sz, -1, buff, ___BUF_SIZE );
				answer.szLinkTarget = buff;

				if( answer.szLinkTarget.empty() )
				{
					answer.szLinkTarget = wszTalkFileName;
					answer.bOtherTargetLink = false;
				}
				else if( answer.szLinkTarget != wszTalkFileName )
				{
					answer.bOtherTargetLink = true;
				}
				else
				{
					answer.bOtherTargetLink = true;
				}

				sz = pTalkElement->GetText();
				ZeroMemory(&buff, sizeof(WCHAR)*___BUF_SIZE);
				MultiByteToWideChar(CP_UTF8, 0, sz, -1, buff, ___BUF_SIZE );
				answer.szAnswer = buff;

				talk.Answers.push_back(answer);
				TalkAnswerSet.insert(_wsz);
			}
			else
			{
				//return false;
			}
		}

		TALK_PARAGRAPH_MAP_IT iter = TalkParagraphMap.find(talk.szIndex);
		if( iter != TalkParagraphMap.end() )
		{
			return false;
		}
		else
		{
			TalkParagraphMap.insert(make_pair(talk.szIndex, talk));

			// 각 인덱별로 해쉬코드를 만들어 둔다.
			ToLowerW(talk.szIndex);
			DWORD nLocalHashCode = GetStringHashCode(talk.szIndex.c_str());
			m_XMLIndexMap.insert( make_pair(nLocalHashCode, talk.szIndex) );
		}
	}

	CDNQuest* pQuest = g_pQuestManager->GetQuest(std::wstring(wszTalkFileName));
	if( pQuest )
	{
		int nQuestID = pQuest->GetQuestInfo().nQuestIndex;

		TiXmlElement* pRoot = doc.RootElement();

		TiXmlNode* pLocalNode = pRoot->FirstChild("journal_data");

		if( !pLocalNode )		
			return false;

		TiXmlElement* pLocalElement = pLocalNode->ToElement();

		if(!pLocalElement)
			return false;

//		const int ___BUF_SIZE = 4096;
		WCHAR Localbuff[___BUF_SIZE] = {0,};
		const char* Localsz = NULL;

		Localsz = pLocalElement->Attribute("use_item_list");
		ZeroMemory(&Localbuff, sizeof(WCHAR)*___BUF_SIZE);
		MultiByteToWideChar(CP_UTF8, 0, Localsz, -1, Localbuff, ___BUF_SIZE );
		std::wstring szList = Localbuff;

		std::vector<std::wstring> tokens;
		TokenizeW(szList, tokens, std::wstring(L",") );

		for( size_t i = 0 ; i < tokens.size() ; i++ )
		{
			int nItemId = _wtoi(tokens[i].c_str());

			TMapQuestItemDataIt Localit = m_QuestItemData.find(nQuestID);
			if( Localit == m_QuestItemData.end() )
			{
				std::set<int> questItemSet;
				questItemSet.insert(nItemId);

				m_QuestItemData.insert(make_pair(nQuestID, questItemSet));
			}
			else
			{
				std::set<int>& questItemSet = Localit->second;
				questItemSet.insert(nItemId);
			}
		}
	}

	// Load "Condition_data"
	if( pQuest )
	{
		int nQuestID = pQuest->GetQuestInfo().nQuestIndex;
		TiXmlElement* pRoot = doc.RootElement();

		LoadQuestCondition( pRoot, nQuestID );
	}

	m_TalkMap.insert(make_pair(std::wstring(wszTalkFileName), TalkParagraphMap));
	m_TalkAnswerMap.insert(make_pair(std::wstring(wszTalkFileName), TalkAnswerSet));

	return true;
}

void CDNGameDataManager::LoadQuestCondition(TiXmlElement* pRootElement, UINT nQuestID)
{
	TiXmlNode* pNode = pRootElement->FirstChild("condition_data");

	if( !pNode )		return;


	TiXmlElement* pElement = pNode->ToElement();

	if(!pElement) return;

	const int ___BUF_SIZE = 4096;
	WCHAR buff[___BUF_SIZE] = L"";
	const char* sz = NULL;

	QuestCondition* pQuestCondition = new QuestCondition;

	pQuestCondition->nQuestIndex = nQuestID;

	TiXmlElement* pChildElement = pElement->FirstChildElement();

	for( pChildElement ; pChildElement != NULL ; pChildElement = pChildElement->NextSiblingElement() )
	{
		sz = pChildElement->Value();
		ZeroMemory(&buff, sizeof(WCHAR)*___BUF_SIZE);
		MultiByteToWideChar(CP_UTF8, 0, sz, -1, buff, ___BUF_SIZE );
		std::wstring szString(buff);

		// Quest Item
		if( szString == L"have_normal_item" )
		{
			sz = pChildElement->GetText();
			ZeroMemory(&buff, sizeof(WCHAR)*___BUF_SIZE);
			MultiByteToWideChar(CP_UTF8, 0, sz, -1, buff, ___BUF_SIZE );

			__HaveNormalItem* pConBase = new __HaveNormalItem;
			pConBase->Init(std::wstring(buff));
			pQuestCondition->ConditionList.push_back(pConBase);
		}
		// 호감도
		else if( szString == L"npc_favor" )
		{
			sz = pChildElement->GetText();
			ZeroMemory(&buff, sizeof(WCHAR)*___BUF_SIZE);
			MultiByteToWideChar(CP_UTF8, 0, sz, -1, buff, ___BUF_SIZE );

			__Reputation_Favor* pConBase = new __Reputation_Favor;
			pConBase->Init(std::wstring(buff));
			pQuestCondition->ConditionList.push_back(pConBase);
		}
		// 비호감도
		else if( szString == L"npc_malice" )
		{
			sz = pChildElement->GetText();
			ZeroMemory(&buff, sizeof(WCHAR)*___BUF_SIZE);
			MultiByteToWideChar(CP_UTF8, 0, sz, -1, buff, ___BUF_SIZE );

			__Reputation_Hatred* pConBase = new __Reputation_Hatred;
			pConBase->Init(std::wstring(buff));
			pQuestCondition->ConditionList.push_back(pConBase);
		}
		// user_level
		else if( szString == L"user_level" )
		{
			sz = pChildElement->GetText();
			ZeroMemory(&buff, sizeof(WCHAR)*___BUF_SIZE);
			MultiByteToWideChar(CP_UTF8, 0, sz, -1, buff, ___BUF_SIZE );

			__UserLevel* pConBase = new __UserLevel;
			pConBase->Init(std::wstring(buff));
			pQuestCondition->ConditionList.push_back(pConBase);
		}
		else if( szString == L"user_class" )
		{
			sz = pChildElement->GetText();
			ZeroMemory(&buff, sizeof(WCHAR)*___BUF_SIZE);
			MultiByteToWideChar(CP_UTF8, 0, sz, -1, buff, ___BUF_SIZE );

			__UserClass* pConBase = new __UserClass;
			pConBase->Init(std::wstring(buff));
			pQuestCondition->ConditionList.push_back(pConBase);
		}
		// prev_quest
		else if( szString == L"prev_quest" )
		{
			sz = pChildElement->GetText();
			ZeroMemory(&buff, sizeof(WCHAR)*___BUF_SIZE);
			MultiByteToWideChar(CP_UTF8, 0, sz, -1, buff, ___BUF_SIZE );

			__PrevQuest* pConBase = new __PrevQuest;
			pConBase->Init(std::wstring(buff));
			pQuestCondition->ConditionList.push_back(pConBase);
		}
		// have_normal_item
		else if( szString == L"have_normal_item" )
		{
			sz = pChildElement->GetText();
			ZeroMemory(&buff, sizeof(WCHAR)*___BUF_SIZE);
			MultiByteToWideChar(CP_UTF8, 0, sz, -1, buff, ___BUF_SIZE );

			__HaveNormalItem* pConBase = new __HaveNormalItem;
			pConBase->Init(std::wstring(buff));
			pQuestCondition->ConditionList.push_back(pConBase);
		}
#if defined(PRE_ADD_QUEST_CHECKCAHEITEM)
		// have_cash_item
		else if( szString == L"have_cash_item" )
		{
			sz = pChildElement->GetText();
			ZeroMemory(&buff, sizeof(WCHAR)*___BUF_SIZE);
			MultiByteToWideChar(CP_UTF8, 0, sz, -1, buff, ___BUF_SIZE );

			__HaveCashItem* pConBase = new __HaveCashItem;
			pConBase->Init(std::wstring(buff));
			pQuestCondition->ConditionList.push_back(pConBase);
		}
#endif
		// have_symbol_item
		//else if( szString == L"have_symbol_item" )
		//{
		//	sz = pChildElement->GetText();
		//	ZeroMemory(&buff, sizeof(WCHAR)*___BUF_SIZE);
		//	MultiByteToWideChar(CP_UTF8, 0, sz, -1, buff, ___BUF_SIZE );

		//	__HaveSymbolItem* pConBase = new __HaveSymbolItem;
		//	pConBase->Init(std::wstring(buff));
		//	pQuestCondition->ConditionList.push_back(pConBase);
		//}
		else if( szString == L"complete_mission" )
		{
			sz = pChildElement->GetText();
			ZeroMemory(&buff, sizeof(WCHAR)*___BUF_SIZE);
			MultiByteToWideChar(CP_UTF8, 0, sz, -1, buff, ___BUF_SIZE );

			__CompleteMission* pConBase = new __CompleteMission;
			pConBase->Init(std::wstring(buff));
			pQuestCondition->ConditionList.push_back(pConBase);
		}
		else
		{

		}
	}

	m_QuestConditionData.insert( make_pair(pQuestCondition->nQuestIndex, pQuestCondition ));
}

QuestCondition * CDNGameDataManager::GetQuestCondition(UINT nQuestID)
{
	TMapQuestConditionDataIt iter = m_QuestConditionData.find( nQuestID );
	if( iter != m_QuestConditionData.end() )
		return iter->second;

	return NULL;
}

bool CDNGameDataManager::RemoveTalk(const WCHAR* _wszTalkFileName)
{
	std::wstring wszTalkFileName(_wszTalkFileName);
	std::transform(wszTalkFileName.begin(), wszTalkFileName.end(), wszTalkFileName.begin(), towlower); 

	TALK_MAP_IT iter = m_TalkMap.find( wszTalkFileName );
	if( iter != m_TalkMap.end() )
	{
		m_TalkMap.erase( iter );
		return true;
	}

	return false;
}

bool CDNGameDataManager::GetTalk(const WCHAR* _wszTalkFileName, OUT TALK_PARAGRAPH_MAP& TalkParagraphMap )
{
	std::wstring wszTalkFileName(_wszTalkFileName);
	std::transform(wszTalkFileName.begin(), wszTalkFileName.end(), wszTalkFileName.begin(), towlower); 

	TALK_MAP_IT iter = m_TalkMap.find(	wszTalkFileName );
	if( iter != m_TalkMap.end() )
	{
		TalkParagraphMap = iter->second;
		return true;
	}

	return false;
}

bool CDNGameDataManager::GetTalkParagraph(IN std::wstring& wszIndex, IN std::wstring& wszTarget, OUT TALK_PARAGRAPH& talk )
{
	TALK_MAP_IT iter = m_TalkMap.find(wszTarget);
	if( iter == m_TalkMap.end() )
	{
		return false;
	}

	TALK_PARAGRAPH_MAP& paragraph_map = iter->second;
	TALK_PARAGRAPH_MAP_IT it = paragraph_map.find(wszIndex);
	if( it == paragraph_map.end() )
	{
		return false;
	}

	talk = it->second;
	return true;
}

TALK_PARAGRAPH & CDNGameDataManager::GetEmptyTalkParagraph()
{
	static TALK_PARAGRAPH sEmptyPara;
	return sEmptyPara;
}

void CDNGameDataManager::GetTalkFileList(IN OUT std::vector<std::wstring>& TalkFileList)
{
	TalkFileList.clear();
	TalkFileList.reserve(m_TalkMap.size()+1);
	TALK_MAP_IT iter = m_TalkMap.begin();

	for( ; iter != m_TalkMap.end() ; iter++ )
	{
		TalkFileList.push_back( iter->first );
	}
}

DWORD CDNGameDataManager::GetStringHashCode(const WCHAR* pString)
{
	std::wstring __wsz(pString);
	ToLowerW(__wsz);
	const WCHAR* pChar = __wsz.c_str();

	DWORD ch;
	DWORD len = (DWORD)wcslen( pChar );
	DWORD result = 5381;
	for( DWORD i = 0 ; i < len ; i++ )
	{
		ch = (unsigned long)pChar[i];
		result = ((result<< 5) + result) + ch; // hash * 33 + ch
	}	  
	return result;
}

bool CDNGameDataManager::GetTalkFileName(DWORD nHashCode, OUT std::wstring& wszIndex)
{
	TXMLFileMap::iterator it = m_XMLFileMap.find(nHashCode);
	if( it ==  m_XMLFileMap.end() )
	{
		wszIndex = L"Error";
		return false;
	}

	wszIndex = it->second;
	return true;
}

bool CDNGameDataManager::GetTalkIndexName(DWORD nHashCode, OUT std::wstring& wszIndex)
{
	TXMLIndexMap::iterator it = m_XMLIndexMap.find(nHashCode);
	if( it ==  m_XMLIndexMap.end() )
	{
		wszIndex = L"Error";
		return false;
	}
	wszIndex = it->second;
	return true;
}

bool CDNGameDataManager::CheckTalkAnswer(IN std::wstring& wszIndex, IN std::wstring& wszTarget)
{
	TALK_ANSWER_MAP_IT iter = m_TalkAnswerMap.find(wszTarget);
	if(m_TalkAnswerMap.end() == iter) {
		return false;
	}

	TALK_ANSWER_SET& answer_set = iter->second;
	TALK_ANSWER_SET_IT it = answer_set.find(wszIndex);
	if(answer_set.end() == it) {
		return false;
	}

	return true;
}

//---------------------------------------------------------------------------------
// ShopData(ShopTable.dnt - TShopData)
//---------------------------------------------------------------------------------

bool CDNGameDataManager::LoadShopData()
{
#ifdef _WORK
	DNTableFileFormat *pSox;
	if (m_bAllLoaded)
		pSox = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TSHOP );
	else
		pSox = GetDNTable( CDnTableDB::TSHOP );
#else		//#ifdef _WORK
	DNTableFileFormat *pSox = GetDNTable( CDnTableDB::TSHOP );
#endif		//#ifdef _WORK
	if (!pSox){
		g_Log.Log( LogType::_FILELOG, L"ShopTable.dnt failed\r\n");
		return false;
	}

	if (pSox->GetItemCount() <= 0){
		g_Log.Log( LogType::_FILELOG, L"ShopTable.dnt Count(%d)\r\n", pSox->GetItemCount());
		return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		SAFE_DELETE_PMAP( TMapShopData, m_pShopData );
	}
#endif		//#ifdef _WORK

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{
		int _nShopID;
		int _nTabID;
		std::vector<int>	_ItemIndex;
		std::vector<int>	_Quantity;
	};

	TempFieldNum sFieldNum;
	sFieldNum._nShopID = pSox->GetFieldNum( "_ShopID" );
	sFieldNum._nTabID = pSox->GetFieldNum( "_TabID" );
	sFieldNum._ItemIndex.reserve( SHOPITEMMAX );
	sFieldNum._Quantity.reserve( SHOPITEMMAX );

	char szTemp[MAX_PATH];
	for( int j=0 ; j<SHOPITEMMAX ; ++j )
	{
		sprintf( szTemp, "_itemindex%d", j+1 );
		sFieldNum._ItemIndex.push_back( pSox->GetFieldNum(szTemp) );
		sprintf( szTemp, "_Quantity%d", j+1);
		sFieldNum._Quantity.push_back( pSox->GetFieldNum(szTemp) );
	}

	//##################################################################
	// Load
	//##################################################################

	//2010.10.7 haling STL 보호 위해 변환
	TShopTabData ShopTabData;
	TShopItem ShopItem = { 0, };
	vector<int> vShopTabList;

	std::vector<int> vAbuseItemID;

	for( int i=0 ; i<pSox->GetItemCount() ; ++i )
	{
		int nItemID = pSox->GetItemID(i);
		int nIdx = pSox->GetIDXprimary(nItemID);

		int nShopID = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._nShopID)->GetInteger();
		if (nShopID <= 0) continue;

		TShopData *pShopData = GetShopData(nShopID);
		if (!pShopData){
			pShopData = new TShopData;
			//2010.10.7 haling STL 보호 위해 생성자로 변환
			//memset(pShopData, 0, sizeof(TShopData));
			pShopData->nShopID = nShopID;
		}

		TItemData *pItemData = NULL;

		//memset(&ShopTabData, 0, sizeof(TShopTabData));
		ShopTabData.Clear();
		ShopTabData.nTabID = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._nTabID)->GetInteger();
		if (ShopTabData.nTabID <= 0) continue;

		ShopTabData.vShopItem.clear();
		ShopTabData.vShopItem.resize(SHOPITEMMAX);
		memset(&ShopItem, 0, sizeof(TShopItem));
		for (int j = 0; j < SHOPITEMMAX; ++j){
			ShopItem.nItemID = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._ItemIndex[j])->GetInteger();
			if (ShopItem.nItemID <= 0) continue;
			ShopItem.nCount = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._Quantity[j])->GetInteger();
			pItemData = GetItemData(ShopItem.nItemID);
			if (!pItemData) continue;
			ShopItem.nMaxCount = pItemData->nOverlapCount;
			ShopItem.nPrice = pItemData->nAmount;

			ShopTabData.vShopItem[j] = ShopItem;
			if (pItemData->nAmount > 0)
			{
				int nItemBuyPrice = pItemData->nAmount - (int)(pItemData->nAmount * MAX_ITEMPRICE_RATE);
				if (pItemData->nSellAmount > nItemBuyPrice)
					vAbuseItemID.push_back(nItemID);
			}
		}

		pShopData->vShopTabData.push_back(ShopTabData);

		m_pShopData[nShopID] = pShopData;
	}

	if (vAbuseItemID.size() > 0)
	{
		WCHAR szTemp[1024];
		std::wstring szError = L"Invalid ShopTable\n\n";

		for(UINT i=0; i<vAbuseItemID.size(); i++)
		{
			swprintf_s( szTemp, L"ItemID : %d\n", vAbuseItemID[i] );
			szError += szTemp;
		}

		MessageBox( NULL, szError.c_str(), L"Critical Error!!", MB_OK );
		return false;
	}

	return true;
}

TShopData* CDNGameDataManager::GetShopData(int nShopID)
{
	if (nShopID <= 0) return NULL;

	TMapShopData::iterator iter = m_pShopData.find(nShopID);
	if (iter != m_pShopData.end()){
		return iter->second;
	}

	return NULL;
}

TShopItem* CDNGameDataManager::GetShopItem(int nShopID, int nShopTabID, int nIndex)
{
	if (nShopID <= 0) 
		return NULL;
	if ((nShopTabID <= 0) || (nShopTabID > SHOPTABMAX)) 
		return NULL;
	if (nIndex >= SHOPITEMMAX) 
		return NULL;

	TShopData *pShopData = GetShopData(nShopID);
	if (!pShopData) return NULL;

	for (int i = 0; i <(int)pShopData->vShopTabData.size(); i++){
		if (pShopData->vShopTabData[i].nTabID == nShopTabID)
			return &(pShopData->vShopTabData[i].vShopItem[nIndex]);
	}

	return NULL;
}

//---------------------------------------------------------------------------------
// SkillShopData(SkillShopTable.dnt - TSkillShopData)
//---------------------------------------------------------------------------------

bool CDNGameDataManager::LoadSkillShopData()
{
#ifdef _WORK
	DNTableFileFormat *pSox;
	if (m_bAllLoaded)
		pSox = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TSKILLSHOP );
	else
		pSox = GetDNTable( CDnTableDB::TSKILLSHOP );
#else		//#ifdef _WORK
	DNTableFileFormat *pSox = GetDNTable( CDnTableDB::TSKILLSHOP );
#endif		//#ifdef _WORK
	if (!pSox){
		g_Log.Log( LogType::_FILELOG, L"SkillShopTable.dnt failed\r\n");
		return false;
	}

	if (pSox->GetItemCount() <= 0){
		g_Log.Log( LogType::_FILELOG, L"SkillShopTable.dnt Count(%d)\r\n", pSox->GetItemCount());
		return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		SAFE_DELETE_PMAP( TMapSkillShopData, m_pSkillShop );
	}
#endif		//#ifdef _WORK

	char szTemp[MAX_PATH];

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{
		std::vector<int>	_SkillIndex;
		std::vector<int>	_SkillCost;
	};

	TempFieldNum sFieldNum;
	sFieldNum._SkillIndex.reserve( SKILLSHOPITEMMAX );
	sFieldNum._SkillCost.reserve( SKILLSHOPITEMMAX );

	for( int j=0 ; j<SKILLSHOPITEMMAX ; ++j )
	{
		sprintf( szTemp, "_SkillIndex%d", j+1);
		sFieldNum._SkillIndex.push_back( pSox->GetFieldNum(szTemp) );

		sprintf( szTemp, "_SkillCost%d", j+1);
		sFieldNum._SkillCost.push_back( pSox->GetFieldNum(szTemp) );
	}

	//##################################################################
	// Load
	//##################################################################

	for( int i=0 ; i<pSox->GetItemCount() ; ++i )
	{
		TSkillShopData* pSkillShopData = new TSkillShopData;
		memset(pSkillShopData, 0, sizeof(TSkillShopData));

		pSkillShopData->nShopID = pSox->GetItemID(i);

		int iIdx = pSox->GetIDXprimary( pSkillShopData->nShopID );
		
		for( int j=0 ; j<SKILLSHOPITEMMAX ; ++j )
		{
			pSkillShopData->SkillShopItem[j].nSkillID = pSox->GetFieldFromLablePtr( iIdx, sFieldNum._SkillIndex[j] )->GetInteger();
			if (pSkillShopData->SkillShopItem[j].nSkillID <= 0) 
				continue;

			pSkillShopData->SkillShopItem[j].nPrice = pSox->GetFieldFromLablePtr( iIdx, sFieldNum._SkillCost[j] )->GetInteger();
		}

		std::pair<TMapSkillShopData::iterator,bool> Ret = m_pSkillShop.insert(make_pair(pSkillShopData->nShopID, pSkillShopData));
		if ( Ret.second == false )
			delete pSkillShopData;
	}

	return true;
}

TSkillShopData* CDNGameDataManager::GetSkillShopData(int nShopID)
{
	if (nShopID <= 0) return NULL;

	TMapSkillShopData::iterator iter = m_pSkillShop.find(nShopID);
	if (iter != m_pSkillShop.end()){
		return iter->second;
	}

	return NULL;
}

// Skill
bool CDNGameDataManager::LoadSkillData()
{
#ifdef _WORK
	DNTableFileFormat *pSkillTable;
	if (m_bAllLoaded)
		pSkillTable = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TSKILL );
	else
		pSkillTable = GetDNTable( CDnTableDB::TSKILL );
#else		//#ifdef _WORK
	DNTableFileFormat *pSkillTable = GetDNTable( CDnTableDB::TSKILL );
#endif		//#ifdef _WORK
	if (!pSkillTable){
		g_Log.Log(LogType::_FILELOG, L"SkillTable.dnt failed\r\n");
		return false;
	}

	if (pSkillTable->GetItemCount() <= 0){
		g_Log.Log(LogType::_FILELOG, L"SkillTable.dnt Count(%d)\r\n", pSkillTable->GetItemCount());
		return false;
	}

#ifdef _WORK
	DNTableFileFormat *pSkillLevelTable;
	if (m_bAllLoaded)
		pSkillLevelTable = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TSKILLLEVEL );
	else
		pSkillLevelTable = GetDNTable( CDnTableDB::TSKILLLEVEL );
#else		//#ifdef _WORK
	DNTableFileFormat *pSkillLevelTable = GetDNTable( CDnTableDB::TSKILLLEVEL );
#endif		//#ifdef _WORK
	if (!pSkillLevelTable){
		g_Log.Log( LogType::_FILELOG, L"SkillLevelTable.dnt failed\r\n");
		return false;
	}

	if (pSkillLevelTable->GetItemCount() <= 0){
		g_Log.Log(LogType::_FILELOG, L"SkillLevelTable.dnt Count(%d)\r\n", pSkillLevelTable->GetItemCount());
		return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		SAFE_DELETE_PMAP(TMapSkillData, m_pSkillData);
		m_mapDefaultCreateData.clear();
	}
#endif		//#ifdef _WORK

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{
		int _NeedWeaponType[ 2 ];
		int	_MaxLevel;
		int	_SkillType;
		int _DurationType;
		int	_TargetType;
		int	_Dissolvable;
		int _NeedJob;
		int _Lock;
		int _UnlockSkillBookItemID;
		int _UnlockPrice;
		int _DuplicatedSkillType;
		int _GlobalCoolTime;
		int _GlobalSkillGroup;
	};

	TempFieldNum sFieldNum;
	sFieldNum._NeedWeaponType[ 0 ] = pSkillTable->GetFieldNum( "_NeedWeaponType1" );
	sFieldNum._NeedWeaponType[ 1 ] = pSkillTable->GetFieldNum( "_NeedWeaponType2" );
	sFieldNum._MaxLevel			= pSkillTable->GetFieldNum( "_MaxLevel" );
	sFieldNum._SkillType		= pSkillTable->GetFieldNum( "_SkillType" );
	sFieldNum._DurationType		= pSkillTable->GetFieldNum( "_DurationType" );
	sFieldNum._TargetType		= pSkillTable->GetFieldNum( "_TargetType" );
	sFieldNum._Dissolvable		= pSkillTable->GetFieldNum( "_Dissolvable" );
	sFieldNum._NeedJob			= pSkillTable->GetFieldNum( "_NeedJob" );
	sFieldNum._Lock				= pSkillTable->GetFieldNum( "_Lock" );
	sFieldNum._UnlockSkillBookItemID = pSkillTable->GetFieldNum( "_UnlockSkillBookItemID" );
	sFieldNum._UnlockPrice		= pSkillTable->GetFieldNum( "_UnlockPrice" );
	sFieldNum._DuplicatedSkillType = pSkillTable->GetFieldNum( "_DuplicatedSkillType" );
	sFieldNum._GlobalCoolTime	= pSkillTable->GetFieldNum( "_GlobalCoolTime" );
	sFieldNum._GlobalSkillGroup = pSkillTable->GetFieldNum( "_GlobalSkillGroup" );

	struct LevelFieldNum
	{
		int _SkillLevel;
		int	_LevelLimit;
		//int	_NeedJobClass; // 스킬테이블로 옮겨짐.
		int _DelayTime;
		int	_AddRange;
		int	_NeedItem;
		int _NeedItemDecreaseCount;
		int _DecreaseHP;
		int _DecreaseSP;
		int _NeedSkillPoint;
		int _EffectClassValue1Duration;
	};

	LevelFieldNum sLevelFieldNum;
	sLevelFieldNum._SkillLevel				= pSkillLevelTable->GetFieldNum( "_SkillLevel" );
	sLevelFieldNum._LevelLimit				= pSkillLevelTable->GetFieldNum( "_LevelLimit" );
	//sLevelFieldNum._NeedJobClass			= pSkillLevelTable->GetFieldNum( "_NeedJobClass" );
	sLevelFieldNum._DelayTime				= pSkillLevelTable->GetFieldNum( "_DelayTime" );
	sLevelFieldNum._AddRange				= pSkillLevelTable->GetFieldNum( "_AddRange" );
	sLevelFieldNum._NeedItem				= pSkillLevelTable->GetFieldNum( "_NeedItem" );
	sLevelFieldNum._NeedItemDecreaseCount	= pSkillLevelTable->GetFieldNum( "_NeedItemDecreaseCount" );
	sLevelFieldNum._DecreaseHP				= pSkillLevelTable->GetFieldNum( "_DecreaseHP" );
	sLevelFieldNum._DecreaseSP				= pSkillLevelTable->GetFieldNum( "_DecreaseSP" );
	sLevelFieldNum._NeedSkillPoint			= pSkillLevelTable->GetFieldNum( "_NeedSkillPoint" );
	sLevelFieldNum._EffectClassValue1Duration = pSkillLevelTable->GetFieldNum( "_EffectClassValue1Duration");

	//##################################################################
	// Load
	//##################################################################

	TSkillLevelData SkillLevelData;
	vector<int>		vlSkillLevelList;

	for( int i=0 ; i<pSkillTable->GetItemCount() ; ++i )
	{
		TSkillData* pSkillData = new TSkillData;
		//2010.10.7 haling STL 보호 위해 생성자로 변환
		//memset(pSkillData, 0, sizeof(TSkillData));

		pSkillData->nSkillID = pSkillTable->GetItemID(i);

		int iIdx = pSkillTable->GetIDXprimary( pSkillData->nSkillID );

		pSkillData->cNeedWeaponType[ 0 ] = pSkillTable->GetFieldFromLablePtr( iIdx, sFieldNum._NeedWeaponType[ 0 ] )->GetInteger();
		pSkillData->cNeedWeaponType[ 1 ] = pSkillTable->GetFieldFromLablePtr( iIdx, sFieldNum._NeedWeaponType[ 1 ] )->GetInteger();
		pSkillData->nMaxLevel		= pSkillTable->GetFieldFromLablePtr( iIdx, sFieldNum._MaxLevel )->GetInteger();
		pSkillData->cSkillType		= pSkillTable->GetFieldFromLablePtr( iIdx, sFieldNum._SkillType )->GetInteger();
		pSkillData->cDurationType	= pSkillTable->GetFieldFromLablePtr( iIdx, sFieldNum._DurationType )->GetInteger();
		pSkillData->cTargetType		= pSkillTable->GetFieldFromLablePtr( iIdx, sFieldNum._TargetType )->GetInteger();
		pSkillData->cDissolvable	= pSkillTable->GetFieldFromLablePtr( iIdx, sFieldNum._Dissolvable )->GetInteger();
		pSkillData->nNeedJobID		= pSkillTable->GetFieldFromLablePtr( iIdx, sFieldNum._NeedJob )->GetInteger();
		pSkillData->bDefaultLocked	= (pSkillTable->GetFieldFromLablePtr( iIdx, sFieldNum._Lock )->GetInteger() == 1) ? true : false;
		pSkillData->nUnlockSkillBookItemID = pSkillTable->GetFieldFromLablePtr( iIdx, sFieldNum._UnlockSkillBookItemID )->GetInteger();
		pSkillData->nUnlockPrice	= pSkillTable->GetFieldFromLablePtr( iIdx, sFieldNum._UnlockPrice )->GetInteger();
		pSkillData->nExclusiveID	= pSkillTable->GetFieldFromLablePtr( iIdx, sFieldNum._DuplicatedSkillType )->GetInteger();
		pSkillData->nGlobalCoolTime	= pSkillTable->GetFieldFromLablePtr( iIdx, sFieldNum._GlobalCoolTime )->GetInteger();
		pSkillData->nGlobalSkillGroup = pSkillTable->GetFieldFromLablePtr( iIdx, sFieldNum._GlobalSkillGroup )->GetInteger();

		if (pSkillLevelTable->GetItemIDListFromField("_SkillIndex", pSkillData->nSkillID, vlSkillLevelList) > 0)
		{
			pSkillData->vLevelDataList.clear();

			for (int j = 0; j <(int)vlSkillLevelList.size(); ++j)
			{
				// pve 스킬 레벨 테이블만 읽어옴.
				if ( 0 < pSkillLevelTable->GetFieldFromLablePtr( vlSkillLevelList.at( j ), "_ApplyType" )->GetInteger() )
					continue;

				memset(&SkillLevelData, 0, sizeof(TSkillLevelData));

				iIdx = pSkillLevelTable->GetIDXprimary( vlSkillLevelList.at(j) );

				SkillLevelData.cSkillLevel				= pSkillLevelTable->GetFieldFromLablePtr( iIdx, sLevelFieldNum._SkillLevel )->GetInteger();
				SkillLevelData.cLevelLimit				= pSkillLevelTable->GetFieldFromLablePtr( iIdx, sLevelFieldNum._LevelLimit )->GetInteger();
				//SkillLevelData.nNeedJobClass			= pSkillLevelTable->GetFieldFromLablePtr( iIdx, sLevelFieldNum._NeedJobClass )->GetInteger();		//스킬 테이블로 옮겨짐.
				SkillLevelData.nDelayTime				= pSkillLevelTable->GetFieldFromLablePtr( iIdx, sLevelFieldNum._DelayTime )->GetInteger();
				SkillLevelData.nAddRange				= pSkillLevelTable->GetFieldFromLablePtr( iIdx, sLevelFieldNum._AddRange )->GetInteger();
				SkillLevelData.nNeedItemID				= pSkillLevelTable->GetFieldFromLablePtr( iIdx, sLevelFieldNum._NeedItem )->GetInteger();
				SkillLevelData.nNeedItemDecreaseCount	= pSkillLevelTable->GetFieldFromLablePtr( iIdx, sLevelFieldNum._NeedItemDecreaseCount )->GetInteger();
				SkillLevelData.nDecreaseHP				= pSkillLevelTable->GetFieldFromLablePtr( iIdx, sLevelFieldNum._DecreaseHP )->GetInteger();
				SkillLevelData.nDecreaseSP				= pSkillLevelTable->GetFieldFromLablePtr( iIdx, sLevelFieldNum._DecreaseSP )->GetInteger();
				SkillLevelData.nNeedSkillPoint			= pSkillLevelTable->GetFieldFromLablePtr( iIdx, sLevelFieldNum._NeedSkillPoint )->GetInteger();
				SkillLevelData.nEffectClassValue1Duration = pSkillLevelTable->GetFieldFromLablePtr( iIdx, sLevelFieldNum._EffectClassValue1Duration )->GetInteger();

				pSkillData->vLevelDataList.push_back(SkillLevelData);
			}
		}

		std::pair<TMapSkillData::iterator,bool> Ret = m_pSkillData.insert(make_pair(pSkillData->nSkillID, pSkillData));
		if( Ret.second == false )
			delete pSkillData;
	}


	// 스킬 리셋 할 때 디폴트 스킬인지 아닌지 구분하기 위해 디폴트 스킬들을 모아둠
	char szLabel[64] = { 0, };
	DNTableFileFormat *pDefaultCreateTable = GetDNTable( CDnTableDB::TDEFAULTCREATE );
	int iNumDefaultCreateTalbleItem = pDefaultCreateTable->GetItemCount();
	for( int i = 0; i < iNumDefaultCreateTalbleItem; ++i )
	{
		int iItemID = pDefaultCreateTable->GetItemID( i );
		TDefaultCreateData DefaultData;

		int iClassID = pDefaultCreateTable->GetFieldFromLablePtr( iItemID, "_ClassID" )->GetInteger();
		
		for (int k = 0; k < DEFAULTSKILLMAX; k++ )
		{
			sprintf_s(szLabel, "_DefaultSkill%d", k+1);
			DefaultData.nDefaultSkillID[ k ] = pDefaultCreateTable->GetFieldFromLablePtr( iItemID, szLabel )->GetInteger();
		}

		m_mapDefaultCreateData.insert( make_pair(iClassID, DefaultData) );
	}

	return true;
}

TSkillData* CDNGameDataManager::GetSkillData(int nSkillID)
{
	if (nSkillID <= 0) return NULL;

	TMapSkillData::iterator iter = m_pSkillData.find(nSkillID);
	if (iter != m_pSkillData.end()){
		return iter->second;
	}
	return NULL;
}

TSkillLevelData* CDNGameDataManager::GetSkillLevelData(int nSkillID, char cSkillLevel)
{
	if (nSkillID <= 0) return NULL;
	if (cSkillLevel <= 0) return NULL;

	TSkillData *pSkill = GetSkillData(nSkillID);
	if (!pSkill) return NULL;

	for (int i = 0; i <(int)pSkill->vLevelDataList.size(); i++){
		if (pSkill->vLevelDataList[i].cSkillLevel == cSkillLevel){
			return &pSkill->vLevelDataList[i];
		}
	}

	return NULL;
}

TSkillTreeData* CDNGameDataManager::GetSkillTreeData( int nSkillID )
{
	if( nSkillID <= 0 ) return NULL;

	TMapSkillTreeData::iterator iter = m_pSkillTreeData.find( nSkillID );
	if( iter != m_pSkillTreeData.end() )
	{
		return iter->second;
	}

	return NULL;
}


TSkillTreeData* CDNGameDataManager::GetSkillTreeDataByIndex( int iIndex ) 
{
	TSkillTreeData* pResult = NULL;
	
	if( 0 <= iIndex && iIndex <(int)m_vlpSkillTreeData.size() )
		pResult = m_vlpSkillTreeData.at( iIndex );

	return pResult;
}


int CDNGameDataManager::GetSkillDelayTime(int nSkillID, char cSkillLevel)
{
	if (nSkillID <= 0) return NULL;
	if (cSkillLevel <= 0) return NULL;

	TSkillLevelData *pData = GetSkillLevelData(nSkillID, cSkillLevel);
	if (!pData) return 0;

	return pData->nDelayTime;
}

int CDNGameDataManager::GetSkillGlobalCoolTime( int nSkillID )
{
	int iResult = 0;
	if( 0 < nSkillID )
	{
		TSkillData* pData = GetSkillData( nSkillID );
	
		// 그룹으로 묶인 스킬만 유효한 값임.
		if( pData && 
			0 < pData->nGlobalSkillGroup )
		{
			iResult = pData->nGlobalCoolTime;
		}
	}

	return iResult;
}

int CDNGameDataManager::GetSkillGlobalGroupID( int nSkillID )
{
	int iResult = 0;
	if( 0 < nSkillID )
	{
		TSkillData* pData = GetSkillData( nSkillID );
		if( pData )
		{
			iResult = pData->nGlobalSkillGroup;
		}
	}

	return iResult;
}

bool CDNGameDataManager::IsSkillNeedJob(int nSkillID, char cSkillLevel, int nJob)
{
	TSkillData *pSkillData = GetSkillData(nSkillID);
	if (!pSkillData) return false;
	// 직업 제한 정보는 스킬레벨테이블에서 스킬테이블로 옮겨짐.
	return(pSkillData->nNeedJobID == nJob);
}

bool CDNGameDataManager::IsSkillLevel(int nSkillID, char cSkillLevel, BYTE cLevel)
{
	TSkillData *pSkillData = GetSkillData(nSkillID);
	if (!pSkillData) return false;
	if ((cSkillLevel <= 0) ||(cSkillLevel >(char)pSkillData->vLevelDataList.size())) return false;
	if (pSkillData->vLevelDataList[cSkillLevel - 1].cLevelLimit > cLevel) return false;
	return true;
}

// 스킬샵은 사라질 예정.
bool CDNGameDataManager::IsSkillShopData(int nSkillID, int nJob, BYTE cLevel)
{
	TSkillData *pSkillData = GetSkillData(nSkillID);
	if (!pSkillData) return false;

	// 스킬레벨 1짜리만 팔기때문에 0만 찾으면 된다
	if (pSkillData->vLevelDataList[0].cLevelLimit > cLevel) return false;

	return true;
}

int CDNGameDataManager::GetNeedSkillPoint(int nSkillID, char cSkillLevel)
{
	TSkillData *pSkillData = GetSkillData(nSkillID);
	if (!pSkillData) return 0;
	if ((cSkillLevel <= 0) ||(cSkillLevel >(char)pSkillData->vLevelDataList.size())) return 0;

	return pSkillData->vLevelDataList[cSkillLevel - 1].nNeedSkillPoint;
}

bool CDNGameDataManager::LoadQuestReward()
{
#ifdef _WORK
	DNTableFileFormat *pSox;
	if (m_bAllLoaded)
		pSox = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TQUESTREWARD );
	else
		pSox = GetDNTable( CDnTableDB::TQUESTREWARD );
#else		//#ifdef _WORK
	DNTableFileFormat *pSox = GetDNTable( CDnTableDB::TQUESTREWARD );
#endif		//#ifdef _WORK
	if( !pSox )
	{
		g_Log.Log(LogType::_FILELOG, L"QuestRewardTable.dnt failed\r\n");
		return false;
	}

	m_QuestRewardMap.clear();
	m_MapQuestCommonRewardID.clear();

	char szTemp[MAX_PATH];

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{
		int					_Gold;
		int					_StringIndex;
		int					_Exp;
		int					_Type;
		int					_SelectMax;
		int					_QuestID;
		int					_QuestStep;
		int					_Class;
		std::vector<int>	_ItemIndex;
		std::vector<int>	_ItemCount;
		int					_MailID;
		int					_Fatigue;
		std::vector<int>	_ItemOptionID;
	};

	TempFieldNum sFieldNum;
	sFieldNum._Gold			= pSox->GetFieldNum( "_Gold" );
	sFieldNum._StringIndex	= pSox->GetFieldNum( "_StringIndex" );
	sFieldNum._Exp			= pSox->GetFieldNum( "_Exp" );
	sFieldNum._Type			= pSox->GetFieldNum( "_Type" );
	sFieldNum._SelectMax	= pSox->GetFieldNum( "_SelectMax" );
	sFieldNum._ItemIndex.reserve( QUESTREWARD_INVENTORYITEMMAX );
	sFieldNum._ItemCount.reserve( QUESTREWARD_INVENTORYITEMMAX );
	sFieldNum._QuestID		= pSox->GetFieldNum( "_questID" );
	sFieldNum._QuestStep	= pSox->GetFieldNum( "_step" );
	sFieldNum._Class		= pSox->GetFieldNum( "_class" );
	sFieldNum._MailID		= pSox->GetFieldNum( "_MailID" );
	sFieldNum._Fatigue		= pSox->GetFieldNum( "_GiveFTG" );

	for( int j=0 ; j<QUESTREWARD_INVENTORYITEMMAX ; ++j )
	{
		sprintf_s( szTemp, "_ItemIndex%d", j+1 );
		sFieldNum._ItemIndex.push_back( pSox->GetFieldNum(szTemp) );
		sprintf_s( szTemp, "_ItemCount%d", j+1 );
		sFieldNum._ItemCount.push_back( pSox->GetFieldNum(szTemp) );
		sprintf_s( szTemp, "_ItemOptionID%d", j+1 );
		sFieldNum._ItemOptionID.push_back( pSox->GetFieldNum(szTemp) );
	}

	//##################################################################
	// Load
	//##################################################################

	for( int i=0 ; i<pSox->GetItemCount() ; ++i )
	{
		TQuestReward RewardData;
		ZeroMemory(&RewardData, sizeof(TQuestReward));

		int nIndex = pSox->GetItemID(i);
		RewardData.nIndex = nIndex;

		int iIdx = pSox->GetIDXprimary( nIndex );

		if( pSox->GetFieldFromLablePtr( iIdx, sFieldNum._Gold ) == NULL )
			continue;

		RewardData.nCoin		= pSox->GetFieldFromLablePtr( iIdx, sFieldNum._Gold )->GetInteger();
		RewardData.nStringIndex = pSox->GetFieldFromLablePtr( iIdx, sFieldNum._StringIndex )->GetInteger();
		RewardData.nExp			= pSox->GetFieldFromLablePtr( iIdx, sFieldNum._Exp )->GetInteger();
		RewardData.cType		= (char)pSox->GetFieldFromLablePtr( iIdx, sFieldNum._Type )->GetInteger();
		RewardData.cSelectMax	= (char)pSox->GetFieldFromLablePtr( iIdx, sFieldNum._SelectMax )->GetInteger();
		RewardData.nQuestID		= pSox->GetFieldFromLablePtr( iIdx, sFieldNum._QuestID )->GetInteger();
		RewardData.nQuestStep	= pSox->GetFieldFromLablePtr( iIdx, sFieldNum._QuestStep )->GetInteger();
		RewardData.nClass		= pSox->GetFieldFromLablePtr( iIdx, sFieldNum._Class )->GetInteger();
		RewardData.nMailID		= pSox->GetFieldFromLablePtr( iIdx, sFieldNum._MailID )->GetInteger();
		RewardData.nFatigue		= pSox->GetFieldFromLablePtr( iIdx, sFieldNum._Fatigue )->GetInteger();

		if( RewardData.cType == 2 && ( RewardData.cSelectMax < 1 || RewardData.cSelectMax > QUESTREWARD_INVENTORYITEMMAX ) )
		{
			_ASSERT( false && "Quest Reward table Error");
			g_Log.Log(LogType::_FILELOG, L"Quest Reward table Error at index [%d]\n", nIndex);
			return false;
		}

		for( int j=0 ; j<QUESTREWARD_INVENTORYITEMMAX ; ++j )
		{
			RewardData.ItemArray[j].nItemID		= pSox->GetFieldFromLablePtr( iIdx, sFieldNum._ItemIndex[j] )->GetInteger();
			RewardData.ItemArray[j].nItemCount	= pSox->GetFieldFromLablePtr( iIdx, sFieldNum._ItemCount[j] )->GetInteger();
			RewardData.ItemArray[j].nItemOptionTableID	= pSox->GetFieldFromLablePtr( iIdx, sFieldNum._ItemOptionID[j] )->GetInteger();
		}

		m_QuestRewardMap.insert(make_pair(nIndex, RewardData));
		if(RewardData.nClass == 0)
			m_MapQuestCommonRewardID.insert(make_pair(RewardData.nQuestID, nIndex));
	}

	return true;
}

bool CDNGameDataManager::GetQuestReward(UINT nRewardCheck, OUT TQuestReward& recom)
{
	TMapQuestRewardIt it = m_QuestRewardMap.find(nRewardCheck);
	if( it == m_QuestRewardMap.end() )
	{
		return false;
	}

	CopyMemory(&recom, &(it->second), sizeof(TQuestReward));

	return true;
}

bool CDNGameDataManager::GetQuestCommonRewardByQuestID(int nQuestID, OUT TQuestReward& recom)
{
	TMapQuestRewardID::iterator iter = m_MapQuestCommonRewardID.find(nQuestID);
	if(iter == m_MapQuestCommonRewardID.end())
		return false;

	int nIndex = iter->second;

	return GetQuestReward(nIndex, recom);
}

bool CDNGameDataManager::LoadPlayerCustomEventUI()
{
	DNTableFileFormat * pSox = GetDNTable(CDnTableDB::TPLAYERCUSTOMEVENTUI);
	if (!pSox)
	{
		g_Log.Log(LogType::_FILELOG, L"playercustomeventui.dnt failed\r\n");
		return false;
	}

	m_EventTable.clear();

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{
		int _Activate;
		int	_MissionID;
		int	_UIString;
	};

	TempFieldNum sFieldNum;
	sFieldNum._Activate = pSox->GetFieldNum( "_Activate" );
	sFieldNum._MissionID = pSox->GetFieldNum( "_Param1" );
	sFieldNum._UIString = pSox->GetFieldNum( "_Param2" );

	int nItemID, nActivate, nMissionID, nUIString;
	for( int i = 0; i < pSox->GetItemCount(); ++i )
	{
		nItemID = nActivate = nMissionID = nUIString = 0;

		int nItemID = pSox->GetItemID( i );
		int iIdx = pSox->GetIDXprimary( nItemID );
		nActivate = pSox->GetFieldFromLablePtr( iIdx, sFieldNum._Activate )->GetInteger();
		nMissionID = pSox->GetFieldFromLablePtr( iIdx, sFieldNum._MissionID )->GetInteger();
		nUIString = pSox->GetFieldFromLablePtr( iIdx, sFieldNum._UIString )->GetInteger();

		if (nActivate > 0)
			m_EventTable.push_back(nMissionID);
	}
	return true;
}

void CDNGameDataManager::GetPlayerCustomEventUIList(std::vector <int> &vList)
{
	if (m_EventTable.empty())
		return;

	vList = m_EventTable;
}

bool CDNGameDataManager::LoadQuestLevelCapReward()
{
	DNTableFileFormat *pSox = GetDNTable(CDnTableDB::TQUEST_LEVELCAP_REWARD);
	if (!pSox)
	{
		g_Log.Log(LogType::_FILELOG, L"QuestLevelCapRewardTable.dnt failed\r\n");
		return false;
	}

	m_QuestLevelCapRewards.clear();

	char szTemp[MAX_PATH];

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{
		char _Type;
		char _ClassID;
		UINT _StringIndex;
		vector<int> _ItemID;
		vector<int> _ItemCount;
	};

	TempFieldNum sFieldNum;
	sFieldNum._Type = pSox->GetFieldNum("_QuestType");
	sFieldNum._ClassID = pSox->GetFieldNum("_class");
	sFieldNum._ItemID.reserve(MAX_QUEST_LEVEL_CAP_REWARD);
	sFieldNum._ItemCount.reserve(MAX_QUEST_LEVEL_CAP_REWARD);

	for (int i = 0; i < MAX_QUEST_LEVEL_CAP_REWARD; ++i)
	{
		sprintf_s(szTemp, "_ItemIndex%d", i + 1);
		sFieldNum._ItemID.push_back(pSox->GetFieldNum(szTemp));
		sprintf_s(szTemp, "_ItemCount%d", i + 1);
		sFieldNum._ItemCount.push_back(pSox->GetFieldNum(szTemp));
	}

	//##################################################################
	// Load
	//##################################################################

	for (int i = 0; i < pSox->GetItemCount(); ++i)
	{
		TQuestLevelCapReward Reward;
		ZeroMemory(&Reward, sizeof(Reward));

		Reward.uIndex = pSox->GetItemID(i);
		int iIdx = pSox->GetIDXprimary(Reward.uIndex);
		Reward.cType = (char)pSox->GetFieldFromLablePtr(iIdx, sFieldNum._Type)->GetInteger();
		Reward.cClassID = (char)pSox->GetFieldFromLablePtr(iIdx, sFieldNum._ClassID)->GetInteger();

		for (int j = 0; j < MAX_QUEST_LEVEL_CAP_REWARD; ++j)
		{
			Reward.Items[j].nItemID = pSox->GetFieldFromLablePtr(iIdx, sFieldNum._ItemID[j])->GetInteger();
			Reward.Items[j].nItemCount = pSox->GetFieldFromLablePtr(iIdx, sFieldNum._ItemCount[j])->GetInteger();
		}

		m_QuestLevelCapRewards.insert(make_pair(tr1::make_tuple(Reward.cType, Reward.cClassID), Reward));
	}

	return true;
}

const TQuestLevelCapReward* CDNGameDataManager::GetQuestLevelCapReward(char cType, char cClassID) const
{
	TMapQuestLevelCapReward::const_iterator it = m_QuestLevelCapRewards.find(tr1::make_tuple(cType, cClassID));
	if (it == m_QuestLevelCapRewards.end())
		return NULL;

	return &(it->second);
}

bool CDNGameDataManager::GetQuestItemSet(UINT nQuestID, OUT std::set<int>& questItemSet)
{
	TMapQuestItemDataIt it = m_QuestItemData.find(nQuestID);
	if( it == m_QuestItemData.end() )
		return false;

	questItemSet = it->second;

	return true;
}

bool CDNGameDataManager::LoadItemCompoundData( void )
{
#ifdef _WORK
	if (m_bAllLoaded)
	{
		if (m_pItemCompounder)
			SAFE_DELETE(m_pItemCompounder);
	}
	
	m_pItemCompounder = new CDnItemCompounder;
	return m_pItemCompounder->InitializeTable(m_bAllLoaded);
#else		//#ifdef _WORK
	m_pItemCompounder = new CDnItemCompounder;
	return m_pItemCompounder->InitializeTable();
#endif		//#ifdef _WORK
}

#if defined( _GAMESERVER )

void CDNGameDataManager::ItemCompound( CMultiRoom* pRoom, CDnItemCompounder::S_ITEM_SETTING_INFO& ItemSetting, CDnItemCompounder::S_OUTPUT* pOutput )
{
	m_pItemCompounder->Compound( pRoom, ItemSetting, pOutput );
}

void CDNGameDataManager::EmblemCompound( CMultiRoom* pRoom, int iPlateItemID, CDnItemCompounder::S_ITEM_SETTING_INFO& ItemSetting, CDnItemCompounder::S_OUTPUT* pOutput )
{
	m_pItemCompounder->EmblemCompound( pRoom, iPlateItemID, ItemSetting, pOutput );
}

#else
void CDNGameDataManager::ItemCompound( CDnItemCompounder::S_ITEM_SETTING_INFO& ItemSetting, CDnItemCompounder::S_OUTPUT* pOutput )
{
	m_pItemCompounder->Compound( ItemSetting, pOutput );
}

void CDNGameDataManager::EmblemCompound( int iPlateItemID, CDnItemCompounder::S_ITEM_SETTING_INFO& ItemSetting, CDnItemCompounder::S_OUTPUT* pOutput )
{
	m_pItemCompounder->EmblemCompound( iPlateItemID, ItemSetting, pOutput );
}

#endif // #if defined( _GAMESERVER )
	

bool CDNGameDataManager::LoadSkillTreeData( void )
{
#ifdef _WORK
	DNTableFileFormat* pSkillTreeTable;
	if (m_bAllLoaded)
		pSkillTreeTable = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TSKILLTREE );
	else
		pSkillTreeTable = GetDNTable( CDnTableDB::TSKILLTREE );
#else		//#ifdef _WORK
	DNTableFileFormat* pSkillTreeTable = GetDNTable( CDnTableDB::TSKILLTREE );
#endif		//#ifdef _WORK
	if( !pSkillTreeTable )
	{
		g_Log.Log( LogType::_FILELOG, L"SkillTreeTable.dnt failed\r\n" );
		return false;
	}

	if( pSkillTreeTable->GetItemCount() <= 0 )
	{
		g_Log.Log( LogType::_FILELOG, L"SkillTreeTable.dnt Count(%d)\r\n", pSkillTreeTable->GetItemCount() );
		return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		SAFE_DELETE_PMAP( TMapSkillTreeData, m_pSkillTreeData );
		SAFE_DELETE(m_pSkillTreeSystem);
	}
#endif		//#ifdef _WORK

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{
		int _SkillTableID;
		int	_TreeSlotIndex;
		int	_ParentSkillID1;
		int _ParentSkillID2;
		int	_ParentSkillID3;
		int	_NeedParentSkillLevel1;
		int _NeedParentSkillLevel2;
		int _NeedParentSkillLevel3;
#if defined(PRE_ADD_SKILL_LEVELUP_LIMIT_BY_SP)
		int _NeedBasicSP;
		int _NeedFirstSP;
		int _NeedSecondSP;
#endif	// #if defined(PRE_ADD_SKILL_LEVELUP_LIMIT_BY_SP)
#if defined( PRE_ADD_ONLY_SKILLBOOK )
		int _NeedSkillBook;
#endif	// #if defined( PRE_ADD_ONLY_SKILLBOOK )
	};

	TempFieldNum sFieldNum;
	sFieldNum._SkillTableID = pSkillTreeTable->GetFieldNum( "_SkillTableID" );
	sFieldNum._TreeSlotIndex = pSkillTreeTable->GetFieldNum( "_TreeSlotIndex" );
	sFieldNum._ParentSkillID1 = pSkillTreeTable->GetFieldNum( "_ParentSkillID1" );
	sFieldNum._ParentSkillID2 = pSkillTreeTable->GetFieldNum( "_ParentSkillID2" );
	sFieldNum._ParentSkillID3 = pSkillTreeTable->GetFieldNum( "_ParentSkillID3" );
	sFieldNum._NeedParentSkillLevel1 = pSkillTreeTable->GetFieldNum( "_NeedParentSkillLevel1" );
	sFieldNum._NeedParentSkillLevel2 = pSkillTreeTable->GetFieldNum( "_NeedParentSkillLevel2" );
	sFieldNum._NeedParentSkillLevel3 = pSkillTreeTable->GetFieldNum( "_NeedParentSkillLevel3" );
#if defined(PRE_ADD_SKILL_LEVELUP_LIMIT_BY_SP)
	sFieldNum._NeedBasicSP = pSkillTreeTable->GetFieldNum( "_NeedBasicSP1" );
	sFieldNum._NeedFirstSP = pSkillTreeTable->GetFieldNum( "_NeedFirstSP1" );
	sFieldNum._NeedSecondSP = pSkillTreeTable->GetFieldNum( "_NeedSecondSP1" );
#endif	// #if defined(PRE_ADD_SKILL_LEVELUP_LIMIT_BY_SP)
#if defined( PRE_ADD_ONLY_SKILLBOOK )
	sFieldNum._NeedSkillBook = pSkillTreeTable->GetFieldNum( "_NeedSkillBook" );
#endif	// #if defined( PRE_ADD_ONLY_SKILLBOOK )

	for( int i = 0; i < pSkillTreeTable->GetItemCount(); ++i )
	{
		TSkillTreeData* pSkillTreeData = new TSkillTreeData;
		memset( pSkillTreeData, 0, sizeof(TSkillTreeData) );

		int iSkillTreeTableID = pSkillTreeTable->GetItemID( i );
		int iIdx = pSkillTreeTable->GetIDXprimary( iSkillTreeTableID );
		pSkillTreeData->nSkillID = pSkillTreeTable->GetFieldFromLablePtr( iIdx, sFieldNum._SkillTableID )->GetInteger();
		pSkillTreeData->nTreeSlotIndex = pSkillTreeTable->GetFieldFromLablePtr( iIdx, sFieldNum._TreeSlotIndex )->GetInteger();
		pSkillTreeData->nParentSkillID[ 0 ] = pSkillTreeTable->GetFieldFromLablePtr( iIdx, sFieldNum._ParentSkillID1 )->GetInteger();
		pSkillTreeData->nParentSkillID[ 1 ] = pSkillTreeTable->GetFieldFromLablePtr( iIdx, sFieldNum._ParentSkillID2 )->GetInteger();
		pSkillTreeData->nParentSkillID[ 2 ] = pSkillTreeTable->GetFieldFromLablePtr( iIdx, sFieldNum._ParentSkillID3 )->GetInteger();
		pSkillTreeData->nNeedParentSkillLevel[ 0 ] = pSkillTreeTable->GetFieldFromLablePtr( iIdx, sFieldNum._NeedParentSkillLevel1 )->GetInteger();
		pSkillTreeData->nNeedParentSkillLevel[ 1 ] = pSkillTreeTable->GetFieldFromLablePtr( iIdx, sFieldNum._NeedParentSkillLevel2 )->GetInteger();
		pSkillTreeData->nNeedParentSkillLevel[ 2 ] = pSkillTreeTable->GetFieldFromLablePtr( iIdx, sFieldNum._NeedParentSkillLevel3 )->GetInteger();

#if defined(PRE_ADD_SKILL_LEVELUP_LIMIT_BY_SP)
		pSkillTreeData->nNeedBasicSP = pSkillTreeTable->GetFieldFromLablePtr( iIdx, sFieldNum._NeedBasicSP )->GetInteger();
		pSkillTreeData->nNeedFirstSP = pSkillTreeTable->GetFieldFromLablePtr( iIdx, sFieldNum._NeedFirstSP )->GetInteger();
		pSkillTreeData->nNeedSecondSP = pSkillTreeTable->GetFieldFromLablePtr( iIdx, sFieldNum._NeedSecondSP )->GetInteger();
#endif	//	#if defined(PRE_ADD_SKILL_LEVELUP_LIMIT_BY_SP)
#if defined( PRE_ADD_ONLY_SKILLBOOK )
		pSkillTreeData->bNeedSkillBook = pSkillTreeTable->GetFieldFromLablePtr( iIdx, sFieldNum._NeedSkillBook )->GetInteger();
#endif	// #if defined( PRE_ADD_ONLY_SKILLBOOK )
		if( 0 < pSkillTreeData->nSkillID )
		{
			std::pair<TMapSkillTreeData::iterator, bool> Ret = m_pSkillTreeData.insert( make_pair(pSkillTreeData->nSkillID, pSkillTreeData) );
			if( Ret.second == false )
				delete pSkillTreeData;
			else
				m_vlpSkillTreeData.push_back( pSkillTreeData );
		}
		else delete pSkillTreeData;
	}

	m_pSkillTreeSystem = new CDnSkillTreeSystem;
	return m_pSkillTreeSystem->InitializeTableUsingDataManager( this );
}

//---------------------------------------------------------------------------------
// PvPMapTable(PvPMaptable.dnt - TPvPMapTable)
//---------------------------------------------------------------------------------

bool CDNGameDataManager::LoadPvPMapTable()
{
#ifdef _WORK
	DNTableFileFormat* pSox;
	if (m_bAllLoaded)
		pSox = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TPVPMAP );
	else
		pSox = GetDNTable( CDnTableDB::TPVPMAP );
#else		//#ifdef _WORK
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TPVPMAP );
#endif		//#ifdef _WORK
	if( !pSox )
	{
		g_Log.Log( LogType::_FILELOG, L"PvPMapTable.dnt failed\r\n" );
		return false;
	}

	if( pSox->GetItemCount() <= 0 )
	{
		g_Log.Log( LogType::_FILELOG, L"PvPMapTable.dnt Count(%d)\r\n", pSox->GetItemCount() );
		return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		SAFE_DELETE_PMAP( TPvPMapTableData, m_PvPMapTable );
	}
#endif		//#ifdef _WORK

	char szTemp[MAX_PATH];

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{
		int					MapTableID;
		int					GameModeType;
		std::vector<int>	GameModeTableID;
		std::vector<int>	NumOfPlayersOption;
		int					Allow_Breakin_PlayingGame;
		int					ItemUsageType;
		int					AllowItemDrop;
		int					IsGuildBattleGround;
		int					ReleaseShow;
		int					LadderType;
	};

	TempFieldNum sFieldNum;
	sFieldNum.MapTableID				= pSox->GetFieldNum( "MapTableID" );
	sFieldNum.GameModeType				= pSox->GetFieldNum( "GameModeType" );
	sFieldNum.Allow_Breakin_PlayingGame	= pSox->GetFieldNum( "Allow_Breakin_PlayingGame" );
	sFieldNum.ItemUsageType				= pSox->GetFieldNum( "ItemUsageType" );
	sFieldNum.AllowItemDrop				= pSox->GetFieldNum( "AllowItemDrop" );
	sFieldNum.IsGuildBattleGround		= pSox->GetFieldNum( "IsGuildBattleGround" );
	sFieldNum.ReleaseShow				= pSox->GetFieldNum( "ReleaseShow" );
	sFieldNum.LadderType				= pSox->GetFieldNum( "_LadderType" );

	sFieldNum.GameModeTableID.reserve(10);
	for( int j=1 ; j<=10 ; ++j )
	{
		sprintf_s( szTemp, "GameModeTableID_%d", j );
		sFieldNum.GameModeTableID.push_back( pSox->GetFieldNum(szTemp) );
	}

	sFieldNum.NumOfPlayersOption.reserve(5);
	for( int j=1 ; j<= 5 ; ++j )
	{
		sprintf_s( szTemp, "NumOfPlayersOption%d", j );
		sFieldNum.NumOfPlayersOption.push_back( pSox->GetFieldNum(szTemp) );
	}

	//##################################################################
	// Load
	//##################################################################

	for( int i=0 ; i<pSox->GetItemCount() ; ++i )
	{
		int nItemID = pSox->GetItemID(i);

		int iIdx = pSox->GetIDXprimary(nItemID);

		int nMapTableID = pSox->GetFieldFromLablePtr( iIdx, sFieldNum.MapTableID )->GetInteger();
		if( nMapTableID == 0 )
			continue;

//		_ASSERT( nMapTableID == nItemID );
		
		TPvPMapTable* pPvPMapTable = new TPvPMapTable;
		pPvPMapTable->vGameModeTableID.reserve( 10 );
		pPvPMapTable->vNumOfPlayerOption.reserve( 5 );

		// GameType
		pPvPMapTable->uiGameType = pSox->GetFieldFromLablePtr( iIdx, sFieldNum.GameModeType )->GetInteger();
		_ASSERT( pPvPMapTable->uiGameType < PvPCommon::GameType::Max );

		// GameMode
		for( int j=1 ; j<=10 ; ++j )
		{
			int iTableID = pSox->GetFieldFromLablePtr( iIdx, sFieldNum.GameModeTableID[j-1] )->GetInteger();
			if( iTableID == 0 )
				continue;

			pPvPMapTable->vGameModeTableID.push_back( iTableID );
		}
		_ASSERT( pPvPMapTable->vGameModeTableID.size() );

		// 인원수
		for( int j=1 ; j<= 5 ; ++j )
		{
			int nCount = pSox->GetFieldFromLablePtr( iIdx, sFieldNum.NumOfPlayersOption[j-1] )->GetInteger();
			if( nCount <= 0 )
				break;
			
			_ASSERT( nCount <= PvPCommon::Common::MaxPlayer );
			pPvPMapTable->vNumOfPlayerOption.push_back( nCount );
		}
		_ASSERT( pPvPMapTable->vNumOfPlayerOption.size() );

		// 난입가능Flag
		pPvPMapTable->bIsBreakInto = ( pSox->GetFieldFromLablePtr( iIdx, sFieldNum.Allow_Breakin_PlayingGame )->GetInteger() == 0 ) ? false : true;

		// PvPCommon::ItemUsageType
		int nItemUsageType = pSox->GetFieldFromLablePtr( iIdx, sFieldNum.ItemUsageType )->GetInteger();
		_ASSERT( nItemUsageType >= 0 && nItemUsageType < PvPCommon::ItemUsageType::Max );

		// 아이템드롭 Flag
		pPvPMapTable->bIsAllowItemDrop = ( pSox->GetFieldFromLablePtr( iIdx, sFieldNum.AllowItemDrop )->GetInteger() == 0 ) ? false : true;

		// 길드전에서 사용할수 있는지 Flag
		pPvPMapTable->bIsGuildBattleGround = ( pSox->GetFieldFromLablePtr( iIdx, sFieldNum.IsGuildBattleGround )->GetInteger() == 0 ) ? false : true;

		// 릴리즈버전에서노출
		pPvPMapTable->bIsReleaseShow = ( pSox->GetFieldFromLablePtr( iIdx, sFieldNum.ReleaseShow )->GetInteger() == 0 ) ? false : true;

		pPvPMapTable->MatchType = static_cast<LadderSystem::MatchType::eCode>(pSox->GetFieldFromLablePtr( iIdx, sFieldNum.LadderType )->GetInteger() );

		if( pPvPMapTable->MatchType > LadderSystem::MatchType::None )
		{
			std::map<LadderSystem::MatchType::eCode,std::vector<int>>::iterator itor = m_LadderMatchTypeMapIndex.find( pPvPMapTable->MatchType );
			if( itor == m_LadderMatchTypeMapIndex.end() )
			{
				std::vector<int> vList;
				vList.push_back( nMapTableID );
				m_LadderMatchTypeMapIndex.insert( std::make_pair(pPvPMapTable->MatchType,vList) );
			}
			else
			{
				(*itor).second.push_back( nMapTableID );
			}
		}

		std::pair<TPvPMapTableData::iterator,bool> Ret = m_PvPMapTable.insert( make_pair( nMapTableID, pPvPMapTable ) );
		if( Ret.second == false )
			delete pPvPMapTable;
	}

	return true;
}


const TPvPMapTable* CDNGameDataManager::GetPvPMapTable( const int nItemID )
{
	TPvPMapTableData::iterator itor = m_PvPMapTable.find( nItemID );
	if( itor != m_PvPMapTable.end() )
		return itor->second;

	return NULL;
}

#ifdef PRE_MOD_PVPRANK
UINT CDNGameDataManager::GetPvPExpThreshold()
{
	return m_nThresholdPvPExpValue;
}
 
int CDNGameDataManager::GetRelativePvPRank(int nExpAbsoluteRank, float fExpRateRank)
{
	if (nExpAbsoluteRank == 0 && fExpRateRank == 0.0f)
		return 0;

	//if (PvPCommon::Common::PvPAbsoluteRankCountMax < nExpAbsoluteRank)
	//	return -1;

	TPvPRankTableData::iterator itor;
	if (fExpRateRank > 0.0f)
	{
		for (itor = m_PvPRankTable.begin(); itor != m_PvPRankTable.end(); itor++)
		{
			if ((*itor).second->cType != PvPCommon::RankTable::RateValue) continue;
			if ((*itor).second->cMinRange <= (BYTE)fExpRateRank && (*itor).second->cMaxRange >= (BYTE)fExpRateRank)
				return (*itor).first;
		}
	}
	else if (nExpAbsoluteRank > 0)
	{
		for (itor = m_PvPRankTable.begin(); itor != m_PvPRankTable.end(); itor++)
		{
			if ((*itor).second->cType != PvPCommon::RankTable::AbsoluteRankValue) continue;
			if ((*itor).second->cMinRange <= nExpAbsoluteRank && (*itor).second->cMaxRange >= nExpAbsoluteRank)
				return (*itor).first;
		}
	}
	
	return -2;
}

#if defined(PRE_ADD_PVP_RANKING)
void CDNGameDataManager::SetRelativePvPRank( BYTE *cPvPLevel, UINT uiExp, int iPvPRank, float fPvPRaito )
{
	if( uiExp >= g_pDataManager->GetPvPExpThreshold() )
	{
		int nRetLevel = g_pDataManager->GetRelativePvPRank(iPvPRank, fPvPRaito);
		if (nRetLevel > 0)
			*cPvPLevel = static_cast<BYTE>(nRetLevel);
	}
}
#endif		//#if defined(PRE_ADD_PVP_RANKING)
#endif		//#ifdef PRE_MOD_PVPRANK

int	CDNGameDataManager::GetRandomLadderMapIndex( LadderSystem::MatchType::eCode MatchType )
{
	std::map<LadderSystem::MatchType::eCode,std::vector<int>>::iterator itor = m_LadderMatchTypeMapIndex.find( MatchType );
	if( itor != m_LadderMatchTypeMapIndex.end() )
	{
		CMtRandom Random;
		Random.srand( timeGetTime() );

		return(*itor).second[Random.rand()%(*itor).second.size()];
	}
	return -1;
}

//---------------------------------------------------------------------------------
// PvPGameModeTable(PvPGameModetable.dnt - TPvPGameModeTable)
//---------------------------------------------------------------------------------

bool CDNGameDataManager::LoadPvPGameModeTable()
{
#ifdef _WORK
	DNTableFileFormat* pSox;
	if (m_bAllLoaded)
		pSox = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TPVPGAMEMODE );
	else
		pSox = GetDNTable( CDnTableDB::TPVPGAMEMODE );
#else		//#ifdef _WORK
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TPVPGAMEMODE );
#endif		//#ifdef _WORK
	if( !pSox )
	{
		g_Log.Log( LogType::_FILELOG, L"PvPGameModeTable.dnt failed\r\n" );
		return false;
	}

	if( pSox->GetItemCount() <= 0 )
	{
		g_Log.Log( LogType::_FILELOG, L"PvPGameModeTable.dnt Count(%d)\r\n", pSox->GetItemCount() );
		return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		SAFE_DELETE_PMAP( TPvPGameModeTableData, m_PvPGameModeTable );
	}
#endif		//#ifdef _WORK

	char szTemp[MAX_PATH];

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{
		int					GamemodeID;
		std::vector<int>	WinCondition;
		std::vector<int>	PlayTime;
		int					Respawn_Time;
		int					Respawn_Nodamage_Time;
		int					Respawn_HP_Percent;
		int					Respawn_MP_Percent;
		int					WinXP_PerRound;
		int					LoseXP_PerRound;
		int					KOItemDropTableID;
		int					StartRegulation;
		int					ReleaseShow;
		int					MedalExp;
		std::vector<int>	VictoryExp;
		std::vector<int>	VicrotyBonusRate;
		std::vector<int>	DefeatExp;
		std::vector<int>	DefeatBonusRate;
		int					NumOfPlayersMin;
		int					NumOfPlayersMax;
		int					LadderType;
		int					ItemID;
		int					FatigueConTime;
		int					FatigueConValue;
		int					FatigueConExp;
		int					FatigueConMedal;
		int					NeedInven;
		int					SkillSetting;
		int					SelectableRevision;
		int					DefaultRevision;
		int nBattleGroundTableID;
		int nAllowedUserCreateMode;
#if defined( PRE_ADD_RACING_MODE )
		int					DefaultRewardItem;
		int					DefaultRewardValue;
		std::vector<int>	RankRewardItem;
		std::vector<int>	RankRewardValue;
#endif

	};

	TempFieldNum sFieldNum;
	sFieldNum.GamemodeID			= pSox->GetFieldNum( "GamemodeID" );
	sFieldNum.Respawn_Time			= pSox->GetFieldNum( "Respawn_Time" );
	sFieldNum.Respawn_Nodamage_Time	= pSox->GetFieldNum( "Respawn_Nodamage_Time" );
	sFieldNum.Respawn_HP_Percent	= pSox->GetFieldNum( "Respawn_HP_Percent" );
	sFieldNum.Respawn_MP_Percent	= pSox->GetFieldNum( "Respawn_MP_Percent" );
	sFieldNum.WinXP_PerRound		= pSox->GetFieldNum( "WinXP_PerRound" );
	sFieldNum.LoseXP_PerRound		= pSox->GetFieldNum( "LoseXP_PerRound" );
	sFieldNum.KOItemDropTableID		= pSox->GetFieldNum( "KOItemDropTableID" );
	sFieldNum.StartRegulation		= pSox->GetFieldNum( "StartRegulation" );
	sFieldNum.ReleaseShow			= pSox->GetFieldNum( "ReleaseShow" );
	sFieldNum.MedalExp				= pSox->GetFieldNum( "MedalExp" );
	sFieldNum.NumOfPlayersMin		= pSox->GetFieldNum( "NumOfPlayers_Min" );
	sFieldNum.NumOfPlayersMax		= pSox->GetFieldNum( "NumOfPlayers_Max" );
	sFieldNum.LadderType			= pSox->GetFieldNum( "LadderType" );
	sFieldNum.ItemID				= pSox->GetFieldNum( "_ItemID" );
	sFieldNum.FatigueConTime		= pSox->GetFieldNum( "_FatigueConTime" );
	sFieldNum.FatigueConValue		= pSox->GetFieldNum( "_FatigueConValue" );
	sFieldNum.FatigueConExp			= pSox->GetFieldNum( "_FatigueConExp" );
	sFieldNum.FatigueConMedal		= pSox->GetFieldNum( "_FatigueConMedal" );
	sFieldNum.NeedInven				= pSox->GetFieldNum( "_NeedInven" );
	sFieldNum.SkillSetting			= pSox->GetFieldNum( "_SkillSetting" );
	sFieldNum.SelectableRevision	= pSox->GetFieldNum( "_SelectableRevision" );
	sFieldNum.DefaultRevision		= pSox->GetFieldNum( "_DefaultRevision" );
	sFieldNum.nBattleGroundTableID = pSox->GetFieldNum( "_BattleGroundID" );
	sFieldNum.nAllowedUserCreateMode = pSox->GetFieldNum( "_UserCreate" );

	sFieldNum.WinCondition.reserve(5);
	for( int j=1 ; j<=5 ; ++j )
	{
		sprintf_s( szTemp, "WinCondition_%d", j );
		sFieldNum.WinCondition.push_back( pSox->GetFieldNum(szTemp) );
	}
	sFieldNum.PlayTime.reserve(5);
	for( int j=1 ; j<= 5 ; ++j )
	{
		sprintf_s( szTemp, "PlayTime_%d", j );
		sFieldNum.PlayTime.push_back( pSox->GetFieldNum(szTemp) );
	}

	sFieldNum.VictoryExp.reserve(5);
	for( int j=1 ; j<= 5 ; ++j )
	{
		sprintf_s( szTemp, "VictoryExp_%d", j );
		sFieldNum.VictoryExp.push_back( pSox->GetFieldNum(szTemp) );
	}

	sFieldNum.VicrotyBonusRate.reserve(5);
	for( int j=1 ; j<= 5 ; ++j )
	{
		sprintf_s( szTemp, "VictoryBonusRate_%d", j );
		sFieldNum.VicrotyBonusRate.push_back( pSox->GetFieldNum(szTemp) );
	}

	sFieldNum.DefeatExp.reserve(5);
	for( int j=1 ; j<= 5 ; ++j )
	{
		sprintf_s( szTemp, "DefeatExp_%d", j );
		sFieldNum.DefeatExp.push_back( pSox->GetFieldNum(szTemp) );
	}

	sFieldNum.DefeatBonusRate.reserve(5);
	for( int j=1 ; j<= 5 ; ++j )
	{
		sprintf_s( szTemp, "DefeatBonusRate_%d", j );
		sFieldNum.DefeatBonusRate.push_back( pSox->GetFieldNum(szTemp) );
	}
#if defined(PRE_ADD_RACING_MODE)
	sFieldNum.DefaultRewardItem = pSox->GetFieldNum( "_ModeDefaultRewardItem" );
	sFieldNum.DefaultRewardValue = pSox->GetFieldNum( "_ModeDefaultRewardValue" );
	sFieldNum.RankRewardItem.reserve(8);
	for( int j=1; j<=8; ++ j )
	{
		sprintf_s( szTemp, "_RankRewardItem%d", j );
		sFieldNum.RankRewardItem.push_back( pSox->GetFieldNum(szTemp) );
	}
	sFieldNum.RankRewardValue.reserve(8);
	for( int j=1; j<=8; ++ j )
	{
		sprintf_s( szTemp, "_RankRewardValue%d", j );
		sFieldNum.RankRewardValue.push_back( pSox->GetFieldNum(szTemp) );
	}
#endif
	//##################################################################
	// Load
	//##################################################################

	for( int i=0 ; i<pSox->GetItemCount() ; ++i )
	{
		int nItemID = pSox->GetItemID(i);

		_ASSERT( nItemID > 0 );

		int iIdx = pSox->GetIDXprimary( nItemID );

		int iGameMode = pSox->GetFieldFromLablePtr( iIdx, sFieldNum.GamemodeID )->GetInteger();
		if( iGameMode == -1 || iGameMode >= PvPCommon::GameMode::Max )
			continue;

		_ASSERT( iGameMode < PvPCommon::GameMode::Max );

		TPvPGameModeTable* pPvPGameModeTable = new TPvPGameModeTable;
		pPvPGameModeTable->nItemID = nItemID;
		pPvPGameModeTable->vWinCondition.reserve( 5 );
		pPvPGameModeTable->vPlayTimeSec.reserve( 5 );

		pPvPGameModeTable->uiGameMode = iGameMode;

		// 승리조건
		for( int j=1 ; j<=5 ; ++j )
		{
			int iWinCondition = pSox->GetFieldFromLablePtr( iIdx, sFieldNum.WinCondition[j-1] )->GetInteger();
			if( iWinCondition == 0 )
				break;
			_ASSERT( iWinCondition > 0 );

			pPvPGameModeTable->vWinCondition.push_back( iWinCondition );
		}
		//_ASSERT( pPvPGameModeTable->vWinCondition.size() );

		// 플레이시간
		for( int j=1 ; j<= 5 ; ++j )
		{
			int iSec = pSox->GetFieldFromLablePtr( iIdx, sFieldNum.PlayTime[j-1] )->GetInteger();
			if( iSec == 0 )
				break;
			_ASSERT( iSec > 0 );

			pPvPGameModeTable->vPlayTimeSec.push_back( iSec );
		}
		//_ASSERT( pPvPGameModeTable->vPlayTimeSec.size() );
		// LadderType
		pPvPGameModeTable->LadderMatchType = static_cast<LadderSystem::MatchType::eCode>(pSox->GetFieldFromLablePtr( iIdx, sFieldNum.LadderType )->GetInteger());
		// 부활대기시간
		pPvPGameModeTable->uiRespawnTimeSec = pSox->GetFieldFromLablePtr( iIdx, sFieldNum.Respawn_Time )->GetInteger();
		// 부활무적시간 
		pPvPGameModeTable->uiRespawnNoDamageTimeSec = pSox->GetFieldFromLablePtr( iIdx, sFieldNum.Respawn_Nodamage_Time )->GetInteger();
		// 부활 시 회복되는 HP%
		pPvPGameModeTable->uiRespawnHPPercent = pSox->GetFieldFromLablePtr( iIdx, sFieldNum.Respawn_HP_Percent )->GetInteger();
		// 부활 시 회복되는 MP%
		pPvPGameModeTable->uiRespawnMPPercent = pSox->GetFieldFromLablePtr( iIdx, sFieldNum.Respawn_MP_Percent )->GetInteger();
		// 승리 보너스 점수
		pPvPGameModeTable->uiWinXPPerRound = pSox->GetFieldFromLablePtr( iIdx, sFieldNum.WinXP_PerRound )->GetInteger();
		// 패배 보너스 점수
		pPvPGameModeTable->uiLoseXPPerRound = pSox->GetFieldFromLablePtr( iIdx, sFieldNum.LoseXP_PerRound )->GetInteger();
		// 적KO시드롭아이템
		pPvPGameModeTable->uiItemDropTableID = pSox->GetFieldFromLablePtr( iIdx, sFieldNum.KOItemDropTableID )->GetInteger();
		// 레벨보정시스템ON/OFF
		pPvPGameModeTable->bIsLevelRegulation = ( pSox->GetFieldFromLablePtr( iIdx, sFieldNum.StartRegulation )->GetInteger() == 1 ) ? true : false;
		// 릴리즈버전에서노출
		pPvPGameModeTable->bIsReleaseShow = ( pSox->GetFieldFromLablePtr( iIdx, sFieldNum.ReleaseShow )->GetInteger() == 1 ) ? true : false;
		// 메달지급점수
		pPvPGameModeTable->uiMedalExp = pSox->GetFieldFromLablePtr( iIdx, sFieldNum.MedalExp )->GetInteger();
		pPvPGameModeTable->uiNumOfPlayersMin = pSox->GetFieldFromLablePtr( iIdx, sFieldNum.NumOfPlayersMin )->GetInteger();
		pPvPGameModeTable->uiNumOfPlayersMax = pSox->GetFieldFromLablePtr( iIdx, sFieldNum.NumOfPlayersMax )->GetInteger();
		pPvPGameModeTable->iRewardItemID = pSox->GetFieldFromLablePtr( iIdx, sFieldNum.ItemID )->GetInteger();
		pPvPGameModeTable->nFatigueConTime = pSox->GetFieldFromLablePtr( iIdx, sFieldNum.FatigueConTime)->GetInteger();
		pPvPGameModeTable->nFatigueConValue = pSox->GetFieldFromLablePtr( iIdx, sFieldNum.FatigueConValue)->GetInteger();
		pPvPGameModeTable->nFatigueConExp = pSox->GetFieldFromLablePtr( iIdx, sFieldNum.FatigueConExp)->GetInteger();
		pPvPGameModeTable->nFatigueConMedal = pSox->GetFieldFromLablePtr( iIdx, sFieldNum.FatigueConMedal)->GetInteger();
		pPvPGameModeTable->iNeedInven		= pSox->GetFieldFromLablePtr( iIdx, sFieldNum.NeedInven)->GetInteger();
		pPvPGameModeTable->iSkillSetting	= pSox->GetFieldFromLablePtr( iIdx, sFieldNum.SkillSetting)->GetInteger();
		pPvPGameModeTable->bIsSelectableRegulation	= ( pSox->GetFieldFromLablePtr( iIdx, sFieldNum.SelectableRevision )->GetInteger() == TRUE ) ? true : false;
		pPvPGameModeTable->bIsDefaultRegulation		= ( pSox->GetFieldFromLablePtr( iIdx, sFieldNum.DefaultRevision )->GetInteger() == TRUE ) ? true : false;
		pPvPGameModeTable->nBattleGroundTableID = pSox->GetFieldFromLablePtr( iIdx, sFieldNum.nBattleGroundTableID)->GetInteger();
		pPvPGameModeTable->bAllowedUserCreateMode = (pSox->GetFieldFromLablePtr( iIdx, sFieldNum.nAllowedUserCreateMode)->GetInteger() > 0) ? true : false;

		// 승리시 얻는 PvPExp 점수
		for( int j=1 ; j<= 5 ; ++j )
		{
			int iExp = pSox->GetFieldFromLablePtr( iIdx, sFieldNum.VictoryExp[j-1] )->GetInteger();
			pPvPGameModeTable->vVictoryExp.push_back( iExp );
		}

		// 승리시 얻는 PvPExp 보너스 점수
		for( int j=1 ; j<= 5 ; ++j )
		{
			int iExp = pSox->GetFieldFromLablePtr( iIdx, sFieldNum.VicrotyBonusRate[j-1] )->GetInteger();
			pPvPGameModeTable->vVictoryBonusRate.push_back( iExp );
		}

		// 패배시 얻는 PvPExp 점수
		for( int j=1 ; j<= 5 ; ++j )
		{
			int iExp = pSox->GetFieldFromLablePtr( iIdx, sFieldNum.DefeatExp[j-1] )->GetInteger();
			pPvPGameModeTable->vDefeatExp.push_back( iExp );
		}

		// 패배시 얻는 PvPExp 보너스 점수
		for( int j=1 ; j<= 5 ; ++j )
		{
			int iExp = pSox->GetFieldFromLablePtr( iIdx, sFieldNum.DefeatBonusRate[j-1] )->GetInteger();
			pPvPGameModeTable->vDefeatBonusRate.push_back( iExp );
		}
#if defined( PRE_ADD_RACING_MODE ) || defined( PRE_ADD_PVP_TOURNAMENT )
		pPvPGameModeTable->nDefaultRewardItem = pSox->GetFieldFromLablePtr( iIdx, sFieldNum.DefaultRewardItem)->GetInteger();
		pPvPGameModeTable->nDefaultRewardValue = pSox->GetFieldFromLablePtr( iIdx, sFieldNum.DefaultRewardValue)->GetInteger();
		// 상위권 보상 아이템 ID
		for( int j=1 ; j<= 8 ; ++j )
		{
			int iRewardItem = pSox->GetFieldFromLablePtr( iIdx, sFieldNum.RankRewardItem[j-1] )->GetInteger();
			pPvPGameModeTable->vRankRewardItem.push_back( iRewardItem );
		}
		// 상위권 보상 아이템 수량
		for( int j=1 ; j<= 8 ; ++j )
		{
			int iRewardValue = pSox->GetFieldFromLablePtr( iIdx, sFieldNum.RankRewardValue[j-1] )->GetInteger();
			pPvPGameModeTable->vRankRewardValue.push_back( iRewardValue );
		}
#endif // #if defined( PRE_ADD_RACING_MODE ) || defined( PRE_ADD_PVP_TOURNAMENT )
		std::pair<TPvPGameModeTableData::iterator,bool> Ret = m_PvPGameModeTable.insert( make_pair( nItemID, pPvPGameModeTable ) );
		if( Ret.second == false )
			delete pPvPGameModeTable;
	}

	return true;
}


const TPvPGameModeTable* CDNGameDataManager::GetPvPGameModeTable( const int nItemID )
{
	TPvPGameModeTableData::iterator itor = m_PvPGameModeTable.find( nItemID );
	if( itor != m_PvPGameModeTable.end() )
		return itor->second;

	return NULL;
}

#if defined( _GAMESERVER )
//---------------------------------------------------------------------------------
//(pvpgamemodeitemsetting.dnt)
//---------------------------------------------------------------------------------

bool CDNGameDataManager::LoadPvPGameModeSkillSetting()
{
#ifdef _WORK
	DNTableFileFormat* pSox;
	if (m_bAllLoaded)
		pSox = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TPVPGAMEMODESKILLSETTING );
	else
		pSox = GetDNTable( CDnTableDB::TPVPGAMEMODESKILLSETTING );
#else		//#ifdef _WORK
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TPVPGAMEMODESKILLSETTING );
#endif		//#ifdef _WORK
	if( !pSox )
	{
		g_Log.Log( LogType::_FILELOG, L"pvpgamemodeitemsetting.dnt failed\r\n" );
		return false;
	}

	if( pSox->GetItemCount() <= 0 )
	{
		g_Log.Log( LogType::_FILELOG, L"pvpgamemodeitemsetting.dnt Count(%d)\r\n", pSox->GetItemCount() );
		return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		m_mPvPGameModeSkillSetting.clear();
	}
#endif		//#ifdef _WORK

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{
		int					_GroupID;
		int					_BaseClass;
		std::pair<int,int>	_SkillIndexLevel[30];
	};

	TempFieldNum sFieldNum;
	sFieldNum._GroupID		= pSox->GetFieldNum( "_GroupID" );
	sFieldNum._BaseClass	= pSox->GetFieldNum( "_BaseClass" );
	for( UINT i=0 ; i<_countof(sFieldNum._SkillIndexLevel) ; ++i )
	{
		char szBuf[MAX_PATH];
		sprintf_s( szBuf, "_SkillIndex%d", i+1 );
		sFieldNum._SkillIndexLevel[i].first	= pSox->GetFieldNum( szBuf );
		sprintf_s( szBuf, "_SkillLevel%d", i+1 );
		sFieldNum._SkillIndexLevel[i].second	= pSox->GetFieldNum( szBuf );
	}


	//##################################################################
	// Load
	//##################################################################

	for( int i=0 ; i<pSox->GetItemCount() ; ++i )
	{
		int nItemID = pSox->GetItemID(i);

		_ASSERT( nItemID > 0 );

		int iIdx = pSox->GetIDXprimary( nItemID );

		int iGroupID	= pSox->GetFieldFromLablePtr( iIdx, sFieldNum._GroupID )->GetInteger();
		int iClassID	= pSox->GetFieldFromLablePtr( iIdx, sFieldNum._BaseClass )->GetInteger()+1;	// 테이블에서는 Index 가 ZeroBase 이기 때문에 +1 해준다.

		std::vector<std::pair<int,int>> vData;

		for( UINT j=0 ; j<_countof(sFieldNum._SkillIndexLevel) ; ++j )
		{
			int iSkillIndex	= pSox->GetFieldFromLablePtr( iIdx, sFieldNum._SkillIndexLevel[j].first )->GetInteger();
			if( iSkillIndex <= 0 )
				continue;
			int iSkillLevel	= pSox->GetFieldFromLablePtr( iIdx, sFieldNum._SkillIndexLevel[j].second )->GetInteger();
			_ASSERT( iSkillLevel < 1 );

			vData.push_back( std::make_pair(iSkillIndex,iSkillLevel) );
		}

		m_mPvPGameModeSkillSetting.insert( std::make_pair( std::make_pair(iGroupID,iClassID),vData ) );
	}

	return true;
}

bool CDNGameDataManager::GetPvPGameModeSkillSetting( int iGroupID, int iClassID, std::vector<std::pair<int,int>>& vData )
{
	map<pair<int,int>,vector<pair<int,int>>>::iterator itor = m_mPvPGameModeSkillSetting.find( std::make_pair(iGroupID,iClassID) );
	if( itor != m_mPvPGameModeSkillSetting.end() )
	{
		vData = (*itor).second;
		return true;
	}
	return false;
}
#endif // #if defined( _GAMESERVER )

bool CDNGameDataManager::LoadBattleGroundMode()
{
#ifdef _WORK
	DNTableFileFormat* pSox;
	if (m_bAllLoaded)
		pSox = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TBATTLEGROUNDMODE );
	else
		pSox = GetDNTable( CDnTableDB::TBATTLEGROUNDMODE );
#else		//#ifdef _WORK
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TBATTLEGROUNDMODE );
#endif		//#ifdef _WORK
	if( !pSox )
	{
		g_Log.Log( LogType::_FILELOG, L"battlegroundmodesetting.dnt failed\r\n" );
		return false;
	}

	if( pSox->GetItemCount() <= 0 )
	{
		g_Log.Log( LogType::_FILELOG, L"battlegroundmodesetting.dnt Count(%d)\r\n", pSox->GetItemCount() );
		return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		m_mBattleGroundModeInfo.clear();
	}
#endif		//#ifdef _WORK

	////##################################################################
	//// FieldNum 미리 색인
	////##################################################################
	struct TempFieldNum
	{
		int nID;
		int nWaitingMin;
		int nFinishingMin;
		int nPvPSkillPoint;
		int nVictoryCondition[PvPCommon::Common::MaximumVitoryCondition];		//PvPCommon::BattleGroundVictoryState
		int nVictoryParam[PvPCommon::Common::MaximumVitoryCondition];		//PvPCommon::BattleGroundVictoryState
		int nBossID[PvPCommon::TeamIndex::Max];									//생성보스아이디
		int nBossSpawnAreaID[PvPCommon::TeamIndex::Max];						//보스생성시 스폰위치
		int nMaximumResourceLimit;
		int nAllOcuupationBonusGain;
		int nOccupationBonusGain[PvPCommon::Common::MaximumCapturePosition];
		int nCaptureScore;
		int nStealScore;
		int nKillScore;
		int nClimaxTime;
		int nClimaxRespawnTime;
	};

	TempFieldNum sFieldNum;
	sFieldNum.nWaitingMin = pSox->GetFieldNum("_WaitingTime");
	sFieldNum.nFinishingMin = pSox->GetFieldNum("_WarFinishTime");
	sFieldNum.nPvPSkillPoint = pSox->GetFieldNum("_GuildSkillPoint");

	char szTemp[MAX_PATH];
	for (int i = 0; i < PvPCommon::Common::MaximumVitoryCondition; i++)
	{
		memset(&szTemp, 0, sizeof(szTemp));
		sprintf_s( szTemp, "_VictoryRule%d", i+1 );
		sFieldNum.nVictoryCondition[i] = pSox->GetFieldNum(szTemp);

		memset(&szTemp, 0, sizeof(szTemp));
		sprintf_s( szTemp, "_VictoryRule%dParam", i+1 );
		sFieldNum.nVictoryParam[i] = pSox->GetFieldNum(szTemp);
	}
	
	for (int i = 0; i < PvPCommon::TeamIndex::Max; i++)
	{
		sFieldNum.nBossID[i] = pSox->GetFieldNum(i == 0 ? "_CosmosBossID" : "_ChaosBossID");
		sFieldNum.nBossSpawnAreaID[i] = pSox->GetFieldNum(i == 0 ? "_CosmosBossPlaceID" : "_ChaosBossPlaceID");
	}

	sFieldNum.nMaximumResourceLimit = pSox->GetFieldNum("_MaxResourceSave");
	sFieldNum.nAllOcuupationBonusGain = pSox->GetFieldNum("_CaptureResourceAll");	
	for (int i = 0; i < PvPCommon::Common::MaximumCapturePosition; i++)
	{
		memset(&szTemp, 0, sizeof(szTemp));
		sprintf_s( szTemp, "_CaptureResource%d", i+1 );
		sFieldNum.nOccupationBonusGain[i] = pSox->GetFieldNum(szTemp);
	}

	sFieldNum.nCaptureScore = pSox->GetFieldNum("_Conquest_Score");
	sFieldNum.nStealScore = pSox->GetFieldNum("_Conquest_StealScore");
	sFieldNum.nKillScore = pSox->GetFieldNum("_Conquest_KillScore");
	sFieldNum.nClimaxTime = pSox->GetFieldNum("_ClimaxTime");
	sFieldNum.nClimaxRespawnTime = pSox->GetFieldNum("_Cl_Respawn_Time");

	////##################################################################
	//// Load
	////##################################################################

	for (int i = 0; i < pSox->GetItemCount(); ++i)
	{
		int nItemID = pSox->GetItemID(i);
		int nIdx = pSox->GetIDXprimary(nItemID);

		TBattleGourndModeInfo Info;
		memset(&Info, 0, sizeof(TBattleGourndModeInfo));

		Info.nID = nItemID;
		Info.nWaitingMin =  pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nWaitingMin)->GetInteger();
		Info.nFinishingMin =  pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nFinishingMin)->GetInteger();
		Info.nPvPSkillPoint =  pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nPvPSkillPoint)->GetInteger();

		for (int j = 0; j < PvPCommon::Common::MaximumVitoryCondition; j++)
		{
			Info.ModeInfo.nVictoryCondition[j] =  pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nVictoryCondition[j])->GetInteger();
			Info.ModeInfo.nVictoryParam[j] =  pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nVictoryParam[j])->GetInteger();
		}

		for (int j = 0; j < PvPCommon::TeamIndex::Max; j++)
		{
			Info.ModeInfo.nBossID[j] =  pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nBossID[j])->GetInteger();
			Info.ModeInfo.nBossSpawnAreaID[j] =  pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nBossSpawnAreaID[j])->GetInteger();
		}

		Info.nMaximumResourceLimit =  pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nMaximumResourceLimit)->GetInteger();
		Info.nAllOcuupationBonusGain =  pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nAllOcuupationBonusGain)->GetInteger();

		for (int j = 0; j < PvPCommon::Common::MaximumCapturePosition; j++)
			Info.nOccupationBonusGain[j] = pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nOccupationBonusGain[j])->GetInteger();

		Info.nCaptureScore = pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nCaptureScore)->GetInteger();
		Info.nStealScore = pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nStealScore)->GetInteger();
		Info.nKillScore = pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nKillScore)->GetInteger();
		Info.nClimaxTime = pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nClimaxTime)->GetInteger();
		Info.nClimaxRespawnTime = pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nClimaxRespawnTime)->GetInteger();

		if(m_mBattleGroundModeInfo.find(Info.nID) == m_mBattleGroundModeInfo.end())
			m_mBattleGroundModeInfo.insert(std::make_pair(Info.nID, Info));
		else
		{
			_ASSERT_EXPR(0, L"같은 맵아이디가 존재함 맵아이디는 유니크해야함");
			return false;
		}
	}
	
	return true;
}

bool CDNGameDataManager::GetBattleGroundModeInfo(int nPvPModeID, TBattleGourndModeInfo &ModeInfo)
{
	std::map <int, TBattleGourndModeInfo>::iterator ii = m_mBattleGroundModeInfo.find(nPvPModeID);
	if(ii != m_mBattleGroundModeInfo.end())
	{
		ModeInfo = (*ii).second;
		return true;
	}
	return false;
}

bool CDNGameDataManager::LoadBattleGroundResourceArea()
{
#ifdef _WORK
	DNTableFileFormat* pSox;
	if (m_bAllLoaded)
		pSox = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TBATTLEGROUNDRESWAR );
	else
		pSox = GetDNTable( CDnTableDB::TBATTLEGROUNDRESWAR );
#else		//#ifdef _WORK
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TBATTLEGROUNDRESWAR );
#endif		//#ifdef _WORK
	if( !pSox )
	{
		g_Log.Log( LogType::_FILELOG, L"battlegroundresourcewar.dnt failed\r\n" );
		return false;
	}

	if( pSox->GetItemCount() <= 0 )
	{
		g_Log.Log( LogType::_FILELOG, L"battlegroundresourcewar.dnt Count(%d)\r\n", pSox->GetItemCount() );
		return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		m_mBattleGroundPositionInfo.clear();
	}
#endif		//#ifdef _WORK

	////##################################################################
	//// FieldNum 미리 색인
	////##################################################################

	struct TempFieldNum
	{
		int nID;
		int nMapID;
		int nPvPModeID;
		int nAreaID;
		int nGainResourceTermTick;
		int nGainResource;
		int nRequireTryTick;
		int nCompleteOccupationTick;
		int nBonusBuffID;
		int nClimaxTick;		
		int nClimaxGainTermTick;
		int nClimaxGainVal;
		int nClimaxTryTick;
		int nClimaxCompleteOccupationTick;
	};

	TempFieldNum sFieldNum;
	sFieldNum.nMapID = pSox->GetFieldNum( "_MapID" );
	sFieldNum.nPvPModeID = pSox->GetFieldNum( "_PvPGamemodeId" );
	sFieldNum.nAreaID = pSox->GetFieldNum( "_EventAreaID" );
	sFieldNum.nGainResourceTermTick	= pSox->GetFieldNum( "_ResourceAddTic" );
	sFieldNum.nGainResource	= pSox->GetFieldNum( "_AddTicResource" );
	sFieldNum.nRequireTryTick	= pSox->GetFieldNum( "_ClickKeepTime" );
	sFieldNum.nCompleteOccupationTick = pSox->GetFieldNum( "_CaptureKeepTime" );
	sFieldNum.nBonusBuffID = pSox->GetFieldNum( "_CaptureBounersBuffID" );
	sFieldNum.nClimaxGainTermTick = pSox->GetFieldNum( "_Cl_ResourceAddTic" );
	sFieldNum.nClimaxGainVal = pSox->GetFieldNum( "_Cl_AddTicResource" );
	sFieldNum.nClimaxTryTick = pSox->GetFieldNum( "_Cl_ClickKeepTime" );
	sFieldNum.nClimaxCompleteOccupationTick = pSox->GetFieldNum( "_Cl_CaptureKeepTime" );

	////##################################################################
	//// Load
	////##################################################################

	std::vector<TPositionAreaInfo> vArea;
	std::vector<TPositionAreaInfo>::iterator iArea;
	for (int i = 0; i < pSox->GetItemCount(); ++i)
	{
		int nItemID = pSox->GetItemID(i);
		int nIdx = pSox->GetIDXprimary(nItemID);

		TPositionAreaInfo Info;
		memset(&Info, 0, sizeof(TPositionAreaInfo));

		Info.nID = nItemID;
		Info.nMapID =  pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nMapID)->GetInteger();
		Info.nPvPModeID =  pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nPvPModeID)->GetInteger();
		Info.nAreaID =  pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nAreaID)->GetInteger();
		Info.nGainResourceTermTick =  pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nGainResourceTermTick)->GetInteger();
		Info.nGainResource =  pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nGainResource)->GetInteger();
		Info.nRequireTryTick =  pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nRequireTryTick)->GetInteger();
		Info.nCompleteOccupationTick =  pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nCompleteOccupationTick)->GetInteger();
		Info.nBonusBuffID =  pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nBonusBuffID)->GetInteger();
		Info.nClimaxGainTermTick =  pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nClimaxGainTermTick)->GetInteger();
		Info.nClimaxGainVal =  pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nClimaxGainVal)->GetInteger();
		Info.nClimaxTryTick =  pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nClimaxTryTick)->GetInteger();
		Info.nClimaxCompleteOccupationTick =  pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nClimaxCompleteOccupationTick)->GetInteger();

		std::map <int, std::vector<TPositionAreaInfo>>::iterator ii = m_mBattleGroundPositionInfo.find(Info.nPvPModeID);
		if(ii == m_mBattleGroundPositionInfo.end())
		{
			vArea.clear();
			vArea.push_back(Info);
			m_mBattleGroundPositionInfo.insert(std::make_pair(Info.nPvPModeID, vArea));
		}
		else
		{
			for(iArea = (*ii).second.begin(); iArea != (*ii).second.end(); iArea++)
			{
				if(Info.nAreaID == (*iArea).nAreaID)
				{
					_ASSERT_EXPR(0, L"이벤트영역ID 중복! 할당되어 있는 맵아이디내 이벤트영역ID는 유니크 해야합니다!");
					return false;
				}
			}

			(*ii).second.push_back(Info);
		}
	}

	return true;
}

bool CDNGameDataManager::GetBattleGroundResourceAreaInfo(int nMapID, int nPvPModeID, std::vector<TPositionAreaInfo> &vList)
{
	std::map <int, std::vector<TPositionAreaInfo>>::iterator ii = m_mBattleGroundPositionInfo.find(nPvPModeID);
	if(ii != m_mBattleGroundPositionInfo.end())
	{
		vList = (*ii).second;
		return true;
	}

	//없으면 맵아이디로도 찾아본다
	bool bCheck = false;
	for(ii = m_mBattleGroundPositionInfo.begin(); ii != m_mBattleGroundPositionInfo.end(); ii++)
	{
		std::vector<TPositionAreaInfo>::iterator ij;
		for(ij = (*ii).second.begin(); ij != (*ii).second.end(); ij++)
		{
			if((*ij).nMapID == nMapID)
			{
				vList.push_back((*ij));
			}
		}
	}
	return bCheck;
}

bool CDNGameDataManager::LoadBattleGroundSkillSet()
{
#ifdef _WORK
	DNTableFileFormat* pTreeSox;
	if (m_bAllLoaded)
		pTreeSox = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TBATTLEGROUNDSKILLTREE );
	else
		pTreeSox = GetDNTable( CDnTableDB::TBATTLEGROUNDSKILLTREE );
#else		//#ifdef _WORK
	DNTableFileFormat* pTreeSox = GetDNTable( CDnTableDB::TBATTLEGROUNDSKILLTREE );
#endif		//#ifdef _WORK
	if( !pTreeSox )
	{
		g_Log.Log( LogType::_FILELOG, L"battlegroundskilltreetable.dnt failed\r\n" );
		return false;
	}

	if( pTreeSox->GetItemCount() <= 0 )
	{
		g_Log.Log( LogType::_FILELOG, L"battlegroundskilltreetable.dnt Count(%d)\r\n", pTreeSox->GetItemCount() );
		return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
	}
#endif		//#ifdef _WORK

	//스킬트리테이블을 기준으로 점령전에 사용하는 스킬리스트를 얻어두자
	std::vector <int> vBGSkillList;
	for (int i = 0; i < pTreeSox->GetItemCount(); ++i)
	{
		int nItemID = pTreeSox->GetItemID(i);
		int nIdx = pTreeSox->GetIDXprimary(nItemID);
		
		int nBGSkillID =  pTreeSox->GetFieldFromLablePtr(nIdx, pTreeSox->GetFieldNum("_SkillTableID"))->GetInteger();

		if(std::find(vBGSkillList.begin(), vBGSkillList.end(), nBGSkillID) == vBGSkillList.end())
		{
			if(nBGSkillID > 0)
				vBGSkillList.push_back(nBGSkillID);
		}
		else
		{
			_ASSERT_EXPR(0, L"중복아이디가 있음");
			return false;
		}
	}

#ifdef _WORK
	DNTableFileFormat* pSkillSox;
	if (m_bAllLoaded)
		pSkillSox = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TBATTLEGROUNDSKILL );
	else
		pSkillSox = GetDNTable( CDnTableDB::TBATTLEGROUNDSKILL );	
#else		//#ifdef _WORK
	DNTableFileFormat* pSkillSox = GetDNTable( CDnTableDB::TBATTLEGROUNDSKILL );
#endif		//#ifdef _WORK
	if( !pSkillSox )
	{
		g_Log.Log( LogType::_FILELOG, L"battlegroundskilltable.dnt failed\r\n" );
		return false;
	}

	if( pSkillSox->GetItemCount() <= 0 )
	{
		g_Log.Log( LogType::_FILELOG, L"battlegroundskilltable.dnt Count(%d)\r\n", pSkillSox->GetItemCount() );
		return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		m_vBattleGroundSkill.clear();
	}
#endif		//#ifdef _WORK

	////##################################################################
	//// FieldNum 미리 색인
	////##################################################################

	struct TempSkillFieldNum
	{
		int nSkillMaxLevel;
		int nSkillType;				//CDnSkill::SkillTypeEnum		//점령전 스킬은 스킬타입에 그닥 영향을 받지 않지만 추가확장시 필요할까 해서 읽어둠
		int nSkillDurationType;		//CDnSkill::DurationTypeEnum
		int nSkillTargetType;		//CDnSkill::TargetTypeEnum
		int nEffectType[PvPCommon::Common::MaximumEffectCount];
		int nEffectApplyType[PvPCommon::Common::MaximumEffectCount];
		int nProcess;
		int nCanDuplicate;
	};

	TempSkillFieldNum sSkillFieldNum;
	sSkillFieldNum.nSkillMaxLevel = pSkillSox->GetFieldNum( "_MaxLevel" );
	sSkillFieldNum.nSkillType = pSkillSox->GetFieldNum( "_SkillType" );
	sSkillFieldNum.nSkillDurationType = pSkillSox->GetFieldNum( "_DurationType" );
	sSkillFieldNum.nSkillTargetType = pSkillSox->GetFieldNum( "_TargetType" );

	char szTemp[MAX_PATH];
	for (int i = 0; i < PvPCommon::Common::MaximumEffectCount; i++)
	{
		memset(&szTemp, 0, sizeof(szTemp));
		sprintf_s( szTemp, "_EffectClass%d", i+1 );
		sSkillFieldNum.nEffectType[i] = pSkillSox->GetFieldNum(szTemp);

		memset(&szTemp, 0, sizeof(szTemp));
		sprintf_s( szTemp, "_EffectClass%dApplyType", i+1 );
		sSkillFieldNum.nEffectApplyType[i] = pSkillSox->GetFieldNum(szTemp);
	}

	sSkillFieldNum.nCanDuplicate = pSkillSox->GetFieldNum( "_SkillDuplicate" );
	sSkillFieldNum.nProcess = pSkillSox->GetFieldNum( "_Processor1" );

	////##################################################################
	//// Load
	////##################################################################

	for (int i = 0; i < pSkillSox->GetItemCount(); ++i)
	{
		int nItemID = pSkillSox->GetItemID(i);
		int nIdx = pSkillSox->GetIDXprimary(nItemID);

		TBattleGroundSkillInfo SkillInfo;
		memset(&SkillInfo, 0, sizeof(TBattleGroundSkillInfo));

		SkillInfo.nSkillID = nItemID;
		SkillInfo.nSkillMaxLevel = pSkillSox->GetFieldFromLablePtr(nIdx, sSkillFieldNum.nSkillMaxLevel)->GetInteger();
		SkillInfo.nSkillType = pSkillSox->GetFieldFromLablePtr(nIdx, sSkillFieldNum.nSkillType)->GetInteger();
		SkillInfo.nSkillDurationType = pSkillSox->GetFieldFromLablePtr(nIdx, sSkillFieldNum.nSkillDurationType)->GetInteger();
		SkillInfo.nSkillTargetType = pSkillSox->GetFieldFromLablePtr(nIdx, sSkillFieldNum.nSkillTargetType)->GetInteger();

		for (int j = 0; j < PvPCommon::Common::MaximumEffectCount; j++)
		{
			SkillInfo.nEffectType[j] = pSkillSox->GetFieldFromLablePtr(nIdx, sSkillFieldNum.nEffectType[j])->GetInteger();
			SkillInfo.nEffectApplyType[j] = pSkillSox->GetFieldFromLablePtr(nIdx, sSkillFieldNum.nEffectApplyType[j])->GetInteger();
		}

		SkillInfo.nCanDuplicate = pSkillSox->GetFieldFromLablePtr(nIdx, sSkillFieldNum.nCanDuplicate)->GetInteger();
		SkillInfo.nProcess = pSkillSox->GetFieldFromLablePtr(nIdx, sSkillFieldNum.nProcess)->GetInteger();

		for(std::vector <TBattleGroundSkillInfo>::iterator ii = m_vBattleGroundSkill.begin(); ii != m_vBattleGroundSkill.end(); ii++)
		{
			if(SkillInfo.nSkillID == (*ii).nSkillID)
			{
				_ASSERT_EXPR(0, L"battlegroundskilltable.dnt 중복아이디!");
				return false;
			}
		}

		std::vector <int>::iterator iverify = std::find(vBGSkillList.begin(), vBGSkillList.end(), SkillInfo.nSkillID);
		if(iverify != vBGSkillList.end())
			m_vBattleGroundSkill.push_back(SkillInfo);
	}

#ifdef _WORK
	DNTableFileFormat* pSkillLevelSox;
	if (m_bAllLoaded)
		pSkillLevelSox = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TBATTLEGROUNDSKILLLEVEL );
	else
		pSkillLevelSox = GetDNTable( CDnTableDB::TBATTLEGROUNDSKILLLEVEL );
#else		//#ifdef _WORK
	DNTableFileFormat* pSkillLevelSox = GetDNTable( CDnTableDB::TBATTLEGROUNDSKILLLEVEL );
#endif		//#ifdef _WORK
	if( !pSkillLevelSox )
	{
		g_Log.Log( LogType::_FILELOG, L"battlegroundskillleveltable.dnt failed\r\n" );
		return false;
	}

	if( pSkillLevelSox->GetItemCount() <= 0 )
	{
		g_Log.Log( LogType::_FILELOG, L"battlegroundskillleveltable.dnt Count(%d)\r\n", pSkillLevelSox->GetItemCount() );
		return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		m_vBattleGroundSkillLevel.clear();
	}
#endif		//#ifdef _WORK

	////##################################################################
	//// FieldNum 미리 색인
	////##################################################################

	struct TempSkillLevelField
	{
		int nSkillID;
		int nSkillLevel;
		int nNeedSkillPoint;			//획득시필요포인트
		int nUseResPoint;			//스킬사용시 소모 점령전 포인트
		int nEffectValue[PvPCommon::Common::MaximumEffectCount];
		int nEffectValueDuration[PvPCommon::Common::MaximumEffectCount];
		//int nMonsterID;
		//int nSummonPos;
		int nActionName;
		int nCoolTime;
	};

	TempSkillLevelField sSkillLevelFieldNum;
	sSkillLevelFieldNum.nSkillID = pSkillLevelSox->GetFieldNum( "_SkillIndex" );
	sSkillLevelFieldNum.nSkillLevel = pSkillLevelSox->GetFieldNum( "_SkillLevel" );
	sSkillLevelFieldNum.nUseResPoint = pSkillLevelSox->GetFieldNum( "_DecreaseTIC" );
	sSkillLevelFieldNum.nNeedSkillPoint = pSkillLevelSox->GetFieldNum( "_NeedSkillPoint" );
	
	for (int i = 0; i < PvPCommon::Common::MaximumEffectCount; i++)
	{
		memset(&szTemp, 0, sizeof(szTemp));
		sprintf_s( szTemp, "_EffectClassValue%d", i+1 );
		sSkillLevelFieldNum.nEffectValue[i] = pSkillLevelSox->GetFieldNum(szTemp);

		memset(&szTemp, 0, sizeof(szTemp));
		sprintf_s( szTemp, "_EffectClassValue%dDuration", i+1 );
		sSkillLevelFieldNum.nEffectValueDuration[i] = pSkillLevelSox->GetFieldNum(szTemp);
	}

	//sSkillLevelFieldNum.nMonsterID = pSkillLevelSox->GetFieldNum( "_SummonMonsterID" );
	//sSkillLevelFieldNum.nSummonPos = pSkillLevelSox->GetFieldNum( "_SummonMonsterPosition" );
	sSkillLevelFieldNum.nCoolTime = pSkillLevelSox->GetFieldNum( "_DelayTime" );
	sSkillLevelFieldNum.nActionName = pSkillLevelSox->GetFieldNum( "_ProcessParam1" );

	////##################################################################
	//// Load
	////##################################################################

	for (int i = 0; i < pSkillLevelSox->GetItemCount(); ++i)
	{
		int nItemID = pSkillLevelSox->GetItemID(i);
		int nIdx = pSkillLevelSox->GetIDXprimary(nItemID);

		TBattleGroundEffectValue SkillLevel;
		SkillLevel.Reset();

		SkillLevel.nSkillID = pSkillLevelSox->GetFieldFromLablePtr(nIdx, sSkillLevelFieldNum.nSkillID)->GetInteger();
		SkillLevel.nSkillLevel = pSkillLevelSox->GetFieldFromLablePtr(nIdx, sSkillLevelFieldNum.nSkillLevel)->GetInteger();
		SkillLevel.nUseResPoint = pSkillLevelSox->GetFieldFromLablePtr(nIdx, sSkillLevelFieldNum.nUseResPoint)->GetInteger();
		SkillLevel.nNeedSkillPoint = pSkillLevelSox->GetFieldFromLablePtr(nIdx, sSkillLevelFieldNum.nNeedSkillPoint)->GetInteger();

		for (int j = 0; j < PvPCommon::Common::MaximumEffectCount; j++)
		{
			SkillLevel.strEffectValue[j] = pSkillLevelSox->GetFieldFromLablePtr(nIdx, sSkillLevelFieldNum.nEffectValue[j])->GetString();
			SkillLevel.nEffectValueDuration[j] = pSkillLevelSox->GetFieldFromLablePtr(nIdx, sSkillLevelFieldNum.nEffectValueDuration[j])->GetInteger();
		}

		//SkillLevel.nMonsterID = pSkillLevelSox->GetFieldFromLablePtr(nIdx, sSkillLevelFieldNum.nMonsterID)->GetInteger();
		//SkillLevel.nSummonDist = pSkillLevelSox->GetFieldFromLablePtr(nIdx, sSkillLevelFieldNum.nSummonPos)->GetInteger();
		SkillLevel.nCoolTime = pSkillLevelSox->GetFieldFromLablePtr(nIdx, sSkillLevelFieldNum.nCoolTime)->GetInteger();
		SkillLevel.strActionName = pSkillLevelSox->GetFieldFromLablePtr(nIdx, sSkillLevelFieldNum.nActionName)->GetString();

		for(std::vector <TBattleGroundEffectValue>::iterator ii = m_vBattleGroundSkillLevel.begin(); ii != m_vBattleGroundSkillLevel.end(); ii++)
		{
			if(SkillLevel.nSkillID == (*ii).nSkillID && SkillLevel.nSkillLevel == (*ii).nSkillLevel)
			{
				_ASSERT_EXPR(0, L"battlegroundskilltable.dnt 아이디와 레벨 중복!!");
				return false;
			}
		}

		std::vector <int>::iterator iverify = std::find(vBGSkillList.begin(), vBGSkillList.end(), SkillLevel.nSkillID);
		if(iverify != vBGSkillList.end())
			m_vBattleGroundSkillLevel.push_back(SkillLevel);
	}
	
	return true;
}

TBattleGroundSkillInfo * CDNGameDataManager::GetBattleGroundSkill(int nSkillID)
{
	if(nSkillID <= 0)
		return NULL;

	for(std::vector <TBattleGroundSkillInfo>::iterator ii = m_vBattleGroundSkill.begin(); ii != m_vBattleGroundSkill.end(); ii++)
	{
		if((*ii).nSkillID == nSkillID)
			return &(*ii);
	}
	return NULL;
}

TBattleGroundEffectValue * CDNGameDataManager::GetBattleGroundSkillLevel(int nSkillID, int nLevel)
{
	if(nSkillID <= 0 || nLevel <= 0)
		return NULL;

	for(std::vector <TBattleGroundEffectValue>::iterator ii = m_vBattleGroundSkillLevel.begin(); ii != m_vBattleGroundSkillLevel.end(); ii ++)
	{
		if((*ii).nSkillID == nSkillID && (*ii).nSkillLevel == nLevel)
			return &(*ii);
	}
	return NULL;
}

//---------------------------------------------------------------------------------
// PvPGameStartConditionTable(PvPGameStartConditiontable.dnt - TPvPGameStartConditionTable)
//---------------------------------------------------------------------------------

bool CDNGameDataManager::LoadPvPGameStartConditionTable()
{
#ifdef _WORK
	DNTableFileFormat* pSox;
	if (m_bAllLoaded)
		pSox = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TPVPGAMESTARTCONDITION );
	else
		pSox = GetDNTable( CDnTableDB::TPVPGAMESTARTCONDITION );
#else		//#ifdef _WORK
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TPVPGAMESTARTCONDITION );
#endif		//#ifdef _WORK
	if( !pSox )
	{
		g_Log.Log( LogType::_FILELOG, L"PvPGameStartConditionTable.dnt failed\r\n" );
		return false;
	}

	if( pSox->GetItemCount() <= 0 )
	{
		g_Log.Log( LogType::_FILELOG, L"PvPGameStartConditionTable.dnt Count(%d)\r\n", pSox->GetItemCount() );
		return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		SAFE_DELETE_PMAP( TPvPGameStartConditionData, m_PvPGameStartConditionTable );
	}
#endif		//#ifdef _WORK

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{
		int PlayerNumber;
		int	Min_TeamPlayerNum_Needed;
		int	Max_TeamPlayerNum_Difference;
	};

	TempFieldNum sFieldNum;
	sFieldNum.PlayerNumber					= pSox->GetFieldNum( "PlayerNumber" );
	sFieldNum.Min_TeamPlayerNum_Needed		= pSox->GetFieldNum( "Min_TeamPlayerNum_Needed" );
	sFieldNum.Max_TeamPlayerNum_Difference	= pSox->GetFieldNum( "Max_TeamPlayerNum_Difference" );

	//##################################################################
	// Load
	//##################################################################


	for( int i=0 ; i<pSox->GetItemCount() ; ++i )
	{
		int nItemID = pSox->GetItemID(i);

		int iIdx = pSox->GetIDXprimary( nItemID );

		int iMaxPlayerNum = pSox->GetFieldFromLablePtr( iIdx, sFieldNum.PlayerNumber )->GetInteger();
		if( iMaxPlayerNum <= 0 )
			continue;

		_ASSERT( iMaxPlayerNum <= PvPCommon::Common::MaxPlayer );

		TPvPGameStartConditionTable* pPvPGameStartConditionTable = new TPvPGameStartConditionTable;

		pPvPGameStartConditionTable->uiMinTeamPlayerNum		= pSox->GetFieldFromLablePtr( iIdx, sFieldNum.Min_TeamPlayerNum_Needed )->GetInteger();
		pPvPGameStartConditionTable->uiMaxTeamPlayerDiff	= pSox->GetFieldFromLablePtr( iIdx, sFieldNum.Max_TeamPlayerNum_Difference )->GetInteger();

		std::pair<TPvPGameStartConditionData::iterator,bool> Ret = m_PvPGameStartConditionTable.insert( make_pair( iMaxPlayerNum, pPvPGameStartConditionTable ) );
		if( Ret.second == false )
			delete pPvPGameStartConditionTable;
	}

	return true;
}


const TPvPGameStartConditionTable* CDNGameDataManager::GetPvPGameStartConditionTable( const UINT uiMaxUser )
{
	TPvPGameStartConditionData::iterator itor = m_PvPGameStartConditionTable.find( uiMaxUser );
	if( itor != m_PvPGameStartConditionTable.end() )
		return itor->second;

	return NULL;
}

//---------------------------------------------------------------------------------
// PvPRankTable(PvPRankTable.dnt - TPvPRankTable)
//---------------------------------------------------------------------------------

bool CDNGameDataManager::LoadPvPRankTable()
{
#ifdef _WORK
	DNTableFileFormat* pSox;
	if (m_bAllLoaded)
		pSox = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TPVPRANK );
	else
		pSox = GetDNTable( CDnTableDB::TPVPRANK );
#else		//#ifdef _WORK
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TPVPRANK );
#endif		//#ifdef _WORK
	if( !pSox )
	{
		g_Log.Log( LogType::_FILELOG, L"PvPRankTable.dnt failed\r\n" );
		return false;
	}

	if( pSox->GetItemCount() <= 0 )
	{
		g_Log.Log( LogType::_FILELOG, L"PvPRankTable.dnt Count(%d)\r\n", pSox->GetItemCount() );
		return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		SAFE_DELETE_PMAP( TPvPRankTableData, m_PvPRankTable );
	}
#endif		//#ifdef _WORK

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{
		int PvPRank;
		int	PvPRankEXP;
#ifdef PRE_MOD_PVPRANK
		int PvPRankType;
		int PvPRankValue;
#endif		//#ifdef PRE_MOD_PVPRANK
	};

	TempFieldNum sFieldNum;
	sFieldNum.PvPRank		= pSox->GetFieldNum( "PvPRank" );
	sFieldNum.PvPRankEXP	= pSox->GetFieldNum( "PvPRankEXP" );
#ifdef PRE_MOD_PVPRANK
	sFieldNum.PvPRankType	= pSox->GetFieldNum( "RankType" );
	sFieldNum.PvPRankValue	= pSox->GetFieldNum( "PVPRankValue" );

	std::string strRankVal;
	std::vector<std::string> tokens;
#endif		//#ifdef PRE_MOD_PVPRANK

	//##################################################################
	// Load
	//##################################################################

	for( int i=0 ; i<pSox->GetItemCount() ; ++i )
	{
		int nItemID = pSox->GetItemID(i);

		int iIdx = pSox->GetIDXprimary( nItemID );

		int iPvPRank = pSox->GetFieldFromLablePtr( iIdx, sFieldNum.PvPRank )->GetInteger();

		if( iPvPRank <= 0 || nItemID != iPvPRank )
		{
			g_Log.Log( LogType::_FILELOG, L"PvPRankTable.dnt nItemID(%d) iPvPRand(%d)\r\n", nItemID, iPvPRank );
			return false;
		}

		_ASSERT( iPvPRank <= PvPCommon::Common::MaxRank );

		TPvPRankTable* pPvPRankTable = new TPvPRankTable;

		pPvPRankTable->uiXP = static_cast<UINT>(pSox->GetFieldFromLablePtr( iIdx, sFieldNum.PvPRankEXP )->GetInteger());
#ifdef PRE_MOD_PVPRANK
		if (pPvPRankTable->uiXP > 0 && pPvPRankTable->uiXP > m_nThresholdPvPExpValue)
			m_nThresholdPvPExpValue = pPvPRankTable->uiXP;

		pPvPRankTable->cType = static_cast<UINT>(pSox->GetFieldFromLablePtr( iIdx, sFieldNum.PvPRankType )->GetInteger());
		if (pPvPRankTable->cType != PvPCommon::RankTable::ExpValue)
		{
			tokens.clear();
			strRankVal = pSox->GetFieldFromLablePtr( iIdx, sFieldNum.PvPRankValue )->GetString();
			TokenizeA(strRankVal, tokens, ";");
			if (tokens.size() != 2)
			{
				_ASSERT_EXPR(0, L"PvPRankTable Error");
				return false;
			}

			pPvPRankTable->cMinRange = atoi(tokens[0].c_str());
			pPvPRankTable->cMaxRange = atoi(tokens[1].c_str());
		}
#endif		//#ifdef PRE_MOD_PVPRANK

		std::pair<TPvPRankTableData::iterator,bool> Ret = m_PvPRankTable.insert( make_pair( iPvPRank, pPvPRankTable) );
		if( Ret.second == false )
			delete pPvPRankTable;
	}

	return true;
}

const TPvPRankTable* CDNGameDataManager::GetPvPRankTable( const UINT uiLevel )
{
	TPvPRankTableData::iterator itor = m_PvPRankTable.find( uiLevel );
	if( itor != m_PvPRankTable.end() )
		return itor->second;

	return NULL;
}

bool CDNGameDataManager::LoadEnchantData()
{
#ifdef _WORK
	// 인첸트 테이블 읽기
	DNTableFileFormat *pEnchantTable;
#ifdef PRE_FIX_MEMOPT_ENCHANT
	CDnTableDB::TableEnum eTableEnum = CDnTableDB::TENCHANT_MAIN;
#else		//#ifdef PRE_FIX_MEMOPT_ENCHANT
	CDnTableDB::TableEnum eTableEnum = CDnTableDB::TENCHANT;
#endif		//#ifdef PRE_FIX_MEMOPT_ENCHANT
	if (m_bAllLoaded)
		pEnchantTable = CDnTableDB::GetInstance().ReLoadSox( eTableEnum );
	else
		pEnchantTable = GetDNTable( eTableEnum );

#else		//#ifdef _WORK
	// 인첸트 테이블 읽기
#ifdef PRE_FIX_MEMOPT_ENCHANT
	DNTableFileFormat *pEnchantTable = GetDNTable( CDnTableDB::TENCHANT_MAIN );
	if( !pEnchantTable ) {
		g_Log.Log(LogType::_FILELOG, L"enchantmaintable.dnt failed\r\n");
		return false;
	}
#else		//#ifdef PRE_FIX_MEMOPT_ENCHANT
	DNTableFileFormat *pEnchantTable = GetDNTable( CDnTableDB::TENCHANT );
	if( !pEnchantTable ) {
		g_Log.Log(LogType::_FILELOG, L"EnchantTable.dnt failed\r\n");
		return false;
	}
#endif		//#ifdef PRE_FIX_MEMOPT_ENCHANT

#endif		//#ifdef _WORK

	if( pEnchantTable->GetItemCount() <= 0 ) {
		g_Log.Log(LogType::_FILELOG, L"EnchantTable.dnt Count(%d)\r\n", pEnchantTable->GetItemCount());
		return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		SAFE_DELETE_PMAPF( TMapEnchantData, m_pEnchantTable, SAFE_DELETE_PMAP( TMapEnchantLevelData, TMapEnchantData_iter->second->pMapLevelData ) );
	}	
#endif		//#ifdef _WORK

#ifdef PRE_FIX_MEMOPT_ENCHANT

#ifdef _WORK
	DNTableFileFormat* pEnchantNeedItemTable = NULL;
	if (m_bAllLoaded)
		pEnchantNeedItemTable = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TENCHANT_NEEDITEM );
	else
		pEnchantNeedItemTable = GetDNTable( CDnTableDB::TENCHANT_NEEDITEM );
#else		//#ifdef _WORK
	DNTableFileFormat* pEnchantNeedItemTable = GetDNTable( CDnTableDB::TENCHANT_NEEDITEM );	
#endif		//#ifdef _WORK	

	if (!pEnchantNeedItemTable)
	{
		g_Log.Log(LogType::_FILELOG, L"enchantneeditemtable.dnt failed\r\n");
		return false;
	}

	LoadEnchantMainTable(*pEnchantTable);
	LoadEnchantNeedItemTable(*pEnchantNeedItemTable);
#else // PRE_FIX_MEMOPT_ENCHANT
	char szTemp[MAX_PATH];

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{
		int					_EnchantID;
		int					_EnchantLevel;
		int					_EnchantRatio;
		std::vector<int>	_NeedItem;
		std::vector<int>	_NeedItemCount;
		int					_NeedCoin;
		int					_FailResultDropItemTableID;
		int					_PotentialID;
		int					_PotentialRatio;
		int					_BreakRatio;
		int					_MinDown;
		int					_MaxDown;
		int					_ProtectItemCount;
#if defined(PRE_ADD_ENCHANTSHIELD_CASHITEM)
		int					_ShieldItemCount; // 상위 강화보호젤리
#endif
	};

	TempFieldNum sFieldNum;
	sFieldNum._EnchantID					= pEnchantTable->GetFieldNum( "_EnchantID" );
	sFieldNum._EnchantLevel					= pEnchantTable->GetFieldNum( "_EnchantLevel" );
	sFieldNum._EnchantRatio					= pEnchantTable->GetFieldNum( "_EnchantRatio" );
	sFieldNum._NeedCoin						= pEnchantTable->GetFieldNum( "_NeedCoin" );
	sFieldNum._FailResultDropItemTableID	= pEnchantTable->GetFieldNum( "_FailResultDropItemTableID" );
	sFieldNum._PotentialID					= pEnchantTable->GetFieldNum( "_PotentialID" );
	sFieldNum._PotentialRatio				= pEnchantTable->GetFieldNum( "_PotentialRatio" );
	sFieldNum._BreakRatio					= pEnchantTable->GetFieldNum( "_BreakRatio" );
	sFieldNum._MinDown						= pEnchantTable->GetFieldNum( "_MinDown" );
	sFieldNum._MaxDown						= pEnchantTable->GetFieldNum( "_MaxDown" );
	sFieldNum._ProtectItemCount				= pEnchantTable->GetFieldNum( "_ProtectItemCount" );
#if defined(PRE_ADD_ENCHANTSHIELD_CASHITEM)
	sFieldNum._ShieldItemCount				= pEnchantTable->GetFieldNum( "_ShieldItemCount" );
#endif

	sFieldNum._NeedItem.reserve(ENCHANTITEMMAX);
	sFieldNum._NeedItemCount.reserve(ENCHANTITEMMAX);
	for( int j=0; j<ENCHANTITEMMAX; j++ ) 
	{
		sprintf_s( szTemp, "_NeedItemID%d", j + 1 );
		sFieldNum._NeedItem.push_back( pEnchantTable->GetFieldNum(szTemp) );
		sprintf_s( szTemp, "_NeedItemCount%d", j + 1 );
		sFieldNum._NeedItemCount.push_back( pEnchantTable->GetFieldNum(szTemp) );
	}

	//##################################################################
	// Load
	//##################################################################

	for( int i=0; i<pEnchantTable->GetItemCount(); i++ ) 
	{
		int nItemID = pEnchantTable->GetItemID(i);

		int iIdx = pEnchantTable->GetIDXprimary( nItemID );

		int nEnchantID = pEnchantTable->GetFieldFromLablePtr( iIdx, sFieldNum._EnchantID )->GetInteger();
		if( nEnchantID < 1 ) 
			continue;
		char cLevel = (char)pEnchantTable->GetFieldFromLablePtr( iIdx, sFieldNum._EnchantLevel )->GetInteger();
		if( cLevel < 1 ) 
			continue;

		TEnchantData *pData = NULL;
		TMapEnchantData::iterator it = m_pEnchantTable.find(nEnchantID);
		if( it != m_pEnchantTable.end() ) 
		{
			pData = it->second;
			if( pData->pMapLevelData.find(cLevel) != pData->pMapLevelData.end() ) 
			{
				g_Log.Log( LogType::_FILELOG, L"EnchantTable.dnt 같은 레벨의 인첸트 발견. 뒤에것 무시됩니다.( EnchantID : %d, TableID : %d, Level : %d\r\n", nEnchantID, nItemID, cLevel );
				continue;
			}
		}
		else 
		{
			pData = new TEnchantData;
			pData->nEnchantID = nEnchantID;
			pData->cCount = 0;
			m_pEnchantTable.insert( make_pair( nEnchantID, pData ) );
		}

		TEnchantLevelData *pLevelData = new TEnchantLevelData;
		pLevelData->nItemID = nItemID;
		pLevelData->cLevel = cLevel;
		_fpreset();
		pLevelData->cEnchantProb = (char)(( pEnchantTable->GetFieldFromLablePtr( iIdx, sFieldNum._EnchantRatio )->GetFloat() + 0.0001f ) * 100.f );
		for( int j=0; j<ENCHANTITEMMAX; j++ ) 
		{
			pLevelData->nNeedItemID[j]		= pEnchantTable->GetFieldFromLablePtr( iIdx, sFieldNum._NeedItem[j] )->GetInteger();
			pLevelData->nNeedItemCount[j]	= pEnchantTable->GetFieldFromLablePtr( iIdx, sFieldNum._NeedItemCount[j] )->GetInteger();
		}
		pLevelData->nNeedCoin			= pEnchantTable->GetFieldFromLablePtr( iIdx, sFieldNum._NeedCoin )->GetInteger();
		pLevelData->nFailResultItemID	= pEnchantTable->GetFieldFromLablePtr( iIdx, sFieldNum._FailResultDropItemTableID )->GetInteger();
		if(pLevelData->nFailResultItemID > 0)
		{
			pLevelData->nFailResultItemID = 0;
			//hgoori
			//현재 테스트용 인챈트 1번이 이 값을 사용하고 있지만, 게임에서는 쓰이지 않습니다.
			//불필요한 로그를 찍고 있기에 필요에 따라 이 후에 로그를 추가하도록 하겠습니다.
			//g_Log.Log(LogType::_FILELOG, L"Not available enchant fail result item.( EnchantID : %d, TableID : %d, Level : %d\r\n", nEnchantID, nItemID, cLevel );
		}
		pLevelData->nPotentialID		= pEnchantTable->GetFieldFromLablePtr( iIdx, sFieldNum._PotentialID )->GetInteger();
		_fpreset();
		pLevelData->cPotentialProb = (char)(( pEnchantTable->GetFieldFromLablePtr( iIdx, sFieldNum._PotentialRatio )->GetFloat() + 0.0001f ) * 100.f );
		_fpreset();
		pLevelData->cBreakRatio = (char)(( pEnchantTable->GetFieldFromLablePtr( iIdx, sFieldNum._BreakRatio )->GetFloat() + 0.0001f ) * 100.f );
		pLevelData->cMinDownLevel = (char)pEnchantTable->GetFieldFromLablePtr( iIdx, sFieldNum._MinDown )->GetInteger();
		pLevelData->cMaxDownLevel = (char)pEnchantTable->GetFieldFromLablePtr( iIdx, sFieldNum._MaxDown )->GetInteger();
		pLevelData->nProtectItemCount = pEnchantTable->GetFieldFromLablePtr( iIdx, sFieldNum._ProtectItemCount )->GetInteger();
#if defined(PRE_ADD_ENCHANTSHIELD_CASHITEM)
		pLevelData->nShieldItemCount = pEnchantTable->GetFieldFromLablePtr( iIdx, sFieldNum._ShieldItemCount )->GetInteger();
#endif

		pData->pMapLevelData.insert( make_pair( cLevel, pLevelData ) );
		pData->cCount++;
	}
#endif // PRE_FIX_MEMOPT_ENCHANT

	return true;
}

#ifdef PRE_FIX_MEMOPT_ENCHANT
bool CDNGameDataManager::LoadEnchantMainTable(const DNTableFileFormat& mainTable)
{
	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{
		int	_EnchantID;
		int	_EnchantLevel;
		int _NeedItemTableID;
		int _StateTableID;
		int _NeedCoin;
	};

	TempFieldNum sFieldNum;
	sFieldNum._EnchantID = mainTable.GetFieldNum( "_EnchantID" );
	sFieldNum._EnchantLevel = mainTable.GetFieldNum( "_EnchantLevel" );
	sFieldNum._NeedItemTableID = mainTable.GetFieldNum( "_NeedItemTableID" );
	sFieldNum._StateTableID = mainTable.GetFieldNum( "_StateID" );
	sFieldNum._NeedCoin = mainTable.GetFieldNum( "_NeedCoin" );

	//##################################################################
	// Load
	//##################################################################

	int i = 0;
	for (; i < mainTable.GetItemCount(); i++ ) 
	{
		int nItemID = mainTable.GetItemID(i);

		int iIdx = mainTable.GetIDXprimary( nItemID );

		int nEnchantID = mainTable.GetFieldFromLablePtr(iIdx, sFieldNum._EnchantID)->GetInteger();
		if (nEnchantID < 1)
			continue;
		char cLevel = (char)mainTable.GetFieldFromLablePtr(iIdx, sFieldNum._EnchantLevel)->GetInteger();
		if (cLevel < 1)
			continue;

		TEnchantData *pData = NULL;
		TMapEnchantData::iterator it = m_pEnchantTable.find(nEnchantID);
		if (it != m_pEnchantTable.end())
		{
			pData = it->second;
			if (pData->pMapLevelData.find(cLevel) != pData->pMapLevelData.end())
			{
				g_Log.Log(LogType::_FILELOG, L"EnchantTable.dnt 같은 레벨의 인첸트 발견. 뒤에것 무시됩니다.( EnchantID : %d, TableID : %d, Level : %d\r\n", nEnchantID, nItemID, cLevel);
				continue;
			}
		}
		else 
		{
			pData = new TEnchantData;
			pData->nEnchantID = nEnchantID;
			pData->cCount = 0;
			m_pEnchantTable.insert(make_pair(nEnchantID, pData));
		}

		TEnchantLevelData *pLevelData = new TEnchantLevelData;
		pLevelData->nItemID = nItemID;
		pLevelData->cLevel = cLevel;
		pLevelData->nNeedItemTableID = mainTable.GetFieldFromLablePtr(iIdx, sFieldNum._NeedItemTableID)->GetInteger();
		pLevelData->nStateTableID = mainTable.GetFieldFromLablePtr(iIdx, sFieldNum._StateTableID)->GetInteger();
		pLevelData->nNeedCoin = mainTable.GetFieldFromLablePtr(iIdx, sFieldNum._NeedCoin)->GetInteger();

		pData->pMapLevelData.insert(make_pair(cLevel, pLevelData));
		pData->cCount++;
	}

	return true;
}

bool CDNGameDataManager::LoadEnchantNeedItemTable(const DNTableFileFormat& needItemTable)
{
	m_EnchantNeedItemTable.clear();

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################
	struct TempFieldNum
	{
		int	_EnchantRatio;
		int	_BreakRatio;
		int _MinDown;
		int _MaxDown;
		std::vector<int> _NeedItem;
		std::vector<int> _NeedItemCount;
		int _UpStateRatio;
		int	_FailResultDropItemTableID;
		int	_ProtectItemCount;
#if defined(PRE_ADD_ENCHANTSHIELD_CASHITEM)
		int	_ShieldItemCount; // 상위 강화보호젤리
#endif
	};

	char szTemp[MAX_PATH];
	TempFieldNum sFieldNum;
	sFieldNum._EnchantRatio = needItemTable.GetFieldNum("_EnchantRatio");
	sFieldNum._BreakRatio = needItemTable.GetFieldNum("_EnchantLevel");
	sFieldNum._MinDown = needItemTable.GetFieldNum("_MinDown");
	sFieldNum._MaxDown = needItemTable.GetFieldNum("_MaxDown");
	sFieldNum._NeedItem.reserve(ENCHANTITEMMAX);
	sFieldNum._NeedItemCount.reserve(ENCHANTITEMMAX);
	for (int j = 0; j < ENCHANTITEMMAX; j++)
	{
		sprintf_s(szTemp, "_NeedItemID%d", j + 1);
		sFieldNum._NeedItem.push_back(needItemTable.GetFieldNum(szTemp));
		sprintf_s(szTemp, "_NeedItemCount%d", j + 1);
		sFieldNum._NeedItemCount.push_back(needItemTable.GetFieldNum(szTemp));
	}
	sFieldNum._UpStateRatio = needItemTable.GetFieldNum("_UpStateRatio");
	sFieldNum._FailResultDropItemTableID = needItemTable.GetFieldNum("_FailResultDropItemTableID");
	sFieldNum._ProtectItemCount = needItemTable.GetFieldNum("_ProtectItemCount");
#if defined(PRE_ADD_ENCHANTSHIELD_CASHITEM)
	sFieldNum._ShieldItemCount = needItemTable.GetFieldNum("_ShieldItemCount"); // 상위 강화보호젤리
#endif

	//##################################################################
	// Load
	//##################################################################
	int i = 0;
	for (; i < needItemTable.GetItemCount(); i++ )
	{
		TEnchantNeedItemData curData;
		int nItemID = needItemTable.GetItemID(i);
		int iIdx = needItemTable.GetIDXprimary(nItemID);

		_fpreset();
		curData.cEnchantProb = (char)((needItemTable.GetFieldFromLablePtr(iIdx, sFieldNum._EnchantRatio)->GetFloat() + 0.0001f) * 100.f);
		_fpreset();
		curData.cBreakRatio = (char)((needItemTable.GetFieldFromLablePtr(iIdx, sFieldNum._BreakRatio)->GetFloat() + 0.0001f) * 100.f);

		curData.cMinDownLevel = needItemTable.GetFieldFromLablePtr(iIdx, sFieldNum._MinDown)->GetInteger();
		curData.cMaxDownLevel = needItemTable.GetFieldFromLablePtr(iIdx, sFieldNum._MaxDown)->GetInteger();

		int j = 0;
		for (; j < ENCHANTITEMMAX; j++)
		{
			curData.nNeedItemID[j] = needItemTable.GetFieldFromLablePtr(iIdx, sFieldNum._NeedItem[j])->GetInteger();
			curData.nNeedItemCount[j] = needItemTable.GetFieldFromLablePtr(iIdx, sFieldNum._NeedItemCount[j])->GetInteger();
		}

		curData.fUpStateRatio = needItemTable.GetFieldFromLablePtr(iIdx, sFieldNum._UpStateRatio)->GetFloat();
		curData.nFailResultItemID = needItemTable.GetFieldFromLablePtr(iIdx, sFieldNum._FailResultDropItemTableID)->GetInteger();
		if (curData.nFailResultItemID > 0)
		{
			curData.nFailResultItemID = 0;
			//hgoori
			//현재 테스트용 인챈트 1번이 이 값을 사용하고 있지만, 게임에서는 쓰이지 않습니다.
			//불필요한 로그를 찍고 있기에 필요에 따라 이 후에 로그를 추가하도록 하겠습니다.
			//g_Log.Log(LogType::_FILELOG, L"Not available enchant fail result item.( EnchantID : %d, TableID : %d, Level : %d\r\n", nEnchantID, nItemID, cLevel );
		}

		curData.nProtectItemCount = needItemTable.GetFieldFromLablePtr(iIdx, sFieldNum._ProtectItemCount)->GetInteger();
#if defined(PRE_ADD_ENCHANTSHIELD_CASHITEM)
		curData.nShieldItemCount = needItemTable.GetFieldFromLablePtr(iIdx, sFieldNum._ShieldItemCount)->GetInteger();
#endif

		m_EnchantNeedItemTable.insert(std::make_pair(nItemID, curData));
	}

	return true;
}
#endif // PRE_FIX_MEMOPT_ENCHANT

bool CDNGameDataManager::LoadPotentialData()
{
#ifdef _WORK
	// 포텐셜 테이블 읽기
	DNTableFileFormat *pPotentialTable;
	if (m_bAllLoaded)
		pPotentialTable = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TPOTENTIAL );
	else
		pPotentialTable = GetDNTable( CDnTableDB::TPOTENTIAL );
#else		//#ifdef _WORK
	// 포텐셜 테이블 읽기
	DNTableFileFormat *pPotentialTable = GetDNTable( CDnTableDB::TPOTENTIAL );
#endif		//#ifdef _WORK
	if( !pPotentialTable ) {
		g_Log.Log(LogType::_FILELOG, L"PotentialTable.dnt failed\r\n");
		return false;
	}

	if( pPotentialTable->GetItemCount() <= 0 ) {
		g_Log.Log(LogType::_FILELOG, L"PotentialTable.dnt Count(%d)\r\n", pPotentialTable->GetItemCount());
		return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		SAFE_DELETE_PMAPF( TMapPotentialData, m_pPotentialTable, SAFE_DELETE_PVEC( TMapPotentialData_iter->second->pVecItemData ) );
	}
#endif		//#ifdef _WORK

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{
		int _PotentialID;
		int	_PotentialRatio;
		int _PotentialNo;

		int _PotentialSkillID;
		int _PotentialSkillLevel;
		int _PotentialSkillUsingType;
	};

	TempFieldNum sFieldNum;
	sFieldNum._PotentialID		= pPotentialTable->GetFieldNum( "_PotentialID" );
	sFieldNum._PotentialRatio	= pPotentialTable->GetFieldNum( "_PotentialRatio" );
	sFieldNum._PotentialNo		= pPotentialTable->GetFieldNum( "_PotentialNo" );

	sFieldNum._PotentialSkillID			= pPotentialTable->GetFieldNum( "_SkillID" );
	sFieldNum._PotentialSkillLevel		= pPotentialTable->GetFieldNum( "_SkillLevel" );
	sFieldNum._PotentialSkillUsingType	= pPotentialTable->GetFieldNum( "_SkillUsingType" );

	//##################################################################
	// Load
	//##################################################################

	for( int i=0; i<pPotentialTable->GetItemCount(); i++ ) 
	{
		int nItemID = pPotentialTable->GetItemID(i);

		int iIdx = pPotentialTable->GetIDXprimary( nItemID );

		int nPotentialID = pPotentialTable->GetFieldFromLablePtr( iIdx, sFieldNum._PotentialID )->GetInteger();
		if( nPotentialID < 1 ) 
			continue;

		TPotentialData *pData = NULL;
		TMapPotentialData::iterator it = m_pPotentialTable.find( nPotentialID );
		if( it != m_pPotentialTable.end() ) 
		{
			pData = it->second;
		}
		else
		{
			pData = new TPotentialData;
			pData->nPotentialID = nPotentialID;
			pData->nTotalProb = 0;
			m_pPotentialTable.insert( make_pair( nPotentialID, pData ) );
		}

		TPotentialDataItem *pItem = new TPotentialDataItem;
		pItem->nItemID = nItemID;
		_fpreset();
		int nProb = (int)(( pPotentialTable->GetFieldFromLablePtr( iIdx, sFieldNum._PotentialRatio )->GetFloat() + 0.001f ) * 100.f );
		pItem->nPotentailOffset = pData->nTotalProb + nProb;
		pItem->nPotentialNo = pPotentialTable->GetFieldFromLablePtr( iIdx, sFieldNum._PotentialNo )->GetInteger();
		pItem->nProb = nProb;
		pData->nTotalProb += nProb;

		pItem->nSkillID = pPotentialTable->GetFieldFromLablePtr( iIdx, sFieldNum._PotentialSkillID )->GetInteger();
		pItem->nSkillLevel = pPotentialTable->GetFieldFromLablePtr( iIdx, sFieldNum._PotentialSkillLevel )->GetInteger();
		pItem->nSkillUsingType = pPotentialTable->GetFieldFromLablePtr( iIdx, sFieldNum._PotentialSkillUsingType )->GetInteger();

		pData->pVecItemData.push_back( pItem );
	}
	return true;
}

TEnchantData *CDNGameDataManager::GetEnchantData( int nEnchantID ) const
{
	TMapEnchantData::const_iterator it = m_pEnchantTable.find( nEnchantID );
	if( it == m_pEnchantTable.end() ) return NULL;
	return it->second;
}

TEnchantLevelData *CDNGameDataManager::GetEnchantLevelData( int nEnchantID, char cLevel )
{
	TEnchantData *pData = GetEnchantData( nEnchantID );
	if( !pData ) return NULL;
	std::map<char, TEnchantLevelData*>::iterator it = pData->pMapLevelData.find( cLevel );
	if( it == pData->pMapLevelData.end() ) return NULL;
	return it->second;
}

#ifdef PRE_FIX_MEMOPT_ENCHANT
const TEnchantNeedItemData* CDNGameDataManager::GetEnchantNeedItemData(int nEnchantID, char cLevel) const
{
	const TEnchantData* pData = GetEnchantData(nEnchantID);
	if (pData == NULL)
		return NULL;

	std::map<char, TEnchantLevelData*>::const_iterator it = pData->pMapLevelData.find(cLevel);
	if (it == pData->pMapLevelData.end())
		return NULL;

	const TEnchantLevelData* pLevelData = (it->second);
	if (pLevelData == NULL)
		return NULL;

	TMapEnchantNeedItemData::const_iterator needItemDataIter = m_EnchantNeedItemTable.find(pLevelData->nNeedItemTableID);
	if (needItemDataIter == m_EnchantNeedItemTable.end())
		return NULL;

	return &(needItemDataIter->second);
}

const TEnchantNeedItemData* CDNGameDataManager::GetEnchantNeedItemData(const TEnchantLevelData& levelData) const
{
	TMapEnchantNeedItemData::const_iterator needItemDataIter = m_EnchantNeedItemTable.find(levelData.nNeedItemTableID);
	if (needItemDataIter == m_EnchantNeedItemTable.end())
		return NULL;

	return &(needItemDataIter->second);
}
#endif

int CDNGameDataManager::GetEnchantMaxLevel( int nEnchantID )
{
	TEnchantData *pData = GetEnchantData( nEnchantID );
	if( !pData ) return 0;

	return pData->cCount;
}

TPotentialData *CDNGameDataManager::GetPotentialData( int nPotentailID )
{
	TMapPotentialData::iterator it = m_pPotentialTable.find( nPotentailID );
	if( it == m_pPotentialTable.end() ) return NULL;

	return it->second;
}


bool CDNGameDataManager::LoadItemDropData()
{
#ifdef _WORK
	DNTableFileFormat *pSox;
	if (m_bAllLoaded)
		pSox = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TITEMDROP );
	else
		pSox = GetDNTable( CDnTableDB::TITEMDROP );
#else		//#ifdef _WORK
	DNTableFileFormat *pSox = GetDNTable( CDnTableDB::TITEMDROP );
#endif		//#ifdef _WORK
	if( !pSox ) {
		g_Log.Log(LogType::_FILELOG, L"ItemDropTable.dnt failed\r\n");
		return false;
	}

	if( pSox->GetItemCount() <= 0 ) {
		g_Log.Log(LogType::_FILELOG, L"ItemDropTable.dnt Count(%d)\r\n", pSox->GetItemCount());
		return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		SAFE_DELETE_PMAP( TMapItemDropData, m_pItemDropTable );
	}
#endif		//#ifdef _WORK

	char szLabel[32];

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{
		int _IsGroup;
		int	_ItemIndex;
		int	_ItemProb;
		int	_ItemInfo;
	};

	std::vector<TempFieldNum> vFieldNum;
	vFieldNum.reserve( 20 );

	for( int j=0; j<20; j++ ) 
	{
		TempFieldNum Temp;

		sprintf_s( szLabel, "_IsGroup%d", j + 1 );
		Temp._IsGroup = pSox->GetFieldNum( szLabel );

		sprintf_s( szLabel, "_Item%dIndex", j + 1 );
		Temp._ItemIndex = pSox->GetFieldNum( szLabel );

		sprintf_s( szLabel, "_Item%dProb", j + 1 );
		Temp._ItemProb = pSox->GetFieldNum( szLabel );

		sprintf_s( szLabel, "_Item%dInfo", j + 1 );
		Temp._ItemInfo = pSox->GetFieldNum( szLabel );

		vFieldNum.push_back( Temp );
	}
	
	//##################################################################
	// Load
	//##################################################################

	for( int i=0; i<pSox->GetItemCount(); i++ ) 
	{
		int nItemID = pSox->GetItemID(i);
		if( nItemID < 1 ) 
			continue;

		TItemDropData *pData = new TItemDropData;

		pData->nItemDropID = nItemID;

		int iIdx = pSox->GetIDXprimary( nItemID );

		for( int j=0; j<20; j++ ) 
		{
			pData->bIsGroup[j]	= ( pSox->GetFieldFromLablePtr( iIdx, vFieldNum[j]._IsGroup )->GetInteger() == TRUE ) ? true : false;
			pData->nIndex[j]	= pSox->GetFieldFromLablePtr( iIdx, vFieldNum[j]._ItemIndex )->GetInteger();
			pData->nProb[j]		= pSox->GetFieldFromLablePtr( iIdx, vFieldNum[j]._ItemProb )->GetInteger();
			pData->nInfo[j]		= pSox->GetFieldFromLablePtr( iIdx, vFieldNum[j]._ItemInfo )->GetInteger();
		}
		m_pItemDropTable.insert( make_pair( nItemID, pData ) );
	}

	return true;
}

TItemDropData *CDNGameDataManager::GetItemDropData( int nDropItemTableID )
{
	TMapItemDropData::iterator it = m_pItemDropTable.find( nDropItemTableID );
	if( it == m_pItemDropTable.end() ) return NULL;

	return it->second;
}

// Appellation
int CDNGameDataManager::LoadAppellationData()
{
#ifdef _WORK
	DNTableFileFormat *pAppellationTable;
	if (m_bAllLoaded)
	{
		pAppellationTable = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TAPPELLATION );
	}
	else
	{
		pAppellationTable = GetDNTable( CDnTableDB::TAPPELLATION );
	}	
#else		//#ifdef _WORK
	DNTableFileFormat *pAppellationTable = GetDNTable( CDnTableDB::TAPPELLATION );
#endif		//#ifdef _WORK
	if( !pAppellationTable ) {
		g_Log.Log(LogType::_FILELOG, L"AppellationTable.dnt failed\r\n");
		return false;
	}

	if( pAppellationTable->GetItemCount() <= 0 ) {
		g_Log.Log( LogType::_FILELOG, L"AppellationTable.dnt Count(%d)\r\n", pAppellationTable->GetItemCount());
		return false;
	}

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{
		int	_LevelLimit;
		int	_Color;
		int _Type;
#if defined( PRE_ADD_NEWCOMEBACK )
		int _SkillLinkItemID;		
#endif
	};

	TempFieldNum sFieldNum;
	sFieldNum._LevelLimit = pAppellationTable->GetFieldNum( "_LevelLimit" );
	sFieldNum._Color = pAppellationTable->GetFieldNum( "_Color" );
	sFieldNum._Type = pAppellationTable->GetFieldNum( "_Type" );	
#if defined( PRE_ADD_NEWCOMEBACK )
	sFieldNum._SkillLinkItemID = pAppellationTable->GetFieldNum( "_SkillLinkItemID" );	
#endif

	//##################################################################
	// Load
	//##################################################################

	for( int i=0; i<pAppellationTable->GetItemCount(); i++ ) 
	{
		int nItemID = pAppellationTable->GetItemID(i);
		int iIdx = pAppellationTable->GetIDXprimary( nItemID );

		TAppellationData AppellationData = {0,};

		AppellationData.nAppellationID = nItemID;
		AppellationData.nArrayIndex = pAppellationTable->GetArrayIndex( nItemID );
		AppellationData.nColor = pAppellationTable->GetFieldFromLablePtr( iIdx, sFieldNum._Color )->GetInteger();
		AppellationData.nLevelLimit = pAppellationTable->GetFieldFromLablePtr( iIdx, sFieldNum._LevelLimit )->GetInteger();
		AppellationData.nType = pAppellationTable->GetFieldFromLablePtr( iIdx, sFieldNum._Type )->GetInteger();
#if defined( PRE_ADD_NEWCOMEBACK )
		AppellationData.nSkillItemID = pAppellationTable->GetFieldFromLablePtr( iIdx, sFieldNum._SkillLinkItemID )->GetInteger();		
#endif

		m_MapAppellationData.insert(make_pair(AppellationData.nAppellationID, AppellationData));

		switch (AppellationData.nType)
		{
		case AppellationType::Type::Normal:
			break;

		case AppellationType::Type::Cash:
			break;

		case AppellationType::Type::Period:
#if defined( PRE_ADD_NEWCOMEBACK )
		case AppellationType::Type::ComeBack:
#endif
			{
				m_mPeriodAppellation.insert( std::make_pair(AppellationData.nAppellationID, AppellationData.nArrayIndex) );
			}
			break;

		case AppellationType::Type::Guild:
			{
				m_mGuildAppellation.insert( std::make_pair(AppellationData.nAppellationID, AppellationData.nArrayIndex) );
			}
			break;
#if defined( PRE_ADD_BESTFRIEND )
		case AppellationType::Type::BestFriend:
			{
				m_mBestFriendAppellation.insert( std::make_pair(AppellationData.nAppellationID, AppellationData.nArrayIndex) );
			}
			break;
#endif // #if defined( PRE_ADD_BESTFRIEND )
		}
	}

	return true;
}

TAppellationData *CDNGameDataManager::GetAppellationData(int nAppellationItemID)
{
	TMapAppellationData::iterator iter = m_MapAppellationData.find(nAppellationItemID);
	if (iter == m_MapAppellationData.end()) return NULL;

	return &(iter->second);
}

int CDNGameDataManager::GetAppellationArrayIndex(int nAppellationItemID)
{
	int nIndex = -1;
	TAppellationData *pData = GetAppellationData(nAppellationItemID);
	if (pData)
		nIndex = pData->nArrayIndex;

	return nIndex;
}

int CDNGameDataManager::GetAppellationLevelLimit(int nAppellationItemID)
{
	int nLevel = 0;
	TAppellationData *pData = GetAppellationData(nAppellationItemID);
	if (pData)
		nLevel = pData->nLevelLimit;

	return nLevel;
}

void CDNGameDataManager::ResetPeriodAppellation(char *Appellation, std::map<int, int> &mapDelAppellation)
{
	for(TMapPeriodAppellationID::iterator itor = m_mPeriodAppellation.begin(); itor != m_mPeriodAppellation.end(); itor++)
	{
		if( GetBitFlag(Appellation, itor->second) == true )
		{
			SetBitFlag( Appellation, itor->second, false );
			mapDelAppellation.insert( std::make_pair(itor->second,itor->first) );					
		}
	}
	return;
}

int CDNGameDataManager::GetPeriodAppellationArrayID(int nItemID)
{	
	TMapPeriodAppellationID::iterator itor = m_mPeriodAppellation.find(nItemID);
	if(itor == m_mPeriodAppellation.end())
		return -1;

	return itor->second;
}

void CDNGameDataManager::ResetGuildAppellation(char *Appellation, std::map<int, int> &mapDelAppellation)
{
	for(TMapGuildAppellationID::iterator itor = m_mGuildAppellation.begin(); itor != m_mGuildAppellation.end(); itor++)
	{
		if( GetBitFlag(Appellation, itor->second) == true )
		{
			SetBitFlag( Appellation, itor->second, false );
			mapDelAppellation.insert( std::make_pair(itor->second,itor->first) );					
		}
	}
	return;
}

#if defined( PRE_ADD_BESTFRIEND )
void CDNGameDataManager::ResetBestFriendAppellation(char *Appellation, std::map<int, int> &mapDelAppellation)
{
	for(TMapBestFriendAppellationID::iterator itor = m_mBestFriendAppellation.begin(); itor != m_mBestFriendAppellation.end(); itor++)
	{
		if( GetBitFlag(Appellation, itor->second) == true )
		{
			SetBitFlag( Appellation, itor->second, false );
			mapDelAppellation.insert( std::make_pair(itor->second,itor->first) );					
		}
	}
	return;
}
#endif

bool CDNGameDataManager::LoadMissionData()
{
#ifdef _WORK
	DNTableFileFormat *pTable;
	if (m_bAllLoaded)
	{
		pTable = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TMISSION );
	}
	else
	{
		pTable = GetDNTable( CDnTableDB::TMISSION );
	}	
#else		//#ifdef _WORK
	DNTableFileFormat *pTable = GetDNTable( CDnTableDB::TMISSION );
#endif		//#ifdef _WORK
	if( !pTable ) {
		g_Log.Log(LogType::_FILELOG, L"MissionTable.dnt failed\r\n");
		return false;
	}

	if( pTable->GetItemCount() <= 0 ) {
		g_Log.Log( LogType::_FILELOG, L"MissionTable.dnt Count(%d)\r\n", pTable->GetItemCount());
		return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		SAFE_DELETE_PVEC( m_pMissionTable );
	}
#endif		//#ifdef _WORK

	char szTemp[MAX_PATH];

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{
		int	_Activate;
		int	_Type;
		int	_nMailID;
		int	_RewardAppellation;
		int	_RewardPoint;
		int	_GainEvent;
		int	_AchieveEvent;
		int	_IsAchieveNotice;
		std::vector<int> _GainType;
		std::vector<int> _GainParam;
		std::vector<int> _GainOperator;
		int	_GainCheckType;
		std::vector<int> _AchieveType;
		std::vector<int> _AchieveParam;
		std::vector<int> _AchieveOperator;
		int	_AchieveCheckType;
	};

	TempFieldNum sFieldNum;
	sFieldNum._Activate 	= pTable->GetFieldNum( "_Activate" );
	sFieldNum._Type	= pTable->GetFieldNum( "_Type" );
	sFieldNum._nMailID = pTable->GetFieldNum( "_MailID" );
	sFieldNum._RewardAppellation = pTable->GetFieldNum( "_RewardAppellation" );
	sFieldNum._RewardPoint = pTable->GetFieldNum( "_RewardPoint" );
	sFieldNum._IsAchieveNotice = pTable->GetFieldNum( "_IsAchieveNotice" );
	sFieldNum._GainEvent = pTable->GetFieldNum( "_GainEvent" );
	sFieldNum._AchieveEvent	= pTable->GetFieldNum( "_AchieveEvent" );

	sFieldNum._GainType.reserve(5);
	sFieldNum._GainParam.reserve(5);
	sFieldNum._GainOperator.reserve(5);
	sFieldNum._AchieveType.reserve(5);
	sFieldNum._AchieveParam.reserve(5);
	sFieldNum._AchieveOperator.reserve(5);

	for( int j=0; j<5; j++ ) 
	{
		sprintf_s( szTemp, "_Gain%dType", j + 1 );
		sFieldNum._GainType.push_back( pTable->GetFieldNum(szTemp) );
		sprintf_s( szTemp, "_Gain%dParam", j + 1 );
		sFieldNum._GainParam.push_back( pTable->GetFieldNum(szTemp) );
		sprintf_s( szTemp, "_Gain%dOperator", j + 1 );
		sFieldNum._GainOperator.push_back( pTable->GetFieldNum(szTemp) );
		sprintf_s( szTemp, "_Achieve%dType", j + 1 );
		sFieldNum._AchieveType.push_back( pTable->GetFieldNum(szTemp) );
		sprintf_s( szTemp, "_Achieve%dParam", j + 1 );
		sFieldNum._AchieveParam.push_back( pTable->GetFieldNum(szTemp) );
		sprintf_s( szTemp, "_Achieve%dOperator", j + 1 );
		sFieldNum._AchieveOperator.push_back( pTable->GetFieldNum(szTemp) );
	}

	sFieldNum._GainCheckType = pTable->GetFieldNum( "_GainCheckType" );
	sFieldNum._AchieveCheckType = pTable->GetFieldNum( "_AchieveCheckType" );

	//##################################################################
	// Load
	//##################################################################

	for( int i=0; i<pTable->GetItemCount(); i++ ) 
	{
		int nItemID = pTable->GetItemID(i);

		int iIdx = pTable->GetIDXprimary( nItemID );

		TMissionData *pData = new TMissionData;
		memset( pData, 0, sizeof(TMissionData) );

		pData->bActivate		= ( pTable->GetFieldFromLablePtr( iIdx, sFieldNum._Activate )->GetInteger() == TRUE ) ? true : false;
		pData->nType			= pTable->GetFieldFromLablePtr( iIdx, sFieldNum._Type )->GetInteger();
		pData->nMailID			= pTable->GetFieldFromLablePtr( iIdx, sFieldNum._nMailID )->GetInteger();
		int nRewardAppellationID = pTable->GetFieldFromLablePtr( iIdx, sFieldNum._RewardAppellation )->GetInteger();
		pData->nRewardAppellation = ( nRewardAppellationID < 1 ) ? -1 : GetAppellationArrayIndex( nRewardAppellationID );
		pData->bIsAchieveNotice	= ( pTable->GetFieldFromLablePtr( iIdx, sFieldNum._IsAchieveNotice )->GetInteger() == TRUE ) ? true : false;
		pData->nRewardPoint		= pTable->GetFieldFromLablePtr( iIdx, sFieldNum._RewardPoint )->GetInteger();
		pData->GainCondition.cEvent	= (char)pTable->GetFieldFromLablePtr( iIdx, sFieldNum._GainEvent )->GetInteger();
		pData->AchieveCondition.cEvent	= (char)pTable->GetFieldFromLablePtr( iIdx, sFieldNum._AchieveEvent )->GetInteger();

		if (pData->bActivate)
		{
			if (pData->GainCondition.cEvent >= EventSystem::EventTypeEnum_Amount)
			{
				g_Log.Log( LogType::_FILELOG, L"MissionTable.dnt Invalid GainEvent\n");
				return false;
			}
			if (pData->AchieveCondition.cEvent >= EventSystem::EventTypeEnum_Amount)
			{
				g_Log.Log( LogType::_FILELOG, L"MissionTable.dnt Invalid AchieveEvent\n");
				return false;
			}
		}

		for( int j=0; j<5; j++ ) 
		{
			pData->GainCondition.cType[j]			= (char)pTable->GetFieldFromLablePtr( iIdx, sFieldNum._GainType[j] )->GetInteger();
			pData->GainCondition.nParam[j]			= pTable->GetFieldFromLablePtr( iIdx, sFieldNum._GainParam[j] )->GetInteger();
			pData->GainCondition.cOperator[j]		= (char)pTable->GetFieldFromLablePtr( iIdx, sFieldNum._GainOperator[j] )->GetInteger();
			pData->AchieveCondition.cType[j]		= (char)pTable->GetFieldFromLablePtr( iIdx, sFieldNum._AchieveType[j] )->GetInteger();
			pData->AchieveCondition.nParam[j]		= pTable->GetFieldFromLablePtr( iIdx, sFieldNum._AchieveParam[j] )->GetInteger();
			pData->AchieveCondition.cOperator[j]	= (char)pTable->GetFieldFromLablePtr( iIdx, sFieldNum._AchieveOperator[j] )->GetInteger();

			if (pData->bActivate)
			{
				if (pData->GainCondition.nParam[j] < 0)
				{
					g_Log.Log( LogType::_FILELOG, L"MissionTable.dnt, GainParm < 0 [ItemID:%d]\n", nItemID);
					return false;
				}

				if (pData->AchieveCondition.nParam[j] < 0)
				{
					g_Log.Log( LogType::_FILELOG, L"MissionTable.dnt, AchieveParam < 0 [ItemID:%d]\n", nItemID);
					return false;
				}

				if (pData->GainCondition.cType[j] >= EventSystem::EventValueTypeEnum_Amount)
				{
					g_Log.Log( LogType::_FILELOG, L"MissionTable.dnt, Invalid GainType\n");
					return false;
				}

				if (CDNEventSystem::s_EventValueFuncList[pData->GainCondition.cType[j]].pFunc == NULL && pData->GainCondition.cType[j] > 0)
				{
					g_Log.Log( LogType::_FILELOG, L"MissionTable.dnt, Invalid s_EventValueFuncList[GainType], please check MissionAPI\n");
					return false;
				}

				if (pData->AchieveCondition.cType[j] >= EventSystem::EventValueTypeEnum_Amount)
				{
					g_Log.Log( LogType::_FILELOG, L"MissionTable.dnt, Invalid AchieveType\n");
					return false;
				}

				if (CDNEventSystem::s_EventValueFuncList[pData->AchieveCondition.cType[j]].pFunc == NULL && pData->AchieveCondition.cType[j] > 0)
				{
					g_Log.Log( LogType::_FILELOG, L"MissionTable.dnt, Invalid s_EventValueFuncList[AchieveType], please check MissionAPI\n");
					return false;
				}
			}
		}
		pData->GainCondition.bCheckType = ( pTable->GetFieldFromLablePtr( iIdx, sFieldNum._GainCheckType )->GetInteger() == TRUE ) ? true : false;
		pData->AchieveCondition.bCheckType = ( pTable->GetFieldFromLablePtr( iIdx, sFieldNum._AchieveCheckType )->GetInteger() == TRUE ) ? true : false;

		// 데이터 가공
		if( pData->nType == MissionType_MetaShow || pData->nType == MissionType_MetaHide )
		{
			// 메타업적이라면 AchieveParam을 미션 인덱스로 바꿔둔다.
			for( int l=0; l<5; l++ )
				pData->AchieveCondition.nParam[l]	= pTable->GetArrayIndex( pData->AchieveCondition.nParam[l] );
		}

		m_pMissionTable.push_back( pData );

		if (nItemID != m_pMissionTable.size())	//문제 없는지 확인해봐야됨
		{
			g_Log.Log( LogType::_FILELOG, L"MissionTable.dnt, missing MissionData [ItemID:%d]\n", nItemID-1);
			return false;
		}
	}
	return true;
}

TMissionData *CDNGameDataManager::GetMissionData( int nArrayIndex )
{
	if( nArrayIndex < 0 || nArrayIndex >= (int)m_pMissionTable.size() ) return NULL;
	return m_pMissionTable[nArrayIndex];
}

char CDNGameDataManager::GetMissionType(int nArrayIndex)
{
	if( nArrayIndex < 0 || nArrayIndex >= (int)m_pMissionTable.size() ) return NULL;
	return m_pMissionTable[nArrayIndex]->nType;
}

int CDNGameDataManager::GetMissionMailID(int nArrayIndex)
{
	if( nArrayIndex < 0 || nArrayIndex >= (int)m_pMissionTable.size() ) return NULL;
	return m_pMissionTable[nArrayIndex]->nMailID;
}

bool CDNGameDataManager::LoadDailyMissionData()
{
#ifdef _WORK
	DNTableFileFormat *pTable;
	if (m_bAllLoaded)
	{
		pTable = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TDAILYMISSION );
	}
	else
	{
		pTable = GetDNTable( CDnTableDB::TDAILYMISSION );
	}
#else		//#ifdef _WORK
	DNTableFileFormat *pTable = GetDNTable( CDnTableDB::TDAILYMISSION );
#endif		//#ifdef _WORK
	if( !pTable ) {
		g_Log.Log(LogType::_FILELOG, L"DailyMissionTable.dnt failed\r\n");
		return false;
	}

	if( pTable->GetItemCount() <= 0 ) {
		g_Log.Log(LogType::_FILELOG, L"DailyMissionTable.dnt Count(%d)\r\n", pTable->GetItemCount());
		return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		m_pGuildWarMissionTable.clear();
		m_pGuildCommonMissionTable.clear();
		m_pWeekendRepeatMissionTable.clear();

		SAFE_DELETE_PMAP( TMapDailyMissionData, m_pDailyMissionTable );
	}
#endif		//#ifdef _WORK

	char szTemp[MAX_PATH];

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{
		int	_MailID;
		int	_RewardAppellation;
		int	_RewardPoint;
		int	_AchieveEvent;
		std::vector<int> _AchieveType;
		std::vector<int> _AchieveParam;
		std::vector<int> _AchieveOperator;
		int	_AchieveCheckType;
		int	_CounterParam;
		int	_CounterOperator;
		int	_type;
		int	_typeParam;
		int _StartTime;
#if defined(PRE_ADD_WEEKLYEVENT_MISSION_ENDTIME)
		int _EndTime;
#endif	// #if defined(PRE_ADD_WEEKLYEVENT_MISSION_ENDTIME)
	};

	TempFieldNum sFieldNum;
	sFieldNum._MailID	= pTable->GetFieldNum( "_MailID" );
	sFieldNum._RewardAppellation	= pTable->GetFieldNum( "_RewardAppellation" );
	sFieldNum._RewardPoint	= pTable->GetFieldNum( "_RewardPoint" );
	sFieldNum._AchieveEvent	= pTable->GetFieldNum( "_AchieveEvent" );

	sFieldNum._AchieveType.reserve(5);
	sFieldNum._AchieveParam.reserve(5);
	sFieldNum._AchieveOperator.reserve(5);

	for( int j=0; j<5; j++ ) 
	{
		sprintf_s( szTemp, "_Achieve%dType", j + 1 );
		sFieldNum._AchieveType.push_back( pTable->GetFieldNum(szTemp) );
		sprintf_s( szTemp, "_Achieve%dParam", j + 1 );
		sFieldNum._AchieveParam.push_back( pTable->GetFieldNum(szTemp) );
		sprintf_s( szTemp, "_Achieve%dOperator", j + 1 );
		sFieldNum._AchieveOperator.push_back( pTable->GetFieldNum(szTemp) );
	}

	sFieldNum._AchieveCheckType = pTable->GetFieldNum( "_AchieveCheckType" );
	sFieldNum._CounterParam = pTable->GetFieldNum( "_CounterParam" );
	sFieldNum._CounterOperator = pTable->GetFieldNum( "_CounterOperator" );
	sFieldNum._type = pTable->GetFieldNum( "_type" );
	sFieldNum._typeParam = pTable->GetFieldNum( "_typeParam" );
	sFieldNum._StartTime = pTable->GetFieldNum( "_StartTime" );
#if defined(PRE_ADD_WEEKLYEVENT_MISSION_ENDTIME)
	sFieldNum._EndTime = pTable->GetFieldNum( "_EndTime" );
#endif	// #if defined(PRE_ADD_WEEKLYEVENT_MISSION_ENDTIME)

	//##################################################################
	// Load
	//##################################################################

	for( int i=0; i<pTable->GetItemCount(); i++ ) 
	{
		int nItemID = pTable->GetItemID(i);
		int iIdx = pTable->GetIDXprimary( nItemID );

		TDailyMissionData *pData = new TDailyMissionData;
		memset( pData, 0, sizeof(TDailyMissionData) );

		pData->nMailID = pTable->GetFieldFromLablePtr( iIdx, sFieldNum._MailID )->GetInteger();
		int nRewardAppellationID = pTable->GetFieldFromLablePtr( iIdx, sFieldNum._RewardAppellation )->GetInteger();
		pData->nRewardAppellation = ( nRewardAppellationID < 1 ) ? -1 : GetAppellationArrayIndex( nRewardAppellationID );
		pData->nRewardPoint	= pTable->GetFieldFromLablePtr( iIdx, sFieldNum._RewardPoint )->GetInteger();
		pData->AchieveCondition.cEvent = (char)pTable->GetFieldFromLablePtr( iIdx, sFieldNum._AchieveEvent )->GetInteger();
		if (pData->AchieveCondition.cEvent >= EventSystem::EventTypeEnum_Amount)
		{
			g_Log.Log( LogType::_FILELOG, L"DailyMissionTable.dnt Invalid AchieveEvent [ItemID:%d]\n", nItemID);		
			return false;
		}

		for( int j=0; j<5; j++ ) 
		{
			pData->AchieveCondition.cType[j] = (char)pTable->GetFieldFromLablePtr( iIdx, sFieldNum._AchieveType[j] )->GetInteger();
			pData->AchieveCondition.nParam[j] = pTable->GetFieldFromLablePtr( iIdx, sFieldNum._AchieveParam[j] )->GetInteger();
			pData->AchieveCondition.cOperator[j] = (char)pTable->GetFieldFromLablePtr( iIdx, sFieldNum._AchieveOperator[j] )->GetInteger();
			if (pData->AchieveCondition.nParam[j] < 0)
			{
				g_Log.Log( LogType::_FILELOG, L"DailyMissionTable.dnt, AchieveParam < 0 [ItemID:%d]\n", nItemID);
				return false;
			}

			if (pData->AchieveCondition.cType[j] >= EventSystem::EventValueTypeEnum_Amount)
			{
				g_Log.Log( LogType::_FILELOG, L"DailyMissionTable.dnt, Invalid AchieveType [ItemID:%d][Type:%d]\n", nItemID,pData->AchieveCondition.cType[j]);				
				return false;
			}

			if (CDNEventSystem::s_EventValueFuncList[pData->AchieveCondition.cType[j]].pFunc == NULL &&pData->AchieveCondition.cType[j] > 0)
			{
				g_Log.Log( LogType::_FILELOG, L"DailyMissionTable.dnt, Invalid s_EventValueFuncList[AchieveType], please check MissionAPI [ItemID:%d]\n", nItemID);
				return false;
			}
		}
		pData->AchieveCondition.bCheckType = ( pTable->GetFieldFromLablePtr( iIdx, sFieldNum._AchieveCheckType )->GetInteger() == TRUE ) ? true : false;
		pData->nCounterParam = pTable->GetFieldFromLablePtr( iIdx, sFieldNum._CounterParam )->GetInteger();
		pData->cCounterOperator = (char)pTable->GetFieldFromLablePtr( iIdx, sFieldNum._CounterOperator )->GetInteger();

		pData->nItemID = nItemID;
		pData->nType = pTable->GetFieldFromLablePtr( iIdx, sFieldNum._type )->GetInteger();
		pData->nTypeParam = pTable->GetFieldFromLablePtr( iIdx, sFieldNum._typeParam )->GetInteger();
		CTimeSet tStartSet(pTable->GetFieldFromLablePtr( iIdx, sFieldNum._StartTime )->GetString(), true);
		pData->tStartDate = tStartSet.GetTimeT64_LC();
#if defined(PRE_ADD_WEEKLYEVENT_MISSION_ENDTIME)
		CTimeSet tEndSet(pTable->GetFieldFromLablePtr( iIdx, sFieldNum._EndTime )->GetString(), true);
		pData->tEndDate = tEndSet.GetTimeT64_LC();
#endif	// #if defined(PRE_ADD_WEEKLYEVENT_MISSION_ENDTIME)

		if(pData->nType == DAILYMISSION_GUILDWAR)
			m_pGuildWarMissionTable.push_back(pData);
		else if(pData->nType == DAILYMISSION_GUILDCOMMON)
			m_pGuildCommonMissionTable.push_back(pData);
		else if(pData->nType == DAILYMISSION_WEEKENDREPEAT)
		{
			if (!tStartSet.CheckIntegrity())
			{
				g_Log.Log(LogType::_FILELOG, L"DailyMissionTable.dnt WEEKENDREPEAT Invalid StartTime failed [ItemID:%d]\r\n", nItemID);
#if defined (_FINAL_BUILD)
				return false;
#endif
			}
#if defined(PRE_ADD_WEEKLYEVENT_MISSION_ENDTIME)
			if (!tEndSet.CheckIntegrity())
			{
				g_Log.Log(LogType::_FILELOG, L"DailyMissionTable.dnt WEEKENDREPEAT Invalid EndTime failed [ItemID:%d]\r\n", nItemID);
#if defined (_FINAL_BUILD)
				return false;
#endif
			}
#endif	// #if defined(PRE_ADD_WEEKLYEVENT_MISSION_ENDTIME)
			m_pWeekendRepeatMissionTable.push_back(pData);
		}
		else if(pData->nType == DAILYMISSION_WEEKENDEVENT)
		{
			if (!tStartSet.CheckIntegrity())
			{
				g_Log.Log(LogType::_FILELOG, L"DailyMissionTable.dnt WEEKENDEVENT Invalid StartTime failed [ItemID:%d]\r\n", nItemID);
#if defined (_FINAL_BUILD)
				return false;
#endif
			}
#if defined(PRE_ADD_WEEKLYEVENT_MISSION_ENDTIME)
			if (!tEndSet.CheckIntegrity())
			{
				g_Log.Log(LogType::_FILELOG, L"DailyMissionTable.dnt WEEKENDEVENT Invalid EndTime failed [ItemID:%d]\r\n", nItemID);
#if defined (_FINAL_BUILD)
				return false;
#endif
			}
#endif	// #if defined(PRE_ADD_WEEKLYEVENT_MISSION_ENDTIME)
			m_pWeekendEventMissionTable.push_back(pData);
		}

		m_pDailyMissionTable.insert( make_pair( nItemID, pData ) );
	}
	return true;
}

TDailyMissionData *CDNGameDataManager::GetDailyMissionData( int nItemID )
{
	TMapDailyMissionData::iterator it = m_pDailyMissionTable.find( nItemID );
	if( it == m_pDailyMissionTable.end() ) return NULL;
	return it->second;
}

int CDNGameDataManager::GetDailyMissionRewarePoint(int nMissionID)
{
	TDailyMissionData *pMission = GetDailyMissionData(nMissionID);
	if(pMission) return pMission->nRewardPoint;
	return 0;
}

TDailyMissionData *CDNGameDataManager::GetWeekendEventMissionData( int nArrayIndex )
{
	if( nArrayIndex < 0 || nArrayIndex >= (int)m_pWeekendEventMissionTable.size() ) return NULL;
	return m_pWeekendEventMissionTable[nArrayIndex];
}

TDailyMissionData *CDNGameDataManager::GetGuildCommonMissionData( int nArrayIndex )
{
	if( nArrayIndex < 0 || nArrayIndex >= (int)m_pGuildCommonMissionTable.size() ) return NULL;
	return m_pGuildCommonMissionTable[nArrayIndex];
}

TDailyMissionData *CDNGameDataManager::GetWeekendRepeatMissionData( int nArrayIndex )
{
	if( nArrayIndex < 0 || nArrayIndex >= (int)m_pWeekendRepeatMissionTable.size() ) return NULL;
	return m_pWeekendRepeatMissionTable[nArrayIndex];
}

TDailyMissionData *CDNGameDataManager::GetGuildWarMissionData( int nArrayIndex )
{
	if( nArrayIndex < 0 || nArrayIndex >= (int)m_pGuildWarMissionTable.size() ) return NULL;
	return m_pGuildWarMissionTable[nArrayIndex];
}
#if defined(PRE_ADD_WEEKLYEVENT_MISSION_ENDTIME)
int CDNGameDataManager::GetWeekendMissionCount(eDailyMissionType eMissionType)
{
	switch(eMissionType)
	{
	case DAILYMISSION_WEEKENDEVENT:	return (int)m_pWeekendEventMissionTable.size();
	case DAILYMISSION_WEEKENDREPEAT:return (int)m_pWeekendRepeatMissionTable.size();
	}
	return -1;
}
#endif	// #if defined(PRE_ADD_WEEKLYEVENT_MISSION_ENDTIME)

void CDNGameDataManager::GetRandomGuildMission( int nRandomSeed, int* vGuildMissionID )
{
	std::vector<int> vMissionID[GUILDWARMISSION_MAXGROUP];
	std::vector<TDailyMissionData*>::iterator iter = m_pGuildWarMissionTable.begin();
	for(; iter!= m_pGuildWarMissionTable.end(); iter++)
	{
		TDailyMissionData* pMission = (*iter);
		if(!pMission) continue;
		if(pMission->nTypeParam >= GUILDWARMISSION_MAXGROUP) continue;

		vMissionID[pMission->nTypeParam].push_back(pMission->nItemID);
	}

	int nSize = 0;
	int nRandomValue = 0;
	for (int i=0; i<GUILDWARMISSION_MAXGROUP; i++)
	{
		nSize = (int)vMissionID[i].size();
		if(nSize <= 0) continue;

		nRandomValue = nRandomSeed % nSize;

		vGuildMissionID[i] = vMissionID[i][nRandomValue];
	}
}

#if defined(PRE_ADD_CHAT_MISSION)
bool CDNGameDataManager::LoadTypingData()
{
#ifdef _WORK
	DNTableFileFormat *pTable;
	if (m_bAllLoaded)
	{
		pTable = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TMISSIONTYPING );
	}
	else
	{
		pTable = GetDNTable( CDnTableDB::TMISSIONTYPING );
	}	
#else		//#ifdef _WORK
	DNTableFileFormat *pTable = GetDNTable( CDnTableDB::TMISSIONTYPING );
#endif		//#ifdef _WORK
	if( !pTable ) {
		g_Log.Log(LogType::_FILELOG, L"TypingTable.dnt failed\r\n");
		return false;
	}

	if( pTable->GetItemCount() <= 0 ) {
		g_Log.Log( LogType::_FILELOG, L"TypingTable.dnt Count(%d)\r\n", pTable->GetItemCount());
		return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		SAFE_DELETE_VEC( m_pTypingTable );
	}
#endif		//#ifdef _WORK

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{
		int	_Activate;
		int	_UIString;
	};

	TempFieldNum sFieldNum;
	sFieldNum._Activate 	= pTable->GetFieldNum( "_Activate" );
	sFieldNum._UIString	= pTable->GetFieldNum( "_UIString" );

	//##################################################################
	// Load
	//##################################################################

	for( int i=0; i<pTable->GetItemCount(); i++ ) 
	{
		int nItemID = pTable->GetItemID(i);

		int iIdx = pTable->GetIDXprimary( nItemID );
		if( pTable->GetFieldFromLablePtr( iIdx, sFieldNum._Activate )->GetInteger() == TRUE )
		{
			m_pTypingTable.push_back(pTable->GetFieldFromLablePtr( iIdx, sFieldNum._UIString )->GetInteger());
		}
	}

	return true;
}

bool CDNGameDataManager::CheckTypingData(int nUIStringMid)
{
	for( int i = 0 ; i < m_pTypingTable.size() ; i++)
	{
		if(m_pTypingTable[i] == nUIStringMid)
			return true;
	}

	return false;
}

#endif

bool CDNGameDataManager::LoadGuildWarRewardData()
{
#ifdef _WORK
	DNTableFileFormat *pTable;
	if (m_bAllLoaded)
		pTable = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TGUILDWARREWARD );	
	else
		pTable = GetDNTable( CDnTableDB::TGUILDWARREWARD );	
#else		//#ifdef _WORK
	DNTableFileFormat *pTable = GetDNTable( CDnTableDB::TGUILDWARREWARD );	
#endif		//#ifdef _WORK
	if( !pTable ) {
		g_Log.Log(LogType::_FILELOG, L"guildwarmailtable.dnt failed\r\n");
		return false;
	}	

	if( pTable->GetItemCount() <= 0 ) {
		g_Log.Log(LogType::_FILELOG, L"guildwarmailtable.dnt Count(%d)\r\n", pTable->GetItemCount());
		//return false;
	}	

#ifdef _WORK
	if (m_bAllLoaded)
	{
		SAFE_DELETE_PVEC( m_pGuildWarRewardData );
	}
#endif		//#ifdef _WORK

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{		
		int	_ResultType;
		int	_ClassID;
		int	_MailID1;
		int	_MailID2;
		int _GuildFestPoint;
		int _GuildPoint;
	};

	TempFieldNum sFieldNum;	
	sFieldNum._ResultType = pTable->GetFieldNum("_ResultType");
	sFieldNum._ClassID = pTable->GetFieldNum("_ClassID");
	sFieldNum._MailID1 = pTable->GetFieldNum("_MailID1");
	sFieldNum._MailID2 = pTable->GetFieldNum("_MailID2");
	sFieldNum._GuildFestPoint = pTable->GetFieldNum("_GuildFestPoint");
	sFieldNum._GuildPoint = pTable->GetFieldNum("_GuildPoint");

	//##################################################################
	// Load
	//##################################################################

	for( int i=0; i<pTable->GetItemCount(); i++ ) 
	{
		int nItemID = pTable->GetItemID(i);
		int iIdx = pTable->GetIDXprimary( nItemID );

		TGuildWarRewardData* Data = new TGuildWarRewardData;
		memset( Data, 0, sizeof(TGuildWarRewardData) );
		Data->cType = (char)pTable->GetFieldFromLablePtr( iIdx, sFieldNum._ResultType )->GetInteger();
		Data->cClass = (char)pTable->GetFieldFromLablePtr( iIdx, sFieldNum._ClassID )->GetInteger();
		Data->nMailID = pTable->GetFieldFromLablePtr( iIdx, sFieldNum._MailID1 )->GetInteger();
		Data->nPresentID = pTable->GetFieldFromLablePtr( iIdx, sFieldNum._MailID2 )->GetInteger();		
		Data->nGuildFestivalPoint = pTable->GetFieldFromLablePtr( iIdx, sFieldNum._GuildFestPoint)->GetInteger();		
		Data->nGuildPoint = pTable->GetFieldFromLablePtr( iIdx, sFieldNum._GuildPoint)->GetInteger();		

		m_pGuildWarRewardData.push_back(Data);
	}
	return true;
}

TGuildWarRewardData* CDNGameDataManager::GetGuildWarRewardData(char cType, char cClass)
{
	std::vector<TGuildWarRewardData*>::iterator iter;
	for( iter=m_pGuildWarRewardData.begin(); iter!=m_pGuildWarRewardData.end(); ++iter)
	{
		TGuildWarRewardData* pRewardData = (*iter);
		if( pRewardData->cType == cType && pRewardData->cClass == cClass )
			return pRewardData;
	}
	return NULL;
}

// CollectionBook
bool CDNGameDataManager::LoadCollectionBookData()
{
#ifdef _WORK
	DNTableFileFormat *pTable;
	if (m_bAllLoaded)
	{
		pTable = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TCOLLECTIONBOOK );
	}
	else
	{
		pTable = GetDNTable( CDnTableDB::TCOLLECTIONBOOK );
	}
#else		//#ifdef _WORK
	DNTableFileFormat *pTable = GetDNTable( CDnTableDB::TCOLLECTIONBOOK );
#endif		//#ifdef _WORK
	if( !pTable ) {
		g_Log.Log(LogType::_FILELOG, L"collectionbooktable.dnt failed\r\n");
		return false;
	}

	if( pTable->GetItemCount() <= 0 ) {
		g_Log.Log(LogType::_FILELOG, L"collectionbooktable.dnt Count(%d)\r\n", pTable->GetItemCount());
		//return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		m_MapCollectionBookData.clear();
	}
#endif		//#ifdef _WORK

	char szTemp[MAX_PATH];

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{
		std::vector<int> _Appellation;
		int	_RewardAppellation;
#if defined(PRE_MOD_APPELLATIONBOOK_RENEWAL)
		int _MailID[2];
#endif	// #if defined(PRE_MOD_APPELLATIONBOOK_RENEWAL)
	};

	TempFieldNum sFieldNum;
	for (int k = 0; k < CollectionAppellationMax; k++)
	{
		sprintf_s(szTemp, "_Appellation%d", k + 1);
		sFieldNum._Appellation.push_back(pTable->GetFieldNum(szTemp));
	}
	sFieldNum._RewardAppellation = pTable->GetFieldNum("_Reward");
#if defined(PRE_MOD_APPELLATIONBOOK_RENEWAL)
	sFieldNum._MailID[0] = pTable->GetFieldNum("_Mail1");
	sFieldNum._MailID[1] = pTable->GetFieldNum("_Mail2");
#endif	// #if defined(PRE_MOD_APPELLATIONBOOK_RENEWAL)
	//##################################################################
	// Load
	//##################################################################

	for( int i=0; i<pTable->GetItemCount(); i++ ) 
	{
		int nItemID = pTable->GetItemID(i);
		int iIdx = pTable->GetIDXprimary( nItemID );

		TCollectionBookData Data;
		memset( &Data, 0, sizeof(TCollectionBookData) );
#if defined(PRE_MOD_APPELLATIONBOOK_RENEWAL)
		int nRewardAppellationID = pTable->GetFieldFromLablePtr( iIdx, sFieldNum._RewardAppellation )->GetInteger();
		Data.nRewardAppellationArrayIndex = ( nRewardAppellationID < 1 ) ? -1 : GetAppellationArrayIndex( nRewardAppellationID );
#endif	// #if defined(PRE_MOD_APPELLATIONBOOK_RENEWAL)
		Data.nID = nItemID;
		for( int k=0; k<CollectionAppellationMax; k++ ) 
		{
			int nAppellationID = pTable->GetFieldFromLablePtr( iIdx, sFieldNum._Appellation[k] )->GetInteger();
			if (nAppellationID > 0)
			{
#if defined(PRE_MOD_APPELLATIONBOOK_RENEWAL)
				TMapAppellationData::iterator iter = m_MapAppellationData.find(nAppellationID);
				if (iter == m_MapAppellationData.end())
				{
					//에러. 콜렉션 칭호를 얻기 위한 조건에 있지만 실제 데이터에 없는 경우임
					g_Log.Log(LogType::_FILELOG, L"Error! LoadCollectionBookData() - CollectionID : %d, NotData AppellationID : %d\n", Data.nID, nAppellationID);
					continue;
				}
				//컬렉션북 칭호를 획득하기 위한 조건이 되는 칭호에게 정보 저장
				iter->second.nUseCollectionBookIndex.push_back(Data.nID);
				Data.nVecAppellationArrayIndex.push_back(GetAppellationArrayIndex(nAppellationID));
			}
		}
		Data.nMailID[0] = pTable->GetFieldFromLablePtr( iIdx, sFieldNum._MailID[0])->GetInteger();
		Data.nMailID[1] = pTable->GetFieldFromLablePtr( iIdx, sFieldNum._MailID[1])->GetInteger();
#else
				Data.nVecAppellationArrayIndex.push_back(GetAppellationArrayIndex(nAppellationID));
			}
		}

		int nRewardAppellationID = pTable->GetFieldFromLablePtr( iIdx, sFieldNum._RewardAppellation )->GetInteger();
		Data.nRewardAppellationArrayIndex = ( nRewardAppellationID < 1 ) ? -1 : GetAppellationArrayIndex( nRewardAppellationID );
#endif	// #if defined(PRE_MOD_APPELLATIONBOOK_RENEWAL)
		m_MapCollectionBookData.insert( make_pair( nItemID, Data ) );
	}

	return true;
}

TCollectionBookData *CDNGameDataManager::GetCollectionBookData(int nID)
{
	TMapCollectionBook::iterator iter = m_MapCollectionBookData.find(nID);
	if (iter == m_MapCollectionBookData.end()) return NULL;
	return &(iter->second);
}

// Tax
bool CDNGameDataManager::LoadTaxData()
{
#ifdef _WORK
	DNTableFileFormat *pSox;
	if (m_bAllLoaded)
		pSox = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TTAX );
	else
		pSox = GetDNTable( CDnTableDB::TTAX );
#else		//#ifdef _WORK
	DNTableFileFormat *pSox = GetDNTable( CDnTableDB::TTAX );
#endif		//#ifdef _WORK
	if (!pSox){
		g_Log.Log(LogType::_FILELOG, L"TaxTable.dnt failed\r\n");
		return false;
	}

	if (pSox->GetItemCount() <= 0){
		g_Log.Log(LogType::_FILELOG, L"TaxTable.dnt Count(%d)\r\n", pSox->GetItemCount());
		return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		SAFE_DELETE_PVEC( m_pTaxData );
	}
#endif		//#ifdef _WORK

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{
		int _TaxSort;
		int _LevelS;
		int _LevelE;
		int _AmountS;
		int _AmountE;
		int _Rate;
		int _TaxAmount;
	};

	TempFieldNum sFieldNum;
	sFieldNum._TaxSort	= pSox->GetFieldNum( "_TaxSort" );
	sFieldNum._LevelS	= pSox->GetFieldNum( "_LevelS" );
	sFieldNum._LevelE	= pSox->GetFieldNum( "_LevelE" );
	sFieldNum._AmountS	= pSox->GetFieldNum( "_AmountS" );
	sFieldNum._AmountE	= pSox->GetFieldNum( "_AmountE" );
	sFieldNum._Rate		= pSox->GetFieldNum( "_Rate" );
	sFieldNum._TaxAmount = pSox->GetFieldNum( "_TaxAmount" );

	//##################################################################
	// Load
	//##################################################################

	for( int i=0 ; i<pSox->GetItemCount() ; ++i )
	{
		TTaxData* pTaxData = new TTaxData;
		memset(pTaxData, 0, sizeof(TTaxData));

		int iIdx = pSox->GetIDXprimary( pSox->GetItemID(i) );

		pTaxData->cTaxType		= pSox->GetFieldFromLablePtr( iIdx, sFieldNum._TaxSort )->GetInteger();
		pTaxData->nLevelStart	= pSox->GetFieldFromLablePtr( iIdx, sFieldNum._LevelS )->GetInteger();
		pTaxData->nLevelEnd		= pSox->GetFieldFromLablePtr( iIdx, sFieldNum._LevelE )->GetInteger();
		pTaxData->nAmountStart	= pSox->GetFieldFromLablePtr( iIdx, sFieldNum._AmountS )->GetInteger();
		pTaxData->nAmountEnd	= pSox->GetFieldFromLablePtr( iIdx, sFieldNum._AmountE )->GetInteger();
		pTaxData->fRate			= pSox->GetFieldFromLablePtr( iIdx, sFieldNum._Rate )->GetFloat();
		pTaxData->nTaxAmount	= pSox->GetFieldFromLablePtr( iIdx, sFieldNum._TaxAmount )->GetInteger();

		m_pTaxData.push_back(pTaxData);
	}

	return true;
}

float CDNGameDataManager::GetTaxRate(int nTaxType, int nLevel, INT64 nPrice)
{
	for (int i = 0; i <(int)m_pTaxData.size(); i++){
		if (m_pTaxData[i]->cTaxType != nTaxType) continue;

		int nGold = (int)(nPrice / 10000);
		if ((m_pTaxData[i]->nLevelStart > nLevel) ||(m_pTaxData[i]->nLevelEnd < nLevel)) continue;
		if ((m_pTaxData[i]->nAmountStart > nGold) ||(m_pTaxData[i]->nAmountEnd < nGold)) continue;

		return m_pTaxData[i]->fRate;
	}

	return 0.f;
}

int CDNGameDataManager::GetTaxAmount(int nTaxType, int nLevel, INT64 nPrice)
{
	for (int i = 0; i <(int)m_pTaxData.size(); i++){
		if (m_pTaxData[i]->cTaxType != nTaxType) continue;

		int nGold = (int)(nPrice / 10000);
		if ((m_pTaxData[i]->nLevelStart > nLevel) ||(m_pTaxData[i]->nLevelEnd < nLevel)) continue;
#if defined(PRE_ADD_SERVER_WAREHOUSE)
		if( nPrice > 0)
#endif // #if defined(PRE_ADD_SERVER_WAREHOUSE)
		{
			if ((m_pTaxData[i]->nAmountStart > nGold) ||(m_pTaxData[i]->nAmountEnd < nGold)) continue;
		}
		return m_pTaxData[i]->nTaxAmount;
	}

	return 0;
}

INT64 CDNGameDataManager::CalcTax(int nTaxType, int nLevel, INT64 nPrice)
{
	float fTaxRate = GetTaxRate(nTaxType, nLevel, nPrice);
	return(INT64)(nPrice * fTaxRate + 0.5f);
}

bool CDNGameDataManager::LoadSealData()
{
#ifdef _WORK
	DNTableFileFormat *pSox;
	if (m_bAllLoaded)
		pSox = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TSEALCOUNT );
	else
		pSox = GetDNTable( CDnTableDB::TSEALCOUNT );
#else		//#ifdef _WORK
	DNTableFileFormat *pSox = GetDNTable( CDnTableDB::TSEALCOUNT );
#endif		//#ifdef _WORK
	if (!pSox){
		g_Log.Log(LogType::_FILELOG, L"SealCountTable.dnt failed\r\n");
		return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		SAFE_DELETE_PVEC( m_pSealCountData );
	}
#endif		//#ifdef _WORK

	char szTemp[MAX_PATH] = { 0, };

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{
		int _Type1;
		int _Type2;
		std::vector<int> _Level;
		std::vector<int> _SuccessRate;
	};

	TempFieldNum sFieldNum;
	sFieldNum._Type1	= pSox->GetFieldNum( "_Type1" );
	sFieldNum._Type2	= pSox->GetFieldNum( "_Type2" );
	sFieldNum._Level.reserve(SEALENCHANTMAX);
	for (int j = 0; j < SEALENCHANTMAX; ++j){
		sprintf_s(szTemp, "_Count%d", j);
		sFieldNum._Level.push_back(pSox->GetFieldNum(szTemp));
		sprintf_s(szTemp, "_SuccessRate%d", j);
		sFieldNum._SuccessRate.push_back(pSox->GetFieldNum(szTemp));
	}

	//##################################################################
	// Load
	//##################################################################

	for( int i=0 ; i<pSox->GetItemCount() ; ++i )
	{
		TSealCountData* pSealCountData = new TSealCountData;
		memset(pSealCountData, 0, sizeof(TSealCountData));

		int iIdx = pSox->GetIDXprimary( pSox->GetItemID(i) );

		pSealCountData->nType1 = pSox->GetFieldFromLablePtr( iIdx, sFieldNum._Type1 )->GetInteger();
		pSealCountData->nType2 = pSox->GetFieldFromLablePtr( iIdx, sFieldNum._Type2 )->GetInteger();

		for (int j = 0; j < SEALENCHANTMAX; j++){
			pSealCountData->nLevel[j] = pSox->GetFieldFromLablePtr( iIdx, sFieldNum._Level[j] )->GetInteger();
		}
		if (pSealCountData->nType1 == 2){
			for (int k = 0; k < SEALENCHANTMAX; k++)
				pSealCountData->cSuccessRate[k] = (char)(( pSox->GetFieldFromLablePtr( iIdx, sFieldNum._SuccessRate[k] )->GetFloat() + 0.0001f ) * 100.f );
		}

		m_pSealCountData.push_back(pSealCountData);
	}

	return true;
}

int CDNGameDataManager::GetTotalSealNeeds(int nTypeParam, int nType2, char cItemLevel)
{
	for (int i = 0; i <(int)m_pSealCountData.size(); i++){
		if (m_pSealCountData[i]->nType1 != nTypeParam) continue;
		if (m_pSealCountData[i]->nType2 != nType2) continue;
		if (SEALENCHANTMAX <= cItemLevel) continue;

		return m_pSealCountData[i]->nLevel[cItemLevel];
	}

	return 0;
}

char CDNGameDataManager::GetSealSuccessRate(int nTypeParam, int nType2, char cItemLevel)
{
	for (int i = 0; i <(int)m_pSealCountData.size(); i++){
		if (m_pSealCountData[i]->nType1 != nTypeParam) continue;
		if (m_pSealCountData[i]->nType2 != nType2) continue;
		if (SEALENCHANTMAX <= cItemLevel) continue;

		return m_pSealCountData[i]->cSuccessRate[cItemLevel];
	}

	return 0;
}

bool CDNGameDataManager::LoadPCBangData()
{
#ifdef _WORK
	DNTableFileFormat *pSox;
	if (m_bAllLoaded)
	{
		pSox = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TPCCAFE );
	}
	else
	{
		pSox = GetDNTable( CDnTableDB::TPCCAFE );
	}
#else		//#ifdef _WORK
	DNTableFileFormat *pSox = GetDNTable( CDnTableDB::TPCCAFE );
#endif		//#ifdef _WORK
	if (!pSox){
		g_Log.Log(LogType::_FILELOG, L"PCCafeTable.dnt failed\r\n");
		return false;
	}

	if (pSox->GetItemCount() <= 0){
		g_Log.Log(LogType::_FILELOG, L"PCCafeTable.dnt Count(%d)\r\n", pSox->GetItemCount());
		return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		SAFE_DELETE_PMAP( TMapPCBangData, m_pPCBangData );
	}
#endif		//#ifdef _WORK

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{
		int _Type;
		int _PCParam[PCParamMax];
		int _PremiumPCParam[PCParamMax];
		int _GoldPCParam[PCParamMax];
		int _SilverPCParam[PCParamMax];
		int _RedPCParam[PCParamMax];
	};

	TempFieldNum sFieldNum;
	sFieldNum._Type	= pSox->GetFieldNum( "_Type" );
	sFieldNum._PCParam[0] = pSox->GetFieldNum( "_PCParam1" );
	sFieldNum._PCParam[1] = pSox->GetFieldNum( "_PCParam2" );
	sFieldNum._PremiumPCParam[0] = pSox->GetFieldNum( "_PremiumPCParam1" );
	sFieldNum._PremiumPCParam[1] = pSox->GetFieldNum( "_PremiumPCParam2" );
	sFieldNum._GoldPCParam[0] = pSox->GetFieldNum( "_GoldPCParam1" );
	sFieldNum._GoldPCParam[1] = pSox->GetFieldNum( "_GoldPCParam2" );
	sFieldNum._SilverPCParam[0] = pSox->GetFieldNum( "_SilverPCParam1" );
	sFieldNum._SilverPCParam[1] = pSox->GetFieldNum( "_SilverPCParam2" );
	sFieldNum._RedPCParam[0] = pSox->GetFieldNum( "_RedPCParam1" );
	sFieldNum._RedPCParam[1] = pSox->GetFieldNum( "_RedPCParam2" );

	//##################################################################
	// Load
	//##################################################################

	for( int i=0 ; i<pSox->GetItemCount() ; ++i )
	{
		int iIdx = pSox->GetIDXprimary( pSox->GetItemID(i) );
		int nType = pSox->GetFieldFromLablePtr( iIdx, sFieldNum._Type )->GetInteger();

		TPCBangData *pPCBangData = GetPCBangData(nType);
		if (!pPCBangData){
			pPCBangData = new TPCBangData;
			pPCBangData->cType = nType;
			m_pPCBangData.insert(make_pair(nType, pPCBangData));
		}

		TPCBangParam Normal = {0,}, Premium = {0,}, Gold = {0,}, Silver = {0,}, Red = {0,};
		for (int j = 0; j < PCParamMax; j++){
			Normal.nParam[j] = pSox->GetFieldFromLablePtr( iIdx, sFieldNum._PCParam[j] )->GetInteger();
			Premium.nParam[j] = pSox->GetFieldFromLablePtr( iIdx, sFieldNum._PremiumPCParam[j] )->GetInteger();
			Gold.nParam[j] = pSox->GetFieldFromLablePtr( iIdx, sFieldNum._GoldPCParam[j] )->GetInteger();
			Silver.nParam[j] = pSox->GetFieldFromLablePtr( iIdx, sFieldNum._SilverPCParam[j] )->GetInteger();
			Red.nParam[j] = pSox->GetFieldFromLablePtr( iIdx, sFieldNum._RedPCParam[j] )->GetInteger();
		}
		if (Normal.nParam[0] > 0)
			pPCBangData->VecPCBangParam.push_back(Normal);
		if (Premium.nParam[0] > 0)
			pPCBangData->VecPremiumPCBangParam.push_back(Premium);
		if (Gold.nParam[0] > 0)
			pPCBangData->VecGoldPCBangParam.push_back(Gold);
		if (Silver.nParam[0] > 0)
			pPCBangData->VecSilverPCBangParam.push_back(Silver);
		if (Red.nParam[0] > 0)
			pPCBangData->VecRedPCBangParam.push_back(Red);
	}

	return true;
}

int CDNGameDataManager::GetPCBangDataCount()
{
	return (int)m_pPCBangData.size();
}

TPCBangData *CDNGameDataManager::GetPCBangData(int nPCBangType)
{
	if (m_pPCBangData.empty()) return NULL;

	TMapPCBangData::iterator iter = m_pPCBangData.find(nPCBangType);
	if (iter == m_pPCBangData.end()) return NULL;

	return iter->second;
}

int CDNGameDataManager::GetPCBangParam1(int nPCBangType, int nPCBangGrade)
{
	TPCBangData *pPCBangData = GetPCBangData(nPCBangType);
	if (!pPCBangData) return 0;

	int nValue = 0;
	switch (nPCBangGrade)
	{
	case PCBang::Grade::None:	// 피씨방이든 아니든 기본값은 넣어줘야함(일반에서 피씨방 갔을때 값 얻어와야함)
	case PCBang::Grade::Normal:
		{
			if (!pPCBangData->VecPCBangParam.empty())
				nValue = pPCBangData->VecPCBangParam[0].nParam[0];
		}
		break;

	case PCBang::Grade::Premium:
		{
			if (!pPCBangData->VecPremiumPCBangParam.empty())
				nValue = pPCBangData->VecPremiumPCBangParam[0].nParam[0];
		}
		break;

	case PCBang::Grade::Gold:
		{
			if (!pPCBangData->VecGoldPCBangParam.empty())
				nValue = pPCBangData->VecGoldPCBangParam[0].nParam[0];
		}
		break;

	case PCBang::Grade::Silver:
		{
			if (!pPCBangData->VecSilverPCBangParam.empty())
				nValue = pPCBangData->VecSilverPCBangParam[0].nParam[0];
		}
		break;

	case PCBang::Grade::Red:
		{
			if (!pPCBangData->VecRedPCBangParam.empty())
				nValue = pPCBangData->VecRedPCBangParam[0].nParam[0];
		}
		break;
	}

	return nValue;
}

int CDNGameDataManager::GetPCBangParam2(int nPCBangType, int nPCBangGrade)
{
	TPCBangData *pPCBangData = GetPCBangData(nPCBangType);
	if (!pPCBangData) return 0;

	int nValue = 0;
	switch (nPCBangGrade)
	{
	case PCBang::Grade::None:	// 피씨방이든 아니든 기본값은 넣어줘야함(일반에서 피씨방 갔을때 값 얻어와야함)
	case PCBang::Grade::Normal:
		{
			if (!pPCBangData->VecPCBangParam.empty())
				nValue = pPCBangData->VecPCBangParam[0].nParam[1];
		}
		break;

	case PCBang::Grade::Premium:
		{
			if (!pPCBangData->VecPremiumPCBangParam.empty())
				nValue = pPCBangData->VecPremiumPCBangParam[0].nParam[1];
		}
		break;

	case PCBang::Grade::Gold:
		{
			if (!pPCBangData->VecGoldPCBangParam.empty())
				nValue = pPCBangData->VecGoldPCBangParam[0].nParam[1];
		}
		break;

	case PCBang::Grade::Silver:
		{
			if (!pPCBangData->VecSilverPCBangParam.empty())
				nValue = pPCBangData->VecSilverPCBangParam[0].nParam[1];
		}
		break;

	case PCBang::Grade::Red:
		{
			if (!pPCBangData->VecRedPCBangParam.empty())
				nValue = pPCBangData->VecRedPCBangParam[0].nParam[1];
		}
		break;
	}

	return nValue;
}

int CDNGameDataManager::GetPCBangNestClearCount(int nPCBangGrade, int nMapID)
{
	TPCBangData *pPCBangData = GetPCBangData(PCBang::Type::NestClearCount);
	if (!pPCBangData) return 0;

	int nClearCount = 0;
	switch (nPCBangGrade)
	{
	case PCBang::Grade::None:
		break;

	case PCBang::Grade::Normal:
		{
			if (!pPCBangData->VecPCBangParam.empty()){
				for (int i = 0; i < (int)pPCBangData->VecPCBangParam.size(); i++){
					if (pPCBangData->VecPCBangParam[i].nParam[0] == nMapID){
						nClearCount = pPCBangData->VecPCBangParam[i].nParam[1];
						break;
					}
				}
			}
		}
		break;

	case PCBang::Grade::Premium:
		{
			if (!pPCBangData->VecPremiumPCBangParam.empty()){
				for (int i = 0; i < (int)pPCBangData->VecPremiumPCBangParam.size(); i++){
					if (pPCBangData->VecPremiumPCBangParam[i].nParam[0] == nMapID){
						nClearCount = pPCBangData->VecPremiumPCBangParam[i].nParam[1];
						break;
					}
				}
			}
		}
		break;

	case PCBang::Grade::Gold:
		{
			if (!pPCBangData->VecGoldPCBangParam.empty()){
				for (int i = 0; i < (int)pPCBangData->VecGoldPCBangParam.size(); i++){
					if (pPCBangData->VecGoldPCBangParam[i].nParam[0] == nMapID){
						nClearCount = pPCBangData->VecGoldPCBangParam[i].nParam[1];
						break;
					}
				}
			}
		}
		break;

	case PCBang::Grade::Silver:
		{
			if (!pPCBangData->VecSilverPCBangParam.empty()){
				for (int i = 0; i < (int)pPCBangData->VecSilverPCBangParam.size(); i++){
					if (pPCBangData->VecSilverPCBangParam[i].nParam[0] == nMapID){
						nClearCount = pPCBangData->VecSilverPCBangParam[i].nParam[1];
						break;
					}
				}
			}
		}
		break;

	case PCBang::Grade::Red:
		{
			if (!pPCBangData->VecRedPCBangParam.empty()){
				for (int i = 0; i < (int)pPCBangData->VecRedPCBangParam.size(); i++){
					if (pPCBangData->VecRedPCBangParam[i].nParam[0] == nMapID){
						nClearCount = pPCBangData->VecRedPCBangParam[i].nParam[1];
						break;
					}
				}
			}
		}
		break;
	}

	return nClearCount;
}

bool CDNGameDataManager::GetPCBangClearBoxFlag(int nPCBangGrade)
{
	if (nPCBangGrade == PCBang::Grade::None) return false;

	int nValue = GetPCBangParam1(PCBang::Type::ClearBox, nPCBangGrade);
	return (nValue == 1) ? true : false;
}

bool CDNGameDataManager::CheckPcbangAppellation(int nAppllationIndex)
{
	TPCBangData *pData = GetPCBangData(PCBang::Type::Appellation);
	if (!pData) return false;

	if (!pData->VecPCBangParam.empty()){
		for (int i = 0; i < (int)pData->VecPCBangParam.size(); i++){
			if (GetAppellationArrayIndex(pData->VecPCBangParam[i].nParam[0]) == nAppllationIndex)
				return true;
		}
	}
	if (!pData->VecPremiumPCBangParam.empty()){
		for (int i = 0; i < (int)pData->VecPremiumPCBangParam.size(); i++){
			if (GetAppellationArrayIndex(pData->VecPremiumPCBangParam[i].nParam[0]) == nAppllationIndex)
				return true;
		}
	}
	if (!pData->VecGoldPCBangParam.empty()){
		for (int i = 0; i < (int)pData->VecGoldPCBangParam.size(); i++){
			if (GetAppellationArrayIndex(pData->VecGoldPCBangParam[i].nParam[0]) == nAppllationIndex)
				return true;
		}
	}
	if (!pData->VecSilverPCBangParam.empty()){
		for (int i = 0; i < (int)pData->VecSilverPCBangParam.size(); i++){
			if (GetAppellationArrayIndex(pData->VecSilverPCBangParam[i].nParam[0]) == nAppllationIndex)
				return true;
		}
	}
	if (!pData->VecRedPCBangParam.empty()){
		for (int i = 0; i < (int)pData->VecRedPCBangParam.size(); i++){
			if (GetAppellationArrayIndex(pData->VecRedPCBangParam[i].nParam[0]) == nAppllationIndex)
				return true;
		}
	}

	return false;
}

// CashShop
bool CDNGameDataManager::LoadCashCommodityData(bool bReload)
{
	DNTableFileFormat *pSox = NULL;
#ifdef _WORK
	if (bReload || m_bAllLoaded)
#else		//#ifdef _WORK
	if (bReload)
#endif		//#ifdef _WORK
		pSox = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TCASHCOMMODITY );
	else
		pSox = GetDNTable( CDnTableDB::TCASHCOMMODITY );

	if( !pSox )
	{
		g_Log.Log(LogType::_FILELOG, L"CashCommodity.dnt failed\r\n");
		return false;
	}

	if( pSox->GetItemCount() <= 0 )
	{
		g_Log.Log(LogType::_FILELOG, L"CashCommodity.dnt Count(%d)\r\n", pSox->GetItemCount());
		return false;
	}

	//백업용 컨테이너 생성
	TMapCashCommodityData pBackupCashCommodityData;
#ifdef _WORK
	if (bReload || m_bAllLoaded)
#else		//#ifdef _WORK
	if (bReload)
#endif		//#ifdef _WORK
	{
		//리로드라면 컨테이너 백업해두고 컨테이너를 비운다.
		ScopeLock <CSyncLock> Lock(m_Sync);

		pBackupCashCommodityData = m_pCashCommodityData;
		m_pCashCommodityData.clear();
	}

	char szTemp[MAX_PATH] = { 0, };

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{
		int _SN;
#if defined(_JP)
		int _SNJPN;
#endif	// #if defined(_JP)
		int _Category;
		int _SubCategory;
		std::vector<int> _ItemID;
		std::vector<int> _LinkSN;
		int _Period;
		int _Price;
		int _Count;
		int _Priority;
		int _OnSale;
		int _State;
		int _Limit;
		int _ReserveGive;
		int _Reserve;
		int _validity;
		int _ReserveAble;
		int _PresentAble;
		int _ItemSort;
		int _PriceFix;
		int _CartAble;
#if defined(PRE_ADD_VIP)
		int _VIPSell;
		int _VIPLevel;
		int _Pay;
		int _PaySale;
		int _VIPPoint;
#endif	// #if defined(PRE_ADD_VIP)
		int _OverlapBuy;
#if defined(PRE_ADD_CASH_REFUND)
		int _NoRefund;
#endif
#if defined(PRE_ADD_CASHSHOP_CREDIT)
		int _CreditAble;
		int _CreditAbleLV;
#endif	// #if defined(PRE_ADD_CASHSHOP_CREDIT)
#if defined(PRE_ADD_SALE_COUPON)
		int _UseCoupon;
#endif
#if defined( PRE_ADD_NEW_MONEY_SEED )
		int _SeedAble;
		int _Seed;
		int _SeedGive;
#endif
	};

	TempFieldNum sFieldNum;
	sFieldNum._SN = pSox->GetFieldNum("_SN");
#if defined(_JP)
	sFieldNum._SNJPN = pSox->GetFieldNum("_SNJPN");
#endif	// #if defined(_JP)
	sFieldNum._Category = pSox->GetFieldNum("_Category");
	sFieldNum._SubCategory = pSox->GetFieldNum("_SubCategory");
	sFieldNum._ItemID.reserve(COMMODITYITEMMAX);
	for (int j = 0; j < COMMODITYITEMMAX; ++j){
		sprintf_s(szTemp, "_ItemID%02d", j + 1);
		sFieldNum._ItemID.push_back(pSox->GetFieldNum(szTemp));
	}
	sFieldNum._LinkSN.reserve(COMMODITYLINKMAX);
	for (int j = 0; j < COMMODITYLINKMAX; ++j){
		sprintf_s(szTemp, "_LinkSN%02d", j + 1);
		sFieldNum._LinkSN.push_back(pSox->GetFieldNum(szTemp));
	}
	sFieldNum._Period = pSox->GetFieldNum("_Period");
	sFieldNum._Price = pSox->GetFieldNum("_Price");
	sFieldNum._Count = pSox->GetFieldNum("_Count");
	sFieldNum._Priority = pSox->GetFieldNum("_Priority");
	sFieldNum._OnSale = pSox->GetFieldNum("_OnSale");
	sFieldNum._State = pSox->GetFieldNum("_State");
	sFieldNum._Limit = pSox->GetFieldNum("_Limit");
	sFieldNum._ReserveGive = pSox->GetFieldNum("_ReserveGive");
	sFieldNum._Reserve = pSox->GetFieldNum("_Reserve");
	sFieldNum._validity = pSox->GetFieldNum("_validity");
	sFieldNum._ReserveAble = pSox->GetFieldNum("_ReserveAble");
	sFieldNum._PresentAble = pSox->GetFieldNum("_PresentAble");
	sFieldNum._ItemSort = pSox->GetFieldNum("_ItemSort");
	sFieldNum._PriceFix = pSox->GetFieldNum("_PriceFix");
	sFieldNum._CartAble = pSox->GetFieldNum("_CartAble");
#if defined(PRE_ADD_VIP)
	sFieldNum._VIPSell = pSox->GetFieldNum("_VIPSell");
	sFieldNum._VIPLevel = pSox->GetFieldNum("_VIPLevel");
	sFieldNum._Pay = pSox->GetFieldNum("_Pay");
	sFieldNum._PaySale = pSox->GetFieldNum("_PaySale");
	sFieldNum._VIPPoint = pSox->GetFieldNum("_VIPPoint");
#endif	// #if defined(PRE_ADD_VIP)
	sFieldNum._OverlapBuy = pSox->GetFieldNum("_OverlapBuy");
#if defined(PRE_ADD_CASH_REFUND)
	sFieldNum._NoRefund = pSox->GetFieldNum("_NoRefund");
#endif
#if defined(PRE_ADD_CASHSHOP_CREDIT)
	sFieldNum._CreditAble = pSox->GetFieldNum("_CreditAble");
	sFieldNum._CreditAbleLV = pSox->GetFieldNum("_CreditAbleLV");
#endif	// #if defined(PRE_ADD_CASHSHOP_CREDIT)
#if defined(PRE_ADD_SALE_COUPON)
	sFieldNum._UseCoupon = pSox->GetFieldNum("_UseCoupon");
#endif
#if defined( PRE_ADD_NEW_MONEY_SEED )
	sFieldNum._SeedAble = pSox->GetFieldNum("_SeedAble");
	sFieldNum._Seed = pSox->GetFieldNum("_Seed");
	sFieldNum._SeedGive = pSox->GetFieldNum("_SeedGive");
#endif

	//##################################################################
	// Load
	//##################################################################

	char *pStr = NULL, Dest[256] = {0,};
	int nJob	= 0;

	for (int i = 0; i < pSox->GetItemCount(); i++){
		TCashCommodityData *pCashData = new TCashCommodityData;
		//2010.10.7 haling STL 보호 위해 생성자로 변환
		//memset(pCashData, 0, sizeof(TCashCommodityData));

		int nID = pSox->GetItemID(i);
		int nIdx = pSox->GetIDXprimary(nID);

		pCashData->nSN = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._SN)->GetInteger();
#if defined(_JP)
		pCashData->strJPSN = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._SNJPN)->GetString();
#endif	// #if defined(_JP)
		pCashData->cCategory = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._Category)->GetInteger();
		pCashData->nSubCategory = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._SubCategory)->GetInteger();

		for (int j = 0; j < COMMODITYITEMMAX; j++){
			pCashData->nItemID[j] = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._ItemID[j])->GetInteger();
		}

		for (int j = 0; j < COMMODITYLINKMAX; j++){
			pCashData->nLinkSN[j] = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._LinkSN[j])->GetInteger();
		}

		pCashData->wPeriod = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._Period)->GetInteger();
		pCashData->nPrice = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._Price)->GetInteger();
		pCashData->nCount = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._Count)->GetInteger();
		pCashData->nPriority = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._Priority)->GetInteger();
		pCashData->bOnSale = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._OnSale)->GetInteger() ? true : false;
		pCashData->cState = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._State)->GetInteger();
		pCashData->bLimit = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._Limit)->GetInteger() ? true : false;
		pCashData->bReserveGive = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._ReserveGive)->GetInteger() ? true : false;
		pCashData->nReserve = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._Reserve)->GetInteger();
		pCashData->nValidity = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._validity)->GetInteger();
		pCashData->bReserveAble = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._ReserveAble)->GetInteger() ? true : false;
		pCashData->bPresentAble = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._PresentAble)->GetInteger() ? true : false;
		pCashData->cItemSort = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._ItemSort)->GetInteger();
		pCashData->nPriceFix = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._PriceFix)->GetInteger();
		pCashData->bCartAble = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._CartAble)->GetInteger() ? true : false;

#if defined(PRE_ADD_VIP)
		pCashData->bVIPSell = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._VIPSell)->GetInteger() ? true : false;
		pCashData->nVIPLevel = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._VIPLevel)->GetInteger();
		pCashData->bAutomaticPay = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._Pay)->GetInteger() ? true : false;
		pCashData->nAutomaticPaySale = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._PaySale)->GetInteger();
		pCashData->nVIPPoint = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._VIPPoint)->GetInteger();
#endif	// #if defined(PRE_ADD_VIP)
		pCashData->nOverlapBuy = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._OverlapBuy)->GetInteger();
#if defined(PRE_ADD_CASH_REFUND)
		pCashData->bNoRefund = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._NoRefund)->GetInteger() ? true : false;
#endif
#if defined(PRE_ADD_CASHSHOP_CREDIT)
		pCashData->bCreditAble = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._CreditAble)->GetInteger() ? true : false;
		pCashData->nCreditAbleLevel = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._CreditAbleLV)->GetInteger();
#endif	// #if defined(PRE_ADD_CASHSHOP_CREDIT)
#if defined(PRE_ADD_SALE_COUPON)
		pCashData->bUseCoupon = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._UseCoupon)->GetInteger() ? true : false;
#endif //#if defined(PRE_ADD_SALE_COUPON)
#if defined( PRE_ADD_NEW_MONEY_SEED )
		pCashData->bSeedAble = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._SeedAble)->GetInteger() ? true : false;
		pCashData->bSeedGive = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._SeedGive)->GetInteger() ? true : false;
		if( pCashData->bSeedGive )
			pCashData->nSeed = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._Seed)->GetInteger();
#endif

		ScopeLock <CSyncLock> Lock(m_Sync);
		std::pair<TMapCashCommodityData::iterator, bool> Ret = m_pCashCommodityData.insert(make_pair(pCashData->nSN, pCashData));
		if (Ret.second == false) delete pCashData;
	}

#ifdef _WORK
	if (bReload || m_bAllLoaded)
#else		//#ifdef _WORK
	if (bReload)
#endif		//#ifdef _WORK
	{
		//기존의 데이터를 지운다.
		ScopeLock <CSyncLock> Lock(m_Sync);
		SAFE_DELETE_PMAP(TMapCashCommodityData, pBackupCashCommodityData);
	}

	return true;
}

bool CDNGameDataManager::GetCashCommodityData(int nSN, TCashCommodityData &Data)
{
	ScopeLock <CSyncLock> Lock(m_Sync);

	TMapCashCommodityData::iterator iter = m_pCashCommodityData.find(nSN);	
	if (m_pCashCommodityData.end() != iter) 
	{
		Data = *(*iter).second;
		return true;
	}
	return false;
}

int CDNGameDataManager::GetCashCommodityPrice(int nSN)
{
	TCashCommodityData CashData;
	if (!GetCashCommodityData(nSN, CashData)) return 0;

	return CashData.nPrice;
}

bool CDNGameDataManager::IsReserveCommodity(int nSN)
{
	TCashCommodityData CashData;
	if (!GetCashCommodityData(nSN, CashData)) return false;

	return CashData.bReserveAble;
}

bool CDNGameDataManager::IsPresentCommodity(int nSN)
{
	TCashCommodityData CashData;
	if (!GetCashCommodityData(nSN, CashData)) return false;

	return CashData.bPresentAble;
}

bool CDNGameDataManager::IsLimitCommodity(int nSN)
{
	TCashCommodityData CashData;
	if (!GetCashCommodityData(nSN, CashData)) return false;

	return CashData.bLimit;
}

int CDNGameDataManager::GetCashCommodityPeriod(int nSN)
{
	TCashCommodityData CashData;
	if (!GetCashCommodityData(nSN, CashData)) return 0;

	return CashData.wPeriod;
}

int CDNGameDataManager::GetCashCommodityCount(int nSN)
{
	TCashCommodityData CashData;
	if (!GetCashCommodityData(nSN, CashData)) return 0;

	return CashData.nCount;
}

bool CDNGameDataManager::IsValidCashCommodityItemID(int nItemSN, int nItemID)
{
	ScopeLock <CSyncLock> Lock(m_Sync);

	TMapCashCommodityData::iterator iter = m_pCashCommodityData.find(nItemSN);
	if (m_pCashCommodityData.end() != iter)
	{
		for (int i = 0; i < COMMODITYITEMMAX; i++)
		{
			if ((*iter).second->nItemID[i] == nItemID)
				return true;
		}
	}
	return false;
}

#if defined(PRE_ADD_VIP)

int CDNGameDataManager::GetCashCommodityVIPPoint(int nSN)
{
	ScopeLock <CSyncLock> Lock(m_Sync);

	TMapCashCommodityData::iterator iter = m_pCashCommodityData.find(nSN);
	if (m_pCashCommodityData.end() != iter)
		return(*iter).second->nVIPPoint;
	return 0;
}

bool CDNGameDataManager::GetCashCommodityPay(int nSN)
{
	ScopeLock <CSyncLock> Lock(m_Sync);

	TMapCashCommodityData::iterator iter = m_pCashCommodityData.find(nSN);
	if (m_pCashCommodityData.end() != iter)
		return(*iter).second->bAutomaticPay;
	return false;
}

int CDNGameDataManager::GetCashCommodityVIPAutomaticPaySalePrice(int nSN)
{
	ScopeLock <CSyncLock> Lock(m_Sync);

	TMapCashCommodityData::iterator iter = m_pCashCommodityData.find(nSN);
	if (m_pCashCommodityData.end() != iter)
		return(*iter).second->nPrice -(int)((*iter).second->nPrice *(*iter).second->nAutomaticPaySale / 100);
	return 0;
}

bool CDNGameDataManager::IsVIPSell(int nSN)
{
	ScopeLock <CSyncLock> Lock(m_Sync);

	TMapCashCommodityData::iterator iter = m_pCashCommodityData.find(nSN);
	if (m_pCashCommodityData.end() != iter)
		return(*iter).second->bVIPSell;
	return false;
}

int CDNGameDataManager::GetCashCommodityVIPLevel(int nSN)
{
	ScopeLock <CSyncLock> Lock(m_Sync);

	TMapCashCommodityData::iterator iter = m_pCashCommodityData.find(nSN);
	if (m_pCashCommodityData.end() != iter)
		return(*iter).second->nVIPLevel;
	return 0;
}

#endif	// #if defined(PRE_ADD_VIP)

int CDNGameDataManager::GetCashCommodityItem0(int nSN)
{
	TCashCommodityData CashData;
	if (!GetCashCommodityData(nSN, CashData)) return 0;

	return CashData.nItemID[0];
}

int CDNGameDataManager::GetCashCommodityOverlapBuy(int nSN)
{
	TCashCommodityData CashData;
	if (!GetCashCommodityData(nSN, CashData)) return 0;

	return CashData.nOverlapBuy;
}

int CDNGameDataManager::GetCashCommodityItemIDCount(int nSN)
{
	ScopeLock <CSyncLock> Lock(m_Sync);

	TMapCashCommodityData::iterator iter = m_pCashCommodityData.find(nSN);
	if (m_pCashCommodityData.end() != iter)
	{
		int nRet = 0;
		for( int i=0; i<COMMODITYITEMMAX; i++)
		{
			if ((*iter).second->nItemID[i] > 0)
				nRet++;
		}
		return nRet;
	}
	return 0;
}

bool CDNGameDataManager::CheckCashSNItemID(int nItemSN, int nItemID)
{
	ScopeLock <CSyncLock> Lock(m_Sync);

	TMapCashCommodityData::iterator iter = m_pCashCommodityData.find(nItemSN);
	if (m_pCashCommodityData.end() != iter)
	{		
		for( int i=0; i<COMMODITYITEMMAX; i++)
		{
			if ((*iter).second->nItemID[i] == nItemID )
				return true;
		}		
	}
	return false;
}

#if defined(PRE_ADD_CASH_REFUND)
bool CDNGameDataManager::GetCashCommodityNoRefund(int nID, int nSN)
{
	ScopeLock <CSyncLock> Lock(m_Sync);

	TCashCommodityData * pCashData = NULL;
	TMapCashCommodityData::iterator iter = m_pCashCommodityData.find(nSN);
	if (m_pCashCommodityData.end() != iter)
		pCashData = (*iter).second;
	
	bool bResult; 
	if (!pCashData)
	{
		bResult = true;		// 불가능..
		return bResult;
	}

	bResult = pCashData->bNoRefund;
	if( !bResult ) // 환불 가능일때 타입으로 한번 더 확인하자..기획팀 실수 방지..
	{
		switch(GetItemMainType(nID))
		{
		case ITEMTYPE_REBIRTH_COIN :
		case ITEMTYPE_INVENTORY_SLOT :
		case ITEMTYPE_WAREHOUSE_SLOT :
		case ITEMTYPE_GESTURE :
		case ITEMTYPE_GUILDWARE_SLOT :
		case ITEMTYPE_FARM_VIP:
		case ITEMTYPE_GLYPH_SLOT:
		case ITEMTYPE_PERIOD_PLATE:
			bResult = true;
			break;
		}
	}	
	return bResult;
}
#endif

#if defined(PRE_ADD_CASHSHOP_CREDIT)
bool CDNGameDataManager::GetCashCommodityCreditAble(int nSN)
{
	TCashCommodityData CashData;
	if (!GetCashCommodityData(nSN, CashData)) return false;

	return CashData.bCreditAble;
}

int CDNGameDataManager::GetCashCommodityCreditAbleLevel(int nSN)
{
	TCashCommodityData CashData;
	if (!GetCashCommodityData(nSN, CashData)) return false;

	return CashData.nCreditAbleLevel;
}
#endif	// #if defined(PRE_ADD_CASHSHOP_CREDIT)
#if defined(PRE_ADD_SALE_COUPON)
bool CDNGameDataManager::IsSaleCouponByItem(int nSN)
{
	TCashCommodityData CashData;
	if (!GetCashCommodityData(nSN, CashData)) return false;

	return CashData.bUseCoupon;
}
#endif

#if defined(PRE_ADD_NEW_MONEY_SEED)
bool CDNGameDataManager::IsSeedCommodity(int nSN)
{
	TCashCommodityData CashData;
	if (!GetCashCommodityData(nSN, CashData)) return false;

	return CashData.bSeedAble;
}
#endif

bool CDNGameDataManager::LoadCashPackageData(bool bReload)
{
	DNTableFileFormat *pSox = NULL;
#ifdef _WORK
	if (bReload || m_bAllLoaded)
#else		//#ifdef _WORK
	if (bReload)
#endif		//#ifdef _WORK
		pSox = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TCASHPACKAGE );
	else
		pSox = GetDNTable( CDnTableDB::TCASHPACKAGE );

	if( !pSox )
	{
		g_Log.Log(LogType::_FILELOG, L"CashPackage.dnt failed\r\n");
		return false;
	}

	if( pSox->GetItemCount() <= 0 )
	{
		g_Log.Log(LogType::_FILELOG, L"CashPackage.dnt Count(%d)\r\n", pSox->GetItemCount());
		return false;
	}

	//백업용 컨테이너 생성
	TMapCashPackageData pBackupCashPackageData;
#ifdef _WORK
	if (bReload || m_bAllLoaded)
#else		//#ifdef _WORK
	if (bReload)
#endif		//#ifdef _WORK
	{
		//리로드라면 컨테이너 백업해두고 컨테이너를 비운다.
		ScopeLock <CSyncLock> Lock(m_Sync);

		pBackupCashPackageData = m_pCashPackageData;
		m_pCashPackageData.clear();
	}
	
	char szTemp[MAX_PATH] = { 0, };

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{
		int _SN;
		std::vector<int> _CommodityID;
	};

	TempFieldNum sFieldNum;
	sFieldNum._SN = pSox->GetFieldNum("_SN");
	sFieldNum._CommodityID.reserve(PACKAGEITEMMAX);
	for (int j = 0; j < PACKAGEITEMMAX; ++j){
		sprintf_s(szTemp, "_CommodityID%02d", j);
		sFieldNum._CommodityID.push_back(pSox->GetFieldNum(szTemp));
	}

	//##################################################################
	// Load
	//##################################################################

	for (int i = 0; i < pSox->GetItemCount(); i++){
		TCashPackageData *pCashData = new TCashPackageData;
		//2010.10.7 haling STL 보호 위해 생성자로 변환
		//memset(pCashData, 0, sizeof(TCashPackageData));

		int nID = pSox->GetItemID(i);
		int nIdx = pSox->GetIDXprimary(nID);

		pCashData->nSN = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._SN)->GetInteger();

		for (int j = 0; j < PACKAGEITEMMAX; j++){
			int nItemSN = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._CommodityID[j])->GetInteger();
			if (nItemSN > 0)
				pCashData->nVecCommoditySN.push_back(nItemSN);
		}

		ScopeLock <CSyncLock> Lock(m_Sync);
		std::pair<TMapCashPackageData::iterator,bool> Ret = m_pCashPackageData.insert(make_pair(pCashData->nSN, pCashData));
		if (Ret.second == false) delete pCashData;
	}

#ifdef _WORK
	if (bReload || m_bAllLoaded)
#else		//#ifdef _WORK
	if (bReload)
#endif		//#ifdef _WORK
	{
		//기존의 데이터를 지운다.
		ScopeLock <CSyncLock> Lock(m_Sync);
		SAFE_DELETE_PMAP(TMapCashPackageData, pBackupCashPackageData);
	}

	return true;
}

bool CDNGameDataManager::GetCashPackageData(int nSN, TCashPackageData &Data)
{
	ScopeLock <CSyncLock> Lock(m_Sync);

	TMapCashPackageData::iterator iter = m_pCashPackageData.find(nSN);
	if (iter != m_pCashPackageData.end())
	{
		Data = *(*iter).second;
		return true;
	}
	return false;
}

bool CDNGameDataManager::IsCashPackageData(int nSN)
{
	ScopeLock <CSyncLock> Lock(m_Sync);

	TMapCashPackageData::iterator iter = m_pCashPackageData.find(nSN);
	if (iter != m_pCashPackageData.end()) return true;

	return false;
}

bool CDNGameDataManager::GetCashCommodityItemSNListByPackage(int nPackageSN, std::vector<DBPacket::TItemSNIDOption> &VecItemSNIDList)
{
	TCashPackageData Package;
	if (!GetCashPackageData(nPackageSN, Package)) return false;
	if (Package.nVecCommoditySN.empty()) return false;
	if (VecItemSNIDList.empty()) return false;
	if (Package.nVecCommoditySN.size() != VecItemSNIDList.size()) return false;

	ScopeLock <CSyncLock> Lock(m_Sync);

	TCashCommodityData CashData;
	for (int i = 0; i < (int)Package.nVecCommoditySN.size(); i++){
		if (VecItemSNIDList[i].nItemSN > 0) continue;

		memset(&CashData, 0, sizeof(TCashCommodityData));
		bool bRet = GetCashCommodityData(Package.nVecCommoditySN[i], CashData);
		if (!bRet) continue;

		for (int j = 0; j < COMMODITYITEMMAX; j++){
			if (CashData.nItemID[j] == VecItemSNIDList[i].nItemID){
				VecItemSNIDList[i].nItemSN = CashData.nSN;
				break;
			}
		}
	}

	return true;
}

int CDNGameDataManager::GetCashPackageCount(int nPackageSN)
{
	ScopeLock <CSyncLock> Lock(m_Sync);

	TMapCashPackageData::iterator iter = m_pCashPackageData.find(nPackageSN);
	if (iter != m_pCashPackageData.end())
		(int)(*iter).second->nVecCommoditySN.size();
	return 0;
}

#if defined(PRE_ADD_VIP)
// 중국 VIP
bool CDNGameDataManager::LoadVIPData()
{
#ifdef _WORK
	DNTableFileFormat *pSox;
	if (m_bAllLoaded)
		pSox = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TVIP );
	else
		pSox = GetDNTable( CDnTableDB::TVIP );
#else		//#ifdef _WORK
	DNTableFileFormat *pSox = GetDNTable( CDnTableDB::TVIP );
#endif		//#ifdef _WORK
	if( !pSox ) {
		g_Log.Log(LogType::_FILELOG, L"VIPTable.dnt failed\r\n");
		return false;
	}

	if( pSox->GetItemCount() <= 0 ) {
		g_Log.Log(LogType::_FILELOG, L"VIPTable.dnt Count(%d)\r\n", pSox->GetItemCount());
		return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		SAFE_DELETE_PVEC(m_pVecVIPData);
	}
#endif		//#ifdef _WORK

	char szTemp[MAX_PATH] = { 0, };

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{
		int _VipID;			// VIPTable에서의 ID
		int _MonthItem;		// 캐시 아이템 상품 넘버(30일 결제시 한번 주기)
		std::vector<int> _MailID;			// 보낼 메일들
		int _AutoPay;
	};

	TempFieldNum sFieldNum;
	sFieldNum._VipID = pSox->GetFieldNum("_VipID");
	sFieldNum._MonthItem = pSox->GetFieldNum("_MonthItem");
	sFieldNum._MailID.reserve(4);
	for (int j = 0; j < 4; ++j){
		sprintf_s(szTemp, "_MailID%d", j + 1);
		sFieldNum._MailID.push_back(pSox->GetFieldNum(szTemp));
	}
	sFieldNum._AutoPay = pSox->GetFieldNum("_AutoPay");

	//##################################################################
	// Load
	//##################################################################

	for (int i = 0; i < pSox->GetItemCount(); i++){
		TVIPData *pVIPData = new TVIPData;
		memset(pVIPData, 0, sizeof(TVIPData));

		int nID = pSox->GetItemID(i);
		int nIdx = pSox->GetIDXprimary(nID);

		pVIPData->nVipID = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._VipID)->GetInteger();
		pVIPData->nMonthItemSN = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._MonthItem)->GetInteger();

		for (int j = 0; j < 4; j++){
			pVIPData->nMailID[j] = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._MailID[j])->GetInteger();
		}

		pVIPData->nAutoPayItemSN = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._AutoPay)->GetInteger();

		m_pVecVIPData.push_back(pVIPData);
	}

	return true;
}

TVIPData *CDNGameDataManager::GetVIPData()
{
	if (m_pVecVIPData.empty()) return NULL;
	return m_pVecVIPData[0];
}

int CDNGameDataManager::GetVIPAutoPayItemSN()
{
	if (m_pVecVIPData.empty()) return 0;
	return m_pVecVIPData[0]->nAutoPayItemSN;
}

int CDNGameDataManager::GetVIP7DaysLeftMailID()
{
	if (m_pVecVIPData.empty()) return 0;
	return m_pVecVIPData[0]->nMailID[0];
}

int CDNGameDataManager::GetVIP1DayLeftMailID()
{
	if (m_pVecVIPData.empty()) return 0;
	return m_pVecVIPData[0]->nMailID[1];
}

int CDNGameDataManager::GetVIPCloseMailID()
{
	if (m_pVecVIPData.empty()) return 0;
	return m_pVecVIPData[0]->nMailID[2];
}

int CDNGameDataManager::GetVIPShortCashMailID()
{
	if (m_pVecVIPData.empty()) return 0;
	return m_pVecVIPData[0]->nMailID[3];
}

#endif	// #if defined(PRE_ADD_VIP)

bool CDNGameDataManager::LoadDarkLairMapData()
{
#ifdef _WORK
	DNTableFileFormat *pSox;
	if (m_bAllLoaded)
		pSox = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TDLMAP );
	else
		pSox = GetDNTable( CDnTableDB::TDLMAP );
#else		//#ifdef _WORK
	DNTableFileFormat *pSox = GetDNTable( CDnTableDB::TDLMAP );
#endif		//#ifdef _WORK
	if( !pSox ) {
		g_Log.Log(LogType::_FILELOG, L"DLMapTable.dnt failed\r\n");
		return false;
	}
	if(pSox->GetItemCount() <= 0){
		g_Log.Log(LogType::_FILELOG, L"DLMapTable.dnt Count(%d)\r\n", pSox->GetItemCount());
		return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		SAFE_DELETE_PMAP( TMapDLMapData, m_pDLMapTable );
	}
#endif		//#ifdef _WORK

	struct TempFieldNum
	{
		int _TotalRound;
		int _FloorCount;

#if defined( PRE_ADD_CHALLENGE_DARKLAIR )
		int _ChallengeType;
#endif	// #if defined( PRE_ADD_CHALLENGE_DARKLAIR )
	};

	TempFieldNum sFieldNum;
	sFieldNum._TotalRound	= pSox->GetFieldNum( "_StageCount" );
	sFieldNum._FloorCount	= pSox->GetFieldNum( "_FloorCount" );

#if defined( PRE_ADD_CHALLENGE_DARKLAIR )
	sFieldNum._ChallengeType = pSox->GetFieldNum( "_ChallengeType" );
#endif	// #if defined( PRE_ADD_CHALLENGE_DARKLAIR )

	// Load
	for( int i=0 ; i<pSox->GetItemCount() ; ++i )
	{
		TDLMapData *pMapData = new TDLMapData;
		memset( pMapData, 0, sizeof(TDLMapData) );

		int nItemID = pSox->GetItemID(i);
		int iIdx = pSox->GetIDXprimary( nItemID );

		pMapData->nTotalRound = pSox->GetFieldFromLablePtr( iIdx, sFieldNum._TotalRound )->GetInteger();
		pMapData->nFloor = pSox->GetFieldFromLablePtr( iIdx, sFieldNum._FloorCount )->GetInteger();

#if defined( PRE_ADD_CHALLENGE_DARKLAIR )
		pMapData->bChallengeDarkLair = pSox->GetFieldFromLablePtr( iIdx, sFieldNum._ChallengeType )->GetInteger() == 0 ? false : true;
#endif	// #if defined( PRE_ADD_CHALLENGE_DARKLAIR )

		m_pDLMapTable.insert( make_pair( nItemID, pMapData ) );
	}

	return true;
}

const TDLMapData *CDNGameDataManager::GetDLMapData( int nItemID )
{
	TMapDLMapData::iterator it = m_pDLMapTable.find( nItemID );
	if( it == m_pDLMapTable.end() ) return NULL;

	return it->second;
}

bool CDNGameDataManager::LoadDarkLairClearData()
{
#ifdef _WORK
	DNTableFileFormat *pSox;
	if (m_bAllLoaded)
		pSox = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TDLDUNGEONCLEAR );
	else
		pSox = GetDNTable( CDnTableDB::TDLDUNGEONCLEAR );
#else		//#ifdef _WORK
	DNTableFileFormat *pSox = GetDNTable( CDnTableDB::TDLDUNGEONCLEAR );
#endif		//#ifdef _WORK
	if( !pSox ) {
		g_Log.Log(LogType::_FILELOG, L"DLStageClearTable.dnt failed\r\n");
		return false;
	}
	if (pSox->GetItemCount() <= 0){
		g_Log.Log(LogType::_FILELOG, L"DLStageClearTable.dnt Count(%d)\r\n", pSox->GetItemCount());
		return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		SAFE_DELETE_PMAPF( TMapDLClearData, m_pDLClearTable, SAFE_DELETE_PVEC( TMapDLClearData_iter->second->pVecClearItem ) );
	}
#endif		//#ifdef _WORK

	struct TempFieldNum
	{
		int _ClearTableID;
		int _LevelMin;
		int _LevelMax;
		int _RoundMin;
		int _RoundMax;
		int _RewardExperience;
		int _ShowBoxCount;
		int _SelectBoxCount;
		int _TreasureBoxRatio[4];
		int _RewardItemID[4];
		int _ReturnWorldID;
		int _ReturnWorldGateID;
		int _RewardGP;
	};

	TempFieldNum sFieldNum;
	sFieldNum._ClearTableID = pSox->GetFieldNum( "_ClearTableID" );
	sFieldNum._LevelMin = pSox->GetFieldNum( "_FromLevel" );
	sFieldNum._LevelMax = pSox->GetFieldNum( "_ToLevel" );
	sFieldNum._RoundMin = pSox->GetFieldNum( "_FromRound" );
	sFieldNum._RoundMax = pSox->GetFieldNum( "_ToRound" );
	sFieldNum._RewardExperience = pSox->GetFieldNum( "_ClearRewardExp" );
	sFieldNum._ShowBoxCount = pSox->GetFieldNum( "_ShowBoxNum" );
	sFieldNum._SelectBoxCount = pSox->GetFieldNum( "_SelectBoxNum" );
	static char *szBoxStr[] = { "_Bronze", "_Silver", "_Gold", "_Platinum" };
	char szLabel[32];
	for( int i=0; i<4; i++ ) {
		sprintf_s( szLabel, "%sTreasureBoxRatioDL", szBoxStr[i] );
		sFieldNum._TreasureBoxRatio[i] = pSox->GetFieldNum( szLabel );
		sprintf_s( szLabel, "%sItemTableID", szBoxStr[i] );
		sFieldNum._RewardItemID[i] = pSox->GetFieldNum( szLabel );
	}
	sFieldNum._ReturnWorldID = pSox->GetFieldNum( "_ReturnWorldID" );
	sFieldNum._ReturnWorldGateID = pSox->GetFieldNum( "_ReturnWorldStartPos" );
	sFieldNum._RewardGP = pSox->GetFieldNum( "_RewardGP" );

	// Load
	for( int i=0 ; i<pSox->GetItemCount() ; ++i )
	{
		TDLClearDataItem *pData = new TDLClearDataItem;
		memset( pData, 0, sizeof(TDLClearDataItem) );

		int nItemID = pSox->GetItemID(i);
		int iIdx = pSox->GetIDXprimary( nItemID );

		int nClearTablEID = pSox->GetFieldFromLablePtr( iIdx, sFieldNum._ClearTableID )->GetInteger();
		pData->nLevelMin = pSox->GetFieldFromLablePtr( iIdx, sFieldNum._LevelMin )->GetInteger();
		pData->nLevelMax = pSox->GetFieldFromLablePtr( iIdx, sFieldNum._LevelMax )->GetInteger();
		pData->nRoundMin = pSox->GetFieldFromLablePtr( iIdx, sFieldNum._RoundMin )->GetInteger();
		pData->nRoundMax = pSox->GetFieldFromLablePtr( iIdx, sFieldNum._RoundMax )->GetInteger();
		pData->nRewardExperience = pSox->GetFieldFromLablePtr( iIdx, sFieldNum._RewardExperience )->GetInteger();
		pData->nShowBoxCount = pSox->GetFieldFromLablePtr( iIdx, sFieldNum._ShowBoxCount )->GetInteger();
		pData->nSelectBoxCount = pSox->GetFieldFromLablePtr( iIdx, sFieldNum._SelectBoxCount )->GetInteger();
		for( int j=0; j<4; j++ ) {
			pData->cTreasureBoxRatio[j] = (char)(( pSox->GetFieldFromLablePtr( iIdx, sFieldNum._TreasureBoxRatio[j] )->GetFloat() + 0.0001f ) * 100.f );
			pData->nRewardItemID[j] = pSox->GetFieldFromLablePtr( iIdx, sFieldNum._RewardItemID[j] )->GetInteger();
		}
		pData->nReturnWorldSetID = pSox->GetFieldFromLablePtr( iIdx, sFieldNum._ReturnWorldID )->GetInteger();
		pData->nReturnWorldGateID = pSox->GetFieldFromLablePtr( iIdx, sFieldNum._ReturnWorldGateID )->GetInteger();
		pData->nRewardGP = pSox->GetFieldFromLablePtr( iIdx, sFieldNum._RewardGP )->GetInteger();

		TMapDLClearData::iterator it = m_pDLClearTable.find( nClearTablEID );
		if( it != m_pDLClearTable.end() ) {
			it->second->pVecClearItem.push_back( pData );
		}
		else {
			TDLClearData *pClearData = new TDLClearData;
			pClearData->pVecClearItem.push_back( pData );
			m_pDLClearTable.insert( make_pair( nClearTablEID, pClearData ) );
		}
	}

	return true;
}

TDLClearDataItem *CDNGameDataManager::GetDLClearData( int nItemID, int nLevel, int nRound )
{
	TMapDLClearData::iterator it = m_pDLClearTable.find( nItemID );
	if( it == m_pDLClearTable.end() ) return NULL;

	TDLClearDataItem *pItem;
	for( DWORD i=0; i<it->second->pVecClearItem.size(); i++ ) {
		pItem = it->second->pVecClearItem[i];
		if( nLevel >= pItem->nLevelMin && nLevel <= pItem->nLevelMax && nRound >= pItem->nRoundMin && nRound <= pItem->nRoundMax ) return pItem;
	}
	return NULL;
}

bool CDNGameDataManager::GetCreateDefaultSkill( char cClassID, int* aDefaultSkills )
{
	if( !aDefaultSkills )
		return false;

	map<int, TDefaultCreateData>::iterator iter = m_mapDefaultCreateData.find( cClassID );
	if( m_mapDefaultCreateData.end() == iter )
		return false;
	

	memcpy( aDefaultSkills, iter->second.nDefaultSkillID, sizeof(int) * DEFAULTSKILLMAX );
	return true;
}


#ifdef PRE_ADD_GACHA_JAPAN
bool CDNGameDataManager::LoadGachaData_JP( void )
{
#ifdef _WORK
	DNTableFileFormat* pSox;
	if (m_bAllLoaded)
		pSox = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TGACHA_JP );
	else
		pSox = GetDNTable( CDnTableDB::TGACHA_JP );
#else		//#ifdef _WORK
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TGACHA_JP );
#endif		//#ifdef _WORK
	if( !pSox )
	{
		g_Log.Log(LogType::_FILELOG, L"GachaJp.dnt failed\r\n");
		return false;
	}

	if( pSox->GetItemCount() <= 0 )
	{
		g_Log.Log(LogType::_FILELOG, L"GachaJp.dnt Count(%d)\r\n", pSox->GetItemCount());
		// return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		SAFE_DELETE_PMAP( TMapGachaData_JP, m_mapGachaData );
	}
#endif		//#ifdef _WORK

	struct TempFieldNum
	{
		int _GachaNum;
		int _LinkedDrop[ CASHEQUIPMAX ];
	};

	TempFieldNum sFieldNum;
	SecureZeroMemory( &sFieldNum, sizeof(TempFieldNum) );

	sFieldNum._GachaNum = pSox->GetFieldNum( "_GachaNum" );
	sFieldNum._LinkedDrop[ CASHEQUIP_HELMET ] = pSox->GetFieldNum( "_LinkedDrop1" );
	sFieldNum._LinkedDrop[ CASHEQUIP_BODY ] = pSox->GetFieldNum( "_LinkedDrop2" );
	sFieldNum._LinkedDrop[ CASHEQUIP_LEG ] = pSox->GetFieldNum( "_LinkedDrop3" );
	sFieldNum._LinkedDrop[ CASHEQUIP_HAND ] = pSox->GetFieldNum( "_LinkedDrop4" );
	sFieldNum._LinkedDrop[ CASHEQUIP_FOOT ] = pSox->GetFieldNum( "_LinkedDrop5" );
	sFieldNum._LinkedDrop[ CASHEQUIP_NECKLACE ] = pSox->GetFieldNum( "_LinkedDrop6" );

	// Load
	for( int iIndex = 0; iIndex < pSox->GetItemCount(); )
	{
		TGachaponData_JP* pData = NULL;

		int iItemID = pSox->GetItemID( iIndex );
		int iIdx = pSox->GetIDXprimary( iItemID );
		int nGachaShopIndex = pSox->GetFieldFromLablePtr( iIdx, sFieldNum._GachaNum )->GetInteger();

		if( m_mapGachaData.end() == m_mapGachaData.find( nGachaShopIndex ) )
		{
			m_mapGachaData[ nGachaShopIndex ] = new TGachaponData_JP;
			SecureZeroMemory( m_mapGachaData[ nGachaShopIndex ], sizeof(TGachaponData_JP) );
		}

		TGachaponData_JP* pGachaponData= m_mapGachaData[ nGachaShopIndex ];
		pGachaponData->nGachaponIndex = nGachaShopIndex;

		// 직업 인덱스 순서대로 4개 값이 있음. 아카데믹 추가되면 5개. 루프 인덱스는 1부터 시작한다.
		for( int nClassID = 1; nClassID <= CLASSKINDMAX; ++nClassID )
		{
#ifdef PRE_FIX_LOAD_GACHA_DATA
			// 현재 1차부터 15차까지는 CLASS:4 / 16차부터는 CLASS:5 로 들어가 있음. 서버는 각 차수의 데이터를 전부 읽어주는 형식으로 되어있어 아래 코드가 있지 않으면 index가 밀리는 현상 발생.
			int nGachaponTableId = pSox->GetItemID(iIndex);
			int iCurGachaponIDXPrimary = pSox->GetIDXprimary( nGachaponTableId );
			int nCurGachaShopIndex = pSox->GetFieldFromLablePtr( iCurGachaponIDXPrimary, sFieldNum._GachaNum )->GetInteger();
			if (nCurGachaShopIndex != nGachaShopIndex)
				break;
#endif

			TGachaponShopInfoByJob_JP& GachaShopInfoByJob = pGachaponData->GachaponShopInfo[ nClassID-1 ];
			GachaShopInfoByJob.nGachaponIndex = nGachaShopIndex;
			GachaShopInfoByJob.nGachaponTableID = pSox->GetItemID( iIndex );
			GachaShopInfoByJob.nJobClassID = nClassID;

			for( int nPart = CASHEQUIPMIN; nPart < CASHEQUIPMAX; ++nPart )
			{
				// 현재까지는 목걸이 까지만 테이블이 정의되어있다. 추후에 테이블이 수정되면 같이 수정해줘야 함.
				if( CASHEQUIP_NECKLACE < nPart )
					break;

				iItemID = pSox->GetItemID( iIndex );
				iIdx = pSox->GetIDXprimary( iItemID );
				GachaShopInfoByJob.nPartsLinkDropTableID[ nPart ] = pSox->GetFieldFromLablePtr( iIdx, sFieldNum._LinkedDrop[ nPart ] )->GetInteger();
			}

			++iIndex;
		}
	}

	return true;
}

TGachaponData_JP* CDNGameDataManager::GetGachaponData_JP( int nGachaShopID )
{
	TMapGachaData_JP::iterator iter = m_mapGachaData.find( nGachaShopID );
	if( m_mapGachaData.end() != iter )
		return iter->second;

	return NULL;
}
#endif // PRE_ADD_GACHA_JAPAN

bool CDNGameDataManager::LoadLevelupEvent()
{
#ifdef _WORK
	DNTableFileFormat *pSox;
	if (m_bAllLoaded)
		pSox = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TLEVELUPEVENT );
	else
		pSox = GetDNTable( CDnTableDB::TLEVELUPEVENT );
#else		//#ifdef _WORK
	DNTableFileFormat *pSox = GetDNTable( CDnTableDB::TLEVELUPEVENT );
#endif		//#ifdef _WORK
	if( !pSox ) {
		g_Log.Log(LogType::_FILELOG, L"LevelupEventTable.dnt failed\r\n");
		return false;
	}

	if( pSox->GetItemCount() <= 0 ) {
		g_Log.Log(LogType::_FILELOG, L"LevelupEventTable.dnt Count(%d)\r\n", pSox->GetItemCount());
		// return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		SAFE_DELETE_PVEC(m_pVecLevelupEvent);
	}
#endif		//#ifdef _WORK

	char szTemp[MAX_PATH] = { 0, };

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{
		int _PCLevel;
		int _ClassID;
		int _Job;
		int _EventType;
		int _MailID;
		int _CashMailID;
	};

	TempFieldNum sFieldNum;
	sFieldNum._PCLevel = pSox->GetFieldNum("_PCLevel");
	sFieldNum._ClassID = pSox->GetFieldNum("_ClassID");
	sFieldNum._Job = pSox->GetFieldNum("_Job");
	sFieldNum._EventType = pSox->GetFieldNum("_EventType");

	sFieldNum._MailID = pSox->GetFieldNum("_MailID");
	sFieldNum._CashMailID = pSox->GetFieldNum("_CashMailID");

	//##################################################################
	// Load
	//##################################################################

	for (int i = 0; i < pSox->GetItemCount(); i++){
		TLevelupEvent *pLevelupEvent = new TLevelupEvent;
		//2010.10.7 haling STL 보호 위해 생성자로 변환
		//memset(pLevelupEvent, 0, sizeof(TLevelupEvent));

		int nID = pSox->GetItemID(i);
		int nIdx = pSox->GetIDXprimary(nID);

		pLevelupEvent->nLevel = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._PCLevel)->GetInteger();
		pLevelupEvent->cClassID = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._ClassID)->GetInteger();
		pLevelupEvent->cJob = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._Job)->GetInteger();
		pLevelupEvent->nEventType = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._EventType)->GetInteger();
		pLevelupEvent->nMailID = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._MailID)->GetInteger();
		pLevelupEvent->nCashMailID = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._CashMailID)->GetInteger();

		m_pVecLevelupEvent.push_back(pLevelupEvent);
	}

	return true;
}

void CDNGameDataManager::GetLevelupEvent(int nLevel, int nClass, int nJob, std::vector<TLevelupEvent*> &VecEventList)
{
	VecEventList.clear();
	if(m_pVecLevelupEvent.empty()) return;

	for (int i = 0; i <(int)m_pVecLevelupEvent.size(); i++){
		if (m_pVecLevelupEvent[i]->nLevel != nLevel) continue;
		if( m_pVecLevelupEvent[i]->cJob > 0 )
		{
			if( m_pVecLevelupEvent[i]->cJob != nJob )
				continue;
		}
		else
		{
			if ((m_pVecLevelupEvent[i]->cClassID != 0) && (m_pVecLevelupEvent[i]->cClassID != nClass)) 
				continue;	// classid 0이면 모두 포함
		}

		VecEventList.push_back(m_pVecLevelupEvent[i]);
	}
}

bool CDNGameDataManager::GetLevelupEventbyType(int nLevel, TLevelupEvent::eEventType eType, TLevelupEvent &Event)
{
	//아오 꼬롬해~ 복귀유저 관련해서 특정 메일아이디 둘곳이 없어서 여기에 넣으셨답니다. 타입3번 레벨업테이블을 읽기는 하지만 복귀유저 이벤트용임!
	//타입으로 읽어서 사용해야한다면 우준홍씨와 이야기 해야함	
	if (m_pVecLevelupEvent.empty()) return false;
	for (int i = 0; i <(int)m_pVecLevelupEvent.size(); i++)
	{
		if (m_pVecLevelupEvent[i]->nLevel != nLevel) continue;
		if (m_pVecLevelupEvent[i]->nEventType == eType)
		{
			Event = *m_pVecLevelupEvent[i];
			return true;
		}
	}
	return false;
}

bool CDNGameDataManager::LoadPromotionData()
{
#ifdef _WORK
	DNTableFileFormat *pSox;
	if (m_bAllLoaded)
		pSox = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TLEVELPROMO );
	else
		pSox = GetDNTable( CDnTableDB::TLEVELPROMO );
#else		//#ifdef _WORK
	DNTableFileFormat *pSox = GetDNTable( CDnTableDB::TLEVELPROMO );
#endif		//#ifdef _WORK
	if( !pSox ) {
		g_Log.Log(LogType::_FILELOG, L"WelfareTable.dnt failed\r\n");
		return false;
	}

	if( pSox->GetItemCount() <= 0 )
		g_Log.Log(LogType::_FILELOG, L"WelfareTable.dnt Count(%d)\r\n", pSox->GetItemCount());

#ifdef _WORK
	if (m_bAllLoaded)
	{
		SAFE_DELETE_PVEC( m_vLvPromotion );
	}
#endif		//#ifdef _WORK

	for (int i = 0; i < pSox->GetItemCount(); i++)
	{
		TPromotionData * pPromo = new TPromotionData;
		memset(pPromo, 0, sizeof(TPromotionData));

		pPromo->nID = pSox->GetItemID(i);
		pPromo->nConditionType = pSox->GetFieldFromLablePtr(pPromo->nID, "_UserConditionType1")->GetInteger();
		pPromo->nConditionValue = atoi(pSox->GetFieldFromLablePtr(pPromo->nID, "_UserConditionValue1")->GetString());
		pPromo->nPromotionType = pSox->GetFieldFromLablePtr(pPromo->nID, "_RewardType1")->GetInteger();
		float fTemp = (float)atof(pSox->GetFieldFromLablePtr(pPromo->nID, "_RewardValue1")->GetString());
		pPromo->nRewardValue = (int)(fTemp * 100);
		
		m_vLvPromotion.push_back(pPromo);
	}
	return true;
}

int CDNGameDataManager::GetPromotionCount()
{
	return(int)m_vLvPromotion.size();
}

const TPromotionData * CDNGameDataManager::GetPromotionByIdx(int nIndex)
{
	if( nIndex < 0 || nIndex >= (int)m_vLvPromotion.size() ) return NULL;
	return m_vLvPromotion[nIndex];
}

const TPromotionData * CDNGameDataManager::GetPromotionByIID(int nID)
{
	if (nID < 0 || m_vLvPromotion.empty()) return NULL;
	TVecPromotion::iterator ii;
	for(ii = m_vLvPromotion.begin(); ii != m_vLvPromotion.end(); ii++)
	{
		if ((*ii)->nID == nID)
			return(*ii);
	}
	return NULL;
}

//---------------------------------------------------------------------------------
// ItemOptionTable(ItemOptionTable.dnt - TItemOptionTableData)
//---------------------------------------------------------------------------------

bool CDNGameDataManager::LoadItemOptionTableData()
{
#ifdef _WORK
	DNTableFileFormat* pSox;
	if (m_bAllLoaded)
		pSox = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TITEMOPTION );
	else
		pSox = GetDNTable( CDnTableDB::TITEMOPTION );
#else		//#ifdef _WORK
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TITEMOPTION );
#endif		//#ifdef _WORK
	if( !pSox )
	{
		g_Log.Log( LogType::_FILELOG, L"ItemOption.dnt failed\r\n" );
		return false;
	}

	if( pSox->GetItemCount() <= 0 )
	{
		g_Log.Log( LogType::_FILELOG, L"ItemOption.dnt Count(%d)\r\n", pSox->GetItemCount() );
		return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		m_ItemOptionTableData.clear();
	}
#endif		//#ifdef _WORK

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{
		int Enchant;
		int Potential1;
		int Potential2;
	};

	TempFieldNum sFieldNum;
	sFieldNum.Enchant		= pSox->GetFieldNum( "_Enchant" );
	sFieldNum.Potential1	= pSox->GetFieldNum( "_Potential1" );
	sFieldNum.Potential2	= pSox->GetFieldNum( "_Potential2" );

	//##################################################################
	// Load
	//##################################################################

	for( int i=0 ; i<pSox->GetItemCount() ; ++i )
	{
		TItemOptionTableData Data;
		int nItemID = pSox->GetItemID(i);

		int iIdx	= pSox->GetIDXprimary( nItemID );

		Data.iEnchant	= pSox->GetFieldFromLablePtr( iIdx, sFieldNum.Enchant )->GetInteger();
		Data.iOption	= pSox->GetFieldFromLablePtr( iIdx, sFieldNum.Potential1 )->GetInteger();
		Data.iPotential = pSox->GetFieldFromLablePtr( iIdx, sFieldNum.Potential2 )->GetInteger();

		m_ItemOptionTableData.insert( std::make_pair(nItemID,Data) );
	}

	return true;
}

const TItemOptionTableData* CDNGameDataManager::GetItemOptionTableData( int iItemID )
{
	TMapItemOptionTableData::iterator itor = m_ItemOptionTableData.find( iItemID );
	if( itor != m_ItemOptionTableData.end() )
		return &itor->second;

	return NULL;
}

//---------------------------------------------------------------------------------
// CombinedShopTable(CombiedShopTable.dnt - TCombinedShopTableData)
//---------------------------------------------------------------------------------

bool CDNGameDataManager::LoadCombinedShopTableData()
{
#ifdef _WORK
	DNTableFileFormat* pSox;
	if (m_bAllLoaded)
		pSox = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TCOMBINEDSHOP );
	else
		pSox = GetDNTable( CDnTableDB::TCOMBINEDSHOP );
#else		//#ifdef _WORK
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TCOMBINEDSHOP );
#endif		//#ifdef _WORK
	if( !pSox )
	{
		g_Log.Log( LogType::_FILELOG, L"CombinedShopTable.dnt failed\r\n" );
		return false;
	}

	if( pSox->GetItemCount() <= 0 )
	{
		g_Log.Log( LogType::_FILELOG, L"CombinedShopTable.dnt Count(%d)\r\n", pSox->GetItemCount() );
		return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		m_CombinedShopTableData.clear();
	}
#endif		//#ifdef _WORK

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{
		int ShopID;
		int TabID;
		int ListID;
		int ItemIndex;
		int Quantity;
		int PurchaseType[Shop::Max::PurchaseType];
		int PurchaseItem[Shop::Max::PurchaseType];
		int PurchaseItemValue[Shop::Max::PurchaseType];
		int PurchaseLimitType;
		int PurchaseLimitValue;
		int buyLimitCount; // 구매 개수 제한.
#if defined (PRE_ADD_COMBINEDSHOP_PERIOD)
		int Period;			// 기간제 아이템
#endif
#if defined( PRE_ADD_LIMITED_SHOP )
		int ShopLimitReset;
#endif
#if defined(PRE_SAMPLEITEMNPC)
		int SampleID;
#endif	// #if defined(PRE_SAMPLEITEMNPC)
	};

	char szBuf[MAX_PATH];

	TempFieldNum sFieldNum;
	sFieldNum.ShopID = pSox->GetFieldNum( "_ShopID" );
	sFieldNum.TabID	= pSox->GetFieldNum( "_TabID" );
	sFieldNum.ListID = pSox->GetFieldNum( "_ListID" );
	sFieldNum.ItemIndex = pSox->GetFieldNum( "_itemindex" );
	sFieldNum.Quantity = pSox->GetFieldNum( "_Quantity" );
	sFieldNum.PurchaseLimitType = pSox->GetFieldNum( "_PurchaseLimitType" );
	sFieldNum.PurchaseLimitValue = pSox->GetFieldNum( "_PurchaseLimitValue" );
	
	for( int i=1 ; i<=Shop::Max::PurchaseType ; ++i )
	{
		sprintf_s( szBuf, "_PurchaseType%d", i );
		sFieldNum.PurchaseType[i-1] = pSox->GetFieldNum( szBuf );
		sprintf_s( szBuf, "_PurchaseItem%d", i );
		sFieldNum.PurchaseItem[i-1] = pSox->GetFieldNum( szBuf );
		sprintf_s( szBuf, "_PurchaseItemValue%d", i );
		sFieldNum.PurchaseItemValue[i-1] = pSox->GetFieldNum( szBuf );
	}
	sFieldNum.buyLimitCount = pSox->GetFieldNum( "_buyLimitCount" );
#if defined (PRE_ADD_COMBINEDSHOP_PERIOD)
	sFieldNum.Period = pSox->GetFieldNum( "_Period" );
#endif
#if defined( PRE_ADD_LIMITED_SHOP )
	sFieldNum.ShopLimitReset = pSox->GetFieldNum( "_ShopLimitReset" );
#endif
#if defined(PRE_SAMPLEITEMNPC)
	sFieldNum.SampleID = pSox->GetFieldNum( "_SampleID" );
#endif	// #if defined(PRE_SAMPLEITEMNPC)

	//##################################################################
	// Load
	//##################################################################

	std::vector<int> vAbuseItemID;

	for( int i=0 ; i<pSox->GetItemCount() ; ++i )
	{
		TCombinedShopTableData Data;
		memset( &Data, 0, sizeof(Data) );
		
		int nItemID = pSox->GetItemID(i);

		int iIdx = pSox->GetIDXprimary( nItemID );
		
		int iShopID = pSox->GetFieldFromLablePtr( iIdx, sFieldNum.ShopID )->GetInteger();
		if( iShopID <= 0 )
			continue;

		int iTabID = pSox->GetFieldFromLablePtr( iIdx, sFieldNum.TabID )->GetInteger();
		int iListID = pSox->GetFieldFromLablePtr( iIdx, sFieldNum.ListID )->GetInteger();
		Data.ShopItem.nItemID = pSox->GetFieldFromLablePtr( iIdx, sFieldNum.ItemIndex )->GetInteger();
		Data.ShopItem.nCount = pSox->GetFieldFromLablePtr( iIdx, sFieldNum.Quantity )->GetInteger();
		Data.ShopItem.buyLimitCount = pSox->GetFieldFromLablePtr(iIdx, sFieldNum.buyLimitCount)->GetInteger();

#if defined (PRE_ADD_COMBINEDSHOP_PERIOD)
		Data.ShopItem.nPeriod = pSox->GetFieldFromLablePtr(iIdx, sFieldNum.Period)->GetInteger();
#endif		
#if defined( PRE_ADD_LIMITED_SHOP )
		Data.ShopItem.nShopLimitReset = pSox->GetFieldFromLablePtr(iIdx, sFieldNum.ShopLimitReset)->GetInteger();
#endif
		const TItemData* pItemData = GetItemData( Data.ShopItem.nItemID );
		if( !pItemData ) 
		{
			g_Log.Log( LogType::_FILELOG, L"CombinedShopTable.dnt ShopID:%d ItemID:%d Invalid\r\n", iShopID, Data.ShopItem.nItemID );
			return false;
		}

		Data.ShopItem.nMaxCount = pItemData->nOverlapCount;
		Data.PurchaseLimitType = static_cast<Shop::PurchaseLimitType::eCode>(pSox->GetFieldFromLablePtr( iIdx, sFieldNum.PurchaseLimitType )->GetInteger());
		Data.iPurchaseLimitValue = pSox->GetFieldFromLablePtr( iIdx, sFieldNum.PurchaseLimitValue )->GetInteger();
#if defined(PRE_SAMPLEITEMNPC)
		Data.nSampleVersion = pSox->GetFieldFromLablePtr(iIdx, sFieldNum.SampleID)->GetInteger();
#endif	// #if defined(PRE_SAMPLEITEMNPC)

		for( int k=0 ; k<Shop::Max::PurchaseType ; ++k )
		{
			Data.PurchaseType[k].PurchaseType = static_cast<Shop::PurchaseType::eCode>(pSox->GetFieldFromLablePtr( iIdx, sFieldNum.PurchaseType[k] )->GetInteger());
			Data.PurchaseType[k].iPurchaseItemID = pSox->GetFieldFromLablePtr( iIdx, sFieldNum.PurchaseItem[k] )->GetInteger();
			Data.PurchaseType[k].iPurchaseItemValue = pSox->GetFieldFromLablePtr( iIdx, sFieldNum.PurchaseItemValue[k] )->GetInteger();

#if defined( PRE_UNIONSHOP_RENEWAL )
			if( Data.PurchaseType[k].PurchaseType == Shop::PurchaseType::UnionPoint )
			{
				if( Data.PurchaseType[k].iPurchaseItemID < 0 || Data.PurchaseType[k].iPurchaseItemID >= NpcReputation::UnionType::Etc )
				{
					g_Log.Log( LogType::_FILELOG, L"CombinedShopTable.dnt ShopID:%d ItemID:%d UnionPoint PurchaseItemID:%d Error\r\n", iShopID, Data.ShopItem.nItemID, Data.PurchaseType[k].iPurchaseItemID );
					return false;
				}
			}
#endif // #if defined( PRE_UNIONSHOP_RENEWAL )

			if( Data.PurchaseType[k].PurchaseType == Shop::PurchaseType::Gold )
			{
				Data.ShopItem.nPrice = Data.PurchaseType[k].iPurchaseItemValue;
				int nItemBuyPrice = Data.ShopItem.nPrice - (int)(Data.ShopItem.nPrice * MAX_ITEMPRICE_RATE);
				if (pItemData->nSellAmount > nItemBuyPrice)
					vAbuseItemID.push_back(nItemID);
			}

			// 중복 PurchaseType 검사
			if( k>0 && Data.PurchaseType[k].PurchaseType != Shop::PurchaseType::None )
			{
				for( int j=0 ; j<k ; ++j )
				{
					if( Data.PurchaseType[j].PurchaseType == Data.PurchaseType[k].PurchaseType )
					{
						g_Log.Log( LogType::_FILELOG, L"CombinedShopTable.dnt ShopID:%d ItemID:%d Duplicate PurchaseType\r\n", iShopID, Data.ShopItem.nItemID );
						return false;
					}
				}
			}
		}

		TMapCombinedShopTableData::iterator itor = m_CombinedShopTableData.find( std::tr1::make_tuple(iShopID,iTabID,iListID) );
		if( itor == m_CombinedShopTableData.end() )
		{
			m_CombinedShopTableData.insert( std::make_pair(std::tr1::make_tuple(iShopID,iTabID,iListID),Data) );
		}
		else
		{
			g_Log.Log( LogType::_FILELOG, L"CombinedShopTable.dnt ShopID:%d TabID:%d ListID:%d Duplicated\r\n", iShopID, iTabID, iListID );
			return false;
		}

		{
			std::map<int,int>::iterator itor = m_CombinedShopIndex.find( iShopID );
			if( itor == m_CombinedShopIndex.end() )
			{
				m_CombinedShopIndex.insert( std::make_pair(iShopID,1) );
			}
			else
			{
				++(*itor).second;
			}
		}
	}

	if (vAbuseItemID.size() > 0)
	{
		WCHAR szTemp[1024];
		std::wstring szError = L"Invalid CombinedShopTable\n\n";

		for(UINT i=0; i<vAbuseItemID.size(); i++)
		{
			swprintf_s( szTemp, L"ItemID : %d\n", vAbuseItemID[i] );
			szError += szTemp;
		}

		MessageBox( NULL, szError.c_str(), L"Critical Error!!", MB_OK );
		return false;
	}

	return true;
}

const TCombinedShopTableData* CDNGameDataManager::GetCombinedShopItemTableData( int iShopID, int iTabID, int iListID )
{
	TMapCombinedShopTableData::iterator itor = m_CombinedShopTableData.find( std::tr1::make_tuple(iShopID,iTabID,iListID) );
	if( itor != m_CombinedShopTableData.end() )
		return &(*itor).second;

	return NULL;
}

bool CDNGameDataManager::IsCombinedShop( int iShopID )
{
	std::map<int,int>::iterator itor = m_CombinedShopIndex.find( iShopID );
	if( itor != m_CombinedShopIndex.end() )
		return((*itor).second>0);

	return false;
}

#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )

//---------------------------------------------------------------------------------
// ReputeTable(ReputeTable.dnt - TReputeTableData)
//---------------------------------------------------------------------------------

bool CDNGameDataManager::LoadReputeTableData()
{
#ifdef _WORK
	DNTableFileFormat* pSox;
	if (m_bAllLoaded)
		pSox = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TREPUTE );
	else
		pSox = GetDNTable( CDnTableDB::TREPUTE );
#else		//#ifdef _WORK
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TREPUTE );
#endif		//#ifdef _WORK
	if( !pSox )
	{
		g_Log.Log( LogType::_FILELOG, L"ReputeTable.dnt failed\r\n" );
		return false;
	}

	if( pSox->GetItemCount() <= 0 )
	{
		g_Log.Log( LogType::_FILELOG, L"ReputeTable.dnt Count(%d)\r\n", pSox->GetItemCount() );
		return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		m_ReputeTableData.clear();
	}
#endif		//#ifdef _WORK

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{
		int NpcID;
#if defined( PRE_ADD_REPUTATION_EXPOSURE )
		int Mission;
		int NpcPresentRepute1;
		int NpcPresentRepute2;
		int NpcPresentID1;
		int NpcPresentID2;
#else
		int QuestID;
#endif // #if defined( PRE_ADD_REPUTATION_EXPOSURE )
		int MaxFavor;
		int TakeFavor;
		int MaxMalice;
		int AddMalice;
		int PresentID[6];
		int FavorNpc;
		int FavorGroupBomb;
		int MaliceNpc;
		int MaliceGroupBomb;
		int	MailID[NpcReputation::Common::MaxMailCount];
		int MailRand[NpcReputation::Common::MaxMailCount];
		int PlusItemID;
		int PlusProb;
		int UnionID;
		int PresentPoint[6];
	};

	TempFieldNum sFieldNum;
	sFieldNum.NpcID			= pSox->GetFieldNum( "_NpcID" );
#if defined( PRE_ADD_REPUTATION_EXPOSURE )
	sFieldNum.Mission			= pSox->GetFieldNum( "_Mission" );
	sFieldNum.NpcPresentRepute1	= pSox->GetFieldNum( "_NpcPresentRepute1" );
	sFieldNum.NpcPresentRepute2	= pSox->GetFieldNum( "_NpcPresentRepute2" );
	sFieldNum.NpcPresentID1		= pSox->GetFieldNum( "_NpcPresentID1" );
	sFieldNum.NpcPresentID2		= pSox->GetFieldNum( "_NpcPresentID2" );
#else
	sFieldNum.QuestID		= pSox->GetFieldNum( "_QuestID2" );
#endif // #if defined( PRE_ADD_REPUTATION_EXPOSURE )
	sFieldNum.MaxFavor		= pSox->GetFieldNum( "_MaxFavor" );
	sFieldNum.TakeFavor		= pSox->GetFieldNum( "_TakeFavor" );
	sFieldNum.MaxMalice		= pSox->GetFieldNum( "_MaxMalice" );
	sFieldNum.AddMalice		= pSox->GetFieldNum( "_AddMalice" );
	for( UINT i=0 ; i<_countof(sFieldNum.PresentID) ; ++i )
	{
		char szBuf[MAX_PATH];
		sprintf_s( szBuf, "_PresentID%d", i+1 );
		sFieldNum.PresentID[i] = pSox->GetFieldNum( szBuf );
	}
	for( UINT i=0 ; i<NpcReputation::Common::MaxMailCount ; ++i )
	{
		char szBuf[MAX_PATH];
		sprintf_s( szBuf, "_MailID%d", i+1 );
		sFieldNum.MailID[i] = pSox->GetFieldNum( szBuf );
		sprintf_s( szBuf, "_Probability%d", i+1 );
		sFieldNum.MailRand[i] = pSox->GetFieldNum( szBuf );
	}
	sFieldNum.FavorNpc			= pSox->GetFieldNum( "_FavorNpc" );
	sFieldNum.FavorGroupBomb	= pSox->GetFieldNum( "_FavorGroupBomb" );
	sFieldNum.MaliceNpc			= pSox->GetFieldNum( "_MaliceNpc" );
	sFieldNum.MaliceGroupBomb	= pSox->GetFieldNum( "_MaliceGroupBomb" );
	sFieldNum.PlusItemID		= pSox->GetFieldNum( "_PlusItemID" );
	sFieldNum.PlusProb			= pSox->GetFieldNum( "_PlusProb" );
	sFieldNum.UnionID			= pSox->GetFieldNum( "_UnionID" );
	for( UINT i=0 ; i<_countof(sFieldNum.PresentPoint) ; ++i )
	{
		char szBuf[MAX_PATH];
		sprintf_s( szBuf, "_PresentPointID%d", i+1 );
		sFieldNum.PresentPoint[i] = pSox->GetFieldNum( szBuf );
	}

	//##################################################################
	// Load
	//##################################################################


	for( int i=0 ; i<pSox->GetItemCount() ; ++i )
	{
		TReputeTableData Data;
		int nItemID = pSox->GetItemID(i);

		int iIdx	= pSox->GetIDXprimary( nItemID );
		int iNpcID	= pSox->GetFieldFromLablePtr( iIdx, sFieldNum.NpcID )->GetInteger();
		if( iNpcID == 0 )
			continue;

#if defined( PRE_ADD_REPUTATION_EXPOSURE )
		Data.iMissionID			= pSox->GetFieldFromLablePtr( iIdx, sFieldNum.Mission )->GetInteger();
		Data.iNpcPresentRepute1	= pSox->GetFieldFromLablePtr( iIdx, sFieldNum.NpcPresentRepute1 )->GetInteger();
		Data.iNpcPresentRepute2	= pSox->GetFieldFromLablePtr( iIdx, sFieldNum.NpcPresentRepute2 )->GetInteger();
		Data.iNpcPresentID1		= pSox->GetFieldFromLablePtr( iIdx, sFieldNum.NpcPresentID1 )->GetInteger();
		Data.iNpcPresentID2		= pSox->GetFieldFromLablePtr( iIdx, sFieldNum.NpcPresentID2 )->GetInteger();
#else
		std::vector<std::string> questTokens;

		std::string szReputeClearQuestString = pSox->GetFieldFromLablePtr(iIdx, sFieldNum.QuestID)->GetString();
		TokenizeA(szReputeClearQuestString, questTokens, ",");

		std::vector<std::string>::iterator iter = questTokens.begin();
		for(; iter != questTokens.end(); ++iter)
		{
			std::string& token = (*iter);
			int questId = atoi(token.c_str());
			Data.iQuestIDs.push_back(questId);
		}
#endif // #if defined( PRE_ADD_REPUTATION_EXPOSURE )

		Data.iMaxFavor			= pSox->GetFieldFromLablePtr( iIdx, sFieldNum.MaxFavor )->GetInteger();
		Data.iTakeFavorPerDay	= pSox->GetFieldFromLablePtr( iIdx, sFieldNum.TakeFavor )->GetInteger();
		Data.iMaxMalice			= pSox->GetFieldFromLablePtr( iIdx, sFieldNum.MaxMalice )->GetInteger();
		Data.iAddMalicePerDay	= pSox->GetFieldFromLablePtr( iIdx, sFieldNum.AddMalice )->GetInteger();
		for( UINT j=0 ; j<_countof(sFieldNum.PresentID) ; ++j )
			Data.iPresentIDArr[j] = pSox->GetFieldFromLablePtr( iIdx, sFieldNum.PresentID[j] )->GetInteger();
		for( UINT j=0 ; j<NpcReputation::Common::MaxMailCount ; ++j )
		{
			Data.iMailID[j]		= pSox->GetFieldFromLablePtr( iIdx, sFieldNum.MailID[j] )->GetInteger();
			if( Data.iMailID[j] <= 0 )
				break;
			Data.iMailRand[j]	= pSox->GetFieldFromLablePtr( iIdx, sFieldNum.MailRand[j] )->GetInteger();

			if( Data.iMailRand[j] <= 0 || Data.iMailRand[j] > NpcReputation::Common::MaxMailRandValue )
			{
				_ASSERT(0);
				g_Log.Log( LogType::_FILELOG, L"ReputeTable.dnt MailID:%d 확률이상\r\n", Data.iMailID[j] );
				return false;
			}
		}
		
		char* szStr = pSox->GetFieldFromLablePtr( iIdx, sFieldNum.FavorNpc )->GetString();
		std::vector<std::string> tokens;
		TokenizeA( szStr, tokens, ";" );
		_ASSERT( tokens.size() <= NpcReputation::Common::MaxFavorEffectNpcCount );
		for( UINT j=0 ; j<tokens.size() ; ++j )
			Data.vFavorNpcID.push_back( atoi(tokens[j].c_str()) );

		szStr = pSox->GetFieldFromLablePtr( iIdx, sFieldNum.MaliceNpc )->GetString();
		tokens.clear();
		TokenizeA( szStr, tokens, ";" );
		_ASSERT( tokens.size() <= NpcReputation::Common::MaxMaliceEffectNpcCount );
		for( UINT j=0 ; j<tokens.size() ; ++j )
			Data.vMaliceNpcID.push_back( atoi(tokens[j].c_str()) );

		Data.iFavorGroupBomb	= pSox->GetFieldFromLablePtr( iIdx, sFieldNum.FavorGroupBomb )->GetInteger();
		Data.iMaliceGroupBomb	= pSox->GetFieldFromLablePtr( iIdx, sFieldNum.MaliceGroupBomb )->GetInteger();
		Data.iPlusItemID		= pSox->GetFieldFromLablePtr( iIdx, sFieldNum.PlusItemID )->GetInteger();
		Data.iPlusProb			= pSox->GetFieldFromLablePtr( iIdx, sFieldNum.PlusProb )->GetInteger();
		Data.iUnionID			= pSox->GetFieldFromLablePtr( iIdx, sFieldNum.UnionID )->GetInteger();
		for( UINT j=0 ; j<_countof(sFieldNum.PresentPoint) ; ++j )
			Data.iPresentPointArr[j] = pSox->GetFieldFromLablePtr( iIdx, sFieldNum.PresentPoint[j] )->GetInteger();

		m_ReputeTableData.insert( std::make_pair(iNpcID,Data) );
	}

	return true;
}

TReputeTableData* CDNGameDataManager::GetReputeTableData( int iItemID )
{
	TMapReputeTableData::iterator itor = m_ReputeTableData.find( iItemID );
	if( itor != m_ReputeTableData.end() )
		return &itor->second;

	return NULL;
}

//---------------------------------------------------------------------------------
// PresentTable(PresentTable.dnt - TPresentTableData)
//---------------------------------------------------------------------------------

bool CDNGameDataManager::LoadPresentTableData()
{
#ifdef _WORK
	DNTableFileFormat* pSox;
	if (m_bAllLoaded)
		pSox = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TPRESENT );
	else
		pSox = GetDNTable( CDnTableDB::TPRESENT );
#else		//#ifdef _WORK
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TPRESENT );
#endif		//#ifdef _WORK
	if( !pSox )
	{
		g_Log.Log( LogType::_FILELOG, L"PresentTable.dnt failed\r\n" );
		return false;
	}

	if( pSox->GetItemCount() <= 0 )
	{
		g_Log.Log( LogType::_FILELOG, L"PresentTable.dnt Count(%d)\r\n", pSox->GetItemCount() );
		return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		m_PresentTableData.clear();
	}
#endif		//#ifdef _WORK

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{
		int PresentType;
		int PresentTypeID;
		int Count;
		int AddFavorPoint;
		int TakeMalicePoint;
		int FavorGroupPoint;
		int MaliceGroupPoint;
	};

	TempFieldNum sFieldNum;
	sFieldNum.PresentType		= pSox->GetFieldNum( "_PresentType" );
	sFieldNum.PresentTypeID		= pSox->GetFieldNum( "_PresentTypeID" );
	sFieldNum.Count				= pSox->GetFieldNum( "_Count" );
	sFieldNum.AddFavorPoint		= pSox->GetFieldNum( "_AddFavorPoint" );
	sFieldNum.TakeMalicePoint	= pSox->GetFieldNum( "_TakeMalicePoint" );
	sFieldNum.FavorGroupPoint	= pSox->GetFieldNum( "_FavorGroupPoint" );
	sFieldNum.MaliceGroupPoint	= pSox->GetFieldNum( "_MaliceGroupPoint" );

	//##################################################################
	// Load
	//##################################################################


	for( int i=0 ; i<pSox->GetItemCount() ; ++i )
	{
		TPresentTableData Data;
		int nItemID = pSox->GetItemID(i);

		int iIdx = pSox->GetIDXprimary( nItemID );

		Data.Type				= static_cast<TPresentTableData::eType>(pSox->GetFieldFromLablePtr( iIdx, sFieldNum.PresentType )->GetInteger());
		Data.iTypeID			= pSox->GetFieldFromLablePtr( iIdx, sFieldNum.PresentTypeID )->GetInteger();
		Data.iCount				= pSox->GetFieldFromLablePtr( iIdx, sFieldNum.Count )->GetInteger();
		Data.iAddFavorPoint		= pSox->GetFieldFromLablePtr( iIdx, sFieldNum.AddFavorPoint )->GetInteger();
		Data.iTakeMalicePoint	= pSox->GetFieldFromLablePtr( iIdx, sFieldNum.TakeMalicePoint )->GetInteger();
		Data.iFavorGroupPoint	= pSox->GetFieldFromLablePtr( iIdx, sFieldNum.FavorGroupPoint )->GetInteger();
		Data.iMaliceGroupPoint	= pSox->GetFieldFromLablePtr( iIdx, sFieldNum.MaliceGroupPoint )->GetInteger();

		_ASSERT( Data.iFavorGroupPoint >= 0 );
		_ASSERT( Data.iMaliceGroupPoint >= 0 );

		m_PresentTableData.insert( std::make_pair(nItemID,Data) );
	}

	return true;
}

TPresentTableData* CDNGameDataManager::GetPresentTableData( int iItemID )
{
	TMapPresentTableData::iterator itor = m_PresentTableData.find( iItemID );
	if( itor != m_PresentTableData.end() )
		return &itor->second;

	return NULL;
}

//---------------------------------------------------------------------------------
// MailTable(MailTable.dnt - TMailTableData)
//---------------------------------------------------------------------------------

bool CDNGameDataManager::LoadMailTableData()
{
	if( m_pItemData.empty() )
	{
		_ASSERT( 0 );
		g_Log.Log( LogType::_FILELOG, L"ItemTable을 먼저 로드해야합니다.\r\n" );
		return false;
	}

#ifdef _WORK
	DNTableFileFormat* pSox;
	if (m_bAllLoaded)
		pSox = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TMAIL );
	else
		pSox = GetDNTable( CDnTableDB::TMAIL );
#else		//#ifdef _WORK
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TMAIL );
#endif		//#ifdef _WORK
	if( !pSox )
	{
		g_Log.Log( LogType::_FILELOG, L"MailTable.dnt failed\r\n" );
		return false;
	}

	if( pSox->GetItemCount() <= 0 )
	{
		g_Log.Log( LogType::_FILELOG, L"MailTable.dnt Count(%d)\r\n", pSox->GetItemCount() );
		return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		m_MailTableData.clear();
	}
#endif		//#ifdef _WORK

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{
		int MailType;
		int MailSender;
		int MailTitle;
		int MailText;
		int MailPresentItem[MAILATTACHITEMMAX];
		int Count[MAILATTACHITEMMAX];
		int	MailPresentMoney;
		int IsCash;
#if defined(PRE_SPECIALBOX)
		int KeepBoxReceive;
		int KeepBoxType;
		int KeepBoxLevelMin;
		int KeepBoxLevelMax;
		int KeepBoxClass;
#endif	// #if defined(PRE_SPECIALBOX)
	};

	TempFieldNum sFieldNum;
	sFieldNum.MailType = pSox->GetFieldNum( "_MailType" );
	sFieldNum.MailSender = pSox->GetFieldNum( "_MailSender" );
	sFieldNum.MailTitle = pSox->GetFieldNum( "_MailTitle" );
	sFieldNum.MailText = pSox->GetFieldNum( "_MailText" );
	sFieldNum.MailPresentMoney = pSox->GetFieldNum( "_MailPresentMoney" );
	sFieldNum.IsCash = pSox->GetFieldNum( "_IsCash" );
#if defined(PRE_SPECIALBOX)
	sFieldNum.KeepBoxReceive = pSox->GetFieldNum( "_KeepBoxReceive" );
	sFieldNum.KeepBoxType = pSox->GetFieldNum( "_KeepBoxType" );
	sFieldNum.KeepBoxLevelMin = pSox->GetFieldNum( "_KeepBoxLevelMin" );
	sFieldNum.KeepBoxLevelMax = pSox->GetFieldNum( "_KeepBoxLevelMax" );
	sFieldNum.KeepBoxClass = pSox->GetFieldNum( "_KeepBoxClass" );
#endif	// #if defined(PRE_SPECIALBOX)

	for( int i=1 ; i<=MAILATTACHITEMMAX ; ++i )
	{
		char szBuf[MAX_PATH];
		
		sprintf( szBuf, "_MailPresentItem%d", i );
		sFieldNum.MailPresentItem[i-1] = pSox->GetFieldNum( szBuf );
		sprintf( szBuf, "_Count%d", i );
		sFieldNum.Count[i-1] = pSox->GetFieldNum( szBuf );
	}

	//##################################################################
	// Load
	//##################################################################


	for( int i=0 ; i<pSox->GetItemCount() ; ++i )
	{
		TMailTableData Data;
		int nItemID = pSox->GetItemID(i);

		int iIdx = pSox->GetIDXprimary( nItemID );

		Data.Code = static_cast<DBDNWorldDef::MailTypeCode::eCode>(pSox->GetFieldFromLablePtr( iIdx, sFieldNum.MailType )->GetInteger());
		Data.nSenderUIStringIndex = pSox->GetFieldFromLablePtr( iIdx, sFieldNum.MailSender )->GetInteger();
		Data.nTitleUIStringIndex = pSox->GetFieldFromLablePtr( iIdx, sFieldNum.MailTitle )->GetInteger();
		Data.nTextUIStringIndex = pSox->GetFieldFromLablePtr( iIdx, sFieldNum.MailText )->GetInteger();
		Data.IsCash = pSox->GetFieldFromLablePtr( iIdx, sFieldNum.IsCash )->GetInteger() ? true : false;
#if defined(PRE_SPECIALBOX)
		Data.nKeepBoxReceive = pSox->GetFieldFromLablePtr( iIdx, sFieldNum.KeepBoxReceive )->GetInteger();
		Data.nKeepBoxType = pSox->GetFieldFromLablePtr( iIdx, sFieldNum.KeepBoxType )->GetInteger();
		Data.nKeepBoxLevelMin = pSox->GetFieldFromLablePtr( iIdx, sFieldNum.KeepBoxLevelMin )->GetInteger();
		Data.nKeepBoxLevelMax = pSox->GetFieldFromLablePtr( iIdx, sFieldNum.KeepBoxLevelMax )->GetInteger();
		Data.nKeepBoxClass = pSox->GetFieldFromLablePtr( iIdx, sFieldNum.KeepBoxClass )->GetInteger();
#endif	// #if defined(PRE_SPECIALBOX)

		for( int i=0 ; i<MAILATTACHITEMMAX ; ++i )
		{
			if(Data.IsCash){
				Data.ItemSNArr[i] = pSox->GetFieldFromLablePtr( iIdx, sFieldNum.MailPresentItem[i] )->GetInteger();
			}
			else{
				Data.ItemIDArr[i] = pSox->GetFieldFromLablePtr( iIdx, sFieldNum.MailPresentItem[i] )->GetInteger();
				if( Data.ItemIDArr[i] <= 0 )
					continue;
				_ASSERT( GetItemData( Data.ItemIDArr[i] ) );
				if( GetItemData( Data.ItemIDArr[i] ) == NULL )
				{
					WCHAR wszBuf[MAX_PATH];
					wsprintf( wszBuf, L"MailTable ID:%d ItemID:%d not found!!\r\n", nItemID, Data.ItemIDArr[i] );
					g_Log.Log( LogType::_FILELOG, wszBuf );
					return false;
				}
				Data.ItemCountArr[i] = pSox->GetFieldFromLablePtr( iIdx, sFieldNum.Count[i] )->GetInteger();
			}
		}

		Data.nPresentMoney = pSox->GetFieldFromLablePtr( iIdx, sFieldNum.MailPresentMoney )->GetInteger();

		m_MailTableData.insert( std::make_pair(nItemID,Data) );
	}

	return true;
}

TMailTableData* CDNGameDataManager::GetMailTableData( int iItemID )
{
	TMapMailTableData::iterator itor = m_MailTableData.find( iItemID );
	if( itor != m_MailTableData.end() )
		return &itor->second;

	return NULL;
}

bool CDNGameDataManager::LoadStoreBenefitData()
{
#ifdef _WORK
	DNTableFileFormat* pSox;
	if (m_bAllLoaded)
		pSox = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TSTOREBENEFIT );
	else
		pSox = GetDNTable( CDnTableDB::TSTOREBENEFIT );
#else		//#ifdef _WORK
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TSTOREBENEFIT );
#endif		//#ifdef _WORK
	if( !pSox )
	{
		_ASSERT( 0 );
		g_Log.Log( LogType::_FILELOG, L"StoreBenefit.dnt failed\r\n" );
		return false;
	}

	if( pSox->GetItemCount() <= 0 )
	{
		g_Log.Log( LogType::_FILELOG, L"StoreBenefit.dnt Count(%d)\r\n", pSox->GetItemCount() );
		return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		m_StoreBenefitData.clear();
	}
#endif		//#ifdef _WORK

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################
	struct TempFieldNum
	{
		int iNpcID;
		int iBenefitType;
		int aiFavorThreshold[ STORE_BENEFIT_MAX ];
		int aiDiscountPercent[ STORE_BENEFIT_MAX ];
	};

	TempFieldNum sFieldNum;
	sFieldNum.iNpcID = pSox->GetFieldNum( "_NpcID" );
	sFieldNum.iBenefitType = pSox->GetFieldNum( "_Benefit" );

	for( int i = 0; i < STORE_BENEFIT_MAX; ++i )
	{
		CHAR szBuf[ MAX_PATH ] = { 0 };
		sprintf_s( szBuf, "_Favor%d", (i+1) );
		sFieldNum.aiFavorThreshold[ i ] = pSox->GetFieldNum( szBuf );
		sprintf_s( szBuf, "_Discount%d", (i+1) );
		sFieldNum.aiDiscountPercent[ i ] = pSox->GetFieldNum( szBuf );
	}


	//##################################################################
	// Load
	//##################################################################

	for( int i = 0; i < pSox->GetItemCount(); ++i )
	{
		TStoreBenefitData Data;
		int iItemID = pSox->GetItemID( i );

		int iIdx = pSox->GetIDXprimary( iItemID );

		Data.iNpcID = pSox->GetFieldFromLablePtr( iIdx, sFieldNum.iNpcID )->GetInteger();
		Data.Type = static_cast<TStoreBenefitData::eType>(pSox->GetFieldFromLablePtr( iIdx, sFieldNum.iBenefitType )->GetInteger());

		for( int k = 0; k < STORE_BENEFIT_MAX; ++k )
		{
			Data.aiFavorThreshold[ k ] = pSox->GetFieldFromLablePtr( iIdx, sFieldNum.aiFavorThreshold[ k ] )->GetInteger();
			Data.aiAdjustPercent[ k ] = pSox->GetFieldFromLablePtr( iIdx, sFieldNum.aiDiscountPercent[ k ] )->GetInteger();
		}

		m_StoreBenefitData.insert( make_pair(Data.iNpcID, Data) );
	}

	return true;
}

void CDNGameDataManager::GetStoreBenefitData( int iNpcID, vector<TStoreBenefitData*>& vlpDatas )
{
	pair<TMapStoreBenefitData::iterator, TMapStoreBenefitData::iterator> iter_pair = m_StoreBenefitData.equal_range( iNpcID );

	TMapStoreBenefitData::iterator iter = iter_pair.first;
	for( iter; iter != iter_pair.second; ++iter )
		vlpDatas.push_back( &(iter->second) );
}

#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
bool CDNGameDataManager::LoadMasterSystemDecreaseTableData()
{
#ifdef _WORK
	DNTableFileFormat* pSox;
	if (m_bAllLoaded)
		pSox = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TMASTERSYSTEM_DECREASE );
	else
		pSox = GetDNTable( CDnTableDB::TMASTERSYSTEM_DECREASE );
#else		//#ifdef _WORK
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TMASTERSYSTEM_DECREASE );
#endif		//#ifdef _WORK
	if( !pSox )
	{
		g_Log.Log( LogType::_FILELOG, L"MasterSysDecreaseTable.dnt failed\r\n" );
		return false;
	}

	if( pSox->GetItemCount() <= 0 )
	{
		g_Log.Log( LogType::_FILELOG, L"MasterSysDecreaseTable.dnt Count(%d)\r\n", pSox->GetItemCount() );
		return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		m_MasterSystemDecreaseTableData.clear();
	}
#endif		//#ifdef _WORK

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{
		int PupilLevel;
		int PupilFeel;
		int FeelUPDecreaseRespect;
		int FeelDownDecreaseRespect;
		int MasterSuccessMailID;
		int MasterFailureMailID;
		int PupilSuccessMailID;
		int PupilFailuserMailID;
	};

	TempFieldNum sFieldNum;
	sFieldNum.PupilLevel				= pSox->GetFieldNum( "_PupilLevel" );
	sFieldNum.PupilFeel					= pSox->GetFieldNum( "_PupilFeel" );
	sFieldNum.FeelUPDecreaseRespect		= pSox->GetFieldNum( "_FeelUPDecreaseRespect" );
	sFieldNum.FeelDownDecreaseRespect	= pSox->GetFieldNum( "_FeelDownDecreaseRespect" );
	sFieldNum.MasterSuccessMailID		= pSox->GetFieldNum( "_MasterSuccessMailID" );
	sFieldNum.MasterFailureMailID		= pSox->GetFieldNum( "_MasterFailureMailID" );
	sFieldNum.PupilSuccessMailID		= pSox->GetFieldNum( "_PupilSuccessMailID" );
	sFieldNum.PupilFailuserMailID		= pSox->GetFieldNum( "_PupilFailureMailID" );

	//##################################################################
	// Load
	//##################################################################

	for( int i=0 ; i<pSox->GetItemCount() ; ++i )
	{
		TMasterSystemDecreaseTableData Data;
		int nItemID = pSox->GetItemID(i);

		int iIdx = pSox->GetIDXprimary( nItemID );
		
		Data.iPupilLevel					= pSox->GetFieldFromLablePtr( iIdx, sFieldNum.PupilLevel )->GetInteger();
		Data.iPupilFavor					= pSox->GetFieldFromLablePtr( iIdx, sFieldNum.PupilFeel )->GetInteger();
		Data.iFavorUpDecreaseRespectPoint	= pSox->GetFieldFromLablePtr( iIdx, sFieldNum.FeelUPDecreaseRespect )->GetInteger();
		Data.iFavorDownDecreaseRespectPoint	= pSox->GetFieldFromLablePtr( iIdx, sFieldNum.FeelDownDecreaseRespect )->GetInteger();
		Data.iMasterSuccessMailID			= pSox->GetFieldFromLablePtr( iIdx, sFieldNum.MasterSuccessMailID )->GetInteger();
		Data.iMasterFailureMailID			= pSox->GetFieldFromLablePtr( iIdx, sFieldNum.MasterFailureMailID )->GetInteger();
		Data.iPupilSuccessMailID			= pSox->GetFieldFromLablePtr( iIdx, sFieldNum.PupilSuccessMailID )->GetInteger();
		Data.iPupilFailureMailID			= pSox->GetFieldFromLablePtr( iIdx, sFieldNum.PupilFailuserMailID )->GetInteger();

		m_MasterSystemDecreaseTableData.insert( std::make_pair(Data.iPupilLevel, Data) );
	}

	return true;
}

TMasterSystemDecreaseTableData*	CDNGameDataManager::GetMasterSystemDecreaseTableData( int iPupilLevel )
{
	TMapMasterSystemDecreaseTableData::iterator itor = m_MasterSystemDecreaseTableData.find( iPupilLevel );
	if( itor != m_MasterSystemDecreaseTableData.end() )
		return &itor->second;

	return NULL;
}

bool CDNGameDataManager::LoadMasterSystemGainTableData()
{
#ifdef _WORK
	DNTableFileFormat* pSox;
	if (m_bAllLoaded)
		pSox = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TMASTERSYSTEM_GAIN );
	else
		pSox = GetDNTable( CDnTableDB::TMASTERSYSTEM_GAIN );
#else		//#ifdef _WORK
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TMASTERSYSTEM_GAIN );
#endif		//#ifdef _WORK
	if( !pSox )
	{
		g_Log.Log( LogType::_FILELOG, L"MasterSysGainTable.dnt failed\r\n" );
		return false;
	}

	if( pSox->GetItemCount() <= 0 )
	{
		g_Log.Log( LogType::_FILELOG, L"MasterSysGainTable.dnt Count(%d)\r\n", pSox->GetItemCount() );
		return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		m_MasterSystemGainTableData.clear();
	}
#endif		//#ifdef _WORK

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{
		int LvlMin;
		int	StageGrade;
		int	GainFeel;
		int GainRespect;
	};

	TempFieldNum sFieldNum;
	sFieldNum.LvlMin		= pSox->GetFieldNum( "_LvlMin" );
	sFieldNum.StageGrade	= pSox->GetFieldNum( "_StageGrade" );
	sFieldNum.GainFeel		= pSox->GetFieldNum( "_GainFeel" );
	sFieldNum.GainRespect	= pSox->GetFieldNum( "_GainRespect" );

	//##################################################################
	// Load
	//##################################################################

	for( int i=0 ; i<pSox->GetItemCount() ; ++i )
	{
		TMasterSystemGainTableData Data;
		int nItemID = pSox->GetItemID(i);

		int iIdx = pSox->GetIDXprimary( nItemID );

		int LvlMin		= pSox->GetFieldFromLablePtr( iIdx, sFieldNum.LvlMin )->GetInteger();
		int StageGrade	= pSox->GetFieldFromLablePtr( iIdx, sFieldNum.StageGrade )->GetInteger();
		
		if( LvlMin <= 0 )
			continue;

		Data.iFavorPoint	= pSox->GetFieldFromLablePtr( iIdx, sFieldNum.GainFeel )->GetInteger();
		Data.iRepectPoint	= pSox->GetFieldFromLablePtr( iIdx, sFieldNum.GainRespect )->GetInteger();

		m_MasterSystemGainTableData.insert( std::make_pair( std::make_pair(LvlMin,static_cast<Dungeon::Difficulty::eCode>(StageGrade)), Data) );
	}

	return true;
}

TMasterSystemGainTableData*	CDNGameDataManager::GetMasterSystemGainTableData( int iLevel, TDUNGEONDIFFICULTY StageDifficulty )
{
	TMapMasterSystemGainTableData::iterator itor = m_MasterSystemGainTableData.find( std::make_pair(iLevel,StageDifficulty) );
	if( itor != m_MasterSystemGainTableData.end() )
		return &itor->second;

	return NULL;
}

#if defined( PRE_ADD_SECONDARY_SKILL )

bool CDNGameDataManager::LoadSecondarySkillTableData()
{
#ifdef _WORK
	DNTableFileFormat* pSox;
	if (m_bAllLoaded)
		pSox = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TSecondarySkill );
	else
		pSox = GetDNTable( CDnTableDB::TSecondarySkill );
#else		//#ifdef _WORK
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TSecondarySkill );
#endif		//#ifdef _WORK
	if( !pSox )
	{
		g_Log.Log( LogType::_FILELOG, L"SecondarySkillTable.dnt failed\r\n" );
		return false;
	}

	if( pSox->GetItemCount() <= 0 )
	{
		g_Log.Log( LogType::_FILELOG, L"SecondarySkillTable.dnt Count(%d)\r\n", pSox->GetItemCount() );
		return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		m_SecondarySkillTableData.clear();
	}
#endif		//#ifdef _WORK

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{
		int SecondarySkillKind;
		int SecondarySkillType;
		int SecondarySkillCharacter;
	};

	TempFieldNum sFieldNum;
	sFieldNum.SecondarySkillKind		= pSox->GetFieldNum( "_SecondarySkillKind" );
	sFieldNum.SecondarySkillType		= pSox->GetFieldNum( "_SecondarySkillType" );
	sFieldNum.SecondarySkillCharacter	= pSox->GetFieldNum( "_SecondarySkillCharacter" );

	//##################################################################
	// Load
	//##################################################################

	for( int i=0 ; i<pSox->GetItemCount() ; ++i )
	{
		TSecondarySkillTableData Data;
		int nItemID = pSox->GetItemID(i);

		int iIdx = pSox->GetIDXprimary( nItemID );

		Data.iSkillID		= nItemID;
		Data.Type			= static_cast<SecondarySkill::Type::eType>(pSox->GetFieldFromLablePtr( iIdx, sFieldNum.SecondarySkillKind )->GetInteger());
		Data.SubType		= static_cast<SecondarySkill::SubType::eType>(pSox->GetFieldFromLablePtr( iIdx, sFieldNum.SecondarySkillType )->GetInteger());
		Data.ExecuteType	= static_cast<SecondarySkill::ExecuteType::eType>(pSox->GetFieldFromLablePtr( iIdx, sFieldNum.SecondarySkillCharacter )->GetInteger());

		m_SecondarySkillTableData.insert( std::make_pair(Data.iSkillID,Data) );
	}

	return true;
}

TSecondarySkillTableData* CDNGameDataManager::GetSecondarySkillTableData( int iSecondarySkillID )
{
	TMapSecondarySkillTableData::iterator itor = m_SecondarySkillTableData.find( iSecondarySkillID );
	if( itor != m_SecondarySkillTableData.end() )
		return &itor->second;

	return NULL;
}

bool CDNGameDataManager::LoadSecondarySkillLevelTableData()
{
#ifdef _WORK
	DNTableFileFormat* pSox;
	if (m_bAllLoaded)
		pSox = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TSecondarySkillLevel );
	else
		pSox = GetDNTable( CDnTableDB::TSecondarySkillLevel );
#else		//#ifdef _WORK
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TSecondarySkillLevel );
#endif		//#ifdef _WORK
	if( !pSox )
	{
		g_Log.Log( LogType::_FILELOG, L"SecondarySkillLevelTable.dnt failed\r\n" );
		return false;
	}

	if( pSox->GetItemCount() <= 0 )
	{
		g_Log.Log( LogType::_FILELOG, L"SecondarySkillLevelTable.dnt Count(%d)\r\n", pSox->GetItemCount() );
		return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		m_SecondarySkillLevelTableData.clear();
	}
#endif		//#ifdef _WORK

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{
		int NextID;
		int SecondarySkillID;
		int SecondarySkillClass;
		int SecondarySkillLevel;
		int SecondarySkillExp;
		int SecondarySkillParam;
	};

	TempFieldNum sFieldNum;
	sFieldNum.NextID				= pSox->GetFieldNum( "NextID" );
	sFieldNum.SecondarySkillID		= pSox->GetFieldNum( "_SecondarySkillID" );
	sFieldNum.SecondarySkillClass	= pSox->GetFieldNum( "_SecondarySkillClass" );
	sFieldNum.SecondarySkillLevel	= pSox->GetFieldNum( "_SecondarySkillLevel" );
	sFieldNum.SecondarySkillExp		= pSox->GetFieldNum( "_SecondarySkillExp" );
	sFieldNum.SecondarySkillParam	= pSox->GetFieldNum( "_SecondarySkillParam" );

	//##################################################################
	// Load
	//##################################################################

	for( int i=0 ; i<pSox->GetItemCount() ; ++i )
	{
		int iSumExp = 0;

		TSecondarySkillLevelTableData Data;
		int nItemID		= pSox->GetItemID(i);
		int iIdx		= pSox->GetIDXprimary( nItemID );

		Data.iNextID	= pSox->GetFieldFromLablePtr( iIdx, sFieldNum.NextID )->GetInteger();
		Data.iSkillID	= pSox->GetFieldFromLablePtr( iIdx, sFieldNum.SecondarySkillID )->GetInteger();
		Data.Grade		= static_cast<SecondarySkill::Grade::eType>(pSox->GetFieldFromLablePtr( iIdx, sFieldNum.SecondarySkillClass )->GetInteger());
		Data.iLevel		= pSox->GetFieldFromLablePtr( iIdx, sFieldNum.SecondarySkillLevel )->GetInteger();
		Data.iExp		= pSox->GetFieldFromLablePtr( iIdx, sFieldNum.SecondarySkillExp )->GetInteger();
		Data.iParam		= pSox->GetFieldFromLablePtr( iIdx, sFieldNum.SecondarySkillParam )->GetInteger();
		
		// Head 읽기
		if( !(Data.Grade == SecondarySkill::Grade::Beginning && Data.iLevel == 1) )
			continue;

		iSumExp += Data.iExp;

		std::vector<TSecondarySkillLevelTableData> vData;
		vData.push_back( Data );
		
		// Tail 읽기
		while( Data.iNextID > 0 )
		{
			iIdx			= pSox->GetIDXprimary( Data.iNextID );

			Data.iNextID	= pSox->GetFieldFromLablePtr( iIdx, sFieldNum.NextID )->GetInteger();
			Data.iSkillID	= pSox->GetFieldFromLablePtr( iIdx, sFieldNum.SecondarySkillID )->GetInteger();
			Data.Grade		= static_cast<SecondarySkill::Grade::eType>(pSox->GetFieldFromLablePtr( iIdx, sFieldNum.SecondarySkillClass )->GetInteger());
			Data.iLevel		= pSox->GetFieldFromLablePtr( iIdx, sFieldNum.SecondarySkillLevel )->GetInteger();
			Data.iExp		= pSox->GetFieldFromLablePtr( iIdx, sFieldNum.SecondarySkillExp )->GetInteger();
			Data.iParam		= pSox->GetFieldFromLablePtr( iIdx, sFieldNum.SecondarySkillParam )->GetInteger();

			iSumExp += Data.iExp;
			Data.iExp = iSumExp;

			vData.push_back( Data );
		}

		m_SecondarySkillLevelTableData.insert( std::make_pair(Data.iSkillID,vData) );
	}

	return true;
}

TSecondarySkillLevelTableData* CDNGameDataManager::GetSecondarySkillLevelTableData( int iSecondarySkillID, SecondarySkill::Grade::eType Grade, int iSkillLevel )
{
	TMapSecondarySkillLevelTableData::iterator itor = m_SecondarySkillLevelTableData.find( iSecondarySkillID );
	if( itor != m_SecondarySkillLevelTableData.end() )
	{
		for( UINT i=0 ; i<(*itor).second.size() ; ++i )
		{
			if((*itor).second[i].Grade == Grade && (*itor).second[i].iLevel == iSkillLevel )
				return &(*itor).second[i];
		}
	}

	return NULL;
}

TSecondarySkillLevelTableData* CDNGameDataManager::GetSecondarySkillLevelTableData( int iSecondarySkillID, int iExp )
{
	TMapSecondarySkillLevelTableData::iterator itor = m_SecondarySkillLevelTableData.find( iSecondarySkillID );
	if( itor == m_SecondarySkillLevelTableData.end() )
		return NULL;

	if( (*itor).second.empty() )
		return NULL;

	for( UINT i=0 ; i<(*itor).second.size() ; ++i )
	{
		if( iExp <(*itor).second[i].iExp )
			return &(*itor).second[i];
	}

	// 마지막 Table Return
	return &(*itor).second[(*itor).second.size()-1];
}

bool CDNGameDataManager::LoadSecondarySkillRecipeTableData()
{
#ifdef _WORK
	DNTableFileFormat* pSox;
	if (m_bAllLoaded)
		pSox = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TSecondarySkillRecipe );
	else
		pSox = GetDNTable( CDnTableDB::TSecondarySkillRecipe );
#else		//#ifdef _WORK
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TSecondarySkillRecipe );
#endif		//#ifdef _WORK
	if( !pSox )
	{
		g_Log.Log( LogType::_FILELOG, L"makeitemrecipetable.dnt failed\r\n" );
		return false;
	}

	if( pSox->GetItemCount() <= 0 )
	{
		g_Log.Log( LogType::_FILELOG, L"makeitemrecipetable.dnt Count(%d)\r\n", pSox->GetItemCount() );
		return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		m_SecondarySkillRecipeTableData.clear();
	}
#endif		//#ifdef _WORK

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{
		int MakeItemID[SecondarySkill::ManufactureSkill::MaxManufactureItemTypeCount];
		int MinExp[SecondarySkill::ManufactureSkill::MaxManufactureItemTypeCount];
		int MaxExp[SecondarySkill::ManufactureSkill::MaxManufactureItemTypeCount];
		int SecondarySkillClass;
		int SecondarySkillLevel;
		int ToolItemID[SecondarySkill::ManufactureSkill::MaxToolItemCount];
		int MasterExp;
		int MaxTime;
		int SuccessProbability;
		int	SuccessUpExp;
		int RecipeUpExp;
		int MaterialItemID[SecondarySkill::ManufactureSkill::MaxMaterialItemCount];
		int MaterialItemCount[SecondarySkill::ManufactureSkill::MaxMaterialItemCount];
		int StartExp;
		int SuccessCount;
	};

	char szBuf[MAX_PATH];
	TempFieldNum sFieldNum;
	for( UINT i=0 ; i<_countof(sFieldNum.MakeItemID) ; ++i )
	{
		sprintf_s( szBuf, "_MakeItem%dID", i+1 );
		sFieldNum.MakeItemID[i]	= pSox->GetFieldNum( szBuf );
		sprintf_s( szBuf, "_MinExp%d", i+1 );
		sFieldNum.MinExp[i]	= pSox->GetFieldNum( szBuf );
		sprintf_s( szBuf, "_MaxExp%d", i+1 );
		sFieldNum.MaxExp[i]	= pSox->GetFieldNum( szBuf );
	}
	sFieldNum.SecondarySkillClass		= pSox->GetFieldNum( "_SecondarySkillClass" );
	sFieldNum.SecondarySkillLevel		= pSox->GetFieldNum( "_SecondarySkillLevel" );
	for( UINT i=0 ; i<_countof(sFieldNum.ToolItemID) ; ++i )
	{
		sprintf_s( szBuf, "_ToolItem%dID", i+1 );
		sFieldNum.ToolItemID[i]	= pSox->GetFieldNum( szBuf );
	}

	sFieldNum.MasterExp				= pSox->GetFieldNum( "_MasterExp" );
	sFieldNum.MaxTime				= pSox->GetFieldNum( "_MaxTime" );
	sFieldNum.SuccessProbability	= pSox->GetFieldNum( "_SuccessProbability" );
	sFieldNum.SuccessUpExp			= pSox->GetFieldNum( "_SuccessUpExp" );
	sFieldNum.RecipeUpExp			= pSox->GetFieldNum( "_RecipeUpExp" );

	for( UINT i=0 ; i<_countof(sFieldNum.MakeItemID) ; ++i )
	{
		sprintf_s( szBuf, "_MaterialItem%dID", i+1 );
		sFieldNum.MaterialItemID[i]	= pSox->GetFieldNum( szBuf );
		sprintf_s( szBuf, "_Count%d", i+1 );
		sFieldNum.MaterialItemCount[i]	= pSox->GetFieldNum( szBuf );
	}

	sFieldNum.StartExp				= pSox->GetFieldNum( "_StartExp" );
	sFieldNum.SuccessCount			= pSox->GetFieldNum( "_SuccessCount" );

	//##################################################################
	// Load
	//##################################################################

	for( int i=0 ; i<pSox->GetItemCount() ; ++i )
	{
		TSecondarySkillRecipeTableData Data;

		int nItemID		= pSox->GetItemID(i);
		int iIdx		= pSox->GetIDXprimary( nItemID );

		for( UINT j=0 ; j<_countof(Data.iManufactureItemID) ; ++j )
		{
			Data.iManufactureItemID[j]	= pSox->GetFieldFromLablePtr( iIdx, sFieldNum.MakeItemID[j] )->GetInteger();
			Data.iMinExp[j]				= pSox->GetFieldFromLablePtr( iIdx, sFieldNum.MinExp[j] )->GetInteger();
			Data.iMaxExp[j]				= pSox->GetFieldFromLablePtr( iIdx, sFieldNum.MaxExp[j] )->GetInteger();
		}
		for( UINT j=0 ; j<_countof(Data.iToolItemID) ; ++j )
			Data.iToolItemID[j] = pSox->GetFieldFromLablePtr( iIdx, sFieldNum.ToolItemID[j] )->GetInteger();
		for( UINT j=0 ; j<_countof(Data.iMaterialItemID) ; ++j )
		{
			Data.iMaterialItemID[j]		= pSox->GetFieldFromLablePtr( iIdx, sFieldNum.MaterialItemID[j] )->GetInteger();
			Data.iMaterialItemCount[j]	= pSox->GetFieldFromLablePtr( iIdx, sFieldNum.MaterialItemCount[j] )->GetInteger();
		}
		Data.RequireGrade			= static_cast<SecondarySkill::Grade::eType>(pSox->GetFieldFromLablePtr( iIdx, sFieldNum.SecondarySkillClass)->GetInteger());
		Data.iRequireSkillLevel		= pSox->GetFieldFromLablePtr( iIdx, sFieldNum.SecondarySkillLevel )->GetInteger();
		Data.iMasterExp				= pSox->GetFieldFromLablePtr( iIdx, sFieldNum.MasterExp )->GetInteger();
		Data.iMaxTime				= pSox->GetFieldFromLablePtr( iIdx, sFieldNum.MaxTime )->GetInteger();
		Data.iSuccessProbability	= pSox->GetFieldFromLablePtr( iIdx, sFieldNum.SuccessProbability )->GetInteger();
		Data.iSuccessUpExp			= pSox->GetFieldFromLablePtr( iIdx, sFieldNum.SuccessUpExp )->GetInteger();
		Data.iRecipeUpExp			= pSox->GetFieldFromLablePtr( iIdx, sFieldNum.RecipeUpExp )->GetInteger();
		Data.iStartExp				= pSox->GetFieldFromLablePtr( iIdx, sFieldNum.StartExp )->GetInteger();
		Data.iSuccessCount			= pSox->GetFieldFromLablePtr( iIdx, sFieldNum.SuccessCount )->GetInteger();
		if( Data.iSuccessCount <= 0 )
		{
			g_Log.Log( LogType::_FILELOG, L"makeitemrecipetable.dnt ItemID:%d SuccessCount:%d Invalid\r\n", nItemID, Data.iSuccessCount );
			_ASSERT(0);
			return false;
		}

		m_SecondarySkillRecipeTableData.insert( std::make_pair(nItemID,Data) );
	}

	return true;
}

TSecondarySkillRecipeTableData* CDNGameDataManager::GetSecondarySkillRecipeTableData( int iItemID )
{
	TMapSecondarySkillRecipeTableData::iterator itor = m_SecondarySkillRecipeTableData.find( iItemID );
	if( itor != m_SecondarySkillRecipeTableData.end() )
		return &(*itor).second;

	return NULL;
}

#endif // #if defined( PRE_ADD_SECONDARY_SKILL )

bool CDNGameDataManager::LoadInstantItemTableData()
{
#ifdef _WORK
	DNTableFileFormat* pSox;
	if (m_bAllLoaded)
		pSox = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TINSTANTITEM );
	else
		pSox = GetDNTable( CDnTableDB::TINSTANTITEM );
#else		//#ifdef _WORK
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TINSTANTITEM );
#endif		//#ifdef _WORK
	if( !pSox )
	{
		g_Log.Log( LogType::_FILELOG, L"instantitemtable.dnt failed\r\n" );
		return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		m_InstantItemTableData.clear();
	}
#endif		//#ifdef _WORK

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{
		int ItemID;
	};

	TempFieldNum sFieldNum;
	sFieldNum.ItemID = pSox->GetFieldNum( "_ItemID" );

	//##################################################################
	// Load
	//##################################################################

	for( int i=0 ; i<pSox->GetItemCount() ; ++i )
	{
		int nItemID = pSox->GetItemID(i);

		int iIdx = pSox->GetIDXprimary( nItemID );
		m_InstantItemTableData.insert( std::make_pair(pSox->GetFieldFromLablePtr( iIdx, sFieldNum.ItemID )->GetInteger(),1) );
	}

	return true;
}

bool CDNGameDataManager::bIsInstantItem( int iItemID )
{
	TMapInstantItemTableData::iterator itor = m_InstantItemTableData.find( iItemID );
	if( itor == m_InstantItemTableData.end() )
		return false;
	return true;
}

bool CDNGameDataManager::LoadFarmCultivateTableData()
{
#ifdef _WORK
	DNTableFileFormat* pSox;
	if (m_bAllLoaded)
		pSox = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TFARMCULTIVATE );
	else
		pSox = GetDNTable( CDnTableDB::TFARMCULTIVATE );
#else		//#ifdef _WORK
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TFARMCULTIVATE );
#endif		//#ifdef _WORK
	if( !pSox )
	{
		g_Log.Log( LogType::_FILELOG, L"farmcultivatetable.dnt failed\r\n" );
		return false;
	}

	if( pSox->GetItemCount() <= 0 )
	{
		g_Log.Log( LogType::_FILELOG, L"farmcultivatetable.dnt Count(%d)\r\n", pSox->GetItemCount() );
		return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		m_FarmCultivateTableData.clear();
	}
#endif		//#ifdef _WORK

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{
		int CultivateMaxTime;
		int MaxWater;
		int ConsumeWater;
		int HarvestItemID;
		int HarvestDrop[Farm::Max::HARVESTITEM_COUNT];
		int HarvestDropID;
		int SkillpointSeed;
		int SkillpointWater;
		int CatalystCount;
		int	RequiredSkillGrade;
		int RequiredSkillLevel;
		int Overlap;
	};

	TempFieldNum sFieldNum;
	
	sFieldNum.CultivateMaxTime		= pSox->GetFieldNum( "_CultivateMaxTime" );
	sFieldNum.MaxWater				= pSox->GetFieldNum( "_MaxWater" );
	sFieldNum.ConsumeWater			= pSox->GetFieldNum( "_ConsumeWater" );
	sFieldNum.HarvestItemID			= pSox->GetFieldNum( "_HarvestItemID" );
	sFieldNum.HarvestDropID			= pSox->GetFieldNum( "_HarvestDropID" );
	sFieldNum.SkillpointSeed		= pSox->GetFieldNum( "_SkillpointSeed" );
	sFieldNum.SkillpointWater		= pSox->GetFieldNum( "_SkillpointWater" );
	sFieldNum.CatalystCount			= pSox->GetFieldNum( "_CatalystCount" );
	sFieldNum.RequiredSkillGrade	= pSox->GetFieldNum( "_SecondarySkillClass" );
	sFieldNum.RequiredSkillLevel	= pSox->GetFieldNum( "_SecondarySkillLevel" );
	sFieldNum.Overlap				= pSox->GetFieldNum( "_Overlap" );

	for( int i=0 ; i<_countof(sFieldNum.HarvestDrop) ; ++i )
	{
		char szBuf[MAX_PATH];
		sprintf_s( szBuf, "_HarvestDrop%d", i+1 );
		sFieldNum.HarvestDrop[i] = pSox->GetFieldNum( szBuf );
	}

	//##################################################################
	// Load
	//##################################################################

	for( int i=0 ; i<pSox->GetItemCount() ; ++i )
	{
		TFarmCultivateTableData Data;

		int iItemID		= pSox->GetItemID(i);
		int iIdx		= pSox->GetIDXprimary( iItemID );

		Data.iCultivateMaxTimeSec	= pSox->GetFieldFromLablePtr( iIdx, sFieldNum.CultivateMaxTime )->GetInteger();
		Data.iMaxWater				= pSox->GetFieldFromLablePtr( iIdx, sFieldNum.MaxWater )->GetInteger();
		Data.iConsumeWater			= pSox->GetFieldFromLablePtr( iIdx, sFieldNum.ConsumeWater )->GetInteger();
		Data.iHarvestNeedItemID		= pSox->GetFieldFromLablePtr( iIdx, sFieldNum.HarvestItemID )->GetInteger();
		Data.iHarvestDropTableID	= pSox->GetFieldFromLablePtr( iIdx, sFieldNum.HarvestDropID )->GetInteger();
		Data.iSkillPointPlant		= pSox->GetFieldFromLablePtr( iIdx, sFieldNum.SkillpointSeed )->GetInteger();
		Data.iSkillPointWater		= pSox->GetFieldFromLablePtr( iIdx, sFieldNum.SkillpointWater )->GetInteger();
		Data.iCatalystCount			= pSox->GetFieldFromLablePtr( iIdx, sFieldNum.CatalystCount )->GetInteger();
		Data.RequiredSkillGrade		= static_cast<SecondarySkill::Grade::eType>(pSox->GetFieldFromLablePtr( iIdx, sFieldNum.RequiredSkillGrade )->GetInteger());
		Data.iRequiredSkillLevel	= pSox->GetFieldFromLablePtr( iIdx, sFieldNum.RequiredSkillLevel )->GetInteger();
		Data.iOverlap = pSox->GetFieldFromLablePtr( iIdx, sFieldNum.Overlap )->GetInteger();
		for( int i=0 ; i<_countof(sFieldNum.HarvestDrop) ; ++i )
		{
			Data.iHarvestDropRate[i] = pSox->GetFieldFromLablePtr( iIdx, sFieldNum.HarvestDrop[i] )->GetInteger();
		}

		m_FarmCultivateTableData.insert( std::make_pair(iItemID,Data) );
	}

	return true;	
}

TFarmCultivateTableData* CDNGameDataManager::GetFarmCultivateTableData( int iItemID )
{
	TMapFarmCultivateTableData::iterator itor = m_FarmCultivateTableData.find( iItemID );
	if( itor != m_FarmCultivateTableData.end() )
		return &(*itor).second;

	return NULL;
}

bool CDNGameDataManager::LoadFishingTableData()
{
#ifdef _WORK
	DNTableFileFormat* pSox;
	if (m_bAllLoaded)
		pSox = CDnTableDB::GetInstance().ReLoadSox(CDnTableDB::TFISHING);
	else
		pSox = GetDNTable(CDnTableDB::TFISHING);
#else		//#ifdef _WORK
	DNTableFileFormat* pSox = GetDNTable(CDnTableDB::TFISHING);
#endif		//#ifdef _WORK
	if( !pSox )
	{
		g_Log.Log( LogType::_FILELOG, L"fishingtable.dnt failed\r\n" );
		return false;
	}

	if( pSox->GetItemCount() <= 0 )
	{
		g_Log.Log( LogType::_FILELOG, L"fishingtable.dnt Count(%d)\r\n", pSox->GetItemCount() );
		return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		m_FishingTableData.clear();
	}
#endif		//#ifdef _WORK

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum : public TFishingTableData
	{
	};

	TempFieldNum sFieldNum;

	sFieldNum.nMaxTime = pSox->GetFieldNum( "_MaxTime" );
	sFieldNum.nPullingTargetMin	= pSox->GetFieldNum( "_TargetMin" );
	sFieldNum.nPullingTargetMax	= pSox->GetFieldNum( "_TargetMax" );
	sFieldNum.nPullingTargetSpeed = pSox->GetFieldNum( "_TargetSpeed" );
	sFieldNum.nPullingTargetUpTime = pSox->GetFieldNum( "_TargetUpTime" );
	sFieldNum.nSkillPointSuccess = pSox->GetFieldNum( "_GetSkillPointSuccess" );
	sFieldNum.nSkillPointFailure = pSox->GetFieldNum( "_GetSkillPointFailure" );
	sFieldNum.nSuccessProbability = pSox->GetFieldNum( "_SuccessProbability" );
	sFieldNum.nSuccessDropTableID = pSox->GetFieldNum( "_SuccessDrapID" );				//오탄데 일단 ㅋ

	//##################################################################
	// Load
	//##################################################################

	for( int i=0 ; i<pSox->GetItemCount() ; ++i )
	{
		TFishingTableData Data;
		memset(&Data, 0, sizeof(TFishingTableData));

		int iItemID		= pSox->GetItemID(i);
		int iIdx		= pSox->GetIDXprimary( iItemID );

		Data.nMaxTime = pSox->GetFieldFromLablePtr( iIdx, sFieldNum.nMaxTime )->GetInteger();
		Data.nPullingTargetMin = pSox->GetFieldFromLablePtr( iIdx, sFieldNum.nPullingTargetMin )->GetInteger();
		Data.nPullingTargetMax = pSox->GetFieldFromLablePtr( iIdx, sFieldNum.nPullingTargetMax )->GetInteger();
		Data.nPullingTargetSpeed = pSox->GetFieldFromLablePtr( iIdx, sFieldNum.nPullingTargetSpeed )->GetInteger();
		Data.nPullingTargetUpTime = pSox->GetFieldFromLablePtr( iIdx, sFieldNum.nPullingTargetUpTime )->GetInteger();
		Data.nSkillPointSuccess = pSox->GetFieldFromLablePtr( iIdx, sFieldNum.nSkillPointSuccess )->GetInteger();
		Data.nSkillPointFailure	= pSox->GetFieldFromLablePtr( iIdx, sFieldNum.nSkillPointFailure )->GetInteger();
		Data.nSuccessProbability = pSox->GetFieldFromLablePtr( iIdx, sFieldNum.nSuccessProbability )->GetInteger();
		Data.nSuccessDropTableID = pSox->GetFieldFromLablePtr( iIdx, sFieldNum.nSuccessDropTableID )->GetInteger();	

		if(m_FishingTableData.find(iItemID) != m_FishingTableData.end())
		{
			_ASSERT_EXPR(0, L"Check Fishing TableID Duplicated!");
			return false;
		}
		m_FishingTableData.insert( std::make_pair(iItemID,Data) );
	}
	return true;
}

TFishingTableData * CDNGameDataManager::GetFishingTableData(int nItemID)
{
	TMapFishingTableData::iterator ii = m_FishingTableData.find(nItemID);
	if(ii != m_FishingTableData.end())
		return &(*ii).second;
	return NULL;
}

bool CDNGameDataManager::LoadFishingPointTableData()
{
#ifdef _WORK
	DNTableFileFormat* pSox;
	if (m_bAllLoaded)
		pSox = CDnTableDB::GetInstance().ReLoadSox(CDnTableDB::TFISHINGPOINT);
	else
		pSox = GetDNTable(CDnTableDB::TFISHINGPOINT);
#else		//#ifdef _WORK
	DNTableFileFormat* pSox = GetDNTable(CDnTableDB::TFISHINGPOINT);
#endif		//#ifdef _WORK
	if( !pSox )
	{
		g_Log.Log( LogType::_FILELOG, L"fishingpointtable.dnt failed\r\n" );
		return false;
	}

	if( pSox->GetItemCount() <= 0 )
	{
		g_Log.Log( LogType::_FILELOG, L"fishingpointtable.dnt Count(%d)\r\n", pSox->GetItemCount() );
		return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		m_FishingPointTableData.clear();
	}
#endif		//#ifdef _WORK

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum : public TFishingPointTableData
	{
	};

	TempFieldNum sFieldNum;

	sFieldNum.nSecondarySkillClass	= pSox->GetFieldNum( "_SecondarySkillClass" );
	sFieldNum.nSecondarySkillLevel	= pSox->GetFieldNum( "_SecondarySkillLevel" );
	sFieldNum.nRequireItemType1	= pSox->GetFieldNum( "_NeedItemType1" );
	sFieldNum.nRequireItemType2	= pSox->GetFieldNum( "_NeedItemType2" );

	std::string strPattern = "_FishingPatternID";
	std::string strPatternProbabillity = "_Probability";
	std::string strPatternTemp;
	std::string strPatternProbabillityTemp;

	USES_CONVERSION;
	for (int i = 0; i < Fishing::Max::FISHINGPATTERNMAX; i++)
	{
		strPatternTemp.clear();
		strPatternProbabillityTemp.clear();

		strPatternTemp = strPattern + I2A(i+1);
		strPatternProbabillityTemp = strPatternProbabillity + I2A(i+1);
		sFieldNum.Pattern[i].nPatternID = pSox->GetFieldNum( strPatternTemp.c_str() );
		sFieldNum.Pattern[i].nProbabillity = pSox->GetFieldNum( strPatternProbabillityTemp.c_str() );
	}

	//##################################################################
	// Load
	//##################################################################

	for( int i=0 ; i<pSox->GetItemCount() ; ++i )
	{
		TFishingPointTableData Data;

		int iItemID		= pSox->GetItemID(i);
		int iIdx		= pSox->GetIDXprimary( iItemID );

		Data.nSecondarySkillClass = pSox->GetFieldFromLablePtr( iIdx, sFieldNum.nSecondarySkillClass )->GetInteger();
		Data.nSecondarySkillLevel = pSox->GetFieldFromLablePtr( iIdx, sFieldNum.nSecondarySkillLevel )->GetInteger();
		Data.nRequireItemType1 = pSox->GetFieldFromLablePtr( iIdx, sFieldNum.nRequireItemType1 )->GetInteger();
		Data.nRequireItemType2 = pSox->GetFieldFromLablePtr( iIdx, sFieldNum.nRequireItemType2 )->GetInteger();
		for (int j = 0; j < Fishing::Max::FISHINGPATTERNMAX; j++)
		{
			Data.Pattern[j].nPatternID = pSox->GetFieldFromLablePtr( iIdx, sFieldNum.Pattern[j].nPatternID )->GetInteger();
			Data.Pattern[j].nProbabillity = pSox->GetFieldFromLablePtr( iIdx, sFieldNum.Pattern[j].nProbabillity )->GetInteger();
		}
		
		if(m_FishingPointTableData.find(iItemID) != m_FishingPointTableData.end())
		{
			_ASSERT_EXPR(0, L"Check Fishing TableID Duplicated!");
			return false;
		}
		m_FishingPointTableData.insert( std::make_pair(iItemID,Data) );
	}
	return true;
}

TFishingPointTableData * CDNGameDataManager::GetFishingPointTableData(int nItemID)
{
	TMapFishingPointTableData::iterator ii = m_FishingPointTableData.find(nItemID);
	if(ii != m_FishingPointTableData.end())
		return &(*ii).second;
	return NULL;
}

bool CDNGameDataManager::LoadFishingAreaTableData()
{
#ifdef _WORK
	DNTableFileFormat* pSox;
	if (m_bAllLoaded)
		pSox = CDnTableDB::GetInstance().ReLoadSox(CDnTableDB::TFISHINGAREA);
	else
		pSox = GetDNTable(CDnTableDB::TFISHINGAREA);
#else		//#ifdef _WORK
	DNTableFileFormat* pSox = GetDNTable(CDnTableDB::TFISHINGAREA);
#endif		//#ifdef _WORK
	if( !pSox )
	{
		g_Log.Log( LogType::_FILELOG, L"fishingareatable.dnt failed\r\n" );
		return false;
	}

	if( pSox->GetItemCount() <= 0 )
	{
		g_Log.Log( LogType::_FILELOG, L"fishingareatable.dnt Count(%d)\r\n", pSox->GetItemCount() );
		return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		m_FishingAreaTableData.clear();
	}
#endif		//#ifdef _WORK

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum : public TFishingAreaTableData
	{
	};

	TempFieldNum sFieldNum;

	sFieldNum.nMatchedMapID	= pSox->GetFieldNum( "_MapID" );
	sFieldNum.nMatchedFishingAreaID	= pSox->GetFieldNum( "_EventAreaID" );
	sFieldNum.nFishingPointID	= pSox->GetFieldNum( "_FishingPointID" );

	//##################################################################
	// Load
	//##################################################################

	for( int i=0 ; i<pSox->GetItemCount() ; ++i )
	{
		TFishingAreaTableData Data;

		int iItemID		= pSox->GetItemID(i);
		int iIdx		= pSox->GetIDXprimary( iItemID );

		Data.nMatchedMapID = pSox->GetFieldFromLablePtr( iIdx, sFieldNum.nMatchedMapID )->GetInteger();
		Data.nMatchedFishingAreaID = pSox->GetFieldFromLablePtr( iIdx, sFieldNum.nMatchedFishingAreaID )->GetInteger();
		Data.nFishingPointID = pSox->GetFieldFromLablePtr( iIdx, sFieldNum.nFishingPointID )->GetInteger();
		
		if(m_FishingAreaTableData.find(iItemID) != m_FishingAreaTableData.end())
		{
			_ASSERT_EXPR(0, L"Check Fishing TableID Duplicated!");
			return false;
		}
		m_FishingAreaTableData.insert( std::make_pair(iItemID,Data) );
	}
	return true;
}

TFishingAreaTableData * CDNGameDataManager::GetFishingAreaTableData(int nItemID)
{
	TMapFishingAreaTableData::iterator ii = m_FishingAreaTableData.find(nItemID);
	if(ii != m_FishingAreaTableData.end())
		return &(*ii).second;
	return NULL;
}

void CDNGameDataManager::GetFishingAreaTableDataByMapID(int nMapID, std::vector <TFishingAreaTableData> &vFishingArea)
{
	TMapFishingAreaTableData::iterator ii;
	for(ii = m_FishingAreaTableData.begin(); ii != m_FishingAreaTableData.end(); ii++)
	{
		if((*ii).second.nMatchedMapID == nMapID)
			vFishingArea.push_back((*ii).second);
	}
}


bool CDNGameDataManager::LoadJobTableData()
{
#ifdef _WORK
	DNTableFileFormat* pSox;
	if (m_bAllLoaded)
		pSox = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TJOB );
	else
		pSox = GetDNTable( CDnTableDB::TJOB );
#else		//#ifdef _WORK
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TJOB );
#endif		//#ifdef _WORK
	if( !pSox )
	{
		g_Log.Log( LogType::_FILELOG, L"JobTable.dnt failed\r\n" );
		return false;
	}

	if( pSox->GetItemCount() <= 0 )
	{
		g_Log.Log( LogType::_FILELOG, L"JobTable.dnt Count(%d)\r\n", pSox->GetItemCount() );
		return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		m_JobTableData.clear();
	}
#endif		//#ifdef _WORK

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{
		int _JobNumber;
		int _BaseClass;
		int _ParentJob;
		int _MaxSPJob[ 5 ];
		int _Class;
	};

	TempFieldNum sFieldNum;
	sFieldNum._JobNumber	= pSox->GetFieldNum( "_JobNumber" );
	sFieldNum._BaseClass	= pSox->GetFieldNum( "_BaseClass" );
	sFieldNum._ParentJob	= pSox->GetFieldNum( "_ParentJob" );
	
	char acBuffer[ 256 ] = { 0 };
	for( int i = 0; i < 5; ++i )
	{
		sprintf_s( acBuffer, "_MaxSPJob%d", i );
		sFieldNum._MaxSPJob[ i ] = pSox->GetFieldNum( acBuffer );
	}

	sFieldNum._Class	= pSox->GetFieldNum( "_Class" );

	//##################################################################
	// Load
	//##################################################################
	for( int i=0 ; i<pSox->GetItemCount() ; ++i )
	{
		TJobTableData Data;
		int nItemID = pSox->GetItemID(i);
		int iIdx	= pSox->GetIDXprimary( nItemID );

		Data.cJobNumber	= pSox->GetFieldFromLablePtr( iIdx, sFieldNum._JobNumber )->GetInteger();
		Data.cBaseClass = pSox->GetFieldFromLablePtr( iIdx, sFieldNum._BaseClass )->GetInteger();
		Data.cParentJob	= pSox->GetFieldFromLablePtr( iIdx, sFieldNum._ParentJob )->GetInteger();
		for( int k = 0; k < 5; ++k )
			Data.afMaxUsingSP[ k ] = pSox->GetFieldFromLablePtr( iIdx, sFieldNum._MaxSPJob[ k ] )->GetFloat();

		Data.cClass = pSox->GetFieldFromLablePtr( iIdx, sFieldNum._Class )->GetInteger();

		m_JobTableData.insert( std::make_pair(nItemID, Data) );

		if( nItemID >= JOB_KIND_MAX )
		{
			g_Log.Log( LogType::_FILELOG, L"MaxJob OverFlow %d/%d\r\n", nItemID, JOB_KIND_MAX );
			return false;
		}
	}

	return true;
}

TJobTableData* CDNGameDataManager::GetJobTableData( int nJobTableID )
{
	TMapJobTableData::iterator itor = m_JobTableData.find( nJobTableID );
	if( itor != m_JobTableData.end() )
	{
		TJobTableData * pTJobTableData = &(itor->second);
		if( pTJobTableData->cJobNumber >= 0 )		// 기본직업은 차수가 0 부터 시작이라 수정.
			return pTJobTableData;
	}
	
	return NULL;
};

int CDNGameDataManager::GetBaseClass( int iJob )
{
	int iResult = -1;

	TMapJobTableData::iterator itor = m_JobTableData.find( iJob );
	if( itor != m_JobTableData.end() )
	{
		TJobTableData * pTJobTableData = &(itor->second);
		iResult = pTJobTableData->cBaseClass;
	}
	
	return iResult;
}

int CDNGameDataManager::GetParentJob( int iJob )
{
	TMapJobTableData::iterator itor = m_JobTableData.find( iJob );
	if( itor != m_JobTableData.end() )
	{
		TJobTableData * pTJobTableData = &(itor->second);
		if( pTJobTableData->cJobNumber > 0 )		// 부모직업을 얻어오는 직업은 루트 직업이 될 수 없으므로 0 보다 큰 직업 아이디만 허용.
			return pTJobTableData->cParentJob;
	}
	return iJob;
}

int CDNGameDataManager::GetJobNumber( int iJob )
{
	TMapJobTableData::iterator itor = m_JobTableData.find( iJob );
	if( itor != m_JobTableData.end() )
	{
		TJobTableData * pTJobTableData = &(itor->second);
		if( pTJobTableData->cJobNumber >= 0 ) // 기본직업은 차수가 0 부터 시작이라 수정.
			return pTJobTableData->cJobNumber;
	}
	return 0;
}

int CDNGameDataManager::GetClass( int iJob )
{
	TMapJobTableData::iterator itor = m_JobTableData.find( iJob );
	if( itor != m_JobTableData.end() )
	{
		TJobTableData * pTJobTableData = &(itor->second);
		if( pTJobTableData->cJobNumber > 0 )
			return pTJobTableData->cClass;
	}

	return 0;
}

void CDNGameDataManager::GetJobGroupID(int nItemID, char cJobCode[])
{
	int nJobNumber = GetJobNumber(nItemID);
	if(nJobNumber >= ITEMJOB_MAX)
		nJobNumber = ITEMJOB_MAX;

	if(nJobNumber == 0)
	{
		cJobCode[0] = nItemID;
	}
	else
	{
		int i = 0;
		int nParentJob = nItemID;
		for(i=0; i<nJobNumber; i++)
		{
			cJobCode[i] = nParentJob;
			nParentJob = GetParentJob(nParentJob);
		}
		cJobCode[i] = nParentJob;
	}
}

// GlyphSkill
bool CDNGameDataManager::LoadGlyphSkillData()
{
#ifdef _WORK
	DNTableFileFormat* pSox;
	if (m_bAllLoaded)
		pSox = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TGLYPHSKILL );
	else
		pSox = GetDNTable( CDnTableDB::TGLYPHSKILL );
#else		//#ifdef _WORK
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TGLYPHSKILL );
#endif		//#ifdef _WORK
	if( !pSox )
	{
		g_Log.Log( LogType::_FILELOG, L"GlyphSkillTable.dnt failed\r\n" );
		return false;
	}

	if( pSox->GetItemCount() <= 0 )
	{
		g_Log.Log( LogType::_FILELOG, L"GlyphSkillTable.dnt Count(%d)\r\n", pSox->GetItemCount() );
		return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		m_MapGlyphSkill.clear();
	}
#endif		//#ifdef _WORK

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{
		int _SkillLevelID;	// 스킬레벨테이블ID
		int _GlyphType;		// 문장 타입
		int _SkillID;		// 스킬테이블ID
	};

	TempFieldNum sFieldNum;
	sFieldNum._SkillLevelID = pSox->GetFieldNum( "_SkillLevelID" );
	sFieldNum._GlyphType = pSox->GetFieldNum( "_GlyphType" );
	sFieldNum._SkillID = pSox->GetFieldNum( "_SkillID" );


	//##################################################################
	// Load
	//##################################################################


	for (int i = 0; i < pSox->GetItemCount(); ++i)
	{
		TGlyphSkillData Data = {0, };
		int nItemID = pSox->GetItemID(i);
		int nIdx	= pSox->GetIDXprimary( nItemID );

		Data.nGlyphID = nItemID;
		Data.nSkillLevelID = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._SkillLevelID)->GetInteger();
		Data.nGlyphType = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._GlyphType)->GetInteger();
		Data.nSkillID = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._SkillID)->GetInteger();

		m_MapGlyphSkill.insert(make_pair(Data.nGlyphID, Data));
	}

	return true;
}

TGlyphSkillData *CDNGameDataManager::GetGlyphSkillData(int nGlyphID)
{
	if(m_MapGlyphSkill.empty()) return NULL;

	TMapGlyphSkillData::iterator iter = m_MapGlyphSkill.find(nGlyphID);
	if(iter == m_MapGlyphSkill.end()) return NULL;

	return &(iter->second);
}

int CDNGameDataManager::GetGlyphType(int nGlyphID)
{
	if(m_MapGlyphSkill.empty()) return 0;

	TGlyphSkillData *pGlyph = GetGlyphSkillData(nGlyphID);
	if(!pGlyph) return 0;

	return pGlyph->nGlyphType;
}

// GlyphSlot
bool CDNGameDataManager::LoadGlyphSlotData()
{
#ifdef _WORK
	DNTableFileFormat* pSox;
	if (m_bAllLoaded)
		pSox = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TGLYPHSLOT );
	else
		pSox = GetDNTable( CDnTableDB::TGLYPHSLOT );
#else		//#ifdef _WORK
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TGLYPHSLOT );
#endif		//#ifdef _WORK
	if( !pSox )
	{
		g_Log.Log( LogType::_FILELOG, L"GlyphSlotTable.dnt failed\r\n" );
		return false;
	}

	if( pSox->GetItemCount() <= 0 )
	{
		g_Log.Log( LogType::_FILELOG, L"GlyphSlotTable.dnt Count(%d)\r\n", pSox->GetItemCount() );
		return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		m_VecGlyphSlot.clear();
	}
#endif		//#ifdef _WORK

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{
		int nSlotID;
		int nLevelLimit;
		int nSlotType;
		int nIsCash;
	};

	TempFieldNum sFieldNum;
	sFieldNum.nSlotID = pSox->GetFieldNum( "_slotID" );
	sFieldNum.nLevelLimit = pSox->GetFieldNum( "_LevelLimit" );
	sFieldNum.nSlotType = pSox->GetFieldNum( "_slotType" );
	sFieldNum.nIsCash = pSox->GetFieldNum( "_isCash" );


	//##################################################################
	// Load
	//##################################################################


	for (int i = 0; i < pSox->GetItemCount(); ++i)
	{
		TGlyphSlotData Data = {0, };
		int nItemID = pSox->GetItemID(i);
		int nIdx	= pSox->GetIDXprimary( nItemID );

		Data.nID = nItemID;
		Data.cSlotID = pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nSlotID)->GetInteger();
		Data.cSlotType = pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nSlotType)->GetInteger();
		Data.cLevelLimit = pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nLevelLimit)->GetInteger();
		Data.IsCash = pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nIsCash)->GetInteger() ? true : false;

		m_VecGlyphSlot.push_back(Data);
	}

	return true;
}

int CDNGameDataManager::GLYPHINDEX_2_SLOTINDEX(int nGlyphIndex)
{
	// 0, 1, 2, 4, 3, 5, 6, 7, 8, 9
	// 6, 7, 5, 8, 12, 9, 11, 10
	switch(nGlyphIndex)
	{
		case GLYPH_ENCHANT1: return 5;
		case GLYPH_ENCHANT2: return 6;
		case GLYPH_ENCHANT3: return 7;
		case GLYPH_ENCHANT4: return 8;
		case GLYPH_ENCHANT5: return 9;
		case GLYPH_ENCHANT6: return 10;
		case GLYPH_ENCHANT7: return 11;
		case GLYPH_ENCHANT8: return 12;
		case GLYPH_SKILL1:	 return 1;
		case GLYPH_SKILL2:	 return 2;
		case GLYPH_SKILL3:	 return 4;
		case GLYPH_SKILL4:	 return 3;
		case GLYPH_SPECIALSKILL: return 0;
		case GLYPH_CASH1: return 13;
		case GLYPH_CASH2: return 14;
		case GLYPH_CASH3: return 15;
#if defined(PRE_ADD_DRAGON_FELLOWSHIP_GLYPH)
		case GLYPH_DRAGON1: return 16;
		case GLYPH_DRAGON2: return 17;
		case GLYPH_DRAGON3: return 18;
		case GLYPH_DRAGON4: return 19;
#endif //#if defined(PRE_ADD_DRAGON_FELLOWSHIP_GLYPH)GLYPH_DRAGON1
	}
	return 0;
}

bool CDNGameDataManager::IsGlyphSlotLevel(int nGlyphIndex, int nCharLevel)
{
#if defined(PRE_ADD_DRAGON_FELLOWSHIP_GLYPH)
	if ((nGlyphIndex < GLYPH_ENCHANT1) ||(nGlyphIndex > GLYPH_DRAGON4)) return false;
#else
	if ((nGlyphIndex < GLYPH_ENCHANT1) ||(nGlyphIndex > GLYPH_CASH3)) return false;
#endif
	if ((nCharLevel <= 0) ||(nCharLevel > CHARLEVELMAX)) return false;

	int nSlot = GLYPHINDEX_2_SLOTINDEX(nGlyphIndex);

	for (int i = 0; i < (int)m_VecGlyphSlot.size(); i++){
		if (m_VecGlyphSlot[i].cSlotID != nSlot) continue;
		if (m_VecGlyphSlot[i].cLevelLimit > nCharLevel) return false;
	}

	return true;
}

bool CDNGameDataManager::IsGlyphLevel(int nGlyphID, int nCharLevel)
{
	if (nGlyphID <= 0) return false;

	TItemData *pItemData = GetItemData(nGlyphID);
	if (!pItemData) return false;

	if (pItemData->cLevelLimit > nCharLevel)
		return false;

	return true;
}

// GlyphCharge
bool CDNGameDataManager::LoadGlyphChargeData()
{
#ifdef _WORK
	DNTableFileFormat* pSox;
	if (m_bAllLoaded)
		pSox = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TGLYPHCHARGE );
	else
		pSox = GetDNTable( CDnTableDB::TGLYPHCHARGE );
#else		//#ifdef _WORK
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TGLYPHCHARGE );
#endif		//#ifdef _WORK
	if( !pSox )
	{
		g_Log.Log( LogType::_FILELOG, L"GlyphChargeTable.dnt failed\r\n" );
		return false;
	}

	if( pSox->GetItemCount() <= 0 )
	{
		g_Log.Log( LogType::_FILELOG, L"GlyphChargeTable.dnt Count(%d)\r\n", pSox->GetItemCount() );
		return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		m_VecGlyphCharge.clear();
	}
#endif		//#ifdef _WORK

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{
		int nGlyphType;
		int nRank;
		int nCharge;
	};

	TempFieldNum sFieldNum;
	sFieldNum.nGlyphType = pSox->GetFieldNum( "_Glyph_type" );
	sFieldNum.nRank = pSox->GetFieldNum( "_Rank" );
	sFieldNum.nCharge = pSox->GetFieldNum( "_Charge" );


	//##################################################################
	// Load
	//##################################################################


	for (int i = 0; i < pSox->GetItemCount(); ++i)
	{
		TGlyphChargeData Data = {0, };
		int nItemID = pSox->GetItemID(i);
		int nIdx	= pSox->GetIDXprimary( nItemID );

		Data.cGlyphType= pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nGlyphType)->GetInteger();
		Data.cGlyphRank = pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nRank)->GetInteger();
		Data.nCharge = pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nCharge)->GetInteger();

		m_VecGlyphCharge.push_back(Data);
	}

	return true;
}

int CDNGameDataManager::GetGlyphCharge(char cGlyphType, char cGlyphRank)
{
	if (m_VecGlyphCharge.empty()) return 0;
	if (cGlyphType <= 0) return 0;
	if (cGlyphRank <= 0) return 0;

	for (int i = 0; i < (int)m_VecGlyphCharge.size(); i++){
		if ((m_VecGlyphCharge[i].cGlyphType == cGlyphType) && (m_VecGlyphCharge[i].cGlyphRank == cGlyphRank))
			return m_VecGlyphCharge[i].nCharge;
	}

	return 0;		
}

// CharmItemKeyTable
bool CDNGameDataManager::LoadCharmItemKeyData()
{
#ifdef _WORK
	DNTableFileFormat* pSox;
	if (m_bAllLoaded)
		pSox = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TCHARMITEMKEY );
	else
		pSox = GetDNTable( CDnTableDB::TCHARMITEMKEY );
#else		//#ifdef _WORK
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TCHARMITEMKEY );
#endif		//#ifdef _WORK
	if( !pSox )
	{
		g_Log.Log( LogType::_FILELOG, L"CharmItemKeyTable.dnt failed\r\n" );
		return false;
	}

	if( pSox->GetItemCount() <= 0 )
	{
		g_Log.Log( LogType::_FILELOG, L"CharmItemKeyTable.dnt Count(%d)\r\n", pSox->GetItemCount() );
		// return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		m_MapCharmKeyItem.clear();
	}
#endif		//#ifdef _WORK

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{
		int _CharmItemID;	// 상자 ItemID
		int _Key[CharmItemKeyMax];	// 열쇠ID
	};

	TempFieldNum sFieldNum;
	sFieldNum._CharmItemID = pSox->GetFieldNum("_CharmItemID");
	sFieldNum._Key[0] = pSox->GetFieldNum("_Key1");
	sFieldNum._Key[1] = pSox->GetFieldNum("_Key2");
	sFieldNum._Key[2] = pSox->GetFieldNum("_Key3");
	sFieldNum._Key[3] = pSox->GetFieldNum("_Key4");
	sFieldNum._Key[4] = pSox->GetFieldNum("_Key5");

	//##################################################################
	// Load
	//##################################################################

	for (int i = 0; i < pSox->GetItemCount(); ++i)
	{
		int nItemID = pSox->GetItemID(i);
		int nIdx = pSox->GetIDXprimary(nItemID);

		TCharmItemKeyData Data = {0, };
		Data.nCharmItemID = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._CharmItemID)->GetInteger();
		if (Data.nCharmItemID <= 0) continue;
		for (int j = 0; j < 5; j++){
			int nKey = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._Key[j])->GetInteger();
			if (nKey > 0)
				Data.nKeyList.push_back(nKey);
		}

		m_MapCharmKeyItem.insert(make_pair(Data.nCharmItemID, Data));
	}

	return true;
}

TCharmItemKeyData *CDNGameDataManager::GetCharmKeyData(int nItemID)
{
	if (m_MapCharmKeyItem.empty()) return NULL;

	TMapCharmKeyItem::iterator iter = m_MapCharmKeyItem.find(nItemID);
	if (iter == m_MapCharmKeyItem.end()) return NULL;

	return &(iter->second);
}

// CharmItemTable
bool CDNGameDataManager::LoadCharmItemData()
{
#ifdef _WORK
	DNTableFileFormat* pSox;
	if (m_bAllLoaded)
		pSox = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TCHARMITEM );
	else
		pSox = GetDNTable( CDnTableDB::TCHARMITEM );
#else		//#ifdef _WORK
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TCHARMITEM );
#endif		//#ifdef _WORK
	if( !pSox )
	{
		g_Log.Log( LogType::_FILELOG, L"CharmItemTable.dnt failed\r\n" );
		return false;
	}

	if( pSox->GetItemCount() <= 0 )
	{
		g_Log.Log( LogType::_FILELOG, L"CharmItemTable.dnt Count(%d)\r\n", pSox->GetItemCount() );
		// return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		m_MapCharmItem.clear();
	}
#endif		//#ifdef _WORK

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{
		int _CharmNum;	// 매력아이템 번호
		int _Count;		// 유저가 획득할 개수
		int _Prob;		// 확률
		int _Period;	// 기간
		int _ItemID;	// 아이템ID
		int _Gold;		// 골드
		int _Msg;		// 메시지 보낼지 여부
	};

	TempFieldNum sFieldNum;
	sFieldNum._CharmNum = pSox->GetFieldNum("_CharmNum");
	sFieldNum._Count = pSox->GetFieldNum("_Count");
	sFieldNum._Prob = pSox->GetFieldNum("_Prob");
	sFieldNum._Period = pSox->GetFieldNum("_Period");
	sFieldNum._ItemID = pSox->GetFieldNum("_ItemID");
	sFieldNum._Msg = pSox->GetFieldNum("_Msg");
	sFieldNum._Gold = pSox->GetFieldNum("_Gold");

	int nNeedEmptyCoinAmount = (int)CGlobalWeightIntTable::GetInstance().GetValue(CGlobalWeightIntTable::CharmItemEmptyCoinAmount);

	//##################################################################
	// Load
	//##################################################################

	for (int i = 0; i < pSox->GetItemCount(); ++i)
	{
		int nItemID = pSox->GetItemID(i);
		int nIdx = pSox->GetIDXprimary(nItemID);

		int nCharmNo = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._CharmNum)->GetInteger();
		if (nCharmNo <= 0) continue;

		TCharmItem Item = {0, };
#if defined(PRE_FIX_68828)
		Item.nCharmID = nItemID;
#endif
		Item.nItemID = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._ItemID)->GetInteger();
		Item.nCount = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._Count)->GetInteger();
		Item.nPeriod = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._Period)->GetInteger();
		Item.nProb = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._Prob)->GetInteger();
		Item.bMsg = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._Msg)->GetInteger() ? true : false;
		Item.nGold = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._Gold)->GetInteger();

		if (!Item.nItemID || !Item.nCount)
		{
			g_Log.Log( LogType::_FILELOG, L"CharmItemTable.dnt ItemID[%d] Invalid Data \r\n", nItemID );
			return false;
		}

		if (Item.nGold > 0 && Item.nGold > nNeedEmptyCoinAmount)
		{
			g_Log.Log( LogType::_FILELOG, L"CharmItemTable.dnt ItemID[%d] Gold OverFlow MaxCoin[%d] \r\n", nItemID, nNeedEmptyCoinAmount );
			return false;
		}
		m_MapCharmItem[nCharmNo].CharmItemList.push_back(Item);
		m_MapCharmItem[nCharmNo].nCharmNo = nCharmNo;
	}

	return true;
}

TCharmItemData *CDNGameDataManager::GetCharmItemData(int nCharmNo)
{
	if (nCharmNo <= 0) return NULL;

	TMapCharmItem::iterator iter = m_MapCharmItem.find(nCharmNo);
	if (iter == m_MapCharmItem.end()) return NULL;

	return &(iter->second);
}

#if defined(PRE_ADD_GETITEM_WITH_RANDOMVALUE)
// CharmCountTable
bool CDNGameDataManager::LoadCharmCountData()
{
#ifdef _WORK
	DNTableFileFormat* pSox;
	if (m_bAllLoaded)
		pSox = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TCHARMCOUNT );
	else
		pSox = GetDNTable( CDnTableDB::TCHARMCOUNT );
#else		//#ifdef _WORK
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TCHARMCOUNT );
#endif		//#ifdef _WORK
	if( !pSox )
	{
		g_Log.Log( LogType::_FILELOG, L"charmcounttable.dnt failed\r\n" );
		return false;
	}

	if( pSox->GetItemCount() <= 0 )
	{
		g_Log.Log( LogType::_FILELOG, L"charmcounttable.dnt Count(%d)\r\n", pSox->GetItemCount() );
		// return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		m_MapCharmCount.clear();
	}
#endif		//#ifdef _WORK

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{
		int _CharmCount_Min;
		int _CharmCount_Max;
	};

	TempFieldNum sFieldNum;
	sFieldNum._CharmCount_Min = pSox->GetFieldNum("_CharmCount_Min");
	sFieldNum._CharmCount_Max = pSox->GetFieldNum("_CharmCount_Max");

	//##################################################################
	// Load
	//##################################################################

	for (int i = 0; i < pSox->GetItemCount(); ++i)
	{
		int nItemID = pSox->GetItemID(i);
		int nIdx = pSox->GetIDXprimary(nItemID);

		TCharmCountData Data = {0, };
		Data.nID = nItemID;
		Data.nMin = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._CharmCount_Min)->GetInteger();
		Data.nMax = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._CharmCount_Max)->GetInteger();

		m_MapCharmCount.insert(make_pair(Data.nID, Data));
	}

	return true;
}

TCharmCountData *CDNGameDataManager::GetCharmCountData(int nID)
{
	TMapCharmCount::iterator iter = m_MapCharmCount.find(nID);
	if (iter == m_MapCharmCount.end()) return NULL;

	return &(iter->second);
}
#endif	// #if defined(PRE_ADD_GETITEM_WITH_RANDOMVALUE)

#if defined (PRE_ADD_CHAOSCUBE)
// ChaosCubeStuffTable
bool CDNGameDataManager::LoadChaosCubeStuffData()
{
#ifdef _WORK
	DNTableFileFormat* pSox;
	if (m_bAllLoaded)
		pSox = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TCHAOSCUBESTUFF );
	else
		pSox = GetDNTable( CDnTableDB::TCHAOSCUBESTUFF );
#else		//#ifdef _WORK
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TCHAOSCUBESTUFF );
#endif		//#ifdef _WORK
	if( !pSox )
	{
		g_Log.Log( LogType::_FILELOG, L"chaoscubestufftable.dnt failed\r\n" );
		return false;
	}

	if( pSox->GetItemCount() <= 0 )
	{
		g_Log.Log( LogType::_FILELOG, L"chaoscubestufftable.dnt Count(%d)\r\n", pSox->GetItemCount() );
		// return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		m_MapChaosStuffItem.clear();
	}
#endif		//#ifdef _WORK

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{
		int _ChaosNum;	// 매력아이템 번호
		int _Count;		// 
		int _ItemID;	// 아이템ID
	};

	TempFieldNum sFieldNum;
	sFieldNum._ChaosNum = pSox->GetFieldNum("_ChaoscubeNum");
	sFieldNum._Count = pSox->GetFieldNum("_Count");
	sFieldNum._ItemID = pSox->GetFieldNum("_ItemID");
	
	
	//##################################################################
	// Load
	//##################################################################

	for (int i = 0; i < pSox->GetItemCount(); ++i)
	{
		int nItemID = pSox->GetItemID(i);
		int nIdx = pSox->GetIDXprimary(nItemID);

		int nChaosNo = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._ChaosNum)->GetInteger();
		if (nChaosNo <= 0) continue;

		TChaosStuffItem Item = {0, };
		Item.nItemID = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._ItemID)->GetInteger();
		Item.nCount = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._Count)->GetInteger();
		
		m_MapChaosStuffItem[Item.nItemID] = Item;
	}

	return true;
}

TChaosStuffItem* CDNGameDataManager::GetChaosStuffItemData(int nItemID)
{
	if (nItemID <= 0) return NULL;

	TMapChaosStuffItem::iterator iter = m_MapChaosStuffItem.find(nItemID);
	if (iter == m_MapChaosStuffItem.end()) return NULL;

	return &(iter->second);
}

// ChaosCubeResultTable
bool CDNGameDataManager::LoadChaosCubeResultData()
{
#ifdef _WORK
	DNTableFileFormat* pSox;
	if (m_bAllLoaded)
		pSox = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TCHAOSCUBERESULT );
	else
		pSox = GetDNTable( CDnTableDB::TCHAOSCUBERESULT );
#else		//#ifdef _WORK
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TCHAOSCUBERESULT );
#endif		//#ifdef _WORK
	if( !pSox )
	{
		g_Log.Log( LogType::_FILELOG, L"chaoscuberesulttable.dnt failed\r\n" );
		return false;
	}

	if( pSox->GetItemCount() <= 0 )
	{
		g_Log.Log( LogType::_FILELOG, L"chaoscuberesulttable.dnt Count(%d)\r\n", pSox->GetItemCount() );
		// return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		m_MapChaosItem.clear();
	}
#endif		//#ifdef _WORK

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{
		int _ChaosNum;	// 매력아이템 번호
		int _Count;		// 유저가 획득할 개수
		int _Prob;		// 확률
		int _Period;	// 기간
		int _ItemID;	// 아이템ID
		int _Gold;		// 골드
		int _Msg;		// 메시지 보낼지 여부
	};

	TempFieldNum sFieldNum;
	sFieldNum._ChaosNum = pSox->GetFieldNum("_ChaoscubeNum");
	sFieldNum._Count = pSox->GetFieldNum("_Count");
	sFieldNum._Prob = pSox->GetFieldNum("_Prob");
	sFieldNum._Period = pSox->GetFieldNum("_Period");
	sFieldNum._ItemID = pSox->GetFieldNum("_ItemID");
	sFieldNum._Msg = pSox->GetFieldNum("_Msg");
	sFieldNum._Gold = pSox->GetFieldNum("_Gold");

	int nNeedEmptyCoinAmount = (int)CGlobalWeightIntTable::GetInstance().GetValue(CGlobalWeightIntTable::CharmItemEmptyCoinAmount);


	//##################################################################
	// Load
	//##################################################################

	for (int i = 0; i < pSox->GetItemCount(); ++i)
	{
		int nItemID = pSox->GetItemID(i);
		int nIdx = pSox->GetIDXprimary(nItemID);

		int nChaosNo = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._ChaosNum)->GetInteger();
		if (nChaosNo <= 0) continue;

		TChaosResultItem Item = {0, };
		Item.nItemID = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._ItemID)->GetInteger();
		Item.nCount = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._Count)->GetInteger();
		Item.nPeriod = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._Period)->GetInteger();
		Item.nProb = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._Prob)->GetInteger();
		Item.bMsg = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._Msg)->GetInteger() ? true : false;
		Item.nGold = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._Gold)->GetInteger();

		if (!Item.nItemID || !Item.nCount)
		{
			g_Log.Log( LogType::_FILELOG, L"chaoscuberesulttable.dnt ItemID[%d] Invalid Data \r\n", nItemID );
			return false;
		}

		if (Item.nGold > 0 && Item.nGold > nNeedEmptyCoinAmount)
		{
			g_Log.Log( LogType::_FILELOG, L"chaoscuberesulttable.dnt ItemID[%d] Gold OverFlow MaxCoin[%d] \r\n", nItemID, nNeedEmptyCoinAmount );
			return false;
		}

		m_MapChaosItem[nChaosNo].ChaosItemList.push_back(Item);
		m_MapChaosItem[nChaosNo].nChaosNo = nChaosNo;
	}

	return true;
}

TChaosItemData* CDNGameDataManager::GetChaosItemData(int nChaosNo)
{
	if (nChaosNo <= 0) return NULL;

	TMapChaosItem::iterator iter = m_MapChaosItem.find(nChaosNo);
	if (iter == m_MapChaosItem.end()) return NULL;

	return &(iter->second);
}
#endif

bool CDNGameDataManager::LoadGuildLevelData()
{
#ifdef _WORK
	DNTableFileFormat* pSox;
	if (m_bAllLoaded)
		pSox = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TGUILDLEVEL );
	else
		pSox = GetDNTable( CDnTableDB::TGUILDLEVEL );
#else		//#ifdef _WORK
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TGUILDLEVEL );
#endif		//#ifdef _WORK

	if( !pSox )
	{
		g_Log.Log( LogType::_FILELOG, L"guildleveltable.dnt failed\r\n" );
		return false;
	}

	if( pSox->GetItemCount() <= 0 )
	{		
		g_Log.Log( LogType::_FILELOG, L"guildleveltable.dnt Count(%d)\r\n", pSox->GetItemCount() );
		return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		m_GuildLevel.clear();
	}
#endif		//#ifdef _WORK

	//##################################################################
	// FieldNum 미리색인
	//##################################################################

	struct TempFieldNum
	{
		int		_GuildLevel;
		int		_ReqGold;
		int		_ReqGuildPoint;
		int		_DailyGuildPointLimit;
		int		_DailyMissionPointLimit;
		int		_IsApplicable;
	};

	TempFieldNum sFieldNum;
	sFieldNum._GuildLevel		= pSox->GetFieldNum("_GuildLevel");
	sFieldNum._ReqGold			= pSox->GetFieldNum("_ReqGold");
	sFieldNum._ReqGuildPoint	= pSox->GetFieldNum("_ReqGuildPoint");
	sFieldNum._DailyGuildPointLimit	= pSox->GetFieldNum("_DailyGuildPointLimit");
	sFieldNum._DailyMissionPointLimit = pSox->GetFieldNum("_DailyMissionPointLimit");
	sFieldNum._IsApplicable		= pSox->GetFieldNum("_IsApplicable");

	//##################################################################
	// Load
	//##################################################################

	for (int i = 0; i < pSox->GetItemCount(); ++i)
	{
		int nItemID = pSox->GetItemID(i);
		int nIdx = pSox->GetIDXprimary(nItemID);

		int nLevel = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._GuildLevel)->GetInteger();

		TGuildLevel Data = {0, };

		Data.nReqGuildPoint = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._ReqGuildPoint)->GetInteger();
		Data.nReqGold = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._ReqGold)->GetInteger();
		Data.nDailyGuildPointLimit = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._DailyGuildPointLimit)->GetInteger();
		Data.nDailyMissionPointLimit = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._DailyMissionPointLimit)->GetInteger();
		Data.bApplicable = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._IsApplicable)->GetInteger() ? true : false;

		if (Data.bApplicable)
		{
			m_GuildLevel[nLevel] = Data;
			m_GuildLevel[nLevel].nLevel = nLevel;

			m_nMaxGuildExp = Data.nReqGuildPoint;
			if (m_nMaxGuildLevel < nLevel)
				m_nMaxGuildLevel = nLevel;
		}
	}
	return true;
}

int CDNGameDataManager::GetMaxGuildExp()
{
	return m_nMaxGuildExp;
}

int CDNGameDataManager::GetMaxMaxGuildLevel()
{
	return m_nMaxGuildLevel;
}

TGuildLevel *CDNGameDataManager::GetGuildLevelData(int nLevel)
{
	if (nLevel <= 0) return NULL;

	TMapGuilLevel::iterator iter = m_GuildLevel.find(nLevel);
	if (iter == m_GuildLevel.end()) return NULL;

	return &(iter->second);
}

bool CDNGameDataManager::LoadGuildWarPointData()
{
#ifdef _WORK
	DNTableFileFormat* pSox;
	if (m_bAllLoaded)
		pSox = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TGUILDWARPOINT );
	else
		pSox = GetDNTable( CDnTableDB::TGUILDWARPOINT );
#else		//#ifdef _WORK
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TGUILDWARPOINT );
#endif		//#ifdef _WORK

	if( !pSox )
	{
		g_Log.Log( LogType::_FILELOG, L"guildwarpointtable.dnt failed\r\n" );
		return false;
	}

	if( pSox->GetItemCount() <= 0 )
	{		
		g_Log.Log( LogType::_FILELOG, L"guildwarpointtable.dnt Count(%d)\r\n", pSox->GetItemCount() );
		return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		m_GuildWarPoint.clear();
	}
#endif		//#ifdef _WORK

	//##################################################################
	// FieldNum 미리색인
	//##################################################################

	struct TempFieldNum
	{
		int		_GuildWarRank;
		int		_RewardGuildPoint;
	};

	TempFieldNum sFieldNum;
	sFieldNum._GuildWarRank		= pSox->GetFieldNum("_GuildWarRank");
	sFieldNum._RewardGuildPoint	= pSox->GetFieldNum("_RewardGuildPoint");

	//##################################################################
	// Load
	//##################################################################

	for (int i = 0; i < pSox->GetItemCount(); ++i)
	{
		int nItemID = pSox->GetItemID(i);
		int nIdx = pSox->GetIDXprimary(nItemID);

		int nRank = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._GuildWarRank)->GetInteger();

		TGuildWarPoint Data = {0, };

		Data.nRewardGuildPoint = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._RewardGuildPoint)->GetInteger();
		
		m_GuildWarPoint[nRank] = Data;
		m_GuildWarPoint[nRank].nRank = nRank;
	}

	return true;
}

TGuildWarPoint *CDNGameDataManager::GetGuildWarPointData(int nRank)
{
	if (nRank <= 0) return NULL;

	TMapGuilWarPoint::iterator iter = m_GuildWarPoint.find(nRank);
	if (iter == m_GuildWarPoint.end()) return NULL;

	return &(iter->second);
}

bool CDNGameDataManager::LoadGuildWareData()
{

	// 최대값 초기화
	m_nGuildLimitMaxCoin = 0;
	m_nGuildLimitMaxItem = 0;

#ifdef _WORK
	DNTableFileFormat* pSox;
	if (m_bAllLoaded)
		pSox = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TGUILDWAREHOUSE );
	else
		pSox = GetDNTable( CDnTableDB::TGUILDWAREHOUSE );
#else		//#ifdef _WORK
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TGUILDWAREHOUSE );
#endif		//#ifdef _WORK
	if( !pSox )
	{
		g_Log.Log( LogType::_FILELOG, L"GuildWareHouse.dnt failed\r\n" );
		return false;
	}

	if( pSox->GetItemCount() <= 0 )
	{
		g_Log.Log( LogType::_FILELOG, L"GuildWareHouse.dnt Count(%d)\r\n", pSox->GetItemCount() );
		return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		m_GuildLimitItem.clear();
		m_GuildLimitCoin.clear();
	}
#endif		//#ifdef _WORK

	//##################################################################
	// FieldNum 미리색인
	//##################################################################

	struct TempFieldNum
	{
		int		_WareHouseType;
		int		_LimitIndex;
		int		_LimitValue;
	};

	TempFieldNum sFieldNum;
	sFieldNum._WareHouseType	= pSox->GetFieldNum("_WareHouseType");
	sFieldNum._LimitIndex		= pSox->GetFieldNum("_Limit_Index");
	sFieldNum._LimitValue		= pSox->GetFieldNum("_Limit_Value");

	//##################################################################
	// Load
	//##################################################################

	for (int i=0; i<pSox->GetItemCount(); i++)
	{
		int nItemID = pSox->GetItemID(i);
		int nIdx = pSox->GetIDXprimary(nItemID);

		int nType			= pSox->GetFieldFromLablePtr(nIdx, sFieldNum._WareHouseType)->GetInteger();
		int nLimitIndex		= pSox->GetFieldFromLablePtr(nIdx, sFieldNum._LimitIndex)->GetInteger();
		int nLimitValue		= pSox->GetFieldFromLablePtr(nIdx, sFieldNum._LimitValue)->GetInteger();

		// 길드창고 아이템 이용제한
		if (nType == 0)	
		{
			// 중복키 검사
			if (m_GuildLimitItem.find(nLimitIndex) != m_GuildLimitItem.end())
			{
				DN_ASSERT(0,	"Check!");
			}

			// 최대값 산출
			if (nLimitValue > m_nGuildLimitMaxItem)
				m_nGuildLimitMaxItem = nLimitValue;
				
			m_GuildLimitItem[nLimitIndex] = nLimitValue;
		}
		// 길드창고 출금 제한
		else if (nType == 1)	
		{
			// 중복키 검사
			if (m_GuildLimitCoin.find(nLimitIndex) != m_GuildLimitCoin.end())
			{
				DN_ASSERT(0, "Check");
			}


			// 최대값 산출
			if (nLimitValue > m_nGuildLimitMaxCoin)
				m_nGuildLimitMaxCoin = nLimitValue;

			m_GuildLimitCoin[nLimitIndex] = nLimitValue;
		}
		else
			DN_ASSERT(0, "Check");
	}

	return true;
}

int CDNGameDataManager::GetGuildLimitItem(int nIndex)
{
	if (nIndex < 0) return -1;

	std::map <int, int>::iterator iter = m_GuildLimitItem.find(nIndex);
	if (iter == m_GuildLimitItem.end()) return -1;

	return iter->second;
}


int CDNGameDataManager::GetGuildLimitCoin(int nIndex)
{
	if (nIndex < 0) return -1;

	std::map <int, int>::iterator iter = m_GuildLimitCoin.find(nIndex);
	if (iter == m_GuildLimitCoin.end()) return -1;

	return iter->second;
}

int CDNGameDataManager::GetGuildLimitMaxItem()
{
	return m_nGuildLimitMaxItem;
}

int CDNGameDataManager::GetGuildLimitMaxCoin()
{
	return m_nGuildLimitMaxCoin;
}

// VehicleTable
bool CDNGameDataManager::LoadVehicleData()
{
#ifdef _WORK
	DNTableFileFormat* pSox;
	if (m_bAllLoaded)
		pSox = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TVEHICLE );
	else
		pSox = GetDNTable( CDnTableDB::TVEHICLE );
#else		//#ifdef _WORK
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TVEHICLE );
#endif		//#ifdef _WORK
	if( !pSox )
	{
		g_Log.Log( LogType::_FILELOG, L"vehicletable.dnt failed\r\n" );
		return false;
	}

	if( pSox->GetItemCount() <= 0 )
	{
		g_Log.Log( LogType::_FILELOG, L"vehicletable.dnt Count(%d)\r\n", pSox->GetItemCount() );
		return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		m_MapVehicle.clear();
	}
#endif		//#ifdef _WORK

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{
		int _VehicleClassID;
		int _VehicleActorID;
		int _DefaultBodyParts;
		int _DefaultPartsA;
		int _DefaultPartsB;
		int _VehicleSummonTime;
		int _VehicleDefaultSpeed;
		int _nPetLevelTypeID;
		int _nPetSkillID1;
		int _nPetSkillID2;
		int _nRange;
		int _nFoodID;
	};

	TempFieldNum sFieldNum;
	sFieldNum._VehicleClassID = pSox->GetFieldNum("_VehicleClassID");
	sFieldNum._VehicleActorID = pSox->GetFieldNum("_VehicleActorID");
	sFieldNum._DefaultBodyParts = pSox->GetFieldNum("_DefaultBodyParts");
	sFieldNum._DefaultPartsA = pSox->GetFieldNum("_DefaultPartsA");
	sFieldNum._DefaultPartsB = pSox->GetFieldNum("_DefaultPartsB");
	sFieldNum._VehicleSummonTime = pSox->GetFieldNum("_VehicleSummonTime");
	sFieldNum._VehicleDefaultSpeed = pSox->GetFieldNum("_VehicleDefaultSpeed");
	sFieldNum._nPetLevelTypeID = pSox->GetFieldNum("_PetLevelTypeID");
	sFieldNum._nPetSkillID1 = pSox->GetFieldNum("_PetSKILLID1");
	sFieldNum._nPetSkillID2 = pSox->GetFieldNum("_PetSKILLID2");
	sFieldNum._nRange = pSox->GetFieldNum("_Range");
	sFieldNum._nFoodID = pSox->GetFieldNum("_FoodID");

	//##################################################################
	// Load
	//##################################################################

	for (int i = 0; i < pSox->GetItemCount(); ++i)
	{
		int nItemID = pSox->GetItemID(i);
		int nIdx = pSox->GetIDXprimary(nItemID);

		TVehicleData Data = {0, };
		Data.nItemID = nItemID;
		Data.nVehicleClassID = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._VehicleClassID)->GetInteger();
		Data.nVehicleActorID = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._VehicleActorID)->GetInteger();
		Data.nDefaultBody = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._DefaultBodyParts)->GetInteger();
		Data.nDefaultParts1 = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._DefaultPartsA)->GetInteger();
		Data.nDefaultParts2 = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._DefaultPartsB)->GetInteger();
		Data.nVehicleSummonTime = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._VehicleSummonTime)->GetInteger();
		Data.nVehicleDefaultSpeed = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._VehicleDefaultSpeed)->GetInteger();
		Data.nPetLevelTypeID = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._nPetLevelTypeID)->GetInteger();
		Data.nPetSkillID1 = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._nPetSkillID1)->GetInteger();
		Data.nPetSkillID2 = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._nPetSkillID2)->GetInteger();
		Data.nRange = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._nRange)->GetInteger();
		Data.nFoodID = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._nFoodID)->GetInteger();
		m_MapVehicle.insert(make_pair(Data.nItemID, Data));
	}

	return true;
}

TVehicleData *CDNGameDataManager::GetVehicleData(int nItemID)
{
	if (nItemID <= 0) return NULL;

	TMapVehicleData::iterator iter = m_MapVehicle.find(nItemID);
	if (iter == m_MapVehicle.end()) return NULL;

	return &(iter->second);
}

int CDNGameDataManager::GetVehicleClassID(int nItemID)
{
	TVehicleData *pVehicle = GetVehicleData(nItemID);
	if (!pVehicle) return 0;

	return pVehicle->nVehicleClassID;
}

int CDNGameDataManager::GetVehicleActorID(int nItemID)
{
	TVehicleData *pVehicle = GetVehicleData(nItemID);
	if (!pVehicle) return 0;

	return pVehicle->nVehicleActorID;
}

int CDNGameDataManager::GetPetLevelTypeID(int nItemID)
{
	TVehicleData *pVehicle = GetVehicleData(nItemID);
	if (!pVehicle) return 0;

	return pVehicle->nPetLevelTypeID;
}

// VehiclePartsTable
bool CDNGameDataManager::LoadVehiclePartsData()
{
#ifdef _WORK
	DNTableFileFormat* pSox;
	if (m_bAllLoaded)
		pSox = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TVEHICLEPARTS );
	else
		pSox = GetDNTable( CDnTableDB::TVEHICLEPARTS );
#else		//#ifdef _WORK
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TVEHICLEPARTS );
#endif		//#ifdef _WORK
	if( !pSox )
	{
		g_Log.Log( LogType::_FILELOG, L"vehiclepartstable.dnt failed\r\n" );
		return false;
	}

	if( pSox->GetItemCount() <= 0 )
	{
		g_Log.Log( LogType::_FILELOG, L"vehiclepartstable.dnt Count(%d)\r\n", pSox->GetItemCount() );
		return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		m_MapVehicleParts.clear();
	}
#endif		//#ifdef _WORK

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{
		int _VehicleClassID;
		int _VehiclePartsType;
		int _SkinName;
		int _AniName;
		int _ActName;
		int _SkinColor;
	};

	TempFieldNum sFieldNum;
	sFieldNum._VehicleClassID = pSox->GetFieldNum("_VehicleClassID");
	sFieldNum._VehiclePartsType = pSox->GetFieldNum("_VehiclePartsType");
	sFieldNum._SkinName = pSox->GetFieldNum("_SkinName");
	sFieldNum._AniName = pSox->GetFieldNum("_AniName");
	sFieldNum._ActName = pSox->GetFieldNum("_ActName");
	sFieldNum._SkinColor = pSox->GetFieldNum("_SkinColor");

	//##################################################################
	// Load
	//##################################################################

	for (int i = 0; i < pSox->GetItemCount(); ++i)
	{
		int nItemID = pSox->GetItemID(i);
		int nIdx = pSox->GetIDXprimary(nItemID);

		//TVehiclePartsData Data = {0, };
		//2010.10.7 haling STL 보호 위해 생성자로 변환
		TVehiclePartsData Data;
		Data.nItemID = nItemID;
		Data.nVehicleClassID = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._VehicleClassID)->GetInteger();
		Data.nVehiclePartsType = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._VehiclePartsType)->GetInteger() + 1;
		Data.strSkinName = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._SkinName)->GetString();
		Data.strAniName = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._AniName)->GetString();
		Data.strActName = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._ActName)->GetString();
		Data.nSkinColor = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._SkinColor)->GetInteger();

		m_MapVehicleParts.insert(make_pair(Data.nItemID, Data));
	}

	return true;
}

TVehiclePartsData *CDNGameDataManager::GetVehiclePartsData(int nItemID)
{
	if (nItemID <= 0) return NULL;

	TMapVehicleParts::iterator iter = m_MapVehicleParts.find(nItemID);
	if (iter == m_MapVehicleParts.end()) return NULL;

	return &(iter->second);
}

int CDNGameDataManager::GetVehiclePartsType(int nItemID)
{
	TVehiclePartsData *pVehicle = GetVehiclePartsData(nItemID);
	if (!pVehicle) return 0;

	return pVehicle->nVehiclePartsType;
}

bool CDNGameDataManager::LoadPetLevelData()
{
#ifdef _WORK
	DNTableFileFormat* pSox;
	if (m_bAllLoaded)
		pSox = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TPETLEVEL );
	else
		pSox = GetDNTable( CDnTableDB::TPETLEVEL );
#else		//#ifdef _WORK
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TPETLEVEL );
#endif		//#ifdef _WORK
	if( !pSox )
	{
		g_Log.Log( LogType::_FILELOG, L"petleveltable.dnt failed\r\n" );
		return false;
	}

	if( pSox->GetItemCount() <= 0 )
	{
		g_Log.Log( LogType::_FILELOG, L"petleveltable.dnt Count(%d)\r\n", pSox->GetItemCount() );
		return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		SAFE_DELETE_PMAP(TMapPetLevelData, m_pPetLevelData);
	}
#endif		//#ifdef _WORK

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{
		int _PetLevelTypeID;
		int _PetLevel;
		int _PetExp;
		std::vector<int> _State;
		std::vector<int> _State_Min;
		std::vector<int> _State_Max;
		int _VipExp;
	};

	char szTemp[256] = { 0, };

	TempFieldNum sFieldNum;
	sFieldNum._PetLevelTypeID = pSox->GetFieldNum("_PetLevelTypeID");
	sFieldNum._PetLevel = pSox->GetFieldNum("_PetLevel");
	sFieldNum._PetExp = pSox->GetFieldNum("_PetExp");
	sFieldNum._State.reserve(PetStateMax);
	sFieldNum._State_Min.reserve(PetStateMax);
	sFieldNum._State_Max.reserve(PetStateMax);
	for (int j = 0; j < PetStateMax; ++j) 
	{
		sprintf_s(szTemp, "_State%d", j+1);
		sFieldNum._State.push_back(pSox->GetFieldNum(szTemp));
		sprintf_s(szTemp, "_State%d_Min", j+1);
		sFieldNum._State_Min.push_back(pSox->GetFieldNum(szTemp));
		sprintf_s(szTemp, "_State%d_Max", j+1);
		sFieldNum._State_Max.push_back(pSox->GetFieldNum(szTemp));
	}

	sFieldNum._VipExp = pSox->GetFieldNum("_VipExp");

	//##################################################################
	// Load
	//##################################################################

	for (int i = 0; i < pSox->GetItemCount(); ++i)
	{
		int nItemID = pSox->GetItemID(i);
		int nIdx = pSox->GetIDXprimary(nItemID);

		int nPetLevelType = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._PetLevelTypeID)->GetInteger();

		TPetLevelData *pData = GetPetLevelData(nPetLevelType);
		if (!pData){
			pData = new TPetLevelData;
			pData->nPetLevelType = nPetLevelType;
			m_pPetLevelData.insert(make_pair(nPetLevelType, pData));
		}

		TPetLevelDetail Detail = {0,};
		Detail.nTableIndex = nItemID;
		Detail.nPetLevel = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._PetLevel)->GetInteger();
		Detail.nPetExp = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._PetExp)->GetInteger();

		for (int j = 0; j < PetStateMax; j++){
			Detail.nState[j] = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._State[j])->GetInteger();
			Detail.szStateMin[j] = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._State_Min[j])->GetString();
			Detail.szStateMax[j] = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._State_Max[j])->GetString();
		}

		Detail.nVipExp = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._VipExp)->GetInteger();

		pData->VecPetLevel.push_back(Detail);
	}

	return true;
}

TPetLevelData *CDNGameDataManager::GetPetLevelData(int nPetLevelType)
{
	TMapPetLevelData::iterator iter = m_pPetLevelData.find(nPetLevelType);
	if (iter == m_pPetLevelData.end()) return NULL;

	return iter->second;
}

TPetLevelDetail *CDNGameDataManager::GetPetLevelDetail(int nPetBodyItemID, int nPetExp)
{
	int nLevelTypeID = GetPetLevelTypeID(nPetBodyItemID);
	TPetLevelData *pPetData = g_pDataManager->GetPetLevelData(nLevelTypeID);
	if (!pPetData) return NULL;
	if (pPetData->VecPetLevel.empty()) return NULL;

	int nPetLevelIndex = -1;
	for (int i = 0; i <(int)pPetData->VecPetLevel.size(); i++)
	{
		if( nPetExp < pPetData->VecPetLevel[i].nPetExp )
		{
			nPetLevelIndex = i;
			break;
		}
		else
			nPetLevelIndex = i;	// MAX레밸일 경우
	}

	if( nPetLevelIndex == -1 )
		return NULL;

	return &(pPetData->VecPetLevel[nPetLevelIndex]);
}

int CDNGameDataManager::GetPetLevelTableIndex(int nPetBodyItemID, int nPetExp)
{
	TPetLevelDetail *pDetail = GetPetLevelDetail(nPetBodyItemID, nPetExp);
	if (!pDetail) return -1;

	return pDetail->nTableIndex;
}

bool CDNGameDataManager::LoadPetFoodData()
{
#ifdef _WORK
	DNTableFileFormat* pSox;
	if (m_bAllLoaded)
		pSox = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TPETFOOD );
	else
		pSox = GetDNTable( CDnTableDB::TPETFOOD );
#else		//#ifdef _WORK
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TPETFOOD );
#endif		//#ifdef _WORK
	if( !pSox )
	{
		g_Log.Log( LogType::_FILELOG, L"petfoodtable.dnt failed\r\n" );
		return false;
	}

	if( pSox->GetItemCount() <= 0 )
	{
		g_Log.Log( LogType::_FILELOG, L"vehiclepartstable.dnt Count(%d)\r\n", pSox->GetItemCount() );
		return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		m_MapVehicleParts.clear();
	}
#endif		//#ifdef _WORK

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{
		int _FullTime;
		int _FullTic;
		int _FullTimeLogout;
		int _FullMaxCount;
		std::vector<int> _UseExp;
		std::vector<int> _UseState;
	};

	TempFieldNum sFieldNum;
	sFieldNum._FullTime = pSox->GetFieldNum("_FullTime");
	sFieldNum._FullTic = pSox->GetFieldNum("_FullTic");
	sFieldNum._FullTimeLogout = pSox->GetFieldNum("_FullTimeLogout");	
	sFieldNum._FullMaxCount = pSox->GetFieldNum("_FullMaxCount");
	sFieldNum._UseExp.reserve(Pet::MAX_SATIETY_SELECTION);
	sFieldNum._UseState.reserve(Pet::MAX_SATIETY_SELECTION);

	char szTemp[256] = { 0, };
	for (int j = 0; j < Pet::MAX_SATIETY_SELECTION; ++j) 
	{
		sprintf_s(szTemp, "_Useexp%d", j+1);
		sFieldNum._UseExp.push_back(pSox->GetFieldNum(szTemp));
		sprintf_s(szTemp, "_Usestate%d", j+1);
		sFieldNum._UseState.push_back(pSox->GetFieldNum(szTemp));
	}

	//##################################################################
	// Load
	//##################################################################

	for (int i = 0; i < pSox->GetItemCount(); ++i)
	{
		int nItemID = pSox->GetItemID(i);
		int nIdx = pSox->GetIDXprimary(nItemID);

		TPetFoodData Data = {0, };		
		Data.nItemID = nItemID;
		Data.nTickTime  = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._FullTime)->GetInteger();
		Data.nFullTic = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._FullTic)->GetInteger();
		Data.cFullTimeLogOut = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._FullTimeLogout)->GetInteger();		
		Data.nFullMaxCount = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._FullMaxCount)->GetInteger();

		for (int j = 0; j < Pet::MAX_SATIETY_SELECTION; j++){
			Data.nUseExp[j] = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._UseExp[j])->GetInteger();
			Data.nUseState[j] = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._UseState[j])->GetInteger();
		}

		m_MapPetFoodData.insert(make_pair(Data.nItemID, Data));
	}
	return true;
}

TPetFoodData *CDNGameDataManager::GetPetFoodData(int nItemID)
{
	TMapPetFoodData::iterator iter = m_MapPetFoodData.find(nItemID);
	if (iter == m_MapPetFoodData.end()) return NULL;

	return &(iter->second);
}

unsigned int CDNGameDataManager::MemoryToRSHashKey(const wchar_t *pszData, long nSize)
{
	unsigned int nB			= 378551;
	unsigned int nA			= 63689;
	unsigned int nHashKey	= 0;

	for (int i = 0; i < nSize; i++)
	{
		nHashKey = nHashKey * nA + pszData[i];
		nA = nA * nB;
	}

	return (nHashKey & 0x7FFFFFFF);
}

#if defined(PRE_ADD_MULTILANGUAGE)
bool CDNGameDataManager::LoadProhibitWord(int nLanguage, const char * pszFilePath)
{
	if (pszFilePath == NULL)
		return false;

	CXMLParser parser;
	if (parser.Open(pszFilePath) == false)
	{
		g_Log.Log(LogType::_FILELOG, L"%S failed\r\n", pszFilePath);
		return false;
	}

	TProhibitWord * pProhibit = GetLanguageSlangFilter(nLanguage);
	if (pProhibit == NULL)
	{
		TProhibitWord mProhibit;
		m_ProhibitWordList.insert(std::make_pair(nLanguage, mProhibit));

		pProhibit = GetLanguageSlangFilter(nLanguage);
		if (pProhibit == NULL)
			return false;
	}
#else		//#if defined(PRE_ADD_MULTILANGUAGE)
bool CDNGameDataManager::LoadProhibitWord()
{
	char buf[_MAX_PATH] = { 0, };
	sprintf_s( buf, "%s/Resource/UIString/ProhibitWord.xml", g_Config.szResourcePath.c_str() );

	CXMLParser parser;

	if (parser.Open(buf) == false)
	{
		g_Log.Log(LogType::_FILELOG, L"ProhibitWord.xml failed\r\n");
		return false;
	}
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)

	if (parser.FirstChildElement("ProhibitWord", true) == true)
	{
		if (parser.FirstChildElement("Account", true) == true)
		{
			if (parser.FirstChildElement("AccountWord", true) == true)
			{
				do {
					if (parser.GetText())
					{
						wstring wszStr = parser.GetText();
						std::transform( wszStr.begin(), wszStr.end(), wszStr.begin(), towlower );
						unsigned int nHashKey = MemoryToRSHashKey(wszStr.c_str(), (long)wszStr.length());
#if defined(PRE_ADD_MULTILANGUAGE)
						pProhibit->insert(make_pair(nHashKey, wszStr));
#else		//#if defined(PRE_ADD_MULTILANGUAGE)
						m_ProhibitWordList.insert(make_pair(nHashKey, wszStr));
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)
					}

					/*if (parser.GetText()){
						wstring wszStr = parser.GetText();
						unsigned int nHashKey = MemoryToRSHashKey(wszStr.c_str(), (long)wszStr.length());
						m_ProhibitWordList.insert(make_pair(nHashKey, wszStr));
					}*/
				} while(parser.NextSiblingElement("AccountWord"));

				parser.GoParent();
			} 

			parser.GoParent();
		}

#if defined(_KR) || defined(_KRAZ)
		if (parser.FirstChildElement("Chat", true) == true)
		{
			if (parser.FirstChildElement("ChatWord", true) == true)
			{
				do {
					if (parser.GetText())
					{
						wstring wszStr = parser.GetText();
						std::transform( wszStr.begin(), wszStr.end(), wszStr.begin(), towlower );
						unsigned int nHashKey = MemoryToRSHashKey(wszStr.c_str(), (long)wszStr.length());
#if defined(PRE_ADD_MULTILANGUAGE)
						pProhibit->insert(make_pair(nHashKey, wszStr));
#else		//#if defined(PRE_ADD_MULTILANGUAGE)
						m_ProhibitWordList.insert(make_pair(nHashKey, wszStr));
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)
					}
				} while(parser.NextSiblingElement("ChatWord"));

				parser.GoParent();
			} 

			parser.GoParent();
		}
#endif
		return true;
	}

	g_Log.Log( LogType::_FILELOG, L"ProhibitWord parsing failed\r\n");
	return false;
}

#if defined(PRE_ADD_MULTILANGUAGE)
CDNGameDataManager::TProhibitWord * CDNGameDataManager::GetLanguageSlangFilter(int nLanguage)
{
	TProhibitWord * pList = NULL;
	std::map <int, TProhibitWord>::iterator ii = m_ProhibitWordList.find(nLanguage);
	if (ii != m_ProhibitWordList.end())
		return &(*ii).second;
	return NULL;
}

bool CDNGameDataManager::CheckProhibitWord(int nLanguage, const wchar_t *pwszWord)
{
	TProhibitWord * pProhibit = GetLanguageSlangFilter(nLanguage);
	if (pProhibit)
	{
		std::wstring::size_type index;

		std::wstring strTemp(pwszWord);
		std::transform ( strTemp.begin(), strTemp.end(), strTemp.begin(), towlower );

		for( std::map<unsigned int, std::wstring>::iterator itor = pProhibit->begin(); itor != pProhibit->end(); itor++ )
		{
			index = strTemp.find( itor->second );
			if( index != string::npos )		
				return true;
		}
	}
	return false;
}

#else		//#if defined(PRE_ADD_MULTILANGUAGE)
bool CDNGameDataManager::CheckProhibitWord(const wchar_t *pwszWord)
{
	std::wstring::size_type index;

	std::wstring strTemp(pwszWord);
	std::transform ( strTemp.begin(), strTemp.end(), strTemp.begin(), towlower );

	for( std::map<unsigned int, std::wstring>::iterator itor = m_ProhibitWordList.begin(); itor != m_ProhibitWordList.end(); itor++ )
	{
		index = strTemp.find( itor->second );
		if( index != string::npos )
			return true;
	}
	return false;
}
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)

bool CDNGameDataManager::LoadExchangeTradeData()
{
#ifdef _WORK
	DNTableFileFormat* pSox;
	if (m_bAllLoaded)
		pSox = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TEXCHANGETRADE );
	else
		pSox = GetDNTable( CDnTableDB::TEXCHANGETRADE );
#else	//#ifdef _WORK
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TEXCHANGETRADE );
#endif		//#ifdef _WORK
	if( !pSox )
	{
		g_Log.Log( LogType::_FILELOG, L"Exchange.dnt failed\r\n" );
		return false;
	}

	if( pSox->GetItemCount() <= 0 )
	{
		g_Log.Log( LogType::_FILELOG, L"Exchange.dnt Count(%d)\r\n", pSox->GetItemCount() );
		return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		m_MapExchangeTrade.clear();
		for (int i = 0; i < ITEMEXCHANGEGROUP_MAX; i++)
			m_VecExchangeGroupID[i].clear();
	}
#endif		//#ifdef _WORK

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{
		int _CategoryType;
		int _GroupID;
		int _ExchangeType;
	};

	TempFieldNum sFieldNum;
	sFieldNum._CategoryType = pSox->GetFieldNum("_CategoryType");
	sFieldNum._GroupID = pSox->GetFieldNum("_GroupID");
	sFieldNum._ExchangeType = pSox->GetFieldNum("_ExchangeType");
	

	//##################################################################
	// Load
	//##################################################################

	for (int i = 0; i < pSox->GetItemCount(); ++i)
	{
		int nItemID = pSox->GetItemID(i);
		int nIdx = pSox->GetIDXprimary(nItemID);

		
		TExchangeTrade Data;
		Data.nItemID		= nItemID;
		Data.nCategoryType	= pSox->GetFieldFromLablePtr(nIdx, sFieldNum._CategoryType)->GetInteger();
		Data.nGroupID		= pSox->GetFieldFromLablePtr(nIdx, sFieldNum._GroupID)->GetInteger();
		Data.ExchangeType	= pSox->GetFieldFromLablePtr(nIdx, sFieldNum._ExchangeType)->GetInteger();
		
		m_MapExchangeTrade.insert(make_pair(Data.nItemID, Data));
		
		// 그룹아이디 최대갯수 검사
		if (Data.nGroupID >= ITEMEXCHANGEGROUP_MAX)
		{
			g_Log.Log( LogType::_FILELOG, L"LoadExchangeTradeData Over ITEMEXCHANGEGROUP_MAX\r\n");
			return false;
		}

		// 카테고리ID에 따라 그룹별로 저장한다.
		if (Data.nCategoryType == 1)
			m_VecExchangeGroupID[Data.nGroupID].push_back(Data.ExchangeType);
	}

	return true;
}

bool CDNGameDataManager::GetExchangeGroupID(int nGroupID, char cItemChange[])
{
	if (nGroupID >= ITEMEXCHANGEGROUP_MAX)
		return false;

	if (m_VecExchangeGroupID[nGroupID].empty())
		return false;

	int i = 0;
	std::vector<char>::iterator iter = m_VecExchangeGroupID[nGroupID].begin();
	for(; iter != m_VecExchangeGroupID[nGroupID].end(); iter++)
	{
		if (i >= ITEMEXCHANGEGROUP_MAX)
			return false;

		cItemChange[i] = (*iter);
		i++;
	}

	return true;
}

TExchangeTrade* CDNGameDataManager::GetExchageTradeData(int nItemID)
{
	if (nItemID <= 0) return NULL;

	TMapExchangeTrade::iterator iter = m_MapExchangeTrade.find(nItemID);
	if (iter == m_MapExchangeTrade.end()) return NULL;

	return &(iter->second);
}

bool CDNGameDataManager::LoadPVPMissionRoom()
{
#ifdef _WORK
	DNTableFileFormat* pSox;
	if (m_bAllLoaded)
		pSox = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TPVPMISSIONROOM );
	else
		pSox = GetDNTable( CDnTableDB::TPVPMISSIONROOM );
#else		//#ifdef _WORK
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TPVPMISSIONROOM );
#endif		//#ifdef _WORK
	if( !pSox )
	{
		g_Log.Log( LogType::_FILELOG, L"PvPMissionRoomTable.dnt failed\r\n" );
		return false;
	}

	if( pSox->GetItemCount() <= 0 )
	{
		g_Log.Log( LogType::_FILELOG, L"PvPMissionRoomTable.dnt Count(%d)\r\n", pSox->GetItemCount() );
		return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		m_MapPvPMissionRoom.clear();
	}
#endif		//#ifdef _WORK

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{
		int _PvPMissionRoomGroup;
		int _PvPMapTableID;
		int _PvPGameModeTableID;
		int _WinCondition;
		int _PlayTime;
		int _MaxPlayers;
		int _StartPlayers;
		int _TitleUIString;
		int _DropItem;
#ifdef PRE_ADD_COLOSSEUM_BEGINNER
		int _ChannelType;
#endif		//#ifdef PRE_ADD_COLOSSEUM_BEGINNER
	};

	TempFieldNum sFieldNum;
	sFieldNum._PvPMissionRoomGroup	= pSox->GetFieldNum("_PvPMissionRoomGroup");
	sFieldNum._PvPMapTableID		= pSox->GetFieldNum("_PvPMapTableID");
	sFieldNum._PvPGameModeTableID	= pSox->GetFieldNum("_PvPGameModeTableID");
	sFieldNum._WinCondition			= pSox->GetFieldNum("_WinCondition");
	sFieldNum._PlayTime				= pSox->GetFieldNum("_PlayTime");
	sFieldNum._MaxPlayers			= pSox->GetFieldNum("_MaxUser");
	sFieldNum._StartPlayers			= pSox->GetFieldNum("_StartUser");
	sFieldNum._TitleUIString		= pSox->GetFieldNum("_TitleUIString");
	sFieldNum._DropItem				= pSox->GetFieldNum("_DropItem");
#ifdef PRE_ADD_COLOSSEUM_BEGINNER
	sFieldNum._ChannelType = pSox->GetFieldNum("_PVPChannel");
#endif		//#ifdef PRE_ADD_COLOSSEUM_BEGINNER

	//##################################################################
	// Load
	//##################################################################

	for (int i = 0; i < pSox->GetItemCount(); ++i)
	{
		int nItemID = pSox->GetItemID(i);
		int nIdx = pSox->GetIDXprimary(nItemID);


		TPvPMissionRoom Data;
		Data.nItemID		= nItemID;
		Data.nGroupID		= pSox->GetFieldFromLablePtr(nIdx, sFieldNum._PvPMissionRoomGroup)->GetInteger();
		Data.nMapID			= pSox->GetFieldFromLablePtr(nIdx, sFieldNum._PvPMapTableID)->GetInteger();
		Data.nModeID		= pSox->GetFieldFromLablePtr(nIdx, sFieldNum._PvPGameModeTableID)->GetInteger();
		Data.nWinCondition	= pSox->GetFieldFromLablePtr(nIdx, sFieldNum._WinCondition)->GetInteger();
		Data.nPlayTime		= pSox->GetFieldFromLablePtr(nIdx, sFieldNum._PlayTime)->GetInteger();
		Data.nMaxPlayers	= pSox->GetFieldFromLablePtr(nIdx, sFieldNum._MaxPlayers)->GetInteger();
		Data.nStartPlayers	= pSox->GetFieldFromLablePtr(nIdx, sFieldNum._StartPlayers)->GetInteger();
		Data.nRoomNameIndex = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._TitleUIString)->GetInteger();
		Data.bDropItem		= pSox->GetFieldFromLablePtr(nIdx, sFieldNum._DropItem)->GetInteger() ? true : false;
#ifdef PRE_ADD_COLOSSEUM_BEGINNER
		Data.nChannelType = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._ChannelType)->GetInteger();
#endif		//#ifdef PRE_ADD_COLOSSEUM_BEGINNER
		
		// 그룹아이디 최대갯수 검사
		if (Data.nGroupID >= PVPMISSIONROOMGROUP_MAX)
		{
			g_Log.Log( LogType::_FILELOG, L"LoadPVPMissionRoom Over PVPMISSIONROOMGROUP_MAX\r\n");
			return false;
		}

		m_MapPvPMissionRoom.insert(make_pair(nItemID, Data));

		m_VecPvPMissionRoom.push_back(Data);
	}

	return true;
}

TPvPMissionRoom* CDNGameDataManager::GetPvPMissionRoom(int nItemID)
{
	if(nItemID <= 0) return NULL;

	TMapPvPMissionRoom::iterator iter = m_MapPvPMissionRoom.find(nItemID);
	if(iter == m_MapPvPMissionRoom.end()) return NULL;

	return &(iter->second);
}

TVecPvPMissionRoom& CDNGameDataManager::GetVecPvPMissionRoom()
{
	return m_VecPvPMissionRoom;
}

bool CDNGameDataManager::LoadGhoulModeCondition()
{
#ifdef _WORK
	DNTableFileFormat* pSox;
	if (m_bAllLoaded)
		pSox = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TGHOULMODE_CONDITION );
	else
		pSox = GetDNTable( CDnTableDB::TGHOULMODE_CONDITION );
#else		//#ifdef _WORK
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TGHOULMODE_CONDITION );
#endif		//#ifdef _WORK
	if( !pSox )
	{
		g_Log.Log( LogType::_FILELOG, L"ghoulconditiontable.dnt failed\r\n" );
		return false;
	}

	if( pSox->GetItemCount() <= 0 )
	{
		g_Log.Log( LogType::_FILELOG, L"ghoulconditiontable.dnt Count(%d)\r\n", pSox->GetItemCount() );
		return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		m_VecGhoulModeCondition.clear();
	}
#endif		//#ifdef _WORK

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{
		int nPlayerMaxNum[PvPCommon::Common::GhouConditionColCount];
		int nGhoulCount[PvPCommon::Common::GhouConditionColCount];
		int nMatchingMutationGroupID[PvPCommon::Common::GhouConditionColCount];
	};

	char szTemp1[32];
	char szTemp2[32];
	char szTemp3[32];
	TempFieldNum sFieldNum;
	for (int i = 0; i < PvPCommon::Common::GhouConditionColCount; i++)
	{
		sprintf_s( szTemp1, "_PlayerMaxNum%d", i+1 );
		sprintf_s( szTemp2, "_GhoulCount%d", i+1 );
		sprintf_s( szTemp3, "_GroupIndex%d", i+1 );

		sFieldNum.nPlayerMaxNum[i]	= pSox->GetFieldNum(szTemp1);
		sFieldNum.nGhoulCount[i] = pSox->GetFieldNum(szTemp2);
		sFieldNum.nMatchingMutationGroupID[i] = pSox->GetFieldNum(szTemp3);
	}

	//##################################################################
	// Load
	//##################################################################

	for (int i = 0; i < pSox->GetItemCount(); ++i)
	{
		int nItemID = pSox->GetItemID(i);
		int nIdx = pSox->GetIDXprimary(nItemID);

		TGhoulConditionData Condition;
		memset(&Condition, 0, sizeof(TGhoulConditionData));

		Condition.nItemID = nItemID;
		Condition.nItemIdx = nIdx;

		for (int j = 0; j < PvPCommon::Common::GhouConditionColCount; j++)
		{
			Condition.nPlayerMaxNum[j] = pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nPlayerMaxNum[j])->GetInteger();
			Condition.nGhoulCount[j] = pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nGhoulCount[j])->GetInteger();
			Condition.nMatchingMutationGroupID[j] = pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nMatchingMutationGroupID[j])->GetInteger();
		}

		m_VecGhoulModeCondition.push_back(Condition);
	}
	return true;
}

void CDNGameDataManager::GetGhoulGroup(int nUserCount, TMonsterMutationGroup &Group, int &nGhoulCount)
{
	if(m_VecGhoulModeCondition.empty())
		return ;

	int nGroupID = -1;
	int nLastCheckUserCount = 0;
	for (int i = 0; i < PvPCommon::Common::GhouConditionColCount; i++)
	{
		if(m_VecGhoulModeCondition[0].nPlayerMaxNum[i] <= 0) continue;
		if(nLastCheckUserCount == 0 ||(m_VecGhoulModeCondition[0].nPlayerMaxNum[i] > nLastCheckUserCount && m_VecGhoulModeCondition[0].nPlayerMaxNum[i] <= nUserCount))
		{
			nLastCheckUserCount = m_VecGhoulModeCondition[0].nPlayerMaxNum[i];
			nGroupID = m_VecGhoulModeCondition[0].nMatchingMutationGroupID[i];
			nGhoulCount = m_VecGhoulModeCondition[0].nGhoulCount[i];
		}
	}

	if(nGhoulCount <= 0)
	{
		for (int i = 0; i < PvPCommon::Common::GhouConditionColCount; i++)
		{
			if(m_VecGhoulModeCondition[0].nPlayerMaxNum[i] <= 0) continue;
			if(nLastCheckUserCount == 0 || m_VecGhoulModeCondition[0].nPlayerMaxNum[i] < nLastCheckUserCount)
			{
				nLastCheckUserCount = m_VecGhoulModeCondition[0].nPlayerMaxNum[i];
				nGroupID = m_VecGhoulModeCondition[0].nMatchingMutationGroupID[i];
				nGhoulCount = m_VecGhoulModeCondition[0].nGhoulCount[i];
				break;
			}
		}
	}

	GetMonsterMutationGroup(nGroupID, Group);
}

bool CDNGameDataManager::LoadMonsterLevel()
{
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TMONSTER );
	if( !pSox )
	{
		g_Log.Log( LogType::_FILELOG, L"monstertable.dnt.dnt failed\r\n" );
		return false;
	}

	if( pSox->GetItemCount() <= 0 )
	{
		g_Log.Log( LogType::_FILELOG, L"monstertable.dnt Count(%d)\r\n", pSox->GetItemCount() );
		return false;
	}

	//##################################################################
	// Load
	//##################################################################

	bool isValidLevel = true;
	int nMonsterLevel = 0;
	for (int i = 0; i < pSox->GetItemCount(); ++i)
	{
		int nItemID = pSox->GetItemID(i);
		nMonsterLevel= pSox->GetFieldFromLablePtr(nItemID, "_Level")->GetInteger();

		if (nMonsterLevel > m_nMaxPlayerLevel)
		{
			g_Log.Log( LogType::_FILELOG, L"monstertable.dnt Level over than MaxPlayerLevel [ItemID:%d][Level:%d]\r\n", nItemID, nMonsterLevel );
			isValidLevel = false;
		}
	}

	return isValidLevel;

}

bool CDNGameDataManager::LoadMonsterMutationGroupData()
{
#ifdef _WORK
	DNTableFileFormat* pSox;
	if (m_bAllLoaded)
		pSox = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TMONSTERGROUP_TRANS );
	else
		pSox = GetDNTable( CDnTableDB::TMONSTERGROUP_TRANS );
#else		//#ifdef _WORK
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TMONSTERGROUP_TRANS );
#endif		//#ifdef _WORK
	if( !pSox )
	{
		g_Log.Log( LogType::_FILELOG, L"monstergrouptable_transform.dnt failed\r\n" );
		return false;
	}

	if( pSox->GetItemCount() <= 0 )
	{
		g_Log.Log( LogType::_FILELOG, L"monstergrouptable_transform.dnt Count(%d)\r\n", pSox->GetItemCount() );
		return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		m_VecMonsterMutationGroup.clear();
	}
#endif		//#ifdef _WORK

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{
		int nMonsterID[PvPCommon::Common::MonsterMutationColCount];
		int nProbability[PvPCommon::Common::MonsterMutationColCount];
	};

	char szTemp1[32];
	char szTemp2[32];
	TempFieldNum sFieldNum;
	for (int i = 0; i < PvPCommon::Common::MonsterMutationColCount; i++)
	{
		sprintf_s( szTemp1, "_MonsterID%d", i+1 );
		sprintf_s( szTemp2, "_Prob%d", i+1 );

		sFieldNum.nMonsterID[i]	= pSox->GetFieldNum(szTemp1);
		sFieldNum.nProbability[i] = pSox->GetFieldNum(szTemp2);
	}


	//##################################################################
	// Load
	//##################################################################

	for (int i = 0; i < pSox->GetItemCount(); ++i)
	{
		int nItemID = pSox->GetItemID(i);
		int nIdx = pSox->GetIDXprimary(nItemID);

		TMonsterMutationGroup Group;
		memset(&Group, 0, sizeof(TMonsterMutationGroup));

		Group.nGroupID = nItemID;
		Group.nGourpIdx = nIdx;

		for (int j = 0; j < PvPCommon::Common::MonsterMutationColCount; j++)
		{
			Group.nMonsterID[j] = pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nMonsterID[j])->GetInteger();
			Group.nProbability[j] = pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nProbability[j])->GetInteger();
		}
		
		m_VecMonsterMutationGroup.push_back(Group);
	}
	
	return true;
}

bool CDNGameDataManager::GetMonsterMutationGroup(int nGroupID, TMonsterMutationGroup &Group)
{
	TVecMonsterMutationGroup::iterator ii;
	for(ii = m_VecMonsterMutationGroup.begin(); ii != m_VecMonsterMutationGroup.end(); ii++)
	{
		if((*ii).nGroupID == nGroupID)
		{
			Group = (*ii);
			return true;
		}
	}
	return false;
}

bool CDNGameDataManager::LoadMonsterMutationData()
{
#ifdef _WORK
	DNTableFileFormat* pSox;
	if (m_bAllLoaded)
		pSox = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TMONSTER_TRANS );
	else
		pSox = GetDNTable( CDnTableDB::TMONSTER_TRANS );
#else		//#ifdef _WORK
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TMONSTER_TRANS );
#endif		//#ifdef _WORK
	if( !pSox )
	{
		g_Log.Log( LogType::_FILELOG, L"monstertable_transform.dnt failed\r\n" );
		return false;
	}

	if( pSox->GetItemCount() <= 0 )
	{
		g_Log.Log( LogType::_FILELOG, L"monstertable_transform.dnt Count(%d)\r\n", pSox->GetItemCount() );
		return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		m_mMonsterMutationData.clear();
	}
#endif		//#ifdef _WORK

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{
		int nMutationActorID;

		int nMutationHP;
		int nMutationStrPMax;
		int nMutationStrMMax;
		int nMutationDefP;
		int nMutationDefM;
		int nMutationStiff;

		int nMutationStiffResis;
		int nMutationCriticalResis;
		int nMutationStunResis;

		int nMutationWeaponIdx1;
		int nMutationWeaponIdx2;
		int nMutationSkillTableID;
		int nMutationSuperArmor;

		int nSizeMin;
		int nSizeMax;
		int nMoveSpeed;
	};
	
	TempFieldNum sFieldNum;	
	sFieldNum.nMutationActorID = pSox->GetFieldNum("_ActorTableID");
	sFieldNum.nMutationHP = pSox->GetFieldNum("_HP");
	sFieldNum.nMutationStrPMax = pSox->GetFieldNum("_StrPMax");
	sFieldNum.nMutationStrMMax = pSox->GetFieldNum("_StrMMax");
	sFieldNum.nMutationDefP = pSox->GetFieldNum("_DefP");
	sFieldNum.nMutationDefM = pSox->GetFieldNum("_DefM");
	sFieldNum.nMutationStiff = pSox->GetFieldNum("_Stiff");
	sFieldNum.nMutationStiffResis = pSox->GetFieldNum("_StiffResistance");
	sFieldNum.nMutationCriticalResis = pSox->GetFieldNum("_CriticalResistance");
	sFieldNum.nMutationStunResis = pSox->GetFieldNum("_StunResistance");
	sFieldNum.nMutationWeaponIdx1 = pSox->GetFieldNum("_Weapon1Index");
	sFieldNum.nMutationWeaponIdx2 = pSox->GetFieldNum("_Weapon2Index");
	sFieldNum.nMutationSkillTableID = pSox->GetFieldNum("_SkillTable");
	sFieldNum.nMutationSuperArmor = pSox->GetFieldNum("_SuperAmmor");
	sFieldNum.nSizeMin = pSox->GetFieldNum("_SizeMin");
	sFieldNum.nSizeMax = pSox->GetFieldNum("_SizeMax");
	sFieldNum.nMoveSpeed = pSox->GetFieldNum("_MoveSpeed");

	//##################################################################
	// Load
	//##################################################################

	for (int i = 0; i < pSox->GetItemCount(); ++i)
	{
		int nItemID = pSox->GetItemID(i);
		int nIdx = pSox->GetIDXprimary(nItemID);

		TMonsterMutationData Monster;
		memset(&Monster, 0, sizeof(TMonsterMutationData));

		Monster.nMutationID = nItemID;
		Monster.nMutationIdx = nIdx;
		Monster.nMutationActorID = pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nMutationActorID)->GetInteger();

		Monster.nMutationHP = pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nMutationHP)->GetInteger();
		Monster.nMutationStrPMax = pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nMutationStrPMax)->GetInteger();
		Monster.nMutationStrMMax = pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nMutationStrMMax)->GetInteger();
		Monster.nMutationDefP = pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nMutationDefP)->GetInteger();
		Monster.nMutationDefM = pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nMutationDefM)->GetInteger();
		Monster.nMutationStiff = pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nMutationStiff)->GetInteger();

		Monster.nMutationStiffResis = pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nMutationStiffResis)->GetInteger();
		Monster.nMutationCriticalResis = pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nMutationCriticalResis)->GetInteger();
		Monster.nMutationStunResis = pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nMutationStunResis)->GetInteger();

		Monster.nMutationWeaponIdx1 = pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nMutationWeaponIdx1)->GetInteger();
		Monster.nMutationWeaponIdx2 = pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nMutationWeaponIdx2)->GetInteger();
		Monster.nMutationSkillTableID = pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nMutationSkillTableID)->GetInteger();
		Monster.nMutationSuperArmor = pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nMutationSuperArmor)->GetInteger();

		Monster.nSizeMin = static_cast<int>(pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nSizeMin)->GetFloat()*100.f);
		Monster.nSizeMax = static_cast<int>(pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nSizeMax)->GetFloat()*100.f);

		Monster.nMoveSpeed = pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nMoveSpeed)->GetInteger();
		
		m_mMonsterMutationData.insert( std::make_pair(nItemID,Monster) );
	}
	return true;
}

bool CDNGameDataManager::GetMonsterMutationData(int nMutationDataID, TMonsterMutationData &Data)
{
	TMapMonsterMutationData::iterator itor = m_mMonsterMutationData.find( nMutationDataID );
	if( itor == m_mMonsterMutationData.end() )
		return false;

	Data = (*itor).second;
	return true;
}

int CDNGameDataManager::GetMonsterMutationActorID( int nMutationDataID )
{
	TMapMonsterMutationData::iterator itor = m_mMonsterMutationData.find( nMutationDataID );
	if( itor == m_mMonsterMutationData.end() )
		return -1;

	return(*itor).second.nMutationActorID;
}

int	CDNGameDataManager::GetMonsterMutationMoveSpeed( int nMutationDataID )
{
	TMapMonsterMutationData::iterator itor = m_mMonsterMutationData.find( nMutationDataID );
	if( itor == m_mMonsterMutationData.end() )
	{
		_ASSERT(0);
		return 0;
	}

	return(*itor).second.nMoveSpeed;
}

bool CDNGameDataManager::LoadMonsterMutationSkillData()
{
#ifdef _WORK
	DNTableFileFormat* pSox;
	if (m_bAllLoaded)
		pSox = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TMONSTERSKILL_TRANS );
	else
		pSox = GetDNTable( CDnTableDB::TMONSTERSKILL_TRANS );
#else		//#ifdef _WORK
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TMONSTERSKILL_TRANS );
#endif		//#ifdef _WORK
	if( !pSox )
	{
		g_Log.Log( LogType::_FILELOG, L"monsterskilltable_transform.dnt failed\r\n" );
		return false;
	}

	if( pSox->GetItemCount() <= 0 )
	{
		g_Log.Log( LogType::_FILELOG, L"monsterskilltable_transform.dnt Count(%d)\r\n", pSox->GetItemCount() );
		return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		m_VecMonsterMutationSkill.clear();
	}
#endif		//#ifdef _WORK

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{
		int nMonsterSkillIndex[PvPCommon::Common::MonsterMutationSkillColCount];
		int nMonsterSkillLevel[PvPCommon::Common::MonsterMutationSkillColCount];
	};

	char szTemp1[32];
	char szTemp2[32];
	TempFieldNum sFieldNum;
	for (int i = 0; i < PvPCommon::Common::MonsterMutationSkillColCount; i++)
	{
		sprintf_s( szTemp1, "_SkillIndex%d", i+1 );
		sprintf_s( szTemp2, "_SkillLevel%d", i+1 );

		sFieldNum.nMonsterSkillIndex[i] = pSox->GetFieldNum(szTemp1);
		sFieldNum.nMonsterSkillLevel[i] = pSox->GetFieldNum(szTemp2);
	}

	//##################################################################
	// Load
	//##################################################################

	for (int i = 0; i < pSox->GetItemCount(); ++i)
	{
		int nItemID = pSox->GetItemID(i);
		int nIdx = pSox->GetIDXprimary(nItemID);

		TMonsterMutationSkill Skill;
		memset(&Skill, 0, sizeof(TMonsterMutationSkill));

		Skill.nMonsterID = nItemID;
		Skill.nMonsterIdx = nIdx;
		for (int j = 0; j < PvPCommon::Common::MonsterMutationSkillColCount; j++)
		{
			Skill.nMonsterSkillIndex[j] = pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nMonsterSkillIndex[j])->GetInteger();
			Skill.nMonsterSkillLevel[j] = pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nMonsterSkillLevel[j])->GetInteger();
		}
		
		m_VecMonsterMutationSkill.push_back(Skill);
	}
	return true;
}

bool CDNGameDataManager::LoadReputeBenefitData()
{
#ifdef _WORK
	DNTableFileFormat* pSox;
	if (m_bAllLoaded)
		pSox = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TREPUTEBENEFIT );
	else
		pSox = GetDNTable( CDnTableDB::TREPUTEBENEFIT );
#else	//#ifdef _WORK
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TREPUTEBENEFIT );
#endif		//#ifdef _WORK
	if( !pSox )
	{
		g_Log.Log( LogType::_FILELOG, L"ReputeBenefit.dnt failed\r\n" );
		return false;
	}

	if( pSox->GetItemCount() <= 0 )
	{
		g_Log.Log( LogType::_FILELOG, L"ReputeBenefit.dnt Count(%d)\r\n", pSox->GetItemCount() );
		return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		m_VecUnionReputeBenefitData.clear();
	}
#endif		//#ifdef _WORK

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{
		int nPeriod;
		int nBenefitType[NpcReputation::Common::MaxBenefitCount];
		int nBenefitNum[NpcReputation::Common::MaxBenefitCount];
	};

	TempFieldNum sFieldNum;	
	sFieldNum.nPeriod = pSox->GetFieldNum("_Period");

	char szTemp1[32];
	char szTemp2[32];
	for (int i=0; i< NpcReputation::Common::MaxBenefitCount; i++)
	{
		sprintf_s( szTemp1, "_Benefit%d", i+1 );
		sprintf_s( szTemp2, "_Benefit%d_Num", i+1 );

		sFieldNum.nBenefitType[i] = pSox->GetFieldNum(szTemp1);
		sFieldNum.nBenefitNum[i] = pSox->GetFieldNum(szTemp2);
	}

	//##################################################################
	// Load
	//##################################################################


	std::vector<int> vAbuseItemID;
	for (int i=0; i<pSox->GetItemCount(); ++i)
	{
		TUnionReputeBenefitData Benefit;
		memset(&Benefit, 0, sizeof(TUnionReputeBenefitData));

		Benefit.nItemID = pSox->GetItemID(i);
		int nIdx = pSox->GetIDXprimary(Benefit.nItemID);
		
		Benefit.nPeriod = pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nPeriod)->GetInteger();

		int nMaxSellRate = 0;
		int nMaxBuyRate = 0;
		for (int j = 0; j < NpcReputation::Common::MaxBenefitCount; j++)
		{
			Benefit.nBenefitType[j] = static_cast<TStoreBenefitData::eType>(pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nBenefitType[j])->GetInteger());
			Benefit.nBenefitNum[j] = pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nBenefitNum[j])->GetInteger();

			if (Benefit.nBenefitType[j] == (int)NpcReputation::StoreBenefit::BuyingPriceDiscount)
				nMaxBuyRate = Benefit.nBenefitNum[j];

			if (Benefit.nBenefitType[j] == (int)NpcReputation::StoreBenefit::SellingPriceUp)
				nMaxSellRate = Benefit.nBenefitNum[j];
		}

		if (nMaxBuyRate + nMaxSellRate >= MAX_ITEMPRICE_RATE * 100)
			vAbuseItemID.push_back(Benefit.nItemID);

		m_VecUnionReputeBenefitData.push_back(Benefit);
	}

	if (vAbuseItemID.size() > 0)
	{
		WCHAR szTemp[1024];
		std::wstring szError = L"Invalid ReputeBenefit\n\n";

		for(UINT i=0; i<vAbuseItemID.size(); i++)
		{
			swprintf_s( szTemp, L"ItemID : %d\n", vAbuseItemID[i] );
			szError += szTemp;
		}

		MessageBox( NULL, szError.c_str(), L"Critical Error!!", MB_OK );
		return false;
	}

	return true;
}

TUnionReputeBenefitData* CDNGameDataManager::GetUnionReputeBenefitByItemID(int nItemID)
{
	TVecUnionReputeBenefitData::iterator ii;
	for(ii = m_VecUnionReputeBenefitData.begin(); ii != m_VecUnionReputeBenefitData.end(); ii++)
	{
		if((*ii).nItemID == nItemID)
		{
			return &(*ii);
		}
	}
	return NULL;
}

bool CDNGameDataManager::LoadGuildMarkData()
{
#ifdef _WORK
	DNTableFileFormat* pSox;
	if (m_bAllLoaded)
		pSox = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TGUILDMARK );
	else
		pSox = GetDNTable( CDnTableDB::TGUILDMARK );
#else		//#ifdef _WORK
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TGUILDMARK );
#endif		//#ifdef _WORK
	if( !pSox )
	{
		g_Log.Log( LogType::_FILELOG, L"GuildmarkTable.dnt failed\r\n" );
		return false;
	}

	if( pSox->GetItemCount() <= 0 )
	{
		g_Log.Log( LogType::_FILELOG, L"GuildmarkTable.dnt Count(%d)\r\n", pSox->GetItemCount() );
		return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		m_VecGuildMarkData.clear();
	}
#endif		//#ifdef _WORK

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{
		int nType;
		int nCash;
		int nMarkOnly;
		int nMarkView;
	};

	TempFieldNum sFieldNum;	
	sFieldNum.nType = pSox->GetFieldNum("_Type");
	sFieldNum.nCash = pSox->GetFieldNum("_IsCash");
	sFieldNum.nMarkOnly = pSox->GetFieldNum("_MarkOnly");
	sFieldNum.nMarkView = pSox->GetFieldNum("_MarkView");


	//##################################################################
	// Load
	//##################################################################

	for (int i=0; i<pSox->GetItemCount(); ++i)
	{
		int nItemID = pSox->GetItemID(i);
		int nIdx = pSox->GetIDXprimary(nItemID);

		TGuildMarkData Mark;
		memset(&Mark, 0, sizeof(TGuildMarkData));

		Mark.nItemID = nItemID;
		Mark.nType = pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nType)->GetInteger();
		Mark.bCash = pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nCash)->GetInteger() ? true : false;
		Mark.bMarkOnly = pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nMarkOnly)->GetInteger() ? true : false;
		Mark.bMarkView = pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nMarkView)->GetInteger() ? true : false;
		
		m_VecGuildMarkData.push_back(Mark);
	}

	return true;
}

int CDNGameDataManager::IsValidCompleteMark(int nItemID, int nType)
{
	TVecGuildMarkData::iterator ii;
	for(ii = m_VecGuildMarkData.begin(); ii != m_VecGuildMarkData.end(); ii++)
	{
		if((*ii).nItemID == nItemID && (*ii).nType == nType)
			return(int)((*ii).bMarkOnly);
	}

	return -1;
}

bool CDNGameDataManager::IsValidGuildMark(int nItemID, bool bCheckCash)
{
	TVecGuildMarkData::iterator ii;
	for(ii = m_VecGuildMarkData.begin(); ii != m_VecGuildMarkData.end(); ii++)
	{
		if((*ii).nItemID == nItemID)
		{
			if(!(*ii).bMarkView)
				return false;

			if(bCheckCash)
			{
				// 캐쉬체크해서 해당 아이템이 캐쉬일 경우 FALSE 처리
				if((*ii).bCash) return false; 
				else return true;
			}
			else
				return true;
			
		}
	}
	return false;
}

bool CDNGameDataManager::LoadPlayerCommonLeaveTable()
{
#ifdef _WORK
	DNTableFileFormat* pSox;
	if (m_bAllLoaded)
		pSox = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TPLAYERCOMMONLEVEL );
	else
		pSox = GetDNTable( CDnTableDB::TPLAYERCOMMONLEVEL );
#else	//#ifdef _WORK
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TPLAYERCOMMONLEVEL );
#endif		//#ifdef _WORK
	if( !pSox )
	{
		g_Log.Log( LogType::_FILELOG, L"playercommonleveltable.dnt failed\r\n" );
		return false;
	}

	if( pSox->GetItemCount() <= 0 )
	{
		g_Log.Log( LogType::_FILELOG, L"playercommonleveltable.dnt Count(%d)\r\n", pSox->GetItemCount() );
		return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		m_mPlayerCommonLevelTable.clear();
	}
#endif		//#ifdef _WORK

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{
		int FatigueRatio;
		int MasterGainExp;
		//int MasterGainItemID;
		//int MasterGainItemNum;
		int AddGainExp;
		int Defense;
		int Critical;
		int FinalDamage;
#if defined(PRE_ADD_TALISMAN_SYSTEM)
		int TalismanCost;
#endif
	};

	TempFieldNum sFieldNum;	
	sFieldNum.FatigueRatio = pSox->GetFieldNum("_Fatigueratio");
	sFieldNum.MasterGainExp = pSox->GetFieldNum("_MasterGainExp");
	//sFieldNum.MasterGainItemID = pSox->GetFieldNum("_MasterGainItem");
	//sFieldNum.MasterGainItemNum = pSox->GetFieldNum("_MasterGainItemNum");
	sFieldNum.AddGainExp = pSox->GetFieldNum("_AddGainExp");
	sFieldNum.Defense = pSox->GetFieldNum( "_Cdefense" );
	sFieldNum.Critical = pSox->GetFieldNum( "_Ccritical" );
	sFieldNum.FinalDamage = pSox->GetFieldNum( "_Cfinaldamage" );
#if defined(PRE_ADD_TALISMAN_SYSTEM)
	sFieldNum.TalismanCost = pSox->GetFieldNum( "_TalismanCost" );
#endif


	//##################################################################
	// Load
	//##################################################################

	for (int i=0; i<pSox->GetItemCount(); ++i)
	{
		int nItemID = pSox->GetItemID(i);
		int nIdx = pSox->GetIDXprimary(nItemID);

		TPlayerCommonLevelTableInfo Data;
		memset(&Data, 0, sizeof(Data));

		Data.iFatigue10000Ratio = pSox->GetFieldFromLablePtr(nIdx, sFieldNum.FatigueRatio)->GetInteger();
		Data.fMasterGainExp = pSox->GetFieldFromLablePtr(nIdx, sFieldNum.MasterGainExp)->GetFloat();		
		//Data.nMasterGainItemID = pSox->GetFieldFromLablePtr(nIdx, sFieldNum.MasterGainItemID)->GetInteger();
		//Data.nMasterGainItemCount = pSox->GetFieldFromLablePtr(nIdx, sFieldNum.MasterGainItemNum)->GetInteger();
		Data.fAddGainExp = pSox->GetFieldFromLablePtr(nIdx, sFieldNum.AddGainExp)->GetFloat();
		if(Data.fAddGainExp < 1)
		{
			//혹시 모르니 값이 1보다 작으면 오류 출력(혹시 1보다 작은 값이 적용되어야 하는 경우에는 클라이언트 CDnStageClearReportDlg::Process 함수의 PHASE03_RANK_START부분도 수정해야함)
			g_Log.Log( LogType::_FILELOG, L"playercommonleveltable.dnt _AddGainExp Error\r\n" );
			return false;
		}

		Data.fDefense = pSox->GetFieldFromLablePtr( nIdx, sFieldNum.Defense )->GetFloat();
		Data.fCritical = pSox->GetFieldFromLablePtr( nIdx, sFieldNum.Critical )->GetFloat();
		Data.fFinalDamage = pSox->GetFieldFromLablePtr( nIdx, sFieldNum.FinalDamage )->GetFloat();
#if defined(PRE_ADD_TALISMAN_SYSTEM)
		sFieldNum.TalismanCost = pSox->GetFieldNum( "_TalismanCost" );
		Data.iTalismanCost = pSox->GetFieldFromLablePtr( nIdx, sFieldNum.TalismanCost )->GetInteger();
#endif

		if (nItemID > m_nMaxPlayerLevel)
			m_nMaxPlayerLevel = nItemID;
		m_mPlayerCommonLevelTable.insert( std::make_pair(nItemID,Data) );
	}

	return true;
}

int CDNGameDataManager::GetFatigue10000RatioFromPlayerCommonLevelTable( int iLevel )
{
	std::map<int,TPlayerCommonLevelTableInfo>::iterator itor = m_mPlayerCommonLevelTable.find( iLevel );
	if( itor != m_mPlayerCommonLevelTable.end() )
		return(*itor).second.iFatigue10000Ratio;

	return 0;
}

TPlayerCommonLevelTableInfo* CDNGameDataManager::GetPlayerCommonLevelTable(int nLevel)
{
	std::map<int,TPlayerCommonLevelTableInfo>::iterator itor = m_mPlayerCommonLevelTable.find( nLevel );
	if( itor != m_mPlayerCommonLevelTable.end() )
		return &(*itor).second;

	return NULL;
}

#if defined(PRE_ADD_TALISMAN_SYSTEM)
int CDNGameDataManager::GetTalismanCostFromPlayerCommonLevelTable( int iLevel )
{
	std::map<int,TPlayerCommonLevelTableInfo>::iterator itor = m_mPlayerCommonLevelTable.find( iLevel );
	if( itor != m_mPlayerCommonLevelTable.end() )
		return(*itor).second.iTalismanCost;

	return 0;
}
#endif

bool CDNGameDataManager::LoadMasterSysFeelTable()
{
#ifdef _WORK
	DNTableFileFormat* pSox;
	if (m_bAllLoaded)
		pSox = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TMASTERSYSFEEL );
	else
		pSox = GetDNTable( CDnTableDB::TMASTERSYSFEEL );
#else		//#ifdef _WORK
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TMASTERSYSFEEL );
#endif		//#ifdef _WORK
	if( !pSox )
	{
		g_Log.Log( LogType::_FILELOG, L"mastersysfeeltable.dnt failed\r\n" );
		return false;
	}

	if( pSox->GetItemCount() <= 0 )
	{
		g_Log.Log( LogType::_FILELOG, L"mastersysfeeltable.dnt Count(%d)\r\n", pSox->GetItemCount() );
		return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		m_vMasterSysFeelTableInfo.clear();
	}
#endif		//#ifdef _WORK

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{
		int MinFeel;
		int MaxFeel;
		int AddExp;
	};

	TempFieldNum sFieldNum;	
	sFieldNum.MinFeel = pSox->GetFieldNum("_MinFeel");
	sFieldNum.MaxFeel = pSox->GetFieldNum("_MaxFeel");
	sFieldNum.AddExp = pSox->GetFieldNum("_AddExp");


	//##################################################################
	// Load
	//##################################################################

	for (int i=0; i<pSox->GetItemCount(); ++i)
	{
		int nItemID = pSox->GetItemID(i);
		int nIdx = pSox->GetIDXprimary(nItemID);

		TMasterSysFeelTableInfo Data;
		memset(&Data, 0, sizeof(Data));

		Data.nMinFeel = pSox->GetFieldFromLablePtr(nIdx, sFieldNum.MinFeel)->GetInteger();
		Data.nMaxFeel = pSox->GetFieldFromLablePtr(nIdx, sFieldNum.MaxFeel)->GetInteger();
		Data.nAddExp = pSox->GetFieldFromLablePtr(nIdx, sFieldNum.AddExp)->GetInteger();	

		m_vMasterSysFeelTableInfo.push_back(Data);
	}
	return true;
}

int CDNGameDataManager::GetMasterSysFeelAddExp(int nFavor)
{	
	std::vector <TMasterSysFeelTableInfo>::iterator ii;
	for(ii = m_vMasterSysFeelTableInfo.begin(); ii != m_vMasterSysFeelTableInfo.end(); ii++)
	{
		if((*ii).nMinFeel <= nFavor && (*ii).nMaxFeel >= nFavor )
			return(*ii).nAddExp;
	}
	return 0;
}

bool CDNGameDataManager::LoadGlobalEventData()
{
#ifdef _WORK
	DNTableFileFormat* pSox;
	if (m_bAllLoaded)
		pSox = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TGLOBALEVENTQUEST );
	else
		pSox = GetDNTable( CDnTableDB::TGLOBALEVENTQUEST );
#else		//#ifdef _WORK
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TGLOBALEVENTQUEST );
#endif		//#ifdef _WORK
	if( !pSox )
	{
		g_Log.Log( LogType::_FILELOG, L"globalevent.dnt failed\r\n" );
		return false;
	}

	if( pSox->GetItemCount() <= 0 )
	{
		g_Log.Log( LogType::_FILELOG, L"globalevent.dnt Count(%d)\r\n", pSox->GetItemCount() );
		return true;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		m_VecGlobalEvent.clear();
	}
#endif		//#ifdef _WORK

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{
		int nScheduleID;
		int nCollectItemID;
		int nCollectTotalCount;
		int nCollectStartDay;
		int nCollectEndDay;
		int nRewardStartDay;
		int nNoticeCount[MAX_NOTICE_WORLDEVENT];

	};

	char* szCollectStartDate;
	char* szCollectEndDate;
	char* szRewardDate;

	TempFieldNum sFieldNum;	
	sFieldNum.nScheduleID = pSox->GetFieldNum("_ScheduleID");
	sFieldNum.nCollectItemID = pSox->GetFieldNum("_CollectItemID");
	sFieldNum.nCollectTotalCount = pSox->GetFieldNum("_CollectTotalCount");
	sFieldNum.nCollectStartDay = pSox->GetFieldNum("_CollectStartDay");
	sFieldNum.nCollectEndDay = pSox->GetFieldNum("_CollectEndDay");
	sFieldNum.nRewardStartDay = pSox->GetFieldNum("_RewardStartDay");

	char szTemp[32];
	for (int i=0; i<MAX_NOTICE_WORLDEVENT; i++)
	{
		sprintf_s( szTemp, "_NoticeCount%d", i+1 );
		sFieldNum.nNoticeCount[i] = pSox->GetFieldNum(szTemp);
	}
	
	//##################################################################
	// Load
	//##################################################################

	for (int i=0; i<pSox->GetItemCount(); ++i)
	{
		int nItemID = pSox->GetItemID(i);
		int nIdx = pSox->GetIDXprimary(nItemID);

		TGlobalEventData Event;
		memset(&Event, 0, sizeof(TGlobalEventData));

		Event.nItemID = nItemID;
		Event.nScheduleID = pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nScheduleID)->GetInteger();
		Event.nCollectItemID = pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nCollectItemID)->GetInteger();
		Event.nCollectTotalCount = pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nCollectTotalCount)->GetInteger();

		szCollectStartDate = pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nCollectStartDay)->GetString();
		szCollectEndDate = pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nCollectEndDay)->GetString();
		szRewardDate = pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nRewardStartDay)->GetString();

		for (int j=0; j<MAX_NOTICE_WORLDEVENT; j++)
		{
			Event.nNoticeCount[j] = pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nNoticeCount[j])->GetInteger();

			if(Event.nNoticeCount[j] == 0)
				Event.bCheckNotice[j] = true;

			if (Event.nCollectTotalCount < Event.nNoticeCount[j])
			{
				g_Log.Log( LogType::_FILELOG, L"globalevent.dnt _CollectItemID < _NoticeCount%d \r\n", j );
				return false;
			}
		}

		CTimeSet tCollectStartSet(szCollectStartDate, true);
		CTimeSet tCollectEndSet(szCollectEndDate, true);
		CTimeSet tRewardSet(szRewardDate, true);

		if(!tCollectStartSet.CheckIntegrity())
		{
			g_Log.Log( LogType::_FILELOG, L"globalevent.dnt _CollectStartDay Invalid\r\n" );
			return false;
		}

		if(!tCollectEndSet.CheckIntegrity())
		{
			g_Log.Log( LogType::_FILELOG, L"globalevent.dnt _CollectEndDay Invalid\r\n" );
			return false;
		}

		if(!tRewardSet.CheckIntegrity())
		{
			g_Log.Log( LogType::_FILELOG, L"globalevent.dnt _RewardStartDay Invalid\r\n" );
			return false;
		}

		Event.tCollectStartDate = tCollectStartSet.GetTimeT64_LC();
		Event.tCollectEndDate = tCollectEndSet.GetTimeT64_LC();
		Event.tRewardDate = tRewardSet.GetTimeT64_LC();

		if(Event.tCollectStartDate > Event.tRewardDate || Event.tCollectStartDate > Event.tCollectEndDate)
		{
			g_Log.Log( LogType::_FILELOG, L"globalevent.dnt Time Sequence Invalid\r\n" );
			return false;
		}

		m_VecGlobalEvent.push_back(Event);
	}

	return true;
}

TGlobalEventData* CDNGameDataManager::GetGlobalEventData(int nScheduleID)
{
	TVecGlobalEvent::iterator ii;
	for(ii = m_VecGlobalEvent.begin(); ii != m_VecGlobalEvent.end(); ii++)
	{
		if((*ii).nScheduleID == nScheduleID)
			return &(*ii);
	}

	return NULL;
}

bool CDNGameDataManager::LoadEveryDayEventData()
{
#ifdef _WORK
	DNTableFileFormat* pSox;
	if (m_bAllLoaded)
		pSox = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TATTENDANCEEVENT );
	else
		pSox = GetDNTable( CDnTableDB::TATTENDANCEEVENT );
#else		//#ifdef _WORK
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TATTENDANCEEVENT );
#endif		//#ifdef _WORK
	if( !pSox )
	{
		g_Log.Log( LogType::_FILELOG, L"EverydayEventTable.dnt failed\r\n" );
		return false;
	}

	if( pSox->GetItemCount() <= 0 )
	{
		g_Log.Log( LogType::_FILELOG, L"EverydayEventTable.dnt Count(%d)\r\n", pSox->GetItemCount() );
		return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		m_VecEveryDayEvent.clear();
	}

#endif		//#ifdef _WORK

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{
		int _nEventOnOff;
		int _nStartDate;
		int _nEndDate;
		int _nSquenceDate;
		int _MailID;
		int _CashMailID;
		int _Number;
	};

	TempFieldNum sFieldNum;
	sFieldNum._nEventOnOff = pSox->GetFieldNum("_ON");
	sFieldNum._nStartDate = pSox->GetFieldNum("_Start_Date");
	sFieldNum._nEndDate = pSox->GetFieldNum("_End_Date");
	sFieldNum._nSquenceDate = pSox->GetFieldNum("_Date_Count");
	sFieldNum._MailID = pSox->GetFieldNum("_MailID");
	sFieldNum._CashMailID = pSox->GetFieldNum("_CashMailID");
	sFieldNum._Number = pSox->GetFieldNum("_Number");

	int nPrevNumber = 0;	//이전 회차와 구분해서 두개의 회차가 있을경우 오류 출력

	//##################################################################
	// Load
	//##################################################################

	for (int i=0; i<pSox->GetItemCount(); ++i)
	{
		int nItemID = pSox->GetItemID(i);
		int nIdx = pSox->GetIDXprimary(nItemID);

		TEveryDayEventData Event;
		memset(&Event, 0, sizeof(TEveryDayEventData));
		
		Event.nEventID = nItemID;
		Event.nEventOnOff = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._nEventOnOff)->GetInteger();
		if( !Event.nEventOnOff )
			continue;
		char* szStartDate = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._nStartDate)->GetString();
		char* szEndDate = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._nEndDate)->GetString();
		Event.nSquenceDateCount = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._nSquenceDate)->GetInteger();
		Event.nMailID			= pSox->GetFieldFromLablePtr( nIdx, sFieldNum._MailID )->GetInteger();
		Event.nCashMailID		= pSox->GetFieldFromLablePtr( nIdx, sFieldNum._CashMailID )->GetInteger();
		Event.nNumber		= pSox->GetFieldFromLablePtr( nIdx, sFieldNum._Number )->GetInteger();

		if(nPrevNumber == 0)
			nPrevNumber = Event.nNumber;
		else if(nPrevNumber != Event.nNumber)
		{
			//활성화된 이벤트가 두개인 경우 오류 출력
			g_Log.Log( LogType::_FILELOG, L"everydayeventtable.dnt Active Number Error\r\n" );
			return false;
		}

		CTimeSet tCollectStartSet(szStartDate, true);
		CTimeSet tCollectEndSet(szEndDate, true);

		if(!tCollectStartSet.CheckIntegrity())
		{
			g_Log.Log( LogType::_FILELOG, L"everydayeventtable.dnt _CollectStartDay Invalid\r\n" );
			return false;
		}

		if(!tCollectEndSet.CheckIntegrity())
		{
			g_Log.Log( LogType::_FILELOG, L"everydayeventtable.dnt _CollectEndDay Invalid\r\n" );
			return false;
		}

		Event.tStartDate = tCollectStartSet.GetTimeT64_LC();
		Event.tEndDate = tCollectEndSet.GetTimeT64_LC();
		
 		m_VecEveryDayEvent.push_back(Event);
	}

	return true;
}

TEveryDayEventData* CDNGameDataManager::GetEveryDayEventRewardMailID(int iOngoingDay)
{
	TVecEveryDayEvent::iterator itor;
	CTimeSet tPresentDate;
	__time64_t tChectDate = tPresentDate.GetTimeT64_LC();

	for(itor = m_VecEveryDayEvent.begin(); itor != m_VecEveryDayEvent.end(); itor++)
	{
		//현재 tStartDated와 tEndDate 체크도 추가		
		if((*itor).nEventOnOff && tChectDate >= (*itor).tStartDate && tChectDate <= (*itor).tEndDate )		
		{
			if((*itor).nSquenceDateCount == iOngoingDay )
			{
				return &(*itor);
			}
		}
	}

	return NULL;
}

bool CDNGameDataManager::CheckEveryDayEventTime(WORD *index)
{
	TVecEveryDayEvent::iterator itor;
	for(itor = m_VecEveryDayEvent.begin(); itor != m_VecEveryDayEvent.end(); itor++)
	{		
		if((*itor).nEventOnOff && *index == (*itor).nNumber )		
		{
			return true;
		}
		else
		{
			*index = (*itor).nNumber;
			return false;
		}
	}	
	return false;
}

#ifdef PRE_FIX_MEMOPT_EXT
char* CDNGameDataManager::GetFileNameFromFileEXT(DNTableFileFormat* pRawSox, int itemId, const char* pFieldName, DNTableFileFormat* pFileNameSox)
{
	if(pRawSox == NULL || pFieldName == NULL || pFieldName[0] == '\0')
		return NULL;

	DNTableCell* pNameField = pRawSox->GetFieldFromLablePtr( itemId, pFieldName );
	return(pNameField != NULL) ? GetFileNameFromFileEXT(pNameField->GetInteger(), pFileNameSox) : NULL;
}

void CDNGameDataManager::GetFileNameFromFileEXT(std::string& result, DNTableFileFormat* pRawSox, int itemId, const char* pFieldName, DNTableFileFormat* pFileNameSox)
{
	if(pRawSox == NULL || pFieldName == NULL || pFieldName[0] == '\0')
		return;

	DNTableCell* pNameField = pRawSox->GetFieldFromLablePtr( itemId, pFieldName );
	if(pNameField != NULL)
	{
		char* pFileName = GetFileNameFromFileEXT(pNameField->GetInteger(), pFileNameSox);
		result = (pFileName) ? pFileName : "";
	}
	else
	{
		result = "";
	}
}

void CDNGameDataManager::GetFileNameFromFileEXT(std::string& result, DNTableFileFormat* pRawSox, int itemId, int fieldIndex, DNTableFileFormat* pFileNameSox)
{
	if(pRawSox == NULL)
		return;

	DNTableCell* pNameField = pRawSox->GetFieldFromLablePtr( itemId, fieldIndex );
	if(pNameField != NULL)
	{
		char* pFileName = GetFileNameFromFileEXT(pNameField->GetInteger(), pFileNameSox);
		result = (pFileName) ? pFileName : "";
	}
	else
	{
		result = "";
	}
}

char* CDNGameDataManager::GetFileNameFromFileEXT(int index, DNTableFileFormat* pFileSox)
{
	if(pFileSox == NULL)
	{
		pFileSox = GetDNTable( CDnTableDB::TFILE );
		if(pFileSox == NULL)
		{
			_ASSERT(0);
			return NULL;
		}
	}

	return pFileSox->GetFieldFromLablePtr(index, "_FileName")->GetString();
}
#endif

bool CDNGameDataManager::bIsGuildReversionItem( int iItemID )
{
	TMapGuildReversionTableData::iterator itor = m_GuildReversionTableData.find( iItemID );
	if( itor == m_GuildReversionTableData.end() )
		return false;
	return true;
}

#if defined( PRE_PARTY_DB )

bool CDNGameDataManager::LoadPartySortWeightTable()
{
#ifdef _WORK
	DNTableFileFormat* pSox;
	if (m_bAllLoaded)
		pSox = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TPARTYSORTWEIGHT );
	else
		pSox = GetDNTable( CDnTableDB::TPARTYSORTWEIGHT );
#else		//#ifdef _WORK
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TPARTYSORTWEIGHT );
#endif		//#ifdef _WORK
	if( !pSox )
	{
		g_Log.Log( LogType::_FILELOG, L"partylistsortweight.dnt failed\r\n" );
		return false;
	}

	if( pSox->GetItemCount() <= 0 )
	{
		g_Log.Log( LogType::_FILELOG, L"partylistsortweight.dnt Count(%d)\r\n", pSox->GetItemCount() );
		return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		m_PartySortWeight.clear();
	}

#endif		//#ifdef _WORK

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{		
		int _nWeight;
	};

	TempFieldNum sFieldNum;	
	sFieldNum._nWeight = pSox->GetFieldNum("_Weight");

	//##################################################################
	// Load
	//##################################################################

	for (int i=0; i<pSox->GetItemCount(); ++i)
	{
		int nItemID = pSox->GetItemID(i);
		int nIdx = pSox->GetIDXprimary(nItemID);

		int iWeight = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._nWeight)->GetInteger();

		m_PartySortWeight.insert( std::make_pair(static_cast<Party::SortWeightIndex::eCode>(nItemID),iWeight) );
	}

	return true;
}

int CDNGameDataManager::GetPartySortWeight( Party::SortWeightIndex::eCode Type )
{
	TMapPartySortWeight::iterator itor = m_PartySortWeight.find( Type );
	if( itor != m_PartySortWeight.end() )
		return (*itor).second;
	return 0;
}

bool CDNGameDataManager::LoadDungeonSortWeightTable()
{
#ifdef _WORK
	DNTableFileFormat* pSox;
	if (m_bAllLoaded)
		pSox = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TDUNGEONENTER );
	else
		pSox = GetDNTable( CDnTableDB::TDUNGEONENTER );
#else		//#ifdef _WORK
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TDUNGEONENTER );
#endif		//#ifdef _WORK
	if( !pSox )
	{
		g_Log.Log( LogType::_FILELOG, L"stageentertable.dnt failed\r\n" );
		return false;
	}

	if( pSox->GetItemCount() <= 0 )
	{
		g_Log.Log( LogType::_FILELOG, L"stageentertable.dnt Count(%d)\r\n", pSox->GetItemCount() );
		return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		m_DungeonSortWeight.clear();
	}

#endif		//#ifdef _WORK

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{		
		int _ListSort;
	};

	TempFieldNum sFieldNum;	
	sFieldNum._ListSort = pSox->GetFieldNum("_ListSort");

	//##################################################################
	// Load
	//##################################################################

	for (int i=0; i<pSox->GetItemCount(); ++i)
	{
		int nItemID = pSox->GetItemID(i);
		int nIdx = pSox->GetIDXprimary(nItemID);

		int iWeight = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._ListSort)->GetInteger();

		m_DungeonSortWeight.insert( std::make_pair(nItemID,iWeight) );
	}

	return true;
}

int CDNGameDataManager::GetDungeonSortWeight( int iMapindex )
{
	const TMapInfo* pMapInfo = GetMapInfo( iMapindex );
	if( pMapInfo == NULL )
		return 0;
	
	TMapDungeonSortWeight::iterator itor = m_DungeonSortWeight.find( pMapInfo->iEnterConditionTable );
	if( itor != m_DungeonSortWeight.end() )
		return (*itor).second;
	return 0;
}

#endif // #if defined( PRE_PARTY_DB )

bool CDNGameDataManager::LoadGuildRewardItemData()
{
#ifdef _WORK
	DNTableFileFormat* pSox;
	if (m_bAllLoaded)
		pSox = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TGUILDREWARDITEM );
	else
		pSox = GetDNTable( CDnTableDB::TGUILDREWARDITEM );
#else		//#ifdef _WORK
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TGUILDREWARDITEM );
#endif		//#ifdef _WORK
	if( !pSox )
	{
		g_Log.Log( LogType::_FILELOG, L"guildrewardtable.dnt failed\r\n" );
		return false;
	}

	if( pSox->GetItemCount() <= 0 )
	{
		g_Log.Log( LogType::_FILELOG, L"guildrewardtable.dnt Count(%d)\r\n", pSox->GetItemCount() );
		return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		m_mGuildRewardItemTableData.clear();
	}

#endif		//#ifdef _WORK

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{		
		int _nNeedGuildLevel;
		int _nNeedGold;
		int _nItemType;
		int _nItemTypeParam1;
		int _nItemTypeParam2;
		int _nCheckInven;
		int _nPeriod;
		int _nCheckMaster;
		int _nCheckType;
		int _nCheckID;
	};

	TempFieldNum sFieldNum;	
	sFieldNum._nNeedGuildLevel = pSox->GetFieldNum("_NeedGuildLevel");
	sFieldNum._nNeedGold = pSox->GetFieldNum("_NeedGold");
	sFieldNum._nItemType = pSox->GetFieldNum("_Type");
	sFieldNum._nItemTypeParam1 = pSox->GetFieldNum("_TypeParam1");
	sFieldNum._nItemTypeParam2 = pSox->GetFieldNum("_TypeParam2");	
	sFieldNum._nCheckInven = pSox->GetFieldNum("_CheckInven");	
	sFieldNum._nPeriod = pSox->GetFieldNum("_Period");
	sFieldNum._nCheckMaster = pSox->GetFieldNum("_GuildMasterLimit");	
	sFieldNum._nCheckType = pSox->GetFieldNum("_PrerequisiteType");	
	sFieldNum._nCheckID = pSox->GetFieldNum("_PrerequisiteID");	

	//##################################################################
	// Load
	//##################################################################

	for (int i=0; i<pSox->GetItemCount(); ++i)
	{
		int nItemID = pSox->GetItemID(i);
		int nIdx = pSox->GetIDXprimary(nItemID);

		TGuildRewardItemData GuildRewardItem;
		memset(&GuildRewardItem, 0, sizeof(GuildRewardItem));

		GuildRewardItem.nItemID = nItemID;
		GuildRewardItem.nNeedGuildLevel = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._nNeedGuildLevel)->GetInteger();
		GuildRewardItem.nNeedGold = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._nNeedGold)->GetInteger();
		GuildRewardItem.nItemType = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._nItemType)->GetInteger();
		GuildRewardItem.nTypeParam1 = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._nItemTypeParam1)->GetInteger();
		GuildRewardItem.nTypeParam2 = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._nItemTypeParam2)->GetInteger();
		GuildRewardItem.bCheckInven = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._nCheckInven)->GetInteger() ? true : false;
		GuildRewardItem.bEternity = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._nPeriod)->GetInteger() ? false : true;
		GuildRewardItem.nPeriod = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._nPeriod)->GetInteger();
		GuildRewardItem.bCheckMaster = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._nCheckMaster)->GetInteger() ? true : false;
		GuildRewardItem.nCheckType = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._nCheckType)->GetInteger();
		GuildRewardItem.nCheckID = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._nCheckID)->GetInteger();
		
		m_mGuildRewardItemTableData.insert( make_pair(nItemID, GuildRewardItem) );
	}

	return true;
}
TGuildRewardItemData* CDNGameDataManager::GetGuildRewardItemData(int nItemID)
{
	TMapGuildRewardItem::iterator itor = m_mGuildRewardItemTableData.find( nItemID );
	if( itor != m_mGuildRewardItemTableData.end() )
		return &itor->second;
	return NULL;
}

#if defined( PRE_ADD_SALE_COUPON)
bool CDNGameDataManager::LoadSaleCouponData()
{	
#ifdef _WORK
	DNTableFileFormat* pSox;
	if (m_bAllLoaded)
		pSox = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TSALECOUPON );
	else
		pSox = GetDNTable( CDnTableDB::TSALECOUPON );
#else		//#ifdef _WORK
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TSALECOUPON );
#endif		//#ifdef _WORK
	if( !pSox )
	{
		g_Log.Log( LogType::_FILELOG, L"coupon.dnt failed\r\n" );
		return false;
	}

	if( pSox->GetItemCount() <= 0 )
	{
		g_Log.Log( LogType::_FILELOG, L"coupon.dnt Count(%d)\r\n", pSox->GetItemCount() );
		return true;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		m_mSaleCouponItemTableData.clear();
	}

#endif		//#ifdef _WORK

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{		
		int _nItemID;
		int _nUseItemSN[MAX_SALE_USEITEM];
	};

	TempFieldNum sFieldNum;	
	memset(&sFieldNum, 0, sizeof(sFieldNum));
	sFieldNum._nItemID = pSox->GetFieldNum("_ID");

	char szTemp[256];
	memset( szTemp, 0, sizeof(szTemp) );
	for( int i=0; i<MAX_SALE_USEITEM; ++i)
	{
		sprintf_s( szTemp, "_UseItem%02d", i+1 );
		sFieldNum._nUseItemSN[i] = pSox->GetFieldNum(szTemp);
	}

	//##################################################################
	// Load
	//##################################################################

	for (int i=0; i<pSox->GetItemCount(); ++i)
	{
		int nItemID = pSox->GetItemID(i);
		int nIdx = pSox->GetIDXprimary(nItemID);

		TSaleCouponData SaleCoupon;
		memset(&SaleCoupon, 0, sizeof(SaleCoupon));

		SaleCoupon.nItemID = nItemID;
		for( int j=0; j<MAX_SALE_USEITEM; ++j)
		{
			SaleCoupon.nUseItemSN[j] = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._nUseItemSN[j])->GetInteger();
		}
		m_mSaleCouponItemTableData.insert( make_pair(nItemID, SaleCoupon) );
	}
	return true;
}
TSaleCouponData* CDNGameDataManager::GetSaleCouponItemData(int nItemID)
{
 	TMapSaleCoupon::iterator itor = m_mSaleCouponItemTableData.find( nItemID );
	if( itor != m_mSaleCouponItemTableData.end() )
		return &itor->second;
	return NULL;
}

bool CDNGameDataManager::IsSaleCouponItemBuyItem(int nCouponItemID, int nBuyItemSN)
{
	TMapSaleCoupon::iterator itor = m_mSaleCouponItemTableData.find( nCouponItemID );
	if( itor != m_mSaleCouponItemTableData.end() )
	{
		for( int i=0; i<MAX_SALE_USEITEM; ++i)
		{
			if( itor->second.nUseItemSN[i] == nBuyItemSN )
				return true;
		}
	}	
	return false;
}
#endif

#if defined(PRE_FIX_NEXTSKILLINFO)
SKILL_LEVEL_INFO CDNGameDataManager::ms_SkillLevelTableInfo;
SKILL_LEVEL_INFO* CDNGameDataManager::GetSkillLevelTableIDList(int nSkillID, int nApplyType)
{
	if (nApplyType >= 2 || nApplyType < 0)
		return &ms_SkillLevelTableInfo;

	SKILL_LEVEL_TABLEID_LIST::iterator findIter = m_SkillLevelTableIDList[nApplyType].find(nSkillID);
	if (findIter != m_SkillLevelTableIDList[nApplyType].end())
		return &findIter->second;

	return &ms_SkillLevelTableInfo;
}

void CDNGameDataManager::InitSkillLevelTableIDList()
{
	DNTableFileFormat* pSkillTable = GetDNTable( CDnTableDB::TSKILL );
	DNTableFileFormat* pSkillLevelTable = GetDNTable( CDnTableDB::TSKILLLEVEL );

	if (pSkillTable == NULL || pSkillLevelTable == NULL)
		return;

	int nSkillCount = pSkillTable->GetItemCount();
	for (int i = 0; i < nSkillCount; ++i)
	{
		int nSkillID = pSkillTable->GetItemID(i);

		//int nSkillID = pSkillTable->GetFieldFromLablePtr(nItemID, "id")->GetInteger();

		std::vector<int> vlSkillLevelList;
		pSkillLevelTable->GetItemIDListFromField( "_SkillIndex", nSkillID, vlSkillLevelList );

		// pve, pvp 대상인지 확인하여 걸러냄.
		vector<int>::iterator iterLevelList = vlSkillLevelList.begin();
		for( iterLevelList; iterLevelList != vlSkillLevelList.end(); ++iterLevelList)
		{
			int iSkillLevelTableID = *iterLevelList;
			int iApplyType = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, "_ApplyType" )->GetInteger();
			int iSkillLevel = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, "_SkillLevel" )->GetInteger();

			AddSkillLevelTableID(nSkillID, iSkillLevel, iSkillLevelTableID, iApplyType);
		}
	}
}

void CDNGameDataManager::AddSkillLevelTableID(int nSkillID, int nSkillLevel, int nSkillLevelTableID, int nApplyType)
{
	if (nApplyType >= 2 || nApplyType < 0)
		return;

	SKILL_LEVEL_TABLEID_LIST::iterator findIter = m_SkillLevelTableIDList[nApplyType].find(nSkillID);
	if (findIter != m_SkillLevelTableIDList[nApplyType].end())
	{
		//기존 리스트가 존재 하면 그 리스트에 추가 한다..
		findIter->second.AddSkillLevelTableID(nSkillLevel, nSkillLevelTableID);
	}
	else
	{
		SKILL_LEVEL_INFO skillLevelInfo;
		skillLevelInfo.AddSkillLevelTableID(nSkillLevel, nSkillLevelTableID);

		m_SkillLevelTableIDList[nApplyType].insert(std::make_pair(nSkillID, skillLevelInfo));
	}
}
#endif // PRE_FIX_NEXTSKILLINFO

#ifdef PRE_ADD_COSRANDMIX
bool CDNGameDataManager::LoadCostumeRandomMixData()
{
	return m_CosRandMixDataMgr.LoadData();
}
#endif

#if defined( PRE_ADD_NAMEDITEM_SYSTEM )
bool CDNGameDataManager::LoadNamedItemData()
{
#ifdef _WORK
	DNTableFileFormat* pSox;
	if (m_bAllLoaded)
		pSox = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TNAMEDITEM );
	else
		pSox = GetDNTable( CDnTableDB::TNAMEDITEM );
#else		//#ifdef _WORK
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TNAMEDITEM );
#endif		//#ifdef _WORK
	if( !pSox )
	{
		g_Log.Log( LogType::_FILELOG, L"nameditemtable.dnt failed\r\n" );
		return false;
	}

	if( pSox->GetItemCount() <= 0 )
	{
		g_Log.Log( LogType::_FILELOG, L"nameditemtable.dnt Count(%d)\r\n", pSox->GetItemCount() );
		return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		m_mNamedItemTableData.clear();
	}

#endif		//#ifdef _WORK

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{		
		int _Named_Max_count;
		int _NamedItem_ID;
		int _Item_ID;		
	};

	TempFieldNum sFieldNum;	
	sFieldNum._Named_Max_count = pSox->GetFieldNum("_Named_Max_count");
	sFieldNum._NamedItem_ID = pSox->GetFieldNum("_NamedItem_ID");
	sFieldNum._Item_ID = pSox->GetFieldNum("_Item_ID");
	

	//##################################################################
	// Load
	//##################################################################

	for (int i=0; i<pSox->GetItemCount(); ++i)
	{
		int nID = pSox->GetItemID(i);
		int nIdx = pSox->GetIDXprimary(nID);

		TNamedItemData NamedItem;
		memset(&NamedItem, 0, sizeof(NamedItem));
		
		NamedItem.nMaxCount = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._Named_Max_count)->GetInteger();
		NamedItem.nItemID = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._NamedItem_ID)->GetInteger();		
		NamedItem.nSwapItemID = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._Item_ID)->GetInteger();		

		m_mNamedItemTableData.insert( make_pair(NamedItem.nItemID, NamedItem) );
	}

	return true;
}

int CDNGameDataManager::GetNamedItemMaxCount( int nItemID )
{
	TMapNamedItem::iterator itor = m_mNamedItemTableData.find( nItemID );
	if( itor != m_mNamedItemTableData.end() )
		return itor->second.nMaxCount;
	return 0;
}

int CDNGameDataManager::GetNamedItemSwapID( int nItemID )
{
	TMapNamedItem::iterator itor = m_mNamedItemTableData.find( nItemID );
	if( itor != m_mNamedItemTableData.end() )
		return itor->second.nSwapItemID;
	return 0;
}
#endif

#ifdef PRE_ADD_EXCHANGE_POTENTIAL
bool CDNGameDataManager::LoadPotentialTransferData()
{
#ifdef _WORK
	DNTableFileFormat* pSox;
	if (m_bAllLoaded)
		pSox = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TPOTENTIAL_TRANS );
	else
		pSox = GetDNTable( CDnTableDB::TPOTENTIAL_TRANS );
#else		//#ifdef _WORK
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TPOTENTIAL_TRANS );
#endif		//#ifdef _WORK
	if( !pSox )
	{
		g_Log.Log( LogType::_FILELOG, L"potentialtransfertable.dnt failed\r\n" );
		return false;
	}

	if( pSox->GetItemCount() <= 0 )
	{
		g_Log.Log( LogType::_FILELOG, L"potentialtransfertable.dnt Count(%d)\r\n", pSox->GetItemCount() );
		return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		m_vPotentialTransferList.clear();
	}

#endif		//#ifdef _WORK

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{		
		int nExtractItemLevel;
		int nExtractItemRank;
		int nInjectItemLevel;
		int nInjectItemRank;
		int nConsumptionCount;
		int nMainType;
		int nSubType;
	};

	TempFieldNum sFieldNum;	
	sFieldNum.nExtractItemLevel = pSox->GetFieldNum("_FromLevel");
	sFieldNum.nExtractItemRank = pSox->GetFieldNum("_FromGrade");
	sFieldNum.nInjectItemLevel = pSox->GetFieldNum("_ToLevel");
	sFieldNum.nInjectItemRank = pSox->GetFieldNum("_ToGrade");
	sFieldNum.nConsumptionCount = pSox->GetFieldNum("_Count");
	sFieldNum.nMainType = pSox->GetFieldNum("_MainType");
	sFieldNum.nSubType = pSox->GetFieldNum("_SubType");


	//##################################################################
	// Load
	//##################################################################

	for (int i=0; i<pSox->GetItemCount(); ++i)
	{
		int nID = pSox->GetItemID(i);
		int nIdx = pSox->GetIDXprimary(nID);

		TPotentialTransferData PotentialTransferData;
		memset(&PotentialTransferData, 0, sizeof(PotentialTransferData));

		PotentialTransferData.nItemID = nID;
		PotentialTransferData.nExtractItemLevel = pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nExtractItemLevel)->GetInteger();
		PotentialTransferData.nExtractItemRank = pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nExtractItemRank)->GetInteger();
		PotentialTransferData.nInjectItemLevel = pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nInjectItemLevel)->GetInteger();
		PotentialTransferData.nInjectItemRank = pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nInjectItemRank)->GetInteger();
		PotentialTransferData.nConsumptionCount = pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nConsumptionCount)->GetInteger();
		PotentialTransferData.nMainType = pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nMainType)->GetInteger();
		PotentialTransferData.nSubType = pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nSubType)->GetInteger();

		m_vPotentialTransferList.push_back(PotentialTransferData);
	}

	return true;
}

int CDNGameDataManager::GetPotentialTransConsumptionCount(int nExtractLevel, int nExtractRank, int nInjectLevel, int nInjectRank, int nMainType, int nSubType)
{
	TVecPotentialTransferItem::iterator ii;
	for (ii = m_vPotentialTransferList.begin(); ii != m_vPotentialTransferList.end(); ii++)
	{
		if ((*ii).nExtractItemLevel == nExtractLevel && (*ii).nExtractItemRank == nExtractRank && \
			(*ii).nInjectItemLevel == nInjectLevel && (*ii).nInjectItemRank == nInjectRank && \
			(*ii).nMainType == nMainType && (*ii).nSubType == nSubType)
			return (*ii).nConsumptionCount;
	}
	return -1;
}
#endif		//#ifdef PRE_ADD_EXCHANGE_POTENTIAL

bool CDNGameDataManager::LoadCashErasableType()
{
#ifdef _WORK
	DNTableFileFormat* pSox;
	if (m_bAllLoaded)
		pSox = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TCASHERASABLETYPE );
	else
		pSox = GetDNTable( CDnTableDB::TCASHERASABLETYPE );
#else		//#ifdef _WORK
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TCASHERASABLETYPE );
#endif		//#ifdef _WORK
	if( !pSox )
	{
		g_Log.Log( LogType::_FILELOG, L"casherasabletypetable.dnt failed\r\n" );
		// return false;
	}

	if( pSox->GetItemCount() <= 0 )
	{
		g_Log.Log( LogType::_FILELOG, L"casherasabletypetable.dnt Count(%d)\r\n", pSox->GetItemCount() );
		// return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		m_VecCashErasableType.clear();
	}

#endif		//#ifdef _WORK

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{		
		int nType;
	};

	TempFieldNum sFieldNum;	
	sFieldNum.nType = pSox->GetFieldNum("_Type");

	//##################################################################
	// Load
	//##################################################################

	for (int i=0; i<pSox->GetItemCount(); ++i)
	{
		int nID = pSox->GetItemID(i);
		int nIdx = pSox->GetIDXprimary(nID);

		int nType = pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nType)->GetInteger();

		m_VecCashErasableType.push_back(nType);
	}

	return true;
}

bool CDNGameDataManager::IsCashErasableType(int nItemID)
{
	int nType = GetItemMainType(nItemID);

	if (m_VecCashErasableType.empty()) return false;

	for (int i = 0; i < (int)m_VecCashErasableType.size(); i++){
		if (nType == m_VecCashErasableType[i])
			return true;
	}

	return false;
}

#if defined( PRE_WORLDCOMBINE_PARTY )
bool CDNGameDataManager::LoadCombinePartyData()
{
#ifdef _WORK
	DNTableFileFormat* pSox;
	if (m_bAllLoaded)
		pSox = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TWORLDCOMBINEPARTY );
	else
		pSox = GetDNTable( CDnTableDB::TWORLDCOMBINEPARTY );
#else		//#ifdef _WORK
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TWORLDCOMBINEPARTY );
#endif		//#ifdef _WORK
	if( !pSox )
	{
		g_Log.Log( LogType::_FILELOG, L"nestmissionpartytable.dnt failed\r\n" );
		return false;
	}

	if( pSox->GetItemCount() <= 0 )
	{
		g_Log.Log( LogType::_FILELOG, L"nestmissionpartytable.dnt Count(%d)\r\n", pSox->GetItemCount() );
		return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		m_mWorldCombinePartyData.clear();
	}

#endif		//#ifdef _WORK

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{		
		int nGroupIndex;
		int nWorldMapID;
		int nTitle;
		int nTargetMap;
		int nMapType;
		int nMaxUser;
		int nItemID;
		int nLimitLevel;
		int nItemLootRule;
		int nClassDice;
		int nSkillID1;
		int nSkillID2;
	};

	TempFieldNum sFieldNum;	
	sFieldNum.nGroupIndex = pSox->GetFieldNum("_NestMissionPartyGroup");
	sFieldNum.nWorldMapID = pSox->GetFieldNum("_WorldMapID");
	sFieldNum.nTitle = pSox->GetFieldNum("_Title");
	sFieldNum.nTargetMap = pSox->GetFieldNum("_MapTableID");
	sFieldNum.nMapType = pSox->GetFieldNum("_MapType");
	sFieldNum.nMaxUser = pSox->GetFieldNum("_MaxUser");
	sFieldNum.nItemID = pSox->GetFieldNum("_GainItem");
	sFieldNum.nLimitLevel = pSox->GetFieldNum("_LimitLevel");
	sFieldNum.nItemLootRule = pSox->GetFieldNum("_ItemSelectDiceType");
	sFieldNum.nClassDice = pSox->GetFieldNum("_SelectClassDice");
	sFieldNum.nSkillID1 = pSox->GetFieldNum("_PartySkill1ID");
	sFieldNum.nSkillID2 = pSox->GetFieldNum("_PartySkill2ID");


	//##################################################################
	// Load
	//##################################################################

	for (int i=0; i<pSox->GetItemCount(); ++i)
	{
		WorldCombineParty::WrldCombinePartyData Data;
		memset(&Data, 0, sizeof(WorldCombineParty::WrldCombinePartyData));
		int nID = pSox->GetItemID(i);
		int nIdx = pSox->GetIDXprimary(nID);

		Data.cIndex = (BYTE)nID;
		Data.cGroupIndex = (BYTE)pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nGroupIndex)->GetInteger();
		Data.nWorldMap = pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nWorldMapID)->GetInteger();
		int nTitleID = pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nTitle)->GetInteger();
		_wcscpy(Data.wszPartyName, _countof(Data.wszPartyName), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nTitleID ), (int)wcslen(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nTitleID )) );
		Data.nTargetMap = pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nTargetMap)->GetInteger();
		Data.Difficulty = (TDUNGEONDIFFICULTY)pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nMapType)->GetInteger();
		Data.cPartyMemberMax = (BYTE)pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nMaxUser)->GetInteger();
		Data.nItemID = pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nItemID)->GetInteger();
		Data.cUserLvLimitMin = (BYTE)pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nLimitLevel)->GetInteger();
		Data.ItemLootRule = (TPARTYITEMLOOTRULE)pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nItemLootRule)->GetInteger();
		Data.nSkillID[0] = pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nSkillID1)->GetInteger();
		Data.nSkillID[1] = pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nSkillID2)->GetInteger();
		if( pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nClassDice)->GetInteger() )
			Data.iBitFlag |= Party::BitFlag::JobDice;
		
		Data.iBitFlag |= Party::BitFlag::WorldmapAllow;		
		Data.PartyType = _WORLDCOMBINE_PARTY;
		Data.ItemRank = ITEMRANK_D;
		m_mWorldCombinePartyData.insert( make_pair(Data.nTargetMap, Data) );		
	}
	return true;
}
WorldCombineParty::WrldCombinePartyData* CDNGameDataManager::GetCombinePartyData(int nPrimaryIndex)
{
	TMapWorldCombinePartyData::iterator itor = m_mWorldCombinePartyData.find(nPrimaryIndex);
	if( itor != m_mWorldCombinePartyData.end() )
		return &(itor->second);
	return NULL;
}
void CDNGameDataManager::CheckCombinePartyData(TMapWorldCombinePartyData ExistCombineParty, TMapWorldCombinePartyData &CreateCombineParty)
{
	for(TMapWorldCombinePartyData::iterator itor = m_mWorldCombinePartyData.begin();itor != m_mWorldCombinePartyData.end();itor++)
	{
		if( ExistCombineParty.find((*itor).first) == ExistCombineParty.end() )
		{
			CreateCombineParty.insert( make_pair((*itor).first, (*itor).second) );
		}
	}
}
#endif

#if defined( PRE_ADD_PCBANG_RENTAL_ITEM)
bool CDNGameDataManager::LoadPcCafeRentData()
{
#ifdef _WORK
	DNTableFileFormat* pSox;
	if (m_bAllLoaded)
		pSox = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TPCBANGRENTALITEM );
	else
		pSox = GetDNTable( CDnTableDB::TPCBANGRENTALITEM );
#else		//#ifdef _WORK
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TPCBANGRENTALITEM );
#endif		//#ifdef _WORK
	if( !pSox )
	{
		g_Log.Log( LogType::_FILELOG, L"pccaferenttable.dnt failed\r\n" );
		return false;
	}

	if( pSox->GetItemCount() <= 0 )
	{
		g_Log.Log( LogType::_FILELOG, L"pccaferenttable.dnt Count(%d)\r\n", pSox->GetItemCount() );
		return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		m_mPcBangRentItemData.clear();
	}

#endif		//#ifdef _WORK

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{		
		int nItemID;
		int nLevelStart;
		int nLevelEnd;
		int nClassID;
		int nJob;
		int nGrade;		
		int nItemOption;
	};

	TempFieldNum sFieldNum;	
	sFieldNum.nLevelStart = pSox->GetFieldNum("_PCLevelStart");
	sFieldNum.nLevelEnd = pSox->GetFieldNum("_PCLevelEnd");
	sFieldNum.nClassID = pSox->GetFieldNum("_ClassID");
	sFieldNum.nJob = pSox->GetFieldNum("_Job");
	sFieldNum.nItemID = pSox->GetFieldNum("_ItemID");
	sFieldNum.nGrade = pSox->GetFieldNum("_Grade");	
	sFieldNum.nItemOption = pSox->GetFieldNum("_ItemOption");	


	//##################################################################
	// Load
	//##################################################################

	for (int i=0; i<pSox->GetItemCount(); ++i)
	{
		TPcBangRentItem Data;
		memset(&Data, 0, sizeof(TPcBangRentItem));
		int nID = pSox->GetItemID(i);
		int nIdx = pSox->GetIDXprimary(nID);
		
		Data.nItemID = pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nItemID)->GetInteger();		
		Data.cLevelStart = (BYTE)pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nLevelStart)->GetInteger();
		Data.cLevelEnd = (BYTE)pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nLevelEnd)->GetInteger();
		Data.cClassID = (BYTE)pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nClassID)->GetInteger();
		Data.cJob = (BYTE)pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nJob)->GetInteger();
		Data.cGrade = (BYTE)pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nGrade)->GetInteger();		
		Data.nItemOption = pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nItemOption)->GetInteger();		
		m_mPcBangRentItemData.insert( make_pair(Data.nItemID, Data) );		
	}
	return true;
}

bool CDNGameDataManager::IsPcCafeRentItem(int nItemID)
{
	if( m_mPcBangRentItemData.empty() )
		return false;

	TMapPcBangRentItemData::iterator itor = m_mPcBangRentItemData.find(nItemID);
	if( itor != m_mPcBangRentItemData.end() )
		return true;
	return false;
}

int CDNGameDataManager::GetPcCafeRentItemID(BYTE cLevel, char cClassID, BYTE cJob, BYTE cPcCafeGrade, std::map<int,int> &vList )
{
	if( m_mPcBangRentItemData.empty() )
		return 0;

	int nCount = 0;
	TPcBangRentItem* pRentItem = NULL;
	for(TMapPcBangRentItemData::iterator itor = m_mPcBangRentItemData.begin(); itor != m_mPcBangRentItemData.end(); itor++)
	{		
		pRentItem = &itor->second;
		if( cLevel >= pRentItem->cLevelStart && cLevel <= pRentItem->cLevelEnd )
		{
			if( pRentItem->cClassID == 0 || pRentItem->cClassID == cClassID )
			{
				if( pRentItem->cJob == 0 || pRentItem->cJob == cJob )
				{
					if( pRentItem->cGrade == 0 || pRentItem->cGrade == cPcCafeGrade )
					{
						const TItemData* pItemData = g_pDataManager->GetItemData( pRentItem->nItemID );
						if( !pItemData )
							continue;
						vList.insert(std::make_pair(pRentItem->nItemID,pRentItem->nItemOption));						
						if( pItemData->nType == ITEMTYPE_PET || pItemData->nType == ITEMTYPE_VEHICLE ) // 펫, 탈것은 인벤토리 갯수체크 안함
							continue;
						++nCount;
					}
				}
			}
		}
	}	
	return nCount;
}

#endif //#if defined( PRE_ADD_PCBANG_RENTAL_ITEM)

#if defined(PRE_ADD_INSTANT_CASH_BUY)
bool CDNGameDataManager::LoadCashBuyShortcut()
{
#ifdef _WORK
	DNTableFileFormat* pDnt;
	if (m_bAllLoaded)
		pDnt = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TCASHBUYSHORTCUT );
	else
		pDnt = GetDNTable( CDnTableDB::TCASHBUYSHORTCUT );
#else		//#ifdef _WORK
	DNTableFileFormat* pDnt = GetDNTable( CDnTableDB::TCASHBUYSHORTCUT );
#endif		//#ifdef _WORK
	if( !pDnt )
	{
		g_Log.Log( LogType::_FILELOG, L"cashbuyshortcuttable.dnt failed\r\n" );
		return false;
	}

	if( pDnt->GetItemCount() <= 0 )
	{
		g_Log.Log( LogType::_FILELOG, L"cashbuyshortcuttable.dnt Count(%d)\r\n", pDnt->GetItemCount() );
		return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		m_MapCashBuyShortcut.clear();
	}

#endif		//#ifdef _WORK

	char szTemp[MAX_PATH] = { 0, };

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{		
		int nType;
		int nAllowMapTypes;
		int nShow;
		std::vector<int> nSN;	
	};

	TempFieldNum sFieldNum;	
	sFieldNum.nType = pDnt->GetFieldNum("_Type");
	sFieldNum.nAllowMapTypes = pDnt->GetFieldNum("_AllowMapTypes");
	sFieldNum.nShow = pDnt->GetFieldNum("_Show");
	sFieldNum.nSN.reserve(CashBuyShortcutMax);
	for (int j = 0; j < CashBuyShortcutMax; j++){
		sprintf_s(szTemp, "_SN%02d", j + 1);
		sFieldNum.nSN.push_back(pDnt->GetFieldNum(szTemp));
	}

	//##################################################################
	// Load
	//##################################################################

	for (int i=0; i<pDnt->GetItemCount(); ++i)
	{
		int nID = pDnt->GetItemID(i);
		int nIdx = pDnt->GetIDXprimary(nID);

		TCashBuyShortcutData Shortcut = {0,};

		Shortcut.nType = pDnt->GetFieldFromLablePtr(nIdx, sFieldNum.nType)->GetInteger();
		Shortcut.nAllowMapTypes = pDnt->GetFieldFromLablePtr(nIdx, sFieldNum.nAllowMapTypes)->GetInteger();
		Shortcut.nShow = pDnt->GetFieldFromLablePtr(nIdx, sFieldNum.nShow)->GetInteger();
		for (int j = 0; j < CashBuyShortcutMax; j++){
			int nSN = pDnt->GetFieldFromLablePtr(nIdx, sFieldNum.nSN[j])->GetInteger();
			if (nSN <= 0) continue;

			Shortcut.nSNList.push_back(nSN);
		}

		std::pair<TMapCashBuyShortcut::iterator, bool> Ret = m_MapCashBuyShortcut.insert(make_pair(Shortcut.nType, Shortcut));
	}

	return true;
}

TCashBuyShortcutData *CDNGameDataManager::GetCashBuyShortcut(int nType)
{
	TMapCashBuyShortcut::iterator iter = m_MapCashBuyShortcut.find(nType);
	if (iter == m_MapCashBuyShortcut.end()) return NULL;

	return &(iter->second);
}

bool CDNGameDataManager::IsCashBuyShortcut(int nType, int nMapIndex, int nItemSN)
{
	const TMapInfo *pMapData = GetMapInfo(nMapIndex);
	if (!pMapData) return false;

	TCashBuyShortcutData *pCash = GetCashBuyShortcut(nType);
	if (!pCash) return false;

	if (pCash->nShow == 0) return false;
	if (pCash->nSNList.empty()) return false;

	bool bFlag = false;
	for (int i = 0; i < (int)pCash->nSNList.size(); i++){
		if (pCash->nSNList[i] == nItemSN){
			bFlag = true;
			break;
		}
	}
	if (!bFlag) return false;

	return (pCash->nAllowMapTypes & pMapData->nAllowMapType) ? true : false;	
}

#endif	// #if defined(PRE_ADD_INSTANT_CASH_BUY)

#if defined(PRE_ADD_EXCHANGE_ENCHANT)
bool CDNGameDataManager::LoadEnchantTransferData()
{
#ifdef _WORK
	DNTableFileFormat* pSox;
	if (m_bAllLoaded)
		pSox = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TENCHANTTRANSFER );
	else
		pSox = GetDNTable( CDnTableDB::TENCHANTTRANSFER );
#else		//#ifdef _WORK
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TENCHANTTRANSFER );
#endif		//#ifdef _WORK
	if( !pSox )
	{
		g_Log.Log( LogType::_FILELOG, L"enchanttransfertable.dnt failed\r\n" );
		return false;
	}

	if( pSox->GetItemCount() <= 0 )
	{
		g_Log.Log( LogType::_FILELOG, L"enchanttransfertable.dnt Count(%d)\r\n", pSox->GetItemCount() );
		return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		m_vExchangeEnchantData.clear();
	}

#endif		//#ifdef _WORK

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{		
		int nRank;
		int nLevelLimit;
		int nEnchantLevel;
		int nNeedCoin;
		int nNeedItemID1;
		int nNeedItemCount1;
		int nNeedItemID2;
		int nNeedItemCount2;
	};

	TempFieldNum sFieldNum;	
	sFieldNum.nRank = pSox->GetFieldNum("_Rank");
	sFieldNum.nLevelLimit = pSox->GetFieldNum("_LevelLimit");
	sFieldNum.nEnchantLevel = pSox->GetFieldNum("_EnchantLevel");
	sFieldNum.nNeedCoin = pSox->GetFieldNum("_NeedCoin");
	sFieldNum.nNeedItemID1 = pSox->GetFieldNum("_NeedItemID1");
	sFieldNum.nNeedItemCount1 = pSox->GetFieldNum("_NeedItemCount1");	
	sFieldNum.nNeedItemID2 = pSox->GetFieldNum("_NeedItemID2");
	sFieldNum.nNeedItemCount2 = pSox->GetFieldNum("_NeedItemCount2");


	//##################################################################
	// Load
	//##################################################################

	for (int i=0; i<pSox->GetItemCount(); ++i)
	{
		TExchangeEnchantData Data;
		memset(&Data, 0, sizeof(TExchangeEnchantData));
		int nID = pSox->GetItemID(i);
		int nIdx = pSox->GetIDXprimary(nID);

		Data.cRank = (BYTE)pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nRank)->GetInteger();		
		Data.cLevelLimit = (BYTE)pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nLevelLimit)->GetInteger();
		Data.cEnchantLevel = (BYTE)pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nEnchantLevel)->GetInteger();
		Data.nNeedCoin = pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nNeedCoin)->GetInteger();
		Data.nNeedItemID1 = pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nNeedItemID1)->GetInteger();
		Data.wNeedItemCount1 = (short)pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nNeedItemCount1)->GetInteger();		
		Data.nNeedItemID2 = pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nNeedItemID2)->GetInteger();		
		Data.wNeedItemCount2 = (short)pSox->GetFieldFromLablePtr(nIdx, sFieldNum.nNeedItemCount2)->GetInteger();		
		m_vExchangeEnchantData.push_back(Data);
	}
	return true;
}

TExchangeEnchantData* CDNGameDataManager::GetExchangeEnchantData(BYTE cRank, BYTE cLevelLimit, BYTE cEnchantLevel)
{
	TVecExchangeEnchantData::iterator ii;
	for (ii = m_vExchangeEnchantData.begin(); ii != m_vExchangeEnchantData.end(); ii++)
	{
		if( (*ii).cRank == cRank && (*ii).cLevelLimit == cLevelLimit && (*ii).cEnchantLevel == cEnchantLevel )
			return &(*ii);
	}
	return NULL;

}
#endif

#if defined(PRE_ADD_WEEKLYEVENT)
bool CDNGameDataManager::LoadWeeklyEvent()
{
#ifdef _WORK
	DNTableFileFormat* pDnt;
	if (m_bAllLoaded)
		pDnt = CDnTableDB::GetInstance().ReLoadSox(CDnTableDB::TWEEKLYEVENT);
	else
		pDnt = GetDNTable(CDnTableDB::TWEEKLYEVENT);
#else		//#ifdef _WORK
	DNTableFileFormat* pDnt = GetDNTable(CDnTableDB::TWEEKLYEVENT);
#endif		//#ifdef _WORK
	if (!pDnt)
	{
		g_Log.Log(LogType::_FILELOG, L"weeklyevent.dnt failed\r\n");
		return false;
	}

	if (pDnt->GetItemCount() <= 0)
	{
		g_Log.Log(LogType::_FILELOG, L"weeklyevent.dnt Count(%d)\r\n", pDnt->GetItemCount());
		return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		m_pMapWeeklyEvent.clear();
	}

#endif	//#ifdef _WORK

	char szTemp[MAX_PATH] = { 0, };

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{		
		int nDayOfWeek;
		int nStartTime;
		int nEndTime;
		int nRaceType;
		int nClassType;
		int nEventType;
		int nValue;
	};

	TempFieldNum sFieldNum;	
	sFieldNum.nDayOfWeek = pDnt->GetFieldNum("_DayOfWeek");
	sFieldNum.nStartTime = pDnt->GetFieldNum("_StartTime");
	sFieldNum.nEndTime = pDnt->GetFieldNum("_EndTime");
	sFieldNum.nRaceType = pDnt->GetFieldNum("_RaceType");
	sFieldNum.nClassType = pDnt->GetFieldNum("_ClassType");
	sFieldNum.nEventType = pDnt->GetFieldNum("_EventType");
	sFieldNum.nValue = pDnt->GetFieldNum("_Value");

	//##################################################################
	// Load
	//##################################################################

	WeeklyEvent::TWeeklyEventData Data = {0,};

	for (int i = 0; i < pDnt->GetItemCount(); ++i)
	{
		int nID = pDnt->GetItemID(i);
		int nIdx = pDnt->GetIDXprimary(nID);

		int nDayOfWeek = pDnt->GetFieldFromLablePtr(nIdx, sFieldNum.nDayOfWeek)->GetInteger();
		if (nDayOfWeek <= 0) continue;
		if( nDayOfWeek == 7 ) // 일요일은 0이다.
			nDayOfWeek = 0;

		WeeklyEvent::TWeeklyEvent *pEvent = GetWeeklyEvent(nDayOfWeek);
		if (!pEvent){
			pEvent = new WeeklyEvent::TWeeklyEvent;
			pEvent->nDayOfWeek = nDayOfWeek;
		}

		memset(&Data, 0, sizeof(Data));
		Data.nStartTime = pDnt->GetFieldFromLablePtr(nIdx, sFieldNum.nStartTime)->GetInteger();
		Data.nEndTime = pDnt->GetFieldFromLablePtr(nIdx, sFieldNum.nEndTime)->GetInteger();
		Data.cRaceType = pDnt->GetFieldFromLablePtr(nIdx, sFieldNum.nRaceType)->GetInteger();
		Data.cClassType = pDnt->GetFieldFromLablePtr(nIdx, sFieldNum.nClassType)->GetInteger();
		Data.nEventType = pDnt->GetFieldFromLablePtr(nIdx, sFieldNum.nEventType)->GetInteger();
		Data.nValue = pDnt->GetFieldFromLablePtr(nIdx, sFieldNum.nValue)->GetInteger();

		pEvent->VecEventData.push_back(Data);

		m_pMapWeeklyEvent[nDayOfWeek] = pEvent;
	}

	return true;	
}

WeeklyEvent::TWeeklyEvent *CDNGameDataManager::GetWeeklyEvent(int nDayOfWeek)
{
	if (nDayOfWeek <= 0) return NULL;

	TMapWeeklyEvent::iterator iter = m_pMapWeeklyEvent.find(nDayOfWeek);
	if (iter == m_pMapWeeklyEvent.end()) return NULL;

	return iter->second;
}

int CDNGameDataManager::GetWeeklyEventValue(int nRaceType, int nClassType, int nEventType, int nThreadIndex)
{
	if (nThreadIndex >= 30) return 0;

	WeeklyEvent::TWeeklyEvent *pWeeklyEvent = GetWeeklyEvent(m_UpdateLocalTime[nThreadIndex].pCurLocalTime.tm_wday);
	if (!pWeeklyEvent) return 0;
	if (pWeeklyEvent->VecEventData.empty()) return 0;

	for (int i = 0; i < (int)pWeeklyEvent->VecEventData.size(); i++){
		if (pWeeklyEvent->VecEventData[i].nStartTime > m_UpdateLocalTime[nThreadIndex].pCurLocalTime.tm_hour) continue;
		if (pWeeklyEvent->VecEventData[i].nEndTime <= m_UpdateLocalTime[nThreadIndex].pCurLocalTime.tm_hour) continue;
		if ((pWeeklyEvent->VecEventData[i].cRaceType > 0) && (pWeeklyEvent->VecEventData[i].cRaceType != nRaceType)) continue;
		if ((pWeeklyEvent->VecEventData[i].cClassType > 0) && (pWeeklyEvent->VecEventData[i].cClassType != nClassType)) continue;
		if (pWeeklyEvent->VecEventData[i].nEventType != nEventType) continue;

		return pWeeklyEvent->VecEventData[i].nValue;
	}

	return 0;
}

float CDNGameDataManager::GetWeeklyEventValuef(int nRaceType, int nClassType, int nEventType, int nThreadIndex)
{
	int nValue = GetWeeklyEventValue(nRaceType, nClassType, nEventType, nThreadIndex);
	if (nValue == 0) return 0.f;

	return (float)(nValue / 100.f);
}

void CDNGameDataManager::WeeklyEventClear()
{
	m_pMapWeeklyEvent.clear();
}

#endif	// #if defined(PRE_ADD_WEEKLYEVENT)

#if defined( PRE_ADD_TOTAL_LEVEL_SKILL )
bool CDNGameDataManager::LoadTotalLevelSkill()
{
#ifdef _WORK
	DNTableFileFormat* pTotalSkillDnt;
	DNTableFileFormat*  pTotalSlot;
	DNTableFileFormat*  pSkillLevelTable;
	DNTableFileFormat* pSkillTable;
	if (m_bAllLoaded)
	{
		pTotalSkillDnt = CDnTableDB::GetInstance().ReLoadSox(CDnTableDB::TTOTALLEVELSKILL);
		pTotalSlot = CDnTableDB::GetInstance().ReLoadSox(CDnTableDB::TTOTALLEVELLSKILLSLOT);
		DNTableFileFormat* pSkillLevelTable = CDnTableDB::GetInstance().ReLoadSox(CDnTableDB::TSKILLLEVEL);
		pSkillTable = CDnTableDB::GetInstance().ReLoadSox(CDnTableDB::TSKILL);
	}
	else
	{
		pTotalSkillDnt = GetDNTable(CDnTableDB::TTOTALLEVELSKILL);
		pTotalSlot = GetDNTable(CDnTableDB::TTOTALLEVELLSKILLSLOT);
		pSkillLevelTable = GetDNTable( CDnTableDB::TSKILLLEVEL );
		pSkillTable = GetDNTable( CDnTableDB::TSKILL );
	}
#else		//#ifdef _WORK
	DNTableFileFormat* pTotalSkillDnt = GetDNTable(CDnTableDB::TTOTALLEVELSKILL);
	DNTableFileFormat* pTotalSlot = GetDNTable(CDnTableDB::TTOTALLEVELLSKILLSLOT);
	DNTableFileFormat* pSkillLevelTable = GetDNTable( CDnTableDB::TSKILLLEVEL );
	DNTableFileFormat* pSkillTable = GetDNTable( CDnTableDB::TSKILL );
#endif		//#ifdef _WORK
	if (!pTotalSkillDnt)
	{
		g_Log.Log(LogType::_FILELOG, L"totallevelskill.dnt failed\r\n");
		return false;
	}

	if (!pTotalSlot)
	{
		g_Log.Log(LogType::_FILELOG, L"totalskillslot.dnt failed\r\n");
		return false;
	}

	if (!pSkillLevelTable)
	{
		g_Log.Log(LogType::_FILELOG, L"SkillLevelTable.dnt failed\r\n");
		return false;
	}

	if (!pSkillTable)
	{
		g_Log.Log(LogType::_FILELOG, L"SkillTable.dnt failed\r\n");
		return false;
	}

	if (pTotalSkillDnt->GetItemCount() <= 0)
	{
		g_Log.Log(LogType::_FILELOG, L"totallevelskill.dnt Count(%d)\r\n", pTotalSkillDnt->GetItemCount());
		return false;
	}
	if (pTotalSlot->GetItemCount() <= 0)
	{
		g_Log.Log(LogType::_FILELOG, L"totalskillslot.dnt Count(%d)\r\n", pTotalSlot->GetItemCount());
		return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		m_MapTotalSkillLevelTabel.clear();
		m_MapTotalSkillSlotTabel.clear();
		m_MapTotalSkillBlow.clear();
	}

#endif	//#ifdef _WORK

	char szTemp[MAX_PATH] = { 0, };

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{		
		int nSkillID;
		int nTotalLevelLimit;
		int nUseLevelLimit;
		int nSkillType;
		int nSlotIndex;
		int nLevelLimit;
		int bCash;
		std::vector<int> EffectBlowID;
		std::vector<int> EffectBlowValue;
	};

	TempFieldNum sFieldNum;	
	sFieldNum.nSkillID = pTotalSkillDnt->GetFieldNum("_SkillTableID");
	sFieldNum.nTotalLevelLimit = pTotalSkillDnt->GetFieldNum("_TotalLevelLimit");
	sFieldNum.nUseLevelLimit = pTotalSkillDnt->GetFieldNum("_LevelLimit");
	sFieldNum.nSkillType = pTotalSkillDnt->GetFieldNum("_SkillType");

	sFieldNum.nSlotIndex = pTotalSlot->GetFieldNum("_SlotID");
	sFieldNum.nLevelLimit = pTotalSlot->GetFieldNum("_LevelLimit");
	sFieldNum.bCash = pTotalSlot->GetFieldNum("_IsCash");
	
	sFieldNum.EffectBlowID.reserve(TotalLevelSkill::Common::MAX_STATE_EFFECT_COUNT);
	sFieldNum.EffectBlowValue.reserve(TotalLevelSkill::Common::MAX_STATE_EFFECT_COUNT);
	for( int i = 0; i < TotalLevelSkill::Common::MAX_STATE_EFFECT_COUNT; ++i )
	{
		sprintf_s( szTemp, "_EffectClass%d", i+1 );
		sFieldNum.EffectBlowID.push_back( pSkillTable->GetFieldNum( szTemp) );		
		sprintf_s( szTemp, "_EffectClassValue%d", i+1 );
		sFieldNum.EffectBlowValue.push_back( pSkillLevelTable->GetFieldNum( szTemp) );		
	}

	

	//##################################################################
	// Load
	//##################################################################
	

	for (int i = 0; i < pTotalSkillDnt->GetItemCount(); ++i)
	{
		int nID = pTotalSkillDnt->GetItemID(i);
		int nIdx = pTotalSkillDnt->GetIDXprimary(nID);		

		TotalLevelSkill::TTotalSkillLevelTable LevelTable;
		memset(&LevelTable, 0, sizeof(LevelTable));	
		
		LevelTable.nSkillID = pTotalSkillDnt->GetFieldFromLablePtr(nIdx, sFieldNum.nSkillID)->GetInteger();
		LevelTable.nTotalLevelLimit= pTotalSkillDnt->GetFieldFromLablePtr(nIdx, sFieldNum.nTotalLevelLimit)->GetInteger();
		LevelTable.nUseLevelLimit= pTotalSkillDnt->GetFieldFromLablePtr(nIdx, sFieldNum.nUseLevelLimit)->GetInteger();
		LevelTable.nSkillType= pTotalSkillDnt->GetFieldFromLablePtr(nIdx, sFieldNum.nSkillType)->GetInteger();

		m_MapTotalSkillLevelTabel.insert(std::make_pair(LevelTable.nSkillID, LevelTable));

		std::list<TotalLevelSkill::TTotalSkillBlowTable> EffectBlowList;
		EffectBlowList.clear();
		for( int j = 0; j < TotalLevelSkill::Common::MAX_STATE_EFFECT_COUNT; ++j )
		{
			std::vector<int> vlSkillLevelList;			
			if( pSkillLevelTable->GetItemIDListFromField( "_SkillIndex", LevelTable.nSkillID, vlSkillLevelList ) <= 0 ) 
				break;

			int iSkillLevelTableID = -1;

			for( int i = 0; i < (int)vlSkillLevelList.size(); ++i )
			{
				int iNowLevel = pSkillLevelTable->GetFieldFromLablePtr( vlSkillLevelList.at(i), "_SkillLevel" )->GetInteger();
				if( iNowLevel == 1 )
				{
					iSkillLevelTableID = vlSkillLevelList.at( i );
					break;
				}
			}

			if( -1 == iSkillLevelTableID )
				break;

			int nSkillIdx = pSkillTable->GetIDXprimary( LevelTable.nSkillID );

			// 마을용 인자는 무조건 한개로 통일			
			TotalLevelSkill::TTotalSkillBlowTable BlowData;
			memset(&BlowData, 0, sizeof(TotalLevelSkill::TTotalSkillBlowTable));
			BlowData.nSkillID = LevelTable.nSkillID;			
			BlowData.nBlowID = pSkillTable->GetFieldFromLablePtr( nSkillIdx, sFieldNum.EffectBlowID[j] )->GetInteger();			

			if(BlowData.nBlowID < 1)
				continue;			

			int nSkillLevelIdx = pSkillLevelTable->GetIDXprimary( iSkillLevelTableID );
			string szValue = pSkillLevelTable->GetFieldFromLablePtr( nSkillLevelIdx, sFieldNum.EffectBlowValue[j] )->GetString();
			BlowData.fBlowValue = (float)atof( szValue.c_str() );
			BlowData.nBlowValue = (int)atoi( szValue.c_str() );
			EffectBlowList.push_back(BlowData);
		}
		m_MapTotalSkillBlow.insert(make_pair(LevelTable.nSkillID, EffectBlowList));
	}

	for (int i = 0; i < pTotalSlot->GetItemCount(); ++i)
	{
		int nID = pTotalSlot->GetItemID(i);
		int nIdx = pTotalSlot->GetIDXprimary(nID);		

		TotalLevelSkill::TTotalSkillSlotTable SlotTable;
		memset(&SlotTable, 0, sizeof(SlotTable));	

		
		SlotTable.nSlotIndex= pTotalSlot->GetFieldFromLablePtr(nIdx, sFieldNum.nSlotIndex)->GetInteger() - 1;
		SlotTable.nLevelLimit= pTotalSlot->GetFieldFromLablePtr(nIdx, sFieldNum.nLevelLimit)->GetInteger();
		SlotTable.bCash= pTotalSlot->GetFieldFromLablePtr(nIdx, sFieldNum.bCash)->GetInteger() ? true : false;		
		
		m_MapTotalSkillSlotTabel.insert(std::make_pair(SlotTable.nSlotIndex, SlotTable));
	}	

	return true;	
}

int CDNGameDataManager::GetTotalLevelSkillType( int nSkillID )
{
	TMapTotalSkillLevelTable::iterator itor = m_MapTotalSkillLevelTabel.find(nSkillID);
	if(itor != m_MapTotalSkillLevelTabel.end())
	{
		return itor->second.nSkillType;
	}
	return -1;
}

float CDNGameDataManager::GetTotalLevelSkillValue( int nSkillID, TotalLevelSkill::Common::eVillageEffectType eType )
{
	TMapTotalSkillBlow::iterator itor = m_MapTotalSkillBlow.find(nSkillID);
	float fValue = 0.0f;
	for(std::list<TotalLevelSkill::TTotalSkillBlowTable>::iterator ii = itor->second.begin();ii != itor->second.end();ii++)
	{
		if((*ii).nBlowID == eType)
			fValue += (*ii).fBlowValue;
	}
	return fValue;
}

int CDNGameDataManager::GetintTotalLevelSkillValue( int nSkillID, TotalLevelSkill::Common::eVillageEffectType eType )
{
	TMapTotalSkillBlow::iterator itor = m_MapTotalSkillBlow.find(nSkillID);	
	for(std::list<TotalLevelSkill::TTotalSkillBlowTable>::iterator ii = itor->second.begin();ii != itor->second.end();ii++)
	{
		if((*ii).nBlowID == eType)
		{
			return (*ii).nBlowValue;
			
		}
	}
	return 0;
}

bool CDNGameDataManager::bIsTotalLevelSkillLimitLevel( int nSlotIndex, int nSkillID, int nTotalLevel, int nCharacterLevel)
{
	TMapTotalSkillLevelTable::iterator itor = m_MapTotalSkillLevelTabel.find(nSkillID);
	if(itor != m_MapTotalSkillLevelTabel.end())
	{
		if( itor->second.nTotalLevelLimit <= nTotalLevel && itor->second.nUseLevelLimit <= nCharacterLevel )
		{
			TMapTotalSkillSlotTable::iterator ii = m_MapTotalSkillSlotTabel.find(nSlotIndex);
			if(ii != m_MapTotalSkillSlotTabel.end())
			{				
				if(ii->second.nLevelLimit <= nCharacterLevel)
					return true;				
			}
		}
	}
	return false;
}

bool CDNGameDataManager::bIsTotalLevelSkillCashSlot( int nSlotIndex )
{	
	TMapTotalSkillSlotTable::iterator ii = m_MapTotalSkillSlotTabel.find(nSlotIndex);
	if(ii != m_MapTotalSkillSlotTabel.end())
	{
		return ii->second.bCash;
	}
	return false;
}
#endif

#if defined(PRE_SPECIALBOX)
bool CDNGameDataManager::LoadKeepBoxProvideItem()
{
#ifdef _WORK
	DNTableFileFormat* pDnt;
	if (m_bAllLoaded)
		pDnt = CDnTableDB::GetInstance().ReLoadSox(CDnTableDB::TKEEPBOXPROVIDEITEM);
	else
		pDnt = GetDNTable(CDnTableDB::TKEEPBOXPROVIDEITEM);
#else		//#ifdef _WORK
	DNTableFileFormat* pDnt = GetDNTable(CDnTableDB::TKEEPBOXPROVIDEITEM);
#endif		//#ifdef _WORK
	if (!pDnt)
	{
		g_Log.Log(LogType::_FILELOG, L"keepboxprovideitemtable.dnt failed\r\n");
		return false;
	}

	if (pDnt->GetItemCount() <= 0)
	{
		g_Log.Log(LogType::_FILELOG, L"keepboxprovideitemtable.dnt Count(%d)\r\n", pDnt->GetItemCount());
		return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		m_pMapKeepBoxProvideItemData.clear();
	}

#endif	//#ifdef _WORK

	char szTemp[MAX_PATH] = { 0, };

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{		
		int nType;
		int nCashSN;
		int nItemID;
		int nCount;
		int nSealCount;
		int nOption;
	};

	TempFieldNum sFieldNum;	
	sFieldNum.nType = pDnt->GetFieldNum("_Type");
	sFieldNum.nCashSN = pDnt->GetFieldNum("_CashSN");
	sFieldNum.nItemID = pDnt->GetFieldNum("_ItemID");
	sFieldNum.nCount = pDnt->GetFieldNum("_Count");
	sFieldNum.nSealCount = pDnt->GetFieldNum("_SealCount");
	sFieldNum.nOption = pDnt->GetFieldNum("_ItemOption");

	//##################################################################
	// Load
	//##################################################################

	for (int i = 0; i < pDnt->GetItemCount(); ++i)
	{
		int nID = pDnt->GetItemID(i);
		int nIdx = pDnt->GetIDXprimary(nID);

		int nType = pDnt->GetFieldFromLablePtr(nIdx, sFieldNum.nType)->GetInteger();

		TKeepBoxProvideItemData *pData = GetKeepBoxProvideItemData(nType);
		if (!pData){
			pData = new TKeepBoxProvideItemData;
			pData->nType = nType;
		}

		TProvideItemData AddItem = {0,};

		AddItem.nCashSN = pDnt->GetFieldFromLablePtr(nIdx, sFieldNum.nCashSN)->GetInteger();
		AddItem.nItemID = pDnt->GetFieldFromLablePtr(nIdx, sFieldNum.nItemID)->GetInteger();
		AddItem.nCount = pDnt->GetFieldFromLablePtr(nIdx, sFieldNum.nCount)->GetInteger();
		AddItem.nSealCount = pDnt->GetFieldFromLablePtr(nIdx, sFieldNum.nSealCount)->GetInteger();
		if (AddItem.nSealCount == -1)
			AddItem.nSealCount = GetItemSealCount(AddItem.nItemID);
		AddItem.nOption = pDnt->GetFieldFromLablePtr(nIdx, sFieldNum.nOption)->GetInteger();

		pData->VecProvideItem.push_back(AddItem);

		m_pMapKeepBoxProvideItemData[nType] = pData;
	}

	return true;
}

TKeepBoxProvideItemData *CDNGameDataManager::GetKeepBoxProvideItemData(int nType)
{
	TMapKeepBoxProvideItemData::iterator iter = m_pMapKeepBoxProvideItemData.find(nType);
	if (iter == m_pMapKeepBoxProvideItemData.end()) return NULL;

	return iter->second;
}
#endif	// #if defined(PRE_SPECIALBOX)

#if defined(PRE_ADD_REMOTE_QUEST)
bool CDNGameDataManager::LoadRemoteQuestData()
{
#ifdef _WORK
	DNTableFileFormat *pTable;
	if (m_bAllLoaded)
	{
		pTable = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TREMOTEQUEST );
	}
	else
	{
		pTable = GetDNTable( CDnTableDB::TREMOTEQUEST );
	}	
#else		//#ifdef _WORK
	DNTableFileFormat *pTable = GetDNTable( CDnTableDB::TREMOTEQUEST );
#endif		//#ifdef _WORK
	if( !pTable ) {
		g_Log.Log(LogType::_FILELOG, L"QuestRemoteTable.dnt failed\r\n");
		return false;
	}

	if( pTable->GetItemCount() <= 0 ) {
		g_Log.Log( LogType::_FILELOG, L"QuestRemoteTable.dnt Count(%d)\r\n", pTable->GetItemCount());
		return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		SAFE_DELETE_PVEC( m_pQuestRemoteTable );
		SAFE_DELETE_MAP(m_MapRemoteQuestID);
	}
#endif		//#ifdef _WORK

	char szTemp[MAX_PATH];

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{
		int	_Activate;
		int _QuestID;
		int _PreQuestCheckType;
		std::vector<int> _PreQuestCondition;
		int	_GainEvent;
		std::vector<int> _GainType;
		std::vector<int> _GainParam;
		std::vector<int> _GainOperator;
		int	_GainCheckType;		
	};

	TempFieldNum sFieldNum;
	sFieldNum._Activate 	= pTable->GetFieldNum( "_Activate" );
	sFieldNum._QuestID	= pTable->GetFieldNum( "_QuestID" );
	sFieldNum._PreQuestCheckType = pTable->GetFieldNum( "_PreQuestCheckType" );
	sFieldNum._GainEvent = pTable->GetFieldNum( "_GainEvent" );
	sFieldNum._GainType.reserve(5);
	sFieldNum._GainParam.reserve(5);
	sFieldNum._GainOperator.reserve(5);	

	for( int j=0; j<5; j++ ) 
	{
		sprintf_s( szTemp, "_Gain%dType", j + 1 );
		sFieldNum._GainType.push_back( pTable->GetFieldNum(szTemp) );
		sprintf_s( szTemp, "_Gain%dParam", j + 1 );
		sFieldNum._GainParam.push_back( pTable->GetFieldNum(szTemp) );
		sprintf_s( szTemp, "_Gain%dOperator", j + 1 );
		sFieldNum._GainOperator.push_back( pTable->GetFieldNum(szTemp) );
	}
	for( int j=0; j<10; j++ ) 
	{
		sprintf_s( szTemp, "_PreQuestCondition%d", j + 1 );
		sFieldNum._PreQuestCondition.push_back( pTable->GetFieldNum(szTemp) );
	}

	sFieldNum._GainCheckType = pTable->GetFieldNum( "_GainCheckType" );

	//##################################################################
	// Load
	//##################################################################

	for( int i=0; i<pTable->GetItemCount(); i++ ) 
	{
		int nItemID = pTable->GetItemID(i);

		int iIdx = pTable->GetIDXprimary( nItemID );

		TRemoteQuestData *pData = new TRemoteQuestData;
		memset( pData, 0, sizeof(TRemoteQuestData) );

		pData->bActivate		= ( pTable->GetFieldFromLablePtr( iIdx, sFieldNum._Activate )->GetInteger() == TRUE ) ? true : false;
		pData->nQuestID			= pTable->GetFieldFromLablePtr( iIdx, sFieldNum._QuestID )->GetInteger();
		pData->bPreQuestCheckType = ( pTable->GetFieldFromLablePtr( iIdx, sFieldNum._PreQuestCheckType )->GetInteger() == TRUE ) ? true : false;

		pData->GainCondition.cEvent	= (char)pTable->GetFieldFromLablePtr( iIdx, sFieldNum._GainEvent )->GetInteger();

		if (pData->bActivate)
		{
			if (pData->GainCondition.cEvent >= EventSystem::EventTypeEnum_Amount)
			{
				g_Log.Log( LogType::_FILELOG, L"QuestRemoteTable.dnt Invalid GainEvent : [ItemID:%d]\n" ,nItemID);
				return false;
			}			
		}

		for( int j=0; j<5; j++ ) 
		{
			pData->GainCondition.cType[j]			= (char)pTable->GetFieldFromLablePtr( iIdx, sFieldNum._GainType[j] )->GetInteger();
			pData->GainCondition.nParam[j]			= pTable->GetFieldFromLablePtr( iIdx, sFieldNum._GainParam[j] )->GetInteger();
			pData->GainCondition.cOperator[j]		= (char)pTable->GetFieldFromLablePtr( iIdx, sFieldNum._GainOperator[j] )->GetInteger();

			if (pData->bActivate)
			{
				if (pData->GainCondition.nParam[j] < 0)
				{
					g_Log.Log( LogType::_FILELOG, L"QuestRemoteTable.dnt, GainParm < 0 [ItemID:%d]\n", nItemID);
					return false;
				}

				if (pData->GainCondition.cType[j] >= EventSystem::EventValueTypeEnum_Amount)
				{
					g_Log.Log( LogType::_FILELOG, L"MissionTable.dnt, Invalid GainType\n");
					return false;
				}

				if (CDNEventSystem::s_EventValueFuncList[pData->GainCondition.cType[j]].pFunc == NULL && pData->GainCondition.cType[j] > 0)
				{
					g_Log.Log( LogType::_FILELOG, L"MissionTable.dnt, Invalid s_EventValueFuncList[GainType], please check MissionAPI\n");
					return false;
				}
			}
		}
		for( int j=0; j<10; j++ ) 
		{
			int nPreQuestID = pTable->GetFieldFromLablePtr( iIdx, sFieldNum._PreQuestCondition[j] )->GetInteger();
			if( 0 == nPreQuestID )
				continue;

			pData->nVecPreQuestCondition.push_back( nPreQuestID );
		}

		pData->GainCondition.bCheckType = ( pTable->GetFieldFromLablePtr( iIdx, sFieldNum._GainCheckType )->GetInteger() == TRUE ) ? true : false;

		m_pQuestRemoteTable.push_back( pData );
		m_MapRemoteQuestID.insert( make_pair(pData->nQuestID, pData->nIndex));
	}

	return true;
}

int CDNGameDataManager::GetRemoteQuestDataSize()
{
	return (int)m_pQuestRemoteTable.size();
}

TRemoteQuestData *CDNGameDataManager::GetRemoteQuestData( int nArrayIndex )
{
	if( nArrayIndex < 0 || nArrayIndex >= (int)m_pQuestRemoteTable.size() ) return NULL;
	return m_pQuestRemoteTable[nArrayIndex];
}

TRemoteQuestData *CDNGameDataManager::GetRemoteQuestDataByQuestID( int nQuestID )
{
	if( nQuestID < 0 || m_pQuestRemoteTable.empty()) return NULL;
	
	TMapRemoteQuestID::iterator iter = m_MapRemoteQuestID.find(nQuestID);
	if(iter == m_MapRemoteQuestID.end())
		return NULL;

	return GetRemoteQuestData(iter->second);
}

#endif

#ifdef PRE_ADD_JOINGUILD_SUPPORT
bool CDNGameDataManager::LoadGuildSupport()
{
#ifdef _WORK
	DNTableFileFormat* pDnt;
	if (m_bAllLoaded)
		pDnt = CDnTableDB::GetInstance().ReLoadSox(CDnTableDB::TGUILDSUPPORT);
	else
		pDnt = GetDNTable(CDnTableDB::TGUILDSUPPORT);
#else		//#ifdef _WORK
	DNTableFileFormat* pDnt = GetDNTable(CDnTableDB::TGUILDSUPPORT);
#endif		//#ifdef _WORK
	if (!pDnt)
	{
		g_Log.Log(LogType::_FILELOG, L"guildmembermaxlv.dnt failed\r\n");
		return false;
	}

	if (pDnt->GetItemCount() <= 0)
	{
		g_Log.Log(LogType::_FILELOG, L"guildmembermaxlv.dnt Count(%d)\r\n", pDnt->GetItemCount());
		return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		m_VecGuildSupportData.clear();
	}

#endif	//#ifdef _WORK

	char szTemp[MAX_PATH] = { 0, };

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{
		int nJoinMinLevel;			//최초길드가입시 최소레벨
		int nJoinMaxLevel;			//최초길드가입시 최대레벨
		int nFirstJoinMailID;		//최초가입시 보상메일아이디
		int nMaxLevelGuildSupportMailID;	//최초가입한 길드에서 만랩시 보상아이디
	};

	TempFieldNum sFieldNum;	
	sFieldNum.nJoinMinLevel = pDnt->GetFieldNum("_Lvmin");
	sFieldNum.nJoinMaxLevel = pDnt->GetFieldNum("_Lvmax");
	sFieldNum.nFirstJoinMailID = pDnt->GetFieldNum("_Join");
	sFieldNum.nMaxLevelGuildSupportMailID = pDnt->GetFieldNum("_Maxlv");

	//##################################################################
	// Load
	//##################################################################

	for (int i = 0; i < pDnt->GetItemCount(); ++i)
	{
		int nID = pDnt->GetItemID(i);
		int nIdx = pDnt->GetIDXprimary(nID);

		TGuildSupportData Data;
		memset(&Data, 0, sizeof(Data));

		Data.nID = nID;
		Data.nJoinMinLevel = pDnt->GetFieldFromLablePtr(nIdx, sFieldNum.nJoinMinLevel)->GetInteger();
		Data.nJoinMaxLevel = pDnt->GetFieldFromLablePtr(nIdx, sFieldNum.nJoinMaxLevel)->GetInteger();
		Data.nFirstJoinMailID = pDnt->GetFieldFromLablePtr(nIdx, sFieldNum.nFirstJoinMailID)->GetInteger();
		Data.nMaxLevelGuildSupportMailID = pDnt->GetFieldFromLablePtr(nIdx, sFieldNum.nMaxLevelGuildSupportMailID)->GetInteger();

		m_VecGuildSupportData.push_back(Data);
	}

	return true;
}

int CDNGameDataManager::GetRewardGuildSupportMailID(int nLevel)
{
	TVecGuildSupportData::iterator ii;
	for (ii = m_VecGuildSupportData.begin(); ii != m_VecGuildSupportData.end(); ii++)
	{
		if ((*ii).nJoinMinLevel <= nLevel && (*ii).nJoinMaxLevel >= nLevel)
			return (*ii).nMaxLevelGuildSupportMailID;
	}
	return 0;
}

int CDNGameDataManager::GetRewardFisrtGuildJoinMailID(int nLevel)
{
	TVecGuildSupportData::iterator ii;
	for (ii = m_VecGuildSupportData.begin(); ii != m_VecGuildSupportData.end(); ii++)
	{
		if ((*ii).nJoinMinLevel <= nLevel && (*ii).nJoinMaxLevel >= nLevel)
			return (*ii).nFirstJoinMailID;
	}
	return 0;
}
#endif		//#ifdef PRE_ADD_JOINGUILD_SUPPORT

#if defined( PRE_WORLDCOMBINE_PVP )
bool CDNGameDataManager::LoadWorldPVPMissionRoom()
{
#ifdef _WORK
	DNTableFileFormat* pSox;
	DNTableFileFormat* pGameModeSox;
	if (m_bAllLoaded)
	{
		pSox = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TWORLDPVPMISSIONROOM );
		pGameModeSox = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TPVPGAMEMODE );
	}
	else
	{
		pSox = GetDNTable( CDnTableDB::TWORLDPVPMISSIONROOM );
		pGameModeSox = GetDNTable( CDnTableDB::TPVPGAMEMODE );
	}
#else		//#ifdef _WORK
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TWORLDPVPMISSIONROOM );
	DNTableFileFormat* pGameModeSox = GetDNTable( CDnTableDB::TPVPGAMEMODE );
#endif		//#ifdef _WORK
	if( !pSox )
	{
		g_Log.Log( LogType::_FILELOG, L"worldmissionroomtable.dnt failed\r\n" );
		return false;
	}	

	if( pSox->GetItemCount() <= 0 )
	{
		g_Log.Log( LogType::_FILELOG, L"worldmissionroomtable.dnt Count(%d)\r\n", pSox->GetItemCount() );
		//return false;
	}

	if( !pGameModeSox )
	{
		g_Log.Log( LogType::_FILELOG, L"PvPGameModeTable.dnt failed\r\n" );
		return false;
	}

	if( pGameModeSox->GetItemCount() <= 0 )
	{
		g_Log.Log( LogType::_FILELOG, L"PvPGameModeTable.dnt Count(%d)\r\n", pGameModeSox->GetItemCount() );
		//return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		m_MapWorldPvPMissionRoom.clear();
		m_VecWorldPvPMissionRoom.clear();
		m_listWorldPvPRoomMode.clear();
	}
#endif		//#ifdef _WORK

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{
		int _PvPMissionRoomGroup;
		int _PvPMapTableID;
		int _PvPGameModeTableID;
		int _WinCondition;
		int _PlayTime;
		int _MaxPlayers;
		int _StartPlayers;
		int _TitleUIString;
		int _DropItem;
		int _LimitLevelMin;
		int _LimitLevelMax;
#ifdef PRE_ADD_COLOSSEUM_BEGINNER
		int _ChannelType;
#endif		//#ifdef PRE_ADD_COLOSSEUM_BEGINNER
	};

	TempFieldNum sFieldNum;
	sFieldNum._PvPMissionRoomGroup	= pSox->GetFieldNum("_WorldMissionRoomGroup");
	sFieldNum._PvPMapTableID		= pSox->GetFieldNum("_PvPMapTableID");
	sFieldNum._PvPGameModeTableID	= pSox->GetFieldNum("_PvPGameModeTableID");
	sFieldNum._WinCondition			= pSox->GetFieldNum("_WinCondition");
	sFieldNum._PlayTime				= pSox->GetFieldNum("_PlayTime");
	sFieldNum._MaxPlayers			= pSox->GetFieldNum("_MaxUser");
	sFieldNum._StartPlayers			= pSox->GetFieldNum("_StartUser");
	sFieldNum._TitleUIString		= pSox->GetFieldNum("_TitleUIString");
	sFieldNum._DropItem				= pSox->GetFieldNum("_DropItem");
	sFieldNum._LimitLevelMin		= pSox->GetFieldNum("_LimitLevelMin");
	sFieldNum._LimitLevelMax		= pSox->GetFieldNum("_LimitLevelMax");
#ifdef PRE_ADD_COLOSSEUM_BEGINNER
	sFieldNum._ChannelType = pSox->GetFieldNum("_PVPChannel");
#endif		//#ifdef PRE_ADD_COLOSSEUM_BEGINNER

	//##################################################################
	// Load
	//##################################################################

	for (int i = 0; i < pSox->GetItemCount(); ++i)
	{
		int nItemID = pSox->GetItemID(i);
		int nIdx = pSox->GetIDXprimary(nItemID);


		TWorldPvPMissionRoom Data;
		memset(&Data, 0, sizeof(Data));
		Data.nItemID		= nItemID;		
		Data.nMapID			= pSox->GetFieldFromLablePtr(nIdx, sFieldNum._PvPMapTableID)->GetInteger();
		Data.cModeID		= pSox->GetFieldFromLablePtr(nIdx, sFieldNum._PvPGameModeTableID)->GetInteger();
		Data.nWinCondition	= pSox->GetFieldFromLablePtr(nIdx, sFieldNum._WinCondition)->GetInteger();
		Data.nPlayTime		= pSox->GetFieldFromLablePtr(nIdx, sFieldNum._PlayTime)->GetInteger();
		Data.cMaxPlayers	= pSox->GetFieldFromLablePtr(nIdx, sFieldNum._MaxPlayers)->GetInteger();
		Data.nStartPlayers	= 2;
		Data.nRoomNameIndex = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._TitleUIString)->GetInteger();
		Data.bDropItem		= pSox->GetFieldFromLablePtr(nIdx, sFieldNum._DropItem)->GetInteger() ? true : false;
		Data.nMinLevel		= pSox->GetFieldFromLablePtr(nIdx, sFieldNum._LimitLevelMin)->GetInteger();
		Data.nMaxLevel		= pSox->GetFieldFromLablePtr(nIdx, sFieldNum._LimitLevelMax)->GetInteger();
#ifdef PRE_ADD_COLOSSEUM_BEGINNER
		Data.nChannelType = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._ChannelType)->GetInteger();
#endif		//#ifdef PRE_ADD_COLOSSEUM_BEGINNER		
		Data.eWorldReqType = WorldPvPMissionRoom::Common::MissionRoom;
//		Data.unRoomOptionBit |= PvPCommon::RoomOption::BreakInto;
		Data.unRoomOptionBit |= PvPCommon::RoomOption::RandomTeam;		

		int nAllowMode = pGameModeSox->GetFieldFromLablePtr( Data.cModeID, "GamemodeID" )->GetInteger();
//		m_listWorldPvPRoomMode.push_back(nAllowMode);
		if(nAllowMode == PvPCommon::GameMode::PvP_AllKill )
			Data.unRoomOptionBit |= PvPCommon::RoomOption::AllKill_RandomOrder;

#if defined(PRE_ADD_MULTILANGUAGE)
		std::wstring wszName = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, Data.nRoomNameIndex, MultiLanguage::eDefaultLanguage );
#else		//#if defined(PRE_ADD_MULTILANGUAGE)
		std::wstring wszName = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, Data.nRoomNameIndex );
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)

		_wcscpy( Data.wszRoomName, _countof(Data.wszRoomName), wszName.c_str(), static_cast<int>(wszName.size()) );

		m_MapWorldPvPMissionRoom.insert(make_pair(nItemID, Data));

		m_VecWorldPvPMissionRoom.push_back(Data);
	}

	m_listWorldPvPRoomMode.push_back(PvPCommon::GameMode::PvP_AllKill);
	m_listWorldPvPRoomMode.push_back(PvPCommon::GameMode::PvP_Tournament);

	return true;
}

TWorldPvPMissionRoom* CDNGameDataManager::GetWorldPvPMissionRoom(int nItemID)
{
	if(nItemID <= 0) return NULL;

	TMapWorldPvPMissionRoom::iterator iter = m_MapWorldPvPMissionRoom.find(nItemID);
	if(iter == m_MapWorldPvPMissionRoom.end()) return NULL;

	return &(iter->second);
}

TVecWorldPvPMissionRoom& CDNGameDataManager::GetWorldVecPvPMissionRoom()
{
	return m_VecWorldPvPMissionRoom;
}

bool CDNGameDataManager::bIsWorldPvPRoomAllowMode( int nMode )
{
	for( std::list<int>::iterator itor = m_listWorldPvPRoomMode.begin(); itor != m_listWorldPvPRoomMode.end(); itor++ )
	{
		if( *itor == nMode )
		{
			return true;
		}
	}
	return false;
}
#endif

#if defined(PRE_ADD_ACTIVEMISSION)

bool CDNGameDataManager::LoadActiveMissionData()
{
#ifdef _WORK
	DNTableFileFormat *pTable;
	if (m_bAllLoaded)
	{
		pTable = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TACTIVEMISSION );
	}
	else
	{
		pTable = GetDNTable( CDnTableDB::TACTIVEMISSION );
	}	
#else		//#ifdef _WORK
	DNTableFileFormat *pTable = GetDNTable( CDnTableDB::TACTIVEMISSION );
#endif		//#ifdef _WORK
	if( !pTable ) {
		g_Log.Log(LogType::_FILELOG, L"ActiveMissionTable.dnt failed\r\n");
		return false;
	}

	if( pTable->GetItemCount() <= 0 ) {
		g_Log.Log( LogType::_FILELOG, L"ActiveMissionTable.dnt Count(%d)\r\n", pTable->GetItemCount());
		return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		SAFE_DELETE_PVEC( m_pActiveMissionTable );
	}
#endif		//#ifdef _WORK

	char szTemp[MAX_PATH];

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{
		int	_Activate;		
		int	_AchieveEvent;
		std::vector<int> _AchieveType;
		std::vector<int> _AchieveParam;
		std::vector<int> _AchieveOperator;
		int	_AchieveCheckType;
		int _MailID;
	};

	TempFieldNum sFieldNum;
	sFieldNum._Activate 	= pTable->GetFieldNum( "_Activate" );	
	sFieldNum._AchieveEvent = pTable->GetFieldNum( "_AchieveEvent" );
	sFieldNum._AchieveType.reserve(5);
	sFieldNum._AchieveParam.reserve(5);
	sFieldNum._AchieveOperator.reserve(5);	
	sFieldNum._MailID = pTable->GetFieldNum( "_MailID" );

	for( int j=0; j<5; j++ ) 
	{
		sprintf_s( szTemp, "_Achieve%dType", j + 1 );
		sFieldNum._AchieveType.push_back( pTable->GetFieldNum(szTemp) );
		sprintf_s( szTemp, "_Achieve%dParam", j + 1 );
		sFieldNum._AchieveParam.push_back( pTable->GetFieldNum(szTemp) );
		sprintf_s( szTemp, "_Achieve%dOperator", j + 1 );
		sFieldNum._AchieveOperator.push_back( pTable->GetFieldNum(szTemp) );
		sprintf_s( szTemp, "_Achieve%dType", j + 1 );
	}

	sFieldNum._AchieveCheckType = pTable->GetFieldNum( "_AchieveCheckType" );

	//##################################################################
	// Load
	//##################################################################

	for( int i=0; i<pTable->GetItemCount(); i++ ) 
	{
		int nItemID = pTable->GetItemID(i);

		int iIdx = pTable->GetIDXprimary( nItemID );

		TActiveMissionData *pData = new TActiveMissionData;
		memset( pData, 0, sizeof(TActiveMissionData) );

		pData->nIndex = nItemID;
		pData->bActivate = ( pTable->GetFieldFromLablePtr( iIdx, sFieldNum._Activate )->GetInteger() == TRUE ) ? true : false;		
		pData->AchieveCondition.cEvent	= (char)pTable->GetFieldFromLablePtr( iIdx, sFieldNum._AchieveEvent )->GetInteger();

		if (pData->bActivate)
		{
			if (pData->AchieveCondition.cEvent >= EventSystem::EventTypeEnum_Amount)
			{
				g_Log.Log( LogType::_FILELOG, L"ActiveMissionTable.dnt Invalid AchieveEvent : [ItemID:%d]\n" ,nItemID);
				return false;
			}			
		}

		for( int j=0; j<5; j++ ) 
		{
			pData->AchieveCondition.cType[j]			= (char)pTable->GetFieldFromLablePtr( iIdx, sFieldNum._AchieveType[j] )->GetInteger();
			pData->AchieveCondition.nParam[j]			= pTable->GetFieldFromLablePtr( iIdx, sFieldNum._AchieveParam[j] )->GetInteger();
			pData->AchieveCondition.cOperator[j]		= (char)pTable->GetFieldFromLablePtr( iIdx, sFieldNum._AchieveOperator[j] )->GetInteger();

			if (pData->bActivate)
			{
				if (pData->AchieveCondition.nParam[j] < 0)
				{
					g_Log.Log( LogType::_FILELOG, L"ActiveMissionTable.dnt, AchieveParm < 0 [ItemID:%d]\n", nItemID);
					return false;
				}

				if (pData->AchieveCondition.cType[j] >= EventSystem::EventValueTypeEnum_Amount)
				{
					g_Log.Log( LogType::_FILELOG, L"MissionTable.dnt, Invalid AchieveType\n");
					return false;
				}

				if (CDNEventSystem::s_EventValueFuncList[pData->AchieveCondition.cType[j]].pFunc == NULL && pData->AchieveCondition.cType[j] > 0)
				{
					g_Log.Log( LogType::_FILELOG, L"MissionTable.dnt, Invalid s_EventValueFuncList[AchieveType], please check MissionAPI\n");
					return false;
				}
			}
		}
		pData->AchieveCondition.bCheckType = ( pTable->GetFieldFromLablePtr( iIdx, sFieldNum._AchieveCheckType )->GetInteger() == TRUE ) ? true : false;
		pData->nMailID = pTable->GetFieldFromLablePtr( iIdx, sFieldNum._MailID )->GetInteger();

		m_pActiveMissionTable.push_back( pData );
	}

	return true;
}

TActiveMissionData* CDNGameDataManager::GetActiveMissionData(int nIndex)
{
	if(nIndex < 0 || nIndex >= (int)m_pActiveMissionTable.size()) return NULL;
	return m_pActiveMissionTable[nIndex];
}


bool CDNGameDataManager::LoadActiveSetData()
{
#ifdef _WORK
	DNTableFileFormat *pTable;
	if (m_bAllLoaded)
	{
		pTable = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TACTIVESET );
	}
	else
	{
		pTable = GetDNTable( CDnTableDB::TACTIVESET );
	}	
#else		//#ifdef _WORK
	DNTableFileFormat *pTable = GetDNTable( CDnTableDB::TACTIVESET );
#endif		//#ifdef _WORK
	if( !pTable ) {
		g_Log.Log(LogType::_FILELOG, L"ActiveSetTable.dnt failed\r\n");
		return false;
	}

	if( pTable->GetItemCount() <= 0 ) {
		g_Log.Log( LogType::_FILELOG, L"ActiveSetTable.dnt Count(%d)\r\n", pTable->GetItemCount());
		return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		for(std::map<int, std::vector<int>>::iterator iter = m_ActiveSetTable.begin() ; iter != m_ActiveSetTable.end() ; iter++ )
			SAFE_DELETE_VEC(iter->second);
		SAFE_DELETE_MAP( m_ActiveSetTable );
	}
#endif		//#ifdef _WORK	

	//##################################################################
	// Load
	//##################################################################

	for( int i=0; i<pTable->GetItemCount(); i++ )
	{
		int nItemID = pTable->GetItemID(i);

		char *szGroupStr;
		std::string strValue;

		int nMapID = pTable->GetFieldFromLablePtr( nItemID, "_MapID" )->GetInteger();
		if(nMapID <= 0)
		{
			g_Log.Log( LogType::_FILELOG, L"ActiveSetTable.dnt Invalid MapID [%d : %d]\r\n", nItemID, nMapID);
			return false;
		}

		szGroupStr = pTable->GetFieldFromLablePtr( nItemID, "_ActiveGroup1" )->GetString();
		if( strlen( szGroupStr ) > 0 ) {
			for( int n=0;; n++ ) {
				strValue = _GetSubStrByCountSafe( n, szGroupStr, ';' );
				if( !strValue.size() ) break;
				int nResultID = atoi(strValue.c_str());
				if( nResultID < 1 ) continue;
				m_ActiveSetTable[nMapID].push_back( nResultID );				
			}
		}
	}

	return true;
}

TActiveMissionData* CDNGameDataManager::GetRandomActiveMission( int nMapIndex, int nRandomSeed )
{
	if( m_ActiveSetTable.find(nMapIndex) == m_ActiveSetTable.end() ) return NULL;
	
	//실제 index 위치는 ID - 1 값
	int nIndex = m_ActiveSetTable[nMapIndex][nRandomSeed % m_ActiveSetTable[nMapIndex].size()] - 1;
	return GetActiveMissionData(nIndex);
}
#endif

#if defined(PRE_ADD_STAGE_CLEAR_ADD_REWARD)
bool CDNGameDataManager::LoadBonusDropTable(void)
{
#ifdef _WORK
	DNTableFileFormat *pTable;
	if (m_bAllLoaded)
	{
		pTable = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TBONUSDROP );
	}
	else
	{
		pTable = GetDNTable( CDnTableDB::TBONUSDROP );
	}
#else		//#ifdef _WORK
	DNTableFileFormat *pTable = GetDNTable( CDnTableDB::TBONUSDROP );
#endif		//#ifdef _WORK
	if( !pTable ) {
		g_Log.Log(LogType::_FILELOG, L"BonusDropTable.dnt failed\r\n");
		return false;
	}

	if( pTable->GetItemCount() <= 0 ) {
		g_Log.Log(LogType::_FILELOG, L"BonusDropTable.dnt Count(%d)\r\n", pTable->GetItemCount());
		//return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		m_MapBonusDropTable.clear();
	}
#endif		//#ifdef _WORK

	char szTemp[MAX_PATH];

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{
		int nItemID;
		int nMapID;
		int nDropType;
		int nNeedItemID[3];
		int nDropItemGroupID[3];
	};

	TempFieldNum sFieldNum;
	sFieldNum.nMapID = pTable->GetFieldNum("_MapID");
	sFieldNum.nDropType = pTable->GetFieldNum("_DropType");
	for(int i = 0; i < 3; i++)
	{
		sprintf_s( szTemp, "_NeedItemID%d", i + 1 );
		sFieldNum.nNeedItemID[i] = pTable->GetFieldNum(szTemp);
		sprintf_s( szTemp, "_DropGroupID%d", i + 1 );
		sFieldNum.nDropItemGroupID[i] = pTable->GetFieldNum(szTemp);

	}

	//##################################################################
	// Load
	//##################################################################

	for( int i=0; i<pTable->GetItemCount(); i++ ) 
	{
		TBonusDropTable Data;
		memset( &Data, 0, sizeof(TBonusDropTable) );

		Data.nIndex = pTable->GetItemID(i);
		int iIdx = pTable->GetIDXprimary( Data.nIndex );
		Data.nMapID = pTable->GetFieldFromLablePtr( iIdx, sFieldNum.nMapID )->GetInteger();
		Data.nDropType = pTable->GetFieldFromLablePtr( iIdx, sFieldNum.nDropType )->GetInteger();
		for(int j = 0; j < 3; j++)
		{
			Data.nNeedItemID[j] = pTable->GetFieldFromLablePtr( iIdx, sFieldNum.nNeedItemID[j] )->GetInteger();
			Data.nDropItemGroupID[j] = pTable->GetFieldFromLablePtr( iIdx, sFieldNum.nDropItemGroupID[j] )->GetInteger();
		}
				
		m_MapBonusDropTable.insert(std::make_pair(Data.nMapID, Data));
	}
	return true;
}

int CDNGameDataManager::GetBonusDropGroupID(int nMapID, int nNeedItemID)
{
	if(m_MapBonusDropTable.empty())
		return 0;
	
	TMapBonusDropTable::iterator iter = m_MapBonusDropTable.find(nMapID);
	if(iter == m_MapBonusDropTable.end())
		return 0;
	
	for(int i=0; i < 3;i++)
	{
		if(iter->second.nNeedItemID[i] == nNeedItemID )
			return iter->second.nDropItemGroupID[i];
	}
	return 0;
}
#endif	// #if defined(PRE_ADD_STAGE_CLEAR_ADD_REWARD)

#if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
bool CDNGameDataManager::LoadDropItemEnchant()
{
#ifdef _WORK
	DNTableFileFormat *pTable;
	if (m_bAllLoaded)
	{
		pTable = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TDROPITEMENCHANT );
	}
	else
	{
		pTable = GetDNTable( CDnTableDB::TDROPITEMENCHANT );
	}
#else		//#ifdef _WORK
	DNTableFileFormat *pTable = GetDNTable( CDnTableDB::TDROPITEMENCHANT );
#endif		//#ifdef _WORK
	if( !pTable ) {
		g_Log.Log(LogType::_FILELOG, L"DropItemEnchant.dnt failed\r\n");
		return false;
	}

	if( pTable->GetItemCount() <= 0 ) {
		g_Log.Log(LogType::_FILELOG, L"DropItemEnchant.dnt Count(%d)\r\n", pTable->GetItemCount());
		//return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		m_MapItemDropEnchantTable.clear();
	}
#endif		//#ifdef _WORK

	char szTemp[MAX_PATH];

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{
		int nDropID;
		int nOption[20];
		int nProb[20];
	};

	TempFieldNum sFieldNum;
	sFieldNum.nDropID = pTable->GetFieldNum("_DropID");

	for(int i = 0; i < 20; i++)
	{
		sprintf_s( szTemp, "_ItemOptionID%d", i + 1 );
		sFieldNum.nOption[i] = pTable->GetFieldNum(szTemp);
		sprintf_s( szTemp, "_ItemOptionRatio%d", i + 1 );
		sFieldNum.nProb[i] = pTable->GetFieldNum(szTemp);

	}

	//##################################################################
	// Load
	//##################################################################

	for( int i=0; i<pTable->GetItemCount(); i++ ) 
	{
		TItemDropEnchantData Data;
		memset( &Data, 0, sizeof(TItemDropEnchantData) );

		Data.nIndex = pTable->GetItemID(i);
		int iIdx = pTable->GetIDXprimary( Data.nIndex );
		Data.nDropID = pTable->GetFieldFromLablePtr( iIdx, sFieldNum.nDropID )->GetInteger();
		for(int j = 0; j < 20; j++)
		{
			Data.nEnchantOption[j] = pTable->GetFieldFromLablePtr( iIdx, sFieldNum.nOption[j] )->GetInteger();
			Data.nEnchantProb[j] = pTable->GetFieldFromLablePtr( iIdx, sFieldNum.nProb[j] )->GetInteger();
			if(Data.nEnchantOption[j] <= 0)
			{
				Data.nEnchantCount = j + 1;
				break;
			}
		}

		m_MapItemDropEnchantTable.insert(std::make_pair(Data.nDropID, Data));
	}

	return true;
}
TItemDropEnchantData* CDNGameDataManager::GetDropItemEnchant(int nDropTableID)
{
	TMapItemDropEnchantTable::iterator iter = m_MapItemDropEnchantTable.find(nDropTableID);
	if(iter == m_MapItemDropEnchantTable.end()) 
		return NULL;

	return &iter->second;
}
#endif	// #if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)

#if defined( PRE_ALTEIAWORLD_EXPLORE )
bool CDNGameDataManager::LoadAlteiaWorldMapInfo()
{
#ifdef _WORK
	DNTableFileFormat *pTable;
	DNTableFileFormat *pTimeTable;
	if (m_bAllLoaded)
	{
		pTable = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TALTEIAWORLDMAP );
		pTimeTable = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TALTEIATIME );
	}
	else
	{
		pTable = GetDNTable( CDnTableDB::TALTEIAWORLDMAP );
		pTimeTable = GetDNTable( CDnTableDB::TALTEIATIME );
	}
#else		//#ifdef _WORK
	DNTableFileFormat *pTable = GetDNTable( CDnTableDB::TALTEIAWORLDMAP );
	DNTableFileFormat *pTimeTable = GetDNTable( CDnTableDB::TALTEIATIME );
#endif		//#ifdef _WORK
	if( !pTable ) {
		g_Log.Log(LogType::_FILELOG, L"AlteiaBoardTable.dnt failed\r\n");
		return false;
	}

	if( !pTimeTable ) {
		g_Log.Log(LogType::_FILELOG, L"alteiatimetable.dnt failed\r\n");
		return false;
	}

	if( pTable->GetItemCount() <= 0 ) {
		g_Log.Log(LogType::_FILELOG, L"AlteiaBoardTable.dnt Count(%d)\r\n", pTable->GetItemCount());
		//return false;
	}

	if( pTimeTable->GetItemCount() <= 0 ) {
		g_Log.Log(LogType::_FILELOG, L"alteiatimetable.dnt Count(%d)\r\n", pTimeTable->GetItemCount());
		//return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		m_MapAlteiaWorldMapInfo.clear();
	}
#endif		//#ifdef _WORK	

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{		
		int nMapID;
		int nDifficulty;
	};

	TempFieldNum sFieldNum;

	sFieldNum.nMapID = pTable->GetFieldNum("_MapID");	
	sFieldNum.nDifficulty = pTable->GetFieldNum("_Difficulty");	

	//##################################################################
	// Load
	//##################################################################

	for( int i=0; i<pTable->GetItemCount(); i++ ) 
	{
		TAlteiaWorldMapInfo Data;
		memset( &Data, 0, sizeof(TAlteiaWorldMapInfo) );

		Data.nIndex = pTable->GetItemID(i);
		int iIdx = pTable->GetIDXprimary( Data.nIndex );
		Data.nMapID = pTable->GetFieldFromLablePtr( iIdx, sFieldNum.nMapID )->GetInteger();		
		Data.Difficulty = (TDUNGEONDIFFICULTY)pTable->GetFieldFromLablePtr( iIdx, sFieldNum.nDifficulty )->GetInteger();		

		m_MapAlteiaWorldMapInfo.insert(std::make_pair(Data.nIndex, Data));
		m_nAlteiaWorldMapMaxCount = Data.nIndex;
	}

	CTimeSet tCurTime;
	tCurTime.Reset();

	for( int i=0; i<pTimeTable->GetItemCount(); i++ ) 
	{
		int nItemID = pTimeTable->GetItemID(i);
		
		char *szPeriod = pTimeTable->GetFieldFromLablePtr( nItemID, "_EventPeriod" )->GetString();
		std::vector<string> vEventPeriod;
		TokenizeA( szPeriod, vEventPeriod, ":" );
		if( vEventPeriod.size() != 2 ) continue;

		CTimeSet tBeginSet( vEventPeriod[0].c_str(), true );
		CTimeSet tEndSet( vEventPeriod[1].c_str(), true );		
		
		if( tCurTime.GetTimeT64_LC() > tEndSet.GetTimeT64_LC() ) continue;

		tm pLastTime;
		time_t Time = tEndSet.GetTimeT64_LC();
		pLastTime = *localtime(&Time);

		if( pLastTime.tm_wday != 6 )
		{
			g_Log.Log(LogType::_FILELOG, L"alteiatimetable.dnt EndDate Error\r\n");
			return false;
		}
		
		m_tAlteiaWorldBeginTime = tBeginSet.GetTimeT64_LC();	
		m_tAlteiaWorldEndTime = tEndSet.GetTimeT64_LC();
	}

	return true;
}

TAlteiaWorldMapInfo* CDNGameDataManager::GetAlteiaWorldMapInfo(int nIndex)
{
	TMapAlteiaWorldMapInfo::iterator iter = m_MapAlteiaWorldMapInfo.find(nIndex);
	if(iter == m_MapAlteiaWorldMapInfo.end()) 
		return NULL;

	return &iter->second;
}

bool CDNGameDataManager::bIsAlteiaWorldEventTime()
{	
	CTimeSet CurTime;
	if( m_tAlteiaWorldBeginTime < CurTime.GetTimeT64_LC() && m_tAlteiaWorldEndTime > CurTime.GetTimeT64_LC() )
		return true;

	return false;
}

#endif // #if defined( PRE_ALTEIAWORLD_EXPLORE )

#if defined( PRE_ADD_STAMPSYSTEM )
bool CDNGameDataManager::LoadStampTable()
{
#ifdef _WORK
	DNTableFileFormat *pTable;
	if (m_bAllLoaded)
	{
		pTable = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TSTAMPCHALLENGE );
	}
	else
	{
		pTable = GetDNTable( CDnTableDB::TSTAMPCHALLENGE );
	}
#else		//#ifdef _WORK
	DNTableFileFormat *pTable = GetDNTable( CDnTableDB::TSTAMPCHALLENGE );
#endif		//#ifdef _WORK
	if( !pTable ) {
		g_Log.Log(LogType::_FILELOG, L"StampTable.dnt failed\r\n");
		return false;
	}

	if( pTable->GetItemCount() <= 0 ) {
		g_Log.Log(LogType::_FILELOG, L"StampTable.dnt Count(%d)\r\n", pTable->GetItemCount());
		//return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		m_MapStampTableData.clear();
	}
#endif		//#ifdef _WORK	

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{		
		int _StampStartDay;
		int _Type;
		int _AssignmentID;
		int _MailID4;
		int _MailID7;
	};

	TempFieldNum sFieldNum;

	sFieldNum._StampStartDay = pTable->GetFieldNum( "_StampStartDay" );
	sFieldNum._Type = pTable->GetFieldNum( "_StampType" );
	sFieldNum._AssignmentID = pTable->GetFieldNum( "_AssignmentID" );
	sFieldNum._MailID4 = pTable->GetFieldNum( "_MailID4" );
	sFieldNum._MailID7 = pTable->GetFieldNum( "_MailID7" );

	//##################################################################
	// Load
	//##################################################################

	for( int i = 0; i < pTable->GetItemCount(); ++ i ) 
	{
		StampSystem::TStampChallenge ChallengeData;
		int nIndex = pTable->GetItemID(i);
		int iIdx = pTable->GetIDXprimary( nIndex );

		std::string szStartTime = pTable->GetFieldFromLablePtr( iIdx, sFieldNum._StampStartDay )->GetString();
		INT64 biStartTime = _atoi64( szStartTime.c_str() );

		ChallengeData.cType = (char)pTable->GetFieldFromLablePtr( iIdx, sFieldNum._Type )->GetInteger();
		ChallengeData.nMailID4 = pTable->GetFieldFromLablePtr( iIdx, sFieldNum._MailID4 )->GetInteger();
		ChallengeData.nMailID7 = pTable->GetFieldFromLablePtr( iIdx, sFieldNum._MailID7 )->GetInteger();

		std::vector<std::string> challengeTokens;

		std::string szAssignmentIDString = pTable->GetFieldFromLablePtr(iIdx, sFieldNum._AssignmentID)->GetString();
		TokenizeA( szAssignmentIDString, challengeTokens, ":" );

		std::vector<std::string>::iterator iter = challengeTokens.begin();
		for(; iter != challengeTokens.end(); ++iter)
		{
			std::string& token = * iter;
			int id = atoi( token.c_str() );
			ChallengeData.setAssignmentID.insert( id );
		}

		StampSystem::TStampTableData & rTableData = m_MapStampTableData[biStartTime];
		rTableData.biStartTime = biStartTime;

		if( StampSystem::Common::MaxChallengeCount > rTableData.vChallengeList.size() )
		{
			rTableData.vChallengeList.push_back(ChallengeData);
		}
	}

	return true;
}

const StampSystem::TStampTableData * CDNGameDataManager::GetStampTableData( INT64 biTime ) const
{
	const StampSystem::TStampTableData * pStampTableData = NULL;

	for( TMapStampTableData::const_iterator iter = m_MapStampTableData.begin() ; iter != m_MapStampTableData.end() ; ++ iter )
	{
		if( biTime < iter->first )
		{
			break;
		}
		pStampTableData = & iter->second;
	}
	return pStampTableData;	
}
#endif // #if defined( PRE_ADD_STAMPSYSTEM )

#if defined(PRE_ADD_TALISMAN_SYSTEM)

bool CDNGameDataManager::LoadTalismanData()
{
#ifdef _WORK
	DNTableFileFormat *pTable;
	if (m_bAllLoaded)
	{
		pTable = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TTALISMANITEM );
	}
	else
	{
		pTable = GetDNTable( CDnTableDB::TTALISMANITEM );
	}	
#else		//#ifdef _WORK
	DNTableFileFormat *pTable = GetDNTable( CDnTableDB::TTALISMANSLOT );
#endif		//#ifdef _WORK
	if( !pTable ) {
		g_Log.Log(LogType::_FILELOG, L"TalismanTable.dnt failed\r\n");
		return false;
	}

	if( pTable->GetItemCount() <= 0 ) {
		g_Log.Log( LogType::_FILELOG, L"TalismanTable.dnt Count(%d)\r\n", pTable->GetItemCount());
		return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		SAFE_DELETE_PVEC( m_pTalismanSlotTable );
	}
#endif		//#ifdef _WORK

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{		
		int _Type;
		int _Period;
	};

	TempFieldNum sFieldNum;
		
	sFieldNum._Type = pTable->GetFieldNum( "_Type" );
	sFieldNum._Period = pTable->GetFieldNum( "_Period" );

	//##################################################################
	// Load
	//##################################################################

	for( int i=0; i<pTable->GetItemCount(); i++ ) 
	{
		int nItemID = pTable->GetItemID(i);
		int iIdx = pTable->GetIDXprimary( nItemID );

		TTalismanData Data;
		memset( &Data, 0, sizeof(TTalismanData) );

		Data.nTalismanID = nItemID;
		Data.nType = pTable->GetFieldFromLablePtr( iIdx, sFieldNum._Type )->GetInteger();
		Data.nPeriod = pTable->GetFieldFromLablePtr( iIdx, sFieldNum._Period )->GetInteger();

		m_MapTalismanTable.insert(make_pair(Data.nTalismanID, Data));
	}

	return true;
}

TTalismanData *CDNGameDataManager::GetTalismanData(int nTalismanID)
{
	if(m_MapTalismanTable.empty()) return NULL;

	TMapTalismanData::iterator iter = m_MapTalismanTable.find(nTalismanID);
	if(iter == m_MapTalismanTable.end()) return NULL;

	return &(iter->second);
}

bool CDNGameDataManager::LoadTalismanSlotData()
{
#ifdef _WORK
	DNTableFileFormat *pTable;
	if (m_bAllLoaded)
	{
		pTable = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TTALISMANSLOT );
	}
	else
	{
		pTable = GetDNTable( CDnTableDB::TTALISMANSLOT );
	}	
#else		//#ifdef _WORK
	DNTableFileFormat *pTable = GetDNTable( CDnTableDB::TTALISMANSLOT );
#endif		//#ifdef _WORK
	if( !pTable ) {
		g_Log.Log(LogType::_FILELOG, L"TalismanSlotTable.dnt failed\r\n");
		return false;
	}

	if( pTable->GetItemCount() <= 0 ) {
		g_Log.Log( LogType::_FILELOG, L"TalismanSlotTable.dnt Count(%d)\r\n", pTable->GetItemCount());
		return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		SAFE_DELETE_PVEC( m_pTalismanSlotTable );
	}
#endif		//#ifdef _WORK

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{
		int _Service;
		int _Level;
		int _Amount;
		int _Item;
		int _ItemCount;
		int _Efficiency;
		int _Type;
	};

	TempFieldNum sFieldNum;

	sFieldNum._Service 	= pTable->GetFieldNum( "_Service" );	
	sFieldNum._Level = pTable->GetFieldNum( "_Level" );
	sFieldNum._Amount = pTable->GetFieldNum( "_Amount" );
	sFieldNum._Item = pTable->GetFieldNum( "_Item" );
	sFieldNum._ItemCount = pTable->GetFieldNum( "_ItemCount" );
	sFieldNum._Efficiency = pTable->GetFieldNum( "_Efficiency" );
	sFieldNum._Type = pTable->GetFieldNum( "_Type" );

	//##################################################################
	// Load
	//##################################################################

	for( int i=0; i<pTable->GetItemCount(); i++ ) 
	{
		int nItemID = pTable->GetItemID(i);
		int iIdx = pTable->GetIDXprimary( nItemID );

		TTalismanSlotData *pData = new TTalismanSlotData;
		memset( pData, 0, sizeof(TTalismanSlotData) );

		//pData->nIndex = nItemID;
		pData->bService = ( pTable->GetFieldFromLablePtr( iIdx, sFieldNum._Service )->GetInteger() == TRUE ) ? true : false;
		pData->nLevel = pTable->GetFieldFromLablePtr( iIdx, sFieldNum._Level )->GetInteger();
		pData->nAmount = pTable->GetFieldFromLablePtr( iIdx, sFieldNum._Amount )->GetInteger();
		pData->nItem = pTable->GetFieldFromLablePtr( iIdx, sFieldNum._Item )->GetInteger();
		pData->nNeedItemCount = pTable->GetFieldFromLablePtr( iIdx, sFieldNum._ItemCount )->GetInteger();
		pData->fEfficiency = pTable->GetFieldFromLablePtr( iIdx, sFieldNum._Efficiency )->GetFloat();
		pData->nType = pTable->GetFieldFromLablePtr( iIdx, sFieldNum._Type )->GetInteger();

		m_pTalismanSlotTable.push_back( pData );
	}

	return true;
}

TTalismanSlotData* CDNGameDataManager::GetTalismanSlotData(int nIndex)
{
	if(nIndex < 0 || nIndex >= (int)m_pTalismanSlotTable.size()) return NULL;
	return m_pTalismanSlotTable[nIndex];
}

float CDNGameDataManager::GetTalismanSlotEfficiency(int nIndex)
{
	if(nIndex < 0 || nIndex >= (int)m_pTalismanSlotTable.size()) return 0.0f;
	return m_pTalismanSlotTable[nIndex]->fEfficiency;
}

bool CDNGameDataManager::IsTalismanSlotLevel(int nTalismanIndex, int nCharLevel)
{
	if ((nTalismanIndex < 0) ||(nTalismanIndex > TALISMAN_MAX)) return false;
	if ((nCharLevel <= 0) ||(nCharLevel > CHARLEVELMAX)) return false;

	TTalismanSlotData *pTalismanSlotData = GetTalismanSlotData(nTalismanIndex);
	if(!pTalismanSlotData) return false;

	return nCharLevel >= pTalismanSlotData->nLevel;
}
#endif	//#if defined(PRE_ADD_TALISMAN_SYSTEM)

#if defined( PRE_DRAGONBUFF )
void CDNGameDataManager::AddWorldBuffData( int nItemID )
{
	// 기준 시간 나오면 여기서 더해서 넣어줌
	TMapWorldBuffData::iterator itor = m_MapWorldBuffData.find(nItemID);
	CTimeSet CurTime;
	int nSecond = static_cast<int>(CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::DragonBuffWaitTime ));	
	CurTime.AddSecond(nSecond);
	if( itor == m_MapWorldBuffData.end() )
	{	
		m_MapWorldBuffData.insert(make_pair(nItemID, CurTime.GetTimeT64_LC()));
	}
	else
	{		
		itor->second = CurTime.GetTimeT64_LC();
	}
}

bool CDNGameDataManager::bIsWorldBuffData( int nItemID )
{
	CTimeSet CurTime;
	TMapWorldBuffData::iterator itor = m_MapWorldBuffData.find(nItemID);
	if( itor == m_MapWorldBuffData.end() )
		return true;

	if(itor->second <= CurTime.GetTimeT64_LC())
		return true;

	return false;
}
#endif

#if defined( PRE_PVP_GAMBLEROOM )
bool CDNGameDataManager::LoadPVPGambleRoomData()
{
#ifdef _WORK
	DNTableFileFormat* pSox;
	if (m_bAllLoaded)
		pSox = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TPVPGAMBLEROOM );
	else
		pSox = GetDNTable( CDnTableDB::TPVPGAMBLEROOM );
#else		//#ifdef _WORK
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TPVPGAMBLEROOM );
#endif		//#ifdef _WORK
	if( !pSox )
	{
		g_Log.Log( LogType::_FILELOG, L"PvPGambleModeRoomTable.dnt failed\r\n" );
		return false;
	}

	if( pSox->GetItemCount() <= 0 )
	{
		g_Log.Log( LogType::_FILELOG, L"PvPGambleModeRoomTable.dnt Count(%d)\r\n", pSox->GetItemCount() );
		return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		m_MapPvPGambleRoom.clear();
	}
#endif		//#ifdef _WORK

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{
		int _PvPMissionRoomGroup;
		int _PvPMapTableID;
		int _PvPGameModeTableID;
		int _WinCondition;
		int _PlayTime;
		int _MaxPlayers;
		int _StartPlayers;
		int _TitleUIString;
		int _DropItem;
#ifdef PRE_ADD_COLOSSEUM_BEGINNER
		int _ChannelType;
#endif		//#ifdef PRE_ADD_COLOSSEUM_BEGINNER
	};

	TempFieldNum sFieldNum;
	sFieldNum._PvPMissionRoomGroup	= pSox->GetFieldNum("_PvPMissionRoomGroup");
	sFieldNum._PvPMapTableID		= pSox->GetFieldNum("_PvPMapTableID");
	sFieldNum._PvPGameModeTableID	= pSox->GetFieldNum("_PvPGameModeTableID");
	sFieldNum._WinCondition			= pSox->GetFieldNum("_WinCondition");
	sFieldNum._PlayTime				= pSox->GetFieldNum("_PlayTime");
	sFieldNum._MaxPlayers			= pSox->GetFieldNum("_MaxUser");
	sFieldNum._StartPlayers			= pSox->GetFieldNum("_StartUser");
	sFieldNum._TitleUIString		= pSox->GetFieldNum("_TitleUIString");	
#ifdef PRE_ADD_COLOSSEUM_BEGINNER
	sFieldNum._ChannelType = pSox->GetFieldNum("_PVPChannel");
#endif		//#ifdef PRE_ADD_COLOSSEUM_BEGINNER

	//##################################################################
	// Load
	//##################################################################

	for (int i = 0; i < pSox->GetItemCount(); ++i)
	{
		int nItemID = pSox->GetItemID(i);
		int nIdx = pSox->GetIDXprimary(nItemID);


		TPvPMissionRoom Data;
		Data.nItemID		= nItemID;
		Data.nGroupID		= pSox->GetFieldFromLablePtr(nIdx, sFieldNum._PvPMissionRoomGroup)->GetInteger();
		Data.nMapID			= pSox->GetFieldFromLablePtr(nIdx, sFieldNum._PvPMapTableID)->GetInteger();
		Data.nModeID		= pSox->GetFieldFromLablePtr(nIdx, sFieldNum._PvPGameModeTableID)->GetInteger();
		Data.nWinCondition	= pSox->GetFieldFromLablePtr(nIdx, sFieldNum._WinCondition)->GetInteger();
		Data.nPlayTime		= pSox->GetFieldFromLablePtr(nIdx, sFieldNum._PlayTime)->GetInteger();
		Data.nMaxPlayers	= pSox->GetFieldFromLablePtr(nIdx, sFieldNum._MaxPlayers)->GetInteger();
		Data.nStartPlayers	= pSox->GetFieldFromLablePtr(nIdx, sFieldNum._StartPlayers)->GetInteger();
		Data.nRoomNameIndex = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._TitleUIString)->GetInteger();
		
#ifdef PRE_ADD_COLOSSEUM_BEGINNER
		Data.nChannelType = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._ChannelType)->GetInteger();
#endif		//#ifdef PRE_ADD_COLOSSEUM_BEGINNER

		m_MapPvPGambleRoom.insert(make_pair(nItemID, Data));		
	}

	return true;
}

TPvPMissionRoom* CDNGameDataManager::GetPvPGambleRoom(int nItemID)
{
	if(nItemID <= 0) return NULL;

	TMapPvPMissionRoom::iterator iter = m_MapPvPGambleRoom.find(nItemID);
	if(iter == m_MapPvPGambleRoom.end()) return NULL;

	return &(iter->second);
}
#endif

#if defined( PRE_ADD_STAGE_WEIGHT )
bool CDNGameDataManager::LoadStageWeightData()
{
#ifdef _WORK
	DNTableFileFormat *pTable;
	if (m_bAllLoaded)
	{
		pTable = CDnTableDB::GetInstance().ReLoadSox( CDnTableDB::TSTAGEWEIGHT );
	}
	else
	{
		pTable = GetDNTable( CDnTableDB::TSTAGEWEIGHT );
	}
#else		//#ifdef _WORK
	DNTableFileFormat *pTable = GetDNTable( CDnTableDB::TSTAGEWEIGHT );
#endif		//#ifdef _WORK
	if( !pTable ) {
		g_Log.Log(LogType::_FILELOG, L"StageWeightTable.dnt failed\r\n");
		return false;
	}

#ifdef _WORK
	if (m_bAllLoaded)
	{
		m_MapStageWeightData.clear();
	}
#endif		//#ifdef _WORK	

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{		
		int _StageWeightID;
		int _HeadCount;
		int _HP;
		int _ATK;
		int _SuperArmor;
		int _ItemDrop;
	};

	TempFieldNum sFieldNum;

	sFieldNum._StageWeightID = pTable->GetFieldNum( "_StageWeightID" );
	sFieldNum._HeadCount = pTable->GetFieldNum( "_HeadCount" );
	sFieldNum._HP = pTable->GetFieldNum( "_HP" );
	sFieldNum._ATK = pTable->GetFieldNum( "_ATK" );
	sFieldNum._SuperArmor = pTable->GetFieldNum( "_SuperArmor" );
	sFieldNum._ItemDrop = pTable->GetFieldNum( "_ItemDrop" );

	//##################################################################
	// Load
	//##################################################################

	typedef std::map<DWORD, TStageWeightData>
			TMapHeadCountWeight;

	for( int i = 0; i < pTable->GetItemCount(); ++ i ) 
	{
		int nIndex = pTable->GetItemID(i);
		int iIdx = pTable->GetIDXprimary( nIndex );

		int nStageWeightID = pTable->GetFieldFromLablePtr( iIdx, sFieldNum._StageWeightID )->GetInteger();
		DWORD dwHeadCount = static_cast<BYTE>( pTable->GetFieldFromLablePtr( iIdx, sFieldNum._HeadCount )->GetInteger() );

		TMapHeadCountWeight & rMapHeadCountWeight = m_MapStageWeightData[nStageWeightID];
		TMapHeadCountWeight::iterator iter = rMapHeadCountWeight.find(dwHeadCount);
		if(rMapHeadCountWeight.end() != iter)
		{
			g_Log.Log( LogType::_FILELOG, L"StageWeightTable.dnt failed - exist(_StageWeightID:%d, _HeadCount:%d)\r\n", nStageWeightID, dwHeadCount );
			return false;
		}

		TStageWeightData & rStageWeightData = rMapHeadCountWeight[dwHeadCount];
		rStageWeightData.fHPRate = pTable->GetFieldFromLablePtr( iIdx, sFieldNum._HP )->GetFloat();
		rStageWeightData.fAttackRate = pTable->GetFieldFromLablePtr( iIdx, sFieldNum._ATK )->GetFloat();
		rStageWeightData.fSuperArmorRate = pTable->GetFieldFromLablePtr( iIdx, sFieldNum._SuperArmor )->GetFloat();
		rStageWeightData.fItemDropRate = pTable->GetFieldFromLablePtr( iIdx, sFieldNum._ItemDrop )->GetFloat();
	}

	return true;
}

const TStageWeightData* CDNGameDataManager::GetStageWeigthData( int nStageWeightID, DWORD dwUserCount )
{
	TMapStageWeightData::iterator iter = m_MapStageWeightData.find( nStageWeightID );
	if(m_MapStageWeightData.end() == iter)
		return NULL;

	std::map<DWORD, TStageWeightData>::iterator sub_iter = iter->second.find(dwUserCount);
	if( iter->second.end() == sub_iter )
		return NULL;

	return & sub_iter->second;
}
#endif // #if defined( PRE_ADD_STAGE_WEIGHT )
