#pragma once

#include "EtUIDialog.h"

class CDnMainBarMenuDlg : public CEtUIDialog
{
public:
	CDnMainBarMenuDlg(UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL);
	virtual ~CDnMainBarMenuDlg(void);

protected:
	CDnMainMenuDlg *pMainMenuDlg;

	CEtUIStatic* m_pButtonSystem;
	CEtUIStatic* m_pButtonHelp;
	CEtUIStatic* m_pButtonArenaRank;
	CEtUIStatic* m_pButtonArena;
	CEtUIStatic* m_pButtonEvent;
	CEtUIStatic* m_pButtonChat;
	CEtUIStatic* m_pButtonMap;
	bool isShow;
	bool bShowTrigger;
public:
	virtual void InitialUpdate();
	virtual void Initialize(bool bShow);
	virtual void Show(bool bShow);
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );

	void SetButtonsHotKeys(int nUIWrappingKeyIndex, BYTE cVK);
};
