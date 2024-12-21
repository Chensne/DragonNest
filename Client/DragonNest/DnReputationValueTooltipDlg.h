#pragma once
#include "EtUIDialog.h"

#ifdef PRE_ADD_NPC_REPUTATION_SYSTEM

class CEtUITextBox;

class CDnReputationValueTooltipDlg : public CEtUIDialog
{
public:
	CDnReputationValueTooltipDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnReputationValueTooltipDlg(void);

protected:
	CEtUITextBox *m_pTBReputationValue;

public:
	void SetReputationPercent( float fCurReputationValue, float fMaxReputationValue );

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
};

#endif // #ifdef PRE_ADD_NPC_REPUTATION_SYSTEM