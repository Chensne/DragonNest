#pragma once
#include "DnCustomDlg.h"
#include "DnItem.h"

class CDnItem;
#ifdef PRE_ADD_NPC_REPUTATION_SYSTEM
class CDnRepStoreBenefitTooltipDlg;
#endif // #ifdef PRE_ADD_NPC_REPUTATION_SYSTEM

#ifdef PRE_ADD_DIRECT_BUY_UPGRADEITEM
class CDnMarketDirectBuyDlg;
#endif // #ifdef PRE_ADD_DIRECT_BUY_UPGRADEITEM

class CDnItemUpgradeDlg : public CDnCustomDlg, public CEtUICallback
{
	enum
	{
		ITEM_UPGRADE_CONFIRM_DIALOG,
		ITEM_UPGRADE_MOVIE_DIALOG,
		ITEMSLOT_MAX = 5,
		EQUIPPED_ITEM = 7, 
		ENCHANTGUARD_ITEMID = 1107302400,
#ifdef _TW
		ENCHANTSHIELD_ITEMID = 1107402752,
#else
		ENCHANTSHIELD_ITEMID = 1107386368,	// 기본은 중국.
#endif
	};

public:
	CDnItemUpgradeDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnItemUpgradeDlg(void);

protected:
	// 강화템 슬롯
	CDnItem *m_pUpgradeItem;
	CDnItemSlotButton *m_pUpgradeItemSlot;
	CDnQuickSlotButton *m_pQuickSlotButton;
#ifdef PRE_ADD_EQUIPED_ITEM_ENCHANT
	CDnSlotButton*	   m_pControlSlotButton;	// 퀵슬롯(인벤토리)버튼 뿐만 아니라 장착슬롯도 적용되기 때문에 해당 컨트롤로 바꾼다. 
	CDnQuickSlotButton *m_pPreInvenSlotButton;
	CDnQuickSlotButton *m_pCurrentInvenSlotButton;
#endif 
	eItemRank m_emUpgradeItemRank;

	// 장착중인 강화템 슬롯
#ifdef PRE_ADD_EQUIPED_ITEM_ENCHANT
	std::vector< CDnItemSlotButton* > m_pVecEquippedUpgradeItemSlot;
#endif 

	// 재료템 슬롯
	CDnItem *m_pItem[ITEMSLOT_MAX];
	CDnItemSlotButton *m_pItemSlotButton[ITEMSLOT_MAX];
#ifdef PRE_ADD_DIRECT_BUY_UPGRADEITEM 
	CEtUIButton *m_pUpgradeItemButton[ITEMSLOT_MAX];
	bool		 m_bIsMiniMarket;
#endif
	CEtUIButton *m_pSmallHelpButton;

	CEtUIStatic *m_pStaticGold;
	CEtUIStatic *m_pStaticSilver;
	CEtUIStatic *m_pStaticBronze;
	DWORD m_dwColorGold;
	DWORD m_dwColorSilver;
	DWORD m_dwColorBronze;

	CEtUIButton *m_pButtonOK;
	CEtUIButton *m_pButtonCancel;

	CDnItem *m_pGuardItem;
	CDnItemSlotButton *m_pGuardItemSlotButton;
#if defined(PRE_ADD_ENCHANTSHIELD_CASHITEM)
	CEtUIComboBox* m_pComboBoxGuard;	
	int m_nCurShieldCount;
	int m_nCurGuardCount;
	int m_nTotalShieldNees;
	int m_nGuardItemID;			// 보호젤리 아이템ID
	int m_nShiledItemID;		// 상위 보호젤리 아이템ID
	char m_cEnchantGuardType;	// 각종 보호젤리 타입 eEnchantGuardType
#else
	CEtUICheckBox *m_pCheckGuard;	
	CEtUIStatic *m_pStaticTextBase;
	CEtUIStatic *m_pStaticTextUse;
	CEtUIStatic *m_pStaticTextNotUse; // 사용불가
#ifdef PRE_ADD_JELLYCOUNT
	CEtUIStatic *m_pStaticTextNotUse2;// 사용불가(사용가능)
#endif // PRE_ADD_JELLYCOUNT
#endif	
	CEtUIStatic *m_pStaticTextCount;
	CEtUIButton *m_pButtonDetail;
	CEtUIStatic *m_pStaticGuard;
	int m_nTotalGuardNeeds;

#ifdef PRE_ADD_NPC_REPUTATION_SYSTEM
	CEtUIStatic* m_pReputationBenefit;
	CDnRepStoreBenefitTooltipDlg* m_pStoreBenefitTooltip;
#endif // #ifdef PRE_ADD_NPC_REPUTATION_SYSTEM
	CEtUIStatic *m_pStaticNextUpgradeState;

	CDnItem *m_pNextUpgradeItem;
	bool m_bShowNextUpgradeTooltip;
	int m_nSlotIndex;
	bool m_bIsCashItem;
#ifdef PRE_ADD_EQUIPED_ITEM_ENCHANT
	bool m_bEnchantEquippedItem;
#endif

	CEtUIStatic *m_pStaticTitle;
	std::wstring m_wszTitle;
	int m_nRemoteItemID;

#ifdef PRE_ADD_EQUIPED_ITEM_ENCHANT
public:
	void CheckUpgradeItem();
#else
	// 강화슬롯 체크
	void CheckUpgradeItem();
#endif

	// 현재 사용할 강화테이블ID. DnItem에 있는 TableID보다 더 구체적인 EnchantTable의 ItemID다..
	// DnItem의 TableID+해당 EnchantLevel까지 같이 해서 구한 값.
	int GetEnchantTableItemID( CDnItem *pItem ) const;

	// 강화에 필요한 돈
	int GetEnchantPrice( int nEnchantTableItemID );

	// 필요아이템 세팅. 갯수 모두 충족되면 true 리턴.
	bool CheckNeedItemInfo( int nEnchantTableItemID );
	void SetItemSlot( int nSlotIndex, int nItemID, int nNeedItemCount, int nCurItemCount );

	// 강화 파괴방지 아이템 체크
	void CheckUpgradeGuardItem();

#if defined(PRE_ADD_ENCHANTSHIELD_CASHITEM)
	// 강화 보조용 아이템 셋팅
	void SetUpgardeGuardItem();
#endif

	void CheckUseItemTooltip();

	// 다음 강화단계
	void RefreshNextUpgradeItem();
	void ShowNextUpgradeToolTip( bool bShow, float fX = 0.f, float fY = 0.f );

	bool IsUpgradableItem(CDnSlotButton* pSlotButton, CDnItem* pItem) const;

public:

#ifdef PRE_ADD_EQUIPED_ITEM_ENCHANT
	void SetUpgradeItem( CDnSlotButton *pPressedButton );
	bool IsEquipeedItem() { return m_bEnchantEquippedItem; }
	bool IsUpgradeItem( CDnSlotButton *pPressedButton ); 
#else
	// 인벤에서 우클릭 바로등록.
	void SetUpgradeItem( CDnQuickSlotButton *pPressedButton );
#endif 

#ifdef PRE_ADD_EQUIPED_ITEM_ENCHANT
	void SetEquippedItem( int nEquipIndex, MIInventoryItem *pItem );
	MIInventoryItem *GetEquippedItem( int nEquipIndex );
	CDnSlotButton   *GetEquipSlotButton( int nEquipIndex );
	void ResetEquippedSlot( int nEquipIndex );
#endif 

	void SetRemoteItemID( int nRemoteItemID );

	void OnRecvEnchant(int nSlotIndex);	// 인자는 검증용.
	void OnRecvEnchantCash(int nSlotIndex);	// 인자는 검증용.
	void OnRecvEnchantComplete( char cEnchantResult, int nItemID, char cLevel = 0, char cOption = 0 );
	void OnRecvEnchantCompleteCash(char cEnchantResult, int nItemID, char cLevel = 0, char cOption = 0);

#ifdef PRE_ADD_JELLYCOUNT
	void CheckUpgradState(); // 아이템의 upgrade 상태에 따른 텍스트처리.
#endif

#ifdef PRE_ADD_EQUIPED_ITEM_ENCHANT
	void				SafeDelForceSlotItem()	{ SAFE_DELETE( m_pUpgradeItem); }
	CDnItemSlotButton* GetCurrentForceSlot() { return m_pUpgradeItemSlot ? m_pUpgradeItemSlot : NULL; }
	void SetPreInvenSlot( CDnQuickSlotButton* pInvenSlot ) { m_pPreInvenSlotButton = pInvenSlot; }		
	CDnQuickSlotButton* GetPreInvenSlot()    { return m_pPreInvenSlotButton ? m_pPreInvenSlotButton : NULL; }
	void SetCurrentInvenSlot( CDnQuickSlotButton* pInvenSlot ) { m_pCurrentInvenSlotButton = pInvenSlot; }	
	CDnQuickSlotButton* GetCurrentInvenSlot() { return m_pCurrentInvenSlotButton; }
#endif 

#ifdef PRE_ADD_DIRECT_BUY_UPGRADEITEM
	void ResetUpgradeItemBtn();
	void ShowDirectBuyMarketDlg(int nSlotIndex);
	void RefreshUpgradeItemButton();
	void SetMiniMarketFlag(bool bFlag) { m_bIsMiniMarket = bFlag; }
#endif

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg = 0 );
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
};