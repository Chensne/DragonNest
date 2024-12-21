#pragma once

#include "DnMessageBox.h"

class CDnMiddleMessageBox : public CDnMessageBox
{
public:
	CDnMiddleMessageBox( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	~CDnMiddleMessageBox();

	virtual void Initialize( bool bShow );
};