#pragma once
#include "EtUIDialog.h"

class CDnDungeonSelectDlg : public CEtUIDialog
{
public:
	CDnDungeonSelectDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnDungeonSelectDlg(void);

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
};
