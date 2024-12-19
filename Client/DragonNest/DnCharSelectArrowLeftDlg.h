#pragma once

#include "EtUIDialog.h"

#ifndef PRE_MOD_SELECT_CHAR

class CDnCharSelectArrowLeftDlg : public CEtUIDialog
{
public:
	CDnCharSelectArrowLeftDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnCharSelectArrowLeftDlg(void);

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
};

#endif // PRE_MOD_SELECT_CHAR