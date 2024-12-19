#pragma once
#include "EtUITabDialog.h"
#include "DnInven.h"

class CDnStoreListDlg;
class CDnSlotButton;
class MIInventoryItem;

class CDnRepairConfirmEquipDlg;
class CDnRepairConfirmAllDlg;

class CDnStoreTabDlg : public CEtUITabDialog, public CEtUICallback
{
public:
	enum
	{
		REPAIR_CONFIRM_EQUIP_DIALOG,
		REPAIR_CONFIRM_ALL_DIALOG,
		INVALID_TABID = -1,
	};

	CDnStoreTabDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnStoreTabDlg(void);

protected:

	int m_nStoreType;

	CEtUIStatic *m_pStaticMyPoint;
	CEtUIStatic *m_pStaticPointValue;
	CEtUIStatic *m_pStaticCover_A;
	CEtUIStatic *m_pStaticCover_B; // 껍데기도 숨겨주자.
#ifdef PRE_ADD_NEW_MONEY_SEED
	CEtUIStatic* m_pStaticSeedText;
	CEtUIStatic* m_pStaticSeedLine;
	CEtUIStatic* m_pStaticSeedBoard;
	CEtUIStatic* m_pStaticSeedPoint;
#endif // PRE_ADD_NEW_MONEY_SEED
	CEtUIRadioButton *m_pTabButtonWeapon;
	CEtUIRadioButton *m_pTabButtonParts;
	CEtUIRadioButton *m_pTabButtonNormal;
	CEtUIRadioButton *m_pTabButtonBox;
	CEtUIRadioButton *m_pTabButton5;
	CEtUIRadioButton *m_pTabButton6;
	CEtUIRadioButton *m_pTabButton7;

	CEtUIStatic* m_pStaticRepurchaseBoard;
	CEtUIStatic* m_pStaticRepurchase;

	CDnStoreListDlg *m_pStoreWeaponDlg;		// Note : 무기
	CDnStoreListDlg *m_pStorePartsDlg;		// Note : 방어구
	CDnStoreListDlg *m_pStoreNormalDlg;		// Note : 소비
	CDnStoreListDlg *m_pStoreBoxDlg;		// Note : 상자
	CDnStoreListDlg *m_pStore5Dlg;			// 이제 그냥 번호로 관리.
	CDnStoreListDlg *m_pStore6Dlg;
	CDnStoreListDlg *m_pStore7Dlg;

	CEtUIButton *m_pButtonRepair;
	CEtUIButton *m_pButtonAllRepair;
#ifdef PRE_ADD_PVPRANK_INFORM
	CEtUIButton* m_pButtonPvPRankInfo;
#endif

	CDnRepairConfirmEquipDlg *m_pRepairConfirmEquipDlg;
	CDnRepairConfirmAllDlg *m_pRepairConfirmAllDlg;

public:
	void SetStoreItem( int nTabID, MIInventoryItem *pItem );
	void ResetAllListSlot();
	void SetTabUIStringID( int nTabID, int nUIStringID );
	void ResetSlot(BYTE cSlotIndex);
	void ResetRepurchaseTab();
	void SetRepurchaseTab(int nLastTabID);
	CEtUIRadioButton* GetStoreTabButtonByTabID(int tabID) const;
	CDnStoreListDlg* GetStoreListDlgByTabID(int tabID) const;
	bool IsRepurchasableTab(int nTabID) const;
	int GetRepurchasableTabID() const;
	bool IsShowRepurchasableTab() const;
	bool IsRepurchaseTabBtnName(const std::string& controlName) const;
	void OnOpenOrCloseRepurchaseTab(bool bOpenRepurchase);
	void OnStoreSell();
	void SetStoreType(int nType);
	int GetStoreType()	{ return m_nStoreType; }
#ifdef PRE_ADD_PVPRANK_INFORM
	void CheckPvPRankButton(int nShopID);
#endif

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void Show( bool bShow );
	virtual void Process( float fElapsedTime );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg = 0 );
};