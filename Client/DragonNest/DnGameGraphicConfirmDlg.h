#pragma once
#include "EtUIDialog.h"
#include "DnSmartMoveCursor.h"

class CDnGameGraphicConfirmDlg : public CEtUIDialog
{
public:
	CDnGameGraphicConfirmDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnGameGraphicConfirmDlg(void);

protected:
	enum
	{
		CONFIRM_TOTAL_TIME = 15,
	};
	float m_fElapsedTime;
	CDnSmartMoveCursor m_SmartMove;

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void Process( float fElapsedTime );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
};