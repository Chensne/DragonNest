#include "StdAfx.h"
#include "DNBillingConnectionRU.h"
#include "DNConnection.h"
#include "DNUserRepository.h"
#include "DNExtManager.h"
#include "DNSQLMembership.h"
#include "DNSQLWorld.h"
#include "Log.h"

extern TCashConfig g_Config;

#if defined(_RU)

CDNBillingConnectionRU *g_pBillingConnection = NULL;

CDNBillingConnectionRU::CDNBillingConnectionRU(void)
{
}

CDNBillingConnectionRU::~CDNBillingConnectionRU(void)
{
}

int CDNBillingConnectionRU::OnBalanceInquiry(CDNUserRepository *pUser)
{
	CDNSQLMembership *pMembershipDB = pUser->GetMembershipDB();
	if (!pMembershipDB){
		return ERROR_GENERIC_INVALIDREQUEST;
	}

	int nCashBalance = 0;
	pMembershipDB->QueryGetCashBalance(pUser->GetAccountDBID(), NULL, nCashBalance);
	pUser->SendBalanceInquiry(ERROR_NONE, nCashBalance, 0);

	return ERROR_NONE;
}

int CDNBillingConnectionRU::OnBuy(CDNUserRepository *pUser)
{
	pUser->OnRecvBillingBuyItem(ERROR_NONE, 0, true);
	return ERROR_NONE;
}

int CDNBillingConnectionRU::OnPackageBuy(CDNUserRepository *pUser)
{
	pUser->OnRecvBillingBuyItem(ERROR_NONE, 0, false);
	return ERROR_NONE;
}

int CDNBillingConnectionRU::OnGift(CDNUserRepository *pUser)
{
	pUser->OnRecvBillingBuyItem(ERROR_NONE, 0, true);
	return ERROR_NONE;
}

int CDNBillingConnectionRU::OnPackageGift(CDNUserRepository *pUser)
{
	pUser->OnRecvBillingBuyItem(ERROR_NONE, 0, false);
	return ERROR_NONE;
}

int CDNBillingConnectionRU::OnCoupon(CDNUserRepository *pUser)
{
	return ERROR_GENERIC_INVALIDREQUEST;
}

#endif	// #if defined(_RU)