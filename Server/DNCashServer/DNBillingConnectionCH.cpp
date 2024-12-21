#include "StdAfx.h"
#include "DNBillingConnectionCH.h"
#include "DNUserRepository.h"
#include "DNManager.h"
#include "DNConnection.h"
#include "DNExtManager.h"
#include "DNSQLMembership.h"
#include "DNSQLWorld.h"
#include "DNSQLManager.h"
#include "Log.h"

#if defined(_CH) || defined(_EU)

CDNBillingConnectionCH *g_pBillingConnection = NULL;

CDNBillingConnectionCH::CDNBillingConnectionCH(void)
{
	m_bInitialize = FALSE;
}

CDNBillingConnectionCH::~CDNBillingConnectionCH(void)
{
}

bool CDNBillingConnectionCH::Init()
{
	DN_ASSERT(!IsInitialize(),	"Already Opened!");

	GSCallbackFuncDef CallbackFunc;
	::memset(&CallbackFunc, 0, sizeof(CallbackFunc));
	CallbackFunc.CallbackAccountAuthenRes = CallbackAccountAuthenRes;
	CallbackFunc.CallbackAccountLockRes = CallbackAccountLockRes;
	CallbackFunc.CallbackAccountUnlockRes = CallbackAccountUnlockRes;

	int nRetVal = GSInitialize(&CallbackFunc, "./Config/ClientConfig.ini", 1);
	if (nRetVal)
		return false;

	m_bInitialize = true;

	return true;
}

void CDNBillingConnectionCH::Final()
{
	if (m_bInitialize) {
		int nRetVal = Uninitialize(1);
		if (nRetVal)
			return;
	}

	m_bInitialize = false;
}

int CDNBillingConnectionCH::OnBalanceInquiry(CDNUserRepository *pUser)
{
	int nRet = pUser->CheckSessID();
	if (nRet != ERROR_NONE)
		return nRet;

	nRet = SendCheckBalance(pUser);
	if (nRet != ERROR_NONE)
		g_Log.Log(LogType::_ERROR, pUser, L"[CASH_BALANCEINQUIRY] CDNBillingConnectionCH::SendCheckBalance() - AccountName:%S Ret:%d\r\n", pUser->GetAccountName(), nRet);

	return nRet;
}

int CDNBillingConnectionCH::OnBuy(CDNUserRepository *pUser)
{
	pUser->SetOrderStep(SNDAORDERSTEP_WORK);

	CDNBillingConnectionCH::TGoodsInfo Goods[MAX_GOOD_LEN] = { 0, };
	int nCount = 0;
	GetBuyGoodsInfo(pUser->GetBuyItemPart(), Goods, nCount);

	int nRet = SendAccountLock(pUser, Goods, nCount, false);
	if (nRet != ERROR_NONE){
		g_Log.Log(LogType::_ERROR, pUser, L"[SendBillingPurchase] Ret : %d\r\n", nRet);
		return nRet;
	}

	return ERROR_NONE;
}

int CDNBillingConnectionCH::OnPackageBuy(CDNUserRepository *pUser)
{
	CDNBillingConnectionCH::TGoodsInfo Goods[MAX_GOOD_LEN] = { 0, };

	Goods[0].nItemSN = pUser->GetPackageSN();
	Goods[0].nPrice = pUser->m_nTotalPrice;

	int nRet = SendAccountLock(pUser, Goods, 1, false);
	if (nRet != ERROR_NONE){
		g_Log.Log(LogType::_ERROR, pUser, L"[SendBillingPackage] Ret : %d\r\n", nRet);
		return nRet;
	}

	return ERROR_NONE;
}

int CDNBillingConnectionCH::OnGift(CDNUserRepository *pUser)
{
	pUser->SetOrderStep(SNDAORDERSTEP_WORK);

	CDNBillingConnectionCH::TGoodsInfo Goods[MAX_GOOD_LEN] = { 0, };
	int nCount = 0;
	GetGiftGoodsInfo(pUser->GetGiftItemPart(), Goods, nCount);

	int nRet = SendAccountLock(pUser, Goods, nCount, true);
	if (nRet != ERROR_NONE){
		g_Log.Log(LogType::_ERROR, pUser, L"[SendBillingPurchaseGift] Ret : %d\r\n", nRet);
		return nRet;
	}

	return ERROR_NONE;
}

int CDNBillingConnectionCH::OnPackageGift(CDNUserRepository *pUser)
{
	CDNBillingConnectionCH::TGoodsInfo Goods[MAX_GOOD_LEN] = { 0, };

	Goods[0].nItemSN = pUser->GetPackageSN();
	Goods[0].nPrice = pUser->m_nTotalPrice;

	int nRet = SendAccountLock(pUser, Goods, 1, true);
	if (nRet != ERROR_NONE){
		g_Log.Log(LogType::_ERROR, pUser, L"[SendBillingPackage] Ret : %d\r\n", nRet);
		return nRet;
	}

	return ERROR_NONE;
}

int CDNBillingConnectionCH::OnCoupon(CDNUserRepository *pUser)
{
	return ERROR_GENERIC_INVALIDREQUEST;
}

int CDNBillingConnectionCH::SendCheckBalance(CDNUserRepository *pUser)
{
	DN_ASSERT(NULL != pUser,					"Invalid!");
	DN_ASSERT(0 != pUser->GetAccountDBID(),		"Invalid!");
	DN_ASSERT(NULL != pUser->GetSessId(),		"Invalid!");	// P.S.> CDNUserRepository 최초 생성 시 지정되어야 함

	// P.S.> 오류 로그는 호출부에서 기록

	int nRetVal = -1;

	GSBsipAccountAuthenReqDef Req;
	::memset(&Req, 0, sizeof(Req));

	Req.user_type = 1;				// 1 : 샨다 사용자 (기본값)
	Req.uid_type = 1;				// 1 : PT ID (문자) / 2 : SNDA ID (숫자)	// 중국은 SNDA ID 숫자를 문자열로 변경하여 계정명으로 사용하며 기존 PT ID 계정명은 사용자만 사용할 뿐 서비스 내에서는 절대 사용할 일 없음
	_strcpy(Req.user_id, _countof(Req.user_id), pUser->GetAccountName(), (int)strlen(pUser->GetAccountName()));
	_strcpy(Req.sess_id, _countof(Req.sess_id), pUser->GetSessId(), (int)strlen(pUser->GetSessId()) );
	nRetVal = GetNewId(Req.order_id);
	if (0 > nRetVal) {
		return nRetVal;
	}
	Req.serv_id_player = -1;		// 게임 번호, 디폴트값 -1 을 사용 시 Api가 설정파일 bsipconfig.ini 로부터 획득
	Req.area_id_player = -1;		// 게임 지역 번호, 디폴트값 -1 을 사용 시 Api가 설정파일 bsipconfig.ini 로부터 획득
	Req.group_id_player = -1;		// 게임 그룹 번호, 디폴트값 -1 을 사용 시 Api가 설정파일 bsipconfig.ini 로부터 획득
	Req.endpoint_type = 1;			// 1 : PC / 2 : 휴대폰
	_strcpy(Req.endpoint_ip, _countof(Req.endpoint_ip), pUser->m_szIp, (int)strlen(pUser->m_szIp));
	Req.platform_type = 1;			// 1 : PC / 2 : EZHOME
	{
		SYSTEMTIME aCurTime;
		::GetLocalTime(&aCurTime);
		sprintf_s(Req.call_time, "%04d-%02d-%02d %02d:%02d:%02d", aCurTime.wYear, aCurTime.wMonth, aCurTime.wDay, aCurTime.wHour, aCurTime.wMinute, aCurTime.wSecond);
	}
	Req.indication_authen = 1;		// 1 : 기본값
	Req.pay_type = 0;				// 0 ; 기본값

	nRetVal = SendAccountAuthenRequest(&Req);
	if (nRetVal < 0)
		g_Log.Log(LogType::_ERROR, L"SendAccountAuthenRequest Ret:%d\r\n", nRetVal);

	return ConvertResult(nRetVal);
}

int CDNBillingConnectionCH::SendAccountLock(CDNUserRepository *pUser, TGoodsInfo *GoodsList, int nGoodsCount, bool bGift)
{
	if (!pUser) return ERROR_GENERIC_INVALIDREQUEST;
	if (!GoodsList) return ERROR_GENERIC_INVALIDREQUEST;
	if (nGoodsCount <= 0) return ERROR_GENERIC_INVALIDREQUEST;

	int nRetVal = -1;

	GSBsipAccountLockReq Req;
	::memset(&Req, 0, sizeof(Req));

	Req.pay_type = 3;				// 3 : 샨다 캐쉬
	Req.app_type = bGift ? 174 : 17;	// 17 : 기본값, 174: 선물
	Req.status = 2;					// 2 : 사용자 상태 온라인
	Req.couple_type = 2;			// 2 : loose coupling, 사용자가 사용할 수 있는 화폐타입, 빌링 백그라운드에서 컨트롤
	Req.fee = 0;					// 수수료
	Req.lock_period = 0;			// 0 : 기본값, 20분
	Req.uid_type = 1;				// 1 : PT ID (문자) / 2 : SNDA ID (숫자)	// 중국은 SNDA ID 숫자를 문자열로 변경하여 계정명으로 사용하며 기존 PT ID 계정명은 사용자만 사용할 뿐 서비스 내에서는 절대 사용할 일 없음
	_strcpy(Req.user_id, _countof(Req.user_id), pUser->GetAccountName(), (int)strlen(pUser->GetAccountName()));
	_strcpy(Req.sess_id, _countof(Req.sess_id), pUser->GetSessId(), (int)strlen(pUser->GetSessId()));
	Req.serv_id_player = -1;		// 게임 번호, 디폴트값 -1 을 사용 시 Api가 설정파일 bsipconfig.ini 로부터 획득
	Req.area_id_player = -1;		// 게임 지역 번호, 디폴트값 -1 을 사용 시 Api가 설정파일 bsipconfig.ini 로부터 획득
	Req.group_id_player = -1;		// 게임 그룹 번호, 디폴트값 -1 을 사용 시 Api가 설정파일 bsipconfig.ini 로부터 획득
	Req.endpoint_type = 1;			// 1 : PC / 2 : 휴대폰
	_strcpy(Req.endpoint_ip, _countof(Req.endpoint_ip), pUser->m_szIp, (int)strlen(pUser->m_szIp));
	Req.platform_type = 1;			// 1 : PC / 2 : EZHOME
	{
		SYSTEMTIME CurTime;
		::GetLocalTime(&CurTime);
		sprintf_s(Req.call_time, "%04d-%02d-%02d %02d:%02d:%02d", CurTime.wYear, CurTime.wMonth, CurTime.wDay, CurTime.wHour, CurTime.wMinute, CurTime.wSecond);
	}
	Req.indication_lock = 1;		// 1 : 기본값 (1 : Lock account, 2 : Reduce account, 5 : Reduce account and get coupon num)
	Req.item_num = nGoodsCount;		// 캐쉬 아이템 결재 요청은 1회에 최대 MAX_GOOD_LEN 개까지 수행

	nRetVal = GetNewId(Req.context_id);	// AccountLock / AccountUnlock 이 한 쌍이 되어야 함
	if (0 > nRetVal)
		g_Log.Log(LogType::_ERROR, L"GetNewId Ret:%d\r\n", nRetVal);

	pUser->SetContextId(Req.context_id);

	nRetVal = GetNewId(Req.order_id);		// AccountLock / AccountUnlock 매회 호출 시 마다 생성 되어야 함
	if (0 > nRetVal)
		g_Log.Log(LogType::_ERROR, L"GetNewId Ret:%d\r\n", nRetVal);

	for (int i = 0; i < nGoodsCount; i++){
		if (GoodsList[i].nItemSN <= 0) continue;

		Req.goods_info[i].area_id_item = -1;		// -1 : 기본값
		Req.goods_info[i].serv_id_item = -1;		// -1 : 기본값
		Req.goods_info[i].group_id_item = -1;		// -1 : 기본값
		Req.goods_info[i].discount = 100;			// 100 : 기본값 (할인 없음)
		Req.goods_info[i].item_num = 1;			// 캐쉬 아이템 결재 요청은 1회씩 수행
		Req.goods_info[i].item_id = GoodsList[i].nItemSN;		// 1번째 캐쉬 아이템의 CommodityID 임 (ItemID 아님 BuyItem->BuyItem.CashItem.nItemID)
		Req.goods_info[i].item_amount = GoodsList[i].nPrice;
		DN_ASSERT(0 < Req.goods_info[i].item_amount, "Check!");
	}

	nRetVal = SendAccountLockRequest(&Req);
	if (nRetVal < 0)
		nRetVal = ConvertResult(nRetVal);

	return nRetVal;
}

int CDNBillingConnectionCH::SendAccountUnlock(CDNUserRepository *pUser, bool bConfirm, const char *pContextId, TGoodsInfo *GoodsList, int nGoodsCount, bool bGift)
{
	DN_ASSERT(NULL != pUser,			"Invalid!");
	DN_ASSERT(NULL != pContextId,		"Invalid!");

	// P.S.> 오류 로그는 호출부에서 기록

	int nRetVal = -1;

	GSBsipAccountUnlockReq Req;
	Req.pay_type = 3;				// 3 : 샨다 캐쉬
	Req.app_type = bGift ? 174 : 17;	// 17: 기본값 174: 선물
	Req.status = 2;					// 2 : 사용자 상태 온라인
	Req.couple_type = 2;			// 2 : loose coupling, 사용자가 사용할 수 있는 화폐타입, 빌링 백그라운드에서 컨트롤
	Req.responsable = 1;			// 1 : 기본값 (AccountUnlockRes 반환 여부)
	Req.uid_type = 1;				// 1 : PT ID (문자) / 2 : SNDA ID (숫자)	// 중국은 SNDA ID 숫자를 문자열로 변경하여 계정명으로 사용하며 기존 PT ID 계정명은 사용자만 사용할 뿐 서비스 내에서는 절대 사용할 일 없음
	_strcpy(Req.user_id, _countof(Req.user_id), pUser->GetAccountName(), (int)strlen(pUser->GetAccountName()));
	_strcpy(Req.sess_id, _countof(Req.sess_id), pUser->GetSessId(), (int)strlen(pUser->GetSessId()));
	_strcpy(Req.context_id, _countof(Req.context_id), pContextId, (int)strlen(pContextId));
	{
		nRetVal = GetNewId(Req.order_id);	// AccountLock / AccountUnlock 매회 호출 시 마다 생성 되어야 함
		if (0 > nRetVal)
			g_Log.Log(LogType::_ERROR, L"GetNewId Ret:%d\r\n", nRetVal);
	}
	Req.serv_id_player = -1;		// 게임 번호, 디폴트값 -1 을 사용 시 Api가 설정파일 bsipconfig.ini 로부터 획득
	Req.area_id_player = -1;		// 게임 지역 번호, 디폴트값 -1 을 사용 시 Api가 설정파일 bsipconfig.ini 로부터 획득
	Req.group_id_player = -1;		// 게임 그룹 번호, 디폴트값 -1 을 사용 시 Api가 설정파일 bsipconfig.ini 로부터 획득
	Req.endpoint_type = 1;			// 1 : PC / 2 : 휴대폰
	_strcpy(Req.endpoint_ip, _countof(Req.endpoint_ip), pUser->m_szIp, (int)strlen(pUser->m_szIp));

	Req.platform_type = 1;			// 1 : PC / 2 : EZHOME
	{
		SYSTEMTIME aCurTime;
		::GetLocalTime(&aCurTime);
		sprintf_s(Req.call_time, "%04d-%02d-%02d %02d:%02d:%02d", aCurTime.wYear, aCurTime.wMonth, aCurTime.wDay, aCurTime.wHour, aCurTime.wMinute, aCurTime.wSecond);
	}
	Req.indication_unlock = 1;				// 1 : 기본값 (1 : Unlock account, 2 : Reduce account, 5 : Reduce account and get coupon num)
	Req.item_num = nGoodsCount;				// 캐쉬 아이템 결재 요청은 1회에 최대 MAX_GOOD_LEN 개까지 수행
	Req.confirm = (bConfirm)?(1):(2);		// 캐쉬 아이템 결재 최종 승인 여부 (1 : 커밋 / 2 : 롤백)
	Req.amount = 0;

	for (int i = 0; i < nGoodsCount; i++){
		if (GoodsList[i].nItemSN <= 0) continue;

		Req.goods_info[i].area_id_item = -1;	// -1 : 기본값
		Req.goods_info[i].serv_id_item = -1;	// -1 : 기본값
		Req.goods_info[i].group_id_item = -1;	// -1 : 기본값
		Req.goods_info[i].discount = 100;		// 100 : 기본값 (할인 없음)
		Req.goods_info[i].item_num = 1;		// 캐쉬 아이템 결재 요청은 1회씩 수행
		Req.goods_info[i].item_id = GoodsList[i].nItemSN;		// CommodityID 임 (CashPackageTable 에서 다시 개별 캐쉬 상품의 CommodityID 로 나뉨)
		Req.goods_info[i].item_amount = GoodsList[i].nPrice;	// 캐쉬 아이템 결재 금액
		Req.amount += Req.goods_info[i].item_amount;
	}

	nRetVal = SendAccountUnlockRequest(&Req);
	if (nRetVal < 0)
		nRetVal = ConvertResult(nRetVal);

	return nRetVal;
}

void CDNBillingConnectionCH::CallbackAccountAuthenRes(GSBsipAccountAuthenResDef* pRes)
{
	if (!pRes) return;

	CDNUserRepository *pUser = g_pManager->GetUser(pRes->user_id);
	if (!pUser){
		g_Log.Log(LogType::_ERROR, L"[CallbackAccountAuthenRes] User NULL! (AccountName:%s) \r\n", pRes->user_id);
		return;
	}

	UINT nBalance = 0;
	int nResult = ConvertResult(pRes->result);

	if (ERROR_NONE == nResult) {
		if (3 == pRes->pay_type1 &&		// 지불타입 3 : 샨다 캐쉬
			0 < pRes->balance1)
		{
			nBalance += pRes->balance1;
		}
/*
		if (3 == pRes->pay_type2 &&		// 지불타입 3 : 샨다 캐쉬
			0 < pRes->balance2)
		{
			nBalance += pRes->balance2;
		}
		if (3 == pRes->pay_type3 &&		// 지불타입 3 : 샨다 캐쉬
			0 < pRes->balance3)
		{
			nBalance += pRes->balance3;
		}
		if (3 == pRes->pay_type4 &&		// 지불타입 3 : 샨다 캐쉬
			0 < pRes->balance4)
		{
			nBalance += pRes->balance4;
		}
		if (3 == pRes->pay_type5 &&		// 지불타입 3 : 샨다 캐쉬
			0 < pRes->balance5)
		{
			nBalance += pRes->balance5;
		}
*/
	}
	else {
		g_Log.Log(LogType::_ERROR, pUser, L"[CallbackAccountAuthenRes] RetVal : %d\r\n", pRes->result);	// 20100621
	}

	pUser->SendBalanceInquiry(nResult, nBalance);
}

void CDNBillingConnectionCH::CallbackAccountLockRes(GSBsipAccountLockResDef* pRes)
{
	if (!pRes) return;

	CDNUserRepository *pUser = g_pManager->GetUser(pRes->user_id);
	if (!pUser) {
		g_Log.Log(LogType::_ERROR, L"[CallbackAccountAuthenRes] User NULL! (AccountName:%s) \r\n", pRes->user_id);
		return;
	}

	pUser->OnRecvAccountLock(pRes->result, pRes->context_id);
}

void CDNBillingConnectionCH::CallbackAccountUnlockRes(GSBsipAccountUnlockResDef* pRes)
{
	if (!pRes) return;

	CDNUserRepository *pUser = g_pManager->GetUser(pRes->user_id);
	if (!pUser) {
		g_Log.Log(LogType::_ERROR, L"[CallbackAccountAuthenRes] User NULL! (AccountName:%s) \r\n", pRes->user_id);
		return;
	}

	pUser->OnRecvAccountUnlock(pRes->result, pRes->context_id, pRes->order_id, pRes->balance);
}

void CDNBillingConnectionCH::GetBuyGoodsInfo(CDNUserRepository::TBuyItemPart *pBuy, TGoodsInfo *GoodsList, int &nTotalCount)
{
	if (!pBuy) return;
	nTotalCount = 0;

	for (int i = 0; i < MAX_GOOD_LEN; i++){
		if (!pBuy->pPartItem[i]) continue;

		GoodsList[nTotalCount].nItemSN = pBuy->pPartItem[i]->BuyItem.nItemSN;
		GoodsList[nTotalCount].nPrice = pBuy->pPartItem[i]->nPrice;
		nTotalCount++;
	}
}

void CDNBillingConnectionCH::GetGiftGoodsInfo(CDNUserRepository::TGiftItemPart *pGift, TGoodsInfo *GoodsList, int &nTotalCount)
{
	if (!pGift) return;
	nTotalCount = 0;

	for (int i = 0; i < MAX_GOOD_LEN; i++){
		if (!pGift->pPartItem[i]) continue;

		GoodsList[nTotalCount].nItemSN = pGift->pPartItem[i]->GiftItem.nItemSN;
		GoodsList[nTotalCount].nPrice = pGift->pPartItem[i]->nPrice;
		nTotalCount++;
	}
}

int CDNBillingConnectionCH::ConvertResult(int nBillingResult)
{
	if (0 <= nBillingResult) {
		return ERROR_NONE;
	}

	switch(nBillingResult) {
	case -106908:	// the account is not exist or the balance is 0		// AccountAuthenRes 의 결과
// 	case -106114:	// user balance is 0
// 	case -106718:	// user balance is 0
		return ERROR_NONE;

	case -100201: return ERROR_SHANDAAUTHOR_100201;						// -100201 = PT 계정 에러	ptId format error
	case -100202: return ERROR_SHANDAAUTHOR_100202;						// -100202	게임 ID 에러	appId error
	case -100203: return ERROR_SHANDAAUTHOR_100203;						// -100203	파티션 ID 에러	areaId error
	case -100206: return ERROR_SHANDAAUTHOR_100206;						// -100206	계정이 존재하지 않음	Account not exist
	case -100207: return ERROR_SHANDAAUTHOR_100207;						// -100207	파티션 ID 존재하지 않음	areaId not exist
	case -100208: return ERROR_SHANDAAUTHOR_100208;						// -100208	게임 ID 존재하지 않음	appId not exist
	case -100210: return ERROR_SHANDAAUTHOR_100210;						// -100210	기타 기록을 찾지 못한 에러	other error
	case -100213: return ERROR_SHANDAAUTHOR_100213;						// -100213	sdid 너무 길거나 영문문자가 포함됨	sdId format error
	case -100299: return ERROR_SHANDAAUTHOR_100299;						// -100299	DB 에러	DB error
	case -120101: return ERROR_SHANDAAUTHOR_120101;						// -120101	숫자 계정이 너무 길거나 영문문자가 포함됨	sndaID format error
	case -120102: return ERROR_SHANDAAUTHOR_120102;						// -120102	PT 계정이 너무 길거나 전부 다 숫자임	ptId format error
	case -120103: return ERROR_SHANDAAUTHOR_120103;						// -120103	유저 계정에 대응되는 accountid 에러	accountID error
	case -120104: return ERROR_SHANDAAUTHOR_120104;						// -120104	잔액 에러	balance error
	case -120105: return ERROR_SHANDAAUTHOR_120105;						// -120105	과금 타입 에러	paytypeId error
	case -120108: return ERROR_SHANDAAUTHOR_120108;						// -120108	LBS 로컬 DB에 해당 데이터가 없음	no data in LBS DB
	case -120109: return ERROR_SHANDAAUTHOR_120109;						// -120109	기타 에러	other errror
	case -120111: return ERROR_SHANDAAUTHOR_120111;						// -120111	게임 ID 에러	appId error
	case -120112: return ERROR_SHANDAAUTHOR_120112;						// -120112	네트워크 상태 에러	Net status error
	case -120113: return ERROR_SHANDAAUTHOR_120113;						// -120113	메시지 에러(메시지 내용 에러)	data body error
	case -129999: return ERROR_SHANDAAUTHOR_129999;						// -129999	DB 에러	DB error
	// Account
	case -102601: return ERROR_SHANDAACCOUNT_102601;					// -102601	과금 메시지 상태 에러	Status error
	case -102602: return ERROR_SHANDAACCOUNT_102602;					// -102602	과금 타입 에러	Paytype error
	case -102603: return ERROR_SHANDAACCOUNT_102603;					// -102603	과금 금액이 0보다 작음	Amount error
	case -102613: return ERROR_SHANDAACCOUNT_102613;					// -102613	패킷 log 시간이 5일전임	Calltime error
	case -102604: return ERROR_SHANDAACCOUNT_102604;					// -102604	PT 계정이 존재하지 않음	Account not exist
	case -102605: return ERROR_SHANDAACCOUNT_102605;					// -102605	게임 파티션 계정이 존재하지 않음	balance not exist
	case -102609: return ERROR_SHANDAACCOUNT_102609;					// -102609	sdid 너무 길거나 영문문자가 포함됨	sdId format error
	case -102610: return ERROR_SHANDAACCOUNT_102610;					// -102610	소비 타입 에러	appType error
	case -102611: return ERROR_SHANDAACCOUNT_102611;					// -102611	커플링 타입 에러, 반드시 1,2 여야 함	coupletype error
	case -102612: return ERROR_SHANDAACCOUNT_102612;					// -102612	begintime이 endtime보다 작거나 같아야 함	begintime must samll to endtime
	case -102615: return ERROR_SHANDAACCOUNT_102615;					// -102615	amount = 0	Amount is 0
	case -102617: return ERROR_SHANDAACCOUNT_102617;					// -102617	기타 에러	other error
	case -102699: return ERROR_SHANDAACCOUNT_102699;					// -102699	DB 에러	DB error
	case -120201: return ERROR_SHANDAACCOUNT_120201;					// -120201	숫자 계정이 너무 길거나 영문문자가 포함됨	sndaID format error
	case -120202: return ERROR_SHANDAACCOUNT_120202;					// -120202	PT 계정이 너무 길거나 전부 다 숫자임	ptId format error
	case -120203: return ERROR_SHANDAACCOUNT_120203;					// -120203	과금 수량 포맷 에러	Amount error
	case -120204: return ERROR_SHANDAACCOUNT_120204;					// -120204	패킷 업로드 시간간격 에러	period error
	case -120205: return ERROR_SHANDAACCOUNT_120205;					// -120205	계정이 존재하지 않음	ID not exist
	case -120206: return ERROR_SHANDAACCOUNT_120206;					// -120206	통합 기록이 없음	no union data
	case -120207: return ERROR_SHANDAACCOUNT_120207;					// -120207	지급 타입 에러	paytypeId error
	case -120208: return ERROR_SHANDAACCOUNT_120208;					// -120208	숫자 계정이 존재하지 않음	sndaID not exist
	case -120209: return ERROR_SHANDAACCOUNT_120209;					// -120209	기타 파라미터 포맷 에러	other param format error
	case -120210: return ERROR_SHANDAACCOUNT_120210;					// -120210	orderid 중복	orderid error
	case -120211: return ERROR_SHANDAACCOUNT_120211;					// -120211	serviceid 에러	appId error
	case -120212: return ERROR_SHANDAACCOUNT_120212;					// -120212	네트워크 상태 에러	net status error
	case -120213: return ERROR_SHANDAACCOUNT_120213;					// -120213	cache flag 에러	flag error
	case -120214: return ERROR_SHANDAACCOUNT_120214;					// -120214	상태 파라미터 에러	status error
	case -120215: return ERROR_SHANDAACCOUNT_120215;					// -120215	한개 sessionid에 여러개 기록이 존재	Data repeat
	case -120216: return ERROR_SHANDAACCOUNT_120216;					// -120216	ptid와 sessionid가 매치되지 않음	ptId and sessid error
	case -120217: return ERROR_SHANDAACCOUNT_120217;					// -120217	PT 계정이 존재하지 않음	ptID not exist
	// case -129999: return ERROR_SHANDAACCOUNT_129999;					// -129999	시스템 에러＋sqlcode	other error
	// 유저 숫자계정			
	case -200101: return ERROR_SHANDABILLING_200101;					// SDIDLONGER	-200101	입력길이 초과	long
	case -200102: return ERROR_SHANDABILLING_200102;					// SDIDNULL	-200102	비어있음	null
	case -200103: return ERROR_SHANDABILLING_200103;					// SDIDILLEGAL	-200103	불법 문자열이 포함됨	format error
	case -200199: return ERROR_SHANDABILLING_200199;					// SDIDOTHER	-200199	기타 에러	other error
	// 유저 PT 계정			
	case -200201: return ERROR_SHANDABILLING_200201;					// PTIDLONGER	-200201	입력길이 초과	long
	case -200202: return ERROR_SHANDABILLING_200202;					// PTIDNULL	-200202	비어있음	null
	case -200203: return ERROR_SHANDABILLING_200203;					// PTIDILLEGAL	-200203	불법 문자열이 포함됨	format error
	case -200204: return ERROR_SHANDABILLING_200204;					// PTIDALLNUM	-200204	전부 다 숫자거나 숫자로 시작됨	format error
	case -200299: return ERROR_SHANDABILLING_200299;					// PTIDOTHER	-200299	기타 에러	other error
	// 서비스 타입 ID			
	case -200301: return ERROR_SHANDABILLING_200301;					// SERVIDLONGER	-200301	입력길이 초과	long
	case -200302: return ERROR_SHANDABILLING_200302;					// SERVIDNULL	-200302	비어있음	null
	case -200303: return ERROR_SHANDABILLING_200303;					// SERVIDILLEGAL	-200303	불법 문자열이 포함됨	format error
	case -200399: return ERROR_SHANDABILLING_200399;					// SERVIDOTHER	-200399	기타 에러	other error
	// 게임 파티션 ID	
	case -200401: return ERROR_SHANDABILLING_200401;					// AREAIDLONGER	-200401	입력길이 초과	long
	case -200402: return ERROR_SHANDABILLING_200402;					// AREAIDNULL	-200402	비어있음	null
	case -200403: return ERROR_SHANDABILLING_200403;					// AREAIDILLEGAL	-200403	불법 문자 포함됨	format error
	case -200499: return ERROR_SHANDABILLING_200499;					// AREAIDOTHER	-200499	기타 에러	other error
	// 세션 id			
	case -200501: return ERROR_SHANDABILLING_200501;					// SESSIDLONGER	-200501	입력길이 초과	long
	case -200502: return ERROR_SHANDABILLING_200502;					// SESSIDNULL	-200502	비어있음	null
	case -200503: return ERROR_SHANDABILLING_200503;					// SESSIDILLEGAL	-200503	불법 문자 포함됨	format error
	case -200504: return ERROR_SHANDABILLING_200504;					// SESSIDERRFORMAT	-200504	포멧 에러(조합 불법)	format error
	case -200599: return ERROR_SHANDABILLING_200599;					// SESSIDOTHER	-200599	기타 에러	other error
	// 데이터패킷 id			
	case -200601: return ERROR_SHANDABILLING_200601;					// UNIQUEIDLONGER	-200601	입력길이 초과	long
	case -200602: return ERROR_SHANDABILLING_200602;					// UNIQUEIDNULL	-200602	비어있음	null
	case -200603: return ERROR_SHANDABILLING_200603;					// UNIQUEIDILLEGAL	-200603	불법 문자 포함됨	format error
	case -200604: return ERROR_SHANDABILLING_200604;					// UNIQUEIDERRFORMAT	-200604	포멧 에러(조합 불법)	format error
	case -200605: return ERROR_SHANDABILLING_200605;					// UNIQUEIDREPEAT	-200605	uniqueid 중복됨	id repeat
	case -200699: return ERROR_SHANDABILLING_200699;					// UNIQUEIDOTHER	-200699	기타 에러	null
	// 월드 ID			
	case -200801: return ERROR_SHANDABILLING_200801;					// GROUPIDLONGER	-200801	입력길이 초과	long
	case -200802: return ERROR_SHANDABILLING_200802;					// GROUPIDNULL	-200802	비어있음	null
	case -200803: return ERROR_SHANDABILLING_200803;					// GROUPIDILLEGAL	-200803	불법 문자 포함됨	format error
	case -200899: return ERROR_SHANDABILLING_200899;					// GROUPIDOTHER	-200899	기타 에러	other error
	// 시작시간			
	case -201301: return ERROR_SHANDABILLING_201301;					// BEGINTIMELONGER	-201301	입력길이 초과	long
	case -201302: return ERROR_SHANDABILLING_201302;					// BEGINTIMENULL	-201302	비어있음	null
	case -201303: return ERROR_SHANDABILLING_201303;					// BEGINTIMEILLEGAL	-201303	불법 문자 포함됨	format error
	case -201304: return ERROR_SHANDABILLING_201304;					// BEGINTIMEERRLONGFOMAT	-201304	포맷 에러, YYYY-MM-DD hh:mi:ss 포맷이어야 함	format error
	case -201305: return ERROR_SHANDABILLING_201305;					// BEGINTIMEERRSHORTFOMAT	-201305	불법 문자 포함됨，YYYY-MM-DD 포맷이어야 함	format error
	case -201399: return ERROR_SHANDABILLING_201399;					// BEGINTIMEOTHER	-201399	기타 에러	other error
	// 종료시간			
	case -201401: return ERROR_SHANDABILLING_201401;					// ENDTIMELONGER	-201401	입력길이 초과	long
	case -201402: return ERROR_SHANDABILLING_201402;					// ENDTIMENULL	-201402	비어있음	null
	case -201403: return ERROR_SHANDABILLING_201403;					// ENDTIMEILLEGAL	-201403	불법 문자 포함됨	format error
	case -201404: return ERROR_SHANDABILLING_201404;					// ENDTIMEERRLONGFOMAT	-201404	포맷 에러, YYYY-MM-DD hh:mi:ss 포맷이어야 함	format error
	case -201405: return ERROR_SHANDABILLING_201405;					// ENDTIMEERRSHORTFOMAT	-201405	불법 문자 포함됨，YYYY-MM-DD 포맷이어야 함	format error
	case -201406: return ERROR_SHANDABILLING_201406;					// ENDTIMEERRVALUE	-201406	종료시간이 시작시간보다 큼	error value
	case -201499: return ERROR_SHANDABILLING_201499;					// ENDTIMEOTHER	-201499	기타 에러	other error
	// 데이터 패킷 id			
	case -201501: return ERROR_SHANDABILLING_201501;					// ORDERIDLONGER	-201501	입력길이 초과	long
	case -201502: return ERROR_SHANDABILLING_201502;					// ORDERIDNULL	-201502	비어있음	null
	case -201503: return ERROR_SHANDABILLING_201503;					// ORDERIDILLEGAL	-201503	불법 문자 포함됨	format error
	case -201504: return ERROR_SHANDABILLING_201504;					// ORDERIDERRFORMAT	-201504	포맷 에러（조합 불법）	format error
	case -201505: return ERROR_SHANDABILLING_201505;					// ORDERIDREPEAT	-201505	orderid중복	ID repeat
	case -201599: return ERROR_SHANDABILLING_201599;					// ORDERIDOTHER	-201599	기타 에러	other error
	// 과금 타입			
	case -201901: return ERROR_SHANDABILLING_201901;					// PAYTYPELONGER	-201901	입력길이 초과	long
	case -201902: return ERROR_SHANDABILLING_201902;					// PAYTYPENULL	-201902	비어있음	null
	case -201903: return ERROR_SHANDABILLING_201903;					// PAYTYPEILLEGAL	-201903	불법 문자 포함됨	format error
	case -201904: return ERROR_SHANDABILLING_201904;					// PAYTYPEERRVALUE	-201904	입력 에러(특정값을 가지지 않음)	error value
	case -201999: return ERROR_SHANDABILLING_201999;					// PAYTYPEOTHER	-201999	기타 에러	other error
	// 소비 타입			
	case -202001: return ERROR_SHANDABILLING_202001;					// APPTYPELONGER	-202001	입력길이 초과	long
	case -202002: return ERROR_SHANDABILLING_202002;					// APPTYPENULL	-202002	비어있음	null
	case -202003: return ERROR_SHANDABILLING_202003;					// APPTYPEILLEGAL	-202003	불법 문자 포함됨	format error
	case -202004: return ERROR_SHANDABILLING_202004;					// APPTYPEERRVALUE	-202004	입력 에러(특정값을 가지지 않음)	error value
	case -202099: return ERROR_SHANDABILLING_202099;					// APPTYPEOTHER	-202099	기타 에러	other error
	// 커플링 타입			
	case -202101: return ERROR_SHANDABILLING_202101;					// COUPLETYPELONGER	-202101	입력길이 초과	long
	case -202102: return ERROR_SHANDABILLING_202102;					// COUPLETYPENULL	-202102	비어있음	null
	case -202103: return ERROR_SHANDABILLING_202103;					// COUPLETYPEILLEGAL	-202103	불법 문자 포함됨	format error
	case -202104: return ERROR_SHANDABILLING_202104;					// COUPLETYPEERRVALUE	-202104	입력 에러(특정값을 가지지 않음)	error value
	case -202199: return ERROR_SHANDABILLING_202199;					// COUPLETYPEOTHER	-202199	기타 에러	other error
	// 금액			
	case -202201: return ERROR_SHANDABILLING_202201;					// AMOUNTLONGER	-202201	입력길이 초과	long
	case -202202: return ERROR_SHANDABILLING_202202;					// AMOUNTNULL	-202202	비어있음	null
	case -202203: return ERROR_SHANDABILLING_202203;					// AMOUNTILLEGAL	-202203	불법 문자 포함됨	format error
	case -202204: return ERROR_SHANDABILLING_202204;					// AMOUNTNEGATIVE	-202204	마이너스 숫자 입력	error value
	case -202205: return ERROR_SHANDABILLING_202205;					// AMOUNTZERO	-202205	0을 입력	error value
	case -202206: return ERROR_SHANDABILLING_202206;					// AMOUNTERRVALUE	-202206	입력한 숫자가 너무 큼	error value
	case -202299: return ERROR_SHANDABILLING_202299;					// AMOUNTOTHER	-202299	기타 에러	other error
	// 빌링 할인율			
	case -202301: return ERROR_SHANDABILLING_202301;					// DISCOUNTLONGER	-202301	입력길이 초과	long
	case -202302: return ERROR_SHANDABILLING_202302;					// DISCOUNTNULL	-202302	비어있음	null
	case -202303: return ERROR_SHANDABILLING_202303;					// DISCOUNTILLEGAL	-202303	불법 문자 포함됨	format error
	case -202399: return ERROR_SHANDABILLING_202399;					// DISCOUNTOTHER	-202399	기타 에러	other error
	// 클라이언트 IP 주소			
	case -202501: return ERROR_SHANDABILLING_202501;					// IPLONGER	-202501	입력길이 초과	long
	case -202502: return ERROR_SHANDABILLING_202502;					// IPNULL	-202502	비어있음	null
	case -202503: return ERROR_SHANDABILLING_202503;					// IPILLEGAL	-202503	불법 문자 포함됨	format error
	case -202504: return ERROR_SHANDABILLING_202504;					// IPERRVALUE	-202504	입력 에러(지정 범위내에 없음)	error value
	case -202599: return ERROR_SHANDABILLING_202599;					// IPOTHER	-202599	기타 에러	other error
	// 상태			
	case -204401: return ERROR_SHANDABILLING_204401;					// STATUSLONGER	-204401	입력길이 초과	long
	case -204402: return ERROR_SHANDABILLING_204402;					// STATUSNULL	-204402	비어있음	null
	case -204403: return ERROR_SHANDABILLING_204403;					// STATUSILLEGAL	-204403	불법 문자 포함됨	format error
	case -204404: return ERROR_SHANDABILLING_204404;					// STATUSERRVALUE	-204404	입력 에러(지정한 값과 일치하지 않음)	error value
	case -204499: return ERROR_SHANDABILLING_204499;					// STATUSOTHER	-204499	기타 에러	other error
	// 유저 uid			
	case -200211: return ERROR_SHANDABILLING_200211;					// UIDLONGER	-200211	입력길이 초과	long
	// CBS서비스 처리			
	case -300001: return ERROR_SHANDABILLING_300001;					// CBSDB_IS_DISCONNECT	-300001	cbs DB 접속 에러	sql connect error
	case -300102: return ERROR_SHANDABILLING_300102;					// CBSDB_PROC_ERROR	-300002	cbs sql 실행 에러	sql error
	case -300103: return ERROR_SHANDABILLING_300103;					// UNAUTHOR_OPERATION	-300003	권한 부여하지 않은 요청 패킷 타입	unauthor
	case -300201: return ERROR_SHANDABILLING_300201;					// AMOUNT_NOT_MATCHING	-300201	금액 불일치	amount error
	case -390001: return ERROR_SHANDABILLING_390001;					// RES_TIMEOUT	-390001	응답 시간 초과	response time out
	// 통신 반환-API			
	case -401001: return ERROR_SHANDABILLING_401001;					// SOCKDISCONNECT	-401001	socket 연결 끊김	socket disconnect
	case -401002: return ERROR_SHANDABILLING_401002;					// SOCKIP_OR_PORTERR	-401002	서버 ip/port 에러	IP Port error
	case -401003: return ERROR_SHANDABILLING_401003;					// SOCKCONNECTFAIL	-401003	socket 연결 실패	socket connect fail
	case -401004: return ERROR_SHANDABILLING_401004;					// SOCKLOCALFAILED	-401004	로컬 초기화 SOCKET 실패	initial fail
	case -401005: return ERROR_SHANDABILLING_401005;					// SNDBUF_OVERFLOW	-401005	발송 buffer overflow	send buffer overflow
	case -401006: return ERROR_SHANDABILLING_401006;					// RCVBUF_OVERFLOW	-401006	접수 buffer overflow	recive buffer overflow
	case -401007: return ERROR_SHANDABILLING_401007;					// TMOBUF_OVERFLOW	-401007	timeout buffer overflow	timeout buffer overflow
	case -401008: return ERROR_SHANDABILLING_401008;					// LSOCKDISCONNECT	-401008	통신 에러	socket error
	case -401011: return ERROR_SHANDABILLING_401011;					// NOINITCLIENT	-401011	클라이언트 초기화하지 않음	client without initial
	// QUEUE 처리			
	case -402001: return ERROR_SHANDABILLING_402001;					// QUEUECACHEFULL	-402001	행렬 cache full	queue cache full
	case -402002: return ERROR_SHANDABILLING_402002;					// QUEUELOCKERROR	-402002	행렬 lock 에러	queue lock error
	case -402003: return ERROR_SHANDABILLING_402003;					// QUEUEOPERATEERROR	-402003	행렬 Push/Pop 전송 파라미터 에러(빈 pointer)	queue param error
	case -402004: return ERROR_SHANDABILLING_402004;					// QUEUEEMPTY	-402004	행렬이 비어있음, Pop 데이터 없음	queue null
	case -402005: return ERROR_SHANDABILLING_402005;					// QUEUEDATAERR	-402005	cache 내용 에러, 길이가 cache 가능 범위 초과 등	queue data error
	// 데이터 전송			
	case -403001: return ERROR_SHANDABILLING_403001;					// USERDATALENERROR	-403001	유저 데이터 Push 길이와 서비스 구조 길이 불일치	data length error
	case -403002: return ERROR_SHANDABILLING_403002;					// COMMUDATAERROR	-403002	접수한 통신 데이터 패킷 암호화 에러	encrypt error
	// POINTER insert			
	case -404001: return ERROR_SHANDABILLING_404001;					// POINTERNULL	-404001	빈 pointer 삽입(pointer가 필요한 곳에서 빈 pointer를 접수할 수 없음)	point null
	// 데이터 encode/decode			
	case -405001: return ERROR_SHANDABILLING_405001;					// ENCRYPTERROR	-405001	암호화 에러	encrypt error
	case -405002: return ERROR_SHANDABILLING_405002;					// DECRYPTERROR	-405002	암호해제 에러	decrypt error
	// 설정정보
	case -406001: return ERROR_SHANDABILLING_406001;					// PARAMPATHERROR	-406001	cache 혹은 annular 파일 경로 에러	path error
	case -406002: return ERROR_SHANDABILLING_406002;					// CONFLOADFAILED	-406002	설정 파일 로딩 실패	fail to load ini
	case -406003: return ERROR_SHANDABILLING_406003;					// CONFGETFAILED	-406003	설정 서버에서 설정 획득 실패	fail to get ini
	case -406004: return ERROR_SHANDABILLING_406004;					// RELOADFAILED	-406004	로딩 실패	fail to reload
	// 설정서버 접속 정보			
	case -407001: return ERROR_SHANDABILLING_407001;					// ERR_MSG_TYPE	-407001	요청 메시지 중 메시지 타입 에러	message type error
	case -407002: return ERROR_SHANDABILLING_407002;					// IP_NOT_EXIST	-407002	요청 메시지 중 hostIP가 없음	hostip not exist
	case -407003: return ERROR_SHANDABILLING_407003;					// ID_NOT_EXIST	-407003	요청 메시지 중 hostId가 없음	hostid not exist
	case -407004: return ERROR_SHANDABILLING_407004;					// SYS_FLAG_INVALID	-407004	OS flag 에러	system flag error
	case -407005: return ERROR_SHANDABILLING_407005;					// MSG_LEN_ERR	-407005	요청 메시지 길이 에러	message length error
	case -407006: return ERROR_SHANDABILLING_407006;					// CANNOTCONNECTDB	-407006	DB 연결 실패	db connect error
	case -407099: return ERROR_SHANDABILLING_407099;					// UNKNOWNED_ERR	-407099	기타 에러	other error
	// 기타			
	case -499999: return ERROR_SHANDABILLING_499999;					// UNDECLARE_ERROR	-499999	기타 에러	other error

	default:
		return ERROR_GENERIC_UNKNOWNERROR;	// 에러번호 지정 필요 !!!
	}

	return ERROR_DB;
}

#endif	// #if defined(_CH) || defined(_EU)

