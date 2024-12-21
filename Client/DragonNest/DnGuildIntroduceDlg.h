#pragma once
#include "EtUIDialog.h"

class CDnGuildIntroduceDlg : public CEtUIDialog, public CEtUICallback
{
public:
	CDnGuildIntroduceDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnGuildIntroduceDlg(void);

protected:
	CEtUIIMEEditBox *m_pEditBoxIntroduce;
	CEtUIButton *m_pButtonOK;

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg = 0 );
};