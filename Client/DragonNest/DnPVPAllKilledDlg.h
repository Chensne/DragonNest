#pragma once

#include "EtUIDialog.h"

class CDnPvPAllKilledDlg : public CEtUIDialog
{
public:
	CDnPvPAllKilledDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnPvPAllKilledDlg();
	virtual void Initialize( bool bShow );
};