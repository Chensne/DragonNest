#pragma once

#include "EtUITabDialog.h"

class CDnNestInfoDlg : public CEtUIDialog
{
public:
	CDnNestInfoDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnNestInfoDlg(void);

	enum
	{
		NUM_ONOFFMISSION = 3,
	};

protected:
	
	CEtUIListBoxEx *m_pListBoxEx;

	bool GetNestInfo(int nMissionID, int &nActivate, int &uiStringID);

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );

	void RefreshInfo(SCGetPlayerCustomEventUI *nestInfo);

};