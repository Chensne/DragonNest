#pragma once

#include "Connection.h"

#if defined(_TH)

const int ASIASOFT_TRANSACTION_LEN	= 30;

class CDNUserRepository;
class CDNBillingConnectionTH: public CConnection
{
private:
	DWORD m_dwReconnectTick;	
	eConnectionKey m_ConnectionKey;
	
	int ConvertResult(int nResult);	
	
public:
	CDNBillingConnectionTH(eConnectionKey pConnectionKey, char szIP[IPLENMAX], int nPort);
	virtual ~CDNBillingConnectionTH(void);

	int AddSendData(char *pData, int nLen);
	
	void Reconnect(DWORD dwCurTick);
	virtual int MessageProcess(int nMainCmd, int nSubCmd, char *pData, int nLen);

	int OnBalanceInquiry(CDNUserRepository *pUser);
	int OnBuy(CDNUserRepository *pUser);
	int OnPackageBuy(CDNUserRepository *pUser);
	int OnGift(CDNUserRepository *pUser);
	int OnPackageGift(CDNUserRepository *pUser);
	int OnCoupon(CDNUserRepository *pUser);

	void QueryPointMessage(char *pData, int nLen);
	void ShopItemMessage(char *pData, int nLen);		

	// Point Query
	void SendCheckCashBalance(CDNUserRepository *pUser);

	// Item Buy, Gift
	int SendPurchaseItem(CDNUserRepository *pUser, UINT uiProductID, UINT uiPrice, INT64 biPurchaseOrderID, const char* ToGiftUserID=NULL);
};

extern CDNBillingConnectionTH *g_pBillingPointConnection;
extern CDNBillingConnectionTH *g_pBillingShopConnection;

#endif