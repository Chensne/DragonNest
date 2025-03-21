#pragma once
#include "EtUIDialog.h"

class CDnBlockAddDlg : public CEtUIDialog, public CEtUICallback
{
public:
	CDnBlockAddDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnBlockAddDlg(void) {}

protected:
	CEtUIIMEEditBox *m_pNameBox;
	CEtUIButton		*m_pOkButton;
	CEtUIButton		*m_pCancelButton;

	void			Add();

public:
	virtual void	Initialize( bool bShow );
	virtual void	InitialUpdate();
	virtual void	Show( bool bShow );
	virtual void	ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void	Process( float fElapsedTime );
	virtual bool	MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual void	OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg = 0 );
	void			ReturnFocus();
	void			SetUICallbackProcessFlag(bool bEnable);
};
