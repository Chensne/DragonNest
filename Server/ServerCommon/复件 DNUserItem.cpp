#include "StdAfx.h"
#include "DNUserItem.h"
#include "DNUserSendManager.h"
#include "DNGameDataManager.h"
#include "DNLogConnection.h"
#include "DNDBConnectionManager.h"
#include "Util.h"
#include "DNUserSession.h"
#include "DNDBConnection.h"
#include "DNMissionSystem.h"
#include "DNAuthManager.h"
#include "DNCashRepository.h"
#include "DNCashConnection.h"

#if defined(_VILLAGESERVER)
#include "DNMasterConnection.h"
#include "MtRandom.h"
#include "DNGuildSystem.h"
#include "DNGuildWare.h"
#include "DNGuildVillage.h"
#include "DNGuildWarManager.h"
extern TVillageConfig g_Config;
#elif defined(_GAMESERVER)
#include "DnPlayerActor.h"
#include "DNGameRoom.h"
#include "DNActor.h"
#include "DnActorState.h"
#include "TaskManager.h"
#include "DnSkillTask.h"
#include "DnItemTask.h"
#include "DNMasterConnectionManager.h"
#include "DnPvPGameTask.h"
#include "PvPRespawnLogic.h"
#include "DNGuildSystem.h"
#endif
#include "DNAppellation.h"
#include "DnCostumeMixDataMgr.h"

#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
#include "NpcReputationProcessor.h"
#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )

#include "DNCountryUnicodeSet.h"

#include "TimeSet.h"
#if defined( _VILLAGESERVER )
#include "DNScriptAPI.h"
#elif defined( _GAMESERVER )
#include "DNGameServerScriptAPI.h"
#endif // #if defined( _VILLAGESERVER )
#include "DNPeriodQuestSystem.h"
#if defined (PRE_ADD_BESTFRIEND)
#include "DNBestFriend.h"
#endif
#if defined(PRE_ADD_REMOTE_QUEST)
#include "DNQuestManager.h"
#include "DNQuest.h"
#endif
#if defined(PRE_ADD_WORLD_EVENT)
#include "DNEvent.h"
#endif //#if defined(PRE_ADD_WORLD_EVENT)

CDNUserItem::CDNUserItem(CDNUserSession * pUserSession)
{
	m_pSession = pUserSession;

	memset(m_Equip, 0, sizeof(m_Equip));	
	memset(m_Glyph, 0, sizeof(m_Glyph));
	memset(m_Inventory, 0, sizeof(m_Inventory));
	memset(m_Warehouse, 0, sizeof(m_Warehouse));
	memset(m_QuestInventory, 0, sizeof(m_QuestInventory));

	memset(m_CashEquip, 0, sizeof(m_CashEquip));
	memset(m_UnionMembership, 0, sizeof(m_UnionMembership));
	m_MapCashInventory.clear();

	memset(&m_VehicleEquip, 0, sizeof(m_VehicleEquip));
	memset(&m_PetEquip, 0, sizeof(m_PetEquip));
	m_MapVehicleInventory.clear();

	ClearExchangeData();

	m_RequestType = RequestType_None;
	m_dwRequestTimer = 0;
	m_dwRequestInterval = 0;

#if defined(PRE_ADD_EQUIPLOCK)
	memset(m_EquipLock, 0, sizeof(m_EquipLock));
	memset(m_CashEquipLock, 0, sizeof(m_CashEquipLock));
#endif	// #if defined(PRE_ADD_EQUIPLOCK)

	memset(&m_ReadMailAttachItem, 0, sizeof(m_ReadMailAttachItem));
	m_VecMyMarketList.clear();
#if defined(PRE_SPECIALBOX)
	memset(&m_SpecialBoxInfo, 0, sizeof(m_SpecialBoxInfo));
	memset(&m_SelectSpecialBoxItem, 0, sizeof(m_SelectSpecialBoxItem));
#endif	// #if defined(PRE_SPECIALBOX)

	m_nDailyMailCount = 0;
	m_nWeeklyRegisterCount = m_nRegisterItemCount = 0;

	m_cInventoryCount = DEFAULTINVENTORYMAX;
	m_cWarehouseCount = DEFAULTWAREHOUSEMAX;
	m_iGlyphExtendCount = 0;
	memset(m_TCashGlyphData, 0, sizeof(m_TCashGlyphData));

#if defined(PRE_PERIOD_INVENTORY)
	m_bEnablePeriodInventory = false;
	m_bEnablePeriodWarehouse = false;
	m_tPeriodInventoryExpireDate = 0;
	m_tPeriodWarehouseExpireDate = 0;
#endif	// #if defined(PRE_PERIOD_INVENTORY)

	m_nCashInventoryTotalCount = m_nCashInventoryPage = 0;
	m_nVehicleInventoryTotalCount = 0;

#if defined(PRE_ADD_TALISMAN_SYSTEM)
	memset(m_Talisman, 0, sizeof(m_Talisman));
	m_nTalismanOpenFlag = 0;
	m_bTalismanCashSlotEntend = 0;
	m_tTalismanExpireDate = 0;
#endif

#if defined(_VILLAGESERVER) && defined(PRE_ADD_CASH_REFUND)
	m_MapPaymentItem.clear();
	m_MapPaymentPackageItem.clear();
	m_bCashMoveInven = false;
#endif // #if defined(_VILLAGESERVER) && defined(PRE_ADD_CASH_REFUND)
	m_bMoveItemCheckGameMode = true;

#ifdef PRE_ADD_CHANGEJOB_CASHITEM
	m_nChangeFirstJob = 0;
	m_nChangeSecondJob = 0;
#endif // #ifdef PRE_ADD_CHANGEJOB_CASHITEM
	m_nSkillPageCount = 1;

	memset(&m_Source, 0, sizeof(m_Source));
	m_fSatietyPercent = 0.0f;
#if defined(PRE_MOD_POTENTIAL_JEWEL_RENEWAL)
	memset(&m_PrevPotentialItem, 0, sizeof(m_PrevPotentialItem));
#endif	// #if defined(PRE_MOD_POTENTIAL_JEWEL_RENEWAL)
}

CDNUserItem::~CDNUserItem(void)
{
	m_MapCashInventory.clear();
	m_VecMyMarketList.clear();
	m_MapVehicleInventory.clear();

#if defined(_VILLAGESERVER) && defined(PRE_ADD_CASH_REFUND)
	m_MapPaymentItem.clear();
	m_MapPaymentPackageItem.clear();
#endif //#if defined(_VILLAGESERVER) && defined(PRE_ADD_CASH_REFUND)
}

INT64 CDNUserItem::MakeItemSerial()
{
	return MakeSerial( static_cast<short>(g_pAuthManager->GetServerID()) );
}

INT64 CDNUserItem::MakeCashItemSerial(UINT nDay)
{
	return MakeCashSerial( static_cast<short>(g_pAuthManager->GetServerID()), nDay*86400 );
}

void CDNUserItem::DoUpdate(DWORD CurTick) 
{
	CalcPetSatiety(CurTick);
}

void CDNUserItem::LoadUserData(TASelectCharacter *pData)
{
	std::vector<TItem> VecRemainInven, VecRemainWare, VecRemainPeriodInven, VecRemainPeriodWare;
	VecRemainInven.clear();
	VecRemainWare.clear();
	VecRemainPeriodInven.clear();
	VecRemainPeriodWare.clear();

	std::vector<TItemInfo> VecTempInven;
	VecTempInven.clear();
	bool bChangeFace = false, bChangeHair = false;

#if defined(PRE_ADD_TALISMAN_SYSTEM)
	SetTalismanSlotOpenFlag(pData->nTalismanOpenFlag);
#endif

	for (int i = 0; i < pData->nItemListCount; i++)
	{
#if defined( PRE_ADD_DIRECTNBUFF )
#if defined( _GAMESERVER )		
		int nType = g_pDataManager->GetItemMainType( pData->ItemList[i].ItemInfo.Item.nItemID );

		if(nType == ITEMTYPE_DIRECT_PARTYBUFF)
		{
			if(m_pSession->GetGameRoom())
			{
				m_pSession->GetGameRoom()->SetDirectPartyBuff( pData->ItemList[i].ItemInfo.Item.nItemID );
				m_pSession->SetDirectPartyBuffItem( true, pData->ItemList[i].ItemInfo.Item.nItemID );
			}
		}		
#endif
#endif
		switch(pData->ItemList[i].Code)
		{
		case DBDNWorldDef::ItemLocation::Equip:
			{
				if (IsEquipItem(pData->ItemList[i].ItemInfo.cSlotIndex, pData->ItemList[i].ItemInfo.Item.nItemID) == false){
					if (g_pDataManager->IsFaceParts(pData->ItemList[i].ItemInfo.Item.nItemID)){
						m_Equip[EQUIP_FACE] = pData->ItemList[i].ItemInfo.Item;
						bChangeFace = true;
					}
					else if (g_pDataManager->IsHairParts(pData->ItemList[i].ItemInfo.Item.nItemID)){
						m_Equip[EQUIP_HAIR] = pData->ItemList[i].ItemInfo.Item;
						bChangeHair = true;
					}
					else
						VecTempInven.push_back(pData->ItemList[i].ItemInfo);

					continue;
				}
				if (GetEquip(pData->ItemList[i].ItemInfo.cSlotIndex)){
					VecTempInven.push_back(pData->ItemList[i].ItemInfo);
					continue;
				}

				m_Equip[pData->ItemList[i].ItemInfo.cSlotIndex] = pData->ItemList[i].ItemInfo.Item;
			}
			break;

		case DBDNWorldDef::ItemLocation::Inventory:
			{
				if (m_Inventory[pData->ItemList[i].ItemInfo.cSlotIndex].nItemID > 0){
					VecRemainInven.push_back(pData->ItemList[i].ItemInfo.Item);
					continue;
				}
				if ((pData->ItemList[i].ItemInfo.cSlotIndex < 0) || (pData->ItemList[i].ItemInfo.cSlotIndex >= GetInventoryCount())){
					VecRemainInven.push_back(pData->ItemList[i].ItemInfo.Item);
					continue;
				}
				m_Inventory[pData->ItemList[i].ItemInfo.cSlotIndex] = pData->ItemList[i].ItemInfo.Item;
			}
			break;

		case DBDNWorldDef::ItemLocation::Warehouse:
			{
				if (m_Warehouse[pData->ItemList[i].ItemInfo.cSlotIndex].nItemID > 0){
					VecRemainWare.push_back(pData->ItemList[i].ItemInfo.Item);
					continue;
				}
				if ((pData->ItemList[i].ItemInfo.cSlotIndex < 0) || (pData->ItemList[i].ItemInfo.cSlotIndex >= GetWarehouseCount())){
					VecRemainWare.push_back(pData->ItemList[i].ItemInfo.Item);
					continue;
				}

				m_Warehouse[pData->ItemList[i].ItemInfo.cSlotIndex] = pData->ItemList[i].ItemInfo.Item;
			}
			break;

#if defined(PRE_PERIOD_INVENTORY)
		case DBDNWorldDef::ItemLocation::PeriodInventory:
			{
				int nIndex = pData->ItemList[i].ItemInfo.cSlotIndex;
				if (m_Inventory[nIndex].nItemID > 0){
					VecRemainPeriodInven.push_back(pData->ItemList[i].ItemInfo.Item);
					continue;
				}
				if ((nIndex < INVENTORYMAX) && (nIndex >= INVENTORYTOTALMAX)){
					VecRemainPeriodInven.push_back(pData->ItemList[i].ItemInfo.Item);
					continue;
				}

				m_Inventory[nIndex] = pData->ItemList[i].ItemInfo.Item;
			}
			break;

		case DBDNWorldDef::ItemLocation::PeriodWarehouse:
			{
				int nIndex = pData->ItemList[i].ItemInfo.cSlotIndex;
				if (m_Warehouse[nIndex].nItemID > 0){
					VecRemainPeriodWare.push_back(pData->ItemList[i].ItemInfo.Item);
					continue;
				}
				if ((nIndex < WAREHOUSEMAX) && (nIndex >= WAREHOUSETOTALMAX)){
					VecRemainPeriodWare.push_back(pData->ItemList[i].ItemInfo.Item);
					continue;
				}

				m_Warehouse[nIndex] = pData->ItemList[i].ItemInfo.Item;
			}
			break;
#endif	// #if defined(PRE_PERIOD_INVENTORY)

		case DBDNWorldDef::ItemLocation::QuestInventory:
			m_QuestInventory[pData->ItemList[i].ItemInfo.cSlotIndex].nItemID = pData->ItemList[i].ItemInfo.Item.nItemID;
			m_QuestInventory[pData->ItemList[i].ItemInfo.cSlotIndex].nSerial = pData->ItemList[i].ItemInfo.Item.nSerial;
			m_QuestInventory[pData->ItemList[i].ItemInfo.cSlotIndex].wCount = pData->ItemList[i].ItemInfo.Item.wCount;
			break;

		case DBDNWorldDef::ItemLocation::CashEquip:
			m_CashEquip[pData->ItemList[i].ItemInfo.cSlotIndex] = pData->ItemList[i].ItemInfo.Item;
			break;

		case DBDNWorldDef::ItemLocation::CashInventory:			
			m_MapCashInventory[pData->ItemList[i].ItemInfo.Item.nSerial] = pData->ItemList[i].ItemInfo.Item;			
			break;

		case DBDNWorldDef::ItemLocation::Glyph:
			m_Glyph[pData->ItemList[i].ItemInfo.cSlotIndex] = pData->ItemList[i].ItemInfo.Item;
			break;
#if defined(PRE_ADD_SERVER_WAREHOUSE)
		case DBDNWorldDef::ItemLocation::ServerWare:
			m_MapServerWarehouse[pData->ItemList[i].ItemInfo.Item.nSerial] = pData->ItemList[i].ItemInfo.Item;
			break;
		case DBDNWorldDef::ItemLocation::ServerWareCash:
			m_MapServerWarehouseCash[pData->ItemList[i].ItemInfo.Item.nSerial] = pData->ItemList[i].ItemInfo.Item;
			break;
#endif //#if defined(PRE_ADD_SERVER_WAREHOUSE)
#if defined(PRE_ADD_TALISMAN_SYSTEM)
		case DBDNWorldDef::ItemLocation::Talisman:
			m_Talisman[pData->ItemList[i].ItemInfo.cSlotIndex] = pData->ItemList[i].ItemInfo.Item;
			break;
#endif
		}
	}

	if (!VecRemainInven.empty()){
		std::queue<int> qSlotList;
		int nInvenCount = FindBlankInventorySlotCountList(qSlotList);
		int nRemainInven = (int)VecRemainInven.size();
		if (nInvenCount < nRemainInven)
			nRemainInven = nInvenCount;

		for (int i = 0; i < nRemainInven; i++){
			m_Inventory[qSlotList.front()] = VecRemainInven[i];
			qSlotList.pop();
		}
	}

#if defined(PRE_PERIOD_INVENTORY)
	if (!VecRemainPeriodInven.empty()){
		std::queue<int> qSlotList;
		int nInvenCount = FindBlankInventorySlotCountList(qSlotList);
		int nRemainInven = (int)VecRemainPeriodInven.size();
		if (nInvenCount < nRemainInven)
			nRemainInven = nInvenCount;

		for (int i = 0; i < nRemainInven; i++){
			int nIndex = qSlotList.front();
			m_Inventory[nIndex] = VecRemainPeriodInven[i];

			m_pSession->GetDBConnection()->QueryChangeItemLocation(m_pSession, VecRemainPeriodInven[i].nSerial, 0, VecRemainPeriodInven[i].nItemID, 
				DBDNWorldDef::ItemLocation::Inventory, nIndex, VecRemainPeriodInven[i].wCount, false, 0, false);

			qSlotList.pop();
		}
	}
#endif	// #if defined(PRE_PERIOD_INVENTORY)

	if (!VecTempInven.empty()){
		std::queue<int> qSlotList;
		int nInvenCount = FindBlankInventorySlotCountList(qSlotList);
		int nRemainInven = (int)VecTempInven.size();
		if (nInvenCount < nRemainInven)
			nRemainInven = nInvenCount;

		for (int i = 0; i < nRemainInven; i++){
			int nIndex = qSlotList.front();
			m_Inventory[nIndex] = VecTempInven[i].Item;

			m_pSession->GetDBConnection()->QueryChangeItemLocation(m_pSession, VecTempInven[i].Item.nSerial, 0, VecTempInven[i].Item.nItemID, 
				DBDNWorldDef::ItemLocation::Inventory, nIndex, VecTempInven[i].Item.wCount, false, 0, false);

			qSlotList.pop();
		}
	}

	if (bChangeFace){
		m_pSession->GetDBConnection()->QueryChangeItemLocation(m_pSession, m_Equip[EQUIP_FACE].nSerial, 0, m_Equip[EQUIP_FACE].nItemID, 
			DBDNWorldDef::ItemLocation::Equip, EQUIP_FACE, m_Equip[EQUIP_FACE].wCount, false, 0, false);
	}
	if (bChangeHair){
		m_pSession->GetDBConnection()->QueryChangeItemLocation(m_pSession, m_Equip[EQUIP_HAIR].nSerial, 0, m_Equip[EQUIP_HAIR].nItemID, 
			DBDNWorldDef::ItemLocation::Equip, EQUIP_HAIR, m_Equip[EQUIP_HAIR].wCount, false, 0, false);
	}

	if (!VecRemainWare.empty()){
		std::queue<int> qSlotList;
		int nWareCount = FindBlankWarehouseSlotCountList(qSlotList);
		int nRemainWare = (int)VecRemainWare.size();
		if (nWareCount < nRemainWare)
			nRemainWare = nWareCount;

		for (int i = 0; i < nRemainWare; i++){
			m_Warehouse[qSlotList.front()] = VecRemainWare[i];
			qSlotList.pop();
		}
	}

#if defined(PRE_PERIOD_INVENTORY)
	if (!VecRemainPeriodWare.empty()){
		std::queue<int> qSlotList;
		int nWareCount = FindBlankWarehouseSlotCountList(qSlotList);
		int nRemainWare = (int)VecRemainPeriodWare.size();
		if (nWareCount < nRemainWare)
			nRemainWare = nWareCount;

		for (int i = 0; i < nRemainWare; i++){
			int nIndex = qSlotList.front();
			m_Warehouse[nIndex] = VecRemainPeriodWare[i];

			m_pSession->GetDBConnection()->QueryChangeItemLocation(m_pSession, VecRemainPeriodWare[i].nSerial, 0, VecRemainPeriodWare[i].nItemID, 
				DBDNWorldDef::ItemLocation::Warehouse, nIndex, VecRemainPeriodWare[i].wCount, false, 0, false);

			qSlotList.pop();
		}
	}
#endif	// #if defined(PRE_PERIOD_INVENTORY)

	if ((!VecRemainWare.empty()) || (!VecRemainInven.empty())){
		m_pSession->GetDBConnection()->QuerySaveItemLocationIndex(m_pSession);
	}

#if defined( PRE_ITEMBUFF_COOLTIME )
	__time64_t _tNowTime;
	time(&_tNowTime);	
	int nLogOutSec = 0;
	DWORD nLogOutTick = 0;
	if( m_pSession->GetLastServerType() == SERVERTYPE_LOGIN && _tNowTime > pData->UserData.Status.tLastConnectDate )
	{
		nLogOutSec = (int)(_tNowTime - pData->UserData.Status.tLastConnectDate);
		if( nLogOutSec > ITEMSKILLMAXCOOLTIME )
			nLogOutSec = ITEMSKILLMAXCOOLTIME;

		nLogOutTick = nLogOutSec * 1000;
	}
	
#endif

	DWORD dwTime = timeGetTime();
#if defined(PRE_PERIOD_INVENTORY)
	for (int i = 0; i < INVENTORYTOTALMAX; i++){
		if (i == GetInventoryCount()){
			if (IsEnablePeriodInventory())
				i = INVENTORYMAX;
			else
				break;
		}
#else	// #if defined(PRE_PERIOD_INVENTORY)
	for( int i=0 ; i<INVENTORYMAX ; ++i )
	{
#endif	// #if defined(PRE_PERIOD_INVENTORY)
#if defined( PRE_ITEMBUFF_COOLTIME )	
		if( m_Inventory[i].nCoolTime > (int)nLogOutTick )
			m_Inventory[i].nCoolTime -= nLogOutTick;
		else
		{
			m_Inventory[i].nCoolTime = 0;
			continue;
		}			
#else
		if( m_Inventory[i].nCoolTime <= 0 )
			continue;
#endif

		TItemData* pItemData = g_pDataManager->GetItemData( m_Inventory[i].nItemID );
		if( pItemData )
		{
			m_CoolTime.AddCoolTime( pItemData->nSkillID, m_Inventory[i].nCoolTime+dwTime );
		}
	}

#if defined(PRE_PERIOD_INVENTORY)
	for (int i = 0; i < WAREHOUSETOTALMAX; i++){
		if (i == GetWarehouseCount()){
			if (IsEnablePeriodWarehouse())
				i = WAREHOUSEMAX;
			else
				break;
		}
#else	// #if defined(PRE_PERIOD_INVENTORY)
	for( int i=0 ; i<WAREHOUSEMAX ; ++i )
	{
#endif	// #if defined(PRE_PERIOD_INVENTORY)
#if defined( PRE_ITEMBUFF_COOLTIME )
		if( m_Warehouse[i].nCoolTime > (int)nLogOutTick )
			m_Warehouse[i].nCoolTime -= nLogOutTick;
		else
		{
			m_Warehouse[i].nCoolTime = 0;
			continue;
		}		
#else
		if( m_Warehouse[i].nCoolTime <= 0 )
			continue;
#endif

		TItemData* pItemData = g_pDataManager->GetItemData( m_Warehouse[i].nItemID );
		if( pItemData )
			m_CoolTime.AddCoolTime( pItemData->nSkillID, m_Warehouse[i].nCoolTime+dwTime );
	}

#if defined( PRE_ITEMBUFF_COOLTIME )
	for( TMapItem::iterator itor = m_MapCashInventory.begin(); itor != m_MapCashInventory.end(); itor++ )
	{
		if( itor->second.nCoolTime > (int)nLogOutTick )
			itor->second.nCoolTime -= nLogOutTick;
		else
		{
			itor->second.nCoolTime = 0;
			continue;
		}		

		TItemData* pItemData = g_pDataManager->GetItemData( itor->second.nItemID );
		if( pItemData )
		{
			m_CoolTime.AddCoolTime( pItemData->nSkillID, itor->second.nCoolTime+dwTime );
		}
	}	
	
#if defined(PRE_ADD_SERVER_WAREHOUSE)
	for( TMapItem::iterator itor = m_MapServerWarehouse.begin(); itor != m_MapServerWarehouse.end(); itor++ )
	{
		if( itor->second.nCoolTime > (int)nLogOutTick )
			itor->second.nCoolTime -= nLogOutTick;
		else
		{
			itor->second.nCoolTime = 0;
			continue;
		}

		TItemData* pItemData = g_pDataManager->GetItemData( itor->second.nItemID );
		if( pItemData )
		{
			m_CoolTime.AddCoolTime( pItemData->nSkillID, itor->second.nCoolTime+dwTime );
		}
	}
	
	for( TMapItem::iterator itor = m_MapServerWarehouseCash.begin(); itor != m_MapServerWarehouseCash.end(); itor++ )
	{
		if( itor->second.nCoolTime > (int)nLogOutTick )
			itor->second.nCoolTime -= nLogOutTick;
		else
		{
			itor->second.nCoolTime = 0;
			continue;
		}

		TItemData* pItemData = g_pDataManager->GetItemData( itor->second.nItemID );
		if( pItemData )
		{
			m_CoolTime.AddCoolTime( pItemData->nSkillID, itor->second.nCoolTime+dwTime );
		}
	}	
#endif // #if defined(PRE_ADD_SERVER_WAREHOUSE)
#endif // #if defined( PRE_ITEMBUFF_COOLTIME )

	// 탈것을 타고 있을때만
	if( pData->VehicleEquip.Vehicle[Vehicle::Slot::Body].nItemID > 0 )
	{
		m_VehicleEquip.dwPartsColor1 = pData->VehicleEquip.dwPartsColor1;

		for (int i = 0; i < Vehicle::Slot::Max; i++)
		{
			m_VehicleEquip.Vehicle[i].nItemID = pData->VehicleEquip.Vehicle[i].nItemID;
			m_VehicleEquip.Vehicle[i].wCount = pData->VehicleEquip.Vehicle[i].wCount;
			m_VehicleEquip.Vehicle[i].bEternity = pData->VehicleEquip.Vehicle[i].bEternity;
			m_VehicleEquip.Vehicle[i].nSerial = pData->VehicleEquip.Vehicle[i].nSerial;
			m_VehicleEquip.Vehicle[i].tExpireDate = pData->VehicleEquip.Vehicle[i].tExpireDate;
			m_VehicleEquip.Vehicle[i].bSoulbound = true;
		}
	}

	// 펫을 소환 하고 있을때만
	if( pData->PetEquip.Vehicle[Pet::Slot::Body].nItemID > 0 )
	{
		CTimeSet TimeSet;
		if( !pData->PetEquip.Vehicle[Pet::Slot::Body].bEternity && pData->PetEquip.Vehicle[Pet::Slot::Body].tExpireDate <= TimeSet.GetTimeT64_LC() )
		{
			TItem items[Pet::Slot::Max];
			memset(items, 0, sizeof(items));

			for (int j = 0; j < Pet::Slot::Max; j++)
			{
				if( pData->PetEquip.Vehicle[j].nItemID > 0 )
				{
					items[j].nItemID = pData->PetEquip.Vehicle[j].nItemID;
					items[j].wCount = pData->PetEquip.Vehicle[j].wCount;
					items[j].bEternity = pData->PetEquip.Vehicle[j].bEternity;
					items[j].nSerial = pData->PetEquip.Vehicle[j].nSerial;
					items[j].tExpireDate = pData->PetEquip.Vehicle[j].tExpireDate;
					items[j].bSoulbound = true;
				}				
			}

			// 기간이 만료된 펫은 캐쉬인벤으로 돌려주자..
			// 파츠먼저
			for (int j = Pet::Slot::Accessory1; j < Pet::Slot::Max; j++)
			{
				if( pData->PetEquip.Vehicle[j].nItemID > 0)
				{
					m_pSession->GetDBConnection()->QueryChangeItemLocation(m_pSession, pData->PetEquip.Vehicle[j].nSerial, 0, pData->PetEquip.Vehicle[j].nItemID, 
						DBDNWorldDef::ItemLocation::CashInventory, 0, 1, false, 0, false, true, pData->PetEquip.Vehicle[Pet::Slot::Body].nSerial, j);

					m_pSession->SendMoveCashItem(MoveType_PetPartsToCashInven, j, NULL, items[j].nSerial, &items[j], ERROR_NONE);					
				}
			}
			// 몸통..
			m_pSession->GetDBConnection()->QueryChangeItemLocation(	m_pSession, pData->PetEquip.Vehicle[Pet::Slot::Body].nSerial, 0, pData->PetEquip.Vehicle[Pet::Slot::Body].nItemID, 
				DBDNWorldDef::ItemLocation::PetInventory, 0, m_PetEquip.Vehicle[Pet::Slot::Body].wCount, false, 0, false);						

			m_pSession->SendMoveCashItem(MoveType_PetBodyToPetInven, Pet::Slot::Body, NULL, items[Pet::Slot::Body].nSerial, &items[Pet::Slot::Body], ERROR_NONE);			
#if defined(PRE_ADD_MISSION_COUPON)
			m_pSession->SetExpiredPetID(pData->PetEquip.Vehicle[Pet::Slot::Body].nItemID);
#endif
		}
		else
		{
			_wcscpy(m_PetEquip.wszNickName, NAMELENMAX, pData->PetEquip.wszNickName, NAMELENMAX);
			m_PetEquip.nExp = pData->PetEquip.nExp;
			m_PetEquip.dwPartsColor1 = pData->PetEquip.dwPartsColor1;
			m_PetEquip.dwPartsColor2 = pData->PetEquip.dwPartsColor2;
			m_PetEquip.nSkillID1 = pData->PetEquip.nSkillID1;
			m_PetEquip.nSkillID2 = pData->PetEquip.nSkillID2;
			m_PetEquip.bSkillSlot = pData->PetEquip.bSkillSlot;
			TVehicleData* pVehicleData = g_pDataManager->GetVehicleData(pData->PetEquip.Vehicle[Pet::Slot::Body].nItemID);
			if (pVehicleData)
				m_PetEquip.nRange = pVehicleData->nRange;
			
			m_PetEquip.nCurrentSatiety = pData->PetEquip.nCurrentSatiety;
			if( pVehicleData && pVehicleData->nFoodID > 0 )
			{		
				// 만복도 펫일 경우 만복도 소모량 체크
				TPetFoodData* pPetFoodData = g_pDataManager->GetPetFoodData( pVehicleData->nFoodID );
				if( pPetFoodData && pData->PetEquip.tLastHungerModifyDate > 0 && m_PetEquip.nCurrentSatiety > 0)
				{				
					CTimeSet TimeSet;
					int nSubTime = (int)(TimeSet.GetTimeT64_LC() - pData->PetEquip.tLastHungerModifyDate);
					int nSubSatiety = ((nSubTime/pPetFoodData->nTickTime)*pPetFoodData->nFullTic)/pPetFoodData->cFullTimeLogOut;					
					m_PetEquip.nCurrentSatiety = m_PetEquip.nCurrentSatiety-nSubSatiety;
					if( m_PetEquip.nCurrentSatiety < 0)
						m_PetEquip.nCurrentSatiety = 0;					
				}
				CalcPetSatietyPercent();
				m_PetEquip.nType |= Pet::Type::ePETTYPE_SATIETY;
				m_PetEquip.dwLastHungerTick = timeGetTime();				
			}			

			for (int j = 0; j < Pet::Slot::Max; j++)
			{
				m_PetEquip.Vehicle[j].nItemID = pData->PetEquip.Vehicle[j].nItemID;
				m_PetEquip.Vehicle[j].wCount = pData->PetEquip.Vehicle[j].wCount;
				m_PetEquip.Vehicle[j].bEternity = pData->PetEquip.Vehicle[j].bEternity;
				m_PetEquip.Vehicle[j].nSerial = pData->PetEquip.Vehicle[j].nSerial;
				m_PetEquip.Vehicle[j].tExpireDate = pData->PetEquip.Vehicle[j].tExpireDate;
				m_PetEquip.Vehicle[j].bSoulbound = true;
				
				if( j == Pet::Slot::Body )
				{
					if( !m_PetEquip.Vehicle[j].bEternity )
						m_PetEquip.nType |= Pet::Type::ePETTYPE_EXPIREDATE;					
				}
			}
		}		
	}
}

void CDNUserItem::RemoveInstantItemData( bool bSend )
{
	CDNDBConnection* pDBCon = m_pSession->GetDBConnection();

	// 장착창
	for( UINT i=0 ; i<_countof(m_Equip) ; ++i )
	{
		if( m_Equip[i].nItemID <= 0 )
			continue;
		if( g_pDataManager->bIsInstantItem( m_Equip[i].nItemID ) == true )
		{
			pDBCon->QueryUseItem( m_pSession, DBDNWorldDef::UseItem::Use, m_Equip[i].nSerial, m_Equip[i].wCount, true );
			_PopEquipSlot( i, m_Equip[i].wCount, bSend );
		}
	}

	// 인벤토리
	for( UINT i=0 ; i<_countof(m_Inventory) ; ++i )
	{
		if( m_Inventory[i].nItemID <= 0 )
			continue;
		if( g_pDataManager->bIsInstantItem( m_Inventory[i].nItemID ) == true )
		{
			pDBCon->QueryUseItem( m_pSession, DBDNWorldDef::UseItem::Use, m_Inventory[i].nSerial, m_Inventory[i].wCount, true );
			_PopInventorySlotItem( i, m_Inventory[i].wCount );
			m_pSession->SendRefreshInven( i, &m_Inventory[i] );
		}
	}

	// 퀘스트인벤토리
	for( UINT i=0 ; i<_countof(m_QuestInventory) ; ++i )
	{
		if( m_QuestInventory[i].nItemID <= 0 )
			continue;
		if( g_pDataManager->bIsInstantItem( m_QuestInventory[i].nItemID ) == true )
		{
			pDBCon->QueryUseItem( m_pSession, DBDNWorldDef::UseItem::Use, m_QuestInventory[i].nSerial, m_QuestInventory[i].wCount, true );
			_PopQuestInventorySlot( i, m_QuestInventory[i].wCount );
			m_pSession->SendRefreshQuestInven( i, m_QuestInventory[i] );
		}
	}

	// 문장
	for( UINT i=0 ; i<_countof(m_Glyph) ; ++i )
	{
		if( m_Glyph[i].nItemID <= 0 )
			continue;
		if( g_pDataManager->bIsInstantItem( m_Glyph[i].nItemID ) == true )
		{
			pDBCon->QueryUseItem( m_pSession, DBDNWorldDef::UseItem::Use, m_Glyph[i].nSerial, m_Glyph[i].wCount, true );
			_PopGlyphSlot( i, m_Glyph[i].wCount, bSend );
		}
	}

#if defined(PRE_ADD_TALISMAN_SYSTEM)
	// 탈리스만
	for( UINT i=0 ; i<_countof(m_Talisman) ; ++i )
	{
		if( m_Talisman[i].nItemID <= 0 )
			continue;
		if( g_pDataManager->bIsInstantItem( m_Talisman[i].nItemID ) == true )
		{
			pDBCon->QueryUseItem( m_pSession, DBDNWorldDef::UseItem::Use, m_Talisman[i].nSerial, m_Talisman[i].wCount, true );
			_PopTalismanSlot( i, m_Talisman[i].wCount, bSend );
		}
	}
#endif

	if( m_VehicleEquip.Vehicle[Vehicle::Slot::Body].nItemID > 0 )
	{
		if( g_pDataManager->bIsInstantItem( m_VehicleEquip.Vehicle[Vehicle::Slot::Body].nItemID ) == true )
		{
			if( m_VehicleEquip.Vehicle[Vehicle::Slot::Saddle].nItemID > 0 )
			{
				pDBCon->QueryDelPetEquipment( m_pSession, m_VehicleEquip.Vehicle[Vehicle::Slot::Body].nSerial, m_VehicleEquip.Vehicle[Vehicle::Slot::Saddle].nSerial);
				_PopVehicleEquipSlot( Vehicle::Slot::Saddle, m_VehicleEquip.Vehicle[Vehicle::Slot::Saddle].wCount, bSend );
			}
			if( m_VehicleEquip.Vehicle[Vehicle::Slot::Hair].nItemID > 0 )
			{
				pDBCon->QueryDelPetEquipment( m_pSession, m_VehicleEquip.Vehicle[Vehicle::Slot::Body].nSerial, m_VehicleEquip.Vehicle[Vehicle::Slot::Hair].nSerial);
				_PopVehicleEquipSlot( Vehicle::Slot::Hair, m_VehicleEquip.Vehicle[Vehicle::Slot::Hair].wCount, bSend );
			}

			pDBCon->QueryUseItem( m_pSession, DBDNWorldDef::UseItem::Use, m_VehicleEquip.Vehicle[Vehicle::Slot::Body].nSerial, m_VehicleEquip.Vehicle[Vehicle::Slot::Body].wCount, true );
			_PopVehicleEquipSlot( Vehicle::Slot::Body, m_VehicleEquip.Vehicle[Vehicle::Slot::Body].wCount, bSend );
		}
	}

	if( m_PetEquip.Vehicle[Pet::Slot::Body].nItemID > 0 )
	{
		if( g_pDataManager->bIsInstantItem( m_PetEquip.Vehicle[Pet::Slot::Body].nItemID ) == true )
		{
			if( m_PetEquip.Vehicle[Pet::Slot::Accessory1].nItemID > 0 )
			{
				pDBCon->QueryDelPetEquipment(m_pSession, m_PetEquip.Vehicle[Pet::Slot::Body].nSerial, m_PetEquip.Vehicle[Pet::Slot::Accessory1].nSerial);
				_PopVehicleEquipSlot( Pet::Slot::Accessory1, m_PetEquip.Vehicle[Pet::Slot::Accessory1].wCount, bSend );
			}
			if( m_PetEquip.Vehicle[Pet::Slot::Accessory2].nItemID > 0 )
			{
				pDBCon->QueryDelPetEquipment(m_pSession, m_PetEquip.Vehicle[Pet::Slot::Body].nSerial, m_PetEquip.Vehicle[Pet::Slot::Accessory2].nSerial);
				_PopVehicleEquipSlot( Pet::Slot::Accessory2, m_PetEquip.Vehicle[Pet::Slot::Accessory2].wCount, bSend );
			}

			pDBCon->QueryUseItem( m_pSession, DBDNWorldDef::UseItem::Use, m_PetEquip.Vehicle[Pet::Slot::Body].nSerial, m_PetEquip.Vehicle[Pet::Slot::Body].wCount, true );
			_PopPetEquipSlot( Pet::Slot::Body, m_PetEquip.Vehicle[Pet::Slot::Body].wCount, bSend );
		}
	}
}

void CDNUserItem::RemoveInstantVehicleItemData( bool bSend )
{
	CDNDBConnection* pDBCon = m_pSession->GetDBConnection();

	TMapVehicle Temp = m_MapVehicleInventory;

	for( TMapVehicle::iterator itor=Temp.begin() ; itor!=Temp.end() ; ++itor )
	{
		if( g_pDataManager->bIsInstantItem((*itor).second.Vehicle[Vehicle::Slot::Body].nItemID ) == false )
			continue;

		if( (*itor).second.Vehicle[Vehicle::Slot::Saddle].nItemID > 0 )
		{
			pDBCon->QueryDelPetEquipment(m_pSession, (*itor).second.Vehicle[Vehicle::Slot::Body].nSerial, (*itor).second.Vehicle[Vehicle::Slot::Saddle].nSerial);
		}
		if( (*itor).second.Vehicle[Vehicle::Slot::Hair].nItemID > 0 )
		{
			pDBCon->QueryDelPetEquipment(m_pSession, (*itor).second.Vehicle[Vehicle::Slot::Body].nSerial, (*itor).second.Vehicle[Vehicle::Slot::Hair].nSerial);
		}

		pDBCon->QueryUseItem( m_pSession, DBDNWorldDef::UseItem::Use, (*itor).second.Vehicle[Vehicle::Slot::Body].nSerial, (*itor).second.Vehicle[Vehicle::Slot::Body].wCount, true );
		_PopVehicleInventoryBySerial((*itor).second.Vehicle[Vehicle::Slot::Body].nSerial );
		
		// Refresh
		for( int i=0 ; i<Vehicle::Slot::Max ; ++i )
			(*itor).second.Vehicle[i].wCount = 0;
		if( bSend )
			m_pSession->SendRefreshVehicleInven((*itor).second );

		if( m_nVehicleInventoryTotalCount > 0 )
			--m_nVehicleInventoryTotalCount;
		else
			_ASSERT(0);
	}
}

void CDNUserItem::RemoveInstantEquipVehicleData( bool bSend )
{
	CDNDBConnection* pDBCon = m_pSession->GetDBConnection();

	if( m_VehicleEquip.Vehicle[Vehicle::Slot::Body].nItemID > 0 )
	{
		if( g_pDataManager->bIsInstantItem( m_VehicleEquip.Vehicle[Vehicle::Slot::Body].nItemID ) == true )
		{
			if( m_VehicleEquip.Vehicle[Vehicle::Slot::Saddle].nItemID > 0 )
			{
				pDBCon->QueryDelPetEquipment( m_pSession, m_VehicleEquip.Vehicle[Vehicle::Slot::Body].nSerial, m_VehicleEquip.Vehicle[Vehicle::Slot::Saddle].nSerial);
				_PopVehicleEquipSlot( Vehicle::Slot::Saddle, m_VehicleEquip.Vehicle[Vehicle::Slot::Saddle].wCount, bSend );
			}
			if( m_VehicleEquip.Vehicle[Vehicle::Slot::Hair].nItemID > 0 )
			{
				pDBCon->QueryDelPetEquipment( m_pSession, m_VehicleEquip.Vehicle[Vehicle::Slot::Body].nSerial, m_VehicleEquip.Vehicle[Vehicle::Slot::Hair].nSerial);
				_PopVehicleEquipSlot( Vehicle::Slot::Hair, m_VehicleEquip.Vehicle[Vehicle::Slot::Hair].wCount, bSend );
			}

			pDBCon->QueryUseItem( m_pSession, DBDNWorldDef::UseItem::Use, m_VehicleEquip.Vehicle[Vehicle::Slot::Body].nSerial, m_VehicleEquip.Vehicle[Vehicle::Slot::Body].wCount, true );
			_PopVehicleEquipSlot( Vehicle::Slot::Body, m_VehicleEquip.Vehicle[Vehicle::Slot::Body].wCount, bSend );
		}
	}
}

void CDNUserItem::RemoveGuildReversionItem( bool bSend )
{
	CDNDBConnection* pDBCon = m_pSession->GetDBConnection();
	BYTE cDBThreadID = m_pSession->GetDBThreadID();
	int iWorldSetID = m_pSession->GetWorldSetID();
	UINT uiAccountDBID = m_pSession->GetAccountDBID();
	int iChannelID = m_pSession->GetChannelID();
	int iMapID = m_pSession->GetMapIndex();
	int nDeleteCount = 0;

	// 장착창
	for( UINT i=0 ; i<_countof(m_Equip) ; ++i )
	{
		if( m_Equip[i].nItemID <= 0 )
			continue;
		if( g_pDataManager->bIsGuildReversionItem( m_Equip[i].nItemID ) == true )
		{
			pDBCon->QueryUseItem( m_pSession, DBDNWorldDef::UseItem::GuildReversionItem, m_Equip[i].nSerial, m_Equip[i].wCount, true );
			_PopEquipSlot( i, m_Equip[i].wCount, bSend );
			nDeleteCount++;
		}
	}

	// 캐쉬장착창
	for( UINT i=0 ; i<_countof(m_CashEquip) ; ++i )
	{
		if( m_CashEquip[i].nItemID <= 0 )
			continue;
		if( g_pDataManager->bIsGuildReversionItem( m_CashEquip[i].nItemID ) == true )
		{
			pDBCon->QueryUseItem( m_pSession, DBDNWorldDef::UseItem::GuildReversionItem, m_CashEquip[i].nSerial, m_CashEquip[i].wCount, true );
			_PopCashEquipSlot( i, m_CashEquip[i].wCount );			
			nDeleteCount++;
		}
	}

	if( nDeleteCount > 0 )
	{
		m_pSession->SendEquipList( this );
		nDeleteCount = 0;
	}

	// 인벤토리
	for( UINT i=0 ; i<_countof(m_Inventory) ; ++i )
	{
		if( m_Inventory[i].nItemID <= 0 )
			continue;
		if( g_pDataManager->bIsGuildReversionItem( m_Inventory[i].nItemID ) == true )
		{
			pDBCon->QueryUseItem( m_pSession, DBDNWorldDef::UseItem::GuildReversionItem, m_Inventory[i].nSerial, m_Inventory[i].wCount, true );
			_PopInventorySlotItem( i, m_Inventory[i].wCount );
			m_pSession->SendRefreshInven( i, &m_Inventory[i] );
		}
	}

	// 창고
	for( UINT i=0 ; i<_countof(m_Warehouse) ; ++i )
	{
		if( m_Warehouse[i].nItemID <= 0 )
			continue;
		if( g_pDataManager->bIsGuildReversionItem( m_Warehouse[i].nItemID ) == true )
		{
			pDBCon->QueryUseItem( m_pSession, DBDNWorldDef::UseItem::GuildReversionItem, m_Warehouse[i].nSerial, m_Warehouse[i].wCount, true );
			_PopWarehouseSlot( i, m_Warehouse[i].wCount );
			m_pSession->SendRefreshWarehouse( i, &m_Warehouse[i] );
		}
	}

	// 퀘스트인벤토리
	for( UINT i=0 ; i<_countof(m_QuestInventory) ; ++i )
	{
		if( m_QuestInventory[i].nItemID <= 0 )
			continue;
		if( g_pDataManager->bIsGuildReversionItem( m_QuestInventory[i].nItemID ) == true )
		{
			pDBCon->QueryUseItem( m_pSession, DBDNWorldDef::UseItem::GuildReversionItem, m_QuestInventory[i].nSerial, m_QuestInventory[i].wCount, true );
			_PopQuestInventorySlot( i, m_QuestInventory[i].wCount );
			m_pSession->SendRefreshQuestInven( i, m_QuestInventory[i] );
		}
	}

	// 캐쉬 인벤
	std::vector<TItem> VecItemList;
	VecItemList.clear();
	nDeleteCount = 0;
	for( TMapItem::iterator itor = m_MapCashInventory.begin(); itor != m_MapCashInventory.end(); )
	{
		if( g_pDataManager->bIsGuildReversionItem( itor->second.nItemID ) == true )
		{
			pDBCon->QueryUseItem( m_pSession, DBDNWorldDef::UseItem::GuildReversionItem, itor->second.nSerial, itor->second.wCount, true );
			TItem TempItem = {0,};
			TempItem.wCount = 0;
			TempItem.nSerial = itor->second.nSerial;
			nDeleteCount++;
			VecItemList.push_back(TempItem);
			if( nDeleteCount >= MAXDELEXPIREITEM )
			{				
				m_pSession->SendRefreshCashInven(VecItemList, false);
				nDeleteCount = 0;
				VecItemList.clear();
			}			
			itor = m_MapCashInventory.erase(itor);			
		}
		else
			itor++;
	}	
	if( nDeleteCount > 0 )	
	{
		m_pSession->SendRefreshCashInven(VecItemList, false);
		nDeleteCount = 0;
	}

	// 문장
	for( UINT i=0 ; i<_countof(m_Glyph) ; ++i )
	{
		if( m_Glyph[i].nItemID <= 0 )
			continue;
		if( g_pDataManager->bIsGuildReversionItem( m_Glyph[i].nItemID ) == true )
		{
			pDBCon->QueryUseItem( m_pSession, DBDNWorldDef::UseItem::GuildReversionItem, m_Glyph[i].nSerial, m_Glyph[i].wCount, true );
			_PopGlyphSlot( i, m_Glyph[i].wCount, bSend );
		}
	}

#if defined(PRE_ADD_TALISMAN_SYSTEM)
	// 탈리스만
	for( UINT i=0 ; i<_countof(m_Talisman) ; ++i )
	{
		if( m_Talisman[i].nItemID <= 0 )
			continue;
		if( g_pDataManager->bIsGuildReversionItem( m_Talisman[i].nItemID ) == true )
		{
			pDBCon->QueryUseItem( m_pSession, DBDNWorldDef::UseItem::GuildReversionItem, m_Talisman[i].nSerial, m_Talisman[i].wCount, true );
			_PopTalismanSlot( i, m_Talisman[i].wCount, bSend );
		}
	}
#endif

	if( m_VehicleEquip.Vehicle[Vehicle::Slot::Body].nItemID > 0 )
	{
		if( g_pDataManager->bIsGuildReversionItem( m_VehicleEquip.Vehicle[Vehicle::Slot::Body].nItemID ) == true )
		{
			if( m_VehicleEquip.Vehicle[Vehicle::Slot::Saddle].nItemID > 0 )
			{
				pDBCon->QueryDelPetEquipment(m_pSession, m_VehicleEquip.Vehicle[Vehicle::Slot::Body].nSerial, m_VehicleEquip.Vehicle[Vehicle::Slot::Saddle].nSerial);
				_PopVehicleEquipSlot( Vehicle::Slot::Saddle, m_VehicleEquip.Vehicle[Vehicle::Slot::Saddle].wCount, bSend );
			}
			if( m_VehicleEquip.Vehicle[Vehicle::Slot::Hair].nItemID > 0 )
			{
				pDBCon->QueryDelPetEquipment(m_pSession, m_VehicleEquip.Vehicle[Vehicle::Slot::Body].nSerial, m_VehicleEquip.Vehicle[Vehicle::Slot::Hair].nSerial);
				_PopVehicleEquipSlot( Vehicle::Slot::Hair, m_VehicleEquip.Vehicle[Vehicle::Slot::Hair].wCount, bSend );
			}

			pDBCon->QueryUseItem( m_pSession, DBDNWorldDef::UseItem::Use, m_VehicleEquip.Vehicle[Vehicle::Slot::Body].nSerial, m_VehicleEquip.Vehicle[Vehicle::Slot::Body].wCount, true );
			_PopVehicleEquipSlot( Vehicle::Slot::Body, m_VehicleEquip.Vehicle[Vehicle::Slot::Body].wCount, bSend );
		}
	}

	if( m_PetEquip.Vehicle[Pet::Slot::Body].nItemID > 0 )
	{
		if( g_pDataManager->bIsGuildReversionItem( m_PetEquip.Vehicle[Pet::Slot::Body].nItemID ) == true )
		{
			TItem items[Pet::Slot::Max];
			memset(items, 0, sizeof(items));

			for (int i = 0; i < Pet::Slot::Max; i++)
			{
				if( m_PetEquip.Vehicle[i].nItemID > 0 )
				{
					items[i].nItemID = m_PetEquip.Vehicle[i].nItemID;
					items[i].wCount = m_PetEquip.Vehicle[i].wCount;
					items[i].bEternity = m_PetEquip.Vehicle[i].bEternity;
					items[i].nSerial = m_PetEquip.Vehicle[i].nSerial;
					items[i].tExpireDate = m_PetEquip.Vehicle[i].tExpireDate;
					items[i].bSoulbound = true;
				}				
			}

			for (int i = Pet::Slot::Accessory1; i < Pet::Slot::Max; i++)
			{
				if( m_PetEquip.Vehicle[i].nItemID > 0 )
				{
					if( g_pDataManager->bIsGuildReversionItem(m_PetEquip.Vehicle[i].nItemID) )
					{						
						//귀속된 아이템의 경우에는 같이 지워준다.
						pDBCon->QueryDelPetEquipment(m_pSession, m_PetEquip.Vehicle[Pet::Slot::Body].nSerial, m_PetEquip.Vehicle[i].nSerial);
						_PopVehicleEquipSlot( i, m_PetEquip.Vehicle[i].wCount, bSend );
					}
					else
					{
						//귀속되지 않은 아이템은 캐쉬인벤으로 다시 돌려준다.
						m_pSession->GetDBConnection()->QueryChangeItemLocation(m_pSession, m_PetEquip.Vehicle[i].nSerial, 0, m_PetEquip.Vehicle[i].nItemID, 
							DBDNWorldDef::ItemLocation::CashInventory, 0, 1, false, 0, false, true, m_PetEquip.Vehicle[Pet::Slot::Body].nSerial, i);

						m_pSession->SendMoveCashItem(MoveType_PetPartsToCashInven, i, NULL, items[i].nSerial, &items[i], ERROR_NONE);											
					}					
				}				
			}			

			pDBCon->QueryUseItem( m_pSession, DBDNWorldDef::UseItem::Use, m_PetEquip.Vehicle[Pet::Slot::Body].nSerial, m_PetEquip.Vehicle[Pet::Slot::Body].wCount, true );
			_PopPetEquipSlot( Pet::Slot::Body, m_PetEquip.Vehicle[Pet::Slot::Body].wCount, bSend );
		}
	}

	//제스처아이템 삭제(탈퇴시 DB에서 자동으로 삭제되므로 메모리상으로만 처리)
	if( m_pSession->GetEffectRepository() )
	{
		m_pSession->GetEffectRepository()->DelGuildRewardItem();
		m_pSession->SendEffectItemGestureList();
	}

	// 칭호
	TAppellationGroup *pAppellation = m_pSession->GetAppellationData();	
	std::map<int, int> mapDelAppellation;	//first에 ArrayIndex가 들어옴
	mapDelAppellation.clear();
	g_pDataManager->ResetGuildAppellation(pAppellation->Appellation, mapDelAppellation);	//캐쉬 칭호 초기화
	for( std::map<int, int>::iterator itor = mapDelAppellation.begin(); itor != mapDelAppellation.end(); itor++ )
		m_pSession->GetAppellation()->SendDelAppellation(itor->first);	//칭호 삭제
	if(m_pSession->GetAppellation()->CheckAppellation())
	{
		m_pSession->SendAppellationList(pAppellation->Appellation);
		m_pSession->GetAppellation()->OnRecvSelectAppellation( pAppellation->nSelectAppellation, pAppellation->nCoverAppellation );	
	}	
}

void CDNUserItem::RemoveGuildReversionVehicleItemData( bool bSend )
{
	CDNDBConnection* pDBCon = m_pSession->GetDBConnection();
	BYTE cDBThreadID = m_pSession->GetDBThreadID();
	int iWorldSetID	= m_pSession->GetWorldSetID();
	UINT uiAccountDBID = m_pSession->GetAccountDBID();
	int iChannelID = m_pSession->GetChannelID();
	int iMapID = m_pSession->GetMapIndex();

	TMapVehicle Temp = m_MapVehicleInventory;

	for( TMapVehicle::iterator itor=Temp.begin() ; itor!=Temp.end() ; ++itor )
	{
		if( g_pDataManager->bIsGuildReversionItem((*itor).second.Vehicle[Vehicle::Slot::Body].nItemID ) == false )
			continue;

		TItem items[Pet::Slot::Max];
		memset(items, 0, sizeof(items));

		for (int i = 0; i < Pet::Slot::Max; i++)
		{
			if( (*itor).second.Vehicle[i].nItemID > 0 )
			{
				items[i].nItemID = (*itor).second.Vehicle[i].nItemID;
				items[i].wCount = (*itor).second.Vehicle[i].wCount;
				items[i].bEternity = (*itor).second.Vehicle[i].bEternity;
				items[i].nSerial = (*itor).second.Vehicle[i].nSerial;
				items[i].tExpireDate = (*itor).second.Vehicle[i].tExpireDate;
				items[i].bSoulbound = true;
			}				
		}

		for(int i = Vehicle::Slot::Saddle; i <= Vehicle::Slot::Hair; i++)
		{
			if((*itor).second.Vehicle[i].nItemID > 0)
			{
				if( g_pDataManager->bIsGuildReversionItem((*itor).second.Vehicle[i].nItemID ) )
				{
					//귀속된 아이템의 경우에는 같이 지워준다.
					pDBCon->QueryDelPetEquipment(m_pSession, (*itor).second.Vehicle[Pet::Slot::Body].nSerial, (*itor).second.Vehicle[i].nSerial);
				}
				else
				{
					//귀속되지 않은 아이템은 캐쉬인벤으로 다시 돌려준다.
					m_pSession->GetDBConnection()->QueryChangeItemLocation(m_pSession, (*itor).second.Vehicle[i].nSerial, 0, (*itor).second.Vehicle[i].nItemID, 
						DBDNWorldDef::ItemLocation::CashInventory, 0, 1, false, 0, false, true, (*itor).second.Vehicle[Pet::Slot::Body].nSerial, i);

					m_pSession->SendMoveCashItem(MoveType_PetPartsToCashInven, i, NULL, items[i].nSerial, &items[i], ERROR_NONE);
				}
			}
		}

		pDBCon->QueryUseItem( m_pSession, DBDNWorldDef::UseItem::Use, (*itor).second.Vehicle[Vehicle::Slot::Body].nSerial, (*itor).second.Vehicle[Vehicle::Slot::Body].wCount, true );
		_PopVehicleInventoryBySerial((*itor).second.Vehicle[Vehicle::Slot::Body].nSerial );

		// Refresh
		for( int i=0 ; i<Vehicle::Slot::Max ; ++i )
			(*itor).second.Vehicle[i].wCount = 0;
		if( bSend )
			m_pSession->SendRefreshVehicleInven((*itor).second );

		if( m_nVehicleInventoryTotalCount > 0 )
			--m_nVehicleInventoryTotalCount;
		else
			_ASSERT(0);
	}
}

void CDNUserItem::SaveUserData()
{
	DWORD dwCompareTime = timeGetTime();
	
#if defined(PRE_PERIOD_INVENTORY)
	for (int i = 0; i < INVENTORYTOTALMAX; i++){
		if (i == GetInventoryCount()){
			if (IsEnablePeriodInventory())
				i = INVENTORYMAX;
			else
				break;
		}
#else	// #if defined(PRE_PERIOD_INVENTORY)
	for( int i=0 ; i<GetInventoryCount() ; ++i )
        {
#endif	// #if defined(PRE_PERIOD_INVENTORY)
		_UpdateInventoryCoolTime( i, dwCompareTime );
	}
#if defined(PRE_PERIOD_INVENTORY)
	for (int i = 0; i < WAREHOUSETOTALMAX; i++){
		if (i == GetWarehouseCount()){
			if (IsEnablePeriodWarehouse())
				i = WAREHOUSEMAX;
			else
				break;
		}
#else	// #if defined(PRE_PERIOD_INVENTORY)
	for (int i = 0; i < GetWarehouseCount(); i++){
#endif	// #if defined(PRE_PERIOD_INVENTORY)
		_UpdateWarehouseCoolTime( i, dwCompareTime );
	}

	// 여기서 만복도 저장
	if( m_PetEquip.Vehicle[Pet::Slot::Body].nItemID > 0 && m_PetEquip.nType & Pet::Type::ePETTYPE_SATIETY )
	{
		m_pSession->GetDBConnection()->QueryModDegreeOfHunger(m_pSession->GetDBThreadID(),m_pSession->GetWorldSetID(), m_pSession->GetAccountDBID(),
			m_PetEquip.Vehicle[Pet::Slot::Body].nSerial, m_PetEquip.nCurrentSatiety );
	}
	TMapVehicle::const_iterator iter = m_MapVehicleInventory.begin();	
	for(;iter != m_MapVehicleInventory.end();iter++)
	{
		if( iter->second.nType & Pet::Type::ePETTYPE_SATIETY)
		{
			m_pSession->GetDBConnection()->QueryModDegreeOfHunger(m_pSession->GetDBThreadID(),m_pSession->GetWorldSetID(), m_pSession->GetAccountDBID(),
				iter->second.Vehicle[Pet::Slot::Body].nSerial, iter->second.nCurrentSatiety );
		}		
	}
}

void CDNUserItem::ResetCoolTime()
{
	m_CoolTime.Clear();
}

int CDNUserItem::_SwapEquipToEquip(int nSrcIndex, int nDestIndex)
{
	if (!_CheckRangeEquipIndex(nSrcIndex)) return ERROR_ITEM_INDEX_UNMATCH;
	if (!_CheckRangeEquipIndex(nDestIndex)) return ERROR_ITEM_INDEX_UNMATCH;

	// 서로 바꿔도 되는지 일단 장착 검사해보쟈
	if (m_Equip[nSrcIndex].nItemID > 0)
	{
		if (!IsEquipItem(nDestIndex, m_Equip[nSrcIndex].nItemID)) return ERROR_ITEM_EQUIPMISMATCH;
	}

	if (m_Equip[nDestIndex].nItemID > 0)
	{
		if (!IsEquipItem(nSrcIndex, m_Equip[nDestIndex].nItemID)) return ERROR_ITEM_EQUIPMISMATCH;
	}

	if ((m_Equip[nSrcIndex].nItemID > 0) && (m_Equip[nDestIndex].nItemID > 0)){
		m_pSession->GetDBConnection()->QuerySwitchItemLocation(m_pSession, DBDNWorldDef::ItemLocation::Equip, 
			nSrcIndex, m_Equip[nDestIndex].nSerial, DBDNWorldDef::ItemLocation::Equip, nDestIndex, m_Equip[nSrcIndex].nSerial);
	}
	else if (m_Equip[nSrcIndex].nItemID > 0){
		m_pSession->GetDBConnection()->QueryChangeItemLocation(m_pSession, m_Equip[nSrcIndex].nSerial, 0, m_Equip[nSrcIndex].nItemID, 
			DBDNWorldDef::ItemLocation::Equip, nDestIndex, m_Equip[nSrcIndex].wCount, false, 0, false);
	}
	else if (m_Equip[nDestIndex].nItemID > 0){
		m_pSession->GetDBConnection()->QueryChangeItemLocation(m_pSession, m_Equip[nDestIndex].nSerial, 0, m_Equip[nDestIndex].nItemID, 
			DBDNWorldDef::ItemLocation::Equip, nSrcIndex, m_Equip[nDestIndex].wCount, false, 0, false);
	}

	TItem SrcEquip = m_Equip[nSrcIndex];

	m_Equip[nSrcIndex] = m_Equip[nDestIndex];
	m_Equip[nDestIndex] = SrcEquip;

	BroadcastChangeEquip(nSrcIndex, m_Equip[nSrcIndex]);
	BroadcastChangeEquip(nDestIndex, m_Equip[nDestIndex]);

	m_pSession->GetEventSystem()->OnEvent( EventSystem::OnItemEquip, 1, EventSystem::ItemID, m_Equip[nDestIndex].nItemID );

	char cLevel = 99;
	for (int i = EQUIP_HELMET; i <= EQUIP_FOOT; i++){
		if (cLevel > m_Equip[i].cLevel) cLevel = m_Equip[i].cLevel;
		if (cLevel == 0) break;
	}
	if (cLevel > 0) {
		m_pSession->GetEventSystem()->OnEvent( EventSystem::OnItemEquip, 1,
			EventSystem::AllPartsMinLevel, cLevel );
	}

	return ERROR_NONE;
}

int CDNUserItem::_SwapInvenToInven(int nSrcIndex, int nDestIndex)
{
	if (!_CheckRangeInventoryIndex(nSrcIndex)) return ERROR_ITEM_INDEX_UNMATCH;
	if (!_CheckRangeInventoryIndex(nDestIndex)) return ERROR_ITEM_INDEX_UNMATCH;

#if defined(PRE_PERIOD_INVENTORY)
	char cSrcInventoryLocationCode = DBDNWorldDef::ItemLocation::Inventory;
	if (_CheckRangePeriodInventoryIndex(nSrcIndex))
		cSrcInventoryLocationCode = DBDNWorldDef::ItemLocation::PeriodInventory;

	char cDestInventoryLocationCode = DBDNWorldDef::ItemLocation::Inventory;
	if (_CheckRangePeriodInventoryIndex(nDestIndex))
		cDestInventoryLocationCode = DBDNWorldDef::ItemLocation::PeriodInventory;
	
	if (cSrcInventoryLocationCode != cDestInventoryLocationCode){
		if ((m_Inventory[nSrcIndex].nItemID > 0) && (m_Inventory[nDestIndex].nItemID > 0)){
			m_pSession->GetDBConnection()->QuerySwitchItemLocation(m_pSession, cSrcInventoryLocationCode, 
				nSrcIndex, m_Inventory[nDestIndex].nSerial, cDestInventoryLocationCode, nDestIndex, m_Inventory[nSrcIndex].nSerial);
		}
		else if (m_Inventory[nSrcIndex].nItemID > 0){
			m_pSession->GetDBConnection()->QueryChangeItemLocation(m_pSession, m_Inventory[nSrcIndex].nSerial, 0, m_Inventory[nSrcIndex].nItemID, 
				cDestInventoryLocationCode, nDestIndex, m_Inventory[nSrcIndex].wCount, false, 0, false);
		}
		else if (m_Inventory[nDestIndex].nItemID > 0){
			m_pSession->GetDBConnection()->QueryChangeItemLocation(m_pSession, m_Inventory[nDestIndex].nSerial, 0, m_Inventory[nDestIndex].nItemID, 
				cSrcInventoryLocationCode, nSrcIndex, m_Inventory[nDestIndex].wCount, false, 0, false);
		}
	}
#endif	// #if defined(PRE_PERIOD_INVENTORY)

	TItem SrcInven = m_Inventory[nSrcIndex];

	m_Inventory[nSrcIndex] = m_Inventory[nDestIndex];
	_UpdateInventoryCoolTime(nSrcIndex);

	m_Inventory[nDestIndex] = SrcInven;
	_UpdateInventoryCoolTime(nDestIndex);

	return ERROR_NONE;
}

int CDNUserItem::_SwapWareToWare(int nSrcIndex, int nDestIndex)
{
	if (!_CheckRangeWarehouseIndex(nSrcIndex)) return ERROR_ITEM_INDEX_UNMATCH;
	if (!_CheckRangeWarehouseIndex(nDestIndex)) return ERROR_ITEM_INDEX_UNMATCH;

#if defined(PRE_PERIOD_INVENTORY)
	char cSrcInventoryLocationCode = DBDNWorldDef::ItemLocation::Warehouse;
	if (_CheckRangePeriodInventoryIndex(nSrcIndex))
		cSrcInventoryLocationCode = DBDNWorldDef::ItemLocation::PeriodWarehouse;

	char cDestInventoryLocationCode = DBDNWorldDef::ItemLocation::Warehouse;
	if (_CheckRangePeriodInventoryIndex(nDestIndex))
		cDestInventoryLocationCode = DBDNWorldDef::ItemLocation::PeriodWarehouse;

	if (cSrcInventoryLocationCode != cDestInventoryLocationCode){
		if ((m_Warehouse[nSrcIndex].nItemID > 0) && (m_Warehouse[nDestIndex].nItemID > 0)){
			m_pSession->GetDBConnection()->QuerySwitchItemLocation(m_pSession, cSrcInventoryLocationCode, 
				nSrcIndex, m_Warehouse[nDestIndex].nSerial, cDestInventoryLocationCode, nDestIndex, m_Warehouse[nSrcIndex].nSerial);
		}
		else if (m_Warehouse[nSrcIndex].nItemID > 0){
			m_pSession->GetDBConnection()->QueryChangeItemLocation(m_pSession, m_Warehouse[nSrcIndex].nSerial, 0, m_Warehouse[nSrcIndex].nItemID, 
				cDestInventoryLocationCode, nDestIndex, m_Warehouse[nSrcIndex].wCount, false, 0, false);
		}
		else if (m_Warehouse[nDestIndex].nItemID > 0){
			m_pSession->GetDBConnection()->QueryChangeItemLocation(m_pSession, m_Warehouse[nDestIndex].nSerial, 0, m_Warehouse[nDestIndex].nItemID, 
				cSrcInventoryLocationCode, nSrcIndex, m_Warehouse[nDestIndex].wCount, false, 0, false);
		}
	}
#endif	// #if defined(PRE_PERIOD_INVENTORY)

	TItem SrcWare = m_Warehouse[nSrcIndex];

	m_Warehouse[nSrcIndex] = m_Warehouse[nDestIndex];
	_UpdateWarehouseCoolTime(nSrcIndex);

	m_Warehouse[nDestIndex] = SrcWare;
	_UpdateWarehouseCoolTime(nDestIndex);

	return ERROR_NONE;
}

int CDNUserItem::_SwapQuestInvenToQuestInven(int nSrcIndex, int nDestIndex)
{
	if (!_CheckRangeQuestInventoryIndex(nSrcIndex)) return ERROR_ITEM_INDEX_UNMATCH;
	if (!_CheckRangeQuestInventoryIndex(nDestIndex)) return ERROR_ITEM_INDEX_UNMATCH;

	if ((m_QuestInventory[nSrcIndex].nItemID > 0) && (m_QuestInventory[nDestIndex].nItemID > 0)){
		m_pSession->GetDBConnection()->QuerySwitchItemLocation(m_pSession, DBDNWorldDef::ItemLocation::QuestInventory, 
			0, m_QuestInventory[nDestIndex].nSerial, DBDNWorldDef::ItemLocation::QuestInventory, 0, m_QuestInventory[nSrcIndex].nSerial);
	}
	else if (m_QuestInventory[nSrcIndex].nItemID > 0){
		m_pSession->GetDBConnection()->QueryChangeItemLocation(m_pSession, m_QuestInventory[nSrcIndex].nSerial, 0, 
			DBDNWorldDef::ItemLocation::QuestInventory, m_QuestInventory[nSrcIndex].nItemID, nDestIndex, m_QuestInventory[nSrcIndex].wCount, false, 0, false);
	}
	else if (m_QuestInventory[nDestIndex].nItemID > 0){
		m_pSession->GetDBConnection()->QueryChangeItemLocation(m_pSession, m_QuestInventory[nDestIndex].nSerial, 0, 
			DBDNWorldDef::ItemLocation::QuestInventory, m_QuestInventory[nDestIndex].nItemID, nSrcIndex, m_QuestInventory[nDestIndex].wCount, false, 0, false);
	}

	TQuestItem SrcQuestInven = m_QuestInventory[nSrcIndex];
	m_QuestInventory[nSrcIndex] = m_QuestInventory[nDestIndex];
	m_QuestInventory[nDestIndex] = SrcQuestInven;

	return ERROR_NONE;
}

int CDNUserItem::_SwapEquipToInven(int nSrcIndex, int nDestIndex)
{
	if (!_CheckRangeEquipIndex(nSrcIndex)) return ERROR_ITEM_INDEX_UNMATCH;
	if (!_CheckRangeInventoryIndex(nDestIndex)) return ERROR_ITEM_INDEX_UNMATCH;

	// 서로 바꿔도 되는지 일단 장착 검사해보쟈
	if (m_Inventory[nDestIndex].nItemID > 0)
	{
		if (!IsEquipItem(nSrcIndex, m_Inventory[nDestIndex].nItemID)) return ERROR_ITEM_EQUIPMISMATCH;
	}

	char cInventoryLocationCode = DBDNWorldDef::ItemLocation::Inventory;
#if defined(PRE_PERIOD_INVENTORY)
	if (_CheckRangePeriodInventoryIndex(nDestIndex))
		cInventoryLocationCode = DBDNWorldDef::ItemLocation::PeriodInventory;
#endif	// #if defined(PRE_PERIOD_INVENTORY)

	if ((m_Equip[nSrcIndex].nItemID > 0) && (m_Inventory[nDestIndex].nItemID > 0)){
		m_pSession->GetDBConnection()->QuerySwitchItemLocation(m_pSession, DBDNWorldDef::ItemLocation::Equip, 
			nSrcIndex, m_Inventory[nDestIndex].nSerial, cInventoryLocationCode, nDestIndex, m_Equip[nSrcIndex].nSerial);
	}
	else if (m_Equip[nSrcIndex].nItemID > 0){
		m_pSession->GetDBConnection()->QueryChangeItemLocation(m_pSession, m_Equip[nSrcIndex].nSerial, 0, m_Equip[nSrcIndex].nItemID, 
			cInventoryLocationCode, nDestIndex, m_Equip[nSrcIndex].wCount, false, 0, false);
	}
	else if (m_Inventory[nDestIndex].nItemID > 0){
		m_pSession->GetDBConnection()->QueryChangeItemLocation(m_pSession, m_Inventory[nDestIndex].nSerial, 0, m_Inventory[nDestIndex].nItemID, 
			DBDNWorldDef::ItemLocation::Equip, nSrcIndex, m_Inventory[nDestIndex].wCount, false, 0, false);
	}

	TItem Equip = m_Equip[nSrcIndex];

	m_Equip[nSrcIndex] = m_Inventory[nDestIndex];

	m_Inventory[nDestIndex] = Equip;
	_UpdateInventoryCoolTime(nDestIndex);

	BroadcastChangeEquip(nSrcIndex, m_Equip[nSrcIndex]);

	return ERROR_NONE;
}

int CDNUserItem::_SwapInvenToEquip(int nSrcIndex, int nDestIndex)
{
	if (!_CheckRangeInventoryIndex(nSrcIndex)) return ERROR_ITEM_INDEX_UNMATCH;
	if (!_CheckRangeEquipIndex(nDestIndex)) return ERROR_ITEM_INDEX_UNMATCH;

	// 서로 바꿔도 되는지 일단 장착 검사해보쟈
	if (m_Inventory[nSrcIndex].nItemID > 0)
	{
		if (!IsEquipItem(nDestIndex, m_Inventory[nSrcIndex].nItemID)) return ERROR_ITEM_EQUIPMISMATCH;
	}

	char cInventoryLocationCode = DBDNWorldDef::ItemLocation::Inventory;
#if defined(PRE_PERIOD_INVENTORY)
	if (_CheckRangePeriodInventoryIndex(nSrcIndex))
		cInventoryLocationCode = DBDNWorldDef::ItemLocation::PeriodInventory;
#endif	// #if defined(PRE_PERIOD_INVENTORY)

	if ((m_Inventory[nSrcIndex].nItemID > 0) && (m_Equip[nDestIndex].nItemID > 0)){
		m_pSession->GetDBConnection()->QuerySwitchItemLocation(m_pSession, cInventoryLocationCode, 
			nSrcIndex, m_Equip[nDestIndex].nSerial, DBDNWorldDef::ItemLocation::Equip, nDestIndex, m_Inventory[nSrcIndex].nSerial);
	}
	else if (m_Inventory[nSrcIndex].nItemID > 0){
		m_pSession->GetDBConnection()->QueryChangeItemLocation(m_pSession, m_Inventory[nSrcIndex].nSerial, 0, m_Inventory[nSrcIndex].nItemID, 
			DBDNWorldDef::ItemLocation::Equip, nDestIndex, m_Inventory[nSrcIndex].wCount, false, 0, false);
	}
	else if (m_Equip[nDestIndex].nItemID > 0){
		m_pSession->GetDBConnection()->QueryChangeItemLocation(m_pSession, m_Equip[nDestIndex].nSerial, 0, m_Equip[nDestIndex].nItemID, 
			cInventoryLocationCode, nSrcIndex, m_Equip[nDestIndex].wCount, false, 0, false);
	}

	TItem Inven = m_Inventory[nSrcIndex];

	m_Inventory[nSrcIndex] = m_Equip[nDestIndex];
	_UpdateInventoryCoolTime(nSrcIndex);

	m_Equip[nDestIndex] = Inven;

	BroadcastChangeEquip(nDestIndex, m_Equip[nDestIndex]);

	m_pSession->GetEventSystem()->OnEvent( EventSystem::OnItemEquip, 1, EventSystem::ItemID, m_Equip[nDestIndex].nItemID );

	char cLevel = 99;
	for (int i = EQUIP_HELMET; i <= EQUIP_FOOT; i++){
		if (cLevel > m_Equip[i].cLevel) cLevel = m_Equip[i].cLevel;
		if (cLevel == 0) break;
	}
	if (cLevel > 0) {
		m_pSession->GetEventSystem()->OnEvent( EventSystem::OnItemEquip, 1,
			EventSystem::AllPartsMinLevel, cLevel );
	}

	return ERROR_NONE;
}

int CDNUserItem::_SwapInvenToWare(int nSrcIndex, int nDestIndex)
{
	if (!_CheckRangeInventoryIndex(nSrcIndex)) return ERROR_ITEM_INDEX_UNMATCH;
	if (!_CheckRangeWarehouseIndex(nDestIndex)) return ERROR_ITEM_INDEX_UNMATCH;

#if defined( PRE_ADD_NAMEDITEM_SYSTEM )
	bool bMoveNamedItem = false;
#endif
	if (m_Inventory[nSrcIndex].nItemID > 0){
		TItem *pSrcItem = &m_Inventory[nSrcIndex];
		int nSrcItemRank = g_pDataManager->GetItemRank(pSrcItem->nItemID);
#if defined( PRE_ADD_NAMEDITEM_SYSTEM )
		if( nSrcItemRank == ITEMRANK_SSS )
			bMoveNamedItem = true;
#endif
	}
	if (m_Warehouse[nDestIndex].nItemID > 0){
		TItem *pDestItem = &m_Warehouse[nDestIndex];
		int nDestItemRank = g_pDataManager->GetItemRank(pDestItem->nItemID);
#if defined( PRE_ADD_NAMEDITEM_SYSTEM )
		if( nDestItemRank == ITEMRANK_SSS )
			bMoveNamedItem = true;
#endif
	}

	char cInventoryLocationCode = DBDNWorldDef::ItemLocation::Inventory;
	char cWarehouseLocationCode = DBDNWorldDef::ItemLocation::Warehouse;
#if defined(PRE_PERIOD_INVENTORY)
	if (_CheckRangePeriodInventoryIndex(nSrcIndex)){
		cInventoryLocationCode = DBDNWorldDef::ItemLocation::PeriodInventory;
	}
	if (_CheckRangePeriodWarehouseIndex(nDestIndex)){
		cWarehouseLocationCode = DBDNWorldDef::ItemLocation::PeriodWarehouse;
	}
#endif	// #if defined(PRE_PERIOD_INVENTORY)

	if ((m_Inventory[nSrcIndex].nItemID > 0) && (m_Warehouse[nDestIndex].nItemID > 0)){
		m_pSession->GetDBConnection()->QuerySwitchItemLocation(m_pSession, cInventoryLocationCode, 
			0, m_Warehouse[nDestIndex].nSerial, cWarehouseLocationCode, 0, m_Inventory[nSrcIndex].nSerial);
	}
	else if (m_Inventory[nSrcIndex].nItemID > 0){
		m_pSession->GetDBConnection()->QueryChangeItemLocation(m_pSession, m_Inventory[nSrcIndex].nSerial, 0, m_Inventory[nSrcIndex].nItemID, 
			cWarehouseLocationCode, nDestIndex, m_Inventory[nSrcIndex].wCount, false, 0, false);
	}
	else if (m_Warehouse[nDestIndex].nItemID > 0){
		m_pSession->GetDBConnection()->QueryChangeItemLocation(m_pSession, m_Warehouse[nDestIndex].nSerial, 0, m_Warehouse[nDestIndex].nItemID, 
			cInventoryLocationCode, nSrcIndex, m_Warehouse[nDestIndex].wCount, false, 0, false);
	}

	TItem Inven = m_Inventory[nSrcIndex];

	m_Inventory[nSrcIndex] = m_Warehouse[nDestIndex];
	_UpdateInventoryCoolTime(nSrcIndex);

	m_Warehouse[nDestIndex] = Inven;
	_UpdateWarehouseCoolTime(nDestIndex);

#if defined( PRE_ADD_NAMEDITEM_SYSTEM )
#if defined( _VILLAGESERVER )
	if( bMoveNamedItem )
	{
		int nNamedItemID = 0;
		TItem* NameItem = GetItemRank(ITEMRANK_SSS);
		if(NameItem)
		{
			nNamedItemID = NameItem->nItemID;
		}
		m_pSession->GetParamData()->nIndex = nNamedItemID;
		m_pSession->SendUserLocalMessage(0, FM_HAVE_NAMEDITEM);		 
	}
#endif
#endif

	return ERROR_NONE;
}

int CDNUserItem::_SwapWareToInven(int nSrcIndex, int nDestIndex)
{
	if (!_CheckRangeWarehouseIndex(nSrcIndex)) return ERROR_ITEM_INDEX_UNMATCH;
	if (!_CheckRangeInventoryIndex(nDestIndex)) return ERROR_ITEM_INDEX_UNMATCH;

#if defined( PRE_ADD_NAMEDITEM_SYSTEM )
	bool bMoveNamedItem = false;
#endif

	TItemData ItemData = { 0, };
	if (m_Warehouse[nSrcIndex].nItemID > 0){
		TItem *pSrcItem = &m_Warehouse[nSrcIndex];
		int nItemRank = g_pDataManager->GetItemRank(pSrcItem->nItemID);
#if defined( PRE_ADD_NAMEDITEM_SYSTEM )
		if( nItemRank == ITEMRANK_SSS )
			bMoveNamedItem = true;
#endif
	}
	if (m_Inventory[nDestIndex].nItemID > 0){
		TItem *pDestItem = &m_Inventory[nDestIndex];
		int nItemRank = g_pDataManager->GetItemRank(pDestItem->nItemID);
#if defined( PRE_ADD_NAMEDITEM_SYSTEM )
		if( nItemRank == ITEMRANK_SSS )
			bMoveNamedItem = true;
#endif
	}

	char cInventoryLocationCode = DBDNWorldDef::ItemLocation::Inventory;
	char cWarehouseLocationCode = DBDNWorldDef::ItemLocation::Warehouse;
#if defined(PRE_PERIOD_INVENTORY)
	if (_CheckRangePeriodInventoryIndex(nDestIndex)){
		cInventoryLocationCode = DBDNWorldDef::ItemLocation::PeriodInventory;
	}
	if (_CheckRangePeriodWarehouseIndex(nSrcIndex)){
		cWarehouseLocationCode = DBDNWorldDef::ItemLocation::PeriodWarehouse;
	}
#endif	// #if defined(PRE_PERIOD_INVENTORY)

	if ((m_Warehouse[nSrcIndex].nItemID > 0) && (m_Inventory[nDestIndex].nItemID > 0)){
		m_pSession->GetDBConnection()->QuerySwitchItemLocation(m_pSession, cWarehouseLocationCode, 
			0, m_Inventory[nDestIndex].nSerial, cInventoryLocationCode, 0, m_Warehouse[nSrcIndex].nSerial);
	}
	else if (m_Warehouse[nSrcIndex].nItemID > 0){
		m_pSession->GetDBConnection()->QueryChangeItemLocation(m_pSession, m_Warehouse[nSrcIndex].nSerial, 0, m_Warehouse[nSrcIndex].nItemID, 
			cInventoryLocationCode, nDestIndex, m_Warehouse[nSrcIndex].wCount, false, 0, false);
	}
	else if (m_Inventory[nDestIndex].nItemID > 0){
		m_pSession->GetDBConnection()->QueryChangeItemLocation(m_pSession, m_Inventory[nDestIndex].nSerial, 0, m_Inventory[nDestIndex].nItemID, 
			cWarehouseLocationCode, nSrcIndex, m_Inventory[nDestIndex].wCount, false, 0, false);
	}

	TItem Ware = m_Warehouse[nSrcIndex];

	m_Warehouse[nSrcIndex] = m_Inventory[nDestIndex];
	_UpdateWarehouseCoolTime(nSrcIndex);

	m_Inventory[nDestIndex] = Ware;
	_UpdateInventoryCoolTime(nDestIndex);

#if defined( PRE_ADD_NAMEDITEM_SYSTEM )
#if defined( _VILLAGESERVER )
	if( bMoveNamedItem )
	{
		int nNamedItemID = 0;
		TItem* NameItem = GetItemRank(ITEMRANK_SSS);
		if(NameItem)
		{
			nNamedItemID = NameItem->nItemID;
		}
		m_pSession->GetParamData()->nIndex = nNamedItemID;
		m_pSession->SendUserLocalMessage(0, FM_HAVE_NAMEDITEM);
	}
#endif
#endif

	return ERROR_NONE;
}

// Cash
int CDNUserItem::_SwapCashEquipToCashInven(int nSrcEquipIndex, INT64 biDestInvenSerial)
{
	if (!_CheckRangeCashEquipIndex(nSrcEquipIndex)) return ERROR_ITEM_INDEX_UNMATCH;
	// Dest는 검사하지않는다.

	// 서로 바꿔도 되는지 일단 장착 검사해보쟈
	int nInvenItemID = 0;
	short wInvenCount = 0;

	TItem NewItem = { 0, };
	const TItem *pInven = GetCashInventory(biDestInvenSerial);
	if (pInven)
	{
		if (!IsEquipItem(nSrcEquipIndex, pInven->nItemID)) return ERROR_ITEM_EQUIPMISMATCH;
		nInvenItemID = pInven->nItemID;
		wInvenCount = pInven->wCount;

		NewItem = *pInven;
	}

	if ((m_CashEquip[nSrcEquipIndex].nItemID > 0) && (nInvenItemID > 0)){
		m_pSession->GetDBConnection()->QuerySwitchItemLocation(m_pSession, DBDNWorldDef::ItemLocation::CashEquip, 
			nSrcEquipIndex, biDestInvenSerial, DBDNWorldDef::ItemLocation::CashInventory, 0, m_CashEquip[nSrcEquipIndex].nSerial);
	}
	else if (m_CashEquip[nSrcEquipIndex].nItemID > 0){
		m_pSession->GetDBConnection()->QueryChangeItemLocation(m_pSession, m_CashEquip[nSrcEquipIndex].nSerial, 0, m_CashEquip[nSrcEquipIndex].nItemID, 
			DBDNWorldDef::ItemLocation::CashInventory, 0, m_CashEquip[nSrcEquipIndex].wCount, false, 0, false);
	}
	else if (nInvenItemID > 0){
		m_pSession->GetDBConnection()->QueryChangeItemLocation(m_pSession, biDestInvenSerial, 0, nInvenItemID, 
			DBDNWorldDef::ItemLocation::CashEquip, nSrcEquipIndex, wInvenCount, false, 0, false);
	}

	TItem Equip = m_CashEquip[nSrcEquipIndex];
	m_CashEquip[nSrcEquipIndex] = NewItem;
	if (NewItem.nItemID > 0)
		_PopCashInventoryBySerial(NewItem.nSerial, NewItem.wCount);
	if (Equip.nItemID > 0) 
		_PushCashInventory(Equip);

	BroadcastChangeCashEquip(nSrcEquipIndex, m_CashEquip[nSrcEquipIndex]);

	return ERROR_NONE;
}

int CDNUserItem::_SwapCashInvenToCashEquip(INT64 biSrcInvenSerial, int nDestEquipIndex)
{
	if (!_CheckRangeCashEquipIndex(nDestEquipIndex)) return ERROR_ITEM_INDEX_UNMATCH;

	const TItem *pInven = GetCashInventory(biSrcInvenSerial);
	if (!pInven) return ERROR_ITEM_INDEX_UNMATCH;

	// 서로 바꿔도 되는지 일단 장착 검사해보쟈
	if (pInven->nItemID > 0){
		if (!IsEquipItem(nDestEquipIndex, pInven->nItemID))
			return ERROR_ITEM_EQUIPMISMATCH;
	}

	if ((pInven->nItemID > 0) && (m_CashEquip[nDestEquipIndex].nItemID > 0)){
		m_pSession->GetDBConnection()->QuerySwitchItemLocation(m_pSession, DBDNWorldDef::ItemLocation::CashInventory, 
			0, m_CashEquip[nDestEquipIndex].nSerial, DBDNWorldDef::ItemLocation::CashEquip, nDestEquipIndex, pInven->nSerial);
	}
	else if (pInven->nItemID > 0){
		m_pSession->GetDBConnection()->QueryChangeItemLocation(m_pSession, pInven->nSerial, 0, pInven->nItemID, 
			DBDNWorldDef::ItemLocation::CashEquip, nDestEquipIndex, pInven->wCount, false, 0, false);
	}
	else if (m_CashEquip[nDestEquipIndex].nItemID > 0){
		m_pSession->GetDBConnection()->QueryChangeItemLocation(m_pSession, m_CashEquip[nDestEquipIndex].nSerial, 0, m_CashEquip[nDestEquipIndex].nItemID, 
			DBDNWorldDef::ItemLocation::CashInventory, 0, m_CashEquip[nDestEquipIndex].wCount, false, 0, false);
	}

	TItem NewInven = *pInven;
	if (NewInven.nItemID > 0)
		_PopCashInventoryBySerial(NewInven.nSerial, NewInven.wCount);
	if (m_CashEquip[nDestEquipIndex].nItemID > 0)
		_PushCashInventory(m_CashEquip[nDestEquipIndex]);

	m_CashEquip[nDestEquipIndex] = NewInven;
	BroadcastChangeCashEquip(nDestEquipIndex, m_CashEquip[nDestEquipIndex]);

	return ERROR_NONE;
}

int CDNUserItem::_SwapCashGlyphToCashInven(int nSrcGlyphIndex, INT64 biDestInvenSerial)
{
	if (!_CheckRangeCashGlyphIndex(nSrcGlyphIndex)) return ERROR_ITEM_INDEX_UNMATCH;
	// Dest는 검사하지않는다.

	// 서로 바꿔도 되는지 일단 장착 검사해보쟈
	int nInvenItemID = 0;
	short wInvenCount = 0;

	TItem NewItem = { 0, };
	const TItem *pInven = GetCashInventory(biDestInvenSerial);
	if (pInven)
	{
		if (!IsGlyphItem(nSrcGlyphIndex, pInven->nItemID)) return ERROR_ITEM_INDEX_UNMATCH;
		nInvenItemID = pInven->nItemID;
		wInvenCount = pInven->wCount;

		NewItem = *pInven;
	}

	if ((m_Glyph[nSrcGlyphIndex].nItemID > 0) && (nInvenItemID > 0)){
		m_pSession->GetDBConnection()->QuerySwitchItemLocation(m_pSession, DBDNWorldDef::ItemLocation::Glyph, 
			nSrcGlyphIndex, biDestInvenSerial, DBDNWorldDef::ItemLocation::CashInventory, 0, m_Glyph[nSrcGlyphIndex].nSerial);
	}
	else if (m_Glyph[nSrcGlyphIndex].nItemID > 0){
		m_pSession->GetDBConnection()->QueryChangeItemLocation(m_pSession, m_Glyph[nSrcGlyphIndex].nSerial, 0, m_Glyph[nSrcGlyphIndex].nItemID, 
			DBDNWorldDef::ItemLocation::CashInventory, 0, m_Glyph[nSrcGlyphIndex].wCount, false, 0, false);
	}
	else if (nInvenItemID > 0){
		m_pSession->GetDBConnection()->QueryChangeItemLocation(m_pSession, biDestInvenSerial, 0, nInvenItemID, 
			DBDNWorldDef::ItemLocation::Glyph, nSrcGlyphIndex, wInvenCount, false, 0, false);
	}

#if defined(_VILLAGESERVER)
	if( m_Glyph[nSrcGlyphIndex].nItemID > 0 ) 
		m_pSession->GetSkill()->OnDetachEquip( &m_Glyph[nSrcGlyphIndex] );
#endif

	TItem Glyph = m_Glyph[nSrcGlyphIndex];
	m_Glyph[nSrcGlyphIndex] = NewItem;
	if (NewItem.nItemID > 0)
		_PopCashInventoryBySerial(NewItem.nSerial, NewItem.wCount);
	if (Glyph.nItemID > 0) 
		_PushCashInventory(Glyph);

#if defined(_VILLAGESERVER)
	if( m_Glyph[nSrcGlyphIndex].nItemID > 0 ) 
		m_pSession->GetSkill()->OnAttachEquip( &m_Glyph[nSrcGlyphIndex] );
#endif

	return ERROR_NONE;
}

int CDNUserItem::_SwapCashInvenToCashGlyph(INT64 biSrcInvenSerial, int nDestGlyphIndex)
{
	if (!_CheckRangeCashGlyphIndex(nDestGlyphIndex)) return ERROR_ITEM_INDEX_UNMATCH;

	const TItem *pInven = GetCashInventory(biSrcInvenSerial);
	if (!pInven) return ERROR_ITEM_INDEX_UNMATCH;

	// 서로 바꿔도 되는지 일단 장착 검사해보쟈
	if (pInven->nItemID > 0){
		if (!IsGlyphItem(nDestGlyphIndex, pInven->nItemID))
			return ERROR_ITEM_INDEX_UNMATCH;
	}

	if ((pInven->nItemID > 0) && (m_Glyph[nDestGlyphIndex].nItemID > 0)){
		m_pSession->GetDBConnection()->QuerySwitchItemLocation(m_pSession, DBDNWorldDef::ItemLocation::CashInventory, 
			0, m_Glyph[nDestGlyphIndex].nSerial, DBDNWorldDef::ItemLocation::Glyph, nDestGlyphIndex, pInven->nSerial);
	}
	else if (pInven->nItemID > 0){
		m_pSession->GetDBConnection()->QueryChangeItemLocation(m_pSession, pInven->nSerial, 0, pInven->nItemID, 
			DBDNWorldDef::ItemLocation::Glyph, nDestGlyphIndex, pInven->wCount, false, 0, false);
	}
	else if (m_Glyph[nDestGlyphIndex].nItemID > 0){
		m_pSession->GetDBConnection()->QueryChangeItemLocation(m_pSession, m_Glyph[nDestGlyphIndex].nSerial, 0, m_Glyph[nDestGlyphIndex].nItemID, 
			DBDNWorldDef::ItemLocation::CashInventory, 0, m_Glyph[nDestGlyphIndex].wCount, false, 0, false);
	}

#if defined(_VILLAGESERVER)
	if( m_Glyph[nDestGlyphIndex].nItemID > 0 ) 
		m_pSession->GetSkill()->OnDetachEquip( &m_Glyph[nDestGlyphIndex] );
#endif

	TItem NewInven = *pInven;
	if (NewInven.nItemID > 0)
		_PopCashInventoryBySerial(NewInven.nSerial, NewInven.wCount);
	if (m_Glyph[nDestGlyphIndex].nItemID > 0)
		_PushCashInventory(m_Glyph[nDestGlyphIndex]);

	m_Glyph[nDestGlyphIndex] = NewInven;

#if defined(_VILLAGESERVER)
	if( m_Glyph[nDestGlyphIndex].nItemID > 0 ) 
		m_pSession->GetSkill()->OnAttachEquip( &m_Glyph[nDestGlyphIndex] );
#endif

	return ERROR_NONE;
}

// equip관련
int CDNUserItem::_PushEquipSlot(int nIndex, const TItem &AddItem)
{
	if (!_CheckRangeEquipIndex(nIndex)) return ERROR_ITEM_INDEX_UNMATCH;
	if (AddItem.nItemID <= 0) return ERROR_ITEM_NOTFOUND;

	if (!IsEquipItem(nIndex, m_Equip[nIndex].nItemID)) return ERROR_ITEM_EQUIPMISMATCH;

	TItem *pEquip = &m_Equip[nIndex];	// 실 db쪽 데이터

	if (m_Equip[nIndex].nItemID > 0){		// 이미 무언가 있다(추가냐 아니냐만 판단하면 될듯)
		if (m_Equip[nIndex].nItemID != AddItem.nItemID) return ERROR_ITEM_FAIL;	// 아이템이 같지않다.
		if ((m_Equip[nIndex].nRandomSeed > 0) && (m_Equip[nIndex].nRandomSeed != AddItem.nRandomSeed)) return ERROR_ITEM_FAIL;	// randomseed가 맞지않다.
		if ((!m_Equip[nIndex].bEternity) && (m_Equip[nIndex].tExpireDate != AddItem.tExpireDate)) return ERROR_ITEM_FAIL;

		int nOverlapCount = g_pDataManager->GetItemOverlapCount(m_Equip[nIndex].nItemID);
		if (nOverlapCount <= 0) return ERROR_ITEM_FAIL;
		if (nOverlapCount == 1) return ERROR_ITEM_UNCOUNTABLE;	// 겹치는 아이템이 아니다
		if (m_Equip[nIndex].wCount + AddItem.wCount > nOverlapCount) return ERROR_ITEM_OVERFLOW;	// 아이템 개수가 넘어갔다

		m_Equip[nIndex].wCount += AddItem.wCount;
	}
	else {	// 빈칸이다.
		m_Equip[nIndex] = AddItem;
	}

	int nItemType = g_pDataManager->GetItemMainType(m_Equip[nIndex].nItemID);
	if (nItemType == ITEMTYPE_NORMAL)
		m_Equip[nIndex].nRandomSeed = 0;

	BroadcastChangeEquip(nIndex, m_Equip[nIndex]);

	return ERROR_NONE;
}

int CDNUserItem::_PopEquipSlot(int nIndex, short wCount, bool bSend/*=true*/ )
{
	if (!_CheckRangeEquipIndex(nIndex)) return ERROR_ITEM_INDEX_UNMATCH;
	if (wCount <= 0) return ERROR_ITEM_NOTFOUND;

	if (m_Equip[nIndex].nItemID <= 0) return ERROR_ITEM_NOTFOUND;	// 아이템이 없다
	if (m_Equip[nIndex].wCount < wCount) return ERROR_ITEM_OVERFLOW;		// 가지고 있는것보다 더 왔다

	m_Equip[nIndex].wCount -= wCount;
	if (m_Equip[nIndex].wCount <= 0) 
		memset(&m_Equip[nIndex], 0, sizeof(TItem));

	if( bSend )
		BroadcastChangeEquip(nIndex, m_Equip[nIndex]);

	return ERROR_NONE;
}

int CDNUserItem::FindBlankEquipSlot()
{
	for (int i = 0; i < EQUIPMAX; i++){
		if (m_Equip[i].nItemID == 0) return i;
	}

	return -1;
}

int CDNUserItem::IsTradeEnableItem(int nInvenType, int nInvenIndex, INT64 biInvenSerial, short wInputCount)
{
	const TItem *pItem = NULL;

	switch(nInvenType)
	{
	case ITEMPOSITION_INVEN:
#if defined(PRE_ADD_SERVER_WAREHOUSE)
	case ITEMPOSITION_SERVERWARE:
#endif //#if defined(PRE_ADD_SERVER_WAREHOUSE)
		{
			pItem = GetInventory(nInvenIndex);
			if (!pItem) return ERROR_ITEM_NOTFOUND;
			if (pItem->nSerial != biInvenSerial) return ERROR_ITEM_NOTFOUND;
		}
		break;

	case ITEMPOSITION_CASHINVEN:
#if defined(PRE_ADD_SERVER_WAREHOUSE)
	case ITEMPOSITION_SERVERWARECASH:
#endif //#if defined(PRE_ADD_SERVER_WAREHOUSE)
		{
			pItem = GetCashInventory(biInvenSerial);
			if (!pItem) return ERROR_ITEM_NOTFOUND;
			if (!pItem->bEternity) return ERROR_MARKET_REGISTITEMFAIL;	// 기간제 아이템은 거래불가
#if defined (PRE_MOD_DMIX_NOTRADE)
			if (pItem->nLookItemID > 0) return ERROR_MARKET_REGISTITEMFAIL; // 합성된 코스튭은 거래 불가
#endif
		}
		break;
	}

	if (!pItem)
		return ERROR_ITEM_NOTFOUND;

	if (pItem->wCount < wInputCount)
		return ERROR_ITEM_OVERFLOW;	// 개수 검사

	TItemData *pItemData = g_pDataManager->GetItemData(pItem->nItemID);
	if (pItemData)
	{
		// 획득 시 귀속아이템
		if (pItemData->cReversion == ITEMREVERSION_BELONG && pItem->bSoulbound == true)
		{
#if defined(PRE_ADD_SERVER_WAREHOUSE)
			if( nInvenType == ITEMPOSITION_SERVERWARE || nInvenType == ITEMPOSITION_SERVERWARECASH )
			{
				if( pItemData->IsWStorage ) // 요건 거래가능
					return ERROR_NONE;
			}
#endif
			return ERROR_ITEM_SOULBOUND;
		}
		if (pItemData->cReversion == ITEMREVERSION_GUILD)
			return ERROR_ITEM_SOULBOUND;
	}
	else
	{
		_DANGER_POINT();
		return ERROR_ITEM_FAIL;
	}

	return ERROR_NONE;
}

int CDNUserItem::IsEquipEnableItem(const TItem& equipExpected) const
{
	TItemData *pItemData = g_pDataManager->GetItemData(equipExpected.nItemID);
	if (pItemData)
	{
		if (pItemData->cReversion == ITEMREVERSION_BELONG && equipExpected.bSoulbound == false)
			return ERROR_ITEM_UNABLE_EQUIP_NO_SOUL_BOUND;
	}
	else
	{
		_DANGER_POINT();
		return ERROR_ITEM_FAIL;
	}

	return ERROR_NONE;
}

int CDNUserItem::GetEquipSlotIndex( int nItemID ) const
{
	const TItemData *pItemData = g_pDataManager->GetItemData(nItemID);
	if (!pItemData) 
		return -1;

	switch( pItemData->nType )
	{
		case ITEMTYPE_WEAPON:
		{
			const TWeaponData *pWeaponData = g_pDataManager->GetWeaponData(pItemData->nItemID);
			if (!pWeaponData) 
				return -1;

			if( pItemData->IsCash ) 
			{
				switch( pWeaponData->cEquipType )
				{
					case WEAPON_SWORD:
					case WEAPON_AXE:
					case WEAPON_HAMMER:
					case WEAPON_SMALLBOW:
					case WEAPON_BIGBOW:
					case WEAPON_CROSSBOW:
					case WEAPON_STAFF:
					case WEAPON_MACE:
					case WEAPON_FLAIL:
					case WEAPON_WAND:
					case WEAPON_CANNON:
					case WEAPON_BUBBLEGUN:
					case WEAPON_FAN:
					case WEAPON_CHAKRAM:
					case WEAPON_SCIMITER:
					case WEAPON_DAGGER:
					{
						return CASHEQUIP_WEAPON1;
					}
					case WEAPON_SHIELD:
					case WEAPON_ARROW:
					case WEAPON_BOOK:
					case WEAPON_ORB:
					case WEAPON_PUPPET:
					case WEAPON_GAUNTLET:
					case WEAPON_GLOVE:
					case WEAPON_CHARM:
					case WEAPON_CROOK:
					{
						return CASHEQUIP_WEAPON2;
					}	
				}
			}
			else 
			{
				switch( pWeaponData->cEquipType )
				{
					case WEAPON_SWORD:
					case WEAPON_AXE:
					case WEAPON_HAMMER:
					case WEAPON_SMALLBOW:
					case WEAPON_BIGBOW:
					case WEAPON_CROSSBOW:
					case WEAPON_STAFF:
					case WEAPON_MACE:
					case WEAPON_FLAIL:
					case WEAPON_WAND:
					case WEAPON_CANNON:
					case WEAPON_BUBBLEGUN:
					case WEAPON_FAN:
					case WEAPON_CHAKRAM:
					case WEAPON_SCIMITER:
					case WEAPON_DAGGER:
					{
						return EQUIP_WEAPON1;
					}
					case WEAPON_SHIELD:
					case WEAPON_ARROW:
					case WEAPON_BOOK:
					case WEAPON_ORB:
					case WEAPON_PUPPET:
					case WEAPON_GAUNTLET:
					case WEAPON_GLOVE:
					case WEAPON_CHARM:
					case WEAPON_CROOK:
					{
						return EQUIP_WEAPON2;
					}
				}
			}
			break;
		}
		case ITEMTYPE_PARTS:
		{
			const TPartData *pPartData = g_pDataManager->GetPartData(pItemData->nItemID);
			if(!pPartData) 
				return -1;

			if( pItemData->IsCash ) 
			{
				switch( pPartData->nParts ) 
				{
					case CASHEQUIP_RING1:
					case CASHEQUIP_RING2:
						return CASHEQUIP_RING1;
					default:
						return pPartData->nParts;
				}
			}
			else 
			{
				switch( pPartData->nParts ) 
				{
					case EQUIP_RING1:
					case EQUIP_RING2:
						return EQUIP_RING1;
					default:
						return pPartData->nParts;
				}
			}
			break;
		}
	}

	return -1;
}

bool CDNUserItem::IsEquipItem(int nSlotIndex, int nItemID) const
{
	const TItemData *pItemData = g_pDataManager->GetItemData(nItemID);
	if (!pItemData) return false;

	if (m_pSession->GetLevel() < pItemData->cLevelLimit) return false;
	if (!g_pDataManager->IsPermitItemJob(nItemID, m_pSession->GetStatusData()->cJobArray)) return false;

	switch(pItemData->nType)
	{
	case ITEMTYPE_WEAPON:
		{
			const TWeaponData *pWeaponData = g_pDataManager->GetWeaponData(pItemData->nItemID);
			if (!pWeaponData) return false;

			if( pItemData->IsCash ) {
				switch(nSlotIndex)
				{
					case CASHEQUIP_WEAPON1:
						{
							switch( pWeaponData->cEquipType )
							{
							case WEAPON_SWORD:
							case WEAPON_AXE:
							case WEAPON_HAMMER:
							case WEAPON_SMALLBOW:
							case WEAPON_BIGBOW:
							case WEAPON_CROSSBOW:
							case WEAPON_STAFF:
							case WEAPON_MACE:
							case WEAPON_FLAIL:
							case WEAPON_WAND:
							case WEAPON_CANNON:
							case WEAPON_BUBBLEGUN:
							case WEAPON_FAN:
							case WEAPON_CHAKRAM:
							case WEAPON_SCIMITER:
							case WEAPON_DAGGER:
								{
									break;
								}
							default:
								{
									return false;
								}
							}
							break;
						}
					case CASHEQUIP_WEAPON2:
						{
							switch( pWeaponData->cEquipType )
							{
							case WEAPON_SHIELD:
							case WEAPON_ARROW:
							case WEAPON_BOOK:
							case WEAPON_ORB:
							case WEAPON_PUPPET:
							case WEAPON_GAUNTLET:
							case WEAPON_GLOVE:
							case WEAPON_CHARM:
							case WEAPON_CROOK:
								{
									break;
								}
							default:
								{
									return false;
								}
							}

							break;
						}
					default:
						return false;
				}
				if( pWeaponData->bOneType ) {
					bool bEnable = true;
					const TItem *pNormalWeaponItem = NULL;
					if( nSlotIndex == CASHEQUIP_WEAPON1 ) pNormalWeaponItem = GetEquip(EQUIP_WEAPON1);
					else if( nSlotIndex == CASHEQUIP_WEAPON2 ) pNormalWeaponItem = GetEquip(EQUIP_WEAPON2);
					if( !pNormalWeaponItem ) bEnable = false;
					if( pNormalWeaponItem ) {
						TWeaponData *pNormalWeapon = g_pDataManager->GetWeaponData(pNormalWeaponItem->nItemID);
						if( !pNormalWeapon ) bEnable = false;
						if( pNormalWeapon && pNormalWeapon->cEquipType != pWeaponData->cEquipType ) bEnable = false;
					}
					if( !bEnable )
						return false;
				}
			}
			else {
				switch(nSlotIndex)
				{
					case EQUIP_WEAPON1:
					{
						switch( pWeaponData->cEquipType )
						{
							case WEAPON_SWORD:
							case WEAPON_AXE:
							case WEAPON_HAMMER:
							case WEAPON_SMALLBOW:
							case WEAPON_BIGBOW:
							case WEAPON_CROSSBOW:
							case WEAPON_STAFF:
							case WEAPON_MACE:
							case WEAPON_FLAIL:
							case WEAPON_WAND:
							case WEAPON_CANNON:
							case WEAPON_BUBBLEGUN:
							case WEAPON_FAN:
							case WEAPON_CHAKRAM:
							case WEAPON_SCIMITER:
							case WEAPON_DAGGER:
							{
								break;
							}
							default:
							{
								return false;
							}
						}
						break;
					}
					case EQUIP_WEAPON2:
					{
						switch( pWeaponData->cEquipType )
						{
							case WEAPON_SHIELD:
							case WEAPON_ARROW:
							case WEAPON_BOOK:
							case WEAPON_ORB:
							case WEAPON_PUPPET:
							case WEAPON_GAUNTLET:
							case WEAPON_GLOVE:
							case WEAPON_CHARM:
							case WEAPON_CROOK:
							{
								break;
							}
							default:
							{
								return false;
							}
						}

						break;
					}
					default:
						return false;
				}
			}
		}
		break;

	case ITEMTYPE_PARTS:
		{
			const TPartData *pPartData = g_pDataManager->GetPartData(pItemData->nItemID);
			if (!pPartData) return false;

			if( pItemData->IsCash ) {
				switch( pPartData->nParts ) {
					case CASHEQUIP_RING1:
					case CASHEQUIP_RING2:
						if( nSlotIndex != CASHEQUIP_RING1 && nSlotIndex != CASHEQUIP_RING2 ) return false;
						break;
					default:
						if (pPartData->nParts != nSlotIndex) return false;
						break;
				}
			}
			else {
				switch( pPartData->nParts ) {
					case EQUIP_RING1:
					case EQUIP_RING2:
						if( nSlotIndex != EQUIP_RING1 && nSlotIndex != EQUIP_RING2 ) return false;
						break;
					default:
						if (pPartData->nParts != nSlotIndex) return false;
						break;
				}
			}
		}
		break;

	case ITEMTYPE_FACIAL:
	case ITEMTYPE_HAIRDRESS:
		return true;

	case ITEMTYPE_VEHICLEEFFECT:
		if( pItemData->IsCash )
		{
			switch(nSlotIndex)
			{
			case CASHEQUIP_EFFECT:
				return true;

			default:
				return false;
			}
		}
		break;

	case ITEMTYPE_PLATE:
	case ITEMTYPE_NORMAL:
	case ITEMTYPE_SKILL:
	case ITEMTYPE_JEWEL:
	default:
		return false;
	}

	return true;
}

void CDNUserItem::BroadcastChangeEquip(int nSlotIndex, const TItem &EquipItem)
{
#if defined(_VILLAGESERVER)
	if (!m_pSession) return;

	m_pSession->GetParamData()->ItemInfo.cSlotIndex = nSlotIndex;
	m_pSession->GetParamData()->ItemInfo.Item = EquipItem;

	m_pSession->SendUserLocalMessage(0, FM_CHANGEEQUIP);

#elif defined(_GAMESERVER)
	if (!m_pSession->GetGameRoom()) return;

	TItemInfo ItemInfo;
	ItemInfo.cSlotIndex = nSlotIndex;
	ItemInfo.Item = EquipItem;

	for (DWORD i = 0; i < m_pSession->GetGameRoom()->GetUserCount(); i++) {
		CDNGameRoom::PartyStruct *pStruct = m_pSession->GetGameRoom()->GetPartyData(i);
		if (pStruct == NULL) continue;
		if (pStruct->pSession != m_pSession) {
			pStruct->pSession->SendChangeEquip(m_pSession->GetSessionID(), ItemInfo);
			if (nSlotIndex == EQUIP_HAIR || nSlotIndex == EQUIP_HELMET || nSlotIndex == EQUIP_EARRING)
				pStruct->pSession->SendPartyMemberPart(m_pSession);
		}
	}
#endif
}

void CDNUserItem::BroadcastChangeCashEquip(int nSlotIndex, const TItem &EquipItem)
{
#if defined(_VILLAGESERVER)
	if (!m_pSession) return;

	m_pSession->GetParamData()->CashItemInfo.cSlotIndex = nSlotIndex;
	m_pSession->GetParamData()->CashItemInfo.Item = EquipItem;

	m_pSession->SendUserLocalMessage(0, FM_CHANGECASHEQUIP);

#elif defined(_GAMESERVER)
	if (!m_pSession->GetGameRoom()) return;

	TItemInfo ItemInfo;
	ItemInfo.cSlotIndex = nSlotIndex;
	ItemInfo.Item = EquipItem;

	for (DWORD i = 0; i < m_pSession->GetGameRoom()->GetUserCount(); i++) {
		CDNGameRoom::PartyStruct *pStruct = m_pSession->GetGameRoom()->GetPartyData(i);
		if (pStruct == NULL) continue;
		if (pStruct->pSession != m_pSession) {
			pStruct->pSession->SendChangeCashEquip(m_pSession->GetSessionID(), ItemInfo);
			if (nSlotIndex == CASHEQUIP_HELMET || nSlotIndex == CASHEQUIP_EARRING)
				pStruct->pSession->SendPartyMemberPart(m_pSession);
		}
	}
#endif
}

// Glyph관련 함수
int CDNUserItem::_PushGlyphSlot(int nIndex, const TItem &AddItem)
{
	if (!_CheckRangeGlyphIndex(nIndex)) return ERROR_ITEM_INDEX_UNMATCH;
	if (AddItem.nItemID <= 0) return ERROR_ITEM_NOTFOUND;
	if ((!m_Glyph[nIndex].bEternity) && (m_Glyph[nIndex].tExpireDate != AddItem.tExpireDate)) return ERROR_ITEM_FAIL;

	if (!g_pDataManager->IsGlyphLevel(AddItem.nItemID, m_pSession->GetLevel()))
		return ERROR_GLYPH_LIMITLEVEL;
	if (!g_pDataManager->IsGlyphSlotLevel(nIndex, m_pSession->GetLevel()))
		return ERROR_GLYPH_SLOTLIMITLEVEL;
	if (!IsGlyphItem(nIndex, m_Glyph[nIndex].nItemID)) return ERROR_ITEM_EQUIPMISMATCH;

#if defined(_VILLAGESERVER)
	if( m_Glyph[nIndex].nItemID > 0 ) 
		m_pSession->GetSkill()->OnDetachEquip( &m_Glyph[nIndex] );
#endif

	if (m_Glyph[nIndex].nItemID > 0){		// 이미 무언가 있다(추가냐 아니냐만 판단하면 될듯)
		if (m_Glyph[nIndex].nItemID != AddItem.nItemID) return ERROR_ITEM_FAIL;	// 아이템이 같지않다.
		if ((m_Glyph[nIndex].nRandomSeed > 0) && (m_Glyph[nIndex].nRandomSeed != AddItem.nRandomSeed)) return ERROR_ITEM_FAIL;	// randomseed가 맞지않다.

		int nOverlapCount = g_pDataManager->GetItemOverlapCount(m_Glyph[nIndex].nItemID);
		if (nOverlapCount <= 0) return ERROR_ITEM_FAIL;
		if (nOverlapCount == 1) return ERROR_ITEM_UNCOUNTABLE;	// 겹치는 아이템이 아니다
		if (m_Glyph[nIndex].wCount + AddItem.wCount > nOverlapCount) return ERROR_ITEM_OVERFLOW;	// 아이템 개수가 넘어갔다

		m_Glyph[nIndex].wCount += AddItem.wCount;
	}
	else {	// 빈칸이다.
		m_Glyph[nIndex] = AddItem;
	}

#if defined(_VILLAGESERVER)
	if( m_Glyph[nIndex].nItemID > 0 ) 
		m_pSession->GetSkill()->OnAttachEquip( &m_Glyph[nIndex] );
#endif

	BroadcastChangeGlyph(nIndex, m_Glyph[nIndex]);

	return ERROR_NONE;
}

int CDNUserItem::_PopGlyphSlot(int nIndex, short wCount, bool bSend/*=true*/ )
{
	if (!_CheckRangeGlyphIndex(nIndex)) return ERROR_ITEM_INDEX_UNMATCH;
	if (wCount <= 0) return ERROR_ITEM_NOTFOUND;

	if (m_Glyph[nIndex].nItemID <= 0) return ERROR_ITEM_NOTFOUND;	// 아이템이 없다
	if (m_Glyph[nIndex].wCount < wCount) return ERROR_ITEM_OVERFLOW;		// 가지고 있는것보다 더 왔다

#if defined(_VILLAGESERVER)
	m_pSession->GetSkill()->OnDetachEquip( &m_Glyph[nIndex] );
#endif

	m_Glyph[nIndex].wCount -= wCount;
	if (m_Glyph[nIndex].wCount <= 0) 
		memset(&m_Glyph[nIndex], 0, sizeof(TItem));

	if( bSend )
		BroadcastChangeGlyph(nIndex, m_Glyph[nIndex]);

	return ERROR_NONE;
}

bool CDNUserItem::IsGlyphItem(int nSlotIndex, int nItemID)
{
	TItemData *pItemData = g_pDataManager->GetItemData(nItemID);
	if (!pItemData) return false;

	// 확장슬롯은 타입검사 없다.
	if( nSlotIndex >= GLYPH_CASH1 && nSlotIndex <= GLYPH_CASH3 )
		return true;

	switch(pItemData->nType)
	{
	case ITEMTYPE_GLYPH:
		{
			int nGlyphType = g_pDataManager->GetGlyphType(nItemID);

			switch(nGlyphType){
			case GLYPETYPE_ENCHANT:
				if( !( nSlotIndex >= GLYPH_ENCHANT1 && nSlotIndex <= GLYPH_ENCHANT8 ) ) return false;
				break;

			case GLYPETYPE_SKILL:
				if( !( nSlotIndex >= GLYPH_SKILL1 && nSlotIndex <= GLYPH_SKILL4 ) ) return false;
				break;

			case GLYPETYPE_SPECIALSKILL:
				if( nSlotIndex != GLYPH_SPECIALSKILL  ) return false;
				break;
#if defined(PRE_ADD_DRAGON_FELLOWSHIP_GLYPH)
			case GLYPHTYPE_DRAGON :
				{
					if( !(nSlotIndex >= GLYPH_DRAGON1 && nSlotIndex <= GLYPH_DRAGON4) )
						return false;
					if( CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::DragonFellowShipGlyph ) == 0.0f )
						return false;
				}				
				break;
#endif //#if defined(PRE_ADD_DRAGON_FELLOWSHIP_GLYPH)
			}
			return true;
		}
		break;
	}

	return false;
}

void CDNUserItem::BroadcastChangeGlyph(int nSlotIndex, const TItem &GlyphItem)
{
#if defined(_VILLAGESERVER)
	if (!m_pSession) return;

	m_pSession->GetParamData()->ItemInfo.cSlotIndex = nSlotIndex;
	m_pSession->GetParamData()->ItemInfo.Item = GlyphItem;

	m_pSession->SendUserLocalMessage(0, FM_CHANGEGLYPH);

#elif defined(_GAMESERVER)
	if (!m_pSession->GetGameRoom()) return;

	TItemInfo ItemInfo;
	ItemInfo.cSlotIndex = nSlotIndex;
	ItemInfo.Item = GlyphItem;

	for(DWORD i = 0; i < m_pSession->GetGameRoom()->GetUserCount(); i++) {
		CDNGameRoom::PartyStruct *pStruct = m_pSession->GetGameRoom()->GetPartyData(i);
		if( pStruct == NULL ) continue;
		if (pStruct->pSession != m_pSession)
			pStruct->pSession->SendChangeGlyph(m_pSession->GetSessionID(), ItemInfo);
	}
#endif
}

// inven
int CDNUserItem::_PushInventorySlotItem(int nIndex, const TItem &AddItem)
{
	if (!_CheckRangeInventoryIndex(nIndex)) return ERROR_ITEM_INDEX_UNMATCH;
	if (AddItem.nItemID <= 0) return ERROR_ITEM_NOTFOUND;

	TItemData *pItemData = g_pDataManager->GetItemData(AddItem.nItemID);
	if (!pItemData) return ERROR_ITEM_FAIL;
	if (pItemData->nOverlapCount <= 0) return ERROR_ITEM_FAIL;

	if (IsValidInventorySlot(nIndex, 0, false)){	// 이미 무언가 있다(추가냐 아니냐만 판단하면 될듯)
		if (m_Inventory[nIndex].nItemID != AddItem.nItemID) return ERROR_ITEM_FAIL;	// 아이템이 같지않다.
		if ((!m_Inventory[nIndex].bEternity) && (m_Inventory[nIndex].tExpireDate != AddItem.tExpireDate)) return ERROR_ITEM_FAIL;

		if ((pItemData->nOverlapCount == 1) && (m_Inventory[nIndex].nRandomSeed > 0) && (m_Inventory[nIndex].nRandomSeed != AddItem.nRandomSeed)) return ERROR_ITEM_FAIL;	// randomseed가 맞지않다.
		if (pItemData->nOverlapCount == 1) return ERROR_ITEM_UNCOUNTABLE;	// 겹치는 아이템이 아니다
		if (m_Inventory[nIndex].wCount + AddItem.wCount > pItemData->nOverlapCount) return ERROR_ITEM_OVERFLOW;	// 아이템 개수가 넘어갔다

		m_Inventory[nIndex].wCount += AddItem.wCount;
	}
	else {	// 빈칸이다.
		m_Inventory[nIndex] = AddItem;
	}

	if (pItemData->nType == ITEMTYPE_NORMAL)
		m_Inventory[nIndex].nRandomSeed = 0;

	_UpdateInventoryCoolTime(nIndex);

	// 인벤에 누적된 아이템 정보를 API로 알려준다.
	if (pItemData->IsCollectingEvent)
	{
		int nCount = GetInventoryItemCount(AddItem.nItemID);
		m_pSession->GetQuest()->OnAddItemEx(AddItem.nItemID, nCount);
	}

#if defined( _GAMESERVER )
#if defined( PRE_ADD_NAMEDITEM_SYSTEM )
	if( pItemData->cRank == ITEMRANK_SSS )
	{
		// 네임드아이템 전체공지
		g_pMasterConnectionManager->SendWorldSystemMsg( m_pSession->GetWorldSetID(), m_pSession->GetAccountDBID(), WORLDCHATTYPE_NAMEDITEM, pItemData->nItemID, m_pSession->GetUserJob() );
	}	
#endif
#endif
	return ERROR_NONE;
}

int CDNUserItem::_PopInventorySlotItem(int nIndex, short wCount)
{	
	if (!_CheckRangeInventoryIndex(nIndex)) return ERROR_ITEM_INDEX_UNMATCH;
	if (wCount <= 0) return ERROR_ITEM_NOTFOUND;

	if (m_Inventory[nIndex].nItemID <= 0)		return ERROR_ITEM_NOTFOUND;		// 아이템이 없다
	if (m_Inventory[nIndex].wCount < wCount)	return ERROR_ITEM_OVERFLOW;		// 가지고 있는것보다 더 왔다

	m_Inventory[nIndex].wCount -= wCount;
	if (m_Inventory[nIndex].wCount <= 0)
		memset(&m_Inventory[nIndex], 0, sizeof(TItem));

	_UpdateInventoryCoolTime( nIndex );

	return ERROR_NONE;
}

int CDNUserItem::FindInventorySlot(int nItemID, short wCount) const
{
	if ((nItemID <= 0) || (wCount <= 0)) return -1;

#if defined(PRE_PERIOD_INVENTORY)
	for (int i = 0; i < INVENTORYTOTALMAX; i++){
		if (i == GetInventoryCount()){
			if (IsEnablePeriodInventory())
				i = INVENTORYMAX;
			else
				break;
		}
#else	// #if defined(PRE_PERIOD_INVENTORY)
	for (int i = 0; i < GetInventoryCount(); i++){
#endif	// #if defined(PRE_PERIOD_INVENTORY)
		if ((m_Inventory[i].nItemID == nItemID) && (m_Inventory[i].wCount >= wCount))
			return i;
	}

	return -1;
}

int CDNUserItem::FindInventorySlot(int nItemID, char cOption, short wCount, bool bSoulBound ) const
{
	if ((nItemID <= 0) ||(wCount <= 0)) return -1;

#if defined(PRE_PERIOD_INVENTORY)
	for (int i = 0; i < INVENTORYTOTALMAX; i++){
		if (i == GetInventoryCount()){
			if (IsEnablePeriodInventory())
				i = INVENTORYMAX;
			else
				break;
		}
#else	// #if defined(PRE_PERIOD_INVENTORY)
	for (int i = 0; i < GetInventoryCount(); i++){
#endif	// #if defined(PRE_PERIOD_INVENTORY)
		if ((m_Inventory[i].nItemID == nItemID) && (m_Inventory[i].wCount >= wCount) ) {
#if defined(PRE_MOD_60583)
			if( (cOption != -1 && cOption != m_Inventory[i].cOption) || (bSoulBound == true && !m_Inventory[i].bSoulbound) ) continue;
#else
			if( cOption != -1 && cOption != m_Inventory[i].cOption) continue;
#endif		
			return i;
		}
	}

	return -1;
}

int CDNUserItem::FindInventorySlotBySerial( INT64 biSerial  ) const
{
#if defined(PRE_PERIOD_INVENTORY)
	for (int i = 0; i < INVENTORYTOTALMAX; i++){
		if (i == GetInventoryCount()){
			if (IsEnablePeriodInventory())
				i = INVENTORYMAX;
			else
				break;
		}
#else	// #if defined(PRE_PERIOD_INVENTORY)
	for (int i = 0; i < GetInventoryCount(); i++){
#endif	// #if defined(PRE_PERIOD_INVENTORY)
		if( m_Inventory[i].nSerial == biSerial ) 
			return i;
	}

	return -1;
}

// CashEquip
int CDNUserItem::_PushCashEquipSlot(int nIndex, const TItem &AddItem, bool bSubParts/* = false*/)
{
	if (!_CheckRangeCashEquipIndex(nIndex)) return ERROR_ITEM_INDEX_UNMATCH;
	if (AddItem.nItemID <= 0) return ERROR_ITEM_NOTFOUND;

	if (!bSubParts && (!IsEquipItem(nIndex, AddItem.nItemID))) return ERROR_ITEM_EQUIPMISMATCH;

	if (m_CashEquip[nIndex].nItemID > 0){		// 이미 무언가 있다(추가냐 아니냐만 판단하면 될듯)
		if (m_CashEquip[nIndex].nItemID != AddItem.nItemID) return ERROR_ITEM_FAIL;	// 아이템이 같지않다.
		if ((m_CashEquip[nIndex].nRandomSeed > 0) && (m_CashEquip[nIndex].nRandomSeed != AddItem.nRandomSeed)) return ERROR_ITEM_FAIL;	// randomseed가 맞지않다.

		int nOverlapCount = g_pDataManager->GetItemOverlapCount(m_CashEquip[nIndex].nItemID);
		if (nOverlapCount <= 0) return ERROR_ITEM_FAIL;
		if (nOverlapCount == 1) return ERROR_ITEM_UNCOUNTABLE;	// 겹치는 아이템이 아니다
		if (m_CashEquip[nIndex].wCount + AddItem.wCount > nOverlapCount) return ERROR_ITEM_OVERFLOW;	// 아이템 개수가 넘어갔다

		m_CashEquip[nIndex].wCount += AddItem.wCount;
	}
	else {	// 빈칸이다.
		m_CashEquip[nIndex] = AddItem;
	}

	int nItemType = g_pDataManager->GetItemMainType(AddItem.nItemID);
	if (nItemType == ITEMTYPE_NORMAL)
		m_CashEquip[nIndex].nRandomSeed = 0;

	BroadcastChangeCashEquip(nIndex, m_CashEquip[nIndex]);

	return ERROR_NONE;
}

int CDNUserItem::_PopCashEquipSlot(int nIndex, short wCount)
{
	if (!_CheckRangeCashEquipIndex(nIndex)) return ERROR_ITEM_INDEX_UNMATCH;
	if (wCount <= 0) return ERROR_ITEM_NOTFOUND;

	if (m_CashEquip[nIndex].nItemID <= 0) return ERROR_ITEM_NOTFOUND;	// 아이템이 없다
	if (m_CashEquip[nIndex].wCount < wCount) return ERROR_ITEM_OVERFLOW;		// 가지고 있는것보다 더 왔다

	m_CashEquip[nIndex].wCount -= wCount;
	if (m_CashEquip[nIndex].wCount <= 0) 
		memset(&m_CashEquip[nIndex], 0, sizeof(TItem));

	BroadcastChangeCashEquip(nIndex, m_CashEquip[nIndex]);

	return ERROR_NONE;
}

int CDNUserItem::TakeOffOnePiece(int nItemID)
{
	TPartData *pPart = g_pDataManager->GetPartData(nItemID);
	if (!pPart) return ERROR_ITEM_NOTFOUND;

	int nCashEquipIndexArray[CASHEQUIPMAX] = { 0, };
	nCashEquipIndexArray[0] = pPart->nParts;
	_PopCashEquipSlot(pPart->nParts, 1);	// 메인 빼주고

	int nTotalCount = 1;
	if (!pPart->nSubPartsList.empty()){
		for (int i = 0; i <(int)pPart->nSubPartsList.size(); i++){
			nCashEquipIndexArray[1 + i] = pPart->nSubPartsList[i];
		}

		nTotalCount += (int)pPart->nSubPartsList.size();
	}
	m_pSession->GetDBConnection()->QueryTakeCashEquipmentOff(m_pSession, nTotalCount, nCashEquipIndexArray);

	return ERROR_NONE;
}

int CDNUserItem::GetCashEquipOnePieceMainParts(int nEquipItemID)
{
	int nPartIndex = g_pDataManager->GetItemDetailType(nEquipItemID);

	TPartData *pPart = NULL;
	for (int i = 0; i < CASHEQUIPMAX; i++){
		pPart = g_pDataManager->GetPartData(m_CashEquip[i].nItemID);
		if (!pPart) continue;
		if (pPart->nSubPartsList.empty()) continue;

		if (pPart->nParts == nPartIndex) return pPart->nParts;
		for (int j = 0; j <(int)pPart->nSubPartsList.size(); j++){
			if (pPart->nSubPartsList[j] == nPartIndex) return pPart->nParts;
		}
	}

	return -1;
}

// cash inven
int CDNUserItem::_PushCashInventory(const TItem &AddItem)
{
	if (AddItem.nItemID <= 0) return ERROR_ITEM_NOTFOUND;

	TItemData *pItemData = g_pDataManager->GetItemData(AddItem.nItemID);
	if (!pItemData) return ERROR_ITEM_FAIL;
	if (!pItemData->IsCash) return ERROR_ITEM_FAIL;	// 캐쉬템 아니면 넣을 수 없다

	m_MapCashInventory[AddItem.nSerial] = AddItem;
	if (pItemData->nType == ITEMTYPE_NORMAL)
		m_MapCashInventory[AddItem.nSerial].nRandomSeed = 0;

	_UpdateCashInventoryCoolTime(AddItem.nSerial);

	m_pSession->GetEventSystem()->OnEvent( EventSystem::OnItemGain, 2, EventSystem::ItemID, AddItem.nItemID, EventSystem::ItemCount, GetCashItemCountByItemID(AddItem.nItemID) );

	return ERROR_NONE;
}

int CDNUserItem::_PopCashInventoryBySerial(INT64 biSerial, short wCount)
{
	if (biSerial <= 0) return ERROR_ITEM_NOTFOUND;
	if (wCount <= 0) return ERROR_ITEM_NOTFOUND;

	TMapItem::iterator iter = m_MapCashInventory.find(biSerial);
	if (iter == m_MapCashInventory.end()) return ERROR_ITEM_NOTFOUND;	// 아이템이 없다
	if (iter->second.wCount < wCount)	return ERROR_ITEM_OVERFLOW;		// 지우려는것보다 적다

	iter->second.wCount -= wCount;
	if (iter->second.wCount <= 0)
		m_MapCashInventory.erase(iter);
	else
		_UpdateCashInventoryCoolTime(biSerial);

	return ERROR_NONE;	
}

// 창고 관련
int CDNUserItem::_PushWarehouseSlot(int nIndex, const TItem &AddItem)
{
	if (!_CheckRangeWarehouseIndex(nIndex)) return ERROR_ITEM_INDEX_UNMATCH;
	if (AddItem.nItemID <= 0) return ERROR_ITEM_NOTFOUND;

	TItemData *pItemData = g_pDataManager->GetItemData(AddItem.nItemID);
	if (!pItemData) return ERROR_ITEM_FAIL;
	if (pItemData->nOverlapCount <= 0) return ERROR_ITEM_FAIL;

	if (IsValidWarehouseSlot(nIndex)){		// 이미 무언가 있다(추가냐 아니냐만 판단하면 될듯)
		if (m_Warehouse[nIndex].nItemID != AddItem.nItemID) return ERROR_ITEM_FAIL;	// 아이템이 같지않다.
		if ((!m_Warehouse[nIndex].bEternity) && (m_Warehouse[nIndex].tExpireDate != AddItem.tExpireDate)) return ERROR_ITEM_FAIL;

		if ((pItemData->nOverlapCount == 1) && (m_Warehouse[nIndex].nRandomSeed > 0) && (m_Warehouse[nIndex].nRandomSeed != AddItem.nRandomSeed)) return ERROR_ITEM_FAIL;	// randomseed가 맞지않다.
		if (pItemData->nOverlapCount == 1) return ERROR_ITEM_UNCOUNTABLE;	// 겹치는 아이템이 아니다
		if (m_Warehouse[nIndex].wCount + AddItem.wCount > pItemData->nOverlapCount) return ERROR_ITEM_OVERFLOW;	// 아이템 개수가 넘어갔다

		m_Warehouse[nIndex].wCount += AddItem.wCount;
	}
	else {	// 빈칸이다.
		m_Warehouse[nIndex] = AddItem;
	}

	if (pItemData->nType == ITEMTYPE_NORMAL)
		m_Warehouse[nIndex].nRandomSeed = 0;

	_UpdateWarehouseCoolTime(nIndex);

	return ERROR_NONE;
}

int CDNUserItem::_PopWarehouseSlot(int nIndex, short wCount)
{
	if (!_CheckRangeWarehouseIndex(nIndex)) return ERROR_ITEM_INDEX_UNMATCH;
	if (wCount <= 0) return ERROR_ITEM_NOTFOUND;

	if (m_Warehouse[nIndex].nItemID <= 0) return ERROR_ITEM_NOTFOUND;	// 아이템이 없다
	if (m_Warehouse[nIndex].wCount < wCount) return ERROR_ITEM_OVERFLOW;		// 가지고 있는것보다 더 왔다

	m_Warehouse[nIndex].wCount -= wCount;
	if (m_Warehouse[nIndex].wCount <= 0) 
		memset(&m_Warehouse[nIndex], 0, sizeof(TItem));

	_UpdateWarehouseCoolTime(nIndex);

	return ERROR_NONE;
}

// 퀘스트 인벤 관련 함수
int CDNUserItem::_PushQuestInventorySlot(int nIndex, const TQuestItem &AddItem)
{
	if (!_CheckRangeQuestInventoryIndex(nIndex)) return ERROR_ITEM_INDEX_UNMATCH;
	if (AddItem.nItemID <= 0) return ERROR_ITEM_NOTFOUND;

	TItemData *pItemData = g_pDataManager->GetItemData(AddItem.nItemID);
	if (!pItemData) return ERROR_ITEM_NOTFOUND;
	if (pItemData->nOverlapCount <= 0) return ERROR_ITEM_FAIL;

	if (m_QuestInventory[nIndex].nItemID > 0){	// 이미 무언가 있을때(추가냐아니냐)
		if (m_QuestInventory[nIndex].nItemID != AddItem.nItemID) return ERROR_ITEM_NOTFOUND;	// 아이템이 같지않다.
		if (pItemData->nOverlapCount == 1) return ERROR_ITEM_UNCOUNTABLE;	// 겹치는 아이템이 아니다
		if (m_QuestInventory[nIndex].wCount + AddItem.wCount > pItemData->nOverlapCount) return ERROR_ITEM_OVERFLOW;	// 아이템 개수가 넘어갔다

		m_QuestInventory[nIndex].wCount += AddItem.wCount;
	}
	else {	// 걍 빈칸
		m_QuestInventory[nIndex] = AddItem;
	}

	m_pSession->GetQuest()->OnAddItem(AddItem.nItemID, AddItem.wCount);

	return ERROR_NONE;
}

int CDNUserItem::_PopQuestInventorySlot(int nIndex, short wCount)
{
	if (!_CheckRangeQuestInventoryIndex(nIndex)) return ERROR_ITEM_INDEX_UNMATCH;
	if (wCount <= 0) return ERROR_ITEM_NOTFOUND;

	if (m_QuestInventory[nIndex].nItemID <= 0) return ERROR_ITEM_NOTFOUND;	// 아이템이 없다
	if (m_QuestInventory[nIndex].wCount < wCount) return ERROR_ITEM_OVERFLOW;		// 가지고 있는것보다 더 왔다

	m_QuestInventory[nIndex].wCount -= wCount;
	if (m_QuestInventory[nIndex].wCount <= 0)
		memset(&m_QuestInventory[nIndex], 0, sizeof(TQuestItem));

	return ERROR_NONE;
}

int CDNUserItem::FindQuestInventorySlot(int nItemID, short wCount)
{
	if ((nItemID <= 0) ||(wCount <= 0)) return -1;

	for (int i = 0; i < QUESTINVENTORYMAX; i++){
		if ((m_QuestInventory[i].nItemID == nItemID) && (m_QuestInventory[i].wCount >= wCount))
			return i;
	}

	return -1;
}

bool CDNUserItem::IsValidSpaceQuestInventorySlot(int nItemID, short wCount)
{
	if ((nItemID <= 0) ||(wCount <= 0)) return false;

	TItemData *pItem = g_pDataManager->GetItemData(nItemID);
	if (!pItem) return false;
	if (pItem->nOverlapCount <= 0) return false;

	int nCount = 0;
	if (pItem->nOverlapCount == 1){		// 겹치지 않는 아이템
		nCount = FindBlankQuestInventorySlotCount();
		if (wCount > nCount) return false;		// 빈슬롯이랑 비교하기
	}
	else {	// 겹치는 아이템
		int nRemain = 0, nBlank = 0, nBundle = 0;

		nCount = FindBlankQuestInventorySlotCount();	// 빈 슬롯개수

		if (pItem->nOverlapCount >= wCount){
			nRemain = wCount;
		}
		else {
			nBundle = wCount / pItem->nOverlapCount;
			nRemain = wCount % pItem->nOverlapCount;

			if (nBundle > nCount) return false;	// 공간부족
		}

		if (nRemain > 0){
			nBlank = FindOverlapQuestInventorySlot(nItemID, nRemain);	// 짜투리가 들어갈 공간이 있는지
			if (nBlank < 0){	// 짜투리 공간은 없다
				if ((nBundle + 1) > nCount) return false;	// 한칸의 여유가 더 있는지...
			}
		}
	}

	return true;
}

int CDNUserItem::FindBlankQuestInventorySlot()
{
	for (int i = 0; i < QUESTINVENTORYMAX; i++){
		if (m_QuestInventory[i].nItemID == 0) return i;
	}

	return -1;
}

int CDNUserItem::FindOverlapQuestInventorySlot(int nItemID, short wCount)
{
	if ((nItemID <= 0) ||(wCount <= 0)) return -1;

	bool boFlag = false;
	TItemData *pQuestItem = NULL;
	for (int i = 0; i < QUESTINVENTORYMAX; i++){
		if (m_QuestInventory[i].nItemID == 0) continue;	// 빈칸이면 패스
		if (m_QuestInventory[i].nItemID != nItemID) continue;	// 같은 아이템이 아니다

		pQuestItem = g_pDataManager->GetItemData(m_QuestInventory[i].nItemID);
		if (!pQuestItem) continue;
		if (pQuestItem->nOverlapCount <= 0) continue;
		if (pQuestItem->nOverlapCount == 1) continue;		// 겹치지 않으면 패스

		if (pQuestItem->nOverlapCount >= m_QuestInventory[i].wCount + wCount){
			return i;
		}
	}

	return -1;
}

// Vehicle Equip
int CDNUserItem::_PushVehicleEquipSlot(int nIndex, const TItem &AddItem )
{
	if ((nIndex < 0) ||(nIndex >= Vehicle::Slot::Max)) return ERROR_ITEM_INDEX_UNMATCH;
	if (AddItem.nItemID <= 0) return ERROR_ITEM_NOTFOUND;

	if (!IsVehicleEquipItem(nIndex, m_VehicleEquip.Vehicle[nIndex].nItemID)) return ERROR_ITEM_EQUIPMISMATCH;

	if (m_VehicleEquip.Vehicle[nIndex].nItemID > 0){		// 이미 무언가 있다(추가냐 아니냐만 판단하면 될듯)
		if (m_VehicleEquip.Vehicle[nIndex].nItemID != AddItem.nItemID) return ERROR_ITEM_FAIL;	// 아이템이 같지않다.

		int nOverlapCount = g_pDataManager->GetItemOverlapCount(m_VehicleEquip.Vehicle[nIndex].nItemID);
		if (nOverlapCount <= 0) return ERROR_ITEM_FAIL;
		if (nOverlapCount == 1) return ERROR_ITEM_UNCOUNTABLE;	// 겹치는 아이템이 아니다
		if (m_VehicleEquip.Vehicle[nIndex].wCount + AddItem.wCount > nOverlapCount) return ERROR_ITEM_OVERFLOW;	// 아이템 개수가 넘어갔다

		m_VehicleEquip.Vehicle[nIndex].wCount += AddItem.wCount;
	}
	else {	// 빈칸이다.
		m_VehicleEquip.Vehicle[nIndex] = AddItem;
	}

	BroadcastChangeVehicleParts(nIndex, m_VehicleEquip.Vehicle[nIndex]);

	return ERROR_NONE;
}

int CDNUserItem::_PopVehicleEquipSlot(int nIndex, short wCount, bool bSend/*=true*/ )
{
	if ((nIndex < 0) ||(nIndex >= Vehicle::Slot::Max)) return ERROR_ITEM_INDEX_UNMATCH;
	if (wCount <= 0) return ERROR_ITEM_NOTFOUND;

	if (m_VehicleEquip.Vehicle[nIndex].nItemID <= 0) return ERROR_ITEM_NOTFOUND;	// 아이템이 없다
	if (m_VehicleEquip.Vehicle[nIndex].wCount < wCount) return ERROR_ITEM_OVERFLOW;		// 가지고 있는것보다 더 왔다

	m_VehicleEquip.Vehicle[nIndex].wCount -= wCount;
	if (m_VehicleEquip.Vehicle[nIndex].wCount <= 0) 
		memset(&m_VehicleEquip.Vehicle[nIndex], 0, sizeof(TItem));

	if( bSend )
		BroadcastChangeVehicleParts(nIndex, m_VehicleEquip.Vehicle[nIndex]);

	return ERROR_NONE;
}

bool CDNUserItem::IsVehicleEquipItem(int nSlotIndex, int nItemID)
{
	TItemData *pItemData = g_pDataManager->GetItemData(nItemID);
	if (!pItemData) return false;

	if (m_pSession->GetLevel() < pItemData->cLevelLimit) return false;

	switch(pItemData->nType)
	{
	case ITEMTYPE_VEHICLE:
	case ITEMTYPE_PET:
		{
			TVehicleData *pVehicleData = g_pDataManager->GetVehicleData(nItemID);
			if (!pVehicleData) return false;
		}
		break;

	case ITEMTYPE_VEHICLEPARTS:
		{
			TVehiclePartsData *pVehiclePartsData = g_pDataManager->GetVehiclePartsData(nItemID);
			if (!pVehiclePartsData) return false;
			if (pVehiclePartsData->nVehiclePartsType != nSlotIndex) return false;

			int nClassID = g_pDataManager->GetVehicleClassID(m_VehicleEquip.Vehicle[Vehicle::Slot::Body].nItemID);
			if (pVehiclePartsData->nVehicleClassID != nClassID) return false;
		}
		break;
	case ITEMTYPE_PETPARTS:
		{
			TVehiclePartsData *pVehiclePartsData = g_pDataManager->GetVehiclePartsData(nItemID);
			if (!pVehiclePartsData) return false;
			if (pVehiclePartsData->nVehiclePartsType != nSlotIndex) return false;
		}
		break;
	}

	return true;
}

bool CDNUserItem::ChangeVehicleDyeColor(const int *pTypeParam)
{
	DWORD dwColor = pTypeParam[0];
	if (m_VehicleEquip.dwPartsColor1 == dwColor) return false;
	m_VehicleEquip.dwPartsColor1 = dwColor;

	BroadcastChangeVehiclePetColor(m_VehicleEquip.Vehicle[Vehicle::Slot::Body].nSerial, dwColor, Vehicle::Parts::BodyColor);
	m_pSession->GetDBConnection()->QueryModPetSkinColor(m_pSession, m_VehicleEquip.Vehicle[Vehicle::Slot::Body].nSerial, m_VehicleEquip.dwPartsColor1, 0);

	return true;
}

bool CDNUserItem::ChangePetBodyColor(const int *pTypeParam)
{
	DWORD dwColor = pTypeParam[0];
	if (m_PetEquip.dwPartsColor1 == dwColor) return false;
	m_PetEquip.dwPartsColor1 = dwColor;

	BroadcastChangeVehiclePetColor( m_PetEquip.Vehicle[Pet::Slot::Body].nSerial, dwColor, Pet::Parts::BodyColor );
	m_pSession->GetDBConnection()->QueryModPetSkinColor(m_pSession, m_PetEquip.Vehicle[Pet::Slot::Body].nSerial, m_PetEquip.dwPartsColor1, m_PetEquip.dwPartsColor2);

	return true;
}

bool CDNUserItem::ChangePetNoseColor(const int *pTypeParam)
{
	DWORD dwColor = pTypeParam[0];
	if (m_PetEquip.dwPartsColor2 == dwColor) return false;
	m_PetEquip.dwPartsColor2 = dwColor;

	BroadcastChangeVehiclePetColor( m_PetEquip.Vehicle[Pet::Slot::Body].nSerial, dwColor, Pet::Parts::NoseColor );
	m_pSession->GetDBConnection()->QueryModPetSkinColor(m_pSession, m_PetEquip.Vehicle[Pet::Slot::Body].nSerial, m_PetEquip.dwPartsColor1, m_PetEquip.dwPartsColor2);

	return true;
}

bool CDNUserItem::AddPetExp(int nAddPetExp)
{
	if (nAddPetExp == 0) return false;
	if (!GetPetEquip()) return false;

	// Check Max Level : 
	TPetLevelDetail *pPetData = g_pDataManager->GetPetLevelDetail(GetPetBodyItemID(), GetPetExp());
	if( NULL == pPetData ) return false;
	if( pPetData->nPetExp == 0 ) return false; //Max Lv 인 경우엔 경험치 값이 0 임.	
	
	m_PetEquip.nExp += nAddPetExp;
	m_pSession->SendAddPetExp(m_pSession->GetSessionID(), m_PetEquip.Vehicle[Pet::Slot::Body].nSerial, m_PetEquip.nExp);

	int nPrevPetLevel = pPetData->nPetLevel;
	pPetData = g_pDataManager->GetPetLevelDetail(GetPetBodyItemID(), GetPetExp());

	if( nPrevPetLevel < pPetData->nPetLevel )	
		m_pSession->GetEventSystem()->OnEvent( EventSystem::OnPetLevelUp );	

#if defined(_GAMESERVER)
	if( nPrevPetLevel < pPetData->nPetLevel )
	{
		// 펫 스텟 적용
		DnActorHandle hActor = m_pSession->GetActorHandle();
		CDnPlayerActor *pPlayer = static_cast<CDnPlayerActor *>( hActor.GetPointer() );
		if( pPlayer )
			pPlayer->RefreshState();
	}
#endif
	return true;
}

void CDNUserItem::BroadcastChangeVehicleParts(int nSlotIndex, const TItem &EquipItem)
{
#if defined(_VILLAGESERVER)
	if (!m_pSession) return;

	m_pSession->GetParamData()->nIndex = nSlotIndex;
	m_pSession->GetParamData()->Vehicle = EquipItem;

	m_pSession->SendUserLocalMessage(0, FM_CHANGEVEHICLE);

#elif defined(_GAMESERVER)
	if (!m_pSession->GetGameRoom()) return;

	for(DWORD i = 0; i < m_pSession->GetGameRoom()->GetUserCount(); i++) {
		CDNGameRoom::PartyStruct *pStruct = m_pSession->GetGameRoom()->GetPartyData(i);
		if( pStruct == NULL ) continue;
		pStruct->pSession->SendChangeVehicleParts(m_pSession->GetSessionID(), nSlotIndex, EquipItem);
	}
#endif
}

void CDNUserItem::BroadcastChangeVehiclePetColor(INT64 biSerial, DWORD dwColor, char cPetPartsColor)
{
#if defined(_VILLAGESERVER)
	if (!m_pSession) return;

	m_pSession->GetParamData()->dwColor = dwColor;
	m_pSession->GetParamData()->cPetPartsType = cPetPartsColor;
	m_pSession->GetParamData()->Vehicle.nSerial = biSerial;
	m_pSession->SendUserLocalMessage(0, FM_CHANGEVEHICLECOLOR);

#elif defined(_GAMESERVER)
	if (!m_pSession->GetGameRoom()) return;

	for(DWORD i = 0; i < m_pSession->GetGameRoom()->GetUserCount(); i++) {
		CDNGameRoom::PartyStruct *pStruct = m_pSession->GetGameRoom()->GetPartyData(i);
		if( pStruct == NULL ) continue;
		pStruct->pSession->SendChangeVehicleColor(m_pSession->GetSessionID(), biSerial, dwColor, cPetPartsColor);
	}
#endif
}

// Vehicle Inven
int CDNUserItem::_PushVehicleInventory(const TVehicle &AddItem)
{
	if (AddItem.Vehicle[Vehicle::Slot::Body].nItemID <= 0) return ERROR_ITEM_NOTFOUND;

	TItemData *pItemData = g_pDataManager->GetItemData(AddItem.Vehicle[Vehicle::Slot::Body].nItemID);
	if (!pItemData) return ERROR_ITEM_FAIL;
	if (!pItemData->IsCash) return ERROR_ITEM_FAIL;	// 캐쉬템 아니면 넣을 수 없다

	m_MapVehicleInventory[AddItem.Vehicle[Vehicle::Slot::Body].nSerial] = AddItem;

	m_pSession->GetEventSystem()->OnEvent( EventSystem::OnItemGain, 1, EventSystem::ItemID, AddItem.Vehicle->nItemID );

	return ERROR_NONE;
}

int CDNUserItem::_PopVehicleInventoryBySerial(INT64 biSerial)
{
	if (biSerial <= 0) return ERROR_ITEM_NOTFOUND;

	TMapVehicle::iterator iter = m_MapVehicleInventory.find(biSerial);
	if (iter == m_MapVehicleInventory.end()) return ERROR_ITEM_NOTFOUND;	// 아이템이 없다

	m_MapVehicleInventory.erase(iter);

	return ERROR_NONE;	
}

// 탈것
void CDNUserItem::LoadVehicleInventory(const TAGetPageVehicle *pVehicle)
{
	if (pVehicle->nTotalListCount == 0) return;

	if (pVehicle->wPageNumber == 1)
		m_nVehicleInventoryTotalCount = pVehicle->nTotalListCount;

	TVehicle AddVehicle;

	for (int i = 0; i < pVehicle->cCount; i++)
	{
		memset(&AddVehicle, 0, sizeof(AddVehicle));

		AddVehicle.dwPartsColor1 = pVehicle->VehicleItem[i].dwPartsColor1;
		AddVehicle.dwPartsColor2 = pVehicle->VehicleItem[i].dwPartsColor2;
		AddVehicle.nExp = pVehicle->VehicleItem[i].nExp;
		_wcscpy(AddVehicle.wszNickName, NAMELENMAX, pVehicle->VehicleItem[i].wszNickName, NAMELENMAX);
		TVehicleData* pVehicleData = g_pDataManager->GetVehicleData( pVehicle->VehicleItem[i].Vehicle[Pet::Slot::Body].nItemID );
		if( pVehicleData )
			AddVehicle.nRange = pVehicleData->nRange;

		AddVehicle.nSkillID1 = pVehicle->VehicleItem[i].nSkillID1;
		AddVehicle.nSkillID2 = pVehicle->VehicleItem[i].nSkillID2;
		AddVehicle.bSkillSlot = pVehicle->VehicleItem[i].bSkillSlot;
		AddVehicle.nCurrentSatiety = pVehicle->VehicleItem[i].nCurrentSatiety;
		if( pVehicleData && pVehicleData->nFoodID > 0 )
		{		
			// 만복도 펫일 경우 만복도 소모량 체크
			TPetFoodData* pPetFoodData = g_pDataManager->GetPetFoodData( pVehicleData->nFoodID );
			if( pPetFoodData && pVehicle->VehicleItem[i].tLastHungerModifyDate > 0 && AddVehicle.nCurrentSatiety > 0)
			{				
				CTimeSet TimeSet;
				int nSubTime = (int)(TimeSet.GetTimeT64_LC() - pVehicle->VehicleItem[i].tLastHungerModifyDate);
				int nSubSatiety = ((nSubTime/pPetFoodData->nTickTime)*pPetFoodData->nFullTic)/pPetFoodData->cFullTimeLogOut;					
				AddVehicle.nCurrentSatiety = AddVehicle.nCurrentSatiety-nSubSatiety;
				if( AddVehicle.nCurrentSatiety < 0)
					AddVehicle.nCurrentSatiety = 0;				
			}
			AddVehicle.dwLastHungerTick = timeGetTime();
			AddVehicle.nType |= Pet::Type::ePETTYPE_SATIETY;
		}
		CTimeSet TimeSet;
		bool bExpireDate = false;
		// 이미 날짜가 지난 펫인데 혹시 악세를 차고 있으면 캐쉬 인벤으로 돌려주기
		if( !pVehicle->VehicleItem[i].Vehicle[Pet::Slot::Body].bEternity && pVehicle->VehicleItem[i].Vehicle[Pet::Slot::Body].tExpireDate <= TimeSet.GetTimeT64_LC() )
			bExpireDate = true;

		for (int j = 0; j < Vehicle::Slot::Max; j++)
		{
			if( bExpireDate )
			{
				if( pVehicle->VehicleItem[i].Vehicle[j].nItemID > 0 && j>= Pet::Slot::Accessory1 )
				{
					TItem item;
					memset(&item, 0, sizeof(item));
					
					item.nItemID = pVehicle->VehicleItem[i].Vehicle[j].nItemID;
					item.wCount = pVehicle->VehicleItem[i].Vehicle[j].wCount;
					item.bEternity = pVehicle->VehicleItem[i].Vehicle[j].bEternity;
					item.nSerial = pVehicle->VehicleItem[i].Vehicle[j].nSerial;
					item.tExpireDate = pVehicle->VehicleItem[i].Vehicle[j].tExpireDate;
					item.bSoulbound = true;					

					m_pSession->GetDBConnection()->QueryChangeItemLocation(m_pSession, pVehicle->VehicleItem[i].Vehicle[j].nSerial, 0, pVehicle->VehicleItem[i].Vehicle[j].nItemID, 
						DBDNWorldDef::ItemLocation::CashInventory, 0, 1, false, 0, false, true, pVehicle->VehicleItem[i].Vehicle[Pet::Slot::Body].nSerial, j);

					_PushCashInventory(item);
					m_pSession->SendMoveCashItem(MoveType_PetPartsToCashInven, j, NULL, item.nSerial, &item, ERROR_NONE);					
					continue;
				}
			}
			AddVehicle.Vehicle[j].nItemID = pVehicle->VehicleItem[i].Vehicle[j].nItemID;
			AddVehicle.Vehicle[j].wCount = pVehicle->VehicleItem[i].Vehicle[j].wCount;
			AddVehicle.Vehicle[j].bEternity = pVehicle->VehicleItem[i].Vehicle[j].bEternity;
			AddVehicle.Vehicle[j].nSerial = pVehicle->VehicleItem[i].Vehicle[j].nSerial;
			AddVehicle.Vehicle[j].tExpireDate = pVehicle->VehicleItem[i].Vehicle[j].tExpireDate;
			AddVehicle.Vehicle[j].bSoulbound = true;
			if( j == Pet::Slot::Body )
			{
				if( !m_PetEquip.Vehicle[j].bEternity )
					AddVehicle.nType |= Pet::Type::ePETTYPE_EXPIREDATE;
			}
		}
#if defined(PRE_ADD_MISSION_COUPON)
		if( bExpireDate && m_pSession )
			m_pSession->GetEventSystem()->OnEvent( EventSystem::OnPetExpired, 1, EventSystem::ItemID, pVehicle->VehicleItem[i].Vehicle[Pet::Slot::Body].nItemID );
#endif
		m_MapVehicleInventory[AddVehicle.Vehicle[Vehicle::Slot::Body].nSerial] = AddVehicle;
	}	
}

int CDNUserItem::GetVehicleBodyItemID()
{
	return m_VehicleEquip.Vehicle[Vehicle::Slot::Body].nItemID;
}

void CDNUserItem::CheatClearVehicleInven()
{
	if (m_MapVehicleInventory.empty()) return;

	TMapVehicle::iterator iter;
	TVehicle Inven;
	memset(&Inven, 0, sizeof(TVehicle));

	for(iter = m_MapVehicleInventory.begin(); iter != m_MapVehicleInventory.end(); ){
		Inven = iter->second;
		m_pSession->GetDBConnection()->QueryUseItem(m_pSession, DBDNWorldDef::UseItem::Use, Inven.Vehicle[Vehicle::Slot::Body].nSerial, Inven.Vehicle[Vehicle::Slot::Body].wCount, true);

		m_MapVehicleInventory.erase(iter++);
		for (int i = 0; i < Vehicle::Slot::Max; i++){
			Inven.Vehicle[i].wCount = 0;
		}
		m_pSession->SendRefreshVehicleInven(Inven, false);
	}
}

int CDNUserItem::_PushPetEquipSlot(int nIndex, const TItem &AddItem )
{
	if ((nIndex < 0) ||(nIndex >= Pet::Slot::Max)) return ERROR_ITEM_INDEX_UNMATCH;
	if (AddItem.nItemID <= 0) return ERROR_ITEM_NOTFOUND;

	if (!IsVehicleEquipItem(nIndex, m_PetEquip.Vehicle[nIndex].nItemID)) return ERROR_ITEM_EQUIPMISMATCH;

	if (m_PetEquip.Vehicle[nIndex].nItemID > 0){		// 이미 무언가 있다(추가냐 아니냐만 판단하면 될듯)
		if (m_PetEquip.Vehicle[nIndex].nItemID != AddItem.nItemID) return ERROR_ITEM_FAIL;	// 아이템이 같지않다.

		int nOverlapCount = g_pDataManager->GetItemOverlapCount(m_PetEquip.Vehicle[nIndex].nItemID);
		if (nOverlapCount <= 0) return ERROR_ITEM_FAIL;
		if (nOverlapCount == 1) return ERROR_ITEM_UNCOUNTABLE;	// 겹치는 아이템이 아니다
		if (m_PetEquip.Vehicle[nIndex].wCount + AddItem.wCount > nOverlapCount) return ERROR_ITEM_OVERFLOW;	// 아이템 개수가 넘어갔다

		m_PetEquip.Vehicle[nIndex].wCount += AddItem.wCount;
	}
	else {	// 빈칸이다.
		m_PetEquip.Vehicle[nIndex] = AddItem;
	}

	BroadcastChangePetParts(nIndex, m_PetEquip.Vehicle[nIndex]);

	return ERROR_NONE;
}

int CDNUserItem::_PopPetEquipSlot(int nIndex, short wCount, bool bSend/*=true*/ )
{
	if ((nIndex < 0) ||(nIndex >= Pet::Slot::Max)) return ERROR_ITEM_INDEX_UNMATCH;

	if (m_PetEquip.Vehicle[nIndex].nItemID <= 0) return ERROR_ITEM_NOTFOUND;	// 아이템이 없다
	if (m_PetEquip.Vehicle[nIndex].wCount < wCount) return ERROR_ITEM_OVERFLOW;		// 가지고 있는것보다 더 왔다

	m_PetEquip.Vehicle[nIndex].wCount -= wCount;
	if (m_PetEquip.Vehicle[nIndex].wCount <= 0) 
		memset(&m_PetEquip.Vehicle[nIndex], 0, sizeof(TItem));

	if( bSend )
		BroadcastChangePetParts(nIndex, m_PetEquip.Vehicle[nIndex]);

	return ERROR_NONE;
}

#if defined(PRE_ADD_SERVER_WAREHOUSE)
int CDNUserItem::_PushServerWare(INT64 biSerial, const TItem &AddItem)
{	
	if (AddItem.nItemID <= 0) return ERROR_ITEM_NOTFOUND;

	TItemData *pItemData = g_pDataManager->GetItemData(AddItem.nItemID);
	if (!pItemData) return ERROR_ITEM_FAIL;
	if (pItemData->nOverlapCount <= 0) return ERROR_ITEM_FAIL;

	TItem* pWare = NULL;
	if( biSerial > 0 )
		pWare = const_cast<TItem*>(GetServerWare(biSerial));

	if ( pWare )
	{		// 이미 무언가 있다(추가냐 아니냐만 판단하면 될듯)
		if (pWare->nItemID != AddItem.nItemID) return ERROR_ITEM_FAIL;	// 아이템이 같지않다.
		if ((!pWare->bEternity) && (pWare->tExpireDate != AddItem.tExpireDate)) return ERROR_ITEM_FAIL;

		if ((pItemData->nOverlapCount == 1) && (pWare->nRandomSeed > 0) && (pWare->nRandomSeed != AddItem.nRandomSeed)) return ERROR_ITEM_FAIL;	// randomseed가 맞지않다.
		if (pItemData->nOverlapCount == 1) return ERROR_ITEM_UNCOUNTABLE;	// 겹치는 아이템이 아니다
		if (pWare->wCount + AddItem.wCount > pItemData->nOverlapCount) return ERROR_ITEM_OVERFLOW;	// 아이템 개수가 넘어갔다

		pWare->wCount += AddItem.wCount;
	}
	else {	// 빈칸이다.
		m_MapServerWarehouse[AddItem.nSerial] = AddItem;		
	}

	if (pItemData->nType == ITEMTYPE_NORMAL && pWare)
		pWare->nRandomSeed = 0;
	return ERROR_NONE;
}

int CDNUserItem::_PopServerWareBySerial(INT64 biSerial, short wCount)
{
	if (biSerial <= 0) return ERROR_ITEM_NOTFOUND;
	if (wCount <= 0) return ERROR_ITEM_NOTFOUND;

	TMapItem::iterator iter = m_MapServerWarehouse.find(biSerial);
	if (iter == m_MapServerWarehouse.end()) return ERROR_ITEM_NOTFOUND;	// 아이템이 없다
	if (iter->second.wCount < wCount)	return ERROR_ITEM_OVERFLOW;		// 지우려는것보다 적다

	iter->second.wCount -= wCount;
	if (iter->second.wCount <= 0)
		m_MapServerWarehouse.erase(iter);	

	return ERROR_NONE;
}

int CDNUserItem::_PushServerWareCash(const TItem &AddItem)
{
	if (AddItem.nItemID <= 0) return ERROR_ITEM_NOTFOUND;

	TItemData *pItemData = g_pDataManager->GetItemData(AddItem.nItemID);
	if (!pItemData) return ERROR_ITEM_FAIL;
	if (!pItemData->IsCash) return ERROR_ITEM_FAIL;	// 캐쉬템 아니면 넣을 수 없다

	m_MapServerWarehouseCash[AddItem.nSerial] = AddItem;
	if (pItemData->nType == ITEMTYPE_NORMAL)
		m_MapServerWarehouseCash[AddItem.nSerial].nRandomSeed = 0;	

	return ERROR_NONE;
}

int CDNUserItem::_PopServerWareCashBySerial(INT64 biSerial, short wCount)
{
	if (biSerial <= 0) return ERROR_ITEM_NOTFOUND;
	if (wCount <= 0) return ERROR_ITEM_NOTFOUND;

	TMapItem::iterator iter = m_MapServerWarehouseCash.find(biSerial);
	if (iter == m_MapServerWarehouseCash.end()) return ERROR_ITEM_NOTFOUND;	// 아이템이 없다
	if (iter->second.wCount < wCount)	return ERROR_ITEM_OVERFLOW;		// 지우려는것보다 적다

	iter->second.wCount -= wCount;
	if (iter->second.wCount <= 0)
		m_MapServerWarehouseCash.erase(iter);	

	return ERROR_NONE;
}
#endif //#if defined(PRE_ADD_SERVER_WAREHOUSE)

#if defined(PRE_ADD_TALISMAN_SYSTEM)
int CDNUserItem::_PopTalismanSlot(int nIndex, short wCount, bool bSend )
{
	if (!_CheckRangeTalismanIndex(nIndex)) return ERROR_ITEM_INDEX_UNMATCH;
	if (wCount <= 0) return ERROR_ITEM_NOTFOUND;

	if (m_Talisman[nIndex].nItemID <= 0) return ERROR_ITEM_NOTFOUND;	// 아이템이 없다
	if (m_Talisman[nIndex].wCount < wCount) return ERROR_ITEM_OVERFLOW;		// 가지고 있는것보다 더 왔다

	m_Talisman[nIndex].wCount -= wCount;
	if (m_Talisman[nIndex].wCount <= 0) 
		memset(&m_Talisman[nIndex], 0, sizeof(TItem));

	if( bSend )
		BroadcastChangeTalisman(nIndex, m_Talisman[nIndex]);

	return ERROR_NONE;
}

int CDNUserItem::CheckTalismanSlotValidation(int nSlotIndex)
{
	TTalismanSlotData* pTalismanSlotDat = g_pDataManager->GetTalismanSlotData(nSlotIndex);
	if( !pTalismanSlotDat ) return ERROR_TALISMAN_FAIL;

	if( !pTalismanSlotDat->bService )
		return ERROR_TALISMAN_SLOT_NOT_SERVICE;

	if( m_pSession->GetLevel() < pTalismanSlotDat->nLevel )
		return ERROR_TALISMAN_SLOT_LIMITLEVEL;

	if( !IsTalismanSlotOpened(nSlotIndex) )
		return ERROR_TALISMAN_SLOT_NOT_OPEND;

	return ERROR_NONE;
}

bool CDNUserItem::SetTalismanSlotOpenFlag(int nSlotIndex, bool bFlag)
{
	if( nSlotIndex < TALISMAN_EXPANSION_START || nSlotIndex >= TALISMAN_MAX )
		return false;	
	if( nSlotIndex-TALISMAN_EXPANSION_START > TALISMAN_EXPANSION_SLOT_LIMIT )
		return false;
	if( GetBitFlag( (char*)&m_nTalismanOpenFlag, nSlotIndex-TALISMAN_EXPANSION_START) == bFlag )
		return false;
	
	SetBitFlag( (char*)&m_nTalismanOpenFlag, nSlotIndex-TALISMAN_EXPANSION_START, bFlag);	
	return true;
}

bool CDNUserItem::IsTalismanSlotOpened(int nSlotIndex)
{
	if( nSlotIndex < 0 || nSlotIndex >= TALISMAN_MAX )
		return false;

	if( nSlotIndex >= TALISMAN_CASH1 && nSlotIndex <= TALISMAN_CASH_MAX )
		return IsTalismanCashSlotEntend();

	else if( nSlotIndex >= TALISMAN_BASIC_START && nSlotIndex <= TALISMAN_BASIC_END )
		return true;

	else if( nSlotIndex >= TALISMAN_EXPANSION_START )
	{
		if( nSlotIndex-TALISMAN_EXPANSION_START > TALISMAN_EXPANSION_SLOT_LIMIT )
			return false;
		return GetBitFlag( (char*)&m_nTalismanOpenFlag, nSlotIndex-TALISMAN_EXPANSION_START);
	}
	
	return false;
}

void CDNUserItem::BroadcastChangeTalisman(int nSlotIndex, const TItem &TalismanItem)
{
	if (!m_pSession) return;

#if defined(_VILLAGESERVER)
	m_pSession->GetParamData()->ItemInfo.cSlotIndex = nSlotIndex;
	m_pSession->GetParamData()->ItemInfo.Item = TalismanItem;
	m_pSession->SendUserLocalMessage(0, FM_CHANGETALISMAN);

#elif defined(_GAMESERVER)
	if (!m_pSession->GetGameRoom()) return;

	TItemInfo ItemInfo;
	ItemInfo.cSlotIndex = nSlotIndex;
	ItemInfo.Item = TalismanItem;

	for(DWORD i = 0; i < m_pSession->GetGameRoom()->GetUserCount(); i++) {
		CDNGameRoom::PartyStruct *pStruct = m_pSession->GetGameRoom()->GetPartyData(i);
		if( pStruct == NULL ) continue;
		if (pStruct->pSession != m_pSession)
			pStruct->pSession->SendChangeTalisman(m_pSession->GetSessionID(), ItemInfo);
	}
#endif
}

int CDNUserItem::OnRecvOpenTalismanSlot(const CSOpenTalismanSlot* pPacket)
{
#if defined(_GAMESERVER)
	if( !m_pSession->GetGameRoom() ) return ERROR_TALISMAN_FAIL_LOACTE;
	if( CDnWorld::GetInstance( m_pSession->GetGameRoom() ).GetMapType() != EWorldEnum::MapTypeWorldMap )
		return ERROR_TALISMAN_FAIL_LOACTE;
#endif

	if( pPacket->nSlotIndex < TALISMAN_EXPANSION_START || pPacket->nSlotIndex >= TALISMAN_MAX )
		return ERROR_TALISMAN_FAIL;
	if( IsTalismanSlotOpened(pPacket->nSlotIndex) )	//이미 열려있음
		return ERROR_TALISMAN_SLOT_ALERADY_OPEN;
	TTalismanSlotData* pTalismanSlot = g_pDataManager->GetTalismanSlotData(pPacket->nSlotIndex);
	if(!pTalismanSlot || !pTalismanSlot->bService)
		return ERROR_TALISMAN_FAIL;
	if( !g_pDataManager->IsTalismanSlotLevel(pPacket->nSlotIndex, m_pSession->GetLevel()) )
		return ERROR_TALISMAN_SLOT_LIMITLEVEL;
	if(pTalismanSlot->nType == TALISMAN_CASH_EXPANSION) //캐쉬슬롯은 EffectItems 으로 처리
		return ERROR_TALISMAN_FAIL;

	if( pTalismanSlot->nAmount > 0 && !m_pSession->CheckEnoughCoin(pTalismanSlot->nAmount) )
		return ERROR_ITEM_INSUFFICIENCY_MONEY;	
	if( pTalismanSlot->nItem > 0 && !CheckEnoughItem( pTalismanSlot->nItem, pTalismanSlot->nNeedItemCount ) )
		return ERROR_ITEM_INSUFFICIENCY_ITEM;
	
	// 아이템 먼저 삭제해 주고
	if( pTalismanSlot->nItem > 0 && !DeleteInventoryByItemID( pTalismanSlot->nItem, pTalismanSlot->nNeedItemCount, DBDNWorldDef::UseItem::Use, pPacket->nSlotIndex ) )
		_DANGER_POINT();
	if( pTalismanSlot->nAmount > 0 )
		m_pSession->DelCoin(pTalismanSlot->nAmount, DBDNWorldDef::CoinChangeCode::TalismanSlotOpen, pPacket->nSlotIndex);

	SetTalismanSlotOpenFlag(pPacket->nSlotIndex, true);	
	m_pSession->GetDBConnection()->QueryModTalismanSlotOpenFlag(m_pSession, GetTalismanSlotOpenFlag() );
	m_pSession->SendOpenTalismanSlot(ERROR_NONE, m_nTalismanOpenFlag);

	return ERROR_NONE;
}

#endif	//#if defined(PRE_ADD_TALISMAN_SYSTEM)

void CDNUserItem::BroadcastChangePetParts(int nSlotIndex, TItem &EquipItem)
{
#if defined(_VILLAGESERVER)
	if (!m_pSession) return;

	m_pSession->GetParamData()->nIndex = nSlotIndex;
	m_pSession->GetParamData()->Vehicle = EquipItem;

	m_pSession->SendUserLocalMessage(0, FM_CHANGEPET);

#elif defined(_GAMESERVER)
	if (!m_pSession->GetGameRoom()) return;

	for(DWORD i = 0; i < m_pSession->GetGameRoom()->GetUserCount(); i++) {
		CDNGameRoom::PartyStruct *pStruct = m_pSession->GetGameRoom()->GetPartyData(i);
		if ( pStruct == NULL ) continue;
		pStruct->pSession->SendChangePetParts(m_pSession->GetSessionID(), nSlotIndex, EquipItem);
	}
#endif
}

void CDNUserItem::BroadcastChangePetBody(const TVehicle &PetInfo)
{
	if (!m_pSession) return;

	TVehicleCompact Pet;
	Pet.SetCompact(PetInfo);

#if defined(_VILLAGESERVER)
	m_pSession->GetParamData()->PetInfo = Pet;
	m_pSession->SendUserLocalMessage(0, FM_CHANGEPETBODY);

#elif defined(_GAMESERVER)
	if (!m_pSession->GetGameRoom()) return;

	for(DWORD i = 0; i < m_pSession->GetGameRoom()->GetUserCount(); i++) {
		CDNGameRoom::PartyStruct *pStruct = m_pSession->GetGameRoom()->GetPartyData(i);
		if( pStruct == NULL ) continue;
		pStruct->pSession->SendChangePetBody(m_pSession->GetSessionID(), Pet);
	}
#endif
}

int CDNUserItem::ChangePetName(const TAChangePetName *pPacket)
{
	const TVehicle* pPet = NULL;
	if (pPacket->itemSerial > 0)
	{
#if defined (_GAMESERVER)
		return ERROR_GENERIC_UNKNOWNERROR;

#elif defined (_VILLAGESERVER)
		const TItem* pItem = GetCashInventory(pPacket->itemSerial);
		if (!pItem)
			return ERROR_GENERIC_UNKNOWNERROR;

		if (!DeleteItemByUse(ITEMPOSITION_CASHINVEN, -1, pPacket->itemSerial))
			return ERROR_GENERIC_UNKNOWNERROR;

		pPet = GetPetEquip();
		if (pPet)
		{
			_wcscpy(m_pSession->GetParamData()->PetInfo.wszNickName, NAMELENMAX, pPacket->name, NAMELENMAX);
			m_pSession->SendUserLocalMessage(0, FM_RENAME_PET);
		}
#endif
	}
	else
		pPet = GetVehicleInventory(pPacket->petSerial);

	TVehicle *pRenamePet = (TVehicle *)pPet;

	if (pRenamePet)
		_wcscpy(pRenamePet->wszNickName, NAMELENMAX, pPacket->name, NAMELENMAX);

	return ERROR_NONE;
}

void CDNUserItem::ModPetExpireDate(const TAModItemExpireDate *pPacket)
{
	const TVehicle *pVehicle = GetVehicleInventory(pPacket->biItemSerial);
	if (pVehicle)
	{
		time_t Time;
		time(&Time);

		TVehicle *pExpirePet = (TVehicle *)pVehicle;
		pExpirePet->Vehicle[Pet::Slot::Body].tExpireDate = Time + pPacket->nMin * 60; // 60 곱한건 초로 환산하기 위함임

		m_pSession->SendRefreshVehicleInven(*pExpirePet);
	}
}

int CDNUserItem::GetPetLevel()
{	
	int nPetLevel = 0;
	if( m_PetEquip.Vehicle[Pet::Slot::Body].nItemID > 0 )
	{
		TPetLevelDetail *pPetData = g_pDataManager->GetPetLevelDetail( m_PetEquip.Vehicle[Pet::Slot::Body].nItemID , m_PetEquip.nExp );
		if(pPetData)
			nPetLevel = pPetData->nPetLevel;
	}	
#if defined(_VILLAGESERVER)	
	TMapVehicle::const_iterator iter = m_MapVehicleInventory.begin();	
	for(;iter != m_MapVehicleInventory.end();iter++)
	{
		TItemData *pPetItemData = g_pDataManager->GetItemData( iter->second.Vehicle[Vehicle::Slot::Body].nItemID );
		if ( pPetItemData && pPetItemData->nType == ITEMTYPE_PET )
		{
			TPetLevelDetail *pPetData = g_pDataManager->GetPetLevelDetail( iter->second.Vehicle[Vehicle::Slot::Body].nItemID , iter->second.nExp );
			if(pPetData)
			{
				if(nPetLevel < pPetData->nPetLevel )
					nPetLevel = pPetData->nPetLevel;
			}			
		}
	}	
#endif
	return nPetLevel;	
}

void CDNUserItem::SetUnionMembership( BYTE cType, int nItemID, __time64_t tExpireDate )
{
	if (NpcReputation::UnionType::Commercial > cType || cType >= NpcReputation::UnionType::Etc)
		return;

	CTimeSet TimeSet;
	if (tExpireDate <= TimeSet.GetTimeT64_LC())
		return;

	m_UnionMembership[cType].cType = cType;
	m_UnionMembership[cType].nItemID = nItemID;
	m_UnionMembership[cType].tExpireDate = tExpireDate;
}

TUnionMembership* CDNUserItem::GetUnionMembership( BYTE cType )
{
	if (NpcReputation::UnionType::Commercial > cType || cType >= NpcReputation::UnionType::Etc)
		return NULL;
	
	return &m_UnionMembership[cType];
}

// exchange
int CDNUserItem::_FindBlankExchangeIndex()
{
	for (int i = 0; i < EXCHANGEMAX; i++){
		if (m_ExchangeData[i].nInvenIndex == -1) return i;
	}

	return -1;
}

bool CDNUserItem::_ExistExchangeInven(int nInvenIndex)
{
	for (int i = 0; i < EXCHANGEMAX; i++){
		if (m_ExchangeData[i].nInvenIndex == nInvenIndex) return true;
	}

	return false;
}

// cooltime
void CDNUserItem::_SetItemCoolTime( const int nSkillID, const int nMaxCoolTime )
{
	if( m_CoolTime.AddCoolTime( nSkillID, timeGetTime()+nMaxCoolTime ) < 0 )
		_DANGER_POINT();

#if defined(PRE_PERIOD_INVENTORY)
	for (int i = 0; i < INVENTORYTOTALMAX; i++){
		if (i == GetInventoryCount()){
			if (IsEnablePeriodInventory())
				i = INVENTORYMAX;
			else
				break;
		}
#else	// #if defined(PRE_PERIOD_INVENTORY)
	for (int i = 0; i < GetInventoryCount(); i++){
#endif	// #if defined(PRE_PERIOD_INVENTORY)
		if( m_Inventory[i].nItemID <= 0 ) 
			continue;

		TItemData* pItemData = g_pDataManager->GetItemData( m_Inventory[i].nItemID );
		if( pItemData && pItemData->nSkillID == nSkillID ){
			m_Inventory[i].nCoolTime = nMaxCoolTime;
		}
	}

#if defined(PRE_PERIOD_INVENTORY)
	for (int i = 0; i < WAREHOUSETOTALMAX; i++){
		if (i == GetWarehouseCount()){
			if (IsEnablePeriodWarehouse())
				i = WAREHOUSEMAX;
			else
				break;
		}
#else	// #if defined(PRE_PERIOD_INVENTORY)
	for (int i = 0; i < GetWarehouseCount(); i++){
#endif	// #if defined(PRE_PERIOD_INVENTORY)
		if( m_Warehouse[i].nItemID <= 0 ) 
			continue;

		TItemData* pItemData = g_pDataManager->GetItemData( m_Warehouse[i].nItemID );
		if( pItemData && pItemData->nSkillID == nSkillID ){
			m_Warehouse[i].nCoolTime = nMaxCoolTime;
		}
	}
}

void CDNUserItem::_UpdateInventoryCoolTime( const int nIndex, const DWORD dwCompareTime )
{
	if (!_CheckRangeInventoryIndex(nIndex))
		return;
	
	TItemData* pItemData = g_pDataManager->GetItemData( m_Inventory[nIndex].nItemID );
	if ( pItemData == NULL )
		return;

	DWORD dwUpdateTime = (dwCompareTime==0) ? timeGetTime() : dwCompareTime;
	DWORD dwEndTime	= m_CoolTime.Update( pItemData->nSkillID, dwUpdateTime );	
	if( dwEndTime == 0 )
		m_Inventory[nIndex].nCoolTime = 0;
	else
		m_Inventory[nIndex].nCoolTime = (dwEndTime-dwUpdateTime);
}

void CDNUserItem::_UpdateCashInventoryCoolTime( const INT64 biSerial, const DWORD dwCompareTime/*=0*/ )
{
	if (biSerial <= 0) return;

	const TItem *pCashInven = GetCashInventory(biSerial);
	if (!pCashInven) return;

	TItemData* pItemData = g_pDataManager->GetItemData( pCashInven->nItemID );
	if ( pItemData == NULL )
		return;

	DWORD dwUpdateTime = (dwCompareTime==0) ? timeGetTime() : dwCompareTime;
	DWORD dwEndTime	= m_CoolTime.Update( pItemData->nSkillID, dwUpdateTime );	
	if( dwEndTime == 0 )
		const_cast<TItem*>(pCashInven)->nCoolTime = 0;
	else
		const_cast<TItem*>(pCashInven)->nCoolTime = (dwEndTime-dwUpdateTime);
}

void CDNUserItem::_UpdateWarehouseCoolTime( const int nIndex, const DWORD dwCompareTime )
{
	if (!_CheckRangeWarehouseIndex(nIndex))
		return;
	
	TItemData* pItemData = g_pDataManager->GetItemData( m_Warehouse[nIndex].nItemID );
	if( pItemData == NULL )
		return;

	DWORD dwUpdateTime = (dwCompareTime==0) ? timeGetTime() : dwCompareTime;
	DWORD dwEndTime	= m_CoolTime.Update( pItemData->nSkillID, dwUpdateTime );	
	if( dwEndTime == 0 )
		m_Warehouse[nIndex].nCoolTime = 0;
	else
		m_Warehouse[nIndex].nCoolTime = (dwEndTime-dwUpdateTime);
}

#if defined( PRE_ITEMBUFF_COOLTIME )

void CDNUserItem::GetCashInventoryCoolTime(DBPacket::TItemCoolTime* CashInventoryCoolTime)
{
	int nIndex = 0;
	for( TMapItem::iterator itor = m_MapCashInventory.begin(); itor != m_MapCashInventory.end(); itor++ )
	{
		TItemData* pItemData = g_pDataManager->GetItemData( itor->second.nItemID );
		if ( pItemData == NULL )
			return;

		if( pItemData->nType != ITEMTYPE_GLOBAL_PARTY_BUFF)
			continue;

		DWORD dwUpdateTime	= timeGetTime();
		DWORD dwEndTime		= m_CoolTime.Update( pItemData->nSkillID, dwUpdateTime );

		if( dwEndTime != 0 )
		{
			CashInventoryCoolTime[nIndex].biItemSerial = itor->second.nSerial;
			CashInventoryCoolTime[nIndex].nCoolTime = itor->second.nCoolTime;
			nIndex++;
			if( nIndex >= CASHINVENTORYDBMAX)
				return;
		}
	}
}

#if defined(PRE_ADD_SERVER_WAREHOUSE)

void CDNUserItem::GetServerWareCoolTime(DBPacket::TItemCoolTime* SeverWareCoolTime)
{
	int nIndex = 0;
	for( TMapItem::iterator itor = m_MapServerWarehouse.begin(); itor != m_MapServerWarehouse.end(); itor++ )
	{
		TItemData* pItemData = g_pDataManager->GetItemData( itor->second.nItemID );
		if ( pItemData == NULL )
			return;

		if( pItemData->nType != ITEMTYPE_GLOBAL_PARTY_BUFF)
			continue;

		DWORD dwUpdateTime = timeGetTime();
		DWORD dwEndTime	= m_CoolTime.Update( pItemData->nSkillID, dwUpdateTime );

		if( dwEndTime != 0 )
		{
			SeverWareCoolTime[nIndex].biItemSerial = itor->second.nSerial;
			SeverWareCoolTime[nIndex].nCoolTime = itor->second.nCoolTime;
			nIndex++;
			if( nIndex >= WAREHOUSEMAX )
				return;			
		}
	}
}

void CDNUserItem::GetServerWareCashCoolTime(DBPacket::TItemCoolTime* ServerWareCashCoolTime)
{
	int nIndex = 0;
	for( TMapItem::iterator itor = m_MapServerWarehouseCash.begin(); itor != m_MapServerWarehouseCash.end(); itor++ )
	{
		TItemData* pItemData = g_pDataManager->GetItemData( itor->second.nItemID );
		if ( pItemData == NULL )
			return;

		if( pItemData->nType != ITEMTYPE_GLOBAL_PARTY_BUFF)
			continue;

		DWORD dwUpdateTime = timeGetTime();
		DWORD dwEndTime	= m_CoolTime.Update( pItemData->nSkillID, dwUpdateTime );

		if( dwEndTime != 0 )
		{
			ServerWareCashCoolTime[nIndex].biItemSerial = itor->second.nSerial;
			ServerWareCashCoolTime[nIndex].nCoolTime = itor->second.nCoolTime;
			nIndex++;
			if( nIndex >= WAREHOUSEMAX )
				return;
		}
	}
}
#endif // #if defined( PRE_ITEMBUFF_COOLTIME )
#endif // #if defined(PRE_ADD_SERVER_WAREHOUSE)

// Repair(내구도)
int CDNUserItem::_CalcRepairEquipPrice()
{
	float fTotalPrice = 0.f;
	float fPrice = 0.f;

	float fEnchantRevision = CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::RepairDurabilityRevision );
	int nMainType = 0, nDetailType = 0;
	for (int i = 0; i < EQUIPMAX; i++){
		if (m_Equip[i].nItemID <= 0) continue;

		nMainType = g_pDataManager->GetItemMainType(m_Equip[i].nItemID);
		nDetailType = g_pDataManager->GetItemDetailType(m_Equip[i].nItemID);

		if ((nMainType == ITEMTYPE_WEAPON) ||((nMainType == ITEMTYPE_PARTS) && ((nDetailType >= PARTS_HELMET) && (nDetailType <= PARTS_FOOT)))){
			int nGapDur = g_pDataManager->GetItemDurability(m_Equip[i].nItemID) - m_Equip[i].wDur;	// 최대내구도 - 현재내구도
			if (nGapDur > 0){
				float fRatio = ( 1.f / g_pDataManager->GetItemDurability(m_Equip[i].nItemID) ) * (float)nGapDur;
				fPrice = g_pDataManager->GetItemDurabilityRepairCoin(m_Equip[i].nItemID) * fRatio;
				// 현재 내구도가 0일 경우
				if (m_Equip[i].wDur == 0) 
					fPrice *= 1.3f;
				// 강화 단계에 따라 수리비 차등
				if( m_Equip[i].cLevel > 0 )
					fPrice *= pow( fEnchantRevision, m_Equip[i].cLevel );

				if( fPrice != 0.f && fPrice < 1.f ) fPrice = 1.f;
				fTotalPrice += fPrice;
			}
		}
	}

	if( fTotalPrice != 0.f && fTotalPrice < 1.f ) return 1;
	return(int)fTotalPrice;
}

int CDNUserItem::_CalcRepairInvenPrice()
{
	float fTotalPrice = 0.f;
	float fPrice = 0.f;

	float fEnchantRevision = CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::RepairDurabilityRevision );

	int nMainType = 0, nDetailType = 0;
#if defined(PRE_PERIOD_INVENTORY)
	for (int i = 0; i < INVENTORYTOTALMAX; i++){
		if (i == GetInventoryCount()){
			if (IsEnablePeriodInventory())
				i = INVENTORYMAX;
			else
				break;
		}
#else	// #if defined(PRE_PERIOD_INVENTORY)
	for (int i = 0; i < GetInventoryCount(); i++){
#endif	// #if defined(PRE_PERIOD_INVENTORY)
		if (m_Inventory[i].nItemID <= 0) continue;

		nMainType = g_pDataManager->GetItemMainType(m_Inventory[i].nItemID);
		nDetailType = g_pDataManager->GetItemDetailType(m_Inventory[i].nItemID);

		if ((nMainType == ITEMTYPE_WEAPON) ||((nMainType == ITEMTYPE_PARTS) && ((nDetailType >= PARTS_HELMET) && (nDetailType <= PARTS_FOOT)))){
			int nGapDur = g_pDataManager->GetItemDurability(m_Inventory[i].nItemID) - m_Inventory[i].wDur;	// 최대내구도 - 현재내구도
			if (nGapDur > 0){
				float fRatio = ( 1.f / g_pDataManager->GetItemDurability(m_Inventory[i].nItemID) ) * (float)nGapDur;
				fPrice = g_pDataManager->GetItemDurabilityRepairCoin(m_Inventory[i].nItemID) * fRatio;
				// 현재 내구도가 0일 경우
				if (m_Inventory[i].wDur == 0)
					fPrice *= 1.3f;
				// 강화 단계에 따라 수리비 차등
				if( m_Inventory[i].cLevel > 0 )
					fPrice *= pow( fEnchantRevision, m_Inventory[i].cLevel );

				if( fPrice != 0.f && fPrice < 1.f ) fPrice = 1.f;
				fTotalPrice += fPrice;
			}
		}
	}

	if( fTotalPrice != 0.f && fTotalPrice < 1.f ) return 1;

	return (int)fTotalPrice;
}

void CDNUserItem::_RepairEquip(std::vector<INT64> &VecSerialList, std::vector<USHORT> &VecDurList)
{
	int nPrice = 0;

	int nMainType = 0, nDetailType = 0;
	for (int i = 0; i < EQUIPMAX; i++){
		if (m_Equip[i].nItemID <= 0) continue;

		nMainType = g_pDataManager->GetItemMainType(m_Equip[i].nItemID);
		nDetailType = g_pDataManager->GetItemDetailType(m_Equip[i].nItemID);

		if ((nMainType == ITEMTYPE_WEAPON) ||((nMainType == ITEMTYPE_PARTS) && ((nDetailType >= PARTS_HELMET) && (nDetailType <= PARTS_FOOT)))){
			int nMaxDur = g_pDataManager->GetItemDurability(m_Equip[i].nItemID);
			int nGapDur = nMaxDur - m_Equip[i].wDur;	// 최대내구도 - 현재내구도
			if (nGapDur > 0){
				SetEquipItemDurability(i, nMaxDur);
				m_pSession->SendRefreshEquip(i, &m_Equip[i]);

				VecSerialList.push_back(m_Equip[i].nSerial);
				VecDurList.push_back(m_Equip[i].wDur);
			}
		}
	}
}

void CDNUserItem::_RepairInven(std::vector<INT64> &VecSerialList, std::vector<USHORT> &VecDurList)
{
	int nPrice = 0;

	int nMainType = 0, nDetailType = 0;
#if defined(PRE_PERIOD_INVENTORY)
	for (int i = 0; i < INVENTORYTOTALMAX; i++){
		if (i == GetInventoryCount()){
			if (IsEnablePeriodInventory())
				i = INVENTORYMAX;
			else
				break;
		}
#else	// #if defined(PRE_PERIOD_INVENTORY)
	for (int i = 0; i < GetInventoryCount(); i++){
#endif	// #if defined(PRE_PERIOD_INVENTORY)
		if (m_Inventory[i].nItemID <= 0) continue;

		nMainType = g_pDataManager->GetItemMainType(m_Inventory[i].nItemID);
		nDetailType = g_pDataManager->GetItemDetailType(m_Inventory[i].nItemID);

		if ((nMainType == ITEMTYPE_WEAPON) ||((nMainType == ITEMTYPE_PARTS) && ((nDetailType >= PARTS_HELMET) && (nDetailType <= PARTS_FOOT)))){
			int nMaxDur = g_pDataManager->GetItemDurability(m_Inventory[i].nItemID);
			int nGapDur = nMaxDur - m_Inventory[i].wDur;	// 최대내구도 - 현재내구도
			if (nGapDur > 0){
				SetInvenItemDurability(i, nMaxDur);
				m_pSession->SendRefreshInven(i, &m_Inventory[i], false);

				VecSerialList.push_back(m_Inventory[i].nSerial);
				VecDurList.push_back(m_Inventory[i].wDur);
			}
		}
	}
}

bool CDNUserItem::_CheckRangeEquipIndex(int nIndex) const
{
	if ((nIndex < 0) || (nIndex >= EQUIPMAX)) return false;
	return true;
}

bool CDNUserItem::_CheckRangeInventoryIndex(int nIndex) const
{
#if defined(PRE_PERIOD_INVENTORY)
	if (m_bEnablePeriodInventory){
		if ((nIndex >= INVENTORYMAX) && (nIndex < INVENTORYTOTALMAX)) return true;
	}
#endif	// #if defined(PRE_PERIOD_INVENTORY)
	
	if ((nIndex < 0) || (nIndex >= GetInventoryCount())) return false;
	return true;
}

bool CDNUserItem::_CheckRangeWarehouseIndex(int nIndex) const
{
#if defined(PRE_PERIOD_INVENTORY)
	if (m_bEnablePeriodWarehouse){
		if ((nIndex >= WAREHOUSEMAX) && (nIndex < WAREHOUSETOTALMAX)) return true;
	}
#endif	// #if defined(PRE_PERIOD_INVENTORY)

	if ((nIndex < 0) || (nIndex >= GetWarehouseCount())) return false;
	return true;
}

bool CDNUserItem::_CheckRangeQuestInventoryIndex(int nIndex) const
{
	if ((nIndex < 0) || (nIndex >= QUESTINVENTORYMAX)) return false;
	return true;
}

bool CDNUserItem::_CheckRangeGlyphIndex(int nIndex) const
{
	if ((nIndex < 0) || (nIndex >= GLYPHMAX)) return false;
	return true;
}

bool CDNUserItem::_CheckRangeCashEquipIndex(int nIndex) const
{
	if ((nIndex < 0) || (nIndex >= CASHEQUIPMAX)) return false;
	return true;
}

bool CDNUserItem::_CheckRangeCashGlyphIndex(int nIndex) const
{
#if defined(PRE_ADD_DRAGON_FELLOWSHIP_GLYPH)
	if( nIndex >= GLYPH_CASH1 && nIndex <= GLYPH_CASH3 )
		return true;
	return false;
#else
	if ((nIndex < GLYPH_CASH1) || (nIndex > GLYPH_CASH3)) return false;
	return true;
#endif //#if defined(PRE_ADD_DRAGON_FELLOWSHIP_GLYPH)
}

bool CDNUserItem::_CheckRangeVehicleBodyIndex(int nIndex) const
{
	if (nIndex != Vehicle::Slot::Body) return false;
	return true;
}

bool CDNUserItem::_CheckRangeVehiclePartsIndex(int nIndex) const
{
	if ((nIndex < Vehicle::Slot::Saddle) || (nIndex >= Vehicle::Slot::Max)) return false;
	return true;
}

bool CDNUserItem::_CheckRangePetBodyIndex(int nIndex) const
{
	if (nIndex != Pet::Slot::Body) return false;
	return true;
}

bool CDNUserItem::_CheckRangePetPartsIndex(int nIndex) const
{
	if ((nIndex < Pet::Slot::Accessory1) || (nIndex >= Pet::Slot::Max)) return false;
	return true;
}

#if defined(PRE_ADD_SERVER_WAREHOUSE)
bool CDNUserItem::_CheckRangeServerWareHouseIndex(int nIndex) const
{
	if ((nIndex < 0) || (nIndex >= WAREHOUSEMAX)) return false;
	return true;
}

bool CDNUserItem::_CheckRangeServerWareHouseCashIndex(int nIndex) const
{
	if ((nIndex < 0) || (nIndex >= WAREHOUSEMAX)) return false;
	return true;}
#endif //#if defined(PRE_ADD_SERVER_WAREHOUSE)

#if defined(PRE_ADD_TALISMAN_SYSTEM)
bool CDNUserItem::_CheckRangeTalismanIndex(int nIndex) const
{
	if ((nIndex < 0) || (nIndex >= TALISMAN_MAX)) return false;
	return true;
}
#endif

#if defined(PRE_PERIOD_INVENTORY)
bool CDNUserItem::_CheckRangePeriodInventoryIndex(int nIndex) const
{
	if (!IsEnablePeriodInventory()) return false;
	if ((nIndex >= INVENTORYMAX) && (nIndex < INVENTORYTOTALMAX)) return true;
	return false;
}

bool CDNUserItem::_CheckRangePeriodWarehouseIndex(int nIndex) const
{
	if (!IsEnablePeriodWarehouse()) return false;
	if ((nIndex >= WAREHOUSEMAX) && (nIndex < WAREHOUSETOTALMAX)) return true;
	return false;
}
#endif	// #if defined(PRE_PERIOD_INVENTORY)

int CDNUserItem::_IsMoveEnableItem(const TItem *pSrcItem, const CSMoveItem *pMove)
{
	if (!pSrcItem) return ERROR_ITEM_NOTFOUND;	// 아이템 존재하지않음

	if (pMove->wCount <= 0) return ERROR_ITEM_NOTFOUND;

	if (pSrcItem->nItemID <= 0) return ERROR_ITEM_NOTFOUND;
	if (pSrcItem->wCount < pMove->wCount) return ERROR_ITEM_OVERFLOW;		// 가지고 있는 것보다 더 많은 값이 오면 안됨
	if (pSrcItem->nSerial != pMove->biSrcItemSerial) return ERROR_ITEM_NOTFOUND;

	return ERROR_NONE;
}

int CDNUserItem::_Equip(const CSMoveItem *pMove)
{
	if (!_CheckRangeEquipIndex(pMove->cSrcIndex)) return ERROR_ITEM_INDEX_UNMATCH;	// 인덱스가 이상하다
	if (!_CheckRangeEquipIndex(pMove->cDestIndex)) return ERROR_ITEM_INDEX_UNMATCH;	// 옮길 곳 인덱스가 이상하다

	if (pMove->wCount <= 0) return ERROR_ITEM_NOTFOUND;	// 카운트가 이상하다
	if (pMove->cSrcIndex == pMove->cDestIndex) return ERROR_ITEM_INDEX_UNMATCH;	// 인덱스가 이상하다
#if defined(PRE_ADD_EQUIPLOCK)
	if (IsLockItem(DBDNWorldDef::ItemLocation::Equip, pMove->cSrcIndex)) return ERROR_ITEM_LOCKITEM_NOTMOVE;
	if (IsLockItem(DBDNWorldDef::ItemLocation::Equip, pMove->cDestIndex)) return ERROR_ITEM_LOCKITEM_NOTMOVE;
#endif	// #if defined(PRE_ADD_EQUIPLOCK)

	// 걍 둘을 싹~ 바꿔준다.(equip은 서로 더해주고 뭐해줄 필요 없다. 걍 바꿔치기만 하면 된다)
	int nRet = _SwapEquipToEquip(pMove->cSrcIndex, pMove->cDestIndex);	// 이안에 디비저장
	if (nRet != ERROR_NONE) return nRet;

	m_pSession->SendMoveItem(pMove->cMoveType, pMove->cSrcIndex, pMove->cDestIndex, &m_Equip[pMove->cSrcIndex], &m_Equip[pMove->cDestIndex], ERROR_NONE);
	return ERROR_NONE;
}

int CDNUserItem::_Inven(const CSMoveItem *pMove)
{
	if (!_CheckRangeInventoryIndex(pMove->cSrcIndex)) return ERROR_ITEM_INDEX_UNMATCH;	// 인덱스가 제대로 맞는지
	if (!_CheckRangeInventoryIndex(pMove->cDestIndex)) return ERROR_ITEM_INDEX_UNMATCH;	// 놓을 곳에 인덱스가 제대로 맞는지

	int nRet = _IsMoveEnableItem(&(m_Inventory[pMove->cSrcIndex]), pMove);
	if (nRet != ERROR_NONE) return nRet;

	int nSrcOverlapCount = g_pDataManager->GetItemOverlapCount(m_Inventory[pMove->cSrcIndex].nItemID);
	if (nSrcOverlapCount <= 0) return ERROR_ITEM_FAIL;

	if (nSrcOverlapCount > 1){	// 겹치는 아이템
		TItem AddItem = m_Inventory[pMove->cSrcIndex];	// 넣어줄 아이템 일단 카피
		TItem SrcBack = m_Inventory[pMove->cSrcIndex];	// src쪽 Backup(1. src지워주고 dest실패할때 다시 복구해야함, 2. 아이템 이동시킬때 써야함)

		char cInventoryLocationCode = DBDNWorldDef::ItemLocation::Inventory;
#if defined(PRE_PERIOD_INVENTORY)
		if (_CheckRangePeriodInventoryIndex(pMove->cDestIndex))
			cInventoryLocationCode = DBDNWorldDef::ItemLocation::PeriodInventory;
#endif	// #if defined(PRE_PERIOD_INVENTORY)

		if (m_Inventory[pMove->cDestIndex].nItemID > 0){		// 이미 뭔가 있다.
			int nDestOverlapCount = g_pDataManager->GetItemOverlapCount(m_Inventory[pMove->cDestIndex].nItemID);

			if( CDNUserItem::bIsDifferentItem( &m_Inventory[pMove->cSrcIndex], &m_Inventory[pMove->cDestIndex] ) == true ){	// 같은 아이템도 아니다.
				if (_SwapInvenToInven(pMove->cSrcIndex, pMove->cDestIndex) != ERROR_NONE) return ERROR_ITEM_NOTFOUND;	// 안에서 디비저장
			}
			else {
				if (m_Inventory[pMove->cDestIndex].wCount == nDestOverlapCount){	// 인벤에 있는 아이템이 max값이라면 걍 swap시킨다.
					if (_SwapInvenToInven(pMove->cSrcIndex, pMove->cDestIndex) != ERROR_NONE) return ERROR_ITEM_NOTFOUND;	// 안에서 디비저장
				}
				else {
					if ((m_Inventory[pMove->cDestIndex].wCount == nDestOverlapCount) ||(pMove->wCount == nDestOverlapCount)){	// 맥스값과 같은 아이템이 있다면 자리만 바꾼다
						if (_SwapInvenToInven(pMove->cSrcIndex, pMove->cDestIndex) != ERROR_NONE) return ERROR_ITEM_NOTFOUND;	// 안에서 디비저장
					}
					else {
						if (m_Inventory[pMove->cDestIndex].wCount + pMove->wCount > nDestOverlapCount){		// 맥스치보다 크다?
							int nGap = nDestOverlapCount - m_Inventory[pMove->cDestIndex].wCount;

							if (nGap > 0){
								INT64 nBeforeSerial = AddItem.nSerial;
								if (_PopInventorySlotItem(pMove->cSrcIndex, nGap) != ERROR_NONE) return ERROR_ITEM_FAIL;	// 먼저 빼주고

								INT64 nNewSerial = 0;
								AddItem.wCount = nGap;
								AddItem.nSerial = m_Inventory[pMove->cDestIndex].nSerial;
								if (m_Inventory[pMove->cSrcIndex].wCount > 0){
									nNewSerial = MakeItemSerial();
								}

								if (_PushInventorySlotItem(pMove->cDestIndex, AddItem) != ERROR_NONE){	// 넣어주고
									m_Inventory[pMove->cSrcIndex] = SrcBack;	// 먼저 빼준거 복구해주자
									return ERROR_ITEM_FAIL;
								}

								m_pSession->GetDBConnection()->QueryChangeItemLocation(m_pSession, nBeforeSerial, nNewSerial, AddItem.nItemID, 
									cInventoryLocationCode, pMove->cDestIndex, nGap, true, AddItem.nSerial, false); 
							}
						}
						else {	// 맥스치보다 작다.
							INT64 nBeforeSerial = AddItem.nSerial;
							int nBeforeCount = AddItem.wCount;
							if (_PopInventorySlotItem(pMove->cSrcIndex, pMove->wCount) != ERROR_NONE) return ERROR_ITEM_FAIL;	// 빼고

							AddItem.wCount = pMove->wCount;
							AddItem.nSerial = m_Inventory[pMove->cDestIndex].nSerial;

							if (_PushInventorySlotItem(pMove->cDestIndex, AddItem) != ERROR_NONE){	// 넣고
								m_Inventory[pMove->cSrcIndex] = SrcBack;	// 먼저 빼준거 복구해주자
								return ERROR_ITEM_FAIL;
							}

							INT64 biNewSerial = 0;
							bool bMergeAll = false;
							if (nBeforeCount == pMove->wCount)
								bMergeAll = true;
							else
								biNewSerial = MakeItemSerial();	// split할 때 시리얼 새로 만들어야함 (디비에서 사용)

							m_pSession->GetDBConnection()->QueryChangeItemLocation(m_pSession, nBeforeSerial, biNewSerial, AddItem.nItemID, 
								cInventoryLocationCode, pMove->cDestIndex, pMove->wCount, true, AddItem.nSerial, bMergeAll); 
						}
					}
				}
			}
		}
		else {	// 아무것도 없는 빈칸이다.
			if (m_Inventory[pMove->cSrcIndex].wCount == pMove->wCount){	// 걍 이동
				if (_SwapInvenToInven(pMove->cSrcIndex, pMove->cDestIndex) != ERROR_NONE) return ERROR_ITEM_NOTFOUND;	// 안에서 디비저장
			}
			else {		// 새로 쪼개는 거니 씨리얼 생성해주고
				INT64 nBeforeSerial = AddItem.nSerial;
				if (_PopInventorySlotItem(pMove->cSrcIndex, pMove->wCount) != ERROR_NONE) return ERROR_ITEM_FAIL;

				AddItem.wCount = pMove->wCount;
				if (m_Inventory[pMove->cSrcIndex].wCount > 0){
					AddItem.nSerial = MakeItemSerial();
				}
				if (_PushInventorySlotItem(pMove->cDestIndex, AddItem) != ERROR_NONE){
					m_Inventory[pMove->cSrcIndex] = SrcBack;	// 먼저 빼준거 복구해주자
					return ERROR_ITEM_FAIL;
				}
				m_pSession->GetDBConnection()->QueryChangeItemLocation(m_pSession, nBeforeSerial, AddItem.nSerial, AddItem.nItemID, 
					cInventoryLocationCode, pMove->cDestIndex, pMove->wCount, false, 0, false); 
			}
		}
	}
	else {	// 안 겹치는 아이템(걍 서로 교환)
		if (_SwapInvenToInven(pMove->cSrcIndex, pMove->cDestIndex) != ERROR_NONE) return ERROR_ITEM_NOTFOUND;	// 안에서 디비저장
	}

	m_pSession->SendMoveItem(pMove->cMoveType, pMove->cSrcIndex, pMove->cDestIndex, &m_Inventory[pMove->cSrcIndex], &m_Inventory[pMove->cDestIndex], ERROR_NONE);

	return ERROR_NONE;
}

int CDNUserItem::_Ware(const CSMoveItem *pMove)
{
	if (!_CheckRangeWarehouseIndex(pMove->cSrcIndex)) return ERROR_ITEM_INDEX_UNMATCH;	// 인덱스가 제대로 맞는지
	if (!_CheckRangeWarehouseIndex(pMove->cDestIndex)) return ERROR_ITEM_INDEX_UNMATCH;	// 놓을 곳에 인덱스가 제대로 맞는지

	int nRet = _IsMoveEnableItem(&(m_Warehouse[pMove->cSrcIndex]), pMove);
	if (nRet != ERROR_NONE) return nRet;

	int nSrcOverlapCount = g_pDataManager->GetItemOverlapCount(m_Warehouse[pMove->cSrcIndex].nItemID);
	if (nSrcOverlapCount <= 0) return ERROR_ITEM_FAIL;

	if (nSrcOverlapCount > 1){	// 겹치는 아이템
		TItem AddItem = m_Warehouse[pMove->cSrcIndex];	// 넣어줄 아이템 일단 카피
		TItem SrcBack = m_Warehouse[pMove->cSrcIndex];	// src쪽 Backup(1. src지워주고 dest실패할때 다시 복구해야함, 2. 아이템 이동시킬때 써야함)

		char cWarehouseLocationCode = DBDNWorldDef::ItemLocation::Warehouse;
#if defined(PRE_PERIOD_INVENTORY)
		if (_CheckRangePeriodWarehouseIndex(pMove->cDestIndex))
			cWarehouseLocationCode = DBDNWorldDef::ItemLocation::PeriodWarehouse;
#endif	// #if defined(PRE_PERIOD_INVENTORY)

		if (m_Warehouse[pMove->cDestIndex].nItemID > 0){		// 이미 뭔가 있다.
			int nDestOverlapCount = g_pDataManager->GetItemOverlapCount(m_Warehouse[pMove->cDestIndex].nItemID);

			if ( CDNUserItem::bIsDifferentItem( &m_Warehouse[pMove->cSrcIndex], &m_Warehouse[pMove->cDestIndex] ) == true ){	// 같은 아이템도 아니다.
				if (_SwapWareToWare(pMove->cSrcIndex, pMove->cDestIndex) != ERROR_NONE) return ERROR_ITEM_NOTFOUND;	// 안에서 디비저장
			}
			else {
				if ((m_Warehouse[pMove->cDestIndex].wCount == nDestOverlapCount) ||(pMove->wCount == nDestOverlapCount)){	// 맥스와 같다면 자리만 바꾼다
					if (_SwapWareToWare(pMove->cSrcIndex, pMove->cDestIndex) != ERROR_NONE) return ERROR_ITEM_NOTFOUND;	// 안에서 디비저장
				}
				else {
					if (m_Warehouse[pMove->cDestIndex].wCount + pMove->wCount > nDestOverlapCount){		// 맥스치보다 크다?
						int nGap = nDestOverlapCount - m_Warehouse[pMove->cDestIndex].wCount;

						if (nGap > 0){
							INT64 nBeforeSerial = AddItem.nSerial;
							if (_PopWarehouseSlot(pMove->cSrcIndex, nGap) != ERROR_NONE) return ERROR_ITEM_FAIL;	// 먼저 빼주고

							INT64 nNewSerial = 0;
							AddItem.wCount = nGap;
							AddItem.nSerial = m_Warehouse[pMove->cDestIndex].nSerial;
							if (m_Warehouse[pMove->cSrcIndex].wCount > 0){
								nNewSerial = MakeItemSerial();
							}

							if (_PushWarehouseSlot(pMove->cDestIndex, AddItem) != ERROR_NONE){	// 넣어주고
								m_Warehouse[pMove->cSrcIndex] = SrcBack;	// 먼저 빼준거 복구해주자
								return ERROR_ITEM_FAIL;
							}
							m_pSession->GetDBConnection()->QueryChangeItemLocation(m_pSession, nBeforeSerial, nNewSerial, AddItem.nItemID, 
								cWarehouseLocationCode, pMove->cDestIndex, nGap, true, AddItem.nSerial, false);
						}
					}
					else {	// 맥스치보다 작다.
						INT64 nBeforeSerial = AddItem.nSerial;
						int nBeforeCount = AddItem.wCount;
						if (_PopWarehouseSlot(pMove->cSrcIndex, pMove->wCount) != ERROR_NONE) return ERROR_ITEM_FAIL;	// 빼고

						AddItem.wCount = pMove->wCount;
						AddItem.nSerial = m_Warehouse[pMove->cDestIndex].nSerial;

						if (_PushWarehouseSlot(pMove->cDestIndex, AddItem) != ERROR_NONE){	// 넣고
							m_Warehouse[pMove->cSrcIndex] = SrcBack;	// 먼저 빼준거 복구해주자
							return ERROR_ITEM_FAIL;
						}

						INT64 biNewSerial = 0;
						bool bMergeAll = false;
						if (nBeforeCount == pMove->wCount)
							bMergeAll = true;
						else
							biNewSerial = MakeItemSerial();	// split할 때 시리얼 새로 만들어야함 (디비에서 사용)

						m_pSession->GetDBConnection()->QueryChangeItemLocation(m_pSession, nBeforeSerial, biNewSerial, AddItem.nItemID, 
							cWarehouseLocationCode, pMove->cDestIndex, pMove->wCount, true, AddItem.nSerial, bMergeAll); 
					}
				}
			}
		}
		else {	// 아무것도 없는 빈칸이다.
			if (m_Warehouse[pMove->cSrcIndex].wCount == pMove->wCount){	// 걍 이동
				if (_SwapWareToWare(pMove->cSrcIndex, pMove->cDestIndex) != ERROR_NONE) return ERROR_ITEM_NOTFOUND;	// 안에서 디비저장
			}
			else {		// 새로 쪼개는 거니 씨리얼 생성해주고
				INT64 nBeforeSerial = AddItem.nSerial;
				if (_PopWarehouseSlot(pMove->cSrcIndex, pMove->wCount) != ERROR_NONE) return ERROR_ITEM_FAIL;

				AddItem.wCount = pMove->wCount;
				if (m_Warehouse[pMove->cSrcIndex].wCount > 0){
					AddItem.nSerial = MakeItemSerial();
				}
				if (_PushWarehouseSlot(pMove->cDestIndex, AddItem) != ERROR_NONE){
					m_Warehouse[pMove->cSrcIndex] = SrcBack;	// 먼저 빼준거 복구해주자
					return ERROR_ITEM_FAIL;
				}
				m_pSession->GetDBConnection()->QueryChangeItemLocation(m_pSession, nBeforeSerial, AddItem.nSerial, AddItem.nItemID, 
					cWarehouseLocationCode, pMove->cDestIndex, pMove->wCount, false, 0, false); 
			}
		}
	}
	else {	// 안 겹치는 아이템(걍 서로 교환)
		if (_SwapWareToWare(pMove->cSrcIndex, pMove->cDestIndex) != ERROR_NONE) return ERROR_ITEM_NOTFOUND;	// 안에서 디비저장
	}

	m_pSession->SendMoveItem(pMove->cMoveType, pMove->cSrcIndex, pMove->cDestIndex, &m_Warehouse[pMove->cSrcIndex], &m_Warehouse[pMove->cDestIndex], ERROR_NONE);
	return ERROR_NONE;
}

int CDNUserItem::_FromEquipToInven(const CSMoveItem *pMove)
{
	if (!_CheckRangeEquipIndex(pMove->cSrcIndex)) return ERROR_ITEM_INDEX_UNMATCH;	// 인덱스가 이상하다
	if (!_CheckRangeInventoryIndex(pMove->cDestIndex)) return ERROR_ITEM_INDEX_UNMATCH;	// 인덱스가 이상하다
#if defined(PRE_ADD_EQUIPLOCK)
	if (IsLockItem(DBDNWorldDef::ItemLocation::Equip, pMove->cSrcIndex)) return ERROR_ITEM_LOCKITEM_NOTMOVE;
#endif	// #if defined(PRE_ADD_EQUIPLOCK)

	const TItem *pEquip = GetEquip(pMove->cSrcIndex);
	if (!pEquip) return ERROR_ITEM_NOTFOUND;		// 아이템이 없다(옮길 필요 없음)

	int nRet = _IsMoveEnableItem(pEquip, pMove);
	if (nRet != ERROR_NONE) return nRet;

	int nEquipOverlapCount = g_pDataManager->GetItemOverlapCount(pEquip->nItemID);
	if (nEquipOverlapCount <= 0) return ERROR_ITEM_FAIL;

	const TItem *pInven = GetInventory(pMove->cDestIndex);

	char cInventoryLocationCode = DBDNWorldDef::ItemLocation::Inventory;
#if defined(PRE_PERIOD_INVENTORY)
	if (_CheckRangePeriodInventoryIndex(pMove->cDestIndex))
		cInventoryLocationCode = DBDNWorldDef::ItemLocation::PeriodInventory;
#endif	// #if defined(PRE_PERIOD_INVENTORY)

	nRet = 0;
	if (nEquipOverlapCount > 1){		// 겹치는 아이템
		if (pInven){		// 이미 무언가가 있다.
			int ret = IsEquipEnableItem(*pInven);
			if (ret != ERROR_NONE)
				return ret;
			int nInvenOverlapCount = g_pDataManager->GetItemOverlapCount(pInven->nItemID);
			if (nInvenOverlapCount <= 0) return ERROR_ITEM_FAIL;

			if (pInven->nItemID != pEquip->nItemID){	// 다른아이템
				nRet = _SwapEquipToInven(pMove->cSrcIndex, pMove->cDestIndex);		// 서로 바꿔준다.	// 안에서 디비저장
				if (nRet != ERROR_NONE) return nRet;
			}
			else {		// 같은 아이템이다
				TItem EquipBack = m_Equip[pMove->cSrcIndex];	// Equip Backup(제대로 안됐을때 복구)
				TItem AddItem = m_Equip[pMove->cSrcIndex];	// 더해줄 아이템 세팅

				if (pInven->wCount + pMove->wCount > nInvenOverlapCount){	// max를 넘어간다.
					int GapCount = nInvenOverlapCount - pInven->wCount;	// 넣을만큼만 넣어준다.
					INT64 nBeforeSerial = AddItem.nSerial;
					if (_PopEquipSlot(pMove->cSrcIndex, GapCount) != ERROR_NONE) return ERROR_ITEM_FAIL;

					INT64 nNewSerial = 0;
					AddItem.wCount = GapCount;
					AddItem.nSerial = pInven->nSerial;
					if (m_Equip[pMove->cSrcIndex].wCount > 0){
						nNewSerial = MakeItemSerial();
					}
					if (_PushInventorySlotItem(pMove->cDestIndex, AddItem) == ERROR_NONE){	// 실패
						m_Equip[pMove->cSrcIndex] = EquipBack;	// equip에 다시 복구
						return ERROR_ITEM_FAIL;
					}
					m_pSession->GetDBConnection()->QueryChangeItemLocation(m_pSession, nBeforeSerial, nNewSerial, AddItem.nItemID, 
						cInventoryLocationCode, pMove->cDestIndex, GapCount, true, AddItem.nSerial, false); 
				}
				else {	// 개수만큼 빼주면 된다
					INT64 nBeforeSerial = AddItem.nSerial;
					if (_PopEquipSlot(pMove->cSrcIndex, pMove->wCount) != ERROR_NONE) return ERROR_ITEM_FAIL;

					INT64 biNewSerial = 0;
					bool bMergeAll = false;
					if (AddItem.wCount == pMove->wCount)
						bMergeAll = true;
					else
						biNewSerial = MakeItemSerial();	// split할 때 시리얼 새로 만들어야함 (디비에서 사용)

					AddItem.wCount = pMove->wCount;
					AddItem.nSerial = pInven->nSerial;

					if (_PushInventorySlotItem(pMove->cDestIndex, AddItem) != ERROR_NONE){	// 실패
						m_Equip[pMove->cSrcIndex] = EquipBack;	// equip에 다시 복구
						return ERROR_ITEM_FAIL;
					}

					m_pSession->GetDBConnection()->QueryChangeItemLocation(m_pSession, nBeforeSerial, biNewSerial, AddItem.nItemID, 
						cInventoryLocationCode, pMove->cDestIndex, pMove->wCount, true, AddItem.nSerial, bMergeAll); 
				}
			}
		}
		else {		// 빈칸이다
			nRet = _SwapEquipToInven(pMove->cSrcIndex, pMove->cDestIndex);		// 서로 바꿔준다.	// 안에서 디비저장
			if (nRet != ERROR_NONE) return nRet;
		}
	}
	else {		// 겹치지 않는 아이템
		nRet = _SwapEquipToInven(pMove->cSrcIndex, pMove->cDestIndex);		// 서로 바꿔준다.	// 안에서 디비저장
		if (nRet != ERROR_NONE) return nRet;
	}

	m_pSession->SendMoveItem(pMove->cMoveType, pMove->cSrcIndex, pMove->cDestIndex, &m_Equip[pMove->cSrcIndex], &m_Inventory[pMove->cDestIndex], ERROR_NONE);
	CheckOneTypeCashWeapon(pMove->cSrcIndex);
	return ERROR_NONE;
}

int CDNUserItem::_FromInvenToEquip(const CSMoveItem *pMove)
{
	if (!_CheckRangeInventoryIndex(pMove->cSrcIndex)) return ERROR_ITEM_INDEX_UNMATCH;	// 인덱스가 이상하다
	if (!_CheckRangeEquipIndex(pMove->cDestIndex)) return ERROR_ITEM_INDEX_UNMATCH;	// 인덱스가 이상하다
#if defined(PRE_ADD_EQUIPLOCK) && defined(_VILLAGESERVER)
	if (IsLockItem(DBDNWorldDef::ItemLocation::Equip, pMove->cDestIndex)) return ERROR_ITEM_LOCKITEM_NOTMOVE;
#endif	// #if defined(PRE_ADD_EQUIPLOCK)

	const TItem *pInven = GetInventory(pMove->cSrcIndex);
	if (!pInven) return ERROR_ITEM_NOTFOUND;	// 아이템 존재하지않음

	int nRet = _IsMoveEnableItem(pInven, pMove);
	if (nRet != ERROR_NONE) return nRet;

	nRet = IsEquipEnableItem(*pInven);
	if (nRet != ERROR_NONE) return nRet;
	if (!IsEquipItem(pMove->cDestIndex, pInven->nItemID)) return ERROR_ITEM_FAIL;		// 장착가능한지 검사

	// PvPRank검사
	TItemData* pItemData = g_pDataManager->GetItemData( pInven->nItemID );
	if ( !pItemData )
		return ERROR_ITEM_NOTFOUND;
	if ( pItemData->nNeedPvPRank > 0 )
	{
		const TPvPGroup* pPvP = m_pSession->GetPvPData();
		if ( pPvP->cLevel < pItemData->nNeedPvPRank )
			return ERROR_ITEM_PVPLEVEL;
	}

	int nInvenOverlapCount = g_pDataManager->GetItemOverlapCount(pInven->nItemID);
	if (nInvenOverlapCount <= 0) return ERROR_ITEM_FAIL;

	const TItem *pEquip = GetEquip(pMove->cDestIndex);
#if defined(PRE_ADD_EQUIPLOCK) && defined(_GAMESERVER)
	if(IsLockItem(DBDNWorldDef::ItemLocation::Equip, pMove->cDestIndex))
	{
		//좀비 모드일때 아이템을 강제로 셋팅하기 때문에, 게임모드랑 아이템을 사용 가능한 맵타입으로 두번 검증해서 잠금 예외처리
		if( !m_pSession->GetGameRoom() || !m_pSession->GetGameRoom()->bIsZombieMode())
			return ERROR_ITEM_LOCKITEM_NOTMOVE;

		if( !pEquip ) //장비가 잠겨있는데, Equip정보가 없으면 문제가 있는것. 로그남김
		{
			g_Log.Log(LogType::_ERROR, m_pSession, L"[DNUserItem::InvenToEquip] LockItem Find Faild(EquipIdx : %d)", pMove->cDestIndex);
			return ERROR_ITEM_LOCKITEM_NOTMOVE;
		}

		TItemData* pSrcItemData = g_pDataManager->GetItemData( pEquip->nItemID );
		if(!pSrcItemData) return ERROR_ITEM_NOTFOUND;

		// 서버에서 임시 장비를 만들어서 강제로 장착하는 컨텐츠의 경우, 해당 아이템의 AllowMapType을 -1으로 값을 설정하면 안됨.
		// 임시 아이템임을 알수 있는 방법이 없기 때문에, 해당 값을 설정하는 것으로 예외처리합니다
		// #76027 장비 보안강화를 위한 장비 잠금 시스템 개발		2013-02-15  karl
		if( (pItemData->nAllowMapType == -1) && (pSrcItemData->nAllowMapType == -1) )	// 임시 생성 장비가 아닌 경우 return
			return ERROR_ITEM_LOCKITEM_NOTMOVE;
	}
#endif	// #if defined(PRE_ADD_EQUIPLOCK)
	nRet = 0;
	if (nInvenOverlapCount > 1){		// 겹치는 아이템
		if (pEquip){		// 무언가 있다
			int nEquipOverlapCount = g_pDataManager->GetItemOverlapCount(pEquip->nItemID);

			if (pEquip->nItemID != pInven->nItemID){	// 다른아이템
				nRet = _SwapInvenToEquip(pMove->cSrcIndex, pMove->cDestIndex);		// 서로 바꿔준다.	// 안에서 디비저장
				if (nRet != ERROR_NONE) return nRet;
			}
			else {		// 같은 아이템 
				TItem InvenBack = m_Inventory[pMove->cSrcIndex];		// backup
				TItem AddItem = m_Inventory[pMove->cSrcIndex];		// 넣을 아이템 세팅

				if (pEquip->wCount + pMove->wCount > nEquipOverlapCount){	// max넘김
					int GapCount = nEquipOverlapCount - pEquip->wCount;
					INT64 nBeforeSerial = AddItem.nSerial;
					nRet = _PopInventorySlotItem(pMove->cSrcIndex, GapCount);
					if (nRet != ERROR_NONE) return nRet;

					INT64 nNewSerial = 0;
					AddItem.wCount = GapCount;
					AddItem.nSerial = pEquip->nSerial;

					if (m_Inventory[pMove->cSrcIndex].wCount > 0){
						nNewSerial = MakeItemSerial();
					}
					nRet = _PushEquipSlot(pMove->cDestIndex, AddItem);
					if (nRet != ERROR_NONE){
						m_Inventory[pMove->cSrcIndex] = InvenBack;	// inven에 다시 복구
						return nRet;
					}
					m_pSession->GetDBConnection()->QueryChangeItemLocation(m_pSession, nBeforeSerial, nNewSerial, AddItem.nItemID, 
						DBDNWorldDef::ItemLocation::Equip, pMove->cDestIndex, GapCount, true, AddItem.nSerial, false); 
				}
				else {
					INT64 nBeforeSerial = AddItem.nSerial;
					nRet = _PopInventorySlotItem(pMove->cSrcIndex, pMove->wCount);
					if (nRet != ERROR_NONE) return nRet;

					INT64 biNewSerial = 0;
					bool bMergeAll = false;
					if (AddItem.wCount == pMove->wCount)
						bMergeAll = true;
					else
						biNewSerial = MakeItemSerial();	// split할 때 시리얼 새로 만들어야함 (디비에서 사용)

					AddItem.wCount = pMove->wCount;
					AddItem.nSerial = pEquip->nSerial;

					nRet = _PushEquipSlot(pMove->cDestIndex, AddItem);
					if (nRet != ERROR_NONE){
						m_Inventory[pMove->cSrcIndex] = InvenBack;	// inven에 다시 복구
						return nRet;
					}
					m_pSession->GetDBConnection()->QueryChangeItemLocation(m_pSession, nBeforeSerial, biNewSerial, AddItem.nItemID, 
						DBDNWorldDef::ItemLocation::Equip, pMove->cDestIndex, pMove->wCount, true, AddItem.nSerial, bMergeAll); 
				}
			}
		}
		else {		// 빈칸이다
			nRet = _SwapInvenToEquip(pMove->cSrcIndex, pMove->cDestIndex);		// 서로 바꿔준다.	// 안에서 디비저장
			if (nRet != ERROR_NONE) return nRet;
		}
	}
	else {		// 겹치지 않는 아이템
		nRet = _SwapInvenToEquip(pMove->cSrcIndex, pMove->cDestIndex);		// 서로 바꿔준다.	// 안에서 디비저장
		if (nRet != ERROR_NONE) return nRet;
	}

	m_pSession->SendMoveItem(pMove->cMoveType, pMove->cSrcIndex, pMove->cDestIndex, &m_Inventory[pMove->cSrcIndex], &m_Equip[pMove->cDestIndex], ERROR_NONE);
	CheckOneTypeCashWeapon(pMove->cDestIndex);
	return ERROR_NONE;
}

int CDNUserItem::_FromInvenToWare(const CSMoveItem *pMove)
{
	if (!_CheckRangeInventoryIndex(pMove->cSrcIndex)) return ERROR_ITEM_INDEX_UNMATCH;	// 인덱스가 이상하다
	if (!_CheckRangeWarehouseIndex(pMove->cDestIndex)) return ERROR_ITEM_INDEX_UNMATCH;	// 인덱스가 이상하다

	const TItem *pInven = GetInventory(pMove->cSrcIndex);
	if (!pInven) return ERROR_ITEM_NOTFOUND;	// 아이템 존재하지않음

#if defined(PRE_ADD_PCBANG_RENTAL_ITEM)
	if( g_pDataManager->IsPcCafeRentItem(pInven->nItemID) )
		return ERROR_ITEM_NOT_MOVE_TO_WARE;
#endif

	int nRet = _IsMoveEnableItem(pInven, pMove);
	if (nRet != ERROR_NONE) return nRet;

	int nInvenOverlapCount = g_pDataManager->GetItemOverlapCount(pInven->nItemID);
	if (nInvenOverlapCount <= 0) return ERROR_ITEM_FAIL;

	const TItem *pWare = GetWarehouse(pMove->cDestIndex);

	char cWarehouseLocationCode = DBDNWorldDef::ItemLocation::Warehouse;
#if defined(PRE_PERIOD_INVENTORY)
	if (_CheckRangePeriodWarehouseIndex(pMove->cDestIndex))
		cWarehouseLocationCode = DBDNWorldDef::ItemLocation::PeriodWarehouse;
#endif	// #if defined(PRE_PERIOD_INVENTORY)
	
	nRet = 0;
	if (nInvenOverlapCount > 1){	// 겹치는 아이템
		if (pWare){		// 창고 index에 무언가 있다
			int nWareOverlapCount = g_pDataManager->GetItemOverlapCount(pWare->nItemID);

			if ( CDNUserItem::bIsDifferentItem( pWare, pInven ) == true )
			{
				if (pInven->wCount == pMove->wCount){	// 그 슬롯 모두 옮기면 바꿔주고
					nRet = _SwapInvenToWare(pMove->cSrcIndex, pMove->cDestIndex);	// 서로 바꿔주기만 하면 끝	// 안에서 디비저장
					if (nRet != ERROR_NONE) return nRet;
				}
				else{	// 슬롯에 일부만 넣을꺼면 취소다
					return ERROR_ITEM_FAIL;
				}
			}
			else {		// 같은 아이템
				TItem InvenBack = m_Inventory[pMove->cSrcIndex];		// backup
				TItem AddItem = m_Inventory[pMove->cSrcIndex];		// 넣을 아이템				

				if (pWare->wCount + pMove->wCount > nWareOverlapCount){		// max값을 넘긴경우
					int nGapCount = nWareOverlapCount - pWare->wCount;
					if (nGapCount > 0){
						INT64 nBeforeSerial = AddItem.nSerial;
						if (_PopInventorySlotItem(pMove->cSrcIndex, nGapCount) != ERROR_NONE) return ERROR_ITEM_FAIL;

						INT64 nNewSerial = 0;
						AddItem.wCount = nGapCount;
						AddItem.nSerial = pWare->nSerial;
						if (m_Inventory[pMove->cSrcIndex].wCount > 0){
							nNewSerial = MakeItemSerial();
						}
						if (_PushWarehouseSlot(pMove->cDestIndex, AddItem) != ERROR_NONE){
							m_Inventory[pMove->cSrcIndex] = InvenBack;	// add하다가 오류나서 다시 복구
							return ERROR_ITEM_FAIL;
						}
						m_pSession->GetDBConnection()->QueryChangeItemLocation(m_pSession, nBeforeSerial, nNewSerial, AddItem.nItemID, 
							cWarehouseLocationCode, pMove->cDestIndex, nGapCount, true, AddItem.nSerial, false); 
					}
				}
				else {	// 걍 넣으면 됨
					INT64 nBeforeSerial = AddItem.nSerial;
					if (_PopInventorySlotItem(pMove->cSrcIndex, pMove->wCount) != ERROR_NONE) return ERROR_ITEM_FAIL;

					INT64 biNewSerial = 0;
					bool bMergeAll = false;
					if (AddItem.wCount == pMove->wCount)
						bMergeAll = true;
					else
						biNewSerial = MakeItemSerial();	// split할 때 시리얼 새로 만들어야함 (디비에서 사용)

					AddItem.wCount = pMove->wCount;
					AddItem.nSerial = pWare->nSerial;

					if (_PushWarehouseSlot(pMove->cDestIndex, AddItem) != ERROR_NONE){
						m_Inventory[pMove->cSrcIndex] = InvenBack;	// add하다가 오류나서 다시 복구
						return ERROR_ITEM_FAIL;
					}
					m_pSession->GetDBConnection()->QueryChangeItemLocation(m_pSession, nBeforeSerial, biNewSerial, AddItem.nItemID, 
						cWarehouseLocationCode, pMove->cDestIndex, pMove->wCount, true, AddItem.nSerial, bMergeAll); 
				}
			}
		}
		else {	// 아무것도 없다
			if (pInven->wCount > pMove->wCount){	// 일부만 옮긴다
				INT64 nBeforeSerial = m_Inventory[pMove->cSrcIndex].nSerial;
				TItem InvenBack = m_Inventory[pMove->cSrcIndex];		// backup

				if (_PopInventorySlotItem(pMove->cSrcIndex, pMove->wCount) != ERROR_NONE) return ERROR_ITEM_FAIL;
				
				TItem AddItem = m_Inventory[pMove->cSrcIndex];		// 넣을 아이템				

				AddItem.wCount = pMove->wCount;
				if (m_Inventory[pMove->cSrcIndex].wCount > 0){
					AddItem.nSerial = MakeItemSerial();
				}
				if (_PushWarehouseSlot(pMove->cDestIndex, AddItem) != ERROR_NONE){
					m_Inventory[pMove->cSrcIndex] = InvenBack;	// add하다가 오류나서 다시 복구
					return ERROR_ITEM_FAIL;
				}
				m_pSession->GetDBConnection()->QueryChangeItemLocation(m_pSession, nBeforeSerial, AddItem.nSerial, AddItem.nItemID, 
					cWarehouseLocationCode, pMove->cDestIndex, pMove->wCount, false, 0, false); 

			}
			else {	// 통째로 옮기는거
				nRet = _SwapInvenToWare(pMove->cSrcIndex, pMove->cDestIndex);	// 서로 바꿔주기만 하면 끝	// 안에서 디비저장
				if (nRet != ERROR_NONE) return nRet;
			}
		}
			
	}
	else {		// 안겹치는 아이템
		nRet = _SwapInvenToWare(pMove->cSrcIndex, pMove->cDestIndex);	// 서로 바꿔주기만 하면 끝	// 안에서 디비저장
		if (nRet != ERROR_NONE) return nRet;
	}

	m_pSession->SendMoveItem(pMove->cMoveType, pMove->cSrcIndex, pMove->cDestIndex, &m_Inventory[pMove->cSrcIndex], &m_Warehouse[pMove->cDestIndex], ERROR_NONE);
	return ERROR_NONE;
}

int CDNUserItem::_FromWareToInven(const CSMoveItem *pMove)
{
	if (!_CheckRangeWarehouseIndex(pMove->cSrcIndex)) return ERROR_ITEM_INDEX_UNMATCH;	// 인덱스가 이상하다
	if (!_CheckRangeInventoryIndex(pMove->cDestIndex)) return ERROR_ITEM_INDEX_UNMATCH;	// 인덱스가 이상하다

	const TItem *pWare = GetWarehouse(pMove->cSrcIndex);
	if (!pWare) return ERROR_ITEM_NOTFOUND;	// 아이템 존재하지않음

	int nRet = _IsMoveEnableItem(pWare, pMove);
	if (nRet != ERROR_NONE) return nRet;

	int nWareOverlapCount = g_pDataManager->GetItemOverlapCount(pWare->nItemID);
	if (nWareOverlapCount <= 0) return ERROR_ITEM_FAIL;

	const TItem *pInven = GetInventory(pMove->cDestIndex);

	char cInventoryLocationCode = DBDNWorldDef::ItemLocation::Inventory;
#if defined(PRE_PERIOD_INVENTORY)
	if (_CheckRangePeriodInventoryIndex(pMove->cDestIndex))
		cInventoryLocationCode = DBDNWorldDef::ItemLocation::PeriodInventory;
#endif	// #if defined(PRE_PERIOD_INVENTORY)

	nRet = 0;
	if (nWareOverlapCount > 1){	// 겹치는 아이템
		if (pInven){		// 이미 index에 뭐가 있다.
			int nInvenOverlapCount = g_pDataManager->GetItemOverlapCount(pInven->nItemID);
			if (nInvenOverlapCount <= 0) return ERROR_ITEM_FAIL;

			if ( CDNUserItem::bIsDifferentItem( pInven, pWare ) == true ){		// 같은 아이템이 아니다
				if (pWare->wCount == pMove->wCount){	// 전체 옮긴다면 바꿔주고
					nRet = _SwapWareToInven(pMove->cSrcIndex, pMove->cDestIndex);	// 안에서 디비저장
					if (nRet != ERROR_NONE) return nRet;
				}
				else{	// 일부만 옮기는건 에러
					return ERROR_ITEM_FAIL;
				}
			}
			else {	// 동일 아이템
				TItem WareBack = m_Warehouse[pMove->cSrcIndex];
				TItem AddItem = m_Warehouse[pMove->cSrcIndex];				

				if (pInven->wCount + pMove->wCount > nInvenOverlapCount){	// max를 넘겼다
					int nGapCount = nInvenOverlapCount - pInven->wCount;
					if (nGapCount > 0){
						INT64 nBeforeSerial = AddItem.nSerial;
						if (_PopWarehouseSlot(pMove->cSrcIndex, nGapCount) != ERROR_NONE) return ERROR_ITEM_FAIL;

						INT64 nNewSerial = 0;
						AddItem.wCount = nGapCount;
						AddItem.nSerial = pInven->nSerial;
						if (m_Warehouse[pMove->cSrcIndex].wCount > 0){
							nNewSerial = MakeItemSerial();
						}
						if (_PushInventorySlotItem(pMove->cDestIndex, AddItem) != ERROR_NONE){
							m_Warehouse[pMove->cSrcIndex] = WareBack;		// add하다가 오류나서 다시 복구
							return ERROR_ITEM_FAIL;
						}
						m_pSession->GetDBConnection()->QueryChangeItemLocation(m_pSession, nBeforeSerial, nNewSerial, AddItem.nItemID, 
							cInventoryLocationCode, pMove->cDestIndex, nGapCount, true, AddItem.nSerial, false); 
					}
				}
				else {	// 걍 넣으면 됨
					INT64 nBeforeSerial = AddItem.nSerial;
					if (_PopWarehouseSlot(pMove->cSrcIndex, pMove->wCount) != ERROR_NONE) return ERROR_ITEM_FAIL;

					INT64 biNewSerial = 0;
					bool bMergeAll = false;
					if (AddItem.wCount == pMove->wCount)
						bMergeAll = true;
					else
						biNewSerial = MakeItemSerial();

					AddItem.wCount = pMove->wCount;
					AddItem.nSerial = pInven->nSerial;	// split할 때 시리얼 새로 만들어야함 (디비에서 사용)

					if (_PushInventorySlotItem(pMove->cDestIndex, AddItem) != ERROR_NONE){
						m_Warehouse[pMove->cSrcIndex] = WareBack;		// add하다가 오류나서 다시 복구
						return ERROR_ITEM_FAIL;
					}
					m_pSession->GetDBConnection()->QueryChangeItemLocation(m_pSession, nBeforeSerial, biNewSerial, AddItem.nItemID, 
						cInventoryLocationCode, pMove->cDestIndex, pMove->wCount, true, AddItem.nSerial, bMergeAll); 
				}
			}
		}
		else {	// 빈칸이다
			if (pWare->wCount > pMove->wCount){	// 분리시켜서 보냈다면
				INT64 nBeforeSerial = m_Warehouse[pMove->cSrcIndex].nSerial;
				TItem WareBack = m_Warehouse[pMove->cSrcIndex];
				if (_PopWarehouseSlot(pMove->cSrcIndex, pMove->wCount) != ERROR_NONE) return ERROR_ITEM_FAIL;
				
				TItem AddItem = m_Warehouse[pMove->cSrcIndex];			

				AddItem.wCount = pMove->wCount;
				if (m_Warehouse[pMove->cSrcIndex].wCount > 0){
					AddItem.nSerial = MakeItemSerial();
				}

				if (_PushInventorySlotItem(pMove->cDestIndex, AddItem) != ERROR_NONE){
					m_Warehouse[pMove->cSrcIndex] = WareBack;		// add하다가 오류나서 다시 복구
					return ERROR_ITEM_FAIL;
				}
				m_pSession->GetDBConnection()->QueryChangeItemLocation(m_pSession, nBeforeSerial, AddItem.nSerial, AddItem.nItemID, 
					cInventoryLocationCode, pMove->cDestIndex, pMove->wCount, false, 0, false); 

			}
			else {	// 전체 옮긴다면
				nRet = _SwapWareToInven(pMove->cSrcIndex, pMove->cDestIndex);	// 안에서 디비저장
				if (nRet != ERROR_NONE) return nRet;
			}
		}
	}
	else {	// 겹치지 않는 아이템
		nRet = _SwapWareToInven(pMove->cSrcIndex, pMove->cDestIndex);	// 안에서 디비저장
		if (nRet != ERROR_NONE) return nRet;
	}

	m_pSession->SendMoveItem(pMove->cMoveType, pMove->cSrcIndex, pMove->cDestIndex, &m_Warehouse[pMove->cSrcIndex], &m_Inventory[pMove->cDestIndex], ERROR_NONE);
	return ERROR_NONE;
}

int CDNUserItem::_FromGlyphToInven(const CSMoveItem *pMove)
{
	if (!_CheckRangeGlyphIndex(pMove->cSrcIndex)) return ERROR_ITEM_INDEX_UNMATCH;	// 인덱스가 이상하다
	if (!_CheckRangeInventoryIndex(pMove->cDestIndex)) return ERROR_ITEM_INDEX_UNMATCH;	// 인덱스가 이상하다

	const TItem *pGlyph = GetGlyph(pMove->cSrcIndex);
	if (!pGlyph) return ERROR_ITEM_NOTFOUND;		// 아이템이 없다(옮길 필요 없음)

	int nRet = _IsMoveEnableItem(pGlyph, pMove);
	if (nRet != ERROR_NONE) return nRet;

	int nGlyphOverlapCount = g_pDataManager->GetItemOverlapCount(pGlyph->nItemID);
	if (nGlyphOverlapCount <= 0) return ERROR_ITEM_FAIL;
	if (nGlyphOverlapCount != 1) return ERROR_ITEM_FAIL;	// 겹치지 않는 아이템

	if (GetInventory(pMove->cDestIndex) != NULL) return ERROR_ITEM_FAIL;

	// 수수료 계산(문쟝아이템의 레벨 제한( ItemTable의 _LevelLimit ) *(문장의 타입, 문장의 등급에 따른 Glyphcharge 테이블의 _Charge값)
	char cGlyphType = g_pDataManager->GetGlyphType(pGlyph->nItemID);
	char cGlyphRank = g_pDataManager->GetItemRank(pGlyph->nItemID);
	int nCharge = g_pDataManager->GetGlyphCharge(cGlyphType, cGlyphRank) * g_pDataManager->GetItemLevelLimit(pGlyph->nItemID);
	if (nCharge <= 0) return ERROR_ITEM_FAIL;

	if (m_pSession->CheckEnoughCoin(nCharge) == false) return ERROR_ITEM_INSUFFICIENCY_MONEY;

	m_pSession->DelCoin(nCharge, DBDNWorldDef::CoinChangeCode::Use, 0, true);

	if (m_pSession->GetDBConnection()){
		char cInventoryLocationCode = DBDNWorldDef::ItemLocation::Inventory;
#if defined(PRE_PERIOD_INVENTORY)
		if (_CheckRangePeriodInventoryIndex(pMove->cDestIndex))
			cInventoryLocationCode = DBDNWorldDef::ItemLocation::PeriodInventory;
#endif	// #if defined(PRE_PERIOD_INVENTORY)

		if (m_Glyph[pMove->cSrcIndex].nItemID > 0){
			m_pSession->GetDBConnection()->QueryChangeItemLocation(m_pSession, m_Glyph[pMove->cSrcIndex].nSerial, 0, m_Glyph[pMove->cSrcIndex].nItemID, 
				cInventoryLocationCode, pMove->cDestIndex, m_Glyph[pMove->cSrcIndex].wCount, false, 0, false);
		}
	}

	TItem GlyphItem = *pGlyph;
	_PopGlyphSlot(pMove->cSrcIndex, pMove->wCount, true);
	_PushInventorySlotItem(pMove->cDestIndex, GlyphItem);

	m_pSession->SendMoveItem(pMove->cMoveType, pMove->cSrcIndex, pMove->cDestIndex, &m_Glyph[pMove->cSrcIndex], &m_Inventory[pMove->cDestIndex], ERROR_NONE);

	return ERROR_NONE;
}

int CDNUserItem::_FromInvenToGlyph(const CSMoveItem *pMove)
{
	if (!_CheckRangeInventoryIndex(pMove->cSrcIndex)) return ERROR_ITEM_INDEX_UNMATCH;	// 인덱스가 이상하다
	if (!_CheckRangeGlyphIndex(pMove->cDestIndex)) return ERROR_ITEM_INDEX_UNMATCH;	// 인덱스가 이상하다

#if defined(PRE_ADD_DRAGON_FELLOWSHIP_GLYPH)
	if ( pMove->cDestIndex >= GLYPH_CASH1 && pMove->cDestIndex <= GLYPH_CASH3 )
	{
		TCashGlyphData* GlyphDate = GetGlyphExpireDate();
		int nIndex = pMove->cDestIndex - GLYPH_CASH1;
		if( !GlyphDate[nIndex].cActiveGlyph )
		{
			return ERROR_ITEM_INDEX_UNMATCH;
		}		
	}
#else
	if ( pMove->cDestIndex >= GLYPH_CASH1 )
	{
		int nIndex = pMove->cDestIndex - GLYPH_CASH1;
		if( !GlyphDate[nIndex].cActiveGlyph )
		{
			return ERROR_ITEM_INDEX_UNMATCH;
		}			
	}
#endif //#if defined(PRE_ADD_DRAGON_FELLOWSHIP_GLYPH)o

	const TItem *pInven = GetInventory(pMove->cSrcIndex);
	if (!pInven) return ERROR_ITEM_NOTFOUND;	// 아이템 존재하지않음

	int nRet = _IsMoveEnableItem(pInven, pMove);
	if (nRet != ERROR_NONE) return nRet;

	int nInvenOverlapCount = g_pDataManager->GetItemOverlapCount(pInven->nItemID);
	if (nInvenOverlapCount <= 0) return ERROR_ITEM_FAIL;
	if (nInvenOverlapCount != 1) return ERROR_ITEM_FAIL;

	// 같은 타입 중복검사
	const TItemData* pGlyphItem = g_pDataManager->GetItemData( pInven->nItemID );
	for( int i=GLYPH_ENCHANT1; i<GLYPHMAX; i++ ) 
	{
		if ( m_Glyph[i].nItemID <= 0 )
			continue;

		const TItemData* pItemData = g_pDataManager->GetItemData( m_Glyph[i].nItemID );
		if( !pItemData )
			return ERROR_ITEM_INVENTOEQUIP_FAIL;
		if ( pGlyphItem && pGlyphItem->nTypeParam[1] == pItemData->nTypeParam[1] )
			return ERROR_ITEM_INVENTOEQUIP_FAIL;
	}

	if (!IsGlyphItem(pMove->cDestIndex, pInven->nItemID)) 
		return ERROR_ITEM_INDEX_UNMATCH;
	if (!g_pDataManager->IsGlyphLevel(pInven->nItemID, m_pSession->GetLevel()))
		return ERROR_GLYPH_LIMITLEVEL;
	if (!g_pDataManager->IsGlyphSlotLevel(pMove->cDestIndex, m_pSession->GetLevel()))
		return ERROR_GLYPH_SLOTLIMITLEVEL;

	if (m_Glyph[pMove->cDestIndex].nItemID > 0)	// 이미 있다면 같은 계열인지 보자
		return ERROR_ITEM_INVENTOEQUIP_FAIL;

	if (m_pSession->GetDBConnection()){
		char cInventoryLocationCode = DBDNWorldDef::ItemLocation::Inventory;
#if defined(PRE_PERIOD_INVENTORY)
		if (_CheckRangePeriodInventoryIndex(pMove->cSrcIndex))
			cInventoryLocationCode = DBDNWorldDef::ItemLocation::PeriodInventory;
#endif	// #if defined(PRE_PERIOD_INVENTORY)
		if (m_Inventory[pMove->cSrcIndex].nItemID > 0){
			m_pSession->GetDBConnection()->QueryChangeItemLocation(m_pSession, m_Inventory[pMove->cSrcIndex].nSerial, 0, m_Inventory[pMove->cSrcIndex].nItemID, 
				DBDNWorldDef::ItemLocation::Glyph, pMove->cDestIndex, m_Inventory[pMove->cSrcIndex].wCount, false, 0, false);
		}
		if (m_Glyph[pMove->cDestIndex].nItemID > 0){
			m_pSession->GetDBConnection()->QueryChangeItemLocation(m_pSession, m_Glyph[pMove->cDestIndex].nSerial, 0, m_Glyph[pMove->cDestIndex].nItemID, 
				cInventoryLocationCode, pMove->cSrcIndex, m_Glyph[pMove->cDestIndex].wCount, false, 0, false);
		}
	}

#if defined(_VILLAGESERVER)
	if ( m_Glyph[pMove->cDestIndex].nItemID > 0 ) 
		m_pSession->GetSkill()->OnDetachEquip( &m_Glyph[pMove->cDestIndex] );
#endif

	TItem Inven = m_Inventory[pMove->cSrcIndex];

	m_Inventory[pMove->cSrcIndex] = m_Glyph[pMove->cDestIndex];
	_UpdateInventoryCoolTime(pMove->cSrcIndex);

	m_Glyph[pMove->cDestIndex] = Inven;

#if defined(_VILLAGESERVER)
	if ( m_Glyph[pMove->cDestIndex].nItemID > 0 ) 
		m_pSession->GetSkill()->OnAttachEquip( &m_Glyph[pMove->cDestIndex] );
#endif

	BroadcastChangeGlyph(pMove->cDestIndex, m_Glyph[pMove->cDestIndex]);

	m_pSession->SendMoveItem(pMove->cMoveType, pMove->cSrcIndex, pMove->cDestIndex, &m_Inventory[pMove->cSrcIndex], &m_Glyph[pMove->cDestIndex], ERROR_NONE);
	return ERROR_NONE;
}

#if defined(PRE_ADD_TALISMAN_SYSTEM)
int CDNUserItem::_Talisman(const CSMoveItem *pMove)
{
#if defined(_GAMESERVER)
	if( !m_pSession->GetGameRoom() ) return ERROR_TALISMAN_FAIL_LOACTE;
	if( CDnWorld::GetInstance( m_pSession->GetGameRoom() ).GetMapType() != EWorldEnum::MapTypeWorldMap )
		return ERROR_TALISMAN_FAIL_LOACTE;	
#endif
	if(!_CheckRangeTalismanIndex(pMove->cSrcIndex)) return ERROR_ITEM_INDEX_UNMATCH;	// 인덱스가 이상하다
	if(!_CheckRangeTalismanIndex(pMove->cDestIndex)) return ERROR_ITEM_INDEX_UNMATCH;	// 인덱스가 이상하다

	if(pMove->wCount <= 0) return ERROR_ITEM_NOTFOUND;	// 카운트가 이상하다
	if(pMove->cSrcIndex == pMove->cDestIndex) return ERROR_ITEM_INDEX_UNMATCH;	// 인덱스가 이상하다

	// 수수료 계산
	int nCharge = g_pDataManager->GetTalismanCostFromPlayerCommonLevelTable(m_pSession->GetLevel());
	if(nCharge <= 0) return ERROR_ITEM_FAIL;

	if(m_pSession->CheckEnoughCoin(nCharge) == false) return ERROR_ITEM_INSUFFICIENCY_MONEY;
	int nRet = _SwapTalismanToTalisman(pMove->cSrcIndex, pMove->cDestIndex);	// 이안에 디비저장
	if(nRet != ERROR_NONE) return nRet;

	m_pSession->DelCoin(nCharge, DBDNWorldDef::CoinChangeCode::TalismanSlotChange, 0, true);
	m_pSession->SendMoveItem(pMove->cMoveType, pMove->cSrcIndex, pMove->cDestIndex, &m_Talisman[pMove->cSrcIndex], &m_Talisman[pMove->cDestIndex], ERROR_NONE);
	return ERROR_NONE;
}

int CDNUserItem::_FromTalismanToInven(const CSMoveItem *pMove)
{
#if defined(_GAMESERVER)
	if( !m_pSession->GetGameRoom() ) return ERROR_TALISMAN_FAIL_LOACTE;	
	if( CDnWorld::GetInstance( m_pSession->GetGameRoom() ).GetMapType() != EWorldEnum::MapTypeWorldMap )
		return ERROR_TALISMAN_FAIL_LOACTE;	
#endif

	if(!_CheckRangeTalismanIndex(pMove->cSrcIndex)) return ERROR_ITEM_INDEX_UNMATCH;	// 인덱스가 이상하다
	if(!_CheckRangeInventoryIndex(pMove->cDestIndex)) return ERROR_ITEM_INDEX_UNMATCH;	// 인덱스가 이상하다

	const TItem *pTalisman = GetTalisman(pMove->cSrcIndex);
	if(!pTalisman) return ERROR_ITEM_NOTFOUND;		// 아이템이 없다(옮길 필요 없음)
	
	int nRet = _IsMoveEnableItem(pTalisman, pMove);
	if(nRet != ERROR_NONE) return nRet;

	int nTalismanOverlapCount = g_pDataManager->GetItemOverlapCount(pTalisman->nItemID);
	if(nTalismanOverlapCount <= 0) return ERROR_ITEM_FAIL;
	if(nTalismanOverlapCount != 1) return ERROR_ITEM_FAIL;	// 겹치지 않는 아이템

	if(GetInventory(pMove->cDestIndex) != NULL) return ERROR_ITEM_FAIL;

	const TItemData* pTalismanItem = g_pDataManager->GetItemData( pTalisman->nItemID );
	if( !pTalismanItem || pTalismanItem->nType != ITEMTYPE_TALISMAN )
		return ERROR_TALISMAN_FAIL;

	// 수수료 계산
	int nCharge = g_pDataManager->GetTalismanCostFromPlayerCommonLevelTable(m_pSession->GetLevel());
	if(nCharge <= 0) return ERROR_ITEM_FAIL;

	if(m_pSession->CheckEnoughCoin(nCharge) == false) return ERROR_ITEM_INSUFFICIENCY_MONEY;
	m_pSession->DelCoin(nCharge, DBDNWorldDef::CoinChangeCode::TalismanSlotChange, 0, true);

	if(m_pSession->GetDBConnection()){
		char cInventoryLocationCode = DBDNWorldDef::ItemLocation::Inventory;
#if defined(PRE_PERIOD_INVENTORY)
		if (_CheckRangePeriodInventoryIndex(pMove->cDestIndex))
			cInventoryLocationCode = DBDNWorldDef::ItemLocation::PeriodInventory;
#endif	// #if defined(PRE_PERIOD_INVENTORY)
		if (m_Talisman[pMove->cSrcIndex].nItemID > 0){
			m_pSession->GetDBConnection()->QueryChangeItemLocation(m_pSession, m_Talisman[pMove->cSrcIndex].nSerial, 0, m_Talisman[pMove->cSrcIndex].nItemID, 
				cInventoryLocationCode, pMove->cDestIndex, m_Talisman[pMove->cSrcIndex].wCount, false, 0, false);
		}
	}

	TItem TalismanItem = *pTalisman;
	_PopTalismanSlot(pMove->cSrcIndex, pMove->wCount, true);
	_PushInventorySlotItem(pMove->cDestIndex, TalismanItem);

	m_pSession->SendMoveItem(pMove->cMoveType, pMove->cSrcIndex, pMove->cDestIndex, &m_Talisman[pMove->cSrcIndex], &m_Inventory[pMove->cDestIndex], ERROR_NONE);
	return ERROR_NONE;
}

int CDNUserItem::_FromInvenToTalisman(const CSMoveItem *pMove)
{
	if(!_CheckRangeInventoryIndex(pMove->cSrcIndex)) return ERROR_ITEM_INDEX_UNMATCH;	// 인덱스가 이상하다
	if(!_CheckRangeTalismanIndex(pMove->cDestIndex)) return ERROR_ITEM_INDEX_UNMATCH;	// 인덱스가 이상하다

	//슬롯체크
	int nRet = CheckTalismanSlotValidation(pMove->cDestIndex);
	if(nRet != ERROR_NONE) return nRet;

	const TItem *pInven = GetInventory(pMove->cSrcIndex);
	if (!pInven) return ERROR_ITEM_NOTFOUND;	// 아이템 존재하지않음

	nRet = _IsMoveEnableItem(pInven, pMove);
	if(nRet != ERROR_NONE) return nRet;

	int nInvenOverlapCount = g_pDataManager->GetItemOverlapCount(pInven->nItemID);
	if(nInvenOverlapCount <= 0) return ERROR_ITEM_FAIL;
	if(nInvenOverlapCount != 1) return ERROR_ITEM_FAIL;

	const TItemData* pTalismanItem = g_pDataManager->GetItemData( pInven->nItemID );
	if( !pTalismanItem || pTalismanItem->nType != ITEMTYPE_TALISMAN )
		return ERROR_TALISMAN_FAIL;
	if( pTalismanItem->cLevelLimit > m_pSession->GetLevel() )
		return ERROR_TALISMAN_LIMITLEVEL;
	if( m_Talisman[pMove->cDestIndex].nItemID > 0 )
		return ERROR_TALISMAN_EQUIP_FAIL;

	// 같은 타입 중복검사 ; 캐쉬는 중복장착 가능하다면 여기서 수정해야 함
	for( int i = 0; i < TALISMAN_MAX; i++ )
	{
		if( m_Talisman[i].nItemID <= 0 )
			continue;

		const TItemData* pItemData = g_pDataManager->GetItemData( m_Talisman[i].nItemID );
		if( pTalismanItem && pTalismanItem->nTypeParam[1] == pItemData->nTypeParam[1] )
			return ERROR_TALISMAN_DUPLICATE;
	}

	if (m_pSession->GetDBConnection()){
		char cInventoryLocationCode = DBDNWorldDef::ItemLocation::Inventory;
#if defined(PRE_PERIOD_INVENTORY)
		if (_CheckRangePeriodInventoryIndex(pMove->cSrcIndex))
			cInventoryLocationCode = DBDNWorldDef::ItemLocation::PeriodInventory;
#endif	// #if defined(PRE_PERIOD_INVENTORY)

		if(m_Inventory[pMove->cSrcIndex].nItemID > 0){
			m_pSession->GetDBConnection()->QueryChangeItemLocation(m_pSession, m_Inventory[pMove->cSrcIndex].nSerial, 0, m_Inventory[pMove->cSrcIndex].nItemID, 
				DBDNWorldDef::ItemLocation::Talisman, pMove->cDestIndex, m_Inventory[pMove->cSrcIndex].wCount, false, 0, false);
		}
		if(m_Talisman[pMove->cDestIndex].nItemID > 0){
			m_pSession->GetDBConnection()->QueryChangeItemLocation(m_pSession, m_Talisman[pMove->cDestIndex].nSerial, 0, m_Talisman[pMove->cDestIndex].nItemID, 
				cInventoryLocationCode, pMove->cSrcIndex, m_Talisman[pMove->cDestIndex].wCount, false, 0, false);
		}
	}

	TItem Inven = m_Inventory[pMove->cSrcIndex];
	m_Inventory[pMove->cSrcIndex] = m_Talisman[pMove->cDestIndex];	
	
	_UpdateInventoryCoolTime(pMove->cSrcIndex);
	m_Talisman[pMove->cDestIndex] = Inven;	

	m_pSession->SendMoveItem(pMove->cMoveType, pMove->cSrcIndex, pMove->cDestIndex, &m_Inventory[pMove->cSrcIndex], &m_Talisman[pMove->cDestIndex], ERROR_NONE);
	BroadcastChangeTalisman(pMove->cDestIndex, m_Talisman[pMove->cDestIndex]);
	return ERROR_NONE;
}

int CDNUserItem::_SwapTalismanToTalisman(int nSrcIndex, int nDestIndex)
{
	if(!_CheckRangeTalismanIndex(nSrcIndex)) return ERROR_ITEM_INDEX_UNMATCH;
	if(!_CheckRangeTalismanIndex(nDestIndex)) return ERROR_ITEM_INDEX_UNMATCH;

	if( m_Talisman[nSrcIndex].nItemID <= 0 )
		return ERROR_ITEM_NOTFOUND;

	int nRet = CheckTalismanSlotValidation(nDestIndex);
	if( nRet != ERROR_NONE ) return nRet;

	if( m_Talisman[nDestIndex].nItemID > 0 )	// 슬롯 스왑
	{
		nRet = CheckTalismanSlotValidation(nSrcIndex);
		if( nRet != ERROR_NONE ) return nRet;

		m_pSession->GetDBConnection()->QuerySwitchItemLocation(m_pSession, DBDNWorldDef::ItemLocation::Talisman, 
			nSrcIndex, m_Talisman[nDestIndex].nSerial, DBDNWorldDef::ItemLocation::Talisman, nDestIndex, m_Talisman[nSrcIndex].nSerial);
	}
	else	// 빈 슬롯으로 이동
	{	// nSrcIndex 은 벨리데이션 체크하지 않음(닫힌슬롯(캐시)에서 열린 슬롯으로 이동은 가능하도록)
		m_pSession->GetDBConnection()->QueryChangeItemLocation(m_pSession, m_Talisman[nSrcIndex].nSerial, 0, m_Talisman[nSrcIndex].nItemID, 
			DBDNWorldDef::ItemLocation::Talisman, nDestIndex, m_Talisman[nSrcIndex].wCount, false, 0, false);
	}

	TItem SrcTalisman = m_Talisman[nSrcIndex];

	m_Talisman[nSrcIndex] = m_Talisman[nDestIndex];
	m_Talisman[nDestIndex] = SrcTalisman;

	BroadcastChangeTalisman(nSrcIndex, m_Talisman[nSrcIndex]);
	BroadcastChangeTalisman(nDestIndex, m_Talisman[nDestIndex]);

	return ERROR_NONE;
}
#endif

#if defined(PRE_ADD_SERVER_WAREHOUSE)
int CDNUserItem::_FromInvenToServerWareHouse(const CSMoveServerWare *pMove)
{
	const TItem *pInven = GetInventory(pMove->cInvenIndex);
	if (!pInven) return ERROR_ITEM_NOTFOUND;	// 아이템 존재하지않음

#if defined(PRE_ADD_PCBANG_RENTAL_ITEM)
	if( g_pDataManager->IsPcCafeRentItem(pInven->nItemID) )
		return ERROR_ITEM_NOT_MOVE_TO_WARE;
#endif
	if (pMove->wCount <= 0) return ERROR_ITEM_NOTFOUND;

	if (pInven->nItemID <= 0) return ERROR_ITEM_NOTFOUND;
	if (pInven->wCount < pMove->wCount) return ERROR_ITEM_OVERFLOW;		// 가지고 있는 것보다 더 많은 값이 오면 안됨
	if (pInven->nSerial != pMove->biInvenItemSerial) return ERROR_ITEM_NOTFOUND;

	//레벨 및 수수료, 아이템 검사.
	int nCheckLevel = static_cast<int>(CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::SERVER_WAREHOUSE_IN_LEVELLIMIT));
	if( nCheckLevel > m_pSession->GetLevel() )	
		return ERROR_ITEM_NOTFOUND; // 창고에 넣을수 있는 레벨이 아님..클라에서 먼저 걸러줌.
	int nPayment = static_cast<int>(CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::SERVER_WAREHOUSE_PAYMENT));	
	// 창고에 넣을 수 있는 아이템 검사
	int nGoldPee = 0;
	if( nPayment == 0 )  // 골드 수수료 모델
	{
		nGoldPee = g_pDataManager->GetTaxAmount(TAX_WSTORAGE, m_pSession->GetLevel(), 0);
		if (nGoldPee == 0 || !m_pSession->CheckEnoughCoin(nGoldPee))
			return ERROR_ITEM_NOTFOUND; // 수수료 부족
	}
	else if( nPayment == 1 ) // 유료화 아이템 모델
	{
		int nNeedItemCount = 1;
		if( g_pDataManager->GetItemMainType(pInven->nItemID) == ITEMTYPE_SERVERWARE_COIN )		
			nNeedItemCount = pMove->wCount+1;
		
		if( GetCashItemCountByType(ITEMTYPE_SERVERWARE_COIN) + GetInventoryItemCountByType(ITEMTYPE_SERVERWARE_COIN) < nNeedItemCount )
			return ERROR_ITEM_NOTFOUND; // 아이템이 없음둥.
	}

	if (!_CheckRangeInventoryIndex(pMove->cInvenIndex)) return ERROR_ITEM_INDEX_UNMATCH;	// 인덱스가 이상하다	

	// 거래 가능 체크..
	int nRet = IsTradeEnableItem(ITEMPOSITION_SERVERWARE, pMove->cInvenIndex, pMove->biInvenItemSerial, pMove->wCount );
	if( nRet != ERROR_NONE )
		return nRet;	

	int nInvenOverlapCount = g_pDataManager->GetItemOverlapCount(pInven->nItemID);
	if (nInvenOverlapCount <= 0) return ERROR_ITEM_FAIL;

	int nServerWareCount = (int)m_MapServerWarehouse.size();
	int nLimiServerWareCount = static_cast<int>(CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::SERVER_WAREHOUSE_INVENCOUNT));	

	const TItem *pWare = GetServerWare(pMove->biWareItemSerial);

	INT64 biWareItemSerial = pMove->biWareItemSerial;

	if (nInvenOverlapCount > 1)
	{	
		// 겹치는 아이템
		if (pWare)
		{		// 창고에 이미 무언가 있다
			int nWareOverlapCount = g_pDataManager->GetItemOverlapCount(pWare->nItemID);
			if (pWare->nItemID != pInven->nItemID)		// 다른 아이템이면 에러다..클라에서 체크함.
				return ERROR_ITEM_FAIL; 
			else 
			{
				// 같은 아이템
				TItem InvenBack = m_Inventory[pMove->cInvenIndex];		// backup
				TItem AddItem = m_Inventory[pMove->cInvenIndex];		// 넣을 아이템

				if (pWare->wCount + pMove->wCount > nWareOverlapCount) // Max값을 넘으면 에러.
					return ERROR_ITEM_FAIL;				
				else 
				{	// 걍 넣으면 됨
					INT64 nBeforeSerial = AddItem.nSerial;
					if (_PopInventorySlotItem(pMove->cInvenIndex, pMove->wCount) != ERROR_NONE) return ERROR_ITEM_FAIL;

					INT64 biNewSerial = 0;
					bool bMergeAll = false;
					if (AddItem.wCount == pMove->wCount)
						bMergeAll = true;
					else
						biNewSerial = MakeItemSerial();	// split할 때 시리얼 새로 만들어야함 (디비에서 사용)

					AddItem.wCount = pMove->wCount;
					AddItem.nSerial = pWare->nSerial;

					if (_PushServerWare(pMove->biWareItemSerial, AddItem) != ERROR_NONE){
						m_Inventory[pMove->cInvenIndex] = InvenBack;	// add하다가 오류나서 다시 복구
						return ERROR_ITEM_FAIL;
					}
					m_pSession->GetDBConnection()->QueryChangeItemLocation(m_pSession, nBeforeSerial, biNewSerial, AddItem.nItemID, 
						DBDNWorldDef::ItemLocation::ServerWare, 0, pMove->wCount, true, AddItem.nSerial, bMergeAll, false, 0, 0, nGoldPee); 
				}
			}
		}
		else 
		{	// 아무것도 없다
			if( nServerWareCount >= nLimiServerWareCount )
				return ERROR_ITEM_FAIL; // 창고가 이미 꽉참

			if (pInven->wCount > pMove->wCount){	// 일부만 옮긴다
				INT64 nBeforeSerial = m_Inventory[pMove->cInvenIndex].nSerial;
				TItem InvenBack = m_Inventory[pMove->cInvenIndex];		// backup
				TItem AddItem = m_Inventory[pMove->cInvenIndex];		// 넣을 아이템

				if (_PopInventorySlotItem(pMove->cInvenIndex, pMove->wCount) != ERROR_NONE) return ERROR_ITEM_FAIL;

				AddItem.wCount = pMove->wCount;
				if (m_Inventory[pMove->cInvenIndex].wCount > 0){
					AddItem.nSerial = MakeItemSerial();
				}
				if (_PushServerWare(pMove->biWareItemSerial, AddItem) != ERROR_NONE){
					m_Inventory[pMove->cInvenIndex] = InvenBack;	// add하다가 오류나서 다시 복구
					return ERROR_ITEM_FAIL;
				}
				biWareItemSerial = AddItem.nSerial;
				m_pSession->GetDBConnection()->QueryChangeItemLocation(m_pSession, nBeforeSerial, AddItem.nSerial, AddItem.nItemID, 
					DBDNWorldDef::ItemLocation::ServerWare, 0, pMove->wCount, false, 0, false, false, 0, 0, nGoldPee);
			}
			else 
			{
				// 통째로 옮기는거..빈자리에 끼워넣기
				INT64 nBeforeSerial = m_Inventory[pMove->cInvenIndex].nSerial;
				TItem AddItem = m_Inventory[pMove->cInvenIndex];		// 넣을 아이템
				if (_PopInventorySlotItem(pMove->cInvenIndex, pMove->wCount) != ERROR_NONE) return ERROR_ITEM_FAIL;				
				nRet = _PushServerWare(0, AddItem);
				if (nRet != ERROR_NONE) return nRet;
				biWareItemSerial = AddItem.nSerial;
				m_pSession->GetDBConnection()->QueryChangeItemLocation(m_pSession, nBeforeSerial, 0, AddItem.nItemID, 
					DBDNWorldDef::ItemLocation::ServerWare, 0, pMove->wCount, false, 0, false, false, 0, 0, nGoldPee);
			}
		}
	}
	else
	{
		if( nServerWareCount >= nLimiServerWareCount )
			return ERROR_ITEM_FAIL; // 창고가 이미 꽉참

		// 통째로 옮기는거..빈자리에 끼워넣기
		INT64 nBeforeSerial = m_Inventory[pMove->cInvenIndex].nSerial;
		TItem AddItem = m_Inventory[pMove->cInvenIndex];		// 넣을 아이템
		if (_PopInventorySlotItem(pMove->cInvenIndex, pMove->wCount) != ERROR_NONE) return ERROR_ITEM_FAIL;		
		nRet = _PushServerWare(0, AddItem);				
		if (nRet != ERROR_NONE) return nRet;
		biWareItemSerial = AddItem.nSerial;
		m_pSession->GetDBConnection()->QueryChangeItemLocation(m_pSession, nBeforeSerial, 0, AddItem.nItemID,
			DBDNWorldDef::ItemLocation::ServerWare, 0, pMove->wCount, false, 0, false, false, 0, 0, nGoldPee);
	}
	const TItem* pWareItem = GetServerWare(biWareItemSerial);
	if( nPayment == 0 )  // 골드 수수료 모델		
		m_pSession->DelCoin(nGoldPee, DBDNWorldDef::CoinChangeCode::DoNotDBSave, 0);	// 돈 빼주고	
	else if ( nPayment == 1 ) // 코인
	{
		if( UseItemByType(ITEMTYPE_SERVERWARE_COIN, 1, true) == false ) // 이건 뭥미..
			return ERROR_ITEM_FAIL;
	}			
	m_pSession->SendMoveServerWareItem(pMove->cMoveType, pMove->cInvenIndex, pWareItem ? pWareItem->nSerial : 0, &m_Inventory[pMove->cInvenIndex], pWareItem, ERROR_NONE);		
	return ERROR_NONE;
}

int CDNUserItem::_FromServerWareHouseToInven(const CSMoveServerWare *pMove)
{	
	if (!_CheckRangeInventoryIndex(pMove->cInvenIndex)) return ERROR_ITEM_INDEX_UNMATCH;	// 인덱스가 이상하다

	const TItem *pWare = GetServerWare(pMove->biWareItemSerial);
	if (!pWare) return ERROR_ITEM_NOTFOUND;	// 아이템 존재하지않음

	if (pMove->wCount <= 0) return ERROR_ITEM_NOTFOUND;

	if (pWare->nItemID <= 0) return ERROR_ITEM_NOTFOUND;
	if (pWare->wCount < pMove->wCount) return ERROR_ITEM_OVERFLOW;		// 가지고 있는 것보다 더 많은 값이 오면 안됨
	if (pWare->nSerial != pMove->biWareItemSerial) return ERROR_ITEM_NOTFOUND;	

	int nWareOverlapCount = g_pDataManager->GetItemOverlapCount(pWare->nItemID);
	if (nWareOverlapCount <= 0) return ERROR_ITEM_FAIL;

	const TItem *pInven = GetInventory(pMove->cInvenIndex);

	BYTE cInvenIndex = pMove->cInvenIndex;

	int nRet = 0;
	if (nWareOverlapCount > 1)
	{	// 겹치는 아이템
		if (pInven)
		{	
			// 이미 index에 뭐가 있다.
			int nInvenOverlapCount = g_pDataManager->GetItemOverlapCount(pInven->nItemID);
			if (nInvenOverlapCount <= 0) return ERROR_ITEM_FAIL;
			if (pInven->nItemID != pWare->nItemID)
				return ERROR_ITEM_FAIL; // 이건 에러..			
			else 
			{
				// 동일 아이템
				TItem WareBack, AddItem;
				memcpy(&WareBack, pWare, sizeof(TItem));
				memcpy(&AddItem, pWare, sizeof(TItem));				

				if (pInven->wCount + pMove->wCount > nInvenOverlapCount)
					return ERROR_ITEM_FAIL; // 이건 에러..
				else
				{
					// 걍 넣으면 됨
					INT64 nBeforeSerial = AddItem.nSerial;
					if (_PopServerWareBySerial(pMove->biWareItemSerial, pMove->wCount) != ERROR_NONE) return ERROR_ITEM_FAIL;

					INT64 biNewSerial = 0;
					bool bMergeAll = false;
					if (AddItem.wCount == pMove->wCount)
						bMergeAll = true;
					else
						biNewSerial = MakeItemSerial();

					AddItem.wCount = pMove->wCount;
					AddItem.nSerial = pInven->nSerial;	// split할 때 시리얼 새로 만들어야함 (디비에서 사용)

					if (_PushInventorySlotItem(pMove->cInvenIndex, AddItem) != ERROR_NONE){
						m_MapServerWarehouse[WareBack.nSerial] = WareBack; // add하다가 오류나서 다시 복구
						return ERROR_ITEM_FAIL;
					}

					char cInventoryLocationCode = DBDNWorldDef::ItemLocation::Inventory;
#if defined(PRE_PERIOD_INVENTORY)
					if (_CheckRangePeriodInventoryIndex(pMove->cInvenIndex))
						cInventoryLocationCode = DBDNWorldDef::ItemLocation::PeriodInventory;
#endif	// #if defined(PRE_PERIOD_INVENTORY)

					m_pSession->GetDBConnection()->QueryChangeItemLocation(m_pSession, nBeforeSerial, biNewSerial, AddItem.nItemID, 
						cInventoryLocationCode, pMove->cInvenIndex, pMove->wCount, true, AddItem.nSerial, bMergeAll); 
				}
			}
		}
		else 
		{
			// 빈칸이다
			if (pWare->wCount > pMove->wCount)
			{
				// 분리시켜서 보냈다면
				INT64 nBeforeSerial = pWare->nSerial;
				// 동일 아이템
				TItem WareBack, AddItem;
				memcpy(&WareBack, pWare, sizeof(TItem));
				memcpy(&AddItem, pWare, sizeof(TItem));
				if (_PopServerWareBySerial(pMove->biWareItemSerial, pMove->wCount) != ERROR_NONE) return ERROR_ITEM_FAIL;				

				AddItem.wCount = pMove->wCount;
				if (pWare->wCount > 0){
					AddItem.nSerial = MakeItemSerial();
				}

				if (_PushInventorySlotItem(pMove->cInvenIndex, AddItem) != ERROR_NONE){
					m_MapServerWarehouse[WareBack.nSerial] = WareBack; // add하다가 오류나서 다시 복구
					return ERROR_ITEM_FAIL;
				}

				char cInventoryLocationCode = DBDNWorldDef::ItemLocation::Inventory;
#if defined(PRE_PERIOD_INVENTORY)
				if (_CheckRangePeriodInventoryIndex(pMove->cInvenIndex))
					cInventoryLocationCode = DBDNWorldDef::ItemLocation::PeriodInventory;
#endif	// #if defined(PRE_PERIOD_INVENTORY)

				m_pSession->GetDBConnection()->QueryChangeItemLocation(m_pSession, nBeforeSerial, AddItem.nSerial, AddItem.nItemID, 
					cInventoryLocationCode, pMove->cInvenIndex, pMove->wCount, false, 0, false);
			}
			else
			{
				// 새로 추가..빈자리에 끼워넣기
				cInvenIndex = FindBlankInventorySlot();
				if( cInvenIndex == -1 )
					return ERROR_ITEM_INVENTORY_NOTENOUGH;				
				TItem AddItem = {0,};
				memcpy(&AddItem, pWare, sizeof(TItem));

				INT64 nBeforeSerial = pWare->nSerial;
				if (_PopServerWareBySerial(pMove->biWareItemSerial, pMove->wCount) != ERROR_NONE) return ERROR_ITEM_FAIL;

				int nRet = _PushInventorySlotItem(cInvenIndex, AddItem);				
				if (nRet != ERROR_NONE) return nRet;

				char cInventoryLocationCode = DBDNWorldDef::ItemLocation::Inventory;
#if defined(PRE_PERIOD_INVENTORY)
				if (_CheckRangePeriodInventoryIndex(cInvenIndex))
					cInventoryLocationCode = DBDNWorldDef::ItemLocation::PeriodInventory;
#endif	// #if defined(PRE_PERIOD_INVENTORY)

				m_pSession->GetDBConnection()->QueryChangeItemLocation(m_pSession, nBeforeSerial, 0, AddItem.nItemID, 
					cInventoryLocationCode, cInvenIndex, pMove->wCount, false, 0, false);
			}
		}
	}
	else
	{	
		// 새로 추가..빈자리에 끼워넣기
		cInvenIndex = FindBlankInventorySlot();
		if( cInvenIndex == -1 )
			return ERROR_ITEM_INVENTORY_NOTENOUGH;		
		TItem AddItem = {0,};
		memcpy(&AddItem, pWare, sizeof(TItem));

		INT64 nBeforeSerial = pWare->nSerial;
		if (_PopServerWareBySerial(pMove->biWareItemSerial, pMove->wCount) != ERROR_NONE) return ERROR_ITEM_FAIL;

		int nRet = _PushInventorySlotItem(cInvenIndex, AddItem);				
		if (nRet != ERROR_NONE) return nRet;

		char cInventoryLocationCode = DBDNWorldDef::ItemLocation::Inventory;
#if defined(PRE_PERIOD_INVENTORY)
		if (_CheckRangePeriodInventoryIndex(cInvenIndex))
			cInventoryLocationCode = DBDNWorldDef::ItemLocation::PeriodInventory;
#endif	// #if defined(PRE_PERIOD_INVENTORY)

		m_pSession->GetDBConnection()->QueryChangeItemLocation(m_pSession, nBeforeSerial, 0, AddItem.nItemID, 
			cInventoryLocationCode, cInvenIndex, pMove->wCount, false, 0, false);
	}
	m_pSession->SendMoveServerWareItem(pMove->cMoveType, cInvenIndex, pMove->biWareItemSerial, &m_Inventory[pMove->cInvenIndex], GetServerWare(pMove->biWareItemSerial), ERROR_NONE);
	return ERROR_NONE;
}

int CDNUserItem::_FromCashToServerWareHouse(const CSMoveCashServerWare *pMove)
{
	const TItem *pInven = GetCashInventory(pMove->biItemSerial); // 캐쉬인벤
	if (!pInven) return ERROR_ITEM_NOTFOUND;	// 아이템 존재하지않음	

	//레벨 및 수수료, 아이템 검사.
	int nCheckLevel = static_cast<int>(CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::SERVER_WAREHOUSE_IN_LEVELLIMIT));
	if( nCheckLevel > m_pSession->GetLevel() )	
		return ERROR_ITEM_NOTFOUND; // 창고에 넣을수 있는 레벨이 아님..클라에서 먼저 걸러줌.
	int nPayment = static_cast<int>(CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::SERVER_WAREHOUSE_PAYMENT));	
	int nGoldPee = 0;
	short wMoveCount = pInven->wCount; 
	int nTotalCount = 0;
	if( nPayment == 0 )  // 골드 수수료 모델
	{
		nGoldPee = g_pDataManager->GetTaxAmount(TAX_WSTORAGE, m_pSession->GetLevel(), 0);
		if (nGoldPee == 0 || !m_pSession->CheckEnoughCoin(nGoldPee))
			return ERROR_ITEM_NOTFOUND; // 수수료 부족
	}
	else if( nPayment == 1 ) // 유료화 아이템 모델
	{		
		int nNeedItemCount = 1;
		nTotalCount = GetCashItemCountByType(ITEMTYPE_SERVERWARE_COIN) + GetInventoryItemCountByType(ITEMTYPE_SERVERWARE_COIN);
		//만약 넣을 아이템이 코인이면
		if( g_pDataManager->GetItemMainType(pInven->nItemID) == ITEMTYPE_SERVERWARE_COIN )
		{			
			if( nTotalCount == pInven->wCount ) // 옮기는게 전체이면..			
			{
				wMoveCount = pInven->wCount-1; // 하나빼고 넣어준다.
				nNeedItemCount = wMoveCount;
			}
			else
				nNeedItemCount = wMoveCount+1;
		}
		if( nNeedItemCount <= 0 || nTotalCount < nNeedItemCount )
			return ERROR_ITEM_NOTFOUND; // 아이템이 없음둥.
	}

	// 거래 가능 체크..
	int nRet = IsTradeEnableItem(ITEMPOSITION_SERVERWARECASH, 0, pMove->biItemSerial, wMoveCount );
	if( nRet != ERROR_NONE )
		return nRet;

	int nServerWareCount = (int)m_MapServerWarehouseCash.size();
	int nLimiServerWareCount = static_cast<int>(CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::SERVER_WAREHOUSE_CASHCOUNT));	
	if( nServerWareCount >= nLimiServerWareCount )
		return ERROR_ITEM_FAIL; // 창고가 이미 꽉참
	
#if defined(PRE_ADD_PCBANG_RENTAL_ITEM)
	if( g_pDataManager->IsPcCafeRentItem(pInven->nItemID) )
		return ERROR_ITEM_NOT_MOVE_TO_WARE;
#endif	

	INT64 biServerWareSerial = pMove->biItemSerial;	
	
	if ( nPayment == 1 && nTotalCount == pInven->wCount ) // 코인이면서 내꺼에서 빼는거면
	{
		if( UseItemByType(ITEMTYPE_SERVERWARE_COIN, 1, true) == false ) // 이건 뭥미..
			return ERROR_ITEM_FAIL;

		// 캐쉬템은 합쳐지는게 없으니 그냥 무조건 넣어주기	
		// 새로 추가..빈자리에 끼워넣기		
		TItem AddItem = {0,};
		memcpy(&AddItem, pInven, sizeof(TItem));
		AddItem.wCount = wMoveCount;

		INT64 nBeforeSerial = pInven->nSerial;
		if (_PopCashInventoryBySerial(pMove->biItemSerial, wMoveCount) != ERROR_NONE) return ERROR_ITEM_FAIL;

		nRet = _PushServerWareCash(AddItem);
		if (nRet != ERROR_NONE) return nRet;

		m_pSession->GetDBConnection()->QueryChangeItemLocation(m_pSession, nBeforeSerial, 0, AddItem.nItemID, 
			DBDNWorldDef::ItemLocation::ServerWareCash, 0, wMoveCount, false, 0, false, false, 0, 0, nGoldPee);
	}
	else
	{
		// 캐쉬템은 합쳐지는게 없으니 그냥 무조건 넣어주기	
		// 새로 추가..빈자리에 끼워넣기		
		TItem AddItem = {0,};
		memcpy(&AddItem, pInven, sizeof(TItem));
		AddItem.wCount = wMoveCount;

		INT64 nBeforeSerial = pInven->nSerial;
		if (_PopCashInventoryBySerial(pMove->biItemSerial, wMoveCount) != ERROR_NONE) return ERROR_ITEM_FAIL;

		nRet = _PushServerWareCash(AddItem);
		if (nRet != ERROR_NONE) return nRet;

		m_pSession->GetDBConnection()->QueryChangeItemLocation(m_pSession, nBeforeSerial, 0, AddItem.nItemID, 
			DBDNWorldDef::ItemLocation::ServerWareCash, 0, wMoveCount, false, 0, false, false, 0, 0, nGoldPee);

		if( nPayment == 0 )  // 골드 수수료 모델	
			m_pSession->DelCoin(nGoldPee, DBDNWorldDef::CoinChangeCode::DoNotDBSave, 0);	// 돈 빼주고
		else
		{
			if( UseItemByType(ITEMTYPE_SERVERWARE_COIN, 1, true) == false ) // 이건 뭥미..
				return ERROR_ITEM_FAIL;
		}		
	}
	m_pSession->SendMoveServerWareCashItem(pMove->cMoveType, pMove->biItemSerial, GetCashInventory(pMove->biItemSerial), GetServerWareCash(biServerWareSerial), ERROR_NONE);
	return ERROR_NONE;
}

int CDNUserItem::_FromServerWareHouseToCash(const CSMoveCashServerWare *pMove)
{
	const TItem *pWare = GetServerWareCash(pMove->biItemSerial);
	if (!pWare) return ERROR_ITEM_NOTFOUND;	// 아이템 존재하지않음

	if (pWare->nItemID <= 0) return ERROR_ITEM_NOTFOUND;	
	if (pWare->nSerial != pMove->biItemSerial) return ERROR_ITEM_NOTFOUND;	

	INT64 biServerWareSerial = pMove->biItemSerial;

	// 캐쉬템은 합쳐지는게 없으니 그냥 무조건 넣어주기
	// 새로 추가..빈자리에 끼워넣기
	TItem AddItem = {0,};
	memcpy(&AddItem, pWare, sizeof(TItem));

	INT64 nBeforeSerial = pWare->nSerial;
	if (_PopServerWareCashBySerial(pMove->biItemSerial, AddItem.wCount) != ERROR_NONE) return ERROR_ITEM_FAIL;

	int nRet = _PushCashInventory(AddItem);
	if (nRet != ERROR_NONE) return nRet;

	m_pSession->GetDBConnection()->QueryChangeItemLocation(m_pSession, nBeforeSerial, 0, AddItem.nItemID, 
		DBDNWorldDef::ItemLocation::CashInventory, 0, AddItem.wCount, false, 0, false);
	
	m_pSession->SendMoveServerWareCashItem(pMove->cMoveType, pMove->biItemSerial, GetServerWareCash(pMove->biItemSerial), GetCashInventory(biServerWareSerial), ERROR_NONE);	
	return ERROR_NONE;
}
#endif // #if defined(PRE_ADD_SERVER_WAREHOUSE)

// 캐쉬쪽
int CDNUserItem::_FromCashEquipToCashInven(const CSMoveCashItem *pMove)
{
	if (pMove->wCount <= 0) return ERROR_ITEM_NOTFOUND;	// 개수 이상

	int nSrcEquipIndex = pMove->cCashEquipIndex;
	INT64 biDestInvenSerial = pMove->biCashInvenSerial;

	if (!_CheckRangeCashEquipIndex(nSrcEquipIndex)) return ERROR_ITEM_INDEX_UNMATCH;	// 인덱스가 이상하다
	// nDest는 검사하지않는다.
#if defined(PRE_ADD_EQUIPLOCK)
	if (IsLockItem(DBDNWorldDef::ItemLocation::CashEquip, nSrcEquipIndex)) return ERROR_ITEM_LOCKITEM_NOTMOVE;
#endif	// #if defined(PRE_ADD_EQUIPLOCK)

	const TItem *pEquip = GetCashEquip(nSrcEquipIndex);
	if (!pEquip) return ERROR_ITEM_NOTFOUND;		// 아이템이 없다(옮길 필요 없음)
	if (pEquip->wCount < pMove->wCount) return ERROR_ITEM_OVERFLOW;		// 가지고 있는 것보다 더 많은 값이 오면 안됨
	if (pEquip->nSerial != pMove->biEquipItemSerial) return ERROR_ITEM_NOTFOUND;

	const TItem *pInven = GetCashInventory(biDestInvenSerial);
	if (pInven)	// Note : 인벤토리의 아이템도 개봉 여부 등을 검사해야할 필요가 있을 것 같아 추가했습니다. 문제 발생시 수정, 혹은 kalliste에게 문의 해 주세요
	{
		int ret = IsEquipEnableItem(*pInven);
		if (ret != ERROR_NONE) return ret;
	}

	// 한벌옷이든 일반옷이든 벗기고 인벤에 넣는다
	TPartData *pEquipPart = g_pDataManager->GetPartData(pEquip->nItemID);
	if (pEquipPart && !pEquipPart->nSubPartsList.empty()){
		TItem MainEquip = m_CashEquip[pEquipPart->nParts];
		int nRet = TakeOffOnePiece(pEquip->nItemID);	// 한벌옷 벗기고(안에서 디비저장)
		if (nRet != ERROR_NONE) return nRet;

		if (_PushCashInventory(MainEquip) == ERROR_NONE){
			m_pSession->SendMoveCashItem(MoveType_CashEquipToCashInven, pEquipPart->nParts, GetCashEquip(pEquipPart->nParts), MainEquip.nSerial, &MainEquip, ERROR_NONE);		// 뺀옷 캐쉬인벤에 넣어줌
		}
	}
	else
	{
		INT64 biEquipSerial = pEquip->nSerial;	// pEquip이 NULL일 경우 없음

		int nRet = _SwapCashEquipToCashInven(nSrcEquipIndex, biDestInvenSerial);		// 서로 바꿔준다.	// 안에서 디비저장
		if (nRet != ERROR_NONE) return nRet;

		m_pSession->SendMoveCashItem(pMove->cMoveType, nSrcEquipIndex, &m_CashEquip[nSrcEquipIndex], biEquipSerial, GetCashInventory(biEquipSerial), ERROR_NONE);
	}

	return ERROR_NONE;
}

int CDNUserItem::_FromCashInvenToCashEquip(const CSMoveCashItem *pMove)
{
	if (pMove->wCount <= 0) return ERROR_ITEM_NOTFOUND;

	int nDestEquipIndex = pMove->cCashEquipIndex;
	INT64 biSrcInvenSerial = pMove->biCashInvenSerial;

	if (!_CheckRangeCashEquipIndex(nDestEquipIndex)) return ERROR_ITEM_INDEX_UNMATCH;	// 인덱스가 이상하다
#if defined(PRE_ADD_EQUIPLOCK)
	if (IsLockItem(DBDNWorldDef::ItemLocation::CashEquip, nDestEquipIndex)) return ERROR_ITEM_LOCKITEM_NOTMOVE;
#endif	// #if defined(PRE_ADD_EQUIPLOCK)

	const TItem *pInven = GetCashInventory(biSrcInvenSerial);
	if (!pInven) return ERROR_ITEM_NOTFOUND;	// 아이템 존재하지않음
	if (pInven->wCount < pMove->wCount) return ERROR_ITEM_OVERFLOW;		// 가지고 있는 것보다 더 많은 값이 오면 안됨
	if (!IsEquipItem(nDestEquipIndex, pInven->nItemID)) return ERROR_ITEM_FAIL;		// 장착가능한지 검사
	if (pInven)
	{
		int ret = IsEquipEnableItem(*pInven);
		if (ret != ERROR_NONE)
			return ret;
	}

	const TItem *pEquip = GetCashEquip(nDestEquipIndex);

	TItem MainInven = { 0, };
	bool bEquipSub = false, bInvenSub = false;
	if (pInven){
		MainInven = *pInven;
		bInvenSub = g_pDataManager->IsSubParts(pInven->nItemID);
	}
	if (pEquip)
		bEquipSub = g_pDataManager->IsSubParts(pEquip->nItemID);

	if (!bEquipSub){
		if (g_pDataManager->GetItemMainType(pInven->nItemID) == ITEMTYPE_PARTS){
			int nMainPart = GetCashEquipOnePieceMainParts(pInven->nItemID);
			if (nMainPart != -1) bEquipSub = true;
		}
	}

	if (!bEquipSub && !bInvenSub){	// Equip:일반옷 Inven:일반옷
		INT64 biSerial = (pEquip && (pEquip->nSerial > 0)) ? pEquip->nSerial : biSrcInvenSerial;

		int nRet = _SwapCashInvenToCashEquip(biSrcInvenSerial, nDestEquipIndex);		// 서로 바꿔준다.	// 안에서 디비저장
		if (nRet != ERROR_NONE) return nRet;

		m_pSession->SendMoveCashItem(pMove->cMoveType, nDestEquipIndex, GetCashEquip(nDestEquipIndex), biSerial, GetCashInventory(biSerial), ERROR_NONE);
	}
	else{
		if (bInvenSub){	// Inven: 한벌옷
			TPartData *pInvenPart = g_pDataManager->GetPartData(pInven->nItemID);
			if (pInvenPart){
				bool bSend = false;
				// 한벌옷 메인자리에 있는 파츠 먼저 빼주고
				TItem Equip = m_CashEquip[pInvenPart->nParts];
				if (_PopCashEquipSlot(pInvenPart->nParts, 1) == ERROR_NONE){	// 그 자리에 있다면 일단 빼버린다
					if (_PushCashInventory(Equip) == ERROR_NONE) bSend = true;
				}

				// 한벌옷 입히고
				int nRet = _PopCashInventoryBySerial(MainInven.nSerial, MainInven.wCount);	// 캐쉬인벤 빼서
				if (nRet != ERROR_NONE) return nRet;
				nRet = _PushCashEquipSlot(pInvenPart->nParts, MainInven);
				if (nRet != ERROR_NONE) return nRet;

				m_pSession->SendMoveCashItem(pMove->cMoveType, nDestEquipIndex, GetCashEquip(nDestEquipIndex), biSrcInvenSerial, GetCashInventory(biSrcInvenSerial), nRet);

				// 온 패킷 먼저 응답보내주고 그담에 나머지 추가로 보내줘야한다.(그래서 요 패킷 cMoveType 뒤로 뺐음 - 땜빵?)
				if (bSend)
					m_pSession->SendMoveCashItem(MoveType_CashEquipToCashInven, pInvenPart->nParts, GetCashEquip(pInvenPart->nParts), Equip.nSerial, GetCashInventory(Equip.nSerial), ERROR_NONE);

				int nSubIndexArray[CASHEQUIPMAX] = { 0, };
				int nCashEquipIndexArray[CASHEQUIPMAX] = { 0, };
				nCashEquipIndexArray[0] = pInvenPart->nParts;

				// 한벌옷 서브파츠 있던 자리에 있는 애들 인벤으로 빼주기
				for (int i = 0; i <(int)pInvenPart->nSubPartsList.size(); i++){
					Equip = m_CashEquip[pInvenPart->nSubPartsList[i]];
					if (_PopCashEquipSlot(pInvenPart->nSubPartsList[i], 1) == ERROR_NONE){
						if (_PushCashInventory(Equip) == ERROR_NONE)
							m_pSession->SendMoveCashItem(MoveType_CashEquipToCashInven, pInvenPart->nSubPartsList[i], GetCashEquip(pInvenPart->nSubPartsList[i]), Equip.nSerial, GetCashInventory(Equip.nSerial), ERROR_NONE);
					}

					nSubIndexArray[i] = pInvenPart->nSubPartsList[i];
					nCashEquipIndexArray[1 + i] = pInvenPart->nSubPartsList[i];
				}

				// 다 됐으면 디비저장
				if (!pInvenPart->nSubPartsList.empty()){
					int nTotalCount = (int)pInvenPart->nSubPartsList.size() + 1;
					m_pSession->GetDBConnection()->QueryTakeCashEquipmentOff(m_pSession, nTotalCount, nCashEquipIndexArray);
					m_pSession->GetDBConnection()->QueryPutOnepieceOn(m_pSession, MainInven.nSerial, pInvenPart->nParts, (int)pInvenPart->nSubPartsList.size(), nSubIndexArray);
				}
			}
		}
		else{	// Inven: 일반옷
			if (!IsEquipItem(nDestEquipIndex, MainInven.nItemID)) return ERROR_ITEM_EQUIPMISMATCH;

			bool bSend = false;
			int nMainPart = -1;
			int nBlankIndex = -1;
			TItem MainEquip = { 0, };
			if (g_pDataManager->GetItemMainType(pInven->nItemID) == ITEMTYPE_PARTS){
				nMainPart = GetCashEquipOnePieceMainParts(pInven->nItemID);
				if (nMainPart == -1) nMainPart = nDestEquipIndex;	// 한벌옷: 대표인덱스 나머지:걍 지금 인덱스 세팅

				MainEquip = m_CashEquip[nMainPart];
				int nRet = TakeOffOnePiece(MainEquip.nItemID);	// 한벌옷 벗기고(안에서 디비저장)
				if (nRet != ERROR_NONE) return nRet;

				if (_PushCashInventory(MainEquip) == ERROR_NONE) bSend = true;
			}

			int nRet = _PopCashInventoryBySerial(MainInven.nSerial, MainInven.wCount);	// 캐쉬인벤 빼서
			if (nRet != ERROR_NONE) return nRet;

			if (_PushCashEquipSlot(nDestEquipIndex, MainInven) == ERROR_NONE){
				m_pSession->GetDBConnection()->QueryPutOnepieceOn(m_pSession, MainInven.nSerial, nDestEquipIndex, 0, NULL);
			}

			m_pSession->SendMoveCashItem(pMove->cMoveType, nDestEquipIndex, GetCashEquip(nDestEquipIndex), biSrcInvenSerial, GetCashInventory(biSrcInvenSerial), ERROR_NONE);

			if (bSend)
				m_pSession->SendMoveCashItem(MoveType_CashEquipToCashInven, nMainPart, GetCashEquip(nMainPart), MainEquip.nSerial, GetCashInventory(MainEquip.nSerial), ERROR_NONE);		// 뺀옷 캐쉬인벤에 넣어줌
		}
	}

	return ERROR_NONE;
}

int CDNUserItem::_FromCashGlyphToCashInven(const CSMoveCashItem *pMove)
{
	if (pMove->wCount <= 0) return ERROR_ITEM_NOTFOUND;	// 개수 이상
	int nSrcGlyphIndex = pMove->cCashEquipIndex;
	INT64 biDestInvenSerial = pMove->biCashInvenSerial;

	if (!_CheckRangeCashGlyphIndex(nSrcGlyphIndex)) return ERROR_ITEM_INDEX_UNMATCH;	// 인덱스가 이상하다
	// nDest는 검사하지않는다.

	const TItem *pGlyph = GetGlyph(nSrcGlyphIndex);
	if (!pGlyph) return ERROR_ITEM_NOTFOUND;		// 아이템이 없다(옮길 필요 없음)
	if (pGlyph->wCount < pMove->wCount) return ERROR_ITEM_OVERFLOW;		// 가지고 있는 것보다 더 많은 값이 오면 안됨
	if (pGlyph->nSerial != pMove->biEquipItemSerial) return ERROR_ITEM_NOTFOUND;

	INT64 biGlyphSerial = pGlyph->nSerial;	// pGlyph이 NULL일 경우 없음

	int nRet = _SwapCashGlyphToCashInven(nSrcGlyphIndex, biDestInvenSerial);		// 서로 바꿔준다.	// 안에서 디비저장
	if (nRet != ERROR_NONE) return nRet;

	m_pSession->SendMoveCashItem(pMove->cMoveType, nSrcGlyphIndex, &m_Glyph[nSrcGlyphIndex], biGlyphSerial, GetCashInventory(biGlyphSerial), ERROR_NONE);
	return ERROR_NONE;
}

int CDNUserItem::_FromCashInvenToCashGlyph(const CSMoveCashItem *pMove)
{
	// 캐시문장아이템은 없당~~~!
	return ERROR_NONE;	
}

int CDNUserItem::_FromVehicleBodyToVehicleInven(const CSMoveCashItem *pMove)
{
	if (pMove->wCount <= 0) return ERROR_ITEM_NOTFOUND;	// 개수 이상
	int nSrcBodyIndex = pMove->cCashEquipIndex;
	INT64 biDestInvenSerial = pMove->biCashInvenSerial;

	if (!_CheckRangeVehicleBodyIndex(nSrcBodyIndex)) return ERROR_ITEM_INDEX_UNMATCH;	// 인덱스가 이상하다
	// nDest는 검사하지않는다.

	TVehicle *pVehicle = GetVehicleEquip();
	if (!pVehicle) return ERROR_ITEM_NOTFOUND;		// 아이템이 없다(옮길 필요 없음)
	if (pVehicle->Vehicle[nSrcBodyIndex].wCount < pMove->wCount) return ERROR_ITEM_OVERFLOW;		// 가지고 있는 것보다 더 많은 값이 오면 안됨
	if (pVehicle->Vehicle[nSrcBodyIndex].nSerial != pMove->biEquipItemSerial) return ERROR_ITEM_NOTFOUND;

	INT64 biVehicleSerial = pVehicle->Vehicle[Vehicle::Slot::Body].nSerial;

	// 서로 바꿔도 되는지 일단 장착 검사해보쟈
	int nInvenItemID = 0;
	short wInvenCount = 0;

	TVehicle NewVehicle;
	memset(&NewVehicle, 0, sizeof(NewVehicle));

	const TVehicle *pVehicleInven = GetVehicleInventory(biDestInvenSerial);
	if (pVehicleInven)
	{
		if (!IsVehicleEquipItem(nSrcBodyIndex, pVehicleInven->Vehicle[Vehicle::Slot::Body].nItemID)) return ERROR_ITEM_INDEX_UNMATCH;
		nInvenItemID = pVehicleInven->Vehicle[Vehicle::Slot::Body].nItemID;
		wInvenCount = pVehicleInven->Vehicle[Vehicle::Slot::Body].wCount;

		NewVehicle = *pVehicleInven;
	}

	if ((m_VehicleEquip.Vehicle[nSrcBodyIndex].nItemID > 0) && (nInvenItemID > 0)){
		m_pSession->GetDBConnection()->QuerySwitchItemLocation(m_pSession, DBDNWorldDef::ItemLocation::Equip, 
			Vehicle::Common::VehicleEquipBody, biDestInvenSerial, DBDNWorldDef::ItemLocation::VehicleInventory, 0, m_VehicleEquip.Vehicle[nSrcBodyIndex].nSerial);
	}
	else if (m_VehicleEquip.Vehicle[nSrcBodyIndex].nItemID > 0){	// 탈것 해제
		m_pSession->GetDBConnection()->QueryChangeItemLocation(m_pSession, m_VehicleEquip.Vehicle[nSrcBodyIndex].nSerial, 0, m_VehicleEquip.Vehicle[nSrcBodyIndex].nItemID, 
			DBDNWorldDef::ItemLocation::VehicleInventory, 0, m_VehicleEquip.Vehicle[nSrcBodyIndex].wCount, false, 0, false);
	}
	else if (nInvenItemID > 0){		// 탈것 소환
		m_pSession->GetDBConnection()->QueryChangeItemLocation(m_pSession, biDestInvenSerial, 0, nInvenItemID, 
			DBDNWorldDef::ItemLocation::Equip, Vehicle::Common::VehicleEquipBody, wInvenCount, false, 0, false);
	}

	TVehicle Equip = m_VehicleEquip;
	m_VehicleEquip = NewVehicle;
	if (NewVehicle.Vehicle[Vehicle::Slot::Body].nItemID > 0)
		_PopVehicleInventoryBySerial(NewVehicle.Vehicle[Vehicle::Slot::Body].nSerial);
	if (Equip.Vehicle[Vehicle::Slot::Body].nItemID > 0) 
		_PushVehicleInventory(Equip);

	for (int i = 0; i < Vehicle::Slot::Max; i++){
		BroadcastChangeVehicleParts(i, m_VehicleEquip.Vehicle[i]);
	}
	BroadcastChangeVehiclePetColor(m_VehicleEquip.Vehicle[0].nSerial, m_VehicleEquip.dwPartsColor1, Vehicle::Parts::BodyColor);

	m_pSession->SendMoveCashItem(pMove->cMoveType, nSrcBodyIndex, GetVehiclePartsEquip(nSrcBodyIndex), biVehicleSerial, GetVehicleBodyInventory(biVehicleSerial), ERROR_NONE);
	return ERROR_NONE;
}

int CDNUserItem::_FromVehicleInvenToVehicleBody(const CSMoveCashItem *pMove)
{
	if (pMove->wCount <= 0) return ERROR_ITEM_NOTFOUND;

	int nDestBodyIndex = pMove->cCashEquipIndex;
	INT64 biSrcInvenSerial = pMove->biCashInvenSerial;

	if (!_CheckRangeVehicleBodyIndex(nDestBodyIndex)) return ERROR_ITEM_INDEX_UNMATCH;	// 인덱스가 이상하다

	const TItem *pInven = GetVehicleBodyInventory(biSrcInvenSerial);
	if (!pInven) return ERROR_ITEM_NOTFOUND;	// 아이템 존재하지않음
	if (pInven->wCount < pMove->wCount) return ERROR_ITEM_OVERFLOW;		// 가지고 있는 것보다 더 많은 값이 오면 안됨
	if (!IsVehicleEquipItem(nDestBodyIndex, pInven->nItemID)) return ERROR_ITEM_FAIL;		// 장착가능한지 검사

	TVehicle *pVehicle = GetVehicleEquip();
	INT64 biSerial = (pVehicle && (pVehicle->Vehicle[Vehicle::Slot::Body].nSerial > 0)) ? pVehicle->Vehicle[Vehicle::Slot::Body].nSerial : biSrcInvenSerial;

	const TVehicle *pVehicleInven = GetVehicleInventory(biSrcInvenSerial);
	if (!pVehicleInven) return ERROR_ITEM_INDEX_UNMATCH;

	if ((pVehicleInven->Vehicle[Vehicle::Slot::Body].nItemID > 0) && (m_VehicleEquip.Vehicle[nDestBodyIndex].nItemID > 0)){
		m_pSession->GetDBConnection()->QuerySwitchItemLocation(m_pSession, DBDNWorldDef::ItemLocation::VehicleInventory, 
			0, m_VehicleEquip.Vehicle[nDestBodyIndex].nSerial, DBDNWorldDef::ItemLocation::Equip, Vehicle::Common::VehicleEquipBody, pVehicleInven->Vehicle[Vehicle::Slot::Body].nSerial);
	}
	else if (pVehicleInven->Vehicle[Vehicle::Slot::Body].nItemID > 0){
		m_pSession->GetDBConnection()->QueryChangeItemLocation(m_pSession, pVehicleInven->Vehicle[Vehicle::Slot::Body].nSerial, 0, pVehicleInven->Vehicle[Vehicle::Slot::Body].nItemID, 
			DBDNWorldDef::ItemLocation::Equip, Vehicle::Common::VehicleEquipBody, pVehicleInven->Vehicle[Vehicle::Slot::Body].wCount, false, 0, false);
	}
	else if (m_VehicleEquip.Vehicle[nDestBodyIndex].nItemID > 0){
		m_pSession->GetDBConnection()->QueryChangeItemLocation(m_pSession, m_VehicleEquip.Vehicle[nDestBodyIndex].nSerial, 0, m_VehicleEquip.Vehicle[nDestBodyIndex].nItemID, 
			DBDNWorldDef::ItemLocation::VehicleInventory, 0, m_VehicleEquip.Vehicle[nDestBodyIndex].wCount, false, 0, false);
	}

	TVehicle NewVehicle = *pVehicleInven;
	if (NewVehicle.Vehicle[Vehicle::Slot::Body].nItemID > 0)
		_PopVehicleInventoryBySerial(NewVehicle.Vehicle[Vehicle::Slot::Body].nSerial);
	if (m_VehicleEquip.Vehicle[nDestBodyIndex].nItemID > 0)
		_PushVehicleInventory(m_VehicleEquip);

	m_VehicleEquip = NewVehicle;

	for (int i = 0; i < Vehicle::Slot::Max; i++){
		BroadcastChangeVehicleParts(i, m_VehicleEquip.Vehicle[i]);
	}
	BroadcastChangeVehiclePetColor( m_VehicleEquip.Vehicle[0].nSerial, m_VehicleEquip.dwPartsColor1, Vehicle::Parts::BodyColor);

	m_pSession->SendMoveCashItem(pMove->cMoveType, nDestBodyIndex, GetVehiclePartsEquip(nDestBodyIndex), biSerial, GetVehicleBodyInventory(biSerial), ERROR_NONE);
	return ERROR_NONE;
}

int CDNUserItem::_FromVehiclePartsToCashInven(const CSMoveCashItem *pMove)
{
	if (pMove->wCount <= 0) return ERROR_ITEM_NOTFOUND;	// 개수 이상
	int nSrcPartsIndex = pMove->cCashEquipIndex;
	INT64 biDestInvenSerial = pMove->biCashInvenSerial;

	if (nSrcPartsIndex != Vehicle::Slot::Saddle) return ERROR_ITEM_INDEX_UNMATCH;

	const TItem *pParts = GetVehiclePartsEquip(nSrcPartsIndex);
	if (!pParts) return ERROR_ITEM_NOTFOUND;		// 아이템이 없다(옮길 필요 없음)
	if (pParts->wCount < pMove->wCount) return ERROR_ITEM_OVERFLOW;		// 가지고 있는 것보다 더 많은 값이 오면 안됨
	if (pParts->nSerial != pMove->biEquipItemSerial) return ERROR_ITEM_NOTFOUND;
	if(g_pDataManager->bIsInstantItem( m_VehicleEquip.Vehicle[Vehicle::Slot::Body].nItemID ) == true ) 
		return ERROR_ITEM_EQUIPTOINVEN_FAIL;
#if defined(PRE_ADD_PCBANG_RENTAL_ITEM)
	if(g_pDataManager->IsPcCafeRentItem( m_VehicleEquip.Vehicle[Vehicle::Slot::Body].nItemID ) == true ) 
		return ERROR_ITEM_EQUIPTOINVEN_FAIL;
#endif

	INT64 biPartsSerial = pParts->nSerial;

	// 서로 바꿔도 되는지 일단 장착 검사해보쟈
	int nInvenItemID = 0;
	short wInvenCount = 0;

	TItem NewItem = { 0, };
	const TItem *pInven = GetCashInventory(biDestInvenSerial);
	if (pInven)
	{
		if (!IsVehicleEquipItem(nSrcPartsIndex, pInven->nItemID)) return ERROR_ITEM_EQUIPMISMATCH;
		nInvenItemID = pInven->nItemID;
		wInvenCount = pInven->wCount;

		NewItem = *pInven;
	}

	if (m_pSession->GetDBConnection()){
		if (m_VehicleEquip.Vehicle[nSrcPartsIndex].nItemID > 0){	// 파츠 해제
			m_pSession->GetDBConnection()->QueryChangeItemLocation(m_pSession, m_VehicleEquip.Vehicle[nSrcPartsIndex].nSerial, 0, m_VehicleEquip.Vehicle[nSrcPartsIndex].nItemID, 
				DBDNWorldDef::ItemLocation::CashInventory, 0, m_VehicleEquip.Vehicle[nSrcPartsIndex].wCount, false, 0, false, true, m_VehicleEquip.Vehicle[Vehicle::Slot::Body].nSerial, nSrcPartsIndex);
		}
		if (nInvenItemID > 0){	// 파츠 장착
			m_pSession->GetDBConnection()->QueryChangeItemLocation(m_pSession, biDestInvenSerial, 0, nInvenItemID, 
				0, 0, wInvenCount, false, 0, false, true, m_VehicleEquip.Vehicle[Vehicle::Slot::Body].nSerial, nSrcPartsIndex);
		}
	}

	TItem Equip = m_VehicleEquip.Vehicle[nSrcPartsIndex];
	m_VehicleEquip.Vehicle[nSrcPartsIndex] = NewItem;
	if (NewItem.nItemID > 0)
		_PopCashInventoryBySerial(NewItem.nSerial, NewItem.wCount);
	if (Equip.nItemID > 0) 
		_PushCashInventory(Equip);

	BroadcastChangeVehicleParts(nSrcPartsIndex, m_VehicleEquip.Vehicle[nSrcPartsIndex]);

	m_pSession->SendMoveCashItem(pMove->cMoveType, nSrcPartsIndex, GetVehiclePartsEquip(nSrcPartsIndex), biPartsSerial, GetCashInventory(biPartsSerial), ERROR_NONE);
	return ERROR_NONE;
}

int CDNUserItem::_FromCashInvenToVehicleParts(const CSMoveCashItem *pMove)
{
	if (pMove->wCount <= 0) return ERROR_ITEM_NOTFOUND;
	int nDestPartsIndex = pMove->cCashEquipIndex;
	INT64 biSrcInvenSerial = pMove->biCashInvenSerial;

	if ((nDestPartsIndex < Vehicle::Slot::Saddle) ||(nDestPartsIndex >= Vehicle::Slot::Max)) 
		return ERROR_ITEM_INDEX_UNMATCH;	// 인덱스가 이상하다

	const TItem *pInven = GetCashInventory(biSrcInvenSerial);
	if (!pInven) return ERROR_ITEM_NOTFOUND;	// 아이템 존재하지않음

	if (pInven->wCount < pMove->wCount) return ERROR_ITEM_OVERFLOW;		// 가지고 있는 것보다 더 많은 값이 오면 안됨
	if (!IsVehicleEquipItem(nDestPartsIndex, pInven->nItemID)) return ERROR_ITEM_FAIL;		// 장착가능한지 검사

	const TItem *pParts = GetVehiclePartsEquip(nDestPartsIndex);
	INT64 biSerial = (pParts && (pParts->nSerial > 0)) ? pParts->nSerial : biSrcInvenSerial;

	if ( g_pDataManager->bIsInstantItem( m_VehicleEquip.Vehicle[Vehicle::Slot::Body].nItemID) == true )
		return ERROR_ITEM_INVENTOEQUIP_FAIL;
#if defined(PRE_ADD_PCBANG_RENTAL_ITEM)
	if(g_pDataManager->IsPcCafeRentItem( m_VehicleEquip.Vehicle[Vehicle::Slot::Body].nItemID ) == true ) 
		return ERROR_ITEM_INVENTOEQUIP_FAIL;
#endif

	if (m_pSession->GetDBConnection()){
		switch(nDestPartsIndex)
		{
		case Vehicle::Slot::Saddle:
			{
				if (m_VehicleEquip.Vehicle[nDestPartsIndex].nItemID > 0){	// 파츠 해제
					m_pSession->GetDBConnection()->QueryChangeItemLocation(m_pSession, m_VehicleEquip.Vehicle[nDestPartsIndex].nSerial, 0, m_VehicleEquip.Vehicle[nDestPartsIndex].nItemID, 
						DBDNWorldDef::ItemLocation::CashInventory, 0, m_VehicleEquip.Vehicle[nDestPartsIndex].wCount, false, 0, false, true, m_VehicleEquip.Vehicle[Vehicle::Slot::Body].nSerial, nDestPartsIndex);
				}
			}
			break;

		case Vehicle::Slot::Hair:	// 헤어는 교체가 아니고 삭제다
			{
				// 이미있는거 지우고
				m_pSession->GetDBConnection()->QueryDelPetEquipment(m_pSession, m_VehicleEquip.Vehicle[Vehicle::Slot::Body].nSerial, biSerial);
			}
			break;
		}

		if (pInven->nItemID > 0){	// 파츠 장착
			m_pSession->GetDBConnection()->QueryChangeItemLocation(m_pSession, pInven->nSerial, 0, pInven->nItemID, 
				0, 0, pInven->wCount, false, 0, false, true, m_VehicleEquip.Vehicle[Vehicle::Slot::Body].nSerial, nDestPartsIndex);
		}
	}

	TItem NewInven = *pInven;
	if (NewInven.nItemID > 0)
		_PopCashInventoryBySerial(NewInven.nSerial, NewInven.wCount);
	if (m_VehicleEquip.Vehicle[nDestPartsIndex].nItemID > 0){
		switch(nDestPartsIndex)
		{
		case Vehicle::Slot::Saddle:
			_PushCashInventory(m_VehicleEquip.Vehicle[nDestPartsIndex]);
			break;

		case Vehicle::Slot::Hair:	// 빼버리는게 아니고 없애버려야함(그냥 새걸로 덮어 씌워야함)
			break;
		}
	}

	m_VehicleEquip.Vehicle[nDestPartsIndex] = NewInven;
	BroadcastChangeVehicleParts(nDestPartsIndex, m_VehicleEquip.Vehicle[nDestPartsIndex]);

	m_pSession->SendMoveCashItem(pMove->cMoveType, nDestPartsIndex, GetVehiclePartsEquip(nDestPartsIndex), biSerial, GetCashInventory(biSerial), ERROR_NONE);
	return ERROR_NONE;
}

int CDNUserItem::_FromPetBodyToVehicleInven(const CSMoveCashItem *pMove)
{
	if (pMove->wCount <= 0) 
		return ERROR_ITEM_NOTFOUND;	// 개수 이상

	int nSrcBodyIndex = pMove->cCashEquipIndex;
	INT64 biDestInvenSerial = pMove->biCashInvenSerial;

	if (!_CheckRangeVehicleBodyIndex(nSrcBodyIndex))
		return ERROR_ITEM_INDEX_UNMATCH;	// 인덱스가 이상하다
	// nDest는 검사하지않는다.

	const TVehicle *pPet = GetPetEquip();
	if (!pPet) 
		return ERROR_ITEM_NOTFOUND;		// 아이템이 없다(옮길 필요 없음)

	if (pPet->Vehicle[nSrcBodyIndex].wCount < pMove->wCount) 
		return ERROR_ITEM_OVERFLOW;		// 가지고 있는 것보다 더 많은 값이 오면 안됨

	if (pPet->Vehicle[nSrcBodyIndex].nSerial != pMove->biEquipItemSerial) 
		return ERROR_ITEM_NOTFOUND;

	INT64 biVehicleSerial = pPet->Vehicle[Pet::Slot::Body].nSerial;

 	if (m_pSession->GetDBConnection())
 	{
		if (m_PetEquip.Vehicle[nSrcBodyIndex].nItemID > 0) // 탈것 해제
 		{	
 			m_pSession->GetDBConnection()->QueryChangeItemLocation(	m_pSession, m_PetEquip.Vehicle[nSrcBodyIndex].nSerial, 0, m_PetEquip.Vehicle[nSrcBodyIndex].nItemID, 
				DBDNWorldDef::ItemLocation::PetInventory, 0, m_PetEquip.Vehicle[nSrcBodyIndex].wCount, false, 0, false);
 		}
 	}
#if defined(_GAMESERVER)
	// 펫 스텟 적용
	DnActorHandle hActor = m_pSession->GetActorHandle();
	CDnPlayerActor *pPlayer = static_cast<CDnPlayerActor *>( hActor.GetPointer() );
#endif

	if (m_PetEquip.Vehicle[Pet::Slot::Body].nItemID > 0) 
	{
#if defined(_GAMESERVER)
		if( m_PetEquip.nSkillID1 > 0)
			pPlayer->RemoveSkill(m_PetEquip.nSkillID1);
		if( m_PetEquip.nSkillID2 > 0)
			pPlayer->RemoveSkill(m_PetEquip.nSkillID2);
#endif
		_PushVehicleInventory(m_PetEquip);
		ZeroMemory(&m_PetEquip, sizeof(m_PetEquip));
	}

	BroadcastChangePetBody(m_PetEquip);

	m_pSession->SendMoveCashItem(pMove->cMoveType, nSrcBodyIndex, GetPetPartsEquip(nSrcBodyIndex), biVehicleSerial, GetVehicleBodyInventory(biVehicleSerial), ERROR_NONE);

#if defined(_GAMESERVER)
	pPlayer->RefreshState( CDnActorState::RefreshEquip );
#endif

	return ERROR_NONE;
}

int CDNUserItem::_FromVehicleInvenToPetBody(const CSMoveCashItem *pMove)
{
	if (pMove->wCount <= 0) return ERROR_ITEM_NOTFOUND;

	int nDestBodyIndex = pMove->cCashEquipIndex;
	INT64 biSrcInvenSerial = pMove->biCashInvenSerial;

	if (!_CheckRangeVehicleBodyIndex(nDestBodyIndex)) return ERROR_ITEM_INDEX_UNMATCH;	// 인덱스가 이상하다

	const TItem *pInven = GetVehicleBodyInventory(biSrcInvenSerial);
	if (!pInven) return ERROR_ITEM_NOTFOUND;	// 아이템 존재하지않음
	if (pInven->wCount < pMove->wCount) return ERROR_ITEM_OVERFLOW;		// 가지고 있는 것보다 더 많은 값이 오면 안됨
	if (!IsVehicleEquipItem(nDestBodyIndex, pInven->nItemID)) return ERROR_ITEM_FAIL;		// 장착가능한지 검사

	const TVehicle *pVehicleInven = GetVehicleInventory(biSrcInvenSerial);
	if (!pVehicleInven) return ERROR_ITEM_INDEX_UNMATCH;

	time_t tCurTime;
	time(&tCurTime);

	if ( !pVehicleInven->Vehicle[Vehicle::Slot::Body].bEternity && tCurTime >= pVehicleInven->Vehicle[Vehicle::Slot::Body].tExpireDate ) 
	{
		return ERROR_ITEM_FAIL;
	}

	if ((pVehicleInven->Vehicle[Vehicle::Slot::Body].nItemID > 0) && (m_PetEquip.Vehicle[nDestBodyIndex].nItemID > 0))
	{
		m_pSession->GetDBConnection()->QuerySwitchItemLocation( m_pSession, DBDNWorldDef::ItemLocation::PetInventory, 0, m_PetEquip.Vehicle[nDestBodyIndex].nSerial, DBDNWorldDef::ItemLocation::Equip, 
			Pet::Common::PetEquipBody, pVehicleInven->Vehicle[Vehicle::Slot::Body].nSerial );
	}
	else if (pVehicleInven->Vehicle[Vehicle::Slot::Body].nItemID > 0)
	{
		m_pSession->GetDBConnection()->QueryChangeItemLocation( m_pSession, pVehicleInven->Vehicle[Vehicle::Slot::Body].nSerial, 0, pVehicleInven->Vehicle[Vehicle::Slot::Body].nItemID, 
			DBDNWorldDef::ItemLocation::Equip, Pet::Common::PetEquipBody, 1, false, 0, false);
	}

	TVehicle NewPet = *pVehicleInven;
	if (NewPet.Vehicle[Pet::Slot::Body].nItemID > 0)
		_PopVehicleInventoryBySerial(NewPet.Vehicle[Pet::Slot::Body].nSerial);
	if (m_PetEquip.Vehicle[nDestBodyIndex].nItemID > 0)
		_PushVehicleInventory(m_PetEquip);
	
	INT64 biSerial = m_PetEquip.Vehicle[Pet::Slot::Body].nSerial;	
#if defined(_GAMESERVER)
	// 펫 스텟 적용
	DnActorHandle hActor = m_pSession->GetActorHandle();
	CDnPlayerActor *pPlayer = static_cast<CDnPlayerActor *>( hActor.GetPointer() );
	if( m_PetEquip.nSkillID1 > 0)
		pPlayer->RemoveSkill(m_PetEquip.nSkillID1);
	if( m_PetEquip.nSkillID2 > 0)
		pPlayer->RemoveSkill(m_PetEquip.nSkillID2);
	if( NewPet.nSkillID1 > 0)
		pPlayer->AddSkill(NewPet.nSkillID1);
	if( NewPet.nSkillID2 > 0)
		pPlayer->AddSkill(NewPet.nSkillID2);
#endif //#if defined(_GAMESERVER)
	m_PetEquip = NewPet;
	CalcPetSatietyPercent();
	BroadcastChangePetBody(m_PetEquip);
		
	m_pSession->SendMoveCashItem(pMove->cMoveType, nDestBodyIndex, GetPetPartsEquip(nDestBodyIndex), biSerial, GetVehicleBodyInventory(biSerial), ERROR_NONE);

#if defined(_GAMESERVER)	
	pPlayer->RefreshState( CDnActorState::RefreshEquip );
#endif
	return ERROR_NONE;
}

//parts 착용처리
int CDNUserItem::_FromPetPartsToCashInven(const CSMoveCashItem *pMove)
{
	if (pMove->wCount <= 0) return ERROR_ITEM_NOTFOUND;	// 개수 이상
	int nSrcPartsIndex = pMove->cCashEquipIndex;
	INT64 biDestInvenSerial = pMove->biCashInvenSerial;

	if (nSrcPartsIndex != Pet::Slot::Accessory1 && nSrcPartsIndex != Pet::Slot::Accessory2) 
		return ERROR_ITEM_INDEX_UNMATCH;

	const TItem *pParts = GetPetPartsEquip(nSrcPartsIndex);
	if (!pParts) return ERROR_ITEM_NOTFOUND;		// 아이템이 없다(옮길 필요 없음) 슬롯에 없는 아이템을 옮기려고 한건가??
	if (pParts->nSerial != pMove->biEquipItemSerial) return ERROR_ITEM_NOTFOUND;

	INT64 biPartsSerial = pParts->nSerial;

	if( g_pDataManager->bIsInstantItem( m_PetEquip.Vehicle[Pet::Slot::Body].nItemID) == true )
		return ERROR_ITEM_EQUIPTOINVEN_FAIL;
#if defined( PRE_ADD_PCBANG_RENTAL_ITEM )
	if( g_pDataManager->IsPcCafeRentItem( m_PetEquip.Vehicle[Pet::Slot::Body].nItemID) == true )
		return ERROR_ITEM_EQUIPTOINVEN_FAIL;
#endif // #if defined( PRE_ADD_PCBANG_RENTAL_ITEM )

	// 서로 바꿔도 되는지 일단 장착 검사해보쟈
	int nInvenItemID = 0;
	short wInvenCount = 0;

	TItem NewItem = { 0, };
	const TItem *pInven = GetCashInventory(biDestInvenSerial);
	if (pInven)
	{
		if (!IsVehicleEquipItem(nSrcPartsIndex, pInven->nItemID)) return ERROR_ITEM_EQUIPMISMATCH;
		nInvenItemID = pInven->nItemID;
		wInvenCount = pInven->wCount;

		NewItem = *pInven;
	}

	if (m_pSession->GetDBConnection())
	{
		if (m_PetEquip.Vehicle[nSrcPartsIndex].nItemID > 0)// 파츠 해제
		{	
			m_pSession->GetDBConnection()->QueryChangeItemLocation(m_pSession, pParts->nSerial, 0, pParts->nItemID, 
				DBDNWorldDef::ItemLocation::CashInventory, 0, 1, false, 0, false, true, m_PetEquip.Vehicle[Pet::Slot::Body].nSerial, nSrcPartsIndex);
		}
	}

	TItem Equip = m_PetEquip.Vehicle[nSrcPartsIndex];
	m_PetEquip.Vehicle[nSrcPartsIndex] = NewItem;
	if (NewItem.nItemID > 0)
		_PopCashInventoryBySerial(NewItem.nSerial, NewItem.wCount);
	if (Equip.nItemID > 0) 
		_PushCashInventory(Equip);

	BroadcastChangePetParts(nSrcPartsIndex, m_PetEquip.Vehicle[nSrcPartsIndex]);

	m_pSession->SendMoveCashItem(pMove->cMoveType, nSrcPartsIndex, GetPetPartsEquip(nSrcPartsIndex), biPartsSerial, GetCashInventory(biPartsSerial), ERROR_NONE);
#if defined(_GAMESERVER)
	// 펫 스텟 적용
	DnActorHandle hActor = m_pSession->GetActorHandle();
	CDnPlayerActor *pPlayer = static_cast<CDnPlayerActor *>( hActor.GetPointer() );

	pPlayer->RefreshState( CDnActorState::RefreshEquip );
#endif
	return ERROR_NONE;
}

int CDNUserItem::_FromCashInvenToPetParts(const CSMoveCashItem *pMove)
{
	if (pMove->wCount <= 0) return ERROR_ITEM_NOTFOUND;
	int nDestPartsIndex = pMove->cCashEquipIndex;
	INT64 biSrcInvenSerial = pMove->biCashInvenSerial;

	if ((nDestPartsIndex < Pet::Slot::Accessory1) ||(nDestPartsIndex >= Pet::Slot::Max)) 
		return ERROR_ITEM_INDEX_UNMATCH;	// 인덱스가 이상하다

	const TItem *pInven = GetCashInventory(biSrcInvenSerial);
	if (!pInven) return ERROR_ITEM_NOTFOUND;	// 아이템 존재하지않음

	if (!IsVehicleEquipItem(nDestPartsIndex, pInven->nItemID)) return ERROR_ITEM_FAIL;		// 슬롯에 장착가능한지 검사

	const TItem *pParts = GetPetPartsEquip(nDestPartsIndex);
	INT64 biSerial = (pParts && (pParts->nSerial > 0)) ? pParts->nSerial : biSrcInvenSerial;

	if( g_pDataManager->bIsInstantItem( m_PetEquip.Vehicle[Pet::Slot::Body].nItemID) == true )
		return ERROR_ITEM_INVENTOEQUIP_FAIL;
#if defined( PRE_ADD_PCBANG_RENTAL_ITEM )
	if( g_pDataManager->IsPcCafeRentItem( m_PetEquip.Vehicle[Pet::Slot::Body].nItemID) == true )
		return ERROR_ITEM_INVENTOEQUIP_FAIL;
#endif // #if defined( PRE_ADD_PCBANG_RENTAL_ITEM )

	// 기존 파츠 해제
	if (m_PetEquip.Vehicle[nDestPartsIndex].nItemID > 0)
	{	
		m_pSession->GetDBConnection()->QueryChangeItemLocation(m_pSession, pParts->nSerial, 0, pParts->nItemID, 
			DBDNWorldDef::ItemLocation::CashInventory, 0, 1, false, 0, false, true, m_PetEquip.Vehicle[Pet::Slot::Body].nSerial, nDestPartsIndex);
	}
	// 신 파츠 장착
	if ( pInven->nItemID > 0 )
	{
		m_pSession->GetDBConnection()->QueryChangeItemLocation(m_pSession, pInven->nSerial, 0, pInven->nItemID, NULL, NULL, 1, false, 0, false, true, m_PetEquip.Vehicle[Pet::Slot::Body].nSerial, nDestPartsIndex);
	}

	TItem NewInven = *pInven;
	if (NewInven.nItemID > 0)
		_PopCashInventoryBySerial(NewInven.nSerial, NewInven.wCount);
	if (m_PetEquip.Vehicle[nDestPartsIndex].nItemID > 0)
		_PushCashInventory(m_PetEquip.Vehicle[nDestPartsIndex]);

	m_PetEquip.Vehicle[nDestPartsIndex] = NewInven;
	BroadcastChangePetParts(nDestPartsIndex, m_PetEquip.Vehicle[nDestPartsIndex]);

	m_pSession->SendMoveCashItem(pMove->cMoveType, nDestPartsIndex, GetPetPartsEquip(nDestPartsIndex), biSerial, GetCashInventory(biSerial), ERROR_NONE);
#if defined(_GAMESERVER)
	// 펫 스텟 적용
	DnActorHandle hActor = m_pSession->GetActorHandle();
	CDnPlayerActor *pPlayer = static_cast<CDnPlayerActor *>( hActor.GetPointer() );

	pPlayer->RefreshState( CDnActorState::RefreshEquip );
#endif
	return ERROR_NONE;
}

// packet 처리 함수
bool CDNUserItem::OnRecvMoveItem(const CSMoveItem *pPacket)
{
	int nRet = ERROR_NONE;

#if defined( _GAMESERVER )
	if( m_bMoveItemCheckGameMode == true )
	{
		switch( pPacket->cMoveType )
		{
			case MoveType_EquipToInven:
			case MoveType_InvenToEquip:
			{
				if( m_pSession->GetGameRoom() && m_pSession->GetGameRoom()->bIsZombieMode() )
					return false;
				break;
			}
		}
	}
#endif // #if defined( _GAMESERVER )

	switch(pPacket->cMoveType)
	{
	case MoveType_Equip: nRet = _Equip(pPacket); break;
	case MoveType_Inven: nRet = _Inven(pPacket); break;
	case MoveType_Ware: nRet = _Ware(pPacket); break;
	case MoveType_EquipToInven: nRet = _FromEquipToInven(pPacket); break;
	case MoveType_InvenToEquip: nRet = _FromInvenToEquip(pPacket); break;
	case MoveType_InvenToWare: nRet = _FromInvenToWare(pPacket); break;
	case MoveType_WareToInven: nRet = _FromWareToInven(pPacket); break;
	case MoveType_Glyph: nRet = ERROR_ITEM_DONTMOVE; break;	// Glyph(pPacket); break;
	case MoveType_GlyphToInven: nRet = _FromGlyphToInven(pPacket); break;
	case MoveType_InvenToGlyph: nRet = _FromInvenToGlyph(pPacket); break;
#if defined(PRE_ADD_TALISMAN_SYSTEM)
	case MoveType_Talisman: nRet = _Talisman(pPacket); break;
	case MoveType_TalismanToInven: nRet = _FromTalismanToInven(pPacket); break;
	case MoveType_InvenToTalisman: nRet = _FromInvenToTalisman(pPacket); break;	
#endif
	default: 
		nRet = ERROR_ITEM_DONTMOVE; break;
	}

	if (nRet != ERROR_NONE){
		m_pSession->SendMoveItem(pPacket->cMoveType, pPacket->cSrcIndex, pPacket->cDestIndex, NULL, NULL, nRet);
		return false;
	}

	return true;
}

bool CDNUserItem::OnRecvMoveCashItem(const CSMoveCashItem *pPacket)
{
	int nRet = ERROR_NONE;

	switch(pPacket->cMoveType)
	{
	case MoveType_CashEquip: nRet = ERROR_ITEM_DONTMOVE; break;
	case MoveType_CashInven: nRet = ERROR_ITEM_DONTMOVE; break;
	case MoveType_CashEquipToCashInven: nRet = _FromCashEquipToCashInven(pPacket); break;
	case MoveType_CashInvenToCashEquip: nRet = _FromCashInvenToCashEquip(pPacket); break;
	case MoveType_CashGlyph: nRet = ERROR_ITEM_DONTMOVE; break;
	case MoveType_CashGlyphToCashInven: nRet = _FromCashGlyphToCashInven(pPacket); break;
	case MoveType_CashInvenToCashGlyph: nRet = _FromCashInvenToCashGlyph(pPacket); break;
	case MoveType_VehicleBodyToVehicleInven: nRet = _FromVehicleBodyToVehicleInven(pPacket); break;
	case MoveType_VehicleInvenToVehicleBody: nRet = _FromVehicleInvenToVehicleBody(pPacket); break;
	case MoveType_VehiclePartsToCashInven: nRet = _FromVehiclePartsToCashInven(pPacket); break;
	case MoveType_CashInvenToVehicleParts: nRet = _FromCashInvenToVehicleParts(pPacket); break;
	case MoveType_PetBodyToPetInven: nRet = _FromPetBodyToVehicleInven(pPacket); break;
	case MoveType_PetInvenToPetBody: nRet = _FromVehicleInvenToPetBody(pPacket); break;
 	case MoveType_PetPartsToCashInven: nRet = _FromPetPartsToCashInven(pPacket); break;
 	case MoveType_CashInvenToPetParts: nRet = _FromCashInvenToPetParts(pPacket); break;
	default: 
		nRet = ERROR_ITEM_DONTMOVE; break;
	}

	if (nRet != ERROR_NONE){
		m_pSession->SendMoveCashItem(pPacket->cMoveType, pPacket->cCashEquipIndex, NULL, pPacket->biCashInvenSerial, NULL, nRet);
		return false;
	}

	return true;
}
#if defined(PRE_ADD_SERVER_WAREHOUSE)
bool CDNUserItem::OnRecvMoveServerWareItem(const CSMoveServerWare* pPacket)
{
	int nRet = ERROR_NONE;
	switch(pPacket->cMoveType)
	{	
#if defined(PRE_ADD_SERVER_WAREHOUSE)
	case MoveType_InvenToServerWare : nRet = _FromInvenToServerWareHouse(pPacket); break;
	case MoveType_ServerWareToInven : nRet = _FromServerWareHouseToInven(pPacket); break;	
#endif

	default: 
		nRet = ERROR_ITEM_DONTMOVE; break;
	}

	if (nRet != ERROR_NONE){
		m_pSession->SendMoveServerWareItem(pPacket->cMoveType, pPacket->cInvenIndex, pPacket->biWareItemSerial, NULL, NULL, nRet);		
		return false;
	}
	return true;
}

bool CDNUserItem::OnRecvMoveServerWareCashItem(const CSMoveCashServerWare* pPacket)
{
	int nRet = ERROR_NONE;

	switch(pPacket->cMoveType)
	{	
#if defined(PRE_ADD_SERVER_WAREHOUSE)
	case MoveType_CashToServerWare : nRet = _FromCashToServerWareHouse(pPacket); break;
	case MoveType_ServerWareToCash : nRet = _FromServerWareHouseToCash(pPacket); break;
#endif
	default: 
		nRet = ERROR_ITEM_DONTMOVE; break;
	}

	if (nRet != ERROR_NONE){
		m_pSession->SendMoveServerWareCashItem(pPacket->cMoveType, pPacket->biItemSerial, NULL, NULL, nRet);		
		return false;
	}
	return true;
}
#endif // #if defined(PRE_ADD_SERVER_WAREHOUSE)

#if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
bool CDNUserItem::OnRecvPickUp(TItem& resultItemInfo, int nItemID, short wCount, int nRandomSeed, char cOption, int nEnchantID)
#else	// #if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
bool CDNUserItem::OnRecvPickUp(TItem& resultItemInfo, int nItemID, short wCount, int nRandomSeed, char cOption)
#endif	// #if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
{
	INT64 nSerial = 0;
	std::vector<TSaveItemInfo> infoList;
	infoList.clear();

	TItem AddItem = { 0, };
#if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
	MakeItemStruct(nItemID, AddItem, 0, nEnchantID);
#else	// #if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
	MakeItemStruct(nItemID, AddItem);
#endif	// #if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)	
	TItemData *pItemData = g_pDataManager->GetItemData(nItemID);
	if (!pItemData) return false;
	if (pItemData->cReversion == ITEMREVERSION_BELONG)
		AddItem.bSoulbound = pItemData->IsSealed ? false : true;
	AddItem.wCount = wCount;
	AddItem.nRandomSeed = nRandomSeed;
	if( cOption != -1 ) AddItem.cOption = cOption;
	int nRet = _CreateInvenItemEx(AddItem, DBDNWorldDef::AddMaterializedItem::Pick, m_pSession->GetPartyID(), infoList, true, CREATEINVEN_PICKUP);

	if (infoList.empty() == false)
		resultItemInfo = infoList[0].Item;

	if (nRet != ERROR_NONE){
		m_pSession->SendPickUp(nRet, -1, NULL, wCount);
		return false;
	}

	m_pSession->GetQuest()->OnAddItem(nItemID, wCount);

	return true;
}

bool CDNUserItem::OnRecvRemoveItem(const CSRemoveItem *pPacket)
{
	if (pPacket->wCount < 0){
		m_pSession->SendRemoveItem(pPacket->cType, pPacket->cSlotIndex, NULL, ERROR_ITEM_NOTFOUND);
		return false;
	}

	int nItemID = 0;
	INT64 nItemSerial = 0;
	int nItemCount = 0;
	bool bSoulbound = false;
	bool bEquipSub = false;	// 한벌옷

	switch( pPacket->cType )
	{
		case ITEMPOSITION_EQUIP:
			if (!_CheckRangeEquipIndex(pPacket->cSlotIndex)) return false;
			if (m_Equip[pPacket->cSlotIndex].nSerial != pPacket->biItemSerial) return false;
#if defined(PRE_ADD_EQUIPLOCK)
			if(IsLockItem(DBDNWorldDef::ItemLocation::Equip, pPacket->cSlotIndex))
			{
				m_pSession->SendRemoveItem(pPacket->cType, pPacket->cSlotIndex, NULL, ERROR_ITEM_REMOVE_DISALLOWANCE);
				return false;
			}
#endif	// #if defined(PRE_ADD_EQUIPLOCK)
			nItemSerial = m_Equip[pPacket->cSlotIndex].nSerial;
			nItemCount = m_Equip[pPacket->cSlotIndex].wCount;
			nItemID = m_Equip[pPacket->cSlotIndex].nItemID;
			bSoulbound = m_Equip[pPacket->cSlotIndex].bSoulbound;

			bEquipSub = g_pDataManager->IsSubParts(nItemID);
			break;

		case ITEMPOSITION_INVEN:
			if (!_CheckRangeInventoryIndex(pPacket->cSlotIndex)) return false;
			if (m_Inventory[pPacket->cSlotIndex].nSerial != pPacket->biItemSerial) return false;

			nItemSerial = m_Inventory[pPacket->cSlotIndex].nSerial;
			nItemCount = m_Inventory[pPacket->cSlotIndex].wCount;
			nItemID = m_Inventory[pPacket->cSlotIndex].nItemID;
			bSoulbound = m_Inventory[pPacket->cSlotIndex].bSoulbound;
			break;

		case ITEMPOSITION_QUESTINVEN:
			if (!_CheckRangeQuestInventoryIndex(pPacket->cSlotIndex)) return false;
			if (m_QuestInventory[pPacket->cSlotIndex].nSerial != pPacket->biItemSerial) return false;

			nItemID = m_QuestInventory[pPacket->cSlotIndex].nItemID;
			nItemSerial = m_QuestInventory[pPacket->cSlotIndex].nSerial;
			nItemCount = m_QuestInventory[pPacket->cSlotIndex].wCount;
			break;

		case ITEMPOSITION_GLYPH:
			if (!_CheckRangeGlyphIndex(pPacket->cSlotIndex)) return false;
			if (m_Glyph[pPacket->cSlotIndex].nSerial != pPacket->biItemSerial) return false;

			nItemID = m_Glyph[pPacket->cSlotIndex].nItemID;
			nItemSerial = m_Glyph[pPacket->cSlotIndex].nSerial;
			nItemCount = m_Glyph[pPacket->cSlotIndex].wCount;
			bSoulbound = m_Glyph[pPacket->cSlotIndex].bSoulbound;
			break;
#if defined(PRE_ADD_TALISMAN_SYSTEM)
		case ITEMPOSITION_TALISMAN:
			{
#if defined(_GAMESERVER)
				if( !m_pSession->GetGameRoom() ) return false;	
				if( CDnWorld::GetInstance( m_pSession->GetGameRoom() ).GetMapType() != EWorldEnum::MapTypeWorldMap )
				{
					m_pSession->SendRemoveItem(pPacket->cType, pPacket->cSlotIndex, NULL, ERROR_TALISMAN_FAIL_LOACTE);
					return false;
				}
#endif
				if (!_CheckRangeTalismanIndex(pPacket->cSlotIndex)) return false;
				if (m_Talisman[pPacket->cSlotIndex].nSerial != pPacket->biItemSerial) return false;

				int nCharge = g_pDataManager->GetTalismanCostFromPlayerCommonLevelTable(m_pSession->GetLevel());
				if (nCharge <= 0) return false;
				if (m_pSession->CheckEnoughCoin(nCharge) == false)
				{
					m_pSession->SendRemoveItem(pPacket->cType, pPacket->cSlotIndex, NULL, ERROR_ITEM_INSUFFICIENCY_MONEY);
					return false;
				}
				nItemID = m_Talisman[pPacket->cSlotIndex].nItemID;
				nItemSerial = m_Talisman[pPacket->cSlotIndex].nSerial;
				nItemCount = m_Talisman[pPacket->cSlotIndex].wCount;
				bSoulbound = m_Talisman[pPacket->cSlotIndex].bSoulbound;				
			}
			break;
#endif
		default: 
			return false;
	}

	if (nItemCount <= 0){
		m_pSession->SendRemoveItem(pPacket->cType, pPacket->cSlotIndex, NULL, ERROR_ITEM_NOTFOUND);
		return false;
	}

	if (!g_pDataManager->IsItemDestruction(nItemID)){	// 파괴불가 아이템 체크
		m_pSession->SendRemoveItem(pPacket->cType, pPacket->cSlotIndex, NULL, ERROR_ITEM_REMOVE_DISALLOWANCE);
		return false;
	}

	int nResult = ERROR_ITEM_FAIL;
	TItem UpdateItem = { 0, };

	switch( pPacket->cType )
	{
		case ITEMPOSITION_EQUIP:
			if (bEquipSub){
				TakeOffOnePiece(nItemID);	// 안에서 디비저장
			}

			nResult = _PopEquipSlot( pPacket->cSlotIndex, pPacket->wCount );
			UpdateItem = m_Equip[pPacket->cSlotIndex];
			break;

		case ITEMPOSITION_INVEN:
			nResult = _PopInventorySlotItem( pPacket->cSlotIndex, pPacket->wCount );
			UpdateItem = m_Inventory[pPacket->cSlotIndex];
			break;

		case ITEMPOSITION_QUESTINVEN:
			nResult = _PopQuestInventorySlot( pPacket->cSlotIndex, pPacket->wCount );
			UpdateItem.nItemID = m_QuestInventory[pPacket->cSlotIndex].nItemID;
			UpdateItem.nSerial = m_QuestInventory[pPacket->cSlotIndex].nSerial;
			UpdateItem.wCount = m_QuestInventory[pPacket->cSlotIndex].wCount;
			break;

		case ITEMPOSITION_GLYPH:
			nResult = _PopGlyphSlot(pPacket->cSlotIndex, pPacket->wCount);

			UpdateItem.nItemID = m_Glyph[pPacket->cSlotIndex].nItemID;
			UpdateItem.nSerial = m_Glyph[pPacket->cSlotIndex].nSerial;
			UpdateItem.wCount = m_Glyph[pPacket->cSlotIndex].wCount;
			break;

#if defined(PRE_ADD_TALISMAN_SYSTEM)
		case ITEMPOSITION_TALISMAN:
			{
				int nCharge = g_pDataManager->GetTalismanCostFromPlayerCommonLevelTable(m_pSession->GetLevel());
				if (nCharge <= 0)
				{
					_DANGER_POINT();
					return false;
				}
				m_pSession->DelCoin(nCharge, DBDNWorldDef::CoinChangeCode::TalismanSlotChange, 0, true);
				nResult = _PopTalismanSlot(pPacket->cSlotIndex, pPacket->wCount);

				UpdateItem.nItemID = m_Talisman[pPacket->cSlotIndex].nItemID;
				UpdateItem.nSerial = m_Talisman[pPacket->cSlotIndex].nSerial;
				UpdateItem.wCount = m_Talisman[pPacket->cSlotIndex].wCount;
			}
			break;
#endif
	}
	
	m_pSession->SendRemoveItem(pPacket->cType, pPacket->cSlotIndex, &UpdateItem, nResult);

#if defined( PRE_ADD_NAMEDITEM_SYSTEM )
#if defined( _VILLAGESERVER )
	if( g_pDataManager->GetItemRank(nItemID) == ITEMRANK_SSS )
	{
		int nNamedItemID = 0;
		TItem* NameItem = GetItemRank(ITEMRANK_SSS);
		if(NameItem)
		{
			nNamedItemID = NameItem->nItemID;
		}
		m_pSession->GetParamData()->nIndex = nNamedItemID;
		m_pSession->SendUserLocalMessage(0, FM_HAVE_NAMEDITEM);
	}
#endif
#endif

	if (nResult == ERROR_NONE){
		if (m_pSession->GetDBConnection()){
			bool bAllFlag = (nItemCount == pPacket->wCount) ? true : false;
			m_pSession->GetDBConnection()->QueryUseItem(m_pSession, DBDNWorldDef::UseItem::Destroy, nItemSerial, pPacket->wCount, bAllFlag );
		}
	}

	return true;
}

int CDNUserItem::OnRecvRemoveCash(INT64 biItemSerial, bool bRecovery)
{
	const TItem *pCashItem = GetCashInventory(biItemSerial);
	if (!pCashItem) return ERROR_ITEM_NOTFOUND;

	if (!g_pDataManager->IsCashErasableType(pCashItem->nItemID)) return ERROR_ITEM_FAIL;

	if (bRecovery){	// 복구
		CTimeSet RemoveCashSet;
		if (RemoveCashSet.GetTimeT64_LC() > pCashItem->tExpireDate)
			return ERROR_ITEM_INSUFFICIENTY_PERIOD_RECOVERYCASH;

		m_pSession->GetDBConnection()->QueryRecoverCashItem(m_pSession, pCashItem);
	}
	else{
		m_pSession->GetDBConnection()->QueryDelCashItem(m_pSession, pCashItem);
	}

	return ERROR_NONE;
}

// Quest Inventory
int CDNUserItem::AddQuestInventory(int nItemID, short wCount, int nQuestID, int nLogCode)
{
	if ((nItemID <= 0) ||(wCount <= 0)) return ERROR_ITEM_NOTFOUND;
	if (!IsValidSpaceQuestInventorySlot(nItemID, wCount)) return ERROR_ITEM_INVENTORY_NOTENOUGH;	// 넣을 공간이 있는가 체크

	TItemData *pItemData = g_pDataManager->GetItemData(nItemID);
	if (!pItemData) return ERROR_ITEM_NOTFOUND;
	if (pItemData->nOverlapCount <= 0) return ERROR_ITEM_FAIL;

	if (pItemData->nType != ITEMTYPE_QUEST) return ERROR_ITEM_NOTFOUND;

	// 집어넣을 아이템 세팅
	TQuestItem AddItem = { 0, };
	AddItem.nItemID = nItemID;
	AddItem.wCount = wCount;
	AddItem.nSerial = MakeItemSerial();

	TItem SaveItem = { 0, };

	int nBlankIndex = -1;

	if (pItemData->nOverlapCount > 1){	// 겹치는 아이템이라면
		if (pItemData->nOverlapCount < wCount){		// max보다 넘치면
			int nBundle = wCount / pItemData->nOverlapCount;	// 몇묶음인지 계산해서

			for (int i = 0; i < nBundle; i++){		// 묶음만큼 넣어주고
				nBlankIndex = FindBlankQuestInventorySlot();

				AddItem.wCount = pItemData->nOverlapCount;
				AddItem.nSerial = MakeItemSerial();	// 여러번 나눠야하기때문에 serial을 생성해야한다.

				if (_PushQuestInventorySlot(nBlankIndex, AddItem) == ERROR_NONE){
					SaveItem.nItemID = AddItem.nItemID;
					SaveItem.nSerial = AddItem.nSerial;
					SaveItem.wCount = AddItem.wCount;
					SaveItem.bEternity = true;

					DBSendAddMaterializedItem(nBlankIndex, DBDNWorldDef::AddMaterializedItem::QuestReward, nQuestID, SaveItem, 0, 0, (INT64)0, DBDNWorldDef::ItemLocation::QuestInventory, 0, false, 0);

					m_pSession->SendRefreshQuestInven(nBlankIndex, m_QuestInventory[nBlankIndex]);
				}
			}

			bool bMerge = true;
			int nRemain = wCount % pItemData->nOverlapCount;
			if (nRemain > 0){		// 혹시 짜투리(?)가 있으면
				nBlankIndex = FindOverlapQuestInventorySlot(AddItem.nItemID, nRemain);
				if (nBlankIndex < 0){
					bMerge = false;
					AddItem.nSerial = MakeItemSerial();
					nBlankIndex = FindBlankQuestInventorySlot();		// 없다면 빈공간 있는지 검사
					if (nBlankIndex < 0){		// 그래도 없다면 나가~
						return ERROR_ITEM_INVENTORY_NOTENOUGH;
					}
				}
				else
					AddItem.nSerial = m_QuestInventory[nBlankIndex].nSerial;

				AddItem.wCount = nRemain;

				if (_PushQuestInventorySlot(nBlankIndex, AddItem) == ERROR_NONE){
					SaveItem.nItemID = AddItem.nItemID;
					SaveItem.nSerial = AddItem.nSerial;
					SaveItem.wCount = AddItem.wCount;
					SaveItem.bEternity = true;

					DBSendAddMaterializedItem(nBlankIndex, DBDNWorldDef::AddMaterializedItem::QuestReward, nQuestID, SaveItem, 0, 0, (INT64)0, DBDNWorldDef::ItemLocation::QuestInventory, 0, bMerge, AddItem.nSerial);

					m_pSession->SendRefreshQuestInven(nBlankIndex, m_QuestInventory[nBlankIndex]);
				}
			}

			return ERROR_NONE;
		}
		else {	// 안넘치는 경우
			bool bMerge = true;
			nBlankIndex = FindOverlapQuestInventorySlot(AddItem.nItemID, wCount);
			if (nBlankIndex < 0){
				bMerge = false;
				AddItem.nSerial = MakeItemSerial();
				nBlankIndex = FindBlankQuestInventorySlot();		// 없다면 빈공간 있는지 검사
				if (nBlankIndex < 0){		// 그래도 없다면 나가~
					return ERROR_ITEM_INVENTORY_NOTENOUGH;
				}
			}
			else
				AddItem.nSerial = m_QuestInventory[nBlankIndex].nSerial;

			AddItem.wCount = wCount;

			if (_PushQuestInventorySlot(nBlankIndex, AddItem) == ERROR_NONE){
				SaveItem.nItemID = AddItem.nItemID;
				SaveItem.nSerial = AddItem.nSerial;
				SaveItem.wCount = AddItem.wCount;
				SaveItem.bEternity = true;

				DBSendAddMaterializedItem(nBlankIndex, DBDNWorldDef::AddMaterializedItem::QuestReward, nQuestID, SaveItem, 0, 0, (INT64)0, DBDNWorldDef::ItemLocation::QuestInventory, 0, bMerge, AddItem.nSerial); 

				m_pSession->SendRefreshQuestInven(nBlankIndex, m_QuestInventory[nBlankIndex]);

				return ERROR_NONE;
			}
		}
	}
	else {		// 겹치지 않는 아이템
		AddItem.wCount = pItemData->nOverlapCount;

		for (int i = 0; i < wCount; i++){
			nBlankIndex = FindBlankQuestInventorySlot();
			if (nBlankIndex >= 0){
				AddItem.nSerial = MakeItemSerial();
				if (_PushQuestInventorySlot(nBlankIndex, AddItem) == ERROR_NONE){
					SaveItem.nItemID = AddItem.nItemID;
					SaveItem.nSerial = AddItem.nSerial;
					SaveItem.wCount = AddItem.wCount;
					SaveItem.bEternity = true;

					DBSendAddMaterializedItem(nBlankIndex, DBDNWorldDef::AddMaterializedItem::QuestReward, nQuestID, SaveItem, 0, 0, (INT64)0, DBDNWorldDef::ItemLocation::QuestInventory, 0, false, 0);

					m_pSession->SendRefreshQuestInven(nBlankIndex, m_QuestInventory[nBlankIndex]);
				}
			}
		}

		return ERROR_NONE;
	}

	return ERROR_ITEM_NOTFOUND;
}

int CDNUserItem::DeleteQuestInventory(int nItemID, short wCount, int nQuestID, int nLogCode)
{
	if ((nItemID <= 0) ||(wCount <= 0)) return ERROR_ITEM_NOTFOUND;

	int nCount = GetQuestInventoryItemCount(nItemID);
	if (nCount < wCount) return ERROR_ITEM_INVENTORY_NOTENOUGH;	// 아이템 공간부족

	TItemData *pItemData = g_pDataManager->GetItemData(nItemID);
	if (!pItemData) return ERROR_ITEM_NOTFOUND;
	if (pItemData->nOverlapCount <= 0) return ERROR_ITEM_FAIL;

	if (pItemData->nType != ITEMTYPE_QUEST) return ERROR_ITEM_NOTFOUND;

	int nIndex = 0;
	TItem Item = { 0, };
	if (pItemData->nOverlapCount == 1){	// 셀수없는 아이템
		for (int i = 0; i < wCount; i++){
			nIndex = FindQuestInventorySlot(nItemID, 1);
			if (nIndex < 0) return ERROR_ITEM_NOTFOUND;

			INT64 nSerial = m_QuestInventory[nIndex].nSerial;
			Item.nItemID = m_QuestInventory[nIndex].nItemID;
			Item.nSerial = m_QuestInventory[nIndex].nSerial;
			Item.wCount = m_QuestInventory[nIndex].wCount;

			_PopQuestInventorySlot(nIndex, 1);
			m_pSession->SendRefreshQuestInven(nIndex, m_QuestInventory[nIndex]);

			switch(nLogCode)
			{
			case DBDNWorldDef::UseItem::Use:
			case DBDNWorldDef::UseItem::DeCompose:
			case DBDNWorldDef::UseItem::Destroy:
				{
					m_pSession->GetDBConnection()->QueryUseItem(m_pSession, nLogCode, Item.nSerial, Item.wCount, true );
				}
			}
		}
	}
	else {	// 셀수있는 아이템
		int nTotal = wCount;
		for (int i = 0; i < QUESTINVENTORYMAX; i++){
			if (m_QuestInventory[i].nItemID == nItemID){
				if (nTotal > 0){
					if (m_QuestInventory[i].wCount >= nTotal){	// 지울것보다 많거나 같다
						nCount = nTotal;
					}
					else {	// 지울것보다 적을때
						nCount = m_QuestInventory[i].wCount;
					}

					INT64 nSerial = m_QuestInventory[i].nSerial;
					Item.nItemID = m_QuestInventory[i].nItemID;
					Item.nSerial = m_QuestInventory[i].nSerial;
					Item.wCount = m_QuestInventory[i].wCount;

					_PopQuestInventorySlot(i, nCount);
					m_pSession->SendRefreshQuestInven(i, m_QuestInventory[i]);

					switch(nLogCode)
					{
					case DBDNWorldDef::UseItem::Use:
					case DBDNWorldDef::UseItem::DeCompose:
					case DBDNWorldDef::UseItem::Destroy:
						{
							bool bAllFlag = (Item.wCount == nCount) ? true : false;
							m_pSession->GetDBConnection()->QueryUseItem(m_pSession, nLogCode, Item.nSerial, nCount, bAllFlag );
						}
					}

					nTotal -= nCount;
					if (nTotal <= 0) return ERROR_NONE;
				}
			}
		}
	}

	return ERROR_NONE;
}

int CDNUserItem::_CreateInvenItemEx(const TItem &CreateItem, int nLogCode, INT64 biFKey, std::vector<TSaveItemInfo> &VecItemList, bool bInsertList/* = false*/, char cType/* = CREATEINVEN_ETC*/)
{
	if ((CreateItem.nItemID <= 0) ||(CreateItem.wCount <= 0)) return ERROR_ITEM_NOTFOUND;
	if (!IsValidSpaceInventorySlot(CreateItem.nItemID, CreateItem.wCount, CreateItem.bSoulbound, CreateItem.cSealCount, CreateItem.bEternity)) return ERROR_ITEM_INVENTORY_NOTENOUGH;	// 넣을 공간이 있는가 체크

	TItemData *pItemData = g_pDataManager->GetItemData(CreateItem.nItemID);
	if (!pItemData) return ERROR_ITEM_NOTFOUND;
	if (pItemData->nOverlapCount <= 0) return ERROR_ITEM_FAIL;

	// 집어넣을 아이템 세팅
	TItem AddItem = CreateItem;
	int nBlankIndex = -1;

	if (pItemData->nOverlapCount > 1){	// 겹치는 아이템이라면
		if (pItemData->nOverlapCount < CreateItem.wCount){		// max보다 넘치면
			int nBundle = CreateItem.wCount / pItemData->nOverlapCount;	// 몇묶음인지 계산해서

			for (int i = 0; i < nBundle; i++){		// 묶음만큼 넣어주고
				nBlankIndex = FindBlankInventorySlot();

				AddItem.wCount = pItemData->nOverlapCount;
				AddItem.nSerial = MakeItemSerial();	// 여러번 나눠야하기때문에 serial을 생성해야한다.

				if (_PushInventorySlotItem(nBlankIndex, AddItem) == ERROR_NONE){
					if (bInsertList){
						TSaveItemInfo ItemInfo;
						ItemInfo.SetInfo(nBlankIndex, AddItem, false);
						VecItemList.push_back(ItemInfo);
					}

					if (nLogCode > 0){
						DBSendAddMaterializedItem(nBlankIndex, nLogCode, biFKey, AddItem, 0, 0, (INT64)0, DBDNWorldDef::ItemLocation::Inventory, 0, false, 0);
					}

					m_pSession->SendRefreshInvenPickUpByType(cType, ERROR_NONE, nBlankIndex, &m_Inventory[nBlankIndex], AddItem.wCount);
				}
			}

			bool bMerge = true;
			int nRemain = CreateItem.wCount % pItemData->nOverlapCount;
			if (nRemain > 0){		// 혹시 짜투리(?)가 있으면
				bMerge = false;
				AddItem.nSerial = MakeItemSerial();
				nBlankIndex = FindOverlapInventorySlot(AddItem.nItemID, nRemain, AddItem.bSoulbound, AddItem.cSealCount, AddItem.bEternity);
				if (nBlankIndex < 0){
					nBlankIndex = FindBlankInventorySlot();		// 없다면 빈공간 있는지 검사
					if (nBlankIndex < 0){		// 그래도 없다면 나가~
						return ERROR_ITEM_INVENTORY_NOTENOUGH;
					}
				}
				else
					AddItem.nSerial = m_Inventory[nBlankIndex].nSerial;

				AddItem.wCount = nRemain;

				if (_PushInventorySlotItem(nBlankIndex, AddItem) == ERROR_NONE){
					if (bInsertList){
						TSaveItemInfo ItemInfo;
						ItemInfo.SetInfo(nBlankIndex, AddItem, bMerge);
						VecItemList.push_back(ItemInfo);
					}

					if (nLogCode > 0){
						DBSendAddMaterializedItem(nBlankIndex, nLogCode, biFKey, AddItem, 0, 0, (INT64)0, DBDNWorldDef::ItemLocation::Inventory, 0, bMerge, AddItem.nSerial);
					}

					m_pSession->SendRefreshInvenPickUpByType(cType, ERROR_NONE, nBlankIndex, &m_Inventory[nBlankIndex], AddItem.wCount);
					
					m_pSession->GetEventSystem()->OnEvent( EventSystem::OnItemGain, 2, EventSystem::ItemID, CreateItem.nItemID, EventSystem::ItemCount, GetInventoryItemCount(CreateItem.nItemID) );
				}
			}

			return ERROR_NONE;
		}
		else {	// 안넘치는 경우
			bool bMerge = true;
			nBlankIndex = FindOverlapInventorySlot(AddItem.nItemID, CreateItem.wCount, AddItem.bSoulbound, AddItem.cSealCount, AddItem.bEternity);
			if (nBlankIndex < 0){
				bMerge = false;
				AddItem.nSerial = MakeItemSerial();
				nBlankIndex = FindBlankInventorySlot();		// 없다면 빈공간 있는지 검사
				if (nBlankIndex < 0){		// 그래도 없다면 나가~
					return ERROR_ITEM_INVENTORY_NOTENOUGH;
				}
				if( cType == CREATEINVEN_REPURCHASE && CreateItem.nSerial > 0 )
					AddItem.nSerial = CreateItem.nSerial;
			}
			else
				AddItem.nSerial = m_Inventory[nBlankIndex].nSerial;

			AddItem.wCount = CreateItem.wCount;

			if (_PushInventorySlotItem(nBlankIndex, AddItem) == ERROR_NONE){
				if (bInsertList){
					TSaveItemInfo ItemInfo;
					ItemInfo.SetInfo(nBlankIndex, AddItem, bMerge);
					VecItemList.push_back(ItemInfo);
				}

				if (nLogCode > 0){
					DBSendAddMaterializedItem(nBlankIndex, nLogCode, biFKey, AddItem, 0, 0, (INT64)0, DBDNWorldDef::ItemLocation::Inventory, 0, bMerge, AddItem.nSerial);
				}

				m_pSession->SendRefreshInvenPickUpByType(cType, ERROR_NONE, nBlankIndex, &m_Inventory[nBlankIndex], AddItem.wCount);

				m_pSession->GetEventSystem()->OnEvent( EventSystem::OnItemGain, 2, EventSystem::ItemID, CreateItem.nItemID, EventSystem::ItemCount, GetInventoryItemCount(CreateItem.nItemID) );

				return ERROR_NONE;
			}
		}
	}
	else {		// 겹치지 않는 아이템
		AddItem.wCount = pItemData->nOverlapCount;

		for (int i = 0; i < CreateItem.wCount; i++){
			nBlankIndex = FindBlankInventorySlot();
			if (nBlankIndex >= 0)
			{
				if( cType == CREATEINVEN_REPURCHASE && CreateItem.nSerial > 0 )
					AddItem.nSerial = CreateItem.nSerial;
				else if (CreateItem.wCount != AddItem.wCount) 
					AddItem.nSerial = MakeItemSerial();

				if (_PushInventorySlotItem(nBlankIndex, AddItem) == ERROR_NONE){
					if (bInsertList){
						TSaveItemInfo ItemInfo;
						ItemInfo.SetInfo(nBlankIndex, AddItem, false);
						VecItemList.push_back(ItemInfo);
					}

					if (nLogCode > 0){
						DBSendAddMaterializedItem(nBlankIndex, nLogCode, biFKey, AddItem, 0, 0, (INT64)0, DBDNWorldDef::ItemLocation::Inventory, 0, false, 0);
					}

					m_pSession->SendRefreshInvenPickUpByType(cType, ERROR_NONE, nBlankIndex, &m_Inventory[nBlankIndex], AddItem.wCount);

					m_pSession->GetEventSystem()->OnEvent( EventSystem::OnItemGain, 2, EventSystem::ItemID, CreateItem.nItemID, EventSystem::ItemCount, GetInventoryItemCount(CreateItem.nItemID) );
				}
			}
		}

		return ERROR_NONE;
	}

	return ERROR_ITEM_NOTFOUND;
}

int CDNUserItem::CreateInvenItem1(int nItemID, short wCount, char cOption, int nRandomSeed, int nLogCode, INT64 biFKey, char cType)
{
	std::vector<TSaveItemInfo> VecItemList;
	VecItemList.clear();

	TItem AddItem = { 0, };
	MakeItemStruct(nItemID, AddItem, 0, 0);
	AddItem.wCount = wCount;
	if ( cOption != -1 ) AddItem.cOption = cOption;
	if ( nRandomSeed != -1 ) AddItem.nRandomSeed = nRandomSeed;

	return _CreateInvenItemEx(AddItem, nLogCode, biFKey, VecItemList, false, cType);
}

int CDNUserItem::CreateInvenItem2(int nItemID, short wCount, std::vector<TSaveItemInfo> &VecItemList, char cType/* = CREATEINVEN_ETC*/, int nShopItemPeriod/* = 0*/)
{
	TItem AddItem = { 0, };
	MakeItemStruct(nItemID, AddItem, nShopItemPeriod, 0);
	AddItem.wCount = wCount;

	return _CreateInvenItemEx(AddItem, 0, 0, VecItemList, true, cType);
}

int CDNUserItem::CreateInvenWholeItem(const TItem &Item, int nLogCode, INT64 biFKey, char cType/* = CREATEINVEN_ETC*/)
{
	std::vector<TSaveItemInfo> VecItemList;
	VecItemList.clear();

	return _CreateInvenItemEx(Item, nLogCode, biFKey, VecItemList, false, cType);
}

int CDNUserItem::CreateInvenWholeItemByIndex(int nInvenIndex, const TItem &Item)
{
	if (!_CheckRangeInventoryIndex(nInvenIndex)) return ERROR_ITEM_INDEX_UNMATCH;

	if (VerifyInventorySlotPacket(nInvenIndex, Item) == false){
		m_pSession->DetachConnection(L"CreateInvenItem VerifyInventorySlotPacket");
		return ERROR_ITEM_FAIL;
	}

	m_Inventory[nInvenIndex] = Item;		// 일단 걍 박아놓자. 디비에서 증분이 아닌 전체값을 전달해주기때문에 걍 이렇게 해본다.
	m_pSession->SendRefreshInven(nInvenIndex, &Item);

	// 인벤에 누적된 아이템 정보를 API로 알려준다.
	TItemData *pItemData = g_pDataManager->GetItemData(Item.nItemID);
	if (pItemData && pItemData->IsCollectingEvent)
	{
		int nCount = GetInventoryItemCount(pItemData->nItemID);
		m_pSession->GetQuest()->OnAddItemEx(pItemData->nItemID, nCount);
	}

	return ERROR_NONE;
}

int CDNUserItem::_CreateCashInvenItemEx(int nItemSN, BYTE cPayMethodCode, int nLogCode, INT64 biFKey, int nPeriod, int nPrice, const TItem &CreateItem)
{
	if (CreateItem.nItemID <= 0) return ERROR_ITEM_NOTFOUND;

	int nCount = CreateItem.wCount;
	if (nItemSN > 0)
		nCount = g_pDataManager->GetCashCommodityCount(nItemSN);

	TItemData *pItemData = g_pDataManager->GetItemData(CreateItem.nItemID);
	if (!pItemData) return ERROR_ITEM_NOTFOUND;
	if (pItemData->nOverlapCount <= 0) return ERROR_ITEM_FAIL;
	if (!pItemData->IsCash) return ERROR_ITEM_FAIL;	// 캐쉬템이 아니면 넣을 수 없다

	// 집어넣을 아이템 세팅
	TItem AddItem;
	memset(&AddItem, 0, sizeof(TItem));

	MakeCashItemStruct(nItemSN, CreateItem.nItemID, AddItem, CreateItem.cOption, nPeriod);	// 여기서 정보 담아온다
	AddItem.wCount = nCount;
	if (CreateItem.nSerial != 0) AddItem.nSerial = CreateItem.nSerial;

	int nItemPrice = nPrice;
	int nItemPeriod = nPeriod;
	if (nItemSN > 0){
		nItemPeriod = g_pDataManager->GetCashCommodityPeriod(nItemSN);
		nItemPrice = g_pDataManager->GetCashCommodityPrice(nItemSN);
	}

	switch(pItemData->nType)
	{
	case ITEMTYPE_VEHICLE:
	case ITEMTYPE_PET:
		{
			TVehicle AddVehicle;
			memset(&AddVehicle, 0, sizeof(AddVehicle));

			MakeVehicleItemStruct(AddItem, AddVehicle);

			if (_PushVehicleInventory(AddVehicle) == ERROR_NONE){
				if (nLogCode > 0){
					BYTE cItemLocationCode = DBDNWorldDef::ItemLocation::VehicleInventory;
					if (pItemData->nType == ITEMTYPE_PET)
						cItemLocationCode = DBDNWorldDef::ItemLocation::PetInventory;
					DBSendAddMaterializedItem(0, nLogCode, biFKey, AddItem, nItemPrice, nItemPeriod, (INT64)0, cItemLocationCode, cPayMethodCode, false, 0, &AddVehicle);
				}

				if (nItemSN > 0)
					m_pSession->DelCashBalance(nItemPrice);	// 캐쉬빼주기

				m_pSession->SendRefreshVehicleInven(AddVehicle);
			}
		}
		break;

	default:
		{
			if (_PushCashInventory(AddItem) == ERROR_NONE){
				if (nLogCode > 0){
					DBSendAddMaterializedItem(0, nLogCode, biFKey, AddItem, nItemPrice, nItemPeriod, (INT64)0, DBDNWorldDef::ItemLocation::CashInventory, cPayMethodCode, false, 0);
				}

				if (nItemSN > 0)
					m_pSession->DelCashBalance(nItemPrice);	// 캐쉬빼주기

				m_pSession->SendRefreshCashInven(AddItem);
			}
		}
		break;
	}

	return ERROR_NONE;
}

int CDNUserItem::CreateCashInvenItem(int nItemID, int nItemCount, int nLogCode, char cOption/* = -1*/, int nPeriod/* = 0*/, int nPrice/* = 0*/, INT64 biFKey/* = 0*/, BYTE cPayMethodCode/* = DBDNWorldDef::PayMethodCode::Coin*/)
{
	TItem AddItem = { 0, };
	AddItem.nItemID = nItemID;
	AddItem.wCount = nItemCount;
	AddItem.cOption = cOption;

	return _CreateCashInvenItemEx(0, cPayMethodCode, nLogCode, biFKey, nPeriod, nPrice, AddItem);
}

int CDNUserItem::CreateCashInvenItemByCheat(int nItemID, short wCount, int nPeriod, int nLogCode)
{
	TItem AddItem;
	memset(&AddItem, 0, sizeof(TItem));
	AddItem.nItemID = nItemID;
	AddItem.wCount = g_pDataManager->GetItemOverlapCount(nItemID);
	AddItem.cSealCount = g_pDataManager->GetItemCashTradeCount(nItemID);	// cash는 봉인없기때문에 이 변수에 tradecount 사용

	for (int i = 0; i < wCount; i++){
		_CreateCashInvenItemEx(0, DBDNWorldDef::PayMethodCode::Coin, nLogCode, 0, nPeriod, 0, AddItem);
	}
	return ERROR_NONE;
}

int CDNUserItem::CreateCashInvenWholeItem(const TItem &Item)
{
	TItemData *pItemData = g_pDataManager->GetItemData(Item.nItemID);
	if (!pItemData) return ERROR_ITEM_NOTFOUND;
	if (!pItemData->IsCash) return ERROR_ITEM_FAIL;	// 캐쉬템이 아니면 넣을 수 없다

	int nRet = ERROR_NONE;
	switch(pItemData->nType)
	{
	case ITEMTYPE_VEHICLE:
	case ITEMTYPE_PET:
		{
			TVehicle AddVehicle;
			memset(&AddVehicle, 0, sizeof(AddVehicle));

			MakeVehicleItemStruct(Item, AddVehicle);

			if (_PushVehicleInventory(AddVehicle) == ERROR_NONE)
				m_pSession->SendRefreshVehicleInven(AddVehicle);				
		}
		break;

	default:
		{
			nRet = _PushCashInventory(Item);
			if (nRet == ERROR_NONE)
				m_pSession->SendRefreshCashInven(Item);
		}
		break;
	}

	return nRet;
}

#if defined(PRE_LEVELUPREWARD_DIRECT)
bool CDNUserItem::CreateCashInvenItemByMailID(int nMailID)
{
	TMailTableData *pMailData = g_pDataManager->GetMailTableData(nMailID);
	if (pMailData){
		for (int i = 0; i < MAILATTACHITEMMAX; i++){
			if (!pMailData->IsCash) continue;
			if (pMailData->ItemSNArr[i] <= 0) continue;

			TCashCommodityData CashData;
			TCashPackageData PackageData;

			memset(&PackageData, 0, sizeof(TCashPackageData));
			bool bPackage = g_pDataManager->GetCashPackageData(pMailData->ItemSNArr[i], PackageData);
			if (bPackage){
				for (int k = 0; k < (int)PackageData.nVecCommoditySN.size(); k++){
					memset(&CashData, 0, sizeof(TCashCommodityData));
					bool bRet = g_pDataManager->GetCashCommodityData(PackageData.nVecCommoditySN[k], CashData);
					if (!bRet) continue;

					TItem AddItem = { 0, };
					AddItem.nItemID = CashData.nItemID[0], AddItem;
					AddItem.wCount = g_pDataManager->GetCashCommodityCount(CashData.nSN);
					AddItem.cOption = -1;

					if (_CreateCashInvenItemEx(0, DBDNWorldDef::PayMethodCode::Coin, DBDNWorldDef::AddMaterializedItem::SystemMail, 0, 0, 0, AddItem) != ERROR_NONE)
						return false;
				}
			}
			else{
				memset(&CashData, 0, sizeof(TCashCommodityData));
				bool bRet = g_pDataManager->GetCashCommodityData(pMailData->ItemSNArr[i], CashData);
				if (!bRet) continue;

				TItem AddItem = { 0, };
				AddItem.nItemID = CashData.nItemID[0], AddItem;
				AddItem.wCount = g_pDataManager->GetCashCommodityCount(CashData.nSN);
				AddItem.cOption = -1;

				if (_CreateCashInvenItemEx(0, DBDNWorldDef::PayMethodCode::Coin, DBDNWorldDef::AddMaterializedItem::SystemMail, 0, 0, 0, AddItem) != ERROR_NONE)
					return false;
			}
		}
	}

	return true;
}
#endif	// #if defined(PRE_LEVELUPREWARD_DIRECT)

bool CDNUserItem::CheckRangeInventoryIndex(int nIndex) const
{
	return _CheckRangeInventoryIndex(nIndex);
}

bool CDNUserItem::AddInventoryByQuest(int nItemID, short wCount, int nQuestID, int nRandomSeed)
{
	INT64 nSerial = 0;
	if (CreateInvenItem1(nItemID, wCount, -1, nRandomSeed, DBDNWorldDef::AddMaterializedItem::QuestReward, nQuestID, CREATEINVEN_QUEST) != ERROR_NONE) return false;

	char cGrade = 0;
	TItemData *pItemData = g_pDataManager->GetItemData(nItemID);
	if (pItemData)
		cGrade = pItemData->cRank;

	m_pSession->GetQuest()->OnAddItem(nItemID, wCount);

	return true;
}

bool CDNUserItem::AddInventoryByQuest( const TQuestReward::_ITEMSET* pItemSet, int nQuestID, int nRandomSeed )
{
	TItem AddItem = { 0, };
	if ( MakeItemStruct(pItemSet->nItemID, AddItem, 0, pItemSet->nItemOptionTableID) == false )
		return false;

	AddItem.wCount = pItemSet->nItemCount;
	if ( nRandomSeed != -1 ) 
		AddItem.nRandomSeed = nRandomSeed;
	
	if ( CreateInvenWholeItem(AddItem, DBDNWorldDef::AddMaterializedItem::QuestReward, nQuestID, CREATEINVEN_QUEST) != ERROR_NONE )
		return false;

	m_pSession->GetQuest()->OnAddItem(pItemSet->nItemID, pItemSet->nItemCount);
	return true;
}

bool CDNUserItem::DeleteInventoryByQuest(int nItemID, short wCount, int nQuestID)
{
	if ((nItemID <= 0) ||(wCount <= 0)) return false;

	int nCount = GetInventoryItemCount(nItemID);
	if (nCount < wCount) return false;	// 아이템 공간부족

	TItemData *pItemData = g_pDataManager->GetItemData(nItemID);
	if (!pItemData) return false;
	if (pItemData->nOverlapCount <= 0) return false;

	int nIndex = 0;
	INT64 nSerial = 0;
	USHORT wDur = 0;
	short wTotalCount = 0;
	if (pItemData->nOverlapCount == 1){	// 셀수없는 아이템
		for (int i = 0; i < wCount; i++){
			nIndex = FindInventorySlot(nItemID, 1);
			if( nIndex < 0 || nIndex >= INVENTORYMAX )
				return false;
			nSerial = m_Inventory[nIndex].nSerial;
			wDur = m_Inventory[nIndex].wDur;
			wTotalCount = m_Inventory[nIndex].wCount;
			_PopInventorySlotItem(nIndex, 1);

			if (m_pSession->GetDBConnection()){
				bool bAllFlag = (wTotalCount == 1) ? true : false;
				m_pSession->GetDBConnection()->QueryUseItem(m_pSession, DBDNWorldDef::UseItem::Use, nSerial, 1, bAllFlag);
			}

			m_pSession->SendRefreshInven(nIndex, &m_Inventory[nIndex], false);
			return true;
		}
	}
	else {	// 셀수있는 아이템
		int nTotal = wCount;
#if defined(PRE_PERIOD_INVENTORY)
		for (int i = 0; i < INVENTORYTOTALMAX; i++){
			if (i == GetInventoryCount()){
				if (IsEnablePeriodInventory())
					i = INVENTORYMAX;
				else
					break;
			}
#else	// #if defined(PRE_PERIOD_INVENTORY)
		for (int i = 0; i < GetInventoryCount(); i++){
#endif	// #if defined(PRE_PERIOD_INVENTORY)
			if (m_Inventory[i].nItemID == nItemID){
				if (nTotal > 0){
					if (m_Inventory[i].wCount >= nTotal){	// 지울것보다 많거나 같다
						nCount = nTotal;
					}
					else {	// 지울것보다 적을때
						nCount = m_Inventory[i].wCount;
					}

					wTotalCount = m_Inventory[i].wCount;
					nSerial = m_Inventory[i].nSerial;
					_PopInventorySlotItem(i, nCount);

					if (m_pSession->GetDBConnection()){
						bool bAllFlag = (wTotalCount == nCount) ? true : false;
						m_pSession->GetDBConnection()->QueryUseItem(m_pSession, DBDNWorldDef::UseItem::Use, nSerial, nCount, bAllFlag);
					}

					m_pSession->SendRefreshInven(i, &m_Inventory[i], false);

					nTotal -= nCount;
					if (nTotal <= 0) 
						return true;
				}
			}
		}
	}

	return false;
}

bool CDNUserItem::DeleteInventoryByItemID(int nItemID, int iCount, int nLogCode, INT64 biFKey/*=0*/ )
{
	std::vector<TItemInfo> VecItemList;
	VecItemList.clear();

	return DeleteInventoryExByItemID(nItemID, -1, iCount, nLogCode, biFKey, VecItemList, false);
}

bool CDNUserItem::DeleteInventoryExByItemID(int nItemID, char cOption, int iDeleteCount, int nLogCode, INT64 biFKey, std::vector<TItemInfo> &VecItemList, bool bInsertList/* = false*/)				// ItemID가지고 inven에 빼기(db 저장 포함)
{
	if ((nItemID <= 0) ||(iDeleteCount <= 0)) return false;

	int nCount = GetInventoryItemCount(nItemID);
	if (nCount < iDeleteCount) return false;	// 아이템 공간부족

	TItemData *pItemData = g_pDataManager->GetItemData(nItemID);
	if (!pItemData) return false;
	if (pItemData->nOverlapCount <= 0) return false;

	if (pItemData->nOverlapCount == 1){	// 셀수없는 아이템
#if defined(PRE_MOD_60583)
		//삭제 후보 아이템의 보유량이 삭제량 보다 많을 경우 봉인이 안된 아이템 먼저 삭제(#60583) - 2012-06-27 by stupidfox 
		bool bFindSoulBindItemFirst = false;
		if(nCount > iDeleteCount)
			bFindSoulBindItemFirst = true;
#endif
		int nIndex = 0;
		for (int i = 0; i < iDeleteCount; i++){
#if defined(PRE_MOD_60583)
			nIndex = FindInventorySlot(nItemID, cOption, 1, bFindSoulBindItemFirst);
			//봉인 안된 아이템 다 떨어졌으면 봉인된 아이템 삭제
			if(nIndex < 0 && bFindSoulBindItemFirst == true)
			{
				bFindSoulBindItemFirst = false;
				nIndex = FindInventorySlot(nItemID, cOption, 1, false);
			}
#else
			nIndex = FindInventorySlot(nItemID, cOption, 1, false);
#endif
			if( nIndex < 0 || nIndex >= INVENTORYMAX)
				return false;
			TItem Item = m_Inventory[nIndex];
			Item.wCount = 1;

			if (bInsertList){
				TItemInfo ItemInfo = {0, };
				ItemInfo.cSlotIndex = nIndex;
				ItemInfo.Item = Item;
				VecItemList.push_back(ItemInfo);
			}

			_PopInventorySlotItem(nIndex, 1);

			switch(nLogCode)
			{
			case DBDNWorldDef::UseItem::Use:
			case DBDNWorldDef::UseItem::DeCompose:
			case DBDNWorldDef::UseItem::Destroy:
#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
			case DBDNWorldDef::UseItem::Present:
#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
				{
					m_pSession->GetDBConnection()->QueryUseItemEx(m_pSession->GetDBThreadID(), m_pSession->GetWorldSetID(), m_pSession->GetAccountDBID(), nLogCode, Item.nSerial, Item.wCount, 
						biFKey ? static_cast<int>(biFKey) : m_pSession->GetChannelID(), m_pSession->GetMapIndex(), m_pSession->GetIpW(), true );
				}
			}

			m_pSession->SendRefreshInven(nIndex, &m_Inventory[nIndex], false);
		}
		return true;
	}
	else {	// 셀수있는 아이템
		int nTotal = iDeleteCount;
#if defined(PRE_PERIOD_INVENTORY)
		for (int i = 0; i < INVENTORYTOTALMAX; i++){
			if (i == GetInventoryCount()){
				if (IsEnablePeriodInventory())
					i = INVENTORYMAX;
				else
					break;
			}
#else	// #if defined(PRE_PERIOD_INVENTORY)
		for (int i = 0; i < GetInventoryCount(); i++){
#endif	// #if defined(PRE_PERIOD_INVENTORY)
			if (m_Inventory[i].nItemID == nItemID){
				if (nTotal > 0){
					if (m_Inventory[i].wCount >= nTotal){	// 지울것보다 많거나 같다
						nCount = nTotal;
					}
					else {	// 지울것보다 적을때
						nCount = m_Inventory[i].wCount;
					}

					TItem Item = m_Inventory[i];
					if ( cOption != -1 && cOption != Item.cOption ) continue;
					Item.wCount = nCount;

					if (bInsertList){
						TItemInfo ItemInfo = {0, };
						ItemInfo.cSlotIndex = i;
						ItemInfo.Item = Item;
						VecItemList.push_back(ItemInfo);
					}

					int nAllItemCount = m_Inventory[i].wCount;
					_PopInventorySlotItem(i, nCount);

					switch(nLogCode)
					{
					case DBDNWorldDef::UseItem::Use:
					case DBDNWorldDef::UseItem::DeCompose:
					case DBDNWorldDef::UseItem::Destroy:
#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
					case DBDNWorldDef::UseItem::Present:
#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
						{
							bool bAllFlag = (nAllItemCount == Item.wCount) ? true : false;
							m_pSession->GetDBConnection()->QueryUseItemEx(m_pSession->GetDBThreadID(), m_pSession->GetWorldSetID(), m_pSession->GetAccountDBID(), nLogCode, Item.nSerial, Item.wCount,
								m_pSession->GetChannelID(), m_pSession->GetMapIndex(), m_pSession->GetIpW(), bAllFlag );
						}
					}
					m_pSession->SendRefreshInven(i, &m_Inventory[i], false);

					nTotal -= nCount;
					if (nTotal <= 0) return true;
				}
			}
		}
	}

	return false;
}

bool CDNUserItem::DeleteInventoryBySlot(int nSlotIndex, short wCount, INT64 biSerial, int nLogCode)
{
	if (m_Inventory[nSlotIndex].nSerial != biSerial) return false;

	TItem Item = m_Inventory[nSlotIndex];
	Item.wCount = wCount;

	INT64 nSerial = Item.nSerial;
	int nAllItemCount = m_Inventory[nSlotIndex].wCount;

	if (_PopInventorySlotItem(nSlotIndex, wCount) != ERROR_NONE) return false;

	switch(nLogCode)
	{
		case DBDNWorldDef::UseItem::Use:
		case DBDNWorldDef::UseItem::DeCompose:
		case DBDNWorldDef::UseItem::Destroy:
		{
			bool bAllFlag = (nAllItemCount == Item.wCount) ? true : false;
			m_pSession->GetDBConnection()->QueryUseItem(m_pSession, nLogCode, nSerial, wCount, bAllFlag );
		}
	}

	// 클라이언트에게 변경 사항을 알려준다.
	m_pSession->SendRefreshInven(nSlotIndex, &m_Inventory[nSlotIndex], false);
	return true;
}

bool CDNUserItem::DeleteItemByUse(int nInvenType, int nInvenIndex, INT64 biInvenSerial, bool bDBSave/*=true*/ )
{
	switch(nInvenType)
	{
	case ITEMPOSITION_INVEN:
		{
			if (!_CheckRangeInventoryIndex(nInvenIndex)) return false;	// index 잘못옴
			if (m_Inventory[nInvenIndex].nItemID <= 0) return false;	// 아이템 없음
			if (m_Inventory[nInvenIndex].nSerial != biInvenSerial) return false;

			TItem InvenItem = m_Inventory[nInvenIndex];
			TItemData *pItemData = g_pDataManager->GetItemData(InvenItem.nItemID);
			if (!pItemData) return false;

			if (InvenItem.wCount <= 0) return false;	// 0이면 못쓴다
			if (pItemData->cLevelLimit > m_pSession->GetLevel()) return false;	// 레벨이 낮으면 나가라
			if (pItemData->nOverlapCount <= 0) return false;
			// if (pItemData->nOverlapCount == 1) return false;	// 겹치는 아이템이 아니다

			_UpdateInventoryCoolTime(nInvenIndex);
#if defined( _FINAL_BUILD )
			if (m_Inventory[nInvenIndex].nCoolTime > 0) return false;	// 쿨타임이 남아있음	
#else
#if defined( _GAMESERVER )
			bool bSkip = false;
			DnActorHandle hActor = m_pSession->GetActorHandle();
			if (hActor && hActor->IsIgnoreSkillCoolTime() )
				bSkip = true;
			if ( bSkip == false )
				if (m_Inventory[nInvenIndex].nCoolTime > 0) return false;	// 쿨타임이 남아있음	
#else
			if (m_Inventory[nInvenIndex].nCoolTime > 0) return false;	// 쿨타임이 남아있음	
#endif // #if defined( _GAMESERVER )
#endif // #if defined( _FINAL_BUILD )

			_SetItemCoolTime(pItemData->nSkillID, pItemData->nMaxCoolTime);	// cooltime 세팅
			m_pSession->SendUseItem(nInvenType, nInvenIndex, biInvenSerial);

			int nLogCode = bDBSave ? DBDNWorldDef::UseItem::Use : 0;
			if (!DeleteInventoryBySlot(nInvenIndex, 1, biInvenSerial, nLogCode )) return false;	// 지워주고
		}
		break;

	case ITEMPOSITION_CASHINVEN:
		{
			const TItem *pInvenItem = GetCashInventory(biInvenSerial);
			if (!pInvenItem) return false;
			TItemData *pItemData = g_pDataManager->GetItemData(pInvenItem->nItemID);
			if ( !pItemData ) return false;

			if (pInvenItem->wCount <= 0) return false;	// 0이면 못쓴다
			if (!pItemData->IsCash) return false;

			_UpdateCashInventoryCoolTime(biInvenSerial);
			if (pInvenItem->nCoolTime > 0) return false;	// 쿨타임이 남아있음	

			_SetItemCoolTime(pItemData->nSkillID, pItemData->nMaxCoolTime);	// cooltime 세팅
			m_pSession->SendUseItem(nInvenType, nInvenIndex, biInvenSerial);

			if (!DeleteCashInventoryBySerial(biInvenSerial, 1, bDBSave )) return false;	// 지워주고
		}
		break;
	}

	return true;
}

bool CDNUserItem::DeleteInventoryByType(int nType, short wCount, int nLogCode, int nTypeParam1/*=-1*/, int iTargetLogItemID/*=0*/, char cTargetLogItemLevel/*=0*/ )		// ItemType가지고 inven에 빼기(db 저장 포함)
{
	if ((nType <= 0) ||(wCount <= 0)) return false;

	int nItemCount = GetInventoryItemCountByType(nType, nTypeParam1);
	if (nItemCount < wCount) return false;	// 아이템 부족

	int nTotal = wCount;
	int nCount = 0;
#if defined(PRE_PERIOD_INVENTORY)
	for (int i = 0; i < INVENTORYTOTALMAX; i++){
		if (i == GetInventoryCount()){
			if (IsEnablePeriodInventory())
				i = INVENTORYMAX;
			else
				break;
		}
#else	// #if defined(PRE_PERIOD_INVENTORY)
	for (int i = 0; i < GetInventoryCount(); i++){
#endif	// #if defined(PRE_PERIOD_INVENTORY)
		if (m_Inventory[i].nItemID <= 0) continue;
		if (g_pDataManager->GetItemMainType(m_Inventory[i].nItemID) != nType) continue;
		if (nTypeParam1 != -1 && g_pDataManager->GetItemTypeParam1(m_Inventory[i].nItemID) != nTypeParam1) continue;
		if (nTotal > 0){
			if (m_Inventory[i].wCount >= nTotal){	// 지울것보다 많거나 같다
				nCount = nTotal;
			}
			else {	// 지울것보다 적을때
				nCount = m_Inventory[i].wCount;
			}

			TItem Item = m_Inventory[i];
			int nAllItemCount = Item.wCount;
			Item.wCount = nCount;

			if (_PopInventorySlotItem(i, nCount) != ERROR_NONE) return false;

			switch(nLogCode)
			{
			case DBDNWorldDef::UseItem::Use:
			case DBDNWorldDef::UseItem::DeCompose:
			case DBDNWorldDef::UseItem::Destroy:
#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
			case DBDNWorldDef::UseItem::Present:
#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
				{
					bool bAllFlag = (nAllItemCount == Item.wCount) ? true : false;
					m_pSession->GetDBConnection()->QueryUseItem(m_pSession, nLogCode, Item.nSerial, Item.wCount, bAllFlag, iTargetLogItemID, cTargetLogItemLevel );
				}
			}
			m_pSession->SendRefreshInven(i, &m_Inventory[i], false);

			nTotal -= nCount;
			if (nTotal <= 0) return true;
		}
	}

	return false;
}

bool CDNUserItem::DeleteCashInventoryByItemID(int nItemID, short wCount, int nLogCode, INT64 biFKey/*=0*/ )
{
	if ((nItemID <= 0) ||(wCount <= 0)) return false;
	if (m_MapCashInventory.empty()) return false;

	int nCount = GetCashItemCountByItemID(nItemID);
	if (nCount < wCount) return false;	// 지울 아이템이 모자름

	TItemData *pItemData = g_pDataManager->GetItemData(nItemID);
	if (!pItemData) return false;
	if (pItemData->nOverlapCount <= 0) return false;

	int nTotalCount = wCount, nDeleteCount = 0;
	TItem TempItem = {0,};
	std::vector<TItem> VecItemList;
	VecItemList.clear();

	TMapItem::iterator iter;
	for(iter = m_MapCashInventory.begin(); iter != m_MapCashInventory.end(); ){
		if (iter->second.nItemID != nItemID){
			++iter;
			continue;
		}
		if (nTotalCount > 0){
			if (iter->second.wCount >= nTotalCount){
				nDeleteCount = nTotalCount;
			}
			else{
				nDeleteCount = iter->second.wCount;
			}

			iter->second.wCount -= nDeleteCount;
			TempItem = iter->second;

			bool bAllFlag = false;
			if (iter->second.wCount <= 0){
				m_MapCashInventory.erase(iter++);
				TempItem.wCount = 0;
				bAllFlag = true;
			}
			else{
				_UpdateCashInventoryCoolTime(TempItem.nSerial);
				++iter;
			}

			VecItemList.push_back(TempItem);

			switch(nLogCode)
			{
			case DBDNWorldDef::UseItem::Use:
			case DBDNWorldDef::UseItem::DeCompose:
			case DBDNWorldDef::UseItem::Destroy:
#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
			case DBDNWorldDef::UseItem::Present:
#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
				{
					m_pSession->GetDBConnection()->QueryUseItemEx(m_pSession->GetDBThreadID(), m_pSession->GetWorldSetID(), m_pSession->GetAccountDBID(), nLogCode, TempItem.nSerial, nDeleteCount,
						biFKey ? static_cast<int>(biFKey) : m_pSession->GetChannelID(), m_pSession->GetMapIndex(), m_pSession->GetIpW(), bAllFlag );
				}
			}

			nTotalCount -= nDeleteCount;
			if (nTotalCount <= 0){
				m_pSession->SendRefreshCashInven(VecItemList, false);
				return true;
			}
		}
	}

	return false;
}

bool CDNUserItem::UseCashItemByType(int nType, int nUseCount, bool bSend)
{
	if (m_MapCashInventory.empty()) return false;

	int nTotal = nUseCount, nGapCount = 0;
	std::vector<TItem> VecItemList;
	VecItemList.clear();
	TItem TempItem = { 0, };

	TMapItem::iterator iter;
	for(iter = m_MapCashInventory.begin(); iter != m_MapCashInventory.end(); ){
		if (g_pDataManager->GetItemMainType(iter->second.nItemID) != nType){
			++iter;
			continue;
		}

		if (nTotal > 0){
			if (iter->second.wCount >= nTotal){	// 지울것보다 많거나 같다
				nGapCount = nTotal;
			}
			else {	// 지울것보다 적을때
				nGapCount = iter->second.wCount;
			}

			iter->second.wCount -= nGapCount;
			TempItem = iter->second;

			bool bAllFlag = false;
			if (iter->second.wCount <= 0){
				m_MapCashInventory.erase(iter++);
				TempItem.wCount = 0;
				bAllFlag = true;
			}
			else{
				_UpdateCashInventoryCoolTime(TempItem.nSerial);
				++iter;
			}

			VecItemList.push_back(TempItem);

			m_pSession->GetDBConnection()->QueryUseItem(m_pSession, DBDNWorldDef::UseItem::Use, TempItem.nSerial, nGapCount, bAllFlag);

			nTotal -= nGapCount;
			if (nTotal <= 0){
				if (bSend) m_pSession->SendRefreshCashInven(VecItemList, false);
				return true;
			}
		}
	}

	return false;
}

bool CDNUserItem::DeleteCashInventoryBySerial(INT64 biInvenSerial, short wCount, bool bSaveDB/* = true*/)
{
	const TItem *pItem = GetCashInventory(biInvenSerial);
	if (!pItem) return false;

	TItem Item = *pItem;
	if (_PopCashInventoryBySerial(biInvenSerial, wCount) != ERROR_NONE) return false;

	if (bSaveDB){
		bool bAllFlag = (Item.wCount == wCount) ? true : false;
		m_pSession->GetDBConnection()->QueryUseItem(m_pSession, DBDNWorldDef::UseItem::Use, biInvenSerial, wCount, bAllFlag);
	}

	// 클라이언트에게 변경 사항을 알려준다.
	const TItem *pTempInven = GetCashInventory(biInvenSerial);
	if (pTempInven)
		Item = *pTempInven;
	else
		Item.wCount = 0;

	m_pSession->SendRefreshCashInven(Item, false);

	return true;
}

static bool CompareCashItem( TItem s1, TItem s2 )
{
	if (s1.bEternity == false && s2.bEternity == true) return true;
	else if (s1.bEternity == true && s2.bEternity == false) return false;
	else if (s1.bEternity == false && s2.bEternity == false){
		if (s1.tExpireDate < s2.tExpireDate) return true;
		else if (s1.tExpireDate > s2.tExpireDate) return false;
	}
	if (s1.cSealCount < s2.cSealCount) return true;
	else if (s1.cSealCount > s2.cSealCount) return false;

	return false;
}

// inven, cash inven 공용 함수.
bool CDNUserItem::UseItemByItemID(int iItemID, int nUseCount, bool bSend, int nTypeParam1 /*= -1*/)
{
	// 아이템 삭제순서는 기간제캐시-일반-무제한캐시(거래불가)-무제한캐시(거래가능)다.
	std::vector<TItem> VecEternityCashItemList;
	std::vector<TItem> VecNoEternityCashItemList;

	TMapItem::iterator iter;
	for(iter = m_MapCashInventory.begin(); iter != m_MapCashInventory.end(); ++iter){
		if ( iter->second.nItemID != iItemID ){
			continue;
		}
		if (nTypeParam1 != -1 && g_pDataManager->GetItemTypeParam1(iter->second.nItemID) != nTypeParam1){
			continue;
		}
		if (IsExpired(iter->second)) // 수량을 셀때도 제외했으니 깍을때도 제외.
			continue;

		if (iter->second.bEternity)
			VecEternityCashItemList.push_back(iter->second);
		else
			VecNoEternityCashItemList.push_back(iter->second);
	}

	// 정렬
	std::sort(VecNoEternityCashItemList.begin(), VecNoEternityCashItemList.end(), CompareCashItem);

	int nTotal = nUseCount, nGapCount = 0;
	std::vector<TItem> VecSendCashItemList;

	std::vector<TItem>::iterator iterItem;
	for(iterItem = VecNoEternityCashItemList.begin(); iterItem != VecNoEternityCashItemList.end(); ++iterItem){
		if (nTotal > 0){
			if (iterItem->wCount >= nTotal){	// 지울것보다 많거나 같다
				nGapCount = nTotal;
			}
			else {	// 지울것보다 적을때
				nGapCount = iterItem->wCount;
			}

			if (_PopCashInventoryBySerial(iterItem->nSerial, nGapCount) != ERROR_NONE) return false;

			iterItem->wCount -= nGapCount;
			bool bAllFlag = false;
			if (iterItem->wCount <= 0){
				iterItem->wCount = 0;
				bAllFlag = true;
			}
			VecSendCashItemList.push_back(*iterItem);

			m_pSession->GetDBConnection()->QueryUseItem(m_pSession, DBDNWorldDef::UseItem::Use, iterItem->nSerial, nGapCount, bAllFlag );

			nTotal -= nGapCount;
			if (nTotal <= 0){
				break;
			}
		}
	}
	if (bSend && !VecSendCashItemList.empty()) m_pSession->SendRefreshCashInven(VecSendCashItemList, false);

	// 일반 인벤토리
	if (nTotal > 0){
		int nInventoryItemCount = GetInventoryItemCount(iItemID, nTypeParam1);
		if (nInventoryItemCount){
			int nDeleteCount = min(nTotal, nInventoryItemCount);
			if (DeleteInventoryByItemID(iItemID, nDeleteCount, DBDNWorldDef::UseItem::Use) == false) return false;
			nTotal -= nDeleteCount;
		}
	}

	// 무기한 캐시아이템
	if (nTotal > 0){
		VecSendCashItemList.clear();
		std::sort(VecEternityCashItemList.begin(), VecEternityCashItemList.end(), CompareCashItem);
		for(iterItem = VecEternityCashItemList.begin(); iterItem != VecEternityCashItemList.end(); ++iterItem){
			if (nTotal > 0){
				if (iterItem->wCount >= nTotal){	// 지울것보다 많거나 같다
					nGapCount = nTotal;
				}
				else {	// 지울것보다 적을때
					nGapCount = iterItem->wCount;
				}

				if (_PopCashInventoryBySerial(iterItem->nSerial, nGapCount) != ERROR_NONE) return false;

				iterItem->wCount -= nGapCount;
				bool bAllFlag = false;
				if (iterItem->wCount <= 0){
					iterItem->wCount = 0;
					bAllFlag = true;
				}
				VecSendCashItemList.push_back(*iterItem);

				m_pSession->GetDBConnection()->QueryUseItem(m_pSession, DBDNWorldDef::UseItem::Use, iterItem->nSerial, nGapCount, bAllFlag );

				nTotal -= nGapCount;
				if (nTotal <= 0){
					break;
				}
			}
		}
		if (bSend && !VecSendCashItemList.empty()) m_pSession->SendRefreshCashInven(VecSendCashItemList, false);
	}

	if( nTotal > 0 )
		return false;

	return true;
}

bool CDNUserItem::UseItemByType(int nType, int nUseCount, bool bSend, int nTypeParam1, int iTargetLogItemID/*=0*/, char cTargetLogItemLevel/*=0*/ )
{
	// 아이템 삭제순서는 기간제캐시-일반-무제한캐시(거래불가)-무제한캐시(거래가능)다.
	std::vector<TItem> VecEternityCashItemList;
	std::vector<TItem> VecNoEternityCashItemList;

	TMapItem::iterator iter;
	for(iter = m_MapCashInventory.begin(); iter != m_MapCashInventory.end(); ++iter){
		if (g_pDataManager->GetItemMainType(iter->second.nItemID) != nType){
			continue;
		}
		if (nTypeParam1 != -1 && g_pDataManager->GetItemTypeParam1(iter->second.nItemID) != nTypeParam1){
			continue;
		}
		if (IsExpired(iter->second)) // 수량을 셀때도 제외했으니 깍을때도 제외.
			continue;

		if( iter->second.bExpireComplete ) // 캐시삭제대기 아이템은 제외.
			continue;

		if (iter->second.bEternity)
			VecEternityCashItemList.push_back(iter->second);
		else
			VecNoEternityCashItemList.push_back(iter->second);
	}

	// 정렬
	std::sort(VecNoEternityCashItemList.begin(), VecNoEternityCashItemList.end(), CompareCashItem);

	int nTotal = nUseCount, nGapCount = 0;
	std::vector<TItem> VecSendCashItemList;

	std::vector<TItem>::iterator iterItem;
	for(iterItem = VecNoEternityCashItemList.begin(); iterItem != VecNoEternityCashItemList.end(); ++iterItem){
		if (nTotal > 0){
			if (iterItem->wCount >= nTotal){	// 지울것보다 많거나 같다
				nGapCount = nTotal;
			}
			else {	// 지울것보다 적을때
				nGapCount = iterItem->wCount;
			}

			if (_PopCashInventoryBySerial(iterItem->nSerial, nGapCount) != ERROR_NONE) return false;

			iterItem->wCount -= nGapCount;
			bool bAllFlag = false;
			if (iterItem->wCount <= 0){
				iterItem->wCount = 0;
				bAllFlag = true;
			}
			VecSendCashItemList.push_back(*iterItem);

			m_pSession->GetDBConnection()->QueryUseItem(m_pSession, DBDNWorldDef::UseItem::Use, iterItem->nSerial, nGapCount, bAllFlag, iTargetLogItemID, cTargetLogItemLevel );

			nTotal -= nGapCount;
			if (nTotal <= 0){
				break;
			}
		}
	}
	if (bSend && !VecSendCashItemList.empty()) m_pSession->SendRefreshCashInven(VecSendCashItemList, false);

	// 일반 인벤토리
	if (nTotal > 0){
		int nInventoryItemCount = GetInventoryItemCountByType(nType, nTypeParam1);
		if (nInventoryItemCount){
			int nDeleteCount = min(nTotal, nInventoryItemCount);
#if defined(PRE_FIX_75305)
			if (DeleteInventoryByType(nType, nDeleteCount, DBDNWorldDef::UseItem::Use, nTypeParam1, iTargetLogItemID, cTargetLogItemLevel ) == false) 
#else	// #if defined(PRE_FIX_75305)
			if (DeleteInventoryByType(nType, nDeleteCount, DBDNWorldDef::UseItem::Use, -1, iTargetLogItemID, cTargetLogItemLevel ) == false) 
#endif	// #if defined(PRE_FIX_75305)
				return false;
			nTotal -= nDeleteCount;
		}
	}

	// 무기한 캐시아이템
	if (nTotal > 0){
		VecSendCashItemList.clear();
		std::sort(VecEternityCashItemList.begin(), VecEternityCashItemList.end(), CompareCashItem);
		for(iterItem = VecEternityCashItemList.begin(); iterItem != VecEternityCashItemList.end(); ++iterItem){
			if (nTotal > 0){
				if (iterItem->wCount >= nTotal){	// 지울것보다 많거나 같다
					nGapCount = nTotal;
				}
				else {	// 지울것보다 적을때
					nGapCount = iterItem->wCount;
				}

				if (_PopCashInventoryBySerial(iterItem->nSerial, nGapCount) != ERROR_NONE) return false;

				iterItem->wCount -= nGapCount;
				bool bAllFlag = false;
				if (iterItem->wCount <= 0){
					iterItem->wCount = 0;
					bAllFlag = true;
				}
				VecSendCashItemList.push_back(*iterItem);
				m_pSession->GetDBConnection()->QueryUseItem(m_pSession, DBDNWorldDef::UseItem::Use, iterItem->nSerial, nGapCount, bAllFlag, iTargetLogItemID, cTargetLogItemLevel );
				nTotal -= nGapCount;
				if (nTotal <= 0){
					break;
				}
			}
		}
		if (bSend && !VecSendCashItemList.empty()) 
			m_pSession->SendRefreshCashInven(VecSendCashItemList, false);
	}

	if( nTotal > 0 )
		return false;

	return true;
}

int CDNUserItem::ProcessBuyCombinedShop( int iShopID, int iTabID, int iListID, short nCount )
{
	const TCombinedShopTableData* pShopTableData = g_pDataManager->GetCombinedShopItemTableData( iShopID, iTabID, iListID );
	if (!pShopTableData) 
		return ERROR_SHOP_DATANOTFOUND;

	const TShopItem* pShopItem = &pShopTableData->ShopItem;

	int iNeedTotalGold = pShopItem->nPrice * nCount;
	int iBuyTotalItemCount = pShopItem->nCount * nCount;

	if( nCount <= 0 || iBuyTotalItemCount > pShopItem->nMaxCount ) 
		return ERROR_ITEM_OVERFLOW;
	if( pShopItem->nItemID <= 0 ) 
		return ERROR_ITEM_NOTFOUND;
	if( pShopItem->nCount <= 0 ) 
		return ERROR_GENERIC_INVALIDREQUEST;

	TItemData *pItemData = g_pDataManager->GetItemData( pShopItem->nItemID );
	if( !pItemData ) 
		return ERROR_SHOP_DATANOTFOUND;
	if( pItemData->nOverlapCount <= 0 ) 
		return ERROR_SHOP_DATANOTFOUND;
	if( pItemData->nType == ITEMTYPE_QUEST ) 
		return ERROR_SHOP_NOTFORSALE;

#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
	// 만약 호감도 상점 혜택 보상 테이블에 등록되어있고, 
	// 혜택종류가 물건살 때 할인이라면 적용시켜 준다. 
	// 샵 아이템의 오버랩 카운트에 따라 아래 루틴에서 분기가 일어나지만 하나의 갯수에 
	// 할인율을 적용한 후 곱해도 상관 없기 때문에 여기서 할인 가격 적용.
	CNpcReputationProcessor::CheckAndCalcUnionBenefit( m_pSession, TStoreBenefitData::BuyingPriceDiscount, iNeedTotalGold );
#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )

	//################################################################################################
	// Check PurchaseType 
	//################################################################################################

	int iNeedTotalItemCount = 0;
	int iNeedTotalLadderPoint = 0;
	int iNeedTotalUnionPoint = 0;
	int iNeedTotalGuildPoint = 0;
#if defined( PRE_ADD_NEW_MONEY_SEED )
	int nNeedTotalSeedPoint = 0;
#endif

#if defined (PRE_ADD_DONATION) || defined(PRE_ADD_COMBINEDSHOP_PERIOD)
	int nPeriod = 0;
#endif // #if defined (PRE_ADD_DONATION)

#if defined( PRE_ADD_LIMITED_SHOP )
	bool bGuildLimitedItem = false;
	if (pShopItem->buyLimitCount > 0)
	{		
#if defined( PRE_FIX_74404 )
		if( pShopItem->buyLimitCount < m_pSession->GetLimitedShopBuyedItem(iShopID, pShopItem->nItemID) + nCount )
		{
			return ERROR_ITEM_BUY_OVERFLOW_COUNT;
		}
#else // #if defined( PRE_FIX_74404 )
		if( pShopItem->buyLimitCount < m_pSession->GetLimitedShopBuyedItem(pShopItem->nItemID) + nCount )
		{
			return ERROR_ITEM_BUY_OVERFLOW_COUNT;
		}
#endif // #if defined( PRE_FIX_74404 )
	}
#endif // #if defined( PRE_ADD_LIMITED_SHOP )

	for( int i=0 ; i<Shop::Max::PurchaseType ; ++i )
	{
		switch( pShopTableData->PurchaseType[i].PurchaseType )
		{
			case Shop::PurchaseType::Gold:
			{
				if( m_pSession->CheckEnoughCoin( iNeedTotalGold ) == false )
					return ERROR_ITEM_INSUFFICIENCY_MONEY;
				break;
			}
			case Shop::PurchaseType::ItemID:
			{
				iNeedTotalItemCount = pShopTableData->PurchaseType[i].iPurchaseItemValue*nCount;
				if( CheckEnoughItem( pShopTableData->PurchaseType[i].iPurchaseItemID, iNeedTotalItemCount ) == false ) 
					return ERROR_ITEM_INSUFFICIENCY_ITEM;

#if defined (PRE_ADD_DONATION)
				TItemData *pNeedItemData = g_pDataManager->GetItemData(pShopTableData->PurchaseType[i].iPurchaseItemID);
				if (!pNeedItemData)
				{
					_DANGER_POINT();
					return ERROR_ITEM_NOTFOUND;
				}
#if defined(PRE_ADD_COMBINEDSHOP_PERIOD)	//캐쉬템 기간은 아래에서 일괄처리
#else
				if (pNeedItemData->nType == ITEMTYPE_DONATION_COUPON)
					nPeriod = pItemData->nTypeParam[1];
#endif

#endif // #if defined (PRE_ADD_DONATION)
				break;
			}
			case Shop::PurchaseType::LadderPoint:
			{
				if( static_cast<int>(CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::PvPLadder_RegularSeason )) == 0 )
					return ERROR_LADDERSYSTEM_EXHIBITION_DURATION;

				iNeedTotalLadderPoint = pShopTableData->PurchaseType[i].iPurchaseItemValue*nCount;
#if defined( PRE_ADD_TOTAL_LEVEL_SKILL )
				float fTotalLevel = m_pSession->GetTotalLevelSkillEffect(TotalLevelSkill::Common::LadderPointSale);
				iNeedTotalLadderPoint = (int)(iNeedTotalLadderPoint - iNeedTotalLadderPoint * fTotalLevel);
#endif
				if( m_pSession->GetPvPLadderScoreInfoPtr()->iPvPLadderPoint < iNeedTotalLadderPoint ) 
					return ERROR_ITEM_INSUFFICIENCY_LADDERPOINT;
				break;
			}
			case Shop::PurchaseType::UnionPoint:
			{
				iNeedTotalUnionPoint = pShopTableData->PurchaseType[i].iPurchaseItemValue*nCount;
#if defined( PRE_UNIONSHOP_RENEWAL )
				if( m_pSession->GetUnionReputePointInfoPtr()->GetUnionReputePoint(pShopTableData->PurchaseType[i].iPurchaseItemID) < iNeedTotalUnionPoint ) 
					return ERROR_ITEM_INSUFFICIENCY_UNIONPOINT;
#else
				if( m_pSession->GetUnionReputePointInfoPtr()->GetUnionReputePoint(pItemData->nTypeParam[2]) < iNeedTotalUnionPoint ) 
					return ERROR_ITEM_INSUFFICIENCY_UNIONPOINT;
#endif // #if defined( PRE_UNIONSHOP_RENEWAL )
				break;
			}
			case Shop::PurchaseType::GuildPoint:
			{
				iNeedTotalGuildPoint = pShopTableData->PurchaseType[i].iPurchaseItemValue*nCount;
				if( m_pSession->GetGuildWarFestivalPoint() < iNeedTotalGuildPoint ) 
					return ERROR_ITEM_INSUFFICIENCY_GUILDWARPOINT;

#if defined (_VILLAGESERVER)
				if (!g_pGuildWarManager || g_pGuildWarManager->GetStepIndex() != GUILDWAR_STEP_REWARD)
					return ERROR_GUILDWAR_NOT_REWARD_STEP;

				if (pShopItem->buyLimitCount > 0)
				{
					int buyedCount = m_pSession->GetGuildWarBuyedItem(pShopItem->nItemID);
					if (buyedCount + nCount > pShopItem->buyLimitCount)
						return ERROR_ITEM_BUY_OVERFLOW_COUNT;
#if defined( PRE_ADD_LIMITED_SHOP )
					bGuildLimitedItem = true;
#endif
				}
#endif // #if defined (_VILLAGESERVER)
				break;			
			}

#if defined(PRE_SAMPLEITEMNPC)
		case Shop::PurchaseType::Sample:
			{
				InitializeSampleShopItem(pShopTableData->nSampleVersion);

				int nRet = CheckSampleShopItem(pShopItem->nItemID);
				if (nRet != ERROR_NONE)
					return nRet;
			}
			break;
#endif	// #if defined(PRE_SAMPLEITEMNPC)
#if defined(PRE_ADD_NEW_MONEY_SEED)
		case Shop::PurchaseType::Seed:
			{
				nNeedTotalSeedPoint = pShopTableData->PurchaseType[i].iPurchaseItemValue*nCount;

				if( m_pSession->GetSeedPoint() < nNeedTotalSeedPoint )
					return ERROR_SEED_INSUFFICIENCY_SEEDPOINT;

				
			}
			break;
#endif	// #if defined(PRE_SAMPLEITEMNPC)
		}
	}

	//################################################################################################
	// Check PurchaseLimitType
	//################################################################################################

	switch( pShopTableData->PurchaseLimitType )
	{
		case Shop::PurchaseLimitType::JobID:
		{
			if( m_pSession->GetUserJob() != pShopTableData->iPurchaseLimitValue )
				return ERROR_ITEM_INSUFFICIENTY_JOBID;
			break;
		}
		case Shop::PurchaseLimitType::Level:
		{
			if( m_pSession->GetLevel() < pShopTableData->iPurchaseLimitValue )
				return ERROR_ITEM_INSUFFICIENTY_LEVEL;
			break;
		}
		case Shop::PurchaseLimitType::PvPRank:
		{
			if( m_pSession->GetPvPData()->cLevel < pShopTableData->iPurchaseLimitValue )
				return ERROR_ITEM_INSUFFICIENTY_PVPRANK;
			break;
		}
		case Shop::PurchaseLimitType::GuildLevel:
		{
			bool bCheck = false;
			CDNGuildBase* pGuild = g_pGuildManager->At(m_pSession->GetGuildUID());
			if (pGuild)
			{
#if !defined( PRE_ADD_NODELETEGUILD )
				CDetachAutoEx<CDNGuildBase>	AutoDetach(pGuild);
				if (true == pGuild->IsEnable() )
				{
#endif
					if( pGuild->GetLevel() >= pShopTableData->iPurchaseLimitValue )
						bCheck = true;
#if !defined( PRE_ADD_NODELETEGUILD )
				}
#endif
			}
			
			if( bCheck == false )
				return ERROR_ITEM_INSUFFICIENTY_GUILDLEVEL;
			break;
		}
		case Shop::PurchaseLimitType::LadderGradePoint:
		{
			if( m_pSession->GetPvPLadderScoreInfoPtr()->GetTopGradePoint() < pShopTableData->iPurchaseLimitValue )
				return ERROR_ITEM_INSUFFICIENTY_LADDERGRADEPOINT;
			break;
		}
	}

	//################################################################################################
	// Check Inventory
	//################################################################################################

	if( pItemData->IsCash == false )
	{
		int iCheckInven = IsValidSpaceInventorySlotFromShop( pItemData, iBuyTotalItemCount );
		if( iCheckInven != ERROR_NONE )
			return iCheckInven;
	}

	//################################################################################################
	// Delete PurchaseType 
	//################################################################################################

	for( int i=0 ; i<Shop::Max::PurchaseType ; ++i )
	{
		switch( pShopTableData->PurchaseType[i].PurchaseType )
		{
		case Shop::PurchaseType::Gold:
			{
#if defined( _GAMESERVER )				
				g_Log.Log(LogType::_ERROR, m_pSession, L"Line7604 PrevCoin=%I64d ChangeGold=%d PickUpCoin=%I64d \r\n", m_pSession->GetCoin(), iNeedTotalGold, m_pSession->GetPickUpCoin());
#endif
				m_pSession->DelCoin( iNeedTotalGold, DBDNWorldDef::CoinChangeCode::DoNotDBSave, 0 );
				break;
			}
		case Shop::PurchaseType::ItemID:
			{
				if( DeleteInventoryByItemID( pShopTableData->PurchaseType[i].iPurchaseItemID, iNeedTotalItemCount, DBDNWorldDef::UseItem::Use ) == false ){
					_DANGER_POINT_MSG( L"ProcessBuyCombinedShop::DeleteInventoryByItemID() Failed!" );
					return ERROR_SHOP_SLOTITEM_NOTFOUND;
				}
				break;
			}
		case Shop::PurchaseType::LadderPoint:
			{
				const_cast<TPvPLadderScoreInfo*>(m_pSession->GetPvPLadderScoreInfoPtr())->UseLadderPoint( iNeedTotalLadderPoint );
				m_pSession->GetDBConnection()->QueryUsePvPLadderPoint( m_pSession, iNeedTotalLadderPoint );
				m_pSession->SendPvPLadderPointRefresh( m_pSession->GetPvPLadderScoreInfoPtr()->iPvPLadderPoint );
				break;
			}
		case Shop::PurchaseType::UnionPoint:
			{
#if defined( PRE_UNIONSHOP_RENEWAL )
				CNpcReputationProcessor::UseUnionReputePoint( m_pSession, pShopTableData->PurchaseType[i].iPurchaseItemID, iNeedTotalUnionPoint );
#else
				CNpcReputationProcessor::UseUnionReputePoint( m_pSession, pItemData->nTypeParam[2], iNeedTotalUnionPoint );
#endif // #if defined( PRE_UNIONSHOP_RENEWAL )
				break;
			}
		case Shop::PurchaseType::GuildPoint:
			{
				m_pSession->DelGuildWarFestivalPoint(iNeedTotalGuildPoint);
				m_pSession->UseEtcPoint( DBDNWorldDef::EtcPointCode::GuildWar_Festival, iNeedTotalGuildPoint );
				break;
			}
#if defined(PRE_SAMPLEITEMNPC)
		case Shop::PurchaseType::Sample:
			{
				SetSampleShopItem(pShopItem->nItemID);
			}
			break;
#endif	// #if defined(PRE_SAMPLEITEMNPC)
#if defined( PRE_ADD_NEW_MONEY_SEED )
		case Shop::PurchaseType::Seed:
			{
				m_pSession->DelSeedPoint( nNeedTotalSeedPoint );
				m_pSession->UseEtcPoint( DBDNWorldDef::EtcPointCode::SeedPoint, nNeedTotalSeedPoint );
				break;
			}
			break;
#endif
		}
	}

	//################################################################################################
	// Buying
	//################################################################################################

	// AddMaterializedItemCode 와 PayMathodCode 가 다양한 형태로 남는데 도열님께서 현재 방식이 로그로써 의미 말고 없기때문에
	// 그냥 PointBuy,Coin 으로 호출하기로 함
#if defined( PRE_ADD_NEW_MONEY_SEED )
	char cAddMaterializedItemCode	= DBDNWorldDef::AddMaterializedItem::PointBuy;
	char cPayMethodCode = DBDNWorldDef::PayMethodCode::Coin;
#else
	const char cAddMaterializedItemCode	= DBDNWorldDef::AddMaterializedItem::PointBuy;
	const char cPayMethodCode = DBDNWorldDef::PayMethodCode::Coin;
#endif

#if defined( PRE_ADD_NEW_MONEY_SEED )
	if( nNeedTotalSeedPoint > 0 )
	{
		cAddMaterializedItemCode = DBDNWorldDef::AddMaterializedItem::SeedPointBuy;
		cPayMethodCode = DBDNWorldDef::PayMethodCode::SeedPoint;
	}
#endif

#if defined(PRE_ADD_COMBINEDSHOP_PERIOD)
	nPeriod = pShopTableData->ShopItem.nPeriod;
#endif

	if( pItemData->IsCash == true )
	{
		for( int i=0 ; i<nCount ; ++i )
		{
#if defined (PRE_ADD_DONATION) || defined(PRE_ADD_COMBINEDSHOP_PERIOD)
			int iRet = CreateCashInvenItem(pShopItem->nItemID, pShopItem->nCount, cAddMaterializedItemCode, -1, nPeriod, iNeedTotalGold/nCount, m_pSession->m_nClickedNpcID, cPayMethodCode);
#else
			int iRet = CreateCashInvenItem(pShopItem->nItemID, pShopItem->nCount, cAddMaterializedItemCode, -1, 0, iNeedTotalGold/nCount, m_pSession->m_nClickedNpcID, cPayMethodCode);
#endif // #if defined (PRE_ADD_DONATION)
			if( iRet != ERROR_NONE )
				return iRet;
		}
	}
	else
	{
		// EffectItem
		if( pItemData->nType == ITEMTYPE_UNION_MEMBERSHIP )
		{
			for( int i=0 ; i<nCount ; ++i )
			{
				char cTemp = 0;
				if( BuyUnionMembership( pItemData, pShopItem->nCount, cTemp ) < 0 )
					return ERROR_SHOP_DATANOTFOUND;
			}
		}
		else
		{
			int iRet = ERROR_NONE;
			std::vector<TSaveItemInfo> VecItemList;
			VecItemList.clear();
			int iAddMaterializedPrice = 0;
			
			if( pItemData->nOverlapCount == 1 )
			{
				for( int i=0 ; i<nCount ; ++i )
				{
#if defined(PRE_ADD_COMBINEDSHOP_PERIOD)
					iRet = CreateInvenItem2( pShopItem->nItemID, pShopItem->nCount, VecItemList, CREATEINVEN_ETC, nPeriod );
#else	// PRE_ADD_COMBINEDSHOP_PERIOD
					iRet = CreateInvenItem2( pShopItem->nItemID, pShopItem->nCount, VecItemList );
#endif	// PRE_ADD_COMBINEDSHOP_PERIOD
					if( iRet != ERROR_NONE ) 
						return iRet;

					iAddMaterializedPrice = iNeedTotalGold/nCount;
				}
			}
			else 
			{
#if defined(PRE_ADD_COMBINEDSHOP_PERIOD)
				iRet = CreateInvenItem2( pShopItem->nItemID, iBuyTotalItemCount, VecItemList, CREATEINVEN_ETC, nPeriod );
#else	// #if defined(PRE_ADD_COMBINEDSHOP_PERIOD)
				iRet = CreateInvenItem2( pShopItem->nItemID, iBuyTotalItemCount, VecItemList );
#endif	// #if defined(PRE_ADD_COMBINEDSHOP_PERIOD)
				if( iRet != ERROR_NONE ) 
					return iRet;

				iAddMaterializedPrice = iNeedTotalGold;
			}

			if( m_pSession->GetDBConnection() )
			{
				for( UINT j=0 ; j<VecItemList.size() ; ++j )
				{
					DBSendAddMaterializedItem(VecItemList[j].cSlotIndex, cAddMaterializedItemCode, m_pSession->m_nClickedNpcID,	VecItemList[j].Item, iAddMaterializedPrice, 0, 0, 
						DBDNWorldDef::ItemLocation::Inventory, cPayMethodCode, VecItemList[j].bMerge, VecItemList[j].Item.nSerial);

#if defined (_VILLAGESERVER)
#if defined( PRE_ADD_LIMITED_SHOP )
					if (pShopItem->buyLimitCount > 0 && bGuildLimitedItem )
#else
					if (pShopItem->buyLimitCount > 0)
#endif
					{
						m_pSession->AddGuildWarBuyedItem(pShopItem->nItemID, nCount);
						m_pSession->GetDBConnection()->QueryAddGuildWarItemTradeRecord(m_pSession, pShopItem->nItemID, nCount);
						m_pSession->SendGuildWarBuyedItem(pShopItem->nItemID, nCount);
					}
#endif // #if defined (_VILLAGESERVER)
#if defined( PRE_ADD_LIMITED_SHOP )
					if (pShopItem->buyLimitCount > 0 && !bGuildLimitedItem )
					{
#if defined( PRE_FIX_74404 )
						m_pSession->AddLimitedShopBuyedItem(iShopID, pShopItem->nItemID, nCount, pShopItem->nShopLimitReset);
#else // #if defined( PRE_FIX_74404 )
						m_pSession->AddLimitedShopBuyedItem(pShopItem->nItemID, nCount, pShopItem->nShopLimitReset);						
#endif // #if defined( PRE_FIX_74404 )
					}
#endif
				}
			}
		}
	}
	
	//################################################################################################
	// Mission
	//################################################################################################

	m_pSession->GetEventSystem()->OnEvent( EventSystem::OnMarketBuy, 2, EventSystem::ItemID, pItemData->nItemID, EventSystem::UseCoinCount, iNeedTotalGold );

	return ERROR_NONE;
}

#if defined(PRE_ADD_REMOTE_OPENSHOP)
void CDNUserItem::OnRecvShopRemoteOpen(Shop::Type::eCode eType)
{
	int nShopID = 0;

	switch(eType)
	{
	case Shop::Type::CombinedReputePoint:
		{
			switch(m_pSession->GetClassID())
			{
			case CLASS_WARRIER: nShopID = 50001; break;
			case CLASS_ARCHER: nShopID = 50002; break;
			case CLASS_SOCERESS: nShopID = 50003; break;
			case CLASS_CLERIC: nShopID = 50004; break;
			case CLASS_ACADEMIC: nShopID = 50005; break;
			case CLASS_KALI: nShopID = 50006; break;
			}
		}
		break;

	default:
		{
			nShopID = 0;
		}
		break;
	}

	m_pSession->m_nShopID = nShopID;
	m_pSession->SetShopType(eType);
	m_pSession->m_bRemoteShopOpen = true;
}
#endif	// #if defined(PRE_ADD_REMOTE_OPENSHOP)

int CDNUserItem::OnRecvBuyNpcShopItem(int nShopID, char cShopTabID, BYTE cShopIndex, short wCount)
{
	if( m_pSession->GetShopType() >= Shop::Type::Combined )
		return ProcessBuyCombinedShop( nShopID, cShopTabID, cShopIndex, wCount );

	if( nShopID <= 0 ) 
		return ERROR_SHOP_NOTFOUND;				// shopid 이상
	if ((cShopIndex >= SHOPITEMMAX)) 
		return ERROR_SHOP_SLOTITEM_NOTFOUND;	// shop slot 이상

	TShopItem *pShopItem = g_pDataManager->GetShopItem(nShopID, cShopTabID, cShopIndex);
	if (!pShopItem) return ERROR_SHOP_DATANOTFOUND;		// shopdata가 없다
	if ((wCount <= 0) ||(wCount > pShopItem->nMaxCount)) return ERROR_ITEM_OVERFLOW;		// max값보다 더 왔다
	if (pShopItem->nItemID <= 0) return ERROR_ITEM_NOTFOUND;	// id가 없다

	if( pShopItem->nCount <= 0 || wCount%pShopItem->nCount != 0 ) return ERROR_GENERIC_INVALIDREQUEST;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	TItemData *pItemData = g_pDataManager->GetItemData(pShopItem->nItemID);
	if (!pItemData) return ERROR_SHOP_DATANOTFOUND;
	if (pItemData->nOverlapCount <= 0) return ERROR_SHOP_DATANOTFOUND;

	if (pItemData->nType == ITEMTYPE_QUEST) return ERROR_SHOP_NOTFORSALE;	// 퀘스트 아이템은 팔지 않는다

	int nOnePrice = pShopItem->nPrice;

#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
	// 만약 호감도 상점 혜택 보상 테이블에 등록되어있고, 
	// 혜택종류가 물건살 때 할인이라면 적용시켜 준다. 
	// 샵 아이템의 오버랩 카운트에 따라 아래 루틴에서 분기가 일어나지만 하나의 갯수에 
	// 할인율을 적용한 후 곱해도 상관 없기 때문에 여기서 할인 가격 적용.
	CNpcReputationProcessor::CheckAndCalcUnionBenefit( m_pSession, TStoreBenefitData::BuyingPriceDiscount, nOnePrice );
#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )

	if (!m_pSession->CheckEnoughCoin(nOnePrice * wCount)) return ERROR_ITEM_INSUFFICIENCY_MONEY;	// 돈이 충분치 않다

	int nRet = 0;
	// 아이템이 충분치 않다.
	int iNeedItemCount = wCount / pShopItem->nCount * pItemData->nNeedBuyItemCount;
	if( !CheckEnoughItem( pItemData->nNeedBuyItemID, iNeedItemCount ) ) return ERROR_ITEM_INSUFFICIENCY_ITEM;

	// 시범경기에는 구입못함
	if( pItemData->iNeedBuyLadderPoint > 0 && static_cast<int>(CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::PvPLadder_RegularSeason )) == 0 )
		return ERROR_LADDERSYSTEM_EXHIBITION_DURATION;

	if( m_pSession->GetPvPLadderScoreInfoPtr()->iPvPLadderPoint < pItemData->iNeedBuyLadderPoint*wCount ) return ERROR_ITEM_INSUFFICIENCY_LADDERPOINT;

	// 길드축제 포인트 검사
	if( m_pSession->GetGuildWarFestivalPoint() < pItemData->iNeedBuyGuildWarPoint*wCount ) return ERROR_ITEM_INSUFFICIENCY_GUILDWARPOINT;
	if (m_pSession->GetUnionReputePointInfoPtr()->GetUnionReputePoint(pItemData->nTypeParam[2]) < pItemData->iNeedBuyUnionPoint*wCount) return ERROR_ITEM_INSUFFICIENCY_UNIONPOINT;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// 먼저 인벤검사
	if (!pItemData->IsCash)
	{
		int nBlankCount = FindBlankInventorySlotCount();
		if (pItemData->nOverlapCount == 1)
		{
			if (nBlankCount < wCount) 
				return ERROR_ITEM_INVENTORY_NOTENOUGH;
		}
		else
		{
			if (nBlankCount < 1 ) 
			{
				TItem ResultItem;
				if( MakeItemStruct( pItemData->nItemID, ResultItem ) == false )
					return ERROR_ITEM_NOTFOUND;
				if( FindOverlapInventorySlot( pItemData->nItemID, wCount, ResultItem.bSoulbound, ResultItem.cSealCount, ResultItem.bEternity ) < 0 )
					return ERROR_ITEM_INVENTORY_NOTENOUGH;
			}
		}
	}

#if defined (PRE_ADD_DONATION) || defined(PRE_ADD_COMBINEDSHOP_PERIOD)
	int nPeriod = 0;
#endif // #if defined (PRE_ADD_DONATION)

	// 아이템으로 구입하는 경우 아이템 빼주고...
	if( pItemData->nNeedBuyItemCount > 0 )
	{
		// 상위에서 아이템 존재를 검사하였기 때문에 여기서 DANGER_POINT() 가 걸리면 문제가 많다~~
		if( !DeleteInventoryByItemID( pItemData->nNeedBuyItemID, iNeedItemCount, DBDNWorldDef::UseItem::Use ) )
			_DANGER_POINT();

#if defined (PRE_ADD_DONATION)
		TItemData *pNeedItemData = g_pDataManager->GetItemData(pItemData->nNeedBuyItemID);
		if (!pNeedItemData)
		{
			_DANGER_POINT();
			return ERROR_ITEM_NOTFOUND;
		}
#if defined(PRE_ADD_COMBINEDSHOP_PERIOD)	//캐쉬템 기간은 아래에서 일괄처리
#else
		if (pNeedItemData->nType == ITEMTYPE_DONATION_COUPON)
			nPeriod = pItemData->nTypeParam[1];
#endif
#endif // #if defined (PRE_ADD_DONATION)
	}

	if (pItemData->IsCash){
#if defined(PRE_ADD_COMBINEDSHOP_PERIOD)
		nPeriod = pShopItem->nPeriod;
#endif
		for (int i = 0; i < wCount; i++){
#if defined (PRE_ADD_DONATION) || defined(PRE_ADD_COMBINEDSHOP_PERIOD)
			nRet = CreateCashInvenItem(pShopItem->nItemID, pShopItem->nCount, DBDNWorldDef::AddMaterializedItem::PointBuy, -1, nPeriod, nOnePrice, m_pSession->m_nClickedNpcID, DBDNWorldDef::PayMethodCode::Coin);
#else
			nRet = CreateCashInvenItem(pShopItem->nItemID, pShopItem->nCount, DBDNWorldDef::AddMaterializedItem::PointBuy, -1, 0, nOnePrice, m_pSession->m_nClickedNpcID, DBDNWorldDef::PayMethodCode::Coin);
#endif // #if defined (PRE_ADD_DONATION)

			if (nRet != ERROR_NONE) return nRet;
			// 물건 산 돈만큼 빼주고...
#if defined( _GAMESERVER )
			g_Log.Log(LogType::_ERROR, m_pSession, L"Line7881 PrevCoin=%I64d ChangeGold=%d PickUpCoin=%I64d \r\n", m_pSession->GetCoin(), nOnePrice, m_pSession->GetPickUpCoin());
#endif
			m_pSession->DelCoin(nOnePrice, DBDNWorldDef::CoinChangeCode::DoNotDBSave, 0);
		}
	}
	else
	{
		int nPurchaseCode = 0; // 일반아이템 구매 : 0 / 이펙트 아이템 구매 : 1 / 실패 : -1
		char cAddMaterializedItemCode = DBDNWorldDef::AddMaterializedItem::PointBuy;
		char cPayMethodCode = DBDNWorldDef::PayMethodCode::Coin;

		if (pItemData->iNeedBuyLadderPoint*wCount > 0)
		{
			cAddMaterializedItemCode = DBDNWorldDef::AddMaterializedItem::PvPLadderPoint;
			cPayMethodCode = DBDNWorldDef::PayMethodCode::PvP;
		}
		if( pItemData->iNeedBuyGuildWarPoint*wCount > 0)
		{
			cAddMaterializedItemCode = DBDNWorldDef::AddMaterializedItem::GuildWarFestivalPoint;
			cPayMethodCode = DBDNWorldDef::PayMethodCode::GuildWar_Festival;
		}
		if (pItemData->iNeedBuyUnionPoint*wCount > 0)
		{
			cAddMaterializedItemCode = DBDNWorldDef::AddMaterializedItem::UnionPoint;
			nPurchaseCode = BuyUnionMembership(pItemData, wCount, cPayMethodCode);
		}

		if (nPurchaseCode == -1) 
			return ERROR_ITEM_INSUFFICIENCY_ITEM;
		
		if (nPurchaseCode == 0)
		{
			if (pItemData->nOverlapCount == 1)
			{
				for (int i = 0; i < wCount; i++)
				{
					// 아이템 인벤에 넣어주고
					std::vector<TSaveItemInfo> VecItemList;
					VecItemList.clear();
					nRet = CreateInvenItem2(pShopItem->nItemID, pShopItem->nCount, VecItemList);
					if (nRet != ERROR_NONE) return nRet;

					// 물건 산 돈만큼 빼주고...
#if defined( _GAMESERVER )
					g_Log.Log(LogType::_ERROR, m_pSession, L"Line7925 PrevCoin=%I64d ChangeGold=%d PickUpCoin=%I64d \r\n", m_pSession->GetCoin(), nOnePrice, m_pSession->GetPickUpCoin());
#endif
					m_pSession->DelCoin(nOnePrice, DBDNWorldDef::CoinChangeCode::DoNotDBSave, 0);

					if (!VecItemList.empty()){
						for (int j = 0; j <(int)VecItemList.size(); j++){
							DBSendAddMaterializedItem(VecItemList[j].cSlotIndex, cAddMaterializedItemCode, m_pSession->m_nClickedNpcID, VecItemList[j].Item, nOnePrice, 0, 0, 
								DBDNWorldDef::ItemLocation::Inventory, cPayMethodCode, VecItemList[j].bMerge, VecItemList[j].Item.nSerial);
						}
					}
				}
			}
			else 
			{
				std::vector<TSaveItemInfo> VecItemList;
				VecItemList.clear();
				nRet = CreateInvenItem2(pShopItem->nItemID, wCount, VecItemList);
				if (nRet != ERROR_NONE) return nRet;

				// 물건 산 돈만큼 빼주고...
#if defined( _GAMESERVER )
				g_Log.Log(LogType::_ERROR, m_pSession, L"Line7946 PrevCoin=%I64d ChangeGold=%d PickUpCoin=%I64d \r\n", m_pSession->GetCoin(), nOnePrice*wCount, m_pSession->GetPickUpCoin());
#endif
				m_pSession->DelCoin(nOnePrice*wCount, DBDNWorldDef::CoinChangeCode::DoNotDBSave, 0);

				if (!VecItemList.empty()){
					for (int i = 0; i <(int)VecItemList.size(); i++){
						DBSendAddMaterializedItem(VecItemList[i].cSlotIndex, cAddMaterializedItemCode, m_pSession->m_nClickedNpcID, VecItemList[i].Item, nOnePrice*wCount, 0, 0, 
							DBDNWorldDef::ItemLocation::Inventory, cPayMethodCode, VecItemList[i].bMerge, VecItemList[i].Item.nSerial);
					}
				}
			}
		}
	}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	if( pItemData->iNeedBuyLadderPoint*wCount > 0 )
	{
		const_cast<TPvPLadderScoreInfo*>(m_pSession->GetPvPLadderScoreInfoPtr())->UseLadderPoint( pItemData->iNeedBuyLadderPoint*wCount );
		m_pSession->GetDBConnection()->QueryUsePvPLadderPoint( m_pSession, pItemData->iNeedBuyLadderPoint*wCount );
		m_pSession->SendPvPLadderPointRefresh( m_pSession->GetPvPLadderScoreInfoPtr()->iPvPLadderPoint );
	}
	if( pItemData->iNeedBuyGuildWarPoint*wCount > 0 )
	{
		// 길드축제 포인트 깍는 처리.
		m_pSession->DelGuildWarFestivalPoint(pItemData->iNeedBuyGuildWarPoint*wCount);
		m_pSession->UseEtcPoint(DBDNWorldDef::EtcPointCode::GuildWar_Festival, pItemData->iNeedBuyGuildWarPoint*wCount);
	}
	if( pItemData->iNeedBuyUnionPoint*wCount > 0 )
		CNpcReputationProcessor::UseUnionReputePoint(m_pSession, pItemData->nTypeParam[2], pItemData->iNeedBuyUnionPoint*wCount);

	m_pSession->GetEventSystem()->OnEvent( EventSystem::OnMarketBuy, 2, EventSystem::ItemID, pItemData->nItemID, EventSystem::UseCoinCount, nOnePrice * wCount );

	return ERROR_NONE;
}

int CDNUserItem::OnRecvSellNpcShopItem(const CSShopSell *pPacket)
{
	if (!_CheckRangeInventoryIndex(pPacket->cInvenIndex)) return ERROR_SHOP_SLOTITEM_NOTFOUND;	// 인벤 인덱스 잘못왔음

	const TItem *pInven = GetInventory(pPacket->cInvenIndex);
	if (!pInven) return ERROR_ITEM_NOTFOUND;
	if (pInven->nItemID <= 0) return ERROR_ITEM_NOTFOUND;	// 인벤에 아이템이 없다
	if ((pPacket->wCount <= 0) ||(pPacket->wCount > pInven->wCount)) return ERROR_ITEM_OVERFLOW;		// 인벤에 있는것보다 더 왔다
	if (pInven->nSerial != pPacket->biItemSerial) return ERROR_ITEM_NOTFOUND;

	TItemData *pItemData = g_pDataManager->GetItemData(pInven->nItemID);
	if (!pItemData) return ERROR_ITEM_NOTFOUND;

	if (pItemData->nType == ITEMTYPE_QUEST) return ERROR_SHOP_NOTFORSALE;	// 퀘스트 아이템은 팔지 않는다
#ifdef PRE_MOD_SELL_SEALEDITEM
	bool bIsBelong = (pItemData->cReversion == ITEMREVERSION_BELONG && pInven->bSoulbound == false);
#else		//#ifdef PRE_MOD_SELL_SEALEDITEM
	if (pItemData->cReversion == ITEMREVERSION_BELONG && pInven->bSoulbound == false) return ERROR_SHOP_NOTFORSALE;		// 봉인 아이템
#endif		//#ifdef PRE_MOD_SELL_SEALEDITEM

	int nPrice = pItemData->nSellAmount * pPacket->wCount;	// 돈계산
	if (nPrice <= 0) return ERROR_SHOP_NOTFORSALE;	// 판매가격 0이면 판매 불가 아이템
	if (!m_pSession->CheckMaxCoin(nPrice)) return ERROR_SHOP_OVERFLOWMONEY;		// 인벤 돈 초과

	TItem SellItem = *pInven;
	SellItem.wCount = pPacket->wCount;
	bool bAllFlag = pPacket->wCount==pInven->wCount ? true:false ;

	// 인벤에서 버리쟈
	if (!DeleteInventoryBySlot(pPacket->cInvenIndex, pPacket->wCount, pPacket->biItemSerial, DBDNWorldDef::UseItem::DoNotDBSave)) return ERROR_SHOP_DATANOTFOUND;

#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
		// 만약 호감도 상점 혜택 보상 테이블에 등록되어있고, 
		// 혜택종류가 물건 팔 때 가격을 올려주는 거라면 적용시켜 준다.
	if(pItemData->nType != ITEMTYPE_FIXEDPRICE)
		CNpcReputationProcessor::CheckAndCalcUnionBenefit( m_pSession, TStoreBenefitData::SellingPriceUp, nPrice );
#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )

#if defined( _GAMESERVER )
	g_Log.Log(LogType::_ERROR, m_pSession, L"Line8021 PrevCoin=%I64d ChangeGold=%d PickUpCoin=%I64d \r\n", m_pSession->GetCoin(), nPrice, m_pSession->GetPickUpCoin());
#endif
	m_pSession->AddCoin(nPrice, DBDNWorldDef::CoinChangeCode::DoNotDBSave, 0);	// 돈 넣어주고

	if (m_pSession->GetDBConnection())
	{		
#ifdef PRE_MOD_SELL_SEALEDITEM
		m_pSession->GetDBConnection()->QueryResellItem(m_pSession, SellItem.nSerial, SellItem.wCount, nPrice, bAllFlag, pItemData->bRebuyable, bIsBelong);
#else		//#ifdef PRE_MOD_SELL_SEALEDITEM
		m_pSession->GetDBConnection()->QueryResellItem(m_pSession, SellItem.nSerial, SellItem.wCount, nPrice, bAllFlag, pItemData->bRebuyable, false);
#endif		//#ifdef PRE_MOD_SELL_SEALEDITEM
	}
	m_pSession->GetEventSystem()->OnEvent( EventSystem::OnMarketSell, 2, EventSystem::ItemID, pItemData->nItemID, EventSystem::UseCoinCount, nPrice );

	return ERROR_NONE;
}

int CDNUserItem::OnRecvShopRepurchase( const CSShopRepurchase* pPacket )
{
	std::map<int,TRepurchaseItemInfo>::iterator itor = m_mRepurchaseItemInfo.find( pPacket->iRepurchaseID );
	if( itor == m_mRepurchaseItemInfo.end() )
		return ERROR_ITEM_NOTFOUND;

	// 소지돈 확인
	int iNeedCoin = static_cast<int>((*itor).second.iSellPrice*CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::Shop_Repurchase_Fee ));
	if( m_pSession->CheckEnoughCoin(iNeedCoin) == false )
		return ERROR_ITEM_INSUFFICIENCY_MONEY;

	const TItemData* pItemData = g_pDataManager->GetItemData( (*itor).second.Item.nItemID );
	if( pItemData == NULL )
		return ERROR_ITEM_NOTFOUND;

	if( IsValidSpaceInventorySlotFromShop( pItemData, (*itor).second.Item.wCount ) != ERROR_NONE )
		return ERROR_ITEM_INVENTORY_NOTENOUGH;

	// 인벤에 넣기
	std::vector<TSaveItemInfo> VecItemList;
	VecItemList.clear();

	int iRet = _CreateInvenItemEx( (*itor).second.Item, 0, 0, VecItemList, true, CREATEINVEN_REPURCHASE );
	if( iRet != ERROR_NONE )
		return iRet;

	BYTE cAddMaterializedItemCode = DBDNWorldDef::AddMaterializedItem::Repurchase;
	BYTE cPayMethodCode = DBDNWorldDef::PayMethodCode::Coin;

	if( !VecItemList.empty() )
	{
		for (int i = 0; i <(int)VecItemList.size(); i++)
		{
			DBSendAddMaterializedItem(VecItemList[i].cSlotIndex, cAddMaterializedItemCode, (*itor).second.iRepurchaseID, VecItemList[i].Item, iNeedCoin, 0, 0, DBDNWorldDef::ItemLocation::Inventory,
				cPayMethodCode, VecItemList[i].bMerge, VecItemList[i].Item.nSerial);
		}
	}

	// 돈감소
#if defined( _GAMESERVER )
	g_Log.Log(LogType::_ERROR, m_pSession, L"Line8078 PrevCoin=%I64d ChangeGold=%d PickUpCoin=%I64d \r\n", m_pSession->GetCoin(), iNeedCoin, m_pSession->GetPickUpCoin());
#endif
	m_pSession->DelCoin( iNeedCoin, DBDNWorldDef::CoinChangeCode::DoNotDBSave, 0 );

	// 리스트제거
	m_mRepurchaseItemInfo.erase( pPacket->iRepurchaseID );
	return ERROR_NONE;
}

// iUseItemID == 0 인경우 상점수리
// iUseITemID != 0 인경우 수리아이템 사용
int CDNUserItem::OnRecvRepairEquip( int iUseItemID/*=0*/ )	// 장착수리(equip)
{
#if defined(PRE_ADD_REPAIR_NPC)
	int nRet = CheckRepairEquip(iUseItemID);
	m_pSession->SendRepairEquip(nRet);

	return nRet;

#else //#if defined(PRE_ADD_REPAIR_NPC)
	if ( iUseItemID == 0 )
	{
#if !defined (PRE_MOD_GAMESERVERSHOP)
#if defined( _GAMESERVER )
		// 게임서버에서는 상점수리가 불가능하다.
		return false;
#endif // #if defined( _GAMESERVER )
#endif
	}

	int nTotalPrice = _CalcRepairEquipPrice();

	if (nTotalPrice == 0){	// 수리할께 하나도 없다
		m_pSession->SendRepairEquip(ERROR_NONE);
		return false;
	}

	if( iUseItemID == 0 )
	{
#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
		// 만약 호감도 상점 혜택 보상 테이블에 등록되어있고, 
		// 혜택종류가 수리비 할인이라면 적용시켜 준다.
		CNpcReputationProcessor::CheckAndCalcUnionBenefit( m_pSession, TStoreBenefitData::RepairFeeDiscount, nTotalPrice );
#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
	}
	else
	{
		const TItemData* pItemData = g_pDataManager->GetItemData( iUseItemID );
		if( pItemData )
			nTotalPrice -= nTotalPrice*pItemData->nTypeParam[0]/100;
	}

	if( nTotalPrice < 0 )
		nTotalPrice = 0;

	if (!m_pSession->CheckEnoughCoin(nTotalPrice)){
		m_pSession->SendRepairEquip(ERROR_ITEM_INSUFFICIENCY_MONEY);
		return false;
	}

#if defined( _GAMESERVER )
	DnActorHandle hActor = m_pSession->GetActorHandle();
	if( hActor && hActor->IsPlayerActor() )
	{
		CDnPlayerActor* pPlayer = static_cast<CDnPlayerActor*>(hActor.GetPointer());
		pPlayer->OnRepairEquipDurability( true, nTotalPrice );		
	}
#else
	std::vector<INT64> VecSerialList;
	std::vector<short> VecDurList;
	VecSerialList.clear();
	VecDurList.clear();
	
	_RepairEquip(VecSerialList, VecDurList);
	if (m_pSession->GetDBConnection())
		m_pSession->GetDBConnection()->QueryModItemDurability(m_pSession, nTotalPrice, VecSerialList, VecDurList);
#endif // #if defined( _GAMESERVER )
	m_pSession->DelCoin(nTotalPrice, DBDNWorldDef::CoinChangeCode::DoNotDBSave, 0);

	m_pSession->SendRepairEquip(ERROR_NONE);

	m_pSession->GetEventSystem()->OnEvent( EventSystem::OnItemRepair, 1, EventSystem::UseCoinCount, nTotalPrice );

	return ERROR_NONE;
#endif //#if defined(PRE_ADD_REPAIR_NPC)
}

void CDNUserItem::OnRecvRepairAll( bool bCheat/*=false*/ )	// 전체수리(equip + inven)
{
#if defined(PRE_ADD_REPAIR_NPC)
	int nRet = CheckRepairAll(bCheat);
	if( nRet == ERROR_ITEM_REPAIR)
		nRet = ERROR_NONE;
	m_pSession->SendRepairAll(nRet);
#else // #if defined(PRE_ADD_REPAIR_NPC)
#if defined(_VILLAGESERVER) || defined (PRE_MOD_GAMESERVERSHOP)
	int nEquipPrice = _CalcRepairEquipPrice();
	int nInvenPrice = _CalcRepairInvenPrice();
	int nTotalPrice = nEquipPrice + nInvenPrice;

	if (nTotalPrice == 0){	// 수리할께 하나도 없다
		m_pSession->SendRepairAll(ERROR_NONE);
		return;
	}

#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
	// 만약 호감도 상점 혜택 보상 테이블에 등록되어있고, 
	// 혜택종류가 수리비 할인이라면 적용시켜 준다.
	CNpcReputationProcessor::CheckAndCalcUnionBenefit( m_pSession, TStoreBenefitData::RepairFeeDiscount, nTotalPrice );
#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )

	if( !bCheat )
	{
		if (!m_pSession->CheckEnoughCoin(nTotalPrice)){
			m_pSession->SendRepairAll(ERROR_ITEM_INSUFFICIENCY_MONEY);
			return;
		}
	}

	std::vector<INT64> VecSerialList;
	std::vector<short> VecDurList;
	VecSerialList.clear();
	VecDurList.clear();

	if (nEquipPrice > 0)
	{
#if defined( _GAMESERVER )
		DnActorHandle hActor = m_pSession->GetActorHandle();
		if( hActor && hActor->IsPlayerActor() )
		{
			CDnPlayerActor* pPlayer = static_cast<CDnPlayerActor*>(hActor.GetPointer());
			pPlayer->OnRepairEquipDurability( true, nTotalPrice );			
		}
#else
		_RepairEquip(VecSerialList, VecDurList);
#endif //#if defined( _GAMESERVER )
	}
	if (nInvenPrice >0) _RepairInven(VecSerialList, VecDurList);

	m_pSession->DelCoin(nTotalPrice, DBDNWorldDef::CoinChangeCode::DoNotDBSave, 0);

	m_pSession->GetDBConnection()->QueryModItemDurability(m_pSession, nTotalPrice, VecSerialList, VecDurList);

	m_pSession->SendRepairAll(ERROR_NONE);

	m_pSession->GetEventSystem()->OnEvent( EventSystem::OnItemRepair, 1, EventSystem::UseCoinCount, nTotalPrice );
#endif //#if defined(_VILLAGESERVER) || defined (PRE_MOD_GAMESERVERSHOP)
#endif // #if defined(PRE_ADD_REPAIR_NPC)
}

#if defined(PRE_ADD_REPAIR_NPC)
int CDNUserItem::CheckRepairEquip( int iUseItemID ) // 장착수리(equip)
{
	int nTotalPrice = _CalcRepairEquipPrice();

	if (nTotalPrice == 0){	// 수리할께 하나도 없다		
		return ERROR_ITEM_REPAIR;
	}

	if( iUseItemID == 0 )
	{
#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
		// 만약 호감도 상점 혜택 보상 테이블에 등록되어있고, 
		// 혜택종류가 수리비 할인이라면 적용시켜 준다.
		CNpcReputationProcessor::CheckAndCalcUnionBenefit( m_pSession, TStoreBenefitData::RepairFeeDiscount, nTotalPrice );
#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
	}
	else
	{
		const TItemData* pItemData = g_pDataManager->GetItemData( iUseItemID );
		if( pItemData )
			nTotalPrice -= nTotalPrice*pItemData->nTypeParam[0]/100;
	}

	if( nTotalPrice < 0 )
		nTotalPrice = 0;

	if (!m_pSession->CheckEnoughCoin(nTotalPrice)){		
		return ERROR_ITEM_INSUFFICIENCY_MONEY;
	}

#if defined( _GAMESERVER )
	DnActorHandle hActor = m_pSession->GetActorHandle();
	if( hActor && hActor->IsPlayerActor() )
	{
		CDnPlayerActor* pPlayer = static_cast<CDnPlayerActor*>(hActor.GetPointer());
		pPlayer->OnRepairEquipDurability( true, nTotalPrice );		
	}
#else
	std::vector<INT64> VecSerialList;
	std::vector<USHORT> VecDurList;
	VecSerialList.clear();
	VecDurList.clear();

	_RepairEquip(VecSerialList, VecDurList);
	if (m_pSession->GetDBConnection())
		m_pSession->GetDBConnection()->QueryModItemDurability(m_pSession, nTotalPrice, VecSerialList, VecDurList);
#endif // #if defined( _GAMESERVER )

#if defined( _GAMESERVER )
	g_Log.Log(LogType::_ERROR, m_pSession, L"Line8281 PrevCoin=%I64d ChangeGold=%d PickUpCoin=%I64d \r\n", m_pSession->GetCoin(), nTotalPrice, m_pSession->GetPickUpCoin());
#endif
	m_pSession->DelCoin(nTotalPrice, DBDNWorldDef::CoinChangeCode::DoNotDBSave, 0);
	m_pSession->GetEventSystem()->OnEvent( EventSystem::OnItemRepair, 1, EventSystem::UseCoinCount, nTotalPrice );

	return ERROR_NONE;
}

int CDNUserItem::CheckRepairAll( bool bCheat )	// 전체수리(equip + inven)
{
	int nEquipPrice = _CalcRepairEquipPrice();
	int nInvenPrice = _CalcRepairInvenPrice();
	int nTotalPrice = nEquipPrice + nInvenPrice;

	if (nTotalPrice == 0){	// 수리할께 하나도 없다		
		return ERROR_ITEM_REPAIR;
	}

#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
	// 만약 호감도 상점 혜택 보상 테이블에 등록되어있고, 
	// 혜택종류가 수리비 할인이라면 적용시켜 준다.
	CNpcReputationProcessor::CheckAndCalcUnionBenefit( m_pSession, TStoreBenefitData::RepairFeeDiscount, nTotalPrice );
#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )

	if( !bCheat )
	{
		if (!m_pSession->CheckEnoughCoin(nTotalPrice)){			
			return ERROR_ITEM_INSUFFICIENCY_MONEY;
		}
	}

	std::vector<INT64> VecSerialList;
	std::vector<USHORT> VecDurList;
	VecSerialList.clear();
	VecDurList.clear();

	if (nEquipPrice > 0)
	{
#if defined( _GAMESERVER )
		DnActorHandle hActor = m_pSession->GetActorHandle();
		if( hActor && hActor->IsPlayerActor() )
		{
			CDnPlayerActor* pPlayer = static_cast<CDnPlayerActor*>(hActor.GetPointer());
			pPlayer->OnRepairEquipDurability( true, 0 );
		}
#else
		_RepairEquip(VecSerialList, VecDurList);
#endif
	}
	if (nInvenPrice >0) _RepairInven(VecSerialList, VecDurList);

#if defined( _GAMESERVER )
	g_Log.Log(LogType::_ERROR, m_pSession, L"Line8333 PrevCoin=%I64d ChangeGold=%d PickUpCoin=%I64d \r\n", m_pSession->GetCoin(), nTotalPrice, m_pSession->GetPickUpCoin());
#endif
	m_pSession->DelCoin(nTotalPrice, DBDNWorldDef::CoinChangeCode::DoNotDBSave, 0);

	m_pSession->GetDBConnection()->QueryModItemDurability(m_pSession, nTotalPrice, VecSerialList, VecDurList);

	m_pSession->GetEventSystem()->OnEvent( EventSystem::OnItemRepair, 1, EventSystem::UseCoinCount, nTotalPrice );
	return ERROR_NONE;
}
#endif //#if defined(PRE_ADD_REPAIR_NPC)

#ifdef PRE_FIX_ONCANCEL_USEITEM
void CDNUserItem::OnCancelUseItem(const CSUseItem& Packet)
{
#if defined _VILLAGESERVER
	switch(Packet.cInvenType)
	{
	case ITEMPOSITION_INVEN:
		{
		}
		break;

	case ITEMPOSITION_CASHINVEN:
		{
			const TItem *pItem = GetCashInventory(Packet.biInvenSerial);
			if (!pItem) return;

			int nItemType = g_pDataManager->GetItemMainType(pItem->nItemID);
			switch(nItemType)
			{
			case ITEMTYPE_COSTUMEMIX:
				m_pSession->StartCostumeMix(Packet.cInvenType, 0);
				break;

			case ITEMTYPE_COSTUMEDESIGN_MIX:
				m_pSession->StartCostumeDesignMix(Packet.cInvenType, Packet.cInvenIndex, 0);
				break;
			}
		}
		break;
	}
#endif // _VILLAGESERVER
}
#endif // PRE_FIX_ONCANCEL_USEITEM

void CDNUserItem::OnRecvUseItem(CSUseItem *pPacket)
{
	if (!m_pSession->IsNoneWindowState())
	{
	#ifdef PRE_FIX_ONCANCEL_USEITEM
		if (pPacket)
			OnCancelUseItem(*pPacket);
	#endif
		return;	// 
	}
	const TItem *pItem = NULL;
#if defined(_GAMESERVER)

	CDnItem *pDnItem = NULL;

	CDNGameRoom *pRoom = m_pSession->GetGameRoom();
	if (!pRoom) return;
	if (!pRoom->GetTaskMng()) return;

	CDnItemTask *pItemTask = (CDnItemTask *)pRoom->GetTaskMng()->GetTask( "ItemTask" );
	if (!pItemTask) return;

#endif	// #if defined(_GAMESERVER)

	switch(pPacket->cInvenType)
	{
	case ITEMPOSITION_INVEN:
		{
			pItem = GetInventory(pPacket->cInvenIndex);
			if (!pItem) return;
			if (pItem->nSerial != pPacket->biInvenSerial) return;

#if defined(_GAMESERVER)
			pDnItem = pItemTask->GetInventoryItem( m_pSession, pPacket->cInvenIndex );
			if (!pDnItem) return;
#endif	// #if defined(_GAMESERVER)
		}
		break;

	case ITEMPOSITION_CASHINVEN:
		{
			pItem = GetCashInventory(pPacket->biInvenSerial);
			if (!pItem) return;
#if defined(_GAMESERVER)
			pDnItem = pItemTask->GetCashInventoryItem( m_pSession, pPacket->biInvenSerial );
			if (!pDnItem) return;
#endif	// #if defined(_GAMESERVER)
		}
		break;

	default:
		{
			if (!pItem) return;
#if defined(_GAMESERVER)
			if (!pDnItem) return;
#endif	// #if defined(_GAMESERVER)
		}
		break;
	}

	if (pItem->wCount <= 0) return;	// 0이면 나가라

	TItemData *pItemData = g_pDataManager->GetItemData(pItem->nItemID);
	if (!pItemData) return;

	switch(pPacket->cInvenType)
	{
	case ITEMPOSITION_INVEN:
		if (pItemData->IsCash) return;	// 캐쉬템 이면 나가라
		break;

	case ITEMPOSITION_CASHINVEN:
		if (!pItemData->IsCash) return;	// 캐쉬템 아니면 나가라
		break;
	}

#if defined(_GAMESERVER)
	// 죽은 상태 사용아이템이 추가되었다 체크한다
	if (!m_pSession->GetActorHandle())
		return;

	if (m_pSession->GetActorHandle()->IsDie())
	{
		if (pRoom->bIsGuildWarMode() == false || pItemData->nType != ITEMTYPE_IMMEDIATEREBIRTH)
			return;
	}
#endif		//#if defined(_GAMESERVER)

	// 과거 전직 히스토리까지 뒤져서 판단해주도록 변경.(ex 1차 전직 스킬북 사용)
	if( !g_pDataManager->IsItemNeedJobHistory( pItem->nItemID, m_pSession->GetStatusData()->cJobArray ) ) return;

	// 레벨 체크
	if( m_pSession->GetLevel() < pItemData->cLevelLimit ) return;

	if( pItemData->iUseLevelLimit > 0 && m_pSession->GetLevel() > pItemData->iUseLevelLimit )
		return;

	// AllowMapType 검사
	if( !g_pDataManager->IsUseItemAllowMapTypeCheck( pItem->nItemID, m_pSession->GetMapIndex() ) )
		return;

#if defined( PRE_ADD_STAGE_USECOUNT_ITEM )
	int nUseLimitItem = 0;	
	nUseLimitItem = GetUseLimitItemCount(pItemData->nItemID);
	if( nUseLimitItem == 0 )
		return;
#endif
	bool bUseItem = false;
	switch( pItemData->nType ) 
	{
	case ITEMTYPE_NORMAL:
	case ITEMTYPE_COOKING:
		{
			INT64 nItemSerialID = pItem->nSerial;
			int nItemRandomSeed = pItem->nRandomSeed;
			if (!DeleteItemByUse(pPacket->cInvenType, pPacket->cInvenIndex, pPacket->biInvenSerial)) break;

#if defined(_VILLAGESERVER)
			m_pSession->SetLastUseItemSkillID( pItemData->nSkillID );
#endif
			// 조금 느리더레도 복사하게 합니다.
			if (pItemData->nSkillID != 0)
			{
#if defined(_GAMESERVER)
				CDnItem *pTempItem;

				PROFILE_TICK_TEST_BLOCK_START( "AAAAAAAAAAAA" );
				pTempItem = CDnItem::CreateItem( pRoom, pItemData->nItemID, nItemRandomSeed );
				PROFILE_TICK_TEST_BLOCK_END();

#if defined( PRE_FIX_BUFFITEM )
				CDNUserSession::RemoveApplySkill( m_pSession->GetPlayerActor(), pItemData->nSkillID );
				TEffectSkillData* EffectSkill = GetEffectSkillItem( EffectSkillNameSpace::SearchType::SkillID, pItemData->nSkillID );
				if( EffectSkill )
				{
					m_pSession->GetItem()->BroadcastDelEffectSkillItemData( EffectSkill->nItemID );
					m_pSession->GetDBConnection()->QueryDelEffectItem( m_pSession, EffectSkill->nItemSerial );
				}				
#endif
				pTempItem->ActivateSkillEffect( m_pSession->GetActorHandle() );
				SAFE_DELETE( pTempItem );
#endif	// #if defined(_GAMESERVER)
			}

			bUseItem = true;
		}
		break;

	case ITEMTYPE_HAIRDYE:
	case ITEMTYPE_SKINDYE:
	case ITEMTYPE_EYEDYE:
		{
#ifdef PRE_FIX_63822
			if (m_pSession->AddDataBaseMessageSequence(MAINCMD_STATUS, QUERY_COLOR) == false)
				break;
#endif		//#ifdef PRE_FIX_63822

			if (!DeleteItemByUse(pPacket->cInvenType, pPacket->cInvenIndex, pPacket->biInvenSerial)) break;
			if ( m_pSession->ChangeDyeColor(pItemData->nType,  pItemData->nTypeParam) == false ) break;
			bUseItem = true;
		}
		break;

	case ITEMTYPE_FACIAL:
	case ITEMTYPE_HAIRDRESS:
		{
#ifdef PRE_FIX_63822
			if (m_pSession->AddDataBaseMessageSequence(MAINCMD_MSGADJUST, QUERY_MSGADJUST_SEQ_CHANGEPARTS) == false)
				break;

			if (!DeleteItemByUse(pPacket->cInvenType, pPacket->cInvenIndex, pPacket->biInvenSerial)) break;
			if ( ChangeDefaultParts( pItemData->nType, pItemData->nTypeParam, pItem->nSerial ) == false ) break;

			if (m_pSession->GetDBConnection())
				m_pSession->GetDBConnection()->QueryMsgAdjSeq(m_pSession, QUERY_MSGADJUST_SEQ_CHANGEPARTS);

			bUseItem = true;
#else		//#ifdef PRE_FIX_63822
			if (!DeleteItemByUse(pPacket->cInvenType, pPacket->cInvenIndex, pPacket->biInvenSerial)) break;
			if ( ChangeDefaultParts( pItemData->nType, pItemData->nTypeParam, pItem->nSerial ) == false ) break;
			bUseItem = true;
#endif		//#ifdef PRE_FIX_63822
		}
		break;

		// 마을에서는 랜덤아이템일 경우만 까줘야한다.
	case ITEMTYPE_RANDOM:
		{
			if (!RequestRandomItem(pPacket)) break;
			bUseItem = true;
			// 프로그레스 상태로 변경
			m_pSession->SetWindowState(WINDOW_PROGRESS);
		}
		break;

		// 스킬북 아이템 사용. 그에 해당하는 스킬의 언락을 시도해본다.
		// 1차적으론 클라에서 검증하고 날아옴. 
		// 따라서 여기서 실패한다면 핵일 가능성이..
	case ITEMTYPE_SKILLBOOK:
		{
#if defined(_VILLAGESERVER)
			if( !m_pSession->GetSkill()->UseSkillBook( pItemData->nItemID ) )	break;
			if (!DeleteItemByUse(pPacket->cInvenType, pPacket->cInvenIndex, pPacket->biInvenSerial)) break;
			bUseItem = true;

#elif defined(_GAMESERVER)
			CDnSkillTask* pSkillTask = static_cast<CDnSkillTask*>(CTaskManager::GetInstance( m_pSession->GetGameRoom() ).GetTask( "SkillTask" ));
			if (!pSkillTask) break;

			bool bSuccess = pSkillTask->UseSkillBook( m_pSession, pItemData->nItemID );
			if( bSuccess )
			{
				if (!DeleteItemByUse(pPacket->cInvenType, pPacket->cInvenIndex, pPacket->biInvenSerial)) break;
				bUseItem = true;
			}

#endif	// #if defined(_VILLAGESERVER)
		}
		break;

	case ITEMTYPE_RESET_SKILL:
	case ITEMTYPE_INFINITY_RESET_SKILL:
		{
#if defined(_VILLAGESERVER)
			// 우선 사용 가능한지 불가능한지 상황만 판단해서 클라로 알려주면 클라 쪽에서 
			// 확인 버튼을 눌렀을 때 인벤에서 아이템 까면서 실제로 리셋이 되도록 한다.
			int iRewardSP = 0;
			vector<int> vlSkillIDsToReset;
			int nRetCode = CanUseSkillResetCashItem( pItemData->nTypeParam[0]-1, pItemData->nTypeParam[1]-1, iRewardSP, vlSkillIDsToReset );
			m_pSession->SendCanUseSkillResetCashItem( pPacket->biInvenSerial, iRewardSP, nRetCode );

			if( ERROR_NONE != nRetCode )
				break;

			bUseItem = true;

#endif	// #if defined(_VILLAGESERVER)
		}
		break;

	case ITEMTYPE_WORLDMSG:
		{
			if (!RequestWorldMsgItem(pPacket)) break;
			bUseItem = true;
		}
		break;

#if defined _VILLAGESERVER
	case ITEMTYPE_COSTUMEMIX:
		{
			m_pSession->StartCostumeMix(pPacket->cInvenType, pPacket->biInvenSerial);
			bUseItem = false;
		}
		break;

	case ITEMTYPE_COSTUMEDESIGN_MIX:
		{
			m_pSession->StartCostumeDesignMix(pPacket->cInvenType, pPacket->cInvenIndex, pPacket->biInvenSerial);
			bUseItem = false;
		}
		break;

#ifdef PRE_ADD_COSRANDMIX
	case ITEMTYPE_COSTUMERANDOM_MIX:
		{
	#ifdef PRE_ADD_COSRANDMIX_ACCESSORY
			m_pSession->StartCostumeRandomMix(CostumeMix::RandomMix::OpenByItem, pPacket->cInvenType, pPacket->biInvenSerial);
	#else
			m_pSession->StartCostumeRandomMix(pPacket->cInvenType, pPacket->biInvenSerial);
	#endif
			bUseItem = false;
		}
		break;
#endif // PRE_ADD_COSRANDMIX
#endif // _VILLAGESERVER

	case ITEMTYPE_VEHICLEHAIRCOLOR:
		{
			if (!DeleteItemByUse(pPacket->cInvenType, pPacket->cInvenIndex, pPacket->biInvenSerial)) break;
			if (ChangeVehicleDyeColor(pItemData->nTypeParam) == false) break;
			bUseItem = true;
		}
		break;

	case ITEMTYPE_PETCOLOR_BODY:
		{
			if (!DeleteItemByUse(pPacket->cInvenType, pPacket->cInvenIndex, pPacket->biInvenSerial)) break;
			if (ChangePetBodyColor(pItemData->nTypeParam) == false) break;
			bUseItem = true;
		}
		break;

	case ITEMTYPE_PETCOLOR_TATOO:
		{
			if (!DeleteItemByUse(pPacket->cInvenType, pPacket->cInvenIndex, pPacket->biInvenSerial)) break;
			if (ChangePetNoseColor(pItemData->nTypeParam) == false) break;
			bUseItem = true;
		}
		break;

	case ITEMTYPE_PETALTOKEN:
		{
			if (g_pCashConnection->m_bPetalSaleAbort) return;	// 충전 막혔다

			m_pSession->GetDBConnection()->QueryPetal(m_pSession, pItemData->nTypeParam[0], pItem->nItemID, pPacket->cInvenType, pPacket->cInvenIndex, pPacket->biInvenSerial);
			bUseItem = true;
		}
		break;

	case ITEMTYPE_APPELLATION:
		{
			int nAppellationArray = pItemData->nTypeParam[0] - 1;
			if( m_pSession->GetAppellation()->IsExistAppellation( nAppellationArray ) ) {
				m_pSession->SendAppellationGainResult( nAppellationArray, ERROR_ITEM_APPELLATIONGAIN_EXIST );
				break;
			}
			if (!DeleteItemByUse(pPacket->cInvenType, pPacket->cInvenIndex, pPacket->biInvenSerial)) break;
			m_pSession->GetAppellation()->RequestAddAppellation( nAppellationArray );
			bUseItem = true;
		}
		break;

	case ITEMTYPE_CHARNAME:
		break;

	case ITEMTYPE_REMOTE_ENCHANT:
	{
#if defined( _VILLAGESERVER )
		int iRet = api_ui_OpenUpgradeItem( m_pSession->GetObjectID(), pItemData->nItemID );
#elif defined( _GAMESERVER )
		int iRet = api_ui_OpenUpgradeItem( m_pSession->GetGameRoom(), m_pSession->GetSessionID(), pItemData->nItemID );
#endif // #if defined( _VILLAGESERVER )
		if( iRet <= 0 )
			break;
		break;
	}
	case ITEMTYPE_REMOTE_ITEMCOMPOUND:
	{
#if defined( _VILLAGESERVER )
		int iRet = api_ui_OpenCompound2Item( m_pSession->GetObjectID(), pItemData->nTypeParam[1], pItemData->nItemID );
#elif defined( _GAMESERVER )
		int iRet = api_ui_OpenCompound2Item( m_pSession->GetGameRoom(), m_pSession->GetSessionID(), pItemData->nTypeParam[1], pItemData->nItemID );
#endif // #if defined( _VILLAGESERVER )
		if( iRet <= 0 )
			break;
		break;
	}

#if defined( _GAMESERVER )
	case ITEMTYPE_RETURN_HOME :
		{
			if( !m_pSession->GetGameRoom() ) return;
			if( !m_pSession->GetGameRoom()->bIsPvPRoom() ) return;

			CDnPvPGameTask * pTask = reinterpret_cast<CDnPvPGameTask *>( m_pSession->GetGameRoom()->GetGameTask() );
			if( !pTask ) return;

			CPvPRespawnLogic *pRespawnLogic = pTask->GetRespawnLogic();
			if( !pRespawnLogic ) return;

			CEtWorldEventArea * pEventArea = pRespawnLogic->OnRespawn( m_pSession->GetActorHandle() );
			if( !pEventArea ) return;

			if (!DeleteItemByUse(pPacket->cInvenType, pPacket->cInvenIndex, pPacket->biInvenSerial)) break;
			api_trigger_WarpActor( pRoom, m_pSession->GetActorHandle()->GetSessionID(), pEventArea->GetCreateUniqueID() );
			bUseItem = true;
		}
		break;
#endif	//#if defined( _GAMESERVER )

	case ITEMTYPE_REPAIR_EQUIPITEM:
	{
		if( OnRecvRepairEquip( pItemData->nItemID ) == ERROR_NONE )
		{
			if( DeleteItemByUse(pPacket->cInvenType, pPacket->cInvenIndex, pPacket->biInvenSerial) == false ) 
				break;
			bUseItem = true;
		}
		break;
	}

	case ITEMTYPE_FATIGUEUP:
		{
			INT64 biValue = 0;
			m_pSession->GetCommonVariableDataValue(CommonVariable::Type::AddFatigue, biValue);
			// 사용 가능 횟수 체크
			int iDailyUseCount = static_cast<int>((CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::FatigueLimit)));
			if( biValue >= iDailyUseCount ) 
			{
				m_pSession->SendFatigueResult(m_pSession->GetSessionID(), ERROR_CANT_USE_ITEM_DAYILY);
				break;
			}
			// max 피로도이상 체크
			int nMax = g_pDataManager->GetFatigue(m_pSession->GetUserJob(), m_pSession->GetLevel());
			if( m_pSession->GetFatigue() >= nMax )	
			{
				m_pSession->SendFatigueResult(m_pSession->GetSessionID(), ERROR_ITEM_FAIL);
				break;
			}

			if( DeleteItemByUse(pPacket->cInvenType, pPacket->cInvenIndex, pPacket->biInvenSerial) == false ) break;

			// 피로도 회복
			m_pSession->IncreaseFatigue(pItemData->nTypeParam[0]);
			// 피로도 속성 테이블 업데이트.
			m_pSession->ModCommonVariableData( CommonVariable::Type::AddFatigue, ++biValue );
		}
		break;

	case ITEMTYPE_ULTIMATEFATIGUEUP:
		{
			if (IsExpired(*pItem)){
				m_pSession->SendFatigueResult(m_pSession->GetSessionID(), ERROR_ITEM_FAIL);
				break;
			}

			// max 피로도이상 체크
			int nMax = g_pDataManager->GetFatigue(m_pSession->GetUserJob(), m_pSession->GetLevel());
			if( m_pSession->GetFatigue() >= nMax )	
			{
				m_pSession->SendFatigueResult(m_pSession->GetSessionID(), ERROR_ITEM_FAIL);
				break;
			}
			
			switch (pItemData->nTypeParam[0])
			{
			case 0:	// 소모성 타입
				{
					if (DeleteItemByUse(pPacket->cInvenType, pPacket->cInvenIndex, pPacket->biInvenSerial) == false){
						m_pSession->SendFatigueResult(m_pSession->GetSessionID(), ERROR_ITEM_FAIL);
						break;
					}
				}
				break;

			case 1:	// 비소모성 타입
				{
				}
				break;
			}

			if (pItemData->nTypeParam[1] > 0){	// 최대사용 레벨 제한
				if (m_pSession->GetLevel() >= pItemData->nTypeParam[1]){
					m_pSession->SendFatigueResult(m_pSession->GetSessionID(), ERROR_ITEM_FAIL);
					break;
				}
			}

			m_pSession->IncreaseFatigue(nMax - m_pSession->GetFatigue());
		}
		break;

#if defined(_GAMESERVER)
	case ITEMTYPE_IMMEDIATEREBIRTH:
		{
			if (m_pSession->GetActorHandle()->IsDie() == false)
				return;

			if( DeleteItemByUse(pPacket->cInvenType, pPacket->cInvenIndex, pPacket->biInvenSerial) == false ) 
				break;

			pRoom->OnRebirth(m_pSession->GetActorHandle());
			bUseItem = true;
			break;
		}
#endif
	case ITEMTYPE_PERIOD_APPELLATION:		//EffectItem으로 변경시켜줘야함
		{
			int nAppellationArray = pItemData->nTypeParam[0] - 1;
			if( m_pSession->GetAppellation()->IsExistAppellation( nAppellationArray ) ) {
				m_pSession->SendAppellationGainResult( nAppellationArray, ERROR_ITEM_APPELLATIONGAIN_EXIST );
				break;
			}

			TItem NewItem = {0,};
			MakeItemStruct(pItemData->nItemID, NewItem, pItemData->nTypeParam[1], 0);

			if (!DeleteItemByUse(pPacket->cInvenType, pPacket->cInvenIndex, pPacket->biInvenSerial)) break;
			if( m_pSession->GetEffectRepository()->Add( pPacket->biInvenSerial, pItemData->nItemID, NewItem.tExpireDate ))
			{
				m_pSession->GetDBConnection()->QueryAddEffectItems(m_pSession, pItemData->nItemID, DBDNWorldDef::EffectItemLifeSpanType::New, DBDNWorldDef::EffectItemGetCode::Cash, 0, 0, 
					NewItem.bEternity, NewItem.nLifespan, NewItem.nSerial, 0);

				m_pSession->GetAppellation()->RequestAddAppellation( nAppellationArray );
				m_pSession->GetAppellation()->SendPeriodAppellationTime(nAppellationArray, NewItem.tExpireDate, false);
				bUseItem = true;
			}
		}
		break;
	case ITEMTYPE_REBIRTH_COIN:
	case ITEMTYPE_REBIRTH_COIN_EX:
		{
			//부활서증가
			int nRebirthCashCoin = pItemData->nTypeParam[0];

			if (nRebirthCashCoin <= 0)
				break;

			if (m_pSession->CheckRebirthCoin(0, nRebirthCashCoin))
			{
				if( DeleteItemByUse(pPacket->cInvenType, pPacket->cInvenIndex, pPacket->biInvenSerial) == false ) 
					break;
				
				m_pSession->AddRebirthCoin(0, nRebirthCashCoin);
				m_pSession->SendIncreaseLife(IncreaseLifeType::Type::RebirthStone, nRebirthCashCoin);

				bUseItem = true;
			}
		}
		break;

	case ITEMTYPE_INCREASE_LIFE:
		{
			//생명석증가
			int nRebirthCoin = pItemData->nTypeParam[0];

			if (nRebirthCoin <= 0)
				break;

			if( m_pSession->CheckRebirthCoin(nRebirthCoin, 0) )
			{
				if( DeleteItemByUse(pPacket->cInvenType, pPacket->cInvenIndex, pPacket->biInvenSerial) == false ) 
					break;
				
				m_pSession->AddRebirthCoin(nRebirthCoin, 0);
				m_pSession->SendIncreaseLife(IncreaseLifeType::Type::LifeStone, nRebirthCoin);
				
				bUseItem = true;
			}
		}
		break;

#if defined( _VILLAGESERVER )
	case ITEMTYPE_EXPAND_SKILLPAGE:
		{		
			if( GetSkillPageCount() >= DualSkill::Type::MAX ) 
			{
				//이미 스킬트리 최대로 확장된 상태
				break;
			}	

			if (!DeleteItemByUse(pPacket->cInvenType, pPacket->cInvenIndex, pPacket->biInvenSerial)) break;
			if( m_pSession->GetEffectRepository()->Add( pPacket->biInvenSerial, pItemData->nItemID, 0 ))
			{
				TItem NewItem = {0,};
				MakeItemStruct(pItemData->nItemID, NewItem, 0, 0); 

				m_pSession->GetDBConnection()->QueryAddEffectItems(m_pSession, pItemData->nItemID, DBDNWorldDef::EffectItemLifeSpanType::New,
					DBDNWorldDef::EffectItemGetCode::Cash, 0, 0, NewItem.bEternity, NewItem.nLifespan, NewItem.nSerial, 0);

				m_pSession->UseExpandSkillPage();
				bUseItem = true;
			}
		}
		break;
#endif //#if defined( _VILLAGESERVER )

	case ITEMTYPE_SOURCE:
		{
			const TSkillData* pSkill = g_pDataManager->GetSkillData(pItemData->nSkillID);
			if (!pSkill || pSkill->vLevelDataList.empty())
				break;

			if (!DeleteItemByUse(pPacket->cInvenType, pPacket->cInvenIndex, pPacket->biInvenSerial))
				break;

			int nDuration = pSkill->vLevelDataList[0].nEffectClassValue1Duration / 1000 / 60;
			if (nDuration <= 0)
				break;

			TItem NewItem = {0,};
			MakeItemStruct(pItemData->nItemID, NewItem, nDuration, 0);

			CTimeSet tTimeSet;
			tTimeSet.AddSecond(nDuration * 60);
			NewItem.tExpireDate = tTimeSet.GetTimeT64_LC();

			char cLifeSpanType = DBDNWorldDef::EffectItemLifeSpanType::New;
			int nRemoveItemID = 0;
#if defined( PRE_ADD_NAMEDITEM_SYSTEM )
			TEffectSkillData* EffectSkill = GetEffectSkillItem( EffectSkillNameSpace::SearchType::ItemType, pItemData->nType );
			if( EffectSkill )
			{
#if defined (_GAMESERVER)	
				if( m_pSession->GetPlayerActor() )
				{
					CDNUserSession::RemoveEffectSkill(m_pSession->GetPlayerActor(), EffectSkill);
				}
#endif

				nRemoveItemID = EffectSkill->nItemID;
				if (!m_pSession->GetEffectRepository()->Remove( EffectSkill->nItemSerial ))
					break;

				if (pItemData->nItemID == nRemoveItemID)
				{
					cLifeSpanType = DBDNWorldDef::EffectItemLifeSpanType::Update;
					nRemoveItemID = 0;
				}
				else
					cLifeSpanType = DBDNWorldDef::EffectItemLifeSpanType::New;
			}
#else
			if (m_Source.nItemID > 0)
			{
				nRemoveItemID = m_Source.nItemID;
				if (!m_pSession->GetEffectRepository()->Remove(m_Source.nSerial))
					break;

				if (pItemData->nItemID == nRemoveItemID)
				{
					cLifeSpanType = DBDNWorldDef::EffectItemLifeSpanType::Update;
					nRemoveItemID = 0;
				}
				else
					cLifeSpanType = DBDNWorldDef::EffectItemLifeSpanType::New;
			}
#endif

			if (!m_pSession->GetEffectRepository()->Add(pPacket->biInvenSerial, pItemData->nItemID, NewItem.tExpireDate))
				break;

			m_pSession->GetDBConnection()->QueryAddEffectItems(m_pSession, pItemData->nItemID, cLifeSpanType, DBDNWorldDef::EffectItemGetCode::Item, 0, 0, 
				NewItem.bEternity, nDuration, NewItem.nSerial, nRemoveItemID);
		
			bUseItem = true;

#if defined( PRE_ADD_NAMEDITEM_SYSTEM )			
			EffectSkill = GetEffectSkillItem( EffectSkillNameSpace::SearchType::SkillID, pItemData->nSkillID );
			BroadcastEffectSkillItemData( true, pItemData->nSkillID, EffectSkillNameSpace::ShowEffectType::NONEEFFECT );
#if defined( _VILLAGESERVER )
			if( EffectSkill )
				m_pSession->SendEffectSkillItemData(m_pSession->GetSessionID(), *EffectSkill, false);
#endif
#if defined (_GAMESERVER)				
			if( EffectSkill )
			{
				CDNUserSession::ApplyEffectSkill(m_pSession->GetPlayerActor(), EffectSkill , false , m_pSession->GetGameRoom()->bIsPvPRoom() );
			}
#endif // #if defined (_GAMESERVER)
#else	//	#if defined( PRE_ADD_NAMEDITEM_SYSTEM )
			BroadcastSourceData(true);
#if defined (_GAMESERVER)
			CDNUserSession::ApplySourceEffect(m_pSession->GetPlayerActor(), GetSource());
#endif // #if defined (_GAMESERVER)
#endif	//	#if defined( PRE_ADD_NAMEDITEM_SYSTEM )
		}
		break;

#if defined (PRE_ADD_NAMEDITEM_SYSTEM)
	case ITEMTYPE_GLOBAL_PARTY_BUFF:
		{
			time_t CurTime;	
			time(&CurTime);
#if defined( PRE_FIX_BUFFITEM )
			if( !pItem->bEternity && pItem->tExpireDate < CurTime )
			{
				break;
			}
#else
			if( pItem->tExpireDate < CurTime )
			{
				break;
			}
#endif

#if defined( PRE_ITEMBUFF_COOLTIME )
			switch(pPacket->cInvenType)
			{
			case ITEMPOSITION_INVEN:
				{
					_UpdateInventoryCoolTime( pPacket->cInvenIndex );
					if (pItem->nCoolTime > 0) break;
				}
				break;;

			case ITEMPOSITION_CASHINVEN:
				{
					_UpdateCashInventoryCoolTime( pPacket->biInvenSerial );
					if (pItem->nCoolTime > 0) break;
				}
				break;
			}
#endif

			const TSkillData* pSkill = g_pDataManager->GetSkillData(pItemData->nSkillID);
			if (!pSkill || pSkill->vLevelDataList.empty())
				break;

			if( pItemData->cRank < ITEMRANK_SSS )
			{
				if (!DeleteItemByUse(pPacket->cInvenType, pPacket->cInvenIndex, pPacket->biInvenSerial))
					break;
			}

			int nDuration = pSkill->vLevelDataList[0].nEffectClassValue1Duration / 1000 / 60;
			if (nDuration <= 0)
				break;

#if defined( PRE_FIX_BUFFITEM )
			EffectSkillNameSpace::ShowEffectType::eType eType = (EffectSkillNameSpace::ShowEffectType::eType)pItemData->nTypeParam[1];

			if( pItemData->cRank == ITEMRANK_SSS )
			{
				eType = EffectSkillNameSpace::ShowEffectType::BUFFEFFECT;
			}

			bool bBuff = false;
			if( m_pSession->GetPartyID() > 0 )
			{
				if( pItemData->nTypeParam[0] == EffectSkillNameSpace::BuffType::Partybuff )
				{
					m_pSession->ApplyPartyEffectSkillItemData( pPacket, pItemData, m_pSession->GetSessionID(), eType, true, true );
					bBuff = true;
				}				
			}

			if( !bBuff )
				ApplyPartyEffectSkillItemData( pPacket, pItemData, m_pSession->GetSessionID(), eType, true, true );
#else		
			if( m_pSession->GetPartyID() > 0 )
			{				
				m_pSession->ApplyPartyEffectSkillItemData( pPacket, pItemData, m_pSession->GetSessionID(), EffectSkillNameSpace::ShowEffectType::BUFFEFFECT, true, true );
			}
			else
			{
				ApplyPartyEffectSkillItemData( pPacket, pItemData, m_pSession->GetSessionID(), EffectSkillNameSpace::ShowEffectType::BUFFEFFECT, true, true );
			}	
#endif			
			
			if( pItemData->cRank == ITEMRANK_SSS )
			{
				_SetItemCoolTime(pItemData->nSkillID, pItemData->nMaxCoolTime);	// cooltime 세팅
				m_pSession->SendUseItem(pPacket->cInvenType, pPacket->cInvenIndex, pPacket->biInvenSerial);
			}			

			bUseItem = true;
		}
		break;
#endif // #if defined (PRE_ADD_NAMEDITEM_SYSTEM)

	case ITEMTYPE_PET_EXP :
		{
			// 펫 소환, 펫 경험치 최대치 검사
			if (!GetPetEquip()) 
				break;

			// Check Max Level : 
			TPetLevelDetail *pPetData = g_pDataManager->GetPetLevelDetail(GetPetBodyItemID(), GetPetExp());
			if( NULL == pPetData ) break;
			if( pPetData->nPetExp == 0 )
				break; // Max Lv 인 경우엔 경험치 값이 0 임.

			if( DeleteItemByUse(pPacket->cInvenType, pPacket->cInvenIndex, pPacket->biInvenSerial) == false ) break;
			UpdatePetExp(pItemData->nTypeParam[0]);			
			bUseItem = true;
		}
		break;
	case ITEMTYPE_PET_FOOD :
		{
			// 펫 소환 검사
			if (!GetPetEquip()) 
				break;
			
			TVehicleData * pPetData =  g_pDataManager->GetVehicleData(m_PetEquip.Vehicle[Vehicle::Slot::Body].nItemID);
			if( pPetData )
			{
				TPetFoodData* pPetFoodData = g_pDataManager->GetPetFoodData(pPetData->nFoodID);
				if( pPetFoodData )
				{
					int nIncSatiety = 0;
					if( m_PetEquip.nCurrentSatiety >= pPetFoodData->nFullMaxCount )
					{
						m_pSession->SendPetFoodEat(ERROR_PETFOOD_FAIL_MAX, nIncSatiety);
						break;
					}					
								
					if( pItemData->IsCash )					
					{
						nIncSatiety = pItemData->nTypeParam[0];												
						if( m_PetEquip.nCurrentSatiety+nIncSatiety > pPetFoodData->nFullMaxCount)
							nIncSatiety = pPetFoodData->nFullMaxCount - m_PetEquip.nCurrentSatiety;						
					}
					else
					{
						if( GetPetSatietyPercent() >= 90.0f) // 90% 까지만 일반템 사용 가능..
						{
							m_pSession->SendPetFoodEat(ERROR_PETFOOD_FAIL_NORMAL, nIncSatiety);
							break;
						}						
						int nMaxSatiety = (int)(pPetFoodData->nFullMaxCount*0.9f);
						nIncSatiety = pItemData->nTypeParam[0];						
						if( nMaxSatiety < m_PetEquip.nCurrentSatiety+nIncSatiety )						
							nIncSatiety = nMaxSatiety - m_PetEquip.nCurrentSatiety;												
					}
					m_PetEquip.nCurrentSatiety += nIncSatiety;
					if( DeleteItemByUse(pPacket->cInvenType, pPacket->cInvenIndex, pPacket->biInvenSerial) == false )
						break;
					if( pItemData->IsCash)
					{
						if( pItemData->nTypeParam[1] > 0) // 경험치도 추가..
							UpdatePetExp(pItemData->nTypeParam[1]);
					}
					m_pSession->SendPetFoodEat(ERROR_NONE, nIncSatiety);
					SetPetSatiety(m_PetEquip.Vehicle[Pet::Slot::Body].nSerial, m_PetEquip.nCurrentSatiety);
					CalcPetSatietyPercent();
					bUseItem = true;
				}
			}			
		}
		break;
	case ITEMTYPE_REMOTE_WAREHOUSE:
		{
#if defined( _VILLAGESERVER )
			api_ui_OpenWareHouse( m_pSession->GetObjectID(), pItemData->nItemID );
#else
			api_ui_OpenWareHouse( m_pSession->GetGameRoom(), m_pSession->GetSessionID(), pItemData->nItemID );
#endif // #if defined( _VILLAGESERVER )
			break;
		}

	case ITEMTYPE_INVENTORY_SLOT_EX:
	case ITEMTYPE_WAREHOUSE_SLOT_EX:
		{
			int nExtendCount = pItemData->nTypeParam[0];
			if (nExtendCount <= 0)
				break;

			if (CheckEffectItemCountLimit(pItemData->nItemID, nExtendCount, false, false) != ERROR_NONE)
				break;

			if (DeleteItemByUse(pPacket->cInvenType, pPacket->cInvenIndex, pPacket->biInvenSerial) == false) 
				break;

			TItem NewItem = {0,};
			MakeCashItemStruct(0, pItemData->nItemID, NewItem);

			m_pSession->GetDBConnection()->QueryAddEffectItems(m_pSession, pItemData->nItemID, DBDNWorldDef::EffectItemLifeSpanType::New, DBDNWorldDef::EffectItemGetCode::Cash, 
				0, 0, NewItem.bEternity, NewItem.nLifespan, NewItem.nSerial, pItem->nItemID);

			m_pSession->GetEffectRepository()->Add(NewItem.nSerial, pItemData->nItemID);

			bUseItem = true;
		}
		break;

#if defined(PRE_PERIOD_INVENTORY)
	case ITEMTYPE_PERIOD_INVENTORY:
		{
			if (DeleteItemByUse(pPacket->cInvenType, pPacket->cInvenIndex, pPacket->biInvenSerial) == false) 
				break;

			TItem NewItem = {0,};
			MakeCashItemStruct(0, pItemData->nItemID, NewItem, -1, pItemData->nTypeParam[1]);

			char cLifeSpanCode = DBDNWorldDef::EffectItemLifeSpanType::New;
			if (IsEnablePeriodInventory())
				cLifeSpanCode = DBDNWorldDef::EffectItemLifeSpanType::Extend;

			m_pSession->GetDBConnection()->QueryAddEffectItems(m_pSession, pItemData->nItemID, cLifeSpanCode, DBDNWorldDef::EffectItemGetCode::Cash, 
				0, 0, NewItem.bEternity, NewItem.nLifespan, NewItem.nSerial, pItem->nItemID);

			m_pSession->GetEffectRepository()->Add(NewItem.nSerial, pItemData->nItemID, NewItem.tExpireDate);

			bUseItem = true;
		}
		break;

	case ITEMTYPE_PERIOD_WAREHOUSE:
		{
			if (DeleteItemByUse(pPacket->cInvenType, pPacket->cInvenIndex, pPacket->biInvenSerial) == false) 
				break;

			TItem NewItem = {0,};
			MakeCashItemStruct(0, pItemData->nItemID, NewItem, -1, pItemData->nTypeParam[1]);

			char cLifeSpanCode = DBDNWorldDef::EffectItemLifeSpanType::New;
			if (IsEnablePeriodWarehouse())
				cLifeSpanCode = DBDNWorldDef::EffectItemLifeSpanType::Extend;

			m_pSession->GetDBConnection()->QueryAddEffectItems(m_pSession, pItemData->nItemID, cLifeSpanCode, DBDNWorldDef::EffectItemGetCode::Cash, 
				0, 0, NewItem.bEternity, NewItem.nLifespan, NewItem.nSerial, pItem->nItemID);

			m_pSession->GetEffectRepository()->Add(NewItem.nSerial, pItemData->nItemID, NewItem.tExpireDate);

			bUseItem = true;
		}
		break;
#endif	// #if defined(PRE_PERIOD_INVENTORY)

	case ITEMTYPE_PERIOD_PLATE_EX:
		{
			int nExtendCount = pItemData->nTypeParam[0];
			if (nExtendCount <= 0)
				break;

			if (CheckEffectItemCountLimit(pItemData->nItemID, nExtendCount, false, false) != ERROR_NONE)
				break;

			if (DeleteItemByUse(pPacket->cInvenType, pPacket->cInvenIndex, pPacket->biInvenSerial) == false) 
				break;

			TItem NewItem = {0,};
			MakeCashItemStruct(0, pItemData->nItemID, NewItem, 0, pItemData->nTypeParam[1]);

			m_pSession->GetDBConnection()->QueryAddEffectItems(m_pSession, pItemData->nItemID, DBDNWorldDef::EffectItemLifeSpanType::New, DBDNWorldDef::EffectItemGetCode::Cash, 
				0, 0, NewItem.bEternity, NewItem.nLifespan, NewItem.nSerial, pItem->nItemID);

			m_pSession->GetEffectRepository()->Add(NewItem.nSerial, NewItem.nItemID, NewItem.tExpireDate);
			m_pSession->SendGlyphExpireData(GetGlyphExpireDate());

			bUseItem = true;
		}
		break;
#if defined(PRE_ADD_EXPUP_ITEM)
	case ITEMTYPE_EXPUP_ITEM :
		{
			INT64 biValue = 0;
			m_pSession->GetCommonVariableDataValue(CommonVariable::Type::EXPUPITEM_COUNT, biValue);
			// 사용 가능 횟수 체크
			int iDailyUseCount = static_cast<int>((CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::ExpUpItem)));
			if( biValue >= iDailyUseCount ) 
			{
				m_pSession->SendDailyLimitItemErr((int)CGlobalWeightTable::ExpUpItem); // 에러 메시지..
				break;
			}

			int nAddExp = pItemData->nTypeParam[0];
			if( nAddExp <= 0)
				break;

			int nLevelLimit = (int)CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::PlayerLevelLimit );
			if( m_pSession->GetLevel() >= nLevelLimit )
				break;
			if( DeleteItemByUse(pPacket->cInvenType, pPacket->cInvenIndex, pPacket->biInvenSerial) == false ) break;

			m_pSession->ChangeExp(nAddExp, DBDNWorldDef::CharacterExpChangeCode::Item, pItemData->nItemID, true);
			
			m_pSession->ModCommonVariableData( CommonVariable::Type::EXPUPITEM_COUNT, ++biValue );
			bUseItem = true;
		}
		break;
#endif
#if defined( PRE_ADD_TOTAL_LEVEL_SKILL )
	case ITEMTYPE_TOTALSKILLLEVEL_SLOT:
		{
			int nExtendCount = pItemData->nTypeParam[0];
			if (nExtendCount <= 0)
			{
				nExtendCount = 1;
			}

			if (CheckEffectItemCountLimit(pItemData->nItemID, nExtendCount, false, false) != ERROR_NONE)
				break;

			if (DeleteItemByUse(pPacket->cInvenType, pPacket->cInvenIndex, pPacket->biInvenSerial) == false) 
				break;

			TItem NewItem = {0,};
			MakeCashItemStruct(0, pItemData->nItemID, NewItem, 0, pItemData->nTypeParam[2]);

			m_pSession->GetDBConnection()->QueryAddEffectItems(m_pSession, pItemData->nItemID, DBDNWorldDef::EffectItemLifeSpanType::New, DBDNWorldDef::EffectItemGetCode::Cash, 
				0, 0, NewItem.bEternity, NewItem.nLifespan, NewItem.nSerial, pItem->nItemID);

			m_pSession->GetEffectRepository()->Add(NewItem.nSerial, NewItem.nItemID, NewItem.tExpireDate);

			bUseItem = true;
		}
		break;
#endif

#if defined(PRE_ADD_TRANSFORM_POTION)
#if defined(_VILLAGESERVER) //빌리지 서버에서만 사용할 수 있음.
	case ITEMTYPE_TRANSFORMPOTION :
	case ITEMTYPE_HEAD_SCALE_POTION:
		{	
			int nExpireTime = pItemData->nTypeParam[1];
			if( nExpireTime <= 0)
				break;

			if (DeleteItemByUse(pPacket->cInvenType, pPacket->cInvenIndex, pPacket->biInvenSerial) == false) 
				break;			
			m_pSession->GetParamData()->nIndex = nExpireTime;
			m_pSession->SetTransformID(pItemData->nItemID, nExpireTime);
			m_pSession->SendUserLocalMessage(0, FM_CHANGE_TRANSFORM);
	
			bUseItem = true;
		}
		break;
#endif //#if defined(_VILLAGESERVER)
#endif //#if defined(PRE_ADD_TRANSFORM_POTION)
#if defined(PRE_ADD_REMOTE_QUEST)
	case ITEMTYPE_ADD_QUEST:
		{
			CDNQuest* pQuest = g_pQuestManager->GetQuest(pItemData->nTypeParam[0]);
			if(!pQuest)
			{
				m_pSession->SendQuestResult(ERROR_QUEST_NOTFOUND);
				return;
			}
			int nQuestID = pItemData->nTypeParam[0];

			if(m_pSession->GetQuest()->IsClearQuest(nQuestID))
			{
				m_pSession->SendQuestResult(ERROR_QUEST_CANT_ADD_QUEST_FROM_ITEM);
				return;
			}

			if( m_pSession->GetQuest()->AddQuest(nQuestID, true) < 0 ) break;
			m_pSession->GetQuest()->SetQuestStepAndJournalStep(nQuestID, 1, 1, false);			
#if defined(_VILLAGESERVER)
			pQuest->OnRemoteStart(m_pSession->GetObjectID());
#elif defined(_GAMESERVER)
			pQuest->OnRemoteStart(m_pSession->GetGameRoom(), m_pSession->GetSessionID());
#endif
			if (!DeleteItemByUse(pPacket->cInvenType, pPacket->cInvenIndex, pPacket->biInvenSerial)) break;
			bUseItem = true;
		}
		break;
#endif //#if defined(PRE_ADD_REMOTE_QUEST)
#if defined(PRE_ADD_PVP_EXPUP_ITEM)
	case ITEMTYPE_PVPEXPUP:
		{
			CDNDBConnection * pDBCon = m_pSession->GetDBConnection();
			if( !pDBCon )
				break;

			INT64 biValue = 0;
			m_pSession->GetCommonVariableDataValue(CommonVariable::Type::PvPExpupItem_UseCount, biValue);

			TPvPGroup * pPvPData = m_pSession->GetPvPData();
			// 사용 가능 횟수 체크
			int iDailyUseCount = static_cast<int>((CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::PvPExpUpItem)));
			if( biValue >= iDailyUseCount ) 
			{
				m_pSession->SendUsePvPExpupItem(ERROR_CANT_USE_PVPEXPUPITEM_DAILY, pPvPData->cLevel, pPvPData->uiXP, 0);
				break;
			}
			// 사용 가능 계급 체크
			if( pItemData->nTypeParam[1] <= pPvPData->cLevel )
			{
				m_pSession->SendUsePvPExpupItem(ERROR_CANT_USE_PVPEXPUPITEM_LEVEL, pPvPData->cLevel, pPvPData->uiXP, 0);
				break;
			}

			if( DeleteItemByUse(pPacket->cInvenType, pPacket->cInvenIndex, pPacket->biInvenSerial) == false ) break;

			pPvPData->uiXP += pItemData->nTypeParam[0];

			for( UINT i = pPvPData->cLevel ; i < PvPCommon::Common::MaxRank ; ++ i )
			{
				const TPvPRankTable* pPvPRankTable = g_pDataManager->GetPvPRankTable( i );
				if( !pPvPRankTable )
					break;

#ifdef PRE_MOD_PVPRANK
				//치트쪽 처리해야함
				if (pPvPRankTable->cType != PvPCommon::RankTable::ExpValue)
					continue;

				if( pPvPData->uiXP >= pPvPRankTable->uiXP )
#if defined(PRE_ADD_PVPLEVEL_MISSION)
					m_pSession->SetPvPLevel(static_cast<BYTE>(i+1));
#else
					pPvPData->cLevel = static_cast<BYTE>(i+1);
#endif
				else
					break;
#else		//#ifdef PRE_MOD_PVPRANK
				if( pPvPData->uiXP >= pPvPRankTable->uiXP )
#if defined(PRE_ADD_PVPLEVEL_MISSION)
					m_pSession->SetPvPLevel(static_cast<BYTE>(i+1));
#else
					pPvPData->cLevel = static_cast<BYTE>(i+1);
#endif
				else
					break;
#endif		//#ifdef PRE_MOD_PVPRANK
			}

			pDBCon->QueryUpdatePvPData( m_pSession->GetDBThreadID(), m_pSession, PvPCommon::QueryUpdatePvPDataType::UseExpupItem );

			m_pSession->ModCommonVariableData( CommonVariable::Type::PvPExpupItem_UseCount, ++biValue );
			m_pSession->SendUsePvPExpupItem(ERROR_NONE, pPvPData->cLevel, pPvPData->uiXP, pItemData->nTypeParam[0]);

			bUseItem = true;
		}
		break;
#endif // #if defined(PRE_ADD_PVP_EXPUP_ITEM)
#if defined(PRE_ADD_TALISMAN_SYSTEM)
	case ITEMTYPE_PERIOD_TALISMAN_EX:
		{
			int nExtendCount = pItemData->nTypeParam[0];
			if (nExtendCount <= 0)
				break;

			if (CheckEffectItemCountLimit(pItemData->nItemID, nExtendCount, false, false) != ERROR_NONE)
				break;

			if (DeleteItemByUse(pPacket->cInvenType, pPacket->cInvenIndex, pPacket->biInvenSerial) == false) 
				break;

			TItem NewItem = {0,};
			MakeCashItemStruct(0, pItemData->nItemID, NewItem, 0, pItemData->nTypeParam[1]);

			m_pSession->GetDBConnection()->QueryAddEffectItems(m_pSession, pItemData->nItemID, DBDNWorldDef::EffectItemLifeSpanType::New, DBDNWorldDef::EffectItemGetCode::Cash, 
				0, 0, NewItem.bEternity, NewItem.nLifespan, NewItem.nSerial, pItem->nItemID);

			m_pSession->GetEffectRepository()->Add(NewItem.nSerial, NewItem.nItemID, NewItem.tExpireDate);
			m_pSession->SendTalismanExpireData(IsTalismanCashSlotEntend(), GetTalismanExpireDate());

			bUseItem = true;
		}
		break;
#endif
#if defined( PRE_ADD_NEW_MONEY_SEED )
	case ITEMTYPE_NEWMONEY_SEED:
		{	
			int nMaxSeedPoint = (int)CGlobalWeightIntTable::GetInstance().GetValue(CGlobalWeightIntTable::MaxSeedPoint);

			if( nMaxSeedPoint < m_pSession->GetSeedPoint() + pItemData->nTypeParam[0] )
				break;
			
			if (DeleteItemByUse(pPacket->cInvenType, pPacket->cInvenIndex, pPacket->biInvenSerial) == false) 
				break;			
			
			m_pSession->AddEtcPoint(DBDNWorldDef::EtcPointCode::SeedPoint, pItemData->nTypeParam[0]);
			
			bUseItem = true;
		}
		break;
#endif
	default:
		break;
	}

	if( bUseItem ) {
		m_pSession->GetEventSystem()->OnEvent( EventSystem::OnItemUse, 1, EventSystem::ItemID, pItemData->nItemID );
	}
#if defined( PRE_ADD_STAGE_USECOUNT_ITEM )
	if( bUseItem && nUseLimitItem > 0 )
	{
		DelUseLimitItemCount(pItemData->nItemID);
	}
#endif
}

void CDNUserItem::OnRecvGetListRepurchaseItem( const TAGetListRepurchaseItem* pPacket )
{
	m_mRepurchaseItemInfo.clear();
	for( int i=0 ; i<pPacket->cCount ; ++i )
		m_mRepurchaseItemInfo.insert( std::make_pair(pPacket->ItemList[i].iRepurchaseID,pPacket->ItemList[i]) );
	m_pSession->SendShopRepurchaseList( pPacket->nRetCode, pPacket->cCount, pPacket->ItemList );

#if defined( _WORK )
	WCHAR wszBuf[MAX_PATH];
	wsprintf( wszBuf, L"[Shop] RepurchaseList Count=%d", m_mRepurchaseItemInfo.size() );
	m_pSession->SendDebugChat( wszBuf );

	for( std::map<int,TRepurchaseItemInfo>::iterator itor=m_mRepurchaseItemInfo.begin() ;  itor!=m_mRepurchaseItemInfo.end() ; ++itor )
	{
		wsprintf( wszBuf, L"[Shop] RepurchaseID:%d ItemID:%d ItemCount:%d SellPrice:%d", (*itor).second.iRepurchaseID, (*itor).second.Item.nItemID, (*itor).second.Item.wCount, (*itor).second.iSellPrice );
		m_pSession->SendDebugChat( wszBuf );
	}
#endif // #if defined( _WORK )
}

int CDNUserItem::UnsealItem(int nInvenIndex, INT64 biInvenSerial)
{
	if (!m_pSession->IsNoneWindowState()) return ERROR_ITEM_FAIL;	// 다른 작업중에는 봉인 안되게 막는다
	if (!IsValidInventorySlot(nInvenIndex, biInvenSerial)) return ERROR_ITEM_INDEX_UNMATCH;	// 인덱스가 맞지않다

	const TItem *pItem = GetInventory(nInvenIndex);
	if (!pItem) return ERROR_ITEM_NOTFOUND;

	TItemData *pItemData = g_pDataManager->GetItemData(pItem->nItemID);
	if (pItemData == NULL || pItem == NULL)
		return ERROR_ITEM_NOTFOUND;

	if (pItem->bSoulbound == true)
	{
		_DANGER_POINT();
		return ERROR_ITEM_UNSEAL_ALREADY;
	}

	const_cast<TItem*>(pItem)->bSoulbound = true;
	m_pSession->SendRefreshInven(nInvenIndex, pItem, false);	// 게임서버 파티스트럭처 인벤토리와 동기화를 위해 리프레쉬로 날립니다.

	m_pSession->GetDBConnection()->QueryModMaterializedItem(m_pSession, *const_cast<TItem*>(pItem), ModItem_SoulBound);

	return ERROR_NONE;
}

int CDNUserItem::SealItem(int nInvenIndex, INT64 biInvenSerial, INT64 biItemSerial)
{
	if (!m_pSession->IsNoneWindowState()) return ERROR_ITEM_FAIL;	// 다른 작업중에는 봉인 안되게 막는다

	if (!IsValidInventorySlot(nInvenIndex, biInvenSerial)) return ERROR_ITEM_INDEX_UNMATCH;	// 인덱스가 맞지않다

	const TItem *pItem = GetInventory(nInvenIndex);
	if (!pItem) return ERROR_ITEM_NOTFOUND;

	TItemData *pItemData = g_pDataManager->GetItemData(pItem->nItemID);
	if (pItemData == NULL || pItem == NULL)
		return ERROR_ITEM_NOTFOUND;

	// 사용하려는 캐시 인장 아이템이 어느 타입인지 확인.
	const TItem *pSealItem = GetCashInventory(biItemSerial);
	if (!pSealItem) pSealItem = GetInventory((int)biItemSerial);
	if (!pSealItem) return ERROR_ITEM_NOTFOUND;
	TItemData *pSealItemData = g_pDataManager->GetItemData(pSealItem->nItemID);
	if (!pSealItemData) return ERROR_ITEM_NOTFOUND;
	if (pSealItemData->nType != ITEMTYPE_SEAL) return ERROR_ITEM_NOTFOUND;

	// 아이템의 정보를 보고 필요 인장개수를 구해와
	int nTotalSealNeeds = g_pDataManager->GetTotalSealNeeds(pSealItemData->nTypeParam[0], pItemData->nSealID, pItem->cLevel);
	nTotalSealNeeds *= pItem->wCount;

	// 인장이 모자른지 확인
	int nTotalCount = GetCashItemCountByType(ITEMTYPE_SEAL, pSealItemData->nTypeParam[0]);
	nTotalCount += GetInventoryItemCountByType(ITEMTYPE_SEAL, pSealItemData->nTypeParam[0]);
	if (nTotalCount < nTotalSealNeeds){
		_DANGER_POINT();
		return ERROR_ITEM_INSUFFICIENCY_ITEM;
	}

	// 봉인할 아이템 봉인여부 확인
	if (pItem->bSoulbound == false)
	{
		_DANGER_POINT();
		return ERROR_ITEM_SEAL_ALREADY;
	}

	// 봉인할 아이템 재봉인 횟수 확인
	if (pItem->cSealCount <= 0)
	{
		_DANGER_POINT();
		return ERROR_ITEM_SEALCOUNT_ZERO;
	}

	// 아이템ID로 일정개수 삭제.
	if (!UseItemByType(ITEMTYPE_SEAL, nTotalSealNeeds, DBDNWorldDef::UseItem::Use, pSealItemData->nTypeParam[0], pItem->nItemID, pItem->cLevel ))
		return ERROR_ITEM_FAIL;

	if (pSealItemData->nTypeParam[0] == 2){
		CMtRandom Random;
		Random.srand( timeGetTime() );
		int nSeed = Random.rand()%100;

		if (nSeed >= g_pDataManager->GetSealSuccessRate(pSealItemData->nTypeParam[0], pItemData->nSealID, pItem->cLevel) )
			return ERROR_ITEM_SEAL_FAIL_BY_RANDOM;
	}

	// 봉인 후 재봉인 감소시키고 리프레쉬인벤.
	const_cast<TItem*>(pItem)->bSoulbound = false;
	const_cast<TItem*>(pItem)->cSealCount -= 1;
	m_pSession->SendRefreshInven(nInvenIndex, pItem, false);

	m_pSession->GetDBConnection()->QueryModMaterializedItem(m_pSession, *const_cast<TItem*>(pItem), ModItem_SoulBound|ModItem_SealCount);

	return ERROR_NONE;
}

bool CDNUserItem::EmblemCompoundComplete( int nEmblemItemID, BYTE cPlateItemSlotIndex, INT64 biPlateItemSerialID, int* apUseItemID, int* apUseItemCount, int nUseItemArrayCount )
{
	// 사용한 아이템들 제거. 플레이트면 플레이트 아이템도 삭제
	if (!DeleteInventoryBySlot( cPlateItemSlotIndex, 1, biPlateItemSerialID, DBDNWorldDef::UseItem::Use ))
		return false;

	for( int nItem = 0; nItem < nUseItemArrayCount; ++nItem )
	{
		if (!DeleteInventoryByItemID( apUseItemID[ nItem ], apUseItemCount[ nItem ], DBDNWorldDef::UseItem::Use ))
			return false;
	}

	// 성공시 문장 아이템, 실패시 결과물 아이템 존재한다면 생성해서 인벤에 추가
	if( 0 != nEmblemItemID )
	{
		if (CreateInvenItem1( nEmblemItemID, 1, -1, -1, DBDNWorldDef::AddMaterializedItem::ItemCompound, m_pSession->m_nClickedNpcID ) != ERROR_NONE)
			return false;
	}

	return true;
}

bool CDNUserItem::ItemCompoundComplete( int nResultItemID, char cResultItemOptionIndex, int* apUseItemID, int* apUseItemCount, int nUseItemArrayCount, INT64 biResultItemIsNeedItem, std::vector<TSaveItemInfo> *pVecResultList/* = NULL*/ )
{
	const TItem * pItem;
	int nSlotIndex;
	std::vector<std::pair<int,const TItem*>> VecModItemList;
	std::vector<TItemInfo> VecDeleteItemList;

	for( int nItem = 0; nItem < nUseItemArrayCount; ++nItem )
	{
		bool bDeleteFailed = false;
		if( biResultItemIsNeedItem && nResultItemID == apUseItemID[nItem] )
		{
			nSlotIndex = FindInventorySlotBySerial( biResultItemIsNeedItem );
			pItem = GetInventory( nSlotIndex );
			if( pItem )
			{
				VecModItemList.push_back(std::make_pair(nSlotIndex, pItem));
				bDeleteFailed = true;
			}
			else
				g_Log.Log(LogType::_ERROR, m_pSession, L"[CDBID:%I64d] ItemCompoundComplete GetInventory Fail(ItemID:%d)\r\n", m_pSession->GetCharacterDBID(), apUseItemID[ nItem ]);
		}
		else
		{
			bDeleteFailed = DeleteInventoryExByItemID( apUseItemID[ nItem ], -1, apUseItemCount[ nItem ], DBDNWorldDef::UseItem::Use, 0, VecDeleteItemList, true );
		}

		if( !bDeleteFailed ) {
			// 아이템 삭제에 실패했다면 리턴시켜야 합니다.
			g_Log.Log(LogType::_ERROR, m_pSession, L"[CDBID:%I64d] ItemCompoundComplete DeleteInventoryExByItemID Fail(ItemID:%d)\r\n", m_pSession->GetCharacterDBID(), apUseItemID[ nItem ]);
			return false;
		}
	}

	// 아이템 조합 생성해서 인벤에 추가. 실패시 결과물 아이템 존재한다면 인벤에 추가.
	if( 0 != nResultItemID )
	{
		if (VecModItemList.empty() == false)
		{
			std::vector<std::pair<int,const TItem*>>::iterator ii;
			for (ii = VecModItemList.begin(); ii != VecModItemList.end(); ii++)
			{
				if( cResultItemOptionIndex > 0 )
					const_cast<TItem*>((*ii).second)->cOption = cResultItemOptionIndex;

#ifdef PRE_FIX_60525
				int nModFieldBitmap = ModItem_Option;

				if (ii->second->bSoulbound == false)
				{
					//#60525
					//봉인된 아이템 봉인 해제...
					const_cast<TItem*>((*ii).second)->bSoulbound = true;

					nModFieldBitmap |= ModItem_SoulBound;
				}

				m_pSession->SendRefreshInven((*ii).first, (*ii).second, false);	// 게임서버 파티스트럭처 인벤토리와 동기화를 위해 리프레쉬로 날립니다.
				m_pSession->GetDBConnection()->QueryModMaterializedItem(m_pSession, *const_cast<TItem*>((*ii).second), nModFieldBitmap);
#else
				m_pSession->SendRefreshInven((*ii).first, (*ii).second, false);	// 게임서버 파티스트럭처 인벤토리와 동기화를 위해 리프레쉬로 날립니다.
				m_pSession->GetDBConnection()->QueryModMaterializedItem(m_pSession, *const_cast<TItem*>((*ii).second), ModItem_Option);

#endif // PRE_FIX_60525

				if (pVecResultList)
				{
					TSaveItemInfo SaveItem;
					SaveItem.SetInfo((*ii).first, *(*ii).second, false);

					pVecResultList->push_back(SaveItem);
				}
			}
		}
		else
		{
			TItem CreateItem = {0,};
			std::vector<TSaveItemInfo> VecItemList;
			MakeItemStruct( nResultItemID, CreateItem );
			if( cResultItemOptionIndex > 0 )
				CreateItem.cOption = cResultItemOptionIndex;
			CreateItem.wCount = 1;

			if (_CreateInvenItemEx( CreateItem, DBDNWorldDef::AddMaterializedItem::ItemCompound, m_pSession->m_nClickedNpcID, ( pVecResultList ) ? *pVecResultList : VecItemList, ( pVecResultList ) ? true : false, CREATEINVEN_ETC ) != ERROR_NONE)
				return false;
		}

		m_pSession->GetEventSystem()->OnEvent( EventSystem::OnItemCompound, 1, EventSystem::ItemID, nResultItemID );
	}
	return true;
}


//-------------------------------------------------------------------------------------------------------------------------------------------------

const TItem* CDNUserItem::GetEquip(int nIndex) const
{
	if (!_CheckRangeEquipIndex(nIndex)) return NULL;
	if (m_Equip[nIndex].nItemID <= 0) return NULL;

	return &m_Equip[nIndex];
}

const TItem *CDNUserItem::GetGlyph(int nIndex) const
{
	if (!_CheckRangeGlyphIndex(nIndex)) return NULL;
	if (m_Glyph[nIndex].nItemID <= 0) return NULL;

	return &m_Glyph[nIndex];
}

const TItem* CDNUserItem::GetInventory(int nIndex) const
{
	if (!_CheckRangeInventoryIndex(nIndex)) return NULL;
	if (m_Inventory[nIndex].nItemID <= 0) return NULL;

	return &(m_Inventory[nIndex]);
}

const TItem* CDNUserItem::GetWarehouse(int nIndex) const
{
	if (!_CheckRangeWarehouseIndex(nIndex)) return NULL;
	if (m_Warehouse[nIndex].nItemID <= 0) return NULL;

	return &(m_Warehouse[nIndex]);
}

const TQuestItem* CDNUserItem::GetQuestInventory(int nIndex) const
{
	if (!_CheckRangeQuestInventoryIndex(nIndex)) return NULL;
	if (m_QuestInventory[nIndex].nItemID <= 0) return NULL;

	return &(m_QuestInventory[nIndex]);
}

const TItem* CDNUserItem::GetCashEquip(int nIndex) const
{
	if (!_CheckRangeCashEquipIndex(nIndex)) return NULL;
	if (m_CashEquip[nIndex].nItemID <= 0) return NULL;

	return &m_CashEquip[nIndex];
}

const TItem* CDNUserItem::GetCashInventory(INT64 biItemSerial) const
{
	if (biItemSerial <= 0) return NULL;
	if (m_MapCashInventory.empty()) return NULL;

	TMapItem::const_iterator iter = m_MapCashInventory.find(biItemSerial);
	if (iter == m_MapCashInventory.end()) return NULL;

	return &(iter->second);
}

const TItem *CDNUserItem::GetVehiclePartsEquip(int nIndex) const
{
	if ((nIndex < 0) || (nIndex >= Vehicle::Slot::Max)) return NULL;
	if (m_VehicleEquip.Vehicle[nIndex].nItemID <= 0) return NULL;

	return &(m_VehicleEquip.Vehicle[nIndex]);
}

TVehicle *CDNUserItem::GetVehicleEquip()
{
	if (m_VehicleEquip.Vehicle[Vehicle::Slot::Body].nItemID <= 0) return NULL;
	return &(m_VehicleEquip);
}

const TItem *CDNUserItem::GetPetPartsEquip(int nIndex) const
{
	if ((nIndex < 0) || (nIndex >= Pet::Slot::Max)) return NULL;
	if (m_PetEquip.Vehicle[nIndex].nItemID <= 0) return NULL;

	return &(m_PetEquip.Vehicle[nIndex]);
}

const TVehicle *CDNUserItem::GetPetEquip() const
{
	if (m_PetEquip.Vehicle[Pet::Slot::Body].nItemID <= 0) return NULL;
	return &(m_PetEquip);
}

const TItem *CDNUserItem::GetVehicleBodyInventory(INT64 biItemSerial) const
{
	if (biItemSerial <= 0) return NULL;
	if (m_MapVehicleInventory.empty()) return NULL;

	TMapVehicle::const_iterator iter = m_MapVehicleInventory.find(biItemSerial);
	if (iter == m_MapVehicleInventory.end()) return NULL;

	return &(iter->second.Vehicle[Vehicle::Slot::Body]);
}

const TVehicle *CDNUserItem::GetVehicleInventory(INT64 biItemSerial) const
{
	if (biItemSerial <= 0) return NULL;
	if (m_MapVehicleInventory.empty()) return NULL;

	TMapVehicle::const_iterator iter = m_MapVehicleInventory.find(biItemSerial);
	if (iter == m_MapVehicleInventory.end()) return NULL;

	return &(iter->second);
}

int CDNUserItem::GetVehicleInventoryCount() const
{
	return static_cast<int>(m_MapVehicleInventory.size());
}

#if defined(PRE_PERIOD_INVENTORY)
void CDNUserItem::SetEnablePeriodInventory(bool bEnable, __time64_t tExpireDate)
{
	m_bEnablePeriodInventory = bEnable;
	if (m_bEnablePeriodInventory){
		if (m_tPeriodInventoryExpireDate > 0){
			__time64_t CurTime;
			time(&CurTime);

			m_tPeriodInventoryExpireDate += (tExpireDate - CurTime);
		}
		else
			m_tPeriodInventoryExpireDate = tExpireDate;
	}

	m_pSession->SendPeriodInventory(m_bEnablePeriodInventory, m_tPeriodInventoryExpireDate);
}

bool CDNUserItem::IsEnablePeriodInventory() const
{
	return m_bEnablePeriodInventory;
}

__time64_t CDNUserItem::GetPeriodInventoryExpireDate() const
{
	return m_tPeriodInventoryExpireDate;
}

void CDNUserItem::SetEnablePeriodWarehouse(bool bEnable, __time64_t tExpireDate)
{
	m_bEnablePeriodWarehouse = bEnable;
	if (m_bEnablePeriodWarehouse){
		if (m_tPeriodWarehouseExpireDate > 0){
			__time64_t CurTime;
			time(&CurTime);

			m_tPeriodWarehouseExpireDate += (tExpireDate - CurTime);
		}
		else
			m_tPeriodWarehouseExpireDate = tExpireDate;
	}

	m_pSession->SendPeriodWarehouse(m_bEnablePeriodWarehouse, m_tPeriodWarehouseExpireDate);
}

bool CDNUserItem::IsEnablePeriodWarehouse() const
{
	return m_bEnablePeriodWarehouse;
}

__time64_t CDNUserItem::GetPeriodWarehouseExpireDate() const
{
	return m_tPeriodWarehouseExpireDate;
}

#endif	// #if defined(PRE_PERIOD_INVENTORY)

#if defined(PRE_ADD_SERVER_WAREHOUSE)
const TItem* CDNUserItem::GetServerWare(INT64 biItemSerial) const
{
	if (biItemSerial <= 0) return NULL;
	if (m_MapServerWarehouse.empty()) return NULL;

	TMapItem::const_iterator iter = m_MapServerWarehouse.find(biItemSerial);
	if (iter == m_MapServerWarehouse.end()) return NULL;

	return &(iter->second);
}

const TItem* CDNUserItem::GetServerWareCash(INT64 biItemSerial) const
{
	if (biItemSerial <= 0) return NULL;
	if (m_MapServerWarehouseCash.empty()) return NULL;

	TMapItem::const_iterator iter = m_MapServerWarehouseCash.find(biItemSerial);
	if (iter == m_MapServerWarehouseCash.end()) return NULL;

	return &(iter->second);
}
#endif

#if defined(PRE_ADD_TALISMAN_SYSTEM)
const TItem* CDNUserItem::GetTalisman(int nIndex) const
{
	if (!_CheckRangeTalismanIndex(nIndex)) return NULL;
	if (m_Talisman[nIndex].nItemID <= 0) return NULL;

	return &m_Talisman[nIndex];
}
#endif

// Equip
void CDNUserItem::GetEquipIDs(int *EquipArray) const
{
	for (int i = 0; i < EQUIPMAX; i++){
		EquipArray[i] = m_Equip[i].nItemID;
	}
}

void CDNUserItem::ChangeEquipItemDurability( int nIndex, USHORT wDur )
{
	const TItem *pItem = GetEquip( nIndex );
	if( !pItem ) return;

	if( g_pDataManager->GetItemDurability( pItem->nItemID ) == 0 ) return;
	SetEquipItemDurability(nIndex, wDur);

	m_pSession->SendRefreshEquip( nIndex, pItem );
}

bool CDNUserItem::CheckEquipByItemID(int nItemID) const
{
	if (nItemID <= 0) return false;

	TItemData *pItemData = g_pDataManager->GetItemData(nItemID);
	if (!pItemData) {
		return false;
	}

	// 장착가능 여부와 상관없이 존재여부 검사 ?

	for (int nIndex = 0 ; EQUIPMAX > nIndex ; ++nIndex) {
		const TItem *pEquip = &m_Equip[nIndex];	// 실 db쪽 데이터
		if (0 == pEquip->nItemID) {
			continue;
		}

		if (pEquip->nItemID == nItemID) {
			return true;
		}
	}

	return false;
}

#if defined(PRE_ADD_EQUIPED_ITEM_ENCHANT) || defined(PRE_ADD_EQUIPLOCK)
bool CDNUserItem::IsValidEquipSlot(int nIndex, INT64 biSerial, bool bCheckSerial /*= true*/) const
{
	if( !_CheckRangeEquipIndex(nIndex) ) return false;
	if( m_Equip[nIndex].nItemID <= 0 ) return false;
	if( bCheckSerial && (m_Equip[nIndex].nSerial != biSerial)) return false;

	return true;
}
#endif	// #if defined(PRE_ADD_EQUIPED_ITEM_ENCHANT) || defined(PRE_ADD_EQUIPLOCK)

void CDNUserItem::GetEquipItemDurability(std::vector<INT64> &VecSerialList, std::vector<USHORT> &VecDurList) const
{
	for (int i = 0; i < EQUIPMAX; i++){
		if (m_Equip[i].nItemID <= 0) continue;
		VecSerialList.push_back(m_Equip[i].nSerial);
		VecDurList.push_back(m_Equip[i].wDur);
	}
}

bool CDNUserItem::ChangeDefaultParts( int nItemType, const int *pTypeParam, INT64 biUseInvenItemSerial )
{
	const TItem *pItem = NULL;
	char cEquipIndex;
	switch( nItemType ) {
		case ITEMTYPE_FACIAL: pItem = GetEquip(EQUIP_FACE); cEquipIndex = EQUIP_FACE; break;
		case ITEMTYPE_HAIRDRESS: pItem = GetEquip(EQUIP_HAIR); cEquipIndex = EQUIP_HAIR; break;
	}
	if( !pItem ) return false;
	if( pItem->nItemID == pTypeParam[0] ) return false;
	const_cast<TItem*>(pItem)->nItemID = pTypeParam[0];
	INT64 biBeforeSerial = pItem->nSerial;
	const_cast<TItem*>(pItem)->nSerial = MakeCashItemSerial(5 * 365);	// 무한?
	m_pSession->SendRefreshEquip( cEquipIndex, pItem );
	BroadcastChangeEquip(cEquipIndex, *const_cast<TItem*>(pItem) );

	// 여기서 디비 저장을 해 보아요.
	m_pSession->GetDBConnection()->QueryUseItem(m_pSession, DBDNWorldDef::UseItem::Destroy, biBeforeSerial, pItem->wCount, true);

	DBSendAddMaterializedItem(cEquipIndex, DBDNWorldDef::AddMaterializedItem::FixedItem, biUseInvenItemSerial, *const_cast<TItem*>(pItem), 0, 0, (INT64)0, DBDNWorldDef::ItemLocation::Equip, 
		DBDNWorldDef::PayMethodCode::Cash, false, 0);
	
	return true;
}

#if defined(PRE_ADD_TALISMAN_SYSTEM)
void CDNUserItem::GetEquipList(char &cCount, char &cCashCount, char &cGlyphCount, char &cTalismanCount, TItemInfo *EquipArray) const
#else
void CDNUserItem::GetEquipList(char &cCount, char &cCashCount, char &cGlyphCount, TItemInfo *EquipArray) const
#endif
{
	TItemInfo *pInfo = &EquipArray[0];
	// Equip Info
	cCount = 0;
	for (int i = 0; i < EQUIPMAX; i++){
		if (m_Equip[i].nItemID <= 0) continue;
		pInfo->cSlotIndex = i;
		pInfo->Item = m_Equip[i];
		cCount++;
		pInfo++;
	}

	// Cash Equip Info
	cCashCount = 0;
	for (int i = 0; i < CASHEQUIPMAX; i++){
		if (m_CashEquip[i].nItemID <= 0) continue;

		pInfo->cSlotIndex = i;
		pInfo->Item = m_CashEquip[i];
		cCashCount++;
		pInfo++;
	}

	// Glyph
	cGlyphCount = 0;
	for (int i = 0; i < GLYPHMAX; i++){
		if (m_Glyph[i].nItemID <= 0) continue;
		pInfo->cSlotIndex = i;
		pInfo->Item = m_Glyph[i];
		cGlyphCount++;
		pInfo++;
	}

#if defined(PRE_ADD_TALISMAN_SYSTEM)
	// Talisman
	cTalismanCount = 0;
	for (int i = 0; i < TALISMAN_MAX; i++){
		if (m_Talisman[i].nItemID <= 0) continue;
		pInfo->cSlotIndex = i;
		pInfo->Item = m_Talisman[i];
		cTalismanCount++;
		pInfo++;
	}
#endif
}

bool CDNUserItem::IsEquipCashItemExist(int nItemID) const
{
	DN_ASSERT(0 != nItemID,	"Invalid!");

	TItemData *pItemData = g_pDataManager->GetItemData(nItemID);
	if (!pItemData) {
		return false;
	}

	// 장착가능 여부와 상관없이 존재여부 검사 ?

	for (int nIndex = 0 ; CASHEQUIPMAX > nIndex ; ++nIndex) {
		const TItem *pEquip = &m_CashEquip[nIndex];	// 실 db쪽 데이터
		if (0 >= pEquip->nItemID) {
			continue;
		}

		if (pEquip->nItemID == nItemID) {
			return true;
		}
	}

	return false;
}

// inven
bool CDNUserItem::CheckEnoughItem( int iItemID, int iCount ) const
{
	// 아이템이 필요하지 않다면...
	if( iCount <= 0 )
		return true;

	// 아이템 여유 검사
	if( GetInventoryItemCount( iItemID ) >= iCount )
		return true;

	return false;
}

int CDNUserItem::GetInventoryItemCount(int nItemID, char cOption) const
{
	if (nItemID <= 0) return 0;

	int nCount = 0;
#if defined(PRE_PERIOD_INVENTORY)
	for (int i = 0; i < INVENTORYTOTALMAX; i++){
		if (i == GetInventoryCount()){
			if (IsEnablePeriodInventory())
				i = INVENTORYMAX;
			else
				break;
		}
#else	// #if defined(PRE_PERIOD_INVENTORY)
	for (int i = 0; i < GetInventoryCount(); i++){
#endif	// #if defined(PRE_PERIOD_INVENTORY)
		if (m_Inventory[i].nItemID == nItemID){
			if( cOption != -1 ) {
				if( m_Inventory[i].cOption != cOption ) continue;
			}
			nCount += m_Inventory[i].wCount;
		}
	}

	return nCount;
}

int CDNUserItem::GetInventoryItemListFromItemID( int nItemID, std::vector<TItem *> &pVecResult )
{
	if( nItemID <= 0 ) return 0;

#if defined(PRE_PERIOD_INVENTORY)
	for (int i = 0; i < INVENTORYTOTALMAX; i++){
		if (i == GetInventoryCount()){
			if (IsEnablePeriodInventory())
				i = INVENTORYMAX;
			else
				break;
		}
#else	// #if defined(PRE_PERIOD_INVENTORY)
	for (int i = 0; i < GetInventoryCount(); i++){
#endif	// #if defined(PRE_PERIOD_INVENTORY)
		if( m_Inventory[i].nItemID == nItemID )
			pVecResult.push_back( &m_Inventory[i] );
	}

	return(int)pVecResult.size();
}

int CDNUserItem::GetInventoryItemListFromItemID( int nItemID, char cOption, std::vector<TItem *> &pVecResult )
{
	if( nItemID <= 0 ) return 0;

	int nCount = 0;
#if defined(PRE_PERIOD_INVENTORY)
	for (int i = 0; i < INVENTORYTOTALMAX; i++){
		if (i == GetInventoryCount()){
			if (IsEnablePeriodInventory())
				i = INVENTORYMAX;
			else
				break;
		}
#else	// #if defined(PRE_PERIOD_INVENTORY)
	for (int i = 0; i < GetInventoryCount(); i++){
#endif	// #if defined(PRE_PERIOD_INVENTORY)
		if( m_Inventory[i].nItemID == nItemID ) {
			if( cOption != -1 ) {
				if( m_Inventory[i].cOption != cOption ) continue;
			}
			pVecResult.push_back( &m_Inventory[i] );
			nCount += m_Inventory[i].wCount;
		}
	}

	return nCount;
}

int CDNUserItem::GetInventoryItemCountByType(int nType, int nTypeParam1) const
{
	int nTotalCount = 0;
#if defined(PRE_PERIOD_INVENTORY)
	for (int i = 0; i < INVENTORYTOTALMAX; i++){
		if (i == GetInventoryCount()){
			if (IsEnablePeriodInventory())
				i = INVENTORYMAX;
			else
				break;
		}
#else	// #if defined(PRE_PERIOD_INVENTORY)
	for (int i = 0; i < GetInventoryCount(); i++){
#endif	// #if defined(PRE_PERIOD_INVENTORY)
		if (m_Inventory[i].nItemID <= 0) continue;
		if (nTypeParam1 != -1 && g_pDataManager->GetItemTypeParam1(m_Inventory[i].nItemID) != nTypeParam1) continue;

		if (g_pDataManager->GetItemMainType(m_Inventory[i].nItemID) == nType){
			nTotalCount += m_Inventory[i].wCount;
		}
	}

	return nTotalCount;
}
TItem* CDNUserItem::GetInventoryItemByType(int nType)
{
#if defined(PRE_PERIOD_INVENTORY)
	for (int i = 0; i < INVENTORYTOTALMAX; i++){
		if (i == GetInventoryCount()){
			if (IsEnablePeriodInventory())
				i = INVENTORYMAX;
			else
				break;
		}
#else	// #if defined(PRE_PERIOD_INVENTORY)
	for (int i = 0; i < GetInventoryCount(); i++){
#endif	// #if defined(PRE_PERIOD_INVENTORY)
		if (m_Inventory[i].nItemID <= 0) continue;

		if (g_pDataManager->GetItemMainType(m_Inventory[i].nItemID) == nType)
			return &m_Inventory[i];			
	}

	return NULL;
}

#if defined( PRE_ADD_NAMEDITEM_SYSTEM )
TItem* CDNUserItem::GetItemRank( BYTE ItemRank )
{
#if defined(PRE_PERIOD_INVENTORY)
	for (int i = 0; i < INVENTORYTOTALMAX; i++){
		if (i == GetInventoryCount()){
			if (IsEnablePeriodInventory())
				i = INVENTORYMAX;
			else
				break;
		}
#else	// #if defined(PRE_PERIOD_INVENTORY)
	for (int i = 0; i < GetInventoryCount(); i++){
#endif	// #if defined(PRE_PERIOD_INVENTORY)
		if (m_Inventory[i].nItemID <= 0) continue;

		if (g_pDataManager->GetItemRank(m_Inventory[i].nItemID) == ItemRank)
			return &m_Inventory[i];			
	}

	return NULL;
}
#endif

bool CDNUserItem::IsValidSpaceInventorySlot(int nItemID, short wCount, bool bSoulBound, char cSealCount, bool bEternity) const
{
	if ((nItemID <= 0) ||(wCount <= 0)) return false;

	TItemData *pItem = g_pDataManager->GetItemData(nItemID);
	if (!pItem) return false;
	if (pItem->nOverlapCount <= 0) return false;

	int nCount = 0;
	if (pItem->nOverlapCount == 1){		// 겹치지 않는 아이템
		nCount = FindBlankInventorySlotCount();
		if (wCount > nCount) return false;		// 빈슬롯이랑 비교하기
	}
	else {
		// 겹쳐지는 아이템(인벤에 동일 아이템들이 몇개씩 남아있어도 신경안쓰고 깔끔하게 넣는 경우만 생각하쟈)
		if (pItem->nOverlapCount < wCount){		// 한번에 겹치는 양보다 많을경우
			int nBundle = wCount / pItem->nOverlapCount;
			if ((wCount % pItem->nOverlapCount) > 0) nBundle++;

			nCount = FindBlankInventorySlotCount();
			if (nBundle > nCount) return false;		// 묶음이랑 빈슬롯이랑 비교하기
		}
		else {
			int nBlank = FindOverlapInventorySlot(nItemID, wCount, bSoulBound, cSealCount, bEternity);	// 기존 아이템중에 한방에 들어갈 아이템이 있는지 
			if (nBlank < 0){
				nBlank = FindBlankInventorySlot();
				if (nBlank < 0) return false;
			}
		}
	}

	return true;
}

// IsValidSpaceInventorySlot 와 다르게 Shop 에서 구입할때는 좀 더 인벤을 디테일하게 체크한다.
// 어쩔 수 없이 전체 아이템 스택 룰을 변경할 수 없어 상점만 통일한다.
int CDNUserItem::IsValidSpaceInventorySlotFromShop( const TItemData* pItemData, int iCheckCount ) const
{
	int iBlankCount = FindBlankInventorySlotCount();
	if( pItemData->nOverlapCount == 1 )
	{
		if( iBlankCount < iCheckCount ) 
			return ERROR_ITEM_INVENTORY_NOTENOUGH;
	}
	else
	{
		if( iBlankCount < 1 ) 
		{
			TItem ResultItem;
			if( MakeItemStruct( pItemData->nItemID, ResultItem ) == false )
				return ERROR_ITEM_NOTFOUND;
			if( FindOverlapInventorySlot( pItemData->nItemID, iCheckCount, ResultItem.bSoulbound, ResultItem.cSealCount, ResultItem.bEternity ) < 0 )
				return ERROR_ITEM_INVENTORY_NOTENOUGH;
		}
	}

	return ERROR_NONE;
}

int CDNUserItem::FindBlankInventorySlot() const
{
#if defined(PRE_PERIOD_INVENTORY)
	for (int i = 0; i < INVENTORYTOTALMAX; i++){
		if (i == GetInventoryCount()){
			if (IsEnablePeriodInventory())
				i = INVENTORYMAX;
			else
				break;
		}
#else	// #if defined(PRE_PERIOD_INVENTORY)
	for (int i = 0; i < GetInventoryCount(); i++){
#endif	// #if defined(PRE_PERIOD_INVENTORY)
		if (m_Inventory[i].nItemID == 0) return i;
	}

	return -1;
}

int CDNUserItem::FindBlankInventorySlotCount() const
{
	int nCount = 0;
#if defined(PRE_PERIOD_INVENTORY)
	for (int i = 0; i < INVENTORYTOTALMAX; i++){
		if (i == GetInventoryCount()){
			if (IsEnablePeriodInventory())
				i = INVENTORYMAX;
			else
				break;
		}
#else	// #if defined(PRE_PERIOD_INVENTORY)
	for (int i = 0; i < GetInventoryCount(); i++){
#endif	// #if defined(PRE_PERIOD_INVENTORY)
		if (m_Inventory[i].nItemID == 0) nCount++;
	}

	return nCount;
}

int CDNUserItem::FindBlankInventorySlotCountList(std::queue<int> &qSlotList) const
{
	int nCount = 0;
#if defined(PRE_PERIOD_INVENTORY)
	for (int i = 0; i < INVENTORYTOTALMAX; i++){
		if (i == GetInventoryCount()){
			if (IsEnablePeriodInventory())
				i = INVENTORYMAX;
			else
				break;
		}
#else	// #if defined(PRE_PERIOD_INVENTORY)
	for (int i = 0; i < GetInventoryCount(); i++){
#endif	// #if defined(PRE_PERIOD_INVENTORY)
		if (m_Inventory[i].nItemID == 0){
			qSlotList.push(i);
			nCount++;
		}
	}

	return nCount;
}

int CDNUserItem::FindOverlapInventorySlot(int nItemID, short wCount, bool bSoulBound, char cSealCount, bool bEternity) const
{
	if ((nItemID <= 0) || (wCount <= 0)) return -1;

	TItemData *pItemData = NULL;
#if defined(PRE_PERIOD_INVENTORY)
	for (int i = 0; i < INVENTORYTOTALMAX; i++){
		if (i == GetInventoryCount()){
			if (IsEnablePeriodInventory())
				i = INVENTORYMAX;
			else
				break;
		}
#else	// #if defined(PRE_PERIOD_INVENTORY)
	for (int i = 0; i < GetInventoryCount(); i++){
#endif	// #if defined(PRE_PERIOD_INVENTORY)
		if (m_Inventory[i].nItemID == 0) continue;	// 빈칸이면 패스
		if (m_Inventory[i].nItemID != nItemID) continue;	// 같은 아이템이 아니다
		if ( m_Inventory[i].bSoulbound != bSoulBound ) continue;
		if ( m_Inventory[i].cSealCount != cSealCount ) continue;
		if ((!m_Inventory[i].bEternity) || (!bEternity)) continue;

		pItemData = g_pDataManager->GetItemData(m_Inventory[i].nItemID);
		if (!pItemData) continue;
		if (pItemData->nOverlapCount <= 0) continue;
		if (pItemData->nOverlapCount == 1) continue;		// 겹치지 않으면 패스

		if (pItemData->nOverlapCount >= m_Inventory[i].wCount + wCount){
			return i;
		}
	}

	return -1;
}

bool CDNUserItem::IsValidInventorySlot(int nIndex, INT64 biSerial, bool bCheckSerial/* = true*/) const
{
	if (!_CheckRangeInventoryIndex(nIndex)) return false;
	if (m_Inventory[nIndex].nItemID <= 0) return false;
	if (bCheckSerial && (m_Inventory[nIndex].nSerial != biSerial)) return false;

	return true;
}

bool CDNUserItem::VerifyInventorySlotPacket(int nInvenIndex, const TItem &Item) const
{
	const TItem *pInven = GetInventory(nInvenIndex);
	if (pInven){
		if (pInven->nItemID != Item.nItemID) return false;
		if (pInven->nSerial != Item.nSerial) return false;
		int nOverlapCount = g_pDataManager->GetItemOverlapCount(Item.nItemID);
		if ((pInven->wCount + Item.wCount) > nOverlapCount) return false;
	}

	return true;
}

void CDNUserItem::ChangeInventoryItemDurability( int nIndex, USHORT wDur )
{
	const TItem *pItem = GetInventory( nIndex );
	if( !pItem ) return;

	if( g_pDataManager->GetItemDurability( pItem->nItemID ) == 0 ) return;
	SetInvenItemDurability(nIndex, wDur);

	m_pSession->SendRefreshInven( nIndex, pItem, false );
}

bool CDNUserItem::SortInventory(CSSortInventory *pPacket)
{
	// 정말 하기싫은 정렬이구나 ;ㅅ;
	TItem NewInventory[INVENTORYMAX] = { 0, };
	TItem TempInventory[INVENTORYMAX] = { 0, };
	memcpy(TempInventory, m_Inventory, sizeof(TempInventory));	// 일단 빽업을 만들쟈;

	if( pPacket->cTotalCount > INVENTORYMAX || GetInventoryCount() > INVENTORYMAX )
		return false;

	for (int i = 0; i < pPacket->cTotalCount; i++)
	{
		if (!_CheckRangeInventoryIndex(pPacket->SlotInfo[i].cCurrent)) 
			return false;
		if (!_CheckRangeInventoryIndex(pPacket->SlotInfo[i].cNew))
			return false;

		NewInventory[pPacket->SlotInfo[i].cNew] = TempInventory[pPacket->SlotInfo[i].cCurrent];
		memset(&TempInventory[pPacket->SlotInfo[i].cCurrent], 0, sizeof(TItem));
	}

	if ((NewInventory[0].nItemID <= 0) ||(NewInventory[0].nSerial <= 0)){		// 정렬했는데 첫번째칸에 값이 없으면 클라에서 뭔가 잘못 날려준거..
		_DANGER_POINT();
		return false;
	}

	int nCount = 0;
	for (int i = 0; i < INVENTORYMAX; i++){
		if (TempInventory[i].nItemID > 0) nCount++;
	}

	if (nCount > 0) return false;	// 뭔가 잘못됐다.

	memcpy(m_Inventory, NewInventory, sizeof(NewInventory));	// 다시 카피(아.. 일케 해야하는가;; ㅠㅠ)

	return true;
}

#if defined(PRE_PERIOD_INVENTORY)
bool CDNUserItem::SortPeriodInventory(CSSortInventory *pPacket)
{
	// 클라 작업하면 다시 해야함!!
	TItem NewInventory[PERIODINVENTORYMAX] = { 0, };
	TItem TempInventory[PERIODINVENTORYMAX] = { 0, };
	memcpy(TempInventory, &m_Inventory[INVENTORYMAX], sizeof(TempInventory));	// 일단 빽업을 만들쟈;

	if( pPacket->cTotalCount > PERIODINVENTORYMAX )
		return false;

	for (int i = 0; i < pPacket->cTotalCount; i++)
	{
		int nCurrentInventoryIndex = pPacket->SlotInfo[i].cCurrent;
		int nNewInventoryIndex = pPacket->SlotInfo[i].cNew + INVENTORYMAX;
		int nCurrentIndex = pPacket->SlotInfo[i].cCurrent - INVENTORYMAX;
		int nNewIndex = pPacket->SlotInfo[i].cNew;

		if (!_CheckRangePeriodInventoryIndex(nCurrentInventoryIndex)) 
			return false;
		if (!_CheckRangePeriodInventoryIndex(nNewInventoryIndex))
			return false;

		NewInventory[nNewIndex] = TempInventory[nCurrentIndex];
		memset(&TempInventory[nCurrentIndex], 0, sizeof(TItem));
	}

	if ((NewInventory[0].nItemID <= 0) ||(NewInventory[0].nSerial <= 0)){		// 정렬했는데 첫번째칸에 값이 없으면 클라에서 뭔가 잘못 날려준거..
		_DANGER_POINT();
		return false;
	}

	int nCount = 0;
	for (int i = 0; i < PERIODINVENTORYMAX; i++){
		if (TempInventory[i].nItemID > 0) nCount++;
	}

	if (nCount > 0) return false;	// 뭔가 잘못됐다.

	memcpy(&m_Inventory[INVENTORYMAX], &NewInventory, sizeof(TItem) * PERIODINVENTORYMAX);

	return true;
}
#endif	// #if defined(PRE_PERIOD_INVENTORY)

void CDNUserItem::GetInventoryItemDurability(std::vector<INT64> &VecSerialList, std::vector<USHORT> &VecDurList) const
{
#if defined(PRE_PERIOD_INVENTORY)
	for (int i = 0; i < INVENTORYTOTALMAX; i++){
		if (i == GetInventoryCount()){
			if (IsEnablePeriodInventory())
				i = INVENTORYMAX;
			else
				break;
		}
#else	// #if defined(PRE_PERIOD_INVENTORY)
	for (int i = 0; i < GetInventoryCount(); i++){
#endif	// #if defined(PRE_PERIOD_INVENTORY)
		if (m_Inventory[i].nItemID <= 0) continue;
		bool bSkip = true;
		switch( g_pDataManager->GetItemMainType( m_Inventory[i].nItemID ) ) 
		{
			case ITEMTYPE_WEAPON:
			case ITEMTYPE_PARTS:
			{
				bSkip = false;
				break;
			}
		}
		if( bSkip )
			continue;
		VecSerialList.push_back(m_Inventory[i].nSerial);
		VecDurList.push_back(m_Inventory[i].wDur);
	}
}

void CDNUserItem::CheatClearInven()
{
#if defined(PRE_PERIOD_INVENTORY)
	for (int i = 0; i < INVENTORYTOTALMAX; i++){
		if (i == GetInventoryCount()){
			if (IsEnablePeriodInventory())
				i = INVENTORYMAX;
			else
				break;
		}
#else	// #if defined(PRE_PERIOD_INVENTORY)
	for (int i = 0; i < GetInventoryCount(); i++){
#endif	// #if defined(PRE_PERIOD_INVENTORY)
		if (m_Inventory[i].nItemID <= 0) continue;
		DeleteInventoryBySlot(i, m_Inventory[i].wCount, m_Inventory[i].nSerial, DBDNWorldDef::UseItem::Use);
	}
}

// cash inven
void CDNUserItem::LoadCashInventory(int nPageNum, int nTotalCount, int nCount, TItem *CashItemList)
{
	m_nCashInventoryPage = nPageNum;
	if (nPageNum == 1)
		m_nCashInventoryTotalCount = nTotalCount;

	for (int i = 0; i < nCount; i++)
	{
#if defined( PRE_ADD_DIRECTNBUFF )
#if defined( _GAMESERVER )		
		int nType = g_pDataManager->GetItemMainType( CashItemList[i].nItemID );

		if(nType == ITEMTYPE_DIRECT_PARTYBUFF)
		{
			if(m_pSession->GetGameRoom())
			{
				m_pSession->GetGameRoom()->SetDirectPartyBuff( CashItemList[i].nItemID );
				m_pSession->SetDirectPartyBuffItem( true, CashItemList[i].nItemID );
			}
		}		
#endif
#endif
		m_MapCashInventory[CashItemList[i].nSerial] = CashItemList[i];
	}

	m_nCashInventoryPage++;
}

void CDNUserItem::ChangeCashInventoryItemDurability(INT64 biSerial, USHORT wDur)
{
	const TItem *pItem = GetCashInventory(biSerial);
	if( !pItem ) return;
	if( g_pDataManager->GetItemDurability( pItem->nItemID ) == 0 ) return;

	const_cast<TItem*>(pItem)->wDur = wDur;

	m_pSession->SendRefreshCashInven( *const_cast<TItem*>(pItem), false );
}

int CDNUserItem::GetCashItemCountByItemID(int nItemID)
{
	if (nItemID <= 0) return 0;
	if (m_MapCashInventory.empty()) return 0;

	int nCount = 0;
	TMapItem::iterator iter;
	for(iter = m_MapCashInventory.begin(); iter != m_MapCashInventory.end(); ++iter){
		if (iter->second.nItemID != nItemID)
			continue;
		if (IsExpired(iter->second))
			continue;
		nCount += iter->second.wCount;
	}

	return nCount;
}

int CDNUserItem::GetCashItemCountByType(int nType, int nTypeParam1)
{
	if (m_MapCashInventory.empty()) return 0;

	int nTotalCount = 0;
	TMapItem::iterator iter;
	for(iter = m_MapCashInventory.begin(); iter != m_MapCashInventory.end(); ++iter){
		if (IsExpired(iter->second)) continue;
		if (nTypeParam1 != -1 && g_pDataManager->GetItemTypeParam1(iter->second.nItemID) != nTypeParam1) continue;
		if (g_pDataManager->GetItemMainType(iter->second.nItemID) == nType){
			nTotalCount += iter->second.wCount;
		}
	}

	return nTotalCount;
}

void CDNUserItem::GetCashInventoryItemListByType( int nType, std::vector<const TItem*>& VecResult, int nTypeParam1/*=-1*/ )
{
	if (m_MapCashInventory.empty()) 
		return;

	int nTotalCount = 0;
	TMapItem::iterator iter;
	for(iter = m_MapCashInventory.begin(); iter != m_MapCashInventory.end(); ++iter)
	{
		if (IsExpired(iter->second))
			continue;
		if (nTypeParam1 != -1 && g_pDataManager->GetItemTypeParam1(iter->second.nItemID) != nTypeParam1) 
			continue;
		if (g_pDataManager->GetItemMainType(iter->second.nItemID) == nType)
			VecResult.push_back( &(*iter).second );
	}
}

TItem *CDNUserItem::GetCashItemByType(int nType)
{
	if (m_MapCashInventory.empty()) return 0;

	int nTotalCount = 0;
	TMapItem::iterator iter;
	for(iter = m_MapCashInventory.begin(); iter != m_MapCashInventory.end(); ++iter){
		if (IsExpired(iter->second)) continue;
		if (g_pDataManager->GetItemMainType(iter->second.nItemID) == nType){
			return &(iter->second);
		}
	}

	return NULL;
}

bool CDNUserItem::IsValidCashItem(int nItemID, short wCount)
{
	if (m_MapCashInventory.empty()) return false;

	int nTotalCount = 0;

	TMapItem::iterator iter;
	for(iter = m_MapCashInventory.begin(); iter != m_MapCashInventory.end(); ++iter){
		if (iter->second.nItemID == nItemID){
			if (IsExpired(iter->second))
				continue;
			nTotalCount += iter->second.wCount;
			if (nTotalCount >= wCount) return true;
		}
	}

	return false;
}

#if defined(_GAMESERVER)
void CDNUserItem::InitializePlayerCashItem(CDnItemTask *pItemTask)
{
	CDnItem *pItem = NULL;

	TMapItem::iterator iter;
	for(iter = m_MapCashInventory.begin(); iter != m_MapCashInventory.end(); ++iter){
		if (iter->second.nItemID == 0)
			continue;

		pItem = pItemTask->CreateItem( &iter->second );
		if( !pItem ) 
		{
			_ASSERT(0);
			continue;
		}
		pItemTask->InsertCashInventoryItem( m_pSession, iter->second.nSerial, pItem );
	}
}

#endif	// #if defined(_GAMESERVER)

void CDNUserItem::CheatClearCashInven()
{
	if (m_MapCashInventory.empty()) return;

	TMapItem::iterator iter;
	TItem Item = {0,};
	for(iter = m_MapCashInventory.begin(); iter != m_MapCashInventory.end(); ){
		Item = iter->second;
		m_pSession->GetDBConnection()->QueryUseItem(m_pSession, DBDNWorldDef::UseItem::Use, Item.nSerial, Item.wCount, true);

		m_MapCashInventory.erase(iter++);
		Item.wCount = 0;
		m_pSession->SendRefreshCashInven(Item, false);
	}
}

int CDNUserItem::GetExtendPrivateFarmFieldCount()
{
#if defined( PRE_ADD_PRIVATEFARM_EXTEND_CASH )

	std::vector<const TItem*> vItem;
	GetCashInventoryItemListByType( ITENTYPE_FARM_PRIVATE_EXTNED, vItem );

	if( vItem.empty() )
		return 0;

	int iExtend = 0;
	for( UINT i=0 ; i<vItem.size() ; ++i )
	{
		const TItemData* pItemData = g_pDataManager->GetItemData( vItem[i]->nItemID );
		if( pItemData )
			iExtend += pItemData->nTypeParam[0];
	}

	return iExtend;
#else
	return 0;
#endif // #if defined( PRE_ADD_PRIVATEFARM_EXTEND_CASH )
}

void CDNUserItem::OnRemoveCashItem(TADelCashItem *pDel)
{
	const TItem *pItem = GetCashInventory(pDel->biItemSerial);
	if (!pItem)
	{
		m_pSession->SendRemoveCash(NULL, ERROR_ITEM_NOTFOUND);
		return;
	}

	int nRet = pDel->nRetCode;
	switch (nRet)
	{
	case ERROR_NONE:
		{
			const_cast<TItem*>(pItem)->bEternity = false;
			const_cast<TItem*>(pItem)->bExpireComplete = true;
			const_cast<TItem*>(pItem)->tExpireDate = pDel->tExpireDate;
		}
		break;

	case 103202:	// 회수할 아이템이 존재하지 않습니다.
		{
			nRet = ERROR_ITEM_INSUFFICIENTY_PERIOD_RECOVERYCASH;
		}
		break;

	default:
		{
			nRet = ERROR_ITEM_FAIL;
		}
		break;
	}

	m_pSession->SendRemoveCash(pItem, nRet);
}

void CDNUserItem::OnRecoverCashItem(TARecoverCashItem *pRecover)
{
	const TItem *pItem = GetCashInventory(pRecover->biItemSerial);
	if (!pItem)
		m_pSession->SendRemoveCash(NULL, ERROR_ITEM_NOTFOUND);

	int nRet = pRecover->nRetCode;
	switch (nRet)
	{
	case ERROR_NONE:
		{
			const_cast<TItem*>(pItem)->bExpireComplete = false;
			const_cast<TItem*>(pItem)->tExpireDate = pRecover->tExpireDate;
			const_cast<TItem*>(pItem)->bEternity = pRecover->bEternity;
		}
		break;

	case 103151:	// 아이템이 존재하지 않습니다.
		{
			nRet = ERROR_ITEM_INSUFFICIENTY_PERIOD_RECOVERYCASH;
		}
		break;

	default:
		{
			nRet = ERROR_ITEM_FAIL;
		}
		break;
	}

	m_pSession->SendRemoveCash(pItem, nRet);
}

// ware관련 함수
bool CDNUserItem::IsValidWarehouseSlot(int nIndex)
{
	if (!_CheckRangeWarehouseIndex(nIndex)) return false;
	if (m_Warehouse[nIndex].nItemID <= 0) return false;

	return true;
}

int CDNUserItem::GetWarehouseItemCount(int nItemID) const
{
	if (nItemID <= 0) {
		return 0;
	}

	TItemData *pItemData = g_pDataManager->GetItemData(nItemID);
	if (!pItemData) {
		return 0;
	}

	int nCount = 0;
#if defined(PRE_PERIOD_INVENTORY)
	for (int i = 0; i < WAREHOUSETOTALMAX; i++){
		if (i == GetWarehouseCount()){
			if (IsEnablePeriodWarehouse())
				i = WAREHOUSEMAX;
			else
				break;
		}
#else	// #if defined(PRE_PERIOD_INVENTORY)
	for (int i = 0; i < GetWarehouseCount(); i++){
#endif	// #if defined(PRE_PERIOD_INVENTORY)
		if (m_Warehouse[i].nItemID == nItemID){
			nCount += m_Warehouse[i].wCount;
		}
	}

	return nCount;
}

int CDNUserItem::FindBlankWarehouseSlotCountList(std::queue<int> &qSlotList)
{
	int nCount = 0;
#if defined(PRE_PERIOD_INVENTORY)
	for (int i = 0; i < WAREHOUSETOTALMAX; i++){
		if (i == GetWarehouseCount()){
			if (IsEnablePeriodWarehouse())
				i = WAREHOUSEMAX;
			else
				break;
		}
#else	// #if defined(PRE_PERIOD_INVENTORY)
	for (int i = 0; i < GetWarehouseCount(); i++){
#endif	// #if defined(PRE_PERIOD_INVENTORY)
		if (m_Warehouse[i].nItemID == 0){
			qSlotList.push(i);
			nCount++;
		}
	}

	return nCount;
}

bool CDNUserItem::SortWarehouse(CSSortWarehouse * pPacket)
{
	// 정말 하기싫은 정렬이구나 ;ㅅ;
	TItem NewWarehouse[WAREHOUSEMAX] = { 0, };
	TItem TempWarehouse[WAREHOUSEMAX] = { 0, };
	memcpy(TempWarehouse, m_Warehouse, sizeof(TempWarehouse));	// 일단 빽업을 만들쟈;

	if( pPacket->cTotalCount > WAREHOUSEMAX || GetWarehouseCount() > WAREHOUSEMAX )
		return false;

	for (int i = 0; i < pPacket->cTotalCount; i++)
	{
		if (!_CheckRangeWarehouseIndex(pPacket->SlotInfo[i].cCurrent))
			return false;
		if (!_CheckRangeWarehouseIndex(pPacket->SlotInfo[i].cNew))
			return false;

		NewWarehouse[pPacket->SlotInfo[i].cNew] = TempWarehouse[pPacket->SlotInfo[i].cCurrent];
		memset(&TempWarehouse[pPacket->SlotInfo[i].cCurrent], 0, sizeof(TItem));
	}

	if ((NewWarehouse[0].nItemID <= 0) || (NewWarehouse[0].nSerial <= 0)){		// 정렬했는데 첫번째칸에 값이 없으면 클라에서 뭔가 잘못 날려준거..
		_DANGER_POINT();
		return false;
	}

	int nCount = 0;
	for (int i = 0; i < WAREHOUSEMAX; i++){
		if (TempWarehouse[i].nItemID > 0) nCount++;
	}

	if (nCount > 0) return false;	// 뭔가 잘못됐다.

	memcpy(m_Warehouse, NewWarehouse, sizeof(NewWarehouse));	// 다시 카피(아.. 일케 해야하는가;; ㅠㅠ)

	return true;
}

#if defined(PRE_PERIOD_INVENTORY)
bool CDNUserItem::SortPeriodWarehouse(CSSortWarehouse * pPacket)
{
	// 클라이언트 작업하면 다시 해야함!!
	TItem NewWarehouse[PERIODWAREHOUSEMAX] = { 0, };
	TItem TempWarehouse[PERIODWAREHOUSEMAX] = { 0, };
	memcpy(TempWarehouse, &m_Warehouse[WAREHOUSEMAX], sizeof(TempWarehouse));	// 일단 빽업을 만들쟈;

	if( pPacket->cTotalCount > PERIODWAREHOUSEMAX )
		return false;

	for (int i = 0; i < pPacket->cTotalCount; i++)
	{
		int nCurrentWarehouseIndex = pPacket->SlotInfo[i].cCurrent;
		int nNewWarehouseIndex = pPacket->SlotInfo[i].cNew + WAREHOUSEMAX;
		int nCurrentIndex = pPacket->SlotInfo[i].cCurrent - WAREHOUSEMAX;
		int nNewIndex = pPacket->SlotInfo[i].cNew;

		if (!_CheckRangeWarehouseIndex(nCurrentWarehouseIndex))
			return false;
		if (!_CheckRangeWarehouseIndex(nNewWarehouseIndex))
			return false;

		NewWarehouse[nNewIndex] = TempWarehouse[nCurrentIndex];
		memset(&TempWarehouse[nCurrentIndex], 0, sizeof(TItem));
	}

	if ((NewWarehouse[0].nItemID <= 0) || (NewWarehouse[0].nSerial <= 0)){		// 정렬했는데 첫번째칸에 값이 없으면 클라에서 뭔가 잘못 날려준거..
		_DANGER_POINT();
		return false;
	}

	int nCount = 0;
	for (int i = 0; i < PERIODWAREHOUSEMAX; i++){
		if (TempWarehouse[i].nItemID > 0) nCount++;
	}

	if (nCount > 0) return false;	// 뭔가 잘못됐다.

	memcpy(&m_Warehouse[WAREHOUSEMAX], &NewWarehouse, sizeof(TItem) * PERIODWAREHOUSEMAX);

	return true;
}
#endif	// #if defined(PRE_PERIOD_INVENTORY)

void CDNUserItem::GetIndexSerialList(int &nTotalInvenWareCount, DBPacket::TItemIndexSerial *SaveList)
{
	DBPacket::TItemIndexSerial *pInfo = &SaveList[0];

	nTotalInvenWareCount = 0;
#if defined(PRE_PERIOD_INVENTORY)
	for (int i = 0; i < INVENTORYTOTALMAX; i++){
		if (i == GetInventoryCount()){
			if (IsEnablePeriodInventory())
				i = INVENTORYMAX;
			else
				break;
		}
#else	// #if defined(PRE_PERIOD_INVENTORY)
	for (int i = 0; i < GetInventoryCount(); i++){
#endif	// #if defined(PRE_PERIOD_INVENTORY)
		if (m_Inventory[i].nItemID <= 0) continue;
		SaveList[nTotalInvenWareCount].cSlotIndex = i;
		SaveList[nTotalInvenWareCount].biItemSerial = m_Inventory[i].nSerial;
		nTotalInvenWareCount++;
	}

#if defined(PRE_PERIOD_INVENTORY)
	for (int i = 0; i < WAREHOUSETOTALMAX; i++){
		if (i == GetWarehouseCount()){
			if (IsEnablePeriodWarehouse())
				i = WAREHOUSEMAX;
			else
				break;
		}
#else	// #if defined(PRE_PERIOD_INVENTORY)
	for (int i = 0; i < GetWarehouseCount(); i++){
#endif	// #if defined(PRE_PERIOD_INVENTORY)
		if (m_Warehouse[i].nItemID <= 0) continue;
		SaveList[nTotalInvenWareCount].cSlotIndex = i;
		SaveList[nTotalInvenWareCount].biItemSerial = m_Warehouse[i].nSerial;
		nTotalInvenWareCount++;
	}
}

// Quest Inventory 관련
int CDNUserItem::FindBlankQuestInventorySlotCountList(std::queue<int> &qSlotList)
{
	int nCount = 0;
	for (int i = 0; i < QUESTINVENTORYMAX; i++){
		if (m_QuestInventory[i].nItemID == 0){
			qSlotList.push(i);
			nCount++;
		}
	}

	return nCount;
}

int CDNUserItem::FindBlankQuestInventorySlotCount()
{
	int nCount = 0;
	for (int i = 0; i < QUESTINVENTORYMAX; i++){
		if (m_QuestInventory[i].nItemID == 0) nCount++;
	}

	return nCount;
}

int	CDNUserItem::GetQuestInventoryItemCount(int nItemID)
{
	if (nItemID <= 0) return 0;

	int nCount = 0;
	for (int i = 0; i < QUESTINVENTORYMAX; i++){
		if (m_QuestInventory[i].nItemID == nItemID){
			nCount += m_QuestInventory[i].wCount;
		}
	}

	return nCount;
}

void CDNUserItem::CheatClearQuestInven()
{
	for (int i = 0; i < QUESTINVENTORYMAX; i++) {
		if (m_QuestInventory[i].nItemID <= 0) continue;

		int aRetVal = DeleteQuestInventory(m_QuestInventory[i].nItemID, m_QuestInventory[i].wCount, 0, DBDNWorldDef::UseItem::Destroy);
		if (ERROR_NONE != aRetVal) {
			DN_ASSERT(0,	"Check!");
		}
	}
}

// 우편
#if defined(_VILLAGESERVER)
int CDNUserItem::IsValidSendMailItem(int nInvenIndex, int nItemID, short wCount, INT64 biSerial, DBPacket::TSendMailItem &ItemInfo)
{
	if (wCount <= 0) return ERROR_MAIL_ATTACHITEMFAIL;

#if defined(PRE_MAILRENEWAL)
	bool IsCash = g_pDataManager->IsCashItem(nItemID);
	if (IsCash){
		const TItem *pCashItem = GetCashInventory(biSerial);
		if (!pCashItem) return ERROR_MAIL_ATTACHITEMFAIL;

		if (pCashItem->nItemID != nItemID) return ERROR_MAIL_ATTACHITEMFAIL;
		if (g_pDataManager->GetItemOverlapCount(pCashItem->nItemID) < wCount) return ERROR_ITEM_OVERFLOW;	// max값 검사
		int nRet = IsTradeEnableItem(ITEMPOSITION_CASHINVEN, nInvenIndex, biSerial, wCount);
		if (nRet != ERROR_NONE)
			return nRet;

		if (pCashItem->nItemID > 0){
			if (g_pDataManager->GetItemMainType(pCashItem->nItemID) == ITEMTYPE_QUEST) return ERROR_MAIL_ATTACHITEMFAIL;
		}

		ItemInfo.cSlotIndex = 0;
		ItemInfo.Item = *pCashItem;
		ItemInfo.Item.wCount = wCount;
	}
	else{
#endif	// #if defined(PRE_MAILRENEWAL)
		if (!IsValidInventorySlot(nInvenIndex, biSerial)) return ERROR_ITEM_INDEX_UNMATCH;
		if (m_Inventory[nInvenIndex].nItemID != nItemID) return ERROR_MAIL_ATTACHITEMFAIL;
		if (m_Inventory[nInvenIndex].nSerial != biSerial) return ERROR_MAIL_ATTACHITEMFAIL;
		if (g_pDataManager->GetItemOverlapCount(m_Inventory[nInvenIndex].nItemID) < wCount) return ERROR_ITEM_OVERFLOW;	// max값 검사
		int nRet = IsTradeEnableItem(ITEMPOSITION_INVEN, nInvenIndex, biSerial, wCount);
		if (nRet != ERROR_NONE)
			return nRet;

		if (m_Inventory[nInvenIndex].nItemID > 0){
			if (g_pDataManager->GetItemMainType(m_Inventory[nInvenIndex].nItemID) == ITEMTYPE_QUEST) return ERROR_MAIL_ATTACHITEMFAIL;
		}

		ItemInfo.cSlotIndex = nInvenIndex;
		ItemInfo.Item = m_Inventory[nInvenIndex];
		ItemInfo.Item.wCount = wCount;
		if (m_Inventory[nInvenIndex].wCount > wCount){	// 수량아이템중 뗘서 보낼경우 시리얼을 새로 발급해서 보내야한다
			ItemInfo.biNewSerial = MakeItemSerial();
		}
#if defined(PRE_MAILRENEWAL)
	}
#endif	// #if defined(PRE_MAILRENEWAL)

	return ERROR_NONE;
}

int CDNUserItem::CheckSendMailItem(const CSSendMail *pPacket)
{
	switch(pPacket->cDeliveryType)
	{
	case MailType::Normal:
		{
			if (pPacket->cAttachItemTotalCount > 1) return ERROR_ITEM_FAIL;
		}
		break;

	case MailType::Premium:
		{
			if (pPacket->cAttachItemTotalCount > MAILATTACHITEMMAX) return ERROR_ITEM_FAIL;
			int nTotalCount = GetCashItemCountByType(ITEMTYPE_PREMIUM_POST);
			if (nTotalCount < 1) return ERROR_ITEM_FAIL;	// 우표 부족
		}
		break;
	}

	INT64 nTotalPrice = 0;
	// 코인 첨부면 그만큼 있는지 먼저 검사한다
	if (pPacket->nAttachCoin < 0) return ERROR_ITEM_OVERFLOWMONEY;
	if (pPacket->nAttachCoin > 0){
		if (pPacket->nAttachCoin > 1000000000) return ERROR_ITEM_OVERFLOWMONEY;
		if (!m_pSession->CheckEnoughCoin(pPacket->nAttachCoin)) return ERROR_MAIL_INSUFFICIENCY_MONEY;

		nTotalPrice += pPacket->nAttachCoin;
	}

	DBPacket::TSendMailItem AttachItemArray[MAILATTACHITEMMAX];
	memset(&AttachItemArray, 0, sizeof(AttachItemArray));
	for (int i = 0; i < pPacket->cAttachItemTotalCount; i++){
		if (pPacket->AttachMailItem[i].nInvenIndex != -1){
			if (g_pDataManager->GetItemMainType(pPacket->AttachMailItem[i].nItemID) == ITEMTYPE_QUEST) return ERROR_MAIL_ATTACHITEMFAIL;

			int nRet = IsValidSendMailItem(pPacket->AttachMailItem[i].nInvenIndex, pPacket->AttachMailItem[i].nItemID, pPacket->AttachMailItem[i].nCount, pPacket->AttachMailItem[i].biSerial, AttachItemArray[i]);
			if (nRet != ERROR_NONE) return nRet;

			nTotalPrice += g_pDataManager->GetItemPrice(pPacket->AttachMailItem[i].nItemID) * pPacket->AttachMailItem[i].nCount;
		}
	}

	// 일반 우편 수수료
	int nDefaultTax = g_pDataManager->GetTaxAmount(TAX_POSTDEFAULT, m_pSession->GetLevel(), nTotalPrice);
	// 첨부아이템 수수료
	INT64 nTax = g_pDataManager->CalcTax(TAX_POST, m_pSession->GetLevel(), nTotalPrice);
	// 수수료랑 코인 더한값이 인벤에 있는지 검사한다
	if (!m_pSession->CheckEnoughCoin(nDefaultTax + nTax + pPacket->nAttachCoin)) return ERROR_MAIL_INSUFFICIENCY_MONEY;

	m_pSession->GetDBConnection()->QuerySendMail(m_pSession, pPacket, AttachItemArray, (int)(nTax + nDefaultTax));

	switch(pPacket->cDeliveryType)
	{
	case MailType::Normal:
		{
			if (!AddDailyMailCount()) return ERROR_ITEM_FAIL;	// 일일발송개수 체크
		}
		break;
	}

	return ERROR_NONE;
}

#ifdef PRE_ADD_JOINGUILD_SUPPORT
void CDNUserItem::SetReadMail(int nMailDBID, INT64 biCoin, const TItem *MailItemArray, BYTE cMailType)
{
	if (cMailType == DBDNWorldDef::MailTypeCode::GuildMaxLevelReward)
		m_ReadMailAttachItem.bAddGuildWare = true;
	else
		m_ReadMailAttachItem.bAddGuildWare = false;

#else		//#ifdef PRE_ADD_JOINGUILD_SUPPORT
void CDNUserItem::SetReadMail(int nMailDBID, INT64 biCoin, const TItem *MailItemArray)
{
#endif		//#ifdef PRE_ADD_JOINGUILD_SUPPORT
	m_ReadMailAttachItem.nMailDBID = nMailDBID;
	m_ReadMailAttachItem.biAttachCoin = biCoin;
	for (int i = 0; i < MAILATTACHITEMMAX; i++){
		m_ReadMailAttachItem.TakeItem[i].Item = MailItemArray[i];
	}
}


void CDNUserItem::ClearReadMail( int iIndex/*=-1*/)
{
	if( iIndex >= 0 )
	{
		if( iIndex >= MAILATTACHITEMMAX || iIndex < 0 )
		{
			_ASSERT(0);
			return;
		}
		
		memset( &m_ReadMailAttachItem.TakeItem[iIndex], 0, sizeof(m_ReadMailAttachItem.TakeItem[0] ) );

		for( UINT i=0 ; i<_countof(m_ReadMailAttachItem.TakeItem) ; ++i )
		{
			if( m_ReadMailAttachItem.TakeItem[i].Item.nItemID != 0 )
				return;
		}

		if (m_ReadMailAttachItem.biAttachCoin > 0) return;	// 코인이 남아있으면 clear시키면 안된다
	}

	memset(&m_ReadMailAttachItem, 0, sizeof(m_ReadMailAttachItem));
}

int CDNUserItem::VerifyAttachItemList(const TAGetListMailAttachment *pMail)
{
	int nRet = ERROR_NONE;
	INT64 nCoin = 0;
	std::queue<int> qSlotList;
	int nInvenCount = FindBlankInventorySlotCountList(qSlotList);
#ifdef PRE_ADD_JOINGUILD_SUPPORT
	int nGuildMailItemCount = 0;
	CDNGuildBase* pGuild = NULL;
	for (int i = 0; i < pMail->cCount; i++)
	{
		if (pMail->MailAttachArray[i].cMailType == DBDNWorldDef::MailTypeCode::GuildMaxLevelReward)
		{
			const TGuildUID GuildUID = m_pSession->GetGuildUID();
			pGuild = g_pGuildManager->At(GuildUID);
			break;
		}
	}
#endif		//#ifdef PRE_ADD_JOINGUILD_SUPPORT

	DBPacket::TRequestTakeAttachInfo InfoList[MAILPAGEMAX] = { 0, };
	std::vector<TItem> VecItems;

	for (int i = 0; i < pMail->cCount; i++){
		if (pMail->MailAttachArray[i].nMailDBID <= 0) continue;

		if (pMail->MailAttachArray[i].biAttachCoin > 0){
			nCoin += pMail->MailAttachArray[i].biAttachCoin;

#ifdef PRE_ADD_JOINGUILD_SUPPORT
			if (pMail->MailAttachArray[i].cMailType == DBDNWorldDef::MailTypeCode::GuildMaxLevelReward)
			{
				if (pGuild)
				{
					bool bCheck = true;
#if !defined( PRE_ADD_NODELETEGUILD )
						CDetachAutoEx<CDNGuildBase>	AutoDetach(pGuild);
					if (FALSE == pGuild->IsEnable())
						bCheck = false;
#endif
					if (bCheck)
					{
						// 빌리지길드로변환
						_ASSERT( dynamic_cast<CDNGuildVillage *>(pGuild) );
						CDNGuildVillage* pGuildVillage = static_cast<CDNGuildVillage *>(pGuild);

						if (pGuildVillage)
						{
							CDNGuildWare * pGuildWare = pGuildVillage->GetGuildWare();
							if (pGuildWare == NULL)
							{
								nRet = ERROR_GUILDWARE_NOT_READY;
								break;
							}

							if (nCoin + pGuildWare->GetWarehouseCoin() > GUILDWARE_COINMAX)
							{
								nRet = ERROR_ITEM_INVENTOWARE02;
								break;
							}
						}
					}
				}
			}
			else
#endif		//PRE_ADD_JOINGUILD_SUPPORT
			{
				if (!m_pSession->CheckMaxCoin(nCoin)){
					nRet = ERROR_ITEM_OVERFLOWMONEY;	// 42억 넘어감
					break;
				}
			}
		}

		InfoList[i].nMailDBID = pMail->MailAttachArray[i].nMailDBID;
		InfoList[i].bAttachCoin = (pMail->MailAttachArray[i].biAttachCoin > 0) ? true : false;

		if (pMail->MailAttachArray[i].cItemAttachCount > 0)
		{
#ifdef PRE_ADD_JOINGUILD_SUPPORT
			if (pMail->MailAttachArray[i].cMailType == DBDNWorldDef::MailTypeCode::GuildMaxLevelReward)
			{
				if (pGuild)
				{
					bool bCheck = true;
#if !defined( PRE_ADD_NODELETEGUILD )
						CDetachAutoEx<CDNGuildBase>	AutoDetach(pGuild);
					if (FALSE == pGuild->IsEnable())
						bCheck = false;
#endif
					if (bCheck)
					{
						// 빌리지길드로변환
						_ASSERT( dynamic_cast<CDNGuildVillage *>(pGuild) );
						CDNGuildVillage* pGuildVillage = static_cast<CDNGuildVillage *>(pGuild);
						if (pGuildVillage)
						{
							CDNGuildWare * pGuildWare = pGuildVillage->GetGuildWare();
							if (pGuildWare == NULL)
							{
								nRet = ERROR_GUILDWARE_NOT_READY;
								break;
							}
							
							for (int j = 0; j < MAILATTACHITEMMAX; j++)
							{
								if (pMail->MailAttachArray[i].Item[j].nItemID <= 0) continue;
								nGuildMailItemCount += pMail->MailAttachArray[i].cItemAttachCount;
							}

							if (nGuildMailItemCount > 0)
							{
								if (pGuild->GetWareSize() - pGuildWare->GetWareItemCount() < nGuildMailItemCount)
								{
									nRet = ERROR_FULL_GUILDWARE;
									break;
								}
							}

							for (int j = 0; j < MAILATTACHITEMMAX; j++)
							{
								if (pMail->MailAttachArray[i].Item[j].nItemID <= 0) continue;
								InfoList[i].biSerial[j] = pMail->MailAttachArray[i].Item[j].nSerial;
							}
						}
					}
				}
			}
			else
#endif		//#ifdef PRE_ADD_JOINGUILD_SUPPORT
			{
#if defined(PRE_MAILRENEWAL)
				for (int j = 0; j < MAILATTACHITEMMAX; j++){
					if (pMail->MailAttachArray[i].Item[j].nItemID <= 0) continue;

					bool IsCash = g_pDataManager->IsCashItem(pMail->MailAttachArray[i].Item[j].nItemID);
					if (IsCash){
						VecItems.push_back(pMail->MailAttachArray[i].Item[j]);
					}
					else{
						if (qSlotList.empty()) return ERROR_ITEM_INVENTORY_NOTENOUGH;
						if (nInvenCount != qSlotList.size()) return ERROR_ITEM_INVENTORY_NOTENOUGH;

						nInvenCount -= pMail->MailAttachArray[i].cItemAttachCount;

						if (nInvenCount < pMail->MailAttachArray[i].cItemAttachCount){
							nRet = ERROR_ITEM_INVENTORY_NOTENOUGH;	// 인벤공간부족
							break;
						}

						InfoList[i].biSerial[j] = pMail->MailAttachArray[i].Item[j].nSerial;
						InfoList[i].cSlotIndex[j] = qSlotList.front();
						qSlotList.pop();
					}
				}

#else	// #if defined(PRE_MAILRENEWAL)
				if (qSlotList.empty()) return ERROR_ITEM_INVENTORY_NOTENOUGH;
				if (nInvenCount != qSlotList.size()) return ERROR_ITEM_INVENTORY_NOTENOUGH;

				if (nInvenCount < pMail->MailAttachArray[i].cItemAttachCount){
					nRet = ERROR_ITEM_INVENTORY_NOTENOUGH;	// 인벤공간부족
					break;
				}
				nInvenCount -= pMail->MailAttachArray[i].cItemAttachCount;

				for (int j = 0; j < MAILATTACHITEMMAX; j++){
					if (pMail->MailAttachArray[i].Item[j].nItemID <= 0) continue;
					if (qSlotList.empty()) return ERROR_ITEM_INVENTORY_NOTENOUGH;

					InfoList[i].biSerial[j] = pMail->MailAttachArray[i].Item[j].nSerial;
					InfoList[i].cSlotIndex[j] = qSlotList.front();
					qSlotList.pop();
				}
#endif	// #if defined(PRE_MAILRENEWAL)
			}
		}
	}

	if (!VecItems.empty()){
		nRet = CheckCashMail(VecItems);
		if (nRet != ERROR_NONE)
			return nRet;
	}

	if( nRet == ERROR_NONE && pMail->cCount > 0){
		m_pSession->GetDBConnection()->QueryTakeMailAttachList(m_pSession, pMail->cCount, InfoList);
	}

	return nRet;
}

int CDNUserItem::VerifyAttachItem(const CSAttachMail *pMail)
{
	if (m_ReadMailAttachItem.nMailDBID != pMail->nMailDBID) return ERROR_DB;

	if (m_ReadMailAttachItem.nMailDBID > 0){
		bool IsAttachItem = false;
		DBPacket::TRequestTakeAttachInfo Info = { 0, };
		Info.nMailDBID = m_ReadMailAttachItem.nMailDBID;
		Info.cAttachSlotIndex = pMail->cAttachSlotIndex;

		switch(pMail->cAttachSlotIndex)
		{
		case -1:	// 모두받기
			{
#ifdef PRE_ADD_JOINGUILD_SUPPORT
				if (m_ReadMailAttachItem.bAddGuildWare)
				{
					if (m_pSession->GetGuildUID().IsSet()) 
					{
						const TGuildUID GuildUID = m_pSession->GetGuildUID();
						CDNGuildBase* pGuild = g_pGuildManager->At(GuildUID);
						if (pGuild)
						{
							bool bCheck = true;
#if !defined( PRE_ADD_NODELETEGUILD )
							CDetachAutoEx<CDNGuildBase>	AutoDetach(pGuild);
							if (FALSE == pGuild->IsEnable())
								bCheck = false;
#endif
							if (bCheck)
							{
								// 빌리지길드로변환
								_ASSERT( dynamic_cast<CDNGuildVillage *>(pGuild) );
								CDNGuildVillage* pGuildVillage = static_cast<CDNGuildVillage *>(pGuild);

								if (pGuildVillage)
								{
									CDNGuildWare * pGuildWare = pGuildVillage->GetGuildWare();
									if (pGuildWare == NULL)
										return ERROR_GUILDWARE_NOT_READY;

									if (m_ReadMailAttachItem.biAttachCoin + pGuildWare->GetWarehouseCoin() > GUILDWARE_COINMAX)
										return ERROR_ITEM_INVENTOWARE02;

									int nGuildMailItemCount = 0;
									for (int j = 0; j < MAILATTACHITEMMAX; j++)
									{
										if (m_ReadMailAttachItem.TakeItem[j].Item.nItemID > 0)
										{
											nGuildMailItemCount++;
											IsAttachItem = true;
										}
									}

									if (nGuildMailItemCount > 0)
									{
										if (pGuild->GetWareSize() - pGuildWare->GetWareItemCount() < nGuildMailItemCount)
											return ERROR_FULL_GUILDWARE;
										
										for (int j = 0; j < MAILATTACHITEMMAX; j++){
											if (m_ReadMailAttachItem.TakeItem[j].Item.nItemID <= 0) continue;
											
											Info.biSerial[j] = m_ReadMailAttachItem.TakeItem[j].Item.nSerial;
										}
									}
								}
							}
							else
								_DANGER_POINT();
						}
					}
				}
				else
#endif		//#ifdef PRE_ADD_JOINGUILD_SUPPORT
				{
					if (!m_pSession->CheckMaxCoin(m_ReadMailAttachItem.biAttachCoin)) return ERROR_ITEM_OVERFLOWMONEY;	// 42억 넘어감

					for (int j = 0; j < MAILATTACHITEMMAX; j++){
						if (m_ReadMailAttachItem.TakeItem[j].Item.nItemID > 0) IsAttachItem = true;
					}

					Info.bAttachCoin = (m_ReadMailAttachItem.biAttachCoin > 0) ? true : false;

					if (IsAttachItem){
						std::queue<int> qSlotList;
						int nInvenCount = FindBlankInventorySlotCountList(qSlotList);

#if defined(PRE_MAILRENEWAL)
						std::vector<TItem> VecItems;
						for (int j = 0; j < MAILATTACHITEMMAX; j++){
							if (m_ReadMailAttachItem.TakeItem[j].Item.nItemID <= 0) continue;
							if (g_pDataManager->IsCashItem(m_ReadMailAttachItem.TakeItem[j].Item.nItemID)){
								Info.biSerial[j] = m_ReadMailAttachItem.TakeItem[j].Item.nSerial;

								VecItems.push_back(m_ReadMailAttachItem.TakeItem[j].Item);
							}
							else{
								if (nInvenCount != qSlotList.size()) return ERROR_ITEM_INVENTORY_NOTENOUGH;
								if (qSlotList.empty()) return ERROR_ITEM_INVENTORY_NOTENOUGH;

								m_ReadMailAttachItem.TakeItem[j].cSlotIndex = qSlotList.front();
								qSlotList.pop();

								Info.biSerial[j] = m_ReadMailAttachItem.TakeItem[j].Item.nSerial;
								Info.cSlotIndex[j] = m_ReadMailAttachItem.TakeItem[j].cSlotIndex;
							}
						}

						if (!VecItems.empty()){
							int nRet = CheckCashMail(VecItems);
							if (nRet != ERROR_NONE)
								return nRet;
						}

#else	// #if defined(PRE_MAILRENEWAL)
						if (qSlotList.empty()) return ERROR_ITEM_INVENTORY_NOTENOUGH;
						if (nInvenCount != qSlotList.size()) return ERROR_ITEM_INVENTORY_NOTENOUGH;

						for (int j = 0; j < MAILATTACHITEMMAX; j++){
							if (m_ReadMailAttachItem.TakeItem[j].Item.nItemID <= 0) continue;
							if (qSlotList.empty()) return ERROR_ITEM_INVENTORY_NOTENOUGH;
							m_ReadMailAttachItem.TakeItem[j].cSlotIndex = qSlotList.front();
							qSlotList.pop();

							Info.biSerial[j] = m_ReadMailAttachItem.TakeItem[j].Item.nSerial;
							Info.cSlotIndex[j] = m_ReadMailAttachItem.TakeItem[j].cSlotIndex;
						}
#endif	// #if defined(PRE_MAILRENEWAL)
					}
				}
			}
			break;

		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
			{
#ifdef PRE_ADD_JOINGUILD_SUPPORT
				if (m_ReadMailAttachItem.bAddGuildWare)
				{
					if (m_ReadMailAttachItem.TakeItem[pMail->cAttachSlotIndex].Item.nItemID > 0) IsAttachItem = true;

					if (IsAttachItem)
					{
						if (m_pSession->GetGuildUID().IsSet()) 
						{
							const TGuildUID GuildUID = m_pSession->GetGuildUID();
							CDNGuildBase* pGuild = g_pGuildManager->At(GuildUID);
							if (pGuild)
							{
								bool bCheck = true;
#if !defined( PRE_ADD_NODELETEGUILD )
									CDetachAutoEx<CDNGuildBase>	AutoDetach(pGuild);
								if (FALSE == pGuild->IsEnable())
									bCheck = false;
#endif
								if (bCheck)
								{
									// 빌리지길드로변환
									_ASSERT( dynamic_cast<CDNGuildVillage *>(pGuild) );
									CDNGuildVillage* pGuildVillage = static_cast<CDNGuildVillage *>(pGuild);

									if (pGuildVillage)
									{
										CDNGuildWare * pGuildWare = pGuildVillage->GetGuildWare();

										if (pGuildWare == NULL)
											return ERROR_GUILDWARE_NOT_READY;
										
										int nGuildMailItemCount = 0;
										for (int j = 0; j < MAILATTACHITEMMAX; j++)
											if (m_ReadMailAttachItem.TakeItem[j].Item.nItemID > 0)
												nGuildMailItemCount++;

										if (nGuildMailItemCount > 0)
										{
											if (pGuild->GetWareSize() - pGuildWare->GetWareItemCount() <= 0)
												return ERROR_FULL_GUILDWARE;					

											Info.biSerial[pMail->cAttachSlotIndex] = m_ReadMailAttachItem.TakeItem[pMail->cAttachSlotIndex].Item.nSerial;
										}
									}
								}
								else
									_DANGER_POINT();
							}
						}
					}
				}
				else
#endif		//#ifdef PRE_ADD_JOINGUILD_SUPPORT
				{
					if (m_ReadMailAttachItem.TakeItem[pMail->cAttachSlotIndex].Item.nItemID > 0) IsAttachItem = true;

					if (IsAttachItem){
						std::queue<int> qSlotList;
						int nInvenCount = FindBlankInventorySlotCountList(qSlotList);

#if defined(PRE_MAILRENEWAL)
						std::vector<TItem> VecItems;
						if (g_pDataManager->IsCashItem(m_ReadMailAttachItem.TakeItem[pMail->cAttachSlotIndex].Item.nItemID)){
							Info.biSerial[pMail->cAttachSlotIndex] = m_ReadMailAttachItem.TakeItem[pMail->cAttachSlotIndex].Item.nSerial;

							VecItems.push_back(m_ReadMailAttachItem.TakeItem[pMail->cAttachSlotIndex].Item);

							int nRet = CheckCashMail(VecItems);
							if (nRet != ERROR_NONE)
								return nRet;
						}
						else{
							if (qSlotList.empty()) return ERROR_ITEM_INVENTORY_NOTENOUGH;
							if (nInvenCount != qSlotList.size()) return ERROR_ITEM_INVENTORY_NOTENOUGH;

							m_ReadMailAttachItem.TakeItem[pMail->cAttachSlotIndex].cSlotIndex = qSlotList.front();
							qSlotList.pop();

							Info.biSerial[pMail->cAttachSlotIndex] = m_ReadMailAttachItem.TakeItem[pMail->cAttachSlotIndex].Item.nSerial;
							Info.cSlotIndex[pMail->cAttachSlotIndex] = m_ReadMailAttachItem.TakeItem[pMail->cAttachSlotIndex].cSlotIndex;
						}

#else	// #if defined(PRE_MAILRENEWAL)
						if (qSlotList.empty()) return ERROR_ITEM_INVENTORY_NOTENOUGH;
						if (nInvenCount != qSlotList.size()) return ERROR_ITEM_INVENTORY_NOTENOUGH;

						m_ReadMailAttachItem.TakeItem[pMail->cAttachSlotIndex].cSlotIndex = qSlotList.front();
						qSlotList.pop();

						Info.biSerial[pMail->cAttachSlotIndex] = m_ReadMailAttachItem.TakeItem[pMail->cAttachSlotIndex].Item.nSerial;
						Info.cSlotIndex[pMail->cAttachSlotIndex] = m_ReadMailAttachItem.TakeItem[pMail->cAttachSlotIndex].cSlotIndex;
#endif	// #if defined(PRE_MAILRENEWAL)
					}
				}

			}
			break;

		default:
			return ERROR_ITEM_FAIL;
		}

		if (Info.bAttachCoin || IsAttachItem)
			m_pSession->GetDBConnection()->QueryTakeMailAttach(m_pSession, Info);
	}

	return ERROR_NONE;
}

int CDNUserItem::TakeAttachItemList(const TATakeAttachMailList *pMail)
{
	for (int i = 0; i < pMail->cPageCount; i++){
#ifdef PRE_ADD_JOINGUILD_SUPPORT
		if (pMail->TakeMailList[i].bAddGuildWare) continue;
#endif		//#ifdef PRE_ADD_JOINGUILD_SUPPORT
		if (pMail->TakeMailList[i].biAttachCoin > 0){
			if (!m_pSession->AddCoin(pMail->TakeMailList[i].biAttachCoin, DBDNWorldDef::CoinChangeCode::DoNotDBSave, 0)) return ERROR_ITEM_OVERFLOWMONEY;
		}

		for (int j = 0; j < MAILATTACHITEMMAX; j++){
			if (pMail->TakeMailList[i].TakeItem[j].Item.nItemID <= 0) continue;

#if defined(PRE_MAILRENEWAL)
			if (g_pDataManager->IsCashItem(pMail->TakeMailList[i].TakeItem[j].Item.nItemID)){
				CreateCashInvenWholeItem(pMail->TakeMailList[i].TakeItem[j].Item);
			}
			else{
				CreateInvenWholeItemByIndex(pMail->TakeMailList[i].TakeItem[j].cSlotIndex, pMail->TakeMailList[i].TakeItem[j].Item);
			}
#else	// #if defined(PRE_MAILRENEWAL)
			CreateInvenWholeItemByIndex(pMail->TakeMailList[i].TakeItem[j].cSlotIndex, pMail->TakeMailList[i].TakeItem[j].Item);
#endif	// #if defined(PRE_MAILRENEWAL)
		}
	}

	return ERROR_NONE;
}

int CDNUserItem::TakeAttachItem(const TATakeAttachMail *pMail)
{
	ClearReadMail( pMail->cAttachSlotIndex );

	switch(pMail->cAttachSlotIndex)
	{
	case -1:
		{
			if (pMail->TakeMail.biAttachCoin > 0){
				if (!m_pSession->AddCoin(pMail->TakeMail.biAttachCoin, DBDNWorldDef::CoinChangeCode::DoNotDBSave, 0)) return ERROR_ITEM_OVERFLOWMONEY;
			}

			for (int j = 0; j < MAILATTACHITEMMAX; j++){
				if (pMail->TakeMail.TakeItem[j].Item.nItemID <= 0) continue;

#if defined(PRE_MAILRENEWAL)
				if (g_pDataManager->IsCashItem(pMail->TakeMail.TakeItem[j].Item.nItemID)){
					CreateCashInvenWholeItem(pMail->TakeMail.TakeItem[j].Item);
				}
				else{
					CreateInvenWholeItemByIndex(pMail->TakeMail.TakeItem[j].cSlotIndex, pMail->TakeMail.TakeItem[j].Item);
				}
#else	// #if defined(PRE_MAILRENEWAL)
				CreateInvenWholeItemByIndex(pMail->TakeMail.TakeItem[j].cSlotIndex, pMail->TakeMail.TakeItem[j].Item);
#endif	// #if defined(PRE_MAILRENEWAL)
			}
		}
		break;

	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
		{
#if defined(PRE_MAILRENEWAL)
			if (g_pDataManager->IsCashItem(pMail->TakeMail.TakeItem[pMail->cAttachSlotIndex].Item.nItemID)){
				CreateCashInvenWholeItem(pMail->TakeMail.TakeItem[pMail->cAttachSlotIndex].Item);
			}
			else{
				CreateInvenWholeItemByIndex(pMail->TakeMail.TakeItem[pMail->cAttachSlotIndex].cSlotIndex, pMail->TakeMail.TakeItem[pMail->cAttachSlotIndex].Item);
			}
#else	// #if defined(PRE_MAILRENEWAL)
			CreateInvenWholeItemByIndex(pMail->TakeMail.TakeItem[pMail->cAttachSlotIndex].cSlotIndex, pMail->TakeMail.TakeItem[pMail->cAttachSlotIndex].Item);
#endif	// #if defined(PRE_MAILRENEWAL)
		}
		break;
	}

	return ERROR_NONE;
}

bool CDNUserItem::AddDailyMailCount()
{
	int nCount = (int)(CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::DailyMailCount));
	if (nCount <= 0) return false;
	if (m_nDailyMailCount == nCount) return false;	// 일일 발송가능 개수가 다 찼음

	m_nDailyMailCount++;
	return true;
}

bool CDNUserItem::DelDailyMailCount()
{
	if (m_nDailyMailCount == 0) return false;
	m_nDailyMailCount--;
	return true;
}

void CDNUserItem::SetDailyMailCount(int nCount)
{
	if (nCount <= 0) return;
	m_nDailyMailCount = nCount;
}

#endif	// #if defined(_VILLAGESERVER)

#if defined(PRE_SPECIALBOX)
void CDNUserItem::SetSpecialBoxInfoList(const TAGetListEventReward *pPacket)
{
	ClearSpecialBoxInfoList();
	memcpy(m_SpecialBoxInfo, pPacket->SpecialBoxInfo, sizeof(TSpecialBoxInfo) * pPacket->cCount);
}

void CDNUserItem::ClearSpecialBoxInfoList()
{
	memset(&m_SpecialBoxInfo, 0, sizeof(m_SpecialBoxInfo));
}

TSpecialBoxInfo *CDNUserItem::GetSpecialBoxInfo(int nEventRewardID)
{
	for (int i = 0; i < SpecialBox::Common::ListMax; i++){
		if (m_SpecialBoxInfo[i].nEventRewardID <= 0) continue;
		if (m_SpecialBoxInfo[i].nEventRewardID == nEventRewardID) 
			return &(m_SpecialBoxInfo[i]);
	}

	return NULL;
}

INT64 CDNUserItem::GetSelectSpecialBoxRewardCoin(int nEventRewardID)
{
	TSpecialBoxInfo *pInfo = GetSpecialBoxInfo(nEventRewardID);
	if (!pInfo) return 0;

	return pInfo->biRewardCoin;
}

void CDNUserItem::SetSelectSpecialBoxItems(const TAGetListEventRewardItem *pPacket)
{
	ClearSelectSpecialBoxItems();

	m_SelectSpecialBoxItem = *pPacket;
}

void CDNUserItem::ClearSelectSpecialBoxItems()
{
	memset(&m_SelectSpecialBoxItem, 0, sizeof(m_SelectSpecialBoxItem));
}

int CDNUserItem::CheckReceiveSpecialBox(int nEventRewardID, int nItemID)
{
	TSpecialBoxInfo *pInfo = GetSpecialBoxInfo(nEventRewardID);
	if (!pInfo) return ERROR_ITEM_NOTFOUND;

	if ((pInfo->cTargetClassCode > 0) && (pInfo->cTargetClassCode != m_pSession->GetClassID())) return ERROR_ITEM_FAIL;
	if ((pInfo->cTargetMinLevel > 0) && (pInfo->cTargetMinLevel > m_pSession->GetLevel())) return ERROR_ITEM_FAIL;
	if ((pInfo->cTargetMaxLevel > 0) && (pInfo->cTargetMaxLevel < m_pSession->GetLevel())) return ERROR_ITEM_FAIL;

	std::queue<int> qSlotList;
	int nInvenCount = FindBlankInventorySlotCountList(qSlotList);

	switch (pInfo->cReceiveTypeCode)
	{
	case SpecialBox::ReceiveTypeCode::All:
		{
			if (!m_pSession->CheckMaxCoin(pInfo->biRewardCoin)) return ERROR_ITEM_OVERFLOWMONEY;

			int nItemCount = 0;
			for (int i = 0; i < m_SelectSpecialBoxItem.cCount; i++){
				if (m_SelectSpecialBoxItem.SpecialBoxItem[i].RewardItem.nItemID <= 0) continue;
				if (m_SelectSpecialBoxItem.SpecialBoxItem[i].bCashItem){
					int nRet = _ReceiveSpecialBoxCash(m_SelectSpecialBoxItem.SpecialBoxItem[i].RewardItem.nItemID, m_SelectSpecialBoxItem.SpecialBoxItem[i].RewardItem.cOption, true);
					if (nRet != ERROR_NONE)
						return nRet;
				}
				else{
					nItemCount++;
				}
			}

			if ((nItemCount > 0) && (nInvenCount < nItemCount)){
				return ERROR_ITEM_INVENTORY_NOTENOUGH;
			}
		}
		break;

	case SpecialBox::ReceiveTypeCode::Select:
		{
			int nCount = 0;
			for (int i = 0; i < m_SelectSpecialBoxItem.cCount; i++){
				if (m_SelectSpecialBoxItem.SpecialBoxItem[i].RewardItem.nItemID <= 0) continue;

				if (m_SelectSpecialBoxItem.SpecialBoxItem[i].RewardItem.nItemID == nItemID){
					if (m_SelectSpecialBoxItem.SpecialBoxItem[i].bCashItem){
						int nRet = _ReceiveSpecialBoxCash(m_SelectSpecialBoxItem.SpecialBoxItem[i].RewardItem.nItemID, m_SelectSpecialBoxItem.SpecialBoxItem[i].RewardItem.cOption, true);
						if (nRet != ERROR_NONE)
							return nRet;
					}
					else{
						if (nInvenCount == 0)
							return ERROR_ITEM_INVENTORY_NOTENOUGH;
					}
					nCount++;
				}
			}

			if (nCount == 0)
				return ERROR_ITEM_FAIL;
		}
		break;
	}

	return ERROR_NONE;
}

int CDNUserItem::ReceiveSpecialBox(int nEventRewardID, int nItemID)
{
	if (m_SelectSpecialBoxItem.nEventRewardID != nEventRewardID) return ERROR_ITEM_NOTFOUND;

	TSpecialBoxInfo *pInfo = GetSpecialBoxInfo(nEventRewardID);
	if (!pInfo) return ERROR_ITEM_NOTFOUND;

	int nRet = ERROR_DB;
	switch (pInfo->cReceiveTypeCode)
	{
	case SpecialBox::ReceiveTypeCode::All:
		{
			if (pInfo->biRewardCoin > 0){
				if (!m_pSession->AddCoin(pInfo->biRewardCoin, DBDNWorldDef::CoinChangeCode::SpecialBox, nEventRewardID, true))
					return ERROR_ITEM_OVERFLOWMONEY;
			}

			for (int i = 0; i < m_SelectSpecialBoxItem.cCount; i++){
				if (m_SelectSpecialBoxItem.SpecialBoxItem[i].RewardItem.nItemID <= 0) continue;

				if (m_SelectSpecialBoxItem.SpecialBoxItem[i].bCashItem){
					nRet = _ReceiveSpecialBoxCash(m_SelectSpecialBoxItem.SpecialBoxItem[i].RewardItem.nItemID, m_SelectSpecialBoxItem.SpecialBoxItem[i].RewardItem.cOption, false);
					if (nRet != ERROR_NONE)
						return nRet;
				}
				else{
					m_SelectSpecialBoxItem.SpecialBoxItem[i].RewardItem.nSerial = MakeItemSerial();
					nRet = CreateInvenWholeItem(m_SelectSpecialBoxItem.SpecialBoxItem[i].RewardItem, DBDNWorldDef::AddMaterializedItem::SpecialBoxReward, nEventRewardID, CREATEINVEN_ETC);
					if (nRet != ERROR_NONE)
						return nRet;
				}
			}
		}
		break;

	case SpecialBox::ReceiveTypeCode::Select:
		{
			if (nItemID <= 0) return ERROR_ITEM_NOTFOUND;

			for (int i = 0; i < m_SelectSpecialBoxItem.cCount; i++){
				if (m_SelectSpecialBoxItem.SpecialBoxItem[i].RewardItem.nItemID <= 0) continue;

				if (m_SelectSpecialBoxItem.SpecialBoxItem[i].RewardItem.nItemID == nItemID){
					if (m_SelectSpecialBoxItem.SpecialBoxItem[i].bCashItem){
						nRet = _ReceiveSpecialBoxCash(m_SelectSpecialBoxItem.SpecialBoxItem[i].RewardItem.nItemID, m_SelectSpecialBoxItem.SpecialBoxItem[i].RewardItem.cOption, false);
						if (nRet != ERROR_NONE)
							return nRet;
					}
					else{
						m_SelectSpecialBoxItem.SpecialBoxItem[i].RewardItem.nSerial = MakeItemSerial();
						nRet = CreateInvenWholeItem(m_SelectSpecialBoxItem.SpecialBoxItem[i].RewardItem, DBDNWorldDef::AddMaterializedItem::SpecialBoxReward, nEventRewardID, CREATEINVEN_ETC);
						if (nRet != ERROR_NONE)
							return nRet;
					}

					break;
				}
			}
		}
		break;
	}

	ClearSelectSpecialBoxItems();

	return ERROR_NONE;
}

int CDNUserItem::_ReceiveSpecialBoxCash(int nItemSN, int nOption, bool bCheckCondition)
{
	std::vector<TEffectItemData> VecEffectItemList, VecCashList;
	VecEffectItemList.clear();
	VecCashList.clear();

	int nRet = 0;

	TCashPackageData PackageData;
	bool bPackage = g_pDataManager->GetCashPackageData(nItemSN, PackageData);
	if (bPackage){	// 패키지일때
		TGiftItem AddItemList[PACKAGEITEMMAX] = {0, };

		for (int i = 0; i < (int)PackageData.nVecCommoditySN.size(); i++){
			int nCashItemSN = PackageData.nVecCommoditySN[i];
			int nCashItemID = g_pDataManager->GetCashCommodityItem0(PackageData.nVecCommoditySN[i]);

			TEffectItemData CashInfo = {0,};
			CashInfo.nItemSN = nCashItemSN;
			CashInfo.nItemID = nCashItemID;
			CashInfo.nCount = g_pDataManager->GetCashCommodityCount(nCashItemSN);
			VecCashList.push_back(CashInfo);

			if (IsEffectCashItem(nCashItemID)){	// 무형아이템이라면
				VecEffectItemList.push_back(CashInfo);
			}

			if (!bCheckCondition){
				nRet = MakeGiftCashItem(nCashItemSN, nCashItemID, nOption, AddItemList[i]);
				if (nRet != ERROR_NONE){
					return nRet;
				}
			}
			else{
				switch (g_pDataManager->GetItemMainType(nCashItemID))
				{
				case ITEMTYPE_CHARACTER_SLOT:
					{
#if defined(PRE_MOD_SELECT_CHAR)
						if (m_SelectSpecialBoxItem.cCharacterCount + CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::Login_Base_CreateCharCount) >= CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::Login_Max_CreateCharCount))
#else	// #if defined(PRE_MOD_SELECT_CHAR)
						if (m_SelectSpecialBoxItem.cCharacterCount >= (CreateCharacterDefaultCountMax + CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::CharacterSlotMax)))
#endif	// #if defined(PRE_MOD_SELECT_CHAR)
							return 101190;	// 101190 = 최대 소유할 수 있는 캐릭터 슬롯 수를 초과합니다.
					}
					break;
				}
			}
		}

		if (!VecEffectItemList.empty()){
			nRet = CheckEffectItemListCountLimit(VecEffectItemList, true, false, false);	// 캐쉬템 몇개까지 살 수 있는지 max검사
			if (nRet != ERROR_NONE){
				return nRet;
			}
		}

		if (!VecCashList.empty()){
			nRet = CheckCashDuplicationBuy(VecCashList, false);
			if (nRet != ERROR_NONE){
				return nRet;
			}
		}

		if (!bCheckCondition){
			for (int i = 0; i < (int)PackageData.nVecCommoditySN.size(); i++){
				ApplyCashShopItem(AddItemList[i].nItemSN, AddItemList[i].AddItem, AddItemList[i].dwPartsColor1, AddItemList[i].dwPartsColor2, &(AddItemList[i].VehiclePart1), &(AddItemList[i].VehiclePart2), true);
			}
		}
	}
	else{	// 패키지 아닐때
		if (nItemSN <= 0){
			return ERROR_ITEM_FAIL;
		}

		int nItemID = g_pDataManager->GetCashCommodityItem0(nItemSN);

		TEffectItemData CashInfo = {0,};
		CashInfo.nItemSN = nItemSN;
		CashInfo.nItemID = nItemID;
		CashInfo.nCount = g_pDataManager->GetCashCommodityCount(nItemSN);
		VecCashList.push_back(CashInfo);

		if (IsEffectCashItem(nItemID)){	// 무형아이템이라면
			VecEffectItemList.push_back(CashInfo);
		}

		if (!VecEffectItemList.empty()){
			nRet = CheckEffectItemListCountLimit(VecEffectItemList, true, false, false);	// 무형 캐쉬템 몇개까지 살 수 있는지 max검사
			if (nRet != ERROR_NONE){
				return nRet;
			}
		}

		if (!VecCashList.empty()){
			nRet = CheckCashDuplicationBuy(VecCashList, false);
			if (nRet != ERROR_NONE){
				return nRet;
			}
		}

		if (!bCheckCondition){
			TGiftItem AddCashItem = { 0, };
			nRet = MakeGiftCashItem(nItemSN, nItemID, nOption, AddCashItem);
			if (nRet != ERROR_NONE){
				return nRet;
			}

			ApplyCashShopItem(AddCashItem.nItemSN, AddCashItem.AddItem, AddCashItem.dwPartsColor1, AddCashItem.dwPartsColor2, &(AddCashItem.VehiclePart1), &(AddCashItem.VehiclePart2), true);
		}
		else{
			switch (g_pDataManager->GetItemMainType(nItemID))
			{
			case ITEMTYPE_CHARACTER_SLOT:
				{
#if defined(PRE_MOD_SELECT_CHAR)
					if (m_SelectSpecialBoxItem.cCharacterCount + CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::Login_Base_CreateCharCount) >= CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::Login_Max_CreateCharCount))
#else	// #if defined(PRE_MOD_SELECT_CHAR)
					if (m_SelectSpecialBoxItem.cCharacterCount >= (CreateCharacterDefaultCountMax + CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::CharacterSlotMax)))
#endif	// #if defined(PRE_MOD_SELECT_CHAR)
						return 101190;	// 101190 = 최대 소유할 수 있는 캐릭터 슬롯 수를 초과합니다.
				}
				break;
			}
		}
	}

	return ERROR_NONE;
}
#endif	// #if defined(PRE_SPECIALBOX)

// market
int CDNUserItem::CheckRegisterMarketItem(CSMarketRegister *pPacket)
{
	bool bPremiumTrade = (GetCashItemCountByType( ITEMTYPE_PREMIUM_TRADE ) > 0);
	if ( pPacket->bPremiumTrade == true && bPremiumTrade == false )	return ERROR_MARKET_CANNOTUSE_PREMIUM;
	if ( CheckRegisterMarketItemCount( bPremiumTrade ) == false ) return ERROR_MARKET_COUNTOVER;

	if ((pPacket->wCount <= 0) ||(pPacket->nPrice <= 0)) return ERROR_ITEM_FAIL;

	int ret = IsTradeEnableItem(pPacket->cInvenType, pPacket->cInvenIndex, pPacket->biInvenSerial, pPacket->wCount);
	if (ret != ERROR_NONE)
		return ret;

	const TItem *pItem = NULL;
	switch(pPacket->cInvenType)
	{
	case ITEMPOSITION_INVEN:
		{
			pItem = GetInventory(pPacket->cInvenIndex);
			if (!pItem) return ERROR_ITEM_NOTFOUND;

			if (pItem->nSerial != pPacket->biInvenSerial) return ERROR_ITEM_FAIL;
		}
		break;

	case ITEMPOSITION_CASHINVEN:
		{
			pItem = GetCashInventory(pPacket->biInvenSerial);
			if (!pItem) return ERROR_ITEM_NOTFOUND;
			if (pItem->wCount != pPacket->wCount) return ERROR_MARKET_REGISTITEMFAIL;
#if defined(_KR) || defined(_KRAZ) || defined(_TW) || defined(_CH)
			if (pItem->cSealCount <= 0) return ERROR_MARKET_REGISTITEMFAIL;	// 아이템 거래 가능 회수가 0이다. 거래불가.
#endif	// #if defined(_JP)
		}
		break;

	default:
		{
			if (!pItem) return ERROR_ITEM_NOTFOUND;
		}
		break;
	}

	TItemData *pItemData= g_pDataManager->GetItemData(pItem->nItemID);
	if (!pItemData) return ERROR_ITEM_NOTFOUND;
	if (pItemData->nType == ITEMTYPE_QUEST) return ERROR_MARKET_REGISTITEMFAIL;

	if (pItemData->nType == ITEMTYPE_PETALTOKEN){		// 페탈상품권이라면
#if defined(_KR) || defined(_KRAZ)
		if (!bPremiumTrade) return ERROR_MARKET_REGISTPREMIUM;
#elif defined(_CH)
		//if (!m_pSession->IsVIP()) return ERROR_MARKET_REGISTVIP;
#endif
#if defined(PRE_FIX_34367)	
		if( pPacket->cPayMethodCode == DBDNWorldDef::PayMethodCode::Petal)
		{
			// 페탈 상품권은 페탈로 등록이 안되게 막는다.
			return ERROR_ITEM_NOTFOUND;
		}	
#endif
	}

	int nPrice = pPacket->nPrice;
	if (nPrice >= 1000000000) nPrice = 1000000000;	// max 10억으로 한다

	int nCharge = 0;

#if defined(PRE_ADD_PETALTRADE)
	if ( pPacket->cPayMethodCode == DBDNWorldDef::PayMethodCode::Coin )		
#endif 
	{
		nCharge = (int)(g_pDataManager->CalcTax(TAX_TRADEREGISTER, m_pSession->GetLevel(), nPrice));
		if (nCharge <= 0) nCharge = 1;	// 최소 1로 수수료 뗀다.
		if ( bPremiumTrade )
		{	// 새로운 수수로 비율로 계산한다.
			nCharge = (int)(g_pDataManager->CalcTax(TAX_TRADEREGISTER_CASH, m_pSession->GetLevel(), nPrice));
			if (nCharge <= 0) nCharge = 0;	// 최소 0로 수수료 뗀다.
		}
	}

	switch(pPacket->cInvenType)
	{
	case ITEMPOSITION_INVEN:
		if (pItemData->IsCash) return ERROR_MARKET_REGISTITEMFAIL;	// 일반인벤일때 캐쉬템 올라오면 안된다
		break;

	case ITEMPOSITION_CASHINVEN:
		if (!pItemData->IsCash) return ERROR_MARKET_REGISTITEMFAIL;	// 캐쉬인벤일때 일반템 올라오면 안된다
#if !defined(_JP)
		// 거래가능 회수 빼주기
		const_cast<TItem*>(pItem)->cSealCount -= 1;
		if (pItem->cSealCount < 0) 
			const_cast<TItem*>(pItem)->cSealCount = 0;
#endif	// #if defined(_JP)
		break;
	}

	// 수수료가 충분치 못하다면
	if (!m_pSession->CheckEnoughCoin(nCharge)) return ERROR_ITEM_INSUFFICIENCY_MONEY;

	int nItemNeedJob = 0;
	if (pItemData->nNeedJobClassList.size() == 1) nItemNeedJob = pItemData->nNeedJobClassList[0];
	else nItemNeedJob = JOB_NONE;

	int nTaxType = TAX_TRADESELLING;
#if defined(PRE_ADD_PETALTRADE)
	if ( pPacket->cPayMethodCode == DBDNWorldDef::PayMethodCode::Petal)
		nTaxType = TAX_TRADEPETALSELLING;
#endif
	int nSellTax = (int)(g_pDataManager->CalcTax(nTaxType, m_pSession->GetLevel(), nPrice));	
	if (nSellTax <= 0) nSellTax = 1;	// 최소 1로 수수료 뗀다.

	TItem Register = *pItem;
	INT64 biNewSerial = 0;
	if (Register.wCount > pPacket->wCount){	// 수량아이템중 뗘서 보낼경우 시리얼을 새로 발급해서 보내야한다
		biNewSerial = MakeItemSerial();
	}
	Register.wCount = pPacket->wCount;

	int nPeriod = (int)(CGlobalWeightTable::GetInstance().GetValue((CGlobalWeightTable::WeightTableIndex)pPacket->cPeriodIndex));
	if ( CheckRegisterMarketPeriod( nPeriod ) == false )
	{
		return ERROR_MARKET_INVALIDPERIOD;
	}

	int nMaxItemCount = GetMarketMaxRegisterItemCount();

	if (pItemData->nExchangeCode <=0 ) return ERROR_MARKET_REGISTITEMFAIL;

	char cPayMethodCode = DBDNWorldDef::PayMethodCode::Coin;
#if defined( PRE_ADD_PETALTRADE)
	cPayMethodCode = pPacket->cPayMethodCode;
#endif
	m_pSession->GetDBConnection()->QueryAddTrade(m_pSession, Register, biNewSerial, pPacket->cInvenIndex, nPrice, nCharge, nSellTax, nPeriod, false, false, pItemData->wszItemName, pItemData->cRank, pItemData->cLevelLimit, 
		nItemNeedJob, pPacket->cInvenType, bPremiumTrade, nMaxItemCount, pItemData->IsCash, cPayMethodCode, pItemData->nExchangeCode);

	return ERROR_NONE;
}

int CDNUserItem::GetMarketMaxRegisterItemCount()
{
	int nRetMaxCount = 0;

	// 캐시 아이템 인벤토리에서 무인 거래소 캐시아이템을 찾는다.
	for(TMapItem::iterator iter = m_MapCashInventory.begin(); iter != m_MapCashInventory.end(); iter++ )
	{
		TItemData *pItemData = g_pDataManager->GetItemData( iter->second.nItemID );
		if( pItemData == NULL )	continue;
		if (IsExpired(iter->second)) continue;

		if( pItemData->nType == ITEMTYPE_PREMIUM_TRADE )
		{
			nRetMaxCount += pItemData->nTypeParam[1];		// 동시 등록 개수
			break;	//	#61925 이슈로 이용권 중복 적용 안되게 수정(전국가 적용이므로 따로 디파인 안함)
		}
	}

	int nRegisterItemCount = (int)(CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::MarketItemRegisterCount_Basic));
	nRetMaxCount += nRegisterItemCount;

	return nRetMaxCount;
}

bool CDNUserItem::CheckRegisterMarketItemCount( bool bPremiumTrade )
{
	int nMaxRegisterItemCount = 0;
	nMaxRegisterItemCount = GetMarketMaxRegisterItemCount();

	// 무인거래소 프리미엄 아이템을 가지고 있으면 주간 등록회수 제한을 받지 않는다.
	if( bPremiumTrade == false )
		if (m_nWeeklyRegisterCount >= (int)(CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::MarketRegisterCount_Basic)))	return false;

	if (m_nRegisterItemCount >= nMaxRegisterItemCount)	return false;

	return true;
}

bool CDNUserItem::CheckRegisterMarketPeriod( int nPeriod )
{
	// 기본 기간은 아이템과 상관없이 사용이 가능하다.
	if( nPeriod == (int)(CGlobalWeightTable::GetInstance().GetValue((CGlobalWeightTable::WeightTableIndex)CGlobalWeightTable::MarketRegisterTime0)) )
		return true;

	// 캐시 아이템 인벤토리에서 무인 거래소 캐시아이템을 찾는다.
	for(TMapItem::iterator iter = m_MapCashInventory.begin(); iter != m_MapCashInventory.end(); iter++ )
	{
		TItemData *pItemData = g_pDataManager->GetItemData( iter->second.nItemID );
		if( pItemData == NULL ) continue;
		if (IsExpired(iter->second)) continue;

		if( pItemData->nType == ITEMTYPE_PREMIUM_TRADE )
		{
			int GlobalWeightTableIndex = pItemData->nTypeParam[0] - 1;
			if( GlobalWeightTableIndex < 0 || GlobalWeightTableIndex >= CGlobalWeightTable::Amount ) continue;

			int nItemPeriod = (int)(CGlobalWeightTable::GetInstance().GetValue((CGlobalWeightTable::WeightTableIndex)GlobalWeightTableIndex));
			if( nItemPeriod == nPeriod )
				return true;
		}
	}

	return false;
}

int CDNUserItem::CancelMarketItem(int nMarketDBID)
{
	if (nMarketDBID <= 0) return ERROR_ITEM_FAIL;
	int nInvenIndex = -1;

	bool bCash = IsCashMyMarketItem(nMarketDBID);
	if (bCash){
		nInvenIndex = 0;
	}
	else{
		std::queue<int> qSlotList;
		int nInvenCount = FindBlankInventorySlotCountList(qSlotList);
		if (qSlotList.empty()) return ERROR_ITEM_INVENTORY_NOTENOUGH;
		if (nInvenCount != qSlotList.size()) return ERROR_ITEM_FAIL;

		nInvenIndex = qSlotList.front();
	}

	m_pSession->GetDBConnection()->QueryCancelTrade(m_pSession, nMarketDBID, nInvenIndex, bCash);

	return ERROR_NONE;
}

#if defined(PRE_ADD_DIRECT_BUY_UPGRADEITEM)
int CDNUserItem::BuyMarketItem(int nMarketDBID, bool bMini)
{
	if (nMarketDBID <= 0) return ERROR_ITEM_FAIL;

	int nInvenIndex = -1;
	std::queue<int> qSlotList;
	int nInvenCount = FindBlankInventorySlotCountList(qSlotList);

	if (nInvenCount != qSlotList.size()) return ERROR_ITEM_FAIL;
	if (!qSlotList.empty())
		nInvenIndex = qSlotList.front();

	m_pSession->GetDBConnection()->QueryBuyTrade(m_pSession, nMarketDBID, nInvenIndex, bMini);

	return ERROR_NONE;
}

#else	// #if defined(PRE_ADD_DIRECT_BUY_UPGRADEITEM)
#if defined(PRE_ADD_PETALTRADE)
int CDNUserItem::BuyMarketItem(int nMarketDBID, int nItemID, int nPetalPrice)
#else
int CDNUserItem::BuyMarketItem(int nMarketDBID, int nItemID)
#endif
{
	if (nMarketDBID <= 0) return ERROR_ITEM_FAIL;
	if (nItemID <= 0) return ERROR_ITEM_FAIL;

	bool bCash = g_pDataManager->IsCashItem(nItemID);
	int nInvenIndex = 0;
	if (!bCash){
		nInvenIndex = -1;

		std::queue<int> qSlotList;
		int nInvenCount = FindBlankInventorySlotCountList(qSlotList);
		if (qSlotList.empty()) return ERROR_ITEM_INVENTORY_NOTENOUGH;
		if (nInvenCount != qSlotList.size()) return ERROR_ITEM_INVENTORY_NOTENOUGH;

		nInvenIndex = qSlotList.front();
	}

#if defined(PRE_ADD_PETALTRADE)
	m_pSession->GetDBConnection()->QueryBuyTrade(m_pSession, nMarketDBID, nInvenIndex, bCash, nPetalPrice);
#else
	m_pSession->GetDBConnection()->QueryBuyTrade(m_pSession, nMarketDBID, nInvenIndex, bCash, 0);
#endif

	return ERROR_NONE;
}
#endif	// #if defined(PRE_ADD_DIRECT_BUY_UPGRADEITEM)

void CDNUserItem::SetRegisterMarketCount(int nWeeklyRegisterCount, int nRegisterItemCount)
{
	if (nWeeklyRegisterCount < 0) nWeeklyRegisterCount = 0;
	int nWeeklyCount = (int)(CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::MarketRegisterCount_Basic));
	if (nWeeklyRegisterCount > nWeeklyCount) nWeeklyRegisterCount = nWeeklyCount;
	m_nWeeklyRegisterCount = nWeeklyRegisterCount;

	if (nRegisterItemCount < 0) nRegisterItemCount = 0;
	int nRegisterCount = (int)(CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::MarketItemRegisterCount_Basic));
	if (nRegisterItemCount > nRegisterCount) nRegisterItemCount = nRegisterCount;
	m_nRegisterItemCount = nRegisterItemCount;
}

void CDNUserItem::AddRegisterItemCount()
{
	int nCount = (int)(CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::MarketItemRegisterCount_Basic));
	if (m_nRegisterItemCount == nCount) return;

	m_nRegisterItemCount += 1;
	if (m_nRegisterItemCount > nCount) m_nRegisterItemCount = nCount;
}

void CDNUserItem::DelRegisterItemCount()
{
	if (m_nRegisterItemCount == 0) return;

	// 프리미엄 여부 확인
	bool bPremiumTrade = ( GetCashItemCountByType( ITEMTYPE_PREMIUM_TRADE ) > 0 ) ? true : false;

	// 판매 카운트 조사
	int nSellingCount = (int)m_VecMyMarketList.size();

	// 프리미엄일 경우
	if (bPremiumTrade)
	{
		// 이 함수 호출후 DelMyMarketList이 호출되기 떄문에 미리 1을 빼준다.
		m_nRegisterItemCount = nSellingCount - 1;
	}
	else	// 프리미엄이 아닐경우
	{
		// 등록수가 판매수보다 크거나 같다면 감소시킨다
		if (m_nRegisterItemCount >= nSellingCount)
			m_nRegisterItemCount -= 1;
	}
	
	if (m_nRegisterItemCount < 0) m_nRegisterItemCount = 0;
}

void CDNUserItem::AddWeeklyRegisterCount()
{
	int nCount = (int)(CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::MarketRegisterCount_Basic));
	if (m_nWeeklyRegisterCount == nCount) return;

	m_nWeeklyRegisterCount += 1;
	if (m_nWeeklyRegisterCount > nCount) m_nWeeklyRegisterCount = nCount;
}

// Exchange
int CDNUserItem::AddExchangeItem(CSExchangeAddItem *pExchange)
{
	if (!IsValidInventorySlot(pExchange->cInvenIndex, pExchange->biItemSerial)) return ERROR_ITEM_INDEX_UNMATCH;							// index 검사
	if (pExchange->cExchangeIndex >= EXCHANGEMAX) return ERROR_ITEM_INDEX_UNMATCH;	// 개인거래창 인덱스 검사
	if (m_ExchangeData[pExchange->cExchangeIndex].wCount > 0) return ERROR_ITEM_ALREADY_EXIST;		// 이미 아이템이 있다
	if (_ExistExchangeInven(pExchange->cInvenIndex)) return ERROR_ITEM_ALREADY_EXIST;		// 이미 같은 인벤인덱스의 아이템이 있다	
	int nRet = IsTradeEnableItem(ITEMPOSITION_INVEN, pExchange->cInvenIndex, pExchange->biItemSerial, pExchange->wCount);
	if (nRet != ERROR_NONE)
		return nRet;

	if (m_Inventory[pExchange->cInvenIndex].nSerial != pExchange->biItemSerial) return ERROR_ITEM_FAIL;
	if ((m_Inventory[pExchange->cInvenIndex].nItemID > 0) && (g_pDataManager->GetItemMainType(m_Inventory[pExchange->cInvenIndex].nItemID) == ITEMTYPE_QUEST)) return ERROR_EXCHANGE_ITEMFAIL;

	m_ExchangeData[pExchange->cExchangeIndex].nInvenIndex = pExchange->cInvenIndex;
	m_ExchangeData[pExchange->cExchangeIndex].wCount = pExchange->wCount;
	m_ExchangeData[pExchange->cExchangeIndex].biSerial = pExchange->biItemSerial;

	return ERROR_NONE;
}

int CDNUserItem::DeleteExchangeItem(char cExchangeIndex)
{
	if ((cExchangeIndex < 0) ||(cExchangeIndex >= EXCHANGEMAX)){
		return ERROR_ITEM_INDEX_UNMATCH;
	}

	m_ExchangeData[cExchangeIndex].nInvenIndex = -1;
	m_ExchangeData[cExchangeIndex].wCount = 0;
	m_ExchangeData[cExchangeIndex].biSerial = 0;

	return ERROR_NONE;
}

int CDNUserItem::AddExchangeCoin(INT64 nCoin)
{
	if ((nCoin < 0) ||(nCoin > COINMAX)) return ERROR_ITEM_FAIL;

	m_nExchangeCoin = nCoin;
	return ERROR_NONE;
}

void CDNUserItem::ClearExchangeData()
{
	for (int i = 0; i < EXCHANGEMAX; i++){
		m_ExchangeData[i].nInvenIndex = -1;
		m_ExchangeData[i].wCount = 0;
		m_ExchangeData[i].biSerial = 0;
	}

	m_nExchangeCoin = 0;
	m_bExchangeConfirm = m_bExchangeRegist = false;
}

bool CDNUserItem::IsValidExchange(int nExchangeCount)
{
	if (nExchangeCount > FindBlankInventorySlotCount()) return false;
	return true;
}

int CDNUserItem::FindExchangeCount()
{
	int nCount = 0;
	for (int i = 0; i < EXCHANGEMAX; i++){
		if (m_ExchangeData[i].nInvenIndex == -1) continue;
		nCount++;
	}

	return nCount;
}

INT64 CDNUserItem::GetExchangeCoin()
{
	if (m_nExchangeCoin < 0) m_nExchangeCoin = 0;
	if (m_nExchangeCoin > COINMAX) m_nExchangeCoin = COINMAX;

	return m_nExchangeCoin;
}

bool CDNUserItem::CompleteExchange(CDNUserSession *pTargetSession, DBPacket::TExchangeItem *ExchangeInfo, int &nExchangeTax)	// 거래 완료하기(내 아이템 상대방 인벤으로 넣어주기)
{
	// 수수료 계산
	INT64 nTotalPrice = 0;
	int nExchangeItemCount = 0;
	for (int i = 0; i < EXCHANGEMAX; i++){
		if (m_ExchangeData[i].nInvenIndex == -1) continue;
		nTotalPrice += (g_pDataManager->GetItemPrice(m_Inventory[m_ExchangeData[i].nInvenIndex].nItemID) * m_ExchangeData[i].wCount);
		nExchangeItemCount++;
	}
	nTotalPrice += m_nExchangeCoin;

	INT64 nTax = g_pDataManager->CalcTax(TAX_DEAL, m_pSession->GetLevel(), nTotalPrice);
	if (!m_pSession->CheckEnoughCoin(nTax)) return false;	// 수수료가 없어서 교환취소

	if (nExchangeItemCount > 0){
		TItem InvenItem = { 0, };
		TItem BackupItem = { 0, };	// 제대로 동작 안됏을때 다시 넣어주는 아이템

		char cGrade = 0;
		TItemData *pItemData = NULL;

		std::queue<int> qSlotList;
		int nInvenCount = pTargetSession->GetItem()->FindBlankInventorySlotCountList(qSlotList);	// 상대방 인벤 빈슬롯 찾기
		if (qSlotList.empty()) return false;
		if (nInvenCount != qSlotList.size()) return false;

		for (int i = 0; i < EXCHANGEMAX; i++){
			if (m_ExchangeData[i].nInvenIndex == -1) continue;
#if defined(PRE_PERIOD_INVENTORY)
			if (_CheckRangePeriodInventoryIndex(m_ExchangeData[i].nInvenIndex) == false) continue;
#endif	// #if defined(PRE_PERIOD_INVENTORY)
			// 내 인벤에서 아이템 얻기
			InvenItem = m_Inventory[m_ExchangeData[i].nInvenIndex];
			BackupItem = InvenItem;
			InvenItem.wCount = m_ExchangeData[i].wCount;

			pItemData = g_pDataManager->GetItemData(InvenItem.nItemID);
			if (pItemData) cGrade = pItemData->cRank;

			// 내 인벤에서 삭제
			INT64 biTempSerial = m_Inventory[m_ExchangeData[i].nInvenIndex].nSerial;
	
			if (DeleteInventoryBySlot(m_ExchangeData[i].nInvenIndex, m_ExchangeData[i].wCount, m_ExchangeData[i].biSerial, DBDNWorldDef::UseItem::DoNotDBSave)){
				// 내 인벤에서 아이템이 제대로 사라졌다면 상대방 인벤에 추가
				if (!qSlotList.empty()){
					ExchangeInfo[i].cSlotIndex = qSlotList.front();
					qSlotList.pop();
					ExchangeInfo[i].nItemID = InvenItem.nItemID;
					ExchangeInfo[i].wCount = InvenItem.wCount;
					ExchangeInfo[i].biSerial = InvenItem.nSerial;
					if (m_Inventory[m_ExchangeData[i].nInvenIndex].wCount > 0)
						InvenItem.nSerial = MakeItemSerial();
					else{
						InvenItem.nSerial = biTempSerial;
					}
					ExchangeInfo[i].biNewSerial = InvenItem.nSerial;

					if (pTargetSession->GetItem()->CreateInvenWholeItemByIndex(ExchangeInfo[i].cSlotIndex, InvenItem) != ERROR_NONE){
						// 상대방 인벤에 넣는거 실패하면 다시 내 인벤에 복구
						m_Inventory[m_ExchangeData[i].nInvenIndex] = BackupItem;
					}
				}
				else{
					// 상대방 인벤에 넣는거 실패하면 다시 내 인벤에 복구
					m_Inventory[m_ExchangeData[i].nInvenIndex] = BackupItem;
				}
			}
		}
	}

	if ((m_nExchangeCoin > 0) && m_pSession->CheckEnoughCoin(m_nExchangeCoin)){
		m_pSession->DelCoin(m_nExchangeCoin, DBDNWorldDef::CoinChangeCode::DoNotDBSave, 0);	// 내돈 빼고 
		pTargetSession->AddCoin(m_nExchangeCoin, DBDNWorldDef::CoinChangeCode::DoNotDBSave, 0);	// 상대방에 돈 넣어주고
	}

	if (nTax > 0){	// 수수료 뺏는다
		nExchangeTax = (int)nTax;
		m_pSession->DelCoin(nTax, DBDNWorldDef::CoinChangeCode::DoNotDBSave, 0, true);
	}

	m_pSession->GetEventSystem()->OnEvent( EventSystem::OnExchange );
	return true;
}

void CDNUserItem::SetEquipItemDurability( int nIndex, USHORT wDur, bool bSend/*=false*/ )
{
	m_Equip[nIndex].wDur = wDur;

	if (m_Equip[nIndex].wDur < 0) m_Equip[nIndex].wDur = 0;
	int nMaxDur = g_pDataManager->GetItemDurability(m_Equip[nIndex].nItemID);
	if (m_Equip[nIndex].wDur > nMaxDur) m_Equip[nIndex].wDur = nMaxDur;

	if( bSend == true )
	{
		m_pSession->SendRefreshEquip( nIndex, &m_Equip[nIndex] );
	}
}

void CDNUserItem::SetInvenItemDurability( int nIndex, USHORT wDur )
{
#if defined(PRE_PERIOD_INVENTORY)
	if (_CheckRangePeriodInventoryIndex(nIndex) == false) return;
#endif	// #if defined(PRE_PERIOD_INVENTORY)
	m_Inventory[nIndex].wDur = wDur;
	if (m_Inventory[nIndex].wDur < 0) m_Inventory[nIndex].wDur = 0;
	int nMaxDur = g_pDataManager->GetItemDurability(m_Inventory[nIndex].nItemID);
	if (m_Inventory[nIndex].wDur > nMaxDur) m_Inventory[nIndex].wDur = nMaxDur;
}

int CDNUserItem::CheckUpgradeEnchant(CSEnchantItem *pEnchant)
{
#if defined(PRE_ADD_EQUIPED_ITEM_ENCHANT)
	TItem *pItem = NULL;
	if( pEnchant->bEnchantEquipedItem )
	{
		if(!IsValidEquipSlot(pEnchant->cItemIndex, pEnchant->biItemSerial)) return ERROR_ITEM_NOTFOUND;
#if defined(PRE_ADD_EQUIPLOCK)
		if(IsLockItem(DBDNWorldDef::ItemLocation::Equip, pEnchant->cItemIndex)) return ERROR_ITEM_LOCKITEM_NOTUPGRADE;
#endif	// #if defined(PRE_ADD_EQUIPLOCK)
		pItem = &m_Equip[pEnchant->cItemIndex];		
	}
	else
	{
		if (!IsValidInventorySlot(pEnchant->cItemIndex, pEnchant->biItemSerial)) return ERROR_ITEM_INDEX_UNMATCH;	// 인덱스가 맞지않다
		if (m_Inventory[pEnchant->cItemIndex].wCount != 1) return ERROR_ITEM_ENCHANTFAIL;
		pItem = &m_Inventory[pEnchant->cItemIndex];
	}
	if( !pItem )  return ERROR_ITEM_NOTFOUND;
	TItemData *pItemData = g_pDataManager->GetItemData(pItem->nItemID);
	if (!pItemData) return ERROR_ITEM_NOTFOUND;

	if (pItemData->nEnchantID <= 0) return ERROR_ITEM_ENCHANTFAIL;	// 인챈트 아이템 아님
	if (pItem->cLevel >= g_pDataManager->GetEnchantMaxLevel(pItemData->nEnchantID)) return ERROR_ITEM_ENCHANTFAIL;	// 인챈트 최대레벨 넘어감
	if (pItemData->cReversion == ITEMREVERSION_BELONG && pItem->bSoulbound == false) return ERROR_ITEM_ENCHANTFAIL;	// 봉인아이템

#ifdef PRE_FIX_MEMOPT_ENCHANT
	const TEnchantLevelData* pEnchantLevelData = g_pDataManager->GetEnchantLevelData(pItemData->nEnchantID, pItem->cLevel + 1);
	if (pEnchantLevelData == NULL)
		return ERROR_ITEM_ENCHANTFAIL;

	const TEnchantNeedItemData* pEnchantNeedItemData = g_pDataManager->GetEnchantNeedItemData(*pEnchantLevelData);
	if (!pEnchantNeedItemData)
		return ERROR_ITEM_NOTFOUND;
	if (pEnchantNeedItemData->cEnchantProb == 0)
		return ERROR_ITEM_ENCHANTFAIL;
#else
	TEnchantLevelData *pEnchantLevelData = g_pDataManager->GetEnchantLevelData(pItemData->nEnchantID, pItem->cLevel + 1);	// 강화할 아이템데이터 찾기
	if (!pEnchantLevelData) return ERROR_ITEM_NOTFOUND;
	if (pEnchantLevelData->cEnchantProb == 0) return ERROR_ITEM_ENCHANTFAIL;
#endif //#ifdef PRE_FIX_MEMOPT_ENCHANT
#else // #if defined(PRE_ADD_EQUIPED_ITEM_ENCHANT)	
	if (!IsValidInventorySlot(pEnchant->cInvenIndex, pEnchant->biItemSerial)) return ERROR_ITEM_INDEX_UNMATCH;	// 인덱스가 맞지않다
	if (m_Inventory[pEnchant->cInvenIndex].nSerial != pEnchant->biItemSerial) return ERROR_ITEM_NOTFOUND;
	if (m_Inventory[pEnchant->cInvenIndex].wCount != 1) return ERROR_ITEM_ENCHANTFAIL;

	TItemData *pItemData = g_pDataManager->GetItemData(m_Inventory[pEnchant->cInvenIndex].nItemID);
	if (!pItemData) return ERROR_ITEM_NOTFOUND;

	if (pItemData->nEnchantID <= 0) return ERROR_ITEM_ENCHANTFAIL;	// 인챈트 아이템 아님
	if (m_Inventory[pEnchant->cInvenIndex].cLevel >= g_pDataManager->GetEnchantMaxLevel(pItemData->nEnchantID)) return ERROR_ITEM_ENCHANTFAIL;	// 인챈트 최대레벨 넘어감
	if (pItemData->cReversion == ITEMREVERSION_BELONG && m_Inventory[pEnchant->cInvenIndex].bSoulbound == false) return ERROR_ITEM_ENCHANTFAIL;	// 봉인아이템

#ifdef PRE_FIX_MEMOPT_ENCHANT
	const TEnchantLevelData* pEnchantLevelData = g_pDataManager->GetEnchantLevelData(pItemData->nEnchantID, m_Inventory[pEnchant->cInvenIndex].cLevel + 1);
	if (pEnchantLevelData == NULL)
		return ERROR_ITEM_ENCHANTFAIL;

	const TEnchantNeedItemData* pEnchantNeedItemData = g_pDataManager->GetEnchantNeedItemData(*pEnchantLevelData);
	if (!pEnchantNeedItemData)
		return ERROR_ITEM_NOTFOUND;
	if (pEnchantNeedItemData->cEnchantProb == 0)
		return ERROR_ITEM_ENCHANTFAIL;
#else
	TEnchantLevelData *pEnchantLevelData = g_pDataManager->GetEnchantLevelData(pItemData->nEnchantID, m_Inventory[pEnchant->cInvenIndex].cLevel + 1);	// 강화할 아이템데이터 찾기
	if (!pEnchantLevelData) return ERROR_ITEM_NOTFOUND;
	if (pEnchantLevelData->cEnchantProb == 0) return ERROR_ITEM_ENCHANTFAIL;
#endif // #ifdef PRE_FIX_MEMOPT_ENCHANT
#endif // #if defined(PRE_ADD_EQUIPED_ITEM_ENCHANT)	

#if defined(PRE_ADD_ENCHANTSHIELD_CASHITEM)
	if ( pEnchant->cEnchantGuardType > 0 )
	{
		int nTotalNeeds = 0, nTotalCount =0;
		switch( pEnchant->cEnchantGuardType )
		{
		case ENCHANT_ITEM_GUARD :
			{
				nTotalNeeds = pEnchantLevelData->nProtectItemCount;
				nTotalCount = GetCashItemCountByType(ITEMTYPE_ENCHANT_BREAKGAURD);
				nTotalCount += GetInventoryItemCountByType(ITEMTYPE_ENCHANT_BREAKGAURD);
			}
			break;
		case ENCHANT_ITEM_SHIELD :
			{
				nTotalNeeds = pEnchantLevelData->nShieldItemCount;
				nTotalCount = GetCashItemCountByType(ITEMTYPE_ENCHANT_SHIELD);
				nTotalCount += GetInventoryItemCountByType(ITEMTYPE_ENCHANT_SHIELD);
			}
			break;
		}
		if (nTotalCount < nTotalNeeds) return ERROR_ITEM_INSUFFICIENCY_ITEM;
		if (nTotalNeeds == 0) return ERROR_ITEM_ENCHANTFAIL;	// ProtectItemCount가 0으로 설정된 강화단계에서는 강화보호아이템 사용할 수 없다.
	}
#else
	// 아이템의 정보를 보고 필요 개수를 구해와 보호아이템이 모자른지 확인(필요 개수는 봉인 인장 개수와 동일하다고 한다.)
	if (pEnchant->bUseEnchantGuard){
	#ifdef PRE_FIX_MEMOPT_ENCHANT
		int nTotalNeeds = pEnchantNeedItemData->nProtectItemCount;
	#else
		int nTotalNeeds = pEnchantLevelData->nProtectItemCount;
	#endif
		int nTotalCount = 0;
		nTotalCount = GetCashItemCountByType(ITEMTYPE_ENCHANT_BREAKGAURD);
		nTotalCount += GetInventoryItemCountByType(ITEMTYPE_ENCHANT_BREAKGAURD);
		if (nTotalCount < nTotalNeeds) return ERROR_ITEM_INSUFFICIENCY_ITEM;
		if (nTotalNeeds == 0) return ERROR_ITEM_ENCHANTFAIL;	// ProtectItemCount가 0으로 설정된 강화단계에서는 강화보호아이템 사용할 수 없다.
	}
#endif	// #if defined(PRE_ADD_ENCHANTSHIELD_CASHITEM)

#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
	int nNeedCoin = pEnchantLevelData->nNeedCoin;

	// 만약 호감도 상점 혜택 보상 테이블에 등록되어있고, 
	// 혜택종류가 강화 비용 할인이 있다면 적용시켜준다.
	CNpcReputationProcessor::CheckAndCalcUnionBenefit( m_pSession, TStoreBenefitData::EnchantFeeDiscount, nNeedCoin );

	if ( !m_pSession->CheckEnoughCoin(nNeedCoin) ) 
		return ERROR_ITEM_INSUFFICIENCY_MONEY;	// 돈이 충분치 않다
#else
	if (!m_pSession->CheckEnoughCoin(pEnchantLevelData->nNeedCoin)) return ERROR_ITEM_INSUFFICIENCY_MONEY;	// 돈이 충분치 않다
#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )

#ifdef PRE_FIX_MEMOPT_ENCHANT
	for (int i = 0; i < ENCHANTITEMMAX; i++)
	{
		if ((pEnchantNeedItemData->nNeedItemID[i] <= 0) ||(pEnchantNeedItemData->nNeedItemCount[i] <= 0)) continue;
		if (GetInventoryItemCount(pEnchantNeedItemData->nNeedItemID[i]) < pEnchantNeedItemData->nNeedItemCount[i]) return ERROR_ITEM_INSUFFICIENCYCOUNT;	// 아이템 개수가 모자름
	}
#else
	for (int i = 0; i < ENCHANTITEMMAX; i++){
		if ((pEnchantLevelData->nNeedItemID[i] <= 0) ||(pEnchantLevelData->nNeedItemCount[i] <= 0)) continue;
		if (GetInventoryItemCount(pEnchantLevelData->nNeedItemID[i]) < pEnchantLevelData->nNeedItemCount[i]) return ERROR_ITEM_INSUFFICIENCYCOUNT;	// 아이템 개수가 모자름
	}
#endif

	DWORD dwInterval = 0;
	switch( pItemData->cRank ) {
		case ITEMRANK_D:
		case ITEMRANK_C:
#ifdef PRE_MOD_ITEMUPGRADE_COOLTIME
			{
				dwInterval = (DWORD)CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::ITEMUPGRADE_COOLTIME_NOMAL_MAGIC_ITEM);
				if (dwInterval == 0)
					dwInterval = 2000;
			}
#else
			dwInterval = 2000;
#endif
			break;
		case ITEMRANK_B:
		case ITEMRANK_A:
#ifdef PRE_MOD_ITEMUPGRADE_COOLTIME
			{
				dwInterval = (DWORD)CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::ITEMUPGRADE_COOLTIME_RARE_EPIC_ITEM);
				if (dwInterval == 0)
					dwInterval = 4000;
			}
#else
			dwInterval = 4000;
#endif
			break;
		case ITEMRANK_S:
#if defined (PRE_ADD_LEGEND_ITEM_UI)
		case ITEMRANK_SS:
#endif // #if defined (PRE_ADD_LEGEND_ITEM_UI)
#ifdef PRE_MOD_ITEMUPGRADE_COOLTIME
			{
				dwInterval = (DWORD)CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::ITEMUPGRADE_COOLTIME_ABOVE_UNIQUE);
				if (dwInterval == 0)
					dwInterval = 6000;
			}
#else
			dwInterval = 6000;
#endif
			break;
	}
	SetRequestTimer(RequestType_Enchant, dwInterval);

	return ERROR_NONE;
}

#if defined( _GAMESERVER )
int GetGaussianRandom( CMultiRoom* pRoom, int nMin, int nMax )
{
	float f1 = ( _rand(pRoom) % 10001 ) / 10000.f;
	float f2 = ( _rand(pRoom) % 10001 ) / 10000.f;

	int nResult = (int)(((nMin+nMax)/2.f) +(( sqrt(-2.f*log(f1)) * cos(2.f*3.1415926f*f2) ) *((nMax-nMin)/6.f) ) );
	if( nResult < nMin || nResult > nMax )
		nResult = (nMin + nMax) / 2; 

	return nResult;
};
#else
int GetGaussianRandom( int nMin, int nMax )
{
	float f1 = ( _rand() % 10001 ) / 10000.f;
	float f2 = ( _rand() % 10001 ) / 10000.f;

	int nResult = (int)(((nMin+nMax)/2.f) +(( sqrt(-2.f*log(f1)) * cos(2.f*3.1415926f*f2) ) *((nMax-nMin)/6.f) ) );
	if( nResult < nMin || nResult > nMax )
		nResult = (nMin + nMax) / 2; 

	return nResult;
};
#endif // #if defined( _GAMESERVER )

int CDNUserItem::UpgradeEnchant(CSEnchantItem *pEnchant)
{
	static long s_EnchantInterlocked = 0;
	CMtRandom Random;
	Random.srand( timeGetTime()+InterlockedIncrement(&s_EnchantInterlocked) );

#if defined(PRE_ADD_EQUIPED_ITEM_ENCHANT)
	TItem *pItem = NULL;
	if( pEnchant->bEnchantEquipedItem )
	{
		if (!IsValidEquipSlot(pEnchant->cItemIndex, pEnchant->biItemSerial)) return ERROR_ITEM_NOTFOUND;
#if defined(PRE_ADD_EQUIPLOCK)
		if(IsLockItem(DBDNWorldDef::ItemLocation::Equip, pEnchant->cItemIndex)) return ERROR_ITEM_LOCKITEM_NOTUPGRADE;
#endif	// #if defined(PRE_ADD_EQUIPLOCK)
		pItem = &m_Equip[pEnchant->cItemIndex];
	}
	else
	{ 
		if (!IsValidInventorySlot(pEnchant->cItemIndex, pEnchant->biItemSerial)) return ERROR_ITEM_INDEX_UNMATCH;	// 인덱스가 맞지않다
		pItem = &m_Inventory[pEnchant->cItemIndex];
	}
#else
	if (!IsValidInventorySlot(pEnchant->cInvenIndex, pEnchant->biItemSerial)) return ERROR_ITEM_INDEX_UNMATCH;	// 인덱스가 맞지않다
	
	TItem *pItem = &m_Inventory[pEnchant->cInvenIndex];
#endif
	TItemData *pItemData = g_pDataManager->GetItemData(pItem->nItemID);
	if (!pItemData) return ERROR_ITEM_NOTFOUND;

	if (pItem->wCount != 1) return ERROR_ITEM_ENCHANTFAIL;
	if (pItem->nSerial != pEnchant->biItemSerial) return ERROR_ITEM_ENCHANTFAIL;
	if (pItemData->nEnchantID <= 0) return ERROR_ITEM_ENCHANTFAIL;	// 인챈트 아이템 아님
	if (pItem->cLevel >= g_pDataManager->GetEnchantMaxLevel(pItemData->nEnchantID)) return ERROR_ITEM_ENCHANTFAIL;	// 인챈트 최대레벨 넘어감
	if (pItemData->cReversion == ITEMREVERSION_BELONG && pItem->bSoulbound == false) return ERROR_ITEM_ENCHANTFAIL;	// 봉인아이템
	
#ifdef PRE_FIX_MEMOPT_ENCHANT
	TEnchantLevelData* pEnchantLevelData = g_pDataManager->GetEnchantLevelData(pItemData->nEnchantID, pItem->cLevel + 1);	// 강화할 아이템데이터 찾기
	if (!pEnchantLevelData)
		return ERROR_ITEM_NOTFOUND;

	const TEnchantNeedItemData* pEnchantNeedItemData = g_pDataManager->GetEnchantNeedItemData(*pEnchantLevelData);
	if (pEnchantNeedItemData == NULL || pEnchantNeedItemData->cEnchantProb == 0)
		return ERROR_ITEM_ENCHANTFAIL;
#else
	TEnchantLevelData *pEnchantLevelData = g_pDataManager->GetEnchantLevelData(pItemData->nEnchantID, pItem->cLevel + 1);	// 강화할 아이템데이터 찾기
	if (!pEnchantLevelData) return ERROR_ITEM_NOTFOUND;
	if (pEnchantLevelData->cEnchantProb == 0) return ERROR_ITEM_ENCHANTFAIL;
#endif

#if defined(PRE_ADD_ENCHANTSHIELD_CASHITEM)
	int nTotalNeeds = 0;
	if ( pEnchant->cEnchantGuardType > 0 )
	{
		int nTotalCount =0;
		switch( pEnchant->cEnchantGuardType )
		{
		case ENCHANT_ITEM_GUARD :
			{
				nTotalNeeds = pEnchantLevelData->nProtectItemCount;
				nTotalCount = GetCashItemCountByType(ITEMTYPE_ENCHANT_BREAKGAURD);
				nTotalCount += GetInventoryItemCountByType(ITEMTYPE_ENCHANT_BREAKGAURD);
			}
			break;
		case ENCHANT_ITEM_SHIELD :
			{
				nTotalNeeds = pEnchantLevelData->nShieldItemCount;
				nTotalCount = GetCashItemCountByType(ITEMTYPE_ENCHANT_SHIELD);
				nTotalCount += GetInventoryItemCountByType(ITEMTYPE_ENCHANT_SHIELD);
			}
			break;
		}
		if (nTotalCount < nTotalNeeds) return ERROR_ITEM_INSUFFICIENCY_ITEM;
		if (nTotalNeeds == 0) return ERROR_ITEM_ENCHANTFAIL;	// ProtectItemCount가 0으로 설정된 강화단계에서는 강화보호아이템 사용할 수 없다.
	}
#else
	// 아이템의 정보를 보고 필요 개수를 구해와 보호아이템이 모자른지 확인(필요 개수는 봉인 인장 개수와 동일하다고 한다.)
	int nTotalNeeds = 0;
	if (pEnchant->bUseEnchantGuard){
	#ifdef PRE_FIX_MEMOPT_ENCHANT
		nTotalNeeds = pEnchantNeedItemData->nProtectItemCount;
	#else
		nTotalNeeds = pEnchantLevelData->nProtectItemCount;
	#endif
		int nTotalCount = 0;
		nTotalCount = GetCashItemCountByType(ITEMTYPE_ENCHANT_BREAKGAURD);
		nTotalCount += GetInventoryItemCountByType(ITEMTYPE_ENCHANT_BREAKGAURD);
		if (nTotalCount < nTotalNeeds) return ERROR_ITEM_INSUFFICIENCY_ITEM;
		if (nTotalNeeds == 0) return ERROR_ITEM_ENCHANTFAIL;	// ProtectItemCount가 0으로 설정된 강화단계에서는 강화보호아이템 사용할 수 없다.
	}
#endif	// #if defined(PRE_ADD_ENCHANTSHIELD_CASHITEM)

	int nNeedCoin = pEnchantLevelData->nNeedCoin;
	if ( m_pSession->bIsRemoteEnchant() == true )
	{
		const TItemData* pRemoteItemData = g_pDataManager->GetItemData( m_pSession->GetRemoteEnchantItemID() );
		if ( pRemoteItemData )
		{
			int nDiscount = nNeedCoin*pRemoteItemData->nTypeParam[0]/100;
			nNeedCoin -= nDiscount;
			if( nNeedCoin < 0 )
				nNeedCoin = 0;
		}
	}

#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
	// 만약 호감도 상점 혜택 보상 테이블에 등록되어있고, 
	// 혜택종류가 강화 비용 할인이 있다면 적용시켜준다.
	CNpcReputationProcessor::CheckAndCalcUnionBenefit( m_pSession, TStoreBenefitData::EnchantFeeDiscount, nNeedCoin );
	if( !m_pSession->CheckEnoughCoin(nNeedCoin) ) 
		return ERROR_ITEM_INSUFFICIENCY_MONEY;	// 돈이 충분치 않다
#if defined( _GAMESERVER )
	g_Log.Log(LogType::_ERROR, m_pSession, L"Line12498 PrevCoin=%I64d ChangeGold=%d PickUpCoin=%I64d \r\n", m_pSession->GetCoin(), nNeedCoin, m_pSession->GetPickUpCoin());
#endif
	m_pSession->DelCoin(nNeedCoin, DBDNWorldDef::CoinChangeCode::DoNotDBSave, 0);	// 돈 빼주고
#else
	if (!m_pSession->CheckEnoughCoin(nNeedCoin)) return ERROR_ITEM_INSUFFICIENCY_MONEY;	// 돈이 충분치 않다
	m_pSession->DelCoin(nNeedCoin, DBDNWorldDef::CoinChangeCode::DoNotDBSave, 0);	// 돈 빼주고
#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )

	// 아이템 체크
#ifdef PRE_FIX_MEMOPT_ENCHANT
	for (int i = 0; i < ENCHANTITEMMAX; i++)
	{
		if ((pEnchantNeedItemData->nNeedItemID[i] <= 0) ||(pEnchantNeedItemData->nNeedItemCount[i] <= 0))
			continue;

		if (GetInventoryItemCount(pEnchantNeedItemData->nNeedItemID[i]) < pEnchantNeedItemData->nNeedItemCount[i])
			return ERROR_ITEM_INSUFFICIENCYCOUNT;	// 아이템 개수가 모자름
	}
#else
	for (int i = 0; i < ENCHANTITEMMAX; i++){
		if ((pEnchantLevelData->nNeedItemID[i] <= 0) ||(pEnchantLevelData->nNeedItemCount[i] <= 0)) continue;
		if (GetInventoryItemCount(pEnchantLevelData->nNeedItemID[i]) < pEnchantLevelData->nNeedItemCount[i]) return ERROR_ITEM_INSUFFICIENCYCOUNT;	// 아이템 개수가 모자름
	}
#endif

	bool boSuccess = false;
#ifdef PRE_FIX_MEMOPT_ENCHANT
	int nEnchantProb = pEnchantNeedItemData->cEnchantProb;
#else	// #ifdef PRE_FIX_MEMOPT_ENCHANT
	int nEnchantProb = pEnchantLevelData->cEnchantProb;
#endif	// #ifdef PRE_FIX_MEMOPT_ENCHANT

#if defined(PRE_ADD_WORLD_EVENT)
	const TEventListInfo* pEventInfo = g_pEvent->GetEventByType(m_pSession->GetWorldSetID(), WorldEvent::EVENT7, m_pSession->GetClassID());
	if (pEventInfo && pEventInfo->nAtt1 != 0 && (pEventInfo->nAtt2 == 0 || pItem->cLevel <= pEventInfo->nAtt2))
	{
		nEnchantProb += (int)(nEnchantProb*(pEventInfo->nAtt1/100.0f));
	}
#else
#if defined(PRE_ADD_WEEKLYEVENT)
	int nThreadID = 0;
#ifdef _GAMESERVER
	nThreadID = m_pSession->GetGameRoom()->GetServerID();
#endif		//#ifdef _GAMESERVER
	int nEventValue = g_pDataManager->GetWeeklyEventValue(WeeklyEvent::Player, m_pSession->GetClassID(), WeeklyEvent::Event_4, nThreadID);

	if (nEventValue != 0)
		nEnchantProb += nEventValue;
#endif	// #if defined(PRE_ADD_WEEKLYEVENT)
#endif //#if defined(PRE_ADD_WORLD_EVENT)

	if ((Random.rand() % 100) < nEnchantProb || m_pSession->GetCheatCommand()->m_bLucky ){	// 강화성공
		boSuccess = true;
	}
	else{
		boSuccess = false;
	}

	std::vector<TItemInfo> VecMaterialItemList;
	VecMaterialItemList.clear();

#ifdef PRE_FIX_MEMOPT_ENCHANT
	int nCount = 0;
	for (int i = 0; i < ENCHANTITEMMAX; i++){
		if ((pEnchantNeedItemData->nNeedItemID[i] <= 0) ||(pEnchantNeedItemData->nNeedItemCount[i] <= 0)) continue;
		if (!DeleteInventoryExByItemID(pEnchantNeedItemData->nNeedItemID[i], -1, pEnchantNeedItemData->nNeedItemCount[i], DBDNWorldDef::UseItem::DoNotDBSave, 0, VecMaterialItemList, true))	// 밑에 한꺼번에 로그를 보내기때문에 따로 여기서 보낼필요없다
			return ERROR_ITEM_ENCHANTFAIL;

		nCount++;
	}
#else
	int nCount = 0;
	for (int i = 0; i < ENCHANTITEMMAX; i++){
		if ((pEnchantLevelData->nNeedItemID[i] <= 0) ||(pEnchantLevelData->nNeedItemCount[i] <= 0)) continue;
		if (!DeleteInventoryExByItemID(pEnchantLevelData->nNeedItemID[i], -1, pEnchantLevelData->nNeedItemCount[i], DBDNWorldDef::UseItem::DoNotDBSave, 0, VecMaterialItemList, true))	// 밑에 한꺼번에 로그를 보내기때문에 따로 여기서 보낼필요없다
			return ERROR_ITEM_ENCHANTFAIL;

		nCount++;
	}
#endif // PRE_FIX_MEMOPT_ENCHANT
	int nSupportItemID = 0;
#if defined(PRE_ADD_ENCHANTSHIELD_CASHITEM)
	int nGuardRatio = 0;
	if (pEnchant->cEnchantGuardType > 0)
	{
		int nType = 0;
		switch(pEnchant->cEnchantGuardType)
		{
		case ENCHANT_ITEM_GUARD:
			{
				nType = ITEMTYPE_ENCHANT_BREAKGAURD;
				// 전에는 TypeParam에서 nGuardRatio를 구해왔는데, 아이템ID가 여러개로 나눠지면서 이게 불가능해졌다.
				// 그래서 nGuardRatio는 그냥 100으로 설정한다.
				nGuardRatio = 100;
			}
			break;
		case ENCHANT_ITEM_SHIELD:
			{
				nType = ITEMTYPE_ENCHANT_SHIELD;
			}
			break;
		}
		if (UseItemByType(nType, nTotalNeeds, true, -1, pItem->nItemID, pItem->cLevel ) == false)
			return ERROR_ITEM_FAIL;
		TItem* pSupportItem = GetCashItemByType(nType);
		if( pSupportItem == NULL)
			pSupportItem = GetInventoryItemByType(nType);

		if( pSupportItem )
			nSupportItemID = pSupportItem->nItemID;
	}
#else
	int nGuardRatio = 0;
	if (pEnchant->bUseEnchantGuard){
		// 전에는 TypeParam에서 nGuardRatio를 구해왔는데, 아이템ID가 여러개로 나눠지면서 이게 불가능해졌다.
		// 그래서 nGuardRatio는 그냥 100으로 설정한다.
		nGuardRatio = 100;
		if (UseItemByType(ITEMTYPE_ENCHANT_BREAKGAURD, nTotalNeeds, true, -1, pItem->nItemID, pItem->cLevel ) == false)
			return ERROR_ITEM_FAIL;
		TItem* pSupportItem = GetCashItemByType(ITEMTYPE_ENCHANT_BREAKGAURD);
		if( pSupportItem == NULL)
			pSupportItem = GetInventoryItemByType(ITEMTYPE_ENCHANT_BREAKGAURD);

		if( pSupportItem )
			nSupportItemID = pSupportItem->nItemID;
	}
#endif	// #if defined(PRE_ADD_ENCHANTSHIELD_CASHITEM)

	if (boSuccess){	// 강화성공
		/*
		if (pItem->cPotential <= 0){	// 잠재력이 하나도 없다면
			if (pEnchantLevelData->nPotentialID > 0){
				TPotentialData *pPotential = g_pDataManager->GetPotentialData(pEnchantLevelData->nPotentialID);
				if (!pPotential) return ERROR_ITEM_NOTFOUND;

				if ((_rand() % 100) < pEnchantLevelData->cPotentialProb || m_pSession->GetCheatCommand()->m_bLucky ){	// 잠재력성공
					if ( pPotential->nTotalProb > 0 ) {
						int nRand = _rand() % pPotential->nTotalProb;
						int nPotentialIndex = 0;
						for (int i = 0; i <(int)pPotential->pVecItemData.size(); i++){
							if (nRand < pPotential->pVecItemData[i]->nPotentailOffset){
								nPotentialIndex = i + 1;
								break;
							}
						}
						pItem->cPotential = nPotentialIndex;
					}
				}
			}
		}
		*/
		pItem->cLevel += 1;
		int nEnchantMaxLevel = g_pDataManager->GetEnchantMaxLevel(pItemData->nEnchantID);
		if (pItem->cLevel > nEnchantMaxLevel) pItem->cLevel = nEnchantMaxLevel;

#if defined(PRE_ADD_EQUIPED_ITEM_ENCHANT)
		if( pEnchant->bEnchantEquipedItem )
		{
			//m_pSession->SendMoveItem(MoveType_Equip, pEnchant->cItemIndex, pEnchant->cItemIndex, pItem, pItem, ERROR_NONE);
			m_pSession->SendRefreshEquip( pEnchant->cItemIndex, pItem, true );
			BroadcastChangeEquip(pEnchant->cItemIndex, *const_cast<TItem*>(pItem) );
#if defined(_GAMESERVER)
			DnActorHandle hActor = m_pSession->GetActorHandle();
			CDnPlayerActor* pPlayer = static_cast<CDnPlayerActor *>( hActor.GetPointer() );
			if( pPlayer )
				pPlayer->RefreshState( CDnActorState::RefreshEquip );
#endif // _GAMESERVER
		}
		else
			m_pSession->SendRefreshInven(pEnchant->cItemIndex, pItem, false);
#else
		m_pSession->SendRefreshInven(pEnchant->cInvenIndex, &m_Inventory[pEnchant->cInvenIndex], false);
#endif
		m_pSession->GetDBConnection()->QueryEnchantItem(m_pSession, pItem->nSerial, nNeedCoin, true, false, pItem->cLevel, pItem->cPotential, NULL, nSupportItemID, VecMaterialItemList);

		m_pSession->GetEventSystem()->OnEvent( EventSystem::OnItemEnchantSuccess, 2,
			EventSystem::ItemID, pItem->nItemID,
			EventSystem::EnchantLevel, pItem->cLevel );

		m_pSession->SendEnchantComplete(pItem->nItemID, pItem->cLevel, pItem->cOption, ERROR_NONE);
		m_pSession->BroadcastingEffect(EffectType_Enchant, EffectState_Success);

		int nWorldNoticeValue = (int)CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::EnchantWorldNoticeValue);
		if (pItem->cLevel >= nWorldNoticeValue){	// 레벨7 이상일때 강화메시지 뿌린다
#if defined( _GAMESERVER )
			g_pMasterConnectionManager->SendWorldSystemMsg( m_pSession->GetWorldSetID(), m_pSession->GetAccountDBID(), WORLDCHATTYPE_ENCHANTITEM, pItem->nItemID, pItem->cLevel);
#else
			g_pMasterConnection->SendWorldSystemMsg(m_pSession->GetAccountDBID(), WORLDCHATTYPE_ENCHANTITEM, pItem->nItemID, pItem->cLevel);
#endif // #if defined( _GAMESERVER )
		
			if (m_pSession->GetGuildUID().IsSet()) 
			{
				const TGuildUID GuildUID = m_pSession->GetGuildUID();
				CDNGuildBase* pGuild = g_pGuildManager->At(GuildUID);
				if (pGuild)
				{
#if !defined( PRE_ADD_NODELETEGUILD )
					CDetachAutoEx<CDNGuildBase>	AutoDetach(pGuild);
					if (TRUE == pGuild->IsEnable())
#endif
						m_pSession->GetDBConnection()->QueryAddGuildHistory(m_pSession->GetDBThreadID(), pGuild->GetUID().nDBID, pGuild->GetUID().nWorldID, GUILDHISTORY_TYPE_ITEMENCT, pItem->cLevel, pItem->nItemID, m_pSession->GetCharacterDBID(), m_pSession->GetCharacterName());
				}
			}
		}
	}
	else {	// 강화실패

		// 강화 실패시 무조건 뽀개는게 아니고 레벨 다운시키는것도 있다.
#ifdef PRE_FIX_MEMOPT_ENCHANT
		bool bBreak = (Random.rand() % 100) < pEnchantNeedItemData->cBreakRatio;
#else
		bool bBreak = (Random.rand() % 100) < pEnchantLevelData->cBreakRatio;
#endif
#if defined(PRE_ADD_MISSION_COUPON)
		char cPrevEnchantLevel = pItem->cLevel;	//강화 실패 미션시 이전 강화레벨 정보 필요
#endif
		bool bDownLevelZero = false;
#if defined(PRE_ADD_ENCHANTSHIELD_CASHITEM)		
		switch( pEnchant->cEnchantGuardType )
		{
		case ENCHANT_ITEM_NONE: break;
		case ENCHANT_ITEM_GUARD :
			{
				if (bBreak){
					// BreakRatio가 0보다 크게 설정된 7강부터 파괴로 들어올 경우 다시 한번 GuardRatio를 체크한다.
					if ((Random.rand() % 100) < nGuardRatio)
						bBreak = false;
				}
				else{
					// BreakRatio가 0으로 설정된 6강시도까지는 다운으로 들어올 경우 다운 레벨을 0으로 고정해준다.
					if ((pEnchantLevelData->cBreakRatio == 0) && ((Random.rand() % 100) < nGuardRatio))
						bDownLevelZero = true;
				}
			}
			break;
		case ENCHANT_ITEM_SHIELD : // 상위보호젤리 다운x, 파괴x
			{
				bBreak = false;
				bDownLevelZero = true;
			}
			break;
		}		
#else
		if (pEnchant->bUseEnchantGuard)
		{
			if (bBreak){
				// BreakRatio가 0보다 크게 설정된 7강부터 파괴로 들어올 경우 다시 한번 GuardRatio를 체크한다.
				if ((Random.rand() % 100) < nGuardRatio)
					bBreak = false;
			}
			else{
				// BreakRatio가 0으로 설정된 6강시도까지는 다운으로 들어올 경우 다운 레벨을 0으로 고정해준다.
#ifdef PRE_FIX_MEMOPT_ENCHANT
					if ((pEnchantNeedItemData->cBreakRatio == 0) && ((Random.rand() % 100) < nGuardRatio))
#else
					if ((pEnchantLevelData->cBreakRatio == 0) && ((Random.rand() % 100) < nGuardRatio))
#endif
					bDownLevelZero = true;
			}
		}
#endif

		if( bBreak ) {
			INT64 nFailSerial = pItem->nSerial;
#if defined(PRE_ADD_EQUIPED_ITEM_ENCHANT)
			if( pEnchant->bEnchantEquipedItem )
			{
				//장착중인 아이템 바로 삭제
				if( _PopEquipSlot( pEnchant->cItemIndex, pItem->wCount) != ERROR_NONE ) return ERROR_ITEM_ENCHANTFAIL;
				m_pSession->SendRemoveItem(ITEMPOSITION_EQUIP, pEnchant->cItemIndex, pItem, ERROR_NONE);
#if defined(_GAMESERVER)
				DnActorHandle hActor = m_pSession->GetActorHandle();
				CDnPlayerActor* pPlayer = static_cast<CDnPlayerActor *>( hActor.GetPointer() );
				if( pPlayer )
				{
					if( pEnchant->cItemIndex < EQUIP_WEAPON1 )
						pPlayer->DetachParts( (CDnParts::PartsTypeEnum)pEnchant->cItemIndex );
					else 
						pPlayer->DetachWeapon( pEnchant->cItemIndex - EQUIP_WEAPON1 );

					CDNGameRoom *pRoom = m_pSession->GetGameRoom();
					if (pRoom && pRoom->GetTaskMng()){
						CDnItemTask *pItemTask = (CDnItemTask *)pRoom->GetTaskMng()->GetTask( "ItemTask" );
						if (pItemTask)
							pItemTask->RemoveEquipItem( m_pSession, pEnchant->cItemIndex );				
					}
					pPlayer->RefreshState( CDnActorState::RefreshEquip );
				}
#endif // _GAMESERVER
			}
			else
				if (!DeleteInventoryBySlot(pEnchant->cItemIndex, 1, pEnchant->biItemSerial, DBDNWorldDef::UseItem::DoNotDBSave)) return ERROR_ITEM_ENCHANTFAIL;	// 아이템삭제	// 밑에서 한꺼번에 로그찍기때문에 여기서 로그값을 보낼필요없다
#else
			if (!DeleteInventoryBySlot(pEnchant->cInvenIndex, 1, pEnchant->biItemSerial, DBDNWorldDef::UseItem::DoNotDBSave)) return ERROR_ITEM_ENCHANTFAIL;	// 아이템삭제	// 밑에서 한꺼번에 로그찍기때문에 여기서 로그값을 보낼필요없다
#endif

			TItem NewItem = {0, };
			int nResultItemID = 0;
#ifdef PRE_FIX_MEMOPT_ENCHANT
			if( pEnchantNeedItemData->nFailResultItemID > 0 ) {
#else
			if( pEnchantLevelData->nFailResultItemID > 0 ) {
#endif
				int nResultItemCount = 0;
				int nLevel = 0;
				int nDepth = ITEMDROP_DEPTH;

#ifdef PRE_FIX_MEMOPT_ENCHANT
	#if defined( PRE_ADD_FARM_DOWNSCALE )
				CDNUserItem::CalcDropItems( m_pSession, pEnchantNeedItemData->nFailResultItemID, nResultItemID, nResultItemCount, nLevel, nDepth );
	#else
				CalcDropItems( pEnchantNeedItemData->nFailResultItemID, nResultItemID, nResultItemCount, nLevel, nDepth );
	#endif // #if defined( PRE_ADD_FARM_DOWNSCALE )
#else
	#if defined( PRE_ADD_FARM_DOWNSCALE )
				CDNUserItem::CalcDropItems( m_pSession, pEnchantLevelData->nFailResultItemID, nResultItemID, nResultItemCount, nLevel, nDepth );
	#else
				CalcDropItems( pEnchantLevelData->nFailResultItemID, nResultItemID, nResultItemCount, nLevel, nDepth );
	#endif // #if defined( PRE_ADD_FARM_DOWNSCALE )
#endif
				if( nResultItemID > 0 ) {
					INT64 nSerial = 0;
					std::vector<TSaveItemInfo> VecItemList;
					VecItemList.clear();
					if (CreateInvenItem2(nResultItemID, nResultItemCount, VecItemList) != ERROR_NONE) {
						nResultItemID = 0;
					}
					if (!VecItemList.empty()) NewItem = VecItemList[0].Item;
				}
			}
			m_pSession->GetDBConnection()->QueryEnchantItem(m_pSession, nFailSerial, nNeedCoin, false, true, 0, 0, &NewItem, nSupportItemID, VecMaterialItemList);
			m_pSession->SendEnchantComplete( nResultItemID, 0, 0, ERROR_ITEM_ENCHANT_POSSIBILITYFAIL_BREAKITEM);
			m_pSession->BroadcastingEffect(EffectType_Enchant, EffectState_Fail);
		}
		else {
			char cDecreaseEnchantLevel = 0;
			if( bDownLevelZero == false )
#ifdef PRE_FIX_MEMOPT_ENCHANT
	#if defined( _GAMESERVER )
				cDecreaseEnchantLevel = (char)GetGaussianRandom( m_pSession->GetGameRoom(), pEnchantNeedItemData->cMinDownLevel, pEnchantNeedItemData->cMaxDownLevel );
	#else
				cDecreaseEnchantLevel = (char)GetGaussianRandom( pEnchantNeedItemData->cMinDownLevel, pEnchantNeedItemData->cMaxDownLevel );
	#endif // #if defined( _GAMESERVER )
#else
	#if defined( _GAMESERVER )
				cDecreaseEnchantLevel = (char)GetGaussianRandom( m_pSession->GetGameRoom(), pEnchantLevelData->cMinDownLevel, pEnchantLevelData->cMaxDownLevel );
	#else
				cDecreaseEnchantLevel = (char)GetGaussianRandom( pEnchantLevelData->cMinDownLevel, pEnchantLevelData->cMaxDownLevel );
	#endif // #if defined( _GAMESERVER )
#endif
			pItem->cLevel -= cDecreaseEnchantLevel;
			if( pItem->cLevel <= 0 ) {
				pItem->cLevel = 0;
//				pItem->cPotential = 0;
			}
#if defined(PRE_ADD_EQUIPED_ITEM_ENCHANT)
			if(cDecreaseEnchantLevel > 0)
			{
				if(pEnchant->bEnchantEquipedItem)
				{
					m_pSession->SendRefreshEquip( pEnchant->cItemIndex, pItem, true );
					BroadcastChangeEquip(pEnchant->cItemIndex, *const_cast<TItem*>(pItem) );
#if defined(_GAMESERVER)
					DnActorHandle hActor = m_pSession->GetActorHandle();
					CDnPlayerActor* pPlayer = static_cast<CDnPlayerActor *>( hActor.GetPointer() );
					if( pPlayer )
						pPlayer->RefreshState( CDnActorState::RefreshEquip );
#endif // _GAMESERVER
				}
				else
					m_pSession->SendRefreshInven(pEnchant->cItemIndex, pItem, false);
			}
#else
			m_pSession->SendRefreshInven(pEnchant->cInvenIndex, &m_Inventory[pEnchant->cInvenIndex], false);
#endif
			int nRet = (cDecreaseEnchantLevel == 0) ? ERROR_ITEM_ENCHANT_POSSIBILITYFAIL_DOWNLEVELZERO : ERROR_ITEM_ENCHANT_POSSIBILITYFAIL_DOWNLEVEL;
			m_pSession->SendEnchantComplete(pItem->nItemID, pItem->cLevel, pItem->cOption, nRet);
			m_pSession->BroadcastingEffect(EffectType_Enchant, EffectState_SoftFail);
			m_pSession->GetDBConnection()->QueryEnchantItem( m_pSession, pItem->nSerial, nNeedCoin, false, false, pItem->cLevel, pItem->cPotential, NULL, nSupportItemID, VecMaterialItemList);
		}

#if defined(PRE_ADD_MISSION_COUPON)
		m_pSession->GetEventSystem()->OnEvent( EventSystem::OnItemEnchantFailed, 2, 
			EventSystem::ItemID, pItemData->nItemID,
			EventSystem::EnchantLevel, (int)cPrevEnchantLevel );
#else
		m_pSession->GetEventSystem()->OnEvent( EventSystem::OnItemEnchantFailed, 1, EventSystem::ItemID, pItemData->nItemID );
#endif
		
	}

	return ERROR_NONE;
}

void CDNUserItem::CheatUpgradeItemLevel(int nItemLevel)
{
	TItemData *pItemData = NULL;
	TItem *pItem = NULL;

	for (int i = 0; i < EQUIPMAX; i++){
		int useItemLevel = nItemLevel;
		pItem = &m_Equip[i];
		pItemData = g_pDataManager->GetItemData(pItem->nItemID);
		if (!pItemData) continue;
		if (pItemData->nEnchantID <= 0) continue;

		int nEnchantMaxLevel = g_pDataManager->GetEnchantMaxLevel(pItemData->nEnchantID);
		if (useItemLevel > nEnchantMaxLevel) useItemLevel = nEnchantMaxLevel;

		pItem->cLevel = useItemLevel;
		m_pSession->GetDBConnection()->QueryModMaterializedItem(m_pSession, *pItem,	ModItem_Level);

		m_pSession->SendRefreshEquip(i, pItem);
	}

#if defined(PRE_PERIOD_INVENTORY)
	for (int i = 0; i < INVENTORYTOTALMAX; i++){
		if (i == GetInventoryCount()){
			if (IsEnablePeriodInventory())
				i = INVENTORYMAX;
			else
				break;
		}
#else	// #if defined(PRE_PERIOD_INVENTORY)
	for (int i = 0; i < GetInventoryCount(); i++){
#endif	// #if defined(PRE_PERIOD_INVENTORY)
		int useItemLevel = nItemLevel;
		pItem = &m_Inventory[i];
		pItemData = g_pDataManager->GetItemData(pItem->nItemID);
		if (!pItemData) continue;
		if (pItemData->nEnchantID <= 0) continue;

		int nEnchantMaxLevel = g_pDataManager->GetEnchantMaxLevel(pItemData->nEnchantID);
		if (useItemLevel > nEnchantMaxLevel) useItemLevel = nEnchantMaxLevel;

		pItem->cLevel = useItemLevel;
		m_pSession->GetDBConnection()->QueryModMaterializedItem(m_pSession, *pItem,	ModItem_Level);

		m_pSession->SendRefreshInven(i, pItem);
	}
}

void CDNUserItem::CheatUpgradeItemPotential(int nPotential)
{
	TItemData *pItemData = NULL;
	TEnchantLevelData *pEnchantLevelData = NULL;
	TItem *pItem = NULL;
	TPotentialData *pPotentialData = NULL;

	for (int i = 0; i < EQUIPMAX; i++){
		pItem = &m_Equip[i];
		pItemData = g_pDataManager->GetItemData(pItem->nItemID);
		if (!pItemData) continue;
		if (pItemData->nEnchantID <= 0) continue;

		pPotentialData = g_pDataManager->GetPotentialData(nPotential);
		if( !pPotentialData ) continue;

		pItem->cPotential = nPotential;
		m_pSession->GetDBConnection()->QueryModMaterializedItem(m_pSession, *pItem,	ModItem_Potential);

		m_pSession->SendRefreshEquip(i, pItem);
	}
}


#if defined (PRE_ADD_EXCHANGE_POTENTIAL)
int CDNUserItem::ExchangePotential(CSExchangePotential * pPacket)
{
	if (m_pSession == NULL || m_pSession->GetDBConnection() == NULL)
		return ERROR_EXCHANGEPOTENTIAL_FAIL;

	if (pPacket->nExtractItemIndex == pPacket->nInjectItemIndex || pPacket->biExtractItemSerial == pPacket->biInjectItemSerial)
		return ERROR_EXCHANGEPOTENTIAL_FAIL;
	
	if (!IsValidInventorySlot(pPacket->nExtractItemIndex, pPacket->biExtractItemSerial) || \
		!IsValidInventorySlot(pPacket->nInjectItemIndex, pPacket->biInjectItemSerial))
	{
		_DANGER_POINT_MSG(L"EXCHANGE_POTENTIAL Index Err");
		return ERROR_ITEM_INDEX_UNMATCH;
	}

	const TItem * pExtractItem = GetInventory(pPacket->nExtractItemIndex);
	const TItem * pInjectItem = GetInventory(pPacket->nInjectItemIndex);
	if (pExtractItem == NULL || pInjectItem == NULL)
	{
		_DANGER_POINT_MSG(L"EXCHANGE_POTENTIAL Inven Err");
		return (pExtractItem == NULL ? ERROR_EXCHANGEPOTENTIAL_EXTRACT_ITEM_VALIDATION_FAIL : ERROR_EXCHANGEPOTENTIAL_INJECT_ITEM_VALIDATION_FAIL);
	}
#if defined(PRE_ADD_PCBANG_RENTAL_ITEM)
	if( g_pDataManager->IsPcCafeRentItem(pExtractItem->nItemID) || g_pDataManager->IsPcCafeRentItem(pInjectItem->nItemID))
	{
		//PC방 렌탈 아이템은 잠재이전 안되게 막는다.
		return ERROR_EXCHANGEPOTENTIAL_FAIL;
	}
#endif
	if (pExtractItem->cPotentialMoveCount > 0 && pExtractItem->cPotentialMoveCount >= static_cast<int>(CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::PotentialMovealbleCount)))
	{
		_DANGER_POINT_MSG(L"EXCHANGE_POTENTIAL Movealbe Count Err");
		return ERROR_EXCHANGEPOTENTIAL_MOVEABLECOUNT_EXPIRED;
	}
	
#ifdef PRE_MOD_75892
	if (pExtractItem->cPotential <= 0)
#else		//#ifdef PRE_MOD_75892
	if (pExtractItem->cPotential <= 0 || pInjectItem->cPotential > 0)
#endif		//#ifdef PRE_MOD_75892
	{
		_DANGER_POINT_MSG(L"EXCHANGE_POTENTIAL Potential Err");
		return (pExtractItem->cPotential <= 0 ? ERROR_EXCHANGEPOTENTIAL_EXTRACT_ITEM_VALIDATION_FAIL : ERROR_EXCHANGEPOTENTIAL_INJECT_ITEM_VALIDATION_FAIL);
	}

	TItemData * pExtrantItemData = g_pDataManager->GetItemData(pExtractItem->nItemID);
	TItemData * pInjectItemData = g_pDataManager->GetItemData(pInjectItem->nItemID);
	if (pExtrantItemData == NULL || pInjectItemData == NULL)
	{
		_DANGER_POINT_MSG(L"EXCHANGE_POTENTIAL TItemData Err");
		g_Log.Log(LogType::_ERROR, L"Potential Exchange Error TItemData Not Fount ItemID[%d]\n", pExtrantItemData == NULL ? pExtrantItemData->nItemID : pInjectItemData->nItemID);
		return ERROR_EXCHANGEPOTENTIAL_FAIL;
	}

	if (pExtrantItemData->cReversion == ITEMREVERSION_BELONG && pExtractItem->bSoulbound == false || pInjectItemData->cReversion == ITEMREVERSION_BELONG && pInjectItem->bSoulbound == false)
	{
		_DANGER_POINT_MSG(L"EXCHANGE_POTENTIAL Soulbound Err");
		return (pExtractItem->bSoulbound == false ? ERROR_EXCHANGEPOTENTIAL_EXTRACT_ITEM_VALIDATION_FAIL : ERROR_EXCHANGEPOTENTIAL_INJECT_ITEM_VALIDATION_FAIL);
	}

	if (pExtrantItemData->cRank != pInjectItemData->cRank && pExtrantItemData->cRank != pInjectItemData->cRank+1)
	{
		_DANGER_POINT_MSG(L"EXCHANGE_POTENTIAL Rank Err");
		return ERROR_EXCHANGEPOTENTIAL_ITEM_COMPARE_VALIDATION_FAIL;
	}

	if (pExtrantItemData->nType != pInjectItemData->nType)
	{
		_DANGER_POINT_MSG(L"EXCHANGE_POTENTIAL MainType Err");
		return ERROR_EXCHANGEPOTENTIAL_ITEM_COMPARE_VALIDATION_FAIL;
	}

	if (g_pDataManager->GetItemDetailType(pExtrantItemData) != g_pDataManager->GetItemDetailType(pInjectItemData))
	{
		_DANGER_POINT_MSG(L"EXCHANGE_POTENTIAL DetailType Err");
		return ERROR_EXCHANGEPOTENTIAL_ITEM_COMPARE_VALIDATION_FAIL;
	}

	if (g_pDataManager->GetItemDetailType(pExtrantItemData) != PARTS_NECKLACE && g_pDataManager->GetItemDetailType(pExtrantItemData) != PARTS_EARRING && 
		g_pDataManager->GetItemDetailType(pExtrantItemData) != PARTS_RING)
	{
		//목걸이, 귀걸이, 반지에 한해서는 직업검사 패쓰해달란다.	#59784
		bool bCheckNeedJob = false;
		if (pExtrantItemData->nNeedJobClassList.empty() && pInjectItemData->nNeedJobClassList.empty())
		{
			bCheckNeedJob = true;
		}
		else if (pExtrantItemData->nNeedJobClassList.empty() == false && pInjectItemData->nNeedJobClassList.empty() == false)
		{
			if (pExtrantItemData->nNeedJobClassList[0] == pInjectItemData->nNeedJobClassList[0])
				bCheckNeedJob = true;
		}

		if (bCheckNeedJob == false)
		{
			_DANGER_POINT_MSG(L"EXCHANGE_POTENTIAL NeedJob Err");
			return ERROR_EXCHANGEPOTENTIAL_ITEM_COMPARE_VALIDATION_FAIL;
		}
	}

	if(pInjectItemData->nTypeParam[1] == 0)
	{
		_DANGER_POINT_MSG(L"EXCHANGE_POTENTIAL InjectItem TypeParam Err");
		return ERROR_EXCHANGEPOTENTIAL_ITEM_COMPARE_VALIDATION_FAIL;
	}

	int nConsumption = g_pDataManager->GetPotentialTransConsumptionCount(pExtrantItemData->cLevelLimit, pExtrantItemData->cRank, \
		pInjectItemData->cLevelLimit, pInjectItemData->cRank, pExtrantItemData->nType, g_pDataManager->GetItemDetailType(pExtrantItemData));

	if (nConsumption <= 0)
	{
		_DANGER_POINT_MSG(L"EXCHANGE_POTENTIAL nConsumption Err");
		return ERROR_GENERIC_UNKNOWNERROR;
	}

	if (nConsumption > GetCashItemCountByType(ITEMTYPE_POTENTIAL_EXTRACTOR) && nConsumption > GetInventoryItemCountByType(ITEMTYPE_POTENTIAL_EXTRACTOR))
	{
		_DANGER_POINT_MSG(L"EXCHANGE_POTENTIAL InjectItem TypeParam Err");
		return ERROR_EXCHANGEPOTENTIAL_ITEM_COMPARE_VALIDATION_FAIL;
	}

	if(UseItemByType(ITEMTYPE_POTENTIAL_EXTRACTOR, nConsumption, true) == false)
	{
		_DANGER_POINT_MSG(L"EXCHANGE_POTENTIAL UseItem Err");
		return ERROR_EXCHANGEPOTENTIAL_FAIL;
	}

	m_pSession->GetDBConnection()->QueryMovePotential(m_pSession, pExtractItem->nSerial, pExtractItem->cPotential, pInjectItem->nSerial, pExtractItem->cPotential, \
		static_cast<int>(CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::PotentialMovealbleCount)), m_pSession->GetMapIndex());
	
	const_cast<TItem*>(pInjectItem)->cPotential = const_cast<TItem*>(pExtractItem)->cPotential;
	const_cast<TItem*>(pExtractItem)->cPotential = 0;
	const_cast<TItem*>(pInjectItem)->cPotentialMoveCount++;

	m_pSession->SendRefreshInven(pPacket->nExtractItemIndex, pExtractItem);
	m_pSession->SendRefreshInven(pPacket->nInjectItemIndex, pInjectItem);
	return ERROR_NONE;
}
#endif		//#if defined (PRE_ADD_EXCHANGE_POTENTIAL)

#if defined(PRE_ADD_EXCHANGE_ENCHANT)
int CDNUserItem::ExchangeEnchant(CSExchangeEnchant* pPacket)
{
	if (m_pSession == NULL || m_pSession->GetDBConnection() == NULL)
		return ERROR_EXCHANGE_ENCHANT_FAIL;

	if (!IsValidInventorySlot(pPacket->nExtractItemIndex, pPacket->biExtractItemSerial) || \
		!IsValidInventorySlot(pPacket->nInjectItemIndex, pPacket->biInjectItemSerial))
	{		
		return ERROR_ITEM_INDEX_UNMATCH;
	}

	const TItem * pExtractItem = GetInventory(pPacket->nExtractItemIndex);
	const TItem * pInjectItem = GetInventory(pPacket->nInjectItemIndex);
	if (pExtractItem == NULL || pInjectItem == NULL)
		return ERROR_EXCHANGE_ENCHANT_FAIL;
	
#if defined(PRE_ADD_PCBANG_RENTAL_ITEM)
	if( g_pDataManager->IsPcCafeRentItem(pExtractItem->nItemID) || g_pDataManager->IsPcCafeRentItem(pInjectItem->nItemID))
	{
		//PC방 렌탈 아이템은 강화이전 안되게 막는다.
		return ERROR_EXCHANGE_ENCHANT_FAIL;
	}
#endif
	if( pExtractItem->cLevel == 0 ) // 원본아이템이 강화되지 않은 아이템
		return ERROR_EXCHANGE_ENCHANT_FAIL;

	if( pExtractItem->cLevel <= pInjectItem->cLevel ) // 원본아이템이 강화레벨이 같거나 낮다.
		return ERROR_EXCHANGE_ENCHANT_FAIL;	

	TItemData * pExtrantItemData = g_pDataManager->GetItemData(pExtractItem->nItemID);
	TItemData * pInjectItemData = g_pDataManager->GetItemData(pInjectItem->nItemID);
	if (pExtrantItemData == NULL || pInjectItemData == NULL)	
		return ERROR_EXCHANGE_ENCHANT_FAIL;	

	// 동일 등급, 동일 레벨의 아이템 체크
	if (pExtrantItemData->cRank != pInjectItemData->cRank || pExtrantItemData->cLevelLimit != pInjectItemData->cLevelLimit )			
		return ERROR_EXCHANGE_ENCHANT_FAIL;

	TExchangeEnchantData* EnchantData = g_pDataManager->GetExchangeEnchantData(pExtrantItemData->cRank, pExtrantItemData->cLevelLimit, pExtractItem->cLevel);
	if( EnchantData == NULL )
		return ERROR_EXCHANGE_ENCHANT_FAIL; //강화 이전 할수 있는 아이템이 아님.

	// 수수료 체크
	if( EnchantData->nNeedCoin > 0 && m_pSession->CheckEnoughCoin(EnchantData->nNeedCoin) == false )	
		return ERROR_EXCHANGE_ENCHANT_REQ_MONEY;

	// 필요 아이템 체크
	std::vector<TItemInfo> VecMaterialItemList;
	VecMaterialItemList.clear();
	if( EnchantData->nNeedItemID1 > 0 && EnchantData->wNeedItemCount1 > 0)
	{
		if (GetInventoryItemCount(EnchantData->nNeedItemID1) < EnchantData->wNeedItemCount1)
			return ERROR_EXCHANGE_ENCHANT_REQ_ITEM; // 아이템이 모자름
		else
		{
			if (!DeleteInventoryExByItemID(EnchantData->nNeedItemID1, -1, EnchantData->wNeedItemCount1, DBDNWorldDef::UseItem::DoNotDBSave, 0, VecMaterialItemList, true))	// 밑에 한꺼번에 로그를 보내기때문에 따로 여기서 보낼필요없다
				return ERROR_EXCHANGE_ENCHANT_REQ_ITEM;
		}
	}

	if( EnchantData->nNeedItemID2 > 0 && EnchantData->wNeedItemCount2 > 0)
	{
		if (GetInventoryItemCount(EnchantData->nNeedItemID2) < EnchantData->wNeedItemCount2)
			return ERROR_EXCHANGE_ENCHANT_REQ_ITEM; // 아이템이 모자름
		else
		{
			if (!DeleteInventoryExByItemID(EnchantData->nNeedItemID2, -1, EnchantData->wNeedItemCount2, DBDNWorldDef::UseItem::DoNotDBSave, 0, VecMaterialItemList, true))	// 밑에 한꺼번에 로그를 보내기때문에 따로 여기서 보낼필요없다
				return ERROR_EXCHANGE_ENCHANT_REQ_ITEM;
		}
	}

	const_cast<TItem*>(pInjectItem)->cLevel = pExtractItem->cLevel;
	const_cast<TItem*>(pExtractItem)->cLevel = 0;	

	m_pSession->DelCoin(EnchantData->nNeedCoin, DBDNWorldDef::CoinChangeCode::DoNotDBSave, 0);	// 돈 빼주고

	m_pSession->GetDBConnection()->QueryEnchantItem(m_pSession, pInjectItem->nSerial, EnchantData->nNeedCoin, true, false, pInjectItem->cLevel, pInjectItem->cPotential, NULL, 0, VecMaterialItemList, pExtractItem->nSerial);
	
	m_pSession->SendRefreshInven(pPacket->nExtractItemIndex, pExtractItem, false);
	m_pSession->SendRefreshInven(pPacket->nInjectItemIndex, pInjectItem, false);

	return ERROR_NONE;
}
#endif

#if defined(_VILLAGESERVER)
// from job degree, to job degree 들의 직업차수는 0 부터 시작된다.
int CDNUserItem::CanUseSkillResetCashItem( int iFromJobDegree, int iToJobDegree, int& iRewardSP, vector<int>& vlSkillIDsToReset )
{
	int iResult = ERROR_NONE;

	// 이미 스킬 리셋 요청을 DB 로 보낸후 응답을 기다리는 중. 잘못된 요청임.
	if( m_pSession->GetSkill()->IsWaitSkillResetDBRes() )
		return ERROR_INVALIDPACKET;

	// 현재 캐릭터에 맞지 않는 직업 차수. 아이템의 스킬 리셋 직업 차수 구간과 같다면 
	// 현재 직업수준에 맞지 않다는 에러코드 클라로 리턴.
	// 하나라도 현재 직업 차수에 맞다면 사용 허용.
	int iInvalidJobDegreeCount = 0;

	// 스킬 리셋 캐쉬템에 지정된 직업차수 범위만큼 리셋할 스킬들을 모아둔다.
	// 상위 직업부터 리셋하기 위해 거꾸로 돕니다.
	int iJobDegreeCount = 0;
	//for( int nJobDegreeToReset = pItemData->nTypeParam[ 1 ]-1; pItemData->nTypeParam[ 0 ]-1 <= nJobDegreeToReset; --nJobDegreeToReset )
	for( int nJobDegreeToReset = iToJobDegree; iFromJobDegree <= nJobDegreeToReset; --nJobDegreeToReset )
	{
		bool bValidClassDegree = (0 <= nJobDegreeToReset) && (nJobDegreeToReset < JOBMAX);
		_ASSERT( bValidClassDegree );
		if( !bValidClassDegree ) 
		{
			iResult = ERROR_INVALIDPACKET;
			break;
		}

		TCharacterStatus* pStatus = m_pSession->GetStatusData();

		// 현재 보유하고 있는 차수의 직업인가. 보유하고 있지 않은 차수의 스킬들은 리셋할 수 없다.
		// 원래 요청 거부되는 거였다가 지금은 그냥 무시되고 현재 차수까지의 직업들의 스킬들을 리셋해준다.
		if( 0 == pStatus->cJobArray[ nJobDegreeToReset ] )
		{
			//iResult = ERROR_ITEM_MISMATCH_RESET_SKILL_JOB;
			++iInvalidJobDegreeCount;
			++iJobDegreeCount;
			continue;
		}

		// 최고 차수 직업 리셋이 아니라면, 상위 직업에서 스킬 하나라도 찍으면 스킬리셋 아이템 사용 불가.
		// 복수개의 차수를 리셋하는 아이템은 가장 상위의 스킬을 리셋할때만 체크하면 된다.
		if( nJobDegreeToReset+1 < JOBMAX && 
			0 == iJobDegreeCount )
		{
			// 상위 직업을 갖고 있는 경우만 상위 직업의 스킬을 찍었는지 체크.
			BYTE cHigherDegreeJob = pStatus->cJobArray[ nJobDegreeToReset+1 ];
			if( 0 != cHigherDegreeJob )
			{
				vector<int> vlUpDegreeJobSkills;
				m_pSession->GetSkill()->GatherThisJobSkill( cHigherDegreeJob, vlUpDegreeJobSkills );
				if( false == vlUpDegreeJobSkills.empty() )
				{
					// 상위 직업 스킬을 찍은 상태라 이 차수의 직업의 스킬을 리셋할 수 없음을 클라로 통보.
					iResult = ERROR_ITEM_ALREADY_HAVE_HIGHER_JOB_SKILL;
					break;
				}
			}
		}

		// 해당 직업의 스킬들을 모아옴.
		BYTE cJobToSkillReset = m_pSession->GetStatusData()->cJobArray[ nJobDegreeToReset ];
		m_pSession->GetSkill()->GatherThisJobSkill( cJobToSkillReset, vlSkillIDsToReset );

		++iJobDegreeCount;
	}

	//if( iInvalidJobDegreeCount == (pItemData->nTypeParam[ 1 ] - pItemData->nTypeParam[ 0 ])+1 )
	if( iInvalidJobDegreeCount == (iToJobDegree - iFromJobDegree)+1 )
		iResult = ERROR_ITEM_MISMATCH_RESET_SKILL_JOB;

	if( ERROR_NONE == iResult )
	{
		// 디폴트 스킬을 얻어와서 현재 디폴트 스킬들만 있고 1렙이라면 스킬을 하나도 찍은 게 없으므로 리셋할 수 없다.
		int aiDefaultSkills[ DEFAULTSKILLMAX ] = { 0 };
		g_pDataManager->GetCreateDefaultSkill( m_pSession->GetClassID(), aiDefaultSkills );
		int iDefaultSkillCount = 0;
		for( ; iDefaultSkillCount < DEFAULTSKILLMAX; ++iDefaultSkillCount )
		{
			if( aiDefaultSkills[ iDefaultSkillCount ] == 0 )
				break;
		}

		// 1 렙짜리 디폴트 스킬은 초기화 대상이 아니므로 초기화 리스트에서 지움.
		vector<int>::iterator vIter = vlSkillIDsToReset.begin();
		for( vIter; vIter != vlSkillIDsToReset.end(); )
		{
			bool bFinded = false;
			for( int i = 0; i < iDefaultSkillCount; ++i )
			{
				if( aiDefaultSkills[ i ] == *vIter )
				{
					if( 1 == m_pSession->GetSkill()->GetSkillLevel( *vIter ) )
					{
						vIter = vlSkillIDsToReset.erase( vIter );
						bFinded = true;
						break;
					}
				}
			}

			if( false == bFinded )
				 vIter++;
		}

		//bool bOnlyHaveDefaultSkill = false;
		if( vlSkillIDsToReset.empty() )
		{
			return ERROR_ITEM_DONT_HAVE_SKILL_TO_RESET;
		}
		//else
		//if( iDefaultSkillCount == vlSkillIDsToReset.size() )
		//{
		//	// 갯수도 같고 전부 디폴트 스킬과 일치하다면.. 렙도 1이라면 찍은 스킬 없음.
		//	bOnlyHaveDefaultSkill = true;
		//	for( int i =  0; i < iDefaultSkillCount; ++i )
		//	{
		//		if( 1 < m_pSession->GetSkill()->GetSkillLevel( vlSkillIDsToReset.at( i ) ) )
		//		{
		//			bOnlyHaveDefaultSkill = false;
		//			break;
		//		}
		//	}

		//	if( bOnlyHaveDefaultSkill )
		//	{
		//		iResult = ERROR_ITEM_DONT_HAVE_SKILL_TO_RESET;
		//	}
		//}

		//if( false == bOnlyHaveDefaultSkill )
		//{
			// 돌려 받을 SP 계산.
			for( int i = 0; i <(int)vlSkillIDsToReset.size(); ++i )
			{
				int iSkillID = vlSkillIDsToReset.at( i );
				int iSkillLevel = m_pSession->GetSkill()->GetSkillLevel( iSkillID );
				_ASSERT( 0 < iSkillLevel && "0 레벨 짜리는 리셋으로 골라져 나오면 안됨." );
				TSkillData* pSkillData = g_pDataManager->GetSkillData( iSkillID );
				if(!pSkillData) continue;

				for( int iLevel = 0; iLevel <= iSkillLevel-1; ++iLevel )
				{
					if( iLevel < (int)pSkillData->vLevelDataList.size() )
					{
						const TSkillLevelData& LevelData = pSkillData->vLevelDataList.at( iLevel );
						iRewardSP += LevelData.nNeedSkillPoint;
					}
					else
					{
						// 스킬을 찍었을 때와 리셋할 때 스킬 레벨 최대치가 다름. 실섭에서는 있을 수 없는 일..
						// 사내 개발 및 테스트 중일 때 죽지 않도록 처리.
						return ERROR_INVALIDPACKET;
					}
				}
			}
		//}
	}

	return iResult;
}


int CDNUserItem::UseSkillResetCashItem( TItemData * pItemData )
{
	int iRewardSP = 0;
	vector<int> vlSkillIDsToReset;
	int iResult = CanUseSkillResetCashItem( pItemData->nTypeParam[0]-1, pItemData->nTypeParam[1]-1, iRewardSP, vlSkillIDsToReset );
	if( ERROR_NONE == iResult )
	{
		// TODO: 리셋할 스킬들(vlSkillIDsToReset)을 모았으면 DB로 리셋 요청~
		m_pSession->GetDBConnection()->QueryResetSkillBySkillIDs( m_pSession, vlSkillIDsToReset );
		m_pSession->GetSkill()->WaitForUseSkillResetCashItemFromDBServer( vlSkillIDsToReset );
	}

	return iResult;
}

#ifdef PRE_ADD_CHANGEJOB_CASHITEM
int CDNUserItem::UseChangeJobCashItem( TItemData* pItemData, int iJobToChange )
{
	int iResult = ERROR_GENERIC_INVALIDREQUEST;

	bool bResult = m_pSession->CheckAndCalcParallelChangeJob( iJobToChange, m_nChangeFirstJob, m_nChangeSecondJob );
	if( bResult )
	{
		// DB 로 전직 아이템 사용 SP 를 보낸다. 
		// 응답오면 빌리지 서버 메모리 상에서도 스킬 리셋을 시키고 현재 직업 정보들을 변경해주고, 클라로 응답을 보내주면 된다.
		int iFirstJobIDBefore = 0;
		int iSecondJobIDBefore = 0;
		TCharacterStatus* pStatus = m_pSession->GetStatusData();
		if( 0 < m_nChangeFirstJob )
		{
			iFirstJobIDBefore = pStatus->cJobArray[ 1 ];
			_ASSERT( iFirstJobIDBefore != 0 );
		}

		if( 0 < m_nChangeSecondJob )
		{
			iSecondJobIDBefore = pStatus->cJobArray[ 2 ];
			_ASSERT( iSecondJobIDBefore != 0 );
		}

		m_pSession->GetDBConnection()->QueryChangeJobCode( m_pSession, iFirstJobIDBefore, m_nChangeFirstJob, iSecondJobIDBefore, m_nChangeSecondJob );
	}

	return iResult;
}

void CDNUserItem::OnResponseChangeJobCode( bool bSuccess )
{
	if( bSuccess )
	{
		if( 0 < m_nChangeFirstJob )
		{
			m_pSession->GetStatusData()->cJobArray[ 1 ] = (char)m_nChangeFirstJob;
			m_pSession->GetStatusData()->cJob = (char)m_nChangeFirstJob;
		}

		if( 0 < m_nChangeSecondJob )
		{
			m_pSession->GetStatusData()->cJobArray[ 2 ] = (char)m_nChangeSecondJob;
			m_pSession->GetStatusData()->cJob = (char)m_nChangeSecondJob;
		}
		m_pSession->GetEventSystem()->OnEvent( EventSystem::OnJobChange );
	}
	else
	{
		// 전직 아이템 사용 실패.
	}

	// 저장해 뒀던 변수들 리셋.
	m_nChangeFirstJob = 0;
	m_nChangeSecondJob = 0;
}
#endif // #ifdef PRE_ADD_CHANGEJOB_CASHITEM

int CDNUserItem::ResetSkillFromQuestScript( void )
{
	int iRewardSP = 0;
	vector<int> vlSkillIDsToReset;
	TCharacterStatus* pStatus = m_pSession->GetStatusData();
	
	int iToJobDegree = 0;
	for( int i = 0; i < JOBMAX; ++i )
	{
		if( 0 == pStatus->cJobArray[ i ] )
		{
			iToJobDegree = i;
			break;
		}
	}

	int iResult = CanUseSkillResetCashItem( 0, iToJobDegree, iRewardSP, vlSkillIDsToReset );
	if( ERROR_NONE == iResult )
	{
		// TODO: 리셋할 스킬들(vlSkillIDsToReset)을 모았으면 DB로 리셋 요청~
		m_pSession->GetDBConnection()->QueryResetSkillBySkillIDs( m_pSession, vlSkillIDsToReset);
		m_pSession->GetSkill()->WaitForUseSkillResetCashItemFromDBServer( vlSkillIDsToReset );
	}

	return iResult;
}
#endif	// _VILLAGESERVER

bool CDNUserItem::RequestRandomItem(CSUseItem *pUseItem)
{
	const TItem *pItem = NULL;
	switch(pUseItem->cInvenType)
	{
	case ITEMPOSITION_INVEN:
		{
			pItem = GetInventory(pUseItem->cInvenIndex);
			if (!pItem) return false;
		}
		break;

	case ITEMPOSITION_CASHINVEN:
		{
			pItem = GetCashInventory(pUseItem->biInvenSerial);
			if (!pItem) return false;
		}
		break;

	default:
		{
			if (!pItem) return false;
		}
		break;
	}

	TItemData *pItemData = g_pDataManager->GetItemData(pItem->nItemID);
	if ( pItemData == NULL ) return false;

	if (pItem->wCount <= 0) return false;	// 0이면 못쓴다
	if (pItemData->cLevelLimit > m_pSession->GetLevel() ) return false;	// 레벨이 낮으면 나가라
	if ( pItemData->nType != ITEMTYPE_RANDOM ) return false;

	switch(pUseItem->cInvenType)
	{
	case ITEMPOSITION_INVEN:
		{
			int nEmptySlotCount = FindBlankInventorySlotCount();
			if( nEmptySlotCount == 0 && pItem->wCount > 1 ) {
				m_pSession->SendRequestRandomItem( pUseItem->cInvenType, -1, 0, -1 );
				return false;
			}
			if( pItemData->nTypeParam[1] > 0 && FindInventorySlot( pItemData->nTypeParam[1], 1 ) == -1 ) {
				m_pSession->SendRequestRandomItem( pUseItem->cInvenType, -1, 0, -2 );
				return false;
			}

			_UpdateInventoryCoolTime( pUseItem->cInvenIndex );
		}
		break;

	case ITEMPOSITION_CASHINVEN:
		{
			if( pItemData->nTypeParam[1] > 0 && IsValidCashItem( pItemData->nTypeParam[1], 1 ) == false ) {
				m_pSession->SendRequestRandomItem( pUseItem->cInvenType, -1, 0, -2 );
				return false;
			}

			_UpdateCashInventoryCoolTime( pUseItem->biInvenSerial );
		}
		break;
	}

	if (pItem->nCoolTime > 0) return false;

	m_pSession->SendRequestRandomItem( pUseItem->cInvenType, pUseItem->cInvenIndex, pUseItem->biInvenSerial, ERROR_NONE );
	m_pSession->BroadcastingEffect(EffectType_Random, EffectState_Open);

	SetRequestTimer( RequestType_UseRandomItem, 2000 );
	return true;
}

bool CDNUserItem::CalcRandomItem(CSCompleteRandomItem *pRandom)
{
	if (!m_pSession->IsWindowState(WINDOW_PROGRESS)) return false;
	const TItem *pItem = NULL;
	switch(pRandom->cInvenType)
	{
	case ITEMPOSITION_INVEN:
		{
			pItem = GetInventory(pRandom->cInvenIndex);
			if (!pItem) return false;
			if (pItem->nSerial != pRandom->biInvenSerial) return false;
		}
		break;

	case ITEMPOSITION_CASHINVEN:
		{
			pItem = GetCashInventory(pRandom->biInvenSerial);
			if (!pItem) return false;
		}
		break;
	}

	if (pItem->wCount <= 0) return false;	// 0이면 못쓴다

	TItemData *pItemData = g_pDataManager->GetItemData(pItem->nItemID);
	if ( pItemData == NULL ) return false;

	if (pItemData->cLevelLimit > m_pSession->GetLevel() ) return false;	// 레벨이 낮으면 나가라
	if( pItemData->nType != ITEMTYPE_RANDOM ) return false;

#if defined(PRE_FIX_73183) // 밑에꺼 끌어올림
	// 여기서 아이템 계산해서 새루 추가해준다.
	int nItemID = 0;
	int nItemCount = 0;
	int nLevel = 1;
	int nDepth = ITEMDROP_DEPTH;

#if defined( PRE_ADD_FARM_DOWNSCALE )
	CDNUserItem::CalcDropItems( m_pSession, pItemData->nTypeParam[0], nItemID, nItemCount, nLevel, nDepth );
#else
	CalcDropItems( pItemData->nTypeParam[0], nItemID, nItemCount, nLevel, nDepth );
#endif // #if defined( PRE_ADD_FARM_DOWNSCALE )
	TItemData* pInsertItemData = NULL;
	if( nItemID > 0)
	{
		pInsertItemData = g_pDataManager->GetItemData(nItemID);
		if( pInsertItemData )
		{
			if(!pInsertItemData->IsCash) // 캐쉬가 아니면 인벤검사..
			{
				int nEmptySlotCount = FindBlankInventorySlotCount();
				if( nEmptySlotCount == 0 && pItem->wCount > 1 ) {
					m_pSession->SendCompleteRandomItem( pRandom->cInvenType, 0, 0, 0, -1 );
					return false;
				}
			}
		}
	}
	
#endif // #if defined(PRE_FIX_73183)

	switch(pRandom->cInvenType)
	{
	case ITEMPOSITION_INVEN:
		{
#if defined(PRE_FIX_73183)
#else
			int nEmptySlotCount = FindBlankInventorySlotCount();
			if( nEmptySlotCount == 0 && pItem->wCount > 1 ) {
				m_pSession->SendCompleteRandomItem( pRandom->cInvenType, 0, 0, 0, -1 );
				return false;
			}
#endif //#if defined(PRE_FIX_73183)
			if( pItemData->nTypeParam[1] > 0 && FindInventorySlot( pItemData->nTypeParam[1], 1 ) == -1 ) {
				m_pSession->SendCompleteRandomItem( pRandom->cInvenType, 0, 0, 0, -2 );
				return false;
			}

			_UpdateInventoryCoolTime( pRandom->cInvenIndex );
			if (pItem->nCoolTime > 0) return false;
			if (!DeleteInventoryBySlot(pRandom->cInvenIndex, 1, pRandom->biInvenSerial, DBDNWorldDef::UseItem::Use)) return false;
		}
		break;

	case ITEMPOSITION_CASHINVEN:
		{
			if( pItemData->nTypeParam[1] > 0 && IsValidCashItem( pItemData->nTypeParam[1], 1 ) == false ) {
				m_pSession->SendCompleteRandomItem( pRandom->cInvenType, 0, 0, 0, -2 );
				return false;
			}

			_UpdateCashInventoryCoolTime( pRandom->biInvenSerial );
			if (pItem->nCoolTime > 0) return false;
			if (!DeleteCashInventoryBySerial(pRandom->biInvenSerial, 1)) return false;
		}
		break;
	}

	_SetItemCoolTime( pItemData->nSkillID, pItemData->nMaxCoolTime );

	// RandomSeed 가 있는 경우 위에 DeleteInventory 에서 Seed 값이 바뀌기 때문에
	// 강제로 srand 해준다.
#if defined( _GAMESERVER )
	_srand( m_pSession->GetGameRoom(), timeGetTime() );
#elif defined( _VILLAGESERVER )
	_srand( timeGetTime() );
#endif // #if defined( _GAMESERVER )

#if defined(PRE_FIX_73183)
#else
	// 여기서 아이템 계산해서 새루 추가해준다.
	int nItemID = 0;
	int nItemCount = 0;
	int nLevel = 1;
	int nDepth = ITEMDROP_DEPTH;

#if defined( PRE_ADD_FARM_DOWNSCALE )
	CDNUserItem::CalcDropItems( m_pSession, pItemData->nTypeParam[0], nItemID, nItemCount, nLevel, nDepth );
#else
	CalcDropItems( pItemData->nTypeParam[0], nItemID, nItemCount, nLevel, nDepth );
#endif // #if defined( PRE_ADD_FARM_DOWNSCALE )
#endif //#if defined(PRE_FIX_73183)

	int nResult = ERROR_GENERIC_UNKNOWNERROR;

	if( nItemID > 0 ) {
#if defined(PRE_FIX_73183)
		if( pInsertItemData )
		{
			if( pInsertItemData->IsCash )			
				nResult = CreateCashInvenItem( nItemID, nItemCount, DBDNWorldDef::AddMaterializedItem::RandomItem, -1, 0, 0, pItemData->nItemID );			
			else			
				nResult = CreateInvenItem1( nItemID, nItemCount, -1, -1, DBDNWorldDef::AddMaterializedItem::RandomItem, 0 );			
		}
#else
		switch(pRandom->cInvenType)
		{
		case ITEMPOSITION_INVEN:
			nResult = CreateInvenItem1( nItemID, nItemCount, -1, -1, DBDNWorldDef::AddMaterializedItem::RandomItem, 0 );
			break;

		case ITEMPOSITION_CASHINVEN:
			nResult = CreateCashInvenItem( nItemID, nItemCount, DBDNWorldDef::AddMaterializedItem::RandomItem, -1, 0, 0, pItemData->nItemID );
			break;
		}
#endif //#if defined(PRE_FIX_73183)
	}

	// 랜덤아이템 생성할 때 퀘스트 OnCounting을 호출한다.
	if (nResult == ERROR_NONE) 
		m_pSession->GetQuest()->OnAddItem(nItemID, nItemCount);		


	m_pSession->SendCompleteRandomItem( pRandom->cInvenType, (char)nLevel, nItemID, nItemCount, 0 );

	switch(nLevel)
	{
	case 1: m_pSession->BroadcastingEffect(EffectType_Random, EffectState_Bad); break;
	case 2: m_pSession->BroadcastingEffect(EffectType_Random, EffectState_Normal); break;
	case 3: m_pSession->BroadcastingEffect(EffectType_Random, EffectState_Good); break;
	case 4: m_pSession->BroadcastingEffect(EffectType_Random, EffectState_VeryGood); break;
	}

	return true;
}

bool CDNUserItem::RequestWorldMsgItem(CSUseItem *pUseItem)
{
	const TItem *pItem = NULL;

	switch(pUseItem->cInvenType)
	{
	case ITEMPOSITION_INVEN:
		{
			// 일단 캐쉬만 한다. 나중에 일반템도 하려면 패킷 만들어야함
			/*
			pItem = GetInventory(nSlotIndex);
			if (!pItem) return false;

			_UpdateInventoryCoolTime( nSlotIndex );
			*/
		}
		break;

	case ITEMPOSITION_CASHINVEN:
		{
			pItem = GetCashInventory(pUseItem->biInvenSerial);
			if (!pItem) return false;

			_UpdateCashInventoryCoolTime(pUseItem->biInvenSerial);
		}
		break;

	default:
		{
			if (!pItem) return false;
		}
		break;
	}

	if (pItem->nCoolTime > 0) return false;
	TItemData *pItemData = g_pDataManager->GetItemData(pItem->nItemID);
	if (!pItemData) return false;

	switch(pUseItem->cInvenType)
	{
	case ITEMPOSITION_INVEN:
		break;

	case ITEMPOSITION_CASHINVEN:
		m_pSession->SendRequestCashWorldMsg(pUseItem->biInvenSerial, (char)pItemData->nTypeParam[0]);
		break;
	}

	return true;
}

namespace DropItemNameSpace {
	struct DropTempStruct {
		bool bIsGroup;
		int nItemID;
		int nCount;
		int nOffset;
	};
};

#if defined( PRE_ADD_FARM_DOWNSCALE )
void CDNUserItem::CalcDropItems( CDNUserSession* pSession, int nDropItemTableID, int &nResultItemID, int &nResultItemCount, int &nLastValue, int &nDepth )
#else
void CDNUserItem::CalcDropItems( int nDropItemTableID, int &nResultItemID, int &nResultItemCount, int &nLastValue, int &nDepth )
#endif // #if defined( PRE_ADD_FARM_DOWNSCALE )
{
	nDepth -= 1;
	if( nDepth < 0 ) return;

	using namespace DropItemNameSpace;
	DNVector(DropTempStruct) VecList;

	int nOffset = 0;
	DropTempStruct Struct;

	TItemDropData *pDropData = g_pDataManager->GetItemDropData( nDropItemTableID );
	if( !pDropData ) return;

	for( DWORD i=0; i<20; i++ ) {
		bool bGroup = pDropData->bIsGroup[i];

		int nIndex = pDropData->nIndex[i];
		if( nIndex < 1 ) continue;

		int nProb = pDropData->nProb[i];
		if( nProb <= 0 ) continue;

		int nInfo = pDropData->nInfo[i];
		if( !bGroup ) {
			if( nInfo < 1 ) continue;
			if( !g_pDataManager->GetItemData( nIndex ) ) continue;
		}

		nOffset += nProb;

		Struct.bIsGroup = bGroup;
		Struct.nItemID = nIndex;
		Struct.nCount = nInfo;
		Struct.nOffset = nOffset;
		VecList.push_back( Struct );
	}

#if defined( PRE_ADD_FARM_DOWNSCALE )
#if defined(_GAMESERVER)
	unsigned int nSeed = 0;
	if( pSession == NULL )
	{
		CMtRandom CalcDropItemRandom;
		static long lCalcDropItemSRand = timeGetTime();
		CalcDropItemRandom.srand( ::InterlockedIncrement(&lCalcDropItemSRand) );
		nSeed = CalcDropItemRandom.rand()%1000000000;
	}
	else
	{
#if defined(PRE_ADD_77490)		
		errno_t err = rand_s(&nSeed);
		if (err != 0) return ;
		nSeed = nSeed%1000000000;
#else
		nSeed = _rand(pSession->GetGameRoom())%1000000000;
#endif //#if defined(PRE_ADD_77490)
	}
#elif defined(_VILLAGESERVER)
	int nSeed = _rand()%1000000000;
#endif
#else
#if defined(_GAMESERVER)
#if defined(PRE_ADD_77490)
	unsigned int nSeed;	
	errno_t err = rand_s(&nSeed);
	if (err != 0) return ;
	nSeed = nSeed%1000000000;
#else
	int nSeed = _rand(m_pSession->GetGameRoom())%1000000000;
#endif //#if defined(PRE_ADD_77490)
#elif defined(_VILLAGESERVER)
	int nSeed = _rand()%1000000000;
#endif
#endif // #if defined( PRE_ADD_FARM_DOWNSCALE )

	int nPrevOffset = 0;
	for( DWORD i=0; i<VecList.size(); i++ ) {
		if( nSeed >= nPrevOffset && nSeed < VecList[i].nOffset ) {
			if( VecList[i].bIsGroup ) {
				if( VecList[i].nCount > 0 ) nLastValue = VecList[i].nCount;
#if defined( PRE_ADD_FARM_DOWNSCALE )
				CDNUserItem::CalcDropItems( pSession, VecList[i].nItemID, nResultItemID, nResultItemCount, nLastValue, nDepth );
#else
				CalcDropItems( VecList[i].nItemID, nResultItemID, nResultItemCount, nLastValue, nDepth );
#endif // #if defined( PRE_ADD_FARM_DOWNSCALE )
			}
			else {
				nResultItemID = VecList[i].nItemID;
				nResultItemCount = VecList[i].nCount;
			}
			break;
		}
		nPrevOffset = VecList[i].nOffset;
	}
}

void CDNUserItem::SetRequestTimer( RequestTypeEnum Type, DWORD dwInterval )
{
	m_RequestType = Type;
	m_dwRequestTimer = timeGetTime();
	m_dwRequestInterval = dwInterval;
}

bool CDNUserItem::IsValidRequestTimer( RequestTypeEnum Type )
{
	DWORD dwCurTime = timeGetTime();
	if( dwCurTime - m_dwRequestTimer < m_dwRequestInterval / 2 ) return false;
	return true;
}

CDNMissionSystem *CDNUserItem::GetMissionSystem() 
{ 
	return m_pSession->GetMissionSystem(); 
}

void CDNUserItem::OnRecvMoveCoinInventoWare(char cType, INT64 nMoveCoin)
{
	if (!m_pSession->CheckEnoughCoin(nMoveCoin)){	// 인벤에 돈이 없다
		m_pSession->SendMoveCoin(cType, 0, 0, 0, ERROR_ITEM_INVENTOWARE01);
		return;
	}
	if (!m_pSession->CheckMaxWarehouseCoin(nMoveCoin)){	// 창고에 넣을 자리가 없다
		m_pSession->SendMoveCoin(cType, 0, 0, 0, ERROR_ITEM_INVENTOWARE02);
		return;
	}

	// AddWarehouseCoin SP 호출이 한번에 이루어지기 때문에 DelCoin 에서 Code 를 일부로 0 으로 넣어준다.
	m_pSession->DelCoin(nMoveCoin, DBDNWorldDef::CoinChangeCode::DoNotDBSave, 0, false);	// 인벤에서 빼고
	m_pSession->AddWarehouseCoin(nMoveCoin, DBDNWorldDef::CoinChangeCode::InvenToWare, 0);	// 창고에 넣어준다

	m_pSession->SendMoveCoin(cType, nMoveCoin, m_pSession->GetCoin(), m_pSession->GetWarehouseCoin(), ERROR_NONE);

	m_pSession->GetEventSystem()->OnEvent( EventSystem::OnWarehouseKeep, 4,
		EventSystem::ItemID, 0,
		EventSystem::WarehouseCoin, (int)m_pSession->GetWarehouseCoin(),
		EventSystem::InvenCoin, (int)m_pSession->GetCoin(),
		EventSystem::UseCoinCount, (int)nMoveCoin );
}

void CDNUserItem::OnRecvMoveCoinWaretoInven(char cType, INT64 nMoveCoin)
{
	if (!m_pSession->CheckEnoughWarehouseCoin(nMoveCoin)){	// 창고에 돈이 없다
		m_pSession->SendMoveCoin(cType, 0, 0, 0, ERROR_ITEM_WARETOINVEN01);
		return;
	}

	if (!m_pSession->CheckMaxCoin(nMoveCoin)){	// 인벤에 넣을 자리가 없다
		m_pSession->SendMoveCoin(cType, 0, 0, 0, ERROR_ITEM_WARETOINVEN02);
		return;
	}

	m_pSession->DelWarehouseCoin(nMoveCoin, DBDNWorldDef::CoinChangeCode::WareToInven, 0);	// 창고에 빼고
	m_pSession->AddCoin(nMoveCoin, DBDNWorldDef::CoinChangeCode::DoNotDBSave, 0, false);	// 인벤에 넣고

	m_pSession->SendMoveCoin(cType, nMoveCoin, m_pSession->GetCoin(), m_pSession->GetWarehouseCoin(), ERROR_NONE);
}

bool CDNUserItem::bIsSameItem( const TItem* pItem, const TItem* pItem2 )
{
	if (pItem->bEternity){
		if( pItem->nItemID == pItem2->nItemID && pItem->bSoulbound == pItem2->bSoulbound && pItem->cSealCount == pItem2->cSealCount )	
			return true;
	}
	else{
		if( pItem->nItemID == pItem2->nItemID && pItem->bSoulbound == pItem2->bSoulbound && pItem->cSealCount == pItem2->cSealCount && pItem->bEternity == pItem2->bEternity && pItem->tExpireDate == pItem2->tExpireDate )	
			return true;
	}

	return false;
}

bool CDNUserItem::bIsDifferentItem( const TItem* pItem, const TItem* pItem2 )
{
	return !bIsSameItem( pItem, pItem2 );
}

bool CDNUserItem::bIsSameItem( const TItem* pItem, const TInvenItemCnt* pItem2 )
{
	if( pItem->nItemID == pItem2->nItemID && pItem->bSoulbound == pItem2->bSoulbound && pItem->cSealCount == pItem2->cSealCount )
		return true;

	return false;
}

bool CDNUserItem::bIsDifferentItem( const TItem* pItem, const TInvenItemCnt* pItem2 )
{
	return !bIsSameItem( pItem, pItem2 );
}

bool CDNUserItem::MakeItemStruct(int nItemID, TItem &ResultItem, int nShopItemPeriod/* = 0*/, int nOptionTableID/* = 0*/)
{
	TItemData *pItemData = g_pDataManager->GetItemData(nItemID);
	if (!pItemData) return false;
	if (pItemData->nOverlapCount <= 0) return false;

	memset(&ResultItem, 0, sizeof(TItem));
	ResultItem.nItemID = nItemID;
	ResultItem.nSerial = MakeItemSerial();
	ResultItem.wCount = 1;
	int nItemPeriod = pItemData->nPeriod;
#if defined(PRE_ADD_COMBINEDSHOP_PERIOD)
	if (nShopItemPeriod > 0)
		nItemPeriod = nShopItemPeriod;
#endif	// #if defined(PRE_ADD_COMBINEDSHOP_PERIOD)
	if (nItemPeriod > 0){
		ResultItem.nLifespan = nItemPeriod * 24 * 60;;
	}
	else{
		nItemPeriod = LIFESPANMAX;	// 무한이면 한 5년으로 넣어놓으면 되겠지? -_-;
		ResultItem.nLifespan = LIFESPANMAX;
		ResultItem.bEternity = true;
	}

	time_t CreateTime;
	time(&CreateTime);
	ResultItem.tExpireDate = CreateTime + (nItemPeriod * 86400);	// 일단 임시로;(디비에서 값 받아오면 달라질꺼다)

	switch( pItemData->nType ) {
		case ITEMTYPE_WEAPON:
			{
				TWeaponData *pWeaponData = g_pDataManager->GetWeaponData(nItemID);
				if( pWeaponData ) ResultItem.wDur = pWeaponData->nDurability;
			}
			break;
		case ITEMTYPE_PARTS:
			{
				TPartData *pPartData = g_pDataManager->GetPartData(nItemID);
				if( pPartData ) ResultItem.wDur = pPartData->nDurability;
			}
			break;
		default:
			ResultItem.wDur = 0;
	}

	// 최초 생성 todo : extract method
	if (pItemData->cReversion == ITEMREVERSION_BELONG )
	{
		ResultItem.bSoulbound = true;
		ResultItem.cSealCount = pItemData->cSealCount;
	}

	CMtRandom Random;
	Random.srand( timeGetTime() );

	switch (pItemData->nType)
	{
	case ITEMTYPE_WEAPON:
	case ITEMTYPE_PARTS:
	case ITEMTYPE_GLYPH:
#if defined(PRE_ADD_TALISMAN_SYSTEM)
	case ITEMTYPE_TALISMAN:
#endif
		{
			ResultItem.nRandomSeed = Random.rand();

			if( ResultItem.cOption == 0 ) {
				if( pItemData->nTypeParam[0] > 0 ) {
					TPotentialData *pPotential = g_pDataManager->GetPotentialData( pItemData->nTypeParam[0] );
					if( pPotential && pPotential->nTotalProb > 0 ) {
						int nRand = Random.rand() % pPotential->nTotalProb;
						for (int i = 0; i <(int)pPotential->pVecItemData.size(); i++){
							if (nRand < pPotential->pVecItemData[i]->nPotentailOffset){
								ResultItem.cOption = i + 1;
								break;
							}
						}
					}
				}
			}
		}
		break;
	}

	if (nOptionTableID > 0){
		const TItemOptionTableData *pItemOption = g_pDataManager->GetItemOptionTableData(nOptionTableID);
		if (pItemOption){
			if (pItemOption->iEnchant > 0)
				ResultItem.cLevel = static_cast<char>(pItemOption->iEnchant);
			if (pItemOption->iOption > 0)
				ResultItem.cOption = static_cast<char>(pItemOption->iOption);
			if (pItemOption->iPotential > 0)
				ResultItem.cPotential = static_cast<char>(pItemOption->iPotential);
		}
	}

	return true;
}

int CDNUserItem::MakeCashItemStruct(int nItemSN, int nItemID, TItem &CashItem, char cOption/* = -1*/, int nPeriod/* = 0*/)
{
	int nCount = 1;
	if (nItemID <= 0) return ERROR_ITEM_NOTFOUND;
	if (nItemSN > 0){
		nCount = g_pDataManager->GetCashCommodityCount(nItemSN);
		if (nCount <= 0) return ERROR_ITEM_NOTFOUND;
	}

	TItemData *pItemData = g_pDataManager->GetItemData(nItemID);
	if (!pItemData) return ERROR_ITEM_NOTFOUND;
	if (pItemData->nOverlapCount <= 0) return ERROR_ITEM_FAIL;
	if (!pItemData->IsCash) return ERROR_ITEM_FAIL;	// 캐쉬템이 아니면 넣을 수 없다

	// 집어넣을 아이템 세팅
	TItem AddItem;
	memset(&AddItem, 0, sizeof(TItem));
	AddItem.nItemID = nItemID;
	AddItem.wCount = nCount;

	int nItemPeriod = nPeriod;
	if (nItemSN > 0)
		nItemPeriod = g_pDataManager->GetCashCommodityPeriod(nItemSN);

	int nSerialPeriod = nItemPeriod;
	if (nItemPeriod <= 0){
		nSerialPeriod = LIFESPANMAX;	// 무한이면 한 5년으로 넣어놓으면 되겠지? -_-;
		AddItem.nLifespan = LIFESPANMAX;
		AddItem.bEternity = true;
	}
	else{
		AddItem.nLifespan = nSerialPeriod * 24 * 60;
	}
	AddItem.nSerial = MakeCashItemSerial(nSerialPeriod);
	time_t CreateTime;
	time(&CreateTime);
	AddItem.tExpireDate = CreateTime + (nSerialPeriod * 86400);	// 일단 임시로;(디비에서 값 받아오면 달라질꺼다)

	switch( pItemData->nType ) {
		case ITEMTYPE_WEAPON:
			{
				TWeaponData *pWeaponData = g_pDataManager->GetWeaponData(nItemID);
				if( pWeaponData ) AddItem.wDur = pWeaponData->nDurability;
			}
			break;
		case ITEMTYPE_PARTS:
			{
				TPartData *pPartData = g_pDataManager->GetPartData(nItemID);
				if( pPartData ) AddItem.wDur = pPartData->nDurability;
			}
			break;
		default:
			AddItem.wDur = 0;
	}

	AddItem.bSoulbound = true;
	AddItem.cSealCount = pItemData->cSealCount;
	if (AddItem.bEternity)
		AddItem.cSealCount = g_pDataManager->GetItemCashTradeCount(nItemID);

	switch (pItemData->nType)
	{
	case ITEMTYPE_WEAPON:
	case ITEMTYPE_PARTS:
		{
			CMtRandom Random;
			Random.srand( timeGetTime() );
			AddItem.nRandomSeed = Random.rand();

			if (cOption != -1)
			{
				AddItem.cOption = cOption;
			}
			else
			{
				if( pItemData->nTypeParam[0] > 0 ) {
					TPotentialData *pPotential = g_pDataManager->GetPotentialData( pItemData->nTypeParam[0] );
					if( pPotential && pPotential->nTotalProb > 0 ) {
						int nRand = Random.rand() % pPotential->nTotalProb;
						for (int i = 0; i <(int)pPotential->pVecItemData.size(); i++){
							if (nRand < pPotential->pVecItemData[i]->nPotentailOffset){
								AddItem.cOption = i + 1;
								break;
							}
						}
					}
				}
			}
		}
		break;
	}

	CashItem = AddItem;
	return ERROR_NONE;
}

int CDNUserItem::MakeVehicleItemStruct(const TItem &BodyItem, TVehicle &AddVehicle)
{
	memset(&AddVehicle, 0, sizeof(TVehicle));

	TVehicleData *pVehicleData = g_pDataManager->GetVehicleData(BodyItem.nItemID);
	if (!pVehicleData) return -1;

	AddVehicle.Vehicle[Vehicle::Slot::Body] = BodyItem;

	TItem PartsItem = {0,};
	MakeCashItemStruct(0, pVehicleData->nDefaultParts1, PartsItem);
	AddVehicle.Vehicle[Vehicle::Slot::Saddle] = PartsItem;

	memset(&PartsItem, 0, sizeof(PartsItem));
	MakeCashItemStruct(0, pVehicleData->nDefaultParts2, PartsItem);
	AddVehicle.Vehicle[Vehicle::Slot::Hair] = PartsItem;

	AddVehicle.dwPartsColor1 = g_pDataManager->GetItemTypeParam1(BodyItem.nItemID);
	AddVehicle.dwPartsColor2 = g_pDataManager->GetItemTypeParam2(BodyItem.nItemID);
	AddVehicle.nRange = pVehicleData->nRange;

	AddVehicle.nSkillID1 = pVehicleData->nPetSkillID1;
	AddVehicle.nSkillID2 = pVehicleData->nPetSkillID2;
	if( AddVehicle.nSkillID2 > 0 ) //스킬 2번이 뚫려있으면 슬롯이 확장된걸로 한다.
		AddVehicle.bSkillSlot = true;

	if( !AddVehicle.Vehicle[Vehicle::Slot::Body].bEternity )
		AddVehicle.nType |= Pet::Type::ePETTYPE_EXPIREDATE;
	if( pVehicleData->nFoodID > 0)
	{					
		TPetFoodData* pPetFoodData = g_pDataManager->GetPetFoodData( pVehicleData->nFoodID );
		if( pPetFoodData )					
		{
			AddVehicle.nCurrentSatiety = pPetFoodData->nFullMaxCount;				
			AddVehicle.nType |= Pet::Type::ePETTYPE_SATIETY;
			AddVehicle.dwLastHungerTick = timeGetTime();
			m_pSession->GetDBConnection()->QueryModDegreeOfHunger(m_pSession->GetDBThreadID(),m_pSession->GetWorldSetID(), m_pSession->GetAccountDBID(),
				AddVehicle.Vehicle[Pet::Slot::Body].nSerial, AddVehicle.nCurrentSatiety );
		}
	}

	return ERROR_NONE;
}

int CDNUserItem::MakeBuyCashItem(int nItemSN, int nItemID, int nOption, TCashItemBase &OutputItem)
{
	int nRet = MakeCashItemStruct(nItemSN, nItemID, OutputItem.CashItem, nOption, 0);
	if (nRet != ERROR_NONE)
		return nRet;

	OutputItem.nItemSN = nItemSN;

	switch (g_pDataManager->GetItemMainType(nItemID))
	{
	case ITEMTYPE_VEHICLE:
	case ITEMTYPE_PET:
		{
			TVehicle AddVehicle;
			memset(&AddVehicle, 0, sizeof(AddVehicle));

			nRet = MakeVehicleItemStruct(OutputItem.CashItem, AddVehicle);
			if (nRet != ERROR_NONE)
				return nRet;

			OutputItem.dwPartsColor1 = AddVehicle.dwPartsColor1;
			OutputItem.dwPartsColor2 = AddVehicle.dwPartsColor2;
			OutputItem.VehiclePart1.SetItem(AddVehicle.Vehicle[Vehicle::Slot::Saddle]);
			OutputItem.VehiclePart2.SetItem(AddVehicle.Vehicle[Vehicle::Slot::Hair]);
		}
		break;

	case ITEMTYPE_WEAPON:
	case ITEMTYPE_PARTS:
		{
			int nPotentialID = g_pDataManager->GetItemTypeParam1(nItemID);
			if (nPotentialID > 0){
				TPotentialData *pPotential = g_pDataManager->GetPotentialData(nPotentialID);
				if (!pPotential) return ERROR_ITEM_FAIL;
				if (static_cast<int>(pPotential->pVecItemData.size()) < nOption) return ERROR_ITEM_FAIL;
			}
		}
		break;

	default:
		break;
	}

	return ERROR_NONE;
}

int CDNUserItem::MakeGiftCashItem(int nItemSN, int nItemID, int nOption, TGiftItem &OutputItem)
{
	TCashItemBase CashBase = {0,};

	int nRet = MakeBuyCashItem(nItemSN, nItemID, nOption, CashBase);
	if (nRet != ERROR_NONE) return nRet;

	OutputItem.nItemSN = CashBase.nItemSN;
	OutputItem.AddItem = CashBase.CashItem;
	OutputItem.dwPartsColor1 = CashBase.dwPartsColor1;
	OutputItem.dwPartsColor2 = CashBase.dwPartsColor2;
	OutputItem.VehiclePart1 = CashBase.VehiclePart1;
	OutputItem.VehiclePart2 = CashBase.VehiclePart2;

	return ERROR_NONE;
}

void CDNUserItem::SetInventoryCount(int nCount)
{
	if ((nCount <= 0) ||(nCount > INVENTORYMAX)) return;
	m_cInventoryCount = nCount;
}

BYTE CDNUserItem::GetInventoryCount() const
{
	return m_cInventoryCount;	// 나중에 캐쉬템 들어가면 더해줘야함
}

void CDNUserItem::SetGlyphExtendCount( int iCount )
{
#if defined(PRE_ADD_DRAGON_FELLOWSHIP_GLYPH)
	if( iCount < 0 ||(iCount > GLYPH_CASH_TOTAL_MAX) ) 
#else
	if( iCount < 0 ||(iCount > GLYPHMAX-GLYPH_CASH1) ) 
#endif //#if defined(PRE_ADD_DRAGON_FELLOWSHIP_GLYPH)
		return;
	m_iGlyphExtendCount = iCount;	
}

int CDNUserItem::GetGlyphEntendCount() const
{
	return m_iGlyphExtendCount;
}

void CDNUserItem::SetGlyphExpireDate( BYTE iCount, __time64_t tExpireDate )
{
	if( (iCount >= CASHGLYPHSLOTMAX) ) 
		return;
	//캐시샵 구입시 한번 체크하지만 혹시 모르니...
	if( !m_TCashGlyphData[iCount].cActiveGlyph )
	{
		m_TCashGlyphData[iCount].cActiveGlyph = iCount+1;
		m_TCashGlyphData[iCount].tGlyphExpireDate = tExpireDate;
	}
}

TCashGlyphData* CDNUserItem::GetGlyphExpireDate()
{
	return m_TCashGlyphData;
}

void CDNUserItem::SetWarehouseCount(int nCount)
{
	if ((nCount <= 0) ||(nCount > WAREHOUSEMAX)) return;
	m_cWarehouseCount = nCount;
}

BYTE CDNUserItem::GetWarehouseCount() const
{
	return m_cWarehouseCount;	// 나중에 캐쉬템 들어가면 더해줘야함
}

int CDNUserItem::GetCashInventoryCount() const
{
	if (m_MapCashInventory.empty()) return 0;
	return(int)m_MapCashInventory.size();
}

// Cashshop
bool CDNUserItem::IsEffectCashItem(int nItemID) const
{
	int nItemType = g_pDataManager->GetItemMainType(nItemID);

	switch(nItemType)
	{
	case ITEMTYPE_REBIRTH_COIN:
	case ITEMTYPE_INVENTORY_SLOT:
	case ITEMTYPE_WAREHOUSE_SLOT:
	case ITEMTYPE_GESTURE:
	case ITEMTYPE_GUILDWARE_SLOT:
	case ITEMTYPE_FARM_VIP:
	case ITEMTYPE_GLYPH_SLOT:
	case ITEMTYPE_PERIOD_PLATE:
#if defined(PRE_ADD_TALISMAN_SYSTEM)
	case ITEMTYPE_PERIOD_TALISMAN_EX:
#endif
		return true;
	}

	return false;
}

int CDNUserItem::CheckEffectItemListCountLimit(std::vector<TEffectItemData> &VecItemList, bool bGift, bool bIgnoreLimit, bool bReceiveGiftAll)
{
	if (VecItemList.empty()) return ERROR_NONE;

	int nTotalRebirthCoin = m_pSession->GetRebirthCashCoin();
	int nTotalInven = GetInventoryCount();
	int nTotalWare = GetWarehouseCount();
	int nTotalGuildWare =  m_pSession->GetGuildWareAllowCount();

	std::vector<int> VecGestureList;
	VecGestureList.clear();

	bool bFarmItem = false, bTotalLevelSkill = false;

	TItemData *pItemData = NULL;
	for (int i = 0; i < (int)VecItemList.size(); i++){
		pItemData = g_pDataManager->GetItemData(VecItemList[i].nItemID);
		if (!pItemData) continue;

		switch(pItemData->nType)
		{
		case ITEMTYPE_REBIRTH_COIN:
		case ITEMTYPE_REBIRTH_COIN_EX:
			{
				int nMax = g_pDataManager->GetCashCoinLimit(m_pSession->GetLevel());
				nTotalRebirthCoin += VecItemList[i].nCount;
				int nRet = ERROR_CASHSHOP_COUNTOVER;
				if (bGift) nRet = ERROR_GIFT_COUNTOVER_REBIRTHCOIN;
				if (nTotalRebirthCoin > nMax){
					if (bReceiveGiftAll){
						VecItemList[i].bFail = true;
						nTotalRebirthCoin -= VecItemList[i].nCount;
					}
					else
						return nRet;
				}
			}
			break;

		case ITEMTYPE_INVENTORY_SLOT:
		case ITEMTYPE_INVENTORY_SLOT_EX:
			{
#if defined(_JP)
				if (bIgnoreLimit) continue;
#endif	// #if defined(_JP)
				nTotalInven += pItemData->nTypeParam[0];
				int nRet = ERROR_CASHSHOP_COUNTOVER;
				if (bGift) nRet = ERROR_GIFT_COUNTOVER_INVEN;
				if (nTotalInven > INVENTORYMAX){
					if (bReceiveGiftAll){
						VecItemList[i].bFail = true;
						nTotalInven -= pItemData->nTypeParam[0];
					}
					else
						return nRet;
				}
			}
			break;

		case ITEMTYPE_GLYPH_SLOT:
		case ITEMTYPE_PERIOD_PLATE:
		case ITEMTYPE_PERIOD_PLATE_EX:
		{
#if defined(_JP)
			if (bIgnoreLimit) continue;
#endif	// #if defined(_JP)

			int nTotalGlyph = GetGlyphEntendCount();
			nTotalGlyph += pItemData->nTypeParam[0];
			int nRet = ERROR_CASHSHOP_COUNTOVER;
			if (bGift) nRet = ERROR_GIFT_COUNTOVER_GLYPH;
#if defined(PRE_ADD_DRAGON_FELLOWSHIP_GLYPH)
			if (nTotalGlyph > GLYPH_CASH_TOTAL_MAX)
#else
			if (nTotalGlyph > GLYPHMAX-GLYPH_CASH1) 
#endif //#if defined(PRE_ADD_DRAGON_FELLOWSHIP_GLYPH)
			{
				if (bReceiveGiftAll)
					VecItemList[i].bFail = true;
				else
					return nRet;
			}

			if (pItemData->nTypeParam[0] == 1)
			{
				//한개씩 오픈인 경우 해당 문장 슬롯이 오픈중인지 체크
				int nIndex = pItemData->nTypeParam[1];
				if (nIndex < 1)
					nIndex = 1;
				TCashGlyphData* GlyphDate = GetGlyphExpireDate();
				if (GlyphDate[nIndex-1].cActiveGlyph){
					if (bReceiveGiftAll)
						VecItemList[i].bFail = true;
					else
						return nRet;
				}
			}
			break;
		}

		case ITEMTYPE_WAREHOUSE_SLOT:
		case ITEMTYPE_WAREHOUSE_SLOT_EX:
			{
#if defined(_JP)
				if (bIgnoreLimit) continue;
#endif	// #if defined(_JP)
				nTotalWare += pItemData->nTypeParam[0];
				int nRet = ERROR_CASHSHOP_COUNTOVER;
				if (bGift) nRet = ERROR_GIFT_COUNTOVER_WARE;
				if (nTotalWare > WAREHOUSEMAX){
					if (bReceiveGiftAll){
						VecItemList[i].bFail = true;
						nTotalWare -= pItemData->nTypeParam[0];
					}
					else
						return nRet;
				}
			}
			break;

		case ITEMTYPE_GESTURE:
			{
				int nGestureID = pItemData->nTypeParam[0];
				if( m_pSession->HasGesture( nGestureID ) ){
					if (bReceiveGiftAll)
						VecItemList[i].bFail = true;
					else
						return ERROR_CASHSHOP_DUPLICATE_GESTURE;
				}
				else{
					if (bReceiveGiftAll){
						if (find(VecGestureList.begin(), VecGestureList.end(), nGestureID) != VecGestureList.end()){
							VecItemList[i].bFail = true;
						}
						else
							VecGestureList.push_back(nGestureID);
					}
				}
			}
			break;

		case ITEMTYPE_GUILDWARE_SLOT:
			{
				if (nTotalGuildWare == -1){
					if (bReceiveGiftAll)
						VecItemList[i].bFail = true;
					else
						return ERROR_GUILD_YOUR_NOT_BELONGANYGUILD;
				}

				int nGuildRoleType = m_pSession->GetGuildRoleType();
				if (nGuildRoleType == -1){
					if (bReceiveGiftAll)
						VecItemList[i].bFail = true;
					else
						return ERROR_GUILD_YOUR_NOT_BELONGANYGUILD;
				}

				if (GUILDROLE_TYPE_MASTER != static_cast<eGuildRoleType>(nGuildRoleType)){
					if (bReceiveGiftAll)
						VecItemList[i].bFail = true;
					else
						return ERROR_GUILD_ONLYAVAILABLE_GUILDMASTER;
				}

				nTotalGuildWare += pItemData->nTypeParam[0];
				int nRet = ERROR_CASHSHOP_COUNTOVER;
				if (bGift) nRet = ERROR_GIFT_COUNTOVER_WARE;

				if (nTotalGuildWare > GUILD_WAREHOUSE_MAX){
					if (bReceiveGiftAll){
						VecItemList[i].bFail = true;
						nTotalGuildWare -= pItemData->nTypeParam[0];
					}
					else
						return nRet;
				}
			}
			break;

#if defined( PRE_ADD_VIP_FARM )
		case ITEMTYPE_FARM_VIP:
			{
				if( m_pSession->GetEffectRepository()->bIsExpiredItem( ITEMTYPE_FARM_VIP ) == false ){
					if (bReceiveGiftAll)
						VecItemList[i].bFail = true;
					else
						return ERROR_CASHSHOP_COUNTOVER;
				}
				else{
					if (bFarmItem)
						VecItemList[i].bFail = true;
					else
						bFarmItem = true;
				}
				break;
			}
#endif // #if defined( PRE_ADD_VIP_FARM )
#if defined( PRE_ADD_TOTAL_LEVEL_SKILL )
		case ITEMTYPE_TOTALSKILLLEVEL_SLOT:
			{
				int nIndex = 0;
				if( pItemData->nTypeParam[1] > 0 && pItemData->nTypeParam[1] <= TotalLevelSkill::Common::MAXSLOTCOUNT )
					nIndex = pItemData->nTypeParam[1] - 1;
				else
					nIndex = TotalLevelSkill::Common::MAXSLOTCOUNT-1;

				if (m_pSession->bIsTotalLevelSkillCashSlot(nIndex)){
					if (bReceiveGiftAll)
						VecItemList[i].bFail = true;
					else
						return ERROR_CASHSHOP_COUNTOVER;
				}
				else{
					if (bTotalLevelSkill)
						VecItemList[i].bFail = true;
					else
						bTotalLevelSkill = true;
				}
			}
			break;
#endif
#if defined(PRE_ADD_TALISMAN_SYSTEM)
		case ITEMTYPE_PERIOD_TALISMAN_EX:
			{
#if defined(_JP)
				if (bIgnoreLimit) continue;
#endif	// #if defined(_JP)
				int nRet = ERROR_CASHSHOP_COUNTOVER;
				if (bGift) nRet = ERROR_GIFT_COUNTOVER_TALISMAN;

				if( IsTalismanCashSlotEntend() )	//탈리스만 캐쉬슬롯은 한번에 다 열림
				{
					if (bReceiveGiftAll)
						VecItemList[i].bFail = true;
					else
						return nRet;
				}
			}
			break;
#endif
		}
	}

	return ERROR_NONE;
}

int CDNUserItem::CheckEffectItemCountLimit(int nItemID, int nItemCount, bool bGift, bool bIgnoreLimit)
{
	std::vector<TEffectItemData> VecItemList;
	VecItemList.clear();

	TEffectItemData EffectData = {0,};
	EffectData.nItemID = nItemID;
	EffectData.nCount = nItemCount;
	VecItemList.push_back(EffectData);

	return CheckEffectItemListCountLimit(VecItemList, bGift, bIgnoreLimit, false);
}

void CDNUserItem::ApplyCashShopItem(int nItemSN, const TItem &CashItem, DWORD dwPartsColor1/* = 0*/, DWORD dwPartsColor2/* = 0*/, TVehicleItem *pVehiclePart1/* = NULL*/, TVehicleItem *pVehiclePart2/* = NULL*/, bool bDBSave/* = false*/)
{
	// 캐시샵에서 사면 캐시서버에서 디비저장을 하기때문에 따로 디비저장을 하지않음 (default: dbsave false)

	TCashItemBase CashData;
	memset(&CashData, 0, sizeof(TCashItemBase));

	CashData.nItemSN = nItemSN;
	CashData.CashItem = CashItem;
	CashData.dwPartsColor1 = dwPartsColor1;
	CashData.dwPartsColor2 = dwPartsColor2;
	CashData.VehiclePart1 = *pVehiclePart1;
	CashData.VehiclePart2 = *pVehiclePart2;

	ApplyCashShopItem(CashData, bDBSave);
}

void CDNUserItem::ApplyCashShopItem(const TCashItemBase &CashData, bool bDBSave/* = false*/)
{
	// 캐시샵에서 사면 캐시서버에서 디비저장을 하기때문에 따로 디비저장을 하지않음 (default: dbsave false)

	int nItemType = g_pDataManager->GetItemMainType(CashData.CashItem.nItemID);

	switch(nItemType)
	{
	case ITEMTYPE_REBIRTH_COIN:
		{
			int nCount = g_pDataManager->GetCashCommodityCount(CashData.nItemSN);
			if( nCount == 0 )
			{
				//뒤에 로그와 짝이 맞아야 함. 안맞으면 다른문제임
				g_Log.Log( LogType::_ERROR, m_pSession, L"ApplyCashShopItem|[ADBID:%u, CDBID:%I64d, SID:%u] ApplyCashShopItem Failed! ItemSN:%d", m_pSession->GetAccountDBID(), m_pSession->GetCharacterDBID(), m_pSession->GetSessionID(), CashData.nItemSN );
			}

			if (bDBSave)
				m_pSession->AddRebirthCoin(0, nCount);
			else
				m_pSession->AddCashRebirthCoin(nCount);
		}
		break;

	case ITEMTYPE_INVENTORY_SLOT:
	case ITEMTYPE_WAREHOUSE_SLOT:
	case ITEMTYPE_GESTURE:
	case ITEMTYPE_GUILDWARE_SLOT:
#if defined( PRE_ADD_VIP_FARM )
	case ITEMTYPE_FARM_VIP:
#endif // #if defined( PRE_ADD_VIP_FARM )
	case ITEMTYPE_GLYPH_SLOT:
	case ITEMTYPE_PERIOD_PLATE:
#if defined(PRE_ADD_TALISMAN_SYSTEM)
	case ITEMTYPE_PERIOD_TALISMAN_EX:
#endif
	{
		if (bDBSave){
			m_pSession->GetDBConnection()->QueryAddEffectItems(m_pSession, CashData.CashItem.nItemID, DBDNWorldDef::EffectItemLifeSpanType::New, DBDNWorldDef::EffectItemGetCode::Cooper, 
				0, 0, CashData.CashItem.bEternity, CashData.CashItem.nLifespan, CashData.CashItem.nSerial, 0);
		}

		if( nItemType == ITEMTYPE_GESTURE )
		{
			m_pSession->SendCashGestureAdd( g_pDataManager->GetItemTypeParam1(CashData.CashItem.nItemID) );
		}
		__time64_t tExpireDate = CashData.CashItem.tExpireDate;
		if( nItemType == ITEMTYPE_GLYPH_SLOT )
		{
			if (CashData.CashItem.bEternity)
				tExpireDate = 0;
		}

		if( !m_pSession->GetEffectRepository()->Add( CashData.CashItem.nSerial, CashData.CashItem.nItemID, tExpireDate ) )
		{
			_DANGER_POINT();
		}
		//기간제 문장슬롯의 경우 기간이 셋팅되고 클라에 알려줘야 하기 떄문에 Add이후에 위치 시킴
		if( nItemType == ITEMTYPE_GLYPH_SLOT || nItemType == ITEMTYPE_PERIOD_PLATE)
		{
			m_pSession->SendGlyphExpireData( GetGlyphExpireDate() );
		}
#if defined(PRE_ADD_TALISMAN_SYSTEM)
		if( nItemType == ITEMTYPE_PERIOD_TALISMAN_EX )
		{
			m_pSession->SendTalismanExpireData( IsTalismanCashSlotEntend(), GetTalismanExpireDate() );
		}
#endif
		break;
	}

	case ITEMTYPE_CHARACTER_SLOT:
		{
			if (bDBSave){
				m_pSession->GetDBConnection()->QueryModCharacterSlotCount(m_pSession);
			}
		}
		break;

	case ITEMTYPE_VEHICLE:
	case ITEMTYPE_PET:
		{
			TVehicle Item;
			memset(&Item, 0, sizeof(Item));

			Item.dwPartsColor1 = CashData.dwPartsColor1;
			Item.dwPartsColor2 = CashData.dwPartsColor2;

			Item.Vehicle[Vehicle::Slot::Body] = CashData.CashItem;
			if (CashData.VehiclePart1.nItemID > 0){
				Item.Vehicle[Vehicle::Slot::Saddle].nItemID = CashData.VehiclePart1.nItemID;
				Item.Vehicle[Vehicle::Slot::Saddle].nSerial = CashData.VehiclePart1.nSerial;
				Item.Vehicle[Vehicle::Slot::Saddle].wCount = CashData.VehiclePart1.wCount;
				Item.Vehicle[Vehicle::Slot::Saddle].bEternity = CashData.VehiclePart1.bEternity;
				Item.Vehicle[Vehicle::Slot::Saddle].tExpireDate = CashData.VehiclePart1.tExpireDate;
				Item.Vehicle[Vehicle::Slot::Saddle].bSoulbound = true;
			}
			if (CashData.VehiclePart2.nItemID > 0){
				Item.Vehicle[Vehicle::Slot::Hair].nItemID = CashData.VehiclePart2.nItemID;
				Item.Vehicle[Vehicle::Slot::Hair].nSerial = CashData.VehiclePart2.nSerial;
				Item.Vehicle[Vehicle::Slot::Hair].wCount = CashData.VehiclePart2.wCount;
				Item.Vehicle[Vehicle::Slot::Hair].bEternity = CashData.VehiclePart2.bEternity;
				Item.Vehicle[Vehicle::Slot::Hair].tExpireDate = CashData.VehiclePart2.tExpireDate;
				Item.Vehicle[Vehicle::Slot::Hair].bSoulbound = true;
			}
			TVehicleData *pVehicleData = g_pDataManager->GetVehicleData(CashData.CashItem.nItemID);
			if (pVehicleData)
			{
				if( pVehicleData->nPetSkillID1 > 0 )				
					Item.nSkillID1 = pVehicleData->nPetSkillID1;
				if( pVehicleData->nPetSkillID2 > 0 )
					Item.nSkillID2 = pVehicleData->nPetSkillID2;		
				if( Item.nSkillID2 > 0 ) //스킬 2번이 뚫려있으면 슬롯이 확장된걸로 한다.
					Item.bSkillSlot = true;	
			}
			if( !Item.Vehicle[Vehicle::Slot::Body].bEternity )
				Item.nType |= Pet::Type::ePETTYPE_EXPIREDATE;
			if( pVehicleData && pVehicleData->nFoodID > 0)
			{					
				TPetFoodData* pPetFoodData = g_pDataManager->GetPetFoodData( pVehicleData->nFoodID );
				if( pPetFoodData )			
				{
					Item.nCurrentSatiety = pPetFoodData->nFullMaxCount;				
					Item.nType |= Pet::Type::ePETTYPE_SATIETY;
					Item.dwLastHungerTick = timeGetTime();
					m_pSession->GetDBConnection()->QueryModDegreeOfHunger(m_pSession->GetDBThreadID(), m_pSession->GetWorldSetID(), m_pSession->GetAccountDBID(),
						Item.Vehicle[Pet::Slot::Body].nSerial, Item.nCurrentSatiety );
				}
			}
			int nRet = _PushVehicleInventory(Item);
			if (nRet == ERROR_NONE){
				if (bDBSave){
					BYTE cItemLocationCode = DBDNWorldDef::ItemLocation::VehicleInventory;
					if (nItemType == ITEMTYPE_PET)
						cItemLocationCode = DBDNWorldDef::ItemLocation::PetInventory;

					DBSendAddMaterializedItem(0, DBDNWorldDef::AddMaterializedItem::SpecialBoxReward, m_SelectSpecialBoxItem.nEventRewardID, CashData.CashItem, 0, 0, (INT64)0, cItemLocationCode, DBDNWorldDef::PayMethodCode::Coin, false, 0, &Item);
				}

				m_pSession->SendRefreshVehicleInven(Item);
			}
		}
		break;

	default:
		{
			int nRet = CreateCashInvenWholeItem(CashData.CashItem);
			if (nRet == ERROR_NONE){
				if (bDBSave){
					DBSendAddMaterializedItem(0, DBDNWorldDef::AddMaterializedItem::SpecialBoxReward, m_SelectSpecialBoxItem.nEventRewardID, CashData.CashItem, 0, 0, (INT64)0, DBDNWorldDef::ItemLocation::CashInventory, DBDNWorldDef::PayMethodCode::Coin, false, 0);
				}
			}
		}
		break;
	}
}

void CDNUserItem::DBQueryCashFailItemApply(TAGetListGiveFailItem *pA)
{
	if (pA->nCount == 0) return;

	DBPacket::TModCashFailItemEx FailItemList[CASHINVENTORYMAX];
	memset(&FailItemList, 0, sizeof(DBPacket::TModCashFailItemEx));

	int nCount = 0;
	for (int i = 0; i < pA->nCount; i++){
#if defined(PRE_ADD_VIP)
		if (g_pDataManager->GetItemMainType(pA->CashFailItem[i].nItemID) == ITEMTYPE_VIP){
			m_pSession->GetDBConnection()->QueryIncreaseVIPPoint(m_pSession, g_pDataManager->GetCashCommodityVIPPoint(pA->CashFailItem[i].nItemSN), pA->CashFailItem[i].biPurchaseOrderID, 
				g_pDataManager->GetCashCommodityPeriod(pA->CashFailItem[i].nItemSN), g_pDataManager->GetCashCommodityPay(pA->CashFailItem[i].nItemSN));
			continue;
		}
#endif	// #if defined(PRE_ADD_VIP)

		if (MakeCashItemStruct(pA->CashFailItem[i].nItemSN, pA->CashFailItem[i].nItemID, FailItemList[nCount].CashItem, pA->CashFailItem[i].cItemOption) != ERROR_NONE) continue;

		FailItemList[nCount].nItemSN = pA->CashFailItem[i].nItemSN;
		FailItemList[nCount].biPurchaseOrderID = pA->CashFailItem[i].biPurchaseOrderID;
		FailItemList[nCount].biSenderCharacterDBID = pA->CashFailItem[i].biSenderCharacterDBID;
		FailItemList[nCount].bGift = pA->CashFailItem[i].bGift;
		FailItemList[nCount].nPaidCashAmount = pA->CashFailItem[i].nPaidCashAmount;
		nCount++;
	}

	if (nCount > 0)
		m_pSession->GetDBConnection()->SendModGiveFail(m_pSession, nCount, FailItemList);
}

int CDNUserItem::CheckCashDuplicationBuy(std::vector<TEffectItemData> &VecItemList, bool bReceiveGiftAll)
{
	if (VecItemList.empty())
		return ERROR_NONE;

	std::vector<int> VecItems;
	VecItems.clear();

	for (int i = 0; i < (int)VecItemList.size(); i++){
		if (g_pDataManager->GetCashCommodityOverlapBuy(VecItemList[i].nItemSN) == 2)
		{
			if (g_pDataManager->GetCashCommodityItemIDCount(VecItemList[i].nItemSN) == 1 && g_pDataManager->GetCashCommodityItem0(VecItemList[i].nItemSN) == VecItemList[i].nItemID)
			{
				if( IsEquipCashItemExist(VecItemList[i].nItemID) || (GetCashItemCountByItemID(VecItemList[i].nItemID) > 0)
#if defined(_VILLAGESERVER) && defined(PRE_ADD_CASH_REFUND)
					|| IsPaymentSameItemByItemID(VecItemList[i].nItemID) || IsPaymentPackageSameItemByItemID(VecItemList[i].nItemID)
#endif
					)
				{
					if (bReceiveGiftAll)
						VecItemList[i].bFail = true;
					else
						return ERROR_CASHSHOP_GIFTCOUNTOVER;
				}

				if (find(VecItems.begin(), VecItems.end(), VecItemList[i].nItemID) == VecItems.end()){
					VecItems.push_back(VecItemList[i].nItemID);
				}
				else{
					if (bReceiveGiftAll)
						VecItemList[i].bFail = true;
					else
						return ERROR_CASHSHOP_GIFTCOUNTOVER;
				}
			}
		}
	}

	return ERROR_NONE;
}

#if defined( PRE_ADD_GACHA_JAPAN ) && defined( _VILLAGESERVER )

int CDNUserItem::RunGachapon_JP( int nJobClass, int nSelectedPart, int& nResultItemID )
{	
	int nResult = ERROR_NONE;

	_ASSERT( 1 <= nJobClass && nJobClass <= CLASSKINDMAX );
	
	// 코인을 갖고 있는가.
	int iNumGachaCoin = GetCashItemCountByItemID( JP_GACHACOIN_ITEM_ID );
	if ( 0 < iNumGachaCoin &&
		CASHEQUIPMIN <= nSelectedPart && nSelectedPart <= CASHEQUIPMAX )
	{
		//// 가챠폰 테이블 열어서 드랍아이템 랜덤 돌림. 
		//// 결과로 나온 캐쉬템 인벤에 추가.
		//DNTableFileFormat* pGachaTable = GetDNTable( CDnTableDB::TGACHA_JP );

		//// TODO: 추후에 npc 와 연동되어야 함.
		//// 현재 선택된 가챠폰 번호는 테스트 중이므로 1로 함.
		//int iGachaIndex = 1;
		//vector<int> vlGachaTableIDs;
		//pGachaTable->GetItemIDListFromField( "_GachaNum", iGachaIndex, vlGachaTableIDs );
		//int iGachaTableID = vlGachaTableIDs.at( nJobClass-1 );

		//// 우선 가챠폰 번호로 테이블의 데이터 영역을 찾는다.
		//char acBuffer[ 32 ] = { 0 };
		//sprintf_s( acBuffer, "_LinkedDrop%d", nSelectedPart+1 );
		//int nDropTableID = pGachaTable->GetFieldFromLablePtr( iGachaTableID, acBuffer )->GetInteger();

		// 가챠폰 정보로 원하는 아이템 드랍 그룹 테이블 아이디를 얻어오자.
		TGachaponData_JP* pGachaData = g_pDataManager->GetGachaponData_JP( m_pSession->m_nGachaponShopID );
		if( pGachaData )
		{
			int nDropTableID = pGachaData->GachaponShopInfo[ nJobClass-1 ].nPartsLinkDropTableID[ nSelectedPart ];

			int nResultItemID = 0;
			int nResultItemCount = 0;
			int nLevel = 0;
			int nDepth = ITEMDROP_DEPTH;
#if defined( PRE_ADD_FARM_DOWNSCALE )
			CDNUserItem::CalcDropItems( m_pSession, nDropTableID, nResultItemID, nResultItemCount, nLevel, nDepth );
#else
			CalcDropItems( nDropTableID, nResultItemID, nResultItemCount, nLevel, nDepth );
#endif // #if defined( PRE_ADD_FARM_DOWNSCALE )

			// 결과로 나온 캐쉬템이 제대로 요청된 직업의 아이템인지 확인.
			const TItemData* pItemData = g_pDataManager->GetItemData( nResultItemID );
			if( pItemData )
			{
				vector<int>::const_iterator iter = find( pItemData->nNeedJobClassList.begin(), pItemData->nNeedJobClassList.end(), nJobClass );
				if( iter != pItemData->nNeedJobClassList.end() )
				{
#if defined (PRE_MOD_GACHA_SYSTEM)
					char cItemOption = GetGachaponAddStat(*pItemData);
					if (cItemOption < 0)
						return ERROR_GACHAPON_JP_INVALID_RESULT_ITEM;

					// 결과로 나온 캐쉬템을 인벤에 넣어줌. 가챠폰으로 나온건 시리얼 넘버 안씀.
					int iNumRemoveCoin = 1;
					if(!DeleteCashInventoryByItemID( JP_GACHACOIN_ITEM_ID, iNumRemoveCoin, DBDNWorldDef::UseItem::Use ))
						return ERROR_GACHAPON_JP_INVALID_RESULT_ITEM;

					CreateCashInvenItem( nResultItemID, 1, DBDNWorldDef::AddMaterializedItem::GetGachaResultCashItem_JP, cItemOption );
#else
					// 결과로 나온 캐쉬템을 인벤에 넣어줌. 가챠폰으로 나온건 시리얼 넘버 안씀.
					int iNumRemoveCoin = 1;
					if(!DeleteCashInventoryByItemID( JP_GACHACOIN_ITEM_ID, iNumRemoveCoin, DBDNWorldDef::UseItem::Use ))
						return ERROR_GACHAPON_JP_INVALID_RESULT_ITEM;

					CreateCashInvenItem( nResultItemID, 1, DBDNWorldDef::AddMaterializedItem::GetGachaResultCashItem_JP );
#endif
				}
				else
				{
					// 결과로 나온 아이템이 요청한 직업과 같지 않음. 패킷 핵일 가능성이 높음.
					nResult = ERROR_GACHAPON_JP_MISMATCH_JOBCLASS;
				}
			}
			else
				nResult = ERROR_GACHAPON_JP_INVALID_RESULT_ITEM;
		}
		else
		{
			nResult = ERROR_GACHAPON_JP_INVALID_SHOP_ID;
		}
	}
	else
	{
		// 코인이 모자라서 사용 실패.
		nResult = ERROR_GACHAPON_JP_NOT_ENOUGH_COIN;
	}

	return nResult;
}
#endif // PRE_ADD_GACHA_JAPAN

#if defined (PRE_MOD_GACHA_SYSTEM) && defined (_VILLAGESERVER)
char CDNUserItem::GetGachaponAddStat(const TItemData& itemData)
{
	if (itemData.nTypeParam[2] > 0)
	{
		CMtRandom Random;
		Random.srand(timeGetTime());

		char cOptionIndex = 0;
		TPotentialData *pPotential = g_pDataManager->GetPotentialData(itemData.nTypeParam[2]);
		if (pPotential && pPotential->nTotalProb > 0)
		{
			int nRand = Random.rand(0, pPotential->nTotalProb);
			char i = 0;
			const std::vector<TPotentialDataItem*>& potentialDataList = pPotential->pVecItemData;
			int nListSize = (int)potentialDataList.size();
			if (nListSize > 126)
			{
				g_Log.Log(LogType::_ERROR, m_pSession, L"[CDBID:%I64d] GetGachaponAddStat Failed : potential nListSize(%d) is larger than char size\r\n", m_pSession->GetCharacterDBID(), nListSize);
				return -1;
			}

			for (; i < (char)nListSize; i++)
			{
				if (nRand < potentialDataList[i]->nPotentailOffset)
				{
					cOptionIndex = i + 1;
					return cOptionIndex;
				}
			}
		}
	}

	return -1;
}
#endif

void CDNUserItem::SetMyMarketList(TMyMarketInfo *pMyMarketList, int nCount)
{
	m_VecMyMarketList.clear();
	for (int i = 0; i < nCount; i++){
		m_VecMyMarketList.push_back(pMyMarketList[i]);
	}
}

bool CDNUserItem::IsCashMyMarketItem(int nMarketDBID)
{
	for (int i = 0; i < (int)m_VecMyMarketList.size(); i++){
		if (m_VecMyMarketList[i].nMarketDBID == nMarketDBID){
			return g_pDataManager->IsCashItem(m_VecMyMarketList[i].nItemID);
		}
	}

	return false;
}

void CDNUserItem::DelMyMarketList(int nMarketDBID)
{
	std::vector <TMyMarketInfo>::iterator iter = m_VecMyMarketList.begin();

	while( iter != m_VecMyMarketList.end() ) 
	{
		if (iter->nMarketDBID == nMarketDBID)
		{
			m_VecMyMarketList.erase( iter );
			return;
		}

		iter++;
	}	
}

int CDNUserItem::CheckCosMix(const INT64* pSerials, UINT nResultItemID, char cOption) const
{
	int i = 0, partsType = 0;
	for(; i < MAXCOSMIXSTUFF; ++i)
	{
		if (pSerials[i] == 0)
			return ERROR_ITEM_COSMIX_STUFF_COUNT;

		const TItem* pItem = GetCashInventory(pSerials[i]);
		if (pItem == NULL)
			return ERROR_ITEM_COSMIX_FAIL_STUFF;

		if (pItem->bEternity == false)
			return ERROR_ITEM_COSMIX_FAIL_STUFF;

		TItemData *pItemData = g_pDataManager->GetItemData(pItem->nItemID);
		if (pItemData == NULL || pItemData->IsEnableCostumeMix == false)
			return ERROR_ITEM_COSMIX_FAIL_STUFF;

		TPartData *pPartData = g_pDataManager->GetPartData(pItemData->nItemID);
		if (!pPartData)
			return ERROR_ITEM_COSMIX_FAIL_STUFF;

		if (i != 0 && partsType != pPartData->nParts)
			return ERROR_ITEM_COSMIX_FAIL_STUFF;

		partsType = pPartData->nParts;
	}

	int potentialID = 0;
	const CDnCostumeMixDataMgr& mgr = g_pDataManager->GetCosMixDataMgr();
	if (nResultItemID != 0)
	{
		TPartData *pPartData = g_pDataManager->GetPartData(nResultItemID);
		if (!pPartData)
			return ERROR_ITEM_COSMIX_NO_RESULT;

		const CDnCostumeMixDataMgr::MIX_RESULT_LIST* pResultItemList = mgr.GetMixResultItemList(m_pSession->GetClassID(), pPartData->nParts);
		bool bInResultList = false;
		CDnCostumeMixDataMgr::MIX_RESULT_LIST::const_iterator resIter = pResultItemList->begin();
		for(; resIter != pResultItemList->end(); ++resIter)
		{
			const CDnCostumeMixDataMgr::SResultItemInfo& info = *resIter;
			if (info.itemId == nResultItemID)
			{
				bInResultList = true;
				break;
			}
		}

		if (bInResultList == false)
			return ERROR_ITEM_COSMIX_NO_RESULT;

		potentialID = mgr.GetPotentialID(pPartData->nParts, ITEMRANK_C);
		TItemData* pItemData = g_pDataManager->GetItemData(nResultItemID);
		if (pItemData == NULL || pItemData->nTypeParam[0] != potentialID)
			return ERROR_ITEM_COSMIX_NO_RESULT;
	}
	else
	{
		potentialID = mgr.GetPotentialID(partsType, ITEMRANK_B);
	}

	if (cOption <= 0 || potentialID < 0)
		return ERROR_ITEM_COSMIX_OPTION;

	TPotentialData* pCurrentPotentialData = g_pDataManager->GetPotentialData(potentialID);
	if (cOption > (int)pCurrentPotentialData->pVecItemData.size())
		return ERROR_ITEM_COSMIX_OPTION;

	return ERROR_NONE;
}

int CDNUserItem::MixCostume(int& resultItemId, const INT64* pSerials, UINT nResultItemID, char cOption)
{
	int nResult = ERROR_NONE;

	nResult = CheckCosMix(pSerials, nResultItemID, cOption);
	if (nResult != ERROR_NONE)
	{
		resultItemId = 0;
		return nResult;
	}

	int i = 0;
	if (nResultItemID == 0)
	{
		int totalWeight = 0, partsType = 0;
		for(i = 0; i < MAXCOSMIXSTUFF; ++i)
		{
			if (pSerials[i] == 0)
				return ERROR_ITEM_COSMIX_STUFF_COUNT;

			const TItem* pItem = GetCashInventory(pSerials[i]);
			if (pItem == NULL)
				return ERROR_ITEM_COSMIX_FAIL_STUFF;

			TItemData *pItemData = g_pDataManager->GetItemData(pItem->nItemID);
			if (pItemData == NULL || pItemData->IsEnableCostumeMix == false)
				return ERROR_ITEM_COSMIX_FAIL_STUFF;

			if (i == 0)
			{
				TPartData *pPartData = g_pDataManager->GetPartData(pItem->nItemID);
				if (!pPartData)
					return ERROR_ITEM_COSMIX_FAIL_STUFF;
				partsType = pPartData->nParts;
			}

			totalWeight += pItemData->nTypeParam[1];
		}

		const CDnCostumeMixDataMgr& mgr = g_pDataManager->GetCosMixDataMgr();
		int tableID = mgr.GetMixDropTableID(m_pSession->GetClassID(), partsType, totalWeight);
		if (tableID == -1)
			return ERROR_ITEM_COSMIX_NO_RESULT;

		int nResultItemCount = 0;
		int nLevel = 0;
		int nDepth = ITEMDROP_DEPTH;
#if defined( PRE_ADD_FARM_DOWNSCALE )
		CDNUserItem::CalcDropItems( m_pSession, tableID, resultItemId, nResultItemCount, nLevel, nDepth );
#else
		CalcDropItems( tableID, resultItemId, nResultItemCount, nLevel, nDepth );
#endif // #if defined( PRE_ADD_FARM_DOWNSCALE )
	}
	else
	{
		resultItemId = nResultItemID;
	}

	if (resultItemId == 0)
		return ERROR_ITEM_COSMIX_NO_RESULT;

	for(i = 0; i < MAXCOSMIXSTUFF; ++i)
	{
		int iNumRemoveCoin = 1;
		if (!DeleteCashInventoryBySerial(pSerials[i], iNumRemoveCoin))
			return ERROR_ITEM_COSMIX_FAIL;
	}

	CreateCashInvenItem(resultItemId, 1, DBDNWorldDef::AddMaterializedItem::CostumeMix, cOption);

	return nResult;
}

int CDNUserItem::CheckCosDesignMix(const INT64* pSerials) const
{
	int nPartsType = 0;

	if (pSerials == NULL)
		return ERROR_ITEM_DESIGNMIX_FAIL;

	int i = 0;
	for(; i < eCDMST_MAX; ++i)
	{
		if (pSerials[i] == 0)
			return ERROR_ITEM_DESIGNMIX_STUFF_COUNT;

		const TItem* pItem = GetCashInventory(pSerials[i]);
		if (pItem == NULL)
			return ERROR_ITEM_DESIGNMIX_FAIL_STUFF;

		TItemData *pItemData = g_pDataManager->GetItemData(pItem->nItemID);
		if (pItemData == NULL)
			return ERROR_ITEM_DESIGNMIX_FAIL_STUFF;

		const CDnCostumeMixDataMgr& mgr = g_pDataManager->GetCosMixDataMgr();
		if (mgr.IsEnableCostumeDesignMixStuff(pItem) == false)
			return ERROR_ITEM_DESIGNMIX_FAIL_STUFF;

		TPartData *pPartData = g_pDataManager->GetPartData(pItemData->nItemID);
		if (!pPartData)
			return ERROR_ITEM_DESIGNMIX_FAIL_STUFF;

		if (i != 0 && nPartsType != pPartData->nParts)
			return ERROR_ITEM_DESIGNMIX_FAIL_STUFF;

		nPartsType = pPartData->nParts;
	}

	return ERROR_NONE;
}

int CDNUserItem::MixCostumeDesign(const INT64* pSerials)
{
#if defined(_VILLAGESERVER)
	int nResult = ERROR_NONE;

	nResult = CheckCosDesignMix(pSerials);
	if (nResult != ERROR_NONE)
		return nResult;

	const TItem* pAbilityItem = GetCashInventory(pSerials[eCDMST_ABILITY]);
	if (pAbilityItem == NULL)
		return ERROR_ITEM_DESIGNMIX_FAIL_STUFF;

#if defined (PRE_MOD_DMIX_NOTRADE)
	if (pAbilityItem->nLookItemID > 0)
		return ERROR_ITEM_DESIGNMIX_FAIL_STUFF;
#endif

	const TItem* pDesignItem = GetCashInventory(pSerials[eCDMST_DESIGN]);
	if (pDesignItem == NULL)
		return ERROR_ITEM_DESIGNMIX_FAIL_STUFF;

	int nLookItemID = (pDesignItem->nLookItemID != 0) ? pDesignItem->nLookItemID : pDesignItem->nItemID;
	m_pSession->GetDBConnection()->QueryModAdditiveItem(m_pSession, pSerials[eCDMST_ABILITY], nLookItemID, pSerials[eCDMST_DESIGN], m_pSession->GetCosDesignMixSerial());
#endif

	return ERROR_NONE;
}


void CDNUserItem::CompleteCostumeDesign(TAModAdditiveItem * pPacket)
{
#if defined(_VILLAGESERVER)
	if (pPacket->nRetCode == ERROR_NONE)
	{
		TItem* pAbilityItem = (TItem*)GetCashInventory(pPacket->biItemSerial);
		if (pAbilityItem)
			pAbilityItem->nLookItemID = pPacket->nAdditiveItemID;
		
		// 디자인 코스튭 삭제
		DeleteItemByUse(ITEMPOSITION_CASHINVEN, -1, pPacket->biAdditiveSerial, false);

		// 합성기 삭제
		INT64 biSerial = m_pSession->GetCosDesignMixSerial();
		DeleteItemByUse(ITEMPOSITION_CASHINVEN, -1, biSerial, false);
	}
	
	m_pSession->SendCosDesignMixComplete(pPacket->nRetCode, pPacket->biItemSerial, pPacket->nAdditiveItemID);
	m_pSession->ResetCostumeDesignCache();
#endif
}

#if defined (PRE_ADD_COSRANDMIX) && defined (_VILLAGESERVER)

#ifdef PRE_ADD_COSRANDMIX_ACCESSORY
eError CDNUserItem::CheckCosRandMix(INT64& currentMixFee, const INT64* pSerials) const
#else
eError CDNUserItem::CheckCosRandMix(const INT64* pSerials) const
#endif
{
	if (pSerials == NULL)
		return ERROR_ITEM_RANDOMMIX_FAIL;

	std::vector<TItem> stuffs;
#ifdef PRE_ADD_COSRANDMIX_ACCESSORY
	std::vector<int> stuffsForFee;
#endif
	int i = 0;
	for(; i < MAX_COSTUME_RANDOMMIX_STUFF; ++i)
	{
		if (pSerials[i] == 0)
			return ERROR_ITEM_RANDOMMIX_STUFF_COUNT;

		const TItem* pItem = GetCashInventory(pSerials[i]);
		if (pItem == NULL)
			return ERROR_ITEM_RANDOMMIX_FAIL_STUFF;

		TItem temp = *pItem;
		stuffs.push_back(temp);
#ifdef PRE_ADD_COSRANDMIX_ACCESSORY
		stuffsForFee.push_back(temp.nItemID);
#endif
	}

	const CDnCostumeRandomMixDataMgr& mgr = g_pDataManager->GetCosRandomMixDataMgr();
	if (mgr.IsEnableCostumeRandomMixStuff(stuffs, m_pSession->GetClassID()) == false)
		return ERROR_ITEM_RANDOMMIX_FAIL_STUFF;

#ifdef PRE_ADD_COSRANDMIX_ACCESSORY
	if (m_pSession->GetCurrentRandomMixOpenType() == CostumeMix::RandomMix::OpenByNpc)
	{
		currentMixFee = mgr.GetRandomMixFee(stuffsForFee);
		if (currentMixFee < 0 || m_pSession->GetCoin() < currentMixFee)
			return ERROR_ITEM_RANDOMMIX_FAIL_MONEY;
	}
	else
	{
		currentMixFee = 0;
	#ifdef PRE_ADD_COSRANDMIX_MIXERTYPE
		int nMixerEnableParsType = m_pSession->GetCurrentRandomMixerEnablePartsType();
		if (nMixerEnableParsType != CostumeMix::RandomMix::MixerCanMixPartsAll)
		{
			if (stuffs.empty())
				return ERROR_ITEM_RANDOMMIX_FAIL_STUFF;

			const TPartData* pPartData = g_pDataManager->GetPartData(stuffs[0].nItemID);
			if (pPartData == NULL)
				return ERROR_ITEM_RANDOMMIX_FAIL_STUFF;

			if (nMixerEnableParsType == CostumeMix::RandomMix::MixerCanMixBasicParts)
			{
				if (mgr.IsBasicCashPartsType(pPartData->nParts) == false)
					return ERROR_ITEM_RANDOMMIX_FAIL_MIXER;
			}
			else
			{
				if (nMixerEnableParsType != pPartData->nParts)
					return ERROR_ITEM_RANDOMMIX_FAIL_MIXER;
			}
		}
	#endif
	}
#else // PRE_ADD_COSRANDMIX_ACCESSORY
	#ifdef PRE_ADD_COSRANDMIX_MIXERTYPE
	int nMixerEnableParsType = m_pSession->GetCurrentRandomMixerEnablePartsType();
	if (nMixerEnableParsType != CostumeMix::RandomMix::MixerCanMixPartsAll)
	{
		if (stuffs.empty())
			return ERROR_ITEM_RANDOMMIX_FAIL_STUFF;

		const TPartData* pPartData = g_pDataManager->GetPartData(stuffs[0].nItemID);
		if (pPartData == NULL)
			return ERROR_ITEM_RANDOMMIX_FAIL_STUFF;

		if (nMixerEnableParsType == CostumeMix::RandomMix::MixerCanMixBasicParts)
		{
			if (mgr.IsBasicCashPartsType(pPartData->nParts) == false)
				return ERROR_ITEM_RANDOMMIX_FAIL_MIXER;
		}
		else
		{
			if (nMixerEnableParsType != pPartData->nParts)
				return ERROR_ITEM_RANDOMMIX_FAIL_MIXER;
		}
	}
	#endif // PRE_ADD_COSRANDMIX_MIXERTYPE
#endif // PRE_ADD_COSRANDMIX_ACCESSORY

	return ERROR_NONE;
}

eError CDNUserItem::MixRandomCostume(int& nResultItemId, const INT64* pSerials)
{
#ifdef PRE_ADD_COSRANDMIX_ACCESSORY
	//	합성 가능여부 최종 검수 및 수수료 체크
	INT64 nFee = 0;
	eError nResult = CheckCosRandMix(nFee, pSerials);
	if (nResult != ERROR_NONE || m_pSession == NULL)
		return nResult;

	CDnCostumeRandomMixDataMgr& mgr = g_pDataManager->GetCosRandomMixDataMgr();
	CostumeMix::RandomMix::eOpenType openType = m_pSession->GetCurrentRandomMixOpenType();

	// 합성기를 사용한 합성이라면 합성기 개수 차감을 위한 합성기 아이템 오브젝트 체크
	const TItem* pMixItem = NULL;
	if (openType == CostumeMix::RandomMix::OpenByItem)
	{
		pMixItem = GetCashInventory(m_pSession->GetRandomDesignMixSerial());
		if (!pMixItem)
			return ERROR_ITEM_NOTFOUND;
	}
	else if (openType == CostumeMix::RandomMix::OpenFail)
	{
		return ERROR_ITEM_RANDOMMIX_FAIL;
	}
#else
	eError nResult = CheckCosRandMix(pSerials);
	if (nResult != ERROR_NONE || m_pSession == NULL)
		return nResult;

	const TItem* pMixItem = GetCashInventory(m_pSession->GetRandomDesignMixSerial());
	if (!pMixItem)
		return ERROR_ITEM_NOTFOUND;

	CDnCostumeRandomMixDataMgr& mgr = g_pDataManager->GetCosRandomMixDataMgr();
#endif // PRE_ADD_COSRANDMIX_ACCESSORY

	eError doMixError = ERROR_NONE;
	nResultItemId = mgr.DoRandomMix(pSerials, MAX_COSTUME_RANDOMMIX_STUFF, m_pSession->GetClassID(), *this, m_pSession->GetCharacterDBID(), doMixError);

	if (nResultItemId == 0)
		return doMixError;

	// 결과 아이템 만들기
	TItemData *pItemData = g_pDataManager->GetItemData(nResultItemId);
	if (!pItemData) 
		return ERROR_ITEM_NOTFOUND;

	if (pItemData->nOverlapCount <= 0) 
		return ERROR_ITEM_FAIL;

	if (!pItemData->IsCash) 
		return ERROR_ITEM_FAIL;	// 캐쉬템이 아니면 넣을 수 없다

	TItem RandomItem;
	memset(&RandomItem, 0, sizeof(TItem));

	MakeCashItemStruct(0, nResultItemId, RandomItem, -1, 0);

#ifdef PRE_ADD_COSRANDMIX_ACCESSORY

	if (openType == CostumeMix::RandomMix::OpenByItem)
	{
		// 합성기(pMixItem) 삭제 / 합성 아이템(pSerials) 삭제 / 결과물 아이템(RandomItem) 생성
		m_pSession->GetDBConnection()->QueryModRandomItem(m_pSession, pSerials[0], pSerials[1], pMixItem->nSerial, pMixItem->wCount, RandomItem, 0);
	}
	else if (openType == CostumeMix::RandomMix::OpenByNpc)
	{
		// 수수료(nFee) 차감 / 합성 아이템(pSerials) 삭제 / 결과물 아이템(RandomItem) 생성
		m_pSession->GetDBConnection()->QueryModRandomItem(m_pSession, pSerials[0], pSerials[1], 0, 0, RandomItem, nFee);
	}

#else // PRE_ADD_COSRANDMIX_ACCESSORY

	m_pSession->GetDBConnection()->QueryModRandomItem(m_pSession, pSerials[0], pSerials[1], pMixItem->nSerial, pMixItem->wCount, RandomItem, 0);

#endif // PRE_ADD_COSRANDMIX_ACCESSORY
	
	return ERROR_NONE;
}

void CDNUserItem::CompleteRandomDesign(TAModRandomItem * pPacket)
{
#if defined(_VILLAGESERVER)
	if (pPacket->nRetCode == ERROR_NONE)
	{
		if (_PushCashInventory(pPacket->RandomItem) == ERROR_NONE)
		{
			// 재료탬 삭제
			DeleteItemByUse(ITEMPOSITION_CASHINVEN, -1, pPacket->biStuffSerialA, false);
			DeleteItemByUse(ITEMPOSITION_CASHINVEN, -1, pPacket->biStuffSerialB, false);

			// 합성기
			DeleteItemByUse(ITEMPOSITION_CASHINVEN, -1, pPacket->biMixItemSerial, false);

			m_pSession->SendRefreshCashInven(pPacket->RandomItem);

			if (pPacket->biFee > 0)
				m_pSession->DelCoin(pPacket->biFee, DBDNWorldDef::CoinChangeCode::DoNotDBSave, 0, true);
		}
	}

	const CDnCostumeRandomMixDataMgr& mgr = g_pDataManager->GetCosRandomMixDataMgr();
	if (mgr.IsWorldMessagingItem(pPacket->RandomItem.nItemID))
#if defined(PRE_FIX_68828)
		g_pMasterConnection->SendWorldSystemMsg(m_pSession->GetAccountDBID(), WORLDCHATTYPE_COSMIX, pPacket->RandomItem.nItemID, 0);		
#else
		g_pMasterConnection->SendWorldSystemMsg(m_pSession->GetAccountDBID(), WORLDCHATTYPE_CHARMITEM, pPacket->RandomItem.nItemID, 0);
#endif

	m_pSession->SendCosRandomMixComplete(pPacket->nRetCode, pPacket->RandomItem.nItemID);
	#ifdef PRE_MOD_COSRANDMIX_NPC_CLOSE
	if (m_pSession->GetCurrentRandomMixOpenType() != CostumeMix::RandomMix::OpenByNpc)
		m_pSession->ResetCostumeRandomMixCache();
	#else
	m_pSession->ResetCostumeRandomMixCache();
	#endif // PRE_MOD_COSRANDMIX_NPC_CLOSE
#endif
}
#endif // PRE_ADD_COSRANDMIX

void CDNUserItem::RequestChangeGuildNameItem(CSGuildRename *pPacket)
{
	// 길드장 체크..ITEMTYPE_GUILDRENAME
	if ( GUILDROLE_TYPE_MASTER != m_pSession->GetGuildSelfView().btGuildRole )
		m_pSession->SendGuildRenameResult(ERROR_GUILD_ONLYAVAILABLE_GUILDMASTER);				
	else
	{
		const TItem* pItem = GetCashInventory(pPacket->biItemSerial);
		if (!pItem)
		{
			g_Log.Log(LogType::_ERROR, m_pSession, L"[CDBID:%I64d] Change guild name failed. Cash item not found. (item serial:%I64d)\r\n", m_pSession->GetCharacterDBID(), pPacket->biItemSerial);
			m_pSession->SendGuildRenameResult(ERROR_GENERIC_INVALIDREQUEST);
			return;
		}

		if (g_pDataManager->GetItemMainType(pItem->nItemID) != ITEMTYPE_GUILDRENAME)
		{
			g_Log.Log(LogType::_ERROR, m_pSession, L"[CDBID:%I64d] Change guild name failed. Invalid item type. (item serial:%I64d)\r\n", m_pSession->GetCharacterDBID(), pPacket->biItemSerial);
			m_pSession->SendGuildRenameResult(ERROR_GENERIC_INVALIDREQUEST);
			return;
		}

		// 기존 길드명과 동일할 경우
		if (!wcscmp(m_pSession->GetGuildSelfView().wszGuildName, pPacket->wszGuildName))
		{
			m_pSession->SendGuildRenameResult(ERROR_GUILD_CANTRENAME_SAME);
			return;
		}

		//길드명 체크
		if (!g_CountryUnicodeSet.Check(pPacket->wszGuildName))
			m_pSession->SendGuildRenameResult(ERROR_GENERIC_INVALIDREQUEST);
		else
		{
			// 프로시저 안에서 길드히스토리를 자동으로 남겨줌 
			m_pSession->GetDBConnection()->QueryChangeGuildName(m_pSession->GetDBThreadID(), m_pSession->GetWorldSetID(), m_pSession->GetAccountDBID(), m_pSession->GetGuildSelfView().GuildUID.nDBID,
				pPacket->wszGuildName, pPacket->biItemSerial);
		}
	}	
}

void CDNUserItem::RequestChangeCharacterNameItem(CSCharacterRename *pPacket)
{
	const TItem* pItem = GetCashInventory(pPacket->biItemSerial);
	if (!pItem)
	{
		g_Log.Log(LogType::_ERROR, m_pSession, L"[CDBID:%I64d] Change character name failed. Cash item not found. (item serial:%I64d)\r\n", m_pSession->GetCharacterDBID(), pPacket->biItemSerial);
		m_pSession->SendCharacterRenameResult(ERROR_GENERIC_INVALIDREQUEST, m_pSession->GetSessionID(), NULL);
		return;
	}

	if (g_pDataManager->GetItemMainType(pItem->nItemID) != ITEMTYPE_CHARNAME)
	{
		g_Log.Log(LogType::_ERROR, m_pSession, L"[CDBID:%I64d] Change character name failed. Invalid item type. (item serial:%I64d)\r\n", m_pSession->GetCharacterDBID(), pPacket->biItemSerial);
		m_pSession->SendCharacterRenameResult(ERROR_GENERIC_INVALIDREQUEST, m_pSession->GetSessionID(), NULL);
		return;
	}

	// 파티에 속해 있을 경우 못하게 막는다.
	if (m_pSession->GetPartyID() > 0)
	{
		m_pSession->SendCharacterRenameResult(ERROR_ITEM_CANNOT_USE_INPARTY, m_pSession->GetSessionID(), NULL);
		return;
	}

	// 길이체크
	int nLength = (int)wcslen( pPacket->wszCharacterName );
	if( nLength < CHARNAMEMIN || nLength > NAMELENMAX - 1 ) {
		m_pSession->SendCharacterRenameResult(ERROR_LOGIN_PROHIBITWORD, m_pSession->GetSessionID(), NULL);
		return;
	}

	// 기존 캐릭터명명과 동일할 경우
	if (!wcscmp(m_pSession->GetCharacterName(), pPacket->wszCharacterName))
	{
		m_pSession->SendGuildRenameResult(ERROR_ITEM_CANNOT_CHANGE_SAMENAME);
		return;
	}

	// 캐릭터명 체크 / 금칙어
	DWORD dwCheckType = ALLOW_STRING_DEFAULT;
#if defined(_US)
	dwCheckType = ALLOW_STRING_CHARACTERNAME_ENG;
#endif

#if defined(PRE_ADD_MULTILANGUAGE)
	if (!g_CountryUnicodeSet.Check(pPacket->wszCharacterName, dwCheckType) || g_pDataManager->CheckProhibitWord(m_pSession->m_eSelectedLanguage, pPacket->wszCharacterName))
#else		//#if defined(PRE_ADD_MULTILANGUAGE)
	if (!g_CountryUnicodeSet.Check(pPacket->wszCharacterName, dwCheckType) || g_pDataManager->CheckProhibitWord(pPacket->wszCharacterName))
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)
	{
		m_pSession->SendCharacterRenameResult(ERROR_LOGIN_PROHIBITWORD, m_pSession->GetSessionID(), NULL);
		return;
	}
#if defined(PRE_ADD_MULTILANGUAGE)
	if( g_pDataManager->CheckProhibitWord(m_pSession->m_eSelectedLanguage, pPacket->wszCharacterName))
#else
	if( g_pDataManager->CheckProhibitWord(pPacket->wszCharacterName))
#endif // #if defined(PRE_ADD_MULTILANGUAGE)
	{
		m_pSession->SendCharacterRenameResult(ERROR_LOGIN_PROHIBITWORD, m_pSession->GetSessionID(), NULL);
		return;
	}
	
	m_pSession->GetDBConnection()->QueryChangeCharacterName(m_pSession, pPacket->wszCharacterName, pPacket->biItemSerial);
}

void CDNUserItem::RequestChangePetName(CSChangePetName *pPacket)
{
	if (pPacket->itemSerial > 0)
	{
#if defined (_GAMESERVER)
		g_Log.Log(LogType::_ERROR, m_pSession, L"[CDBID:%I64d] Change pet name failed. Invalid location. (item serial:%I64d)\r\n", m_pSession->GetCharacterDBID(), pPacket->itemSerial);
		m_pSession->SendChangePetNameResult(ERROR_GENERIC_INVALIDREQUEST, m_pSession->GetSessionID(), pPacket->petSerial, NULL);
		return;
#endif // #if defined (_GAMESERVER)

		const TVehicle* pPet = GetPetEquip();
		if (!pPet)
		{
			// 펫이 정상적으로 파기 되었거나, 비정상적으로 소환해제가 된 경우.
			m_pSession->SendChangePetNameResult(ERROR_ITEM_ALREADY_EXPIRED, m_pSession->GetSessionID(), pPacket->petSerial, NULL);
			return;
		}

		const TItem *pItem = GetCashInventory(pPacket->itemSerial);
		if (!pItem)
		{
			g_Log.Log(LogType::_ERROR, m_pSession, L"[CDBID:%I64d] Change pet name failed. Cash item not found. (item serial:%I64d)\r\n", m_pSession->GetCharacterDBID(), pPacket->itemSerial);
			m_pSession->SendChangePetNameResult(ERROR_GENERIC_INVALIDREQUEST, m_pSession->GetSessionID(), pPacket->petSerial, NULL);
			return;
		}

		if (g_pDataManager->GetItemMainType(pItem->nItemID) != ITEMTYPE_PET_RENAME)
		{
			g_Log.Log(LogType::_ERROR, m_pSession, L"[CDBID:%I64d] Change pet name failed. Invalid item type. (item serial:%I64d)\r\n", m_pSession->GetCharacterDBID(), pPacket->itemSerial);
			m_pSession->SendChangePetNameResult(ERROR_GENERIC_INVALIDREQUEST, m_pSession->GetSessionID(), pPacket->petSerial, NULL);
			return;
		}

		if (m_PetEquip.Vehicle[Pet::Slot::Body].nSerial != pPacket->petSerial)
		{
			g_Log.Log(LogType::_ERROR, m_pSession, L"[CDBID:%I64d] Change pet name failed. Invalid pet serial. (pet serial:%I64d)\r\n", m_pSession->GetCharacterDBID(), pPacket->petSerial);
			m_pSession->SendChangePetNameResult(ERROR_GENERIC_INVALIDREQUEST, m_pSession->GetSessionID(), pPacket->petSerial, NULL);
			return;
		}

		if (wcscmp(m_PetEquip.wszNickName, pPacket->name) == 0)
		{
			m_pSession->SendChangePetNameResult(ERROR_ITEM_CANNOT_CHANGE_SAME_PET_NAME, m_pSession->GetSessionID(), pPacket->petSerial, NULL);
			return;
		}
	}
	else
	{
		const TVehicle* pPet = GetVehicleInventory(pPacket->petSerial);
		if (!pPet)
		{
			// 펫이 파기 된 경우.
			m_pSession->SendChangePetNameResult(ERROR_ITEM_ALREADY_EXPIRED, m_pSession->GetSessionID(), pPacket->petSerial, NULL);
			return;
		}

		if (wcslen(pPet->wszNickName) > 0)
		{
			m_pSession->SendChangePetNameResult(ERROR_GENERIC_INVALIDREQUEST, m_pSession->GetSessionID(), pPacket->petSerial, NULL);
			return;
		}
	}

	int length = (int)wcslen(pPacket->name);
	if (length < CHARNAMEMIN || length > NAMELENMAX - 1) 
	{
#if defined( _US ) || defined(_SG) || defined(_TH)
		m_pSession->SendChangePetNameResult(ERROR_PETNAME_SHORTAGE_4, m_pSession->GetSessionID(), pPacket->petSerial, NULL);
#else		//#if defined( _US ) || defined(_SG) || defined(_TH)
		m_pSession->SendChangePetNameResult(ERROR_PETNAME_SHORTAGE_2, m_pSession->GetSessionID(), pPacket->petSerial, NULL);
#endif		//#if defined( _US ) || defined(_SG) || defined(_TH)
		return;
	}

	DWORD dwCheckType = ALLOW_STRING_DEFAULT;
#if defined(_US)
	dwCheckType = ALLOW_STRING_CHARACTERNAME_ENG;
#endif

#if defined(PRE_ADD_MULTILANGUAGE)
	if (!g_CountryUnicodeSet.Check(pPacket->name, dwCheckType) || g_pDataManager->CheckProhibitWord(m_pSession->m_eSelectedLanguage, pPacket->name))
#else		//#if defined(PRE_ADD_MULTILANGUAGE)
	if (!g_CountryUnicodeSet.Check(pPacket->name, dwCheckType) || g_pDataManager->CheckProhibitWord(pPacket->name))
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)
	{
		m_pSession->SendChangePetNameResult(ERROR_PETNAME_PROHIBIT, m_pSession->GetSessionID(), pPacket->petSerial, NULL);
		return;
	}

 	m_pSession->GetDBConnection()->QueryChangePetName(m_pSession->GetDBThreadID(), m_pSession->GetWorldSetID(), m_pSession->GetAccountDBID(), pPacket->itemSerial, pPacket->petSerial, pPacket->name);
}

int CDNUserItem::EnchantJewel( int nInvenIndex, INT64 biInvenSerial, INT64 biItemSerial )
{
#if defined(_CH)
	return ERROR_ITEM_FAIL;
#endif  // #if defined(_CH) (임시로 막음)

	if (!m_pSession->IsNoneWindowState()) return ERROR_ITEM_FAIL;	// 딴짓할때 못한다
	if (!IsValidInventorySlot(nInvenIndex, biInvenSerial)) return ERROR_ITEM_INDEX_UNMATCH;	// 인덱스가 맞지않다

	const TItem *pItem = GetInventory(nInvenIndex);
	if (!pItem) return ERROR_ITEM_NOTFOUND;
	TItemData *pItemData = g_pDataManager->GetItemData(pItem->nItemID);
	if( !pItemData ) return ERROR_ITEM_NOTFOUND;

	bool bCashItem = true;
	const TItem *pEnchantJewelItem = GetCashInventory( biItemSerial );
	if( !pEnchantJewelItem ) {
		bCashItem = false;
		pEnchantJewelItem = GetInventory((int)biItemSerial );
	}
	if( !pEnchantJewelItem ) return ERROR_ITEM_NOTFOUND;

	TItemData *pEnchantJewelItemData = g_pDataManager->GetItemData(pEnchantJewelItem->nItemID);
	TEnchantJewelData *pEnchantJewelData = g_pDataManager->GetEnchantJewelData(pEnchantJewelItem->nItemID);
	if( !pEnchantJewelItemData || !pEnchantJewelData ) return ERROR_ITEM_NOTFOUND;
	if( pEnchantJewelItemData->nType != ITEMTYPE_ENCHANT_JEWEL ) return ERROR_ITEM_NOTFOUND;
	if( pItemData->nEnchantID == 0 ) return ERROR_ITEM_ENCHANTJEWEL_INVALIDTYPE;

	if(( pItemData->nApplicableValue & pEnchantJewelData->nApplyApplicableValue ) != pItemData->nApplicableValue )
		return ERROR_ITEM_ENCHANTJEWEL_INVALIDTYPE;
	if( pEnchantJewelData->nRequireMaxItemLevel != 0 && pItemData->cLevelLimit > pEnchantJewelData->nRequireMaxItemLevel )
		return ERROR_ITEM_ENCHANTJEWEL_INVALIDTYPE;
	if( pItem->cLevel < pEnchantJewelData->nRequireEnchantLevel )
		return ERROR_ITEM_ENCHANTJEWEL_INVALIDTYPE;
	if( pItem->cLevel >= pEnchantJewelData->nEnchantLevel )
		return ERROR_ITEM_ENCHANTJEWEL_INVALIDTYPE;
	if( pEnchantJewelData->bCanApplySealedItem == false )
	{
		if( pItem->bSoulbound == false )
		{
			return ERROR_ITEM_POTENTIAL_INVALIDTYPE;
		}
	}

	if( bCashItem )
	{
		if (!DeleteCashInventoryBySerial( biItemSerial, 1, DBDNWorldDef::UseItem::Use ))
			return ERROR_ITEM_FAIL;
	}
	else 
	{
		if (!DeleteInventoryBySlot((int)biItemSerial, 1, pEnchantJewelItem->nSerial, DBDNWorldDef::UseItem::Use ))
			return ERROR_ITEM_FAIL;
	}

	// 강화 적용.
	const_cast<TItem*>(pItem)->cLevel = pEnchantJewelData->nEnchantLevel;
	m_pSession->SendRefreshInven(nInvenIndex, pItem, false);
	m_pSession->GetDBConnection()->QueryModMaterializedItem(m_pSession, *const_cast<TItem*>(pItem), ModItemAll, pEnchantJewelData->nItemID);
	return ERROR_NONE;
}

int CDNUserItem::PotentialItem( int nInvenIndex, INT64 biInvenSerial, INT64 biItemSerial )
{
#if defined(PRE_MOD_POTENTIAL_JEWEL_RENEWAL)
	//윈도우 상태가 Blind일때만 잠재력 부여 가능
	if(!m_pSession->IsWindowState(WINDOW_BLIND)) return ERROR_ITEM_FAIL;
#else
 	if (!m_pSession->IsNoneWindowState()) return ERROR_ITEM_FAIL;	// 딴짓할때 잠재력 못한다
#endif	// #if defined(PRE_MOD_POTENTIAL_JEWEL_RENEWAL)
	if (!IsValidInventorySlot(nInvenIndex, biInvenSerial)) return ERROR_ITEM_INDEX_UNMATCH;	// 인덱스가 맞지않다

	const TItem *pItem = GetInventory(nInvenIndex);
	if (!pItem) return ERROR_ITEM_NOTFOUND;
	TItemData *pItemData = g_pDataManager->GetItemData(pItem->nItemID);
	if( !pItemData ) return ERROR_ITEM_NOTFOUND;

	bool bCashItem = true;
	const TItem *pPotentialItem = GetCashInventory( biItemSerial );
	if( !pPotentialItem ) {
		bCashItem = false;
		pPotentialItem = GetInventory((int)biItemSerial );
	}
	if( !pPotentialItem ) return ERROR_ITEM_NOTFOUND;

	TItemData *pPotentialItemData = g_pDataManager->GetItemData(pPotentialItem->nItemID);
	TPotentialJewelData *pPotentialJewelData = g_pDataManager->GetPotentialJewelData(pPotentialItem->nItemID);
	if( !pPotentialItemData || !pPotentialJewelData ) return ERROR_ITEM_NOTFOUND;
	if( pPotentialItemData->nType != ITEMTYPE_POTENTIAL_JEWEL ) return ERROR_ITEM_NOTFOUND;
	if( pItemData->nTypeParam[1] == 0 ) return ERROR_ITEM_POTENTIAL_INVALIDTYPE;

	TPotentialData *pPotential = g_pDataManager->GetPotentialData( pItemData->nTypeParam[1] );
	if( !pPotential ) return ERROR_ITEM_POTENTIAL_INVALIDTYPE;

	if(( pItemData->nApplicableValue & pPotentialJewelData->nApplyApplicableValue ) != pItemData->nApplicableValue )
		return ERROR_ITEM_POTENTIAL_INVALIDTYPE;
#if defined(PRE_MOD_POTENTIAL_JEWEL_RENEWAL)
#else
	if( !pPotentialJewelData->bErasable && pItem->cPotential > 0 ) return ERROR_ITEM_POTENTIAL_INVALIDTYPE;
	if( pPotentialJewelData->bErasable && pItem->cPotential <= 0 ) return ERROR_ITEM_POTENTIAL_INVALIDTYPE;
#endif	// #if defined(PRE_MOD_POTENTIAL_JEWEL_RENEWAL)
	if( pPotentialJewelData->bCanApplySealedItem == false )
	{
		if( pItem->bSoulbound == false )
		{
			return ERROR_ITEM_POTENTIAL_INVALIDTYPE;
		}
	}

	if( bCashItem ){
		if (!DeleteCashInventoryBySerial( biItemSerial, 1, DBDNWorldDef::UseItem::Use ))
			return ERROR_ITEM_FAIL;
	}
	else {
		if (!DeleteInventoryBySlot((int)biItemSerial, 1, pPotentialItem->nSerial, DBDNWorldDef::UseItem::Use ))
			return ERROR_ITEM_FAIL;
	}
#if defined(PRE_MOD_POTENTIAL_JEWEL_RENEWAL)
	//잠재력 부여를 취소하기 위해 저장
	m_PrevPotentialItem.nItemID = pItem->nItemID;
	m_PrevPotentialItem.nSerial = pItem->nSerial;
	m_PrevPotentialItem.cPotential = pItem->cPotential;
	m_PrevPotentialItem.cPotentialMoveCount = pItem->cPotentialMoveCount;
	m_PrevPotentialItem.nPotentialItemSerial = biItemSerial;
	m_PrevPotentialItem.nPotentialItemID = pPotentialItemData->nItemID;
#endif	// #if defined(PRE_MOD_POTENTIAL_JEWEL_RENEWAL)
	// 잠제력 부칩니다.
	if( pPotentialJewelData->nPotentialNo == 0 )
		const_cast<TItem*>(pItem)->cPotential = pPotentialJewelData->nPotentialNo;
	else {
		int nTotalProb = 0;
		std::vector<TPotentialDataItem*> pVecList;
		std::vector<int> nVecOffset;
		std::vector<int> nVecIndex;
		for( DWORD i=0; i<pPotential->pVecItemData.size(); i++ ) {
			if( pPotential->pVecItemData[i]->nPotentialNo == pPotentialJewelData->nPotentialNo ) {
				pVecList.push_back( pPotential->pVecItemData[i] );
				nVecOffset.push_back( nTotalProb + pPotential->pVecItemData[i]->nProb );
				nVecIndex.push_back( i + 1 );
				nTotalProb += pPotential->pVecItemData[i]->nProb;
			}
		}
		if( nTotalProb > 0 ) {
			CMtRandom Random;
			Random.srand( timeGetTime() );
			int nRand = Random.rand() % nTotalProb;

			int nPotentialIndex = 0;
			for (int i = 0; i <(int)pVecList.size(); i++){
				if (nRand < nVecOffset[i] ){
					nPotentialIndex = nVecIndex[i];
					break;
				}
			}
			const_cast<TItem*>(pItem)->cPotential = nPotentialIndex;
		}
	}
#if defined(PRE_MOD_POTENTIAL_JEWEL_RENEWAL)
	//새로 부여된 잠재력 저장
	m_PrevPotentialItem.cNowPotential = pItem->cPotential;
#if defined (PRE_ADD_EXCHANGE_POTENTIAL)
	const_cast<TItem*>(pItem)->cPotentialMoveCount = 0;
#endif	// #if defined (PRE_ADD_EXCHANGE_POTENTIAL)
#else	// #if defined(PRE_MOD_POTENTIAL_JEWEL_RENEWAL)
#if defined (PRE_ADD_EXCHANGE_POTENTIAL)
	//잠재가능횟수 조정
	if (const_cast<TItem*>(pItem)->cPotential <= 0)
		const_cast<TItem*>(pItem)->cPotentialMoveCount = 0;
#endif		//#if defined (PRE_ADD_EXCHANGE_POTENTIAL)
#endif	// #if defined(PRE_MOD_POTENTIAL_JEWEL_RENEWAL)
	m_pSession->SendRefreshInven(nInvenIndex, pItem, false);

	m_pSession->GetDBConnection()->QueryModMaterializedItem(m_pSession, *const_cast<TItem*>(pItem), ModItem_Potential);

	return ERROR_NONE;
}
#if defined(PRE_MOD_POTENTIAL_JEWEL_RENEWAL)
int CDNUserItem::RollbackPotentialItem( int nInvenIndex, INT64 biInvenSerial, INT64 biCodeItemSerial )
{
	if(m_PrevPotentialItem.nItemID == 0 || biInvenSerial == 0)
	{
		g_Log.Log(LogType::_ERROR, m_pSession, L"[RollbackPotential] PotentialInfo Not Set(ItemID : %d, Serial : %I64d)\r\n", m_PrevPotentialItem.nItemID, biInvenSerial );
		return ERROR_ITEM_NOTFOUND;
	}
	//윈도우 상태가 Blind일때만 잠재력 부여 가능
	if(!m_pSession->IsWindowState(WINDOW_BLIND)) return ERROR_ITEM_FAIL;
	if (!IsValidInventorySlot(nInvenIndex, biInvenSerial)) return ERROR_ITEM_INDEX_UNMATCH;	// 인덱스가 맞지않다

	const TItem *pItem = GetInventory(nInvenIndex);
	if (!pItem) return ERROR_ITEM_NOTFOUND;
	//사용한 코드 아이템 정보 가져오기
	TPotentialJewelData *pPotentialJewelData = g_pDataManager->GetPotentialJewelData(m_PrevPotentialItem.nPotentialItemID);
	if( !pPotentialJewelData ) return ERROR_ITEM_NOTFOUND;

	//아이템 ID, 시리얼로 서버에 저장된 값과 클라한테 받은 값으로 체크
	if(pItem->nItemID != m_PrevPotentialItem.nItemID || biInvenSerial != m_PrevPotentialItem.nSerial || biInvenSerial != pItem->nSerial)
	{
		g_Log.Log(LogType::_ERROR, m_pSession, L"[RollbackPotential] Not Matching RollbackPotential Info(Server[ItemID:%d][Serial:%I64d] Client[ItemID:%d][Serial:%I64d])\r\n", m_PrevPotentialItem.nItemID, m_PrevPotentialItem.nSerial, pItem->nItemID, biInvenSerial );
		return ERROR_ITEM_NOTFOUND;
	}
	//클라한테 받은 잠재력코드값(인벤번호 or 캐쉬아이템시리얼)으로 체크
	if(biCodeItemSerial != m_PrevPotentialItem.nPotentialItemSerial)
	{
		g_Log.Log(LogType::_ERROR, m_pSession, L"[RollbackPotential] Not Matching PotentialCode(Server[CodeInfo:%I64d] Client[CodeInfo:%I64d])\r\n", m_PrevPotentialItem.nPotentialItemSerial, biCodeItemSerial );
		return ERROR_ITEM_NOTFOUND;
	}
	//잠재 이전 사용 후 잠재력 취소를 요청할 수 있기 때문에, 현재 잠재력을 체크함
	if(pItem->cPotential != m_PrevPotentialItem.cNowPotential) return ERROR_ITEM_FAIL;

	//수수료 차감 및 체크
	if(pPotentialJewelData->nRollbackAmount < 0)
	{
		//수수료값이 음수면 유저한테 돈을 주는것. 리소스가 잘못된 경우임
		g_Log.Log(LogType::_ERROR, m_pSession, L"[RollbackPotentialItem] PotentialRollback Amount Is Wrong(ItemID:[%d] JewelID[%d] Charge[%d])\n", m_PrevPotentialItem.nItemID, pPotentialJewelData->nItemID, pPotentialJewelData->nRollbackAmount);
		return ERROR_ITEM_FAIL;
	}
	if(!m_pSession->CheckEnoughCoin(pPotentialJewelData->nRollbackAmount)) return ERROR_ITEM_INSUFFICIENCY_MONEY;	// 돈이 충분치 않음
	m_pSession->DelCoin(pPotentialJewelData->nRollbackAmount, DBDNWorldDef::CoinChangeCode::Use, 0);

	//잠재력 롤백
	const_cast<TItem*>(pItem)->cPotential = m_PrevPotentialItem.cPotential;
	const_cast<TItem*>(pItem)->cPotentialMoveCount = m_PrevPotentialItem.cPotentialMoveCount;

	m_pSession->SendRefreshInven(nInvenIndex, pItem, false);
	m_pSession->GetDBConnection()->QueryModMaterializedItem(m_pSession, *const_cast<TItem*>(pItem), ModItem_Potential);

	return ERROR_NONE;
}
#endif	// #if defined(PRE_MOD_POTENTIAL_JEWEL_RENEWAL)
int CDNUserItem::RequestCharmItem(CSCharmItemRequest *pPacket)
{
	if (!m_pSession->IsNoneWindowStateSet(WINDOW_PROGRESS)) return ERROR_ITEM_FAIL;

	const TItem *pItem = NULL;
	switch(pPacket->cInvenType)
	{
	case ITEMPOSITION_INVEN:
		{
			pItem = GetInventory(pPacket->sInvenIndex);
			if (!pItem) return ERROR_ITEM_FAIL;
			if (pItem->nSerial != pPacket->biInvenSerial) return ERROR_ITEM_FAIL;
		}
		break;

	case ITEMPOSITION_CASHINVEN:
		{
			pItem = GetCashInventory(pPacket->biInvenSerial);
			if (!pItem) return ERROR_ITEM_FAIL;
		}
		break;

	default:
		{
			if (!pItem) return ERROR_ITEM_FAIL;
		}
		break;
	}

	TItemData *pItemData = g_pDataManager->GetItemData(pItem->nItemID);
	if (pItemData == NULL) return ERROR_ITEM_FAIL;
	if (pItem->wCount <= 0) return ERROR_ITEM_FAIL;	// 0이면 못쓴다

	int nNeedEmptySlotCount = CharmItemNeedEmptySlotCount(pItemData->nType, pItemData->nTypeParam[0], pItemData->nTypeParam[2], true);
	if (nNeedEmptySlotCount == -1)
		return ERROR_ITEM_FAIL;

	if (nNeedEmptySlotCount > 0){
		int nEmptySlotCount = FindBlankInventorySlotCount();
		if (nEmptySlotCount < nNeedEmptySlotCount){
			return ERROR_ITEM_INVENTORY_NOTENOUGH;
		}
		if (nEmptySlotCount == 0 && pItem->wCount > 1){
			return ERROR_ITEM_FAIL;
		}
	}

	// 열쇠 유무
	if (pItemData->nTypeParam[1] == 1 || pItemData->nTypeParam[1] == 2){	// 열쇠 필요할 경우
		if (!CheckCharmKey(pItem->nItemID, pPacket->nKeyItemID, pPacket->cKeyInvenIndex)){
			return ERROR_ITEM_NEEDKEY;
		}
	}

	float fTime = 0.f;
	switch (pItemData->nType)
	{
	case ITEMTYPE_CHARM:
		{
			int nNeedEmptyCoinAmount = (int)CGlobalWeightIntTable::GetInstance().GetValue(CGlobalWeightIntTable::CharmItemEmptyCoinAmount);
			INT64 biEmptyCoinAmount = COINMAX - m_pSession->GetCoin();

			if (nNeedEmptyCoinAmount > biEmptyCoinAmount)	
			{
				return ERROR_ITEM_OVERFLOW_CHARMCOIN;
			}

			fTime = CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::CharmItemOpenTime);
		}
		break;

	case ITEMTYPE_ALLGIVECHARM:
	case ITEMTYPE_CHARMRANDOM:
		{
			fTime = CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::AllGiveCharmItemOpenTime);
		}
		break;
	}

	switch(pPacket->cInvenType)
	{
	case ITEMPOSITION_INVEN:
		{
			_UpdateInventoryCoolTime(pPacket->sInvenIndex);
		}
		break;

	case ITEMPOSITION_CASHINVEN:
		{
			_UpdateCashInventoryCoolTime(pPacket->biInvenSerial);
		}
		break;
	}

	if (pItem->nCoolTime > 0) return ERROR_ITEM_FAIL;

#if defined(PRE_ADD_GETITEM_WITH_RANDOMVALUE)
	m_pSession->SendCharmItemRequest(pPacket->cInvenType, pPacket->sInvenIndex, pPacket->biInvenSerial, ERROR_NONE, &m_CalculatedRandomCharmList);
#else	// #if defined(PRE_ADD_GETITEM_WITH_RANDOMVALUE)
	m_pSession->SendCharmItemRequest(pPacket->cInvenType, pPacket->sInvenIndex, pPacket->biInvenSerial, ERROR_NONE);
#endif	// #if defined(PRE_ADD_GETITEM_WITH_RANDOMVALUE)
	m_pSession->BroadcastingEffect(EffectType_Random, EffectState_Open);

	if (fTime < 1000.0f) fTime = 1000.0f;
	SetRequestTimer(RequestType_UseRandomItem, (DWORD)fTime);

	return ERROR_NONE;
}

int CDNUserItem::CompleteCharmItem(CSCharmItemComplete *pPacket)
{
	if (!m_pSession->IsWindowState(WINDOW_PROGRESS)) return ERROR_ITEM_FAIL;

	const TItem *pItem = NULL;
	switch(pPacket->cInvenType)
	{
	case ITEMPOSITION_INVEN:
		{
			pItem = GetInventory(pPacket->sInvenIndex);
			if (!pItem) return ERROR_ITEM_FAIL;
			if (pItem->nSerial != pPacket->biInvenSerial) return ERROR_ITEM_FAIL;
		}
		break;

	case ITEMPOSITION_CASHINVEN:
		{
			pItem = GetCashInventory(pPacket->biInvenSerial);
			if (!pItem) return ERROR_ITEM_FAIL;
		}
		break;

	default:
		return ERROR_ITEM_FAIL;
	}

	if (pItem->wCount <= 0) return ERROR_ITEM_FAIL;	// 0이면 못쓴다

	TItemData *pItemData = g_pDataManager->GetItemData(pItem->nItemID);
	if (pItemData == NULL) return ERROR_ITEM_FAIL;

	if (pItemData->cLevelLimit > m_pSession->GetLevel()) return ERROR_ITEM_FAIL;	// 레벨이 낮으면 나가라

	int nNeedEmptySlotCount = CharmItemNeedEmptySlotCount(pItemData->nType, pItemData->nTypeParam[0], pItemData->nTypeParam[2], false);
	if (nNeedEmptySlotCount == -1){
		return ERROR_ITEM_FAIL;
	}

	if (nNeedEmptySlotCount > 0){
		int nEmptySlotCount = FindBlankInventorySlotCount();
		if (nEmptySlotCount < nNeedEmptySlotCount){
			return ERROR_ITEM_INVENTORY_NOTENOUGH;
		}
		if (nEmptySlotCount == 0 && pItem->wCount > 1){
			return ERROR_ITEM_FAIL;
		}
	}

	// 열쇠 유무
	if (pItemData->nTypeParam[1] == 1 || pItemData->nTypeParam[1] == 2){	// 열쇠 필요할 경우
		if (!DeleteCharmKey(pItem->nItemID, pPacket->nKeyItemID, pPacket->biKeyItemSerial, pPacket->cKeyInvenIndex)){
			return ERROR_ITEM_NEEDKEY;
		}
	}	

	switch (pItemData->nType)
	{
	case ITEMTYPE_CHARM:
		{
			int nNeedEmptyCoinAmount = (int)CGlobalWeightIntTable::GetInstance().GetValue(CGlobalWeightIntTable::CharmItemEmptyCoinAmount);
			INT64 biEmptyCoinAmount = COINMAX - m_pSession->GetCoin();

			if (nNeedEmptyCoinAmount > biEmptyCoinAmount)	
			{
				return ERROR_ITEM_OVERFLOW_CHARMCOIN;
			}
		}
		break;

	case ITEMTYPE_ALLGIVECHARM:
	case ITEMTYPE_CHARMRANDOM:
		{
		}
		break;
	}

	switch(pPacket->cInvenType)
	{
	case ITEMPOSITION_INVEN:
		{
			_UpdateInventoryCoolTime( pPacket->sInvenIndex );
			if (pItem->nCoolTime > 0) return ERROR_ITEM_FAIL;

			if (!DeleteInventoryBySlot(pPacket->sInvenIndex, 1, pPacket->biInvenSerial, DBDNWorldDef::UseItem::Use)) return ERROR_ITEM_FAIL;			
		}
		break;

	case ITEMPOSITION_CASHINVEN:
		{
			_UpdateCashInventoryCoolTime( pPacket->biInvenSerial );
			if (pItem->nCoolTime > 0) return ERROR_ITEM_FAIL;

			if (!DeleteCashInventoryBySerial(pPacket->biInvenSerial, 1)) return ERROR_ITEM_FAIL;
		}
		break;
	}

	_SetItemCoolTime( pItemData->nSkillID, pItemData->nMaxCoolTime );

	switch (pItemData->nType)
	{
	case ITEMTYPE_CHARM:
		{
			// RandomSeed 가 있는 경우 위에 DeleteInventory 에서 Seed 값이 바뀌기 때문에
			// 강제로 srand 해준다.
#if defined( _GAMESERVER )
			_srand( m_pSession->GetGameRoom(), timeGetTime() );
#elif defined( _VILLAGESERVER )
			_srand( timeGetTime() );
#endif // #if defined( _GAMESERVER )

			// 여기서 아이템 계산해서 새루 추가해준다.
			TCharmItem ResultItem = {0,};
			CalcCharmDropItems(pItemData->nTypeParam[0], ResultItem);
			if (ResultItem.nGold > 0)
			{
				if (!m_pSession->AddCoin(ResultItem.nGold, DBDNWorldDef::CoinChangeCode::CharmItem, ResultItem.nItemID))
				{
					return ERROR_ITEM_OVERFLOW_CHARMCOIN;
				}
			}
			else
			{
				if (ResultItem.nItemID > 0)
				{
					if (g_pDataManager->IsCashItem(ResultItem.nItemID))
						CreateCashInvenItem(ResultItem.nItemID, ResultItem.nCount, DBDNWorldDef::AddMaterializedItem::RandomItem, -1, ResultItem.nPeriod, 0, pItemData->nItemID);
					else
						CreateInvenItem1(ResultItem.nItemID, ResultItem.nCount, -1, -1, DBDNWorldDef::AddMaterializedItem::RandomItem, 0);
				}
			}

			if (ResultItem.bMsg)
			{
#if defined(PRE_FIX_68828)
#if defined(_VILLAGESERVER)
				g_pMasterConnection->SendWorldSystemMsg(m_pSession->GetAccountDBID(), WORLDCHATTYPE_CHARMITEM, ResultItem.nCharmID, ResultItem.nGold);
#else
				g_pMasterConnectionManager->SendWorldSystemMsg(m_pSession->GetWorldSetID(), m_pSession->GetAccountDBID(), WORLDCHATTYPE_CHARMITEM, ResultItem.nCharmID, ResultItem.nGold);
#endif //#if defined(_VILLAGESERVER)
#else
#if defined(_VILLAGESERVER)
				g_pMasterConnection->SendWorldSystemMsg(m_pSession->GetAccountDBID(), WORLDCHATTYPE_CHARMITEM, ResultItem.nItemID, ResultItem.nGold);
#else
				g_pMasterConnectionManager->SendWorldSystemMsg(m_pSession->GetWorldSetID(), m_pSession->GetAccountDBID(), WORLDCHATTYPE_CHARMITEM, ResultItem.nItemID, ResultItem.nGold);
#endif //#if defined(_VILLAGESERVER)
#endif //#if defined(PRE_FIX_68828)
			}

			if (ResultItem.nGold > 0)
				m_pSession->SendCharmItemComplete(pPacket->cInvenType, ResultItem.nItemID, ResultItem.nGold, ResultItem.nPeriod, ERROR_NONE);
			else
				m_pSession->SendCharmItemComplete(pPacket->cInvenType, ResultItem.nItemID, ResultItem.nCount, ResultItem.nPeriod, ERROR_NONE);
		}
		break;

	case ITEMTYPE_ALLGIVECHARM:
		{
			TCharmItemData *pCharmData = g_pDataManager->GetCharmItemData(pItemData->nTypeParam[0]);
			if (!pCharmData) 
				return ERROR_ITEM_FAIL;	

			for (int i = 0; i <(int)pCharmData->CharmItemList.size(); i++)
			{
				if (pCharmData->CharmItemList[i].nItemID <= 0) continue;
				if (pCharmData->CharmItemList[i].nCount <= 0) continue;
				if (!g_pDataManager->IsPermitItemJob(pCharmData->CharmItemList[i].nItemID, m_pSession->GetStatusData()->cJobArray)) continue;

				if (g_pDataManager->IsCashItem(pCharmData->CharmItemList[i].nItemID))
					CreateCashInvenItem(pCharmData->CharmItemList[i].nItemID, pCharmData->CharmItemList[i].nCount, DBDNWorldDef::AddMaterializedItem::RandomItem, -1, pCharmData->CharmItemList[i].nPeriod, 0, pItemData->nItemID);
				else
					CreateInvenItem1(pCharmData->CharmItemList[i].nItemID, pCharmData->CharmItemList[i].nCount, -1, -1, DBDNWorldDef::AddMaterializedItem::RandomItem, 0);

				if (pCharmData->CharmItemList[i].bMsg)
				{
#if defined(PRE_FIX_68828)
#if defined(_VILLAGESERVER)
					g_pMasterConnection->SendWorldSystemMsg(m_pSession->GetAccountDBID(), WORLDCHATTYPE_CHARMITEM, pCharmData->CharmItemList[i].nCharmID, pCharmData->CharmItemList[i].nGold);
#else
					g_pMasterConnectionManager->SendWorldSystemMsg(m_pSession->GetWorldSetID(), m_pSession->GetAccountDBID(), WORLDCHATTYPE_CHARMITEM, pCharmData->CharmItemList[i].nCharmID, pCharmData->CharmItemList[i].nGold);
#endif //#if defined(_VILLAGESERVER)
#else
#if defined(_VILLAGESERVER)
					g_pMasterConnection->SendWorldSystemMsg(m_pSession->GetAccountDBID(), WORLDCHATTYPE_CHARMITEM, pCharmData->CharmItemList[i].nItemID, pCharmData->CharmItemList[i].nGold);
#else
					g_pMasterConnectionManager->SendWorldSystemMsg(m_pSession->GetWorldSetID(), m_pSession->GetAccountDBID(), WORLDCHATTYPE_CHARMITEM, pCharmData->CharmItemList[i].nItemID, pCharmData->CharmItemList[i].nGold);
#endif //#if defined(_VILLAGESERVER)
#endif //#if defined(PRE_FIX_68828)
				}
			}

			m_pSession->SendCharmItemComplete(pPacket->cInvenType, -1, 0, 0, ERROR_NONE);		
		}
		break;

	case ITEMTYPE_CHARMRANDOM:
		{
			if (m_CalculatedRandomCharmList.empty()) return ERROR_ITEM_FAIL;

			for (int i = 0; i < (int)m_CalculatedRandomCharmList.size(); i++)
			{
				if (m_CalculatedRandomCharmList[i].nItemID <= 0) continue;
				if (m_CalculatedRandomCharmList[i].nCount <= 0) continue;
				if (!g_pDataManager->IsPermitItemJob(m_CalculatedRandomCharmList[i].nItemID, m_pSession->GetStatusData()->cJobArray)) continue;

				if (g_pDataManager->IsCashItem(m_CalculatedRandomCharmList[i].nItemID))
					CreateCashInvenItem(m_CalculatedRandomCharmList[i].nItemID, m_CalculatedRandomCharmList[i].nCount, DBDNWorldDef::AddMaterializedItem::RandomItem, -1, m_CalculatedRandomCharmList[i].nPeriod, 0, pItemData->nItemID);
				else
					CreateInvenItem1(m_CalculatedRandomCharmList[i].nItemID, m_CalculatedRandomCharmList[i].nCount, -1, -1, DBDNWorldDef::AddMaterializedItem::RandomItem, 0);

				if (m_CalculatedRandomCharmList[i].bMsg)
				{
#if defined(PRE_FIX_68828)
#if defined(_VILLAGESERVER)
					g_pMasterConnection->SendWorldSystemMsg(m_pSession->GetAccountDBID(), WORLDCHATTYPE_CHARMITEM, m_CalculatedRandomCharmList[i].nCharmID, m_CalculatedRandomCharmList[i].nGold);
#else
					g_pMasterConnectionManager->SendWorldSystemMsg(m_pSession->GetWorldSetID(), m_pSession->GetAccountDBID(), WORLDCHATTYPE_CHARMITEM, m_CalculatedRandomCharmList[i].nCharmID, m_CalculatedRandomCharmList[i].nGold);
#endif //#if defined(_VILLAGESERVER)
#else
#if defined(_VILLAGESERVER)
					g_pMasterConnection->SendWorldSystemMsg(m_pSession->GetAccountDBID(), WORLDCHATTYPE_CHARMITEM, m_CalculatedRandomCharmList[i].nItemID, m_CalculatedRandomCharmList[i].nGold);
#else
					g_pMasterConnectionManager->SendWorldSystemMsg(m_pSession->GetWorldSetID(), m_pSession->GetAccountDBID(), WORLDCHATTYPE_CHARMITEM, m_CalculatedRandomCharmList[i].nItemID, m_CalculatedRandomCharmList[i].nGold);
#endif //#if defined(_VILLAGESERVER)
#endif //#if defined(PRE_FIX_68828)
				}
			}
			m_CalculatedRandomCharmList.clear();

			m_pSession->SendCharmItemComplete(pPacket->cInvenType, -1, 0, 0, ERROR_NONE);		
		}
		break;
	}

	m_pSession->BroadcastingEffect(EffectType_Random, EffectState_Good);

	return ERROR_NONE;
}

bool CDNUserItem::CheckCharmKey(int nItemID, int nNeedKeyItemID, int nNeedKeyInvenIndex)
{
	if ((nItemID <= 0) ||(nNeedKeyItemID <= 0)) return false;
	TCharmItemKeyData *pCharmKey = g_pDataManager->GetCharmKeyData(nItemID);
	if (!pCharmKey) return false;
	if (pCharmKey->nKeyList.empty()) return false;

	int nCount = 0;
	for (int i = 0; i <(int)pCharmKey->nKeyList.size(); i++){
		if (pCharmKey->nKeyList[i] != nNeedKeyItemID) continue;

		if (g_pDataManager->IsCashItem(pCharmKey->nKeyList[i]))
			nCount = GetCashItemCountByItemID(pCharmKey->nKeyList[i]);
		else
			nCount = GetInventoryItemCount(pCharmKey->nKeyList[i]);

		if (nCount > 0) return true;
	}

	return false;
}

bool CDNUserItem::DeleteCharmKey(int nItemID, int nNeedKeyItemID, INT64 biNeedKeySerial, int nNeedKeyInvenIndex)
{
	if ((nItemID <= 0) ||(nNeedKeyItemID <= 0)) return false;
	TCharmItemKeyData *pCharmKey = g_pDataManager->GetCharmKeyData(nItemID);
	if (!pCharmKey) return false;
	if (pCharmKey->nKeyList.empty()) return false;

	int nCount = 0;
	for (int i = 0; i <(int)pCharmKey->nKeyList.size(); i++){
		if (pCharmKey->nKeyList[i] != nNeedKeyItemID) continue;

		if (g_pDataManager->IsCashItem(pCharmKey->nKeyList[i])){
			nCount = GetCashItemCountByItemID(pCharmKey->nKeyList[i]);
			if (nCount > 0){
				if (!DeleteCashInventoryBySerial(biNeedKeySerial, 1, true))
					return false;
				return true;
			}
		}
		else{
			nCount = GetInventoryItemCount(pCharmKey->nKeyList[i]);
			if (nCount > 0){
				if (!DeleteInventoryBySlot(nNeedKeyInvenIndex, 1, biNeedKeySerial, DBDNWorldDef::UseItem::Use))
					return false;

				return true;
			}
		}
	}

	return false;
}

void CDNUserItem::CalcCharmDropItems(int nCharmNo, TCharmItem &ResultItem)
{
	TCharmItemData *pCharmData = g_pDataManager->GetCharmItemData(nCharmNo);
	if (!pCharmData) return;

	int nSumProb = 0;
	for (int i = 0; i <(int)pCharmData->CharmItemList.size(); i++){
		pCharmData->CharmItemList[i].nAccumulationProb = 0;	// 시작할때 0으로 초기화시켜놓는다

		if (pCharmData->CharmItemList[i].nItemID <= 0) continue;
		if (pCharmData->CharmItemList[i].nCount <= 0) continue;
		if (pCharmData->CharmItemList[i].nProb <= 0) continue;

		nSumProb += pCharmData->CharmItemList[i].nProb;
		pCharmData->CharmItemList[i].nAccumulationProb = nSumProb;
	}
#if defined(_GAMESERVER)
#if defined(PRE_ADD_77490)
	unsigned int nSeed;	
	errno_t err = rand_s(&nSeed);
	if (err != 0) return ;
	nSeed = nSeed%nSumProb;
#else
	int nSeed = _rand(m_pSession->GetGameRoom())%nSumProb;
#endif //#if defined(PRE_ADD_77490)
#elif defined(_VILLAGESERVER)
	int nSeed = _rand()%nSumProb;
#endif

	memset(&ResultItem, 0, sizeof(ResultItem));

	int nPrev = 0;
	for (int i = 0; i <(int)pCharmData->CharmItemList.size(); i++){
		if ((nSeed >= nPrev) && (nSeed < pCharmData->CharmItemList[i].nAccumulationProb)){
			ResultItem.nItemID = pCharmData->CharmItemList[i].nItemID;
			ResultItem.nCount = pCharmData->CharmItemList[i].nCount;
			ResultItem.nPeriod = pCharmData->CharmItemList[i].nPeriod;
			ResultItem.nGold = pCharmData->CharmItemList[i].nGold;
			ResultItem.bMsg = pCharmData->CharmItemList[i].bMsg;
#if defined(PRE_FIX_68828)
			ResultItem.nCharmID = pCharmData->CharmItemList[i].nCharmID;
#endif
			break;
		}
		nPrev = pCharmData->CharmItemList[i].nAccumulationProb;
	}
}

int CDNUserItem::CharmItemNeedEmptySlotCount(int nItemType, int nCharmNo, int nCharmCountTableID, bool bInsertCharmList)
{
	switch (nItemType)
	{
	case ITEMTYPE_CHARM:
		{
			return (int)CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::CharmItemEmptySlotCount);
		}
		break;

#if defined( PRE_ADD_EASYGAMECASH )
	case ITEMTYPE_ALLGIVECHARM:
		{
			TCharmItemData *pCharmData = g_pDataManager->GetCharmItemData(nCharmNo);
			if (!pCharmData) return -1;

			int nCount = 0;
			for (int i = 0; i < (int)pCharmData->CharmItemList.size(); i++)
			{
				// 일반 아이템만 체크
				if (pCharmData->CharmItemList[i].nItemID <= 0) continue;
				if (pCharmData->CharmItemList[i].nCount <= 0) continue;
				if (pCharmData->CharmItemList[i].nProb <= 0) continue;	
				if(!g_pDataManager->IsPermitItemJob(pCharmData->CharmItemList[i].nItemID, m_pSession->GetStatusData()->cJobArray)) continue;

				if( !g_pDataManager->IsCashItem(pCharmData->CharmItemList[i].nItemID) )
					nCount++;		
			}

			return nCount;
		}
		break;
#endif		//#if defined( PRE_ADD_EASYGAMECASH )

#ifdef PRE_ADD_GETITEM_WITH_RANDOMVALUE
	case ITEMTYPE_CHARMRANDOM:
		{
			if (bInsertCharmList){
				m_CalculatedRandomCharmList.clear();

				TCharmItemData *pCharmData = g_pDataManager->GetCharmItemData(nCharmNo);
				if (!pCharmData) return -1;

				TCharmCountData *pCharmCount = g_pDataManager->GetCharmCountData(nCharmCountTableID);
				if (!pCharmCount) return -1;

				unsigned int randval;
				errno_t err = rand_s(&randval);
				if (err != 0) return -1;

				int nMin = pCharmCount->nMin;
				int nMax = pCharmCount->nMax;

				int nGiveCount = randval % ( nMax - nMin + 1 ) + nMin;
				if (nGiveCount > RANDOMGIVE_CHARMITEM_MAX)
					return -1;

				TCharmItem GiveItem;
				for (int i = 0; i < nGiveCount; i++)
				{
					memset(&GiveItem, 0, sizeof(TCharmItem));
					CalcCharmDropItems(nCharmNo, GiveItem);

					m_CalculatedRandomCharmList.push_back(GiveItem);
				}

				//기획서 수정분.
				return nMax;
			}
			else{
				return (int)m_CalculatedRandomCharmList.size();
			}
		}
		break;
#endif		//#ifdef PRE_ADD_GETITEM_WITH_RANDOMVALUE
	}

	return -1;
}

#if defined (PRE_ADD_CHAOSCUBE)
bool CDNUserItem::RequestChaosCube(CSChaosCubeRequest *pPacket)
{
	if(!m_pSession->IsNoneWindowStateSet(WINDOW_PROGRESS)) return false;

	// 골드 검사
	int nNeedEmptyCoinAmount = (int)CGlobalWeightIntTable::GetInstance().GetValue(CGlobalWeightIntTable::CharmItemEmptyCoinAmount);
	INT64 biEmptyCoinAmount = COINMAX - m_pSession->GetCoin();

	if (nNeedEmptyCoinAmount > biEmptyCoinAmount)	
	{
		m_pSession->SendChaosCubeRequest(pPacket->cInvenType, 0, NULL, ERROR_ITEM_OVERFLOW_CHARMCOIN);
		return false;
	}

	// 큐브 검사
	int nDropType = -1;
	int nChaosNum = 0;
	const TItem *pCubeItem = NULL;
	switch (pPacket->cCubeInvenType)
	{
		case ITEMPOSITION_INVEN:
			{
				pCubeItem = GetInventory(pPacket->sCubeInvenIndex);
				if (!pCubeItem) return false;
				if (pCubeItem->nSerial != pPacket->biCubeInvenSerial) return false;
			}
			break;

		case ITEMPOSITION_CASHINVEN:
			{
				pCubeItem = GetCashInventory(pPacket->biCubeInvenSerial);
				if (!pCubeItem) return false;
			}
			break;
		default:
			{
				if (!pCubeItem) return false;
			}
			break;

		if (pCubeItem->wCount <=0)
			return false;
	}

	TItemData *pCubeItemData = g_pDataManager->GetItemData(pCubeItem->nItemID);
	if (pCubeItemData == NULL) 
		return false;

#if defined( PRE_ADD_CHOICECUBE )
	if (pCubeItemData->nType != ITEMTYPE_CHAOSCUBE && pCubeItemData->nType != ITEMTYPE_CHOICECUBE) 
		return false;
#else
	if (pCubeItemData->nType != ITEMTYPE_CHAOSCUBE) 
		return false;
#endif

	if (pCubeItemData->nTypeParam[0] <= 0)  
		return false;
	nChaosNum = pCubeItemData->nTypeParam[0];

	if (pCubeItemData->nTypeParam[1] < 0) 
		return false;
	nDropType = pCubeItemData->nTypeParam[1];

	// 재료 검사
	if (pPacket->nCount > MAX_CHAOSCUBE_STUFF)
		return false;

	int nItemID = 0;
	int nStuffItemID = 0;
	short wTotalCount = 0;
	for (int i=0; i<pPacket->nCount; i++)
	{
		switch(pPacket->cInvenType)
		{
		case ITEMPOSITION_INVEN:
			{
				const TItem *pStuffItem = NULL;
				pStuffItem = GetInventory(pPacket->ChaosItem[i].sInvenIndex);
				if (!pStuffItem || pStuffItem->nSerial != pPacket->ChaosItem[i].biInvenSerial || pStuffItem->wCount <=0) 
					return false;
				nItemID = pStuffItem->nItemID;
			}
			break;

		case ITEMPOSITION_CASHINVEN:
			{
				const TItem *pStuffItem = NULL;
				pStuffItem = GetCashInventory(pPacket->ChaosItem[i].biInvenSerial);
				if (!pStuffItem || pStuffItem->nSerial != pPacket->ChaosItem[i].biInvenSerial || pStuffItem->wCount <=0) 
					return false;
				nItemID = pStuffItem->nItemID;
			}
			break;

		case ITEMPOSITION_VEHICLE:
			{
				const TVehicle *pVehicle = GetVehicleInventory(pPacket->ChaosItem[i].biInvenSerial);
				if (!pVehicle) return false;
				nItemID = pVehicle->Vehicle[Vehicle::Slot::Body].nItemID;
			}
			break;

		default:
				return false;
			break;
		}

		TItemData *pItemData = g_pDataManager->GetItemData(nItemID);
		if (pItemData == NULL) 
			return false;

		wTotalCount += pPacket->ChaosItem[i].wCount;
		nStuffItemID = pItemData->nItemID;

		switch(pPacket->cInvenType)
		{
		case ITEMPOSITION_INVEN:
			{
				int nNeedEmptySlotCount = (int)CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::CharmItemEmptySlotCount);

				int nEmptySlotCount = FindBlankInventorySlotCount();
				if (nEmptySlotCount < nNeedEmptySlotCount)
				{
					m_pSession->SendChaosCubeRequest(pPacket->cInvenType, 0, NULL, ERROR_ITEM_INVENTORY_NOTENOUGH);
					return false;
				}

				if (nEmptySlotCount == 0)
				{
					m_pSession->SendChaosCubeRequest(pPacket->cInvenType, 0, NULL, ERROR_ITEM_FAIL);
					return false;
				}
				_UpdateInventoryCoolTime(pPacket->ChaosItem[i].sInvenIndex);
				
			}
			break;

		case ITEMPOSITION_CASHINVEN:
			{
				_UpdateCashInventoryCoolTime(pPacket->ChaosItem[i].biInvenSerial);
			}
			break;
		}
	}

	TChaosStuffItem *pChaosStuffData = g_pDataManager->GetChaosStuffItemData(nStuffItemID);
	if (!pChaosStuffData) 
		return false;

	if (wTotalCount != pChaosStuffData->nCount || wTotalCount == 0)
	{
		m_pSession->SendChaosCubeRequest(pPacket->cInvenType, 0, NULL, ERROR_ITEM_FAIL);
		return false;
	}

#if defined( PRE_ADD_CHOICECUBE )
	if( pCubeItemData->nType == ITEMTYPE_CHOICECUBE && pPacket->nChoiceItemID > 0 )
	{
		// 제대로 선택했는지 체크
		bool bCheck = false;
		TChaosItemData *pChaosData = g_pDataManager->GetChaosItemData(pCubeItemData->nTypeParam[0]);
		if (!pChaosData) return false;
		for (int i=0; i<(int)pChaosData->ChaosItemList.size(); i++)
		{
			if ( pChaosData->ChaosItemList[i].nItemID == pPacket->nChoiceItemID )
			{				
				if (nDropType == 2 || nDropType == 3)
				{
					if( pPacket->nChoiceItemID == nStuffItemID )
						return false;
					else
						bCheck = true;

				}
				else
				{
					bCheck = true;
					break;
				}
			}
		}
		if(!bCheck)
			return false;
	}
#endif

	// 작동 시작
	m_pSession->SendChaosCubeRequest(pPacket->cInvenType, pPacket->nCount, pPacket->ChaosItem, ERROR_NONE);
	m_pSession->BroadcastingEffect(EffectType_Random, EffectState_Open);

	float fTime = CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::CharmItemOpenTime);
	if (fTime < 1000.0f) fTime = 1000.0f;
	SetRequestTimer(RequestType_UseRandomItem, (DWORD)fTime);	
	return true;
}


bool CDNUserItem::CompleteChaosCube(CSChaosCubeComplete *pPacket)
{
	if (!m_pSession->IsWindowState(WINDOW_PROGRESS)) return false;

	// 골드검사
	int nNeedEmptyCoinAmount = (int)CGlobalWeightIntTable::GetInstance().GetValue(CGlobalWeightIntTable::CharmItemEmptyCoinAmount);
	INT64 biEmptyCoinAmount = COINMAX - m_pSession->GetCoin();

	if (nNeedEmptyCoinAmount > biEmptyCoinAmount)	
	{
		m_pSession->SendChaosCubeComplete(0, -1, 0, 0, ERROR_ITEM_OVERFLOW_CHARMCOIN);
		return false;
	}

	// 큐브검사
	int nChaosNum = 0;
	int nDropType = -1;
	const TItem *pCubeItem = NULL;
	switch (pPacket->cCubeInvenType)
	{
		case ITEMPOSITION_INVEN:
			{
				pCubeItem = GetInventory(pPacket->sCubeInvenIndex);
				if (!pCubeItem) return false;
				if (pCubeItem->nSerial != pPacket->biCubeInvenSerial) return false;
			}
			break;

		case ITEMPOSITION_CASHINVEN:
			{
				pCubeItem = GetCashInventory(pPacket->biCubeInvenSerial);
				if (!pCubeItem) return false;
			}
			break;
		default:
			{
				if (!pCubeItem) return false;
			}
			break;

		if (pCubeItem->wCount <=0)
			return false;
	}

	TItemData *pCubeItemData = g_pDataManager->GetItemData(pCubeItem->nItemID);
	if (pCubeItemData == NULL) 
		return false;

#if defined( PRE_ADD_CHOICECUBE )
	if (pCubeItemData->nType != ITEMTYPE_CHAOSCUBE && pCubeItemData->nType != ITEMTYPE_CHOICECUBE) 
		return false;
#else
	if (pCubeItemData->nType != ITEMTYPE_CHAOSCUBE) 
		return false;
#endif

	if (pCubeItemData->nTypeParam[0] <= 0)  
		return false;
	nChaosNum = pCubeItemData->nTypeParam[0];

	if (pCubeItemData->nTypeParam[1] < 0) 
		return false;
	nDropType = pCubeItemData->nTypeParam[1];


	// 아이템 검사
	if (pPacket->nCount > MAX_CHAOSCUBE_STUFF)
		return false;

	int nItemID = 0;
	int nStuffItemID = 0;
	short wTotalCount = 0;
	for (int i=0; i<pPacket->nCount; i++)
	{
		switch(pPacket->cInvenType)
		{
		case ITEMPOSITION_INVEN:
			{
				const TItem *pStuffItem  = NULL;
				pStuffItem = GetInventory(pPacket->ChaosItem[i].sInvenIndex);
				if (!pStuffItem || pStuffItem->nSerial != pPacket->ChaosItem[i].biInvenSerial || pStuffItem->wCount <=0) 
					return false;
				nItemID = pStuffItem->nItemID;
			}
			break;

		case ITEMPOSITION_CASHINVEN:
			{
				const TItem *pStuffItem  = NULL;
				pStuffItem  = GetCashInventory(pPacket->ChaosItem[i].biInvenSerial);
				if (!pStuffItem || pStuffItem->nSerial != pPacket->ChaosItem[i].biInvenSerial || pStuffItem->wCount <=0) 
					return false;
				nItemID = pStuffItem->nItemID;
			}
			break;

		case ITEMPOSITION_VEHICLE:
			{
				const TVehicle *pVehicle = GetVehicleInventory(pPacket->ChaosItem[i].biInvenSerial);
				if (!pVehicle) return false;
				nItemID = pVehicle->Vehicle[Vehicle::Slot::Body].nItemID;
			}
			break;

		default:
			return false;
		}

		TItemData *pItemData = g_pDataManager->GetItemData(nItemID);
		if (pItemData == NULL) 
			return false;		

		wTotalCount += pPacket->ChaosItem[i].wCount;
		nStuffItemID = pItemData->nItemID;

		switch(pPacket->cInvenType)
		{
		case ITEMPOSITION_INVEN:
			{
				int nNeedEmptySlotCount = (int)CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::CharmItemEmptySlotCount);

				int nEmptySlotCount = FindBlankInventorySlotCount();
				if (nEmptySlotCount < nNeedEmptySlotCount)
				{
					m_pSession->SendChaosCubeRequest(pPacket->cInvenType, 0, NULL, ERROR_ITEM_INVENTORY_NOTENOUGH);
					return false;
				}

				if (nEmptySlotCount == 0)
				{
					m_pSession->SendChaosCubeRequest(pPacket->cInvenType, 0, NULL, ERROR_ITEM_FAIL);
					return false;
				}
				_UpdateInventoryCoolTime(pPacket->ChaosItem[i].sInvenIndex);
			}
			break;

		case ITEMPOSITION_CASHINVEN:
				_UpdateCashInventoryCoolTime( pPacket->ChaosItem[i].biInvenSerial );
			break;
		}

		_SetItemCoolTime( pItemData->nSkillID, pItemData->nMaxCoolTime );
	}

	TChaosStuffItem *pChaosStuffData = g_pDataManager->GetChaosStuffItemData(nStuffItemID);
	if (!pChaosStuffData) 
		return false;

	if (wTotalCount != pChaosStuffData->nCount || wTotalCount == 0)
	{
		m_pSession->SendChaosCubeRequest(pPacket->cInvenType, 0, NULL, ERROR_ITEM_FAIL);
		return false;
	}

	// 드랍 아이템 계산
	int nDropItemID = 0;
	int nDropItemCount = 0;
	int nDropPeriod = 0;
	int nDropGold = 0;
	bool bMsg = false;

#if defined( PRE_ADD_CHOICECUBE )
	if(pCubeItemData->nType == ITEMTYPE_CHAOSCUBE )
	{
		if (!CalcChaosDropItems(nChaosNum, nDropType, nStuffItemID, nDropItemID, nDropItemCount, nDropPeriod, nDropGold, bMsg))
			return false;
	}
	else if(pCubeItemData->nType == ITEMTYPE_CHOICECUBE )
	{
		nDropItemID = pPacket->nChoiceItemID;
		if (!CalcChoiceDropItems(nChaosNum, nDropType, nStuffItemID, nDropItemID, nDropItemCount, nDropPeriod, nDropGold, bMsg))
			return false;
	}
#else
	if (!CalcChaosDropItems(nChaosNum, nDropType, nStuffItemID, nDropItemID, nDropItemCount, nDropPeriod, nDropGold, bMsg))
		return false;
#endif

	// 큐브 삭제
	switch(pPacket->cCubeInvenType)
	{
		case ITEMPOSITION_INVEN:
			{
				if (!DeleteInventoryBySlot(pPacket->sCubeInvenIndex, 1, pPacket->biCubeInvenSerial, DBDNWorldDef::UseItem::Use)) 
					return false;
			}
			break;

		case ITEMPOSITION_CASHINVEN:
			{
				if (!DeleteCashInventoryBySerial(pPacket->biCubeInvenSerial, 1)) 
					return false;			
			}
			break;
	}

	// 재료 삭제
	for (int i=0; i<pPacket->nCount; i++)
	{
		switch(pPacket->cInvenType)
		{
			case ITEMPOSITION_INVEN:
				{
					if (!DeleteInventoryBySlot(pPacket->ChaosItem[i].sInvenIndex, pPacket->ChaosItem[i].wCount, pPacket->ChaosItem[i].biInvenSerial, DBDNWorldDef::UseItem::Use)) 
						return false;
				}
				break;

			case ITEMPOSITION_CASHINVEN:
				{
					if (!DeleteCashInventoryBySerial(pPacket->ChaosItem[i].biInvenSerial, pPacket->ChaosItem[i].wCount)) 
						return false;			
				}
				break;

			case ITEMPOSITION_VEHICLE:
				{
					const TVehicle *pVehicle = GetVehicleInventory(pPacket->ChaosItem[i].biInvenSerial);
					if (!pVehicle) 
						return false;

					nItemID = pVehicle->Vehicle[Vehicle::Slot::Body].nItemID;
					TItemData *pItemData = g_pDataManager->GetItemData(nItemID);
					if (pItemData == NULL) 
						return false;

					CDNDBConnection* pDBCon = m_pSession->GetDBConnection();
					if (pItemData->nType == ITEMTYPE_VEHICLE)
					{
						if( pVehicle->Vehicle[Vehicle::Slot::Saddle].nItemID > 0 )
							pDBCon->QueryDelPetEquipment( m_pSession, pVehicle->Vehicle[Vehicle::Slot::Body].nSerial, pVehicle->Vehicle[Vehicle::Slot::Saddle].nSerial);

						if( pVehicle->Vehicle[Vehicle::Slot::Hair].nItemID > 0 )
							pDBCon->QueryDelPetEquipment( m_pSession, pVehicle->Vehicle[Vehicle::Slot::Body].nSerial, pVehicle->Vehicle[Vehicle::Slot::Hair].nSerial);

						pDBCon->QueryUseItem( m_pSession, DBDNWorldDef::UseItem::Use, pVehicle->Vehicle[Vehicle::Slot::Body].nSerial, pVehicle->Vehicle[Vehicle::Slot::Body].wCount, true );
						_PopVehicleInventoryBySerial(pVehicle->Vehicle[Vehicle::Slot::Body].nSerial );
					}
					else if (pItemData->nType == ITEMTYPE_PET)
					{
						if( pVehicle->Vehicle[Pet::Slot::Accessory1].nItemID > 0 )
							pDBCon->QueryDelPetEquipment(m_pSession, pVehicle->Vehicle[Pet::Slot::Body].nSerial, pVehicle->Vehicle[Pet::Slot::Accessory1].nSerial);
							
						if( pVehicle->Vehicle[Pet::Slot::Accessory2].nItemID > 0 )
							pDBCon->QueryDelPetEquipment(m_pSession, pVehicle->Vehicle[Pet::Slot::Body].nSerial, pVehicle->Vehicle[Pet::Slot::Accessory2].nSerial);
						
						pDBCon->QueryUseItem( m_pSession, DBDNWorldDef::UseItem::Use, pVehicle->Vehicle[Pet::Slot::Body].nSerial, pVehicle->Vehicle[Pet::Slot::Body].wCount, true );
						_PopVehicleInventoryBySerial(pVehicle->Vehicle[Pet::Slot::Body].nSerial );
					}
					else
						return false;

					TVehicle Inven;
					memset(&Inven, 0, sizeof(TVehicle));
					Inven = *pVehicle;

					for (int i = 0; i < Vehicle::Slot::Max; i++)
						Inven.Vehicle[i].wCount = 0;
					
					m_pSession->SendRefreshVehicleInven(Inven, false);

					if( m_nVehicleInventoryTotalCount > 0 )
						--m_nVehicleInventoryTotalCount;
					else
						_ASSERT(0);
				}
				break;
		}
	}
	
	// 드랍아이템 제공
	if (nDropGold > 0)
	{
		if (!m_pSession->AddCoin(nDropGold, DBDNWorldDef::CoinChangeCode::ChaosItem, nDropItemID))
		{
			m_pSession->SendChaosCubeComplete(pPacket->cInvenType, -1, 0, 0, ERROR_ITEM_OVERFLOW_CHARMCOIN);
			return false;
		}
	}
	else
	{
		if (nDropItemID > 0)
		{
			if (g_pDataManager->IsCashItem(nDropItemID))
				CreateCashInvenItem(nDropItemID, nDropItemCount, DBDNWorldDef::AddMaterializedItem::RandomItem, -1, nDropPeriod, 0, nChaosNum);
			else
				CreateInvenItem1(nDropItemID, nDropItemCount, -1, -1, DBDNWorldDef::AddMaterializedItem::RandomItem, 0);
		}
	}

	if (bMsg)
	{
#if defined(_VILLAGESERVER)
#if defined( PRE_ADD_CHOICECUBE )
		if(pCubeItemData->nType == ITEMTYPE_CHAOSCUBE )
			g_pMasterConnection->SendWorldSystemMsg(m_pSession->GetAccountDBID(), WORLDCHATTYPE_CHAOSITEM, nDropItemID, nDropGold);
		else if(pCubeItemData->nType == ITEMTYPE_CHOICECUBE)
			g_pMasterConnection->SendWorldSystemMsg(m_pSession->GetAccountDBID(), WORLDCHATTYPE_CHOICEITEM, nDropItemID, nDropGold);

#else	//	#if defined( PRE_ADD_CHOICECUBE )
		g_pMasterConnection->SendWorldSystemMsg(m_pSession->GetAccountDBID(), WORLDCHATTYPE_CHAOSITEM, nDropItemID, nDropGold);
#endif	//	#if defined( PRE_ADD_CHOICECUBE )
#else
#if defined( PRE_ADD_CHOICECUBE )
		if(pCubeItemData->nType == ITEMTYPE_CHAOSCUBE )
			g_pMasterConnectionManager->SendWorldSystemMsg(m_pSession->GetWorldSetID(), m_pSession->GetAccountDBID(), WORLDCHATTYPE_CHAOSITEM, nDropItemID, nDropGold);
		else if(pCubeItemData->nType == ITEMTYPE_CHOICECUBE)
			g_pMasterConnectionManager->SendWorldSystemMsg(m_pSession->GetWorldSetID(), m_pSession->GetAccountDBID(), WORLDCHATTYPE_CHOICEITEM, nDropItemID, nDropGold);
#else	//	#if defined( PRE_ADD_CHOICECUBE )
		g_pMasterConnectionManager->SendWorldSystemMsg(m_pSession->GetWorldSetID(), m_pSession->GetAccountDBID(), WORLDCHATTYPE_CHAOSITEM, nDropItemID, nDropGold);
#endif	//	#if defined( PRE_ADD_CHOICECUBE )
#endif
	}

	if (nDropGold > 0)
		m_pSession->SendChaosCubeComplete(pPacket->cInvenType, nDropItemID, nDropGold, nDropPeriod, ERROR_NONE);
	else
		m_pSession->SendChaosCubeComplete(pPacket->cInvenType, nDropItemID, nDropItemCount, nDropPeriod, ERROR_NONE);

	m_pSession->BroadcastingEffect(EffectType_Random, EffectState_Good);

	return true;
}

bool CDNUserItem::CalcChaosDropItems(int nChaosNo, int nDropType, int nStuffItemID, int &nResultItemID, int &nResultItemCount, int &nResultItemPeriod, int &nResultGold, bool &bMsg)
{
	TChaosItemData *pChaosData = g_pDataManager->GetChaosItemData(nChaosNo);
	if (!pChaosData) return false;

	std::vector<TChaosResultItem> ChaosItemList;
	ChaosItemList.clear();

	for (int i=0; i<(int)pChaosData->ChaosItemList.size(); i++)
	{
		if (nDropType == 2 || nDropType == 3)
		{
			if (pChaosData->ChaosItemList[i].nItemID == nStuffItemID)
				continue;
		}

		ChaosItemList.push_back(pChaosData->ChaosItemList[i]);
	}

	int nSumProb = 0;
	for (int i=0; i<(int)ChaosItemList.size(); i++)
	{
		ChaosItemList[i].nAccumulationProb = 0;

		if (nDropType == 1 || nDropType == 3)
		{
			if (ChaosItemList[i].nItemID == nStuffItemID)
				continue;
		}

		if (ChaosItemList[i].nItemID <= 0) continue;
		if (ChaosItemList[i].nCount <= 0) continue;
		if (ChaosItemList[i].nProb <= 0) continue;

		nSumProb += ChaosItemList[i].nProb;
		ChaosItemList[i].nAccumulationProb = nSumProb;
	}

#if defined(_GAMESERVER)
	_srand( m_pSession->GetGameRoom(), timeGetTime() );
	int nSeed = _rand(m_pSession->GetGameRoom())%nSumProb;
#elif defined(_VILLAGESERVER)
	_srand( timeGetTime() );
	int nSeed = _rand()%nSumProb;
#endif

	int nPrev = 0;
	for (int i = 0; i <(int)ChaosItemList.size(); i++)
	{
		if ((nSeed >= nPrev) && (nSeed < ChaosItemList[i].nAccumulationProb))
		{
			nResultItemID = ChaosItemList[i].nItemID;
			nResultItemCount = ChaosItemList[i].nCount;
			nResultItemPeriod = ChaosItemList[i].nPeriod;
			nResultGold = ChaosItemList[i].nGold;
			bMsg = ChaosItemList[i].bMsg;
			break;
		}
		nPrev = ChaosItemList[i].nAccumulationProb;
	}

	return true;
}
#endif // #if defined (PRE_ADD_CHAOSCUBE)

#if defined( PRE_ADD_CHOICECUBE )
bool CDNUserItem::CalcChoiceDropItems(int nChaosNo, int nDropType, int nStuffItemID, int nResultItemID, int &nResultItemCount, int &nResultItemPeriod, int &nResultGold, bool &bMsg)
{
	TChaosItemData *pChaosData = g_pDataManager->GetChaosItemData(nChaosNo);
	if (!pChaosData) return false;	
	
	bool bCheck = false;
	for (int i=0; i<(int)pChaosData->ChaosItemList.size(); i++)
	{
		if ( pChaosData->ChaosItemList[i].nItemID == nResultItemID )
		{

			if (nDropType == 2 || nDropType == 3)
			{
				if(nResultItemID == nStuffItemID)
					return false;
				else
				{
					bCheck = true;					
				}
			}
			else
			{
				bCheck = true;
			}
		}
		if(bCheck)
		{
			nResultItemCount = pChaosData->ChaosItemList[i].nCount;
			nResultItemPeriod = pChaosData->ChaosItemList[i].nPeriod;
			nResultGold = pChaosData->ChaosItemList[i].nGold;
			bMsg = pChaosData->ChaosItemList[i].bMsg;
			break;
		}
	}
	return bCheck;
}
#endif

#if defined (PRE_ADD_BESTFRIEND)
bool CDNUserItem::RequestBestFriendItem(CSBestFriendItemRequest *pPacket)
{
	if(!m_pSession->IsNoneWindowStateSet(WINDOW_PROGRESS)) return false;

	TItemData *pItemData = g_pDataManager->GetItemData(pPacket->nMemuItemID);
	if (pItemData == NULL) return false;

	if (pItemData->cLevelLimit > m_pSession->GetLevel()) return false;
	if (pItemData->nType != ITEMTYPE_MENUBOX) return false;

	const TItem* pItem = GetCashInventory(pPacket->biGiftSerial);
	if (!pItem) return false;

	if (false == m_pSession->GetBestFriend()->IsRegistered())
	{
		m_pSession->SendBestFriendItemRequest(0, ERROR_BESTFRIEND_NOT_REGISTERED);
		return false;
	}

	if (m_pSession->GetBestFriend()->GetInfo().biItemSerial != 0)
	{
		m_pSession->SendBestFriendItemRequest(0, ERROR_BESTFRIEND_HAVE_REWARDITEM);
		return false;
	}

	_UpdateCashInventoryCoolTime( pPacket->biGiftSerial );

	m_pSession->SendBestFriendItemRequest(pPacket->biGiftSerial, ERROR_NONE);
	m_pSession->BroadcastingEffect(EffectType_Random, EffectState_Open);

	float fTime = CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::CharmItemOpenTime);
	if (fTime < 1000.0f) fTime = 1000.0f;
	SetRequestTimer(RequestType_UseRandomItem, (DWORD)fTime);

	return true;
}

bool CDNUserItem::CompleteBestFriendItem(CSBestFriendItemComplete *pPacket)
{
	if (!m_pSession->IsWindowState(WINDOW_PROGRESS)) return false;

	TItemData *pItemData = g_pDataManager->GetItemData(pPacket->nMemuItemID);
	if (pItemData == NULL) return false;

	if (pItemData->cLevelLimit > m_pSession->GetLevel()) return false;
	if (pItemData->nType != ITEMTYPE_MENUBOX) return false;

	if (false == m_pSession->GetBestFriend()->IsRegistered())
	{
		m_pSession->SendBestFriendItemComplete(0, 0, 0, ERROR_BESTFRIEND_NOT_REGISTERED);
		return false;
	}

	if (m_pSession->GetBestFriend()->GetInfo().biItemSerial != 0)
	{
		m_pSession->SendBestFriendItemComplete(0, 0, 0, ERROR_BESTFRIEND_HAVE_REWARDITEM);
		return false;
	}

	_UpdateCashInventoryCoolTime( pPacket->biGiftSerial );
	
	_SetItemCoolTime( pItemData->nSkillID, pItemData->nMaxCoolTime );

#if defined( _GAMESERVER )
	_srand( m_pSession->GetGameRoom(), timeGetTime() );
#elif defined( _VILLAGESERVER )
	_srand( timeGetTime() );
#endif // #if defined( _GAMESERVER )

	TCharmItem ResultItem = {0,};
	CalcCharmDropItems(pItemData->nTypeParam[1], ResultItem);

	if (ResultItem.nItemID > 0)
	{
		if (g_pDataManager->IsCashItem(ResultItem.nItemID))
		{
			if (!DeleteCashInventoryBySerial(pPacket->biGiftSerial, 1))
				return false;

			CreateCashInvenItem(ResultItem.nItemID, ResultItem.nCount, DBDNWorldDef::AddMaterializedItem::BestFriendItem, -1, ResultItem.nPeriod, 0, pItemData->nItemID, DBDNWorldDef::PayMethodCode::BestFriend);
		}
		else
			return false;
	}
	else
		return false;

	m_pSession->GetDBConnection()->QueryGetBestFriend(m_pSession->GetDBThreadID(), m_pSession, true);

	m_pSession->SendBestFriendItemComplete(ResultItem.nItemID, ResultItem.nCount, ResultItem.nPeriod, ERROR_NONE);

	m_pSession->BroadcastingEffect(EffectType_Random, EffectState_Good);

	return true;
}
#endif

#if defined(_VILLAGESERVER)

int CDNUserItem::InGuildWare(CDNGuildWare* pGuildWare, TAMoveItemInGuildWare* pMove)
{
	const TItem* pDestItem = pGuildWare->GetWareItem(pMove->DestItem.biSerial);
	if (!pDestItem) return ERROR_ITEM_FAIL;	// 이미 앞에서 바꿨을테니 여기까지 오면 안될듯?

	const TItem* pSrcItem = pGuildWare->GetWareItem(pMove->SrcItem.biSerial);
	if (!pSrcItem) return ERROR_ITEM_NOTFOUND; // 아이템존재하지않음

	if (pSrcItem->wCount < pMove->SrcItem.wCount) return ERROR_ITEM_OVERFLOW; // 가지고있는것보다더많은값이오면안됨

	// 다른 아이템인 경우 바꿔주지 않는다.
	if ( CDNUserItem::bIsDifferentItem( pDestItem, pSrcItem ) == true )
		return ERROR_ITEM_FAIL;

	TItemInfo SrcWareInfo = {0,};
	SrcWareInfo.cSlotIndex = pMove->SrcItem.cSlotIndex;
	SrcWareInfo.Item = *pSrcItem;
	SrcWareInfo.Item.wCount = pMove->SrcItem.wCount;
	if (pMove->SrcItem.biNewSerial > 0)
		SrcWareInfo.Item.nSerial = pMove->SrcItem.biNewSerial;
	
	pGuildWare->UpdateWareInfo(SrcWareInfo);

	TItemInfo DestWareInfo = {0,};
	DestWareInfo.cSlotIndex = pMove->DestItem.cSlotIndex;
	DestWareInfo.Item = *pDestItem;
	DestWareInfo.Item.wCount = pMove->DestItem.wCount;
	if (pMove->DestItem.biNewSerial > 0)
		DestWareInfo.Item.nSerial = pMove->DestItem.biNewSerial;

	pGuildWare->UpdateWareInfo(DestWareInfo);

	m_pSession->SendMoveGuildItem(MoveType_GuildWare, pMove->SrcItem.biSerial, pMove->DestItem.biSerial, &SrcWareInfo, &DestWareInfo, ERROR_NONE);	
	pGuildWare->BroadCastRefreshGuildItem(MoveType_GuildWare, pMove->SrcItem.biSerial, pMove->DestItem.biSerial, &SrcWareInfo, &DestWareInfo, m_pSession);

	return ERROR_NONE;
}

int CDNUserItem::FromInvenToGuildWare(CDNGuildWare* pGuildWare, TAMoveInvenToGuildWare* pMove)
{
	const TItem* pInven = GetInventory(pMove->InvenItem.cSlotIndex);
	if (!pInven) return ERROR_ITEM_NOTFOUND; // 아이템 존재하지 않음

	if (pInven->wCount < pMove->InvenItem.wCount) return ERROR_ITEM_OVERFLOW; // 가지고 있는것보다 더 많은 값이 오면 안됨
	if (pInven->nSerial != pMove->InvenItem.biSerial) return ERROR_ITEM_NOTFOUND;	// 시리얼 체크

	int nInvenOverlapCount = g_pDataManager->GetItemOverlapCount(pInven->nItemID);
	if (nInvenOverlapCount <= 0) return ERROR_ITEM_FAIL;

	TItemInfo InvenInfo = {0,}, GuildWareInfo = {0,};

	InvenInfo.cSlotIndex = pMove->InvenItem.cSlotIndex;
	InvenInfo.Item = *pInven;
	InvenInfo.Item.wCount = pMove->InvenItem.wCount;
	if (pMove->InvenItem.biNewSerial > 0)
		InvenInfo.Item.nSerial = pMove->InvenItem.biNewSerial;

	const TItem *pWare = pGuildWare->GetWareItem(pMove->GuildWareItem.biSerial);
	if (pWare){
		// 다른 아이템인 경우 바꿔주지 않는다.
		if ( CDNUserItem::bIsDifferentItem( pInven, pWare ) == true )
			return ERROR_ITEM_FAIL;

		GuildWareInfo.Item = *pWare;
	}
	else{
		GuildWareInfo.Item = *pInven;
		GuildWareInfo.Item.nSerial = pMove->GuildWareItem.biNewSerial;
	}

	GuildWareInfo.cSlotIndex = pMove->GuildWareItem.cSlotIndex;
	GuildWareInfo.Item.wCount = pMove->GuildWareItem.wCount;

	// 인벤먼저 지우고
	int nRet = _PopInventorySlotItem(pMove->InvenItem.cSlotIndex, (pInven->wCount - pMove->InvenItem.wCount));
	if (nRet != ERROR_NONE) return nRet;

	// 길드창고 지우고
	pGuildWare->UpdateWareInfo(GuildWareInfo);

	m_pSession->SendMoveGuildItem(MoveType_InvenToGuildWare, pMove->InvenItem.biSerial, pMove->GuildWareItem.biSerial, &InvenInfo, &GuildWareInfo, ERROR_NONE);
	pGuildWare->BroadCastRefreshGuildItem(MoveType_InvenToGuildWare, pMove->InvenItem.biSerial, pMove->GuildWareItem.biSerial, &InvenInfo, &GuildWareInfo, m_pSession);

	return ERROR_NONE;
}

int CDNUserItem::FromGuildWareToInven(CDNGuildWare* pGuildWare, TAMoveGuildWareToInven* pMove)
{
	const TItem* pWare = pGuildWare->GetWareItem(pMove->GuildWareItem.biSerial);
	if (!pWare) return ERROR_ITEM_NOTFOUND; // 아이템 존재하지 않음

	if (pWare->wCount < pMove->GuildWareItem.wCount) return ERROR_ITEM_OVERFLOW; // 가지고 있는것보다 더 많은 값이 오면 안됨
	if (pWare->nSerial != pMove->GuildWareItem.biSerial) return ERROR_ITEM_NOTFOUND;	// 시리얼 체크

	int nGuildWareOverlapCount = g_pDataManager->GetItemOverlapCount(pWare->nItemID);
	if (nGuildWareOverlapCount <= 0) return ERROR_ITEM_FAIL;

	TItemInfo InvenInfo = {0,}, GuildWareInfo = {0,};

	GuildWareInfo.cSlotIndex = pMove->GuildWareItem.cSlotIndex;
	GuildWareInfo.Item = *pWare;
	GuildWareInfo.Item.wCount = pMove->GuildWareItem.wCount;
	if (pMove->GuildWareItem.biNewSerial > 0)
		GuildWareInfo.Item.nSerial = pMove->GuildWareItem.biNewSerial;

	const TItem* pInven = NULL;
	if (pMove->InvenItem.nItemID > 0)
		pInven = GetInventory(pMove->InvenItem.cSlotIndex);

	if (pInven)	// 인벤아이템이 존재한다.
	{
		// 다른 아이템인 경우 바꿔주지 않는다.
		if ( CDNUserItem::bIsDifferentItem( pInven, pWare ) == true )
			return ERROR_ITEM_FAIL;

		InvenInfo.Item = *pInven;
		InvenInfo.Item.wCount = pMove->InvenItem.wCount - pInven->wCount;
	}
	else{
#if defined(PRE_PERIOD_INVENTORY)
		if (_CheckRangePeriodInventoryIndex(pMove->InvenItem.cSlotIndex) == false) return ERROR_ITEM_FAIL;
#endif	// #if defined(PRE_PERIOD_INVENTORY)
		InvenInfo.Item = *pWare;
		InvenInfo.Item.nSerial = pMove->InvenItem.biNewSerial;
		InvenInfo.Item.wCount = pMove->InvenItem.wCount;
	}
	InvenInfo.cSlotIndex = pMove->InvenItem.cSlotIndex;

	// 길드창고 지우고
	pGuildWare->UpdateWareInfo(GuildWareInfo);
	// 인벤 지우고
	_PushInventorySlotItem(InvenInfo.cSlotIndex, InvenInfo.Item);
	InvenInfo.Item.wCount = pMove->InvenItem.wCount;	// 클라에 날려줄땐 total값으로

	m_pSession->SendMoveGuildItem(MoveType_GuildWareToInven, pMove->GuildWareItem.biSerial, pMove->InvenItem.biSerial, &GuildWareInfo, &InvenInfo, ERROR_NONE, pMove->nDailyTakeItemCount);
	pGuildWare->BroadCastRefreshGuildItem(MoveType_GuildWareToInven, pMove->GuildWareItem.biSerial, pMove->InvenItem.biSerial, &GuildWareInfo, &InvenInfo, m_pSession);

	return ERROR_NONE;
}

int CDNUserItem::NothingMoveItemInGuildWare(CDNGuildWare* pGuildWare, CSMoveGuildItem* pMove)
{
	const TItemInfo* SrcInfo = pGuildWare->GetWareItemInfo(pMove->biSrcItemSerial);
	const TItemInfo* Destnfo = pGuildWare->GetWareItemInfo(pMove->biDestItemSerial);

	if (SrcInfo && Destnfo)
		m_pSession->SendMoveGuildItem(MoveType_GuildWare, pMove->biSrcItemSerial, pMove->biDestItemSerial, SrcInfo, Destnfo, ERROR_NONE);
	else
		return ERROR_ITEM_NOTFOUND;

	return ERROR_NONE;
}

int CDNUserItem::NothingMoveInvenToGuildWare(CDNGuildWare* pGuildWare, CSMoveGuildItem* pMove)
{
#if defined(PRE_PERIOD_INVENTORY)
	if (_CheckRangePeriodInventoryIndex(pMove->cSrcIndex) == false) return ERROR_ITEM_FAIL;
#endif	// #if defined(PRE_PERIOD_INVENTORY)

	const TItem* CheckSrcItem = &m_Inventory[pMove->cSrcIndex];
	const TItem* CheckDestItem = pGuildWare->GetWareItem(pMove->biDestItemSerial);

	if (!CheckSrcItem || !CheckDestItem)
		return ERROR_ITEM_NOTFOUND;

	TItemInfo SrcInfo;
	SrcInfo.cSlotIndex = pMove->cSrcIndex;
	SrcInfo.Item = m_Inventory[pMove->cSrcIndex];

	TItemInfo DestInfo;
	DestInfo.cSlotIndex = pMove->cDestIndex;
	DestInfo.Item = *CheckDestItem;

	m_pSession->SendMoveGuildItem(MoveType_InvenToGuildWare, pMove->biSrcItemSerial, pMove->biDestItemSerial, &SrcInfo, &DestInfo, ERROR_NONE);
	
	return ERROR_NONE;
}

int CDNUserItem::NothingMoveGuildWareToInven(CDNGuildWare* pGuildWare, CSMoveGuildItem* pMove, int nDailyTakeItemCount)
{
#if defined(PRE_PERIOD_INVENTORY)
	if (_CheckRangePeriodInventoryIndex(pMove->cDestIndex) == false) return ERROR_ITEM_FAIL;
#endif	// #if defined(PRE_PERIOD_INVENTORY)

	const TItem* CheckSrcItem = pGuildWare->GetWareItem(pMove->biSrcItemSerial);
	const TItem* CheckDestItem = &m_Inventory[pMove->cDestIndex];

	if (!CheckSrcItem || !CheckDestItem)
		return ERROR_ITEM_NOTFOUND;

	TItemInfo SrcInfo;
	SrcInfo.cSlotIndex = pMove->cSrcIndex;
	SrcInfo.Item = *CheckSrcItem;

	TItemInfo DestInfo;
	DestInfo.cSlotIndex = pMove->cDestIndex;
	DestInfo.Item = m_Inventory[pMove->cDestIndex];

	m_pSession->SendMoveGuildItem(MoveType_GuildWareToInven, pMove->biSrcItemSerial, pMove->biDestItemSerial, &SrcInfo, &DestInfo, ERROR_NONE, nDailyTakeItemCount);
	
	return ERROR_NONE;
}
#endif // #if defined(_VILLAGESERVER)

#if defined(_VILLAGESERVER) && defined(PRE_ADD_CASH_REFUND)
void CDNUserItem::LoadPaymentItem(TAPaymentItemList* pPaymentItemList)
{
	if( pPaymentItemList->nPaymentCount == 0)
		return;
	for (int i=0; i<pPaymentItemList->nPaymentCount; ++i)
	{
		if( pPaymentItemList->PaymentItemList[i].PaymentItemInfo.biDBID == 0)
			break;
		m_MapPaymentItem[pPaymentItemList->PaymentItemList[i].PaymentItemInfo.biDBID] = pPaymentItemList->PaymentItemList[i];
	}
}

void CDNUserItem::LoadPaymentPackageItem(TAPaymentPackageItemList* pPaymentPackageItemList)
{
	if( pPaymentPackageItemList->nPaymentPackegeCount == 0)
		return;

	for (int i=0; i<pPaymentPackageItemList->nPaymentPackegeCount; ++i)
	{
		if( pPaymentPackageItemList->PaymentPackageItemList[i].PaymentPackageItemInfo.biDBID == 0)
			break;

		std::vector<DBPacket::TItemSNIDOption> VecItemSNID;
		VecItemSNID.clear();

		for (int j = 0; j < PACKAGEITEMMAX; j++){
			if (pPaymentPackageItemList->PaymentPackageItemList[i].PaymentPackageItemInfo.ItemInfoList[j].nItemID <= 0) continue;

			DBPacket::TItemSNIDOption ItemSNID = {0,};
			ItemSNID.nItemID = pPaymentPackageItemList->PaymentPackageItemList[i].PaymentPackageItemInfo.ItemInfoList[j].nItemID;
			ItemSNID.cOption = pPaymentPackageItemList->PaymentPackageItemList[i].PaymentPackageItemInfo.ItemInfoList[j].cItemOption;
			VecItemSNID.push_back(ItemSNID);
		}

		g_pDataManager->GetCashCommodityItemSNListByPackage(pPaymentPackageItemList->PaymentPackageItemList[i].PaymentPackageItemInfo.nPackageSN, VecItemSNID);

		if (VecItemSNID.empty()) continue;
		for (int j = 0; j < (int)VecItemSNID.size(); j++){
			pPaymentPackageItemList->PaymentPackageItemList[i].PaymentPackageItemInfo.ItemInfoList[j].nItemSN = VecItemSNID[j].nItemSN;
			pPaymentPackageItemList->PaymentPackageItemList[i].PaymentPackageItemInfo.ItemInfoList[j].nItemID = VecItemSNID[j].nItemID;
			pPaymentPackageItemList->PaymentPackageItemList[i].PaymentPackageItemInfo.ItemInfoList[j].cItemOption = VecItemSNID[j].cOption;
		}

		m_MapPaymentPackageItem[pPaymentPackageItemList->PaymentPackageItemList[i].PaymentPackageItemInfo.biDBID] = pPaymentPackageItemList->PaymentPackageItemList[i];
	}
}

int CDNUserItem::MakeSendPaymentItem(SCPaymentList* pPaymentList, int nPageNum)
{	
	pPaymentList->nTotalPaymentCount = (int)m_MapPaymentItem.size();
	int nCount = 0;
	int nFirstIndex = nPageNum*CASHINVENTORYMAX;	
	
	for(TMapPaymentItem::iterator iter = m_MapPaymentItem.begin(); iter != m_MapPaymentItem.end(); iter++ )
	{
		if( nCount == nFirstIndex + CASHINVENTORYMAX)
			break;
		if( nCount >= nFirstIndex )
		{			
			memcpy(&pPaymentList->ItemList[nCount-nFirstIndex], &iter->second.PaymentItemInfo, sizeof(TPaymentItemInfo));
			++pPaymentList->cInvenCount;
		}
		++nCount;	
	}
	return 0;
}

int CDNUserItem::MakeSendPaymentPackageItem(SCPaymentPackageList* pPaymentPackageList, int nPageNum)
{
	pPaymentPackageList->nTotalPaymentCount = (int)m_MapPaymentPackageItem.size();
	int nCount = 0;
	int nFirstIndex = nPageNum*CASHINVENTORYMAX;

	for(TMapPaymentPackageItem::iterator iter = m_MapPaymentPackageItem.begin(); iter != m_MapPaymentPackageItem.end(); iter++ )
	{
		if( nCount == nFirstIndex + CASHINVENTORYMAX)
			break;
		if( nCount >= nFirstIndex )
		{			
			memcpy(&pPaymentPackageList->ItemList[nCount-nFirstIndex], &iter->second.PaymentPackageItemInfo, sizeof(TPaymentPackageItemInfo));
			++pPaymentPackageList->cInvenCount;
		}
		++nCount;	
	}
	return 0;

}

void CDNUserItem::AddPaymentItem(const TPaymentItemInfoEx& pPaymentItem )
{
	if( pPaymentItem.PaymentItemInfo.biDBID > 0 )
		m_MapPaymentItem[pPaymentItem.PaymentItemInfo.biDBID ] = pPaymentItem;	
}

void CDNUserItem::AddPaymentPackageItem(const TPaymentPackageItemInfoEx& pPaymentPackageItem )
{
	if( pPaymentPackageItem.PaymentPackageItemInfo.biDBID > 0 )
		m_MapPaymentPackageItem[pPaymentPackageItem.PaymentPackageItemInfo.biDBID] = pPaymentPackageItem;	
}

int CDNUserItem::PaymentItemMoveToCashInven(INT64 biDBID)
{
	if( m_MapPaymentItem.empty() )
		return ERROR_ITEM_NOTFOUND;

	TMapPaymentItem::iterator iter = m_MapPaymentItem.find(biDBID);
	if(iter == m_MapPaymentItem.end()) return ERROR_ITEM_NOTFOUND;


	TPaymentItemInfo* pPaymentItem = &iter->second.PaymentItemInfo;

	//pPaymentItem->tPaymentDate
	return ERROR_NONE;
}

int CDNUserItem::PaymentPackageItemMoveToCashInven(INT64 biDBID)
{
	return ERROR_NONE;
}

TPaymentItemInfoEx* CDNUserItem::GetPaymentItem(INT64 biDBID)
{
	if( m_MapPaymentItem.empty() )
		return NULL;

	TMapPaymentItem::iterator iter = m_MapPaymentItem.find(biDBID);
	if(iter == m_MapPaymentItem.end()) return NULL;

	return &iter->second;
}

TPaymentPackageItemInfoEx* CDNUserItem::GetPaymentPackageItem(INT64 biDBID)
{
	if( m_MapPaymentPackageItem.empty() )
		return NULL;

	TMapPaymentPackageItem::iterator iter = m_MapPaymentPackageItem.find(biDBID);
	if(iter == m_MapPaymentPackageItem.end()) return NULL;

	return &iter->second;
}

void CDNUserItem::DelPaymentItem(INT64 biDBID)
{
	if( m_MapPaymentItem.empty() )
		return;

	TMapPaymentItem::iterator iter = m_MapPaymentItem.find(biDBID);
	if(iter == m_MapPaymentItem.end()) return;

	m_MapPaymentItem.erase(iter);
}

void CDNUserItem::DelPaymentPackageItem(INT64 biDBID)
{
	if( m_MapPaymentPackageItem.empty() )
		return;

	TMapPaymentPackageItem::iterator iter = m_MapPaymentPackageItem.find(biDBID);
	if (iter == m_MapPaymentPackageItem.end()) return;

	m_MapPaymentPackageItem.erase(iter);
}

bool CDNUserItem::IsPaymentSameItemByItemID(int nItemID)
{
	if( m_MapPaymentItem.empty() ) return false;

	if (nItemID <= 0) return false;	

	int nCount = 0;
	TMapPaymentItem::iterator iter;
	for(iter = m_MapPaymentItem.begin(); iter != m_MapPaymentItem.end(); ++iter){
		if (iter->second.PaymentItemInfo.ItemInfo.nItemID == nItemID)
			return true;
	}
	return false;
}

bool CDNUserItem::IsPaymentPackageSameItemByItemID(int nItemID)
{
	if( m_MapPaymentPackageItem.empty() ) return false;

	if (nItemID <= 0) return false;	

	int nCount = 0;
	TMapPaymentPackageItem::iterator iter;
	for(iter = m_MapPaymentPackageItem.begin(); iter != m_MapPaymentPackageItem.end(); ++iter)
	{
		for( int i=0; i<PACKAGEITEMMAX; ++i)
		{
			if( iter->second.PaymentPackageItemInfo.ItemInfoList[i].nItemID == 0 )
				return false; // 여기 오면 더이상 없는거..
			if (iter->second.PaymentPackageItemInfo.ItemInfoList[i].nItemID == nItemID)
				return true;
		}
	}
	return false;
}

#endif // #if defined(_VILLAGESERVER) && defined(PRE_ADD_CASH_REFUND)

// 유실된 아이템 복구
void CDNUserItem::CalcMissingItem(TAMissingItemList *pList)
{
	if (pList->cCount <= 0) return;

	TItemData *pItemData = NULL;
	char cItemLocation = 0;
	BYTE cSlotIndex = -1;

	std::queue<int> qSlotList, qQuestSlotList;
	int nInvenCount = FindBlankInventorySlotCountList(qSlotList);
	int nQuestInvenCount = FindBlankQuestInventorySlotCountList(qQuestSlotList);

	for (int i = 0; i < pList->cCount; i++){
		if (pList->MissingList[i].nItemID <= 0) continue;
		pItemData = g_pDataManager->GetItemData(pList->MissingList[i].nItemID);
		if (!pItemData) continue;

		if (pItemData->IsCash){		// 캐쉬템
			switch(pItemData->nType)
			{
			case ITEMTYPE_VEHICLE:
				{
					cItemLocation = DBDNWorldDef::ItemLocation::VehicleInventory;
					cSlotIndex = 0;
				}
				break;

			default:
				{
					cItemLocation = DBDNWorldDef::ItemLocation::CashInventory;
					cSlotIndex = 0;
				}
				break;
			}
		}
		else{	// 일반템
			switch(pItemData->nType)
			{
			case ITEMTYPE_QUEST:
				{
					if (qQuestSlotList.empty()) continue;

					cItemLocation = DBDNWorldDef::ItemLocation::QuestInventory;
					cSlotIndex = qQuestSlotList.front();
					qQuestSlotList.pop();
				}
				break;

			default:
				{
					if (qSlotList.empty()) continue;

					cSlotIndex = qSlotList.front();
					cItemLocation = DBDNWorldDef::ItemLocation::Inventory;
#if defined(PRE_PERIOD_INVENTORY)
					if (_CheckRangePeriodInventoryIndex(cSlotIndex))
						cItemLocation = DBDNWorldDef::ItemLocation::PeriodInventory;
#endif	// #if defined(PRE_PERIOD_INVENTORY)
					qSlotList.pop();
				}
				break;
			}
		}

		m_pSession->GetDBConnection()->QueryRecoverMissingItem(m_pSession, cItemLocation, cSlotIndex, pList->MissingList[i]);
	}
}

void CDNUserItem::RecoverMissingItem(TARecoverMissingItem *pMissing)
{
	if (pMissing->RecoverItem.nItemID <= 0) return;

	switch(pMissing->cItemLocationCode)
	{
	case DBDNWorldDef::ItemLocation::Inventory:
#if defined(PRE_PERIOD_INVENTORY)
	case DBDNWorldDef::ItemLocation::PeriodInventory:
#endif	// #if defined(PRE_PERIOD_INVENTORY)
		CreateInvenWholeItemByIndex(pMissing->cSlotIndex, pMissing->RecoverItem);
		break;

	case DBDNWorldDef::ItemLocation::QuestInventory:
		{
			TQuestItem Item = {0,};
			Item.nItemID = pMissing->RecoverItem.nItemID;
			Item.nSerial = pMissing->RecoverItem.nSerial;
			Item.wCount = pMissing->RecoverItem.wCount;

			m_QuestInventory[pMissing->cSlotIndex] = Item;
			m_pSession->SendRefreshQuestInven(pMissing->cSlotIndex, Item);
		}		
		break;

	case DBDNWorldDef::ItemLocation::CashInventory:
	case DBDNWorldDef::ItemLocation::VehicleInventory:
		CreateCashInvenWholeItem(pMissing->RecoverItem);
		break;
	}
}

bool CDNUserItem::IsCompleteLimitlessItem()
{
	if ((m_nCashInventoryTotalCount - (int)m_MapCashInventory.size()) > 0) return false;
	if ((m_nVehicleInventoryTotalCount - (int)m_MapVehicleInventory.size()) > 0) return false;

	return true;
}

int CDNUserItem::BuyUnionMembership(TItemData* pItemData, SHORT wCount, char &cPayMethodCode)
{	
	switch(pItemData->nTypeParam[2])
	{
		case NpcReputation::UnionType::Commercial:
		{
			cPayMethodCode = DBDNWorldDef::PayMethodCode::Union_Commerical;
			break;
		}
		case NpcReputation::UnionType::Liberty:
		{
			cPayMethodCode = DBDNWorldDef::PayMethodCode::Union_Liberty;
			break;
		}
		case NpcReputation::UnionType::Royal:
		{
			cPayMethodCode = DBDNWorldDef::PayMethodCode::Union_Royal;
			break;
		}
		default:
			return -1;
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////
	if (pItemData->nType == ITEMTYPE_UNION_MEMBERSHIP)
	{
		// 멤버쉽 아이템은 무조건 한개로 판다.
		if (wCount != 1)
			return -1;
	
		TUnionMembership* pMembership = GetUnionMembership(pItemData->nTypeParam[2]);
		TUnionReputeBenefitData* pBenefitData = g_pDataManager->GetUnionReputeBenefitByItemID(pItemData->nItemID);

		if (!pMembership || !pBenefitData)
			return -1;

		char cLifeSpanRenewal = DBDNWorldDef::EffectItemLifeSpanType::New;
		int nLifeSpan = pBenefitData->nPeriod * 60 * 24; // 분단위
		int nRemoveItemID = pMembership->nItemID; // 기존 아이템정보를 저장해 놓는다.

		CTimeSet ItemTimeSet;
		if (pMembership->nItemID == pItemData->nItemID && pMembership->tExpireDate > ItemTimeSet.GetTimeT64_LC())
		{
			// 기간연장
			nRemoveItemID = 0;	// 기존아이템을 삭제없음.
			cLifeSpanRenewal = DBDNWorldDef::EffectItemLifeSpanType::Extend;

			CTimeSet ExtendTimeSet(pMembership->tExpireDate, true);
			ExtendTimeSet.AddSecond(nLifeSpan*60);
			ItemTimeSet = ExtendTimeSet;
		}
		else
		{	
			cLifeSpanRenewal = DBDNWorldDef::EffectItemLifeSpanType::New;
			ItemTimeSet.AddSecond(nLifeSpan*60);
		}
		
		SetUnionMembership( pItemData->nTypeParam[2], pItemData->nItemID, ItemTimeSet.GetTimeT64_LC() );

		m_pSession->SendBuyUnionMembershipResult( pMembership );

		m_pSession->GetDBConnection()->QueryAddEffectItems(m_pSession, pItemData->nItemID, cLifeSpanRenewal,
			DBDNWorldDef::EffectItemGetCode::Union, 0, pItemData->iNeedBuyUnionPoint, 0, nLifeSpan, MakeItemSerial(), nRemoveItemID);
		
		return 1;
	}

	// 일반아이템
	return 0;
}

int CDNUserItem::ChangeGuildMark(CSGuildMark *pPacket)
{
	if (!m_pSession->IsNoneWindowState()) return ERROR_ITEM_FAIL;	// 딴짓할때못한다

	// 길드장 체크..ITEMTYPE_GUILDMARK
	if( GUILDROLE_TYPE_MASTER != m_pSession->GetGuildSelfView().btGuildRole )
		return ERROR_GUILD_ONLYAVAILABLE_GUILDMASTER;

	TItemData *pItemData = g_pDataManager->GetItemData(pPacket->nItemID);
	if( !pItemData ) return ERROR_ITEM_NOTFOUND;

	// 마크 검증
	if (pItemData->nType != ITEMTYPE_GUILDMARK)
		return ERROR_ITEM_CANNOT_CHANGE_GUILDMARK;

	int nResult = g_pDataManager->IsValidCompleteMark(pPacket->wGuildMark, 2);

	if (-1 == nResult) return ERROR_ITEM_NOTFOUND;
	
	if (1 == nResult)
	{
		// 완성형 마크일 경우 강제로 ID를 세팅한다.
		pPacket->wGuildMarkBG = 1;
		pPacket->wGuildMarkBorder = 2;
	}

	if( pItemData->nTypeParam[0] == 1 )
	{
		// 신규 생성
		if( m_pSession->GetGuildSelfView().wGuildMark != 0 && m_pSession->GetGuildSelfView().wGuildMarkBG != 0 &&
			m_pSession->GetGuildSelfView().wGuildMarkBorder != 0 )
		{
			return ERROR_ITEM_CANNOT_CHANGE_GUILDMARK;
		}

	}
	else if( pItemData->nTypeParam[0] == 2 )
	{
		// 변경
		if( m_pSession->GetGuildSelfView().wGuildMark == 0 && m_pSession->GetGuildSelfView().wGuildMarkBG == 0 &&
			m_pSession->GetGuildSelfView().wGuildMarkBorder == 0 )
		{
			return ERROR_ITEM_CANNOT_CHANGE_GUILDMARK;
		}
	}

	bool bCheckCash = !pItemData->IsCash;
	
	if (!g_pDataManager->IsValidGuildMark(pPacket->wGuildMark, bCheckCash))
		return ERROR_ITEM_CANNOT_CHANGE_GUILDMARK;

	if (!g_pDataManager->IsValidGuildMark(pPacket->wGuildMarkBG, bCheckCash))
		return ERROR_ITEM_CANNOT_CHANGE_GUILDMARK;

	if (!g_pDataManager->IsValidGuildMark(pPacket->wGuildMarkBorder, bCheckCash))
		return ERROR_ITEM_CANNOT_CHANGE_GUILDMARK;

	// 인벤토리 검증
	const TItem *pGuildMarkItem = NULL;
	if (pItemData->IsCash)
		pGuildMarkItem = GetCashInventory( pPacket->biItemSerial );
	else
		pGuildMarkItem = GetInventory(pPacket->nInvenIndex);
	
	if (!pGuildMarkItem) return ERROR_ITEM_NOTFOUND;

	// 아이템 제거
	if (pItemData->IsCash)
	{
		if (!DeleteCashInventoryBySerial( pPacket->biItemSerial, 1, DBDNWorldDef::UseItem::Use ))
			return ERROR_ITEM_FAIL;
	}
	else
	{
		if (!DeleteInventoryBySlot( pPacket->nInvenIndex, 1, pPacket->biItemSerial, DBDNWorldDef::UseItem::Use ))
			return ERROR_ITEM_FAIL;
	}
		
	// 길드마크 적용
	m_pSession->GetDBConnection()->QueryChangeGuildMark(m_pSession->GetDBThreadID(),  m_pSession->GetAccountDBID(), m_pSession->GetCharacterDBID(), m_pSession->GetGuildSelfView().GuildUID.nDBID, 
									m_pSession->GetWorldSetID(), pPacket->wGuildMark, pPacket->wGuildMarkBG, pPacket->wGuildMarkBorder);
		
	return ERROR_NONE;
}

int CDNUserItem::GetPetBodyItemID()
{
	return m_PetEquip.Vehicle[Pet::Slot::Body].nItemID;
}

INT64 CDNUserItem::GetPetBodySerial()
{
	return m_PetEquip.Vehicle[Pet::Slot::Body].nSerial;
}

int CDNUserItem::GetPetExp()
{
	return m_PetEquip.nExp;
}

void CDNUserItem::ItemExpireByCheat(INT64 biItemSerial)
{
	m_pSession->GetDBConnection()->QueryItemExpireByCheat(m_pSession->GetDBThreadID(), m_pSession->GetWorldSetID(), m_pSession->GetAccountDBID(), m_pSession->GetCharacterDBID(), biItemSerial);
}

int CDNUserItem::ModItemExpireDate(const CSModItemExpireDate* pItem, int nMin)
{
	m_pSession->GetDBConnection()->QueryModItemExpireDate( m_pSession->GetDBThreadID(), m_pSession->GetWorldSetID(), m_pSession->GetAccountDBID(), m_pSession->GetCharacterDBID(), pItem->biExpireDateItemSerial,
		pItem->biItemSerial, nMin, m_pSession->GetMapIndex(), m_pSession->GetIpW() );

	return ERROR_NONE;
}

void CDNUserItem::UpdatePetExp(int nExp)
{
	if( AddPetExp(nExp) )
		m_pSession->GetDBConnection()->QueryModPetExp(m_pSession->GetDBThreadID(), m_pSession->GetWorldSetID(), m_pSession->GetAccountDBID(), m_PetEquip.Vehicle[Vehicle::Slot::Body].nSerial, m_PetEquip.nExp);
}

void CDNUserItem::CalcPetSatiety(DWORD CurTick)
{	
	if( m_PetEquip.Vehicle[Pet::Slot::Body].nItemID > 0 && m_PetEquip.nType & Pet::Type::ePETTYPE_SATIETY )
	{
		TVehicleData * pPetData =  g_pDataManager->GetVehicleData(m_PetEquip.Vehicle[Vehicle::Slot::Body].nItemID);
		if( pPetData )
		{
			TPetFoodData* pPetFoodData = g_pDataManager->GetPetFoodData(pPetData->nFoodID);
			if( pPetFoodData && CurTick > m_PetEquip.dwLastHungerTick + pPetFoodData->nTickTime*1000 )
			{
				SetPetSatiety(m_PetEquip.Vehicle[Pet::Slot::Body].nSerial, m_PetEquip.nCurrentSatiety-pPetFoodData->nFullTic);
				CalcPetSatietyPercent();
			}
		}
	}
	TMapVehicle::iterator iter = m_MapVehicleInventory.begin();	
	for(;iter != m_MapVehicleInventory.end();iter++)
	{
		if( iter->second.nType & Pet::Type::ePETTYPE_SATIETY && iter->second.wszNickName[0] != '\0')
		{
			TVehicleData * pPetData =  g_pDataManager->GetVehicleData(iter->second.Vehicle[Vehicle::Slot::Body].nItemID);
			if( pPetData )
			{
				TPetFoodData* pPetFoodData = g_pDataManager->GetPetFoodData(pPetData->nFoodID);
				if( pPetFoodData && CurTick > iter->second.dwLastHungerTick + pPetFoodData->nTickTime*1000 )
				{
					SetPetSatiety(iter->second.Vehicle[Pet::Slot::Body].nSerial, iter->second.nCurrentSatiety-pPetFoodData->nFullTic);					
				}
			}
		}		
	}
}

void CDNUserItem::CalcPetSatietyPercent()
{
	m_fSatietyPercent = 0.0f;

	TVehicleData * pPetData =  g_pDataManager->GetVehicleData(m_PetEquip.Vehicle[Vehicle::Slot::Body].nItemID);
	if( pPetData )
	{
		TPetFoodData* pPetFoodData = g_pDataManager->GetPetFoodData(pPetData->nFoodID);
		if( pPetFoodData )
		{
			if( pPetFoodData->nFullMaxCount != 0)
				m_fSatietyPercent = static_cast<float>( m_PetEquip.nCurrentSatiety ) / static_cast<float>( pPetFoodData->nFullMaxCount ) * 100.0f;
			else
				m_fSatietyPercent = 0;

			if( m_fSatietyPercent < 0.0f )
				m_fSatietyPercent = 0.0f;
			else if( m_fSatietyPercent > 100.0f )
				m_fSatietyPercent = 100.0f;
		}
	}

#if defined(_GAMESERVER)
	// 펫 스텟 적용
	DnActorHandle hActor = m_pSession->GetActorHandle();
	CDnPlayerActor* pPlayer = static_cast<CDnPlayerActor *>( hActor.GetPointer() );
	if( pPlayer )
		pPlayer->RefreshState( CDnActorState::RefreshEquip );
#endif // _GAMESERVER
}

bool CDNUserItem::IsSatietyPet()
{
	if( m_PetEquip.Vehicle[Pet::Slot::Body].nItemID > 0 && m_PetEquip.nType & Pet::Type::ePETTYPE_SATIETY )
		return true;
	return false;
}

float CDNUserItem::GetSatietyApplyExpRatio()
{
	float fRatio = 0.0f;
	TVehicleData * pPetData = g_pDataManager->GetVehicleData(m_PetEquip.Vehicle[Vehicle::Slot::Body].nItemID);
	if( pPetData )
	{
		TPetFoodData* pPetFoodData = g_pDataManager->GetPetFoodData(pPetData->nFoodID);
		if( pPetFoodData )
		{
			if( 0 <= m_fSatietyPercent && m_fSatietyPercent <= Pet::Satiety::SatietySelection0 )
			{
				fRatio = static_cast<float>( pPetFoodData->nUseExp[0]) / 100.0f;				
			}
			else if( Pet::Satiety::SatietySelection0 < m_fSatietyPercent && m_fSatietyPercent <= Pet::Satiety::SatietySelection1 )
			{
				fRatio = static_cast<float>( pPetFoodData->nUseExp[1]) / 100.0f;				
			}
			else if( Pet::Satiety::SatietySelection1 < m_fSatietyPercent && m_fSatietyPercent <= Pet::Satiety::SatietySelection2 )
			{
				fRatio = static_cast<float>( pPetFoodData->nUseExp[2]) / 100.0f;				
			}
			else if( Pet::Satiety::SatietySelection2 < m_fSatietyPercent && m_fSatietyPercent <= Pet::Satiety::SatietySelection3 )
			{
				fRatio = static_cast<float>( pPetFoodData->nUseExp[3]) / 100.0f;				
			}
			else if( Pet::Satiety::SatietySelection3 < m_fSatietyPercent && m_fSatietyPercent <= Pet::Satiety::SatietySelection4 )
			{
				fRatio = static_cast<float>( pPetFoodData->nUseExp[4]) / 100.0f;				
			}
		}
	}
	return fRatio;
}

float CDNUserItem::GetSatietyApplyStateRatio()
{
	float fRatio = 0.0f;
	TVehicleData * pPetData = g_pDataManager->GetVehicleData(m_PetEquip.Vehicle[Vehicle::Slot::Body].nItemID);
	if( pPetData )
	{
		TPetFoodData* pPetFoodData = g_pDataManager->GetPetFoodData(pPetData->nFoodID);
		if( pPetFoodData )
		{
			if( 0 <= m_fSatietyPercent && m_fSatietyPercent <= Pet::Satiety::SatietySelection0 )
			{
				fRatio = static_cast<float>( pPetFoodData->nUseState[0]) / 100.0f;				
			}
			else if( Pet::Satiety::SatietySelection0 < m_fSatietyPercent && m_fSatietyPercent <= Pet::Satiety::SatietySelection1 )
			{
				fRatio = static_cast<float>( pPetFoodData->nUseState[1]) / 100.0f;				
			}
			else if( Pet::Satiety::SatietySelection1 < m_fSatietyPercent && m_fSatietyPercent <= Pet::Satiety::SatietySelection2 )
			{
				fRatio = static_cast<float>( pPetFoodData->nUseState[2]) / 100.0f;				
			}
			else if( Pet::Satiety::SatietySelection2 < m_fSatietyPercent && m_fSatietyPercent <= Pet::Satiety::SatietySelection3 )
			{
				fRatio = static_cast<float>( pPetFoodData->nUseState[3]) / 100.0f;				
			}
			else if( Pet::Satiety::SatietySelection3 < m_fSatietyPercent && m_fSatietyPercent <= Pet::Satiety::SatietySelection4 )
			{
				fRatio = static_cast<float>( pPetFoodData->nUseState[4]) / 100.0f;				
			}
		}
	}
	return fRatio;
}

void CDNUserItem::SetPetSatiety(INT64 biPetSerial, int nSatiety)
{	
	if( nSatiety < 0)
		nSatiety = 0;
	// Find Pet
	TVehicle* pPet = NULL;
	if(m_PetEquip.Vehicle[Pet::Slot::Body].nItemID > 0 && m_PetEquip.Vehicle[Pet::Slot::Body].nSerial == biPetSerial)
		pPet = &m_PetEquip;
	else
	{
		TMapVehicle::iterator iter = m_MapVehicleInventory.find(biPetSerial);
		if (iter == m_MapVehicleInventory.end()) return;

		pPet = &iter->second;
	}	
	pPet->nCurrentSatiety = nSatiety;
	pPet->dwLastHungerTick = timeGetTime();
	m_pSession->SendPetCurrentSatiety(biPetSerial, nSatiety);	
}

void CDNUserItem::SendPetSatiety()
{
	if( m_PetEquip.Vehicle[Pet::Slot::Body].nItemID > 0 && m_PetEquip.nType & Pet::Type::ePETTYPE_SATIETY )
	{
		m_pSession->SendPetCurrentSatiety(m_PetEquip.Vehicle[Pet::Slot::Body].nSerial, m_PetEquip.nCurrentSatiety);
	}
}

int CDNUserItem::ReturnItemToNpc(int nScheduleID)
{
	TGlobalEventData* pEvent = g_pDataManager->GetGlobalEventData(nScheduleID);
	if (!pEvent)
		return -1;

	int nCollectCount = 0;
	int nTempCount = 0;
#if defined(PRE_PERIOD_INVENTORY)
	for (int i = 0; i < INVENTORYTOTALMAX; i++){
		if (i == GetInventoryCount()){
			if (IsEnablePeriodInventory())
				i = INVENTORYMAX;
			else
				break;
		}
#else	// #if defined(PRE_PERIOD_INVENTORY)
	for (int i = 0; i < GetInventoryCount(); i++){
#endif	// #if defined(PRE_PERIOD_INVENTORY)
		if (m_Inventory[i].nItemID == pEvent->nCollectItemID)
		{
			nTempCount = m_Inventory[i].wCount;
			if (DeleteInventoryByItemID(m_Inventory[i].nItemID, m_Inventory[i].wCount, DBDNWorldDef::UseItem::Use))
				nCollectCount += nTempCount;
		}
	}

	int nCurrentCount = g_pPeriodQuestSystem->GetWorldEventCount(m_pSession->GetWorldSetID(), nScheduleID);
	if (nCurrentCount < pEvent->nCollectTotalCount)
		m_pSession->GetDBConnection()->QueryModWorldEventQuestCounter(m_pSession, nScheduleID, nCollectCount);

	return nCollectCount;
}

#if defined(PRE_ADD_REMOVE_PREFIX)
int CDNUserItem::RemovePrefix( int nInvenIndex, INT64 biInvenSerial, INT64 biItemSerial )
{
	if( !m_pSession->IsNoneWindowState() ) return ERROR_ITEM_FAIL;	// 딴짓할때 잠재력 못한다 
	if (!IsValidInventorySlot(nInvenIndex, biInvenSerial)) return ERROR_ITEM_INDEX_UNMATCH;	// 인덱스가 맞지않다

	const TItem *pItem = GetInventory(nInvenIndex);
	if (!pItem) return ERROR_ITEM_NOTFOUND;
	TItemData *pItemData = g_pDataManager->GetItemData(pItem->nItemID);
	if( !pItemData ) return ERROR_ITEM_NOTFOUND;

	bool bCashItem = true;
	const TItem *pPotentialItem = GetCashInventory( biItemSerial );
	if( !pPotentialItem ) {
		bCashItem = false;
		pPotentialItem = GetInventory((int)biItemSerial );
	}
	if( !pPotentialItem ) return ERROR_ITEM_NOTFOUND;

	TItemData *pPotentialItemData = g_pDataManager->GetItemData(pPotentialItem->nItemID);
	TPotentialJewelData *pPotentialJewelData = g_pDataManager->GetPotentialJewelData(pPotentialItemData->nItemID);
	if( !pPotentialItemData || !pPotentialJewelData ) return ERROR_ITEM_NOTFOUND;
	if( pPotentialItemData->nType != ITEMTYPE_POTENTIAL_JEWEL ) return ERROR_ITEM_NOTFOUND;
	
	//옵션이 있어야 하고...
	if (pItem->cOption <= 0) return ERROR_ITEM_POTENTIAL_INVALIDTYPE;

	TPotentialData *pPotential = g_pDataManager->GetPotentialData( pItemData->nTypeParam[0] );
	if( !pPotential ) return ERROR_ITEM_POTENTIAL_INVALIDTYPE;

	//옵션값이 잠재 리스트 범위 내에 있어야 하고..
	if (pItem->cOption > (int)pPotential->pVecItemData.size()) return ERROR_ITEM_POTENTIAL_INVALIDTYPE;
	//잠재 설정값의 SkillUsingType이 맞아야 한다..
	TPotentialDataItem* pPotentialDataItem = pPotential->pVecItemData[pItem->cOption - 1];
	if (NULL == pPotentialDataItem || pPotentialDataItem->nSkillUsingType != Item::SkillUsingType::PrefixSkill)
		return ERROR_ITEM_POTENTIAL_INVALIDTYPE;

	if(( pItemData->nApplicableValue & pPotentialJewelData->nApplyApplicableValue ) != pItemData->nApplicableValue )
		return ERROR_ITEM_POTENTIAL_INVALIDTYPE;
	
	if( pPotentialJewelData->bCanApplySealedItem == false )
	{
		if( pItem->bSoulbound == false )
		{
			return ERROR_ITEM_POTENTIAL_INVALIDTYPE;
		}
	}

	if (pPotentialJewelData->bSuffix == false) return ERROR_ITEM_POTENTIAL_INVALIDTYPE;

	//아이템 사용...
	if( bCashItem ){
		if (!DeleteCashInventoryBySerial( biItemSerial, 1, DBDNWorldDef::UseItem::Use ))
			return ERROR_ITEM_FAIL;
	}
	else {
		if (!DeleteInventoryBySlot((int)biItemSerial, 1, pPotentialItem->nSerial, DBDNWorldDef::UseItem::Use ))
			return ERROR_ITEM_FAIL;
	}

	//옵션값은 0으로 리셋시킴.
	const_cast<TItem*>(pItem)->cOption = 0;
	
	m_pSession->SendRefreshInven(nInvenIndex, pItem, false);

	m_pSession->GetDBConnection()->QueryModMaterializedItem(m_pSession, *const_cast<TItem*>(pItem), ModItem_Option);

	return ERROR_NONE;
}
#endif // PRE_ADD_REMOVE_PREFIX

int CDNUserItem::AddPetSkill( INT64 biItemSerial, char& cSlotNum, int& nSkillID )
{
	if (!m_pSession->IsNoneWindowState() != WINDOW_NONE) return ERROR_ITEM_FAIL;	// 딴짓할때 스킬 못 끼움
	if( m_PetEquip.Vehicle[Pet::Slot::Body].nItemID <= 0) return ERROR_ITEM_FAIL; // 펫이 소환되어 있지 않음.
	if( m_PetEquip.nSkillID1 && m_PetEquip.nSkillID2 ) return ERROR_ITEM_FAIL; // 슬롯이 2개가 꽉 차있음.
	
	if( m_PetEquip.nSkillID1 == 0 )
		cSlotNum = 1;
	else if (m_PetEquip.nSkillID2 == 0)
		cSlotNum = 2;

	if( cSlotNum == 2 && !m_PetEquip.bSkillSlot ) //슬롯이 2번째꺼이면 슬롯 확장이 되어있는지 확인	
		return ERROR_ITEM_FAIL;	
	
	const TItem *pPetSkillItem = GetCashInventory( biItemSerial );
	if( !pPetSkillItem ) return ERROR_ITEM_NOTFOUND;

	TItemData *pPetSkillItemData = g_pDataManager->GetItemData(pPetSkillItem->nItemID);
	if( !pPetSkillItemData || pPetSkillItemData->nTypeParam[0] <= 0)
		return ERROR_ITEM_NOTFOUND;

	switch(cSlotNum)
	{
	case 1 : m_PetEquip.nSkillID1 = nSkillID = pPetSkillItemData->nTypeParam[0]; break;
	case 2 : m_PetEquip.nSkillID2 = nSkillID = pPetSkillItemData->nTypeParam[0]; break;
	default: return ERROR_ITEM_NOTFOUND;
	}
	
	//아이템 사용...	
	if (!DeleteCashInventoryBySerial( biItemSerial, 1, DBDNWorldDef::UseItem::Use ))
		return ERROR_ITEM_FAIL;

	m_pSession->GetDBConnection()->QueryModPetSkill(m_pSession->GetDBThreadID(), m_pSession->GetWorldSetID(), m_pSession->GetAccountDBID(), m_PetEquip.Vehicle[Pet::Slot::Body].nSerial, cSlotNum, pPetSkillItemData->nTypeParam[0]);
#if defined(_GAMESERVER)
	// 펫 스텟 적용
	DnActorHandle hActor = m_pSession->GetActorHandle();
	CDnPlayerActor *pPlayer = static_cast<CDnPlayerActor *>( hActor.GetPointer() );
	if( pPlayer && pPlayer->IsCanPetMode() )
	{
		if( cSlotNum == 1 && m_PetEquip.nSkillID1 > 0)
			pPlayer->AddSkill(m_PetEquip.nSkillID1);
		else if( cSlotNum == 2 && m_PetEquip.nSkillID2 > 0)
			pPlayer->AddSkill(m_PetEquip.nSkillID2);
	}
#endif
	return ERROR_NONE;
}

int CDNUserItem::PetSkillExpand(INT64 biItemSerial)
{
	if (!m_pSession->IsNoneWindowState() != WINDOW_NONE) return ERROR_ITEM_FAIL;	// 딴짓할때 스킬 확장 못함
	if( m_PetEquip.Vehicle[Pet::Slot::Body].nItemID <= 0) return ERROR_ITEM_FAIL; // 펫이 소환되어 있지 않음.
	if( m_PetEquip.bSkillSlot ) // 이미 슬롯이 확장 되어 있는지 확인
		return ERROR_ITEM_FAIL;	

	const TItem *pPetSkillItem = GetCashInventory( biItemSerial );	
	if( !pPetSkillItem ) return ERROR_ITEM_NOTFOUND;	

	//아이템 사용...	
	if (!DeleteCashInventoryBySerial( biItemSerial, 1, DBDNWorldDef::UseItem::Use ))
		return ERROR_ITEM_FAIL;

	m_PetEquip.bSkillSlot = true;

	m_pSession->GetDBConnection()->QueryModPetSkillExpand(m_pSession->GetDBThreadID(), m_pSession->GetWorldSetID(), m_pSession->GetAccountDBID(), m_PetEquip.Vehicle[Pet::Slot::Body].nSerial);
	return ERROR_NONE;
}

int CDNUserItem::DelPetSkill( char cSlotNum)
{
#if defined(_GAMESERVER)
	// 펫 스텟 적용
	DnActorHandle hActor = m_pSession->GetActorHandle();
	CDnPlayerActor *pPlayer = static_cast<CDnPlayerActor *>( hActor.GetPointer() );	
#endif // #if defined(_GAMESERVER)
	if (!m_pSession->IsNoneWindowState()) return ERROR_ITEM_FAIL;	// 딴짓할때 스킬 못 뺌
	if( m_PetEquip.Vehicle[Pet::Slot::Body].nItemID <= 0) return ERROR_ITEM_FAIL; // 펫이 소환되어 있지 않음.
	if( cSlotNum == 1 && m_PetEquip.nSkillID1 > 0)	
	{
#if defined(_GAMESERVER)
		pPlayer->RemoveSkill(m_PetEquip.nSkillID1);
#endif
		m_PetEquip.nSkillID1 = 0;
	}
	else if( cSlotNum == 2 && m_PetEquip.nSkillID2 > 0)	
	{
#if defined(_GAMESERVER)
		pPlayer->RemoveSkill(m_PetEquip.nSkillID2);
#endif
		m_PetEquip.nSkillID2 = 0;
	}
	else
		return ERROR_ITEM_FAIL;

	m_pSession->GetDBConnection()->QueryModPetSkill(m_pSession->GetDBThreadID(), m_pSession->GetWorldSetID(), m_pSession->GetAccountDBID(), m_PetEquip.Vehicle[Pet::Slot::Body].nSerial, cSlotNum, 0);
	return ERROR_NONE;	
}

void CDNUserItem::SetSource(INT64 nSerial, int nItemID, time_t tExpireDate)
{
	RemoveSource();

	m_Source.nSerial = nSerial;
	m_Source.nItemID = nItemID;
	m_Source.tExpireDate = tExpireDate;
}

TSourceData CDNUserItem::GetSource() const
{
	if (m_Source.nItemID <= 0)
		return TSourceData();

	CTimeSet timeSet;
	CTimeSet expireDate(m_Source.tExpireDate, true);
	int remainTime = static_cast<int>(expireDate - timeSet);
	if (remainTime <= 0)
		remainTime = 0;

	TSourceData source;
	source.nItemID = m_Source.nItemID;
	source.nRemainTime = remainTime;

	return source;
}

void CDNUserItem::RemoveSource()
{
	memset(&m_Source, 0, sizeof(m_Source));
}

void CDNUserItem::BroadcastSourceData(bool bUsedInGameServer) const
{
#if defined (_VILLAGESERVER)
	m_pSession->SendUserLocalMessage(0, FM_USE_SOURCE);
#else
	if (!m_pSession->GetGameRoom())
		return;

	for (DWORD i = 0; i < m_pSession->GetGameRoom()->GetUserCount(); ++i)
	{
		const CDNGameRoom::PartyStruct* pPartyStruct = m_pSession->GetGameRoom()->GetPartyData(i);
		TSourceData SourceData = GetSource();
		pPartyStruct->pSession->SendSourceData(m_pSession->GetSessionID(), SourceData, bUsedInGameServer);
	}
#endif // #if defined (_VILLAGESERVER)
}

#if defined( PRE_ADD_NAMEDITEM_SYSTEM )
void CDNUserItem::AddEffectSkillItem( INT64 nItemSerial, int nItemID, int nSkillID, int nSkillLevel, __time64_t tExpireDate, bool bEternity )
{
	TEffectSkillData EffectSkill;
	memset( &EffectSkill, 0, sizeof(TEffectSkillData) );
	EffectSkill.nItemSerial = nItemSerial;
	EffectSkill.nItemID = nItemID;
	EffectSkill.nSkillID = nSkillID;
	EffectSkill.nSkillLevel = nSkillLevel;
	EffectSkill.tExpireDate = tExpireDate;
	EffectSkill.bEternity = bEternity;
	m_mEffectSkillList.insert( std::make_pair( nSkillID, EffectSkill ) );
}

void CDNUserItem::DelEffectSkillItem( int nSkillID )
{
	map<int, TEffectSkillData>::iterator itor = m_mEffectSkillList.find( nSkillID );
	if( itor != m_mEffectSkillList.end() )
	{
#if defined( PRE_FIX_BUFFITEM )
		TEffectSkillData* EffectSkill = GetEffectSkillItem( EffectSkillNameSpace::SearchType::ItemID, itor->second.nItemID );
		if( EffectSkill )
		{				
			m_pSession->GetDBConnection()->QueryDelEffectItem( m_pSession, EffectSkill->nItemSerial );
			m_pSession->SendDelEffectSkillItemData( m_pSession->GetSessionID(), EffectSkill->nItemID );
#if defined( _VILLAGESERVER )
			m_pSession->DelPartyEffectSkillItemData();
#elif( _GAMESERVER )
			BroadcastDelEffectSkillItemData( EffectSkill->nItemID );
#endif
		}		
#endif
		m_mEffectSkillList.erase( itor );
	}
}

void CDNUserItem::GetEffectSkillItem( std::vector<TEffectSkillData>& vEffectSkill )
{
	for( std::map<int, TEffectSkillData>::iterator itor = m_mEffectSkillList.begin(); itor != m_mEffectSkillList.end(); itor++ )
	{
		CTimeSet timeSet;
		CTimeSet expireDate(itor->second.tExpireDate, true);
		int remainTime = static_cast<int>(expireDate - timeSet);
		if (remainTime <= 0 || itor->second.bEternity)
			remainTime = 0;

		TEffectSkillData EffectSkill;
		EffectSkill = itor->second;
		EffectSkill.nRemainTime = remainTime;
		vEffectSkill.push_back(EffectSkill);		
	}	
}

TEffectSkillData* CDNUserItem::GetEffectSkillItem( char cSearchType, int nValue )
{
	std::map<int, TEffectSkillData>::iterator itor = m_mEffectSkillList.begin();
	bool bCheck = false;
	switch( cSearchType )
	{
	case EffectSkillNameSpace::SearchType::ItemType:
		{
			for( ; itor != m_mEffectSkillList.end(); itor++ )
			{
				TItemData* pItemData = g_pDataManager->GetItemData( itor->second.nItemID );
				if( pItemData && pItemData->nType == nValue )
				{
					bCheck = true;
					break;
				}			
			}
		}
		break;
	case EffectSkillNameSpace::SearchType::ItemID:
		{
			for( ; itor != m_mEffectSkillList.end(); itor++ )
			{
				TItemData* pItemData = g_pDataManager->GetItemData( itor->second.nItemID );
				if( pItemData && pItemData->nItemID == nValue )
				{
					bCheck = true;
					break;
				}
			}
		}
		break;
	case EffectSkillNameSpace::SearchType::SkillID:
		{
			itor = m_mEffectSkillList.find( nValue );
			if( itor != m_mEffectSkillList.end() )
				bCheck = true;
		}
		break;
	}	
	if( bCheck )
	{
		CTimeSet timeSet;
		CTimeSet expireDate(itor->second.tExpireDate, true);
		int remainTime = static_cast<int>(expireDate - timeSet);
		if (remainTime <= 0 || itor->second.bEternity)
			remainTime = 0;

		itor->second.nRemainTime = remainTime;		
		return &itor->second;		
	}
	else
		return NULL;
}

void CDNUserItem::BroadcastEffectSkillItemData( bool bUsedInGameServer, int nSkillID, BYTE cEffectType )
{	
#if defined (_VILLAGESERVER)
	m_pSession->GetParamData()->nIndex = nSkillID;
	m_pSession->GetParamData()->cLevel = cEffectType;
	m_pSession->SendUserLocalMessage(0, FM_USE_EFFECTSKILL);
#else
	if (!m_pSession->GetGameRoom())
		return;

	if( nSkillID > 0 )
	{
		for (DWORD i = 0; i < m_pSession->GetGameRoom()->GetUserCount(); ++i)
		{
			const CDNGameRoom::PartyStruct* pPartyStruct = m_pSession->GetGameRoom()->GetPartyData(i);			
			TEffectSkillData* EffectSkill = GetEffectSkillItem( EffectSkillNameSpace::SearchType::SkillID, nSkillID );			
			if(EffectSkill)
			{
				pPartyStruct->pSession->SendEffectSkillItemData(m_pSession->GetSessionID(), *EffectSkill, bUsedInGameServer);	
				if( cEffectType > EffectSkillNameSpace::ShowEffectType::NONEEFFECT )
					pPartyStruct->pSession->SendShowEffect( EffectSkill->nItemID, m_pSession->GetSessionID(), cEffectType);
			}
		}
	}
	else
	{
		for (DWORD i = 0; i < m_pSession->GetGameRoom()->GetUserCount(); ++i)
		{
			const CDNGameRoom::PartyStruct* pPartyStruct = m_pSession->GetGameRoom()->GetPartyData(i);
			std::vector<TEffectSkillData> vEffectSkill;
			vEffectSkill.clear();
			GetEffectSkillItem( vEffectSkill);
			pPartyStruct->pSession->SendEffectSkillItemData(m_pSession->GetSessionID(), vEffectSkill, bUsedInGameServer);
		}
	}	
#endif // #if defined (_VILLAGESERVER)
}

void CDNUserItem::BroadcastDelEffectSkillItemData(int nItemID)
{	
#if defined( _GAMESERVER )
	if (!m_pSession->GetGameRoom())
		return;

	for (DWORD i = 0; i < m_pSession->GetGameRoom()->GetUserCount(); ++i)
	{
		const CDNGameRoom::PartyStruct* pPartyStruct = m_pSession->GetGameRoom()->GetPartyData(i);			
		pPartyStruct->pSession->SendDelEffectSkillItemData(m_pSession->GetSessionID(), nItemID);	
	}
#endif
}

void CDNUserItem::ApplyPartyEffectSkillItemData( CSUseItem *pPacket, TItemData *pItemData, UINT nUseSessionID, EffectSkillNameSpace::ShowEffectType::eType eType, bool bSendDB, bool bUseItem )
{
	if( !m_pSession )
		return;
	if( !pPacket || !pItemData )
		return;
	//pPacket의 biInvenSerial만 사용중이며 다른 내용도 사용하려면 서버에서 패킷생성해서 함수호출하는 부분도 수정해줘야함
	const TSkillData* pSkill = g_pDataManager->GetSkillData(pItemData->nSkillID);	

	if(!pSkill)
		return;

	int nDuration = 0;	
	// nEffectClassValue1Duration 이 0인 경우는 영구버프.
#if defined( PRE_FIX_BUFFITEM )
	int SkillDuration = 0;
	for (int i = 0; i <(int)pSkill->vLevelDataList.size(); i++)
	{
		if( pSkill->vLevelDataList[i].cSkillLevel == pItemData->cSkillLevel )
		{
			SkillDuration = pSkill->vLevelDataList[i].nEffectClassValue1Duration;
			break;
		}
	}	
	if( SkillDuration > 0 )
		nDuration = SkillDuration / 1000 / 60;
#else
	if(pSkill->vLevelDataList[0].nEffectClassValue1Duration > 0)
		nDuration = pSkill->vLevelDataList[0].nEffectClassValue1Duration / 1000 / 60;
#endif

	TItem NewItem = {0,};
	MakeItemStruct(pItemData->nItemID, NewItem, nDuration, 0);

	// 여기 들어오는 값이 일자가 시간 이라서 예외처리
	CTimeSet tTimeSet;
	tTimeSet.AddSecond(nDuration * 60);
	NewItem.tExpireDate = tTimeSet.GetTimeT64_LC();

	char cLifeSpanType = DBDNWorldDef::EffectItemLifeSpanType::New;
	int nRemoveItemID = 0;
	BYTE cEffectType = eType;

	if( eType > EffectSkillNameSpace::ShowEffectType::NONEEFFECT &&  m_pSession->GetSessionID() == nUseSessionID )
		cEffectType = EffectSkillNameSpace::ShowEffectType::SPELLEFFECT;

	DBDNWorldDef::EffectItemGetCode::eCode eGetCode = DBDNWorldDef::EffectItemGetCode::Item;

	if( pItemData->nType == ITEMTYPE_BESTFRIENDBUFFITEM )
	{
		eGetCode = DBDNWorldDef::EffectItemGetCode::BestFriend;
		NewItem.bEternity = true;
	}

	if( pItemData->cReversion == ITEMREVERSION_GUILD )
	{
		eGetCode = DBDNWorldDef::EffectItemGetCode::Guild;
	}

	TEffectSkillData* EffectSkill = GetEffectSkillItem( EffectSkillNameSpace::SearchType::SkillID, pItemData->nSkillID );
	if( EffectSkill )
	{
#if defined (_GAMESERVER)	
		if( m_pSession->GetPlayerActor() )
		{
			CDNUserSession::RemoveEffectSkill(m_pSession->GetPlayerActor(), EffectSkill);
		}
#endif

		nRemoveItemID = EffectSkill->nItemID;
		if(bSendDB)
		{
			if (!m_pSession->GetEffectRepository()->Remove( EffectSkill->nItemSerial ))
				return;

			if (pItemData->nItemID == nRemoveItemID)
			{
				cLifeSpanType = DBDNWorldDef::EffectItemLifeSpanType::Update;
				nRemoveItemID = 0;
			}
			else
			{
				cLifeSpanType = DBDNWorldDef::EffectItemLifeSpanType::New;
			}
		}
		else
		{
			DelEffectSkillItem( pItemData->nSkillID );
		}
	}
#if defined( PRE_FIX_BUFFITEM )
#if defined( _GAMESERVER )	
	else
	{
		CDNUserSession::RemoveApplySkill( m_pSession->GetPlayerActor(), pItemData->nSkillID );	
	}	
#endif
#endif

	if(bSendDB)
	{
		if (!m_pSession->GetEffectRepository()->Add(NewItem.nSerial, pItemData->nItemID, NewItem.tExpireDate))
			return;

		m_pSession->GetDBConnection()->QueryAddEffectItems(m_pSession, pItemData->nItemID, cLifeSpanType, eGetCode, 0, 0, NewItem.bEternity, nDuration, NewItem.nSerial, nRemoveItemID);
	}	
	else
	{
		// DB에 저장할 필요없는 버프들은 그냥 이펙트스킬에 바로 넣어줘서 처리함
		AddEffectSkillItem( NewItem.nSerial, pItemData->nItemID, pItemData->nSkillID, pItemData->cSkillLevel, NewItem.tExpireDate, true );		
	}
	
	EffectSkill = GetEffectSkillItem( EffectSkillNameSpace::SearchType::SkillID, pItemData->nSkillID );
	BroadcastEffectSkillItemData( bUseItem, pItemData->nSkillID, cEffectType );
#if defined( _VILLAGESERVER )
	if( EffectSkill )
		m_pSession->SendEffectSkillItemData(m_pSession->GetSessionID(), *EffectSkill, bUseItem);
#endif
#if defined (_GAMESERVER)		
	if( EffectSkill )
		CDNUserSession::ApplyEffectSkill(m_pSession->GetPlayerActor(), EffectSkill, !bUseItem, m_pSession->GetGameRoom()->bIsPvPRoom() );			
#endif // #if defined (_GAMESERVER)
}
#endif

void CDNUserItem::RefreshExpireitem(TADelExpiritem *pItem)
{
	//기간지난 아이템 우선은 캐시인벤만 체크해서 넘겨준다. 이 후에 다른부분에서도 기간만료가 들어가게 되면 추가해줘야함
	std::vector<TItem> VecItemList;
	VecItemList.clear();
	for( int i=0; i < pItem->cCount;i++)
	{
		TItem TempItem = {0,};
		const TItem *pInvenItem = GetCashInventory(pItem->biItemSerial[i]);
		if (!pInvenItem) continue;
		TItemData *pItemData = g_pDataManager->GetItemData(pInvenItem->nItemID);
		if ( !pItemData ) continue;

		if (pInvenItem->wCount <= 0) continue;	// 0이면 못쓴다
		if (!pItemData->IsCash) continue;	

		TempItem.wCount = 0;
		TempItem.nSerial = pInvenItem->nSerial;

		VecItemList.push_back(TempItem);		
	}
	m_pSession->SendRefreshCashInven(VecItemList, false);
	//DB에서는 이미 지워진 상태이기 때문에 따로 DB처리 필요없음
}

int CDNUserItem::CheckWarpVillage(INT64 nItemSerial) const
{
	if (m_pSession->GetPartyID() > 0)
		return ERROR_ITEM_CANNOT_USE_INPARTY;

	const TItem* pItem = GetCashInventory(nItemSerial);
	if (!pItem)
		return ERROR_ITEM_FAIL;

	const TItemData* pItemData = g_pDataManager->GetItemData(pItem->nItemID);
	if (!pItemData)
		return ERROR_ITEM_FAIL;

	switch (pItemData->nType)
	{
	case ITEMTYPE_FREE_PASS:
		if (pItem->wCount <= 0)
			return ERROR_ITEM_FAIL;
		break;

	case ITEMTYPE_UNLIMITED_FREE_PASS:
		break;

	default:
		return ERROR_ITEM_FAIL;
	}

	return ERROR_NONE;
}

int CDNUserItem::TryWarpVillage(int nMapIndex, INT64 nItemSerial)
{
	if (!m_pSession->IsWindowState(WINDOW_BLIND))
		return ERROR_ITEM_FAIL;

	int nResult = CheckWarpVillage(nItemSerial);
	if (nResult != ERROR_NONE)
		return nResult;

	GlobalEnum::eMapTypeEnum nMapType = g_pDataManager->GetMapType(nMapIndex);
	if (nMapType != GlobalEnum::MAP_VILLAGE)
		return ERROR_ITEM_FAIL;

	const TMapInfo* pMapInfo = g_pDataManager->GetMapInfo(m_pSession->GetMapIndex());
	if (!pMapInfo || !pMapInfo->bAllowFreePass)
		return ERROR_ITEM_FAIL;

	int nPermitLevel = 0;
#if defined (_VILLAGESERVER)
	const sChannelInfo* pChannelInfo = g_pMasterConnection->GetChannelInfoFromMapIndex(nMapIndex);
	if (pChannelInfo &&	(pChannelInfo->nChannelAttribute & GlobalEnum::CHANNEL_ATT_DARKLAIR))
		return ERROR_ITEM_FAIL;
#if defined(PRE_ADD_DWC)
	if (pChannelInfo &&	(pChannelInfo->nChannelAttribute & (GlobalEnum::CHANNEL_ATT_PVP | GlobalEnum::CHANNEL_ATT_PVPLOBBY | GlobalEnum::CHANNEL_ATT_FARMTOWN | GlobalEnum::CHANNEL_ATT_DWC)))
#else
	if (pChannelInfo &&	(pChannelInfo->nChannelAttribute & (GlobalEnum::CHANNEL_ATT_PVP | GlobalEnum::CHANNEL_ATT_PVPLOBBY | GlobalEnum::CHANNEL_ATT_FARMTOWN)))
#endif
	{
		nPermitLevel = pChannelInfo->nLimitLevel;
	}
#else
	const TChannelInfo* pChannelInfo = g_pDataManager->GetChannelInfo(m_pSession->GetWorldSetID(), nMapIndex);
	if (pChannelInfo &&	(pChannelInfo->nAttribute & GlobalEnum::CHANNEL_ATT_DARKLAIR))
		return ERROR_ITEM_FAIL;
#if defined(PRE_ADD_DWC)
	if (pChannelInfo &&	(pChannelInfo->nAttribute & (GlobalEnum::CHANNEL_ATT_PVP | GlobalEnum::CHANNEL_ATT_PVPLOBBY | GlobalEnum::CHANNEL_ATT_FARMTOWN | GlobalEnum::CHANNEL_ATT_DWC)))
#else
	if (pChannelInfo &&	(pChannelInfo->nAttribute & (GlobalEnum::CHANNEL_ATT_PVP | GlobalEnum::CHANNEL_ATT_PVPLOBBY | GlobalEnum::CHANNEL_ATT_FARMTOWN)))
#endif
	{
		nPermitLevel = pChannelInfo->nLimitLevel;
	}
#endif // #if defined (_VILLAGESERVER)
	else
	{
		nPermitLevel = g_pDataManager->GetMapPermitLevel(nMapIndex);
		if (nPermitLevel < 0)
			return ERROR_ITEM_FAIL;
	}

	if (m_pSession->GetLevel() < nPermitLevel)
		return ERROR_ITEM_INSUFFICIENTY_LEVEL;

	return m_pSession->TryWarpVillage(nMapIndex, nItemSerial);
}

bool CDNUserItem::IsExpired(const TItem& Item)
{
	if (Item.bEternity)
		return false;

	time_t CurTime;
	time( &CurTime );		

	if (Item.tExpireDate >= CurTime)
		return false;

	return true;
}

int CDNUserItem::ProcessBuyPrivateGuildRewardItem( TGuildRewardItemData* GuildRewardItemData, short wCount )
{
	TItemData *pItemData = g_pDataManager->GetItemData(GuildRewardItemData->nTypeParam1);
	if (!pItemData) return ERROR_SHOP_DATANOTFOUND;
	if (pItemData->nOverlapCount <= 0) return ERROR_SHOP_DATANOTFOUND;

	if (pItemData->nType == ITEMTYPE_QUEST) return ERROR_SHOP_NOTFORSALE;	// 퀘스트 아이템은 팔지 않는다

	int nOnePrice = GuildRewardItemData->nNeedGold;
	
#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
	CNpcReputationProcessor::CheckAndCalcUnionBenefit( m_pSession, TStoreBenefitData::BuyingPriceDiscount, nOnePrice );
#endif
	if (!m_pSession->CheckEnoughCoin(nOnePrice)) return ERROR_ITEM_INSUFFICIENCY_MONEY;	// 돈이 충분치 않다

	// 치장형 아이템
	// 먼저 인벤검사
	if (!pItemData->IsCash)
	{
		int nBlankCount = FindBlankInventorySlotCount();
		if (pItemData->nOverlapCount == 1)
		{
			if (nBlankCount < wCount) 
				return ERROR_ITEM_INVENTORY_NOTENOUGH;
		}
		else
		{
			if (nBlankCount < 1 ) 
			{
				TItem ResultItem;
				if( MakeItemStruct( pItemData->nItemID, ResultItem ) == false )
					return ERROR_ITEM_NOTFOUND;
				if( FindOverlapInventorySlot( pItemData->nItemID, wCount, ResultItem.bSoulbound, ResultItem.cSealCount, ResultItem.bEternity ) < 0 )
					return ERROR_ITEM_INVENTORY_NOTENOUGH;
			}
		}
	}		

	int nRet = CheckPrivateGuildRewardItem( GuildRewardItemData, pItemData );
	if( nRet > 0 )
		return nRet;
	char cAddMaterializedItemCode = DBDNWorldDef::AddMaterializedItem::GuildReversionItem;
	char cPayMethodCode = DBDNWorldDef::PayMethodCode::Coin;

	if( pItemData->IsCash )
	{		
		if( pItemData->nType == ITEMTYPE_GESTURE )
		{
			// 물건 산 돈만큼 빼주고...				
			m_pSession->DelCoin(nOnePrice, DBDNWorldDef::CoinChangeCode::GuildRewardBuy, pItemData->nItemID);
			m_pSession->SendEffectItemGestureAdd( pItemData->nTypeParam[0] );

			TItem NewItem = {0,};
			MakeItemStruct(pItemData->nItemID, NewItem, pItemData->nTypeParam[1], 0);

			if( m_pSession->GetEffectRepository()->Add( NewItem.nSerial, pItemData->nItemID, NewItem.tExpireDate ))
			{
				m_pSession->GetDBConnection()->QueryAddEffectItems(m_pSession, pItemData->nItemID, DBDNWorldDef::EffectItemLifeSpanType::New,
					DBDNWorldDef::EffectItemGetCode::Guild, m_pSession->GetGuildSelfView().GuildUID.nDBID, 0, NewItem.bEternity, NewItem.nLifespan, NewItem.nSerial, 0);
			}
		}
		else
		{
			m_pSession->DelCoin(nOnePrice*wCount, DBDNWorldDef::CoinChangeCode::DoNotDBSave, 0);
			int nPeriod = GuildRewardItemData->nPeriod;
			CreateCashInvenItem(pItemData->nItemID, wCount, cAddMaterializedItemCode, -1, nPeriod, nOnePrice, m_pSession->GetGuildUID().nDBID, cPayMethodCode);			
		}		
		m_pSession->SendBuyGuildRewardItem(ERROR_NONE, GuildRewardItemData->nItemID);
		return ERROR_NONE;
	}

	if (pItemData->nOverlapCount == 1)
	{
		for (int i = 0; i < wCount; i++)
		{
			if( pItemData->nType == ITEMTYPE_GESTURE )
			{
				// 물건 산 돈만큼 빼주고...				
				m_pSession->DelCoin(nOnePrice, DBDNWorldDef::CoinChangeCode::GuildRewardBuy, pItemData->nItemID);
				m_pSession->SendEffectItemGestureAdd( pItemData->nTypeParam[0] );

				TItem NewItem = {0,};
				MakeItemStruct(pItemData->nItemID, NewItem, pItemData->nTypeParam[1], 0);
				
				if( m_pSession->GetEffectRepository()->Add( NewItem.nSerial, pItemData->nItemID, NewItem.tExpireDate ))
				{
					m_pSession->GetDBConnection()->QueryAddEffectItems(m_pSession, pItemData->nItemID, DBDNWorldDef::EffectItemLifeSpanType::New,
						DBDNWorldDef::EffectItemGetCode::Guild, pItemData->nItemID, m_pSession->GetGuildSelfView().GuildUID.nDBID, NewItem.bEternity, NewItem.nLifespan, NewItem.nSerial, 0);
				}				
			}
			else
			{			
			// 아이템 인벤에 넣어주고
				std::vector<TSaveItemInfo> VecItemList;
				VecItemList.clear();
				nRet = CreateInvenItem2(pItemData->nItemID, 1, VecItemList);
				if (nRet != ERROR_NONE) return nRet;

				// 물건 산 돈만큼 빼주고...
				m_pSession->DelCoin(nOnePrice, DBDNWorldDef::CoinChangeCode::DoNotDBSave, 0);

				if (!VecItemList.empty()){
					for (int j = 0; j <(int)VecItemList.size(); j++){
						DBSendAddMaterializedItem(VecItemList[j].cSlotIndex, cAddMaterializedItemCode, m_pSession->GetGuildUID().nDBID,	VecItemList[j].Item, nOnePrice, 0, 0, 
							DBDNWorldDef::ItemLocation::Inventory, cPayMethodCode, VecItemList[j].bMerge, VecItemList[j].Item.nSerial);
					}
				}
			}
		}
	}
	else 
	{
		std::vector<TSaveItemInfo> VecItemList;
		VecItemList.clear();
		nRet = CreateInvenItem2(pItemData->nItemID, wCount, VecItemList);
		if (nRet != ERROR_NONE) return nRet;

		// 물건 산 돈만큼 빼주고...
		m_pSession->DelCoin(nOnePrice*wCount, DBDNWorldDef::CoinChangeCode::DoNotDBSave, 0);

		if (!VecItemList.empty()){
			for (int i = 0; i <(int)VecItemList.size(); i++){
				DBSendAddMaterializedItem(VecItemList[i].cSlotIndex, cAddMaterializedItemCode, m_pSession->GetGuildUID().nDBID, VecItemList[i].Item, nOnePrice*wCount, 0, 0, 
					DBDNWorldDef::ItemLocation::Inventory, cPayMethodCode, VecItemList[i].bMerge, VecItemList[i].Item.nSerial);
			}
		}
	}	
		
	m_pSession->SendBuyGuildRewardItem(ERROR_NONE, GuildRewardItemData->nItemID);
	return ERROR_NONE;
}

int CDNUserItem::ProcessBuyPublicGuildRewardItem( TGuildRewardItem *RewardItemInfo, TGuildRewardItemData* GuildRewardItemData, UINT nGuildDBID )
{
	int nOnePrice =GuildRewardItemData->nNeedGold;

#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
	CNpcReputationProcessor::CheckAndCalcUnionBenefit( m_pSession, TStoreBenefitData::BuyingPriceDiscount, nOnePrice );
#endif

	if (!m_pSession->CheckEnoughCoin(nOnePrice)) return ERROR_ITEM_INSUFFICIENCY_MONEY;	// 돈이 충분치 않다
	
		// 길드 효과 적용 아이템
#if defined( _VILLAGESERVER )
	BYTE cUpdateType = 0;	//업데이트 타입 0:생성 1:추가 2:대체
	int nIndex = GuildRewardItemData->nItemType;
	int nPrevRewardItem = 0;
	if( RewardItemInfo )
	{
		if(GuildRewardItemData->nCheckType == GUILD_REWARDITEM_CHECKTYPE_PREVITEM )
		{
			// 이전 아이템이 필요한 경우 체크
			if( RewardItemInfo[nIndex].nItemID != GuildRewardItemData->nCheckID )
			{
				//구입 불가
				return ERROR_ITEM_PREITEM_REQUEST;
			}
			//이전 아이템이 필요하면 무조건 대체
			cUpdateType = GUILDREWARDEFFECT_ADDTYPE_EXCHANGE;
			nPrevRewardItem = GuildRewardItemData->nCheckID;
		}
		else
		{
			if( RewardItemInfo[nIndex].nItemID == 0 )
			{
				// 기존 효과가 없는경우
				cUpdateType = GUILDREWARDEFFECT_ADDTYPE_NEW;
			}			
			else if( RewardItemInfo[nIndex].nItemID > 0 && RewardItemInfo[nIndex].nItemID == GuildRewardItemData->nItemID )
			{
				if( GuildRewardItemData->bEternity )
				{
					return ERROR_ITEM_ALREADY_EXIST;
				}
				// 기존 효과에 기간 추가
				cUpdateType = GUILDREWARDEFFECT_ADDTYPE_Add;;
			}
			else if( RewardItemInfo[nIndex].nItemID > 0 )
			{
				if( RewardItemInfo[nIndex].nEffectValue < GuildRewardItemData->nTypeParam1 )
				{
					cUpdateType = GUILDREWARDEFFECT_ADDTYPE_EXCHANGE;
					nPrevRewardItem = RewardItemInfo[nIndex].nItemID;
				}
				else
					return ERROR_ITEM_BETTEREFFECT_ITEM;
			}
		}

		if (m_pSession->AddDataBaseMessageSequence(MAINCMD_GUILD, QUERY_ADD_GUILDREWARDITEM) == false)
			return ERROR_GENERIC_TIME_WAIT;

		m_pSession->DelCoin(nOnePrice, DBDNWorldDef::CoinChangeCode::DoNotDBSave, 0);
		m_pSession->GetDBConnection()->QueryAddGuildRewardItem( m_pSession->GetDBThreadID(), m_pSession->GetWorldSetID(), m_pSession->GetAccountDBID(), m_pSession->GetCharacterDBID(), nGuildDBID,
			GuildRewardItemData->nItemID, GuildRewardItemData->bEternity, GuildRewardItemData->nPeriod * 24 * 60, cUpdateType, nOnePrice, m_pSession->GetMapIndex(), nPrevRewardItem );
	}
#endif	

	return ERROR_NONE;
}

int CDNUserItem::CheckPrivateGuildRewardItem( TGuildRewardItemData* GuildRewardItemData, TItemData *pItemData )
{
	if(GuildRewardItemData->nItemType == GUILDREWARDEFFECT_TYPE_MAKEGUILDMARK)
	{
		switch (pItemData->nTypeParam[0])
		{
		case 1:	// 신규 생성
			{
				if( m_pSession->GetGuildSelfView().wGuildMark != 0 && m_pSession->GetGuildSelfView().wGuildMarkBG != 0 &&
					m_pSession->GetGuildSelfView().wGuildMarkBorder != 0 )
				{				
					return ERROR_ITEM_GUILDMARK_EXIST;
				}			
			}
			break;

		case 2:	// 변경
			{
				if( m_pSession->GetGuildSelfView().wGuildMark == 0 && m_pSession->GetGuildSelfView().wGuildMarkBG == 0 &&
					m_pSession->GetGuildSelfView().wGuildMarkBorder == 0 )
				{
					return ERROR_ITEM_GUILDMARK_REQUIRE;
				}
			}
			break;
		}
	}

	if( GuildRewardItemData->nCheckType == GUILD_REWARDITEM_CHECKTYPE_GUILDMARK )
	{
		if( m_pSession->GetGuildSelfView().wGuildMark == 0 && m_pSession->GetGuildSelfView().wGuildMarkBG == 0 &&
			m_pSession->GetGuildSelfView().wGuildMarkBorder == 0 )
		{
			return ERROR_ITEM_GUILDMARK_REQUIRE;
		}
	}

	if( GuildRewardItemData->bCheckInven )
	{
		if( IsEquipCashItemExist(pItemData->nItemID) || IsValidCashItem(pItemData->nItemID, 1) )
		{
			return ERROR_ITEM_ALREADY_GUILDITEM;
		}
		else if( pItemData->nType == ITEMTYPE_GESTURE )
		{
			if( m_pSession->HasGesture( pItemData->nTypeParam[0]) )
				return ERROR_ITEM_ALREADY_GUILDITEM;
		}
		else if( pItemData->nType == ITEMTYPE_APPELLATION )
		{
			int nAppellationArray = pItemData->nTypeParam[0] - 1;
			if( m_pSession->GetAppellation()->IsExistAppellation( nAppellationArray ) ) 
			{
				return ERROR_ITEM_ALREADY_GUILDITEM;
			}
		}
	}
	
	return ERROR_NONE;
}

void CDNUserItem::CheckOneTypeCashWeapon(int nEquipIndex, bool bSend)
{
	int nCashIndex = 0;
	if (nEquipIndex == EQUIP_WEAPON1) nCashIndex = CASHEQUIP_WEAPON1;
	else if (nEquipIndex == EQUIP_WEAPON2) nCashIndex = CASHEQUIP_WEAPON2;
	const TItem *pCashWeaponItem = GetCashEquip(nCashIndex);
	if (!pCashWeaponItem) return;

	TWeaponData *pWeapon = g_pDataManager->GetWeaponData(pCashWeaponItem->nItemID);
	if (!pWeapon) return;

	if (pWeapon->bOneType){
		bool bOneTypeMatched = false;
		if (m_Equip[nEquipIndex].nItemID > 0){
			TWeaponData *pNewWeapon = g_pDataManager->GetWeaponData(m_Equip[nEquipIndex].nItemID);
			if (pNewWeapon && pNewWeapon->cEquipType == pWeapon->cEquipType) bOneTypeMatched = true;
		}
		if (!bOneTypeMatched) {
#if defined(_VILLAGESERVER)
			TItem CashEquip = *pCashWeaponItem;
			if (_PopCashEquipSlot(nCashIndex, 1) == ERROR_NONE){
				if (_PushCashInventory(CashEquip) == ERROR_NONE){
					m_pSession->GetDBConnection()->QueryChangeItemLocation(m_pSession, CashEquip.nSerial, 0, CashEquip.nItemID, 
						DBDNWorldDef::ItemLocation::CashInventory, 0, CashEquip.wCount, false, 0, false);
					if( bSend )
						m_pSession->SendMoveCashItem(MoveType_CashEquipToCashInven, nCashIndex, GetCashEquip(nCashIndex), CashEquip.nSerial, GetCashInventory(CashEquip.nSerial), ERROR_NONE);
				}
			}
#endif
#if defined(_GAMESERVER)
			CDNGameRoom *pRoom = m_pSession->GetGameRoom();
			if (pRoom && pRoom->GetTaskMng()){
				CDnItemTask *pItemTask = (CDnItemTask *)pRoom->GetTaskMng()->GetTask( "ItemTask" );
				if (pItemTask){
					CSMoveCashItem MoveCashItem;
					memset(&MoveCashItem, 0, sizeof(MoveCashItem));
					MoveCashItem.cMoveType = MoveType_CashEquipToCashInven;
					MoveCashItem.cCashEquipIndex = nCashIndex;
					MoveCashItem.biEquipItemSerial = pCashWeaponItem->nSerial;
					MoveCashItem.biCashInvenSerial = 0;
					MoveCashItem.wCount = 1;
					pItemTask->OnRecvItemMoveCashItem( m_pSession, &MoveCashItem, sizeof(CSMoveCashItem) );
				}
			}
#endif
		}
	}
}

void CDNUserItem::DBSendAddMaterializedItem(BYTE cItemSlotIndex, char cAddMaterializedItemCode, INT64 biFKey, const TItem &AddItem, INT64 biItemPrice, int nItemPeriod, INT64 biSenderCharacterDBID, 
											BYTE cItemLocationCode, BYTE cPayMethodCode, bool bMerge, INT64 biMergeTargetItemSerial, TVehicle *pVehicleEquip/* = NULL*/)
{
	m_pSession->GetDBConnection()->QueryAddMaterializedItem(m_pSession, cItemSlotIndex, cAddMaterializedItemCode, biFKey, AddItem, biItemPrice, biSenderCharacterDBID, 
		cItemLocationCode, cPayMethodCode, bMerge, biMergeTargetItemSerial, pVehicleEquip);
}

#if defined(PRE_ADD_PCBANG_RENTAL_ITEM)
bool CDNUserItem::IsHavePcCafeRentItem()
{
	// 장착창
	for( UINT i=0 ; i<_countof(m_Equip) ; ++i )
	{
		if( m_Equip[i].nItemID <= 0 )
			continue;
		if( g_pDataManager->IsPcCafeRentItem( m_Equip[i].nItemID ) == true )
			return true;		
	}

	// 인벤토리
#if defined(PRE_PERIOD_INVENTORY)
	for (int i = 0; i < INVENTORYTOTALMAX; i++){
		if (i == GetInventoryCount()){
			if (IsEnablePeriodInventory())
				i = INVENTORYMAX;
			else
				break;
		}
#else	// #if defined(PRE_PERIOD_INVENTORY)
	for (int i = 0; i < GetInventoryCount(); i++){
#endif	// #if defined(PRE_PERIOD_INVENTORY)
		if( m_Inventory[i].nItemID <= 0 )
			continue;
		if( g_pDataManager->IsPcCafeRentItem( m_Inventory[i].nItemID ) == true )
			return true;		
	}

	if( m_VehicleEquip.Vehicle[Vehicle::Slot::Body].nItemID > 0 )
	{
		if( g_pDataManager->IsPcCafeRentItem( m_VehicleEquip.Vehicle[Vehicle::Slot::Body].nItemID ) == true )
			return true;
	}

	if( m_PetEquip.Vehicle[Pet::Slot::Body].nItemID > 0 )
	{
		if( g_pDataManager->IsPcCafeRentItem( m_PetEquip.Vehicle[Pet::Slot::Body].nItemID ) == true )
			return true;
	}	

	TMapVehicle Temp = m_MapVehicleInventory;

	for( TMapVehicle::iterator itor=Temp.begin() ; itor!=Temp.end() ; ++itor )
	{
		if( g_pDataManager->IsPcCafeRentItem((*itor).second.Vehicle[Vehicle::Slot::Body].nItemID ) == true )
			return true;
	}
	return false;
}

bool CDNUserItem::RemovePCBangRentalItem(bool bSend)
{
	bool bDeleteCheck = false;
	CDNDBConnection* pDBCon = m_pSession->GetDBConnection();

	// 장착창
	for( UINT i=0 ; i<_countof(m_Equip) ; ++i )
	{
		if( m_Equip[i].nItemID <= 0 )
			continue;
		if( g_pDataManager->IsPcCafeRentItem( m_Equip[i].nItemID ) == true )
		{
			pDBCon->QueryUseItem( m_pSession, DBDNWorldDef::UseItem::Use, m_Equip[i].nSerial, m_Equip[i].wCount, true );
			_PopEquipSlot( i, m_Equip[i].wCount, bSend );
			bDeleteCheck = true;
			CheckOneTypeCashWeapon(i, false);
		}
	}

	// 인벤토리
#if defined(PRE_PERIOD_INVENTORY)
	for (int i = 0; i < INVENTORYTOTALMAX; i++){
		if (i == GetInventoryCount()){
			if (IsEnablePeriodInventory())
				i = INVENTORYMAX;
			else
				break;
		}
#else	// #if defined(PRE_PERIOD_INVENTORY)
	for (int i = 0; i < GetInventoryCount(); i++){
#endif	// #if defined(PRE_PERIOD_INVENTORY)
		if( m_Inventory[i].nItemID <= 0 )
			continue;
		if( g_pDataManager->IsPcCafeRentItem( m_Inventory[i].nItemID ) == true )
		{
			pDBCon->QueryUseItem( m_pSession, DBDNWorldDef::UseItem::Use, m_Inventory[i].nSerial, m_Inventory[i].wCount, true );
			_PopInventorySlotItem( i, m_Inventory[i].wCount );
			m_pSession->SendRefreshInven( i, &m_Inventory[i] );
			bDeleteCheck = true;
		}
	}

	if( m_VehicleEquip.Vehicle[Vehicle::Slot::Body].nItemID > 0 )
	{
		if( g_pDataManager->IsPcCafeRentItem( m_VehicleEquip.Vehicle[Vehicle::Slot::Body].nItemID ) == true )
		{
			if( m_VehicleEquip.Vehicle[Vehicle::Slot::Saddle].nItemID > 0 )
			{
				pDBCon->QueryDelPetEquipment( m_pSession, m_VehicleEquip.Vehicle[Vehicle::Slot::Body].nSerial, m_VehicleEquip.Vehicle[Vehicle::Slot::Saddle].nSerial);
				_PopVehicleEquipSlot( Vehicle::Slot::Saddle, m_VehicleEquip.Vehicle[Vehicle::Slot::Saddle].wCount, bSend );
			}
			if( m_VehicleEquip.Vehicle[Vehicle::Slot::Hair].nItemID > 0 )
			{
				pDBCon->QueryDelPetEquipment( m_pSession, m_VehicleEquip.Vehicle[Vehicle::Slot::Body].nSerial, m_VehicleEquip.Vehicle[Vehicle::Slot::Hair].nSerial);
				_PopVehicleEquipSlot( Vehicle::Slot::Hair, m_VehicleEquip.Vehicle[Vehicle::Slot::Hair].wCount, bSend );
			}

			pDBCon->QueryUseItem( m_pSession, DBDNWorldDef::UseItem::Use, m_VehicleEquip.Vehicle[Vehicle::Slot::Body].nSerial, m_VehicleEquip.Vehicle[Vehicle::Slot::Body].wCount, true );
			_PopVehicleEquipSlot( Vehicle::Slot::Body, m_VehicleEquip.Vehicle[Vehicle::Slot::Body].wCount, bSend );
			bDeleteCheck = true;
		}
	}

	if( m_PetEquip.Vehicle[Pet::Slot::Body].nItemID > 0 )
	{
		if( g_pDataManager->IsPcCafeRentItem( m_PetEquip.Vehicle[Pet::Slot::Body].nItemID ) == true )
		{
			if( m_PetEquip.Vehicle[Pet::Slot::Accessory1].nItemID > 0 )
			{
				pDBCon->QueryDelPetEquipment(m_pSession, m_PetEquip.Vehicle[Pet::Slot::Body].nSerial, m_PetEquip.Vehicle[Pet::Slot::Accessory1].nSerial);
				_PopVehicleEquipSlot( Pet::Slot::Accessory1, m_PetEquip.Vehicle[Pet::Slot::Accessory1].wCount, bSend );
			}
			if( m_PetEquip.Vehicle[Pet::Slot::Accessory2].nItemID > 0 )
			{
				pDBCon->QueryDelPetEquipment(m_pSession, m_PetEquip.Vehicle[Pet::Slot::Body].nSerial, m_PetEquip.Vehicle[Pet::Slot::Accessory2].nSerial);
				_PopVehicleEquipSlot( Pet::Slot::Accessory2, m_PetEquip.Vehicle[Pet::Slot::Accessory2].wCount, bSend );
			}

			pDBCon->QueryUseItem( m_pSession, DBDNWorldDef::UseItem::Use, m_PetEquip.Vehicle[Pet::Slot::Body].nSerial, m_PetEquip.Vehicle[Pet::Slot::Body].wCount, true );
			_PopPetEquipSlot( Pet::Slot::Body, m_PetEquip.Vehicle[Pet::Slot::Body].wCount, bSend );
			bDeleteCheck = true;
		}
	}	
	return bDeleteCheck;
}

bool  CDNUserItem::RemovePCBangRentalItemVehicle(bool bSend)
{
	bool bDeleteCheck = false;
	CDNDBConnection* pDBCon = m_pSession->GetDBConnection();

	TMapVehicle Temp = m_MapVehicleInventory;

	for( TMapVehicle::iterator itor=Temp.begin() ; itor!=Temp.end() ; ++itor )
	{
		if( g_pDataManager->IsPcCafeRentItem((*itor).second.Vehicle[Vehicle::Slot::Body].nItemID ) == false )
			continue;

		if( (*itor).second.Vehicle[Vehicle::Slot::Saddle].nItemID > 0 )
		{
			pDBCon->QueryDelPetEquipment(m_pSession, (*itor).second.Vehicle[Vehicle::Slot::Body].nSerial, (*itor).second.Vehicle[Vehicle::Slot::Saddle].nSerial);
		}
		if( (*itor).second.Vehicle[Vehicle::Slot::Hair].nItemID > 0 )
		{
			pDBCon->QueryDelPetEquipment(m_pSession, (*itor).second.Vehicle[Vehicle::Slot::Body].nSerial, (*itor).second.Vehicle[Vehicle::Slot::Hair].nSerial);
		}

		pDBCon->QueryUseItem( m_pSession, DBDNWorldDef::UseItem::Use, (*itor).second.Vehicle[Vehicle::Slot::Body].nSerial, (*itor).second.Vehicle[Vehicle::Slot::Body].wCount, true );
		_PopVehicleInventoryBySerial((*itor).second.Vehicle[Vehicle::Slot::Body].nSerial );

		// Refresh
		for( int i=0 ; i<Vehicle::Slot::Max ; ++i )
			(*itor).second.Vehicle[i].wCount = 0;
		if( bSend )
			m_pSession->SendRefreshVehicleInven((*itor).second );

		if( m_nVehicleInventoryTotalCount > 0 )
			--m_nVehicleInventoryTotalCount;
		else
			_ASSERT(0);
		bDeleteCheck = true;
	}
	return bDeleteCheck;
}

bool CDNUserItem::CreatePCRentalItem(int nItemID, int nOptionID)
{
	const TItemData* pItemData = g_pDataManager->GetItemData( nItemID );
	if( !pItemData )
		return false;

	if( pItemData->IsCash )	
	{
		if( CreateCashInvenItem(nItemID, 1, DBDNWorldDef::AddMaterializedItem::Trigger, -1, 0, 0, 0, DBDNWorldDef::PayMethodCode::Mission) != ERROR_NONE)			
			return false;
	}
	else
	{
		std::vector<TSaveItemInfo> VecItemList;
		VecItemList.clear();

		TItem AddItem = { 0, };
		MakeItemStruct(nItemID, AddItem, 0, nOptionID);
		AddItem.wCount = 1;

		if( _CreateInvenItemEx(AddItem, DBDNWorldDef::AddMaterializedItem::Trigger, 0, VecItemList, false, CREATEINVEN_ETC) != ERROR_NONE)
			return false;
	}		
	return true;
}
#endif

#if defined(PRE_ADD_SERVER_WAREHOUSE)
int CDNUserItem::CopySendServerWare(TItem* pData)
{
	int nCount = 0;
	for( TMapItem::iterator itor = m_MapServerWarehouse.begin(); itor != m_MapServerWarehouse.end(); ++itor)
	{
		if( itor->second.nItemID > 0 )					
		{
			memcpy(&pData[nCount], &itor->second, sizeof(TItem));		
			++nCount;
		}		
	}
	return nCount;
}

int CDNUserItem::CopySendServerWareCash(TItem* pData)
{
	int nCount = 0;
	for( TMapItem::iterator itor = m_MapServerWarehouseCash.begin(); itor != m_MapServerWarehouseCash.end(); ++itor)
	{
		if( itor->second.nItemID > 0 )					
		{
			memcpy(&pData[nCount], &itor->second, sizeof(TItem));		
			++nCount;
		}		
	}
	return nCount;
}
#endif

void CDNUserItem::InitializeSampleShopItem(int nSampleVersion)
{
	INT64 biValue = 0;
	m_pSession->GetCommonVariableDataValue(CommonVariable::Type::SampleDataVersion, biValue);

	if (biValue != nSampleVersion){
		m_pSession->ModCommonVariableData(CommonVariable::Type::SampleDataVersion, nSampleVersion);
		m_pSession->ModCommonVariableData(CommonVariable::Type::SampleItem1, biValue);
		m_pSession->ModCommonVariableData(CommonVariable::Type::SampleItem2, biValue);
		m_pSession->ModCommonVariableData(CommonVariable::Type::SampleItem3, biValue);
		m_pSession->ModCommonVariableData(CommonVariable::Type::SampleItem4, biValue);
		m_pSession->ModCommonVariableData(CommonVariable::Type::SampleItem5, biValue);
		m_pSession->ModCommonVariableData(CommonVariable::Type::SampleItem6, biValue);
		m_pSession->ModCommonVariableData(CommonVariable::Type::SampleItem7, biValue);
		m_pSession->ModCommonVariableData(CommonVariable::Type::SampleItem8, biValue);
		m_pSession->ModCommonVariableData(CommonVariable::Type::SampleItem9, biValue);
		m_pSession->ModCommonVariableData(CommonVariable::Type::SampleItem10, biValue);
	}
}

int CDNUserItem::CheckSampleShopItem(int nItemID)
{
	INT64 biValue = 0;
	int nCount = 0;

	m_pSession->GetCommonVariableDataValue(CommonVariable::Type::SampleItem1, biValue);
	if (biValue == nItemID) return ERROR_ITEM_BUY_OVERFLOW_COUNT;
	if (biValue == 0) nCount++;
	m_pSession->GetCommonVariableDataValue(CommonVariable::Type::SampleItem2, biValue);
	if (biValue == nItemID) return ERROR_ITEM_BUY_OVERFLOW_COUNT;
	if (biValue == 0) nCount++;
	m_pSession->GetCommonVariableDataValue(CommonVariable::Type::SampleItem3, biValue);
	if (biValue == nItemID) return ERROR_ITEM_BUY_OVERFLOW_COUNT;
	if (biValue == 0) nCount++;
	m_pSession->GetCommonVariableDataValue(CommonVariable::Type::SampleItem4, biValue);
	if (biValue == nItemID) return ERROR_ITEM_BUY_OVERFLOW_COUNT;
	if (biValue == 0) nCount++;
	m_pSession->GetCommonVariableDataValue(CommonVariable::Type::SampleItem5, biValue);
	if (biValue == nItemID) return ERROR_ITEM_BUY_OVERFLOW_COUNT;
	if (biValue == 0) nCount++;
	m_pSession->GetCommonVariableDataValue(CommonVariable::Type::SampleItem6, biValue);
	if (biValue == nItemID) return ERROR_ITEM_BUY_OVERFLOW_COUNT;
	if (biValue == 0) nCount++;
	m_pSession->GetCommonVariableDataValue(CommonVariable::Type::SampleItem7, biValue);
	if (biValue == nItemID) return ERROR_ITEM_BUY_OVERFLOW_COUNT;
	if (biValue == 0) nCount++;
	m_pSession->GetCommonVariableDataValue(CommonVariable::Type::SampleItem8, biValue);
	if (biValue == nItemID) return ERROR_ITEM_BUY_OVERFLOW_COUNT;
	if (biValue == 0) nCount++;
	m_pSession->GetCommonVariableDataValue(CommonVariable::Type::SampleItem9, biValue);
	if (biValue == nItemID) return ERROR_ITEM_BUY_OVERFLOW_COUNT;
	if (biValue == 0) nCount++;
	m_pSession->GetCommonVariableDataValue(CommonVariable::Type::SampleItem10, biValue);
	if (biValue == nItemID) return ERROR_ITEM_BUY_OVERFLOW_COUNT;
	if (biValue == 0) nCount++;

	if (nCount == 0) return ERROR_ITEM_OVERFLOW;

	return ERROR_NONE;
}

void CDNUserItem::SetSampleShopItem(int nItemID)
{
	INT64 biValue = 0;

	m_pSession->GetCommonVariableDataValue(CommonVariable::Type::SampleItem1, biValue);
	if (biValue == 0){
		m_pSession->ModCommonVariableData(CommonVariable::Type::SampleItem1, nItemID);
		return;
	}

	m_pSession->GetCommonVariableDataValue(CommonVariable::Type::SampleItem2, biValue);
	if (biValue == 0){
		m_pSession->ModCommonVariableData(CommonVariable::Type::SampleItem2, nItemID);
		return;
	}

	m_pSession->GetCommonVariableDataValue(CommonVariable::Type::SampleItem3, biValue);
	if (biValue == 0){
		m_pSession->ModCommonVariableData(CommonVariable::Type::SampleItem3, nItemID);
		return;
	}

	m_pSession->GetCommonVariableDataValue(CommonVariable::Type::SampleItem4, biValue);
	if (biValue == 0){
		m_pSession->ModCommonVariableData(CommonVariable::Type::SampleItem4, nItemID);
		return;
	}

	m_pSession->GetCommonVariableDataValue(CommonVariable::Type::SampleItem5, biValue);
	if (biValue == 0){
		m_pSession->ModCommonVariableData(CommonVariable::Type::SampleItem5, nItemID);
		return;
	}

	m_pSession->GetCommonVariableDataValue(CommonVariable::Type::SampleItem6, biValue);
	if (biValue == 0){
		m_pSession->ModCommonVariableData(CommonVariable::Type::SampleItem6, nItemID);
		return;
	}

	m_pSession->GetCommonVariableDataValue(CommonVariable::Type::SampleItem7, biValue);
	if (biValue == 0){
		m_pSession->ModCommonVariableData(CommonVariable::Type::SampleItem7, nItemID);
		return;
	}

	m_pSession->GetCommonVariableDataValue(CommonVariable::Type::SampleItem8, biValue);
	if (biValue == 0){
		m_pSession->ModCommonVariableData(CommonVariable::Type::SampleItem8, nItemID);
		return;
	}

	m_pSession->GetCommonVariableDataValue(CommonVariable::Type::SampleItem9, biValue);
	if (biValue == 0){
		m_pSession->ModCommonVariableData(CommonVariable::Type::SampleItem9, nItemID);
		return;
	}

	m_pSession->GetCommonVariableDataValue(CommonVariable::Type::SampleItem10, biValue);
	if (biValue == 0){
		m_pSession->ModCommonVariableData(CommonVariable::Type::SampleItem10, nItemID);
		return;
	}
}

int CDNUserItem::CheckModGiftReceive(TAModGiftReceiveFlag *pCashShop)
{
#if defined(PRE_RECEIVEGIFTALL)
	if (pCashShop->cCount <= 0)	// 상품이 하나도 없다
		return ERROR_ITEM_FAIL;

	std::vector<TEffectItemData> VecEffectItemList, VecCashList;
	VecEffectItemList.clear();
	VecCashList.clear();

	int nRet = 0;

	for (int j = 0; j < pCashShop->cCount; j++){
		TCashPackageData PackageData;
		bool bPackage = g_pDataManager->GetCashPackageData(pCashShop->ReceiveGift[j].nItemSN, PackageData);
		if (bPackage){	// 패키지일때
			std::vector<DBPacket::TItemSNIDOption> VecItemSNID;
			VecItemSNID.clear();

			for (int i = 0; i < PACKAGEITEMMAX; i++){
				if (pCashShop->ReceiveGift[j].ItemIDOptions[i].nItemID == 0) continue;

				DBPacket::TItemSNIDOption ItemSNID = {0,};
				ItemSNID.nItemID = pCashShop->ReceiveGift[j].ItemIDOptions[i].nItemID;
				VecItemSNID.push_back(ItemSNID);
			}
			if (!g_pDataManager->GetCashCommodityItemSNListByPackage(pCashShop->ReceiveGift[j].nItemSN, VecItemSNID)){
				return ERROR_ITEM_FAIL;
			}

			for (int i = 0; i < (int)VecItemSNID.size(); i++){
				TEffectItemData CashInfo = {0,};
				CashInfo.nGiftDBID = pCashShop->ReceiveGift[j].GiftData.nGiftDBID;
				CashInfo.nItemSN = VecItemSNID[i].nItemSN;
				CashInfo.nItemID = VecItemSNID[i].nItemID;
				CashInfo.nCount = g_pDataManager->GetCashCommodityCount(VecItemSNID[i].nItemSN);
				VecCashList.push_back(CashInfo);

				if (IsEffectCashItem(VecItemSNID[i].nItemID)){	// 무형아이템이라면
					VecEffectItemList.push_back(CashInfo);
				}
			}
		}
		else{	// 패키지 아닐때
			int nItemID = pCashShop->ReceiveGift[j].ItemIDOptions[0].nItemID;

			if ((pCashShop->ReceiveGift[j].nItemSN <= 0) ||(nItemID <= 0)){
				return ERROR_ITEM_FAIL;
			}

			TEffectItemData CashInfo = {0,};
			CashInfo.nGiftDBID = pCashShop->ReceiveGift[j].GiftData.nGiftDBID;
			CashInfo.nItemSN = pCashShop->ReceiveGift[j].nItemSN;
			CashInfo.nItemID = nItemID;
			CashInfo.nCount = g_pDataManager->GetCashCommodityCount(pCashShop->ReceiveGift[j].nItemSN);
			VecCashList.push_back(CashInfo);

			if (IsEffectCashItem(nItemID)){	// 무형아이템이라면
				VecEffectItemList.push_back(CashInfo);
			}
		}
	}

	CheckCashDuplicationBuy(VecCashList, pCashShop->bReceiveAll);
	for (int i = 0; i < (int)VecCashList.size(); i++){
		if (!VecCashList[i].bFail) continue;

		if (!m_pSession->m_VecReceiveGiftResultList.empty()){
			std::vector<INT64>::iterator iter = find(m_pSession->m_VecReceiveGiftResultList.begin(), m_pSession->m_VecReceiveGiftResultList.end(), VecCashList[i].nGiftDBID);
			if (iter != m_pSession->m_VecReceiveGiftResultList.end()){
				m_pSession->m_VecReceiveGiftResultList.erase(iter);
			}
		}

		for (int j = 0; j < pCashShop->cCount; j++){
			if (VecCashList[i].nGiftDBID == pCashShop->ReceiveGift[j].GiftData.nGiftDBID)
				pCashShop->ReceiveGift[j].bFail = true;
		}
	}

	CheckEffectItemListCountLimit(VecEffectItemList, true, false, pCashShop->bReceiveAll);	// 캐쉬템 몇개까지 살 수 있는지 max검사
	for (int i = 0; i < (int)VecEffectItemList.size(); i++){
		if (!VecEffectItemList[i].bFail) continue;

		if (!m_pSession->m_VecReceiveGiftResultList.empty()){
			std::vector<INT64>::iterator iter = find(m_pSession->m_VecReceiveGiftResultList.begin(), m_pSession->m_VecReceiveGiftResultList.end(), VecEffectItemList[i].nGiftDBID);
			if (iter != m_pSession->m_VecReceiveGiftResultList.end()){
				m_pSession->m_VecReceiveGiftResultList.erase(iter);
			}
		}

		for (int j = 0; j < pCashShop->cCount; j++){
			if (VecEffectItemList[i].nGiftDBID == pCashShop->ReceiveGift[j].GiftData.nGiftDBID)
				pCashShop->ReceiveGift[j].bFail = true;
		}
	}
#endif	// #if defined(PRE_RECEIVEGIFTALL)

	return ERROR_NONE;
}

int CDNUserItem::ModGiftReceiveFlag(const TAModGiftReceiveFlag *pCashShop)
{
	if (pCashShop->nRetCode != ERROR_NONE)
		return pCashShop->nRetCode;

	if (pCashShop->cCount <= 0)	// 상품이 하나도 없다
		return ERROR_ITEM_FAIL;

	std::vector<TEffectItemData> VecEffectItemList, VecCashList;
	VecEffectItemList.clear();
	VecCashList.clear();

	int nRet = 0;

	bool bReceiveAll = false;
#if defined(PRE_RECEIVEGIFTALL)
	bReceiveAll = pCashShop->bReceiveAll;
#endif	// #if defined(PRE_RECEIVEGIFTALL)

	for (int j = 0; j < pCashShop->cCount; j++){
		if (pCashShop->ReceiveGift[j].bFail) continue;

		TCashPackageData PackageData;
		bool bPackage = g_pDataManager->GetCashPackageData(pCashShop->ReceiveGift[j].nItemSN, PackageData);
		if (bPackage){	// 패키지일때
			TGiftItem AddItemList[PACKAGEITEMMAX] = {0, };

			std::vector<DBPacket::TItemSNIDOption> VecItemSNID;
			VecItemSNID.clear();

			for (int i = 0; i < PACKAGEITEMMAX; i++){
				if (pCashShop->ReceiveGift[j].ItemIDOptions[i].nItemID == 0) continue;

				DBPacket::TItemSNIDOption ItemSNID = {0,};
				ItemSNID.nItemID = pCashShop->ReceiveGift[j].ItemIDOptions[i].nItemID;
				VecItemSNID.push_back(ItemSNID);
			}
			if (!g_pDataManager->GetCashCommodityItemSNListByPackage(pCashShop->ReceiveGift[j].nItemSN, VecItemSNID)){
				return ERROR_ITEM_FAIL;
			}

			for (int i = 0; i < (int)VecItemSNID.size(); i++){
				if (!bReceiveAll){
					TEffectItemData CashInfo = {0,};
					CashInfo.nItemSN = VecItemSNID[i].nItemSN;
					CashInfo.nItemID = VecItemSNID[i].nItemID;
					CashInfo.nCount = g_pDataManager->GetCashCommodityCount(VecItemSNID[i].nItemSN);
					VecCashList.push_back(CashInfo);

					if (IsEffectCashItem(VecItemSNID[i].nItemID)){	// 무형아이템이라면
						VecEffectItemList.push_back(CashInfo);
					}
				}

				nRet = MakeGiftCashItem(VecItemSNID[i].nItemSN, VecItemSNID[i].nItemID, pCashShop->ReceiveGift[j].ItemIDOptions[i].cOption, AddItemList[i]);
				if (nRet != ERROR_NONE){
					return nRet;
				}
			}

			int nTotalCount = 1;
#if defined(PRE_RECEIVEGIFTALL)
			if (bReceiveAll)
				nTotalCount = (int)m_pSession->m_VecReceiveGiftResultList.size();
#endif	// #if defined(PRE_RECEIVEGIFTALL)

			if (!bReceiveAll){
				if (!VecEffectItemList.empty()){
					nRet = CheckEffectItemListCountLimit(VecEffectItemList, true, false, bReceiveAll);	// 캐쉬템 몇개까지 살 수 있는지 max검사
					if (nRet != ERROR_NONE){
						return nRet;
					}
				}

				if (!VecCashList.empty()){
					nRet = CheckCashDuplicationBuy(VecCashList, bReceiveAll);
					if (nRet != ERROR_NONE){
						return nRet;
					}
				}
			}

			m_pSession->GetDBConnection()->QueryReceiveGift(m_pSession, bReceiveAll, nTotalCount, pCashShop->ReceiveGift[j].GiftData, (int)PackageData.nVecCommoditySN.size(), AddItemList, pCashShop->ReceiveGift[j].nItemSN);
		}
		else{	// 패키지 아닐때
			int nItemID = pCashShop->ReceiveGift[j].ItemIDOptions[0].nItemID;

			if ((pCashShop->ReceiveGift[j].nItemSN <= 0) ||(nItemID <= 0)){
				return ERROR_ITEM_FAIL;
			}

			if (!bReceiveAll){
				TEffectItemData CashInfo = {0,};
				CashInfo.nItemSN = pCashShop->ReceiveGift[j].nItemSN;
				CashInfo.nItemID = nItemID;
				CashInfo.nCount = g_pDataManager->GetCashCommodityCount(pCashShop->ReceiveGift[j].nItemSN);
				VecCashList.push_back(CashInfo);

				if (IsEffectCashItem(nItemID)){	// 무형아이템이라면
					VecEffectItemList.push_back(CashInfo);
				}

				if (!VecEffectItemList.empty()){
					nRet = CheckEffectItemListCountLimit(VecEffectItemList, true, false, bReceiveAll);	// 무형 캐쉬템 몇개까지 살 수 있는지 max검사
					if (nRet != ERROR_NONE){
						return nRet;
					}
				}
				if (!VecCashList.empty()){
					nRet = CheckCashDuplicationBuy(VecCashList, bReceiveAll);
					if (nRet != ERROR_NONE){
						return nRet;
					}
				}
			}

			TGiftItem AddCashItem = { 0, };
			nRet = MakeGiftCashItem(pCashShop->ReceiveGift[j].nItemSN, nItemID, pCashShop->ReceiveGift[j].ItemIDOptions[0].cOption, AddCashItem);
			if (nRet != ERROR_NONE)
				return nRet;

			int nTotalCount = 1;
#if defined(PRE_RECEIVEGIFTALL)
			if (bReceiveAll)
				nTotalCount = (int)m_pSession->m_VecReceiveGiftResultList.size();
#endif	// #if defined(PRE_RECEIVEGIFTALL)

			m_pSession->GetDBConnection()->QueryReceiveGift(m_pSession, bReceiveAll, nTotalCount, pCashShop->ReceiveGift[j].GiftData, 1, &AddCashItem, pCashShop->ReceiveGift[j].nItemSN);
		}
	}

	return ERROR_NONE;
}

int CDNUserItem::CheckCashMail(std::vector<TItem> &VecItemList)
{
	if (VecItemList.empty()) return ERROR_ITEM_FAIL;

	std::vector<TEffectItemData> VecEffectItemList, VecCashList;
	VecEffectItemList.clear();
	VecCashList.clear();

	int nRet = 0;

	for (int i = 0; i < (int)VecItemList.size(); i++){
		TEffectItemData CashInfo = {0,};
		CashInfo.nItemID = VecItemList[i].nItemID;
		CashInfo.nCount = VecItemList[i].wCount;
		VecCashList.push_back(CashInfo);

		if (IsEffectCashItem(VecItemList[i].nItemID)){	// 무형아이템이라면
			VecEffectItemList.push_back(CashInfo);
		}
	}

	nRet = CheckCashDuplicationBuy(VecCashList, false);
	if (nRet != ERROR_NONE)
		return nRet;

	nRet = CheckEffectItemListCountLimit(VecEffectItemList, true, false, false);
	if (nRet != ERROR_NONE)
		return nRet;

	return ERROR_NONE;
}

#if defined(PRE_ADD_EQUIPLOCK)
void CDNUserItem::LoadLockItem(const TAGetListLockedItems* pData)
{
	time_t CurTime;
	time( &CurTime );		

	for(int i = 0; i < pData->nCount; i++)
	{
		//잠금 해제가 가능한지 확인한다
		if( (pData->LockItem[i].ItemData.tUnLockDate <= CurTime) && (pData->LockItem[i].ItemData.eItemLockStatus == EquipItemLock::RequestUnLock) )
		{
			//잠금 상태를 해제한다
			UnLockEquipItem(pData->LockItem[i].cItemCode, pData->LockItem[i].cItemSlotIndex, false);
			continue;
		}
		
		//잠금 슬롯이 맞는지 확인
		switch(pData->LockItem[i].cItemCode)
		{
		case DBDNWorldDef::ItemLocation::Equip:
			{
				if(pData->LockItem[i].cItemSlotIndex >= EQUIPMAX) break;
				if(pData->LockItem[i].biItemSerial == 0) break;
				if(m_Equip[pData->LockItem[i].cItemSlotIndex].nItemID <= 0)
				{
					//구울모드 예외처리
					int nIndex = FindInventorySlotBySerial(pData->LockItem[i].biItemSerial);
					if(nIndex == -1) break;
					const TItem* pItem = GetInventory( nIndex );
					if( pItem == NULL ) break;
					//장착 가능한지 확인
					int nRet = IsEquipEnableItem(*pItem);
					if (nRet != ERROR_NONE) break;
					if (!IsEquipItem(pData->LockItem[i].cItemSlotIndex, pItem->nItemID)) break;
			
					if( _SwapInvenToEquip(nIndex, pData->LockItem[i].cItemSlotIndex) != ERROR_NONE ) break;
				}
				m_EquipLock[pData->LockItem[i].cItemSlotIndex] = pData->LockItem[i].ItemData;
				continue;
			}
			break;
		case DBDNWorldDef::ItemLocation::CashEquip:
			{
				if(pData->LockItem[i].cItemSlotIndex >= CASHEQUIPMAX) break;
				if(pData->LockItem[i].biItemSerial == 0) break;
				if(m_CashEquip[pData->LockItem[i].cItemSlotIndex].nItemID <= 0) break;
				m_CashEquipLock[pData->LockItem[i].cItemSlotIndex] = pData->LockItem[i].ItemData;
				continue;
			}
			break;
		}
		UnLockEquipItem(pData->LockItem[i].cItemCode, pData->LockItem[i].cItemSlotIndex, false);
	}	
}
//잠금 장비 리스트를 가져오는 함수
void CDNUserItem::GetLockItemList(int &nEquipLockCount, int &nCashEquipLockCount, EquipItemLock::TLockItemInfo* pLockList)
{
	for(int i = 0; i< EQUIPMAX; i++)
	{
		if(m_EquipLock[i].eItemLockStatus == None)
			continue;

		//LockItemList를 전송하기 직전에 렌탈 아이템을 지워주고 있음
		//렌탈 아이템이 삭제되면 해당 슬롯의 잠금 정보도 삭제
		if(m_Equip[i].nItemID <= 0)
		{
			UnLockEquipItem(DBDNWorldDef::ItemLocation::Equip, i, false);
			continue;
		}

		pLockList[nEquipLockCount].ItemData = m_EquipLock[i];
		pLockList[nEquipLockCount].cItemSlotIndex = i;
		nEquipLockCount++;
	}
	for(int i = 0; i< CASHEQUIPMAX; i++)
	{
		if(m_CashEquipLock[i].eItemLockStatus == None)
			continue;

		//LockItemList를 전송하기 직전에 렌탈 아이템을 지워주고 있음
		//잠금 정보를 전송하기 전에 캐쉬쪽도 한번더 검증을 거쳐준다
		if(m_CashEquip[i].nItemID <= 0)
		{
			UnLockEquipItem(DBDNWorldDef::ItemLocation::CashEquip, i, false);
			continue;
		}

		pLockList[nEquipLockCount + nCashEquipLockCount].ItemData = m_CashEquipLock[i];
		pLockList[nEquipLockCount + nCashEquipLockCount].cItemSlotIndex = i;
		nCashEquipLockCount++;
	}
}
//해당 슬롯이 잠금 슬롯이 될수 있는지 확인하는 함수
bool CDNUserItem::IsValidEquipLockSlot(BYTE cItemLocation, BYTE cItemSlotIndex, INT64 biSerial)
{
	if(cItemLocation == DBDNWorldDef::ItemLocation::Equip)
		return IsValidEquipSlot(cItemSlotIndex, biSerial, (biSerial != 0));
	else if(cItemLocation == DBDNWorldDef::ItemLocation::CashEquip)
		return IsValidCashEquipSlot(cItemSlotIndex, biSerial, (biSerial != 0));

	return false;
}

bool CDNUserItem::IsValidCashEquipSlot(int nIndex, INT64 biSerial, bool bCheckSerial /*= true*/) const
{
	if( !_CheckRangeCashEquipIndex(nIndex) ) return false;
	if( m_CashEquip[nIndex].nItemID <= 0 ) return false;
	if( bCheckSerial && (m_CashEquip[nIndex].nSerial != biSerial)) return false;

	return true;
}

//해당 아이템이 잠겼는지 확인하는 함수
bool CDNUserItem::IsLockItem(BYTE cItemLocation, BYTE cItemSlotIndex)
{
	if(!IsValidEquipLockSlot(cItemLocation, cItemSlotIndex))
		return false;

	//해당 아이템이 잠금해제가 가능한지 확인 -> true가 리턴되면 잠긴 아이템을 해제한것.
	if(CheckItemUnLock(cItemLocation, cItemSlotIndex))
		return false;
	
	switch(cItemLocation)
	{
	case DBDNWorldDef::ItemLocation::Equip:
		{
			if(m_EquipLock[cItemSlotIndex].eItemLockStatus != EquipItemLock::None)
				return true;
		}
		break;
	case DBDNWorldDef::ItemLocation::CashEquip:
		{
			if(m_CashEquipLock[cItemSlotIndex].eItemLockStatus != EquipItemLock::None)
				return true;
		}
		break;
	}
	return false;
}

void CDNUserItem::LockEquipItem(BYTE cItemLocation, BYTE cItemSlotIndex, __time64_t LockDate)
{
	if(!IsValidEquipLockSlot(cItemLocation, cItemSlotIndex))
		return ;
	
	switch(cItemLocation)
	{
	case DBDNWorldDef::ItemLocation::Equip:
		{
			m_EquipLock[cItemSlotIndex].eItemLockStatus = EquipItemLock::Lock;
			m_EquipLock[cItemSlotIndex].tUnLockDate = LockDate;
			m_EquipLock[cItemSlotIndex].tUnLockRequestDate = 0;
		}
		break;
	case DBDNWorldDef::ItemLocation::CashEquip:
		{
			m_CashEquipLock[cItemSlotIndex].eItemLockStatus = EquipItemLock::Lock;
			m_CashEquipLock[cItemSlotIndex].tUnLockDate = LockDate;
			m_CashEquipLock[cItemSlotIndex].tUnLockRequestDate = 0;
		}
		break;
	}
}
void CDNUserItem::RequestUnLockEquipItem(BYTE cItemLocation, BYTE cItemSlotIndex, __time64_t UnLockDate, __time64_t UnLockRequestDate)
{
	if(!IsValidEquipLockSlot(cItemLocation, cItemSlotIndex))
		return ;

	switch(cItemLocation)
	{
	case DBDNWorldDef::ItemLocation::Equip:
		{
			m_EquipLock[cItemSlotIndex].eItemLockStatus = EquipItemLock::RequestUnLock;
			m_EquipLock[cItemSlotIndex].tUnLockDate = UnLockDate;
			m_EquipLock[cItemSlotIndex].tUnLockRequestDate = UnLockRequestDate;
		}
		break;
	case DBDNWorldDef::ItemLocation::CashEquip:
		{
			m_CashEquipLock[cItemSlotIndex].eItemLockStatus = EquipItemLock::RequestUnLock;
			m_CashEquipLock[cItemSlotIndex].tUnLockDate = UnLockDate;
			m_CashEquipLock[cItemSlotIndex].tUnLockRequestDate = UnLockRequestDate;
		}
		break;
	}
}

void CDNUserItem::UnLockEquipItem(BYTE cItemLocation, BYTE cItemSlotIndex, bool CheckValidItem)
{
	//잠금 모드를 해제할때, CheckValidItem가 true인 경우에만 슬롯인덱스와 해당 슬롯에 장비를 확인한다
	if(!IsValidEquipLockSlot(cItemLocation, cItemSlotIndex) && CheckValidItem)
		return ;

	int nItemID = 0;
	INT64 biItemSerial = 0;

	switch(cItemLocation)
	{
	case DBDNWorldDef::ItemLocation::Equip:
		{
			if(cItemSlotIndex < EQUIPMAX)
			{
				m_EquipLock[cItemSlotIndex].eItemLockStatus = EquipItemLock::None;
				m_EquipLock[cItemSlotIndex].tUnLockDate = 0;
				m_EquipLock[cItemSlotIndex].tUnLockRequestDate = 0;
				nItemID = m_Equip[cItemSlotIndex].nItemID;
				biItemSerial = m_Equip[cItemSlotIndex].nSerial;
			}
		}
		break;
	case DBDNWorldDef::ItemLocation::CashEquip:
		{
			if(cItemSlotIndex < CASHEQUIPMAX)
			{
				m_CashEquipLock[cItemSlotIndex].eItemLockStatus = EquipItemLock::None;
				m_CashEquipLock[cItemSlotIndex].tUnLockDate = 0;
				m_CashEquipLock[cItemSlotIndex].tUnLockRequestDate = 0;
				nItemID = m_CashEquip[cItemSlotIndex].nItemID;
				biItemSerial = m_CashEquip[cItemSlotIndex].nSerial;
			}
		}
		break;
	}

	m_pSession->GetDBConnection()->QueryUnLockItem(m_pSession, (DBDNWorldDef::ItemLocation::eCode)cItemLocation, cItemSlotIndex, nItemID, biItemSerial);
}

//요청한 슬롯이 잠금 해제가 가능하면 해제하는 함수
bool CDNUserItem::CheckItemUnLock(BYTE cItemLocation, BYTE cItemSlotIndex)
{
	time_t CurTime;
	time( &CurTime );		

	switch(cItemLocation)
	{
	case DBDNWorldDef::ItemLocation::Equip:
		{
			if( m_EquipLock[cItemSlotIndex].tUnLockDate > CurTime || m_EquipLock[cItemSlotIndex].eItemLockStatus != EquipItemLock::RequestUnLock )
				return false;
		}
		break;
	case DBDNWorldDef::ItemLocation::CashEquip:
		{
			if( m_CashEquipLock[cItemSlotIndex].tUnLockDate > CurTime || m_CashEquipLock[cItemSlotIndex].eItemLockStatus != EquipItemLock::RequestUnLock)
				return false;
		}
		break;
	default:
		return false;
	}

	UnLockEquipItem(cItemLocation, cItemSlotIndex);
	return true;
}
#endif	// #if defined(PRE_ADD_EQUIPLOCK)

#if defined( PRE_ADD_STAGE_USECOUNT_ITEM )
int CDNUserItem::GetUseLimitItemCount( int nItemID )
{
#if defined( _GAMESERVER )
	CDNGameRoom::PartyStruct *pStruct = m_pSession->GetGameRoom()->GetPartyData(m_pSession);
	if( pStruct )
	{
		std::map<int,int>::iterator itor = pStruct->UseLimitItem.find(nItemID);
		if( itor != pStruct->UseLimitItem.end() )
		{			
			return itor->second;
		}
	}	
#endif
	return -1;
}

void CDNUserItem::DelUseLimitItemCount( int nItemID, int nCount/*=1*/)
{
#if defined( _GAMESERVER )
	CDNGameRoom::PartyStruct *pStruct = m_pSession->GetGameRoom()->GetPartyData(m_pSession);
	if( pStruct )
	{
		std::map<int,int>::iterator itor = pStruct->UseLimitItem.find(nItemID);
		if( itor != pStruct->UseLimitItem.end() )
		{			
			itor->second = itor->second-nCount;
			m_pSession->SendStageUseLimitItem(nItemID, itor->second);
		}
	}		
#endif
}
#endif

void CDNUserItem::GetInventoryList(TItemInfo *InventoryList, BYTE &cTotalCount)
{
	int nCount = 0;

#if defined(PRE_PERIOD_INVENTORY)
	for (int i = 0; i < INVENTORYTOTALMAX; i++){
		if (i == GetInventoryCount()){
			i = INVENTORYMAX;
		}
#else	// #if defined(PRE_PERIOD_INVENTORY)
	for (int i = 0; i < GetInventoryCount(); i++){
#endif	// #if defined(PRE_PERIOD_INVENTORY)
		if (m_Inventory[i].nItemID <= 0) continue;

		InventoryList[nCount].cSlotIndex = i;
		InventoryList[nCount].Item = m_Inventory[i];
		nCount++;
	}

	cTotalCount = nCount;
}

void CDNUserItem::GetWarehouseList(TItemInfo *WarehouseList, BYTE &cTotalCount)
{
	int nCount = 0;

#if defined(PRE_PERIOD_INVENTORY)
	for (int i = 0; i < WAREHOUSETOTALMAX; i++){
		if (i == GetWarehouseCount()){
			i = WAREHOUSEMAX;
		}
#else	// #if defined(PRE_PERIOD_INVENTORY)
	for (int i = 0; i < GetWarehouseCount(); i++){
#endif	// #if defined(PRE_PERIOD_INVENTORY)
		if (m_Warehouse[i].nItemID <= 0) continue;

		WarehouseList[nCount].cSlotIndex = i;
		WarehouseList[nCount].Item = m_Warehouse[i];
		nCount++;
	}

	cTotalCount = nCount;
}

#if defined(PRE_ADD_WORLD_MSG_RED)
bool CDNUserItem::IsVaildWorldChatItem(char cChatType, INT64 biItemSerial, int &nItemID)
{	
	const TItem *pItem = GetCashInventory(biItemSerial);
	if ( !pItem ) return false;

	const TItemData *pItemData = g_pDataManager->GetItemData(pItem->nItemID);
	if( !pItemData) return false;
	if( pItemData->nType != ITEMTYPE_WORLDMSG ) return false;

	switch(cChatType)
	{
	case CHATTYPE_CHANNEL:
		if(pItemData->nTypeParam[0] != WorldChatUseItem::CashItemTypeParam::YellowBird) return false;
		break;
	case CHATTYPE_WORLD:
		if(pItemData->nTypeParam[0] != WorldChatUseItem::CashItemTypeParam::BlueBird) return false;
		break;
	case CHATTYPE_WORLD_POPMSG:
		if(pItemData->nTypeParam[0] != WorldChatUseItem::CashItemTypeParam::RedBird) return false;
		break;
	default:
		return false;
	}
	nItemID = pItem->nItemID;
	return true;
}
#endif	// #if defined(PRE_ADD_WORLD_MSG_RED)