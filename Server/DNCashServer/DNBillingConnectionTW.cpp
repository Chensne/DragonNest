#include "StdAfx.h"
#include "DNBillingConnectionTW.h"
#include "DNIocpManager.h"
#include "DNUserRepository.h"
#include "DNManager.h"
#include "Log.h"
#include "TimeSet.h"

extern TCashConfig g_Config;

#if defined(_TW)

CDNBillingConnectionTW *g_pBillingPointConnection = NULL;
CDNBillingConnectionTW *g_pBillingShopConnection = NULL;
CDNBillingConnectionTW *g_pBillingCouponConnection = NULL;
CDNBillingConnectionTW *g_pBillingCouponRollBackConnection = NULL;

CDNBillingConnectionTW::CDNBillingConnectionTW(eConnectionKey pConnectionKey, char szIP[IPLENMAX], int nPort): CConnection()
{
	SetIp(szIP);
	SetPort(nPort);

	Init(1024*200, 1024*200);
	m_dwReconnectTick = 0;
	m_dwTimeOutTick = 0;
	m_ConnectionKey = pConnectionKey;		
	m_nSendCount = 0;
}

CDNBillingConnectionTW::~CDNBillingConnectionTW(void)
{
	
}

int CDNBillingConnectionTW::AddSendData(char *pData, int nLen)
{
	int Ret = 0;

	if (GetDelete()) return -1;
	if (m_pIocpManager && m_pSocketContext && (m_pSocketContext->m_Socket != INVALID_SOCKET))
	{
		m_SendSync.Lock();
		Ret = m_pSendBuffer->Push( pData, nLen );						// 데이터
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

void CDNBillingConnectionTW::DoUpdate(DWORD dwCurTick)
{
	if( g_Config.nProtocolType == 2 )
	{
		if( m_dwTimeOutTick + 3000 < dwCurTick )
		{
			m_dwTimeOutTick = dwCurTick;
			ScopeLock<CSyncLock> Lock(m_TimeOutLock);
			TTimeOutUser::iterator iter;
			for (iter = m_pMapTimeOutList.begin(); iter != m_pMapTimeOutList.end(); ++iter)
			{
				// 타임아웃~~
				if( iter->second + GASH_QUERY_TIMEOUT_TICK < dwCurTick )
				{
					// 존재하는 유저인지 뒤지고..
					CDNUserRepository *pUser = g_pManager->GetUser(iter->first);
					if (!pUser)
					{
						g_Log.Log(LogType::_ERROR, L"[GASH:DoUpdate] User NULL! (AccountDBID:%u) \r\n", iter->first);
						m_pMapTimeOutList.erase(iter);
						continue;
					}
					SendPointWriteOff(pUser->GetAccountName(), pUser->m_szIp, pUser->GetPurchaseOrderID(), pUser->GetAccountDBID());
					// 보내고 강제로 셋팅
					iter->second = dwCurTick;
				}
			}
		}
	}	
}

bool CDNBillingConnectionTW::AddTimeOut(UINT nAccountDBID, DWORD dTimeOutTick)
{
	ScopeLock<CSyncLock> Lock(m_TimeOutLock);

	std::pair<TTimeOutUser::iterator, bool> RetVal = m_pMapTimeOutList.insert(TTimeOutUser::value_type(nAccountDBID, dTimeOutTick));
	if (!RetVal.second) return false;

	return true;
}

bool CDNBillingConnectionTW::DelTimeOut(UINT nAccountDBID)
{
	if (m_pMapTimeOutList.empty()) return false;

	ScopeLock<CSyncLock> Lock(m_TimeOutLock);

	TTimeOutUser::iterator iter = m_pMapTimeOutList.find(nAccountDBID);
	if (iter == m_pMapTimeOutList.end()) return false;

	m_pMapTimeOutList.erase(iter);	
	return true;
}

void CDNBillingConnectionTW::Reconnect(DWORD dwCurTick)
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

int CDNBillingConnectionTW::MessageProcess(int nMainCmd, int nSubCmd, char *pData, int nLen)
{	
	switch(m_ConnectionKey)
	{
	case CONNECTIONKEY_TW_QUERY :
		QueryPointMessage(pData, nLen);
		break;
	case CONNECTIONKEY_TW_SHOPITEM :
		ShopItemMessage(pData, nLen);
		break;
	case CONNECTIONKEY_TW_COUPON :
		CouponMessage(pData, nLen);
		break;
	case CONNECTIONKEY_TW_COUPON_ROLLBACK :
		CouponRollBackMessage(pData, nLen);
		break;
	}
	
	return ERROR_NONE;
}

int CDNBillingConnectionTW::OnBalanceInquiry(CDNUserRepository *pUser)
{
	if (!m_bActive)
		return ERROR_GENERIC_INVALIDREQUEST;

	SendQueryUserPoint(pUser);
	return ERROR_NONE;
}

int CDNBillingConnectionTW::OnBuy(CDNUserRepository *pUser)
{
	if (!m_bActive)
		return ERROR_GENERIC_INVALIDREQUEST;

	CDNUserRepository::TBuyItem *pBuyItem = pUser->GetBuyItem(pUser->m_cCurProductCount);
	if (!pBuyItem) return ERROR_GENERIC_INVALIDREQUEST;

	SendPurchaseItem(pUser->GetAccountName(), pUser->m_szIp, pBuyItem->nPrice, pUser->GetAccountDBID(), pUser->GetPurchaseOrderID(), pBuyItem->BuyItem.nItemSN);	
	return ERROR_NONE;
}

int CDNBillingConnectionTW::OnPackageBuy(CDNUserRepository *pUser)
{
	if (!m_bActive)
		return ERROR_GENERIC_INVALIDREQUEST;

	SendPurchaseItem(pUser->GetAccountName(), pUser->m_szIp, pUser->m_nTotalPrice, pUser->GetAccountDBID(), pUser->GetPurchaseOrderID(), pUser->GetPackageSN());
	return ERROR_NONE;
}

int CDNBillingConnectionTW::OnGift(CDNUserRepository *pUser)
{
	if (!m_bActive)
		return ERROR_GENERIC_INVALIDREQUEST;

	CDNUserRepository::TGiftItem *pGiftItem = pUser->GetGiftItem(pUser->m_cCurProductCount);
	if (!pGiftItem) return ERROR_GENERIC_INVALIDREQUEST;

	SendPurchaseItem(pUser->GetAccountName(), pUser->m_szIp, pGiftItem->nPrice, pUser->GetAccountDBID(), pGiftItem->biPurchaseOrderID, pGiftItem->GiftItem.nItemSN);
	return ERROR_NONE;
}

int CDNBillingConnectionTW::OnPackageGift(CDNUserRepository *pUser)
{
	if (!m_bActive)
		return ERROR_GENERIC_INVALIDREQUEST;

	SendPurchaseItem(pUser->GetAccountName(), pUser->m_szIp, pUser->m_nTotalPrice, pUser->GetAccountDBID(), pUser->GetPurchaseOrderID(), pUser->GetPackageSN());
	return ERROR_NONE;
}

int CDNBillingConnectionTW::OnCoupon(CDNUserRepository *pUser)
{
	if (!m_bActive)
		return ERROR_GENERIC_INVALIDREQUEST;

	SendCouponUse(pUser->GetCoupon(), pUser->GetAccountName(), pUser->GetCharacterDBID(), pUser->GetWorldID(), pUser->m_szIp, pUser->GetAccountDBID());
	return ERROR_NONE;
}

static char * PSTRDEFAULT = { "-1" };
void CDNBillingConnectionTW::QueryPointMessage(char *pData, int nLen)
{	
	// 받은거 뿌린거 다 찍어달래요.
	g_Log.Log(LogType::_FILELOG, L"[GASHLOG:QueryPointMessage] %S\r\n", pData);
	{
		ScopeLock<CSyncLock> Lock(m_SendCountLock);
		--m_nSendCount;
	}
	int nResultIndex = 0;
	char* token = strtok(pData,";");
	int nResult = 0;
	char* strOutString = PSTRDEFAULT;
	UINT nAccountDBID = 0;
	while(token != NULL )
	{
		switch( nResultIndex )
		{
		case 0 : nResult = atoi(token); break;
		case 1 : strOutString = token; break;		
		case 2 : nAccountDBID = strtoul(token, NULL, 10); break;
		}
		token = strtok(NULL,";");
		++ nResultIndex;
	}

	CDNUserRepository *pUser = g_pManager->GetUser(nAccountDBID);
	if (!pUser){
		g_Log.Log(LogType::_ERROR, L"[GASH:QueryPointMessage] User NULL! (AccountDBID:%u) \r\n", nAccountDBID);
		return;
	}	

#if defined (PRE_ADD_REJECT_DUPLICATED_CASH_REQUEST)
	CRequestReleaser releaser(pUser->m_Requesting);
#endif // #if defined (PRE_ADD_REJECT_DUPLICATED_CASH_REQUEST)

	if( nResult == 1)	
		pUser->SendBalanceInquiry(ERROR_NONE, atoi(strOutString));
	else
	{
		//기타 나머지 에러처리..	
		int nConvertResult = ConvertResult(strOutString);
		pUser->SendBalanceInquiry(nConvertResult, 0);
		g_Log.Log(LogType::_ERROR, pUser, L"[GASH:QueryPointMessage] Point Query Fail! (AccountDBID:%u) %S\r\n", nAccountDBID, strOutString);
		if( nConvertResult == ERROR_GENERIC_UNKNOWNERROR )
		{
			//짤라 버리자..재연결 하게..
			DetachConnection(L"UnkownError");
		}		
	}	
}

void CDNBillingConnectionTW::ShopItemMessage(char *pData, int nLen)
{	
	// 받은거 뿌린거 다 찍어달래요.
	g_Log.Log(LogType::_FILELOG, L"[GASHLOG:ShopItemMessage] %S\r\n", pData);

	int nResultIndex = 0;
	char* token = strtok(pData,";");
	int nResult = 0;
	char* strOutString = NULL;
	UINT nAccountDBID = 0;
	char* strAccountName = NULL;
	while(token != NULL )
	{
		switch( nResultIndex )
		{
		case 0 : nResult = atoi(token); break;
		case 1 : strOutString = token; break;
		//case 2 : nAccountDBID = strtoul(token, NULL, 10); break;			
			// 구매관련은 AccountName으로 처리해야 할듯..PacketID를 PurchaseOrderID로 사용.
		case 3 : strAccountName = token; break;
		}
		token = strtok(NULL,";");
		++ nResultIndex;
	}	

	CDNUserRepository *pUser = g_pManager->GetUser(strAccountName);
	if (!pUser){
		g_Log.Log(LogType::_ERROR, L"[GASH:ShopItemMessage] User NULL! (AccountDBID:%s) \r\n", strAccountName);
		return;
	}

	if( g_Config.nProtocolType == 2 ) // New
		DelTimeOut(pUser->GetAccountDBID());

	if( nResult == 1)		
	{
		if( g_Config.nProtocolType == 2 && strcmp(strOutString, "Write_Off_Is_Successful") == 0) // New
		{
			g_Log.Log(LogType::_ERROR, pUser, L"[GASH:ShopItemMessage] WriteOff Success! (AccountDBID:%u)\r\n", nAccountDBID);
			pUser->OnRecvBillingBuyItem(ERROR_GENERIC_UNKNOWNERROR); //유저한테는 그냥 알려지지 않은 에러..		
		}		
		else		
			pUser->OnRecvBillingBuyItem(ERROR_NONE);
	}
	else
	{
		pUser->OnRecvBillingBuyItem(ConvertResult(strOutString));
		g_Log.Log(LogType::_ERROR, pUser, L"[GASH:ShopItemMessage] Item Purchase Fail! (AccountDBID:%u) %S \r\n", nAccountDBID, strOutString);
	}	
}

void CDNBillingConnectionTW::CouponMessage(char *pData, int nLen)
{	
	// 받은거 뿌린거 다 찍어달래요.
	g_Log.Log(LogType::_FILELOG, L"[GASHLOG:CouponMessage] %S\r\n", pData);

	char token[256] = {0, };
	size_t start = 0;
	const char *delims = ";";
	
	int nResultIndex = 0;
	int nResult = 0;
	char strOutString[20] = {0,};
	UINT nAccountDBID = 0;
	int nOutStringType = 0;
	while(pData[start] != '\0' )
	{
		size_t end = strcspn(pData + start, delims);
		sprintf_s(token, "%.*s", end, pData+start);
		start += (pData[start + end] != '\0') ? end + 1 : end;

		switch( nResultIndex )
		{
		case 0 : nResult = atoi(token); break;
		case 1 : nOutStringType = atoi(token); break;
		case 2 : strncpy(strOutString, token, 20); break;			
		case 4 : nAccountDBID = strtoul(token, NULL, 10); break;			
		}
		++ nResultIndex;
	}	

	CDNUserRepository *pUser = g_pManager->GetUser(nAccountDBID);
	if (!pUser){
		g_Log.Log(LogType::_ERROR, L"[GASH:CouponMessage] User NULL! (AccountDBID:%u) \r\n", nAccountDBID);
		return;
	}

#if defined (PRE_ADD_REJECT_DUPLICATED_CASH_REQUEST)
	CRequestReleaser releaser(pUser->m_Requesting);
#endif // #if defined (PRE_ADD_REJECT_DUPLICATED_CASH_REQUEST)

	if( nResult == 1)		
	{
		int nSerial = atoi(strOutString);		
		pUser->OnRecvBillingCoupon(ERROR_NONE, ERROR_NONE, nSerial, 0);
	}
	else
	{
		pUser->SendCoupon(ConvertCouponResult(nOutStringType), false);
		g_Log.Log(LogType::_ERROR, pUser, L"[GASH:CouponMessage] Coupon Fail! (AccountDBID:%u) %d %S \r\n", nAccountDBID, nOutStringType, strOutString);
	}	
}

void CDNBillingConnectionTW::CouponRollBackMessage(char *pData, int nLen)
{
	// 받은거 뿌린거 다 찍어달래요.
	g_Log.Log(LogType::_FILELOG, L"[GASHLOG:CouponRollBackMessage] %S\r\n", pData);

	int nResultIndex = 0;
	char* token = strtok(pData,";");
	int nResult = 0;
	char* strOutString = NULL;
	UINT nAccountDBID = 0;
	while(token != NULL )
	{
		switch( nResultIndex )
		{
		case 0 : nResult = atoi(token); break;
		case 1 : strOutString = token; break;		
		case 2 : nAccountDBID = strtoul(token, NULL, 10); break;
		}
		token = strtok(NULL,";");
		++ nResultIndex;
	}
	
	// 여기서 딱히 할껀없다..롤백이 안되면 어쩌자는겨..알아서 1:1문의 들어갈듯.그냥 에러나 찍어주자.
	if( nResult == 0)				
		g_Log.Log(LogType::_ERROR, L"[GASH:CouponRollBackMessage] RollBack Fail! (AccountDBID:%u) %S \r\n", nAccountDBID, strOutString);		
}

int CDNBillingConnectionTW::SendQueryUserPoint(CDNUserRepository *pUser)
{	
	if (!pUser)
	{
		g_Log.Log(LogType::_ERROR, L"[GASH:SendQueryUserPoint] User NULL!\r\n");
		return ERROR_GENERIC_INVALIDREQUEST;
	}			

	{
		ScopeLock<CSyncLock> Lock(m_SendCountLock);
		if( m_nSendCount > 30) // 요청이랑 30개 정도 차이나면 잘라 버리자..
		{
			//짤라 버리자..재연결 하게..
			m_nSendCount = 0;
			DetachConnection(L"Send Over Count");
			return ERROR_GENERIC_INVALIDREQUEST;
		}
		++m_nSendCount;
	}	
	char strMessage[256] = {0,};
	SNPRINTFA(strMessage, 256, "610401'AQ'%s'0'0'P'0'%s'%u'\r\n", pUser->GetAccountName(), pUser->m_szIp, pUser->GetAccountDBID());
	
	AddSendData(strMessage, static_cast<int>(::strlen(strMessage)));

	// 받은거 뿌린거 다 찍어달래요.	
	g_Log.Log(LogType::_FILELOG, pUser, L"[GASHLOG:SendQueryUserPoint] %S", strMessage);
	return ERROR_NONE;
}

int CDNBillingConnectionTW::SendPurchaseItem(char *pAccountName, char *pIp, int nPoint, UINT nAccountDBID, INT64 biPurchaseOrderID, int nItemSN)
{	
	CDNUserRepository *pUser = g_pManager->GetUser(nAccountDBID);
	if (!pUser)
	{
		g_Log.Log(LogType::_ERROR, L"[GASH:SendPurchaseItem] User NULL! (AccountDBID:%u) \r\n", nAccountDBID);
		return ERROR_GENERIC_UNKNOWNERROR;
	}			

	if( g_Config.nProtocolType == 1 ) // OLD
	{
		SendChargePoint(pAccountName, pIp, nPoint, biPurchaseOrderID, nItemSN);
	}
	else // New
	{ 
		if( !biPurchaseOrderID )
		{
			g_Log.Log(LogType::_ERROR, pUser, L"[GASH:SendPurchaseItem] biPurchaseOrderID Error! (AccountDBID:%u) %s\r\n", nAccountDBID);
			return ERROR_GENERIC_UNKNOWNERROR;
		}
		pUser->SetPurchaseOrderID(biPurchaseOrderID);
		SendAccountingWithTransactionID(pAccountName, pIp, nPoint, biPurchaseOrderID, nAccountDBID, nItemSN);		
	}
	return ERROR_NONE;
}

void CDNBillingConnectionTW::SendChargePoint(char *pAccountName, char *pIp, int nPoint, INT64 biPurchaseOrderID, int nItemSN)
{
	char strMessage[256] = {0,};
	CTimeSet aCurTime;

	SNPRINTFA(strMessage, 256, "610401'AQ'%s'P'%04d/%02d/%02d %02d:%02d:%02d'%d'%s'%d'%I64d'\r\n", pAccountName, 
		aCurTime.GetYear(), aCurTime.GetMonth(), aCurTime.GetDay(), aCurTime.GetHour(), aCurTime.GetMinute(), aCurTime.GetSecond(), nPoint, pIp, nItemSN, biPurchaseOrderID);

	AddSendData(strMessage, static_cast<int>(::strlen(strMessage)));

	// 받은거 뿌린거 다 찍어달래요.
	g_Log.Log(LogType::_FILELOG, L"[GASHLOG:SendChargePoint] %S", strMessage);
}

void CDNBillingConnectionTW::SendAccountingWithTransactionID(char *pAccountName, char *pIp, int nPoint, INT64 biPurchaseOrderID, UINT nAccountDBID, int nItemSN)
{		
	char strMessage[256] = {0,};

	char szTransactionID[GASH_TRANSACTION_LEN] = {0,};	
	CTimeSet aCurTime;
	// 일반트랜잭션 00
	SNPRINTFA(szTransactionID, GASH_TRANSACTION_LEN, "%04d%02d%02d00%I64d", aCurTime.GetYear(), aCurTime.GetMonth(), aCurTime.GetDay(), biPurchaseOrderID);
	
	SNPRINTFA(strMessage, 256, "610401'AQ'%s'PT'%04d/%02d/%02d %02d:%02d:%02d'%d'%s'%s'%d'%I64d'\r\n", pAccountName, 
		aCurTime.GetYear(), aCurTime.GetMonth(), aCurTime.GetDay(), aCurTime.GetHour(), aCurTime.GetMinute(), aCurTime.GetSecond(), nPoint, pIp, szTransactionID, nItemSN, biPurchaseOrderID);

	AddTimeOut(nAccountDBID, ::GetTickCount());

	AddSendData(strMessage, static_cast<int>(::strlen(strMessage)));

	// 받은거 뿌린거 다 찍어달래요.
	g_Log.Log(LogType::_FILELOG, L"[GASHLOG:SendAccountingWithTransactionID] %S", strMessage);
}

void CDNBillingConnectionTW::SendPointWriteOff(char* pAccountName, char *pIp, INT64 biPurchaseOrderID, UINT nAccountDBID)
{
	char strMessage[256] = {0,};
	
	char szTransactionID[GASH_TRANSACTION_LEN] = {0,};	
	char szTransactionOffID[GASH_TRANSACTION_LEN] = {0,};
	CTimeSet aCurTime;
	// 일반트랜잭션 00
	SNPRINTFA(szTransactionID, GASH_TRANSACTION_LEN, "%04d%02d%02d00%I64d", aCurTime.GetYear(), aCurTime.GetMonth(), aCurTime.GetDay(), biPurchaseOrderID);
	// 취소트랜잭션 01
	SNPRINTFA(szTransactionOffID, GASH_TRANSACTION_LEN, "%04d%02d%02d01%I64d", aCurTime.GetYear(), aCurTime.GetMonth(), aCurTime.GetDay(), biPurchaseOrderID);

	SNPRINTFA(strMessage, 256, "610401'AQ'%s'W'%s''%s'%04d/%02d/%02d %02d:%02d:%02d'%s'%I64d'%u'\r\n", pAccountName, szTransactionID, szTransactionOffID,
		aCurTime.GetYear(), aCurTime.GetMonth(), aCurTime.GetDay(), aCurTime.GetHour(), aCurTime.GetMinute(), aCurTime.GetSecond(), pIp, biPurchaseOrderID, nAccountDBID);

	AddSendData(strMessage, static_cast<int>(::strlen(strMessage)));

	// 받은거 뿌린거 다 찍어달래요.
	g_Log.Log(LogType::_FILELOG, L"[GASHLOG:SendPointWriteOff] %S", strMessage);
}

void CDNBillingConnectionTW::SendCouponUse(char* pCoupon, char* pAccountName, INT64 nCharacterDBID, char cWorldID, char* pIp, UINT nAccountDBID)
{
	CDNUserRepository *pUser = g_pManager->GetUser(nAccountDBID);
	if (!pUser)
	{
		g_Log.Log(LogType::_ERROR, L"[GASH:SendCouponUse] User NULL! (AccountDBID:%u) \r\n", nAccountDBID);
		return;
	}	

	char strMessage[256] = {0,};

	SNPRINTFA(strMessage, 256, "%s'I'0'%s'%I64d'%d'%s'%u'\r\n", pCoupon, pAccountName, nCharacterDBID, cWorldID, pIp, nAccountDBID);	

	AddSendData(strMessage, static_cast<int>(::strlen(strMessage)));

	// 받은거 뿌린거 다 찍어달래요.
	g_Log.Log(LogType::_FILELOG, pUser, L"[GASHLOG:SendCouponUse] %S", strMessage);
}

void CDNBillingConnectionTW::SendCouponFailResult(char* pCoupon, UINT nAccountDBID)
{
	char strMessage[256] = {0,};
	CTimeSet aCurTime;
	SNPRINTFA(strMessage, 256, "%s'R'%04d-%02d-%02d %02d:%02d:%02d'%u'\r\n", pCoupon, 
		aCurTime.GetYear(), aCurTime.GetMonth(), aCurTime.GetDay(), aCurTime.GetHour(), aCurTime.GetMinute(), aCurTime.GetSecond(), nAccountDBID);	

	AddSendData(strMessage, static_cast<int>(::strlen(strMessage)));

	// 받은거 뿌린거 다 찍어달래요.
	g_Log.Log(LogType::_FILELOG, L"[GASHLOG:SendCouponFailResult] %S", strMessage);
}

int CDNBillingConnectionTW::ConvertResult(char* strOutString)
{
	int nRet = ERROR_NONE;

	if( strcmp(strOutString, "Wrong_ID_or_Password") == 0 )
		nRet = ERROR_GAMANIA_INVALID_ID_OR_PASSWORD;
	else if( strcmp(strOutString, "No_Such_ServiceAccount") == 0 )
		nRet = ERROR_GAMANIA_NO_SUCH_ACCOUNT;
	else if( strcmp(strOutString, "No_Such_Service") == 0 )
		nRet = ERROR_GAMANIA_NO_SUCH_SERVICE;
	else if( strcmp(strOutString, "No_Enough_Point") == 0 )
		nRet = ERROR_GAMANIA_NO_ENOUGH_POINT;
	else if( strcmp(strOutString, "Wrong_EventOTP") == 0 )
		nRet = ERROR_GAMANIA_WRONG_EVENTOTP;
	else if( strcmp(strOutString, "Wrong_OTP") == 0 )
		nRet = ERROR_GAMANIA_WRONG_OTP;	
	else if( strcmp(strOutString, "ServiceAccount_Locked") == 0 )
		nRet = ERROR_GAMANIA_ACCOUNT_LOCK;
	else if( strcmp(strOutString, "Service_Not_Available") == 0 )
		nRet = ERROR_GAMANIA_SERVICE_NOT_AVAILABLE;	
	else if( strcmp(strOutString, "Time_Out") == 0 )
		nRet = ERROR_GAMANIA_TIME_OUT;	
	else if( strcmp(strOutString, "Contract_Block") == 0 )
		nRet = ERROR_GAMANIA_CONTRACT_BLOCK;
	else if( strcmp(strOutString, "ServiceAccount_Already_Login") == 0 )
		nRet = ERROR_GAMANIA_ALREADY_LOGIN;
	else if( strcmp(strOutString, "ServiceAccount_Already_Waiting") == 0 )
		nRet = ERROR_GAMANIA_ALREADY_WAITING;
	else
		nRet = ERROR_GENERIC_UNKNOWNERROR;
	return nRet;
}

int CDNBillingConnectionTW::ConvertCouponResult(int nOutStringType)
{
	int nRet = ERROR_NONE;

	switch(nOutStringType)
	{
	case 0 : nRet = ERROR_GAMANIA_COUPON_AVAILABE; break;
	case 1 : nRet = ERROR_GAMANIA_COUPON_USED; break;
	case 2 : nRet = ERROR_GAMANIA_COUPON_EXCHANGING; break;
	case 3 : nRet = ERROR_GAMANIA_COUPON_UNAVAILABE; break;
	case 4 : nRet = ERROR_GAMANIA_COUPON_USED_DATE; break;
	case 5 : nRet = ERROR_GAMANIA_COUPON_NOT_START_USE; break;
	case 6 : nRet = ERROR_GAMANIA_COUPON_EXPIRED; break;
	case 7 : nRet = ERROR_GAMANIA_COUPON_WRONG; break;
	case 12 : nRet = ERROR_GAMANIA_COUPON_WRONG_WORLD; break;
	case 10 :
	case 11 :
	case 13 :
	case 14 :
		nRet = ERROR_GAMANIA_COUPON_REINPUT; break;
	case 15 : nRet = ERROR_GAMANIA_COUPON_ACCOUNT; break;
	case 16 : nRet = ERROR_GAMANIA_COUPON_WORD; break;
	case 17 : nRet = ERROR_GAMANIA_COUPON_COUNT; break;
	default : nRet = ERROR_GENERIC_UNKNOWNERROR;
	}	
	return nRet;
}

#endif