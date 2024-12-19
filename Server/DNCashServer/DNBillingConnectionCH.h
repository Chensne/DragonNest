#pragma once

/*
	P.S.>
		- ���� �ֹ� ID �� AccountLock / AccountUnlock �� order_id �� ���� ���ϹǷ� �̿� ������ context_id �� �����
		- 
*/

#if defined(_CH) || defined(_EU)

#include "DNUserRepository.h"


enum eSndaOrderStep		// SNDA ���� ����
{
	SNDAORDERSTEP_NONE,		// �غ�
	SNDAORDERSTEP_WORK,		// ����
	SNDAORDERSTEP_SUCCESS,	// ����
	SNDAORDERSTEP_FAIL,		// ����
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
	static int SendCheckBalance(CDNUserRepository *pUser);	// �ܾ� ��ȸ ��û

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
