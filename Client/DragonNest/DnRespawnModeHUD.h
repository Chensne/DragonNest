#pragma once
#include "EtUIDialog.h"
#include "DnPVPBaseHUD.h"

class CDnRespawnModeHUD : public CDnPVPBaseHUD
{

protected:

#ifdef PRE_MOD_PVPOBSERVER
	bool m_bFirstTeam;
#endif // #ifdef PRE_MOD_PVPOBSERVER


public:
	CDnRespawnModeHUD( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnRespawnModeHUD(void);

protected:

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();

#ifdef PRE_MOD_PVPOBSERVER
	virtual void Process( float fElapsedTime );
	void GameStart();
#endif // PRE_MOD_PVPOBSERVER

};