#pragma once
#include "DnItem.h"
#include "DNPacket.h"
#include "DnStoreRepurchaseSystem.h"

class CDnStoreTabDlg;
class CDnItem;

class CDnTradeItem
{
public:
	CDnTradeItem(void);
	virtual ~CDnTradeItem(void);

	struct TShopPurchaseType
	{
		Shop::PurchaseType::eCode PurchaseType;
		int iPurchaseItemID;
		int iPurchaseItemValue;
	};

	struct TCombinedShopTableData
	{
		TShopPurchaseType PurchaseType[Shop::Max::PurchaseType];
		Shop::PurchaseLimitType::eCode PurchaseLimitType;
		int iPuschaseLimitValue;
#if defined( PRE_ADD_LIMITED_SHOP )
		int nBuyLimitCount;
		int nShopLimitReset;
#if defined( PRE_FIX_74404 )
		int nShopID;
#endif // #if defined( PRE_FIX_74404 )
#endif
	};

	typedef std::vector<TCombinedShopTableData>				COMBINED_SHOP_VEC;
	typedef std::vector<TCombinedShopTableData>::iterator	COMBINED_SHOP_VEC_ITER;
	struct SCombinedStoreTabData {
		COMBINED_SHOP_VEC vecCombineStoreItem;
	};
	typedef std::vector<SCombinedStoreTabData> COMBINED_STORE_DATA_VEC;
	COMBINED_STORE_DATA_VEC m_vecCombinedStoreData;

protected:
	bool m_bRequestWait;
	int m_nRequestSrcItemSlot;
	int m_nRequestDestItemSlot;

	// CDnItem안에 슬롯인덱스(특정 상점탭에서의 슬롯 번호)까지 들어있다.
	typedef std::vector<CDnItem*>		STORE_ITEM_VEC;
	typedef STORE_ITEM_VEC::iterator	STORE_ITEM_VEC_ITER;
	struct SStoreTabData {
		//int nTabID;	// 벡터인덱스 자체를 tabID로 쓸수있기때문에 뺀다.
		STORE_ITEM_VEC vecStoreItem;
	};
	typedef std::vector<SStoreTabData>	STORE_DATA_VEC;
	STORE_DATA_VEC m_vecStoreData;

	CDnStoreTabDlg *m_pStoreDialog;
	CDnItem *m_pTradeSellItem;
	CDnItem *m_pTradeBuyItem;

	// 판매 사운드
	int m_nSellSoundIndex;
	CDnStoreRepurchaseSystem m_StoreRepurchaseSystem;

	CDnItem *CreateItem( int nItemID, short wCount, BYTE cSlotIndex );	// 왜 ItemTask의 CreateItem을 같이 안쓸까...
	friend class CDnSkillStoreDlg;		// 왜 스킬상점에서 이 CreateItem을 사용하는거지? 예전부터 이렇게 되있었는데...

	void DeleteStoreData();
	void OpenShopSuccess();

	void CombinedShopOpen( int nShopID );

public:
	bool Initialize();
	void Finalize();

	void SetStoreDialog( CDnStoreTabDlg *pDialog);

	bool IsRequestWait() { return m_bRequestWait; }
	bool IsExpensiveItem( CDnItem *pItem );

	CDnItem* GetStoreItem( int nSlotIndex );
	void DeleteAllStore();

	CDnItem* GetStoreItem( int nTabID, int nSlotIndex );
	TCombinedShopTableData * GetCombinedStoreItem( int nTabID, int nSlotIndex );
	bool CheckCombinedShopBuy( int nTabID, BYTE cSlotIndex, int nCount );

	void SetTradeSellItem( CDnItem *pItem ) { m_pTradeSellItem = pItem; }
	CDnItem *GetTradeSellItem() { return m_pTradeSellItem; }

	void SetTradeBuyItem( CDnItem *pItem ) { m_pTradeBuyItem = pItem; }
	CDnItem *GetTradeBuyItem() { return m_pTradeBuyItem; }

public:
	void RequestShopBuy( int nTabID, BYTE cSlotIndex, int nCount );
	bool RequestShopSell(const CDnItem& sellItem, int count);
	bool IsShopSellableItem(const CDnItem& item, int count) const;

public:
	void OnRecvShopOpen( SCShopOpen *pPacket );
	void OnRecvShopBuy( SCShopBuyResult *pPacket );
	void OnRecvShopSell( SCShopSellResult *pPacket );
	void OnRecvShopGetRepurchaseList(SCShopRepurchaseList* pPacket);
	void OnRecvShopRepurchase(SCShopRepurchase* pPacket);

	void SetItemToDlgAndStoreDataVector(int nTabID, CDnItem* pItem);
	const CDnStoreRepurchaseSystem& GetStoreRepurchaseSystem() const { return m_StoreRepurchaseSystem; }
#ifdef PRE_FIX_LADDERSHOP_SKILL
	int CalcPurchaseLadderPoint(int nPoint) const;
#endif
};