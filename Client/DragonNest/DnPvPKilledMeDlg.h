#pragma once

#include "DnCustomDlg.h"

class CDnPvPKilledMeDlg : public CDnCustomDlg
{
public:
	CDnPvPKilledMeDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnPvPKilledMeDlg();

protected:
	CEtUIStatic *m_pStaticEnemy;
	CEtUIStatic *m_pStaticFriendly;
	CEtUIStatic *m_pStaticEnemyBG;
	CEtUIStatic *m_pStaticFriendlyBG;
	CEtUIStatic *m_pStaticKilledBy;
	CEtUIStatic *m_pStaticName;
	CEtUIStatic *m_pStaticCount;

	float m_fDelta;
public:
	// CDnCustonDlg
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void Show( bool bShow );
	virtual void Process( float fElapsedTime );
	virtual void Render( float fElapsedTime );

	void Refresh( WCHAR *wszName, int nKillCount );
};