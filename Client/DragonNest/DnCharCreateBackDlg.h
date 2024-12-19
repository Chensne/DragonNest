#pragma once
#include "EtUIDialog.h"

#ifdef PRE_MOD_SELECT_CHAR
class CDnCharCreateBackDlg : public CEtUIDialog, public CEtUICallback
#else // PRE_MOD_SELECT_CHAR
class CDnCharCreateBackDlg : public CEtUIDialog
#endif // PRE_MOD_SELECT_CHAR
{
public:
	CDnCharCreateBackDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnCharCreateBackDlg(void);

protected:
	CEtUIButton *m_pButtonBack;

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void Show( bool bShow );
#ifdef PRE_MOD_SELECT_CHAR
	virtual void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg );
#endif // PRE_MOD_SELECT_CHAR
};
