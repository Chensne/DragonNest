#pragma once

#ifdef PRE_ADD_CASHSHOP_RENEWAL

#include "EtUITabDialog.h"
#include "DnCashShopDefine.h"
#include "DnCashShopCommonDlgMgr.h"
#include "DnCashShopDlg.h"

class CDnCashShopCatDlg;
class CDnCashShopPreviewDlg;
class CDnCashShopCartDlg;
class CDnCashShopCommonDlgMgr;
class CDnCashShopInvenTabDlg;
class CDnCashShopCouponDlg;
class CDnCashShopAboutPetal;
class CDnCashShopGiftFailDlg;
class CDnDummyModalDlg;
#ifdef PRE_ADD_CASHSHOP_REFUND_CL
class CDnCashShopRefundInvenDlg;
#endif
class CDnCashShopPetPreviewDlg;


class CDnCashShopMainBanner;

//class CDnCashShopRenewalDlg : public CDnCustomDlg, virtual public CDnCashShopDlg  //public CEtUITabDialog, public CEtUICallback
class CDnCashShopRenewalDlg : public CDnCashShopDlg  //public CEtUITabDialog, public CEtUICallback
{
public:
	enum
	{
		PREVIEW_DLG,
		INVEN_DLG,
		SEND_GIFT_DLG,
		CASHCHARGE_ALERT_DLG,
		LEVEL_ALERT_CART_DLG,
		LEVEL_ALERT_PREVIEWCART_DLG,
		LEVEL_ALERT_BUYITEMNOW_DLG,
	};

	CDnCashShopRenewalDlg(UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL);
	virtual ~CDnCashShopRenewalDlg();

	virtual void		Initialize(bool bShow);
	virtual void		Show(bool bShow);
	void				UpdateGiftReceiverInfo(const SCCashShopCheckReceiver& info);
	void				UpdateRecvGiftList();
	void				UpdateCashReserveAmount();

	void				OpenGiftReply(const WCHAR* pName, int giftDBID);

	void				OpenBuyCart(bool bShow);
	void				OpenBuyPreviewCart(bool bShow);
	void				GiftCart(bool bClear);
	void				GiftPreviewCart(bool bClear);
	void				UpdateCart(eRetCartAction action, const SCartActionParam& param);

#ifdef PRE_ADD_INVEN_EXTENSION
	void				ReserveBuyItemNowOnNextOpened(CASHITEM_SN sn);
#endif

	void				ClearPreviewCart(int classId);
	bool				AttachPartsToPreview(CASHITEM_SN sn, ITEMCLSID itemId, bool bCheckPackageCashRing2 = false);
	void				DetachPartsToPreview(int classId, CASHITEM_SN sn);
	void				HandleCashShopError(eCashShopError code, bool bShowCode, CEtUIDialog* pCallbackDlg = NULL) const;

	bool				IsOpenCashShopInvenDlg() const;
	bool				IsOpenCashShopRecvGiftDlg() const;
	void				DisableAllDlgs(bool bEnable, const std::wstring& text, float delayTime = 0.f);
	eRetPreviewChange	ChangePreviewAvatar(int classId,bool bForce = false);

	void                ChangeVehicleParts(int nClassID = 0 ,int nPartClassID = 0, bool bDefault=false);
	void				ShowGiftPayDlg(bool bShow, eCashUnitType type);
	void				ShowCouponDlg(bool bShow);
	void				ShowRecvGiftDlg(bool bShow);
	void				ShowGiftFailDlg(bool bShow, const std::wstring& msg, const std::vector<CASHITEM_SN>& itemList, UINT uType, int callBackDlgID = -1, CEtUICallback* pCall = NULL) const;
	eCashShopError		IsCartGiftable(eCashUnitType type) const;
	bool				IsCartBuyable() const;
	bool				IsPreviewCartBuyable() const;

	void				EnableDummyModalDlg(bool bSet);
	void				OnCloseCashShopDlg();

#ifdef PRE_ADD_CASHSHOP_REFUND_CL
	void				UpdateRefundInven();
	CDnCashShopRefundInvenDlg* GetRefundInven() { return m_pInvenRefundDlg; }
#endif

	void				ShowCashShopMsgBox(const std::wstring& firstmsg, const std::wstring& secondmsg, int nID, CEtUICallback* pCallBack);
	void				ChangePetPreviewAvatar( int nItemID );
	void				ChangePetPreviewParts( int nPartsType, int nItemID );
	void				ChangePetPreviewColor( eItemTypeEnum eItemType, DWORD dwColor );
	void				OpenBuyPetPreviewCart();
	bool				IsExistPetAvatar();
	void				SwapPreview( bool bShowChar );
	void				DetachPartsToPetPreview( int nPartsSlot );
	CDnCashShopPetPreviewDlg* GetPetPreviewDlg() { return m_pPetPreviewDlg; }
#ifdef PRE_ADD_SALE_COUPON
	virtual void				CheckShowSaleTabButton( bool bSaleCouponGift = false );
	void				SetSaleTabButtonBlink( bool bBlink );	
#ifdef PRE_ADD_SALE_COUPON_CATEGORY
	void				SetSaleCouponOpen( bool bSaleCouponOpen, int nSaleCouponSubCategory ) { m_bSaleCouponOpen = bSaleCouponOpen; m_nSaleCouponSubCategory = nSaleCouponSubCategory; }
#else // PRE_ADD_SALE_COUPON_CATEGORY
	void				SetSaleCouponOpen( bool bSaleCouponOpen ) { m_bSaleCouponOpen = bSaleCouponOpen; }
#endif // PRE_ADD_SALE_COUPON_CATEGORY
#endif // PRE_ADD_SALE_COUPON

#ifdef PRE_ADD_LIMITED_CASHITEM
	void				UpdateCurrentGoodsDlg();
#endif // PRE_ADD_LIMITED_CASHITEM
#ifdef PRE_ADD_CADGE_CASH
	void				CadgeButtonClick() { m_bCadgeButtonClick = true; }
	void				SetReserveCadgeItemList( std::wstring strCadgeReciever, int nMailID, int nPackageSN, int nCount, TWishItemInfo* pItemList );
	int					GetCadgeMailID() { return m_nMailID; }
	void				ResetCadgeMailID() { m_nMailID = 0; }
#endif // PRE_ADD_CADGE_CASH

#ifdef PRE_ADD_CASHSHOP_ACTOZ
	void ShowConfirmRefund( CDnItem * pItem );
#endif // PRE_ADD_CASHSHOP_ACTOZ

	void SelectMainCategory( int tabID ); // 메인카테고리선택.
	void SelectSubCategory( int mainTabID, int subTabID ); // 서브카테고리선택.

	void SetShowIngameBannerItem( std::wstring & strItemName ){
		m_bShowIngameBannerItem = true;
		m_strBannerItemName.assign( strItemName );
	}
	void ShowIngameBannerItem();

private:
	int GetSortStringNum(eCashShopEtcSortType type) const;

	void ChangeSortOrder( bool bJob, int nValue );	// 정렬방식 변경.
	void SearchItems( const std::wstring & str ); // 상품검색.
	
protected:
	virtual void		Render(float fElapsedTime);
	virtual void		InitialUpdate();
	virtual void		ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0);
	virtual void		OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg = 0 );
	virtual void		Process( float fElapsedTime );
	virtual bool		MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );


	virtual void OnChangeResolution();

protected:
	const WCHAR*		GetCatTitle(const SCashShopTabShowInfo& info) const;
	void				MoveCategory(bool bNext);


	
	CEtUIIMEEditBox * m_EditBoxSearch;
	CEtUIStatic * m_pStaticText;
	
	//CEtUIComboBox * m_pJobSortComboBox;
	//CEtUIComboBox * m_pEtcSortComboBox;

	//const eCashShopSubCatType	m_Type;

	CDnCashShopMainBanner * m_pCSMainBanner;


	class CDnCashShopMenuListDlg * m_pMenuListDlg;

	bool m_bShowIngameBannerItem; // 인게임배너에서 클릭한경우 해당아이템을 검색해서 보여준다.
	std::wstring m_strBannerItemName;
	
	
};

#endif // PRE_ADD_CASHSHOP_RENEWAL