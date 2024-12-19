#pragma once

#if defined(_RU)

class CDNUserRepository;
class CDNBillingConnectionRU
{
public:
	CDNBillingConnectionRU(void);
	~CDNBillingConnectionRU(void);

	int OnBalanceInquiry(CDNUserRepository *pUser);
	int OnBuy(CDNUserRepository *pUser);
	int OnPackageBuy(CDNUserRepository *pUser);
	int OnGift(CDNUserRepository *pUser);
	int OnPackageGift(CDNUserRepository *pUser);
	int OnCoupon(CDNUserRepository *pUser);
};

extern CDNBillingConnectionRU *g_pBillingConnection;

#endif	// #if defined(_RU)