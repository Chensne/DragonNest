#include "StdAfx.h"
#include "DNBillingConnectionSG.h"
#include "Log.h"
#include "DNUserRepository.h"

extern TCashConfig g_Config;

#if defined(_SG)

#include "../ServerCommon/SG/Soap/CouponSoap/BasicHttpBinding_USCOREICouponService.nsmap"
#include "../ServerCommon/SG/Soap/CouponSoap/soapBasicHttpBinding_USCOREICouponServiceProxy.h"

CDNBillingConnectionSG *g_pBillingConnection = NULL;

CDNBillingConnectionSG::CDNBillingConnectionSG(void)
{	
}

CDNBillingConnectionSG::~CDNBillingConnectionSG(void)
{

}

int CDNBillingConnectionSG::OnBalanceInquiry(CDNUserRepository *pUser)
{
	return SendCheckBalance(pUser);
}

int CDNBillingConnectionSG::OnBuy(CDNUserRepository *pUser)
{
	CDNUserRepository::TBuyItem *pBuyItem = pUser->GetBuyItem(pUser->m_cCurProductCount);
	if (!pBuyItem) return ERROR_GENERIC_INVALIDREQUEST;

	SendPurchaseItem(pUser, pBuyItem->BuyItem.nItemSN, pBuyItem->nPrice, 1, pBuyItem->nPrice);
	return ERROR_NONE;
}

int CDNBillingConnectionSG::OnPackageBuy(CDNUserRepository *pUser)
{
	SendPurchaseItem(pUser, pUser->GetPackageSN(), pUser->m_nTotalPrice, 1, pUser->m_nTotalPrice);
	return ERROR_NONE;
}

int CDNBillingConnectionSG::OnGift(CDNUserRepository *pUser)
{
	CDNUserRepository::TGiftItem *pGiftItem = pUser->GetGiftItem(pUser->m_cCurProductCount);
	if (!pGiftItem) return ERROR_GENERIC_INVALIDREQUEST;

	SendGift(pUser, pGiftItem->GiftItem.nItemSN, pGiftItem->nPrice, 1, pGiftItem->nPrice, pUser->GetReceiverAccountName());
	return ERROR_NONE;
}

int CDNBillingConnectionSG::OnPackageGift(CDNUserRepository *pUser)
{
	SendGift(pUser, pUser->GetPackageSN(), pUser->m_nTotalPrice, 1, pUser->m_nTotalPrice, pUser->GetReceiverAccountName());
	return ERROR_NONE;
}

int CDNBillingConnectionSG::OnCoupon(CDNUserRepository *pUser)
{
	SendCouponUse(pUser, pUser->GetCoupon());
	return ERROR_NONE;
}

// Coupon
void CDNBillingConnectionSG::SendCouponUse(CDNUserRepository *pUser, char const *pCoupon)
{
	// 쿠폰 상태 확인
	BasicHttpBinding_USCOREICouponService service;
	service.endpoint = g_Config.CouponServerInfo.c_str();
	_ns1__ScreenCoupon query;
	_ns1__ScreenCouponResponse ans;
	std::string str_code = std::string(pCoupon);

	int gameID = 1;
	query.gameID = &gameID;
	query.code = &str_code;
	INT64 uiCharacterDBID = pUser->GetCharacterDBID();
	query.charID = &uiCharacterDBID;
	
	if( service.__ns1__ScreenCoupon(&query, &ans) == SOAP_OK )
	{
		int nBillingResult = *(ans.ScreenCouponResult->StatusCode);
		int nResult = ConvertCouponResult(nBillingResult);

		if( nBillingResult != 1)
		{
			pUser->SendCoupon(nResult);
			g_Log.Log(LogType::_ERROR, pUser, L"[CHERRY:SendCouponUse] Coupon Fail! (AccountDBID:%u) %d(%d) %S \r\n", pUser->GetAccountDBID(), nResult, nBillingResult, (*ans.ScreenCouponResult->StatusDescription).c_str());
			return;
		}
		else
		{
			// 아이템 지급, 아이템 타입? 1:Dn Points, 2:Item, 3:Cash Item 			
			// 일단 캐쉬아이템만
			if( *ans.ScreenCouponResult->ItemType == 3 )
			{				
				int nSerial = atoi((*ans.ScreenCouponResult->ItemID).c_str());				
				if( pUser->OnRecvBillingCoupon(ERROR_NONE, ERROR_NONE, nSerial, 0) == true )
				{
					// 쿠폰 사용등록					
					_ns1__UpdateCouponStatus CouponStatusQuery;
					_ns1__UpdateCouponStatusResponse CouponStatusAns;

					std::string str_characterName = pUser->GetCharacterName();
					std::string str_cid = pUser->GetAccountName();					
					std::string str_ip = pUser->m_szIp;

					int gameID = 1;
					INT64 CharacterID = (int)pUser->GetCharacterDBID();
					CouponStatusQuery.gameID = &gameID;
					CouponStatusQuery.characterID = &CharacterID;
					CouponStatusQuery.characterName = &str_characterName;
					CouponStatusQuery.cherryID = &str_cid;
					CouponStatusQuery.code = &str_code;
					CouponStatusQuery.IP = &str_ip;

					if( service.__ns1__UpdateCouponStatus(&CouponStatusQuery, &CouponStatusAns) == SOAP_OK )
					{
						if( *CouponStatusAns.UpdateCouponStatusResult == false )						
							g_Log.Log(LogType::_ERROR, pUser, L"[CHERRY:SendCouponUse] Coupon UpdateStatus Fail! Coupon:%S (AccountDBID:%u)\r\n", pCoupon, pUser->GetAccountDBID());
						else
							return;
					}
					else
						g_Log.Log(LogType::_ERROR, pUser, L"[CHERRY:SendCouponUse] Coupon UpdateStatus Connection Fail! Coupon:%S (AccountDBID:%u)\r\n", pCoupon, pUser->GetAccountDBID());
				}
				else
				{
					g_Log.Log(LogType::_ERROR, pUser, L"[CHERRY:SendCouponUse] OnRecvBillingCoupon false! Coupon:%S (AccountDBID:%u)\r\n", pCoupon, pUser->GetAccountDBID());
					return;	// OnRecvBillingCoupon 안에서 send보냄.
				}
			}
			else			
				g_Log.Log(LogType::_ERROR, pUser, L"[CHERRY:SendCouponUse] Coupon Fail! Coupon:%S ItemType:%d (AccountDBID:%u)\r\n", pCoupon, *ans.ScreenCouponResult->ItemType, pUser->GetAccountDBID());			
		}			
	}
	else	
		g_Log.Log(LogType::_ERROR, pUser, L"[CHERRY:SendCouponUse] Coupon Fail! (AccountDBID:%u) %d Connection Fail! \r\n", pUser->GetAccountDBID(), ERROR_GENERIC_UNKNOWNERROR);
	
	pUser->SendCoupon(ERROR_GENERIC_UNKNOWNERROR);
	return;
}

int CDNBillingConnectionSG::SendCheckBalance(CDNUserRepository *pUser)
{
	ReceivedData result;
	UINT uiOderID =  MakeOrderID();
	result = Transaction::GetBalance(g_Config.AuthToken.c_str(), pUser->GetWorldID(), pUser->GetAccountName(), uiOderID);
	if( result.StatusID == 30 ) // Error_None
		pUser->SendBalanceInquiry(ERROR_NONE, result.CherryCredits);
	else
	{
		g_Log.Log(LogType::_ERROR, pUser, L"[CHERRY:SendCheckBalance] Fail!! (AccountDBID:%u) Order:%u result:%d \r\n", pUser->GetAccountDBID(), uiOderID, result.StatusID);		
		return ERROR_GENERIC_INVALIDREQUEST;
	}

	return ERROR_NONE;
}

void CDNBillingConnectionSG::SendPurchaseItem(CDNUserRepository *pUser, UINT uiProductID, UINT uiUnitPrice, UINT uiQuantity, UINT uiTotalAmount)
{
	ReceivedData result;
	UINT uiOderID =  MakeOrderID();
	result = Transaction::GetPurchase(g_Config.AuthToken.c_str(), pUser->GetWorldID(), pUser->GetAccountName(), uiOderID, uiProductID, uiUnitPrice,
		uiQuantity, uiTotalAmount, pUser->m_szIp);
	
	if( result.StatusID == 10 || result.StatusID == 30 ||result.StatusID == 11) // Error_None
	{
		// Compelete
		result = Transaction::GetCompleted(g_Config.AuthToken.c_str(), pUser->GetWorldID(), pUser->GetAccountName(), uiOderID, result.TransactionID);
		if( result.StatusID == 10 || result.StatusID == 30 )		
		{
			pUser->OnRecvBillingBuyItem(ERROR_NONE);
			return;
		}
		else if( result.StatusID == 11 ) // CC deducted, pending to complete
		{
			if( GetStatus(g_Config.AuthToken.c_str(), pUser->GetWorldID(), pUser->GetAccountName(), uiOderID,uiProductID, uiTotalAmount) == true )
			{
				pUser->OnRecvBillingBuyItem(ERROR_NONE);
				return;
			}			
		}
		pUser->OnRecvBillingBuyItem(ERROR_GENERIC_UNKNOWNERROR);
		g_Log.Log(LogType::_ERROR, pUser, L"[CHERRY:SendPurchaseItem] GetCompleted Fail!! (AccountDBID:%u) Order:%u result:%d ProductID:%u \r\n", pUser->GetAccountDBID(), uiOderID, result.StatusID, uiProductID);	
		return;
	}
	else if( result.ServerID == 25 ) //Order ID does not exist
	{
		if( GetStatus(g_Config.AuthToken.c_str(), pUser->GetWorldID(), pUser->GetAccountName(), uiOderID,uiProductID, uiTotalAmount) == true )
		{
			// Compelete
			result = Transaction::GetCompleted(g_Config.AuthToken.c_str(), pUser->GetWorldID(), pUser->GetAccountName(), uiOderID, result.TransactionID);
			if( result.StatusID == 10 || result.StatusID == 30 )	
			{
				pUser->OnRecvBillingBuyItem(ERROR_NONE);
				return;
			}
			else if( result.StatusID == 11 ) // CC deducted, pending to complete
			{
				if( GetStatus(g_Config.AuthToken.c_str(), pUser->GetWorldID(), pUser->GetAccountName(), uiOderID,uiProductID, uiTotalAmount) == true )
				{
					pUser->OnRecvBillingBuyItem(ERROR_NONE);
					return;
				}			
			}
			pUser->OnRecvBillingBuyItem(ERROR_GENERIC_UNKNOWNERROR);
			g_Log.Log(LogType::_ERROR, pUser, L"[CHERRY:SendPurchaseItem] GetCompleted Fail!! (AccountDBID:%u) Order:%u result:%d ProductID:%u \r\n", pUser->GetAccountDBID(), uiOderID, result.StatusID, uiProductID);	
			return;
		}		
	}	
	pUser->OnRecvBillingBuyItem(ERROR_GENERIC_UNKNOWNERROR);
	g_Log.Log(LogType::_ERROR, pUser, L"[CHERRY:SendPurchaseItem] GetPurchase Fail!! (AccountDBID:%u) Order:%u result:%d ProductID:%u \r\n", pUser->GetAccountDBID(), uiOderID, result.StatusID, uiProductID);		
}

void CDNBillingConnectionSG::SendGift(CDNUserRepository *pUser, UINT uiProductID, UINT uiUnitPrice, UINT uiQuantity, UINT uiTotalAmount, const char* ToCherryID)
{
	ReceivedData result;
	UINT uiOderID =  MakeOrderID();
	result = Transaction::GetSendGift(g_Config.AuthToken.c_str(), pUser->GetWorldID(), pUser->GetAccountName(), uiOderID, uiProductID, uiUnitPrice,
		uiQuantity, uiTotalAmount, ToCherryID, pUser->m_szIp);

	if( result.StatusID == 10 || result.StatusID == 30 || result.StatusID == 11) // Error_None
	{
		// Compelete
		result = Transaction::GetCompleted(g_Config.AuthToken.c_str(), pUser->GetWorldID(), pUser->GetAccountName(), uiOderID, result.TransactionID);
		if( result.StatusID == 10 || result.StatusID == 30 )	
		{
			pUser->OnRecvBillingBuyItem(ERROR_NONE);
			return;
		}
		else if( result.StatusID == 11 ) // CC deducted, pending to complete
		{
			if( GetStatus(g_Config.AuthToken.c_str(), pUser->GetWorldID(), pUser->GetAccountName(), uiOderID,uiProductID, uiTotalAmount) == true )
			{
				pUser->OnRecvBillingBuyItem(ERROR_NONE);
				return;
			}
		}
		pUser->OnRecvBillingBuyItem(ERROR_GENERIC_UNKNOWNERROR);
		g_Log.Log(LogType::_ERROR, pUser, L"[CHERRY:SendGift] GetCompleted Fail!! (AccountDBID:%u) Order:%u result:%d ProductID:%u \r\n", pUser->GetAccountDBID(), uiOderID, result.StatusID, uiProductID);	
		return;
	}
	else if( result.ServerID == 25 ) //Order ID does not exist
	{
		if( GetStatus(g_Config.AuthToken.c_str(), pUser->GetWorldID(), pUser->GetAccountName(), uiOderID,uiProductID, uiTotalAmount) == true )
		{
			// Compelete
			result = Transaction::GetCompleted(g_Config.AuthToken.c_str(), pUser->GetWorldID(), pUser->GetAccountName(), uiOderID, result.TransactionID);
			if( result.StatusID == 10 || result.StatusID == 30 )	
			{
				pUser->OnRecvBillingBuyItem(ERROR_NONE);
				return;
			}
			else if( result.StatusID == 11 ) // CC deducted, pending to complete
			{
				if( GetStatus(g_Config.AuthToken.c_str(), pUser->GetWorldID(), pUser->GetAccountName(), uiOderID,uiProductID, uiTotalAmount) == true )
				{
					pUser->OnRecvBillingBuyItem(ERROR_NONE);
					return;
				}
			}
			pUser->OnRecvBillingBuyItem(ERROR_GENERIC_UNKNOWNERROR);
			g_Log.Log(LogType::_ERROR, pUser, L"[CHERRY:SendGift] GetCompleted Fail!! (AccountDBID:%u) Order:%u result:%d ProductID:%u\r\n", pUser->GetAccountDBID(), uiOderID, result.StatusID, uiProductID);	
			return;
		}		
	}	
	pUser->OnRecvBillingBuyItem(ERROR_GENERIC_UNKNOWNERROR);	
	g_Log.Log(LogType::_ERROR, pUser, L"[CHERRY:SendGift] GetSendGift Fail!! (AccountDBID:%u) Order:%u result:%d ProductID:%u \r\n", pUser->GetAccountDBID(), uiOderID, result.StatusID, uiProductID);	
}

// GetPurchase, GetSendGift, GetCompleted 검사
bool CDNBillingConnectionSG::GetStatus(const char* AuthToken, char ServerID, const char* CherryID, unsigned int OrderID, unsigned int ProductID, unsigned int TotalAmount)
{
	ReceivedData result;
	for( int i=0; i<3; ++i )
	{
		result = Transaction::GetStatus(AuthToken, ServerID, CherryID, OrderID, ProductID, TotalAmount);
		if( result.StatusID == 10 || result.StatusID == 30 )
			return true;
	}
	g_Log.Log(LogType::_ERROR, L"[CHERRY:GetStatus] GetStatus Fail!! Order:%u result:%d ProductID:%u\r\n", OrderID, result.StatusID, ProductID);		
	return false;
}

int CDNBillingConnectionSG::ConvertCouponResult(int nResult)
{
	int nRet = ERROR_GENERIC_UNKNOWNERROR;
	switch(nResult)
	{
	case -1 : nRet = ERROR_CHERRY_COUPON_ALREADY; break;
	case -2 : nRet = ERROR_CHERRY_COUPON_EXPRIED; break;		
	case -3 : nRet = ERROR_CHERRY_COUPON_INVALID; break;
	case -4 : nRet = ERROR_CHERRY_COUPON_AND_CHARACTERID_MISMATCH; break;
	case -99 :		
	case -999 :
		nRet = ERROR_GENERIC_UNKNOWNERROR;
		break;
	}
	return nRet;
}

int CDNBillingConnectionSG::ConvertBillingResult(int nResult)
{
	int nRet = ERROR_GENERIC_UNKNOWNERROR;
	switch(nResult)
	{
	case -1 : nRet = ERROR_CHERRY_COUPON_ALREADY; break;
	case -2 : nRet = ERROR_CHERRY_COUPON_EXPRIED; break;		
	case -3 : nRet = ERROR_CHERRY_COUPON_INVALID; break;		
	case -99 :		
	case -999 :
		nRet = ERROR_GENERIC_UNKNOWNERROR;
		break;
	}
	return nRet;
}

UINT CDNBillingConnectionSG::MakeOrderID()
{
	static long Adder = rand();

	time_t CreateTime;
	time(&CreateTime);
	tm *tm_ptr = localtime(&CreateTime);

	long Local = static_cast<short>(InterlockedIncrement( &Adder ));

	UINT nTest = (static_cast<UINT>(tm_ptr->tm_mday&0xF)<<28) | (static_cast<UINT>(tm_ptr->tm_hour&0xF)<<24) 
		| (static_cast<UINT>(tm_ptr->tm_min&0xF)<<20) | (static_cast<UINT>(tm_ptr->tm_sec&0xF)<<16) | (Local&0xFFFF);

	return nTest;
}
#endif	// #if defined(_SG)