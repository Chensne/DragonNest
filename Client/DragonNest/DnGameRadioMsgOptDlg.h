#pragma once
#include "DnOptionDlg.h"

class CDnGameRadioMsgOptDlg : public CDnOptionDlg
{
public:
	CDnGameRadioMsgOptDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL);
	virtual ~CDnGameRadioMsgOptDlg();

private:
	typedef CDnOptionDlg BaseClass;

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );

public:
	virtual void ExportSetting();
	virtual void ImportSetting();
	virtual bool IsChanged();
};
