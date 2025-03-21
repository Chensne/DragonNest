#pragma once

#include "EtUIDialog.h"

#ifdef PRE_ADD_INSTANT_CASH_BUY
class CDnInvenExtensionDlg : public CEtUIDialog, public CEtUICallback
#else // PRE_ADD_INSTANT_CASH_BUY
class CDnInvenExtensionDlg : public CEtUIDialog
#endif // PRE_ADD_INSTANT_CASH_BUY
{
public:
	CDnInvenExtensionDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnInvenExtensionDlg(void);

protected:
	CEtUIButton *m_pButtonCash;

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void Process( float fElapsedTime );
#ifdef PRE_ADD_INSTANT_CASH_BUY
	virtual void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg = 0 );
#endif // PRE_ADD_INSTANT_CASH_BUY
};