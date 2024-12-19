#pragma once
#include "DnCustomDlg.h"

class CDnStoreConfirmDlg;
class CDnStoreConfirmExDlg;

class CDnStoreSlotDlg : public CDnCustomDlg, public CEtUICallback
{
	enum
	{
		STORE_CONFIRM_DIALOG,
		STORE_CONFIRMEX_DIALOG,
	};

public:
	CDnStoreSlotDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnStoreSlotDlg(void);

protected:
	std::vector<CDnItemSlotButton*> m_vecSlotButton;
	CDnItemSlotButton *m_pDragButton;
	CDnItemSlotButton *m_pPressedButton;

	CDnStoreConfirmExDlg *m_pStoreConfirmExDlg;		// Note : 중첩아이템 확인창(개수 입력)
	CDnStoreConfirmDlg *m_pStoreConfirmDlg;			// Note : 중첩이 안되는 아이템의 확인창

public:
	void SetItem( MIInventoryItem *pItem );
	void ResetAllSlot();
	void ResetSlot( int nSlotIndex );
	void ResetRepurchaseSlot(int nSlotIndex);

	int GetSlotStartIndex();

	bool IsEmptySlotDialog();
	bool IsFullSlotDialog();

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void InitCustomControl( CEtUIControl *pControl );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg = 0 );
	virtual void Process( float fElapsedTime );
};
