#pragma once

#include "EtUIDialog.h"

class CDnMiniPlayerGuildWarTabDlg : public CEtUIDialog
{
public:
	CDnMiniPlayerGuildWarTabDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnMiniPlayerGuildWarTabDlg();

protected:
	CEtUIStatic * m_pPartyName;

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );

	void SetPartyName( int nParytNumber );
};
