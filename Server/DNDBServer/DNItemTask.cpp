#include "StdAfx.h"
#include "DNItemTask.h"
#include "DNConnection.h"
#include "DNSQLConnectionManager.h"
#include "DNSQLMembership.h"
#include "DNSQLWorld.h"
#include "DNExtManager.h"
#include "Util.h"
#include "Log.h"

CDNItemTask::CDNItemTask(CDNConnection* pConnection)
: CDNMessageTask(pConnection)
{

}

CDNItemTask::~CDNItemTask(void)
{
}

void CDNItemTask::OnRecvMessage(int nThreadID, int nMainCmd, int nSubCmd, char* pData)
{
	CDNSQLMembership *pMembershipDB = NULL;
	CDNSQLWorld *pWorldDB = NULL;
	int nRet = ERROR_DB;
	TQHeader *pHeader = (TQHeader*)pData;
	if( pHeader->nAccountDBID > 0)
	{
		if( nSubCmd != QUERY_SAVEITEMLOCATIONINDEX && g_pSPErrorCheckManager->bIsError(pHeader->nAccountDBID) == true )
		{
			//g_Log.Log(LogType::_ERROR, pHeader->cWorldSetID, pHeader->nAccountDBID, 0, 0, L"[SP_ERRORCHECK] Main:%d, Sub:%d\r\n", nMainCmd, nSubCmd);
			return;
		}
	}

	switch (nSubCmd)
	{
	case QUERY_ADDITEM:
		{
			TQAddMaterializedItem *pItem = (TQAddMaterializedItem*)pData;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pItem->cWorldSetID);
			if (pWorldDB){
				nRet = pWorldDB->QueryAddMaterializedItem(pItem);
				if (nRet != ERROR_NONE){
					g_Log.Log(LogType::_ERROR, pItem->cWorldSetID, pItem->nAccountDBID, pItem->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_ADDITEM:%d] Query Error Ret:%d\r\n", pItem->biCharacterDBID, pItem->cWorldSetID, nRet);

					m_pConnection->QueryResultError(pItem->nAccountDBID, nRet, nMainCmd, nSubCmd);	// µ∫Òø°º≠ π∫∞° ø°∑Ø∞™¿ª πÒæÓ≥¬¿∏¥œ ∞¡ ≤˜æÓπˆ∏∞¥Ÿ
				}
			}
			else{
				g_Log.Log(LogType::_ERROR, pItem->cWorldSetID, pItem->nAccountDBID, pItem->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_ADDITEM:%d] pWorldDB not found\r\n", pItem->biCharacterDBID, pItem->cWorldSetID);
			}			
		}
		break;

	case QUERY_USEITEM:
		{
			TQUseItem *pItem = (TQUseItem*)pData;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pItem->cWorldSetID);
			if (pWorldDB){
				nRet = pWorldDB->QueryUseItem(pItem);
				if (nRet != ERROR_NONE)
				{
					g_Log.Log(LogType::_ERROR, pItem->cWorldSetID, pItem->nAccountDBID, 0, 0, L"[QUERY_USEITEM:%d] Query Error Ret:%d\r\n", pItem->cWorldSetID, nRet);

					m_pConnection->QueryResultError(pItem->nAccountDBID, nRet, nMainCmd, nSubCmd);	// µ∫Òø°º≠ π∫∞° ø°∑Ø∞™¿ª πÒæÓ≥¬¿∏¥œ ∞¡ ≤˜æÓπˆ∏∞¥Ÿ
				}
			}
			else{
				g_Log.Log(LogType::_ERROR, pItem->cWorldSetID, pItem->nAccountDBID, 0, 0, L"[QUERY_USEITEM:%d] pWorldDB not found\r\n", pItem->cWorldSetID);
			}			
		}
		break;

	case QUERY_RESELLITEM:
		{
			TQResellItem *pItem = (TQResellItem*)pData;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pItem->cWorldSetID);
			if (pWorldDB){
				nRet = pWorldDB->QueryResellItem(pItem);
				if (nRet != ERROR_NONE){
					g_Log.Log(LogType::_ERROR, pItem->cWorldSetID, pItem->nAccountDBID, pItem->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_RESELLITEM:%d] Query Error Ret:%d\r\n", pItem->biCharacterDBID, pItem->cWorldSetID, nRet);

					m_pConnection->QueryResultError(pItem->nAccountDBID, nRet, nMainCmd, nSubCmd);	// µ∫Òø°º≠ π∫∞° ø°∑Ø∞™¿ª πÒæÓ≥¬¿∏¥œ ∞¡ ≤˜æÓπˆ∏∞¥Ÿ
				}
#if defined(PRE_FIX_74387)
				else
				{
					TAHeader Item = {0,};
					Item.nAccountDBID = pItem->nAccountDBID;
					Item.nRetCode = nRet;
					m_pConnection->AddSendData(nMainCmd, nSubCmd, (char*)&Item, sizeof(Item));
				}
#endif //#if defined(PRE_FIX_74387)
			}
			else{
				g_Log.Log(LogType::_ERROR, pItem->cWorldSetID, pItem->nAccountDBID, pItem->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_RESELLITEM:%d] pWorldDB not found\r\n", pItem->biCharacterDBID, pItem->cWorldSetID);
			}			
		}
		break;

	case QUERY_ENCHANTITEM:
		{
			TQEnchantItem *pItem = (TQEnchantItem*)pData;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pItem->cWorldSetID);
			if (pWorldDB){
				nRet = pWorldDB->QueryEnchantItem(pItem);
				if (nRet != ERROR_NONE){
					g_Log.Log(LogType::_ERROR, pItem->cWorldSetID, pItem->nAccountDBID, 0, 0, L"[QUERY_ENCHANTITEM:%d] Query Error Ret:%d\r\n", pItem->cWorldSetID, nRet);

					m_pConnection->QueryResultError(pItem->nAccountDBID, nRet, nMainCmd, nSubCmd);	// µ∫Òø°º≠ π∫∞° ø°∑Ø∞™¿ª πÒæÓ≥¬¿∏¥œ ∞¡ ≤˜æÓπˆ∏∞¥Ÿ
				}
			}
			else{
				g_Log.Log(LogType::_ERROR, pItem->cWorldSetID, pItem->nAccountDBID, 0, 0, L"[QUERY_ENCHANTITEM:%d] pWorldDB not found\r\n", pItem->cWorldSetID);
			}			
		}
		break;

	case QUERY_CHANGEITEMLOCATION:
		{
			TQChangeItemLocation *pItem = (TQChangeItemLocation*)pData;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pItem->cWorldSetID);
			if (pWorldDB){
				nRet = pWorldDB->QueryChangeItemLocation(pItem);
				if (nRet != ERROR_NONE){
					g_Log.Log(LogType::_ERROR, pItem->cWorldSetID, pItem->nAccountDBID, pItem->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_CHANGEITEMLOCATION:%d] Query Error Ret:%d\r\n", pItem->biCharacterDBID, pItem->cWorldSetID, nRet);

					m_pConnection->QueryResultError(pItem->nAccountDBID, nRet, nMainCmd, nSubCmd);	// µ∫Òø°º≠ π∫∞° ø°∑Ø∞™¿ª πÒæÓ≥¬¿∏¥œ ∞¡ ≤˜æÓπˆ∏∞¥Ÿ
				}
			}
			else{
				g_Log.Log(LogType::_ERROR, pItem->cWorldSetID, pItem->nAccountDBID, pItem->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_CHANGEITEMLOCATION:%d] pWorldDB not found\r\n", pItem->biCharacterDBID, pItem->cWorldSetID);
			}			
		}
		break;

	case QUERY_SWITCHITEMLOCATION:
		{
			TQSwitchItemLocation *pItem = (TQSwitchItemLocation*)pData;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pItem->cWorldSetID);
			if (pWorldDB){
				nRet = pWorldDB->QuerySwitchItemLocation(pItem);
				if (nRet != ERROR_NONE){
					g_Log.Log(LogType::_ERROR, pItem->cWorldSetID, pItem->nAccountDBID, pItem->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_SWITCHITEMLOCATION:%d] Query Error Ret:%d\r\n", pItem->biCharacterDBID, pItem->cWorldSetID, nRet);

					m_pConnection->QueryResultError(pItem->nAccountDBID, nRet, nMainCmd, nSubCmd);	// µ∫Òø°º≠ π∫∞° ø°∑Ø∞™¿ª πÒæÓ≥¬¿∏¥œ ∞¡ ≤˜æÓπˆ∏∞¥Ÿ
				}
			}
			else{
				g_Log.Log(LogType::_ERROR, pItem->cWorldSetID, pItem->nAccountDBID, pItem->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_SWITCHITEMLOCATION:%d] pWorldDB not found\r\n", pItem->biCharacterDBID, pItem->cWorldSetID);
			}			
		}
		break;

	case QUERY_MODITEMDURABILITY:
		{
			TQModItemDurability *pItem = (TQModItemDurability*)pData;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pItem->cWorldSetID);
			if (pWorldDB){
				if( pItem->biCurrentCoin > 0 && pItem->biPickUpCoin > 0 )
				{
					INT64 biTotalCoin = 0;
					nRet = pWorldDB->QueryMidtermCoin(pItem->biCharacterDBID, pItem->biPickUpCoin, pItem->iChannelID, pItem->iMapID, biTotalCoin);
					if( nRet != ERROR_NONE || biTotalCoin != pItem->biCurrentCoin )
					{
						g_Log.Log(LogType::_ERROR, pItem->cWorldSetID, pItem->nAccountDBID, pItem->biCharacterDBID, 0, L"[CDBID:%lld] [QueryMidtermCoin:%d] Query Error Ret:%d DBCoint:%I64d ServerCoin:%I64d\r\n", pItem->biCharacterDBID, pItem->cWorldSetID, nRet, biTotalCoin, pItem->biCurrentCoin);
						nRet = ERROR_DB;
						m_pConnection->QueryResultError(pItem->nAccountDBID, nRet, nMainCmd, nSubCmd);	// µ∫Òø°º≠ π∫∞° ø°∑Ø∞™¿ª πÒæÓ≥¬¿∏¥œ ∞¡ ≤˜æÓπˆ∏∞¥Ÿ
						break;
					}
				}

				nRet = pWorldDB->QueryModItemDurability(pItem);
				if (nRet != ERROR_NONE){
					g_Log.Log(LogType::_ERROR, pItem->cWorldSetID, pItem->nAccountDBID, pItem->biCharacterDBID, 0, L"[QUERY_MODITEMDURABILITY:%d] Query Error Ret:%d\r\n", pItem->cWorldSetID, nRet);

					m_pConnection->QueryResultError(pItem->nAccountDBID, nRet, nMainCmd, nSubCmd);	// µ∫Òø°º≠ π∫∞° ø°∑Ø∞™¿ª πÒæÓ≥¬¿∏¥œ ∞¡ ≤˜æÓπˆ∏∞¥Ÿ
				}
			}
			else{
				g_Log.Log(LogType::_ERROR, pItem->cWorldSetID, pItem->nAccountDBID, pItem->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_MODITEMDURABILITY:%d] pWorldDB not found\r\n", pItem->cWorldSetID);
			}			
		}
		break;

	case QUERY_MODITEM:
		{
			TQModMaterializedItem *pItem = (TQModMaterializedItem*)pData;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pItem->cWorldSetID);
			if (pWorldDB){
				nRet = pWorldDB->QueryModMaterializedItem(pItem);
				if (nRet != ERROR_NONE){
					g_Log.Log(LogType::_ERROR, pItem->cWorldSetID, pItem->nAccountDBID, 0, 0, L"[QUERY_MODITEM:%d] Query Error Ret:%d\r\n", pItem->cWorldSetID, nRet);

					m_pConnection->QueryResultError(pItem->nAccountDBID, nRet, nMainCmd, nSubCmd);	// µ∫Òø°º≠ π∫∞° ø°∑Ø∞™¿ª πÒæÓ≥¬¿∏¥œ ∞¡ ≤˜æÓπˆ∏∞¥Ÿ
				}
			}
			else{
				g_Log.Log(LogType::_ERROR, pItem->cWorldSetID, pItem->nAccountDBID, 0, 0, L"[QUERY_MODITEM:%d] pWorldDB not found\r\n", pItem->cWorldSetID);
			}			
		}
		break;

	case QUERY_EXCHANGEPROPERTY:
		{
			TQExchangeProperty *pItem = (TQExchangeProperty*)pData;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pItem->cWorldSetID);
			if (pWorldDB){
				nRet = pWorldDB->QueryExchangeProperty(pItem);
				if (nRet != ERROR_NONE){
					g_Log.Log(LogType::_ERROR, pItem->cWorldSetID, pItem->nAccountDBID, pItem->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_EXCHANGEPROPERTY:%d] Query Error Ret:%d\r\n", pItem->biCharacterDBID, pItem->cWorldSetID, nRet);

					m_pConnection->QueryResultError(pItem->nAccountDBID, nRet, nMainCmd, nSubCmd);	// µ∫Òø°º≠ π∫∞° ø°∑Ø∞™¿ª πÒæÓ≥¬¿∏¥œ ∞¡ ≤˜æÓπˆ∏∞¥Ÿ
					m_pConnection->QueryResultError(pItem->uiTargetAccountDBID, nRet, nMainCmd, nSubCmd);	// µ∫Òø°º≠ π∫∞° ø°∑Ø∞™¿ª πÒæÓ≥¬¿∏¥œ ∞¡ ≤˜æÓπˆ∏∞¥Ÿ
				}
			}
			else{
				g_Log.Log(LogType::_ERROR, pItem->cWorldSetID, pItem->nAccountDBID, pItem->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_EXCHANGEPROPERTY:%d] pWorldDB not found\r\n", pItem->biCharacterDBID, pItem->cWorldSetID);
			}			
		}
		break;

	case QUERY_UPSEQUIP:
		{
			TQUpsEquip *pItem = (TQUpsEquip*)pData;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pItem->cWorldSetID);
			if (pWorldDB){
				nRet = pWorldDB->QueryUpsEquipment(pItem->biCharacterDBID, pItem->cItemLocation, pItem->cEquipSlotIndex, pItem->biSerial);
				if (nRet != ERROR_NONE){
					g_Log.Log(LogType::_ERROR, pItem->cWorldSetID, pItem->nAccountDBID, pItem->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_UPSEQUIP:%d] Query Error Ret:%d\r\n", pItem->biCharacterDBID, pItem->cWorldSetID, nRet);

					m_pConnection->QueryResultError(pItem->nAccountDBID, nRet, nMainCmd, nSubCmd);	// µ∫Òø°º≠ π∫∞° ø°∑Ø∞™¿ª πÒæÓ≥¬¿∏¥œ ∞¡ ≤˜æÓπˆ∏∞¥Ÿ
				}
			}
			else{
				g_Log.Log(LogType::_ERROR, pItem->cWorldSetID, pItem->nAccountDBID, pItem->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_UPSEQUIP:%d] pWorldDB not found\r\n", pItem->biCharacterDBID, pItem->cWorldSetID);
			}
		}
		break;

	case QUERY_TAKECASHEQUIPMENTOFF:
		{
			TQTakeCashEquipmentOff *pItem = (TQTakeCashEquipmentOff*)pData;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pItem->cWorldSetID);
			if (pWorldDB){
				nRet = pWorldDB->QueryTakeCashEquipmentOff(pItem);
				if (nRet != ERROR_NONE){
					g_Log.Log(LogType::_ERROR, pItem->cWorldSetID, pItem->nAccountDBID, pItem->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_TAKECASHEQUIPMENTOFF:%d] Query Error Ret:%d\r\n", pItem->biCharacterDBID, pItem->cWorldSetID, nRet);

					m_pConnection->QueryResultError(pItem->nAccountDBID, nRet, nMainCmd, nSubCmd);	// µ∫Òø°º≠ π∫∞° ø°∑Ø∞™¿ª πÒæÓ≥¬¿∏¥œ ∞¡ ≤˜æÓπˆ∏∞¥Ÿ
				}
			}
			else{
				g_Log.Log(LogType::_ERROR, pItem->cWorldSetID, pItem->nAccountDBID, pItem->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_TAKECASHEQUIPMENTOFF:%d] pWorldDB not found\r\n", pItem->biCharacterDBID, pItem->cWorldSetID);
			}
		}
		break;

	case QUERY_PUTONEPIECEON:
		{
			TQPutOnepieceOn *pItem = (TQPutOnepieceOn*)pData;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pItem->cWorldSetID);
			if (pWorldDB){
				nRet = pWorldDB->QueryPutOnepieceOn(pItem);
				if (nRet != ERROR_NONE){
					g_Log.Log(LogType::_ERROR, pItem->cWorldSetID, pItem->nAccountDBID, pItem->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_PUTONEPIECEON:%d] Query Error Ret:%d\r\n", pItem->biCharacterDBID, pItem->cWorldSetID, nRet);

					m_pConnection->QueryResultError(pItem->nAccountDBID, nRet, nMainCmd, nSubCmd);	// µ∫Òø°º≠ π∫∞° ø°∑Ø∞™¿ª πÒæÓ≥¬¿∏¥œ ∞¡ ≤˜æÓπˆ∏∞¥Ÿ
				}
			}
			else{
				g_Log.Log(LogType::_ERROR, pItem->cWorldSetID, pItem->nAccountDBID, pItem->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_PUTONEPIECEON:%d] pWorldDB not found\r\n", pItem->biCharacterDBID, pItem->cWorldSetID);
			}
		}
		break;

	case QUERY_GETPAGEMATERIALIZEDITEM:
		{
			TQGetPageMaterializedItem *pItem = (TQGetPageMaterializedItem*)pData;

			TAGetPageMaterializedItem Item;
			memset(&Item, 0, sizeof(Item));

			Item.nAccountDBID = pItem->nAccountDBID;
			Item.nRetCode = ERROR_DB;
			Item.wPageNumber = pItem->nPageNumber;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pItem->cWorldSetID);
			if (pWorldDB){
				if (pItem->nPageNumber == 1)
					pWorldDB->QueryGetCountMaterializedItem(pItem->biCharacterDBID, DBDNWorldDef::ItemLocation::CashInventory, Item.nTotalListCount);

				Item.nRetCode = pWorldDB->QueryGetPageMaterializedItem(pItem, &Item);

				if (Item.nRetCode != ERROR_NONE)
					g_Log.Log(LogType::_ERROR, pItem->cWorldSetID, pItem->nAccountDBID, pItem->biCharacterDBID, 0, L"[ADBID:%u] [QUERY_GETPAGEMATERIALIZEDITEM:%d] Result:%d\r\n", pItem->nAccountDBID, pItem->cWorldSetID, Item.nRetCode);
			}
			else{
				g_Log.Log(LogType::_ERROR, pItem->cWorldSetID, pItem->nAccountDBID, pItem->biCharacterDBID, 0, L"[ADBID:%u] [QUERY_GETPAGEMATERIALIZEDITEM:%d] pWorldDB not found\r\n", pItem->nAccountDBID, pItem->cWorldSetID);
			}

			m_pConnection->AddSendData(nMainCmd, nSubCmd, (char*)&Item, sizeof(Item));
		}
		break;

	case QUERY_GETPAGEVEHICLE:
		{
			TQGetPageVehicle *pItem = (TQGetPageVehicle*)pData;

			TAGetPageVehicle Item;
			memset(&Item, 0, sizeof(Item));

			Item.nAccountDBID = pItem->nAccountDBID;
			Item.nRetCode = ERROR_DB;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pItem->cWorldSetID);
			if (pWorldDB)
			{
				if (pItem->nPageNumber == 1)
				{
					pWorldDB->QueryGetCountMaterializedItem(pItem->biCharacterDBID, DBDNWorldDef::ItemLocation::PetInventory, Item.nTotalListCount);
				}

				std::vector<DBPacket::TVehicleInfo> VecItemList;
				VecItemList.clear();
				Item.nRetCode = pWorldDB->QueryGetPagePet(pItem->biCharacterDBID, pItem->nPageNumber, Item.nTotalListCount * 4, DBDNWorldDef::PetType::Pet, false, VecItemList);

				if (!VecItemList.empty()){
					TVehicleCompact AddVehicle;
					memset(&AddVehicle, 0, sizeof(AddVehicle));

					std::map<INT64, TVehicleCompact> MapVehicleList;
					MapVehicleList.clear();

					for (int i = 0; i < (int)VecItemList.size(); i++){
						if (VecItemList[i].nPetIndex == 0)
							memset(&AddVehicle, 0, sizeof(AddVehicle));

						if (VecItemList[i].nPetIndex == Vehicle::Slot::Sundries)
						{
							AddVehicle.dwPartsColor1 = VecItemList[i].dwPartsColor1;
							AddVehicle.dwPartsColor2 = VecItemList[i].dwPartsColor2;
							AddVehicle.nExp = VecItemList[i].nExp;
							_wcscpy(AddVehicle.wszNickName, NAMELENMAX, VecItemList[i].wszNickName, NAMELENMAX);
							AddVehicle.nSkillID1 = VecItemList[i].nSkillID1;
							AddVehicle.nSkillID2 = VecItemList[i].nSkillID2;
							AddVehicle.bSkillSlot = VecItemList[i].bSkillSlot;
							AddVehicle.tLastHungerModifyDate = VecItemList[i].tLastHungerModifyDate;
							AddVehicle.nCurrentSatiety = VecItemList[i].nCurrentSatiety;
						}
						else{
							AddVehicle.Vehicle[VecItemList[i].nPetIndex].nItemID = VecItemList[i].nItemID;
							AddVehicle.Vehicle[VecItemList[i].nPetIndex].nSerial = VecItemList[i].nSerial;
							AddVehicle.Vehicle[VecItemList[i].nPetIndex].wCount = VecItemList[i].wCount;
							AddVehicle.Vehicle[VecItemList[i].nPetIndex].bEternity = VecItemList[i].bEternity;
							AddVehicle.Vehicle[VecItemList[i].nPetIndex].tExpireDate = VecItemList[i].tExpireDate;
						}

						MapVehicleList[AddVehicle.Vehicle[Vehicle::Slot::Body].nSerial] = AddVehicle;
					}

					Item.cCount = 0;
					std::map<INT64, TVehicleCompact>::iterator iter;
					for (iter = MapVehicleList.begin(); iter != MapVehicleList.end(); ++iter){
						Item.VehicleItem[Item.cCount] = iter->second;
						Item.cCount++;

						if (Item.cCount >= VEHICLEINVENTORYPAGEMAX){
							Item.wPageNumber++;
							m_pConnection->AddSendData(nMainCmd, nSubCmd, (char*)&Item, sizeof(Item));

							memset(Item.VehicleItem, 0, sizeof(Item.VehicleItem));
							Item.cCount = 0;
						}
					}

					if (Item.cCount > 0){
						Item.wPageNumber++;
						m_pConnection->AddSendData(nMainCmd, nSubCmd, (char*)&Item, sizeof(Item));
					}
				}
				else
				{
					Item.wPageNumber = 1;
					m_pConnection->AddSendData(nMainCmd, nSubCmd, (char*)&Item, sizeof(Item));
				}

				if (Item.nRetCode != ERROR_NONE)
					g_Log.Log(LogType::_ERROR, pItem->cWorldSetID, pItem->nAccountDBID, pItem->biCharacterDBID, 0, L"[ADBID:%u] [QUERY_GETPAGEVEHICLE:%d] Result:%d\r\n", pItem->nAccountDBID, pItem->cWorldSetID, Item.nRetCode);
			}
			else{
				g_Log.Log(LogType::_ERROR, pItem->cWorldSetID, pItem->nAccountDBID, pItem->biCharacterDBID, 0, L"[ADBID:%u] [QUERY_GETPAGEVEHICLE:%d] pWorldDB not found\r\n", pItem->nAccountDBID, pItem->cWorldSetID);
			}

			// m_pConnection->AddSendData(nMainCmd, nSubCmd, (char*)&Item, sizeof(Item));
		}
		break;

	case QUERY_MODPETSKINCOLOR:
		{
			TQModPetSkinColor *pItem = (TQModPetSkinColor*)pData;

			TAModPetSkinColor Item;
			memset(&Item, 0, sizeof(Item));

			Item.nAccountDBID = pItem->nAccountDBID;
			Item.nRetCode = ERROR_DB;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pItem->cWorldSetID);
			if (pWorldDB)
			{
				Item.nRetCode = pWorldDB->QueryModPetSkinColor(pItem->biBodySerial, pItem->dwSkinColor, pItem->dwSkinColor2);
				if (Item.nRetCode != ERROR_NONE)
					g_Log.Log(LogType::_ERROR, pItem->cWorldSetID, pItem->nAccountDBID, 0, 0, L"[ADBID:%u] [QUERY_MODPETSKINCOLOR:%d] Result:%d\r\n", pItem->nAccountDBID, pItem->cWorldSetID, Item.nRetCode);
			}
			else{
				g_Log.Log(LogType::_ERROR, pItem->cWorldSetID, pItem->nAccountDBID, 0, 0, L"[ADBID:%u] [QUERY_MODPETSKINCOLOR:%d] pWorldDB not found\r\n", pItem->nAccountDBID, pItem->cWorldSetID);
			}

			m_pConnection->AddSendData(nMainCmd, nSubCmd, (char*)&Item, sizeof(Item));
		}
		break;

	case QUERY_DELPETEQUIPMENT:
		{
			TQDelPetEquipment *pItem = (TQDelPetEquipment*)pData;

			TADelPetEquipment Item;
			memset(&Item, 0, sizeof(Item));

			Item.nAccountDBID = pItem->nAccountDBID;
			Item.nRetCode = ERROR_DB;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pItem->cWorldSetID);
			if (pWorldDB){
				Item.nRetCode = pWorldDB->QueryDelPetEquipment(pItem->biBodySerial, pItem->biPartsSerial, pItem->wszIp);
				if (Item.nRetCode != ERROR_NONE)
					g_Log.Log(LogType::_ERROR, pItem->cWorldSetID, pItem->nAccountDBID, 0, 0, L"[ADBID:%u] [QUERY_DELPETEQUIPMENT:%d] Result:%d\r\n", pItem->nAccountDBID, pItem->cWorldSetID, Item.nRetCode);
			}
			else{
				g_Log.Log(LogType::_ERROR, pItem->cWorldSetID, pItem->nAccountDBID, 0, 0, L"[ADBID:%u] [QUERY_DELPETEQUIPMENT:%d] pWorldDB not found\r\n", pItem->nAccountDBID, pItem->cWorldSetID);
			}

			m_pConnection->AddSendData(nMainCmd, nSubCmd, (char*)&Item, sizeof(Item));
		}
		break;

	case QUERY_CHANGEPETNAME:
		{
			TQChangePetName *pItem = reinterpret_cast<TQChangePetName *>(pData);

			TAChangePetName Item;
			memset(&Item, 0, sizeof(Item));

			Item.nRetCode = ERROR_DB;
			Item.nAccountDBID = pItem->nAccountDBID;
			Item.itemSerial = pItem->itemSerial;
			Item.petSerial = pItem->petSerial;
			_wcscpy(Item.name, _countof(Item.name), pItem->name, (int)wcslen(pItem->name));

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pItem->cWorldSetID);
			if( pWorldDB )
			{
				Item.nRetCode = pWorldDB->QueryChangePetName(pItem->petSerial, pItem->name);

				if (Item.nRetCode != ERROR_NONE)
					g_Log.Log(LogType::_ERROR, pItem->cWorldSetID, pItem->nAccountDBID, 0, 0, L"[QUERY_CHANGEPETNAME:%d] (Ret:%d)\r\n", pItem->cWorldSetID, Item.nRetCode);
			}
			else
			{
				g_Log.Log(LogType::_ERROR, pItem->cWorldSetID, pItem->nAccountDBID, 0, 0, L"[QUERY_CHANGEPETNAME:%d] pWorldDB not found\r\n", pItem->cWorldSetID);
			}

			m_pConnection->AddSendData(nMainCmd, nSubCmd, reinterpret_cast<char*>(&Item), sizeof(TAChangePetName));
		}
		break;

	case QUERY_ITEMEXPIREBYCHEAT:
		{
			TQItemExpire* pItem = reinterpret_cast<TQItemExpire*>(pData);
			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pItem->cWorldSetID);
			if( pWorldDB )
			{
				TAItemExpire packet;
				memset(&packet, 0, sizeof(packet));
				packet.nAccountDBID = pItem->nAccountDBID;
				packet.biSerial = pItem->biSerial;
				packet.nRetCode = pWorldDB->QueryItemExpireByCheat(pItem);
				m_pConnection->AddSendData(nMainCmd, nSubCmd, reinterpret_cast<char*>(&packet), sizeof(TAItemExpire));
			}
			else
			{
				g_Log.Log(LogType::_ERROR, pItem->cWorldSetID, pItem->nAccountDBID, 0, 0, L"[QUERY_ITEMEXPIREBYCHEAT:%d] pWorldDB not found\r\n", pItem->cWorldSetID);
			}
		}
		break;		

	case QUERY_MODITEMEXPIREDATE:
		{
			TQModItemExpireDate* pItem = reinterpret_cast<TQModItemExpireDate*>(pData);

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pItem->cWorldSetID);
			if( pWorldDB )
			{
				TAModItemExpireDate Item;
				memset(&Item, 0, sizeof(Item));

				Item.nAccountDBID = pItem->nAccountDBID;
				Item.biItemSerial = pItem->biItemSerial;
				Item.nMin = pItem->nMin;

				Item.nRetCode = pWorldDB->QueryModItemExpireDate(pItem);

				m_pConnection->AddSendData(nMainCmd, nSubCmd, reinterpret_cast<char*>(&Item), sizeof(TAModItemExpireDate));

				if( Item.nRetCode == ERROR_NONE )
				{
					// ±‚∞£ ø¨¿Â æ∆¿Ã≈€ ¡¶∞≈
					TQUseItem UseItem;
					memset( &UseItem, 0, sizeof(UseItem) );

					UseItem.cThreadID		= pItem->cThreadID;
					UseItem.cWorldSetID		= pItem->cWorldSetID;
					UseItem.nAccountDBID	= pItem->nAccountDBID;
					UseItem.Code			= DBDNWorldDef::UseItem::Use;
					UseItem.biItemSerial	= pItem->biExpireDateItemSerial;
					UseItem.nUsedItemCount	= 1;
					UseItem.iMapID			= pItem->iMapID;
					_wcscpy( UseItem.wszIP, _countof(UseItem.wszIP), pItem->wszIP, (int)wcslen(pItem->wszIP) );

					OnRecvMessage( pItem->cThreadID, MAINCMD_ITEM, QUERY_USEITEM, reinterpret_cast<char*>(&UseItem) );
				}
			}
			else
			{
				g_Log.Log(LogType::_ERROR, pItem->cWorldSetID, pItem->nAccountDBID, pItem->biCharacterDBID, 0, L"[QUERY_MODITEMEXPIREDATE:%d] pWorldDB not found\r\n", pItem->cWorldSetID);
			}
		}
		break;

	case QUERY_MODPETEXP:
		{
			TQModPetExp* pItem = reinterpret_cast<TQModPetExp*>(pData);

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pItem->cWorldSetID);
			if( pWorldDB )
			{
				TAModPetExp A;
				memset(&A, 0, sizeof(A));
				A.nAccountDBID = pItem->nAccountDBID;
				A.nRetCode = pWorldDB->QueryModPetExp(pItem);
				m_pConnection->AddSendData(nMainCmd, nSubCmd, reinterpret_cast<char*>(&A), sizeof(TAModPetExp));
			}
		}
		break;

	case QUERY_MODPETSKILL :			// ∆Í Ω∫≈≥∫Ø∞Ê
		{
			TQModPetSkill* pItem = reinterpret_cast<TQModPetSkill*>(pData);
			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pItem->cWorldSetID);
			if( pWorldDB )
			{
				TAHeader packet;
				memset(&packet, 0, sizeof(packet));
				packet.nAccountDBID = pItem->nAccountDBID;
				packet.nRetCode = pWorldDB->QueryModPetSkill(pItem);
				m_pConnection->AddSendData(nMainCmd, nSubCmd, reinterpret_cast<char*>(&packet), sizeof(TAHeader));
			}
			else
			{
				g_Log.Log(LogType::_ERROR, pItem->cWorldSetID, pItem->nAccountDBID, 0, 0, L"[QUERY_MODPETSKILL:%d] pWorldDB not found\r\n", pItem->cWorldSetID);
			}
		}
		break;

	case QUERY_MODPETSKILLEXPAND :	// ∆Í Ω∫≈≥ΩΩ∑‘ »Æ¿Â
		{
			TQModPetSkillExpand* pItem = reinterpret_cast<TQModPetSkillExpand*>(pData);
			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pItem->cWorldSetID);
			if( pWorldDB )
			{
				TAHeader packet;
				memset(&packet, 0, sizeof(packet));
				packet.nAccountDBID = pItem->nAccountDBID;
				packet.nRetCode = pWorldDB->QueryModPetSkillExpand(pItem->biItemSerial);
				m_pConnection->AddSendData(nMainCmd, nSubCmd, reinterpret_cast<char*>(&packet), sizeof(TAHeader));
			}
			else
			{
				g_Log.Log(LogType::_ERROR, pItem->cWorldSetID, pItem->nAccountDBID, 0, 0, L"[QUERY_MODPETSKILLEXPAND:%d] pWorldDB not found\r\n", pItem->cWorldSetID);
			}
		}
		break;

	case QUERY_RECOVERMISSINGITEM:
		{
			TQRecoverMissingItem *pItem = (TQRecoverMissingItem*)pData;

			TARecoverMissingItem Item;
			memset(&Item, 0, sizeof(TARecoverMissingItem));

			Item.nAccountDBID = pItem->nAccountDBID;
			Item.nRetCode = ERROR_DB;
			Item.cItemLocationCode = pItem->cItemLocationCode;
			Item.cSlotIndex = pItem->cSlotIndex;
			Item.RecoverItem = pItem->RecoverItem;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pItem->cWorldSetID);
			if (pWorldDB){
				Item.nRetCode = pWorldDB->QueryRecoverMissingItem(pItem->biCharacterDBID, pItem->RecoverItem.nSerial, pItem->cItemLocationCode, pItem->cSlotIndex);
				if (Item.nRetCode != ERROR_NONE)
					g_Log.Log(LogType::_ERROR, pItem->cWorldSetID, pItem->nAccountDBID, pItem->biCharacterDBID, 0, L"[ADBID:%u] [QUERY_RECOVERMISSINGITEM:%d] Location:%d Slot:%d ItemID:%d Result:%d\r\n", pItem->nAccountDBID, pItem->cWorldSetID, Item.cItemLocationCode, Item.cSlotIndex, Item.RecoverItem.nItemID, Item.nRetCode);
			}
			else{
				g_Log.Log(LogType::_ERROR, pItem->cWorldSetID, pItem->nAccountDBID, pItem->biCharacterDBID, 0, L"[ADBID:%u] [QUERY_RECOVERMISSINGITEM] pWorldDB not found\r\n", pItem->nAccountDBID, pItem->cWorldSetID);
			}

			m_pConnection->AddSendData(nMainCmd, nSubCmd, (char*)&Item, sizeof(Item));
		}
		break;

	case QUERY_ADDEFFECTITEMS:
		{
			TQAddEffectItems *pItem = (TQAddEffectItems*)pData;

			int nItemType = g_pExtManager->GetItemMainType(pItem->iItemID);
			switch (nItemType)
			{
			case ITEMTYPE_UNION_MEMBERSHIP:
			case ITEMTYPE_PERIOD_APPELLATION:
			case ITEMTYPE_EXPAND_SKILLPAGE:
			case ITEMTYPE_SOURCE:
			case ITEMTYPE_GESTURE:
#if defined( PRE_ADD_NAMEDITEM_SYSTEM )
			case ITEMTYPE_GLOBAL_PARTY_BUFF:
#endif
#if defined( PRE_ADD_BESTFRIEND )
			case ITEMTYPE_BESTFRIENDBUFFITEM:
#endif
			case ITEMTYPE_INVENTORY_SLOT:
			case ITEMTYPE_INVENTORY_SLOT_EX:
			case ITEMTYPE_WAREHOUSE_SLOT:
			case ITEMTYPE_WAREHOUSE_SLOT_EX:
#if defined(PRE_PERIOD_INVENTORY)
			case ITEMTYPE_PERIOD_INVENTORY:
			case ITEMTYPE_PERIOD_WAREHOUSE:
#endif	// #if defined(PRE_PERIOD_INVENTORY)
			case ITEMTYPE_PERIOD_PLATE_EX:
#if defined( PRE_ADD_TOTAL_LEVEL_SKILL )
			case ITEMTYPE_TOTALSKILLLEVEL_SLOT:
#endif
#if defined( PRE_ADD_NEWCOMEBACK )
			case ITEMTYPE_COMEBACK_EFFECTITEM:
#endif
				{
					pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pItem->cWorldSetID);
					if (pWorldDB)
					{
						nRet  = pWorldDB->QueryAddEffectItems(pItem);
						if (nRet  != ERROR_NONE)
						{
							g_Log.Log(LogType::_ERROR, pItem->cWorldSetID, pItem->nAccountDBID, pItem->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_ADDEFFECTITEMS:%d] Query Error Ret:%d\r\n", pItem->biCharacterDBID, pItem->cWorldSetID, nRet);
							m_pConnection->QueryResultError(pItem->nAccountDBID, nRet, nMainCmd, nSubCmd);
						}
					}
					else
					{
						g_Log.Log(LogType::_ERROR, pItem->cWorldSetID, pItem->nAccountDBID, pItem->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_ADDEFFECTITEMS:%d] pWorldDB not found\r\n", pItem->biCharacterDBID, pItem->cWorldSetID);
					}
				}
				break;

			default:
				{
					m_pConnection->QueryResultError(pItem->nAccountDBID, nRet, nMainCmd, nSubCmd);
				}
				break;
			}
			break;

		}

	case QUERY_MODADDITIVEITEM:
		{
			TQModAdditiveItem* pItem = (TQModAdditiveItem*)pData;

			TAModAdditiveItem Item;
			memset(&Item, 0, sizeof(TAModAdditiveItem));

			Item.nAccountDBID = pItem->nAccountDBID;
			Item.nRetCode = ERROR_DB;
			Item.biItemSerial = pItem->biItemSerial;
			Item.nAdditiveItemID = pItem->nAdditiveItemID;
			Item.biAdditiveSerial = pItem->biAdditiveSerial;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pItem->cWorldSetID);
			if (pWorldDB){
				Item.nRetCode = pWorldDB->QueryModAdditiveItem(pItem, &Item);
				if (Item.nRetCode != ERROR_NONE)
					g_Log.Log(LogType::_ERROR, pItem->cWorldSetID, pItem->nAccountDBID, pItem->biCharacterDBID, 0, L"[ADBID:%u] [QUERY_MODADDITIVEITEM:%d] Result:%d\r\n", pItem->nAccountDBID, pItem->cWorldSetID, Item.nRetCode);
			}
			else{
				g_Log.Log(LogType::_ERROR, pItem->cWorldSetID, pItem->nAccountDBID, pItem->biCharacterDBID, 0, L"[ADBID:%u] [QUERY_MODADDITIVEITEM] WorldDB not found\r\n", pItem->nAccountDBID, pItem->cWorldSetID);
			}

			m_pConnection->AddSendData(nMainCmd, nSubCmd, (char*)&Item, sizeof(Item));

			if (Item.nRetCode == ERROR_NONE)
			{
				// µ¿⁄¿Œ æ∆¿Ã≈€ ªË¡¶
				TQUseItem UseItem;
				memset( &UseItem, 0, sizeof(UseItem) );

				UseItem.cThreadID		= pItem->cThreadID;
				UseItem.cWorldSetID		= pItem->cWorldSetID;
				UseItem.nAccountDBID	= pItem->nAccountDBID;
				UseItem.Code			= DBDNWorldDef::UseItem::Use;
				UseItem.biItemSerial	= pItem->biAdditiveSerial;
				UseItem.nUsedItemCount	= 1;
				UseItem.iMapID			= pItem->nMapID;
				_wcscpy( UseItem.wszIP, _countof(UseItem.wszIP), pItem->wszIP, (int)wcslen(pItem->wszIP) );

				OnRecvMessage( pItem->cThreadID, MAINCMD_ITEM, QUERY_USEITEM, reinterpret_cast<char*>(&UseItem) );

				// «’º∫±‚ ªË¡¶
				memset( &UseItem, 0, sizeof(UseItem) );
				UseItem.cThreadID		= pItem->cThreadID;
				UseItem.cWorldSetID		= pItem->cWorldSetID;
				UseItem.nAccountDBID	= pItem->nAccountDBID;
				UseItem.Code			= DBDNWorldDef::UseItem::Use;
				UseItem.biItemSerial	= pItem->biMixItemSerial;
				UseItem.nUsedItemCount	= 1;
				UseItem.iMapID			= pItem->nMapID;
				_wcscpy( UseItem.wszIP, _countof(UseItem.wszIP), pItem->wszIP, (int)wcslen(pItem->wszIP));

				OnRecvMessage( pItem->cThreadID, MAINCMD_ITEM, QUERY_USEITEM, reinterpret_cast<char*>(&UseItem) );
			}

			break;
		}

#if defined (PRE_ADD_COSRANDMIX)
	case QUERY_MODRANDOMITEM:
		{
			TQModRandomItem *pItem = (TQModRandomItem*)pData;

			TAModRandomItem Item;
			memset(&Item, 0, sizeof(TAModRandomItem));

			Item.nAccountDBID = pItem->nAccountDBID;
			Item.biStuffSerialA = pItem->biStuffSerialA;
			Item.biStuffSerialB = pItem->biStuffSerialB;
			Item.biMixItemSerial = pItem->biMixItemSerial;
			Item.biFee = pItem->biFee;
			Item.RandomItem = pItem->RandomItem;

			Item.nRetCode = ERROR_DB;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pItem->cWorldSetID);
			if (pWorldDB){
				Item.nRetCode = pWorldDB->QueryModRandomItem(pItem, &Item);
				if (Item.nRetCode != ERROR_NONE)
					g_Log.Log(LogType::_ERROR, pItem->cWorldSetID, pItem->nAccountDBID, pItem->biCharacterDBID, 0, L"[ADBID:%u] [QUERY_MODRANDOMITEM:%d] Result:%d\r\n", pItem->nAccountDBID, pItem->cWorldSetID, Item.nRetCode);
			}
			else{
				g_Log.Log(LogType::_ERROR, pItem->cWorldSetID, pItem->nAccountDBID, pItem->biCharacterDBID, 0, L"[ADBID:%u] [QUERY_MODRANDOMITEM] WorldDB not found\r\n", pItem->nAccountDBID, pItem->cWorldSetID);
			}

			m_pConnection->AddSendData(nMainCmd, nSubCmd, (char*)&Item, sizeof(Item));
		}
		break;
#endif

	case QUERY_SAVEITEMLOCATIONINDEX:
		{
			TQSaveItemLocationIndex *pItem = (TQSaveItemLocationIndex*)pData;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pItem->cWorldSetID);
			if (pWorldDB)
				pWorldDB->QuerySaveItemLocationIndex(pItem);
		}
		break;

	case QUERY_GETLIST_REPURCHASEITEM:
		{
			TQGetListRepurchaseItem* pItem = reinterpret_cast<TQGetListRepurchaseItem*>(pData);
			TAGetListRepurchaseItem Item;
			memset( &Item, 0, sizeof(Item) );

			Item.nAccountDBID = pItem->nAccountDBID;
			Item.nRetCode		= ERROR_DB;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pItem->cWorldSetID);
			if( pWorldDB )
			{
				Item.nRetCode = pWorldDB->QueryGetListRepurchaseItem( pItem, &Item );
				if( Item.nRetCode != ERROR_NONE)
					g_Log.Log(LogType::_ERROR, pItem->cWorldSetID, pItem->nAccountDBID, pItem->biCharacterDBID, 0, L"[ADBID:%u] [QUERY_GETLIST_REPURCHASEITEM:%d]Result:%d\r\n", pItem->nAccountDBID, pItem->cWorldSetID, Item.nRetCode );
			}
			else
			{
				g_Log.Log(LogType::_ERROR, pItem->cWorldSetID, pItem->nAccountDBID, pItem->biCharacterDBID, 0, L"[ADBID:%u] [QUERY_GETLIST_REPURCHASEITEM:%d] pWorldDB not found\r\n", pItem->nAccountDBID, pItem->cWorldSetID);
			}

			int iSize = sizeof(Item)-sizeof(Item.ItemList)+(sizeof(Item.ItemList[0])*Item.cCount);
			m_pConnection->AddSendData( nMainCmd, nSubCmd, reinterpret_cast<char*>(&Item), iSize );
		}
		break;

	case QUERY_DELEXPIREITEM:
		{
			TQDelExpiritem* pItem = reinterpret_cast<TQDelExpiritem*>(pData);
			TADelExpiritem Item;
			memset( &Item, 0, sizeof(Item) );

			std::list<INT64> DelExpireitemList;
			DelExpireitemList.clear();

			Item.nAccountDBID = pItem->nAccountDBID;
			Item.nRetCode		= ERROR_DB;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pItem->cWorldSetID);
			if( pWorldDB )
			{
				Item.nRetCode = pWorldDB->QueryDeleteExpireItem( pItem->biCharacterDBID, DelExpireitemList );
				if( Item.nRetCode != ERROR_NONE)
					g_Log.Log(LogType::_ERROR, pItem->cWorldSetID, pItem->nAccountDBID, pItem->biCharacterDBID, 0, L"[ADBID:%u] [QUERY_DELEXPIREITEM:%d]Result:%d\r\n", pItem->nAccountDBID, pItem->cWorldSetID, Item.nRetCode );
			}
			else
			{
				g_Log.Log(LogType::_ERROR, pItem->cWorldSetID, pItem->nAccountDBID, pItem->biCharacterDBID, 0, L"[ADBID:%u] [QUERY_DELEXPIREITEM:%d] pWorldDB not found\r\n", pItem->nAccountDBID, pItem->cWorldSetID);
			}

			bool bSend = false;
			for( std::list<INT64>::iterator itor = DelExpireitemList.begin(); itor != DelExpireitemList.end(); itor++)
			{
				Item.biItemSerial[Item.cCount] = *itor;
				Item.cCount++;
				if(Item.cCount >= MAXDELEXPIREITEM)
				{				
					m_pConnection->AddSendData( nMainCmd, nSubCmd, reinterpret_cast<char*>(&Item), sizeof(Item) );
					Item.cCount = 0;
					memset( &Item.biItemSerial, 0, sizeof(Item.biItemSerial) );
					bSend = false;
				}
				else
				{				
					bSend = true;
				}
			}		
			if(bSend)
			{
				int iSize = sizeof(Item)-sizeof(Item.biItemSerial)+(sizeof(Item.biItemSerial[0])*Item.cCount);
				m_pConnection->AddSendData( nMainCmd, nSubCmd, reinterpret_cast<char*>(&Item), iSize );
			}
		}
		break;

	case QUERY_DELCASHITEM:
		{
			TQDelCashItem *pItem = (TQDelCashItem*)pData;
			TADelCashItem Item;
			memset(&Item, 0, sizeof(TADelCashItem));

			Item.nAccountDBID = pItem->nAccountDBID;
			Item.biItemSerial = pItem->biItemSerial;
			Item.nRetCode = ERROR_DB;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pItem->cWorldSetID);
			if (pWorldDB){
				Item.nRetCode = pWorldDB->QueryDelCashItem(pItem, &Item);

				if (Item.nRetCode != ERROR_NONE)
					g_Log.Log(LogType::_ERROR, pItem->cWorldSetID, pItem->nAccountDBID, 0, 0, L"[ADBID:%u] [QUERY_DELCASHITEM:%d] Result:%d\r\n", pItem->nAccountDBID, pItem->cWorldSetID, Item.nRetCode);
			}
			else{
				g_Log.Log(LogType::_ERROR, pItem->cWorldSetID, pItem->nAccountDBID, 0, 0, L"[ADBID:%u] [QUERY_DELCASHITEM:%d] pWorldDB not found\r\n", pItem->nAccountDBID, pItem->cWorldSetID);
			}

			m_pConnection->AddSendData(nMainCmd, nSubCmd, (char*)&Item, sizeof(Item));
		}
		break;

	case QUERY_RECOVERCASHITEM:
		{
			TQRecoverCashItem *pItem = (TQRecoverCashItem*)pData;
			TARecoverCashItem Item;
			memset(&Item, 0, sizeof(TARecoverCashItem));

			Item.nAccountDBID = pItem->nAccountDBID;
			Item.biItemSerial = pItem->biItemSerial;
			Item.nRetCode = ERROR_DB;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pItem->cWorldSetID);
			if (pWorldDB){
				Item.nRetCode = pWorldDB->QueryRecoverCashItem(pItem, &Item);

				if (Item.nRetCode != ERROR_NONE)
					g_Log.Log(LogType::_ERROR, pItem->cWorldSetID, pItem->nAccountDBID, 0, 0, L"[ADBID:%u] [QUERY_RECOVERCASHITEM:%d] Result:%d\r\n", pItem->nAccountDBID, pItem->cWorldSetID, Item.nRetCode);
			}
			else{
				g_Log.Log(LogType::_ERROR, pItem->cWorldSetID, pItem->nAccountDBID, 0, 0, L"[ADBID:%u] [QUERY_RECOVERCASHITEM:%d] pWorldDB not found\r\n", pItem->nAccountDBID, pItem->cWorldSetID);
			}

			m_pConnection->AddSendData(nMainCmd, nSubCmd, (char*)&Item, sizeof(Item));
		}
		break;

#if defined( PRE_ADD_NAMEDITEM_SYSTEM )
	case QUERY_CHECK_NAMEDITEMCOUNT:
		{
			TQCheckNamedItemCount *pItem = (TQCheckNamedItemCount*)pData;
			TACheckNamedItemCount Item;
			memset(&Item, 0, sizeof(TACheckNamedItemCount));
			Item.nAccountDBID = pItem->nAccountDBID;
			Item.nItemID = pItem->nItemID;
			Item.cIndex = pItem->cIndex;
			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pItem->cWorldSetID);
			if (pWorldDB){
				Item.nRetCode = pWorldDB->QueryCheckNamedItemCount(pItem, &Item);

				if (Item.nRetCode != ERROR_NONE)
					g_Log.Log(LogType::_ERROR, pItem->cWorldSetID, pItem->nAccountDBID, 0, 0, L"[ADBID:%u] [QUERY_CHECK_NAMEDITEMCOUNT:%d] Result:%d\r\n", pItem->nAccountDBID, pItem->cWorldSetID, Item.nRetCode);
			}
			else{
				g_Log.Log(LogType::_ERROR, pItem->cWorldSetID, pItem->nAccountDBID, 0, 0, L"[ADBID:%u] [QUERY_CHECK_NAMEDITEMCOUNT:%d] pWorldDB not found\r\n", pItem->nAccountDBID, pItem->cWorldSetID);
			}

			m_pConnection->AddSendData(nMainCmd, nSubCmd, (char*)&Item, sizeof(Item));
		}
		break;
	case QUERY_CHEATCHECK_NAMEDITEMCOUNT:
		{
			TQCheckNamedItemCount *pItem = (TQCheckNamedItemCount*)pData;
			TACheckNamedItemCount Item;
			memset(&Item, 0, sizeof(TACheckNamedItemCount));
			Item.nAccountDBID = pItem->nAccountDBID;
			Item.nItemID = pItem->nItemID;
			Item.cIndex = pItem->cIndex;
			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pItem->cWorldSetID);
			if (pWorldDB){
				Item.nRetCode = pWorldDB->QueryCheckNamedItemCount(pItem, &Item);

				if (Item.nRetCode != ERROR_NONE)
					g_Log.Log(LogType::_ERROR, pItem->cWorldSetID, pItem->nAccountDBID, 0, 0, L"[ADBID:%u] [QUERY_CHECK_NAMEDITEMCOUNT:%d] Result:%d\r\n", pItem->nAccountDBID, pItem->cWorldSetID, Item.nRetCode);
			}
			else{
				g_Log.Log(LogType::_ERROR, pItem->cWorldSetID, pItem->nAccountDBID, 0, 0, L"[ADBID:%u] [QUERY_CHECK_NAMEDITEMCOUNT:%d] pWorldDB not found\r\n", pItem->nAccountDBID, pItem->cWorldSetID);
			}

			m_pConnection->AddSendData(nMainCmd, nSubCmd, (char*)&Item, sizeof(Item));
		}
		break;
#endif

#if defined (PRE_ADD_EXCHANGE_POTENTIAL)
	case QUERY_MOVEPOTENTIAL:
		{
			TQMovePotential * pItem = (TQMovePotential*)pData;
			TAMovePotential Item;
			memset(&Item, 0, sizeof(TAMovePotential));

			Item.nAccountDBID = pItem->nAccountDBID;
			Item.nRetCode = ERROR_DB;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pItem->cWorldSetID);
			if (pWorldDB)
			{
				Item.nRetCode = pWorldDB->QueryMovePotential(pItem, &Item);
				if (Item.nRetCode != ERROR_NONE)
					g_Log.Log(LogType::_ERROR, pItem->cWorldSetID, pItem->nAccountDBID, pItem->biCharacterDBID, 0, L"[ADBID:%u] [QUERY_MOVEPOTENTIAL:%d] Result:%d\r\n", pItem->nAccountDBID, pItem->cWorldSetID, Item.nRetCode);
			}
			else
				g_Log.Log(LogType::_ERROR, pItem->cWorldSetID, pItem->nAccountDBID, pItem->biCharacterDBID, 0, L"[ADBID:%u] [QUERY_MOVEPOTENTIAL:%d] pWorldDB not found\r\n", pItem->nAccountDBID, pItem->cWorldSetID);

			m_pConnection->AddSendData(nMainCmd, nSubCmd, (char*)&Item, sizeof(Item));
		}
		break;
#endif		//#if defined (PRE_ADD_EXCHANGE_POTENTIAL)
	case QUERY_MODDEGREEOFHUNGER :
		{
			TQModDegreeOfHunger* pItem = reinterpret_cast<TQModDegreeOfHunger*>(pData);
			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pItem->cWorldSetID);
			if( pWorldDB )
			{
				TAHeader packet;
				memset(&packet, 0, sizeof(packet));
				packet.nAccountDBID = pItem->nAccountDBID;
				packet.nRetCode = pWorldDB->QueryModDegreeOfHunger(pItem);
				//m_pConnection->AddSendData(nMainCmd, nSubCmd, reinterpret_cast<char*>(&packet), sizeof(TAHeader));
			}
			else
			{
				g_Log.Log(LogType::_ERROR, pItem->cWorldSetID, pItem->nAccountDBID, 0, 0, L"[QUERY_MODDEGREEOFHUNGER:%d] pWorldDB not found\r\n", pItem->cWorldSetID);
			}
		}
		break;
	case QUERY_MWTEST:
		{
			TQMWTest* pItem = (TQMWTest*)pData;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pItem->cWorldSetID);
			if (pWorldDB)
			{
				pWorldDB->QueryMWTest( pItem );
			}
			break;
		}
#if defined( PRE_ADD_LIMITED_SHOP )
	case QUERY_GETLIMITEDSHOPITEM:
		{
			TQGetLimitedShopItem* pItem = reinterpret_cast<TQGetLimitedShopItem*>(pData);
			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pItem->cWorldSetID);
			if (pWorldDB)
			{
				TAGetLimitedShopItem Item;
				memset(&Item, 0, sizeof(Item));
				Item.nAccountDBID = pItem->nAccountDBID;

				Item.nRetCode = pWorldDB->QueryGetDailyLimitedShopItem(pItem, &Item);
				if(Item.nRetCode == ERROR_NONE)
				{
					Item.nRetCode = pWorldDB->QueryGetWeeklyLimitedShopItem(pItem, &Item);
				}
				if (ERROR_NONE != Item.nRetCode)
					g_Log.Log(LogType::_ERROR, pItem->cWorldSetID, pItem->nAccountDBID, pItem->biCharacterDBID, 0, L"[QUERY_GETLIMITEDSHOPITEM] query error (ret:%d)\r\n", Item.nRetCode);				

				int len = sizeof(TAGetLimitedShopItem) - sizeof(Item.ItemData) + (sizeof(Item.ItemData[0]) * Item.nCount);
				if (len > 0)
					m_pConnection->AddSendData(nMainCmd, nSubCmd, reinterpret_cast<char*>(&Item), len);
			}
			else
				g_Log.Log(LogType::_ERROR, pItem->cWorldSetID, pItem->nAccountDBID, pItem->biCharacterDBID, 0, L"[QUERY_GETLIMITEDSHOPITEM] worldDB not found\r\n");
		}
		break;
	case QUERY_ADDLIMITEDSHOPITEM:
		{
			TQAddLimitedShopItem* pItem = reinterpret_cast<TQAddLimitedShopItem*>(pData);
			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pItem->cWorldSetID);
			if (pWorldDB)
			{
				int nRet = ERROR_DB;
				if(pItem->ItemData.nResetCycle == LimitedShop::LimitedItemType::Day)
					nRet = pWorldDB->QueryAdddailyLimitedShopItem(pItem);
				else if(pItem->ItemData.nResetCycle == LimitedShop::LimitedItemType::Week)
					nRet = pWorldDB->QueryAddWeeklyLimitedShopItem(pItem);
				
				if (ERROR_NONE != nRet)
					g_Log.Log(LogType::_ERROR, pItem->cWorldSetID, pItem->nAccountDBID, pItem->biCharacterDBID, 0, L"[QUERY_ADDLIMITEDSHOPITEM] query error (ret:%d)\r\n", nRet);
			}
			else
				g_Log.Log(LogType::_ERROR, pItem->cWorldSetID, pItem->nAccountDBID, pItem->biCharacterDBID, 0, L"[QUERY_ADDLIMITEDSHOPITEM] worldDB not found\r\n");
		}
		break;
	case QUERY_RESETLIMITEDSHOPITEM:
		{
			TQResetLimitedShopItem* pItem = reinterpret_cast<TQResetLimitedShopItem*>(pData);
			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pItem->cWorldSetID);
			if (pWorldDB)
			{
				int nRet = ERROR_DB;
				if(pItem->nResetCycle == LimitedShop::LimitedItemType::Day)
					nRet = pWorldDB->QueryResetdailyLimitedShopItem(pItem);
				else if(pItem->nResetCycle == LimitedShop::LimitedItemType::Week)
					nRet = pWorldDB->QueryResetWeeklyLimitedShopItem(pItem);

				if (ERROR_NONE != nRet)
					g_Log.Log(LogType::_ERROR, pItem->cWorldSetID, pItem->nAccountDBID, pItem->biCharacterDBID, 0, L"[QUERY_RESETLIMITEDSHOPITEM] query error (ret:%d)\r\n", nRet);
			}
			else
				g_Log.Log(LogType::_ERROR, pItem->cWorldSetID, pItem->nAccountDBID, pItem->biCharacterDBID, 0, L"[QUERY_RESETLIMITEDSHOPITEM] worldDB not found\r\n");
		}
		break;	
#endif
	case QUERY_DEL_EFFECTITEM:
		{
			TQDelEffectItem* pItem = reinterpret_cast<TQDelEffectItem*>(pData);
			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pItem->cWorldSetID);
			if (pWorldDB)
			{
				int nRet = ERROR_DB;
				nRet = pWorldDB->QueryDelEffectItem(pItem);

				if (ERROR_NONE != nRet)
					g_Log.Log(LogType::_ERROR, pItem->cWorldSetID, pItem->nAccountDBID, pItem->biCharacterDBID, 0, L"[QueryDelEffectItem] query error (ret:%d)\r\n", nRet);
			}
			else
				g_Log.Log(LogType::_ERROR, pItem->cWorldSetID, pItem->nAccountDBID, pItem->biCharacterDBID, 0, L"[QueryDelEffectItem] worldDB not found\r\n");
		}
		break;
#if defined(PRE_ADD_TALISMAN_SYSTEM)
	case QUERY_MOD_TALISMANSLOTOPENFLAG:
		{
			TQModTalismanSlotOpenFlag* pItem = reinterpret_cast<TQModTalismanSlotOpenFlag*>(pData);
			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pItem->cWorldSetID);
			if (pWorldDB)
			{
				int nRet = ERROR_DB;
				nRet = pWorldDB->QueryModTalismanSlotOpenFlag(pItem);

				if (ERROR_NONE != nRet)
					g_Log.Log(LogType::_ERROR, pItem->cWorldSetID, pItem->nAccountDBID, pItem->biCharacterDBID, 0, L"[TQModTalismanSlotOpenFlag] query error (ret:%d)\r\n", nRet);
			}
			else
				g_Log.Log(LogType::_ERROR, pItem->cWorldSetID, pItem->nAccountDBID, pItem->biCharacterDBID, 0, L"[TQModTalismanSlotOpenFlag] worldDB not found\r\n");
		}
		break;
#endif
#if defined(PRE_ADD_CHNC2C)
	case QUERY_GET_GAMEMONEY :
		{	
			TQGetGameMoney* pItem = reinterpret_cast<TQGetGameMoney*>(pData);			
			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pItem->cWorldSetID);
			if (pWorldDB)
			{
				TAGetGameMoney Item;
				memset(&Item, 0, sizeof(Item));
				Item.nAccountDBID = pItem->nAccountDBID;
				Item.biCharacterDBID = pItem->biCharacterDBID;
				memcpy(&Item.szSeqID, pItem->szSeqID, sizeof(Item.szSeqID));

				Item.nRetCode = pWorldDB->QueryGetGameMoney(pItem, &Item);				
				if (ERROR_NONE != Item.nRetCode)
					g_Log.Log(LogType::_ERROR, pItem->cWorldSetID, pItem->nAccountDBID, pItem->biCharacterDBID, 0, L"[QUERY_GET_GAMEMONEY] query error (ret:%d)\r\n", Item.nRetCode);

				m_pConnection->AddSendData(nMainCmd, nSubCmd, reinterpret_cast<char*>(&Item), sizeof(TAGetGameMoney));
			}
			else			
				g_Log.Log(LogType::_ERROR, pItem->cWorldSetID, pItem->nAccountDBID, pItem->biCharacterDBID, 0, L"[QUERY_GET_GAMEMONEY] worldDB not found\r\n");			
		}
		break;
	case QUERY_KEEP_GAMEMONEY :
		{
			TQKeepGameMoney* pItem = reinterpret_cast<TQKeepGameMoney*>(pData);			
			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pItem->cWorldSetID);
			if (pWorldDB)
			{
				TAKeepGameMoney Item;
				memset(&Item, 0, sizeof(Item));
				Item.nAccountDBID = pItem->nAccountDBID;
				Item.biCharacterDBID = pItem->biCharacterDBID;
				Item.biReduceCoin = pItem->biReduceCoin;
				Item.cWorldSetID = pItem->cWorldSetID;
				memcpy(&Item.szSeqID, pItem->szSeqID, sizeof(Item.szSeqID));

				Item.nRetCode = pWorldDB->QueryKeepGameMoney(pItem);				
				if (ERROR_NONE != Item.nRetCode)
					g_Log.Log(LogType::_ERROR, pItem->cWorldSetID, pItem->nAccountDBID, pItem->biCharacterDBID, 0, L"[QUERY_KEEP_GAMEMONEY] query error (ret:%d)\r\n", Item.nRetCode);

				m_pConnection->AddSendData(nMainCmd, nSubCmd, reinterpret_cast<char*>(&Item), sizeof(TAKeepGameMoney));
			}
			else			
				g_Log.Log(LogType::_ERROR, pItem->cWorldSetID, pItem->nAccountDBID, pItem->biCharacterDBID, 0, L"[QUERY_KEEP_GAMEMONEY] worldDB not found\r\n");
		}
		break;
	case QUERY_TRANSFER_GAMEMONEY :
		{
			TQTransferGameMoney* pItem = reinterpret_cast<TQTransferGameMoney*>(pData);			
			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pItem->cWorldSetID);
			if (pWorldDB)
			{
				TATransferGameMoney Item;
				memset(&Item, 0, sizeof(Item));
				Item.nAccountDBID = pItem->nAccountDBID;
				Item.biCharacterDBID = pItem->biCharacterDBID;
				Item.biAddCoin = pItem->biAddCoin;
				Item.cWorldSetID = pItem->cWorldSetID;
				memcpy(&Item.szSeqID, pItem->szSeqID, sizeof(Item.szSeqID));

				Item.nRetCode = pWorldDB->QueryTransferGameMoney(pItem);				
				if (ERROR_NONE != Item.nRetCode)
					g_Log.Log(LogType::_ERROR, pItem->cWorldSetID, pItem->nAccountDBID, pItem->biCharacterDBID, 0, L"[QUERY_TRANSFER_GAMEMONEY] query error (ret:%d)\r\n", Item.nRetCode);

				m_pConnection->AddSendData(nMainCmd, nSubCmd, reinterpret_cast<char*>(&Item), sizeof(TAKeepGameMoney));
			}
			else			
				g_Log.Log(LogType::_ERROR, pItem->cWorldSetID, pItem->nAccountDBID, pItem->biCharacterDBID, 0, L"[QUERY_TRANSFER_GAMEMONEY] worldDB not found\r\n");

		}
		break;
#endif //#if defined(PRE_ADD_CHNC2C)
#if defined(PRE_ADD_EQUIPLOCK)
	case QUERY_ADDLOCK_ITEM:
		{
			TQLockItemInfo *pItem = reinterpret_cast<TQLockItemInfo*>(pData);
			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pItem->cWorldSetID);
			if (pWorldDB)
			{
				TALockItemInfo Item;
				memset(&Item, 0, sizeof(Item));

				Item.Code = pItem->Code;
				Item.nAccountDBID = pItem->nAccountDBID;
				Item.cItemSlotIndex = pItem->cItemSlotIndex;
				Item.nRetCode = pWorldDB->QueryAddLockItem(pItem, &Item);
				if(ERROR_NONE != Item.nRetCode)
					g_Log.Log(LogType::_ERROR, pItem->cWorldSetID, pItem->nAccountDBID, pItem->biCharacterDBID, 0, L"[QUERY_ADDLOCK_ITEM] query error (ret:%d)\r\n", Item.nRetCode);

				m_pConnection->AddSendData(nMainCmd, nSubCmd, reinterpret_cast<char*>(&Item), sizeof(TALockItemInfo));
			}
			else			
				g_Log.Log(LogType::_ERROR, pItem->cWorldSetID, pItem->nAccountDBID, pItem->biCharacterDBID, 0, L"[QUERY_ADDLOCK_ITEM] worldDB not found\r\n");
		}
		break;
	case QUERY_REQUEST_ITEMUNLOCK:
		{
			TQUnLockRequsetItemInfo *pItem = reinterpret_cast<TQUnLockRequsetItemInfo*>(pData);
			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pItem->cWorldSetID);
			if (pWorldDB)
			{
				TAUnLockRequestItemInfo Item;
				memset(&Item, 0, sizeof(Item));

				Item.Code = pItem->Code;
				Item.nAccountDBID = pItem->nAccountDBID;
				Item.cItemSlotIndex = pItem->cItemSlotIndex;
				Item.nRetCode = pWorldDB->QueryRequestItemUnlock(pItem, &Item);
				if(ERROR_NONE != Item.nRetCode)
					g_Log.Log(LogType::_ERROR, pItem->cWorldSetID, pItem->nAccountDBID, pItem->biCharacterDBID, 0, L"[QUERY_REQUEST_ITEMUNLOCK] query error (ret:%d)\r\n", Item.nRetCode);

				m_pConnection->AddSendData(nMainCmd, nSubCmd, reinterpret_cast<char*>(&Item), sizeof(TAUnLockRequestItemInfo));
			}
			else			
				g_Log.Log(LogType::_ERROR, pItem->cWorldSetID, pItem->nAccountDBID, pItem->biCharacterDBID, 0, L"[QUERY_REQUEST_ITEMUNLOCK] worldDB not found\r\n");
		}
		break;
	case QUERY_UNLOCK_ITEM:
		{
			TQLockItemInfo *pItem = reinterpret_cast<TQLockItemInfo*>(pData);
			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pItem->cWorldSetID);
			if (pWorldDB)
			{
				int nRetcode = pWorldDB->QueryUnlockItem(pItem);
				if(ERROR_NONE != nRetcode)
					g_Log.Log(LogType::_ERROR, pItem->cWorldSetID, pItem->nAccountDBID, pItem->biCharacterDBID, 0, L"[QUERY_UNLOCK_ITEM] query error (ret:%d)\r\n", nRetcode);
			}
			else			
				g_Log.Log(LogType::_ERROR, pItem->cWorldSetID, pItem->nAccountDBID, pItem->biCharacterDBID, 0, L"[QUERY_UNLOCK_ITEM] worldDB not found\r\n");
		}
		break;
#endif	// #if defined(PRE_ADD_EQUIPLOCK)
	}
}
