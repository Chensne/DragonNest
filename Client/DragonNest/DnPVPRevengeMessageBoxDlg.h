#pragma once

#include "EtUIDialog.h"

class CDnPVPRevengeMessageBoxDlg : public CEtUIDialog
{
public:
	CDnPVPRevengeMessageBoxDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnPVPRevengeMessageBoxDlg(void);

protected:
	CEtUIStatic * m_pInviteStatic;
	CEtUIStatic * m_pMessageStatic;

public :
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );

public:
	void SetMessageBox( const std::wstring szRevengeRequestUser );
};