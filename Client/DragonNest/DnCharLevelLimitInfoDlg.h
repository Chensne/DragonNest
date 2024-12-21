#pragma once
#include "EtUIDialog.h"


#ifdef PRE_MOD_SELECT_CHAR

class CDnCharLevelLimitInfoDlg : public CEtUIDialog
{
public:
	CDnCharLevelLimitInfoDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnCharLevelLimitInfoDlg();

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();

public:
	void SetLevelLimitCount( int nLevelLimitCount );
};

#endif // PRE_MOD_SELECT_CHAR