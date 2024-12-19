#pragma once

#ifdef PRE_ADD_CASHREMOVE

#include "DnCustomControlCommon.h"

class MIInventoryItem;
class CDnInvenSlotDlg;

class CDnInvenStandByCashRemoveDlg : public CEtUIDialog
{
public:
	void SetItem( MIInventoryItem *pItem );
	void ResetSlot( int nSlotIndex );
	void SetSlotType( ITEM_SLOT_TYPE slotType );

	void ReleaseNewGain();
	int GetRegisteredItemCount();

	std::vector<CDnInvenSlotDlg *>& GetInvenSlotDlgList() { return m_pVecInvenSlotDlg; }

protected:
	enum
	{
		NUM_PAGE_PER_GROUP = 5,
	};

	CEtUIButton *m_pButtonPageGroupPrev;
	CEtUIButton *m_pButtonPageGroupNext;
	CEtUIButton *m_pButtonPagePrev;
	CEtUIButton *m_pButtonPageNext;
	CEtUIButton *m_pButtonPage[NUM_PAGE_PER_GROUP];

	std::vector<CDnInvenSlotDlg *> m_pVecInvenSlotDlg;

	int m_nCurPage;
	int m_nMaxPage;

	void CheckSlotIndex( int nSlotIndex, bool bInsert );
	void RefreshPageControl( bool bCheckMaxPage = false );
	void RefreshCashInvenSlot();

	void NextPage();
	void PrevPage();
	void NextGroupPage();
	void PrevGroupPage();

public:
	CDnInvenStandByCashRemoveDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnInvenStandByCashRemoveDlg(void);

	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual void Process( float fElapsedTime );
	virtual void MoveDialog( float fX, float fY );
};


#endif