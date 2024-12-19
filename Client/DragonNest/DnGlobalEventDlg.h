#pragma once
#include "EtUIDialog.h"

class CDnGlobalEventDlg : public CEtUIDialog
{
public:
	CDnGlobalEventDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnGlobalEventDlg(void);

protected:

	CEtUIStatic *m_pStaticCount;
	CEtUIStatic *m_pStaticPercent;
	CEtUIProgressBar *m_pProgressBar;
	
public:

	void SetEventInfo(SCScorePeriodQuest* pInfo);

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
};