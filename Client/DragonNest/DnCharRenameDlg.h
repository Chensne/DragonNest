#pragma once
#include "EtUIDialog.h"

class CDnGuildRenameConfirmDlg;
class CDnItem;
class CDnCharRenameDlg : public CEtUIDialog, public CEtUICallback
{
public:
	CDnCharRenameDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnCharRenameDlg(void);

protected:
	CEtUIIMEEditBox *m_pEditBox;	
	CEtUIButton *m_pButtonOK;

	CDnItem *m_pItem;

public:
	void EnableControl( bool bEnable );

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg = 0 );

	void SetCharRenameItem( CDnItem *pItem );
};