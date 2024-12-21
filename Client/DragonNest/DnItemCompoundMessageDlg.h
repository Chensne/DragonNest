#pragma once
#ifdef PRE_MOD_ITEM_COMPOUND_DIALOG_RENEWAL

#include "DnCustomDlg.h"
#include "DnSmartMoveCursor.h"
#include "DnItemTask.h"
#include "DnItemCompoundProgressDlg.h"
#include "DnCompoundBase.h"

class CDnItemCompoundMessageBox : public CDnCustomDlg
{

public:
	CDnItemCompoundMessageBox( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL, bool bAutoCursor = false );
	virtual ~CDnItemCompoundMessageBox();


protected:
	CEtUIStatic* m_pStaticTitle;
	CDnItemSlotButton* m_pItemSlotButton;
	CEtUIButton* m_ButtonOK;
	CEtUIButton* m_ButtonCancel;
	CDnItem*	 m_pItem;

	CDnSmartMoveCursor m_SmartMove;
	//CDnItemCompoundProgressDlg*		m_pProgressDlg;

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );

	void Reset();
	void SetItemSlot( CDnItem* pItem );
};
#endif 