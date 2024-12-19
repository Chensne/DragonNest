#pragma once
#include "DnOptionTabDlg.h"

class CDnGameViewOptDlg;
class CDnGameQuickMsgOptDlg;
class CDnGameCommOptDlg;
class CDnGameMovieOptDlg;

class CDnGameOptDlg : public CDnOptionTabDlg, public CEtUICallback
{
	enum
	{
		GAMEOPTION_VIEW_DIALOG,
		GAMEOPTION_MACRO_DIALOG,
		GAMEOPTION_COMM_DIALOG,	
		GAMEOPTION_MOVIE_DIALOG,
	};

public:
	CDnGameOptDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnGameOptDlg(void);

protected:
	CEtUIRadioButton *m_pTabButtonView;
	CEtUIRadioButton *m_pTabButtonMacro;
	CEtUIRadioButton *m_pTabButtonComm;
	CEtUIRadioButton *m_pTabButtonMovie;

	CDnGameViewOptDlg *m_pGameViewOptDlg;
	CDnGameQuickMsgOptDlg *m_pGameQuickMsgOptDlg;
	CDnGameCommOptDlg *m_pGameCommOptDlg;
	CDnGameMovieOptDlg *m_pGameMovieOptDlg;

	/*int m_nPrevTabID;
	bool m_bShowChangeMsg;*/

	typedef CDnOptionTabDlg BaseClass;

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	/*virtual void Show( bool bShow );*/
	/*virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );*/
	virtual void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg = 0 );
};
