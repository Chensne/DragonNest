#pragma once
#include "EtUIDialog.h"

class CDnChatHelpDlg : public CEtUIDialog
{
public:
	CDnChatHelpDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnChatHelpDlg(void);

protected:
	CEtUITextBox *m_pTextBoxHelp;

public:
	void AddChatHelpText( LPCWSTR wszKey1, LPCWSTR wszKey2, LPCWSTR wszText, int nChatType, DWORD dwColor );

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
};
