#pragma once

#if defined(_SG)
#pragma comment (lib, "../ServerCommon/SG/CherryCreditsClients.lib")
#include "../ServerCommon/SG/Transaction.h"

class CDNUserRepository;
class CDNBillingConnectionSG
{
private:
	UINT MakeOrderID();
	int ConvertCouponResult(int nResult);
	int ConvertBillingResult(int nResult);	
	bool GetStatus(const char* AuthToken, char ServerID, const char* CherryID, unsigned int OrderID, unsigned int ProductID, unsigned int TotalAmount);

public:
	CDNBillingConnectionSG(void);
	~CDNBillingConnectionSG(void);

	int OnBalanceInquiry(CDNUserRepository *pUser);
	int OnBuy(CDNUserRepository *pUser);
	int OnPackageBuy(CDNUserRepository *pUser);
	int OnGift(CDNUserRepository *pUser);
	int OnPackageGift(CDNUserRepository *pUser);
	int OnCoupon(CDNUserRepository *pUser);

	//Billing
	int SendCheckBalance(CDNUserRepository *pUser);	// 잔액조회
	void SendPurchaseItem(CDNUserRepository *pUser, UINT uiProductID, UINT uiUnitPrice, UINT uiQuantity, UINT uiTotalAmount);// 아이템 구매
	void SendGift(CDNUserRepository *pUser, UINT uiProductID, UINT uiUnitPrice, UINT uiQuantity, UINT uiTotalAmount, const char* ToCherryID);// 아이템 선물

	// Coupon
	void SendCouponUse(CDNUserRepository *pUser, char const *pCoupon);
};

extern CDNBillingConnectionSG *g_pBillingConnection;

#endif	// #if defined(_SG)


/*
StatusID Definition
10 TRANSACTION: Successfully Completed
11 TRANSACTION: CC Deducted, pending to complete
12 TRANSACTION: Refunded
13 TRANSACTION: Pending Refund
20 TRANSACTION: Failed
21 TRANSACTION: Failed due to insufficient fund
22 TRANSACTION: Failed due to user suspended
23 TRANSACTION: Failed due to duplicated transaction
24 TRANSACTION: Failed due to unknown error
25 TRANSACTION: Order ID does not exist
26 TRANSACTION: Failed to completed due to transaction already been completed
27 TRANSACTION: Failed to refund due to transaction already been refunded
30 REQUEST: Successfully Processed
31 REQUEST: Failed due to invalid packet
32 REQUEST: Failed due to invalid header
33 REQUEST: Failed due to invalid condition ID
34 REQUEST: Failed due to invalid order ID
35 REQUEST: Failed due to invalid product ID
36 REQUEST: Failed due to invalid Cherry ID (buyer)
37 REQUEST: Failed due to invalid unit price
38 REQUEST: Failed due to invalid total amount
39 REQUEST: Failed due to invalid quantity
40 REQUEST: Failed due to invalid Cherry ID (recipient)
41 REQUEST: Failed due to invalid transaction ID
42 REQUEST: Failed due to invalid request
43 REQUEST: Failed due to DB error
44 REQUEST: Failed due to server maintenance
45 REQUEST: Failed due to unknown error
*/