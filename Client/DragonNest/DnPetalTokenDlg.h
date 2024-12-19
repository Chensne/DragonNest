#pragma once
#include "EtUIDialog.h"
#include "DnItem.h"

class CDnItem;
class CDnQuickSlotButton;

class CDnPetalTokenDlg : public CEtUIDialog, public CEtUICallback
{
public:
	CDnPetalTokenDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnPetalTokenDlg();

protected:
	CEtUIStatic *m_pStaticText;
	CDnItem *m_pItem;

public:
	void SetPetalTokenItem( CDnItem *pItem );

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
};
