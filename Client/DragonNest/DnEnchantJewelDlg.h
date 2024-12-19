#pragma once
#include "DnCustomDlg.h"

class CDnItem;

class CDnEnchantJewelDlg : public CDnCustomDlg
{
public:
	CDnEnchantJewelDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnEnchantJewelDlg();

protected:
	CDnItem *m_pItem;
	CDnItemSlotButton *m_pItemSlot;
	CEtUIStatic *m_pStaticText;
	CEtUIButton *m_pButtonApply;
	CDnQuickSlotButton *m_pButtonQuickSlot;
	int m_nSoundIdx;

	CDnItem *m_pEnchantJewelItem;

public:
	void SetEnchantJewelItem( CDnItem *pItem );
	void OnRecvEnchantJewelItem( int nResult, int nSlotIndex );

	bool CanApplyEnchantJewel( CDnSlotButton *pDragButton );
	void CheckEnchantJewelItem();

	void SetEnchantJewelItem( CDnQuickSlotButton *pPressedButton );

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
};