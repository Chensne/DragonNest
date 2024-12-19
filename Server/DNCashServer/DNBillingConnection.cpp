#include "StdAfx.h"
#include "DNBillingConnection.h"
#include "DNUserRepository.h"

#if defined(_WORK)

CDNBillingConnection *g_pBillingConnection = NULL;

CDNBillingConnection::CDNBillingConnection(void)
{
}

CDNBillingConnection::~CDNBillingConnection(void)
{
}

int CDNBillingConnection::OnBalanceInquiry(CDNUserRepository *pUser)
{
	pUser->SendBalanceInquiry(ERROR_NONE, 0, 0);
	return ERROR_NONE;
}

int CDNBillingConnection::OnBuy(CDNUserRepository *pUser)
{
	return ERROR_GENERIC_INVALIDREQUEST;
}

int CDNBillingConnection::OnPackageBuy(CDNUserRepository *pUser)
{
	return ERROR_GENERIC_INVALIDREQUEST;
}

int CDNBillingConnection::OnGift(CDNUserRepository *pUser)
{
	return ERROR_GENERIC_INVALIDREQUEST;
}

int CDNBillingConnection::OnPackageGift(CDNUserRepository *pUser)
{
	return ERROR_GENERIC_INVALIDREQUEST;
}

int CDNBillingConnection::OnCoupon(CDNUserRepository *pUser)
{
	return ERROR_GENERIC_INVALIDREQUEST;
}

#endif	// #if defined(_WORK)