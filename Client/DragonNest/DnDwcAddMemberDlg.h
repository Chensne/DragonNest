#pragma once
#include "EtUIDialog.h"

#if defined(PRE_ADD_DWC)

class CDnDwcAddMemberDlg : public CEtUIDialog, public CEtUICallback
{
public:
	CDnDwcAddMemberDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL, bool bAutoCursor = false );
	virtual ~CDnDwcAddMemberDlg();
protected:
	CEtUIComboBox*		m_pComboBox;
	CEtUIIMEEditBox*	m_pIMEEditBox;
	CEtUIButton*		m_pInviteButton;
public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void Process( float fElapsedTime );
	virtual void Show( bool bShow );
};

#endif // PRE_ADD_DWC