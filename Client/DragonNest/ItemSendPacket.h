#pragma once

#include "DNPacket.h"
#include "ClientSessionManager.h"
#include "ClientSocket.h"

// ITEM -----------------------------------------------------------------------------

inline void SendMoveItem(char cMoveType, char cSrcIndex, INT64 biSrcItemSerial, BYTE cDestIndex, short wCount)
{
	CSMoveItem MoveItem;
	memset(&MoveItem, 0, sizeof(CSMoveItem));

	MoveItem.cMoveType = cMoveType;
	MoveItem.cSrcIndex = cSrcIndex;
	MoveItem.biSrcItemSerial = biSrcItemSerial;
	MoveItem.cDestIndex = cDestIndex;
	MoveItem.wCount = wCount;

	CClientSessionManager::GetInstance().SendPacket(CS_ITEM, eItem::CS_MOVEITEM, (char*)&MoveItem, sizeof(CSMoveItem));
}

inline void SendRemoveItem(char cSlotType, char cSlotIndex, short wCount, INT64 biItemSerial)
{
	CSRemoveItem RemoveItem;
	memset(&RemoveItem, 0, sizeof(CSRemoveItem));

	RemoveItem.cType = cSlotType;
	RemoveItem.cSlotIndex = cSlotIndex;
	RemoveItem.wCount = wCount;
	RemoveItem.biItemSerial = biItemSerial;

	CClientSessionManager::GetInstance().SendPacket(CS_ITEM, eItem::CS_REMOVEITEM, (char*)&RemoveItem, sizeof(CSRemoveItem));
}

inline void SendMoveCashItem(char cMoveType, BYTE cCashEquipIndex, INT64 biEquipSerial, INT64 biCashInvenSerial, short wCount)
{
	CSMoveCashItem MoveCashItem;
	memset(&MoveCashItem, 0, sizeof(MoveCashItem));

	MoveCashItem.cMoveType = cMoveType;
	MoveCashItem.cCashEquipIndex = cCashEquipIndex;
	MoveCashItem.biEquipItemSerial = biEquipSerial;
	MoveCashItem.biCashInvenSerial = biCashInvenSerial;
	MoveCashItem.wCount = wCount;

	CClientSessionManager::GetInstance().SendPacket(CS_ITEM, eItem::CS_MOVECASHITEM, (char*)&MoveCashItem, sizeof(CSMoveCashItem));
}

#if defined(PRE_ADD_CASHREMOVE)
inline void SendRemoveCash(INT64 biCashItemSerial, bool bRecovery)
{
	CSRemoveCash Item;
	memset(&Item, 0, sizeof(CSRemoveCash));

	Item.biItemSerial = biCashItemSerial;
	Item.bRecovery = bRecovery;

	CClientSessionManager::GetInstance().SendPacket(CS_ITEM, eItem::CS_REMOVECASH, (char*)&Item, sizeof(CSRemoveCash));
}
#endif	// #if defined(PRE_ADD_CASHREMOVE)

inline void SendMoveGuildItem(char cMoveType, char cSrcIndex, INT64 biSrcItemSerial, char cDestIndex, INT64 biDestItemSerial, short wCount)
{
	CSMoveGuildItem MoveItem;
	memset(&MoveItem, 0, sizeof(CSMoveGuildItem));

	MoveItem.cMoveType = cMoveType;
	MoveItem.cSrcIndex = cSrcIndex;
	MoveItem.biSrcItemSerial = biSrcItemSerial;
	MoveItem.cDestIndex = cDestIndex;
	MoveItem.biDestItemSerial = biDestItemSerial;
	MoveItem.wCount = wCount;

	CClientSessionManager::GetInstance().SendPacket(CS_ITEM, eItem::CS_MOVEGUILDITEM, (char*)&MoveItem, sizeof(CSMoveGuildItem));
}

inline void SendUseItem( char cInvenType, BYTE cInvenIndex, INT64 biInvenSerial )
{
	CSUseItem UseItem;
	memset( &UseItem, 0, sizeof(CSUseItem) );

	UseItem.cInvenType = cInvenType;
	UseItem.cInvenIndex = cInvenIndex;
	UseItem.biInvenSerial = biInvenSerial;

	CClientSessionManager::GetInstance().SendPacket( CS_ITEM, eItem::CS_USEITEM, (char*)&UseItem, sizeof(CSUseItem) );
}

inline void SendMoveCoin(char cType, INT64 nCoin)
{
	CSMoveCoin Coin;
	memset(&Coin, 0, sizeof(CSMoveCoin));

	Coin.cMoveType = cType;
	Coin.nMoveCoin = nCoin;

	CClientSessionManager::GetInstance().SendPacket(CS_ITEM, eItem::CS_MOVECOIN, (char*)&Coin, sizeof(CSMoveCoin));
}

inline void SendMoveGuildCoin(char cType, INT64 nCoin)
{
	CSMoveGuildCoin Coin;
	memset(&Coin, 0, sizeof(CSMoveGuildCoin));

	Coin.cMoveType = cType;
	Coin.nMoveCoin = nCoin;

	CClientSessionManager::GetInstance().SendPacket(CS_ITEM, eItem::CS_MOVEGUILDCOIN, (char*)&Coin, sizeof(CSMoveGuildCoin));
}

inline void SendEmblemCompoundOpenReq( CSEmblemCompoundOpenReq& CompoundOpenReq )
{
	CClientSessionManager::GetInstance().SendPacket( CS_ITEM, eItem::CS_EMBLEM_COMPOUND_OPEN_REQ, (char*)&CompoundOpenReq, sizeof(CSEmblemCompoundOpenReq) );
}

inline void SendItemCompoundOpenReq( CSItemCompoundOpenReq& CompoundOpenReq )
{
	CClientSessionManager::GetInstance().SendPacket( CS_ITEM, eItem::CS_ITEM_COMPOUND_OPEN_REQ, (char*)&CompoundOpenReq, sizeof(CSItemCompoundOpenReq) );
}

inline void SendEmblemCompoundReq( CSCompoundEmblemReq& EmblemCompoundReq )
{
	CClientSessionManager::GetInstance().SendPacket( CS_ITEM, eItem::CS_EMBLEM_COMPOUND_REQ, (char*)&EmblemCompoundReq, sizeof(CSCompoundEmblemReq) );
}

inline void SendItemCompoundReq( CSCompoundItemReq& ItemCompoundReq )
{
	CClientSessionManager::GetInstance().SendPacket( CS_ITEM, eItem::CS_ITEM_COMPOUND_REQ, (char*)&ItemCompoundReq, sizeof(CSCompoundItemReq) );
}

inline void SendEmblemCompoundCancelReq( CSEmblemCompoundCancelReq& CompoundCancelReq )
{
	CClientSessionManager::GetInstance().SendPacket( CS_ITEM, eItem::CS_EMBLEM_COMPOUND_CANCEL_REQ, (char*)&CompoundCancelReq, sizeof(CSEmblemCompoundCancelReq) );
}

inline void SendItemCompoundCancelReq( CSItemCompoundCancelReq& CompoundCancelReq )
{
	CClientSessionManager::GetInstance().SendPacket( CS_ITEM, eItem::CS_ITEM_COMPOUND_CANCEL_REQ, (char*)&CompoundCancelReq, sizeof(CSItemCompoundCancelReq) );
}

inline void SendItemDisjointReq( CSItemDisjointReq& ItemDisjointReq )
{
	CClientSessionManager::GetInstance().SendPacket( CS_ITEM, eItem::CS_DISJOINT_REQ, (char*)&ItemDisjointReq, sizeof(CSItemDisjointReq) );
}

inline void SendItemDisjointReqNew(CSItemDisjointReqNew& ItemDisjointReq)
{
	CClientSessionManager::GetInstance().SendPacket(CS_ITEM, eItem::CS_DISJOINT_REQ, (char*)&ItemDisjointReq, sizeof(CSItemDisjointReqNew));
}

inline void SendRebirth()
{
	CClientSessionManager::GetInstance().SendPacket(CS_ITEM, eItem::CS_REBIRTH, NULL, 0);
}

inline void SendRebirthCoin()
{
	CClientSessionManager::GetInstance().SendPacket(CS_ITEM, eItem::CS_REBIRTH_COIN, NULL, 0);
}

// 강화
#if defined(PRE_ADD_ENCHANTSHIELD_CASHITEM)
inline void SendEnchant(int nInvenIndex, INT64 biSerial, char cUseEnchantGuardType, bool bCashItem)
#else //#if defined(PRE_ADD_ENCHANTSHIELD_CASHITEM)
inline void SendEnchant(int nInvenIndex, INT64 biSerial, bool bUseEnchantGuard, bool bCashItem)
#endif  // #if defined(PRE_ADD_ENCHANTSHIELD_CASHITEM)
{
	CSEnchantItem Enchant;
	memset(&Enchant, 0, sizeof(CSEnchantItem));
#ifdef PRE_ADD_EQUIPED_ITEM_ENCHANT
	Enchant.cItemIndex = nInvenIndex;
#else 
	Enchant.cInvenIndex = nInvenIndex;
#endif 
	Enchant.biItemSerial = biSerial;

#if defined(PRE_ADD_ENCHANTSHIELD_CASHITEM)
	Enchant.cEnchantGuardType = cUseEnchantGuardType;
#else
	Enchant.bUseEnchantGuard = bUseEnchantGuard;
#endif
	Enchant.bCashItem = bCashItem;
	CClientSessionManager::GetInstance().SendPacket(CS_ITEM, eItem::CS_ENCHANT, (char*)&Enchant, sizeof(CSEnchantItem));
}

#ifdef PRE_ADD_EQUIPED_ITEM_ENCHANT // 디파인이 복잡해져서 따로 하나 정의해서 사용합니다. - elkain03 
#if defined(PRE_ADD_ENCHANTSHIELD_CASHITEM)
inline void SendEnchant(int nInvenIndex, INT64 biSerial, char cUseEnchantGuardType, bool bEquip, bool bCashItem)
#else 
inline void SendEnchant(int nInvenIndex, INT64 biSerial, bool bUseEnchantGuard, bool bEquip, bool bCashItem)
#endif 
{
	CSEnchantItem Enchant;
	memset(&Enchant, 0, sizeof(CSEnchantItem));

#ifdef PRE_ADD_EQUIPED_ITEM_ENCHANT
	Enchant.cItemIndex = nInvenIndex;
	Enchant.bEnchantEquipedItem = bEquip;
#else 
	Enchant.cInvenIndex = nInvenIndex;
#endif 
	Enchant.biItemSerial = biSerial;

#if defined(PRE_ADD_ENCHANTSHIELD_CASHITEM)
	Enchant.cEnchantGuardType = cUseEnchantGuardType;
#else
	Enchant.bUseEnchantGuard = bUseEnchantGuard;
#endif

	Enchant.bCashItem = bCashItem;

	CClientSessionManager::GetInstance().SendPacket(CS_ITEM, eItem::CS_ENCHANT, (char*)&Enchant, sizeof(CSEnchantItem));
}
#endif // PRE_ADD_EQUIPED_ITEM_ENCHANT

#if defined(PRE_ADD_ENCHANTSHIELD_CASHITEM)
inline void SendEnchantComplete(int nInvenIndex, INT64 biSerial, char cUseEnchantGuardType)
#else //#if defined(PRE_ADD_ENCHANTSHIELD_CASHITEM)
inline void SendEnchantComplete(int nInvenIndex, INT64 biSerial, bool bUseEnchantGuard)
#endif //#if defined(PRE_ADD_ENCHANTSHIELD_CASHITEM)
{
	CSEnchantItem Enchant;
	memset(&Enchant, 0, sizeof(CSEnchantItem));

#ifdef PRE_ADD_EQUIPED_ITEM_ENCHANT
	Enchant.cItemIndex = nInvenIndex;
#else 
	Enchant.cInvenIndex = nInvenIndex;
#endif 
	Enchant.biItemSerial = biSerial;

#if defined(PRE_ADD_ENCHANTSHIELD_CASHITEM)
	Enchant.cEnchantGuardType = cUseEnchantGuardType;
#else
	Enchant.bUseEnchantGuard = bUseEnchantGuard;
#endif // #if defined(PRE_ADD_ENCHANTSHIELD_CASHITEM)

	CClientSessionManager::GetInstance().SendPacket(CS_ITEM, eItem::CS_ENCHANTCOMPLETE, (char*)&Enchant, sizeof(CSEnchantItem));
}

#ifdef PRE_ADD_EQUIPED_ITEM_ENCHANT // 디파인이 복잡해져서 따로 하나 정의해서 사용합니다. - elkain03 
#if defined(PRE_ADD_ENCHANTSHIELD_CASHITEM)
inline void SendEnchantComplete(int nInvenIndex, INT64 biSerial, char cUseEnchantGuardType, bool bEquip, bool bCashItem )
#else 
inline void SendEnchantComplete(int nInvenIndex, INT64 biSerial, bool bUseEnchantGuard, bool bEquip, bool bCashItem)
#endif 
{
	CSEnchantItem Enchant;
	memset(&Enchant, 0, sizeof(CSEnchantItem));

	Enchant.cItemIndex = nInvenIndex;
	Enchant.bEnchantEquipedItem = bEquip;
	Enchant.biItemSerial = biSerial;

#if defined(PRE_ADD_ENCHANTSHIELD_CASHITEM)
	Enchant.cEnchantGuardType = cUseEnchantGuardType;
#else
	Enchant.bUseEnchantGuard = bUseEnchantGuard;
#endif // #if defined(PRE_ADD_ENCHANTSHIELD_CASHITEM)
	Enchant.bCashItem = bCashItem;

	CClientSessionManager::GetInstance().SendPacket(CS_ITEM, eItem::CS_ENCHANTCOMPLETE, (char*)&Enchant, sizeof(CSEnchantItem));
}
#endif // PRE_ADD_EQUIPED_ITEM_ENCHANT

inline void SendEnchantCancel()
{
	CClientSessionManager::GetInstance().SendPacket(CS_ITEM, eItem::CS_ENCHANTCANCEL, NULL, 0);
}

inline void SendCompleteRandomItem( int nInvenType, BYTE cInvenIndex, INT64 biInvenSerial )
{
	CSCompleteRandomItem RandomItem;
	memset( &RandomItem, 0, sizeof(CSCompleteRandomItem) );

	RandomItem.cInvenType = nInvenType;
	RandomItem.cInvenIndex = cInvenIndex;
	RandomItem.biInvenSerial = biInvenSerial;

	CClientSessionManager::GetInstance().SendPacket( CS_ITEM, eItem::CS_COMPLETE_RANDOMITEM, (char*)&RandomItem, sizeof(CSCompleteRandomItem) );
}

inline void SendCancelRandomItem()
{
	CClientSessionManager::GetInstance().SendPacket( CS_ITEM, eItem::CS_CANCEL_RANDOMITEM, NULL, 0 );
}

inline void SendRequestCharmItem( int nInvenType, short sInvenIndex, INT64 biInvenSerial, BYTE cKeyInvenIndex, int nKeyItemID, INT64 biKeyItemSerial )
{
	CSCharmItemRequest CharmItem;
	memset( &CharmItem, 0, sizeof(CSCharmItemRequest) );

	CharmItem.cInvenType = nInvenType;
	CharmItem.sInvenIndex = sInvenIndex;
	CharmItem.biInvenSerial = biInvenSerial;
	CharmItem.cKeyInvenIndex = cKeyInvenIndex;
	CharmItem.nKeyItemID = nKeyItemID;
	CharmItem.biKeyItemSerial = biKeyItemSerial;

	CClientSessionManager::GetInstance().SendPacket( CS_ITEMGOODS, eItemGoods::CS_CHARMITEMREQUEST, (char*)&CharmItem, sizeof(CSCharmItemRequest) );
}

inline void SendCompleteCharmItem( int nInvenType, short sInvenIndex, INT64 biInvenSerial, BYTE cKeyInvenIndex, int nKeyItemID, INT64 biKeyItemSerial )
{
	CSCharmItemComplete CharmItem;
	memset( &CharmItem, 0, sizeof(CSCharmItemComplete) );

	CharmItem.cInvenType = nInvenType;
	CharmItem.sInvenIndex = sInvenIndex;
	CharmItem.biInvenSerial = biInvenSerial;
	CharmItem.cKeyInvenIndex = cKeyInvenIndex;
	CharmItem.nKeyItemID = nKeyItemID;
	CharmItem.biKeyItemSerial = biKeyItemSerial;

	CClientSessionManager::GetInstance().SendPacket( CS_ITEMGOODS, eItemGoods::CS_CHARMITEMCOMPLETE, (char*)&CharmItem, sizeof(CSCharmItemComplete) );
}

inline void SendCancelCharmItem()
{
	CClientSessionManager::GetInstance().SendPacket( CS_ITEMGOODS, eItemGoods::CS_CHARMITEMCANCEL, NULL, 0 );
}

inline void SendSortInventory(TSortSlot *SortSlotArray, int nCount)
{
	CSSortInventory Sort;
	memset(&Sort, 0, sizeof(CSSortInventory));

	Sort.cTotalCount = nCount;
	memcpy(Sort.SlotInfo, SortSlotArray, sizeof(Sort.SlotInfo));

	int nLen = sizeof(CSSortInventory) - sizeof(Sort.SlotInfo) + (sizeof(TSortSlot) * nCount);
	CClientSessionManager::GetInstance().SendPacket(CS_ITEM, eItem::CS_SORTINVENTORY, (char*)&Sort, nLen);
}

inline void SendSortWarehouse(TSortSlot *SortSlotArray, int nCount)
{
	CSSortWarehouse Sort;
	memset(&Sort, 0, sizeof(CSSortWarehouse));

	Sort.cTotalCount = nCount;
	memcpy(Sort.SlotInfo, SortSlotArray, sizeof(Sort.SlotInfo));

	int nLen = sizeof(CSSortWarehouse) - sizeof(Sort.SlotInfo) + (sizeof(TSortSlot) * nCount);
	CClientSessionManager::GetInstance().SendPacket(CS_ITEM, eItem::CS_SORTWAREHOUSE, (char*)&Sort, nLen);
}

// Item Goods
inline void SendUnSeal(int nInvenIndex, INT64 biInvenSerial)
{
	CSUnSealItem unseal;
	memset(&unseal, 0, sizeof(CSUnSealItem));

	unseal.cInvenIndex = nInvenIndex;
	unseal.biInvenSerial = biInvenSerial;

	CClientSessionManager::GetInstance().SendPacket(CS_ITEMGOODS, eItemGoods::CS_UNSEAL, (char*)&unseal, sizeof(CSUnSealItem));
}

inline void SendSeal(int nInvenIndex, INT64 biInvenSerial, INT64 biItemSerial)
{
	CSSealItem Seal;
	memset(&Seal, 0, sizeof(CSSealItem));

	Seal.cInvenIndex = nInvenIndex;
	Seal.biInvenSerial = biInvenSerial;
	Seal.biItemSerial = biItemSerial;

	CClientSessionManager::GetInstance().SendPacket(CS_ITEMGOODS, eItemGoods::CS_SEAL, (char*)&Seal, sizeof(CSSealItem));
}

#ifdef PRE_ADD_GACHA_JAPAN
inline void SendGacha_JP( int nJobClassID, int nSelectedPart )
{
	CSGachaRequest_JP GachaRequest;
	memset( &GachaRequest, 0, sizeof(CSGachaRequest_JP) );

	GachaRequest.nJobClassID = nJobClassID;
	GachaRequest.nSelectedPart = nSelectedPart;

	CClientSessionManager::GetInstance().SendPacket( CS_ITEMGOODS, eItemGoods::CS_JP_GACHAPON_REQ, (char*)&GachaRequest, sizeof(CSGachaRequest_JP) );
}
#endif // PRE_ADD_GACHA_JAPAN

inline void SendCostumeMixOpen()
{
	CClientSessionManager::GetInstance().SendPacket(CS_ITEMGOODS, eItemGoods::CS_COSMIXOPEN, NULL, 0);
}

inline void SendCostumeMixClose()
{
	CClientSessionManager::GetInstance().SendPacket(CS_ITEMGOODS, eItemGoods::CS_COSMIXCLOSE, NULL, 0);
}

inline void SendCostumeMixCloseComplete()
{
	CClientSessionManager::GetInstance().SendPacket(CS_ITEMGOODS, eItemGoods::CS_COSMIXCOMPLETECLOSE, NULL, 0);
}

inline void SendCostumeMix(INT64* nStuffSerialIDs, int serialCount, UINT nResultItemID, char cOption)
{
	CSCosMix cosMixData;
	memset(&cosMixData, 0, sizeof(CSCosMix));

	int i = 0;
	for (; i < serialCount; ++i)
	{
		if (nStuffSerialIDs[i] == ITEMCLSID_NONE)
			return;
		cosMixData.nStuffSerialID[i] = nStuffSerialIDs[i];
	}
	cosMixData.nResultItemID = nResultItemID;
	cosMixData.cOption = cOption;

	CClientSessionManager::GetInstance().SendPacket(CS_ITEMGOODS, eItemGoods::CS_COSMIXCOMPLETE, (char*)&cosMixData, sizeof(CSCosMix));
}

inline void SendCostumeDesignMixClose()
{
	CClientSessionManager::GetInstance().SendPacket(CS_ITEMGOODS, eItemGoods::CS_DESIGNMIXCLOSE, NULL, 0);
}

inline void SendCostumeDesignMixCloseComplete()
{
	CClientSessionManager::GetInstance().SendPacket(CS_ITEMGOODS, eItemGoods::CS_DESIGNMIXCOMPLETECLOSE, NULL, 0);
}

inline void SendCostumeDesignMix(INT64* nStuffSerialIDs)
{
	CSCosDesignMix cosDesignMixData;
	memset(&cosDesignMixData, 0, sizeof(CSCosDesignMix));

	int i = 0;
	for (; i < eCDMST_MAX; ++i)
	{
		if (nStuffSerialIDs[i] == 0)
			return;
		cosDesignMixData.nStuffSerialID[i] = nStuffSerialIDs[i];
	}

	CClientSessionManager::GetInstance().SendPacket(CS_ITEMGOODS, eItemGoods::CS_DESIGNMIXCOMPLETE, (char*)&cosDesignMixData, sizeof(CSCosDesignMix));
}

#ifdef PRE_ADD_COSRANDMIX
inline void SendCostumeRandomMixClose()
{
	CClientSessionManager::GetInstance().SendPacket(CS_ITEMGOODS, eItemGoods::CS_RANDOMMIXCLOSE, NULL, 0);
}

inline void SendCostumeRandomMixCloseComplete()
{
	CClientSessionManager::GetInstance().SendPacket(CS_ITEMGOODS, eItemGoods::CS_RANDOMMIXCOMPLETECLOSE, NULL, 0);
}

inline void SendCostumeRandomMix(INT64* nStuffSerialIDs)
{
	CSCosRandomMix cosRandomMixData;
	memset(&cosRandomMixData, 0, sizeof(CSCosRandomMix));

	int i = 0;
	for (; i < MAX_COSTUME_RANDOMMIX_STUFF; ++i)
	{
		if (nStuffSerialIDs[i] == 0)
			return;
		cosRandomMixData.nStuffSerialID[i] = nStuffSerialIDs[i];
	}

	CClientSessionManager::GetInstance().SendPacket(CS_ITEMGOODS, eItemGoods::CS_RANDOMMIXCOMPLETE, (char*)&cosRandomMixData, sizeof(CSCosRandomMix));
}
#endif

inline void SendUseChangeJobCashItem( INT64 biItemSerial, int iJobID )
{
	CSUseChangeJobCashItemReq Packet;
	Packet.biItemSerial = biItemSerial;
	Packet.iJobToChange = iJobID;

	CClientSessionManager::GetInstance().SendPacket( CS_ITEMGOODS, eItemGoods::CS_USE_CHANGEJOB_CASHITEM_REQ, (char*)&Packet, sizeof(Packet) );
}

inline void SendPotentialJewel( int nInvenIndex, INT64 biInvenSerial, INT64 biItemSerial )
{
	CSPotentialItem Potential;
	memset(&Potential, 0, sizeof(CSPotentialItem));

	Potential.cInvenIndex = nInvenIndex;
	Potential.biInvenSerial = biInvenSerial;
	Potential.biItemSerial = biItemSerial;

	CClientSessionManager::GetInstance().SendPacket(CS_ITEMGOODS, eItemGoods::CS_POTENTIALJEWEL, (char*)&Potential, sizeof(CSPotentialItem));
}

inline void SendPotentialJewel888(int nInvenIndex, INT64 biInvenSerial, INT64 biItemSerial)
{
	CSPotentialItem Potential;
	memset(&Potential, 0, sizeof(CSPotentialItem));

	Potential.cInvenIndex = nInvenIndex;
	Potential.biInvenSerial = biInvenSerial;
	Potential.biItemSerial = biItemSerial;

	CClientSessionManager::GetInstance().SendPacket(CS_ITEMGOODS, eItemGoods::CS_POTENTIALJEWEL888, (char*)&Potential, sizeof(CSPotentialItem));
}

inline void SendPotentialJewel999(int nInvenIndex, INT64 biInvenSerial, INT64 biItemSerial)
{
	CSPotentialItem Potential;
	memset(&Potential, 0, sizeof(CSPotentialItem));

	Potential.cInvenIndex = nInvenIndex;
	Potential.biInvenSerial = biInvenSerial;
	Potential.biItemSerial = biItemSerial;

	CClientSessionManager::GetInstance().SendPacket(CS_ITEMGOODS, eItemGoods::CS_POTENTIALJEWEL999, (char*)&Potential, sizeof(CSPotentialItem));
}

#ifdef PRE_MOD_POTENTIAL_JEWEL_RENEWAL
inline void SendRollBackPotentialJewel( int nInvenIndex, INT64 biInvenSerial, INT64 biItemSerial )
{
	CSPotentialItem Potential;
	memset(&Potential, 0, sizeof(CSPotentialItem));

	Potential.cInvenIndex = nInvenIndex;
	Potential.biInvenSerial = biInvenSerial;
	Potential.biItemSerial = biItemSerial;

	CClientSessionManager::GetInstance().SendPacket(CS_ITEMGOODS, eItemGoods::CS_POTENTIALJEWEL_ROLLBACK, (char*)&Potential, sizeof(CSPotentialItem));
}
#endif

inline void SendGuildRename( char cInvenType, BYTE cInvenIndex, INT64 biItemSerial, const WCHAR* wszGuildName)
{
	CSGuildRename GuildRename;
	memset(&GuildRename, 0, sizeof(CSGuildRename));

	GuildRename.cInvenType = cInvenType;
	GuildRename.cInvenIndex = cInvenIndex;
	GuildRename.biItemSerial = biItemSerial;

	_wcscpy(GuildRename.wszGuildName, _countof(GuildRename.wszGuildName), wszGuildName, (int)wcslen(wszGuildName));

	CClientSessionManager::GetInstance().SendPacket(CS_ITEMGOODS, eItemGoods::CS_GUILDRENAME, (char*)&GuildRename, sizeof(CSGuildRename));
}

inline void SendCharRename( char cInvenType, BYTE cInvenIndex, INT64 biItemSerial, const WCHAR* wszCharName)
{
	CSCharacterRename CharRename;
	memset(&CharRename, 0, sizeof(CSCharacterRename));

	CharRename.cInvenType = cInvenType;
	CharRename.cInvenIndex = cInvenIndex;
	CharRename.biItemSerial = biItemSerial;

	_wcscpy(CharRename.wszCharacterName, _countof(CharRename.wszCharacterName), wszCharName, (int)wcslen(wszCharName));

	CClientSessionManager::GetInstance().SendPacket(CS_ITEMGOODS, eItemGoods::CS_CHARACTERRENAME, (char*)&CharRename, sizeof(CSCharacterRename));
}

inline void SendEnchantJewel( int nInvenIndex, INT64 biInvenSerial, INT64 biItemSerial )
{
	CSEnchantJewelItem EnchantJewel;
	memset(&EnchantJewel, 0, sizeof(CSEnchantJewelItem));

	EnchantJewel.cInvenIndex = nInvenIndex;
	EnchantJewel.biInvenSerial = biInvenSerial;
	EnchantJewel.biItemSerial = biItemSerial;

	CClientSessionManager::GetInstance().SendPacket(CS_ITEMGOODS, eItemGoods::CS_ENCHANTJEWEL, (char*)&EnchantJewel, sizeof(CSEnchantJewelItem));
}

inline void SendGuildMark( int nItemID, INT64 biItemSerial, int nInvenIndex, short wGuildMark, short wGuildMarkBG, short wGuildMarkBorder )
{
	CSGuildMark packet;
	packet.nItemID = nItemID;
	packet.biItemSerial = biItemSerial;
	packet.nInvenIndex = nInvenIndex;
	packet.wGuildMark = wGuildMark;
	packet.wGuildMarkBG = wGuildMarkBG;
	packet.wGuildMarkBorder = wGuildMarkBorder;
	CClientSessionManager::GetInstance().SendPacket(CS_ITEMGOODS, eItemGoods::CS_GUILDMARK, (char*)&packet, sizeof(CSGuildMark));
}

#if defined(PRE_ADD_REMOVE_PREFIX)
inline void SendRemovePrefix(int nInvenIndex, INT64 biInvenSerial, INT64 biItemSerial)
{
	CSEnchantJewelItem removePrefix;
	memset(&removePrefix, 0, sizeof(CSEnchantJewelItem));

	removePrefix.cInvenIndex = nInvenIndex;
	removePrefix.biInvenSerial = biInvenSerial;
	removePrefix.biItemSerial = biItemSerial;

	CClientSessionManager::GetInstance().SendPacket(CS_ITEMGOODS, eItemGoods::CS_REMOVE_PREFIX, (char*)&removePrefix, sizeof(CSEnchantJewelItem));
}
#endif // PRE_ADD_REMOVE_PREFIX

inline void SendWarpVillageList( INT64 iUsedItemSerial )
{
	CSWarpVillageList Packet;
	Packet.nItemSerial = iUsedItemSerial;

	CClientSessionManager::GetInstance().SendPacket( CS_ITEMGOODS, eItemGoods::CS_WARP_VILLAGE_LIST, (char*)&Packet, sizeof(CSWarpVillageList) );
}

inline void SendWarpVillage( int iMapID, INT64 iUsedItemSerial )
{
	CSWarpVillage Packet;
	Packet.nMapIndex = iMapID;
	Packet.nItemSerial = iUsedItemSerial;

	CClientSessionManager::GetInstance().SendPacket( CS_ITEMGOODS, eItemGoods::CS_WARP_VILLAGE, (char*)&Packet, sizeof(CSWarpVillage) );
}

#ifdef PRE_ADD_AUTOUNPACK
inline void SendRequestAutoUnpackItem( INT64 biInvenSerial, bool bShow )
{
	CSAutoUnpack pPacket;
	memset( &pPacket, 0, sizeof(CSAutoUnpack) );

	pPacket.bShow = bShow;
	pPacket.serial = biInvenSerial;

	CClientSessionManager::GetInstance().SendPacket( CS_ITEM, eItem::CS_USE_AUTOUNPACK_CASHITEM_REQ, (char*)&pPacket, sizeof(CSAutoUnpack) );
}
#endif

#ifdef PRE_ADD_EXCHANGE_POTENTIAL
inline void SendExchangePotential( int nExtractItemIndex, INT64 biExtractItemSerial, int nInjectItemIndex, INT64 biInjectItemSerial )
{
	CSExchangePotential ExchangePotential;
	memset(&ExchangePotential, 0, sizeof(CSExchangePotential));

	ExchangePotential.nExtractItemIndex = nExtractItemIndex;
	ExchangePotential.biExtractItemSerial = biExtractItemSerial;
	ExchangePotential.nInjectItemIndex = nInjectItemIndex;
	ExchangePotential.biInjectItemSerial = biInjectItemSerial;

	CClientSessionManager::GetInstance().SendPacket(CS_ITEMGOODS, eItemGoods::CS_EXCHANGE_POTENTIAL, (char*)&ExchangePotential, sizeof(CSExchangePotential));
}
#endif


#ifdef PRE_ADD_CHAOSCUBE

inline void SendRequestChaosCube( CSChaosCubeRequest & packet )
{
	CClientSessionManager::GetInstance().SendPacket(CS_ITEMGOODS, eItemGoods::CS_CHAOSCUBEREQUEST, (char*)&packet, sizeof(CSChaosCubeRequest) );
}

inline void SendCompleteChaosCube( CSChaosCubeComplete & packet )
{
	CClientSessionManager::GetInstance().SendPacket(CS_ITEMGOODS, eItemGoods::CS_CHAOSCUBECOMPLETE, (char*)&packet, sizeof(CSChaosCubeComplete) );
}

inline void SendCancelChaosCube()
{
	CClientSessionManager::GetInstance().SendPacket( CS_ITEMGOODS, eItemGoods::CS_CHAOSCUBECANCEL, NULL, 0 );
}
#endif


#ifdef PRE_ADD_BESTFRIEND

// 절친확인.
inline void SendRequestSearchBF( BestFriend::CSSearch & packet )
{
	CClientSessionManager::GetInstance().SendPacket(CS_BESTFRIEND, eBestFriend::CS_SEARCH, (char*)&packet, sizeof(BestFriend::CSSearch) );
}

// 절친등록.
inline void SendRegistBF( BestFriend::CSRegist & packet )
{
	CClientSessionManager::GetInstance().SendPacket(CS_BESTFRIEND, eBestFriend::CS_REGIST, (char*)&packet, sizeof(BestFriend::CSRegist) );
}

// 절친요청수락.
inline void SendAcceptBF( BestFriend::CSAccept & packet )
{
	CClientSessionManager::GetInstance().SendPacket(CS_BESTFRIEND, eBestFriend::CS_ACCEPT, (char*)&packet, sizeof(BestFriend::CSAccept) );
}

// 보상아이템번호 전송.
inline void SendRewardItemBF( CSBestFriendItemRequest & packet )
{	
	CClientSessionManager::GetInstance().SendPacket(CS_ITEMGOODS, eItemGoods::eCSItemGoods::CS_BESTFRIENDREQUEST, (char*)&packet, sizeof(CSBestFriendItemRequest) );
}

// 보상아이템완료 전송.
inline void SendRewardItemCompleteBF( CSBestFriendItemComplete & packet )
{	
	CClientSessionManager::GetInstance().SendPacket(CS_ITEMGOODS, eItemGoods::eCSItemGoods::CS_BESTFRIENDCOMPLETE, (char*)&packet, sizeof(CSBestFriendItemComplete) );
}


// 보상아이템취소 전송.
inline void SendRewardItemCancelBF()
{	
	CClientSessionManager::GetInstance().SendPacket(CS_ITEMGOODS, eItemGoods::eCSItemGoods::CS_BESTFRIENDCANCEL, NULL, 0 );
}

// 절친메모전송.
inline void SendMemoBF( BestFriend::CSEditMemo & packet )
{	
	CClientSessionManager::GetInstance().SendPacket(CS_BESTFRIEND, eBestFriend::eCSBestFriend::CS_EDITMEMO, (char*)&packet, sizeof(BestFriend::CSEditMemo) );
}

// 절친파기 or 절친파기취소.
inline void SendBrokeupOrCancelBF( BestFriend::CSCancel & packet )
{	
	CClientSessionManager::GetInstance().SendPacket(CS_BESTFRIEND, eBestFriend::eCSBestFriend::CS_CANCELBF, (char*)&packet, sizeof(BestFriend::CSCancel) );
}

inline void SendRequestBFInfo()
{
	CClientSessionManager::GetInstance().SendPacket(CS_BESTFRIEND, eBestFriend::eCSBestFriend::CS_GETINFO, NULL, 0);
}

#endif

#if defined( PRE_ADD_NOTIFY_ITEM_COMPOUND )
inline void SendItemCompoundNotify( CSCompoundNotify& CompoundNotifyInfo )
{
	CClientSessionManager::GetInstance().SendPacket(CS_CHAR, eChar::CS_COMPOUND_NOTIFY, (char*)&CompoundNotifyInfo, sizeof(CSCompoundNotify));
}
#endif

#ifdef PRE_ADD_EXCHANGE_ENCHANT
inline void SendItemExchangeEnchant(INT64 biExtractItemSerial, int extractItemId, INT64 biInjectItemSerial, int injectItemId)
{
	CSExchangeEnchant packet;
	memset(&packet, 0, sizeof(CSExchangeEnchant));

	packet.nExtractItemIndex = extractItemId;
	packet.biExtractItemSerial = biExtractItemSerial;
	packet.nInjectItemIndex = injectItemId;
	packet.biInjectItemSerial = biInjectItemSerial;

	CClientSessionManager::GetInstance().SendPacket(CS_ITEMGOODS, eItemGoods::CS_EXCHANGE_ENCHANT, (char*)&packet, sizeof(CSExchangeEnchant));

}
#endif

#if defined( PRE_ADD_STAGE_CLEAR_ADD_REWARD )
inline void SendStageClearBonusRewardSelect( UINT nPropID, int nItemID )
{
	CSStageClearBonusRewardSelect packet;
	memset(&packet, 0, sizeof(CSStageClearBonusRewardSelect));

	packet.nPropID = nPropID;
	packet.nItemID = nItemID;

	CClientSessionManager::GetInstance().SendPacket(CS_CHAR, eChar::CS_STAGECLEAR_BONUSREWARD_SELECT, (char*)&packet, sizeof(CSStageClearBonusRewardSelect));
}
#endif	// PRE_ADD_STAGE_CLEAR_ADD_REWARD

#ifdef PRE_ADD_SERVER_WAREHOUSE
inline void SendMoveItemServerWarehouse(char cMoveType, char cInvenIndex, INT64 biSrcItemSerial, INT64 biDestItemSerial, short wCount)
{
	CSMoveServerWare packet;
	memset(&packet, 0, sizeof(CSMoveServerWare));

	packet.cMoveType = cMoveType;
	packet.cInvenIndex = cInvenIndex;
	packet.biInvenItemSerial = biSrcItemSerial;
	packet.biWareItemSerial = biDestItemSerial;
	packet.wCount = wCount;

	CClientSessionManager::GetInstance().SendPacket(CS_ITEM, eItem::CS_MOVE_SERVERWARE, (char*)&packet, sizeof(CSMoveServerWare));
};

inline void SendMoveCashItemServerWarehouse(char cMoveType, char cSrcIndex, INT64 biSrcItemSerial)
{
	CSMoveCashServerWare packet;
	memset(&packet, 0, sizeof(CSMoveCashServerWare));

	packet.cMoveType = cMoveType;
	packet.biItemSerial = biSrcItemSerial;

	CClientSessionManager::GetInstance().SendPacket(CS_ITEM, eItem::CS_MOVE_CASHSERVERWARE, (char*)&packet, sizeof(CSMoveCashServerWare));
}
#endif

#ifdef PRE_ADD_EQUIPLOCK
inline void SendItemEquipLock(bool bIsCash, BYTE equipSlotIndex, INT64 biItemSerial)
{
	CSItemLockReq packet;
	memset(&packet, 0, sizeof(CSItemLockReq));

	packet.IsCashEquip = bIsCash;
	packet.nItemSlotIndex = equipSlotIndex;
	packet.biItemSerial = biItemSerial;

	CClientSessionManager::GetInstance().SendPacket(CS_ITEM, eItem::CS_ITEM_LOCK_REQ, (char*)&packet, sizeof(CSItemLockReq));
}

inline void SendItemEquipUnLock(bool bIsCash, BYTE equipSlotIndex, INT64 biItemSerial)
{
	CSItemLockReq packet;
	memset(&packet, 0, sizeof(CSItemLockReq));

	packet.IsCashEquip = bIsCash;
	packet.nItemSlotIndex = equipSlotIndex;
	packet.biItemSerial = biItemSerial;

	CClientSessionManager::GetInstance().SendPacket(CS_ITEM, eItem::CS_ITEM_UNLOCK_REQ, (char*)&packet, sizeof(CSItemLockReq));
}
#endif

#if defined(PRE_PERIOD_INVENTORY)
inline void SendSortPeriodInventory(TSortSlot *SortSlotArray, int nCount)
{
	CSSortInventory Sort;
	memset(&Sort, 0, sizeof(CSSortInventory));

	Sort.cTotalCount = nCount;
	memcpy(Sort.SlotInfo, SortSlotArray, sizeof(Sort.SlotInfo));

	int nLen = sizeof(CSSortInventory) - sizeof(Sort.SlotInfo) + (sizeof(TSortSlot) * nCount);
	CClientSessionManager::GetInstance().SendPacket(CS_ITEM, eItem::CS_SORTPERIODINVENTORY, (char*)&Sort, nLen);
}

inline void SendSortPeriodWarehouse(TSortSlot *SortSlotArray, int nCount)
{
	CSSortWarehouse Sort;
	memset(&Sort, 0, sizeof(CSSortWarehouse));

	Sort.cTotalCount = nCount;
	memcpy(Sort.SlotInfo, SortSlotArray, sizeof(Sort.SlotInfo));

	int nLen = sizeof(CSSortWarehouse) - sizeof(Sort.SlotInfo) + (sizeof(TSortSlot) * nCount);
	CClientSessionManager::GetInstance().SendPacket(CS_ITEM, eItem::CS_SORTPERIODWAREHOUSE, (char*)&Sort, nLen);
}
#endif	// #if defined(PRE_PERIOD_INVENTORY)