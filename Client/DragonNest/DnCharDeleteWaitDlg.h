#pragma once

#include "EtUIDialog.h"

class CDnCharDeleteWaitDlg : public CEtUIDialog, public CEtUICallback
{
public:
	CDnCharDeleteWaitDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnCharDeleteWaitDlg(void);

protected:
	CEtUIStatic *m_pStaticDeleteRemainTime;
	CEtUIButton *m_pButtonDeleteCancel;
	__time64_t m_ExpectTime;
	CEtUIStatic *m_pStaticSlotBlock;

protected:
	void UpdateDeleteRemainTime();

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void Process( float fElapsedTime );
	virtual void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg = 0 );
	virtual void Show( bool bShow );

public:
	void SetRemainTime( __time64_t RemainTime );
	void ShowSlotBlock( bool bShow );
};
