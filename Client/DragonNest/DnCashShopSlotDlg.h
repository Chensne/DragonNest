#pragma once
#include "DnCustomDlg.h"

//#define _CASHSHOP_UI
#ifdef _CASHSHOP_UI

class CDnCashShopSlotDlg : public CDnCustomDlg, public CEtUICallback
{
public:
	CDnCashShopSlotDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnCashShopSlotDlg(void);

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void InitCustomControl( CEtUIControl *pControl );
};

#endif // _CASHSHOP_UI