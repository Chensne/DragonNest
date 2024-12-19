
#pragma once

#include "EtUIDialog.h"

class CEtUITextBox;

class CDnGuildWarMarkTooltipDlg : public CEtUIDialog
{
public:
	CDnGuildWarMarkTooltipDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnGuildWarMarkTooltipDlg(void);

protected:
	bool MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	CEtUITextBox *m_pContents;

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );

	bool SetMarkTooltip();
	bool AdjustPosition(float orgX, float orgY);
};