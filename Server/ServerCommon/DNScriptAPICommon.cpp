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
	// desc : �κ��丮�� Ư�� �󽽷Կ� �������� ���� (P.S.> api_user_CheckInvenForAddItemList ���� ����)
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

	// desc : �κ��丮�� �󽽷Ե鿡 �������� Ư�� ������ŭ ���� (P.S.> api_user_CheckInvenForAddItemList ���� ����)
	bool __SetBlankInventorySlotCount(TInvenItemCnt pTempInven[INVENTORYMAX], const TItem& Item, int nOverlapCount, int nCount /* ä�� ���԰��� */)
	{
		DN_ASSERT(NULL != pTempInven,	"Invalid!");
		DN_ASSERT(0 != Item.nItemID,			"Invalid!");
		DN_ASSERT(0 < Item.wCount,			"Invalid!");
		DN_ASSERT(0 < nOverlapCount,	"Invalid!");
		DN_ASSERT(0 < nCount,			"Invalid!");

		// !!! ���� - �� �Լ� ȣ�� ���� üũ�� Ȯ���� �Ǿ����� �����ϰ� �����ϴ� �κи� ����

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

	// desc : �κ��丮�� Ư�� ���Կ� �������� ��ø ���� (P.S.> api_user_CheckInvenForAddItemList ���� ����)
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

	// desc : �κ��丮�� �󽽷� ã�� (P.S.> api_user_CheckInvenForAddItemList ���� ����)
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

	// desc : �κ��丮�� �󽽷��� �� ���� �ִ��� ���� ã�� (P.S.> api_user_CheckInvenForAddItemList ���� ����)
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

	// desc : �κ��丮�� �� �� �ִ� ������ �� ���� ������ wCount ���� �� �ִ� Index ã�� (P.S.> api_user_CheckInvenForAddItemList ���� ����)
	int __FindOverlapInventorySlot(TInvenItemCnt pTempInven[INVENTORYMAX], int nTempInvenCount, const TItem& Item)
	{
		DN_ASSERT(NULL != pTempInven,								"Invalid!");
		DN_ASSERT(CHECK_RANGE(nTempInvenCount, 0, INVENTORYMAX),	"Invalid!");

		if ((Item.nItemID <= 0) || (Item.wCount <= 0)) DN_RETURN(-1);

		TItemData *pItemData = NULL;
		for (int iIndex = 0 ; nTempInvenCount > iIndex ; ++iIndex) {
			if (pTempInven[iIndex].nItemID == 0) {
				// ��ĭ�̸� �н�
				continue;
			}

			pItemData = g_pDataManager->GetItemData(pTempInven[iIndex].nItemID);
			if (!pItemData) continue;

			if (pItemData->nOverlapCount == 1) continue;			// ��ġ�� ������ �н�
			if( CDNUserItem::bIsDifferentItem( &Item, &pTempInven[iIndex] ) ) continue;	// ���� �������� �ƴ�

			if (pItemData->nOverlapCount >= pTempInven[iIndex].wCount + Item.wCount) {
				return iIndex;
			}
		}

		return -1;
	}

	// desc : �κ��丮�� �������� ���� �� �ִ� ������ ������� �˻� (P.S.> api_user_CheckInvenForAddItemList ���� ����)
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
		if (pItem->nOverlapCount == 1){		// ��ġ�� �ʴ� ������
			nCount = __FindBlankInventorySlotCount(pTempInven, nTempInvenCount);
			if (Item.wCount > nCount) {
				// �󽽷��̶� ���ϱ�
				return false;
			}

			return(__SetBlankInventorySlotCount(pTempInven, Item, 1, nCount));
		}
		else {
			// �������� ������ (�κ��� ���� �����۵��� ��� �����־ �Ű�Ⱦ��� ����ϰ� �ִ� ��츸 ��������)
			if (pItem->nOverlapCount < Item.wCount){		// �ѹ��� ��ġ�� �纸�� �������
				int nBundle = Item.wCount / pItem->nOverlapCount;
				if ((Item.wCount % pItem->nOverlapCount) > 0) nBundle++;

				nCount = __FindBlankInventorySlotCount(pTempInven, nTempInvenCount);
				if (nBundle > nCount) {
					// �����̶� �󽽷��̶� ���ϱ�
					return false;
				}

				return(__SetBlankInventorySlotCount(pTempInven, Item, pItem->nOverlapCount, nCount));
			}
			else {
				int nBlank = __FindOverlapInventorySlot(pTempInven, nTempInvenCount, Item );	// ���� �������߿� �ѹ濡 �� �������� �ִ��� 
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

	// desc : ����Ʈ �κ��丮�� Ư�� �󽽷Կ� �������� ���� (P.S.> api_quest_CheckQuestInvenForAddItemList ���� ����)
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

	// desc : ����Ʈ �κ��丮�� �󽽷Ե鿡 �������� Ư�� ������ŭ ���� (P.S.> api_quest_CheckQuestInvenForAddItemList ���� ����)
	bool __SetBlankQuestInventorySlotCount(TQuestItemCnt pTempQuestInven[QUESTINVENTORYMAX], int nItemID, short wCount, int nOverlapCount, int nCount /* ä�� ���԰��� */)
	{
		DN_ASSERT(NULL != pTempQuestInven,	"Invalid!");
		DN_ASSERT(0 != nItemID,				"Invalid!");
		DN_ASSERT(0 < wCount,				"Invalid!");
		DN_ASSERT(0 < nOverlapCount,		"Invalid!");
		DN_ASSERT(0 < nCount,				"Invalid!");

		// !!! ���� - �� �Լ� ȣ�� ���� üũ�� Ȯ���� �Ǿ����� �����ϰ� �����ϴ� �κи� ����

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

	// desc : ����Ʈ �κ��丮�� Ư�� ���Կ� �������� ��ø ���� (P.S.> api_quest_CheckQuestInvenForAddItemList ���� ����)
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

	// desc : ����Ʈ �κ��丮�� �󽽷��� �� ���� �ִ��� ���� ã�� (P.S.> api_quest_CheckQuestInvenForAddItemList ���� ����)
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

	// desc : ����Ʈ �κ��丮�� �󽽷� ã�� (P.S.> api_quest_CheckQuestInvenForAddItemList ���� ����)
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

	// desc : ����Ʈ �κ��丮�� �� �� �ִ� ������ �� ���� ������ wCount ���� �� �ִ� Index ã�� (P.S.> api_quest_CheckQuestInvenForAddItemList ���� ����)
	int __FindOverlapQuestInventorySlot(TQuestItemCnt pTempQuestInven[QUESTINVENTORYMAX], int nItemID, short wCount)
	{
		DN_ASSERT(NULL != pTempQuestInven,	"Invalid!");

		if ((nItemID <= 0) || (wCount <= 0)) DN_RETURN(-1);

		bool boFlag = false;
		TItemData *pQuestItem = NULL;
		for (int iIndex = 0 ; QUESTINVENTORYMAX > iIndex ; ++iIndex) {
			if (pTempQuestInven[iIndex].nItemID == 0) continue;	// ��ĭ�̸� �н�

			pQuestItem = g_pDataManager->GetItemData(pTempQuestInven[iIndex].nItemID);
			if (!pQuestItem) continue;

			if (pQuestItem->nOverlapCount == 1) continue;				// ��ġ�� ������ �н�
			if (pTempQuestInven[iIndex].nItemID != nItemID) continue;	// ���� �������� �ƴϴ�

			if (pQuestItem->nOverlapCount >= pTempQuestInven[iIndex].wCount + wCount){
				return iIndex;
			}
		}

		return -1;
	}

	// desc : ����Ʈ �κ��丮�� �������� ���� �� �ִ� ������ ������� �˻� (P.S.> api_quest_CheckQuestInvenForAddItemList ���� ����)
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
		if (pItem->nOverlapCount == 1){		// ��ġ�� �ʴ� ������
			nCount = __FindBlankQuestInventorySlotCount(pTempQuestInven);
			if (wCount > nCount) {
				// �󽽷��̶� ���ϱ�
				return false;
			}

			return(__SetBlankQuestInventorySlotCount(pTempQuestInven, nItemID, wCount, 1, nCount));
		}
		else {	// ��ġ�� ������
			int nRemain = 0, nBlank = 0, nBundle = 0;

			int nCount = __FindBlankQuestInventorySlotCount(pTempQuestInven);	// �� ���԰���

			if (pItem->nOverlapCount >= wCount){
				nRemain = wCount;
			}
			else {
				nBundle = wCount / pItem->nOverlapCount;
				nRemain = wCount % pItem->nOverlapCount;

				if (nBundle > nCount) return false;	// ��������
			}

			if (nRemain > 0){
				nBlank = __FindOverlapQuestInventorySlot(pTempQuestInven, nItemID, nRemain);	// ¥������ �� ������ �ִ���
				if (nBlank < 0){	// ¥���� ������ ����
					if ((nBundle + 1) > nCount) return false;	// ��ĭ�� ������ �� �ִ���...
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

		//���� ������, ���� �Ѵ� �ƴϸ� ������ ����.
#if defined (_VILLAGESERVER)
		UINT nUserObjectID = static_cast<CDNUserSession*>(pUser)->GetObjectID();
#endif

#if defined (_GAMESERVER)
		UINT nUserObjectID = static_cast<CDNUserSession*>(pUser)->GetSessionID();
#endif

		bool bSelectedItems[QUESTREWARD_INVENTORYITEMMAX];
		CopyMemory(bSelectedItems, bSelectedArray, sizeof(bSelectedItems));
		if (QuestReward.cType == 1) // ��ü �����̸� �������� ���� �����Ѱ����� ���. (��ü ����� ���� ���� ���� ����.)
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

		// ���� ��繮���� �����س��´�.
		// �������� �� ��Ŷ�� �� ��� ����߿� ������ ������ ���۵Ȱ���.. 
		TALK_PARAGRAPH& para = pUser->GetLastTalkParagraph();
		bool bResult = g_pDataManager->GetTalkParagraph(std::wstring(wszTalkIndex), std::wstring(wszTargetFile), para);
		if ( bResult == false )
		{
#if defined (_VILLAGESERVER)
			if (!g_pDataManager->CheckTalkAnswer(std::wstring(wszTalkIndex), std::wstring(wszTargetFile))) {	// �ƽ� ���� talk_paragraph ���� talk_answer �� �����ϴ� ��찡 ������ �̰��� ��ȿ��
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

		// ���� ��繮���� �����س��´�.
		// �������� �� ��Ŷ�� �� ��� ����߿� ������ ������ ���۵Ȱ���.. 
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
	desc   : �������� ����Ʈ ���� ������ ä������ �����ش�.
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

	// nQuestType : ������� �ʴ� �Ű�������.
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

		// ����Ʈ �Ϸ�
		int nReturn = pUser->GetQuest()->RewardAfterCompletingQuest(nQuestID, bDelPlayList);

		// ������� ���� �ʱ�ȭ
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
			// ����Ʈ ���� ���������� �����ϱ� ���Ͽ� �Ҵ���� �ε����� ����� �� ����
			DN_RETURN(-3);
		}

		if (!CHECK_RANGE(nMemoIndex, 1, QUESTMEMOMAX)) {
			// ����Ʈ �޸� �ε��� ������ ��� (1~10)
			DN_RETURN(-3);
		}

		return pUser->GetQuest()->SetQuestMemo(nQuestID, nMemoIndex-1, iVal);
	}

	int api_quest_GetQuestMemo(CDNUserBase* pUser, int nQuestID, char nMemoIndex)
	{
		if (!pUser)
			return -1;

		if (!CHECK_RANGE(nMemoIndex, 1, QUESTMEMOMAX)) {
			// ����Ʈ �޸� �ε��� ������ ��� (1~10)
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
		// �ϴ��� �ӽ÷� ������� Ȯ���Ѵ�. 
		// �κ��� ������� ������ 
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
			// �ӽ� �κ��丮�� ���� �κ��丮�� ������ ���� �������� ����

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
			// ������ ���̺� �ƹ����� ���� ?
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

			bool bResult = __IsValidSpaceInventorySlotAndSet(TempInven, pUserItem->GetInventoryCount(), Item );		// RandomSeed �� ��� ?
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

		bool bResult = pUserItem->AddInventoryByQuest(nItemIndex, nItemCnt, nQuestID, 0);	// �����ۿ� ���� randomseed �� ��� �������� �������ֽñ�!

#ifndef _FINAL_BUILD
		if ( bResult )
		{
			std::wstring wszMsg;
			wszMsg = FormatW( L"������ ���� : id %d count : %d" , nItemIndex, nItemCnt );
			pUser->SendChat(CHATTYPE_NORMAL, (int)wszMsg.size()*sizeof(WCHAR), L"", (WCHAR*)wszMsg.c_str());
		}
		else
		{
			std::wstring wszMsg;
			wszMsg = FormatW( L"������ ���� ���� : id %d count : %d" , nItemIndex, nItemCnt );
			pUser->SendChat(CHATTYPE_NORMAL, (int)wszMsg.size()*sizeof(WCHAR), L"", (WCHAR*)wszMsg.c_str());
		}
#endif //#ifndef _FINAL_BUILD
#endif //#if defined (_VILLAGESERVER)

#if defined (_GAMESERVER)
		TItem ResultItem;
		if( CDNUserItem::MakeItemStruct(nItemIndex, ResultItem) == false )
			return -2;
		// �ϴ��� �ӽ÷� ������� Ȯ���Ѵ�. 
		// �κ��� ������� ������ 
		if ( !pUserItem->IsValidSpaceInventorySlot(nItemIndex, nItemCnt, ResultItem.bSoulbound, ResultItem.cSealCount, ResultItem.bEternity ) ) 
		{
			return -3;
		}

		bool bResult = pUserItem->AddInventoryByQuest(nItemIndex, nItemCnt, nQuestID, 0);	// �����ۿ� ���� randomseed �� ��� �������� �������ֽñ�!

#if defined( PRE_ENABLE_QUESTCHATLOG )
		if ( bResult )
		{
			// �ӽ÷� ������ ���� �ʰ� ���� ä��â���� ����ش�.
			std::wstring wszMsg;
			wszMsg = FormatW( L"������ ���� : id %d count : %d" , nItemIndex, nItemCnt );
			pUser->SendChat(CHATTYPE_NORMAL, (int)wszMsg.size()*sizeof(WCHAR), L"", (WCHAR*)wszMsg.c_str());
		}
		else
		{
			// �ӽ÷� ������ ���� �ʰ� ���� ä��â���� ����ش�.
			std::wstring wszMsg;
			wszMsg = FormatW( L"������ ���� ���� : id %d count : %d" , nItemIndex, nItemCnt );
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
			wszMsg = FormatW( L"������ ���� : id %d count : %d" , nItemIndex, nItemCnt );
			pUser->SendChat(CHATTYPE_NORMAL, (int)wszMsg.size()*sizeof(WCHAR), L"", (WCHAR*)wszMsg.c_str());
		}
		else
		{
			std::wstring wszMsg;
			wszMsg = FormatW( L"������ ���� ���� : id %d count : %d" , nItemIndex, nItemCnt );
			pUser->SendChat(CHATTYPE_NORMAL, (int)wszMsg.size()*sizeof(WCHAR), L"", (WCHAR*)wszMsg.c_str());
		}
#endif
#endif

#if defined (_GAMESERVER)
#if defined (PRE_ENABLE_QUESTCHATLOG)
		if ( bResult )
		{
			std::wstring wszMsg;
			wszMsg = FormatW( L"������ ���� : id %d count : %d" , nItemIndex, nItemCnt );
			pUser->SendChat(CHATTYPE_NORMAL, (int)wszMsg.size()*sizeof(WCHAR), L"", (WCHAR*)wszMsg.c_str());
		}
		else
		{
			std::wstring wszMsg;
			wszMsg = FormatW( L"������ ���� ���� : id %d count : %d" , nItemIndex, nItemCnt );
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
			wszMsg = FormatW( L"������ ���� : id %d count : %d" , nItemIndex, nCount );
			pUser->SendChat(CHATTYPE_NORMAL, (int)wszMsg.size()*sizeof(WCHAR), L"", (WCHAR*)wszMsg.c_str());
		}
		else
		{
			std::wstring wszMsg;
			wszMsg = FormatW( L"������ ���� ���� : id %d count : %d" , nItemIndex, nCount );
			pUser->SendChat(CHATTYPE_NORMAL, (int)wszMsg.size()*sizeof(WCHAR), L"", (WCHAR*)wszMsg.c_str());
		}
#endif
#endif

#if defined (_GAMESERVER)
#if defined (PRE_ENABLE_QUESTCHATLOG)
		if ( bResult )
		{
			std::wstring wszMsg;
			wszMsg = FormatW( L"������ ���� : id %d count : %d" , nItemIndex, nItemCnt );
			pUser->SendChat(CHATTYPE_NORMAL, (int)wszMsg.size()*sizeof(WCHAR), L"", (WCHAR*)wszMsg.c_str());
		}
		else
		{
			std::wstring wszMsg;
			wszMsg = FormatW( L"������ ���� ���� : id %d count : %d" , nItemIndex, nItemCnt );
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
			wszLog = FormatW( L"������ �÷��� ���� �ε����� �߸��Ǿ����ϴ�. : %d", nCutSceneTableID);
			pUser->SendChat(CHATTYPE_NORMAL, (int)wszLog.size()*sizeof(WCHAR), L"", (WCHAR*)wszLog.c_str());
			return -1;
		}
		std::string szFileName = pSox->GetFieldFromLablePtr( nCutSceneTableID, "_FileName" )->GetString();
		WCHAR wszMsg[512];

		ZeroMemory(&wszMsg, sizeof(WCHAR)*512);
		MultiByteToWideChar(CP_ACP, 0, szFileName.c_str(), -1, wszMsg, 512 );

		std::wstring wszLog;
		wszLog = FormatW( L"������ �÷��� %s : ", wszMsg);
		pUser->SendChat(CHATTYPE_NORMAL, (int)wszLog.size()*sizeof(WCHAR), L"", (WCHAR*)wszLog.c_str());
#endif

		pUser->SendPlayCutScene( nCutSceneTableID,-1, -1, nNpcObjectID, !bIgnoreFadeIn, UINT_MAX );
		pUser->SetCalledNpcResponse(true, false);	// ���� Ŭ���̾�Ʈ�� ������ UI ���� ��û ���� ��ȭ�� �̾����� �ʱ� ������ ������ �ƴѵ��� ���Ǿƴϰ� NPC ���俩�� üũ�� �ɸ��� �ǹǷ� ��ȭ�� ������ ó���� ��

		return 1;
	}
	
	int api_ui_OpenWareHouse(CDNUserBase* pUser, int iItemID/*=0*/ )
	{
		if (!pUser)
			return -1;

#ifndef _FINAL_BUILD
		std::wstring wszLog;
		wszLog = FormatW( L"â����");
		pUser->SendChat(CHATTYPE_NORMAL, (int)wszLog.size()*sizeof(WCHAR), L"", (WCHAR*)wszLog.c_str());
#endif

		pUser->SendShowWarehouse( iItemID );
		pUser->SetCalledNpcResponse(true, false);	// ���� Ŭ���̾�Ʈ�� ������ UI ���� ��û ���� ��ȭ�� �̾����� �ʱ� ������ ������ �ƴѵ��� ���Ǿƴϰ� NPC ���俩�� üũ�� �ɸ��� �ǹǷ� ��ȭ�� ������ ó���� ��

		return 1;
	}

#if defined (PRE_MOD_GAMESERVERSHOP)
	int api_ui_OpenShop(CDNUserBase* pUser, int nShopID, Shop::Type::eCode Type)
	{

#ifndef _FINAL_BUILD
		std::wstring wszLog;
		wszLog = FormatW( L"������");
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
		pUser->SetCalledNpcResponse(true, false);	// ���� Ŭ���̾�Ʈ�� ������ UI ���� ��û ���� ��ȭ�� �̾����� �ʱ� ������ ������ �ƴѵ��� ���Ǿƴϰ� NPC ���俩�� üũ�� �ɸ��� �ǹǷ� ��ȭ�� ������ ó���� ��

		return 1;
	}

	int api_ui_OpenSkillShop(CDNUserBase* pUser)
	{
#ifndef _FINAL_BUILD
		//std::wstring wszLog;
		//wszLog = FormatW( L"���ְ� ����ϴ� ��ų�� ����");
		//pUser->SendChat(CHATTYPE_NORMAL, (int)wszLog.size()*sizeof(WCHAR), L"", (WCHAR*)wszLog.c_str());
#endif

		pUser->SendSkillShopOpen();

		//TSkillShopData *pShopData = g_pDataManager->GetSkillShopData(nSkillShopID);
		//if ( !pShopData )
		//	return -2;
		//
		//pUser->m_nShopID = nSkillShopID;
		//pUser->SendSkillShopList(pShopData);

		pUser->SetCalledNpcResponse(true, false);	// ���� Ŭ���̾�Ʈ�� ������ UI ���� ��û ���� ��ȭ�� �̾����� �ʱ� ������ ������ �ƴѵ��� ���Ǿƴϰ� NPC ���俩�� üũ�� �ɸ��� �ǹǷ� ��ȭ�� ������ ó���� ��

		return 1;
	}
#endif
	
	int api_ui_OpenCompoundEmblem(CDNUserBase* pUser)
	{
		if (!pUser)
			return -1;

#ifndef _FINAL_BUILD
		std::wstring wszLog;
		wszLog = FormatW( L"���庸�� �ռ� â����");
		pUser->SendChat(CHATTYPE_NORMAL, (int)wszLog.size()*sizeof(WCHAR), L"", (WCHAR*)wszLog.c_str());
#endif

		pUser->SendOpenCompoundEmblem();
		pUser->SetCalledNpcResponse(true, false);	// ���� Ŭ���̾�Ʈ�� ������ UI ���� ��û ���� ��ȭ�� �̾����� �ʱ� ������ ������ �ƴѵ��� ���Ǿƴϰ� NPC ���俩�� üũ�� �ɸ��� �ǹǷ� ��ȭ�� ������ ó���� ��

		return 1;
	}
	
	int api_ui_OpenUpgradeJewel(CDNUserBase* pUser)
	{
		if (!pUser)
			return -1;

#ifndef _FINAL_BUILD
		std::wstring wszLog;
		wszLog = FormatW( L"���λ��� ����");
		pUser->SendChat(CHATTYPE_NORMAL, (int)wszLog.size()*sizeof(WCHAR), L"", (WCHAR*)wszLog.c_str());
#endif

		pUser->SendOpenUpdagrageJewel();
		pUser->SetCalledNpcResponse(true, false);	// ���� Ŭ���̾�Ʈ�� ������ UI ���� ��û ���� ��ȭ�� �̾����� �ʱ� ������ ������ �ƴѵ��� ���Ǿƴϰ� NPC ���俩�� üũ�� �ɸ��� �ǹǷ� ��ȭ�� ������ ó���� ��

		return 1;
	}
	
	int api_ui_OpenGlyphLift(CDNUserBase* pUser)
	{
		if ( !pUser ) 
			return -1;

#ifndef _FINAL_BUILD
		std::wstring wszLog;
		wszLog = FormatW( L"����â ����");
		pUser->SendChat(CHATTYPE_NORMAL, (int)wszLog.size()*sizeof(WCHAR), L"", (WCHAR*)wszLog.c_str());
#endif

		pUser->SendOpenGlyphLift();
		pUser->SetCalledNpcResponse(true, false);	// ���� Ŭ���̾�Ʈ�� ������ UI ���� ��û ���� ��ȭ�� �̾����� �ʱ� ������ ������ �ƴѵ��� ���Ǿƴϰ� NPC ���俩�� üũ�� �ɸ��� �ǹǷ� ��ȭ�� ������ ó���� ��

		return 1;
	}

	int api_ui_OpenMailBox(CDNUserBase* pUser)
	{
		if (!pUser)
			return -1;

#ifndef _FINAL_BUILD
		std::wstring wszLog;
		wszLog = FormatW( L"������ ����");
		pUser->SendChat(CHATTYPE_NORMAL, (int)wszLog.size()*sizeof(WCHAR), L"", (WCHAR*)wszLog.c_str());
#endif

		pUser->SendOpenMailBox();
		pUser->SetCalledNpcResponse(true, false);	// ���� Ŭ���̾�Ʈ�� ������ UI ���� ��û ���� ��ȭ�� �̾����� �ʱ� ������ ������ �ƴѵ��� ���Ǿƴϰ� NPC ���俩�� üũ�� �ɸ��� �ǹǷ� ��ȭ�� ������ ó���� ��

		return 1;
	}
	
	int api_ui_OpenDisjointItem(CDNUserBase* pUser)
	{
		if (!pUser)
			return -1;

#if defined (_VILLAGESERVER)
#ifndef _FINAL_BUILD
	wstring wszString = FormatW(L"�������������� ��� ������ ����Դϴ�.\r\n");
	pUser->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );
#endif	// _FINAL_BUILD
#endif
		
#if defined (_GAMESERVER)
#ifndef _FINAL_BUILD
		std::wstring wszLog;
		wszLog = FormatW( L"����â ����");
		pUser->SendChat(CHATTYPE_NORMAL, (int)wszLog.size()*sizeof(WCHAR), L"", (WCHAR*)wszLog.c_str());
#endif
#endif
		pUser->SendOpenDisjointItem();
		pUser->SetCalledNpcResponse(true, false);	// ���� Ŭ���̾�Ʈ�� ������ UI ���� ��û ���� ��ȭ�� �̾����� �ʱ� ������ ������ �ƴѵ��� ���Ǿƴϰ� NPC ���俩�� üũ�� �ɸ��� �ǹǷ� ��ȭ�� ������ ó���� ��

		return 1;
	}
	
	int api_ui_OpenCompoundItem(CDNUserBase* pUser, int nCompoundShopID)
	{
		if (!pUser)
			return -1;

#ifndef _FINAL_BUILD
		std::wstring wszLog;
		wszLog = FormatW( L"������ �ռ�â ����");
		pUser->SendChat(CHATTYPE_NORMAL, (int)wszLog.size()*sizeof(WCHAR), L"", (WCHAR*)wszLog.c_str());
#endif

		pUser->SendOpenCompoundItem(nCompoundShopID);
		pUser->SetCalledNpcResponse(true, false);	// ���� Ŭ���̾�Ʈ�� ������ UI ���� ��û ���� ��ȭ�� �̾����� �ʱ� ������ ������ �ƴѵ��� ���Ǿƴϰ� NPC ���俩�� üũ�� �ɸ��� �ǹǷ� ��ȭ�� ������ ó���� ��

		return 1;
	}

	int api_ui_OpenCashShop(CDNUserBase* pUser)
	{
		if (!pUser)
			return -1;

#ifndef _FINAL_BUILD
		std::wstring wszLog;
		wszLog = FormatW( L"ĳ�� ������ ���� ����");
		pUser->SendChat(CHATTYPE_NORMAL, (int)wszLog.size()*sizeof(WCHAR), L"", (WCHAR*)wszLog.c_str());
#endif

		pUser->SendOpenCashShop();
		pUser->SetCalledNpcResponse(true, false);	// ���� Ŭ���̾�Ʈ�� ������ UI ���� ��û ���� ��ȭ�� �̾����� �ʱ� ������ ������ �ƴѵ��� ���Ǿƴϰ� NPC ���俩�� üũ�� �ɸ��� �ǹǷ� ��ȭ�� ������ ó���� ��

		return 1;
	}
	
	int api_ui_OpenGuildMgrBox(CDNUserBase* pUser, int nGuildMgrNo)
	{
		if (!pUser)
			return -1;

#ifndef _FINAL_BUILD
		std::wstring wszLog;
		wszLog = FormatW( L"��� ���� ��ȭ���� ����");
		pUser->SendChat(CHATTYPE_NORMAL, (int)wszLog.size()*sizeof(WCHAR), L"", (WCHAR*)wszLog.c_str());
#endif

		pUser->SendOpenGuildMgrBox(nGuildMgrNo);
		pUser->SetCalledNpcResponse(true, false);	// ���� Ŭ���̾�Ʈ�� ������ UI ���� ��û ���� ��ȭ�� �̾����� �ʱ� ������ ������ �ƴѵ��� ���Ǿƴϰ� NPC ���俩�� üũ�� �ɸ��� �ǹǷ� ��ȭ�� ������ ó���� ��

		return 1;
	}
	
	int api_ui_OpenGacha_JP(CDNUserBase* pUser, int nGachaShopID)
	{
#ifdef PRE_ADD_GACHA_JAPAN
		if (!pUser)
			return -1;

#ifndef _FINAL_BUILD
		std::wstring wszLog;
		wszLog = FormatW( L"�Ϻ� ��í�� ����");
		pUser->SendChat(CHATTYPE_NORMAL, (int)wszLog.size()*sizeof(WCHAR), L"", (WCHAR*)wszLog.c_str());
#endif

		TGachaponData_JP* pGachaData = g_pDataManager->GetGachaponData_JP( nGachaShopID );
		if( !pGachaData )
			return -2;

		pUser->m_nGachaponShopID = nGachaShopID;
		pUser->SendGachaShopOpen_JP( nGachaShopID );

		// ���� Ŭ���̾�Ʈ�� ������ UI ���� ��û ���� ��ȭ�� �̾����� �ʱ� ������ ������ �ƴѵ��� ���Ǿƴϰ� NPC ���俩�� üũ�� �ɸ��� �ǹǷ� ��ȭ�� ������ ó���� ��
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
		wszLog = FormatW( L"NPC �����ϱ� UI ����");
		pUser->SendChat(CHATTYPE_NORMAL, (int)wszLog.size()*sizeof(WCHAR), L"", (WCHAR*)wszLog.c_str());
#endif

		// Ŭ���̾�Ʈ���� npc ���� �����ϱ� UI ����� ��Ŷ ����.
		pUser->SendOpenGiveNpcPresent( nNpcID );

		// ���� Ŭ���̾�Ʈ�� ������ UI ���� ��û ���� ��ȭ�� �̾����� �ʱ� ������ ������ �ƴѵ��� ���Ǿƴϰ� NPC ���俩�� üũ�� �ɸ��� �ǹǷ� ��ȭ�� ������ ó���� ��
		// �׷����� npc �����ϱ�� ��� ��ȭ�� �̾����Ƿ� �׳� ���д�. �̰� ȣ���ϸ� ��ȭ ���̾�αװ� �ٷ� ������.
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
		// ���̽� ��Ʈ�� ���
#if defined(PRE_ADD_MULTILANGUAGE)
		std::wstring wszBaseString = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nBaseStringIdx, pUser->m_eSelectedLanguage );
#else		//#if defined(PRE_ADD_MULTILANGUAGE)
		std::wstring wszBaseString = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nBaseStringIdx );
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)

		// ���� �Ķ���� ���̺� ���� �ִ��� Ȯ�� �ϰ�
		lua_tinker::table t = ParamTable.get<lua_tinker::table>(1);
		const char* __szKey = t.get<const char*>(1);

		// ������ 
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
		wszMsg = FormatW( L"�ɺ��������߰� %s ID:%d %d��" , bResult ? L"����":L"����", nItemID, (int)wCount);
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
		wszMsg = FormatW( L"�ɺ����������� %s ID:%d %d��" , bResult ? L"����":L"����", nItemID, (int)wCount);
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
		wszMsg = FormatW( L"�ɺ������� ��ȸ UserObjID: %d nItemID:%d %d�� %d�� ����" , nUserObjectID, nItemIndex, (int)nItemCnt, nCount);
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

		// ����Ʈ �κ��丮 ������ ���� ������ üũ�� �����˸� ����
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
			// �ӽ� ����Ʈ �κ��丮�� ���� ����Ʈ �κ��丮�� ������ ���� �������� ����

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
			// ������ ���̺� �ƹ����� ���� ?
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
		wszMsg = FormatW( L"����Ʈ �������߰� %s ID:%d %d��" , bResult ? L"����":L"����", nItemID, (int)wCount);
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
		wszMsg = FormatW( L"����Ʈ ���������� %s ID:%d %d��" , bResult ? L"����":L"����", nItemID, (int)wCount);
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
		wszMsg = FormatW( L"����Ʈ ���������� %s ID:%d %d��" , bResult ? L"����":L"����", nItemID, (int)nCount);
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
		
		//���� ������, ���� �Ѵ� �ƴϸ� ������ ����.
#if defined(_VILLAGESERVER)
		UINT nUserID = static_cast<CDNUserSession*>(pUser)->GetObjectID();
#endif

#if defined(_GAMESERVER)
		UINT nUserID = static_cast<CDNUserSession*>(pUser)->GetSessionID();
#endif
		std::wstring wszMsg;
		wszMsg = FormatW( L"����Ʈ ������ ��ȸ UserObjID: %d nItemID:%d %d�� %d�� ����" , nUserID, nItemIndex, (int)nItemCnt, nCount);
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
		pUser->SetCalledNpcResponse(true, false);	// ���� Ŭ���̾�Ʈ�� ������ UI ���� ��û ���� ��ȭ�� �̾����� �ʱ� ������ ������ �ƴѵ��� ���Ǿƴϰ� NPC ���俩�� üũ�� �ɸ��� �ǹǷ� ��ȭ�� ������ ó���� ��

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

		// ����Ʈ ���� ���̺��� �����ؼ�
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
					wszMsg = FormatW( L"����Ʈ ����ĸ ���� ���̺� ��ȸ ���� ����ƮID:%d Ÿ��:%d Ŭ����ID:%d ", table.nQuestID, pQuest->GetQuestInfo().cQuestType, pUser->GetClassID());
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
			//���� ������, ���� �Ѵ� �ƴϸ� ������ ����.
#if defined (_VILLAGESERVER)
			UINT nUserObjectID = static_cast<CDNUserSession*>(pUser)->GetObjectID();
#endif

#if defined (_GAMESERVER)
			UINT nUserObjectID = static_cast<CDNUserSession*>(pUser)->GetSessionID();
#endif
			std::wstring wszMsg;
			wszMsg = FormatW( L"���� ����UserObjID: %d ���̺�ID:%d ����ƮIDL%d " ,
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

		// �ش� �̼��� ���ų� ��Ȱ��ȭ �����̸� ����
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

		// �ش� �̼��� ���ų� ��Ȱ��ȭ �����̸� ����
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
			// ����Ʈ �޸� �ε��� ������ ��� (1~10)
			DN_RETURN(LONG_MIN);
		}

		if (QUESTMEMOREWARDCHECK == nMemoIndex) {
			// ����Ʈ ���� ���������� �����ϱ� ���Ͽ� �Ҵ���� �ε����� ����� �� ����
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
			// ����Ʈ �޸� �ε��� ������ ��� (1~10)
			DN_RETURN(LONG_MAX);
		}

		if (QUESTMEMOREWARDCHECK == nMemoIndex) {
			// ����Ʈ ���� ���������� �����ϱ� ���Ͽ� �Ҵ���� �ε����� ����� �� ����
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

		// ���� ������ �ܰ谪�� ��Ʈ ������ ����.
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

		// �ٲٱ� ���ϴ� ������ �ܰ谡 ���ų� ū�� Ȯ��.
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
						// �θ� ������ �¾ƾ� ��.
						int iParentJobID = pJobTable->GetFieldFromLablePtr( iItemID, "_ParentJob" )->GetInteger();
						if( iParentJobID == iNowJob )
						{
							pUser->SetUserJob( nJobID );
							bSuccess = true;
							break;
						}
						else
						{
							// �ٲٰ��� �ϴ� ������ �θ� ������ ���� ������ �ƴ�.
#ifndef _FINAL_BUILD
							wstring wszString = FormatW(L"���� �������� ���� �� �� ���� �����Դϴ�.!!\r\n");
							pUser->SendChat(CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str());
#endif // _FINAL_BUILD
							return -3;
						}
					}
					else
					{
						// �ٲٰ����ϴ� ������ �Ʒ� �ܰ���. ���ٲ�.
#ifndef _FINAL_BUILD
						wstring wszString = FormatW(L"���ų� ���� �ܰ��� �������� �ٲ� �� �����ϴ�!!\r\n");
						pUser->SendChat(CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str());
#endif	// _FINAL_BUILD
						return -3;
					}
				}
				else
				{
					// �ٲٰ����ϴ� ������ �ٸ� Ŭ������. ���ٲ�.
#ifndef _FINAL_BUILD
					wstring wszString = FormatW(L"�ٸ� Ŭ������ �������� �ٲ� �� �����ϴ�!!\r\n");
					pUser->SendChat(CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str());
#endif	// _FINAL_BUILD
					return -3;
				}
			}
		}

		if( false == bSuccess )
		{
#ifndef _FINAL_BUILD
			wstring wszString = FormatW(L"�߸��� Job ID �Դϴ�..\r\n");
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
			// ���� �߻�
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

		// Ŭ���̾�Ʈ���� ���õ� ������ ������ ������� ��Ŷ ������.
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
			return 2; //������ ���� �Ϸ�
		else if( nRet == ERROR_ITEM_REPAIR)
			return 0; // ������ ������ ����
		else if( nRet == ERROR_ITEM_INSUFFICIENCY_MONEY)
			return 1; // ������ ����
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
		wszLog = FormatW( L"��ȭ�̵� ����");
		pUser->SendChat(CHATTYPE_NORMAL, (int)wszLog.size()*sizeof(WCHAR), L"", (WCHAR*)wszLog.c_str());
#endif

		pUser->SendOpenExchangeEnchant();
		pUser->SetCalledNpcResponse(true, false);	// ���� Ŭ���̾�Ʈ�� ������ UI ���� ��û ���� ��ȭ�� �̾����� �ʱ� ������ ������ �ƴѵ��� ���Ǿƴϰ� NPC ���俩�� üũ�� �ɸ��� �ǹǷ� ��ȭ�� ������ ó���� ��

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
		
		pUser->SetCalledNpcResponse(true, true); // ���� Ŭ���̾�Ʈ�� ������ UI ���� ��û ���� ��ȭ�� �̾����� �ʱ� ������ ������ �ƴѵ��� ���Ǿƴϰ� NPC ���俩�� üũ�� �ɸ��� �ǹǷ� ��ȭ�� ������ ó���� ��
		pUser->SendOpenTextureDialog(data);
		return 1;
	}

	int api_Close_Texture_Dialog(CDNUserBase* pUser, int nDlgID)
	{
		if(!pUser)
			return -1;

		pUser->SetCalledNpcResponse(true, true); // ���� Ŭ���̾�Ʈ�� ������ UI ���� ��û ���� ��ȭ�� �̾����� �ʱ� ������ ������ �ƴѵ��� ���Ǿƴϰ� NPC ���俩�� üũ�� �ɸ��� �ǹǷ� ��ȭ�� ������ ó���� ��
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

		pUser->SetCalledNpcResponse(true, true); // ���� Ŭ���̾�Ʈ�� ������ UI ���� ��û ���� ��ȭ�� �̾����� �ʱ� ������ ������ �ƴѵ��� ���Ǿƴϰ� NPC ���俩�� üũ�� �ɸ��� �ǹǷ� ��ȭ�� ������ ó���� ��
		pUser->SendOpenChangeJobDialog();
		return 1;
	}
}