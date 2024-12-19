#include "StdAfx.h"
#include "DNBillingConnectionKRAZ.h"
#include "../ServerCommon/KRAZ/OzBillingSDK.h"
#include "../ServerCommon/KRAZ/StrToActozStr.h"
#include "DNUserRepository.h"
#include "DNManager.h"
#include "DNExtManager.h"

#if defined(_KRAZ)

CDNBillingConnectionKRAZ *g_pBillingConnection = NULL;
bool g_bSDKBilling = false;

void __stdcall OnResError(const __int64 _i64SeqNum, const  int _iErrorCode)
{
	g_Log.Log(LogType::_FILEDBLOG, L"[ActozBilling] OnResError SeqNum:%I64d ErrorCode:%d\r\n", _i64SeqNum, _iErrorCode);
}

void __stdcall OnServerAuth(const bool _bIsOK, const int _iErrorCode)
{
	if (_bIsOK){
		g_bSDKBilling = true;
		g_Log.Log(LogType::_FILELOG, L"[ActozBilling] OnServerAuth OK!! ErrorCode:%d\r\n", _iErrorCode);
	}
	else{
		g_Log.Log(LogType::_FILELOG, L"[ActozBilling] OnServerAuth Fail!! ErrorCode:%d\r\n", _iErrorCode);
	}
}

void __stdcall OnUserBalance(const __int64 _i64SeqNum, LPCWSTR _szPlayerID, const unsigned int _uiReMainCash, const unsigned int _uiReMainMile, const char _cResultValue, LPCWSTR _szReservedFields, int _iCSite)
{
	UINT nAccountDBID = (UINT)_i64SeqNum;
	CDNUserRepository *pUser = g_pManager->GetUser(nAccountDBID);
	if (!pUser){
		g_Log.Log(LogType::_ERROR, 0, nAccountDBID, 0, 0, L"[ActozBilling] [AID:%u] pUser NULL Result:%d \r\n", nAccountDBID);
		return;
	}

	pUser->SendBalanceInquiry(ERROR_NONE, _uiReMainCash);
}

void __stdcall OnUseMoney(const __int64 _i64SeqNum, LPCWSTR _szPlayerID, const unsigned int _uiReMainCash, const unsigned int _uiReMainMile, const char _cResultValue, LPCWSTR _szReservedFields, int _iCSite)
{
	UINT nAccountDBID = (UINT)_i64SeqNum;
	CDNUserRepository *pUser = g_pManager->GetUser(nAccountDBID);
	if (!pUser){
		g_Log.Log(LogType::_ERROR, 0, nAccountDBID, 0, 0, L"[ActozBilling] [AID:%u] pUser NULL Result:%d \r\n", nAccountDBID);
		return;
	}

	pUser->SetUsedPaidCash(_szReservedFields);
	pUser->OnRecvBillingBuyItem(CDNBillingConnectionKRAZ::ConvertResult(_cResultValue));
}

void __stdcall OnUseCoupon(const __int64 _i64SeqNum, LPCWSTR _szPlayerID, const char _cItemType, LPCWSTR _szItemDesc, const char _cResultValue, LPCWSTR _szReservedFields, int _iCSite)
{

}

void __stdcall OnSetMile(const __int64 _i64SeqNum, LPCWSTR _szPlayerID, const unsigned int _uiReMainMile, const char _cResultValue, LPCWSTR _szReservedFields, int _iCSite)
{

}

CDNBillingConnectionKRAZ::CDNBillingConnectionKRAZ(void)
{
}

CDNBillingConnectionKRAZ::~CDNBillingConnectionKRAZ(void)
{
}

bool CDNBillingConnectionKRAZ::Init()
{
	HMODULE hDll = NULL;
	hDll = LoadLibraryW(L"OzBillingSDK_x64.dll");
	if (!hDll){
		g_Log.Log(LogType::_FILELOG, L"CDNBillingConnectionKRAZ OzBillingSDK_x64 Dll Load Fail!!\r\n");
		return false;
	}

	Init_SDK = (fnInit_SDK)GetProcAddress(hDll, "Init_SDK");
	SDKConnect = (fnSDKConnect)GetProcAddress(hDll, "SDKConnect");//OSP에서 Call할 함수구조체와 접속할 IP와 Port를 Parameter로 받는다..
	OSP_CONNECTION_CHECK = (fnOSP_CONNECTION_CHECK)GetProcAddress(hDll, "OSP_CONNECTION_CHECK");
	UserBalance = (fnUserBalance)GetProcAddress(hDll, "UserBalance");//잔액조회
	UseMoney = (fnUseMoney)GetProcAddress(hDll, "UseMoney");//아이템구매
	UseCoupon = (fnUseCoupon)GetProcAddress(hDll, "UseCoupon");//쿠폰사용
	SetMile = (fnSetMile)GetProcAddress(hDll, "SetMile");//마일리지 적립

	funcID.OnResError = &OnResError;		//Requeset Packet에대한 Error값(Error일경우만)
	funcID.OnServerAuth = &OnServerAuth;	//게임서버의 서비스번호및 프로토콜 버전등록에 대한 Response값
	funcID.OnUserBalance = &OnUserBalance;	// 잔액조회 결과값
	funcID.OnUseMoney = &OnUseMoney;		// 아이템 구매 결과값
	funcID.OnUseCoupon = &OnUseCoupon;		// 쿠폰 사용 결과값
	funcID.OnSetMile = &OnSetMile;		// 마일리지 적립 결과값

#define OzBillingSDK_CODE "50ABFEE347C00A5538B980AE1FA55081"

	char szMD5[32+1] ={0,};
	if (Init_SDK(szMD5) == -1){
		g_Log.Log(LogType::_FILELOG, L"ActozBilling Init_SDK Fail!!\r\n");
		return false;
	}
	else{
		g_Log.Log(LogType::_FILELOG, L"ActozBilling Init_SDK OK!!\r\n");
	}

	if (strcmp(OzBillingSDK_CODE, szMD5) != 0) 
	{
		g_Log.Log(LogType::_FILELOG, L"ActozBilling OzBillingSDK_CODE Fail!!\r\n");
		return false;
	}
	else{
		g_Log.Log(LogType::_FILELOG, L"ActozBilling OzBillingSDK_CODE OK!!\r\n");
	}

	int nCon = SDKConnect(funcID, true);	//함수를 등록하고 DebugMode(확인로그를 남긴다..)값을 넘겨줘 OSP에 접속한다
	if (nCon < 0){
		g_Log.Log(LogType::_FILELOG, L"ActozBilling ConSock Fail!!\r\n");
		return false;
	}
	else{
		g_Log.Log(LogType::_FILELOG, L"ActozBilling ConSock OK!!\r\n");
	}

	return true;
}

void CDNBillingConnectionKRAZ::Reconnect(DWORD dwCurTick)
{
	if (m_dwReconnectTick + 5000 < dwCurTick){
		m_dwReconnectTick = dwCurTick;

		if (!g_bSDKBilling){
			int nResult = OSP_CONNECTION_CHECK();
			if (nResult == 0){
				g_bSDKBilling = false;
				g_Log.Log(LogType::_FILEDBLOG, L"ActozBilling OSP_CONNECTION_CHECK Fail!! (Result:%d)\r\n", nResult);
			}
			else{
				g_bSDKBilling = true;
				g_Log.Log(LogType::_FILEDBLOG, L"ActozBilling OSP_CONNECTION_CHECK OK!! (Result:%d)\r\n", nResult);
			}
		}
	}
}

int CDNBillingConnectionKRAZ::OnBalanceInquiry(CDNUserRepository *pUser)
{
	int nResult = UserBalance(pUser->GetAccountDBID(), pUser->GetAccountNameW(), pUser->m_wszIp, 0);
	if (nResult < 0) return ERROR_GENERIC_INVALIDREQUEST;

	return ERROR_NONE;
}

int CDNBillingConnectionKRAZ::OnBuy(CDNUserRepository *pUser)
{
	CDNUserRepository::TBuyItem *pBuyItem = pUser->GetBuyItem(pUser->m_cCurProductCount);
	if (!pBuyItem) return ERROR_GENERIC_INVALIDREQUEST;

	if (pBuyItem->nPrice == 0){
		pUser->OnRecvBillingBuyItem(ERROR_NONE);
		g_Log.Log(LogType::_NORMAL, L"OnBuy 0Cash Item SN:%d ID:%d\r\n", pBuyItem->BuyItem.nItemSN, pBuyItem->BuyItem.CashItem.nItemID);
		return ERROR_NONE;
	}

	TITEM_INFO_UNICODE ItemInfo[MAX_ITEM_BUY_COUNT] = {0,};
	ItemInfo[0].itemID = pBuyItem->BuyItem.nItemSN;
	ItemInfo[0].itemCount = pBuyItem->BuyItem.CashItem.wCount;
	ItemInfo[0].itemPrice = pBuyItem->nPrice;

	ItemNameStrToActozStr(pBuyItem->BuyItem.nItemSN, ItemInfo[0].itemName);

	int nResult = UseMoney(pUser->GetAccountDBID(), true, pUser->GetAccountNameW(), pUser->GetCharacterNameW(), pUser->GetWorldID(), pUser->m_wszIp, L"-", L"-", pBuyItem->nPrice, 0, 0, 1, ItemInfo, 0);
	if (nResult < 0) return ERROR_GENERIC_INVALIDREQUEST;

	return ERROR_NONE;
}

int CDNBillingConnectionKRAZ::OnPackageBuy(CDNUserRepository *pUser)
{
	TITEM_INFO_UNICODE ItemInfo[MAX_ITEM_BUY_COUNT] = {0,};
	ItemInfo[0].itemID = pUser->GetPackageSN();
	ItemInfo[0].itemCount = 1;
	ItemInfo[0].itemPrice = pUser->m_nTotalPrice;

	ItemNameStrToActozStr(pUser->GetPackageSN(), ItemInfo[0].itemName);

	int nResult = UseMoney(pUser->GetAccountDBID(), true, pUser->GetAccountNameW(), pUser->GetCharacterNameW(), pUser->GetWorldID(), pUser->m_wszIp, L"-", L"-", pUser->m_nTotalPrice, 0, 0, 1, ItemInfo, 0);
	if (nResult < 0) return ERROR_GENERIC_INVALIDREQUEST;

	return ERROR_NONE;
}

int CDNBillingConnectionKRAZ::OnGift(CDNUserRepository *pUser)
{
	CDNUserRepository::TGiftItem *pGiftItem = pUser->GetGiftItem(pUser->m_cCurProductCount);
	if (!pGiftItem) return ERROR_GENERIC_INVALIDREQUEST;

	TITEM_INFO_UNICODE ItemInfo[MAX_ITEM_BUY_COUNT] = {0,};
	ItemInfo[0].itemID = pGiftItem->GiftItem.nItemSN;
	ItemInfo[0].itemCount = 1;
	ItemInfo[0].itemPrice = pGiftItem->nPrice;

	ItemNameStrToActozStr(pGiftItem->GiftItem.nItemSN, ItemInfo[0].itemName);

	int nResult = UseMoney(pUser->GetAccountDBID(), false, pUser->GetAccountNameW(), pUser->GetCharacterNameW(), pUser->GetWorldID(), pUser->m_wszIp, pUser->GetReceiverCharacterNameW(), L"-", pGiftItem->nPrice, 0, 0, 1, ItemInfo, 0);
	if (nResult < 0) return ERROR_GENERIC_INVALIDREQUEST;

	return ERROR_NONE;
}

int CDNBillingConnectionKRAZ::OnPackageGift(CDNUserRepository *pUser)
{
	TITEM_INFO_UNICODE ItemInfo[MAX_ITEM_BUY_COUNT] = {0,};
	ItemInfo[0].itemID = pUser->GetPackageSN();
	ItemInfo[0].itemCount = 1;
	ItemInfo[0].itemPrice = pUser->m_nTotalPrice;

	ItemNameStrToActozStr(pUser->GetPackageSN(), ItemInfo[0].itemName);

	int nResult = UseMoney(pUser->GetAccountDBID(), false, pUser->GetAccountNameW(), pUser->GetCharacterNameW(), pUser->GetWorldID(), pUser->m_wszIp, pUser->GetReceiverCharacterNameW(), L"-", pUser->m_nTotalPrice, 0, 0, 1, ItemInfo, 0);
	if (nResult < 0) return ERROR_GENERIC_INVALIDREQUEST;

	return ERROR_NONE;
}

int CDNBillingConnectionKRAZ::OnCoupon(CDNUserRepository *pUser)
{
	/*
	int nResult = UseCoupon(pUser->GetAccountDBID(), pUser->GetAccountNameW(), pUser->GetWorldID(), pUser->GetCharacterNameW(), pUser->m_wszIp, pUser->GetCouponW(), 0);
	if (nResult < 0) return ERROR_GENERIC_INVALIDREQUEST;
	*/

	pUser->SendCoupon(ERROR_NONE);		// 임시

	return ERROR_NONE;
}

int CDNBillingConnectionKRAZ::ConvertResult(const char cResultValue)
{
	switch(cResultValue)
	{
	case 'O': return ERROR_NONE;	// 성공
	case 'L': return ERROR_CASHSHOP_CASH_NOTENOUGH;	// 캐쉬부족
	default:
		return ERROR_GENERIC_INVALIDREQUEST;
	}
}

void CDNBillingConnectionKRAZ::ItemNameStrToActozStr(int nItemSN, WCHAR *pOut)
{
	std::string strItemName;
	g_pExtManager->GetCashCommodityName(nItemSN, strItemName);

	char szBuf[128] = {0,};

	StrToActozStr2(strItemName.c_str(), szBuf);

	USES_CONVERSION;
	std::wstring wstrItemName = A2CW(szBuf);

	_wcscpy(pOut, 50, wstrItemName.c_str(), (int)wcslen(wstrItemName.c_str()));
}

#endif	// #if defined(_KRAZ)