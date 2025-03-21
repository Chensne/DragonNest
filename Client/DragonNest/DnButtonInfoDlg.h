#pragma once
#include "EtUIDialog.h"

class CEtUITextBox;

class CDnButtonInfoDlg : public CEtUIDialog
{
public:
	CDnButtonInfoDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnButtonInfoDlg(void);

protected:
	CEtUITextBox *m_pTextBoxInfo;

public:
	void SetInfo( LPCWSTR szwInfo, LPCWSTR szwKey );

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
};