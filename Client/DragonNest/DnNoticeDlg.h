#pragma once
#include "EtUIDialog.h"

class CEtUIHtmlTextBox;

class CDnNoticeDlg : public CEtUIDialog
{
public:
	CDnNoticeDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnNoticeDlg(void);

protected:
	CEtUIHtmlTextBox *m_pTextBox;

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
};