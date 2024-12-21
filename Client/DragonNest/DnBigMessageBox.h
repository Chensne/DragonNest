#pragma once

#include "DnMessageBox.h"

class CDnBigMessageBox : public CDnMessageBox
{
public:
	CDnBigMessageBox( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	~CDnBigMessageBox();

	virtual void Initialize( bool bShow );
};