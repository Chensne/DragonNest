#pragma once
#include "EtUIDialog.h"

class CDnPVPLadderInviteConfirmDlg : public CEtUIDialog
{
public:
	CDnPVPLadderInviteConfirmDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnPVPLadderInviteConfirmDlg(void);

protected:

	CEtUIProgressBar *m_pProgressBarTime;
	CEtUIStatic *m_pStaticName;
	CEtUIStatic *m_pStaticPoint;
	CEtUIStatic *m_pStaticNumber;
	CEtUIButton *m_pButtonOK;
	CEtUIButton *m_pButtonCancel;
	std::wstring m_wszName;

	float m_fTotalTime;
	float m_fElapsedTime;

public:
	void SetElapsedTime( float fElapsedTime );
	void SetInviteInfo(LadderSystem::SC_INVITE_CONFIRM_REQ *pData);

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void Process( float fElapsedTime );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
};
