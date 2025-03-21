#pragma once

#if defined(_KRAZ)

class CDNUserRepository;
class CDNBillingConnectionKRAZ
{
private:
	DWORD m_dwReconnectTick;

public:
	CDNBillingConnectionKRAZ(void);
	~CDNBillingConnectionKRAZ(void);

	bool Init();
	void Reconnect(DWORD dwCurTick);

	int OnBalanceInquiry(CDNUserRepository *pUser);
	int OnBuy(CDNUserRepository *pUser);
	int OnPackageBuy(CDNUserRepository *pUser);
	int OnGift(CDNUserRepository *pUser);
	int OnPackageGift(CDNUserRepository *pUser);
	int OnCoupon(CDNUserRepository *pUser);

	static int ConvertResult(const char cResultValue);
	void ItemNameStrToActozStr(int nItemSN, WCHAR *pOut);
};

extern CDNBillingConnectionKRAZ *g_pBillingConnection;

#endif	// #if defined(_KRAZ)