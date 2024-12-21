#include "StdAfx.h"
#include "DNCashConnection.h"
#include "DNIocpManager.h"
#include "DNGameDataManager.h"
#include "DNUserSession.h"
#include "Util.h"

#if defined(_VILLAGESERVER)
#include "DNUserSessionManager.h"
#ifdef PRE_ADD_LIMITED_CASHITEM
#include "DNLimitedCashItemRepository.h"
#endif		//#ifdef PRE_ADD_LIMITED_CASHITEM

extern TVillageConfig g_Config;
#elif defined(_GAMESERVER)

extern TGameConfig g_Config;
#endif

CDNCashConnection *g_pCashConnection = NULL;

CDNCashConnection::CDNCashConnection(void)
{
#if defined(_FINAL_BUILD)
	Init(1024 * 1024, 1024 * 1024);
#else	// #if defined(_FINAL_BUILD)
	Init(1024 * 100, 1024 * 100);
#endif	// #if defined(_FINAL_BUILD)

	m_bPetalSaleAbort = false;

	m_cSaleAbortCount = 0;
	memset(&m_nSaleAbortList, 0, sizeof(m_nSaleAbortList));
}

CDNCashConnection::~CDNCashConnection(void)
{
}

void CDNCashConnection::Reconnect()
{
	if (!CConnection::GetActive() && !GetConnecting() && g_pIocpManager)
	{
		SetConnecting(true);
		if (g_pIocpManager->AddConnectionEx(this, CONNECTIONKEY_CASH, g_Config.CashInfo.szIP, g_Config.CashInfo.nPort) < 0){
			SetConnecting(false);
			// g_Log.Log(LogType::_ERROR, L"CashServer Fail(%S, %d)\r\n", g_Config.CashInfo.szIP, g_Config.CashInfo.nPort);
		}
		else 
		{
			//g_Log.Log( LogType::_NORMAL, L"CashServer (%S, %d) Connecting..\r\n",g_Config.CashInfo.szIP, g_Config.CashInfo.nPort);			
		}
	}
}

int CDNCashConnection::MessageProcess(int nMainCmd, int nSubCmd, char *pData, int nLen)
{
#if defined(_VILLAGESERVER)
	if (nMainCmd == MAINCMD_CASH)
	{
		if (nSubCmd == CASH_SALEABORTLIST)
		{
			TASaleAbortList *pAbort = (TASaleAbortList*)pData;
			m_bPetalSaleAbort = false;

			g_pUserSessionManager->SendSaleAbortList(pAbort->cCount, pAbort->nAbortList);

			//일단 보내고
			m_cSaleAbortCount = pAbort->cCount;
			memset(&m_nSaleAbortList, 0, sizeof(m_nSaleAbortList));		//원래있던거 날리고

			for (int i = 0; i < pAbort->cCount; i++){
				m_nSaleAbortList[i] = pAbort->nAbortList[i];			//배열에 넣어준다. 사이즈 전체로 멤카피하면 안뎀~

				if (g_pDataManager->GetItemMainType(g_pDataManager->GetCashCommodityItem0(pAbort->nAbortList[i])) == ITEMTYPE_PETALTOKEN){
					m_bPetalSaleAbort = true;				
				}
				g_Log.Log(LogType::_ERROR, L"[SendSaleAbortList] nAbortList:%d\r\n", pAbort->nAbortList[i]);
			}

			return ERROR_NONE;
		}
#ifdef PRE_ADD_LIMITED_CASHITEM
		else if (nSubCmd == CASH_LIMITEDITEM_INFOLIST)
		{
			TAGetLimitedItemList * pPacket = (TAGetLimitedItemList*)pData;

			if (g_pLimitedCashItemRepository)
				g_pLimitedCashItemRepository->UpdateLimitedItemList(pPacket);

			return ERROR_NONE;
		}
		else if (nSubCmd == CASH_CHANGEDLIMITEDITEM_INFOLIST)
		{
			TAGetChangedLimitedItemList * pPacket = (TAGetChangedLimitedItemList*)pData;

			if (g_pLimitedCashItemRepository)
			{
				for (int i = 0; i < pPacket->cCount; i++)
				{
					g_pLimitedCashItemRepository->UpdateChangedLimitedItemMax(pPacket->Limited[i].nProductID, pPacket->Limited[i].nMaxCount);
				}
			}
			return ERROR_NONE;
		}
#endif		//#ifdef PRE_ADD_LIMITED_CASHITEM
	}
#endif	// #if defined(_VILLAGESERVER)

	UINT nAccountDBID;
	memcpy(&nAccountDBID, pData, sizeof(UINT));

#if defined(_VILLAGESERVER)
	CDNUserSession *pSession = g_pUserSessionManager->FindUserSessionByAccountDBID(nAccountDBID);
	if (!pSession){
		g_Log.Log(LogType::_ERROR, g_Config.nWorldSetID, nAccountDBID, 0, 0, L"[ADBID:%u] CDNCashConnection::MessageProcess UserSession not found\r\n", nAccountDBID);
		return ERROR_GENERIC_INVALIDREQUEST;
	}
	pSession->CashMessageProcess(nMainCmd, nSubCmd, pData, nLen);

#elif defined(_GAMESERVER)
	CDNRUDPGameServer* pServer = g_pGameServerManager->GetGameServerByAID(nAccountDBID);
	if (pServer)	//디비 메세지 파싱
	{
		pServer->StoreExternalBuffer(nAccountDBID, nMainCmd, nSubCmd, pData, nLen, EXTERNALTYPE_CASH);
		return ERROR_NONE;
	}
#endif	// #if defined(_VILLAGESERVER)

	return ERROR_NONE;
}

void CDNCashConnection::SendBalanceInquiry(CDNUserSession *pSession, bool bOpen, bool bServer)
{
	TQCashBalanceInquiry Cash;
	memset(&Cash, 0, sizeof(TQCashBalanceInquiry));
	Cash.nAccountDBID = pSession->GetAccountDBID();
	if (pSession->GetAccountNameA()) 
		_strcpy(Cash.szAccountName, _countof(Cash.szAccountName), pSession->GetAccountNameA(), (int)strlen(pSession->GetAccountNameA()));
	Cash.bOpen = bOpen;
	Cash.bServer = bServer;
#if defined(_KR) || defined(_US)
	Cash.nNexonSN = pSession->m_nNexonSN;
#endif	// #if defined(_KR)
	if (pSession->GetIp())
		Cash.nIp = (UINT)inet_addr(pSession->GetIp());

	AddSendData(MAINCMD_CASH, CASH_BALANCEINQUIRY, (char*)&Cash, sizeof(TQCashBalanceInquiry));
}

void CDNCashConnection::SendDelUser(UINT nAccountDBID)
{
	TQCashDelUser Cash;
	Cash.nAccountDBID = nAccountDBID;
	AddSendData(MAINCMD_CASH, CASH_DELUSER, (char*)&Cash, sizeof(TQCashDelUser));
}

void CDNCashConnection::SendBuy(CDNUserSession *pSession, const CSCashShopBuy *pCashShop, TCashBuyItem *BuyList)
{
	TQCashBuy Cash;
	memset(&Cash, 0, sizeof(TQCashBuy));

	Cash.nAccountDBID = pSession->GetAccountDBID();
	Cash.biCharacterDBID = pSession->GetCharacterDBID();
	Cash.cWorldSetID = pSession->GetWorldSetID();
	Cash.nMapID = pSession->GetMapIndex();
	Cash.nChannelID = pSession->GetChannelID();	
	if (pSession->GetCharacterNameA()) 
		_strcpy(Cash.szCharacterName, _countof(Cash.szCharacterName), pSession->GetCharacterNameA(), (int)strlen(pSession->GetCharacterNameA()));
	Cash.cAge = pSession->GetAge();
	Cash.bPCBang = pSession->IsPCBang();
	Cash.cCartType = pCashShop->cType;
#if defined(_VILLAGESERVER) && defined(PRE_ADD_CASH_REFUND)
	Cash.bMoveCashInven = pCashShop->bMoveCashInven;
#endif
	Cash.cPaymentRules = pCashShop->cPaymentRules;
#if defined(PRE_ADD_SALE_COUPON)
	Cash.biSaleCouponSerial = pCashShop->biSaleCouponSerial;
#endif

	if (BuyList){
		Cash.cProductCount = pCashShop->cCount;

		switch (Cash.cCartType)
		{
		case 3:
		case 7:
			if (Cash.cProductCount > PREVIEWCARTLISTMAX)
				Cash.cProductCount = PREVIEWCARTLISTMAX;
			break;

		default:
			if (Cash.cProductCount > CARTLISTMAX)
				Cash.cProductCount = CARTLISTMAX;
			break;
		}

		for (int i = 0; i < Cash.cProductCount; i++){
			Cash.BuyList[i] = BuyList[i];
		}
	}

	int nLen = sizeof(TQCashBuy) - sizeof(Cash.BuyList) + (sizeof(TCashBuyItem) * Cash.cProductCount);
	AddSendData(MAINCMD_CASH, CASH_BUY, (char*)&Cash, nLen);
}

void CDNCashConnection::SendPackageBuy(CDNUserSession *pSession, const CSCashShopPackageBuy *pCashShop, TCashItemBase *BuyList)
{
	TQCashPackageBuy Cash;
	memset(&Cash, 0, sizeof(TQCashPackageBuy));

	Cash.nAccountDBID = pSession->GetAccountDBID();
	Cash.biCharacterDBID = pSession->GetCharacterDBID();
	Cash.cWorldSetID = pSession->GetWorldSetID();
	Cash.nMapID = pSession->GetMapIndex();
	Cash.nChannelID = pSession->GetChannelID();	
	if (pSession->GetCharacterNameA())
		_strcpy(Cash.szCharacterName, _countof(Cash.szCharacterName), pSession->GetCharacterNameA(), (int)strlen(pSession->GetCharacterNameA()));
	Cash.cAge = pSession->GetAge();
	Cash.bPCBang = pSession->IsPCBang();
	Cash.nPackageSN = pCashShop->nPackageSN;
#if defined(_VILLAGESERVER) && defined(PRE_ADD_CASH_REFUND)
	Cash.bMoveCashInven = pCashShop->bMoveCashInven;
#endif
	Cash.cPaymentRules = pCashShop->cPaymentRules;
#if defined(PRE_ADD_SALE_COUPON)
	Cash.biSaleCouponSerial = pCashShop->biSaleCouponSerial;
#endif

	Cash.cPackageCount = pCashShop->cCount;

	for (int i = 0; i < Cash.cPackageCount; i++){
		Cash.BuyList[i] = BuyList[i];
	}

	int nLen = sizeof(TQCashPackageBuy) - sizeof(Cash.BuyList) + (sizeof(TCashItemBase) * Cash.cPackageCount);
	AddSendData(MAINCMD_CASH, CASH_PACKAGEBUY, (char*)&Cash, nLen);
}

void CDNCashConnection::SendGift(CDNUserSession *pSession, const CSCashShopGift *pCashShop, TCashGiftItem *GiftList)
{
	TQCashGift Cash;
	memset(&Cash, 0, sizeof(TQCashGift));

	Cash.nAccountDBID = pSession->GetAccountDBID();
	Cash.biCharacterDBID = pSession->GetCharacterDBID();
	Cash.cWorldSetID = pSession->GetWorldSetID();
	if (pSession->GetCharacterNameA()) 
		_strcpy(Cash.szCharacterName, _countof(Cash.szCharacterName), pSession->GetCharacterNameA(), (int)strlen(pSession->GetCharacterNameA()));
	Cash.cAge = pSession->GetAge();
	Cash.bPCBang = pSession->IsPCBang();
	Cash.nMapID = pSession->GetMapIndex();
	Cash.nChannelID = pSession->GetChannelID();
	if (pCashShop->wszToCharacterName)
		_wcscpy(Cash.wszReceiverCharacterName, NAMELENMAX, pCashShop->wszToCharacterName, (int)wcslen(pCashShop->wszToCharacterName));
	if (pCashShop->wszMessage)
		_wcscpy(Cash.wszMessage, GIFTMESSAGEMAX, pCashShop->wszMessage, (int)wcslen(pCashShop->wszMessage));

	Cash.cCartType = pCashShop->cType;

#if defined(PRE_ADD_CADGE_CASH)
	Cash.nMailDBID = pCashShop->nMailDBID;
#endif	// #if defined(PRE_ADD_CADGE_CASH)

	if (GiftList){
		Cash.cProductCount = pCashShop->cCount;

		switch (Cash.cCartType)
		{
		case 3:
		case 7:
			if (Cash.cProductCount > PREVIEWCARTLISTMAX) Cash.cProductCount = PREVIEWCARTLISTMAX;
			break;

		default:
			if (Cash.cProductCount > CARTLISTMAX) Cash.cProductCount = CARTLISTMAX;
			break;
		}

		for (int i = 0; i < Cash.cProductCount; i++){
			Cash.GiftList[i] = GiftList[i];
		}
	}

	int nLen = sizeof(TQCashGift) - sizeof(Cash.GiftList) + (sizeof(TCashGiftItem) * Cash.cProductCount);
	AddSendData(MAINCMD_CASH, CASH_GIFT, (char*)&Cash, nLen);
}

void CDNCashConnection::SendPackageGift(CDNUserSession *pSession, const CSCashShopPackageGift *pCashShop, TCashPackageGiftItem *GiftList)
{
	TQCashPackageGift Cash;
	memset(&Cash, 0, sizeof(TQCashPackageGift));

	Cash.nAccountDBID = pSession->GetAccountDBID();
	Cash.biCharacterDBID = pSession->GetCharacterDBID();
	Cash.cWorldSetID = pSession->GetWorldSetID();
	if (pSession->GetCharacterNameA()) 
		_strcpy(Cash.szCharacterName, _countof(Cash.szCharacterName), pSession->GetCharacterNameA(), (int)strlen(pSession->GetCharacterNameA()));
	Cash.cAge = pSession->GetAge();
	Cash.bPCBang = pSession->IsPCBang();
	Cash.nMapID = pSession->GetMapIndex();
	Cash.nChannelID = pSession->GetChannelID();
	if (pCashShop->wszToCharacterName)
		_wcscpy(Cash.wszReceiverCharacterName, NAMELENMAX, pCashShop->wszToCharacterName, (int)wcslen(pCashShop->wszToCharacterName));
	if (pCashShop->wszMessage)
		_wcscpy(Cash.wszMessage, GIFTMESSAGEMAX, pCashShop->wszMessage, (int)wcslen(pCashShop->wszMessage));

#if defined(PRE_ADD_CADGE_CASH)
	Cash.nMailDBID = pCashShop->nMailDBID;
#endif	// #if defined(PRE_ADD_CADGE_CASH)

	Cash.nPackageSN = pCashShop->nPackageSN;
	Cash.cPackageCount = pCashShop->cCount;

	for (int i = 0; i < Cash.cPackageCount; i++){
		Cash.GiftList[i] = GiftList[i];
	}

	int nLen = sizeof(TQCashPackageGift) - sizeof(Cash.GiftList) + (sizeof(TCashPackageGiftItem) * Cash.cPackageCount);
	AddSendData(MAINCMD_CASH, CASH_PACKAGEGIFT, (char*)&Cash, nLen);
}

void CDNCashConnection::SendCoupon(CDNUserSession *pSession, WCHAR *pCoupon)
{
	TQCashCoupon Cash;
	memset(&Cash, 0, sizeof(TQCashCoupon));

	Cash.nAccountDBID = pSession->GetAccountDBID();
	Cash.biCharacterDBID = pSession->GetCharacterDBID();
	Cash.cWorldSetID = pSession->GetWorldSetID();
	if (pCoupon)
		_wcscpy(Cash.wszCoupon, COUPONMAX, pCoupon, (int)wcslen(pCoupon));
	Cash.bPCBang = pSession->IsPCBang();
	if (pSession->GetCharacterNameA())
		_strcpy(Cash.szCharacterName, NAMELENMAX, pSession->GetCharacterNameA(), (int)strlen(pSession->GetCharacterNameA()));

	AddSendData(MAINCMD_CASH, CASH_COUPON, (char*)&Cash, sizeof(TQCashCoupon));
}

void CDNCashConnection::SendVIPBuy(CDNUserSession *pSession, int nItemSN, bool bServer/* = false*/)
{
	TQCashVIPBuy Cash;
	memset(&Cash, 0, sizeof(TQCashVIPBuy));

	Cash.nAccountDBID = pSession->GetAccountDBID();
	Cash.biCharacterDBID = pSession->GetCharacterDBID();
	Cash.nItemSN = nItemSN;
	Cash.cWorldSetID = pSession->GetWorldSetID();
	Cash.bPCBang = pSession->IsPCBang();
	Cash.nMapID = pSession->GetMapIndex();
	Cash.nChannelID = pSession->GetChannelID();
	Cash.bAutoPay = pSession->IsVIPAutoPay();
	Cash.bServer = bServer;

	AddSendData(MAINCMD_CASH, CASH_VIPBUY, (char*)&Cash, sizeof(TQCashVIPBuy));
}

void CDNCashConnection::SendVIPGift(CDNUserSession *pSession, const CSVIPGift *pCashShop)
{
	TQCashVIPGift Cash;
	memset(&Cash, 0, sizeof(TQCashVIPGift));

	Cash.nAccountDBID = pSession->GetAccountDBID();
	Cash.biCharacterDBID = pSession->GetCharacterDBID();
	Cash.nItemSN = pCashShop->nItemSN;
	Cash.cWorldSetID = pSession->GetWorldSetID();
	Cash.bPCBang = pSession->IsPCBang();
	Cash.nMapID = pSession->GetMapIndex();
	Cash.nChannelID = pSession->GetChannelID();
	if (pCashShop->wszToCharacterName)
		_wcscpy(Cash.wszReceiverCharacterName, NAMELENMAX, pCashShop->wszToCharacterName, (int)wcslen(pCashShop->wszToCharacterName));
	if (pCashShop->wszMessage)
		_wcscpy(Cash.wszMessage, GIFTMESSAGEMAX, pCashShop->wszMessage, (int)wcslen(pCashShop->wszMessage));

	AddSendData(MAINCMD_CASH, CASH_VIPGIFT, (char*)&Cash, sizeof(TQCashVIPGift));
}

void CDNCashConnection::SendSaleAbortList()
{
	AddSendData(MAINCMD_CASH, CASH_SALEABORTLIST, NULL, 0);
}

#ifdef PRE_ADD_LIMITED_CASHITEM
void CDNCashConnection::SendGetLimitedItemList()
{
	AddSendData(MAINCMD_CASH, CASH_LIMITEDITEM_INFOLIST, NULL, 0);
}

void CDNCashConnection::SendCheatChangeLimitMax(int nSN, int nLimitMax)
{
	TQChangeLimitMax packet;
	memset(&packet, 0, sizeof(TQChangeLimitMax));

	packet.nSN = nSN;
	packet.nLimitMax = nLimitMax;

	AddSendData(MAINCMD_CASH, CASH_CHANGEDLIMITEDITEM_INFOLIST, (char*)&packet, sizeof(TQChangeLimitMax));
}
#endif		//#ifdef PRE_ADD_LIMITED_CASHITEM

#if defined(_VILLAGESERVER) && defined(PRE_ADD_CASH_REFUND)
void CDNCashConnection::SendMoveCashInven(UINT nAccountDBID, UINT64 biCharacterDBID, char cWorldSetID, int nMapID, int nChannelID, TCashItemBase* pCashItem, TPaymentItemInfoEx* pPaymentItemInfo )
{
	TQCashMoveCashInven Cash;
	memset(&Cash, 0, sizeof(TQCashMoveCashInven));

	Cash.nAccountDBID = nAccountDBID;
	Cash.biCharacterDBID = biCharacterDBID;
	Cash.cWorldSetID = cWorldSetID;
	Cash.nMapID = nMapID;
	Cash.nChannelID = nChannelID;
	memcpy(&Cash.CashItem, pCashItem, sizeof(TCashItemBase));
	memcpy(&Cash.PaymentItemInfo, pPaymentItemInfo, sizeof(TPaymentItemInfoEx));

	AddSendData(MAINCMD_CASH, CASH_MOVE_CASHINVEN, (char*)&Cash, sizeof(TQCashMoveCashInven));
}

void CDNCashConnection::SendPackageMoveCashInven(UINT nAccountDBID, UINT64 biCharacterDBID, char cWorldSetID, int nMapID, int nChannelID, TCashItemBase* pCashItemList, TPaymentPackageItemInfoEx* pPaymentPackageItemInfo )
{
	TQCashPackageMoveCashInven Cash;
	memset(&Cash, 0, sizeof(TQCashPackageMoveCashInven));

	Cash.nAccountDBID = nAccountDBID;
	Cash.biCharacterDBID = biCharacterDBID;
	Cash.cWorldSetID = cWorldSetID;
	Cash.nMapID = nMapID;
	Cash.nChannelID = nChannelID;
	memcpy(&Cash.CashItemList, pCashItemList, sizeof(Cash.CashItemList));
	memcpy(&Cash.PaymentPackageItemInfo, pPaymentPackageItemInfo, sizeof(TPaymentPackageItemInfoEx));

	AddSendData(MAINCMD_CASH, CASH_PACKAGE_MOVE_CASHINVEN, (char*)&Cash, sizeof(TQCashPackageMoveCashInven));
}

void CDNCashConnection::SendCashRefund(UINT nAccountDBID, char cItemType, char cWorldSetID, int nItemSN, UINT uiOrderNo, INT64 biPurchaseOrderDetailID )
{
	TQCashRefund Cash;
	memset(&Cash, 0, sizeof(TQCashRefund));

	Cash.nAccountDBID = nAccountDBID;
	Cash.cItemType = cItemType;
	Cash.cWorldSetID = cWorldSetID;
	Cash.nItemSN = nItemSN;
	Cash.uiOrderNo = uiOrderNo;
	Cash.biPurchaseOrderDetailID = biPurchaseOrderDetailID;

	AddSendData(MAINCMD_CASH, CASH_REFUNDCASH, (char*)&Cash, sizeof(TQCashRefund));
}
#endif
