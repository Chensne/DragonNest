#pragma once
#include "EtUIDialog.h"
#include "DnPVPBaseHUD.h"

class CDnIndividualRespawnModeHUD : public CDnPVPBaseHUD
{

public:
	CDnIndividualRespawnModeHUD( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnIndividualRespawnModeHUD(void);

protected:

	CEtUIStatic *m_pBestUser;
	CEtUIStatic *m_pBestUserName;
	CEtUIStatic *m_pBestUserCount;
	virtual void Process( float fElapsedTime );

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	void ProcessBestUserName();	
};