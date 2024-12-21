#pragma once

#pragma pack(push, 1)

//-------------------------------------------------
//	Cash Packet (Nexon)
//-------------------------------------------------
struct TBillingHeader
{
	BYTE cPacketID;		// 0xAF ����
	UINT nLength;		// 
};

#if (defined(_KR) || defined(_US)) && defined(_CASHSERVER)
struct TBonusProductInfo
{
	UINT nProductNo;	// ���ʽ� ��ǰ ��ȣ
	char szExtend[10];	// ���ʽ���
};

struct TProductInquiry
{
	int nCommodityItemSN;	// CashCommodity ��ǰ��ȣ
	int nProductNo;			// ��ǰ��ȣ
	int nRelationProductNo;	// ������ǰ �ƴҰ�� 0 ��ǥ��ǰ�� ��� product no�� ����
	short wProductExpire;		// ���Ⱓ (��)
	short wProductPieces;		// ���� ��ǰ��
	char szProductID[100];		// ���Ӽ������� ����ϴ� DBID
	char szProductGUID[32];		// NISMS���� �ο��Ǵ� ��ǰ GUID �׽�Ʈ, ���ȯ�� ������ ��
	int nPaymentType;			// ���Ҽ���
	char szProductType[2];		// ��ǰŸ��
	int nSalePrice;			// ��ǰ�Ǹűݾ�
	int nCategoryNo;			// ��ǰ�� ���� ī�װ� ��ȣ
	short wProductStatus;		// ��ǰ ���� ��ȣ
	char cBonusProductCount;	// ���ʽ���ǰ �� ī��Ʈ
	std::vector<TBonusProductInfo> vBonusProduct;
};

struct TProductItem
{
	int nProductNo;			// ��ǰ��ȣ
	short wOrderQuantity;	// �ֹ�����
	char szExtend[10];		// Ȯ�尪
};

struct TPickupItemSub
{
	UINT nProductNo;		// ��ǰ��ȣ
	char szProductName[100];// ��ǰ�̸�
	char szProductID[100];	// ���Ӽ������� ����ϴ� DBID
	USHORT wProductExpire;		// ���Ⱓ (��)
	USHORT wProductPieces;		// ���� ��ǰ��
	char szProductAttribute[64][5];	// ��ǰ �Ӽ���
};

struct TPickupItemBonus
{
	UINT nBonusProduct;		// ���ʽ� ��ǰ��ȣ
	UINT nBonusAmount;		// ���ʽ� �ݾ�
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

 AccountDBID�� �� �տ� �������. ���ɷ� ���� ã��
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
	DWORD dwPartsColor1;		// ���� ����
	DWORD dwPartsColor2;	// ���� ����2
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
	bool bMoveCashInven;	// ĳ���κ����� �ٷ� �ű�� �÷���
#endif
	char cPaymentRules;	// Cash::PaymentRules
};

struct TQCashBuy: TQCashBuyBase
{
	char cCartType;		// ���� īƮ Ÿ�� (3,7: PREVIEWCARTLISTMAX, ������: CARTLISTMAX, 50 + eInstantCashShopBuyType: �������)
	char cProductCount;
#if defined(PRE_ADD_SALE_COUPON)
	INT64 biSaleCouponSerial; // �������� ���
#endif
	TCashBuyItem BuyList[PREVIEWCARTLISTMAX];
};

struct TACashBuy : public TCashHeader
{
	int nResult;
	char cPaymentRules;	// Cash::PaymentRules
	int nPetalBalance;
	char cCartType;		// ���� īƮ Ÿ�� (3,7: PREVIEWCARTLISTMAX, ������: CARTLISTMAX, 50 + eInstantCashShopBuyType: �������)
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
	int nPackageSN;		// ��Ű�� ��ǥ SN	
#if defined(PRE_ADD_SALE_COUPON)
	INT64 biSaleCouponSerial; // �������� ���
#endif
	char cPackageCount;	// ��Ű���ȿ� ��ǰ��
	TCashItemBase BuyList[PACKAGEITEMMAX];
};

struct TACashPackageBuy : public TCashHeader
{
	int nResult;
	char cPaymentRules;	// Cash::PaymentRules	
	int nPetalBalance;
	int nPackageSN;		// ��Ű�� ��ǥ SN
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
	char cCartType;		// ���� īƮ Ÿ�� (3,7: PREVIEWCARTLISTMAX, ������: CARTLISTMAX, 50 + eInstantCashShopBuyType: �������)
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
	char cCartType;		// ���� īƮ Ÿ�� (3,7: PREVIEWCARTLISTMAX, ������: CARTLISTMAX, 50 + eInstantCashShopBuyType: �������)
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
	int nPackageSN;		// ��Ű�� ��ǥ SN
	char cPackageCount;	// ��Ű���ȿ� ��ǰ��
	TCashPackageGiftItem GiftList[PACKAGEITEMMAX];
};

struct TACashPackageGift : public TCashHeader
{
	UINT nReceiverAccountDBID;
	INT64 biReceiverCharacterDBID;
	int nReceiverGiftCount;
	int nResult;
	int nPetalBalance;
	int nPackageSN;		// ��Ű�� ��ǥ SN
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
	bool bAutoPay;			// 0=��������, 1=�ڵ�����
	bool bServer;			// ������ ����� �������� ��������
};

struct TACashVIPBuy : public TCashHeader
{
	int nResult;
	int nPetal;
	int nCash;
	int nItemSN;
	int nVIPTotalPoint;
	__time64_t tVIPEndDate;
	bool bAutoPay;			// 0=��������, 1=�ڵ�����
	bool bServer;			// ������ ����� �������� ��������
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
	int nTotalPetal;			// ��Ż�� ������ �� �ִ�.(��Ż ��Ż)
#if defined( PRE_ADD_NEW_MONEY_SEED )
	int nTotalSeed;				// �õ� ����
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
	int nTotalPetal;			// ��Ż�� ������ �� �ִ�.(��Ż ��Ż)	
#if defined( PRE_ADD_NEW_MONEY_SEED )
	int nTotalSeed;				// �õ� ����
#endif
	TCashItemBase CashItemList[PACKAGEITEMMAX];
	INT64 biPurchaseOrderDetailID;
};

// CASH_REFUNDCASH
struct TQCashRefund : public TCashHeader
{
	char cItemType;				// 1..�Ϲ�, 2..Package
	char cWorldSetID;
	int nItemSN;	
	UINT uiOrderNo;				// Nexon
	INT64 biPurchaseOrderDetailID;	// �츮��
};

struct TACashRefund : public TCashHeader
{
	int nResult;
	char cItemType;
	INT64 biPurchaseOrderDetailID;	// �츮��
};



#pragma pack(pop)
