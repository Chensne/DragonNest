#pragma once
#include "EtUIDialog.h"

#ifdef PRE_ADD_CHARSELECT_PREVIEW_CASHITEM

class CDnCharSelectCostumePreviewDlg : public CEtUIDialog
{
public:
	CDnCharSelectCostumePreviewDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnCharSelectCostumePreviewDlg();

protected:
	CEtUIRadioButton* m_pRevertMyCostumeButton;
	CEtUIRadioButton* m_pShowCashCostumeButton;
	int				  m_nLastClickRadioBtnID;

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void Show( bool bShow );
	virtual void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg = 0 );

	void ResetRadioButton();
};

#endif