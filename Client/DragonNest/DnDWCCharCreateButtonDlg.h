#pragma once
#include "EtUIDialog.h"

#if defined(PRE_ADD_DWC)
class CDnDWCCharCreateDlg;
class CDnDWCCharCreateButtonDlg: public CEtUIDialog, public CEtUICallback
{
public:
	CDnDWCCharCreateButtonDlg(UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL);
	virtual ~CDnDWCCharCreateButtonDlg();

protected:
	CDnDWCCharCreateDlg* pParentDlg;

public:
	virtual void Initialize( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg );
};

#endif // PRE_ADD_DWC