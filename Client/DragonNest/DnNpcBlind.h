#pragma once

#include "DnBlindDlg.h"

class CDnNpcBlind : public CDnBlindDlg
{
public:
	CDnNpcBlind( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnNpcBlind(void);

public:
	virtual void Process( float fElapsedTime );
};
