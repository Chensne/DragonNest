#pragma once
#include "EtUIDialog.h"

class CDnRebirthFailDlg : public CEtUIDialog
{
public:
	CDnRebirthFailDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnRebirthFailDlg(void);

public:
	virtual void Initialize( bool bShow );

protected:
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
};
