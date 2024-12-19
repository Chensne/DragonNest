#pragma once
#include "DnCustomDlg.h"
#include "DnCashShopDefine.h"
#include "DnItem.h"


class CDnCashShopPayItemDlg;
class CDnCashShopMessageBox;
#ifdef PRE_ADD_CADGE_CASH
class CDnCashShopCommonDlgMgr;
#endif // PRE_ADD_CADGE_CASH


class CDnCashShopPayDlg : public CDnCustomDlg, public CEtUICallback
{
	enum
	{
		MESSAGEBOX_CONFIRM,
		MESSAGEBOX_ERROR,
		MESSAGEBOX_INVEN_CONFIRM,
#ifdef PRE_ADD_CASHSHOP_CREDIT
		MESSAGEBOX_CREDIT_ERROR,
#endif // PRE_ADD_CASHSHOP_CREDIT
	};

	struct SPaySlotUnit
	{
		CEtUIStatic*		pBase;
		CEtUIStatic*		pCBase;
		CEtUIStatic*		pPBase;

		CDnQuickSlotButton*	pSlotBtn;		// todo : replace CDnQuickSlotButton with ui resrc file. by kalliste
		CEtUIStatic*		pName;
		CEtUIStatic*		pQuickSlotBG;

		CEtUIStatic*		pPrice;
		CEtUIStatic*		pCount;
		CEtUIStatic*		pPeriod;

		CEtUIComboBox*		pAbilityComboBox;
		CEtUIComboBox*		pPeriodComboBox;
		CEtUIComboBox*		pSkillComboBox;

		CEtUIButton*		pClose;

		CDnItem*			pItem;

		bool				bCostume;
		bool				m_bSelectPeriod;

		SPaySlotUnit()
		{
			pSlotBtn = NULL;

			// Statics
			pBase = pName = pQuickSlotBG = 
			pPrice = pCount = pCBase = 
			pPeriod = pBase = NULL;

			pAbilityComboBox = NULL;
			pPeriodComboBox = NULL;
			pSkillComboBox = NULL;
			pItem = NULL;
			pClose = NULL;
			bCostume = false;
			m_bSelectPeriod = false;
		}

		~SPaySlotUnit()
		{
			SAFE_DELETE(pItem);
		}

		void Clear()
		{
			pSlotBtn->ResetSlot();
			pName->ClearText();
			pPrice->ClearText();
			pCount->ClearText();
			pPeriod->ClearText();
			pAbilityComboBox->RemoveAllItems();
			pPeriodComboBox->RemoveAllItems();
			pSkillComboBox->RemoveAllItems();
			SAFE_DELETE(pItem);
			m_bSelectPeriod = false;
		}

		void Show(bool bShow)
		{
			pBase->Show(bShow);
			pCBase->Show(bShow);
			pPBase->Show(bShow);
			pSlotBtn->Show(bShow);
			pName->Show(bShow);
			pQuickSlotBG->Show(bShow);
			pPrice->Show(bShow);
			pCount->Show(bShow);
			pPeriod->Show(bShow);
			pClose->Show(bShow);

			pAbilityComboBox->Show(bShow);
			pPeriodComboBox->Show(bShow);
			pSkillComboBox->Show(bShow);

			if (bCostume)
			{
				pPrice->Show(false);
				pCount->Show(false);
				pPeriod->Show(false);
			}
			else
			{
				pAbilityComboBox->Show(false);
				pPeriodComboBox->Show(false);
				pSkillComboBox->Show(false);
			}
			pPeriodComboBox->Show( m_bSelectPeriod );
		}

		void ShowCloseBtn(bool bShow)
		{
			pClose->Show(bShow);
		}

#ifdef PRE_ADD_CADGE_CASH
		void Enable( bool bEnable )
		{
			pClose->Enable( bEnable );
			pAbilityComboBox->Enable( bEnable );
			pPeriodComboBox->Enable( bEnable );
			pSkillComboBox->Enable( bEnable );
		}
#endif // PRE_ADD_CADGE_CASH

		void Set(const SCashShopItemInfo& info, const SCashShopCartItemInfo& cartItemInfo);
		void SetSkillList( ITEMCLSID itemID, int nOptionIndex );
	};

public:
	CDnCashShopPayDlg(UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL);
	virtual ~CDnCashShopPayDlg(void);

#ifdef PRE_ADD_CADGE_CASH
	virtual void	Initialize(bool bShow, CDnCashShopCommonDlgMgr* pCommonDlgMgr);
#else // PRE_ADD_CADGE_CASH
	virtual void	Initialize(bool bShow);
#endif // PRE_ADD_CADGE_CASH
	void			Show(bool bShow);

	void			Update(eCashUnitType type);

	void			UpdateItemList(const CART_ITEM_LIST& willPayItemList);
	void			UpdateInfoList(const CART_ITEM_LIST& willPayItemList);
	void			UpdateEtc();
	bool			HasEnoughMoneyToBuy(int moneyNeeded, bool bBuyReserve) const;
	void			Clear(bool bAll = true);

protected:
	virtual void	Process(float fElapsedTime);
	virtual void	InitialUpdate();
	virtual void	InitCustomControl(CEtUIControl* pControl);
	virtual void	ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void	OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg );

private:
	bool			IsBuyReserveMode() const;
#ifdef PRE_ADD_NEW_MONEY_SEED
	bool			IsBuySeedMode() const;
#endif // PRE_ADD_NEW_MONEY_SEED
#ifdef PRE_ADD_CASHSHOP_CREDIT
	bool			IsBuyCashMode() const;
	bool			IsBuyCreditMode() const;
	const SCashShopItemInfo* CheckCreditableItem();
#endif // PRE_ADD_CASHSHOP_CREDIT
	void			ClearSlots(bool bShowOff);

	void			UpdatePage(const CART_ITEM_LIST& itemList);
	void			UpdatePageStatic();
	void			UpdatePageControllers(const CART_ITEM_LIST& itemList);

	void			NextPage();
	void			PrevPage();

	void			SetCartItemAbility(int id, ITEMCLSID itemId, int itemListIndex);
	void			SetCartItemSkill( int id, int nSkillID );

	bool			DoTurnOverPage(bool bPrev, const CART_ITEM_LIST& itemList);
	bool			IsGiftMode() const;

#ifdef PRE_ADD_CASHSHOP_REFUND_CL
	void			SetRefundStatic();
	bool			IsDirectMoveCashInven() const;
#endif
	void			SortItemList(CART_ITEM_LIST& resultList, const CART_ITEM_LIST& srcList, std::vector<int>& vecWillPayItemIndex);

	eCashUnitType			m_Mode;

	std::map<int, CDnCashShopPayItemDlg*>	m_pItemCacheList;

	std::vector<SPaySlotUnit>	m_SlotUnits;
	CEtUIListBoxEx*				m_pGoodsInfoListBox;

	CEtUIStatic*		m_pTitle;
	CEtUITextBox*		m_pSum;
	CEtUIStatic*		m_pDiscount;
#ifdef PRE_ADD_NEW_MONEY_SEED
	CEtUITextBox*		m_pReserve;
#else // PRE_ADD_NEW_MONEY_SEED
	CEtUIStatic*		m_pReserve;
#endif // PRE_ADD_NEW_MONEY_SEED
	CEtUIStatic*		m_pInfo;
	CEtUICheckBox*		m_pBuyReserve;
#ifdef PRE_ADD_CASHSHOP_CREDIT
	CEtUICheckBox*		m_pBuyCash;
	CEtUICheckBox*		m_pBuyCredit;
	CEtUIStatic*		m_pBuyCreditTitle;
	CEtUIStatic*		m_pBuyMsg;
#endif // PRE_ADD_CASHSHOP_CREDIT
	CEtUIStatic*		m_pBuyReserveTitle;
#ifdef PRE_ADD_NEW_MONEY_SEED
	CEtUICheckBox*		m_pBuySeed;
	CEtUIStatic*		m_pBuySeedTitle;
	int					m_nSeed;	// 시드 적립금
#endif // PRE_ADD_NEW_MONEY_SEED
#ifdef PRE_ADD_CASHSHOP_REFUND_CL
	CEtUICheckBox*		m_pDirectInvenCheckBox;
	CEtUIStatic*		m_pDirectInvenCheckBoxTitle;
	CEtUIStatic*		m_pDirectInvenStatic;
#endif

#ifdef PRE_ADD_CADGE_CASH
	CEtUIButton*				m_pCadgeButton;
	CEtUIStatic*				m_pCadgeRecieverName;
	CDnCashShopCommonDlgMgr*	m_pCommonDlgMgr;
#endif // PRE_ADD_CADGE_CASH

#ifdef PRE_ADD_CASHSHOP_ACTOZ
	CEtUIStatic * m_pStaticBuyMsg;
#endif // PRE_ADD_CASHSHOP_ACTOZ

	CEtUIButton*		m_pBuyBtn;
	CEtUIButton*		m_pCancelBtn;
	CEtUIButton*		m_pChargeBtn;
	CEtUIButton*		m_pCloseBtn;

	int					m_Sum;
	int					m_Reserve;

	CEtUIButton*		m_pPrevBtn;
	CEtUIButton*		m_pNextBtn;
	CEtUIStatic*		m_pPageNum;

	int					m_CurrentPage;
	int					m_MaxPage;

	std::map<int, int>	m_DataSlotIdxMatchList;	// key : slot index

	CDnCashShopMessageBox* m_pCashShopMsgBox;
#if defined( PRE_ADD_CASHSHOP_CREDIT ) || defined( PRE_ADD_NEW_MONEY_SEED )
	eBuyMode			m_eBuyMode;
#endif // PRE_ADD_CASHSHOP_CREDIT or PRE_ADD_NEW_MONEY_SEED
#ifdef PRE_ADD_SALE_COUPON
	INT64				m_ApplyCouponSN;
#endif // PRE_ADD_SALE_COUPON
	CASHITEM_SN			m_nLastPresentSN;

	std::vector<int>	m_vecWillPayItemIndex;
};