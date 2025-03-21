#include "stdafx.h"
#include "DnScriptAPI.h"
#include "DNUserSession.h"
#include "DNUserSessionManager.h"
#include "DNFieldManager.h"
#include "DNNpcObject.h"
#include "DNNpcObjectManager.h"
#include "DNUserItem.h"
#include "DNGameDataManager.h"

#include "DNLogConnection.h"
#include "DnLuaAPIDefine.h"
#include "DNReplaceString.h"
#include "EtUIXML.h"

#include "DNMasterConnection.h"
#include "DNPartyManager.h"
#include "DNQuestManager.h"
#include "TimeSet.h"
#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
#include "ReputationSystemRepository.h"
#include "NpcReputationProcessor.h"
#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
#if defined( PRE_ADD_SECONDARY_SKILL )
#include "SecondarySkillRepository.h"
#endif // #if defined( PRE_ADD_SECONDARY_SKILL )
#include "DNGuildSystem.h"
#if !defined(_FINAL_BUILD)
extern CLog g_ScriptLog;
#endif	// #if !defined(_FINAL_BUILD)
#include "DNGuildWarManager.h"
#include "DNGuildVillage.h"
#include "DNPeriodQuestSystem.h"
#include "DNScriptAPICommon.h"

int CheckInvenForQuestReward(CDNUserBase* pUser, const TQuestReward& QuestReward, const bool bSelectedArray[])
{
	return DNScriptAPI::CheckInvenForQuestReward(pUser, QuestReward, bSelectedArray);
}

void DefAllAPIFunc(lua_State* pLuaState)
{
	DefineLuaAPI(pLuaState);

	LUA_TINKER_DEF(pLuaState, api_npc_NextTalk);
	LUA_TINKER_DEF(pLuaState, api_npc_NextScript);
	
	LUA_TINKER_DEF(pLuaState, api_npc_SetParamString);
	LUA_TINKER_DEF(pLuaState, api_npc_SetParamInt);
	LUA_TINKER_DEF(pLuaState, api_log_AddLog);
	LUA_TINKER_DEF(pLuaState, api_log_UserLog);
	LUA_TINKER_DEF(pLuaState, api_quest_DumpQuest);
	
	LUA_TINKER_DEF(pLuaState, api_quest_AddHuntingQuest);
	LUA_TINKER_DEF(pLuaState, api_quest_AddQuest);
	LUA_TINKER_DEF(pLuaState, api_quest_UserHasQuest);
	LUA_TINKER_DEF(pLuaState, api_quest_GetPlayingQuestCnt);
	
	LUA_TINKER_DEF(pLuaState, api_quest_CompleteQuest);
	LUA_TINKER_DEF(pLuaState, api_quest_MarkingCompleteQuest);
	LUA_TINKER_DEF(pLuaState, api_quest_IsMarkingCompleteQuest);
	
	
	LUA_TINKER_DEF(pLuaState, api_npc_GetNpcIndex);
	LUA_TINKER_DEF(pLuaState, api_quest_SetQuestStepAndJournalStep);
	LUA_TINKER_DEF(pLuaState, api_quest_SetQuestStep);
	LUA_TINKER_DEF(pLuaState, api_quest_GetQuestStep);
	LUA_TINKER_DEF(pLuaState, api_quest_SetJournalStep);
	LUA_TINKER_DEF(pLuaState, api_quest_GetJournalStep);
	LUA_TINKER_DEF(pLuaState, api_quest_SetQuestMemo);
	LUA_TINKER_DEF(pLuaState, api_quest_GetQuestMemo);
	LUA_TINKER_DEF(pLuaState, api_quest_CancelQuest);
	

	LUA_TINKER_DEF(pLuaState, api_quest_SetCountingInfo);
	LUA_TINKER_DEF(pLuaState, api_quest_ClearCountingInfo);
	LUA_TINKER_DEF(pLuaState, api_quest_IsAllCompleteCounting);
	LUA_TINKER_DEF(pLuaState, api_user_CheckInvenForAddItem);
	LUA_TINKER_DEF(pLuaState, api_user_CheckInvenForAddItemList);
	LUA_TINKER_DEF(pLuaState, api_user_AddItem);
	LUA_TINKER_DEF(pLuaState, api_user_DelItem);
	LUA_TINKER_DEF(pLuaState, api_user_AllDelItem);
	LUA_TINKER_DEF(pLuaState, api_user_HasItem);

	LUA_TINKER_DEF(pLuaState, api_user_GetUserClassID);
	LUA_TINKER_DEF(pLuaState, api_user_GetUserJobID);
	LUA_TINKER_DEF(pLuaState, api_user_GetUserLevel);
	LUA_TINKER_DEF(pLuaState, api_user_GetUserInvenBlankCount);
	LUA_TINKER_DEF(pLuaState, api_quest_GetUserQuestInvenBlankCount);
	
	LUA_TINKER_DEF(pLuaState, api_user_AddExp);
	LUA_TINKER_DEF(pLuaState, api_user_AddCoin);
	LUA_TINKER_DEF(pLuaState, api_user_PlayCutScene);
	LUA_TINKER_DEF(pLuaState, api_ui_OpenWareHouse);
	LUA_TINKER_DEF(pLuaState, api_ui_OpenShop);
	LUA_TINKER_DEF(pLuaState, api_ui_OpenSkillShop);
	LUA_TINKER_DEF(pLuaState, api_ui_OpenMarket);
	LUA_TINKER_DEF(pLuaState, api_ui_OpenCompoundEmblem);
	LUA_TINKER_DEF(pLuaState, api_ui_OpenUpgradeJewel);
	LUA_TINKER_DEF(pLuaState, api_ui_OpenMailBox);
	LUA_TINKER_DEF(pLuaState, api_ui_OpenUpgradeItem);
	LUA_TINKER_DEF(pLuaState, api_ui_OpenDisjointItem);
	LUA_TINKER_DEF(pLuaState, api_ui_OpenCompoundItem);
	LUA_TINKER_DEF(pLuaState, api_ui_OpenCashShop);
	LUA_TINKER_DEF(pLuaState, api_ui_OpenGuildMgrBox);
	LUA_TINKER_DEF(pLuaState, api_ui_OpenGacha_JP);
	LUA_TINKER_DEF(pLuaState, api_ui_OpenGiveNpcPresent);
	LUA_TINKER_DEF(pLuaState, api_ui_OpenDarkLairRankBoard);
	LUA_TINKER_DEF(pLuaState, api_ui_OpenPvPLadderRankBoard);
	LUA_TINKER_DEF(pLuaState, api_ui_OpenGlyphLift);
#ifdef PRE_ADD_OPEN_QUEST_TEXTURE_DIALOG
	LUA_TINKER_DEF(pLuaState, api_ui_OpenTextureDialog);
	LUA_TINKER_DEF(pLuaState, api_ui_CloseTextureDialog);
#endif
	LUA_TINKER_DEF(pLuaState, api_user_UserMessage);
	LUA_TINKER_DEF(pLuaState, api_quest_AddSymbolItem);
	LUA_TINKER_DEF(pLuaState, api_quest_DelSymbolItem);
	LUA_TINKER_DEF(pLuaState, api_quest_HasSymbolItem);
	
	LUA_TINKER_DEF(pLuaState, api_quest_CheckQuestInvenForAddItem);
	LUA_TINKER_DEF(pLuaState, api_quest_CheckQuestInvenForAddItemList);
	LUA_TINKER_DEF(pLuaState, api_quest_AddQuestItem);
	LUA_TINKER_DEF(pLuaState, api_quest_DelQuestItem);
	LUA_TINKER_DEF(pLuaState, api_quest_AllDelQuestItem);
	LUA_TINKER_DEF(pLuaState, api_quest_HasQuestItem);

	LUA_TINKER_DEF(pLuaState, api_ui_OpenGuildCreate);
	LUA_TINKER_DEF(pLuaState, api_ui_OpenQuestReward);
	LUA_TINKER_DEF(pLuaState, api_quest_RewardQuestUser);

	LUA_TINKER_DEF(pLuaState, api_user_ChangeChannel);
	LUA_TINKER_DEF(pLuaState, api_user_ChangeMap);
	LUA_TINKER_DEF(pLuaState, api_user_GetPartymemberCount);
	LUA_TINKER_DEF(pLuaState, api_user_IsPartymember);

	LUA_TINKER_DEF(pLuaState, api_user_RequestEnterPVP);

	LUA_TINKER_DEF(pLuaState, api_user_GetStageConstructionLevel);
	LUA_TINKER_DEF(pLuaState, api_user_GetMapIndex);
	LUA_TINKER_DEF(pLuaState, api_user_GetLastStageClearRank);
	LUA_TINKER_DEF(pLuaState, api_user_EnoughCoin );
	LUA_TINKER_DEF(pLuaState, api_user_GetCoin);
	LUA_TINKER_DEF(pLuaState, api_user_DelCoin);
	LUA_TINKER_DEF(pLuaState, api_user_IsMissionGained);
	LUA_TINKER_DEF(pLuaState, api_user_IsMissionAchieved);
	LUA_TINKER_DEF(pLuaState, api_user_HasItemWarehouse);
	LUA_TINKER_DEF(pLuaState, api_user_HasItemEquip);
	LUA_TINKER_DEF(pLuaState, api_guild_GetGuildMemberRole);
	LUA_TINKER_DEF(pLuaState, api_quest_IncQuestMemo);
	LUA_TINKER_DEF(pLuaState, api_quest_DecQuestMemo);
	LUA_TINKER_DEF(pLuaState, api_user_SetUserJobID);
	LUA_TINKER_DEF(pLuaState, api_user_IsJobInSameLine);
	LUA_TINKER_DEF(pLuaState, api_user_HasCashItem);
	LUA_TINKER_DEF(pLuaState, api_user_HasCashItemEquip);
	LUA_TINKER_DEF(pLuaState, api_quest_IncCounting);
	LUA_TINKER_DEF(pLuaState, api_quest_IsPlayingQuestMaximum);
	LUA_TINKER_DEF(pLuaState, api_quest_ForceCompleteQuest);
	LUA_TINKER_DEF(pLuaState, api_quest_ResetSkill);

	LUA_TINKER_DEF(pLuaState, api_quest_GetWorldEventStep);
	LUA_TINKER_DEF(pLuaState, api_quest_GetWorldEventCount);
	LUA_TINKER_DEF(pLuaState, api_quest_ReturnItemToNpc);
	LUA_TINKER_DEF(pLuaState, api_quest_OpenScoreWorldEvent);
	LUA_TINKER_DEF(pLuaState, api_quest_OpenFarmAreaList);

	LUA_TINKER_DEF(pLuaState, api_user_ChangePos);

	LUA_TINKER_DEF(pLuaState, api_npc_GetFavorPoint);
	LUA_TINKER_DEF(pLuaState, api_npc_GetMalicePoint);
	LUA_TINKER_DEF(pLuaState, api_npc_GetFavorPercent);
	LUA_TINKER_DEF(pLuaState, api_npc_GetMalicePercent);
	LUA_TINKER_DEF(pLuaState, api_npc_AddFavorPoint);
	LUA_TINKER_DEF(pLuaState, api_npc_AddMalicePoint);
	LUA_TINKER_DEF(pLuaState, api_npc_SendSelectedPresent);
	LUA_TINKER_DEF(pLuaState, api_npc_Rage);
	LUA_TINKER_DEF(pLuaState, api_npc_Disappoint);

	LUA_TINKER_DEF(pLuaState, api_user_ResetSkill);
	LUA_TINKER_DEF(pLuaState, api_npc_CreateSecondarySkill );
	LUA_TINKER_DEF(pLuaState, api_npc_CheckSecondarySkill );
	LUA_TINKER_DEF(pLuaState, api_ui_OpenGuildWareHouse );
	LUA_TINKER_DEF(pLuaState, api_guild_HasGuildWare );

	LUA_TINKER_DEF(pLuaState, api_ui_OpenMovieBrowser);

	LUA_TINKER_DEF(pLuaState, api_guildwar_IsPreparation );
	LUA_TINKER_DEF(pLuaState, api_guildwar_GetStep );
	LUA_TINKER_DEF(pLuaState, api_guildwar_GetTrialCurrentState );
	LUA_TINKER_DEF(pLuaState, api_guildWar_GetTrialStats );
	LUA_TINKER_DEF(pLuaState, api_guildWar_GetMatchList );
	LUA_TINKER_DEF(pLuaState, api_guildWar_GetPopularityVote );	
	LUA_TINKER_DEF(pLuaState, api_guildWar_GetFinalProgress );	

	LUA_TINKER_DEF(pLuaState, api_ui_OpenCompound2Item);
	LUA_TINKER_DEF(pLuaState, api_ui_OpenBrowser);

#if defined (PRE_ADD_DONATION)
	LUA_TINKER_DEF(pLuaState, api_ui_OpenDonation);
#endif // #if defined (PRE_ADD_DONATION)
	LUA_TINKER_DEF(pLuaState, api_ui_OpenInventory);
	LUA_TINKER_DEF(pLuaState, api_user_GetPCCafe);
	LUA_TINKER_DEF(pLuaState, api_user_SetPCCafeItem);
#if defined(PRE_ADD_REPAIR_NPC)
	LUA_TINKER_DEF(pLuaState, api_user_RepairItem);
#endif
#ifdef PRE_ADD_EXCHANGE_ENCHANT
	LUA_TINKER_DEF(pLuaState, api_ui_OpenExchangeEnchant);
#endif
#if defined(PRE_ADD_CHALLENGE_DARKLAIR)
	LUA_TINKER_DEF(pLuaState, api_env_CheckCloseGateByTime);
#endif
#ifdef PRE_ADD_COSRANDMIX_ACCESSORY
	LUA_TINKER_DEF(pLuaState, api_ui_OpenCostumeRandomMix);
#endif
#if defined( PRE_DRAGONBUFF )
	LUA_TINKER_DEF(pLuaState, api_quest_WorldBuffCheck);
	LUA_TINKER_DEF(pLuaState, api_quest_ApplyWorldBuff);
#endif
#if defined( PRE_ADD_DWC )
	LUA_TINKER_DEF(pLuaState, api_ui_OpenCreateDwcTeamDlg);
#endif

	LUA_TINKER_DEF(pLuaState, api_ui_OpenJobChange);
}

CDNUserSession* __GetUserObj(UINT nUserObjectID)
{
	return g_pUserSessionManager->FindUserSessionByObjectID(nUserObjectID);
}

// desc : 인벤토리의 특정 빈슬롯에 아이템을 세팅 (P.S.> api_user_CheckInvenForAddItemList 에서 사용됨)
bool __SetBlankInventorySlot(TInvenItemCnt pTempInven[INVENTORYMAX], const TItem& Item, int iIndex)
{
	DN_ASSERT(NULL != pTempInven,	"Invalid!");
	DN_ASSERT(0 != Item.nItemID,			"Invalid!");
	DN_ASSERT(0 < Item.wCount,			"Invalid!");
	DN_ASSERT(-1 < iIndex,			"Invalid!");

	if (pTempInven[iIndex].nItemID != 0) {
		return false;
	}

	pTempInven[iIndex].Set( Item.nItemID, Item.wCount, Item.bSoulbound, Item.cSealCount );

	return true;
}

// desc : 인벤토리의 빈슬롯들에 아이템을 특정 개수만큼 세팅 (P.S.> api_user_CheckInvenForAddItemList 에서 사용됨)
bool __SetBlankInventorySlotCount(TInvenItemCnt pTempInven[INVENTORYMAX], const TItem& Item, int nOverlapCount, int nCount /* 채울 슬롯개수 */)
{
	DN_ASSERT(NULL != pTempInven,	"Invalid!");
	DN_ASSERT(0 != Item.nItemID,			"Invalid!");
	DN_ASSERT(0 < Item.wCount,			"Invalid!");
	DN_ASSERT(0 < nOverlapCount,	"Invalid!");
	DN_ASSERT(0 < nCount,			"Invalid!");

	// !!! 주의 - 본 함수 호출 전에 체크가 확실히 되었음을 전제하고 세팅하는 부분만 수행

	int nBundle = Item.wCount / nOverlapCount;
	int nRemain = Item.wCount % nOverlapCount;

	for (int iIndex = 0 ; INVENTORYMAX > iIndex ; ++iIndex) 
	{
		if (0 < nBundle) {
			if (pTempInven[iIndex].nItemID == 0) 
			{
				pTempInven[iIndex].Set( Item.nItemID, nOverlapCount, Item.bSoulbound, Item.cSealCount );
				--nBundle;
				continue;
			}
		}

		if (0 < nRemain) 
		{
			if( CDNUserItem::bIsSameItem( &Item, &pTempInven[iIndex] ) && nOverlapCount >= pTempInven[iIndex].wCount + nRemain )
			{
				pTempInven[iIndex].wCount += nRemain;
				nRemain = 0;
				continue;
			}
		}
	}

	if (0 < nBundle) 
	{
		return false;
	}
	if (0 < nRemain) 
	{
		for (int iIndex = 0 ; INVENTORYMAX > iIndex ; ++iIndex) 
		{
			if (pTempInven[iIndex].nItemID == 0) {
				pTempInven[iIndex].Set( Item.nItemID, nRemain, Item.bSoulbound, Item.cSealCount );
				nRemain = 0;
				break;
			}
		}
	}
	if (0 < nRemain) 
	{
		return false;
	}

	return true;
}

// desc : 인벤토리의 특정 슬롯에 아이템을 중첩 세팅 (P.S.> api_user_CheckInvenForAddItemList 에서 사용됨)
bool __SetOverlapInventorySlot(TInvenItemCnt pTempInven[INVENTORYMAX], const TItem& Item, int nOverlapCount, int iIndex)
{
	DN_ASSERT(NULL != pTempInven,	"Invalid!");
	DN_ASSERT(0 != Item.nItemID,			"Invalid!");
	DN_ASSERT(0 < Item.wCount,			"Invalid!");
	DN_ASSERT(0 < nOverlapCount,	"Invalid!");
	DN_ASSERT(-1 < iIndex,			"Invalid!");

	if ( CDNUserItem::bIsDifferentItem( &Item, &pTempInven[iIndex] ) || nOverlapCount < pTempInven[iIndex].wCount + Item.wCount )
	{
		return false;
	}

	pTempInven[iIndex].wCount += Item.wCount;

	return true;
}

// desc : 인벤토리에 빈슬롯 찾기 (P.S.> api_user_CheckInvenForAddItemList 에서 사용됨)
int __FindBlankInventorySlot(TInvenItemCnt pTempInven[INVENTORYMAX], int nTempInvenCount)
{
	DN_ASSERT(NULL != pTempInven,								"Invalid!");
	DN_ASSERT(CHECK_RANGE(nTempInvenCount, 0, INVENTORYMAX),	"Invalid!");

	for (int iIndex = 0 ; nTempInvenCount > iIndex ; ++iIndex) {
		if (pTempInven[iIndex].nItemID == 0) {
			return iIndex;
		}
	}

	return -1;
}

// desc : 인벤토리에 빈슬롯이 몇 개나 있는지 개수 찾기 (P.S.> api_user_CheckInvenForAddItemList 에서 사용됨)
int __FindBlankInventorySlotCount(TInvenItemCnt pTempInven[INVENTORYMAX], int nTempInvenCount)
{
	DN_ASSERT(NULL != pTempInven,								"Invalid!");
	DN_ASSERT(CHECK_RANGE(nTempInvenCount, 0, INVENTORYMAX),	"Invalid!");

	int nCount = 0;
	for (int iIndex = 0 ; nTempInvenCount > iIndex ; ++iIndex) {
		if (pTempInven[iIndex].nItemID == 0) {
			++nCount;
		}
	}

	return nCount;
}

// desc : 인벤토리에 셀 수 있는 아이템 중 남은 공간에 wCount 넣을 수 있는 Index 찾기 (P.S.> api_user_CheckInvenForAddItemList 에서 사용됨)
int __FindOverlapInventorySlot(TInvenItemCnt pTempInven[INVENTORYMAX], int nTempInvenCount, const TItem& Item)
{
	DN_ASSERT(NULL != pTempInven,								"Invalid!");
	DN_ASSERT(CHECK_RANGE(nTempInvenCount, 0, INVENTORYMAX),	"Invalid!");

	if ((Item.nItemID <= 0) || (Item.wCount <= 0)) DN_RETURN(-1);

	TItemData *pItemData = NULL;
	for (int iIndex = 0 ; nTempInvenCount > iIndex ; ++iIndex) {
		if (pTempInven[iIndex].nItemID == 0) {
			// 빈칸이면 패스
			continue;
		}

		pItemData = g_pDataManager->GetItemData(pTempInven[iIndex].nItemID);
		if (!pItemData) continue;

		if (pItemData->nOverlapCount == 1) continue;			// 겹치지 않으면 패스
		if( CDNUserItem::bIsDifferentItem( &Item, &pTempInven[iIndex] ) ) continue;	// 같은 아이템이 아님

		if (pItemData->nOverlapCount >= pTempInven[iIndex].wCount + Item.wCount) {
			return iIndex;
		}
	}

	return -1;
}

// desc : 인벤토리에 아이템을 넣을 수 있는 공간이 충분한지 검사 (P.S.> api_user_CheckInvenForAddItemList 에서 사용됨)
bool __IsValidSpaceInventorySlotAndSet(TInvenItemCnt pTempInven[INVENTORYMAX], int nTempInvenCount, const TItem& Item )
{
	DN_ASSERT(NULL != pTempInven,								"Invalid!");
	DN_ASSERT(CHECK_RANGE(nTempInvenCount, 0, INVENTORYMAX),	"Invalid!");

	if ((Item.nItemID <= 0) || (Item.wCount <= 0)) 
		DN_RETURN(false);

	TItemData *pItem = g_pDataManager->GetItemData(Item.nItemID);
	if (!pItem) return false;

	if (ITEMTYPE_QUEST == pItem->nType) {
		return false;
	}

	int nCount = 0;
	if (pItem->nOverlapCount == 1){		// 겹치지 않는 아이템
		nCount = __FindBlankInventorySlotCount(pTempInven, nTempInvenCount);
		if (Item.wCount > nCount) {
			// 빈슬롯이랑 비교하기
			return false;
		}

		return(__SetBlankInventorySlotCount(pTempInven, Item, 1, nCount));
	}
	else {
		// 겹쳐지는 아이템 (인벤에 동일 아이템들이 몇개씩 남아있어도 신경안쓰고 깔끔하게 넣는 경우만 생각하쟈)
		if (pItem->nOverlapCount < Item.wCount){		// 한번에 겹치는 양보다 많을경우
			int nBundle = Item.wCount / pItem->nOverlapCount;
			if ((Item.wCount % pItem->nOverlapCount) > 0) nBundle++;

			nCount = __FindBlankInventorySlotCount(pTempInven, nTempInvenCount);
			if (nBundle > nCount) {
				// 묶음이랑 빈슬롯이랑 비교하기
				return false;
			}

			return(__SetBlankInventorySlotCount(pTempInven, Item, pItem->nOverlapCount, nCount));
		}
		else {
			int nBlank = __FindOverlapInventorySlot(pTempInven, nTempInvenCount, Item );	// 기존 아이템중에 한방에 들어갈 아이템이 있는지 
			if (nBlank < 0){
				nBlank = __FindBlankInventorySlot(pTempInven, nTempInvenCount);
				if (nBlank < 0) {
					return false;
				}

				return(__SetBlankInventorySlot(pTempInven, Item, nBlank));
			}

			return(__SetOverlapInventorySlot(pTempInven, Item, pItem->nOverlapCount, nBlank));
		}
	}

	return true;
}

// desc : 퀘스트 인벤토리의 특정 빈슬롯에 아이템을 세팅 (P.S.> api_quest_CheckQuestInvenForAddItemList 에서 사용됨)
bool __SetBlankQuestInventorySlot(TQuestItemCnt pTempQuestInven[QUESTINVENTORYMAX], int nItemID, short wCount, int iIndex)
{
	DN_ASSERT(NULL != pTempQuestInven,	"Invalid!");
	DN_ASSERT(0 != nItemID,				"Invalid!");
	DN_ASSERT(0 < wCount,				"Invalid!");
	DN_ASSERT(-1 < iIndex,				"Invalid!");

	if (pTempQuestInven[iIndex].nItemID != 0) {
		return false;
	}

	pTempQuestInven[iIndex].Set(nItemID, wCount);

	return true;
}

// desc : 퀘스트 인벤토리의 빈슬롯들에 아이템을 특정 개수만큼 세팅 (P.S.> api_quest_CheckQuestInvenForAddItemList 에서 사용됨)
bool __SetBlankQuestInventorySlotCount(TQuestItemCnt pTempQuestInven[QUESTINVENTORYMAX], int nItemID, short wCount, int nOverlapCount, int nCount /* 채울 슬롯개수 */)
{
	DN_ASSERT(NULL != pTempQuestInven,	"Invalid!");
	DN_ASSERT(0 != nItemID,				"Invalid!");
	DN_ASSERT(0 < wCount,				"Invalid!");
	DN_ASSERT(0 < nOverlapCount,		"Invalid!");
	DN_ASSERT(0 < nCount,				"Invalid!");

	// !!! 주의 - 본 함수 호출 전에 체크가 확실히 되었음을 전제하고 세팅하는 부분만 수행

	int nBundle = wCount / nOverlapCount;
	int nRemain = wCount % nOverlapCount;

	for (int iIndex = 0 ; QUESTINVENTORYMAX > iIndex ; ++iIndex) {
		if (0 < nBundle) {
			if (pTempQuestInven[iIndex].nItemID == 0) {
				pTempQuestInven[iIndex].Set(nItemID, nOverlapCount);
				--nBundle;
				continue;
			}
		}

		if (0 < nRemain) {
			if (pTempQuestInven[iIndex].nItemID == nItemID &&
				nOverlapCount >= pTempQuestInven[iIndex].wCount + nRemain
				)
			{
				pTempQuestInven[iIndex].wCount += nRemain;
				nRemain = 0;
				continue;
			}
		}
	}

	if (0 < nBundle) {
		return false;
	}
	if (0 < nRemain) {
		for (int iIndex = 0 ; QUESTINVENTORYMAX > iIndex ; ++iIndex) {
			if (pTempQuestInven[iIndex].nItemID == 0) {
				pTempQuestInven[iIndex].Set(nItemID, nRemain);
				nRemain = 0;
				break;
			}
		}
	}
	if (0 < nRemain) {
		return false;
	}

	return true;
}

// desc : 퀘스트 인벤토리의 특정 슬롯에 아이템을 중첩 세팅 (P.S.> api_quest_CheckQuestInvenForAddItemList 에서 사용됨)
bool __SetOverlapQuestInventorySlot(TQuestItemCnt pTempQuestInven[QUESTINVENTORYMAX], int nItemID, short wCount, int nOverlapCount, int iIndex)
{
	DN_ASSERT(NULL != pTempQuestInven,	"Invalid!");
	DN_ASSERT(0 != nItemID,				"Invalid!");
	DN_ASSERT(0 < wCount,				"Invalid!");
	DN_ASSERT(0 < nOverlapCount,		"Invalid!");
	DN_ASSERT(-1 < iIndex,				"Invalid!");

	if (pTempQuestInven[iIndex].nItemID != nItemID || 
		nOverlapCount < pTempQuestInven[iIndex].wCount + wCount
		)
	{
		return false;
	}

	pTempQuestInven[iIndex].wCount += wCount;

	return true;
}

// desc : 퀘스트 인벤토리에 빈슬롯이 몇 개나 있는지 개수 찾기 (P.S.> api_quest_CheckQuestInvenForAddItemList 에서 사용됨)
int __FindBlankQuestInventorySlotCount(TQuestItemCnt pTempQuestInven[QUESTINVENTORYMAX])
{
	DN_ASSERT(NULL != pTempQuestInven,	"Invalid!");

	int nCount = 0;
	for (int iIndex = 0 ; QUESTINVENTORYMAX > iIndex ; ++iIndex) {
		if (pTempQuestInven[iIndex].nItemID == 0) {
			++nCount;
		}
	}

	return nCount;
}

// desc : 퀘스트 인벤토리에 빈슬롯 찾기 (P.S.> api_quest_CheckQuestInvenForAddItemList 에서 사용됨)
int __FindBlankQuestInventorySlot(TQuestItemCnt pTempQuestInven[QUESTINVENTORYMAX])
{
	DN_ASSERT(NULL != pTempQuestInven,	"Invalid!");

	for (int iIndex = 0 ; QUESTINVENTORYMAX > iIndex ; ++iIndex) {
		if (pTempQuestInven[iIndex].nItemID == 0) {
			return iIndex;
		}
	}

	return -1;
}

// desc : 퀘스트 인벤토리에 셀 수 있는 아이템 중 남은 공간에 wCount 넣을 수 있는 Index 찾기 (P.S.> api_quest_CheckQuestInvenForAddItemList 에서 사용됨)
int __FindOverlapQuestInventorySlot(TQuestItemCnt pTempQuestInven[QUESTINVENTORYMAX], int nItemID, short wCount)
{
	DN_ASSERT(NULL != pTempQuestInven,	"Invalid!");

	if ((nItemID <= 0) || (wCount <= 0)) DN_RETURN(-1);

	bool boFlag = false;
	TItemData *pQuestItem = NULL;
	for (int iIndex = 0 ; QUESTINVENTORYMAX > iIndex ; ++iIndex) {
		if (pTempQuestInven[iIndex].nItemID == 0) continue;	// 빈칸이면 패스

		pQuestItem = g_pDataManager->GetItemData(pTempQuestInven[iIndex].nItemID);
		if (!pQuestItem) continue;

		if (pQuestItem->nOverlapCount == 1) continue;				// 겹치지 않으면 패스
		if (pTempQuestInven[iIndex].nItemID != nItemID) continue;	// 같은 아이템이 아니다

		if (pQuestItem->nOverlapCount >= pTempQuestInven[iIndex].wCount + wCount){
			return iIndex;
		}
	}

	return -1;
}

// desc : 퀘스트 인벤토리에 아이템을 넣을 수 있는 공간이 충분한지 검사 (P.S.> api_quest_CheckQuestInvenForAddItemList 에서 사용됨)
bool __IsValidSpaceQuestInventorySlotAndSet(TQuestItemCnt pTempQuestInven[QUESTINVENTORYMAX], int nItemID, short wCount)
{
	DN_ASSERT(NULL != pTempQuestInven,	"Invalid!");

	if ((nItemID <= 0) || (wCount <= 0)) DN_RETURN(false);

	TItemData *pItem = g_pDataManager->GetItemData(nItemID);
	if (!pItem) return false;

	if (ITEMTYPE_QUEST != pItem->nType) {
		return false;
	}

	int nCount = 0;
	if (pItem->nOverlapCount == 1){		// 겹치지 않는 아이템
		nCount = __FindBlankQuestInventorySlotCount(pTempQuestInven);
		if (wCount > nCount) {
			// 빈슬롯이랑 비교하기
			return false;
		}

		return(__SetBlankQuestInventorySlotCount(pTempQuestInven, nItemID, wCount, 1, nCount));
	}
	else {	// 겹치는 아이템
		int nRemain = 0, nBlank = 0, nBundle = 0;

		int nCount = __FindBlankQuestInventorySlotCount(pTempQuestInven);	// 빈 슬롯개수

		if (pItem->nOverlapCount >= wCount){
			nRemain = wCount;
		}
		else {
			nBundle = wCount / pItem->nOverlapCount;
			nRemain = wCount % pItem->nOverlapCount;

			if (nBundle > nCount) return false;	// 공간부족
		}

		if (nRemain > 0){
			nBlank = __FindOverlapQuestInventorySlot(pTempQuestInven, nItemID, nRemain);	// 짜투리가 들어갈 공간이 있는지
			if (nBlank < 0){	// 짜투리 공간은 없다
				if ((nBundle + 1) > nCount) return false;	// 한칸의 여유가 더 있는지...
			}
		}

		return(__SetBlankQuestInventorySlotCount(pTempQuestInven, nItemID, wCount, pItem->nOverlapCount, nCount));
	}

	return true;
}

void api_log_AddLog(const char* szLog)
{
	DNScriptAPI::api_log_AddLog(szLog);
}

void api_log_UserLog(UINT nUserObjectID, const char* szLog)
{
#ifndef _FINAL_BUILD
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if ( !pUser ) return;

	DNScriptAPI::api_log_UserLog(pUser, szLog);
#endif // _FINAL_BUILD
}

void api_quest_DumpQuest(UINT nUserObjectID)
{
#ifndef _FINAL_BUILD
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if ( !pUser ) return;

	DNScriptAPI::api_quest_DumpQuest(pUser);
#endif // _FINAL_BUILD
}

void api_npc_NextTalk(UINT nUserObjectID, UINT nNpcObjectID , const char* szTalkIndex, const char* szTargetFile)
{
	CDNUserBase* pUser = __GetUserObj(nUserObjectID);
	if ( !pUser ) return;

	DNScriptAPI::api_npc_NextTalk(pUser, nNpcObjectID, szTalkIndex, szTargetFile);
}

void api_npc_NextScript(UINT nUserObjectID, UINT nNpcObjectID, const char* szTalkIndex, const char* szTargetFile)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if (!pUser)
	{
		g_Log.Log(LogType::_ERROR, pUser, L"[CS_NPCTALK] (5-1) UID:%d, PAK:%d/%s/%s\n", nUserObjectID, nNpcObjectID, szTalkIndex,szTargetFile);
		return;
	}
	
	CDNNpcObject* pNpc = g_pFieldManager->GetNpcObjectByNpcObjID(pUser->GetChannelID(), nNpcObjectID);
	if ( !pNpc ) return;

	DNScriptAPI::api_npc_NextScript(pUser, pNpc, nNpcObjectID, szTalkIndex, szTargetFile);
}

int api_npc_SetParamString(UINT nUserObjectID, const char* szParamKey, int nParamID)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if ( !pUser ) return -1;

	return DNScriptAPI::api_npc_SetParamString(pUser, szParamKey, nParamID);
}

int api_npc_SetParamInt(UINT nUserObjectID, const char* szParamKey, int nValue)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if ( !pUser ) return -1;

	return DNScriptAPI::api_npc_SetParamInt(pUser, szParamKey, nValue);
}

int api_quest_AddHuntingQuest(UINT nUserObjectID, int nQuestID, int nQuestStep, int nJournalStep, int nCountingSlot, int nCountingType, int nCountingIndex, int nTargetCnt)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if ( !pUser ) return -1;

	return DNScriptAPI::api_quest_AddHuntingQuest(pUser, nQuestID, nQuestStep, nJournalStep, nCountingSlot, nCountingType, nCountingIndex, nTargetCnt);
}

// nQuestType : 사용하지 않는 매개변수임.
int api_quest_AddQuest(UINT nUserObjectID, int nQuestID, int nQuestType)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if ( !pUser ) return -1;

	return DNScriptAPI::api_quest_AddQuest(pUser, nQuestID, nQuestType);
}

int api_quest_CompleteQuest(UINT nUserObjectID, int nQuestID, bool bDelPlayList, bool bRepeat)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if ( !pUser ) return -1;

	return DNScriptAPI::api_quest_CompleteQuest(pUser, nQuestID, bDelPlayList);
}

int api_quest_MarkingCompleteQuest(UINT nUserObjectID, int nQuestID)
{
	// 보상퀘스트의 마킹을 api_quest_CompleteQuest으로 통합하기 위해 더이상 이 API는 사용하지 않는다.
	return 1;
}

int api_quest_IsMarkingCompleteQuest(UINT nUserObjectID, int nQuestID)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if ( !pUser ) 
		return -1;

	return DNScriptAPI::api_quest_IsMarkingCompleteQuest(pUser, nQuestID);
}

int api_quest_UserHasQuest(UINT nUserObjectID, int nQuestID)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if ( !pUser ) 
		return -1;

	return DNScriptAPI::api_quest_UserHasQuest(pUser, nQuestID);
}

int api_quest_GetPlayingQuestCnt(UINT nUserObjectID)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if ( !pUser ) 
		return -1;

	return DNScriptAPI::api_quest_GetPlayingQuestCnt(pUser);
}

int api_npc_GetNpcIndex(UINT nUserObjectID, UINT nNpcObjID)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if ( !pUser ) 
		return -1;

	CDNNpcObject* pNpc = g_pFieldManager->GetNpcObjectByNpcObjID(pUser->GetChannelID(), nNpcObjID);
	if ( !pNpc ) 
		return -2;

	return DNScriptAPI::api_npc_GetNpcIndex(pNpc);
}

int api_quest_SetQuestStepAndJournalStep(UINT nUserObjectID, int nQuestID, short nQuestStep, int nJournalStep)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if ( !pUser ) return -1;

	return DNScriptAPI::api_quest_SetQuestStepAndJournalStep(pUser, nQuestID, nQuestStep, nJournalStep);
}

int api_quest_SetQuestStep(UINT nUserObjectID, int nQuestID, short nQuestStep)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if ( !pUser ) 
		return -1;

	return DNScriptAPI::api_quest_SetQuestStep(pUser, nQuestID, nQuestStep);
}

int api_quest_GetQuestStep(UINT nUserObjectID, int nQuestID)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if ( !pUser ) 
		return -1;

	return DNScriptAPI::api_quest_GetQuestStep(pUser, nQuestID);
}

int api_quest_SetJournalStep(UINT nUserObjectID, int nQuestID, int nJournalStep)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if ( !pUser ) 
		return -1;

	return DNScriptAPI::api_quest_SetJournalStep(pUser, nQuestID, nJournalStep);
}

int api_quest_GetJournalStep(UINT nUserObjectID, int nQuestID)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if ( !pUser ) 
		return -1;

	return DNScriptAPI::api_quest_GetJournalStep(pUser, nQuestID);
}

int api_quest_SetQuestMemo(UINT nUserObjectID, int nQuestID, char nMemoIndex, TP_QUESTMEMO nVal)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if ( !pUser ) {
		return -1;
	}

	return DNScriptAPI::api_quest_SetQuestMemo(pUser, nQuestID, nMemoIndex, nVal);
}

int api_quest_GetQuestMemo(UINT nUserObjectID, int nQuestID, char nMemoIndex)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if ( !pUser ) {
		return -1;
	}

	return DNScriptAPI::api_quest_GetQuestMemo(pUser, nQuestID, nMemoIndex);
}

int api_quest_SetCountingInfo(UINT nUserObjectID, int nQuestID, int nSlot, int nCountingType, int nCountingIndex, int nTargetCnt)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if ( !pUser ) 
		return -1;

	return DNScriptAPI::api_quest_SetCountingInfo(pUser, nQuestID, nSlot, nCountingType, nCountingIndex, nTargetCnt);
}

int api_quest_ClearCountingInfo(UINT nUserObjectID, int nQuestID)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if ( !pUser ) 
		return -1;

	return DNScriptAPI::api_quest_ClearCountingInfo(pUser, nQuestID);
}

int api_quest_IsAllCompleteCounting(UINT nUserObjectID, int nQuestID)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if ( !pUser ) 
		return -1;

	return DNScriptAPI::api_quest_IsAllCompleteCounting(pUser, nQuestID);
}

int api_quest_CancelQuest(UINT nUserObjectID, int nQuestID)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if ( !pUser ) return -1;
#if defined(_CH)
	if (pUser->GetFCMState() != FCMSTATE_NONE){
		pUser->SendQuestResult(ERROR_QUEST_FCM);
		return -1;
	}
#endif

	pUser->GetQuest()->OnCancelQuest(nQuestID);

	return 1;
}

int api_user_CheckInvenForAddItem(UINT nUserObjectID, int nItemIndex, int nItemCnt)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if ( !pUser ) 
		return -1;

	return DNScriptAPI::api_user_CheckInvenForAddItem(pUser, nItemIndex, nItemCnt);
}

int api_user_CheckInvenForAddItemList(UINT nUserObjectID, lua_tinker::table ItemTable)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if (!pUser) {
		return -1;
	}

	return DNScriptAPI::api_user_CheckInvenForAddItemList(pUser, ItemTable);
}

int api_user_AddItem(UINT nUserObjectID, int nItemIndex, int nItemCnt, int nQuestID)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if ( !pUser ) 
		return -1;

	return DNScriptAPI::api_user_AddItem(pUser, nItemIndex, nItemCnt, nQuestID);
}

int api_user_DelItem(UINT nUserObjectID, int nItemIndex, int nItemCnt, int nQuestID)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if ( !pUser ) 
		return -1;

	return DNScriptAPI::api_user_DelItem(pUser, nItemIndex, nItemCnt, nQuestID);
}

int api_user_AllDelItem(UINT nUserObjectID, int nItemIndex)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if ( !pUser ) 
		return -1;

	return DNScriptAPI::api_user_AllDelItem(pUser, nItemIndex);
}

int api_user_HasItem(UINT nUserObjectID, int nItemIndex, int nItemCnt)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if ( !pUser ) 
		return -1;

	return DNScriptAPI::api_user_HasItem(pUser, nItemIndex, nItemCnt);
}

int api_user_GetUserClassID(UINT nUserObjectID)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if ( !pUser ) return -1;

	return DNScriptAPI::api_user_GetUserClassID(pUser);
}

int api_user_GetUserJobID(UINT nUserObjectID)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if ( !pUser ) return -1;

	return DNScriptAPI::api_user_GetUserJobID(pUser);
}

int api_user_GetUserLevel(UINT nUserObjectID)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if ( !pUser ) return -1;

	return DNScriptAPI::api_user_GetUserLevel(pUser);
}

int api_user_GetUserInvenBlankCount(UINT nUserObjectID)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if ( !pUser ) 
		return -1;

	return DNScriptAPI::api_user_GetUserInvenBlankCount(pUser);
}

int api_quest_GetUserQuestInvenBlankCount(UINT nUserObjectID)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if ( !pUser ) 
		return -1;

	return DNScriptAPI::api_quest_GetUserQuestInvenBlankCount(pUser);
}

/*
desc   : 유저에게 경험치를 추가한다.
param  : 유저인덱스, 경험치
return : -1 => 유저를 찾을수 없음, 
*/
int api_user_AddExp(UINT nUserObjectID, int nQuestID, int nAddExp)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if ( !pUser ) 
		return -1;

#if defined(_CH)
	if (pUser->GetFCMState() != FCMSTATE_NONE){
		pUser->SendQuestResult(ERROR_QUEST_FCM);
		return -1;
	}
#endif

	pUser->ChangeExp(nAddExp, DBDNWorldDef::CharacterExpChangeCode::Quest, nQuestID);

	int nExp = pUser->GetExp();

#ifndef _FINAL_BUILD
	std::wstring wszMsg;
	wszMsg = FormatW( L"경험치추가 : %d 최종 : %d" , nAddExp, nExp);
	pUser->SendChat(CHATTYPE_NORMAL, (int)wszMsg.size()*sizeof(WCHAR), L"", (WCHAR*)wszMsg.c_str());
#endif
	
	return 1;
}

/*
desc   : 유저에게 돈을 추가한다.
param  : 유저인덱스, 돈
return : -1 => 유저를 찾을수 없음, 
*/
int api_user_AddCoin(UINT nUserObjectID, int nAddCoin, int nQuestID)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if ( !pUser ) 
		return -1;

#if defined(_CH)
	if (pUser->GetFCMState() != FCMSTATE_NONE){
		pUser->SendQuestResult(ERROR_QUEST_FCM);
		return -1;
	}
#endif

	if (!pUser->AddCoin(nAddCoin, DBDNWorldDef::CoinChangeCode::QuestReward, nQuestID))
		return -1;

	INT64 nCoin = pUser->GetCoin();

#ifndef _FINAL_BUILD
	std::wstring wszMsg;
	wszMsg = FormatW( L"코인추가 : %d 최종 : %lld" , nAddCoin, nCoin);
	pUser->SendChat(CHATTYPE_NORMAL, (int)wszMsg.size()*sizeof(WCHAR), L"", (WCHAR*)wszMsg.c_str());
#endif

	return 1;
}

int api_user_PlayCutScene(UINT nUserObjectID, UINT nNpcObjectID, int nCutSceneTableID, bool bIgnoreFadeIn)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if ( !pUser ) 
		return -1;

	return DNScriptAPI::api_user_PlayCutScene(pUser, nNpcObjectID, nCutSceneTableID, bIgnoreFadeIn);
}

int api_ui_OpenWareHouse(UINT nUserObjectID, int iItemID/*=0*/ )
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if ( !pUser ) 
		return -1;

	return DNScriptAPI::api_ui_OpenWareHouse(pUser,iItemID);
}

/*
desc   : 유저에게 상점를 열라고 알린다.
param  : 유저인덱스, 상점 ID
return : -1 => 유저를 찾을수 없음, -2 면 샵이 없음. 1이면 성공
*/
int api_ui_OpenShop(UINT nUserObjectID, int nShopID, Shop::Type::eCode Type/*=Shop::Type::Normal*/ )
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if ( !pUser ) 
		return -1;
#if defined (PRE_MOD_GAMESERVERSHOP)
	return DNScriptAPI::api_ui_OpenShop(pUser, nShopID, Type);
#else

#ifndef _FINAL_BUILD
	std::wstring wszLog;
	wszLog = FormatW( L"샵열기");
	pUser->SendChat(CHATTYPE_NORMAL, (int)wszLog.size()*sizeof(WCHAR), L"", (WCHAR*)wszLog.c_str());
#endif

	TShopData *pShopData = g_pDataManager->GetShopData(nShopID);
	if ( !pShopData )
	{
		if( false == g_pDataManager->IsCombinedShop( nShopID ) )
			return -2;
	}

#if defined(PRE_ADD_REMOTE_OPENSHOP)
	pUser->m_bRemoteShopOpen = false;
#endif	// #if defined(PRE_ADD_REMOTE_OPENSHOP)
	pUser->m_nShopID = nShopID;
	pUser->SendShopOpen(nShopID,Type);
	pUser->SetShopType( Type );
	pUser->SetCalledNpcResponse(true, false);	// 보통 클라이언트에 보내는 UI 개설 요청 이후 대화로 이어지지 않기 때문에 에러가 아닌데도 본의아니게 NPC 응답여부 체크에 걸리게 되므로 대화한 것으로 처리해 줌

	return 1;
#endif
}



/*
desc   : 유저에게 스킬 상점를 열라고 알린다.
param  : 유저인덱스, 스킬 상점 ID
return : -1 => 유저를 찾을수 없음, -2 면 샵이 없음. 1이면 성공
*/
int api_ui_OpenSkillShop( UINT nUserObjectID )
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if ( !pUser ) 
		return -1;

#if defined (PRE_MOD_GAMESERVERSHOP)
	return DNScriptAPI::api_ui_OpenSkillShop(pUser);
#else
	pUser->SendSkillShopOpen();
	pUser->SetCalledNpcResponse(true, false);	// 보통 클라이언트에 보내는 UI 개설 요청 이후 대화로 이어지지 않기 때문에 에러가 아닌데도 본의아니게 NPC 응답여부 체크에 걸리게 되므로 대화한 것으로 처리해 줌

	return 1;
#endif
}

/*
desc   : 유저에게 무인상점을 열라고 알린다.
param  : 유저인덱스
return : -1 => 유저를 찾을수 없음, -2 면 샵이 없음. 1이면 성공
*/
int api_ui_OpenMarket(UINT nUserObjectID)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if ( !pUser ) 
		return -1;

#ifndef _FINAL_BUILD
	std::wstring wszLog;
	wszLog = FormatW( L"무인상점 열기");
	pUser->SendChat(CHATTYPE_NORMAL, (int)wszLog.size()*sizeof(WCHAR), L"", (WCHAR*)wszLog.c_str());
#endif

	pUser->SendMarketList(NULL, 0, 0, ERROR_NONE);
	pUser->SetCalledNpcResponse(true, false);	// 보통 클라이언트에 보내는 UI 개설 요청 이후 대화로 이어지지 않기 때문에 에러가 아닌데도 본의아니게 NPC 응답여부 체크에 걸리게 되므로 대화한 것으로 처리해 줌

	return 1;
}

int api_ui_OpenCompoundEmblem(UINT nUserObjectID)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if ( !pUser ) 
		return -1;

	return DNScriptAPI::api_ui_OpenCompoundEmblem(pUser);
}

int api_ui_OpenUpgradeJewel(UINT nUserObjectID)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if ( !pUser ) 
		return -1;

	return DNScriptAPI::api_ui_OpenUpgradeJewel(pUser);
}

int api_ui_OpenGlyphLift(UINT nUserObjectID)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if ( !pUser ) 
		return -1;

	return DNScriptAPI::api_ui_OpenGlyphLift(pUser);
}

int api_ui_OpenMailBox(UINT nUserObjectID)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if ( !pUser ) 
		return -1;

	return DNScriptAPI::api_ui_OpenMailBox(pUser);
}

/*
desc   : 유저에게 아이템 강화창을 열라고 시킨다.
param  : 유저인덱스
return : -1 => 유저를 찾을수 없음, 1이면 성공
*/

int api_ui_OpenUpgradeItem(UINT nUserObjectID,int iItemID/*=0*/ )
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if ( !pUser ) 
		return -1;

#ifndef _FINAL_BUILD
	std::wstring wszLog;
	wszLog = FormatW( L"아이템 강화창 열기");
	pUser->SendChat(CHATTYPE_NORMAL, (int)wszLog.size()*sizeof(WCHAR), L"", (WCHAR*)wszLog.c_str());
#endif

	pUser->SendOpenUpgradeItem( iItemID );

	pUser->SetCalledNpcResponse(true, false);	// 보통 클라이언트에 보내는 UI 개설 요청 이후 대화로 이어지지 않기 때문에 에러가 아닌데도 본의아니게 NPC 응답여부 체크에 걸리게 되므로 대화한 것으로 처리해 줌

	pUser->SetRemoteEnchantItemID(iItemID);
	return 1;
}

int api_ui_OpenDisjointItem(UINT nUserObjectID)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if ( !pUser ) 
		return -1;

	return DNScriptAPI::api_ui_OpenDisjointItem(pUser);
}

int api_ui_OpenCompoundItem(UINT nUserObjectID, int nCompoundShopID)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if ( !pUser ) 
		return -1;

	return DNScriptAPI::api_ui_OpenCompoundItem(pUser, nCompoundShopID);
}

int api_ui_OpenCompound2Item(UINT nUserObjectID, int nCompoundGroupID, int iItemID/*=0*/ )
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if ( !pUser ) 
		return -1;

#ifndef _FINAL_BUILD
	std::wstring wszLog;
	wszLog = FormatW( L"아이템 합성창 열기");
	pUser->SendChat(CHATTYPE_NORMAL, (int)wszLog.size()*sizeof(WCHAR), L"", (WCHAR*)wszLog.c_str());
#endif

	pUser->SendOpenCompound2Item(nCompoundGroupID, iItemID );

	pUser->SetCalledNpcResponse(true, false);	// 보통 클라이언트에 보내는 UI 개설 요청 이후 대화로 이어지지 않기 때문에 에러가 아닌데도 본의아니게 NPC 응답여부 체크에 걸리게 되므로 대화한 것으로 처리해 줌

	pUser->SetRemoteItemCompoundItemID( iItemID );

	return 1;
}

int api_ui_OpenCashShop(UINT nUserObjectID)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if ( !pUser ) 
		return -1;

	return DNScriptAPI::api_ui_OpenCashShop(pUser);
}

int api_ui_OpenGuildMgrBox(UINT nUserObjectID, int nGuildMgrNo)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if ( !pUser ) 
		return -1;

	return DNScriptAPI::api_ui_OpenGuildMgrBox(pUser, nGuildMgrNo);
}

int api_ui_OpenGacha_JP( UINT nUserObjectID, int nGachaShopID )
{
#ifdef PRE_ADD_GACHA_JAPAN
	CDNUserSession* pUser = __GetUserObj( nUserObjectID );
	if( !pUser )
		return -1;

	return DNScriptAPI::api_ui_OpenGacha_JP(pUser, nGachaShopID);
#else
	return -3;
#endif // PRE_ADD_GACHA_JAPAN
}

int api_ui_OpenGiveNpcPresent( UINT nUserObjectID, int nNpcID )
{
#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
	CDNUserSession* pUser = __GetUserObj( nUserObjectID );
	if( !pUser )
		return -1;

	return DNScriptAPI::api_ui_OpenGiveNpcPresent(pUser, nNpcID);
#else
	return -1;
#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
}

/*
desc   : 다크레어랭킹게시판 다이얼로그를 연다.
param  : 대화중인 Npc ID 
return : -1 => 유저를 찾을수 없음. 1 이면 성공.
*/
int api_ui_OpenDarkLairRankBoard( UINT nUserObjectID, int nNpcID, int iMapIndex, int iPlayerCount )
{
	CDNUserSession* pUser = __GetUserObj( nUserObjectID );
	if( !pUser )
		return -1;

	_ASSERT( iPlayerCount > 0 && iPlayerCount <= PARTYCOUNTMAX );

#ifndef _FINAL_BUILD
	std::wstring wszLog;
	wszLog = FormatW( L"다크레어랭킹게시판 UI 열기");
	pUser->SendChat(CHATTYPE_NORMAL, (int)wszLog.size()*sizeof(WCHAR), L"", (WCHAR*)wszLog.c_str());
#endif

	// 클라이언트한테 npc 에게 선물하기 UI 띄우라고 패킷 보냄.
	pUser->SendOpenDarkLairRankBoard( nNpcID, iMapIndex, iPlayerCount );

	// 보통 클라이언트에 보내는 UI 개설 요청 이후 대화로 이어지지 않기 때문에 에러가 아닌데도 본의아니게 NPC 응답여부 체크에 걸리게 되므로 대화한 것으로 처리해 줌
	// 그렇지만 npc 선물하기는 계속 대화가 이어지므로 그냥 놔둔다. 이거 호출하면 대화 다이얼로그가 바로 닫힌다.
	pUser->SetCalledNpcResponse( true, false );

	return 1;
}


/*
desc   : PvP래더랭킹게시판 다이얼로그를 연다.
param  : 대화중인 Npc ID 
return : -1 => 유저를 찾을수 없음. 1 이면 성공.
*/
int api_ui_OpenPvPLadderRankBoard( UINT nUserObjectID, int nNpcID, int iPvPLadderCode )
{
	CDNUserSession* pUser = __GetUserObj( nUserObjectID );
	if( !pUser )
		return -1;

	_ASSERT( LadderSystem::bIsServiceMatchType( static_cast<LadderSystem::MatchType::eCode>(iPvPLadderCode) ) == true );

#ifndef _FINAL_BUILD
	std::wstring wszLog;
	wszLog = FormatW( L"PvP래더랭킹게시판 UI 열기");
	pUser->SendChat(CHATTYPE_NORMAL, (int)wszLog.size()*sizeof(WCHAR), L"", (WCHAR*)wszLog.c_str());
#endif

	// 클라이언트한테 npc 에게 선물하기 UI 띄우라고 패킷 보냄.
	pUser->SendOpenPvPLadderRankBoard( nNpcID, static_cast<LadderSystem::MatchType::eCode>(iPvPLadderCode) );

	// 보통 클라이언트에 보내는 UI 개설 요청 이후 대화로 이어지지 않기 때문에 에러가 아닌데도 본의아니게 NPC 응답여부 체크에 걸리게 되므로 대화한 것으로 처리해 줌
	// 그렇지만 npc 선물하기는 계속 대화가 이어지므로 그냥 놔둔다. 이거 호출하면 대화 다이얼로그가 바로 닫힌다.
	pUser->SetCalledNpcResponse( true, false );

	return 1;
}

#if defined(PRE_ADD_OPEN_QUEST_TEXTURE_DIALOG)
int api_ui_OpenTextureDialog(UINT nUserObjectID, int nFileIndex, float fX, float fY, int nPos, int nTime, int nDialogIndex, bool bAutoCloseDialog)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if(!pUser)
	{
		return -1;
	}
	
	SCOpenTextureDialog sDlgData;
	sDlgData.fX = fX;
	sDlgData.fY = fY;
	sDlgData.nTime = nTime;
	sDlgData.nPosition = nPos;
	sDlgData.nFileIndex = nFileIndex;
	sDlgData.nDialogIndex = nDialogIndex;
	sDlgData.bAutoCloseDialog = bAutoCloseDialog;

	return DNScriptAPI::api_Open_Texture_Dialog(pUser, sDlgData);
}

int api_ui_CloseTextureDialog(UINT nUserObjectID, int nDlgID)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if(!pUser)
	{
		return -1;
	}

	return DNScriptAPI::api_Close_Texture_Dialog(pUser, nDlgID);
}
#endif

int api_user_UserMessage(UINT nUserObjectID, int nType, int nBaseStringIdx, lua_tinker::table ParamTable)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if ( !pUser )
	{
		return -1;
	}

	return DNScriptAPI::api_user_UserMessage(pUser, nType, nBaseStringIdx, ParamTable);
}

int api_quest_AddSymbolItem(UINT nUserObjectID, int nItemID, short wCount)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if ( !pUser ) 
		return -1;

	return DNScriptAPI::api_quest_AddSymbolItem(pUser, nItemID, wCount);
}

int api_quest_DelSymbolItem(UINT nUserObjectID, int nItemID, short wCount)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if ( !pUser ) 
		return -1;

	return DNScriptAPI::api_quest_DelSymbolItem(pUser, nItemID, wCount);
}

int api_quest_HasSymbolItem(UINT nUserObjectID, int nItemIndex, int nItemCnt)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if ( !pUser ) 
		return -1;

	return DNScriptAPI::api_quest_HasSymbolItem(pUser, nItemIndex, nItemCnt);
}

int api_quest_CheckQuestInvenForAddItem(UINT nUserObjectID, int nItemIndex, int nItemCnt)
{

	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if (!pUser) {
		return -1;
	}

	return DNScriptAPI::api_quest_CheckQuestInvenForAddItem(pUser, nItemIndex, nItemCnt);
}

int api_quest_CheckQuestInvenForAddItemList(UINT nUserObjectID, lua_tinker::table ItemTable)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if (!pUser) {
		return -1;
	}

	return DNScriptAPI::api_quest_CheckQuestInvenForAddItemList(pUser, ItemTable);
}

int api_quest_AddQuestItem(UINT nUserObjectID, int nItemID, short wCount, int nQuestID)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if ( !pUser ) 
		return -1;

	return DNScriptAPI::api_quest_AddQuestItem(pUser, nItemID, wCount, nQuestID);
}

int api_quest_DelQuestItem(UINT nUserObjectID, int nItemID, short wCount, int nQuestID)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if ( !pUser ) 
		return -1;

	return DNScriptAPI::api_quest_DelQuestItem(pUser, nItemID, wCount, nQuestID);
}

int api_quest_AllDelQuestItem(UINT nUserObjectID, int nItemID)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if ( !pUser ) 
		return -1;

	return DNScriptAPI::api_quest_AllDelQuestItem(pUser, nItemID);
}

int api_quest_HasQuestItem(UINT nUserObjectID, int nItemIndex, int nItemCnt)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if ( !pUser ) 
		return -1;

	return DNScriptAPI::api_quest_HasQuestItem(pUser, nItemIndex, nItemCnt);
}

int api_ui_OpenGuildCreate(UINT nUserObjectID)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if ( !pUser ) 
		return -1;

	return DNScriptAPI::api_ui_OpenGuildCreate(pUser);
}

int api_ui_OpenQuestReward(UINT nUserObjectID, int nRewardTableIndex, bool bActivate)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if ( !pUser ) 
		return -1;

	return DNScriptAPI::api_ui_OpenQuestReward(pUser, nRewardTableIndex, bActivate);
}

int api_quest_RewardQuestUser(UINT nUserObjectID, int nRewardTableIndext, int nQuestID, int nRewardCheck)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if (!pUser) {
		return -1;
	}

	return DNScriptAPI::api_quest_RewardQuestUser(pUser, nRewardTableIndext, nQuestID, nRewardCheck);
}

/*
desc   : 유저를 채널이동시킨다.
param  : 유저인덱스, 채널타입
return : -1 => 유저를 찾을수 없음, -2 없는 채널타입 -3 없는 채널
*/

int api_user_ChangeChannel(UINT nUserObjectID, int ChannelType )
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if( !pUser ) 
		return -1;

	int iAttr = 0;
	switch( ChannelType )
	{
		// PvP로비
		case 1:
		{
			iAttr = GlobalEnum::CHANNEL_ATT_PVPLOBBY;
			break;
		}
		default:
		{
			return -2;
		}
	}
	const sChannelInfo* pChannelInfo = g_pMasterConnection->GetChannelInfoFromAttr( iAttr );
	if( pChannelInfo == NULL )
		return -3;
	pUser->CmdSelectChannel( pChannelInfo->nChannelID, true );

	return 0;
}

int api_user_ChangeMap(UINT nUserObjectID, int nMapIndex, int nGateNo)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if ( !pUser ) return -1;
#if defined(_CH)
	if (pUser->GetFCMState() != FCMSTATE_NONE){
		pUser->SendQuestResult(ERROR_QUEST_FCM);
		return -1;
	}
#endif

	if (pUser->GetPartyID() > 0)
	{
		CDNParty * pParty = g_pPartyManager->GetParty(pUser->GetPartyID());
		if (pParty)
		{
			int nPermitCnt = g_pDataManager->GetMapPermitPartyCount(nMapIndex);
			if (nPermitCnt > 0 && pParty->GetMemberCount() > nPermitCnt)
				return -1;
		}
	}

	if( pUser->IsWindowState( WINDOW_BLIND ) == false )
	{
		if (!pUser->IsNoneWindowState()) 
			return -1;
	}

	int nMapType = g_pDataManager->GetMapType( nMapIndex );
	switch( nMapType ) {
		case GlobalEnum::MAP_UNKNOWN:
			return -2;
		case GlobalEnum::MAP_VILLAGE:
			if( pUser->GetPartyID() > 0 ) {
				CDNParty *pParty = g_pPartyManager->GetParty(pUser->GetPartyID());
				if (!pParty) return -1;
				if (g_pMasterConnection && g_pMasterConnection->GetActive())
					g_pMasterConnection->SendVillageToVillage(REQINFO_TYPE_PARTY, pParty->GetPartyID(), nMapIndex, -1, -1, pParty);	//pUser->GetLevel()
				else
					return -1;
			}
			else
			{
				if (g_pMasterConnection && g_pMasterConnection->GetActive())
					g_pMasterConnection->SendVillageToVillage(REQINFO_TYPE_SINGLE, pUser->GetAccountDBID(), nMapIndex, -1, -1);
				else
					return -1;
			}
			break;
		case GlobalEnum::MAP_WORLDMAP:
		case GlobalEnum::MAP_DUNGEON:
			{
				// 파티일 경우 과연 스크립트에서 강제 이동시에 다른놈들이 딸려가되 되겠느냐.. 의 문제가 있습니다.
				// 일단은 이동되게 해놓겟지만 분명 꼬이는 경우가 생길듯 하오니 추후 논의 후 처리하지말입니다.
				if( pUser->GetPartyID() > 0 ) {
					CDNParty *pParty = g_pPartyManager->GetParty(pUser->GetPartyID());
					if (!pParty) return -1;
					pParty->SetRandomSeed( timeGetTime() );
					if (g_pMasterConnection && g_pMasterConnection->GetActive())
					{
						g_pMasterConnection->SendReqGameID( GameTaskType::Normal, REQINFO_TYPE_PARTY, pParty->GetPartyID(), pUser->GetChannelID(), pParty->GetRandomSeed(), nMapIndex, nGateNo, CONVERT_TO_DUNGEONDIFFICULTY(0), true, pParty );		// GameID를 요청
					}
					else
						return -1;
				}
				else {
					pUser->m_cGateNo = nGateNo;
					pUser->m_nSingleRandomSeed = timeGetTime();
					if (g_pMasterConnection && g_pMasterConnection->GetActive())
						g_pMasterConnection->SendReqGameID( GameTaskType::Normal, REQINFO_TYPE_SINGLE, pUser->GetAccountDBID(), pUser->GetChannelID(), pUser->m_nSingleRandomSeed, nMapIndex, 1, CONVERT_TO_DUNGEONDIFFICULTY(0), true );	// GameID를 요청
					else
						return -1;
				}
			}
			break;
	}
	return 0;
}

/*
desc   : 해당 유저의 파티원수를 리턴한다.
param  : 유저인덱스
return : -1 => 유저를 찾을수 없음, 그외는 파티원수 (주의 !!!> 파티에 가입되지 않은 사용자도 1 을 반환)
*/
int api_user_GetPartymemberCount(UINT nUserObjectID)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if ( !pUser ) 
		return -1;
	if (pUser->GetPartyID() > 0)
	{
		//파티라면 멤버카운트
		CDNParty* pParty = g_pPartyManager->GetParty(pUser->GetPartyID());
		if (!pParty) return -1;

		return pParty->GetMemberCount();
	}
	
	// 파티가 없으면 혼자라는 뜻이므로 1
	return 1;
}

/*
desc   : 해당 유저가 파티에 가입된 상태인지 체크한다.
param  : 유저인덱스
return : -1 => 유저를 찾을수 없음, 0 => 파티에 가입 않되어있음, 1 => 파티에 가입 되어있음
*/
int api_user_IsPartymember(UINT nUserObjectID)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if ( !pUser ) return -1;
	if (pUser->GetPartyID() > 0)
	{
		//파티라면 멤버카운트
		CDNParty* pParty = g_pPartyManager->GetParty(pUser->GetPartyID());
		if (!pParty) return -1;

		return 1;
	}

	// 파티에 가입하지 않은 상태이면 0 을 반환
	return 0;
}

//blondy
/*
desc   : 해당유저를 PVP로비오 안내.
param  : 유저인덱스
return : 1 성공 -1 => 유저를 찾을수 없음
*/
int api_user_RequestEnterPVP( UINT nUserObjectID )
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if ( !pUser ) return -1;

	pUser->SendPvPVillageToLobby();
	return 1;

}
//blondy end

/*
desc   : 해당 유저의 스테이지 난이도를 구함 (P.S.> VillageServer 에서는 항상 실패함)
param  : 유저인덱스
return : 0 ~ 4 => 던전 난이도 (0:쉬움 / 1:보통 / 2:어려움 / 3:마스터 : 4:어비스), 그 외는 실패 (기본 -1)
*/
int api_user_GetStageConstructionLevel(UINT nUserObjectID)
{
//	DN_ASSERT(0,	"Can't be Called!");

	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if (!pUser) {
		return -1;
	}

	return -1;
}

int api_user_GetMapIndex(UINT nUserObjectID)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if (!pUser) {
		return -1;
	}

	return DNScriptAPI::api_user_GetMapIndex(pUser);
}

int api_user_GetLastStageClearRank(UINT nUserObjectID)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if (!pUser) {
		return -1;
	}

	return DNScriptAPI::api_user_GetLastStageClearRank(pUser);
}

int api_user_EnoughCoin(UINT nUserObjectID, int nCoin )
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if (!pUser) {
		return -1;
	}

	return DNScriptAPI::api_user_EnoughCoin(pUser, nCoin);
}

/*
desc   : 유저의 현재 소지금을 확인한다.
param  : 유저인덱스
return : -1 => 유저를 찾을수 없음, 0 이상 => 현재 소지금
*/
int api_user_GetCoin(UINT nUserObjectID)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if (!pUser) {
		return -1;
	}

#if defined(_CH)
	if (pUser->GetFCMState() != FCMSTATE_NONE){
		pUser->SendQuestResult(ERROR_QUEST_FCM);
		return -1;
	}
#endif

	return (int)(pUser->GetCoin());
}

/*
desc   : 유저에게 돈을 감소한다.
param  : 유저인덱스, 돈 (0 초과)
return : -1 => 유저를 찾을수 없음, 1 => 성공
*/
int api_user_DelCoin(UINT nUserObjectID, int nDelCoin)
{
	DN_ASSERT(0 < nDelCoin,	"Invalid!");

	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if (!pUser) {
		return -1;
	}

#if defined(_CH)
	if (pUser->GetFCMState() != FCMSTATE_NONE){
		pUser->SendQuestResult(ERROR_QUEST_FCM);
		return -1;
	}
#endif

	pUser->DelCoin(nDelCoin, DBDNWorldDef::CoinChangeCode::Use, 0);

	INT64 nCoin = pUser->GetCoin();

#ifndef _FINAL_BUILD
	std::wstring wszMsg;
	wszMsg = FormatW( L"코인감소 : %d 최종 : %lld" , nDelCoin, nCoin);
	pUser->SendChat(CHATTYPE_NORMAL, (int)wszMsg.size()*sizeof(WCHAR), L"", (WCHAR*)wszMsg.c_str());
#endif

	return 1;
}

int api_user_IsMissionGained(UINT nUserObjectID, int nMissionIndex)
{
	DN_ASSERT(CHECK_LIMIT(nMissionIndex, MISSIONMAX),	"Invalid!");

	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if (!pUser) {
		return -1;
	}

	return DNScriptAPI::api_user_IsMissionGained(pUser, nMissionIndex);
}

int api_user_IsMissionAchieved(UINT nUserObjectID, int nMissionIndex)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if (!pUser) {
		return -1;
	}

	return DNScriptAPI::api_user_IsMissionAchieved(pUser, nMissionIndex);
}

int api_user_HasItemWarehouse(UINT nUserObjectID, int nItemIndex, int nItemCnt)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if (!pUser) {
		return -1;
	}

	return DNScriptAPI::api_user_HasItemWarehouse(pUser, nItemIndex, nItemCnt);
}

int api_user_HasItemEquip(UINT nUserObjectID, int nItemIndex)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if (!pUser) {
		return -1;
	}

	return DNScriptAPI::api_user_HasItemEquip(pUser, nItemIndex);
}

int api_guild_GetGuildMemberRole(UINT nUserObjectID)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if (!pUser) {
		return -1;
	}

	return DNScriptAPI::api_guild_GetGuildMemberRole(pUser);
}

/*
desc   : 유저의 현재 길드의 창고소유 여부를 알려준다.
param  : 유저인덱스
return : -1 : 유저를 찾을수 없음 / 0 : 길드창고 없음 / 1 : 길드창고 있음
*/
int api_guild_HasGuildWare(UINT nUserObjectID)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if (!pUser) {
		return -1;
	}

	const TGuildUID GuildUID = pUser->GetGuildUID ();
	if (!GuildUID.IsSet())
		return -1;

	CDNGuildBase* pGuild = g_pGuildManager->At (GuildUID);
	if (!pGuild)
		return 0;


#if !defined( PRE_ADD_NODELETEGUILD )
	// 길드가존재한다.
	CDetachAutoEx<CDNGuildBase>	AutoDetach (pGuild);
	if (FALSE == pGuild->IsEnable()) return 0;
#endif

	if (pGuild->GetWareSize() > 0)
		return 1;

	return 0;
}

int api_quest_IncQuestMemo(UINT nUserObjectID, int nQuestID, char nMemoIndex)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if ( !pUser ) {
		return LONG_MIN;
	}

	return DNScriptAPI::api_quest_IncQuestMemo(pUser, nQuestID, nMemoIndex);
}

int api_quest_DecQuestMemo(UINT nUserObjectID, int nQuestID, char nMemoIndex)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if ( !pUser ) {
		return LONG_MAX;
	}

	return DNScriptAPI::api_quest_DecQuestMemo(pUser, nQuestID, nMemoIndex);
}

int api_user_SetUserJobID(UINT nUserObjectID, int nJobID)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if ( !pUser ) {
		return -1;
	}

	return DNScriptAPI::api_user_SetUserJobID(pUser, nJobID);
}

int api_user_IsJobInSameLine(UINT nUserObjectID, int nBaseJobID)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if ( !pUser ) {
		return -1;
	}

	return DNScriptAPI::api_user_IsJobInSameLine(pUser, nBaseJobID);
}

int api_user_HasCashItem(UINT nUserObjectID, int nItemIndex, int nItemCnt)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if ( !pUser ) 
		return -1;

	return DNScriptAPI::api_user_HasCashItem(pUser, nItemIndex, nItemCnt);
}

int api_user_HasCashItemEquip(UINT nUserObjectID, int nItemIndex)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if (!pUser) {
		return -1;
	}

	return DNScriptAPI::api_user_HasCashItemEquip(pUser, nItemIndex);
}

int api_quest_IncCounting(UINT nUserObjectID, int nCountingType, int nCountingIndex)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if (!pUser) {
		return -1;
	}

	return DNScriptAPI::api_quest_IncCounting(pUser, nCountingType, nCountingIndex);
}

int api_quest_IsPlayingQuestMaximum(UINT nUserObjectID)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if (!pUser) {
		return -1;
	}

	return DNScriptAPI::api_quest_IsPlayingQuestMaximum(pUser);
}

int api_quest_ForceCompleteQuest(UINT nUserObjectID, int nQuestID, int nQuestCode, int bDoMark, int bDoDelete, int bDoRepeat)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if (!pUser) {
		return -1;
	}

	return DNScriptAPI::api_quest_ForceCompleteQuest(pUser, nQuestID, nQuestCode, bDoMark, bDoDelete, bDoRepeat);
}

int api_quest_ResetSkill(UINT nUserObjectID, int nQuestID)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if (!pUser) {
		return -1;
	}

	CDNUserQuest* pQuest = pUser->GetQuest();
	if (!pQuest) {
		return -2;
	}

	bool bResult = pQuest->IsClearQuest(nQuestID);

	if ( bResult )
		return -3;

#if defined(_VILLAGESERVER)
	pUser->GetSkill()->ResetSkill( pUser->GetSkillPage() );
#elif defined(_GAMESERVER)
	return -1;
#endif // #if defined(_VILLAGESERVER)

	return 1;
}

int api_quest_GetWorldEventStep(UINT nUserObjectID, int nScheduleID)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if ( !pUser ) 
		return -1;

	if (!g_pPeriodQuestSystem)
		return -1;

	return g_pPeriodQuestSystem->GetWorldEventStep(pUser->GetWorldSetID(), nScheduleID);
}

int api_quest_GetWorldEventCount (UINT nUserObjectID, int nScheduleID)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if ( !pUser ) 
		return -1;

	if (!g_pPeriodQuestSystem)
		return -1;

	return g_pPeriodQuestSystem->GetWorldEventCount(pUser->GetWorldSetID(), nScheduleID);
}

int api_quest_ReturnItemToNpc (UINT nUserObjectID, int nScheduleID)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if ( !pUser ) 
		return -1;

	int nCount = pUser->GetItem()->ReturnItemToNpc(nScheduleID);
	if (nCount <= 0)
		return -1;

	return nCount;
}

void api_quest_OpenScoreWorldEvent (UINT nUserObjectID, int nScheduleID)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if ( !pUser ) 
		return;

	TGlobalEventData* pEvent = g_pDataManager->GetGlobalEventData(nScheduleID);
	if (!pEvent) 
		return;

	int nMaxCount = pEvent->nCollectTotalCount;
	int nCurrentCount = g_pPeriodQuestSystem->GetWorldEventCount(pUser->GetWorldSetID(), nScheduleID);
	
	if (nCurrentCount >= nMaxCount)
		nCurrentCount = nMaxCount;
	
	pUser->SendScorePeriodQuest(nCurrentCount, nMaxCount);
}

/*
desc   : 특정 사용자에게 농장구역 리스트를 보여준다
param  : 유저인덱스
return : -1 : 유저를 찾을수 없음, 1 성공
*/
int api_quest_OpenFarmAreaList (UINT nUserObjectID)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if ( !pUser ) 
		return -1;

	pUser->SendFarmOpenList();

	pUser->ReqFarmInfo(false);

	return 1;
}

/*
desc   : 특정 사용자의 위치를 이동 시킨다.
param  : 유저인덱스, X 좌표, Y 좌표, Z 좌표
return : -1 : 유저를 찾을수 없음, 1 성공
*/
int api_user_ChangePos(UINT nUserObjectID, int nX, int nY, int nZ, int nLookX, int nLookY )
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if (!pUser) {
		return -1;
	}
#if defined(_CH)
	if (pUser->GetFCMState() != FCMSTATE_NONE){
		pUser->SendQuestResult(ERROR_QUEST_FCM);
		return -1;
	}
#endif

	pUser->ChangePos(nX, nY, nZ, float(nLookX), float(nLookY));

	return 1;
}


int api_npc_GetFavorPoint( UINT uiUserObjectID, int iNpcID )
{
#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
	CDNUserSession* pUser = __GetUserObj(uiUserObjectID);
	if (!pUser)
		return -1;

	return DNScriptAPI::api_npc_GetFavorPoint(pUser, iNpcID);
#else
	return -1;
#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
}

int api_npc_GetMalicePoint( UINT uiUserObjectID, int iNpcID )
{
#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
	CDNUserSession* pUser = __GetUserObj(uiUserObjectID);
	if( !pUser )
		return -1;

	return DNScriptAPI::api_npc_GetMalicePoint(pUser, iNpcID);
#else
	return -1;
#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
}

int api_npc_GetFavorPercent( UINT uiUserObjectID, int iNpcID )
{
#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
	CDNUserSession* pUser = __GetUserObj(uiUserObjectID);
	if( !pUser )
		return -1;

	return DNScriptAPI::api_npc_GetFavorPercent(pUser, iNpcID);
#else
	return -1;
#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
}

int api_npc_GetMalicePercent( UINT uiUserObjectID, int iNpcID )
{
#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
	CDNUserSession* pUser = __GetUserObj(uiUserObjectID);
	if( !pUser )
		return -1;

	return DNScriptAPI::api_npc_GetMalicePercent(pUser, iNpcID);
#else
	return -1;
#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
}

int api_npc_AddFavorPoint( UINT uiUserObjectID, int iNpcID, int val )
{
#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
	CDNUserSession* pUser = __GetUserObj(uiUserObjectID);
	if( !pUser )
		return -1;

	return DNScriptAPI::api_npc_AddFavorPoint(pUser, iNpcID, val);
#else
	return -1;
#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
}

int api_npc_AddMalicePoint( UINT uiUserObjectID, int iNpcID, int val )
{
#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
	CDNUserSession* pUser = __GetUserObj(uiUserObjectID);
	if( !pUser )
		return -1;

	return DNScriptAPI::api_npc_AddMalicePoint(pUser, iNpcID, val);
#else
	return -1;
#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
}

int api_npc_SendSelectedPresent( UINT uiUserObjectID, int iNpcID )
{
#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
	CDNUserSession* pUser = __GetUserObj(uiUserObjectID);
	if( !pUser )
		return -1;

	return DNScriptAPI::api_npc_SendSelectedPresent(pUser, iNpcID);
#else
	return -1;
#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
}

int api_npc_Rage( UINT uiUserObjectID, int iNpcID )
{
#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
	CDNUserSession* pUser = __GetUserObj(uiUserObjectID);
	if( !pUser )
		return -1;

	return DNScriptAPI::api_npc_Rage(pUser, iNpcID);
#else
	return -1;
#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
}

int api_npc_Disappoint( UINT uiUserObjectID, int iNpcID )
{
#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
	CDNUserSession* pUser = __GetUserObj(uiUserObjectID);
	if( !pUser )
		return -1;

	return DNScriptAPI::api_npc_Disappoint(pUser, iNpcID);
#else
	return -1;
#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
}

int api_user_ResetSkill(UINT nUserObjectID)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if (!pUser) {
		return -1;
	}

	pUser->GetItem()->ResetSkillFromQuestScript();

	return 1;
}

int api_npc_CreateSecondarySkill( UINT uiUserObjectID, int iSkillID )
{
#if defined( PRE_ADD_SECONDARY_SKILL )
	CDNUserSession* pUser = __GetUserObj(uiUserObjectID);
	if (!pUser) 
		return -1;

	CSecondarySkillRepository* pRepository = pUser->GetSecondarySkillRepository();
	if( pRepository == NULL )
		return 0;

	if( pRepository->Create( iSkillID ) == false )
		return 0;

	return 1;
#else
	return -1;
#endif // #if defined( PRE_ADD_SECONDARY_SKILL )
}

int api_npc_CheckSecondarySkill( UINT uiUserObjectID, int iSkillID )
{
#if defined( PRE_ADD_SECONDARY_SKILL )
	CDNUserSession* pUser = __GetUserObj(uiUserObjectID);
	if (!pUser) 
		return -1;

	CSecondarySkillRepository* pRepository = pUser->GetSecondarySkillRepository();
	if( pRepository == NULL )
		return 0;

	if( pRepository->Get( iSkillID ) == NULL )
		return 0;

	return 1;
#else
	return -1;
#endif // #if defined( PRE_ADD_SECONDARY_SKILL )
}

/*
desc	: 유저에게 길드창고를 열라고 알린다.
param	: 유저인덱스
return  : -1 => 유저를 찾을 수 없음, -2=>길드원이 아님, 1=>성공
*/
int api_ui_OpenGuildWareHouse (UINT nUserObjectID)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);

	if ( !pUser ) 
		return -1;

	if (g_pGuildManager->OnRecvApiOpenGuildWare (pUser))
		pUser->SetCalledNpcResponse(true, false);	
	else
		return -2;

	return 1;
}


int api_ui_OpenMovieBrowser(UINT nUserObjectID)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if ( !pUser ) 
		return -1;

#ifndef _FINAL_BUILD
	std::wstring wszLog;
	wszLog = FormatW( L"무비클립 브라우저 열기");
	pUser->SendChat(CHATTYPE_NORMAL, (int)wszLog.size()*sizeof(WCHAR), L"", (WCHAR*)wszLog.c_str());
#endif

	pUser->SendOpenMovieBrowser();

	pUser->SetCalledNpcResponse(true, false);	// 보통 클라이언트에 보내는 UI 개설 요청 이후 대화로 이어지지 않기 때문에 에러가 아닌데도 본의아니게 NPC 응답여부 체크에 걸리게 되므로 대화한 것으로 처리해 줌

	return 1;
}

int api_ui_OpenBrowser(UINT nUserObjectID, const char* szUrl, int nX, int nY, int nWidth, int nHeight)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if ( !pUser ) 
		return -1;

#ifndef _FINAL_BUILD
	std::wstring wszLog;
	wszLog = FormatW( L"npc 브라우저 열기 (%s)", szUrl);
	pUser->SendChat(CHATTYPE_NORMAL, (int)wszLog.size()*sizeof(WCHAR), L"", (WCHAR*)wszLog.c_str());
#endif

	pUser->SendOpenBrowser(szUrl, nX, nY, nWidth, nHeight);

	pUser->SetCalledNpcResponse(true, false);	// 보통 클라이언트에 보내는 UI 개설 요청 이후 대화로 이어지지 않기 때문에 에러가 아닌데도 본의아니게 NPC 응답여부 체크에 걸리게 되므로 대화한 것으로 처리해 줌

	return 1;
}

/*
desc   : 길드전 신청 여부를 알려준다. (신청기간만 허용)
param  : 유저인덱스
return : -1 : 유저를 찾을수 없음 / 0 : 길드전 신청불가능 / 1 : 길드전 신청가능
*/
int api_guildwar_IsPreparation(UINT nUserObjectID)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if ( !pUser ) 
		return -1;

	if (g_pGuildWarManager)
	{
		if (g_pGuildWarManager->GetStepIndex() == GUILDWAR_STEP_PREPARATION && pUser->GetGuildSelfView().IsSet())
		{
			if (pUser->GetGuildSelfView().btGuildRole == GUILDROLE_TYPE_MASTER && 
				pUser->GetGuildSelfView().wWarSchduleID != g_pGuildWarManager->GetScheduleID())
				return 1;
		}
	}
	return 0;
}
/*
desc   : 길드전 기간 체크.
param  : 유저인덱스
return : -1 : 유저를 찾을수 없음 / eGuildWarStepType / 0 : 길드전 기간 아님 / 1 : 길드전 신청 기간 / 2 : 길드전 예선 기간 / 3 : 본선 및 보상기간
*/
int api_guildwar_GetStep(UINT nUserObjectID)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if ( !pUser ) 
		return -1;

	if (g_pGuildWarManager)
		return g_pGuildWarManager->GetStepIndex();	
	return 0;
}
/*
desc   : 길드전 예선 진행 현황 보기
param  : 유저인덱스
return : return : -1 => 유저를 찾을수 없음, -2 => 예선전 기간이 아님, 0 : 성공
*/
int api_guildwar_GetTrialCurrentState(UINT nUserObjectID)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if ( !pUser ) 
		return -1;

	if( g_pGuildWarManager->GetStepIndex() != GUILDWAR_STEP_TRIAL)
		return -2;	

	int nGuildWarPoint = 0;
	CDNGuildBase* pGuild = g_pGuildManager->At(pUser->GetGuildUID());

	if (pGuild)
	{
#if !defined( PRE_ADD_NODELETEGUILD )
		CDetachAutoEx<CDNGuildBase>	AutoDetach (pGuild);
		if (FALSE == pGuild->IsEnable()) return -1;
#endif

		if( pGuild->IsEnrollGuildWar() )
		{
			CDNGuildVillage* pGuildVillage = static_cast<CDNGuildVillage *>(pGuild);
			if( pGuildVillage->GetGuildWarScheduleID() != g_pGuildWarManager->GetScheduleID() )
			{
				pGuildVillage->GuildWarReset();
				pGuildVillage->SetGuildWarScheduleID(g_pGuildWarManager->GetScheduleID());
			}
			nGuildWarPoint = pGuildVillage->GetGuildWarPoint();
		}
	}
	// 열어라를 먼저 보낸다..(예선 결과창하고 UI를 같이써서 이걸로 보내야함)
	pUser->SetCalledNpcResponse(true, false);	// 보통 클라이언트에 보내는 UI 개설 요청 이후 대화로 이어지지 않기 때문에 에러가 아닌데도 본의아니게 NPC 응답여부 체크에 걸리게 되므로 대화한 것으로 처리해 줌
	pUser->SendGuildWarTrialResultUIOpen();
	pUser->SendGuildWarStatusOpen( g_pGuildWarManager->GetBlueTeamPoint(), g_pGuildWarManager->GetRedTeamPoint(), pUser->GetGuildWarPoint(), nGuildWarPoint);
	return 0;
}
/*
desc   : 길드전 예선 결과
param  : 유저인덱스
return : return : -1 => 유저를 찾을수 없음, -2 => 본선 및 보상기간이 아님, -3 => 아직 예선결과 집계가 완료되지 않음, 0 : 성공
*/
int api_guildWar_GetTrialStats(UINT nUserObjectID)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if ( !pUser ) 
		return -1;

	if( g_pGuildWarManager->GetStepIndex() != GUILDWAR_STEP_REWARD)
		return -2;
	if( !g_pGuildWarManager->GetGuildWarStats() )
		return -3;
	// 여기서 부터 문제..길드전에 참여하지 않은 넘은 바로 보내고..참여한 넘은 길드, 내정보 검색해야함.

	pUser->SetCalledNpcResponse(true, false);	// 보통 클라이언트에 보내는 UI 개설 요청 이후 대화로 이어지지 않기 때문에 에러가 아닌데도 본의아니게 NPC 응답여부 체크에 걸리게 되므로 대화한 것으로 처리해 줌
	pUser->SendGuildWarTrialResultUIOpen();

	CDNGuildBase* pGuild = g_pGuildManager->At(pUser->GetGuildUID());
	if (pGuild)
	{
#if !defined( PRE_ADD_NODELETEGUILD )
		CDetachAutoEx<CDNGuildBase>	AutoDetach (pGuild);
		if (FALSE == pGuild->IsEnable()) return -1;
#endif

		if( pGuild->IsEnrollGuildWar() )
		{
			CDNGuildVillage* pGuildVillage = static_cast<CDNGuildVillage *>(pGuild);
			if (!pGuildVillage->GetGuildWarStats() )
			{
				// 내 길드 집계정보 가져오기				
				CDNDBConnection* pDBCon = pGuild->GetDBCon();
				if (pDBCon)				
					pDBCon->QueryGetGuildWarPointGuildPartTotal(pGuild->GetDBThreadID(), pUser->GetWorldSetID(), pUser->GetAccountDBID(), 'G', pUser->GetGuildUID().nDBID);				
				return 0;
			}
			else if( !pUser->GetGuildWarStats() )
			{
				CDNDBConnection* pDBCon = pUser->GetDBConnection();
				if( pDBCon )				
					pDBCon->QueryGetGuildWarPointPartTotal(pUser->GetDBThreadID(), pUser->GetWorldSetID(), pUser->GetAccountDBID(), 'C', pUser->GetCharacterDBID());
				return 0;
			}			
		}
	}
	// 길드에도 가입되어 있지 않고 길드전에도 참여하지 않았으면 바로 보내자.
	pUser->SendGuildWarTrialResultOpen();	
	return 0;
}
/*
desc   : 길드전 결승 대진표 보기
param  : 유저인덱스
return : return : -1 => 유저를 찾을수 없음, -2 => 본선 및 보상기간이 아님, -3 => 대진표가 셋팅되지 않았음, 0: 성공
*/
int api_guildWar_GetMatchList(UINT nUserObjectID)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if ( !pUser ) 
		return -1;

	if( g_pGuildWarManager->GetStepIndex() != GUILDWAR_STEP_REWARD)
		return -2;
	if( !g_pGuildWarManager->GetGuildWarStats() )
		return -3;

	pUser->SetCalledNpcResponse(true, false);	// 보통 클라이언트에 보내는 UI 개설 요청 이후 대화로 이어지지 않기 때문에 에러가 아닌데도 본의아니게 NPC 응답여부 체크에 걸리게 되므로 대화한 것으로 처리해 줌
	pUser->SendGuildWarTournamentInfoUIOpen();

	bool bPopularityVote = false;
	CDNGuildBase* pGuild = g_pGuildManager->At(pUser->GetGuildUID());
	if (pGuild)
	{
#if !defined( PRE_ADD_NODELETEGUILD )
		CDetachAutoEx<CDNGuildBase>	AutoDetach (pGuild);
		if (FALSE == pGuild->IsEnable()) return -1;
#endif

		// 길드전에 참여했고 결승 진출팀이 아니고 내가 예선에 참가한 캐릭터 이면.
		if( pGuild->IsEnrollGuildWar() && !g_pGuildWarManager->IsGuildWarFinal(pUser->GetGuildUID()) && pUser->GetGuildWarPoint() )
		{
			// 투표한적 없고 DB에서 꺼내온적 없으면
			if( !pUser->GetGuildWarVote() && !pUser->GetGuildWarVoteLoading() )
			{					
				CDNDBConnection* pDBCon = pUser->GetDBConnection();
				if (pDBCon)			
					pDBCon->QueryGetGuildWarPopularityVote(pUser->GetDBThreadID(), pUser->GetWorldSetID(), pUser->GetAccountDBID(), 'C', pUser->GetCharacterDBID());								
				return 0;
			}
			// DB에서 꺼내온게 있으면..
			else if( pUser->GetGuildWarVoteLoading() )
			{
				bPopularityVote = !pUser->GetGuildWarVote();
			}
		}
	}
	pUser->SendGuildWarTournamentInfoOpen(bPopularityVote);	
	return 0;
}
/*
desc   : 길드전 인기 투표 결과
param  : 유저인덱스
return : return : -1 => 유저를 찾을수 없음, -2 => 지난 인기 투표 결과 없음, -3 => 길드전이 진행중임, 0: 성공
*/
int api_guildWar_GetPopularityVote(UINT nUserObjectID)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if ( !pUser ) 
		return -1;

	if( g_pGuildWarManager->GetStepIndex() == GUILDWAR_STEP_REWARD)
		return -3;

	if( g_pGuildWarManager->GetGuildWarVote() && g_pGuildWarManager->GetGuildWarVoteCount() == 0)
		return -2;

	// DB에서 꺼내온적이 없으면
	if( !g_pGuildWarManager->GetGuildWarVote() )
	{	
		CDNDBConnection* pDBCon = pUser->GetDBConnection();
		if (pDBCon)			
			pDBCon->QueryGetGuildWarPopularityVote(pUser->GetDBThreadID(), pUser->GetWorldSetID(), pUser->GetAccountDBID(), 'G', 0);					
		return 0;		
	}
	pUser->SendGuildWarVoteTop(g_pGuildWarManager->GetGuildWarVoteCount(), g_pGuildWarManager->GetGuildWarVoteGuildName());
	return 0;	
}

/*
desc   : 길드전 본선 유무 체크.
param  : 유저인덱스
return : -1 : 유저를 찾을수 없음 / 0 : 본선 진행 안함 / 1 : 본선 진행함
*/
int api_guildWar_GetFinalProgress(UINT nUserObjectID)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if ( !pUser ) 
		return -1;

	if (g_pGuildWarManager)
		return (int)g_pGuildWarManager->GetFinalProgress();	
	return 0;
}

#if defined (PRE_ADD_DONATION)
int api_ui_OpenDonation(UINT nUserObjectID)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);

	return DNScriptAPI::api_ui_OpenDonation(pUser);
}
#endif // #if defined (PRE_ADD_DONATION)

int api_ui_OpenInventory(UINT nUserObjectID)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);

	return DNScriptAPI::api_ui_OpenInventory(pUser);
}

/*
desc	: 유저의 PC Cafe등급
param	:
return	: -1 => 유저를 찾을수 없음, 0 => PC방이 아님. 나머지 PC방 등급.
*/
int api_user_GetPCCafe(UINT nUserObjectID)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if( !pUser)
		return -1;	
	return pUser->GetPCBangGrade();
}

/*
desc	: PC방 유저에게 무기, 탈것과 같은 대여아이템을 만들어서 넣어준다.
param	:
return	: -1 => 유저를 찾을수 없음, 0 => 성공, 1 => PC방 유저가 아님, 2 => 지급할 아이템이 없음, 3 => 인벤이 모자름, 4 => 이미 대여아이템을 가지고 있음, 5 => 아이템 에러.
*/
int api_user_SetPCCafeItem(UINT nUserObjectID)
{
#if defined( PRE_ADD_PCBANG_RENTAL_ITEM )
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if( !pUser )
		return -1;
	if( pUser->GetPCBangGrade() == 0)
		return 1;

	// 여기서 이미 대여 아이템을 가지고 있는지 체크
	if( pUser->GetItem()->IsHavePcCafeRentItem())
		return 4;

	std::map<int,int> mapItemIDList;
	int nRequireInven = g_pDataManager->GetPcCafeRentItemID(pUser->GetLevel(), pUser->GetClassID(), pUser->GetUserJob(), pUser->GetPCBangGrade(),mapItemIDList);
	if( mapItemIDList.empty())
		return 2;

	// 인벤 체크..
	if( nRequireInven > pUser->GetItem()->FindBlankInventorySlotCount() ) 
	{	
		pUser->SendPcCafeRentItem(ERROR_ITEM_INVENTORY_NOTENOUGH, nRequireInven);
		return 3;
	}

	std::map<int,int>::iterator itor;
	for( itor=mapItemIDList.begin(); itor !=mapItemIDList.end(); ++itor )
	{
		if( !pUser->GetItem()->CreatePCRentalItem(itor->first, itor->second) )
			return 5;
	}
	pUser->SendPcCafeRentItem(ERROR_NONE, 0);	
	return 0;
#else
	return -1;
#endif		
}

/*
desc   : 유저가 가진 장비 아이템을 수리하고, 수리비용을 차감합니다.
param  : 유저, 전체수리 유무(1-전체수리, 0-장착한아이템)
return : -1 : 유저를 찾을수 없음 / 0 : 수리할 아이템 없음 / 1 : 소지금 부족 / 2 : 아이템 수리 완료
*/
int api_user_RepairItem(UINT nUserObjectID, int iAllRepair)
{
#if defined(PRE_ADD_REPAIR_NPC)
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if ( !pUser ) return -1;

	return DNScriptAPI::api_user_RepairItem(pUser, iAllRepair);
#else
	return -1;
#endif
}


int api_ui_OpenExchangeEnchant(UINT nUserObjectID)
{
#ifdef PRE_ADD_EXCHANGE_ENCHANT
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if ( !pUser ) 
		return -1;

	return DNScriptAPI::api_ui_OpenExchangeEnchant(pUser);
#else
	return -1;
#endif
}

int api_env_CheckCloseGateByTime(int iMapIndex)
{
#if defined(PRE_ADD_CHALLENGE_DARKLAIR)
	return DNScriptAPI::api_env_CheckCloseGateByTime(iMapIndex);
#else
	return 0;
#endif
}

int api_ui_OpenCostumeRandomMix(UINT nUserObjectID)
{
#ifdef PRE_ADD_COSRANDMIX_ACCESSORY
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if ( !pUser ) 
		return -1;

#ifndef _FINAL_BUILD
	std::wstring wszLog;
	wszLog = FormatW( L"npc 랜덤합성 열기");
	pUser->SendChat(CHATTYPE_NORMAL, (int)wszLog.size()*sizeof(WCHAR), L"", (WCHAR*)wszLog.c_str());
#endif

	pUser->StartCostumeRandomMix(CostumeMix::RandomMix::OpenByNpc, 0, 0);
	pUser->SetCalledNpcResponse(true, false);	// 보통 클라이언트에 보내는 UI 개설 요청 이후 대화로 이어지지 않기 때문에 에러가 아닌데도 본의아니게 NPC 응답여부 체크에 걸리게 되므로 대화한 것으로 처리해 줌

	return 1;
#else
	return -1;
#endif
}

int api_quest_WorldBuffCheck(int nItemID)
{
#if defined(PRE_DRAGONBUFF)
	return DNScriptAPI::api_quest_WorldBuffCheck(nItemID);
#else
	return -1;
#endif
}

int api_quest_ApplyWorldBuff(UINT nUserObjectID, int nItemID, int nMapIdx)
{
#if defined(PRE_DRAGONBUFF)
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if ( !pUser ) 
		return -1;
	return DNScriptAPI::api_quest_ApplyWorldBuff(pUser, nItemID, nMapIdx);
#else
	return -1;
#endif
}

#ifdef PRE_ADD_DWC
void api_ui_OpenCreateDwcTeamDlg(UINT nUserObjectID)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if ( !pUser ) 
		return;

#ifndef _FINAL_BUILD
	std::wstring wszLog;
	wszLog = FormatW( L"DWC 팀 생성 대화상자 열기");
	pUser->SendChat(CHATTYPE_NORMAL, (int)wszLog.size()*sizeof(WCHAR), L"", (WCHAR*)wszLog.c_str());
#endif

	pUser->SendOpenDwcTeamDialog();
	pUser->SetCalledNpcResponse(true, false);	// 보통 클라이언트에 보내는 UI 개설 요청 이후 대화로 이어지지 않기 때문에 에러가 아닌데도 본의아니게 NPC 응답여부 체크에 걸리게 되므로 대화한 것으로 처리해 줌
}
#endif // PRE_ADD_DWC

int api_ui_OpenJobChange(UINT nUserObjectID)
{
	CDNUserSession* pUser = __GetUserObj(nUserObjectID);
	if (!pUser)
		return -1;

	return DNScriptAPI::api_Open_ChangeJobDialog(pUser); 
}