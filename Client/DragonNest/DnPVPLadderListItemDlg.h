#pragma once
#include "EtUIDialog.h"

class CDnPVPLadderListItemDlg : public CEtUIDialog
{
public:
	CDnPVPLadderListItemDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnPVPLadderListItemDlg(void);

protected:

	CEtUIStatic *m_pStaticClassName;
	CEtUIStatic *m_pStaticWinCount;
	CEtUIStatic *m_pStaticLossCount;
	CEtUIStatic *m_pStaticDrawCount;
	CEtUIStatic *m_pStaticWinPercent;
public:

	void SetClassInfo(TPvPLadderJobScore *JobInfo);
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
};
