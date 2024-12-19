#pragma once
#include "DnCustomDlg.h"

#if defined( PRE_ALTEIAWORLD_EXPLORE )

class CDnAlteaInfoDlg : public CDnCustomDlg
{
public:
	CDnAlteaInfoDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnAlteaInfoDlg(void);

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void InitCustomControl(CEtUIControl *pControl);

protected:
	std::vector<CDnItemSlotButton *> m_vecPreviewItemSlotBtn;
};

#endif	// #if defined( PRE_ALTEIAWORLD_EXPLORE )