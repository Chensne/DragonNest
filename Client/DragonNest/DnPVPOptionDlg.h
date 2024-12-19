#pragma once
#include "EtUIDialog.h"

class CDnPVPOptionDlg : public CEtUIDialog
{
public:
	CDnPVPOptionDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnPVPOptionDlg(void);

protected:
	CEtUICheckBox * m_pCheckBoxPvPFatigue;
	float m_fEnableTime;
	CEtUICheckBox * m_pCheckBoxSkillPrimary;
	CEtUICheckBox * m_pCheckBoxSkillSecondary;

public:	

	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );	
	virtual void Show( bool bShow );
	virtual void Process( float fElapsedTime );

	void EnalbeFatigue(bool bTrue);

	void ProcessEnableTime( float fElapsedTime );
};

