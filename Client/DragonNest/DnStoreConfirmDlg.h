#pragma once
#include "DnCustomDlg.h"
#include "DnStoreTabDlg.h"
#include "DnSmartMoveCursor.h"
#include "DnInterface.h"

class CDnItem;
#ifdef PRE_ADD_NPC_REPUTATION_SYSTEM
class CDnRepStoreBenefitTooltipDlg;
#endif // #ifdef PRE_ADD_NPC_REPUTATION_SYSTEM

class CDnStoreConfirmDlg : public CDnCustomDlg
{
public:
	CDnStoreConfirmDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnStoreConfirmDlg(void);

protected:
	CDnItemSlotButton *m_pItemSlotButton;

	CEtUIStatic *m_pItemName;
	CEtUIStatic *m_pItemAmount1;
	CEtUIStatic *m_pItemAmount2;
	CEtUIStatic *m_pItemText;
#ifdef PRE_ADD_NPC_REPUTATION_SYSTEM
	CEtUIStatic *m_pReputationBenefit;
	CDnRepStoreBenefitTooltipDlg* m_pStoreBenefitTooltip;
	int m_iNowStoreBenefit;
#endif // #ifdef PRE_ADD_NPC_REPUTATION_SYSTEM
#ifdef PRE_MOD_SELL_SEALEDITEM
	CEtUIStatic *m_pStaticSellSealedItem;
#endif

	void SetCombinedBuy( CDnItem * pItem, int nTabID, int nCount );
	void SetRepurchaseBuy(const CDnItem& item);

	// 스마트무브
	CDnSmartMoveCursor m_SmartMove;

	CDnInterface::emSTORE_CONFIRM_TYPE m_emConfirmType;

public:
	void SetItem( CDnItem *pItem, CDnInterface::emSTORE_CONFIRM_TYPE type );
	CDnItem *GetItem() { return (CDnItem*)m_pItemSlotButton->GetItem(); }

	CDnInterface::emSTORE_CONFIRM_TYPE GetConfirmType() { return m_emConfirmType; }

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void InitCustomControl( CEtUIControl *pControl );
	virtual void Show( bool bShow );
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
};