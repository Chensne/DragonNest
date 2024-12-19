#pragma once
#include "EtUIDialog.h"

class CDnMissionRewardCoinTooltipDlg : public CEtUIDialog
{
public:
	CDnMissionRewardCoinTooltipDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnMissionRewardCoinTooltipDlg(void);

protected:
	CEtUIStatic *m_pStaticGold;
	CEtUIStatic *m_pStaticSilver;
	CEtUIStatic *m_pStaticBronze;

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();

	void SetRewardCoin( int nRewardCoin );
};