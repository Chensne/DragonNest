#pragma once
#include "DnDungeonEnterDlg.h"

class CDnEventDungeonEnterDlg : public CDnDungeonEnterDlg
{
public:
	CDnEventDungeonEnterDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnEventDungeonEnterDlg();

protected:

protected:
	virtual void InitializeEnterLevelDlg();

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
};