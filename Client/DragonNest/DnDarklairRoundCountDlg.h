#pragma once

#include "DnCustomDlg.h"
class CDnDarklairRoundCountDlg : public CDnCustomDlg
{
public:
	CDnDarklairRoundCountDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnDarklairRoundCountDlg();

protected:

	enum{
#if defined(PRE_ADD_CHALLENGE_DARKLAIR)
		E_MAX_POSITION = 4,
#else
		E_MAX_POSITION = 2,
#endif	// #if defined(PRE_ADD_CHALLENGE_DARKLAIR)
	};

	CEtUIStatic *m_pStaticRoundCount[E_MAX_POSITION][10];
	CEtUIStatic *m_pStaticRound;
	CEtUIStatic *m_pStaticBossRound;
	CEtUIStatic *m_pStaticFinalRound;
	CEtUIStatic *m_pStaticBonusRound;
	float m_fElapsedTime;

protected:

public:
	// CDnCustomDlg
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void Show( bool bShow );
	virtual void Process( float fElapsedTime );

	void Show( int nRound, bool bBoss, bool bFinal );
};