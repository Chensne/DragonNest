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

	INT64 m_nReleaseTicks;	// ��ǰ ��ȸ�� �޾Ƴ��� Ticks (big-endian)

	std::map<int, TProductInquiry> m_MapItemSNList;
	std::map<int, TProductInquiry> m_MapProductNoList;
	std::map<int, TProductInquiry> m_MapHiddenProductNoList;	// ������ ������ ����Ʈ

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
	void OnRecvRefund(UINT nPacketNo, CByteStream &Stream); // û�� öȸ
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

	void SendProductInquiry(int nPageNo, int nRowPerPage);	// ��ǰ��ȸ
	void SendHiddenProductInquiry(int nPageNo, int nRowPerPage);	// ��ǰ��ȸ

	void SendCheckBalance(CDNUserRepository *pUser);	// �ܾ���ȸ
	void SendCheckBalanceWithNotRefundable(CDNUserRepository *pUser);	// �ܾ���ȸ

	void SendPurchaseItem(CDNUserRepository *pUser, UINT nPaymentType);	// ������ ����
	void SendPurchaseItemDiscount(CDNUserRepository *pUser, int nProductNo, int nProductPrice, UINT nPaymentType);	// ������ ����
	void SendPurchaseItemExtend(CDNUserRepository *pUser, UINT nPaymentType);	// ������ ����
	int SendPurchasePackageItem(CDNUserRepository *pUser, UINT nPaymentType);	// ������ ����
	int SendPurchasePackageItemExtend(CDNUserRepository *pUser, UINT nPaymentType);	// ������ ����
#if defined(PRE_ADD_CASH_REFUND)
	void SendRefund(CDNUserRepository *pUser, UINT uiOrderNo, int nProductNo, INT64 biPurchaseOrderDetailID);
#endif // #if defined(PRE_ADD_CASH_REFUND)

	void SendPurchaseGift(CDNUserRepository *pUser, UINT nPaymentType);	// �����ϱ�
	int SendPurchasePackageGiftItem(CDNUserRepository *pUser, UINT nPaymentType);	// ��Ű�� ����

	void SendCouponUsing(CDNUserRepository *pUser);

	void SendInventoryInquiryOrder(UINT nAccountDBID, WCHAR *pCharName);
	void SendInventoryPickup(UINT nAccountDBID, int nOrderNo, int nProductNo, short wOrderQuantity);

	void SendInventoryCheck(UINT nAccountDBID, WCHAR *pCharName, BYTE IsPresent);	// ���� �� ���� ���� ��ǰ�� �ִ��� üũ (IsPresent 0:�Ϲݱ��� 1:���� 2:�Ϲݱ���+����
	//void SendInventoryPickupOnce(UINT nAccountDBID, UINT nOrderNo, UINT nItemSN, short wItemCount);	// NISMS Inventory���� �������� ��ǰ���� ��������
	//void SendInventoryPickupRollback(UINT nAccountDBID, UINT nOrderNo, UINT nProductNo);	// Inventory Pickup���� ���� �������� ����
};

extern CDNBillingConnectionKR *g_pBillingConnection;

#endif