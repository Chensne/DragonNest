#pragma once
#include "EtUIDialog.h"

class CDnGameInfoOptDlg : public CEtUIDialog
{
public:
	CDnGameInfoOptDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnGameInfoOptDlg(void);

protected:
	CEtUIButton *m_pButtonPrev;
	CEtUIButton *m_pButtonNext;
	CEtUIStatic *m_pStaticPage;

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
};