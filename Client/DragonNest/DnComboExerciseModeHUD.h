#pragma once

#ifdef PRE_ADD_PVP_COMBOEXERCISE


#include "EtUIDialog.h"
#include "DnPVPBaseHUD.h"

class CDnComboExerciseModeHUD : public CDnPVPBaseHUD
{

public:
	CDnComboExerciseModeHUD( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnComboExerciseModeHUD(void);

protected:
	
	virtual void Process( float fElapsedTime );

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();

public:

	void ShowMaster( bool bMaster );

};


#endif // PRE_ADD_PVP_COMBOEXERCISE