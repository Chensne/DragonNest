#pragma once
#include "EtUIDialog.h"

class CDnRebirthDlg : public CEtUIDialog, public CEtUICallback
{
public:
	CDnRebirthDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnRebirthDlg(void);

protected:
	CEtUIButton *m_pButtonCoin;
	CEtUIStatic *m_pStaticCoin;

public:
	void SetRebirthCoin( int nCoin );

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void Show( bool bShow );
	virtual void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg = 0 );
};
