#include "StdAfx.h"
#include "DNBillingConnectionID.h"
#include "DNUserRepository.h"
#include "DNExtManager.h"
#include "Log.h"

extern TCashConfig g_Config;

#if defined(_ID)
#include "HttpClientManager.h"
#include "Util.h"

CHttpClientManager * g_pHttpCouponManager = NULL;
CDNBillingConnectionID *g_pBillingConnection = NULL;

CDNBillingConnectionID::CDNBillingConnectionID(int nWorkerThreadSize)
{
	g_pHttpClientManager = new CHttpClientManager();
	g_pHttpClientManager->CreateHttpClinet(nWorkerThreadSize, KreonBilling::ShopServerAddress);

	g_pHttpCouponManager = new CHttpClientManager();
	g_pHttpCouponManager->CreateHttpClinet(nWorkerThreadSize, KreonBilling::CouponServerAddress);
}

CDNBillingConnectionID::~CDNBillingConnectionID(void)
{
	SAFE_DELETE(g_pHttpClientManager);
	SAFE_DELETE(g_pHttpCouponManager);
}

int CDNBillingConnectionID::OnBalanceInquiry(CDNUserRepository *pUser)
{
	CHttpClient* pHttpClient = g_pHttpClientManager->FindHttpClient();
	if (!pHttpClient)
		return ERROR_KREON_INVALID_REQUEST;

	char szTemp[128] = {0,};
	SNPRINTFA(szTemp, _countof(szTemp), "%s%s%s", KreonBilling::ShopKey, pUser->GetAccountName(), KreonBilling::ShopID);

	std::string strKey;
	MD5Hash(szTemp, (int)strlen(szTemp), strKey);

	CHttpClientAuto Auto(pHttpClient, g_pHttpClientManager);

	char szPostData[256]={0,};
	SNPRINTFA(szPostData, _countof(szPostData),"SID=%s&UID=%s&A-KEY=%s",
		KreonBilling::ShopID, pUser->GetAccountName(), strKey.c_str());	

	int nStatus = pHttpClient->SendRequsetPostA(KreonBilling::BalanceUrl, szPostData);
	if( nStatus != 200) // 200이 아니면 다 에러?
	{
		g_Log.Log(LogType::_ERROR, L"Kreon Billing GetStatus!(%d)\r\n", nStatus);
		return ERROR_KREON_INVALID_REQUEST;
	}
	BYTE byBuff[1024]={0,};
	if( pHttpClient->RecvResponse((LPBYTE)byBuff, 1024) == 0)
	{
		int nLength = (int)strlen((char*)byBuff);
		byBuff[nLength-1] = '\0';		// } 이거 없앨려고 하나 뺍니다.

		int nCashBalance = 0;
		int nResult = ConvertResult((char*)byBuff,false, nCashBalance);
		pUser->SendBalanceInquiry(nResult, nCashBalance);		
	}
	return ERROR_NONE;
}

void CDNBillingConnectionID::SendPurchaseItem(CDNUserRepository* pUser, bool bGift, bool bPackage)
{
	CHttpClient* pHttpClient = g_pHttpClientManager->FindHttpClient();
	if (!pHttpClient) {
		pUser->OnRecvBillingBuyItem(ERROR_KREON_INVALID_REQUEST) ;
		return;		
	}	
	char szTemp[128] = {0,};

	CHttpClientAuto Auto(pHttpClient, g_pHttpClientManager);

	std::string strPostData;
	char szPostData[256]={0,};
	if( !bPackage && bGift )
	{
		if( pUser->m_VecGiftItemList.size() <= pUser->m_cCurProductCount )
		{
			pUser->OnRecvBillingBuyItem(ERROR_KREON_INVALID_REQUEST) ;
			return;
		}
		sprintf_s(szTemp, "%I64d", pUser->m_VecGiftItemList[pUser->m_cCurProductCount].biPurchaseOrderID);
	}
	else
		sprintf_s(szTemp, "%I64d", pUser->GetPurchaseOrderID());

	SNPRINTFA(szPostData, _countof(szPostData),"SHOP-ID=%s&SHOP-TRNO=%s&USER-IP=%s&BUYER-ID=%s&BUYER-CHARACTER=%s",
		KreonBilling::ShopID, szTemp, pUser->m_szIp, pUser->GetAccountName(), pUser->GetCharacterName());	

	strPostData=szPostData;
	strPostData.append("&BUYER-SERVER=");
	sprintf_s(szTemp, "%d", g_Config.nWorldSetID); // 서버명은 따로 없으니 그냥 월드 번호.
	strPostData.append(szTemp);
	if( bGift )
	{
		strPostData.append("&RECIPIENT-ID=");
		strPostData.append(pUser->GetReceiverAccountName());
		if (pUser->m_wszReceiverCharacterName[0] > 0)
		{
			char szReceiverCharName[64] = {0, };
			WideCharToMultiByte( CP_ACP, 0, pUser->m_wszReceiverCharacterName, -1, szReceiverCharName, 64, NULL, NULL );
			strPostData.append("&RECIPIENT-CHARACTER=");
			strPostData.append(szReceiverCharName);
			strPostData.append("&RECIPIENT-SERVER=");
			strPostData.append(szTemp);			
		}
	}
	strPostData.append("&PG-CODE=P001");	
	int nProductCount = 1;
	if( !bPackage && !bGift)
		nProductCount = pUser->m_cProductCount;

	sprintf_s(szTemp, "%d", nProductCount);	
	strPostData.append("&TOTAL-COUNT=");
	strPostData.append(szTemp);
	if( !bPackage && bGift )
		sprintf_s(szTemp, "%d", pUser->m_VecGiftItemList[pUser->m_cCurProductCount].nPrice);			
	else
		sprintf_s(szTemp, "%d", pUser->m_nTotalPrice);	
	
	strPostData.append("&TOTAL-PRICE=");
	strPostData.append(szTemp);

	if( bPackage )
	{
		sprintf_s(szTemp, "%d", pUser->GetPackageSN());	
		strPostData.append("&P1-CODE=");
		strPostData.append(szTemp);
		strPostData.append("&P1-TYPE=C033"); // 아이템

		std::string strItemName;
		bool bNameRet = g_pExtManager->GetCashCommodityName(pUser->GetPackageSN(), strItemName);
		if( !bNameRet )
		{
			pUser->OnRecvBillingBuyItem(ERROR_KREON_INVALID_REQUEST) ;
			return;
		}
		strPostData.append("&P1-NAME=");
		strPostData.append(strItemName.c_str());
		strPostData.append("&P1-COUNT=1");
		sprintf_s(szTemp, "%d", pUser->m_nTotalPrice);
		strPostData.append("&P1-PRICESUM=");
		strPostData.append(szTemp);		
	}
	else if( bGift ) // 선물은 한개씩 처리.
	{
		sprintf_s(szTemp, "%d", pUser->m_VecGiftItemList[pUser->m_cCurProductCount].GiftItem.nItemSN);	
		strPostData.append("&P1-CODE=");
		strPostData.append(szTemp);
		strPostData.append("&P1-TYPE=C033"); // 아이템

		std::string strItemName;
		bool bNameRet = g_pExtManager->GetCashCommodityName(pUser->m_VecGiftItemList[pUser->m_cCurProductCount].GiftItem.nItemSN, strItemName);
		if( !bNameRet )
		{
			pUser->OnRecvBillingBuyItem(ERROR_KREON_INVALID_REQUEST) ;
			return;
		}
		strPostData.append("&P1-NAME=");
		strPostData.append(strItemName.c_str());				
		strPostData.append("&P1-COUNT=1");
		sprintf_s(szTemp, "%d", pUser->m_VecGiftItemList[pUser->m_cCurProductCount].nPrice);
		strPostData.append("&P1-PRICESUM=");
		strPostData.append(szTemp);
	}
	else
	{			
		char szParameter[32] = {0,};
		std::string strItemName;
	
		for( int i=0; i<nProductCount; ++i)
		{
			sprintf_s(szParameter, "&P%d-CODE=", i+1);
			sprintf_s(szTemp, "%d", pUser->m_VecBuyItemList[i].BuyItem.nItemSN);
			strPostData.append(szParameter);
			strPostData.append(szTemp);				
			sprintf_s(szParameter, "&P%d-TYPE=C033", i+1);
			strPostData.append(szParameter);				
			sprintf_s(szParameter, "&P%d-NAME=", i+1);
			bool bNameRet = g_pExtManager->GetCashCommodityName(pUser->m_VecBuyItemList[i].BuyItem.nItemSN, strItemName);
			if( !bNameRet )
			{
				pUser->OnRecvBillingBuyItem(ERROR_KREON_INVALID_REQUEST) ;
				return;
			}
			strPostData.append(szParameter);
			strPostData.append(strItemName.c_str());						
			sprintf_s(szParameter, "&P%d-COUNT=1", i+1);
			strPostData.append(szParameter);				
			sprintf_s(szParameter, "&P%d-PRICESUM=", i+1);
			sprintf_s(szTemp, "%d", pUser->m_VecBuyItemList[i].nPrice);	
			strPostData.append(szParameter);
			strPostData.append(szTemp);				
		}			
	}
	if( !bPackage && bGift )
		sprintf_s(szTemp, "%s%d%s%d%s", KreonBilling::ShopKey, nProductCount, pUser->GetAccountName(), pUser->m_VecGiftItemList[pUser->m_cCurProductCount].nPrice, KreonBilling::ShopID);			
	else
		sprintf_s(szTemp, "%s%d%s%d%s", KreonBilling::ShopKey, nProductCount, pUser->GetAccountName(), pUser->m_nTotalPrice, KreonBilling::ShopID);

	std::string strKey;
	MD5Hash(szTemp, (int)strlen(szTemp), strKey);
	strPostData.append("&A-KEY=");
	strPostData.append(strKey.c_str());

	int nStatus = pHttpClient->SendRequsetPostA(KreonBilling::ShopUrl, strPostData.c_str());
	if( nStatus != 200) // 200이 아니면 다 에러?
	{
		g_Log.Log(LogType::_ERROR, L"Kreon Billing Coupon GetStatus!(%d)\r\n", nStatus);
		return;
	}
	BYTE byBuff[2048]={0,};
	if( pHttpClient->RecvResponse((LPBYTE)byBuff, 2048) == 0)
	{
		int nCashBalance = 0;
		if( bGift && !bPackage)
			pUser->OnRecvBillingBuyItem(ConvertResult((char*)byBuff, true, nCashBalance ), 0, false);		
		else
			pUser->OnRecvBillingBuyItem(ConvertResult((char*)byBuff, true, nCashBalance ), 0, true);		
	}
}

int CDNBillingConnectionID::OnBuy(CDNUserRepository *pUser)
{
	SendPurchaseItem(pUser, false, false);
	return ERROR_NONE;
}

int CDNBillingConnectionID::OnPackageBuy(CDNUserRepository *pUser)
{
	SendPurchaseItem(pUser, false, true);
	return ERROR_NONE;
}

int CDNBillingConnectionID::OnGift(CDNUserRepository *pUser)
{
	SendPurchaseItem(pUser, true, false);
	return ERROR_NONE;
}

int CDNBillingConnectionID::OnPackageGift(CDNUserRepository *pUser)
{
	SendPurchaseItem(pUser, true, true);
	return ERROR_NONE;
}

int CDNBillingConnectionID::OnCoupon(CDNUserRepository *pUser)
{
	CHttpClient* pHttpClient = g_pHttpCouponManager->FindHttpClient();
	if (!pHttpClient) {
		return ERROR_GENERIC_INVALIDREQUEST;		
	}	
	char szTemp[128] = {0,};

	CHttpClientAuto Auto(pHttpClient, g_pHttpCouponManager);
	
	char szPostData[256]={0,};	
	SNPRINTFA(szPostData, _countof(szPostData),"couponkey=%s&servicecode=%s&userid=%s&remoteip=%s",
		pUser->m_szCoupon, KreonBilling::ShopID, pUser->GetAccountName(), pUser->m_szIp);

	int nStatus = pHttpClient->SendRequsetPostA(KreonBilling::CouponUrl, szPostData);
	if( nStatus != 200) // 200이 아니면 다 에러?
	{
		g_Log.Log(LogType::_ERROR, L"Kreon Billing Shop GetStatus!(%d)\r\n", nStatus);
		return ERROR_KREON_INVALID_REQUEST;
	}
	BYTE byBuff[2048]={0,};
	if( pHttpClient->RecvResponse((LPBYTE)byBuff, 2048) == 0)
	{
		int nItemSN = 0;
		int nRet = ConvertCouponResult((char*)byBuff, nItemSN);	
		if (nRet != ERROR_NONE)
			return nRet;

		pUser->OnRecvBillingCoupon(ERROR_NONE, ERROR_NONE, nItemSN, 0);
	}

	return ERROR_NONE;
}

int CDNBillingConnectionID::ConvertResult(char* strData, bool bShop, int& nCashBalance)
{
	// CashBalance
	// {"RESULT-CODE":"S000","RESULT-MESSAGE":"Success","SID":"DRNEST","CN":"44820790","UID":"kreondn0001","CASH-BALANCE":100000}
	int nResultIndex = 0;
	char* strResult = NULL;
	char* strMessage = NULL;	
	char* strCashBalance = NULL;
	char* token = strtok(strData,"\"");	
	while(token != NULL)
	{
		if( nResultIndex == 3)
			strResult = token;
		else if ( nResultIndex == 7 )
			strMessage = token;		
		else if( !bShop && nResultIndex == 22)		
			strCashBalance = token;
		token = strtok(NULL,"\"");
		++nResultIndex;
	}
	int nRet = ERROR_KREON_INVALID_REQUEST;
	if( strcmp(strResult, "S000") == 0 )
		nRet = ERROR_NONE;
	else if( strcmp(strResult, "E302") == 0 )
		nRet = ERROR_KREON_NOT_ALLOWD_IP;
	else if( strcmp(strResult, "E401") == 0 )
		nRet = ERROR_KREON_BLOCK_GEMSCOOL;
	else if( strcmp(strResult, "E402") == 0 )
		nRet = ERROR_KREON_NO_ENOUGH_POINT;
	else if( strcmp(strResult, "E403") == 0 )
		nRet = ERROR_KREON_LIMIT_AMOUNT;
	else if( strcmp(strResult, "E406") == 0 )
		nRet = ERROR_KREON_NOT_ALLOWD_IP;
	
	if( nRet == ERROR_KREON_INVALID_REQUEST && strMessage)
		g_Log.Log(LogType::_ERROR, L"Kreon Billing Shop Fail!!(%S,%S)\r\n", strResult, strMessage);	
	if( !bShop && nRet == ERROR_NONE)
	{
		if( strCashBalance == NULL)
			nCashBalance = 0;
		else
			nCashBalance = atoi(strCashBalance+1);
	}
	return nRet;
}

int CDNBillingConnectionID::ConvertCouponResult(char* strData,int& nItemSN)
{
	int nResultIndex = 0;
	char* strResult = NULL;
	char* strMessage = NULL;	
	char* strItemID = NULL;
	char* token = strtok(strData,"	");
	while(token != NULL)
	{
		switch(nResultIndex)
		{
		case 0 : strResult = token; break;
		case 1 : strMessage = token; break;		
		case 2 : strItemID = token; break;
		}
		token = strtok(NULL,"	");
		++nResultIndex;
	}
	int nRet = ERROR_NONE;
	if( strcmp(strResult, "S000") == 0 )
		nRet = ERROR_NONE;
	else if( strcmp(strResult, "E001") == 0 )
		nRet = ERROR_NEXONBILLING_62;		// = 1432,								// 쿠폰번호가 올바르지 못함
	else if( strcmp(strResult, "E002") == 0 )
		nRet = ERROR_NEXONBILLING_51;		// = 1428,								// 사용된 쿠폰
	else if( strcmp(strResult, "E010") == 0 )
		nRet = ERROR_NEXONBILLING_65;		// = 1434,								// 쿠폰 기간 만료.
	else if( strcmp(strResult, "E020") == 0 )
		nRet = ERROR_NEXONBILLING_149;      // = 1442,								// 쿠폰 처리 오류(쿠폰디비등)
	else	
		nRet = ERROR_KREON_INVALID_REQUEST;

	if( nRet == ERROR_KREON_INVALID_REQUEST && strMessage)
		g_Log.Log(LogType::_ERROR, L"Kreon Billing Coupon Fail!!(%S,%S)\r\n", strResult, strMessage);	
	if( nRet == ERROR_NONE)
	{
		if( strItemID == NULL)
			nItemSN = 0;
		else
			nItemSN = atoi(strItemID);
	}
	return nRet;
}

#endif	// #if defined(_ID)