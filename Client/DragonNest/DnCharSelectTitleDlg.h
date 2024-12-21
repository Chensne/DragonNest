#pragma once
#include "EtUIDialog.h"

#ifndef PRE_MOD_SELECT_CHAR

class CDnCharSelectTitleDlg : public CEtUIDialog
{
public:
	CDnCharSelectTitleDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnCharSelectTitleDlg(void);

public:
	virtual void Initialize( bool bShow );
};

#endif // PRE_MOD_SELECT_CHAR
