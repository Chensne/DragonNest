#pragma once
#include "EtUIDialog.h"

class CDnDummyModalDlg : public CEtUIDialog
{
public:
	CDnDummyModalDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_MODAL, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnDummyModalDlg() {}

	virtual void Initialize( bool bShow );
};