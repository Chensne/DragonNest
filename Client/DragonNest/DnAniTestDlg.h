#pragma once

#include "EtUIDialog.h"

#ifdef PRE_TEST_ANIMATION_UI

class CDnAniTestDlg : public CEtUIDialog
{
public:
	CDnAniTestDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnAniTestDlg(void);

protected:
	CEtUIAnimation* m_pTestAni;

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void Show(bool bShow);
};

#endif