#pragma once
#include "DnChatRoomCreateDlg.h"

class CDnChatRoomConfigDlg : public CDnChatRoomCreateDlg
{
public:
	CDnChatRoomConfigDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnChatRoomConfigDlg(void);

	void SetInfo( TChatRoomView &View, int nAllow, LPCWSTR pwszPassword );

	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
};