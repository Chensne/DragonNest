#pragma once

#pragma pack(push, 1)

//-------------------------------------------------
//	Cash Packet (Nexon)
//-------------------------------------------------
struct TBillingHeader
{
	BYTE cPacketID;		// 0xAF 고정
	UINT nLength;		// 
};

#if (defined(_KR) || defined(_US)) && defined(_CASHSERVER)
struct TBonusProductInfo
{
	UINT nProductNo;	// 보너스 상품 번호
	char szExtend[10];	// 보너스값
};

struct TProductInquiry
{
	int nCommodityItemSN;	// CashCommodity 상품번호
	int nProductNo;			// 상품번호
	int nRelationProductNo;	// 연관상품 아닐경우 0 대표상품일 경우 product no와 동일
	short wProductExpire;		// 사용기간 (일)
	short wProductPieces;		// 개별 상품수
	char szProductID[100];		// 게임서버에서 사용하는 DBID
	char szProductGUID[32];		// NISMS에서 부여되는 상품 GUID 테스트, 상용환경 동일한 값
	int nPaymentType;			// 지불수단
	char szProductType[2];		// 상품타입
	int nSalePrice;			// 상품판매금액
	int nCategoryNo;			// 상품이 속한 카테고리 번호
	short wProductStatus;		// 상품 상태 번호
	char cBonusProductCount;	// 보너스상품 총 카운트
	std::vector<TBonusProductInfo> vBonusProduct;
};

struct TProductItem
{
	int nProductNo;			// 상품번호
	short wOrderQuantity;	// 주문수량
	char szExtend[10];		// 확장값
};

struct TPickupItemSub
{
	UINT nProductNo;		// 상품번호
	char szProductName[100];// 상품이름
	char szProductID[100];	// 게임서버에서 사용하는 DBID
	USHORT wProductExpire;		// 사용기간 (일)
	USHORT wProductPieces;		// 개별 상품수
	char szProductAttribute[64][5];	// 상품 속성값
};

struct TPickupItemBonus
{
	UINT nBonusProduct;		// 보너스 상품번호
	UINT nBonusAmount;		// 보너스 금액
};

struct TInventoryInquiryProduct
{
	int nOrderNo;
	int nProductNo;
	char cProductKind;
	char szProductName[100];
	char szProductID[100];
	short wProductExpire;
	short wProductPieces;
	short wOrderQuantity;
	short wRemainProductQuantity;
	char cPresent;
	char cRead;
	char cSenderServerNo;
	char szSenderGameID[64];
	char szSenderPresentMessage[100];
	char szProductAttribute[64][5];
	char szExtend[10];
};

#endif

/*-------------------------------------------------
	Cash Packet

 AccountDBID를 꼭 앞에 써줘야함. 저걸로 유저 찾음
-------------------------------------------------*/
struct TCashHeader
{
	UINT nAccountDBID;
};

// CASH_BALANCEINQUIRY
struct TQCashBalanceInquiry : public TCashHeader
{
	char szAccountName[IDLENMAX];
	bool bOpen;
	bool bServer;
	UINT nIp;
#if defined(_KR) || defined(_US)
	UINT nNexonSN;
#endif	// _KR _CH
};

struct TACashBalanceInquiry : public TCashHeader
{
	int nResult;
	int nCashBalance;
	int nPetal;
	bool bOpen;
	bool bServer;
#if defined(_US)
	int nNotRefundableBalance;	// 
#endif	// _US
};

struct TCashItemBase
{
	int nItemSN;
	TItem CashItem;
	DWORD dwPartsColor1;		// 파츠 색깔
	DWORD dwPartsColor2;	// 파츠 색깔2
	TVehicleItem VehiclePart1;
	TVehicleItem VehiclePart2;
	int nPaidCashAmount;
};

struct TCashBuyItem: TCashItemBase
{
	char cCartIndex;
	bool bFail;
};

// CASH_BUY
struct TQCashBuyBase : public TCashHeader
{
	INT64 biCharacterDBID;
	char cWorldSetID;
	char szCharacterName[NAMELENMAX];
	char cAge;
	bool bPCBang;
	int nMapID;
	int nChannelID;
#if defined(PRE_ADD_CASH_REFUND)
	bool bMoveCashInven;	// 캐쉬인벤으로 바로 옮기기 플래그
#endif
	char cPaymentRules;	// Cash::PaymentRules
};

struct TQCashBuy: TQCashBuyBase
{
	char cCartType;		// 구매 카트 타입 (3,7: PREVIEWCARTLISTMAX, 나머지: CARTLISTMAX, 50 + eInstantCashShopBuyType: 간편결재)
	char cProductCount;
#if defined(PRE_ADD_SALE_COUPON)
	INT64 biSaleCouponSerial; // 세일쿠폰 사용
#endif
	TCashBuyItem BuyList[PREVIEWCARTLISTMAX];
};

struct TACashBuy : public TCashHeader
{
	int nResult;
	char cPaymentRules;	// Cash::PaymentRules
	int nPetalBalance;
	char cCartType;		// 구매 카트 타입 (3,7: PREVIEWCARTLISTMAX, 나머지: CARTLISTMAX, 50 + eInstantCashShopBuyType: 간편결재)
#if defined(PRE_ADD_CASH_REFUND)
	TPaymentItemInfoEx ItemList[PREVIEWCARTLISTMAX];	
#endif
#if defined(PRE_ADD_SALE_COUPON)
	INT64 biSaleCouponSerial;
#endif
#if defined( PRE_ADD_NEW_MONEY_SEED )
	INT64 nSeedPoint;
#endif
	char cProductCount;
	TCashBuyItem BuyList[PREVIEWCARTLISTMAX];
};

// CASH_PACKAGEBUY
struct TQCashPackageBuy: TQCashBuyBase
{
	int nPackageSN;		// 패키지 대표 SN	
#if defined(PRE_ADD_SALE_COUPON)
	INT64 biSaleCouponSerial; // 세일쿠폰 사용
#endif
	char cPackageCount;	// 패키지안에 상품들
	TCashItemBase BuyList[PACKAGEITEMMAX];
};

struct TACashPackageBuy : public TCashHeader
{
	int nResult;
	char cPaymentRules;	// Cash::PaymentRules	
	int nPetalBalance;
	int nPackageSN;		// 패키지 대표 SN
#if defined(PRE_ADD_CASH_REFUND)
	TPaymentPackageItemInfoEx PaymentItem;	
#endif
#if defined(PRE_ADD_SALE_COUPON)
	INT64 biSaleCouponSerial;
#endif
#if defined( PRE_ADD_NEW_MONEY_SEED )
	INT64 nSeedPoint;
#endif
	char cPackageCount;
	TCashItemBase BuyList[PACKAGEITEMMAX];
};

struct TCashGiftItem
{
	char cCartIndex;
	int nItemSN;
	int nItemID;
	char cItemOption;
	bool bFail;
};

struct TCashPackageGiftItem
{
	int nItemSN;
	int nItemID;
	char cItemOption;
};

struct TQCashGiftBase : public TCashHeader
{
	INT64 biCharacterDBID;
	char cWorldSetID;
	char szCharacterName[NAMELENMAX];
	char cAge;
	bool bPCBang;
	int nMapID;
	int nChannelID;
	WCHAR wszReceiverCharacterName[NAMELENMAX];
	WCHAR wszMessage[GIFTMESSAGEMAX];
#if defined(PRE_ADD_CADGE_CASH)
	int nMailDBID;
#endif	// #if defined(PRE_ADD_CADGE_CASH)
};

// CASH_GIFT
struct TQCashGift: TQCashGiftBase
{
	char cCartType;		// 구매 카트 타입 (3,7: PREVIEWCARTLISTMAX, 나머지: CARTLISTMAX, 50 + eInstantCashShopBuyType: 간편결재)
	char cProductCount;
	TCashGiftItem GiftList[PREVIEWCARTLISTMAX];
};

struct TACashGift : public TCashHeader
{
	UINT nReceiverAccountDBID;
	INT64 biReceiverCharacterDBID;
	int nReceiverGiftCount;
	int nResult;
	int nPetalBalance;
	char cCartType;		// 구매 카트 타입 (3,7: PREVIEWCARTLISTMAX, 나머지: CARTLISTMAX, 50 + eInstantCashShopBuyType: 간편결재)
#if defined(PRE_ADD_CADGE_CASH)
	int nMailDBID;
#endif	// #if defined(PRE_ADD_CADGE_CASH)
#if defined( PRE_ADD_NEW_MONEY_SEED )
	INT64 nSeedPoint;
#endif
	char cGiftCount;
	TCashGiftItem GiftList[PREVIEWCARTLISTMAX];
};

// CASH_PACKAGEGIFT
struct TQCashPackageGift: TQCashGiftBase
{
	int nPackageSN;		// 패키지 대표 SN
	char cPackageCount;	// 패키지안에 상품들
	TCashPackageGiftItem GiftList[PACKAGEITEMMAX];
};

struct TACashPackageGift : public TCashHeader
{
	UINT nReceiverAccountDBID;
	INT64 biReceiverCharacterDBID;
	int nReceiverGiftCount;
	int nResult;
	int nPetalBalance;
	int nPackageSN;		// 패키지 대표 SN
#if defined(PRE_ADD_CADGE_CASH)
	int nMailDBID;
#endif	// #if defined(PRE_ADD_CADGE_CASH)
#if defined( PRE_ADD_NEW_MONEY_SEED )
	INT64 nSeedPoint;
#endif
	char cPackageCount;
	TCashPackageGiftItem GiftList[PACKAGEITEMMAX];
};

// CASH_COUPON
struct TQCashCoupon : public TCashHeader
{
	INT64 biCharacterDBID;
	char cWorldSetID;
	char szCharacterName[NAMELENMAX];
	WCHAR wszCoupon[COUPONMAX];
	bool bPCBang;
};

struct TACashCoupon : public TCashHeader
{
	int nResult;
};

// CASH_DELUSER
struct TQCashDelUser : public TCashHeader
{
};


// CASH_VIPBUY,
struct TQCashVIPBuy : public TCashHeader
{
	INT64 biCharacterDBID;
	int nItemSN;
	char cWorldSetID;
	bool bPCBang;
	int nMapID;
	int nChannelID;
	bool bAutoPay;			// 0=수동결제, 1=자동결제
	bool bServer;			// 유저가 산건지 서버에서 보낸건지
};

struct TACashVIPBuy : public TCashHeader
{
	int nResult;
	int nPetal;
	int nCash;
	int nItemSN;
	int nVIPTotalPoint;
	__time64_t tVIPEndDate;
	bool bAutoPay;			// 0=수동결제, 1=자동결제
	bool bServer;			// 유저가 산건지 서버에서 보낸건지
#if defined( PRE_ADD_NEW_MONEY_SEED )
	INT64 nSeedPoint;
#endif
};

// CASH_VIPGIFT,
struct TQCashVIPGift : public TCashHeader
{
	INT64 biCharacterDBID;
	int nItemSN;
	char cWorldSetID;
	bool bPCBang;
	int nMapID;
	int nChannelID;
	WCHAR wszReceiverCharacterName[NAMELENMAX];
	WCHAR wszMessage[GIFTMESSAGEMAX];
};

struct TACashVIPGift : public TCashHeader
{
	int nResult;
	int nPetal;
	int nCash;
	int nItemSN;
	UINT nReceiverAccountDBID;
	INT64 biReceiverCharacterDBID;
	int nReceiverGiftCount;
#if defined( PRE_ADD_NEW_MONEY_SEED )
	INT64 nSeedPoint;
#endif
};

#ifdef PRE_ADD_LIMITED_CASHITEM
struct TQGetLimitedItemList
{
};

struct TAGetLimitedItemList
{
	BYTE cIsLast;
	BYTE cCount;
	LimitedCashItem::TLimitedQuantityCashItem Limited[LimitedCashItem::Common::DefaultPacketCount];
};

struct TAGetChangedLimitedItemList
{
	BYTE cCount;
	LimitedCashItem::TChangedLimitedQuantity Limited[LimitedCashItem::Common::DefaultPacketCount];
};

struct TQChangeLimitMax
{
	int nSN;
	int nLimitMax;
};
#endif		//#ifdef PRE_ADD_LIMITED_CASHITEM

// CASH_SALEABORTLIST
struct TASaleAbortList
{
	BYTE cCount;
	int nAbortList[SALEABORTLISTMAX];
};

// CASH_MOVE_CASHINVEN
struct TQCashMoveCashInven : public TCashHeader
{
	INT64 biCharacterDBID;
	char cWorldSetID;
	int nMapID;
	int nChannelID;
	TCashItemBase CashItem;
	TPaymentItemInfoEx PaymentItemInfo;
};

struct TACashMoveCashInven : public TCashHeader
{
	int nResult;
	int nTotalPetal;			// 페탈이 적립될 수 있다.(토탈 페탈)
#if defined( PRE_ADD_NEW_MONEY_SEED )
	int nTotalSeed;				// 시드 적립
#endif
	TCashItemBase CashItem;
	INT64 biPurchaseOrderDetailID;
};

// CASH_PACKAGE_MOVE_CASHINVEN
struct TQCashPackageMoveCashInven : public TCashHeader
{
	INT64 biCharacterDBID;
	char cWorldSetID;
	int nMapID;
	int nChannelID;
	TCashItemBase CashItemList[PACKAGEITEMMAX];
	TPaymentPackageItemInfoEx PaymentPackageItemInfo;
};

struct TACashPackageMoveCashInven : public TCashHeader
{
	int nResult;
	int nTotalPetal;			// 페탈이 적립될 수 있다.(토탈 페탈)	
#if defined( PRE_ADD_NEW_MONEY_SEED )
	int nTotalSeed;				// 시드 적립
#endif
	TCashItemBase CashItemList[PACKAGEITEMMAX];
	INT64 biPurchaseOrderDetailID;
};

// CASH_REFUNDCASH
struct TQCashRefund : public TCashHeader
{
	char cItemType;				// 1..일반, 2..Package
	char cWorldSetID;
	int nItemSN;	
	UINT uiOrderNo;				// Nexon
	INT64 biPurchaseOrderDetailID;	// 우리꺼
};

struct TACashRefund : public TCashHeader
{
	int nResult;
	char cItemType;
	INT64 biPurchaseOrderDetailID;	// 우리꺼
};



#pragma pack(pop)
