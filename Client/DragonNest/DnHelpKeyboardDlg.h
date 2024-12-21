#pragma once
#include "EtUITabDialog.h"

class CDnHelpKeyboardTabDlg;
class CDnHelpKeyboardDlg : public CEtUITabDialog
{
public:
	CDnHelpKeyboardDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnHelpKeyboardDlg();

protected:
	bool m_bJoypadPush;
	CEtUIRadioButton *m_pNormalTab;
	CEtUIRadioButton *m_pBattleTab;

	CDnHelpKeyboardTabDlg *m_pNormalTabDlg;
	CDnHelpKeyboardTabDlg *m_pBattleTabDlg;
public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	void SetJoypadPush( bool bPush )	{ m_bJoypadPush = bPush; }
};