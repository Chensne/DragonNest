#pragma once
#include "EtUIDialog.h"

class CDnGRBDlg : public CEtUIDialog
{
public:
	CDnGRBDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnGRBDlg(void);

public:
	virtual void Initialize( bool bShow );
	virtual void Show( bool bShow );
};
