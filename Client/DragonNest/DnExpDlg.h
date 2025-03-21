#pragma once
#include "EtUIDialog.h"

class CEtUITextBox;

class CDnExpDlg : public CEtUIDialog
{
public:
	CDnExpDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnExpDlg(void);

protected:
	CEtUITextBox *m_pTextBoxExp;

public:
	void SetExp( int nCurExp, int nMaxExp );

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
};
