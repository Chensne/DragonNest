#pragma once
#include "EtUIDialog.h"
#include "DnSmartMoveCursor.h"

class CDnGuildMarkInfoDlg : public CEtUIDialog
{
public:
	CDnGuildMarkInfoDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnGuildMarkInfoDlg(void);

protected:
	CDnSmartMoveCursor m_SmartMove;

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg );
};