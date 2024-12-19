#pragma once
#include "EtUIDialog.h"

class CDnGateQuestionDlg : public CEtUIDialog
{
public:
	CDnGateQuestionDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnGateQuestionDlg(void);

protected:
	CEtUIButton *m_pButtonStart;
	CEtUIButton *m_pButtonCancel;
	CEtUIButton* m_pButtonPartyDlgOpen;
	CEtUIStatic *m_pStaticTitle;

protected:
	void EnableButtons();

public:
	void SetTitleName( const wchar_t *wszTitle );

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void Process( float fElapsedTime );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
};
