#pragma once
#include "EtUIDialog.h"

class CDnGuildYesNoDlg : public CEtUIDialog
{
public:
	CDnGuildYesNoDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnGuildYesNoDlg(void);

	enum eGuildYesNoType
	{
		GuildYesNo_None = -1,
		GuildYesNo_Dismiss,
		GuildYesNo_Leave,
		GuildYesNo_GuildWarEnroll,
	};

protected:
	CEtUIStatic *m_pTitle;
	CEtUIStatic *m_pMessage;

	eGuildYesNoType m_eType;

public:
	void SetGuildYesNoType( eGuildYesNoType eType );

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
};