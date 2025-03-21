#pragma once
#include "EtUIDialog.h"

class CDnGuildRoleChangeDlg : public CEtUIDialog
{
public:
	CDnGuildRoleChangeDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnGuildRoleChangeDlg(void);

protected:
	CEtUIButton *m_pButtonOK;
	CEtUIStatic *m_pName;
	CEtUIComboBox *m_pComboBox;

	INT64 m_nCharacterDBID;

	void InitControl();

public:
	void SetGuildMemberDBID( INT64 nDBID ) { m_nCharacterDBID = nDBID; }

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg );
};