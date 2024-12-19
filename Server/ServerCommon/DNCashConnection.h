#pragma once

#include "Connection.h"

class CDNCashConnection: public CConnection
{
public:
	bool m_bPetalSaleAbort;

	BYTE m_cSaleAbortCount;
	int m_nSaleAbortList[SALEABORTLISTMAX];

public:
	CDNCashConnection(void);
	~CDNCashConnection(void);

	void Reconnect();
	virtual int MessageProcess(int nMainCmd, int nSubCmd, char *pData, int nLen);

	void SendBalanceInquiry(CDNUserSession *pSession, bool bOpen, bool bServer);
	void SendDelUser(UINT nAccountDBID);
	void SendBuy(CDNUserSession *pSession, const CSCashShopBuy *pCashShop, TCashBuyItem *BuyList);
	void SendPackageBuy(CDNUserSession *pSession, const CSCashShopPackageBuy *pCashShop, TCashItemBase *BuyList);

#if defined(_VILLAGESERVER) && defined(PRE_ADD_CASH_REFUND)
	void SendMoveCashInven(UINT nAccountDBID, UINT64 biCharacterDBID, char cWorldSetID, int nMapID, int nChannelID, TCashItemBase* pCashItem, TPaymentItemInfoEx* pPaymentItemInfo );
	void SendPackageMoveCashInven(UINT nAccountDBID, UINT64 biCharacterDBID, char cWorldSetID, int nMapID, int nChannelID, TCashItemBase* pCashItemList, TPaymentPackageItemInfoEx* pPaymentPackageItemInfo );
	void SendCashRefund(UINT nAccountDBID, char cItemType, char cWorldSetID, int nItemSN, UINT uiOrderNo, INT64 biPurchaseOrderDetailID );
#endif
	void SendGift(CDNUserSession *pSession, const CSCashShopGift *pCashShop, TCashGiftItem *GiftList);
	void SendPackageGift(CDNUserSession *pSession, const CSCashShopPackageGift *pCashShop, TCashPackageGiftItem *GiftList);
	void SendCoupon(CDNUserSession *pSession, WCHAR *pCoupon);

	void SendVIPBuy(CDNUserSession *pSession, int nItemSN, bool bServer = false);
	void SendVIPGift(CDNUserSession *pSession, const CSVIPGift *pCashShop);

	void SendSaleAbortList();
#ifdef PRE_ADD_LIMITED_CASHITEM
	void SendGetLimitedItemList();
	void SendCheatChangeLimitMax(int nSN, int nLimitMax);
#endif		//#ifdef PRE_ADD_LIMITED_CASHITEM
};

extern CDNCashConnection *g_pCashConnection;

