#pragma once
#include "EtUIDialog.h"

class CDnPartyKickDlg : public CEtUIDialog
{
public:
	CDnPartyKickDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnPartyKickDlg(void);

public:
	virtual void Initialize( bool bShow );
};
