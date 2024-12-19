#pragma once
#include "EtUIDialog.h"

#ifdef PRE_MOD_SELECT_CHAR

class CDnCharLoginTitleDlg : public CEtUIDialog
{
public:
	CDnCharLoginTitleDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnCharLoginTitleDlg();

public:
	virtual void Initialize( bool bShow );	
	void SetTitle( int nTitleString );
};

#endif // PRE_MOD_SELECT_CHAR