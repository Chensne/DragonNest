#pragma once

#include "Task.h"
#include "DnItem.h"
#include "DnParts.h"
#include "GameListener.h"
#include "MultiSingleton.h"
#include "DNGameRoom.h"

class CDnEmblemFactory;

class CDnItemTask : public CTask, public CMultiSingleton<CDnItemTask, MAX_SESSION_COUNT>, public CGameListener, public TBoostMemoryPool< CDnItemTask >
{
public:
	CDnItemTask(CDNGameRoom * pRoom);
	virtual ~CDnItemTask();

	//CDnEmblemFactory* m_pEmblemFactory;


protected:
	int OnRecvItemMessage( CDNUserSession * pSession, int nSubCmd, char *pData, int nLen );
	int OnRecvItemGoodsMessage( CDNUserSession * pSession, int nSubCmd, char *pData, int nLen );

public:
	bool Initialize();

	virtual void Process( LOCAL_TIME LocalTime, float fDelta );

	virtual int OnDispatchMessage( CDNUserSession * pSession, int nMainCmd, int nSubCmd, char *pData, int nLen );
	
	virtual int OnRecvItemMoveItem( CDNUserSession * pSession, CSMoveItem *pPacket, int nLen );
	virtual int OnRecvItemMoveCashItem( CDNUserSession * pSession, CSMoveCashItem *pPacket, int nLen );
	virtual int OnRecvItemRemoveItem( CDNUserSession * pSession, CSRemoveItem *pPacket, int nLen );
	virtual int OnRecvItemPickupItem( CDNUserSession * pSession, SCPickUp *pPacket, int nLen );
	virtual int OnRecvItemUseItem( CDNUserSession *pSession, CSUseItem *pPacket, int nLen );
	//virtual int OnRecvCompoundEmblem( CDNUserSession* pSession, CSCompoundEmblemReq* pPacket );		// 게임 서버에선 쓰지 않습니다..
	//virtual int OnRecvUpgradeJewel( CDNUserSession* pSession, CSUpgradeJewelReq* pPacket );
	virtual int OnRecvRebirth( CDNUserSession* pSession, int nLen );
	virtual int OnRecvRebirthCoin( CDNUserSession* pSession, int nLen );
	virtual int OnRecvItemRefreshQItem( CDNUserSession *pSession, SCRefreshQuestInven *pPacket, int nLen );
	virtual int OnRecvItemRemoveQItem( CDNUserSession *pSession, SCRemoveItem *pPacket, int nLen );
	virtual int OnRecvItemRefreshInven( CDNUserSession *pSession, SCRefreshInven *pPacket, int nLen );
	virtual int OnRecvItemRefreshCashInven( CDNUserSession *pSession, SCRefreshCashInven *pPacket, int nLen );
	virtual int OnRecvItemRefreshEquip( CDNUserSession *pSession, SCRefreshEquip *pPacket, int nLen );
	virtual int OnRecvItemWarehouseSort(CDNUserSession *pSession, CSSortWarehouse * pPacket, int nLen);
#ifdef _ADD_NEWDISJOINT
	virtual int OnRecvItemDisjoint( CDNUserSession* pSession, CSItemDisjointReqNew* pPacket, int nLen );
#else
	virtual int OnRecvItemDisjoint(CDNUserSession* pSession, CSItemDisjointReq* pPacket, int nLen);
#endif
	virtual int OnRecvItemCompleteRandomItem( CDNUserSession* pSession, CSCompleteRandomItem *pPacket, int nLen );
	virtual int OnRecvItemCancelRandomItem( CDNUserSession* pSession, char *pPacket, int nLen );
	virtual int OnRecvItemSortInventory( CDNUserSession* pSession, CSSortInventory *pPacket, int nLen );
	virtual int OnRecvItemEnchant( CDNUserSession* pSession, CSEnchantItem* pPacket, int nLen );
	virtual int OnRecvItemEnchantComplete( CDNUserSession* pSession, CSEnchantItem* pPacket, int nLen );
	virtual int OnRecvItemEnchantCancel( CDNUserSession* pSession, int nLen );
	virtual int OnRecvItemCompoundOpenReq( CDNUserSession* pSession, CSItemCompoundOpenReq* pPacket, int nLen );
	virtual int OnrecvItemCompoundReq( CDNUserSession* pSession, CSCompoundItemReq* pPacket, int nLen );
	virtual int OnRecvItemCompoundCancelReq( CDNUserSession* pSession, CSItemCompoundCancelReq* pPacket, int nLen );
	virtual int OnRecvItemModitemExpireDate( CDNUserSession *pSession, CSModItemExpireDate *pPacket, int nLen );
	virtual int OnRecvItemDeletePetSkill( CDNUserSession *pSession, CSPetSkillDelete *pPacket, int nLen );

	void PickUpItem( DnActorHandle hActor, DnDropItemHandle hDropItem, TPARTYITEMLOOTRULE LootRule );

	// 아이템을 바닥에 떨어뜨릴때 보내는것 위에 RecvDropItem 은 아이템을 인벤에서 버릴때 받는것임.. 해깔려잉 -> 그래서 휴지통에 버리는건 RemoveItem으로 수정했음 ㅎㅎ
#if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
	DnDropItemHandle RequestDropItem( DWORD dwUniqueID, EtVector3 &vPos, int nItemID, int nSeed, int nCount, short nRotate, UINT nOwnerSessionID = -1, int nEchantID = 0 );
#else	// #if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
	DnDropItemHandle RequestDropItem( DWORD dwUniqueID, EtVector3 &vPos, int nItemID, int nSeed, int nCount, short nRotate, UINT nOwnerSessionID = -1 );
#endif	// #if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)

	bool InitializePlayerItem( CDNUserSession* pSession );
	bool InitializePlayerItem();
	CDnItem *CreateItem( const TItem *pInfo );

	bool InsertEquipItem( CDNUserSession * pSession, int nEquipIndex, CDnItem *pItem, bool bGenerationEvent = true );
	bool RemoveEquipItem( CDNUserSession * pSession, int nEquipIndex );
	CDnItem *GetEquipItem( CDNUserSession * pSession, int nEquipIndex );

	bool InsertCashEquipItem( CDNUserSession * pSession, int nEquipIndex, CDnItem *pItem, bool bGenerationEvent = true );
	bool RemoveCashEquipItem( CDNUserSession * pSession, int nEquipIndex );
	CDnItem *GetCashEquipItem( CDNUserSession * pSession, int nEquipIndex );

	bool InsertGlyphItem( CDNUserSession * pSession, int nEquipIndex, CDnItem *pItem, bool bGenerationEvent = true );
	bool RemoveGlyphItem( CDNUserSession * pSession, int nEquipIndex );
	CDnItem *GetGlyphItem( CDNUserSession * pSession, int nEquipIndex );

#if defined(PRE_ADD_TALISMAN_SYSTEM)
	bool InsertTalismanItem ( CDNUserSession * pSession, int nEquipIndex, CDnItem *pItem );
	bool RemoveTalismanItem( CDNUserSession * pSession, int nEquipIndex );
	CDnItem *GetTalismanItem( CDNUserSession * pSession, int nEquipIndex );
#endif

	bool InsertInventoryItem( CDNUserSession * pSession, int nSlotIndex, CDnItem *pItem );
	bool RemoveInventoryItem( CDNUserSession * pSession, int nSlotIndex );
	CDnItem *GetInventoryItem( CDNUserSession * pSession, int nSlotIndex );

	bool InsertCashInventoryItem( CDNUserSession * pSession, INT64 biItemSerial, CDnItem *pItem );
	bool RemoveCashInventoryItem( CDNUserSession * pSession, INT64 biItemSerial );
	CDnItem *GetCashInventoryItem( CDNUserSession * pSession, INT64 biItemSerial );

	bool InsertVehicleEquipItem( CDNUserSession * pSession, int nEquipIndex, CDnItem *pItem, bool bGenerationEvent = true );
	bool RemoveVehicleEquipItem( CDNUserSession * pSession, int nEquipIndex );
	CDnItem *GetVehicleEquipItem( CDNUserSession * pSession, int nEquipIndex );

	bool InsertVehicleInventoryItem( CDNUserSession * pSession, INT64 biItemSerial, CDnItem *pItem );
	bool RemoveVehicleInventoryItem( CDNUserSession * pSession, INT64 biItemSerial );
	CDnItem *GetVehicleInventoryItem( CDNUserSession * pSession, INT64 biItemSerial );

	bool SortInventory( CDNUserSession *pSession, CSSortInventory *pSort);
	int ScanItemFromID( DnActorHandle hActor, int nItemTableID, std::vector<CDnItem *> *pVecResult );

	void RequestRebirthCoinUseAnyPlayer( CDNUserSession *pUser, CDNUserSession *pTarget );

	int FindInventoryItemFromItemType( CDNUserSession * pSession, eItemTypeEnum Type, std::vector<CDnItem *> &pVecResult );
	int FindCashInventoryItemFromItemType( CDNUserSession * pSession, eItemTypeEnum Type, std::vector<CDnItem *> &pVecResult );

private:

	bool	_bIsRebirth();

	CDnItem* CDnItemTask::FindRebirthItem(CDNUserSession* pSession, int nAllowMapType, int nItemID);
};
