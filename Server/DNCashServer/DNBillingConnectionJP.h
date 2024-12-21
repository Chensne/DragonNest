#pragma once

#if defined(_JP)

class CDNUserRepository;
class CDNSQLMembership;
class CDNSQLWorld;
class CDNBillingConnectionJP
{
public:
	CDNBillingConnectionJP(void);
	~CDNBillingConnectionJP(void);

	int OnBalanceInquiry(CDNUserRepository *pUser);
	int OnBuy(CDNUserRepository *pUser);
	int OnPackageBuy(CDNUserRepository *pUser);
	int OnGift(CDNUserRepository *pUser);
	int OnPackageGift(CDNUserRepository *pUser);
	int OnCoupon(CDNUserRepository *pUser);

	int ConvertResult(int nBillingResult);
	int ConvertGetBillNoResult(int nBillingResult);

	int SendBillingBalanceInquire(char *pAccountName, int &nCashBalance);
	int SendBillingBuy(CDNUserRepository *pUser, CDNSQLMembership *pMembershipDB, CDNSQLWorld *pWorldDB);
	int SendBillingPackageBuy(CDNUserRepository *pUser, CDNSQLMembership *pMembershipDB, CDNSQLWorld *pWorldDB);
	int SendBillingGift(CDNUserRepository *pUser, CDNSQLMembership *pMembershipDB, CDNSQLWorld *pWorldDB);
	int SendBillingPackageGift(CDNUserRepository *pUser, CDNSQLMembership *pMembershipDB, CDNSQLWorld *pWorldDB);
};

extern CDNBillingConnectionJP *g_pBillingConnection;

#endif	// #if defined(_JP)