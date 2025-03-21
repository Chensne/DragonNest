#include "EtUIDialog.h"

class CDnMasterIntroduceDlg;

class CDnPupilListDlg : public CEtUIDialog, public CEtUICallback
{
public :
	CDnPupilListDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnPupilListDlg(void);

protected :

	enum{
		INTRODUCTION_DIALOG,
		PUPIL_DELETE_DIALOG,
	};

	CEtUIListBoxEx *		m_pListBoxEx;
	CEtUIButton *			m_pTitleButton;
	CEtUIButton *			m_pChatButton;
	CEtUIButton *			m_pJoinButton;
	CEtUIButton *			m_pQuitButton;
	CDnMasterIntroduceDlg * m_pMasterIntroduceDlg;

	bool m_bTitle;

	void IsIntroduction();

public :

	void RefreshIntroduction();
	void RefreshPupilList();
	void RefreshLeave();
	void RefreshRespectPoint();

public :
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void Show( bool bShow );
	virtual void Process( float fElapsedTime );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg = 0 );
};
