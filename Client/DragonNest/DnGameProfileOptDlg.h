#pragma once
#include "EtUIDialog.h"

class CDnGameProfileOptDlg : public CEtUIDialog, CEtUICallback
{
public:
	CDnGameProfileOptDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnGameProfileOptDlg(void);

protected:
	CEtUICheckBox *m_pCheckMale;
	CEtUICheckBox *m_pCheckFemale;
	CEtUILineIMEEditBox *m_pEditBoxPR;
	CEtUICheckBox *m_pCheckOpen;

public:
	void SetProfile( TProfile *pProfile );

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg = 0 );
};
