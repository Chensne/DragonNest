#pragma once
#include "EtUIDialog.h"

#if defined(_TH) && defined(_AUTH)

class CDnTHOTPDlg : public CEtUIDialog
{
public:
	CDnTHOTPDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnTHOTPDlg(void);

protected:
	CEtUIEditBox*	m_pEditBoxOTPCode;
	CEtUIButton*	m_pButtonOK;
	CEtUIButton*	m_pButtonCancel;

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void Show( bool bShow );
};

#endif	// _TH && _AUTH