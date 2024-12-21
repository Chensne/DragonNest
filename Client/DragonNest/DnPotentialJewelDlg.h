#pragma once
#include "DnCustomDlg.h"
#include "DnItem.h"

class CDnItem;
class CDnQuickSlotButton;

#ifdef PRE_MOD_POTENTIAL_JEWEL_RENEWAL

class CDnPotentialStatsDlg;
class CDnPotentialCheckCancelDlg;
class CDnPotentialJewelDlg : public CDnCustomDlg, public CEtUICallback
{
	enum
	{
		ITEM_CATEGORY_WEAPON = 17,
		ITEM_CATEGORY_ARMOR,
		ITEM_CATEGORY_ACCESSORY,
	};

public:
	CDnPotentialJewelDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnPotentialJewelDlg();

protected:
	CDnItem*			m_pItem;
	CDnItemSlotButton*	m_pItemSlot;
	
	CEtUIButton*		m_pButtonApply;
	CDnQuickSlotButton*	m_pButtonQuickSlot;

	CEtUIButton*		m_pButtonCharStats;
	CEtUIButton*		m_pButtonPotentialApply;
	
	CEtUITextBox*		m_pCurrentDescription;
	CEtUITextBox*		m_pAtferDescription;

	CEtUIStatic*		m_pNotifyText;
	CEtUIStatic*		m_pGrayIcon[3];
	
	char				m_cPrevPotentialIndex;
	int					m_nSoundIdx;
	CDnItem*			m_pPotentialItem;
	float				m_fElapsedTime;
	bool				m_bIsPotentialApply;
	INT64				m_nCodeSerialNumber;
	INT64				m_nPotentialID;
	INT64				m_nPotentialCharge;

	bool				m_bEnoughCoin;

	CDnPotentialStatsDlg*		m_pDetailStatsDlg;
	CDnPotentialCheckCancelDlg* m_pCheckCancleDlg;

public:
	void SetPotentialItem( CDnItem *pItem );
	void SetPotentialItem( CDnQuickSlotButton *pPressedButton );
	bool IsPotentialableItem( CDnSlotButton *pDragButton );
	void CheckPotentialItem();
	void OnRecvPotentialItem( int nResult, int nSlotIndex );
	void OnRecvPotentialItemRollBack( int nResult, int nSlotIndex );

	void CheckCurrentPotentialState(CDnState* pStats, CEtUITextBox* pTextBox);
	void SetPotentialDescription();
	void Reset();

	const CDnItem*  GetPotentialItem()		 { return m_pPotentialItem; }
	CDnItem*		GetCurrentItem()		 { return m_pItem; }
	
	void			SetPotentialItemID(CDnItem* pItem);
	const INT64	    GetPotentialItemID()	 { return m_nPotentialID;}

	bool IsEnoughMoney();

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void Process( float fElapsedTime );
	virtual void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg = 0 );
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
};

#else // PRE_MOD_POTENTIAL_JEWEL_RENEWAL

class CDnPotentialJewelDlg : public CDnCustomDlg, public CEtUICallback
{
public:
	CDnPotentialJewelDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnPotentialJewelDlg();

protected:
	CDnItem *m_pItem;
	CDnItemSlotButton *m_pItemSlot;
	CEtUIStatic *m_pStaticText;
	CEtUIButton *m_pButtonApply;
	CDnQuickSlotButton *m_pButtonQuickSlot;
	int m_nSoundIdx;

	CDnItem *m_pPotentialItem;
public:
	void SetPotentialItem( CDnQuickSlotButton *pPressedButton );
	bool IsPotentialableItem( CDnSlotButton *pDragButton );
	void CheckPotentialItem();
	void SetPotentialItem( CDnItem *pItem );
	void OnRecvPotentialItem( int nResult, int nSlotIndex );

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
};

#endif // PRE_MOD_POTENTIAL_JEWEL_RENEWAL