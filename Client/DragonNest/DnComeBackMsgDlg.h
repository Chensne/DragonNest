
#ifdef PRE_ADD_NEWCOMEBACK

#include "EtUIDialog.h"


class CDnComeBackMsgDlg : public CEtUIDialog
{

public:


	CDnComeBackMsgDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL, bool bAutoCursor = false );
	virtual ~CDnComeBackMsgDlg(){}

	// Override - CEtUIDialog //
	virtual void Initialize( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl * pControl, UINT uMsg = 0 );		
	
};

#endif // PRE_ADD_NEWCOMEBACK
