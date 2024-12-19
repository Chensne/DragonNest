#include "EtUIDialog.h"

class CDnPupilInListDlg : public CEtUIDialog
{
public :
	CDnPupilInListDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnPupilInListDlg(void);

public :

	void SetInfo( const TPupilInfoWithLocation & RefData );

public :
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void Show( bool bShow );
	virtual void Process( float fElapsedTime );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
};
