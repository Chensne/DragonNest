#pragma once
#include "EtUIDialog.h"

class CDnGuildDismissDlg : public CEtUIDialog
{
public:
	CDnGuildDismissDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnGuildDismissDlg(void);

protected:
	CEtUIButton *m_pButtonYes;

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
};