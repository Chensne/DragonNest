#pragma once
#include "DnCaptionExDlg.h"

class CDnCaptionDlg_07 : public CDnCaptionExDlg
{
public:
	CDnCaptionDlg_07( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnCaptionDlg_07(void);

public:
	virtual void Initialize( bool bShow );
};