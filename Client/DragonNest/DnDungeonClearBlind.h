#pragma once

#include "DnBlindDlg.h"

class CDnDungeonClearBlind : public CDnBlindDlg
{
public:
	CDnDungeonClearBlind( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnDungeonClearBlind(void);

public:
	virtual void Process( float fElapsedTime );
};
