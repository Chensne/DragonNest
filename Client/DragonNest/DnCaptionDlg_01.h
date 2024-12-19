#pragma once
#include "DnCaptionDlg.h"

class CDnCaptionDlg_01 : public CDnCaptionDlg
{
public:
	CDnCaptionDlg_01( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnCaptionDlg_01(void);

public:
	virtual void Initialize( bool bShow );
};