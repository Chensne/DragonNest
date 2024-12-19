#include "EtUIDialog.h"

class CDnMasterInformationDlg;
class CDnPupilInformationDlg;

class CDnMasterInfoDlg : public CEtUIDialog, public CEtUICallback
{
public :
	CDnMasterInfoDlg ( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnMasterInfoDlg (void);

protected :

	enum{
		MASTER_RECALL_DIALOG,
		MASTER_DELETE_DIALOG
	};

	CDnMasterInformationDlg * m_pMasterInformationDlg;
	CDnPupilInformationDlg  * m_pPupilInformationDlg;

	CTreeItem * m_pMasterTree;
	CTreeItem * m_pPupilTree;
	bool		m_bHide;

public :

	void RefreshTree();
	void RefreshMyMasterInfo();
	void RefreshClassmateInfo();

public :
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void Show( bool bShow );
	virtual void Process( float fElapsedTime );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg = 0 );
};
