#pragma once

#include "EtUIDialog.h"

class CDnCashShopMenuDlg : public CEtUIDialog, public CEtUICallback
{
public:
	enum
	{
		MESSAGEBOX_EXITCHATROOM
	};
	CDnCashShopMenuDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnCashShopMenuDlg();

	virtual void	Initialize( bool bShow );
	virtual void	InitialUpdate();
	virtual void	Process( float fElapsedTime );
	virtual void	ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );

	virtual void	Show( bool bShow );

	void			Disable(bool bDisable) { m_bDisable = bDisable; }
	bool			IsEnableButton();

protected:
	virtual void		OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg = 0 );

private:
	CEtUIButton*	m_pBtn;
	bool			m_bDisable;
};