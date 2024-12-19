#pragma once
#include "EtUIDialog.h"

class CDnQuestPathInfoDlg : public CEtUIDialog
{
public:
	CDnQuestPathInfoDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnQuestPathInfoDlg(void);

public:
	virtual void InitialUpdate();
	virtual void Initialize(bool bShow);
};
