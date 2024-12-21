#pragma once

#include "EtUIDialog.h"

class CDnMarketAboutDlg : public CEtUIDialog
{
	enum{
		GOLD_TAX_UNDER1 = 5,
		GOLD_TAX_OVER1 = 6,
		PETAL_TAX_ID = 12,
	};

public :
	CDnMarketAboutDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnMarketAboutDlg();

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
};