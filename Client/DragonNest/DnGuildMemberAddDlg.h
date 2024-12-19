#pragma once
#include "EtUIDialog.h"

class CDnGuildMemberAddDlg : public CEtUIDialog
{
public:
	CDnGuildMemberAddDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnGuildMemberAddDlg(void);

protected:
	CEtUIIMEEditBox *m_pEditBoxName;
	CEtUIButton *m_pButtonOK;

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
};