#pragma once

#if defined(_ID)
namespace KreonBilling
{	
	const static char* ShopID = {"DRNEST"};
	const static char* ShopKey = {"DN324NE90WIN"};
	const static TCHAR* ShopServerAddress = {_T("gas.gemscool.com")};
	const static TCHAR* BalanceUrl = {_T("/gpay/balance.gpay")};	
	const static TCHAR* ShopUrl = {_T("/gpay/payment.gpay")};

	const static TCHAR* CouponServerAddress = {_T("gas.gemscool.com")};
	const static TCHAR* CouponUrl = {_T("/gcoupon/getCouponCheckWin.kreon")};
};
class CDNUserRepository;
class CDNSQLMembership;
class CDNSQLWorld;
class CDNBillingConnectionID
{
public:
	CDNBillingConnectionID(int nWorkerThreadSize);
	~CDNBillingConnectionID(void);

	int OnBalanceInquiry(CDNUserRepository *pUser);
	int OnBuy(CDNUserRepository *pUser);
	int OnPackageBuy(CDNUserRepository *pUser);
	int OnGift(CDNUserRepository *pUser);
	int OnPackageGift(CDNUserRepository *pUser);
	int OnCoupon(CDNUserRepository *pUser);

	void SendPurchaseItem(CDNUserRepository* pUser, bool bGift, bool bPackage);
	int ConvertResult(char* strData, bool bShop, int& nCashBalance);	
	int ConvertCouponResult(char* strData, int& nItemSN);
};

extern CDNBillingConnectionID *g_pBillingConnection;

#endif	// #if defined(_ID)