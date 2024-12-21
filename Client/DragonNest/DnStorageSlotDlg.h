#pragma once
#include "DnCustomDlg.h"

#ifdef PRE_ADD_INVEN_EXTENSION
class CDnStorageExtensionDlg;
#endif

class CDnStorageSlotDlg : public CDnCustomDlg
{
	enum
	{
#ifdef PRE_ADD_INVEN_EXTENSION
		STORAGE_EXTENSION_DIALOG,
#endif
	};

public:
	CDnStorageSlotDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnStorageSlotDlg(void);

protected:
	std::vector<CDnItemSlotButton*> m_vecSlotButton;
	CDnItemSlotButton *m_pDragButton;
	CDnItemSlotButton *m_pPressedButton;

#ifdef PRE_ADD_INVEN_EXTENSION
	CDnStorageExtensionDlg *m_pStorageExtensionDlg;
	int m_nUseItemCnt;
#endif

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
#ifdef PRE_ADD_INVEN_EXTENSION
	virtual void InitialUpdate();
#endif
	virtual void InitCustomControl( CEtUIControl *pControl );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
#ifdef PRE_ADD_INVEN_EXTENSION
	virtual void Show( bool bShow );
	void CheckShowExtensionDlg( bool bShow );
#endif
};
