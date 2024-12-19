#pragma once
#include "DnMoneyInputDlg.h"

class CDnGuildMoneyInputDlg : public CDnMoneyInputDlg
{
public:
	CDnGuildMoneyInputDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnGuildMoneyInputDlg(void);

	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Process( float fElapsedTime );

protected:
	CEtUIStatic *m_pStaticLimitGold;
	CEtUIStatic *m_pStaticLimitSilver;
	CEtUIStatic *m_pStaticLimitBronze;
	CEtUIStatic *m_pStaticLimitBack;
	CEtUIStatic *m_pStaticLimitMaster;
};