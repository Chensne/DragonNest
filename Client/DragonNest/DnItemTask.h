#pragma once
#include "Task.h"
//#include "ClientTcpSession.h"
//#include "ClientUdpSession.h"
#include "MessageListener.h"
#include "DnItem.h"
#include "DnParts.h"
#include "DNProtocol.h"
#include "DNPacket.h"
#include "DnCustomControlCommon.h"
#include "DnItemCompounder.h"
#include "DnCharInventory.h"
#include "DnStorageInventory.h"
#include "DnGuildInventory.h"
#include "DnCashInventory.h"

#ifdef PRE_ADD_CASHREMOVE
#include "DnCashRemoveInventory.h"
#endif
#ifdef PRE_ADD_SERVER_WAREHOUSE
#include "DnWorldServerStorageInventory.h"
#include "DnUIDefine.h"
#endif

#include "DnPlayerActor.h"
#ifdef PRE_MONITOR_SUPER_NOTE
#include "DnIllegalActivityMonitorAgentMgr.h"
#endif
#include "DnCostumeMixDataMgr.h"
#ifdef PRE_ADD_COSRANDMIX
#include "DnCostumeRandomMixDataMgr.h"
#endif

#include "DnPetInventory.h"
#ifdef PRE_ADD_CASHSHOP_REFUND_CL
#include "DnRefundCashInventory.h"
#endif
#ifdef PRE_ADD_EQUIPLOCK
#include "DnItemLockMgr.h"
#endif

#define ILLEGAL_REBIRTH_COIN_COUNT -1

class CDnEmblemFactory;

class CDnItemTask : public CTask, public CTaskListener, public CSingleton<CDnItemTask>
{
public:
	CDnItemTask();
	virtual ~CDnItemTask();

	enum DropItemEnum 
	{
		DEFINE_ITEM,
		CREATE_ITEM,
	};

	enum EquipDurabilityStateEnum 
	{
		Durability_None,
		Durability_Yellow,
		Durability_Orange,
		Durability_Red,
	};

	struct ItemCompoundInfo
	{
		int iItemID;
		int iUseCount;

		ItemCompoundInfo() : iItemID( 0 ), iUseCount( 0 ) {}
	};

	struct SFatigue
	{
		int nMaxFatigue;	// 최대 피로도값
		int nFatigue;		// 현재값

		SFatigue() : nMaxFatigue(0), nFatigue(0) {}
	};

	struct SCashGlyph
	{
		bool bOpen;
		__time64_t tTime;

		SCashGlyph() : bOpen( false ), tTime( -1 ) {}

		void Clear()
		{
			bOpen = false;
			tTime = -1;
		}
	};

#ifdef PRE_ADD_TALISMAN_SYSTEM
	struct SCashTalisman
	{
		bool bOpenSlot;
		bool bRecvExpirePacket;
		__time64_t tTime;

		SCashTalisman() : bOpenSlot(false), bRecvExpirePacket(false), tTime(-1) {}

		void Clear()
		{			
			bOpenSlot = false;
			bRecvExpirePacket = false;
			tTime = -1;
		}
	};
#endif

#ifdef PRE_MOD_ITEM_COMPOUND_DIALOG_RENEWAL
	struct CompoundItemPackage
	{	
		int nCompoundID;
		INT64 nCost;
		std::vector<CDnItemTask::ItemCompoundInfo> vNeedItemInfo;
	};
#endif 

protected:

	enum
	{
		// 기본 퀵슬롯안에 제스처퀵슬롯을 포함시켰기때문에, 퀵슬롯 오프셋을 염두해두고 인덱스를 구해야한다.
		// 현재 기본스킬 10*2개가 앞에 존재한다.
		// 이부분 슬롯인덱스 변경되는경우가 생기면 수정해주어야한다, 공용 디파인으로 빼던지 해야할듯
		LIFESKILL_QUICKSLOT_OFFSET = QUICKSLOTSEPARATE,
	};

	// 돈을 인벤토리에 종속적으로 해야할까..
	INT64 m_nCoin;			// 캐릭 인벤토리 돈
	INT64 m_nCoinStorage;	// 창고돈
	INT64 m_nCoinGuildWare;

	// 길드창고는 다른 창고와 달리 하루 사용제한이 있다.
	int m_nTakeGuildWareItemCount;
	INT64 m_biWithdrawGuildWareCoin;
	bool m_bReceivedGuildWare;

	// 부활 코인
	int m_nRebirthCoinCount;
	int m_nRebirthCashCoinCount;
	int m_nRebirthPCBangCoinCount;
	int m_nUsableRebirthCoinCount;
	int m_nMaxUsableRebirthCoincount;
#ifdef PRE_ADD_VIP
	int m_nVIPRebirthCoinCount;
#endif
	int m_nSpecialRebirthItemCount;
	std::vector<int> m_vSpecailRebirthItemIDs;
	
	SFatigue m_sFatigue;
	SFatigue m_sWeekFatigue;
	SFatigue m_sPCBangFatigue;
	SFatigue m_sEventFatigue;
#ifdef PRE_ADD_VIP
	SFatigue m_sVIPFatigue;
#endif

#if defined( PRE_USA_FATIGUE )
	int m_nNoFatigueExpValue;
	int m_nFatigueExpValue;
#endif	// #if defined( PRE_USA_FATIGUE )

	CDnItem *m_pEquip[EQUIPMAX];
	CDnItem *m_pCashEquip[CASHEQUIPMAX];
	CDnItem *m_pGlyph[GLYPHMAX];
#if defined(PRE_ADD_TALISMAN_SYSTEM)
	CDnItem *m_pTalisman[TALISMAN_MAX];
#endif

#ifdef PRE_ADD_NEW_MONEY_SEED
	INT64 m_nSeed;
#endif // PRE_ADD_NEW_MONEY_SEED

	bool m_bActiveInventory[3];
	int  m_nRequestSrcItemSlot;
	int  m_nRequestDestItemSlot;
	bool m_bRequestWait;
	bool m_bRequestQuickSlot;
	bool m_bRequestRepair;

	CDnCharInventory m_CharInventory;			// Note : 유저 인벤토리
	CDnCharInventory m_QuestInventory;			// Note : 퀘스트 인벤토리
	CDnCashInventory m_CashInventory;			// Note : 캐쉬 인벤토리
	CDnStorageInventory m_StorageInventory;		// Note : 창고 인벤토리
	CDnGuildInventory m_GuildInventory;			// Note : 길드 인벤토리
#ifdef PRE_ADD_SERVER_WAREHOUSE
	CDnWorldServerStorageInventory m_WorldServerStorageInventory; // Note : 서버창고 일반아이템 인벤토리
	CDnWorldServerStorageInventory m_WorldServerStorageCashInventory; // Note : 서버창고 캐시아이템 인벤토리
#endif

#ifdef PRE_ADD_CASHREMOVE
	CDnCashRemoveInventory m_CashRemoveInventory;// Nore: 캐쉬삭제 인벤토리.	
#endif

	CDnPetInventory m_PetInventory;            // Note : 탈것 인벤토리 : 캐쉬로 사용합니다.
	CDnItem *m_pVehicleEquip;
	CDnItem *m_pVehicleEquipParts;
	int m_nPetInvenCounter;

	CDnItem *m_pPetEquip;
	CDnItem *m_pPetEquipParts1;
	CDnItem *m_pPetEquipParts2;

#ifdef PRE_ADD_CASHSHOP_REFUND_CL
	CDnRefundCashInventory m_RefundCashInventory;
#endif

	int m_nCashInvenCounter;
#ifdef PRE_ADD_SERVER_WAREHOUSE
	int m_nWorldServerStorageInvenCounter;
	int m_nWorldServerStorageCashInvenCounter;
#endif

	//CDnEmblemFactory* m_pEmblemFactory;
	CDnItemCompounder* m_pItemCompounder;

	// 문장보옥/아이템 합성과 같이 쓰이고 있음.
	struct EmblemCompoundResult
	{
		CDnItemCompounder::S_OUTPUT Output;
		vector<ItemCompoundInfo> vlItemlInfosToRemove;

		EmblemCompoundResult( void ) {};
	};
	deque<EmblemCompoundResult> m_dqWaitEmblemCompoundReq;

	// 분해패킷 대기시간
	//float m_fDelayDisjoint;

	//struct JewelUpgradeResult
	//{
	//	int iNewJewelItemID;
	//	int iCost;
	//	
	//	JewelUpgradeResult( void ) : iNewJewelItemID( 0 ), iCost( 0 ) {};
	//};
	//deque<JewelUpgradeResult> m_dqWaitJewelUpgradeReq;

	// 사운드 인덱스. 더 많아지면, 따로 관리해야할 거 같은데..
	int m_nRepairSound;
	int m_nMoneySound;	// 창고에 돈 맡기고 꺼내는 소리
	int m_nItemSortSound;

#ifdef	PRE_ADD_NAMEDITEM_SYSTEM
	std::vector<SCEffectSkillItem> m_vecBackUp_EffectSkillInfo;
#endif

#ifdef PRE_MONITOR_SUPER_NOTE
	CDnIllegalActivityMonitorAgentMgr m_IAMAMgr;
#endif

	// 일본 가챠폰 요청 패킷 보냄.
#ifdef PRE_ADD_GACHA_JAPAN
	bool m_bSendGachaponReq;
	SCRefreshCashInven m_GachaResultCashItemPacket;
#endif // PRE_ADD_GACHA_JAPAN
	CDnCostumeMixDataMgr m_CosMixDataMgr;
	bool m_bLockCostumeSendPacket;
	bool m_bLockCosDesignMixSendPacket;
#ifdef PRE_ADD_COSRANDMIX
	bool m_bLockCosRandomMixSendPacket;
#endif
#ifdef PRE_ADD_COSRANDMIX
	CDnCostumeRandomMixDataMgr m_CosRandMixDataMgr;
#endif

#ifdef PRE_ADD_CHANGEJOB_CASHITEM
	bool m_bSendChangeJobReq;
#endif // #ifdef PRE_ADD_CHANGEJOB_CASHITEM

#if defined(PRE_ADD_VIP_FARM) || defined(PRE_ADD_FARM_DOWNSCALE)
	__time64_t m_tFarmVIPTime;
#endif	//#if defined(PRE_ADD_VIP_FARM) || defined(PRE_ADD_FARM_DOWNSCALE)

	SCashGlyph m_CashGlyph[CASHGLYPHSLOTMAX];
	int m_nCashGlyphCount;

	CDnItem* m_pSourceItem;

#if defined(PRE_ADD_TALISMAN_SYSTEM)
	SCashTalisman m_CashTalisman;
#endif

private:
#ifdef PRE_MOD_NESTREBIRTH
	bool m_bOnChangeRebirthCoinOrItem;
#endif

#ifdef PRE_ADD_BESTFRIEND
	BestFriend::SCGetInfo * m_pBestfriendInfo;
#endif


#if defined( PRE_ADD_LIMITED_SHOP )
#if defined( PRE_FIX_74404 )
	std::list<LimitedShop::LimitedItemData> m_listLimitedShopItem;
#else // #if defined( PRE_FIX_74404 )
	std::map<int, LimitedShop::LimitedItemData> m_mapLimitedShopItem;
#endif // #if defined( PRE_FIX_74404 )
#endif // #if defined( PRE_ADD_LIMITED_SHOP )

#ifdef PRE_ADD_EQUIPLOCK
	CDnItemLockMgr m_ItemLockMgr;
#endif

protected:
	void OnRecvItemMessage( int nSubCmd, char *pData, int nSize );
	void OnRecvItemGoodsMessage( int nSubCmd, char *pData, int nSize );
	void OnRecvCharMessage( int nSubCmd, char *pData, int nSize );
	void OnRecvEtcMessage( int nSubCmd, char *pData, int nSize );

	static bool __stdcall OnLoadRecvChangeParts( void *pThis, void *pParam, int nSize, LOCAL_TIME LocalTime );

	//blondy
	bool IsPVP( );
	//blondy end
public:
	bool Initialize();
	void Finalize();

	virtual void Process( LOCAL_TIME LocalTime, float fDelta );

	void InsertChangePartsThread( DnActorHandle hActor );

	CDnItem *CreateItem( TItemInfo &Info );
	CDnItem *CreateItem( TItem &Info );
	CDnItem *CreateItem( TVehicleItem &Info );

	bool RequestMoveItem( char cMoveType, int nSrcIndex, INT64 biSrcSerial, int nDestIndex, int nCount );
	void RequestRemoveItem( char cMoveType, int nSrcIndex, int nCount, INT64 biSerial );

#ifdef PRE_ADD_CASHREMOVE
	void RequestRemoveCashItem( INT64 biSerial, bool bRecovery );	
	__time64_t GetTimeBySN( INT64 nSerial );
#endif

	bool IsEquipItem( const int nSlotIndex, const CDnItem *pItem, std::wstring &strErrorMsg );
	bool IsRequestWait() { return m_bRequestWait; }
	void SetRequestWait(bool bSet){ m_bRequestWait = bSet;}
	bool IsRequestRepair() { return m_bRequestRepair; }

	bool CheckDie( bool bDeleteItem );

	void SetCoin( INT64 nValue );
	void AddCoin( INT64 nValue );
	INT64 GetCoin() { return m_nCoin; }
	void SetCoinStorage( INT64 nValue );
	INT64 GetCoinStorage() { return m_nCoinStorage; }
	void SetGuildWareCoin( INT64 nValue );
	INT64 GetGuildWareCoin() { return m_nCoinGuildWare; }
	int GetTakeGuildWareItemCount() { return m_nTakeGuildWareItemCount; }
	INT64 GetWithdrawGuildWareCoin() { return m_biWithdrawGuildWareCoin; }
	void SetGuildWareReceivedState( bool bReceived ) { m_bReceivedGuildWare = bReceived; }
	void SetFatigue( int nFatigue, int nWeekFatigue, int nPCBangFatigue, int nEventFatigue );
	void GetFatigue( int &nFatigue, int &nWeekFatigue, int &nPCBangFatigue, int &nEventFatigue );
	void SetMaxFatigue( int nFatigue, int nWeekFatigue, int nPCBangFatigue, int nEventFatigue );
	void GetMaxFatigue( int &nFatigue, int &nWeekFatigue, int &nPCBangFatigue, int &nEventFatigue );
#ifdef PRE_ADD_NEW_MONEY_SEED
	void SetSeed( INT64 nValue ) { m_nSeed = nValue; }
	void AddSeed( INT64 nValue ) { m_nCoin += nValue; }
	INT64 GetSeed() { return m_nSeed; }
#endif // PRE_ADD_NEW_MONEY_SEED

	enum eFatigueType
	{
		eDAILYFTG,
		eWEEKLYFTG,
		ePCBANGFTG,
		eEVENTFTG,
		eVIPFTG,
		eFTGMAX
	};
	void SetFatigue(eFatigueType type, int value);
	void SetMaxFatigue(eFatigueType type, int value);
	int	 GetFatigue(eFatigueType type) const;
	int	 GetMaxFatigue(eFatigueType type) const;
	void OnChangedFatigue( int nFatigue, int nWeekFatigue, int nPCBangFatigue, int nEventFatigue, int nVIPFatigue );

#if defined( PRE_USA_FATIGUE )
	int GetNoFatigueExpValue() { return m_nNoFatigueExpValue; }
	int GetFatigueExpValue() { return m_nFatigueExpValue; }
#endif	// #if defined( PRE_USA_FATIGUE )

	enum eRebirthCoinType
	{
		eDAILYCOIN,
		ePCBANGCOIN,
		eVIPCOIN,
		eFREECOIN_MAX,

		eCASHCOIN = eFREECOIN_MAX,
		eCOINMAX
	};
	void SetRebirthCoin(eRebirthCoinType type, int nCoinCount);
	int  GetRebirthCoin(eRebirthCoinType type) const;
	const WCHAR* GetRebirthCoinString(eRebirthCoinType type) const;
	void GetRebirthCoin( int &nRebirthCoinCount, int &nRebirthCashCoinCount, int &nRebirthPCBangCoinCount );

#ifdef PRE_MONITOR_SUPER_NOTE
	void AddIllegalActivityMonitor(eIAMA_CheckType type, float fTerm);
#endif
	void SetUsableRebirthCoin( int nUsableRebirthCoin ) { m_nUsableRebirthCoinCount = nUsableRebirthCoin; }
	int GetUsableRebirthCoin() { return m_nUsableRebirthCoinCount; }
	void SetMaxUsableRebirthCoin( int nMaxUsableRebirthCoin ) 
	{ 
		m_nMaxUsableRebirthCoincount = nMaxUsableRebirthCoin;
		m_nUsableRebirthCoinCount = nMaxUsableRebirthCoin;
	}
	int GetMaxUsableRebirthCoin() { return m_nMaxUsableRebirthCoincount; }
	bool IsCanRebirth();
#ifdef PRE_ADD_INSTANT_CASH_BUY
	bool IsCanRebirthIfHaveCoin();
#endif // PRE_ADD_INSTANT_CASH_BUY
	int GetRebirthCoinCount();
	//blondy
	bool IsCanPVPRebirth();

	void ResetSpecialRebirthItemID()	{ m_vSpecailRebirthItemIDs.clear(); m_nSpecialRebirthItemCount = 0; }
#ifdef PRE_MOD_NESTREBIRTH
	void SetSpecialRebirthItemID( int nSpecailRebirthItemID );
	void SetSpecialRebirthItemCount( int nSpecialRebirthItemCount );
#else
	void SetSpecialRebirthItemID( int nSpecailRebirthItemID ) { m_vSpecailRebirthItemIDs.push_back( nSpecailRebirthItemID ); }
	void SetSpecialRebirthItemCount( int nSpecialRebirthItemCount ) { m_nSpecialRebirthItemCount = nSpecialRebirthItemCount; }
#endif
	int  GetSpecialRebirthItemCount() const { return m_nSpecialRebirthItemCount; }

#ifdef PRE_MOD_NESTREBIRTH
	void SetUpdateRebirthCoinOrItem(bool bChange) { m_bOnChangeRebirthCoinOrItem = bChange; }
	bool IsOnChangeRebirthCoinOrItem() const { return m_bOnChangeRebirthCoinOrItem; }
#endif

	bool CDnItemTask::IsDisableItemInCharInven( CDnItem * pItem );
	//blondyend

	CDnCharInventory	&GetCharInventory()		{ return m_CharInventory; }
	CDnCharInventory	&GetQuestInventory()	{ return m_QuestInventory; }
	CDnCashInventory	&GetCashInventory()		{ return m_CashInventory; }
	CDnStorageInventory &GetStorageInventory()	{ return m_StorageInventory; }
	CDnGuildInventory	&GetGuildInventory()	{ return m_GuildInventory; }

	CDnPetInventory    &GetPetInventory()      { return m_PetInventory; }

#ifdef PRE_ADD_CASHREMOVE
	CDnCashRemoveInventory &GetCashRemoveInventory() {	return m_CashRemoveInventory; }
#endif

#ifdef PRE_ADD_SERVER_WAREHOUSE
	CDnWorldServerStorageInventory& GetWorldServerStorageInventory() { return m_WorldServerStorageInventory; }
	CDnWorldServerStorageInventory& GetWorldServerStorageCashInventory() { return m_WorldServerStorageCashInventory; }
	StorageUIDef::eError IsEnableStoreOnWorldServerStorage(CDnItem* pItem);
#endif

	bool RemovePetEquipItem( int nEquipIndex );
	void RemoveAllPetEquipItem();

#ifdef PRE_ADD_CASHSHOP_REFUND_CL
	CDnRefundCashInventory& GetRefundCashInventory() { return m_RefundCashInventory; }
	const CDnRefundCashInventory& GetRefundCashInventory() const { return m_RefundCashInventory; }
#endif

	// Note : 장비
	//
	bool InsertEquipItem( CDnItem *pItem );
	bool RemoveEquipItem( int nEquipIndex );
	void RemoveAllEquipItem();
	CDnItem *GetEquipItem( int nEquipIndex ) { return m_pEquip[nEquipIndex]; }
	void RefreshEquip();

	bool InsertCashEquipItem( CDnItem *pItem );
	bool RemoveCashEquipItem( int nEquipIndex );
	void RemoveAllCashEquipItem();
	CDnItem *GetCashEquipItem( int nEquipIndex ) { return m_pCashEquip[nEquipIndex]; }
	void RefreshCashEquip();

	// 강화
#if defined(PRE_ADD_ENCHANTSHIELD_CASHITEM)
#ifdef PRE_ADD_EQUIPED_ITEM_ENCHANT
	void RequestEnchant( CDnItem *pItem, char cGuardType = 0, bool bEquip = false );
#else 
	void RequestEnchant( CDnItem *pItem, char cGuardType = 0 ); // eEnchantGuardType
#endif // PRE_ADD_EQUIPED_ITEM_ENCHANT
#else
#ifdef PRE_ADD_EQUIPED_ITEM_ENCHANT
	void RequestEnchant( CDnItem *pItem, bool bGuard = false, bool bEquip = false );
#else 
	void RequestEnchant( CDnItem *pItem, bool bGuard = false );
#endif // PRE_ADD_EQUIPED_ITEM_ENCHANT
#endif
#ifdef PRE_ADD_EXCHANGE_ENCHANT
	void RequestExchangeEnchant(const CDnItem* pSourceItem, const CDnItem* pTargetItem);
#endif

	// 문장
	bool InsertGlyphItem( CDnItem *pItem );
	bool RemoveGlyphItem( int nEquipIndex );
	void RemoveAllGlyphItem();
	CDnItem *GetGlyphItem( int nEquipIndex ) { return m_pGlyph[nEquipIndex]; }
	void RefreshGlyph();
	int GetCashGlyphCount() { return m_nCashGlyphCount; }
	bool IsCashGlyphOpen( int nIndex )	{ return m_CashGlyph[nIndex].bOpen; }
	int  GetEquipCashGlyphIndex( CDnItem * pItem );
	__time64_t GetCashGlyphPeriod( int nCount )	{ return m_CashGlyph[nCount].tTime; }

#if defined(PRE_ADD_TALISMAN_SYSTEM)
	// 탈리스만
	bool	 InsertTalismanItem( CDnItem* pItem );
	bool	 RemoveTalismanItem( int nEquipIndex );
	void	 RemoveAllTalismanItem();

	CDnItem* GetTalismanItem(int nEquipIndex) { return m_pTalisman[nEquipIndex]; }	
	float	 GetTalismanSlotRatio(int nSlotIndex);

	SCashTalisman& GetCashTalismanSlot() { return m_CashTalisman; }
	bool	 IsOpenCashTalismanSlot() { return m_CashTalisman.bOpenSlot; }
	bool	 IsCompareCashTalismanSlotID(CDnItem* pItem);
#endif

	// QuickSlot 관련
	//
	void RequestAddQuickSlot( int nSlotID, MIInventoryItem::InvenItemTypeEnum Type, int nID );
	void RequestDelQuickSlot( int nSlotID );

	// LifeSkill QuickSlot 관련
	void RequestAddLifeSkillQuickSlot( int nSlotID, MIInventoryItem::InvenItemTypeEnum Type, INT64 biID );
	void RequestDelLifeSkillQuickSlot( int nSlotID );

	// Note: 문장 보옥
	//
	bool RequestEmblemCompoundOpen( CDnItem* pPlateItem, int iCompoundTableID, vector<ItemCompoundInfo>& vlEmblemCompoundInfo );
	bool RequestItemCompoundOpen( int iCompoundTableID, vector<ItemCompoundInfo>& vlItemCompoundInfo, INT64 biNeedItemSerialID = 0, INT64 iDiscountedCost = 0 );
	bool RequestEmblemCompound( CDnItem* pPlateItem, int iCompoundTableID, vector<ItemCompoundInfo>& vlEmblemCompoundInfo );
	bool RequestItemCompound( int iCompoundTableID, vector<ItemCompoundInfo>& vlItemCompoundInfo, INT64 biNeedItemSerialID = 0, INT64 iDiscountedCost = 0 );
	void RequestEmblemCompoundCancel( void );
	void RequestItemCompoundCancel( void );
	//

	// 랜덤아이템 취소
	void RequestRandomItemCancel();

	const CDnItemCompounder::S_PLATE_INFO* GetPlateInfoByItemID( int iItemID );
	//const CDnEmblemFactory::S_JEWEL_INFO* GetJewelInfoByItemID( int iItemID );

	void GetCompoundInfo( int iCompoundTableID, CDnItemCompounder::S_COMPOUND_INFO_EXTERN* pInfo );

	// 아이템 분해 관련
	void RequestItemDisjoint( CDnItem* pItem, UINT nNpcUniqueID );
	void RequestItemDisjointNew(CSItemDisjointReqNew Req);
	//void RequestItemDisjoint( float fDelayTime, int nCount, CDnItem *pItems );
	//void ProcessDisjointReqeust();

	// 수리
	int CalcRepairEquipPrice();
	int CalcRepairCharInvenPrice();
	void RequestRepairEquip();
	void RequestRepairAll();
	EquipDurabilityStateEnum GetEquipDurabilityState();

	// 돈 입금, 출금
	void RequestInvenToWare( INT64 nMoney );	// 인벤에서 창고로,
	void RequestWareToInven( INT64 nMoney );	// 창고에서 인벤으로,
	void RequestInvenToGuildWare( INT64 nMoney );
	void RequestGuildWareToInven( INT64 nMoney );
	void OnAddCoin(CDnPlayerActor* pWinner);

	// 정렬
	void RequestSortInven();
	void SortCashInven();
	void RequestSortStorage();

#ifdef PRE_ADD_CASHREMOVE	
	void SortCashRemoveInven(); // #52830
#endif

	// 탈것 
	bool InsertVehicleItem( CDnItem *pItem );
	bool RemoveVehicleItem( int nEquipIndex );
	void RemoveAllVehicleItem();
	CDnItem *GetVehicleItem() { return m_pVehicleEquip; }
	CDnItem *GetVehiclePartsItem() { return m_pVehicleEquipParts; }
	void SortVehicleInven();

	bool InsertPetItem( CDnItem *pItem );
	bool RemovePetItem( int nEquipIndex );
	void RemoveAllPetItem();
	CDnItem* GetPetItem() { return m_pPetEquip; }
	CDnItem* GetPetParts1Item() { return m_pPetEquipParts1; }
	CDnItem* GetPetParts2Item() { return m_pPetEquipParts2; }

#ifdef PRE_ADD_GACHA_JAPAN
	// 일본 가챠폰
	void RequestGacha_JP( int iJobClassID, int iSelectePart );
	void ResetSendGachaReq( void ) { m_bSendGachaponReq = false; };
#endif

#ifdef PRE_ADD_COSRANDMIX
	bool ReadyItemCostumeMixOpen(eItemTypeEnum type);
#else
	bool ReadyItemCostumeMixOpen(bool bDesignMix);
#endif // PRE_ADD_COSRANDMIX
	bool RequestItemCostumeMixOpen();
	bool RequestItemCostumeMixClose();
	bool RequestItemCostumeMixCloseComplete();
	bool RequestItemCostumeMix(INT64* pSerials, int count, UINT resultItemId, char ability);

	bool IsLockCosMixSendPacket() const { return m_bLockCostumeSendPacket; }
	bool IsLockCosDesignMixSendPacket() const { return m_bLockCosDesignMixSendPacket; }

	bool RequestItemCostumeDesignMixClose();
	bool RequestItemCostumeDesignMix(INT64* pSerials);
	bool RequestItemCostumeDesignMixCloseComplete();
#ifdef PRE_ADD_COSRANDMIX
	bool IsLockCosRandMixSendPacket() const { return m_bLockCosRandomMixSendPacket; }
	bool RequestItemCostumeRandomMixCloseComplete();
	bool RequestItemCostumeRandomMixClose();
	bool RequestItemCostumeRandomMix(INT64* pSerials);
#endif

#if defined(PRE_PERIOD_INVENTORY)
	void RequestSortPeriodInven();
	void RequestSortPeriodStorage();
#endif	// #if defined(PRE_PERIOD_INVENTORY)

	const CDnCostumeMixDataMgr& GetCostumeMixDataMgr() const	{ return m_CosMixDataMgr; }
	CDnCostumeMixDataMgr& GetCostumeMixDataMgr()				{ return m_CosMixDataMgr; }

#ifdef PRE_ADD_COSRANDMIX
	const CDnCostumeRandomMixDataMgr& GetCostumeRandomMixDataMgr() const	{ return m_CosRandMixDataMgr; }
	CDnCostumeRandomMixDataMgr& GetCostumeRandomMixDataMgr()				{ return m_CosRandMixDataMgr; }
#endif

#ifdef PRE_ADD_CHANGEJOB_CASHITEM
	bool RequestUseChangeJobCashItem( INT64 biItemSerial, int iJobID );
#endif // #ifdef PRE_ADD_CHANGEJOB_CASHITEM

#if defined(PRE_ADD_VIP_FARM) || defined(PRE_ADD_FARM_DOWNSCALE)
	void SetVipFarmTime( __time64_t tExpireDate )	{ m_tFarmVIPTime = tExpireDate; }
	__time64_t GetVipFarmTime()		{ return m_tFarmVIPTime; }
#endif	//#if defined(PRE_ADD_VIP_FARM) || defined(PRE_ADD_FARM_DOWNSCALE)

#ifdef PRE_FIX_MAKECHARMITEM
	void MakeCharmItemList( std::vector<boost::tuple<int, int, int>> & vCharmItemList, const short sInvenIndex, const BYTE cInvenType, const INT64& nCharmItemSerial, CDnItem * pCharmItem = NULL );
#else
	void MakeCharmItemList( std::vector<boost::tuple<int, int, int>> & vCharmItemList, const short sInvenIndex, const BYTE cInvenType, CDnItem * pCharmItem = NULL );
#endif
public:
	virtual void OnDisconnectTcp( bool bValidDisconnect );
	virtual void OnDisconnectUdp();

	// ClientSession
	virtual void OnDispatchMessage( int nMainCmd, int nSubCmd, char *pData, int nSize );

public:
	virtual void OnRecvItemInventory( SCInvenList *pPacket );
	virtual void OnRecvItemCashInventory( SCCashInvenList *pPacket );
	virtual void OnRecvItemMove( SCMoveItem *pPacket );
	virtual void OnRecvItemCashMove( SCMoveCashItem *pPacket );
	virtual void OnRecvItemUserChangeColor( SCChangeColor *pPacket );
	virtual void OnRecvItemUserChangeEquip( SCChangeEquip *pPacket );
	virtual void OnRecvItemUserChangeCashEquip( SCChangeCashEquip *pPacket );
	virtual void OnRecvItemPickup( SCPickUp *pPacket );
	virtual void OnRecvItemRemove( SCRemoveItem *pPacket );
#ifdef PRE_ADD_CASHREMOVE
	virtual void OnRecvItemRemoveCash( SCRemoveCash * pPacket );
#endif
	virtual void OnRecvItemRefreshInven( SCRefreshInven *pPacket );
	virtual void OnRecvItemRefreshCashInven( SCRefreshCashInven *pPacket );
	virtual void OnRecvItemStorage( SCWarehouseList *pPacket );
#ifdef PRE_ADD_SERVER_WAREHOUSE
	virtual void OnRecvItemMoveWorldServerStorage(SCMoveServerWare* pPacket);
	virtual void OnRecvItemMoveWorldServerCashStorage(SCMoveCashServerWare* pPacket);
#endif
	virtual void OnRecvItemRefreshEquip( SCRefreshEquip *pPacket );
	virtual void OnRecvItemRefreshWarehouse( SCRefreshWarehouse *pPacket );
	virtual void OnRecvItemRepairEquip( SCRepairResult *pPacket );
	virtual void OnRecvItemRepairAll( SCRepairResult *pPacket );
	virtual void OnRecvItemMoveCoin( SCMoveCoin *pPacket );
	virtual void OnRecvItemOpenGuildWare( SCOpenGuildWare *pPacket );
	virtual void OnRecvItemMoveGuildItem( SCMoveGuildItem *pPacket );
	virtual void OnRecvItemMoveGuildCoin( SCMoveGuildCoin *pPacket );
	virtual void OnRecvItemRefreshGuildItem( SCRefreshGuildItem *pPacket );
	virtual void OnRecvItemRefreshGuildCoin( SCRefreshGuildCoin *pPacket );	
	virtual void OnRecvItemRequestRandomItem( SCRequestRandomItem *pPacket );
	virtual void OnRecvItemCompleteRandomItem( SCCompleteRandomItem *pPacket );
	virtual void OnRecvItemRequestCharmItem( SCCharmItemRequest *pPacket );
	virtual void OnRecvItemCompleteCharmItem( SCCharmItemComplete *pPacket );
	virtual void OnRecvItemPetalTokenResult( SCPetalTokenResult *pPacket );
	virtual void OnRecvItemAppellationGainResult( SCAppellationGainResult *pPacket );

	virtual void OnRecvItemDropItemMsg( SCDropItem *pPacket );
	virtual void OnRecvItemCreateDropItem( SCCreateDropItem *pPacket );
	virtual void OnRecvItemCreateDropItemList( SCCreateDropItemList *pPacket );
	virtual void OnRecvItemUse( SCUseItem* pPacket );
	virtual void OnRecvItemGuildRename( SCGuildRename *pPacket );
	virtual void OnRecvItemCharacterRename( SCCharacterRename *pPacket );
	
	virtual void OnRecvEmblemCompoundOpenRes( SCEmblemCompundOpenRes* pPacket );
	virtual void OnRecvItemCompoundOpenRes( SCItemCompoundOpenRes* pPacket );
	virtual void OnRecvEmblemCompoundRes( SCCompoundEmblemRes* pPacket );
	virtual void OnRecvItemCompoundRes( SCCompoundItemRes* pPacket );
	virtual void OnRecvEmblemCompoundCancelRes( SCEmblemCompoundCancelRes* pPacket );
	virtual void OnRecvItemCompoundCancelRes( SCItemCompoundCancelRes* pPacket );
	
	virtual void OnRecvItemDisjointRes(SCItemDisjointRes* pPacket);
	virtual void OnRecvItemDisjointResNew(SCItemDisjointResNew* pPacket);
	virtual void OnRecvItemEnchant( SCEnchantItem* pPacket );
	virtual void OnRecvItemEnchantComplete( SCEnchantComplete* pPacket );
	virtual void OnRecvItemEnchantCancel( SCEnchantCancel* pPacket );
	//virtual void OnRecvUpgradeJewelRes( SCUpgradeJewelRes* pPacket );
	virtual void OnRecvInventoryMaxCount( SCInventoryMaxCount* pPacket );
	virtual void OnRecvWarehouseMaxCount( SCWarehouseMaxCount* pPacket );
	virtual void OnRecvRebirthCoin( SCRebirthCoin* pPacket );

	virtual void OnRecvSortInventory( SCSortInventory *pPacket );
	virtual void OnRecvDecreaseDurabilityInventory( SCDecreaseDurabilityInventory *pPacket );

	virtual void OnRecvSortWarehouse( SCSortWarehouse * pPacket );

	virtual void OnRecvCharQuickSlotList( SCQuickSlotList *pPacket );
	virtual void OnRecvCharAddQuickSlot(SCAddQuickSlot * pData);
	virtual void OnRecvCharDelQuickSlot(SCDelQuickSlot * pData);
	virtual void OnRecvCharFatigue( SCFatigue *pData );

	virtual void OnRecvItemEquipList( SCEquipList *pPacket );
	virtual void OnRecvItemUserChangeGlyph(SCChangeGlyph *pPacket);

#if defined( PRE_USA_FATIGUE )
	virtual void OnRecvEtcChannelInfoFatigueInfo( SCChannelInfoFatigueInfo *pPacket );
#endif	// #if defined( PRE_USA_FATIGUE )

	void DetachAllParts( MAPartsBody* pActor );
	void OnRecvQuestItemInventory(SCQuestInvenList* pPacket);
	void OnRecvQuestItemRefreshInven(SCRefreshQuestInven* pPacket);

	// 장비 아이템 슬롯 쿨타임
	void OnRecvGlyphSlotCoolTime( SCGlyphTimeInfo* pPacket );

	void OnRecvUnsealItem(SCUnSealItem* pData);
	void OnRecvSealItem(SCSealItem* pData);
	void OnRecvBroadcastingEffect( SCBroadcastingEffect *pPacket );

	void OnRecvItemRequestCashWorldMsg( SCRequestCashWorldMsg *pPacket );

#ifdef PRE_ADD_GACHA_JAPAN
	// 일본 가챠폰
	void OnRecvItemGachaOpen_JP( SCGachaShopOpen_JP* pPacket );
	void OnRecvItemGachaRes_JP( SCGachaRes_JP* pPacket );
	void OnStopGachaRoulette( void );
#endif

	void OnRecvItemCosMixOpen(SCCosMixOpen* pPacket);
	void OnRecvItemCosMixClose(SCCosMixClose* pPacket);
	void OnRecvItemCosMixCloseComplete(SCCosMixClose* pPacket);
	void OnRecvItemCosMixComplete(SCCosMix* pPacket);

	void OnRecvItemCosDesignMixOpen(SCCosDesignMixOpen* pData);
	void OnRecvItemCosDesignMixClose(SCCosDesignMixClose* pPacket);
	void OnRecvItemCosDesignMixCloseComplete(SCCosDesignMixClose* pPacket);
	void OnRecvItemCosDesignMixComplete(SCCosDesignMix* pPacket);

#ifdef PRE_ADD_COSRANDMIX
	void OnRecvItemCosRandomMixOpen(SCCosRandomMixOpen* pData);
	void OnRecvItemCosRandomMixClose(SCCosRandomMixClose* pData);
	void OnRecvItemCosRandomMixCloseComplete(SCCosRandomMixClose* pData);
	void OnRecvItemCosRandomMixComplete(SCCosRandomMix* pPacket);
#endif

	void OnRecvItemPotentialItem( SCPotentialItem *pPacket );
#ifdef PRE_MOD_POTENTIAL_JEWEL_RENEWAL	
	void OnRecvItemPotentialItemRollBack( SCPotentialItem *pPacket );
#endif
	void OnRecvItemEnchantJewelItem( SCEnchantJewelItem *pPacket );
	void OnRecvItemGuildMark( SCGuildMark *pPacket );

	void OnRecvVehicleEquipListMessage(SCVehicleEquipList *pPacket);
	void OnRecvVehicleInvenListMessage(SCVehicleInvenList *pPacket);
	void OnRecvChangeVehiclePartsMessage(SCChangeVehicleParts *pPacket);
	void OnRecvChangeVehicleColorMessagse(SCChangeVehicleColor *pPacket);
	void OnRecvRefreshVehicleInvenMessage(SCRefreshVehicleInven *pPacket);

	void OnRecvChangePetPartsMessage( SCChangeVehicleParts* pPacket );
	void OnRecvChangePetBodyMessage( SCChangePetBody* pPacket );
	void OnRecvChangePetColorMessagse( SCChangeVehicleColor* pPacket );
	void OnRecvPetEquipList( SCVehicleEquipList* pPacket );
	void OnRecvAddPetExp( SCAddPetExp *pPacket );
#ifdef PRE_ADD_CHANGEJOB_CASHITEM
	void OnRecvChangeJobCashItem( SCUseChangeJobCashItemRes* pPacket );
#endif // #ifdef PRE_ADD_CHANGEJOB_CASHITEM

#if defined(PRE_ADD_VIP_FARM)
	void OnRecvEffectItemInfo( SCEffectItemInfo * pPacket );
#endif	//#if defined(PRE_ADD_VIP_FARM)

	void OnRecvGlyphExtendCount( SCGlyphExtendCount* pPacket );
	void OnRecvGlyphExpireData( SCGlyphExpireData * pPacket );

	void OnRecvIncreaseLife( SCIncreaseLife* pPacket );

	void OnRecvWarpVillageList( SCWarpVillageList* pPacket );
	void OnRecvWarpVillageRes( SCWarpVillage *pPacket );

#ifdef PRE_ADD_EXCHANGE_POTENTIAL
	void OnRecvExchangePotential( SCExchangePotential *pPacket );
#endif
#ifdef PRE_ADD_EXCHANGE_ENCHANT
	void OnRecvExchangeEnchant(SCExchangeEnchant* pPakcet);
#endif
#if defined(PRE_ADD_PCBANG_RENTAL_ITEM)
	void OnRecvPcCafeRentItem( SCPcCafeRentItem* pPacket);
#endif
#if defined(PRE_ADD_TALISMAN_SYSTEM)
	void OnRecvTalismanItem( SCChangeTalisman* pPacket);
	void OnRecvTalismanExpireData( SCTalismanExpireData* pPacket );
	void OnRecvTalismanOpenSlot( SCOpenTalismanSlot* pPacket );
#endif

#ifdef PRE_ADD_EQUIPLOCK
	void OnRecvEquipItemLockList(SCEquipItemLockList* pData);
	void OnRecvEquipItemLock(SCItemLockRes* pData);
	void OnRecvEquipItemUnLock(SCItemLockRes* pData);
#endif

#if defined(PRE_PERIOD_INVENTORY)
	void OnRecvPeriodInventory( SCPeriodInventory * pData );
	void OnRecvPeriodWarehouse( SCPeriodWarehouse * pData );
	void OnRecvSortPeriodInventory( SCSortInventory * pData );
	void OnRecvSortPeriodWarehouse( SCSortWarehouse * pData );
#endif	// #if defined(PRE_PERIOD_INVENTORY)

#ifdef PRE_ADD_STAGE_USECOUNT_ITEM
	void OnRecvStageUseLimitItem( SCStageUseLimitItem* pData );
#endif // PRE_ADD_STAGE_USECOUNT_ITEM

public:
	CDnItem *FindItem( int nItemTableID, ITEM_SLOT_TYPE slotType );
	CDnItem *FindItem( INT64 nSerialID );
	int FindItemFromItemType( eItemTypeEnum Type, ITEM_SLOT_TYPE slotType, std::vector<CDnItem *> &pVecResult );
	int FindItemCountFromItemType( eItemTypeEnum Type, int nTypeParam1 = -1 );
	int FindItemListFromItemTypeEx( eItemTypeEnum Type, std::vector<CDnItem *> &pVecResult, int nTypeParam1 = -1 );
	static void GetItemListForExtendUseItem( std::vector<CDnItem *> &pVecResult, std::vector<int> &VecItemID, std::vector<bool> &VecEternity );

	int GetAllowGameMode() const;
	void PlayMoneySound();

	enum eItemSoundType {
		ItemSound_Root,
		ItemSound_Drag,
		ItemSound_Use,
	};

	void PlayItemSound( int nItemID, eItemSoundType Type );	
	void ResetRequestWait();

	bool CheckRequestMoveItem( bool bDelete );
	bool CheckRequestEquipItem( char cMoveType );

protected:
	CDnItem* FindItemEquip( int nItemTableID );
	void MoveInventoryItem( CDnInventory &srcInven, CDnInventory &destInven, TItemInfo &SrcItem, TItemInfo &DestItem );
#ifdef PRE_ADD_SERVER_WAREHOUSE
	void MoveCashInventoryItem(CDnInventory& srcInven, CDnInventory& destInven, TItem& srcItem, const int nSrcSlotIndex, TItem& destItem, const int nDestSlotIndex);
#endif

	void PlayPickupSound( CDnItem *pItem );
	void PlayItemMoveSound( SCMoveItem *pPacket );
	void PlayEnchantResultSound(int nFileIndex);

	void OnFinishRecvCashInventory();

	virtual void OnRefreshCashInventory();

public:
	CDnItem* FindRebirthItem(int nAllowMapType);
#ifdef PRE_MOD_NESTREBIRTH
	int GetRebirthItemCount_MineOnly();
#endif

#if defined(PRE_ADD_REMOVE_PREFIX)
	void OnRecvItemRemovePrefix( SCEnchantJewelItem *pPacket );
#endif // PRE_ADD_REMOVE_PREFIX
	void OnRecvItemExpandSkillPage(SCExpandSkillPageItem *pPacket);
#ifndef PRE_ADD_NAMEDITEM_SYSTEM		// 네임드 아이템 디파인이 켜지면 마을 상태효과 패킷이 통합되었으므로 이 패킷이 오지 않는다. 따라서 해당 디파인이 켜져 있으면 제거.
	void OnRecvItemUseSource( SCSourceItem* pPacket );
#endif // #ifndef PRE_ADD_NAMEDITEM_SYSTEM
	void RemoveUsedSourceItemReferenceObjectAndIcon( int iItemID );
	CDnItem* GetSourceItem( void ) { return m_pSourceItem; };
	int GetUsedSourceItemDurationTime( int nItemID );


#if defined(PRE_FIX_44486)
	bool CheckSkillLevelUpInfo(char cMoveType, int nSrcIndex, int nDestIndex);
	bool CheckSkillLevelUpInfo(CDnItem *pSrcItem, CDnItem* pDestItem);
#endif // PRE_FIX_44486

	void RequestWarpVillageList( INT64 iUsedItemSerial );
	void RequestUseWarpVillageCashItem( int iMapID, INT64 iUsedItemSerial );

#if defined(PRE_FIX_45899)
	void GetItemByMoveType(char cMoveType, int nSrcIndex, int nDestIndex, CDnItem** pDestItem, CDnItem** pSrcItem);
	void DisalbeSkillByItemMove(CDnItem *pSrcItem, CDnItem *pDestItem);
#endif // PRE_FIX_45899

#ifdef PRE_ADD_AUTOUNPACK
	// Packet 전송.
	void RequestAutoUnpackItem( INT64 biInvenSerial, bool bShow );

	// Packet 수신.
	void OnRecvAutoUnpack( SCAutoUnpack * pData );
#endif

#if defined( PRE_ADD_NAMEDITEM_SYSTEM )
	void ApplyBackUpEffectSkillData( DnActorHandle hActor );
	bool ApplyEffectSkillData( SCEffectSkillItem *pData , DnActorHandle hActor );
	void OnRecvEffectSkillItem( SCEffectSkillItem *pPacket );
	void OnRecvRemoveEffectSkillItem( SCDelEffectSkillItem *pPacket );
	void OnRecvShowEffect( SCShowEffect *pPacket );	
	void OnRecvRefreshNamedItem( SCNamedItemID *pPacket );
	void AddSourceItem( DnActorHandle hActor, TEffectSkillData EffectSkill, bool bUsedInGameServer );
	void AddEffectItem( DnActorHandle hActor, TEffectSkillData EffectSkill, bool bUsedInGameServer );
	void RemoveEffectItem( DnActorHandle hActor, int nSkillItemIndex );
	void AddVillagePartyEffectItem( UINT nUniqueID , TEffectSkillData EffectSkill );
#endif

#if defined(PRE_ADD_EXPUP_ITEM)
	void OnRecvDailyItemError( SCDailyLimitItemError *pPacket);
#endif

#ifdef PRE_ADD_CHAOSCUBE
	void RequestCancelChaosCube();
	void RequestChaosCube( struct CSChaosCubeRequest & packet );
	void CompleteChaosCube( struct CSChaosCubeComplete & packet );
	void OnRecvChaosCubeRequest( SCChaosCubeRequest * pData );	
	void OnRecvChaosCubeComplete( SCChaosCubeComplete * pData );
#endif


#ifdef PRE_ADD_BESTFRIEND
	void OnRecvBestfriend( int nSubCmd, char *pData, int nSize ); // 절친패킷응답.

	void RequestSearchBF( std::wstring & strID ); // 절친확인요청.
	void OnRecvSearchBF( char * pData ); // 절친확인결과.

	void RequestRegistBF( std::wstring &strID ); // 절친등록요청.
	
	void OnRecvRegistREQ( char * pData ); // 절친등록요청확인.
	void OnRecvRegistACK( char * pData );

	void OnRecvComplerBF( char * pData );

	void RequestAcceptCancelBF( bool AcceptorCancel, UINT nFromAccountDBID, INT64 biFromCharacterDBID, WCHAR * wszFromName ); // 절친요청수락.
	void RequestRefusalBF(); // 절친요청거절.

	void OnRecvInfoBF( char * pData ); // 절친정보.

	void RequestRewardItemBF( INT64 giftSerial, int itemID );          // 보상아이템번호전송.	
	void OnRecvBestFriendItemReq( SCBestFriendItemRequest * packet );  // 보상아이템결과.	
	void SendBFItemComplet( INT64 giftSerial, int itemID );		   // 보상아이템완료.
	void OnRecvBestFriendItemComp( SCBestFriendItemComplete * packet );// 보상아이템완료결과.
	void RequestBFItemCancel(); // 보상아이템선택 취소.
		
	void RequestMemoBF( LPCWSTR strMemo ); // 절친메모전송.
	void OnRecvMemo( char * pData );// 메모수신.	

	void RequestBrokeupOrCancelBF( bool bCancel ); // TRUE:절친파기,  FALSE:절친파기취소.

	void OnRecvCancelBF( char * pData );// 절친 or 절친파기.
	void OnRecvCloseBF(); // 절친해제.

	void RequestBFInfo(); // 절친정보요청.

	void InitBestfriendInfo();
#endif


#if defined( PRE_ADD_LIMITED_SHOP )
	void OnRecvLimitedShopItemData( SCLimitedShopItemData *pPacket);
#if defined( PRE_FIX_74404 )
	int GetLimitedShopItemCount(int nShopID, int nItemID);
#else// #if defined( PRE_FIX_74404 )
	int GetLimitedShopItemCount(int nItemID);
#endif// #if defined( PRE_FIX_74404 )
	void AddLimitedShopItemCount(LimitedShop::LimitedItemData ItemData);
	void ClearLimitedShopItemCount();
#endif

#if defined( PRE_ADD_TRANSFORM_POTION)
	void OnRecvTransformItem( SCChangeTransform *pPacket );
#endif

#ifdef PRE_ADD_PARTSITEM_TOOLTIP_INFO
	bool IsEnableCompoundItem(int itemId) const;
#endif

#ifdef PRE_ADD_SERVER_WAREHOUSE
	void OnRecvItemWorldServerStorageList(SCServerWareList* pPacket);
	void OnRecvItemWorldServerCashStorageList(SCCashServerWareList* pPacket);
#endif


#ifdef PRE_ADD_GAMEQUIT_REWARD
	// Table로부터 Item 찾기.
	// < Table > LevelupEventTable => MailTable => CashCommodity => ItemTable => CharmItemTable
	// : nLevel 값이 0 보다 작은경우 level 무시,   nClass 가 -1 인경우 무시( 0 인경우는 모든직업 )
	void FindEventItems( int nEventType, int nLevel, int nClass, std::vector< CDnItem * > & vecItem );
#endif // PRE_ADD_GAMEQUIT_REWARD

#ifdef PRE_ADD_PVP_EXPUP_ITEM
	void OnRecvItemPvPExpUp( SCUsePvPExpup* pPacket );
#endif

#ifdef PRE_ADD_EQUIPLOCK
	const CDnItemLockMgr& GetItemLockMgr() const { return m_ItemLockMgr; }
#endif

#ifdef PRE_ADD_LEVELUP_REWARD_NOTIFIER
	bool IsLevelUpBoxItem( const CDnItem* pItem );
	CDnItem* GetExistLevelUpBox( const int& nLevel );
#endif
};

#define GetItemTask()	CDnItemTask::GetInstance()