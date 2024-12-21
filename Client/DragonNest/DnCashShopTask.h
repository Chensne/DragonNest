#pragma once

#pragma warning(disable : 4995)

#include "Task.h"
#include "MessageListener.h"
#include "DNProtocol.h"
#include "DNPacket.h"
#include "DnCashShopDefine.h"
#ifdef PRE_MOD_BROWSER
#else
#include <exdisp.h>
#include "DnCashShopIESink.h"

extern CComModule _Module;
#endif // PRE_MOD_BROWSER

#ifdef PRE_MOD_CASHSHOP_NOETERNITY_TOOLTIP
#include "DnCustomControlCommon.h"
#endif


#define PET_CLASS_ID	100

#ifdef PRE_FIX_NOMOVE_BROWSER
LRESULT CALLBACK ChildWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
#endif

#ifdef PRE_ADD_SALE_COUPON
#define MAX_COUPON_SALEITEM		20
#endif // PRE_ADD_SALE_COUPON


class CDnItem;
class CDnCashShopDlg;
class CDnCashShopTask : public CTask, public CTaskListener, public CSingleton<CDnCashShopTask>
{
public:
	enum ePreviewType
	{
		ePreview_Normal,
		ePreview_Package
	};

	enum eRecvGiftType
	{
		eRGT_MIN = 4,
		eRGT_COUPON = eRGT_MIN,
		eRGT_GM,
		eRGT_QUEST,
		eRGT_MISSION,
		eRGT_LEVELUP,
		eRGT_VIP,
		eRGT_TIMEEVENT = 15,
		eRGT_MAX
	};

	struct SPackagePreviewUnit
	{
		ePreviewType previewType;
		CASHITEM_SN	packageSN;
	};

	CDnCashShopTask();
	virtual ~CDnCashShopTask();

	bool Initialize();
#ifdef PRE_MOD_BROWSER
#else
	void InitializeChargeBrowser();
#endif
	void Finalize();
	virtual void Process(LOCAL_TIME LocalTime, float fDelta);

	bool IsOpenCashShop()		{ return m_bOpened; };
#ifdef PRE_MOD_CASHSHOP_NOETERNITY_TOOLTIP
	bool IsUsingCashShopTooltip(CASHITEM_SN sn, ITEM_SLOT_TYPE slotType) const;
#else
	bool IsUsingCashShopTooltip(CASHITEM_SN sn) const;
#endif

	void SetCashShopDlg(CDnCashShopDlg* pDlg) { m_pCashShopDlg = pDlg; }
	CDnCashShopDlg* GetCashShopDlg() { return m_pCashShopDlg; }

	void PutItemIntoCart(const SCashShopItemInfo& itemInfo);

	void RequestCashShopBalanceInquiry();
	void RequestCashShopOpen();
	void RequestCashShopClose();
	void RequestCashShopGiftReceiverInfo(const wchar_t *pReceiverName);
	void RequestCashShopRecvGiftBasicInfo();
	void RequestCashShopRecvItemByCoupon(const WCHAR* couponNum);
	void RequestCashShopRecvGift(INT64 giftDBID, char cEmoticon, const std::wstring& memo, char cPayMethodCode);
	void RequestCashShopGetChargeTime();
#if defined(PRE_ADD_GIFT_RETURN)
	void RequestCashShopGiftReturn(INT64 giftDBID);
#endif
#ifdef PRE_ADD_CADGE_CASH
	void RequestCashShopSendGiftItemNow(const std::wstring& receiverName, const std::wstring& memo, int nCadgeMailID);
	void RequestCashShopSendGiftCart(const std::wstring& receiverName, const std::wstring& memo, int nCadgeMailID);
#else // PRE_ADD_CADGE_CASH
	void RequestCashShopSendGiftItemNow(const std::wstring& receiverName, const std::wstring& memo);
	void RequestCashShopSendGiftCart(const std::wstring& receiverName, const std::wstring& memo);
#endif // PRE_ADD_CADGE_CASH
	void RequestCashShopSendGiftPreviewCart(const std::wstring& receiverName, const std::wstring& memo);
#ifdef PRE_ADD_CASHSHOP_REFUND_CL
	void RequestCashShopBuyCart(bool bBuyReserveMoney, bool bDirectMoveCashInven);
	void RequestCashShopBuyPreviewCart(bool bBuyReserveMoney, bool bDirectMoveCashInven);
	void RequestCashShopBuyItemNow(bool bBuyReserveMoney, bool bDirectMoveCashInven);
#else
	void RequestCashShopBuyCart(bool bBuyReserveMoney);
	void RequestCashShopBuyPreviewCart(bool bBuyReserveMoney);
	void RequestCashShopBuyItemNow(bool bBuyReserveMoney);
#endif
#ifdef PRE_ADD_CASHSHOP_REFUND_CL
	void RequestCashShopWithdrawFromRefundInven(INT64 itemDBID, bool bPackage);
	void RequestCashShopRefund(INT64 itemDBID, bool bPackage);
#endif
#ifdef PRE_ADD_STEAMWORKS
	void OnSteamOverlayDeActivated();
#endif // PRE_ADD_STEAMWORKS
	void OnRecvCashShopOpen(SCCashShopOpen* pPacket);
	void OnRecvCashShopClose(SCCashShopClose* pPacket);
	void OnRecvCashShopGiftReceiverInfo(SCCashShopCheckReceiver* pReceiverInfo);
	void OnRecvCashShopRecvGiftBasicInfo(SCCashShopGiftList* pPacket);
	void OnRecvCashShopRecvItemByCoupon(SCCashShopCoupon* pData);
	void OnRecvCashShopBuy(SCCashShopBuy* pPacket);
	void OnRecvCashShopPackageBuy(SCCashShopPackageBuy* pData);
	void OnRecvCashShopBalanceInquiry(SCCashShopBalanceInquiry* pData);
	void OnRecvCashShopRecvGift(SCCashShopReceiveGift* pData);
	void OnRecvCashShopRecvGiftAll(SCCashShopReceiveGiftAll* pData);
	void OnRecvCashShopSendGift(SCCashShopGift* pPacket);
	void OnRecvCashShopSendPackageGift(SCCashShopPackageGift* pData);
	void OnRecvGiftNotify(SCNotifyGift* pPacket);
#ifdef PRE_ADD_VIP
	void OnRecvCashShopVIPBuy(SCVIPBuy* pPacket);
	void OnRecvCashShopVIPGift(SCVIPGift* pPacket);
	bool OnRecvCashShopVIPInfo(SCVIPInfo* pPacket);
#endif // PRE_ADD_VIP
	void OnRecvSaleAbortList(SCSaleAbortList* pPacket);

#if defined(PRE_ADD_GIFT_RETURN)
	void OnRecvCashShopGiftReturn(SCCashShopGiftReturn* pData);
#endif
	void OnRecvCashShopServerTimeForCharge(SCCashShopChargeTime* pData);
#ifdef PRE_ADD_LIMITED_CASHITEM
	void RequestCashShopRefresh();
	void OnRecvCashShopLimitedItemInfo(SCQuantityLimitedItem * pPacket);
	void OnRecvCashShopChangedLimitedItemInfo(SCChangedQuantityLimitedItem * pPacket);
#endif		//#ifdef PRE_ADD_LIMITED_CASHITEM

#ifdef PRE_ADD_CASHSHOP_REFUND_CL
	void OnRecvCashShopPaymentList(SCPaymentList* pData);
	void OnRecvCashShopPaymentPackageList(SCPaymentPackageList* pData);
	void OnRecvCashShopWidthdrawFromRefundInven(SCMoveCashInven* pData);
	void OnRecvCashShopRefund(SCCashRefund* pData);

	const SRefundCashItemInfo* GetRefundItemInfo(INT64 dbid) const;
	bool AddItemToRefundInven(const TPaymentItemInfo& packetInfo, int invenIdx);
	bool AddPackageItemToRefundInven(const TPaymentPackageItemInfo& packetInfo, int invenIdx);

	bool IsRefundable(CASHITEM_SN sn) const;
	bool IsAllCartItemRefundable(eCashUnitType type) const;
#endif

#ifdef PRE_MOD_CASHSHOP_NOETERNITY_TOOLTIP
	bool IsOpenCashShopRecvGiftDlg() const;
#endif

	bool IsRecvGiftReplyAble(int payMethodCode) const;

	void OnOpenCashShop();

	void ClearCartItems(bool bWithPreviewCart);

	//void OnRecvCashShopSendGift();		// todo : 선물하고 응답 필요.

	const SCashShopItemInfo* GetItemInfo(CASHITEM_SN sn) const;
	SCashShopItemInfo* GetItemInfo(CASHITEM_SN sn);
	SCashShopCartItemInfo* GetCartItemInfo(int cartSlotIndex);

	const CS_INFO_LIST*	GetSubCatItemList(eCashShopSubCatType type) const;
	CS_INFO_LIST* GetSubCatItemList(eCashShopSubCatType type);
	const SCashShopItemInfo* GetSubCatItem(eCashShopSubCatType type, int index) const;
	int GetSubCatItemListSize(eCashShopSubCatType type) const;

	const RECV_GIFT_LIST& GetRecvGiftList() const { return m_RecvGiftInfoList; }
	const SCashShopRecvGiftBasicInfo* GetRecvGiftInfo(INT64 giftDBID) const;
	const CART_ITEM_LIST& GetCartList() const { return m_CartList; }
	const CART_ITEM_LIST* GetPreviewCartList(int classId) const;
	CART_ITEM_LIST* GetPreviewCartList(int classId);
	const CART_ITEM_LIST* GetBuyPreviewCartList() const;
	CART_ITEM_LIST* GetBuyPreviewCartList();

	const SCashShopCartItemInfo& GetBuyItemNow() const { return m_BuyItemNow; }
	//const SCashShopCartItemInfo& GetGiftItemNow() const { return m_GiftItemNow; }

	CS_INFO_LIST& GetWishList() { return m_WishList; }
	const std::vector<CASHITEM_SN>* GetInPackageItemList(CASHITEM_SN packageSN) const;
	int	GetCartItemCount() const { return (int)m_CartList.size(); }
	int	GetBuyPreviewCartCount() const;
	int	GetPreviewCartClassId() const { return m_PreviewCartClassId; }
	const SPackagePreviewUnit* GetClassPackagePreviewUnit(int classId) const;

	const SCashShopCartItemInfo* GetCartListItemInfo(const CART_ITEM_LIST& list, int index) const;

	int	GetOnSaleCount(const CS_INFO_LIST& list) const;
	std::wstring GetAbilityString( ITEMCLSID itemId );
	int	GetAbilityStringNumber(ITEMCLSID id) const;
	bool IsValidSkill( ITEMCLSID ability, int nOptionIndex ) const;
	void SetCartItemSkill( int index, int nOptionIndex );
	void SetPackageItemSkill( bool bGift, CASHITEM_SN sn, int nOptionIndex );
	void SetBuyItemNowSkill( int nOptionIndex );
	void SetPreviewCartItemSkill( int index, int nOptionIndex );
	void DoSetPackageItemSkill( CART_ITEM_LIST& itemList, CASHITEM_SN sn, int nOptionIndex );
#ifdef PRE_ADD_CASHSHOP_CREDIT
	int	GetUserCredit() const { return m_UserCredit; }
	int	GetUserPrepaid() const { return m_UserPrepaid; }
#endif // PRE_ADD_CASHSHOP_CREDIT
#if defined( PRE_ADD_CASHSHOP_CREDIT ) || defined( PRE_ADD_NEW_MONEY_SEED )
	void SetBuyMode( eBuyMode eBuyMode ) { m_eBuyMode = eBuyMode; }
	eBuyMode GetBuyMode() { return m_eBuyMode; }
#endif // PRE_ADD_CASHSHOP_CREDIT or PRE_ADD_NEW_MONEY_SEED

	int	GetUserCash() const	{ return m_UserCash; }
	int	GetUserReserve() const { return m_UserReserve; }
#ifdef PRE_ADD_NEW_MONEY_SEED
	int	GetUserSeed() const { return m_UserSeed; }
#endif // PRE_ADD_NEW_MONEY_SEED

	const WCHAR* GetCashLimitItemStartDate(CASHITEM_SN sn) const;
	const WCHAR* GetCashLimitItemEndDate(CASHITEM_SN sn) const;
	int GetCashLimitBuyAbleCount(CASHITEM_SN sn) const;
#ifdef PRE_ADD_LIMITED_CASHITEM
	int	GetCashLimitItemMaxCount(CASHITEM_SN sn) const;
	int	GetCashLimitItemRemainCount(CASHITEM_SN sn) const;
#endif // PRE_ADD_LIMITED_CASHITEM

	void SetCartItemAbility(int index, ITEMCLSID abilityID);
	void SelectCartItemSN(eCashUnitType type, int index, CASHITEM_SN sn);

	void SetPreviewCartItemAbility(int index, ITEMCLSID abilityID);
	bool SetItemAbility(CASHITEM_SN sn, ITEMCLSID& ability);

	void RemoveCartItem(int cartItemId);
	void RemovePreviewCartItem(CASHITEM_SN sn);
	void RemovePreviewCartItemById(int id);

	void SetBuyItemNowAbility(ITEMCLSID abilityID);

	//void SetGiftItemNowAbility(ITEMCLSID abilityID);
	void SetGiftItemNowSN(CASHITEM_SN sn);
	void SetPreviewCartClassId(int classId)		{ m_PreviewCartClassId = classId; }

	bool IsRequestingGiftBasicInfoes() const { return m_bRequestingGiftBasicInfoes; }
	bool IsCartEmpty(eCashUnitType type, int classId = -1) const;
	bool IsPackageItem(CASHITEM_SN sn) const;
	eCashShopError IsItemGiftable(eCashUnitType type, std::vector<CASHITEM_SN>& unableItemList) const;
	eCashShopError IsItemGiftable(eCashUnitType type, int receiverClassId, int receiverLevel, std::vector<CASHITEM_SN>& unableItemList) const;
	bool IsItemPermanent(CASHITEM_SN sn) const;

	bool HandleItemGiftable(eCashUnitType type, int receiverJobId, int receiverLevel);
	bool IsCartItemBuyable(const CART_ITEM_LIST& cartItemList, std::vector<CASHITEM_SN>& unableItemList) const;
	ePreviewType GetPreviewType(int classId) const;
	int	GetAttachableClass(int classId, ITEMCLSID itemId) const;
	int	GetCashShopDiscountRate(float priceDiscounted, float fixedPrice, float reserveAmount) const;
	CART_ITEM_LIST& GetBuyPackageItemList() { return m_BuyPackageItemList; }
	CART_ITEM_LIST& GetGiftPackageItemList() { return m_GiftPackageItemList; }
	void SetPackageItemAbility(bool bGift, CASHITEM_SN sn, ITEMCLSID ability, int itemListIndex);
	void DoSetPackageItemAbility(CART_ITEM_LIST& itemList, CASHITEM_SN sn, ITEMCLSID ability, int itemListIndex);

	void OpenChargeBrowser();
	void OnExceptionalCloseChargeBrowser(bool onInitOLE);
	void OpenBrowser(const std::string& url, const SUICoord& coord, eGeneralBrowserType type, const std::wstring& msgBoxString);
	void CloseBrowser();
	bool IsOpenChargeBrowser() const { return m_bChargeBrowserOpened; }
	bool IsOpenGeneralBrowser() const { return m_bGeneralBrowserOpened; }
	bool GetFullScreenModeFlag() const { return m_bFullScreenMode; }
	void CloseChargeBrowser();

	void PutItemIntoCart(CASHITEM_SN sn);
	bool PutItemIntoBuyItemNow(CASHITEM_SN sn);
	bool PutItemIntoGiftItemNow(CASHITEM_SN sn);
	void DoPutItemIntoItemNow(CASHITEM_SN sn, ITEMCLSID ability, SCashShopCartItemInfo& result, CART_ITEM_LIST& packageItemList);
	bool PutItemIntoPreviewCart(int classId, CASHITEM_SN sn, ITEMCLSID itemId, bool *pCheckPackageCashRing2 = NULL);
	bool PutItemIntoCartItemNow(CASHITEM_SN sn);

	void BuyCart();
	void BuyPreviewCart(int classId);
	void ShowGiftPayDlg(bool bShow, eCashUnitType type, const std::wstring& receiver, const std::wstring& memo);
	void GiftCart(bool bClear);
	void GiftPreviewCart();

	void ClearPreviewCart(int classId);

	void AddWishListItem(CASHITEM_SN sn);
	void DeleteWishListItem(CASHITEM_SN sn);
	bool IsInWishList(CASHITEM_SN sn) const;
	bool HandleItemPurchasable(CASHITEM_SN sn, ITEMCLSID itemId);
	bool IsCartPurchasble(eCashUnitType type, int classIdx = -1) const;
	bool HasPermanentItem(eCashUnitType type) const;

	bool Sort(eCashShopSubCatType catType, eCashShopEtcSortType sortType);
	void AttachPartsToPreview(CASHITEM_SN sn, ITEMCLSID itemId);
	bool IsAttachablePackageItem(CASHITEM_SN sn) const;
	bool IsValidSN(CASHITEM_SN sn) const;
	eCashShopError IsAllCartItemPeriodSelected(eCashUnitType type, bool bHandleError = false) const;
	bool IsPeriodSelectableItem(CASHITEM_SN presentSN) const;
	bool IsValidAbility(ITEMCLSID ability) const;
	eCashShopError IsAllCartItemAbilitySelected(eCashUnitType type, bool bHandleError = false) const;
	int	GetValidAbilityCount(const std::vector<ITEMCLSID>& abilityList) const;
	eCashShopError IsAllCartItemSelected(eCashUnitType type, bool bHandleError = false) const;
#ifdef PRE_ADD_CADGE_CASH
	eCashShopError IsCheckEnableCadge( eCashUnitType type );
	void RequestCadge( eCashUnitType type, const std::wstring& receiver, const std::wstring& memo );
	void OnRecvCashShopCadge( SCCashShopCadge* pData );
	bool SetCadgeItemList( int nPackageSN, CART_ITEM_LIST& itemList );
	void SetCadgeListToCartList();
#endif // PRE_ADD_CADGE_CASH

	bool IsCostumeItem(ITEMCLSID id) const;
#ifdef PRE_ADD_VIP
	bool IsVIPItem(ITEMCLSID id) const;
#endif
#ifdef PRE_MOD_BROWSER
#else
	LRESULT WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
#endif

	eRetPreviewChange ChangePreviewAvatar(int classId,bool bForce = false);
	void ChangeVehicleParts(int nClassID =0 ,int nPartClassID = 0 , bool bDefault=false);
	void GetJobUsable(std::vector<int>& usableClassIds, ITEMCLSID itemId) const;
	const int GetClassArrayIndex(int classId) const;

	void SetGiftBasicInfo(const std::wstring& receiver, const std::wstring& memo);
	const SGiftSendBasicInfo& GetGiftBasicInfo() const { return m_GiftBasicInfo; }
	void ClearGiftBasicInfo() { m_GiftBasicInfo.Clear(); }
	bool IsGiftNeedWarning(eCashUnitType type) const;
	void TerminateCashShop();
	void HandleCashShopError(eCashShopError code, bool bShowCode, CEtUIDialog* pCallbackDlg = NULL);
	int	 GetGiftListCount() const { return m_GiftListCount; }

	const std::map<int, SCashShopMainTabShowInfo>& GetShowMainTabList() const { return m_CashMainTabShowInfo; }
	const std::map<int, SCashShopTabShowInfo>* GetShowSubTabList(int mainTabNum) const;
	const SCashShopMainTabShowInfo* GetShowMainTabInfo(int tabNum) const;

#ifdef PRE_ADD_CASHSHOP_SUBCAT_COLOR
	const std::map<int, SCashShopSubTabShowInfo>*CDnCashShopTask::GetShowSubTabCatList( int mainTabNum ) const;
#endif

	const SCashShopTabShowInfo*	GetShowSubTab(int mainTabNum, int subTabNum) const;

	bool HandleItemLevelLimit(eCashUnitType type, int userLevel, int callBackDlgID /* = -1 */, CEtUICallback* pCall /* = NULL */);
	bool HandleItemLevelLimit(const CART_ITEM_LIST& itemList, int userLevel, int callBackDlgID = -1, CEtUICallback* pCall = NULL) const;
	bool HandleItemLevelLimit(const SCashShopCartItemInfo& itemInfo, int userLevel, int callBackDlgID /* = -1 */, CEtUICallback* pCall /* = NULL */);
	bool HandleItemOverlapBuy(const CART_ITEM_LIST& itemList, eCashUnitType type, int callBackDlgID, CEtUICallback* pCall);
	bool HandleWarning(CASHITEM_SN itemSN, int callBackDlgID, CEtUICallback* pCall);

	void ClearPetPreviewCart();	// 펫 바디 빼고 나머지 초기화
	bool PutItemIntoPetPreviewCart( CASHITEM_SN sn, ITEMCLSID itemId, int nPetPartsType = -1 );

	void SetNotOnSaleItem(int count, const CASHITEM_SN* itemList);
	bool IsNotOnSaleItem(CASHITEM_SN sn) const;

	bool IsCashTradable() const	{ return m_bCashTradable; }
	bool IsCashTradeCountRestrict() const { return m_bCashTradeCountRestrict; }

	bool IsCashShopMode(eCashShopMode mode) const;
#ifdef PRE_ADD_SALE_COUPON
	bool CheckCoupon();
	INT64 GetItemApplyCouponSN( CASHITEM_SN sn );
	const SCashShopItemInfo* GetSubCatItemByPresentID( eCashShopSubCatType type, int index ) const;
	void SetApplyCouponSN( INT64 nApplyCouponSN ) { m_nApplyCouponSN = nApplyCouponSN; }
	INT64 GetApplyCouponSN() { return m_nApplyCouponSN; }
#endif // PRE_ADD_SALE_COUPON
#ifdef PRE_ADD_INSTANT_CASH_BUY
	int	GetInstantCashBuyEnableType( eInstantCashShopBuyType eBuyType );
#endif // PRE_ADD_INSTANT_CASH_BUY

#ifdef PRE_ADD_CASHSHOP_RENEWAL	
	// 상품검색 - 상품명에 인자 str 이 포함되는 아이템들을 찾아서 카테고리로 추가한다.
	#define MAINCATEGORY_SERACH (999999999)
	#define SUBCATEGORY_SERACH (9999)
	void SearchItems( const std::wstring & str );
	void SetSortJob( int nJob ){
		m_SortJob = nJob;
	}
	void SetSortEtc( int nEtc ){
		m_SortEtc = nEtc;
	}	
	int GetSortJob(){
		return m_SortJob;
	}
	int GetetSortEtc(){
		return m_SortEtc;
	}	

	void UpdateIngameBannerItem();	
#endif // PRE_ADD_CASHSHOP_RENEWAL

protected:
	virtual void OnDispatchMessage(int nMainCmd, int nSubCmd, char *pData, int nSize);

private:
	void OnRecvCashShopMessage(int nSubCmd, char *pData, int nSize);

	void ConvertRecvGiftInfo(const TGiftInfo& src);

	void SetPackagePreview(int classId, ePreviewType type, CASHITEM_SN sn);
	bool ChangePreviewAvatarWhenAttachPackageItem(int classId, CASHITEM_SN sn);

	void GetChargeAddress(std::string& addressString);
	void ProcessGeneralBrowser();
	void DoOpenChargeBrowser();
#ifdef PRE_ADD_STEAMWORKS
	void DoOpenSteamChargeBrowser();
#endif // PRE_ADD_STEAMWORKS

private:
	bool m_bOpened;
	bool m_bRequestingCashShopOpen;

	CS_BASIC_INFO_LIST m_BasicInfoList;

	CS_CATEGORY_LIST m_ItemListByCategory;
	std::map<ITEMCLSID, int> m_AbilityDescInfoList;
	std::map<CASHITEM_SN, SCashItemLimitInfo> m_CashItemLimitInfo;
	std::map<CASHITEM_SN, std::vector<CASHITEM_SN> > m_CashItemPackageInfo;
	std::map<int, std::vector<int> > m_GiftReactInfo;
	std::map<int, SCashShopMainTabShowInfo> m_CashMainTabShowInfo;			// key : main_tab number

#ifdef PRE_ADD_CASHSHOP_SUBCAT_COLOR
	std::map<int, std::map<int, SCashShopSubTabShowInfo> > m_CashSubTabCatShowInfo;	// key : main_tab number, sub_category : sub_cate number
	bool ComposeStringSetChar( CString& pString, CString& InStr, char ch );
#endif

	std::map<int, std::map<int, SCashShopTabShowInfo> >	m_CashSubTabShowInfo;			// key : main_tab number, sublist : sub_tab number

	CART_ITEM_LIST m_PreviewCartList[CLASSKINDMAX];
	CART_ITEM_LIST m_PetPreviewCartList;
	int	m_CartIndexGen;
	int	m_PreviewCartIndexGen;
	CART_ITEM_LIST m_CartList;
	CS_INFO_LIST m_WishList;
	SCashShopCartItemInfo m_BuyItemNow;
	CDnCashShopDlg*	m_pCashShopDlg;
	SCashShopItemInfo m_SelectedItem;
#ifdef PRE_ADD_CADGE_CASH
	CART_ITEM_LIST m_CadgeList;
#endif // PRE_ADD_CADGE_CASH
	RECV_GIFT_LIST m_RecvGiftInfoList;
	bool m_bRequestingGiftBasicInfoes;
#ifdef PRE_ADD_CASHSHOP_CREDIT
	int	m_UserCredit;
	int	m_UserPrepaid;
#endif // PRE_ADD_CASHSHOP_CREDIT

#if defined( PRE_ADD_CASHSHOP_CREDIT ) || defined( PRE_ADD_NEW_MONEY_SEED )
	eBuyMode m_eBuyMode;
#endif // PRE_ADD_CASHSHOP_CREDIT or PRE_ADD_NEW_MONEY_SEED

	int	m_UserCash;
	int	m_UserReserve;
#ifdef PRE_ADD_NEW_MONEY_SEED
	int m_UserSeed;
#endif // PRE_ADD_NEW_MONEY_SEED

	int	m_PreviewCartClassId;
	int	m_CashShopBGMIndex;
	EtSoundChannelHandle m_hCashShopBGM;
	bool m_bSoundOnOpen;
	bool m_bSoundOnClose;
	float m_fSoundDelta;

	SPackagePreviewUnit m_PackagePreviewUnit[CLASSKINDMAX];
	CART_ITEM_LIST m_BuyPackageItemList;
	CART_ITEM_LIST m_GiftPackageItemList;
	SGiftSendBasicInfo m_GiftBasicInfo;
	int	m_CashCoinRestriction;

	bool m_bFullScreenMode;
	bool m_bChargeBrowserOpened;
	bool m_bGeneralBrowserOpened;
#ifdef PRE_MOD_BROWSER
#else
	eGeneralBrowserType	m_GeneralBrowserOpenedType;
#endif
	std::string m_AccountName;

	bool m_bBuying;
	int	m_GiftListCount;

	std::set<CASHITEM_SN> m_NotOnSaleList;

	bool m_bCashTradable;	//	todo by kalliste : combine CashShopMode
	bool m_bCashTradeCountRestrict;
	int	m_CashShopMode;
	eCashChargeBrowserType m_ChargeBrowserType;
	int	m_ChargeBrowserWidth;
	int	m_ChargeBrowserHeight;

#ifdef PRE_RECEIVEGIFTALL
	int m_nSendItemSize;
#endif

#ifdef PRE_ADD_CASHSHOP_REFUND_CL
	std::map<INT64, SRefundCashItemInfo> m_RefundInvenItems;
	int	m_RefundInvenItemCount[eRITYPE_MAX];
#endif
	__time64_t m_ChargeTime;
#ifdef PRE_ADD_SALE_COUPON
	INT64 m_nApplyCouponSN;
#endif // PRE_ADD_SALE_COUPON

#ifdef PRE_ADD_CASHSHOP_RENEWAL
	std::wstring m_strSearch; // 상품검색 단어.
	int m_SortJob; // 정렬기준-직업.
	int m_SortEtc; // 정렬기준-기타.	
#endif // PRE_ADD_CASHSHOP_RENEWAL


#if defined(PRE_ADD_68286)
public:
	void RequestCashShopRecvGiftBasicInfoByShortCut();
#endif // PRE_ADD_68286

#ifdef PRE_RECEIVEGIFTALL
public:
	void SetSendItemSize(int nSize) { m_nSendItemSize = nSize; }
	int  GetSendItemSize() { return m_nSendItemSize; }
	std::map<int, std::vector<int>> GetGiftReactInfo() { return m_GiftReactInfo; }
#endif
};

#define GetCashShopTask() CDnCashShopTask::GetInstance()

#ifdef PRE_ADD_SALE_COUPON

static bool CompareCouponExpire( CDnItem* s1, CDnItem* s2 )
{
	if( s1 == NULL || s2 == NULL ) return false;
	if( s1->IsEternityItem() || s2->IsEternityItem() ) return false;

	if( *(s1->GetExpireDate()) < *(s2->GetExpireDate()) ) return true;
	else if( *(s1->GetExpireDate()) > *(s2->GetExpireDate()) ) return false;

	return false;
}

#endif // PRE_ADD_SALE_COUPON