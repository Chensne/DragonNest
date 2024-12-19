#pragma once
#include "EtUITabDialog.h"

#if defined( PRE_ALTEIAWORLD_EXPLORE )

class CDnAlteaCheckDlg;
class CDnAlteaInfoDlg;
class CDnAlteaRankDlg;
class CDnAlteaInviteDlg;

class CDnAlteaTabDlg : public CEtUITabDialog
{
public:
	CDnAlteaTabDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnAlteaTabDlg(void);

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void Process( float fElapsedTime );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );

public:
	void SetCount( const int nEnableCount, const int nHoldCount );
	void EnableEnterTime();

	CDnAlteaCheckDlg * GetAlteaCheckDlg() { return m_pAlteaCheckDlg; }
	CDnAlteaRankDlg * GetPersonalRankDlg() { return m_pAlteaPersonalRankDlg; }
	CDnAlteaRankDlg * GetGuildRankDlg() { return m_pAlteaGuildRankDlg; }
	CDnAlteaInviteDlg * GetInviteDlg() { return m_pAlteaInviteDlg; }

protected:
	CEtUIButton * m_pEnter_Button;
	CEtUIButton * m_pCancle_Button;

	CEtUIStatic * m_pEnable_Static;
	CEtUIStatic * m_pHold_Static;

	CEtUIRadioButton * m_pEnter_TabButton;
	CEtUIRadioButton * m_pIntro_TabButton;
	CEtUIRadioButton * m_pPersonalRank_TabButton;
	CEtUIRadioButton * m_pGuildRank_TabButton;
	CEtUIRadioButton * m_pSend_TabButton;

	CDnAlteaCheckDlg * m_pAlteaCheckDlg;
	CDnAlteaInfoDlg * m_pAlteaInfoDlg;
	CDnAlteaRankDlg * m_pAlteaPersonalRankDlg;
	CDnAlteaRankDlg * m_pAlteaGuildRankDlg;
	CDnAlteaInviteDlg * m_pAlteaInviteDlg;
};

#endif	// #if defined( PRE_ALTEIAWORLD_EXPLORE )