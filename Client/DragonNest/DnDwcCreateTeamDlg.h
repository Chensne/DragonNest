#pragma once
#include "EtUIDialog.h"

#if defined(PRE_ADD_DWC)

class CDnDwcCreateTeamDlg : public CEtUIDialog, public CEtUICallback
{
public:
	CDnDwcCreateTeamDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnDwcCreateTeamDlg();

protected:
	CEtUIIMEEditBox*	m_pIMEEditBox;
	CEtUIButton*		m_pInviteButton;

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void Process( float fElapsedTime );
	virtual void Show( bool bShow );
	virtual void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg = 0 );
};

#endif // PRE_ADD_DWC