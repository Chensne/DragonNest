
#pragma once

#ifdef PRE_ADD_SERVER_WAREHOUSE

#include "DnCustomDlg.h"

class CDnWorldServerStorageSlotDlg : public CDnCustomDlg
{
	enum
	{
#ifdef PRE_ADD_INVEN_EXTENSION
		STORAGE_EXTENSION_DIALOG,
#endif
	};

public:
	CDnWorldServerStorageSlotDlg(UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL);
	virtual ~CDnWorldServerStorageSlotDlg(void);

	void SetItem(MIInventoryItem *pItem);
	void ResetSlot(int nSlotIndex);
	void SetUseItemCnt(DWORD dwItemCnt);
	int GetSlotStartIndex();
	void SetSlotType(ITEM_SLOT_TYPE type);

	bool IsEmptySlot();
	int GetEmptySlot();

	virtual void Initialize( bool bShow );
	virtual void InitCustomControl( CEtUIControl *pControl );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );

protected:
	void RequestMoveItem(CDnSlotButton *pDragButton, CDnItemSlotButton *pPressedButton);

	std::vector<CDnItemSlotButton*> m_vecSlotButton;
	CDnItemSlotButton *m_pDragButton;
	CDnItemSlotButton *m_pPressedButton;
};

#endif // PRE_ADD_ACCOUNT_STORAGE