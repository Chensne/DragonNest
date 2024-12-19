#pragma once
#include "EtUIDialog.h"

#if defined(PRE_ADD_DWC)

class CDnDWCCharCreateSelectDlg;
class CDnDWCCharCreateButtonDlg;
#ifdef PRE_MOD_SELECT_CHAR
class CDnCharLoginTitleDlg;
#endif
class CDnDWCCharCreateDlg: public CEtUIDialog, public CEtUICallback
{
public:
	CDnDWCCharCreateDlg(UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL);
	virtual ~CDnDWCCharCreateDlg();

protected:
	CDnDWCCharCreateSelectDlg*	m_pDWCCharSelectDlg;
	CDnDWCCharCreateButtonDlg*	m_pDWCCharButtonDlg;
	CEtUITextureControl*		m_pTextureCtrl;
	CEtUIStatic*				m_pClassTextStatic;
#ifdef PRE_MOD_SELECT_CHAR
	CDnCharLoginTitleDlg*		m_pCreateCharTitleDlg;
#endif // PRE_MOD_SELECT_CHAR

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void Show( bool bShow );
	virtual void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg );

	const INT64 GetCharDBID();
	const int   GetCharJobID();
	void		ChangeCharImage(std::string strFileName, int nClassMidID);
};

#endif // PRE_ADD_DWC