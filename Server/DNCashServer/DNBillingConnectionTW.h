#pragma once

#include "Connection.h"

#if defined(_TW)

const int GASH_QUERY_TIMEOUT_TICK = 10000; //���� Ÿ�Ӿƿ� 10��..
const int GASH_TRANSACTION_LEN	= 50;


class CDNUserRepository;
class CDNBillingConnectionTW: public CConnection
{
private:
	DWORD m_dwReconnectTick;
	DWORD m_dwTimeOutTick;
	eConnectionKey m_ConnectionKey;			

	CSyncLock m_TimeOutLock;
	typedef std::map<UINT, DWORD> TTimeOutUser;			// first: AccountDBID, TICK
	TTimeOutUser m_pMapTimeOutList;

	bool AddTimeOut(UINT nAccountDBID, DWORD dTimeOutTick);
	bool DelTimeOut(UINT nAccountDBID);	
	
	int ConvertResult(char* strOutString);	
	int ConvertCouponResult(int nOutStringType);

	CSyncLock m_SendCountLock;
	int m_nSendCount;
	
public:
	CDNBillingConnectionTW(eConnectionKey pConnectionKey, char szIP[IPLENMAX], int nPort);
	virtual ~CDNBillingConnectionTW(void);

	int AddSendData(char *pData, int nLen);

	void DoUpdate(DWORD dwCurTick);
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
	void CouponMessage(char *pData, int nLen);
	void CouponRollBackMessage(char *pData, int nLen);

	int SendPurchaseItem(char *pAccountName, char *pIp, int nPoint, UINT nAccountDBID, INT64 biPurchaseOrderID, int nItemSN);

	// Point Query
	int SendQueryUserPoint(CDNUserRepository *pUser);
	
	// OLD Protocol	
	void SendChargePoint(char *pAccountName, char *pIp, int nPoint, INT64 biPurchaseOrderID, int nItemSN);

	// NEW Protocol
	void SendAccountingWithTransactionID(char *pAccountName, char *pIp, int nPoint, INT64 biPurchaseOrderID, UINT nAccountDBID, int nItemSN);
	void SendPointWriteOff(char* pAccountName, char *pIp, INT64 biPurchaseOrderID, UINT nAccountDBID);

	// Coupon
	void SendCouponUse(char* pCoupon, char* pAccountName, INT64 nCharacterDBID, char cWorldID, char* pIp, UINT nAccountDBID);
	void SendCouponFailResult(char* pCoupon, UINT nAccountDBID);
};

extern CDNBillingConnectionTW *g_pBillingPointConnection;
extern CDNBillingConnectionTW *g_pBillingShopConnection;
extern CDNBillingConnectionTW *g_pBillingCouponConnection;
extern CDNBillingConnectionTW *g_pBillingCouponRollBackConnection;

#endif