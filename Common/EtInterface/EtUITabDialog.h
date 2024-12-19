#pragma once

#include "EtUIDialog.h"
#include "EtUIDialogGroup.h"

class CEtUIControl;

class CEtUITabDialog : public CEtUIDialog
{
public:
	CEtUITabDialog( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL, bool bAutoCursor = false );
	virtual ~CEtUITabDialog(void);

protected:
	std::vector< std::pair<DWORD, CEtUIControl*> >  m_vecTabControl;
	CEtUIDialogGroup m_groupTabDialog;

protected:
	void AddTabDialog( CEtUIControl *pControl, CEtUIDialog *pDialog );
	void ChangeDialog( CEtUIControl *pControl, CEtUIDialog *pDialog );
	

public:
	void SetCheckedTab( DWORD dwControlID );
	DWORD					GetCurrentTabID();
	const CEtUIRadioButton*	GetCurrentTabControl() const;
	CEtUIRadioButton*		GetTabControl(DWORD dwTabID);
	int GetFrontTabID();

public:
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );	
	virtual void MoveDialog( float fX, float fY );
};
