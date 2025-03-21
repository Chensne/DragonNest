#pragma once

/*
	P.S.>
		- 샨다 주문 ID 는 AccountLock / AccountUnlock 시 order_id 는 각각 변하므로 이에 일정한 context_id 를 사용함
		- 
*/

#if defined(_CH) || defined(_EU)

#include "DNUserRepository.h"


enum eSndaOrderStep		// SNDA 결제 과정
{
	SNDAORDERSTEP_NONE,		// 준비
	SNDAORDERSTEP_WORK,		// 진행
	SNDAORDERSTEP_SUCCESS,	// 성공
	SNDAORDERSTEP_FAIL,		// 실패
};

class CDNBillingConnectionCH
{
private:
	static void CallbackAccountAuthenRes(GSBsipAccountAuthenResDef*);
	static void CallbackAccountLockRes(GSBsipAccountLockResDef*);
	static void CallbackAccountUnlockRes(GSBsipAccountUnlockResDef*);

public:
	struct TGoodsInfo
	{
		int nItemSN;
		int nPrice;
	};

public:
	CDNBillingConnectionCH(void);
	~CDNBillingConnectionCH(void);

	bool Init();
	void Final();
	bool IsInitialize() const { return m_bInitialize; }

	int OnBalanceInquiry(CDNUserRepository *pUser);
	int OnBuy(CDNUserRepository *pUser);
	int OnPackageBuy(CDNUserRepository *pUser);
	int OnGift(CDNUserRepository *pUser);
	int OnPackageGift(CDNUserRepository *pUser);
	int OnCoupon(CDNUserRepository *pUser);

public:
	static int SendCheckBalance(CDNUserRepository *pUser);	// 잔액 조회 요청

	static int SendAccountLock(CDNUserRepository *pUser, TGoodsInfo *GoodsList, int nGoodsCount, bool bGift);
	static int SendAccountUnlock(CDNUserRepository *pUser, bool bConfirm, const char *pContextId, TGoodsInfo *GoodsList, int nGoodsCount, bool bGift);

	void GetBuyGoodsInfo(CDNUserRepository::TBuyItemPart *pBuy, TGoodsInfo *GoodsList, int &nTotalCount);
	void GetGiftGoodsInfo(CDNUserRepository::TGiftItemPart *pGift, TGoodsInfo *GoodsList, int &nTotalCount);

	static int ConvertResult(int nBillingResult);

public:
	bool m_bInitialize;
};

extern CDNBillingConnectionCH *g_pBillingConnection;

#endif	// #if defined(_CH) || defined(_EU)
