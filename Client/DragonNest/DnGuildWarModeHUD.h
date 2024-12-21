#pragma once
#include "EtUIDialog.h"
#include "DnPVPBaseHUD.h"

class CDnGuildWarModeHUD : public CDnPVPBaseHUD
{

public:
	CDnGuildWarModeHUD( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnGuildWarModeHUD(void);

protected:

	CEtUIStatic * m_pRedPoint;
	CEtUIStatic * m_pBluePoint;
	CEtUIStatic * m_pResourcePoint;
	CEtUIStatic * m_pTime;

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Process( float fElapsedTime );

	void SetRedPoint( int nPoint, int nTotalScore );
	void SetBluePoint( int nPoint, int nTotalScore );
	void SetResourcePoint( int nPoint ) { m_pResourcePoint->SetIntToText( nPoint ); }
};
