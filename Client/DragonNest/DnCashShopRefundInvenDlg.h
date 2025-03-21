#pragma once

#ifdef PRE_ADD_CASHSHOP_REFUND_CL
#include "DnInventoryDlg.h"

class MIInventoryItem;
class CDnInvenSlotDlg;
class CDnCashShopRefundMsgBox;
class CDnCashShopRefundInvenDlg : public CEtUIDialog, public CDnInventoryDlg
{
public:
	CDnCashShopRefundInvenDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnCashShopRefundInvenDlg(void);

	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );

	void SetItem( MIInventoryItem *pItem );
	void ResetSlot( int nSlotIndex );
	void ResetSlot( MIInventoryItem *pItem );
	void SetUseItemCnt( DWORD dwItemCnt );

	void ReleaseNewGain();
	int GetRegisteredItemCount();

	void Withdraw(CDnSlotButton* pPressedBtn);
	void Refund(CDnSlotButton* pPressedBtn);

	std::vector<CDnInvenSlotDlg *>& GetInvenSlotDlgList() { return m_pVecInvenSlotDlg; }

protected:
	void CheckSlotIndex( int nSlotIndex, bool bInsert );
	void RefreshPageControl( bool bCheckMaxPage = false );
	void RefreshCashInvenSlot();

	void NextPage();
	void PrevPage();

	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

private:
	CDnCashShopRefundMsgBox* m_pRefundConfirmMsgBox;
	CEtUIStatic* m_pSlotBG;
	CEtUIButton* m_pButtonPagePrev;
	CEtUIButton* m_pButtonPageNext;
	CEtUIButton* m_pButtonRefund;

	std::vector<CDnInvenSlotDlg *> m_pVecInvenSlotDlg;

	int m_nCurPage;
	int m_nMaxPage;
};

#endif // PRE_ADD_CASHSHOP_REFUND_CL