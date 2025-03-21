#pragma once
#include "DnCustomDlg.h"

#ifdef PRE_MOD_POTENTIAL_JEWEL_RENEWAL

class CDnPotentialJewelDlg;
class CDnPotentialCheckCancelDlg : public CEtUIDialog, public CEtUICallback
{
public:
	CDnPotentialCheckCancelDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnPotentialCheckCancelDlg();

public:
	CEtUIStatic* m_pInfoText;

	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void Process( float fElapsedTime );
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	void SetMoney(int nOnePrice );
};

#endif //PRE_MOD_POTENTIAL_JEWEL_RENEWAL