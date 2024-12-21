#include "StdAfx.h"
#include "DNUserRepository.h"
#include "DNConnection.h"
#include "DNExtManager.h"
#include "DNSQLMembership.h"
#include "DNSQLWorld.h"
#include "DNSQLManager.h"
#include "Log.h"
#include "Util.h"
#ifdef PRE_ADD_LIMITED_CASHITEM
#include "DNLimitedCashItemRepository.h"
#endif		//#ifdef PRE_ADD_LIMITED_CASHITEM

#if defined(_WORK)
#include "DNBillingConnection.h"
#elif defined(_KR) || defined(_US)
#include "DNBillingConnectionKR.h"
#elif defined(_KRAZ)
#include "DNBillingConnectionKRAZ.h"
#elif defined(_JP)
#include "DNBillingConnectionJP.h"
#elif defined(_CH) || defined(_EU)
#include "DNBillingConnectionCH.h"
#elif defined(_TW)
#include "DNBillingConnectionTW.h"
#include "TimeSet.h"
#elif defined(_SG)
#include "DNBillingConnectionSG.h"
#elif defined(_TH)
#include "DNBillingConnectionTH.h"
#elif defined(_ID)
#include "DNBillingConnectionID.h"
#elif defined(_RU)  || defined(_KAMO)  //[OK_Cash] 
#include "DNBillingConnectionRU.h"
#endif	// _KR, _JP, _CH

#if defined (PRE_ADD_REJECT_DUPLICATED_CASH_REQUEST)
void ReleaseExchange(USER_REQUEST volatile* pDestination)
{
#if defined (_WIN64)
	InterlockedExchange64(pDestination, CASH_NO_REQUESTING);
#else
	InterlockedExchange(pDestination, CASH_NO_REQUESTING);
#endif // #if defined (_WIN64)
}
#endif // #if defined (PRE_ADD_REJECT_DUPLICATED_CASH_REQUEST)

CDNUserRepository::CDNUserRepository(void)
{
	m_nNexonSN = 0;
	m_cPaymentRules = 0;

	m_nAccountDBID = 0;
	memset(&m_szAccountName, 0, sizeof(m_szAccountName));
	memset(&m_wszAccountName, 0, sizeof(m_wszAccountName));
	memset(&m_szCharacterName, 0, sizeof(m_szCharacterName));
	memset(&m_wszCharacterName, 0, sizeof(m_wszCharacterName));
	m_biCharacterDBID = 0;

#if defined(_CH) || defined(_EU)
	m_sess_id[0] = '\0';
#endif	// #if defined(_CH) || defined(_EU)
	memset(&m_szReceiverAccountName, 0, sizeof(m_szReceiverAccountName));

	m_nIp = 0;
	memset(&m_szIp, 0, sizeof(m_szIp));
	memset(&m_wszIp, 0, sizeof(m_wszIp));

#if defined (PRE_ADD_REJECT_DUPLICATED_CASH_REQUEST)
	m_Requesting = CASH_NO_REQUESTING;
#endif // #if defined (PRE_ADD_REJECT_DUPLICATED_CASH_REQUEST)

	Clear();
}

CDNUserRepository::~CDNUserRepository(void)
{
	Clear();
}

void CDNUserRepository::Clear()
{
	m_pConnection = NULL;
	m_nThreadID = 0;

	m_cWorldSetID = 0;
	m_nMapID = 0;
	m_nChannelID = 0;

	m_bOpen = false;	
#if defined( PRE_ADD_NEW_MONEY_SEED )
	m_nSeedPoint = 0;
#endif
	m_cAge = 0;
	m_bPCBang = false;
	m_cBuyCartType = m_cGiftCartType = 0;		// 구매 카트 타입
	m_nPackageSN = 0;

	m_biPurchaseOrderID = 0;
	m_nPickupCount = 0;

#if defined(_CH) || defined(_EU)
	m_context_id[0] = '\0';
	m_nOrderStep = SNDAORDERSTEP_NONE;

	m_nPartCount = 0;
	m_VecBuyItemPartList.clear();	// 중국은 5개씩 묶어서 보내야해서 하나 더 만든다. (머 이러냐..)
	m_VecGiftItemPartList.clear();
#endif	// #if defined(_CH) || defined(_EU)

	m_nTotalPrice = 0;
	m_nUsedPaidCash = 0;

	m_nSubCommand = 0;
	m_cProductCount = 0;
	memset(&m_ProductPackageList, 0, sizeof(m_ProductPackageList));

	m_VecBuyItemList.clear();

	memset(&m_GiftPackageList, 0, sizeof(m_GiftPackageList));
	m_VecGiftItemList.clear();

	m_biCouponOrderID = 0;
	m_nCouponSN = 0;

	memset(&m_wszReceiverCharacterName, 0, sizeof(m_wszReceiverCharacterName));
	memset(&m_wszMessage, 0, sizeof(m_wszMessage));

	memset(&m_wszCoupon, 0, sizeof(m_wszCoupon));
	memset(&m_szCoupon, 0, sizeof(m_szCoupon));

	memset(&m_VIP, 0, sizeof(TVIPItem));
	m_bAutoPay = false;
	m_bServer = false;

#if defined(PRE_ADD_CASH_REFUND)
	m_biPurchaseOrderDetailID = 0;
	memset(&m_PaymentItem, 0, sizeof(m_PaymentItem));	
	memset(&m_PaymentPackageItem, 0, sizeof(TPaymentPackageItemInfoEx));
	m_bCashMoveInven = false;
	m_cRefundType = 0;
#endif

#if defined(PRE_ADD_SALE_COUPON)
	m_biSaleCouponSerial = 0;
#endif
#if defined(PRE_ADD_CADGE_CASH)
	m_nMailDBID = 0;
#endif	// #if defined(PRE_ADD_CADGE_CASH)

	m_cCurProductCount = 0;
}

void CDNUserRepository::SetReceiverAccountName()
{
	CDNSQLWorld *pWorldDB = GetWorldDB();
	if (!pWorldDB) return;

	char szTempAccountName[IDLENMAX];
	int nRet = pWorldDB->QueryGetCharacterPartialy10(m_wszReceiverCharacterName, szTempAccountName);
	if (nRet != ERROR_NONE) return;

	_strcpy(m_szReceiverAccountName, IDLENMAX, szTempAccountName, IDLENMAX);
}

#if defined(_CH) || defined(_EU)
CDNUserRepository::TBuyItemPart *CDNUserRepository::GetBuyItemPart()
{
	if (m_VecBuyItemPartList.empty()) return NULL;
	if (m_nPartCount >= (int)m_VecBuyItemPartList.size()) return NULL;

	return &(m_VecBuyItemPartList[m_nPartCount]);
}

CDNUserRepository::TGiftItemPart *CDNUserRepository::GetGiftItemPart()
{
	if (m_VecGiftItemPartList.empty()) return NULL;
	if (m_nPartCount >= (int)m_VecGiftItemPartList.size()) return NULL;

	return &(m_VecGiftItemPartList[m_nPartCount]);
}

int CDNUserRepository::CheckSessID()
{
	if ('\0' == GetSessId()[0]) {
		int nRetVal = GetNewId(SetSessId());
		if (0 > nRetVal)
			return ERROR_GENERIC_USER_NOT_FOUND;
	}

	return ERROR_NONE;
}

#endif	// #if defined(_CH) || defined(_EU)

void CDNUserRepository::SetPurchaseOrderDetailID(const char* pPurchaseOrderDetailIDs)
{
	switch (m_nSubCommand)
	{
	case CASH_BUY:
		{
			if (m_VecBuyItemList.empty())
				return;

			// 파싱
			string strOrderDetails = pPurchaseOrderDetailIDs;
			
			std::vector<std::string> tokens;
			TokenizeA(strOrderDetails.c_str(), tokens, ",");

			std::vector<std::string>::iterator ii;
			int nCount = 0;
			for (ii = tokens.begin(); ii != tokens.end(); ii++)
			{
				m_VecBuyItemList[nCount].biPurchaseOrderDetailID = _atoi64((*ii).c_str());
				++nCount;		
			}
		}
		break;

	case CASH_PACKAGEBUY:
		{
#if defined(PRE_ADD_CASH_REFUND)
			m_biPurchaseOrderDetailID = _atoi64(pPurchaseOrderDetailIDs);
#endif
		}
		break;
	}
}

int CDNUserRepository::GetPetalBalance()
{
	CDNSQLMembership *pMembershipDB = GetMembershipDB();
	if (!pMembershipDB) return 0;

	int nPetalBalance = 0;
	pMembershipDB->QueryGetPetalBalance(m_nAccountDBID, nPetalBalance);

	return nPetalBalance;
}

CDNSQLMembership *CDNUserRepository::GetMembershipDB()
{
	CDNSQLMembership *pMembershipDB = g_pSQLManager->FindMembershipDB(m_nThreadID);
	if (!pMembershipDB)
		g_Log.Log(LogType::_ERROR, this, L"[%d] pMembershipDB not found\r\n", m_nSubCommand);

	return pMembershipDB;
}

CDNSQLWorld *CDNUserRepository::GetWorldDB()
{
	CDNSQLWorld *pWorldDB = g_pSQLManager->FindWorldDB(m_nThreadID, m_cWorldSetID);
	if (!pWorldDB)
		g_Log.Log(LogType::_ERROR, this, L"[%d] pWorldDB not found\r\n", m_nSubCommand);

	return pWorldDB;
}

bool CDNUserRepository::SetBuyItem(TCashBuyItem &Item)
{
	TBuyItem BuyItem;
	memset(&BuyItem, 0, sizeof(TBuyItem));

	BuyItem.BuyItem = Item;
	BuyItem.nReservePetal = g_pExtManager->GetCashCommodityReserve(Item.nItemSN);
	BuyItem.nPrice = g_pExtManager->GetCashCommodityPrice(Item.nItemSN);
	BuyItem.nLimitCount = g_pExtManager->GetCashBuyAbleCount(Item.nItemSN);
	BuyItem.BuyItem.bFail = true;	// default를 fail로 설정... 제대로 가졌을때만 true로 한다.

#if defined( PRE_ADD_NEW_MONEY_SEED )
	BuyItem.nSeedPoint = g_pExtManager->GetCashCommoditySeed(Item.nItemSN);
#endif

#if defined(_KR) || defined(_US)
	BuyItem.nProductNo = g_pBillingConnection->GetProductNoBySN(Item.nItemSN);
#endif	// _KR _CH
#if defined(PRE_ADD_SALE_COUPON)
	if( BuyItem.nPrice <= 0 && !g_pExtManager->GetCashCommodityUseCoupon(Item.nItemSN))
#else
	if( BuyItem.nPrice <= 0 )
#endif
		return false;
	m_nTotalPrice += BuyItem.nPrice;

	m_VecBuyItemList.push_back(BuyItem);
	return true;
}

void CDNUserRepository::SetBuyItemPartByShanda(int nStartIndex, int nEndIndex, INT64 biOrderID, const char* pPurchaseOrderDetailIDs)
{
#if defined(_CH) || defined(_EU)
	TBuyItemPart Part = {0,};
	// 파싱
	string strOrderDetails = pPurchaseOrderDetailIDs;

	std::vector<std::string> tokens;
	TokenizeA(strOrderDetails.c_str(), tokens, ",");

	std::vector<std::string>::iterator ii;
	int nCount = nStartIndex;
	for (ii = tokens.begin(); ii != tokens.end(); ii++)
	{
		if( nCount <= nEndIndex )
		{
			m_VecBuyItemList[nCount].biPurchaseOrderDetailID = _atoi64((*ii).c_str());
		}
		++nCount;
	}

	nCount = 0;
	for (int i = nStartIndex; i < nEndIndex; i++)
	{
		Part.biPurchaseOrderID = biOrderID;
		Part.pPartItem[nCount] = &m_VecBuyItemList[i];
		Part.nOrderStep = SNDAORDERSTEP_NONE;
		Part.context_id[0] = '\0';
		nCount++;
	}

	m_VecBuyItemPartList.push_back(Part);
#endif	// #if defined(_CH) || defined(_EU)
}

CDNUserRepository::TBuyItem *CDNUserRepository::GetBuyItem(int nIndex)
{
	if (m_VecBuyItemList.empty()) return NULL;
	if (m_cProductCount <= nIndex) return NULL;
	return &(m_VecBuyItemList[nIndex]);
}

bool CDNUserRepository::SetGiftItem(TCashGiftItem &Item)
{
	TGiftItem GiftItem;
	memset(&GiftItem, 0, sizeof(TGiftItem));

	GiftItem.GiftItem = Item;
	GiftItem.nReservePetal = g_pExtManager->GetCashCommodityReserve(Item.nItemSN);
	GiftItem.nPrice = g_pExtManager->GetCashCommodityPrice(Item.nItemSN);
	GiftItem.nLimitCount = g_pExtManager->GetCashBuyAbleCount(Item.nItemSN);
	GiftItem.GiftItem.bFail = true;	// default를 fail로 설정... 제대로 선물 가졌을때만 true로 한다.
#if defined(_KR) || defined(_US)
	GiftItem.nProductNo = g_pBillingConnection->GetProductNoBySN(Item.nItemSN);
#elif defined(_CH) || defined(_EU)
	GiftItem.nOrderStep = SNDAORDERSTEP_NONE;
#endif	// _KR _CH _TW
#if defined( PRE_ADD_NEW_MONEY_SEED )
	GiftItem.nSeedPoint = g_pExtManager->GetCashCommoditySeed(Item.nItemSN);
#endif
	if( GiftItem.nPrice <= 0)
		return false;

	m_nTotalPrice += GiftItem.nPrice;

	m_VecGiftItemList.push_back(GiftItem);
	return true;
}

void CDNUserRepository::SetGiftItemPartByShanda()
{
#if defined(_CH) || defined(_EU)
	int nTotalCount = (int)m_VecGiftItemList.size();
	int nStx = 0, nEtx = std::min<int>(MAX_GOOD_LEN, nTotalCount);
	while(true) {
		TGiftItemPart Part = {0,};
		int nCount = 0;
		for (int i = nStx; i < nEtx; i++){
			Part.pPartItem[nCount] = &m_VecGiftItemList[i];
			Part.nOrderStep = SNDAORDERSTEP_NONE;
			Part.context_id[0] = '\0';
			nCount++;
		}

		m_VecGiftItemPartList.push_back(Part);

		if (nTotalCount <= nEtx) {
			break;
		}

		nStx = nEtx;
		nEtx = std::min<int>(nEtx + MAX_GOOD_LEN, nTotalCount);
	}
#endif	// #if defined(_CH) || defined(_EU)
}

void CDNUserRepository::SetGiftItemPurchaseOrderID(int nIndex, INT64 biPurchaseOrderID)
{
	if (m_VecGiftItemList.empty()) return;
	if (m_cProductCount <= nIndex) return;

	m_VecGiftItemList[nIndex].biPurchaseOrderID = biPurchaseOrderID;
}

CDNUserRepository::TGiftItem *CDNUserRepository::GetGiftItem(int nIndex)
{
	if (m_VecGiftItemList.empty()) return NULL;
	if (m_cProductCount <= nIndex) return NULL;
	return &(m_VecGiftItemList[nIndex]);
}

void CDNUserRepository::SetBalanceInquiry(int nThreadID, CDNConnection *pConnection, TQCashBalanceInquiry *pCash)
{
	Clear();

	m_nSubCommand = CASH_BALANCEINQUIRY;
	m_nThreadID = nThreadID;
	m_pConnection = pConnection;

	m_nAccountDBID = pCash->nAccountDBID;
	_strcpy(m_szAccountName, _countof(m_szAccountName), pCash->szAccountName, (int)strlen(pCash->szAccountName));
	MultiByteToWideChar(CP_ACP, 0, m_szAccountName, -1, m_wszAccountName, IDLENMAX);

	m_bOpen = pCash->bOpen;
#if defined(_KR) || defined(_US)
	m_nNexonSN = pCash->nNexonSN;
#endif	// #if defined(_KR) || defined(_US)

	m_nIp = pCash->nIp;
	_inet_addr(pCash->nIp, m_szIp);
	MultiByteToWideChar(CP_ACP, 0, m_szIp, -1, m_wszIp, IPLENMAX);

	m_bServer = pCash->bServer;
}

bool CDNUserRepository::SetBuy(int nThreadID, CDNConnection *pConnection, TQCashBuy *pCash)
{
	Clear();

	m_nSubCommand = CASH_BUY;
	m_nThreadID = nThreadID;
	m_pConnection = pConnection;
	m_nAccountDBID = pCash->nAccountDBID;
	_strcpy(m_szCharacterName, _countof(m_szCharacterName), pCash->szCharacterName, (int)strlen(pCash->szCharacterName));
	MultiByteToWideChar(CP_ACP, 0, m_szCharacterName, -1, m_wszCharacterName, NAMELENMAX);
	m_biCharacterDBID = pCash->biCharacterDBID;
	m_cWorldSetID = pCash->cWorldSetID;
	m_nMapID = pCash->nMapID;
	m_nChannelID = pCash->nChannelID;	
	m_cAge = pCash->cAge;
	m_bPCBang = pCash->bPCBang;
	m_cBuyCartType = pCash->cCartType;
	m_cProductCount = pCash->cProductCount;
#if defined(PRE_ADD_CASH_REFUND)
	m_bCashMoveInven = pCash->bMoveCashInven;
#endif
	m_cPaymentRules = pCash->cPaymentRules;
#if defined(PRE_ADD_SALE_COUPON)
	m_biSaleCouponSerial = pCash->biSaleCouponSerial;
#endif

	for (int i = 0; i < m_cProductCount; i++)
	{
		if( !SetBuyItem(pCash->BuyList[i]) )
			return false;
	}
	return true;
}

void CDNUserRepository::SetPackageBuy(int nThreadID, CDNConnection *pConnection, TQCashPackageBuy *pCash)
{
	Clear();

	m_nThreadID = nThreadID;
	m_nSubCommand = CASH_PACKAGEBUY;
	m_nAccountDBID = pCash->nAccountDBID;
	_strcpy(m_szCharacterName, _countof(m_szCharacterName), pCash->szCharacterName, (int)strlen(pCash->szCharacterName));
	MultiByteToWideChar(CP_ACP, 0, m_szCharacterName, -1, m_wszCharacterName, NAMELENMAX);
	m_pConnection = pConnection;
	m_biCharacterDBID = pCash->biCharacterDBID;
	m_cWorldSetID = pCash->cWorldSetID;
	m_nMapID = pCash->nMapID;
	m_nChannelID = pCash->nChannelID;	
	m_cProductCount = pCash->cPackageCount;
	memcpy(m_ProductPackageList, pCash->BuyList, sizeof(TCashItemBase) * pCash->cPackageCount);
	m_nPackageSN = pCash->nPackageSN;
	m_nTotalPrice = g_pExtManager->GetCashCommodityPrice(m_nPackageSN);
	m_cPaymentRules = pCash->cPaymentRules;
#if defined(PRE_ADD_CASH_REFUND)
	m_bCashMoveInven = pCash->bMoveCashInven;
#endif	// #if defined(PRE_ADD_CASH_REFUND)
#if defined(PRE_ADD_SALE_COUPON)
	m_biSaleCouponSerial = pCash->biSaleCouponSerial;
#endif
}

bool CDNUserRepository::SetGift(int nThreadID, CDNConnection *pConnection, TQCashGift *pCash)
{
	Clear();

	m_nSubCommand = CASH_GIFT;
	m_nThreadID = nThreadID;
	m_pConnection = pConnection;

	m_nAccountDBID = pCash->nAccountDBID;
	_strcpy(m_szCharacterName, _countof(m_szCharacterName), pCash->szCharacterName, (int)strlen(pCash->szCharacterName));
	MultiByteToWideChar(CP_ACP, 0, m_szCharacterName, -1, m_wszCharacterName, NAMELENMAX);
	m_biCharacterDBID = pCash->biCharacterDBID;
	m_cWorldSetID = pCash->cWorldSetID;
	m_nMapID = pCash->nMapID;
	m_nChannelID = pCash->nChannelID;

	_wcscpy(m_wszReceiverCharacterName, NAMELENMAX, pCash->wszReceiverCharacterName, NAMELENMAX);
	_wcscpy(m_wszMessage, GIFTMESSAGEMAX, pCash->wszMessage, GIFTMESSAGEMAX);

	m_cAge = pCash->cAge;
	m_bPCBang = pCash->bPCBang;
	m_cGiftCartType = pCash->cCartType;
#if defined(PRE_ADD_CADGE_CASH)
	m_nMailDBID = pCash->nMailDBID;
#endif	// #if defined(PRE_ADD_CADGE_CASH)
	m_cProductCount = pCash->cProductCount;

	for (int i = 0; i < m_cProductCount; i++)
	{
		if( !SetGiftItem(pCash->GiftList[i]) )		
			return false;	
	}
	return true;
}

void CDNUserRepository::SetPackageGift(int nThreadID, CDNConnection *pConnection, TQCashPackageGift *pCash)
{
	Clear();

	m_nSubCommand = CASH_PACKAGEGIFT;
	m_nThreadID = nThreadID;
	m_pConnection = pConnection;

	m_nAccountDBID = pCash->nAccountDBID;
	_strcpy(m_szCharacterName, _countof(m_szCharacterName), pCash->szCharacterName, (int)strlen(pCash->szCharacterName));
	MultiByteToWideChar(CP_ACP, 0, m_szCharacterName, -1, m_wszCharacterName, NAMELENMAX);
	m_biCharacterDBID = pCash->biCharacterDBID;
	m_cWorldSetID = pCash->cWorldSetID;
	m_nMapID = pCash->nMapID;
	m_nChannelID = pCash->nChannelID;
	m_cProductCount = pCash->cPackageCount;
	memcpy(m_GiftPackageList, pCash->GiftList, sizeof(TCashPackageGiftItem) * m_cProductCount);
	m_nPackageSN = pCash->nPackageSN;
	m_nTotalPrice = g_pExtManager->GetCashCommodityPrice(m_nPackageSN);

	_wcscpy(m_wszReceiverCharacterName, NAMELENMAX, pCash->wszReceiverCharacterName, NAMELENMAX);
	_wcscpy(m_wszMessage, GIFTMESSAGEMAX, pCash->wszMessage, GIFTMESSAGEMAX);

	m_cAge = pCash->cAge;
	m_bPCBang = pCash->bPCBang;
#if defined(PRE_ADD_CADGE_CASH)
	m_nMailDBID = pCash->nMailDBID;
#endif	// #if defined(PRE_ADD_CADGE_CASH)
}

void CDNUserRepository::SetCoupon(int nThreadID, CDNConnection *pConnection, TQCashCoupon *pCash)
{
	Clear();

	m_nSubCommand = CASH_COUPON;
	m_nThreadID = nThreadID;
	m_pConnection = pConnection;
	m_biCharacterDBID = pCash->biCharacterDBID;
	m_cWorldSetID = pCash->cWorldSetID;
	m_bPCBang = pCash->bPCBang;
	_strcpy(m_szCharacterName, NAMELENMAX, pCash->szCharacterName, NAMELENMAX);
	MultiByteToWideChar(CP_ACP, 0, m_szCharacterName, -1, m_wszCharacterName, NAMELENMAX);
	_wcscpy(m_wszCoupon, COUPONMAX, pCash->wszCoupon, COUPONMAX);
	WideCharToMultiByte(CP_ACP, NULL, pCash->wszCoupon, -1, m_szCoupon, sizeof(m_szCoupon), NULL, NULL);
}

void CDNUserRepository::SetVIPBuy(int nThreadID, CDNConnection *pConnection, TQCashVIPBuy *pCash)
{
	Clear();

	m_nSubCommand = CASH_VIPBUY;
	m_nThreadID = nThreadID;
	m_pConnection = pConnection;
	m_nAccountDBID = pCash->nAccountDBID;
	m_biCharacterDBID = pCash->biCharacterDBID;
	m_cWorldSetID = pCash->cWorldSetID;
	m_nMapID = pCash->nMapID;
	m_nChannelID = pCash->nChannelID;
	m_bPCBang = pCash->bPCBang;
	m_bAutoPay = pCash->bAutoPay;
	m_bServer = pCash->bServer;

	m_VIP.nItemSN = pCash->nItemSN;
	m_VIP.nPrice = g_pExtManager->GetCashCommodityPrice(pCash->nItemSN);
	if (m_bAutoPay)	// 자동결제면 할인된 가격으로 
		m_VIP.nPrice = g_pExtManager->GetCashCommodityVIPAutomaticPaySalePrice(pCash->nItemSN);

	m_VIP.nLimitCount = g_pExtManager->GetCashBuyAbleCount(pCash->nItemSN);
#if defined(_CH) || defined(_EU)
	m_VIP.nOrderStep = SNDAORDERSTEP_NONE;
#endif	// #if defined(_CH) || defined(_EU)
}

void CDNUserRepository::SetVIPGift(int nThreadID, CDNConnection *pConnection, TQCashVIPGift *pCash)
{
	Clear();

	m_nSubCommand = CASH_VIPGIFT;
	m_nThreadID = nThreadID;
	m_pConnection = pConnection;
	m_nAccountDBID = pCash->nAccountDBID;
	m_biCharacterDBID = pCash->biCharacterDBID;
	m_cWorldSetID = pCash->cWorldSetID;
	m_nMapID = pCash->nMapID;
	m_nChannelID = pCash->nChannelID;
	m_bPCBang = pCash->bPCBang;
	_wcscpy(m_wszReceiverCharacterName, NAMELENMAX, pCash->wszReceiverCharacterName, NAMELENMAX);
	_wcscpy(m_wszMessage, GIFTMESSAGEMAX, pCash->wszMessage, GIFTMESSAGEMAX);

	m_VIP.nItemSN = pCash->nItemSN;
	m_VIP.nPrice = g_pExtManager->GetCashCommodityPrice(pCash->nItemSN);
	m_VIP.nLimitCount = g_pExtManager->GetCashBuyAbleCount(pCash->nItemSN);
#if defined(_CH) || defined(_EU)
	m_VIP.nOrderStep = SNDAORDERSTEP_NONE;
#endif	// #if defined(_CH) || defined(_EU)
}

void CDNUserRepository::SetRefund(int nThreadID, CDNConnection *pConnection, TQCashRefund* pCash)
{
	Clear();
	m_nThreadID = nThreadID;
	m_pConnection = pConnection;
	m_nAccountDBID = pCash->nAccountDBID;
	m_cWorldSetID = pCash->cWorldSetID;
#if defined(PRE_ADD_CASH_REFUND)
	m_cRefundType = pCash->cItemType;
	m_biPurchaseOrderDetailID = pCash->biPurchaseOrderDetailID;
#endif	// #if defined(PRE_ADD_CASH_REFUND)
}

int CDNUserRepository::OnBalanceInquiry(int nThreadID, CDNConnection *pConnection, TQCashBalanceInquiry *pCash)
{
	SetBalanceInquiry(nThreadID, pConnection, pCash);
/*
#if defined(_1TW) || defined(_1TH)
	int nRet = g_pBillingPointConnection->OnBalanceInquiry(this);
#else	// defined(_TW)
	int nRet = g_pBillingConnection->OnBalanceInquiry(this);
#endif	// defined(_TW)
*/
#pragma message("TEST CASH BALANCE!")
	int nCashBalance = 0; 
	CDNSQLMembership *pMembershipDB = GetMembershipDB();
	if (!pMembershipDB){
		SendBalanceInquiry(ERROR_GENERIC_INVALIDREQUEST, 0, 0);
		return ERROR_GENERIC_INVALIDREQUEST;
	}

	int nRet = pMembershipDB->QueryGetCashBalance(this->GetAccountDBID(), NULL, nCashBalance);


	if (nRet != ERROR_NONE){
		SendBalanceInquiry(nRet, 0, 0);
		return nRet;
	}else{
		SendBalanceInquiry(nRet,nCashBalance, 0);
	
	}

	return ERROR_NONE;
}

int CDNUserRepository::OnBuy(int nThreadID, CDNConnection *pConnection, TQCashBuy *pCash)
{
	if( !SetBuy(nThreadID, pConnection, pCash) )
	{
		SendBuyGift(ERROR_GENERIC_INVALIDREQUEST);
		return ERROR_GENERIC_INVALIDREQUEST;
	}

	int nAdd = 0;
	for (int i = 0; i < pCash->cProductCount; i++){
		if (!CheckSaleItemSN(pCash->BuyList[i].nItemSN)){
			g_Log.Log(LogType::_ERROR, this, L"[CASH_BUY] CheckSaleItemSN Error (ItemSN:%d)\r\n", pCash->BuyList[i].nItemSN);
			SendBuyGift(ERROR_CASHSHOP_PROHIBITSALE);
			return ERROR_CASHSHOP_PROHIBITSALE;
		}

		if (g_pExtManager->GetItemMainType(pCash->BuyList[i].CashItem.nItemID) == ITEMTYPE_CHARACTER_SLOT){
			nAdd++;
		}
	}

#if defined(PRE_MOD_SELECT_CHAR)
	int nRet = CheckCharacterSlotCount(pCash->nAccountDBID, nAdd);
#else	// #if defined(PRE_MOD_SELECT_CHAR)
	int nRet = CheckCharacterSlotCount(pCash->biCharacterDBID, nAdd);
#endif	// #if defined(PRE_MOD_SELECT_CHAR)
	if (nRet != ERROR_NONE){
		SendBuyGift(nRet);
		return nRet;
	}

	if (pCash->cPaymentRules == Cash::PaymentRules::Petal){		// 페탈로 물건사기
		nRet = PurchaseItemByPetal();  //$_$  질듐뭔찜
	}
#if defined( PRE_ADD_NEW_MONEY_SEED )
	else if( pCash->cPaymentRules == Cash::PaymentRules::Seed )
	{
		//nRet = PurchaseItemBySeed(); //$_$  蘆綾뭔찜
	}
#endif
	else {	// 캐쉬로 물건사기

		int nCashBalance = 0; 

		CDNSQLMembership *pMembershipDB = GetMembershipDB();
		if (!pMembershipDB){
			SendBuyGift(ERROR_GENERIC_INVALIDREQUEST);
			return ERROR_GENERIC_INVALIDREQUEST;
		}

		int nRet = pMembershipDB->QueryGetCashBalance(this->GetAccountDBID(), NULL, nCashBalance);

		int nPetal = GetPetalBalance();

		if (m_nTotalPrice <= nCashBalance) 
		{
		  nRet = PurchaseBuyItem();       //듐환뭔찜
		}
		else if (m_nTotalPrice <= nPetal) 
		{
			pCash->cPaymentRules = Cash::PaymentRules::Petal;
			nRet = PurchaseItemByPetal();  //$_$  질듐뭔찜
		}
		else
		{
			printf("Not Enough Money!\n");
			SendBuyGift(ERROR_GENERIC_INVALIDREQUEST);  //@_@
			return ERROR_GENERIC_INVALIDREQUEST;
		}
	}

	return nRet;
}

int CDNUserRepository::OnPackageBuy(int nThreadID, CDNConnection *pConnection, TQCashPackageBuy *pCash)
{
	SetPackageBuy(nThreadID, pConnection, pCash);

#if defined(PRE_ADD_SALE_COUPON)
	if( m_nTotalPrice <= 0 && !g_pExtManager->GetCashCommodityUseCoupon(pCash->nPackageSN))
#else
	if( m_nTotalPrice <=0 )
#endif
	{
		SendBuyGift(ERROR_ITEM_FAIL);
		return ERROR_ITEM_FAIL;
	}

	if (!CheckSaleItemSN(pCash->nPackageSN)){
		g_Log.Log(LogType::_ERROR, this, L"[CASH_PACKAGEBUY] CheckSaleItemSN Error (ItemSN:%d)\r\n", pCash->nPackageSN);
		SendBuyGift(ERROR_CASHSHOP_PROHIBITSALE);
		return ERROR_CASHSHOP_PROHIBITSALE;
	}

	int nRet = ERROR_DB;
	int nAdd = 0;
	for (int i = 0; i < pCash->cPackageCount; i++){
		if (g_pExtManager->GetItemMainType(pCash->BuyList[i].CashItem.nItemID) == ITEMTYPE_CHARACTER_SLOT){
			nAdd++;
		}
	}
#if defined(PRE_MOD_SELECT_CHAR)
	nRet = CheckCharacterSlotCount(pCash->nAccountDBID, nAdd);
#else	// #if defined(PRE_MOD_SELECT_CHAR)
	nRet = CheckCharacterSlotCount(pCash->biCharacterDBID, nAdd);
#endif	// #if defined(PRE_MOD_SELECT_CHAR)
	if (nRet != ERROR_NONE){
		SendBuyGift(nRet);
		return nRet;
	}

	INT64 biOrderID = 0;
	if (pCash->cPaymentRules == Cash::PaymentRules::Petal){		// 페탈로 물건사기
		PurchaseItemByPetal();
	}
#if defined( PRE_ADD_NEW_MONEY_SEED )
	else if( pCash->cPaymentRules == Cash::PaymentRules::Seed )
	{
		nRet = PurchaseItemBySeed();
	}
#endif
	else {	// 캐쉬로 물건 사기
		nRet = AddPurchaseOrderByCash(0);
		if (nRet != ERROR_NONE){
			SendBuyGift(nRet);
			return nRet;
		}

#if defined(PRE_ADD_SALE_COUPON)
		if( m_nTotalPrice == 0 && g_pExtManager->GetCashCommodityUseCoupon(pCash->nPackageSN) )
		{
			// 가격이 0원이고 쿠폰아이템이면 빌링으로 쏘지 않고 바로 지급(중국빌링서버의 경우 0원도 에러로 리턴하기 때문에 빌링서버를 거치지 않고 처리)
			OnRecvBillingBuyItem(ERROR_NONE);
			return ERROR_NONE;
		}
#endif	//	#if defined(PRE_ADD_SALE_COUPON)

#if defined(_TW) || defined(_TH)
		nRet = g_pBillingShopConnection->OnPackageBuy(this);
#else	// defined(_TW)
		nRet = g_pBillingConnection->OnPackageBuy(this);
#endif	// defined(_TW)
		if (nRet != ERROR_NONE){
			SendBuyGift(nRet);
			return nRet;
		}
	}

	return ERROR_NONE;
}

int CDNUserRepository::OnGift(int nThreadID, CDNConnection *pConnection, TQCashGift *pCash)
{
	if( !SetGift(nThreadID, pConnection, pCash) )
	{
		SendBuyGift(ERROR_ITEM_FAIL);
		return ERROR_ITEM_FAIL;
	}

	// onsale인 애가 아닌데 사면 에러값 뱉어라
	for (int i = 0; i < pCash->cProductCount; i++){
		if (!CheckSaleItemSN(pCash->GiftList[i].nItemSN)){
			g_Log.Log(LogType::_ERROR, this, L"[CASH_GIFT] CheckSaleItemSN Error (ItemSN:%d)\r\n", pCash->GiftList[i].nItemSN);
			SendBuyGift(ERROR_CASHSHOP_PROHIBITSALE);
			return ERROR_CASHSHOP_PROHIBITSALE;
		}
	}

#if defined(_SG) || defined(_TH) || defined(_ID)// 싱가폴, 태국, 인도네시아은 AccountName필요함
	SetReceiverAccountName();
#endif // #if defined(_SG) || defined(_TH)

	return PurchaseGiftItem();
}

int CDNUserRepository::OnPackageGift(int nThreadID, CDNConnection *pConnection, TQCashPackageGift *pCash)
{
	SetPackageGift(nThreadID, pConnection, pCash);

	if( m_nTotalPrice <=0 )
	{
		SendBuyGift(ERROR_ITEM_FAIL);
		return ERROR_ITEM_FAIL;
	}

	if (!CheckSaleItemSN(pCash->nPackageSN)){
		g_Log.Log(LogType::_ERROR, this, L"[CASH_PACKAGEGIFT] CheckSaleItemSN Error (ItemSN:%d)\r\n", pCash->nPackageSN);
		SendBuyGift(ERROR_CASHSHOP_PROHIBITSALE);
		return ERROR_CASHSHOP_PROHIBITSALE;
	}

#if defined(_SG) || defined(_TH) || defined(_ID)// 싱가폴, 태국, 인도네시아은 AccountName필요함
	SetReceiverAccountName();
#endif // #if defined(_SG) || defined(_TH)

	int nRet = AddPurchaseOrderByCash(0);
	if (nRet != ERROR_NONE){
		SendBuyGift(nRet);
		return nRet;
	}

#if defined(_TW) || defined(_TH)
	nRet = g_pBillingShopConnection->OnPackageGift(this);
#else	//
	nRet = g_pBillingConnection->OnPackageGift(this);
#endif	//
	if (nRet != ERROR_NONE){
		SendBuyGift(nRet);
		return nRet;
	}

	return ERROR_NONE;
}

int CDNUserRepository::OnCoupon(int nThreadID, CDNConnection *pConnection, TQCashCoupon *pCash)
{
	//return ERROR_GENERIC_INVALIDREQUEST;  //[debug] 埼뵀뚝뻣묘콘，殮쌈럿쀼呵겨

	SetCoupon(nThreadID, pConnection, pCash);

	int nRet = ERROR_GENERIC_INVALIDREQUEST;
#if defined(_TW)
	nRet = g_pBillingCouponConnection->OnCoupon(this);
#elif defined(_TH)
	nRet = g_pBillingShopConnection->OnCoupon(this);
#else
	nRet = g_pBillingConnection->OnCoupon(this);
#endif
	//if (nRet != ERROR_NONE)//RawCode
		//SendCoupon(nRet);//RawCode
	    SendCoupon(ERROR_GENERIC_INVALIDREQUEST);  

	return ERROR_NONE;
}

int CDNUserRepository::OnVIPBuy(int nThreadID, CDNConnection *pConnection, TQCashVIPBuy *pCash)
{
	SetVIPBuy(nThreadID, pConnection, pCash);

	CDNSQLMembership *pMembershipDB = GetMembershipDB();
	if (!pMembershipDB){
		SendVIPBuyGift(ERROR_GENERIC_INVALIDREQUEST, 0);
		return ERROR_GENERIC_INVALIDREQUEST;
	}

	if (!CheckSaleItemSN(pCash->nItemSN)){
		SendVIPBuyGift(ERROR_CASHSHOP_PROHIBITSALE, 0);
		return ERROR_CASHSHOP_PROHIBITSALE;
	}

	TCashCommodityData CashData;
	bool bRet= g_pExtManager->GetCashCommodityData(pCash->nItemSN, CashData);
	if (!bRet){
		SendVIPBuyGift(ERROR_ITEM_FAIL, 0);
		return ERROR_ITEM_FAIL;
	}

	INT64 biOrderID = 0;

	int nVIPPrice = CashData.nPrice;
	if (pCash->bAutoPay)	// 자동결제면 할인된 가격으로 
		nVIPPrice = (int)((CashData.nPrice * (100 - CashData.nAutomaticPaySale)) / 100);

#ifdef PRE_ADD_LIMITED_CASHITEM
	int nRet = pMembershipDB->QueryAddPurchaseOrderByCash(pCash->biCharacterDBID, pCash->bPCBang, pCash->nItemSN, nVIPPrice, g_pExtManager->GetCashBuyAbleCount(pCash->nItemSN), 
		CashData.nItemID[0], -1, g_pExtManager->GetCashLimitedItemCount(pCash->nItemSN), nVIPPrice, m_szIp, false, NULL, NULL, biOrderID);
#else		//#ifdef PRE_ADD_LIMITED_CASHITEM
	int nRet = pMembershipDB->QueryAddPurchaseOrderByCash(pCash->biCharacterDBID, pCash->bPCBang, pCash->nItemSN, nVIPPrice, g_pExtManager->GetCashBuyAbleCount(pCash->nItemSN), 
		CashData.nItemID[0], -1, nVIPPrice, m_szIp, false, NULL, NULL, biOrderID);
#endif		//#ifdef PRE_ADD_LIMITED_CASHITEM
	if (nRet != ERROR_NONE){
		SendVIPBuyGift(nRet, 0);
		return nRet;
	}

#ifdef PRE_ADD_LIMITED_CASHITEM
	if (g_pLimitedCashItemRepository && g_pExtManager->GetCashLimitedItemCount(pCash->nItemSN) > 0)
	{
		std::vector<LimitedCashItem::TLimitedQuantityCashItem> vList;
		if (g_pLimitedCashItemRepository->GetLimitedItemList(vList))
			m_pConnection->SendLimiteItemInfoList(vList);
	}
#endif		//#ifdef PRE_ADD_LIMITED_CASHITEM

	m_VIP.biPurchaseOrderID = biOrderID;

#if defined(_CH) || defined(_EU)
	CDNBillingConnectionCH::TGoodsInfo Goods[MAX_GOOD_LEN] = { 0, };

	Goods[0].nItemSN = m_VIP.nItemSN;
	Goods[0].nPrice = g_pExtManager->GetCashCommodityPrice(m_VIP.nItemSN);

	nRet = g_pBillingConnection->SendAccountLock(this, Goods, 1, false);
	if (nRet < 0){
		g_Log.Log(LogType::_ERROR, this, L"[SendBillingVIPBuy] Ret : %d\r\n", nRet);
		SendVIPBuyGift(nRet, 0);
		return nRet;
	}

#elif defined(_TW)
	// GASH는 아이템리스트 같은게 없고 단순 포인트 차감	
	g_pBillingShopConnection->SendPurchaseItem(m_szAccountName, m_szIp, m_VIP.nPrice, m_nAccountDBID, m_VIP.biPurchaseOrderID, m_VIP.nItemSN);
#endif	// #if defined(_CH) || defined(_EU)

#if defined(_KAMO)  //[OK_Cash] 
	if (nRet == ERROR_NONE){

		int nCashOutgoID = 0, nCashBalance = 0;
		int nRetDeductCash = pMembershipDB->QueryDeductCash(m_nAccountDBID, biOrderID, nVIPPrice, nCashOutgoID, nCashBalance); //VIP

		if (ERROR_NONE == nRetDeductCash)
		{
			SendVIPBuyGift(nRetDeductCash, 0);  //있떴듐뭔찜냥묘
		} 
		else
		{
			printf("[Err]CDNUserRepository::OnVIPBuy AccountID=%d Ret=%d,TotalPrice=%d,CashBalance=%d",m_nAccountDBID,nRetDeductCash,nVIPPrice,nCashBalance);
			//SendVIPBuyGift(ERROR_GENERIC_INVALIDREQUEST, 0);  //있떴듐뭔찜呵겨，옵콘角질듐
		}

		return nRetDeductCash;
	}
#endif

	return ERROR_NONE;
}

int CDNUserRepository::OnVIPGift(int nThreadID, CDNConnection *pConnection, TQCashVIPGift *pCash)
{
	SendVIPBuyGift(ERROR_GENERIC_INVALIDREQUEST, 0);
	return ERROR_GENERIC_INVALIDREQUEST;

/*
	SetVIPGift(nThreadID, pConnection, pCash);

	CDNSQLMembership *pMembershipDB = GetMembershipDB();
	if (!pMembershipDB){
		SendVIPBuyGift(ERROR_GENERIC_INVALIDREQUEST, 0);
		return ERROR_GENERIC_INVALIDREQUEST;
	}

	if (!CheckSaleItemSN(pCash->nItemSN)){
		SendVIPBuyGift(ERROR_CASHSHOP_PROHIBITSALE, 0);
		return ERROR_CASHSHOP_PROHIBITSALE;
	}

	TCashCommodityData CashData;
	bool bRet = g_pExtManager->GetCashCommodityData(pCash->nItemSN, CashData);
	if (!bRet){
		SendVIPBuyGift(ERROR_ITEM_FAIL, 0);
		return ERROR_ITEM_FAIL;
	}

	INT64 biOrderID = 0;
#ifdef PRE_ADD_LIMITED_CASHITEM
	int nRet = pMembershipDB->QueryAddPurchaseOrderByCash(pCash->biCharacterDBID, pCash->bPCBang, pCash->nItemSN, CashData.nPrice, g_pExtManager->GetCashBuyAbleCount(pCash->nItemSN), 
		CashData.nItemID[0], -1, g_pExtManager->GetCashLimitedItemCount(pCash->nItemSN), CashData.nPrice, m_szIp, true, pCash->wszReceiverCharacterName, pCash->wszMessage, biOrderID);
#else		//#ifdef PRE_ADD_LIMITED_CASHITEM
	int nRet = pMembershipDB->QueryAddPurchaseOrderByCash(pCash->biCharacterDBID, pCash->bPCBang, pCash->nItemSN, CashData.nPrice, g_pExtManager->GetCashBuyAbleCount(pCash->nItemSN), 
		CashData.nItemID[0], -1, CashData.nPrice, m_szIp, true, pCash->wszReceiverCharacterName, pCash->wszMessage, biOrderID);
#endif		//#ifdef PRE_ADD_LIMITED_CASHITEM

	if (nRet != ERROR_NONE){
		SendVIPBuyGift(nRet, 0);
		return nRet;
	}

#ifdef PRE_ADD_LIMITED_CASHITEM
	if (g_pLimitedCashItemRepository && g_pExtManager->GetCashLimitedItemCount(pCash->nItemSN) > 0)
	{
		std::vector<LimitedCashItem::TLimitedQuantityCashItem> vList;
		if (g_pLimitedCashItemRepository->GetLimitedItemList(vList))
			m_pConnection->SendLimiteItemInfoList(vList);
	}
#endif		//#ifdef PRE_ADD_LIMITED_CASHITEM

	m_VIP.biPurchaseOrderID = biOrderID;

#if defined(_CH) || defined(_EU)
	CDNBillingConnectionCH::TGoodsInfo Goods[MAX_GOOD_LEN] = { 0, };

	Goods[0].nItemSN = m_VIP.nItemSN;
	Goods[0].nPrice = g_pExtManager->GetCashCommodityPrice(m_VIP.nItemSN);

	nRet = g_pBillingConnection->SendAccountLock(this, Goods, 1, true);
	if (nRet < 0){
		g_Log.Log(LogType::_ERROR, this, L"[SendBillingVIPGift] Ret : %d\r\n", nRet);
		SendVIPBuyGift(nRet, 0);
		return nRet;
	}

#elif defined(_TW)
	// GASH는 아이템리스트 같은게 없고 단순 포인트 차감	
	g_pBillingShopConnection->SendPurchaseItem(m_szAccountName, m_szIp, m_VIP.nPrice, m_nAccountDBID, m_VIP.biPurchaseOrderID, m_VIP.nItemSN);

#endif	// #if defined(_CH) || defined(_EU)

#if defined(_KAMO)  //[OK_Cash] 
	if (nRet == ERROR_NONE){
		SendVIPBuyGift(nRet, 0);
		return nRet;
	}
#endif

	return ERROR_NONE;
	*/
}

void CDNUserRepository::OnRefund(int nThreadID, CDNConnection *pConnection, TQCashRefund* pCash)
{
#if defined(PRE_ADD_CASH_REFUND)
	/*
	SetRefund(nThreadID, pConnection, pCash);

	int nProductNo = g_pBillingConnection->GetProductNoBySN(pCash->nItemSN);
	g_pBillingConnection->SendRefund(this, pCash->uiOrderNo, nProductNo, pCash->biPurchaseOrderDetailID);
	*/
#endif	// #if defined(PRE_ADD_CASH_REFUND)
}

void CDNUserRepository::OnMoveCashInven(int nThreadID, CDNConnection *pConnection, TQCashMoveCashInven* pCash)
{
#if defined(PRE_ADD_CASH_REFUND)
	m_cWorldSetID = pCash->cWorldSetID;

	CDNSQLMembership *pMembershipDB = GetMembershipDB();
	if (!pMembershipDB){
		m_pConnection->SendMoveCashInven(pCash->nAccountDBID, ERROR_GENERIC_INVALIDREQUEST, 0, 0, NULL, 0);
		return;
	}

	// 결재인벤 플래그 끄고
	int nRet = pMembershipDB->QueryModPurchaseOrderDetail(pCash->PaymentItemInfo.PaymentItemInfo.biDBID);	
	if (nRet != ERROR_NONE)
	{
		m_pConnection->SendMoveCashInven(pCash->nAccountDBID, nRet, 0, 0, NULL, 0);
		return;
	}
	// 상품 DB에 넣고
	nRet = CashItemDBProcess(pCash->cWorldSetID, pCash->nAccountDBID, pCash->biCharacterDBID, pCash->nMapID, pCash->nChannelID, 0, pCash->CashItem, 
		g_pExtManager->GetCashCommodityPrice(pCash->PaymentItemInfo.PaymentItemInfo.ItemInfo.nItemSN), pCash->PaymentItemInfo.PaymentItemInfo.biDBID, DBDNWorldDef::AddMaterializedItem::CashBuy);

	if (nRet != ERROR_NONE)			// 만약 실패하면 실패플래그 켜고..
		pMembershipDB->QueryModGiveFailFlag2(pCash->PaymentItemInfo.PaymentItemInfo.biDBID, true);			

	// 혹시 페탈 적립해야하는지 검사
	int nReservePetal = g_pExtManager->GetCashCommodityReserve(pCash->PaymentItemInfo.PaymentItemInfo.ItemInfo.nItemSN);
	int nTotalPetal = 0;
	if( nReservePetal > 0 )
	{
		nRet = pMembershipDB->QueryAddPetalIncomeCashRefund(pCash->biCharacterDBID, pCash->PaymentItemInfo.PaymentItemInfo.biDBID, nReservePetal, nTotalPetal);
		if( nRet != ERROR_NONE ) // 여기까지 와서 에러를 줘야하나..
		{
			m_pConnection->SendMoveCashInven(pCash->nAccountDBID, nRet, 0, 0, NULL, 0);
			return;
		}
	}
	else
	{
		nTotalPetal = GetPetalBalance();
	}
	/*
	// 빌링에 SendPickUp 날리자
	int nProductNo = g_pBillingConnection->GetProductNoBySN(pCash->PaymentItemInfo.PaymentItemInfo.ItemInfo.nItemSN);
	g_pBillingConnection->SendInventoryPickup(pCash->nAccountDBID, pCash->PaymentItemInfo.uiOrderNo, nProductNo, 1); // 수량은 무조건 1개.
	*/

	INT64 nSeedPoint = 0;
#if defined( PRE_ADD_NEW_MONEY_SEED )	
	int nSeed = g_pExtManager->GetCashCommoditySeed(pCash->PaymentItemInfo.PaymentItemInfo.ItemInfo.nItemSN);
	if( nSeed > 0 )
	{
		nRet = AddSeedPoint( nSeed );
		if( nRet != ERROR_NONE )
		{
			g_Log.Log(LogType::_ERROR, this, L"[OnMoveCashInven] AddSeedPoint Fail SeedPoint(%d)\r\n", nSeed);			
		}
	}
	nSeedPoint = GetSeedPoint();	
#endif	
	
	m_pConnection->SendMoveCashInven(pCash->nAccountDBID, ERROR_NONE, nTotalPetal, pCash->PaymentItemInfo.PaymentItemInfo.biDBID, &pCash->CashItem, nSeedPoint);
#endif	// #if defined(PRE_ADD_CASH_REFUND)
}

void CDNUserRepository::OnPackageMoveCashInven(int nThreadID, CDNConnection *pConnection, TQCashPackageMoveCashInven* pCash)
{
#if defined(PRE_ADD_CASH_REFUND)
	m_cWorldSetID = pCash->cWorldSetID;

	CDNSQLMembership *pMembershipDB = GetMembershipDB();
	if (!pMembershipDB){
		m_pConnection->SendPackageMoveCashInven(pCash->nAccountDBID, ERROR_GENERIC_INVALIDREQUEST, 0, 0, NULL, 0);
		return;
	}

	// 결재인벤 플래그 끄고
	int nRet = pMembershipDB->QueryModPurchaseOrderDetail(pCash->PaymentPackageItemInfo.PaymentPackageItemInfo.biDBID);	
	if (nRet != ERROR_NONE)
	{
		m_pConnection->SendPackageMoveCashInven(pCash->nAccountDBID, nRet, 0, 0, NULL, 0);
		return;
	}	

	int nProductCount = 0;
	for( int i=0; i<PACKAGEITEMMAX; ++i)
	{
		if( pCash->PaymentPackageItemInfo.PaymentPackageItemInfo.ItemInfoList[i].nItemID == 0)
			break;
		++nProductCount;
	}

	if (nProductCount == 0)
	{
		m_pConnection->SendPackageMoveCashInven(pCash->nAccountDBID, ERROR_GENERIC_UNKNOWNERROR, 0, 0, NULL, 0);
		return;
	}

	int nTotalPrice = g_pExtManager->GetCashCommodityPrice(pCash->PaymentPackageItemInfo.PaymentPackageItemInfo.nPackageSN);
	int nUnitPrice = nTotalPrice / nProductCount;
	int nTempPrice = 0;
	for (int i = 0; i < nProductCount; i++)
	{
		if (i == (nProductCount - 1)) nUnitPrice = nTotalPrice - nTempPrice;					
		// 상품 DB에 넣고
		nRet = CashItemDBProcess(pCash->cWorldSetID, pCash->nAccountDBID, pCash->biCharacterDBID, pCash->nMapID, pCash->nChannelID, 0, pCash->CashItemList[i], 
			nUnitPrice, pCash->PaymentPackageItemInfo.PaymentPackageItemInfo.biDBID, DBDNWorldDef::AddMaterializedItem::CashBuy);

		if (nRet != ERROR_NONE){
			pMembershipDB->QueryModGiveFailFlag2(pCash->PaymentPackageItemInfo.PaymentPackageItemInfo.biDBID, true);						
			// 일단 실패해도 return 하지말고 다 처리는 하자
		}
		nTempPrice += nUnitPrice;
	}

	// 혹시 페탈 적립해야하는지 검사
	int nReservePetal = g_pExtManager->GetCashCommodityReserve(pCash->PaymentPackageItemInfo.PaymentPackageItemInfo.nPackageSN);
	int nTotalPetal = 0;
	if( nReservePetal > 0 )
	{
		nRet = pMembershipDB->QueryAddPetalIncomeCashRefund(pCash->biCharacterDBID, pCash->PaymentPackageItemInfo.PaymentPackageItemInfo.biDBID, nReservePetal, nTotalPetal);
		if( nRet != ERROR_NONE ) // 여기까지 와서 에러를 줘야하나..
		{
			m_pConnection->SendPackageMoveCashInven(pCash->nAccountDBID, nRet, 0, 0, NULL, 0);
			return;
		}
	}
	else
	{
		nTotalPetal = GetPetalBalance();
	}

	

	INT64 nSeedPoint = 0;
#if defined( PRE_ADD_NEW_MONEY_SEED )	
	int nSeed = g_pExtManager->GetCashCommoditySeed(pCash->PaymentPackageItemInfo.PaymentPackageItemInfo.nPackageSN);
	if( nSeed > 0 )
	{
		nRet = AddSeedPoint( nSeed );
		if( nRet != ERROR_NONE )
		{
			g_Log.Log(LogType::_ERROR, this, L"[OnPackageMoveCashInven] AddSeedPoint Fail SeedPoint(%d)\r\n", nSeed);			
		}
	}
	nSeedPoint = GetSeedPoint();	
#endif
	/*
	// 빌링에 SendPickUp 날리자
	int nProductNo = g_pBillingConnection->GetProductNoBySN(pCash->PaymentPackageItemInfo.PaymentPackageItemInfo.nPackageSN);
	g_pBillingConnection->SendInventoryPickup(pCash->nAccountDBID, pCash->PaymentPackageItemInfo.uiOrderNo, nProductNo, 1); // 수량은 무조건 1개.
	*/
	m_pConnection->SendPackageMoveCashInven(pCash->nAccountDBID, ERROR_NONE, nTotalPetal, pCash->PaymentPackageItemInfo.PaymentPackageItemInfo.biDBID, pCash->CashItemList, nSeedPoint);
#endif	// #if defined(PRE_ADD_CASH_REFUND)
}

int CDNUserRepository::CalcPackageProcess()
{
	CDNSQLMembership *pMembershipDB = GetMembershipDB();
	if (!pMembershipDB)
		return ERROR_GENERIC_INVALIDREQUEST;

	if (0 == m_cProductCount)
	{
		printf("[err] CDNUserRepository::CalcPackageProcess m_cProductCount=0\n");  //0꼇콘鱗槨뇜鑒
		return ERROR_GENERIC_INVALIDREQUEST;
	}

	int nRet = 0;

#if defined(_KRAZ)
	int nTotalPrice = m_nUsedPaidCash;
#else	// #if defined(_KRAZ)
	int nTotalPrice = g_pExtManager->GetCashCommodityPrice(m_nPackageSN);
#endif	// #if defined(_KRAZ)
	int nUnitPrice = nTotalPrice / m_cProductCount;
	int nTempPrice = 0;
	for (int i = 0; i < m_cProductCount; i++){
		if (i == (m_cProductCount - 1)) nUnitPrice = nTotalPrice - nTempPrice;

		int nCode = DBDNWorldDef::AddMaterializedItem::CashBuy;

		nRet = CashItemDBProcess(m_cWorldSetID, m_nAccountDBID, m_biCharacterDBID, m_nMapID, m_nChannelID, m_cPaymentRules, m_ProductPackageList[i], nUnitPrice, m_biPurchaseOrderID, nCode);

		if (nRet != ERROR_NONE){
			pMembershipDB->QueryModGiveFailFlag(m_biPurchaseOrderID, m_nPackageSN, m_ProductPackageList[i].CashItem.nItemID, true);
			g_Log.Log(LogType::_ERROR, this, L"[SetBillingNo(Package)] CashItemDBQuery Fail(%d) Order(%I64d)\r\n", nRet, m_biPurchaseOrderID);
			// 일단 실패해도 return 하지말고 다 처리는 하자
		}
		nTempPrice += nUnitPrice;
	}

	return nRet;
}

#if defined(PRE_ADD_CASH_REFUND)
void CDNUserRepository::OnRecvBillingRefund(int nResult)
{
	if( nResult != ERROR_NONE)
	{
		SendCashRefund(nResult);
		return;
	}

	CDNSQLMembership *pMembershipDB = GetMembershipDB();
	if (!pMembershipDB){
		SendCashRefund(ERROR_DB);  // 이미 환불됐는데 이럼 안됨..
		return;
	}

	// 환불 처리..
	int nRet = pMembershipDB->QueryCashRefund(m_biPurchaseOrderDetailID);
	if (nRet != ERROR_NONE){
		SendCashRefund(nRet);
		return;
	}

	SendCashRefund(ERROR_NONE);	
}
#endif

bool CDNUserRepository::OnRecvBillingCoupon(int nBillingResult, int nResult, int nItemSN, UINT nOrderNo)
{
	// 씨리얼 패키지가 존재하는지 유무.
	if (!g_pExtManager->IsOnSaleCommodity(nItemSN))
	{
		SendCoupon(ERROR_CASHSHOP_PROHIBITSALE);
		return false;
	}

	CDNSQLMembership *pMembershipDB = GetMembershipDB();
	if (!pMembershipDB){
		SendCoupon(ERROR_GENERIC_INVALIDREQUEST);
		return false;
	}

	string SNStr, ItemIDStr, ItemOptionStr;
	BYTE cOption = -1;

	TCashCommodityData CashData;
	memset(&CashData, 0, sizeof(CashData));

	TCashPackageData PackageData;
	memset(&PackageData, 0, sizeof(PackageData));
	bool bPackage = g_pExtManager->GetCashPackageData(nItemSN, PackageData);

	if (bPackage){	// package
		SNStr = FormatA("%d", nItemSN);
		for (int i = 0; i < (int)PackageData.nVecCommoditySN.size(); i++){
			if (PackageData.nVecCommoditySN[i] <= 0) continue;

			if (!ItemIDStr.empty()){
				ItemIDStr.append("|");
				ItemOptionStr.append("|");
			}

			memset(&CashData, 0, sizeof(TCashCommodityData));
			bool bCash = g_pExtManager->GetCashCommodityData(PackageData.nVecCommoditySN[i], CashData);
			if (!bCash) continue;

			ItemIDStr.append(boost::lexical_cast<std::string>(CashData.nItemID[0]));
			ItemOptionStr.append(boost::lexical_cast<std::string>(static_cast<int>(cOption)));			
		}
	}
	else{
		memset(&CashData, 0, sizeof(TCashCommodityData));
		SNStr = FormatA("%d", nItemSN);
		bool bCash = g_pExtManager->GetCashCommodityData(nItemSN, CashData);
		if (bCash){
			ItemIDStr.append(boost::lexical_cast<std::string>(CashData.nItemID[0]));
			ItemOptionStr.append(boost::lexical_cast<std::string>(static_cast<int>(cOption)));
		}
	}

	m_nCouponSN = nItemSN;

	string OrderStr = FormatA("%d", nOrderNo);
	int nRet = pMembershipDB->QueryMakeGiftByCoupon(m_biCharacterDBID, m_bPCBang, SNStr.c_str(), ItemIDStr.c_str(), ItemOptionStr.c_str(), m_wszCoupon, OrderStr.c_str(), m_szIp, m_biCouponOrderID);
	if (nRet != ERROR_NONE){
		SendCoupon(nRet);
		return false;
	}

	char szOrderNo[PUBLISHERORDERKEYMAX] = {0, };
	sprintf_s(szOrderNo, "%d", nOrderNo);
	char cOrderCode = DBDNMembership::OrderStatusCode::FailPayment;
	if (nResult == ERROR_NONE)
		cOrderCode = DBDNMembership::OrderStatusCode::Success;

	nRet = pMembershipDB->QuerySetPurchaseOrderResult(m_biCouponOrderID, cOrderCode, szOrderNo, nBillingResult, g_pExtManager->GetCashCommodityReserve(m_nCouponSN), m_nUsedPaidCash);
	if (nRet != ERROR_NONE){
		g_Log.Log(LogType::_ERROR, this, L"[SetCoupon] QuerySetPurchaseOrderResult Fail(%d) Order(Eye:%I64d Nexon:%u)\r\n", 
			nResult, m_biCouponOrderID, nOrderNo);
		SendCoupon(nRet);
		return false;
	}

	SendCoupon(nResult);
	return true;
}

void CDNUserRepository::OnRecvBillingPurchase(int nResult, int nBillingResult, UINT nOrderNo, char *pNexonOrderID)
{
	CDNSQLMembership *pMembershipDB = GetMembershipDB();
	if (!pMembershipDB){
		SendBuyGift(ERROR_GENERIC_INVALIDREQUEST);
		return;
	}

	switch (m_nSubCommand)
	{
	case CASH_PACKAGEBUY:
		{
			char szOrderNo[PUBLISHERORDERKEYMAX] = {0, };
			sprintf_s(szOrderNo, "%d", nOrderNo);
			char cOrderCode = DBDNMembership::OrderStatusCode::FailPayment;
			if (nResult == ERROR_NONE)
				cOrderCode = DBDNMembership::OrderStatusCode::Success;

			int nPetalAmount = 0;
#if defined( PRE_ADD_NEW_MONEY_SEED )
			int nSeedAmount = 0;
#endif
#if defined(PRE_ADD_CASH_REFUND) // 패키지는 캐쉬인벤 바로가기 일때만 페탈 저장..나머지는 캐쉬인벤으로 옮길때.
			if( GetCashMoveInven() )
#endif
			{
				nPetalAmount = g_pExtManager->GetCashCommodityReserve(m_nPackageSN);
#if defined( PRE_ADD_NEW_MONEY_SEED )
				nSeedAmount = g_pExtManager->GetCashCommoditySeed(m_nPackageSN);
#endif
			}

			int nRet = pMembershipDB->QuerySetPurchaseOrderResult(m_biPurchaseOrderID, cOrderCode, szOrderNo, nBillingResult, nPetalAmount, m_nUsedPaidCash);
			if (nRet != ERROR_NONE){
				SendBuyGift(nRet);
				g_Log.Log(LogType::_ERROR, this, L"[SetBillingNo(Package)] QuerySetPurchaseOrderResult Fail(%d) Order(Eye:%I64d Nexon:%u)\r\n", 
					nResult, m_biPurchaseOrderID, nOrderNo);
				return;
			}

#if defined( PRE_ADD_NEW_MONEY_SEED )
			nRet = AddSeedPoint( nSeedAmount );
			if( ERROR_NONE != nRet )
			{
				g_Log.Log(LogType::_ERROR, this, L"[SetBillingNo(Package)] AddSeedPoint Fail PackageSN(%d) SeedPoint(%d)\r\n", m_nPackageSN, nSeedAmount);
			}
#endif
			// 제대로 됐을때만 
			if (nResult == ERROR_NONE)
			{
#if defined(PRE_ADD_CASH_REFUND) // 패키지는 캐쉬인벤 바로가기 일때만 값을 넣어주자..나머지는 캐쉬인벤으로 옮길때.
				if( !GetCashMoveInven() )
				{					
					__time64_t _tNow;
					time(&_tNow);
					m_PaymentPackageItem.uiOrderNo = nOrderNo;
					m_PaymentPackageItem.nPrice = g_pExtManager->GetCashCommodityPrice(m_nPackageSN);
					m_PaymentPackageItem.PaymentPackageItemInfo.nPackageSN = m_nPackageSN;
					m_PaymentPackageItem.PaymentPackageItemInfo.tPaymentDate = _tNow;
					m_PaymentPackageItem.PaymentPackageItemInfo.biDBID = m_biPurchaseOrderDetailID;

					for (int i = 0; i < m_cProductCount; i++)
					{
						m_PaymentPackageItem.PaymentPackageItemInfo.ItemInfoList[i].nItemSN = m_ProductPackageList[i].nItemSN;
						m_PaymentPackageItem.PaymentPackageItemInfo.ItemInfoList[i].nItemID = m_ProductPackageList[i].CashItem.nItemID;
						m_PaymentPackageItem.PaymentPackageItemInfo.ItemInfoList[i].cItemOption = m_ProductPackageList[i].CashItem.cOption;
					}
				}
				else
#endif
				{
					CalcPackageProcess();
				}				
			}

			SendBuyGift(nResult);
		}
		break;

	case CASH_BUY:
		{
			if (m_VecBuyItemList.empty()) return;

			char szData[64] = { 0, };
			memcpy(szData, &pNexonOrderID[14], strlen(pNexonOrderID) - 14);
			INT64 biOrderID = _atoi64(szData);
			if (biOrderID != m_biPurchaseOrderID){
				g_Log.Log(LogType::_ERROR, this, L"[SetBillingNo(Buy)] biOrderID != m_biPurchaseOrderID Order(Eye:%I64d Nexon:%u)\r\n", m_biPurchaseOrderID, biOrderID);
				return;
			}

			// m_nBillingOrderNo = nOrderNo;

			char szOrderNo[PUBLISHERORDERKEYMAX] = {0, };
			sprintf_s(szOrderNo, "%d", nOrderNo);
			char cOrderCode = DBDNMembership::OrderStatusCode::FailPayment;
			if (nResult == ERROR_NONE)
				cOrderCode = DBDNMembership::OrderStatusCode::Success;

			int nTotalReservePetal = 0;
#if defined( PRE_ADD_NEW_MONEY_SEED )
			int  nTotalSeedPoint = 0;
#endif
			for (int i = 0; i < (int)m_VecBuyItemList.size(); i++)
			{
#if defined(PRE_ADD_CASH_REFUND) // 즉시적용 상품이나 환불불가능한 애들만 우선 Petal적립..나머지는 캐쉬인벤으로 옮길때.
				if( GetCashMoveInven() || g_pExtManager->GetCashCommodityNoRefund(m_VecBuyItemList[i].BuyItem.CashItem.nItemID, m_VecBuyItemList[i].BuyItem.nItemSN) )
#endif
				{
				
				nTotalReservePetal += m_VecBuyItemList[i].nReservePetal;
#if defined( PRE_ADD_NEW_MONEY_SEED )
				nTotalSeedPoint += m_VecBuyItemList[i].nSeedPoint;
#endif
				}
			}

			int nRet = pMembershipDB->QuerySetPurchaseOrderResult(m_biPurchaseOrderID, cOrderCode, szOrderNo, nBillingResult, nTotalReservePetal, m_nUsedPaidCash);
			if (nRet != ERROR_NONE){
				g_Log.Log(LogType::_ERROR, this, L"[SetBillingNo(Buy)] QuerySetPurchaseOrderResult Fail(%d) Order(Eye:%I64d Nexon:%u)\r\n", nResult, m_biPurchaseOrderID, nOrderNo);
				SendBuyGift(nRet);
				return;
			}
#if defined( PRE_ADD_NEW_MONEY_SEED )
			nRet = AddSeedPoint( nTotalSeedPoint );
			if( ERROR_NONE != nRet )
			{
				g_Log.Log(LogType::_ERROR, this, L"[SetBillingNo(Buy)] AddSeedPoint Fail SeedPoint(%d)\r\n", nTotalSeedPoint);
			}
#endif

			if (nResult == ERROR_NONE)
			{
#if defined(PRE_ADD_CASH_REFUND)
				int nCount = 0;
#endif
				for (int i = 0; i < (int)m_VecBuyItemList.size(); i++)
				{
					m_VecBuyItemList[i].BuyItem.bFail = false;

#if defined(PRE_ADD_CASH_REFUND) // 즉시적용 상품이나 환불불가능한 애들만 우선 ItemInsert..나머지는 캐쉬인벤으로 옮길때...
					if( GetCashMoveInven() || g_pExtManager->GetCashCommodityNoRefund(m_VecBuyItemList[i].BuyItem.CashItem.nItemID, m_VecBuyItemList[i].BuyItem.nItemSN) )										
#endif
					{
						int nCode = DBDNWorldDef::AddMaterializedItem::CashBuy;

						nRet = CashItemDBProcess(m_cWorldSetID, m_nAccountDBID, m_biCharacterDBID, m_nMapID, m_nChannelID, m_cPaymentRules, m_VecBuyItemList[i].BuyItem, 
							m_VecBuyItemList[i].nPrice, m_biPurchaseOrderID, nCode);
						if (nRet != ERROR_NONE)
						{
							pMembershipDB->QueryModGiveFailFlag2(m_VecBuyItemList[i].biPurchaseOrderDetailID, true);
							g_Log.Log(LogType::_ERROR, this, L"[SetBillingNo(Buy)] CashItemDBQuery Fail(%d) Order(%I64d) Item(SN:%d ID:%d)\r\n", nRet, m_biPurchaseOrderID, m_VecBuyItemList[i].BuyItem.nItemSN, m_VecBuyItemList[i].BuyItem.CashItem.nItemID);
							// 일단 실패해도 return 하지말고 다 처리는 하자
						}
					}
#if defined(PRE_ADD_CASH_REFUND)
					else
					{
						__time64_t _tNow;
						time(&_tNow);
						// 결재인벤에 추가된 리스트..
						m_PaymentItem[nCount].uiOrderNo = nOrderNo;
						m_PaymentItem[nCount].nPrice = m_VecBuyItemList[i].nPrice;
						m_PaymentItem[nCount].PaymentItemInfo.biDBID = m_VecBuyItemList[i].biPurchaseOrderDetailID;
						m_PaymentItem[nCount].PaymentItemInfo.ItemInfo.nItemSN = m_VecBuyItemList[i].BuyItem.nItemSN;
						m_PaymentItem[nCount].PaymentItemInfo.ItemInfo.nItemID = m_VecBuyItemList[i].BuyItem.CashItem.nItemID;						
						m_PaymentItem[nCount].PaymentItemInfo.tPaymentDate = _tNow; // 그냥 지금 산걸로 셋팅.
						nCount++;					
					}
#endif
				}
			}

			SendBuyGift(nResult);
		}
		break;

	case CASH_GIFT:
		{
			if (m_VecGiftItemList.empty()) return;
			// m_nPickupCount++;

			if (nOrderNo > 0){
				char szData[64] = { 0, };
				memcpy(szData, &pNexonOrderID[14], strlen(pNexonOrderID) - 14);
				INT64 biOrderID = _atoi64(szData);

				if (m_VecGiftItemList[0].biPurchaseOrderID == biOrderID){
					for (int i = 0; i < (int)m_VecGiftItemList.size(); i++){
						char szOrderNo[PUBLISHERORDERKEYMAX] = {0, };
						sprintf_s(szOrderNo, "%d", nOrderNo);
						char cOrderCode = DBDNMembership::OrderStatusCode::FailPayment;
						if (nResult == ERROR_NONE){
							m_VecGiftItemList[i].GiftItem.bFail = false;
							cOrderCode = DBDNMembership::OrderStatusCode::Success;
						}

						int nRet = pMembershipDB->QuerySetPurchaseOrderResult(m_VecGiftItemList[i].biPurchaseOrderID, cOrderCode, szOrderNo, nBillingResult, m_VecGiftItemList[i].nReservePetal, m_nUsedPaidCash);
						if (nRet != ERROR_NONE){
							SendBuyGift(nRet);
							g_Log.Log(LogType::_ERROR, this, L"[SetBillingNo(Gift)] QuerySetPurchaseOrderResult Fail(%d) Order(Eye:%I64d Nexon:%u)\r\n", nResult, m_VecGiftItemList[i].biPurchaseOrderID, nOrderNo);
							return;
						}
					}
				}
			}

			SendBuyGift(nResult);
		}
		break;

	case CASH_PACKAGEGIFT:
		{
			char szOrderNo[PUBLISHERORDERKEYMAX] = {0, };
			sprintf_s(szOrderNo, "%d", nOrderNo);
			char cOrderCode = DBDNMembership::OrderStatusCode::FailPayment;
			if (nResult == ERROR_NONE)
				cOrderCode = DBDNMembership::OrderStatusCode::Success;

			int nRet = pMembershipDB->QuerySetPurchaseOrderResult(m_biPurchaseOrderID, cOrderCode, szOrderNo, nBillingResult, g_pExtManager->GetCashCommodityReserve(m_nPackageSN), m_nUsedPaidCash);
			if (nRet != ERROR_NONE){
				SendBuyGift(nRet);
				g_Log.Log(LogType::_ERROR, this, L"[SetBillingNo(PackageGift)] QuerySetPurchaseOrderResult Fail(%d) Order(Eye:%I64d Nexon:%u)\r\n", nResult, m_biPurchaseOrderID, nOrderNo);
				return;
			}

			SendBuyGift(nResult);
		}
		break;
	}
}

#if defined(_CH) || defined(_EU)
int CDNUserRepository::GetTotalPetalBuyPartList()
{
	int nTotal = 0;

	if (GetBuyItemPart()){
		for (int i = 0; i < MAX_GOOD_LEN; i++){
			if (!GetBuyItemPart()->pPartItem[i]) continue;
			nTotal += GetBuyItemPart()->pPartItem[i]->nReservePetal;
		}
	}
	return nTotal;
}

void CDNUserRepository::SetSuccessBuyPartList()
{
	if (GetBuyItemPart()){
		for (int i = 0; i < MAX_GOOD_LEN; i++){
			if (!GetBuyItemPart()->pPartItem[i]) continue;
			GetBuyItemPart()->pPartItem[i]->BuyItem.bFail = false;
		}
	}
}

void CDNUserRepository::SetOrderStepBuyPartList(int nOrderStep)
{
	if (GetBuyItemPart()){
		GetBuyItemPart()->nOrderStep = nOrderStep;
	}
}

void CDNUserRepository::SetSuccessGiftPartList()
{
	if (GetGiftItemPart()){
		for (int i = 0; i < MAX_GOOD_LEN; i++){
			if (!GetGiftItemPart()->pPartItem[i]) continue;
			GetGiftItemPart()->pPartItem[i]->GiftItem.bFail = false;
		}
	}
}

void CDNUserRepository::SetOrderStepGiftPartList(int nOrderStep)
{
	if (GetGiftItemPart()){
		GetGiftItemPart()->nOrderStep = nOrderStep;
	}
}

void CDNUserRepository::OnRecvAccountLock(int nBillingResult, char *pBillingContextID)
{
#if defined (PRE_ADD_REJECT_DUPLICATED_CASH_REQUEST)
	CRequestReleaser releaser(m_Requesting);
#endif // #if defined (PRE_ADD_REJECT_DUPLICATED_CASH_REQUEST)

	int nResult = g_pBillingConnection->ConvertResult(nBillingResult);

	switch(m_nSubCommand)
	{
	case CASH_BUY:
		{
			if (strncmp(GetContextId(), pBillingContextID, GetContextIdCnt()))		// pBillingOrderID 는 AccountLock / AccountUnlock 시 마다 변하므로 대신 pBillingContextID 를 사용
				break;

			if (SNDAORDERSTEP_WORK != GetOrderStep())
				break;

			CDNBillingConnectionCH::TGoodsInfo Goods[MAX_GOOD_LEN] = { 0, };
			int nCount = 0;

			int nRet = ERROR_DB;
			if (nResult == ERROR_NONE){
				g_pBillingConnection->GetBuyGoodsInfo(GetBuyItemPart(), Goods, nCount);

				nRet = g_pBillingConnection->SendAccountUnlock(this, true, pBillingContextID, Goods, nCount, false);
				if (nRet >= ERROR_NONE) break;
			}

			g_Log.Log(LogType::_ERROR, this, L"[OnRecvAccountLock:CASH_BUY] nRet:%d nResult:%d\r\n", nRet, nResult);	// 20100621

			SetOrderStepBuyPartList(SNDAORDERSTEP_FAIL);
			m_nPartCount++;

			int nPartListMax = (int)m_VecBuyItemPartList.size();
			for (int i = m_nPartCount; i < nPartListMax; i++){
				memset(&Goods, 0, sizeof(Goods));
				nCount = 0;
				g_pBillingConnection->GetBuyGoodsInfo(GetBuyItemPart(), Goods, nCount);

				nRet = g_pBillingConnection->SendAccountLock(this, Goods, nCount, false);
				if (nRet >= 0) break;

				g_Log.Log(LogType::_ERROR, this, L"[OnRecvAccountLock:CASH_BUY] SendAccountLock() nRet : %d\r\n", nRet);	// 20100621

				SetOrderStepBuyPartList(SNDAORDERSTEP_FAIL);
				m_nPartCount++;
			}

			if (m_nPartCount == nPartListMax) {
				SendBuyGift(nResult);
			}
#if defined (PRE_ADD_REJECT_DUPLICATED_CASH_REQUEST)
			else
				releaser.Lock();
#endif // #if defined (PRE_ADD_REJECT_DUPLICATED_CASH_REQUEST)
		}
		break;

	case CASH_PACKAGEBUY:
		{
			if (strncmp(GetContextId(), pBillingContextID, GetContextIdCnt()))		// pBillingOrderID 는 AccountLock / AccountUnlock 시 마다 변하므로 대신 pBillingContextID 를 사용
				break;

			if (ERROR_NONE != nResult){
				SendBuyGift(nResult);
				break;
			}

			CDNBillingConnectionCH::TGoodsInfo Goods[MAX_GOOD_LEN] = { 0, };

			Goods[0].nItemSN = m_nPackageSN;
			Goods[0].nPrice = g_pExtManager->GetCashCommodityPrice(m_nPackageSN);

			int nRet = g_pBillingConnection->SendAccountUnlock(this, true, pBillingContextID, Goods, 1, false);
			if (0 > nRet) {
				g_Log.Log(LogType::_ERROR, this, L"[OnRecvAccountLock:CASH_PACKAGEBUY] SendAccountUnlock() nRet: %d\r\n", nRet);	// 20100621
			}
		}
		break;

	case CASH_GIFT:
		{
			if (strncmp(GetContextId(), pBillingContextID, GetContextIdCnt()))		// pBillingOrderID 는 AccountLock / AccountUnlock 시 마다 변하므로 대신 pBillingContextID 를 사용
				break;

			if (SNDAORDERSTEP_WORK != GetOrderStep())
				break;

			CDNBillingConnectionCH::TGoodsInfo Goods[MAX_GOOD_LEN] = { 0, };
			int nCount = 0;

			int nRet = ERROR_DB;
			if (nResult == ERROR_NONE){
				g_pBillingConnection->GetGiftGoodsInfo(GetGiftItemPart(), Goods, nCount);

				nRet = g_pBillingConnection->SendAccountUnlock(this, true, pBillingContextID, Goods, nCount, true);
				if (nRet >= ERROR_NONE) break;
			}

			g_Log.Log(LogType::_ERROR, this, L"[OnRecvAccountLock:CASH_GIFT] nRet:%d\r\n", nRet);	// 20100621

			SetOrderStepGiftPartList(SNDAORDERSTEP_FAIL);
			m_nPartCount++;

			int nPartListMax = (int)m_VecGiftItemPartList.size();
			for (int i = m_nPartCount; i < nPartListMax; i++){
				memset(&Goods, 0, sizeof(Goods));
				nCount = 0;
				g_pBillingConnection->GetGiftGoodsInfo(GetGiftItemPart(), Goods, nCount);

				nRet = g_pBillingConnection->SendAccountLock(this, Goods, nCount, true);
				if (nRet >= 0) break;

				g_Log.Log(LogType::_ERROR, this, L"[OnRecvAccountLock:CASH_BUY] SendAccountLock() nRet : %d\r\n", nRet);	// 20100621

				SetOrderStepGiftPartList(SNDAORDERSTEP_FAIL);
				m_nPartCount++;
			}

			if (m_nPartCount == nPartListMax) {
				SendBuyGift(nResult);
			}
#if defined (PRE_ADD_REJECT_DUPLICATED_CASH_REQUEST)
			else
				releaser.Lock();
#endif // #if defined (PRE_ADD_REJECT_DUPLICATED_CASH_REQUEST)
		}
		break;

	case CASH_PACKAGEGIFT:
		{
			if (strncmp(GetContextId(), pBillingContextID, GetContextIdCnt())) {		// pBillingOrderID 는 AccountLock / AccountUnlock 시 마다 변하므로 대신 pBillingContextID 를 사용
				break;
			}

			if (ERROR_NONE != nResult){
				SendBuyGift(nResult);
				break;
			}

			CDNBillingConnectionCH::TGoodsInfo Goods[MAX_GOOD_LEN] = { 0, };

			Goods[0].nItemSN = m_nPackageSN;
			Goods[0].nPrice = g_pExtManager->GetCashCommodityPrice(m_nPackageSN);

			int nRet = g_pBillingConnection->SendAccountUnlock(this, true, pBillingContextID, Goods, 1, true);
			if (0 > nRet) {
				g_Log.Log(LogType::_ERROR, this, L"[OnRecvAccountLock:CASH_PACKAGEBUY] SendAccountUnlock() nRet : %d\r\n", nRet);	// 20100621
			}
		}
		break;

	case CASH_VIPBUY:
	case CASH_VIPGIFT:
		{
			if (strncmp(GetContextId(), pBillingContextID, GetContextIdCnt())) {	// pBillingOrderID 는 AccountLock / AccountUnlock 시 마다 변하므로 대신 pBillingContextID 를 사용
				break;
			}

			if (ERROR_NONE != nResult){
				SendVIPBuyGift(nResult, 0);
				break;
			}

			CDNBillingConnectionCH::TGoodsInfo Goods[MAX_GOOD_LEN] = { 0, };

			Goods[0].nItemSN = m_VIP.nItemSN;
			Goods[0].nPrice = m_VIP.nPrice;

			bool bGift = false;
			if (m_nSubCommand == CASH_VIPGIFT)
				bGift = true;

			int nRet = g_pBillingConnection->SendAccountUnlock(this, true, pBillingContextID, Goods, 1, bGift);
			if (0 > nRet) {
				g_Log.Log(LogType::_ERROR, this, L"[OnRecvAccountLock:%d] SendAccountUnlock() nRet : %d\r\n", m_nSubCommand, nRet);	// 20100621
			}
		}
		break;

	default:
		break;
	}
}

void CDNUserRepository::OnRecvAccountUnlock(int nBillingResult, char *pBillingContextID, char *pBillingOrderID, int nBalance)
{
#if defined (PRE_ADD_REJECT_DUPLICATED_CASH_REQUEST)
	CRequestReleaser releaser(m_Requesting);
#endif // #if defined (PRE_ADD_REJECT_DUPLICATED_CASH_REQUEST)

	CDNSQLMembership *pMembershipDB = GetMembershipDB();
	if (!pMembershipDB){
		SendBuyGift(ERROR_DB);
		return;
	}

	int nResult = g_pBillingConnection->ConvertResult(nBillingResult);

	switch(m_nSubCommand)
	{
	case CASH_BUY:
		{
			if (m_VecBuyItemPartList.empty()) return;
			if (!GetBuyItemPart()) return;

			if (strncmp(GetContextId(), pBillingContextID, GetContextIdCnt())) {		// pBillingOrderID 는 AccountLock / AccountUnlock 시 마다 변하므로 대신 pBillingContextID 를 사용
				break;
			}

			if (SNDAORDERSTEP_WORK != GetOrderStep()) {
				break;
			}

			char szOrderKey[PUBLISHERORDERKEYMAX] = {0, };
			sprintf_s(szOrderKey, "%s", pBillingOrderID);			// 20100809 중국에 맞게 수정됨
			char szContextKey[PUBLISHERORDERKEYMAX] = {0, };
			sprintf_s(szContextKey, "%s", pBillingContextID);		// 20100809 중국에 맞게 수정됨

			char cOrderCode = DBDNMembership::OrderStatusCode::FailPayment;
			if (ERROR_NONE == nResult) {
				SetSuccessBuyPartList();
				cOrderCode = DBDNMembership::OrderStatusCode::Success;
			}

			int nRetCode = pMembershipDB->QuerySetPurchaseOrderResult(GetBuyItemPart()->biPurchaseOrderID, cOrderCode, szOrderKey, nBillingResult, GetTotalPetalBuyPartList(), m_nUsedPaidCash, szContextKey);
			if (ERROR_NONE != nRetCode) {
				SendBuyGift(nRetCode);
				SetOrderStepBuyPartList(SNDAORDERSTEP_FAIL);

				g_Log.Log(LogType::_ERROR, this, L"[OnRecvAccountUnlock:CASH_BUY] QuerySetPurchaseOrderResult Fail(%d) Order(Eye:%I64d Shanda:%S)\r\n", 
					nRetCode, GetPurchaseOrderID(), pBillingContextID);	// pBillingOrderID 는 AccountLock / AccountUnlock 시 마다 변하므로 대신 pBillingContextID 를 사용
				return;
			}

			if (GetBuyItemPart()){
				SetOrderStepBuyPartList(SNDAORDERSTEP_SUCCESS);

				for (int i = 0; i < MAX_GOOD_LEN; i++){
					if (!GetBuyItemPart()->pPartItem[i]) continue;

					int nCode = DBDNWorldDef::AddMaterializedItem::CashBuy;

					nRetCode = CashItemDBProcess(m_cWorldSetID, m_nAccountDBID, m_biCharacterDBID, m_nMapID, m_nChannelID, m_cPaymentRules, GetBuyItemPart()->pPartItem[i]->BuyItem, 
						GetBuyItemPart()->pPartItem[i]->nPrice, GetBuyItemPart()->biPurchaseOrderID, nCode);

					if (ERROR_NONE != nRetCode) {
						pMembershipDB->QueryModGiveFailFlag2(GetBuyItemPart()->pPartItem[i]->biPurchaseOrderDetailID, true);
					}
				}
			}

			m_nPartCount++;

			CDNBillingConnectionCH::TGoodsInfo Goods[MAX_GOOD_LEN] = { 0, };
			int nCount = 0;

			int nPartListMax = (int)m_VecBuyItemPartList.size();
			for (int i = m_nPartCount; i < nPartListMax; i++){
				memset(&Goods, 0, sizeof(Goods));
				nCount = 0;
				g_pBillingConnection->GetBuyGoodsInfo(GetBuyItemPart(), Goods, nCount);

				nRetCode = g_pBillingConnection->SendAccountLock(this, Goods, nCount, false);
				if (nRetCode >= 0) break;

				g_Log.Log(LogType::_ERROR, this, L"[OnRecvAccountUnlock:CASH_BUY] SendAccountLock() nRetCode : %d\r\n", nRetCode);	// 20100621

				SetOrderStepBuyPartList(SNDAORDERSTEP_FAIL);
				m_nPartCount++;
			}

			if (m_nPartCount == nPartListMax) {
				SendBuyGift(nResult);
			}
#if defined (PRE_ADD_REJECT_DUPLICATED_CASH_REQUEST)
			else
				releaser.Lock();
#endif // #if defined (PRE_ADD_REJECT_DUPLICATED_CASH_REQUEST)
		}
		break;

	case CASH_PACKAGEBUY:
		{
			if (strncmp(GetContextId(), pBillingContextID, GetContextIdCnt()))		// pBillingOrderID 는 AccountLock / AccountUnlock 시 마다 변하므로 대신 pBillingContextID 를 사용
				break;

			char szOrderKey[PUBLISHERORDERKEYMAX] = {0, };
			sprintf_s(szOrderKey, "%s", pBillingOrderID);			// 20100809 중국에 맞게 수정됨
			char szContextKey[PUBLISHERORDERKEYMAX] = {0, };
			sprintf_s(szContextKey, "%s", pBillingContextID);		// 20100809 중국에 맞게 수정됨

			char cOrderCode = DBDNMembership::OrderStatusCode::FailPayment;
			if (nResult == ERROR_NONE)
				cOrderCode = DBDNMembership::OrderStatusCode::Success;

			int nRet = pMembershipDB->QuerySetPurchaseOrderResult(GetPurchaseOrderID(), cOrderCode, szOrderKey, nBillingResult, g_pExtManager->GetCashCommodityReserve(GetPackageSN()), m_nUsedPaidCash, szContextKey);
			if (nRet != ERROR_NONE){
				g_Log.Log(LogType::_ERROR, this, L"[OnRecvAccountUnlock(Package)] CASH_PACKAGEBUY QuerySetPurchaseOrderResult Fail(%d) Order(Eye:%I64d Shanda:%S)\r\n", 
					nRet, GetPurchaseOrderID(), pBillingContextID);	// pBillingOrderID 는 AccountLock / AccountUnlock 시 마다 변하므로 대신 pBillingContextID 를 사용
				SendBuyGift(nRet);
				return;
			}

			if (nResult == ERROR_NONE){
				CalcPackageProcess();
			}

			SendBuyGift(nResult);
		}
		break;

	case CASH_GIFT:
		{
			if (m_VecGiftItemPartList.empty()) return;
			if (!GetGiftItemPart()) return;

			if (strncmp(GetContextId(), pBillingContextID, GetContextIdCnt())) {		// pBillingOrderID 는 AccountLock / AccountUnlock 시 마다 변하므로 대신 pBillingContextID 를 사용
				break;
			}

			if (SNDAORDERSTEP_WORK != GetOrderStep()) {
				break;
			}

			char szOrderKey[PUBLISHERORDERKEYMAX] = {0, };
			sprintf_s(szOrderKey, "%s", pBillingOrderID);			// 20100809 중국에 맞게 수정됨
			char szContextKey[PUBLISHERORDERKEYMAX] = {0, };
			sprintf_s(szContextKey, "%s", pBillingContextID);		// 20100809 중국에 맞게 수정됨

			char cOrderCode = DBDNMembership::OrderStatusCode::FailPayment;
			if (ERROR_NONE == nResult) {
				SetSuccessGiftPartList();
				cOrderCode = DBDNMembership::OrderStatusCode::Success;
			}

			for (int i = 0; i < MAX_GOOD_LEN; i++){
				if (!GetGiftItemPart()->pPartItem[i]) continue;

				int nRet = pMembershipDB->QuerySetPurchaseOrderResult(GetGiftItemPart()->pPartItem[i]->biPurchaseOrderID, cOrderCode, szOrderKey, nBillingResult, GetGiftItemPart()->pPartItem[i]->nReservePetal, m_nUsedPaidCash, szContextKey);
				if (ERROR_NONE != nRet) {
					g_Log.Log(LogType::_ERROR, this, L"[OnRecvAccountUnlock] CASH_GIFT QuerySetPurchaseOrderResult Fail(%d) Order(Eye:%I64d Shanda:%S)\r\n", 
						nRet, GetGiftItemPart()->pPartItem[i]->biPurchaseOrderID, pBillingContextID);	// pBillingOrderID 는 AccountLock / AccountUnlock 시 마다 변하므로 대신 pBillingContextID 를 사용
					SendBuyGift(nRet);
					return;
				}
			}
			m_nPartCount++;

			CDNBillingConnectionCH::TGoodsInfo Goods[MAX_GOOD_LEN] = { 0, };
			int nCount = 0;

			int nPartListMax = (int)m_VecGiftItemPartList.size();
			for (int i = m_nPartCount; i < nPartListMax; i++){
				memset(&Goods, 0, sizeof(Goods));
				nCount = 0;
				g_pBillingConnection->GetGiftGoodsInfo(GetGiftItemPart(), Goods, nCount);

				int nRetCode = g_pBillingConnection->SendAccountLock(this, Goods, nCount, true);
				if (nRetCode >= 0) break;

				g_Log.Log(LogType::_ERROR, this, L"[OnRecvAccountUnlock:CASH_GIFT] SendAccountLock() nRetCode : %d\r\n", nRetCode);	// 20100621

				SetOrderStepGiftPartList(SNDAORDERSTEP_FAIL);
				m_nPartCount++;
			}

			if (m_nPartCount == nPartListMax) {
				SendBuyGift(nResult);
			}
#if defined (PRE_ADD_REJECT_DUPLICATED_CASH_REQUEST)
			else
				releaser.Lock();
#endif // #if defined (PRE_ADD_REJECT_DUPLICATED_CASH_REQUEST)
		}
		break;

	case CASH_PACKAGEGIFT:
		{
			if (strncmp(GetContextId(), pBillingContextID, GetContextIdCnt()))		// pBillingOrderID 는 AccountLock / AccountUnlock 시 마다 변하므로 대신 pBillingContextID 를 사용
				break;

			char szOrderKey[PUBLISHERORDERKEYMAX] = {0, };
			sprintf_s(szOrderKey, "%s", pBillingOrderID);			// 20100809 중국에 맞게 수정됨
			char szContextKey[PUBLISHERORDERKEYMAX] = {0, };
			sprintf_s(szContextKey, "%s", pBillingContextID);		// 20100809 중국에 맞게 수정됨

			char cOrderCode = DBDNMembership::OrderStatusCode::FailPayment;
			if (nResult == ERROR_NONE) cOrderCode = DBDNMembership::OrderStatusCode::Success;

			int nRet = pMembershipDB->QuerySetPurchaseOrderResult(GetPurchaseOrderID(), cOrderCode, szOrderKey, nBillingResult, g_pExtManager->GetCashCommodityReserve(GetPackageSN()), m_nUsedPaidCash, szContextKey);
			if (nRet != ERROR_NONE){
				g_Log.Log(LogType::_ERROR, this, L"[OnRecvAccountUnlock(CASH_PACKAGEGIFT)] QuerySetPurchaseOrderResult Fail(%d) Order(Eye:%I64d Shanda:%S)\r\n", 
					nRet, GetPurchaseOrderID(), pBillingContextID);	// pBillingOrderID 는 AccountLock / AccountUnlock 시 마다 변하므로 대신 pBillingContextID 를 사용
				SendBuyGift(nRet);
				return;
			}

			SendBuyGift(nResult);
		}
		break;

	case CASH_VIPBUY:
	case CASH_VIPGIFT:
		{
			if (strncmp(GetContextId(), pBillingContextID, GetContextIdCnt()))	// pBillingOrderID 는 AccountLock / AccountUnlock 시 마다 변하므로 대신 pBillingContextID 를 사용
				break;

			char szOrderKey[PUBLISHERORDERKEYMAX] = {0, };
			sprintf_s(szOrderKey, "%s", pBillingOrderID);
			char szContextKey[PUBLISHERORDERKEYMAX] = {0, };
			sprintf_s(szContextKey, "%s", pBillingContextID);

			char cOrderCode = DBDNMembership::OrderStatusCode::FailPayment;
			if (nResult == ERROR_NONE) cOrderCode = DBDNMembership::OrderStatusCode::Success;

			int nRet = pMembershipDB->QuerySetPurchaseOrderResult(m_VIP.biPurchaseOrderID, cOrderCode, szOrderKey, nBillingResult, g_pExtManager->GetCashCommodityReserve(m_VIP.nItemSN), m_nUsedPaidCash, szContextKey);
			if (nRet != ERROR_NONE){
				g_Log.Log(LogType::_ERROR, this, L"[OnRecvAccountUnlock(CASH_VIPBUY)] QuerySetPurchaseOrderResult Fail(%d) Order(Eye:%I64d Shanda:%S)\r\n", 
					nRet, m_VIP.biPurchaseOrderID, pBillingContextID);	// pBillingOrderID 는 AccountLock / AccountUnlock 시 마다 변하므로 대신 pBillingContextID 를 사용
				SendVIPBuyGift(nRet, 0);
				return;
			}

			SendVIPBuyGift(nResult, nBalance);
		}
		break;

	default:
		break;
	}
}
#endif	// #if defined(_CH) || defined(_EU)

// Connection으로 Send하기..
void CDNUserRepository::SendBalanceInquiry(int nResult, int nCashBalance, int nNotRefundableBalance/* = 0*/)
{
	if (!m_pConnection) return;
	m_pConnection->SendBalanceInquiry(nResult, m_nAccountDBID, nCashBalance, GetPetalBalance(), m_bOpen, m_bServer, nNotRefundableBalance);
}

#ifdef PRE_ADD_LIMITED_CASHITEM
void CDNUserRepository::SendBuyGift(int nResult, bool bUpdateLimitedCount)
#else		//#ifdef PRE_ADD_LIMITED_CASHITEM
void CDNUserRepository::SendBuyGift(int nResult)
#endif		//#ifdef PRE_ADD_LIMITED_CASHITEM
{
	//if (ERROR_NONE != nResult)  return;

	if (!m_pConnection) return;

#ifdef PRE_ADD_LIMITED_CASHITEM
	if (g_pLimitedCashItemRepository && bUpdateLimitedCount)
	{
		std::vector<LimitedCashItem::TLimitedQuantityCashItem> vList;
		if (g_pLimitedCashItemRepository->GetLimitedItemList(vList))
			m_pConnection->SendLimiteItemInfoList(vList);
	}
#endif		//#ifdef PRE_ADD_LIMITED_CASHITEM

	INT64 nSeedPoint = 0;
#if defined( PRE_ADD_NEW_MONEY_SEED )
	nSeedPoint = GetSeedPoint();
#endif

	switch (m_nSubCommand)
	{
	case CASH_PACKAGEBUY:
		{
#if defined(PRE_ADD_CASH_REFUND)
#if defined(PRE_ADD_SALE_COUPON)
			m_pConnection->SendPackageBuy(m_nAccountDBID, nResult, GetPetalBalance(), m_nPackageSN, m_cProductCount, m_ProductPackageList, &m_PaymentPackageItem, m_cPaymentRules, m_biSaleCouponSerial, nSeedPoint);
#else
			m_pConnection->SendPackageBuy(m_nAccountDBID, nResult, GetPetalBalance(), m_nPackageSN, m_cProductCount, m_ProductPackageList, &m_PaymentPackageItem, m_cPaymentRules, nSeedPoint);
#endif // #if defined(PRE_ADD_SALE_COUPON)
#else // #if defined(PRE_ADD_CASH_REFUND)
#if defined(PRE_ADD_SALE_COUPON)
			m_pConnection->SendPackageBuy(m_nAccountDBID, nResult, GetPetalBalance(), m_nPackageSN, m_cProductCount, m_ProductPackageList, NULL, m_cPaymentRules, m_biSaleCouponSerial, nSeedPoint);
#else
			m_pConnection->SendPackageBuy(m_nAccountDBID, nResult, GetPetalBalance(), m_nPackageSN, m_cProductCount, m_ProductPackageList, NULL, m_cPaymentRules, nSeedPoint);
#endif // #if defined(PRE_ADD_SALE_COUPON)
#endif //#if defined(PRE_ADD_CASH_REFUND)
		}
		break;

	case CASH_BUY:
		{
			TCashBuyItem CashBuyItem[PREVIEWCARTLISTMAX];
			memset(&CashBuyItem, 0, sizeof(CashBuyItem));
			for (int i = 0; i < (int)m_VecBuyItemList.size(); i++)
			{
				CashBuyItem[i] = m_VecBuyItemList[i].BuyItem;
			}
#if defined(PRE_ADD_CASH_REFUND)
#if defined(PRE_ADD_SALE_COUPON)
			m_pConnection->SendBuy(m_nAccountDBID, nResult, m_cBuyCartType, GetPetalBalance(), m_cProductCount, CashBuyItem, m_PaymentItem, m_cPaymentRules, m_biSaleCouponSerial, nSeedPoint);
#else
			m_pConnection->SendBuy(m_nAccountDBID, nResult, m_cBuyCartType, GetPetalBalance(), m_cProductCount, CashBuyItem, m_PaymentItem, m_cPaymentRules, nSeedPoint);
#endif //#if defined(PRE_ADD_SALE_COUPON)
#else //#if defined(PRE_ADD_CASH_REFUND)
#if defined(PRE_ADD_SALE_COUPON)
			m_pConnection->SendBuy(m_nAccountDBID, nResult, m_cBuyCartType, GetPetalBalance(), m_cProductCount, CashBuyItem, NULL, m_cPaymentRules, m_biSaleCouponSerial, nSeedPoint);
#else
			m_pConnection->SendBuy(m_nAccountDBID, nResult, m_cBuyCartType, GetPetalBalance(), m_cProductCount, CashBuyItem, NULL, m_cPaymentRules, nSeedPoint);
#endif //#if defined(PRE_ADD_SALE_COUPON)
#endif //#if defined(PRE_ADD_CASH_REFUND)
		}
		break;

	case CASH_PACKAGEGIFT:
		{
			if (nResult != ERROR_NONE){
#if defined(PRE_ADD_CADGE_CASH)
				m_pConnection->SendPackageGift(nResult, m_nAccountDBID, 0, (INT64)0, 0, 0, m_nPackageSN, m_cProductCount, m_GiftPackageList, m_nMailDBID, nSeedPoint);
#else	// #if defined(PRE_ADD_CADGE_CASH)
				m_pConnection->SendPackageGift(nResult, m_nAccountDBID, 0, (INT64)0, 0, 0, m_nPackageSN, m_cProductCount, m_GiftPackageList, 0, nSeedPoint);
#endif	// #if defined(PRE_ADD_CADGE_CASH)
			}
			else{
				CDNSQLMembership *pMembershipDB = GetMembershipDB();
				if (!pMembershipDB){
#if defined(PRE_ADD_CADGE_CASH)
					m_pConnection->SendPackageGift(nResult, m_nAccountDBID, 0, (INT64)0, 0, 0, m_nPackageSN, m_cProductCount, m_GiftPackageList, m_nMailDBID, nSeedPoint);
#else	// #if defined(PRE_ADD_CADGE_CASH)
					m_pConnection->SendPackageGift(nResult, m_nAccountDBID, 0, (INT64)0, 0, 0, m_nPackageSN, m_cProductCount, m_GiftPackageList, 0, nSeedPoint);
#endif	// #if defined(PRE_ADD_CADGE_CASH)
					return;
				}
				CDNSQLWorld *pWorldDB = GetWorldDB();
				if (!pWorldDB){
#if defined(PRE_ADD_CADGE_CASH)
					m_pConnection->SendPackageGift(nResult, m_nAccountDBID, 0, (INT64)0, 0, 0, m_nPackageSN, m_cProductCount, m_GiftPackageList, m_nMailDBID, nSeedPoint);
#else	// #if defined(PRE_ADD_CADGE_CASH)
					m_pConnection->SendPackageGift(nResult, m_nAccountDBID, 0, (INT64)0, 0, 0, m_nPackageSN, m_cProductCount, m_GiftPackageList, 0, nSeedPoint);
#endif	// #if defined(PRE_ADD_CADGE_CASH)
					return;
				}

				UINT nReceiverAccountDBID = 0;
				INT64 biReceiverCharacterDBID = 0;
				pWorldDB->QueryGetCharacterPartialy7(m_wszReceiverCharacterName, nReceiverAccountDBID, biReceiverCharacterDBID);

				int nGiftCount = 0;
				pMembershipDB->QueryGetCountNotReceivedGift(biReceiverCharacterDBID, nGiftCount);

#if defined(PRE_ADD_CADGE_CASH)
				m_pConnection->SendPackageGift(nResult, m_nAccountDBID, nReceiverAccountDBID, biReceiverCharacterDBID, nGiftCount, GetPetalBalance(), m_nPackageSN, m_cProductCount, m_GiftPackageList, m_nMailDBID, nSeedPoint);
#else	// #if defined(PRE_ADD_CADGE_CASH)
				m_pConnection->SendPackageGift(nResult, m_nAccountDBID, nReceiverAccountDBID, biReceiverCharacterDBID, nGiftCount, GetPetalBalance(), m_nPackageSN, m_cProductCount, m_GiftPackageList, 0, nSeedPoint);
#endif	// #if defined(PRE_ADD_CADGE_CASH)
			}
		}
		break;

	case CASH_GIFT:
		{
			TCashGiftItem CashGiftList[PREVIEWCARTLISTMAX] = {0,};
			CDNSQLMembership *pMembershipDB = GetMembershipDB();
			if (!pMembershipDB){
#if defined(PRE_ADD_CADGE_CASH)
				m_pConnection->SendGift(ERROR_DB, m_nAccountDBID, 0, (INT64)0, 0, 0, m_cGiftCartType, m_cProductCount, CashGiftList, m_nMailDBID, 0);
#else	// #if defined(PRE_ADD_CADGE_CASH)
				m_pConnection->SendGift(ERROR_DB, m_nAccountDBID, 0, (INT64)0, 0, 0, m_cGiftCartType, m_cProductCount, CashGiftList, 0, 0);
#endif	// #if defined(PRE_ADD_CADGE_CASH)
				return;
			}
			CDNSQLWorld *pWorldDB = GetWorldDB();
			if (!pWorldDB){
#if defined(PRE_ADD_CADGE_CASH)
				m_pConnection->SendGift(ERROR_DB, m_nAccountDBID, 0, (INT64)0, 0, 0, m_cGiftCartType, m_cProductCount, CashGiftList, m_nMailDBID, 0);
#else	// #if defined(PRE_ADD_CADGE_CASH)
				m_pConnection->SendGift(ERROR_DB, m_nAccountDBID, 0, (INT64)0, 0, 0, m_cGiftCartType, m_cProductCount, CashGiftList, 0, 0);
#endif	// #if defined(PRE_ADD_CADGE_CASH)
				return;
			}

			for (int i = 0; i < (int)m_VecGiftItemList.size(); i++){
				CashGiftList[i] = m_VecGiftItemList[i].GiftItem;
			}

			if (nResult != ERROR_NONE){
#if defined(PRE_ADD_CADGE_CASH)
				m_pConnection->SendGift(nResult, m_nAccountDBID, 0, (INT64)0, 0, GetPetalBalance(), m_cGiftCartType, m_cProductCount, CashGiftList, m_nMailDBID, nSeedPoint);
#else	// #if defined(PRE_ADD_CADGE_CASH)
				m_pConnection->SendGift(nResult, m_nAccountDBID, 0, (INT64)0, 0, GetPetalBalance(), m_cGiftCartType, m_cProductCount, CashGiftList, 0, nSeedPoint);
#endif	// #if defined(PRE_ADD_CADGE_CASH)
			}
			else{
				UINT nReceiverAccountDBID = 0;
				INT64 biReceiverCharacterDBID = 0;
				pWorldDB->QueryGetCharacterPartialy7(m_wszReceiverCharacterName, nReceiverAccountDBID, biReceiverCharacterDBID);

				int nGiftCount = 0;
				pMembershipDB->QueryGetCountNotReceivedGift(biReceiverCharacterDBID, nGiftCount);

#if defined(PRE_ADD_CADGE_CASH)
				m_pConnection->SendGift(nResult, m_nAccountDBID, nReceiverAccountDBID, biReceiverCharacterDBID, nGiftCount, GetPetalBalance(), m_cGiftCartType, m_cProductCount, CashGiftList, m_nMailDBID, nSeedPoint);
#else	// #if defined(PRE_ADD_CADGE_CASH)
				m_pConnection->SendGift(nResult, m_nAccountDBID, nReceiverAccountDBID, biReceiverCharacterDBID, nGiftCount, GetPetalBalance(), m_cGiftCartType, m_cProductCount, CashGiftList, 0, nSeedPoint);
#endif	// #if defined(PRE_ADD_CADGE_CASH)
			}
		}
		break;
	}
}

#if defined(_TW)
void CDNUserRepository::SendCoupon(int nResult, bool bGashSend)
#else
void CDNUserRepository::SendCoupon(int nResult)
#endif // #if defined(_TW)
{
#if defined(_TW)
	if( nResult == ERROR_NONE )
	{
		g_Log.Log(LogType::_ERROR, this, L"[Coupon Success!] Coupon:%S, Serial:%d, AccName:%S, ChaName:%S \r\n", m_szCoupon, m_nCouponSN, m_szAccountName, m_szCharacterName);
	}
	else
	{
		g_Log.Log(LogType::_ERROR, this, L"[Coupon Error!] Coupon:%S, Serial:%d, AccName:%S, ChaName:%S, Result:%d \r\n", m_szCoupon, m_nCouponSN, m_szAccountName, m_szCharacterName, nResult);	
		if( bGashSend ) // 쿠폰 롤백 Send
			g_pBillingCouponRollBackConnection->SendCouponFailResult(m_szCoupon, m_nAccountDBID);
	}
#endif

	if (!m_pConnection) return;
	m_pConnection->SendCoupon(m_nAccountDBID, nResult);
}

#if defined(PRE_ADD_CASH_REFUND)
void CDNUserRepository::SendCashRefund(int nResult)
{
	if (!m_pConnection) return;
	m_pConnection->SendCashRefund(m_nAccountDBID, nResult, m_cRefundType, m_biPurchaseOrderDetailID);
}
#endif

void CDNUserRepository::SendVIPBuyGift(int nResult, int nCash)
{
	if (!m_pConnection) return;

	switch (m_nSubCommand)
	{
	case CASH_VIPBUY:
		{
			if (nResult != ERROR_NONE){
				m_pConnection->SendVIPBuy(m_nAccountDBID, nResult, 0, nCash, m_VIP.nItemSN, 0, 0, m_bAutoPay, m_bServer, 0);
			}
			else{
				CDNSQLMembership *pMembershipDB = GetMembershipDB();
				if (!pMembershipDB){
					m_pConnection->SendVIPBuy(m_nAccountDBID, ERROR_DB, 0, nCash, m_VIP.nItemSN, 0, 0, m_bAutoPay, m_bServer, 0);
					return;
				}

				int nPetal = 0;
				int nVIPPoint = 0;
				__time64_t tEndDate = 0;
				if (!m_bAutoPay){
					m_bAutoPay = g_pExtManager->GetCashCommodityPay(m_VIP.nItemSN);
				}
				int nRet = pMembershipDB->QueryIncreaseVIPBasicPoint(GetCharacterDBID(), g_pExtManager->GetCashCommodityVIPPoint(m_VIP.nItemSN), m_VIP.biPurchaseOrderID, 
					g_pExtManager->GetCashCommodityPeriod(m_VIP.nItemSN), m_bAutoPay, nVIPPoint, tEndDate);

				if (nRet != ERROR_NONE){
					pMembershipDB->QueryModGiveFailFlag(m_VIP.biPurchaseOrderID, m_VIP.nItemSN, g_pExtManager->GetCashCommodityItem0(m_VIP.nItemSN), true);
					nResult = nRet;
				}

				INT64 nSeedPoint = 0;
#if defined( PRE_ADD_NEW_MONEY_SEED )
				nSeedPoint = GetSeedPoint();
#endif
				m_pConnection->SendVIPBuy(m_nAccountDBID, nResult, GetPetalBalance(), nCash, m_VIP.nItemSN, nVIPPoint, tEndDate, m_bAutoPay, m_bServer, nSeedPoint);
			}
		}
		break;

	case CASH_VIPGIFT:
		{
			if (nResult != ERROR_NONE){
				m_pConnection->SendVIPGift(m_nAccountDBID, nResult, 0, nCash, m_VIP.nItemSN, 0, 0, 0, 0);
			}
			else{
				CDNSQLMembership *pMembershipDB = GetMembershipDB();
				if (!pMembershipDB){
					m_pConnection->SendVIPGift(m_nAccountDBID, ERROR_DB, 0, nCash, m_VIP.nItemSN, 0, 0, 0, 0);
					return;
				}
				CDNSQLWorld *pWorldDB = GetWorldDB();
				if (!pWorldDB){
					m_pConnection->SendVIPGift(m_nAccountDBID, ERROR_DB, 0, nCash, m_VIP.nItemSN, 0, 0, 0, 0);
					return;
				}

				UINT nReceiverAccountDBID = 0;
				INT64 biReceiverCharacterDBID = 0;
				pWorldDB->QueryGetCharacterPartialy7(m_wszReceiverCharacterName, nReceiverAccountDBID, biReceiverCharacterDBID);

				int nGiftCount = 0;
				pMembershipDB->QueryGetCountNotReceivedGift(biReceiverCharacterDBID, nGiftCount);

				INT64 nSeedPoint = 0;
#if defined( PRE_ADD_NEW_MONEY_SEED )
				nSeedPoint = GetSeedPoint();
#endif

				m_pConnection->SendVIPGift(m_nAccountDBID, nResult, GetPetalBalance(), nCash, m_VIP.nItemSN, nReceiverAccountDBID, biReceiverCharacterDBID, nGiftCount, nSeedPoint);
			}
		}
		break;
	}
}

void CDNUserRepository::OnRecvBillingBuyItem(int nBillingResult, UINT nOrderNo, bool bCart)
{
#if defined (PRE_ADD_REJECT_DUPLICATED_CASH_REQUEST)
	CRequestReleaser releaser(m_Requesting);
#endif // #if defined (PRE_ADD_REJECT_DUPLICATED_CASH_REQUEST)

	CDNSQLMembership *pMembershipDB = GetMembershipDB();
	if (!pMembershipDB){
		SendBuyGift(ERROR_GENERIC_INVALIDREQUEST);
		return;
	}

	char szOrderNo[PUBLISHERORDERKEYMAX] = {0, };
	sprintf_s(szOrderNo, "%d", nOrderNo);

	switch (m_nSubCommand)
	{
	case CASH_BUY:
		{
			if (m_VecBuyItemList.empty()) return;

			char cOrderCode = DBDNMembership::OrderStatusCode::FailPayment;
			if (nBillingResult == ERROR_NONE) 
				cOrderCode = DBDNMembership::OrderStatusCode::Success;			

			int nPetalAmount = 0;
#if defined(PRE_ADD_CASH_REFUND) // 패키지는 캐쉬인벤 바로가기 일때만 페탈 저장..나머지는 캐쉬인벤으로 옮길때.
			if( GetCashMoveInven() )
#endif
				nPetalAmount = m_VecBuyItemList[m_cCurProductCount].nReservePetal;

			// 참고로 대만은 감마니아 orderkey가 없음.
			int nRet = pMembershipDB->QuerySetPurchaseOrderResult(m_biPurchaseOrderID, cOrderCode, szOrderNo, nBillingResult, nPetalAmount, m_nUsedPaidCash);
			if (nRet != ERROR_NONE)
			{				
				SendBuyGift(nRet);
				g_Log.Log(LogType::_ERROR, this, L"[BillingBuyItem] QuerySetPurchaseOrderResult Fail(%d) Order(Eye:%I64d)\r\n", nBillingResult, m_biPurchaseOrderID);
				return;
			}

			if( nBillingResult == ERROR_NONE )
			{
				if( bCart )
				{
					for (int i = 0; i < (int)m_VecBuyItemList.size(); i++)
					{
						m_VecBuyItemList[i].BuyItem.bFail = false;
						int nCode = DBDNWorldDef::AddMaterializedItem::CashBuy;

						nRet = CashItemDBProcess(m_cWorldSetID, m_nAccountDBID, m_biCharacterDBID, m_nMapID, m_nChannelID, m_cPaymentRules, m_VecBuyItemList[i].BuyItem, m_VecBuyItemList[i].nPrice, m_biPurchaseOrderID, nCode);
						if (nRet != ERROR_NONE)
						{
							pMembershipDB->QueryModGiveFailFlag2(m_VecBuyItemList[i].biPurchaseOrderDetailID, true);
							g_Log.Log(LogType::_ERROR, this, L"[BillingBuyItem] CashItemDBQuery Fail(%d) Order(%I64d) Item(SN:%d ID:%d)\r\n", nRet, m_biPurchaseOrderID, m_VecBuyItemList[i].BuyItem.nItemSN, m_VecBuyItemList[i].BuyItem.CashItem.nItemID);
							// 일단 실패해도 return 하지말고 다 처리는 하자
						}
					}

#if defined(_RU)  || defined(_KAMO)  //[OK_Cash] 
					int nCashOutgoID = 0, nCashBalance = 0;
					int nRetDeductCash = pMembershipDB->QueryDeductCash(m_nAccountDBID, m_biPurchaseOrderID, m_nTotalPrice, nCashOutgoID, nCashBalance); //侶쟁렝岺질듐뭔찜

					if (ERROR_NONE == nRetDeductCash)
					{
						SendBuyGift(nBillingResult);  //있떴듐뭔찜냥묘
					} 
					else
					{
						printf("[Err]pMembershipDB->QueryDeductCash AccountID=%d Ret=%d,TotalPrice=%d,CashBalance=%d",m_nAccountDBID,nRetDeductCash,m_nTotalPrice,nCashBalance);
						//SendBuyGift(nRetDeductCash);  //있떴듐뭔찜呵겨，옵콘角질듐
						SendBuyGift(ERROR_GENERIC_INVALIDREQUEST);  //@_@
					}

#elif defined(_WORK)  
		//SendBuyGift(nBillingResult); 
#endif	
				}
				else
				{
					m_VecBuyItemList[m_cCurProductCount].BuyItem.bFail = false;

#if defined(PRE_ADD_CASH_REFUND) // 즉시적용 상품이나 환불불가능한 애들만 우선 ItemInsert..나머지는 캐쉬인벤으로 옮길때...
					if( GetCashMoveInven() || g_pExtManager->GetCashCommodityNoRefund(m_VecBuyItemList[m_cCurProductCount].BuyItem.CashItem.nItemID, m_VecBuyItemList[m_cCurProductCount].BuyItem.nItemSN) )										
#endif
					{
						int nCode = DBDNWorldDef::AddMaterializedItem::CashBuy;
						
						nRet = CashItemDBProcess(m_cWorldSetID, m_nAccountDBID, m_biCharacterDBID, m_nMapID, m_nChannelID, m_cPaymentRules, m_VecBuyItemList[m_cCurProductCount].BuyItem, m_VecBuyItemList[m_cCurProductCount].nPrice, m_biPurchaseOrderID, nCode);
						if (nRet != ERROR_NONE)
						{
							pMembershipDB->QueryModGiveFailFlag2(m_VecBuyItemList[m_cCurProductCount].biPurchaseOrderDetailID, true);
							g_Log.Log(LogType::_ERROR, this, L"[BillingBuyItem] CashItemDBQuery Fail(%d) Order(%I64d)\r\n", nRet, m_biPurchaseOrderID);						
						}
					}
#if defined(PRE_ADD_CASH_REFUND)
					else
					{
						__time64_t _tNow;
						time(&_tNow);
						// 결재인벤에 추가된 리스트..
						m_PaymentItem[m_cCurProductCount].uiOrderNo = nOrderNo;
						m_PaymentItem[m_cCurProductCount].nPrice = m_VecBuyItemList[m_cCurProductCount].nPrice;
						m_PaymentItem[m_cCurProductCount].PaymentItemInfo.biDBID = m_VecBuyItemList[m_cCurProductCount].biPurchaseOrderDetailID;
						m_PaymentItem[m_cCurProductCount].PaymentItemInfo.ItemInfo.nItemSN = m_VecBuyItemList[m_cCurProductCount].BuyItem.nItemSN;
						m_PaymentItem[m_cCurProductCount].PaymentItemInfo.ItemInfo.nItemID = m_VecBuyItemList[m_cCurProductCount].BuyItem.CashItem.nItemID;						
						m_PaymentItem[m_cCurProductCount].PaymentItemInfo.tPaymentDate = _tNow; // 그냥 지금 산걸로 셋팅.
					}
#endif

					++m_cCurProductCount;
					if( m_cCurProductCount == m_cProductCount )				
						SendBuyGift(nBillingResult);
					else
					{
						PurchaseBuyItem();
#if defined (PRE_ADD_REJECT_DUPLICATED_CASH_REQUEST)
						releaser.Lock();
#endif // #if defined (PRE_ADD_REJECT_DUPLICATED_CASH_REQUEST)
					}
				}
			}
			else
			{
				SendBuyGift(nBillingResult);
			}
		}
		break;

	case CASH_PACKAGEBUY:
		{			
			char cOrderCode = DBDNMembership::OrderStatusCode::FailPayment;
			if (nBillingResult == ERROR_NONE) cOrderCode = DBDNMembership::OrderStatusCode::Success;

			int nPetalAmount = 0;
#if defined(PRE_ADD_CASH_REFUND) // 패키지는 캐쉬인벤 바로가기 일때만 페탈 저장..나머지는 캐쉬인벤으로 옮길때.
			if( GetCashMoveInven() )
#endif
				nPetalAmount = g_pExtManager->GetCashCommodityReserve(m_nPackageSN);

			int nRet = pMembershipDB->QuerySetPurchaseOrderResult(m_biPurchaseOrderID, cOrderCode, szOrderNo, nBillingResult, nPetalAmount, m_nUsedPaidCash);
			if (nRet != ERROR_NONE){
				g_Log.Log(LogType::_ERROR, this, L"[BillingBuyItem(Package)] QuerySetPurchaseOrderResult Fail(%d) Order(Eye:%I64d)\r\n", nBillingResult, m_biPurchaseOrderID);
				SendBuyGift(nRet);
				return;
			}
			if( nBillingResult == ERROR_NONE )
			{
#if defined(PRE_ADD_CASH_REFUND) // 패키지는 캐쉬인벤 바로가기 일때만 값을 넣어주자..나머지는 캐쉬인벤으로 옮길때.
				if( !GetCashMoveInven() )
				{					
					__time64_t _tNow;
					time(&_tNow);
					m_PaymentPackageItem.uiOrderNo = nOrderNo;
					m_PaymentPackageItem.nPrice = g_pExtManager->GetCashCommodityPrice(m_nPackageSN);
					m_PaymentPackageItem.PaymentPackageItemInfo.nPackageSN = m_nPackageSN;
					m_PaymentPackageItem.PaymentPackageItemInfo.tPaymentDate = _tNow;
					m_PaymentPackageItem.PaymentPackageItemInfo.biDBID = m_biPurchaseOrderDetailID;

					for (int i = 0; i < m_cProductCount; i++)
					{
						m_PaymentPackageItem.PaymentPackageItemInfo.ItemInfoList[i].nItemSN = m_ProductPackageList[i].nItemSN;
						m_PaymentPackageItem.PaymentPackageItemInfo.ItemInfoList[i].nItemID = m_ProductPackageList[i].CashItem.nItemID;
						m_PaymentPackageItem.PaymentPackageItemInfo.ItemInfoList[i].cItemOption = m_ProductPackageList[i].CashItem.cOption;
					}
				}
				else
#endif
				{
					CalcPackageProcess();
#if defined(_RU)  || defined(_KAMO)  //[OK_Cash] 
					int nCashOutgoID = 0, nCashBalance = 0;
					pMembershipDB->QueryDeductCash(m_nAccountDBID, m_biPurchaseOrderID, m_nTotalPrice, nCashOutgoID, nCashBalance);
#endif	
				}
			}
			SendBuyGift(nBillingResult);
		}
		break;

	case CASH_GIFT :
		{
			if (m_VecGiftItemList.empty()) 
				return;

			char cOrderCode = DBDNMembership::OrderStatusCode::FailPayment;
			if (nBillingResult == ERROR_NONE)			
				cOrderCode = DBDNMembership::OrderStatusCode::Success;
			printf("%s Cash Gift Success order\n",__FUNCTION__);
			if( bCart)
			{
				for (int i = 0; i < (int)m_VecGiftItemList.size(); i++)
				{					
					char cOrderCode = DBDNMembership::OrderStatusCode::FailPayment;
					if (nBillingResult == ERROR_NONE){
						m_VecGiftItemList[i].GiftItem.bFail = false;
						cOrderCode = DBDNMembership::OrderStatusCode::Success;
					}

					int nRet = pMembershipDB->QuerySetPurchaseOrderResult(m_VecGiftItemList[i].biPurchaseOrderID, cOrderCode, szOrderNo, nBillingResult, m_VecGiftItemList[i].nReservePetal, m_nUsedPaidCash);
					if (nRet != ERROR_NONE){
						SendBuyGift(nRet);
						g_Log.Log(LogType::_ERROR, this, L"[BillingBuyItem(Gift)] QuerySetPurchaseOrderResult Fail(%d) Order(Eye:%I64d)\r\n", nBillingResult, m_VecGiftItemList[i].biPurchaseOrderID );
						return;
					}

#if defined(_RU)  || defined(_KAMO)  //[OK_Cash] 
					int nCashOutgoID = 0, nCashBalance = 0;
					int nRetDeductCash = pMembershipDB->QueryDeductCash(m_nAccountDBID, m_VecGiftItemList[i].biPurchaseOrderID, m_VecGiftItemList[i].nPrice, nCashOutgoID, nCashBalance);
#endif	
					printf("%s nRetDeductCash = %d\n", __FUNCTION__, nRetDeductCash);
				}
				SendBuyGift(nBillingResult);
			}
			else
			{
				if( nBillingResult == ERROR_NONE )
					m_VecGiftItemList[m_cCurProductCount].GiftItem.bFail = false;

				// 참고로 대만은 감마니아 orderkey가 없음.
				int nRet = pMembershipDB->QuerySetPurchaseOrderResult(m_VecGiftItemList[m_cCurProductCount].biPurchaseOrderID, cOrderCode, szOrderNo, nBillingResult, m_VecGiftItemList[m_cCurProductCount].nReservePetal, m_nUsedPaidCash);
				if (nRet != ERROR_NONE)
				{
					g_Log.Log(LogType::_ERROR, this, L"[BillingBuyItem(Gift)] QuerySetPurchaseOrderResult Fail(%d) Order(Eye:%I64d)\r\n", nBillingResult, m_VecGiftItemList[m_cCurProductCount].biPurchaseOrderID);					
				}

				++m_cCurProductCount;
				if( nBillingResult != ERROR_NONE || m_cCurProductCount == m_cProductCount )				
					SendBuyGift(nBillingResult);				
				else
				{
					PurchaseGiftItem();
#if defined (PRE_ADD_REJECT_DUPLICATED_CASH_REQUEST)
					releaser.Lock();
#endif // #if defined (PRE_ADD_REJECT_DUPLICATED_CASH_REQUEST)
				}
			}			
		}
		break;

	case CASH_PACKAGEGIFT:
		{			
			char cOrderCode = DBDNMembership::OrderStatusCode::FailPayment;
			if (nBillingResult == ERROR_NONE)
				cOrderCode = DBDNMembership::OrderStatusCode::Success;

			int nRet = pMembershipDB->QuerySetPurchaseOrderResult(m_biPurchaseOrderID, cOrderCode, szOrderNo, nBillingResult, g_pExtManager->GetCashCommodityReserve(m_nPackageSN), m_nUsedPaidCash);
			if (nRet != ERROR_NONE){
				g_Log.Log(LogType::_ERROR, this, L"[BillingBuyItem(GiftPackage)] QuerySetPurchaseOrderResult Fail(%d) Order(Eye:%I64d)\r\n", nBillingResult, m_biPurchaseOrderID);
				SendBuyGift(nRet);
				return;
			}

#if defined(_RU)  || defined(_KAMO)  //[OK_Cash] 
			int nCashOutgoID = 0, nCashBalance = 0;
			pMembershipDB->QueryDeductCash(m_nAccountDBID, m_biPurchaseOrderID, m_nTotalPrice, nCashOutgoID, nCashBalance);
#endif	
			SendBuyGift(nBillingResult);
		}
		break;	

	case CASH_VIPBUY:
	case CASH_VIPGIFT:
		{
			char cOrderCode = DBDNMembership::OrderStatusCode::FailPayment;
			if (nBillingResult == ERROR_NONE)
				cOrderCode = DBDNMembership::OrderStatusCode::Success;

			int nRet = pMembershipDB->QuerySetPurchaseOrderResult(m_VIP.biPurchaseOrderID, cOrderCode, szOrderNo, nBillingResult, g_pExtManager->GetCashCommodityReserve(m_VIP.nItemSN), m_nUsedPaidCash);
			if (nRet != ERROR_NONE){
				g_Log.Log(LogType::_ERROR, this, L"[BillingBuyItem(CASH_VIPBUY)] QuerySetPurchaseOrderResult Fail(%d) Order(Eye:%I64d)\r\n", nRet, m_VIP.biPurchaseOrderID);
				SendVIPBuyGift(nRet, 0);
				return;
			}

#if defined(_RU)  || defined(_KAMO)  //[OK_Cash] 
			int nCashOutgoID = 0, nCashBalance = 0;
			pMembershipDB->QueryDeductCash(m_nAccountDBID, m_biPurchaseOrderID, m_nTotalPrice, nCashOutgoID, nCashBalance);
#endif	

			SendVIPBuyGift(nBillingResult, 0);
		}
		break;
	}
}

int CDNUserRepository::PurchaseItemByPetal()
{
	INT64 biOrderID = 0;
#ifdef PRE_ADD_LIMITED_CASHITEM
	bool bUpdateLimitedCount = false;
#endif		//#ifdef PRE_ADD_LIMITED_CASHITEM

	CDNSQLMembership *pMembershipDB = GetMembershipDB();
	if (!pMembershipDB){
		SendBuyGift(ERROR_GENERIC_INVALIDREQUEST);
		return ERROR_GENERIC_INVALIDREQUEST;
	}

	switch (m_nSubCommand)
	{
	case CASH_BUY:
		{
			std::vector<string> VecItemString;
			AppendItemString(0, m_cProductCount, VecItemString);

			int nRet = pMembershipDB->QueryPurchaseItemByPetal(m_biCharacterDBID, m_bPCBang, VecItemString, m_nTotalPrice, m_szIp, biOrderID);
			if (nRet != ERROR_NONE){
				SendBuyGift(nRet);
				return nRet;
			}

#ifdef PRE_ADD_LIMITED_CASHITEM
			if (atoi(VecItemString[Append_QuantityLimited].c_str()) > 0)
				bUpdateLimitedCount = true;
#endif		//#ifdef PRE_ADD_LIMITED_CASHITEM

			for (int i = 0; i < m_cProductCount; i++){
				nRet = CashItemDBProcess(m_cWorldSetID, m_nAccountDBID, m_biCharacterDBID, m_nMapID, m_nChannelID, m_cPaymentRules, m_VecBuyItemList[i].BuyItem, 
					g_pExtManager->GetCashCommodityPrice(m_ProductPackageList[i].nItemSN), biOrderID, DBDNWorldDef::AddMaterializedItem::ReserveMoneyBuy);

				if (nRet != ERROR_NONE){
					SendBuyGift(nRet);
					return nRet;
				}

				m_VecBuyItemList[i].BuyItem.bFail = false;
			}

		}
		break;

	case CASH_PACKAGEBUY:
		{
			std::vector<string> VecItemString;
			AppendItemString(0, 0, VecItemString);

			int nRet = pMembershipDB->QueryPurchaseItemByPetal(m_biCharacterDBID, m_bPCBang, VecItemString, m_nTotalPrice, m_szIp, biOrderID);
			if (nRet != ERROR_NONE){
				SendBuyGift(nRet);
				return nRet;
			}

#ifdef PRE_ADD_LIMITED_CASHITEM
			if (atoi(VecItemString[Append_QuantityLimited].c_str()) > 0)
				bUpdateLimitedCount = true;
#endif		//#ifdef PRE_ADD_LIMITED_CASHITEM

			CalcPackageProcess();
		}
		break;
	}

#ifdef PRE_ADD_LIMITED_CASHITEM
	SendBuyGift(ERROR_NONE, bUpdateLimitedCount);
#else		//#ifdef PRE_ADD_LIMITED_CASHITEM
	SendBuyGift(ERROR_NONE);
#endif		//#ifdef PRE_ADD_LIMITED_CASHITEM

	return ERROR_NONE;
}

#if defined( PRE_ADD_NEW_MONEY_SEED )
int CDNUserRepository::PurchaseItemBySeed()
{
	INT64 biOrderID = 0;
#ifdef PRE_ADD_LIMITED_CASHITEM
	bool bUpdateLimitedCount = false;
#endif		//#ifdef PRE_ADD_LIMITED_CASHITEM

	CDNSQLMembership *pMembershipDB = GetMembershipDB();
	if (!pMembershipDB){
		SendBuyGift(ERROR_GENERIC_INVALIDREQUEST);
		return ERROR_GENERIC_INVALIDREQUEST;
	}

	CDNSQLWorld *pWorldDB = GetWorldDB();
	if (!pWorldDB)
	{
		SendBuyGift(ERROR_GENERIC_INVALIDREQUEST);
		return ERROR_GENERIC_INVALIDREQUEST;
	}

	switch (m_nSubCommand)
	{
	case CASH_BUY:
		{
			std::vector<string> VecItemString;
			AppendItemString(0, m_cProductCount, VecItemString);

			int nRet = pMembershipDB->QueryPurchaseItemBySeed(m_biCharacterDBID, m_bPCBang, VecItemString, m_nTotalPrice, m_szIp, biOrderID);
			if (nRet != ERROR_NONE){
				SendBuyGift(nRet);
				return nRet;
			}

#ifdef PRE_ADD_LIMITED_CASHITEM
			if (atoi(VecItemString[Append_QuantityLimited].c_str()) > 0)
				bUpdateLimitedCount = true;
#endif		//#ifdef PRE_ADD_LIMITED_CASHITEM

			nRet = UseSeedPoint();

			if( nRet != ERROR_NONE )
			{
				SendBuyGift(ERROR_GENERIC_INVALIDREQUEST);
				return ERROR_GENERIC_INVALIDREQUEST;
			}

			for (int i = 0; i < m_cProductCount; i++){
				nRet = CashItemDBProcess(m_cWorldSetID, m_nAccountDBID, m_biCharacterDBID, m_nMapID, m_nChannelID, m_cPaymentRules, m_VecBuyItemList[i].BuyItem, 
					g_pExtManager->GetCashCommodityPrice(m_ProductPackageList[i].nItemSN), biOrderID, DBDNWorldDef::AddMaterializedItem::ReserveMoneyBuy);

				if (nRet != ERROR_NONE){
					SendBuyGift(nRet);
					return nRet;
				}

				m_VecBuyItemList[i].BuyItem.bFail = false;
			}

		}
		break;

	case CASH_PACKAGEBUY:
		{
			std::vector<string> VecItemString;
			AppendItemString(0, 0, VecItemString);

			int nRet = pMembershipDB->QueryPurchaseItemBySeed(m_biCharacterDBID, m_bPCBang, VecItemString, m_nTotalPrice, m_szIp, biOrderID);
			if (nRet != ERROR_NONE){
				SendBuyGift(nRet);
				return nRet;
			}

#ifdef PRE_ADD_LIMITED_CASHITEM
			if (atoi(VecItemString[Append_QuantityLimited].c_str()) > 0)
				bUpdateLimitedCount = true;
#endif		//#ifdef PRE_ADD_LIMITED_CASHITEM
			
			nRet = UseSeedPoint();

			if( nRet != ERROR_NONE )
			{
				SendBuyGift(ERROR_GENERIC_INVALIDREQUEST);
				return ERROR_GENERIC_INVALIDREQUEST;
			}

			CalcPackageProcess();
		}
		break;
	}

	int nRet = pMembershipDB->QuerySetPurchaseOrderResult(biOrderID, 2, 0, 0, 0, 0);

#ifdef PRE_ADD_LIMITED_CASHITEM
	SendBuyGift(ERROR_NONE, bUpdateLimitedCount);
#else		//#ifdef PRE_ADD_LIMITED_CASHITEM
	SendBuyGift(ERROR_NONE);
#endif		//#ifdef PRE_ADD_LIMITED_CASHITEM

	return ERROR_NONE;
}
#endif
int CDNUserRepository::AddPurchaseOrderByCash(int nIndex)
{
#if defined(_WORK)
	return ERROR_NONE;
#endif	// 

	INT64 biOrderID = 0;
	int nRet = ERROR_DB;
#ifdef PRE_ADD_LIMITED_CASHITEM
	bool bUpdateLimitedCount = false;
#endif		//#ifdef PRE_ADD_LIMITED_CASHITEM

	CDNSQLMembership *pMembershipDB = GetMembershipDB();
	if (!pMembershipDB)
		return ERROR_GENERIC_INVALIDREQUEST;

	switch (m_nSubCommand)
	{
	case CASH_BUY:
		{
#if defined(_KR) || defined(_US) || defined(_RU) || defined(_ID) || defined(_KAMO) //[OK_Cash]	// 전체 통으로 처리 
			std::vector<string> VecItemString;
			AppendItemString(0, m_cProductCount, VecItemString);

			char szTempOrderDetails[1024] = {0, };
			nRet = pMembershipDB->QueryAddPurchaseOrderByCash(m_biCharacterDBID, m_bPCBang, VecItemString, m_nTotalPrice, m_szIp, false, NULL, NULL, biOrderID, szTempOrderDetails);
			if (nRet != ERROR_NONE)
				return nRet;

#ifdef PRE_ADD_LIMITED_CASHITEM
			if (atoi(VecItemString[Append_QuantityLimited].c_str()) > 0)
				bUpdateLimitedCount = true;
#endif		//#ifdef PRE_ADD_LIMITED_CASHITEM

			SetPurchaseOrderDetailID(szTempOrderDetails);
			SetPurchaseOrderID(biOrderID);

#elif defined(_CH) || defined(_EU)	// 5깨씩 처리 (중국만 특이함)
			int nStx = 0, nEtx = std::min<int>(MAX_GOOD_LEN, m_cProductCount);
			while(true) {
				std::vector<string> VecItemPartString;
				AppendItemString(nStx, nEtx, VecItemPartString);

				int nPrice = 0;
				for (int i = nStx; i < nEtx; i++){
					if (m_VecBuyItemList[i].BuyItem.nItemSN > 0){
						nPrice += m_VecBuyItemList[i].nPrice;
					}
				}

				char szTempOrderDetails[1024] = {0, };
				nRet = pMembershipDB->QueryAddPurchaseOrderByCash(m_biCharacterDBID, m_bPCBang, VecItemPartString, nPrice, m_szIp, false, NULL, NULL, biOrderID, szTempOrderDetails);
				if (nRet != ERROR_NONE)
					return nRet;

#ifdef PRE_ADD_LIMITED_CASHITEM
				if (atoi(VecItemString[Append_QuantityLimited].c_str()) > 0)
					bUpdateLimitedCount = true;
#endif		//#ifdef PRE_ADD_LIMITED_CASHITEM

				SetBuyItemPartByShanda(nStx, nEtx, biOrderID, szTempOrderDetails);					
				if (m_cProductCount <= nEtx) {
					break;
				}

				nStx = nEtx;
				nEtx = std::min<int>(nEtx + MAX_GOOD_LEN, m_cProductCount);
			}

#else	// #if defined(_JP) || defined(_TW) || defined(_SG) || defined(_TH)	// 1개씩 처리
			if (m_cProductCount <= nIndex) return ERROR_ITEM_FAIL;

			char szTempOrderDetail[64] = {0, };

			std::string StrRefundable;
			StrRefundable.clear();
#if defined(PRE_ADD_CASH_REFUND)
			if( m_bCashMoveInven || g_pExtManager->GetCashCommodityNoRefund(m_VecBuyItemList[nIndex].BuyItem.CashItem.nItemID, m_VecBuyItemList[nIndex].BuyItem.nItemSN) )
				StrRefundable = "0";		// 캐쉬인벤으로 바로 넣기
			else
				StrRefundable = "1";		// 결재인벤으로
#endif	// #if defined(PRE_ADD_CASH_REFUND)

#ifdef PRE_ADD_LIMITED_CASHITEM
			nRet = pMembershipDB->QueryAddPurchaseOrderByCash(m_biCharacterDBID, m_bPCBang, m_VecBuyItemList[nIndex].BuyItem.nItemSN, m_VecBuyItemList[nIndex].nPrice,
				m_VecBuyItemList[nIndex].nLimitCount, m_VecBuyItemList[nIndex].BuyItem.CashItem.nItemID, m_VecBuyItemList[nIndex].BuyItem.CashItem.cOption, g_pExtManager->GetCashLimitedItemCount(m_VecBuyItemList[nIndex].BuyItem.nItemSN), 
				m_VecBuyItemList[nIndex].nPrice, m_szIp, false, NULL, NULL, biOrderID, szTempOrderDetail, StrRefundable.c_str());
#else		//#ifdef PRE_ADD_LIMITED_CASHITEM
			nRet = pMembershipDB->QueryAddPurchaseOrderByCash(m_biCharacterDBID, m_bPCBang, m_VecBuyItemList[nIndex].BuyItem.nItemSN, m_VecBuyItemList[nIndex].nPrice,
				m_VecBuyItemList[nIndex].nLimitCount, m_VecBuyItemList[nIndex].BuyItem.CashItem.nItemID, m_VecBuyItemList[nIndex].BuyItem.CashItem.cOption,
				m_VecBuyItemList[nIndex].nPrice, m_szIp, false, NULL, NULL, biOrderID, szTempOrderDetail, StrRefundable.c_str());
#endif		//#ifdef PRE_ADD_LIMITED_CASHITEM
			if (nRet != ERROR_NONE)
				return nRet;

#ifdef PRE_ADD_LIMITED_CASHITEM
			if (g_pExtManager->GetCashLimitedItemCount(m_VecBuyItemList[nIndex].BuyItem.nItemSN) > 0)
				bUpdateLimitedCount = true;
#endif		//#ifdef PRE_ADD_LIMITED_CASHITEM

			m_VecBuyItemList[nIndex].biPurchaseOrderDetailID = _atoi64(szTempOrderDetail);	
			SetPurchaseOrderID(biOrderID);
#endif	//
		}
		break;

	case CASH_PACKAGEBUY:
		{
			std::vector<string> VecItemString;
			AppendItemString(0, 0, VecItemString);

			char szTempOrderDetails[64] = {0, };
			nRet = pMembershipDB->QueryAddPurchaseOrderByCash(m_biCharacterDBID, m_bPCBang, VecItemString, m_nTotalPrice, m_szIp, false, NULL, NULL, biOrderID, szTempOrderDetails);
			if (nRet != ERROR_NONE)
				return nRet;

#ifdef PRE_ADD_LIMITED_CASHITEM
			if (atoi(VecItemString[Append_QuantityLimited].c_str()) > 0)
				bUpdateLimitedCount = true;
#endif		//#ifdef PRE_ADD_LIMITED_CASHITEM

			SetPurchaseOrderID(biOrderID);
			SetPurchaseOrderDetailID(szTempOrderDetails);
		}
		break;

	case CASH_GIFT:
		{
#if defined(_KR) || defined(_US) || defined(_CH) || defined(_EU) || defined(_ID)	// 선물을 담기로 여러개 구매할수 있음..
			for (int i = 0; i < m_cProductCount; i++){	// 전체로 처리하지말고 한개씩 처리한다
#ifdef PRE_ADD_LIMITED_CASHITEM
				nRet = pMembershipDB->QueryAddPurchaseOrderByCash(m_biCharacterDBID, m_bPCBang, m_VecGiftItemList[i].GiftItem.nItemSN, m_VecGiftItemList[i].nPrice, 
					m_VecGiftItemList[i].nLimitCount, m_VecGiftItemList[i].GiftItem.nItemID, m_VecGiftItemList[i].GiftItem.cItemOption, g_pExtManager->GetCashLimitedItemCount(m_VecGiftItemList[i].GiftItem.nItemSN),
					m_VecGiftItemList[i].nPrice, m_szIp, true, m_wszReceiverCharacterName, m_wszMessage, biOrderID);
#else		//#ifdef PRE_ADD_LIMITED_CASHITEM
				nRet = pMembershipDB->QueryAddPurchaseOrderByCash(m_biCharacterDBID, m_bPCBang, m_VecGiftItemList[i].GiftItem.nItemSN, m_VecGiftItemList[i].nPrice, 
					m_VecGiftItemList[i].nLimitCount, m_VecGiftItemList[i].GiftItem.nItemID, m_VecGiftItemList[i].GiftItem.cItemOption,
					m_VecGiftItemList[i].nPrice, m_szIp, true, m_wszReceiverCharacterName, m_wszMessage, biOrderID);
#endif		//#ifdef PRE_ADD_LIMITED_CASHITEM

				if (nRet != ERROR_NONE)
					return nRet;

#ifdef PRE_ADD_LIMITED_CASHITEM
				if (g_pExtManager->GetCashLimitedItemCount(m_VecGiftItemList[i].GiftItem.nItemSN) > 0)
					bUpdateLimitedCount = true;
#endif		//#ifdef PRE_ADD_LIMITED_CASHITEM

				SetGiftItemPurchaseOrderID(i, biOrderID);
#if defined(PRE_ADD_CADGE_CASH)
				if (m_nMailDBID > 0){
					CDNSQLWorld *pWorldDB = GetWorldDB();
					if (pWorldDB)
						pWorldDB->QueryModWishProducts(m_nMailDBID, biOrderID);
				}
#endif	// #if defined(PRE_ADD_CADGE_CASH)
			}

			SetGiftItemPartByShanda();

#else	//
			if (m_cProductCount <= nIndex) return ERROR_ITEM_FAIL;

#ifdef PRE_ADD_LIMITED_CASHITEM
			nRet = pMembershipDB->QueryAddPurchaseOrderByCash(m_biCharacterDBID, m_bPCBang, m_VecGiftItemList[nIndex].GiftItem.nItemSN, m_VecGiftItemList[nIndex].nPrice, 
				m_VecGiftItemList[nIndex].nLimitCount, m_VecGiftItemList[nIndex].GiftItem.nItemID, m_VecGiftItemList[nIndex].GiftItem.cItemOption, g_pExtManager->GetCashLimitedItemCount(m_VecGiftItemList[nIndex].GiftItem.nItemSN),
				m_VecGiftItemList[nIndex].nPrice, m_szIp, true, m_wszReceiverCharacterName, m_wszMessage, biOrderID);
#else		//#ifdef PRE_ADD_LIMITED_CASHITEM
			nRet = pMembershipDB->QueryAddPurchaseOrderByCash(m_biCharacterDBID, m_bPCBang, m_VecGiftItemList[nIndex].GiftItem.nItemSN, m_VecGiftItemList[nIndex].nPrice, 
				m_VecGiftItemList[nIndex].nLimitCount, m_VecGiftItemList[nIndex].GiftItem.nItemID, m_VecGiftItemList[nIndex].GiftItem.cItemOption,
				m_VecGiftItemList[nIndex].nPrice, m_szIp, true, m_wszReceiverCharacterName, m_wszMessage, biOrderID);
#endif		//#ifdef PRE_ADD_LIMITED_CASHITEM

			if (nRet != ERROR_NONE) 
				return nRet;

#ifdef PRE_ADD_LIMITED_CASHITEM
			if (g_pExtManager->GetCashLimitedItemCount(m_VecGiftItemList[nIndex].GiftItem.nItemSN) > 0)
				bUpdateLimitedCount = true;
#endif		//#ifdef PRE_ADD_LIMITED_CASHITEM

			SetGiftItemPurchaseOrderID(nIndex, biOrderID);
			SetPurchaseOrderID(biOrderID);
#if defined(PRE_ADD_CADGE_CASH)
			if (m_nMailDBID > 0){
				CDNSQLWorld *pWorldDB = GetWorldDB();
				if (pWorldDB)
					pWorldDB->QueryModWishProducts(m_nMailDBID, biOrderID);
			}
#endif	// #if defined(PRE_ADD_CADGE_CASH)

#endif	//
		}
		break;

	case CASH_PACKAGEGIFT:
		{
			std::vector<string> VecItemString;
			AppendItemString(0, 0, VecItemString);

			INT64 biOrderID = 0;
			nRet = pMembershipDB->QueryAddPurchaseOrderByCash(m_biCharacterDBID, m_bPCBang, VecItemString, m_nTotalPrice, m_szIp, true, m_wszReceiverCharacterName, m_wszMessage, biOrderID);
			if (nRet != ERROR_NONE)
				return nRet;

#ifdef PRE_ADD_LIMITED_CASHITEM
			if (atoi(VecItemString[Append_QuantityLimited].c_str()) > 0)
				bUpdateLimitedCount = true;
#endif		//#ifdef PRE_ADD_LIMITED_CASHITEM

			SetPurchaseOrderID(biOrderID);
#if defined(PRE_ADD_CADGE_CASH)
			if (m_nMailDBID > 0){
				CDNSQLWorld *pWorldDB = GetWorldDB();
				if (pWorldDB)
					pWorldDB->QueryModWishProducts(m_nMailDBID, biOrderID);
			}
#endif	// #if defined(PRE_ADD_CADGE_CASH)
		}
		break;

	case CASH_VIPBUY:
		{

		}
		break;

	case CASH_VIPGIFT:
		{

		}
		break;
	}

#ifdef PRE_ADD_LIMITED_CASHITEM
	if (g_pLimitedCashItemRepository && bUpdateLimitedCount)
	{
		std::vector<LimitedCashItem::TLimitedQuantityCashItem> vList;
		if (g_pLimitedCashItemRepository->GetLimitedItemList(vList))
			m_pConnection->SendLimiteItemInfoList(vList);
	}
#endif		//#ifdef PRE_ADD_LIMITED_CASHITEM

	return ERROR_NONE;
}

int CDNUserRepository::PurchaseBuyItem()
{
	int nRet = ERROR_NONE;
#if defined(_JP)
	// 일본은 빌링함수 안에서 처리한다.
#else	// #if defined(_JP)
	nRet = AddPurchaseOrderByCash(m_cCurProductCount);
	if (nRet != ERROR_NONE){
		SendBuyGift(nRet);
		return nRet;
	}

#endif	// #if defined(_JP)

#if defined(_TW) || defined(_TH)
	nRet = g_pBillingShopConnection->OnBuy(this);
#else	// defined(_TW)
	nRet = g_pBillingConnection->OnBuy(this);
#endif	// defined(_TW)
	if (nRet != ERROR_NONE){
		SendBuyGift(nRet);
	}

	return nRet;
}

int CDNUserRepository::PurchaseGiftItem()
{
	int nRet = ERROR_NONE;
#if defined(_JP)
	// 일본은 빌링함수 안에서 처리한다.
#else	// #if defined(_JP)
	nRet = AddPurchaseOrderByCash(m_cCurProductCount);
	if (nRet != ERROR_NONE){
		SendBuyGift(nRet);
		return nRet;
	}

#endif	// #if defined(_JP)

#if defined(_TW) || defined(_TH)
	nRet = g_pBillingShopConnection->OnGift(this);
#else	//
	nRet = g_pBillingConnection->OnGift(this);
#endif	//
	if (nRet != ERROR_NONE){
		SendBuyGift(nRet);
	}

	return nRet;
}

int CDNUserRepository::CashItemDBProcess(int nWorldSetID, int nAccountDBID, INT64 biCharacterDBID, int nMapID, int nChannelID, char cPaymentRules, TCashItemBase &BuyItem, int nPrice, INT64 biPurchaseOrderID,
										 int nAddMaterializedItemCode, INT64 biSenderCharacterDBID/* = 0*/, bool bGift/* = false*/, char cPayMethodCode/* = DBDNWorldDef::PayMethodCode::Cash*/)
{
	CDNSQLMembership *pMembershipDB = GetMembershipDB();
	if (!pMembershipDB)
		return ERROR_GENERIC_INVALIDREQUEST;

	CDNSQLWorld *pWorldDB = GetWorldDB();
	if (!pWorldDB)
		return ERROR_GENERIC_INVALIDREQUEST;

	int nRet = ERROR_DB;

	if (m_cPaymentRules == Cash::PaymentRules::Petal) 
		nAddMaterializedItemCode = DBDNWorldDef::AddMaterializedItem::ReserveMoneyBuy;
#if defined( PRE_ADD_NEW_MONEY_SEED )
	else if (m_cPaymentRules == Cash::PaymentRules::Seed) 
		nAddMaterializedItemCode = DBDNWorldDef::AddMaterializedItem::SeedPointBuy;
#endif

	switch (g_pExtManager->GetItemMainType(BuyItem.CashItem.nItemID))
	{
	case ITEMTYPE_CHARACTER_SLOT:
		{
			char cMaxCount = 0;
			nRet = pMembershipDB->QueryModCharacterSlotCount(biCharacterDBID, 1, cMaxCount);	// 34: 캐릭터 슬롯 개수제한
		}
		break;

	default:
		{
			nRet = pWorldDB->CashItemDBQuery(nAccountDBID, biCharacterDBID, nMapID, nChannelID, cPaymentRules, BuyItem, nPrice, biPurchaseOrderID, nAddMaterializedItemCode, m_wszIp, biSenderCharacterDBID, bGift, cPayMethodCode);
		}
		break;
	}

	return nRet;
}

bool CDNUserRepository::CheckSaleItemSN(int nItemSN)
{
	if (!g_pExtManager->IsOnSaleCommodity(nItemSN)){
		g_Log.Log(LogType::_ERROR, this, L"[CheckSaleItemSN] IsOnSaleCommodity Error(ItemSN:%d)\r\n", nItemSN);
		return false;
	}
#if defined(_KR) || defined(_US)
	if (!g_pBillingConnection->CheckProduct(nItemSN)){
		g_Log.Log(LogType::_ERROR, this, L"[CheckSaleItemSN] CheckProduct Error(ItemSN:%d)\r\n", nItemSN);
		return false;
	}
#endif	// #if defined(_KR) || defined(_US)
	if (!g_pExtManager->IsOnSaleDate(nItemSN)){
		g_Log.Log(LogType::_ERROR, this, L"[CheckSaleItemSN] IsOnSaleDate Error(ItemSN:%d)\r\n", nItemSN);
	    return false;  //$_$ 빔쫠뭔찜珂쇌돨掘齡
	}

	return true;
}

#if defined(PRE_MOD_SELECT_CHAR)
int CDNUserRepository::CheckCharacterSlotCount(int nAccountDBID, int nAddCount)
#else	// #if defined(PRE_MOD_SELECT_CHAR)
int CDNUserRepository::CheckCharacterSlotCount(INT64 biCharacterDBID, int nAddCount)
#endif	// #if defined(PRE_MOD_SELECT_CHAR)
{
	CDNSQLMembership *pMembershipDB = GetMembershipDB();
	if (!pMembershipDB)
		return ERROR_GENERIC_INVALIDREQUEST;

	int nRet = ERROR_DB;
	if (nAddCount > 0){
		char cCharacterCount = 0;
#if defined(PRE_MOD_SELECT_CHAR)
		nRet = pMembershipDB->QueryGetCharacterSlotCount(nAccountDBID, 0, 0, cCharacterCount);
#else	// #if defined(PRE_MOD_SELECT_CHAR)
		nRet = pMembershipDB->QueryGetCharacterSlotCount(0, 0, biCharacterDBID, cCharacterCount);
#endif	// #if defined(PRE_MOD_SELECT_CHAR)

		if (nRet == ERROR_NONE){
#if defined(PRE_MOD_SELECT_CHAR)
			if (cCharacterCount + g_pExtManager->GetGlobalWeightValue(1023) >= g_pExtManager->GetGlobalWeightValue(1022)){
#else	// #if defined(PRE_MOD_SELECT_CHAR)
			if (cCharacterCount + nAddCount > (CreateCharacterDefaultCountMax + g_pExtManager->GetGlobalWeightValue(34))){
#endif	// #if defined(PRE_MOD_SELECT_CHAR)
				return ERROR_CASHSHOP_COUNTOVER;
			}
		}
		else
		{
			return ERROR_ITEM_FAIL;
		}
	}

	return ERROR_NONE;
}

void CDNUserRepository::AppendItemString(int nStartIndex, int nEndIndex, std::vector<std::string> &VecItemString)
{
	VecItemString.resize(Append_Max);

	switch (m_nSubCommand)
	{
	case CASH_BUY:
		{
			for (int i = nStartIndex; i < nEndIndex; i++){
				if (m_VecBuyItemList[i].BuyItem.nItemSN > 0){
					if (!VecItemString[Append_ItemSN].empty()){
						VecItemString[Append_ItemSN].append(",");
						VecItemString[Append_Price].append(",");
						VecItemString[Append_Limit].append(",");
						VecItemString[Append_ItemID].append(",");
						VecItemString[Append_Option].append(",");
#if defined(PRE_ADD_CASH_REFUND)
						VecItemString[Append_Refundable].append(",");
#endif
#ifdef PRE_ADD_LIMITED_CASHITEM
						VecItemString[Append_QuantityLimited].append(",");
#endif		//#ifdef PRE_ADD_LIMITED_CASHITEM
					}
					VecItemString[Append_ItemSN].append(boost::lexical_cast<std::string>(m_VecBuyItemList[i].BuyItem.nItemSN));
					VecItemString[Append_Price].append(boost::lexical_cast<std::string>(g_pExtManager->GetCashCommodityPrice(m_VecBuyItemList[i].BuyItem.nItemSN)));
					VecItemString[Append_Limit].append(boost::lexical_cast<std::string>(g_pExtManager->GetCashBuyAbleCount(m_VecBuyItemList[i].BuyItem.nItemSN)));
					VecItemString[Append_ItemID].append(boost::lexical_cast<std::string>(m_VecBuyItemList[i].BuyItem.CashItem.nItemID));
					BYTE cOption = m_VecBuyItemList[i].BuyItem.CashItem.cOption;
					VecItemString[Append_Option].append(boost::lexical_cast<std::string>(static_cast<int>(cOption)));
#if defined(PRE_ADD_CASH_REFUND)					
					if( m_bCashMoveInven || g_pExtManager->GetCashCommodityNoRefund(m_VecBuyItemList[i].BuyItem.CashItem.nItemID, m_VecBuyItemList[i].BuyItem.nItemSN) )
						VecItemString[Append_Refundable].append("0");	// 캐쉬인벤으로 바로 들어가기
					else
						VecItemString[Append_Refundable].append("1");  // 결재인벤으로 들어가기
#endif
#ifdef PRE_ADD_LIMITED_CASHITEM
					VecItemString[Append_QuantityLimited].append(boost::lexical_cast<std::string>(g_pExtManager->GetCashLimitedItemCount(m_VecBuyItemList[i].BuyItem.nItemSN)));
#endif		//#ifdef PRE_ADD_LIMITED_CASHITEM
				}
			}
		}
		break;

	case CASH_PACKAGEBUY:
		{
			VecItemString[Append_ItemSN] = FormatA("%d", m_nPackageSN);
			VecItemString[Append_Price] = FormatA("%d", g_pExtManager->GetCashCommodityPrice(m_nPackageSN));
			VecItemString[Append_Limit] = FormatA("%d", g_pExtManager->GetCashBuyAbleCount(m_nPackageSN));
			for (int i = 0; i < m_cProductCount; i++){
				if (m_ProductPackageList[i].nItemSN > 0){
					if (!VecItemString[Append_ItemID].empty()){
						VecItemString[Append_ItemID].append("|");
						VecItemString[Append_Option].append("|");
					}
					VecItemString[Append_ItemID].append(boost::lexical_cast<std::string>(m_ProductPackageList[i].CashItem.nItemID));
					BYTE cOption = m_ProductPackageList[i].CashItem.cOption;
					VecItemString[Append_Option].append(boost::lexical_cast<std::string>(static_cast<int>(cOption)));
				}
			}

#ifdef PRE_ADD_LIMITED_CASHITEM
			VecItemString[Append_QuantityLimited].append(boost::lexical_cast<std::string>(g_pExtManager->GetCashLimitedItemCount(m_nPackageSN)));
#endif		//#ifdef PRE_ADD_LIMITED_CASHITEM

#if defined(PRE_ADD_CASH_REFUND)
			if( m_bCashMoveInven || g_pExtManager->GetCashCommodityNoRefund(0, m_nPackageSN))	
			{
				VecItemString[Append_Refundable] = "0";		// 캐쉬인벤으로 바로 넣기
				m_bCashMoveInven = true;
			}
			else 				
				VecItemString[Append_Refundable] = "1";		// 결재인벤으로
#endif	// #if defined(PRE_ADD_CASH_REFUND)
		}
		break;

	case CASH_PACKAGEGIFT:
		{
			VecItemString[Append_ItemSN] = FormatA("%d", m_nPackageSN);
			VecItemString[Append_Price] = FormatA("%d", g_pExtManager->GetCashCommodityPrice(m_nPackageSN));
			VecItemString[Append_Limit] = FormatA("%d", g_pExtManager->GetCashBuyAbleCount(m_nPackageSN));
			for (int i = 0; i < m_cProductCount; i++){
				if (m_GiftPackageList[i].nItemSN > 0){
					if (!VecItemString[Append_ItemID].empty()){
						VecItemString[Append_ItemID].append("|");
						VecItemString[Append_Option].append("|");
					}	
					VecItemString[Append_ItemID].append(boost::lexical_cast<std::string>(m_GiftPackageList[i].nItemID));
					BYTE cOption = m_GiftPackageList[i].cItemOption;
					VecItemString[Append_Option].append(boost::lexical_cast<std::string>(static_cast<int>(cOption)));
				}
			}

#ifdef PRE_ADD_LIMITED_CASHITEM
			VecItemString[Append_QuantityLimited].append(boost::lexical_cast<std::string>(g_pExtManager->GetCashLimitedItemCount(m_nPackageSN)));
#endif		//#ifdef PRE_ADD_LIMITED_CASHITEM		
		}
		break;
	}
}

void CDNUserRepository::SetUsedPaidCash(LPCWSTR pReservedFields)
{
	if (!pReservedFields) return;

	std::vector<std::wstring> UPCTokens;
	TokenizeW(pReservedFields, UPCTokens, L"=");
	if (UPCTokens.size() != 2) return;

	m_nUsedPaidCash = _wtoi(UPCTokens[1].c_str());
}

#if defined( PRE_ADD_NEW_MONEY_SEED )
int CDNUserRepository::AddSeedPoint( int nSeedPoint )
{
	CDNSQLWorld *pWorldDB = GetWorldDB();

	if (!pWorldDB)
	{		
		return ERROR_GENERIC_INVALIDREQUEST;
	}

	TQAddEtcPoint pQ;
	memset( &pQ, 0, sizeof(pQ) );

	pQ.biCharacterDBID = GetCharacterDBID();
	pQ.cType = DBDNWorldDef::EtcPointCode::SeedPoint;
	pQ.biAddPoint = nSeedPoint;
	pQ.nMapID = GetMapID();
	MultiByteToWideChar(CP_ACP, 0, m_szIp, -1, pQ.wszIP, IPLENMAX);

	TAAddEtcPoint pA;
	memset( &pA, 0, sizeof(pA) );

	return pWorldDB->QueryAddEtcPoint( &pQ, &pA );	
}

int CDNUserRepository::UseSeedPoint()
{
	CDNSQLWorld *pWorldDB = GetWorldDB();
	
	if (!pWorldDB)
	{		
		return ERROR_GENERIC_INVALIDREQUEST;
	}

	TQUseEtcPoint pQ;
	memset( &pQ, 0, sizeof(pQ) );

	pQ.biCharacterDBID = GetCharacterDBID();
	pQ.cType = DBDNWorldDef::EtcPointCode::SeedPoint;
	pQ.biUsePoint = m_nTotalPrice;
	pQ.nMapID = GetMapID();
	MultiByteToWideChar(CP_ACP, 0, m_szIp, -1, pQ.wszIP, IPLENMAX);

	TAUseEtcPoint pA;
	memset( &pA, 0, sizeof(pA) );
	
	return pWorldDB->QueryUseEtcPoint( &pQ, &pA );	
}

INT64 CDNUserRepository::GetSeedPoint()
{
	CDNSQLWorld *pWorldDB = GetWorldDB();

	if (!pWorldDB)
	{		
		return 0;
	}	

	INT64 nPoint = 0;
	pWorldDB->QueryGetEtcPoint( GetCharacterDBID(), DBDNWorldDef::EtcPointCode::SeedPoint, nPoint);	
	return nPoint;
}
#endif