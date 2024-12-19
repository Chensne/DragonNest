#pragma once

#if defined(PRE_ADD_DWC)

#include "EtUIDialog.h"
class CEtUITextBox;
class CDnDWCMarkTooltipDlg : public CEtUIDialog
{
public:
	CDnDWCMarkTooltipDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnDWCMarkTooltipDlg();

protected:
	CEtUITextBox *m_pContents;

	virtual void InitialUpdate();
	
	bool MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
	virtual void Initialize( bool bShow );
	bool SetMarkTooltip();
	bool AdjustPosition(float orgX, float orgY);
};

#endif // PRE_ADD_DWC