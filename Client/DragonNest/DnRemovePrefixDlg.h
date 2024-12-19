#pragma once
#include "DnCustomDlg.h"

class CDnItem;

class CDnRemovePrefixDlg : public CDnCustomDlg
{
public:
	CDnRemovePrefixDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnRemovePrefixDlg();

protected:
	CDnItem *m_pItem;
	CDnItemSlotButton *m_pItemSlot;
	CEtUIStatic *m_pStaticText;
	CEtUIButton *m_pButtonApply;
	CDnQuickSlotButton *m_pButtonQuickSlot;
	int m_nSoundIdx;

	CDnItem *m_pPrefixItem;

public:
	void SetItem( CDnItem *pItem );
	void OnRecvRemovePrefixItem( int nResult, int nSlotIndex );

	bool CanApplyRemovePrefix( CDnSlotButton *pDragButton );
	void CheckRemovePrefixItem();

	void SetItem( CDnQuickSlotButton *pPressedButton );

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
};