#pragma once
#include "EtUIDialog.h"

#ifdef PRE_ADD_VIP_FARM

class CEtUITextBox;

class CDnLifeUnionMarkTooltipDlg : public CEtUIDialog
{
public:
	CDnLifeUnionMarkTooltipDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnLifeUnionMarkTooltipDlg(void);

protected:
	bool MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	CEtUITextBox *m_pContents;

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );

	void SetMarkTooltip();
};

#endif // #ifdef PRE_ADD_VIP_FARM