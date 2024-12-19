#pragma once

/*---------------------------------------------------------------------------------------
									CDNUserItem

	- ���� ĳ���� �������� ����ϴ� Ŭ����
	- VillageServer, GameServer�� �������� ������ �ִ� Ŭ���� �̹Ƿ�
      if defined ��ĥ(?)�� ������
---------------------------------------------------------------------------------------*/

#if defined(_GAMESERVER)
class CDnItemTask;
#endif	// #if defined(_GAMESERVER)
class CDNDBConnection;
class CDNUserSession;
class CDNMissionSystem;
class CDNGuildWare;

class CDNCoolTime
{
public:
	CDNCoolTime(){}
	~CDNCoolTime(){}

	void Clear()
	{
		m_mCoolTime.clear();
	}

	int	AddCoolTime( const int nSkillID, const DWORD dwEndTime )
	{
		std::pair<std::map<int,DWORD>::iterator,bool> Ret = m_mCoolTime.insert( std::make_pair( nSkillID, dwEndTime ) );
		if( Ret.second == false )
			return -1;
		return 0;
	}

	DWORD Update( const int nSkillID, const DWORD dwEndTime )
	{
		std::map<int,DWORD>::iterator itor = m_mCoolTime.find( nSkillID );
		if( itor == m_mCoolTime.end() )
			return 0;

		// ��Ÿ�� �ð� �������� ����
		if( dwEndTime >= itor->second )
		{
			m_mCoolTime.erase( itor );
			return 0;
		}
	
		return itor->second;
	}

	void GetCoolTimeData( std::vector<std::pair<int,int>>& vData )
	{
		std::map<int,DWORD> Temp = m_mCoolTime;

		for( std::map<int,DWORD>::iterator itor=Temp.begin() ; itor!=Temp.end() ; ++itor )
		{
			int iEndTime = Update((*itor).first, timeGetTime() );
			if( iEndTime > 0 )
			{
				vData.push_back( std::make_pair((*itor).first,iEndTime-timeGetTime()) );
			}
		}
	}

private:
	std::map<int,DWORD>	m_mCoolTime;
};

class CDNUserItem: public TBoostMemoryPool<CDNUserItem>
{
public:
	struct TSaveItemInfo: public TItemInfo
	{
		bool bMerge;

		TSaveItemInfo()
		{
			cSlotIndex = 0;
			bMerge = false;
			memset(&Item, 0, sizeof(TItem));
		}

		void SetInfo(BYTE SlotIndex, const TItem &SetItem, bool Merge)
		{
			cSlotIndex = SlotIndex;
			Item = SetItem;
			bMerge = Merge;
		}
	};

	enum RequestTypeEnum {
		RequestType_None = -1,
		RequestType_UseRandomItem,
		RequestType_Enchant,
	};

private:
	CDNUserSession *m_pSession;

	TItem m_Equip[EQUIPMAX];							// ���â
	TItem m_Glyph[GLYPHMAX];							// ����
#if defined(PRE_PERIOD_INVENTORY)
	TItem m_Inventory[INVENTORYTOTALMAX];				// �κ�â
	TItem m_Warehouse[WAREHOUSETOTALMAX];				// â��â
#else	// #if defined(PRE_PERIOD_INVENTORY)
	TItem m_Inventory[INVENTORYMAX];					// �κ�â
	TItem m_Warehouse[WAREHOUSEMAX];					// â��â
#endif	// #if defined(PRE_PERIOD_INVENTORY)
	TQuestItem m_QuestInventory[QUESTINVENTORYMAX];		// ����Ʈ �κ�â
#if defined(PRE_ADD_TALISMAN_SYSTEM)
	TItem m_Talisman[TALISMAN_MAX];						// Ż������
#endif
#if defined(PRE_ADD_EQUIPLOCK)
	EquipItemLock::TLockItem m_EquipLock[EQUIPMAX];
	EquipItemLock::TLockItem m_CashEquipLock[CASHEQUIPMAX];
#endif	// #if defined(PRE_ADD_EQUIPLOCK)

	TItem m_CashEquip[CASHEQUIPMAX];					// ���â(ĳ��)
	TUnionMembership m_UnionMembership[NpcReputation::UnionType::Etc]; // ����ȣ����

	typedef std::map<INT64, TItem> TMapItem;
	TMapItem m_MapCashInventory;				// �κ�â(ĳ��) first: Serial

#if defined(_VILLAGESERVER) && defined(PRE_ADD_CASH_REFUND)
	typedef std::map<INT64, TPaymentItemInfoEx> TMapPaymentItem;
	TMapPaymentItem m_MapPaymentItem;					// ���� �κ�(��ǰ)
	typedef std::map<INT64, TPaymentPackageItemInfoEx> TMapPaymentPackageItem;
	TMapPaymentPackageItem m_MapPaymentPackageItem;	// ���� �κ�(��Ű��)
	bool m_bCashMoveInven;
#endif // #if defined(_VILLAGESERVER) && defined(PRE_ADD_CASH_REFUND)
#if defined(PRE_ADD_SERVER_WAREHOUSE)
	TMapItem m_MapServerWarehouse;					// ���� â�� �Ϲ�..���� ���տ����� 150������ �⺻ ������ GlobalWeightTable
	TMapItem m_MapServerWarehouseCash;				// ���� â�� ĳ��
#endif

	TVehicle m_VehicleEquip;
	TVehicle m_PetEquip;
	typedef std::map<INT64, TVehicle> TMapVehicle;
	TMapVehicle m_MapVehicleInventory;
	float m_fSatietyPercent;

	CDNCoolTime	m_CoolTime;

	BYTE m_cInventoryCount;
	BYTE m_cWarehouseCount;
#if defined(PRE_PERIOD_INVENTORY)
	bool m_bEnablePeriodInventory;
	__time64_t m_tPeriodInventoryExpireDate;
	bool m_bEnablePeriodWarehouse;
	__time64_t m_tPeriodWarehouseExpireDate;
#endif	// #if defined(PRE_PERIOD_INVENTORY)
	int m_iGlyphExtendCount;
	//�ѹ��� ������ �����ٴµ� Ȥ�� �Ѱ��� ������ ������ �𸣱� ������ ������ ���彽�Կ� ���� ������ ����
	TCashGlyphData	m_TCashGlyphData[CASHGLYPHSLOTMAX];
#if defined(PRE_ADD_TALISMAN_SYSTEM)
	int m_nTalismanOpenFlag;
	bool m_bTalismanCashSlotEntend;
	__time64_t	m_tTalismanExpireDate;	// Ż������ ĳ�� ���� �Ⱓ	
#endif

	bool m_bMoveItemCheckGameMode;

	TExchangeData m_ExchangeData[EXCHANGEMAX];		// �ŷ�����Ʈ
	INT64 m_nExchangeCoin;		// �ŷ� ����
	bool m_bExchangeRegist;		// �ŷ� ��ǰ ���
	bool m_bExchangeConfirm;	// �ŷ� ���� Ȯ��

	int m_nDailyMailCount;		// 1�� ���� �� �ִ� �����
	int m_nWeeklyRegisterCount;		// ����� AM 04:00 �� �������� �����ϰ� ����� �ŷ� Ƚ�� / ���ݵ���Ҽ��ִ� Ƚ��
	int m_nRegisterItemCount;		// �÷����� ������ ����

	// ���� ��û ���� ���� ����ߴٰ� ó���ϴ� �͵��� ���ؼ� �ϴ� �������þ��
	RequestTypeEnum m_RequestType;
	DWORD m_dwRequestTimer;
	DWORD m_dwRequestInterval;

	DBPacket::TTakeAttachInfo m_ReadMailAttachItem;	// 
	std::vector<TMyMarketInfo> m_VecMyMarketList;	// backup��?
#if defined(PRE_SPECIALBOX)
	TSpecialBoxInfo m_SpecialBoxInfo[SpecialBox::Common::ListMax];
	TAGetListEventRewardItem m_SelectSpecialBoxItem;
#endif	// #if defined(PRE_SPECIALBOX)

	int m_nCashInventoryTotalCount;
	int m_nCashInventoryPage;
	int m_nVehicleInventoryTotalCount;

#ifdef PRE_ADD_CHANGEJOB_CASHITEM
	int m_nChangeFirstJob;
	int m_nChangeSecondJob;
#endif // #ifdef PRE_ADD_CHANGEJOB_CASHITEM
	char m_nSkillPageCount;

	TItem m_Source; // �ٿ�
	std::map<int,TRepurchaseItemInfo> m_mRepurchaseItemInfo;

#if defined( PRE_ADD_NAMEDITEM_SYSTEM )
	typedef map<int, TEffectSkillData> TMapEffectSkillList;
	TMapEffectSkillList m_mEffectSkillList;
#endif
#ifdef PRE_ADD_GETITEM_WITH_RANDOMVALUE
	std::vector<TCharmItem> m_CalculatedRandomCharmList;
#endif		//#ifdef PRE_ADD_GETITEM_WITH_RANDOMVALUE
#if defined(PRE_MOD_POTENTIAL_JEWEL_RENEWAL)
	TPrevPotentialData m_PrevPotentialItem;
#endif	// #if defined(PRE_MOD_POTENTIAL_JEWEL_RENEWAL)

private:
	int _Equip(const CSMoveItem *pMove);					// Equip <-> Equip
	int _Inven(const CSMoveItem *pMove);					// Inven <-> Inven
	int _Ware(const CSMoveItem *pMove);					// Ware <-> Ware
	int _FromEquipToInven(const CSMoveItem *pMove);		// Equip -> Inven
	int _FromInvenToEquip(const CSMoveItem *pMove);		// Inven -> Equip
	int _FromInvenToWare(const CSMoveItem *pMove);			// Inven -> Ware
	int _FromWareToInven(const CSMoveItem *pMove);			// Ware -> Inven
	int _FromGlyphToInven(const CSMoveItem *pMove);		// Glyph -> Inven
	int _FromInvenToGlyph(const CSMoveItem *pMove);		// Inven -> Glyph

	int _FromCashEquipToCashInven(const CSMoveCashItem *pMove);		// CashEquip -> CashInven
	int _FromCashInvenToCashEquip(const CSMoveCashItem *pMove);		// CashInven -> CashEquip
	int _FromCashGlyphToCashInven(const CSMoveCashItem *pMove);		// CashGlyph -> CashInven
	int _FromCashInvenToCashGlyph(const CSMoveCashItem *pMove);		// CashInven -> CashGlyph
	int _FromVehicleBodyToVehicleInven(const CSMoveCashItem *pMove);	// VehicleBody <-> VehicleInven
	int _FromVehicleInvenToVehicleBody(const CSMoveCashItem *pMove);	// VehicleInven <-> VehicleBody
	int _FromVehiclePartsToCashInven(const CSMoveCashItem *pMove);		// VehicleParts <-> CashInven
	int _FromCashInvenToVehicleParts(const CSMoveCashItem *pMove);		// CashInven <-> VehicleParts
	int _FromPetBodyToVehicleInven(const CSMoveCashItem *pMove);	//PetBody�� �꽽�Կ��� Ż��&�� �κ�����(����)
	int _FromVehicleInvenToPetBody(const CSMoveCashItem *pMove);	//PetBody�� Ż��&�� �κ����� �� ��������(��ȯ)
	int _FromPetPartsToCashInven(const CSMoveCashItem *pMove);		//PetParts�� �� ���Կ��� ĳ���κ�����(����)
	int _FromCashInvenToPetParts(const CSMoveCashItem *pMove);		//PetParts�� ĳ���κ����� �� ��������(����)

#if defined(PRE_ADD_TALISMAN_SYSTEM)
	int _Talisman(const CSMoveItem *pMove);	// Talisman <-> Talisman
	int _FromTalismanToInven(const CSMoveItem *pMove);	// Talisman <-> Inven
	int _FromInvenToTalisman(const CSMoveItem *pMove);	// Inven <-> Talisman	
#endif

#if defined(PRE_ADD_SERVER_WAREHOUSE)
	int _FromInvenToServerWareHouse(const CSMoveServerWare *pMove);
	int _FromServerWareHouseToInven(const CSMoveServerWare *pMove);
	int _FromCashToServerWareHouse(const CSMoveCashServerWare *pMove);
	int _FromServerWareHouseToCash(const CSMoveCashServerWare *pMove);
#endif // #if defined(PRE_ADD_SERVER_WAREHOUSE)

	// ����ϰ� �ٲ�ġ��� �Լ���
	int _SwapEquipToEquip(int nSrcIndex, int nDestIndex);	// Equip <-> Equip
	int _SwapInvenToInven(int nSrcIndex, int nDestIndex);	// Inven <-> Inven
	int _SwapWareToWare(int nSrcIndex, int nDestIndex);		// Ware <-> Ware
	int _SwapQuestInvenToQuestInven(int nSrcIndex, int nDestIndex);		// QuestInven <-> QuestInven
	int _SwapEquipToInven(int nSrcIndex, int nDestIndex);	// Equip -> Inven
	int _SwapInvenToEquip(int nSrcIndex, int nDestIndex);	// Inven -> Equip
	int _SwapInvenToWare(int nSrcIndex, int nDestIndex);	// Inven -> Ware
	int _SwapWareToInven(int nSrcIndex, int nDestIndex);	// Ware -> Inven
	int _SwapCashEquipToCashInven(int nSrcEquipIndex, INT64 biDestInvenSerial);	// CashEquip -> CashInven
	int _SwapCashInvenToCashEquip(INT64 biSrcInvenSerial, int nDestEquipIndex);	// CashInven -> CashEquip
	int _SwapCashGlyphToCashInven(int nSrcGlyphIndex, INT64 biDestInvenSerial);	// CashGlyph -> CashInven
	int _SwapCashInvenToCashGlyph(INT64 biSrcInvenSerial, int nDestGlyphIndex);	// CashInven -> CashGlyph
#if defined(PRE_ADD_TALISMAN_SYSTEM)
	int _SwapTalismanToTalisman(int nSrcIndex, int nDestIndex);	// Talisman <-> Talisman
#endif

	// Equip���� �Լ�
	int _PushEquipSlot(int nIndex, const TItem &AddItem);
	int _PopEquipSlot(int nIndex, short wCount, bool bSend=true );
	// Glyph���� �Լ�
	int _PushGlyphSlot(int nIndex, const TItem &AddItem);
	int _PopGlyphSlot(int nIndex, short wCount, bool bSend=true );
	// inven���� �Լ�
	int _PushInventorySlotItem(int nIndex, const TItem &AddItem);
	int _PopInventorySlotItem(int nIndex, short wCount);
	// CashEquip
	int _PushCashEquipSlot(int nIndex, const TItem &AddItem, bool bSubParts = false);
	int _PopCashEquipSlot(int nIndex, short wCount);
	// Cash inven���� �Լ�
	int _PushCashInventory(const TItem &AddItem);
	int _PopCashInventoryBySerial(INT64 biSerial, short wCount);
	// â�� ���� �Լ�
	int _PushWarehouseSlot(int nIndex, const TItem &AddItem);
	int _PopWarehouseSlot(int nIndex, short wCount);
	// ����Ʈ �κ� ���� �Լ�
	int _PushQuestInventorySlot(int nIndex, const TQuestItem &AddItem);
	int _PopQuestInventorySlot(int nIndex, short wCount);
	// Ż��
	// Vehicle Equip
	int _PushVehicleEquipSlot(int nIndex, const TItem &AddItem );
	int _PopVehicleEquipSlot(int nIndex, short wCount, bool bSend=true );
	// Vehicle Inven
	int _PushVehicleInventory(const TVehicle &AddItem);
	int _PopVehicleInventoryBySerial(INT64 biSerial);
 	int _PushPetEquipSlot(int nIndex, const TItem &AddItem );
 	int _PopPetEquipSlot(int nIndex, short wCount, bool bSend=true );
#if defined(PRE_ADD_SERVER_WAREHOUSE)
	int _PushServerWare(INT64 biSerial, const TItem &AddItem);
	int _PopServerWareBySerial(INT64 biSerial, short wCount);
	int _PushServerWareCash(const TItem &AddItem);
	int _PopServerWareCashBySerial(INT64 biSerial, short wCount);
#endif
#if defined(PRE_ADD_TALISMAN_SYSTEM)
	// Talisman���� �Լ�
	int _PopTalismanSlot(int nIndex, short wCount, bool bSend=true );
#endif

	// exchange
	int _FindBlankExchangeIndex();	// �� �����ε��� ã��
	bool _ExistExchangeInven(int nInvenIndex);	// �̹� �ִ� �κ��������� �� �ٸ������� �ִ´ٸ� ���ƾ��Ѵ�

	// cooltime ����
	void _SetItemCoolTime( const int nSkillID, const int nMaxCoolTime );				// ���� skillid ������ ������ ���� ��Ÿ������...
	void _UpdateInventoryCoolTime( const int nIndex, const DWORD dwCompareTime=0 );	// Update cooltime
	void _UpdateCashInventoryCoolTime( const INT64 biSerial, const DWORD dwCompareTime=0 );	// Update cooltime
	void _UpdateWarehouseCoolTime( const int nIndex, const DWORD dwCompareTime=0 );	// Update cooltime

	// Repair(������)
	int _CalcRepairEquipPrice();
	int _CalcRepairInvenPrice();
	void _RepairEquip(std::vector<INT64> &VecSerialList, std::vector<USHORT> &VecDurList);
	void _RepairInven(std::vector<INT64> &VecSerialList, std::vector<USHORT> &VecDurList);

	bool _CheckRangeEquipIndex(int nIndex) const;
	bool _CheckRangeInventoryIndex(int nIndex) const;
	bool _CheckRangeWarehouseIndex(int nIndex) const;
	bool _CheckRangeQuestInventoryIndex(int nIndex) const;
	bool _CheckRangeGlyphIndex(int nIndex) const;
	bool _CheckRangeCashEquipIndex(int nIndex) const;
	bool _CheckRangeCashGlyphIndex(int nIndex) const;
	bool _CheckRangeVehicleBodyIndex(int nIndex) const;
	bool _CheckRangeVehiclePartsIndex(int nIndex) const;
	bool _CheckRangePetBodyIndex(int nIndex) const;
	bool _CheckRangePetPartsIndex(int nIndex) const;
#if defined(PRE_ADD_SERVER_WAREHOUSE)
	bool _CheckRangeServerWareHouseIndex(int nIndex) const;
	bool _CheckRangeServerWareHouseCashIndex(int nIndex) const;
#endif //#if defined(PRE_ADD_SERVER_WAREHOUSE)
#if defined(PRE_ADD_TALISMAN_SYSTEM)
	bool _CheckRangeTalismanIndex(int nIndex) const;
#endif
#if defined(PRE_PERIOD_INVENTORY)
	bool _CheckRangePeriodInventoryIndex(int nIndex) const;
	bool _CheckRangePeriodWarehouseIndex(int nIndex) const;
#endif	// #if defined(PRE_PERIOD_INVENTORY)

	int _IsMoveEnableItem(const TItem *pSrcItem, const CSMoveItem *pMove);

	int _CreateInvenItemEx(const TItem &CreateItem, int nLogCode, INT64 biFKey, std::vector<TSaveItemInfo> &VecItemList, bool bInsertList, char cType = CREATEINVEN_ETC);
	int _CreateCashInvenItemEx(int nItemSN, BYTE cPayMethodCode, int nLogCode, INT64 biFKey, int nPeriod, int nPrice, const TItem &CreateItem);

#if defined(PRE_SPECIALBOX)
	int _ReceiveSpecialBoxCash(int nItemSN, int nOption, bool bCheckCondition);
#endif	// #if defined(PRE_SPECIALBOX)

public:
	CDNUserItem(CDNUserSession *pUserSession);
	virtual ~CDNUserItem(void);

	void SetMoveItemCheckGameMode( bool bFlag ){ m_bMoveItemCheckGameMode = bFlag; }

	static INT64 MakeItemSerial();		// Serial �����
	static INT64 MakeCashItemSerial(UINT nDay);		// CashItem Serial �����

	void DoUpdate(DWORD CurTick);	

	void LoadUserData(TASelectCharacter *pData);
	void SaveUserData();
	void RemoveInstantItemData( bool bSend );
	void RemoveInstantVehicleItemData( bool bSend );
	void RemoveInstantEquipVehicleData( bool bSend ); // ���� �������� Ż�� ����

	void RemoveGuildReversionItem( bool bSend );
	void RemoveGuildReversionVehicleItemData( bool bSend );
	void ResetCoolTime();
#if defined( PRE_ADD_FARM_DOWNSCALE )
	static void CalcDropItems( CDNUserSession* pSesion, int nDropItemTableID, int &nResultItemID, int &nResultItemCount, int &nLastValue, int &nDepth );
#else
	void CalcDropItems( int nDropItemTableID, int &nResultItemID, int &nResultItemCount, int &nLastValue, int &nDepth );
#endif // #if defined( PRE_ADD_FARM_DOWNSCALE )

	//------ ��Ŷó���� �Լ�(?) -> ���� Send�� ������ �Լ��� ����
	bool OnRecvMoveItem(const CSMoveItem *pPacket);	// Move Process
	bool OnRecvMoveCashItem(const CSMoveCashItem *pPacket);	// Move Process
#if defined(PRE_ADD_SERVER_WAREHOUSE)
	bool OnRecvMoveServerWareItem(const CSMoveServerWare* pPacket);
	bool OnRecvMoveServerWareCashItem(const CSMoveCashServerWare* pPacket);
#endif // #if defined(PRE_ADD_SERVER_WAREHOUSE)
#if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
	bool OnRecvPickUp(TItem& resultItemInfo, int nItemID, short wCount, int nRandomSeed, char cOption, int nEnchantID = 0);						// �ٴ� -> Inven
#else	// #if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
	bool OnRecvPickUp(TItem& resultItemInfo, int nItemID, short wCount, int nRandomSeed, char cOption);						// �ٴ� -> Inven
#endif	// #if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
	bool OnRecvRemoveItem(const CSRemoveItem *pPacket);		// Equip, Inven -> ������
	int OnRecvRemoveCash(INT64 biItemSerial, bool bRecovery);
#if defined(PRE_ADD_REMOTE_OPENSHOP)
	void OnRecvShopRemoteOpen(Shop::Type::eCode eType);
#endif	// #if defined(PRE_ADD_REMOTE_OPENSHOP)
	int OnRecvBuyNpcShopItem(int nShopID, char cShopTabID, BYTE cShopIndex, short wCount);			// npcshop �����ϱ�
	int OnRecvSellNpcShopItem(const CSShopSell *pPacket);						// npcshop �Ǹ��ϱ�
	int OnRecvShopRepurchase( const CSShopRepurchase* pPacket );
	int OnRecvRepairEquip( int iUseItemID=0 );	// ��������(equip)
	void OnRecvRepairAll( bool bCheat=false );	// ��ü����(equip + inven)
#if defined(PRE_ADD_REPAIR_NPC)
	int CheckRepairEquip( int iUseItemID ); // ��������(equip)
	int CheckRepairAll( bool bCheat );	// ��ü����(equip + inven)
#endif //#if defined(PRE_ADD_REPAIR_NPC)
#ifdef PRE_FIX_ONCANCEL_USEITEM
	void OnCancelUseItem(const CSUseItem& Packet);
#endif
	void OnRecvUseItem(CSUseItem *pPacket);
	void OnRecvGetListRepurchaseItem( const TAGetListRepurchaseItem* pPacket );
#if defined(PRE_ADD_TALISMAN_SYSTEM)
	int OnRecvOpenTalismanSlot(const CSOpenTalismanSlot* pPacket);
#endif

	int ReturnItemToNpc(int nScheduleID);
	int UnsealItem(int nInvenIndex, INT64 biInvenSerial);
	int SealItem(int nInvenIndex, INT64 biInvenSerial, INT64 biItemSerial);
	bool EmblemCompoundComplete( int nEmblemItemID, BYTE cPlateItemSlotIndex, INT64 biPlateItemSerialID, int* apUseItemID, int* apUseItemCount, int nUseItemArrayCount );
	bool ItemCompoundComplete( int nResultItemID, char cResultItemOptionIndex, int* apUseItemID, int* apUseItemCount, int nUseItemArrayCount, INT64 biResultItemIsNeedItem, std::vector<TSaveItemInfo> *pVecResultList = NULL );

	// inven�� ������ �ֱ�(���� ������� �� �� ���ųֱ�) - �޾ ��ų�(Pickup), ����Ʈ���� �־��ְų�(db ���� ����)
	int CreateInvenItem1(int nItemID, short wCount, char cOption, int nRandomSeed, int nLogCode, INT64 biFKey, char cType = CREATEINVEN_ETC);
	int CreateInvenItem2(int nItemID, short wCount, std::vector<TSaveItemInfo> &VecItemList, char cType = CREATEINVEN_ETC, int nShopItemPeriod = 0);	// ��� �α׳��涧 ���� �Լ�
	int CreateInvenWholeItem(const TItem &Item, int nLogCode, INT64 biFKey, char cType = CREATEINVEN_ETC);	// �̹� �ִ� ������ �κ��� ������ ���� �Լ�
	int CreateInvenWholeItemByIndex(int nInvenIndex, const TItem &Item);	// �� �ε����� �ھƹ����°�

	// Cash, VehicleInven(ItemType�� ���� �Ϲ� ĳ������ Ż������ ����)
	int CreateCashInvenItem(int nItemID, int nItemCount, int nLogCode, char cOption = -1, int nPeriod = 0, int nPrice = 0, INT64 biFKey = 0, BYTE cPayMethodCode = DBDNWorldDef::PayMethodCode::Coin);	// ��í���� ItemSN���� ItemID�θ� ĳ���� �����ϴ°�
	int CreateCashInvenWholeItem(const TItem &Item);	// �� �ھƹ����°�
	int CreateCashInvenItemByCheat(int nItemID, short wCount, int nPeriod, int nLogCode);
#if defined(PRE_LEVELUPREWARD_DIRECT)
	bool CreateCashInvenItemByMailID(int nMailID);
#endif	// #if defined(PRE_LEVELUPREWARD_DIRECT)

	bool DeleteItemByUse(int nInvenType, int nInvenIndex, INT64 biInvenSerial, bool bDBSave=true );	// �ش� �κ� �ε����� �ִ� �����ִ� ������ 1���� �����ֱ�(db ���� ����)
	bool UseItemByType(int nType, int nUseCount, bool bSend, int nTypeParam1 = -1, int iTargetLogItemID=0, char cTargetLogItemLevel=0 );
	bool UseItemByItemID(int iItemID, int nUseCount, bool bSend, int nTypeParam1 = -1);

	//-------------------------------------------------------------------------------------------------------------------------------------------------

	// Index�� TItem ���
	const TItem *GetEquip(int nIndex) const;		// m_pItemGroup->Equip[cIndex].Item
	const TItem *GetGlyph(int nIndex) const;
	const TItem* GetInventory(int nIndex) const;	// m_pItemGroup->Inventory[cIndex].Item
	const TItem* GetWarehouse(int nIndex) const;	// m_pItemGroup->Warehouse[cIndex].Item
	const TQuestItem* GetQuestInventory(int nIndex) const;	// m_pItemGroup->QuestInventory[cIndex].Item
	const TItem* GetCashEquip(int nIndex) const;
	const TItem* GetCashInventory(INT64 biItemSerial) const;
	const TItem *GetVehiclePartsEquip(int nIndex) const;
	TVehicle *GetVehicleEquip();
	const TVehicle* GetPetEquip() const;
	const TItem* GetPetPartsEquip(int nIndex) const;
	const TItem *GetVehicleBodyInventory(INT64 biItemSerial) const;
	const TVehicle *GetVehicleInventory(INT64 biItemSerial) const;
#if defined(PRE_ADD_SERVER_WAREHOUSE)
	const TItem* GetServerWare(INT64 biItemSerial) const;
	const TItem* GetServerWareCash(INT64 biItemSerial) const;	
#endif
#if defined(PRE_ADD_TALISMAN_SYSTEM)
	const TItem *GetTalisman(int nIndex) const;
#endif

	// Equip���� �Լ�
	int GetEquipSlotIndex( int nItemID ) const;
	int FindBlankEquipSlot();									// equip�� �󽽷� ã��
	void GetEquipIDs(int *EquipArray) const;	// ���� �����۵� ���
	void GetEquipItemDurability(std::vector<INT64> &VecSerialList, std::vector<USHORT> &VecDurList) const;
#if defined(PRE_ADD_TALISMAN_SYSTEM)
	void GetEquipList(char &cCount, char &cCashCount, char &cGlyphCount, char &cTalismanCount, TItemInfo *EquipArray) const;
#else
	void GetEquipList(char &cCount, char &cCashCount, char &cGlyphCount, TItemInfo *EquipArray) const;
#endif
	void BroadcastChangeEquip(int nSlotIndex, const TItem &EquipItem);	// �ֺ� �ֵ鿡�� Equip�ٲ�� �ѷ��ֱ�
	void ChangeEquipItemDurability( int nIndex, USHORT wDur );
	bool ChangeDefaultParts( int nItemType, const int *pTypeParam, INT64 biUseInvenItemSerial ); // ��, ��� �� Default ���� ����
	int IsEquipEnableItem(const TItem& equipExpected) const;
	bool IsEquipItem(int nSlotIndex, int nItemID) const;		// Equip Slot�� �´� �������������� �˻�
	bool CheckEquipByItemID(int nItemID) const;			// Equip �� Ư�� �������� �����ߴ��� �˻�
#if defined(PRE_ADD_EQUIPED_ITEM_ENCHANT) || defined(PRE_ADD_EQUIPLOCK)
	bool IsValidEquipSlot(int nIndex, INT64 biSerial, bool bCheckSerial = true) const;	// Equip �� ���� ���� �ߴ���
#endif	// #if defined(PRE_ADD_EQUIPED_ITEM_ENCHANT) ||  || defined(PRE_ADD_EQUIPLOCK)

	// CashEquip
	int TakeOffOnePiece(int nItemID);	// ĳ���� ����
	int GetCashEquipOnePieceMainParts(int nEquipItemID);	// �ѹ����϶� ��������ã��
	void BroadcastChangeCashEquip(int nSlotIndex, const TItem &EquipItem);	// �ֺ� �ֵ鿡�� CashEquip�ٲ�� �ѷ��ֱ�
	bool IsEquipCashItemExist(int nItemID) const;		// Equip �� Ư�� ĳ�� �������� �����ߴ��� �˻�

	// Glyph
	bool IsGlyphItem(int nSlotIndex, int nItemID);		// Glyph Slot�� �´� ���������� �˻�
	void BroadcastChangeGlyph(int nSlotIndex, const TItem &GlyphItem);	// �ֺ� �ֵ鿡�� Glyph�ٲ�� �ѷ��ֱ�

#if defined(PRE_ADD_TALISMAN_SYSTEM)
	// Talisman
	int CheckTalismanSlotValidation(int nSlotIndex);
	bool SetTalismanSlotOpenFlag(int nSlotIndex, bool bFlag);
	bool IsTalismanSlotOpened(int nSlotIndex);
	int GetTalismanSlotOpenFlag() { return m_nTalismanOpenFlag; }
	void SetTalismanSlotOpenFlag( int nFlag ){ m_nTalismanOpenFlag = nFlag; }
	bool IsTalismanCashSlotEntend() { return m_bTalismanCashSlotEntend; }
	void SetTalismanCashSlotEntend( bool bTalismanCashSlotEntend ) { m_bTalismanCashSlotEntend = bTalismanCashSlotEntend; }	
	void SetTalismanExpireDate( __time64_t tExpireDate ){ m_tTalismanExpireDate = tExpireDate; }
	__time64_t GetTalismanExpireDate() { return m_tTalismanExpireDate; }
	void BroadcastChangeTalisman(int nSlotIndex, const TItem &TalismanItem);
#endif

	int IsTradeEnableItem(int nInvenType, int nInvenIndex, INT64 biInvenSerial, short wInputCount);

	// inven���� �Լ�
	bool CheckRangeInventoryIndex(int nIndex) const;
	bool AddInventoryByQuest(int nItemID, short wCount, int nQuestID, int nRandomSeed);	// itemid, count, randomseed�� ���� inventory�� �ֱ�
	bool AddInventoryByQuest( const TQuestReward::_ITEMSET* pItemSet, int nQuestID, int nRandomSeed );	// TQuestReward::_ItemSet�� ���� inventory�� �ֱ�
	bool DeleteInventoryByQuest(int nItemID, short wCount, int nQuestID);				// ItemID������ inven�� ����(db ���� ����)
	bool DeleteInventoryByItemID(int nItemID, int iCount, int nLogCode, INT64 biFKey=0 );// ItemID������ inven�� ����(db ���� ����)
	bool DeleteInventoryExByItemID(int nItemID, char cOption, int iCount, int nLogCode, INT64 biFKey, std::vector<TItemInfo> &VecItemList, bool bInsertList);		// ItemID������ inven�� ����(db ���� ����) - ���� list�� 
	bool DeleteInventoryBySlot(int nSlotIndex, short wCount, INT64 biSerial, int nLogCode);					// SlotIndex������ inven�� ����(db ���� ����)
	bool DeleteInventoryByType(int nType, short wCount, int nLogCode, int nTypeParam1 = -1, int iTargetLogItemID=0, char cTargetLogItemLevel=0 );
	int FindInventorySlot(int nItemID, short wCount) const;	// �ش� ItemID, Count�� inven���� indexã��
	int FindInventorySlot(int nItemID, char cOption, short wCount, bool bSoulBound = false ) const;
	int FindInventorySlotBySerial( INT64 biSerial ) const;
	bool CheckEnoughItem( int iItemID, int iCount ) const;
	int	GetInventoryItemCount(int nItemID, char cOption = -1) const;						// id�� �ش��ϴ� ������ ��� �ִ���
	int GetInventoryItemCountByType(int nType, int nTypeParam1 = -1) const;
	bool IsValidSpaceInventorySlot(int nItemID, short wCount, bool bSoulBound, char cSealCount, bool bEternity) const;	// �������� ���� �� �ִ� ������ ������� �˻�
	int IsValidSpaceInventorySlotFromShop( const TItemData* pItemData, int iCheckCount ) const;
	int FindBlankInventorySlot() const;									// inven�� �󽽷� ã��
	int FindBlankInventorySlotCount() const;									// inven�� �󽽷��� ��� �ִ��� ����ã��
	int FindBlankInventorySlotCountList(std::queue<int> &qSlotList) const;
	int FindOverlapInventorySlot(int nItemID, short wCount, bool bSoulBound, char cSealCount, bool bEternity) const;	// inven�� �����ִ� �������� ���� ������ wCount ���� �� �ִ� Indexã��
	bool IsValidInventorySlot(int nIndex, INT64 biSerial, bool bCheckSerial = true) const;	// inven�� ���� �ִ���
	bool VerifyInventorySlotPacket(int nInvenIndex, const TItem &Item) const;	// ���λ���, ���� ���� index�޾Ƽ� ��� ���ٿ� ������ �˻��ϱ�
	void ChangeInventoryItemDurability( int nIndex, USHORT wDur );
	bool SortInventory(CSSortInventory *pPacket);
#if defined(PRE_PERIOD_INVENTORY)
	bool SortPeriodInventory(CSSortInventory *pPacket);
#endif	// #if defined(PRE_PERIOD_INVENTORY)
	void GetInventoryItemDurability(std::vector<INT64> &VecSerialList, std::vector<USHORT> &VecDurList) const;
	void CheatClearInven();
	int GetInventoryItemListFromItemID( int nItemID, std::vector<TItem *> &pVecResult );
	int GetInventoryItemListFromItemID( int nItemID, char cOption, std::vector<TItem *> &pVecResult );
	TItem* GetInventoryItemByType(int nType);

	// Cash inven���� �Լ�
	void LoadCashInventory(int nPageNum, int nTotalCount, int nCount, TItem *CashItemList);
	bool UseCashItemByType(int nType, int nUseCount, bool bSend);
	bool DeleteCashInventoryByItemID(int nItemID, short wCount, int nLogCode, INT64 biFKey=0 );				// ItemID������ inven�� ����(db ���� ����)
	bool DeleteCashInventoryBySerial(INT64 biInvenSerial, short wCount, bool bSaveDB = true);					// SlotIndex������ inven�� ����(db ���� ����)
	void ChangeCashInventoryItemDurability(INT64 biSerial, USHORT wDur);
	int	GetCashItemCountByItemID(int nItemID);						// id�� �ش��ϴ� ������ ��� �ִ���
	int GetCashItemCountByType(int nType, int nTypeParam1 = -1);	// �� Ÿ���� ĳ���� ��� �������ֳ�
	void GetCashInventoryItemListByType( int nType, std::vector<const TItem*>& VecResult, int nTypeParam1=-1 );
	TItem *GetCashItemByType(int nType);	// Ÿ���� ������ �����Ͱ�
	bool IsValidCashItem(int nItemID, short wCount);	// ĳ���������� �ִ���
#if defined(_GAMESERVER)
	void InitializePlayerCashItem(CDnItemTask *pItemTask);
#endif	// #if defined(_GAMESERVER)
	void CheatClearCashInven();
	int GetExtendPrivateFarmFieldCount();
	void OnRemoveCashItem(TADelCashItem *pDel);
	void OnRecoverCashItem(TARecoverCashItem *pRecover);

	// ware���� �Լ�
	bool IsValidWarehouseSlot(int nIndex);	// ware�� ���� �ִ���
	int	GetWarehouseItemCount(int nItemID) const;						// id�� �ش��ϴ� ������ ��� �ִ���
	int FindBlankWarehouseSlotCountList(std::queue<int> &qSlotList);
	bool SortWarehouse(CSSortWarehouse * pPacket);
#if defined(PRE_PERIOD_INVENTORY)
	bool SortPeriodWarehouse(CSSortWarehouse * pPacket);
#endif	// #if defined(PRE_PERIOD_INVENTORY)

	void GetIndexSerialList(int &nTotalInvenWareCount, DBPacket::TItemIndexSerial *SaveList);

	// Quest Inventory ����
	int AddQuestInventory(int nItemID, short wCount, int nQuestID, int nLogCode);	// -> Quest Inventory(�ֱ�)
	int DeleteQuestInventory(int nItemID, short wCount, int nQuestID, int nLogCode);	// <- Quest Inventory(������)
	int FindQuestInventorySlot(int nItemID, short wCount);			// �ش� ItemID, Count�� quest inven���� indexã��
	int FindBlankQuestInventorySlot();								// inven�� �󽽷� ã��
	int FindOverlapQuestInventorySlot(int nItemID, short wCount);	// inven�� �����ִ� �������� ���� ������ wCount ���� �� �ִ� Indexã��
	int FindBlankQuestInventorySlotCountList(std::queue<int> &qSlotList);
	int FindBlankQuestInventorySlotCount();								// inven�� �󽽷��� ��� �ִ��� ����ã��
	int	GetQuestInventoryItemCount(int nItemID);						// id�� �ش��ϴ� ������ ����
	void CheatClearQuestInven();
	bool IsValidSpaceQuestInventorySlot(int nItemID, short wCount);		// ����Ʈ �������� ���� �� �ִ� ������ ������� �˻�

	// Ż��
	int GetVehicleBodyItemID();
	void LoadVehicleInventory(const TAGetPageVehicle *pVehicle);
	void CheatClearVehicleInven();
	// VehicleEquip
	bool IsVehicleEquipItem(int nSlotIndex, int nItemID);		// Vehicle�� �´� ���������� �˻�
	bool ChangeVehicleDyeColor(const int *pTypeParam);
	void BroadcastChangeVehicleParts(int nSlotIndex, const TItem &EquipItem);	// �ֺ� �ֵ鿡�� Equip�ٲ�� �ѷ��ֱ�

	// Pet
	int GetPetBodyItemID();
	INT64 GetPetBodySerial();
	int GetPetExp();
	int ModItemExpireDate(const CSModItemExpireDate* pItem, int nMin);	// ������ �Ⱓ ����.
	void UpdatePetExp(int nExp);
	bool ChangePetBodyColor(const int *pTypeParam);
	bool ChangePetNoseColor(const int *pTypeParam);
	bool AddPetExp(int nAddPetExp);
	void ClearPetVehicleInven();
	void ItemExpireByCheat(INT64 biItemSerial);
	void BroadcastChangePetParts(int nSlotIndex, TItem &EquipItem);
	void BroadcastChangePetBody(const TVehicle &PetInfo);
	void BroadcastChangeVehiclePetColor(INT64 biSerial, DWORD dwColor, char cPetPartsColor);	// �ֺ� �ֵ鿡�� Equip�ٲ�� �ѷ��ֱ�
	int ChangePetName(const TAChangePetName *pPacket);
	void ModPetExpireDate(const TAModItemExpireDate *pPacket);
	int GetPetLevel();	// �� ����������(������������ ������ �ִ� ���� �ְ���, ���Ӽ����� �����ȯ�Ǿ��ִ� �� ����)
	void SetPetSatiety(INT64 biPetSerial, int nSatiety);
	void CalcPetSatiety(DWORD CurTick);
	float GetPetSatietyPercent() { return m_fSatietyPercent; }
	void CalcPetSatietyPercent();
	float GetSatietyApplyExpRatio();
	float GetSatietyApplyStateRatio();
	bool IsSatietyPet();
	void SendPetSatiety();

	void SetUnionMembership( BYTE cType, int nItemID, __time64_t tExpireDate );
	TUnionMembership* GetUnionMembership( BYTE cType );

	// ����
#if defined(_VILLAGESERVER)
	int IsValidSendMailItem(int nInvenIndex, int nItemID, short wCount, INT64 biSerial, DBPacket::TSendMailItem &ItemInfo);
	int CheckSendMailItem(const CSSendMail *pPacket);
#ifdef PRE_ADD_JOINGUILD_SUPPORT
	void SetReadMail(int nMailDBID, INT64 biCoin, const TItem *MailItemArray, BYTE cMailType);
#else		//#ifdef PRE_ADD_JOINGUILD_SUPPORT
	void SetReadMail(int nMailDBID, INT64 biCoin, const TItem *MailItemArray);
#endif		//#ifdef PRE_ADD_JOINGUILD_SUPPORT
	void ClearReadMail( int iIndex=-1 );
	int VerifyAttachItemList(const TAGetListMailAttachment *pMail);
	int VerifyAttachItem(const CSAttachMail *pMail);
	int TakeAttachItemList(const TATakeAttachMailList *pMail);
	int TakeAttachItem(const TATakeAttachMail *pMail);

	bool AddDailyMailCount();
	bool DelDailyMailCount();
	void SetDailyMailCount(int nCount);
	int GetDailyMailCount() { return m_nDailyMailCount; }
#endif	// #if defined(_VILLAGESERVER)

#if defined(PRE_SPECIALBOX)
	void SetSpecialBoxInfoList(const TAGetListEventReward *pPacket);
	void ClearSpecialBoxInfoList();
	TSpecialBoxInfo *GetSpecialBoxInfo(int nEventRewardID);
	INT64 GetSelectSpecialBoxRewardCoin(int nEventRewardID);
	void SetSelectSpecialBoxItems(const TAGetListEventRewardItem *pPacket);
	void ClearSelectSpecialBoxItems();

	int CheckReceiveSpecialBox(int nEventRewardID, int nItemID);
	int ReceiveSpecialBox(int nEventRewardID, int nItemID);
#endif	// #if defined(PRE_SPECIALBOX)

	// ���λ���
	int CheckRegisterMarketItem(CSMarketRegister *pPacket);	// market ����ϱ����� üũ
	bool CheckRegisterMarketItemCount( bool bPremiumTrade );
	bool CheckRegisterMarketPeriod( int nPeriod );
	int GetMarketMaxRegisterItemCount();

	int CancelMarketItem(int nMarketDBID);
#if defined(PRE_ADD_DIRECT_BUY_UPGRADEITEM)
	int BuyMarketItem(int nMarketDBID, bool bMini);
#else	// #if defined(PRE_ADD_DIRECT_BUY_UPGRADEITEM)
#if defined(PRE_ADD_PETALTRADE)
	int BuyMarketItem(int nMarketDBID, int nItemID, int nPetalPrice);
#else
	int BuyMarketItem(int nMarketDBID, int nItemID);
#endif
#endif	// #if defined(PRE_ADD_DIRECT_BUY_UPGRADEITEM)
	void SetRegisterMarketCount(int nWeeklyRegisterCount, int nRegisterItemCount);
	void AddRegisterItemCount();
	void DelRegisterItemCount();
	int GetRegisterItemCount() { return m_nRegisterItemCount; }
	void AddWeeklyRegisterCount();

	// ���ΰŷ�
	int AddExchangeItem(CSExchangeAddItem *pPacket);	// �ŷ� ������ �����ϱ�
	int DeleteExchangeItem(char cExchangeIndex);	// �ŷ� ������ �����
	int AddExchangeCoin(INT64 nCoin);					// �ŷ� ���� ����
	void ClearExchangeData();						// �ŷ� ������ �ʱ�ȭ
	bool IsValidExchange(int nExchangeCount);	// �ŷ��� ��������(�ŷ��������� �κ��� �� �� �� �ִ���)
	int FindExchangeCount();		// �ŷ��������� ��ΰ�
	INT64 GetExchangeCoin();
	bool CompleteExchange(CDNUserSession *pTargetSession, DBPacket::TExchangeItem *ExchangeInfo, int &nExchangeTax);	// �ŷ� �Ϸ��ϱ�(�� ������ ���� �κ����� �־��ֱ�)
	inline void SetExchangeRegist() { m_bExchangeRegist = true; }
	inline bool GetExchangeRegist() { return m_bExchangeRegist; }

	inline void SetExchangeConfirm() { m_bExchangeConfirm = true; }
	inline bool GetExchangeConfirm() { return m_bExchangeConfirm; }

	void SetEquipItemDurability( int nIndex, USHORT wDur, bool bSend=false );
	void SetInvenItemDurability( int nIndex, USHORT wDur );

	// ��ȭ, �����
	int CheckUpgradeEnchant(CSEnchantItem *pPacket);
	int UpgradeEnchant(CSEnchantItem *pPacket);
	void CheatUpgradeItemLevel(int nItemLevel);
	void CheatUpgradeItemPotential(int nPotential);

#if defined (PRE_ADD_EXCHANGE_POTENTIAL)
	int ExchangePotential(CSExchangePotential * pPacket);
#endif		//#if defined (PRE_ADD_EXCHANGE_POTENTIAL)
#if defined(PRE_ADD_EXCHANGE_ENCHANT)
	int ExchangeEnchant(CSExchangeEnchant* pPacket);
#endif

	int MixCostume(int& resultItemId, const INT64* pSerials, UINT nResultItemID, char cOption);
	int CheckCosMix(const INT64* pSerials, UINT nResultItemID, char cOption) const;
	int CheckCosDesignMix(const INT64* pSerials) const;
	int MixCostumeDesign(const INT64* pSerials);
	void CompleteCostumeDesign(TAModAdditiveItem * pPacket);
#if defined (PRE_ADD_COSRANDMIX) && defined (_VILLAGESERVER)
	#ifdef PRE_ADD_COSRANDMIX_ACCESSORY
	eError CheckCosRandMix(INT64& currentMixFee, const INT64* pSerials) const;
	#else
	eError CheckCosRandMix(const INT64* pSerials) const;
	#endif
	eError MixRandomCostume(int& nResultItemId, const INT64* pSerials);
	void CompleteRandomDesign(TAModRandomItem * pPacket);
#endif

#if defined(_VILLAGESERVER)
	//int CanUseSkillResetCashItem( TItemData * pItemData, int& iRewardSP, vector<int>& vlSkillIDsToReset );
	int CanUseSkillResetCashItem( int iFromJobDegree, int iToJobDegree, int& iRewardSP, vector<int>& vlSkillIDsToReset );
	int UseSkillResetCashItem( TItemData * pItemData );
	int ResetSkillFromQuestScript( void );

#ifdef PRE_ADD_CHANGEJOB_CASHITEM
	int UseChangeJobCashItem( TItemData* pItemData, int iJobToChange );
	void OnResponseChangeJobCode( bool bSuccess );
	int GetChangeFirstJobID( void ) { return m_nChangeFirstJob; };
	int GetChangeSecondJobID( void ) { return m_nChangeSecondJob; };
#endif // #ifdef PRE_ADD_CHANGEJOB_CASHITEM
#endif

	// ���� ������
	bool RequestRandomItem(CSUseItem *pPacket);
	bool CalcRandomItem(CSCompleteRandomItem *pPacket);
	// �����ȭ ĳ�þ�����
	bool RequestWorldMsgItem(CSUseItem *pPacket);

	// ��ų���� ������
	bool UseSkillResetItem( int nIndex );

	// ���ټ� ����
	int PotentialItem( int nInvenIndex, INT64 biInvenSerial, INT64 biItemSerial );

	int PotentialItem888(int nInvenIndex, INT64 biInvenSerial, INT64 biItemSerial);

	int PotentialItem999(int nInvenIndex, INT64 biInvenSerial, INT64 biItemSerial);

	// ���� ���� ������
	void RequestChangeGuildNameItem(CSGuildRename *pPacket);

	// ĳ���͸� ���� ������
	void RequestChangeCharacterNameItem(CSCharacterRename *pPacket);
	bool CanChangeCharacterName();

	void RequestChangePetName(CSChangePetName *pPacket);

	int EnchantJewel( int nInvenIndex, INT64 biInvenSerial, INT64 biItemSerial );

	void SetRequestTimer( RequestTypeEnum Type, DWORD dwInterval );
	bool IsValidRequestTimer( RequestTypeEnum Type );

	CDNMissionSystem *GetMissionSystem();

	void OnRecvMoveCoinInventoWare(char cType, INT64 nMoveCoin);
	void OnRecvMoveCoinWaretoInven(char cType, INT64 nMoveCoin);

	static bool MakeItemStruct(int nItemID, TItem &ResultItem, int nShopItemPeriod = 0, int nOptionTableID = 0);	// �������� �ڵ� �������ִµ��Ͽ� �̰����� ������. createinven�Ҷ��� ���⼭ ����
	static bool bIsSameItem( const TItem* pItem, const TItem* pItem2 );
	static bool bIsDifferentItem( const TItem* pItem, const TItem* pItem2 );
	static bool bIsSameItem( const TItem* pItem, const TInvenItemCnt* pItem2 );
	static bool bIsDifferentItem( const TItem* pItem, const TInvenItemCnt* pItem2 );
#if defined(PRE_MOD_POTENTIAL_JEWEL_RENEWAL)
	//����� �ʱ�ȭ
	void ResetPrevPotentialData(){ memset(&m_PrevPotentialItem, 0, sizeof(m_PrevPotentialItem));}
	//����� �ο� ���(���� ��������� ����)
	int RollbackPotentialItem( int nInvenIndex, INT64 biInvenSerial, INT64 biCodeItemSerial );
#endif	// #if defined(PRE_MOD_POTENTIAL_JEWEL_RENEWAL)

	static int MakeCashItemStruct(int nItemSN, int nItemID, TItem &CashItem, char cOption = -1, int nPeriod = 0);
	int MakeVehicleItemStruct(const TItem &BodyItem, TVehicle &AddVehicle);

	int MakeBuyCashItem(int nItemSN, int nItemID, int nOption, TCashItemBase &OutputItem);
	int MakeGiftCashItem(int nItemSN, int nItemID, int nOption, TGiftItem &OutputItem);

	void SetInventoryCount(int nCount);
	BYTE GetInventoryCount() const;

	void SetGlyphExtendCount( int iCount );
	int	GetGlyphEntendCount() const;
	void SetGlyphExpireDate( BYTE iCount, __time64_t tExpireDate );
	TCashGlyphData* GetGlyphExpireDate();

	void SetWarehouseCount(int nCount);
	BYTE GetWarehouseCount() const;

	int GetCashInventoryCount() const;
	int GetVehicleInventoryCount() const;

#if defined(PRE_PERIOD_INVENTORY)
	void SetEnablePeriodInventory(bool bEnable, __time64_t tExpireDate);
	bool IsEnablePeriodInventory() const;
	__time64_t GetPeriodInventoryExpireDate() const;
	void SetEnablePeriodWarehouse(bool bEnable, __time64_t tExpireDate);
	bool IsEnablePeriodWarehouse() const;
	__time64_t GetPeriodWarehouseExpireDate() const;
#endif	// #if defined(PRE_PERIOD_INVENTORY)

	// Cashshop
	bool IsEffectCashItem(int nItemID) const;	// �����������̳�...?
	int CheckEffectItemListCountLimit(std::vector<TEffectItemData> &VecItemList, bool bGift, bool bIgnoreLimit, bool bReceiveGiftAll);	// �ƽ����� �Ѿ�� ����
	int CheckEffectItemCountLimit(int nItemID, int nItemCount, bool bGift, bool bIgnoreLimit);	// �ƽ����� �Ѿ�� ����
	void ApplyCashShopItem(int nItemSN, const TItem &CashItem, DWORD dwPartsColor1 = 0, DWORD dwPartsColor2 = 0, TVehicleItem *pVehiclePart1 = NULL, TVehicleItem *pVehiclePart2 = NULL, bool bDBSave = false);
	void ApplyCashShopItem(const TCashItemBase &CashData, bool bDBSave = false);
	void DBQueryCashFailItemApply(TAGetListGiveFailItem *pA);
	int CheckCashDuplicationBuy(std::vector<TEffectItemData> &VecItemList, bool bReceiveGiftAll);		// �ߺ�����üũ

	// Gachapon_JP
#if defined( PRE_ADD_GACHA_JAPAN ) && defined( _VILLAGESERVER )
	int RunGachapon_JP( int nJobClassID, int nSelectedPart, int& nResultItemID );
#endif // PRE_ADD_GACHA_JAPAN
#if defined (PRE_MOD_GACHA_SYSTEM) && defined (_VILLAGESERVER)
	char GetGachaponAddStat(const TItemData& itemData);
#endif

	void SetMyMarketList(TMyMarketInfo *pMyMarketList, int nCount);
	bool IsCashMyMarketItem(int nMarketDBID);
	void DelMyMarketList(int nMarketDBID);

	int RequestCharmItem(CSCharmItemRequest *pPacket);
	int CompleteCharmItem(CSCharmItemComplete *pPacket);
	bool CheckCharmKey(int nItemID, int nNeedKeyItemID, int nNeedKeyInvenIndex);
	bool DeleteCharmKey(int nItemID, int nNeedKeyItemID, INT64 biNeedKeySerial, int nNeedKeyInvenIndex);
	void CalcCharmDropItems(int nCharmNo, TCharmItem &ResultItem);
	int CharmItemNeedEmptySlotCount(int nItemType, int nCharmNo, int nCharmCountTableID, bool bInsertCharmList);

#if defined (PRE_ADD_CHAOSCUBE)
	bool RequestChaosCube(CSChaosCubeRequest *pPacket);
	bool CompleteChaosCube(CSChaosCubeComplete *pPacket);
	bool CalcChaosDropItems(int nChaosNo, int nDropType, int nStuffItemID, int &nResultItemID, int &nResultItemCount, int &nResultItemPeriod, int &nResultGold, bool &bMsg);
#endif
#if defined( PRE_ADD_CHOICECUBE )
	bool CalcChoiceDropItems(int nChaosNo, int nDropType, int nStuffItemID, int nResultItemID, int &nResultItemCount, int &nResultItemPeriod, int &nResultGold, bool &bMsg);
#endif

#if defined (PRE_ADD_BESTFRIEND)
	bool RequestBestFriendItem(CSBestFriendItemRequest *pPacket);
	bool CompleteBestFriendItem(CSBestFriendItemComplete *pPacket);
#endif

#if defined(_VILLAGESERVER)
	int InGuildWare(CDNGuildWare* pGuildWare, TAMoveItemInGuildWare* pMove);
	int FromInvenToGuildWare(CDNGuildWare* pGuildWare, TAMoveInvenToGuildWare* pMove);
	int FromGuildWareToInven(CDNGuildWare* pGuildWare, TAMoveGuildWareToInven* pMove);

	int NothingMoveItemInGuildWare(CDNGuildWare* pGuildWare, CSMoveGuildItem* pMove);
	int NothingMoveInvenToGuildWare(CDNGuildWare* pGuildWare, CSMoveGuildItem* pMove);
	int NothingMoveGuildWareToInven(CDNGuildWare* pGuildWare, CSMoveGuildItem* pMove, int nDailyTakeItemCount);
#endif //#if defined(_VILLAGESERVER)

#if defined(_VILLAGESERVER) && defined(PRE_ADD_CASH_REFUND)
	void LoadPaymentItem(TAPaymentItemList* pPaymentItemList);
	void LoadPaymentPackageItem(TAPaymentPackageItemList* pPaymentPackageItemList);
	inline bool GetCashMoveInven() { return m_bCashMoveInven; };
	inline void SetCashMoveInven(bool bCashMoveInven) { m_bCashMoveInven = bCashMoveInven; };
	inline bool IsPaymentItemEmpty() { return m_MapPaymentItem.empty(); };
	inline bool IsPaymentPackageItemEmpty() { return m_MapPaymentPackageItem.empty(); };
	inline int GetPaymentItemCount() { return(int)m_MapPaymentItem.size(); };
	inline int GetPaymentPackageItemCount() { return(int)m_MapPaymentPackageItem.size(); };
	int MakeSendPaymentItem(SCPaymentList* pPaymentList, int nPageNum);
	int MakeSendPaymentPackageItem(SCPaymentPackageList* pPaymentPackageList, int nPageNum);
	void AddPaymentItem(const TPaymentItemInfoEx& pPaymentItem );
	void AddPaymentPackageItem(const TPaymentPackageItemInfoEx& pPaymentPackageItem );
	int PaymentItemMoveToCashInven(INT64 biDBID); //biPurchaseOrderDetailID;
	int PaymentPackageItemMoveToCashInven(INT64 biDBID); //biPurchaseOrderDetailID;
	TPaymentItemInfoEx* GetPaymentItem(INT64 biDBID); // biPurchaseOrderDetailID;
	TPaymentPackageItemInfoEx* GetPaymentPackageItem(INT64 biDBID); // biPurchaseOrderDetailID;
	void DelPaymentItem(INT64 biDBID);
	void DelPaymentPackageItem(INT64 biDBID);
	bool IsPaymentSameItemByItemID(int nItemID);	// ���� ������ ID�� �����ְ� �����κ��� �ִ��� ����
	bool IsPaymentPackageSameItemByItemID(int nItemID);
#endif // #if defined(_VILLAGESERVER) && defined(PRE_ADD_CASH_REFUND)

	// ���ǵ� ������ ����
	void CalcMissingItem(TAMissingItemList *pList);
	void RecoverMissingItem(TARecoverMissingItem *pMissing);

	bool IsCompleteLimitlessItem();
	int GetVehicleInventoryTotalCount(){ return m_nVehicleInventoryTotalCount; }
	int GetVehicleInventoryItemCount(){ return static_cast<int>(m_MapVehicleInventory.size()); }

	int GetCashInventoryTotalCount(){ return m_nCashInventoryTotalCount; }
	int GetCashInventoryPage(){ return m_nCashInventoryPage; }
	int BuyUnionMembership(TItemData* pItemData, SHORT wCount, char &cPayMethodCode);

	int ChangeGuildMark(CSGuildMark *pPacket);

	int ProcessBuyCombinedShop( int iShopID, int iShopTabID, int iShopListID, short nCount );

#if defined(PRE_ADD_REMOVE_PREFIX)
	int RemovePrefix( int nInvenIndex, INT64 biInvenSerial, INT64 biItemSerial );
#endif // PRE_ADD_REMOVE_PREFIX
	char GetSkillPageCount() { return m_nSkillPageCount;}
	void SetSkillPageCount(int nSkillPageCount) { m_nSkillPageCount = nSkillPageCount; }
	int AddPetSkill( INT64 biItemSerial, char& cSlotNum, int& nSkillID );
	int PetSkillExpand(INT64 biItemSerial);
	int DelPetSkill( char cSlotNum);
	void SetSource(INT64 nSerial, int nItemID, time_t tExpireDate);
	TSourceData GetSource() const;
	void RemoveSource();
	void BroadcastSourceData(bool bUsedInGameServer) const;

#if defined( PRE_ADD_NAMEDITEM_SYSTEM )
	void AddEffectSkillItem( INT64 nItemSerial, int nItemID, int nSkillID, int nSkillLevel, __time64_t tExpireDate, bool bEternity = false );
	void DelEffectSkillItem( int nSkillID );
	void GetEffectSkillItem( std::vector<TEffectSkillData>& vEffectSkill );
	TEffectSkillData* GetEffectSkillItem( char cType, int nValue );	// �ϳ��� ��ü�� ������	
	void BroadcastEffectSkillItemData(bool bUsedInGameServer, int nSkillID = 0, BYTE cEffectType = EffectSkillNameSpace::ShowEffectType::NONEEFFECT);	
	void BroadcastDelEffectSkillItemData(int nItemID);	
	TItem* GetItemRank( BYTE ItemRank );
	void ApplyPartyEffectSkillItemData( CSUseItem *pPacket, TItemData *pItemData, UINT nUseSessionID, EffectSkillNameSpace::ShowEffectType::eType eType, bool bSendDB, bool bUseItem );
#endif

	void RefreshExpireitem(TADelExpiritem *pItem);

	int CheckWarpVillage(INT64 nItemSerial) const;
	int TryWarpVillage(int nMapIndex, INT64 nItemSerial);

	static bool IsExpired(const TItem& Item);
	int ProcessBuyPrivateGuildRewardItem( TGuildRewardItemData* GuildRewardItemData, short wCount );	// ��庸�� ���ο������
	int CheckPrivateGuildRewardItem(TGuildRewardItemData* GuildRewardItemData, TItemData *pItemData);	// ���ξ����� ���� ����üũ
	int	ProcessBuyPublicGuildRewardItem( TGuildRewardItem *RewardItemInfo, TGuildRewardItemData* GuildRewardItemData, UINT nGuildDBID );	// ��庸�� ȿ��������
	void CheckOneTypeCashWeapon(int nEquipIndex, bool bSend=true);

	void DBSendAddMaterializedItem(BYTE cItemSlotIndex, char cAddMaterializedItemCode, INT64 biFKey, const TItem &AddItem, INT64 biItemPrice, int nItemPeriod, INT64 biSenderCharacterDBID, 
		BYTE cItemLocationCode, BYTE cPayMethodCode, bool bMerge, INT64 biMergeTargetItemSerial, TVehicle *pVehicleEquip = NULL);

#if defined(PRE_ADD_PCBANG_RENTAL_ITEM)
	bool IsHavePcCafeRentItem();
	bool RemovePCBangRentalItem(bool bSend);
	bool RemovePCBangRentalItemVehicle(bool bSend);
	bool CreatePCRentalItem(int nItemID, int nOptionID);
#endif
#if defined(PRE_ADD_SERVER_WAREHOUSE)
	int CopySendServerWare(TItem* pData);
	int CopySendServerWareCash(TItem* pData);
#endif

	void InitializeSampleShopItem(int nSampleVersion);
	int CheckSampleShopItem(int nItemID);
	void SetSampleShopItem(int nItemID);

	int CheckModGiftReceive(TAModGiftReceiveFlag *pCashShop);
	int ModGiftReceiveFlag(const TAModGiftReceiveFlag *pCashShop);

#if defined( PRE_ITEMBUFF_COOLTIME )
	// ������Ÿ�� ITEMTYPE_GLOBAL_PARTY_BUFF �� ��Ÿ��ó��. �߰��ϰ� ������ �Լ��ȿ� ó���ؾ���		
	void GetCashInventoryCoolTime(DBPacket::TItemCoolTime* CashInventoryCoolTime);
#if defined( PRE_ADD_SERVER_WAREHOUSE )	
	void GetServerWareCoolTime(DBPacket::TItemCoolTime* SeverWareCoolTime);	
	void GetServerWareCashCoolTime(DBPacket::TItemCoolTime* ServerWareCashCoolTime);
#endif // #if defined( PRE_ADD_SERVER_WAREHOUSE )
#endif // #if defined( PRE_ITEMBUFF_COOLTIME )
	int CheckCashMail(std::vector<TItem> &VecItemList);
#if defined(PRE_ADD_EQUIPLOCK)
	//ĳ�� �κ� �����ε����� �ø���� ������ Ȯ���ϴ� �Լ�
	bool IsValidCashEquipSlot(int nIndex, INT64 biSerial, bool bCheckSerial /*= true*/) const;
	bool IsValidEquipLockSlot(BYTE cItemLocation, BYTE cItemSlotIndex, INT64 biSerial = 0);
	
	bool IsLockItem(BYTE cItemLocation, BYTE cItemSlotIndex);
	void LoadLockItem(const TAGetListLockedItems* pLockItemList);
	void GetLockItemList(int &nEquipLockCount, int &nCashEquipLockCount, EquipItemLock::TLockItemInfo* pLockList);
	void LockEquipItem(BYTE cItemLocation, BYTE cItemSlotIndex, __time64_t LockDate);
	void RequestUnLockEquipItem(BYTE cItemLocation, BYTE cItemSlotIndex, __time64_t LockDate, __time64_t UnLockRequestDate);
	void UnLockEquipItem(BYTE cItemLocation, BYTE cItemSlotIndex, bool CheckValidItem = true);

	bool CheckItemUnLock(BYTE cItemLocation, BYTE cItemSlotIndex);
#endif	// #if defined(PRE_ADD_EQUIPLOCK)
#if defined( PRE_ADD_STAGE_USECOUNT_ITEM )
	int GetUseLimitItemCount(int nItemID);
	void DelUseLimitItemCount(int nItemID, int nCount=1);
#endif
#if defined(PRE_ADD_WORLD_MSG_RED)
	bool IsVaildWorldChatItem(char cChatType, INT64 biItemSerial, int &nItemID);
#endif	// #if defined(PRE_ADD_WORLD_MSG_RED)

	void GetInventoryList(TItemInfo *InventoryList, BYTE &cTotalCount);
	void GetWarehouseList(TItemInfo *WarehouseList, BYTE &cTotalCount);
};
