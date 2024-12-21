#include "EtUIDialog.h"

class CDnPupilInformationDlg : public CEtUIDialog
{
public :
	CDnPupilInformationDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnPupilInformationDlg(void);

protected :

	CEtUIStatic * m_pStaticMasterName[MasterSystem::Max::MasterCount];

public :

	void RefreshInformation();

public :
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void Show( bool bShow );
	virtual void Process( float fElapsedTime );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
};
