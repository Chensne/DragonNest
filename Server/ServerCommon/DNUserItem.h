#pragma once

/*---------------------------------------------------------------------------------------
									CDNUserItem

	- 유저 캐릭터 아이템쪽 담당하는 클래스
	- VillageServer, GameServer가 공통으로 가지고 있는 클래스 이므로
      if defined 떡칠(?)함 ㅋㅋㅋ
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

		// 쿨타임 시간 지났으면 삭제
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

	TItem m_Equip[EQUIPMAX];							// 장비창
	TItem m_Glyph[GLYPHMAX];							// 문장
#if defined(PRE_PERIOD_INVENTORY)
	TItem m_Inventory[INVENTORYTOTALMAX];				// 인벤창
	TItem m_Warehouse[WAREHOUSETOTALMAX];				// 창고창
#else	// #if defined(PRE_PERIOD_INVENTORY)
	TItem m_Inventory[INVENTORYMAX];					// 인벤창
	TItem m_Warehouse[WAREHOUSEMAX];					// 창고창
#endif	// #if defined(PRE_PERIOD_INVENTORY)
	TQuestItem m_QuestInventory[QUESTINVENTORYMAX];		// 퀘스트 인벤창
#if defined(PRE_ADD_TALISMAN_SYSTEM)
	TItem m_Talisman[TALISMAN_MAX];						// 탈리스만
#endif
#if defined(PRE_ADD_EQUIPLOCK)
	EquipItemLock::TLockItem m_EquipLock[EQUIPMAX];
	EquipItemLock::TLockItem m_CashEquipLock[CASHEQUIPMAX];
#endif	// #if defined(PRE_ADD_EQUIPLOCK)

	TItem m_CashEquip[CASHEQUIPMAX];					// 장비창(캐쉬)
	TUnionMembership m_UnionMembership[NpcReputation::UnionType::Etc]; // 연합호감도

	typedef std::map<INT64, TItem> TMapItem;
	TMapItem m_MapCashInventory;				// 인벤창(캐쉬) first: Serial

#if defined(_VILLAGESERVER) && defined(PRE_ADD_CASH_REFUND)
	typedef std::map<INT64, TPaymentItemInfoEx> TMapPaymentItem;
	TMapPaymentItem m_MapPaymentItem;					// 결재 인벤(단품)
	typedef std::map<INT64, TPaymentPackageItemInfoEx> TMapPaymentPackageItem;
	TMapPaymentPackageItem m_MapPaymentPackageItem;	// 결재 인벤(패키지)
	bool m_bCashMoveInven;
#endif // #if defined(_VILLAGESERVER) && defined(PRE_ADD_CASH_REFUND)
#if defined(PRE_ADD_SERVER_WAREHOUSE)
	TMapItem m_MapServerWarehouse;					// 서버 창고 일반..서버 통합용으로 150개까지 기본 제한은 GlobalWeightTable
	TMapItem m_MapServerWarehouseCash;				// 서버 창고 캐쉬
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
	//한번에 세개가 열린다는데 혹시 한개씩 열리게 될지도 모르기 때문에 각각의 문장슬롯에 대한 정보를 저장
	TCashGlyphData	m_TCashGlyphData[CASHGLYPHSLOTMAX];
#if defined(PRE_ADD_TALISMAN_SYSTEM)
	int m_nTalismanOpenFlag;
	bool m_bTalismanCashSlotEntend;
	__time64_t	m_tTalismanExpireDate;	// 탈리스만 캐쉬 슬롯 기간	
#endif

	bool m_bMoveItemCheckGameMode;

	TExchangeData m_ExchangeData[EXCHANGEMAX];		// 거래리스트
	INT64 m_nExchangeCoin;		// 거래 코인
	bool m_bExchangeRegist;		// 거래 물품 등록
	bool m_bExchangeConfirm;	// 거래 최종 확인

	int m_nDailyMailCount;		// 1일 보낼 수 있는 우편수
	int m_nWeeklyRegisterCount;		// 토요일 AM 04:00 를 기준으로 일주일간 등록한 거래 횟수 / 지금등록할수있는 횟수
	int m_nRegisterItemCount;		// 올려놓은 아이템 갯수

	// 뭔가 요청 관련 연출 대기했다가 처리하는 것들을 위해서 일단 만들어놔봤어요
	RequestTypeEnum m_RequestType;
	DWORD m_dwRequestTimer;
	DWORD m_dwRequestInterval;

	DBPacket::TTakeAttachInfo m_ReadMailAttachItem;	// 
	std::vector<TMyMarketInfo> m_VecMyMarketList;	// backup용?
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

	TItem m_Source; // 근원
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
	int _FromPetBodyToVehicleInven(const CSMoveCashItem *pMove);	//PetBody를 펫슬롯에서 탈것&펫 인벤으로(해제)
	int _FromVehicleInvenToPetBody(const CSMoveCashItem *pMove);	//PetBody를 탈것&펫 인벤에서 펫 슬롯으로(소환)
	int _FromPetPartsToCashInven(const CSMoveCashItem *pMove);		//PetParts를 펫 슬롯에서 캐쉬인벤으로(해제)
	int _FromCashInvenToPetParts(const CSMoveCashItem *pMove);		//PetParts를 캐쉬인벤에서 펫 슬롯으로(착용)

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

	// 깔끔하게 바꿔치기용 함수들
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

	// Equip관련 함수
	int _PushEquipSlot(int nIndex, const TItem &AddItem);
	int _PopEquipSlot(int nIndex, short wCount, bool bSend=true );
	// Glyph관련 함수
	int _PushGlyphSlot(int nIndex, const TItem &AddItem);
	int _PopGlyphSlot(int nIndex, short wCount, bool bSend=true );
	// inven관련 함수
	int _PushInventorySlotItem(int nIndex, const TItem &AddItem);
	int _PopInventorySlotItem(int nIndex, short wCount);
	// CashEquip
	int _PushCashEquipSlot(int nIndex, const TItem &AddItem, bool bSubParts = false);
	int _PopCashEquipSlot(int nIndex, short wCount);
	// Cash inven관련 함수
	int _PushCashInventory(const TItem &AddItem);
	int _PopCashInventoryBySerial(INT64 biSerial, short wCount);
	// 창고 관련 함수
	int _PushWarehouseSlot(int nIndex, const TItem &AddItem);
	int _PopWarehouseSlot(int nIndex, short wCount);
	// 퀘스트 인벤 관련 함수
	int _PushQuestInventorySlot(int nIndex, const TQuestItem &AddItem);
	int _PopQuestInventorySlot(int nIndex, short wCount);
	// 탈것
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
	// Talisman관련 함수
	int _PopTalismanSlot(int nIndex, short wCount, bool bSend=true );
#endif

	// exchange
	int _FindBlankExchangeIndex();	// 빈 슬롯인덱스 찾기
	bool _ExistExchangeInven(int nInvenIndex);	// 이미 있는 인벤아이템을 또 다른곳에다 넣는다면 막아야한다

	// cooltime 관련
	void _SetItemCoolTime( const int nSkillID, const int nMaxCoolTime );				// 같은 skillid 가지고 있으면 같은 쿨타임으로...
	void _UpdateInventoryCoolTime( const int nIndex, const DWORD dwCompareTime=0 );	// Update cooltime
	void _UpdateCashInventoryCoolTime( const INT64 biSerial, const DWORD dwCompareTime=0 );	// Update cooltime
	void _UpdateWarehouseCoolTime( const int nIndex, const DWORD dwCompareTime=0 );	// Update cooltime

	// Repair(내구도)
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

	static INT64 MakeItemSerial();		// Serial 만들기
	static INT64 MakeCashItemSerial(UINT nDay);		// CashItem Serial 만들기

	void DoUpdate(DWORD CurTick);	

	void LoadUserData(TASelectCharacter *pData);
	void SaveUserData();
	void RemoveInstantItemData( bool bSend );
	void RemoveInstantVehicleItemData( bool bSend );
	void RemoveInstantEquipVehicleData( bool bSend ); // 현재 착용중인 탈것 전용

	void RemoveGuildReversionItem( bool bSend );
	void RemoveGuildReversionVehicleItemData( bool bSend );
	void ResetCoolTime();
#if defined( PRE_ADD_FARM_DOWNSCALE )
	static void CalcDropItems( CDNUserSession* pSesion, int nDropItemTableID, int &nResultItemID, int &nResultItemCount, int &nLastValue, int &nDepth );
#else
	void CalcDropItems( int nDropItemTableID, int &nResultItemID, int &nResultItemCount, int &nLastValue, int &nDepth );
#endif // #if defined( PRE_ADD_FARM_DOWNSCALE )

	//------ 패킷처리용 함수(?) -> 무언가 Send를 날리는 함수들 묶음
	bool OnRecvMoveItem(const CSMoveItem *pPacket);	// Move Process
	bool OnRecvMoveCashItem(const CSMoveCashItem *pPacket);	// Move Process
#if defined(PRE_ADD_SERVER_WAREHOUSE)
	bool OnRecvMoveServerWareItem(const CSMoveServerWare* pPacket);
	bool OnRecvMoveServerWareCashItem(const CSMoveCashServerWare* pPacket);
#endif // #if defined(PRE_ADD_SERVER_WAREHOUSE)
#if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
	bool OnRecvPickUp(TItem& resultItemInfo, int nItemID, short wCount, int nRandomSeed, char cOption, int nEnchantID = 0);						// 바닥 -> Inven
#else	// #if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
	bool OnRecvPickUp(TItem& resultItemInfo, int nItemID, short wCount, int nRandomSeed, char cOption);						// 바닥 -> Inven
#endif	// #if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
	bool OnRecvRemoveItem(const CSRemoveItem *pPacket);		// Equip, Inven -> 휴지통
	int OnRecvRemoveCash(INT64 biItemSerial, bool bRecovery);
#if defined(PRE_ADD_REMOTE_OPENSHOP)
	void OnRecvShopRemoteOpen(Shop::Type::eCode eType);
#endif	// #if defined(PRE_ADD_REMOTE_OPENSHOP)
	int OnRecvBuyNpcShopItem(int nShopID, char cShopTabID, BYTE cShopIndex, short wCount);			// npcshop 구매하기
	int OnRecvSellNpcShopItem(const CSShopSell *pPacket);						// npcshop 판매하기
	int OnRecvShopRepurchase( const CSShopRepurchase* pPacket );
	int OnRecvRepairEquip( int iUseItemID=0 );	// 장착수리(equip)
	void OnRecvRepairAll( bool bCheat=false );	// 전체수리(equip + inven)
#if defined(PRE_ADD_REPAIR_NPC)
	int CheckRepairEquip( int iUseItemID ); // 장착수리(equip)
	int CheckRepairAll( bool bCheat );	// 전체수리(equip + inven)
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

	// inven에 아이템 넣기(개수 상관없이 걍 다 쑤셔넣기) - 줏어서 얻거나(Pickup), 퀘스트에서 넣어주거나(db 저장 포함)
	int CreateInvenItem1(int nItemID, short wCount, char cOption, int nRandomSeed, int nLogCode, INT64 biFKey, char cType = CREATEINVEN_ETC);
	int CreateInvenItem2(int nItemID, short wCount, std::vector<TSaveItemInfo> &VecItemList, char cType = CREATEINVEN_ETC, int nShopItemPeriod = 0);	// 디비 로그남길때 쓰는 함수
	int CreateInvenWholeItem(const TItem &Item, int nLogCode, INT64 biFKey, char cType = CREATEINVEN_ETC);	// 이미 있는 아이템 인벤에 넣을때 쓰는 함수
	int CreateInvenWholeItemByIndex(int nInvenIndex, const TItem &Item);	// 걍 인덱스에 박아버리는거

	// Cash, VehicleInven(ItemType에 따라 일반 캐쉬인지 탈것인지 구분)
	int CreateCashInvenItem(int nItemID, int nItemCount, int nLogCode, char cOption = -1, int nPeriod = 0, int nPrice = 0, INT64 biFKey = 0, BYTE cPayMethodCode = DBDNWorldDef::PayMethodCode::Coin);	// 가챠같이 ItemSN없이 ItemID로만 캐쉬템 생성하는거
	int CreateCashInvenWholeItem(const TItem &Item);	// 걍 박아버리는거
	int CreateCashInvenItemByCheat(int nItemID, short wCount, int nPeriod, int nLogCode);
#if defined(PRE_LEVELUPREWARD_DIRECT)
	bool CreateCashInvenItemByMailID(int nMailID);
#endif	// #if defined(PRE_LEVELUPREWARD_DIRECT)

	bool DeleteItemByUse(int nInvenType, int nInvenIndex, INT64 biInvenSerial, bool bDBSave=true );	// 해당 인벤 인덱스에 있는 셀수있는 아이템 1개씩 지워주기(db 저장 포함)
	bool UseItemByType(int nType, int nUseCount, bool bSend, int nTypeParam1 = -1, int iTargetLogItemID=0, char cTargetLogItemLevel=0 );
	bool UseItemByItemID(int iItemID, int nUseCount, bool bSend, int nTypeParam1 = -1);

	//-------------------------------------------------------------------------------------------------------------------------------------------------

	// Index로 TItem 얻기
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

	// Equip관련 함수
	int GetEquipSlotIndex( int nItemID ) const;
	int FindBlankEquipSlot();									// equip에 빈슬롯 찾기
	void GetEquipIDs(int *EquipArray) const;	// 장착 아이템들 얻기
	void GetEquipItemDurability(std::vector<INT64> &VecSerialList, std::vector<USHORT> &VecDurList) const;
#if defined(PRE_ADD_TALISMAN_SYSTEM)
	void GetEquipList(char &cCount, char &cCashCount, char &cGlyphCount, char &cTalismanCount, TItemInfo *EquipArray) const;
#else
	void GetEquipList(char &cCount, char &cCashCount, char &cGlyphCount, TItemInfo *EquipArray) const;
#endif
	void BroadcastChangeEquip(int nSlotIndex, const TItem &EquipItem);	// 주변 애들에게 Equip바뀐거 뿌려주기
	void ChangeEquipItemDurability( int nIndex, USHORT wDur );
	bool ChangeDefaultParts( int nItemType, const int *pTypeParam, INT64 biUseInvenItemSerial ); // 얼굴, 헤어 등 Default 파츠 변경
	int IsEquipEnableItem(const TItem& equipExpected) const;
	bool IsEquipItem(int nSlotIndex, int nItemID) const;		// Equip Slot에 맞는 장착아이템인지 검사
	bool CheckEquipByItemID(int nItemID) const;			// Equip 에 특정 아이템을 장착했는지 검사
#if defined(PRE_ADD_EQUIPED_ITEM_ENCHANT) || defined(PRE_ADD_EQUIPLOCK)
	bool IsValidEquipSlot(int nIndex, INT64 biSerial, bool bCheckSerial = true) const;	// Equip 에 정말 장착 했는지
#endif	// #if defined(PRE_ADD_EQUIPED_ITEM_ENCHANT) ||  || defined(PRE_ADD_EQUIPLOCK)

	// CashEquip
	int TakeOffOnePiece(int nItemID);	// 캐쉬템 벗기
	int GetCashEquipOnePieceMainParts(int nEquipItemID);	// 한벌옷일때 메인파츠찾기
	void BroadcastChangeCashEquip(int nSlotIndex, const TItem &EquipItem);	// 주변 애들에게 CashEquip바뀐거 뿌려주기
	bool IsEquipCashItemExist(int nItemID) const;		// Equip 에 특정 캐쉬 아이템을 장착했는지 검사

	// Glyph
	bool IsGlyphItem(int nSlotIndex, int nItemID);		// Glyph Slot에 맞는 아이템인지 검사
	void BroadcastChangeGlyph(int nSlotIndex, const TItem &GlyphItem);	// 주변 애들에게 Glyph바뀐거 뿌려주기

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

	// inven관련 함수
	bool CheckRangeInventoryIndex(int nIndex) const;
	bool AddInventoryByQuest(int nItemID, short wCount, int nQuestID, int nRandomSeed);	// itemid, count, randomseed에 의해 inventory에 넣기
	bool AddInventoryByQuest( const TQuestReward::_ITEMSET* pItemSet, int nQuestID, int nRandomSeed );	// TQuestReward::_ItemSet에 의해 inventory에 넣기
	bool DeleteInventoryByQuest(int nItemID, short wCount, int nQuestID);				// ItemID가지고 inven에 빼기(db 저장 포함)
	bool DeleteInventoryByItemID(int nItemID, int iCount, int nLogCode, INT64 biFKey=0 );// ItemID가지고 inven에 빼기(db 저장 포함)
	bool DeleteInventoryExByItemID(int nItemID, char cOption, int iCount, int nLogCode, INT64 biFKey, std::vector<TItemInfo> &VecItemList, bool bInsertList);		// ItemID가지고 inven에 빼기(db 저장 포함) - 보통 list를 
	bool DeleteInventoryBySlot(int nSlotIndex, short wCount, INT64 biSerial, int nLogCode);					// SlotIndex가지고 inven에 빼기(db 저장 포함)
	bool DeleteInventoryByType(int nType, short wCount, int nLogCode, int nTypeParam1 = -1, int iTargetLogItemID=0, char cTargetLogItemLevel=0 );
	int FindInventorySlot(int nItemID, short wCount) const;	// 해당 ItemID, Count로 inven에서 index찾기
	int FindInventorySlot(int nItemID, char cOption, short wCount, bool bSoulBound = false ) const;
	int FindInventorySlotBySerial( INT64 biSerial ) const;
	bool CheckEnoughItem( int iItemID, int iCount ) const;
	int	GetInventoryItemCount(int nItemID, char cOption = -1) const;						// id에 해당하는 아이템 몇개가 있는지
	int GetInventoryItemCountByType(int nType, int nTypeParam1 = -1) const;
	bool IsValidSpaceInventorySlot(int nItemID, short wCount, bool bSoulBound, char cSealCount, bool bEternity) const;	// 아이템을 넣을 수 있는 공간이 충분한지 검사
	int IsValidSpaceInventorySlotFromShop( const TItemData* pItemData, int iCheckCount ) const;
	int FindBlankInventorySlot() const;									// inven에 빈슬롯 찾기
	int FindBlankInventorySlotCount() const;									// inven에 빈슬롯이 몇개나 있는지 개수찾기
	int FindBlankInventorySlotCountList(std::queue<int> &qSlotList) const;
	int FindOverlapInventorySlot(int nItemID, short wCount, bool bSoulBound, char cSealCount, bool bEternity) const;	// inven에 셀수있는 아이템중 남은 공간에 wCount 넣을 수 있는 Index찾기
	bool IsValidInventorySlot(int nIndex, INT64 biSerial, bool bCheckSerial = true) const;	// inven에 정말 있는지
	bool VerifyInventorySlotPacket(int nInvenIndex, const TItem &Item) const;	// 무인상점, 우편 같이 index받아서 디비 갔다온 아이템 검사하기
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

	// Cash inven관련 함수
	void LoadCashInventory(int nPageNum, int nTotalCount, int nCount, TItem *CashItemList);
	bool UseCashItemByType(int nType, int nUseCount, bool bSend);
	bool DeleteCashInventoryByItemID(int nItemID, short wCount, int nLogCode, INT64 biFKey=0 );				// ItemID가지고 inven에 빼기(db 저장 포함)
	bool DeleteCashInventoryBySerial(INT64 biInvenSerial, short wCount, bool bSaveDB = true);					// SlotIndex가지고 inven에 빼기(db 저장 포함)
	void ChangeCashInventoryItemDurability(INT64 biSerial, USHORT wDur);
	int	GetCashItemCountByItemID(int nItemID);						// id에 해당하는 아이템 몇개가 있는지
	int GetCashItemCountByType(int nType, int nTypeParam1 = -1);	// 저 타입의 캐쉬템 몇개나 가지고있나
	void GetCashInventoryItemListByType( int nType, std::vector<const TItem*>& VecResult, int nTypeParam1=-1 );
	TItem *GetCashItemByType(int nType);	// 타입인 아이템 데이터값
	bool IsValidCashItem(int nItemID, short wCount);	// 캐쉬아이템이 있는지
#if defined(_GAMESERVER)
	void InitializePlayerCashItem(CDnItemTask *pItemTask);
#endif	// #if defined(_GAMESERVER)
	void CheatClearCashInven();
	int GetExtendPrivateFarmFieldCount();
	void OnRemoveCashItem(TADelCashItem *pDel);
	void OnRecoverCashItem(TARecoverCashItem *pRecover);

	// ware관련 함수
	bool IsValidWarehouseSlot(int nIndex);	// ware에 정말 있는지
	int	GetWarehouseItemCount(int nItemID) const;						// id에 해당하는 아이템 몇개가 있는지
	int FindBlankWarehouseSlotCountList(std::queue<int> &qSlotList);
	bool SortWarehouse(CSSortWarehouse * pPacket);
#if defined(PRE_PERIOD_INVENTORY)
	bool SortPeriodWarehouse(CSSortWarehouse * pPacket);
#endif	// #if defined(PRE_PERIOD_INVENTORY)

	void GetIndexSerialList(int &nTotalInvenWareCount, DBPacket::TItemIndexSerial *SaveList);

	// Quest Inventory 관련
	int AddQuestInventory(int nItemID, short wCount, int nQuestID, int nLogCode);	// -> Quest Inventory(넣기)
	int DeleteQuestInventory(int nItemID, short wCount, int nQuestID, int nLogCode);	// <- Quest Inventory(버리기)
	int FindQuestInventorySlot(int nItemID, short wCount);			// 해당 ItemID, Count로 quest inven에서 index찾기
	int FindBlankQuestInventorySlot();								// inven에 빈슬롯 찾기
	int FindOverlapQuestInventorySlot(int nItemID, short wCount);	// inven에 셀수있는 아이템중 남은 공간에 wCount 넣을 수 있는 Index찾기
	int FindBlankQuestInventorySlotCountList(std::queue<int> &qSlotList);
	int FindBlankQuestInventorySlotCount();								// inven에 빈슬롯이 몇개나 있는지 개수찾기
	int	GetQuestInventoryItemCount(int nItemID);						// id에 해당하는 아이템 개수
	void CheatClearQuestInven();
	bool IsValidSpaceQuestInventorySlot(int nItemID, short wCount);		// 퀘스트 아이템을 넣을 수 있는 공간이 충분한지 검사

	// 탈것
	int GetVehicleBodyItemID();
	void LoadVehicleInventory(const TAGetPageVehicle *pVehicle);
	void CheatClearVehicleInven();
	// VehicleEquip
	bool IsVehicleEquipItem(int nSlotIndex, int nItemID);		// Vehicle에 맞는 아이템인지 검사
	bool ChangeVehicleDyeColor(const int *pTypeParam);
	void BroadcastChangeVehicleParts(int nSlotIndex, const TItem &EquipItem);	// 주변 애들에게 Equip바뀐거 뿌려주기

	// Pet
	int GetPetBodyItemID();
	INT64 GetPetBodySerial();
	int GetPetExp();
	int ModItemExpireDate(const CSModItemExpireDate* pItem, int nMin);	// 아이템 기간 연장.
	void UpdatePetExp(int nExp);
	bool ChangePetBodyColor(const int *pTypeParam);
	bool ChangePetNoseColor(const int *pTypeParam);
	bool AddPetExp(int nAddPetExp);
	void ClearPetVehicleInven();
	void ItemExpireByCheat(INT64 biItemSerial);
	void BroadcastChangePetParts(int nSlotIndex, TItem &EquipItem);
	void BroadcastChangePetBody(const TVehicle &PetInfo);
	void BroadcastChangeVehiclePetColor(INT64 biSerial, DWORD dwColor, char cPetPartsColor);	// 주변 애들에게 Equip바뀐거 뿌려주기
	int ChangePetName(const TAChangePetName *pPacket);
	void ModPetExpireDate(const TAModItemExpireDate *pPacket);
	int GetPetLevel();	// 펫 레벨얻어오기(빌리지서버는 가지고 있는 펫중 최고레벨, 게임서버는 현재소환되어있는 펫 레벨)
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

	// 우편
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

	// 무인상점
	int CheckRegisterMarketItem(CSMarketRegister *pPacket);	// market 등록하기전에 체크
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

	// 개인거래
	int AddExchangeItem(CSExchangeAddItem *pPacket);	// 거래 아이템 세팅하기
	int DeleteExchangeItem(char cExchangeIndex);	// 거래 아이템 지우기
	int AddExchangeCoin(INT64 nCoin);					// 거래 코인 세팅
	void ClearExchangeData();						// 거래 아이템 초기화
	bool IsValidExchange(int nExchangeCount);	// 거래가 가능한지(거래아이템이 인벤에 다 들어갈 수 있는지)
	int FindExchangeCount();		// 거래아이템이 몇개인가
	INT64 GetExchangeCoin();
	bool CompleteExchange(CDNUserSession *pTargetSession, DBPacket::TExchangeItem *ExchangeInfo, int &nExchangeTax);	// 거래 완료하기(내 아이템 상대방 인벤으로 넣어주기)
	inline void SetExchangeRegist() { m_bExchangeRegist = true; }
	inline bool GetExchangeRegist() { return m_bExchangeRegist; }

	inline void SetExchangeConfirm() { m_bExchangeConfirm = true; }
	inline bool GetExchangeConfirm() { return m_bExchangeConfirm; }

	void SetEquipItemDurability( int nIndex, USHORT wDur, bool bSend=false );
	void SetInvenItemDurability( int nIndex, USHORT wDur );

	// 강화, 잠재력
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

	// 랜덤 아이템
	bool RequestRandomItem(CSUseItem *pPacket);
	bool CalcRandomItem(CSCompleteRandomItem *pPacket);
	// 월드대화 캐시아이템
	bool RequestWorldMsgItem(CSUseItem *pPacket);

	// 스킬리셋 아이템
	bool UseSkillResetItem( int nIndex );

	// 포텐셜 보석
	int PotentialItem( int nInvenIndex, INT64 biInvenSerial, INT64 biItemSerial );

	int PotentialItem888(int nInvenIndex, INT64 biInvenSerial, INT64 biItemSerial);

	int PotentialItem999(int nInvenIndex, INT64 biInvenSerial, INT64 biItemSerial);

	// 길드명 변경 아이템
	void RequestChangeGuildNameItem(CSGuildRename *pPacket);

	// 캐릭터명 변경 아이템
	void RequestChangeCharacterNameItem(CSCharacterRename *pPacket);
	bool CanChangeCharacterName();

	void RequestChangePetName(CSChangePetName *pPacket);

	int EnchantJewel( int nInvenIndex, INT64 biInvenSerial, INT64 biItemSerial );

	void SetRequestTimer( RequestTypeEnum Type, DWORD dwInterval );
	bool IsValidRequestTimer( RequestTypeEnum Type );

	CDNMissionSystem *GetMissionSystem();

	void OnRecvMoveCoinInventoWare(char cType, INT64 nMoveCoin);
	void OnRecvMoveCoinWaretoInven(char cType, INT64 nMoveCoin);

	static bool MakeItemStruct(int nItemID, TItem &ResultItem, int nShopItemPeriod = 0, int nOptionTableID = 0);	// 여기저기 코드 산재해있는듯하여 이곳으로 합쳤음. createinven할때도 여기서 얻어옴
	static bool bIsSameItem( const TItem* pItem, const TItem* pItem2 );
	static bool bIsDifferentItem( const TItem* pItem, const TItem* pItem2 );
	static bool bIsSameItem( const TItem* pItem, const TInvenItemCnt* pItem2 );
	static bool bIsDifferentItem( const TItem* pItem, const TInvenItemCnt* pItem2 );
#if defined(PRE_MOD_POTENTIAL_JEWEL_RENEWAL)
	//잠재력 초기화
	void ResetPrevPotentialData(){ memset(&m_PrevPotentialItem, 0, sizeof(m_PrevPotentialItem));}
	//잠재력 부여 취소(이전 잠재력으로 복구)
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
	bool IsEffectCashItem(int nItemID) const;	// 무형아이템이냐...?
	int CheckEffectItemListCountLimit(std::vector<TEffectItemData> &VecItemList, bool bGift, bool bIgnoreLimit, bool bReceiveGiftAll);	// 맥스값이 넘어가면 리턴
	int CheckEffectItemCountLimit(int nItemID, int nItemCount, bool bGift, bool bIgnoreLimit);	// 맥스값이 넘어가면 리턴
	void ApplyCashShopItem(int nItemSN, const TItem &CashItem, DWORD dwPartsColor1 = 0, DWORD dwPartsColor2 = 0, TVehicleItem *pVehiclePart1 = NULL, TVehicleItem *pVehiclePart2 = NULL, bool bDBSave = false);
	void ApplyCashShopItem(const TCashItemBase &CashData, bool bDBSave = false);
	void DBQueryCashFailItemApply(TAGetListGiveFailItem *pA);
	int CheckCashDuplicationBuy(std::vector<TEffectItemData> &VecItemList, bool bReceiveGiftAll);		// 중복구매체크

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
	bool IsPaymentSameItemByItemID(int nItemID);	// 같은 아이템 ID를 가진애가 결재인벤에 있는지 여부
	bool IsPaymentPackageSameItemByItemID(int nItemID);
#endif // #if defined(_VILLAGESERVER) && defined(PRE_ADD_CASH_REFUND)

	// 유실된 아이템 복구
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
	TEffectSkillData* GetEffectSkillItem( char cType, int nValue );	// 하나의 객체만 얻어오기	
	void BroadcastEffectSkillItemData(bool bUsedInGameServer, int nSkillID = 0, BYTE cEffectType = EffectSkillNameSpace::ShowEffectType::NONEEFFECT);	
	void BroadcastDelEffectSkillItemData(int nItemID);	
	TItem* GetItemRank( BYTE ItemRank );
	void ApplyPartyEffectSkillItemData( CSUseItem *pPacket, TItemData *pItemData, UINT nUseSessionID, EffectSkillNameSpace::ShowEffectType::eType eType, bool bSendDB, bool bUseItem );
#endif

	void RefreshExpireitem(TADelExpiritem *pItem);

	int CheckWarpVillage(INT64 nItemSerial) const;
	int TryWarpVillage(int nMapIndex, INT64 nItemSerial);

	static bool IsExpired(const TItem& Item);
	int ProcessBuyPrivateGuildRewardItem( TGuildRewardItemData* GuildRewardItemData, short wCount );	// 길드보상 개인용아이템
	int CheckPrivateGuildRewardItem(TGuildRewardItemData* GuildRewardItemData, TItemData *pItemData);	// 개인아이템 구매 조건체크
	int	ProcessBuyPublicGuildRewardItem( TGuildRewardItem *RewardItemInfo, TGuildRewardItemData* GuildRewardItemData, UINT nGuildDBID );	// 길드보상 효과아이템
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
	// 아이템타입 ITEMTYPE_GLOBAL_PARTY_BUFF 만 쿨타임처리. 추가하고 싶으면 함수안에 처리해야함		
	void GetCashInventoryCoolTime(DBPacket::TItemCoolTime* CashInventoryCoolTime);
#if defined( PRE_ADD_SERVER_WAREHOUSE )	
	void GetServerWareCoolTime(DBPacket::TItemCoolTime* SeverWareCoolTime);	
	void GetServerWareCashCoolTime(DBPacket::TItemCoolTime* ServerWareCashCoolTime);
#endif // #if defined( PRE_ADD_SERVER_WAREHOUSE )
#endif // #if defined( PRE_ITEMBUFF_COOLTIME )
	int CheckCashMail(std::vector<TItem> &VecItemList);
#if defined(PRE_ADD_EQUIPLOCK)
	//캐쉬 인벤 슬롯인덱스랑 시리얼로 아이템 확인하는 함수
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
