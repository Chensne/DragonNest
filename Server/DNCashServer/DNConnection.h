#pragma once

#include "Connection.h"

class CDNSQLMembership;
class CDNConnection: public CConnection
{
public:
	CDNConnection(void);
	~CDNConnection(void);

	int MessageProcess(int iMainCmd, int iSubCmd, char *pData, int iLen);
	void DBMessageProcess(char *pData, int nThreadID);

	void OnRecvCashShop(int nThreadID, int nSubCmd, char *pData);

	void SendSaleAbortList(std::vector<int> &VecProhibitSaleList);
#ifdef PRE_ADD_LIMITED_CASHITEM
	void SendLimiteItemInfoList(std::vector<LimitedCashItem::TLimitedQuantityCashItem> &vList);
	void SendChangedLimitedItemInfoList(std::vector<LimitedCashItem::TChangedLimitedQuantity> &vList);
#endif		//#ifdef PRE_ADD_LIMITED_CASHITEM
	void SendBalanceInquiry(UINT nResult, UINT nAccountDBID, int nCashBalance, int nPetal, bool bOpen, bool bServer, int nNotRefundableBalance = 0);	

#if defined(PRE_ADD_SALE_COUPON)
	void SendBuy(UINT nAccountDBID, int nResult, char cCartType, int nPetalBalance, char cProductCount, TCashBuyItem *BuyList, TPaymentItemInfoEx* pPaymentItem=NULL, char cPaymentRules = 0, INT64 biSaleCouponSerial = 0, INT64 nSeedPoint=0);
	void SendPackageBuy(UINT nAccountDBID, int nResult, int nPetalBalance, int nPackageSN, char cPackageCount, TCashItemBase *PackageList, TPaymentPackageItemInfoEx* PaymentPackageItem=NULL, char cPaymentRules = 0, INT64 biSaleCouponSerial=0, INT64 nSeedPoint=0);
#else
	void SendBuy(UINT nAccountDBID, int nResult, char cCartType, int nPetalBalance, char cProductCount, TCashBuyItem *BuyList, TPaymentItemInfoEx* pPaymentItem=NULL, char cPaymentRules = 0, INT64 nSeedPoint=0);
	void SendPackageBuy(UINT nAccountDBID, int nResult, int nPetalBalance, int nPackageSN, char cPackageCount, TCashItemBase *PackageList, TPaymentPackageItemInfoEx* PaymentPackageItem=NULL, char cPaymentRules = 0, INT64 nSeedPoint=0);
#endif

#if defined(PRE_ADD_CASH_REFUND)
	void SendMoveCashInven(UINT nAccountDBID, int nResult, int nTotalPetal, INT64 biPurchaseOrderDetailID, TCashItemBase* pCashItem, INT64 nSeedPoint);	
	void SendPackageMoveCashInven(UINT nAccountDBID, int nResult, int nTotalPetal, INT64 biPurchaseOrderDetailID, TCashItemBase* pCashItemList, INT64 nSeedPoint);	
	void SendCashRefund(UINT nAccountDBID, int nResult, char cItemType, INT64 biPurchaseOrderDetailID );
#endif	
	void SendGift(int nResult, UINT nAccountDBID, UINT nReceiverAccountDBID, INT64 biReceiverCharacterDBID, int nReceiverGiftCount, int nPetal, char cCartType, char cGiftCount, TCashGiftItem *GiftList, int nMailDBID, INT64 nSeedPoint);
	void SendPackageGift(int nResult, UINT nAccountDBID, UINT nReceiverAccountDBID, INT64 biReceiverCharacterDBID, int nReceiverGiftCount, int nPetalBalance, int nPackageSN, char cPackageCount, TCashPackageGiftItem *PackageList, int nMailDBID, INT64 nSeedPoint);
	void SendCoupon(UINT nAccountDBID, int nResult);
	void SendVIPBuy(UINT nAccountDBID, int nResult, int nPetal, int nCash, int nItemSN, int nVIPTotalPoint, __time64_t tVIPEndDate, bool bAutoPay, bool bServer, INT64 nSeedPoint);
	void SendVIPGift(UINT nAccountDBID, int nResult, int nPetal, int nCash, int nItemSN, UINT nReceiverAccountDBID, INT64 biReceiverCharacterDBID, int nReceiverGiftCount, INT64 nSeedPoint);
};
