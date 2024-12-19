#include "StdAfx.h"
#include "DNBillingConnectionTH.h"
#include "DNIocpManager.h"
#include "DNUserRepository.h"
#include "DNManager.h"
#include "Log.h"
#include "TimeSet.h"

extern TCashConfig g_Config;

#if defined(_TH)

CDNBillingConnectionTH *g_pBillingPointConnection = NULL;
CDNBillingConnectionTH *g_pBillingShopConnection = NULL;

CDNBillingConnectionTH::CDNBillingConnectionTH(eConnectionKey pConnectionKey, char szIP[IPLENMAX], int nPort): CConnection()
{
	SetIp(szIP);
	SetPort(nPort);

	Init(1024*200, 1024*200);
	m_dwReconnectTick = 0;	
	m_ConnectionKey = pConnectionKey;		
}

CDNBillingConnectionTH::~CDNBillingConnectionTH(void)
{
	
}

int CDNBillingConnectionTH::AddSendData(char *pData, int nLen)
{
	int Ret = 0;

	if (GetDelete()) return -1;
	if (m_pIocpManager && m_pSocketContext && (m_pSocketContext->m_Socket != INVALID_SOCKET))
	{
		m_SendSync.Lock();
		Ret = m_pSendBuffer->Push( pData, nLen );						// ������
		m_SendSync.UnLock();

		if (Ret == 0)
		{
			m_pIocpManager->m_nAddSendBufSize += nLen;
			m_pIocpManager->AddSendCall(m_pSocketContext);
		}
		else if (Ret < 0)
			m_pIocpManager->DetachSocket(m_pSocketContext, L"AddSendData Error");		
	}
	return Ret;
}

void CDNBillingConnectionTH::Reconnect(DWORD dwCurTick)
{
	if (m_dwReconnectTick + 5000 < dwCurTick){
		m_dwReconnectTick = dwCurTick;

		if (!GetActive() && !GetConnecting() )
		{
			SetConnecting(true);
			if (g_pIocpManager->AddConnectionEx(this, m_ConnectionKey, m_szIP, m_wPort) < 0)
			{
				SetConnecting(false);
				g_Log.Log(LogType::_ERROR, L"BillingServer Fail(%S, %d)\r\n", m_szIP, m_wPort);
			}
			else {
				g_Log.Log(LogType::_NORMAL, L"BillingServer(%S, %d) Connecting..\r\n", m_szIP, m_wPort);
				
			}
		}
	}	
}

int CDNBillingConnectionTH::MessageProcess(int nMainCmd, int nSubCmd, char *pData, int nLen)
{	
	switch(m_ConnectionKey)
	{
	case CONNECTIONKEY_TH_QUERY :
		QueryPointMessage(pData, nLen);
		break;
	case CONNECTIONKEY_TH_SHOPITEM :
		ShopItemMessage(pData, nLen);
		break;
	}
	
	return ERROR_NONE;
}

int CDNBillingConnectionTH::OnBalanceInquiry(CDNUserRepository *pUser)
{
	SendCheckCashBalance(pUser);
	return ERROR_NONE;
}

int CDNBillingConnectionTH::OnBuy(CDNUserRepository *pUser)
{
	CDNUserRepository::TBuyItem *pBuyItem = pUser->GetBuyItem(pUser->m_cCurProductCount);
	if (!pBuyItem) return ERROR_GENERIC_INVALIDREQUEST;

	SendPurchaseItem(pUser, pBuyItem->BuyItem.nItemSN, pBuyItem->nPrice, pUser->GetPurchaseOrderID());
	return ERROR_NONE;
}

int CDNBillingConnectionTH::OnPackageBuy(CDNUserRepository *pUser)
{
	SendPurchaseItem(pUser, pUser->GetPackageSN(), pUser->m_nTotalPrice, pUser->GetPurchaseOrderID());
	return ERROR_NONE;
}

int CDNBillingConnectionTH::OnGift(CDNUserRepository *pUser)
{
	CDNUserRepository::TGiftItem *pGiftItem = pUser->GetGiftItem(pUser->m_cCurProductCount);
	if (!pGiftItem) return ERROR_GENERIC_INVALIDREQUEST;

	SendPurchaseItem(pUser, pGiftItem->GiftItem.nItemSN, pGiftItem->nPrice, pGiftItem->biPurchaseOrderID, pUser->GetReceiverAccountName());
	return ERROR_NONE;
}

int CDNBillingConnectionTH::OnPackageGift(CDNUserRepository *pUser)
{
	SendPurchaseItem(pUser, pUser->GetPackageSN(), pUser->m_nTotalPrice, pUser->GetPurchaseOrderID(), pUser->GetReceiverAccountName());
	return ERROR_NONE;
}

int CDNBillingConnectionTH::OnCoupon(CDNUserRepository *pUser)
{
	return ERROR_GENERIC_INVALIDREQUEST;
}

void CDNBillingConnectionTH::QueryPointMessage(char *pData, int nLen)
{	
	int nResultIndex = 0;
	char* token = strtok(pData,"|");
	int nResult = 0;
	char* strCash = NULL;
	char* strBonus = NULL;
	char* strOutString = NULL;
	
	UINT nAccountDBID = 0;
	while(token != NULL )
	{
		// error_code|error_desc|Balancecash|BalanceBonus|sock_id;
		switch( nResultIndex )
		{
		case 0 : nResult = atoi(token); break;
		case 1 : strOutString = token; break;
		case 2 : strCash = token; break;		
		case 3 : strBonus = token; break;		
		case 4 : nAccountDBID = strtoul(token, NULL, 10); break;
		}
		token = strtok(NULL,"|");
		++ nResultIndex;
	}

	CDNUserRepository *pUser = g_pManager->GetUser(nAccountDBID);
	if (!pUser){
		g_Log.Log(LogType::_ERROR, L"[AsiaSoft:QueryPointMessage] User NULL! (AccountDBID:%u) \r\n", nAccountDBID);
		return;
	}	

#if defined (PRE_ADD_REJECT_DUPLICATED_CASH_REQUEST)
	CRequestReleaser releaser(pUser->m_Requesting);
#endif // #if defined (PRE_ADD_REJECT_DUPLICATED_CASH_REQUEST)

	if( nResult == 0)	
		pUser->SendBalanceInquiry(ERROR_NONE, atoi(strCash)+atoi(strBonus));
	else if ( nResult == -8001) // �ѹ��� ���� ���� ����(Invalid UserID)
		pUser->SendBalanceInquiry(ERROR_NONE, 0);
	else
	{
		//��Ÿ ������ ����ó��..
		int nConvertResult = ConvertResult(nResult);
		pUser->SendBalanceInquiry(nConvertResult, 0);
		g_Log.Log(LogType::_ERROR, pUser, L"[AsiaSoft:QueryPointMessage] Point Query Fail! (AccountDBID:%u) %d:%S\r\n", nAccountDBID, nResult, strOutString);	
	}
}

void CDNBillingConnectionTH::ShopItemMessage(char *pData, int nLen)
{	
	int nResultIndex = 0;
	char* token = strtok(pData,"|");
	int nResult = 0;
	char* strCash = NULL;
	char* strBonus = NULL;
	char* strOutString = NULL;
	UINT nOrderNo = 0;

	UINT nAccountDBID = 0;
	
	//error_code|error_desc|Balancecash|BalanceBonus|CashTransactionconfirm|sock_id;
	while(token != NULL )
	{
		switch( nResultIndex )
		{
		case 0 : nResult = atoi(token); break;
		case 1 : strOutString = token; break;
		case 2 : strCash = token; break;		
		case 3 : strBonus = token; break;
		case 4 : nOrderNo = atoi(token); break;
		case 5 : nAccountDBID = strtoul(token, NULL, 10); break;		
		}
		token = strtok(NULL,"|");
		++ nResultIndex;
	}	

	CDNUserRepository *pUser = g_pManager->GetUser(nAccountDBID);
	if (!pUser){
		g_Log.Log(LogType::_ERROR, L"[AsiaSoft:ShopItemMessage] User NULL! (AccountDBID:%d) \r\n", nAccountDBID);
		return;
	}	

	if( nResult == 0)			
		pUser->OnRecvBillingBuyItem(ERROR_NONE, nOrderNo);
	else
	{
		pUser->OnRecvBillingBuyItem(ConvertResult(nResult));
		g_Log.Log(LogType::_ERROR, pUser, L"[AsiaSoft:ShopItemMessage] Item Purchase Fail! (AccountDBID:%u) %S \r\n", nAccountDBID, strOutString);
	}
}

void CDNBillingConnectionTH::SendCheckCashBalance(CDNUserRepository *pUser)
{		
	if (!pUser)
	{
		g_Log.Log(LogType::_ERROR, L"[AsiaSoft:SendQueryUserPoint] User NULL! (AccountDBID:%u) \r\n", pUser->GetAccountDBID());
		return;
	}

	char strMessage[256] = {0,};
	SNPRINTFA(strMessage, 256, "%s|102|%s|%d;", pUser->GetAccountName(), pUser->m_szIp, pUser->GetAccountDBID());

	AddSendData(strMessage, static_cast<int>(::strlen(strMessage)));	
}

int CDNBillingConnectionTH::SendPurchaseItem(CDNUserRepository *pUser, UINT uiProductID, UINT uiPrice, INT64 biPurchaseOrderID, const char* ToGiftUserID)
{		
	if (!pUser)
	{
		g_Log.Log(LogType::_ERROR, L"[AsiaSoft:SendPurchaseItem] User NULL! (AccountDBID:%u) \r\n", pUser->GetAccountDBID());
		return ERROR_GENERIC_UNKNOWNERROR;
	}

	char szTransactionID[ASIASOFT_TRANSACTION_LEN] = {0,};	
	CTimeSet aCurTime;
	// Ʈ����� ID
	SNPRINTFA(szTransactionID, ASIASOFT_TRANSACTION_LEN, "%04d%02d%02d%I64d", aCurTime.GetYear(), aCurTime.GetMonth(), aCurTime.GetDay(), biPurchaseOrderID);

	char strMessage[256] = {0,};
	if(ToGiftUserID) // ��� ����
		SNPRINTFA(strMessage, 256, "%s|%s|%d|%d|1|102|1|%s|%s|%d;", pUser->GetAccountName(), ToGiftUserID, uiPrice, uiProductID, szTransactionID, pUser->m_szIp, pUser->GetAccountDBID());
	else
		SNPRINTFA(strMessage, 256, "%s|%s|%d|%d|1|102|1|%s|%s|%d;", pUser->GetAccountName(), pUser->GetAccountName(), uiPrice, uiProductID, szTransactionID, pUser->m_szIp, pUser->GetAccountDBID());

	AddSendData(strMessage, static_cast<int>(::strlen(strMessage)));
	return ERROR_NONE;
}

int CDNBillingConnectionTH::ConvertResult(int nResult)
{	
	int nRet = ERROR_NONE;

	switch(nResult)
	{		
		case -8003 : //Invalid ServiceID or Invalid Input Item Detail (ItemID, Item Price not match with billing)
			nRet = ERROR_ASIASOFT_NO_SUCH_SERVICE;
			break;			
		case -8004: // Not Enough Cash to buy Item
			nRet = ERROR_ASIASOFT_NO_ENOUGH_POINT;
			break;
		case -1001 : // Invalid request parameters.		
		case -8002 : // Recoed not found		
		case -8005 : //System Error
		case -8006 : //Invalid ServiceID		
		case -9009 : // DB error.
		case -9010 : // Unexpected error.			
		default:
			nRet = ERROR_GENERIC_UNKNOWNERROR;
	}	
	return nRet;	
}

#endif