#pragma once
#include "EtUIDialog.h"

#ifdef PRE_MOD_APPELLATIONBOOK_RENEWAL

class CDnAppellationRewardCoinTooltipDlg : public CEtUIDialog
{
public:
	CDnAppellationRewardCoinTooltipDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnAppellationRewardCoinTooltipDlg(void);

protected:
	CEtUIStatic *m_pStaticGold;
	CEtUIStatic *m_pStaticSilver;
	CEtUIStatic *m_pStaticBronze;

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();

	void SetRewardCoin( int nRewardCoin );
};

#endif