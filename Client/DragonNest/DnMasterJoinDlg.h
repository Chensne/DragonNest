#include "EtUIDialog.h"

class CDnMasterJoinDlg : public CEtUIDialog
{
public :
	CDnMasterJoinDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnMasterJoinDlg(void);

protected :

	CEtUIListBoxEx *	m_pListBoxEx;
	int					m_nCurSelect;

public :

	void RefreshInfo( int nCurSelect );

public :
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void Show( bool bShow );
	virtual void Process( float fElapsedTime );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
};
