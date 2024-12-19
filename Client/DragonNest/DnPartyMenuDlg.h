#pragma once
#include "EtUIDialog.h"

class CEtUIButton;

class CDnPartyMenuDlg : public CEtUIDialog
{
public:
	CDnPartyMenuDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnPartyMenuDlg(void);

protected:
	CEtUIButton *m_pButtonGive;
	CEtUIButton *m_pButtonOut;
	CEtUIButton *m_pButtonPrivate;
	CEtUIButton *m_pButtonFriend;
	CEtUIButton *m_pButtonGuild;

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
};
