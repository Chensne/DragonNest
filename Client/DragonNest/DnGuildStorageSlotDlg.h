#pragma once
#include "DnCustomDlg.h"

class CDnGuildStorageSlotDlg : public CDnCustomDlg
{
public:
	CDnGuildStorageSlotDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnGuildStorageSlotDlg(void);

protected:
	std::vector<CDnItemSlotButton*> m_vecSlotButton;
	CDnItemSlotButton *m_pDragButton;
	CDnItemSlotButton *m_pPressedButton;

protected:
	void RequestMoveItem( CDnSlotButton *pDragButton, CDnItemSlotButton *pPressedButton );

public:
	void SetItem( MIInventoryItem *pItem );
	void ResetSlot( int nSlotIndex );
	void SetUseItemCnt( DWORD dwItemCnt );
	int GetSlotStartIndex();

	bool IsEmptySlot();
	int GetEmptySlot();

public:
	virtual void Initialize( bool bShow );
	virtual void InitCustomControl( CEtUIControl *pControl );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
};