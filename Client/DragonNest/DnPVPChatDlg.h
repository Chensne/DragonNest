#pragma once

#include "DnChatDlg.h"


class CDnPVPChatDlg : public CDnChatDlg
{
public:
	CDnPVPChatDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnPVPChatDlg(void);

public:
	virtual void Initialize( bool bShow );
	//virtual void InitialUpdate();
};