
#include "stdafx.h"
#include "DnMainFrame.h"
#include "DNGameDataManager.h"
#include "DNQuestManager.h"
#include "DnScriptManager.h"

#if !defined( _FINAL_BUILD ) && !defined (__COVERITY__)

extern TGameConfig g_Config;

class LoadExtTable_unittest:public testing::Test
{
protected:

	virtual void SetUp() 
	{
		g_Config.szResourcePath = "r:\\GameRes";
		m_pMainFrame				= new CDnMainFrame;
		g_pDataManager				= new CDNGameDataManager;
		g_pQuestManager				= new CDNQuestManager();
		g_pNpcQuestScriptManager	= new DnScriptManager();
		EXPECT_TRUE( m_pMainFrame->PreInitialize() );
		EXPECT_TRUE( m_pMainFrame->InitializeDevice() );
		g_pQuestManager->LoadAllQuest("QuestTable.ext", g_pNpcQuestScriptManager);	
	}
	virtual void TearDown()
	{
		delete g_pNpcQuestScriptManager;
		delete g_pQuestManager;
		delete g_pDataManager;
		delete m_pMainFrame;
	}

	CDnMainFrame*	m_pMainFrame;
};

TEST_F( LoadExtTable_unittest, Load )
{
	EXPECT_TRUE( g_pDataManager->LoadMapInfo() );
	EXPECT_TRUE( g_pDataManager->LoadSkillData() );
	EXPECT_TRUE( g_pDataManager->LoadItemData() );
	EXPECT_TRUE( g_pDataManager->LoadActorData() );
	EXPECT_TRUE( g_pDataManager->LoadNpcData() );
	EXPECT_TRUE( g_pDataManager->LoadWeaponData() );
	EXPECT_TRUE( g_pDataManager->LoadPartData() );
	EXPECT_TRUE( g_pDataManager->LoadTalkData() );
	EXPECT_TRUE( g_pDataManager->LoadShopData() );
	EXPECT_TRUE( g_pDataManager->LoadSkillShopData() );
	EXPECT_TRUE( g_pDataManager->LoadQuestReward() );
	EXPECT_TRUE( g_pDataManager->LoadEnchantData() );
	EXPECT_TRUE( g_pDataManager->LoadPotentialData() );
	EXPECT_TRUE( g_pDataManager->LoadItemDropData() );
	EXPECT_TRUE( g_pDataManager->LoadMissionData() );
#if defined(_VILLAGESERVER)
	EXPECT_TRUE( g_pDataManager->LoadItemCompoundData() );
#endif
	EXPECT_TRUE( g_pDataManager->LoadPvPMapTable() );
	EXPECT_TRUE( g_pDataManager->LoadPvPGameModeTable() );
	EXPECT_TRUE( g_pDataManager->LoadPvPGameStartConditionTable() );
	EXPECT_TRUE( g_pDataManager->LoadPvPRankTable() );
	EXPECT_TRUE( g_pDataManager->LoadChannelInfo() );
}

#endif // #if !defined( _FINAL_BUILD )
