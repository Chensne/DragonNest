#include "EtUIDialog.h"

class CDnMasterIntroduceDlg : public CEtUIDialog
{
public :
	CDnMasterIntroduceDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnMasterIntroduceDlg(void);

public :

	void OnRecvIntroduction( bool bRegist );

public :
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void Show( bool bShow );
	virtual void Process( float fElapsedTime );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
};
