#pragma once
#include "DnPassWordInputDlg.h"

class CDnChatRoomPassWordDlg : public CDnPassWordInputDlg
{
public:
	CDnChatRoomPassWordDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnChatRoomPassWordDlg(void);
};