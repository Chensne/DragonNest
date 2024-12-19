#include "StdAfx.h"
#include "DNBillingConnectionJP.h"
#include "DNConnection.h"
#include "DNUserRepository.h"
#include "DNExtManager.h"
#include "DNSQLMembership.h"
#include "DNSQLWorld.h"
#include "Log.h"

extern TCashConfig g_Config;

#if defined(_JP)

CDNBillingConnectionJP *g_pBillingConnection = NULL;

CDNBillingConnectionJP::CDNBillingConnectionJP(void)
{
	int nRet = -1;
	std::wstring wStr = g_Config.wszServiceType;
	ToLowerW(wStr);
	nRet = HanBillingInit("J_DNEST");
	if (nRet == HAN_BILLING_OK){
		g_Log.Log(LogType::_FILEDBLOG, L"HanBillingInit OK!!\r\n");
	}
	else{
		g_Log.Log(LogType::_FILEDBLOG, L"HanBillingInit Fail (Error:%d, %d)!!\r\n", nRet, ConvertResult(nRet));
	}
}

CDNBillingConnectionJP::~CDNBillingConnectionJP(void)
{
}

int CDNBillingConnectionJP::OnBalanceInquiry(CDNUserRepository *pUser)
{
	int nCashBalance = 0;
	int nRet = SendBillingBalanceInquire(pUser->GetAccountName(), nCashBalance);
	if (nRet != ERROR_NONE){
		g_Log.Log(LogType::_ERROR, pUser, L"[OnBalanceInquiry] Ret:%d\r\n", nRet);
		return ERROR_GENERIC_INVALIDREQUEST;	// �����̽������� ������ ��ü;
	}

	pUser->SendBalanceInquiry(nRet, nCashBalance);
	return ERROR_NONE;
}

int CDNBillingConnectionJP::OnBuy(CDNUserRepository *pUser)
{
	CDNSQLMembership *pMembershipDB = pUser->GetMembershipDB();
	if (!pMembershipDB) return ERROR_GENERIC_INVALIDREQUEST;

	CDNSQLWorld *pWorldDB = pUser->GetWorldDB();
	if (!pWorldDB) return ERROR_GENERIC_INVALIDREQUEST;

	int nRet = SendBillingBuy(pUser, pMembershipDB, pWorldDB);
	if (nRet != ERROR_NONE){
		g_Log.Log(LogType::_ERROR, pUser, L"[OnBuy] Ret:%d\r\n", nRet);
		nRet = ERROR_GENERIC_INVALIDREQUEST;	// �����̽������� ������ ��ü;
	}

	pUser->SendBuyGift(nRet);
	return ERROR_NONE;
}

int CDNBillingConnectionJP::OnPackageBuy(CDNUserRepository *pUser)
{
	CDNSQLMembership *pMembershipDB = pUser->GetMembershipDB();
	if (!pMembershipDB) return ERROR_GENERIC_INVALIDREQUEST;

	CDNSQLWorld *pWorldDB = pUser->GetWorldDB();
	if (!pWorldDB) return ERROR_GENERIC_INVALIDREQUEST;

	int nRet = SendBillingPackageBuy(pUser, pMembershipDB, pWorldDB);
	if (nRet != ERROR_NONE){
		g_Log.Log(LogType::_ERROR, pUser, L"[OnPackageBuy] Ret:%d\r\n", nRet);
		nRet = ERROR_GENERIC_INVALIDREQUEST;	// �����̽������� ������ ��ü;
	}
	pUser->SendBuyGift(nRet);
	return ERROR_NONE;
}

int CDNBillingConnectionJP::OnGift(CDNUserRepository *pUser)
{
	CDNSQLMembership *pMembershipDB = pUser->GetMembershipDB();
	if (!pMembershipDB) return ERROR_GENERIC_INVALIDREQUEST;

	CDNSQLWorld *pWorldDB = pUser->GetWorldDB();
	if (!pWorldDB) return ERROR_GENERIC_INVALIDREQUEST;

	int nRet = SendBillingGift(pUser, pMembershipDB, pWorldDB);
	if (nRet != ERROR_NONE){
		g_Log.Log(LogType::_ERROR, pUser, L"[OnGift] Ret:%d\r\n", nRet);
		nRet = ERROR_GENERIC_INVALIDREQUEST;	// �����̽������� ������ ��ü;
	}
	pUser->SendBuyGift(nRet);
	return ERROR_NONE;
}

int CDNBillingConnectionJP::OnPackageGift(CDNUserRepository *pUser)
{
	CDNSQLMembership *pMembershipDB = pUser->GetMembershipDB();
	if (!pMembershipDB) return ERROR_GENERIC_INVALIDREQUEST;

	CDNSQLWorld *pWorldDB = pUser->GetWorldDB();
	if (!pWorldDB) return ERROR_GENERIC_INVALIDREQUEST;

	int nRet = SendBillingPackageGift(pUser, pMembershipDB, pWorldDB);
	if (nRet != ERROR_NONE){
		g_Log.Log(LogType::_ERROR, pUser, L"[OnPackageGift] Ret:%d\r\n", nRet);
		nRet = ERROR_GENERIC_INVALIDREQUEST;	// �����̽������� ������ ��ü;
	}
	pUser->SendBuyGift(nRet);
	return ERROR_NONE;
}

int CDNBillingConnectionJP::OnCoupon(CDNUserRepository *pUser)
{
	return ERROR_GENERIC_INVALIDREQUEST;
}

int CDNBillingConnectionJP::ConvertResult(int nBillingResult)
{
	switch (nBillingResult)
	{
	case HAN_BILLING_OK: return ERROR_NONE;	// return ERROR_HAN_BILLING_OK;

	// +) fault from Billing Server
	case HAN_BILLING_GAMECODE_NOTEXIST: return ERROR_HAN_BILLING_GAMECODE_NOTEXIST;				// GAMEID �� �����ϴ�.
	case HAN_BILLING_USERNO_NOTEXIST: return ERROR_HAN_BILLING_USERNO_NOTEXIST;					// User Number �� �����ϴ�.
	case HAN_BILLING_CMD_NOTEXIST: return ERROR_HAN_BILLING_CMD_NOTEXIST;						// ���� Ŀ�ǵ尡 �����ϴ�.
	case HAN_BILLING_ORDNO_NOTEXIST: return ERROR_HAN_BILLING_ORDNO_NOTEXIST;					// �ֹ���ȣ�� �����ϴ�.
	case HAN_BILLING_SCD_NOTEXIST: return ERROR_HAN_BILLING_SCD_NOTEXIST;						// ��ǰ�ڵ尡 �����ϴ�.
	case HAN_BILLING_SNAME_NOTEXIST: return ERROR_HAN_BILLING_SNAME_NOTEXIST;					// ��ǰ �̸��� �����ϴ�.
	case HAN_BILLING_QTY_NOTEXIST: return ERROR_HAN_BILLING_QTY_NOTEXIST;						// ��ǰ ������ �����ϴ�.
	case HAN_BILLING_AMT_NOTEXIST: return ERROR_HAN_BILLING_AMT_NOTEXIST;						// ���Ű����� �����ϴ�.
	case HAN_BILLING_IP_NOTEXIST: return ERROR_HAN_BILLING_IP_NOTEXIST;							// Ŭ���̾�Ʈ IP �ּҰ� �����ϴ�.
	case HAN_BILLING_USERNO_INVALID: return ERROR_HAN_BILLING_USERNO_INVALID;					// User Number�� �ùٸ��� �ʽ��ϴ�.
	case HAN_BILLING_BUY_FAIL: return ERROR_HAN_BILLING_BUY_FAIL;								// �� �ֹ���ȣ�δ� ���� ���� �ʾҽ��ϴ�.
	case HAN_BILLING_GAMECODE_INVALID: return ERROR_HAN_BILLING_GAMECODE_INVALID;				// ���Ӿ��̵� �ùٸ��� �ʽ��ϴ�.
	case HAN_BILLING_BALANCE_INSUFFICIENT: return ERROR_HAN_BILLING_BALANCE_INSUFFICIENT;		// �ܾ��� �����մϴ�.
	case HAN_BILLING_CMD_UNEXPECTED: return ERROR_HAN_BILLING_CMD_UNEXPECTED;					// ����ġ ���� Ŀ�ǵ带 �����߽��ϴ�.
	case HAN_BILLING_DB_CONNECTFAIL: return ERROR_HAN_BILLING_DB_CONNECTFAIL;					// ���� DB ���ӿ� �����߽��ϴ�.
	case HAN_BILLING_SVR_UNEXPECTED: return ERROR_HAN_BILLING_SVR_UNEXPECTED;					// �����ý��� ����ġ ���� ������ �߻��߽��ϴ�.
	// -) fault from function / this system
	case HAN_BILLING_ARGUMENT_INVALID: return ERROR_HAN_BILLING_ARGUMENT_INVALID;				// ���޵� ���ڰ� �ùٸ��� �ʽ��ϴ�.
	case HAN_BILLING_INITED_ALREADY: return ERROR_HAN_BILLING_INITED_ALREADY;					// ����� �̹� �ʱ�ȭ �Ǿ����ϴ�.
	case HAN_BILLING_INITED_NOT: return ERROR_HAN_BILLING_INITED_NOT;							// ����� �ʱ�ȭ ���� �ʾҽ��ϴ�.
	case HAN_BILLING_INITED_FAIL: return ERROR_HAN_BILLING_INITED_FAIL;							// ��� �ʱ�ȭ�� �����߽��ϴ�.
	case HAN_BILLING_INITED_FAILBALANCE: return ERROR_HAN_BILLING_INITED_FAILBALANCE;			// �ܾ���ȸĿ�ؼ� �ʱ�ȭ�� �����߽��ϴ�.
	case HAN_BILLING_INITED_FAILBUY: return ERROR_HAN_BILLING_INITED_FAILBUY;					// ����Ŀ�ؼ� �ʱ�ȭ�� �����߽��ϴ�.
	case HAN_BILLING_INITED_FAILCONFIRM: return ERROR_HAN_BILLING_INITED_FAILCONFIRM;			// ����Ȯ��Ŀ�ؼ� �ʱ�ȭ�� �����߽��ϴ�.
	case HAN_BILLING_GETCONN_FAILBALANCE: return ERROR_HAN_BILLING_GETCONN_FAILBALANCE;			// �ܾ���ȸ Ŀ�ؼ� ��⿡ �����߽��ϴ�.
	case HAN_BILLING_GETCONN_FAILBUY: return ERROR_HAN_BILLING_GETCONN_FAILBUY;					// ���� Ŀ�ؼ� ��⿡ �����߽��ϴ�.
	case HAN_BILLING_GETCONN_FAILCONFIRM: return ERROR_HAN_BILLING_GETCONN_FAILCONFIRM;			// ����Ȯ�� Ŀ�ؼ� ��⿡ �����߽��ϴ�.
	case HAN_BILLING_OPENREQ_FAIL: return ERROR_HAN_BILLING_OPENREQ_FAIL;						// Request ���¿� �����߽��ϴ�.
	case HAN_BILLING_SENDREQ_FAIL: return ERROR_HAN_BILLING_SENDREQ_FAIL;						// ���� ���� Send �� �����߽��ϴ�.
	case HAN_BILLING_RECVRESULT_FAIL: return ERROR_HAN_BILLING_RECVRESULT_FAIL;					// �������� Recv �� �����߽��ϴ�.
	case HAN_BILLING_RECVRESULT_INVALID: return ERROR_HAN_BILLING_RECVRESULT_INVALID;			// �����κ��� ���޵� ���������� �ùٸ��� �ʽ��ϴ�.
	case HAN_BILLING_INITED_FAILBILLNO: return ERROR_HAN_BILLING_INITED_FAILBILLNO;
	case HAN_BILLING_GETCONN_FAILBILLNO: return ERROR_HAN_BILLING_GETCONN_FAILBILLNO;
	case HAN_BILLING_INITED_FAILBILLNOACC: return ERROR_HAN_BILLING_INITED_FAILBILLNOACC;
	case HAN_BILLING_GETCONN_FAILBILLNOACC: return ERROR_HAN_BILLING_GETCONN_FAILBILLNOACC;
	case HAN_BILLING_DLL_UNEXPECTED: return ERROR_HAN_BILLING_DLL_UNEXPECTED;					// ��� ������ ����ġ ���� ������ �߻��߽��ϴ�.
	}

	return ERROR_DB;
}

int CDNBillingConnectionJP::ConvertGetBillNoResult(int nBillingResult)
{
	switch (nBillingResult)
	{
	// 4.2	HanBillingGetBillNoAcc() ���� Return Value
	// +) fault from TGS
	case HAN_BILLING_GETBILLNO_NEED_PARAM: return ERROR_HAN_BILLING_GETBILLNO_NEED_PARAM;			// �ʼ� �Ķ���Ͱ� �����Ǿ����ϴ�.
	case HAN_BILLING_GETBILLNO_IP_MISMATCH: return ERROR_HAN_BILLING_GETBILLNO_IP_MISMATCH;			// IP�� Ʋ���ϴ�.
	case HAN_BILLING_GETBILLNO_USERID_MISMATCH: return ERROR_HAN_BILLING_GETBILLNO_USERID_MISMATCH;	// ����� ID�� Ʋ���ϴ�.
	case HAN_BILLING_GETBILLNO_DB_CONN_ERROR: return ERROR_HAN_BILLING_GETBILLNO_DB_CONN_ERROR;		// DB ���ῡ �����߽��ϴ�.
	case HAN_BILLING_GETBILLNO_USER_NOTEXIST: return ERROR_HAN_BILLING_GETBILLNO_USER_NOTEXIST;		// ��ϵ��� ���� ������Դϴ�.
	case HAN_BILLING_GETBILLNO_COOKIE_ERROR: return ERROR_HAN_BILLING_GETBILLNO_COOKIE_ERROR;		// cookie ����
	case HAN_BILLING_GETBILLNO_COOKIE_IS_EMPTY: return ERROR_HAN_BILLING_GETBILLNO_COOKIE_IS_EMPTY;	// cookie�� ������ϴ�.
	case HAN_BILLING_GETBILLNO_COOKIE_TIMEOUT: return ERROR_HAN_BILLING_GETBILLNO_COOKIE_TIMEOUT;	// ����� cookie�Դϴ�.
	}

	return ERROR_DB;
}

int CDNBillingConnectionJP::SendBillingBalanceInquire(char *pAccountName, int &nCashBalance)
{
	char szBillingNo[MAX_BILLNO_LEN] = { 0, }, szRetMsg[128] = { 0, };
	int nBillingRet = HanBillingGetBillNoAcc(pAccountName, szBillingNo, szRetMsg, 128);

	if (nBillingRet != HAN_BILLING_OK)
		return ConvertGetBillNoResult(nBillingRet);

	HANBILL_BALANCEINFO BalanceInfo = { 0, };
		nBillingRet = HanBillingBalanceInquire(pAccountName, szBillingNo, szRetMsg, 128, &BalanceInfo);
		if (nBillingRet != HAN_BILLING_OK) 
		g_Log.Log(LogType::_ERROR, L"[CASH_BALANCEINQUIRY] AccountName:%S Ret:%S nBillingRet:%d\r\n", pAccountName, szRetMsg, nBillingRet);

	nCashBalance = BalanceInfo.nTotalBalance;

	return ConvertResult(nBillingRet);
}

int CDNBillingConnectionJP::SendBillingBuy(CDNUserRepository *pUser, CDNSQLMembership *pMembershipDB, CDNSQLWorld *pWorldDB)
{
	char szBillingNo[MAX_BILLNO_LEN] = { 0, }, szRetMsg[128] = { 0, };
	int nBillingRet = HanBillingGetBillNoAcc(pUser->GetAccountName(), szBillingNo, szRetMsg, 128);
	if (nBillingRet != HAN_BILLING_OK)
		return ConvertGetBillNoResult(nBillingRet);

	HANBILL_BUYINFO BuyInfo = {0, };
	char szOrderNo[MAX_CODELEN] = {0, };

	int nRet = ERROR_DB;

	TCashCommodityData CashData;
	for (int i = 0; i < pUser->m_cProductCount; i++){	// ��ü�� ó���������� �Ѱ��� ó���Ѵ�
		CDNUserRepository::TBuyItem *pBuyItem = pUser->GetBuyItem(i);
		if (!pBuyItem)
			return ERROR_ITEM_NOTFOUND;

		memset(&CashData, 0, sizeof(TCashCommodityData));
		bool bRet = g_pExtManager->GetCashCommodityData(pBuyItem->BuyItem.nItemSN, CashData);
		if (!bRet)
			return ERROR_ITEM_NOTFOUND;

		nRet = pUser->AddPurchaseOrderByCash(i);
		if (nRet != ERROR_NONE)
			return nRet;

		sprintf_s(szOrderNo, "%020I64d", pUser->GetPurchaseOrderID());
		memset(&BuyInfo, 0, sizeof(HANBILL_BUYINFO));
		nBillingRet = HanBillingBuy(pUser->GetAccountName(), szBillingNo, szRetMsg, 128, CashData.strJPSN.c_str(), CashData.strName.c_str(), 1, CashData.nPrice, pUser->m_szIp, szOrderNo, &BuyInfo);

		char cOrderCode = DBDNMembership::OrderStatusCode::FailPayment;
		if (nBillingRet == HAN_BILLING_OK){
			cOrderCode = DBDNMembership::OrderStatusCode::Success;
			pBuyItem->BuyItem.bFail = false;
		}
		else{	// �����ϸ�fail
			pBuyItem->BuyItem.bFail = true;
			g_Log.Log(LogType::_ERROR, pUser, L"[CASH_BUY] AccountName:%S Ret:%S\r\n", pUser->GetAccountName(), szRetMsg);
		}

		nRet = pMembershipDB->QuerySetPurchaseOrderResult(pUser->GetPurchaseOrderID(), cOrderCode, BuyInfo.szPaymentCode, nBillingRet, CashData.nReserve, pUser->m_nUsedPaidCash);
		if (nRet != ERROR_NONE){
			g_Log.Log(LogType::_ERROR, pUser, L"[CASH_BUY] QuerySetPurchaseOrderResult Fail(%d) Order(Eye:%I64d NHN:%S)\r\n", nBillingRet, pUser->GetPurchaseOrderID(), BuyInfo.szPaymentCode);

			return nRet;
		}

		if (nBillingRet == HAN_BILLING_OK){
			nRet = pUser->CashItemDBProcess(pUser->GetWorldID(), pUser->GetAccountDBID(), pUser->GetCharacterDBID(), pUser->GetMapID(), pUser->GetChannelID(), pUser->GetPaymentRules(),
				pBuyItem->BuyItem, CashData.nPrice, pUser->GetPurchaseOrderID(), DBDNWorldDef::AddMaterializedItem::CashBuy);

			if (nRet != ERROR_NONE){
				pMembershipDB->QueryModGiveFailFlag(pUser->GetPurchaseOrderID(), pBuyItem->BuyItem.nItemSN, pBuyItem->BuyItem.CashItem.nItemID, true);
			}
		}
		else
			nRet = ConvertResult(nBillingRet);
	}

	return nRet;
}

int CDNBillingConnectionJP::SendBillingPackageBuy(CDNUserRepository *pUser, CDNSQLMembership *pMembershipDB, CDNSQLWorld *pWorldDB)
{
	char szBillingNo[MAX_BILLNO_LEN] = { 0, }, szRetMsg[128] = { 0, };
	int nBillingRet = HanBillingGetBillNoAcc(pUser->GetAccountName(), szBillingNo, szRetMsg, 128);
	if (nBillingRet != HAN_BILLING_OK)
		return ConvertGetBillNoResult(nBillingRet);

	HANBILL_BUYINFO BuyInfo = {0, };
	char szOrderNo[MAX_CODELEN] = {0, };

	sprintf_s(szOrderNo, "%020I64d", pUser->GetPurchaseOrderID());
	memset(&BuyInfo, 0, sizeof(HANBILL_BUYINFO));

	std::string strJPSN, strJPName;
	bool bCashRet = g_pExtManager->GetCashCommodityJPSN(pUser->GetPackageSN(), strJPSN);
	bool bNameRet = g_pExtManager->GetCashCommodityName(pUser->GetPackageSN(), strJPName);
	nBillingRet = HanBillingBuy(pUser->GetAccountName(), szBillingNo, szRetMsg, 128, (bCashRet) ? strJPSN.c_str() : NULL, (bCashRet) ? strJPName.c_str() : NULL, 1, pUser->m_nTotalPrice, pUser->m_szIp, szOrderNo, &BuyInfo);

	char cOrderCode = DBDNMembership::OrderStatusCode::FailPayment;
	if (nBillingRet == HAN_BILLING_OK) cOrderCode = DBDNMembership::OrderStatusCode::Success;
	else
		g_Log.Log(LogType::_ERROR, pUser, L"[CASH_PACKAGEBUY] AccountName:%S Ret:%S\r\n", pUser->GetAccountName(), szRetMsg);

	int nRet = pMembershipDB->QuerySetPurchaseOrderResult(pUser->GetPurchaseOrderID(), cOrderCode, BuyInfo.szPaymentCode, nBillingRet, g_pExtManager->GetCashCommodityReserve(pUser->GetPackageSN()), pUser->m_nUsedPaidCash);
	if (nRet != ERROR_NONE){
		g_Log.Log(LogType::_ERROR, pUser, L"[CASH_PACKAGEBUY] QuerySetPurchaseOrderResult Fail(%d) Order(Eye:%I64d NHN:%S)\r\n", nBillingRet, pUser->GetPurchaseOrderID(), BuyInfo.szPaymentCode);

		return nRet;
	}

	nRet = ConvertResult(nBillingRet);
	if (nRet == ERROR_NONE)
		pUser->CalcPackageProcess();

	return nRet;
}

int CDNBillingConnectionJP::SendBillingGift(CDNUserRepository *pUser, CDNSQLMembership *pMembershipDB, CDNSQLWorld *pWorldDB)
{
	char szBillingNo[MAX_BILLNO_LEN] = { 0, }, szRetMsg[128] = { 0, };
	int nBillingRet = HanBillingGetBillNoAcc(pUser->GetAccountName(), szBillingNo, szRetMsg, 128);
	if (nBillingRet != HAN_BILLING_OK)
		return ConvertGetBillNoResult(nBillingRet);

	HANBILL_BUYINFO GiftInfo = {0, };
	char szOrderNo[MAX_CODELEN] = {0, };

	TCashCommodityData CashData;
	for (int i = 0; i < pUser->m_cProductCount; i++){	// ��ü�� ó���������� �Ѱ��� ó���Ѵ�
		CDNUserRepository::TGiftItem *pGiftItem = pUser->GetGiftItem(i);
		if (!pGiftItem)
			return ERROR_ITEM_NOTFOUND;

		memset(&CashData, 0, sizeof(TCashCommodityData));
		bool bRet = g_pExtManager->GetCashCommodityData(pGiftItem->GiftItem.nItemSN, CashData);
		if (!bRet)
			return ERROR_ITEM_NOTFOUND;

		int nRet = pUser->AddPurchaseOrderByCash(i);
		if (nRet != ERROR_NONE)
			return nRet;

		sprintf_s(szOrderNo, "%020I64d", pUser->GetPurchaseOrderID());
		memset(&GiftInfo, 0, sizeof(HANBILL_BUYINFO));
		nBillingRet = HanBillingBuy(pUser->GetAccountName(), szBillingNo, szRetMsg, 128, CashData.strJPSN.c_str(), CashData.strName.c_str(), 1, CashData.nPrice, pUser->m_szIp, szOrderNo, &GiftInfo);

		char cOrderCode = DBDNMembership::OrderStatusCode::FailPayment;
		if (nBillingRet == HAN_BILLING_OK){
			cOrderCode = DBDNMembership::OrderStatusCode::Success;
			pGiftItem->GiftItem.bFail = false;
		}
		else{	// �����ϸ�fail
			pGiftItem->GiftItem.bFail = true;
			g_Log.Log(LogType::_ERROR, pUser, L"[CASH_GIFT] AccountName:%S Ret:%S\r\n", pUser->GetAccountName(), szRetMsg);
		}

		nRet = pMembershipDB->QuerySetPurchaseOrderResult(pUser->GetPurchaseOrderID(), cOrderCode, GiftInfo.szPaymentCode, nBillingRet, CashData.nReserve, pUser->m_nUsedPaidCash);
		if (nRet != ERROR_NONE){
			g_Log.Log(LogType::_ERROR, pUser, L"[CASH_GIFT] QuerySetPurchaseOrderResult Fail(%d) Order(Eye:%I64d NHN:%S)\r\n", nBillingRet, pUser->GetPurchaseOrderID(), GiftInfo.szPaymentCode);

			return nRet;
		}
	}

	return ConvertResult(nBillingRet);
}

int CDNBillingConnectionJP::SendBillingPackageGift(CDNUserRepository *pUser, CDNSQLMembership *pMembershipDB, CDNSQLWorld *pWorldDB)
{
	char szBillingNo[MAX_BILLNO_LEN] = { 0, }, szRetMsg[128] = { 0, };
	int nBillingRet = HanBillingGetBillNoAcc(pUser->GetAccountName(), szBillingNo, szRetMsg, 128);
	if (nBillingRet != HAN_BILLING_OK)
		return ConvertGetBillNoResult(nBillingRet);

	HANBILL_BUYINFO GiftInfo = {0, };
	char szOrderNo[MAX_CODELEN] = {0, };

	sprintf_s(szOrderNo, "%020I64d", pUser->GetPurchaseOrderID());
	memset(&GiftInfo, 0, sizeof(HANBILL_BUYINFO));

	std::string strJPSN, strJPName;
	bool bCashRet = g_pExtManager->GetCashCommodityJPSN(pUser->GetPackageSN(), strJPSN);
	bool bNameRet = g_pExtManager->GetCashCommodityName(pUser->GetPackageSN(), strJPName);
	nBillingRet = HanBillingBuy(pUser->GetAccountName(), szBillingNo, szRetMsg, 128, (bCashRet) ? strJPSN.c_str() : NULL, (bNameRet) ? strJPName.c_str() : NULL, 1, pUser->m_nTotalPrice, pUser->m_szIp, szOrderNo, &GiftInfo);

	char cOrderCode = DBDNMembership::OrderStatusCode::FailPayment;
	if (nBillingRet == HAN_BILLING_OK) cOrderCode = DBDNMembership::OrderStatusCode::Success;
	else
		g_Log.Log(LogType::_ERROR, pUser, L"[CASH_PACKAGEGIFT] AccountName:%S Ret:%S\r\n", pUser->GetAccountName(), szRetMsg);

	int nRet = pMembershipDB->QuerySetPurchaseOrderResult(pUser->GetPurchaseOrderID(), cOrderCode, GiftInfo.szPaymentCode, nBillingRet, g_pExtManager->GetCashCommodityReserve(pUser->GetPackageSN()), pUser->m_nUsedPaidCash);
	if (nRet != ERROR_NONE){
		g_Log.Log(LogType::_ERROR, pUser, L"[CASH_PACKAGEGIFT] QuerySetPurchaseOrderResult Fail(%d) Order(Eye:%I64d NHN:%S)\r\n", nBillingRet, pUser->GetPurchaseOrderID(), GiftInfo.szPaymentCode);

		return nRet;
	}

	return ConvertResult(nBillingRet);
}

#endif	// #if defined(_JP)