#pragma once
#include "EtUIDialog.h"

class CDnGuildAuthDlg : public CEtUIDialog
{
public:
	CDnGuildAuthDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnGuildAuthDlg(void);

protected:
	CEtUIButton *m_pButtonOK;

	CEtUIComboBox *m_pComboGrade;
	CEtUICheckBox *m_pCheckOption[GUILDAUTH_TYPE_CNT];
	CEtUIComboBox *m_pComboTakeItem;
	CEtUIComboBox *m_pComboWithDraw;

	void InitControl();
	void Apply();

	bool IsChanged();
	DWORD m_dwPrevComboGradeIndex;
	bool m_bShowChangeMsg;

public:
	void RefreshAuth();

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg );
};