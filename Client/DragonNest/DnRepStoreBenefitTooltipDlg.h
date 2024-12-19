#pragma once
#include "EtUIDialog.h"

#ifdef PRE_ADD_NPC_REPUTATION_SYSTEM

class CEtUITextBox;

class CDnRepStoreBenefitTooltipDlg : public CEtUIDialog
{
public:
	CDnRepStoreBenefitTooltipDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnRepStoreBenefitTooltipDlg(void);

protected:
	CEtUITextBox *m_pTBRepStoreBenefit;

public:
	void SetText( const wchar_t* pText );

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
};

#endif // #ifdef PRE_ADD_NPC_REPUTATION_SYSTEM