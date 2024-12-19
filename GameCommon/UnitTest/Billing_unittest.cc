
#include "stdafx.h"
#include "DNExtManager.h"
#include "EtResourceMng.h"
#include "DNConnection.h"
#include "DNUserDeleteManager.h"
#include "DNSQLManager.h"
#include "DNThreadManager.h"
#include "DNManager.h"
#include "Util.h"

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
#elif defined(_SG)
#include "DNBillingConnectionSG.h"
#elif defined(_TH)
#include "DNBillingConnectionTH.h"
#elif defined(_ID)
#include "DNBillingConnectionID.h"
#elif defined(_RU)  || defined (_KAMO)
#include "DNBillingConnectionRU.h"
#endif

extern TCashConfig g_Config;

#if !defined( _FINAL_BUILD ) && !defined (__COVERITY__)

class Billing_unittest:public testing::Test
{
protected:

	static void SetUpTestCase()
	{
		g_Config.szResourcePath = "r:\\GameRes";

		// ResourceMng 积己
		CEtResourceMng::CreateInstance();

		// Path 汲沥
		std::string szResource = g_Config.szResourcePath + "\\Resource";
		std::string szNationStr;
		if( szNationStr.empty() && !g_Config.szResourceNation.empty() ) szNationStr = g_Config.szResourceNation;
		if( !szNationStr.empty() ) 
		{
			szResource += szNationStr;
			CEtResourceMng::GetInstance().AddResourcePath( szResource, true );
			szResource = g_Config.szResourcePath + "\\Resource";
		}

		CEtResourceMng::GetInstance().AddResourcePath( szResource, true );

		g_pExtManager = new CDNExtManager();

		g_pManager = new CDNManager;
		g_pThreadManager = new CDNThreadManager;
		g_pUserDeleteManager = new CDNUserDeleteManager;

#if defined(_WORK)
		g_pBillingConnection = new CDNBillingConnection;
#elif defined(_KR) || defined(_US)
		g_pBillingConnection = new CDNBillingConnectionKR;
#elif defined(_JP)
		g_pBillingConnection = new CDNBillingConnectionJP;
#elif defined(_CH) || defined(_EU)
		g_pBillingConnection = new CDNBillingConnectionCH;
		g_pBillingConnection->Init();
#elif defined(_TW)
		g_pBillingPointConnection = new CDNBillingConnectionTW(CONNECTIONKEY_TW_QUERY, "202.80.108.38", 5412);
		g_pBillingShopConnection = new CDNBillingConnectionTW(CONNECTIONKEY_TW_SHOPITEM, "202.80.108.38", 5413);
		g_pBillingCouponConnection = new CDNBillingConnectionTW(CONNECTIONKEY_TW_COUPON, "210.208.90.119", 5168);
		g_pBillingCouponRollBackConnection = new CDNBillingConnectionTW(CONNECTIONKEY_TW_COUPON_ROLLBACK, "210.208.90.119", 5169);
#elif defined(_SG)
		g_pBillingConnection = new CDNBillingConnectionSG;
#elif defined(_TH)
		g_pBillingPointConnection = new CDNBillingConnectionTH(CONNECTIONKEY_TH_QUERY, "203.144.137.50", 43102);
		g_pBillingShopConnection = new CDNBillingConnectionTH(CONNECTIONKEY_TH_SHOPITEM, "203.144.137.50", 44102);
#elif defined(_ID)
		g_pBillingConnection = new CDNBillingConnectionID(g_Config.nThreadMax);
#elif defined(_RU)  || defined (_KAMO)
		g_pBillingConnection = new CDNBillingConnectionRU;
#endif

		g_pSQLManager = new CDNSQLManager;
		g_pSQLManager->CreateDB();
	}

	static void TearDownTestCase()
	{
#if defined(_CH)
		g_pBillingConnection->Final();
#elif defined(_TW) || defined(_TH)
		SAFE_DELETE(g_pBillingPointConnection);
		SAFE_DELETE(g_pBillingShopConnection);
#if defined(_TW)
		SAFE_DELETE(g_pBillingCouponConnection);
		SAFE_DELETE(g_pBillingCouponRollBackConnection);
#endif //#if defined(_TW)
#else // #if defined(_TW) || defined(_TH)
		SAFE_DELETE(g_pBillingConnection);
#endif	// 

		SAFE_DELETE(g_pManager);
		SAFE_DELETE(g_pUserDeleteManager);
		SAFE_DELETE(g_pThreadManager);
		SAFE_DELETE(g_pSQLManager);
		SAFE_DELETE(g_pExtManager);
	}

	virtual void SetUp() 
	{
		m_pConnection = new CDNConnection();
	}
	virtual void TearDown()
	{
		SAFE_DELETE( m_pConnection );
	}

	CDNConnection *m_pConnection;
};

TEST_F(Billing_unittest, BalanceInquiryTest)
{
	TQCashBalanceInquiry Cash;
	memset(&Cash, 0, sizeof(Cash));
	Cash.bOpen = false;
	Cash.bServer = false;
	Cash.nIp = 0;
	std::string szAccountName;
#if defined(_WORK)
	// 192.168.0.20, 43002, CharacterDBID = 13, qweqwe
	Cash.nAccountDBID = 1;
	szAccountName = "saset";
#elif defined(_KR)
	// 192.168.0.20, 43002, CharacterDBID = 76181, 成成成
	Cash.nAccountDBID = 15606;
	szAccountName = "sassette";
#elif defined(_CH)
	// 192.168.0.41, 43000, CharacterDBID = 496, mumumu
	Cash.nAccountDBID = 251;
	szAccountName = "shandadn003";
#elif defined(_ID)
	// 192.168.0.55, 43000, CharacterDBID = 27, qweqweqweqwe
	Cash.nAccountDBID = 8;
	szAccountName = "kreondn0002";
#elif defined(_JP)
	// 192.168.0.42, 43000, CharacterDBID = 1004, 5443
	Cash.nAccountDBID = 2103;
	szAccountName = "100000003877";
#elif defined(_SG)
	// 192.168.0.51, 43000, CharacterDBID = 154, 69885
	Cash.nAccountDBID = 3020;
	szAccountName = "T1105252191298X";
#elif defined(_TH)
	// 192.168.0.53, 43000, CharacterDBID = 117, abc11
	Cash.nAccountDBID = 27;
	szAccountName = "DNAS0002";
#elif defined(_TW)
	// 192.168.0.43, 43000, CharacterDBID = 322, Cash13
	Cash.nAccountDBID = 2050;
	szAccountName = "rdt000202";
#elif defined(_US)
	// 192.168.0.44, 43000, CharacterDBID = 267, rull,
	Cash.nAccountDBID = 7077;
	szAccountName = "dntester170";
#endif
	sprintf(Cash.szAccountName, "%s", szAccountName.c_str());

	m_pConnection->OnRecvCashShop(0, CASH_BALANCEINQUIRY, (char*)&Cash);
}

TEST_F(Billing_unittest, CashBuyTest)
{
	/*
	TQCashBuy Cash;
	memset(&Cash, 0, sizeof(Cash));

	Cash.cWorldSetID = 1;
	Cash.nMapID = 1;
	Cash.nChannelID = 1;
	Cash.bPetalPurchase = true;
	Cash.cAge = 10;
	Cash.bPCBang = false;
	Cash.cCartType = 3;
#if defined(_VILLAGESERVER) && defined(PRE_ADD_CASH_REFUND)
	Cash.bMoveCashInven = false;
#endif
#if defined(_US)
	Cash.cPaymentRules = PaymentRules::Credit;
#endif	// #if defined(_US)
#if defined(PRE_ADD_SALE_COUPON)
	Cash.biSaleCouponSerial = 0;
#endif
	Cash.cProductCount = 5;
	Cash.BuyList[0].nItemSN = 301000000;
	Cash.BuyList[0].CashItem.nItemID = 1121978368;
	Cash.BuyList[0].CashItem.nSerial = MakeCashSerial(1, LIFESPANMAX);
	Cash.BuyList[0].CashItem.wCount = 1;
	*/
}


#endif // #if !defined( _FINAL_BUILD )
