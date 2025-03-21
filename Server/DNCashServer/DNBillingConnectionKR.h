#pragma once

#include "Connection.h"

#if defined(_KR) || defined(_US)

const int ProductRowPerPage = 100;

class CByteStream;
class CDNUserRepository;
class CDNBillingConnectionKR: public CConnection
{
private:
	DWORD m_dwReconnectTick;
	DWORD m_dwHeartBeatTick;

	INT64 m_nReleaseTicks;	// 상품 조회시 받아놨던 Ticks (big-endian)

	std::map<int, TProductInquiry> m_MapItemSNList;
	std::map<int, TProductInquiry> m_MapProductNoList;
	std::map<int, TProductInquiry> m_MapHiddenProductNoList;	// 쿠폰용 아이템 리스트

	int m_nPage;
	int m_nHiddenPage;

	int m_nTotalProductCount;
	int m_nTotalHiddenProductCount;

public:
	CDNBillingConnectionKR(void);
	virtual ~CDNBillingConnectionKR(void);

	void DoUpdate(DWORD dwCurTick);
	void Reconnect(DWORD dwCurTick);
	virtual int MessageProcess(int nMainCmd, int nSubCmd, char *pData, int nLen);

	int OnBalanceInquiry(CDNUserRepository *pUser);
	int OnBuy(CDNUserRepository *pUser);
	int OnPackageBuy(CDNUserRepository *pUser);
	int OnGift(CDNUserRepository *pUser);
	int OnPackageGift(CDNUserRepository *pUser);
	int OnCoupon(CDNUserRepository *pUser);

	TProductInquiry *GetProductBySN(int nItemSN);
	int GetProductNoBySN(int nItemSN);
	int GetItemSNByProductNo(int nProductNo);
	TProductInquiry *GetProductByProductNo(int nProductNo);
	bool CheckProduct(int nItemSN);

	// Hidden
	TProductInquiry *GetHiddenProductByProductNo(int nProductNo);
	INT64 GetItemSNByHiddenProductNo(int nProductNo);

	int ConvertResult(int nBillingResult);
#if defined(PRE_ADD_CASH_REFUND)
	int RefundConvertResult(int nBillingResult);
#endif

	void OnRecvInitialize(UINT nPacketNo, CByteStream &Stream);
	void OnRecvHeartBeat(UINT nPacketNo, CByteStream &Stream);
	void OnRecvCheckBalance(UINT nPacketNo, CByteStream &Stream);
	void OnRecvCheckBalanceWithNotRefundable(UINT nPacketNo, CByteStream &Stream);
	void OnRecvPurchaseItem(UINT nPacketNo, CByteStream &Stream);
	void OnRecvPurchaseItemDiscount(UINT nPacketNo, CByteStream &Stream);
	void OnRecvPurchaseItemExtend(UINT nPacketNo, CByteStream &Stream);

#if defined(PRE_ADD_CASH_REFUND)
	void OnRecvRefund(UINT nPacketNo, CByteStream &Stream); // 청약 철회
#endif // #if defined(PRE_ADD_CASH_REFUND)

	void OnRecvPurchaseGift(UINT nPacketNo, CByteStream &Stream);
	void OnRecvCouponUsing(UINT nPacketNo, CByteStream &Stream);
	void OnRecvProductInquiry2(UINT nPacketNo, CByteStream &Stream);
	void OnRecvHiddenProductInquiry(UINT nPacketNo, CByteStream &Stream);
	void OnRecvInventoryPickup(UINT nPacketNo, CByteStream &Stream);
	void OnRecvInventoryInquiryOrder(UINT nPacketNo, CByteStream &Stream);

	//void OnRecvPurchaseItemAttribute(UINT nPacketNo, CByteStream &Stream);
	void OnRecvInventoryCheck(UINT nPacketNo, CByteStream &Stream);
	//void OnRecvInventoryPickupOnce(UINT nPacketNo, CByteStream &Stream);
	//void OnRecvPickupRollback(UINT nPacketNo, CByteStream &Stream);
	//void OnRecvInventoryClear(UINT nPacketNo, CByteStream &Stream);

	void SendInitialize();
	void SendHeartBeat();

	void SendProductInquiry(int nPageNo, int nRowPerPage);	// 상품조회
	void SendHiddenProductInquiry(int nPageNo, int nRowPerPage);	// 상품조회

	void SendCheckBalance(CDNUserRepository *pUser);	// 잔액조회
	void SendCheckBalanceWithNotRefundable(CDNUserRepository *pUser);	// 잔액조회

	void SendPurchaseItem(CDNUserRepository *pUser, UINT nPaymentType);	// 아이템 구매
	void SendPurchaseItemDiscount(CDNUserRepository *pUser, int nProductNo, int nProductPrice, UINT nPaymentType);	// 아이템 구매
	void SendPurchaseItemExtend(CDNUserRepository *pUser, UINT nPaymentType);	// 아이템 구매
	int SendPurchasePackageItem(CDNUserRepository *pUser, UINT nPaymentType);	// 아이템 구매
	int SendPurchasePackageItemExtend(CDNUserRepository *pUser, UINT nPaymentType);	// 아이템 구매
#if defined(PRE_ADD_CASH_REFUND)
	void SendRefund(CDNUserRepository *pUser, UINT uiOrderNo, int nProductNo, INT64 biPurchaseOrderDetailID);
#endif // #if defined(PRE_ADD_CASH_REFUND)

	void SendPurchaseGift(CDNUserRepository *pUser, UINT nPaymentType);	// 선물하기
	int SendPurchasePackageGiftItem(CDNUserRepository *pUser, UINT nPaymentType);	// 패키지 선물

	void SendCouponUsing(CDNUserRepository *pUser);

	void SendInventoryInquiryOrder(UINT nAccountDBID, WCHAR *pCharName);
	void SendInventoryPickup(UINT nAccountDBID, int nOrderNo, int nProductNo, short wOrderQuantity);

	void SendInventoryCheck(UINT nAccountDBID, WCHAR *pCharName, BYTE IsPresent);	// 구매 및 선물 받은 상품이 있는지 체크 (IsPresent 0:일반구매 1:선물 2:일반구매+선물
	//void SendInventoryPickupOnce(UINT nAccountDBID, UINT nOrderNo, UINT nItemSN, short wItemCount);	// NISMS Inventory에서 게임으로 상품정보 가져오기
	//void SendInventoryPickupRollback(UINT nAccountDBID, UINT nOrderNo, UINT nProductNo);	// Inventory Pickup으로 인한 이전상태 복구
};

extern CDNBillingConnectionKR *g_pBillingConnection;

#endif