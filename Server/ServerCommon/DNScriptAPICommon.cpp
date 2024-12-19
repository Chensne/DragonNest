#include "stdafx.h"
#include "DNScriptAPICommon.h"
#include "DnLuaAPIDefine.h"
#include "DNGameDataManager.h"
#include "DNQuestManager.h"
#include "DNUserSession.h"
#include "DNNpc.h"
#include "DNReplaceString.h"
#include "EtUIXML.h"
#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
#include "ReputationSystemRepository.h"
#include "NpcReputationProcessor.h"
#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )

#include "DNQuest.h"

#if defined (_GAMESERVER)
#include "DnPlayerActor.h"
#endif

#if defined(_VILLAGESERVER) && defined( PRE_DRAGONBUFF )
#include "DNUserSessionManager.h"
#include "DNMasterConnection.h"
#endif

extern CLog g_ScriptLog;

namespace DNScriptAPI
{
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

#if defined (_VILLAGESERVER)
		if (ITEMTYPE_QUEST == pItem->nType) {
			return false;
		}
#endif

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

	int CheckInvenForQuestReward(CDNUserBase* pUser, const TQuestReward& QuestReward, const bool bSelectedArray[])
	{
		if (!pUser)
			return -1;

		const CDNUserItem* pUserItem = pUser->GetItem();
		if (!pUserItem)
			return -2;

		TInvenItemCnt TempInven[INVENTORYMAX];
		::memset(TempInven, 0, sizeof(TempInven));
#if defined(PRE_PERIOD_INVENTORY)
		for (int i = 0; i < INVENTORYTOTALMAX; i++)
		{
			if (i == pUserItem->GetInventoryCount()){
				if (pUserItem->IsEnablePeriodInventory())
					i = INVENTORYMAX;
				else
					break;
			}
#else	// #if defined(PRE_PERIOD_INVENTORY)
		for (int i = 0 ; i < INVENTORYMAX; ++i) 
		{
#endif	// #if defined(PRE_PERIOD_INVENTORY)
			const TItem* pItem = pUserItem->GetInventory(i);
			if (pItem) 
				TempInven[i].Set(pItem->nItemID, pItem->wCount, pItem->bSoulbound, pItem->cSealCount);
		}

		//만약 빌리지, 게임 둘다 아니면 컴파일 에러.
#if defined (_VILLAGESERVER)
		UINT nUserObjectID = static_cast<CDNUserSession*>(pUser)->GetObjectID();
#endif

#if defined (_GAMESERVER)
		UINT nUserObjectID = static_cast<CDNUserSession*>(pUser)->GetSessionID();
#endif

		bool bSelectedItems[QUESTREWARD_INVENTORYITEMMAX];
		CopyMemory(bSelectedItems, bSelectedArray, sizeof(bSelectedItems));
		if (QuestReward.cType == 1) // 전체 보상이면 아이템을 전부 선택한것으로 취급. (전체 보상과 선택 보상 로직 통일.)
		{
			for (int i = 0; i < QUESTREWARD_INVENTORYITEMMAX; ++i)
				bSelectedItems[i] = true;
		}

		for (int i = 0; i < QUESTREWARD_INVENTORYITEMMAX; ++i) 
		{
			if(!bSelectedItems[i])
				continue;

			if (QuestReward.ItemArray[i].nItemID <= 0 || QuestReward.ItemArray[i].nItemCount <= 0) 
				continue;

			TItem Item;
			if (!CDNUserItem::MakeItemStruct(QuestReward.ItemArray[i].nItemID, Item))
				return -3;

			Item.wCount = QuestReward.ItemArray[i].nItemCount;
			if (!__IsValidSpaceInventorySlotAndSet(TempInven, pUserItem->GetInventoryCount(), Item))
				return -4;
		}

		for (int i = 0; i < MAX_QUEST_LEVEL_CAP_REWARD; ++i)
		{
			if (QuestReward.LevelCapItemArray[i].nItemID <= 0 || QuestReward.LevelCapItemArray[i].nItemCount <= 0)
				continue;

			TItem Item;
			if (!CDNUserItem::MakeItemStruct(QuestReward.LevelCapItemArray[i].nItemID, Item))
				return -3;

			Item.wCount = QuestReward.LevelCapItemArray[i].nItemCount;
			if (!__IsValidSpaceInventorySlotAndSet(TempInven, pUserItem->GetInventoryCount(), Item))
				return -4;
		}

		return 1;
	}

	void api_npc_NextTalk(CDNUserBase* pUser, UINT nNpcObjectID, const char* szTalkIndex, const char* szTargetFile)
	{
		if (!pUser)
			return;

		WCHAR wszTalkIndex[512] = {0,};
		WCHAR wszTargetFile[512] = {0,};

		ZeroMemory(&wszTalkIndex, sizeof(WCHAR)*512);
		MultiByteToWideChar(CP_ACP, 0, szTalkIndex, -1, wszTalkIndex, 512 );

		ZeroMemory(&wszTargetFile, sizeof(WCHAR)*512);
		MultiByteToWideChar(CP_ACP, 0, szTargetFile, -1, wszTargetFile, 512 );

		// 다음 대사문단을 세팅해놓는다.
		// 유저한테 온 패킷이 이 대사 목록중에 없으면 뭔가가 조작된거임.. 
		TALK_PARAGRAPH& para = pUser->GetLastTalkParagraph();
		bool bResult = g_pDataManager->GetTalkParagraph(std::wstring(wszTalkIndex), std::wstring(wszTargetFile), para);
		if ( bResult == false )
		{
#if defined (_VILLAGESERVER)
			if (!g_pDataManager->CheckTalkAnswer(std::wstring(wszTalkIndex), std::wstring(wszTargetFile))) {	// 컷신 등은 talk_paragraph 없이 talk_answer 만 존재하는 경우가 있으며 이것은 유효함
				pUser->SendNextTalkError();
			}
#endif
			return;
		}

		pUser->SetCalledNpcResponse(true, true);

		std::vector<TalkParam>& talkParamList = pUser->GetTalkParamList();
		pUser->NextTalk(nNpcObjectID, wszTalkIndex, wszTargetFile, talkParamList );
		talkParamList.clear();
	}

	void api_npc_NextScript(CDNUserBase* pUser, CDnNpc* pNpc, UINT nNpcObjectID, const char* szTalkIndex, const char* szTargetFile)
	{
		WCHAR wszTalkIndex[512];
		WCHAR wszTargetFile[512];

		ZeroMemory(&wszTalkIndex, sizeof(WCHAR)*512);
		MultiByteToWideChar(CP_ACP, 0, szTalkIndex, -1, wszTalkIndex, 512 );

		ZeroMemory(&wszTargetFile, sizeof(WCHAR)*512);
		MultiByteToWideChar(CP_ACP, 0, szTargetFile, -1, wszTargetFile, 512 );

		// 다음 대사문단을 세팅해놓는다.
		// 유저한테 온 패킷이 이 대사 목록중에 없으면 뭔가가 조작된거임.. 
		TALK_PARAGRAPH& para = pUser->GetLastTalkParagraph();
		bool bResult = g_pDataManager->GetTalkParagraph(std::wstring(wszTalkIndex), std::wstring(wszTargetFile), para);
		if ( bResult == false ) return;

		bool& bSkipCheck = pUser->GetSkipParagraphCheck();
		bSkipCheck = true;

		pUser->SetCalledNpcResponse(true, true);

		pNpc->Talk(pUser, nNpcObjectID, std::wstring(wszTalkIndex), std::wstring(wszTargetFile) );
	}

	int api_npc_SetParamString(CDNUserBase* pUser, const char* szParamKey, int nParamID)
	{
		if (!pUser)
			return -1;

		std::vector<TalkParam>& talkParamList = pUser->GetTalkParamList();

		if ( talkParamList.size() > 10 )		return -2;
		std::wstring wszParam;
		ToWideString(std::string(szParamKey), wszParam);

		TalkParam talkparam;
		ZeroMemory(&talkparam, sizeof(TalkParam));
		talkparam.cType = TalkParam::STRING;
		_wcscpy(talkparam.wszKey, _countof(talkparam.wszKey), wszParam.c_str(), (int)wcslen(wszParam.c_str()));
		talkparam.nValue = nParamID;

		talkParamList.push_back(talkparam);
		return 1;
	}

	int api_npc_SetParamInt(CDNUserBase* pUser, const char* szParamKey, int nValue)
	{
		if (!pUser)
			return -1;

		std::vector<TalkParam>& talkParamList = pUser->GetTalkParamList();

		if ( talkParamList.size() > 10 )		return -2;
		std::wstring wszParam;
		ToWideString(std::string(szParamKey), wszParam);

		TalkParam talkparam;
		ZeroMemory(&talkparam, sizeof(TalkParam));
		talkparam.cType = TalkParam::INT;
		_wcscpy(talkparam.wszKey, _countof(talkparam.wszKey), wszParam.c_str(), (int)wcslen(wszParam.c_str()));
		talkparam.nValue = nValue;

		talkParamList.push_back(talkparam);
		return 1;
	}

	void api_log_AddLog(const char* szLog)
	{
#if !defined(_FINAL_BUILD)
		WCHAR wszMsg[512];

		ZeroMemory(&wszMsg, sizeof(WCHAR)*512);
		MultiByteToWideChar(CP_ACP, 0, szLog, -1, wszMsg, 512 );

		g_ScriptLog.Log(LogType::_FILELOG, wszMsg);
#endif //#if !defined(_FINAL_BUILD)
	}

	void api_log_UserLog(CDNUserBase* pUser, const char* szLog)
	{
#if !defined(_FINAL_BUILD)
		if (!pUser)
			return;

		WCHAR wszMsg[512]={0,};

		ZeroMemory(&wszMsg, sizeof(WCHAR)*512);
		MultiByteToWideChar(CP_ACP, 0, szLog, -1, wszMsg, 512 );

		std::wstring wszString;
		wszString = wszMsg;
		static_cast<CDNUserSession*>(pUser)->SendChat(CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str());
#endif //#if !defined(_FINAL_BUILD)
	}

	/*
	desc   : 유저에게 퀘스트 관련 정보를 채팅으로 보내준다.
	param  : 
	return : void
	*/
	void api_quest_DumpQuest(CDNUserBase* pUser)
	{
#if !defined(_FINAL_BUILD)
		if (!pUser)
			return;

		std::vector<TQuest> QuestList;
		pUser->GetQuest()->DumpQuest(QuestList);

		for ( size_t i = 0 ; i < QuestList.size() ; i++ )
		{
			TQuest& quest = QuestList[i];

			std::wstring wszString;
			wszString = FormatW( L"[Slot:%d] QID:%d Type:%d QStep:%d JStep:%d Marking:%d\n", i, 
				(int)quest.nQuestID, g_pQuestManager->GetQuestType(quest.nQuestID), (int)quest.nQuestStep, (int)quest.cQuestJournal,
				(!quest.nQuestID)?(0):(pUser->GetQuest()->IsClearQuest(quest.nQuestID))
				);

			static_cast<CDNUserSession*>(pUser)->SendChat(CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str());
		}
#endif //#if !defined(_FINAL_BUILD)
	}
		
	int api_quest_AddHuntingQuest(CDNUserBase* pUser, int nQuestID, int nQuestStep, int nJournalStep, int nCountingSlot, int nCountingType, int nCountingIndex, int nTargetCnt)
	{
		if (!pUser)
			return -1;

#if defined(_CH)
		if (pUser->GetFCMState() != FCMSTATE_NONE){
			pUser->SendQuestResult(ERROR_QUEST_FCM);
			return -1;
		}
#endif

		return pUser->GetQuest()->AddHuntingQuest(nQuestID, nQuestStep, nJournalStep, nCountingSlot, nCountingType, nCountingIndex, nTargetCnt);
	}

	// nQuestType : 사용하지 않는 매개변수임.
	int api_quest_AddQuest(CDNUserBase* pUser, int nQuestID, int nQuestType)
	{
		if (!pUser)
			return -1;

#if defined(_CH)
		if (pUser->GetFCMState() != FCMSTATE_NONE){
			pUser->SendQuestResult(ERROR_QUEST_FCM);
			return -1;
		}
#endif

		return pUser->GetQuest()->AddQuest(nQuestID);
	}

	int api_quest_CompleteQuest(CDNUserBase* pUser, int nQuestID, bool bDelPlayList)
	{
		if (!pUser)
			return -1;

#if defined(_CH)
		if (pUser->GetFCMState() != FCMSTATE_NONE){
			pUser->SendQuestResult(ERROR_QUEST_FCM);
			return -1;
		}
#endif

		// 퀘스트 완료
		int nReturn = pUser->GetQuest()->RewardAfterCompletingQuest(nQuestID, bDelPlayList);

		// 보상관련 변수 초기화
		pUser->GetQuest()->ResetRewardFlag();

		return nReturn;
	}

	int api_quest_IsMarkingCompleteQuest(CDNUserBase* pUser, int nQuestID)
	{
		if (!pUser) 
			return -1;

		return (pUser->GetQuest()->IsClearQuest(nQuestID)) ? 1 : 0;
	}

	int api_quest_UserHasQuest(CDNUserBase* pUser, int nQuestID)
	{
		if (!pUser) 
			return -1;

		return pUser->GetQuest()->HasQuest(nQuestID);
	}

	int api_quest_GetPlayingQuestCnt(CDNUserBase* pUser)
	{
		if (!pUser)
			return -1;

		return pUser->GetQuest()->GetPlayingQuestCount();
	}
	
	int api_npc_GetNpcIndex(CDnNpc* pNpc)
	{
		if (!pNpc) 
			return -2;
		
		return pNpc->GetNpcData()->nNpcID;
	}
	
	int api_quest_SetQuestStepAndJournalStep(CDNUserBase* pUser, int nQuestID, short nQuestStep, int nJournalStep)
	{
		if (!pUser)
			return -1;

#if defined(_CH)
		if (pUser->GetFCMState() != FCMSTATE_NONE){
			pUser->SendQuestResult(ERROR_QUEST_FCM);
			return -1;
		}
#endif

		if ( nJournalStep < 0 || nJournalStep > 255 )
			return -3;

		return pUser->GetQuest()->SetQuestStepAndJournalStep(nQuestID, nQuestStep, nJournalStep, true);
	}
	
	int api_quest_SetQuestStep(CDNUserBase* pUser, int nQuestID, short nQuestStep)
	{
		if (!pUser) 
			return -1;

#if defined(_CH)
		if (pUser->GetFCMState() != FCMSTATE_NONE){
			pUser->SendQuestResult(ERROR_QUEST_FCM);
			return -1;
		}
#endif

		return pUser->GetQuest()->SetQuestStep(nQuestID, nQuestStep);
	}

	int api_quest_GetQuestStep(CDNUserBase* pUser, int nQuestID)
	{
		if (!pUser) 
			return -1;

		return pUser->GetQuest()->GetQuestStep(nQuestID);
	}
	
	int api_quest_SetJournalStep(CDNUserBase* pUser, int nQuestID, int nJournalStep)
	{
		if (!pUser) 
			return -1;

#if defined(_CH)
		if (pUser->GetFCMState() != FCMSTATE_NONE){
			pUser->SendQuestResult(ERROR_QUEST_FCM);
			return -1;
		}
#endif

		if ( nJournalStep < 0 || nJournalStep > 255 )
			return -3;

		return pUser->GetQuest()->SetJournalStep(nQuestID, nJournalStep);
	}

	int api_quest_GetJournalStep(CDNUserBase* pUser, int nQuestID)
	{
		if (!pUser)
			return -1;

		return pUser->GetQuest()->GetJournalStep(nQuestID);
	}
	
	int api_quest_SetQuestMemo(CDNUserBase* pUser, int nQuestID, char nMemoIndex, int iVal)
	{
		if (!pUser)
			return -1;

#if defined(_CH)
		if (pUser->GetFCMState() != FCMSTATE_NONE){
			pUser->SendQuestResult(ERROR_QUEST_FCM);
			return -1;
		}
#endif

		if (QUESTMEMOREWARDCHECK == nMemoIndex) {
			// 퀘스트 보상 무한지급을 방지하기 위하여 할당받은 인덱스는 사용할 수 없음
			DN_RETURN(-3);
		}

		if (!CHECK_RANGE(nMemoIndex, 1, QUESTMEMOMAX)) {
			// 퀘스트 메모 인덱스 범위를 벗어남 (1~10)
			DN_RETURN(-3);
		}

		return pUser->GetQuest()->SetQuestMemo(nQuestID, nMemoIndex-1, iVal);
	}

	int api_quest_GetQuestMemo(CDNUserBase* pUser, int nQuestID, char nMemoIndex)
	{
		if (!pUser)
			return -1;

		if (!CHECK_RANGE(nMemoIndex, 1, QUESTMEMOMAX)) {
			// 퀘스트 메모 인덱스 범위를 벗어남 (1~10)
			DN_RETURN(-3);
		}

		return pUser->GetQuest()->GetQuestMemo(nQuestID, nMemoIndex-1);
	}
	
	int api_quest_SetCountingInfo(CDNUserBase* pUser, int nQuestID, int nSlot, int nCountingType, int nCountingIndex, int nTargetCnt)
	{
		if (!pUser)
			return -1;

		return pUser->GetQuest()->SetCountingInfo(nQuestID, nSlot, nCountingType, nCountingIndex, nTargetCnt);
	}

	int api_quest_ClearCountingInfo(CDNUserBase* pUser, int nQuestID)
	{
		if (!pUser)
			return -1;

		return pUser->GetQuest()->ClearCountingInfo(nQuestID);
	}
	
	int api_quest_IsAllCompleteCounting(CDNUserBase* pUser, int nQuestID)
	{
		if (!pUser) 
			return -1;

		return pUser->GetQuest()->IsAllCompleteCounting(nQuestID);
	}

	int api_user_CheckInvenForAddItem(CDNUserBase* pUser, int nItemIndex, int nItemCnt)
	{
		if (!pUser)
			return -1;

		CDNUserItem* pUserItem = pUser->GetItem();
		if (!pUserItem) {
			return -2;
		}

		TItem ResultItem;
		if( CDNUserItem::MakeItemStruct( nItemIndex, ResultItem ) == false )
			return -2;
		// 일단은 임시로 빈공간만 확인한다. 
		// 인벤에 빈공간이 없으면 
		if (!pUserItem->IsValidSpaceInventorySlot(nItemIndex, nItemCnt, ResultItem.bSoulbound, ResultItem.cSealCount, ResultItem.bEternity)) 
		{
#if defined (_GAMESERVER)
			pUser->SendQuestResult(ERROR_ITEM_INVENTORY_NOTENOUGH);
#endif
			return -3;
		}

		return 1;
	}
	
	int api_user_CheckInvenForAddItemList(CDNUserBase* pUser, lua_tinker::table ItemTable)
	{
		if (!pUser)
			return -1;

		CDNUserItem* pUserItem = pUser->GetItem();
		if (!pUserItem) {
			return -2;
		}

		TInvenItemCnt TempInven[INVENTORYMAX];
		{
			// 임시 인벤토리에 현재 인벤토리의 아이템 개수 정보들을 복사

			::memset(TempInven, 0, sizeof(TempInven));

			for (int iIndex = 0 ; INVENTORYMAX > iIndex ; ++iIndex) 
			{
				const TItem* pItem = pUserItem->GetInventory(iIndex);
				if (pItem) 
				{
					TempInven[iIndex].Set(pItem->nItemID, pItem->wCount, pItem->bSoulbound, pItem->cSealCount );
				}
				else 
				{
					TempInven[iIndex].Reset();
				}
			}
		}

		if (0 >= ItemTable.getSize()) 
		{
			// 아이템 테이블에 아무값도 없음 ?
			DN_RETURN(false);
		}

		for (int iIndex = 1 ; ItemTable.getSize() >= iIndex ; ++iIndex) 
		{
			lua_tinker::table pTable = ItemTable.get<lua_tinker::table>(iIndex);
			if (0 >= pTable.getSize()) 
			{
				continue;
			}

			TItem Item;
			if( CDNUserItem::MakeItemStruct( pTable.get<int>(1), Item ) == false )
				return -2;
			Item.wCount = pTable.get<int>(2);

			bool bResult = __IsValidSpaceInventorySlotAndSet(TempInven, pUserItem->GetInventoryCount(), Item );		// RandomSeed 는 어떻게 ?
			if (!bResult) 
			{
#if defined (_GAMESERVER)
				pUser->SendQuestResult(ERROR_ITEM_INVENTORY_NOTENOUGH);
#endif
				return -3;
			}
		}

		return 1;
	}

	int api_user_AddItem(CDNUserBase* pUser, int nItemIndex, int nItemCnt, int nQuestID)
	{
		if (!pUser)
			return -1;

#if defined(_CH)
		if (pUser->GetFCMState() != FCMSTATE_NONE){
			pUser->SendQuestResult(ERROR_QUEST_FCM);
			return -1;
		}
#endif

		CDNUserItem* pUserItem = pUser->GetItem();
		if ( !pUserItem )
			return -2;

#if defined (_VILLAGESERVER)
		int nResult = api_user_CheckInvenForAddItem(pUser, nItemIndex, nItemCnt);
		
		if ( nResult < 0 )
			return nResult;

		bool bResult = pUserItem->AddInventoryByQuest(nItemIndex, nItemCnt, nQuestID, 0);	// 아이템에 따라서 randomseed 를 어떻게 넣을껀지 결정해주시길!

#ifndef _FINAL_BUILD
		if ( bResult )
		{
			std::wstring wszMsg;
			wszMsg = FormatW( L"아이템 지급 : id %d count : %d" , nItemIndex, nItemCnt );
			pUser->SendChat(CHATTYPE_NORMAL, (int)wszMsg.size()*sizeof(WCHAR), L"", (WCHAR*)wszMsg.c_str());
		}
		else
		{
			std::wstring wszMsg;
			wszMsg = FormatW( L"아이템 지급 실패 : id %d count : %d" , nItemIndex, nItemCnt );
			pUser->SendChat(CHATTYPE_NORMAL, (int)wszMsg.size()*sizeof(WCHAR), L"", (WCHAR*)wszMsg.c_str());
		}
#endif //#ifndef _FINAL_BUILD
#endif //#if defined (_VILLAGESERVER)

#if defined (_GAMESERVER)
		TItem ResultItem;
		if( CDNUserItem::MakeItemStruct(nItemIndex, ResultItem) == false )
			return -2;
		// 일단은 임시로 빈공간만 확인한다. 
		// 인벤에 빈공간이 없으면 
		if ( !pUserItem->IsValidSpaceInventorySlot(nItemIndex, nItemCnt, ResultItem.bSoulbound, ResultItem.cSealCount, ResultItem.bEternity ) ) 
		{
			return -3;
		}

		bool bResult = pUserItem->AddInventoryByQuest(nItemIndex, nItemCnt, nQuestID, 0);	// 아이템에 따라서 randomseed 를 어떻게 넣을껀지 결정해주시길!

#if defined( PRE_ENABLE_QUESTCHATLOG )
		if ( bResult )
		{
			// 임시로 실제로 주진 않고 유저 채팅창에만 찍어준다.
			std::wstring wszMsg;
			wszMsg = FormatW( L"아이템 지급 : id %d count : %d" , nItemIndex, nItemCnt );
			pUser->SendChat(CHATTYPE_NORMAL, (int)wszMsg.size()*sizeof(WCHAR), L"", (WCHAR*)wszMsg.c_str());
		}
		else
		{
			// 임시로 실제로 주진 않고 유저 채팅창에만 찍어준다.
			std::wstring wszMsg;
			wszMsg = FormatW( L"아이템 지급 실패 : id %d count : %d" , nItemIndex, nItemCnt );
			pUser->SendChat(CHATTYPE_NORMAL, (int)wszMsg.size()*sizeof(WCHAR), L"", (WCHAR*)wszMsg.c_str());
		}
#endif // #if defined( PRE_ENABLE_QUESTCHATLOG )
#endif //#if defined (_GAMESERVER)

		return 1;
	}

	int api_user_DelItem(CDNUserBase* pUser, int nItemIndex, int nItemCnt, int nQuestID)
	{
		if (!pUser)
			return -1;

#if defined(_CH)
		if (pUser->GetFCMState() != FCMSTATE_NONE){
			pUser->SendQuestResult(ERROR_QUEST_FCM);
			return -1;
		}
#endif

		CDNUserItem* pUserItem = pUser->GetItem();
		if ( !pUserItem )
			return -2;

		int nCount = pUserItem->GetInventoryItemCount(nItemIndex);
		if ( nCount < nItemCnt )
		{
			return -3;
		}

		bool bResult = pUserItem->DeleteInventoryByQuest(nItemIndex, nItemCnt, nQuestID);

#if defined (_VILLAGESERVER)
#ifndef _FINAL_BUILD
		if ( bResult )
		{
			std::wstring wszMsg;
			wszMsg = FormatW( L"아이템 제거 : id %d count : %d" , nItemIndex, nItemCnt );
			pUser->SendChat(CHATTYPE_NORMAL, (int)wszMsg.size()*sizeof(WCHAR), L"", (WCHAR*)wszMsg.c_str());
		}
		else
		{
			std::wstring wszMsg;
			wszMsg = FormatW( L"아이템 제거 실패 : id %d count : %d" , nItemIndex, nItemCnt );
			pUser->SendChat(CHATTYPE_NORMAL, (int)wszMsg.size()*sizeof(WCHAR), L"", (WCHAR*)wszMsg.c_str());
		}
#endif
#endif

#if defined (_GAMESERVER)
#if defined (PRE_ENABLE_QUESTCHATLOG)
		if ( bResult )
		{
			std::wstring wszMsg;
			wszMsg = FormatW( L"아이템 제거 : id %d count : %d" , nItemIndex, nItemCnt );
			pUser->SendChat(CHATTYPE_NORMAL, (int)wszMsg.size()*sizeof(WCHAR), L"", (WCHAR*)wszMsg.c_str());
		}
		else
		{
			std::wstring wszMsg;
			wszMsg = FormatW( L"아이템 제거 실패 : id %d count : %d" , nItemIndex, nItemCnt );
			pUser->SendChat(CHATTYPE_NORMAL, (int)wszMsg.size()*sizeof(WCHAR), L"", (WCHAR*)wszMsg.c_str());
		}
#endif
#endif
		return 1;
	}

	int api_user_AllDelItem(CDNUserBase* pUser, int nItemIndex)
	{
		if (!pUser)
			return -1;

#if defined(_CH)
		if (pUser->GetFCMState() != FCMSTATE_NONE){
			pUser->SendQuestResult(ERROR_QUEST_FCM);
			return -1;
		}
#endif

		CDNUserItem* pUserItem = pUser->GetItem();
		if ( !pUserItem )
			return -2;

		int nCount = pUserItem->GetInventoryItemCount(nItemIndex);

		bool bResult = pUserItem->DeleteInventoryByQuest(nItemIndex, nCount, 0);

#if defined (_VILLAGESERVER)
#ifndef _FINAL_BUILD
		if ( bResult )
		{
			std::wstring wszMsg;
			wszMsg = FormatW( L"아이템 제거 : id %d count : %d" , nItemIndex, nCount );
			pUser->SendChat(CHATTYPE_NORMAL, (int)wszMsg.size()*sizeof(WCHAR), L"", (WCHAR*)wszMsg.c_str());
		}
		else
		{
			std::wstring wszMsg;
			wszMsg = FormatW( L"아이템 제거 실패 : id %d count : %d" , nItemIndex, nCount );
			pUser->SendChat(CHATTYPE_NORMAL, (int)wszMsg.size()*sizeof(WCHAR), L"", (WCHAR*)wszMsg.c_str());
		}
#endif
#endif

#if defined (_GAMESERVER)
#if defined (PRE_ENABLE_QUESTCHATLOG)
		if ( bResult )
		{
			std::wstring wszMsg;
			wszMsg = FormatW( L"아이템 제거 : id %d count : %d" , nItemIndex, nItemCnt );
			pUser->SendChat(CHATTYPE_NORMAL, (int)wszMsg.size()*sizeof(WCHAR), L"", (WCHAR*)wszMsg.c_str());
		}
		else
		{
			std::wstring wszMsg;
			wszMsg = FormatW( L"아이템 제거 실패 : id %d count : %d" , nItemIndex, nItemCnt );
			pUser->SendChat(CHATTYPE_NORMAL, (int)wszMsg.size()*sizeof(WCHAR), L"", (WCHAR*)wszMsg.c_str());
		}
#endif
#endif
		return 1;
	}

	int api_user_HasItem(CDNUserBase* pUser, int nItemIndex, int nItemCnt)
	{
		if (!pUser)
			return -1;

		CDNUserItem* pUserItem = pUser->GetItem();
		if ( !pUserItem )
			return -2;

		int nCount = pUserItem->GetInventoryItemCount(nItemIndex);
		if (nCount < nItemCnt)
		{
			return -3;
		}

		return nCount;
	}
	
	int api_user_GetUserClassID(CDNUserBase* pUser)
	{
		if (!pUser)
			return -1;

		return int(pUser->GetClassID());
	}

	int api_user_GetUserJobID(CDNUserBase* pUser)
	{
		if (!pUser)
			return -1;

		return pUser->GetUserJob();
	}

	int api_user_GetUserLevel(CDNUserBase* pUser)
	{
		if (!pUser)
			return -1;

		return int(pUser->GetLevel());
	}

	int api_user_GetUserInvenBlankCount(CDNUserBase* pUser)
	{
		if (!pUser)
			return -1;

		CDNUserItem* pUserItem = pUser->GetItem();
		if ( !pUserItem )
			return -2;

		return pUserItem->FindBlankInventorySlotCount();
	}

	int api_quest_GetUserQuestInvenBlankCount(CDNUserBase* pUser)
	{
		if (!pUser)
			return -1;

		CDNUserItem* pUserItem = pUser->GetItem();
		if ( !pUserItem )
			return -2;

		return pUserItem->FindBlankQuestInventorySlotCount();
	}
	
	int api_user_PlayCutScene(CDNUserBase* pUser, UINT nNpcObjectID, int nCutSceneTableID, bool bIgnoreFadeIn)
	{
		if (!pUser)
			return -1;

#ifndef _FINAL_BUILD
		DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TCUTSCENE );
		if( pSox->IsExistItem( nCutSceneTableID ) == false ) {
			std::wstring wszLog;
			wszLog = FormatW( L"동영상 플레이 파일 인덱스가 잘못되었습니다. : %d", nCutSceneTableID);
			pUser->SendChat(CHATTYPE_NORMAL, (int)wszLog.size()*sizeof(WCHAR), L"", (WCHAR*)wszLog.c_str());
			return -1;
		}
		std::string szFileName = pSox->GetFieldFromLablePtr( nCutSceneTableID, "_FileName" )->GetString();
		WCHAR wszMsg[512];

		ZeroMemory(&wszMsg, sizeof(WCHAR)*512);
		MultiByteToWideChar(CP_ACP, 0, szFileName.c_str(), -1, wszMsg, 512 );

		std::wstring wszLog;
		wszLog = FormatW( L"동영상 플레이 %s : ", wszMsg);
		pUser->SendChat(CHATTYPE_NORMAL, (int)wszLog.size()*sizeof(WCHAR), L"", (WCHAR*)wszLog.c_str());
#endif

		pUser->SendPlayCutScene( nCutSceneTableID,-1, -1, nNpcObjectID, !bIgnoreFadeIn, UINT_MAX );
		pUser->SetCalledNpcResponse(true, false);	// 보통 클라이언트에 보내는 UI 개설 요청 이후 대화로 이어지지 않기 때문에 에러가 아닌데도 본의아니게 NPC 응답여부 체크에 걸리게 되므로 대화한 것으로 처리해 줌

		return 1;
	}
	
	int api_ui_OpenWareHouse(CDNUserBase* pUser, int iItemID/*=0*/ )
	{
		if (!pUser)
			return -1;

#ifndef _FINAL_BUILD
		std::wstring wszLog;
		wszLog = FormatW( L"창고열기");
		pUser->SendChat(CHATTYPE_NORMAL, (int)wszLog.size()*sizeof(WCHAR), L"", (WCHAR*)wszLog.c_str());
#endif

		pUser->SendShowWarehouse( iItemID );
		pUser->SetCalledNpcResponse(true, false);	// 보통 클라이언트에 보내는 UI 개설 요청 이후 대화로 이어지지 않기 때문에 에러가 아닌데도 본의아니게 NPC 응답여부 체크에 걸리게 되므로 대화한 것으로 처리해 줌

		return 1;
	}

#if defined (PRE_MOD_GAMESERVERSHOP)
	int api_ui_OpenShop(CDNUserBase* pUser, int nShopID, Shop::Type::eCode Type)
	{

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
	}

	int api_ui_OpenSkillShop(CDNUserBase* pUser)
	{
#ifndef _FINAL_BUILD
		//std::wstring wszLog;
		//wszLog = FormatW( L"돈주고 언락하는 스킬샵 열기");
		//pUser->SendChat(CHATTYPE_NORMAL, (int)wszLog.size()*sizeof(WCHAR), L"", (WCHAR*)wszLog.c_str());
#endif

		pUser->SendSkillShopOpen();

		//TSkillShopData *pShopData = g_pDataManager->GetSkillShopData(nSkillShopID);
		//if ( !pShopData )
		//	return -2;
		//
		//pUser->m_nShopID = nSkillShopID;
		//pUser->SendSkillShopList(pShopData);

		pUser->SetCalledNpcResponse(true, false);	// 보통 클라이언트에 보내는 UI 개설 요청 이후 대화로 이어지지 않기 때문에 에러가 아닌데도 본의아니게 NPC 응답여부 체크에 걸리게 되므로 대화한 것으로 처리해 줌

		return 1;
	}
#endif
	
	int api_ui_OpenCompoundEmblem(CDNUserBase* pUser)
	{
		if (!pUser)
			return -1;

#ifndef _FINAL_BUILD
		std::wstring wszLog;
		wszLog = FormatW( L"문장보옥 합성 창열기");
		pUser->SendChat(CHATTYPE_NORMAL, (int)wszLog.size()*sizeof(WCHAR), L"", (WCHAR*)wszLog.c_str());
#endif

		pUser->SendOpenCompoundEmblem();
		pUser->SetCalledNpcResponse(true, false);	// 보통 클라이언트에 보내는 UI 개설 요청 이후 대화로 이어지지 않기 때문에 에러가 아닌데도 본의아니게 NPC 응답여부 체크에 걸리게 되므로 대화한 것으로 처리해 줌

		return 1;
	}
	
	int api_ui_OpenUpgradeJewel(CDNUserBase* pUser)
	{
		if (!pUser)
			return -1;

#ifndef _FINAL_BUILD
		std::wstring wszLog;
		wszLog = FormatW( L"무인상점 열기");
		pUser->SendChat(CHATTYPE_NORMAL, (int)wszLog.size()*sizeof(WCHAR), L"", (WCHAR*)wszLog.c_str());
#endif

		pUser->SendOpenUpdagrageJewel();
		pUser->SetCalledNpcResponse(true, false);	// 보통 클라이언트에 보내는 UI 개설 요청 이후 대화로 이어지지 않기 때문에 에러가 아닌데도 본의아니게 NPC 응답여부 체크에 걸리게 되므로 대화한 것으로 처리해 줌

		return 1;
	}
	
	int api_ui_OpenGlyphLift(CDNUserBase* pUser)
	{
		if ( !pUser ) 
			return -1;

#ifndef _FINAL_BUILD
		std::wstring wszLog;
		wszLog = FormatW( L"문장창 열기");
		pUser->SendChat(CHATTYPE_NORMAL, (int)wszLog.size()*sizeof(WCHAR), L"", (WCHAR*)wszLog.c_str());
#endif

		pUser->SendOpenGlyphLift();
		pUser->SetCalledNpcResponse(true, false);	// 보통 클라이언트에 보내는 UI 개설 요청 이후 대화로 이어지지 않기 때문에 에러가 아닌데도 본의아니게 NPC 응답여부 체크에 걸리게 되므로 대화한 것으로 처리해 줌

		return 1;
	}

	int api_ui_OpenMailBox(CDNUserBase* pUser)
	{
		if (!pUser)
			return -1;

#ifndef _FINAL_BUILD
		std::wstring wszLog;
		wszLog = FormatW( L"우편함 열기");
		pUser->SendChat(CHATTYPE_NORMAL, (int)wszLog.size()*sizeof(WCHAR), L"", (WCHAR*)wszLog.c_str());
#endif

		pUser->SendOpenMailBox();
		pUser->SetCalledNpcResponse(true, false);	// 보통 클라이언트에 보내는 UI 개설 요청 이후 대화로 이어지지 않기 때문에 에러가 아닌데도 본의아니게 NPC 응답여부 체크에 걸리게 되므로 대화한 것으로 처리해 줌

		return 1;
	}
	
	int api_ui_OpenDisjointItem(CDNUserBase* pUser)
	{
		if (!pUser)
			return -1;

#if defined (_VILLAGESERVER)
#ifndef _FINAL_BUILD
	wstring wszString = FormatW(L"스테이지에서만 사용 가능한 기능입니다.\r\n");
	pUser->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );
#endif	// _FINAL_BUILD
#endif
		
#if defined (_GAMESERVER)
#ifndef _FINAL_BUILD
		std::wstring wszLog;
		wszLog = FormatW( L"분해창 열기");
		pUser->SendChat(CHATTYPE_NORMAL, (int)wszLog.size()*sizeof(WCHAR), L"", (WCHAR*)wszLog.c_str());
#endif
#endif
		pUser->SendOpenDisjointItem();
		pUser->SetCalledNpcResponse(true, false);	// 보통 클라이언트에 보내는 UI 개설 요청 이후 대화로 이어지지 않기 때문에 에러가 아닌데도 본의아니게 NPC 응답여부 체크에 걸리게 되므로 대화한 것으로 처리해 줌

		return 1;
	}
	
	int api_ui_OpenCompoundItem(CDNUserBase* pUser, int nCompoundShopID)
	{
		if (!pUser)
			return -1;

#ifndef _FINAL_BUILD
		std::wstring wszLog;
		wszLog = FormatW( L"아이템 합성창 열기");
		pUser->SendChat(CHATTYPE_NORMAL, (int)wszLog.size()*sizeof(WCHAR), L"", (WCHAR*)wszLog.c_str());
#endif

		pUser->SendOpenCompoundItem(nCompoundShopID);
		pUser->SetCalledNpcResponse(true, false);	// 보통 클라이언트에 보내는 UI 개설 요청 이후 대화로 이어지지 않기 때문에 에러가 아닌데도 본의아니게 NPC 응답여부 체크에 걸리게 되므로 대화한 것으로 처리해 줌

		return 1;
	}

	int api_ui_OpenCashShop(CDNUserBase* pUser)
	{
		if (!pUser)
			return -1;

#ifndef _FINAL_BUILD
		std::wstring wszLog;
		wszLog = FormatW( L"캐쉬 아이템 상점 열기");
		pUser->SendChat(CHATTYPE_NORMAL, (int)wszLog.size()*sizeof(WCHAR), L"", (WCHAR*)wszLog.c_str());
#endif

		pUser->SendOpenCashShop();
		pUser->SetCalledNpcResponse(true, false);	// 보통 클라이언트에 보내는 UI 개설 요청 이후 대화로 이어지지 않기 때문에 에러가 아닌데도 본의아니게 NPC 응답여부 체크에 걸리게 되므로 대화한 것으로 처리해 줌

		return 1;
	}
	
	int api_ui_OpenGuildMgrBox(CDNUserBase* pUser, int nGuildMgrNo)
	{
		if (!pUser)
			return -1;

#ifndef _FINAL_BUILD
		std::wstring wszLog;
		wszLog = FormatW( L"길드 관리 대화상자 열기");
		pUser->SendChat(CHATTYPE_NORMAL, (int)wszLog.size()*sizeof(WCHAR), L"", (WCHAR*)wszLog.c_str());
#endif

		pUser->SendOpenGuildMgrBox(nGuildMgrNo);
		pUser->SetCalledNpcResponse(true, false);	// 보통 클라이언트에 보내는 UI 개설 요청 이후 대화로 이어지지 않기 때문에 에러가 아닌데도 본의아니게 NPC 응답여부 체크에 걸리게 되므로 대화한 것으로 처리해 줌

		return 1;
	}
	
	int api_ui_OpenGacha_JP(CDNUserBase* pUser, int nGachaShopID)
	{
#ifdef PRE_ADD_GACHA_JAPAN
		if (!pUser)
			return -1;

#ifndef _FINAL_BUILD
		std::wstring wszLog;
		wszLog = FormatW( L"일본 가챠폰 열기");
		pUser->SendChat(CHATTYPE_NORMAL, (int)wszLog.size()*sizeof(WCHAR), L"", (WCHAR*)wszLog.c_str());
#endif

		TGachaponData_JP* pGachaData = g_pDataManager->GetGachaponData_JP( nGachaShopID );
		if( !pGachaData )
			return -2;

		pUser->m_nGachaponShopID = nGachaShopID;
		pUser->SendGachaShopOpen_JP( nGachaShopID );

		// 보통 클라이언트에 보내는 UI 개설 요청 이후 대화로 이어지지 않기 때문에 에러가 아닌데도 본의아니게 NPC 응답여부 체크에 걸리게 되므로 대화한 것으로 처리해 줌
		pUser->SetCalledNpcResponse( true, false );

		return 1;
#endif
		return -3;
	}
	
	int api_ui_OpenGiveNpcPresent(CDNUserBase* pUser, int nNpcID)
	{
#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
		if (!pUser)
			return -1;

#ifndef _FINAL_BUILD
		std::wstring wszLog;
		wszLog = FormatW( L"NPC 선물하기 UI 열기");
		pUser->SendChat(CHATTYPE_NORMAL, (int)wszLog.size()*sizeof(WCHAR), L"", (WCHAR*)wszLog.c_str());
#endif

		// 클라이언트한테 npc 에게 선물하기 UI 띄우라고 패킷 보냄.
		pUser->SendOpenGiveNpcPresent( nNpcID );

		// 보통 클라이언트에 보내는 UI 개설 요청 이후 대화로 이어지지 않기 때문에 에러가 아닌데도 본의아니게 NPC 응답여부 체크에 걸리게 되므로 대화한 것으로 처리해 줌
		// 그렇지만 npc 선물하기는 계속 대화가 이어지므로 그냥 놔둔다. 이거 호출하면 대화 다이얼로그가 바로 닫힌다.
		//pUser->SetCalledNpcResponse( true, false );

		return 1;
#else
		return -1;
#endif
	}

	int api_ui_OpenDonation(CDNUserBase* pUser)
	{
		if (!pUser)
			return -1;

#if defined (PRE_ADD_DONATION)
		pUser->SendOpenDonation();
		pUser->SetCalledNpcResponse( true, false );
#endif // #if defined (PRE_ADD_DONATION)

		return 1;
	}

	int api_ui_OpenInventory(CDNUserBase* pUser)
	{
		if (!pUser)
			return -1;

		pUser->SendOpenInventory();
		pUser->SetCalledNpcResponse( true, false );

		return 1;
	}

	int api_user_UserMessage(CDNUserBase* pUser, int nType, int nBaseStringIdx, lua_tinker::table ParamTable)
	{
		if (!pUser)
			return -1;

		std::vector<DNReplaceString::DynamicReplaceStringInfo> ReplaceParamList;
		// 베이스 스트링 얻고
#if defined(PRE_ADD_MULTILANGUAGE)
		std::wstring wszBaseString = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nBaseStringIdx, pUser->m_eSelectedLanguage );
#else		//#if defined(PRE_ADD_MULTILANGUAGE)
		std::wstring wszBaseString = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nBaseStringIdx );
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)

		// 먼저 파라미터 테이블에 값이 있는지 확인 하고
		lua_tinker::table t = ParamTable.get<lua_tinker::table>(1);
		const char* __szKey = t.get<const char*>(1);

		// 있으면 
		if ( __szKey )
		{
			for ( int i = 1 ; i <= ParamTable.getSize() ; i++ )
			{
				lua_tinker::table pTable = ParamTable.get<lua_tinker::table>(i);
				char* szKey = pTable.get<char*>(1);
				int nValueType = pTable.get<int>(2);
				int nValue = pTable.get<int>(3);

				DNReplaceString::DynamicReplaceStringInfo param;
				ToWideString(szKey, param.szKey);
				param.cValueType = (char)nValueType;
				param.nValue = nValue;
				ReplaceParamList.push_back(param);
			}
		}

#if defined(_SERVER) &&defined(PRE_ADD_MULTILANGUAGE)
		g_ReplaceString.Relpace(wszBaseString, ReplaceParamList, pUser->m_eSelectedLanguage );
#else		//#if defined(_SERVER) &&defined(PRE_ADD_MULTILANGUAGE)
		g_ReplaceString.Relpace(wszBaseString, ReplaceParamList );
#endif		//#if defined(_SERVER) &&defined(PRE_ADD_MULTILANGUAGE)

		if ( wszBaseString.size() > 256)
			return -3;

		switch(nType)
		{
		case CHATTYPE_NORMAL:
		case CHATTYPE_PARTY:
		case CHATTYPE_PRIVATE:
		case CHATTYPE_GUILD:
		case CHATTYPE_CHANNEL:
		case CHATTYPE_SYSTEM:
			break;
		default:
			return -2;
		}

		pUser->SendChat((eChatType)nType, (int)wszBaseString.size()*sizeof(WCHAR), L"", (WCHAR*)wszBaseString.c_str());
		
		return 1;
	}
	
	int api_quest_AddSymbolItem(CDNUserBase* pUser, int nItemID, short wCount)
	{
		/*
		if (!pUser)
			return -1;

		bool bResult = false;
		if (pUser->GetItem()->AddSymbolItem(nItemID, wCount, 0, ITEMLOG_ADDQUESTINVEN) == ERROR_NONE) bResult = true;

#ifndef _FINAL_BUILD
		std::wstring wszMsg;
		wszMsg = FormatW( L"심볼아이템추가 %s ID:%d %d개" , bResult ? L"성공":L"실패", nItemID, (int)wCount);
		pUser->SendChat(CHATTYPE_NORMAL, (int)wszMsg.size()*sizeof(WCHAR), L"", (WCHAR*)wszMsg.c_str());
#endif
		*/

		return 1;
	}

	int api_quest_DelSymbolItem(CDNUserBase* pUser, int nItemID, short wCount)
	{
		/*
		if (!pUser)
			return -1;

		bool bResult = false;
		if (pUser->GetItem()->DelSymbolItem(nItemID, wCount, 0, ITEMLOG_DELETEQUESTINVEN) == ERROR_NONE) bResult = true;

#ifndef _FINAL_BUILD
		std::wstring wszMsg;
		wszMsg = FormatW( L"심볼아이템제거 %s ID:%d %d개" , bResult ? L"성공":L"실패", nItemID, (int)wCount);
		pUser->SendChat(CHATTYPE_NORMAL, (int)wszMsg.size()*sizeof(WCHAR), L"", (WCHAR*)wszMsg.c_str());
#endif
		*/

		return 1;
	}

	int api_quest_HasSymbolItem(CDNUserBase* pUser, int nItemIndex, int nItemCnt)
	{
		/*
		if (!pUser)
			return -1;

		CDNUserItem* pUserItem = pUser->GetItem();
		if ( !pUserItem )
			return -2;

		int nCount = pUserItem->ExistSymbolItemCount(nItemIndex);
#ifndef _FINAL_BUILD
		std::wstring wszMsg;
		wszMsg = FormatW( L"심볼아이템 조회 UserObjID: %d nItemID:%d %d개 %d개 있음" , nUserObjectID, nItemIndex, (int)nItemCnt, nCount);
		pUser->SendChat(CHATTYPE_NORMAL, (int)wszMsg.size()*sizeof(WCHAR), L"", (WCHAR*)wszMsg.c_str());
#endif

		if (nCount < nItemCnt)
		{
			return -3;
		}

		return nCount;
		*/

		return 0;
	}
	
	int api_quest_CheckQuestInvenForAddItem(CDNUserBase* pUser, int nItemIndex, int nItemCnt)
	{
		if (!pUser)
			return -1;

		CDNUserItem* pUserItem = pUser->GetItem();
		if (!pUserItem) {
			return -2;
		}

#if defined (_VILLAGESERVER)
		if (!pUserItem->IsValidSpaceQuestInventorySlot(nItemIndex, nItemCnt))
			return -3;
#endif

#if defined (_GAMESERVER)
		if (!pUserItem->IsValidSpaceQuestInventorySlot(nItemIndex, nItemCnt)) {
			pUser->SendQuestResult(ERROR_QUEST_INVENTORY_NOTENOUGH);
			return -3;
		}

		// 퀘스트 인벤토리 공간에 대한 별도의 체크와 사전알림 존재
		int nResult = pUserItem->FindBlankQuestInventorySlotCount();
		if (QUESTINVENBLANKCHECKMIN >= nResult) {
			pUser->SendQuestResult(ERROR_QUEST_INVENTORY_ALMOSTFULL);
		}
#endif
		return 1;
	}

	int api_quest_CheckQuestInvenForAddItemList(CDNUserBase* pUser, lua_tinker::table ItemTable)
	{
		if (!pUser)
			return -1;

		CDNUserItem* pUserItem = pUser->GetItem();
		if (!pUserItem) {
			return -2;
		}

		TQuestItemCnt TempQuestInven[QUESTINVENTORYMAX];
		{
			// 임시 퀘스트 인벤토리에 현재 퀘스트 인벤토리의 아이템 개수 정보들을 복사

			::memset(TempQuestInven, 0, sizeof(TempQuestInven));

			for (int iIndex = 0 ; QUESTINVENTORYMAX > iIndex ; ++iIndex) {
				const TQuestItem* pQuestItem = pUserItem->GetQuestInventory(iIndex);
				if (pQuestItem) {
					TempQuestInven[iIndex].Set(pQuestItem->nItemID, pQuestItem->wCount);
				}
				else {
					TempQuestInven[iIndex].Reset();
				}
			}
		}

		if (0 >= ItemTable.getSize()) {
			// 아이템 테이블에 아무값도 없음 ?
			DN_RETURN(false);
		}

		for (int iIndex = 1 ; ItemTable.getSize() >= iIndex ; ++iIndex) {
			lua_tinker::table pTable = ItemTable.get<lua_tinker::table>(iIndex);
			if (0 >= pTable.getSize()) {
				continue;
			}

			bool bResult = __IsValidSpaceQuestInventorySlotAndSet(TempQuestInven, pTable.get<int>(1), pTable.get<int>(2));
			if (!bResult) {
				return -3;
			}
		}

		return 1;
	}

	int api_quest_AddQuestItem(CDNUserBase* pUser, int nItemID, short wCount, int nQuestID)
	{
		if (!pUser)
			return -1;

#if defined(_CH)
		if (pUser->GetFCMState() != FCMSTATE_NONE){
			pUser->SendQuestResult(ERROR_QUEST_FCM);
			return -1;
		}
#endif

		bool bResult = false;
		if (pUser->GetItem()->AddQuestInventory(nItemID, wCount, nQuestID, DBDNWorldDef::AddMaterializedItem::QuestReward) == ERROR_NONE) bResult = true;

#ifndef _FINAL_BUILD
		std::wstring wszMsg;
		wszMsg = FormatW( L"퀘스트 아이템추가 %s ID:%d %d개" , bResult ? L"성공":L"실패", nItemID, (int)wCount);
		pUser->SendChat(CHATTYPE_NORMAL, (int)wszMsg.size()*sizeof(WCHAR), L"", (WCHAR*)wszMsg.c_str());
#endif

		return bResult ? 1 : 0;
	}

	int api_quest_DelQuestItem(CDNUserBase* pUser, int nItemID, short wCount, int nQuestID)
	{
		if (!pUser)
			return -1;

#if defined(_CH)
		if (pUser->GetFCMState() != FCMSTATE_NONE){
			pUser->SendQuestResult(ERROR_QUEST_FCM);
			return -1;
		}
#endif

		bool bResult = false;
		if (pUser->GetItem()->DeleteQuestInventory(nItemID, wCount, nQuestID, DBDNWorldDef::UseItem::Destroy) == ERROR_NONE) bResult = true;

#ifndef _FINAL_BUILD
		std::wstring wszMsg;
		wszMsg = FormatW( L"퀘스트 아이템제거 %s ID:%d %d개" , bResult ? L"성공":L"실패", nItemID, (int)wCount);
		pUser->SendChat(CHATTYPE_NORMAL, (int)wszMsg.size()*sizeof(WCHAR), L"", (WCHAR*)wszMsg.c_str());
#endif

		return bResult ? 1 : 0;
	}

	int api_quest_AllDelQuestItem(CDNUserBase* pUser, int nItemID)
	{
		if (!pUser)
			return -1;

#if defined(_CH)
		if (pUser->GetFCMState() != FCMSTATE_NONE){
			pUser->SendQuestResult(ERROR_QUEST_FCM);
			return -1;
		}
#endif
		bool bResult = false;

		CDNUserItem* pUserItem = pUser->GetItem();
		if ( !pUserItem )
			return -2;

		int nCount = pUserItem->GetQuestInventoryItemCount(nItemID);

		if (pUser->GetItem()->DeleteQuestInventory(nItemID, nCount, 0, DBDNWorldDef::UseItem::Destroy) == ERROR_NONE) bResult = true;

#ifndef _FINAL_BUILD
		std::wstring wszMsg;
		wszMsg = FormatW( L"퀘스트 아이템제거 %s ID:%d %d개" , bResult ? L"성공":L"실패", nItemID, (int)nCount);
		pUser->SendChat(CHATTYPE_NORMAL, (int)wszMsg.size()*sizeof(WCHAR), L"", (WCHAR*)wszMsg.c_str());
#endif

		return bResult ? 1 : 0;
	}

	int api_quest_HasQuestItem(CDNUserBase* pUser, int nItemIndex, int nItemCnt)
	{
		if (!pUser)
			return -1;

		CDNUserItem* pUserItem = pUser->GetItem();
		if ( !pUserItem )
			return -2;

		int nCount = pUserItem->GetQuestInventoryItemCount(nItemIndex);
#ifndef _FINAL_BUILD
		
		//만약 빌리지, 게임 둘다 아니면 컴파일 에러.
#if defined(_VILLAGESERVER)
		UINT nUserID = static_cast<CDNUserSession*>(pUser)->GetObjectID();
#endif

#if defined(_GAMESERVER)
		UINT nUserID = static_cast<CDNUserSession*>(pUser)->GetSessionID();
#endif
		std::wstring wszMsg;
		wszMsg = FormatW( L"퀘스트 아이템 조회 UserObjID: %d nItemID:%d %d개 %d개 있음" , nUserID, nItemIndex, (int)nItemCnt, nCount);
		pUser->SendChat(CHATTYPE_NORMAL, (int)wszMsg.size()*sizeof(WCHAR), L"", (WCHAR*)wszMsg.c_str());
#endif

		if (nCount < nItemCnt)
		{
			return -3;
		}

		return nCount;
	}

	int api_ui_OpenGuildCreate(CDNUserBase* pUser)
	{
		if (!pUser)
			return -1;

		// N/A

		return 1;
	}

	int api_ui_OpenQuestReward(CDNUserBase* pUser, int nRewardTableIndex, bool bActivate)
	{
		if (!pUser)
			return -1;

		pUser->SendOpenQuestReward(nRewardTableIndex, bActivate);
		pUser->SetCalledNpcResponse(true, false);	// 보통 클라이언트에 보내는 UI 개설 요청 이후 대화로 이어지지 않기 때문에 에러가 아닌데도 본의아니게 NPC 응답여부 체크에 걸리게 되므로 대화한 것으로 처리해 줌

		return 1;
	}

	int api_quest_RewardQuestUser(CDNUserBase* pUser, int nRewardTableIndext, int nQuestID, int nRewardCheck)
	{
		if (!pUser)
			return -1;

#if defined(_CH)
		if (pUser->GetFCMState() != FCMSTATE_NONE){
			pUser->SendQuestResult(ERROR_QUEST_FCM);
			return -1;
		}
#endif

		CDNUserItem* pUserItem = pUser->GetItem();
		if (!pUserItem) {
			return -2;
		}

		// 퀘스트 보상 테이블을 참조해서
		TQuestReward table;
		bool bResult = g_pDataManager->GetQuestReward(nRewardTableIndext, table);
		if (!bResult) {
			return -4;
		}

		DN_ASSERT(0 < table.nQuestID,	"Invalid!");
		DN_ASSERT(0 < table.nQuestStep,	"Invalid!");

		if (table.nQuestStep != pUser->GetQuest()->GetQuestStep(table.nQuestID) || (table.nClass != 0 && table.nClass != pUser->GetClassID()))
		{
			return -5;
		}

		ZeroMemory(table.LevelCapItemArray, sizeof(table.LevelCapItemArray));
		if (pUser->GetLevel() >= (int)CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::PlayerLevelLimit))
		{
			CDNQuest* pQuest = g_pQuestManager->GetQuest(table.nQuestID);
			if(!pQuest)
				return -6;

			if (pQuest->IsLevelCapReward())
			{
				const TQuestLevelCapReward* pLevelCapReward = g_pDataManager->GetQuestLevelCapReward(pQuest->GetQuestInfo().cQuestType, pUser->GetClassID());
				if (!pLevelCapReward)
				{
#if !defined (_FINAL_BUILD)
					wstring wszMsg;
					wszMsg = FormatW( L"퀘스트 레벨캡 보상 테이블 조회 실패 퀘스트ID:%d 타입:%d 클래스ID:%d ", table.nQuestID, pQuest->GetQuestInfo().cQuestType, pUser->GetClassID());
					pUser->SendChat(CHATTYPE_NORMAL, (int)wszMsg.size()*sizeof(WCHAR), L"", (WCHAR*)wszMsg.c_str());
#endif // #if !defined (_FINAL_BUILD)
					return -7;
				}

				CopyMemory(table.LevelCapItemArray, pLevelCapReward->Items, sizeof(pLevelCapReward->Items));
			}
		}

		if (pUser->GetQuest()) {
			pUser->GetQuest()->SetReward(table, nQuestID, nRewardCheck);

#ifndef _FINAL_BUILD
			//만약 빌리지, 게임 둘다 아니면 컴파일 에러.
#if defined (_VILLAGESERVER)
			UINT nUserObjectID = static_cast<CDNUserSession*>(pUser)->GetObjectID();
#endif

#if defined (_GAMESERVER)
			UINT nUserObjectID = static_cast<CDNUserSession*>(pUser)->GetSessionID();
#endif
			std::wstring wszMsg;
			wszMsg = FormatW( L"보상 세팅UserObjID: %d 테이블ID:%d 퀘스트IDL%d " ,
				nUserObjectID, nRewardTableIndext, nQuestID);
			pUser->SendChat(CHATTYPE_NORMAL, (int)wszMsg.size()*sizeof(WCHAR), L"", (WCHAR*)wszMsg.c_str());
#endif
		}

		return 1;
	}

	int api_user_GetMapIndex(CDNUserBase* pUser)
	{
		if (!pUser)
			return -1;

		return pUser->GetMapIndex();
	}
	
	int api_user_GetLastStageClearRank(CDNUserBase* pUser)
	{
		if (!pUser)
			return -1;

		return pUser->GetLastStageClearRank();
	}
		
	int api_user_EnoughCoin(CDNUserBase* pUser, int nCoin)
	{
		if (!pUser)
			return -1;

#if defined(_CH)
		if (pUser->GetFCMState() != FCMSTATE_NONE){
			pUser->SendQuestResult(ERROR_QUEST_FCM);
			return -1;
		}
#endif

		if( pUser->GetCoin() < (INT64)nCoin )
			return -2;

		return 1;
	}
	
	int api_user_GetCoin(CDNUserBase* pUser)
	{
		if (!pUser)
			return -1;

#if defined(_CH)
		if (pUser->GetFCMState() != FCMSTATE_NONE){
			pUser->SendQuestResult(ERROR_QUEST_FCM);
			return -1;
		}
#endif

		return static_cast<int>(pUser->GetCoin());
	}
	
	int api_user_IsMissionGained(CDNUserBase* pUser, int nMissionIndex)
	{
		if (!pUser)
			return -1;

#if defined(_CH)
		if (pUser->GetFCMState() != FCMSTATE_NONE){
			pUser->SendQuestResult(ERROR_QUEST_FCM);
			return -1;
		}
#endif

		// 해당 미션이 없거나 비활성화 상태이면 실패
		TMissionData *pData = g_pDataManager->GetMissionData(nMissionIndex);
		if (!pData || !pData->bActivate) return 0;

		return(GetBitFlag(pUser->GetMissionData()->MissionGain, nMissionIndex));
	}
	
	int api_user_IsMissionAchieved(CDNUserBase* pUser, int nMissionIndex)
	{
		DN_ASSERT(CHECK_LIMIT(nMissionIndex, MISSIONMAX),	"Invalid!");

		if (!pUser)
			return -1;

#if defined(_CH)
		if (pUser->GetFCMState() != FCMSTATE_NONE){
			pUser->SendQuestResult(ERROR_QUEST_FCM);
			return -1;
		}
#endif

		// 해당 미션이 없거나 비활성화 상태이면 실패
		TMissionData *pData = g_pDataManager->GetMissionData(nMissionIndex);
		if (!pData || !pData->bActivate) {
			return 0;
		}

		return(GetBitFlag(pUser->GetMissionData()->MissionAchieve, nMissionIndex));
	}
	
	int api_user_HasItemWarehouse(CDNUserBase* pUser, int nItemIndex, int nItemCnt)
	{
		if (!pUser)
			return -1;

#if defined(_CH)
		if (pUser->GetFCMState() != FCMSTATE_NONE){
			pUser->SendQuestResult(ERROR_QUEST_FCM);
			return -1;
		}
#endif

		const CDNUserItem* pUserItem = pUser->GetItem();
		if (!pUserItem) {
			return -2;
		}

		int nCount = pUserItem->GetWarehouseItemCount(nItemIndex);
		if (nCount < nItemCnt) {
			return -3;
		}

		return nCount;
	}

	int api_user_HasItemEquip(CDNUserBase* pUser, int nItemIndex)
	{
		if (!pUser)
			return -1;

#if defined(_CH)
		if (pUser->GetFCMState() != FCMSTATE_NONE){
			pUser->SendQuestResult(ERROR_QUEST_FCM);
			return -1;
		}
#endif

		const CDNUserItem* pUserItem = pUser->GetItem();
		if (!pUserItem) {
			return -2;
		}

		bool bResult = pUserItem->CheckEquipByItemID(nItemIndex);
		if (!bResult) {
			return -2;
		}

		return 1;
	}
	
	int api_guild_GetGuildMemberRole(CDNUserBase* pUser)
	{
		if (!pUser)
			return -1;

#if defined(_CH)
		if (pUser->GetFCMState() != FCMSTATE_NONE){
			pUser->SendQuestResult(ERROR_QUEST_FCM);
			return -1;
		}
#endif

		if (pUser->GetGuildSelfView().IsSet()) {
			return(pUser->GetGuildSelfView().btGuildRole + 1);
		}

		return 0;
	}
	
	int api_quest_IncQuestMemo(CDNUserBase* pUser, int nQuestID, char nMemoIndex)
	{
		if (!pUser)
			return LONG_MIN;

#if defined(_CH)
		if (pUser->GetFCMState() != FCMSTATE_NONE){
			pUser->SendQuestResult(ERROR_QUEST_FCM);
			return -1;
		}
#endif

		if (!CHECK_RANGE(nMemoIndex, 1, QUESTMEMOMAX)) {
			// 퀘스트 메모 인덱스 범위를 벗어남 (1~10)
			DN_RETURN(LONG_MIN);
		}

		if (QUESTMEMOREWARDCHECK == nMemoIndex) {
			// 퀘스트 보상 무한지급을 방지하기 위하여 할당받은 인덱스는 사용할 수 없음
			DN_RETURN(LONG_MIN);
		}

		return pUser->GetQuest()->IncQuestMemo(nQuestID, nMemoIndex-1);
	}

	int api_quest_DecQuestMemo(CDNUserBase* pUser, int nQuestID, char nMemoIndex)
	{
		if (!pUser)
			return LONG_MAX;

#if defined(_CH)
		if (pUser->GetFCMState() != FCMSTATE_NONE){
			pUser->SendQuestResult(ERROR_QUEST_FCM);
			return -1;
		}
#endif

		if (!CHECK_RANGE(nMemoIndex, 1, QUESTMEMOMAX)) {
			// 퀘스트 메모 인덱스 범위를 벗어남 (1~10)
			DN_RETURN(LONG_MAX);
		}

		if (QUESTMEMOREWARDCHECK == nMemoIndex) {
			// 퀘스트 보상 무한지급을 방지하기 위하여 할당받은 인덱스는 사용할 수 없음
			DN_RETURN(LONG_MAX);
		}

		return pUser->GetQuest()->DecQuestMemo(nQuestID, nMemoIndex-1);
	}
	
	int api_user_SetUserJobID(CDNUserBase* pUser, int nJobID)
	{
		if (!pUser)
			return -1;

#if defined(_CH)
		if (pUser->GetFCMState() != FCMSTATE_NONE){
			pUser->SendQuestResult(ERROR_QUEST_FCM);
			return -1;
		}
#endif

		if (nJobID == pUser->GetUserJob()) {
			return 0;
		}

		DNTableFileFormat* pJobTable = GetDNTable( CDnTableDB::TJOB );

		// 현재 직업의 단계값과 루트 직업을 얻어옴.
		int iNowJob = pUser->GetUserJob();
		int iNowJobDeep = 0;
		int iNowRootJob = 0;
		for( int i = 0; i < pJobTable->GetItemCount(); ++i )
		{
			int iItemID = pJobTable->GetItemID( i );
			if( iItemID == iNowJob )
			{
				iNowJobDeep = pJobTable->GetFieldFromLablePtr( iItemID, "_JobNumber" )->GetInteger();
				iNowRootJob = pJobTable->GetFieldFromLablePtr( iItemID, "_BaseClass" )->GetInteger();
				break;
			}
		}

		// 바꾸기 원하는 직업과 단계가 같거나 큰지 확인.
		bool bSuccess = false;
		map<int, int> mapRootJob;
		for( int i = 0; i < pJobTable->GetItemCount(); ++i )
		{
			int iItemID = pJobTable->GetItemID( i );
			if( iItemID == nJobID )
			{
				int iJobRootToChange = pJobTable->GetFieldFromLablePtr( iItemID, "_BaseClass" )->GetInteger();
				if( iNowRootJob == iJobRootToChange )
				{
					int iJobDeepToChange = pJobTable->GetFieldFromLablePtr( iItemID, "_JobNumber" )->GetInteger();
					if( iNowJobDeep < iJobDeepToChange )
					{
						// 부모 직업도 맞아야 함.
						int iParentJobID = pJobTable->GetFieldFromLablePtr( iItemID, "_ParentJob" )->GetInteger();
						if( iParentJobID == iNowJob )
						{
							pUser->SetUserJob( nJobID );
							bSuccess = true;
							break;
						}
						else
						{
							// 바꾸고자 하는 직업의 부모 직업이 현재 직업이 아님.
#ifndef _FINAL_BUILD
							wstring wszString = FormatW(L"현재 직업에선 전직 할 수 없는 직업입니다.!!\r\n");
							pUser->SendChat(CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str());
#endif // _FINAL_BUILD
							return -3;
						}
					}
					else
					{
						// 바꾸고자하는 직업이 아래 단계임. 못바꿈.
#ifndef _FINAL_BUILD
						wstring wszString = FormatW(L"같거나 낮은 단계의 직업으로 바꿀 수 없습니다!!\r\n");
						pUser->SendChat(CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str());
#endif	// _FINAL_BUILD
						return -3;
					}
				}
				else
				{
					// 바꾸고자하는 직업이 다른 클래스임. 못바꿈.
#ifndef _FINAL_BUILD
					wstring wszString = FormatW(L"다른 클래스의 직업으로 바꿀 수 없습니다!!\r\n");
					pUser->SendChat(CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str());
#endif	// _FINAL_BUILD
					return -3;
				}
			}
		}

		if( false == bSuccess )
		{
#ifndef _FINAL_BUILD
			wstring wszString = FormatW(L"잘못된 Job ID 입니다..\r\n");
			pUser->SendChat(CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str());
#endif	// _FINAL_BUILD
			return -2;
		}

		return 1;
	}
	
	int api_user_IsJobInSameLine(CDNUserBase* pUser, int nBaseJobID)
	{
		if (!pUser)
			return -1;

		DNTableFileFormat* pSox = GetDNTable(CDnTableDB::TJOB);
		if (!pSox) {
			g_Log.Log( LogType::_FILELOG, L"JobTable.ext failed\r\n");
			return -2;
		}

		int nJobID = pUser->GetUserJob();
		if (!pSox->IsExistItem(nJobID)) {
			return 0;
		}

		do {
			if (nBaseJobID == nJobID) {
				return 1;
			}

			DNTableCell *pSoxField = pSox->GetFieldFromLablePtr(nJobID, "_ParentJob");
			if (!pSoxField) {
				break;
			}

			nJobID = pSoxField->GetInteger();
			if (0 >= nJobID) {
				break;
			}

		} while(1);

		return 0;
	}
	
	int api_user_HasCashItem(CDNUserBase* pUser, int nItemIndex, int nItemCnt)
	{
		if (!pUser)
			return -1;

		CDNUserItem* pUserItem = pUser->GetItem();
		if ( !pUserItem )
			return -2;

		int nCount = pUserItem->GetCashItemCountByItemID(nItemIndex);
		if (nCount < nItemCnt)
		{
			return -3;
		}

		return nCount;
	}

	int api_user_HasCashItemEquip(CDNUserBase* pUser, int nItemIndex)
	{
		if (!pUser)
			return -1;

#if defined(_CH)
		if (pUser->GetFCMState() != FCMSTATE_NONE){
			pUser->SendQuestResult(ERROR_QUEST_FCM);
			return -1;
		}
#endif

		const CDNUserItem* pUserItem = pUser->GetItem();
		if (!pUserItem) {
			return -2;
		}

		bool bResult = pUserItem->IsEquipCashItemExist(nItemIndex);
		if (!bResult) {
			return -2;
		}

		return 1;
	}
	
	int api_quest_IncCounting(CDNUserBase* pUser, int nCountingType, int nCountingIndex)
	{
		if (!pUser)
			return -1;

		pUser->GetQuest()->OnCounting(nCountingType, nCountingIndex);

		return 1;
	}
	
	int api_quest_IsPlayingQuestMaximum(CDNUserBase* pUser)
	{
		if (!pUser)
			return -1;

		if (MAX_PLAY_QUEST <= pUser->GetQuest()->GetPlayingQuestCount()) {
			return 1;
		}

		return 0;
	}
	
	int api_quest_ForceCompleteQuest(CDNUserBase* pUser, int nQuestID, int nQuestCode, int bDoMark, int bDoDelete, int bDoRepeat)
	{
		if (!pUser)
			return -1;

		CDNUserQuest* pQuest = pUser->GetQuest();
		if (!pQuest) {
			return -2;
		}

		int aRetVal = pQuest->ForceCompleteQuest(nQuestID, nQuestCode, bDoMark, bDoDelete, bDoRepeat);
		if (1 != aRetVal) {
			// 오류 발생
		}
		
		return aRetVal;
	}

	int api_npc_GetFavorPoint(CDNUserBase* pUser, int iNpcID)
	{
#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
		if (!pUser)
			return -1;

		CReputationSystemRepository* pReputationSystem = pUser->GetReputationSystem();
		if( !pReputationSystem )
			return -2;

		return static_cast<int>(pReputationSystem->GetNpcReputation( iNpcID, IReputationSystem::NpcFavor ));
#else
		return -1;
#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
	}
	
	int api_npc_GetMalicePoint(CDNUserBase* pUser, int iNpcID)
	{
#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
		if (!pUser)
			return -1;

		CReputationSystemRepository* pReputationSystem = pUser->GetReputationSystem();
		if( !pReputationSystem )
			return -2;

		return static_cast<int>(pReputationSystem->GetNpcReputation( iNpcID, IReputationSystem::NpcMalice ));
#else
		return -1;
#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
	}

	int api_npc_GetFavorPercent(CDNUserBase* pUser, int iNpcID)
	{
#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
		if (!pUser)
			return -1;

		CReputationSystemRepository* pReputationSystem = pUser->GetReputationSystem();
		if( !pReputationSystem )
			return -2;

		return pReputationSystem->GetNpcReputationPercent( iNpcID, IReputationSystem::NpcFavor );
#else
		return -1;
#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
	}

	int api_npc_GetMalicePercent(CDNUserBase* pUser, int iNpcID )
	{
#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
		if (!pUser)
			return -1;

		CReputationSystemRepository* pReputationSystem = pUser->GetReputationSystem();
		if( !pReputationSystem )
			return -2;

		return pReputationSystem->GetNpcReputationPercent( iNpcID, IReputationSystem::NpcMalice );
#else
		return -1;
#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
	}

	int api_npc_AddFavorPoint(CDNUserBase* pUser, int iNpcID, int val)
	{
#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
		if (!pUser)
			return -1;

		CReputationSystemRepository* pReputationSystem = pUser->GetReputationSystem();
		if( !pReputationSystem )
			return -2;

		CNpcReputationProcessor::Process( static_cast<CDNUserSession*>(pUser), iNpcID, IReputationSystem::NpcFavor, val );
		return 1;
#else
		return -1;
#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
	}

	int api_npc_AddMalicePoint(CDNUserBase* pUser, int iNpcID, int val)
	{
#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
		if (!pUser)
			return -1;

		CReputationSystemRepository* pReputationSystem = pUser->GetReputationSystem();
		if( !pReputationSystem )
			return -2;

		CNpcReputationProcessor::Process( static_cast<CDNUserSession*>(pUser), iNpcID, IReputationSystem::NpcMalice, val );
		return 1;
#else
		return -1;
#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
	}

	int api_npc_SendSelectedPresent(CDNUserBase* pUser, int iNpcID )
	{
#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
		if (!pUser)
			return -1;

		// 클라이언트에게 선택된 선물의 정보를 보내라고 패킷 보낸다.
		pUser->SendRequestSendSelectedPresent( iNpcID );

		return 1;
#else
		return -1;
#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
	}
	
	int api_npc_Rage(CDNUserBase* pUser, int iNpcID)
	{
#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
		if (!pUser)
			return -1;

		pUser->SendShowNpcEffect( iNpcID, 0 );

		return 1;
#else
		return -1;
#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
	}

	int api_npc_Disappoint(CDNUserBase* pUser, int iNpcID)
	{
#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
		if (!pUser)
			return -1;

		pUser->SendShowNpcEffect( iNpcID, 1 );

		return 1;
#else 
		return -1;
#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
	}

	int api_user_RepairItem(CDNUserBase* pUser, int iAllRepair)
	{
#if defined(PRE_ADD_REPAIR_NPC)
		if (!pUser)
			return -1;
		int nRet = ERROR_NONE;
		if( iAllRepair )		
		{
			nRet = pUser->GetItem()->CheckRepairAll(false);	
			if( nRet == ERROR_NONE )
				pUser->SendRepairAll(ERROR_NONE);			
		}
		else		
		{
			nRet = pUser->GetItem()->CheckRepairEquip(0);		
			if( nRet == ERROR_NONE )
				pUser->SendRepairEquip(ERROR_NONE);
		}

		if( nRet == ERROR_NONE)
			return 2; //아이템 수리 완료
		else if( nRet == ERROR_ITEM_REPAIR)
			return 0; // 수리할 아이템 없음
		else if( nRet == ERROR_ITEM_INSUFFICIENCY_MONEY)
			return 1; // 소지금 부족
		return -1;
#else
		return -1;
#endif
	}

	int api_ui_OpenExchangeEnchant(CDNUserBase* pUser)
	{
#ifdef PRE_ADD_EXCHANGE_ENCHANT
		if (!pUser)
			return -1;

#ifndef _FINAL_BUILD
		std::wstring wszLog;
		wszLog = FormatW( L"강화이동 오픈");
		pUser->SendChat(CHATTYPE_NORMAL, (int)wszLog.size()*sizeof(WCHAR), L"", (WCHAR*)wszLog.c_str());
#endif

		pUser->SendOpenExchangeEnchant();
		pUser->SetCalledNpcResponse(true, false);	// 보통 클라이언트에 보내는 UI 개설 요청 이후 대화로 이어지지 않기 때문에 에러가 아닌데도 본의아니게 NPC 응답여부 체크에 걸리게 되므로 대화한 것으로 처리해 줌

		return 1;
#else
		return -1;
#endif
	}

	int api_env_CheckCloseGateByTime(int iMapID)
	{
#ifdef PRE_ADD_CHALLENGE_DARKLAIR
		return g_pDataManager->IsCloseGateByTime(iMapID);
#else
		return 0;
#endif
	}

#if defined(PRE_ADD_OPEN_QUEST_TEXTURE_DIALOG)
	int api_Open_Texture_Dialog(CDNUserBase* pUser, SCOpenTextureDialog data)
	{
		if(!pUser)
			return -1;
		
		pUser->SetCalledNpcResponse(true, true); // 보통 클라이언트에 보내는 UI 개설 요청 이후 대화로 이어지지 않기 때문에 에러가 아닌데도 본의아니게 NPC 응답여부 체크에 걸리게 되므로 대화한 것으로 처리해 줌
		pUser->SendOpenTextureDialog(data);
		return 1;
	}

	int api_Close_Texture_Dialog(CDNUserBase* pUser, int nDlgID)
	{
		if(!pUser)
			return -1;

		pUser->SetCalledNpcResponse(true, true); // 보통 클라이언트에 보내는 UI 개설 요청 이후 대화로 이어지지 않기 때문에 에러가 아닌데도 본의아니게 NPC 응답여부 체크에 걸리게 되므로 대화한 것으로 처리해 줌
		pUser->SendCloseTextureDialog(nDlgID);
		return 1;
	}
#endif
	int api_quest_WorldBuffCheck(int nItemID)
	{		
#if defined( PRE_DRAGONBUFF )
		if( g_pDataManager )
		{
			if( g_pDataManager->bIsWorldBuffData(nItemID) )
				return 1;
		}
#endif
		return -1;		
	}

	int api_quest_ApplyWorldBuff(CDNUserBase* pUser, int nItemID, int nMapIdx)
	{
		if(!pUser)
			return -1;
#if defined(_VILLAGESERVER) && defined( PRE_DRAGONBUFF )
		if( g_pUserSessionManager )
		{
			if( g_pDataManager )
			{
				g_pDataManager->AddWorldBuffData(nItemID);
			}
			g_pUserSessionManager->ApplyWorldBuff( pUser->GetCharacterName(), nItemID, nMapIdx);
		}

		if( g_pMasterConnection && g_pMasterConnection->GetActive() )
		{
			g_pMasterConnection->SendApplyWorldBuff( pUser->GetCharacterName(), nItemID, nMapIdx );				
		}
#endif
		return -1;		
	}

	int api_Open_ChangeJobDialog(CDNUserBase* pUser)
	{
		if (!pUser)
			return -1;

		pUser->SetCalledNpcResponse(true, true); // 보통 클라이언트에 보내는 UI 개설 요청 이후 대화로 이어지지 않기 때문에 에러가 아닌데도 본의아니게 NPC 응답여부 체크에 걸리게 되므로 대화한 것으로 처리해 줌
		pUser->SendOpenChangeJobDialog();
		return 1;
	}
}