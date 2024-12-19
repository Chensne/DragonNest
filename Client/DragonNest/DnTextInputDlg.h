#pragma once
#include "EtUIDialog.h"

class CDnTextInputDlg : public CEtUIDialog
{
public:
	CDnTextInputDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnTextInputDlg(void);

protected:
	CEtUITextBox *m_pTextBoxText;
	CEtUIIMEEditBox *m_pEditBoxText;

public:
	void SetText( LPCWSTR wszText );
	LPCWSTR GetText();

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
};