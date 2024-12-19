#pragma once
#include "DnOptionTabDlg.h"

class CDnGameRadioMsgOptDlg;
class CDnGameMacroMsgOptDlg;

class CDnGameQuickMsgOptDlg : public CDnOptionTabDlg
{
public:
	enum
	{
		RADIOMSG_OPTION_DIALOG = 0,
		MACROMSG_OPTION_DIALOG,
	};

public:
	CDnGameQuickMsgOptDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnGameQuickMsgOptDlg();

private:
	typedef CDnOptionTabDlg BaseClass;

protected:
	CEtUIRadioButton *m_pTabButtonRadioMsg;
	CEtUIRadioButton *m_pTabButtonMacroMsg;

	CDnGameRadioMsgOptDlg *m_pGameRadioMsgOptDlg;
	CDnGameMacroMsgOptDlg *m_pGameMacroMsgOptDlg;

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
};