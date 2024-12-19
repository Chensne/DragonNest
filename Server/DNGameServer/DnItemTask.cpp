#include "StdAfx.h"
#include "DnItemTask.h"
#include "GameSendPacket.h"
#include "DnPlayerActor.h"
#include "DnDropItem.h"
#include "DNUserSession.h"
#include "DNLogConnection.h"
#include "DnEmblemFactory.h"
#include "DNMasterConnectionManager.h"
#include "DNLogConnection.h"
#include "DNUserSession.h"
#include "DNGameDataManager.h"
#include "TaskManager.h"
#include "DnGameTask.h"
#include "DNMissionSystem.h"
#include "DNDBConnectionManager.h"
#include "DnStateBlow.h"
#include "DnNPCActor.h"
#include "DNRestraint.h"
#include "DnSkillTask.h"
#include "DnPartyTask.h"
#include "DNDBConnection.h"

CDnItemTask::CDnItemTask(CDNGameRoom * pRoom)
: CTask( pRoom )
, CMultiSingleton<CDnItemTask, MAX_SESSION_COUNT>( pRoom )
{
}

CDnItemTask::~CDnItemTask()
{
}

bool CDnItemTask::Initialize()
{
	return true;
}

void CDnItemTask::Process( LOCAL_TIME LocalTime, float fDelta )
{
}

int CDnItemTask::OnDispatchMessage( CDNUserSession * pSession, int nMainCmd, int nSubCmd, char *pData, int nLen )
{
	if (pSession->IsCertified() == false)
	{
		g_Log.Log(LogType::_MOVEPACKET_SEQ, pSession, L"CDnItemTask::OnDispatchMessage RState[%d] UState[%d] SCMD[%d]\n", GetRoom()->GetRoomState(), pSession->GetState(), nSubCmd);
		return ERROR_NONE;
	}

	switch( nMainCmd ) {
		case CS_ITEM: return OnRecvItemMessage( pSession, nSubCmd, pData, nLen );
		case CS_ITEMGOODS: return OnRecvItemGoodsMessage( pSession, nSubCmd, pData, nLen );
	}

	return ERROR_NONE;
}

int CDnItemTask::OnRecvItemMessage( CDNUserSession * pSession, int nSubCmd, char *pData, int nLen )
{
	switch( nSubCmd ) {
		case eItem::CS_MOVEITEM: return OnRecvItemMoveItem( pSession, (CSMoveItem*)pData, nLen );
		case eItem::CS_MOVECASHITEM: return OnRecvItemMoveCashItem( pSession, (CSMoveCashItem*)pData, nLen );
		case eItem::CS_REMOVEITEM: return OnRecvItemRemoveItem( pSession, (CSRemoveItem*)pData, nLen );
		case eItem::CS_REMOVECASH: return pSession->OnRecvItemMessage(nSubCmd, pData, nLen);
		case eItem::CS_USEITEM: return OnRecvItemUseItem( pSession, (CSUseItem*)pData, nLen);
		case eItem::CS_REBIRTH: return OnRecvRebirth( pSession, nLen );
		case eItem::CS_REBIRTH_COIN: return OnRecvRebirthCoin( pSession, nLen );
#ifdef _ADD_NEWDISJOINT
		case eItem::CS_DISJOINT_REQ: return OnRecvItemDisjoint( pSession, (CSItemDisjointReqNew* )pData, nLen );
#else
		case eItem::CS_DISJOINT_REQ: return OnRecvItemDisjoint(pSession, (CSItemDisjointReq*)pData, nLen);
#endif
		case eItem::CS_COMPLETE_RANDOMITEM: return OnRecvItemCompleteRandomItem( pSession, (CSCompleteRandomItem *)pData, nLen );
		case eItem::CS_CANCEL_RANDOMITEM: return OnRecvItemCancelRandomItem( pSession, pData, nLen );
		case eItem::CS_SORTINVENTORY: return OnRecvItemSortInventory( pSession, (CSSortInventory*)pData, nLen );
		case eItem::CS_ENCHANT:	return OnRecvItemEnchant( pSession, (CSEnchantItem*)pData, nLen );
		case eItem::CS_ENCHANTCOMPLETE: return OnRecvItemEnchantComplete( pSession, (CSEnchantItem*)pData, nLen );
		case eItem::CS_ENCHANTCANCEL: return OnRecvItemEnchantCancel( pSession, nLen );
		case eItem::CS_ITEM_COMPOUND_OPEN_REQ: return OnRecvItemCompoundOpenReq( pSession, reinterpret_cast<CSItemCompoundOpenReq*>(pData), nLen );
		case eItem::CS_ITEM_COMPOUND_REQ: return OnrecvItemCompoundReq( pSession, reinterpret_cast<CSCompoundItemReq*>(pData), nLen );
		case eItem::CS_ITEM_COMPOUND_CANCEL_REQ: return OnRecvItemCompoundCancelReq( pSession, reinterpret_cast<CSItemCompoundCancelReq*>(pData), nLen );
		case eItem::CS_MODITEMEXPIREDATE:	return OnRecvItemModitemExpireDate(pSession, (CSModItemExpireDate*)pData, nLen);
		case eItem::CS_DELETE_PET_SKILL:	return OnRecvItemDeletePetSkill(pSession, (CSPetSkillDelete*)pData, nLen);
		// 서버 Item 쪽에서 알려주는 것들
		case eItem::SC_PICKUP: return OnRecvItemPickupItem( pSession, (SCPickUp*)pData, nLen );
		case eItem::SC_REFRESHQUESTINVEN: return OnRecvItemRefreshQItem( pSession, (SCRefreshQuestInven *)pData, nLen );
		case eItem::SC_REMOVEQUESTITEM: return OnRecvItemRemoveQItem( pSession, (SCRemoveItem *)pData, nLen );
		case eItem::SC_REFRESHINVEN: return OnRecvItemRefreshInven( pSession, (SCRefreshInven *)pData, nLen );
		case eItem::SC_REFRESHCASHINVEN: return OnRecvItemRefreshCashInven( pSession, (SCRefreshCashInven *)pData, nLen );
		case eItem::SC_REFRESHEQUIP: return OnRecvItemRefreshEquip( pSession, (SCRefreshEquip *)pData, nLen );
		case eItem::CS_SORTWAREHOUSE: return OnRecvItemWarehouseSort(pSession, (CSSortWarehouse*)pData, nLen);
#if defined(PRE_ADD_EQUIPLOCK)
		case eItem::CS_ITEM_LOCK_REQ: return pSession->OnRecvItemMessage(nSubCmd, pData, nLen);
		case eItem::CS_ITEM_UNLOCK_REQ:return pSession->OnRecvItemMessage(nSubCmd, pData, nLen);
#endif	// #if defined(PRE_ADD_EQUIPLOCK)

		default:	return ERROR_UNKNOWN_HEADER;								//상위에서 리턴이 되지 않았다면 진짜 처리 않하는 패킷 수신이 된거다
	}
}

int CDnItemTask::OnRecvItemGoodsMessage( CDNUserSession * pSession, int nSubCmd, char *pData, int nLen )
{
	return pSession->OnRecvItemGoodsMessage(nSubCmd, pData, nLen);
}

int CDnItemTask::OnRecvItemMoveItem( CDNUserSession *pSession, CSMoveItem *pPacket, int nLen )
{
	if (sizeof(CSMoveItem) != nLen)
		return ERROR_INVALIDPACKET;

	// 창고 관련한 애들은 Blind상태로 들어옴..
	if (!pSession->IsNoneWindowState() && !pSession->IsWindowState(WINDOW_BLIND))
	{
		int nRet = ERROR_ITEM_FAIL;
		switch(pPacket->cMoveType)
		{
			case MoveType_EquipToInven: nRet = ERROR_ITEM_EQUIPTOINVEN_FAIL; break;
			case MoveType_InvenToEquip: nRet = ERROR_ITEM_INVENTOEQUIP_FAIL; break;
		}

		pSession->SendMoveItem(pPacket->cMoveType, pPacket->cSrcIndex, pPacket->cDestIndex, NULL, NULL, nRet);
		return ERROR_NONE;
	}
	// SyncWait 상태에서 아이템 이동시 에러 처리합니다. ( 클라에서도 한번 막지만 서버에서도 한번더 체크합니다. )
	if( CDnPartyTask::IsActive(GetRoom()) && !CDnPartyTask::GetInstance( GetRoom() ).IsSyncComplete() ) {
		pSession->SendMoveItem(pPacket->cMoveType, pPacket->cSrcIndex, pPacket->cDestIndex, NULL, NULL, ERROR_ITEM_FAIL);
		return ERROR_NONE;
	}

	switch(pPacket->cMoveType)
	{
		case MoveType_Ware:
		case MoveType_InvenToWare:
		case MoveType_WareToInven:
		{
			if( pSession->GetItem()->OnRecvMoveItem( pPacket ) == true )
			{
				switch( pPacket->cMoveType )
				{
					case MoveType_InvenToWare:
					{
						if( pPacket->cSrcIndex != -1 ) 
							RemoveInventoryItem( pSession, pPacket->cSrcIndex );
						break;
					}
					case MoveType_WareToInven:
					{
						if( pPacket->cDestIndex != -1 ) 
						{
							const TItem* pItemData = pSession->GetItem()->GetInventory( pPacket->cDestIndex );
							if( pItemData && pItemData->nItemID > 0 ) 
							{
								CDnItem *pItem = CreateItem( pItemData );
								InsertInventoryItem( pSession, pPacket->cDestIndex, pItem );
							}
						}
						break;
					}
				}
			}

			return ERROR_NONE;
		}

		case MoveType_Equip: 
			{
				if( pSession->GetActorHandle() && pSession->GetItem()->OnRecvMoveItem(pPacket) == true ) {
					CDnPlayerActor *pActor = (CDnPlayerActor*)pSession->GetActorHandle().GetPointer();
					if( pPacket->cSrcIndex != -1 ) {
						if( pPacket->cSrcIndex < EQUIP_WEAPON1 )
							pActor->DetachParts( (CDnParts::PartsTypeEnum)pPacket->cSrcIndex );
						else 
							pActor->DetachWeapon( pPacket->cSrcIndex - EQUIP_WEAPON1 );

						RemoveEquipItem( pSession, pPacket->cSrcIndex );
					}
					if( pPacket->cDestIndex != -1 ) {
						if( pPacket->cDestIndex < EQUIP_WEAPON1 )
							pActor->DetachParts( (CDnParts::PartsTypeEnum)pPacket->cDestIndex );
						else 
							pActor->DetachWeapon( pPacket->cDestIndex - EQUIP_WEAPON1 );

						RemoveEquipItem( pSession, pPacket->cDestIndex );
					}

					const TItem *pItemData = pSession->GetItem()->GetEquip( pPacket->cSrcIndex );
					if( pItemData && pItemData->nItemID > 0 ) {
						CDnItem *pItem = CreateItem( pItemData );
						InsertEquipItem( pSession, pPacket->cSrcIndex, pItem );

						if( pItem ) {
							if( pPacket->cSrcIndex < EQUIP_WEAPON1 ) {
								if( pItem->GetItemType() == ITEMTYPE_PARTS )
									pActor->AttachParts( ((CDnParts*)pItem)->GetMySmartPtr(), (CDnParts::PartsTypeEnum)pPacket->cSrcIndex );
							}
							else {
								if( pItem->GetItemType() == ITEMTYPE_WEAPON )
									pActor->AttachWeapon( ((CDnWeapon*)pItem)->GetMySmartPtr(), pPacket->cSrcIndex - EQUIP_WEAPON1 );
							}
						}
					}

					pItemData = pSession->GetItem()->GetEquip( pPacket->cDestIndex );
					if( pItemData && pItemData->nItemID > 0 ) {
						CDnItem *pItem = CreateItem( pItemData );
						InsertEquipItem( pSession, pPacket->cDestIndex, pItem );

						if( pItem ) {
							if( pPacket->cDestIndex < EQUIP_WEAPON1 ) {
								if( pItem->GetItemType() == ITEMTYPE_PARTS )
									pActor->AttachParts( ((CDnParts*)pItem)->GetMySmartPtr(), (CDnParts::PartsTypeEnum)pPacket->cDestIndex );
							}
							else {
								if( pItem->GetItemType() == ITEMTYPE_WEAPON )
									pActor->AttachWeapon( ((CDnWeapon*)pItem)->GetMySmartPtr(), pPacket->cDestIndex - EQUIP_WEAPON1 );
							}
						}
					}

					pActor->RefreshState();
					return ERROR_NONE;
				}
				break;
			}
		case MoveType_Inven:
			{
				if( pSession->GetItem()->OnRecvMoveItem(pPacket) == true ) {
					if( pPacket->cSrcIndex != -1 ) RemoveInventoryItem( pSession, pPacket->cSrcIndex );
					if( pPacket->cDestIndex != -1 ) RemoveInventoryItem( pSession, pPacket->cDestIndex );

					const TItem *pItemData = pSession->GetItem()->GetInventory( pPacket->cSrcIndex );
					if( pItemData && pItemData->nItemID > 0 ) {
						CDnItem *pItem = CreateItem( pItemData );
						InsertInventoryItem( pSession, pPacket->cSrcIndex, pItem );
					}

					pItemData = pSession->GetItem()->GetInventory( pPacket->cDestIndex );
					if( pItemData && pItemData->nItemID > 0 ) {
						CDnItem *pItem = CreateItem( pItemData );
						InsertInventoryItem( pSession, pPacket->cDestIndex, pItem );
					}
				}	else
					_DANGER_POINT();
				return ERROR_NONE;
			}
			break;
		case MoveType_QuestInven:
			{
				if( pSession->GetItem()->OnRecvMoveItem(pPacket) == true ) {
					return ERROR_NONE;
				}
				return ERROR_NONE;
			}
			break;
		case MoveType_EquipToInven: 
			{
				if( pSession->GetActorHandle() && pSession->GetItem()->OnRecvMoveItem(pPacket) == true ) {
					CDnPlayerActor *pActor = (CDnPlayerActor*)pSession->GetActorHandle().GetPointer();
					if( pPacket->cSrcIndex != -1 ) {
						if( pPacket->cSrcIndex < EQUIP_WEAPON1 )
							pActor->DetachParts( (CDnParts::PartsTypeEnum)pPacket->cSrcIndex );
						else 
							pActor->DetachWeapon( pPacket->cSrcIndex - EQUIP_WEAPON1 );

						RemoveEquipItem( pSession, pPacket->cSrcIndex );
					}
					if( pPacket->cDestIndex != -1 ) {
						RemoveInventoryItem( pSession, pPacket->cDestIndex );
					}

					const TItem *pItemData = pSession->GetItem()->GetEquip( pPacket->cSrcIndex );
					if( pItemData && pItemData->nItemID > 0 ) {
						CDnItem *pItem = CreateItem( pItemData );
						InsertEquipItem( pSession, pPacket->cSrcIndex, pItem );

						if( pItem ) {
							if( pPacket->cSrcIndex < EQUIP_WEAPON1 ) {
								if( pItem->GetItemType() == ITEMTYPE_PARTS )
									pActor->AttachParts( ((CDnParts*)pItem)->GetMySmartPtr(), (CDnParts::PartsTypeEnum)pPacket->cSrcIndex );
							}
							else {
								if( pItem->GetItemType() == ITEMTYPE_WEAPON )
									pActor->AttachWeapon( ((CDnWeapon*)pItem)->GetMySmartPtr(), pPacket->cSrcIndex - EQUIP_WEAPON1 );
							}
						}
					}

					pItemData = pSession->GetItem()->GetInventory( pPacket->cDestIndex );
					if( pItemData && pItemData->nItemID > 0 ) {
						CDnItem *pItem = CreateItem( pItemData );
						InsertInventoryItem( pSession, pPacket->cDestIndex, pItem );
					}
					pActor->RefreshState();
				}	else
					_DANGER_POINT();
				return ERROR_NONE;
			}
			break;
		case MoveType_InvenToEquip: 
			{
				if( pSession->GetActorHandle() && pSession->GetItem()->OnRecvMoveItem(pPacket) == true ) {
					CDnPlayerActor *pActor = (CDnPlayerActor*)pSession->GetActorHandle().GetPointer();

					if( pPacket->cSrcIndex != -1 ) {
						RemoveInventoryItem( pSession, pPacket->cSrcIndex );
					}

					if( pPacket->cDestIndex != -1 ) {
						if( pPacket->cDestIndex < EQUIP_WEAPON1 )
							pActor->DetachParts( (CDnParts::PartsTypeEnum)pPacket->cDestIndex );
						else 
							pActor->DetachWeapon( pPacket->cDestIndex - EQUIP_WEAPON1 );

						RemoveEquipItem( pSession, pPacket->cDestIndex );
					}

					const TItem *pItemData = pSession->GetItem()->GetInventory( pPacket->cSrcIndex );
					if( pItemData && pItemData->nItemID > 0 ) {
						CDnItem *pItem = CreateItem( pItemData );
						InsertInventoryItem( pSession, pPacket->cSrcIndex, pItem );
					}

					pItemData = pSession->GetItem()->GetEquip( pPacket->cDestIndex );
					if( pItemData && pItemData->nItemID > 0 ) {
						CDnItem *pItem = CreateItem( pItemData );
						InsertEquipItem( pSession, pPacket->cDestIndex, pItem );

						if( pItem ) {
							if( pPacket->cDestIndex < EQUIP_WEAPON1 ) {
								if( pItem->GetItemType() == ITEMTYPE_PARTS )
									pActor->AttachParts( ((CDnParts*)pItem)->GetMySmartPtr(), (CDnParts::PartsTypeEnum)pPacket->cDestIndex );
							}
							else {
								if( pItem->GetItemType() == ITEMTYPE_WEAPON )
									pActor->AttachWeapon( ((CDnWeapon*)pItem)->GetMySmartPtr(), pPacket->cDestIndex - EQUIP_WEAPON1 );
							}
						}
					}
					pActor->RefreshState();
				}	else
					_DANGER_POINT();
				return ERROR_NONE;
			}
			break;

		case MoveType_Glyph:
			{
				if( pSession->GetActorHandle() )
					pSession->GetItem()->OnRecvMoveItem(pPacket);
				return ERROR_NONE;
			}
			break;

		case MoveType_GlyphToInven:
			{
				if( pSession->GetActorHandle() )
					pSession->GetItem()->OnRecvMoveItem(pPacket);
				return ERROR_NONE;
			}
			break;

		case MoveType_InvenToGlyph:
			{
				if( pSession->GetActorHandle() && pSession->GetItem()->OnRecvMoveItem(pPacket) == true ) {
					CDnPlayerActor *pActor = (CDnPlayerActor*)pSession->GetActorHandle().GetPointer();

					if( pPacket->cSrcIndex != -1 ) {
						RemoveInventoryItem( pSession, pPacket->cSrcIndex );
					}

					if( pPacket->cDestIndex != -1 ) {
						pActor->DetachGlyph( (CDnGlyph::GlyphSlotEnum)pPacket->cDestIndex );

						RemoveGlyphItem( pSession, pPacket->cDestIndex );
					}

					const TItem *pItemData = pSession->GetItem()->GetInventory( pPacket->cSrcIndex );
					if( pItemData && pItemData->nItemID > 0 ) {
						CDnItem *pItem = CreateItem( pItemData );
						InsertInventoryItem( pSession, pPacket->cSrcIndex, pItem );
					}

					pItemData = pSession->GetItem()->GetGlyph( pPacket->cDestIndex );
					if( pItemData && pItemData->nItemID > 0 ) {
						CDnItem *pItem = CreateItem( pItemData );
						InsertGlyphItem( pSession, pPacket->cDestIndex, pItem );

						if( pItem ) {
							pActor->AttachGlyph( ((CDnGlyph*)pItem)->GetMySmartPtr(), (CDnGlyph::GlyphSlotEnum)pPacket->cDestIndex );
						}
					}
					pActor->RefreshState();
				}
				else
					_DANGER_POINT();
				return ERROR_NONE;	
			}
			break;
#if defined(PRE_ADD_TALISMAN_SYSTEM)
		case MoveType_Talisman:
			{
				if( pSession->GetActorHandle() && pSession->GetItem()->OnRecvMoveItem(pPacket) == true )
				{
					CDnPlayerActor *pActor = (CDnPlayerActor*)pSession->GetActorHandle().GetPointer();					
					if( pPacket->cSrcIndex != -1 )
					{
						pActor->DetachTalisman( pPacket->cSrcIndex );
						RemoveTalismanItem( pSession, pPacket->cSrcIndex );
					}
					if( pPacket->cDestIndex != -1 )
					{
						pActor->DetachTalisman( pPacket->cDestIndex );
						RemoveTalismanItem( pSession, pPacket->cDestIndex );
					}

					const TItem *pItemData = pSession->GetItem()->GetTalisman( pPacket->cSrcIndex );
					if( pItemData && pItemData->nItemID > 0 ) {
						CDnItem *pItem = CreateItem( pItemData );
						
						if( pItem ) pActor->AttachTalisman(((CDnTalisman*)pItem)->GetMySmartPtr(), pPacket->cSrcIndex, g_pDataManager->GetTalismanSlotEfficiency(pPacket->cSrcIndex));
						InsertTalismanItem( pSession, pPacket->cSrcIndex, pItem );
					}

					pItemData = pSession->GetItem()->GetTalisman( pPacket->cDestIndex );
					if( pItemData && pItemData->nItemID > 0 ) {
						CDnItem *pItem = CreateItem( pItemData );
						
						if( pItem ) pActor->AttachTalisman(((CDnTalisman*)pItem)->GetMySmartPtr(), pPacket->cDestIndex, g_pDataManager->GetTalismanSlotEfficiency(pPacket->cDestIndex));
						InsertTalismanItem( pSession, pPacket->cDestIndex, pItem );
					}
					pActor->RefreshState();
				}	else
					_DANGER_POINT();
				return ERROR_NONE;
			}
			break;
		case MoveType_TalismanToInven:
			{
				if( pSession->GetActorHandle() && pSession->GetItem()->OnRecvMoveItem(pPacket) == true ) {
					CDnPlayerActor *pActor = (CDnPlayerActor*)pSession->GetActorHandle().GetPointer();
					pActor->DetachTalisman( pPacket->cSrcIndex );
					RemoveTalismanItem( pSession, pPacket->cSrcIndex );
					
					const TItem *pItemData = pSession->GetItem()->GetInventory( pPacket->cDestIndex );
					if( pItemData && pItemData->nItemID > 0 ) {
						CDnItem *pItem = CreateItem( pItemData );
						InsertInventoryItem( pSession, pPacket->cDestIndex, pItem );
					}
					pActor->RefreshState();
				}	else
					_DANGER_POINT();
				return ERROR_NONE;
			}
			break;
		case MoveType_InvenToTalisman:
			{
				if( pSession->GetActorHandle() && pSession->GetItem()->OnRecvMoveItem(pPacket) == true ) {
					CDnPlayerActor *pActor = (CDnPlayerActor*)pSession->GetActorHandle().GetPointer();

					if( pPacket->cSrcIndex != -1 ) {
						RemoveInventoryItem( pSession, pPacket->cSrcIndex );
					}

					const TItem *pItemData = pSession->GetItem()->GetTalisman( pPacket->cDestIndex );
					if( pItemData && pItemData->nItemID > 0 ) {
						CDnItem *pItem = CreateItem( pItemData );
						InsertTalismanItem( pSession, pPacket->cDestIndex, pItem );
						if( pItem ) pActor->AttachTalisman( ((CDnTalisman*)pItem)->GetMySmartPtr(), pPacket->cDestIndex, g_pDataManager->GetTalismanSlotEfficiency(pPacket->cDestIndex) );
					}
					pActor->RefreshState();				
				}	else
					_DANGER_POINT();
				return ERROR_NONE;
			}
			break;
#endif
	}

	return ERROR_UNKNOWN_HEADER;
}

int CDnItemTask::OnRecvItemMoveCashItem( CDNUserSession * pSession, CSMoveCashItem *pPacket, int nLen )
{
	if (sizeof(CSMoveCashItem) != nLen)
		return ERROR_INVALIDPACKET;
	if (!pSession->IsNoneWindowState())
	{
		int nRet = ERROR_ITEM_FAIL;

		switch(pPacket->cMoveType)
		{
		case MoveType_CashEquipToCashInven: nRet = ERROR_ITEM_EQUIPTOINVEN_FAIL; break;
		case MoveType_CashInvenToCashEquip: nRet = ERROR_ITEM_INVENTOEQUIP_FAIL; break;

		case MoveType_VehicleBodyToVehicleInven:
		case MoveType_VehicleInvenToVehicleBody:
		case MoveType_VehiclePartsToCashInven:
		case MoveType_CashInvenToVehicleParts:
			nRet = ERROR_ITEM_INVENTOEQUIP_FAIL;
			break;

		case MoveType_PetInvenToPetBody:
			{
				nRet = ERROR_CANT_RECALL_PET_STATUS;
				break;
			}
		}
		pSession->SendMoveCashItem(pPacket->cMoveType, pPacket->cCashEquipIndex, NULL, pPacket->biCashInvenSerial, NULL, nRet);

		return ERROR_NONE;
	}
	// SyncWait 상태에서 아이템 이동시 에러 처리합니다. ( 클라에서도 한번 막지만 서버에서도 한번더 체크합니다. )
	if( CDnPartyTask::IsActive(GetRoom()) && !CDnPartyTask::GetInstance( GetRoom() ).IsSyncComplete() ) {
		pSession->SendMoveCashItem(pPacket->cMoveType, pPacket->cCashEquipIndex, NULL, pPacket->biCashInvenSerial, NULL, ERROR_ITEM_FAIL);
		return ERROR_NONE;
	}

	switch(pPacket->cMoveType)
	{
	case MoveType_CashEquip:
		{
			if (pSession->GetActorHandle())
				pSession->GetItem()->OnRecvMoveCashItem(pPacket);

			return ERROR_NONE;
		}
		break;

	case MoveType_CashInven:
		{
			if (pSession->GetActorHandle())
				pSession->GetItem()->OnRecvMoveCashItem(pPacket);
			return ERROR_NONE;
		}
		break;

	case MoveType_CashEquipToCashInven:
		{
			const TItem *pBeforeEquip = pSession->GetItem()->GetCashEquip( pPacket->cCashEquipIndex );
			INT64 biEquipSerial = (pBeforeEquip == NULL) ? 0 : pBeforeEquip->nSerial;

			if( pSession->GetActorHandle() && pSession->GetItem()->OnRecvMoveCashItem(pPacket) == true ) {
				CDnPlayerActor *pActor = (CDnPlayerActor*)pSession->GetActorHandle().GetPointer();
				if( pPacket->cCashEquipIndex != -1 ) {
					if( pPacket->cCashEquipIndex < CASHEQUIP_WEAPON1 )
						pActor->DetachCashParts( (CDnParts::PartsTypeEnum)pPacket->cCashEquipIndex );
					else if(pPacket->cCashEquipIndex == CASHEQUIP_WEAPON1 || pPacket->cCashEquipIndex == CASHEQUIP_WEAPON2)
					{
						pActor->DetachCashWeapon( pPacket->cCashEquipIndex - CASHEQUIP_WEAPON1 );
					}
					// 게임서버 이큅 디테치
					else if(pPacket->cCashEquipIndex == CASHEQUIP_EFFECT)
					{
						pActor->SetVehicleEffectIndex(0);
					}

					RemoveCashEquipItem( pSession, pPacket->cCashEquipIndex );
				}
				if( pPacket->biCashInvenSerial > 0 ) {
					RemoveCashInventoryItem( pSession, pPacket->biCashInvenSerial );
				}

				const TItem *pEquip = pSession->GetItem()->GetCashEquip( pPacket->cCashEquipIndex );
				if( pEquip && pEquip->nItemID > 0 ) {
					CDnItem *pItem = CreateItem( pEquip );
					InsertCashEquipItem( pSession, pPacket->cCashEquipIndex, pItem );

					if( pItem ) {
						if( pPacket->cCashEquipIndex < CASHEQUIP_WEAPON1 ) {
							if( pItem->GetItemType() == ITEMTYPE_PARTS )
								pActor->AttachCashParts( ((CDnParts*)pItem)->GetMySmartPtr(), (CDnParts::PartsTypeEnum)pPacket->cCashEquipIndex );
						}
						else if(pPacket->cCashEquipIndex == CASHEQUIP_WEAPON1 || pPacket->cCashEquipIndex == CASHEQUIP_WEAPON2)
						{
							if( pItem->GetItemType() == ITEMTYPE_WEAPON )
								pActor->AttachCashWeapon( ((CDnWeapon*)pItem)->GetMySmartPtr(), pPacket->cCashEquipIndex - CASHEQUIP_WEAPON1 );
						}
						// 게임서버 이큅 어태치
						else if(pPacket->cCashEquipIndex == CASHEQUIP_EFFECT)
						{
							if(pItem->GetItemType() == ITEMTYPE_VEHICLEEFFECT)
								pActor->SetVehicleEffectIndex(pItem->GetClassID());
						}
					}
				}

				const TItem *pInven = pSession->GetItem()->GetCashInventory( biEquipSerial );
				if( pInven && pInven->nItemID > 0 ) {
					CDnItem *pItem = CreateItem( pInven );
					InsertCashInventoryItem( pSession, pInven->nSerial, pItem );
				}
				pActor->RefreshState();
			}
			else
				_DANGER_POINT();

			return ERROR_NONE;
		}
		break;

	case MoveType_CashInvenToCashEquip:
		{
			const TItem *pBeforeEquip = pSession->GetItem()->GetCashEquip( pPacket->cCashEquipIndex );
			INT64 biSerial = (pBeforeEquip && (pBeforeEquip->nSerial > 0)) ? pBeforeEquip->nSerial : pPacket->biCashInvenSerial;

			if( pSession->GetActorHandle() && pSession->GetItem()->OnRecvMoveCashItem(pPacket) == true ) {
				CDnPlayerActor *pActor = (CDnPlayerActor*)pSession->GetActorHandle().GetPointer();

				if( pPacket->biCashInvenSerial > 0 ) {
					RemoveCashInventoryItem( pSession, pPacket->biCashInvenSerial );
				}

				if( pPacket->cCashEquipIndex != -1 ) {
					if( pPacket->cCashEquipIndex < CASHEQUIP_WEAPON1 )
						pActor->DetachCashParts( (CDnParts::PartsTypeEnum)pPacket->cCashEquipIndex );
					else if(pPacket->cCashEquipIndex == CASHEQUIP_WEAPON1 || pPacket->cCashEquipIndex == CASHEQUIP_WEAPON2)
					{
						pActor->DetachCashWeapon( pPacket->cCashEquipIndex - CASHEQUIP_WEAPON1 );
					}
					// 게임서버 이큅 디테치
					else if(pPacket->cCashEquipIndex == CASHEQUIP_EFFECT)
					{
						pActor->SetVehicleEffectIndex(0);
					}

					RemoveCashEquipItem( pSession, pPacket->cCashEquipIndex );
				}

				const TItem *pInven = pSession->GetItem()->GetCashInventory( biSerial );
				if( pInven && pInven->nItemID > 0 ) {
					CDnItem *pItem = CreateItem( pInven );
					InsertCashInventoryItem( pSession, pPacket->biCashInvenSerial, pItem );
				}

				const TItem *pEquip = pSession->GetItem()->GetCashEquip( pPacket->cCashEquipIndex );
				if( pEquip && pEquip->nItemID > 0 ) {
					CDnItem *pItem = CreateItem( pEquip );
					InsertCashEquipItem( pSession, pPacket->cCashEquipIndex, pItem );

					if( pItem ) {
						if( pPacket->cCashEquipIndex < CASHEQUIP_WEAPON1 ) {
							if( pItem->GetItemType() == ITEMTYPE_PARTS )
								pActor->AttachCashParts( ((CDnParts*)pItem)->GetMySmartPtr(), (CDnParts::PartsTypeEnum)pPacket->cCashEquipIndex );
						}
						else if(pPacket->cCashEquipIndex == CASHEQUIP_WEAPON1 || pPacket->cCashEquipIndex == CASHEQUIP_WEAPON2)
						{
							if( pItem->GetItemType() == ITEMTYPE_WEAPON )
								pActor->AttachCashWeapon( ((CDnWeapon*)pItem)->GetMySmartPtr(), pPacket->cCashEquipIndex - CASHEQUIP_WEAPON1 );
						}
						// 게임서버 이큅 어테치
						else if(pPacket->cCashEquipIndex == CASHEQUIP_EFFECT)
						{
							if(pItem->GetItemType() == ITEMTYPE_VEHICLEEFFECT)
								pActor->SetVehicleEffectIndex(pItem->GetClassID());
						}
					}
				}
				pActor->RefreshState();
			}
			else
				_DANGER_POINT();

			return ERROR_NONE;
		}
		break;

	case MoveType_CashGlyph:
		{
			if (pSession->GetActorHandle())
				pSession->GetItem()->OnRecvMoveCashItem(pPacket);

			return ERROR_NONE;
		}
		break;

	case MoveType_CashGlyphToCashInven:
		{
			const TItem *pBeforeGlyph = pSession->GetItem()->GetGlyph( pPacket->cCashEquipIndex );
			INT64 biGlyphSerial = (pBeforeGlyph == NULL) ? 0 : pBeforeGlyph->nSerial;

			if( pSession->GetActorHandle() && pSession->GetItem()->OnRecvMoveCashItem(pPacket) == true ) {
				CDnPlayerActor *pActor = (CDnPlayerActor*)pSession->GetActorHandle().GetPointer();
				if( pPacket->cCashEquipIndex != -1 ) {
					pActor->DetachGlyph( (CDnGlyph::GlyphSlotEnum)pPacket->cCashEquipIndex );

					RemoveGlyphItem( pSession, pPacket->cCashEquipIndex );
				}
				if( pPacket->biCashInvenSerial > 0 ) {
					RemoveCashInventoryItem( pSession, pPacket->biCashInvenSerial );
				}

				const TItem *pEquip = pSession->GetItem()->GetGlyph( pPacket->cCashEquipIndex );
				if( pEquip && pEquip->nItemID > 0 ) {
					CDnItem *pItem = CreateItem( pEquip );
					InsertGlyphItem( pSession, pPacket->cCashEquipIndex, pItem );

					if( pItem ) {
						pActor->AttachGlyph( ((CDnGlyph*)pItem)->GetMySmartPtr(), (CDnGlyph::GlyphSlotEnum)pPacket->cCashEquipIndex );
					}
				}

				const TItem *pInven = pSession->GetItem()->GetCashInventory( biGlyphSerial );
				if( pInven && pInven->nItemID > 0 ) {
					CDnItem *pItem = CreateItem( pInven );
					InsertCashInventoryItem( pSession, pInven->nSerial, pItem );
				}
				pActor->RefreshState();
			}
			else
				_DANGER_POINT();

			return ERROR_NONE;
		}
		break;

	case MoveType_CashInvenToCashGlyph:
		{
			const TItem *pBeforeGlyph = pSession->GetItem()->GetGlyph( pPacket->cCashEquipIndex );
			INT64 biSerial = (pBeforeGlyph && (pBeforeGlyph->nSerial > 0)) ? pBeforeGlyph->nSerial : pPacket->biCashInvenSerial;

			if( pSession->GetActorHandle() && pSession->GetItem()->OnRecvMoveCashItem(pPacket) == true ) {
				CDnPlayerActor *pActor = (CDnPlayerActor*)pSession->GetActorHandle().GetPointer();

				if( pPacket->biCashInvenSerial > 0 ) {
					RemoveCashInventoryItem( pSession, pPacket->biCashInvenSerial );
				}

				if( pPacket->cCashEquipIndex != -1 ) {
					pActor->DetachGlyph( (CDnGlyph::GlyphSlotEnum)pPacket->cCashEquipIndex );

					RemoveGlyphItem( pSession, pPacket->cCashEquipIndex );
				}

				const TItem *pInven = pSession->GetItem()->GetCashInventory( biSerial );
				if( pInven && pInven->nItemID > 0 ) {
					CDnItem *pItem = CreateItem( pInven );
					InsertCashInventoryItem( pSession, pPacket->biCashInvenSerial, pItem );
				}

				const TItem *pEquip = pSession->GetItem()->GetGlyph( pPacket->cCashEquipIndex );
				if( pEquip && pEquip->nItemID > 0 ) {
					CDnItem *pItem = CreateItem( pEquip );
					InsertGlyphItem( pSession, pPacket->cCashEquipIndex, pItem );

					if( pItem ) {
						pActor->AttachGlyph( ((CDnGlyph*)pItem)->GetMySmartPtr(), (CDnGlyph::GlyphSlotEnum)pPacket->cCashEquipIndex );
					}
				}
				pActor->RefreshState();
			}
			else
				_DANGER_POINT();

			return ERROR_NONE;
		}
		break;

	// Dungeon 타입에서는 탈것을 장착하거나 , 탈것 장착템을 사용할수 없습니다.
	// 현재 클라이언트에서 Dungeon 상태라면 패킷을 보내지 않도록 설정해두었습니다.
	// 하지만 만약 Dungeon에서 아이템을 이동하는 현상이 생기면 이곳에서도 막아주어야 하겟지만
	// 그럴일은 거의 없으니 아직 따로 처리는 하지않겠습니다.

	case MoveType_VehicleBodyToVehicleInven:
		{
			TVehicle *pBeforeVehicleEquip = pSession->GetItem()->GetVehicleEquip();
			INT64 biEquipSerial = (pBeforeVehicleEquip == NULL) ? 0 : pBeforeVehicleEquip->Vehicle[Vehicle::Slot::Body].nSerial;

			if( pSession->GetActorHandle()) {
				CDnPlayerActor *pActor = (CDnPlayerActor*)pSession->GetActorHandle().GetPointer();
				if(!pActor)
					return ERROR_ITEM_DONTMOVE; // 이러면 안됩니다.

				CDnVehicleActor *pVehicleActor = pActor->GetMyVehicleActor();
				if (!pVehicleActor || !pActor->IsVehicleMode()) return ERROR_ITEM_DONTMOVE; // 안탔는데 내리라고하는경우 !!

				if (pSession->GetItem()->OnRecvMoveCashItem(pPacket) != true) return ERROR_NONE;

				if (pPacket->cCashEquipIndex != -1) {
					for (int i = 0; i < Vehicle::Slot::Max; i++){
						RemoveVehicleEquipItem(pSession, i);
					}
				}
				if (pPacket->biCashInvenSerial > 0) {
					RemoveVehicleInventoryItem(pSession, pPacket->biCashInvenSerial);
				}

				TVehicle *pEquipVehicle = pSession->GetItem()->GetVehicleEquip();
				if (pEquipVehicle){
					for (int i = 0; i < Vehicle::Slot::Max; i++){
						if (pEquipVehicle->Vehicle[i].nItemID <= 0) continue;
						CDnItem *pItem = CreateItem(&(pEquipVehicle->Vehicle[i]));
						InsertVehicleEquipItem(pSession, pPacket->cCashEquipIndex, pItem);
					}
				}

				const TItem *pInven = pSession->GetItem()->GetCashInventory(biEquipSerial);
				if (pInven && pInven->nItemID > 0) {
					CDnItem *pItem = CreateItem(pInven);
					InsertCashInventoryItem(pSession, pInven->nSerial, pItem);
				}

				// Rotha - 이경우에는 탈것을 내리라는 메세지 : 내려주면됩니다
				pActor->UnRideVehicle(); 
				pActor->RefreshState();
			}
			else
				_DANGER_POINT();

			return ERROR_NONE;			
		}
		break;

	case MoveType_VehicleInvenToVehicleBody:
		{
			TVehicle *pBeforeVehicleEquip = pSession->GetItem()->GetVehicleEquip();
			INT64 biEquipSerial = (pBeforeVehicleEquip && (pBeforeVehicleEquip->Vehicle[Vehicle::Slot::Body].nSerial > 0)) ? pBeforeVehicleEquip->Vehicle[Vehicle::Slot::Body].nSerial : pPacket->biCashInvenSerial;

			if (pSession->GetActorHandle()) {
				CDnPlayerActor *pActor = (CDnPlayerActor*)pSession->GetActorHandle().GetPointer();
				if(!pActor)
					return ERROR_ITEM_DONTMOVE; // 이러면 안됩니다.

				if (pActor->IsVehicleMode() && pActor->GetMyVehicleActor() ) 
					pActor->UnRideVehicle();
		
				if (pSession->GetItem()->OnRecvMoveCashItem(pPacket) != true) return ERROR_NONE;

				if (pPacket->biCashInvenSerial > 0) {
					RemoveVehicleInventoryItem(pSession, pPacket->biCashInvenSerial);
				}

				if (pPacket->cCashEquipIndex != -1) {
					RemoveVehicleEquipItem(pSession, pPacket->cCashEquipIndex);
				}

				const TItem *pInven = pSession->GetItem()->GetVehicleBodyInventory(biEquipSerial);
				if (pInven) {
					CDnItem *pItem = CreateItem(pInven);
					InsertVehicleInventoryItem(pSession, pPacket->biCashInvenSerial, pItem);
				}

				TVehicle *pEquipVehicle = pSession->GetItem()->GetVehicleEquip();

				if (pEquipVehicle) {
					for (int i = 0; i < Vehicle::Slot::Max; i++){
						if (pEquipVehicle->Vehicle[i].nItemID <= 0) continue;
						CDnItem *pItem = CreateItem(&(pEquipVehicle->Vehicle[i]));
						InsertVehicleEquipItem(pSession, pPacket->cCashEquipIndex, pItem);
					}

					// Rotha - 탈것을 태웁니다. //
					pActor->RideVehicle(pEquipVehicle); // Tvehicle에 탈것파츠정보 포함하기때문에 탈것 장착 및 파츠및 색상도 알아서 장착
				}

				pActor->RefreshState();
			}
			else
				_DANGER_POINT();

			return ERROR_NONE;
		}
		break;

	case MoveType_VehiclePartsToCashInven:
		{
			const TItem *pBeforeParts = pSession->GetItem()->GetVehiclePartsEquip( pPacket->cCashEquipIndex );
			INT64 biPartsSerial = (pBeforeParts == NULL) ? 0 : pBeforeParts->nSerial;

			if (pSession->GetActorHandle()) {
				CDnPlayerActor *pActor = (CDnPlayerActor*)pSession->GetActorHandle().GetPointer();
				if(!pActor)
					return ERROR_ITEM_DONTMOVE; // 이러면 안됩니다.

				CDnVehicleActor *pVehicleActor = pActor->GetMyVehicleActor();
				if (!pVehicleActor || !pActor->IsVehicleMode()) return ERROR_ITEM_DONTMOVE; // Rotha - 탈것 장착은 무조건 탈것을 탄상태에서 작동 합니다.

				if (pSession->GetItem()->OnRecvMoveCashItem(pPacket) != true) return ERROR_NONE;

				if( pPacket->cCashEquipIndex != -1 ) {
					pVehicleActor->UnEquipItem(pPacket->cCashEquipIndex); // Rotha - 장착해제
					RemoveVehicleEquipItem( pSession, pPacket->cCashEquipIndex );
				}
				if( pPacket->biCashInvenSerial > 0 ) {
					RemoveCashInventoryItem( pSession, pPacket->biCashInvenSerial );
				}

				const TItem *pParts = pSession->GetItem()->GetVehiclePartsEquip( pPacket->cCashEquipIndex );
				if( pParts && pParts->nItemID > 0 ) {
					CDnItem *pItem = CreateItem( pParts );
					InsertVehicleEquipItem( pSession, pPacket->cCashEquipIndex, pItem );

					if( pItem )
						pVehicleActor->EquipItem(*pParts); // Rotha - 장착		
				}

				const TItem *pInven = pSession->GetItem()->GetCashInventory( biPartsSerial );
				if( pInven && pInven->nItemID > 0 ) {
					CDnItem *pItem = CreateItem( pInven );
					InsertCashInventoryItem( pSession, pInven->nSerial, pItem );
				}
				pActor->RefreshState();
			}
			else
				_DANGER_POINT();

			return ERROR_NONE;
		}
		break;

	case MoveType_CashInvenToVehicleParts:
		{
			const TItem *pBeforeParts = pSession->GetItem()->GetVehiclePartsEquip( pPacket->cCashEquipIndex );
			INT64 biSerial = (pBeforeParts && (pBeforeParts->nSerial > 0)) ? pBeforeParts->nSerial : pPacket->biCashInvenSerial;

			if( pSession->GetActorHandle()) {
				CDnPlayerActor *pActor = (CDnPlayerActor*)pSession->GetActorHandle().GetPointer();
				if(!pActor)
					return ERROR_ITEM_DONTMOVE; // 이러면 안됩니다.

				CDnVehicleActor *pVehicleActor = pActor->GetMyVehicleActor();
				if (!pVehicleActor || !pActor->IsVehicleMode()) return ERROR_ITEM_DONTMOVE; // Rotha -  탄 상태에서만 가능
				if (pSession->GetItem()->OnRecvMoveCashItem(pPacket) != true) return ERROR_NONE;

				if( pPacket->biCashInvenSerial > 0 ) {
					RemoveCashInventoryItem( pSession, pPacket->biCashInvenSerial );
				}

				if( pPacket->cCashEquipIndex != -1 ) {
					pVehicleActor->UnEquipItem(pPacket->cCashEquipIndex); //  Rotha - 장착해제
					RemoveVehicleEquipItem( pSession, pPacket->cCashEquipIndex );
				}

				const TItem *pInven = pSession->GetItem()->GetCashInventory( biSerial );
				if( pInven && pInven->nItemID > 0 ) {
					CDnItem *pItem = CreateItem( pInven );
					InsertCashInventoryItem( pSession, pPacket->biCashInvenSerial, pItem );
				}

				const TItem *pEquip = pSession->GetItem()->GetVehiclePartsEquip( pPacket->cCashEquipIndex );
				if( pEquip && pEquip->nItemID > 0 ) {
					CDnItem *pItem = CreateItem( pEquip );
					InsertVehicleEquipItem( pSession, pPacket->cCashEquipIndex, pItem );

					if( pItem )
						pVehicleActor->EquipItem(*pEquip); // Rotha - 장착					}
				}
				pActor->RefreshState();
			}
			else
				_DANGER_POINT();

			return ERROR_NONE;
		}
		break;

	case MoveType_PetBodyToPetInven:
	case MoveType_PetInvenToPetBody:
	case MoveType_PetPartsToCashInven:
	case MoveType_CashInvenToPetParts:
		{
			if (pSession->GetItem()->OnRecvMoveCashItem(pPacket) != true) 
				return ERROR_NONE;

			break;
		}
	}

	return ERROR_UNKNOWN_HEADER;
}

int CDnItemTask::OnRecvItemRemoveItem( CDNUserSession *pSession, CSRemoveItem *pPacket, int nLen )
{
	if (sizeof(CSRemoveItem) != nLen)
		return ERROR_INVALIDPACKET;

	if (pSession->IsTutorial()){
		pSession->SendRemoveItem(pPacket->cType, pPacket->cSlotIndex, NULL, ERROR_ITEM_FAIL);
		return ERROR_NONE;
	}

	if (!pSession->IsNoneWindowState() && !pSession->IsWindowState(WINDOW_BLIND))
	{
		pSession->SendRemoveItem(pPacket->cType, pPacket->cSlotIndex, NULL, ERROR_ITEM_FAIL);
		return ERROR_NONE;
	}

	if (pSession->GetRestraint()->CheckRestraint(_RESTRAINTTYPE_TRADE) == false)
		return ERROR_NONE;

	if( !pSession->GetActorHandle() )
		return ERROR_NONE;

#if defined(PRE_ADD_EQUIPLOCK)
	//게임 모드가 구울모드면 아이템 파괴를 막아버린다
	if (!GetRoom() || GetRoom()->bIsZombieMode())
	{
		pSession->SendRemoveItem(pPacket->cType, pPacket->cSlotIndex, NULL, ERROR_ITEM_FAIL);
		return ERROR_NONE;
	}
#endif	// #if defined(PRE_ADD_EQUIPLOCK)

	if( pSession->GetItem()->OnRecvRemoveItem( pPacket ) == false )
		return ERROR_NONE;

	switch( pPacket->cType ) {
		case ITEMPOSITION_EQUIP:
			{
				CDnPlayerActor *pActor = (CDnPlayerActor*)pSession->GetActorHandle().GetPointer();

				if( pPacket->cSlotIndex < EQUIP_WEAPON1 )
					pActor->DetachParts( (CDnParts::PartsTypeEnum)pPacket->cSlotIndex );
				else 
					pActor->DetachWeapon( pPacket->cSlotIndex - EQUIP_WEAPON1 );

				RemoveEquipItem( pSession, pPacket->cSlotIndex );

				// 여기 코드는 필요없는데..흐음.. ( 에러났을때 대비였나.. )
				const TItem *pItemData = pSession->GetItem()->GetEquip( pPacket->cSlotIndex );
				if( pItemData && pItemData->nItemID > 0 ) {
					CDnItem *pItem = CreateItem( pItemData );
					InsertEquipItem( pSession, pPacket->cSlotIndex, pItem );

					if( pItem ) {
						if( pPacket->cSlotIndex < EQUIP_WEAPON1 ) {
							if( pItem->GetItemType() == ITEMTYPE_PARTS )
								pActor->AttachParts( ((CDnParts*)pItem)->GetMySmartPtr(), (CDnParts::PartsTypeEnum)pPacket->cSlotIndex );
						}
						else {
							if( pItem->GetItemType() == ITEMTYPE_WEAPON )
								pActor->AttachWeapon( ((CDnWeapon*)pItem)->GetMySmartPtr(), pPacket->cSlotIndex - EQUIP_WEAPON1 );
						}
					}
				}
				//
				pActor->RefreshState();
				return ERROR_NONE;
			}
			break;
		case ITEMPOSITION_INVEN:
			{
				RemoveInventoryItem( pSession, pPacket->cSlotIndex );

				const TItem *pItemData = pSession->GetItem()->GetEquip( pPacket->cSlotIndex );
				if( pItemData && pItemData->nItemID > 0 ) {
					CDnItem *pItem = CreateItem( pItemData );
					InsertInventoryItem( pSession, pPacket->cSlotIndex, pItem );
				}

				if( CDnPartyTask::IsActive(GetRoom()) ) CDnPartyTask::GetInstance(GetRoom()).UpdateGateInfo();

				return ERROR_NONE;
			}
			break;
		case ITEMPOSITION_GLYPH:
			{
				CDnPlayerActor *pActor = (CDnPlayerActor*)pSession->GetActorHandle().GetPointer();

				pActor->DetachGlyph( (CDnGlyph::GlyphSlotEnum)pPacket->cSlotIndex );

				RemoveGlyphItem( pSession, pPacket->cSlotIndex );

				// 여기 코드는 필요없는데..흐음.. ( 에러났을때 대비였나.. )
				const TItem *pItemData = pSession->GetItem()->GetGlyph( (CDnGlyph::GlyphSlotEnum)pPacket->cSlotIndex );
				if( pItemData && pItemData->nItemID > 0 ) {
					CDnItem *pItem = CreateItem( pItemData );
					InsertGlyphItem( pSession, pPacket->cSlotIndex, pItem );

					if( pItem ) {
						pActor->AttachGlyph( ((CDnGlyph*)pItem)->GetMySmartPtr(), (CDnGlyph::GlyphSlotEnum)pPacket->cSlotIndex );
					}
				}
				//
				pActor->RefreshState();
				return ERROR_NONE;
			}
			break;
#if defined(PRE_ADD_TALISMAN_SYSTEM)
		case ITEMPOSITION_TALISMAN:
			{
				CDnPlayerActor *pActor = (CDnPlayerActor*)pSession->GetActorHandle().GetPointer();
				pActor->DetachTalisman( pPacket->cSlotIndex );
				RemoveTalismanItem( pSession, pPacket->cSlotIndex );

				pActor->RefreshState();
				return ERROR_NONE;
			}
			break;
#endif
		case ITEMPOSITION_QUESTINVEN:
			{
				return ERROR_NONE;
			}
			break;
	}

	return ERROR_UNKNOWN_HEADER;
}

int CDnItemTask::OnRecvItemPickupItem( CDNUserSession *pSession, SCPickUp *pPacket, int nLen )
{
	switch( pPacket->nRetCode ) {
		case ERROR_NONE:
			{
				const TItem *pItemData = pSession->GetItem()->GetInventory( pPacket->PickUpItem.cSlotIndex );
				if( pPacket->PickUpItem.cSlotIndex != -1 ) {
					RemoveInventoryItem( pSession, pPacket->PickUpItem.cSlotIndex );
				}
				if (pItemData)
				{
					CDnItem *pItem = CreateItem( pItemData );
					if( pItem ) {
						InsertInventoryItem( pSession, pPacket->PickUpItem.cSlotIndex, pItem );
					}
				}
			}
			break;
		default:
			break;
	}
	// 느릴 수 도 있다.
	if( CDnPartyTask::IsActive(GetRoom()) ) CDnPartyTask::GetInstance(GetRoom()).UpdateGateInfo();

	return ERROR_NONE;
}

int CDnItemTask::OnRecvItemUseItem( CDNUserSession *pSession, CSUseItem *pPacket, int nLen )
{
	if (sizeof(CSUseItem) != nLen)
		return ERROR_INVALIDPACKET;

	// 제제검사
	if( pSession->GetRestraint()->CheckRestraint(_RESTRAINTTYPE_TRADE) == false )
		return ERROR_NONE;	

	// 죽은상태에서 사용가능한 요리아이템이 생김 게임서버에서 판단시점 OnRecvUseItem단으로 옮김
	// Note 한기: 죽은 상태에선 아이템 사용 불가. 물약 같은 거 못 먹음.
	/*if( !pSession->GetActorHandle() )
		return ERROR_NONE;

	if( pSession->GetActorHandle()->IsDie() )
		return ERROR_NONE;*/

	pSession->GetItem()->OnRecvUseItem(pPacket);

	return ERROR_GENERIC_INVALIDREQUEST;
}

int CDnItemTask::OnRecvItemRefreshQItem( CDNUserSession *pSession, SCRefreshQuestInven *pPacket, int nLen )
{
	return ERROR_NONE;
}

int CDnItemTask::OnRecvItemRemoveQItem( CDNUserSession *pSession, SCRemoveItem *pPacket, int nLen )
{
	return ERROR_NONE;
}

int CDnItemTask::OnRecvRebirth( CDNUserSession* pSession, int nLen )
{
	if (nLen != 0)
		return ERROR_INVALIDPACKET;

	if (GetRoom()->GetRoomState() != _GAME_STATE_PLAY || pSession->GetState() != SESSION_STATE_GAME_PLAY)
	{
		_DANGER_POINT_MSG(L"if (GetRoom()->GetRoomState() != _GAME_STATE_PLAY)");
		return ERROR_NONE;
	}

	pSession->SendBackToVillage(true);
	return ERROR_NONE;
}

// 부활 가능한지 검사
bool CDnItemTask::_bIsRebirth()
{
	// 미션 Fail 상태이기 때문에 부활 불가능합니다.
	CDnGameTask* pTask = (CDnGameTask*)CTaskManager::GetInstance(GetRoom()).GetTask( "GameTask" );
	if( !pTask || pTask->IsDungeonFailed() )
		return false;
	
	// PvP방에서는 부활 불가능합니다.
	if( !GetRoom() || GetRoom()->bIsPvPRoom() )
		return false;
	
	return true;
}

//아이템아이디로 체크
CDnItem* CDnItemTask::FindRebirthItem(CDNUserSession* pSession, int nAllowMapType, int nItemID)
{	
	std::vector<CDnItem *> rebirthItems;
	ScanItemFromID(pSession->GetActorHandle(), nItemID, &rebirthItems);
	if (rebirthItems.empty())
		return false;

	std::vector<CDnItem *>::iterator iter = rebirthItems.begin();
	std::vector<CDnItem *>::iterator endIter = rebirthItems.end();
	for (; iter != endIter; ++iter)
	{
		CDnItem* pItem = (*iter);
		TItemData* pItemData = g_pDataManager->GetItemData( pItem->GetClassID() );
		if (NULL == pItemData)
			continue;

		//이 맵에서 사용할 수 있는 부활 아이템이 있으면
		if (pItemData->nAllowMapType & nAllowMapType)
			return pItem;
	}

	//아이템중 이 맵에서 사용할 아이템이 없으면 부활 불가
	return NULL;
}

int CDnItemTask::OnRecvRebirthCoin( CDNUserSession* pSession, int nLen )
{
	if (nLen != 0)
		return ERROR_INVALIDPACKET;

	
	if( pSession->GetState() != SESSION_STATE_GAME_PLAY)
		return ERROR_NONE;

	bool bIsPartyRestore = false;
	CDNGameRoom::PartyBackUpStruct BackupInfo;
	if( GetRoom()->GetBackupPartyInfo( pSession->GetCharacterDBID(), BackupInfo ) == true )
	{
		if( BackupInfo.nHPPercent > 0 )
			bIsPartyRestore = true;
	}
	
	if( bIsPartyRestore == false && GetRoom()->GetPartyData(pSession)->nUsableRebirthCoin == 0 && GetRoom()->GetPartyData(pSession)->nUsableRebirthItemCoin == 0 ) 
		return ERROR_NONE;

	// 부활가능한지 검사
	if( !_bIsRebirth() )
		return ERROR_NONE;

	DnActorHandle hActor = pSession->GetActorHandle();

	// 씨드래곤 맵상에서는 부활 아이템이 있어야 함 [2010/12/21 semozz]
	const TMapInfo* pMapData = g_pDataManager->GetMapInfo( pSession->GetMapIndex() );
	if (NULL == pMapData)
		return ERROR_NONE;

	CDnWorld *pWorld = GetRoom()->GetWorld();
	if (NULL == pWorld)
		return ERROR_NONE;

	eDragonNestType _dragonNestType = pWorld->GetDragonNestType();
	
	//씨드래곤네스트 맵이면
	bool isDragonNestMap = _dragonNestType == eDragonNestType::SeaDragon;

	CDnItem* pRebirthItem = NULL;
	if( !hActor->IsDie() || hActor->IsAppliedThisStateBlow(STATE_BLOW::BLOW_057))
		return ERROR_NONE;

	bool isRebirthAble = false;
	bool bUseRebirthItem = false;

	//코인 부활 먼저 체크
	if( bIsPartyRestore == false && GetRoom()->GetPartyData(pSession)->nUsableRebirthCoin == -1 )
	{
		//부활 횟수 제한없음
		isRebirthAble = pSession->DecreaseRebirthCoin(1);
	}
	else if ( bIsPartyRestore == false && GetRoom()->GetPartyData(pSession)->nUsableRebirthCoin > 0 )
	{		
		//부활 횟수 제한
		isRebirthAble = pSession->DecreaseRebirthCoin(1);
	}

	if ( bIsPartyRestore == false && !isRebirthAble && GetRoom()->GetPartyData(pSession)->ReverseItemList.size() > 0 && GetRoom()->GetPartyData(pSession)->nUsableRebirthItemCoin > 0 )
	{
		//아이템 부활 체크
		for( std::list<int>::iterator itor =  GetRoom()->GetPartyData(pSession)->ReverseItemList.begin(); itor != GetRoom()->GetPartyData(pSession)->ReverseItemList.end(); itor++ )
		{
			pRebirthItem = FindRebirthItem(pSession, pMapData->nAllowMapType, *itor);
			if( pRebirthItem )
			{
				isRebirthAble = true;
				bUseRebirthItem = true;
				break;
			}
		}		
	}	
	
	if (isRebirthAble || bIsPartyRestore == true )
	{
		// Note : 이벤토리에서 부활 코인이 남아 있는지 확인한다.
		//	남아 있다면 코인을 하나 소모하고 부활 상태효과를 추가한다.
		if( hActor )
		{
			std::string strParam = "Coin";
			if( bIsPartyRestore == true )
			{
				if( BackupInfo.nHPPercent > 0 )
				{
					if( BackupInfo.nHPPercent < 100 )
					{
						strParam.append("/");
						strParam.append(boost::lexical_cast<std::string>(BackupInfo.nHPPercent));
					}

					if( BackupInfo.nSPPercent < 100 )
					{
						strParam.append("/");
						strParam.append(boost::lexical_cast<std::string>(BackupInfo.nSPPercent));
					}
				}
			}
			hActor->CmdAddStateEffect( NULL, STATE_BLOW::BLOW_057, 5000, strParam.c_str() );

			// Note 한기: 부활 누르자마자 스킬 쓰는 경우 클라이언트에서 날아온 스킬 사용 요청 패킷이 서버에 도착했을 때,
			// 서버에서는 아직 "Die" 액션 실행중이라서 스킬 사용 실패가 되고 클라이언트는 스킬 사용으로 될 수 있으므로 
			// "Stand" 액션을 실행해줌. (유령된 후에 움직이면 서버쪽에서 "Stand" 액션이 들어가는데 그 전엔 "Die" 상태임)
			hActor->SetActionQueue( "Stand" );
		}

		pSession->GetEventSystem()->OnEvent( EventSystem::OnRebirth );
	}
	
	if( GetRoom()->GetPartyData(pSession)->nUsableRebirthCoin > 0 && bUseRebirthItem == false )
	{
		//코인사용
		if( bIsPartyRestore == false )
		{
			GetRoom()->GetPartyData(pSession)->nUsableRebirthCoin -= 1;
		}		

		if (GetRoom()->GetPartyData(pSession)->nUsableRebirthCoin <= 0) 
			GetRoom()->GetPartyData(pSession)->nUsableRebirthCoin = 0;
	}
	else if( GetRoom()->GetPartyData(pSession)->nUsableRebirthItemCoin > 0 && bUseRebirthItem == true )
	{
		//아이템 부활
		if (!pSession->GetItem()->DeleteInventoryByItemID(pRebirthItem->GetClassID(), 1, DBDNWorldDef::UseItem::Use)) 
			return ERROR_ITEM_FAIL;

		GetRoom()->GetPartyData(pSession)->nUsableRebirthItemCoin -= 1;

		if (GetRoom()->GetPartyData(pSession)->nUsableRebirthItemCoin <= 0) 
			GetRoom()->GetPartyData(pSession)->nUsableRebirthItemCoin = 0;

		for( std::list<int>::iterator itor =  GetRoom()->GetPartyData(pSession)->ReverseItemList.begin(); itor != GetRoom()->GetPartyData(pSession)->ReverseItemList.end(); itor++ )
			pSession->SendSpecialRebirthItem( *itor, GetRoom()->GetPartyData(pSession)->nUsableRebirthItemCoin );		
	}

	pSession->SendRebirthCoin(ERROR_NONE, GetRoom()->GetPartyData(pSession)->nUsableRebirthCoin, _REBIRTH_SELF, pSession->GetSessionID());	// 코인 결과값을 날려준다

	if( bIsPartyRestore == true )
		GetRoom()->DelBackupPartyInfo( pSession->GetCharacterDBID() );

	return ERROR_NONE;
}

int CDnItemTask::OnRecvItemRefreshInven( CDNUserSession *pSession, SCRefreshInven *pPacket, int nLen )
{
	RemoveInventoryItem( pSession, pPacket->ItemInfo.cSlotIndex );

	if( pPacket->ItemInfo.Item.nItemID > 0 ) {
		CDnItem *pItem = CreateItem( &pPacket->ItemInfo.Item );
		InsertInventoryItem( pSession, pPacket->ItemInfo.cSlotIndex, pItem );
	}
	if( pPacket->bNewSign ) {
		if( CDnPartyTask::IsActive(GetRoom()) ) CDnPartyTask::GetInstance(GetRoom()).UpdateGateInfo();
	}
	return ERROR_NONE;
}

int CDnItemTask::OnRecvItemRefreshCashInven( CDNUserSession *pSession, SCRefreshCashInven *pPacket, int nLen )
{
	for (int i = 0; i < pPacket->nCount; i++){
		RemoveCashInventoryItem( pSession, pPacket->ItemList[i].nSerial );

		if( pPacket->ItemList[i].nItemID > 0 && pPacket->ItemList[i].wCount > 0 ) {
			CDnItem *pItem = CreateItem( &(pPacket->ItemList[i]) );
			InsertCashInventoryItem( pSession, pPacket->ItemList[i].nSerial, pItem );
		}
	}
	return ERROR_NONE;
}

int CDnItemTask::OnRecvItemRefreshEquip( CDNUserSession *pSession, SCRefreshEquip *pPacket, int nLen )
{
	return ERROR_NONE;
}

int CDnItemTask::OnRecvItemWarehouseSort(CDNUserSession *pSession, CSSortWarehouse * pPacket, int nLen)
{
	if (sizeof(CSSortWarehouse) - sizeof(pPacket->SlotInfo) + (sizeof(TSortSlot) * pPacket->cTotalCount) != nLen)
		return ERROR_INVALIDPACKET;

	if( !pSession->IsWindowState(WINDOW_BLIND) )
	{
		pSession->SendSortWarehouse(ERROR_GENERIC_INVALIDREQUEST);
		return ERROR_NONE;
	}

	if (pPacket->cTotalCount > WAREHOUSEMAX)
	{
		pSession->SendSortWarehouse(ERROR_GENERIC_INVALIDREQUEST);
		return ERROR_NONE;
	}

	if (pSession->GetItem()->SortWarehouse(pPacket)) pSession->SendSortWarehouse(ERROR_NONE);
	else pSession->SendSortWarehouse(ERROR_GENERIC_INVALIDREQUEST);
		return ERROR_NONE;
}
#ifdef _ADD_NEWDISJOINT
int CDnItemTask::OnRecvItemDisjoint(CDNUserSession* pSession, CSItemDisjointReqNew* pPacket, int nLen)
{
	if (sizeof(CSItemDisjointReqNew) != nLen)
		return ERROR_INVALIDPACKET;

	if (pSession->GetRestraint()->CheckRestraint(_RESTRAINTTYPE_TRADE) == false)
		return ERROR_NONE;

	int iResult = ERROR_NONE;
#ifdef _ADD_NEWDISJOINT
	SCItemDisjointResNew *pResultPacket = new SCItemDisjointResNew();
	ZeroMemory(pResultPacket, sizeof(SCItemDisjointResNew));
#endif
	int nLastIndex = 0;
	for (int k = 0; k < 4; k++)
	{
		if (pPacket->cSlotIndex[k] == 0 || pPacket->biItemSerial[k] == 0)
			continue;

		CDnItem* pItem = GetInventoryItem(pSession, pPacket->cSlotIndex[k]);
		if (!pItem)
		{
			_DANGER_POINT();
			return ERROR_NONE;
		}
		if (pItem->GetSerialID() != pPacket->biItemSerial[k]) return ERROR_ITEM_FAIL;

		TItemData *pItemData = g_pDataManager->GetItemData(pItem->GetClassID());
		int nItemID = pItem->GetClassID();
		bool bSuccessDisjoint = false;
		if (pItem && pItemData)
		{
			// 아이템 분해기 프랍의 정확한 위치가 필요할 듯. 
			// 일단 지금은 자신의 플레이어의 위치로 하면 될까나.
			DnActorHandle hNpc = CDnActor::FindActorFromUniqueID(pSession->GetGameRoom(), pPacket->nNpcUniqueID);
			if (hNpc) {
				int nDisjointType = 0;
				if (hNpc->IsNpcActor()) {
					CDnNPCActor *pNpc = static_cast<CDnNPCActor *>(hNpc.GetPointer());
					nDisjointType = (pNpc->GetNpcData()) ? pNpc->GetNpcData()->nParam[0] : 0;
				}

				if (pItem->CanDisjoint() && pItem->CanDisjointType(nDisjointType) && pItem->CanDisjointLevel(pSession->GetLevel()))
				{
					// 귀속이면서 봉인상태의 아이템인지 검사
					bool bSealedItem = false;
					if (pItemData->cReversion == ITEMREVERSION_BELONG && pItem->IsSoulbBound() == false)
						bSealedItem = true;
					if (!bSealedItem)
					{
						for (int i = 0; i < pPacket->nCount[k]; ++i)
						{
							bSuccessDisjoint = false;
							iResult = ERROR_NONE;

							// 분해는 가능한 아이템인가. 돈이 충분한가.
							if (pSession->CheckEnoughCoin(pItem->GetDisjointCost()))
							{
								// 아이템 드랍되면 알아서 줏어가므로 성공 여부만 보내주면 됨.
								// 갖고 있던 아이템은 삭제.
								DNVector(CDnItem::DropItemStruct) VecDisjointItems;
								pItem->GetDisjointItems(VecDisjointItems);

								if (!VecDisjointItems.empty())
									bSuccessDisjoint = true;

								int nLocalItemID = pItem->GetClassID();
								bool bDeleteItemResult = pSession->GetItem()->DeleteInventoryBySlot(pPacket->cSlotIndex[k], 1, pPacket->biItemSerial[k], DBDNWorldDef::UseItem::DeCompose);
								_ASSERT(bDeleteItemResult);
								if (bDeleteItemResult)
								{
									EtVector3 vPos = *hNpc->GetPosition();

									// npc 바라보는 위치에서 2 미터 정도 앞으로 밀어서 생성시킴
									vPos += (hNpc->GetMatEx()->m_vZAxis * 200.0f);

									for (DWORD i = 0; i < VecDisjointItems.size(); ++i)
									{
										pResultPacket->nItemID[i + nLastIndex] = VecDisjointItems[i].nItemID;
										pResultPacket->nCount[i + nLastIndex] = VecDisjointItems[i].nCount;
										nLastIndex++;

										//int nRotate = (int)(((VecDisjointItems[i].nSeed % 360) / (float)VecDisjointItems.size()) * i);
										pSession->GetItem()->CreateInvenItem1(VecDisjointItems[i].nItemID, VecDisjointItems[i].nCount, -1, -1, DBDNWorldDef::AddMaterializedItem::ItemDecompose, 0, CREATEINVEN_ETC);

										//RequestDropItem(STATIC_INSTANCE(CDnDropItem::s_dwUniqueCount)++, vPos, VecDisjointItems[i].nItemID,
										//	VecDisjointItems[i].nSeed, VecDisjointItems[i].nCount, nRotate, pSession->GetSessionID());
									}

									pSession->DelCoin(pItem->GetDisjointCost(), DBDNWorldDef::CoinChangeCode::DisjointTax, pSession->m_nClickedNpcID);
									pSession->GetEventSystem()->OnEvent(EventSystem::OnItemDisjoint, 1, EventSystem::ItemID, nLocalItemID);
								}
								else
									iResult = ERROR_ITEM_FAIL;		// 아이템 삭제가 안되는 이상한 에러.
							}
							else
							{
								iResult = ERROR_ITEM_INSUFFICIENCY_MONEY;
							}

							if (iResult != ERROR_NONE)
								break;
						}
					}
					else
						iResult = ERROR_ITEM_CAN_NOT_DISJOINT;
				}
				else
					iResult = ERROR_ITEM_CAN_NOT_DISJOINT;
			}
			else iResult = ERROR_ITEM_FAIL;
		}
		else
			iResult = ERROR_ITEM_NOTFOUND;		// 존재하지 않는 아이템을 분해하려고..
		pResultPacket->bSuccess = bSuccessDisjoint;
		// 아이템 분해 성공. 클라로 결과 보냄.
	}
	if (ERROR_NONE == iResult)
		SendItemDisjointResNew(pSession, pResultPacket); //wtf
	else
		SendItemDisjointResNew(pSession, pResultPacket);		// 0 은 실패

	return iResult;
}
#else
int CDnItemTask::OnRecvItemDisjoint(CDNUserSession* pSession, CSItemDisjointReq* pPacket, int nLen)
{
	if (sizeof(CSItemDisjointReq) != nLen)
		return ERROR_INVALIDPACKET;

	if (pSession->GetRestraint()->CheckRestraint(_RESTRAINTTYPE_TRADE) == false)
		return ERROR_NONE;

	int iResult = ERROR_NONE;

	CDnItem* pItem = GetInventoryItem(pSession, pPacket->cSlotIndex);
	if (!pItem)
	{
		_DANGER_POINT();
		return ERROR_NONE;
	}
	if (pItem->GetSerialID() != pPacket->biItemSerial) return ERROR_ITEM_FAIL;
#ifdef _ADD_NEWDISJOINT
	SCItemDisjointResNew *pResultPacket = new SCItemDisjointResNew();
	ZeroMemory(pResultPacket, sizeof(SCItemDisjointResNew));
#endif
	TItemData *pItemData = g_pDataManager->GetItemData(pItem->GetClassID());
	int nItemID = pItem->GetClassID();
	bool bSuccessDisjoint = false;
	if (pItem && pItemData)
	{
		// 아이템 분해기 프랍의 정확한 위치가 필요할 듯. 
		// 일단 지금은 자신의 플레이어의 위치로 하면 될까나.
		DnActorHandle hNpc = CDnActor::FindActorFromUniqueID(pSession->GetGameRoom(), pPacket->nNpcUniqueID);
		if (hNpc) {
			int nDisjointType = 0;
			if (hNpc->IsNpcActor()) {
				CDnNPCActor *pNpc = static_cast<CDnNPCActor *>(hNpc.GetPointer());
				nDisjointType = (pNpc->GetNpcData()) ? pNpc->GetNpcData()->nParam[0] : 0;
			}

			if (pItem->CanDisjoint() && pItem->CanDisjointType(nDisjointType) && pItem->CanDisjointLevel(pSession->GetLevel()))
			{
				// 귀속이면서 봉인상태의 아이템인지 검사
				bool bSealedItem = false;
				if (pItemData->cReversion == ITEMREVERSION_BELONG && pItem->IsSoulbBound() == false)
					bSealedItem = true;
				if (!bSealedItem)
				{
					for (int i = 0; i<pPacket->nCount; ++i)
					{
						bSuccessDisjoint = false;
						iResult = ERROR_NONE;

						// 분해는 가능한 아이템인가. 돈이 충분한가.
						if (pSession->CheckEnoughCoin(pItem->GetDisjointCost()))
						{
							// 아이템 드랍되면 알아서 줏어가므로 성공 여부만 보내주면 됨.
							// 갖고 있던 아이템은 삭제.
							DNVector(CDnItem::DropItemStruct) VecDisjointItems;
							pItem->GetDisjointItems(VecDisjointItems);

							if (!VecDisjointItems.empty())
								bSuccessDisjoint = true;

							int nLocalItemID = pItem->GetClassID();
							bool bDeleteItemResult = pSession->GetItem()->DeleteInventoryBySlot(pPacket->cSlotIndex, 1, pPacket->biItemSerial, DBDNWorldDef::UseItem::DeCompose);
							_ASSERT(bDeleteItemResult);
							if (bDeleteItemResult)
							{
								EtVector3 vPos = *hNpc->GetPosition();

								// npc 바라보는 위치에서 2 미터 정도 앞으로 밀어서 생성시킴
								vPos += (hNpc->GetMatEx()->m_vZAxis * 200.0f);

								for (DWORD i = 0; i < VecDisjointItems.size(); ++i)
								{
#ifdef _ADD_NEWDISJOINT
									pResultPacket->nItemID[i] = VecDisjointItems[i].nItemID;
									pResultPacket->nCount[i] = VecDisjointItems[i].nCount;
#endif
									int nRotate = (int)(((VecDisjointItems[i].nSeed % 360) / (float)VecDisjointItems.size()) * i);
									RequestDropItem(STATIC_INSTANCE(CDnDropItem::s_dwUniqueCount)++, vPos, VecDisjointItems[i].nItemID,
										VecDisjointItems[i].nSeed, VecDisjointItems[i].nCount, nRotate, pSession->GetSessionID());
								}

								pSession->DelCoin(pItem->GetDisjointCost(), DBDNWorldDef::CoinChangeCode::DisjointTax, pSession->m_nClickedNpcID);
								pSession->GetEventSystem()->OnEvent(EventSystem::OnItemDisjoint, 1, EventSystem::ItemID, nLocalItemID);
							}
							else
								iResult = ERROR_ITEM_FAIL;		// 아이템 삭제가 안되는 이상한 에러.
						}
						else
						{
							iResult = ERROR_ITEM_INSUFFICIENCY_MONEY;
						}

						if (iResult != ERROR_NONE)
							break;
					}
				}
				else
					iResult = ERROR_ITEM_CAN_NOT_DISJOINT;
			}
			else
				iResult = ERROR_ITEM_CAN_NOT_DISJOINT;
		}
		else iResult = ERROR_ITEM_FAIL;
	}
	else
		iResult = ERROR_ITEM_NOTFOUND;		// 존재하지 않는 아이템을 분해하려고..
#ifdef _ADD_NEWDISJOINT
	pResultPacket->bSuccess = bSuccessDisjoint;
	// 아이템 분해 성공. 클라로 결과 보냄.
	if (ERROR_NONE == iResult)
		SendItemDisjointResNew(pSession, pResultPacket); //wtf
	else
		SendItemDisjointResNew(pSession, pResultPacket);		// 0 은 실패
#else
											// 아이템 분해 성공. 클라로 결과 보냄.
	if (ERROR_NONE == iResult)
		SendItemDisjointRes(pSession, nItemID, bSuccessDisjoint);
	else
		SendItemDisjointRes(pSession, nItemID, bSuccessDisjoint);		// 0 은 실패
#endif
	return iResult;
}
#endif
int CDnItemTask::OnRecvItemCompleteRandomItem( CDNUserSession* pSession, CSCompleteRandomItem *pPacket, int nLen )
{
	if (sizeof(CSCompleteRandomItem) != nLen)
		return ERROR_INVALIDPACKET;

	if( !pSession->GetItem()->IsValidRequestTimer( CDNUserItem::RequestType_UseRandomItem ) ) {
		// 나쁜놈.
		return ERROR_NONE;
	}

	pSession->GetItem()->CalcRandomItem( pPacket );

	//UseItem에서 WINDOW_PROGRESS상태로 변경되어 있음.
	pSession->IsWindowStateNoneSet(WINDOW_PROGRESS);

	return ERROR_NONE;
}

int CDnItemTask::OnRecvItemCancelRandomItem( CDNUserSession* pSession, char *pPacket, int nLen )
{
	if (nLen != 0)
		return ERROR_INVALIDPACKET;

	pSession->GetItem()->SetRequestTimer( CDNUserItem::RequestType_None, 0 );
	pSession->BroadcastingEffect( EffectType_Random, EffectState_Cancel );

	//UseItem에서 WINDOW_PROGRESS상태로 변경되어 있음.
	pSession->IsWindowStateNoneSet(WINDOW_PROGRESS);
	return ERROR_NONE;
}

int CDnItemTask::OnRecvItemSortInventory( CDNUserSession* pSession, CSSortInventory *pPacket, int nLen )
{
	if (sizeof(CSSortInventory) - sizeof(pPacket->SlotInfo) + (sizeof(TSortSlot) * pPacket->cTotalCount) != nLen)
		return ERROR_INVALIDPACKET;

	CDNGameRoom::PartyStruct *pStruct = GetPartyData( pSession );
	if (!pStruct) return ERROR_GENERIC_UNKNOWNERROR;

	if ((pSession->GetItem()->SortInventory(pPacket)) && SortInventory(pSession, pPacket)){
		pSession->SendSortInventory(ERROR_NONE);
	}
	else pSession->SendSortInventory(ERROR_GENERIC_INVALIDREQUEST);

	return ERROR_NONE;
}

int CDnItemTask::OnRecvItemEnchant( CDNUserSession* pSession, CSEnchantItem* pPacket, int nLen )
{
	return pSession->ParseEnchant( pPacket, nLen );
}

int CDnItemTask::OnRecvItemEnchantComplete( CDNUserSession* pSession, CSEnchantItem* pPacket, int nLen )
{
	return pSession->ParseEnchantComplete( pPacket, nLen );
}

int CDnItemTask::OnRecvItemEnchantCancel( CDNUserSession* pSession, int nLen )
{
	return pSession->ParseEnchantCancel( nLen );
}


int CDnItemTask::OnRecvItemCompoundOpenReq( CDNUserSession* pSession, CSItemCompoundOpenReq* pPacket, int nLen )
{
	return pSession->ParseItemCompound( pPacket, nLen );
}

int CDnItemTask::OnrecvItemCompoundReq( CDNUserSession* pSession, CSCompoundItemReq* pPacket, int nLen )
{
	return pSession->ParseItemCompoundComplete( pPacket, nLen );
}
int CDnItemTask::OnRecvItemCompoundCancelReq( CDNUserSession* pSession, CSItemCompoundCancelReq* pPacket, int nLen )
{
	return pSession->ParseItemCompoundCancel( pPacket, nLen );
}


void CDnItemTask::PickUpItem( DnActorHandle hActor, DnDropItemHandle hDropItem, TPARTYITEMLOOTRULE LootRule )
{
#if defined(_CH)
	CDNUserSession *pSession = NULL;
	if( hActor && hActor->IsPlayerActor() ) pSession = ((CDnPlayerActor*)hActor.GetPointer())->GetUserSession();
	if (pSession && (pSession->GetFCMState() != FCMSTATE_NONE)){	// 3시간 이상 게임하면 아이템 줏기 못함 090624
		return;
	}
#endif

	bool bPickup = true;

	// ItemID 가 0 이면 돈이다!!
	if( hDropItem->GetItemID() == 0 ) {
		if( hDropItem->GetOverlapCount() < 1 ) return;
		if( hDropItem->GetOverlapCount() < (int)GetPartyUserCount(CDNGameRoom::ePICKUPITEM) ) {
			if( hActor )
			{
				CDnPlayerActor *pPlayer = static_cast<CDnPlayerActor *>(hActor.GetPointer());
				if (!pPlayer->GetUserSession()->CheckMaxCoin(hDropItem->GetOverlapCount())){
					pPlayer->GetUserSession()->SendPickUp(ERROR_ITEM_OVERFLOWMONEY, -1, NULL, 0);
					return;
				}
				pPlayer->CmdAddCoin( hDropItem->GetOverlapCount(), DBDNWorldDef::CoinChangeCode::PickUp, 0, true );
			}
		}
		else 
		{
			int nLiveUserCount = GetPartyUserCount(CDNGameRoom::ePICKUPITEM);

			if( nLiveUserCount == 0 ) return;
			int nCoin = hDropItem->GetOverlapCount() / nLiveUserCount;
			int nDivide = hDropItem->GetOverlapCount() % nLiveUserCount;
			for( DWORD i=0; i<GetUserCount(); i++ ) {
				if( !GetUserData(i)->GetActorHandle() )
					continue;
				CDnPlayerActor *pPlayer = static_cast<CDnPlayerActor *>(GetUserData(i)->GetActorHandle().GetPointer());
				if( pPlayer->IsDie() )				continue;
				if( GetUserData(i)->bIsGMTrace() )	continue;
				CDNUserSession* pUserSession = GetUserData(i);
				if (pUserSession == NULL 
	#ifdef _CH
					|| pUserSession->GetFCMState() != FCMSTATE_NONE
	#endif
					)
					continue;

				int nDropCoin = 0;
				if( hActor == pPlayer ) {
					nDropCoin = nCoin + nDivide;
					if (!pPlayer->GetUserSession()->CheckMaxCoin(nCoin + nDivide)){
						pPlayer->GetUserSession()->SendPickUp(ERROR_ITEM_OVERFLOWMONEY, -1, NULL, 0);
						continue;
					}
					pPlayer->CmdAddCoin( nCoin + nDivide, DBDNWorldDef::CoinChangeCode::PickUp, 0, true );
				}
				else {
					if (!pPlayer->GetUserSession()->CheckMaxCoin(nCoin)){
						pPlayer->GetUserSession()->SendPickUp(ERROR_ITEM_OVERFLOWMONEY, -1, NULL, 0);
						continue;
					}

					pPlayer->CmdAddCoin( nCoin, DBDNWorldDef::CoinChangeCode::PickUp, 0, true );
				}
			}
		}
		hDropItem->Send( CDnDropItem::DPT_PICKUP, NULL );

		if( hActor && hActor->IsPlayerActor() ) 
			((CDnPlayerActor*)hActor.GetPointer())->UpdateGetItem();

		SAFE_RELEASE_SPTR( hDropItem );
		return;
	}

	// 여기서 아이템을 집어넣는거 체크 해주시고
	eItemTypeEnum Type = CDnItem::GetItemType( hDropItem->GetItemID() );
	if( Type == ITEMTYPE_INSTANT ) {
		int nItemID = hDropItem->GetItemID();
		int nSeed = hDropItem->GetRandomSeed();

		CDnItem *pItem = CDnItem::CreateItem( hActor->GetRoom(), nItemID, nSeed );
		if( pItem ) {
			pItem->ActivateSkillEffect( hActor );
			SAFE_DELETE( pItem );

			char szBuf[16] = { 0, };
			DWORD dwUniqueID = hActor->GetUniqueID();
			CMemoryStream Stream( szBuf, 16 );
			Stream.Write( &dwUniqueID, sizeof(DWORD) );
			Stream.Write( &nItemID, sizeof(int) );
			Stream.Write( &nSeed, sizeof(int) );

			hDropItem->Send( CDnDropItem::DPT_PICKUPINSTANT, &Stream );

			if( hActor && hActor->IsPlayerActor() ) 
				((CDnPlayerActor*)hActor.GetPointer())->UpdateGetItem();

			SAFE_RELEASE_SPTR( hDropItem );

			if( hActor && hActor->IsPlayerActor() )
				((CDnPlayerActor*)hActor.GetPointer())->GetUserSession()->GetEventSystem()->OnEvent( EventSystem::OnItemUse, 1,EventSystem::ItemID, nItemID );

		}
		else hDropItem->Send( CDnDropItem::DPT_NOPICKUP, NULL );
	}
	else {
		CDNUserSession *pSession = NULL;
		if( hActor && hActor->IsPlayerActor() ) pSession = ((CDnPlayerActor*)hActor.GetPointer())->GetUserSession();
		if( pSession == NULL ) return;
		int nItemID = hDropItem->GetItemID();
		short wCount = hDropItem->GetOverlapCount();
		int nRandomSeed = hDropItem->GetRandomSeed();

		TItemData *pItemData = g_pDataManager->GetItemData(nItemID);
		bool bProcessPickup = false;

		int itemShareEnableCount = GetPartyUserCount(CDNGameRoom::ePICKUPITEM);
		if (itemShareEnableCount > 1 && hDropItem->GetOwnerUniqueID() == -1 )
		{
			if (pItemData)
			{
				if (pItemData->cReversion == ITEMREVERSION_BELONG) // todo : Check the dice game history of dropitem.
				{
					if (hDropItem->IsReversionItem() == false)
					{
						hDropItem->SetReversionItem(true);
						hDropItem->LockReversionItem(true);
					}

					if (hDropItem->IsReversionLocked())
					{
						if (pSession->GetGameRoom()->IsEnableAddRequestGetReversionItem(hDropItem))
						{
							TItem itemInfo;
#if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
							pSession->GetItem()->MakeItemStruct(nItemID, itemInfo, 0, hDropItem->GetEnchantID());
#else	// #if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
							pSession->GetItem()->MakeItemStruct(nItemID, itemInfo);
#endif	// #if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
							hDropItem->MakeItemStruct( itemInfo );
							TItemData *pItemData = g_pDataManager->GetItemData(nItemID);
							if (pItemData)
							{
								if (pItemData->cReversion == ITEMREVERSION_BELONG)
									itemInfo.bSoulbound = pItemData->IsSealed ? false : true;
							}
							pSession->GetGameRoom()->AddRequestGetReversionItem(itemInfo, hDropItem);
						}

						bPickup = false;
						bProcessPickup = true;
					}
				}
			}
		}

		TItem pickupItemInfo;
		if (bProcessPickup == false)
		{
#if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
			bPickup = pSession->GetItem()->OnRecvPickUp(pickupItemInfo, hDropItem->GetItemID(), hDropItem->GetOverlapCount(), hDropItem->GetRandomSeed(), hDropItem->GetOption(), hDropItem->GetEnchantID());
#else	// #if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
			bPickup = pSession->GetItem()->OnRecvPickUp(pickupItemInfo, hDropItem->GetItemID(), hDropItem->GetOverlapCount(), hDropItem->GetRandomSeed(), hDropItem->GetOption());
#endif	// #if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
		}

		if( bPickup ) {
			// 실제로 아이템 먹은놈에게만 SC_ITEM 관련 메세지가 가고
			// [누가] [어떤 아아템]을 먹었습니다. 라는 메세지를 따로 보내기도 뭐하니
			// DropItem 에게 보낼때 정보를 가치 보내주도록 한다.
			char pBuffer[32];
			CPacketCompressStream Stream( pBuffer, 32 );

			bool bNameLink = true;
			Stream.Write(&bNameLink, sizeof(bool));
			//	item information for name link.
			Stream.Write(&pickupItemInfo.cLevel, sizeof(char));
			Stream.Write(&pickupItemInfo.nRandomSeed, sizeof(int));
			Stream.Write(&pickupItemInfo.wDur, sizeof(USHORT));
			Stream.Write(&pickupItemInfo.cPotential, sizeof(char));
			Stream.Write(&pickupItemInfo.cOption, sizeof(char));
			Stream.Write(&pickupItemInfo.cSealCount, sizeof(char));

			DWORD dwUniqueID = hActor->GetUniqueID();
			Stream.Write( &LootRule, sizeof(char) );
			switch( LootRule ) {
				case ITEMLOOTRULE_RANDOM:
				case ITEMLOOTRULE_NONE:
				case ITEMLOOTRULE_OWNER:
				case ITEMLOOTRULE_INORDER:
					Stream.Write( &dwUniqueID, sizeof(DWORD) );
					break;
				default:
					break;
			}
			hDropItem->Send( CDnDropItem::DPT_PICKUP, &Stream );

			if( hActor && hActor->IsPlayerActor() ) 
				((CDnPlayerActor*)hActor.GetPointer())->UpdateGetItem();

			SAFE_RELEASE_SPTR( hDropItem );
		}
		else hDropItem->Send( CDnDropItem::DPT_NOPICKUP, NULL );
	}
}
#if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
DnDropItemHandle CDnItemTask::RequestDropItem( DWORD dwUniqueID, EtVector3 &vPos, int nItemID, int nSeed, int nCount, short nRotate, UINT nOwnerSessionID, int nEchantID )
#else	// #if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
DnDropItemHandle CDnItemTask::RequestDropItem( DWORD dwUniqueID, EtVector3 &vPos, int nItemID, int nSeed, int nCount, short nRotate, UINT nOwnerSessionID)
#endif	// #if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
{
#if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
	DnDropItemHandle hDropItemHandle = CDnDropItem::DropItem( GetRoom(), vPos, dwUniqueID, nItemID, nSeed, -1, nCount, nRotate, nOwnerSessionID, nEchantID );
#else	// #if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
	DnDropItemHandle hDropItemHandle = CDnDropItem::DropItem( GetRoom(), vPos, dwUniqueID, nItemID, nSeed, -1, nCount, nRotate, nOwnerSessionID );
#endif	// #if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
	if( hDropItemHandle )
	{
		for( DWORD i=0; i<GetUserCount(); i++ ) 
		{
			CDNUserSession* pGameSession = GetUserData(i);
			if( pGameSession && pGameSession->GetState() == SESSION_STATE_GAME_PLAY )
				SendGameCreateDropItem( pGameSession, dwUniqueID, vPos, nItemID, nSeed, hDropItemHandle->GetOption(), nCount, nRotate, nOwnerSessionID );
		}
	}

	return hDropItemHandle;
}

CDnItem *CDnItemTask::CreateItem( const TItem *pInfo )
{
	int nItemID = pInfo->nItemID;
	eItemTypeEnum Type = CDnItem::GetItemType( nItemID );
	CDnItem *pItem = NULL;

	if( nItemID == 0 ) return NULL;
	switch( Type ) {
		case ITEMTYPE_WEAPON:
			pItem = CDnWeapon::CreateWeapon( GetRoom(), nItemID, pInfo->nRandomSeed, pInfo->cOption, pInfo->cLevel, pInfo->cPotential, false, false,  pInfo->cSealCount, pInfo->bSoulbound );
			break;
		case ITEMTYPE_PARTS:
			pItem = CDnParts::CreateParts( GetRoom(), nItemID, pInfo->nRandomSeed, pInfo->cOption, pInfo->cLevel, pInfo->cPotential, pInfo->cSealCount, pInfo->bSoulbound );
			break;
		case ITEMTYPE_GLYPH:
			pItem = CDnGlyph::CreateGlyph( GetRoom(), nItemID, pInfo->nRandomSeed, pInfo->cOption, pInfo->cLevel, pInfo->cPotential, pInfo->cSealCount, pInfo->bSoulbound );
			break;
#if defined(PRE_ADD_TALISMAN_SYSTEM)
		case ITEMTYPE_TALISMAN:
			pItem = CDnTalisman::CreateTalisman( GetRoom(), nItemID, pInfo->nRandomSeed, pInfo->cOption, pInfo->cLevel, pInfo->cPotential, pInfo->cSealCount, pInfo->bSoulbound );
			break;
#endif
		default:
			pItem = CDnItem::CreateItem( GetRoom(), nItemID, pInfo->nRandomSeed );
			break;
	}
	if( pItem == NULL ) {
#if defined (_WORK)
		GetRoom()->GetDBConnection()->QueryUseItemEx( GetRoom()->GetDBThreadID(), GetRoom()->GetWorldSetID(), 0, DBDNWorldDef::UseItem::Destroy, pInfo->nSerial, pInfo->wCount, 0, 0, L"", true );
#else
		_ASSERT( !"아이템 생성 실패" );
#endif // #if defined (_WORK)
		return NULL;
	}
	*pItem = *const_cast<TItem*>(pInfo);

	return pItem;
}

bool CDnItemTask::InsertEquipItem( CDNUserSession *pSession, int nEquipIndex, CDnItem *pItem, bool bGenerationEvent )
{
	CDNGameRoom::PartyStruct *pStruct = GetPartyData( pSession );
	if( pStruct == NULL ) return false;
	if( nEquipIndex < 0 || nEquipIndex >= EQUIPMAX ) return false;
	if( pStruct->pEquip[nEquipIndex] ) return false;	
	
#ifdef _DEBUG
	if (nEquipIndex >= EQUIPMAX) 
		_ASSERT(0);

	for (int i = 0; i < EQUIPMAX; i++)
		if (pStruct->pEquip[i] == pItem)
			_ASSERT(0);
#endif

	pStruct->pEquip[nEquipIndex] = pItem;

	if( bGenerationEvent ) {
		pSession->GetEventSystem()->OnEvent( EventSystem::OnItemEquip, 1,
			EventSystem::ItemID, pItem->GetClassID() );

		char cLevel = 99;
		for (int i = EQUIP_HELMET; i <= EQUIP_FOOT; i++){
			if (!pStruct->pEquip[i]) cLevel = 0;
			else if (cLevel > pStruct->pEquip[i]->GetEnchantLevel()) cLevel = pStruct->pEquip[i]->GetEnchantLevel();
			if (cLevel == 0) break;
		}
		if (cLevel > 0)
		{
			pSession->GetEventSystem()->OnEvent( EventSystem::OnItemEquip, 1,
				EventSystem::AllPartsMinLevel, cLevel );
		}
	}

	return true;
}

bool CDnItemTask::RemoveEquipItem( CDNUserSession *pSession, int nEquipIndex )
{
	CDNGameRoom::PartyStruct *pStruct = GetPartyData( pSession );
	if( pStruct == NULL ) return false;
	if( nEquipIndex < 0 || nEquipIndex >= EQUIPMAX ) return false;
	if( !pStruct->pEquip[nEquipIndex] ) return false;

	SAFE_DELETE( pStruct->pEquip[nEquipIndex] );
	return true;
}

CDnItem *CDnItemTask::GetEquipItem( CDNUserSession *pSession, int nEquipIndex )
{
	CDNGameRoom::PartyStruct *pStruct = GetPartyData( pSession );
	if( pStruct == NULL ) return NULL;
	if( nEquipIndex < 0 || nEquipIndex >= EQUIPMAX ) return NULL;
	return pStruct->pEquip[nEquipIndex];
}

bool CDnItemTask::InsertCashEquipItem( CDNUserSession * pSession, int nEquipIndex, CDnItem *pItem, bool bGenerationEvent )
{
	CDNGameRoom::PartyStruct *pStruct = GetPartyData( pSession );
	if( pStruct == NULL ) return false;
	if( nEquipIndex < 0 || nEquipIndex >= CASHEQUIPMAX ) return false;
	if( pStruct->pCashEquip[nEquipIndex] ) return false;	

#ifdef _DEBUG
	if (nEquipIndex >= CASHEQUIPMAX) 
		_ASSERT(0);

	for (int i = 0; i < CASHEQUIPMAX; i++)
		if (pStruct->pCashEquip[i] == pItem)
			_ASSERT(0);
#endif

	pStruct->pCashEquip[nEquipIndex] = pItem;

	return true;
}

bool CDnItemTask::RemoveCashEquipItem( CDNUserSession * pSession, int nEquipIndex )
{
	CDNGameRoom::PartyStruct *pStruct = GetPartyData( pSession );
	if( pStruct == NULL ) return false;
	if( nEquipIndex < 0 || nEquipIndex >= CASHEQUIPMAX ) return false;
	if( !pStruct->pCashEquip[nEquipIndex] ) return false;

	SAFE_DELETE( pStruct->pCashEquip[nEquipIndex] );
	return true;
}

CDnItem *CDnItemTask::GetCashEquipItem( CDNUserSession * pSession, int nEquipIndex )
{
	CDNGameRoom::PartyStruct *pStruct = GetPartyData( pSession );
	if( pStruct == NULL ) return NULL;
	if( nEquipIndex < 0 || nEquipIndex >= CASHEQUIPMAX ) return NULL;
	return pStruct->pCashEquip[nEquipIndex];
}

bool CDnItemTask::InsertGlyphItem( CDNUserSession * pSession, int nEquipIndex, CDnItem *pItem, bool bGenerationEvent )
{
	CDNGameRoom::PartyStruct *pStruct = GetPartyData( pSession );
	if( pStruct == NULL ) return false;
	if( nEquipIndex < 0 || nEquipIndex >= GLYPHMAX ) return false;
	if( pStruct->pGlyph[nEquipIndex] ) return false;	

#ifdef _DEBUG
	if (nEquipIndex >= GLYPHMAX) 
		_ASSERT(0);

	for (int i = 0; i < GLYPHMAX; i++)
		if (pStruct->pGlyph[i] == pItem)
			_ASSERT(0);
#endif

	pStruct->pGlyph[nEquipIndex] = pItem;

	return true;
}

bool CDnItemTask::RemoveGlyphItem( CDNUserSession * pSession, int nEquipIndex )
{
	CDNGameRoom::PartyStruct *pStruct = GetPartyData( pSession );
	if( pStruct == NULL ) return false;
	if( nEquipIndex < 0 || nEquipIndex >= GLYPHMAX ) return false;
	if( !pStruct->pGlyph[nEquipIndex] ) return false;

	SAFE_DELETE( pStruct->pGlyph[nEquipIndex] );
	return true;
}

CDnItem *CDnItemTask::GetGlyphItem( CDNUserSession * pSession, int nEquipIndex )
{
	CDNGameRoom::PartyStruct *pStruct = GetPartyData( pSession );
	if( pStruct == NULL ) return NULL;
	if( nEquipIndex < 0 || nEquipIndex >= GLYPHMAX ) return NULL;
	return pStruct->pGlyph[nEquipIndex];
}

#if defined(PRE_ADD_TALISMAN_SYSTEM)
bool CDnItemTask::InsertTalismanItem( CDNUserSession * pSession, int nEquipIndex, CDnItem *pItem )
{
	CDNGameRoom::PartyStruct *pStruct = GetPartyData( pSession );
	if( pStruct == NULL ) return false;
	if( nEquipIndex < 0 || nEquipIndex >= TALISMAN_MAX ) return false;
	if( pStruct->pTalisman[nEquipIndex] ) return false;	

#ifdef _DEBUG
	if (nEquipIndex >= TALISMAN_MAX) 
		_ASSERT(0);

	for (int i = 0; i < TALISMAN_MAX; i++)
		if (pStruct->pTalisman[i] == pItem)
			_ASSERT(0);
#endif

	pStruct->pTalisman[nEquipIndex] = pItem;

	return true;
}

bool CDnItemTask::RemoveTalismanItem( CDNUserSession * pSession, int nEquipIndex )
{
	CDNGameRoom::PartyStruct *pStruct = GetPartyData( pSession );
	if( pStruct == NULL ) return false;
	if( nEquipIndex < 0 || nEquipIndex >= TALISMAN_MAX ) return false;
	if( !pStruct->pTalisman[nEquipIndex] ) return false;

	SAFE_DELETE( pStruct->pTalisman[nEquipIndex] );
	return true;
}

CDnItem *CDnItemTask::GetTalismanItem( CDNUserSession * pSession, int nEquipIndex )
{
	CDNGameRoom::PartyStruct *pStruct = GetPartyData( pSession );
	if( pStruct == NULL ) return NULL;
	if( nEquipIndex < 0 || nEquipIndex >= TALISMAN_MAX ) return NULL;
	return pStruct->pTalisman[nEquipIndex];
}
#endif

bool CDnItemTask::InsertInventoryItem( CDNUserSession *pSession, int nSlotIndex, CDnItem *pItem )
{
	CDNGameRoom::PartyStruct *pStruct = GetPartyData( pSession );
	if( pStruct == NULL ) return false;
	if( nSlotIndex < 0 || nSlotIndex >= INVENTORYMAX ) return false;

	if( pStruct->pInventory[nSlotIndex] ) {
		SAFE_DELETE( pStruct->pInventory[nSlotIndex] );
	}

#ifdef _DEBUG
	if (nSlotIndex >= INVENTORYMAX)
		_ASSERT(0);

	for (int i = 0; i < INVENTORYMAX; i++)
		if (pStruct->pInventory[i] == pItem)
			_ASSERT(0);
#endif

	pStruct->pInventory[nSlotIndex] = pItem;
	return true;
}

bool CDnItemTask::RemoveInventoryItem( CDNUserSession *pSession, int nSlotIndex )
{
	CDNGameRoom::PartyStruct *pStruct = GetPartyData( pSession );
	if( pStruct == NULL ) return false;
	if( nSlotIndex < 0 || nSlotIndex >= INVENTORYMAX ) return false;

	if( pStruct->pInventory[nSlotIndex] ) {
		SAFE_DELETE( pStruct->pInventory[nSlotIndex] );
		return true;
	}
	return false;
}

CDnItem *CDnItemTask::GetInventoryItem( CDNUserSession *pSession, int nSlotIndex )
{
	CDNGameRoom::PartyStruct *pStruct = GetPartyData( pSession );
	if( pStruct == NULL ) return NULL;
	if( nSlotIndex < 0 || nSlotIndex >= INVENTORYMAX ) return NULL;
	return pStruct->pInventory[nSlotIndex];
}

bool CDnItemTask::InsertCashInventoryItem( CDNUserSession * pSession, INT64 biItemSerial, CDnItem *pItem )
{
	CDNGameRoom::PartyStruct *pStruct = GetPartyData( pSession );
	if( pStruct == NULL ) return false;
	if (biItemSerial <= 0) return false;

	std::map<INT64, CDnItem*>::iterator iter = pStruct->pMapCashInventory.find(biItemSerial);
	if (iter != pStruct->pMapCashInventory.end()){
		SAFE_DELETE(iter->second);
	}

	pStruct->pMapCashInventory[biItemSerial] = pItem;

	return true;
}

bool CDnItemTask::RemoveCashInventoryItem( CDNUserSession * pSession, INT64 biItemSerial )
{
	CDNGameRoom::PartyStruct *pStruct = GetPartyData( pSession );
	if( pStruct == NULL ) return false;
	if (biItemSerial <= 0) return NULL;

	std::map<INT64, CDnItem*>::iterator iter = pStruct->pMapCashInventory.find(biItemSerial);
	if (iter != pStruct->pMapCashInventory.end()){
		SAFE_DELETE(iter->second);
		pStruct->pMapCashInventory.erase(iter);
		return true;
	}

	return false;
}

CDnItem *CDnItemTask::GetCashInventoryItem( CDNUserSession * pSession, INT64 biItemSerial )
{
	CDNGameRoom::PartyStruct *pStruct = GetPartyData( pSession );
	if( pStruct == NULL ) return NULL;
	if (biItemSerial <= 0) return NULL;

	std::map<INT64, CDnItem*>::iterator iter = pStruct->pMapCashInventory.find(biItemSerial);
	if (iter == pStruct->pMapCashInventory.end()) return NULL;
	return iter->second;
}

bool CDnItemTask::InsertVehicleEquipItem( CDNUserSession * pSession, int nEquipIndex, CDnItem *pItem, bool bGenerationEvent/* = true*/ )
{
	CDNGameRoom::PartyStruct *pStruct = GetPartyData( pSession );
	if( pStruct == NULL ) return false;
	if( nEquipIndex < Vehicle::Slot::Body || nEquipIndex >= Vehicle::Slot::Max ) return false;
	if( pStruct->pVehicleEquip[nEquipIndex] ) return false;	

#ifdef _DEBUG
	if (nEquipIndex >= Vehicle::Slot::Max) 
		_ASSERT(0);

	for (int i = 0; i < Vehicle::Slot::Max; i++)
		if (pStruct->pVehicleEquip[i] == pItem)
			_ASSERT(0);
#endif

	pStruct->pVehicleEquip[nEquipIndex] = pItem;

	return true;
}

bool CDnItemTask::RemoveVehicleEquipItem( CDNUserSession * pSession, int nEquipIndex )
{
	CDNGameRoom::PartyStruct *pStruct = GetPartyData( pSession );
	if( pStruct == NULL ) return false;
	if( nEquipIndex < Vehicle::Slot::Body || nEquipIndex >= Vehicle::Slot::Max ) return false;
	if( !pStruct->pVehicleEquip[nEquipIndex] ) return false;

	SAFE_DELETE( pStruct->pVehicleEquip[nEquipIndex] );
	return true;
}

CDnItem *CDnItemTask::GetVehicleEquipItem( CDNUserSession * pSession, int nEquipIndex )
{
	CDNGameRoom::PartyStruct *pStruct = GetPartyData( pSession );
	if( pStruct == NULL ) return NULL;
	if( nEquipIndex < Vehicle::Slot::Body || nEquipIndex >= Vehicle::Slot::Max ) return false;
	return pStruct->pVehicleEquip[nEquipIndex];
}

bool CDnItemTask::InsertVehicleInventoryItem( CDNUserSession * pSession, INT64 biItemSerial, CDnItem *pItem )
{
	CDNGameRoom::PartyStruct *pStruct = GetPartyData( pSession );
	if( pStruct == NULL ) return false;
	if (biItemSerial <= 0) return false;

	std::map<INT64, CDnItem*>::iterator iter = pStruct->pMapVehicleInventory.find(biItemSerial);
	if (iter != pStruct->pMapVehicleInventory.end()){
		SAFE_DELETE(iter->second);
	}

	pStruct->pMapVehicleInventory[biItemSerial] = pItem;

	return true;
}

bool CDnItemTask::RemoveVehicleInventoryItem( CDNUserSession * pSession, INT64 biItemSerial )
{
	CDNGameRoom::PartyStruct *pStruct = GetPartyData( pSession );
	if( pStruct == NULL ) return false;
	if (biItemSerial <= 0) return NULL;

	std::map<INT64, CDnItem*>::iterator iter = pStruct->pMapVehicleInventory.find(biItemSerial);
	if (iter != pStruct->pMapVehicleInventory.end()){
		SAFE_DELETE(iter->second);
		pStruct->pMapVehicleInventory.erase(iter);
		return true;
	}

	return false;
}

CDnItem *CDnItemTask::GetVehicleInventoryItem( CDNUserSession * pSession, INT64 biItemSerial )
{
	CDNGameRoom::PartyStruct *pStruct = GetPartyData( pSession );
	if( pStruct == NULL ) return NULL;
	if (biItemSerial <= 0) return NULL;

	std::map<INT64, CDnItem*>::iterator iter = pStruct->pMapVehicleInventory.find(biItemSerial);
	if (iter == pStruct->pMapVehicleInventory.end()) return NULL;
	return iter->second;
}

bool CDnItemTask::SortInventory( CDNUserSession *pSession, CSSortInventory *pSort)
{
	CDNGameRoom::PartyStruct *pStruct = GetPartyData( pSession );
	if( pStruct == NULL ) return false;

	// 정말 하기싫은 정렬이구나 ;ㅅ;
	CDnItem* pNewInventory[INVENTORYMAX] = { 0, };

	for (int i = 0; i < pSort->cTotalCount; i++){
		if (pSort->SlotInfo[i].cCurrent < 0) continue;
		pNewInventory[pSort->SlotInfo[i].cNew] = pStruct->pInventory[pSort->SlotInfo[i].cCurrent];
	}

	for (int i = 0; i < INVENTORYMAX; i++){
		pStruct->pInventory[i] = pNewInventory[i]; 
	}
	return true;
}

bool CDnItemTask::InitializePlayerItem( CDNUserSession* pSession )
{
	const TItem *pItemData;
	CDnItem *pItem;
	for( int j=0; j<EQUIPMAX; j++ ) 
	{
		pItemData = pSession->GetItem()->GetEquip( j );
		if( !pItemData || pItemData->nItemID == 0 ) 
			continue;
		pItem = CreateItem( pItemData );
		if( !pItem ) 
		{
			_ASSERT(0);
			continue;
		}
		InsertEquipItem( pSession, j, pItem, false );
	}

	for( int j=0; j<CASHEQUIPMAX; j++ ) 
	{
		pItemData = pSession->GetItem()->GetCashEquip( j );
		if( !pItemData || pItemData->nItemID == 0 ) 
			continue;
		pItem = CreateItem( pItemData );
		if( !pItem ) 
		{
			_ASSERT(0);
			continue;
		}
		InsertCashEquipItem( pSession, j, pItem, false );
	}

	for( int j=0; j<GLYPHMAX; j++ ) {
		pItemData = pSession->GetItem()->GetGlyph(j);
		if( !pItemData || pItemData->nItemID == 0 ) 
			continue;
		pItem = CreateItem( pItemData );
		if( !pItem ) 
		{
			_ASSERT(0);
			continue;
		}
		InsertGlyphItem( pSession, j, pItem, false );
	}

#if defined(PRE_ADD_TALISMAN_SYSTEM)
	for( int j=0; j<TALISMAN_MAX; j++ ) {
		pItemData = pSession->GetItem()->GetTalisman(j);
		if( !pItemData || pItemData->nItemID == 0 ) 
			continue;
		pItem = CreateItem( pItemData );
		if( !pItem ) 
		{
			_ASSERT(0);
			continue;
		}
		InsertTalismanItem( pSession, j, pItem );
	}
#endif

	for( int j=0; j<INVENTORYMAX; j++ ) 
	{
		pItemData = pSession->GetItem()->GetInventory( j );
		if( !pItemData || pItemData->nItemID == 0 ) 
			continue;
		pItem = CreateItem( pItemData );
		if( !pItem ) 
		{
#if defined( _WORK )
			const_cast<TItem*>(pItemData)->nItemID = 0;
#else
			_ASSERT(0);
#endif // #if defined( _WORK )
			continue;
		}
		InsertInventoryItem( pSession, j, pItem );
	}

	pSession->GetItem()->InitializePlayerCashItem(this);

	return true;
}

bool CDnItemTask::InitializePlayerItem()
{
	const TItem *pItemData;
	CDnItem *pItem;
	CDNUserSession *pSession;
	for( DWORD i=0; i<GetUserCount(); i++ ) {
		pSession = GetUserData(i);

		for( int j=0; j<EQUIPMAX; j++ ) {
			pItemData = pSession->GetItem()->GetEquip(j);
			if( !pItemData || pItemData->nItemID == 0 ) continue;
			pItem = CreateItem( pItemData );
			if( !pItem ) {
				_ASSERT(0);
				continue;
			}
			InsertEquipItem( pSession, j, pItem );
		}

		for( int j=0; j<CASHEQUIPMAX; j++ ) {
			pItemData = pSession->GetItem()->GetCashEquip(j);
			if( !pItemData || pItemData->nItemID == 0 ) continue;
			pItem = CreateItem( pItemData );
			if( !pItem ) {
				_ASSERT(0);
				continue;
			}
			InsertCashEquipItem( pSession, j, pItem );
		}

		for( int j=0; j<GLYPHMAX; j++ ) {
			pItemData = pSession->GetItem()->GetGlyph(j);
			if( !pItemData || pItemData->nItemID == 0 ) continue;
			pItem = CreateItem( pItemData );
			if( !pItem ) {
				_ASSERT(0);
				continue;
			}
			InsertGlyphItem( pSession, j, pItem );
		}

#if defined(PRE_ADD_TALISMAN_SYSTEM)
		for( int j=0; j<TALISMAN_MAX; j++ ) {
			pItemData = pSession->GetItem()->GetTalisman(j);
			if( !pItemData || pItemData->nItemID == 0 ) continue;
			pItem = CreateItem( pItemData );
			if( !pItem ) {
				_ASSERT(0);
				continue;
			}
			InsertTalismanItem( pSession, j, pItem );			
		}
#endif

		for( int j=0; j<INVENTORYMAX; j++ ) {
			pItemData = pSession->GetItem()->GetInventory(j);
			if( !pItemData || pItemData->nItemID == 0 ) 
				continue;
			pItem = CreateItem( pItemData );
			if( !pItem ) {
				_ASSERT(0);
				continue;
			}
			InsertInventoryItem( pSession, j, pItem );
		}

		pSession->GetItem()->InitializePlayerCashItem(this);
	}
	return true;
}

int CDnItemTask::ScanItemFromID( DnActorHandle hActor, int nItemTableID, std::vector<CDnItem *> *pVecResult )
{
	for( DWORD i=0; i<GetUserCount(); i++ ) {
		if( GetUserData(i)->GetActorHandle() == hActor ) {
			int nCount = 0;
			CDNGameRoom::PartyStruct *pStruct = GetPartyData(i);
			if( pStruct == NULL ) continue;
			//CDNSession::PartyStruct *pStruct = GetPartyData(i);
			for( DWORD j=0; j<INVENTORYMAX; j++ ) {
				CDnItem *pItem = pStruct->pInventory[j];
				if( pItem == NULL ) continue;
				if( pItem->GetClassID() == nItemTableID ) {
					if( pVecResult ) pVecResult->push_back( pItem );
					nCount += pItem->GetOverlapCount();
				}
			}
			return nCount;
		}
	}
	return 0;
}


void CDnItemTask::RequestRebirthCoinUseAnyPlayer( CDNUserSession *pUser, CDNUserSession *pTarget )
{
	// 부활가능한지 검사
	if( !_bIsRebirth() )
		return;

	DnActorHandle hActor = pTarget->GetActorHandle();

	if (pUser->GetRebirthCashCoin() > 0){
		pUser->DelCashRebirthCoin(1);

		// Note : 이벤토리에서 부활 코인이 남아 있는지 확인한다.
		//	남아 있다면 코인을 하나 소모하고 부활 상태효과를 추가한다.
		if( hActor )
		{
			hActor->CmdAddStateEffect( NULL, STATE_BLOW::BLOW_057, 5000, "Coin" );
		}

		pUser->GetEventSystem()->OnEvent( EventSystem::OnRebirthAny );
	}
	else
	{
		_ASSERT(0);
		return;
	}

	if (GetRoom()->GetPartyData(pTarget)->nUsableRebirthCoin > 0){
		GetRoom()->GetPartyData(pTarget)->nUsableRebirthCoin -= 1;
		if (GetRoom()->GetPartyData(pTarget)->nUsableRebirthCoin <= 0) GetRoom()->GetPartyData(pTarget)->nUsableRebirthCoin = 0;
	}

	pUser->SendRebirthCoin(ERROR_NONE, GetPartyData(pUser)->nUsableRebirthCoin, _REBIRTH_REBIRTHEE, pTarget->GetSessionID());
	pTarget->SendRebirthCoin(ERROR_NONE, GetPartyData(pTarget)->nUsableRebirthCoin, _REBIRTH_REBIRTHER, pUser->GetSessionID());
}

int CDnItemTask::FindInventoryItemFromItemType( CDNUserSession * pSession, eItemTypeEnum Type, std::vector<CDnItem *> &pVecResult )
{
	CDNGameRoom::PartyStruct *pStruct = GetPartyData( pSession );
	if( !pStruct ) return 0;
	for( int i=0; i<INVENTORYMAX; i++ ) {
		if( !pStruct->pInventory[i] ) continue;
		if( pStruct->pInventory[i]->GetItemType() == Type )
			pVecResult.push_back( pStruct->pInventory[i] );
	}

	return (int)pVecResult.size();
}

int CDnItemTask::FindCashInventoryItemFromItemType( CDNUserSession * pSession, eItemTypeEnum Type, std::vector<CDnItem *> &pVecResult )
{
	CDNGameRoom::PartyStruct *pStruct = GetPartyData( pSession );
	if( !pStruct ) return 0;

	std::map<INT64, CDnItem*>::iterator iter;
	for (iter = pStruct->pMapCashInventory.begin(); iter != pStruct->pMapCashInventory.end(); ++iter){
		if (iter->second->GetItemType() == Type)
			pVecResult.push_back( iter->second );
	}

	return (int)pVecResult.size();
}

int CDnItemTask::OnRecvItemModitemExpireDate( CDNUserSession *pSession, CSModItemExpireDate *pPacket, int nLen )
{
	//현재 게임서버에서는 사용불가임(클라이언트에서 막혀있음. 확인 후  사용 가능해야 한다면 리턴문 삭제)
	return ERROR_NONE;

	if (sizeof(CSModItemExpireDate) != nLen)
		return ERROR_INVALIDPACKET;			

	if( !pSession->GetActorHandle() )
		return ERROR_NONE;

	// Note 한기: 죽은 상태에선 아이템 사용 불가. 물약 같은 거 못 먹음.
	if( pSession->GetActorHandle()->IsDie() )
		return ERROR_NONE;

	CDNUserItem *pItem = pSession->GetItem();
	if(!pItem)
		return ERROR_NONE;	

	const TItem *pExpendItem = pSession->GetItem()->GetCashInventory(pPacket->biExpireDateItemSerial);
	bool bFlag = false;		//사용 성공인지 실패인지 체크

	if(pExpendItem)
	{
		TItemData *pExpendItemData = g_pDataManager->GetItemData(pExpendItem->nItemID);				
		
		if(pExpendItemData)
		{
			int nMin = 0;
			//기간연장 아이템 타입에 따라 대상 아이템 가져오는 위치가 달라짐
			switch(pExpendItemData->nType)
			{						
				case ITEMTYPE_PET_EXPIRE:
					{
						const TVehicle *pPet = pItem->GetVehicleInventory(pPacket->biItemSerial);
						if(pPet)
						{
							TItemData *pPetItemData = g_pDataManager->GetItemData( pPet->Vehicle[Pet::Slot::Body].nItemID );
							if( pPetItemData && pPetItemData->nType == ITEMTYPE_PET )
							{	
								nMin = pExpendItemData->nTypeParam[0] * 24 * 60;
								bFlag = true;
							}
						}
					}
					break;
				default:
					break;
			}

			if(bFlag)
			{
				pItem->ModItemExpireDate(pPacket, nMin);
				pItem->DeleteCashInventoryBySerial(pPacket->biExpireDateItemSerial,1,false); //기간연장아이템 제거함 위ModItemExpireDate() 함수 안에서 아이템 사용하는 쿼리도 실행됨.									
			}
		}
	}				
	return ERROR_NONE;
}

int CDnItemTask::OnRecvItemDeletePetSkill( CDNUserSession *pSession, CSPetSkillDelete *pPacket, int nLen )
{
	if (sizeof(CSPetSkillDelete) != nLen)
		return ERROR_INVALIDPACKET;

	CDNUserItem *pItem = pSession->GetItem();
	if(!pItem)
		return ERROR_NONE;	

	int nRet = pItem->DelPetSkill(pPacket->cSlotNum);
	pSession->SendPetSkillDelete(nRet, pPacket->cSlotNum);
	return ERROR_NONE;
}
