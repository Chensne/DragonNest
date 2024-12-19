#include "StdAfx.h"
#include "DNBillingConnectionKR.h"
#include "Stream.h"
#include "DNIocpManager.h"
#include "DNConnection.h"
#include "DNExtManager.h"
#include "DNUserRepository.h"
#include "DNManager.h"
#include "Log.h"

extern TCashConfig g_Config;

#if defined(_KR) || defined(_US)

CDNBillingConnectionKR *g_pBillingConnection = NULL;

CDNBillingConnectionKR::CDNBillingConnectionKR(void): CConnection()
{
	SetIp(g_Config.CashInfo.szIP);
	SetPort(g_Config.CashInfo.nPort);

	Init(1024 * 500, 1024 * 500);

	m_dwReconnectTick = m_dwHeartBeatTick = 0;
	m_nReleaseTicks = 0;

	m_MapItemSNList.clear();
	m_MapProductNoList.clear();
	m_MapHiddenProductNoList.clear();

	m_nPage = m_nHiddenPage = m_nTotalProductCount = m_nTotalHiddenProductCount = 0;
}

CDNBillingConnectionKR::~CDNBillingConnectionKR(void)
{
	SAFE_DELETE_MAP(m_MapItemSNList);
	SAFE_DELETE_MAP(m_MapProductNoList);
	SAFE_DELETE_MAP(m_MapHiddenProductNoList);
}

void CDNBillingConnectionKR::DoUpdate(DWORD dwCurTick)
{
	if (m_dwHeartBeatTick + 6000 < dwCurTick){
		SendHeartBeat();
		m_dwHeartBeatTick = dwCurTick;
	}
}

void CDNBillingConnectionKR::Reconnect(DWORD dwCurTick)
{
	if (m_dwReconnectTick + 5000 < dwCurTick){
		m_dwReconnectTick = dwCurTick;

		if (!GetActive() && !GetConnecting() )
		{
			SetConnecting(true);
			if (g_pIocpManager->AddConnectionEx(this, CONNECTIONKEY_BILLING, m_szIP, m_wPort) < 0)
			{
				SetConnecting(false);
				g_Log.Log(LogType::_ERROR, L"BillingServer Fail(%S, %d)\r\n", m_szIP, m_wPort);
			}
			else {
				g_Log.Log(LogType::_NORMAL, L"BillingServer(%S, %d) Connecting..\r\n", m_szIP, m_wPort);				
			}
		}
	}
}

int CDNBillingConnectionKR::MessageProcess(int nMainCmd, int nSubCmd, char *pData, int nLen)
{
	CByteStream Stream(pData, nLen + 5);

	BYTE cPacketID = 0;
	UINT nLength = 0;
	UINT nPacketNo = 0;
	BYTE cPacketType = 0;

	Stream.Read(&cPacketID, sizeof(BYTE));
	Stream.Read(&nLength, sizeof(UINT));
	Stream.Read(&nPacketNo, sizeof(UINT));
	nPacketNo = SWAP32(nPacketNo);			// User AccountDBID
	Stream.Read(&cPacketType, sizeof(BYTE));

	switch (cPacketType){
	case CASH_INITIALIZE: OnRecvInitialize(nPacketNo, Stream); break;
	case CASH_HEARTBEAT: OnRecvHeartBeat(nPacketNo, Stream); break;
	case CASH_CHECK_BALANCE: OnRecvCheckBalance(nPacketNo, Stream); break;
	case CASH_CHECK_BALANCE_WITH_NOT_REFUNDABLE: OnRecvCheckBalanceWithNotRefundable(nPacketNo, Stream); break;
	case CASH_PURCHASE_ITEM: OnRecvPurchaseItem(nPacketNo, Stream); break;
	case CASH_PURCHASE_ITEM_DISCOUNT: OnRecvPurchaseItemDiscount(nPacketNo, Stream); break;
	case CASH_PURCHASE_ITEM_EXTEND: OnRecvPurchaseItemExtend(nPacketNo, Stream); break;
	case CASH_PURCHASE_GIFT: OnRecvPurchaseGift(nPacketNo, Stream); break;
	case CASH_COUPON_USING: OnRecvCouponUsing(nPacketNo, Stream); break;

	case CASH_PRODUCT_INQUIRY2: OnRecvProductInquiry2(nPacketNo, Stream); break;
	case CASH_HIDDEN_PRODUCT_INQUIRY: OnRecvHiddenProductInquiry(nPacketNo, Stream); break;

#if defined(PRE_ADD_CASH_REFUND)
	case CASH_REFUND : OnRecvRefund(nPacketNo, Stream); break;
#endif // #if defined(PRE_ADD_CASH_REFUND)	

	case CASH_INVENTORY_PICKUP: OnRecvInventoryPickup(nPacketNo, Stream); break;
	case CASH_INVENTORY_INQUIRY_ORDER: OnRecvInventoryInquiryOrder(nPacketNo, Stream); break;

	//case CASH_PURCHASE_ITEM_ATTRIBUTE: OnRecvPurchaseItemAttribute(nPacketNo, Stream); break;
	case CASH_INVENTORY_CHECK: OnRecvInventoryCheck(nPacketNo, Stream); break;
	//case CASH_INVENTORY_PICKUP_ONCE: OnRecvInventoryPickupOnce(nPacketNo, Stream); break;
	//case CASH_INVENTORY_PICKUP_ROLLBACK: OnRecvPickupRollback(nPacketNo, Stream); break;
	//case CASH_INVENTORY_CLEAR: OnRecvInventoryClear(nPacketNo, Stream); break;
	}

	return ERROR_NONE;
}

int CDNBillingConnectionKR::OnBalanceInquiry(CDNUserRepository *pUser)
{
	if (!m_bActive)
		return ERROR_GENERIC_INVALIDREQUEST;

#if defined(_KR)
	SendCheckBalance(pUser);
#elif defined(_US)
	SendCheckBalanceWithNotRefundable(pUser);
#endif	// 

	return ERROR_NONE;
}

int CDNBillingConnectionKR::OnBuy(CDNUserRepository *pUser)
{
	if (!m_bActive)
		return ERROR_GENERIC_INVALIDREQUEST;

#if defined(_KR)
	SendPurchaseItem(pUser, 13001);
	//SendPurchaseItemDiscount(pUser, 0, 0, 13001);
#elif defined(_US)
	SendPurchaseItemExtend(pUser, 13001);
#endif
	return ERROR_NONE;
}

int CDNBillingConnectionKR::OnPackageBuy(CDNUserRepository *pUser)
{
	if (!m_bActive)
		return ERROR_GENERIC_INVALIDREQUEST;

	int nRet = ERROR_DB;
#if defined(_KR)
	nRet = SendPurchasePackageItem(pUser, 13001);
#elif defined(_US)
	nRet = SendPurchasePackageItemExtend(pUser, 13001);
#endif

	if (nRet != ERROR_NONE)
		return nRet;

	return ERROR_NONE;
}

int CDNBillingConnectionKR::OnGift(CDNUserRepository *pUser)
{
	if (!m_bActive)
		return ERROR_GENERIC_INVALIDREQUEST;

	SendPurchaseGift(pUser, 13001);

	return ERROR_NONE;
}

int CDNBillingConnectionKR::OnPackageGift(CDNUserRepository *pUser)
{
	if (!m_bActive)
		return ERROR_GENERIC_INVALIDREQUEST;

	SendPurchasePackageGiftItem(pUser, 13001);	// 13001 넥슨캐시

	return ERROR_NONE;
}

int CDNBillingConnectionKR::OnCoupon(CDNUserRepository *pUser)
{
	if (!m_bActive)
		return ERROR_GENERIC_INVALIDREQUEST;

	SendCouponUsing(pUser);

	return ERROR_NONE;
}

TProductInquiry *CDNBillingConnectionKR::GetProductBySN(int nItemSN)
{
	std::map<int, TProductInquiry>::iterator iter = m_MapItemSNList.find(nItemSN);
	if (iter == m_MapItemSNList.end()) return NULL;
	return &iter->second;
}

int CDNBillingConnectionKR::GetProductNoBySN(int nItemSN)
{
	TProductInquiry *pProduct = GetProductBySN(nItemSN);
	if (pProduct) return pProduct->nProductNo;
	return 0;
}

int CDNBillingConnectionKR::GetItemSNByProductNo(int nProductNo)
{
	TProductInquiry *pProduct = GetProductByProductNo(nProductNo);
	if (pProduct) return pProduct->nCommodityItemSN;
	return 0;
}

TProductInquiry *CDNBillingConnectionKR::GetProductByProductNo(int nProductNo)
{
	std::map<int, TProductInquiry>::iterator iter = m_MapProductNoList.find(nProductNo);
	if (iter == m_MapProductNoList.end()) return NULL;
	return &iter->second;
}

bool CDNBillingConnectionKR::CheckProduct(int nItemSN)
{
	TProductInquiry *pProduct = GetProductBySN(nItemSN);
	if (!pProduct) return false;

	TCashCommodityData CashData;
	bool bCash = g_pExtManager->GetCashCommodityData(nItemSN, CashData);
	if (!bCash) return false;

#if defined(PRE_ADD_SALE_COUPON)
	if( pProduct->nSalePrice <= 0 && !g_pExtManager->GetCashCommodityUseCoupon(nItemSN))
		return false;
	if (CashData.nPrice <= 0 && !g_pExtManager->GetCashCommodityUseCoupon(nItemSN)) return false;
#else
	if (pProduct->nSalePrice <= 0) return false;
	if (CashData.nPrice <= 0) return false;
#endif	

	if (pProduct->wProductExpire != CashData.wPeriod){
		g_Log.Log(LogType::_ERROR, L"CheckProduct Error (Period NISMS:%d Ext:%d)\r\n", pProduct->wProductExpire, CashData.wPeriod);
		return false;	// 기간
	}
	if (pProduct->nSalePrice != CashData.nPrice){
		g_Log.Log(LogType::_ERROR, L"CheckProduct Error (Price NISMS:%d Ext:%d)\r\n", pProduct->nSalePrice, CashData.nPrice);
		return false;	// 가격
	}
	if (pProduct->wProductPieces != CashData.nCount){
		g_Log.Log(LogType::_ERROR, L"CheckProduct Error (Count NISMS:%d Ext:%d)\r\n", pProduct->wProductPieces, CashData.nCount);
		return false;	// 개수
	}

	return true;
}

// Hidden
TProductInquiry *CDNBillingConnectionKR::GetHiddenProductByProductNo(int nProductNo)
{
	std::map<int, TProductInquiry>::iterator iter = m_MapHiddenProductNoList.find(nProductNo);
	if (iter == m_MapHiddenProductNoList.end()) return NULL;
	return &iter->second;
}

INT64 CDNBillingConnectionKR::GetItemSNByHiddenProductNo(int nProductNo)
{
	TProductInquiry *pProduct = GetHiddenProductByProductNo(nProductNo);
	if (pProduct) return pProduct->nCommodityItemSN;
	return 0;
}

int CDNBillingConnectionKR::ConvertResult(int nBillingResult)
{
	switch (nBillingResult)
	{
	case 1:		return ERROR_NONE;
	case 0:		return ERROR_NEXONBILLING_0;
	case 99:	return ERROR_NEXONBILLING_99;
	case 255:	return ERROR_NEXONBILLING_255;
	case 2:		return ERROR_NEXONBILLING_2;
	case 3:		return ERROR_NEXONBILLING_3;
	case 4:		return ERROR_NEXONBILLING_4;
	case 5:		return ERROR_NEXONBILLING_5;
	case 10:	return ERROR_NEXONBILLING_10;
	case 11:	return ERROR_NEXONBILLING_11;
	case 21:	return ERROR_NEXONBILLING_21;
	case 22:	return ERROR_NEXONBILLING_22;
	case 23:	return ERROR_NEXONBILLING_23;
	case 24:	return ERROR_NEXONBILLING_24;
	case 25:	return ERROR_NEXONBILLING_25;
	case 26:	return ERROR_NEXONBILLING_26;
	case 27:	return ERROR_NEXONBILLING_27;
	case 28:	return ERROR_NEXONBILLING_28;
	case 35:	return ERROR_NEXONBILLING_35;
	case 51:	return ERROR_NEXONBILLING_51;
	case 52:	return ERROR_NEXONBILLING_52;
	case 54:	return ERROR_NEXONBILLING_54;		
	case 61:	return ERROR_NEXONBILLING_61;
	case 62:	return ERROR_NEXONBILLING_62;
	case 63:	return ERROR_NEXONBILLING_63;
	case 65:	return ERROR_NEXONBILLING_65;
	case 66:	return ERROR_NEXONBILLING_66;
	case 81:	return ERROR_NEXONBILLING_81;
	case 82:	return ERROR_NEXONBILLING_82;
	case 100:	return ERROR_NEXONBILLING_100;
	case 101:	return ERROR_NEXONBILLING_101;
	case 102:	return ERROR_NEXONBILLING_102;
	case 103:	return ERROR_NEXONBILLING_103;
	case 149:	return ERROR_NEXONBILLING_149;
	case 1000:	return ERROR_NEXONBILLING_1000;
	case 1001:	return ERROR_NEXONBILLING_1001;
	case 1002:	return ERROR_NEXONBILLING_1002;
	case 1003:	return ERROR_NEXONBILLING_1003;
	case 11001:	return ERROR_NEXONBILLING_11001;
	case 11002:	return ERROR_NEXONBILLING_11002;
	case 12001:	return ERROR_NEXONBILLING_12001;
	case 12002:	return ERROR_NEXONBILLING_12002;
	case 12040:	return ERROR_NEXONBILLING_12040;
	case 12044:	return ERROR_NEXONBILLING_12044;
	case 120043: return ERROR_NEXONBILLING_120043;
	case 120062: return ERROR_NEXONBILLING_120062;
	default:
		return ERROR_NEXONBILLING_149;
	}

	return 0;
}

#if defined(PRE_ADD_CASH_REFUND)
int CDNBillingConnectionKR::RefundConvertResult(int nBillingResult)
{
	switch(nBillingResult)
	{
	case 0:		return ERROR_NEXONBILLING_0;
	case 1:		return ERROR_NONE;
	case 99:	return ERROR_NEXONBILLING_99;
	case 255:	return ERROR_NEXONBILLING_255;
	case 2:		return ERROR_NEXONBILLING_REFUND_2;
	case 3:		return ERROR_NEXONBILLING_REFUND_3;
	case 4:		return ERROR_NEXONBILLING_REFUND_4;
	case 5:		return ERROR_NEXONBILLING_REFUND_5;
	case 10:	return ERROR_NEXONBILLING_REFUND_10;
	case 11:	return ERROR_NEXONBILLING_REFUND_11;
	case 12:	return ERROR_NEXONBILLING_REFUND_12;
	}
	return ERROR_NEXONBILLING_0;
}
#endif

// OnRecv
void CDNBillingConnectionKR::OnRecvInitialize(UINT nPacketNo, CByteStream &Stream)
{
	USHORT wServiceCodeLen = 0;
	char szServiceCode[6] = { 0, };
	BYTE cServiceNo = 0;
	int nResult = 0;

	Stream.Read(&wServiceCodeLen, sizeof(USHORT));
	wServiceCodeLen = SWAP16(wServiceCodeLen);
	Stream.Read(szServiceCode, wServiceCodeLen);
	Stream.Read(&cServiceNo, sizeof(BYTE));
	Stream.Read(&nResult, sizeof(UINT));
	nResult = SWAP32(nResult);

	switch (nResult){
	case 0:	
		g_Log.Log(LogType::_ERROR, L"[CASH_INITIALIZE] Fail \r\n"); 
		break;	// 오류

	case 1: 
		g_Log.Log(LogType::_NORMAL, L"[CASH_INITIALIZE] Success \r\n"); 

		m_nPage = 1;
		SendProductInquiry(m_nPage, ProductRowPerPage);	// 1page
		m_nHiddenPage = 1;
		SendHiddenProductInquiry(m_nHiddenPage, ProductRowPerPage);
		break;	// 정상
	}
}

void CDNBillingConnectionKR::OnRecvHeartBeat(UINT nPacketNo, CByteStream &Stream)
{
	int nResult = 0;
	Stream.Read(&nResult, sizeof(UINT));
	nResult = SWAP32(nResult);

	switch (nResult){
	case 0:	// 오류
	case 1:	// 정상
	case 255:	// Maintenance
		break;

	case 17:	// 상품정보 업데이트
		m_nPage = 1;
		m_nTotalProductCount = 0;
		SendProductInquiry(m_nPage, ProductRowPerPage);	// 1page
		m_nHiddenPage = 1;
		m_nTotalHiddenProductCount = 0;
		SendHiddenProductInquiry(m_nHiddenPage, ProductRowPerPage);
		break;
	}
}

void CDNBillingConnectionKR::OnRecvCheckBalance(UINT nPacketNo, CByteStream &Stream)
{
	CDNUserRepository *pUser = g_pManager->GetUser(nPacketNo);
	if (!pUser){
		g_Log.Log(LogType::_ERROR, L"[OnRecvCheckBalance] User NULL! (AccountDBID:%u) \r\n", nPacketNo);
		return;
	}

	int nResult = 0;
	int nBalance = 0;

	Stream.Read(&nResult, sizeof(int));
	nResult = SWAP32(nResult);
	Stream.Read(&nBalance, sizeof(int));
	nBalance = SWAP32(nBalance);

	pUser->SendBalanceInquiry(ConvertResult(nResult), nBalance);

#if defined (PRE_ADD_REJECT_DUPLICATED_CASH_REQUEST)
	CRequestReleaser releaser(pUser->m_Requesting);
#endif // #if defined (PRE_ADD_REJECT_DUPLICATED_CASH_REQUEST)
}

void CDNBillingConnectionKR::OnRecvCheckBalanceWithNotRefundable(UINT nPacketNo, CByteStream &Stream)
{
	CDNUserRepository *pUser = g_pManager->GetUser(nPacketNo);
	if (!pUser){
		g_Log.Log(LogType::_ERROR, L"[OnRecvCheckBalance] User NULL! (AccountDBID:%u) \r\n", nPacketNo);
		return;
	}

	int nResult = 0;
	int nBalance = 0, nNotRefundableBalance = 0;

	Stream.Read(&nResult, sizeof(int));
	nResult = SWAP32(nResult);
	Stream.Read(&nBalance, sizeof(int));
	nBalance = SWAP32(nBalance);
	Stream.Read(&nNotRefundableBalance, sizeof(int));
	nNotRefundableBalance = SWAP32(nNotRefundableBalance);

	pUser->SendBalanceInquiry(ConvertResult(nResult), nBalance, nNotRefundableBalance);

#if defined (PRE_ADD_REJECT_DUPLICATED_CASH_REQUEST)
	CRequestReleaser releaser(pUser->m_Requesting);
#endif // #if defined (PRE_ADD_REJECT_DUPLICATED_CASH_REQUEST)
}

void CDNBillingConnectionKR::OnRecvPurchaseItem(UINT nPacketNo, CByteStream &Stream)
{
	CDNUserRepository *pUser = g_pManager->GetUser(nPacketNo);
	if (!pUser){
		g_Log.Log(LogType::_ERROR, L"[OnRecvPurchaseItem] User NULL! (AccountDBID:%u) \r\n", nPacketNo);
		return;
	}

#if defined (PRE_ADD_REJECT_DUPLICATED_CASH_REQUEST)
	CRequestReleaser releaser(pUser->m_Requesting);
#endif // #if defined (PRE_ADD_REJECT_DUPLICATED_CASH_REQUEST)

	USHORT wLen = 0;
	char szOrderID[64] = { 0, };
	int nResult = 0;
	UINT nOrderNo = 0;
	char cProductCount = 0;
	TProductItem PurchaseItem = {0, };

	Stream.Read(&wLen, sizeof(USHORT));
	wLen = SWAP16(wLen);
	Stream.Read(szOrderID, wLen);			// OrderID

	Stream.Read(&nResult, sizeof(UINT));	// Result
	nResult = SWAP32(nResult);

	Stream.Read(&nOrderNo, sizeof(UINT));	// OrderNo (빌링에서 온거)
	nOrderNo = SWAP32(nOrderNo);

	Stream.Read(&cProductCount, sizeof(char));

	if (nResult != 1){	// error!
		pUser->OnRecvBillingPurchase(ConvertResult(nResult), nResult, nOrderNo, szOrderID);
		g_Log.Log(LogType::_ERROR, pUser, L"[OnRecvPurchaseItem] nResult Error(%d) OrderNo:%d \r\n", nResult, nOrderNo);
		return;
	}

	for (int i = 0; i < cProductCount; i++)
	{
		memset(&PurchaseItem, 0, sizeof(TProductItem));
		Stream.Read(&PurchaseItem.nProductNo, sizeof(int));
		PurchaseItem.nProductNo = SWAP32(PurchaseItem.nProductNo);
		Stream.Read(&PurchaseItem.wOrderQuantity, sizeof(short));
		PurchaseItem.wOrderQuantity = SWAP16(PurchaseItem.wOrderQuantity);
		Stream.Read(&wLen, sizeof(USHORT));
		wLen = SWAP16(wLen);
		Stream.Read(PurchaseItem.szExtend, wLen);
#if defined(PRE_ADD_CASH_REFUND) // 즉시적용 상품이나 환불불가능한 애들만 우선 SetPickup..나머지는 캐쉬인벤으로 옮길때.
		int nItemSN = GetItemSNByProductNo(PurchaseItem.nProductNo);	
		int nItemID = g_pExtManager->GetCashCommodityItem0(nItemSN);
		if( pUser->GetCashMoveInven() || g_pExtManager->GetCashCommodityNoRefund( nItemID, nItemSN) )
#endif
		{
			SendInventoryPickup(nPacketNo, nOrderNo, PurchaseItem.nProductNo, PurchaseItem.wOrderQuantity);
		}
	}

	pUser->OnRecvBillingPurchase(ConvertResult(nResult), nResult, nOrderNo, szOrderID);
	// SendInventoryPickup(nPacketNo, nOrderNo, PurchaseItem.nProductNo, PurchaseItem.wOrderQuantity);	// 인벤에 들어오게끔 날려준다
}

void CDNBillingConnectionKR::OnRecvPurchaseItemDiscount(UINT nPacketNo, CByteStream &Stream)
{
	CDNUserRepository *pUser = g_pManager->GetUser(nPacketNo);
	if (!pUser){
		g_Log.Log(LogType::_ERROR, L"[OnRecvPurchaseItemDiscount] User NULL! (AccountDBID:%u) \r\n", nPacketNo);
		return;
	}

#if defined (PRE_ADD_REJECT_DUPLICATED_CASH_REQUEST)
	CRequestReleaser releaser(pUser->m_Requesting);
#endif // #if defined (PRE_ADD_REJECT_DUPLICATED_CASH_REQUEST)

	USHORT wLen = 0;
	char szOrderID[64] = { 0, };
	int nResult = 0;
	UINT nOrderNo = 0;
	char cProductCount = 0;
	TProductItem PurchaseItem = {0, };

	Stream.Read(&wLen, sizeof(USHORT));
	wLen = SWAP16(wLen);
	Stream.Read(szOrderID, wLen);			// OrderID

	Stream.Read(&nResult, sizeof(UINT));	// Result
	nResult = SWAP32(nResult);

	Stream.Read(&nOrderNo, sizeof(UINT));	// OrderNo (빌링에서 온거)
	nOrderNo = SWAP32(nOrderNo);

	if (nResult != 1){	// error!
		pUser->OnRecvBillingPurchase(ConvertResult(nResult), nResult, nOrderNo, szOrderID);
		g_Log.Log(LogType::_ERROR, pUser, L"[OnRecvPurchaseItem] nResult Error(%d) OrderNo:%d \r\n", nResult, nOrderNo);
		return;
	}

	memset(&PurchaseItem, 0, sizeof(TProductItem));
	Stream.Read(&PurchaseItem.nProductNo, sizeof(int));
	PurchaseItem.nProductNo = SWAP32(PurchaseItem.nProductNo);
	Stream.Read(&PurchaseItem.wOrderQuantity, sizeof(short));
	PurchaseItem.wOrderQuantity = SWAP16(PurchaseItem.wOrderQuantity);
	Stream.Read(&wLen, sizeof(USHORT));
	wLen = SWAP16(wLen);
	Stream.Read(PurchaseItem.szExtend, wLen);
#if defined(PRE_ADD_CASH_REFUND) // 즉시적용 상품이나 환불불가능한 애들만 우선 SetPickup..나머지는 캐쉬인벤으로 옮길때.
	int nItemSN = GetItemSNByProductNo(PurchaseItem.nProductNo);	
	int nItemID = g_pExtManager->GetCashCommodityItem0(nItemSN);
	if( pUser->GetCashMoveInven() || g_pExtManager->GetCashCommodityNoRefund( nItemID, nItemSN) )
#endif
	{
		SendInventoryPickup(nPacketNo, nOrderNo, PurchaseItem.nProductNo, PurchaseItem.wOrderQuantity);
	}

	pUser->OnRecvBillingPurchase(ConvertResult(nResult), nResult, nOrderNo, szOrderID);
}

void CDNBillingConnectionKR::OnRecvPurchaseItemExtend(UINT nPacketNo, CByteStream &Stream)
{
	CDNUserRepository *pUser = g_pManager->GetUser(nPacketNo);
	if (!pUser){
		g_Log.Log(LogType::_ERROR, L"[OnRecvPurchaseItemExtend] User NULL! (AccountDBID:%u) \r\n", nPacketNo);
		return;
	}

#if defined (PRE_ADD_REJECT_DUPLICATED_CASH_REQUEST)
	CRequestReleaser releaser(pUser->m_Requesting);
#endif // #if defined (PRE_ADD_REJECT_DUPLICATED_CASH_REQUEST)

	USHORT wLen = 0;
	char szOrderID[64] = { 0, };
	int nResult = 0;
	UINT nOrderNo = 0;
	char cProductCount = 0, cPaymentRuleID = 0;
	TProductItem PurchaseItem = {0, };

	Stream.Read(&wLen, sizeof(USHORT));
	wLen = SWAP16(wLen);
	Stream.Read(szOrderID, wLen);			// OrderID

	Stream.Read(&nResult, sizeof(UINT));	// Result
	nResult = SWAP32(nResult);

	Stream.Read(&nOrderNo, sizeof(UINT));	// OrderNo (빌링에서 온거)
	nOrderNo = SWAP32(nOrderNo);

	Stream.Read(&cPaymentRuleID, sizeof(char));	// Payment RuleID

	Stream.Read(&cProductCount, sizeof(char));

	if (nResult != 1){	// error!
		pUser->OnRecvBillingPurchase(ConvertResult(nResult), nResult, nOrderNo, szOrderID);
		g_Log.Log(LogType::_ERROR, pUser, L"[OnRecvPurchaseItem] nResult Error(%d) OrderNo:%d \r\n", nResult, nOrderNo);
		return;
	}

	for (int i = 0; i < cProductCount; i++)
	{
		memset(&PurchaseItem, 0, sizeof(TProductItem));
		Stream.Read(&PurchaseItem.nProductNo, sizeof(int));
		PurchaseItem.nProductNo = SWAP32(PurchaseItem.nProductNo);
		Stream.Read(&PurchaseItem.wOrderQuantity, sizeof(short));
		PurchaseItem.wOrderQuantity = SWAP16(PurchaseItem.wOrderQuantity);
		Stream.Read(&wLen, sizeof(USHORT));
		wLen = SWAP16(wLen);
		Stream.Read(PurchaseItem.szExtend, wLen);
		SendInventoryPickup(nPacketNo, nOrderNo, PurchaseItem.nProductNo, PurchaseItem.wOrderQuantity);
	}

	pUser->OnRecvBillingPurchase(ConvertResult(nResult), nResult, nOrderNo, szOrderID);
}

#if defined(PRE_ADD_CASH_REFUND)
void CDNBillingConnectionKR::OnRecvRefund(UINT nPacketNo, CByteStream &Stream)
{
	CDNUserRepository *pUser  = g_pManager->GetUser(nPacketNo);
	if (!pUser)
	{
		g_Log.Log(LogType::_ERROR, L"[OnRecvRefund] User NULL! (AccountDBID:%u) \r\n", nPacketNo);
		return;
	}

#if defined (PRE_ADD_REJECT_DUPLICATED_CASH_REQUEST)
	CRequestReleaser releaser(pUser->m_Requesting);
#endif // #if defined (PRE_ADD_REJECT_DUPLICATED_CASH_REQUEST)

	int nResult = 0;	

	Stream.Read(&nResult, sizeof(UINT));
	nResult = SWAP32(nResult);

	pUser->OnRecvBillingRefund(RefundConvertResult(nResult));
}
#endif // #if defined(PRE_ADD_CASH_REFUND)

void CDNBillingConnectionKR::OnRecvPurchaseGift(UINT nPacketNo, CByteStream &Stream)
{
	CDNUserRepository *pUser = g_pManager->GetUser(nPacketNo);
	if (!pUser){
		g_Log.Log(LogType::_ERROR, L"[OnRecvPurchaseGift] User NULL! (AccountDBID:%u) \r\n", nPacketNo);
		return;
	}

#if defined (PRE_ADD_REJECT_DUPLICATED_CASH_REQUEST)
	CRequestReleaser releaser(pUser->m_Requesting);
#endif // #if defined (PRE_ADD_REJECT_DUPLICATED_CASH_REQUEST)

	USHORT wLen = 0;
	char szOrderID[64] = { 0, };
	int nResult = 0;
	UINT nOrderNo = 0;
	char cProductCount = 0;
	TProductItem PurchaseItem = {0, };

	Stream.Read(&wLen, sizeof(USHORT));
	wLen = SWAP16(wLen);
	Stream.Read(szOrderID, wLen);			// OrderID

	Stream.Read(&nResult, sizeof(UINT));
	nResult = SWAP32(nResult);				// Result

	Stream.Read(&nOrderNo, sizeof(UINT));
	nOrderNo = SWAP32(nOrderNo);			// 빌링 OrderNo

	Stream.Read(&cProductCount, sizeof(char));

	if (nResult != 1){	// error!
		pUser->OnRecvBillingPurchase(ConvertResult(nResult), nResult, nOrderNo, szOrderID);
		g_Log.Log(LogType::_ERROR, pUser, L"[OnRecvPurchaseGift] nResult Error(%d) OrderNo:%d \r\n", nResult, nOrderNo);
		return;
	}

	for (int i = 0; i < cProductCount; i++){
		memset(&PurchaseItem, 0, sizeof(TProductItem));
		Stream.Read(&PurchaseItem.nProductNo, sizeof(int));
		PurchaseItem.nProductNo = SWAP32(PurchaseItem.nProductNo);
		Stream.Read(&PurchaseItem.wOrderQuantity, sizeof(short));
		PurchaseItem.wOrderQuantity = SWAP16(PurchaseItem.wOrderQuantity);
		Stream.Read(&wLen, sizeof(USHORT));
		wLen = SWAP16(wLen);
		Stream.Read(PurchaseItem.szExtend, wLen);

		SendInventoryPickup(nPacketNo, nOrderNo, PurchaseItem.nProductNo, PurchaseItem.wOrderQuantity);	// 인벤에 들어오게끔 날려준다
	}

	pUser->OnRecvBillingPurchase(ConvertResult(nResult), nResult, nOrderNo, szOrderID);
}

void CDNBillingConnectionKR::OnRecvCouponUsing(UINT nPacketNo, CByteStream &Stream)
{
	CDNUserRepository *pUser = g_pManager->GetUser(nPacketNo);
	if (!pUser){
		g_Log.Log(LogType::_ERROR, L"[OnRecvCouponUsing] User NULL! (AccountDBID:%u) \r\n", nPacketNo);
		return;
	}

#if defined (PRE_ADD_REJECT_DUPLICATED_CASH_REQUEST)
	CRequestReleaser releaser(pUser->m_Requesting);
#endif // #if defined (PRE_ADD_REJECT_DUPLICATED_CASH_REQUEST)

	USHORT wLen = 0;
	char szCouponTXID[65] = { 0, };
	int nResult = 0;
	UINT nOrderNo = 0;
	char cProductCount = 0;
	TProductItem PurchaseItem = {0, };

	Stream.Read(&wLen, sizeof(USHORT));
	wLen = SWAP16(wLen);
	Stream.Read(szCouponTXID, wLen);		// Coupon TXID

	Stream.Read(&nResult, sizeof(UINT));
	nResult = SWAP32(nResult);				// Result

	Stream.Read(&nOrderNo, sizeof(UINT));
	nOrderNo = SWAP32(nOrderNo);			// 빌링 OrderNo

	Stream.Read(&cProductCount, sizeof(char));

	if (nResult != 1){	// error!
		pUser->SendCoupon(ConvertResult(nResult));
		g_Log.Log(LogType::_ERROR, pUser, L"[OnRecvCouponUsing] nResult Error(%d) \r\n", nResult);
		return;
	}

	int nCouponItemSN = 0;
	for (int i = 0; i < cProductCount; i++){
		memset(&PurchaseItem, 0, sizeof(TProductItem));
		Stream.Read(&PurchaseItem.nProductNo, sizeof(int));
		PurchaseItem.nProductNo = SWAP32(PurchaseItem.nProductNo);
		Stream.Read(&wLen, sizeof(USHORT));
		wLen = SWAP16(wLen);
		Stream.Read(PurchaseItem.szExtend, wLen);

		nCouponItemSN = (int)GetItemSNByHiddenProductNo(PurchaseItem.nProductNo);
		if (nCouponItemSN <= 0)
			nCouponItemSN = (int)GetItemSNByProductNo(PurchaseItem.nProductNo);
	}

	pUser->OnRecvBillingCoupon(nResult, ConvertResult(nResult), nCouponItemSN, nOrderNo);
	// SendInventoryPickup(nPacketNo, nOrderNo, PurchaseItem.nProductNo, 1);	// 인벤에 들어오게끔 날려준다
}

void CDNBillingConnectionKR::OnRecvProductInquiry2(UINT nPacketNo, CByteStream &Stream)
{
	int nResult = 0;
	Stream.Read(&nResult, sizeof(UINT));
	nResult = SWAP32(nResult);
	g_Log.Log(LogType::_FILEDBLOG, L"[CASH_PRODUCT_INQUIRY2] Result:%d\r\n", nResult);

	Stream.Read(&m_nReleaseTicks, sizeof(INT64));

	int nTotalProductCount = 0;
	Stream.Read(&nTotalProductCount, sizeof(int));
	nTotalProductCount = SWAP32(nTotalProductCount);

	if (nTotalProductCount <= 0){
		g_Log.Log(LogType::_FILEDBLOG, L"[CASH_PRODUCT_INQUIRY2] nTotalProductCount <= 0\r\n");
		return;
	}

	int nProductCount = 0;
	Stream.Read(&nProductCount, sizeof(int));
	nProductCount = SWAP32(nProductCount);

	USHORT wLen = 0;
	TProductInquiry Product = { 0, };
	TBonusProductInfo BonusProduct = { 0, };

	for (int i = 0; i < nProductCount; i++){
		memset(&Product, 0, sizeof(TProductInquiry));
		Stream.Read(&Product.nProductNo, sizeof(int));
		Product.nProductNo = SWAP32(Product.nProductNo);

		Stream.Read(&Product.nRelationProductNo, sizeof(int));
		Product.nRelationProductNo = SWAP32(Product.nRelationProductNo);

		Stream.Read(&Product.wProductExpire, sizeof(short));
		Product.wProductExpire = SWAP16(Product.wProductExpire);

		Stream.Read(&Product.wProductPieces, sizeof(short));
		Product.wProductPieces = SWAP16(Product.wProductPieces);

		Stream.Read(&wLen, sizeof(USHORT));
		wLen = SWAP16(wLen);
		Stream.Read(Product.szProductID, wLen);

		Stream.Read(&wLen, sizeof(USHORT));
		wLen = SWAP16(wLen);
		Stream.Read(Product.szProductGUID, wLen);

		Stream.Read(&Product.nPaymentType, sizeof(int));
		Product.nPaymentType = SWAP32(Product.nPaymentType);

		Stream.Read(&wLen, sizeof(USHORT));
		wLen = SWAP16(wLen);
		Stream.Read(Product.szProductType, wLen);

		Stream.Read(&Product.nSalePrice, sizeof(int));
		Product.nSalePrice = SWAP32(Product.nSalePrice);

		Stream.Read(&Product.nCategoryNo, sizeof(int));
		Product.nCategoryNo = SWAP32(Product.nCategoryNo);

		Stream.Read(&Product.wProductStatus, sizeof(short));
		Product.wProductStatus = SWAP16(Product.wProductStatus);

		Stream.Read(&Product.cBonusProductCount, sizeof(char));

		for (int j = 0; j < Product.cBonusProductCount; j++){
			memset(&BonusProduct, 0, sizeof(TBonusProductInfo));
			Stream.Read(&BonusProduct.nProductNo, sizeof(UINT));
			BonusProduct.nProductNo = SWAP32(BonusProduct.nProductNo);

			Stream.Read(&wLen, sizeof(USHORT));
			wLen = SWAP16(wLen);

			Stream.Read(BonusProduct.szExtend, wLen);

			Product.vBonusProduct.push_back(BonusProduct);
		}

		INT64 biItemSN = _atoi64(Product.szProductID);
		Product.nCommodityItemSN = (int)biItemSN;
		m_MapItemSNList[Product.nCommodityItemSN] = Product;
		m_MapProductNoList[Product.nProductNo] = Product;

		m_nTotalProductCount++;

		g_Log.Log(LogType::_FILEDBLOG, L"[OnRecvProductInquiry2] %d,%d\r\n", Product.nCommodityItemSN, Product.nProductNo);
	}

	int nRemainCount = nTotalProductCount - m_nTotalProductCount;
	if (nRemainCount > 0){
		m_nPage++;
		SendProductInquiry(m_nPage, ProductRowPerPage);
	}

	g_Log.Log(LogType::_FILEDBLOG, L"[CASH_PRODUCT_INQUIRY2] nRemainCount:%d nTotalProductCount:%d m_nTotalProductCount:%d m_nPage:%d \r\n", nRemainCount, nTotalProductCount, m_nTotalProductCount, m_nPage);
}

void CDNBillingConnectionKR::OnRecvHiddenProductInquiry(UINT nPacketNo, CByteStream &Stream)
{
	int nResult = 0;
	Stream.Read(&nResult, sizeof(UINT));
	nResult = SWAP32(nResult);
	g_Log.Log(LogType::_FILEDBLOG, L"[OnRecvHiddenProductInquiry] Result:%d\r\n", nResult);

	Stream.Read(&m_nReleaseTicks, sizeof(INT64));

	UINT nTotalProductCount = 0;
	Stream.Read(&nTotalProductCount, sizeof(UINT));
	nTotalProductCount = SWAP32(nTotalProductCount);

	if (nTotalProductCount <= 0){
		g_Log.Log(LogType::_FILEDBLOG, L"[OnRecvHiddenProductInquiry] nTotalProductCount <= 0\r\n");
		return;
	}

	int nProductCount = 0;
	Stream.Read(&nProductCount, sizeof(int));
	nProductCount = SWAP32(nProductCount);

	USHORT wLen = 0;
	TProductInquiry Product = { 0, };
	TBonusProductInfo BonusProduct = { 0, };

	for (int i = 0; i < nProductCount; i++){
		memset(&Product, 0, sizeof(TProductInquiry));
		Stream.Read(&Product.nProductNo, sizeof(int));
		Product.nProductNo = SWAP32(Product.nProductNo);

		Stream.Read(&Product.nRelationProductNo, sizeof(int));
		Product.nRelationProductNo = SWAP32(Product.nRelationProductNo);

		Stream.Read(&Product.wProductExpire, sizeof(short));
		Product.wProductExpire = SWAP16(Product.wProductExpire);

		Stream.Read(&Product.wProductPieces, sizeof(short));
		Product.wProductPieces = SWAP16(Product.wProductPieces);

		Stream.Read(&wLen, sizeof(USHORT));
		wLen = SWAP16(wLen);
		Stream.Read(Product.szProductID, wLen);

		Stream.Read(&wLen, sizeof(USHORT));
		wLen = SWAP16(wLen);
		Stream.Read(Product.szProductGUID, wLen);

		Stream.Read(&Product.nPaymentType, sizeof(int));
		Product.nPaymentType = SWAP32(Product.nPaymentType);

		Stream.Read(&wLen, sizeof(USHORT));
		wLen = SWAP16(wLen);
		Stream.Read(Product.szProductType, wLen);

		Stream.Read(&Product.nSalePrice, sizeof(int));
		Product.nSalePrice = SWAP32(Product.nSalePrice);

		Stream.Read(&Product.nCategoryNo, sizeof(int));
		Product.nCategoryNo = SWAP32(Product.nCategoryNo);

		Stream.Read(&Product.cBonusProductCount, sizeof(char));

		for (int j = 0; j < Product.cBonusProductCount; j++){
			memset(&BonusProduct, 0, sizeof(TBonusProductInfo));

			Stream.Read(&BonusProduct.nProductNo, sizeof(UINT));
			BonusProduct.nProductNo = SWAP32(BonusProduct.nProductNo);

			Stream.Read(&wLen, sizeof(USHORT));
			wLen = SWAP16(wLen);
			Stream.Read(BonusProduct.szExtend, wLen);

			Product.vBonusProduct.push_back(BonusProduct);
		}

		INT64 biItemSN = _atoi64(Product.szProductID);
		Product.nCommodityItemSN = (int)biItemSN;
		m_MapItemSNList[Product.nCommodityItemSN] = Product;
		m_MapHiddenProductNoList[Product.nProductNo] = Product;

		m_nTotalHiddenProductCount++;

		g_Log.Log(LogType::_FILEDBLOG, L"[Hidden] %d,%d\r\n", Product.nCommodityItemSN, Product.nProductNo);
	}

	int nRemainCount = nTotalProductCount - m_nTotalHiddenProductCount;
	if (nRemainCount > 0){
		m_nHiddenPage++;
		SendHiddenProductInquiry(m_nHiddenPage, ProductRowPerPage);
	}

	g_Log.Log(LogType::_FILEDBLOG, L"[OnRecvHiddenProductInquiry] nRemainCount:%d nTotalProductCount:%d m_nTotalHiddenProductCount:%d m_nHiddenPage:%d \r\n", nRemainCount, nTotalProductCount, m_nTotalHiddenProductCount, m_nHiddenPage);
}

void CDNBillingConnectionKR::OnRecvInventoryPickup(UINT nPacketNo, CByteStream &Stream)
{
	CDNUserRepository *pUser = g_pManager->GetUser(nPacketNo);
	if (!pUser){
		g_Log.Log(LogType::_ERROR, L"[OnRecvInventoryPickup] User NULL! (AccountDBID:%u) \r\n", nPacketNo);
		return;
	}

#if defined (PRE_ADD_REJECT_DUPLICATED_CASH_REQUEST)
	CRequestReleaser releaser(pUser->m_Requesting);
#endif // #if defined (PRE_ADD_REJECT_DUPLICATED_CASH_REQUEST)

	int nResult = 0;
	int nOrderNo = 0;

	TProductItem PurchaseItem = { 0, };
	Stream.Read(&nResult, sizeof(int));
	nResult = SWAP32(nResult);
	Stream.Read(&nOrderNo, sizeof(int));
	nOrderNo = SWAP32(nOrderNo);
	Stream.Read(&PurchaseItem.nProductNo, sizeof(int));
	PurchaseItem.nProductNo = SWAP32(PurchaseItem.nProductNo);
	Stream.Read(&PurchaseItem.wOrderQuantity, sizeof(short));
	PurchaseItem.wOrderQuantity = SWAP16(PurchaseItem.wOrderQuantity);

	// pUser->SetPickup(PurchaseItem.nProductNo, ConvertResult(nResult), nResult, nOrderNo);
}

void CDNBillingConnectionKR::OnRecvInventoryInquiryOrder(UINT nPacketNo, CByteStream &Stream)
{
	CDNUserRepository *pUser = g_pManager->GetUser(nPacketNo);
	if (!pUser){
		g_Log.Log(LogType::_ERROR, L"[OnRecvInventoryInquiryOrder] User NULL! (AccountDBID:%u) \r\n", nPacketNo);
		return;
	}

	UINT nResult = 0;
	Stream.Read(&nResult, sizeof(UINT));		// Result
	nResult = SWAP32(nResult);

	int nTotalCount = 0;
	Stream.Read(&nTotalCount, sizeof(int));		// TotalCount
	nTotalCount = SWAP32(nTotalCount);

	int nReturnCount = 0;
	Stream.Read(&nReturnCount, sizeof(int));	// ReturnCount
	nReturnCount = SWAP32(nReturnCount);

	TInventoryInquiryProduct Product = {0, };
	for (int i = 0; i < nReturnCount; i++){
		Stream.Read(&Product.nOrderNo, sizeof(int));		// OrderNo
		Product.nOrderNo = SWAP32(Product.nOrderNo);

		Stream.Read(&Product.nProductNo, sizeof(int));		// ProductNo
		Product.nProductNo = SWAP32(Product.nProductNo);

		Stream.Read(&Product.cProductKind, sizeof(char));	// ProductKind

		int wLen = 0;
		Stream.Read(&wLen, sizeof(USHORT));
		wLen = SWAP16(wLen);
		Stream.Read(Product.szProductName, wLen);				// ProductName

		Stream.Read(&wLen, sizeof(USHORT));
		wLen = SWAP16(wLen);
		Stream.Read(Product.szProductID, wLen);				// ProductID

		Stream.Read(&Product.wProductExpire, sizeof(short));		// Product Expire
		Product.wProductExpire = SWAP16(Product.wProductExpire);

		Stream.Read(&Product.wProductPieces, sizeof(short));		// Product Pieces
		Product.wProductPieces = SWAP16(Product.wProductPieces);

		Stream.Read(&Product.wOrderQuantity, sizeof(short));	// Order Quantity
		Product.wOrderQuantity = SWAP16(Product.wOrderQuantity);

		Stream.Read(&Product.wRemainProductQuantity, sizeof(short));	// Remain Product Quantity
		Product.wRemainProductQuantity = SWAP16(Product.wRemainProductQuantity);

		Stream.Read(&Product.cPresent, sizeof(char));		// Is Present

		Stream.Read(&Product.cRead, sizeof(char));		// Is Read

		Stream.Read(&Product.cSenderServerNo, sizeof(char));		// Sender Server No

		Stream.Read(&wLen, sizeof(USHORT));
		wLen = SWAP16(wLen);
		Stream.Read(Product.szSenderGameID, wLen);				// Sender Game ID

		Stream.Read(&wLen, sizeof(USHORT));
		wLen = SWAP16(wLen);
		Stream.Read(Product.szSenderPresentMessage, wLen);		// Sender Present Message

		for (int i = 0; i < 5; i++){
			Stream.Read(&wLen, sizeof(USHORT));
			wLen = SWAP16(wLen);
			Stream.Read(Product.szProductAttribute[i], wLen);
		}

		Stream.Read(&wLen, sizeof(USHORT));
		wLen = SWAP16(wLen);
		Stream.Read(Product.szExtend, wLen);					// Extend value

		SendInventoryPickup(pUser->GetAccountDBID(), Product.nOrderNo, Product.nProductNo, Product.wOrderQuantity);
	}
}

void CDNBillingConnectionKR::OnRecvInventoryCheck(UINT nPacketNo, CByteStream &Stream)
{
	CDNUserRepository *pUser = g_pManager->GetUser(nPacketNo);
	if (!pUser){
		g_Log.Log(LogType::_ERROR, L"[OnRecvInventoryInquiryOrder] User NULL! (AccountDBID:%u) \r\n", nPacketNo);
		return;
	}

	UINT nResult = 0;
	Stream.Read(&nResult, sizeof(UINT));		// Result
	nResult = SWAP32(nResult);

	char cOrderType = 0;
	Stream.Read(&cOrderType, sizeof(char));

	int nTotalCount = 0;
	Stream.Read(&nTotalCount, sizeof(UINT));
	nTotalCount = SWAP32(nTotalCount);
}

// Send
void CDNBillingConnectionKR::SendInitialize()
{
	UINT nPacketNo = 0;
	nPacketNo = SWAP32(nPacketNo);
	BYTE cPacketType = CASH_INITIALIZE;
	char szServiceCode[6] = "DNEST";	// 게임종류
	USHORT wServiceCodeLen = (USHORT)strlen(szServiceCode);
	USHORT wSwapLen = SWAP16(wServiceCodeLen);

	// Header
	BYTE cPacketID = 0xAF;
	UINT nLength = sizeof(UINT) + sizeof(BYTE) + sizeof(USHORT) + wServiceCodeLen + sizeof(BYTE);		// length: PacketNo + PacketType + Data...
	nLength = SWAP32(nLength);

	CByteStream Stream;
	// Header
	Stream.Write(&cPacketID, sizeof(BYTE));
	Stream.Write(&nLength, sizeof(UINT));
	// Data
	Stream.Write(&nPacketNo, sizeof(UINT));
	Stream.Write(&cPacketType, sizeof(BYTE));
	Stream.Write(&wSwapLen, sizeof(USHORT));
	Stream.Write(szServiceCode, wServiceCodeLen);
	Stream.Write(&g_Config.nServerNo, sizeof(BYTE));	// 서버번호

	AddSendData(Stream.GetBuffer(), Stream.Size());
}

void CDNBillingConnectionKR::SendHeartBeat()
{
	UINT nPacketNo = 0;
	nPacketNo = SWAP32(nPacketNo);
	BYTE cPacketType = CASH_HEARTBEAT;

	// Header
	BYTE cPacketID = 0xAF;
	UINT nLength = sizeof(UINT) + sizeof(BYTE) + sizeof(INT64);		// length: PacketNo + PacketType + Data...
	nLength = SWAP32(nLength);

	CByteStream Stream;
	// Header
	Stream.Write(&cPacketID, sizeof(BYTE));
	Stream.Write(&nLength, sizeof(UINT));
	// Data
	Stream.Write(&nPacketNo, sizeof(UINT));
	Stream.Write(&cPacketType, sizeof(BYTE));
	Stream.Write(&m_nReleaseTicks, sizeof(INT64));

	AddSendData(Stream.GetBuffer(), Stream.Size());
}

void CDNBillingConnectionKR::SendProductInquiry(int nPageNo, int nRowPerPage)
{
	UINT nPacketNo = 0;
	nPacketNo = SWAP32(nPacketNo);
	BYTE cPacketType = CASH_PRODUCT_INQUIRY2;
	int nPage = SWAP32(nPageNo);
	int nRow = SWAP32(nRowPerPage);

	// Header
	BYTE cPacketID = 0xAF;
	UINT nLength = sizeof(UINT) + sizeof(BYTE) + (sizeof(int) * 2);		// length: PacketNo + PacketType + Data...
	nLength = SWAP32(nLength);

	CByteStream Stream;
	// Header
	Stream.Write(&cPacketID, sizeof(BYTE));
	Stream.Write(&nLength, sizeof(UINT));
	// Data
	Stream.Write(&nPacketNo, sizeof(UINT));
	Stream.Write(&cPacketType, sizeof(BYTE));
	Stream.Write(&nPage, sizeof(int));
	Stream.Write(&nRow, sizeof(int));

	AddSendData(Stream.GetBuffer(), Stream.Size());
}

void CDNBillingConnectionKR::SendHiddenProductInquiry(int nPageNo, int nRowPerPage)
{
	UINT nPacketNo = 0;
	nPacketNo = SWAP32(nPacketNo);
	BYTE cPacketType = CASH_HIDDEN_PRODUCT_INQUIRY;
	int nPage = SWAP32(nPageNo);
	int nRow = SWAP32(nRowPerPage);

	// Header
	BYTE cPacketID = 0xAF;
	UINT nLength = sizeof(UINT) + sizeof(BYTE) + (sizeof(int) * 2);		// length: PacketNo + PacketType + Data...
	nLength = SWAP32(nLength);

	CByteStream Stream;
	// Header
	Stream.Write(&cPacketID, sizeof(BYTE));
	Stream.Write(&nLength, sizeof(UINT));
	// Data
	Stream.Write(&nPacketNo, sizeof(UINT));
	Stream.Write(&cPacketType, sizeof(BYTE));
	Stream.Write(&nPage, sizeof(int));
	Stream.Write(&nRow, sizeof(int));

	AddSendData(Stream.GetBuffer(), Stream.Size());	
}

void CDNBillingConnectionKR::SendCheckBalance(CDNUserRepository *pUser)
{
	UINT nPacketNo = pUser->GetAccountDBID();
	nPacketNo = SWAP32(nPacketNo);
	BYTE cPacketType = CASH_CHECK_BALANCE;
	USHORT wNameLen = (USHORT)strlen(pUser->GetAccountName());
	USHORT wSwapLen = SWAP16(wNameLen);

	// Header
	BYTE cPacketID = 0xAF;
	UINT nLength = sizeof(UINT) + sizeof(BYTE) + sizeof(USHORT) + wNameLen;		// length: PacketNo + PacketType + Data...
	nLength = SWAP32(nLength);

	CByteStream Stream;
	// Header
	Stream.Write(&cPacketID, sizeof(BYTE));
	Stream.Write(&nLength, sizeof(UINT));
	// Data
	Stream.Write(&nPacketNo, sizeof(UINT));
	Stream.Write(&cPacketType, sizeof(BYTE));
	Stream.Write(&wSwapLen, sizeof(USHORT));
	Stream.Write(pUser->GetAccountName(), wNameLen);

	AddSendData(Stream.GetBuffer(), Stream.Size());
}

void CDNBillingConnectionKR::SendCheckBalanceWithNotRefundable(CDNUserRepository *pUser)
{
	UINT nPacketNo = pUser->GetAccountDBID();
	nPacketNo = SWAP32(nPacketNo);
	BYTE cPacketType = CASH_CHECK_BALANCE_WITH_NOT_REFUNDABLE;
	USHORT wNameLen = (USHORT)strlen(pUser->GetAccountName());
	USHORT wSwapLen = SWAP16(wNameLen);

	// Header
	BYTE cPacketID = 0xAF;
	UINT nLength = sizeof(UINT) + sizeof(BYTE) + sizeof(USHORT) + wNameLen;		// length: PacketNo + PacketType + Data...
	nLength = SWAP32(nLength);

	CByteStream Stream;
	// Header
	Stream.Write(&cPacketID, sizeof(BYTE));
	Stream.Write(&nLength, sizeof(UINT));
	// Data
	Stream.Write(&nPacketNo, sizeof(UINT));
	Stream.Write(&cPacketType, sizeof(BYTE));
	Stream.Write(&wSwapLen, sizeof(USHORT));
	Stream.Write(pUser->GetAccountName(), wNameLen);

	AddSendData(Stream.GetBuffer(), Stream.Size());
}

void CDNBillingConnectionKR::SendPurchaseItem(CDNUserRepository *pUser, UINT nPaymentType)
{
	UINT nPacketNo = pUser->GetAccountDBID();
	nPacketNo = SWAP32(nPacketNo);
	BYTE cPacketType = CASH_PURCHASE_ITEM;
	char cReason = 1;	// GameClient;
	if (pUser->IsPCBang()) cReason = 3;
	USHORT wAccountLen = (USHORT)strlen(pUser->GetAccountName());
	USHORT wCharLen = (USHORT)strlen(pUser->GetCharacterName());
	USHORT wSwapALen = SWAP16(wAccountLen);
	USHORT wSwapCLen = SWAP16(wCharLen);
	UINT nSwapNexonSN = SWAP32(pUser->GetNexonSN());

	char szDate[256] = {0, }, szOrderID[256] = {0, };
	tm *pToday;
	time_t raw;
	time(&raw);
	pToday = localtime(&raw);
	strftime(szDate, 256, "%Y%m%d%H%M%S", pToday);
	sprintf_s(szOrderID, "%s%I64d", szDate, pUser->GetPurchaseOrderID());
	USHORT wOrderLen = (USHORT)strlen(szOrderID);
	USHORT wSwapOLen = SWAP16(wOrderLen);

	UINT nSwapPaymentType = SWAP32(nPaymentType);
	UINT nSwapTotalAmount = SWAP32(pUser->m_nTotalPrice);

	std::map<int, short> MapProductList;
	MapProductList.clear();

	for (int i = 0; i < (int)pUser->m_VecBuyItemList.size(); i++){
		MapProductList[pUser->m_VecBuyItemList[i].nProductNo] += 1;
	}

	char cProductCount = (char)MapProductList.size();

	// Header
	BYTE cPacketID = 0xAF;
	// No + Type + 
	UINT nLength = sizeof(UINT) + sizeof(BYTE) + sizeof(UINT) + sizeof(char) + sizeof(USHORT) + wCharLen + (sizeof(USHORT) * 2) + (wAccountLen * 2) + sizeof(UINT) + sizeof(char) + 
		sizeof(USHORT) + wOrderLen + sizeof(UINT) + sizeof(UINT) + sizeof(char) + ((sizeof(int) + sizeof(short)) * cProductCount);

	nLength = SWAP32(nLength);

	CByteStream Stream;
	// Header
	Stream.Write(&cPacketID, sizeof(BYTE));
	Stream.Write(&nLength, sizeof(UINT));
	// Data 
	Stream.Write(&nPacketNo, sizeof(UINT));		// PacketNo
	Stream.Write(&cPacketType, sizeof(BYTE));	// PacketType
	Stream.Write(&pUser->m_nIp, sizeof(UINT));	// Remote IP
	Stream.Write(&cReason, sizeof(char));		// Reason
	Stream.Write(&wSwapCLen, sizeof(USHORT));
	Stream.Write(pUser->GetCharacterName(), wCharLen);			// Game ID
	Stream.Write(&wSwapALen, sizeof(USHORT));
	Stream.Write(pUser->GetAccountName(), wAccountLen);	// User ID
	Stream.Write(&nSwapNexonSN, sizeof(UINT));	// User OID
	Stream.Write(&wSwapALen, sizeof(USHORT));
	Stream.Write(pUser->GetAccountName(), wAccountLen);	// User Name (UserID와 동일하게)
	Stream.Write(&pUser->m_cAge, sizeof(char));	// User Age
	Stream.Write(&wSwapOLen, sizeof(USHORT));
	Stream.Write(szOrderID, wOrderLen);			// Order ID
	Stream.Write(&nSwapPaymentType, sizeof(UINT));	// Payment Type
	Stream.Write(&nSwapTotalAmount, sizeof(UINT));	// Total Amount

	Stream.Write(&cProductCount, sizeof(char));
	for (std::map<int, short>::iterator iter = MapProductList.begin(); iter != MapProductList.end(); ++iter){
		int nSwapProductNo = SWAP32(iter->first);
		short wSwapItemCount = SWAP16(iter->second);

		Stream.Write(&nSwapProductNo, sizeof(int));		// product no
		Stream.Write(&wSwapItemCount, sizeof(short));	// order quantity
	}

	AddSendData(Stream.GetBuffer(), Stream.Size());
}

void CDNBillingConnectionKR::SendPurchaseItemDiscount(CDNUserRepository *pUser, int nProductNo, int nProductPrice, UINT nPaymentType)
{
	UINT nPacketNo = pUser->GetAccountDBID();
	nPacketNo = SWAP32(nPacketNo);
	BYTE cPacketType = CASH_PURCHASE_ITEM_DISCOUNT;
	char cReason = 1;	// GameClient;
	if (pUser->IsPCBang()) cReason = 3;
	USHORT wAccountLen = (USHORT)strlen(pUser->GetAccountName());
	USHORT wCharLen = (USHORT)strlen(pUser->GetCharacterName());
	USHORT wSwapALen = SWAP16(wAccountLen);
	USHORT wSwapCLen = SWAP16(wCharLen);
	UINT nSwapNexonSN = SWAP32(pUser->GetNexonSN());

	char szDate[256] = {0, }, szOrderID[256] = {0, };
	tm *pToday;
	time_t raw;
	time(&raw);
	pToday = localtime(&raw);
	strftime(szDate, 256, "%Y%m%d%H%M%S", pToday);
	sprintf_s(szOrderID, "%s%I64d", szDate, pUser->GetPurchaseOrderID());
	USHORT wOrderLen = (USHORT)strlen(szOrderID);
	USHORT wSwapOLen = SWAP16(wOrderLen);

	UINT nSwapPaymentType = SWAP32(nPaymentType);

	// Header
	BYTE cPacketID = 0xAF;
	// No + Type + 
	UINT nLength = sizeof(UINT) + sizeof(BYTE) + sizeof(UINT) + sizeof(char) + sizeof(USHORT) + wCharLen + (sizeof(USHORT) * 2) + (wAccountLen * 2) + sizeof(UINT) + sizeof(char) + 
		sizeof(USHORT) + wOrderLen + sizeof(UINT) + sizeof(int) + sizeof(int);

	nLength = SWAP32(nLength);

	CByteStream Stream;
	// Header
	Stream.Write(&cPacketID, sizeof(BYTE));
	Stream.Write(&nLength, sizeof(UINT));
	// Data 
	Stream.Write(&nPacketNo, sizeof(UINT));		// PacketNo
	Stream.Write(&cPacketType, sizeof(BYTE));	// PacketType
	Stream.Write(&pUser->m_nIp, sizeof(UINT));	// Remote IP
	Stream.Write(&cReason, sizeof(char));		// Reason
	Stream.Write(&wSwapCLen, sizeof(USHORT));
	Stream.Write(pUser->GetCharacterName(), wCharLen);			// Game ID
	Stream.Write(&wSwapALen, sizeof(USHORT));
	Stream.Write(pUser->GetAccountName(), wAccountLen);	// User ID
	Stream.Write(&nSwapNexonSN, sizeof(UINT));	// User OID
	Stream.Write(&wSwapALen, sizeof(USHORT));
	Stream.Write(pUser->GetAccountName(), wAccountLen);	// User Name (UserID와 동일하게)
	Stream.Write(&pUser->m_cAge, sizeof(char));	// User Age
	Stream.Write(&wSwapOLen, sizeof(USHORT));
	Stream.Write(szOrderID, wOrderLen);			// Order ID
	Stream.Write(&nSwapPaymentType, sizeof(UINT));	// Payment Type

	int nSwapProductNo = SWAP32(nProductNo);
	int nSwapProductPrice = SWAP32(nProductPrice);

	Stream.Write(&nSwapProductNo, sizeof(int));		// product no
	Stream.Write(&nSwapProductPrice, sizeof(int));		// product price (discount)

	AddSendData(Stream.GetBuffer(), Stream.Size());
}

void CDNBillingConnectionKR::SendPurchaseItemExtend(CDNUserRepository *pUser, UINT nPaymentType)
{
	UINT nPacketNo = pUser->GetAccountDBID();
	nPacketNo = SWAP32(nPacketNo);
	BYTE cPacketType = CASH_PURCHASE_ITEM_EXTEND;
	char cReason = 1;	// GameClient;
	if (pUser->IsPCBang()) cReason = 3;
	USHORT wAccountLen = (USHORT)strlen(pUser->GetAccountName());
	USHORT wCharLen = (USHORT)strlen(pUser->GetCharacterName());
	USHORT wSwapALen = SWAP16(wAccountLen);
	USHORT wSwapCLen = SWAP16(wCharLen);
	UINT nSwapNexonSN = SWAP32(pUser->GetNexonSN());

	char szDate[256] = {0, }, szOrderID[256] = {0, };
	tm *pToday;
	time_t raw;
	time(&raw);
	pToday = localtime(&raw);
	strftime(szDate, 256, "%Y%m%d%H%M%S", pToday);
	sprintf_s(szOrderID, "%s%I64d", szDate, pUser->GetPurchaseOrderID());
	USHORT wOrderLen = (USHORT)strlen(szOrderID);
	USHORT wSwapOLen = SWAP16(wOrderLen);

	UINT nSwapPaymentType = SWAP32(nPaymentType);
	UINT nSwapTotalAmount = SWAP32(pUser->m_nTotalPrice);

	std::map<int, short> MapProductList;
	MapProductList.clear();

	for (int i = 0; i < (int)pUser->m_VecBuyItemList.size(); i++){
		MapProductList[pUser->m_VecBuyItemList[i].nProductNo] += 1;
	}

	char cProductCount = (char)MapProductList.size();
	BYTE cPaymentRuleID = pUser->GetPaymentRules();

	// Header
	BYTE cPacketID = 0xAF;
	// No + Type + 
	UINT nLength = sizeof(UINT) + sizeof(BYTE) + sizeof(UINT) + sizeof(char) + sizeof(USHORT) + wCharLen + (sizeof(USHORT) * 2) + (wAccountLen * 2) + sizeof(UINT) + sizeof(char) + 
		sizeof(USHORT) + wOrderLen + sizeof(UINT) + sizeof(BYTE) + sizeof(UINT) + sizeof(char) + ((sizeof(int) + sizeof(short)) * cProductCount);

	nLength = SWAP32(nLength);

	CByteStream Stream;
	// Header
	Stream.Write(&cPacketID, sizeof(BYTE));
	Stream.Write(&nLength, sizeof(UINT));
	// Data 
	Stream.Write(&nPacketNo, sizeof(UINT));		// PacketNo
	Stream.Write(&cPacketType, sizeof(BYTE));	// PacketType
	Stream.Write(&pUser->m_nIp, sizeof(UINT));	// Remote IP
	Stream.Write(&cReason, sizeof(char));		// Reason
	Stream.Write(&wSwapCLen, sizeof(USHORT));
	Stream.Write(pUser->GetCharacterName(), wCharLen);			// Game ID
	Stream.Write(&wSwapALen, sizeof(USHORT));
	Stream.Write(pUser->GetAccountName(), wAccountLen);	// User ID
	Stream.Write(&nSwapNexonSN, sizeof(UINT));	// User OID
	Stream.Write(&wSwapALen, sizeof(USHORT));
	Stream.Write(pUser->GetAccountName(), wAccountLen);	// User Name (UserID와 동일하게)
	Stream.Write(&pUser->m_cAge, sizeof(char));	// User Age
	Stream.Write(&wSwapOLen, sizeof(USHORT));
	Stream.Write(szOrderID, wOrderLen);			// Order ID
	Stream.Write(&nSwapPaymentType, sizeof(UINT));	// Payment Type
	Stream.Write(&cPaymentRuleID, sizeof(BYTE));	// Payment RuleID
	Stream.Write(&nSwapTotalAmount, sizeof(UINT));	// Total Amount

	Stream.Write(&cProductCount, sizeof(char));
	for (std::map<int, short>::iterator iter = MapProductList.begin(); iter != MapProductList.end(); ++iter){
		int nSwapProductNo = SWAP32(iter->first);
		short wSwapItemCount = SWAP16(iter->second);

		Stream.Write(&nSwapProductNo, sizeof(int));		// product no
		Stream.Write(&wSwapItemCount, sizeof(short));	// order quantity
	}

	AddSendData(Stream.GetBuffer(), Stream.Size());
}

#if defined(PRE_ADD_CASH_REFUND)
void CDNBillingConnectionKR::SendRefund(CDNUserRepository *pUser, UINT uiOrderNo, int nProductNo, INT64 biPurchaseOrderDetailID)
{
	UINT nPacketNo = pUser->GetAccountDBID();
	nPacketNo = SWAP32(nPacketNo);
	BYTE cPacketType = CASH_REFUND;	

	char szDate[256] = {0, }, szRequestID[64] = {0, };
	tm *pToday;
	time_t raw;
	time(&raw);
	pToday = localtime(&raw);
	strftime(szDate, 256, "%Y%m%d%H%M%S", pToday);
	sprintf_s(szRequestID, "%s%I64d", szDate, biPurchaseOrderDetailID);
	USHORT wRequestLen = (USHORT)strlen(szRequestID);
	USHORT wSwapRLen = SWAP16(wRequestLen);

	USHORT wCharLen = (USHORT)strlen(pUser->GetCharacterName());	
	USHORT wSwapCLen = SWAP16(wCharLen);

	UINT uiSwapOrderNo = SWAP32(uiOrderNo);	
	int nSwapProductNo = SWAP32(nProductNo);	

	short sQuantity = 1;
	short sSwapQuantity = SWAP16(sQuantity);			// 환불 수량은 무조건 한개

	// Header
	BYTE cPacketID = 0xAF;
	// No + Type + 
	UINT nLength = sizeof(UINT) + sizeof(BYTE) + sizeof(UINT) + sizeof(USHORT) + wRequestLen + sizeof(USHORT) + wCharLen+ sizeof(UINT) + sizeof(int) + sizeof(short);

	nLength = SWAP32(nLength);

	CByteStream Stream;
	// Header
	Stream.Write(&cPacketID, sizeof(BYTE));
	Stream.Write(&nLength, sizeof(UINT));
	// Data 
	Stream.Write(&nPacketNo, sizeof(UINT));		// PacketNo
	Stream.Write(&cPacketType, sizeof(BYTE));	// PacketType
	Stream.Write(&pUser->m_nIp, sizeof(UINT));	// Remote IP	
	
	Stream.Write(&wSwapRLen, sizeof(USHORT));
	Stream.Write(szRequestID, wRequestLen);			// Request ID

	Stream.Write(&wSwapCLen, sizeof(USHORT));
	Stream.Write(pUser->GetCharacterName(), wCharLen);			// Game ID

	Stream.Write(&uiSwapOrderNo, sizeof(UINT));		// order no
	Stream.Write(&nSwapProductNo, sizeof(int));		// product no
	Stream.Write(&sSwapQuantity, sizeof(short));	// quantity	

	AddSendData(Stream.GetBuffer(), Stream.Size());
}
#endif // #if defined(PRE_ADD_CASH_REFUND)

int CDNBillingConnectionKR::SendPurchasePackageItem(CDNUserRepository *pUser, UINT nPaymentType)
{
	UINT nPacketNo = pUser->GetAccountDBID();
	nPacketNo = SWAP32(nPacketNo);
	BYTE cPacketType = CASH_PURCHASE_ITEM;
	char cReason = 1;	// GameClient;
	if (pUser->IsPCBang()) cReason = 3;
	USHORT wAccountLen = (USHORT)strlen(pUser->GetAccountName());
	USHORT wCharLen = (USHORT)strlen(pUser->GetCharacterName());
	USHORT wSwapALen = SWAP16(wAccountLen);
	USHORT wSwapCLen = SWAP16(wCharLen);

	UINT nSwapNexonSN = SWAP32(pUser->GetNexonSN());

	char szDate[256] = {0, }, szOrderID[256] = {0, };
	tm *pToday;
	time_t raw;
	time(&raw);
	pToday = localtime(&raw);
	strftime(szDate, 256, "%Y%m%d%H%M%S", pToday);
	sprintf_s(szOrderID, "%s%I64d", szDate, pUser->GetPurchaseOrderID());
	USHORT wOrderLen = (USHORT)strlen(szOrderID);
	USHORT wSwapOLen = SWAP16(wOrderLen);

	UINT nSwapPaymentType = SWAP32(nPaymentType);
	UINT nSwapTotalAmount = SWAP32(pUser->m_nTotalPrice);

	int nLen = 0;
	int nSwapProductNo = 0;
	short wSwapItemCount = 0;

	TProductInquiry *pProduct = NULL;
	pProduct = GetProductBySN(pUser->GetPackageSN());
	if (!pProduct){
		return ERROR_SHOP_SLOTITEM_NOTFOUND;
	}

	char cPackageCount = 1;
	nSwapProductNo = SWAP32(pProduct->nProductNo);
	short wCount = 1;
	wSwapItemCount = SWAP16(wCount);
	nLen = sizeof(int) + sizeof(short);

	// Header
	BYTE cPacketID = 0xAF;
	// No + Type + 
	UINT nLength = sizeof(UINT) + sizeof(BYTE) + sizeof(UINT) + sizeof(char) + sizeof(USHORT) + wCharLen + (sizeof(USHORT) * 2) + (wAccountLen * 2) + sizeof(UINT) + sizeof(char) + 
		sizeof(USHORT) + wOrderLen + sizeof(UINT) + sizeof(UINT) + sizeof(char) + nLen;

	nLength = SWAP32(nLength);

	CByteStream Stream;
	// Header
	Stream.Write(&cPacketID, sizeof(BYTE));
	Stream.Write(&nLength, sizeof(UINT));
	// Data 
	Stream.Write(&nPacketNo, sizeof(UINT));		// PacketNo
	Stream.Write(&cPacketType, sizeof(BYTE));	// PacketType
	Stream.Write(&pUser->m_nIp, sizeof(UINT));	// Remote IP
	Stream.Write(&cReason, sizeof(char));		// Reason
	Stream.Write(&wSwapCLen, sizeof(USHORT));
	Stream.Write(pUser->GetCharacterName(), wCharLen);			// Game ID
	Stream.Write(&wSwapALen, sizeof(USHORT));
	Stream.Write(pUser->GetAccountName(), wAccountLen);	// User ID
	Stream.Write(&nSwapNexonSN, sizeof(UINT));	// User OID
	Stream.Write(&wSwapALen, sizeof(USHORT));
	Stream.Write(pUser->GetAccountName(), wAccountLen);	// User Name (UserID와 동일하게)
	Stream.Write(&pUser->m_cAge, sizeof(char));	// User Age
	Stream.Write(&wSwapOLen, sizeof(USHORT));
	Stream.Write(szOrderID, wOrderLen);			// Order ID
	Stream.Write(&nSwapPaymentType, sizeof(UINT));	// Payment Type
	Stream.Write(&nSwapTotalAmount, sizeof(UINT));	// Total Amount
	Stream.Write(&cPackageCount, sizeof(char));
	Stream.Write(&nSwapProductNo, sizeof(int));		// product no
	Stream.Write(&wSwapItemCount, sizeof(short));	// order quantity

	AddSendData(Stream.GetBuffer(), Stream.Size());

	return ERROR_NONE;
}

int CDNBillingConnectionKR::SendPurchasePackageItemExtend(CDNUserRepository *pUser, UINT nPaymentType)
{
	UINT nPacketNo = pUser->GetAccountDBID();
	nPacketNo = SWAP32(nPacketNo);
	BYTE cPacketType = CASH_PURCHASE_ITEM_EXTEND;
	char cReason = 1;	// GameClient;
	if (pUser->IsPCBang()) cReason = 3;
	USHORT wAccountLen = (USHORT)strlen(pUser->GetAccountName());
	USHORT wCharLen = (USHORT)strlen(pUser->GetCharacterName());
	USHORT wSwapALen = SWAP16(wAccountLen);
	USHORT wSwapCLen = SWAP16(wCharLen);
	UINT nSwapNexonSN = SWAP32(pUser->GetNexonSN());

	char szDate[256] = {0, }, szOrderID[256] = {0, };
	tm *pToday;
	time_t raw;
	time(&raw);
	pToday = localtime(&raw);
	strftime(szDate, 256, "%Y%m%d%H%M%S", pToday);
	sprintf_s(szOrderID, "%s%I64d", szDate, pUser->GetPurchaseOrderID());
	USHORT wOrderLen = (USHORT)strlen(szOrderID);
	USHORT wSwapOLen = SWAP16(wOrderLen);

	UINT nSwapPaymentType = SWAP32(nPaymentType);
	UINT nSwapTotalAmount = SWAP32(pUser->m_nTotalPrice);

	int nLen = 0;
	int nSwapProductNo = 0;
	short wSwapItemCount = 0;

	TProductInquiry *pProduct = NULL;
	pProduct = GetProductBySN(pUser->GetPackageSN());
	if (!pProduct){
		return ERROR_SHOP_SLOTITEM_NOTFOUND;
	}

	char cPaymentRules = 0;
	cPaymentRules = pUser->GetPaymentRules();

	char cPackageCount = 1;
	nSwapProductNo = SWAP32(pProduct->nProductNo);
	short wCount = 1;
	wSwapItemCount = SWAP16(wCount);
	nLen = sizeof(int) + sizeof(short);

	// Header
	BYTE cPacketID = 0xAF;
	// No + Type + 
	UINT nLength = sizeof(UINT) + sizeof(BYTE) + sizeof(UINT) + sizeof(char) + sizeof(USHORT) + wCharLen + (sizeof(USHORT) * 2) + (wAccountLen * 2) + sizeof(UINT) + sizeof(char) + 
		sizeof(USHORT) + wOrderLen + sizeof(UINT) + sizeof(BYTE) + sizeof(UINT) + sizeof(char) + nLen;

	nLength = SWAP32(nLength);

	CByteStream Stream;
	// Header
	Stream.Write(&cPacketID, sizeof(BYTE));
	Stream.Write(&nLength, sizeof(UINT));
	// Data 
	Stream.Write(&nPacketNo, sizeof(UINT));		// PacketNo
	Stream.Write(&cPacketType, sizeof(BYTE));	// PacketType
	Stream.Write(&pUser->m_nIp, sizeof(UINT));	// Remote IP
	Stream.Write(&cReason, sizeof(char));		// Reason
	Stream.Write(&wSwapCLen, sizeof(USHORT));
	Stream.Write(pUser->GetCharacterName(), wCharLen);			// Game ID
	Stream.Write(&wSwapALen, sizeof(USHORT));
	Stream.Write(pUser->GetAccountName(), wAccountLen);	// User ID
	Stream.Write(&nSwapNexonSN, sizeof(UINT));	// User OID
	Stream.Write(&wSwapALen, sizeof(USHORT));
	Stream.Write(pUser->GetAccountName(), wAccountLen);	// User Name (UserID와 동일하게)
	Stream.Write(&pUser->m_cAge, sizeof(char));	// User Age
	Stream.Write(&wSwapOLen, sizeof(USHORT));
	Stream.Write(szOrderID, wOrderLen);			// Order ID
	Stream.Write(&nSwapPaymentType, sizeof(UINT));	// Payment Type
	Stream.Write(&cPaymentRules, sizeof(BYTE));	// Payment RuleID
	Stream.Write(&nSwapTotalAmount, sizeof(UINT));	// Total Amount
	Stream.Write(&cPackageCount, sizeof(char));
	Stream.Write(&nSwapProductNo, sizeof(int));		// product no
	Stream.Write(&wSwapItemCount, sizeof(short));	// order quantity

	AddSendData(Stream.GetBuffer(), Stream.Size());

	return ERROR_NONE;
}

void CDNBillingConnectionKR::SendPurchaseGift(CDNUserRepository *pUser, UINT nPaymentType)
{
	UINT nPacketNo = pUser->GetAccountDBID();
	nPacketNo = SWAP32(nPacketNo);

	BYTE cPacketType = CASH_PURCHASE_GIFT;

	char cReason = 1;	// GameClient;
	if (pUser->IsPCBang()) cReason = 3;

	char cServerNo = g_Config.nServerNo;

	char szReceiverCharName[64] = {0, }, szMessage[256] = {0, };
	if (pUser->m_wszReceiverCharacterName[0] > 0) WideCharToMultiByte( CP_ACP, 0, pUser->m_wszReceiverCharacterName, -1, szReceiverCharName, 64, NULL, NULL );

	USHORT wSAccountLen = (USHORT)strlen(pUser->GetAccountName());
	USHORT wSCharLen = (USHORT)strlen(pUser->GetCharacterName());
	USHORT wRCharLen = (USHORT)strlen(szReceiverCharName);
	USHORT wMsgLen = 0;	// (USHORT)strlen(szMessage);

	USHORT wSwapSALen = SWAP16(wSAccountLen);
	USHORT wSwapSCLen = SWAP16(wSCharLen);
	USHORT wSwapRCLen = SWAP16(wRCharLen);
	USHORT wSwapMsgLen = SWAP16(wMsgLen);

	UINT nSwapNexonSN = SWAP32(pUser->GetNexonSN());

	char szDate[256] = {0, }, szOrderID[256] = {0, };
	tm *pToday;
	time_t raw;
	time(&raw);
	pToday = localtime(&raw);
	strftime(szDate, 256, "%Y%m%d%H%M%S", pToday);

	INT64 biOrderID = pUser->m_VecGiftItemList[0].biPurchaseOrderID;	// 맨처음꺼 purchaseorderid로 보낸다 (0번을 대표로 하지 뭐)
	sprintf_s(szOrderID, "%s%d", szDate, biOrderID);
	USHORT wOrderLen = (USHORT)strlen(szOrderID);
	USHORT wSwapOLen = SWAP16(wOrderLen);

	UINT nSwapPaymentType = SWAP32(nPaymentType);
	UINT nSwapTotalAmount = SWAP32(pUser->m_nTotalPrice);

	//int nSwapProductNo = SWAP32(nProductNo);
	//short wSwapItemCount = SWAP16(nCount);

	std::map<int, short> MapProductList;
	MapProductList.clear();

	for (int i = 0; i < (int)pUser->m_VecGiftItemList.size(); i++){
		MapProductList[pUser->m_VecGiftItemList[i].nProductNo] += 1;
	}

	char cProductCount = (char)MapProductList.size();

	// Header
	BYTE cPacketID = 0xAF;
	// No + Type + 
	UINT nLength = sizeof(UINT) + sizeof(BYTE) + sizeof(UINT) + sizeof(char) + sizeof(USHORT) + wSCharLen + (sizeof(USHORT) * 2) + (wSAccountLen * 2) + sizeof(UINT) + sizeof(char) +
		sizeof(char) + sizeof(USHORT) + wRCharLen + sizeof(USHORT) + wMsgLen + sizeof(USHORT) + wOrderLen + sizeof(UINT) + sizeof(UINT) + sizeof(char) + ((sizeof(int) + sizeof(short)) * cProductCount);

	nLength = SWAP32(nLength);

	CByteStream Stream;
	// Header
	Stream.Write(&cPacketID, sizeof(BYTE));
	Stream.Write(&nLength, sizeof(UINT));
	// Data
	Stream.Write(&nPacketNo, sizeof(UINT));		// PacketNo
	Stream.Write(&cPacketType, sizeof(BYTE));	// PacketType
	Stream.Write(&pUser->m_nIp, sizeof(UINT));	// Remote IP
	Stream.Write(&cReason, sizeof(char));		// Reason
	Stream.Write(&wSwapSCLen, sizeof(USHORT));
	Stream.Write(pUser->GetCharacterName(), wSCharLen);	// Sender Game ID
	Stream.Write(&wSwapSALen, sizeof(USHORT));
	Stream.Write(pUser->GetAccountName(), wSAccountLen);	// Sender User ID
	Stream.Write(&nSwapNexonSN, sizeof(UINT));			// Sender User OID
	Stream.Write(&wSwapSALen, sizeof(USHORT));
	Stream.Write(pUser->GetAccountName(), wSAccountLen);		// Sender User Name (UserID와 동일하게)
	Stream.Write(&pUser->m_cAge, sizeof(char));			// Sender User Age
	Stream.Write(&cServerNo, sizeof(char));			// Receiver Server No
	Stream.Write(&wSwapRCLen, sizeof(USHORT));
	Stream.Write(szReceiverCharName, wRCharLen);	// Receiver Game ID
	Stream.Write(&wMsgLen, sizeof(USHORT));
	Stream.Write(szMessage, wMsgLen);				// Message
	Stream.Write(&wSwapOLen, sizeof(USHORT));
	Stream.Write(szOrderID, wOrderLen);				// Order ID
	Stream.Write(&nSwapPaymentType, sizeof(UINT));	// Payment Type
	Stream.Write(&nSwapTotalAmount, sizeof(UINT));	// Total Amount
	// char cProductCount = 1;
	Stream.Write(&cProductCount, sizeof(char));

	for (std::map<int, short>::iterator iter = MapProductList.begin(); iter != MapProductList.end(); ++iter){
		int nSwapProductNo = SWAP32(iter->first);
		short wSwapItemCount = SWAP16(iter->second);

		Stream.Write(&nSwapProductNo, sizeof(int));		// product no
		Stream.Write(&wSwapItemCount, sizeof(short));	// order quantity
	}

	AddSendData(Stream.GetBuffer(), Stream.Size());
}

int CDNBillingConnectionKR::SendPurchasePackageGiftItem(CDNUserRepository *pUser, UINT nPaymentType)
{
	UINT nPacketNo = pUser->GetAccountDBID();
	nPacketNo = SWAP32(nPacketNo);

	BYTE cPacketType = CASH_PURCHASE_GIFT;

	char cReason = 1;	// GameClient;
	if (pUser->IsPCBang()) cReason = 3;

	char cServerNo = g_Config.nServerNo;

	char szReceiverCharName[64] = {0, }, szMessage[256] = {0, };
	if (pUser->m_wszReceiverCharacterName[0] > 0) WideCharToMultiByte( CP_ACP, 0, pUser->m_wszReceiverCharacterName, -1, szReceiverCharName, 64, NULL, NULL );
	//if (pCash->wszMessage[0] > 0) WideCharToMultiByte( CP_ACP, 0, pCash->wszMessage, -1, szMessage, 256, NULL, NULL );

	USHORT wSAccountLen = (USHORT)strlen(pUser->GetAccountName());
	USHORT wSCharLen = (USHORT)strlen(pUser->GetCharacterName());
	USHORT wRCharLen = (USHORT)strlen(szReceiverCharName);
	USHORT wMsgLen = 0;	// (USHORT)strlen(szMessage);

	USHORT wSwapSALen = SWAP16(wSAccountLen);
	USHORT wSwapSCLen = SWAP16(wSCharLen);
	USHORT wSwapRCLen = SWAP16(wRCharLen);
	USHORT wSwapMsgLen = SWAP16(wMsgLen);

	UINT nSwapNexonSN = SWAP32(pUser->GetNexonSN());

	char szDate[256] = {0, }, szOrderID[256] = {0, };
	tm *pToday;
	time_t raw;
	time(&raw);
	pToday = localtime(&raw);
	strftime(szDate, 256, "%Y%m%d%H%M%S", pToday);
	sprintf_s(szOrderID, "%s%d", szDate, pUser->GetPurchaseOrderID());
	USHORT wOrderLen = (USHORT)strlen(szOrderID);
	USHORT wSwapOLen = SWAP16(wOrderLen);

	UINT nSwapPaymentType = SWAP32(nPaymentType);
	UINT nSwapTotalAmount = SWAP32(pUser->m_nTotalPrice);

	TProductInquiry *pProduct = NULL;
	pProduct = GetProductBySN(pUser->GetPackageSN());
	if (!pProduct){
		return ERROR_SHOP_SLOTITEM_NOTFOUND;
	}

	char cPackageCount = 1;
	int nSwapProductNo = SWAP32(pProduct->nProductNo);
	short wCount = 1;
	short wSwapItemCount = SWAP16(wCount);

	// Header
	BYTE cPacketID = 0xAF;
	// No + Type + 
	UINT nLength = sizeof(UINT) + sizeof(BYTE) + sizeof(UINT) + sizeof(char) + sizeof(USHORT) + wSCharLen + (sizeof(USHORT) * 2) + (wSAccountLen * 2) + sizeof(UINT) + sizeof(char) +
		sizeof(char) + sizeof(USHORT) + wRCharLen + sizeof(USHORT) + wMsgLen + sizeof(USHORT) + wOrderLen + sizeof(UINT) + sizeof(UINT) + sizeof(char) + sizeof(int) + sizeof(short);

	nLength = SWAP32(nLength);

	CByteStream Stream;
	// Header
	Stream.Write(&cPacketID, sizeof(BYTE));
	Stream.Write(&nLength, sizeof(UINT));
	// Data
	Stream.Write(&nPacketNo, sizeof(UINT));		// PacketNo
	Stream.Write(&cPacketType, sizeof(BYTE));	// PacketType
	Stream.Write(&pUser->m_nIp, sizeof(UINT));	// Remote IP
	Stream.Write(&cReason, sizeof(char));		// Reason
	Stream.Write(&wSwapSCLen, sizeof(USHORT));
	Stream.Write(pUser->GetCharacterName(), wSCharLen);	// Sender Game ID
	Stream.Write(&wSwapSALen, sizeof(USHORT));
	Stream.Write(pUser->GetAccountName(), wSAccountLen);	// Sender User ID
	Stream.Write(&nSwapNexonSN, sizeof(UINT));			// Sender User OID
	Stream.Write(&wSwapSALen, sizeof(USHORT));
	Stream.Write(pUser->GetAccountName(), wSAccountLen);		// Sender User Name (UserID와 동일하게)
	Stream.Write(&pUser->m_cAge, sizeof(char));			// Sender User Age
	Stream.Write(&cServerNo, sizeof(char));			// Receiver Server No
	Stream.Write(&wSwapRCLen, sizeof(USHORT));
	Stream.Write(szReceiverCharName, wRCharLen);	// Receiver Game ID
	Stream.Write(&wMsgLen, sizeof(USHORT));
	Stream.Write(szMessage, wMsgLen);				// Message
	Stream.Write(&wSwapOLen, sizeof(USHORT));
	Stream.Write(szOrderID, wOrderLen);				// Order ID
	Stream.Write(&nSwapPaymentType, sizeof(UINT));	// Payment Type
	Stream.Write(&nSwapTotalAmount, sizeof(UINT));	// Total Amount
	char cProductCount = 1;
	Stream.Write(&cProductCount, sizeof(char));

	Stream.Write(&nSwapProductNo, sizeof(int));		// product no
	Stream.Write(&wSwapItemCount, sizeof(short));	// order quantity

	AddSendData(Stream.GetBuffer(), Stream.Size());

	return ERROR_NONE;
}

void CDNBillingConnectionKR::SendCouponUsing(CDNUserRepository *pUser)
{
	UINT nPacketNo = pUser->GetAccountDBID();
	nPacketNo = SWAP32(nPacketNo);

	BYTE cPacketType = CASH_COUPON_USING;

	char cReason = 1;	// GameClient;
	if (pUser->IsPCBang()) cReason = 3;

	USHORT wAccountLen = (USHORT)strlen(pUser->GetAccountName());
	USHORT wCharLen = (USHORT)strlen(pUser->GetCharacterName());
	USHORT wCouponLen = (USHORT)strlen(pUser->GetCoupon());

	USHORT wSwapALen = SWAP16(wAccountLen);
	USHORT wSwapCLen = SWAP16(wCharLen);
	USHORT wSwapCouponLen = SWAP16(wCouponLen);

	UINT nSwapNexonSN = SWAP32(pUser->GetNexonSN());
	char IsCafe = 0;
	if (pUser->IsPCBang()) IsCafe = 1;

	// Header
	BYTE cPacketID = 0xAF;
	// No + Type + 
	UINT nLength = sizeof(UINT) + sizeof(BYTE) + sizeof(UINT) + sizeof(char) + sizeof(USHORT) + wCharLen + sizeof(char) + (sizeof(USHORT) * 2) + (wAccountLen * 2) + sizeof(UINT) + sizeof(USHORT) + wCouponLen;
	nLength = SWAP32(nLength);

	CByteStream Stream;
	// Header
	Stream.Write(&cPacketID, sizeof(BYTE));
	Stream.Write(&nLength, sizeof(UINT));
	// Data
	Stream.Write(&nPacketNo, sizeof(UINT));		// PacketNo
	Stream.Write(&cPacketType, sizeof(BYTE));	// PacketType
	Stream.Write(&pUser->m_nIp, sizeof(UINT));	// Remote IP
	Stream.Write(&cReason, sizeof(char));		// Reason
	Stream.Write(&wSwapCLen, sizeof(USHORT));
	Stream.Write(pUser->GetCharacterName(), wCharLen);	// Game ID
	Stream.Write(&IsCafe, sizeof(char));		// IsCafe
	Stream.Write(&wSwapALen, sizeof(USHORT));
	Stream.Write(pUser->GetAccountName(), wAccountLen);	// User ID
	Stream.Write(&nSwapNexonSN, sizeof(UINT));			// User OID
	Stream.Write(&wSwapALen, sizeof(USHORT));
	Stream.Write(pUser->GetAccountName(), wAccountLen);		// User Name (UserID와 동일하게)
	Stream.Write(&wSwapCouponLen, sizeof(USHORT));
	Stream.Write(pUser->GetCoupon(), wCouponLen);				// CouponString

	AddSendData(Stream.GetBuffer(), Stream.Size());
}

void CDNBillingConnectionKR::SendInventoryInquiryOrder(UINT nAccountDBID, WCHAR *pCharName)
{
	UINT nPacketNo = nAccountDBID;
	nPacketNo = SWAP32(nPacketNo);
	BYTE cPacketType = CASH_INVENTORY_INQUIRY_ORDER;

	char szCharName[64] = {0, };
	if (pCharName) WideCharToMultiByte( CP_ACP, 0, pCharName, -1, szCharName, 64, NULL, NULL );
	USHORT wCharLen = (USHORT)strlen(szCharName);
	USHORT wSwapCLen = SWAP16(wCharLen);

	char cShowInventory = 10;
	int nPageIndex = 1;
	nPageIndex = SWAP32(nPageIndex);
	int nRowPerPage = 100;
	nRowPerPage = SWAP32(nRowPerPage);

	// Header
	BYTE cPacketID = 0xAF;
	UINT nLength = sizeof(UINT) + sizeof(BYTE) + wCharLen + sizeof(USHORT) + sizeof(char) + sizeof(int) + sizeof(int);
	nLength = SWAP32(nLength);

	CByteStream Stream;
	// Header
	Stream.Write(&cPacketID, sizeof(BYTE));
	Stream.Write(&nLength, sizeof(UINT));
	// Data 
	Stream.Write(&nPacketNo, sizeof(UINT));		// PacketNo
	Stream.Write(&cPacketType, sizeof(BYTE));	// PacketType
	Stream.Write(&wSwapCLen, sizeof(USHORT));
	Stream.Write(szCharName, wCharLen);			// Game ID
	Stream.Write(&cShowInventory, sizeof(char));	// show inven
	Stream.Write(&nPageIndex, sizeof(int));		// page index
	Stream.Write(&nRowPerPage, sizeof(int));		// row per page

	AddSendData(Stream.GetBuffer(), Stream.Size());
}

void CDNBillingConnectionKR::SendInventoryPickup(UINT nAccountDBID, int nOrderNo, int nProductNo, short wOrderQuantity)
{
	UINT nPacketNo = nAccountDBID;
	nPacketNo = SWAP32(nPacketNo);
	BYTE cPacketType = CASH_INVENTORY_PICKUP;
	UINT nSwapOrderNo = SWAP32(nOrderNo);
	UINT nSwapProductNo = SWAP32(nProductNo);
	short wSwapItemCount = SWAP16(wOrderQuantity);

	char szExt[100] = {0, };
	USHORT wExtLen = (USHORT)strlen(szExt);
	USHORT wSwapExtLen = SWAP16(wExtLen);

	// Header
	BYTE cPacketID = 0xAF;
	UINT nLength = sizeof(UINT) + sizeof(BYTE) + sizeof(UINT) + sizeof(UINT) + sizeof(short) + sizeof(USHORT) + wExtLen;	// (sizeof(UINT) * 3) + sizeof(BYTE) + sizeof(short);		// length: PacketNo + PacketType + Data...
	nLength = SWAP32(nLength);

	CByteStream Stream;
	// Header
	Stream.Write(&cPacketID, sizeof(BYTE));
	Stream.Write(&nLength, sizeof(UINT));
	// Data
	Stream.Write(&nPacketNo, sizeof(UINT));
	Stream.Write(&cPacketType, sizeof(BYTE));
	Stream.Write(&nSwapOrderNo, sizeof(UINT));
	Stream.Write(&nSwapProductNo, sizeof(UINT));
	Stream.Write(&wSwapItemCount, sizeof(short));	// show inventory
	Stream.Write(&wSwapExtLen, sizeof(USHORT));
	Stream.Write(szExt, wExtLen);			// Game ID

	AddSendData(Stream.GetBuffer(), Stream.Size());
}

void CDNBillingConnectionKR::SendInventoryCheck(UINT nAccountDBID, WCHAR *pCharName, BYTE IsPresent)
{
	UINT nPacketNo = nAccountDBID;
	nPacketNo = SWAP32(nPacketNo);
	BYTE cPacketType = CASH_INVENTORY_CHECK;
	char szCharName[64] = {0, };
	if (pCharName) WideCharToMultiByte( CP_ACP, 0, pCharName, -1, szCharName, 64, NULL, NULL );
	USHORT wCharLen = (USHORT)strlen(szCharName);
	USHORT wSwapCLen = SWAP16(wCharLen);
	BYTE cShowInventory = 10;	// 사용자에게 노출 (true)

	// Header
	BYTE cPacketID = 0xAF;
	UINT nLength = sizeof(UINT) + (sizeof(BYTE) * 3) + wCharLen + sizeof(USHORT);		// length: PacketNo + PacketType + Data...
	nLength = SWAP32(nLength);

	CByteStream Stream;
	// Header
	Stream.Write(&cPacketID, sizeof(BYTE));
	Stream.Write(&nLength, sizeof(UINT));
	// Data
	Stream.Write(&nPacketNo, sizeof(UINT));
	Stream.Write(&cPacketType, sizeof(BYTE));
	Stream.Write(&wSwapCLen, sizeof(USHORT));
	Stream.Write(szCharName, wCharLen);				// Game ID
	Stream.Write(&cShowInventory, sizeof(BYTE));	// show inventory
	Stream.Write(&IsPresent, sizeof(BYTE));			// IsPresent

	AddSendData(Stream.GetBuffer(), Stream.Size());
}

/*
void CDNBillingConnectionKR::SendInventoryPickupOnce(UINT nAccountDBID, UINT nOrderNo, UINT nItemSN, short wItemCount)
{
	UINT nPacketNo = nAccountDBID;
	nPacketNo = SWAP32(nPacketNo);
	BYTE cPacketType = CASH_INVENTORY_PICKUP_ONCE;
	UINT nSwapOrderNo = SWAP32(nOrderNo);
	UINT nSwapItemSN = SWAP32(nItemSN);
	short wSwapItemCount = SWAP16(wItemCount);

	// Header
	BYTE cPacketID = 0xAF;
	UINT nLength = (sizeof(UINT) * 3) + sizeof(BYTE) + sizeof(short);		// length: PacketNo + PacketType + Data...
	nLength = SWAP32(nLength);

	CByteStream Stream;
	// Header
	Stream.Write(&cPacketID, sizeof(BYTE));
	Stream.Write(&nLength, sizeof(UINT));
	// Data
	Stream.Write(&nPacketNo, sizeof(UINT));
	Stream.Write(&cPacketType, sizeof(BYTE));
	Stream.Write(&nSwapOrderNo, sizeof(UINT));
	Stream.Write(&nSwapItemSN, sizeof(UINT));
	Stream.Write(&wSwapItemCount, sizeof(short));	// show inventory

	AddSendData(Stream.GetBuffer(), Stream.Size());
}

void CDNBillingConnectionKR::SendInventoryPickupRollback(UINT nAccountDBID, UINT nOrderNo, UINT nProductNo)
{
	UINT nPacketNo = nAccountDBID;
	nPacketNo = SWAP32(nPacketNo);
	BYTE cPacketType = CASH_INVENTORY_PICKUP_ROLLBACK;
	UINT nSwapOrderNo = SWAP32(nOrderNo);
	UINT nSwapProductNo = SWAP32(nProductNo);

	// Header
	BYTE cPacketID = 0xAF;
	UINT nLength = (sizeof(UINT) * 3) + sizeof(BYTE);		// length: PacketNo + PacketType + Data...
	nLength = SWAP32(nLength);

	CByteStream Stream;
	// Header
	Stream.Write(&cPacketID, sizeof(BYTE));
	Stream.Write(&nLength, sizeof(UINT));
	// Data
	Stream.Write(&nPacketNo, sizeof(UINT));
	Stream.Write(&cPacketType, sizeof(BYTE));
	Stream.Write(&nSwapOrderNo, sizeof(UINT));
	Stream.Write(&nSwapProductNo, sizeof(UINT));

	AddSendData(Stream.GetBuffer(), Stream.Size());
}
*/
#endif