#pragma once
#include "EtUIDialog.h"

#ifdef PRE_ADD_DRAGON_GEM

class CDnDragonGemRemoveDlg : public CEtUIDialog 
{
public:
	CDnDragonGemRemoveDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnDragonGemRemoveDlg(void);

	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show(bool bShow);
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual void Process( float fElapsedTime );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );

protected:
};

#endif // PRE_ADD_DRAGON_GEM