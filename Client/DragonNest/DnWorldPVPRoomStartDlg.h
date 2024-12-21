#pragma once

#include "EtUIDialog.h"

#ifdef PRE_WORLDCOMBINE_PVP

class CDnWorldPVPRoomStartDlg : public CEtUIDialog
{
public:
	CDnWorldPVPRoomStartDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnWorldPVPRoomStartDlg();

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );

	void SetPVPIndex( UINT uiPVPIndex ) { m_uiPVPIndex = uiPVPIndex; }

protected:
	UINT m_uiPVPIndex;
};

#endif // PRE_WORLDCOMBINE_PVP