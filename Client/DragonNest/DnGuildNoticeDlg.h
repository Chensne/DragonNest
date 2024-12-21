#pragma once
#include "EtUIDialog.h"

class CDnGuildNoticeDlg : public CEtUIDialog, public CEtUICallback
{
public:
	CDnGuildNoticeDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnGuildNoticeDlg(void);

protected:
	CEtUIIMEEditBox *m_pEditBoxNotice;
	CEtUIButton *m_pButtonOK;

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg = 0 );
};