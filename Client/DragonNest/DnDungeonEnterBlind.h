#pragma once
#include "DnBlindDlg.h"

class CDnDungeonEnterBlind : public CDnBlindDlg
{
public:
	CDnDungeonEnterBlind( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnDungeonEnterBlind(void);

public:
	virtual void Process( float fElapsedTime );
};