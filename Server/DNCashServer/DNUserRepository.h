#pragma once

#if defined (PRE_ADD_REJECT_DUPLICATED_CASH_REQUEST)
#if defined (_WIN64)
typedef LONGLONG USER_REQUEST;
#else
typedef LONG USER_REQUEST;
#endif // #if defined (_WIN64)

const USER_REQUEST CASH_NO_REQUESTING = 0;
const USER_REQUEST CASH_REQUESTING = 1;

void ReleaseExchange(USER_REQUEST volatile* pDestination);

class CRequestReleaser
{
public:
	CRequestReleaser(USER_REQUEST& requesting)
		: m_Requesting(requesting)
	{
		_ASSERT(m_Requesting == CASH_REQUESTING);
		m_Lock = false;
	}

	~CRequestReleaser()
	{
		if (!m_Lock)
			ReleaseExchange(&m_Requesting);
	}

public:
	void Lock() { m_Lock = true; }

private:
	USER_REQUEST& m_Requesting;
	bool m_Lock;
};
#endif // #if defined (PRE_ADD_REJECT_DUPLICATED_CASH_REQUEST)

// village, game으로 보낼 데이터들 billing갔다올 사이에 잠시 저장하는 저장소라고나 할까?;;
class CDNSQLWorld;
class CDNSQLMembership;
class CDNConnection;
class CDNUserRepository
{
public:
	struct TBuyItem
	{
		TCashBuyItem BuyItem;

		int nReservePetal;
		int nPrice;
		int nLimitCount;
#if defined( PRE_ADD_NEW_MONEY_SEED )
		int nSeedPoint;
#endif

		int nProductNo;
		INT64 biPurchaseOrderDetailID;	// 우리꺼 상품별 ID
	};

	struct TGiftItem
	{
		TCashGiftItem GiftItem;
		INT64 biPurchaseOrderID;	// 우리꺼
		int nReservePetal;
		int nPrice;
		int nLimitCount;

		// Nexon
		int nProductNo;

		int nOrderStep;		// eSndaOrderStep, gash
#if defined( PRE_ADD_NEW_MONEY_SEED )
		int nSeedPoint;
#endif
	};

	struct TVIPItem
	{
		int nItemSN;
		INT64 biPurchaseOrderID;	// 우리꺼
		int nPrice;
		int nLimitCount;

		int nOrderStep;		// eSndaOrderStep
	};

	// 샨다용
#if defined(_CH) || defined(_EU)
	struct TBuyItemPart
	{
		TBuyItem *pPartItem[MAX_GOOD_LEN];
		INT64 biPurchaseOrderID;	// 우리꺼
		int nOrderStep;		// eSndaOrderStep
		char context_id[MAX_ORDERID_LEN+1];		// 샨다 문맥 ID (주문 당 한번, GetNewId() 를 사용하여 생성)		// 개별 상품	// 각 AccountLock / AccountUnlock Request 시 한 쌍으로 유지되어야 함.
	};

	struct TGiftItemPart
	{
		TGiftItem *pPartItem[MAX_GOOD_LEN];
		int nOrderStep;		// eSndaOrderStep
		char context_id[MAX_ORDERID_LEN+1];		// 샨다 문맥 ID (주문 당 한번, GetNewId() 를 사용하여 생성)		// 개별 상품	// 각 AccountLock / AccountUnlock Request 시 한 쌍으로 유지되어야 함.
	};
#endif	// #if defined(_CH) || defined(_EU)

private:
	UINT m_nAccountDBID;
	char m_szAccountName[IDLENMAX];
	WCHAR m_wszAccountName[IDLENMAX];
	char m_szCharacterName[NAMELENMAX];
	WCHAR m_wszCharacterName[NAMELENMAX];
	INT64 m_biCharacterDBID;

	CDNConnection *m_pConnection;
	int m_nThreadID;
	char m_cWorldSetID;
	int m_nMapID;
	int m_nChannelID;

	int m_nSubCommand;

	// Nexon
	UINT m_nNexonSN;
	char m_cPaymentRules;	// Cash::PaymentRules

	// Shanda
#if defined(_CH) || defined(_EU)
	char m_sess_id[MAX_SESSID_LEN+1];		// 샨다 세션 ID (세션 당 한번, GetNewId() 를 사용하여 생성)		// 각 사용자 별 1회의 로그인 과정에 해당 필드 내용이 일치하도록 해야 함.
	char m_context_id[MAX_ORDERID_LEN+1];	// 샨다 문맥 ID (주문 당 한번, GetNewId() 를 사용하여 생성)		// 패키지 상품/선물		// 각 AccountLock / AccountUnlock Request 시 한 쌍으로 유지되어야 함.
	int m_nOrderStep;	// eSndaOrderStep / eGashOrderStep
#endif	// #if defined(_CH) || defined(_EU)

	int m_nPickupCount;

	bool m_bOpen;	
#if defined( PRE_ADD_NEW_MONEY_SEED )	
	INT64 m_nSeedPoint;
#endif
	bool m_bPCBang;
	char m_cBuyCartType;	// 구매 카트 타입
	char m_cGiftCartType;	// 선물 카트 타입

	int m_nPackageSN;

	INT64 m_biPurchaseOrderID;			// 우리꺼
#if defined(PRE_ADD_CASH_REFUND)
	INT64 m_biPurchaseOrderDetailID;	// 우리꺼
#endif
	INT64 m_biCouponOrderID;	// 우리꺼
	int m_nCouponSN;

	TCashItemBase m_ProductPackageList[PACKAGEITEMMAX];
	TCashPackageGiftItem m_GiftPackageList[PACKAGEITEMMAX];

#if defined(PRE_ADD_CASH_REFUND)
	bool m_bCashMoveInven;
#endif
#if defined(PRE_ADD_SALE_COUPON)
	INT64 m_biSaleCouponSerial;
#endif
#if defined(PRE_ADD_CADGE_CASH)
	int m_nMailDBID;
#endif	// #if defined(PRE_ADD_CADGE_CASH)

public:
	char m_cCurProductCount;

	char m_cProductCount;
	std::vector<TBuyItem> m_VecBuyItemList;
	std::vector<TGiftItem> m_VecGiftItemList;

#if defined(_CH) || defined(_EU)
	int m_nPartCount;
	std::vector<TBuyItemPart> m_VecBuyItemPartList;	// 중국은 5개씩 묶어서 보내야해서 하나 더 만든다. (머 이러냐..)
	std::vector<TGiftItemPart> m_VecGiftItemPartList;	// 중국은 5개씩 묶어서 보내야해서 하나 더 만든다. (머 이러냐..)
#endif	// _CH

#if defined(PRE_ADD_CASH_REFUND)
	TPaymentItemInfoEx m_PaymentItem[PREVIEWCARTLISTMAX];	
	TPaymentPackageItemInfoEx m_PaymentPackageItem;
	char m_cRefundType;
#endif

	char m_cAge;
	UINT m_nIp;
	char m_szIp[IPLENMAX];
	WCHAR m_wszIp[IPLENMAX];

	WCHAR m_wszReceiverCharacterName[NAMELENMAX];
	char m_szReceiverAccountName[IDLENMAX];
	WCHAR m_wszMessage[GIFTMESSAGEMAX];

	WCHAR m_wszCoupon[COUPONMAX];
	char m_szCoupon[COUPONMAX]; // 대만, 싱가폴은 char형으로 변환해서 저장

	int m_nTotalPrice;
	int m_nUsedPaidCash;

	TVIPItem m_VIP;
	bool m_bAutoPay;
	bool m_bServer;

#if defined (PRE_ADD_REJECT_DUPLICATED_CASH_REQUEST)
	USER_REQUEST m_Requesting;
#endif // #if defined (PRE_ADD_REJECT_DUPLICATED_CASH_REQUEST)

private:
	void Clear();

public:
	CDNUserRepository(void);
	~CDNUserRepository(void);

	void SetAccountDBID(UINT nAccountDBID) { m_nAccountDBID = nAccountDBID; }
	UINT GetAccountDBID() { return m_nAccountDBID; }
	void SetAccountName(char *pName) { _strcpy(m_szAccountName, _countof(m_szAccountName), pName, (int)strlen(pName)); }
	char *GetAccountName() { return m_szAccountName; }
	WCHAR *GetAccountNameW() { return m_wszAccountName; }
	char *GetCharacterName() { return m_szCharacterName; }
	WCHAR *GetCharacterNameW() { return m_wszCharacterName; }
	INT64 GetCharacterDBID() { return m_biCharacterDBID; }
	char GetWorldID() { return m_cWorldSetID; }
	int GetMapID() { return m_nMapID; }
	int GetChannelID() { return m_nChannelID; }
	void SetReceiverAccountName();
	char *GetReceiverAccountName() { return m_szReceiverAccountName; }
	WCHAR *GetReceiverCharacterNameW() { return m_wszReceiverCharacterName; }

	char *GetCoupon() { return m_szCoupon; }
	WCHAR *GetCouponW() { return m_wszCoupon; }

	CDNConnection * GetConnection() { return m_pConnection; }
	int GetThreadID() { return m_nThreadID; }

	UINT GetNexonSN() { return m_nNexonSN; }
	char GetPaymentRules() { return m_cPaymentRules; }

#if defined(_CH) || defined(_EU)
	char* SetSessId() { return m_sess_id; }
	const char* GetSessId() const { return m_sess_id; }
	int GetSessIdCnt() const { return(static_cast<int>(_countof(m_sess_id))); }
	void SetContextId(char *pContext) { _strcpy(m_context_id, _countof(m_context_id), pContext, (int)strlen(pContext)); }	// pRes->order_id 는 AccountLock / AccountUnlock 시 마다 변하므로 대신 pRes->context_id 를 사용
	const char* GetContextId() const { return m_context_id; }
	int GetContextIdCnt() const { return(static_cast<int>(_countof(m_context_id))); }
	void SetOrderStep(int nOrderStep) { m_nOrderStep = nOrderStep; }	
	int GetOrderStep() const { return m_nOrderStep; }

	CDNUserRepository::TBuyItemPart *GetBuyItemPart();	// 현재 pickupcount에 해당하는 데이터 찾기
	CDNUserRepository::TGiftItemPart *GetGiftItemPart();	// 현재 pickupcount에 해당하는 데이터 찾기

	int CheckSessID();
#endif	// #if defined(_CH) || defined(_EU)

	bool IsOpen() { return m_bOpen; }	
	bool IsPCBang() { return m_bPCBang; }
	char GetBuyCartType() { return m_cBuyCartType; }	// 구매 카트 타입
	int GetPackageSN() { return m_nPackageSN; }

	void SetPurchaseOrderID(INT64 biPurchaseOrderID) { m_biPurchaseOrderID = biPurchaseOrderID; }
	INT64 GetPurchaseOrderID() const { return m_biPurchaseOrderID; }

	int GetSubCommand() { return m_nSubCommand; }
	void SetPurchaseOrderDetailID(const char* pPurchaseOrderDetailIDs);

	int GetPetalBalance();

	CDNSQLMembership *GetMembershipDB();
	CDNSQLWorld *GetWorldDB();

	bool SetBuyItem(TCashBuyItem &Item);
	void SetBuyItemPartByShanda(int nStartIndex, int nEndIndex, INT64 biOrderID, const char* pPurchaseOrderDetailIDs);
	CDNUserRepository::TBuyItem *GetBuyItem(int nIndex);
	bool SetGiftItem(TCashGiftItem &Item);
	void SetGiftItemPartByShanda();
	void SetGiftItemPurchaseOrderID(int nIndex, INT64 biPurchaseOrderID);
	CDNUserRepository::TGiftItem *GetGiftItem(int nIndex);

	void SetBalanceInquiry(int nThreadID, CDNConnection *pConnection, TQCashBalanceInquiry *pCash);
	bool SetBuy(int nThreadID, CDNConnection *pConnection, TQCashBuy *pCash);
	void SetPackageBuy(int nThreadID, CDNConnection *pConnection, TQCashPackageBuy *pCash);
	bool SetGift(int nThreadID, CDNConnection *pConnection, TQCashGift *pCash);
	void SetPackageGift(int nThreadID, CDNConnection *pConnection, TQCashPackageGift *pCash);
	void SetCoupon(int nThreadID, CDNConnection *pConnection, TQCashCoupon *pCash);
	void SetVIPBuy(int nThreadID, CDNConnection *pConnection, TQCashVIPBuy *pCash);
	void SetVIPGift(int nThreadID, CDNConnection *pConnection, TQCashVIPGift *pCash);
	void SetRefund(int nThreadID, CDNConnection *pConnection, TQCashRefund* pCash);

	int OnBalanceInquiry(int nThreadID, CDNConnection *pConnection, TQCashBalanceInquiry *pCash);
	int OnBuy(int nThreadID, CDNConnection *pConnection, TQCashBuy *pCash);
	int OnPackageBuy(int nThreadID, CDNConnection *pConnection, TQCashPackageBuy *pCash);
	int OnGift(int nThreadID, CDNConnection *pConnection, TQCashGift *pCash);
	int OnPackageGift(int nThreadID, CDNConnection *pConnection, TQCashPackageGift *pCash);
	int OnCoupon(int nThreadID, CDNConnection *pConnection, TQCashCoupon *pCash);
	int OnVIPBuy(int nThreadID, CDNConnection *pConnection, TQCashVIPBuy *pCash);
	int OnVIPGift(int nThreadID, CDNConnection *pConnection, TQCashVIPGift *pCash);
	void OnRefund(int nThreadID, CDNConnection *pConnection, TQCashRefund* pCash);
	void OnMoveCashInven(int nThreadID, CDNConnection *pConnection, TQCashMoveCashInven* pCash);
	void OnPackageMoveCashInven(int nThreadID, CDNConnection *pConnection, TQCashPackageMoveCashInven* pCash);

	int CalcPackageProcess();

	bool OnRecvBillingCoupon(int nBillingResult, int nResult, int nItemSN, UINT nOrderNo);

	// 한국, 미국 전용
	void OnRecvBillingPurchase(int nResult, int nBillingResult, UINT nOrderNo, char *pNexonOrderID);	// purchase item 응답으로 orderno를 받는거 세팅해준다
	// 기타 국가..(중국, 일본 제외)
	void OnRecvBillingBuyItem(int nBillingResult, UINT nOrderNo=0, bool bCart=false); // 장바구니 기능 On,Off(Off면 여러번 보냄)

#if defined(_CH) || defined(_EU)
	int GetTotalPetalBuyPartList();
	void SetSuccessBuyPartList();
	void SetOrderStepBuyPartList(int nOrderStep);
	void SetSuccessGiftPartList();
	void SetOrderStepGiftPartList(int nOrderStep);

	void OnRecvAccountLock(int nBillingResult, char *pBillingContextID);
	void OnRecvAccountUnlock(int nBillingResult, char *pBillingContextID, char *pBillingOrderID, int nBalance);
#endif	// #if defined(_CH) || defined(_EU)

	// Connection으로 Send하기..
	void SendBalanceInquiry(int nResult, int nCashBalance, int nNotRefundableBalance = 0);
#ifdef PRE_ADD_LIMITED_CASHITEM
	void SendBuyGift(int nResult, bool bUpdateLimitedCount = false);
#else		//#ifdef PRE_ADD_LIMITED_CASHITEM
	void SendBuyGift(int nResult);
#endif		//#ifdef PRE_ADD_LIMITED_CASHITEM
#if defined(_TW)
	void SendCoupon(int nResult, bool bGashSend=true);
#else
	void SendCoupon(int nResult);
#endif // #if defined(_TW)
	void SendVIPBuyGift(int nResult, int nCash);

#if defined(PRE_ADD_CASH_REFUND)
	void SendCashRefund(int nResult);
	void OnRecvBillingRefund(int nResult);
	inline bool GetCashMoveInven() { return m_bCashMoveInven; };
	inline void SetCashMoveInven(bool bCashMoveInven) { m_bCashMoveInven = bCashMoveInven; };		
#endif

	int PurchaseItemByPetal();
#if defined( PRE_ADD_NEW_MONEY_SEED )
	int PurchaseItemBySeed();
#endif
	int AddPurchaseOrderByCash(int nIndex);
	int PurchaseBuyItem();
	int PurchaseGiftItem();

	// 안에서 CashItemDBQuery 처리.. 한번 더 감쌌음
	int CashItemDBProcess(int nWorldSetID, int nAccountDBID, INT64 biCharacterDBID, int nMapID, int nChannelID, char cPaymentRules, TCashItemBase &BuyItem, int nPrice, 
		INT64 biPurchaseOrderID, int nAddMaterializedItemCode, INT64 biSenderCharacterDBID = 0, bool bGift = false, char cPayMethodCode = DBDNWorldDef::PayMethodCode::Cash);

	bool CheckSaleItemSN(int nItemSN);
#if defined(PRE_MOD_SELECT_CHAR)
	int CheckCharacterSlotCount(int nAccountDBID, int nAddCount);
#else	// #if defined(PRE_MOD_SELECT_CHAR)
	int CheckCharacterSlotCount(INT64 biCharacterDBID, int nAddCount);
#endif	// #if defined(PRE_MOD_SELECT_CHAR)

	void AppendItemString(int nStartIndex, int nEndIndex, std::vector<std::string> &VecItemString);
	void SetUsedPaidCash(LPCWSTR pReservedFields);

#if defined( PRE_ADD_NEW_MONEY_SEED )
	INT64 GetSeedPoint();		
	int AddSeedPoint( int nSeedPoint );	
	int UseSeedPoint();		
#endif
};


