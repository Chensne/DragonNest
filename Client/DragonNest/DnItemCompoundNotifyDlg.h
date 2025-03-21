#pragma once
#include "DnCustomDlg.h"
#include "DnItemSlotButton.h"
#include "DnItemCompounder.h"
#include "DnItem.h"


#ifdef PRE_ADD_NOTIFY_ITEM_COMPOUND

class CDnItemCompoundNotifyDlg : public CDnCustomDlg
{
public:					 
	CDnItemCompoundNotifyDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnItemCompoundNotifyDlg(void);
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void Show( bool bShow );
	virtual void OnChangeResolution();

	void SetItemNotify( int nItemIndex , int nOptionIndex );
	void ResetItemNotify();
	void RefreshItemPercentage();
	void OpenCompoundShopFromCompoundIndex( int nCompoundIndex );

	static bool IsMatchedNotifyItem( int nCompoundIndex );
	static int GetCompoundIndex() { return s_nNotifyItemCompoundID; }

protected:

	static int s_nNotifyItemIndex;
	static int s_nNotifyItemCompoundID;

	CEtUIProgressBar *m_pProgressBar;
	CEtUIButton *m_pButtonCover;
	CEtUIStatic *m_pStaticItemName;
	CDnItemSlotButton *m_pItemSlotButton;
	CDnItem *m_pNotifyItem;
};

#endif