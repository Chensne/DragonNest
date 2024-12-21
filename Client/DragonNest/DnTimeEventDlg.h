#pragma once

#include "DnCustomDlg.h"
#include "DnTimeEventTask.h"
#include "DnItem.h"

class CDnTimeEventListItem;
class CDnTimeEventDlg : public CDnCustomDlg
{
public:
	CDnTimeEventDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnTimeEventDlg();

protected:
	CEtUIListBoxEx *m_pListBoxEx;

protected:

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );

	void RefreshInfo();
	void InvertCheckOthers( CDnTimeEventListItem *pListItem );
};