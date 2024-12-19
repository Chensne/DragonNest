#pragma once
#include "EtUIDialog.h"
#include "DnClimaxEffect.h"

class CDnOccupationModeClimaxDlg : public CEtUIDialog, public CDnClimaxEffect
{
public:
	CDnOccupationModeClimaxDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnOccupationModeClimaxDlg(void);

	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void Process( float fElapsedTime );

public:
	void StartClimaxEffect();
};
