#pragma once
#include "EtUIDialog.h"

class CDnStageClearMoveDlg : public CEtUIDialog, public CEtUICallback
{
	enum
	{
		MESSAGEBOX_STAGECLEAR_GO_TOWN,
		MESSAGEBOX_STAGECLEAR_GO_STAGE,
	};
public:
	CDnStageClearMoveDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnStageClearMoveDlg(void);

	virtual void Initialize( bool bShow );
	virtual void Show( bool bShow );

protected:
	virtual void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg /* = 0 */ );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
};